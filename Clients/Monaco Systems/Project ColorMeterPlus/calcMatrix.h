//calcMatrix.h
struct XYZ {
	double  X;
	double  Y;
	double  Z;
};
typedef struct XYZ XYZ;

struct UVW {
	double U;
	double V;
	double W;
};
typedef struct UVW   UVW;

typedef struct Lab {
	float L;
	float a;
	float b;
}Lab;

typedef struct LHC {
	float Lum;
	float Hue;
	float Chroma;
} LHC;

/*** LUV structure is for U* V* L* data ***/

struct LUV {
	double L;
	double U;
	double V;
};
typedef struct LUV   LUV;

struct HVC {
	double H;
	double V;
	double C;
};
typedef struct HVC   HVC;


typedef struct RGB {
	double r;
	double g;
	double b;
}RGB;
	
struct xyL {
	double  x;
	double  y;
	double  z;
	double  L;
} ;
typedef struct xyL xyL;
	
struct uvL {
	double  u;
	double  v;
	double  w;
	double  L;
} ;
typedef struct uvL uvL;

struct cdif {
	double  U1; /* This is equivelent to U*V*W* */
	double  V1;
	double  W1;
} ;


typedef struct blackbody_point{
double ColorTemp;
double u;
double v;
}BlackBodyPoint;


void calcMatrix(ReferenceFile *MonitorData);

void calcScalars(ReferenceFile *MonitorData);


void Invert ( double *matptr,  short order );

void Trans ( double *inptr, short nrow, short ncol, double *outptr );

short MatMult ( double *aptr, short nrow, short pcol, double *bptr, short prow, short ncol, double *outptr);
  
short A_solve(double *c_mat,short c_rows,short c_cols,double *b_mat,short b_rows,short b_cols, double *a_mat);

//ColorTemp routines


short Kelvin2xy(double ColorTemp,double *x, double *y);

short find_yd(double x,double *yd) ;

double xy2Kelvin(double x, double y);


/* Routines from Color.c */

void Find_XYZRGB (xyL *Red,xyL *Green,xyL *Blue,xyL *White,double *TheMatrix);

void xyL_XYZ ( xyL *xyLptr, XYZ *XYZptr );

void xyL_uvL( xyL *xyLptr, uvL *uvLptr );

