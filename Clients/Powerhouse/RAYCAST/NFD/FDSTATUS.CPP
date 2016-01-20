/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// fdstatus.cpp : implementation file
//

#include "stdafx.h"
#include <windowsx.h>
#include "fd.h"
#include "fdstatus.h"
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

#define MIN_HEART_RATE	9
#define MAX_HEART_RATE	4

extern BOOL				fPaused;
extern int				fdsWidth, fdsHeight;
extern int				IdleTicks;
extern int				HeartRate;

extern CFdView		*OurView;
extern CPalette		*CPalPtr;
extern int far		sysNames[];
extern COLORREF far	sysColors[];

extern CFdStatus	*fds;

extern ACKENG far	*ae;

extern int				DocAction;

long	long_divide(long denom,long divisor);

PDIB			pDib;		//	Pointer to Device Independent Bitmap
CBitmap		*pBitmap[4];
char			*bmNames[]={"bmp\\status4.bmp",
						"bmp\\lredkey1.bmp",
						"bmp\\lgrnkey1.bmp",
						"bmp\\lblukey1.bmp"};

////////////////////////////////////////////////////////////////////////////
// CFdStatus::TextHealth

void CFdStatus::TextHealth(void)
{
	if (!fds) return;
char			txt[10]={"DEAD"};
long			tmp;
CBrush		sb;
CDC				*pDC=GetDC();
COLORREF	cr;
RECT			rect;

	tmp = long_divide(ae->ObjList[PLAYER_OBJECT].Health * 100L,MAX_HEALTH);
	if (tmp > 100L) tmp = 100L;

	HeartRate = (int)(MAX_HEART_RATE + long_divide(tmp,20L));
	if (HeartRate > MIN_HEART_RATE) HeartRate = MIN_HEART_RATE;

	pDC->SelectPalette(CPalPtr,FALSE);
	pDC->RealizePalette();
	pDC->SetBkColor(PALETTEINDEX(219));
	sb.CreateSolidBrush(PALETTEINDEX(219));

	//	Clear old Health number
	rect.left=60;rect.top=9;rect.right=100;rect.bottom=23;
	pDC->FillRect(&rect,&sb);

	
	//	Write Helath number
	if (ae->ObjList[PLAYER_OBJECT].Health >= 0)
		wsprintf(txt,"%ld",tmp);
	pDC->SetTextColor(PALETTEINDEX(240));
	pDC->DrawText(txt,-1,&rect,DT_RIGHT);

	//	Clear old Health graph
	rect.left=12;rect.top=24;rect.right=100;rect.bottom=30;
	pDC->FillRect(&rect,&sb);
	sb.DeleteObject();

	//	Draw Health graph
	if (tmp <= 20)
		cr = PALETTEINDEX(12);	//	RED
	else
		cr = PALETTEINDEX(24);	//	GREEN
	rect.right=rect.left+(int)long_divide(tmp * 88L,100L);
	sb.CreateSolidBrush(cr);
	pDC->FillRect(&rect,&sb);
	sb.DeleteObject();

	//	Draw border around graph
	rect.left=11;rect.top=23;rect.right=101;rect.bottom=31;
	sb.CreateSolidBrush(PALETTEINDEX(240));
	pDC->FrameRect(&rect,&sb);
	sb.DeleteObject();

	ReleaseDC(pDC);
}

////////////////////////////////////////////////////////////////////////////
// CFdStatus::TextAmmo

void CFdStatus::TextAmmo(void)
{
	if (!fds) return;
int		tmp;
char	txt[30]={"---"};
CBrush	sb;
CDC		*pDC=GetDC();
RECT	rect;
	tmp = *ae->WeaponsCarried[ObjList[PLAYER_OBJECT]->Weapon].Ammo;

	pDC->SelectPalette(CPalPtr,FALSE);
	pDC->RealizePalette();
	pDC->SetBkColor(PALETTEINDEX(219));
	sb.CreateSolidBrush(PALETTEINDEX(219));

	rect.left=60;rect.top=37;rect.right=100;rect.bottom=52;
	pDC->FillRect(&rect,&sb);

    if (tmp != -1)
		wsprintf(txt,"%d",tmp);
	pDC->SetTextColor(PALETTEINDEX(240));
	pDC->DrawText(txt,-1,&rect,DT_RIGHT);

	rect.left=10;rect.top=58;rect.right=150;rect.bottom=74;
	pDC->FillRect(&rect,&sb);
             
	txt[0] = 0;
	if (!ae->WeaponsCarried[ObjList[PLAYER_OBJECT]->Weapon].Possessed)
		lstrcpy(txt, "No ");
	lstrcat(txt, Weapons[ObjList[PLAYER_OBJECT]->Weapon].Name);
	pDC->DrawText(txt,-1,&rect,DT_CENTER);

	sb.DeleteObject();
	ReleaseDC(pDC);
}

////////////////////////////////////////////////////////////////////////////
// CFdStatus::TextAmmo

void CFdStatus::TextMsg(char *msg)
{
	if (!fds) return;
CBrush	sb;
CDC		*pDC=GetDC();
RECT	rect;

	pDC->SelectPalette(CPalPtr,FALSE);
	pDC->RealizePalette();
	pDC->SetBkColor(PALETTEINDEX(219));

	rect.left=10;rect.top=75;rect.right=150;rect.bottom=91;

	sb.CreateSolidBrush(PALETTEINDEX(219));
	pDC->FillRect(&rect,&sb);

	pDC->SetTextColor(PALETTEINDEX(240));
	pDC->DrawText(msg,-1,&rect,DT_CENTER);

	sb.DeleteObject();
	ReleaseDC(pDC);
	
	IdleTicks = 0;
}

////////////////////////////////////////////////////////////////////////////
// CFdStatus::TextAmmo

void CFdStatus::DrawKeys(void)
{
	if (!fds) return;
int			result;
CBrush		sb;
CDC			pDC;
CBitmap		*oldbm;
CClientDC	dc(fds);
RECT		rect;

	pDC.CreateCompatibleDC(&dc);
	dc.SelectPalette(CPalPtr,FALSE);
	dc.RealizePalette();

	rect.left=109;rect.top=11;rect.right=129;rect.bottom=50;
	sb.CreateSolidBrush(PALETTEINDEX(219));
	dc.FillRect(&rect,&sb);
	sb.DeleteObject();

	if (ae->Keys & KEY_RED)
	{
		oldbm = pDC.SelectObject(pBitmap[1]);
		result = dc.StretchBlt(109, 11, 20, 10,
				&pDC, 0, 0, 20, 10, SRCCOPY);
		pDC.SelectObject(oldbm);
    }

	if (ae->Keys & KEY_GREEN)
	{
		oldbm = pDC.SelectObject(pBitmap[2]);
		result = dc.StretchBlt(109, 25, 20, 10,
				&pDC, 0, 0, 20, 10, SRCCOPY);
		pDC.SelectObject(oldbm);
    }

	if (ae->Keys & KEY_BLUE)
	{
		oldbm = pDC.SelectObject(pBitmap[3]);
		result = dc.StretchBlt(109, 39, 20, 10,
				&pDC, 0, 0, 20, 10, SRCCOPY);
		pDC.SelectObject(oldbm);
    }

	result = pDC.DeleteDC();
}

/////////////////////////////////////////////////////////////////////////////
// CFdStatus

CFdStatus::CFdStatus(HWND pParent,RECT rect)
{
int		w=(rect.right - rect.left) + 1;
int		h=(rect.bottom - rect.top) + 1;

	CreateEx(0,NULL,"WinAdv Status",
		WS_POPUP|WS_BORDER|WS_CAPTION|WS_VISIBLE|WS_CHILD,
		rect.left,rect.top,w,h,pParent,NULL,NULL);
}

////////////////////////////////////////////////////////////////////////////
// ~CFdStatus

CFdStatus::~CFdStatus()
{
}

BEGIN_MESSAGE_MAP(CFdStatus, CWnd)
	//{{AFX_MSG_MAP(CFdStatus)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFdStatus message handlers

////////////////////////////////////////////////////////////////////////////
// CFdStatus::OnCreate

int CFdStatus::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
int		i,result;
long	sz,bytes;
void	far	*tDib;

	for (i=0 ; i < 4 ; i++)
	{
		TRACE("Opening Status BMP file %s\n",bmNames[i]);
		pDib = DibOpenFile(bmNames[i]);
		ASSERT(pDib != NULL);
		sz = DibSizeImage(pDib);
		tDib = DibPtr(pDib);

		pBitmap[i] = new CBitmap();
		result = pBitmap[i]->CreateBitmap(DibWidth(pDib),DibHeight(pDib),1,8,NULL);
		bytes = pBitmap[i]->SetBitmapBits(sz,tDib);

		result = DibFree(pDib);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////
// CFdStatus::OnDestroy

void CFdStatus::OnDestroy()
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
int		i;

	for (i=0 ; i < 4 ; i++)
	{
		pBitmap[i]->DeleteObject();
		delete pBitmap[i];
	}
}
////////////////////////////////////////////////////////////////////////////
// CFdStatus::OnPaint

void CFdStatus::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
int		result;
CDC		pDC;
CBitmap	*oldbm;
RECT	rect;
	GetClientRect(&rect);
	pDC.CreateCompatibleDC(&dc);
	dc.SelectPalette(CPalPtr,FALSE);
	dc.RealizePalette();
	oldbm = pDC.SelectObject(pBitmap[0]);
	result = dc.StretchBlt(rect.left, rect.top,
				rect.right - rect.left + 1 ,rect.bottom - rect.top + 1,
				&pDC, 0, 0, fdsWidth, fdsHeight, SRCCOPY);
	pDC.SelectObject(oldbm);
	result = pDC.DeleteDC();
	TextHealth(); TextAmmo(); DrawKeys();
	if (DocAction)
	{
		if (DocAction == 1)
		{
			TextMsg("Game Loaded");
		}
		else
			TextMsg("Game Saved");
		DocAction = 0;
	}
	else
	if (fPaused) TextMsg("Game Paused");
	if (OurView) OurView->SetFocus();
	// Do not call CWnd::OnPaint() for painting messages
}

////////////////////////////////////////////////////////////////////////////
// CFdStatus::OnKillFocus

void CFdStatus::OnKillFocus(CWnd* pNewWnd)
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

////////////////////////////////////////////////////////////////////////////
// CFdStatus::OnSetFocus

void CFdStatus::OnSetFocus(CWnd* pOldWnd)
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
	TRACE("Number of entries mapped at CFdStatus::OnSetFocus = %d\n", nMapped);
	ReleaseDC(pDC);
}

////////////////////////////////////////////////////////////////////////////
// CFdStatus::OnMove

void CFdStatus::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);
	
	// TODO: Add your message handler code here
RECT	rect;
	if (!IsIconic())
	{
		GetWindowRect(&rect);
		AfxGetApp()->WriteProfileInt("Preferences", "Status_X", rect.left);
		AfxGetApp()->WriteProfileInt("Preferences", "Status_Y", rect.top);
	}
}

////////////////////////////////////////////////////////////////////////////
// CFdStatus::OnSize

void CFdStatus::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if ((cx <= 0) || (cy <= 0)) return;
	TRACE("CFdStatus::OnSize: nType = %d, cx = %d, cy = %d\n", nType, cx, cy);
}
