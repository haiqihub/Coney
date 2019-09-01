/*************************************************************************//**
 *****************************************************************************
 * @file   mllink.c
 * @author JSON
 * @version 3.0
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

PRIVATE int delete(struct inode *pin, struct inode *dir_inode);
/*****************************************************************************
 *                                do_unmllink
 *****************************************************************************/
/**
 * Remove a file.
 *
 * @note We clear the i-node in inode_array[] although it is not really needed.
 *       We don't clear the data bytes so the file is recoverable.
 * 
 * @return On success, zero is returned.  On error, -1 is returned.
 *****************************************************************************/
PUBLIC int do_unmllink()
{
	char pathname[MAX_PATH];
	char filename[MAX_PATH];

	/* get parameters from the message */
	int name_len = fs_msg.NAME_LEN;	/* length of filename */
	int src = fs_msg.source;	/* caller proc nr. */
	assert(name_len < MAX_PATH);
	phys_copy((void*)va2la(TASK_FS, pathname),
		  (void*)va2la(src, fs_msg.PATHNAME),
		  name_len);
	pathname[name_len] = 0;
	//select_console(0);

	if (strcmp(pathname , "/") == 0) {
		printl("{FS} FS:do_unlink():: cannot unlink the root\n");
		return -1;
	}

	struct inode *dir_node = 0;

	if (name_len > 0){
		if(pathname[0] == '/') {
			dir_node = root_inode;
		}
		else{
			dir_node = (struct inode *)fs_msg.NODE;
		}
	}
	else return INVALID_FORM;
	

	if (split_dir_file(&dir_node, pathname, filename) == INVALID_PATH){
		return INVALID_PATH;
	}
	if(filename[0] == '\0'){
		return INVALID_FORM;
	}
	
	int inode_nr = get_inode_nr(dir_node, filename);
	if(inode_nr == INVALID_INODE){
		return FILENONEXIST;
	}	

	struct inode * pin = get_inode(dir_node->i_dev, inode_nr);

	if (pin->i_mode != I_REGULAR && pin->i_mode != I_DIRECTORY) { 
		/* can only remove regular files */
		printl("{FS} cannot remove file %s, because "
		       "it is not a regular file.\n",
		       pathname);
		return UNREMOVE;
	}
	int restype = delete(pin, dir_node);
	
	if(restype != REGULAR){
		return restype;
	}
	/************************************************/
	/* set the inode-nr to 0 in the directory entry */
	/************************************************/
	int dir_blk0_nr = dir_node->i_start_sect;
	int nr_dir_blks = (dir_node->i_size + SECTOR_SIZE) / SECTOR_SIZE;
	int nr_dir_entries =
		dir_node->i_size / DIR_ENTRY_SIZE; /* including unused slots
						     * (the file has been
						     * deleted but the slot
						     * is still there)
						     */
	int m = 0;
	struct dir_entry * pde = 0;
	int flg = 0;
	int dir_size = 0;

	for (int i = 0; i < nr_dir_blks; i++) {
		RD_SECT(dir_node->i_dev, dir_blk0_nr + i);

		pde = (struct dir_entry *)fsbuf;
		int j;
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) {
			if (++m > nr_dir_entries)
				break;

			if (pde->inode_nr == inode_nr) {
				/* pde->inode_nr = 0; */
				memset(pde, 0, DIR_ENTRY_SIZE);
				WR_SECT(dir_node->i_dev, dir_blk0_nr + i);
				flg = 1;
				break;
			}

			if (pde->inode_nr != INVALID_INODE)
				dir_size += DIR_ENTRY_SIZE;
		}

		if (m > nr_dir_entries || /* all entries have been iterated OR */
		    flg) /* file is found */
			break;
	}
	assert(flg);
	if (m == nr_dir_entries) { /* the file is the last one in the dir */
		dir_node->i_size = dir_size;
		sync_inode(dir_node);
	}
	return REGULAR;
}

/*****************************************************************************
 *                                delete
 *****************************************************************************/
/**
 * @author JSON
 * @version 3.0
 * delete a file or a dir
 *****************************************************************************/
PRIVATE int delete(struct inode *pin, struct inode *dir_inode){
	if (pin->i_cnt > 1) {	/* the file was opened */
		return UNREMOVE;
	}

	if (pin->i_mode == I_DIRECTORY){
		int dir_blk0_nr = pin->i_start_sect;
		int nr_dir_blks = (pin->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
		int nr_dir_entries = pin->i_size / DIR_ENTRY_SIZE;
		int m = 0;
		struct dir_entry * pde;
		struct inode *childpin = 0;
		for (int i = 0; i < nr_dir_blks; i++) {
			int rettype = REGULAR;
			RD_SECT(pin->i_dev, dir_blk0_nr + i);
			pde = (struct dir_entry *)fsbuf;
			for (int j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) {
				if (pde->inode_nr){
					/* child file exist */
					rettype = delete(get_inode(pin->i_dev, pde->inode_nr), pin);
				}
				if (++m > nr_dir_entries)
					break;
				if (rettype != REGULAR)
					return rettype;
			}
			if (m > nr_dir_entries) /* all entries have been iterated */
				break;
		}
	}

	struct super_block * sb = get_super_block(pin->i_dev);
	int inode_nr = pin->i_num;

	/*************************/
	/* free the bit in i-map */
	/*************************/
	int byte_idx = inode_nr / 8;
	int bit_idx = inode_nr % 8;
	assert(byte_idx < SECTOR_SIZE);	/* we have only one i-map sector */
	/* read sector 2 (skip bootsect and superblk): */
	RD_SECT(pin->i_dev, 2);
	assert(fsbuf[byte_idx % SECTOR_SIZE] & (1 << bit_idx));
	fsbuf[byte_idx % SECTOR_SIZE] &= ~(1 << bit_idx);
	WR_SECT(pin->i_dev, 2);

	/**************************/
	/* free the bits in s-map */
	/**************************/
	/*
	 *           bit_idx: bit idx in the entire i-map
	 *     ... ____|____
	 *                  \        .-- byte_cnt: how many bytes between
	 *                   \      |              the first and last byte
	 *        +-+-+-+-+-+-+-+-+ V +-+-+-+-+-+-+-+-+
	 *    ... | | | | | |*|*|*|...|*|*|*|*| | | | |
	 *        +-+-+-+-+-+-+-+-+   +-+-+-+-+-+-+-+-+
	 *         0 1 2 3 4 5 6 7     0 1 2 3 4 5 6 7
	 *  ...__/
	 *      byte_idx: byte idx in the entire i-map
	 */
	bit_idx  = pin->i_start_sect - sb->n_1st_sect + 1;
	byte_idx = bit_idx / 8;
	int bits_left = pin->i_nr_sects;
	int byte_cnt = (bits_left - (8 - (bit_idx % 8))) / 8;

	/* current sector nr. */
	int s = 2  /* 2: bootsect + superblk */
		+ sb->nr_imap_sects + byte_idx / SECTOR_SIZE;

	RD_SECT(pin->i_dev, s);

	int i;
	/* clear the first byte */
	for (i = bit_idx % 8; (i < 8) && bits_left; i++,bits_left--) {
		assert((fsbuf[byte_idx % SECTOR_SIZE] >> i & 1) == 1);
		fsbuf[byte_idx % SECTOR_SIZE] &= ~(1 << i);
	}

	/* clear bytes from the second byte to the second to last */
	int k;
	i = (byte_idx % SECTOR_SIZE) + 1;	/* the second byte */
	for (k = 0; k < byte_cnt; k++,i++,bits_left-=8) {
		if (i == SECTOR_SIZE) {
			i = 0;
			WR_SECT(pin->i_dev, s);
			RD_SECT(pin->i_dev, ++s);
		}
		assert(fsbuf[i] == 0xFF);
		fsbuf[i] = 0;
	}

	/* clear the last byte */
	if (i == SECTOR_SIZE) {
		i = 0;
		WR_SECT(pin->i_dev, s);
		RD_SECT(pin->i_dev, ++s);
	}
	unsigned char mask = ~((unsigned char)(~0) << bits_left);
	assert((fsbuf[i] & mask) == mask);
	fsbuf[i] &= (~0) << bits_left;
	WR_SECT(pin->i_dev, s);

	/***************************/
	/* clear the i-node itself */
	/***************************/
	pin->i_mode = 0;
	pin->i_size = 0;
	pin->i_start_sect = 0;
	pin->i_nr_sects = 0;
	sync_inode(pin);
	/* release slot in inode_table[] */
	put_inode(pin);

	return REGULAR;
}
