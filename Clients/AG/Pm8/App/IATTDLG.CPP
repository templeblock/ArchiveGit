/*
// $Workfile: IATTDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/IATTDLG.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:44   Fred
// Initial revision.
// 
//    Rev 1.0   20 Aug 1996 18:21:00   Fred
// AT&T Installation
*/

#include "stdafx.h"
#include "installr.h"
#include "iattdlg.h"

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CAttWorldNetServiceDialog dialog

CAttWorldNetServiceDialog::CAttWorldNetServiceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAttWorldNetServiceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAttWorldNetServiceDialog)
	//}}AFX_DATA_INIT
	m_pDDB = NULL;
}

CAttWorldNetServiceDialog::~CAttWorldNetServiceDialog()
{
	delete m_pDDB;
	m_pDDB = NULL;
}

void CAttWorldNetServiceDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttWorldNetServiceDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAttWorldNetServiceDialog, CDialog)
	//{{AFX_MSG_MAP(CAttWorldNetServiceDialog)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttWorldNetServiceDialog message handlers

BOOL CAttWorldNetServiceDialog::OnInitDialog() 
{
	INHERITED::OnInitDialog();
	
	CWindowDC dcWindow(this);
		
	m_pDDB = new CDDB(
						AfxGetInstanceHandle(),
						NULL,
						NULL,
						MAKEINTRESOURCE(IDB_ATTWORLDNET),
						NULL,
						RT_BITMAP,
						dcWindow.GetSafeHdc(),
						TRUE);

#ifndef WIN32
	CenterWindow();		// Automatic in MFC 4.0
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAttWorldNetServiceDialog::OnEraseBkgnd(CDC* pDC) 
{
	INHERITED::OnEraseBkgnd(pDC);

	if ((m_pDDB != NULL) && (m_pDDB->IsValid()))
	{
		BITMAP Bitmap;
									
		if (CBitmap::FromHandle(m_pDDB->GetBitmap())->GetObject(sizeof(Bitmap), &Bitmap) != 0)
		{
			CWnd* pWnd = GetDlgItem(IDC_ATTWORLDNETBMP);
			if (pWnd != NULL)
			{
				CRect crWindow;
				pWnd->GetWindowRect(crWindow);
				ScreenToClient(crWindow);
				CRect crBitmap(0, 0, Bitmap.bmWidth, Bitmap.bmHeight);
				crBitmap.OffsetRect(crWindow.left, crWindow.top);

	  			CPalette *pcpOld;
  				if ((pcpOld = pDC->SelectPalette(CPalette::FromHandle(m_pDDB->GetPalette()), FALSE)) != NULL)
				{
					pDC->RealizePalette();
					
					CDC cdcBitmap;
					if ((cdcBitmap.CreateCompatibleDC(pDC)) != NULL)
					{
						CBitmap *pcbmOld;
							
						if ((pcbmOld = cdcBitmap.SelectObject(CBitmap::FromHandle(m_pDDB->GetBitmap()))) != NULL)
						{
							pDC->BitBlt(
								crBitmap.left,
								crBitmap.top,
								crBitmap.Width(),
								crBitmap.Height(),
								&cdcBitmap,
								0,
								0,
								SRCCOPY);
									
							cdcBitmap.SelectObject(pcbmOld);
						}
					}
					pDC->SelectPalette(pcpOld, FALSE);
					pDC->RealizePalette();
				}
			}
		}
	}

	return TRUE;
}
