////////////////////////////////////////////////////////////////////////////////////
// The table base class
// table.cpp
// James Vogh
// (c) Monaco Systems Inc.
// 6/25/97
////////////////////////////////////////////////////////////////////////////////////

#include "tablepc.h"
#include <math.h>

#define	MinVal(a,b)			(((a) > (b)) ? (b) : (a))
#define	MaxVal(a,b)			(((a) < (b)) ? (b) : (a))

Table::Table(void)
{
current_x = 0;
current_y = 0;

pointArray = NULL; 

Tx = 0;
Ty = 0;

	Tx = 355;
	Ty = 273;

	a = 10;
	b = 0;
	c = 0;
	d = -10;

}

Table::~Table(void)
{
if (pointArray) delete pointArray;
}


void Table::getPatchPoint(int32 patchnum,double *point)
{
point[0] = pointArray[patchnum*2];
point[1] = pointArray[patchnum*2+1];
}


// translate a patch location to 
void Table::translateToTable(int patchnum, int32 *table)
{
	double x,y;

	x = pointArray[patchnum*2];
	y = pointArray[patchnum*2+1];

	table[0] = a*x+b*y+Tx+0.5;
	table[1] = c*x+d*y+Ty+0.5;

	table[0] = MaxVal(table[0],min_x);
	table[0] = MinVal(table[0],max_x);
	table[1] = MaxVal(table[1],min_y);
	table[1] = MinVal(table[1],max_y);
}

// get the current location of the table
McoStatus Table::getLocation(int32 *position,int ref)
{
position[0] = current_x;
position[1] = current_y;
return MCO_SUCCESS;
}

McoStatus Table::setPoints_1(int32 *tablePoints,double *patchPoints)
{
double tx,ty;
double nx,ny;
double x,y;

x = patchPoints[2];
y = patchPoints[3];

tx = tablePoints[0];
ty = tablePoints[1];

nx = a*x+b*y+tx+0.5;
ny = c*x+d*y+ty+0.5;

if ((nx <= max_x) && (nx >= min_x) && (ny <= max_y) && (ny >= min_y))
	{
	Tx = tx;
	Ty = ty;
	}
else
	{
	if ((nx > max_x) || (nx < min_x))
		{
		a *= -1;
		}
	if ((ny > max_y) || (ny < min_y))
		{
		d *= -1;
		}
	Tx = tx;
	Ty = ty;
	}
return MCO_SUCCESS;
}

double Table::_getMaxDiff(double x,double y)
{
double diff = 0;

if (x < min_x) diff = min_x - x;
if ((x > max_x) && (x - max_x > diff)) diff = x - max_x;

if ((y < min_y) && (min_y -y > diff)) diff = min_y - y;
if ((y > max_y) && (y - max_y > diff)) diff = y - max_y;

return diff;
}

McoStatus Table::setPoints_2(int32 *tablePoints,double *patchPoints)
{
double x1,x2,xp1,xp2,xp2_1,xp2_2;
double y1,y2,yp1,yp2,yp2_1,yp2_2;
double A,B,C,D,E,F;
double a1,b1,a2,b2,a3,b3,a4,b4;
double md_1,md_2;
double sq;

// first need to find the third table point
// assume that scaling does not change and that no skewing occurs

x1 = patchPoints[2];
y1 = patchPoints[3];

x2 = patchPoints[4];
y2 = patchPoints[5];

xp1 = tablePoints[2] - tablePoints[0];
yp1 = tablePoints[3] - tablePoints[1];

A = sqrt(xp1*xp1+yp1*yp1);
B = sqrt(x1*x1+y1*y1);
C = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
D = y1/x2;
E = (A*C/B)*(A*C/B);
F = (A/D)*(A/D);

a1 = xp1;
if (a1 == 0) a1 = 1E-20;
b1 = yp1;
if (b1 == 0) b1 = 1E-20;

a2 = a1*a1;
b2 = b1*b1;

a3 = a2*a1;
b3 = b2*b1;

a4 = a2*a2;
b4 = b2*b2;

sq = sqrt(-a2*(a4+b4+2*a2*(b2-E-F)+(E-F)*(E-F)-2*b2*(E+F)));

// calculate the two sets of answers

xp2_1 = tablePoints[0] + (a4+a2*(b2-E+F)-b1*sq)/(2*a1*(a2+b2));
yp2_1 = tablePoints[1] + (a2*b1+b3+b1*(-E+F)+sq)/(2*(a2+b2));

xp2_2 = tablePoints[0] + (a4+a2*(b2-E+F)+b1*sq)/(2*a1*(a2+b2));
yp2_2 = tablePoints[1] + (a2*b1+b3+b1*(-E+F)-sq)/(2*(a2+b2));

// find out which set is correct

if ((xp2_1 <= max_x) && (xp2_1 >= min_x) && (yp2_1 <= max_y) && (yp2_1 >= min_y))
	{
	// use first set
	xp2 = xp2_1;
	yp2 = yp2_1;
	}
else if ((xp2_2 <= max_x) && (xp2_2 >= min_x) && (yp2_2 <= max_y) && (yp2_2 >= min_y))
	{
	// use second set
	xp2 = xp2_2;
	yp2 = yp2_2;
	}
else // neither set produced a good point, try to find the best
	{
	md_1 = _getMaxDiff(xp2_1,yp2_1);
	md_2 = _getMaxDiff(xp2_2,yp2_2);
	if (md_1 < md_2)
		{
		// use first set
		xp2 = xp2_1;
		yp2 = yp2_1;	
		}
	else
		{
		// use second set
		xp2 = xp2_2;
		yp2 = yp2_2;
		}
	}

a = (-xp2*y1+Tx*(y1-y2)+xp1*y2)/(-x2*y1+x1*y2);
b = (Tx*(-x1+x2)-x2*xp1+x1*xp2)/(-x2*y1+x1*y2);
c = (Ty*(y1-y2)+y2*yp1-y1*yp2)/(-x2*y1+x1*y2);
d = (Ty*(-x1+x2)-x2*yp1+x1*yp2)/(-x2*y1+x1*y2);

return MCO_SUCCESS;
}


// get the rotation, scale, and displacement from a set of points
McoStatus Table::setPoints_3(int32 *tablePoints,double *patchPoints)
{
double x1,x2,xp1,xp2;
double y1,y2,yp1,yp2;


Tx = tablePoints[0];
Ty = tablePoints[1];

x1 = patchPoints[2];
y1 = patchPoints[3];

x2 = patchPoints[4];
y2 = patchPoints[5];

xp1 = tablePoints[2];
yp1 = tablePoints[3];

xp2 = tablePoints[4];
yp2 = tablePoints[5];

a = (-xp2*y1+Tx*(y1-y2)+xp1*y2)/(-x2*y1+x1*y2);
b = (Tx*(-x1+x2)-x2*xp1+x1*xp2)/(-x2*y1+x1*y2);
c = (Ty*(y1-y2)+y2*yp1-y1*yp2)/(-x2*y1+x1*y2);
d = (Ty*(-x1+x2)-x2*yp1+x1*yp2)/(-x2*y1+x1*y2);

return MCO_SUCCESS;
}


// width and height are in milimeters

// variable first specifies the loacation of the first patch
// and has the following meaning
//
//    0			1
//
//
//	  2			3
//

// Variable direction specifies if the patches should be read horizontally or vertically
// 0 = horz, 1 = vert

McoStatus Table::setUpPatches(int32 numCol,int32 numRow,double w,double h,int first, int direction, 
							  TableRect *tablerect)
{
double col_dx,col_dy;
double row_dx,row_dy;
int32	c;
double	x0,y0,x1,y1;
int	i,j;

// build an array with relative x&y points

if (pointArray) delete pointArray;

pointArray = new double[numCol*numRow*2];
if (!pointArray) return MCO_MEM_ALLOC_ERROR;

min_x = tablerect->min_x;
max_x = tablerect->max_x;
min_y = tablerect->min_y;
max_y = tablerect->max_y;

switch (direction) {
	case 0:	//  move horizontally
	switch (first) {
		case 0: 
			col_dx = 0;
			col_dy = -h/(double)numCol;
			row_dx = w/(double)numRow;
			row_dy = 0;
			break;
		case 1:
			col_dx = 0;
			col_dy = -h/(double)numCol;
			row_dx = -w/(double)numRow;
			row_dy = 0;
			break;
		case 2:
			col_dx = 0;
			col_dy = h/(double)numCol;;
			row_dx = w/(double)numRow;
			row_dy = 0;
			break;
		case 3:
			col_dx = 0;
			col_dy = h/(double)numCol;;
			row_dx = -w/(double)numRow;
			row_dy = 0;
			break;
		}
		break;
	case 1: // move vertically
		switch (first) {
		case 0: 
			col_dx = w/(double)numCol;
			col_dy = 0;
			row_dx = 0;
			row_dy = -h/(double)numRow;
			break;
		case 1:
			col_dx = -w/(double)numCol;
			col_dy = 0;
			row_dx = 0;
			row_dy = -h/(double)numRow;
			break;
		case 2:
			col_dx = w/(double)numCol;
			col_dy = 0;
			row_dx = 0;
			row_dy = h/(double)numRow;
			break;
		case 3:
			col_dx = -w/(double)numCol;
			col_dy = 0;
			row_dx = 0;
			row_dy = h/(double)numRow;
			break;
		}
		break;
	}		

x0 = 0;
y0 = 0;
c = 0;
for (i=0; i<numCol; i++)
	{
	x1 = x0;
	y1 = y0;
	for (j=0; j<numRow; j++)
		{
		pointArray[c*2] = x1;
		pointArray[c*2+1] = y1;
		c++;
		x1 += row_dx;
		y1 += row_dy;
		}
	x0 += col_dx;
	y0 += col_dy;
	}	

return MCO_SUCCESS;
}