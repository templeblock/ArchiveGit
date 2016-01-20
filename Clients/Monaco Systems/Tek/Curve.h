#pragma once

enum CMYK
{
	CI_CYAN,
	CI_MAGENTA,
	CI_YELLOW,
	CI_GAMMA,
	CI_NCMYK,
};

/*** CONSTANTS ***/

// nSCale is 100*100; percentages at decimal points)
const int nSCALE = 10000; //256; //100
const int MaxNumPoints = 101;
const int MaxNumSplinePoints = 257;

/*** TYPE DEFINITIONS ***/

class	CCurveBrush;
typedef CCurveBrush& RCCurveBrush;
typedef CCurveBrush* PCCurveBrush;

class	CCurvePen;
typedef CCurvePen& RCCurvePen;
typedef CCurvePen* PCCurvePen;

class	CCurve;
typedef CCurve& RCCurve;
typedef CCurve* PCCurve;

class CCurveBrush
{
protected:
	HBRUSH PhysicalBrush;
	int Combo;
	COLORREF Color;
	int Width;
	void Physicalize();
public:
	CCurveBrush();
	CCurveBrush(COLORREF color);
	CCurveBrush(int width,int combo,COLORREF color);
	CCurveBrush(const CCurveBrush& from);
	virtual CCurveBrush& operator=(const CCurveBrush& from);
	virtual ~CCurveBrush();
	virtual CCurveBrush* Copy()
	{
		CCurveBrush* result=new CCurveBrush(*this);
		return(result);
	};
	HBRUSH SetInto(HDC canvas);
	void SetCombo(int combo);
	void SetColor(COLORREF color);
	COLORREF GetColor(void);
};

class CCurvePen
{
protected:
	HPEN PhysicalPen;
	int Combo;
	COLORREF Color;
	int Width;
	void Physicalize();
public:
	CCurvePen();
	CCurvePen(COLORREF color);
	CCurvePen(int width,int combo,COLORREF color);
	CCurvePen(const CCurvePen& from);
	virtual CCurvePen& operator=(const CCurvePen& from);
	virtual ~CCurvePen();
	virtual CCurvePen* Copy()
	{
		CCurvePen* result=new CCurvePen(*this);
		return(result);
	};
	HPEN SetInto(HDC canvas);
	void SetCombo(int combo);
	void SetColor(COLORREF color);
	COLORREF GetColor(void);
	void SetWidth(int width);
};

class CCurve
{
protected:
	// change any of the following enums.
	// DefaultHitDistance	How close to the line the mouse has to be before I call it a hit.
	// DefaultMergeDistance	How close two points need to be dragged before they will merge
	// HandlesDim			The width and height of the box around the current handle
	enum {DefaultHitDistance=nSCALE/8,DefaultMergeDistance=nSCALE/16,
		HandlesDim=nSCALE/25};
	POINT SplinePoints[MaxNumSplinePoints];
	POINT Points[MaxNumPoints];
	int NumPoints;
	int iCurrent;
	int Color;
	BOOL bSelected;
	// pen for drawing the curve
	CCurvePen Pen;
	// brush for drawing the Handles box
	CCurveBrush Brush;
	// Do not let a point left of its left neighbor or
	// right of its right neighbor
	void AdjustForNeighbors(const POINT& point,int index,
				POINT& actual);
	BOOL PointWasClicked(const POINT& point,int& index);
	int ClosestToIndex(int index);
	int ClosestToPoint(const POINT& point);
	BOOL CanEditPoint(int index);
	BOOL CanAddPoint();
	// find the point to my right
	int RightNeighbor(const POINT& point);
	// if points are close to eachother, make them a single point
	// to turn off merging, set DefaultMergeDiatance to a negative value
	void MergePoint(int Index);
public:
	CCurve(COLORREF color);
	CCurve(const CCurve& from);
	CCurve(const POINT* Points,
			int numPoints,COLORREF color);
	virtual CCurve& operator=(const CCurve& from);
	virtual ~CCurve();
	virtual CCurve* Copy()
	{
		CCurve* result=new CCurve(*this);
		return(result);
	};
	int GetNumPoints() { return NumPoints; };
	void Select(BOOL bSelect);
	BOOL Selected(void);
	int GetCurrent(void);
	void ProcessPoint(const POINT& point);
	void SetToDefaults();
	void SetToEmpty();
	int GetOutput(int x);
	BOOL GetPoint(int index, POINT& Point);
	BOOL SetPoint(int index, POINT& Point);
	void SetPoints(const POINT* Points,int numPoints);
	int GetPoints(POINT* Points);
	void SetColor(COLORREF color);
	void SplineLine(HDC canvas);
	void Draw(HDC canvas, BOOL bShowSelection, BOOL bWantHandles);
	void DrawHandles(HDC canvas, BOOL bWantHandles);
	BOOL CurveWasClicked(const POINT& point, POINT& pointNew);
	void PrepareChange(); // not really used anymore,good for future use
	void FinishChange(); // not really used anymore, good for future use
	void LeftDown(HDC canvas,const POINT& point);
	void LeftDrag(HDC canvas,const POINT& point);
	void LeftUp(HDC canvas,const POINT& point);
	BOOL DeletePoint(int Index);
};
