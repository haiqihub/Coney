
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
#include "keyboard.h"
#include "proto.h"

/*****************************************************************************
 *                                scanf
 *****************************************************************************/
/**
 * The most famous one.
 * only support space as spilt,and can't make format for input.
 * will ignore anything except '%d' and '%s' in fmt string.
 * @note do not call me in any TASK, call me in USER PROC.
 * 
 * @param fmt  The format string
 * 
 * @return  The number of vars read.
 *****************************************************************************/
PUBLIC int scanf(const char *fmt, ...)
{
	
	
	char buf[70];
	va_list arg = (va_list)((char*)(&fmt) + 4);        /* 4 是参数 fmt 所占堆栈中的大小 */	
	int pos=0;
	int cnt=0;
	
	int tot=0;
	char* p =fmt;
	for(;*p;p++){
		if(*p=='%')tot++;	
	}
	
	while(cnt!=tot){
		memset(0,buf,sizeof(buf));
		int c = read(0, buf, 70);
		
		buf[69]=0;
		cnt += vsscanf(buf,fmt+pos, arg+(cnt<<2),&pos);
		
	}

	return cnt;
}

