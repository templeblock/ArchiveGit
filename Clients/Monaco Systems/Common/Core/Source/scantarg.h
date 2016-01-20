/*  scantarg.h				 		    */
/*		Scan the target for the data    */
/* October 7, 1993						*/
/* Copyright 1993 Monaco Systems Inc.	*/
/*										*/

#ifndef SCANTARG
#define SCANTARG

#include "mcotypes.h"
#include "mcostat.h"
#include "monitor.h"
#include "monacoprint.h"

//#define MAX_NUM_PATCHES 264
//#define MAX_NUM_DUPL	1
//#define MAX_NUM_PATCHES 180
#define MAX_NUM_DUPL	1

#define	MinVal(a,b)			(((a) > (b)) ? (b) : (a))
#define	MaxVal(a,b)			(((a) < (b)) ? (b) : (a))


typedef struct {
double	h,v;
}SCPoint;

typedef struct  {
double 	top,bottom,left,right;
}SCRect;

class McoPatches {
private:
protected:
public:
	int16	num_patches;
	int16	num_duplicate;
	double	max_dif;
	SCRect	*patches;

	McoPatches(int np);
	~McoPatches(void);

	};


class ScanTarg {

private:
protected:

double	a,b,c,d,e,f;

int32	_horcheck(Rect *inrect, SCPoint *p, unsigned char *data);
int32	_vercheck(Rect *inrect, SCPoint *p, unsigned char *data);

public:
int16	num_cmp;
int32	rbyte;

void Transform(SCPoint *pin,SCPoint *pout);
void InvsTransform(SCPoint *pin,SCPoint *pout);
void GetFullRect(SCPoint *topleft,SCPoint *botright,Rect *r);
void GetRGB(Rect *inrect,SCPoint *p,unsigned char *data,double *colors);
void GetRGBInt(Rect *inrect,SCPoint *p,unsigned char *data,int16 *colors);
McoStatus ScanSingTarg(Rect *inrect, SCRect *scanarea,unsigned char *data, short *colors);
void SetTransform(Point *TopRight, Point *TopLeft, Point *BotLeft);
McoStatus CheckScanTarg(Rect *inrect, SCRect *scanarea,unsigned char *data, 
	int32 in_tolerance);

McoStatus ScanSingTarg2(Rect *inrect, SCRect *scanarea,unsigned char *data, unsigned char *colors);

};

#endif