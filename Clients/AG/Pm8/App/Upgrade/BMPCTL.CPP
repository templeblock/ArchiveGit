/*
// $Workfile: BMPCTL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/BMPCTL.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:04   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 16:42:56   Fred
// New class names
// 
//    Rev 1.0   08 Sep 1996 09:40:20   Fred
// Initial revision.
// 
//    Rev 1.1   02 Feb 1996 13:44:02   FRED
// Added palette support to bitmap control
// 
//    Rev 1.1   02 Feb 1996 09:51:50   FRED
// Palette Support
// 
//    Rev 1.0   22 Nov 1995 08:52:02   FRED
// Initial revision.
// 
//    Rev 1.2   30 Oct 1995 12:11:46   FRED
//  
// 
//    Rev 1.1   26 Oct 1995 10:27:52   FRED
//  
// 
//    Rev 1.0   24 Oct 1995 16:55:14   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "bmpctl.h"
#include "ddb.h"

#ifndef WIN32
#include <auxdata.h>					// for AfxFindResourceHandle().
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapControl

WNDPROC CBitmapControl::m_pfnSuperWndProc = NULL;

LONG CALLBACK AFX_EXPORT CBitmapControl::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/

	CBitmapControl* pControl = new CBitmapControl;
	pControl->Attach(hWnd);

	pControl->m_fCreatedByClassName = TRUE;

	/*
	// Subclass the window by changing the window procedure to AfxWndProc
	// and pass the current message to it.
	*/

	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);

#ifdef STRICT
	return ::CallWindowProc((WNDPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#endif
}

BOOL CBitmapControl::RegisterMyClass()
{
	WNDCLASS wcls;

	m_pfnSuperWndProc = ::DefWindowProc;

	static const char szClassName[] = "InstantUpgradeBitmap";

	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CBitmapControl::FirstMsgOnlyWndProc);
	}

	wcls.style = 0;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CBitmapControl::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;

	return (RegisterClass(&wcls) != 0);
}

void CBitmapControl::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

CBitmapControl::CBitmapControl()
{
	m_fCreatedByClassName = FALSE;
	m_pDDB = NULL;
}

CBitmapControl::~CBitmapControl()
{
	FreeBitmapResource();
}

BEGIN_MESSAGE_MAP(CBitmapControl, CWnd)
	//{{AFX_MSG_MAP(CBitmapControl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapControl message handlers

void CBitmapControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect crUpdate(&dc.m_ps.rcPaint);

	if (!crUpdate.IsRectEmpty())
	{
		if (LoadBitmapResource())
		{
			CRect crClient;
			GetClientRect(&crClient);
		
			CRect crBitmap(0, 0, (int)m_nBitmapWidth, (int)m_nBitmapHeight);
			CRect crDraw;
	
			if (m_dwStyle & SBS_SIZEBITMAP)
			{
				crDraw = crClient;
			}
			else
			{
				crDraw = crBitmap;
				crDraw.OffsetRect(crClient.left, crClient.top);
			}

			int nOldMapMode = dc.SetMapMode(MM_TEXT);

			CPalette* pOldPalette;
			CPalette* pCurrentPalette = CPalette::FromHandle(m_pDDB->GetPalette());

			if ((pOldPalette = dc.SelectPalette(pCurrentPalette, FALSE)) != NULL)
			{
				dc.RealizePalette();

				CDC cdcBitmap;

				if ((cdcBitmap.CreateCompatibleDC(&dc)) != NULL)
				{
					CPalette *pOldBitmapPalette;

					if ((pOldBitmapPalette = cdcBitmap.SelectPalette(pCurrentPalette, FALSE)) != NULL)
					{
						cdcBitmap.RealizePalette();
						cdcBitmap.SetMapMode(MM_TEXT);

						CBitmap *pOldBitmap;

						if ((pOldBitmap = cdcBitmap.SelectObject(CBitmap::FromHandle(m_pDDB->GetBitmap()))) != NULL)
						{
							int nOldStretchMode = dc.SetStretchBltMode(STRETCH_DELETESCANS);

							dc.StretchBlt(
								crDraw.left,
								crDraw.top,
								crDraw.Width(),
								crDraw.Height(),
								&cdcBitmap,
								0,
								0,
								m_nBitmapWidth,
								m_nBitmapHeight,
								SRCCOPY);
							
							dc.SetStretchBltMode(nOldStretchMode);
							cdcBitmap.SelectObject(pOldBitmap);
						}
						
						cdcBitmap.SelectPalette(pOldBitmapPalette, FALSE);
					}
				}

				dc.SelectPalette(pOldPalette, FALSE);
			}

			dc.SetMapMode(nOldMapMode);
		}
	}
}

BOOL CBitmapControl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

int CBitmapControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// by default, associate with our parent
	m_pcwAssociate = GetParent();

	// copy style DWORD
	m_dwStyle = lpCreateStruct->style;

	// Get bitmap and set our size.
	SetBitmapResource();

	return 0;
}

LRESULT CBitmapControl::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	if (lResult > 0)
	{
		SetBitmapResource();
	}
	return Default();
}

BOOL CBitmapControl::SetBitmapResource(void)
{
	FreeBitmapResource();
	
	if (LoadBitmapResource())
	{
		if (!(m_dwStyle & SBS_NOSIZECONTROL))
		{
			int nWidth = m_nBitmapWidth;
			int nHeight = m_nBitmapHeight;

			CRect crClient;
			GetClientRect(&crClient);

			int nDeltaX = nWidth - crClient.Width();
			int nDeltaY = nHeight - crClient.Height();
			int nDeltaLeft = nDeltaX/2;
			int nDeltaRight = (nDeltaX+1)/2;
			int nDeltaTop = nDeltaY/2;
			int nDeltaBottom = (nDeltaY+1)/2;

			CRect crWindow; 
			GetWindowRect(&crWindow);
			GetParent()->ScreenToClient(crWindow);

			crWindow.left -= nDeltaLeft;
			crWindow.top -= nDeltaTop;
			crWindow.right += nDeltaRight;
			crWindow.bottom += nDeltaBottom;

			SetWindowPos(NULL,
							 crWindow.left,
							 crWindow.top,
							 crWindow.Width(),
							 crWindow.Height(),
							 SWP_NOZORDER);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CBitmapControl::LoadBitmapResource(void)
{
	BOOL fResult = TRUE;
	
	if (m_pDDB == NULL)
	{
		fResult = FALSE;
		
		CString csName;
		
		TRY
		{
			// Get the resource name from the window title.
			GetWindowText(csName);

			CWindowDC dcWindow(this);
	
			m_pDDB = new CDDB(
				AfxGetInstanceHandle(),
				csName,
				RT_BITMAP,
				dcWindow.GetSafeHdc(),
				TRUE);
				
			if ((m_pDDB != NULL) && (m_pDDB->IsValid()))
			{
				// Get size of bitmap.

				BITMAP Bitmap;

				if (CBitmap::FromHandle(m_pDDB->GetBitmap())->GetObject(sizeof(Bitmap), &Bitmap) != 0)
				{
					m_nBitmapWidth = Bitmap.bmWidth;
					m_nBitmapHeight = Bitmap.bmHeight;

					fResult = TRUE;
				}
			}
		}
		END_TRY
	}
	
	return fResult;
}

void CBitmapControl::FreeBitmapResource(void)
{
	delete m_pDDB;
	m_pDDB = NULL;
}

