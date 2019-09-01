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
//trash,totally.
PUBLIC int vsscanf(char* buf,const char *fmt, va_list args,int *pos)
{
	char*	p=fmt;

	int	m=0;
	
	//memset(0,buf,sizeof(buf));
	for(fmt=fmt+*pos;*fmt;fmt++){
		if(*fmt=='%'){
			fmt++;
			if(*fmt=='d'){
				int d=0;
				while(*buf==' '||*buf=='\n'||*buf=='\t')buf++;
				while(*buf&&*buf!=' '&&*buf!='\n'&&*buf!='\t'){
					d=d*10+(*buf-'0');
					buf++;				
				}
				m++;
				*((int*)*((int*)args)) = d;
				args+=4;
				if(*buf==0)break;
				continue;		
			}
			if(*fmt=='s'){
				char *dest=(char*)(*(int*)(args));				
				args+=4;	
				while(*buf==' '||*buf=='\n'||*buf=='\t')buf++;
				while(*buf&&*buf!=' '&&*buf!='\n'&&*buf!='\t'){
					*dest=*buf;
					dest++;
					buf++;				
				}
				m++;
				if(*buf==0)break;
				continue;
			}
		}
		
	}
	fmt++;
	*pos=fmt-p;
	return m;

}
/*======================================================================*
                                 sscanf/DO NOT use,tks a lot/
 *======================================================================*/
int sscanf(char *buf, const char *fmt, ...)
{
	va_list arg = (va_list)((char*)(&fmt) + 4);        /* 4 是参数 fmt 所占堆栈中的大小 */
	return vsscanf(buf, fmt, arg,0);
}
