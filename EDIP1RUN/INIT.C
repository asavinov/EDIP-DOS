#include <alloc.h>
#include <conio.h>
#include "sw.h"

extern   struct  attw_struct {
	int      atn;                /* a quantity of the attributes in the module */
	int      csv;   /* the attribute number, a vertical position of the cursor */
	int      csh;   /* a horizontal position of the cursor in the attr. window */
	int      up;    /* the number of the upper string in the attributes window */
	}      acb;
extern   struct  valw_struct {
	int      vln;                /* quantity of values of the active attribute */
	int      csv;                  /* the number of value, a vertical position */
	int      csh;  /* a horizontal position of the cursor in the values window */
	int      up;        /* the number of the upper string in the values window */
	long     ofs[16]; /* a displacements to the values of the active attribute */
	}      vcb;
extern w_descr  all_scr,att_w,val_w;
extern wc_descr att_wc,val_wc;

/*****************************************************************************/
initscreen() {  /* _________________________________SCREEN_INITIALIZATION___ */
/*****************************************************************************/
	sw_Store( all_scr );
	sw_draw( &att_w,&att_wc ); initatt_at();
	sw_puts( 61,2,"Inf",0x0c ); sw_puts( 67,2,"Exp",0x0c );
	sw_puts( 74,2,"Hyp",0x0c );
	scr[2][60].ch = scr[2][65].ch = scr[2][72].ch = 194;                  /* Â */
	sw_dye_ch( 60,3,60,14,179 ); sw_dye_ch( 65,3,65,14,179 );             /* ³ */
	sw_dye_ch( 72,3,72,14,179 );                                          /* ³ */
	scr[15][60].ch = scr[15][65].ch = scr[15][72].ch = 193;               /* Á */
	sw_draw( &val_w,&val_wc ); initval_at();
	sw_puts( 57,16,"Fct",0x0c ); sw_puts( 61,16,"Inf",0x0c );
	sw_puts( 67,16,"Exp",0x0c ); sw_puts( 74,16,"Hyp",0x0c );
	scr[16][56].ch = scr[16][60].ch = \
	scr[16][65].ch = scr[16][72].ch = 194;                                /* Â */
	sw_dye_ch( 56,17,56,22,179 ); sw_dye_ch( 60,17,60,22,179 );           /* ³ */
	sw_dye_ch( 65,17,65,22,179 ); sw_dye_ch( 72,17,72,22,179 );           /* ³ */
	scr[23][56].ch = scr[23][60].ch = \
	scr[23][65].ch = scr[23][72].ch = 193;                                /* Á */
	sw_dye_at( 9,2,29,2,0x5f ); sw_dye_at( 9,16,21,16,0x0c );
	sw_dye_at( 0,0,79,0,0x10 ); sw_dye_at( 0,24,79,24,0x10 );
	sw_dye_ch( 0,0,79,1,32 );   sw_dye_ch( 0,24,79,24,32 );
	sw_dye_at( 0,1,79,1,0x0f );
	window( 1,1,80,25 ); gotoxy( 4,1 ); textattr( 0x1b );
	cputs("Knowledge   Consultation  Inference   Explanation   Hypothesis   Options");
	scr[0][3].at  = scr[0][15].at = scr[0][29].at = \
	scr[0][41].at	= scr[0][55].at = scr[0][68].at = 0x1c;
  sw_puts( 1,1,"Goal ÄÄ Att:      Val:    ",0x07 );
  sw_puts( 29,1,"Mod. name",0x07 ); sw_puts( 55,1,"Cons. name",0x07 );
/*  sw_puts( 1,24,
	"^F1-Comment F2-Save F3-Load F6-Goal F7-Hyp F8-Expl F9-Infer F10-Menu",
  0x1b );   */
	}
/*****************************************************************************/
initatt_at() {  /* _____________DISPLAY_ATTRIBUTES_INITIALIZATION_IN_attw___ */
/*****************************************************************************/
	sw_dye_at( 1 ,3,3 ,14,0x07 );                                    /* number */
	sw_dye_at( 4 ,3,59,14,0x0e );                            /* attribute name */
	sw_dye_at( 60,3,60,14,0x0d );                                      /* line */
	sw_dye_at( 61,3,64,14,0x0e );                                 /* inference */
	sw_dye_at( 65,3,65,14,0x0d );                                      /* line */
	sw_dye_at( 66,3,71,14,0x0f );                               /* explanation */
	sw_dye_at( 72,3,72,14,0x0d );                                      /* line */
	sw_dye_at( 73,3,78,14,0x03 );                                /* hypothesis */
	}
/*****************************************************************************/
initval_at() {  /* _____________DISPLAY_ATTRIBUTES_INITIALIZATION_IN_valw___ */
/*****************************************************************************/
	sw_dye_at( 1 ,17,2 ,22,0x07 );                                   /* number */
	sw_dye_at( 3 ,17,55,22,0x0e );                               /* value name */
	sw_dye_at( 56,17,56,22,0x0d );                                     /* line */
	sw_dye_at( 57,17,59,22,0x0b );                                    /* value */
	sw_dye_at( 60,17,60,22,0x0d );                                     /* line */
	sw_dye_at( 61,17,64,22,0x0e );                                /* inference */
	sw_dye_at( 65,17,65,22,0x0d );                                     /* line */
	sw_dye_at( 66,17,71,22,0x0f );                              /* explanation */
	sw_dye_at( 72,17,72,22,0x0d );                                     /* line */
	sw_dye_at( 73,17,78,22,0x03 );                               /* hypothesis */
	}
/*****************************************************************************/
mrkattcs() {  /* _______________MARKS_CURSOR_IN_THE_ATTRIBUTE_WINDOW_attw___ */
/*****************************************************************************/
	int x = acb.csv - acb.up +3;
	sw_dye_at( 1 ,x,3 ,x,0x4f );                 							       /* number */
	sw_dye_at( 4 ,x,59,x,0x4a );                                 /* attr. name */
	sw_dye_at( 60,x,60,x,0x4d );                                       /* line */
	sw_dye_at( 61,x,64,x,0x4e );                                  /* inference */
	sw_dye_at( 65,x,65,x,0x4d );                                       /* line */
	sw_dye_at( 66,x,71,x,0x4f );                                /* explanation */
	sw_dye_at( 72,x,72,x,0x4d );                                       /* line */
	sw_dye_at( 73,x,78,x,0x43 );                                 /* hypothesis */
	}
/*****************************************************************************/
mrkvalcs() {  /* ___________________MARKS_CURSOR_IN_THE_VALUE_WINDOW_valw___ */
/*****************************************************************************/
	int x = vcb.csv - vcb.up+17;
	sw_dye_at( 1 ,x,2 ,x,0x2f );                                     /* number */
	sw_dye_at( 3 ,x,55,x,0x24 );                                 /* value name */
	sw_dye_at( 56,x,56,x,0x2d );                                       /* line */
	sw_dye_at( 57,x,59,x,0x2c );                                      /* value */
	sw_dye_at( 60,x,60,x,0x2d );                                       /* line */
	sw_dye_at( 61,x,64,x,0x2c );                                  /* inference */
	sw_dye_at( 65,x,65,x,0x2d );                                       /* line */
	sw_dye_at( 66,x,71,x,0x2c );                                /* explanation */
	sw_dye_at( 72,x,72,x,0x2d );                                       /* line */
	sw_dye_at( 73,x,78,x,0x2c );                                 /* hypothesis */
	}
/*****************************************************************************/
clrattcs() {  /* ______________CLEARS_CURSOR_IN_THE_ATTRIBUTE_WINDOW_attw___ */
/*****************************************************************************/
	int x = acb.csv - acb.up +3;
	sw_dye_at( 1 ,x,3 ,x,0x07 );                                     /* number */
	sw_dye_at( 4 ,x,59,x,0x0e );                                 /* attr. name */
	sw_dye_at( 60,x,60,x,0x0d );                                       /* line */
	sw_dye_at( 61,x,64,x,0x0e );                                  /* inference */
	sw_dye_at( 65,x,65,x,0x0d );                                       /* line */
	sw_dye_at( 66,x,71,x,0x0f );                                /* explanation */
	sw_dye_at( 72,x,72,x,0x0d );                                       /* line */
	sw_dye_at( 73,x,78,x,0x03 );                                 /* hypothesis */
	}
/*****************************************************************************/
clrvalcs() {  /* __________________CLEARS_CURSOR_IN_THE_VALUE_WINDOW_valw___ */
/*****************************************************************************/
	int x = vcb.csv - vcb.up+17;
	sw_dye_at( 1 ,x,2 ,x,0x07 );                                     /* number */
	sw_dye_at( 3 ,x,55,x,0x0e );                                 /* value name */
	sw_dye_at( 56,x,56,x,0x0d );                                       /* line */
	sw_dye_at( 57,x,59,x,0x0b );                                      /* value */
	sw_dye_at( 60,x,60,x,0x0d );                                       /* line */
	sw_dye_at( 61,x,64,x,0x0e );                                  /* inference */
	sw_dye_at( 65,x,65,x,0x0d );                                       /* line */
	sw_dye_at( 66,x,71,x,0x0f );                                /* explanation */
	sw_dye_at( 72,x,72,x,0x0d );                                       /* line */
	sw_dye_at( 73,x,78,x,0x03 );                                 /* hypothesis */
	}
