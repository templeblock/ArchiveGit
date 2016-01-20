/* (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
POINTUTL.C : basic bezier and point utilities
********************************************************************************
********************************************************************************/
//********************************* Constants ******************************* 

#include "pp.h"
#include "pointutl.h"

//*********************************** Macros ******************************** 

#define SGN(a) (((a) < 0) ? -1 : 0)
#define MAXDEPTH 12
#define EPSILON  .00013


/* ***************************** Local Functions **************************** */

void 	LOCAL convert_to_5th_bezier_form(LPDPOINT w, LPDPOINT lpTest , LPDPOINT Bezier);
int 	LOCAL find_roots(LPDPOINT pW, double *pt, int depth);
int     LOCAL  crossing_count(LPDPOINT pQ);
int     LOCAL  control_polygon_flat_enough(LPDPOINT pQ);
double  LOCAL  compute_X_intercept(LPDPOINT pQ);
void 	LOCAL  para_bezier(LPDPOINT FBezier, int degree, double t, LPDPOINT lpResult, LPDPOINT lpLeft = NULL, LPDPOINT lpRight = NULL);



//=======================================================================
//  split the bezier section at parameter T using deCastelgieu's algorithm
//=======================================================================
void SplitBezier(LPPOINT BezierIn, LPPOINT BezierOut1, LPPOINT BezierOut2, double T)
{
    DPOINT Tmp[BEZIER_IN];
    int Order, Idx, MaxIdx;
	double l;
	
	for (Idx=0; Idx<BEZIER_IN;Idx++)
    {
		Tmp[Idx].x = BezierIn[Idx].x;
		Tmp[Idx].y = BezierIn[Idx].y;
    } 
    for (Order = 0; Order < BEZIER_IN; Order++)
    {
        MaxIdx = BEZIER_IN - Order - 1;
        BezierOut1[Order].x = (int)Tmp[0].x;
        BezierOut1[Order].y = (int)Tmp[0].y;
        BezierOut2[MaxIdx].x = (int)Tmp[MaxIdx].x;
        BezierOut2[MaxIdx].y = (int)Tmp[MaxIdx].y;
        for (Idx = 0; Idx < MaxIdx; Idx++)
        {
            l = T*Tmp[Idx + 1].x;
            Tmp[Idx].x = l + (1.0-T)*Tmp[Idx].x;
			l = T*Tmp[Idx + 1].y;
            Tmp[Idx].y = l + (1.0-T)*Tmp[Idx].y;
        }
    }
}

//=======================================================================
// 	based on DS code.
//	On entry lpBezier should be an array of 4 points.
//	lpFoundPt will be the point closest to lpTestPt on the bezier.
//	Returns the bezier parameter of lpFoundPt.
//=======================================================================
double BezierClosest(LPPOINT lpFoundPt, LPPOINT lpTestPt, LPPOINT Bezier)
//=======================================================================
{                                                                                     
	DPOINT  w[6];           // Ctl pts for 5th-degree eqn
	double   t_candidate[6]; // Possible roots
	int      n_solutions;    // Number of roots found
	double   t;              // Parameter value of closest
	
	double  HitDist, Dist;
	DPOINT p, ptTest, found;
	int	i;
	double xscale, yscale;
	int xoff, yoff;
	DPOINT FBezier[4];
	
	// must scale and position bezier for algorithm to work!
	xoff = Bezier[0].x;
	xoff = Bezier[0].y;
	xscale= yscale = 1.0;
	for (i=0;i<BEZIER_IN;i++)
	{
		if (Bezier[i].x > 0)
			xscale = Max(xscale, Bezier[i].x);
		else
			xscale = Max(xscale, -Bezier[i].x);
		if (Bezier[i].y > 0)
			yscale = Max(yscale, Bezier[i].y);
		else
			yscale = Max(yscale, -Bezier[i].y);
	}
	for (i=0;i<4;i++)
	{
		FBezier[i].x = (double)(Bezier[i].x-xoff)/xscale;
		FBezier[i].y = (double)(Bezier[i].y-yoff)/yscale;
	}
	ptTest.x = (double)(lpTestPt->x-xoff)/xscale;
	ptTest.y = (double)(lpTestPt->y-yoff)/yscale;
	
	// Convert problem to 5th-degree Bezier form
	convert_to_5th_bezier_form(w, &ptTest, FBezier);
	
	/* Find all possible roots of 5th-degree equation */
	n_solutions = find_roots(w, t_candidate, 0);
	
	/* Compare distances of HitPt to all candidates, and to t=0, and t=1 */
	/* Check distance to beginning of curve, where t = 0	*/
	HitDist = DDistanceSq(&ptTest, &FBezier[0]);
	found = FBezier[0];
	t = 0.0;
	
	/* Find distances for candidate points	*/
	for (i = 0; i < n_solutions; i++)
	{
	   	para_bezier(FBezier, 3, t_candidate[i], &p);
		Dist = DDistanceSq(&ptTest, &p);
		if (Dist < HitDist)
		{
		  	HitDist = Dist;
			found = p;
		 	t = t_candidate[i];
		}
	}
	
	/* Finally, look at distance to end point, where t = 1.0 */
	Dist = DDistanceSq(&ptTest, &FBezier[3]);
	if (Dist < HitDist)
	{
	  	HitDist = Dist;
		found = FBezier[3];
	  	t = 1.0;
	}
	lpFoundPt->x = (int)(found.x*xscale) + xoff;
	lpFoundPt->y = (int)(found.y*yscale) + yoff;
	return t;
}


//=======================================================================
// convert_to_5th_bezier_form : based on DS code
//   Given a point and a Bezier curve, generate a 5th-degree
//   Bezier-format equation (w[6]) whose solution finds the point on the
//   curve (Bezier[4]) nearest to lpTest.
//=======================================================================
void LOCAL convert_to_5th_bezier_form(LPDPOINT w, LPDPOINT lpTest , LPDPOINT Bezier)
{
	int     i, j, k, m, n, ub, lb;
	int 	row, column;        // Table indices
	DPOINT c[4];      // V(i)'s - P
	DPOINT d[3];        // V(i+1) - V(i)
	double  cdTable[3][4];    // Dot product of c, d
	static double z[3][4] = { // Precomputed "z" for cubics
	  {1.0, 0.6, 0.3, 0.1},
	  {0.4, 0.6, 0.6, 0.4},
	  {0.1, 0.3, 0.6, 1.0},};
	
	// Determine the c's -- these are vectors created by subtracting
	// point P from each of the control points
	for (i = 0; i <= 3; i++)
	{
		c[i].x = Bezier[i].x - lpTest->x;
		c[i].y = Bezier[i].y - lpTest->y;
	}
	// Determine the d's -- these are vectors created by subtracting
	// each control point from the next
	for (i = 0; i <= 2; i++)
	{
		d[i].x = (Bezier[i+1].x - Bezier[i].x) * 3.0;
		d[i].y = (Bezier[i+1].y - Bezier[i].y) * 3.0;
	}
	
	// Create the c,d table -- this is a table of dot products of the
	// c's and d's
	for (row = 0; row <= 2; row++)
	{
		for (column = 0; column <= 3; column++)
		{
		  cdTable[row][column] = DDotProduct(&d[row], &c[column]);
		}
	}
	
	// Now, apply the z's to the dot products, on the skew diagonal
	// Also, set up the x-values, making these "points"
	for (i = 0; i <= 5; i++)
	{
		w[i].y = 0.0;
		w[i].x = (double)(i)/5;
	}
	
	n = 3;
	m = 2;
	for (k = 0; k <= n + m; k++)
	{
		lb = max(0, k - m);
		ub = min(k, n);
		for (i = lb; i <= ub; i++)
		{
			j = k - i;
			w[i+j].y += cdTable[j][i] * z[j][i];
		}
	}
}

//=======================================================================
// FindRoots :	based on DS code
//   Given a 5th-degree equation in Bernstein-Bezier form, find
//   all of the roots in the interval [0, 1].  Return the number
//   of roots found.
//=======================================================================
int LOCAL find_roots(LPDPOINT pW, double *pt, int depth)
{
	int 	i;
	DPOINT 	Left [6],      // New left and right
	        Right[6];      // control polygons
	int     left_count,    // Solution count from
	        right_count;   // children
	double  left_t [6],    // Solutions from kids
	        right_t[6];
	
	switch (crossing_count(pW))
	{
		case 0 :
			// No solutions here
		  	return 0;
		  	break;
		case 1 :
			// Unique solution
			// Stop recursion when the tree is deep enough
			// if deep enough, return 1 solution at midpoint
			if (depth >= MAXDEPTH)
			{
				pt[0] = (pW[0].x + pW[5].x)/2.0;
				return 1;
			}
			if (control_polygon_flat_enough(pW))
			{
			  	pt[0] = compute_X_intercept(pW);
			  	return 1;
			}
		break;
	}
	// Otherwise, solve recursively after
	// subdividing control polygon
	para_bezier(pW, 5, 0.5, NULL, Left, Right);
	left_count  = find_roots(Left, left_t,  depth+1);
	right_count = find_roots(Right,right_t, depth+1);
	
	// Gather solutions together
	for (i = 0; i < left_count; i++)
		pt[i] = left_t[i];
	for (i = 0; i < right_count; i++)
	  	pt[i+left_count] = right_t[i];
	// Send back total number of solutions
	return (left_count+right_count);
}


//=======================================================================
// CrossingCount : 	based on DS codE
// Count the number of times a Bezier control polygon
// crosses the 0-axis. This number is >= the number of roots.
//=======================================================================
int LOCAL crossing_count(LPDPOINT pQ)
//=======================================================================
{
	int i;
	int n_crossings = 0; //  Number of zero-crossings
	int sign, old_sign;  //  Sign of coefficients
	
	sign = old_sign = SGN(pQ[0].y);
	for (i = 1; i <= 5; i++)
	{
		sign = SGN(pQ[i].y);
		if (sign != old_sign)
		  	n_crossings++;
		old_sign = sign;
	}
	return n_crossings;
}


//=======================================================================
// control_polygon_flat_enough :	based on DS code
//   Check if the control polygon of a Bezier curve is flat enough
//   for recursive subdivision to bottom out.
//=======================================================================
int LOCAL control_polygon_flat_enough(LPDPOINT pQ)
//=======================================================================
{
	int     i;                  // Index variable
	double  distance[6];        // Distances from pts to line
	double  max_distance_above; // maximum of these
	double  max_distance_below;
	double  error;              // Precision of root
//	DPOINT  t;                  // Vector from pQ[0] to pQ[degree]
	double  intercept_1,
	        intercept_2,
	        left_intercept,
	        right_intercept;
	double  a, b, c;  // Coefficients of implicit
	                  // eqn for line from pQ[0]-pQ[deg]
	double	abSquared;
	
	/* Find the  perpendicular distance		*/
	/* from each interior control point to 	*/
	/* line connecting pQ[0] and pQ[5]	*/
	
	// Derive the implicit equation for line connecting first
	// and last control points
	a = pQ[0].y - pQ[5].y;
	b = pQ[5].x - pQ[0].x;
	c = pQ[0].x * pQ[5].y - pQ[5].x * pQ[0].y;
	
	abSquared = (a * a) + (b * b);
	
	for (i = 1; i < 5; i++)
	{
	  // Compute distance from each of the points to that line
	  distance[i] = a * pQ[i].x + b * pQ[i].y + c;
	  if (distance[i] > 0.0)
	  {
	    distance[i] = (distance[i] * distance[i]) / abSquared;
	  }
	  if (distance[i] < 0.0)
	  {
	    distance[i] = -((distance[i] * distance[i]) / abSquared);
	  }
	}
	
	// Find the largest distance
	max_distance_above = 0.0;
	max_distance_below = 0.0;
	
	for (i = 1; i < 5; i++)
	{
	  if (distance[i] < 0.0)
	  {
	    max_distance_below = min(max_distance_below, distance[i]);
	  }
	  if (distance[i] > 0.0) {
	  max_distance_above = max(max_distance_above, distance[i]);
	  }
	}
	
	{
	  double	det, dInv;
	  double	a1, b1, c1, a2, b2, c2;
	
	  /*  Implicit equation for zero line */
	  a1 = 0.0;
	  b1 = 1.0;
	  c1 = 0.0;
	
	  /*  Implicit equation for "above" line */
	  a2 = a;
	  b2 = b;
	  c2 = c + max_distance_above;
	
	  det = a1 * b2 - a2 * b1;
	  dInv = 1.0/det;
	
	  intercept_1 = (b1 * c2 - b2 * c1) * dInv;
	
	  /*  Implicit equation for "below" line */
	  a2 = a;
	  b2 = b;
	  c2 = c + max_distance_below;
	
	  det = a1 * b2 - a2 * b1;
	  dInv = 1.0/det;
	
	  intercept_2 = (b1 * c2 - b2 * c1) * dInv;
	}
	
	/* Compute intercepts of bounding box	*/
	left_intercept = min(intercept_1, intercept_2);
	right_intercept = max(intercept_1, intercept_2);
	
	error = 0.5 * (right_intercept-left_intercept);
	if (error < EPSILON)
	{
	  return 1;
	}
	else
	{
	  return 0;
	}
}


//=======================================================================
//  ComputeXIntercept :
//    Compute intersection of chord from first control point to last
//    with 0-axis.
//=======================================================================
double LOCAL compute_X_intercept(LPDPOINT pQ)
//=======================================================================
{							  
  double  XLK, YLK, XNM, YNM, XMK, YMK;
  double  det, detInv;
  double  S, T;
  double  X, Y;

  XLK = 1.0 - 0.0;
  YLK = 0.0 - 0.0;
  XNM = pQ[5].x - pQ[0].x;
  YNM = pQ[5].y - pQ[0].y;
  XMK = pQ[0].x;
  YMK = pQ[0].y;

  det = XNM*YLK - YNM*XLK;
  detInv = 1.0/det;

  S = (XNM*YMK - YNM*XMK) * detInv;
  T = (XLK*YMK - YLK*XMK) * detInv;

  X = 0.0 + XLK * S;
  Y = 0.0 + YLK * S;

  return X;
}

//=======================================================================
//  Bezier :	based on DS code
//    Evaluate a Bezier curve at a particular parameter value
//    Fill in control points for resulting sub-curves if "Left" and
//    "Right" are non-null.
//	int      degree: Control pts
//	double   t: Parameter value
//	LPPOINT lpResult: RETURN point at parameter
//	LPDPOINT lpLeft: RETURN left half ctl pts
//	LPDPOINT lpRight: RETURN right half ctl pts
//=======================================================================
void 	LOCAL  para_bezier(LPDPOINT FBezier, int degree, double t, LPDPOINT lpResult, LPDPOINT lpLeft, LPDPOINT lpRight)
//=======================================================================
{
	int  i, j;    // Index variables
	DPOINT Vtemp[6][6];
	double T;
	
	T = t;
	/* Copy control points	*/
	for (j =0; j <= degree; j++)
	  	Vtemp[0][j] = FBezier[j];
	
	/* Triangle computation	*/
	for (i = 1; i <= degree; i++)
		for (j =0 ; j <= degree - i; j++)
		{
      		Vtemp[i][j].x =
      		  (1.0 - t) * Vtemp[i-1][j].x + t * Vtemp[i-1][j+1].x;
      		Vtemp[i][j].y =
      		  (1.0 - t) * Vtemp[i-1][j].y + t * Vtemp[i-1][j+1].y;
		}
	
	if (lpLeft != NULL)
		for (j = 0; j <= degree; j++)
			lpLeft[j]  = Vtemp[j][0];
	if (lpRight != NULL)
	  for (j = 0; j <= degree; j++)
	    lpRight[j] = Vtemp[degree-j][j];
	if (lpResult)
	{
		lpResult->x = Vtemp[degree][0].x;
		lpResult->y = Vtemp[degree][0].y;
	}
}


//=======================================================================
//	lpLine should be 3 points defining the corner.
//	lpHandles1 & lpHandles2 (2 point arrays) will be the ideal handles
//	for lpLine[0] to lpLine[1] and lpLine[1] to lpLine[2] 
//=======================================================================
void ComputeBezierHandles(LPPOINT lpLine, LPPOINT lpHandles1, LPPOINT lpHandles2)
//=======================================================================
{
	if (PtEq(lpLine[0], lpLine[1]))
	{
		lpHandles1[0] = lpHandles1[1] = lpLine[0];
		if (PtEq(lpLine[1], lpLine[2]))
			lpHandles2[0] = lpHandles2[1] = lpLine[0];
		else
			SplitLine(&lpLine[1], &lpLine[2], &lpHandles2[0], &lpHandles2[1]);
		return;
	}		
	else if (PtEq(lpLine[1], lpLine[2]))
	{
		lpHandles2[0] = lpHandles2[1] = lpLine[0];
		SplitLine(&lpLine[0], &lpLine[1], &lpHandles1[0], &lpHandles1[1]);
		return;
	}
	// have ok lines
	// get the non corner handles
	SplitLine(&lpLine[0], &lpLine[1], &lpHandles1[0], &lpHandles1[1]);
	SplitLine(&lpLine[1], &lpLine[2], &lpHandles2[0], &lpHandles2[1]);
	// compute the corner to curve
	lpHandles1[1].x = (4*lpLine[1].x+lpLine[0].x-lpLine[2].x)/4;
	lpHandles1[1].y = (4*lpLine[1].y+lpLine[0].y-lpLine[2].y)/4;
	lpHandles2[0].x = (4*lpLine[1].x+lpLine[2].x-lpLine[0].x)/4;
	lpHandles2[0].y = (4*lpLine[1].y+lpLine[2].y-lpLine[0].y)/4;
}		



//=======================================================================
//	Splits the segment (start,end) into 3rds.  out1 = 1/3, out2=2/3
//=======================================================================
void SplitLine(LPPOINT start, LPPOINT end, LPPOINT out1, LPPOINT out2)
//=======================================================================
{
	out1->x = start->x + (end->x - start->x)/3;
	out1->y = start->y + (end->y - start->y)/3;
	out2->x = start->x + 2*(end->x - start->x)/3;
	out2->y = start->y + 2*(end->y - start->y)/3;
}
