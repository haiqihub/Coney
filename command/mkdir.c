#include "stdio.h"
#define file_path 128

int main(int argc, char * argv[])
{
	/* open src file */
	char *pathname = argv[1];
	void *dir_node = conf_dir(argv[argc - 1], GET, 0);
	int fd = mlopendir(pathname, O_CREAT, &dir_node);

	if(fd < 0){
		switch(fd){
		case INVALID_FORM:
			printf("invalid form!\n");
		case FILENONEXIST:
		case INVALID_PATH:
			printf("no such path!\n");
		case FILEEXIST:
			printf("file exist!\n");
		}
		return 1;
	}
	return 0;
}
