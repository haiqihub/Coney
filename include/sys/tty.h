
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				tty.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef _ORANGES_TTY_H_
#define _ORANGES_TTY_H_


#define TTY_IN_BYTES		256	/* tty input queue size */
#define TTY_OUT_BUF_LEN		2	/* tty output buffer size */
#define TTY_TEXT_BYTES          4096    /* tty text mode */

struct s_tty;
struct s_console;

/* TTY MODE */
enum console_mode{
	ICONSOLE = 1,
	ITEXT
};

/* TTY */
typedef struct s_tty
{
	u32	ibuf[TTY_IN_BYTES];	/* TTY input buffer */
	u32*	ibuf_head;		/* the next free slot */
	u32*	ibuf_tail;		/* the val to be processed by TTY */
	int	ibuf_cnt;		/* how many */

	int	tty_caller;
	int	tty_procnr;
	void*	tty_req_buf;
	int	tty_left_cnt;
	int	tty_trans_cnt;

	struct s_console *	console;
	int     imode;
	/**
 	* @author JSON
 	* @version 2.0	
 	*/
	char    itext[TTY_TEXT_BYTES];
	int     itext_cnt;
	int     itext_pos;
}TTY;

#endif /* _ORANGES_TTY_H_ */
