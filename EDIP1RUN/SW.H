#define    scr         (*scr_ptr)
#define    scrn        (*scrn_ptr)

#define Gettxt(x)      gettext((x).l,(x).u,(x).r,(x).d,(x).img)
#define Puttxt(x)      puttext((x).l,(x).u,(x).r,(x).d,(x).img)
#define sw_Store(x)    (x).img = malloc(((x).r-(x).l+1)*((x).d-(x).u+1)*2);\
											 Gettxt(x)
#define sw_Restore(x)  Puttxt(x); free((x).img)

typedef         struct  texel_struct {
	unsigned char   ch;
	unsigned char   at;
	}             scr_array[25][80];
typedef   int   scrn_array[25][80];
typedef         struct sw_wind {
	char            l,u,r,d;
	char            t;
	char            *ttl;
	void            *img;
	}             w_descr;
typedef         struct sw_windcolors {
	char            ft;
	unsigned char   fc,bc,tc,hc;
	}             wc_descr;
typedef         struct sw_menu {
	char            cs;                             /* current cursor position */
	char            en;                                      /* entries number */
	char            eh;                            /* entries number on height */
	char            ew;                                         /* enter width */
	unsigned char   fl;               /* display attribute of the first letter */
	char            **entr;
	char            **prom;
	long		        *help;
	}             m_descr;

extern scr_array  far  *scr_ptr;
extern scrn_array far  *scrn_ptr;
extern unsigned   char b[2][6];

void  sw_draw( w_descr *w,wc_descr *wc );
int   sw_menu( w_descr *w,wc_descr *wc,m_descr *m );
void  sw_dye_at( char l,char u,char r,char d,char at );
void  sw_dye_ch( char l,char u,char r,char d,char ch );
void  sw_puts( char l,char u,char *strptr,char at );
void  sw_gettext( char l,char u,char r,char d,int *txt );
void  sw_puttext( char l,char u,char r,char d,int *txt );
