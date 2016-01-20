// octools.cpp : implementation of the CMainFrame class
//

#include "pp.h"
#include "afxpriv.h"
#include "limits.h"
#include "octools.h"
#include "ctxhelp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW
const TCHAR _afxWndControlBar[] = _T("AfxControlBar");

#define MAXINT	SHRT_MAX
#define Output  OutputDebugString

COCToolList* OCToolList = NULL;

/////////////////////////////////////////////////////////////////////////////
// COCButton
/////////////////////////////////////////////////////////////////////////////

COCButton::~COCButton()
{
}


void COCButton::Serialize(CArchive &ar)
{
	DWORD 			dw;

	if (ar.IsStoring())
	{
		ar << (DWORD)nID;
		ar << (DWORD)nStyle;
		ar << (DWORD)x;
		ar << (DWORD)y;
		ar << (DWORD)Size.cx;
		ar << (DWORD)Size.cy;
		ar << (DWORD)IDMasterBM;
		ar << Text;
		ar << ClassName;

	}
	else
	{
		ar >> dw;
		nID			= dw;
		ar >> dw;
		nStyle		= dw;
		ar >> dw;
		x			= dw;
		ar >> dw;
		y			= dw;
		ar >> dw;
		Size.cx		= dw;
		ar >> dw;
		Size.cy		= dw;
		ar >> dw;
		IDMasterBM	= dw;
		ar >> Text;
		ar >> ClassName;
	}
}

COCButton* COCButton::Duplicate()
{
	COCButton*	pBt = new COCButton();

	*pBt = *this;
	return(pBt);
}


// create a mono bitmap mask:
void COCButton::CreateMask(CPoint ptOffset, BOOL bHiliteShadow, int Dem)
{
	// initalize whole area with 0's
	PatBlt(OCToolList->hDCMono, 0, 0, Size.cx-2, Size.cy-2, WHITENESS);

	// create mask based on color bitmap
	// convert this to 1's
	SetBkColor(OCToolList->hDCGlyphs, OCToolList->clrBtnFace);
	if (nStyle & TBBS_TEXTBTN)
	{
		CRect	r = CRect(CPoint(ptOffset.x, ptOffset.y),CSize(Size.cx-2, Size.cy-2));
		DrawText(OCToolList->hDCMono, Text, r, OCToolList->clrBtnGrayText);
	}
	else
	{
		CSize	ISize =	CSize(Dem, Dem);

		BitBlt(OCToolList->hDCMono, ptOffset.x, ptOffset.y, ISize.cx, ISize.cy,
			OCToolList->hDCGlyphs, iImage * ISize.cx, 0, SRCCOPY);

		// convert this to 1's
		SetBkColor(OCToolList->hDCGlyphs, OCToolList->clrBtnHilite);

		// OR in the new 1's
		BitBlt(OCToolList->hDCMono, ptOffset.x, ptOffset.y, ISize.cx, ISize.cy,
			OCToolList->hDCGlyphs, iImage * ISize.cx, 0, SRCPAINT);

		if (bHiliteShadow)
			BitBlt(OCToolList->hDCMono, 1, 1, Size.cx-3, Size.cy-3,
				OCToolList->hDCMono, 0, 0, SRCAND);
	}
}

void COCButton::DrawText(HDC hDC, CString &Str, CRect &r, COLORREF color)
{
	HFONT Old = (HFONT)SelectObject(hDC, OCToolList->hfontSys.GetSafeHandle());
	SetTextColor(hDC, color);
	::DrawText(hDC, Str, Str.GetLength(), &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	SelectObject(hDC, Old);
}

// Raster Ops
#define ROP_DSPDxax  0x00E20746L
#define ROP_PSDPxax  0x00B8074AL

BOOL COCButton::DrawButton(CDC* pDC, int Dem)
{
	CSize		ISize =	CSize(Dem, Dem);
	int			x = Origin.x;
	int			y = Origin.y;
	HBRUSH		hBr;
	ASSERT_VALID(pDC);

	int dx = Size.cx;
	int dy = Size.cy;
	if (!OCToolList->bWin4)
	{
		// make the coordinates the interior of the button
		x += 1;
		y += 1;
		dx -= 2;
		dy -= 2;

		// border around button
		pDC->FillSolidRect(x,    y-1,    dx, 1,  OCToolList->clrWindowFrame);
		pDC->FillSolidRect(x,    y+dy,   dx, 1,  OCToolList->clrWindowFrame);
		pDC->FillSolidRect(x-1,  y,  1,  dy, OCToolList->clrWindowFrame);
		pDC->FillSolidRect(x+dx, y,  1,  dy, OCToolList->clrWindowFrame);
	}


	// interior grey
	pDC->FillSolidRect(x, y, dx, dy, OCToolList->clrBtnFace);

	// determine offset of bitmap (centered within button)
	CPoint ptOffset;
	if (nStyle & TBBS_TEXTBTN)
		ptOffset.x = ptOffset.y = 0;
	else
	{
		ptOffset.x = (dx - ISize.cx - 1) / 2;
		ptOffset.y = (dy - ISize.cy) / 2;
	}

	if (nStyle & (TBBS_PRESSED | TBBS_CHECKED))
	{
		// pressed in or checked
		pDC->Draw3dRect(x, y, dx, dy,
			OCToolList->bWin4 ? OCToolList->clrWindowFrame : OCToolList->clrBtnShadow,
			OCToolList->bWin4 ? OCToolList->clrBtnHilite : OCToolList->clrBtnFace);

		if (OCToolList->bWin4)
		{
			pDC->Draw3dRect(x + 1, y + 1, dx - 2, dy - 2,
				OCToolList->clrBtnShadow, OCToolList->clrBtnFace);
		}

		// for any depressed button, add one to the offsets.
		ptOffset.x += 1;
		ptOffset.y += 1;
	}
	else
	{
		// regular button look
		pDC->Draw3dRect(x, y, dx, dy, OCToolList->clrBtnHilite,
			OCToolList->bWin4 ? OCToolList->clrWindowFrame : OCToolList->clrBtnShadow);
		pDC->Draw3dRect(x + 1, y + 1, dx - 2, dy - 2,
			OCToolList->clrBtnFace, OCToolList->clrBtnShadow);
	}

	if ((nStyle & TBBS_PRESSED) || !(nStyle & TBBS_DISABLED))
	{
		// normal image version
		if (nStyle & TBBS_TEXTBTN)
		{
			CRect	r = CRect(CPoint(x + ptOffset.x, y + ptOffset.y),CSize(dx, dy));
			int		old = pDC->SetBkMode(TRANSPARENT);

			DrawText(pDC->m_hDC, Text, r, OCToolList->clrBtnText);
			pDC->SetBkMode(old);
		}
		else
		{
			BitBlt(pDC->m_hDC, x + ptOffset.x, y + ptOffset.y,
				ISize.cx, ISize.cy,
				OCToolList->hDCGlyphs, iImage * ISize.cx, 0, SRCCOPY);
		}

		if (nStyle & TBBS_PRESSED)
			return TRUE;        // nothing more to do (rest of style is ignored)
	}

	HBITMAP hbmMono = CreateBitmap(Size.cx-2, Size.cy-2,	1, 1, NULL);
	HBITMAP hbmMonoOld = (HBITMAP)SelectObject(OCToolList->hDCMono, hbmMono);

	if (nStyle & TBBS_DISABLED)
	{
		// disabled or indeterminate version
		CreateMask(ptOffset, FALSE, Dem);

		pDC->SetTextColor(0L);                  // 0's in mono -> 0 (for ROP)
		pDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1's in mono -> 1

		if (nStyle & TBBS_DISABLED)
		{
			// disabled - draw the hilighted shadow
			hBr = ::CreateSolidBrush(OCToolList->clrBtnHilite);

			HGDIOBJ hbrOld = pDC->SelectObject(hBr);
			if (hbrOld != NULL)
			{
				// draw hilight color where we have 0's in the mask
				BitBlt(pDC->m_hDC, x + 1, y + 1, Size.cx - 2, Size.cy - 2,
					OCToolList->hDCMono, 0, 0, ROP_PSDPxax);
				pDC->SelectObject(hbrOld);
				DeleteObject(hBr);
			}
		}

		hBr = ::CreateSolidBrush(OCToolList->clrBtnShadow);
		HGDIOBJ hbrOld = pDC->SelectObject(hBr);
		if (hbrOld != NULL)
		{
			// draw the shadow color where we have 0's in the mask
			BitBlt(pDC->m_hDC, x, y, Size.cx - 2, Size.cy - 2,
				OCToolList->hDCMono, 0, 0, ROP_PSDPxax);
			pDC->SelectObject(hbrOld);
			DeleteObject(hBr);
		}
	}

	// if it is checked do the dither brush avoiding the glyph
	if (nStyle & TBBS_CHECKED)
	{
		HGDIOBJ hbrOld = pDC->SelectObject(OCToolList->hbrDither);
		if (hbrOld != NULL)
		{
			ptOffset.x -= OCToolList->cxBorder2;
			ptOffset.y -= OCToolList->cyBorder2;
			CreateMask(ptOffset, nStyle & TBBS_DISABLED, Dem);

			pDC->SetTextColor(0L);              // 0 -> 0
			pDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1 -> 1

			ASSERT(OCToolList->cxBorder2 == OCToolList->cyBorder2);
			int delta = OCToolList->cxBorder2*2;

			// only draw the dither brush where the mask is 1's
			BitBlt(pDC->m_hDC,
				x + OCToolList->cxBorder2, y + OCToolList->cyBorder2, dx-delta, dy-delta,
				OCToolList->hDCMono, 0, 0, ROP_DSPDxax);
			pDC->SelectObject(hbrOld);
		}
	}
	SelectObject(OCToolList->hDCMono, hbmMonoOld);
	DeleteObject(hbmMono);

	return TRUE;
}

void COCButton::Move(CPoint Pt, BOOL Horz)
{
	Origin = Pt;
	if (nStyle & TBBS_WINDOW)		// Window
		SetWindowPos(hWnd, NULL, Pt.x, Pt.y, Size.cx, Size.cy, SWP_NOZORDER|SWP_NOACTIVATE);
}

COCButton::COCButton()
{
	int i = OCToolList->GetDefaultGap() + OCToolList->m_SharedBorder * 2;

	nID			= 0;
	nStyle		= TBBS_SEPARATOR;
	x			= 0;
	y			= 0;
	IDMasterBM	= 0;
	Size		= CSize(i,i);
	Origin      = CPoint(0,0);
	iImage		= 0;
	Text		= "";
	ClassName	= "";
	hWnd		= NULL;
}

COCButton::COCButton(UINT id, int xx, int yy, UINT BM)
{
	nID			= id;
	nStyle		= TBBS_BUTTON;
	x			= xx;
	y			= yy;
	IDMasterBM	= BM;
	Size		= OCToolList->GetButtonSize();
	iImage		= 0;
	Text		= "";
	Origin      = CPoint(0,0);
	ClassName	= "";
	hWnd		= NULL;
}

COCButton::COCButton(UINT id, CString UName, CString FName)
{
	nID			= id;
	nStyle		= TBBS_TEXTBTN;
	x			= 0;
	y			= 0;
	IDMasterBM	= 0;
	Size		= CSize(50, OCToolList->GetButtonSize().cy);
	iImage		= 0;
	Text		= UName;			// what the user see on the face of the button
	Origin      = CPoint(0,0);
	ClassName	= FName;			// the name of the macro file to run
	hWnd		= NULL;
}

COCButton::COCButton(UINT id, CString Str)
{
	nID			= id;
	nStyle		= TBBS_TEXTBTN;
	x			= 0;
	y			= 0;
	IDMasterBM	= 0;
	Size		= CSize(50, OCToolList->GetButtonSize().cy);
	iImage		= 0;
	Text		= Str;
	Origin      = CPoint(0,0);
	ClassName	= "";
	hWnd		= NULL;
}

COCButton::COCButton(HWND	w)
{
	CRect	r;

	nID			= 0;
	nStyle		= TBBS_DIALOG;
	x			= 0;
	y			= 0;
	IDMasterBM	= 0;
	iImage		= 0;
	Text		= "";
	Origin      = CPoint(0,0);
	ClassName	= "";
	hWnd		= w;
	GetWindowRect(hWnd, &r);
	Size		= CSize(r.Width(), r.Height());
}

const COCButton& COCButton::operator=(const COCButton& Src)
{
	nID			= Src.nID;
	nStyle		= Src.nStyle;
	x			= Src.x;
	y			= Src.y;
	IDMasterBM	= Src.IDMasterBM;
	Size		= Src.Size;
	iImage		= Src.iImage;
	Text		= Src.Text;
	Origin      = Src.Origin;
	ClassName	= Src.ClassName;
	hWnd		= Src.hWnd;
	return *this;
}

const COCButton& COCButton::operator=(const OCBUTTON &Src)
{
	nID			= Src.nID;
	x			= Src.x;
	y			= Src.y;
	IDMasterBM	= Src.IDMasterBM;
	nStyle		= TBBS_BUTTON;
	Size		= CSize(0,0);
	iImage		= 0;
	Text		= "";
	Origin      = CPoint(0,0);
	return *this;
}


void  COCButton::CalcSize(BOOL Large)
{
	CSize		BSize = OCToolList->GetButtonSize(Large);
	if (nStyle & TBBS_SEPARATOR)	   // separator,width includes 2 pixel overlap
		Size.cy = Size.cx = OCToolList->GetDefaultGap() + OCToolList->m_SharedBorder * 2;
	else if (nStyle & TBBS_TEXTBTN)		// text button
	{
		HFONT Old = (HFONT)SelectObject(OCToolList->hDCMono, OCToolList->hfontSys.GetSafeHandle());
		CRect	r = CRect(0,0,0,0);

		::DrawText(OCToolList->hDCMono, Text, Text.GetLength(), &r, 
		DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_CALCRECT);
		SelectObject(OCToolList->hDCMono, Old);
		Size.cx = r.Width() + 10; 
		Size.cy = BSize.cy;
	}
	else  // button
		Size  = BSize;
}


/////////////////////////////////////////////////////////////////////////////
// COCToolList
/////////////////////////////////////////////////////////////////////////////

static HBITMAP CreateDitherBitmap();

// Initialization code
COCToolList::COCToolList()
{
	FNAME 	Str;
	unsigned long	size;

	ASSERT(!OCToolList);
	hfontSys.CreateFont(-10, 0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,
		GetSystemMetrics(SM_DBCSENABLED) ?
		_T("Terminal") : _T("MS Sans Serif"));
	HBITMAP hbmGray = ::CreateDitherBitmap();
	if (hbmGray != NULL)
		{
		hbrDither = ::CreatePatternBrush(hbmGray);
		DeleteObject(hbmGray);
		}
	// Mono DC and Bitmap for disabled image
	hDCMono = ::CreateCompatibleDC(NULL);
	hDCGlyphs = CreateCompatibleDC(NULL);
	// Cached system values (updated in CFrameWnd::OnSysColorChange)
	UpdateSysColors();

	// hi-bit of HIWORD is set if the platform is Win32s
	DWORD dwVersion = ::GetVersion();
	bWin4 = (BYTE)dwVersion >= 4;

	m_SharedBorder = bWin4? 0 : 1;

	// cxBorder2 and cyBorder are 2x borders for Win4
	cxBorder2 = bWin4 ? CX_BORDER*2 : CX_BORDER;
	cyBorder2 = bWin4 ? CY_BORDER*2 : CY_BORDER;

	m_Draging = FALSE;
	m_Customize	= FALSE;
	m_ColorIcons = TRUE;
	m_ToolTips   = TRUE;
	Str[0] = 0;
	GetIniPath(Str);
	m_FileName = Str;
	size = sizeof(FNAME);
	GetUserName(Str, &size);
	m_FileName += Str;
	m_FileName += ".TBP";
	OCToolList = this;
}

int   COCToolList::GetDefaultGap()
{
	return(bWin4? DEFAULT_GAP_WIN95 : DEFAULT_GAP_WINNT);
}

CSize COCToolList::GetButtonSize(BOOL Large)
{
	CSize	Size;

	Size.cy = Size.cx = Large? 32 : 24;
	if (bWin4)
		Size.cx--;
	return(Size);
}


COCToolList::~COCToolList()
{
DeleteDC(hDCMono);
DeleteObject(hbrDither);
DeleteDC(hDCGlyphs);
UnLoad();
}

void COCToolList::FillListBox(CListBox*	pList, int Sel)
{
	POSITION      	Pos;
   	COCToolBar*		pToolBar;
	int				i;

	pList->ResetContent();
	for (Pos = m_List.GetHeadPosition(); Pos != NULL ;)
	{
		pToolBar = m_List.GetNext(Pos);
		i = pList->AddString(pToolBar->GetName());
		pToolBar->SetUpState(pToolBar->IsToolBarUp());
		pList->SetItemData(i, (long)pToolBar);
	}
	pList->SetCurSel(Sel);
}

POSITION COCToolList::FindToolBox(CString &Name)
   {
   POSITION        	NPos,Pos;
   COCToolBar*		pBar;

   NPos = m_List.GetHeadPosition();
   for (; NPos != NULL;)
      {
      Pos = NPos;
      pBar = m_List.GetNext(NPos);
      if (pBar->GetName() == Name)
         return(Pos);
      }
   return(NULL);
   }

BOOL COCToolList::Delete(COCToolBar* pBar)
{
  	POSITION        Pos = m_List.Find(pBar);

	return Delete(Pos);
}

BOOL COCToolList::Delete(POSITION Pos)
{
	COCToolBar*		pBar;

   	if (!Pos)
    	return(FALSE);
   	pBar = m_List.GetAt(Pos);
   	if (pBar->IsToolBarUp())
   		pBar->CloseToolBar();
   	m_List.RemoveAt(Pos);

   delete pBar;
   return(TRUE);
   }

void COCToolList::SetColorIcons(BOOL f)
{
	m_ColorIcons = f;
}

void COCToolList::SetToolTips(BOOL f)
{
	m_ToolTips = f;
}

void COCToolList::Serialize(CArchive &ar)
{
   	COCToolBar*		pBar;
   	POSITION		Pos;
	DWORD 			dw;
	WORD			w;
	int				i;

	if (ar.IsStoring())
	{
		ar << (WORD)m_ColorIcons;
		ar << (WORD)m_ToolTips;
		ar << (DWORD)m_List.GetCount();
   		for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
    	{
    		pBar = m_List.GetNext(Pos);
			pBar->Serialize(ar);
    	}
	}
	else
	{
		ar >> w;
		m_ColorIcons = w;
		ar >> w;
		m_ToolTips = w;
		ar >> dw;
   		for (i = 0; i < dw ; i++)
    	{
			pBar = AddToolBar();
			pBar->Serialize(ar);
			if (pBar->GetStyle2() & OCTOOLS_ISUP)
				pBar->Create(m_pFrame);
    	}
	}
}

void COCToolList::UnLoad()
{
   	COCToolBar*	pBar;
   	POSITION		Pos;

   	for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
    {
    	pBar = m_List.GetNext(Pos);
      	delete pBar;
    }
}

COCToolBar* COCToolList::AddToolBar()
{
	COCToolBar	*pBar = NewToolBar();
	m_List.AddTail(pBar);
	return(pBar);
}

BOOL COCToolList::CreateToolBar(COCButton *pButton, CPoint Pt, CString &Str)
{
	COCToolBar	*pBar = AddToolBar();

	pBar->SetName(Str);
	pBar->SetPos(Pt);
	if (!pBar->Create(m_pFrame))
		return FALSE;
	pBar->InsertButton(pButton->Duplicate());
	return(TRUE);
}

void COCToolList::SetCustomizing(BOOL f)
{
	m_Customize = f;
   	COCToolBar*	pBar;
   	POSITION	Pos;

	for (Pos = m_List.GetHeadPosition(); Pos != NULL;)
	{
    	pBar = m_List.GetNext(Pos);
		pBar->SetCustomizing(f);
    }
}

void COCToolList::StartDrag(CWnd *pOwner, CPoint Pt, CRect Bt)
{
	CWindowDC dc(NULL);

	m_pDragOwner = pOwner;
	m_SrcPoint	 = Pt;
	m_Draging 	 = TRUE;
	m_pDragOwner->ClientToScreen(&m_SrcPoint);
	m_DragPoint = m_SrcPoint;
	m_DragOffset.cx = Bt.left - Pt.x;
	m_DragOffset.cy = Bt.top  - Pt.y;
	m_DragSize  = CSize(Bt.Width(), Bt.Height());
	pOwner->SetCapture();
	dc.DrawFocusRect(CRect(m_DragPoint + m_DragOffset, m_DragSize));
}

void COCToolList::MouseMove(CPoint point)
{
	CWindowDC dc(NULL);

	m_pDragOwner->ClientToScreen(&point);

	dc.DrawFocusRect(CRect(m_DragPoint + m_DragOffset, m_DragSize));
	m_DragPoint = point;
	dc.DrawFocusRect(CRect(m_DragPoint + m_DragOffset, m_DragSize));
}

BOOL COCToolList::EndDrag(CPoint Pt, BOOL Cancel)
{
	CWnd	*pWnd;
	CPoint	 Pt2;
 	CWindowDC dc(NULL);
	BOOL		Found = FALSE;

	dc.DrawFocusRect(CRect(m_DragPoint + m_DragOffset, m_DragSize));
	m_pDragOverWnd = NULL;
	if (m_Draging && !Cancel)
	{
		m_pDragOwner->ClientToScreen(&Pt);
		m_DropPoint = Pt;
		Pt2 = Pt;
		if (pWnd = CWnd::WindowFromPoint(Pt))
		{
			pWnd->ScreenToClient(&Pt);
			pWnd = pWnd->ChildWindowFromPoint(Pt);
			if (pWnd)
			{
				if (pWnd->IsKindOf(RUNTIME_CLASS(COCToolBar)))
					Found = TRUE;
				else
				{
					pWnd = pWnd->GetParent();
					if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(COCToolBar)))
						Found = TRUE;
				}
				if (Found)	
				{
					m_pDragOverWnd = (COCToolBar *)pWnd;
 					m_pDragOverWnd->ScreenToClient(&m_DropPoint);
 					m_pDragOverWnd->ScreenToClient(&m_SrcPoint);
				}
			}
		}
	}
	else Cancel = TRUE;
	ReleaseCapture();
	m_Draging = FALSE;
	return(!Cancel);
}

BOOL COCToolList::IsDraging()
{
	if (m_Draging && CWnd::GetCapture() != m_pDragOwner)
		m_Draging = FALSE;
	return(m_Draging);
}


void COCToolList::UpdateSysColors()
{
	clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
	clrBtnGrayText = ::GetSysColor(COLOR_GRAYTEXT);
	clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
	clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);
}



/////////////////////////////////////////////////////////////////////////////

static HBITMAP CreateDitherBitmap()
{
	struct  // BITMAPINFO with 16 colors
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD      bmiColors[16];
	} bmi;
	memset(&bmi, 0, sizeof(bmi));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 8;
	bmi.bmiHeader.biHeight = 8;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 1;
	bmi.bmiHeader.biCompression = BI_RGB;

	COLORREF clr = ::GetSysColor(COLOR_BTNFACE);
	bmi.bmiColors[0].rgbBlue = GetBValue(clr);
	bmi.bmiColors[0].rgbGreen = GetGValue(clr);
	bmi.bmiColors[0].rgbRed = GetRValue(clr);

	clr = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	bmi.bmiColors[1].rgbBlue = GetBValue(clr);
	bmi.bmiColors[1].rgbGreen = GetGValue(clr);
	bmi.bmiColors[1].rgbRed = GetRValue(clr);

	// initialize the brushes
	long patGray[8];
	for (int i = 0; i < 8; i++)
	   patGray[i] = (i & 1) ? 0xAAAA5555L : 0x5555AAAAL;

	HDC hDC = GetDC(NULL);
	HBITMAP hbm = CreateDIBitmap(hDC, &bmi.bmiHeader, CBM_INIT,
		(LPBYTE)patGray, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);
	ReleaseDC(NULL, hDC);

	return hbm;
}

/////////////////////////////////////////////////////////////////////////////
// COCToolBar
/////////////////////////////////////////////////////////////////////////////

const DWORD DockSides[4][2] =
{
	{AFX_IDW_DOCKBAR_TOP,	OCTOOLS_DOCKTOP},
	{AFX_IDW_DOCKBAR_BOTTOM,OCTOOLS_DOCKBOTTOM},
	{AFX_IDW_DOCKBAR_LEFT,	OCTOOLS_DOCKLEFT},
	{AFX_IDW_DOCKBAR_RIGHT,	OCTOOLS_DOCKRIGHT},
};

UINT COCToolBar::GetDockingSide()
{
	CDockBar* 	pDock;
	CFrameWnd*	pFrame;

	if (!m_pDockBar || IsFloating())
		return(0);
	pFrame = GetDockingFrame();
	for (int i=0; i < 4; i++)
	{
		pDock = (CDockBar*)pFrame->GetControlBar(DockSides[i][0]);
		if (pDock ==  m_pDockBar)
			return(DockSides[i][1]);
	}
	return(0);
}

void COCToolBar::SetLargeIcon(BOOL f)
{
	COCButton*		pTBB;
	if (f)
		m_Style2 |= OCTOOLS_LARGEICONS;
	else
		m_Style2 &= ~OCTOOLS_LARGEICONS;
	for (int i = 0; i < m_nCount;i++)
	{
		pTBB = GetButtonPtr(i);
		pTBB->CalcSize(f);	
	}
	if (GetSafeHwnd())
		BuildBitmap();
}

void COCToolBar::SetName(CString Str)
{
	m_Name = Str;
	if (GetSafeHwnd())
	{
		SetWindowText(m_Name);
		if (IsFloating())
		{
			m_pDockBar->SetWindowText(m_Name);
			GetDockingFrame()->SetWindowText(m_Name);
		}
	}
}

BOOL COCToolBar::IsToolBarUp()
{
	if (GetSafeHwnd() && (GetStyle() & WS_VISIBLE))
		return(TRUE);
	return(FALSE);
}

void COCToolBar::Serialize(CArchive &ar)
{
	COCButton**		pTBB;
 	DWORD 			dw;
	int				i,count;

	if (ar.IsStoring())
	{
		GetBarRect();
		if (IsFloating())
			m_Style2 |= OCTOOLS_ISFLOATING;
		else
		{
			m_Style2 &= ~(OCTOOLS_ISFLOATING | OCTOOLS_DOCKALL);
			m_Style2 |= GetDockingSide();
		}
		if (IsToolBarUp())
			m_Style2 |= OCTOOLS_ISUP;
		else
			m_Style2 &= ~OCTOOLS_ISUP;

		ar << m_Name;
		ar << (DWORD)m_Spliting;
		ar << (DWORD)m_nRows;
		ar << (DWORD)m_Style2;
		ar << (DWORD) m_BarRect.left;
		ar << (DWORD) m_BarRect.top;
		ar << (DWORD) m_BarRect.right;
		ar << (DWORD) m_BarRect.bottom;
		ar << (DWORD) m_nCount;
		for (i = 0; i < m_nCount;i++)
			ar << GetButtonPtr(i);
	}
	else
	{
		ar >> m_Name;
		ar >> dw;
		m_Spliting 	= dw;
		ar >> dw;
		m_nRows		= dw;
		ar >> dw;
		m_Style2 		= dw;
		ar >> dw;
		m_BarRect.left 	= dw;
		ar >> dw;
		m_BarRect.top 		= dw;
		ar >> dw;
		m_BarRect.right 	= dw;
		ar >> dw;
		m_BarRect.bottom 	= dw;
		ar >> dw;
		count			= dw;
		AllocElements(count);
		for (i = 0; i < m_nCount;i++)
		{
			pTBB = GetButtonPtrPtr(i);
			ar >> *pTBB;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// COCToolBar creation etc

COCButton* COCToolBar::GetButtonPtr(int nIndex)
{
	COCButton*	pTBB;
	ASSERT(nIndex >= 0 && nIndex < m_nCount);
	ASSERT(m_pData != NULL);
	pTBB = ((COCButton**)m_pData)[nIndex];
	ASSERT(pTBB != NULL);
	return (pTBB);
}

COCButton** COCToolBar::GetButtonPtrPtr(int nIndex)
{
	COCButton**	pTBB;
	ASSERT(nIndex >= 0 && nIndex < m_nCount);
	ASSERT(m_pData != NULL);
	pTBB = &((COCButton**)m_pData)[nIndex];
	return (pTBB);
}

COCToolBar::COCToolBar()
{
	m_Name = "";
	m_hbmImageWell = NULL;
	m_WinButton = m_iButtonCapture = -1;      // nothing captured

	m_Horz   = -1;
	m_nRows  = 1;
	// UISG standard sizes
	m_Style2 = OCTOOLS_ISFLOATING;
	m_cxLeftBorder = m_cxRightBorder = 
	m_cyTopBorder = m_cyBottomBorder = 5;   // 3 pixel for top/bottom gaps

	m_BarRect.SetRectEmpty();
	m_Spliting  = 0;
	// adjust sizes when running on Win4
	m_cxDefaultGap = OCToolList->bWin4? DEFAULT_GAP_WIN95 : DEFAULT_GAP_WINNT;
}

COCToolBar::~COCToolBar()
{
	if (m_pData)
	{
		COCButton* pTBB;
		for (int i = 0; i < m_nCount; i++)
		{
			pTBB = GetButtonPtr(i);
			delete pTBB;
		}
		free(m_pData);
		m_pData  = NULL;
		m_nCount = 0;
	}
	DeleteObject(m_hbmImageWell);
}

BOOL COCToolBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	COCButton* pTBB;
	if (pParentWnd != NULL)
		ASSERT_VALID(pParentWnd);   // must have a parent

	// save the style
	m_dwStyle = dwStyle | WS_VISIBLE;
	if (nID == AFX_IDW_TOOLBAR)
		m_dwStyle |= CBRS_HIDE_INPLACE;

	// create the HWND
	if (!CWnd::Create(_afxWndControlBar, NULL, dwStyle, CRect(10000, 10000, 100, 100), pParentWnd, nID))
		return FALSE;
	SetFont(&OCToolList->hfontSys, TRUE);
	for (int i = 0; i < m_nCount; i++)
	{
		pTBB = GetButtonPtr(i);
		pTBB->Create(this);
	}
	SetBarStyle(GetBarStyle() |	CBRS_TOOLTIPS | CBRS_FLYBY);
	SetWindowText(m_Name);
	EnableDocking(CBRS_ALIGN_ANY);
	StartResizing();
	Resize();
	EndResizing();
	BuildBitmap();
	if (m_Style2 & OCTOOLS_ISFLOATING)
		((CFrameWnd*)pParentWnd)->FloatControlBar(this, m_BarRect.TopLeft(), CBRS_ALIGN_TOP | (GetBarStyle() & CBRS_FLOAT_MULTI));
	else
	{
		UINT	Side;
		UINT	Style = m_Style2 & OCTOOLS_DOCKALL;
		LPCRECT	lpRect = NULL;

		for (int i=0; i < 4; i++)
		{
			if (Style == DockSides[i][1])
			{
				Side = DockSides[i][0];
				break;
			}
		}
		if (!m_BarRect.IsRectEmpty())
			lpRect = &m_BarRect;
		((CFrameWnd*)pParentWnd)->DockControlBar(this, Side, lpRect);
	}
	ShowWindow(SW_SHOW);
	RecalcLayout();
	return TRUE;
}

void COCToolBar::SetHeight(int cyHeight)
{
	ASSERT_VALID(this);

	CSize		MaxSize = GetMaxButtonSize();
	int nHeight = cyHeight;
	if (m_dwStyle & CBRS_BORDER_TOP)
		cyHeight -= OCToolList->cyBorder2;
	if (m_dwStyle & CBRS_BORDER_BOTTOM)
		cyHeight -= OCToolList->cyBorder2;
	m_cyBottomBorder = (cyHeight - MaxSize.cy) / 2;
	// if there is an extra pixel, m_cyTopBorder will get it
	m_cyTopBorder = cyHeight - MaxSize.cy - m_cyBottomBorder;
	if (m_cyTopBorder < 0)
	{
		TRACE1("Warning: COCToolBar::SetHeight(%d) is smaller than button.\n",
			nHeight);
		m_cyBottomBorder += m_cyTopBorder;
		m_cyTopBorder = 0;  // will clip at bottom
	}
	// bottom border will be ignored (truncate as needed)
	Invalidate();   // just to be nice if called when toolbar is visible
}

HBITMAP	BuildBitmap(COCButton **pData, int Count, CSize Size)
{
	HDC 	hDCScreen 	= ::GetDC(NULL);
	HDC		hDCSrc		= CreateCompatibleDC(hDCScreen);
	HDC		hDCDst  	= OCToolList->hDCGlyphs;
	HBITMAP hbmDstOld,hbmSrcOld, hBM;
	UINT	IDBitmap = 0;
	UINT	TmpID;
	int		NumButton = 0;
	SIZE	BMSize;
	COCBitmap	hBMRes(Size);
	int		XSrc,YSrc,XDst,YDst,i;
	HBITMAP	hBitmap;
	COCButton** pTBB;

	if (!Count)
		return(NULL);
	pTBB = pData;
	for (i = 0; i < Count; i++, pTBB++)
	{
		if (!((*pTBB)->nStyle & TBBS_SEPARATOR))	// not separator
			NumButton++;
	}
	BMSize.cx =	NumButton * Size.cx;
	BMSize.cy = Size.cy;

	hBitmap = CreateCompatibleBitmap(hDCScreen, BMSize.cx, BMSize.cy);
	hbmDstOld = (HBITMAP)::SelectObject(hDCDst, hBitmap);
	ReleaseDC(NULL, hDCScreen);

	hbmSrcOld = NULL;
	pTBB = pData;
	for (i = 0,NumButton = 0; i < Count; i++, pTBB++)
	{
		if ((*pTBB)->nStyle & TBBS_BUTTON)	// button
		{
			TmpID = (*pTBB)->IDMasterBM;
			if (IDBitmap != TmpID)
			{
				IDBitmap = (*pTBB)->IDMasterBM;
				if (!(hBMRes.LoadBitmap(IDBitmap)))
				{
					::SelectObject(hDCDst, hbmDstOld);
					if (hbmSrcOld)
						::SelectObject(hDCSrc, hbmSrcOld);
					DeleteDC(hDCSrc);
					return(hBitmap);
				}

				hBM = (HBITMAP)::SelectObject(hDCSrc, hBMRes.GetSafeHandle());
				if (!hbmSrcOld)
					hbmSrcOld = hBM;
			}
			XSrc = (*pTBB)->x * Size.cx;
			YSrc = (*pTBB)->y * Size.cy;
			XDst = NumButton * Size.cx;
			YDst = 0;
			BitBlt( hDCDst, XDst, YDst, Size.cx, Size.cy,
					hDCSrc,XSrc,YSrc, SRCCOPY);
			(*pTBB)->iImage = NumButton++;
		}
	}
	SelectObject(hDCDst, hbmDstOld);
	if (hbmSrcOld)
		SelectObject(hDCSrc, hbmSrcOld);
	DeleteDC(hDCSrc);
	return (hBitmap);
}


// your App have to ship with these four DLLs
//		BARSCBMP	Small color icons.
//		BARSBBMP	Small black and white icons.
//		BARLCBMP	Large color icons.
//		BARLBBMP	Large black and white icons.
LPTSTR TBarIconDLL[4] = {"TBAR16C.dll","TBAR16B.dll","TBAR24C.dll","TBAR24B.dll"};

BOOL COCToolBar::BuildBitmap()
{
	CSize		ISize =	m_Style2 & OCTOOLS_LARGEICONS? CSize(24, 24) : CSize(16,16);
	HINSTANCE	hOrigRes = AfxGetResourceHandle();
	int			i    = m_Style2 & OCTOOLS_LARGEICONS ? 2 : 0;
	HINSTANCE	hRes;

	i += OCToolList->m_ColorIcons? 0 : 1;
	hRes = LoadLibrary(TBarIconDLL[i]); 
	AfxSetResourceHandle(hRes);
	if (m_hbmImageWell)
		DeleteObject(m_hbmImageWell);     // get rid of old one
	m_hbmImageWell = ::BuildBitmap((COCButton**)m_pData, m_nCount, ISize);
	AfxSetResourceHandle(hOrigRes);
	FreeLibrary(hRes);
	RecalcLayout();
	return(m_hbmImageWell != NULL);
}

BOOL COCToolBar::ShowToolBar(CFrameWnd *pFrame, CWnd *pOwner)
{
	if (GetSafeHwnd())
		pFrame->ShowControlBar(this, (GetStyle() & WS_VISIBLE) == 0, FALSE);
   	else
	{
		if (!Create(pFrame))
			return FALSE;
		if (pOwner)
			SetOwner(pOwner);
	}
	return(TRUE);
}

void COCToolBar::CloseToolBar()
{
	if (!GetSafeHwnd())
		return;

	if (IsFloating())
		GetDockingFrame()->DestroyWindow();
	else
	{
		CFrameWnd*		pDockFrame = GetDockingFrame();
		m_pDockBar->RemoveControlBar(this);
		m_pDockBar = NULL;
		SendMessage(WM_CLOSE, 0, 0);
		pDockFrame->RecalcLayout();
	}
}

void COCToolBar::RecalcLayout()
{
	CWnd	*pWnd;

	if (!GetSafeHwnd())
		return;
	pWnd = GetDockingFrame();
	if (IsFloating())
	{
		ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)));
		((CMiniDockFrameWnd *)pWnd)->RecalcLayout();
	}
	else
	{
		if (pWnd)
			{
			ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
			((CFrameWnd *)pWnd)->RecalcLayout();
			}
	}
	Invalidate();
}

BOOL COCToolBar::DeleteButton(int loc, BOOL ReBuild)
{
	int			SepLoc = -1;
	BOOL		DelSep = FALSE;
	COCButton**	pOldButton = (COCButton**)m_pData;
	COCButton**	pData= pOldButton;
	COCButton**	ppTBB;
	COCButton* 	pTBB;
	int			 NewCount = m_nCount - 1;
	int			 OldCount = m_nCount;

	if (loc >= m_nCount)
		return(FALSE);

	pTBB = GetButtonPtr(loc);
	if (pTBB->hWnd)
		m_WinButton = -1;
	if (loc)
	{
		if (DelSep = GetButtonPtr(loc-1)->nStyle & TBBS_SEPARATOR)
			SepLoc = loc - 1;
	}
	else if ((loc+1) < m_nCount)
		if (DelSep = GetButtonPtr(loc+1)->nStyle & TBBS_SEPARATOR)
			SepLoc = loc + 1;
	if (SepLoc != -1)
		NewCount--;
	m_pData  = NULL;
	m_nCount = 0;
	// first allocate array for panes and copy initial data
	if (!AllocElements(NewCount))
		return FALSE;

	ppTBB = (COCButton **)m_pData;
	for (int i = 0; i < OldCount; i++)
	{
		ASSERT(*pOldButton != NULL);
		if (i != loc && i != SepLoc)
		{
			*ppTBB = *pOldButton;
			ppTBB++;
		}
		else
		{
			COCButton*	pBt = *pOldButton;
			delete pBt;
		}
		pOldButton++;
	}
	free(pData);
	if (ReBuild)
	{
		if (!BuildBitmap())
			return FALSE;
	}
	else
		RecalcLayout();
	return TRUE;
}

// if loc == -1, then add to the end
// always inserts before except when -1(at the end) it inserts after.
BOOL COCToolBar::InsertButton(COCButton *pButton, int loc, BOOL ReBuild)
{
	ASSERT_VALID(this);
	ASSERT(pButton);
	COCButton	**pOldButton = (COCButton**)m_pData;
	COCButton	**pData= pOldButton;
	int			 NewCount = m_nCount + 1;

	if (loc == -1)
		loc = m_nCount;
	m_pData  = NULL;
	m_nCount = 0;
	// first allocate array for panes and copy initial data
	if (!AllocElements(NewCount))
		return FALSE;
	
	pButton->CalcSize(m_Style2 & OCTOOLS_LARGEICONS);	
	// go through them adding buttons
	COCButton** pTBB;
	for (int i = 0; i < NewCount;i++)
	{
		pTBB = GetButtonPtrPtr(i);
		if (i == loc)
		{
			*pTBB = pButton;
			(*pTBB)->Create(this);
		}
		else
		{
			ASSERT(*pOldButton != NULL);
			*pTBB = *pOldButton;
			pOldButton++;
		}
	}
	free(pData);
	if (ReBuild)
	{
		if (!BuildBitmap())
			return FALSE;
	}
	else
		RecalcLayout();
	return TRUE;
}

BOOL COCToolBar::AllocElements(int nElements)
{
	ASSERT_VALID(this);
	if (m_pData != NULL)
	{
		ASSERT(m_nCount != 0);
		free(m_pData);
	}
	else
	{
		// no initialized yet
		ASSERT(m_nCount == 0);
	}

	if (nElements)
	{
		if ((m_pData = calloc(nElements, sizeof(COCButton*))) == NULL)
			return FALSE;
	}
	else  m_pData = NULL;

	m_nCount = nElements;
	return TRUE;
}


BOOL COCToolBar::SetButtons(const OCBUTTON* lpIDArray, int nIDCount)
{
	ASSERT_VALID(this);
	ASSERT(nIDCount >= 1);  // must be at least one of them
	ASSERT(lpIDArray == NULL ||
		AfxIsValidAddress(lpIDArray, sizeof(OCBUTTON) * nIDCount, FALSE));

	// first allocate array for panes and copy initial data
	if (!AllocElements(nIDCount))
		return FALSE;
	ASSERT(nIDCount == m_nCount);

	if (lpIDArray != NULL)
	{
		// go through them adding buttons
		COCButton** pTBB;
		for (int i = 0; i < nIDCount;i++)
		{
			pTBB = GetButtonPtrPtr(i);
			*pTBB  = new COCButton();
			**pTBB = *lpIDArray;
			(*pTBB)->CalcSize(m_Style2 & OCTOOLS_LARGEICONS);
			lpIDArray++;
		}
		if (!BuildBitmap())
			return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// COCToolBar attribute access

int COCToolBar::CommandToIndex(UINT nIDFind)
{
	ASSERT_VALID(this);

	COCButton* pTBB;
	for (int i = 0; i < m_nCount; i++)
	{
		pTBB = GetButtonPtr(i);
		if (pTBB->nID == nIDFind)
			return i;
	}
	return -1;
}

UINT COCToolBar::GetItemID(int nIndex)
{
	ASSERT_VALID(this);

	return GetButtonPtr(nIndex)->nID;
}

void COCToolBar::GetItemRect(int nIndex, LPRECT lpRect)
{
	COCButton* pTBB;
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0 && nIndex < m_nCount);

	pTBB = GetButtonPtr(nIndex);
	lpRect->right = (lpRect->left = pTBB->Origin.x) + pTBB->Size.cx;
	lpRect->bottom = (lpRect->top = pTBB->Origin.y) + pTBB->Size.cy;
}

inline UINT COCToolBar::GetButtonStyle(int nIndex)
{
	return GetButtonPtr(nIndex)->nStyle;
}

void COCToolBar::SetButtonStyle(int nIndex, UINT nStyle)
{
	COCButton* pTBB = GetButtonPtr(nIndex);
	UINT nOldStyle = pTBB->nStyle;
	if (nOldStyle != nStyle)
	{
		// update the style and invalidate
		pTBB->nStyle = nStyle;

		// invalidate the button only if both styles not "pressed"
		if (!(nOldStyle & nStyle & TBBS_PRESSED))
			InvalidateButton(nIndex);
	}
}


CSize COCToolBar::GetMaxButtonSize()
{
	CSize	Size = CSize(0,0);

	ASSERT_VALID(this);
	COCButton* pTBB;
	for (int i = 0; i < m_nCount;i++)
	{
		pTBB = GetButtonPtr(i);
		if (pTBB->Size.cx > Size.cx)
			Size.cx = pTBB->Size.cx;
		if (pTBB->Size.cy > Size.cy)
			Size.cy = pTBB->Size.cy;
	}
	return(Size);
}

void COCToolBar::DoPaint(CDC* pDC)
{
	CRect rect;
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	CControlBar::DoPaint(pDC);      // draw border
	GetClientRect(rect);
	CalcInsideRect(rect, m_dwStyle & CBRS_ORIENT_HORZ);

	HBITMAP hbmOldGlyphs = (HBITMAP)SelectObject(OCToolList->hDCGlyphs, m_hbmImageWell);
	COCButton* pTBB;
	for (int i = 0; i < m_nCount; i++)
	{
		pTBB = GetButtonPtr(i);
		GetItemRect(i, &rect);
		if (!(pTBB->nStyle & TBBS_SEPARATOR) && !(pTBB->nStyle & TBBS_WINDOW))
			pTBB->DrawButton(pDC, m_Style2 & OCTOOLS_LARGEICONS? 24 : 16);
	}
	SelectObject(OCToolList->hDCGlyphs, hbmOldGlyphs);
	if (m_WinButton != -1)
		m_WinRect.Draw(pDC);
}

void COCToolBar::InvalidateButton(int nIndex)
{
	ASSERT_VALID(this);

	CRect rect;
	GetItemRect(nIndex, &rect);
	InvalidateRect(rect, FALSE);    // don't erase background
}

UINT COCToolBar::OnCmdHitTest(CPoint point, CPoint* pCenter)
{
	ASSERT_VALID(this);

	// check child windows first by calling CControlBar
	UINT nHit = CControlBar::OnCmdHitTest(point, pCenter);
	if (nHit != (UINT)-1)
		return nHit;

	// now hit test against COCToolBar buttons
	nHit = (UINT)HitTest(point);
	if (nHit != (UINT)-1)
	{
		COCButton* pTBB = GetButtonPtr(nHit);
		nHit = pTBB->nID;
	}
	return nHit;
}

int COCToolBar::GetWhatLine(int y)
{
 	COCButton* 	pTBB;
	int 		iButton;

	for (iButton = 0; iButton < m_nCount; iButton++)
	{
		pTBB = GetButtonPtr(iButton);
		if (pTBB->nStyle & TBBS_SEPARATOR)
			continue;
		if (y < (pTBB->Origin.y + pTBB->Size.cy))
			return(iButton);
	}
	// it is below the last line.
	for (iButton = m_nCount-1; iButton > 0; iButton--)
	{
		pTBB = GetButtonPtr(iButton);
		if (pTBB->nStyle & TBBS_NEWLINE)
			return(iButton);
	}
	return(0); // it only has one line
}

int COCToolBar::GetInsertLoc(CPoint &point)
{
 	COCButton* 	pTBB;
	CRect		r;
	BOOL 		bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE;
	int  		loc,Start;

	if (bHorz || IsFloating())
	{
		loc = point.x;
		Start = GetWhatLine(point.y);
		for (int iButton = Start; iButton < m_nCount; iButton++)
		{
			pTBB = GetButtonPtr(iButton);
			if (pTBB->nStyle & TBBS_NEWLINE && Start != iButton)
			{
				if (GetButtonPtr(iButton-1)->nStyle & TBBS_SEPARATOR)
					iButton--;
				return iButton;		// beyond the end of line
			}
			if (loc < (pTBB->Origin.x + pTBB->Size.cx))
				return iButton;     // hit !
		}
	}
	else
	{
		loc = point.y;
		for (int iButton = 0; iButton < m_nCount; iButton++)
		{
			pTBB = GetButtonPtr(iButton);
			if (loc < (pTBB->Origin.y + pTBB->Size.cy))
				return iButton;     // hit !
		}
	}

	return m_nCount - 1;      // nothing hit
}

int COCToolBar::HitTest(CPoint point) // in window relative coords
{
	COCButton* 	pTBB;
	CRect		r;
	for (int iButton = 0; iButton < m_nCount; iButton++)
	{
		pTBB = GetButtonPtr(iButton);
		ASSERT(pTBB != NULL);
		r = CRect(pTBB->Origin, pTBB->Size);
		if (r.PtInRect(point) && !(pTBB->nStyle & TBBS_SEPARATOR))
			return iButton;     // hit !
	}

	return -1;      // nothing hit
}

/////////////////////////////////////////////////////////////////////////////
// COCToolBar message handlers

BEGIN_MESSAGE_MAP(COCToolBar, CControlBar)
	//{{AFX_MSG_MAP(COCToolBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SIZECHILD, OnResizeChild)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
IMPLEMENT_DYNAMIC(COCToolBar, CControlBar)

void COCToolBar::OnDestroy()
{
GetBarRect();
}

void COCToolBar::GetBarRect()
{
	if (!GetSafeHwnd())
		return;
	if (IsFloating())
		GetParentFrame()->GetWindowRect(&m_BarRect);
	else
		GetWindowRect(&m_BarRect);
}

void COCToolBar::DoWinButtonResizing(CPoint Pt)
{
	CWnd			Desktop;
	CWnd*			pDesktop;
	CRect			r;

	pDesktop = CWnd::FromHandlePermanent(::GetDesktopWindow());
	if (!pDesktop)
	{
		pDesktop = &Desktop;
		Desktop.Attach(::GetDesktopWindow());
	}
	Pt.x = m_RightSize.left;
	Pt.y = m_RightSize.top + m_RightSize.Height() / 2;

	BOOL ret = m_WinRect.Track(this, Pt, FALSE, pDesktop);
	if (ret)
	{
		CRect		r = m_WinRect.m_rect;
		COCButton* 	pTBB = GetButtonPtr(m_WinButton);
		
		pTBB->Size = r.Size() + CSize(WINDOWSEPARATOR*2-2,-2);
		m_RightSize = CRect(CPoint(r.right-1, r.top), CSize(m_WinRect.m_nHandleSize, r.Height()));
		RecalcLayout();
	}
	if (pDesktop == &Desktop)
		Desktop.Detach();
}

void COCToolBar::SetControlRects()
{
	if (m_WinButton != -1)
	{
		CRect	r;
		GetItemRect(m_WinButton, &r);
		r.InflateRect(-WINDOWSEPARATOR+1,1);
		m_WinRect.m_rect = r;
		m_RightSize = CRect(CPoint(r.right-1, r.top), CSize(m_WinRect.m_nHandleSize, r.Height()));
	}
}

void COCToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (OCToolList->m_Customize && m_WinButton != -1 && m_RightSize.PtInRect(point)) // do the tracking
	{
		DoWinButtonResizing(point);
		return;
	}

	if ((m_iButtonCapture = HitTest(point)) < 0) // nothing hit
	{
		CControlBar::OnLButtonDown(nFlags, point);
		return;
	}

	COCButton* pTBB = GetButtonPtr(m_iButtonCapture);
	if (OCToolList->m_Customize)
	{
		CRect	r;
		GetItemRect(m_iButtonCapture, &r);
		if (pTBB->nStyle & TBBS_CONTROL)
		{
			m_WinButton = m_iButtonCapture;
			m_WinRect.m_nStyle |= CRectTracker::hatchedBorder;
			SetControlRects();
			Invalidate();
			UpdateWindow();
		}
		else if (m_WinButton != -1)
		{
			m_WinButton = -1;
			Invalidate();
			UpdateWindow();
		}
		OCToolList->StartDrag(this, point, r);
		return;
	}
	// update the button before checking for disabled status
	UpdateButton(m_iButtonCapture);
	if (pTBB->nStyle & TBBS_DISABLED)
	{
		m_iButtonCapture = -1;
		return;     // don't press it
	}

	pTBB->nStyle |= TBBS_PRESSED;
	InvalidateButton(m_iButtonCapture);
	UpdateWindow(); // immediate feedback
	SetCapture();
	GetOwner()->SendMessage(WM_SETMESSAGESTRING, (WPARAM)pTBB->nID);
}

void COCToolBar::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	if (OCToolList->IsDraging())
		OCToolList->MouseMove(point);

	if (m_iButtonCapture >= 0 && !OCToolList->m_Customize) // track buttons
	{
		COCButton* pTBB = GetButtonPtr(m_iButtonCapture);

		UINT nNewStyle = (pTBB->nStyle & ~TBBS_PRESSED);
		int iButtonCapture = m_iButtonCapture;
		if (GetCapture() != this)
		{
			m_iButtonCapture = -1; // lost capture
			GetOwner()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
		}
		else
		{
			// should be pressed if still hitting the captured button
			if (HitTest(point) == m_iButtonCapture)
				nNewStyle |= TBBS_PRESSED;
		}
		SetButtonStyle(iButtonCapture, nNewStyle);
		UpdateWindow(); // immediate feedback
	}
}

BOOL COCToolBar::DeleteSeparator(int loc)
{
	COCButton	*pButton = GetButtonPtr(loc);

	if (!(pButton->nStyle & TBBS_SEPARATOR))
		return(FALSE);
	if (m_WinButton != -1)
		if (loc < m_WinButton)
			m_WinButton--;
	DeleteButton(loc, FALSE);
	return(TRUE);
}

// only allows one seperator in a row.
void COCToolBar::InsertSeparator(int loc)
{
	BOOL		Insert = TRUE;

	if (loc)
		Insert &= !(GetButtonPtr(loc-1)->nStyle & TBBS_SEPARATOR);
	else
		Insert = FALSE;
	Insert &= !(GetButtonPtr(loc)->nStyle & TBBS_SEPARATOR);
	if (Insert)
	{
		COCButton*	pBt = new COCButton();
		pBt->Size.cy = pBt->Size.cx = m_cxDefaultGap + OCToolList->m_SharedBorder * 2;
		if (m_WinButton != -1 && loc <= m_WinButton)
			m_WinButton++;
		InsertButton(pBt, loc, FALSE);
	}
}

void COCToolBar::MoveButton(int From, int To)
{
 	COCButton** pTBB;
	COCButton** pTBB1;
	COCButton* 	pTBB2;
	BOOL		DelSep = TRUE;
	int			 i;

	if (To == -1)
		To = m_nCount - 1;
	if (From)
		DelSep = GetButtonPtr(From-1)->nStyle & TBBS_SEPARATOR;
	if ((From+1) < m_nCount)
		DelSep &= GetButtonPtr(From+1)->nStyle & TBBS_SEPARATOR;
	pTBB2 = GetButtonPtr(From);
	if (From < To)		// shift up
	{
		for (int i = From; i < To; i++)
		{
			pTBB   = GetButtonPtrPtr(i);
			pTBB1  = GetButtonPtrPtr(i+1);
			*pTBB = *pTBB1;
		}
	}
	else				// shift down
	{  // To < From
		for ( i=From; i > To; i--)
		{
			pTBB   = GetButtonPtrPtr(i);
			pTBB1  = GetButtonPtrPtr(i-1);
			*pTBB  = *pTBB1;
		}
	}
	pTBB = GetButtonPtrPtr(To);
	*pTBB = pTBB2;
	if (m_WinButton != -1 && pTBB2->nStyle & TBBS_CONTROL)
		m_WinButton = To;
	if (DelSep)
		DeleteSeparator(From);
	RecalcLayout();
}


void COCToolBar::ExecuteDrag(int SrcLoc)
{
	COCToolBar	*pBar = OCToolList->m_pDragOverWnd;
	CPoint		 Pt   = OCToolList->m_DropPoint;
	CRect		 r;
	BOOL		 After;

	if (pBar) // insert
	{
		BOOL 		 bHorz = (pBar->m_dwStyle & CBRS_ORIENT_HORZ) ? TRUE : FALSE;
		int			 loc  = pBar->GetInsertLoc(Pt);
		COCButton	*pButton = GetButtonPtr(SrcLoc);
		
		if (IsFloating())
			bHorz = TRUE;
		pBar->GetItemRect(loc, &r);
		After = ( bHorz && Pt.x > (r.left + r.Width() /2)) ||
				(!bHorz && Pt.y > (r.top  + r.Height()/2));
		if (pBar != this)
		{
			if (After)
				loc++;
			pBar->InsertButton(pButton->Duplicate(), loc);
			pBar->SetCustomizing(TRUE);
			DeleteButton(SrcLoc, TRUE);
		}
		else    // it is a move
		{
			if (loc == SrcLoc) // add, delete separator
			{
				CPoint		 SrcPt   = OCToolList->m_SrcPoint;
				BOOL		 diff = bHorz? SrcPt.x - Pt.x : SrcPt.y - Pt.y;

				if (diff > 0 && loc)
				{
					if (!DeleteSeparator(loc - 1) && (loc + 1) != m_nCount)
						InsertSeparator(loc + 1);
				}
				else if (diff < 0)
					pBar->InsertSeparator(loc);
			}
			else if ((loc+1) == SrcLoc)
			{
				if (!pBar->DeleteSeparator(loc))
					MoveButton(SrcLoc, loc);
			}
			else   // move button
				MoveButton(SrcLoc, loc);
		}
	}
	else // delete button
		DeleteButton(SrcLoc);
}

void COCToolBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_iButtonCapture < 0)
	{
		CControlBar::OnLButtonUp(nFlags, point);
		return;     // not captured
	}

	COCButton* pTBB = GetButtonPtr(m_iButtonCapture);
	if (OCToolList->m_Customize && OCToolList->IsDraging())
	{
		if (OCToolList->EndDrag(point))
			ExecuteDrag(m_iButtonCapture);
		m_iButtonCapture = -1;
		return;
	}
	UINT nIDCmd = 0;

	UINT nNewStyle = (pTBB->nStyle & ~TBBS_PRESSED);
	if (GetCapture() == this)
	{
		// we did not lose the capture
		ReleaseCapture();
		if (HitTest(point) == m_iButtonCapture)
		{
			// give button a chance to update
			UpdateButton(m_iButtonCapture);

			// then check for disabled state
			if (!(pTBB->nStyle & TBBS_DISABLED))
			{
				// pressed, will send command notification
				nIDCmd = pTBB->nID;

				if (pTBB->nStyle & TBBS_CHECKBOX)
					nNewStyle ^= TBBS_CHECKED;
			}
		}
	}

	GetOwner()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);

	int iButtonCapture = m_iButtonCapture;
	m_iButtonCapture = -1;
	if (nIDCmd != 0)
		GetOwner()->SendMessage(WM_COMMAND, nIDCmd);    // send command

	SetButtonStyle(iButtonCapture, nNewStyle);
	UpdateButton(iButtonCapture);

	UpdateWindow(); // immediate feedback
}

void COCToolBar::OnCancelMode()
{
	CControlBar::OnCancelMode();

	if (m_iButtonCapture >= 0)
	{
		COCButton* pTBB = GetButtonPtr(m_iButtonCapture);
		UINT nNewStyle = (pTBB->nStyle & ~TBBS_PRESSED);
		if (GetCapture() == this)
			ReleaseCapture();
		SetButtonStyle(m_iButtonCapture, nNewStyle);
		m_iButtonCapture = -1;
		UpdateWindow();
	}
}

void COCToolBar::OnSysColorChange()
{
	OCToolList->UpdateSysColors();
	// re-initialize global dither brush
	HBITMAP hbmGray = ::CreateDitherBitmap();
	if (hbmGray != NULL)
	{
		HBRUSH hbrNew = ::CreatePatternBrush(hbmGray);
		if (hbrNew != NULL)
		{
			DeleteObject(OCToolList->hbrDither);      // free old one
			OCToolList->hbrDither = hbrNew;
		}
		::DeleteObject(hbmGray);
	}

	// re-color bitmap for toolbar
	if (m_hbmImageWell != NULL)
		BuildBitmap();
}

/////////////////////////////////////////////////////////////////////////////
// COCToolBar idle update through COCToolCmdUI class

class COCToolCmdUI : public CCmdUI        // class private to this file !
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
};

void COCToolCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
	COCToolBar* pToolBar = (COCToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT(pToolBar->IsKindOf(RUNTIME_CLASS(COCToolBar)));
	ASSERT(m_nIndex < m_nIndexMax);

	UINT Style     = pToolBar->GetButtonStyle(m_nIndex);
	UINT nNewStyle = Style & ~TBBS_DISABLED;
	if (!bOn)
		nNewStyle |= TBBS_DISABLED;
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle);
	if (nNewStyle & TBBS_CONTROL)
	{
		if (OCToolList->m_Customize)
			bOn = FALSE;
		COCButton* pTBB = pToolBar->GetButtonPtr(m_nIndex);
		::EnableWindow(pTBB->hWnd, bOn);
	}
}

void COCToolCmdUI::SetCheck(int nCheck)
{
	ASSERT(nCheck >= 0 && nCheck <= 2); // 0=>off, 1=>on, 2=>indeterminate
	COCToolBar* pToolBar = (COCToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT(pToolBar->IsKindOf(RUNTIME_CLASS(COCToolBar)));
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) & ~TBBS_CHECKED;
	if (nCheck == 1)
		nNewStyle |= TBBS_CHECKED;
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle | TBBS_CHECKBOX);
}

void COCToolCmdUI::SetText(LPCTSTR)
{
	// ignore it
}

void COCToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	COCToolCmdUI state;
	state.m_pOther = this;

	state.m_nIndexMax = (UINT)m_nCount;
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		COCButton* pTBB = GetButtonPtr(state.m_nIndex);
		state.m_nID = pTBB->nID;

		// ignore separators
		if (!(pTBB->nStyle & TBBS_SEPARATOR))
			state.DoUpdate(pTarget, bDisableIfNoHndler);
	}

	// update the dialog controls added to the toolbar
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

void COCToolBar::UpdateButton(int nIndex)
{
	// determine target of command update
	CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
	if (pTarget == NULL || !pTarget->IsFrameWnd())
		pTarget = GetParentFrame();

	// send the update notification
	if (pTarget != NULL)
	{
		COCToolCmdUI state;
		state.m_pOther = this;
		state.m_nIndex = nIndex;
		state.m_nIndexMax = (UINT)m_nCount;
		COCButton* pTBB = GetButtonPtr(nIndex);
		state.m_nID = pTBB->nID;
		state.DoUpdate(pTarget, pTarget->m_bAutoMenuEnable);
	}
}


// if bStretch is TRUE then tool bar is being docked permenantly.
CSize COCToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CSize		NewSize;
	BOOL		IsHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	if (m_Horz == -1)
		m_Horz = IsHorz;
	// if bHorz != IsHorz this means he is only cashing the size of
	// the other orientation for future use, so do not update.
	if (!IsFloating() || bHorz != IsHorz)
 		NewSize = GetDockedSize(bHorz, bHorz == IsHorz);
	else
 		NewSize = GetNonDockedSize(bHorz == IsHorz);
	m_Horz = IsHorz;
	return NewSize;
}

// gets the size of the ToolBar if it is Docked
CSize COCToolBar::GetDockedSize(BOOL bHorz, BOOL Update)
{
	COCButton* 	pTBB;
	CRect 		rect;
	int			x,y;
	BOOL		Floating = IsFloating();
	int			Max = 0;
	CSize		MaxSize = GetMaxButtonSize();

	rect.SetRectEmpty();        // only need top and left
	CalcInsideRect(rect, bHorz);

	if (bHorz)
	{
		x = rect.left;
		y = rect.top;
		for (int i = 0; i < m_nCount; i++)
		{
			pTBB = GetButtonPtr(i);
			if (pTBB->nStyle & TBBS_NEWLINE && Floating)
			{
				if (x > Max)
					Max = x;
				y += MaxSize.cy;
				if (GetButtonPtr(i-1)->nStyle & TBBS_SEPARATOR)
					y += m_cxDefaultGap + OCToolList->m_SharedBorder * 2;
				x = rect.left;
			}
			if (Update)
			{
				pTBB->Move(CPoint(x, y), bHorz);
				if (pTBB->nStyle & TBBS_WINDOW)		// Window
					SetControlRects();
			}
			if (!Floating || !m_Spliting || !(pTBB->nStyle & TBBS_SEPARATOR))
				x += pTBB->Size.cx - OCToolList->m_SharedBorder; 	 // adjust for overlap
		}
		if (x > Max)
			Max = x;
		Max -= rect.right;
		y += MaxSize.cy - rect.bottom;
	}
	else
	{
		x = rect.left;
		y = rect.top;
		for (int i = 0; i < m_nCount; i++)
		{
			pTBB = GetButtonPtr(i);
			if (Update)
			{
				pTBB->Move(CPoint(x, y), bHorz);
				if (pTBB->nStyle & TBBS_WINDOW)		// Window
					SetControlRects();
			}
			y  += pTBB->Size.cy - OCToolList->m_SharedBorder;
		}
		x += MaxSize.cx - rect.right;
		y -= rect.bottom;
		Max = x;
	}
x = max(Max, MaxSize.cx - rect.Width());
y = max(y,   MaxSize.cy - rect.Height());
return(CSize(x, y));
}

// gets the size of the ToolBar if it is not Docked
CSize COCToolBar::GetNonDockedSize(BOOL Update)
{
	CSize	Size;
	StartResizing();
	Resize();
	Size = GetDockedSize(TRUE, Update);
	EndResizing();
	return(Size);
}

// get the new row number ofter a resize.  Before we decide to split it is +1, or -1.  after we decide to split
// it could grow or shrink alot.
int COCToolBar::GetNumOfRows(int OldRows, SizingAction Action)
{
	int		R,NewRows = OldRows;
	int		MaxW,MinW,W;
	BOOL	Found = FALSE;

	if (Action == SizeShrink)
	{
		NewRows--;
		R = NewRows = max(NewRows, 1);
		MinW = GetDockedSize(TRUE, FALSE).cx;
		while (R)		// this loop is not the most effecient we can take
		{									// the approatch of a bubble sort.
			m_Spliting = m_nGroups >= R? FALSE: TRUE;
			if (Resize(R))					// Resize == FALSE.  failed to fit on R rows.
			{
				W = GetDockedSize(TRUE, FALSE).cx;
				if ( W > MinW)
				{
					Found = TRUE;
					NewRows = R;
					break;
				}
			}
			R--;
		}
	}
	else
	{
		NewRows++;
		R = NewRows = min(NewRows, m_nButtons);
		MaxW = GetDockedSize(TRUE, FALSE).cx;
		while (R <= m_nButtons)		// this loop is not the most effecient we can take
		{									// the approatch of a bubble sort.
			m_Spliting = m_nGroups >= R? FALSE: TRUE;
			if (Resize(R))			// Resize == FALSE.  failed to fit on R rows.
			{
				W = GetDockedSize(TRUE, FALSE).cx;
				if (W < MaxW)
				{
					Found = TRUE;
					NewRows = R;
					break;
				}
			}
			R++;
		}
	}
	if (!Found)
		NewRows = OldRows;
	m_Spliting = m_nGroups >= NewRows? FALSE: TRUE;
	ClearLineBreaks();
	return(NewRows);
}

void COCToolBar::ClearLineBreaks()
{
	COCButton*	pTBB;

	for (int i = 0; i < m_nCount; i++)
	{
		pTBB = GetButtonPtr(i);
		pTBB->nStyle &= ~TBBS_NEWLINE;
	}

}

LRESULT COCToolBar::OnResizeChild(WPARAM wParam, LPARAM lParam) 
{
	if (m_WinButton != -1)
	{
		m_WinButton = -1;
		Invalidate();
		UpdateWindow();
	}
	if (m_tracker.StartTracking(this, wParam))
		RecalcLayout();
	return 0;
}

int COCToolBar::GetWidthOfGroup(int &index, int &ActualW)
{
	COCButton*	pTBB;
	int			w = 0;

	ActualW = 0;
	for (index; index < m_nCount; index++) // leading separators
	{
		pTBB = GetButtonPtr(index);
		if (pTBB->nStyle & TBBS_SEPARATOR)
			w += pTBB->Size.cx;
		else break;
	}
		
	for (; index < m_nCount; index++)
	{
		pTBB = GetButtonPtr(index);
		if (!(pTBB->nStyle & TBBS_SEPARATOR))
		{
			ActualW += pTBB->Size.cx;
			w += pTBB->Size.cx;
		}
		else break;
	}

	for (; index < m_nCount; index++) // trailing separators
	{
		pTBB = GetButtonPtr(index);
		if (pTBB->nStyle & TBBS_SEPARATOR)
			w += pTBB->Size.cx;
		else break;
	}
	return(w);
}

int COCToolBar::FitButtons(int AvailW, int MaxRows, OCGROUP* pGroup)
{
	COCButton*	pTBB;
	int	W, i,RowW = 0;
	int	Rows = 1;
	int Max = 0;
	int	S = pGroup->Start;
	int	E = pGroup->End;

	for (i=S; i < E; i++)
	{
		pTBB = GetButtonPtr(i);
		pTBB->nStyle &= ~TBBS_NEWLINE;
	}

	for (i=S; i < E; i++)
	{
		pTBB = GetButtonPtr(i);
		if (pTBB->nStyle & TBBS_SEPARATOR)
			continue;
		W = pTBB->Size.cx;
		if (!RowW || W+RowW <= AvailW)
	 		RowW += W;
		else
		{
			pTBB->nStyle |= TBBS_NEWLINE;
			if (RowW > Max)
				Max = RowW;
			Rows++;
			RowW = W;
		}
	}
	if (!Max)
		Max = pGroup->ActualWidth;
	if (Rows > MaxRows)
		Max = MAXINT;
	return(Max);
}

int COCToolBar::FitGroup(OCGROUP*	pGroup, int AvailW, int &ActualW)
{
	int		Max,Min = 0;
	int		Target = MAXINT;
	COCButton* 	pTBB;
	int			i,W,RowW;
	int			Rows = 1;

	RowW = W = 0;
	for (i=pGroup->Start; i < pGroup->End; i++)		// how many lines needed
	{
		pTBB = GetButtonPtr(i);
		if (pTBB->nStyle & TBBS_SEPARATOR)
			continue;
		W = pTBB->Size.cx;
		if (!W || W+RowW <= AvailW)
	 		RowW += W;
		else
		{
			Rows++;
			RowW = W;
		}
	}

	Max = pGroup->ActualWidth;
	RowW = AvailW / Rows;
	while (1)
	{
		W = FitButtons(RowW, Rows, pGroup);
		if (W <= RowW)
		{
			if (W < Target)
				Target = RowW;
			Max = RowW;
			W = (Min + RowW+1) / 2;
			if (W == RowW)
				break;
			RowW = W;
		}
		else if (W == MAXINT)
		{
			W = (Max + RowW+1) / 2;
			Min = RowW;
			if (W == RowW)
				break;
			RowW = W;
		}
		else
		{
			W = (Max + RowW+1) / 2;
			Min = RowW;
			if (W == RowW)
				break;
			RowW = W;
		}
	}
	if (Target == MAXINT)
		Target = RowW;
	ActualW = FitButtons(Target, Rows, pGroup);
	i=pGroup->Start;
	if (i)		// mark the beginning of every group as Line break except for the first
	{
		pTBB = GetButtonPtr(i);
		pTBB->nStyle |= TBBS_NEWLINE;
	}
	return(Rows);
}

int COCToolBar::GetSplitingWidth(int Width, int MaxRows)
{
	OCGROUP*	pGroup = m_pGroup;
	int			nRows = 0;
	int			w,MaxW;

	w = MaxW = 0;
	while (pGroup)
	{
		nRows += FitGroup(pGroup, Width, w);
		if ( w > MaxW)
			MaxW = w;
		pGroup = pGroup->pNext;
	}
	if (nRows < MaxRows)
		MaxW = MAXINT;
	if (nRows > MaxRows)
		MaxW = MAXINT + 1;
	return(MaxW);
}

int COCToolBar::GetNonSplitingWidth(int	Width, int MaxRows)
{
	OCGROUP*	pGroup = m_pGroup;
	COCButton*	pTBB;
	int			nRows = 1;
	int			ww,w,MaxW;
	BOOL		NewLine;
	int			RemGroups = m_nGroups;

	w = MaxW = 0;
	pTBB = GetButtonPtr(pGroup->Start);
	ww = pGroup->Width;
	while (1)
	{
		NewLine = (RemGroups <= (MaxRows - nRows)) && w;
		if (!NewLine && (!w || nRows == MaxRows || ww + w < Width))
		{
			w += ww;
			pGroup = pGroup->pNext;
			if (!pGroup)
				break;
			pTBB = GetButtonPtr(pGroup->Start);
			ww = pGroup->Width;
			RemGroups--;
		}
		else
		{
			pTBB->nStyle |= TBBS_NEWLINE;
			nRows++;
			if (w > MaxW)
				MaxW = w;
			w = 0;
		}
	}
	if (w > MaxW)
		MaxW = w;
	if (nRows < MaxRows)
		MaxW = MAXINT;
	if (nRows > MaxRows)
		MaxW = MAXINT + 1;
	return(MaxW);
}

int COCToolBar::GetNewWidth(int	Width, int MaxRows)
{
	ClearLineBreaks();
	return(m_Spliting ?GetSplitingWidth(Width, MaxRows) : GetNonSplitingWidth(Width, MaxRows));
}

int COCToolBar::Resize(int MaxRows)
{
	int		Max = m_TotalWidth;
	int		Min = 0;
	int		Cur,w;
	int		Target = MAXINT;
	BOOL	DidFit = FALSE;
	CRect	r;

	r.SetRectEmpty();        // only need top and left
	CalcInsideRect(r, TRUE);

	if (!m_nCount)
		return(TRUE);
	if (MaxRows == -1)
		MaxRows = m_nRows;
	Cur = Max / MaxRows;
	while (1)
	{
		w = GetNewWidth(Cur, MaxRows);
		if (w <= Cur)			// did fit in MaxRows
		{
			DidFit = TRUE;
			if (w < Target)
				Target = Cur;
			Max = Cur;
			w = (Min + Cur+1) / 2;
		}
		else if (w == MAXINT)	// did fit in < MaxRows
		{
			Max = Cur;
			w = (Min + Cur+1) / 2;
		}
		else					// did fit in > MaxRows
		{
			w = (Max + Cur+1) / 2;
			Min = Cur;
		}
		if (w == Cur)
			break;
		Cur = w;
	}
	if (Target == MAXINT)
		return(FALSE);
	GetNewWidth(Target, MaxRows);
	return(TRUE);
}

void COCToolBar::StartResizing()
{
	OCGROUP*	pGroup;
	COCButton*	pTBB;
	CRect		r;

	m_pGroup = pGroup = (OCGROUP*)malloc(sizeof(OCGROUP));
	m_nGroups = 0;
	m_TotalWidth = GetTotalWidth(); 
	for (int iButton = 0; iButton < m_nCount;)
	{		
		m_nGroups++;
		pGroup->pNext = (OCGROUP*)malloc(sizeof(OCGROUP));
		pGroup->End = iButton;
		pGroup = pGroup->pNext;
		pGroup->Start = iButton;
		pGroup->Width = GetWidthOfGroup(iButton, pGroup->ActualWidth);
	}
	pGroup->End = m_nCount;
	pGroup->pNext = NULL;
	pGroup = m_pGroup;
	m_pGroup = m_pGroup->pNext;
	free(pGroup);

	m_nButtons = 0;
	for (int i = 0; i < m_nCount; i++) // leading separators
	{
		pTBB = GetButtonPtr(i);
		if (!(pTBB->nStyle & TBBS_SEPARATOR))
			m_nButtons++;
	}
}

void COCToolBar::EndResizing()
{
	OCGROUP*	pGroup = m_pGroup;
	OCGROUP*	pPrevGroup;

	for (; pGroup;)
	{
		pPrevGroup = pGroup;
		pGroup = pGroup->pNext;
		free(pPrevGroup);
	}
	m_pGroup = NULL;
}

int COCToolBar::GetTotalWidth()
{
	CRect 	rect;
	CRect	r;
	int		w = 0;

	rect.SetRectEmpty();        // only need top and left
	CalcInsideRect(rect, TRUE);
	for (int iButton = 0; iButton < m_nCount; iButton++)
	{
		GetItemRect(iButton, &r);
		w += r.Width();
	}
	return(w - rect.Width());
}

void COCToolBar::SetCustomizing(BOOL f)
{
	COCButton*	pTBB;
	for (int i = 0; i < m_nCount; i++)
	{
		pTBB = GetButtonPtr(i);
		if (pTBB->nStyle & TBBS_WINDOW)
			::EnableWindow(pTBB->hWnd, !f);
	}
	if (m_WinButton != -1)
	{
		m_WinButton = -1;
		Invalidate();
		UpdateWindow();
	}
}


BOOL COCToolBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    CPoint pt;
    HWND hChild;

    GetCursorPos( &pt );
	if (pWnd == this && m_WinButton != -1)
	{
		pWnd->ScreenToClient(&pt);
		if (m_RightSize.PtInRect(pt))
		{
			SetCursor(LoadCursor(NULL,IDC_SIZEWE));
			return TRUE;
		}
	}

    // check for disabled control
    if( hChild = MyChildWindowFromPoint( pWnd->GetSafeHwnd(), pt ))
        return( SetupCursor( hChild, nHitTest, 0 ) );
	if (!SetupCursor( pWnd->GetSafeHwnd(), nHitTest, 0 ))
		return CControlBar::OnSetCursor(pWnd, nHitTest, message);
	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// COCToolBar diagnostics

#ifdef _DEBUG
void COCToolBar::AssertValid() const
{
	CControlBar::AssertValid();
}

void COCToolBar::Dump(CDumpContext& dc) const
{
	CControlBar::Dump(dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
// COCBitmap
/////////////////////////////////////////////////////////////////////////////

COCBitmap::COCBitmap(CSize Size)
{
	m_ImageSize = Size;
	m_ID 		 = 0;
}

COCBitmap::~COCBitmap()
{
}

HBITMAP COCBitmap::LoadBitmap(LPCTSTR lpszResourceName)
{
	HRSRC 		hRsrcImage;     // handle to loaded resource for image
	HINSTANCE 	hInstImage; 	// instance handle to load image from

	ASSERT(lpszResourceName != NULL);

	hInstImage = AfxFindResourceHandle(lpszResourceName, RT_BITMAP);
	if ((hRsrcImage = ::FindResource(hInstImage,lpszResourceName, RT_BITMAP)) == NULL)
		return FALSE;

	return (LoadSysColorBitmap(hInstImage,hRsrcImage));
}

HBITMAP COCBitmap::LoadBitmap(UINT ID)
{ 
	m_ID = ID;
	return LoadBitmap(MAKEINTRESOURCE(m_ID));
}


void COCBitmap::StartDraw()
{
	m_hbmOld = (HBITMAP)SelectObject(OCToolList->hDCGlyphs, GetSafeHandle());
}

void COCBitmap::EndDraw()
{
	SelectObject(OCToolList->hDCGlyphs, m_hbmOld);
}

BOOL COCBitmap::DrawOCButton(CDC* pDC, CPoint PtDst, CPoint PtSrc, CSize BSize)
{
	ASSERT_VALID(pDC);
	

	int dx = BSize.cx;
	int dy = BSize.cy;
	if (!OCToolList->bWin4)
	{
		// make the coordinates the interior of the button
		PtDst.x += 1;
		PtDst.y += 1;
		dx -= 2;
		dy -= 2;

		// border around button
		pDC->FillSolidRect(PtDst.x,    PtDst.y-1,    dx, 1,  OCToolList->clrWindowFrame);
		pDC->FillSolidRect(PtDst.x,    PtDst.y+dy,   dx, 1,  OCToolList->clrWindowFrame);
		pDC->FillSolidRect(PtDst.x-1,  PtDst.y,  1,  dy, OCToolList->clrWindowFrame);
		pDC->FillSolidRect(PtDst.x+dx, PtDst.y,  1,  dy, OCToolList->clrWindowFrame);
	}


	// interior grey
	pDC->FillSolidRect(PtDst.x, PtDst.y, dx, dy, OCToolList->clrBtnFace);

	// determine offset of bitmap (centered within button)
	CPoint ptOffset;
	ptOffset.x = (dx - m_ImageSize.cx - 1) / 2;
	ptOffset.y = (dy - m_ImageSize.cy) / 2;

	// regular button look
	pDC->Draw3dRect(PtDst.x, PtDst.y, dx, dy, OCToolList->clrBtnHilite,
		OCToolList->bWin4 ? OCToolList->clrWindowFrame : OCToolList->clrBtnShadow);
	pDC->Draw3dRect(PtDst.x + 1, PtDst.y + 1, dx - 2, dy - 2,
		OCToolList->clrBtnFace, OCToolList->clrBtnShadow);

	// normal image version
	BitBlt(pDC->m_hDC, PtDst.x + ptOffset.x, PtDst.y + ptOffset.y,
		m_ImageSize.cx, m_ImageSize.cy,
		OCToolList->hDCGlyphs, PtSrc.x, PtSrc.y, SRCCOPY);

	return TRUE;
}

/*
	DIBs use RGBQUAD format:
		0xbb 0xgg 0xrr 0x00

	Reasonably efficient code to convert a COLORREF into an
	RGBQUAD is byte-order-dependent, so we need different
	code depending on the byte order we're targeting.
*/
#define RGB_TO_RGBQUAD(r,g,b)   (RGB(b,g,r))
#define CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))

HBITMAP COCBitmap::LoadSysColorBitmap(HINSTANCE hInst, HRSRC hRsrc)
{
	struct COLORMAP
	{
		// use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
		DWORD rgbqFrom;
		int iSysColorTo;
	};
	static const COLORMAP sysColorMap[] =
	{
		// mapping from color in DIB to system color
		{ RGB_TO_RGBQUAD(0x00, 0x00, 0x00),  COLOR_BTNTEXT },       // black
		{ RGB_TO_RGBQUAD(0x80, 0x80, 0x80),  COLOR_BTNSHADOW },     // dark grey
		{ RGB_TO_RGBQUAD(0xC0, 0xC0, 0xC0),  COLOR_BTNFACE },       // bright grey
		{ RGB_TO_RGBQUAD(0xFF, 0xFF, 0xFF),  COLOR_BTNHIGHLIGHT }   // white
	};
	const int nMaps = 4;

	HGLOBAL hglb;
	if ((hglb = ::LoadResource(hInst, hRsrc)) == NULL)
		return NULL;

	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
	if (lpBitmap == NULL)
		return NULL;

	// make copy of BITMAPINFOHEADER so we can modify the color table
	const int nColorTableSize = 16;
	UINT nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc(nSize);
	if (lpBitmapInfo == NULL)
		return NULL;
	memcpy(lpBitmapInfo, lpBitmap, nSize);

	// color table is in RGBQUAD DIB format
	DWORD* pColorTable =
		(DWORD*)(((LPBYTE)lpBitmapInfo) + (UINT)lpBitmapInfo->biSize);

	for (int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		// look for matching RGBQUAD color in original
		for (int i = 0; i < nMaps; i++)
		{
			if (pColorTable[iColor] == sysColorMap[i].rgbqFrom)
			{
				pColorTable[iColor] =
					CLR_TO_RGBQUAD(::GetSysColor(sysColorMap[i].iSysColorTo));
				break;
			}
		}
	}

	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);
	::ReleaseDC(NULL, hDCScreen);

	if (hbm != NULL)
	{
		HBITMAP hbmOld = (HBITMAP)::SelectObject(OCToolList->hDCGlyphs, hbm);

		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

		StretchDIBits(OCToolList->hDCGlyphs, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(OCToolList->hDCGlyphs, hbmOld);

	}

	// free copy of bitmap info struct and resource itself
	::free(lpBitmapInfo);
	::FreeResource(hglb);

	SafeAttach(hbm);
	return hbm;
}

void COCBitmap::SafeAttach(HBITMAP hbm)
{
	if (GetSafeHandle())
		DeleteObject();
	Attach(hbm);
}

/////////////////////////////////////////////////////////////////////////////
// COCRectTracker
/////////////////////////////////////////////////////////////////////////////



COCRectTracker::COCRectTracker()
{
m_nStyle |= hatchedBorder;
}

SizingAction COCRectTracker::AdjustRight(LPRECT lpRect, int Rows, int Max)
{
	SizingAction	CurAction = SizeNone;
	int				delta1 = m_CurRect.right - lpRect->right;
	int				delta2 = m_CurRect.right - m_PrevRect.right;

	if (!BETWEEN(lpRect->right, m_CurRect.right))
	{
		if (BETWEEN(lpRect->right, m_PrevRect.right))
		{
			m_CurRect = m_PrevRect;
			CurAction = m_PrevAction == SizeExpand? SizeShrink : SizeExpand;
			m_PrevAction = SizeNone;
		}
		else if (m_PrevAction == SizeNone || delta1 * delta2 < 0 )	// other side of previous action
		{
			if (delta1 < 0)
				CurAction = Rows > 1? SizeShrink : SizeNone;
			else
				CurAction = Rows < Max? SizeExpand : SizeNone;
			m_PrevAction = CurAction;
		}
	}
return(CurAction);
}

SizingAction COCRectTracker::AdjustLeft(LPRECT lpRect, int Rows, int Max)
{
	SizingAction	CurAction = SizeNone;
	int				delta1 = m_CurRect.left - lpRect->left;
	int				delta2 = m_CurRect.left - m_PrevRect.left;

	if (!BETWEEN(lpRect->left, m_CurRect.left))
		if (BETWEEN(lpRect->left, m_PrevRect.left))
		{
			m_CurRect = m_PrevRect;
			CurAction = m_PrevAction == SizeExpand? SizeShrink : SizeExpand;
			m_PrevAction = SizeNone;
		}
		else if (m_PrevAction == SizeNone || delta1 * delta2 < 0 )	// other side of previous action
		{
			if (delta1 < 0)
				CurAction = Rows < Max? SizeExpand : SizeNone;
			else
				CurAction = Rows > 1? SizeShrink : SizeNone;
			m_PrevAction = CurAction;
		}
return(CurAction);
}

SizingAction COCRectTracker::AdjustBottom(LPRECT lpRect, int Rows, int Max)
{
	SizingAction	CurAction = SizeNone;
	int				delta1 = m_CurRect.bottom - lpRect->bottom;
	int				delta2 = m_CurRect.bottom - m_PrevRect.bottom;

	if (!BETWEEN(lpRect->bottom, m_CurRect.bottom))
		if (BETWEEN(lpRect->bottom, m_PrevRect.bottom))
		{
			m_CurRect = m_PrevRect;
			CurAction = m_PrevAction == SizeExpand? SizeShrink : SizeExpand;
			m_PrevAction = SizeNone;
		}
		else if (m_PrevAction == SizeNone || delta1 * delta2 < 0 )	// other side of previous action
		{
			if (delta1 < 0)
				CurAction = Rows < Max? SizeExpand : SizeNone;
			else
				CurAction = Rows > 1? SizeShrink : SizeNone;
			m_PrevAction = CurAction;
		}
return(CurAction);
}

SizingAction COCRectTracker::AdjustTop(LPRECT lpRect, int Rows, int Max)
{
	SizingAction	CurAction = SizeNone;
	int				delta1 = m_CurRect.top - lpRect->top;
	int				delta2 = m_CurRect.top - m_PrevRect.top;

	if (!BETWEEN(lpRect->top, m_CurRect.top))
		if (BETWEEN(lpRect->top, m_PrevRect.top))
		{
			m_CurRect = m_PrevRect;
			CurAction = m_PrevAction == SizeExpand? SizeShrink : SizeExpand;
			m_PrevAction = SizeNone;
		}
		else if (m_PrevAction == SizeNone || delta1 * delta2 < 0 )	// other side of previous action
		{
			if (delta1 < 0)
				CurAction = Rows > 1? SizeShrink : SizeNone;
			else
				CurAction = Rows < Max? SizeExpand : SizeNone;
			m_PrevAction = CurAction;
		}
return(CurAction);
}

void COCRectTracker::AdjustRect(int nHandle, LPRECT lpRect)
{
	SizingAction	CurAction;
	switch(nHandle)
	{
		case hitRight:
			CurAction = AdjustRight(lpRect, m_nNewRows, m_pBar->GetNumOfButtons());
			break;
		case hitLeft:
			CurAction = AdjustLeft(lpRect, m_nNewRows, m_pBar->GetNumOfButtons());
			break;
		case hitTop:
			CurAction = AdjustTop(lpRect, m_nNewRows, m_pBar->GetNumOfButtons());
			break;
		case hitBottom:
			CurAction = AdjustBottom(lpRect, m_nNewRows, m_pBar->GetNumOfButtons());
			break;
	}
	if (CurAction != SizeNone)
	{
		int		NewRows = m_pBar->GetNumOfRows(m_nNewRows, CurAction);
		if (NewRows != m_nNewRows)
		{
			m_nNewRows = NewRows;
			m_pBar->Resize(m_nNewRows);
			CSize Size = m_pBar->GetDockedSize(TRUE, FALSE);
			switch(nHandle)
			{
				case hitRight:
					lpRect->right  = lpRect->left + Size.cx - m_BarRect.Width();
					lpRect->bottom = lpRect->top  + Size.cy - m_BarRect.Height();
					break;
				case hitLeft:
					lpRect->left  = lpRect->right - Size.cx + m_BarRect.Width();
					lpRect->bottom = lpRect->top  + Size.cy - m_BarRect.Height();
					break;
				case hitTop:
					lpRect->top   = lpRect->bottom  - Size.cy + m_BarRect.Height();
					lpRect->right = lpRect->left + Size.cx - m_BarRect.Width();
					break;
				case hitBottom:
					lpRect->bottom = lpRect->top  + Size.cy - m_BarRect.Height();
					lpRect->right  = lpRect->left + Size.cx - m_BarRect.Width();
					break;
			}
			m_PrevRect = m_CurRect;
			m_CurRect  = *lpRect;
			return;
		}
	}
	*lpRect = m_CurRect;
}

BOOL COCRectTracker::StartTracking(COCToolBar *pBar, UINT flags)
{
	CRectTracker::TrackerHit		HitTest;
 	CRect			HandleRect;
	CPoint			point;
	CWnd*			pWnd;
	CWnd			Desktop;
	CWnd*			pDesktop;

	m_pBar = pBar;
	pWnd = pBar->GetDockingFrame();
	ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)));
	pWnd->GetWindowRect(&m_rect);
	pWnd->ScreenToClient(&m_rect);
	m_CurRect = m_InitRect = m_PrevRect = m_rect;
	pBar->GetWindowRect(&m_BarRect);
	pWnd->ScreenToClient(&m_BarRect);
	m_BarRect.left -= m_InitRect.left;
	m_BarRect.top -= m_InitRect.top;
	m_BarRect.right -= m_InitRect.right;
	m_BarRect.bottom -= m_InitRect.bottom;
	switch (flags)
	{
		case HTLEFT:
			HitTest = CRectTracker::hitLeft;
			break;
		case HTRIGHT:
			HitTest = CRectTracker::hitRight;
			break;
		case HTTOP:
			HitTest = CRectTracker::hitTop;
			break;
		case HTBOTTOM:
			HitTest = CRectTracker::hitBottom;
			break;
	}
	GetHandleRect(HitTest, &HandleRect);
	point = CPoint(HandleRect.left + HandleRect.Width() / 2,HandleRect.top + HandleRect.Height() / 2);
	pDesktop = CWnd::FromHandlePermanent(::GetDesktopWindow());
	if (!pDesktop)
	{
		pDesktop = &Desktop;
		Desktop.Attach(::GetDesktopWindow());
	}

	m_nNewRows = m_pBar->GetNumRows();
	m_PrevAction = SizeNone;
	pBar->StartResizing();
	BOOL ret = Track(pWnd, point, FALSE, pDesktop);
	if (ret)
		m_pBar->SetNumRows(m_nNewRows);
	pBar->Resize();
	pBar->EndResizing();
	if (pDesktop == &Desktop)
		Desktop.Detach();
	return(ret);
}

#undef new
IMPLEMENT_SERIAL(COCButton, CObject, 0)
