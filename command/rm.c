#include "stdio.h"
#define file_path 128

int main(int argc, char * argv[])
{
	/* open src file */
	char *pathname = argv[1];
	int fd = unmllink(pathname, conf_dir(argv[argc - 1], GET, 0));

	if(fd < 0){
		switch(fd){
		case INVALID_FORM:
			printf("invalid form!\n");
			break;
		case FILENONEXIST:
		case INVALID_PATH:
			printf("no such path!\n");
			break;
		}
		return 1;
	}
	return 0;
}
