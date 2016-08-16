#include <conio.h>
#include "sw_keys.h"
#include "sw.h"

/*
*	  name         sw_draw 컴 draws a window using TC library
*   synopsis     w 컴 pointer to desired window description structure
*                wc 컴 pointer to desired window color description structure
*   description  Draws a border of the following types:
*                      wc->ft = -1 컴 no border;
*                                0 컴 a single line border;
*                                1 컴 a double line border.
*                Draws a window title begining at w->t position from
*                the upper left corner.
*/

/*****************************************************************************/
void sw_draw( w_descr *w,wc_descr *wc ) {
/*****************************************************************************/
	unsigned char  hor,ver;
	char           i,wd,ht;
	textattr( wc->fc );
	if( wc->ft >= 0 ) {
		window( w->l,w->u,w->r,w->d+1 );
		hor = b[wc->ft][1]; ver = b[wc->ft][3];
		gotoxy( 1,1 ); putch( b[wc->ft][0] );
		wd = w->r-w->l+1;
		for( i=2;i<wd;i++ ) putch( hor );
		putch( b[wc->ft][2] );
		ht = w->d-w->u+1;
		for( i=2;i<ht;i++ ) {	putch( ver ); gotoxy( wd,i ); putch( ver );	}
		putch( b[wc->ft][4] );
		for( i=2;i<wd;i++ ) putch( hor );
		putch( b[wc->ft][5] );
		}
	gotoxy( w->t,1 ); textattr( wc->tc ); cputs( w->ttl );
	window( w->l+1,w->u+1,w->r-1,w->d-1 ); textattr( wc->bc ); clrscr();
	}

/*
*   name         sw_menu 컴 organizes a window menu using TC library;
*   synopsis     w 컴 pointer to desired window description structure;
*                wc 컴 pointer to desired window color description structure;
*                m->en 컴 number of the menu enters;
*                m->eh - the number of the menu enters in row;
*                m->ew - the width of one enter;
*                m->cs - current cursor position;
*                m->entr[] 컴 array of the pointers to menu enters strings
*                returns the number of the enter selected
*   description  Draws the window using function sw_draw; sw_menu don't
*                care saving and restoring the images underneath menu window;
*/

#define    mrkcs()     sw_dye_at( w->l+col*(m->ew+2),w->u+row,\
											 w->l+(col+1)*(m->ew+2)-1,w->u+row,wc->hc )
#define    clrcs()     sw_dye_at( w->l+col*(m->ew+2),w->u+row,\
											 w->l+(col+1)*(m->ew+2)-1,w->u+row,wc->bc ); \
											 scr[w->u+row][w->l+col*(m->ew+2)+1].at = m->fl

/*****************************************************************************/
sw_menu( w_descr *w,wc_descr *wc,m_descr *m ) {
/*****************************************************************************/
	int i,k,col,row;
	sw_draw( w,wc );
	for( i=0,row=0,col=0;i<m->en;i++ ) {
		row = i / m->eh; col = i % m->eh;
		gotoxy( col*(m->ew+2)+2,row+1 ); cputs( m->entr[i] );
		scr[w->u+row][w->l+col*(m->ew+2)+1].at = m->fl;
		}
	row = m->cs / m->eh; col = m->cs % m->eh; mrkcs();
	if( m->prom ) sw_puts( 1,24,m->prom[0],0x1b );
	do {
		k = bioskey( 0 );
		switch( k ) {
			case LEFTKEY :
				clrcs();
				m->cs = ( m->cs+m->en-1 ) % m->en; row = m->cs / m->eh;
																					 col = m->cs % m->eh;
				mrkcs();
				if( m->prom ) { sw_dye_ch( 1,24,79,24,32 ); sw_puts( 1,24,m->prom[m->cs],0x1b ); }
			break;
			case RIGHTKEY :
				clrcs();
				m->cs = ++m->cs % m->en; row = m->cs / m->eh;
																 col = m->cs % m->eh;
				mrkcs();
        if( m->prom ) { sw_dye_ch( 1,24,79,24,32 ); sw_puts( 1,24,m->prom[m->cs],0x1b ); }
			break;
			case UPKEY    :
				clrcs();
/*  		m->cs = (m->cs+m->en-m->eh) % m->en; row = m->cs / m->eh;
																						 col = m->cs % m->eh;    */
				if( m->en <= m->eh ) {
					m->cs = ( m->cs+m->en-1 ) % m->en; row = m->cs / m->eh;
																						 col = m->cs % m->eh;
					}
				else {
					if( row ) row--;
					else {
						col = ( col+m->eh-1 ) % m->eh;
						if( m->en/m->eh * m->eh + col < m->en ) row = m->en/m->eh;
						else row = m->en/m->eh - 1;
						}
					m->cs = row*m->eh + col;
					}
				mrkcs();
        if( m->prom ) { sw_dye_ch( 1,24,79,24,32 ); sw_puts( 1,24,m->prom[m->cs],0x1b ); }
			break;
			case DOWNKEY    :
				clrcs();
/*			m->cs = (m->cs+m->eh) % m->en; row = m->cs / m->eh;
																			 col = m->cs % m->eh;          */
				if( m->en <= m->eh ) {
					m->cs = ++m->cs % m->en; row = m->cs / m->eh;
																	 col = m->cs % m->eh;
					}
				else {
					if( m->cs+m->eh < m->en ) row++;
					else { row = 0; col = ++col % m->eh; }
					m->cs = row*m->eh + col;
					}
				mrkcs();
        if( m->prom ) { sw_dye_ch( 1,24,79,24,32 ); sw_puts( 1,24,m->prom[m->cs],0x1b ); }
			break;
			case HOMEKEY    :
				clrcs();
				m->cs = row = col = 0; mrkcs();
        if( m->prom ) { sw_dye_ch( 1,24,79,24,32 ); sw_puts( 1,24,m->prom[m->cs],0x1b ); }
			break;
			case ENDKEY    :
				clrcs();
				m->cs = m->en - 1; row = m->cs / m->eh; col = m->cs % m->eh;
				mrkcs();
        if( m->prom ) { sw_dye_ch( 1,24,79,24,32 ); sw_puts( 1,24,m->prom[m->cs],0x1b ); }
			break;
			case CRKEY  :
				if( m->prom ) sw_dye_ch( 1,24,79,24,32 );
				return( m->cs );
			case ESCKEY :
      	if( m->prom ) sw_dye_ch( 1,24,79,24,32 );
				return( -1 );
      case F1_KEY :   sw_menuhelp();
			default :
				for( i=0;i<m->en;i++ )
					if( ( char )k == tolower( *m->entr[i] ) ) {
						clrcs();
						m->cs = i; row = m->cs / m->eh; col = m->cs % m->eh;
						mrkcs();
          	if( m->prom ) sw_dye_ch( 1,24,79,24,32 );
						return( m->cs );
						}
			}
		} while( 1 );
	}

#undef mrkcs()
#undef clrcs()

/*****************************************************************************/
void sw_dye_at( char l,char u,char r,char d,char at ) {
/*****************************************************************************/
	int i,j;
	for( j=u;j<=d;j++ ) for( i=l;i<=r;i++ ) scr[j][i].at = at;
	}
/*****************************************************************************/
void sw_dye_ch( char l,char u,char r,char d,char ch ) {
/*****************************************************************************/
	int i,j;
	for( j=u;j<=d;j++ ) for( i=l;i<=r;i++ ) scr[j][i].ch = ch;
	}
/*****************************************************************************/
void sw_puts( char l,char u,char *strptr,char at ) {
/*****************************************************************************/
  char ch;
	int  i=0;
	while( ( ch = strptr[i] ) != 0 ) {
		scr[u][l+i].ch = ch;	scr[u][l+i++].at = at;
		}
	}
/*****************************************************************************/
void sw_gettext( char l,char u,char r,char d,int *txt ) {
/*****************************************************************************/
	int i,j;
	for( i=0;i<d-u+1;i++ )
		for( j=0;j<r-l+1;j++ )
			( txt+i*( r-l+1 ) )[j] = scrn[u-1+i][l-1+j];
	}
/*****************************************************************************/
void sw_puttext( char l,char u,char r,char d,int *txt ) {
/*****************************************************************************/
	int i,j;
	for( i=0;i<d-u+1;i++ )
		for( j=0;j<r-l+1;j++ )
			scrn[u-1+i][l-1+j] = ( txt+i*( r-l+1 ) )[j];
	}