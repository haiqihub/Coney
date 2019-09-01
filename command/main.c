#include"stdio.h"

int main(int argc, char * argv[])
{
	int fd;
	char * a="fro";
	printf("%s\n",argv[argc - 1]);
	void *dir_node = conf_dir(argv[argc - 1], GET, 0);
	fd = mlopendir(a, O_RDWR, &dir_node);
	conf_dir(argv[argc - 1], SET, dir_node);
	return 0;
}
