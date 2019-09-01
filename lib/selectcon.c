/*************************************************************************//**
 *****************************************************************************
 * @file   write.c
 * @brief  write()
 * @author Forrest Y. Yu
 * @date   2008
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
 *                                selectcon
 *****************************************************************************/
/**
 * select console.
 * 
 * @param fd     File descriptor.
 * @param buf    Buffer including the bytes to write.
 * @param count  How many bytes to write.
 * 
 * @return  On success, the number of bytes written are returned.
 *          On error, -1 is returned.
 *****************************************************************************/
PUBLIC int selectcon(int num)
{
	MESSAGE msg;
	msg.type = SELCON;
	msg.CNT  = num;

	send_recv(BOTH, TASK_TTY, &msg);
	return 0;
}
