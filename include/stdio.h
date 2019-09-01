/*************************************************************************//**
 *****************************************************************************
 * @file   stdio.h
 * @brief  
 * @author Forrest Y. Yu
 * @date   2008
 *****************************************************************************
 *****************************************************************************/

#ifndef	_ORANGES_STDIO_H_
#define	_ORANGES_STDIO_H_

#include "type.h"

/* the assert macro */
#define ASSERT
#ifdef ASSERT
void assertion_failure(char *exp, char *file, char *base_file, int line);
#define assert(exp)  if (exp) ; \
        else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif

/* EXTERN */
#define	EXTERN	extern	/* EXTERN is defined as extern except in global.c */

/* string */
#define	STR_DEFAULT_LEN	1024

#define	O_CREAT		1
#define	O_RDWR		2
#define	O_TRUNC		4

#define SEEK_SET	1
#define SEEK_CUR	2
#define SEEK_END	3

#define	MAX_PATH	128

#define GET             1
#define SET             2
#define NAME            3

/**
 * @struct stat
 * @brief  File status, returned by syscall stat();
 */
struct stat {
	int st_dev;		/* major/minor device number */
	int st_ino;		/* i-node number */
	int st_mode;		/* file mode, protection bits, etc. */
	int st_rdev;		/* device ID (if special file) */
	int st_size;		/* file size */
};

/**
 * @struct time
 * @brief  RTC time from CMOS.
 */
struct time {
	u32 year;
	u32 month;
	u32 day;
	u32 hour;
	u32 minute;
	u32 second;
};

/**
 * @author JSON
 * @version 3.0
 */

enum mlfsrettype {
	REGULAR = -10,
	INVALID_PATH,
	INVALID_FORM,
	INVALID_CMD,
	FILENONEXIST,
	FILEEXIST,
	UNREMOVE,
	UNKNOWN
};

#define  BCD_TO_DEC(x)      ( (x >> 4) * 10 + (x & 0x0f) )

/*========================*
 * printf, printl, printx *
 *========================*
 *
 *   printf:
 *
 *           [send msg]                WRITE           DEV_WRITE
 *                      USER_PROC ------------→ FS -------------→ TTY
 *                              ↖______________↙↖_______________/
 *           [recv msg]             SYSCALL_RET       SYSCALL_RET
 *
 *----------------------------------------------------------------------
 *
 *   printl: variant-parameter-version printx
 *
 *          calls vsprintf, then printx (trap into kernel directly)
 *
 *----------------------------------------------------------------------
 *
 *   printx: low level print without using IPC
 *
 *                       trap directly
 *           USER_PROC -- -- -- -- -- --> KERNEL
 *
 *
 *----------------------------------------------------------------------
 */

/* printf.c */
PUBLIC  int     printf(const char *fmt, ...);
PUBLIC  int     printl(const char *fmt, ...);

/* vsprintf.c */
PUBLIC  int     vsprintf(char *buf, const char *fmt, va_list args);
PUBLIC	int	sprintf(char *buf, const char *fmt, ...);

/*--------*/
/* 库函数 */
/*--------*/

#ifdef ENABLE_DISK_LOG
#define SYSLOG syslog
#endif

/* lib/open.c */
PUBLIC	int	open		(const char *pathname, int flags);

/* lib/close.c */
PUBLIC	int	close		(int fd);

/* lib/read.c */
PUBLIC int	read		(int fd, void *buf, int count);

/* lib/write.c */
PUBLIC int	write		(int fd, const void *buf, int count);

/* lib/lseek.c */
PUBLIC	int	lseek		(int fd, int offset, int whence);

/* lib/unlink.c */
PUBLIC	int	unlink		(const char *pathname);

/* lib/getpid.c */
PUBLIC int	getpid		();

/* lib/fork.c */
PUBLIC int	fork		();

/* lib/exit.c */
PUBLIC void	exit		(int status);

/* lib/wait.c */
PUBLIC int	wait		(int * status);

/* lib/exec.c */
PUBLIC int	exec		(const char * path);
PUBLIC int	execl		(const char * path, const char *arg, ...);
PUBLIC int	execv		(const char * path, char * argv[]);

/* lib/stat.c */
PUBLIC int	stat		(const char *path, struct stat *buf);

/* lib/syslog.c */
PUBLIC	int	syslog		(const char *fmt, ...);

/* lib/get_tty_name.c */
PUBLIC  int     get_tty_name    (char *pathname);

/* lib/get_tty_name.c */
PUBLIC  int     do_tty_cache    (char *content, int len);

/* lib/mlopendir.c */
PUBLIC 	int 	mlopendir	(const char *pathname, int flags, void **pp_node);

/* lib/mlopenfile.c */
PUBLIC 	int 	mlopenfile	(const char *pathname, int flags, void *dir_node);

/* lib/mlls.c */
PUBLIC 	int 	mlls(void **pp_node,char out[][12]);

/* lib/unmllink.c */
PUBLIC 	int 	unmllink	(const char *pathname, void *dir_node);

/* lib/conf_dir.c */
PUBLIC 	void *conf_dir	(const char *ttyname, int flags, void *dir_node);

PUBLIC int getprotable(char out[][16]);
PUBLIC void killproc(char procname[16]);

/* scanf.c */

PUBLIC  int	scanf(const char *fmt, ...);

/* vsscanf.c */

PUBLIC	int	vsscanf(char* buf,const char *fmt, va_list args,int *pos);
PUBLIC	int	sscanf(char* buf,const char *fmt, ...);

PUBLIC	int	selectcon(int num);

PUBLIC int getdirlevel();

char * dir_file[20];
int dir_file_count;
#endif /* _ORANGES_STDIO_H_ */
