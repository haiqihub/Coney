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

PUBLIC void killproc(char procname[16]){
	MESSAGE msg;
	msg.type = KILL;
	msg.PROC_NAME=procname;
	send_recv(SEND, TASK_MM, &msg);
}
