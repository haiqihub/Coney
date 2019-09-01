#include "stdio.h"

PUBLIC int char2int(char* str)
{
	//printf("start convert\n");
	int result=0, tmp;
	char*p=str;
	for(; *p !=0; p+=1)
	{
		tmp=*p-'0';
		result=result*10+tmp;
	}
	//printf("end convert\n");
	return result;
}
