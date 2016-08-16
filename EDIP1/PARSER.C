#include "edip.h"
/*___________________________________________________________________________*/
int mask[17] = {                             /* index is the values number */
	0,1,3,7,15,31,63,127,255,511,1023,2043,4095,8191,16383,32767,-1
	};
int  eof_switch, decl_switch, attval_switch, condis_switch, cur_attr;
att_tab_descr *tab;
int  att_tab_len=0;
int  class,id;
/****************************************************************************/
parser() { /*_______________________________________________________________*/
/****************************************************************************/
	int i;
	eof_switch = PERMIT;
	decl_switch = PERMIT;
	attval_switch = ATTRIBUTE;
	condis_switch = CON;
	scanner();
	while( class == SPECIAL && id == 10 ) {
		declaration();
		scanner();
		}
	decl_switch = DENY;

  matr_buf_len = farcoreleft() / (2 * attr_num) - 5;
  matr_buf = (int far*) farmalloc( (long)matr_buf_len * (2 * attr_num) );
  if( matr_buf == NULL ) {                                         /* ERROR */
  	printf( "ERROR: Not enough memory to allocate matrix buffer\n" );
    exit ( 1 );
    }
  printf( "%u disjuncts contains buffer.\n", matr_buf_len );
	for( i=0; i<attr_num; i++ ) {                     /* initialize zero line */
  	*(matr_buf + i) = mask[ tab[i].vln ];
    }

	while( class != SPECIAL || id != 9 ) {
		proposition();
		scanner();
		}
  return;
	}
/****************************************************************************/
declaration() { /*__________________________________________________________*/
/****************************************************************************/
	eof_switch = DENY;
	decl_switch = PERMIT;
	attval_switch = ATTRIBUTE;
	scanner();
	if( class != ATTRIBUTE ) {                                    /*! ERROR !*/
    printf( "ERROR: %d, Attribute is expected.\n", bcb.row );
    exit( 1 );
		}
	cur_attr = id;
	scanner();
	if( class != SPECIAL || id != 6 ) {                           /*! ERROR !*/
  	printf( "ERROR: %d, '=' is expected.\n", bcb.row );
    exit( 1 );
    }
	scanner();
	if( class != SPECIAL || id != 4 ) {                           /*! ERROR !*/
  	printf( "ERROR: %d, '{' is expected.\n", bcb.row );
    exit( 1 );
    }
	attval_switch = VALUE;
	scanner();
	if( class != VALUE ) {                                        /*! ERROR !*/
  	printf( "ERROR: %d, Value is expected.\n", bcb.row );
    exit( 1 );
    }
	scanner();
	do {
		if( class != SPECIAL || id != 3 ) {                         /*! ERROR !*/
    	printf( "ERROR: %d, ',' is expected.\n", bcb.row );
      exit( 1 );
      }
		scanner();
		if( class != VALUE ) {                                      /*! ERROR !*/
    	printf( "ERROR: %d, Value is expected.\n", bcb.row );
      exit( 1 );
      }
		scanner();
		} while( class != SPECIAL || id != 5 );
	scanner();
	if( class != SPECIAL || id != 2 ) {                           /*! ERROR !*/
  	printf( "ERROR: %d, ';' is expected.\n", bcb.row );
    exit( 1 );
    }
	attval_switch = ATTRIBUTE;
	decl_switch = DENY;
  eof_switch = PERMIT;
  return;
	}
/****************************************************************************/
proposition() { /*__________________________________________________________*/
/****************************************************************************/
	int i;
  if( m_str_num >= matr_buf_len ) {                                /* ERROR */
  	printf( "ERROR: Not enough memory for the rules\n" );
    exit( 1 );
    }
	for( i=0;i<attr_num;i++ ) {
		Matr( m_str_num+1,i ) = 0;
    }
	condis_switch = CON;
	if( class != SPECIAL || id != 8 ) {
		elem_prop();
		}
	while( class != SPECIAL || id != 8 ) {
		if( class != SPECIAL || id != 0 ) {                          /*! ERROR !*/
    	printf( "ERROR: %d, '&' or '->' are expected.\n", bcb.row );
      exit( 1 );
      }
		scanner();
		elem_prop();
		}
	condis_switch = DIS;
	scanner();
	if( class != SPECIAL || id != 2 ) {
		elem_prop();
		}
	while( class != SPECIAL || id != 2 ) {
		if( class != SPECIAL || id != 1 ) {                         /*! ERROR !*/
    	printf( "ERROR: %d, '|' or ';' are expected.\n", bcb.row );
      exit( 1 );
      }
		scanner();
		elem_prop();
		}
  m_str_num++;
	condis_switch = CON;
	attval_switch = ATTRIBUTE;
  eof_switch = PERMIT;
  return;
	}
/****************************************************************************/
elem_prop() { /*____________________________________________________________*/
/****************************************************************************/
	int np;                                            /* negative / positive */
	int valn,attn;
	if( class != ATTRIBUTE ) {                                     /*! ERROR !*/
  	printf( "ERROR: %d, Attribute is expected.\n", bcb.row );
    exit( 1 );
    }
	attn = cur_attr = id;
	scanner();
	if( class == SPECIAL && id == 6 )       np = 1;
	else if( class == SPECIAL && id == 7 )  np = 0;
	else {                                                        /*! ERROR !*/
  	printf( "ERROR: %d, '=' or '!=' are expected.\n", bcb.row );
    exit( 1 );
    }
	attval_switch = VALUE;
	scanner();
	if( class != VALUE ) {                                        /*! ERROR !*/
  	printf( "ERROR: %d, Value is expected.\n", bcb.row );
    exit( 1 );
    }
	valn = id;
	attval_switch = ATTRIBUTE;
	scanner();
	if( condis_switch == CON )
		if( np == 0 )	{	
			Matr( m_str_num+1,attn ) |=  (1<<valn);
      }
		else {       		
			Matr( m_str_num+1,attn ) |= ~(1<<valn) & mask[tab[attn].vln];
			}
	else if( condis_switch == DIS )
		if( np == 0 )	{ 
			Matr( m_str_num+1,attn ) |= ~(1<<valn) & mask[tab[attn].vln];
			}
		else {        		
			Matr( m_str_num+1,attn ) |=  (1<<valn);
      }
  return;
	}
/****************************************************************************/
scanner() { /*______________________________________________________________*/
/****************************************************************************/
	unsigned char num[3],str[80];
	int           i,next;
	long          lev;
  char far      *cur;

	next = Nextchar();
	while( isspace(next) || next == '/' || next == '"' ) { 
  	if( isspace(next) ) next = Nextchar();              /* skip over spaces */
		else if( next == '/' ) {                  /* skip over general comments */
      next = Nextchar();
			if( next == '*' ) {
      	do { 
					if( (next = Nextchar()) == 255 ) {
						printf("ERROR: %d, Unexpected end of file in comments.\n",bcb.row); 
            exit( 1 );
            }
					} while( next != '*' );
        next = Nextchar();
        if( next == '/' ) next = Nextchar();
        else {
          printf( "ERROR: %d, After '*' must follow '/'.\n", bcb.row );
          exit( 1 );	
          }
      	}
      else {
      	printf( "ERROR: %d, After '/' must follow '*'.\n", bcb.row );
        exit( 1 );	
        }
			}
		else if( next == '"' ) {     /* domain comment of the current attribute */ 
			tab[cur_attr].com = bcb.lev;
			do { 
				if( (next = Nextchar()) == 255 ) {
        	printf("ERROR: %d, Unexpected end of file in comments.\n",bcb.row); 
          exit( 1 );
          }
				} while( next != '"' );
			tab[cur_attr].cln = bcb.lev - tab[cur_attr].com - 1;
      next = Nextchar();
			}
    }
	switch( next ) {
		case '&' : class = SPECIAL; id = 0; 	return( id );
		case '|' : class = SPECIAL; id = 1; 	return( id );
		case ';' : class = SPECIAL; id = 2; 	return( id );
		case ',' : class = SPECIAL; id = 3; 	return( id );
		case '{' : class = SPECIAL; id = 4; 	return( id );
		case '}' : class = SPECIAL; id = 5; 	return( id );
		case '=' : class = SPECIAL; id = 6;   return( id );
		case '!' : class = SPECIAL; id = 7;
							 next = Nextchar();
							 if( next == '=' )          return( id );
							 else {                                           /*! ERROR !*/
							 	 printf( "ERROR: %d, After '!' must follow '='.\n", bcb.row );
								 exit( 1 );
								 }
		case '-' : class = SPECIAL; id = 8;
							 next = Nextchar();
							 if( next == '>' )          return( id );
							 else {                                            /*! ERROR !*/
               	 printf( "ERROR: %d, After '-' must follow '>'.\n", bcb.row );
                 exit( 1 );
                 }
		case 255 : class = SPECIAL; id = 9;                              /* EOF */
							 if( eof_switch == PERMIT ) return( id );
							 else {                                            /*! ERROR !*/
               	 printf( "ERROR: %d, Unexpected end of file.\n", bcb.row );
                 exit( 1 );
                 }
		case 'e' : lev = bcb.lev - 1; cur = bcb.cur - 1;
							 if( (next = Nextchar()) == 'n' )
								 if( (next = Nextchar()) == 'u' )
									 if( (next = Nextchar()) == 'm' ) {
										 next = Nextchar();
										 if( isspace(next) ) {
							         class = SPECIAL; id = 10;  return( id );
											 }
										 }
							 bcb.lev = lev; bcb.cur = cur; next = Nextchar();
		           break;
		}
	if( Isalnum( next ) ) {
		lev = bcb.lev - 1; cur = bcb.cur - 1;
		i = 0; str[i] = next;
		do {
			str[++i] = Nextchar();
			} while( Isalnum( str[i] ) || isspace( str[i] ) );
		Backup();
		do { i--;	} while( isspace( str[i] ) );
		str[++i] = '\0';
		i = sym_tab( lev,i,str );
		class = attval_switch; id = i; return( id );
		}
	else {                                                       /*! ERROR !*/
   	printf( "ERROR: %d, Unpermissible first character %c in identificator.\n", 
		        bcb.row, next );
    exit( 1 );
    }
  return( -1 );
	}
/****************************************************************************/
sym_tab( long offs, int len, unsigned char *s ) { /*________________________*/
/****************************************************************************/
	long lev;             /* to save the current position and then restore it */
	char far *cur;       
	int i;
	lev = bcb.lev; cur = bcb.cur;
	i = find_str( s );
	if( decl_switch == DENY ) {
		if( i<0 ) {                                                  /*! ERROR !*/
    	printf( "ERROR: %d, Undeclared identificator %s.\n", bcb.row, s );
      exit( 1 );
      }
		}
	else if( decl_switch == PERMIT ) {
		if( i >= 0 ) {                                               /*! ERROR !*/
    	printf( "ERROR: %d, Redeclaration of %s.\n", bcb.row, s );
      exit( 1 );
      }
		else if( attval_switch == ATTRIBUTE ) {
					 attr_num++;
					 if( attr_num > att_tab_len ) alloc_att_tab();
					 tab[attr_num-1].nam = offs;
					 tab[attr_num-1].nln = len;
					 tab[attr_num-1].vln = 0;
					 tab[attr_num-1].len = 0;
           tab[attr_num-1].com = 0;
           tab[attr_num-1].cln = 0;
					 i = attr_num-1;
					 }
				 else if(attval_switch == VALUE ) {
								i = ++tab[cur_attr].vln;
								if( i > tab[cur_attr].len ) alloc_val_tab();
								tab[cur_attr].val[i-1].nam = offs;
								tab[cur_attr].val[i-1].nln = len;
								i--;
								}
		}
	bcb.lev = lev; bcb.cur = cur;
  return( i );
	}
/****************************************************************************/
find_str( unsigned char *s ) { /*___________________________________________*/
/****************************************************************************/
	int i;
	if( attval_switch == ATTRIBUTE ) {
		for( i=0; i<attr_num; i++ )
			if( compar( tab[i].nam,tab[i].nln,s ) ) break;
		if( i >= attr_num ) i = -1;
		}
	else if( attval_switch == VALUE ) {
		for( i=0; i<tab[cur_attr].vln; i++ )
			if( compar( tab[cur_attr].val[i].nam,tab[cur_attr].val[i].nln,s ) )
				break;
		if( i >= tab[cur_attr].vln ) i = -1;
		}
	return( i );
	}
/****************************************************************************/
compar( long offs,int len,unsigned char *s ) { /*___________________________*/
/****************************************************************************/
	int i;
	bcb.lev = offs; bcb.cur = bcb.buf + bcb.lev;
	for( i=0;i<len;i++ ) if( s[i] != Nextchar() ) break;
	if( i<len || s[i] ) return( 0 );
	return( 1 );
	}
/****************************************************************************/
alloc_att_tab() { /*________________________________________________________*/
/****************************************************************************/
#define  INITSIZE  1
#define  INCRSIZE  1
	if( att_tab_len ) {
		att_tab_len += INCRSIZE;
		tab = (att_tab_descr*) realloc( tab,att_tab_len * sizeof(att_tab_descr) );
		}
	else {
  	att_tab_len = INITSIZE;
		tab = ( att_tab_descr* )malloc( att_tab_len * sizeof( att_tab_descr ) );
		}
  return;
	}
#undef   INITSIZE
#undef   INCRSIZE
/****************************************************************************/
alloc_val_tab() { /*________________________________________________________*/
/****************************************************************************/
#define  INITSIZE  2
#define  INCRSIZE  1
	if( tab[cur_attr].len ) {
		tab[cur_attr].len += INCRSIZE;
		tab[cur_attr].val =	( val_tab_descr* )realloc( tab[cur_attr].val,
												tab[cur_attr].len * sizeof( val_tab_descr ) );
		}
	else {
    tab[cur_attr].len = INITSIZE;
		tab[cur_attr].val =
		( val_tab_descr* )malloc( tab[cur_attr].len * sizeof( val_tab_descr ) );
		}
  return;
	}
#undef   INITSIZE
#undef   INCRSIZE
