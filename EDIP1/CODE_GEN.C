#include "edip.h"
#define  ESCKEY    0x011b

  time_t time1, time2, Ttime1, Ttime2;
  double Elapsed;
  int min, sec;

/****************************************************************************/
code_generator() { /*___________________________FINDS_ALL_PRIME_DISJUNCTS___*/
/****************************************************************************/
  unsigned int i, j;


  time (&Ttime1);
  printf( "   P A S S : 0\n" );
  for ( i=1; i<=m_str_num; i++ ) {
  	if ( (j = absorb_fst(i)) != 0 ) {
    	printf( "WARNING: Rule %u is a consecuence of rule %u.\n", i, j );
      }
    if( (j = absorb_sec(i)) != 0 ) {
    	printf( "WARNING: Rule %u is a consecuance of rule %u.\n", i, j );
      }
    }
  printf ( "                            CURRENT    TOTAL     TIME\n" );
  printf ( "                            -------   -------   -------\n" );


  for ( i=1; m_str_num > not_str_num && i < PassNum+1; i++ ) {

    time (&time1);                          /* засечь начальное время */
    printf ( "   P A S S : %u    Resolution", i );

    j = paste_88 ( matr_buf, attr_num, not_str_num, m_str_num );

    gotoxy( 40, wherey() ); printf( "%5u", j );
    time (&time2);                             /* засечь конечное время */
    Elapsed = difftime ( time2, time1 );
    min = (int) Elapsed / 60; sec = (int) Elapsed % 60;
    gotoxy ( 49, wherey() ); printf( "%3u:%2u", min, sec );

    time (&time1);                          /* засечь начальное время */
    printf ( "\n" );
    not_str_num = m_str_num; m_str_num = j;
    printf ( "                  Absorbtion" );
    absorb_end_88 ();
    gotoxy ( 40, wherey() ); printf( "%5u", m_str_num );
    time (&time2);                             /* засечь конечное время */
    Elapsed = difftime ( time2, time1 );
    min = (int) Elapsed / 60; sec = (int) Elapsed % 60;
    gotoxy ( 49, wherey() ); printf( "%3u:%2u", min, sec );
    printf ( "\n" );
    }

  time (&Ttime2);
  Elapsed = difftime ( Ttime2, Ttime1 );
  min = (int) Elapsed / 60; sec = (int) Elapsed % 60;
  printf( "                            -------   -------   -------\n" );
  gotoxy ( 49, wherey() ); printf( "%3u:%2u\n", min, sec );

  return;
  }
/****************************************************************************/
absorb_fst( unsigned int strn ) { /*________________________________________*/
                                  /* is of use only in pass 0               */
/****************************************************************************/
	unsigned int j;
	for( j=1; j<strn; j++ )	if( absorb(j,strn) ) return(j);
	return( 0 );
	}
/****************************************************************************/
absorb_sec( unsigned int strn ) { /*________________________________________*/
/****************************************************************************/
	unsigned int j;
	for( j=strn+1; j<=m_str_num; j++ ) if( absorb(j,strn) ) return(j);
	return( 0 );
	}

/****************************************************************************/
absorb_end() { /*___________________________________________________________*/
               /*   cuts out only last pass lines                           */
/****************************************************************************/
	unsigned int i,j;
	for( i=m_str_num-1; i>not_str_num; i-- ) {
  	gotoxy( 30, wherey() ); printf( "%5u     %5u", i, m_str_num );
		if( absorb_sec(i) ) {
				Mstrcpy( i,m_str_num );
        m_str_num--;
				}
		}
  return;
	}
/****************************************************************************/
absorb_end_88() { /*________________________________________________________*/
/****************************************************************************/
	unsigned int i,j;
  int huge *line_ptr;

  j = explain ?  not_str_num : 0;
  line_ptr = matr_buf + (long)(m_str_num-1)*(long)(attr_num);
	for( i=m_str_num-1; i>j; i-- ) {
  	gotoxy( 30, wherey() ); printf( "%5u     %5u", i, m_str_num );
		if( cutout_sec_88( line_ptr, i, attr_num, m_str_num ) ) {
        m_str_num--;
        if( i <= not_str_num ) not_str_num--;
				}
    line_ptr -= attr_num;
		}
  return;
	}

/****************************************************************************/
absorb( unsigned int u, unsigned int v) {
/****************************************************************************/
	unsigned int i;                                        /* section counter */
	for( i=0;i<attr_num;i++ )	{
		if( ~Matr(v,i) & Matr(u,i) ) return( 0 );
    }
	return( 1 );
	}
/****************************************************************************/
output_info( unsigned int current, unsigned int total ) {
/****************************************************************************/
  gotoxy( 30, wherey() ); printf( "%5u     %5u", current, total );
  if( bioskey(1) == ESCKEY ) {
  	bioskey(0);
		printf( "\nERROR: User break.\n" );
		exit( 1 );
		}
  if( total >= matr_buf_len ) {
   	printf( "ERROR: Not enough memory for the rules\n" );
   	exit( 1 );
   	}
  return;
	}
