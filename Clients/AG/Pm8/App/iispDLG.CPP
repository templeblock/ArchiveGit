/*
// $Workfile: iispDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/iispDLG.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     6/24/98 8:26a Jayn
// 
// 2     3/26/98 10:29a Fredf
// Changes to install program to install Internet Explorer 4.0 and the Web
// Publishing Wizard.
// 
// 1     3/25/98 2:45p Fredf
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
#include "iispdlg.h"
#include "pmwcfg.h"

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CIspDialog dialog

CIspDialog::CIspDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CIspDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIspDialog)
	//}}AFX_DATA_INIT
	m_pDDB = NULL;
}

CIspDialog::~CIspDialog()
{
	delete m_pDDB;
	m_pDDB = NULL;
}

void CIspDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIspDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIspDialog, CDialog)
	//{{AFX_MSG_MAP(CIspDialog)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIspDialog message handlers

BOOL CIspDialog::OnInitDialog() 
{
	INHERITED::OnInitDialog();
	
	GetConfiguration()->ReplaceDialogText(this);

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

BOOL CIspDialog::OnEraseBkgnd(CDC* pDC) 
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
