/*************************************************************************//**
 *****************************************************************************
 * @file   do_tty_cache.c
 * @brief  do_tty_cache()
 * @author JSON
 * @version 1.0
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
 *                                do_tty_cache()
 *****************************************************************************/
PUBLIC int do_tty_cache(char* tty_name, int len){
	MESSAGE msg;
	msg.PATHNAME	= tty_name;
	msg.CNT 	= len;
	msg.DEVICE	= 0;
	msg.type	= DEV_CACHE;
	msg.FLAGS	= 0;

	send_recv(BOTH, TASK_TTY, &msg);
	return msg.FLAGS;
}
