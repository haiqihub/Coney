#include "stdio.h"

int main(int argc, char * argv[])
{
	int i;
	int num;
	char *caller=argv[argc - 1];
	num=caller[strlen(caller)-1]-'0';
	if(argc<=2){
	printf("please proc name\n");
	return 0;
	}
	

	char p[16];
	for(i=0;i<16;i++){
		p[i]=*(argv[1]+i);
	}

	printf("kill proc %s\n",p);
	killproc(p);
	selectcon(num);
	return 0;
}
