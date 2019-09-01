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
	/*
	int  i=0;
	char *tmp1;
	char *tmp2;
	for(i=0;i<msg.CNT;i++){
		tmp1=*(out+i);
		tmp2=*(msg.PROCS_NAME+i);
		memcpy( tmp1,tmp2,16);
		*(tmp1+16)='\0';
		printf("test%d:%s\n",i,tmp2);
        }
*/
	memcpy( *out,*msg.PROCS_NAME,16*msg.CNT);
	return msg.CNT;
}
