/*******************************************************************
 *
 *     Data Structures for WACOM Windows Mouse Driver Interface
 *
 *******************************************************************/

typedef struct tagTDATA
  {
    int     x;			/* absolute X coordinate */
    int     y ; 		/* absolute Y coordinate */
    char    button;		/* button bits (L=0, R=1, M=2) */
    char    contact;		/* non-zero if on tablet */
    char    stylus;		/* non-zero if stylus */
    char    pres_val;		/* pressure reading */
    char    pressure;		/* non-zero if pressure pen */
    char    fill [7];		/* for future use */
  } TDATA;

typedef struct tagTPARMS
  {
    char    pressure;		/* non-zero if pressure pen */
    char    sensitivity;	/* pressure threshold (-32 to 32) */
    char    absmode;		/* bit 0 : absolute area enabled */
				/* bit 1 : relative mode enabled */
    int     xcorner;		/* x corner coordinte in thousandths */
    int     ycorner;		/* y corner coordinate in thousandths */
    int     awidth;		/* width of absolute area in thousandths */
    int     aheight;		/* height of absolute area in thousandths */
    int     xoffset;		/* mapping offset (usually 0) */
    int     yoffset;		/* mapping offset (usually 0) */
    int     xdivisor;		/* mapping divisor (usually awidth) */
    int     ydivisor;		/* mapping divisor (usually aheight) */
    char    fill [9];		/* for future use */
  } TPARMS;

typedef TDATA FAR	*LPTDATA;
typedef TPARMS FAR	*LPTPARMS;

int far PASCAL GetTabletData (LPTDATA);
int far PASCAL GetTabletParms (LPTPARMS);
int far PASCAL SetTabletParms (LPTPARMS);
