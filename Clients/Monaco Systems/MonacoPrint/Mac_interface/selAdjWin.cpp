/*********************************************/
/* selAdjWin.c							 	 */
/* Selectivly adjust colors					 */
/* Jun 3 1996							     */
/*********************************************/
/*   James Vogh 							 */
/*********************************************/

#include <math.h>
#include "selAdjWin.h"
#include "colortran.h"
#include "xyztorgb.h"
#include "profiledoc.h"
#include "tweak_patch.h"
#include "Monaco Slider.h"
#include "pascal_string.h"
#include "think_bugs.h"
#include "fullname.h"
#include "errordial.h"
#include "gammut_comp.h"

#define OK 1
#define Cancel 2

CAdjWin::CAdjWin(ProfileDocInf *dc,XyztoRgb *xtr,double g)
{
short	tids[] = SADJ_IDS;
int i;
int32	temp;
RefConType* theRefCon;
Rect r;
Handle h;
short itemType;
GammutComp *gammutcomp;
double l_linear,gammutExpand;
double	stepL,stepA;
McoStatus state;

int16		PR_P[] = SADJ_PICT_IDS;
int16		PR_OFF[] = SADJ_OFFSET;
ControlHandle	sliderControl;
Str255		filename1 = MONACO_TECHON_GENERIC;
Str255		filename2 = MONACO_TECHON_DRYJET;

priority = 1;
doc = dc;


stepL = 100.0/32.0;
stepA = 255.0/32.0;

for (i=0; i<33; i++) 
	{
	xa1[i] = i * stepL;
	xa2[i] = i*stepA - 128.0;
	}



WinType = SelAdjWindow;
WinNum = 0;

for (i=0; i<NUM_SADJ_IDS; i++) ids[i] = tids[i];

setDialog(ids[SADJ_ID]);
threeD_box(ids[SADJ_PICK_BOX]);
frame_button(ids[SADJ_OK_BOX]);	




tweak = dc->tweak;
xyztorgb = xtr;
gamma = g;

desiredLAB[0] = 0;
desiredLAB[1] = 0;
desiredLAB[2] = 0;

ConvertToRGB(desiredLAB,desiredRGB);
ConvertToGammutC(desiredLAB,gammutLAB);

rgb_d[0] = 0;
rgb_d[1] = 0;
rgb_d[2] = 0;

rgb_p[0] = 0;
rgb_p[1] = 0;
rgb_p[2] = 0;

printedLAB[0] = 0;
printedLAB[1] = 0;
printedLAB[2] = 0;

SliderL = 0;
Cr = 15;
Lr = 15;


GetDItem ( dp, ids[SADJ_LIGHT_RANGE], &itemType, (Handle*)&sliderControl, &r );
SetCtlValue ( sliderControl,(int32)Lr);

GetDItem ( dp, ids[SADJ_COLOR_RANGE], &itemType, (Handle*)&sliderControl, &r );
SetCtlValue ( sliderControl,(int32)Cr);



ConvertToLAB(printedLAB,printedRGB);
CopyTextToDial(0);
updateWindowData(0);


// initialize comminications with the techkon
state = doc->openInputDevice(0,1,0);
if (state != MCO_SUCCESS) McoErrorAlert(state);
// calibrate the device
if (doc->tcom) doc->tcom->Calibrate();

Current_Text = 0;
_last_tick_time2 = 0;


}

CAdjWin::~CAdjWin()
{
if (doc != 0L) doc->closeInputDevice();
}

void CAdjWin::SetViewLRange(void)
{
double L;
double Lmin,Lmax;
Rect r;
Handle h;
short itemType;
ControlHandle	sliderControl;

L = tweak->findPerc(Lr,VIEW_RANGE);
if (L > 100) L = 100;

Lmin = gammutLAB[0]-L;
if (Lmin < 0) Lmin = 0;

Lmax = gammutLAB[0]+L;
if (Lmax > 100) Lmax = 100;

if (SliderL < Lmin) SliderL = Lmin;
if (SliderL > Lmax) SliderL = Lmax;

GetDItem ( dp, ids[SADJ_VIEW_SLIDER], &itemType, (Handle*)&sliderControl, &r );
SetCtlValue ( sliderControl,(int32)SliderL);

SetCtlMin ( sliderControl,(int32)Lmin);
SetCtlMax ( sliderControl,(int32)Lmax);
}


#define bigger(a, b) ((a >= b) ? a :b)
#define smaller(a, b) ((a < b) ? a :b)

void CAdjWin::ConvertToRGB(double *lab,double *rgb)
{
McoStatus state;
double	big_rgb;

rgb[0] = lab[0];
rgb[1] = lab[1];
rgb[2] = lab[2];

labtonxyzinplace(rgb,1);
state = xyztorgb->compute(rgb, gamma, 1);

big_rgb = bigger(rgb[0],bigger(rgb[1],rgb[2]));
if (big_rgb>255)
	{
	rgb[0] = rgb[0]*255/big_rgb;
	rgb[1] = rgb[1]*255/big_rgb;
	rgb[2] = rgb[2]*255/big_rgb;
	}
if (rgb[0] <0) rgb[0] = 0;
if (rgb[1] <0) rgb[1] = 0;
if (rgb[2] <0) rgb[2] = 0;
}

void CAdjWin::ConvertToLAB(double *rgb,double *lab)
{
McoStatus state;

lab[0] = rgb[0];
lab[1] = rgb[1];
lab[2] = rgb[2];

state = xyztorgb->computeInv(lab, gamma, 1);
nxyztoLabinplace(lab,1);


}

McoStatus CAdjWin::interpolate_33(double *lab,double *labc,double *table)
{
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai1,ai2,bi1,bi2,ci1,ci2;
double t,u,v;
double	r1,r2,r3,r4,r5,r6,r7,r8;
double	*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;


a = lab[0]*0.32;
b = (lab[1]+128)*(32.0/255.0);
c = (lab[2]+128)*(32.0/255.0);



if (a >= 32) a = 31.999999;
if (a < 0) a = 0;
if (b >= 32) b = 31.999999;
if (b < 0) b = 0;
if (c >= 32) c = 31.999999;
if (c < 0) c = 0;
			
a1 = ((int32)a)*3267;
a2 = a1 + 3267;
b1 = ((int32)b)*99;
b2 = b1 + 99;
c1 = ((int32)c)*3;
c2 = c1 + 3;

t = a - (int32)a;
u = b - (int32)b;
v = c - (int32)c;

r1 = (1-u)*(1-v);
r2 = (1-u)*v;
r3 = u*(1-v);
r4 = u*v;
r5 = t*r1;
r6 = t*r2;
r7 = t*r3;
r8 = t*r4;
a = (1-t);
r1 = a*r1;
r2 = a*r2;
r3 = a*r3;
r4 = a*r4;

p1 = table +a1+b1+c1;
p2 = table +a1+b1+c2;
p3 = table +a1+b2+c1;
p4 = table +a1+b2+c2;
p5 = table +a2+b1+c1;
p6 = table +a2+b1+c2;
p7 = table +a2+b2+c1;
p8 = table +a2+b2+c2;
			
labc[0] = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));
	
labc[1] = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));	
	
labc[2] = r1*(*(p1)) +
	r2*(*(p2)) +
	r3*(*(p3)) +
	r4*(*(p4)) +
	r5*(*(p5)) +
	r6*(*(p6)) +
	r7*(*(p7)) +
	r8*(*(p8));
return MCO_SUCCESS;							 
}


void CAdjWin::ConvertToGammutC(double *lab,double *labc)
{
double	*labTable;

labTable = (double*)McoLockHandle(doc->_labtableH);

interpolate_33(lab,labc,labTable);

McoUnlockHandle(doc->_labtableH);
}

//Do an update if a key was pressed 0.5 seconds or more ago
Boolean CAdjWin::checkChanged(void)
{
if (KeyHasBeenHit)
	{
	if (LastKeyPress < TickCount() - 50) 
		{
		KeyHasBeenHit = FALSE;
		return TRUE;
		}
	}
return FALSE;
}

McoStatus CAdjWin::CopyTextFromDial(void)
{
	McoStatus state;
	Rect r;
	Handle h;
	short itemType;
	Str255		s;
	int32		color;
	int32		icolor;
	int32		fcolor;
	
	Boolean		Changed = FALSE;
	Boolean		changed = FALSE;
	
	

	GetDItem (dp,ids[SADJ_Des_R], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 255) color = 255;
	if (color < 0) color = 0;
	if (desiredRGBi[0] != color) changed = TRUE;
	desiredRGB[0] = color;
	desiredRGBi[0] = color;
	
	GetDItem (dp,ids[SADJ_Des_G], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 255) color = 255;
	if (color < 0) color = 0;
	if (desiredRGBi[1] != color) changed = TRUE;
	desiredRGB[1] = color;
	desiredRGBi[1] = color;
	
	GetDItem (dp,ids[SADJ_Des_B], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 255) color = 255;
	if (color < 0) color = 0;
	if (desiredRGBi[2] != color) changed = TRUE;
	desiredRGB[2] = color;
	desiredRGBi[2] = color;	
		
	if (changed) 
		{
		ConvertToLAB(desiredRGB,desiredLAB);
		ConvertToGammutC(desiredLAB,gammutLAB);
		ConvertToRGB(gammutLAB,gammutRGB);
		CopyTextToDial(1);
		KeyHasBeenHit = TRUE;
		LastKeyPress = TickCount();
		return MCO_SUCCESS;
		}

		
	GetDItem (dp,ids[SADJ_Des_L], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 100) color = 100;
	if (color < 0) color = 0;
	if (desiredLABi[0] != color) changed = TRUE;
	desiredLAB[0] = color;
	desiredLABi[0] = color;
	
	GetDItem (dp,ids[SADJ_Des_a], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 128) color = 128;
	if (color < -128) color = -128;
	if (desiredLABi[1] != color) changed = TRUE;
	desiredLAB[1] = color;
	desiredLABi[1] = color;
	
	GetDItem (dp,ids[SADJ_Des_b], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 128) color = 128;
	if (color < -128) color = -128;
	if (desiredLABi[2] != color) changed = TRUE;
	desiredLAB[2] = color;
	desiredLABi[2] = color;
	
	if (changed) 
		{
		ConvertToRGB(desiredLAB,desiredRGB);
		ConvertToGammutC(desiredLAB,gammutLAB);
		ConvertToRGB(gammutLAB,gammutRGB);
		CopyTextToDial(2);
		KeyHasBeenHit = TRUE;
		LastKeyPress = TickCount();
		return MCO_SUCCESS;
		}

	GetDItem (dp,ids[SADJ_Pri_R], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 255) color = 255;
	if (color < 0) color = 0;
	if (printedRGBi[0] != color) changed = TRUE;
	printedRGB[0] = color;
	printedRGBi[0] = color;
	
	GetDItem (dp,ids[SADJ_Pri_G], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 255) color = 255;
	if (color < 0) color = 0;
	if (printedRGBi[1] != color) changed = TRUE;
	printedRGB[1] = color;
	printedRGBi[1] = color;
	
	GetDItem (dp,ids[SADJ_Pri_B], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 255) color = 255;
	if (color < 0) color = 0;
	if (printedRGBi[2] != color) changed = TRUE;
	printedRGB[2] = color;
	printedRGBi[2] = color;

	if (changed) 
		{
		ConvertToLAB(printedRGB,printedLAB);
		CopyTextToDial(3);
		KeyHasBeenHit = TRUE;
		LastKeyPress = TickCount();
		return MCO_SUCCESS;
		}
		
	GetDItem (dp,ids[SADJ_Pri_L], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 100) color = 100;
	if (color < 0) color = 0;
	if (printedLABi[0] != color) changed = TRUE;
	printedLAB[0] = color;
	printedLABi[0] = color;
	
	GetDItem (dp,ids[SADJ_Pri_a], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 128) color = 128;
	if (color < -128) color = -128;
	if (printedLABi[1] != color) changed = TRUE;
	printedLAB[1] = color;
	printedLABi[1] = color;
	
	GetDItem (dp,ids[SADJ_Pri_b], &itemType, &h, &r);
	GetIText(h,s);
	StringToNum (s, &color);
	if (color > 128) color = 128;
	if (color < -128) color = -128;
	if (printedLABi[2] != color) changed = TRUE;
	printedLAB[2] = color;
	printedLABi[2] = color;

	if (changed) 
		{
		ConvertToRGB(printedLAB,printedRGB);
		CopyTextToDial(4);
		KeyHasBeenHit = TRUE;
		LastKeyPress = TickCount();
		return MCO_SUCCESS;
		}
	
//Changed = changed;
return MCO_SUCCESS;
	
}

int round_float(double x);
int round_float(double x)
{
if (x > 0) x += 0.5;
else x -= 0.5;
return (int)x;
}

McoStatus CAdjWin::CopyViewTextToDial(double *labd,double *labp)
{

	McoStatus	state;
	short 		itemtype;
	short		item;
	Handle		h;
	Rect		r;
	Str255		s;
	int32		temp;
	int32		temp_i;
	
	int			i;
	Boolean		changed = FALSE;
	
	
	GetDItem (dp,ids[SADJ_VIEW_D_L], &itemtype, &h, &r);
	temp_i = round_float(labd[0]);
	desiredRGBi[0] = temp_i;
	NumToString (temp_i, s);
	SetIText (h, s);
	
	GetDItem (dp,ids[SADJ_VIEW_D_a], &itemtype, &h, &r);
	temp_i = round_float(labd[1]);
	desiredRGBi[0] = temp_i;
	NumToString (temp_i, s);
	SetIText (h, s);
	
	GetDItem (dp,ids[SADJ_VIEW_D_b], &itemtype, &h, &r);
	temp_i = round_float(labd[2]);
	desiredRGBi[0] = temp_i;
	NumToString (temp_i, s);
	SetIText (h, s);
	
	GetDItem (dp,ids[SADJ_VIEW_P_L], &itemtype, &h, &r);
	temp_i = round_float(labp[0]);
	desiredRGBi[0] = temp_i;
	NumToString (temp_i, s);
	SetIText (h, s);
	
	GetDItem (dp,ids[SADJ_VIEW_P_a], &itemtype, &h, &r);
	temp_i = round_float(labp[1]);
	desiredRGBi[0] = temp_i;
	NumToString (temp_i, s);
	SetIText (h, s);
	
	GetDItem (dp,ids[SADJ_VIEW_P_b], &itemtype, &h, &r);
	temp_i = round_float(labp[2]);
	desiredRGBi[0] = temp_i;
	NumToString (temp_i, s);
	SetIText (h, s);
	
	
return MCO_SUCCESS;	
}



McoStatus CAdjWin::CopyTextToDial(short w)
{

	McoStatus	state;
	short 		itemtype;
	short		item;
	Handle		h;
	Rect		r;
	Str255		s;
	int32		temp;
	int32		temp_i;
	
	int			i;
	Boolean		changed = FALSE;
	
	if (w != 1)
		{
		GetDItem (dp,ids[SADJ_Des_R], &itemtype, &h, &r);
		temp_i = round_float(desiredRGB[0]);
		desiredRGBi[0] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		
		GetDItem (dp,ids[SADJ_Des_G], &itemtype, &h, &r);
		temp_i = round_float(desiredRGB[1]);
		desiredRGBi[1] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		
		GetDItem (dp,ids[SADJ_Des_B], &itemtype, &h, &r);
		temp_i = round_float(desiredRGB[2]);
		desiredRGBi[2] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		}
	
	if (w != 2)
		{
		GetDItem (dp,ids[SADJ_Des_L], &itemtype, &h, &r);
		temp_i = round_float(desiredLAB[0]);
		desiredLABi[0] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		
		GetDItem (dp,ids[SADJ_Des_a], &itemtype, &h, &r);
		temp_i = round_float(desiredLAB[1]);
		desiredLABi[1] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		
		GetDItem (dp,ids[SADJ_Des_b], &itemtype, &h, &r);
		temp_i = round_float(desiredLAB[2]);
		desiredLABi[2] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		}

	GetDItem (dp,ids[SADJ_Gam_L], &itemtype, &h, &r);
	temp_i = round_float(gammutLAB[0]);
	gammutLABi[0] = temp_i;
	NumToString (temp_i, s);
	SetIText (h, s);
	
	GetDItem (dp,ids[SADJ_Gam_a], &itemtype, &h, &r);
	temp_i = round_float(gammutLAB[1]);
	gammutLABi[1] = temp_i;
	NumToString (temp_i, s);
	SetIText (h, s);
	
	GetDItem (dp,ids[SADJ_Gam_b], &itemtype, &h, &r);
	temp_i = round_float(gammutLAB[2]);
	gammutLABi[2] = temp_i;
	NumToString (temp_i, s);
	SetIText (h, s);
	
	
	
	if (w != 3)
		{
		GetDItem (dp,ids[SADJ_Pri_R], &itemtype, &h, &r);
		temp_i = round_float(printedRGB[0]);
		printedRGBi[0] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		
		GetDItem (dp,ids[SADJ_Pri_G], &itemtype, &h, &r);
		temp_i = round_float(printedRGB[1]);
		printedRGBi[1] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		
		GetDItem (dp,ids[SADJ_Pri_B], &itemtype, &h, &r);
		temp_i = round_float(printedRGB[2]);
		printedRGBi[2] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		}
		
	if (w != 4)
		{	
		GetDItem (dp,ids[SADJ_Pri_L], &itemtype, &h, &r);
		temp_i = round_float(printedLAB[0]);
		printedLABi[0] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		
		GetDItem (dp,ids[SADJ_Pri_a], &itemtype, &h, &r);
		temp_i = round_float(printedLAB[1]);
		printedLABi[1] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		
		GetDItem (dp,ids[SADJ_Pri_b], &itemtype, &h, &r);
		temp_i = round_float(printedLAB[2]);
		printedLABi[2] = temp_i;
		NumToString (temp_i, s);
		SetIText (h, s);
		}
return MCO_SUCCESS;
}



// Assumes a 128*128 floating point rgb map
McoStatus CAdjWin::CopyMultColorBox(GWorldPtr src,double *rgb)
{

int32			i;
int32			temp;
PixMapHandle	srcPixMap;
short			srcRowBytes;
long			*srcBaseAddr;
long			*srcAddr1;
double			*dataBaseAddr;
double			*dataAddr;
int32			newdataRowBytes;
int32			data_inc;
signed char			mmuMode;
short			row, column;
short			width;
short			height;
GDHandle	oldGD;
GWorldPtr	oldGW;
McoStatus 	status;
unsigned char	color_lut[256];


//return MCO_SUCCESS;
	srcPixMap = GetGWorldPixMap ( src );

	if( LockPixels ( srcPixMap ) ) 
	{
		GetGWorld(&oldGW,&oldGD);

		srcBaseAddr = (long *) GetPixBaseAddr ( srcPixMap );	/* get the address of the pixmap */
		srcRowBytes = (**srcPixMap).rowBytes & 0x7fff;			/* get the row increment */
		dataBaseAddr = rgb;

		width = (**srcPixMap).bounds.right - (**srcPixMap).bounds.left;
		height = (**srcPixMap).bounds.bottom - (**srcPixMap).bounds.top;
	
		dataAddr = dataBaseAddr;
		mmuMode = true32b;
 		SwapMMUMode ( &mmuMode );						/* set the MMU to 32-bit mode */
 		for ( row = 0; row < height; row++ ) 
		{
	
			srcAddr1 = srcBaseAddr;
			for ( column = 0; column < width; column++ ) 
			{
				*srcAddr1 = (int32)(*dataAddr) << 16;;	
				dataAddr++;
				*srcAddr1 += (int32)(*dataAddr) << 8;
				dataAddr++;
				*srcAddr1 += (int32)(*dataAddr);
				dataAddr ++; 
				srcAddr1++;					/* bump to next pixel */
			}
	
			srcBaseAddr = (long *) ( (char *) srcBaseAddr +  srcRowBytes );	/* go to the next row */
		}
 		SwapMMUMode ( &mmuMode );						/* restore the previous MMU mode */
		UnlockPixels ( srcPixMap );						/* unlock the pixmap */
	}
return MCO_SUCCESS;
}


McoStatus CAdjWin::DrawMultColorBox(short item,double *rgb)
{
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	GWorldPtr	BigGW;
	Point		offset;


	PixMapHandle myPixMapHandle;

	Rect		bounds;
	Rect		winRect2;

	QDErr		error;
	GrafPtr			oldGraf;
	RGBColor	c,oldbackcolor;
	Rect 		drawRect;

	Rect r;
	Handle h;
	short itemType;


	GetDItem (dp,item, &itemType, &h, &r);

	GetPort(&oldGraf);
	SetPort(dp);

	GetBackColor(&oldbackcolor);
	c.red = 65535;
	c.green = 65535;
	c.blue = 65535;
	RGBBackColor(&c);
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	RGBForeColor(&c);

	SetRect(&drawRect,0,0,128,128);
	
	error = NewGWorld( &BigGW, 32, &drawRect, 0, 0, 0 );
	if (error == 0)
	{
		GetGWorld(&oldGW,&oldGD);
		SetGWorld(BigGW,nil);

		EraseRect( &drawRect );		/* clear the GWorld */
		
		myPixMapHandle = GetGWorldPixMap( BigGW );	/* 7.0 only */
		

		CopyMultColorBox(BigGW,rgb);
		// return to old gworld
		SetGWorld(oldGW,oldGD);	
		
		CopyBits( (BitMap*)*myPixMapHandle, (BitMap*)&dp->portBits, &drawRect, &r, srcCopy + ditherCopy, 0 );
		DisposeGWorld( BigGW );
	}
	RGBBackColor(&oldbackcolor);
return MCO_SUCCESS;
}

McoStatus CAdjWin::CreateMultColorBox(double *lab,double Cr,double *rgb)
{
int     i,j,n = 0;
double	aR,aL,bT,bB;
double	a,b;
double  a_inc,b_inc;
double 	*rgb_p;
double	L = lab[0];
McoStatus	state;
double	big_rgb;
double  range;


range = tweak->findPerc(Cr,VIEW_RANGE);

aL = lab[1] - range;
aR = lab[1] + range;
bT = lab[2] - range;
bB = lab[2] + range;

if (aL < -128) aL = -128;
if (aR > 128) aR = 128;

if (bT < -128) bT = -128;
if (bB > 128) bB = 128;

rgb_p = rgb;

b_inc = (lab[2] - bT)/64;
b = bT;
for (i = 0; i<64; i++)
	{
	a_inc = (lab[1] - aL)/64;
	a = aL;
	for (j=0; j<64; j++)
		{
		(*rgb_p++) = L;
		(*rgb_p++) = a;
		(*rgb_p++) = b;
		n++;
		a += a_inc;
		}
	a_inc = (aR - lab[1])/64;
	a = lab[1];
	for (j=0; j<64; j++)
		{
		(*rgb_p++) = L;
		(*rgb_p++) = a;
		(*rgb_p++) = b;
		n++;
		a += a_inc;
		}
	b += b_inc;
	}
b_inc = (bB - lab[2])/64;
b = lab[2];
for (i = 0; i<64; i++)
	{
	a_inc = (lab[1] - aL)/64;
	a = aL;
	for (j=0; j<64; j++)
		{
		(*rgb_p++) = L;
		(*rgb_p++) = a;
		(*rgb_p++) = b;
		n++;
		a += a_inc;
		}
	a_inc = (aR - lab[1])/64;
	a = lab[1];
	for (j=0; j<64; j++)
		{
		(*rgb_p++) = L;
		(*rgb_p++) = a;
		(*rgb_p++) = b;
		n++;
		a += a_inc;
		}
	b += b_inc;
	}
labtonxyzinplace(rgb,128*128);
state = xyztorgb->compute(rgb, gamma, 128*128);
rgb_p = rgb;
for (i=0; i<128*128; i++)
	{
	big_rgb = bigger(rgb_p[0],bigger(rgb_p[1],rgb_p[2]));
	if (big_rgb>255)
		{
		rgb_p[0] = rgb_p[0]*255/big_rgb;
		rgb_p[1] = rgb_p[1]*255/big_rgb;
		rgb_p[2] = rgb_p[2]*255/big_rgb;
		}
	if (rgb_p[0] <0) rgb_p[0] = 0;
	if (rgb_p[1] <0) rgb_p[1] = 0;
	if (rgb_p[2] <0) rgb_p[2] = 0;
	rgb_p +=3;
	}

return MCO_SUCCESS;
}


// return the lab value within a box given the center lab value, the mouse position, and
// the color range

McoStatus CAdjWin::getBoxPointColor(double *labOut,double *labCent, double Cr, short item, Point where)
{
Rect r;
Handle h;
short itemType;
double	x,y;
double	aR,aL,bT,bB;
double  range;


range = tweak->findPerc(Cr,VIEW_RANGE);

aL = labCent[1] - range;
aR = labCent[1] + range;
bT = labCent[2] - range;
bB = labCent[2] + range;


if (aL < -128) aL = -128;
if (aR > 128) aR = 128;

if (bT < -128) bT = -128;
if (bB > 128) bB = 128;


GetDItem (dp,item, &itemType, &h, &r);

// get the x,y coordinates
x = ((double)where.h - (double)r.left)/((double)r.right - (double)r.left);
y = ((double)where.v - (double)r.top)/((double)r.bottom - (double)r.top);


labOut[0] = labCent[0];
if (x < 0.5)
	{
	if (y < 0.5)
		{
		labOut[1] = x*2*labCent[1]+(1-x*2)*aL;
		labOut[2] = y*2*labCent[2]+(1-y*2)*bT;
		}
	else
		{
		y = y - 0.5;
		labOut[1] = x*2*labCent[1]+(1-x*2)*aL;
		labOut[2] = y*2*bB+(1-y*2)*labCent[2];
		}
	}
else
	{
	x = x - 0.5;
	if (y < 0.5)
		{
		labOut[1] = x*2*aR+(1-x*2)*labCent[1];
		labOut[2] = y*2*labCent[2]+(1-y*2)*bT;
		}
	else
		{
		y = y - 0.5;
		labOut[1] = x*2*aR+(1-x*2)*labCent[1];
		labOut[2] = y*2*bB+(1-y*2)*labCent[2];
		}
	}

return MCO_SUCCESS;
}

// recreate the data within the window
McoStatus CAdjWin::updateWindowData(int changed)
{
double	lab[3];

SetViewLRange();

CreateMultColorBox(gammutLAB,10,rgb1);

lab[0] = SliderL;
lab[1] = gammutLAB[1];
lab[2] = gammutLAB[2];

CreateMultColorBox(lab,Cr,rgb2);

DrawWindow();
return MCO_SUCCESS;
}

McoStatus CAdjWin::DrawWinowParts(void)
{
McoStatus state;

if (Current_Text == 0) 
	{
	unbox_item(dp,ids[SADJ_Pri_Box]);
	box_item(dp,ids[SADJ_Des_Box]);
	}
else
	{
	unbox_item(dp,ids[SADJ_Des_Box]);
	box_item(dp,ids[SADJ_Pri_Box]);	
	}
// draw the multiColorBoxes
state = DrawMultColorBox(ids[SADJ_PICK_BOX],rgb1);
if (state != MCO_SUCCESS) return state;

state = DrawMultColorBox(ids[SADJ_VIEW_BOX],rgb2);
if (state != MCO_SUCCESS) return state;

// draw the desired and printed boxes
state = DrawColorBox(ids[SADJ_Des_Box],desiredRGB);
if (state != MCO_SUCCESS) return state;

state = DrawColorBox(ids[SADJ_Pri_Box],printedRGB);
if (state != MCO_SUCCESS) return state;


state = DrawColorBox(ids[SADJ_Gam_Box],gammutRGB);
if (state != MCO_SUCCESS) return state;

state = DrawColorBox(ids[SADJ_VIEW_DES],rgb_d);
if (state != MCO_SUCCESS) return state;

state = DrawColorBox(ids[SADJ_VIEW_PRI],rgb_p);
if (state != MCO_SUCCESS) return state;

return state;
}

McoStatus CAdjWin::DrawWindow(void)	
{			
	Rect updateRect;
	RGBColor	c,oldbackcolor;

	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	return DrawWinowParts();
}	

McoStatus CAdjWin::UpdateWindow(void)	
{			
	Rect updateRect;
	McoStatus state;
	
	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	BeginUpdate( dp );
	state = DrawWinowParts();
	DrawDialog( dp );
	EndUpdate( dp );
	
	return state;
}


// Get the active region
McoStatus CAdjWin::makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn)
{
Rect r;
Handle h;
short itemType;

GetDItem (dp,ids[SADJ_PICK_BOX], &itemType, &h, &r);

RectRgn(previewRgn,&r);
XorRgn(mouseRgn,previewRgn,mouseRgn);

GetDItem (dp,ids[SADJ_VIEW_BOX], &itemType, &h, &r);

RectRgn(previewRgn,&r);
XorRgn(mouseRgn,previewRgn,mouseRgn);

return MCO_SUCCESS;
}

McoStatus CAdjWin::doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype)
{
McoStatus state = MCO_SUCCESS;
unsigned char 	*previewOutBuf;
unsigned char 	*previewInBuf;
GrafPtr			oldGraf;
Rect r1,r2;
Handle h;
short itemType;
double  labc[3];
double	lab[3];
double  rgb[3];

WindowPtr	oldP;
	
GetPort(&oldP);
SetPort( dp ) ;

GlobalToLocal(&where);

GetDItem (dp,ids[SADJ_PICK_BOX], &itemType, &h, &r1);
GetDItem (dp,ids[SADJ_VIEW_BOX], &itemType, &h, &r2);

if (PtInRect(where,&r1))
	{
	cursortype = WC_CrossTemp;
	state = getBoxPointColor(lab,gammutLAB,20,ids[SADJ_PICK_BOX] ,where);
	if (state != MCO_SUCCESS) return state;
	ConvertToRGB(lab,rgb);
	state = DrawColorBox(ids[SADJ_Pri_Box],rgb);
	if (state != MCO_SUCCESS) return state;
	}
else if (PtInRect(where,&r2))
	{
	cursortype = WC_CrossTemp;
	labc[0] = SliderL;
	labc[1] = gammutLAB[1];
	labc[2] = gammutLAB[2];
	state = getBoxPointColor(lab,labc,Cr,ids[SADJ_VIEW_BOX] ,where);
	ConvertToRGB(lab,rgb_d);
	state = DrawColorBox(ids[SADJ_VIEW_DES],rgb_d);
	if (state != MCO_SUCCESS) return state;
	tweak->Modify_Color(lab,labc,desiredLAB,printedLAB,Lr,Cr);
	ConvertToRGB(labc,rgb_p);
	state = DrawColorBox(ids[SADJ_VIEW_PRI],rgb_p);
	CopyViewTextToDial(lab,labc);
	if (state != MCO_SUCCESS) return state;	
	}
else 
	{
	cursortype = WC_SetBack;
	state = DrawColorBox(ids[SADJ_Pri_Box],printedRGB);
	if (state != MCO_SUCCESS) return state;	
	}
SetPort(oldP);
return state;
} 

Boolean CAdjWin::isMyObject(Point where,short *item) 
{
WindowPtr oldP;
short  titem;
ControlHandle	sliderControl;

GetPort(&oldP);
SetPort(dp);
GlobalToLocal(&where);
titem = FindDItem(dp,where);
titem ++;
if ((titem == ids[SADJ_VIEW_SLIDER]) || (titem == ids[SADJ_LIGHT_RANGE]) || (titem == ids[SADJ_COLOR_RANGE]))
	{
	if (FindControl(where,dp,(ControlHandle*)&sliderControl) != inThumb)
		{
		*item = titem;
		SetPort(oldP);
		return TRUE;
		}
	}
SetPort(oldP);
return FALSE;
}

McoStatus CAdjWin::SaveTweakedPatches(void)
{
McoStatus state = MCO_SUCCESS;
RawData	  *tempPatchesD;

tempPatchesD = new RawData;
if (tempPatchesD == NULL) return MCO_MEM_ALLOC_ERROR;
state = tempPatchesD->copyRawData(doc->patchD);
if (state != MCO_SUCCESS) return state;


state = tweak->eval(doc->patchD,tempPatchesD,doc->patchD->format.total);
if (state != MCO_SUCCESS) goto bail;
state = doc->handle_save_datafile(tempPatchesD);

bail:
delete tempPatchesD;
return state;
}

Boolean CAdjWin::TimerEventPresent(void)
{
if( TickCount() - _last_tick_time2 > 4 ) 
	{
	return TRUE;
	}
return FALSE;
}





McoStatus CAdjWin::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc,void **data, Boolean &changed)
{
StandardFileReply		theReply;
SFTypeList typeList;
FileFormat *file;
McoStatus state;
Str255				inname;
FILE 				*fs;
Rect r;
Handle h;
short itemType;
ControlHandle	sliderControl;
Point		mpoint;

SetPort(dp);
GlobalToLocal(&clickPoint);

typeList[0]='MPO2';

Changed = FALSE;
*numwc = 0;
state = MCO_SUCCESS;

if (item == OK)
	{
	tweak->Modify_Table(desiredLAB,printedLAB,Lr,Cr);
	*numwc = 1;
	wc[0].code = WE_Close_Window;
	wc[0].wintype = WinType;
	wc[0].winnum = WinNum;
	wc[0].doc = doc;
	}
else if (item == Cancel)
	{
	*numwc = 1;
	wc[0].code = WE_Close_Window;
	wc[0].wintype = WinType;
	wc[0].winnum = WinNum;
	wc[0].doc = doc;
	}
else if (item == ids[SADJ_APPLY])
	{
	tweak->Modify_Table(gammutLAB,printedLAB,Lr,Cr);
	}
else if (item == ids[SADJ_LOAD])
	{
	StandardGetFile(nil,1,typeList,&theReply);
		if (theReply.sfGood == TRUE)
		{
		file = new FileFormat;
		if (file == NULL) return MCO_MEM_ALLOC_ERROR;
		state = file->openFile(&theReply.sfFile);
		if (state != MCO_SUCCESS) {McoErrorAlert(state); goto failLoad;}
		state = tweak->Load_Table(file);
		if (state != MCO_SUCCESS) {McoErrorAlert(state); goto failLoad;}
		state = file->closeFile();
		if (state != MCO_SUCCESS) {McoErrorAlert(state); goto failLoad;}
failLoad:
		state = MCO_SUCCESS;	// error alert takes care of error
		delete file;
		}
	}
else if (item == ids[SADJ_SAVE])
	{
	StandardPutFile("\pEnter name to save correction table as","\p",&theReply);
	if (theReply.sfGood == TRUE)
		{
		file = new FileFormat;
		if (file == NULL) return MCO_MEM_ALLOC_ERROR;
		state = file->createFilewithtype(&theReply.sfFile,theReply.sfScript,MONACO_SIG,'MPO2');
		if (state != MCO_SUCCESS) {McoErrorAlert(state); goto failSave;}
		state = tweak->Save_Table(file);
		if (state != MCO_SUCCESS) {McoErrorAlert(state); goto failSave;}
		state = file->closeFile();
		if (state != MCO_SUCCESS) {McoErrorAlert(state); goto failSave;}
failSave:
		state = MCO_SUCCESS; // error alert takes care of error
		delete file;
		}
	}
else if (item == ids[SADJ_DIFFER])
	{
	typeList[0] = 'TEXT';
	StandardGetFile(nil, 1, typeList, &theReply);
	if(theReply.sfGood == TRUE)
		{

		memcpy(inname, theReply.sfFile.name, *(theReply.sfFile.name)+1);
		PathNameFromDirID(theReply.sfFile.parID, theReply.sfFile.vRefNum, inname);
		ptocstr(inname);	
		
		fs = fopen( (char*)inname, "r" );
		if(!fs)
			return MCO_FILE_OPEN_ERROR;
		state = tweak->Load_Data(fs);
		if (state != MCO_SUCCESS) return state;
		fclose(fs);
		}
	
	}
else if (item == ids[SADJ_PICK_BOX])
	{
	do 
		{
		state = getBoxPointColor(printedLAB,gammutLAB,20,ids[SADJ_PICK_BOX] ,clickPoint);
		if (state != MCO_SUCCESS) return state;
		ConvertToRGB(printedLAB,printedRGB);
		state = DrawColorBox(ids[SADJ_Pri_Box],printedRGB);
		if (state != MCO_SUCCESS) return state;
		state = CopyTextToDial(0);
		if (state != MCO_SUCCESS) return state;
		GetMouse ( &clickPoint );
		}  while ( StillDown ( ) );
	}
else if (item == ids[SADJ_VIEW_SLIDER])
	{
	GetMouse(&mpoint);
	do {
		SliderL = GetSlider(mpoint,item);
		updateWindowData(0);
		GetMouse(&mpoint);
		} while ( StillDown ( ) );
	}
else if (item == ids[SADJ_LIGHT_RANGE])
	{
	GetMouse(&mpoint);
	do {
		Lr = GetSlider(mpoint,item);
		updateWindowData(0);
		GetMouse(&mpoint);
		} while ( StillDown ( ) );
	}
else if (item == ids[SADJ_COLOR_RANGE])
	{
	GetMouse(&mpoint);
	do {
		Cr = GetSlider(mpoint,item);
		Cr = 100*pow(Cr/100,0.2);
		updateWindowData(0);
		GetMouse(&mpoint);
		} while ( StillDown ( ) );
	}
else if (item == ids[SADJ_SAVE_PATCH])
	{
	SaveTweakedPatches();
	}
else if ((item == ids[SADJ_Des_L]) || (item == ids[SADJ_Des_a]) || (item == ids[SADJ_Des_b]) ||
		 (item == ids[SADJ_Des_R]) || (item == ids[SADJ_Des_G]) || (item == ids[SADJ_Des_B]))
	{
	CopyTextFromDial();
	Current_Text = 0;
	}
else if ((item == ids[SADJ_Pri_L]) || (item == ids[SADJ_Pri_a]) || (item == ids[SADJ_Pri_b]) ||
		 (item == ids[SADJ_Pri_R]) || (item == ids[SADJ_Pri_G]) || (item == ids[SADJ_Pri_B]))
	{
	CopyTextFromDial();
	Current_Text = 1;
	}
else if ((item == TIMER_ITEM ) && (doc->tcom != 0L))
	{   // a timer event has occured
	_last_tick_time2 = TickCount(); 
	if (!doc->tcom->IsBufferEmpty())	
		{
		if (doc->tcom->IsDataPresent())
			{
			if (Current_Text == 0)
				{
				state = doc->tcom->getNextPatch(desiredLAB);
				ConvertToRGB(desiredLAB,desiredRGB);
				ConvertToGammutC(desiredLAB,gammutLAB);
				ConvertToRGB(gammutLAB,gammutRGB);
				}
			else
				{
				state = doc->tcom->getNextPatch(printedLAB);
				ConvertToRGB(printedLAB,printedRGB);
				}
			if (state != MCO_SUCCESS) McoErrorAlert(state);
			state = MCO_SUCCESS;
			CopyTextToDial(0);
			updateWindowData(0);
			}
		}
	}
else if (item == ids[SADJ_Des_Box])
	{
	Current_Text = 0;
	DrawWindow();
	}
else if (item == ids[SADJ_Pri_Box])
	{
	Current_Text = 1;
	DrawWindow();
	}
return state;	
}
		