/*
// $Header: /PM8/App/fill.cpp 1     3/03/99 6:05p Gbeddow $
//
// $Workfile: fill.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/fill.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 12    6/18/98 6:11p Rlovejoy
// Fix for clip-offset bug. (Leaves 1-pixel residue behind).
// 
// 11    6/03/98 4:12p Rlovejoy
// Code to ensure minimum bitmap.
// 
// 10    5/14/98 10:52a Rlovejoy
// Revision; Apply x,y flips to gradient fills.
// 
// 9     5/12/98 11:34a Rlovejoy
// Added radial fills.
// 
// 8     5/08/98 2:15p Rlovejoy
// Use doubles to avoid the plague of overflow.
// 
// 7     5/08/98 1:40p Rlovejoy
// Added STEP_PRECISION define.
// 
// 6     5/08/98 12:02p Rlovejoy
// Added rotation.
// 
// 5     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 4     4/29/98 6:30p Rlovejoy
// Use intersecting clip region.
// 
// 3     4/29/98 5:59p Rlovejoy
// Sweep gradients & initial revisions.
// 
// 2     4/28/98 10:45a Rlovejoy
// Initial creation.
*/

#include "stdafx.h"

#include <math.h>
#include "fill.h"
#include "polypoly.h"

/////////////////////////////////////////////////////////////////////////////
// CFill class

CFill::CFill()
{
	m_fFillColorExists = FALSE;
	m_dRotation = 0.;
}

CFill::~CFill()
{

}

// Brush properties
CDibPatternBrush* CFill::GetDIBBrush(CPolyPolygon* pPoly)
{
	return pPoly->Brush();
}

// Operations required by the fill
#define INTERSECT 1
#define NO_INTERSECT 0
#define CO_LINEAR -1

#define SAME_SIGN( a, b ) (((long) ((unsigned long) a^(unsigned long)b)) >= 0 )

void CFill::GenerateRotatedFillBounds(CPoint ptBounds[], ANGLE dRotation, CPoint ptFillBounds[])
{
	// Determine an enclosing 4-point polygon.
	// Do this by generating a line which runs through a point in 'ptBounds' and
	// makes an angle of 'dRotation' with the y-axis. If this line intersects
	// any of the lines formed by connecting 'ptBounds', iterate to the next 
	// point in 'ptBounds'. If not, save the line. Continue until you have 4
	// "inifinitely" long lines which enclose the bounds. The intersections of
	// these lines give the resultant bounds.

	// We assume that the points in 'ptBounds' begin with the top-most, left-most
	// point at index 0, and continue counter-clockwise around the bound.

	int i, j, k, check;

	// Center bounds around the origin to avoid calculation overflows
	CPoint ptOriginBounds[4];
	CPoint ptCenter;
	FindCenterPoint(ptBounds, 4, ptCenter);
	
	// Move points to origin
	for (i = 0; i < 4; i++)
	{
		ptOriginBounds[i] = ptBounds[i] - ptCenter;
	}

	// Compute an extend distance
	int nExtend;
	int nWidth = max(abs(ptBounds[2].x - ptBounds[0].x), abs(ptBounds[3].x - ptBounds[1].x));
	int nHeight = max(abs(ptBounds[2].y - ptBounds[0].y), abs(ptBounds[3].y - ptBounds[1].y));
	nExtend = max(nWidth, nHeight) * 2;

	// Determine where to start
	ANGLE dPositiveRot = dRotation;
	if (dPositiveRot < 0.)
	{
		dPositiveRot = PI2 + dPositiveRot;
	}
	while (dPositiveRot > PI2)
	{
		dPositiveRot -= PI2;
	}
	if (dPositiveRot < PI / 2)
	{
		check = 0;
	}
	else if (dPositiveRot < PI)
	{
		check = 1;
	}
	else if (dPositiveRot < 3*PI/2)
	{
		check = 2;
	}
	else
	{
		check = 3;
	}

	CPoint ptLine[2], ptLastLine[2], ptFirstLine[2];
	for (i = 0; i < 4; i++)
	{
		// Build a line and check for intersections
		j = check;
		k = 0;
		do 
		{
			// Nudge the center point and generate the line
			CPoint ptNudged = ptOriginBounds[j];
			if (j == 0 || j == 2)
			{
				ptNudged.x += ((j == 0) ? -2 : 2);
			}
			else
			{
				ptNudged.y += ((j == 3) ? -2 : 2);
			}
			ConstructThroughLine(ptNudged, dRotation, (i & 1), nExtend, ptLine);

			if (IntersectLines(NULL, &ptLine[0], &ptLine[1], &ptOriginBounds[j], &ptOriginBounds[(j+1 > 3) ? 0 : j+1]) != INTERSECT &&
				 IntersectLines(NULL, &ptLine[0], &ptLine[1], &ptOriginBounds[j], &ptOriginBounds[(j-1 < 0) ? 3 : j-1]) != INTERSECT)
			{
				break;
			}
			if (++j == 4) j = 0;
		}
		while (++k < 4);
		if ((check = j+1) == 4) check = 0;

		// Assuming we have a good line, find another end point
		if (i)
		{
			IntersectLines(&ptFillBounds[4-i], &ptLine[0], &ptLine[1], &ptLastLine[0], &ptLastLine[1]);
		}
		else
		{
			ptFirstLine[0] = ptLine[0];
			ptFirstLine[1] = ptLine[1];
		}
		ptLastLine[0] = ptLine[0];
		ptLastLine[1] = ptLine[1];
	}

	// Get last point
	IntersectLines(&ptFillBounds[0], &ptFirstLine[0], &ptFirstLine[1], &ptLastLine[0], &ptLastLine[1]);

	// Move points back
	for (i = 0; i < 4; i++)
	{
		ptFillBounds[i] += ptCenter;
	}

}

void CFill::ConstructThroughLine(CPoint& ptThrough, ANGLE dRotation, BOOL fHorizontal, int nExtend, CPoint ptLine[])
{
	// Create initial horizontal or vertical line
	CPoint ptFlatLine[2];
	ptFlatLine[0] = ptThrough;
	ptFlatLine[1] = ptThrough;
	if (fHorizontal)
	{
		ptFlatLine[0].x -= nExtend;
		ptFlatLine[1].x += nExtend;
	}
	else
	{
		ptFlatLine[0].y += nExtend;
		ptFlatLine[1].y -= nExtend;
	}
	
	// Rotate the end-points
	RotateLine(ptFlatLine, ptThrough, dRotation, ptLine);
}

void CFill::FindCenterPoint(CPoint ptBounds[], int nPoints, CPoint& ptCenter)
{
	// Find center point
	ptCenter.x = 0;
	ptCenter.y = 0;
	for (int i = 0; i < nPoints; i++)
	{
		ptCenter += ptBounds[i];
	}
	ptCenter.x /= nPoints;
	ptCenter.y /= nPoints;
}

void CFill::RotateLine(CPoint ptLine[], CPoint ptCenter, ANGLE dRotation, CPoint ptRotatedLine[])
{
	RotatePoints(ptLine, ptCenter, 2, dRotation, ptRotatedLine);
}

void CFill::RotatePoints(CPoint ptPoints[], CPoint ptCenter, int nPoints, ANGLE dRotation, CPoint ptRotatedPoints[])
{
	// Compute sin & cosine of the given angle
	double dSin, dCos;
	dSin = sin(dRotation);
	dCos = cos(dRotation);
	
	// Rotate the points
	for (int i = 0; i < nPoints; i++)
	{
		// Make point relative to origin
		CPoint ptRelative = ptPoints[i] - ptCenter;
		CPoint ptRotated;

		// Rotate point
		ptRotated.x = (LONG)(ptRelative.x*dCos + ptRelative.y*dSin);
		ptRotated.y = (LONG)(-ptRelative.x*dSin + ptRelative.y*dCos);

		// Bring it on home
		ptRotatedPoints[i] = ptCenter + ptRotated;
	}
}

int CFill::IntersectLines(CPoint* ptIntersect, CPoint* p1a, CPoint* p1b, CPoint* p2a, CPoint* p2b)
{
	// Use Graphics Gems variable names
	long Ax, Bx, Cx, Ay, By, Cy;
	long x1lo, x1hi, y1lo, y1hi;
	long d, e, f;
	double num, offset;

	// Bounding box tests
	Ax = p1b->x - p1a->x;
	Bx = p2a->x - p2b->x;

	if (Ax < 0) {
		x1lo = p1b->x;
		x1hi = p1a->x;
	}
	else {
		x1lo = p1a->x;
		x1hi = p1b->x;
	}
	if (Bx > 0) {
		if (x1hi < p2b->x || p2a->x < x1lo)
			return NO_INTERSECT;
	}
	else {
		if (x1hi < p2a->x || p2b->x < x1lo)
			return NO_INTERSECT;
	}
	
	Ay = p1b->y - p1a->y;
	By = p2a->y - p2b->y;

	if (Ay < 0) {
		y1lo = p1b->y;
		y1hi = p1a->y;
	}
	else {
		y1lo = p1a->y;
		y1hi = p1b->y;
	}
	if (By > 0) {
		if (y1hi < p2b->y || p2a->y < y1lo)
			return NO_INTERSECT;
	}
	else {							
		if (y1hi < p2a->y || p2b->y < y1lo)
			return NO_INTERSECT;
	}
					
	// Possible intersection
	Cx = p1a->x - p2a->x;
	Cy = p1a->y - p2a->y;
	
	// May get overflow here-- be careful
	d = By*Cx - Bx*Cy;
	f = Ay*Bx - Ax*By;				

	// alpha tests
	if (f > 0) {
		if (d < 0 || d > f)
			return NO_INTERSECT;
	}
	else {
		if (d > 0 || d < f)
			return NO_INTERSECT;
	}

	e = Ax*Cy - Ay*Cx;

	// beta tests
	if (f > 0) {
		if (e < 0 || e > f)
			return NO_INTERSECT;
	}
	else {
		if (e > 0 || e < f)
			return NO_INTERSECT;
	}
	
	// We have an intersection
	if (f == 0)
		return CO_LINEAR;

	// Compute intersection (watch out for overflow)
	if (ptIntersect != NULL) 
	{
		num = (double)d*(double)Ax;
		offset = SAME_SIGN(num, f) ? f>>1 : -f>>1;
		ptIntersect->x = (long)(p1a->x + (num+offset) / f);

		num = (double)d*(double)Ay;
		offset = SAME_SIGN(num, f) ? f>>1 : -f>>1;
		ptIntersect->y = (long)(p1a->y + (num+offset) / f);
	}
		
	return INTERSECT;	
}

/////////////////////////////////////////////////////////////////////////////
// CNullFill class
	
CNullFill::CNullFill() : CFill()
{

}

CNullFill::~CNullFill()
{

}

/////////////////////////////////////////////////////////////////////////////
// CFlatFill class
	
CFlatFill::CFlatFill()
{
	m_nFillType = FillFormatV2::FillSolid;
}

CFlatFill::~CFlatFill()
{

}

void CFlatFill::DoFill(CDC* pDC, CPolyPolygon* pPoly)
{
	// Select pen & brush
	pDC->SelectStockObject(NULL_PEN);
	CDibPatternBrush* pBrush = CFill::GetDIBBrush(pPoly);
	CBrush *pOldBrush = NULL;
	if (pBrush->Color(m_clFillColor))
	{
		pOldBrush = pDC->SelectObject(pBrush->Brush());
	}

	if (pPoly->Closed())
	{
		pDC->PolyPolygon(pPoly->Points(), pPoly->Counts(), pPoly->ContoursInPath());
	}
	else
	{
		ASSERT(pPoly->ContoursInPath() == 1);
		pDC->Polygon(pPoly->Points(), *pPoly->Counts());
	}

	// Restore brush
	if (pOldBrush != NULL)
	{
		pDC->SelectObject(pOldBrush);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGradientFill class
	
CGradientFill::CGradientFill() : CFill()
{
	m_fBlendColorExists = FALSE;
}

CGradientFill::~CGradientFill()
{

}

BOOL CGradientFill::SelectClipRegion(CDC* pDC, CPolyPolygon* pPoly)
{
   CRgn rgnClip;

	if (pPoly->Closed())
	{
		rgnClip.CreatePolyPolygonRgn(pPoly->Points(), pPoly->Counts(), pPoly->ContoursInPath(), ALTERNATE);
	}
	else
	{
		ASSERT(pPoly->ContoursInPath() == 1);
		rgnClip.CreatePolygonRgn(pPoly->Points(), *pPoly->Counts(), ALTERNATE);
	}

	// Select the clip region
	m_nContext = pDC->SaveDC();
	int nReturn = pDC->SelectClipRgn(&rgnClip, RGN_AND);
	if (nReturn == NULLREGION || nReturn == ERROR)
	{
		pDC->RestoreDC(-1);
		return FALSE;
	}
	return TRUE;
}

void CGradientFill::RestoreClipRegion(CDC* pDC)
{	
	// Restore the context
   if (m_nContext != 0)
   {
		/* Always RestoreDC() with -1 to avoid problems in metafiles. */
      pDC->RestoreDC(-1 /*nContext*/);
   }
}

void CGradientFill::GetColorIncrements(long nSpan, DWORD& dwR, DWORD& dwG, DWORD& dwB, long& dwRStep, long& dwGStep, long& dwBStep)
{	
	dwR = GetRValue(m_clFillColor) << FRAC_BITS;
	dwG = GetGValue(m_clFillColor) << FRAC_BITS;
	dwB = GetBValue(m_clFillColor) << FRAC_BITS;
	
	dwRStep = ((long)((GetRValue(m_clBlendColor) << FRAC_BITS) - dwR)) / nSpan;
	dwGStep = ((long)((GetGValue(m_clBlendColor) << FRAC_BITS) - dwG)) / nSpan;
	dwBStep = ((long)((GetBValue(m_clBlendColor) << FRAC_BITS) - dwB)) / nSpan;
}

/////////////////////////////////////////////////////////////////////////////
// CSweepGradientFill class

CSweepGradientFill::CSweepGradientFill()
{
}

CSweepGradientFill::~CSweepGradientFill()
{

}

void CSweepGradientFill::SweepFill(CDC* pDC, CDibPatternBrush* pBrush, CPoint ptBounds[], ANGLE dRotation)
{
	int i;

	// Grab a DC to draw filled shape into
	PBOX boxEnclose;
	compute_enclosing_bound((PPNT_PTR)ptBounds, 4, &boxEnclose);
	CRect rctEnclose((LPCRECT)&boxEnclose);
	CBitBlt bitBlitter(pDC, rctEnclose);
	CDC* pDestDC = bitBlitter.GetMemDC();
	if (pDestDC == NULL)
		return;

	// Set up DC
	pDestDC->SetPolyFillMode(ALTERNATE);
	pDestDC->SelectStockObject(NULL_PEN);

	// Rotate the bounds
	CPoint ptFillBounds[4];
	GenerateRotatedFillBounds(ptBounds, dRotation, ptFillBounds);

	// Move points into DestDC space
	CPoint ptDestTopLeft = bitBlitter.GetTopLeft();
	int nXMove = (rctEnclose.left - ptDestTopLeft.x);
	int nYMove = (rctEnclose.top - ptDestTopLeft.y);
	for (i = 0; i < 4; i++)
	{
		ptFillBounds[i].x -= nXMove;
		ptFillBounds[i].y -= nYMove;
	}

	// Compute step polygon increments
	// Fixed format for DWORDS: (16.16), (whole, fractional)
	CPoint ptStep;
	CPoint ptSpan = ptFillBounds[1] - ptFillBounds[0];
	long nSpan = (long)sqrt((ptSpan.x * ptSpan.x) + (ptSpan.y * ptSpan.y)) / STEP_PRECISION;
	ptStep.x = ((ptFillBounds[1].x - ptFillBounds[0].x) << FRAC_BITS) / nSpan;
	ptStep.y = ((ptFillBounds[1].y - ptFillBounds[0].y) << FRAC_BITS) / nSpan;
	ptStep.x *= STEP_PRECISION;
	ptStep.y *= STEP_PRECISION;

	// Set initial color bar
	CPoint ptBar[] = {ptFillBounds[0], ptFillBounds[0], ptFillBounds[3], ptFillBounds[3]};
	CPoint ptFraction(0,0);

	// Setup color increments
	DWORD dwR, dwG, dwB;
	long dwRStep, dwGStep, dwBStep;
	GetColorIncrements(nSpan, dwR, dwG, dwB, dwRStep, dwGStep, dwBStep);
	dwRStep *= STEP_PRECISION;
	dwGStep *= STEP_PRECISION;
	dwBStep *= STEP_PRECISION;
	
	// Set initial color primes
	DWORD dwRp = dwR;
	DWORD dwGp = dwG;
	DWORD dwBp = dwB;

	// Draw bars at each increment
	for (i = 0; i < nSpan; i++)
	{
		// Bump up colors
		dwRp += dwRStep;
		dwGp += dwGStep;
		dwBp += dwBStep;

		// Increment fractional step
		ptFraction += ptStep;

		// Bump up the bar
		long nXStep = ptFraction.x >> FRAC_BITS;
		long nYStep = ptFraction.y >> FRAC_BITS;
		ptBar[1].x += nXStep;
		ptBar[1].y += nYStep;
		ptBar[2].x += nXStep;
		ptBar[2].y += nYStep;

		// Strip off whole...
		ptFraction.x &= FRAC_MASK;
		ptFraction.y &= FRAC_MASK;

		// If color has changed a full increment, draw the bar
		if ((dwRp >> (FRAC_BITS+0)) != (dwR >> (FRAC_BITS+0)) ||
			 (dwGp >> (FRAC_BITS+0)) != (dwG >> (FRAC_BITS+0)) ||
			 (dwBp >> (FRAC_BITS+0)) != (dwB >> (FRAC_BITS+0)))
		{			
			// Draw the filled bar
			SetBrushColor(pBrush, dwR, dwG, dwB);
			CBrush *pOldBrush = pDestDC->SelectObject(pBrush->Brush());
			pDestDC->Polygon(ptBar, 4);
			pDestDC->SelectObject(pOldBrush);

			// Set for next cycle
			dwR = dwRp;
			dwG = dwGp;
			dwB = dwBp;

			// Inchworm the bar
			ptBar[0] = ptBar[1];
			ptBar[3] = ptBar[2];
		}
   }

	// Draw final bar
	SetBrushColor(pBrush, dwR, dwG, dwB);
	CBrush *pOldBrush = pDestDC->SelectObject(pBrush->Brush());
	ptBar[1] = ptFillBounds[1];
	ptBar[2] = ptFillBounds[2];
	pDestDC->Polygon(ptBar, 4);
	pDestDC->SelectObject(pOldBrush);

	// Blast to screen
	bitBlitter.CopyToScreen(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CSweepGradientFillH class

CSweepGradientFillH::CSweepGradientFillH()
{
	m_dRotation = 0.;
	m_dShapeRotation = 0.;
	m_nFillType = FillFormatV2::FillSweepRight;
}

CSweepGradientFillH::~CSweepGradientFillH()
{

}

void CSweepGradientFillH::SetRotations(ANGLE dRotation, ANGLE dShapeRotation, BOOL fXFlip /*= FALSE*/, BOOL fYFlip /*= FALSE*/)
{
	CGradientFill::SetRotations(dRotation, dShapeRotation);
	if (fXFlip)
	{
		m_dRotation += PI;
	}
}

void CSweepGradientFillH::DoFill(CDC* pDC, CPolyPolygon* pPoly)
{
	// Set up draw bound
	CPoint ptBounds[4];
	pPoly->GetBoundingBox(ptBounds, m_dShapeRotation);

	// Do the sweep
	SweepFill(
		pDC,
		CFill::GetDIBBrush(pPoly),
		ptBounds,
		m_dRotation);
}   

/////////////////////////////////////////////////////////////////////////////
// CSweepGradientFillV class

CSweepGradientFillV::CSweepGradientFillV()
{
	m_dRotation = 0.;
	m_dShapeRotation = 0;
	m_nFillType = FillFormatV2::FillSweepDown;
}

CSweepGradientFillV::~CSweepGradientFillV()
{

}

void CSweepGradientFillV::SetRotations(ANGLE dRotation, ANGLE dShapeRotation, BOOL fXFlip /*= FALSE*/, BOOL fYFlip /*= FALSE*/)
{
	CGradientFill::SetRotations(dRotation, dShapeRotation);
	if (fYFlip)
	{
		m_dRotation += PI;
	}
}

void CSweepGradientFillV::DoFill(CDC* pDC, CPolyPolygon* pPoly)
{
	// Set up draw bound
	CPoint ptBounds[4];
	pPoly->GetBoundingBox(ptBounds, m_dShapeRotation);

	// Do the sweep
	SweepFill(
		pDC,
		CFill::GetDIBBrush(pPoly),
		ptBounds,
		m_dRotation-PI/2);
}

/////////////////////////////////////////////////////////////////////////////
// CRadialGradientFill class

BOOL CRadialGradientFill::m_fTrigComputed = FALSE;

CRadialGradientFill::CRadialGradientFill()
{

}


CRadialGradientFill::~CRadialGradientFill()
{

}

void CRadialGradientFill::ComputePolygonEllipse(CPoint ptCenter, int nWidth, int nHeight, ANGLE dRotation, CPoint ptRing[])
{
	ANGLE dAngleIncr = PI2 / N_RING_POINTS;
	ANGLE dAngle;
	int i;
	int nLength = max(nWidth, nHeight);
	static double dSin[N_RING_POINTS];
	static double dCos[N_RING_POINTS];

	for (i = 0, dAngle = 0.; i < N_RING_POINTS; i++, dAngle += dAngleIncr)
	{
		if (!m_fTrigComputed)
		{
			dSin[i] = sin(dAngle);
			dCos[i] = cos(dAngle);
		}
		ptRing[i].x = (long)(ptCenter.x + nLength*dCos[i]);
		ptRing[i].y = (long)(ptCenter.y + nLength*dSin[i]);

		// Bring the point onto the ellipse
		if (nWidth > nHeight)
		{
			ptRing[i].y -= (long)((nWidth - nHeight)*dSin[i]);
		}
		else
		{
			ptRing[i].x -= (long)((nHeight - nWidth)*dCos[i]);
		}

	}
	m_fTrigComputed = TRUE;

	// Now rotate the points
	if (dRotation != 0.)
	{
		RotatePoints(ptRing, ptCenter, N_RING_POINTS, dRotation, ptRing);
	}

}

void CRadialGradientFill::RadialFill(CDC* pDC, CDibPatternBrush* pBrush, CPoint ptCenter, long nWidthRadius, long nHeightRadius, ANGLE dRotation)
{
	// Grab a DC to draw filled shape into
	long nRadius = max(nWidthRadius, nHeightRadius);
	CBitBlt bitBlitter(pDC, nRadius*2, nRadius*2, ptCenter.x-nRadius, ptCenter.y-nRadius);
	CDC* pDestDC = bitBlitter.GetMemDC();
	if (pDestDC == NULL)
		return;
	CPoint ptDestCenter = bitBlitter.GetCenter();

	// Set up DC
	pDestDC->SetPolyFillMode(ALTERNATE);
	pDestDC->SelectStockObject(NULL_PEN);

	// Sizing
	long nRingWidthRadius = nWidthRadius;
	long nRingHeightRadius = nHeightRadius;
	long nStepRadius = nRadius / STEP_PRECISION;

	// Setup color increments
	DWORD dwR, dwG, dwB;
	long dwRStep, dwGStep, dwBStep;
	GetColorIncrements(nStepRadius, dwR, dwG, dwB, dwRStep, dwGStep, dwBStep);

	// Set initial color primes
	DWORD dwRp = dwR;
	DWORD dwGp = dwG;
	DWORD dwBp = dwB;

	// Setup ring points
	long nRingWidthStep, nRingHeightStep;
	int nPolyCounts[] = {N_RING_POINTS, N_RING_POINTS};
	CPoint* ptRing = new CPoint[N_RING_POINTS*2];
	ComputePolygonEllipse(ptDestCenter, nRingWidthRadius, nRingHeightRadius, dRotation, ptRing);
	
	// Compute ring steps
	if (nRingWidthRadius > nRingHeightRadius)
	{
		nRingWidthStep = STEP_PRECISION << FRAC_BITS;
		nRingHeightStep = ((nRingHeightRadius * STEP_PRECISION) << FRAC_BITS) / nRingWidthRadius;
	}
	else
	{
		nRingWidthStep = ((nRingWidthRadius * STEP_PRECISION) << FRAC_BITS) / nRingHeightRadius;
		nRingHeightStep = STEP_PRECISION << FRAC_BITS;
	}
	nRingWidthRadius <<= FRAC_BITS;
	nRingHeightRadius <<= FRAC_BITS;

	// Draw circles at each increment
	for (int i = 0; i < nStepRadius; i++)
	{
		// Bump up colors
		dwRp += dwRStep;
		dwGp += dwGStep;
		dwBp += dwBStep;

		// If color has changed a full increment, draw the bar
		nRingWidthRadius -= nRingWidthStep;
		nRingHeightRadius -= nRingHeightStep;
		if ((dwRp >> (FRAC_BITS+0)) != (dwR >> (FRAC_BITS+0)) ||
			 (dwGp >> (FRAC_BITS+0)) != (dwG >> (FRAC_BITS+0)) ||
			 (dwBp >> (FRAC_BITS+0)) != (dwB >> (FRAC_BITS+0)))
		{
			// Compute inner ring
			ComputePolygonEllipse(ptDestCenter, nRingWidthRadius >> FRAC_BITS, nRingHeightRadius >> FRAC_BITS, dRotation, &ptRing[(i & 1) ? 0 : N_RING_POINTS]);
			
			// Draw the polygon
			SetBrushColor(pBrush, dwR, dwG, dwB);
			CBrush *pOldBrush = pDestDC->SelectObject(pBrush->Brush());
			pDestDC->PolyPolygon(ptRing, nPolyCounts, 2);
			pDestDC->SelectObject(pOldBrush);

			// Set for next cycle
			dwR = dwRp;
			dwG = dwGp;
			dwB = dwBp;
		}
   }

	// Draw final circle
	SetBrushColor(pBrush, dwR, dwG, dwB);
	CBrush *pOldBrush = pDestDC->SelectObject(pBrush->Brush());
	pDestDC->Polygon(ptRing, N_RING_POINTS);
	pDestDC->SelectObject(pOldBrush);

	delete [] ptRing;

	// Blast to screen
	bitBlitter.CopyToScreen(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CRadialGradientFillCenter class

CRadialGradientFillCenter::CRadialGradientFillCenter()
{
	m_dRotation = 0.;
	m_dShapeRotation = 0;
	m_nFillType = FillFormatV2::FillRadialCenter;
}


CRadialGradientFillCenter::~CRadialGradientFillCenter()
{

}

void CRadialGradientFillCenter::DoFill(CDC* pDC, CPolyPolygon* pPoly)
{
	// Compute enclosing bound & get center
	CPoint ptBounds[4];
	long nWidth, nHeight;
	pPoly->GetBoundingBox(ptBounds, m_dShapeRotation, nWidth, nHeight);
	CPoint ptCenter;
	FindCenterPoint(ptBounds, 4, ptCenter);	

	// Do the fill (gradient rotation is always equal to shape rotation)
	int nWidthRadius = nWidth / 2;
	int nHeightRadius = nHeight / 2;
	long nRadius = (long)(sqrt((nWidthRadius * nWidthRadius) + (nHeightRadius * nHeightRadius)) * 1.05);

	// Pass radius as the biggest dimension; compute the other relative
	if (nWidth > nHeight)
	{
		nHeight = nRadius; //nHeightRadius * nRadius / nWidthRadius;
		nWidth = nRadius * nWidthRadius / nHeightRadius;
	}
	else
	{
		nWidth = nRadius;//nWidthRadius * nRadius / nHeightRadius;
		nHeight = nRadius * nHeightRadius / nWidthRadius;
	}
	RadialFill(pDC, CFill::GetDIBBrush(pPoly), ptCenter, nWidth, nHeight, m_dShapeRotation);
}

/////////////////////////////////////////////////////////////////////////////
// CRadialGradientFillCorner class

CRadialGradientFillCorner::CRadialGradientFillCorner()
{
	m_dRotation = 0;
	m_dShapeRotation = 0;
	m_fXFlip = m_fYFlip = FALSE;
	m_nFillType = FillFormatV2::FillRadialCorner;
}


CRadialGradientFillCorner::~CRadialGradientFillCorner()
{

}

void CRadialGradientFillCorner::SetRotations(ANGLE dRotation, ANGLE dShapeRotation, BOOL fXFlip /*= FALSE*/, BOOL fYFlip /*= FALSE*/)
{
	m_dShapeRotation = dShapeRotation;

	// Keep flip info
	m_fXFlip = fXFlip;
	m_fYFlip = fYFlip;
}

void CRadialGradientFillCorner::DoFill(CDC* pDC, CPolyPolygon* pPoly)
{
	// Compute enclosing bound
	CPoint ptBounds[4];
	int nCorner;
	pPoly->GetBoundingBox(ptBounds, m_dShapeRotation, nCorner);

	// If object is flipped, find correct corner
	if (m_fXFlip)
	{
		if (--nCorner < 0)
		{
			nCorner = 3;
		}
	}
	if (m_fYFlip)
	{
		if (++nCorner == 4)
		{
			nCorner = 0;
		}
	}
	if (m_fXFlip && m_fYFlip)
	{
		nCorner = (nCorner+2) & 3;
	}

	// Compute a line through corner point, then rotate
	CPoint ptThroughLine[2];
	FindCenterPoint(ptBounds, 4, ptThroughLine[0]);	// ptThroughLine[0] is the center point
	ptThroughLine[1] = ptBounds[nCorner];
	CPoint ptSpan = ptThroughLine[1] - ptThroughLine[0];
	ptThroughLine[1] += ptSpan;							// arbitrary extend
	RotateLine(ptThroughLine, ptThroughLine[0], m_dRotation, ptThroughLine);

	// Intersection of this line and bounds is "corner" point
	// Need to extend
	CPoint ptCorner;
	if (IntersectLines(&ptCorner, &ptThroughLine[0], &ptThroughLine[1], &ptBounds[0], &ptBounds[1]) == INTERSECT ||
		 IntersectLines(&ptCorner, &ptThroughLine[0], &ptThroughLine[1], &ptBounds[1], &ptBounds[2]) == INTERSECT ||
		 IntersectLines(&ptCorner, &ptThroughLine[0], &ptThroughLine[1], &ptBounds[2], &ptBounds[3]) == INTERSECT ||
		 IntersectLines(&ptCorner, &ptThroughLine[0], &ptThroughLine[1], &ptBounds[3], &ptBounds[0]) == INTERSECT)
	{
		// Compute "radius"- distance from center to original bound point
		long nRadius = (long)(sqrt((ptSpan.x * ptSpan.x) + (ptSpan.y * ptSpan.y)) * 2.05);

		// Do the fill
		RadialFill(pDC, CFill::GetDIBBrush(pPoly), ptCorner, nRadius, nRadius, 0.);
	}
}

/////////////////////////////////////////////////////////////////////////////

CFill* DeriveFillType(int nType)
{
	CFill* pFillObject;

	// Create an object of the specified type
	switch (nType)
	{
	case FillFormatV2::FillNone:
		pFillObject = new CNullFill;	// to ensure we return something
		break;
	case FillFormatV2::FillSolid:
		pFillObject = new CFlatFill;
		break;
	case FillFormatV2::FillSweepRight:
		pFillObject = new CSweepGradientFillH;
		break;
	case FillFormatV2::FillSweepDown:
		pFillObject = new CSweepGradientFillV;
		break;
	case FillFormatV2::FillRadialCenter:
		pFillObject = new CRadialGradientFillCenter;
		break;
	case FillFormatV2::FillRadialCorner:
		pFillObject = new CRadialGradientFillCorner;
		break;
	default:
		pFillObject = NULL;
		break;
	}

	return pFillObject;
}

/////////////////////////////////////////////////////////////////////////////
// CBitBlt class

CBitBlt::CBitBlt(CDC* pDC, int nWidth, int nHeight, int nTopLeftX, int nTopLeftY) :
	m_rctImgBound(CPoint(nTopLeftX, nTopLeftY), CSize(nWidth, nHeight))
{
	CreateMembers(pDC, nWidth, nHeight);
}

CBitBlt::CBitBlt(CDC* pDC, CRect& rctEnclose) :
	m_rctImgBound(rctEnclose)
{
	CreateMembers(pDC, rctEnclose.Width(), rctEnclose.Height());
}

void CBitBlt::CreateMembers(CDC* pDC, int nWidth, int nHeight)
{
	// Create the memDC
	m_MemDC.CreateCompatibleDC(pDC);
	m_MemDC.SelectPalette(pDC->GetCurrentPalette(), FALSE);
	m_MemDC.RealizePalette();

	// Dimensions of bitmap should not exceed clip box
	CRect rctClip;
	pDC->GetClipBox(rctClip);
	int nBMWidth = (nWidth > rctClip.Width()) ? rctClip.Width() : nWidth;
	int nBMHeight = (nHeight > rctClip.Height()) ? rctClip.Height() : nHeight;
	m_ptTopLeft = m_rctImgBound.TopLeft() - rctClip.TopLeft();
	
	// Compute center point now, before we mangle the bounds
	m_ptCenter.x = nBMWidth / 2 - (rctClip.CenterPoint().x - m_rctImgBound.CenterPoint().x);
	m_ptCenter.y = nBMHeight / 2 - (rctClip.CenterPoint().y - m_rctImgBound.CenterPoint().y);
	m_rctImgBound = rctClip;

	m_fBadCreation = (!m_Bitmap.CreateCompatibleBitmap(pDC, nBMWidth, nBMHeight));
		 
	m_pOldBitmap = m_MemDC.SelectObject(&m_Bitmap);
}

CBitBlt::~CBitBlt()
{
	// Delete the bitmap
	m_MemDC.SelectObject(m_pOldBitmap);
	m_Bitmap.DeleteObject();
}

CDC* CBitBlt::GetMemDC()
{
	return (m_fBadCreation) ? NULL : &m_MemDC;
}

CPoint CBitBlt::GetCenter()
{
	return m_ptCenter;
}

CPoint CBitBlt::GetTopLeft()
{
	return m_ptTopLeft;
}

void CBitBlt::CopyFromScreen(CDC* pDC)
{
	if (m_pOldBitmap != NULL)
	{
		// Copy screen contents to bitmap
		m_MemDC.BitBlt(
			0, 0,
			m_rctImgBound.Width(),
			m_rctImgBound.Height(),
			pDC, 
			m_rctImgBound.left,
			m_rctImgBound.top,
			SRCCOPY);
	}
}

void CBitBlt::CopyToScreen(CDC* pDC)
{		
	pDC->BitBlt(
		m_rctImgBound.left,
		m_rctImgBound.top,
		m_rctImgBound.Width(),
		m_rctImgBound.Height(),
		&m_MemDC, 
		0, 0,
		SRCCOPY);
}



