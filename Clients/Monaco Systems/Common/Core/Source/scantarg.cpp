/*  scantarg.c				 		    */
/*		Scan the target for the data    */
/* October 7, 1993						*/
/* Copyright 1993 Monaco Systems Inc.	*/
/*										*/


#include "scantarg.h"
/*
#include "dart.h"
#include "SC_rsrc.h"
*/
//total # of patches been checked
#define TOTAL_PATCHES	12
#define HOR_POINTS 5
#define VER_POINTS 5

//area where data from in a single patch in normalized space
#define HOR_NOR_LENGTH (1.0/69.0)	//(0.0151515)
#define VER_NOR_LENGTH (1.0/39.0)	//(0.0277777)

//total # of components points in a single patch
#define TOTAL_POINTS	(HOR_POINTS*VER_POINTS)
#define TOTAL_COMP_POINTS	(HOR_POINTS*VER_POINTS*3)

//match points in single patch in vertical direction
//at least 2/3 have to be matched in all three r, g, b components
#define MIN_VER_MATCH_POINTS (VER_POINTS*2)
#define MIN_HOR_MATCH_POINTS (HOR_POINTS*2)

//points tolerance for point match in patches
#define POINT_TOLERANCE	10

//min of descending patches
//for statistical reason, one patch can be off 
#define MIN_NEUTRAL_PATCHES (TOTAL_PATCHES -1)

//min of descending patches
#define MIN_DESCEND_PATCHES (TOTAL_PATCHES -1)

//min of match patches( 1/3 grid), strict checking
#define MIN_MATCH_PATCHES (TOTAL_PATCHES)

//min of neutral points to make this patch neutral
#define MIN_NEUTRAL_POINTS (TOTAL_POINTS -3)

//max of ascending points in a patch
#define MAX_ASCEND_POINTS	(3*HOR_POINTS)

void McoErrorState(int num, char *vn);

//transform from normalized space to target space
void ScanTarg::Transform(SCPoint *pin,SCPoint *pout)
{
pout->h = a*pin->h+b*pin->v+c;
pout->v = d*pin->h+e*pin->v+f;
}

//transform from target space to normalized space
void ScanTarg::InvsTransform(SCPoint *pin,SCPoint *pout)
{
pout->h = (-c*e+b*f+e*pin->h-b*pin->v)/(-b*d+a*e);
pout->v = ( c*d-a*f-d*pin->h+a*pin->v)/(-b*d+a*e);
}

// Find the rectangle which encloses the region to be scanned
void ScanTarg::GetFullRect(SCPoint *topleft,SCPoint *botright,Rect *r)
{
	SCPoint p1,p2;
	
	r->top = 32000;
	r->bottom = -32000;
	r->right = -32000;
	r->left = 32000;
	
	p2.h=topleft->h;
	p2.v=topleft->v;
	
	r->top = MinVal(r->top,p2.v);
	r->bottom = MaxVal(r->bottom,p2.v);
	r->right = MaxVal(r->right,p2.h);
	r->left = MinVal(r->left,p2.h);
	
	p2.h=botright->h;
	p2.v=topleft->v;
	
	r->top = MinVal(r->top,p2.v);
	r->bottom = MaxVal(r->bottom,p2.v);
	r->right = MaxVal(r->right,p2.h);
	r->left = MinVal(r->left,p2.h);
	
	p2.h=topleft->h;
	p2.v=botright->v;
	
	r->top = MinVal(r->top,p2.v);
	r->bottom = MaxVal(r->bottom,p2.v);
	r->right = MaxVal(r->right,p2.h);
	r->left = MinVal(r->left,p2.h);
	
	p2.h=botright->h;
	p2.v=botright->v;
	
	
	r->top = MinVal(r->top,p2.v);
	r->bottom = MaxVal(r->bottom,p2.v);
	r->right = MaxVal(r->right,p2.h);
	r->left = MinVal(r->left,p2.h);
}

// Get the RGB values for the point p in the bitmap data.
void ScanTarg::GetRGB(Rect *inrect,SCPoint *p,unsigned char *data,double *colors)
{
int32		num_c,r,c;
char		ts[255];

//num_c=(int)(inrect->right-inrect->left)*3;
r = (int32)(p->v - inrect->top)*rbyte;
c = (int32)(p->h - inrect->left)*num_cmp;
if (num_cmp == 3)
	{
	colors[0]=(double)(data[c+r])/255;
	colors[1]=(double)(data[c+r+1])/255;
	colors[2]=(double)(data[c+r+2])/255;
	}
if (num_cmp == 1)
	{
	colors[0]=(double)(data[c+r])/255;
	colors[1]=(double)(data[c+r])/255;
	colors[2]=(double)(data[c+r])/255;
	}
//sprintf(ts,"%d %d\n%d %d %d %f %f %f",c,r,data[c+r],data[c+r+1],data[c+r+2]);
//McoErrorState(0,ts);
}

void ScanTarg::GetRGBInt(Rect *inrect,SCPoint *p,unsigned char *data,int16 *colors)
{
	int32		num_c,r,c;
	char		ts[255];
	
	r = (int32)(p->v - inrect->top)*rbyte;
	c = (int32)(p->h - inrect->left)*num_cmp;
	if (num_cmp == 3){
		colors[0]=(int16)data[c+r];
		colors[1]=(int16)data[c+r+1];
		colors[2]=(int16)data[c+r+2];
	}
	if (num_cmp == 1){
		colors[0] = (int16)data[c+r];
		colors[1] = (int16)data[c+r];
		colors[2] = (int16)data[c+r];
	}
}


McoStatus ScanTarg::ScanSingTarg(Rect *inrect, SCRect *scanarea,unsigned char *data, short *colors)
{
	//dart	*dn;
	int32 	i,count;
	SCPoint	p1,p2,p3;
	double   input[3];
	double   input2[3];
	McoStatus	state;
	
	state = MCO_SUCCESS;
	
	count = 0;
	p1.h=1;
	p1.v=1;
	InvsTransform(&p1,&p2);
	for (i=0; i<3; i++) input2[i]=0;
	for (p3.h=scanarea->left; p3.h<scanarea->right; p3.h += p2.h)
		for (p3.v=scanarea->top; p3.v<scanarea->bottom; p3.v += p2.v){
			count ++;
			Transform(&p3,&p1);
			GetRGB( inrect, &p1, data, input);
			for (i=0; i<3; i++) input2[i] += input[i];
		}
	colors[0]=(short)(input2[0]*255/count);
	colors[1]=(short)(input2[1]*255/count);
	colors[2]=(short)(input2[2]*255/count);
	return MCO_SUCCESS;
}

McoStatus ScanTarg::ScanSingTarg2(Rect *inrect, SCRect *scanarea,unsigned char *data, unsigned char *colors)
{
	//dart	*dn;
	int32 	i,count;
	SCPoint	p1,p2,p3;
	double   input[3];
	double   input2[3];
	McoStatus	state;
	
	state = MCO_SUCCESS;
	
	count = 0;
	p1.h=1;
	p1.v=1;
	InvsTransform(&p1,&p2);
	for (i=0; i<3; i++) input2[i]=0;
	for (p3.h=scanarea->left + p2.h; p3.h<scanarea->right - p2.h; p3.h += p2.h)
		for (p3.v=scanarea->top + p2.v; p3.v<scanarea->bottom - p2.v; p3.v += p2.v){
			count ++;
			Transform(&p3,&p1);
			GetRGB( inrect, &p1, data, input);
			for (i=0; i<3; i++) input2[i] += input[i];
		}
	colors[0]=(unsigned char)((input2[0]*255.0/count) + 0.5);
	colors[1]=(unsigned char)((input2[1]*255.0/count) + 0.5);
	colors[2]=(unsigned char)((input2[2]*255.0/count) + 0.5);
	return MCO_SUCCESS;
}

int32	ScanTarg::_horcheck(Rect *inrect, SCPoint *p, unsigned char *data)
{
	double i;
	int32 j;
	SCPoint p1, p2, p3, p4, p5, p6;
	int16	input1[3], input2[3];
	int32	matchpoints = 0, counts = 0;
	int16	val;
	

	p5.h=1;
	p5.v=1;
	InvsTransform(&p5,&p6);
	p3.v = p->v;
	p4.v = p->v + VER_NOR_LENGTH;
	for (i = 0; i < HOR_NOR_LENGTH; i += p6.h){
		counts++;
		p3.h = p->h + i;
		Transform(&p3,&p1);
		GetRGBInt( inrect, &p1, data, input1);

		p4.h = p3.h;
		Transform(&p4,&p1);
		GetRGBInt( inrect, &p1, data, input2);
		for(j = 0; j < 3; j++){
			val = input1[j] - input2[j];
			if( val < POINT_TOLERANCE && val > -POINT_TOLERANCE)
				matchpoints++;
		}		
	}
	
	if( (double)matchpoints/(double)(counts*3) > 0.6 )
		return 1;
	else
		return 0;
}		


int32	ScanTarg::_vercheck(Rect *inrect, SCPoint *p, unsigned char *data)
{
	double i;
	int32 j;
	SCPoint p1, p2, p3, p4, p5, p6;
	int16	input1[3], input2[3];
	int32	matchpoints = 0, counts = 0;
	int16	val;
	

	p5.h=1;
	p5.v=1;
	InvsTransform(&p5,&p6);
	p3.h = p->h;
	p4.h = p->h + HOR_NOR_LENGTH;
	for (i = 0; i < VER_NOR_LENGTH; i += p6.v){
		counts++;
		p3.v = p->v + i;
		Transform(&p3,&p1);
		GetRGBInt( inrect, &p1, data, input1);

		p4.v = p3.v;
		Transform(&p4,&p1);
		GetRGBInt( inrect, &p1, data, input2);
		for(j = 0; j < 3; j++){
			val = input1[j] - input2[j];
			if( val < POINT_TOLERANCE && val > -POINT_TOLERANCE)
				matchpoints++;
		}		
	}
	
	if( (double)matchpoints/(double)(counts*3) > 0.6 )
		return 1;
	else
		return 0;
}		


//current scanning sequence
//	0 	5	10	15	20 <- horizontal checking line
//	1	6	11	16	21
//	2	7	12	17	22
//	3	8	13	18	23
//	4	9	14	19	24 <- horizontal checking line
//  ^---------------^---- vertical checking line

McoStatus ScanTarg::CheckScanTarg(Rect *inrect, SCRect *scanarea,unsigned char *data, 
	int32 in_tolerance)
{
	int32 	h, i, j, k;
	SCPoint	p1,p2,p3;
	int16   input[3];
	double   input2[3];
	McoStatus	state;
	
	//modified for checking targets
	int32	minpatch;	//values of patches which has min value scanned so far, init to 255
	int32	descendpatches = 0;	//# of patches which has value larger than previous one
	int32	vermatch = 0;	//# of patches with matched value vertically
	int32	hormatch = 0;	//# of patches with matched value horizontally
	int32	singlevermatch;	//# of points with matched value vertically in a patch
	int32	singlehormatch;	//# of points with matched value horizontally in a patch
	int32	blackpatchred; //values of black patches red component
	int32	verpoints = VER_POINTS;	//# of points measured vertically
	int32	horpoints = HOR_POINTS; 	//# of points measured horizontally
	int16	firstpatch[TOTAL_COMP_POINTS];
	int16	lastpatch[TOTAL_COMP_POINTS];
	int16	curpatch[TOTAL_COMP_POINTS];
	int16	*firstpatchptr, *lastpatchptr, *curpatchptr;
	int16	*tempfirstptr, *templastptr, *tempcurptr;
	int32	neutralpoints;	//# of neutral points in a singular patch
	int32	neutral_tolerance = in_tolerance; // value of neutral tolerance
	int32	neutralpatches = 0; //# of neutral patches
	int32	ascendpoints;	//# of ascending points in a patch
	int16	min, max;
	double	ver_step;

	state = MCO_SUCCESS;

	//initialize the last patch
	for (i = 0; i < HOR_POINTS; i++)
		for (j = 0; j < VER_POINTS; j++)
			for(k = 0; k < 3; k++){
				firstpatch[(i*VER_POINTS +j)*3 + k] = 255;
				lastpatch[(i*VER_POINTS +j)*3 + k] = 255;
			}

	firstpatchptr = firstpatch;
	lastpatchptr = lastpatch;
	curpatchptr = curpatch;
	//one step value in normalized space
	p2.h = HOR_NOR_LENGTH/(double)horpoints;
	p2.v = VER_NOR_LENGTH/(double)verpoints; 

	for( h = 0; h < TOTAL_PATCHES; h++){

		//offset part plus moving part
		ver_step = VER_NOR_LENGTH*1.5 + h*(VER_NOR_LENGTH*3);

		//scanning the targets
		for (i = 0; i < HOR_POINTS; i++)
			for (j = 0; j < VER_POINTS; j++){
				p3.h = scanarea->left + i*p2.h;
				p3.v = scanarea->top + ver_step + VER_NOR_LENGTH + j*p2.v;
				Transform(&p3,&p1);
				GetRGBInt( inrect, &p1, data, input);
				for(k = 0; k < 3; k++)
					curpatchptr[(i*VER_POINTS +j)*3 + k] = input[k];
			}
	
		//check integrity within the patch
		//left and right two lines
		singlevermatch = 0;
		for (j = 0; j < VER_POINTS; j++)
			for(k = 0; k < 3; k++){
				int16 val=curpatchptr[j*3+k]-curpatchptr[((HOR_POINTS-1)*(VER_POINTS)+j)*3+k];
				if( val < POINT_TOLERANCE && val > -POINT_TOLERANCE)
					singlevermatch++;
			}
	
		if(singlevermatch > MIN_VER_MATCH_POINTS)
			vermatch++;
		else{
				p3.h = scanarea->left;
				p3.v = scanarea->top + ver_step + VER_NOR_LENGTH;
				if(_vercheck(inrect, &p3, data))
					vermatch++;
		}

		//top and bottom two lines
		singlehormatch = 0;
		for (i = 0; i < HOR_POINTS; i++)
			for(k = 0; k < 3; k++){
				int16 val=curpatchptr[(HOR_POINTS*i)*3+k]-curpatchptr[(HOR_POINTS*i+VER_POINTS-1)*3+k];
				if( val < POINT_TOLERANCE && val > -POINT_TOLERANCE)
					singlehormatch++;
			}
	
		if(singlehormatch > MIN_HOR_MATCH_POINTS)
			hormatch++;
		else{
				p3.h = scanarea->left;
				p3.v = scanarea->top + ver_step + VER_NOR_LENGTH;
				if(_horcheck(inrect, &p3, data))
					hormatch++;
		}

		//check neutrality of patch, the patches should be neutral patch
		//check every points in the patches
		neutralpoints = 0;
		for (i = 0; i < TOTAL_COMP_POINTS; i+=3){
			max = curpatchptr[i];
			min = curpatchptr[i];
			if( curpatchptr[i+1] > max)
				max = curpatchptr[i+1];
			else
				min = curpatchptr[i+1];
	
			if( curpatchptr[i+2] > max)
				max = curpatchptr[i+2];
			else if( curpatchptr[i+2] < min)
				min = curpatchptr[i+2];
	
			if(max - min < neutral_tolerance)
				neutralpoints++;
		}

		//is this patch a neutral patch
		if(neutralpoints >= MIN_NEUTRAL_POINTS)
			neutralpatches++;
	
		ascendpoints = 0;
		//is this patch in descending order compare to the last patch
		tempfirstptr = firstpatchptr;
		templastptr = lastpatchptr;
		tempcurptr = curpatchptr;
		for(i = 0; i < TOTAL_COMP_POINTS; i++)
			if( *curpatchptr++ > *lastpatchptr++)
				ascendpoints++;
	
		if(ascendpoints < MAX_ASCEND_POINTS)
			descendpatches++;
		else{	//compare cur patch with first patch
			curpatchptr = tempcurptr;
			ascendpoints = 0;
			for(i = 0; i < TOTAL_COMP_POINTS; i++)
				if( *curpatchptr++ > *firstpatchptr++)
					ascendpoints++;
			if(ascendpoints >= MAX_ASCEND_POINTS)
				descendpatches--;	//something wrong
		}	

		//exchange curpatchptr and lastpatchptr
		curpatchptr = tempfirstptr;
		lastpatchptr = tempcurptr;
		firstpatchptr = templastptr;
	}

	//finally, is this a real neutral patches?
	if(vermatch < MIN_MATCH_PATCHES || hormatch < MIN_MATCH_PATCHES)
		return MCO_BADTARGET;

	if( neutralpatches < MIN_NEUTRAL_PATCHES)
		return MCO_BADTARGET;

	if( descendpatches < MIN_DESCEND_PATCHES)
		return MCO_BADTARGET;

	return MCO_SUCCESS;
}


/* Set the variables for the transform */
void ScanTarg::SetTransform(Point *TopRight, Point *TopLeft, Point *BotLeft) 
{
a = TopRight->h - TopLeft->h;
b = BotLeft->h - TopLeft->h;
c = TopLeft->h;
d = TopRight->v - TopLeft->v;
e = BotLeft->v - TopLeft->v;
f = TopLeft->v;
}

McoPatches::McoPatches(int np)
{
num_patches = np;
num_duplicate = 0;
max_dif = 0;
patches = new SCRect[np];
}

McoPatches::~McoPatches(void)
{
if (patches) delete patches;
}