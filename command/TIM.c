#include "stdio.h"
#define max_file_len 4000
#define max_rd_len 128

int max(int a, int b){
	return a > b? a : b;
}

int main(int argc, char * argv[])
{
	int num;
	char *pathname = argv[1];
	char *caller=argv[argc - 1];
	void *dir_node = conf_dir(argv[argc - 1], GET, 0);
	int fd = mlopenfile(pathname, O_RDWR, dir_node);
	num=caller[strlen(caller)-1]-'0';
	if(fd < 0){
		switch(fd){
		case INVALID_FORM:
			printf("invalid form!\n");
			break;
		case INVALID_PATH:
			printf("no such path!\n");
			break;
		case FILENONEXIST:
			fd = mlopenfile(pathname, O_CREAT, dir_node);
			break;
		}
		if(fd < 0){
			return 1;
		}
	}

	/*read and process the file content */
	char content[max_file_len];
	int contpos = read(fd, content, max_file_len);
	close(fd);

	/* open tty console */
	char tty[10];
	int fd_stdin;
	int fd_stdout;
	if (!get_tty_name(tty)){
		printf("No available console!\n");
		return 1;
	}
	fd_stdin  = open(tty, O_RDWR);
	fd_stdout = open(tty, O_RDWR);

	/* print to console */
	if (!do_tty_cache(tty, strlen(tty))){
		printf("Switch mode failed!\n");
		return 1;
	}
	write(fd_stdout, content, contpos);

	fd = mlopenfile(pathname, O_RDWR,dir_node);
	
	/* process buf from keyboard */
	char rdbuf[max_rd_len];
	int rdlen;
	while(1){
		rdlen = read(fd_stdin, rdbuf, max_rd_len);
		rdbuf[rdlen] = 0;
		
		if(rdlen > 0 && rdbuf[0] == ':'){
			if(strcmp(rdbuf, ":q\n") == 0){
				/*quit without save*/
				break;
			}
			if(strcmp(rdbuf, ":wq\n") == 0){
				content[contpos] = 0;
				write(fd, content, contpos);
				printf("save file: %s, len: %d\n", pathname, contpos);
				break;
			}
		}
		int rdpos = 0;
		while(rdbuf[rdpos]){
			if(rdbuf[rdpos] == '\b'){
				contpos--;
			}
			else{
				content[contpos++] = rdbuf[rdpos];
			}
			rdpos++;
		}
	}
	close(fd_stdin);
	close(fd_stdout);
	close(fd);
	selectcon(num);
	return 0;
}
