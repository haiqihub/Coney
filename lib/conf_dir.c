/*************************************************************************//**
 *****************************************************************************
 * @file   conf_dir.c
 * @brief  conf_dir()
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
#include "proto.h"

/*****************************************************************************
 *                                conf_dir()
 *****************************************************************************/
PUBLIC void *conf_dir(const char *ttyname, int flags, void *dir_node){
	MESSAGE msg;
	msg.type	= DEV_DIR;
	msg.PATHNAME	= (void*)ttyname;
	msg.FLAGS	= flags;
	msg.CNT		= strlen(ttyname);
	msg.DEVICE	= 0;
	msg.NODE   	= dir_node;
	send_recv(BOTH, TASK_TTY, &msg);
	return msg.NODE;
}
