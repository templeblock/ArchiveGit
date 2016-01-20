typedef unsigned short ushort;
typedef unsigned char uchar;

const long  SIDEBITS = 8;
const long  SIDE     = 1L << SIDEBITS;
const long  PTS      = (SIDE * SIDE);
const long  ROUND    = (1L << FIX) >> 1;
const short YFIX     = 2;
const int   FOVBITS  = 7;
const int   FOV      = 1 << FOVBITS;
const int   MAXD     = 60;

extern uchar *Surface;
extern uchar *Colors;
extern ushort *gPrj;

#define Point(x,y) (Surface[((x)&(SIDE-1)) + (((long)((y)&(SIDE-1))) << SIDEBITS)])
#define Color(x,y) (Colors[((x)&(SIDE-1)) + (((long)((y)&(SIDE-1))) << SIDEBITS)])

#define Unproject(vx, z)\
    (((vx) * (z)) >> FOVBITS)
#define Project(x, z)\
    (((x) << FOVBITS) / (z))
    
extern char ISurface();
extern void BuildProjectionTable();

