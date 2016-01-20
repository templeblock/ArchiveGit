/* 

	The curve control 

	Based on MonacoCOLOR curve dialog
	
	(c) Copyright 1993-1997 Monaco Systems Inc.
	By James Vogh     */


#include <QDOffscreen.h>
#include "curvecont.h"
#include "spline.h"
#include "splint.h"
#include "picelin.h"
#include "pascal_string.h"


#define R_LUT_COLORS {	  0, 30000, 30000, 45000, 	  0, 40000, 40000, 30000,     0,     0,     0, 60000, 63000, 60000}
#define G_LUT_COLORS {30000, 	 0, 30000, 45000, 40000,	 0, 40000, 30000, 60000,     0, 60000,     0, 63000, 60000}
#define B_LUT_COLORS {30000, 30000, 	0, 45000, 40000, 40000, 	0, 30000,     0, 60000, 60000, 60000,     0, 60000}


#define BACKGROUND { 65535,65535,65535}

#define CURVE_H	14
#define CURVE_V 14

#define CURVE_ON { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
					0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, \
					0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, \
					0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, \
					0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, \
					0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
					0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, \
					0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
					0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, \
					0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, \
					0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, \
					0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, \
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
					
#define CURVE_OFF {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
				  	0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, \
				  	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
				    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }


#define NUM_COLOR_NAMES 12
#define COLOR_NAME_ID 	17013

#define MAX_DIST 0.08		// max distance is 0.125
#define MAX_DIST_CURVE 6 // distance from clickpoint to curve

#define	MinVal(a,b)			(((a) > (b)) ? (b) : (a))
#define	MaxVal(a,b)			(((a) < (b)) ? (b) : (a))

#define round(a)	(a+0.5)

//#include <sane.h>

// The constructor

CurveControl::CurveControl(WindowPtr wp, short *control_ids,int startNum,int hf, int lf,baseWin *pw,int cn):control(wp)
{
limitExtend = lf;
handFlag = hf;
parentWin = pw;
controlNum = cn;
_init(control_ids,startNum);
}

CurveControl::~CurveControl()
{
dp = 0L;
}

/* Set up the control */

McoStatus CurveControl::_init( short *control_ids,int startNum)
{
	McoStatus	state = MCO_SUCCESS;
	int16		i,j,k,l;
	short 		itemType;
	short		item;
	Handle 		cn1,h;
	Rect 		r;

	long		RED[] = R_LUT_COLORS;
	long		GREEN[] = G_LUT_COLORS;
	long		BLUE[] = B_LUT_COLORS;
	
	int16		ma_back[] = BACKGROUND;
	
	short 		wi,hi;

	c_text1_num = -1;
	c_text2_num = -1;

	// copy the ids

	for (i=0; i<NUM_CURVE_CNT_IDS; i++) ids[i] = control_ids[i];
	for (i=0; i<NUM_CURVE_DIAL_IDS; i++) if (ids[i] > -1) ids[i] += startNum;


	// set colors

	background.red=ma_back[0];
	background.green=ma_back[1];
	background.blue=ma_back[2];

			
	for (i=0; i<NUM_HP_IDS; i++) 
		{
		REDS[i]=RED[i];
		GREENS[i]=GREEN[i];
		BLUES[i]=BLUE[i];
		}

	// set up curve stuff

	current_cmp = 0;	
	active_lut=-1;
	active_cmp=-1;
	active_hand=-1;
	


	GetDItem (dp, ids[C_CurveWin], &itemType, &h, &wr);

	wr.top += ids[C_Top];
	wr.left += ids[C_Left];

	wr.bottom -= ids[C_Bottom];
	wr.right -= ids[C_Right];
	
	PointsInCurve = (wr.right-wr.left)/2;
	
	lastmenu = 1;	



	return state;
}

void CurveControl::SetUpCurves(int num_lut,int *num_cmp,double *hmi,double *hma, double *vmi, double *vma)
{
int i,j;

num_luts = num_lut;

for (i=0; i<num_luts; i++)
	{
	num_cmps[i] = num_cmp[i];
	active_luts[i] = 1;
	hmin[i] = hmi[i];
	hmax[i] = hma[i];
	vmin[i] = vmi[i];
	vmax[i] = vma[i];
	for (j=0; j<num_cmps[i]; j++) 
		{
		num_hands[i][j] = 0;
		lut_color[i][j] = j;
		active_cmps[i][j] = 0;
		freshen_cmp[i][j] = 0;
		curve_type[i][j] = C_SplineCurve;
		}	
	}

if ((active_lut != 0) && (active_lut != 1)) active_hand = -1;
active_luts[0] = 1;
active_luts[1] = 1;
for (i=0; i<num_luts; i++)
	for (j=0; j<num_cmps[i]; j++)
		{
		active_cmps[i][j]=1;
		freshen_cmp[i][j]=1;
		}	

}


McoStatus CurveControl::CopyIntoCurves(int ln, int cn,int num_h,double xmi,double xma, double ymi, double yma,double *datax,double *datay)
{
int i;

num_hands[ln][cn] = num_h;

for (i=0; i<num_h; i++)
	{
	h_x[ln][cn*MaxNumHands+i] = (datax[i]-xmi)/(xma-xmi);
	h_y[ln][cn*MaxNumHands+i] = (datay[i]-ymi)/(yma-ymi);
	}
//active_cmps[ln][cn] = 1;
freshen_cmp[ln][cn] = 1;
_getPoints();
_drawCurveWin(); 
return MCO_SUCCESS;
}


void CurveControl::CopyOutofCurves(int ln, int cn,int *num_h,double xmi,double xma, double ymi, double yma,double *datax,double *datay)
{
int i;

*num_h = num_hands[ln][cn];

for (i=0; i<*num_h; i++)
	{
	datax[i] = h_x[ln][cn*MaxNumHands+i]*(xma-xmi) + xmi;
	datay[i] = h_y[ln][cn*MaxNumHands+i]*(xma-xmi) + xmi;
	}
}


// Set up the colors menu 

void CurveControl::_setColorMenu(void)
{
	Str255		theString;
	int			i;
	MenuHandle 	theMenu;
	McoStatus	state;
	int			three_ID;
	

if (ids[C_ColorMenu] == -1) return;

theMenu=GetMenu(ids[C_ColorMenuID]);	

	
for (i=0; i<5; i++)	DelMenuItem(theMenu,1);		


i=1;
do
	{
	GetIndString(theString,ids[C_Menu_STRS_ID],i);
	InsMenuItem(theMenu,theString,i);
	i++;
	}
while ((short)theString[0]!=0);

}	

// redraw the control
McoStatus 	CurveControl::CopyParameters(void)
{
int i,j;

short 			itemType;
Handle 			cn1;
Rect 			r;
int				item_val;

	if (ids[C_ColorMenu] == -1) return MCO_SUCCESS;

	GetDItem (dp, ids[C_ColorMenu] , &itemType,&cn1,&r);
	SetCtlValue((ControlHandle)cn1,lastmenu);

return MCO_SUCCESS;
}


// Get the number of the handle that is at the specified location

int16 CurveControl::_getHandNum(int x, int y)
{
	int i,j,k,min_d,min_c,min_l,index;
	long 	tx,ty;
	double	min_dist,d,fx,fy,thx,thy;


min_l=-1;
min_d=-1;
min_dist=1000000000;

// translate from screen to curve coordinates
tx = MinVal(x,wr.right);
tx = MaxVal(x,wr.left);
tx = tx - wr.left;
fx = (double)tx/(wr.right-wr.left);
ty = MinVal(y,wr.bottom);
ty = MaxVal(y,wr.top);
ty = wr.bottom - ty;
fy = (double)ty/(wr.bottom-wr.top);


// search for the closest handle
for (i=num_luts -1; i>=0; i--)
  {
   if (active_luts[i]==1)
	{
	for (j=0; j<num_cmps[i]; j++)
		{
		if (active_cmps[i][j])
			{
			for (k=0; k<num_hands[i][j]; k++)
				{
				thx = h_x[i][j*MaxNumHands+k];	// the coordinates of the handle
				thy = h_y[i][j*MaxNumHands+k];
				
				// calculate the distance
				d=(fx-thx)*(fx-thx)+(fy-thy)*(fy-thy);
				if (d<min_dist) 
					{
					min_l=i;
					min_c=j;
					min_d=k;
					min_dist=d;
					} 
				}
			}
		}
	}
  }
// select the active handle if a handle was close enough to the mouse click
if (min_dist<=MAX_DIST*MAX_DIST) 
	{
	active_hand=min_d;
	active_cmp=min_c;
	active_lut=min_l;
	}
else 
	{
	active_hand=-1;
	active_cmp=-1;
	}
return -1;
} 


// add a point to a curve 

McoStatus CurveControl::_addHandle(int16 i, int16 j, double x, double y)
{
int k,l,m;

// First get the new handle number
if (num_hands[i][j] >= MaxNumHands) return MCO_OUT_OF_RANGE;
for (k=0; k<num_hands[i][j]; k++) 
	{
	m = j*MaxNumHands+k;
	if (x < h_x[i][m]) 
		{
		for (l=(j+1)*MaxNumHands-1; l>m; l--) // make space in the array
			{
			h_x[i][l] = h_x[i][l-1];
			h_y[i][l] = h_y[i][l-1];
			}
		// now add the new handle
		num_hands[i][j]++;	
		h_x[i][m] = x;
		h_y[i][m] = y;
		active_hand=k;
		active_cmp=j;
		active_lut=i;
		return MCO_SUCCESS;
		}
	}
return MCO_OUT_OF_RANGE;
}

// delete a handle

McoStatus CurveControl::_deleteHandle(int16 i, int16 j, int16 k)
{
int l,m;

m = j*MaxNumHands+k;

if ((k != 0) && (k !=  num_hands[i][j] - 1))  // make sure it is not the first or las point
	{
	for (l=m; l<(j+1)*MaxNumHands-1; l++)	// move the data
		{
		h_x[i][l] = h_x[i][l+1];
		h_y[i][l] = h_y[i][l+1];
		}
	num_hands[i][j]--;				
	active_hand=-1;			// make no handle active
	active_cmp=-1;
	freshen_cmp[i][j] = 1;
	}
return MCO_SUCCESS;
		
}
			

// make a curve linear again by removing all but the first and last handles and
// then setting them to min and max
			
McoStatus CurveControl::_resetCurve(int16 i, int16 j)
{

int k,l,m;

l = num_hands[i][j];

for (k=1; k <l-1 ; k++)
	{
	_deleteHandle(i,j,1);
	}
h_x[i][j*MaxNumHands] = 0;
h_y[i][j*MaxNumHands] = 0;
h_x[i][j*MaxNumHands+1] = 1.0;
h_y[i][j*MaxNumHands+1] = 1.0;
active_cmp = j;
if (active_hand > -1) active_hand = 0;
freshen_cmp[i][j] = 1;
return MCO_SUCCESS;
}


/* Select the curve that is closest, if it is close enough */
/* This version works in screen coords */


Boolean CurveControl::_getCurveNum(int x, int y)
{
	int i,j,k,l,min_d,min_c,min_l,index;
	long 	d,min_dist,tx,tx2,ty,thx,thy;
	double	fx,fy;

min_l=-1;
min_d=-1;
min_dist=1000000000;


// get coordinates in both screen and curve
tx = MinVal(x,wr.right);
tx = MaxVal(x,wr.left);
tx = tx - wr.left;
fx = (double)tx/(wr.right-wr.left);
tx = (int)(fx*PointsInCurve);

ty = MinVal(y,wr.bottom);
ty = MaxVal(y,wr.top);
ty = wr.bottom - ty;
fy = (double)ty/(wr.bottom-wr.top);
ty = (wr.bottom-wr.top) - ty;


for (i=num_luts-1; i>=0; i--)
	{
	if (active_luts[i]==1)
		{
		for (j=0; j<num_cmps[i];  j++)
			{
			if (active_cmps[i][j])
				{
				thy = points[i][tx+j*MAX_POINTS];
				// if close enough then add a handle
				if ((thy-ty)*(thy-ty) <  MAX_DIST_CURVE*MAX_DIST_CURVE)
					{
					_addHandle(i,j,fx,fy);
					return TRUE;
					}
				}
			}
		}
	}
return FALSE;
}

// draw a handle into a gworld


McoStatus CurveControl::_copyData24to32Or(GWorldPtr src,unsigned char *data,int32 dataRowBytes,Rect *dataRect, int count)
{
	Rect r2;
	Handle h;
	short itemType;
	Str255	s;

PixMapHandle	srcPixMap;
short			srcRowBytes;
long			*srcBaseAddr;
long			*srcAddr1;
long			*copyBaseAddr;
long			*copyAddr;
unsigned char			*dataBaseAddr;
unsigned char			*dataAddr;
signed char			mmuMode;
short			row, column;
short			width;
short			height;
GDHandle	oldGD;
GWorldPtr	oldGW;
//return MCO_SUCCESS;
	srcPixMap = GetGWorldPixMap ( src );

	if( LockPixels ( srcPixMap ) ) 
	{
		GetGWorld(&oldGW,&oldGD);

		srcBaseAddr = (long *) GetPixBaseAddr ( srcPixMap );	/* get the address of the pixmap */
		srcRowBytes = (**srcPixMap).rowBytes & 0x7fff;			/* get the row increment */
		
		dataBaseAddr = data;
		
		if ((**srcPixMap).bounds.top > (dataRect->top)) 
			{
			dataBaseAddr += ((**srcPixMap).bounds.top - (dataRect->top))*dataRowBytes;	
			dataRect->top = (**srcPixMap).bounds.top;
			}
		
		if ((**srcPixMap).bounds.left > (dataRect->left)) 
			{
			dataBaseAddr += ((**srcPixMap).bounds.left - (dataRect->left))*3;
			dataRect->left = (**srcPixMap).bounds.left;
			}
			
		srcBaseAddr += ((int32)dataRect->top-(int32)(**srcPixMap).bounds.top)*srcRowBytes/4 + 
			((int32)dataRect->left-(int32)(**srcPixMap).bounds.left);
			
			
		if (dataRect->right > (**srcPixMap).bounds.right) width = (**srcPixMap).bounds.right - dataRect->left;
		else width = dataRect->right-dataRect->left;
		
		if (dataRect->bottom > (**srcPixMap).bounds.bottom) height = (**srcPixMap).bounds.bottom - dataRect->top;
		else height = dataRect->bottom-dataRect->top;
	
		mmuMode = true32b;
 		SwapMMUMode ( &mmuMode );						/* set the MMU to 32-bit mode */
 		for ( row = 0; row < height; row++ ) 
		{
			srcAddr1 = srcBaseAddr;
			dataAddr = dataBaseAddr;
			for ( column = 0; column < width; column++ ) 
				{
				if ((dataAddr[0] != 0) || (dataAddr[1] != 0) || (dataAddr[2] != 0) )
					{
					*srcAddr1 = 0;	
					*srcAddr1 += (int32)(*dataAddr) << 16;
					dataAddr++;
					*srcAddr1 += (int32)(*dataAddr) << 8;
					dataAddr++;
					*srcAddr1 += (int32)(*dataAddr);
					dataAddr++; 
					srcAddr1++;					/* bump to next pixel */
					}
				else
					{
					dataAddr++;
					dataAddr++;
					dataAddr++; 
					srcAddr1++;					/* bump to next pixel */
					}
				}
	
			srcBaseAddr = (long *) ( (char *) srcBaseAddr +  srcRowBytes );	/* go to the next row */
			dataBaseAddr = (unsigned char *) ( (char *) dataBaseAddr +  dataRowBytes);
		}
 		SwapMMUMode ( &mmuMode );						/* restore the previous MMU mode */
		UnlockPixels ( srcPixMap );						/* unlock the pixmap */
	}
return MCO_SUCCESS;
}


//  Draw the handles the new version


void CurveControl::_drawHands(GWorldPtr src, Rect *r)
{
	Rect r2;
	Handle h;
	short itemType;
	Str255	s;

	Rect hr;
	PicHandle	hand_pic;
	int i,j,k,l,m,n,x,y,dx,dy;
	int32	*data;
	int32	index;
	int16	curve_on[] = CURVE_ON;
	int16 	curve_off[] = CURVE_OFF;
	unsigned char	curve_p[CURVE_H*CURVE_H*3];
	
if (!handFlag) return;

n = 0;	
for (i=0; i<num_luts; i++)
	{ 
	if (active_luts[i]) for (j=num_cmps[i]-1; j>=0; j--)	// if an active lut then draw handles
		{
		if (active_cmps[i][j]) 	// if an active componet then draw handles
			{
			for (k=0; k<num_hands[i][j]; k++)
				{
	
				dx = CURVE_H;
				dy = CURVE_V;
				// the coordinates in screen coords
				x=1+(h_x[i][j*MaxNumHands+k]*(r->right-r->left))+r->left;
				y=1+r->bottom-(h_y[i][j*MaxNumHands+k]*(r->bottom-r->top));
			
				SetRect(&hr,x-dx/2,y-dy/2,x+dx/2,y+dy/2);
	
				m = 0;
				if ((i == active_lut) && (j == active_cmp) && (k == active_hand))
					{
					for (l=0; l<CURVE_H*CURVE_H; l++) // draw an active hand
						{
						curve_p[m++] = curve_on[l]*(REDS[lut_color[i][j]+4]>>8);
						curve_p[m++] = curve_on[l]*(GREENS[lut_color[i][j]+4]>>8);
						curve_p[m++] = curve_on[l]*(BLUES[lut_color[i][j]+4]>>8);
						}
					}
				else 		
					{
					for (l=0; l<CURVE_H*CURVE_H; l++) // draw an inactive hand
						{
						curve_p[m++] = curve_off[l]*(REDS[lut_color[i][j]]>>8);
						curve_p[m++] = curve_off[l]*(GREENS[lut_color[i][j]]>>8);
						curve_p[m++] = curve_off[l]*(BLUES[lut_color[i][j]]>>8);
						}
					}
				_copyData24to32Or( src,curve_p,3*CURVE_H,&hr,k);	// draw into the offscreen gworld
				
				}
			}
		n++;
		}
	else n+=num_cmps[i];
	}
} 



				
/* Draw the curve onto the screen (actually into the GWorld) */

void CurveControl::_drawCurve(Rect *r,int16 *points, RGBColor *c)
{
	int i,k;
	short x,y;
	RGBColor	tc;
	int16 numpoints = r->right-r->left;
	int16 inc;


PenNormal();
PenSize(2,2);
GetForeColor(&tc);
RGBForeColor(c);
inc = numpoints/PointsInCurve;
k = 0;
while (points[k] == -1) k++;
MoveTo(r->left+k*inc,points[k]+r->top);
x = r->left+k*inc;
for (i=k+1; i<PointsInCurve; i++)
	{
	x += inc;
	y=points[i]+r->top;
	if (points[i] > -1) LineTo(x,y);
	}
RGBForeColor(&tc);
}

/* draw the curve window */

void CurveControl::_drawCurveWin(void)
{
	Rect r;
	Handle h;
	short itemType;
	RgnHandle	curClip;
	RGBColor	c,white;
	int			i,j,k,l,m;
	Str255		s;
	GrafPtr		oldgp;
	Str255		ColorNames[NUM_COLOR_NAMES];
	RGBColor	oldback,oldfore;

	QDErr		error;
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	GWorldPtr	BigGW;

	PixMapHandle myPixMapHandle;
	double		x,y;

	PicHandle	pict;
	short  red;
	WindowPtr	oldP;

#ifdef  NO_QD 
return;
#endif

GetPort(&oldP);
SetPort( dp ) ;

// load the color names from the resource 
for (i=0; i<NUM_COLOR_NAMES; i++)
	{	
	GetIndString(ColorNames[i],NUM_COLOR_NAMES,i+1);
	}

GetDItem (dp, ids[C_CurveWin], &itemType, &h, &r);

_getPoints();

GetBackColor(&oldback);



white.red = 65535;
white.green = 65535;
white.blue = 65535;

RGBBackColor(&white);

k=0;
l=0;
m=0;
 error = NewGWorld( &BigGW, 32, &r, 0, 0, 0 );	// create the gworld
if (error == 0)
	{
	GetGWorld(&oldGW,&oldGD);
	SetGWorld(BigGW,nil);
	myPixMapHandle = GetGWorldPixMap( BigGW );	// 7.0 only 
	LockPixels(myPixMapHandle);
	RGBBackColor(&oldback);
	EraseRect( &r );
	RGBBackColor(&background); 
	EraseRect( &wr );		// clear the GWorld 

	RGBBackColor(&white);

	if (ids[C_Pict] != -1)
		{
		pict = GetPicture(ids[C_Pict]);
		if (pict != 0L) 
			{
			DrawPicture(pict,&r);
			ReleaseResource((Handle)pict);
			}
		}
	
	for (i=0; i<num_luts; i++)
		for (j=num_cmps[i]-1; j>=0; j--)
			{
			if ((active_lut==i) && (active_cmp==j)) m=k;	// find the color of the curve
			if ((active_cmp==j) && (active_lut==i))
				{
				c.red=REDS[lut_color[i][j]+4];
				c.green=GREENS[lut_color[i][j]+4];
				c.blue=BLUES[lut_color[i][j]+4];
				}
			else
				{
				c.red=REDS[lut_color[i][j]];
				c.green=GREENS[lut_color[i][j]];
				c.blue=BLUES[lut_color[i][j]];
				}
			if ((active_luts[i]) && (active_cmps[i][j]))
				{
				_drawCurve(&wr,&points[i][j*MAX_POINTS],&c);
				}
			}
	SetGWorld(oldGW,oldGD);	
	_drawHands(BigGW,&wr);
	// draw onto the screen
	CopyBits( (BitMap*)*myPixMapHandle, (BitMap*)&dp->portBits, &r, &r, srcCopy, 0 );
	UnlockPixels(myPixMapHandle);
	DisposeGWorld( BigGW );
	}
RGBBackColor(&oldback);
// update the text in the dialog if the text is present
if ((active_lut > -1) && (active_cmp > -1) && (active_hand > -1))
	{
	x = h_x[active_lut][active_cmp*MaxNumHands+active_hand];
	x = x*(hmax[active_lut]-hmin[active_lut])+hmin[active_lut];
	y = h_y[active_lut][active_cmp*MaxNumHands+active_hand];
	y = y*(vmax[active_lut]-vmin[active_lut])+vmin[active_lut];

	SetItemTextToNum(ids[C_Text1],x);

	SetItemTextToNum(ids[C_Text2],y);

	SetItemText(ids[C_Text3],ColorNames[lut_color[active_lut][active_cmp]]);

	}
else 
	{
	c_text1_num = -1;
	c_text2_num = -1;

	SetItemText(ids[C_Text1],"\p");

	SetItemText(ids[C_Text2],"\p");

	SetItemText(ids[C_Text3],"\p");

	}	

SetPort(oldP);

}



/* Do the spline and get the points */

void CurveControl::_getPoints(void)
{

	double tx[MaxNumHands+1],ty[MaxNumHands+1],y2[MAX_POINTS+1],yd1,ydn,y3; 
	double first_y,last_y;
	double first_x, last_x;
	double kf;
	int i,j,k,k2,l,m;
	int16	lastx;
	Rect r;
	Handle h;
	short itemType;
	double	y_scale;
	
//GetDItem (dp,ids[C_CurveWin], &itemType, &h, &r);

r = wr;

y_scale = r.bottom-r.top;

l=0;
m=0;
for (i=0; i<num_luts; i++) 
	if (active_luts[i])
	{
	for (j=0; j<num_cmps[i]; j++)
	if ((active_cmps[i][j]) && (freshen_cmp[i][j]))
		{
		freshen_cmp[i][j] = 0;
		k2 = 0;
		lastx = -32000;
		if (num_hands[i][j] < 2)
			{
			l = j*MAX_POINTS;
			for (k=0; k<PointsInCurve; k++)
				{
				points[i][l++] = -1;
				}
			}
		else 
			{
			for (k=0; k<num_hands[i][j]; k++)		// load the array with handle data
				{
				tx[k+1]=(double)h_x[i][j*MaxNumHands+k]*PointsInCurve;
				ty[k+1]=(double)h_y[i][j*MaxNumHands+k]*y_scale;
				}
			k2 = k;
			yd1=(ty[2]-ty[1])/(tx[2]-tx[1]);
			ydn=(ty[k2]-ty[k2-1])/(tx[k2]-tx[k2-1]);
			switch (curve_type[i][j]) {
				case C_SplineCurve:spline(tx,ty,(int)k2,yd1,ydn,y2);	// if spline type then do spline
					break;
				}
			first_y = (double)h_y[i][j*MaxNumHands];						// clipping if first or last points are 
			first_y *= y_scale;
			last_y = (double)h_y[i][j*MaxNumHands + num_hands[i][j] - 1];// not zero or one
			last_y *= y_scale;

			
			first_x = (double)h_x[i][j*MaxNumHands];
			first_x *= PointsInCurve;
			last_x = (double)h_x[i][j*MaxNumHands + num_hands[i][j] - 1];
			last_x *= PointsInCurve;

			l = j*MAX_POINTS;
			for (k=0; k<PointsInCurve; k++)
				{
				switch (curve_type[i][j]) {
					case C_SplineCurve:
						if (k <= first_x) y3 = first_y;
						else if ( k >= last_x) y3 = last_y;
						else splint(tx,ty,y2,(int)k2,(double)k,&y3);  // calculate the y value with the spline
						break;
					case C_PicLinearCurve:
						piceLin(tx,ty,y2,(int)k2,(double)k,&y3);		// calculate with picewise linear function
						break;
					}
				points[i][l] = ((y_scale-(int16)round(y3)));
				points[i][l] = MinVal(points[i][l],(int16)y_scale);
				points[i][l] = MaxVal(points[i][l],0);
				if ((limitExtend) && ((k<first_x) || (k>last_x))) points[i][l] = -1;
				l++;
				}
			m++;
			}
		}
	else 		// curve does not need to be refreshed
		{
		m++;
		if ((active_cmps[i][j]) && !(freshen_cmp[i][j])) l += PointsInCurve;
		}
	} 
	
}


// The mouse is down and a handle is being draged 

Boolean CurveControl::_dragHand(void)
{
	int i,j,k,l,m;
	Point	where;

	Rect r;
	Handle h;
	short itemType;
	int16	old_hy;
	int16	left_limit,right_limit;
	double	x,y;
	int32	index;


old_hy = h_y[active_lut][active_cmp*MaxNumHands+active_hand];
//GetDItem (dp,ids[C_CurveWin], &itemType, &h, &r);	
r = wr;
l=0;
m=0;

_drawCurveWin();
if (!StillDown ( )) return FALSE;
if ((active_hand > -1) && (active_lut > -1))
	{
	while ( StillDown ( ) )	/* Hang arounnd until the button is released	*/
		{
		index = active_cmp*MaxNumHands+active_hand;

		GetMouse ( &where );
		where.v = MaxVal(where.v,r.top);
		where.v = MinVal(where.v,r.bottom);
		h_y[active_lut][index] =  	
			1-((double)where.v-(double)r.top)/((double)r.bottom-(double)r.top);

	//	where.h = MaxVal(where.h,r.left);
	//	where.h = MinVal(where.h,r.right);

		h_x[active_lut][index] = 
			((double)where.h-(double)r.left)/((double)r.right-(double)r.left);
		
		if (active_hand == 0) h_x[active_lut][index] = MaxVal(h_x[active_lut][index],0.0);
		else if ((h_x[active_lut][index] < h_x[active_lut][index-1]) && (active_hand != (num_hands[active_lut][active_cmp]-1)))
			{
			_deleteHandle(active_lut,active_cmp,active_hand); 
			freshen_cmp[active_lut][active_cmp] = 1;
			_getPoints();
			_drawCurveWin();
			return TRUE;
			}
		else h_x[active_lut][index] = 
			MaxVal(h_x[active_lut][index],(h_x[active_lut][index-1]+0.01));

		if (active_hand == (num_hands[active_lut][active_cmp]-1)) h_x[active_lut][index] = MinVal(h_x[active_lut][index],1.0);
		else if ((h_x[active_lut][index] > h_x[active_lut][index+1]) && (active_hand != 0))
			{
			_deleteHandle(active_lut,active_cmp,active_hand); 
			freshen_cmp[active_lut][active_cmp] = 1;
			_getPoints();
			_drawCurveWin();
			return TRUE;
			}
		else h_x[active_lut][index] = 
			MinVal(h_x[active_lut][index],(h_x[active_lut][index+1]-0.01));

		
		// set flag indicating that curve data needs to be updated
		freshen_cmp[active_lut][active_cmp] = 1;
		_getPoints();
		_drawCurveWin();
		if (parentWin) parentWin->updateDataForControl(controlNum);
		}
	return TRUE;
	}
return FALSE;
}


// update the curves 
// Turns on the correct active curve
// and turns off the inactive curves
McoStatus CurveControl::_updateCurveData(void)
{
int i,j;

short 			itemType;
Handle 			cn1;
Rect 			r;
int				item_val;



// Make sure that the menu is present (not present grayscale)
if (ids[C_ColorMenu] > -1) 
	{
	GetDItem (dp, ids[C_ColorMenu] , &itemType,&cn1,&r);
	item_val = GetCtlValue((ControlHandle)cn1);
	}
else item_val = 1;
lastmenu = item_val;
// turn on all curves
if (item_val==ids[C_AllNum])
	{
	if ((active_lut != 0) && (active_lut != 1)) active_hand = -1;
	active_luts[0] = 1;
	active_luts[1] = 1;
	for (i=0; i<num_luts; i++)
		for (j=0; j<num_cmps[i]; j++)
			{
			active_cmps[i][j]=1;
			freshen_cmp[i][j]=1;
			}	
	}
// Turn on a one of the other curves
else
	{
	if (active_lut != 0)  active_hand = -1;
	active_lut = 0;
	active_luts[0] = 1;
	if (ids[C_FirstNum] != -1) item_val -= ids[C_FirstNum];
	else 	item_val--;
	for (i=0; i<num_luts; i++)
		for (j=0; j<num_cmps[i]; j++)
			active_cmps[i][j]=0;
	active_cmps[0][item_val]=1;
	freshen_cmp[0][item_val]=1;
	}
return MCO_SUCCESS;
}

// update the data in the screen
McoStatus 	CurveControl::UpdateControl(void)
{
	short 		itemType;
	Rect		r;
	Handle 		h;

_drawCurveWin();
return MCO_SUCCESS;
}

void CurveControl::getActiveHand(int *hand)
{
hand[0] = active_cmp;
hand[1] = active_hand;
}

// do the events for the control
McoStatus	CurveControl::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed,int *didIt)
{
	short 			itemType;
	Handle 			cn1;
	Rect 			r;
	Boolean 		Changed;
	Point			where;
	McoStatus		state = MCO_SUCCESS;
	Boolean			changed2;
	Str255			s;
	long				temp,temp2;


Changed = FALSE;
*didIt = FALSE;

if ((item == ids[C_CurveWin]) && (item != -1) && (handFlag))
	{
	*didIt = 1;
	Changed = TRUE;
	//GetMouse ( &where );
	where = clickPoint;
	GlobalToLocal(&where);
	// Find the handle
	_getHandNum(where.h,where.v);
	if (active_hand>=0)
		{
		// Drag it
		_dragHand();
		}
	else {
		if (_getCurveNum(where.h,where.v)) 
			{
			_dragHand();	
			}
		else _drawCurveWin();				
		}
	}
if ((item == ids[C_ColorMenu]) && (item != -1))
	{
	*didIt = 1;

	_updateCurveData();
	_getPoints();
	_drawCurveWin(); 	
	}
else if ((item==ids[C_Reset]) && (active_hand > -1) && (item != -1))
	{
	*didIt = 1;

	_resetCurve(active_lut,active_cmp);
	_getPoints();
	_drawCurveWin();
	}
else if (active_hand>-1)
	{
	changed2 = FALSE;

	if (ids[C_Text2] > -1)
		{
		//Look at text and update curve if neccesary
		
		temp2 = h_y[active_lut][active_cmp*MaxNumHands+active_hand]*(vmax[active_lut]-vmin[active_lut])+vmin[active_lut];

		changed2 = changed2 | GetItemTextFromNum(ids[C_Text2],&temp2,vmin[active_lut],vmax[active_lut]);
				
		h_y[active_lut][active_cmp*MaxNumHands+active_hand] = 
				(temp2-vmin[active_lut])/(vmax[active_lut]-vmin[active_lut]);
				
		}

	if (ids[C_Text1] > -1)
		{

		temp2 = h_x[active_lut][active_cmp*MaxNumHands+active_hand]*(hmax[active_lut]-hmin[active_lut])+hmin[active_lut];

		changed2 = changed2 | GetItemTextFromNum(ids[C_Text1],&temp2,hmin[active_lut],hmax[active_lut]);

		h_x[active_lut][active_cmp*MaxNumHands+active_hand] = 
			(temp2-hmin[active_lut])/(hmax[active_lut]-hmin[active_lut]);

		}
		
	// update window and data if a change occured	
	if (changed2)
		{	
		*didIt = 1;

		freshen_cmp[active_lut][active_cmp] = 1;
	
		_getPoints();
		_drawCurveWin();
		Changed = TRUE;
		}
	}
changed = changed | Changed;
return MCO_SUCCESS;
}






