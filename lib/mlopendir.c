/*************************************************************************//**
 *****************************************************************************
 * @file   mlopendir.c
 * @brief  mlopendir()
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
 *                                mlopendir()
 *****************************************************************************/
PUBLIC int mlopendir(const char *pathname, int flags, void **pp_node){
	MESSAGE msg;

	msg.type	= MLCD;

	msg.PATHNAME	= (void*)pathname;
	msg.FLAGS	= flags;
	msg.NAME_LEN	= strlen(pathname);
	msg.NODE	= *pp_node;
	send_recv(BOTH, TASK_FS, &msg);
	assert(msg.type == SYSCALL_RET);
	*pp_node = msg.NODE;

	return msg.RETVAL;
}
