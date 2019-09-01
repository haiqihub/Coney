/*************************************************************************//**
 *****************************************************************************
 * @file   get_tty_name.c
 * @brief  get_tty_name()
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
 *                                get_tty_name()
 *****************************************************************************/
PUBLIC int get_tty_name(char* pathname){
	MESSAGE msg;
	msg.PATHNAME	= pathname;
	msg.DEVICE	= 0;
	msg.type	= DEV_GETNAME;
	msg.FLAGS	= 0;

	send_recv(BOTH, TASK_TTY, &msg);
	return msg.FLAGS;
}
