/*************************************************************************//**
 *****************************************************************************
 * @file   fs/mlls.c
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

/*****************************************************************************
 *                                do_mlls
 *****************************************************************************/
/**
 * get all files and dirs
 * 
 * @return File descriptor if successful, otherwise a negative error code.
 *****************************************************************************/
PUBLIC int do_mlls(char dir_name[20][12]){
	int fd = -1;		/* return value */

	/* get parameters from the message */
	struct inode *dir_node = 0;
	dir_node = (struct inode *)fs_msg.NODE;
	int result = get_all_dir(dir_node,dir_name);
	return result;
	/*else if(strcmp(pathname, "/") == 0){
		pin = root_inode;
	}*/
}

