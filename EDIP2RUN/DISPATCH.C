#include <string.h>
#include "sw_keys.h"
#include "sw.h"

#define  INFERENCE       F9_KEY
#define  EXPLAIN         F8_KEY
#define  HYPOTHESIS      F7_KEY
#define  GOAL            F6_KEY
#define  NOTKEY          DELKEY
#define  MAYBEKEY        INSKEY
#define  YESKEY          CRKEY
#define  RESET           ALT_R

extern   struct  attw_struct {
	int      atn;                /* a quantity of the attributes in the module */
	int      csv;   /* the attribute number, a vertical position of the cursor */
	int      csh;   /* a horizontal position of the cursor in the attr. window */
	int      up;    /* the number of the upper string in the attributes window */
	}      acb;
extern   struct  valw_struct {
	int      vln;                /* quantity of values of the active attribute */
	int      csv;                  /* the number of value, a vertical position */
	int      csh;  /* a horizontal position of the cursor in the values window */
	int      up;        /* the number of the upper string in the values window */
	long     ofs[16]; /* a displacements to the values of the active attribute */
	}      vcb;
extern   struct  state_struct {
	int      fct;			  		        		          						    /* the facts */
	int 		 inf;                                                 /* inference */
  long     ffct;
  long     finf;
					 struct  mrk_struct {
		int 	   inf;                                        /* inference marker */
		int    	 exp;                                      /* explanation marker */
		int      hyp;                                       /* hypothesis marker */
		}      mrk;
	}      *s;
extern   struct  environ_struct {
	short    sound;
	short    conf;                    /* config.edp auto save: 1 - On, 0 - Off */
	short    cons;             /* consultation file auto save: 1 - On, 0 - Off */
	short    bak;              /* create backup file variable: 1 - YES, 0 - NO */
	}      env;
extern char    change_s[66],curbase[80];
extern int     actw,curexp,curhyp,mstrn;
extern w_descr all_scr;

/*****************************************************************************/
dispatch() {  /* ______________________________________________DISPATCHER___ */
/*****************************************************************************/
	int k,i;
	do {
		k = bioskey( 0 );
		switch( k ) {
			case TABKEY       : tabkey();               break;    /* window switch */
			case LEFTKEY      : leftkey();              break;
			case RIGHTKEY     : rightkey();             break;
			case UPKEY        : upkey(); 	              break;
			case DOWNKEY      : downkey();	            break;
			case HOMEKEY      : homekey();	            break;
			case ENDKEY       : endkey();     	        break;
			case PGUPKEY      : pgupkey();	            break;
			case PGDNKEY      : pgdnkey();    	        break;
			case CTRL_PGUP    : ctrl_pgupkey();	        break;
			case CTRL_PGDN    : ctrl_pgdnkey();         break;
			case INFERENCE    : inference();	          break;
			case EXPLAIN      : explain( 1,-1 );        break;
			case CTRL_F8      : explain( 1,curexp );    break;
			case ALT_F8       : explain( 0,curexp );    break;
			case SHIFT_F8     : explain( 0,mstrn );     break;
			case HYPOTHESIS   : hypothesis( 1,-1 );     break;
			case CTRL_F7      : hypothesis( 1,curhyp ); break;
			case ALT_F7       : hypothesis( 0,curhyp ); break;
			case SHIFT_F7     : hypothesis( 0,mstrn );  break;
			case GOAL         : goal();            	    break;
			case F2_KEY       : save();                 break;
			case F3_KEY       :
				if( loadk() ) initmod( curbase );
			break;
			case F4_KEY       : loadc();                break;
			case SHIFT_F10    : version();              break;
			case F10_KEY      :
				if( f10_key() == -1 ) return;       			break;
			case ALT_K        :
				if( alt_k() == -1 )   return;          		break;
			case ALT_C        : alt_c();	              break;
			case ALT_I        : alt_i();	              break;
			case ALT_E        : alt_e();	              break;
			case ALT_H        : alt_h();	              break;
			case ALT_O        : alt_o();	              break;
			case ALT_X        :
				if( env.cons ) save();
				if( env.conf ) write_config( "config.edp" );
				sw_Restore( all_scr );
				return;
/*
			case NOTKEY :
				if( actw ) {                              /* values window is active */
					s[acb.csv].inf = s[acb.csv].fct &= ~(1<<vcb.csv);
					scr[vcb.csv-vcb.up+17][58].ch = 0;
					}
			break;
			case MAYBEKEY     :
				if( actw ) {                              /* values window is active */
					s[acb.csv].inf = s[acb.csv].fct |= 1<<vcb.csv;
					scr[vcb.csv-vcb.up+17][58].ch = 4;
					}
			break;
			case YESKEY       :
				if( actw ) {                              /* values window is active */
					s[acb.csv].inf = s[acb.csv].fct = 1<<vcb.csv;
					for( i=0;i<6 && i<vcb.vln;i++ ) scr[17+i][58].ch = 0;
					scr[vcb.csv-vcb.up+17][58].ch = 4;
					}
			break;
*/
			case RESET        : /* resets "yes" for all values of active attribute */
				s[acb.csv].inf = s[acb.csv].fct = -1;
				fillvalw( acb.csv );
			break;
      case CTRL_F1      :	comment();              break;
      case CTRL_S       :
				if( env.sound ) env.sound = 0;
				else            env.sound = 1;
				break;
      case F1_KEY       : help();                 break;
      case GREY_PLUS    : grey_plus();            break;
      case GREY_MINUS   : grey_minus();           break;
			}
		} while( 1 );
	}
