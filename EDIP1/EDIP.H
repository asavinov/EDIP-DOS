
#include <fcntl.h>
#include <alloc.h>
#include <ctype.h>  /* isdigit() isspace() */
#include <dir.h>
#include <dos.h>
#include <io.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*__________________________________________________________________________*/
#define SPECIAL        2
#define ATTRIBUTE      0
#define VALUE          1
#define DENY           0
#define PERMIT         1
#define CON            0                               
#define DIS            1
#define Matr(x,y)      (*((int huge *)(matr_buf+(long)(x)*(long)attr_num+(long)(y))))

#define Mstrcpy(y,x)   movedata(FP_SEG(matr_buf+(long)(x)*(long)(attr_num)),\
                                FP_OFF(matr_buf+(long)(x)*(long)(attr_num)),\
                                FP_SEG(matr_buf+(long)(y)*(long)(attr_num)),\
																FP_OFF(matr_buf+(long)(y)*(long)(attr_num)),\
																2*attr_num )

#define Nextchar()     getchr()
#define Backup()       bcb.cur--; bcb.lev--
#define Isalnum(x)     ( isalnum(x) || (((x)>=128) && ((x)<=255)) )
/*__________________________________________________________________________*/
typedef  struct  buf_struct {
	int      row;
	long     siz;
	long     fst;
	long     lst;
	long     lev;
	char far *buf;
	char far *cur;
	}      bufdescr;

typedef struct val_tab_struct {
	long nam;
	int  nln;
	}     val_tab_descr;

typedef struct att_tab_struct {
	long          nam;
	int           nln;
	long          com;
	int           cln;
	val_tab_descr *val;
	int           vln;
	int           len;
	}     att_tab_descr;
/*__________________________________________________________________________*/
extern int huge *matr_buf;
extern unsigned int matr_buf_len;
extern unsigned int m_str_num;
extern unsigned int not_str_num;
extern int attr_num;
extern bufdescr bcb;
extern int explain;
extern int PassNum;
