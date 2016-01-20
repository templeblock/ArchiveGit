#ifndef IN_CURVCONTROL
#define IN_CURVCONTROL
/* curvecont.h									 	*/
/* 			The curve control class   				*/
/* Sept 27, 1993								  	*/
/* converted to a control Oct 1997					*/
/* Copyright 1993-1997 Monaco Systems Inc			  */

#include <QDOffscreen.h>
#include "control.h"

#define REFRESH	200
#define MaxDialLuts 3 		// The max # of luts used within the curve dialog
#define MaxNumCmps		(4)
#define MaxNumHands		(256)
#define	MAX_POINTS	(400)


#define NUM_HP_IDS	14


typedef enum {
	C_SplineCurve = 0,
	C_PicLinearCurve
	} MS_CurveTypes;	

typedef enum {
	C_CurveWin = 0,	// the id of pane where the curev is displayed
	C_Text1,		// X value
	C_Text2,		// Y value
	C_Text3,		// color name
	C_Reset,		// reset button
	C_ColorMenu,	// id of the menu
	C_ColorMenuID,	// id of the menu
	C_Menu_STRS_ID,	// names of the items in the menu
	C_AllNum,		// the number in the menu of the "ALL" item 
	C_FirstNum,
	C_Pict,			// the id of the pict for the background
	C_Top,
	C_Left,
	C_Bottom,
	C_Right
	} MS_CDialogIDs;
	
#define NUM_CURVE_CNT_IDS (C_Right+1)
#define NUM_CURVE_DIAL_IDS (C_ColorMenuID+1)
	
/* flag definitions */

typedef enum {
	Scanner_Flag = 0
	} MS_Flags;
	
	


class CurveControl:public control {
private:
protected:



int16		num_luts;								// The number of lookup tables
int16		num_cmps[MaxDialLuts];					// The number of components in each lookup table
int16		num_hands[MaxDialLuts][MaxNumCmps];		// The number of hands in each 
int16		lut_color[MaxDialLuts][MaxNumCmps];		// The color of each lookup table
int16		active_luts[MaxDialLuts];				// Flags indicating which lookup tables are active
int16		active_cmps[MaxDialLuts][MaxNumCmps];	// Active components of each lookup table
int16		freshen_cmp[MaxDialLuts][MaxNumCmps];	// A flag indicating that a component needs updating
double		h_x[MaxDialLuts][MaxNumCmps*MaxNumHands];	// x values for all lookup tables, all are between 0 and 1
double		h_y[MaxDialLuts][MaxNumCmps*MaxNumHands];	// y values for all lookup tables, all are between 0 and 1
double		hmin[MaxDialLuts],hmax[MaxDialLuts];	// Maximum and minimum horizontal values
double		vmin[MaxDialLuts],vmax[MaxDialLuts];	// Maximum and minimum vertical values

int16		points[MaxDialLuts][MaxNumCmps*MAX_POINTS];	// the points that make up the curve
int32		PointsInCurve;														// points are in screen coordinates

MS_CurveTypes		curve_type[MaxDialLuts][MaxNumCmps];		// curve is spline or piecewise linear


int16 		c_text1_num,c_text2_num;

int16		ids[NUM_CURVE_CNT_IDS];

int16		active_lut;
int16		active_cmp;
int16		current_cmp;
int16		active_hand;

// The following are used by the menu to determine what menu item coresponds with each lookup table and component


int32		REDS[NUM_HP_IDS];
int32		GREENS[NUM_HP_IDS];
int32		BLUES[NUM_HP_IDS];


RGBColor	background;

int			lastmenu;

McoStatus 	_init( short *control_ids,int startNum);
void 		_setColorMenu(void);
int16 		_getHandNum(int x, int y);
McoStatus 	_addHandle(int16 i, int16 j, double x, double y);
McoStatus 	_deleteHandle(int16 i, int16 j, int16 k);
McoStatus 	_resetCurve(int16 i, int16 j);
Boolean 	_getCurveNum(int x, int y);
McoStatus 	_copyData24to32Or(GWorldPtr src,unsigned char *data,int32 dataRowBytes,Rect *dataRect, int count);
void 		_drawHands(GWorldPtr src, Rect *r);
void 		_drawCurve(Rect *r,int16 *points, RGBColor *c);
void 		_drawCurveWin(void);
void 		_getPoints(void);
Boolean		_dragHand(void);
McoStatus 	_updateCurveData(void);

Rect		wr;

int			limitExtend;
int			handFlag;

baseWin		*parentWin;
int			controlNum;

public:


	CurveControl(WindowPtr wp, short *control_ids,int startNum,int hf, int lf,baseWin *pw,int cn);		
	~CurveControl(void);		

// update the data in the screen
McoStatus 	UpdateControl(void);

void 		SetUpCurves(int num_lut,int *num_cmp,double *hmi,double *hma, double *vmi, double *vma);

McoStatus 	CopyIntoCurves(int ln, int cn,int num_h,double xmi,double xma, double ymi, double yma,double *datax,double *datay);
void 		CopyOutofCurves(int ln, int cn,int *num_h,double xmi,double xma, double ymi, double yma,double *datax,double *datay);
// get the active hand
void getActiveHand(int *hand);

// redraw the control
McoStatus 	CopyParameters(void);

// do the events for the control
McoStatus	DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed,int *didit);


};

/* Draw the curve */


#endif


