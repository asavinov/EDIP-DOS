#include   "edip_l.h"
#include   <sys\stat.h>
/*__________________________________________________________________________*/
int huge *matr_buf;
unsigned int matr_buf_len=0;
unsigned int m_str_num=0;
int          attr_num;
bufdescr bcb;
int  explain;
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
	printf( "Expert System Shell EDIP Command-line Compiler, Version 2.0 - quick, 1990\n" );
  printf( "Phone: (0422) 217131, " );
	putchar('S'); putchar('a'); putchar('v'); putchar('i');
	putchar('n'); putchar('o'); putchar('v'); putchar(' ');
	putchar('A'); putchar('l'); putchar('e'); putchar('x');
	putchar('a'); putchar('n'); putchar('d'); putchar('e');
  putchar('r'); putchar('.'); putchar('\n');
  puts( "" );
  if( argc == 2 ) {
    explain = 0;
  	fnsplit( argv[1], drive, dir, name, ext );
    }
  else if( argc == 3 ) {
  	if( argv[1][0] == '/' && argv[1][1] == 'e' && argv[1][2] == '\0' ) {
    	explain = 1;
      }
    else {
    	usage();
      exit( 1 );
      }
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
  printf( "%d attributes and %d rules in knowledge base.\n", attr_num, m_str_num );
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
	printf( "Usage:     EDIP [/e] filename[.BAS]\n" );
  printf( "filename - Source knowledge base file name to compile,\n" );
  printf( "           .BAS is default extension.\n" );
  printf( "/e       - option to create module with the emproved explaination\n" );
  printf( "           (now this possibility is not used by user interface);\n" );
  printf( "           note that using this option will require more compilation time,\n" );
  printf( "           and the resulted .MOD file will be greater;\n" );
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
