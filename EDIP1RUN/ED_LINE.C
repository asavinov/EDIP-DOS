/*****************************************************************************/
ed_line( char *line,w_descr *w,wc_descr *wc ) {
/*****************************************************************************/
	int r,k,j,i=0;
	sw_save( *w ); sw_draw( w,wc );
	gotoxy( 2,1 ); cursor( 0x06,0x07 );
	while( line[i] ) putch( line[i++] );
	r = i;
	do {
		k = bioskey( 0 );
		switch( k ) {
			case LEFTKEY  : if( i )   { gotoxy( wherex()-1,1 ); i--; } break;
			case RIGHTKEY : if( i<r ) { gotoxy( wherex()+1,1 ); i++; } break;
			case BSKEY    :
				if( i ) {
					gotoxy( wherex()-1,1 ); i--;
					for( j=i;j<r;j++ ) scr[w->u][w->l+j+1].ch = scr[w->u][w->l+j+2].ch;
					r--;
					}
			break;
			case DELKEY   :
				if( i<r ) {
					for( j=i;j<r;j++ ) scr[w->u][w->l+j+1].ch = scr[w->u][w->l+j+2].ch;
					r--;
					}
			break;
			case HOMEKEY  : if( i )   { gotoxy( 2,1 );   i=0; } break;
			case ENDKEY   : if( i<r ) { gotoxy( r+2,1 ); i=r; } break;
			case ESCKEY   : sw_restore( *w ); cursor( 0,0 ); return( -1 );
			case CRKEY    :
				for( j=0;j<r;j++ ) line[j] = scr[w->u][w->l+j+1].ch;
				line[r] = '\x0'; sw_restore( *w ); cursor( 0,0 ); return( 0 );
			default       :
				if( isprint((char)k) && (r<(w->r-w->l-3)) ) {
					for( j=r;j>=i;j-- ) scr[w->u][w->l+j+2].ch = scr[w->u][w->l+j+1].ch;
					r++; putch( toupper(k) ); i++;
					}
			}
		} while( 1 );
	}
