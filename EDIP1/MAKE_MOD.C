#include "edip.h"
extern att_tab_descr *tab;
extern char modname[];
extern int modh;
/****************************************************************************/
make_mod() { /*_____________________________________________________________*/
/****************************************************************************/
  char str[80];
  int  i, j, n, r, res=0;
  long pos, Len;
  char huge *Start;


  res += write( modh, "EDIP\20\0", 6 );
  res += write( modh, &attr_num, 2 );
  res += write( modh, &m_str_num, 2 );
  if( res != 10 ) {
  	printf( "ERROR: Can not write to file %s.\n", modname );
    }
	lseek( modh, 34 + 10*attr_num, SEEK_SET );
	for( i=0;i<attr_num;i++ ) {                /* write attributes and values */
		pos = tell( modh );
		lseek( modh, 34 + 10*i, SEEK_SET );
    write( modh, &tab[i].vln, 2 );
    write( modh, &pos, 4 );
    write( modh, "\0\0\0\0", 4 );      /* if comment field will not be used */
		lseek( modh, pos, SEEK_SET );
    bcb.lev = tab[i].nam; bcb.cur = bcb.buf + bcb.lev;
    write( modh, bcb.cur, tab[i].nln );
    write( modh, "\0", 1 );
		for( j=0; j<tab[i].vln; j++ ) {                       /* loop on values */
    	bcb.lev = tab[i].val[j].nam; bcb.cur = bcb.buf + bcb.lev;
      write( modh, bcb.cur, tab[i].val[j].nln );
      write( modh, "\0", 1 );
			}
		}
  for( i=0;i<attr_num;i++ ) {           /* write comments to the attributes */
	  pos = tell( modh );
    lseek( modh, 34 + 10*i + 6, SEEK_SET );
    write( modh, &pos, 4 );
    lseek( modh, pos, SEEK_SET );
    bcb.lev = tab[i].com; bcb.cur = bcb.buf + bcb.lev;
		write( modh, bcb.cur, tab[i].cln );
    write( modh, "\0", 1 );
    }

  pos = tell( modh );                               /* write matrix to file */
  lseek ( modh, 22, SEEK_SET );
  write ( modh, &pos, 4 );
  lseek ( modh, pos, SEEK_SET );

  Len = 2 * (long) attr_num * (long) m_str_num;
  n = Len / 30000;
  r = Len % 30000;
  Start = (char huge*) (matr_buf + attr_num);

  for ( i = 0; i < n; i++ ) {
    write ( modh, (void*) Start, 30000 );
    Start += 30000;
    }
  write ( modh, (void*) Start, r );
  return;
  }
