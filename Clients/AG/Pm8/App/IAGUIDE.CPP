/*
// $Workfile: IAGUIDE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IAGUIDE.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:42   Fred
// Initial revision.
// 
//    Rev 1.1   18 Jul 1996 17:18:28   Fred
// Handles sound button background
// 
//    Rev 1.0   14 Mar 1996 13:33:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:42   FRED
// Initial revision.
// 
//    Rev 2.2   07 Aug 1995 17:13:56   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:36:30   JAY
// Reverted. New series.
// 
//    Rev 1.0   18 Jul 1994 16:29:00   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#pragma warning(disable:4100)
#pragma warning(disable:4505)
#pragma warning(disable:4704)

#ifndef NOSTRICT
#define STRICT
#endif

#include "install.h"
#include "iaguide.h"
#include "ddb.h"
#include "sparkle.h"
#include "util.h"

#include "misc.h"
#include "error.h"
#include "soundmgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstallAudioGuideDialog dialog


CInstallAudioGuideDialog::CInstallAudioGuideDialog(CWnd* pParent /*=NULL*/, BOOL fMonochrome /*=FALSE*/)
	: CPmwDialog(CInstallAudioGuideDialog::IDD, pParent),
		m_fMonochrome(fMonochrome)
{
	//{{AFX_DATA_INIT(CInstallAudioGuideDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_pDDB = NULL;
	m_pSparklers = NULL;
	m_nTimerID = 0;
}

CInstallAudioGuideDialog::~CInstallAudioGuideDialog()
{
	if (m_nTimerID != 0)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	
	if (m_pSparklers != NULL)
	{
		CSparklers *p = m_pSparklers;
		m_pSparklers = NULL;
		delete p;
	}
	
	if (m_pDDB != NULL)
	{
		CDDB *p = m_pDDB;
		m_pDDB = NULL;
		delete p;
	}
}

void CInstallAudioGuideDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallAudioGuideDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstallAudioGuideDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CInstallAudioGuideDialog)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInstallAudioGuideDialog message handlers

void CInstallAudioGuideDialog::OnDestroy()
{
	CPmwDialog::OnDestroy();
}

BOOL CInstallAudioGuideDialog::OnInitDialog()
{
	CWindowDC dcWindow(this);
		
	m_pDDB = new CDDB(
						AfxGetInstanceHandle(),
						MAKEINTRESOURCE(IDB_AUDIO_GUIDE1),
						MAKEINTRESOURCE(IDB_AUDIO_GUIDE4),
						MAKEINTRESOURCE(IDB_AUDIO_GUIDE8),
						NULL,
						RT_BITMAP,
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
						
			CWnd *pcwndOk;
						
			if ((pcwndOk = GetDlgItem(IDOK)) != NULL)
			{
				CRect crOk;
							
				pcwndOk->GetWindowRect(&crOk);
							
				pcwndOk->MoveWindow(
					((Bitmap.bmWidth*1)/2)-(crOk.Width()/2),
					358-(crOk.Height()/2),
					crOk.Width(),
					crOk.Height(),
					TRUE);
			}
		}
			
		Util::CenterWindowInWindow(this);
	}
			
	CPmwDialog::OnInitDialog();

	CPmwSoundButton* pSoundButton;
	
	if ((pSoundButton = (CPmwSoundButton*)GetDlgItem(ID_SOUND_SPEAKER)) != NULL)
	{
		pSoundButton->SetBackgroundColor(TRUE);
	}
	if ((pSoundButton = (CPmwSoundButton*)GetDlgItem(ID_SOUND_STOP)) != NULL)
	{
		pSoundButton->SetBackgroundColor(TRUE);
	}
	if ((pSoundButton = (CPmwSoundButton*)GetDlgItem(ID_SOUND_PLAY)) != NULL)
	{
		pSoundButton->SetBackgroundColor(TRUE);
	}
	if ((pSoundButton = (CPmwSoundButton*)GetDlgItem(ID_SOUND_EXPLAIN)) != NULL)
	{
		pSoundButton->SetBackgroundColor(TRUE);
	}

	return TRUE;
}

BOOL CInstallAudioGuideDialog::OnEraseBkgnd(CDC* pDC)
{
	return CPmwDialog::OnEraseBkgnd(pDC);
}

#if 0
HBRUSH CInstallAudioGuideDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
 	if (nCtlColor == CTLCOLOR_BTN)
	{
		/*
		// This gets rid of white dots at button corners.
		*/
				
		pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
		pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
		return (HBRUSH)GetStockObject(BLACK_BRUSH);
	}
			
	return CPmwDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}
#endif

void CInstallAudioGuideDialog::OnPaint()
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
			
		if (m_pSparklers == NULL)
		{
			if (!m_fMonochrome)
			{
				m_pSparklers = new CSparklers;
					
				#define LOGICAL_BLUE RGB(0,0,255)
					
				static SPARKLEINFO SparkleInfo[] =
				{
					{ MAKEINTRESOURCE(IDB_SPARKLE), LOGICAL_BLUE, 31, 126,  85, 0, -50 },
					{ MAKEINTRESOURCE(IDB_SPARKLE), LOGICAL_BLUE, 31, 350, 175, 0, -50 },
					{ MAKEINTRESOURCE(IDB_SPARKLE), LOGICAL_BLUE, 31,  64,  92, 0, -50 },
					{ MAKEINTRESOURCE(IDB_SPARKLE), LOGICAL_BLUE, 31, 229, 215, 0, -50 },
					{ MAKEINTRESOURCE(IDB_SPARKLE), LOGICAL_BLUE, 31,  77,  44, 0, -50 },
					{ MAKEINTRESOURCE(IDB_SPARKLE), LOGICAL_BLUE, 31, 288, 223, 0, -50 }
				};
					
				const UINT wSparkleCount = sizeof(SparkleInfo)/sizeof(SparkleInfo[0]);
						
				m_pSparklers->Init(			
					AfxGetInstanceHandle(),		// instance containing resource
					m_hWnd,							// window in which to draw sparkle
					m_pDDB->GetBitmap(),			// handle of background bitmap
					m_pDDB->GetPalette(),		// palette to use
					wSparkleCount,					// number of sparkles in sparkle array
					&SparkleInfo[0]);				// array of sparkle information structure
						
				SetTimer(SPARKLE_TIMER_ID, 50, NULL);
			}
		}
	}
}

void CInstallAudioGuideDialog::OnPaletteChanged(CWnd* pFocusWnd)
{
	CPmwDialog::OnPaletteChanged(pFocusWnd);
}

BOOL CInstallAudioGuideDialog::OnQueryNewPalette()
{
	return CPmwDialog::OnQueryNewPalette();
}

void CInstallAudioGuideDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == SPARKLE_TIMER_ID)
	{
		if ((m_pSparklers != NULL) && (m_pSparklers->IsValid()))
		{
			m_pSparklers->Tick();
		}
	}

	CPmwDialog::OnTimer(nIDEvent);
}
