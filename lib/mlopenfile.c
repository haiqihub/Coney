/*************************************************************************//**
 *****************************************************************************
 * @file   mlopenfile.c
 * @brief  mlopenfile()
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
 *                                mlopenfile()
 *****************************************************************************/
PUBLIC int mlopenfile(const char *pathname, int flags, void *dir_node){
	MESSAGE msg;

	msg.type	= MLOPEN;

	msg.PATHNAME	= (void*)pathname;
	msg.FLAGS	= flags;
	msg.NAME_LEN	= strlen(pathname);
	msg.NODE	= dir_node;

	send_recv(BOTH, TASK_FS, &msg);
	assert(msg.type == SYSCALL_RET);

	return msg.RETVAL;
}
