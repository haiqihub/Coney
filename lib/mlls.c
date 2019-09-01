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
 *                                mlls()
 *****************************************************************************/
PUBLIC int mlls(void **pp_node,char out[20][12]){
	char dir_name[20][12];
	MESSAGE msg;
	msg.type	= MLLS;
	msg.DIR_NAME	= dir_name;
	msg.NODE	= *pp_node;

	send_recv(BOTH, TASK_FS, &msg);
	memcpy( *out,*msg.DIR_NAME,12*msg.CNT);
	return msg.CNT;
}
