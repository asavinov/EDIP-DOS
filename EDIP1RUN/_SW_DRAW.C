/*
*	  name         _sw_draw 컴 draws a window without using TC library
*                through the direct access to the display buffer
*   synopsis     w 컴 pointer to desired window description structure
*                wc 컴 pointer to desired window color description structure
*   description  Saves the image below the window before drawing using
*                pointer w->img (a memory should be allocated for image
*                to save before using this function).
*                Draws a border of the following types:
*                      wc->ft = -1 컴 no border;
*                                0 컴 a single line border;
*                                1 컴 a double line border.
*                Draws a window title begining at w->t position from
*                the upper left corner.
*/
/*****************************************************************************/
void _sw_draw( w_descr *w,wc_descr *wc ) {
/*****************************************************************************/
	char i,j;
	w->img=malloc( ( w->r-w->l+1 )*( w->d-w->u+1 )*2 );
	gettxt( *w );
	sw_dye_at( w->l-1,w->u-1,w->r-1,w->d-1,wc->fc );
	if( wc->ft<0 ) sw_dye_ch( w->l-1,w->u-1,w->r-1,w->d-1,32 );
	else {
		sw_dye_ch( w->l,w->u-1,w->r-2,w->u-1,b[wc->ft][1] );
		sw_dye_ch( w->l,w->d-1,w->r-2,w->d-1,b[wc->ft][1] );
		sw_dye_ch( w->l-1,w->u,w->l-1,w->d-2,b[wc->ft][3] );
		sw_dye_ch( w->r-1,w->u,w->r-1,w->d-2,b[wc->ft][3] );
		scr[w->u-1][w->l-1].ch=b[wc->ft][0];
		scr[w->u-1][w->r-1].ch=b[wc->ft][2];
		scr[w->d-1][w->l-1].ch=b[wc->ft][4];
		scr[w->d-1][w->r-1].ch=b[wc->ft][5];
		}
	sw_puts( w->l+w->t-2,w->u-1,w->ttl,wc->tc );
	sw_dye_ch( w->l,w->u,w->r-2,w->d-2,32 );
	sw_dye_at( w->l,w->u,w->r-2,w->d-2,wc->bc );
	}
