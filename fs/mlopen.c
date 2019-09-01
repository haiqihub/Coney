/*************************************************************************//**
 *****************************************************************************
 * @file   fs/mlopen.c
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

PRIVATE struct inode * create(char * name, struct inode *dir_inode);
PRIVATE void set_path(const char *name, struct inode *pin, struct inode *dir);
/*****************************************************************************
 *                                do_mlopen
 *****************************************************************************/
/**
 * Open a file or a dir and return the file descriptor.
 * 
 * @return File descriptor if successful, otherwise a negative error code.
 *****************************************************************************/
PUBLIC int do_mlopen(){
	int fd = -1;		/* return value */

	char pathname[MAX_PATH];
	char filename[MAX_PATH];

	/* get parameters from the message */
	int flags = fs_msg.FLAGS;	/* access mode */
	int name_len = fs_msg.NAME_LEN;	/* length of filename */
	int src = fs_msg.source;	/* caller proc nr. */
	int type = fs_msg.type;
	assert(name_len < MAX_PATH);
	phys_copy((void*)va2la(TASK_FS, pathname),
		  (void*)va2la(src, fs_msg.PATHNAME),
		  name_len);
	pathname[name_len] = 0;

	struct inode *dir_node = 0;
	struct inode *pin = 0;
	if (name_len > 0){
		if(pathname[0] == '/') {
			dir_node = root_inode;
		}
		else{
			dir_node = (struct inode *)fs_msg.NODE;
		}
	}
	else{
		return INVALID_FORM;
	}
	
	if (split_dir_file(&dir_node, pathname, filename) == INVALID_PATH){
		return INVALID_PATH;
	}
	if(filename[0] != '\0'){
		if(strcmp(filename, "..") == 0){
			if((void *)dir_node != fs_msg.NODE)
			{
				pin = dir_node;
			}
			else if((void *)dir_node == fs_msg.NODE && dir_node != root_inode){
				dirlevel--;
				pin = dir_node->i_parent;
			}
		}
		else{
			/* filename exist */
			int inodenr = get_inode_nr(dir_node, filename);
			if(inodenr == INVALID_INODE){
				if(flags & O_CREAT){
					pin = create(filename, dir_node);
					set_path(filename, pin, dir_node);
					if(fs_msg.type == MLCD){
						dirlevel++;
						/* create a dir */
						put_inode(pin);
						return REGULAR;
					}
				}
				else return FILENONEXIST;
			}
			else if(flags & O_RDWR){
				dirlevel++;
				pin = get_inode(dir_node->i_dev, inodenr);
				set_path(filename, pin, dir_node);
			}
			else return FILEEXIST;
		}
	}
	
	else if(strcmp(pathname, "/") == 0){
		pin = root_inode;
	}
	/* find a free slot in PROCESS::filp[] */
	int i;
	for (i = 0; i < NR_FILES; i++) {
		if (pcaller->filp[i] == 0) {
			fd = i;
			break;
		}
	}
	if ((fd < 0) || (fd >= NR_FILES))
		panic("filp[] is full (PID:%d)", proc2pid(pcaller));

	/* find a free slot in f_desc_table[] */
	for (i = 0; i < NR_FILE_DESC; i++)
		if (f_desc_table[i].fd_inode == 0)
			break;
	if (i >= NR_FILE_DESC)
		panic("f_desc_table[] is full (PID:%d)", proc2pid(pcaller));

	if (pin) {
		int imode = pin->i_mode;
		if(imode != I_DIRECTORY){
			if(fs_msg.type == MLCD){
				return INVALID_CMD;
			}
			/* connects proc with file_descriptor */
			pcaller->filp[fd] = &f_desc_table[i];
	
			/* connects file_descriptor with inode */
			f_desc_table[i].fd_inode = pin;
	
			f_desc_table[i].fd_mode = flags;
			f_desc_table[i].fd_cnt = 1;
			f_desc_table[i].fd_pos = 0;

			pin->i_used++;
			
			if (imode == I_CHAR_SPECIAL) {
				/* cannot open tty in this func */
				return UNKNOWN;
			}
			else {
				assert(pin->i_mode == I_REGULAR);
			}
		}
		else if (imode == I_DIRECTORY && fs_msg.type == MLCD) {
			dirlevel++;
			fs_msg.NODE = (void *)pin;
		}
		else {
			/* open a dir */
			return INVALID_CMD;
		}
	}
	else {
		return UNKNOWN;
	}
	return fd;
}

/*****************************************************************************
 *                                create
 *****************************************************************************/
/**
 * @author JSON
 * @version 3.0
 * @create_file
 *****************************************************************************/
PRIVATE struct inode * create(char * name, struct inode *dir_inode){

	int inode_nr = alloc_imap_bit(dir_inode->i_dev);
	int free_sect_nr = alloc_smap_bit(dir_inode->i_dev,
					  NR_DEFAULT_FILE_SECTS);
	
	struct inode * new_inode = get_inode(dir_inode->i_dev, inode_nr);
	
	if(fs_msg.type == MLOPEN){
		new_inode->i_mode = I_REGULAR;
	}
	else{
		new_inode->i_mode = I_DIRECTORY;
	}
	new_inode->i_size = 0;
	new_inode->i_start_sect = free_sect_nr;
	new_inode->i_nr_sects = NR_DEFAULT_FILE_SECTS;

	new_inode->i_dev = dir_inode->i_dev;
	new_inode->i_cnt = 1;
	new_inode->i_num = inode_nr;

	/* write to the inode array */
	sync_inode(new_inode);

	new_dir_entry(dir_inode, new_inode->i_num, name);

	return new_inode;
}


/*****************************************************************************
 *                                set_path
 *****************************************************************************/
/**
 * @author JSON
 * @version 3.0
 *****************************************************************************/
PRIVATE void set_path(const char *name, struct inode *pin, struct inode *dir){
	char path[MAX_PATH];
	int len;
	phys_copy((void *)va2la(TASK_FS, path), (void *)(dir->pathname), dir->path_len - 1);
	phys_copy((void *)va2la(TASK_FS, path + dir->path_len - 1), (void *)va2la(TASK_FS, name), strlen(name));
	len = dir->path_len - 1 + strlen(name);
	if(pin->i_mode == I_DIRECTORY){
		path[len++] = '/';
	}
	path[len++]='\0';
	pin->path_len = len;
	phys_copy((void *)(pin->pathname), (void *)va2la(TASK_FS, path), len);

	pin->i_used = 0;
	pin->i_parent = dir;
}

