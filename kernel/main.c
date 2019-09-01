
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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
#include "proto.h"


//460
PRIVATE void MyInit();
PRIVATE void MKUser();
PRIVATE void InitUser();
PRIVATE void addNewUser();
PRIVATE int verifyPriority();
PRIVATE void initDir(char tty[10]);
PRIVATE void su(char tty[10]);
PRIVATE int box();
PRIVATE int calc();
PRIVATE int toc();
void help();
/*****************************************************************************
 *                               kernel_main
 *****************************************************************************/
/**
 * jmp from kernel.asm::_start. 
 * 
 *****************************************************************************/
PUBLIC int kernel_main()
{
	disp_str("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	usercount=0;
	userlevel=1;
	MyInit();
	int i, j, eflags, prio;
        u8  rpl;
        u8  priv; /* privilege */

	struct task * t;
	struct proc * p = proc_table;

	char * stk = task_stack + STACK_SIZE_TOTAL;

	for (i = 0; i < NR_TASKS + NR_PROCS; i++,p++,t++) {
		if (i >= NR_TASKS + NR_NATIVE_PROCS) {
			p->p_flags = FREE_SLOT;
			continue;
		}

	        if (i < NR_TASKS) {     /* TASK */
                        t	= task_table + i;
                        priv	= PRIVILEGE_TASK;
                        rpl     = RPL_TASK;
                        eflags  = 0x1202;/* IF=1, IOPL=1, bit 2 is always 1 */
			prio    = 15;
                }
                else {                  /* USER PROC */
                        t	= user_proc_table + (i - NR_TASKS);
                        priv	= PRIVILEGE_USER;
                        rpl     = RPL_USER;
                        eflags  = 0x202;	/* IF=1, bit 2 is always 1 */
			prio    = 5;
                }

		strcpy(p->name, t->name);	/* name of the process */
		p->p_parent = NO_TASK;

		if (strcmp(t->name, "INIT") != 0) {
			p->ldts[INDEX_LDT_C]  = gdt[SELECTOR_KERNEL_CS >> 3];
			p->ldts[INDEX_LDT_RW] = gdt[SELECTOR_KERNEL_DS >> 3];

			/* change the DPLs */
			p->ldts[INDEX_LDT_C].attr1  = DA_C   | priv << 5;
			p->ldts[INDEX_LDT_RW].attr1 = DA_DRW | priv << 5;
		}
		else {		/* INIT process */
			unsigned int k_base;
			unsigned int k_limit;
			int ret = get_kernel_map(&k_base, &k_limit);
			assert(ret == 0);
			init_desc(&p->ldts[INDEX_LDT_C],
				  0, /* bytes before the entry point
				      * are useless (wasted) for the
				      * INIT process, doesn't matter
				      */
				  (k_base + k_limit) >> LIMIT_4K_SHIFT,
				  DA_32 | DA_LIMIT_4K | DA_C | priv << 5);

			init_desc(&p->ldts[INDEX_LDT_RW],
				  0, /* bytes before the entry point
				      * are useless (wasted) for the
				      * INIT process, doesn't matter
				      */
				  (k_base + k_limit) >> LIMIT_4K_SHIFT,
				  DA_32 | DA_LIMIT_4K | DA_DRW | priv << 5);
		}

		p->regs.cs = INDEX_LDT_C << 3 |	SA_TIL | rpl;
		p->regs.ds =
			p->regs.es =
			p->regs.fs =
			p->regs.ss = INDEX_LDT_RW << 3 | SA_TIL | rpl;
		p->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
		p->regs.eip	= (u32)t->initial_eip;
		p->regs.esp	= (u32)stk;
		p->regs.eflags	= eflags;

		p->ticks = p->priority = prio;

		p->p_flags = 0;
		p->p_msg = 0;
		p->p_recvfrom = NO_TASK;
		p->p_sendto = NO_TASK;
		p->has_int_msg = 0;
		p->q_sending = 0;
		p->next_sending = 0;

		for (j = 0; j < NR_FILES; j++)
			p->filp[j] = 0;

		stk -= t->stacksize;
	}

	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

	init_clock();
    init_keyboard();

	restart();

	while(1){}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}


/**
 * @struct posix_tar_header
 * Borrowed from GNU `tar'
 */
struct posix_tar_header
{				/* byte offset */
	char name[100];		/*   0 */
	char mode[8];		/* 100 */
	char uid[8];		/* 108 */
	char gid[8];		/* 116 */
	char size[12];		/* 124 */
	char mtime[12];		/* 136 */
	char chksum[8];		/* 148 */
	char typeflag;		/* 156 */
	char linkname[100];	/* 157 */
	char magic[6];		/* 257 */
	char version[2];	/* 263 */
	char uname[32];		/* 265 */
	char gname[32];		/* 297 */
	char devmajor[8];	/* 329 */
	char devminor[8];	/* 337 */
	char prefix[155];	/* 345 */
	/* 1500 */
};

/*****************************************************************************
 *                                untar
 *****************************************************************************/
/**
 * Extract the tar file and store them.
 * 
 * @param filename The tar file.
 *****************************************************************************/
void untar(const char * filename)
{
	printf("[extract `%s'\n", filename);
	int fd = open(filename, O_RDWR);
	assert(fd != -1);

	char buf[SECTOR_SIZE * 16];
	int chunk = sizeof(buf);
	int i = 0;
	int bytes = 0;

	while (1) {
		bytes = read(fd, buf, SECTOR_SIZE);
		/*			
		if(bytes != SECTOR_SIZE && (bytes != 1 || buf[0] != 0)){
			assert(0);
		}*/
		if (buf[0] == 0) {
			if (i == 0)
				printf("    need not unpack the file.\n");
			break;
		}
		i++;

		struct posix_tar_header * phdr = (struct posix_tar_header *)buf;

		/* calculate the file size */
		char * p = phdr->size;
		int f_len = 0;
		while (*p)
			f_len = (f_len * 8) + (*p++ - '0'); /* octal */

		int bytes_left = f_len;
		int fdout = open(phdr->name, O_CREAT | O_RDWR | O_TRUNC);
		if (fdout == -1) {
			printf("    failed to extract file: %s\n", phdr->name);
			printf(" aborted]\n");
			close(fd);
			return;
		}
		printf("    %s\n", phdr->name);
		while (bytes_left) {
			int iobytes = min(chunk, bytes_left);
			read(fd, buf,
			     ((iobytes - 1) / SECTOR_SIZE + 1) * SECTOR_SIZE);
			bytes = write(fdout, buf, iobytes);
			assert(bytes == iobytes);
			bytes_left -= iobytes;
		}
		close(fdout);
	}
	/*
	if (i) {usercount
		lseek(fd, 0, SEEK_SET);
		buf[0] = 0;
		bytes = write(fd, buf, 1);
		assert(bytes == 1);
	}*/

	close(fd);

	printf(" done, %d files extracted]\n", i);
}

/*****************************************************************************
 *                                shabby_shell
 *****************************************************************************/
/**
 * A very very simple shell.
 * 
 * @param tty_name  TTY file name.
 *****************************************************************************/
int shabby_shell(int pid)
{
	char tty[10];
	char * dirmain="main";
	int fd_stdin;
	int fd_stdout;
	int i;
	int dir_level;
	if (get_tty_name(tty)){
		fd_stdin  = open(tty, O_RDWR);
		assert(fd_stdin  == 0);
		fd_stdout = open(tty, O_RDWR);
		assert(fd_stdout == 1);
	}
	else {
		printf("tty_name: %s\n",tty);
		return 1;
	}
	char path[MAX_PATH];
	char rdbuf[128];
	
	animation();
	printf("welcome!\n");
	//printf("tty_name: %s\n",tty);
	//printf("%d\n",tty[strlen(tty)-1]-'0');
    	initDir(tty);
	while (1) {
		conf_dir(tty, NAME, path);
		int r;
		if(userlevel)
			write(1, "$ ", 2);
		else
			write(1, "[sudo]$ ", 8);
		write(1, path, strlen(path));
		dir_level=0;
		i=0;
		for(;i<strlen(path);i++)
		{
			if(path[i]=='/')
			{
				
				dir_level++;
			}
		}
		r = read(0, rdbuf, 70);
		rdbuf[r] = 0;
		int argc = 0;
		char * argv[PROC_ORIGIN_STACK];
		char * p = rdbuf;
		char * s;
		int word = 0;
		char ch;
		do {
			ch = *p;
			if (*p != ' ' && *p != 0 && !word) {
				s = p;
				word = 1;
			}
			if ((*p == ' ' || *p == 0) && word) {
				word = 0;
				argv[argc++] = s;
				*p = 0;
			}
			p++;
		} while(ch);
		argv[argc++] = tty;
		argv[argc] = 0;
		
		if(argc==1)continue;
		//exit the shell
		if(!strcmp(argv[0],"exit")){
			break;
		}
		else if((!strcmp(argv[0],"cd"))&&dir_level==2&&userlevel==1&&(!strcmp(argv[1],".."))){
			printf("permission denied!\n");
			continue;
		}
		
		int fd = open(argv[0], O_RDWR);
		if (fd == -1) {
			if(strcmp(rdbuf,"help") == 0){
				help();
			}else if(strcmp(rdbuf, "sudo") == 0){
				if(verifyPriority())
				{
					userlevel=0;
				}
				else
					printf("Wrong Password\n");
			}else if(strcmp(rdbuf, "desudo") == 0){
				if(verifyPriority())
				{
					userlevel=1;
				}
				else
					printf("Wrong Password\n");
			}else if(strcmp(rdbuf, "box") == 0){
				box();
			}else if(strcmp(rdbuf, "toc") == 0){
				toc();
			}else if(strcmp(rdbuf, "calc") == 0){
				calc();
			}else if(strcmp(rdbuf, "mine") == 0){
				mine();
			}else if(strcmp(rdbuf, "su") == 0){
				su(tty);
			}else if(strcmp(rdbuf, "addUser") == 0){
				if(userlevel)
				{
					printf("permission denied!\n");
				}
				else
					addNewUser();
			}
			else if(rdbuf[0]){
				write(0, "{", 1);
				write(0, rdbuf, r);
				write(0, "}\n", 2);
				printf("invalid instruction\n");
			}
		}
		else {
			close(fd);
			int pid = fork();
			if (pid != 0) { /* parent */
				int s;
				wait(&s);
			}
			else {	/* child */
				
				execv(argv[0], argv);
			}
		}
	}

	close(1);
	close(0);
	return 0;
}

void test(){

}

void help(){
	printf("--------------------------------------\n");
	printf("----------                ------------\n");
	printf("----------      help      ------------\n");
	printf("----------                ------------\n");
	printf("--------------------------------------\n");
	printf("-    sudo:rights of administrators   -\n");
	printf("-        desudo:rights of user       -\n");
	printf("-            su:switch user          -\n");
	printf("-               addUser              -\n");
	printf("-            mkdir <name>            -\n");
	printf("-           cd <dir or ..>           -\n");
	printf("-             rm <name>              -\n");
	printf("-          killproc <name>           -\n");
	printf("-              proclist              -\n");
	printf("-                ls                  -\n");
	printf("-            TIM <file>              -\n");
	printf("-               mine                 -\n");
	printf("-               calc                 -\n");
	printf("-                toc                 -\n");
	printf("-                box                 -\n");
	printf("-               help                 -\n");
	printf("--------------------------------------\n");
	return;
}

/*****************************************************************************
 *                                MKUser
 *****************************************************************************/
/**
 * user procedure.
 * 
 *****************************************************************************/
PRIVATE void MKUser()
{
	int fd;
    fd = open("UsersPWD", O_CREAT);
	if(fd!=-1)
		close(fd);
    fd = open("Users", O_CREAT);
	if(fd!=-1)
		close(fd);
	InitUser();
	if(usercount==0)//installed==MAGIC_V2
	{
		printf("You need to create the first user!\n");
		fd = open("Users", O_RDWR);
		close(fd);
		fd = open("UsersPWD", O_RDWR);
		close(fd);
		printf("Please enter a new user:");
		char user[128];
		int r = read(0, user, 128);
		user[r] = 0;
		printf("$ user:%s\n",user);
		printf("Please enter the password:");
		char pw[128];
		r = read(0, pw, 128);
		pw[r] = 0;
		printf("$ password:%s\n",pw);
		addUser(user,pw);
	}
	else
	{
		printf("There is an old OpenOS FS\n");
	}
	InitUser();
	while (1)
    	{
        printf("usercount is %d\n",usercount);
		if(usercount==0)
		{
			printf("WRONG! NO USER EXISTS\n");
			while(1){}
		}
        int flag = 0;
        printf("Enter User Name:");
        char buf[128];
        int r = read(0, buf, 128);
        buf[r] = 0;
        int i;
        for (i = 0; i < usercount; i++)
        {
            if (strcmp(buf, users[i]) == 0 && strcmp(buf, "empty") != 0)
            {
                printf("Enter %s Password:",users[i]);
                char buf[128];
                int r = read(0, buf, 128);
                buf[r] = 0;
                if (strcmp(buf, passwords[i]) == 0)
                {
                    strcpy(currentUser, users[i]);
                    flag = 1;
                    break;
                }
            }
        }
        if (flag)
            break;
        else
            printf("Password Error Or User Not Exist!\n");
    }
	return;
}
/*****************************************************************************
 *                                Init
 *****************************************************************************/
/**
 * The hen.
 * 
 *****************************************************************************/
void Init()
{	
	int i=0,j=0;
	char* var="empty";
	int fd_stdin  = open("/dev_tty0", O_RDWR);
	assert(fd_stdin  == 0);
	int fd_stdout = open("/dev_tty0", O_RDWR);
	assert(fd_stdout == 1);

	printf("Init() is running ...\n");
	/* extract `cmd.tar' */
	untar("/cmd.tar");
	/* init `cmd.tar' */
	for(i=0;i<2;i++)
		for(j=0;j<6;j++)
			users[i][j]=var[j];
	MKUser();
	for(int i = 0; i < 2; i++){
		int pid = fork();
		if (pid != 0) { /* parent process */
			printf("[parent is running, child pid:%d]\n", pid);
		}
		else {	/* child process */
			printf("[child is running, pid:%d]\n", getpid());
			close(fd_stdin);
			close(fd_stdout);
			exit(shabby_shell(getpid()));
			assert(0);
		}
	}
	

	while (1) {
		int s;
		int child = wait(&s);
		printf("child (%d) exited with status: %d.\n", child, s);
	}

	assert(0);
}


/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	for(;;);
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	for(;;);
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestC()
{
	for(;;);
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}

/*****************************************************************************
 *                                MyInit
 *****************************************************************************/
PUBLIC void MyInit(){
	int i;
	for (i = 0; i < NR_CONSOLES; i++){
		tty_ctrl[i] = 0;
		tty_dir[i] = 0;
	}
	tty_ctrl[0] = 1;
}

/*****************************************************************************
 *                                Users
 *****************************************************************************/
PRIVATE void InitUser()
{
	int fd = -1, n = 0, i = 0,j=0, count = 0, k = 0;
  	char bufr[128] = "";
    char bufp[128] = "";
    char buff[128] = "";
	usercount=0;
	i=0;
	j=0;
    /* init users */
	fd = open("Users", O_RDWR);
    n = read(fd, bufr, 128);
    bufr[strlen(bufr)] = '\0';
    for (i = 0; i < strlen(bufr); i++)
    {
        if (bufr[i] != ' ')
        {
            users[count][k] = bufr[i];
            k++;
        }
        else
        {
            while (bufr[i] == ' ')
            {
                i++;
                if (bufr[i] == '\0')
                {
                    users[count][k] = '\0';
                    if (strcmp(users[count], "empty") != 0)
                        usercount++;
                    count++;
                    break;
                }
            }
            if (bufr[i] == '\0')
            {
                break;
            }
            i--;
            users[count][k] = '\0';
            if (strcmp(users[count], "empty") != 0)
                usercount++;
            k = 0;
            count++;
        }
    }
    close(fd);
    count = 0;
    k = 0;

    /* init password */
	fd = open("UsersPWD", O_RDWR);
    n = read(fd, bufp, 1024);
    for (i = 0; i < strlen(bufp); i++)
    {
        if (bufp[i] != ' ')
        {
            passwords[count][k] = bufp[i];
            k++;
        }
        else
        {
            while (bufp[i] == ' ')
            {
                i++;
                if (bufp[i] == '\0')
                {
                    count++;
                    break;
                }
            }
            if (bufp[i] == '\0')
                break;
            i--;
            passwords[count][k] = '\0';
            k = 0;
            count++;
        }
    }
    close(fd);
}
/*****************************************************************************
 *                                addNewUser



 * add a new user into OpenOS
 *****************************************************************************/
PRIVATE void addNewUser()
{
	printf("Please enter a new user:");
	char user[128];
	int r = read(0, user, 128);
	user[r] = 0;
	if(strcmp(user,"")== 0)
	{
		printf("Wrong Input!");
		return;
	}
	else if(strcmp(r,"exit")== 0)
	{
		return;
	}
	printf("$ user:%s\n",user);
	printf("Please enter the password:");
	char pw[128];
	r = read(0, pw, 128);
	pw[r] = 0;
	if(strcmp(pw,"")== 0)
	{
		printf("Wrong Iput!");
		return;
	}
	else if(strcmp(r,"exit")== 0)
	{
		return;
	}
	printf("$ password:%s\n",pw);
	addUser(user,pw);
}


PRIVATE int verifyPriority()
{
	printf("[Admin Password]:");
	char password[128];
	int r = read(0, password, 128);
	password[r] = 0;
	if(strcmp(passwords[0],password)==0)
	{
		return 1;
	}
	return 0;
}

PRIVATE void initDir(char tty[10])
{
	void *dir_node;
	int fd;
	
	dir_node = conf_dir(tty, GET, 0);
	fd = mlopendir(currentUser, O_CREAT, &dir_node);
	dir_node = conf_dir(tty, GET, 0);
	fd = mlopendir(currentUser, O_RDWR, &dir_node);
	conf_dir(tty, SET, dir_node);
}
/*****************************************************************************
 *                                su
 *****************************************************************************/
PRIVATE void su(char tty[10])
{
	int flag=0;
	printf("Enter User Name:");
    char buf[128];
    int r = read(0, buf, 128);
    buf[r] = 0;
    int i;
    for (i = 0; i < usercount; i++)
    {
        if (strcmp(buf, users[i]) == 0 && strcmp(buf, "empty") != 0)
        {
            printf("Enter %s Password:",users[i]);
            char buf[128];
            int r = read(0, buf, 128);
            buf[r] = 0;
            if (strcmp(buf, passwords[i]) == 0)
            {
                strcpy(currentUser, users[i]);
                flag = 1;
                break;
            }
        }
    }
    if (flag)
	{
		int i;
		int dir_level_su=0;
		char path[MAX_PATH];
		char *pathname_su = "..";
		void *dir_node;
		int fd;
		conf_dir(tty, NAME, path);
		i=0;
		for(;i<strlen(path);i++)
		{
			if(path[i]=='/')
			{
				
				dir_level_su++;
			}
		}
		while(dir_level_su!=1)
		{
			dir_node = conf_dir(tty, GET, 0);
			fd = mlopendir(pathname_su, O_RDWR, &dir_node);
			conf_dir(tty, SET, dir_node);
			conf_dir(tty, NAME, path);
			i=0;
			dir_level_su=0;
			for(;i<strlen(path);i++)
			{
				if(path[i]=='/')
				{
				
					dir_level_su++;
				}
			}
		}
		initDir(tty);
	}
    else
        printf("Password Error Or User Not Exist!\n");
}

/*****************************************************************************
 *                                box



 * game : box
 *****************************************************************************/
PRIVATE int box() {
    printf("Welcome to Push Box Game!\n");
    printf("1--actor\n");
    printf("2--box\n");
    printf("3--goal\n");
    printf("9--wall\n");
    
    char image[4][4]={
        0,1,0,0,
        0,9,2,0,
        0,0,0,0,
        0,0,0,0};
    
    int flag=0;
    int goalx=3;
    int goaly=1;
    int chestx=1;
    int chesty=2;
    int perx=0;
    int pery=1;
    int operation=0;
    
    
    int i;  //行循环变量
    int j;  //列循环变量
    for (i=0; i<4; ++i)
        {
        for (j=0; j<4; ++j)
            {
                if(i==goalx && j==goaly){
                    printf("%d  ", 3);
                 }
                 else{
                    printf("%d  ", image[i][j]);
                 }
             }
            printf("\n");
         }
    
    printf("Now you can use 1--left 3--right 5--up 2--down 6-quit to begin game!\n");
    while(flag==0){
        scanf("%d",&operation);
    if(operation==6){
	printf("box exits!\n");
        return 0;
    }
    if(operation==1){
        if(pery==0|| image[perx][pery-1]==9 || (image[perx][pery-1]==2&&pery==1 ||(image[perx][pery-1]==2&&pery>=2 &&image[perx][pery-2]==9))){
            printf("You can not go left!\n");
        }
        else if(image[perx][pery-1]==2&&pery>=2 &&image[perx][pery-2]==0 ){
            chesty=chesty-1;
            
            image[perx][pery]=0;
          pery=pery-1;
            image[perx][pery]=1;
            image[perx][pery-1]=2;
            
        }
        else{
            image[perx][pery]=0;
            pery=pery-1;
            image[perx][pery]=1;
            
        }
    }
    else if(operation==3){
        if(pery==3|| image[perx][pery+1]==9 || (image[perx][pery+1]==2&&pery==2 ||(image[perx][pery+1]==2&&pery<=1 &&image[perx][pery+2]==9))){
                   printf("You can not go right!\n");
               }
        else if(image[perx][pery+1]==2&&pery<=1 &&image[perx][pery+2]==0 ){
                   chesty=chesty+1;
                   
                   image[perx][pery]=0;
                 pery=pery+1;
                   image[perx][pery]=1;
                   image[perx][pery+1]=2;
                  
               }
        else{
                   image[perx][pery]=0;
                   pery=pery+1;
                   image[perx][pery]=1;
                  
               }
    }
    else if(operation==2){
       if(perx==3|| image[perx+1][pery]==9 || (image[perx+1][pery]==2&&perx==2 ||(image[perx+1][pery]==2&&perx<=1 &&image[perx+2][pery]==9))){
                  printf("You can not go down!\n");
              }
       else if(image[perx+1][pery]==2&&perx<=1 &&image[perx+2][pery]==0 ){
                  chestx=chestx+1;
                  
                  image[perx][pery]=0;
                perx=perx+1;
                  image[perx][pery]=1;
                  image[perx+1][pery]=2;
                 
              }
       else{
                  image[perx][pery]=0;
                  perx=perx+1;
                  image[perx][pery]=1;
                  
              }
    }
    else if(operation==5){
       if(perx==0|| image[perx-1][pery]==9 || (image[perx-1][pery]==2&&perx==1 ||(image[perx-1][pery]==2&&perx>=2 &&image[perx-2][pery]==9))){
                  printf("You can not go up!\n");
              }
       else if(image[perx-1][pery]==2&&perx>=2 &&image[perx-2][pery]==0 ){
                  chestx=chestx-1;
                  
                  image[perx][pery]=0;
                perx=perx-1;
                  image[perx][pery]=1;
                  image[perx-1][pery]=2;
                  
              }
       else{
                  image[perx][pery]=0;
                  perx=perx-1;
                  image[perx][pery]=1;
                  
              }
    }
    

    for (i=0; i<4; ++i)
    {
        for (j=0; j<4; ++j)
        {
            if(i==goalx && j==goaly){
                if(goalx==chestx&&goaly==chesty){
                printf("%d  ", 0);
                    flag=1;
                }
                else {
                    printf("%d  ", 3);
                    
                }
            }
            else{
            printf("%d  ", image[i][j]);
            }
        }
        printf("\n");
    }
    }
    printf("Congratulations on your successful performance!\n");
    
    return 0;
}
/*****************************************************************************
 *                                calc



 * app : calc
 *****************************************************************************/

PRIVATE int calc() {
    
    int a = 0;
    int b = 0;
    int operation;
    int result = 0;
    printf("Corresponding Number Represents Operation\n");
    printf("1--add\n");
    printf("2--minus\n");
    printf("3--multiply\n");
    printf("4--divide\n");
    printf("Please enter corresponding number\n");
    scanf("%d",&operation);
    printf("Please enter first Operate number\n");
    scanf("%d",&a);
    printf("Please enter second Operate number\n");
    scanf("%d",&b);
    if(operation==1){
        result=a+b;
        printf("%d add %d is %d\n",a,b,result);
    }
    else if(operation==2){
        result=a-b;
        printf("%d minus %d is %d\n",a,b,result);
    }
    else if(operation==3){
        result=a*b;
        printf("%d multiply %d is %d\n",a,b,result);
    }
    else if(operation==4){
        result=a/b;
        printf("%d divide %d is %d\n",a,b,result);
    }
   
    return 0;
}

/*****************************************************************************
 *                                toc



 * game : toc
 *****************************************************************************/
PRIVATE int toc() {
    char image[3][3]={
    0,0,0,
    0,0,0,
    0,0,0};
    printf("Welcome To TicTacToe!\n");
    int i;  //行循环变量
    int j;  //列循环变量
       for (i=0; i<3; ++i)
           {
           for (j=0; j<3; ++j)
               {
                   
                       printf("%d ", image[i][j]);
                
                }
               printf("\n");
            }
    int x=0;
    int y=0;
    int flag=0;
   
   
    while(flag==0){
        
    printf("First gamer,please input your (x y)!\n");
    scanf("%d %d",&x,&y);
        if((x==0)||(x==1)||(x==2)){
    image[x][y]=1;
    for (i=0; i<3; ++i)
           {
           for (j=0; j<3; ++j)
               {
                   
                       printf("%d ", image[i][j]);
                
                }
               printf("\n");
            }
        if((image[0][0]==1 && image[0][1]==1 && image[0][2]==1)||
           (image[1][0]==1 && image[1][1]==1 && image[1][2]==1)||
           (image[2][0]==1 && image[2][1]==1 && image[2][2]==1)||
           (image[0][0]==1 && image[1][0]==1 && image[2][0]==1)||
           (image[0][1]==1 && image[1][1]==1 && image[2][1]==1)||
           (image[0][2]==1 && image[1][2]==1 && image[2][2]==1)||
           (image[0][0]==1 && image[1][1]==1 && image[2][2]==1)||
           (image[0][2]==1 && image[1][1]==1 && image[2][0]==1)){
            printf("First gamer win!\n");
            flag=1;
            return 0;
        }
        }
        else{
            return 0;
        }
    printf("Second gamer,please input your (x y)!\n");
    scanf("%d %d",&x,&y);
    if((x==0)||(x==1)||(x==2)){
    image[x][y]=2;
    for (i=0; i<3; ++i)
           {
           for (j=0; j<3; ++j)
               {
                 
                       printf("%d ", image[i][j]);
                   
                }
               printf("\n");
            }
        
        
        if((image[0][0]==2 && image[0][1]==2 && image[0][2]==2)||
           (image[1][0]==2 && image[1][1]==2 && image[1][2]==2)||
           (image[2][0]==2 && image[2][1]==2 && image[2][2]==2)||
           (image[0][0]==2 && image[1][0]==2 && image[2][0]==2)||
           (image[0][1]==2 && image[1][1]==2 && image[2][1]==2)||
           (image[0][2]==2 && image[1][2]==2 && image[2][2]==2)||
           (image[0][0]==2 && image[1][1]==2 && image[2][2]==2)||
           (image[0][2]==2 && image[1][1]==2 && image[2][0]==2)){
            printf("Second gamer win!\n");
            flag=1;
            return 0;
        }
    }
    else{
        return 0;
    }
        
    }
}

/*****************************************************************************
 *                                mine



 * game : mine
 *****************************************************************************/
int mine() {
    char rel[10][10]={
        0,0,0,0,0,0,2,-1,-1,1,
        0,0,0,0,0,1,3,-1,3,1,
        1,1,0,1,1,2,-1,2,1,0,
        -1,1,0,1,-1,3,2,2,0,0,
        1,1,0,1,2,3,-1,1,0,0,
        0,0,0,0,1,-1,2,1,0,0,
        0,0,0,0,1,1,1,0,1,1,
        0,0,0,0,0,1,1,1,1,-1,
        0,0,0,0,0,1,-1,1,1,1,
        0,0,0,0,0,1,1,1,0,0};
    char image[10][10]={
        9,9,9,9,9,9,9,9,9,9,
        9,9,9,9,9,9,9,9,9,9,
        9,9,9,9,9,9,9,9,9,9,
        9,9,9,9,9,9,9,9,9,9,
        9,9,9,9,9,9,9,9,9,9,
        9,9,9,9,9,9,9,9,9,9,
        9,9,9,9,9,9,9,9,9,9,
        9,9,9,9,9,9,9,9,9,9,
        9,9,9,9,9,9,9,9,9,9,
        9,9,9,9,9,9,9,9,9,9,
    };
    
    int count =0;
    int i;  //行循环变量
    int j;  //列循环变量
       for (i=0; i<10; ++i)
           {
           for (j=0; j<10; ++j)
               {
                   
                       printf("%d", image[i][j]);
                
                }
               printf("\n");
            }
    
    int x=0;
    int y=0;
    
   
    while(count<90){
        printf("Please input your (x y)!\n");
           scanf("%d %d",&x,&y);
    if(rel[x][y]==-1){
        printf("Lose!\n");
        for (i=0; i<10; ++i)
        {
        for (j=0; j<10; ++j)
            {
                
                    printf("%d", rel[i][j]);
             
             }
            printf("\n");
         }
        return 0;
    }
    else{
        image[x][y]=rel[x][y];
        count++;
        if(rel[x+1][y]!=-1){
            image[x+1][y]=rel[x+1][y];
            count++;
        }
         if(rel[x-1][y]!=-1){
            image[x-1][y]=rel[x-1][y];
            count++;
        }
         if(rel[x+1][y+1]!=-1){
            image[x+1][y+1]=rel[x+1][y+1];
            count++;
        }
         if(rel[x+1][y-1]!=-1){
            image[x+1][y-1]=rel[x+1][y-1];
            count++;
        }
         if(rel[x-1][y+1]!=-1){
            image[x-1][y+1]=rel[x-1][y+1];
            count++;
        }
        if(rel[x-1][y-1]!=-1){
            image[x-1][y-1]=rel[x-1][y-1];
            count++;
        }
         if(rel[x][y+1]!=-1){
            image[x][y+1]=rel[x][y+1];
            count++;
        }
        if(rel[x][y-1]!=-1){
            image[x][y-1]=rel[x][y-1];
            count++;
        }
        for (i=0; i<10; ++i)
        {
        for (j=0; j<10; ++j)
            {
                
                    printf("%d", image[i][j]);
             
             }
            printf("\n");
         }
        if(count==90){
            printf("Success!\n");
                return 0;
        }
    }
        
        
    }
}
