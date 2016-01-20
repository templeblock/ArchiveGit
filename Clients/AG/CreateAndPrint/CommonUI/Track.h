#pragma once

#include "Grid.h"

//////////////////////////////////////////////////////////////////////
// 4 Corner Distortion definition
struct DISTORT
{
	RECT RectOrig;
	RECT Rect;
	POINT p[4];
};

#define NUM_HANDLES 15
#define H_SIZE 6

#define TR_SIZE				1
#define TR_ROTATE			2
#define TR_DISTORT			4
#define TR_SHEAR			8
#define TR_EXCLUSIVE		16
#define TR_CONSTRAINASPECT	32
#define TR_DRAWASLINE		64
#define TR_BOUNDTOSYMBOL	128

#define TOOLCODE_ABORT	-1
#define TOOLCODE_UNUSED	 0
#define TOOLCODE_DONE	 1
#define TOOLCODE_UPDATE	 2

typedef void (*DRAWPROC)(HDC hDC, bool bOn, int iToolCode, void* pData);

class CTrack
{
public:
	CTrack();
	virtual ~CTrack();

public:
	void Init(CAGDC* pDC, int iWhatCanDo, DRAWPROC lpDrawProc, void* pData, RECT& SymbolRect, RECT& BoundRect, RECT& PageRect, CAGMatrix& ViewToDeviceMatrix, CAGMatrix& SymbolMatrix, CGrid& Grid);
	void Mode(int iWhatCanDo, bool fDisplay);
	bool Down(int x, int y, bool bInsideMoveAllowed);
	bool Up(int x, int y);
	bool Done(bool bSuccess);
	void GetExcessRect(RECT* pRect);
	CAGMatrix GetChangeMatrix();
	bool SetCursor(int x, int y);
	bool Move(int x, int y);
	bool Moved(void);
	void Delta(int dx, int dy);
	void Flip(bool fFlipX, bool fFlipY);
	void RotateConstrain(bool bConstrainX, bool bConstrainY);
	void Paint(HDC hDC);
	void Draw(bool bOn, HDC hDC = NULL);
	const CAGMatrix& GetMatrix() { return m_Matrix; }
	bool SetCursor(int x, int y, bool bInsideMoveAllowed, bool& bPtInBorder, bool& bPtInHandle);
	bool PtInBorder() 
		{ return m_bPtInBorder; }
	bool PtInHandle() 
		{ return m_bPtInHandle; }
	const RECT& GetRectOrig() const
		{ return m_Distort.RectOrig; }

	void Init(CAGDC* pDC, RECT& SymbolRect, RECT& BoundRect, RECT& PageRect, CAGMatrix& ViewToDeviceMatrix, CAGMatrix& SymbolMatrix, CGrid& Grid);
	void Highlight(bool bOn, HDC hDC, bool bHash);
	void StopHighlight();
	void PauseDrawing();
	void ResumeDrawing();

protected:
	void HlineFlash(HDC hDC, int x1, int y1, int x2, int y2, bool bOn);

	void ZeroData();
	void Scale(int x, int y, long hx, long hy, int ax, int ay, bool bConstrainAspect);
	double GetAngle(double fCenterX, double fCenterY, double fx, double fy);
	void Rotate(int x, int y, int iStartRotateX, int iStartRotateY);
	void Shear(int x, int y, long hx, long hy, bool bConstrainX, bool bConstrainY);
	void DrawHandles(HDC hDC, bool bOn, POINT* lpPt);
	void DrawHandle(HDC hDC, RECT* pRect, bool fCircle, bool bOn);
	void Dline(HDC hDC, int x1, int y1, int x2, int y2, bool bOn);
	void Hline(HDC hDC, int x1, int y1, int x2, int y2, bool bOn);
	void Dellipse(HDC hDC, RECT* pRect, bool bOn);
	bool PtInBorder(POINT pt);

#ifdef NOTUSED //j
	int ClosestDistortPoint(int index, int iMode);
	void MapOriginalRect(DISTORT* pDistort, RECT* pSrcRect, RECT* pDstRect);
	void MapOriginalPoint(DISTORT* pDistort, POINT* lpSrc, POINT* lpDst);
	void IntersectSegments(POINT p0, POINT p1, POINT p2, POINT p3, POINT* lpPt);
#endif NOTUSED //j

protected:
	CAGDC* m_pAGDC;
	DRAWPROC m_pDrawProc;
	void* m_pData;
	RECT m_PageRect;
	RECT m_BoundRect;
	RECT m_BoundRectScreen;
	RECT m_StartRect;
	bool m_iHandleInUse[NUM_HANDLES];
	RECT m_hHandleRects[NUM_HANDLES];
	int m_iWhatCanDo;
	bool m_bExclusive;
	bool m_bShear;
	bool m_bMoved;
	bool m_bRotateConstrainX;
	bool m_bRotateConstrainY;
	bool m_bMoveOnly;
	bool m_bConstrainAspect;
	bool m_bIsDrawn;
	bool m_bPtInBorder;
	bool m_bPtInHandle;
	int m_iHandleGrabbed;
	int m_iHiHandleInUse;
	int m_iLoHandleInUse;
	int m_iLastX, m_iLastY;
	int m_iLastDownX, m_iLastDownY;
	POINT m_ptCenter;
	POINT m_ptRotate;
	DISTORT m_Distort;
	CAGMatrix m_StartMatrix;
	CAGMatrix m_Matrix;
	CAGMatrix m_OrigMatrix;
	CAGMatrix m_ViewToDeviceMatrix;
	CGrid m_Grid;
	int m_iStartRotateX;
	int m_iStartRotateY;

	bool m_bIsHighlight;
	bool m_bHash;

#ifdef READOUT
	double m_fReadoutAngleX;
	double m_fReadoutAngleY;
	double m_fReadoutScaleX;
	double m_fReadoutScaleY;
	double m_fReadoutAngle1X;
	double m_fReadoutAngle1Y;
	double m_fReadoutScale1X;
	double m_fReadoutScale1Y;
#endif READOUT
};
