/*
// $Workfile: IABOUT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/IABOUT.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     6/29/98 9:35a Mwilson
// changed to load bitmap from file instead of resource
// 
// 2     4/08/98 12:34p Fredf
// New explorer progress indicator.
// 
//    Rev 1.0   14 Aug 1997 15:21:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:42   Fred
// Initial revision.
// 
//    Rev 1.10   29 May 1997 15:09:32   Fred
//  
// 
//    Rev 1.9   19 May 1997 13:55:44   Fred
// Event Reminder, etc.
// 
//    Rev 1.8   25 Apr 1997 14:35:02   Fred
//  
// 
//    Rev 1.7   25 Apr 1997 14:22:58   Fred
// Removed introduction
// 
//    Rev 1.6   21 Apr 1997 16:01:46   Jay
// 4.0 changes
// 
//    Rev 1.5   22 Aug 1996 15:59:08   Fred
// Introduction
// 
//    Rev 1.4   21 Aug 1996 15:11:16   Fred
//  
// 
//    Rev 1.3   15 Aug 1996 12:56:32   Fred
// New PMW 3.0 splash screen
// 
//    Rev 1.2   07 May 1996 12:43:44   Jay
// From PMW2
// 
//    Rev 2.7   26 Apr 1996 13:08:50   JAY
// More demo code
// 
//    Rev 2.6   24 Apr 1996 16:43:14   JAY
// New upgradable demo value.
// 
//    Rev 2.5   19 Apr 1996 08:54:28   JAY
// New button scheme. Install is always available.
// 
//    Rev 2.4   05 Feb 1996 14:09:06   FRED
// Dynamically displays program title is about box bitmap
// 
//    Rev 2.3   26 Jan 1996 08:27:58   FRED
// Added Autoplay support to installer.
// 
//    Rev 2.2   07 Aug 1995 17:12:36   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:36:28   JAY
// Reverted. New series.
// 
//    Rev 1.7   19 Jul 1994 09:03:06   JAY
// INTRO.WAV  -> S6V2000.WAV
// INTRO8.WAV -> S8V2000.WAV
// 
//    Rev 1.6   15 Jul 1994 18:00:22   FRED
// Tweaked location of Start Installation button.
// No sparkles for monochrome mode.
// 
// 
//    Rev 1.5   15 Jul 1994 16:18:54   FRED
// 
//    Rev 1.4   15 Jul 1994 14:10:28   JAY
// Moved the sounds for the installation program.
// 
//    Rev 1.3   15 Jul 1994 14:07:06   FRED
// Initial support for "No Sound" button.
// 
// 
//    Rev 1.2   15 Jul 1994 10:39:56   JAY
// Support for monochrome sound buttons.
// 
//    Rev 1.1   29 Jun 1994 16:24:40   JAY
// Added initial sound.
// 
//    Rev 1.0   15 Jun 1994 19:00:52   FRED
// Initial revision.
*/ 

#pragma warning(disable:4100)
#pragma warning(disable:4505)
#pragma warning(disable:4704)

#ifndef NOSTRICT
#define STRICT
#endif

#include "stdafx.h"

#include "install.h"
#include "iabout.h"
#include "ddb.h"
#include "util.h"
#include "pmwcfg.h"

#include "misc.h"
#include "error.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstallAboutDialog dialog


CInstallAboutDialog::CInstallAboutDialog(BOOL fInstalled, CWnd* pParent /*=NULL*/, BOOL fMonochrome /*=FALSE*/)
	: CDialog(CInstallAboutDialog::IDD, pParent),
		m_fMonochrome(fMonochrome)
{
	//{{AFX_DATA_INIT(CInstallAboutDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_pDDB = NULL;
	m_nTimerID = 0;
	m_fInstalled = fInstalled;
}

CInstallAboutDialog::~CInstallAboutDialog()
{
	if (m_nTimerID != 0)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	
	delete m_pDDB;
	m_pDDB = NULL;
}

void CInstallAboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallAboutDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstallAboutDialog, CDialog)
	//{{AFX_MSG_MAP(CInstallAboutDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_INSTALL, OnInstall)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_RUN, OnRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInstallAboutDialog message handlers



BOOL CInstallAboutDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWindowDC dcWindow(this);
	CString csBmpFile;
	csBmpFile.LoadString(IDS_INSTALL_BMP_FILE);
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);

	CString csBmpPath(path); 
	csBmpPath = csBmpPath.Left(csBmpPath.ReverseFind('\\') + 1) + csBmpFile;
	
	m_pDDB = new CDDB(	csBmpPath,
						csBmpPath,
						csBmpPath,
						NULL,
						dcWindow.GetSafeHdc(),
						TRUE);
				
	if ((m_pDDB != NULL) && (m_pDDB->IsValid()))
	{
		BITMAP Bitmap;
									
		if (CBitmap::FromHandle(m_pDDB->GetBitmap())->GetObject(sizeof(Bitmap), &Bitmap) != 0)
		{
			CRect crWindow;
			GetWindowRect(&crWindow);
				
			CRect crClient;
			GetClientRect(&crClient);
				
			MoveWindow(
				0,
				0,
				Bitmap.bmWidth+crWindow.Width()-crClient.Width(),
				Bitmap.bmHeight+crWindow.Height()-crClient.Height(),
				TRUE);

			GetClientRect(&crClient);
						
			// Configure the buttons.
			
			CWnd* pRun = GetDlgItem(IDC_RUN);
			CWnd* pInstall = GetDlgItem(IDC_INSTALL);
			CWnd* pRemove = GetDlgItem(IDC_REMOVE);
			CWnd* pCancel = GetDlgItem(IDCANCEL);

			if ((pRun != NULL)
			 && (pInstall != NULL)
			 && (pRemove != NULL)
			 && (pCancel != NULL))
			{
				int nButtons = 4;

				CRect crRun;
				CRect crInstall;
				CRect crRemove;
				CRect crCancel;
							
				pRun->GetWindowRect(&crRun);
				pInstall->GetWindowRect(&crInstall);
				pRemove->GetWindowRect(&crRemove);
				pCancel->GetWindowRect(&crCancel);
							
				if (m_fInstalled)
				{
					// PrintMaster is already installed. We offer the user the
					// choice of running the program, installing the program,
					// removing the program, running the introduction,
					// or leaving the installation program.

					CString csReinstall;
					TRY
					{
						csReinstall.LoadString(IDS_Reinstall);
						pInstall->SetWindowText(csReinstall);
					}
					END_TRY
				}
				else
				{
					// PrintMaster is not installed. We offer the user the
					// choice of installing or leaving the installation program,
					// or running the introduction.

					// Get rid of "run"... it doesn't exist yet!
					pRun->ShowWindow(SW_HIDE);
					pRun = NULL;
					crRun.SetRectEmpty();

					// Get rid of "remove"... it doesn't exist yet!
					pRemove->ShowWindow(SW_HIDE);
					pRemove = NULL;
					crRemove.SetRectEmpty();

					nButtons = 2;
				}

				// Position the buttons.

				int nSpacing = (crClient.Width()-crRun.Width()-crInstall.Width()-crRemove.Width()-crCancel.Width()) / (nButtons+1);

				int nX = crClient.left;
				
				if (pRun != NULL)
				{
					nX += nSpacing;

					pRun->MoveWindow(
						nX,
						325-(crRun.Height()/2),
						crRun.Width(),
						crRun.Height(),
						TRUE);

					nX += crRun.Width();
				}
				
				if (pInstall != NULL)
				{
					nX += nSpacing;

					pInstall->MoveWindow(
						nX,
						325-(crInstall.Height()/2),
						crInstall.Width(),
						crInstall.Height(),
						TRUE);

					nX += crInstall.Width();
				}
				
				if (pRemove != NULL)
				{
					nX += nSpacing;

					pRemove->MoveWindow(
						nX,
						325-(crRemove.Height()/2),
						crRemove.Width(),
						crRemove.Height(),
						TRUE);

					nX += crRemove.Width();
				}
				
				if (pCancel != NULL)
				{
					nX += nSpacing;

					pCancel->MoveWindow(
						nX,
						325-(crCancel.Height()/2),
						crCancel.Width(),
						crCancel.Height(),
						TRUE);

					nX += crCancel.Width();
				}
			}
		}

		GetConfiguration()->ReplaceDialogText(this);
		CenterWindow();
	}
			
	return TRUE;
}

void CInstallAboutDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if ((m_pDDB != NULL) && (m_pDDB->IsValid()))
	{
		CRect crUpdate(&dc.m_ps.rcPaint);
		
		if (!crUpdate.IsRectEmpty())
		{
			int nOldMapMode;
				
			nOldMapMode = dc.SetMapMode(MM_TEXT);
   
  			CPalette *pcpOld;
  			
			if ((pcpOld = dc.SelectPalette(CPalette::FromHandle(m_pDDB->GetPalette()), FALSE)) != NULL)
			{
				dc.RealizePalette();
					
				CDC cdcBitmap;
				
				if ((cdcBitmap.CreateCompatibleDC(&dc)) != NULL)
				{
					cdcBitmap.SetMapMode(MM_TEXT);
					
					CBitmap *pcbmOld;
						
					if ((pcbmOld = cdcBitmap.SelectObject(CBitmap::FromHandle(m_pDDB->GetBitmap()))) != NULL)
					{
						dc.BitBlt(
							crUpdate.left,
							crUpdate.top,
							crUpdate.Width(),
							crUpdate.Height(),
							&cdcBitmap,
							crUpdate.left,
							crUpdate.top,
							SRCCOPY);
															
						cdcBitmap.SelectObject(pcbmOld);
					}
				}
					
				dc.SelectPalette(pcpOld, FALSE);
			}
				
			dc.SetMapMode(nOldMapMode);
		}

	}
}

BOOL CInstallAboutDialog::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

void CInstallAboutDialog::OnPaletteChanged(CWnd* pFocusWnd)
{
	CDialog::OnPaletteChanged(pFocusWnd);
}

BOOL CInstallAboutDialog::OnQueryNewPalette()
{
	return CDialog::OnQueryNewPalette();
}
	
void CInstallAboutDialog::OnTimer(UINT nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

void CInstallAboutDialog::OnDestroy()
{
	CDialog::OnDestroy();
}

void CInstallAboutDialog::OnInstall()
{
	EndDialog(IDC_INSTALL);
}

void CInstallAboutDialog::OnRemove()
{
	EndDialog(IDC_REMOVE);
}

void CInstallAboutDialog::OnRun()
{
	EndDialog(IDC_RUN);
}
