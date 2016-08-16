#include <conio.h>
#include <dos.h>

#define  ATTW   0
#define  VALW   1

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
					 struct  mrk_struct {
		int 		 inf;                                        /* inference marker */
		int   	 exp;                                      /* explanation marker */
		int   	 hyp;                                       /* hypothesis marker */
		}      mrk;
	}      *s;
extern int actw,fstinf,lstinf,fstexp,lstexp,fsthyp,lsthyp;

/*****************************************************************************/
cursor( int startline,int endline ) {
/*****************************************************************************/
	union REGS regs;
	if( startline == 0 && endline == 0 ) regs.h.ch = 0x20;
	else { regs.h.ch = startline; regs.h.cl = endline; }
	regs.h.ah = 1; int86( 0x10,&regs,&regs );
	}
/*****************************************************************************/
tabkey() {
/*****************************************************************************/
	if( actw ) {                                    /* values window is active */
		sw_dye_at( 9,16,21,16,0x0c );
		switch( acb.csh ) {
			case 0 : sw_dye_at( 9, 2, 29,2, 0x5f ); break;
			case 1 : sw_dye_at( 61,2, 63,2, 0x5f ); break;
			case 2 : sw_dye_at( 67,2, 69,2, 0x5f ); break;
			case 3 : sw_dye_at( 74,2, 76,2, 0x5f ); break;
			}
		actw = ATTW; window(2,4,79,15);
		}
	else {                                      /* attributes window is active */
		switch( acb.csh ) {
			case 0 : sw_dye_at( 9, 2, 29,2, 0x0c ); break;
			case 1 : sw_dye_at( 61,2, 63,2, 0x0c ); break;
			case 2 : sw_dye_at( 67,2, 69,2, 0x0c ); break;
			case 3 : sw_dye_at( 74,2, 76,2, 0x0c ); break;
			}
		sw_dye_at( 9,16,21,16,0x5f );
		actw = VALW; window( 2,18,79,23 );
		}
	}
/*****************************************************************************/
leftkey() {
/*****************************************************************************/
	if( !actw )                                 /* attributes window is active */
		switch( acb.csh ) {
			case 0 :
				sw_dye_at( 9, 2,29,2,0x0c ); sw_dye_at( 74,2,76,2,0x5f ); acb.csh = 3;
			break;
			case 1 :
				sw_dye_at( 61,2,63,2,0x0c ); sw_dye_at( 9, 2,29,2,0x5f ); acb.csh = 0;
			break;
			case 2 :
				sw_dye_at( 67,2,69,2,0x0c ); sw_dye_at( 61,2,63,2,0x5f ); acb.csh = 1;
			break;
			case 3 :
				sw_dye_at( 74,2,76,2,0x0c ); sw_dye_at( 67,2,69,2,0x5f ); acb.csh = 2;
			break;
			}
	}
/*****************************************************************************/
rightkey() {
/*****************************************************************************/
	if( !actw )                                 /* attributes window is active */
		switch( acb.csh ) {
			case 0 :
				sw_dye_at( 9, 2,29,2,0x0c ); sw_dye_at( 61,2,63,2,0x5f ); acb.csh = 1;
			break;
			case 1 :
				sw_dye_at( 61,2,63,2,0x0c ); sw_dye_at( 67,2,69,2,0x5f ); acb.csh = 2;
			break;
			case 2 :
				sw_dye_at( 67,2,69,2,0x0c ); sw_dye_at( 74,2,76,2,0x5f ); acb.csh = 3;
			break;
			case 3 :
				sw_dye_at( 74,2,76,2,0x0c ); sw_dye_at( 9, 2,29,2,0x5f ); acb.csh = 0;
			break;
			}
	}
/*****************************************************************************/
upkey() {
/*****************************************************************************/
	if( actw ) {                                    /* values window is active */
		if( vcb.csv>0 ) {    /* upper limit is not achieved in the values window */
			clrvalcs();
			if( --vcb.csv<vcb.up ) {             /* exit out of upper window limit */
				vcb.up = vcb.csv;	gotoxy( 1,1 );
				insline(); putval( 1,vcb.csv );
				}
			mrkvalcs();
			}
		}
	else
		switch( acb.csh ) {
			case 0 :
				if( acb.csv>0 ) {  /* upper limit is not achieved in the att. window */
					clrattcs();
					if( --acb.csv<acb.up ) {         /* exit out of upper window limit */
						acb.up = acb.csv;	gotoxy( 1,1 );
						insline(); putatt( 1,acb.csv );
						}
					mrkattcs();
					vcb.csv = vcb.up = 0; fillvalw( acb.csv );
					}
			break;
			case 1 : uptree( fstinf ); break;
			case 2 : uptree( fstexp ); break;
			case 3 : uptree( fsthyp ); break;
			}
	}
/*****************************************************************************/
downkey() {
/*****************************************************************************/
	if( actw ) {                                    /* values window is active */
		if( vcb.csv < vcb.vln-1 ) {   /* lower limit is not achieved in the valw */
			clrvalcs();
			if( ++vcb.csv > vcb.up+5 ) {                          /* out of window */
				vcb.up = vcb.csv-5;	gotoxy( 1,6);
				putch('\n'); putval( 6,vcb.csv );
				}
			mrkvalcs();
			}
		}
	else
    switch( acb.csh ) {
			case 0 :
				if( acb.csv<acb.atn-1 ) { /* lower limit is not achieved in the attw */
					clrattcs();
					if( ++acb.csv > acb.up+11 ) {                     /* out of window */
						acb.up = acb.csv-11; gotoxy( 1,12 );
						putch('\n'); putatt( 12,acb.csv );
						}
					mrkattcs();
					vcb.csv = vcb.up = 0; fillvalw( acb.csv );
					}
      break;
			case 1 : downtree( lstinf ); break;
			case 2 : downtree( lstexp ); break;
			case 3 : downtree( lsthyp ); break;
			}
	}
/*****************************************************************************/
uptree( int fst ) {
/*****************************************************************************/
	int j;
	if( acb.csv > fst ) {
		j = acb.csv;
		switch( acb.csh ) {
			case 1 : while( !s[--j].mrk.inf ) ; break;
			case 2 : while( !s[--j].mrk.exp ) ; break;
			case 3 : while( !s[--j].mrk.hyp ) ; break;
			}
		clrattcs(); acb.csv = j;
		if( j >= acb.up ) mrkattcs();
		else { acb.up = acb.csv; fillattw(); }
		vcb.up = vcb.csv = 0; fillvalw();
		}
	}
/*****************************************************************************/
downtree( int lst ) {
/*****************************************************************************/
	int j;
	if( acb.csv < lst ) {
  	j = acb.csv;
		switch( acb.csh ) {
			case 1 : while( !s[++j].mrk.inf ) ; break;
			case 2 : while( !s[++j].mrk.exp ) ; break;
			case 3 : while( !s[++j].mrk.hyp ) ; break;
			}
		clrattcs(); acb.csv = j;
		if( j - acb.up < 12 ) mrkattcs();
		else { acb.up = acb.csv - 11; fillattw(); }
		vcb.up = vcb.csv = 0; fillvalw();
		}
	}
/*****************************************************************************/
homekey() {
/*****************************************************************************/
	if( actw ) {
		if( vcb.csv != vcb.up ) {	clrvalcs(); vcb.csv = vcb.up; mrkvalcs();	}
		}
	else
		if( acb.csv != acb.up ) {
			clrattcs(); acb.csv = acb.up; mrkattcs();
			vcb.csv = vcb.up = 0; fillvalw( acb.csv );
			}
	}
/*****************************************************************************/
endkey() {
/*****************************************************************************/
	if( actw ) {
		if( vcb.csv != vcb.up+5 && vcb.csv != vcb.vln-1 ) {
			clrvalcs();
			if( vcb.vln<6 ) vcb.csv=vcb.vln-1; else vcb.csv = vcb.up+5;
			mrkvalcs();
			}
		}
	else
		if( acb.csv != acb.up+11 && acb.csv != acb.atn-1 ) {
			clrattcs();
			if( acb.atn<12 ) acb.csv=acb.atn-1; else acb.csv = acb.up+11;
			mrkattcs();
			vcb.csv = vcb.up = 0; fillvalw( acb.csv );
			}
	}
/*****************************************************************************/
pgupkey() {
/*****************************************************************************/
	if( !actw )	{
		if( acb.up ) {
			if( acb.up >= 12 ) { acb.csv -= 12; acb.up -=12; }
			else { acb.csv -= acb.up; acb.up = 0; }
			fillattw();
			vcb.csv = vcb.up = 0; fillvalw( acb.csv );
			}
		}
	else
		if( vcb.up ) {
			if( vcb.up >= 6 ) { vcb.csv -= 6; vcb.up -= 6; }
			else { vcb.csv -= vcb.up; vcb.up = 0; }
			fillvalw( acb.csv );
			}
	}
/*****************************************************************************/
pgdnkey() {
/*****************************************************************************/
	if( !actw )	{
		if( acb.atn > acb.up+12 ) {
			if( acb.atn >= acb.up+24 ) { acb.csv += 12; acb.up +=12; }
			else { acb.csv += acb.atn-acb.up-12; acb.up = acb.atn-12; }
			fillattw();
			vcb.csv = vcb.up = 0; fillvalw( acb.csv );
			}
		}
	else
		if( vcb.vln > vcb.up+6 ) {
			if( vcb.vln >= vcb.up+12 ) { vcb.csv += 6; vcb.up += 6; }
			else { vcb.csv += vcb.vln-vcb.up-6; vcb.up = vcb.vln-6; }
			fillvalw( acb.csv );
			}
	}
/*****************************************************************************/
ctrl_pgupkey() {
/*****************************************************************************/
	if( !actw ) {
		if( acb.csv ) {
			clrattcs(); acb.csv = 0;
			if( !acb.up ) mrkattcs();	else { acb.up = 0; fillattw(); }
			vcb.csv = vcb.up = 0; fillvalw( acb.csv );
			}
		}
	else
		if( vcb.csv ) {
			clrvalcs(); vcb.csv = 0;
			if( !vcb.up ) mrkvalcs();	else { vcb.up = 0; fillvalw( acb.csv ); }
			}
	}
/*****************************************************************************/
ctrl_pgdnkey() {
/*****************************************************************************/
	if( !actw ) {
		if( acb.csv != acb.atn-1 ) {
			clrattcs(); acb.csv = acb.atn-1;
			if( acb.atn <= acb.up+12 ) mrkattcs();
			else { acb.up = acb.atn-12; fillattw(); }
			vcb.csv = vcb.up = 0; fillvalw( acb.csv );
			}
		}
	else
		if( vcb.csv != vcb.vln-1 ) {
			clrvalcs(); vcb.csv = vcb.vln-1;
			if( vcb.vln <= vcb.up+6 ) mrkvalcs();
			else { vcb.up = vcb.vln-6; fillvalw( acb.csv ); }
			}
	}