/*****************************************************************************/
int _sw_menu( w_descr *w,wc_descr *wc,int entrn,unsigned char fl,
							char *m_entr[],char *m_prom[] )	{
/*****************************************************************************/
	int i,j,k;
	_sw_draw( w,wc );
	for( j=0;j<entrn;j++ )
		sw_puts( w->l,w->u+j,m_entr[j],wc->bc );
	sw_dye_at( w->l,w->u,w->r-2,w->u,wc->hc );
  for( j=1;j<entrn;j++ ) scr[w->u+j][w->l+1].at=fl; j = 0;
	do {
		i = bioskey(0);
		switch( i ) {
			case UPKEY :
				sw_dye_at( w->l,w->u+j,w->r-2,w->u+j,wc->bc );
				scr[w->u+j][w->l+1].at=fl;
				j=( j+entrn-1 ) % entrn;
				sw_dye_at( w->l,w->u+j,w->r-2,w->u+j,wc->hc );
				sw_puts( 1,24,m_prom[j],0x1b );
			break;
			case DOWNKEY :
				sw_dye_at( w->l,w->u+j,w->r-2,w->u+j,wc->bc );
				scr[w->u+j][w->l+1].at=fl;
				j = ++j % entrn;
				sw_dye_at( w->l,w->u+j,w->r-2,w->u+j,wc->hc );
				sw_puts( 1,24,m_prom[j],0x1b );
			break;
			case CRKEY  : _sw_restore( *w ); return( j );
			case ESCKEY : _sw_restore( *w ); return( -1 );
			default :
				for( k=0;k<entrn;k++ )
					if( (char)i == tolower( *(m_entr[k]) ) ) {
						sw_restore( *w );	return( k );
						}
			}
		} while( 1 );
	}
