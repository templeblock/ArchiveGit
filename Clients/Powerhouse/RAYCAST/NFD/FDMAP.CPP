/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// fdmap.cpp : implementation file
//

#include "stdafx.h"
#include "fd.h"
#include "fdmap.h"
#include "fddoc.h"
#include "fdview.h"
#include "utils.h"
#include "ack\ack3d.h"
#include "ack\ackeng.h"
#include "ack\ackext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CFdView			*OurView;
extern CPalette			*CPalPtr;
extern int far			sysNames[];
extern COLORREF far	sysColors[];

extern CFdMap				*fdm;

extern ACKENG far		*ae;

int					xMag = FDM_WIDTH / 32;
int					yMag = FDM_HEIGHT / 32;
int					mLeft=0, mTop=0;

/////////////////////////////////////////////////////////////////////////////
// CFdMap::DrawMap

void CFdMap::DrawMap(void)
{
	if (!fdm) return;
int			i,j,k,l,x,y,pos;
UINT		grid,far *xg,far *yg;
CBrush	sb;
CDC			*pDC=GetDC();
COLORREF	cr;
RECT		rect;

	pDC->SelectPalette(CPalPtr,FALSE);
	pDC->RealizePalette();
	pDC->SetBkColor(PALETTEINDEX(240));

	//	Draw all seen Walls and Doors
	for (i=0,y=0 ; i < 32 ; i++)
	{
		xg = &ae->xGrid[mLeft+(mTop+i)*GRID_WIDTH];
		yg = &ae->yGrid[mLeft+(mTop+i)*GRID_HEIGHT];
		for (j=0,x=0 ; j < 32 ; j++)
		{
			grid = *xg++;
			if (grid & WALL_WAS_SEEN)
			{
				if ((grid & 0xff) >= DOOR_SIDECODE)
				{	//	Special color for door
					cr = PALETTEINDEX(127);
				}
				else
				{	//	Regular wall
					cr = PALETTEINDEX(31);
				}
				for (k=y,l=y+yMag ; k < l ; k++)
					pDC->SetPixel(x, k, cr);
			}
			grid = *yg++;
			if (grid & WALL_WAS_SEEN)
			{
				if ((grid & 0xff) >= DOOR_SIDECODE)
				{	//	Special color for door
					cr = PALETTEINDEX(127);
				}
				else
				{	//	Regular wall
					cr = PALETTEINDEX(31);
				}
				for (k=x,l=x+xMag ; k < l ; k++)
					pDC->SetPixel(k, y, cr);
			}
			x += xMag;
		}
		y += yMag;
	}
	
	//	Draw all seen Objects
	sb.CreateSolidBrush(PALETTEINDEX(15));	//	Red
	for (i=1 ; i < ae->MaxObjects ; i++)
	{
		if (!ae->ObjList[i].Active ||
				!(ae->ObjList[i].Flags & OF_WAS_SEEN) ||
				!ObjList[ae->ObjList[i].bmNum]->Speed)
			continue;
		pos = ae->ObjList[i].mPos;

		y = (pos & 0xFC0) >> 6;
		if (y < mTop || y >= (mTop + 32)) continue;

		x = pos & 0x3F;
		if (x < mLeft || x >= (mLeft + 32)) continue;

		rect.left = (x - mLeft) * xMag + 1;
		rect.top = (y - mTop) * yMag + 1;
		rect.right = rect.left + 2;
		rect.bottom = rect.top + 2;
		pDC->FillRect(&rect,&sb);
	}
	sb.DeleteObject();

	//	Draw Player
	sb.CreateSolidBrush(PALETTEINDEX(255));	//	Blue
	do
	{
		pos = ae->ObjList[0].mPos;

		y = (pos & 0xFC0) >> 6;
		if (y < mTop || y >= (mTop + 32)) continue;

		x = pos & 0x3F;
		if (x < mLeft || x >= (mLeft + 32)) continue;

		rect.left = (x - mLeft) * xMag + 1;
		rect.top = (y - mTop) * yMag + 1;
		rect.right = rect.left + 2;
		rect.bottom = rect.top + 2;
		pDC->FillRect(&rect,&sb);
	}
	while (0);
	sb.DeleteObject();

	ReleaseDC(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CFdMap

CFdMap::CFdMap(HWND pParent,RECT rect)
{
int		w=(rect.right - rect.left) + 1;
int		h=(rect.bottom - rect.top) + 1;

	CreateEx(0,NULL,"WinAdv Map",
		WS_POPUP|WS_BORDER|WS_CAPTION|WS_VISIBLE|WS_CHILD,
		rect.left,rect.top,w,h,pParent,NULL,NULL);
}

CFdMap::~CFdMap()
{
}

BEGIN_MESSAGE_MAP(CFdMap, CWnd)
	//{{AFX_MSG_MAP(CFdMap)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFdMap message handlers

int CFdMap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CFdMap::OnDestroy()
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CFdMap::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
RECT	rect;
	GetClientRect(&rect);
	dc.SelectPalette(CPalPtr,FALSE);
	dc.RealizePalette();
	dc.PatBlt(rect.left,rect.top,rect.right-rect.left+1,rect.bottom-rect.top+1,BLACKNESS);
	DrawMap();
	if (OurView) OurView->SetFocus();
	// Do not call CWnd::OnPaint() for painting messages
}

void CFdMap::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	if (pNewWnd)
	if (pNewWnd->GetParentFrame() == GetParentFrame()) return;
CDC		*pDC=GetDC();
	SetSystemPaletteUse(pDC->m_hDC, SYSPAL_STATIC);
   	CPalPtr->UnrealizeObject();
	SetSysColors(21, sysNames, sysColors);
	ReleaseDC(pDC);
}

void CFdMap::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	InvalidateRect(NULL,FALSE);
	if (pOldWnd)
	if (pOldWnd->GetParentFrame() == GetParentFrame()) return;
int		nMapped;
CDC		*pDC=GetDC();
	SetSystemPaletteUse(pDC->m_hDC, SYSPAL_NOSTATIC);
    CPalPtr->UnrealizeObject();
	pDC->SelectPalette(CPalPtr, FALSE);
	nMapped = pDC->RealizePalette();
	TRACE("Number of entries mapped at CFdMap::OnSetFocus = %d\n", nMapped);
	ReleaseDC(pDC);
}

void CFdMap::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);
	
	// TODO: Add your message handler code here
RECT	rect;
	if (!IsIconic())
	{
		GetWindowRect(&rect);
		AfxGetApp()->WriteProfileInt("Preferences", "Map_X", rect.left);
		AfxGetApp()->WriteProfileInt("Preferences", "Map_Y", rect.top);
	}
}

void CFdMap::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if ((cx <= 0) || (cy <= 0)) return;
	TRACE("CFdMap::OnSize: nType = %d, cx = %d, cy = %d\n", nType, cx, cy);
}

void CFdMap::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	mLeft = (mLeft + (point.x / xMag)) - 16;
	if (mLeft < 0) mLeft = 0;
	if (mLeft > 32) mLeft = 32;

	mTop = (mTop + (point.y / yMag)) - 16;
	if (mTop < 0) mTop = 0;
	if (mTop > 32) mTop = 32;

	CWnd::OnLButtonDown(nFlags, point);
}
