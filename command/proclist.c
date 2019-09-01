#include "stdio.h"

int main(int argc, char * argv[])
{
	int i;
	int cnt;
	char out[60][16];

	cnt=getprotable(out);
	printf("proc name:\n");	
	for(i=0;i<cnt;i++){
		printf("%s\n",*(out+i));
	}
	return 0;
}
