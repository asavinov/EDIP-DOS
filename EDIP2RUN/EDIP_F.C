#include <alloc.h>
#include <bios.h>
#include <conio.h>
#include <ctype.h>
#include <dir.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sw_keys.h"
#include "sw.h"
/*****************************************************************************/
/* _____________________________________________________MACRO_DEFINITIONS___ */
/*****************************************************************************/
#define    BCBSIZ       30000
#define    ATTW         0
#define    VALW         1
/*****************************************************************************/
/* ______________________________________________________TYPE_DEFINITIONS___ */
/*****************************************************************************/
typedef  struct  buf_struct {
	long      fst;
	long      lst;
	long      lev;
	char     *buf;
	char     *cur;
	}      bufdescr;
typedef  struct  attw_struct {
	int      atn;                /* a quantity of the attributes in the module */
	int      csv;   /* the attribute number, a vertical position of the cursor */
	int      csh;   /* a horizontal position of the cursor in the attr. window */
	int      up;    /* the number of the upper string in the attributes window */
	}      attwdescr;
typedef  struct  valw_struct {
	int      vln;                /* quantity of values of the active attribute */
	int      csv;                  /* the number of value, a vertical position */
	int      csh;  /* a horizontal position of the cursor in the values window */
	int      up;        /* the number of the upper string in the values window */
	long     ofs[16]; /* a displacements to the values of the active attribute */
	}      valwdescr;
typedef  struct  state_struct {
	int      fct;			  		        		          						    /* the facts */
	int 		 inf;                                                 /* inference */
  unsigned long     ffct;                                     /* fuzzy facts */
  unsigned long     finf;                                 /* fuzzy inference */
					 struct  mrk_struct {
		int 	   inf;                                        /* inference marker */
		int    	 exp;                                      /* explanation marker */
		int      hyp;                                       /* hypothesis marker */
		}      mrk;
	}      statedescr;
typedef  struct  environ_struct {
	short    sound;                                  /* sound: 1 - On, 0 - Off */
	short    conf;                    /* config.edp auto save: 1 - On, 0 - Off */
	short    cons;             /* consultation file auto save: 1 - On, 0 - Off */
	short    bak;              /* create backup file variable: 1 - On, 0 - Off */
	}      environdescr;
/*****************************************************************************/
/* ______________________________________________________GLOBAL_VARIABLES___ */
/*****************************************************************************/
bufdescr       bcb;                                  /* buffer control block */
attwdescr      acb;                        /* attribute window control block */
valwdescr      vcb;                            /* value window control block */
statedescr     *s;                          /* a pointer to the state vector */
environdescr   env;                                   /* environment options */
long           mofs;/* a displacement to the matrix beginning in module file */
int            mstrn;             /* a quantity of the strings in the matrix */
int            modh;                          /* a handle of the module file */
int         	 actw;                       /* an active window: ATTW or VALW */
int         	 contr;                               /* state system variable */
int            fstinf;
int            lstinf;
int            resatt;
int 					 curexp;
int            fstexp;
int            lstexp;
int            goalatt;                                    /* goal attribute */
int            goalval;                                        /* goal value */
int            curhyp;
int            fsthyp;
int            lsthyp;
char           curbase[80];                           /* current module path */
char           curcon[80];                      /* current consultation path */
char           basename[13];                     /* current module (kb) name */
char           conname[13];                     /* current consultation name */
char           loadk_s[80]  = "*.MOD";
char           loadc_s[80]  = "*.CON";
char           writ_s[80];
char           change_s[66];
extern scr_array  far   *scr_ptr  = ( scr_array far *  )0xb8000000L;
extern scrn_array far   *scrn_ptr = ( scrn_array far * )0xb8000000L;
extern unsigned   char  b[2][6] = {
	{ 218,196,191,179,192,217 },
	{ 201,205,187,186,200,188 }
	};

unsigned long fmask_l[16] = {
	1l, 1l<<2, 1l<<4, 1l<<6, 1l<<8, 1l<<10, 1l<<12, 1l<<14,
	1l<<16, 1l<<18, 1l<<20, 1l<<22, 1l<<24, 1l<<26, 1l<<28, 1l<<30 };
unsigned long fmask_h[16] =	{
	1l<<1, 1l<<3, 1l<<5, 1l<<7, 1l<<9, 1l<<11, 1l<<13, 1l<<15,
	1l<<17, 1l<<19, 1l<<21, 1l<<23, 1l<<25, 1l<<27, 1l<<29, 1l<<31 };
unsigned long fmask[16] = {
	3l, 3l<<2, 3l<<4, 3l<<6, 3l<<8, 3l<<10, 3l<<12, 3l<<14,
	3l<<16, 3l<<18, 3l<<20, 3l<<22, 3l<<24, 3l<<26, 3l<<28, 3l<<30 };

w_descr    all_scr   = { 1,1,80,25,0,"",0 };
wc_descr   menu_wc   = { 0,0x09,0x31,0x0d,0x70 };
wc_descr   str_wc    = { 0,0x0b,0x70,0x0d,0x31 };
w_descr    att_w     = { 1,3,80,16,10," A T T R I B U T E S ",0 };
wc_descr   att_wc    = { 0,0x0d,0x0e,0x0f,0x4a };
w_descr    val_w     = { 1,17,80,24,10," V A L U E S ",0 };
wc_descr   val_wc    = { 0,0x0d,0x0e,0x0f,0xa4 };
w_descr    mainm_w   = { 1,0,80,2,1,"",0 };
wc_descr   mainm_wc  = { -1,0x00,0x1b,0x00,0x5b };
/*   w_descr    vers_w    = { 21,8,61,18,14," V E R S I O N ",0 };      */
w_descr    vers_w    = { 15,5,67,21,20," V E R S I O N ",0 };
/*   wc_descr   vers_wc   = { 1,0x0c,0x71,0x0c,0x74 };                  */
wc_descr   vers_wc   = { 1,0x07,0x01,0x07,0x74 };
w_descr    err_w     = { 15,5,65,14,20," E R R O R ",0 };
wc_descr   err_wc    = { 0,0x0c,0x4b,0x0c,0xce };
w_descr    think_w   = { 21,8,60,20,12," T H I N K I N G ",0 };
wc_descr   think_wc  = { 0,0x0f,0x17,0x0f,0x9e };
w_descr    yn_w      = { 21,6,61,10,15," V E R I F Y ",0 };
wc_descr   yn_wc     = { 0,0x0c,0x71,0x0c,0xfe };
w_descr    fname_w   = { 11,6,68,20,21," Select File Name ",0 };
w_descr    know_w    = { 3,2,18, 7,0,"",0 };
w_descr    cons_w    = { 15,2,30,8,0,"",0 };
w_descr    infer_w   = { 29,2,46,5,0,"",0 };
w_descr    expl_w    = { 41,2,71,7,0,"",0 };
w_descr    hyp_w     = { 50,2,79,7,0,"",0 };
w_descr    opt_w     = { 60,2,79,7,0,"",0 };
w_descr    env_w     = { 52,4,77,8,0,"",0 };
w_descr    loadk_w   = { 5,4,49,6,8, " Load Knowledge Base File Name ",0 };
w_descr    changek_w = { 5,5,49,7,16," New Directory ",0 };
w_descr    loadc_w   = { 17,4,61,6,9, " Load Consultation File Name ",0 };
w_descr    save_w    = { 17,6,61,8,14," Rename NONAME.CON ",0 };
w_descr    writ_w    = { 17,7,61,9,9, " New Consultation File Name ",0 };
w_descr    changec_w = { 17,8,61,10,16," New Directory ",0 };
w_descr    com_w     = { 5,3,75,24,25," Comment ",0 };
wc_descr   com_wc    = { 1,0x0a,0x71,0x0a,0x74 };
wc_descr   help_wc   = { 1,0x0a,0x07,0x2f,0x74 };
char       *mainm_entr[] = { " Knowledge","Consultation"," Inference",
					 "Explanation"," Hypothesis"," Options" };
long       mainm_help[]  = { 0, 0, 0, 0, 0, 0 };
char       *know_entr[]  = { "Load      F3","Change dir","OS shell",
					 "Quit   Alt-X" };
long       know_help[]   = { 0, 0, 0, 0 };
char       *cons_entr[]  = { "Load      F3","New","Save      F2",
					 "Write to","Change dir" };
long       cons_help[]   = { 0, 0, 0, 0, 0 };
char       *infer_entr[] = { "Inference   F9","Set goal    F6" };
long       infer_help[]  = { 0, 0 };
char       *expl_entr[]  = { "First explanation        F8",
					 "Next  explanation   CTRL-F8","Prev. explanation    ALT-F8",
					 "Last  explanation  SHIFT-F8" };
long       expl_help[]   = { 0, 0, 0, 0 };
char       *hyp_entr[]   = { "First hypothesis        F7",
					 "Next  hypothesis   CTRL-F7","Prev. hypothesis    ALT-F7",
					 "Last  hypothesis  SHIFT-F7" };
long       hyp_help[]    = { 0, 0, 0, 0 };
char       *opt_entr[]   = { "Environment","Save options","Arguments",
					 "Retrieve options" };
long       opt_help[]    = { 0, 0, 0, 0 };
char       *env_entr[3];
char       *know_prom[]  = { "Loads a knowledge base file from the disk",
					 "Displays the current directory and lets you change it",
					 "Leaves EDIP temporarily","Exit from EDIP" };
char       *cons_prom[]  = { "Loads a consultation file from the disk",
					 "Starts a new consultation with the name NONAME.CON",
					 "Saves the consultation file to the disk",
					 "Changes the name of the consultation file and writes it to the disk",
					 "Displays the current directory and lets you change it" };
char       *infer_prom[] = { "Infers on input data","Sets a goal" };
char       *expl_prom[]  = {
					 "Searchs a first explanation","Searchs a next explanation",
					 "Searchs a previous explanation","Searchs a last explanation" };
char       *hyp_prom[]   = {
					 "Gives a first hypothesis","Gives a next hypothesis",
					 "Gives a previous hypothesis","Gives a last hypothesis" };
char       *opt_prom[]   = {
					 "Lets you tailor the system inviroment",
					 "Lets you save your options in a configuration file config.edp",
					 "Allows you to control how the integrated environment works",
					 "Loads a configuration file" };
char       *env_prom[]   = {
					 "Automatically saves a config. file whenever you Quit or OS shell( toggle on )",
					 "Automatically saves a cons. file whenever you Quit or OS shell( toggle on )",
					 "Creates a backup file when you Save( toggle on )" };
m_descr    mainm_m   = { 0,6,6,11,0x1b,mainm_entr,0,mainm_help };
m_descr    fname_m   = { 0,0,4,12,0x70,0,0,0L };
m_descr    know_m    = { 0,4,1,12,0x3c,know_entr,know_prom,know_help };
m_descr    cons_m    = { 0,5,1,12,0x3c,cons_entr,cons_prom,cons_help };
m_descr    infer_m   = { 0,2,1,14,0x3c,infer_entr,infer_prom,infer_help };
m_descr    expl_m    = { 0,4,1,27,0x3c,expl_entr,expl_prom,expl_help };
m_descr    hyp_m     = { 0,4,1,26,0x3c,hyp_entr,hyp_prom,hyp_help };
m_descr    opt_m     = { 0,4,1,16,0x3c,opt_entr,opt_prom,opt_help };
m_descr    env_m     = { 0,3,1,22,0x3c,env_entr,env_prom,0L };
char       *err_entr[]   = {
"\n\r               Can not open file.",             /* initmod(),validmod() */
"\n\r                Path not found.",                        /* findfiles() */
"\n\r            There is no such files.",                    /* findfiles() */
"\n\r          Invalid file name extension.",                  /* validmod() */
"\n\r             It is not EDIP module.",                     /* validmod() */
"\n\r                Invalid version.",                        /* validmod() */
"\n\r          It is not EDIP consultation.",                  /* validcon() */
"\n\r      This consultation is not in compliance\n\r              with knowledge base.",/* validcon() */
"\n\r            Can not create BAK file.",                    /* save_con() */
"\n\r   Can not open configuration file config.edp .\n\r               Load a module file.",/* load_config() */
"\n\r    Can not creat configuration file config.edp"      /* write_config() */
};
char       *think_entr[] = {
" Inference is in progress . . .",
"Explanation is in progress . . .",
" Hypothesis is in progress . . ."
};
char       *yn_entr[] = {
" Overwrite this file ?  Press   or"
};
char       *think_mes[]  = {
"Input data are in the contradiction            with knowledge.",
"      Can not make inference:           unsufficient quantity of data.",
" Inference is finished successully.",
"    There is nothing to explain.",
"          This is the fact.",
"  There is no others explanations.",
"Explanation is finished successully.",
"   This goal is infered already.",
"       This goal is the fact.",
"   There is no others hypothesis.",
"Hypothesis is finished successully."
};
/*****************************************************************************/
/* _________________________________________________FUNCTIONAL_PROTOTYPES___ */
/*****************************************************************************/
char getchr( long offset );
long getlong( long offset );
/*****************************************************************************/
main() {  /* ________________________________________________MAIN_PROGRAM___ */
/*****************************************************************************/
	start();
	if( load_config( "config.edp" ) == -1 ) { if( no_config() == -1 ) return; }
	else if( initmod( curbase ) == -1 ) if( no_config() == -1 ) return;
	dispatch();
	endmod();
	}
/*****************************************************************************/
start() {  /* ______________________________________SYSTEM_INITIALIZATION___ */
/*****************************************************************************/
	cursor( 0,0 );
	initscreen();
	getcwd( change_s,66 );
  version();
	}
/*****************************************************************************/
initmod( char *modpath ) {  /* _____________________MODULE_INITIALIZATION___ */
/*****************************************************************************/
	int i,*txt;
	txt = ( int *)malloc( 80*sizeof( int ) ); sw_gettext( 1,25,80,25,txt );
	sw_dye_ch( 0,24,79,24,32 );
	sw_puts( 1,24,"Loading Module File . . .",0x1c );
	if( (modh = _open( modpath,O_RDONLY ) ) == -1 ) {
		err_mes( 0 );                                       /* can not open file */
		sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
    return( -1 );
		}
	bcb.buf = ( char * )malloc( BCBSIZ );
	loadbuf( 0L );
	acb.atn = getint ( 6L );   /* a quantity of the attributes in the block */
	mofs = getlong( 22L );/* a displacement to the matrix beginning in the file */
	mstrn = getint( 8L );         /* a quantity of the strings in the matrix */
  contr = 0;
	s = ( statedescr * )malloc( acb.atn*sizeof( statedescr ) );
	for( i=0;i<acb.atn;i++ ) {                     /* initialize state vector */
		s[i].fct = s[i].inf = -1;
    s[i].ffct = s[i].finf = ~0L;
		s[i].mrk.inf = 0;
		s[i].mrk.exp = s[i].mrk.hyp = -1;
		}                                            
	fstinf  = acb.atn - 1; lstinf = 0;
	fstexp  = acb.atn - 1; lstexp = 0; curexp = 0;
	fsthyp  = acb.atn - 1; lsthyp = 0; curhyp = 0;
	goalatt = goalval = 0; showgoal();
	strcpy( curcon,"NONAME.CON" );
	showbase(); showcon();
	textattr( 0x0e );
	acb.csv = acb.up = 0;	fillattw();
	vcb.csv = vcb.up = 0; fillvalw( acb.csv );
	actw = ATTW;
  sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
	}
/*****************************************************************************/
endmod() { /* _____________________________________________REMOVES_MODULE___ */
/*****************************************************************************/
	free( bcb.buf );
	free( s );
	_close( modh );
	}
/*****************************************************************************/
loadbuf( long fromwhere ) { /* ______________________________LOADS_BUFFER___ */
/*****************************************************************************/
	lseek( modh,fromwhere,SEEK_SET );
	_read( modh,bcb.buf,BCBSIZ );
	bcb.fst = fromwhere;
	bcb.lst = fromwhere + BCBSIZ - 1;
	bcb.cur = bcb.buf;
	}
/*****************************************************************************/
char getchr( long offset ) { /* ___________RECEIVES_CHARACTER_FROM_BUFFER___ */
/*****************************************************************************/
	if( offset >= bcb.fst && offset <= bcb.lst )
		return( *(( char * )( bcb.buf + offset - bcb.fst )) );
	else {
		loadbuf( offset );
		return( *(( char * )( bcb.buf )) );
		}
	}
/*****************************************************************************/
int getint( long offset ) { /* _________________RECEIVES_WORD_FROM_BUFFER___ */
/*****************************************************************************/
	if( offset >= bcb.fst && offset <= bcb.lst - 1 )
		return( *( ( int * )( bcb.buf + offset - bcb.fst ) ) );
	else {
		loadbuf( offset );
		return( *( ( int * )( bcb.buf ) ) );
		}
	}
/*****************************************************************************/
long getlong( long offset ) { /* _______________RECEIVES_LONG_FROM_BUFFER___ */
/*****************************************************************************/
	if( offset >= bcb.fst && offset <= bcb.lst - 3 )
		return( *( ( long * )( bcb.buf + offset - bcb.fst ) ) );
	else {
		loadbuf( offset );
		return( *( ( long * )( bcb.buf ) ) );
		}
	}
/*****************************************************************************/
fillattw() { /* _______________________________DISPLAYS_ATTRIBUTES_WINDOW___ */
/*****************************************************************************/
	int i;
	window( 2,4,60,15 ); textattr( 0x0e ); clrscr(); initatt_at();
	sw_dye_ch( 62,3,63,14,32 ); sw_dye_ch( 67,3,70,14,32 );
	sw_dye_ch( 74,3,77,14,32 );
	for( i=0;i<acb.atn && i<12;i++ ) putatt( i + 1,i + acb.up );
	mrkattcs();
	if( actw ) window( 2,18,79,23 ); else	window( 2,4,79,15 );
	}
/*****************************************************************************/
fillvalw( int attn ) { /* ________DISPLAYS_VALUE_WINDOW_OF_ATTRIBUTE_attn___ */
/*****************************************************************************/
	int i;
	vcb.vln = getint ( 34L + 10L * (long) attn );
	fillvalofs( acb.csv );
	window( 2,18,56,23 ); textattr( 0x0e ); clrscr(); initval_at();
	sw_dye_ch( 57,17,59,22,32 );                              /* clear fct col */
	sw_dye_ch( 61,17,64,22,32 );                              /* clear inf col */
	sw_dye_ch( 67,17,68,22,32 ); sw_dye_ch( 74,17,75,22,32 );
	for( i=0; i<vcb.vln && i<6; i++ ) putval( i + 1,i + vcb.up );
	mrkvalcs();
	if( actw ) window( 2,18,79,23 ); else	window( 2,4,79,15 );
	}
/*****************************************************************************/
fillvalofs( int attn ) { /* ________________________________FILLS_vcb.ofs___ */
/*****************************************************************************/
	int  j,k;
	long i;
	i = getlong ( 36L + 10L * (long) attn );
	for ( j=0; j < vcb.vln; j++ ) { while( getchr(i++) ); vcb.ofs[j] = i; }
	}
/*****************************************************************************/
putatt( int strn,int attn ) { /* _________________DISPLAYS_ATTRIBUTE_attn___ */
/*****************************************************************************/
	int  j=0;
	long i;
	char str[4];
	i = getlong ( 36L + 10L * (long) attn );
	itoa( attn, str, 10 );
	sw_puts( 1, strn+2, str, 0x07 );
	while( (scr[strn+2][5+j++].ch = getchr(i++)) != 0 );
	if( s[attn].mrk.inf ) {
		scr[strn+2][62].ch = 17; scr[strn+2][63].ch = 196;                 /* <Ä */
		}
	putattexp( attn ); putatthyp( attn );
	scr[strn+2][60].ch = scr[strn+2][65].ch = scr[strn+2][72].ch = 179;  /*  ³ */
	}
/*****************************************************************************/
putval( int strn,int valn ) { /*_____DISPLAYS_VALUE_valn_OF_ACTIVE_ATTRIBUTE_*/
/*****************************************************************************/
	int  j=0;
	long i;
	char str[4];

	i = vcb.ofs[valn];
	itoa( valn,str,10 );
	sw_puts( 1,strn+16,str,0x07 );
	while( (scr[strn+16][4+j++].ch = getchr(i++)) != 0 );    /* the value name */

  if( s[acb.csv].ffct & fmask_h[valn] )                      /* draw fct col */
  	if( s[acb.csv].ffct & fmask_l[valn] ) {
			scr[strn+16][57].ch =                                  /* 11: 3 fishky */
      scr[strn+16][58].ch =
      scr[strn+16][59].ch = 4;
      }
    else {
    	scr[strn+16][57].ch =                                  /* 10: 2 fishky */
      scr[strn+16][58].ch = 4;
      scr[strn+16][59].ch = 32;
      }
  else
		if( s[acb.csv].ffct & fmask_l[valn] ) {
    	scr[strn+16][57].ch = 4;                               /* 01: 1 fishku */
      scr[strn+16][58].ch =
      scr[strn+16][59].ch = 32;
      }
    else {
    	scr[strn+16][57].ch =                                  /* 00: 0 fishek */
      scr[strn+16][58].ch =
      scr[strn+16][59].ch = 32;
      }

  if( s[acb.csv].finf & fmask_h[valn] )                      /* draw inf col */
  	if( s[acb.csv].finf & fmask_l[valn] ) {
			scr[strn+16][61].ch =                                  /* 11: 3 fishky */
      scr[strn+16][62].ch =
      scr[strn+16][63].ch = 4;
      }
    else {
    	scr[strn+16][61].ch =                                  /* 10: 2 fishky */
      scr[strn+16][62].ch = 4;
      scr[strn+16][63].ch = 32;
      }
  else
		if( s[acb.csv].finf & fmask_l[valn] ) {
    	scr[strn+16][61].ch = 4;                               /* 01: 1 fishku */
      scr[strn+16][62].ch =
      scr[strn+16][63].ch = 32;
      }
    else {
    	scr[strn+16][61].ch =                                  /* 00: 0 fishek */
      scr[strn+16][62].ch =
      scr[strn+16][63].ch = 32;
      }

	putvalexp( valn ); putvalhyp( valn );
	scr[strn+16][56].ch = scr[strn+16][60].ch = \
	scr[strn+16][65].ch = scr[strn+16][72].ch = 179;                     /*  ³ */
	}
/*****************************************************************************/
putvalfctcol() { /* _________________________DISPLAYS_FACT_COLOMN_IN_valw___ */
/*****************************************************************************/
	int i;
	for( i=vcb.up;i<vcb.up+6 && i<vcb.vln;i++ ) {
	  if( s[acb.csv].ffct & fmask_h[i] )                       /* draw fct col */
  		if( s[acb.csv].ffct & fmask_l[i] ) {
				scr[i-vcb.up+17][57].ch =                            /* 11: 3 fishky */
	      scr[i-vcb.up+17][58].ch =
  	    scr[i-vcb.up+17][59].ch = 4;
	      }
  	  else {
    		scr[i-vcb.up+17][57].ch =                            /* 10: 2 fishky */
	      scr[i-vcb.up+17][58].ch = 4;
  	    scr[i-vcb.up+17][59].ch = 32;
    	  }
	  else
			if( s[acb.csv].ffct & fmask_l[i] ) {
    		scr[i-vcb.up+17][57].ch = 4;                         /* 01: 1 fishku */
      	scr[i-vcb.up+17][58].ch =
	      scr[i-vcb.up+17][59].ch = 32;
  	    }
    	else {
	    	scr[i-vcb.up+17][57].ch =                            /* 00: 0 fishek */
  	    scr[i-vcb.up+17][58].ch =
    	  scr[i-vcb.up+17][59].ch = 32;
      	}
    }
	}
/*****************************************************************************/
putattinfcol() { /* ____________________DISPLAYS_INFERENCE_COLOMN_IN_attw___ */
/*****************************************************************************/
	int i;
	for( i=acb.up;i<acb.up+12 && i<acb.atn;i++ ) {
		scr[i-acb.up+3][62].ch = ( s[i].mrk.inf ? 17  : 0 );                /* < */
		scr[i-acb.up+3][63].ch = ( s[i].mrk.inf ? 196 : 0 );                /* Ä */
		}
	}
/*****************************************************************************/
putvalinfcol() { /* ____________________DISPLAYS_INFERENCE_COLOMN_IN_valw___ */
/*****************************************************************************/
	int i;
	for( i=vcb.up; i<vcb.up+6 && i<vcb.vln; i++ ) {
	  if( s[acb.csv].finf & fmask_h[i] )                       /* draw inf col */
  		if( s[acb.csv].finf & fmask_l[i] ) {
				scr[i-vcb.up+17][61].ch =                            /* 11: 3 fishky */
      	scr[i-vcb.up+17][62].ch =
	      scr[i-vcb.up+17][63].ch = 4;
  	    }
    	else {
    		scr[i-vcb.up+17][61].ch =                            /* 10: 2 fishky */
	      scr[i-vcb.up+17][62].ch = 4;
  	    scr[i-vcb.up+17][63].ch = 32;
    	  }
	  else
			if( s[acb.csv].finf & fmask_l[i] ) {
    		scr[i-vcb.up+17][61].ch = 4;                         /* 01: 1 fishku */
      	scr[i-vcb.up+17][62].ch =
	      scr[i-vcb.up+17][63].ch = 32;
  	    }
    	else {
    		scr[i-vcb.up+17][61].ch =                            /* 00: 0 fishek */
	      scr[i-vcb.up+17][62].ch =
  	    scr[i-vcb.up+17][63].ch = 32;
    	  }
    }
	}
/*****************************************************************************/
putattexpcol() { /* ______________________DISPLAYS_EXPLAIN_COLOMN_IN_attw___ */
/*****************************************************************************/
	int i;
	sw_dye_ch( 67,3,70,14,32 );
	for( i=acb.up;i<acb.atn && i<acb.up+12;i++ ) putattexp(i);
	}
/*****************************************************************************/
putvalexpcol() { /* ______________________DISPLAYS_EXPLAIN_COLOMN_IN_valw___ */
/*****************************************************************************/
	int i;
	sw_dye_ch( 67,17,68,22,32 );
	for( i=vcb.up;i<vcb.vln && i<vcb.up+6;i++ ) putvalexp(i);
	}
/*****************************************************************************/
putatthypcol() { /* ___________________DISPLAYS_HYPOTHESIS_COLOMN_IN_attw___ */
/*****************************************************************************/
	int i;
	sw_dye_ch( 74,3,77,14,32 );
	for( i=acb.up;i<acb.atn && i<acb.up+12;i++ ) putatthyp(i);
	}
/*****************************************************************************/
putvalhypcol() { /* ___________________DISPLAYS_HYPOTHESIS_COLOMN_IN_valw___ */
/*****************************************************************************/
	int i;
	sw_dye_ch( 74,17,75,22,32 );
	for( i=vcb.up;i<vcb.vln && i<vcb.up+6;i++ ) putvalhyp(i);
	}
/*****************************************************************************/
putattexp( int attn ) { /* _____DISPLAYS_EXPLAIN_POSITION_IN_attw_FOR_attn__ */
/*****************************************************************************/
	int x = attn - acb.up + 3;
	if( attn >= fstexp && attn <= lstexp )
		if( s[attn].mrk.exp ) {
			if( attn == resatt ){ scr[x][67].ch = 17; scr[x][68].ch = 196;}  /* <Ä */
			else                { scr[x][67].ch = 196;scr[x][68].ch = 16; }  /* -> */
			scr[x][69].ch = 196;                                             /*  Ä */
			if( attn == fstexp )      scr[x][70].ch = 191;                   /*  ¿ */
			else if( attn == lstexp ) scr[x][70].ch = 217;                   /*  Ù */
					 else                 scr[x][70].ch = 180; 		               /*  ´ */
			}
		else                        scr[x][70].ch = 179;                   /*  ³ */
	}
/*****************************************************************************/
putvalexp( int valn ) { /* _____DISPLAYS_EXPLAIN_POSITION_IN_valw_FOR_valn__ */
/*****************************************************************************/
	int x;
	x = valn - vcb.up + 17;
	if( !(s[acb.csv].mrk.exp & (1<<valn)) )
		if( acb.csv == resatt ) {
			scr[x][67].ch = 17; scr[x][68].ch = 196;                         /* <Ä */
			}
		else {
			scr[x][67].ch = 4; scr[x][68].ch = 32;                        /* fishk */
			}
	}
/*****************************************************************************/
putatthyp( int attn ) { /* __DISPLAYS_HYPOTHESIS_POSITION_IN_attw_FOR_attn__ */
/*****************************************************************************/
	int x = attn - acb.up + 3;
	if( attn >= fsthyp && attn <= lsthyp )
		if( s[attn].mrk.hyp ) {
			if( attn == goalatt ){ scr[x][74].ch = 17; scr[x][75].ch = 196;} /* <Ä */
			else                 { scr[x][74].ch = 196;scr[x][75].ch = 16; } /* -> */
			scr[x][76].ch = 196;                                             /*  Ä */
			if( fsthyp != lsthyp )
				if( attn == fsthyp )      scr[x][77].ch = 191;                 /*  ¿ */
				else if( attn == lsthyp ) scr[x][77].ch = 217;                 /*  Ù */
						 else                 scr[x][77].ch = 180;                 /*  ´ */
			}
		else                          scr[x][77].ch = 179;                 /*  ³ */
	}
/*****************************************************************************/
putvalhyp( int valn ) { /* __DISPLAYS_HYPOTHESIS_POSITION_IN_valw_FOR_valn__ */
/*****************************************************************************/
	int x;
	x = valn - vcb.up + 17;
	if( !(s[acb.csv].mrk.hyp & (1<<valn)) )
		if( acb.csv == goalatt ) {
			scr[x][74].ch = 17; scr[x][75].ch = 196;                      
			}                                                                /* <Ä */
		else {
			scr[x][74].ch = 4; scr[x][75].ch = 32;                        /* fishk */
			}
	}
/*****************************************************************************/
infstr( int strn ) { /* _______________INFERENCE_ON_STRING_strn_OF_MATRIX___ */
/*****************************************************************************/
	int i,j,k=0,maxcomp=0;
	for( j=0;j<acb.atn;j++ )                                   /* section loop */
		if( s[j].fct & getint ( mofs + 2L * (long) acb.atn * (long) strn + 2L * (long) j ) )
			if( !k ) { k=1; i=j; }
			else return( 1 );            /* exists more then one non zero sections */
	if( k ) {                              /* exists the only non zero section */
  	s[i].mrk.inf = getint ( mofs + 2L * (long)acb.atn * (long)strn + 2L * (long)i );
		s[i].inf &= getint ( mofs + 2L * (long)acb.atn * (long)strn + 2L * (long)i );
    maxcomp = max_comp( strn, i );
    set_finf( strn, i, maxcomp );
    return( 1 );
    }
	else return( 0 );                                         /* contradiction */
	}

/*****************************************************************************/
max_comp( int strn, int secn ) { /* finds value of maximum component in vector */
/* s[].ffct corresponding to 1 in line strn of matrix excluding section secn */
/*****************************************************************************/
	int i,j,k;
  unsigned long max=0, cur=0;
  for( i=0; i<acb.atn; i++ ) {
  	if( i != secn ) {
    	k = getint ( mofs + 2L * (long)acb.atn * (long)strn + 2L * (long)i );
			for( j=0; j<16; j++ ) {
      	if( (1<<j) & k ) {
        	cur = (s[i].ffct>>(2*j)) & fmask[0];
          if( cur > max ) max = cur;
        	}
        }
    	}
  	}
  return( (int)max );
	}
/*****************************************************************************/
set_finf( int strn, int secn, int max ) {
/*****************************************************************************/
	int j,k;
  unsigned long m;

  m = (long)max;
	k = getint ( mofs + 2L * (long)acb.atn * (long)strn + 2L * (long)secn );
	for( j=0; j<16; j++ ) {
    if( !((1<<j) & k) ) {
      if( (s[secn].finf & fmask[j]) > (m<<2*j) ) {
        s[secn].finf &= ~fmask[j];
        s[secn].finf |= (m<<2*j);
      	}
    	}
		}
	}
/*****************************************************************************/
inference() { /*______________________________________INFERENCE_ON_MATRIX___ */
/*****************************************************************************/
	int i,k=0;
	for( i=0;i<acb.atn;i++ ) {
		s[i].inf = s[i].fct;
    s[i].finf = s[i].ffct;
    s[i].mrk.inf = 0;
    }
	thinking( 0 ); contr = 0;
	for( i=0; i<mstrn; i++ )                      /* loop on strings of matrix */
		if( infstr( i ) ) band_incr( i );
		else { contr = 1; break; }                              /* contradiction */
	if( !contr ) {                                                  /* success */
		for( i=0; i<acb.atn; i++ )
			if( s[i].mrk.inf ) k = 1;
		if( !k ) {                              /* unsufficient quantity of data */
			thinking_mes( 1 );
      putattexpcol(); putvalexpcol();
			putvalfctcol(); putattinfcol(); putvalinfcol();
			return;
			}
		for( i=0; i<acb.atn; i++ )	  if( s[i].mrk.inf ) { fstinf = i; break; };
		for( i=acb.atn-1; i>=0; i-- )	if( s[i].mrk.inf ) { lstinf = i; break; };
		thinking_mes( 2 );											                      
		}
	else {                                                    /* contradiction */
		for( i=0; i<acb.atn; i++ ) {
			 s[i].inf = s[i].fct;
       s[i].finf = s[i].ffct;
			 s[i].mrk.inf = 0;
			 }
		fstinf = acb.atn - 1; lstinf = 0;
		thinking_mes( 0 );
		}
	fstexp = acb.atn - 1; lstexp = 0;
	putattexpcol(); putvalexpcol();
	putvalfctcol(); putattinfcol(); putvalinfcol();
	}
/*****************************************************************************/
expstr( int strn ) { /* _____________EXPLANATION_ON_STRING_strn_OF_MATRIX___ */
/*****************************************************************************/
	int i,k;
	if( (k = getint (mofs + 2L * (long)acb.atn * (long)strn + 2L * (long)acb.csv)) == 0 ) return( 0 );
	if(  k & ( 1<<vcb.csv ) ) return( 0 );
	for( i=0;i<acb.atn;i++ )
		if( i != acb.csv )
			if( s[i].fct & getint ( mofs + 2L * (long)acb.atn * (long)strn + 2L * (long)i ) ) return( 0 );
	return( 1 );
	}
/*****************************************************************************/
expcontrstr( int strn ) { /* _____EXPLANATION_CONTR_ON_STRING_strn_OF_MATRIX_*/
/*****************************************************************************/
	int i;
	for( i=0;i<acb.atn;i++ )
			if( s[i].fct & getint ( mofs + 2L * (long)acb.atn * (long)strn + 2L * (long)i ) ) return( 0 );
	return( 1 );
	}
/*****************************************************************************/
explain( int direct,int strn ) { /* ________________EXPLANATION_ON_MATRIX___ */
/*****************************************************************************/
	int i,j;
	thinking( 1 );
	if( !contr ) {
		if( s[acb.csv].inf & (1<<vcb.csv) ) {
			thinking_mes( 3 ); return;                       /* nothing to explain */
			}
		if( !(s[acb.csv].fct & (1<<vcb.csv)) ) {
			thinking_mes( 4 ); return;                                 /* the fact */
			}
		if( direct )
			for( i=strn+1;i<mstrn;i++ ) {     						  	   /* forward search */
				band_incr( i ); if( expstr( i ) ) break;
				}
		else
			for( i=strn-1;i>=0;i-- ) {                              /* back search */
				band_decr( i ); if( expstr( i ) ) break;
				}
		}
	else {
		if( direct )
			for( i=strn+1;i<mstrn;i++ ) {     						  	   /* forward search */
				band_incr( i ); if( expcontrstr( i ) ) break;
				}
		else
			for( i=strn-1;i>=0;i-- ) {                              /* back search */
				band_decr( i ); if( expcontrstr( i ) ) break;
				}
		}
	if( i == -1 || i == mstrn ) {
		thinking_mes( 5 ); return;                     /* no others explanations */
		}
	curexp = i;
	for( j=0; j<acb.atn; j++ )
		s[j].mrk.exp = getint ( mofs + 2L * (long)acb.atn * (long)i + 2L * (long)j );
	if( !contr ) {
		s[acb.csv].mrk.exp = ~getint ( mofs + 2L * (long)acb.atn * (long)i + 2L * (long)acb.csv );
		resatt = acb.csv;
		}
	else resatt = -1;
	for( j=0;j<=acb.atn-1;j++ ) if( s[j].mrk.exp ) { fstexp = j; break; };
	for( j=acb.atn-1;j>=0;j-- ) if( s[j].mrk.exp ) { lstexp = j; break; };
	thinking_mes( 6 );                                            /* success */
	putattexpcol(); putvalexpcol();
	}
/*****************************************************************************/
hypstr( int strn ) { /* ______________HYPOTHESIS_ON_STRING_strn_OF_MATRIX___ */
/*****************************************************************************/
	int i,k;
	if( (k = getint ( mofs + 2L * (long)acb.atn * (long)strn + 2L * (long)goalatt )) == 0 )
		return( 0 );
	if(  k & ( 1<<goalval ) )
		return( 0 );
	return( 1 );
	}
/*****************************************************************************/
hypothesis( int direct,int strn ) { /* ______________HYPOTHESIS_ON_MATRIX___ */
/*****************************************************************************/
	int i,j,k=0;
	thinking( 2 );
	if( !(s[goalatt].fct & (1<<goalval)) ) {
		thinking_mes( 8 ); return;                                   /* the fact */
		}
	if( !(s[goalatt].inf & (1<<goalval)) ) {
		thinking_mes( 7 ); return;                         /* is infered already */
		}
	if( direct )
		for( i=strn+1;i<mstrn;i++ ) {   								  	   /* forward search */
			band_incr( i ); if( hypstr( i ) ) break;
			}
	else
		for( i=strn-1;i>=0;i-- ) {                                /* back search */
			band_decr( i ); if( hypstr( i ) ) break;
			}
	if( i == -1 || i == mstrn ) {
		thinking_mes( 9 ); return;                       /* no others hypothesis */
		}
	curhyp = i;
	for( j=0;j<acb.atn;j++ )
		if( (s[j].mrk.hyp = s[j].fct & getint ( mofs + 2L * (long)acb.atn * (long)i + 2L * (long)j )) != 0 )
			k = 1;
	if( !k ) {                                                /* contradiction */
		contr = 1;
    for( i=0;i<acb.atn;i++ ) { s[i].inf = s[i].fct; s[i].mrk.inf = 0; }
		fsthyp = acb.atn - 1; lsthyp = 0; thinking_mes( 0 );
		putatthypcol(); putvalhypcol();
		putattinfcol(); putvalinfcol();
		return;
		}
	s[goalatt].mrk.hyp = ~getint ( mofs + 2L * (long)acb.atn * (long)i + 2L * (long)goalatt );
	for( j=0;j<=acb.atn-1;j++ )	if( s[j].mrk.hyp ) { fsthyp = j; break; };
	for( j=acb.atn-1;j>=0;j-- ) if( s[j].mrk.hyp ) { lsthyp = j; break; };
	thinking_mes( 10 );                                            /* success */
	putatthypcol(); putvalhypcol();
	}
/*****************************************************************************/
err_mes( int err_n ) {  /*_DRAWS_err_w_AND_YIELDS_MESSAGE_ABOUT_ERROR_errn___*/
/*****************************************************************************/
	int i;
	putch( 7 );
	sw_Store( err_w ); sw_draw( &err_w,&err_wc );
	gotoxy( 4,2 ); cputs( err_entr[err_n] );
	gotoxy( 39,7 ); cputs( "Press " );
	textattr( err_wc.hc ); cputs( "ESC" ); textattr( att_wc.bc );
	do {
		if( ESCKEY == bioskey( 0 ) ) {
			sw_Restore( err_w );
      if( actw ) window( 2,18,79,23 ); else	window( 2,4,79,15 );
			return;
			}
		} while( 1 );
	}
/*****************************************************************************/
thinking( int think_n ) {  /* _______________DRAW_THINKING_WINDOW_think_w___ */
/*****************************************************************************/
	sw_Store( think_w ); sw_draw( &think_w,&think_wc );
	gotoxy( 4,7 ); cputs( think_entr[ think_n ] );
	sw_dye_at( 23,15,56,15,0x22 ); sw_dye_ch( 23,15,56,15,178 );          /* ² */
	}
/*****************************************************************************/
thinking_mes( int think_n ) {  /* ______YIELDS_MESSAGE_think_mes[think_n]___ */
/*****************************************************************************/
	gotoxy( 28,10 ); cputs( "Press " );
	gotoxy( 2,2 ); textattr( 0x1c ); cputs( think_mes[ think_n ] );
	gotoxy( 34,10 ); textattr( think_wc.hc ); cputs( "ESC" );
	textattr( att_wc.bc );
	do {
		if( ESCKEY == bioskey( 0 ) ) {
			sw_Restore( think_w );
      if( actw ) window( 2,18,79,23 ); else	window( 2,4,79,15 );
			return;
			}
		} while( 1 );
	}
/*****************************************************************************/
yes_no( int yn_n ) {  /*__DRAWS_YES/NO_WINDOW_yn_w_AND_YIELDS_MESSAGE_yn_n___*/
/*****************************************************************************/
	int k;
	sw_Store( yn_w ); sw_draw( &yn_w,&yn_wc );
	gotoxy( 2,2 ); cputs( yn_entr[ yn_n ] );
	textattr( yn_wc.hc );
	gotoxy( 32,2 ); putch('Y'); gotoxy( 37,2 ); putch('N');
	textattr( att_wc.bc );
  if( actw ) window( 2,18,79,23 ); else	window( 2,4,79,15 );
	do {
		k = bioskey( 0 );
		switch( k ) {
			case ESCKEY :	sw_Restore( yn_w );	return( -1 );
			case Y_KEY  :
			case SHIFT_Y: sw_Restore( yn_w );	return( 1 );
			case N_KEY  :
			case SHIFT_N: sw_Restore( yn_w );	return( 0 );
			}
		} while( 1 );
	}
/*****************************************************************************/
band_incr( int i ) {  /* ___DISPLAY_INCR._BAND_OF_INF._PROCESS_IN_think_w___ */
/*****************************************************************************/
	int j,l,k;                                
	j = 34*(long)(i+1) / mstrn; l = 34*(long)i / mstrn;
	if( j != l ) {
		for( k=l+1; k<=j; k++ ) scr[15][k+22].at = 0x04;
	  if( env.sound ) { sound(20); delay(1); nosound(); }
    }
	delay( 800 / mstrn );
	}
/*****************************************************************************/
band_decr( int i ) {  /* ___DISPLAY_DECR._BAND_OF_INF._PROCESS_IN_think_w___ */
/*****************************************************************************/
	int j,l,k;
	j = 34*(long)(i+1) / mstrn; l = 34*(long)i / mstrn;
	if( j != l ) {
		for( k=j; k>=l+1; k-- ) scr[15][k+22].at = 0x04;
    if( env.sound ) { sound(20); delay(1); nosound(); }
    }
	delay( 800 / mstrn );
	}
/*****************************************************************************/
showgoal() {  /* __DISPLAYS_GOAL_ATTRIBUTE_goalatt_AND_GOAL_VALUE_goalval___ */
/*****************************************************************************/
	char stratt[4],strval[3];
	itoa( goalatt,stratt,10 ); itoa( goalval,strval,10 );
	sw_dye_ch( 14,1,17,1,32 ); sw_puts( 14,1,stratt,0x07 );
	sw_dye_ch( 24,1,26,1,32 ); sw_puts( 24,1,strval,0x07 );
	}
/*****************************************************************************/
showbase() { /* _DISPLAYS_CURRENT_NAME_OF_MODULE_basename_AND_DRIVE_drive___ */
/*****************************************************************************/
	char drive[3],name[9],ext[5];
	fnsplit( curbase,drive,NULL,name,ext );
	if( !strcmp( drive,"" ) ) {
		drive[0] = change_s[0]; drive[1] = change_s[1]; drive[2] = 0x00;
		}
	fnmerge( basename,NULL,NULL,name,ext );
	sw_dye_ch( 39,1,52,1,32 );
	sw_puts( 39,1,drive,0x07 ); sw_puts( 41,1,basename,0x07 );
	}
/*****************************************************************************/
showcon() { /* ___DISPLAYS_CURRENT_NAME_OF_CONS-N_conname_AND_DRIVE_drive___ */
/*****************************************************************************/
	char drive[3],name[9],ext[5];
	fnsplit( curcon,drive,NULL,name,ext );
	if( !strcmp( drive,"" ) ) {
		drive[0] = change_s[0]; drive[1] = change_s[1]; drive[2] = 0x00;
		}
	fnmerge( conname,NULL,NULL,name,ext );
	sw_dye_ch( 66,1,79,1,32 );
	sw_puts( 66,1,drive,0x07 ); sw_puts( 68,1,conname,0x07 );
	}
/*****************************************************************************/
findfiles( char *destin,char *sourse ) { /* ________FINDS_FILES_BY_TEMPLATE_ */
/*****************************************************************************/
	char   *fn_entr[52],drive[3],dir[66],name[9],ext[5];
	int    i,filesn = 0;
	struct ffblk ffblk;
	i = findfirst( sourse,&ffblk,0 );
	while( !i && filesn<52 ) {
		fn_entr[filesn] = (char *) malloc( 13 );
		strcpy( fn_entr[filesn++],ffblk.ff_name );
		i = findnext( &ffblk );
		}
	if( !filesn ) {	err_mes( 2 ); return( 0 );	}             /* no such files */
	sw_Store( fname_w );
	fname_m.cs = 0; fname_m.en = filesn; fname_m.entr = fn_entr;
	i = sw_menu( &fname_w,&str_wc,&fname_m ); sw_Restore( fname_w );
	if( i<0 ) { for( i=0;i<filesn;i++ ) free( fn_entr[i] ); return(0); }/* ESC */
	fnsplit( sourse,drive,dir,NULL,NULL ); /* select drive and dir from sourse */
	fnsplit( fn_entr[i],NULL,NULL,name,ext );           /* select name and ext */
	fnmerge( destin,drive,dir,name,ext );   /* compose full path and return it */
	for( i=0;i<filesn;i++ ) free( fn_entr[i] );
	return( 1 );
	}
/*****************************************************************************/
validmod( char *path ) {  /* _________________________VERIFYS_MODULE_FILE___ */
/*****************************************************************************/
	char ext[5];
	int  hand;
	fnsplit( path,NULL,NULL,NULL,ext );
	if( ext[1] != 'M' || ext[2] != 'O' || ext[3] != 'D' ) {
		err_mes( 3 ); return( 0 );                 /* ivalid file name extension */
		}
	if( ( hand = _open( path,O_RDONLY ) ) == -1 ) {
		err_mes( 0 ); return( 0 );                          /* can not open file */
		}
	lseek( hand,0,SEEK_SET );
	_read( hand,ext,5 );
	if( ext[0] != 'E' || ext[1] != 'D' || ext[2] != 'I' || ext[3] != 'P' ) {
		_close( hand ); err_mes( 4 ); return( 0 );          /* it is not EDIP kb */
		}
	if( ext[4] != 0x10 ) {
		_close( hand ); err_mes( 5 ); return( 0 );            /* invalid version */
		}
	_close( hand ); return( 1 );
	}
/*****************************************************************************/
validcon( char *path ) {  /* ___________________VERIFYS_CONSULTATION_FILE___ */
/*****************************************************************************/
	char ext[13];
	int  hand,i=0;
	fnsplit( path,NULL,NULL,NULL,ext );
	if( ext[1] != 'C' || ext[2] != 'O' || ext[3] != 'N' ) {
		err_mes( 3 ); return( 0 );                 /* ivalid file name extension */
		}
	if( ( hand = _open( path,O_RDONLY ) ) == -1 ) {
		err_mes( 0 ); return( 0 );                          /* can not open file */
		}
	lseek( hand,0,SEEK_SET );
	_read( hand,ext,5 );
	if( ext[0] != 'E' || ext[1] != 'D' || ext[2] != 'I' || ext[3] != 'P' ) {
		_close( hand ); err_mes( 6 ); return( 0 );          /* it is not EDIP db */
		}
	if( ext[4] != 0x10 ) {
		_close( hand ); err_mes( 5 ); return( 0 );            /* invalid version */
		}
	lseek( hand,6,SEEK_SET );
	_read( hand,ext,13 );
	while( ext[i] != 0 ) {
		if( ext[i] != basename[i] ) {
			_close( hand ); err_mes( 7 ); return( 0 );        /* invalid base name */
			}
		i++;
		}
	_close( hand ); return( 1 );
	}
/*****************************************************************************/
f10_key() {
/*****************************************************************************/
	int k,*txt;
	txt = ( int *)malloc( 80*sizeof( int ) ); sw_gettext( 1,1,80,1,txt );
	do {
		switch( sw_menu( &mainm_w,&mainm_wc,&mainm_m ) ) {
			case -1 : k = -1;      break;
			case  0 :
				if( ( k = alt_k() ) == -1 ) return( -1 );
			break;
			case  1 : k = alt_c(); break;
			case  2 : k = alt_i(); break;
			case  3 : k = alt_e(); break;
			case  4 : k = alt_h(); break;
			case  5 : alt_o();     break;
			}
		} while( k != -1 && k != 1 );
	sw_puttext( 1,1,80,1,txt ); free( txt );
	textattr( 0x0e );
	}
/*****************************************************************************/
alt_k() {
/*****************************************************************************/
	int k=0;
	sw_Store( know_w );
	do {
		switch( sw_menu( &know_w,&menu_wc,&know_m ) ) {
			case -1 : k = -1;                  break;
			case  0 : k = loadk();             break;
			case  1 : change_dir( changek_w ); break;
			case  2 : k = os_shell();     		 break;
			case  3 :
				if( env.cons ) save();
				if( env.conf ) write_config( "config.edp" );
				sw_Restore( all_scr );
				return( -1 );
			}
		} while( k != -1 && k != 1 && k != 2 );
	sw_Restore( know_w );
	if( k == 1 ) initmod( curbase );
	textattr( 0x0e );
	if( k != -1 ) return( 1 );
	}
/*****************************************************************************/
alt_c() {
/*****************************************************************************/
	int k=0;
	sw_Store( cons_w );
	do {
		switch( sw_menu( &cons_w,&menu_wc,&cons_m ) ) {
			case -1 : k = -1;                  break;
			case  0 : k = loadc();             break;
			case  1 : k = new_con();           break;
			case  2 : k = save();              break;
			case  3 : k = write_to();          break;
			case  4 : change_dir( changec_w ); break;
			}
		} while( k != -1 && k != 1 );
	sw_Restore( cons_w );
	textattr( 0x0e );
	if( k != -1 ) return( 1 );
	}
/*****************************************************************************/
alt_i() {
/*****************************************************************************/
	int i;
	sw_Store( infer_w );
	i = sw_menu( &infer_w,&menu_wc,&infer_m );
	sw_Restore( infer_w );
	switch( i ) {
		case 0 : inference(); break;
		case 1 : goal();      break;
		}
	textattr( 0x0e );
	if( i != -1 ) return( 1 );
	}
/*****************************************************************************/
alt_e() {
/*****************************************************************************/
	int i;
	sw_Store( expl_w );
	i = sw_menu( &expl_w,&menu_wc,&expl_m );
  sw_Restore( expl_w );
	switch( i ) {
		case 0  : explain( 1,-1);      break;
		case 1  : explain( 1,curexp ); break;
		case 2  : explain( 0,curexp ); break;
		case 3  : explain( 0,mstrn );  break;
		}
	textattr( 0x0e );
	if( i != -1 ) return( 1 );
	}
/*****************************************************************************/
alt_h() {
/*****************************************************************************/
	int i;
	sw_Store( hyp_w );
	i = sw_menu( &hyp_w,&menu_wc,&hyp_m);
  sw_Restore( hyp_w );
	switch( i ) {
		case 0  : hypothesis( 1,-1);      break;
		case 1  : hypothesis( 1,curhyp ); break;
		case 2  : hypothesis( 0,curhyp ); break;
		case 3  : hypothesis( 0,mstrn );  break;
		}
	textattr( 0x0e );
	if( i != -1 ) return( 1 );
	}
/*****************************************************************************/
alt_o() {
/*****************************************************************************/
	int k;
	sw_Store( opt_w );
	do {
	switch( sw_menu( &opt_w,&menu_wc,&opt_m ) ) {
		case -1 : k = -1;                       break;
		case  0 : envi();   		                break;
		case  1 : write_config( "config.edp" ); break;
		}
		} while( k != -1 );
	sw_Restore( opt_w );
	textattr( 0x0e );
	}
/*****************************************************************************/
envi() {
/*****************************************************************************/
	int k;
	sw_Store( env_w );
	do {
		if( env.conf ) env_entr[0] = "Config auto save   On";
		else           env_entr[0] = "Config auto save   Off";
		if( env.cons ) env_entr[1] = "Consult auto save  On";
		else           env_entr[1] = "Consult auto save  Off";
		if( env.bak  ) env_entr[2] = "Backup files       On";
		else           env_entr[2] = "Backup files       Off";
		switch( sw_menu( &env_w,&menu_wc,&env_m ) ) {
			case -1 : k = -1;                    break;
			case  0 : env.conf = ++env.conf % 2; break;
			case  1 : env.cons = ++env.cons % 2; break;
			case  2 : env.bak  = ++env.bak  % 2; break;
			}
		} while( k != -1 );
	sw_Restore( env_w );
	}
/*****************************************************************************/
loadk() {
/*****************************************************************************/
	char auxstr[80];
	if( ed_row( loadk_s,loadk_s,&loadk_w,&str_wc ) == -1 ) return( 0 );
	if( !findfiles( auxstr,loadk_s ) ) return( 0 );
	if( validmod( auxstr ) ) { strcpy( curbase,auxstr ); endmod(); return( 1 ); }
	}
/*****************************************************************************/
change_dir( w_descr change_w ) {
/*****************************************************************************/
	char auxstr[66];
	if( !ed_row( auxstr,change_s,&change_w,&str_wc ) )
		if( chdir( auxstr ) == -1 ) err_mes( 1 );              /* path not found */
		else getcwd( change_s,66 );
	}
/*****************************************************************************/
os_shell() {
/*****************************************************************************/
	w_descr cur_scr = { 1,1,80,25,0,"",0 };
	char    *comstr;
	sw_Store( cur_scr ); comstr = getenv( "COMSPEC" );
	cursor( 0x06,0x07 ); spawnl( P_WAIT,comstr,comstr,NULL );
	sw_Restore( cur_scr ); cursor( 0,0 );
	return( 2 );
	}
/*****************************************************************************/
loadc() {
/*****************************************************************************/
	char auxstr[80];
	if( ed_row( loadc_s,loadc_s,&loadc_w,&str_wc ) == -1 ) return( 0 );
	if( !findfiles( auxstr,loadc_s ) ) return( 0 );
	if( validcon( auxstr ) ) {
		strcpy( curcon,auxstr ); load_con( curcon ); return( 1 );
		}
	}
/*****************************************************************************/
new_con() {
/*****************************************************************************/
	int i;
	for( i=0;i<acb.atn;i++ ) {
		s[i].inf = s[i].fct = -1;
    s[i].finf = s[i].ffct = -1;
		s[i].mrk.inf = s[i].mrk.exp = s[i].mrk.hyp = 0;
		}
	fstinf = acb.atn - 1; lstinf = 0;
	fstexp = acb.atn - 1; lstexp = 0; curexp = 0;
	fsthyp = acb.atn - 1; lsthyp = 0; curhyp = 0;
	strcpy( curcon,"NONAME.CON" ); showcon();
	sw_dye_ch( 62,3, 63,14,32 ); sw_dye_ch( 67,3, 70,14,32 );
	sw_dye_ch( 74,3, 77,14,32 );
	sw_dye_ch( 62,17,63,22,32 ); sw_dye_ch( 67,17,68,22,32 );
	sw_dye_ch( 74,17,75,22,32 );
	putvalfctcol();
	return( 1 );
	}
/*****************************************************************************/
save() {
/*****************************************************************************/
	if( strcmp( conname,"NONAME.CON" ) != 0 ) save_con( curcon );
	else {
		if( ed_row( curcon,curcon,&save_w,&str_wc ) == -1 ) return( 0 );
		write_to_con( curcon );
		}
	return( 1 );
	}
/*****************************************************************************/
write_to() {
/*****************************************************************************/
	if( ed_row( curcon,writ_s,&writ_w,&str_wc ) == -1 ) return( 0 );
	write_to_con( curcon );
	strcpy( writ_s,"" );
	return( 1 );
	}
/*****************************************************************************/
save_con( char *pathcon ) {
/*****************************************************************************/
	char drive[3],dir[66],name[9],ext[5],pathconbak[80];
	if( env.bak ) {
		fnsplit( pathcon,drive,dir,name,ext );
		fnmerge( pathconbak,drive,dir,name,".BAK" );
		unlink( pathconbak );
		if( rename( pathcon,pathconbak ) == -1 ) {
			err_mes( 8 ); return;                       /* can not create BAK file */
			}
		}
	write_con( pathcon );
	}
/*****************************************************************************/
write_to_con( char *pathcon ) {
/*****************************************************************************/
	struct ffblk ffblk;
	if( !findfirst( pathcon,&ffblk,0 ) ) {               /* file alredy exists */
		if( yes_no( 0 ) ) {	write_con( pathcon ); showcon(); }     /* overwrite? */
		}
	else { write_con( pathcon ); showcon(); }                   /* create file */
	}
/*****************************************************************************/
load_con( char *pathcon ) {
/*****************************************************************************/
	int hand,*txt;
	txt = ( int *)malloc( 80*sizeof( int ) ); sw_gettext( 1,25,80,25,txt );
	sw_dye_ch( 0,24,79,24,32 );
	sw_puts( 1,24,"Loading Consultation File . . .",0x1c );
	if( ( hand = _open( pathcon,O_RDONLY ) ) == -1 ) {
		err_mes( 0 );                                       /* can not open file */
		sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
    return( 0 );
		}
	lseek( hand,22,SEEK_SET );
	_read( hand,&fstinf, sizeof( int ) );
	_read( hand,&lstinf, sizeof( int ) );
	_read( hand,&fstexp, sizeof( int ) );
	_read( hand,&lstexp, sizeof( int ) );
	_read( hand,&fsthyp, sizeof( int ) );
	_read( hand,&lsthyp, sizeof( int ) );
	_read( hand,&resatt, sizeof( int ) );
	_read( hand,&goalatt,sizeof( int ) );
	_read( hand,&goalval,sizeof( int ) );
	_read( hand,s,acb.atn*sizeof(statedescr) );
	_close( hand );
	putattinfcol(); putattexpcol(); putatthypcol();
	putvalfctcol(); putvalinfcol(); putvalexpcol(); putvalhypcol();
	showgoal(); showcon();
  sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
	}
/*****************************************************************************/
write_con( char *pathcon ) {
/*****************************************************************************/
	static char EDIP[6] = { 'E','D','I','P',0x20,0x00 };
	int         hand,*txt;
	txt = ( int *)malloc( 80*sizeof( int ) ); sw_gettext( 1,25,80,25,txt );
	sw_dye_ch( 0,24,79,24,32 );
	sw_puts( 1,24,"Saving Consultation File . . .",0x1c );
	if( ( hand = _creat( pathcon,FA_ARCH ) ) == -1 ) {
		err_mes( 0 );                                       /* can not open file */
		sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
    return;
		}
	_write( hand,EDIP,6 );
	_write( hand,basename,13 );
	lseek ( hand,20,SEEK_SET );
	_write( hand,&acb.atn,sizeof( int ) );
	_write( hand,&fstinf, sizeof( int ) );
	_write( hand,&lstinf, sizeof( int ) );
	_write( hand,&fstexp, sizeof( int ) );
	_write( hand,&lstexp, sizeof( int ) );
	_write( hand,&fsthyp, sizeof( int ) );
	_write( hand,&lsthyp, sizeof( int ) );
	_write( hand,&resatt, sizeof( int ) );
	_write( hand,&goalatt,sizeof( int ) );
	_write( hand,&goalval,sizeof( int ) );
	_write( hand,s,sizeof( statedescr )*acb.atn );
	_close( hand );
	sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
	}
/*****************************************************************************/
load_config( char *pathcon ) {
/*****************************************************************************/
	int  hand,*txt;
	txt = ( int *)malloc( 80*sizeof( int ) ); sw_gettext( 1,25,80,25,txt );
	sw_dye_ch( 0,24,79,24,32 );
	sw_puts( 1,24,"Loading Configuration File . . .",0x1c );
	if( ( hand = _open( pathcon,O_RDONLY ) ) == -1 ) {
		err_mes( 9 );                         /* can not open configuration file */
		sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
    return( -1 );
		}
	lseek( hand,6,SEEK_SET );  _read( hand,curbase,80 );
	_read( hand,&env.conf,sizeof( short ) );
	_read( hand,&env.cons,sizeof( short ) );

	_read( hand,&env.bak, sizeof( short ) );
	_close( hand );
	sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
	}
/*****************************************************************************/
write_config( char *pathcon ) {
/*****************************************************************************/
	int  hand,*txt;
  static char EDIP[6] = { 'E','D','I','P',0x10,0x00 };
	txt = ( int *)malloc( 80*sizeof( int ) ); sw_gettext( 1,25,80,25,txt );
	sw_dye_ch( 0,24,79,24,32 );
	sw_puts( 1,24,"Saving Configuration File . . .",0x1c );
	rest_curbase();
	if( ( hand = _creat( pathcon,FA_ARCH ) ) == -1 ) {
		err_mes( 10 );                       /* can not creat configuration file */
		sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
    return;
		}
	_write( hand,EDIP,6 );
	_write( hand,curbase,80 );
	_write( hand,&env.conf,sizeof( short ) );
	_write( hand,&env.cons,sizeof( short ) );
	_write( hand,&env.bak, sizeof( short ) );
	_close( hand );
	sw_dye_ch( 0,24,79,24,32 ); sw_puttext( 1,25,80,25,txt ); free( txt );
	}
/*****************************************************************************/
rest_curbase() {  /* __RESTORES_A_FULL_PATH_OF_THE_CURRENT_MODULE_curbase___ */
/*****************************************************************************/
	char drive[3],dir[66],name[9],ext[5],curdrive[3],curdir[66];
	int  i = 0;
	fnsplit( curbase,drive,dir,name,ext );
	curdrive[0] = change_s[0]; curdrive[1] = change_s[1];	curdrive[2] = 0x00;
	while( ( curdir[i] = change_s[i+2] ) != 0 ) i++;
	if( i > 1 ) strcat( curdir,"\\" );
	if( !strcmp( drive,"" ) ) {
		strcpy( drive,curdrive );
		if( dir[0] != '\\' ) { strcat( curdir,dir ); strcpy( dir,curdir ); }
		}
	fnmerge( curbase,drive,dir,name,ext );
	}
/*****************************************************************************/
goal() {  /* __________________ESTABLISHES_AND_DISPLAYS_goalatt_&_goalval___ */
/*****************************************************************************/
	goalatt = acb.csv; goalval = vcb.csv;	showgoal();
	}
/*****************************************************************************/
version() {  /* ____________DISPLAYS_VERSION_IN_THE_VERSION_WINDOW_vers_w___ */
/*****************************************************************************/
	int i;
  char empty[] = "                                       ";
  char *lines[16] = {
  	"", "", "",
    "         Expert  System  Shell         ",
    "                  EDIP                 ",
    " User Interface Integrated Environment ",
    "              Version  2.1             ",
    "               1990-1993               ",
    "",
    "ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ",
    "       Institute of Mathematics        ",
    "    Artificial Intelligence Systems    ",
    "              Laboratory               ",
    " Academiei 5, 277028 Kishinev, Moldavia",
    "   Phone: (0422) 738130, A.A.Savinov   ",
    ""
    };

	sw_Store( vers_w ); sw_draw( &vers_w,&vers_wc );

  for( i=1; i<16; i++ ) {
	textattr( 0x00 );
    insline();
	textattr( 0x70 );
	gotoxy( 7,1 );
	cputs( empty );

    if( i == 12 )  textattr( vers_wc.hc );
	else if ( i < 7 ) textattr( 0x71 );
	else           textattr( 0x70 );

    gotoxy( 7,1 );  cputs( lines[16-i] );
    delay( 50 );
    }

	do {
		if( ESCKEY == bioskey( 0 ) ) {
			sw_Restore( vers_w );
      if( actw ) window( 2,18,79,23 ); else	window( 2,4,79,15 );
			return;
			}
		} while( 1 );
	}
/*****************************************************************************/
no_config() {  /* _____________DISABLES_SYSTEM_BEFORE_LOAD_OF_MODULE_FILE___ */
/*****************************************************************************/
	int k,i;
	env.conf = env.cons = env.bak = 1;
	do {
		k = bioskey( 0 );
		switch( k ) {
			case SHIFT_F10 : version(); break;
			case F3_KEY    :
				if( err_loadk() == 1 ) { initmod( curbase ); return( 0 ); }
			break;
			case ALT_K     :
				i = err_alt_k();
				if( i == -1 ) return( -1 ); if( i == 1 ) return( 0 );
			break;
			case ALT_X     : sw_Restore( all_scr ); return( -1 );
			}
		} while( 1 );
	}
/*****************************************************************************/
err_alt_k() {
/*****************************************************************************/
	int k=0;
	sw_Store( know_w );
	do {
		switch( sw_menu( &know_w,&menu_wc,&know_m ) ) {
			case -1 : k = -1;                   break;
			case  0 : k = err_loadk();          break;
			case  1 : change_dir( changek_w );  break;
			case  2 : k = os_shell();     		  break;
			case  3 : sw_Restore( all_scr );    return( -1 );
			}
		} while( k != -1 && k != 1 && k != 2 );
  sw_Restore( know_w );
	if( k == 1 ) initmod( curbase );
	textattr( 0x0e );
	return( 1 );
	}
/*****************************************************************************/
err_loadk() {
/*****************************************************************************/
	char auxstr[70];
	if( ed_row( loadk_s,loadk_s,&loadk_w,&str_wc ) == -1 ) return( 0 );
	if( !findfiles( auxstr,loadk_s ) ) return( 0 );
	if( validmod( auxstr ) ) { strcpy( curbase,auxstr ); return( 1 );	}
	}
/*****************************************************************************/
comment() { /* __________________DISPLAYS_COMMENT_TO_THE_CURRENT_ATTRIBUTE___*/
/*****************************************************************************/
	long comofs;
  char c;

	comofs = getlong( 34L + 10L * (long) acb.csv + 6L );
  if( comofs ) {
 		sw_Store( com_w ); sw_draw( &com_w,&com_wc );
		do {
  		putch( c=getchr( comofs++ ) );
    	} while( c );
    }
  else return;
	do {
		if( ESCKEY == bioskey( 0 ) ) {
			sw_Restore( com_w );
      if( actw ) window( 2,18,79,23 ); else	window( 2,4,79,15 );
			return;
			}
		} while( 1 );
  }
/*****************************************************************************/
help() { /*__________________________________________________DISPLAYS_HELP___*/
/*****************************************************************************/
	long helpofs = 0;
  char c;
  int  helph;

  if( actw == ATTW )                       /* help for the attribute window */
  	switch( acb.csh ) {
    	case 0 : helpofs = 0;
      case 1 : helpofs = 0;
      case 2 : helpofs = 0;
      case 3 : helpofs = 0;
      }
  else         helpofs = 0;                   /* help for the values window */

  if( helpofs ) {
    if( (helph = open( "edip_run.hlp", O_RDONLY ) ) == -1 ) {
    	err_mes( 0 );
      return( -1 );
      }
    lseek( helph, helpofs, SEEK_SET );
 		sw_Store( com_w ); sw_draw( &com_w,&help_wc );
		do {
    	read( helph, &c, 1 );
  		putch( c );
    	} while( c );
    }
  else return(-1);
	do {
		if( ESCKEY == bioskey( 0 ) ) {
			sw_Restore( com_w );
      if( actw ) window( 2,18,79,23 ); else	window( 2,4,79,15 );
			return(0);
			}
		} while( 1 );
  }
/*****************************************************************************/
sw_menuhelp( m_descr *m ) { /*_____________DISPLAYS_HELP_FOR_THE_MENU_ITEM___*/
/*****************************************************************************/
	long helpofs = 0;
  char c;
  int  helph;

  if( m->help &&                          /* if there is help for this menu */
  	  m->help[m->cs] ) {              /* and there is help for current item */
    helpofs = m->help[m->cs];
    }
  else return(-1);

  if( (helph = open( "edip_run.hlp", O_RDONLY ) ) == -1 ) {
  	err_mes( 0 );
    return( -1 );
    }
  lseek( helph, helpofs, SEEK_SET );
	sw_Store( com_w ); sw_draw( &com_w, &help_wc );
	do {
   	read( helph, &c, 1 );
 		putch( c );
   	} while( c );

	do {
		if( ESCKEY == bioskey( 0 ) ) {
			sw_Restore( com_w );
      if( actw ) window( 2,18,79,23 ); else	window( 2,4,79,15 );
			return(0);
			}
		} while( 1 );
  }
