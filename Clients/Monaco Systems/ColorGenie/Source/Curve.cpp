#include "stdafx.h"
#include "Spline.h"
#include "curve.h"
#include <math.h>
#include <float.h>

// Curve points are integers on a scale of 0 to nSCALE (100*100; percentages at decimal points)

/////////////////////////////////////////////////////////////////////////////
CCurve::CCurve(COLORREF color) :
	Pen(color),
	Brush(color)
{
	SetToDefaults();
	SetColor(color);
}


/////////////////////////////////////////////////////////////////////////////
CCurve::CCurve(const CCurve& from) :
	Pen(from.Pen),
	Brush(from.Brush)
{
	SetToDefaults();
	SetColor(from.Color);
	SetPoints(from.Points, from.NumPoints);
}


/////////////////////////////////////////////////////////////////////////////
CCurve::CCurve(const POINT* Points, int numPoints, COLORREF color) :
	Pen(color),
	Brush(color)
{
	SetToDefaults();
	SetColor(color);
	SetPoints(Points, numPoints);
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::SetToDefaults()
{
	Points[0].x = 0;
	Points[0].y = 0;
	Points[1].x = nSCALE;
	Points[1].y = nSCALE;
	NumPoints = 2;
	iCurrent = 0;
	Select(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CCurve::SetToEmpty()
{
	Points[0].x = 0;
	Points[0].y = 0;
	Points[1].x = nSCALE;
	Points[1].y = nSCALE;
	NumPoints = 0;
	iCurrent = 0;
	Select(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
CCurve& CCurve::operator = (const CCurve& from)
{
	if (this != &from)
	{ // Check for a = a case
		SetPoints(from.Points, from.NumPoints);
		iCurrent = (NumPoints-1)/2;
		Pen = from.Pen;
		Brush = from.Brush;
		Color = from.Color;
	}

	return *this;
}


/////////////////////////////////////////////////////////////////////////////
CCurve::~CCurve()
{
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::Select( BOOL bSelect )
{
	bSelected = bSelect;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCurve::Selected(void)
{
	return bSelected;
}

/////////////////////////////////////////////////////////////////////////////
int CCurve::GetCurrent(void)
{
	return iCurrent; // return the current point
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::ProcessPoint(const POINT& point)
{ // Set iCurrent to after completion
	// If a point was clicked, return it
	if (PointWasClicked(point, iCurrent))
		return; // return the current point

	// If nothing can be added, return the closest point
	if (!CanAddPoint())
	{
		iCurrent = ClosestToPoint(point);
		return; // return the closest point
	}
	
	// If the point is not on the curve, return nothing (the anchor)
	POINT pointNew = point;
	if (!CurveWasClicked(point,pointNew))
	{
		iCurrent = 0; // return nothing (the anchor)
		return;
	}

	// Add a new point and return it

	// Find the right hand neighbor
	iCurrent = RightNeighbor(pointNew);

	// make room in the array for the new member
	for (int i = NumPoints-1; i >= iCurrent; i--)
		Points[i+1] = Points[i];

	Points[iCurrent] = pointNew;
	NumPoints++;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCurve::CurveWasClicked(const POINT& point, POINT& pointNew)
{
	for (int i = 0; i < MaxNumSplinePoints; i++)
	{
		int iDistance = abs(point.x - SplinePoints[i].x);
		iDistance += abs(point.y - SplinePoints[i].y);
		if ( iDistance < HandlesDim )
		{
			pointNew = SplinePoints[i];
			return TRUE; // a hit!
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCurve::PointWasClicked(const POINT& point, int& index)
{
	for (int i = 1; i < NumPoints-1; i++)
	{
		int iDistance = abs(point.x - Points[i].x);
		iDistance += abs(point.y - Points[i].y);
		if ( iDistance < HandlesDim )
		{
			index = i;
			return TRUE; // a hit!
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
inline float Distance(const POINT& p1, const POINT& p2)
{
	return float(_hypot(float(p1.x-p2.x), float(p1.y-p2.y)));
}


/////////////////////////////////////////////////////////////////////////////
int CCurve::ClosestToIndex(int index)
{
	if ( NumPoints <= 2 )
		return 0;

	if ( index <= 0 )
		return 1;

	if ( index >= (NumPoints-1) )
		return (NumPoints-2);

	float distLeft  = Distance(Points[index], Points[index-1]);
	float distRight = Distance(Points[index], Points[index+1]);

	if ( distLeft < distRight )
		return index-1;
	else
		return index+1;
}


/////////////////////////////////////////////////////////////////////////////
int CCurve::ClosestToPoint(const POINT& point)
{
	int index = 0;
	float minDist = FLT_MAX;

	// do not include the two end points
	for (int i = 1;i < (NumPoints-1);i++)
	{
		float Dist = Distance(point, Points[i]);
		if (Dist < minDist)
		{
			minDist = Dist;
			index = i;
		}
	}

	return index;
}

/////////////////////////////////////////////////////////////////////////////
// find the point to my right
int CCurve::RightNeighbor(const POINT& point)
{
	// find the neighbor on the right
	for (int index = 0; index < NumPoints; index++)
	{
		if (Points[index].x > point.x)
			return index; // found it
	}

	return NumPoints-1; // must be the last point
}

/////////////////////////////////////////////////////////////////////////////
int CCurve::GetOutput(int x)
{
	// First see if we have an exact pojnt match
	for (int i = 0; i < NumPoints; i++)
	{
		if (x == Points[i].x)
			return Points[i].y;
	}

	// If not, look it up in the spline array
	x = ((x * (MaxNumSplinePoints-1)) + nSCALE/2) / nSCALE;
	if (x < 0)
		x = 0;
	if (x > (MaxNumSplinePoints-1))
		x = (MaxNumSplinePoints-1);
	return SplinePoints[x].y;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCurve::GetPoint(int index, POINT& Point)
{
	if (index < 0 || index >= NumPoints)
		return FALSE;

	Point = Points[index];
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCurve::SetPoint(int index, POINT& Point)
{
	if (index < 0 || index >= NumPoints)
		return FALSE;

	AdjustForNeighbors(Point, index, Point);
	Points[index] = Point;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
int CCurve::GetPoints(POINT* pPoint)
{
	for (int i = 0; i < NumPoints; i++)
		pPoint[i] = Points[i];
	return NumPoints;
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::SetPoints(const POINT* PointsNew, int numPointsNew)
{
	if (numPointsNew > MaxNumPoints)
		numPointsNew = MaxNumPoints;
	NumPoints = numPointsNew;
	for (int i = 0; i < NumPoints; i++)
		Points[i] = PointsNew[i];
	iCurrent = (NumPoints-1)/2;
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::SetColor(COLORREF color)
{
	Pen.SetColor(color);
	Brush.SetColor(color);
	Color = color;
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::PrepareChange()
{
	Pen.SetCombo(R2_XORPEN);
	COLORREF color = Pen.GetColor();
	Pen.SetColor(~color);
	Brush.SetCombo(R2_XORPEN);
	color = Brush.GetColor();
	Brush.SetColor(~color);
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::FinishChange()
{
	Pen.SetCombo(R2_COPYPEN);
	COLORREF color = Pen.GetColor();
	Pen.SetColor(~color);
	Brush.SetCombo(R2_COPYPEN);
	color = Brush.GetColor();
	Brush.SetColor(~color);
}


/////////////////////////////////////////////////////////////////////////////
// Do not let a point left of its left neighbor or
// right of its right neighbor plus MIN_SEP - minimum separation in x
void CCurve::AdjustForNeighbors(const POINT& point, int index, POINT& actual)
{
	#define MIN_SEP 1
	actual = point;
	if (index < (NumPoints-1)
	 && actual.x > Points[index+1].x - MIN_SEP)
		actual.x = Points[index+1].x - MIN_SEP;
	if (index > 0
	 && actual.x < Points[index-1].x + MIN_SEP)
		actual.x = Points[index-1].x + MIN_SEP;
}

/////////////////////////////////////////////////////////////////////////////
void CCurve::LeftDown(HDC hDC, const POINT& point)
{
	Draw(hDC, TRUE/*bShowSelection*/, TRUE/*bWantHandles*/); // erase old

	ProcessPoint(point);

	if (!CanEditPoint(iCurrent))
		return;
	
	// do not drag past neighbors
	POINT actual;
	AdjustForNeighbors(point, iCurrent, actual);

	Points[iCurrent] = actual;
	Draw(hDC, TRUE/*bShowSelection*/, TRUE/*bWantHandles*/);	// draw new
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::LeftDrag(HDC hDC, const POINT& point)
{
	if (!CanEditPoint(iCurrent))
		return;
	
	// do not drag past neighbors
	POINT actual;
	AdjustForNeighbors(point, iCurrent, actual);

	Draw(hDC, TRUE/*bShowSelection*/, TRUE/*bWantHandles*/);
	Points[iCurrent] = actual;
	Draw(hDC, TRUE/*bShowSelection*/, TRUE/*bWantHandles*/);
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::LeftUp(HDC hDC, const POINT& point)
{
	if (!CanEditPoint(iCurrent))
		return;

	// do not drag past neighbors
	POINT actual;
	AdjustForNeighbors(point, iCurrent, actual);

	Draw(hDC, TRUE/*bShowSelection*/, TRUE/*bWantHandles*/);
	Points[iCurrent] = actual;
	Draw(hDC, TRUE/*bShowSelection*/, TRUE/*bWantHandles*/);
	
	// merge  points, if nessisary
	MergePoint(iCurrent);
}


/////////////////////////////////////////////////////////////////////////////
// if points are close to each other, make them a single point
// to turn off merging, set DefaultMergeDistance to a negative value
void CCurve::MergePoint(int Index)
{ // Set iCurrent to after completion
	if (Index <= 0 || Index >= (NumPoints-1))
		return; // nochange on endpoints

	if (Distance(Points[Index], Points[Index-1]) <= DefaultMergeDistance)
	{ // merge them
		DeletePoint(Index);
		iCurrent = Index - 1;
	}
	else
	if (Distance(Points[Index], Points[Index+1]) <= DefaultMergeDistance)
	{ // merge them
		DeletePoint(Index);
		iCurrent = Index;
	}
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCurve::DeletePoint(int Index)
{
	if (Index <= 0 || Index >= (NumPoints-1))
		return FALSE; // nochange on endpoints

	for (int i = Index; i < NumPoints; i++)
		Points[i] = Points[i+1];

	NumPoints--;

	if (iCurrent == Index)
		iCurrent = ClosestToIndex(Index);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCurve::CanEditPoint(int index)
{
	return index > 0 && index < (NumPoints-1);
	//return TRUE; //allow end points to be edited as well
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCurve::CanAddPoint()
{
	return NumPoints < MaxNumPoints;
}

/////////////////////////////////////////////////////////////////////////////
void CCurve::SplineLine(HDC hDC)
{
	Spline(Points, NumPoints, nSCALE, SplinePoints, MaxNumSplinePoints);
	if(NumPoints)
		Polyline(hDC, SplinePoints, MaxNumSplinePoints);
	else
		Polyline(hDC, SplinePoints, 0);
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::Draw(HDC hDC, BOOL bShowSelection, BOOL bWantHandles)
{
	// A hack to compute the pen width based on the curve window width
	#define WIDTH 254

	if ( bShowSelection && Selected() )
		Pen.SetWidth(((2/*pixels*/ * nSCALE) + WIDTH/2) / WIDTH);
	else
		Pen.SetWidth(0);

	HPEN oldPen = Pen.SetInto(hDC);
	SplineLine(hDC);
	DrawHandles(hDC, bWantHandles);
	SelectObject(hDC, oldPen);	
}


/////////////////////////////////////////////////////////////////////////////
void CCurve::DrawHandles(HDC hDC, BOOL bWantHandles)
{
	if ( !bWantHandles || !Selected() )
		return;

	if (!CanEditPoint(iCurrent))
		iCurrent = ClosestToIndex(iCurrent);

	HBRUSH oldBrush = Brush.SetInto(hDC);
	
	// draw a small boxes to show the  points
	for (int i = 1;i < (NumPoints-1); i++)
	{
		int iSize = (i == iCurrent ? HandlesDim/2 : HandlesDim/4);
		CPoint origin;
		CPoint corner;
		origin = Points[i];
		corner = Points[i];
		corner.x += iSize;
		corner.y += iSize;
		origin.x -= iSize;
		origin.y -= iSize;
		Rectangle(hDC, origin.x, origin.y, corner.x, corner.y);
	}
 
	SelectObject(hDC, oldBrush);
}

/////////////////////////////////////////////////////////////////////////////
//	START OF CCurveBrush CODE

/////////////////////////////////////////////////////////////////////////////
CCurveBrush::CCurveBrush()
{
	PhysicalBrush = 0;
	Combo = R2_COPYPEN;
	Color = RGB(255,255,255);
}

/////////////////////////////////////////////////////////////////////////////
CCurveBrush::CCurveBrush(COLORREF color)
{
	PhysicalBrush = 0;
	Combo = R2_COPYPEN;
	Color = color & 0x00FFFFFF; // zero out high byte
}

/////////////////////////////////////////////////////////////////////////////
CCurveBrush::CCurveBrush(int width, int combo, COLORREF color)
{
	PhysicalBrush = 0;
	Combo = R2_COPYPEN;
	Color = color & 0x00FFFFFF; // zero out high byte
}

/////////////////////////////////////////////////////////////////////////////
CCurveBrush::CCurveBrush(const CCurveBrush& from)
{
	PhysicalBrush = 0;
	Combo = R2_COPYPEN;
	Color = from.Color;
}

/////////////////////////////////////////////////////////////////////////////
CCurveBrush& CCurveBrush::operator = (const CCurveBrush& from)
{
	if (this != &from)
	{ // Check for a = a case
		PhysicalBrush = 0;
		Combo = R2_COPYPEN;
		Color = from.Color;
	}

	return *this;
}

/////////////////////////////////////////////////////////////////////////////
CCurveBrush::~CCurveBrush()
{
	if (PhysicalBrush)
		DeleteObject(PhysicalBrush);
}

/////////////////////////////////////////////////////////////////////////////
HBRUSH CCurveBrush::SetInto(HDC hDC)
{
	if (!PhysicalBrush)
		Physicalize();
	SetROP2(hDC, Combo);
	HBRUSH hBrush = (HBRUSH)SelectObject(hDC, PhysicalBrush);
	return hBrush;
}

/////////////////////////////////////////////////////////////////////////////
void CCurveBrush::SetCombo(int combo)
{
	Combo = combo;
}

/////////////////////////////////////////////////////////////////////////////
void CCurveBrush::SetColor(COLORREF color)
{
	Color = color & 0x00FFFFFF; // zero out high byte
	if (PhysicalBrush)
		DeleteObject(PhysicalBrush);
	PhysicalBrush = 0;
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CCurveBrush::GetColor(void)
{
	return Color;
}

/////////////////////////////////////////////////////////////////////////////
void CCurveBrush::Physicalize()
{
	if (PhysicalBrush)
		DeleteObject(PhysicalBrush);
	LOGBRUSH brush;
	brush.lbStyle = BS_SOLID;
	brush.lbColor = Color;
	
	((CCurveBrush*)this)->PhysicalBrush = CreateBrushIndirect(&brush);
}

/////////////////////////////////////////////////////////////////////////////
//	START OF CCurvePen CODE

/////////////////////////////////////////////////////////////////////////////
CCurvePen::CCurvePen()
{
	PhysicalPen = 0;
	Combo = R2_COPYPEN;
	Width = 0;
	Color = RGB(255,255,255);
}

/////////////////////////////////////////////////////////////////////////////
CCurvePen::CCurvePen(COLORREF color)
{
	PhysicalPen = 0;
	Combo = R2_COPYPEN;
	Width = 0;
	Color = color & 0x00FFFFFF; // zero out high byte
}

/////////////////////////////////////////////////////////////////////////////
CCurvePen::CCurvePen(int width, int combo, COLORREF color)
{
	PhysicalPen = 0;
	Combo = combo;
	Width = width;
	Color = color & 0x00FFFFFF; // zero out high byte
}

/////////////////////////////////////////////////////////////////////////////
CCurvePen::CCurvePen(const CCurvePen& from)
{
	PhysicalPen = 0;
	Combo = from.Combo;
	Width = from.Width;
	Color = from.Color;
}

/////////////////////////////////////////////////////////////////////////////
CCurvePen& CCurvePen::operator = (const CCurvePen& from)
{
	if (this != &from)
	{ // Check for a = a case
		PhysicalPen = 0;
		Combo = from.Combo;
		Width = from.Width;		
		Color = from.Color;
	}

	return *this;
}

/////////////////////////////////////////////////////////////////////////////
CCurvePen::~CCurvePen()
{
	if (PhysicalPen)
		DeleteObject(PhysicalPen);
}

/////////////////////////////////////////////////////////////////////////////
HPEN CCurvePen::SetInto(HDC hDC)
{
	if (!PhysicalPen)
		Physicalize();
	SetROP2(hDC, Combo);
	HPEN hPen = (HPEN)SelectObject(hDC, PhysicalPen);
	return hPen;
}

/////////////////////////////////////////////////////////////////////////////
void CCurvePen::SetCombo(int combo)
{
	Combo = combo;
}

/////////////////////////////////////////////////////////////////////////////
void CCurvePen::SetColor(COLORREF color)
{
	Color = color & 0x00FFFFFF; // zero out high byte
	if (PhysicalPen)
		DeleteObject(PhysicalPen);
	PhysicalPen = 0;
}

/////////////////////////////////////////////////////////////////////////////
COLORREF CCurvePen::GetColor(void)
{
	return Color;
}

/////////////////////////////////////////////////////////////////////////////
void CCurvePen::SetWidth(int width)
{
	Width = width;
	if (PhysicalPen)
		DeleteObject(PhysicalPen);
	PhysicalPen = 0;
}

/////////////////////////////////////////////////////////////////////////////
void CCurvePen::Physicalize()
{
	if (PhysicalPen)
		DeleteObject(PhysicalPen);
	LOGBRUSH brush;
	brush.lbStyle = BS_SOLID;
	brush.lbColor = Color;
	
	((CCurvePen*)this)->PhysicalPen = 
		ExtCreatePen(PS_GEOMETRIC | PS_JOIN_ROUND | PS_SOLID, 
			Width, &brush, 0, NULL);
}
