/*
*   name         paste_c - pastes all disjuncts in matrix for one pass
*   synopsis     matr_ptr - pointer to the matrix
*                a_num    - the attributes number
*                n_num    - the lines number already pasted
*                m_num    - the lines number in matrix
*                return   - the lines number in resulting matrix
*   description  This function makes one pass through the matrix pointed to
*                by matr_ptr and pastes all m_num disjuncts excepting the 
*                first n_num with each other. Every new line is tested to 
*                know if it is absorbed by the all previous lines.
*/    
#include <bios.h>
#include "edip.h"
#define  ESCKEY    0x011b            
/****************************************************************************/
int paste_c( int huge* matr_ptr, int a_num, 
             unsigned int n_num, unsigned int m_num ) { /*__________________*/
/****************************************************************************/
	int huge* res_ptr; 
	int huge* slow_ptr; 
	int huge* fast_ptr;
	int huge* absb_ptr;
  unsigned int r_num, slow, fast, sect, i, j; 

  r_num = m_num;
  res_ptr = matr_ptr + ( (long)a_num * (long)(r_num+1) );

  slow_ptr = matr_ptr + ( (long)a_num * (long)(n_num+1) );
  for( slow=n_num+1; slow<=m_num; slow++ ) {
    output_info( slow, r_num );

  	fast_ptr = matr_ptr + a_num;
    for( fast=1; fast<slow; fast++ ) {
    	for( sect=0; sect<a_num; sect++ ) {
      	*(res_ptr+sect) = *(slow_ptr+sect) & *(fast_ptr+sect);
        if( *(res_ptr+sect) == *(slow_ptr+sect) ||
            *(res_ptr+sect) == *(fast_ptr+sect) ) continue;
        for( i=0; i<a_num; i++ ) {                      /* loop for pasting */
        	if( i == sect ) continue;    /* if this section is already pasted */
          *(res_ptr+i) = *(slow_ptr+i) | *(fast_ptr+i);
          if( *(res_ptr+i) == *(matr_ptr+i) ) goto not_paste;
          }
        absb_ptr = matr_ptr + a_num;
        for( i=1; i<=r_num; i++ ) {
        	for( j=0; j<a_num; j++ ) {
          	if( ~*(res_ptr+j) & *(absb_ptr+j) ) break;   /* if not absorbed */
            }
          if( j == a_num ) break;                            /* if absorbed */
          absb_ptr = absb_ptr + a_num;
          }
        if( i > r_num ) {                    /* if new line is not absorbed */
        	r_num++; 
					res_ptr = res_ptr + a_num;
          }
        not_paste:
        ;
        }
      fast_ptr = fast_ptr + a_num;
      }
    slow_ptr = slow_ptr + a_num;
    }
  return( r_num );
  }