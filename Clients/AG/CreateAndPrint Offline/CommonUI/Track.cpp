#include "stdafx.h"
#include "ctp.h"
#include "Track.h"
#include "AGMatrix.h"
#include "AGDC.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

static bool m_bConstrainX;
static bool m_bConstrainY;
static bool ConstrainXY(LPINT lpX, LPINT lpY, bool bButtonDown, bool bInit, bool bActive);

#define _OFFSET 40
#define LINE_THICKNESS 6

#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)

extern "C"
{
	double atan2(double, double);
}

#define H_NONE			0	// none
#define H_UL			1	// upper-left
#define H_UR			2	// upper-right
#define H_LR			3	// lower-right
#define H_LL			4	// lower-left
#define H_TOP			5	// top middle
#define H_RIGHT 		6	// right middle
#define H_BOTTOM		7	// bottom middle
#define H_LEFT			8	// left middle
#define H_ROTATE		9	// rotation point
#define H_CENTER		10	// rotation center point
#define H_CORNER_UL 	11	// distorted upper-left
#define H_CORNER_UR 	12	// distorted upper-right
#define H_CORNER_LR 	13	// distorted lower-right
#define H_CORNER_LL 	14	// distorted lower-left
#define H_MOVE			100	// move object pseudo-handle

//////////////////////////////////////////////////////////////////////
CTrack::CTrack()
{
	ZeroData();
}

//////////////////////////////////////////////////////////////////////
CTrack::~CTrack()
{
}

//////////////////////////////////////////////////////////////////////
void CTrack::ZeroData()
{
	m_bIsHighlight = false;
	m_bHash = false;

	m_pAGDC = NULL;
	m_bIsDrawn = false;
	m_pDrawProc = NULL;

	m_ptCenter.x = 0;
	m_ptCenter.y = 0;
	m_ptRotate.x = 0;
	m_ptRotate.y = 0;

	m_iWhatCanDo = 0;
	m_bShear = false;
	m_bExclusive = false;
	m_bMoved = false;
	m_bConstrainAspect = true;
	m_bRotateConstrainX = false;
	m_bRotateConstrainY = false;
	m_bMoveOnly = false;
	m_iHandleGrabbed = 0;
	m_iLastX = 0;
	m_iLastY = 0;
	m_iLastDownX = 0;
	m_iLastDownY = 0;
	m_iStartRotateX = 0;
	m_iStartRotateY = 0;
	m_bPtInBorder = false;
	m_bPtInHandle = false;

	::SetRectEmpty(&m_PageRect);
	::SetRectEmpty(&m_BoundRect); 
	::SetRectEmpty(&m_BoundRectScreen);
	::SetRectEmpty(&m_StartRect);

	::ZeroMemory(m_iHandleInUse, sizeof(m_iHandleInUse));
	::ZeroMemory(m_hHandleRects, sizeof(m_hHandleRects));
	::ZeroMemory(&m_Distort, sizeof(m_Distort));

	m_ViewToDeviceMatrix.Unity();
	m_Matrix.Unity();
	m_OrigMatrix.Unity();

#ifdef READOUT
	m_fReadoutAngleX = 0.0;
	m_fReadoutAngleY = 0.0;
	m_fReadoutScaleX = 0.0;
	m_fReadoutScaleY = 0.0;
	m_fReadoutAngle1X  = 0.0;
	m_fReadoutAngle1Y  = 0.0;
	m_fReadoutScale1X  = 0.0;
	m_fReadoutScale1Y  = 0.0;
#endif READOUT
}

//////////////////////////////////////////////////////////////////////
void CTrack::Init(CAGDC* pDC, RECT& SymbolRect, RECT& BoundRect, RECT& PageRect, CAGMatrix& ViewToDeviceMatrix, CAGMatrix& SymbolMatrix, CGrid& Grid)
{
	ZeroData();

	m_pAGDC = pDC;
	m_ViewToDeviceMatrix = ViewToDeviceMatrix;
	m_OrigMatrix = SymbolMatrix;
	m_Matrix = m_OrigMatrix;
	m_Grid = Grid;

	m_PageRect = PageRect;
	m_BoundRectScreen = m_PageRect;

	// Setup the bounding rectangle
	m_Distort.Rect = SymbolRect;

	// Don't let the rect be empty in either dimension
	::InflateRect(&m_Distort.Rect, !WIDTH(m_Distort.Rect), !HEIGHT(m_Distort.Rect));

	m_Distort.RectOrig = m_Distort.Rect;
	m_Distort.p[0].x = m_Distort.Rect.left;
	m_Distort.p[0].y = m_Distort.Rect.top;
	m_Distort.p[1].x = m_Distort.Rect.right;
	m_Distort.p[1].y = m_Distort.Rect.top;
	m_Distort.p[2].x = m_Distort.Rect.right;
	m_Distort.p[2].y = m_Distort.Rect.bottom;
	m_Distort.p[3].x = m_Distort.Rect.left;
	m_Distort.p[3].y = m_Distort.Rect.bottom;
	m_ptCenter.x = (m_Distort.Rect.left + m_Distort.Rect.right)/2;
	m_ptCenter.y = (m_Distort.Rect.top  + m_Distort.Rect.bottom)/2;
	m_ptRotate.x = m_ptCenter.x;
	m_ptRotate.y = (m_Distort.Rect.top  + m_ptCenter.y)/2;
}

//////////////////////////////////////////////////////////////////////
void CTrack::Init(CAGDC* pDC, int iWhatCanDo, DRAWPROC lpDrawProc, void* pData, RECT& SymbolRect, RECT& BoundRect, RECT& PageRect, CAGMatrix& ViewToDeviceMatrix, CAGMatrix& SymbolMatrix, CGrid& Grid)
{
	ZeroData();

	m_iWhatCanDo = 	iWhatCanDo;
	Mode(m_iWhatCanDo, false/*fDisplay*/);

	RotateConstrain(false/*bConstrainX*/, false/*bConstrainY*/);

	m_pAGDC = pDC;
	m_pDrawProc = lpDrawProc;
	m_pData = pData;

	m_ViewToDeviceMatrix = ViewToDeviceMatrix;
	m_OrigMatrix = SymbolMatrix;
	m_Matrix = m_OrigMatrix;
	m_Grid = Grid;

	m_PageRect = PageRect;
	if (m_iWhatCanDo & TR_BOUNDTOSYMBOL)
	{
		m_BoundRect = BoundRect;
		m_Matrix.Transform(m_BoundRect);
		m_BoundRectScreen = m_BoundRect;
		m_ViewToDeviceMatrix.Transform(m_BoundRectScreen);
	}
	else
		m_BoundRectScreen = m_PageRect;

	// Setup the bounding rectangle
	m_Distort.Rect = SymbolRect;

	// Don't let the rect be empty in either dimension
	::InflateRect(&m_Distort.Rect, !WIDTH(m_Distort.Rect), !HEIGHT(m_Distort.Rect));

	m_Distort.RectOrig = m_Distort.Rect;
	m_Distort.p[0].x = m_Distort.Rect.left;
	m_Distort.p[0].y = m_Distort.Rect.top;
	m_Distort.p[1].x = m_Distort.Rect.right;
	m_Distort.p[1].y = m_Distort.Rect.top;
	m_Distort.p[2].x = m_Distort.Rect.right;
	m_Distort.p[2].y = m_Distort.Rect.bottom;
	m_Distort.p[3].x = m_Distort.Rect.left;
	m_Distort.p[3].y = m_Distort.Rect.bottom;
	m_ptCenter.x = (m_Distort.Rect.left + m_Distort.Rect.right)/2;
	m_ptCenter.y = (m_Distort.Rect.top  + m_Distort.Rect.bottom)/2;
	m_ptRotate.x = m_ptCenter.x;
	m_ptRotate.y = (m_Distort.Rect.top  + m_ptCenter.y)/2;

	// init to no handles grabbed and no transforming done
	m_iHandleGrabbed = 0;

	m_bMoveOnly = true;

#ifdef READOUT
	m_fReadoutAngleX = m_fReadoutAngleY = 0;
	m_fReadoutScaleX = m_fReadoutScaleY = 1.0;
#endif READOUT
}

//////////////////////////////////////////////////////////////////////
void CTrack::Mode(int iWhatCanDo, bool fDisplay)
{
	if (fDisplay)
		Draw(false/*bOn*/);

	bool bCanSize		= !!(iWhatCanDo & TR_SIZE); 
	bool bCanRotate		= !!(iWhatCanDo & TR_ROTATE);   
	bool bCanDistort	= !!(iWhatCanDo & TR_DISTORT);  
	bool bDrawAsLine	= !!(iWhatCanDo & TR_DRAWASLINE);  
	m_bShear			= !!(iWhatCanDo & TR_SHEAR);
	m_bExclusive		= !!(iWhatCanDo & TR_EXCLUSIVE);
	m_bConstrainAspect	= !!(iWhatCanDo & TR_CONSTRAINASPECT);

	m_iHandleInUse[H_NONE]		= false;									// none
	m_iHandleInUse[H_UL]		= (bCanSize | m_bShear);					// upper-left
	m_iHandleInUse[H_UR]		= (bCanSize | m_bShear) && !bDrawAsLine;	// upper-right
	m_iHandleInUse[H_LR]		= (bCanSize | m_bShear);					// lower-right
	m_iHandleInUse[H_LL]		= (bCanSize | m_bShear) && !bDrawAsLine;	// lower-left
	m_iHandleInUse[H_TOP]		= (bCanSize | m_bShear) && !bDrawAsLine;	// top middle
	m_iHandleInUse[H_RIGHT]		= (bCanSize | m_bShear) && !bDrawAsLine;	// right middle
	m_iHandleInUse[H_BOTTOM]	= (bCanSize | m_bShear) && !bDrawAsLine;	// bottom middle
	m_iHandleInUse[H_LEFT]		= (bCanSize | m_bShear) && !bDrawAsLine;	// left middle
	m_iHandleInUse[H_ROTATE]	= bCanRotate;								// rotation point
	m_iHandleInUse[H_CENTER]	= bCanRotate;								// rotation center point
	m_iHandleInUse[H_CORNER_UL]	= bCanDistort;								// distorted upper-left
	m_iHandleInUse[H_CORNER_UR]	= bCanDistort;								// distorted upper-right
	m_iHandleInUse[H_CORNER_LR]	= bCanDistort;								// distorted lower-right
	m_iHandleInUse[H_CORNER_LL]	= bCanDistort;								// distorted lower-left

	if (fDisplay)
		Draw(true/*bOn*/);
}

//////////////////////////////////////////////////////////////////////
bool CTrack::Down(int x, int y, bool bInsideMoveAllowed)
{
	m_bMoved = false;
	m_iHandleGrabbed = 0;
	m_bPtInBorder = false;
	m_bPtInHandle = false;

	POINT Point = {x, y};
	for (int h = 0; h < NUM_HANDLES; h++)
	{
		if (!m_iHandleInUse[h])
			continue;

		if (PtInRect(&m_hHandleRects[h], Point))
		{
			m_iHandleGrabbed = h;
			m_bPtInHandle = true;
			break;
		}
	}

	if (m_iHandleGrabbed)
	{
		if (m_bShear)
		{
			if (!CONTROL
				&& m_iHandleGrabbed != H_MOVE
				&& m_iHandleGrabbed != H_CENTER
				&& m_iHandleGrabbed != H_ROTATE )
				ConstrainXY(&x, &y, true/*bButtonDown*/, true/*bInit*/, true/*bActive*/);
		}
	}
	else
	{
		if (PtInBorder(Point))
			m_bPtInBorder = true;

		RECT rect = m_Distort.Rect;
		m_Matrix.Transform(rect);
		m_ViewToDeviceMatrix.Transform(rect);
		// Make sure the rect is big enough to grab
		if (WIDTH(rect) < 20)
			::InflateRect(&rect, 10, 0);
		if (HEIGHT(rect) < 20)
			::InflateRect(&rect, 0, 10);
		if (m_bPtInBorder || (bInsideMoveAllowed && PtInRect(&rect, Point)))
		{
			m_iHandleGrabbed = H_MOVE;
			m_bPtInHandle = false;
		}
	}

	if (m_iHandleGrabbed)
	{
		POINT pt = {x, y};
		m_iLastDownX = pt.x;
		m_iLastDownY = pt.y;
		m_ViewToDeviceMatrix.Inverse().Transform(pt);
		m_iLastX = pt.x;
		m_iLastY = pt.y;

		m_StartRect = m_Distort.Rect;
		m_Matrix.Transform(m_StartRect);
		m_StartMatrix = m_Matrix;
		m_iStartRotateX = m_ptRotate.x;
		m_iStartRotateY = m_ptRotate.y;

#ifdef READOUT
		m_fReadoutScale1X = m_fReadoutScaleX;
		m_fReadoutScale1Y = m_fReadoutScaleY;
		m_fReadoutAngle1X = m_fReadoutAngleX;
		m_fReadoutAngle1Y = m_fReadoutAngleY;
#endif READOUT
	}

	return !!m_iHandleGrabbed;
}

//////////////////////////////////////////////////////////////////////
bool CTrack::Up(int x, int y)
{
	if (!m_iHandleGrabbed)
		return false;

	m_iHandleGrabbed = 0;
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CTrack::Done(bool bSuccess)
{
	Draw(false/*bOn*/);

	if (m_pDrawProc && m_pAGDC)
	{
		int iToolCode = (bSuccess ? TOOLCODE_DONE : TOOLCODE_ABORT);
		HDC hDC = m_pAGDC->GetHDC();
		m_pDrawProc(hDC, false/*bOn*/, iToolCode, m_pData);
	}

	// return whether the symbol was only moved
	return m_bMoveOnly;
}

//////////////////////////////////////////////////////////////////////
void CTrack::GetExcessRect(RECT* pRect)
{
	if (!pRect)
		return;

	::UnionRect(pRect, &m_hHandleRects[H_ROTATE], &m_hHandleRects[H_CENTER]);
	::InflateRect(pRect, H_SIZE, H_SIZE);
}

//////////////////////////////////////////////////////////////////////
CAGMatrix CTrack::GetChangeMatrix()
{
	CAGMatrix ChangeMatrix = m_Matrix * m_OrigMatrix.Inverse();
	return ChangeMatrix;
}

//////////////////////////////////////////////////////////////////////
bool CTrack::SetCursor(int x, int y, bool bInsideMoveAllowed, bool& bPtInBorder, bool& bPtInHandle)
{
	POINT Point = {x, y};
	HINSTANCE hInst = NULL;
	LPCSTR lpCursor = NULL;
	bPtInBorder = false;
	bPtInHandle = false;
	for (int h = 0; h < NUM_HANDLES; h++)
	{
		if (!m_iHandleInUse[h])
			continue;

		if (!PtInRect(&m_hHandleRects[h], Point))
			continue;

		bPtInHandle = true;
		switch (h)
		{
			case H_ROTATE:
				lpCursor = MAKEINTRESOURCE(ID_ROTATE);
				hInst = _AtlBaseModule.GetResourceInstance();
				break;
			case H_CENTER:
				lpCursor = IDC_ARROW;
				break;
			case H_CORNER_UL:
			case H_CORNER_UR:
			case H_CORNER_LR:
			case H_CORNER_LL:
				lpCursor = IDC_ARROW;
				break;
			case H_UL:
			case H_LR:
				lpCursor = IDC_SIZENWSE;
				break;
			case H_UR:
			case H_LL:
				lpCursor = IDC_SIZENESW;
				break;
			case H_TOP:
			case H_BOTTOM:
				lpCursor = IDC_SIZENS;
				break;
			case H_LEFT:
			case H_RIGHT:
				lpCursor = IDC_SIZEWE;
				break;
			default:
				break;
		}
		break;
	}

	if (!lpCursor)
	{
		if (PtInBorder(Point))
			bPtInBorder = true;

		RECT rect = m_Distort.Rect;
		m_Matrix.Transform(rect);
		m_ViewToDeviceMatrix.Transform(rect);
		// Make sure the rect is big enough to grab
		if (WIDTH(rect) < 20)
			::InflateRect(&rect, 10, 0);
		if (HEIGHT(rect) < 20)
			::InflateRect(&rect, 0, 10);

		if (bPtInBorder || (bInsideMoveAllowed && PtInRect(&rect, Point)))
			lpCursor = IDC_SIZEALL;
	}

	if (lpCursor)
	{
		::SetCursor(::LoadCursor(hInst, lpCursor));
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CTrack::SetCursor(int x, int y)
{
	POINT Point = {x, y};
	LPCSTR lpCursor = NULL;
	HINSTANCE hInst = NULL;
	for (int h = 0; h < NUM_HANDLES; h++)
	{
		if (!m_iHandleInUse[h])
			continue;

		if (!PtInRect(&m_hHandleRects[h], Point))
			continue;

		switch (h)
		{
			case H_ROTATE:
				lpCursor = MAKEINTRESOURCE(ID_ROTATE);
				hInst = _AtlBaseModule.GetResourceInstance();
				break;
			case H_CENTER:
				lpCursor = (LPCSTR)IDC_ARROW;
				hInst = NULL;
				break;
			case H_CORNER_UL:
			case H_CORNER_UR:
			case H_CORNER_LR:
			case H_CORNER_LL:
				lpCursor = (LPCSTR)IDC_ARROW;
				hInst = NULL;
				break;
			case H_UL:
			case H_LR:
				lpCursor = (LPCSTR)IDC_SIZENWSE;
				hInst = NULL;
				break;
			case H_UR:
			case H_LL:
				lpCursor = (LPCSTR)IDC_SIZENESW;
				hInst = NULL;
				break;
			case H_TOP:
			case H_BOTTOM:
				lpCursor = (LPCSTR)IDC_SIZENS;
				hInst = NULL;
				break;
			case H_LEFT:
			case H_RIGHT:
				lpCursor = (LPCSTR)IDC_SIZEWE;
				hInst = NULL;
				break;
			default:
				break;
		}
		break;
	}

	if (!lpCursor)
	{
		if (PtInBorder(Point))
		{
			lpCursor = IDC_SIZEALL;
			hInst = NULL;
		}
	}

	if (lpCursor)
	{
		::SetCursor(::LoadCursor(hInst, lpCursor));
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CTrack::PtInBorder(POINT pt)
{
	CAGMatrix Matrix = m_Matrix * m_ViewToDeviceMatrix;
	Matrix.Inverse().Transform(pt);

	CRect Rect = m_Distort.Rect;
    Rect.InflateRect(2*_OFFSET, 2*_OFFSET);
	bool bInOuterRect = !!Rect.PtInRect(pt);
    Rect.DeflateRect(3*_OFFSET, 3*_OFFSET);
	bool bInInnerRect = !!Rect.PtInRect(pt);
	return bInOuterRect && !bInInnerRect;
}

//////////////////////////////////////////////////////////////////////
bool CTrack::Move(int x, int y)
{
	if (!m_iHandleGrabbed)
		return false;

	// Prevent any accidental moves on the first click
	if (m_iHandleGrabbed == H_MOVE && !m_bMoved)
	{
		#define CLOSENESS 10 // pixels
		int dx = m_iLastDownX - x;
		int dy = m_iLastDownY - y;
		if (dx >= -CLOSENESS && dx <= CLOSENESS && dy >= -CLOSENESS && dy <= CLOSENESS)
			return false;
	}

	if (m_bShear)
		ConstrainXY(&x, &y, true/*bButtonDown*/, false/*bInit*/, m_bShear/*bActive*/);

	x = bound(x, m_BoundRectScreen.left, m_BoundRectScreen.right);
	y = bound(y, m_BoundRectScreen.top, m_BoundRectScreen.bottom);

	POINT pt = {x, y};
	m_ViewToDeviceMatrix.Inverse().Transform(pt);
	x = pt.x;
	y = pt.y;

	if (x == m_iLastX && y == m_iLastY)
		return false;

	m_bMoved = true;
	Draw(false/*bOn*/);

	switch (m_iHandleGrabbed)
	{
		case H_UL:
		{
			m_Grid.Snap(pt);
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_ROTATE))
				Mode(m_iWhatCanDo & ~TR_ROTATE, false/*fDisplay*/);

			if (m_bShear)
				Shear(pt.x, pt.y, m_Distort.Rect.left, m_Distort.Rect.top, m_bConstrainX, m_bConstrainY);
			else
				Scale(pt.x, pt.y, m_Distort.Rect.left, m_Distort.Rect.top, m_Distort.Rect.right, m_Distort.Rect.bottom, m_bConstrainAspect ^ CONTROL);
			break;
		}

		case H_UR:
		{
			m_Grid.Snap(pt);
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_ROTATE))
				Mode(m_iWhatCanDo & ~TR_ROTATE, false/*fDisplay*/);

			if (m_bShear)
				Shear(pt.x, pt.y, m_Distort.Rect.right, m_Distort.Rect.top, m_bConstrainX, m_bConstrainY);
			else
				Scale(pt.x, pt.y, m_Distort.Rect.right, m_Distort.Rect.top, m_Distort.Rect.left, m_Distort.Rect.bottom, m_bConstrainAspect ^ CONTROL);
			break;
		}

		case H_LR:
		{
			m_Grid.Snap(pt);
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_ROTATE))
				Mode(m_iWhatCanDo & ~TR_ROTATE, false/*fDisplay*/);

			if (m_bShear)
				Shear(pt.x, pt.y, m_Distort.Rect.right, m_Distort.Rect.bottom, m_bConstrainX, m_bConstrainY);
			else
				Scale(pt.x, pt.y, m_Distort.Rect.right, m_Distort.Rect.bottom, m_Distort.Rect.left, m_Distort.Rect.top, m_bConstrainAspect ^ CONTROL);
			break;
		}

		case H_LL:
		{
			m_Grid.Snap(pt);
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_ROTATE))
				Mode(m_iWhatCanDo & ~TR_ROTATE, false/*fDisplay*/);

			if (m_bShear)
				Shear(pt.x, pt.y, m_Distort.Rect.left, m_Distort.Rect.bottom, m_bConstrainX, m_bConstrainY);
			else
				Scale(pt.x, pt.y, m_Distort.Rect.left, m_Distort.Rect.bottom, m_Distort.Rect.right, m_Distort.Rect.top, m_bConstrainAspect ^ CONTROL);
			break;
		}

		case H_TOP:
		{
			m_Grid.Snap(pt);
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_ROTATE))
				Mode(m_iWhatCanDo & ~TR_ROTATE, false/*fDisplay*/);

			int midx = (m_Distort.Rect.left + m_Distort.Rect.right)/2;

			if (m_bShear)
				Shear(pt.x, pt.y, midx, m_Distort.Rect.top, m_bConstrainX, m_bConstrainY);
			else
			{
				POINT ptMid = {midx, m_Distort.Rect.top};
				long dummy;
				m_Matrix.Transform(ptMid, pt.x, dummy);
				Scale(pt.x, pt.y, midx, m_Distort.Rect.top, midx, m_Distort.Rect.bottom, false/*bConstrainAspect*/);
			}
			break;
		}

		case H_RIGHT:
		{
			m_Grid.Snap(pt);
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_ROTATE))
				Mode(m_iWhatCanDo & ~TR_ROTATE, false/*fDisplay*/);

			int midy = (m_Distort.Rect.top + m_Distort.Rect.bottom)/2;

			if (m_bShear)
				Shear(pt.x, pt.y, m_Distort.Rect.right, midy, m_bConstrainX, m_bConstrainY);
			else
			{
				POINT ptMid = {m_Distort.Rect.right, midy};
				long dummy;
				m_Matrix.Transform(ptMid, dummy, pt.y);
				Scale(pt.x, pt.y, m_Distort.Rect.right, midy, m_Distort.Rect.left, midy, false/*bConstrainAspect*/);
			}
			break;
		}

		case H_BOTTOM:
		{
			m_Grid.Snap(pt);
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_ROTATE))
				Mode(m_iWhatCanDo & ~TR_ROTATE, false/*fDisplay*/);

			int midx = (m_Distort.Rect.left + m_Distort.Rect.right)/2;

			if (m_bShear)
				Shear(pt.x, pt.y, midx, m_Distort.Rect.bottom, m_bConstrainX, m_bConstrainY);
			else
			{
				POINT ptMid = {midx, m_Distort.Rect.bottom};
				long dummy;
				m_Matrix.Transform(ptMid, pt.x, dummy);
				Scale(pt.x, pt.y, midx, m_Distort.Rect.bottom, midx, m_Distort.Rect.top, false/*bConstrainAspect*/);
			}
			break;
		}

		case H_LEFT:
		{
			m_Grid.Snap(pt);
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_ROTATE))
				Mode(m_iWhatCanDo & ~TR_ROTATE, false/*fDisplay*/);

			int midy = (m_Distort.Rect.top + m_Distort.Rect.bottom)/2;

			if (m_bShear)
				Shear(pt.x, pt.y, m_Distort.Rect.left, midy, m_bConstrainX, m_bConstrainY);
			else
			{
				POINT ptMid = {m_Distort.Rect.left, midy};
				long dummy;
				m_Matrix.Transform(ptMid, dummy, pt.y);
				Scale(pt.x, pt.y, m_Distort.Rect.left, midy, m_Distort.Rect.right, midy, false/*bConstrainAspect*/);
			}
			break;
		}

		case H_CENTER:
		{
			// transform points to transformed position
			m_Matrix.Transform(m_ptCenter);
			m_Matrix.Transform(m_ptRotate);
			int dx = pt.x - m_ptCenter.x;
			int dy = pt.y - m_ptCenter.y;
			m_ptRotate.x += dx;
			m_ptRotate.y += dy;
			m_ptCenter = pt;
			// transform points back to untransformed position
			m_Matrix.Inverse().Transform(m_ptCenter);
			m_Matrix.Inverse().Transform(m_ptRotate);
			break;
		}

		case H_CORNER_UL:
		case H_CORNER_UR:
		case H_CORNER_LR:
		case H_CORNER_LL:
		{
			m_Grid.Snap(pt);
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_ROTATE))
				Mode(m_iWhatCanDo & ~TR_ROTATE, false/*fDisplay*/);

			// transform the new point back to its untransformed position
			POINT ptTemp = pt;
			m_Matrix.Inverse().Transform(ptTemp);
			int i = m_iHandleGrabbed - H_CORNER_UL;
			m_Distort.p[i] = ptTemp;

			// Update the m_Distort.Rect rectangle
			m_Distort.Rect.left   = min(min(min(m_Distort.p[0].x, m_Distort.p[1].x), m_Distort.p[2].x), m_Distort.p[3].x);
			m_Distort.Rect.right  = max(max(max(m_Distort.p[0].x, m_Distort.p[1].x), m_Distort.p[2].x), m_Distort.p[3].x);
			m_Distort.Rect.top    = min(min(min(m_Distort.p[0].y, m_Distort.p[1].y), m_Distort.p[2].y), m_Distort.p[3].y);
			m_Distort.Rect.bottom = max(max(max(m_Distort.p[0].y, m_Distort.p[1].y), m_Distort.p[2].y), m_Distort.p[3].y);
			break;
		}

		case H_ROTATE:
		{
			m_bMoveOnly = false;
			if (m_bExclusive && (m_iWhatCanDo & TR_SIZE))
				Mode(m_iWhatCanDo & ~TR_SIZE, false/*fDisplay*/);

			// transform points to transformed position
			Rotate(pt.x, pt.y, m_iStartRotateX, m_iStartRotateY);
			m_ptRotate = pt;
			// transform points back to untransformed position
			m_Matrix.Inverse().Transform(m_ptRotate);
			break;
		}

		case H_MOVE:
		{
			// Calculate how far we SHOULD move
			POINT delta1 = {pt.x - m_iLastX, pt.y - m_iLastY};

			// Calculate where we are right now
			RECT rect = m_Distort.Rect;
			m_Matrix.Transform(rect);

			// Calculate the new top-left point
			POINT ptNewTL = {rect.left + delta1.x, rect.top + delta1.y};

			// Snap it to the grid
			m_Grid.Snap(ptNewTL);

			if (m_iWhatCanDo & TR_BOUNDTOSYMBOL)
			{
				int delta;
				delta = ptNewTL.x - m_BoundRect.left;
				if (delta < 0)
					ptNewTL.x -= delta;
				delta = ptNewTL.y - m_BoundRect.top;
				if (delta < 0)
					ptNewTL.y -= delta;
				delta = (ptNewTL.x + WIDTH(rect)) - m_BoundRect.right;
				if (delta > 0)
					ptNewTL.x -= delta;
				delta = (ptNewTL.y + HEIGHT(rect)) - m_BoundRect.bottom;
				if (delta > 0)
					ptNewTL.y -= delta;
			}

			// Calculate how far we WILL move
			POINT delta2 = {ptNewTL.x - rect.left, ptNewTL.y - rect.top};

			// Adjust the point for the next time around
			x -= (delta1.x - delta2.x);
			y -= (delta1.y - delta2.y);

			m_Matrix.Translate(delta2.x, delta2.y);
			break;
		}

		default:
			return false;
		}

	if (m_pDrawProc && m_pAGDC)
	{
		HDC hDC = m_pAGDC->GetHDC();
		m_pDrawProc(hDC, false/*bOn*/, TOOLCODE_UPDATE, m_pData);
	}

	Draw(true/*bOn*/);
	m_iLastX = x;
	m_iLastY = y;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CTrack::Moved(void)
{
	return m_bMoved;
}

//////////////////////////////////////////////////////////////////////
void CTrack::Delta(int dx, int dy)
{
	POINT ptOffset = {dx, dy};
	CAGMatrix CombinedMatrix = m_Matrix * m_ViewToDeviceMatrix;
	CombinedMatrix.Inverse().Transform(&ptOffset, 1, false);

	m_bMoved = true;
	Draw(false/*bOn*/);
	m_Matrix.Translate(ptOffset.x, ptOffset.y);

	if (m_pDrawProc && m_pAGDC)
	{
		HDC hDC = m_pAGDC->GetHDC();
		m_pDrawProc(hDC, false/*bOn*/, TOOLCODE_UPDATE, m_pData);
	}

	Draw(true/*bOn*/);
}

//////////////////////////////////////////////////////////////////////
void CTrack::Scale(int x, int y, long hx, long hy, int ax, int ay, bool bConstrainAspect)
{
	POINT pt = {hx, hy};
	m_StartMatrix.Transform(pt, hx, hy);

	// get delta of move
	int dx = x - hx;	/* positive is a move to the right */
	int dy = y - hy;	/* positive is a move to the bottom */

	m_Matrix = m_StartMatrix;
	int minx = m_StartRect.left;
	int maxx = m_StartRect.right;
	int miny = m_StartRect.top;
	int maxy = m_StartRect.bottom;

	// get center of handle positions
	int midx = (minx + maxx)/2;
	int midy = (miny + maxy)/2;

	int old_width = maxx - minx + 1;
	int old_height = maxy - miny + 1;

	int new_width;
	if (hx <= midx)
		new_width = old_width - dx;
	else
		new_width = old_width + dx;

	int new_height;
	if (hy <= midy)
		new_height = old_height - dy;
	else
		new_height = old_height + dy;

	if (new_width < 3)
		new_width = 3;
	if (new_height < 3)
		new_height = 3;
	double scalex = (double)new_width / old_width;
	double scaley = (double)new_height / old_height;
	if (bConstrainAspect)
	{
		// to avoid scaling right off the image, always scale as little as possible
		scalex = scaley = max(scaley, scalex);
	}
	pt.x = ax;
	pt.y = ay;

	double fdx, fdy;
	m_Matrix.Transform(pt, fdx, fdy);
	m_Matrix.Scale(scalex, scaley, fdx, fdy);

#ifdef READOUT
	m_fReadoutScaleX = m_fReadoutScale1X * scalex;
	m_fReadoutScaleY = m_fReadoutScale1Y * scaley;

	if (m_fReadoutScaleX != m_fReadoutScaleY)
	 	ReadOutF(IDS_SCALEXY, 2, m_fReadoutScaleX * 100, m_fReadoutScaleY * 100);
	else
		ReadOutF(IDS_SCALE, 1, m_fReadoutScaleX * 100);
#endif READOUT
}

//////////////////////////////////////////////////////////////////////
void CTrack::Flip(bool fFlipX, bool fFlipY)
{
	Draw(false/*bOn*/);

	m_bMoveOnly = false;
	RECT rect = m_Distort.Rect;
	m_Matrix.Transform(rect);
	double fcx = (double)(rect.left + rect.right) / 2;
	double fcy = (double)(rect.top + rect.bottom) / 2;
	double fsx = (fFlipX ? -1.0 : 1.0);
	double fsy = (fFlipY ? -1.0 : 1.0);
	m_Matrix.Scale(fsx, fsy, fcx, fcy);

	if (m_pDrawProc && m_pAGDC)
	{
		HDC hDC = m_pAGDC->GetHDC();
		m_pDrawProc(hDC, false/*bOn*/, TOOLCODE_UPDATE, m_pData);
	}

	Draw(true/*bOn*/);
}

//////////////////////////////////////////////////////////////////////
void CTrack::RotateConstrain(bool bConstrainX, bool bConstrainY)
{
	m_bRotateConstrainX = bConstrainX;
	m_bRotateConstrainY = bConstrainY;
}

//////////////////////////////////////////////////////////////////////
double CTrack::GetAngle(double fCenterX, double fCenterY, double fx, double fy)
{
	#define PI 3.14159265

	fx = fx - fCenterX;
	fy = fCenterY - fy;

	double radians = atan2(fy, fx);
	double angle = (radians * 180.0) / PI;
	if (angle < 0)
		angle -= 0.1;
	else
		angle += 0.1;

	return angle;
}

//////////////////////////////////////////////////////////////////////
void CTrack::Rotate(int x, int y, int iStartRotateX, int iStartRotateY)
{
	double fCenterX, fCenterY;
	m_StartMatrix.Transform(m_ptCenter, fCenterX, fCenterY);

	POINT ptStart = {iStartRotateX, iStartRotateY};
	double fStartX, fStartY;
	m_StartMatrix.Transform(ptStart, fStartX, fStartY);
	double old_angle = GetAngle(fCenterX, fCenterY, fStartX, fStartY);
	double new_angle = GetAngle(fCenterX, fCenterY, x, y);
	double angle = old_angle - new_angle;
	double anglex = (m_bRotateConstrainX ? 0.0 : angle);
	double angley = (m_bRotateConstrainY ? 0.0 : angle);

	m_Matrix = m_StartMatrix;
	m_Matrix.Rotate(anglex, angley, fCenterX, fCenterY);

#ifdef READOUT
	m_fReadoutAngleX = m_fReadoutAngle1X + angle;
	m_fReadoutAngleY = m_fReadoutAngle1Y + angle;
	while (m_fReadoutAngleX < 0)
		m_fReadoutAngleX += 360.0;
	while (m_fReadoutAngleY < 0)
		m_fReadoutAngleY += 360.0;
	while (m_fReadoutAngleX >= 360.0)
		m_fReadoutAngleX -= 360.0;
	while (m_fReadoutAngleY >= 360.0)
		m_fReadoutAngleY -= 360.0;

	if (m_fReadoutAngleX != m_fReadoutAngleY)
		ReadOutF(IDS_ROTATEXY, 2, m_fReadoutAngleX, m_fReadoutAngleY);
	else
		ReadOutF(IDS_ROTATE, 1, m_fReadoutAngleX);
#endif READOUT
}

//////////////////////////////////////////////////////////////////////
void CTrack::Shear(int x, int y, long hx, long hy, bool bConstrainX, bool bConstrainY)
{
	int ox = hx;
	int oy = hy;

	POINT pt = {hx, hy};
	m_StartMatrix.Transform(pt, hx, hy);

	// get delta of move
	int dx = x - hx;	/* positive is a move to the right */
	int dy = y - hy;	/* positive is a move to the bottom */

	m_Matrix = m_StartMatrix;
	int minx = m_StartRect.left;
	int maxx = m_StartRect.right;
	int miny = m_StartRect.top;
	int maxy = m_StartRect.bottom;

	// get center of handle positions
	int midx = (minx + maxx)/2;
	int midy = (miny + maxy)/2;

	int length;
	if (bConstrainY)
	{	// If Constraining Y, Shear in X-Direction
		if (hx == minx || hx == maxx) // left or right side
		{
			if (hy > midy)	// bottom side
			{
				length = hy - miny;
			}
			else
			{	// top side
				length = maxy - hy;
				dx = -dx;
			}
		}
		else
		if (hy == miny)	// top side
		{
			length = maxy - miny;
			dx = -dx;
		}
		else
		if (hy == maxy)   // bottom side
		{
			length = hy - miny;
		}
		else
		{ // the point is not at any extreme
			if (hy > midy)	// bottom side
			{
				length = hy - miny;
			}
			else
			{	// top side
				length = maxy - hy;
			dx = -dx;
				}
		}
		int inc;
		if (dx < 0)
			inc = -1;
		else
			inc = 1;
		int count = dx/inc;
		while (--count >= 0)
		{
			m_Matrix.ShearX(length, inc);
			dx -= inc;
		}
		m_Matrix.ShearX(length, dx);
	}
	else
	{ // if Constraining X, shear in Y-Direction
		if (hy == miny || hy == maxy)	// top or bottom side
		{
			if (hx > midx)	// right side
			{
				length = hx - minx;
			}
			else
			{	// left side
				length = maxx - hx;
				dy = -dy;
			}
		}
		else
		if (hx == minx)	// left side
		{
			length = maxx - hx;
			dy = -dy;
		}
		else
		if (hx == maxx)	// right side
		{
			length = hx - minx;
		}
		else
		{ // the point is not at any extreme
			if (hx > midx)	// right side
			{
				length = hx - minx;
			}
			else
			{	// left side
				length = maxx - hx;
				dy = -dy;
			}
		}
		int inc;
		if (dy < 0)
			inc = -1;
		else
			inc = 1;
		int count = dy/inc;
		while (--count >= 0)
		{
			m_Matrix.ShearY(length, inc);
			dy -= inc;
		}
		m_Matrix.ShearY(length, dy);
	}

	pt.x = ox;
	pt.y = oy;
	double fhx, fhy;
	m_Matrix.Transform(pt, fhx, fhy);
	if (!bConstrainX)
		fhx = x;
	if (!bConstrainY)
		fhy = y;
	double fnx, fny;
	m_Matrix.Transform(pt, fnx, fny);
	double fmx = fhx - fnx;
	double fmy = fhy - fny;
	m_Matrix.Translate(fmx, fmy);
}

//////////////////////////////////////////////////////////////////////
void CTrack::PauseDrawing()
{
	if (m_bIsDrawn)
		m_bIsDrawn = false;
}

//////////////////////////////////////////////////////////////////////
void CTrack::ResumeDrawing()
{
	m_bIsDrawn = true;
}

//////////////////////////////////////////////////////////////////////
void CTrack::Paint(HDC hDC)
{
	if (m_bIsHighlight)
		Highlight(true, hDC, m_bHash);

	if (m_bIsDrawn)
		Draw(true/*bOn*/, hDC);
}

//////////////////////////////////////////////////////////////////////
void CTrack::StopHighlight()
{
	//Highlight(false, NULL);
	m_bIsHighlight = false;
}

//////////////////////////////////////////////////////////////////////
void CTrack::Highlight(bool bOn, HDC hDC, bool bHash)
{
	if (!m_pAGDC)
		return;

	m_bIsHighlight = bOn;
	m_bHash = bHash;

	if (!hDC)
		hDC = m_pAGDC->GetHDC();

	POINT pt[4];
	pt[0] = m_Distort.p[0];
	pt[1] = m_Distort.p[1];
	pt[2] = m_Distort.p[2];
	pt[3] = m_Distort.p[3];

	CAGMatrix Matrix = m_Matrix * m_ViewToDeviceMatrix;

	if (!(m_iWhatCanDo & TR_DRAWASLINE))
	{
		RECT Offset = {-1, -1, 0, 0};
		Matrix.Inverse().Transform((POINT*)&Offset, 2, false);

		pt[0].x += Offset.left;
		pt[0].y += Offset.top;
		pt[1].x += Offset.right;
		pt[1].y += Offset.top;
		pt[2].x += Offset.right;
		pt[2].y += Offset.bottom;
		pt[3].x += Offset.left;
		pt[3].y += Offset.bottom;
	}

	Matrix.Transform(pt[0]);
	Matrix.Transform(pt[1]);
	Matrix.Transform(pt[2]);
	Matrix.Transform(pt[3]);

	// Before drawing the transform border, open up the clipping to include a border pixel
	HRGN hClipRegion = m_pAGDC->SaveClipRegion();
	RECT rClipRect = m_PageRect;	
	::InflateRect(&rClipRect, 1, 1); // Add a border pixel
	m_pAGDC->SetClipRect(rClipRect, false/*bIncludeRawDC*/);

	HlineFlash(hDC, pt[0].x, pt[0].y, pt[1].x, pt[1].y, m_bHash);
	HlineFlash(hDC, pt[1].x, pt[1].y, pt[2].x, pt[2].y, m_bHash);
	HlineFlash(hDC, pt[2].x, pt[2].y, pt[3].x, pt[3].y, m_bHash);
	HlineFlash(hDC, pt[3].x, pt[3].y, pt[0].x, pt[0].y, m_bHash);

	m_pAGDC->RestoreClipRegion(hClipRegion);
}

//////////////////////////////////////////////////////////////////////
void CTrack::Draw(bool bOn, HDC hDC)
{
	if (!m_pAGDC)
		return;

	m_bIsDrawn = bOn;

	if (!hDC)
		hDC = m_pAGDC->GetHDC();

	POINT pt[4];
	pt[0] = m_Distort.p[0];
	pt[1] = m_Distort.p[1];
	pt[2] = m_Distort.p[2];
	pt[3] = m_Distort.p[3];

	CAGMatrix Matrix = m_Matrix * m_ViewToDeviceMatrix;

	// A trick to draw the selection rectangle outside of a transformed object
	if (!(m_iWhatCanDo & TR_DRAWASLINE))
	{
		RECT Offset = {-1, -1, 0, 0};
		Matrix.Inverse().Transform((POINT*)&Offset, 2, false);

		pt[0].x += Offset.left;
		pt[0].y += Offset.top;
		pt[1].x += Offset.right;
		pt[1].y += Offset.top;
		pt[2].x += Offset.right;
		pt[2].y += Offset.bottom;
		pt[3].x += Offset.left;
		pt[3].y += Offset.bottom;
	}

	Matrix.Transform(pt[0]);
	Matrix.Transform(pt[1]);
	Matrix.Transform(pt[2]);
	Matrix.Transform(pt[3]);

	// Before drawing the transform border, open up the clipping to include a border pixel
	HRGN hClipRegion = m_pAGDC->SaveClipRegion();
	RECT rClipRect = m_PageRect;	
	::InflateRect(&rClipRect, 1, 1); // Add a border pixel
	m_pAGDC->SetClipRect(rClipRect, false/*bIncludeRawDC*/);

	if (m_iWhatCanDo & TR_DRAWASLINE)
	{
		Dline(hDC, pt[0].x, pt[0].y, pt[2].x, pt[2].y, bOn);
	}
	else
	{
		Hline(hDC, pt[0].x, pt[0].y, pt[1].x, pt[1].y, bOn);
		Hline(hDC, pt[1].x, pt[1].y, pt[2].x, pt[2].y, bOn);
		Hline(hDC, pt[2].x, pt[2].y, pt[3].x, pt[3].y, bOn);
		Hline(hDC, pt[3].x, pt[3].y, pt[0].x, pt[0].y, bOn);
	}

	DrawHandles(hDC, bOn, pt);

	m_pAGDC->RestoreClipRegion(hClipRegion);
}

//////////////////////////////////////////////////////////////////////
void CTrack::DrawHandles(HDC hDC, bool bOn, POINT* pCorners)
{
	POINT ptRotate = {0,0};
	POINT ptCenter = {0,0};

	for (int h = 0; h < NUM_HANDLES; h++)
	{
		if (!m_iHandleInUse[h])
			continue;

		switch (h)
		{
			case H_UL:
			case H_UR:
			case H_LR:
			case H_LL:
			{
				int i = h - H_UL;
				m_hHandleRects[h].left = pCorners[i].x;
				m_hHandleRects[h].top  = pCorners[i].y;
				break;
			}
			case H_TOP:
			{
				m_hHandleRects[h].left = (pCorners[0].x + pCorners[1].x)/2;
				m_hHandleRects[h].top  = (pCorners[0].y + pCorners[1].y)/2;
				break;
			}
			case H_RIGHT:
			{
				m_hHandleRects[h].left = (pCorners[1].x + pCorners[2].x)/2;
				m_hHandleRects[h].top  = (pCorners[1].y + pCorners[2].y)/2;
				break;
			}
			case H_BOTTOM:
			{
				m_hHandleRects[h].left = (pCorners[2].x + pCorners[3].x)/2;
				m_hHandleRects[h].top  = (pCorners[2].y + pCorners[3].y)/2;
				break;
			}
			case H_LEFT:
			{
				m_hHandleRects[h].left = (pCorners[3].x + pCorners[0].x)/2;
				m_hHandleRects[h].top  = (pCorners[3].y + pCorners[0].y)/2;
				break;
			}
			case H_ROTATE:
			{
				m_Matrix.Transform(m_ptRotate, m_hHandleRects[h].left, m_hHandleRects[h].top);
				m_ViewToDeviceMatrix.Transform((POINT&)m_hHandleRects[h].left);
				ptRotate.x = m_hHandleRects[h].left;
				ptRotate.y = m_hHandleRects[h].top;
				break;
			}
			case H_CENTER:
			{
				m_Matrix.Transform(m_ptCenter, m_hHandleRects[h].left, m_hHandleRects[h].top);
				m_ViewToDeviceMatrix.Transform((POINT&)m_hHandleRects[h].left);
				ptCenter.x = m_hHandleRects[h].left;
				ptCenter.y = m_hHandleRects[h].top;
				break;
			}
			case H_CORNER_UL:
			case H_CORNER_UR:
			case H_CORNER_LR:
			case H_CORNER_LL:
			{
				int i = h - H_CORNER_UL;
				m_Matrix.Transform(m_Distort.p[i], m_hHandleRects[h].left, m_hHandleRects[h].top);
				m_ViewToDeviceMatrix.Transform((POINT&)m_hHandleRects[h].left);
				break;
			}
			default:
				break;
		}

		m_hHandleRects[h].left -= (H_SIZE/2);
		m_hHandleRects[h].top  -= (H_SIZE/2);
		m_hHandleRects[h].right  = m_hHandleRects[h].left + H_SIZE;
		m_hHandleRects[h].bottom = m_hHandleRects[h].top + H_SIZE;
		DrawHandle(hDC, &m_hHandleRects[h], h >= H_CENTER, bOn);
	}

	if (ptRotate.x || ptRotate.y || ptCenter.x || ptCenter.y)
		Dline(hDC, ptCenter.x, ptCenter.y, ptRotate.x, ptRotate.y, bOn);
}

//////////////////////////////////////////////////////////////////////
void CTrack::DrawHandle(HDC hDC, RECT* pRect, bool fCircle, bool bOn)
{
	if (fCircle)
	{
		Dellipse(hDC, pRect, bOn);
	}
	else
	{
		for (int y = pRect->top; y <= pRect->bottom; ++y)
			Dline(hDC, pRect->left, y, pRect->right+1, y, bOn);
	}
}

//////////////////////////////////////////////////////////////////////
void CTrack::Dline(HDC hDC, int x1, int y1, int x2, int y2, bool bOn)
{
	int OldROP = ::SetROP2(hDC, R2_BLACK);
	::MoveToEx(hDC, x1, y1, NULL);
	::LineTo(hDC, x2, y2);
	::SetROP2(hDC, OldROP);
}

//////////////////////////////////////////////////////////////////////
void CTrack::Hline(HDC hDC, int x1, int y1, int x2, int y2, bool bOn)
{
	HBITMAP hBitmap = ::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDB_HATCH_BORDER_1));
	LOGBRUSH lb;
	lb.lbStyle = BS_PATTERN;
	lb.lbColor = RGB(0,0,0);
	lb.lbHatch = (int)hBitmap;  

	HPEN hPen = ::ExtCreatePen(PS_GEOMETRIC, LINE_THICKNESS, &lb, 0, NULL);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);

	int OldROP = ::SetROP2(hDC, R2_MERGEPENNOT);
	::MoveToEx(hDC, x1, y1, NULL);
	::LineTo(hDC, x2, y2);
	::SetROP2(hDC, OldROP);

	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBitmap);
}

//////////////////////////////////////////////////////////////////////
void CTrack::HlineFlash(HDC hDC, int x1, int y1, int x2, int y2, bool bOn)
{
	HBITMAP hBitmap = ::LoadBitmap(_AtlBaseModule.GetResourceInstance(), (bOn?MAKEINTRESOURCE(IDB_HATCH_BORDER_1):MAKEINTRESOURCE(IDB_HATCH_BORDER)));
	LOGBRUSH lb;
	lb.lbStyle = BS_PATTERN;
	lb.lbColor = RGB(0,0,0);
	lb.lbHatch = (int)hBitmap;  

	HPEN hPen = ::ExtCreatePen(PS_GEOMETRIC, LINE_THICKNESS, &lb, 0, NULL);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);

	int OldROP = ::SetROP2(hDC, R2_COPYPEN); //R2_MERGEPENNOT);
	::MoveToEx(hDC, x1, y1, NULL);
	::LineTo(hDC, x2, y2);
	::SetROP2(hDC, OldROP);

	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBitmap);
}

//////////////////////////////////////////////////////////////////////
void CTrack::Dellipse(HDC hDC, RECT* pRect, bool bOn)
{
	int OldROP = ::SetROP2(hDC, R2_NOT);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, ::GetStockObject(NULL_BRUSH));
	::Ellipse(hDC, pRect->left, pRect->top, pRect->right+1, pRect->bottom+1);
	::SelectObject(hDC, hOldBrush);
	::SetROP2(hDC, OldROP);
}

#ifdef NOTUSED //j
//////////////////////////////////////////////////////////////////////
int CTrack::ClosestDistortPoint(int index, int iMode)
{
	if (iMode == 'x')
	{
		switch (index)
		{
			case 0:
				return 3;
			case 1:
				return 2;
			case 2:
				return 1;
			case 3:
				return 0;
		}
	}
	else // if (iMode == 'y')
	{
		switch (index)
		{
			case 0:
				return 1;
			case 1:
				return 0;
			case 2:
				return 3;
			case 3:
				return 2;
		}
	}

	int iDelta = INT_MAX;
	int iClosest = 0;
	for (int i=0; i<4; i++)
	{
		if (i == index)
			continue;
		int delta;
		if (iMode == 'x')
			delta = abs(m_Distort.p[index].x - m_Distort.p[i].x);
		else // if (iMode == 'y')
			delta = abs(m_Distort.p[index].y - m_Distort.p[i].y);
		if (delta < iDelta)
		{
			iClosest = i;
			iDelta = delta;
		}
	}

	return iClosest;
}

//////////////////////////////////////////////////////////////////////
void CTrack::GetForwardDistort(DISTORT* pDistort)
{
	DISTORT d;
	d.RectOrig = m_Distort.RectOrig;
	d.p[0] = m_Distort.p[0];
	d.p[1] = m_Distort.p[1];
	d.p[2] = m_Distort.p[2];
	d.p[3] = m_Distort.p[3];

	m_Matrix.Transform(d.p[0]);
	m_Matrix.Transform(d.p[1]);
	m_Matrix.Transform(d.p[2]);
	m_Matrix.Transform(d.p[3]);

	d.Rect.left   = min(min(min(d.p[0].x, d.p[1].x), d.p[2].x), d.p[3].x);
	d.Rect.right  = max(max(max(d.p[0].x, d.p[1].x), d.p[2].x), d.p[3].x);
	d.Rect.top	  = min(min(min(d.p[0].y, d.p[1].y), d.p[2].y), d.p[3].y);
	d.Rect.bottom = max(max(max(d.p[0].y, d.p[1].y), d.p[2].y), d.p[3].y);
	*pDistort = d;
}

//////////////////////////////////////////////////////////////////////
void CTrack::MapOriginalRect(DISTORT* pDistort, RECT* pSrcRect, RECT* pDstRect)
{
	POINT p0 = {pSrcRect->left, pSrcRect->top};
	MapOriginalPoint(pDistort, &p0, &p0);
	POINT p1 = {pSrcRect->right, pSrcRect->top};
	MapOriginalPoint(pDistort, &p1, &p1);
	POINT p2 = {pSrcRect->right, pSrcRect->bottom};
	MapOriginalPoint(pDistort, &p2, &p2);
	POINT p3 = {pSrcRect->left, pSrcRect->bottom};
	MapOriginalPoint(pDistort, &p3, &p3);

	pDstRect->left	 = min(min(min(p0.x, p1.x), p2.x), p3.x);
	pDstRect->right	 = max(max(max(p0.x, p1.x), p2.x), p3.x);
	pDstRect->top	 = min(min(min(p0.y, p1.y), p2.y), p3.y);
	pDstRect->bottom = max(max(max(p0.y, p1.y), p2.y), p3.y);
}

//////////////////////////////////////////////////////////////////////
void CTrack::MapOriginalPoint(DISTORT* pDistort, POINT* lpSrc, POINT* lpDst)
{
	// Compute p0 and p2 based on the X percentage

	DISTORT& Distort = *pDistort;
	int Numer = lpSrc->x - Distort.RectOrig.left;
	int Denom = Distort.RectOrig.right - Distort.RectOrig.left;

	POINT p0, p1, p2, p3;
	if (Numer == Denom /*UNITY*/)
	{
		p0  = Distort.p[1];
		p2  = Distort.p[2];
	}
	else
	if (!Numer)
	{
		p0  = Distort.p[0];
		p2  = Distort.p[3];
	}
	else
	{
		double fPercentX = (double)Numer / Denom;
		p0.x  = Distort.p[0].x + ((Distort.p[1].x - Distort.p[0].x) * fPercentX);
		p0.y  = Distort.p[0].y + ((Distort.p[1].y - Distort.p[0].y) * fPercentX);
		p2.x  = Distort.p[2].x + ((Distort.p[3].x - Distort.p[2].x) * (-fPercentX+1.0));
		p2.y  = Distort.p[2].y + ((Distort.p[3].y - Distort.p[2].y) * (-fPercentX+1.0));
	}

	// Compute P1 and p3 based on the Y percentage

	Numer = lpSrc->y - Distort.RectOrig.top;
	Denom = Distort.RectOrig.bottom - Distort.RectOrig.top;

	if (Numer == Denom /*UNITY*/)
	{
		p1  = Distort.p[2];
		p3  = Distort.p[3];
	}
	else
	if (!Numer)
	{
		p1  = Distort.p[1];
		p3  = Distort.p[0];
	}
	else
	{
		double fPercentY = (double)Numer / Denom;
		p1.x  = Distort.p[1].x + ((Distort.p[2].x - Distort.p[1].x) * fPercentY);
		p1.y  = Distort.p[1].y + ((Distort.p[2].y - Distort.p[1].y) * fPercentY);
		p3.x  = Distort.p[3].x + ((Distort.p[0].x - Distort.p[3].x) * (-fPercentY+1.0));
		p3.y  = Distort.p[3].y + ((Distort.p[0].y - Distort.p[3].y) * (-fPercentY+1.0));
	}

	// Compute the points at the same x and y percentages of each of the 4 segments
	// The intersection of P02 and P13 is our new point
	IntersectSegments(p0, p2, p1, p3, lpDst);
}

//////////////////////////////////////////////////////////////////////
void CTrack::IntersectSegments(POINT A0, POINT A1, POINT B0, POINT B1, POINT* lpPt)
{ // Compute the intersection of segment A and segment B
	int dxA, dyA, dxB, dyB;
	if ((dxA = A1.x - A0.x) < 0) dxA = -dxA;
	if ((dyA = A1.y - A0.y) < 0) dyA = -dyA;
	if ((dxB = B1.x - B0.x) < 0) dxB = -dxB;
	if ((dyB = B1.y - B0.y) < 0) dyB = -dyB;
	if (min(dxA, dyB) < min(dyA, dxB)) // In the quest for accuracy
	{ // swap the segments
		POINT T;
		T = A0; A0 = B0; B0 = T;
		T = A1; A1 = B1; B1 = T;
	}

	dxA = A1.x - A0.x;
	dyA = A1.y - A0.y;
	dxB = B1.x - B0.x;
	dyB = B1.y - B0.y;

	if (!dxA || !dyB)
	{ // If the slope is still undefined, get out
		*lpPt = A0;
		return;
	}

	double fSlopeA = (double)dyA / (double)dxA;
	double fSlopeB = (double)dxB / (double)dyB;
	double fxAB = B0.x - A0.x;
	double fyAB = B0.y - A0.y;
	double fNumer = fxAB - (fyAB * fSlopeB);
	double fDenom = 1.0 - (fSlopeA * fSlopeB);
	int dx = (fNumer / fDenom);
	lpPt->x = A0.x + dx;
	lpPt->y = A0.y + (dx * fSlopeA);
}

#endif NOTUSED //j

//////////////////////////////////////////////////////////////////////
static bool ConstrainXY(LPINT lpX, LPINT lpY, bool bButtonDown, bool bInit, bool bActive)
{
	static bool ConstrainLooking;
	static POINT Constrain;

	int x = *lpX;
	int y = *lpY;
	if (bInit)
	{
		m_bConstrainX = m_bConstrainY = false;
		ConstrainLooking = bActive;
		Constrain.x = x;
		Constrain.y = y;
		return false;
	}

	if (!bButtonDown)
	{
		m_bConstrainX = m_bConstrainY = false;
		ConstrainLooking = false;
		return false;
	}

	if (ConstrainLooking)
	{
		int dx = abs(Constrain.x - x);
		int dy = abs(Constrain.y - y);
		if (dx > 1 || dy > 1)
		{
			ConstrainLooking = false;
			if (dx > dy)
				m_bConstrainY = true;
			else
				m_bConstrainX = true;
		}
		else
		{
			x = Constrain.x;
			y = Constrain.y;
		}
	}

	if (m_bConstrainX)
		x = Constrain.x;
	if (m_bConstrainY)
		y = Constrain.y;
	*lpX = x;
	*lpY = y;

	return false;
}

