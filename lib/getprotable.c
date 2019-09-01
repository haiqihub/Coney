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

PUBLIC int getprotable(char out[NR_TASKS + NR_PROCS][16]){
	char proc_name[NR_TASKS + NR_PROCS][16];
	MESSAGE msg;
	msg.type = GET_PROCNAME;
	msg.PROCS_NAME=proc_name;
	send_recv(BOTH, TASK_SYS, &msg);
	memcpy( *out,*msg.PROCS_NAME,16*msg.CNT);
	return msg.CNT;
}
