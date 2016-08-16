/*
*  name:                  edits row of characters
*  synopsis:     sourse - the string to display at the begining of the editing;
*                         before calling ed_row() contains starting row;
*                         remains unchanged.
*                destin - the string to return edited row; after exit from
*                         ed_row() contains resulting row; is changed
*                         unless ESC is pressed.
*                w -      pointer to window description structure.
*                wc -     pointer to window colors description structure.
*  description:           takes the sourse string and displays it at window
*                         with sizes and colors pointed to by w and wc;
*                         then permits to edit this row; when pressed CR
*                         return 0 and saves resulting row at the destination
*                         string; when pressed ESC return -1 and destination
*                         string is not changed.
*/

#include <alloc.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include "sw_keys.h"
#include "sw.h"

#define  MAXROWLEN       80

/*****************************************************************************/
ed_row( char *destin,char *sourse,w_descr *w,wc_descr *wc ) { /*_ROW_EDITOR__*/
/*****************************************************************************/
	char row[MAXROWLEN];
	int i,j,l,r,k,wd;
	wd = w->r - w->l - 3;                     /* the number of chars in window */
	sw_Store( *w ); sw_draw( w,wc );
	strcpy( row,sourse );
	textattr( wc->hc );
	i = r = strlen( sourse );
	if( (l = r - wd + 1) <= 0 ) l = 0;
	else scr[w->u][w->l].ch = 17;
	gotoxy( 2,1 ); for( j=l;j<r;j++ ) putch( row[j] );
	textattr( wc->bc );
	cursor( 0x06,0x07 );
	k = bioskey( 0 );
	if( isprint( (char)k ) ) {
		sw_dye_ch( w->l,w->u,w->r-2,w->d-2,32 );
		gotoxy( 2,1 ); i = l = r = 0; row[i] = 0;
		}
	sw_dye_at( w->l+1,w->u,w->r-3,w->d-2,wc->bc );
	do {
		switch( k ) {
			case LEFTKEY  :
				if( i ) {
					i--;
					if( i >= l ) gotoxy( wherex()-1,1 );
					else {
						l--; for( j=l;j<l+wd && j<r;j++ ) putch( row[j] ); gotoxy( 2,1 );
						if( r-l >= wd ) scr[w->u][w->r-2].ch = 16;
						if( !l ) scr[w->u][w->l].ch = 32;
						}
					}
			break;
			case RIGHTKEY :
				if( r > i ) {
					i++;
					if( i <= l+wd-1 ) gotoxy( wherex()+1,1 );
					else {
						if( !l ) scr[w->u][w->l].ch = 17;
						l++; gotoxy( 2,1 );
						for( j=l;j<l+wd && j<=r;j++ ) putch( row[j] );
						gotoxy( wherex()-1,1 );
						if( r-l < wd ) scr[w->u][w->r-2].ch = 32;
						}
					}
			break;
			case BSKEY    :
				if( i ) {
					for( j=i;j<=r;j++ ) row[j-1] = row[j];
					r--; i--;
					if( i+1 == l ) { l--; if( !l ) scr[w->u][w->l].ch = 32; }
					else {
						gotoxy( wherex()-1,1 );
						for( j=i;j<l+wd && j<=r;j++ ) putch( row[j] );
						gotoxy( i-l+2,1 );
						}
          if( r-l < wd ) scr[w->u][w->r-2].ch = 32;
					}
			break;
			case DELKEY   :
				if( r && i != r ) {
					for( j=i;j<r;j++ ) row[j]=row[j+1];
					r--;
					for( j=i;j<l+wd && j<=r;j++ ) putch( row[j] );
					gotoxy( i-l+2,1 );
          if( r-l < wd ) scr[w->u][w->r-2].ch = 32;
					}
			break;
			case HOMEKEY  :
				if( i ) {
					if( l ) { l = 0; gotoxy( 2,1 ); for( j=0;j<wd;j++) putch( row[j] ); }
					gotoxy( 2,1 ); i = 0;
					scr[w->u][w->l].ch = 32;
					if( r-l >= wd ) scr[w->u][w->r-2].ch = 16;
					}
			break;
			case ENDKEY   :
				if( i < r ) {
					i = r;
					if( l < (j = r-wd+1) ) {
						l=j; gotoxy( 2,1 ); for( j=l;j<l+wd;j++ ) putch( row[j] );
						}
					gotoxy( i-l+2,1 );
					scr[w->u][w->r-2].ch = 32;
					if( l ) scr[w->u][w->l].ch = 17;
					}
			break;
			case ESCKEY   : sw_Restore( *w ); cursor( 0,0 ); return( -1 );
			case CRKEY    :
				strcpy( destin,row );
				sw_Restore( *w ); cursor( 0,0 ); return( 0 );
			default       :
				if( isprint( (char)k ) && r < MAXROWLEN-1 ) {
					r++;
					for( j=r;j>i;j-- ) row[j] = row[j-1]; row[i] = toupper(k);
					if( i == l+wd-1 ) {
						gotoxy( 2,1 ); l++; j=l; if( l ) scr[w->u][w->l].ch = 17;
						}
					else j = i;
					for( ;j<l+wd && j<r;j++ ) putch( row[j] );
					i++;
					gotoxy( i-l+2,1 );
          if( r-l >= wd ) scr[w->u][w->r-2].ch = 16;
					}
			}
			k = bioskey( 0 );
		} while( 1 );
	}

#undef  MAXROWLEN
