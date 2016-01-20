/*
// $Workfile: TutorDlg.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TutorDlg.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 15    12/04/98 5:31p Gbeddow
// directory reorganization in preparation for integrating Print Shop
// 
// 14    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 13    7/26/98 7:05p Hforman
// changed baseclass from CDialog to CPmwDialog for palette handling
// 
// 12    7/24/98 6:30p Jayn
// Added a ReleaseDC for a GetDC().
// 
// 11    7/24/98 4:37p Hforman
// put string in string table
// 
// 10    7/21/98 2:51p Johno
// Relative and "[[" paths are now used for tutorials -
// path_manager.make_full_path() will prompt for CD when they are expanded
// 
// 9     7/08/98 1:15p Hforman
// fixed bug with size of dialog between mini/design tutorials
// 
// 8     5/15/98 4:42p Fredf
// 
// 7     4/21/98 4:01p Rlovejoy
// Changes to put buttons on top of bitmap.
// 
// 6     4/17/98 4:06p Rlovejoy
// Fixed spacing.
// 
// 5     4/03/98 3:55p Rlovejoy
// Use compressed bitmap resources.
// 
// 4     11/24/97 3:31p Hforman
// fixed problem in ShowTutorial()
// 
// 3     11/06/97 6:04p Hforman
// minor fixes to ShowTutorial()
 * 
 * 2     10/30/97 6:52p Hforman
 * finishing up functionality, etc.
*/

#include "stdafx.h"
#include <afxpriv.h> // for WM_DISABLEMODAL
#include "pmw.h"
#include "mainfrm.h"
#include "sortarry.h"
#include "TutRes.h"
#include "TutList.h"
#include "TutorDlg.h"
#include "filcmprs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTutorialDlg dialog


CTutorialDlg::CTutorialDlg(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CTutorialDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTutorialDlg)
	//}}AFX_DATA_INIT

   m_hLib = NULL;
}

CTutorialDlg::~CTutorialDlg()
{
   if (m_hLib)
   {
      FreeLibrary(m_hLib);
      m_hLib = NULL;
   }

	// Free decompressed bitmaps
	CBitmapFileCompress Decompressor;
	int i, nBitmaps = m_bitmapArray.GetSize();
	for (i = 0; i < nBitmaps; i++)
	{
		Decompressor.FreeData((LPBYTE)m_bitmapArray[i]);
	}

}

void CTutorialDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTutorialDlg)
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_TUT_NEXT, m_btnNext);
	DDX_Control(pDX, IDC_TUT_BACK, m_btnBack);
	DDX_Control(pDX, IDC_TUT_BITMAP, m_ctrlBitmap);
	//}}AFX_DATA_MAP
}

void CTutorialDlg::ShowTutorial(int nTutorialID)
{
   BOOL fOK = FALSE;

	CWaitCursor wait;

   // Open the tutorial DLL, get the title, list of pages,
   // and the bitmaps for each page
   CTutorialList* pList = ((CMainFrame*)AfxGetMainWnd())->GetTutorialList();
	LPCSTR pszPath = NULL;
	LPCTSTR pszCaption = NULL;
	if (pList->GetInfoFromID(nTutorialID, &pszCaption, NULL, NULL, &pszPath))
	{
		CString csPath = GetGlobalPathManager()->LocatePath(pszPath);

		HINSTANCE hLib = LoadLibrary(csPath);
		if (hLib)
		{
			m_csCaption = pszCaption;

			// Parse info in resource string to get bitmap resource names
			CStringArray nameArray;
			char buf[512];
			LoadString(hLib, IDS_TUTORIAL_PAGES, buf, sizeof(buf));
			CMlsStringArray csaPages;
			csaPages.SetNames(buf);

			// Now load all the page bitmaps
			CBitmapFileCompress Decompressor;
			m_numPages = csaPages.GetSize();
			m_nCurPage = 0;
			m_bitmapArray.RemoveAll();
			for (int i = 0; i < m_numPages; i++)
			{
				// Load & decompress the bitmaps
				LPBYTE pBitmap;
				DWORD dwBitmapSize;
				if (Decompressor.Decompress(hLib, (LPCSTR)csaPages[i], &pBitmap, &dwBitmapSize))
				{
					if (pBitmap)
					{
						m_bitmapArray.Add((LPBITMAPINFO)pBitmap);
						fOK = TRUE;
					}
				}
				else // we've got a problem
				{
					fOK = FALSE;
					break;
				}
			}
		}

		if (fOK)
		{
			// close any already open tutorial lib
			if (m_hLib)
			{
				FreeLibrary(m_hLib);
				m_hLib = NULL;
			}
			m_hLib = hLib;

			if (m_hWnd == NULL)
				Create(IDD_TUTORIAL, AfxGetMainWnd());
			else
				ResetDialog();
		}
		else
		{
			if (hLib)
				FreeLibrary(hLib);
			AfxMessageBox(IDS_TUTORIAL_ERROR_LOADING);
		}
	}
}

void CTutorialDlg::ResetDialog()
{
   // resize dialog, position buttons, etc.
   if (m_bitmapArray.GetSize() > 0)
   {
      LPBITMAPINFOHEADER pBitmap = (LPBITMAPINFOHEADER)m_bitmapArray[0];
      int bmwidth = pBitmap->biWidth;
      int bmheight = pBitmap->biHeight;
      m_ctrlBitmap.SetWindowPos(NULL, 0, 0, bmwidth, bmheight,
                                SWP_NOMOVE | SWP_NOZORDER);

      CRect rcBtn;
      m_btnCancel.GetClientRect(&rcBtn);
      int captionHeight = GetSystemMetrics(SM_CYCAPTION);

		// Adjust for caption
		CRect rcBitmapAdjust;
		m_ctrlBitmap.GetClientRect(&rcBitmapAdjust);
		::AdjustWindowRect(&rcBitmapAdjust, WS_VISIBLE | WS_POPUP | WS_CAPTION, FALSE);

		// reset these baseclass members so CPmwDialog doesn't muck with font size!
		m_nNormalWidth = m_nNormalHeight = 0;
      SetWindowPos(NULL, 0, 0, rcBitmapAdjust.Width(), rcBitmapAdjust.Height(),
                   SWP_NOMOVE | SWP_NOZORDER);

      // position buttons
      CRect rcDlg, rcCtrl;
      GetClientRect(rcDlg);
      m_ctrlBitmap.GetClientRect(rcCtrl);

      int ypos = rcCtrl.bottom - 6;

      m_btnCancel.GetClientRect(rcCtrl);
      int rightPos = rcDlg.right - rcCtrl.Width() - 6;

      m_btnCancel.SetWindowPos(NULL, rightPos, ypos - rcCtrl.Height(), 0, 0,
                               SWP_NOSIZE | SWP_NOZORDER);
      m_btnNext.GetClientRect(rcCtrl);
      rightPos -= (rcCtrl.Width() + 96);
      m_btnNext.SetWindowPos(NULL, rightPos, ypos - rcCtrl.Height(), 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER);
      m_btnBack.GetClientRect(rcCtrl);
      rightPos -= rcCtrl.Width();
      m_btnBack.SetWindowPos(NULL, rightPos, ypos - rcCtrl.Height(), 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER);
   }

   // set the title in the titlebar
	SetTitle();
   UpdateData(FALSE);

	if (m_numPages > 1)
	{
		// Multiple pages.
	   // enable/disable buttons for Page 1
		m_btnBack.EnableWindow(FALSE);
		m_btnBack.ShowWindow(SW_SHOW);
		m_btnNext.EnableWindow(m_numPages > 1);
		m_btnNext.ShowWindow(SW_SHOW);
		SetButtonFocus((m_numPages > 1) ? m_btnNext : m_btnCancel);
	}
	else
	{
		// Single page. Hide Next and Back buttons.
		m_btnBack.EnableWindow(FALSE);
		m_btnBack.ShowWindow(SW_HIDE);
		m_btnNext.EnableWindow(FALSE);
		m_btnNext.ShowWindow(SW_HIDE);
		SetButtonFocus(m_btnCancel);
	}

   Invalidate(FALSE);
}

void CTutorialDlg::SetTitle(void)
{
	CString csFormat;
	CString csTitle;
	TRY
	{
//		csFormat.LoadString(m_numPages == 1 ? IDS_TUTORIAL_TITLE_SINGLE_PAGE : IDS_DESIGN_TUTORIAL_TITLE);
		if (m_numPages == 1)
		{
			csFormat = "%s";
		}
		else
		{
			csFormat = "%s  (Page %d of %d)";
		}
		csTitle.Format(csFormat, (LPCSTR)m_csCaption, m_nCurPage+1, m_numPages);
		SetWindowText(csTitle);
	}
	END_TRY
}

void CTutorialDlg::SetButtonFocus(CButton& btn)
{
	btn.SetFocus();
	SetDefID(btn.GetDlgCtrlID());
}

BEGIN_MESSAGE_MAP(CTutorialDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CTutorialDlg)
   ON_BN_CLICKED(IDC_TUT_BACK, OnTutBack)
	ON_BN_CLICKED(IDC_TUT_NEXT, OnTutNext)
	ON_WM_PAINT()
	ON_MESSAGE(WM_DISABLEMODAL, OnDisableModal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTutorialDlg message handlers

BOOL CTutorialDlg::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	ResetDialog();
	CenterWindow();

	return FALSE;  // return FALSE since I set the focus to a control
}

void CTutorialDlg::OnPaint() 
{
   CPaintDC dc(this); // device context for painting
	
   if (m_nCurPage > m_bitmapArray.GetUpperBound())
      return;  // bad news

   // copy bitmap to control
   LPBITMAPINFOHEADER pBitmap = (LPBITMAPINFOHEADER)m_bitmapArray[m_nCurPage];
   CDC* pDC = m_ctrlBitmap.GetDC();
	if (pDC != NULL)
	{
		// Use our global PrintMaster palette.
		extern CPalette* pOurPal;
		CPalette* pOldPal = pDC->SelectPalette(pOurPal, FALSE);
		if (pOldPal != NULL)
		{
			pDC->RealizePalette();
		}

		LPBYTE pBitmapData = (LPBYTE)pBitmap + sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD);
		::SetDIBitsToDevice(pDC->GetSafeHdc(),
								  1, 1,              // dest x, y
								  pBitmap->biWidth,
								  pBitmap->biHeight,
								  0, 0,              // source x, y
								  0,                 // first scan line
								  pBitmap->biHeight, // # of scan lines
								  pBitmapData,
								  (LPBITMAPINFO)pBitmap,
								  DIB_RGB_COLORS);

		if (pOldPal != NULL)
		{
			pDC->SelectPalette(pOldPal, FALSE);
		}

		m_ctrlBitmap.ReleaseDC(pDC);
	}

	// Make sure buttons are on top
	m_btnCancel.Invalidate();
	m_btnBack.Invalidate();
	m_btnNext.Invalidate();
}

void CTutorialDlg::OnTutNext() 
{
   if (m_nCurPage < m_numPages-1)
   {
      ++m_nCurPage;
      UpdateData(FALSE);
      m_btnBack.EnableWindow(TRUE);
      m_btnNext.SetFocus();    // in case they used accelerator
      Invalidate(FALSE);
   }
   if (m_nCurPage == m_numPages-1)
   {
      m_btnNext.EnableWindow(FALSE);
		SetButtonFocus(m_btnBack);
   }
	SetTitle();
}

void CTutorialDlg::OnTutBack() 
{
   m_btnBack.SetFocus();    // in case they used accelerator
   if (m_nCurPage > 0)
   {
      --m_nCurPage;
      UpdateData(FALSE);
      Invalidate(FALSE);
      m_btnNext.EnableWindow(TRUE);
   }
   if (m_nCurPage == 0)
   {
      m_btnBack.EnableWindow(FALSE);
		SetButtonFocus(m_btnNext);
   }
	SetTitle();
}

void CTutorialDlg::OnCancel() 
{
	DestroyWindow();
}

// override default CPmwDialog::OnOK(), since it will
// make our Modeless dialog box invisible
void CTutorialDlg::OnOK()
{
}

// override default OnDisableModal() since we want
// this dialog to always be enabled
LRESULT CTutorialDlg::OnDisableModal(WPARAM, LPARAM)
{
   return TRUE;   // don't disable
}

