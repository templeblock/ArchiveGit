#include <math.h>
#include "line.p"

/******************************************************************************
From comp.graphics.algorithms FAQ

7) How do I find the distance from a point to a line?


    Let the point be C (XC,YC) and the line be AB (XA,YA) to (XB,YB).
    The length of the line segment AB is L:

        L=((XB-XA)**2+(YB-YA)**2)**0.5

    and
            (YA-YC)(YA-YB)-(XA-XC)(XB-XA)
        r = -----------------------------
                        L**2

            (YA-YC)(XB-XA)-(XA-XC)(YB-YA)
        s = -----------------------------
                        L**2

    Let I be the point of perpendicular projection of C onto AB, the

        XI=XA+r(XB-XA)
        YI=YA+r(YB-YA)

    Distance from A to I = r*L
    Distance from C to I = s*L

    If r<0      I is on backward extension of AB
    If r>1      I is on ahead extension of AB
    If 0<=r<=1  I is on AB

    If s<0      C is left of AB (you can just check the numerator)
    If s>0      C is right of AB
    If s=0      C is on AB
******************************************************************************/

//************************************************************************
long LineLength( long XA, long YA, long XB, long YB)
//************************************************************************
{
	double L2 = pow(XB - XA, 2) + pow(YB - YA, 2);
	return (long)(pow(L2, 0.5));
}

//************************************************************************
double LineLengthSquared( long XA, long YA, long XB, long YB)
//************************************************************************
{
	return pow(XB - XA, 2) + pow(YB - YA, 2);
}

//************************************************************************
int PointHitLine(double dL2,			// Length already squared
				 long XA, long YA,		
				 long XB, long YB, 
				 long XC, long YC)
//************************************************************************
{
	double s = ((YA-YC)*(XB-XA) - (XA-XC)*(YB-YA)) / dL2;
	if ( s <= 0 )
		return 1;

	return 0;
}