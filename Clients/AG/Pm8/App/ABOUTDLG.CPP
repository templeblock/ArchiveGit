/*
// $Header: /PM8/App/ABOUTDLG.CPP 1     3/03/99 6:01p Gbeddow $
//
// About dialog.
//
// $Log: /PM8/App/ABOUTDLG.CPP $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 15    9/28/98 10:50a Johno
// Use non - static GetHubDLLName()
// 
// 14    9/14/98 12:05p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 13    8/12/98 5:03p Psasse
// Fixed a kludge with the dynamic resizing of the intro dialog
// 
// 12    7/21/98 11:16a Mwilson
// changed character id
// 
// 11    7/11/98 2:47p Jayn
// ReleaseDC() not called on same window as GetDC().
// 
// 10    7/01/98 5:28p Mwilson
// changed icon to bitmap
// 
// 9     6/18/98 7:04p Psasse
// re-center startup dialog after re-sizing
// 
// 8     6/17/98 2:43p Psasse
// dynamic startup dialog
// 
// 7     6/16/98 8:27p Psasse
// resizing startup dialog (in progress)
// 
// 6     6/05/98 12:10p Fredf
// Changes for new legal and copyright information.
// 
// 5     5/26/98 1:53p Dennis
// Program icon now originates from App specific resource file (if any)
// 
// 4     5/11/98 9:06p Psasse
// 
// 3     4/20/98 2:18p Fredf
// Migrated 4.0 About Box and Startup dialog changes.
// 
//    Rev 1.17   26 Feb 1998 09:41:32   Jay
// New about dialog and startup for NG
// 
//    Rev 1.16   02 Feb 1998 15:15:32   Jay
// New copyrights
// 
//    Rev 1.15   03 Sep 1997 17:55:10   Fred
// Uses IsNetworkVersion()
// 
//    Rev 1.14   29 Aug 1997 16:04:04   Fred
// Network version
// 
//    Rev 1.13   30 Jul 1997 12:43:00   johno
// Added RunAboutAddOn
// 
//    Rev 1.12   14 Jul 1997 11:17:06   Fred
// Removed some obsolete ifdefs
// 
//    Rev 1.11   01 Jul 1997 17:18:52   johno
// Added CBadPrinterDialog
// 
//    Rev 1.10   01 Jul 1997 10:10:58   johno
// Added exit dialog
// 
// 
//    Rev 1.9   30 Jun 1997 11:54:50   Jay
// Centers on desktop.
// 
//    Rev 1.8   18 Jun 1997 16:43:16   johno
// Addon stuff
// 
//    Rev 1.7   06 Jun 1997 17:14:44   Jay
// 16-bit fix.
// 
//    Rev 1.6   06 Jun 1997 16:47:08   Jay
// Startup dialog.
// 
//    Rev 1.5   03 Apr 1997 14:31:06   Jay
// Resource cleanup
// 
//    Rev 1.4   05 Nov 1996 17:31:14   Jay
// New network stuff
// 
//    Rev 1.3   04 Nov 1996 16:00:38   johno
// Strings come from CResourceString
// 
//    Rev 1.2   28 Oct 1996 14:07:00   Jay
// Moved changes from 3.01.
// 
//    Rev 1.2   24 Oct 1996 02:56:10   Fred
// Added Product Number to About Box
// 
//    Rev 1.1   11 Apr 1996 13:24:42   FRED
// Copied from PMW2
// 
//    Rev 2.7   21 Mar 1996 09:38:48   FRED
// Removed reliance on build.id, uses MSREG.INI instead
// 
//    Rev 2.6   12 Feb 1996 09:27:46   FRED
// Version is 2.10.xx
// 
//    Rev 2.5   24 Jan 1996 10:03:16   JAY
// New string management in Configuration
// 
//    Rev 2.4   17 Nov 1995 14:41:54   JAY
//  
// 
//    Rev 2.3   17 Nov 1995 13:14:54   JAY
// Now gets license/registration info from register DLL.
// 
//    Rev 2.2   08 Aug 1995 19:02:54   FRED
//  
// 
//    Rev 2.1   08 Feb 1995 13:34:08   JAY
// Reverted. New series.
// 
//    Rev 1.5   01 Dec 1994 09:52:10   JAY
//  
// 
//    Rev 1.4   30 Nov 1994 17:02:26   JAY
//  
// 
//    Rev 1.3   07 Oct 1994 09:33:18   JAY
// Removes the "striving" credit if not the cd version.
// 
//    Rev 1.2   16 Sep 1994 14:12:48   JAY
// New reduced dialog for Micro Center version.
// 
//    Rev 1.1   31 May 1994 12:44:00   JAY
// format_string() now takes an LPCSTR (not LPSTR).
// 
//    Rev 1.0   06 May 1994 17:12:18   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwini.h"
#include "pmwdoc.h"
#include "aboutdlg.h"
#include "register.h"
#include "pmwcfg.h"
#include "util.h"
#include "rmagcom.h"

#include "hubmgr.h"  // For RunAboutAddOn

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CPalette* pOurPal;  // PrintMaster Palette

CAboutDlg::CAboutDlg() : CPmwDialog(GetIDD())
{
   //{{AFX_DATA_INIT(CAboutDlg)
   //}}AFX_DATA_INIT
}

UINT CAboutDlg::GetIDD(void)
{
	if (CPrintMasterConfiguration::IsAddOn())
	{
		return IDD_ABOUT_ADDON;
	}
	else
	{
		return IDD_ABOUTBOX;
	}
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CPmwDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_ABOUT_ICON, m_cImage);
	//}}AFX_DATA_MAP
}

void CAboutDlg::format_string(UINT id, LPCSTR string)
{
   char format[80];
   char text[80];

   if (GetDlgItemText(id, format, sizeof(format)) != 0)
   {
      wsprintf(text, format, string);
      SetDlgItemText(id, text);
   }
}

BOOL CAboutDlg::OnInitDialog()
{
   CPmwDialog::OnInitDialog();

   CPmwApp *pApp = GET_PMWAPP();

/*
// Build the version number.
*/

   format_string(IDC_ABOUT_VERSION, pApp->GetVersion());

	if (!CPrintMasterConfiguration::IsAddOn())
	{
	/*
	// Build the parent part number.
	*/

		CString csPPN = pApp->GetParentPartNumber();
		format_string(IDC_ABOUT_PPN, csPPN);

	/*
	// Add anything else.
	*/

	/* Fill in the text for the license counts, etc. */

	//   format_string(IDC_ABOUT_SERIAL_NUMBER,
	//            pApp->m_installation_info.szSerialNumber);

		CString csFirstName;
		CString csLastName;
		CString csCompany;

		if (pApp->user_is_registered())
		{
			TRY
			{
				RegisterDLL.GetRegistrationData("FirstName", csFirstName);
				RegisterDLL.GetRegistrationData("LastName", csLastName);
				if (!csLastName.IsEmpty())
				{
					if (!csFirstName.IsEmpty())
					{
						csFirstName += ' ';
					}
					csFirstName += csLastName;
				}
				RegisterDLL.GetRegistrationData("Company", csCompany);
			}
			END_TRY

			// Name is required for valid registration.
			if (!csFirstName.IsEmpty())
			{
				SetDlgItemText(IDC_ABOUT_LICENSED_USER, csFirstName);
				SetDlgItemText(IDC_ABOUT_LICENSED_COMPANY, csCompany);
			}
			else
			{
				// JRO SetDlgItemText(IDC_REGISTERED_TO, "Registered");
				SetDlgItemText(IDC_ABOUT_LICENSED_USER, "");
				SetDlgItemText(IDC_ABOUT_LICENSED_COMPANY, "");
				SetDlgItemText (IDC_REGISTERED_TO, pApp->GetResourceStringPointer (IDS_REGISTERED));
			}
		// Get rid of this. We could hide it instead, but this seems easier.
		}
		else
		{
			// JRO SetDlgItemText(IDC_REGISTERED_TO, "Not registered");
			SetDlgItemText(IDC_ABOUT_LICENSED_USER, "");
			SetDlgItemText(IDC_ABOUT_LICENSED_COMPANY, "");
			SetDlgItemText (IDC_REGISTERED_TO, pApp->GetResourceStringPointer (IDS_REGISTERED_NOT));
		}

		format_string(IDC_ABOUT_CURRENT_USER, (LPCSTR)pApp->m_csUserName);

		if (GetConfiguration()->IsNetworkVersion())
		{
			CString string;
			LoadConfigurationString(IDS_NETWORK_LICENSE, string);

			SetDlgItemText(IDC_ABOUT_LICENSE, string);
		}
		else if (pApp->m_installation_info.bNetCount == 0)
		{
			CString string;
			LoadConfigurationString(IDS_SINGLE_USER_LICENSE, string);

			SetDlgItemText(IDC_ABOUT_LICENSE, string);
		}
		else
		{
			char buffer[40];

			CString string;
			LoadConfigurationString(IDS_MULTI_USER_LICENSE, string);

			sprintf(buffer, string,
					pApp->m_nUserCount,
					pApp->m_installation_info.bNetCount);
			SetDlgItemText(IDC_ABOUT_LICENSE, buffer);
		}
	}

	// No more copyrights in the about box...
	// They are now in the "Legals" dialog.
#if 0
	// Handle the copyright message.
	CString csCopyright;
	pApp->GetCopyrightString(csCopyright);
	SetDlgItemText(IDC_COPYRIGHT, csCopyright);
#endif

   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
   {
		CBitmap bmp;
      CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());

      rlProductSpecific.LoadBitmap(bmp, CSharedIDResourceManager::riCharacterSmall, pOurPal);
		if(bmp.m_hObject != NULL)
		{
			HBITMAP hBmp = m_cImage.SetBitmap(bmp);
			if(hBmp)
				DeleteObject(hBmp);
			bmp.Detach();
		}
   }


   return TRUE;  // return TRUE  unless you set the focus to a control
}


BEGIN_MESSAGE_MAP(CAboutDlg, CPmwDialog)
   //{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CTLCOLOR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStartupDialog dialog

CStartupDialog::CStartupDialog(CWnd* pParent /*=NULL*/)
   : CDialog(GetIDD(), pParent)
{
   //{{AFX_DATA_INIT(CStartupDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   pParent->EnableWindow(FALSE);
   Create(GetIDD(), pParent);
}

CStartupDialog::~CStartupDialog()
{
   if (m_hWnd != NULL)
   {
      DestroyWindow();
   }
}

UINT CStartupDialog::GetIDD(void)
{
	if (CPrintMasterConfiguration::IsAddOn())
	{
		return IDD_STARTUP;
		//return IDD_STARTUP_ADDON;
	}
	else
	{
		return IDD_STARTUP;
		//return GET_PMWAPP()->HasNGImages() ? IDD_STARTUP_WITH_NG : IDD_STARTUP;
	}
}

void CStartupDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CStartupDialog)
	DDX_Control(pDX, IDC_ABOUT_ICON, m_cImage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStartupDialog, CDialog)
   //{{AFX_MSG_MAP(CStartupDialog)
	ON_WM_CTLCOLOR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStartupDialog message handlers

BOOL CStartupDialog::OnInitDialog() 
{
   CPmwApp *pApp = GET_PMWAPP();
#ifdef WIN32
   CenterWindow();
#else
   // CenterWindow() has an activation bug in 16-bit.
   // So use ours.
   Util::CenterWindowInWindow(this, CWnd::GetDesktopWindow());
#endif
   CDialog::OnInitDialog();

	// Handle the copyright message.
	CString csCopyright;
	GET_PMWAPP()->GetCopyrightString(csCopyright);
	SetDlgItemText(IDC_COPYRIGHT, csCopyright);

	ResizeDlg();

#ifdef WIN32
   CenterWindow();
#else
   // CenterWindow() has an activation bug in 16-bit.
   // So use ours.
   Util::CenterWindowInWindow(this, CWnd::GetDesktopWindow());
#endif

   GetConfiguration()->ReplaceDialogText(this);

   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
   {
		CBitmap bmp;
      CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());
		ASSERT(pOurPal);
      rlProductSpecific.LoadBitmap(bmp, CSharedIDResourceManager::riCharacterSmall, pOurPal);
		if(bmp.m_hObject != NULL)
		{
			HBITMAP hBmp = m_cImage.SetBitmap(bmp);
			if(hBmp)
				DeleteObject(hBmp);
			bmp.Detach();
		}
   }

   return TRUE;  // return TRUE unless you set the focus to a control
}

void CStartupDialog::ResizeDlg()
{
	CStatic* pstaCopyright = (CStatic*)GetDlgItem(IDC_COPYRIGHT);
	RECT rCopy;
	pstaCopyright->GetWindowRect(&rCopy);
	ScreenToClient(&rCopy);

	CDC* pDC = pstaCopyright->GetDC();

	CString strText;
	GetDlgItemText(IDC_COPYRIGHT, strText);

	//keep a copy of the original size for resizing purposes
	RECT rOldCopy(rCopy);

	//select the font into the DC to Calculate text box size
	CFont* pFont = GetFont();
	CFont* pOldFont = pDC->SelectObject(pFont);

	//Calculate text box size without drawing
	int height = pDC->DrawText(strText, &rCopy, DT_WORDBREAK | DT_CALCRECT | DT_LEFT | DT_EXTERNALLEADING);

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	
	//pad text box with one extra row
	rCopy.bottom += tm.tmHeight;

	//resize text box based on calculated size
	pstaCopyright->MoveWindow(&rCopy, TRUE);

	//wrap the 3-D border around the text box
	CStatic* pstaBorder = (CStatic*)GetDlgItem(IDC_BORDER);
	CRect rBorder;
	pstaBorder->GetWindowRect(&rBorder);
	ScreenToClient(&rBorder);
	pstaBorder->MoveWindow(rBorder.left, rBorder.top, rBorder.Width(), rBorder.Height() - (rOldCopy.bottom - rCopy.bottom)); 

	//wrap dialog box around the 3-D border
	CRect rDlg;
	GetWindowRect(&rDlg);
	MoveWindow(rDlg.left, rDlg.top, rDlg.Width(), rDlg.Height() - (rOldCopy.bottom - rCopy.bottom)); 

	//clean-up
	pDC->SelectObject(pOldFont);
	pstaCopyright->ReleaseDC(pDC);
}

BOOL CStartupDialog::DestroyWindow() 
{
   GetParent()->EnableWindow(TRUE);
   return CDialog::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CExitDialog dialog

CExitDialog::CExitDialog(CWnd* pParent /*=NULL*/)
   : CDialog(CExitDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CExitDialog)
	//}}AFX_DATA_INIT
}

CExitDialog::~CExitDialog()
{
   if (m_hWnd != NULL)
   {
      DestroyWindow();
   }
}

void CExitDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CExitDialog)
	DDX_Control(pDX, IDC_ABOUT_ICON, m_cImage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExitDialog, CDialog)
   //{{AFX_MSG_MAP(CExitDialog)
	ON_WM_CTLCOLOR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExitDialog message handlers

BOOL CExitDialog::OnInitDialog() 
{
   CPmwApp *pApp = GET_PMWAPP();
#ifdef WIN32
   CenterWindow();
#else
   // CenterWindow() has an activation bug in 16-bit.
   // So use ours.
   Util::CenterWindowInWindow(this, CWnd::GetDesktopWindow());
#endif
   CDialog::OnInitDialog();

   GetConfiguration()->ReplaceDialogText(this);

   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
   {
		CBitmap bmp;
      CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());

      rlProductSpecific.LoadBitmap(bmp, CSharedIDResourceManager::riCharacterSmall, pOurPal);
		if(bmp.m_hObject != NULL)
		{
			HBITMAP hBmp = m_cImage.SetBitmap(bmp);
			if(hBmp)
				DeleteObject(hBmp);
			bmp.Detach();
		}
   }

   return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CExitDialog::DestroyWindow() 
{
   GetParent()->EnableWindow(TRUE);
   return CDialog::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CExitDialog dialog

CBadPrinterDialog::CBadPrinterDialog(CWnd* pParent /*=NULL*/)
   : CDialog(CBadPrinterDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CBadPrinterDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

CBadPrinterDialog::~CBadPrinterDialog()
{
   if (m_hWnd != NULL)
   {
      DestroyWindow();
   }
}

void CBadPrinterDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CBadPrinterDialog)
	DDX_Control(pDX, IDC_ABOUT_ICON, m_cImage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBadPrinterDialog, CDialog)
   //{{AFX_MSG_MAP(CBadPrinterDialog)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBadPrinterDialog message handlers

BOOL CBadPrinterDialog::OnInitDialog() 
{
   CPmwApp *pApp = GET_PMWAPP();
#ifdef WIN32
   CenterWindow();
#else
   // CenterWindow() has an activation bug in 16-bit.
   // So use ours.
   Util::CenterWindowInWindow(this, CWnd::GetDesktopWindow());
#endif
   CDialog::OnInitDialog();

   GetConfiguration()->ReplaceDialogText(this);

   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
   {
		CBitmap bmp;
      CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());

      rlProductSpecific.LoadBitmap(bmp, CSharedIDResourceManager::riCharacterSmall, pOurPal);
		if(bmp.m_hObject != NULL)
		{
			HBITMAP hBmp = m_cImage.SetBitmap(bmp);
			if(hBmp)
				DeleteObject(hBmp);
			bmp.Detach();
		}
   }

   return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CBadPrinterDialog::DestroyWindow() 
{
   GetParent()->EnableWindow(TRUE);
   return CDialog::DestroyWindow();
}

void RunAboutAddOn (void)
{
	CHubResourceManager  HubResourceManager (GetConfiguration()->GetHubDLLName ());   

   HINSTANCE NewResourceHandle, OldResourceHandle = AfxGetResourceHandle ();
   
   if ((NewResourceHandle = HubResourceManager.GetDLLHandle ()) != NULL)
   {
      AfxSetResourceHandle (NewResourceHandle);
   
      CAboutDlg Dialog;
      Dialog.DoModal();

      AfxSetResourceHandle (OldResourceHandle);
   }   
}

