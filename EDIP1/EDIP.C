#include   "edip.h"
#include   <sys\stat.h>

/*__________________________________________________________________________*/
int huge *matr_buf;
unsigned int matr_buf_len=0;
unsigned int m_str_num=0;
unsigned int not_str_num=0;
int          attr_num;
bufdescr bcb;
int  explain = 0;
int  PassNum;
char basname[13], modname[13];
int modh;
/*__________________________________________________________________________*/
int        bash;
/*__________________________________________________________________________*/
char getchr();
/****************************************************************************/
main( int argc,char *argv[] ) {
/****************************************************************************/
	char path[MAXPATH], drive[MAXDRIVE], dir[MAXDIR];
  char name[MAXFILE], ext[MAXEXT];
  
	puts( "" );
	printf( "Expert System Shell EDIP Command-line Compiler, Version 1.1, 1990-1993\n" );
	printf( "Phone: (042+2) 73-81-30, " );
	putchar('A'); putchar('.'); putchar('A'); putchar('.');
	putchar('S'); putchar('a'); putchar('v'); putchar('i');
	putchar('n'); putchar('o'); putchar('v');
	putchar('\n');
  puts( "" );
  PassNum = 10000;
  if ( argc == 2 ) {
	fnsplit( argv[1], drive, dir, name, ext );
    }
  else if ( argc == 3 ) {
  	if ( argv[1][0] == '/' && (argv[1][1] == 'p' || argv[1][1] == 'P') ) {
		if ( argv[1][2] == '=' ) {
			if ( argv[1][3] == '0' && argv[1][4] == '\0' ) PassNum = 0;
            else
			  if ( ! (PassNum = atoi ( & (argv[1][3]) )) ) { usage(); exit( 1 ); }
            }
        else if ( isdigit ( argv[1][2] ) ) {
			if ( ! (PassNum = atoi ( & (argv[1][2]) )) ) { usage(); exit( 1 ); }
            }
		else { usage(); exit( 1 ); }
		}
    else { usage(); exit( 1 ); }
  	fnsplit( argv[2], drive, dir, name, ext );
    }
  else {
		usage();
    exit( 1 );
    }
  
  if( ext[0] == 0 ) {                              /* there is no extension */
   	ext[0] = '.'; ext[1] = 'B'; ext[2] = 'A'; ext[3] = 'S';
    }
  fnmerge( path, drive, dir, name, ext );
  fnmerge( basname, drive, NULL, name, ext );
  fnmerge( modname, drive, NULL, name, ".MOD" );

  if( (bash = _open( path,O_RDONLY ) ) == -1 ) {               /* open file */
		printf( "ERROR: Can not open file %s\.n", path );              /* ERROR */
    exit( 1 );
		}
  bcb.siz = filelength( bash );
  bcb.buf = ( char far * )farmalloc( bcb.siz );          /* allocate buffer */
  if( !bcb.buf ) {
		printf( "ERROR: Not enough memory to allocate file buffer.\n");/* ERROR */
    _close( bash );
    exit( 1 );
    }
  if( _read( bash, bcb.buf, bcb.siz ) != bcb.siz ) { /* read file to buffer */
  	printf( "ERROR: Can not read file %s.\n", path );               /* ERROR */
    farfree( bcb.buf );
    _close( bash );
    exit( 1 );
    }
	bcb.fst = 0;
	bcb.lst = bcb.siz - 1;
	bcb.cur = bcb.buf;
  bcb.row = 1;

  printf( "Compiling %s file to %s file.\n", basname, modname );
  parser();
  printf( "There are %d attributes and %d rules.\n", attr_num, m_str_num );
  code_generator();

  modh = open( modname, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, 
	                      S_IREAD | S_IWRITE );
  if( modh == -1 ) {
		printf( "ERROR: Can not open file %s.\n", modname ); 
    perror( "" );
 	  exit( 1 );
		}
  make_mod();
  close( modh );

  farfree( bcb.buf );
	_close( bash );
  return;
	}
/****************************************************************************/
usage() { /* ___________________________OUTPUTS_DESCRIPTION_OF_THE_USAGE___ */
/****************************************************************************/
  printf( "Usage:     EDIP [/p[=]xxx] filename[.BAS]\n" );
  printf( "filename - Source knowledge base file name to compile,\n" );
  printf( "           .BAS is default extension.\n" );
  printf( "/p       - option which indicates the maximal number of passes; \n" );
  printf( "           if it is omitted then all passes are fulfilled and \n" );
  printf( "           inference engine guarantees finding all conclusions; if it  \n" );
  printf( "           is equal to 0, then the knowledge base is not compiled \n" );
  printf( "xxx        the number of passes. \n" );
  return;
	}
/****************************************************************************/
char getchr() {             /* ___________RECEIVES_CHARACTER_FROM_BUFFER___ */
/****************************************************************************/
	if( bcb.lev <= bcb.lst && bcb.lev >= bcb.fst ) {
  	if( *bcb.cur == '\n' ) bcb.row++ ;
  	bcb.lev++;
		return( *bcb.cur++ );
    }
	else {
		return( EOF );
		}
	}
