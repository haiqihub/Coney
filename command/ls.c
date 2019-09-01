#include "stdio.h"

int main(int argc, char * argv[])
{
	char out[20][12];
	void *dir_node = conf_dir(argv[argc - 1], GET, 0);
	int cnt = mlls(&dir_node,out);
	//printf("success");
	//return 0;
	int i=0;
	for(i=0;i<cnt;i++){
		printf("%s\n",*(out+i));
	}
	return 0;
}
