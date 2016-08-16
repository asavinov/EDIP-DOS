#include "edip_l.h"
#define  ESCKEY    0x011b            
/****************************************************************************/
code_generator() { /*___________________________FINDS_ALL_PRIME_DISJUNCTS___*/
/****************************************************************************/
	unsigned int i,j;
  int huge *l_ptr;

	printf( "   P A S S : 0\n" );
  for( i=1; i<=m_str_num; i++ ) {
  	if( (j = absorb_fst(i)) != 0 ) {
    	printf( "WARNING: Rule %u is a consecuence of rule %u.\n", i, j );
      }
    if( (j = absorb_sec(i)) != 0 ) {  
    	printf( "WARNING: Rule %u is a consecuance of rule %u.\n", i, j );
      }
    }
  printf( "                           Resolution     Absorbtion\n" );
  printf( "                           ----------     ----------\n" );

  l_ptr = matr_buf + (long)attr_num*(long)m_str_num;
  do {
  	gotoxy( 30, wherey() ); printf( "%5u", m_str_num );
  	j = pastl_88( matr_buf, l_ptr, attr_num, m_str_num );
    if( j ) {
    	l_ptr += attr_num;
      m_str_num++;
      }
	  gotoxy( 30, wherey() ); printf( "%5u", m_str_num );
  	if( bioskey(1) == ESCKEY ) {
  		bioskey(0);
			printf( "\nERROR: User break.\n" );
			exit( 1 );
			}
	  if( m_str_num >= matr_buf_len ) {
  	 	printf( "ERROR: Not enough memory for the rules\n" );
   		exit( 1 );
	   	}
    if( !explain ) {
      absorb_end_88();
    	}
  	} while( j );

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
absorb_end_88() { /*________________________________________________________*/ 
/****************************************************************************/
	unsigned int i,j;
  int huge *line_ptr;

  line_ptr = matr_buf + (long)(m_str_num-1)*(long)(attr_num);
	for( i=m_str_num-1; i>0; i-- ) {
		if( cutout_sec_88( line_ptr, i, attr_num, m_str_num ) ) {
      m_str_num--;
      gotoxy( 45, wherey() ); printf( "%5u", m_str_num );
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
