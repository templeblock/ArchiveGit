/*
// $Workfile: CARDSRVR.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CARDSRVR.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 4     2/08/99 8:48p Psasse
// New improved scheduling of Online Greetings
// 
// 3     8/20/98 11:03a Jayn
// No transparency in GIF creation. Adds PPN to INI file sent. Overridable
// path in configuration.
// 
// 2     6/05/98 4:41p Jayn
// Now passes the URL to the Card FTP DLL.
// 
//    Rev 1.0   14 Aug 1997 15:18:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:24   Fred
// Initial revision.
// 
//    Rev 1.9   15 Jul 1997 16:21:52   Jay
// Check for WinSock DLL. SetErrorMode to prevent open error message box.
// 
//    Rev 1.8   11 Jul 1997 11:45:30   johno
// Use DEBUG_ERRORBOX macro on DLL load fail
// 
// 
//    Rev 1.7   10 Jul 1997 14:40:16   Jay
// 16-bit fix.
// 
//    Rev 1.6   10 Jul 1997 12:31:24   Jay
// Card server, etc.
// 
//    Rev 1.5   03 Jul 1997 10:47:44   Jay
// New unmangled name for 16-bit.
// 
//    Rev 1.4   01 Jul 1997 15:21:06   Jay
// New 16-bit DLL
// 
//    Rev 1.3   23 May 1997 16:14:58   Jay
// Disclaimer dialog
// 
//    Rev 1.2   14 May 1997 10:16:42   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.1   05 May 1997 08:53:42   Jay
// Added DLL. Better error handling.
// 
//    Rev 1.0   18 Apr 1997 17:09:46   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwcfg.h"
#include <afxdtctl.h>
#include "CardSrvr.h"
#include "util.h"
#include "cardftp\cardftp.h"     // For notification definitions.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardServerDLL

// Constructor
CCardServerDLL::CCardServerDLL()
{
   m_hDLL = NULL;
   m_pSendFiles = NULL;
}

// Destructor
CCardServerDLL::~CCardServerDLL()
{
   Shutdown();
}

/*
// Startup the DLL.
*/

BOOL CCardServerDLL::Startup(LPCSTR pszDLLName)
{
	UINT wOldSem = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
   m_hDLL = LoadLibrary(pszDLLName);
	::SetErrorMode(wOldSem);
#ifdef  WIN32
   if (m_hDLL == NULL)
#else
   if (m_hDLL < HINSTANCE_ERROR)
#endif
   {
      // DLL load failed.
      m_hDLL = NULL;
      DEBUG_ERRORBOX("DLL Load Failed!");      
   }
   else
   {
      // Load the entry point.
#ifdef WIN32
      if ((m_pSendFiles = (CARDFTP_SENDFILES)::GetProcAddress(m_hDLL, "send_files")) == NULL)
#else
      if ((m_pSendFiles = (CARDFTP_SENDFILES)::GetProcAddress(m_hDLL, "SEND_FILES")) == NULL)
#endif
      {
         Shutdown();
      }
   }
   return (m_hDLL != NULL);
}

/*
// Shutdown the DLL.
*/

void CCardServerDLL::Shutdown(void)
{
   if (m_hDLL != NULL)
   {
      FreeLibrary(m_hDLL);
      m_hDLL = NULL;
   }
}

#ifdef WIN32
static void WINAPI CardFTPNotify(int nNotify, LPVOID pNotifyData)
#else
void WINAPI __export CardFTPNotify(int nNotify, LPVOID pNotifyData)
#endif
{
   ASSERT(pNotifyData != NULL);
   if (pNotifyData != NULL)
   {
      CCardServerProgressDialog* pDialog = (CCardServerProgressDialog*)pNotifyData;
      UINT uString;
      double dPosition = 0;
      switch (nNotify)
      {
         case CARDFTP_Initializing:
         {
            pDialog->SetStatus(IDS_Initializing);
            break;
         }
         case CARDFTP_Connecting:
         {
            pDialog->SetStatus(IDS_Connecting);
            uString = IDS_Connecting;
            break;
         }
         case CARDFTP_SendingTextFile:
         {
            pDialog->SetStatus(IDS_SendingTextFile, 0.0/3.0);
            break;
         }
         case CARDFTP_SendingFront:
         {
            pDialog->SetStatus(IDS_SendingFront, 1.0/3.0);
            break;
         }
         case CARDFTP_SendingInside:
         {
            pDialog->SetStatus(IDS_SendingInside, 2.0/3.0);
            break;
         }
         case CARDFTP_Closing:
         {
            pDialog->SetStatus(IDS_Closing, 1.0);
            break;
         }
         case CARDFTP_Aborting:
         {
            break;
         }
         default:
         {
            // What could this be?
            ASSERT(FALSE);
            break;
         }
      }
   }
}

int CCardServerDLL::SendFiles(LPCSTR pTheDir, CCardServerProgressDialog& ProgressDialog)
{
   int nResult = -1;
   ASSERT(m_pSendFiles != NULL);
	CString csURL = GET_PMWAPP()->OurGetProfileString("Online",
																	  "GreetingsURL",
																	  GetConfiguration()->GetOnlineGreetingsFTPSite());

   if (m_pSendFiles != NULL)
   {
      nResult = m_pSendFiles(pTheDir, csURL, &CardFTPNotify, &ProgressDialog);
   }
   return nResult;
}

/////////////////////////////////////////////////////////////////////////////
// CCardServerDialog dialog

CString CCardServerDialog::m_csLastFrom;
CString CCardServerDialog::m_csLastTo;
CString CCardServerDialog::m_csLastTitle;

CCardServerDialog::CCardServerDialog(CWnd* pParent /*=NULL*/)
   : CPmwDialog(CCardServerDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CCardServerDialog)
   m_csFrom = _T("");
   m_csTo = _T("");
	m_csTitle = _T("");
	m_ctTime = 0;
	//}}AFX_DATA_INIT
   m_csFrom = m_csLastFrom;
   m_csTo = m_csLastTo;
   m_csTitle = m_csLastTitle;
}

void CCardServerDialog::DoDataExchange(CDataExchange* pDX)
{
   CPmwDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCardServerDialog)
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_ctrlMonthCal);
   DDX_Text(pDX, IDC_FROM, m_csFrom);
   DDV_MaxChars(pDX, m_csFrom, 64);
   DDX_Text(pDX, IDC_TO, m_csTo);
   DDV_MaxChars(pDX, m_csTo, 64);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_csTitle);
	DDX_MonthCalCtrl(pDX, IDC_MONTHCALENDAR1, m_ctTime);
	//}}AFX_DATA_MAP

   if (pDX->m_bSaveAndValidate)
   {
      m_csLastFrom = m_csFrom;
      m_csLastTo = m_csTo;
      m_csLastTitle = m_csTitle;
   }
}

BEGIN_MESSAGE_MAP(CCardServerDialog, CPmwDialog)
   //{{AFX_MSG_MAP(CCardServerDialog)
   ON_EN_CHANGE(IDC_FROM, OnChangeFrom)
   ON_EN_CHANGE(IDC_TO, OnChangeTo)
	ON_NOTIFY(MCN_SELCHANGE, IDC_MONTHCALENDAR1, OnSelchangeMonthcalendar1)
	ON_EN_CHANGE(IDC_EDIT_TITLE, OnChangeTitle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardServerDialog message handlers

BOOL CCardServerDialog::OnInitDialog() 
{
   CPmwDialog::OnInitDialog();

   // Set the focus to the From button.
   CEdit* pFrom = (CEdit*)GetDlgItem(IDC_FROM);
   if (pFrom != NULL)
   {
      pFrom->SetFocus();
      pFrom->SetSel(0, -1);
   }

   // Update the OK button.
   UpdateOK();

	m_ctrlMonthCal.GetCurSel(&m_stTime);

	//set the range of the calendar control
	CTime timeNow = CTime::GetCurrentTime();
	CTimeSpan timeMax(90, 0, 0, 0);  //90 days in future
	CTime timeThen = timeNow + timeMax;

	m_ctrlMonthCal.SetRange(&timeNow, &timeThen);

   return FALSE;
}

void CCardServerDialog::OnChangeFrom() 
{
   UpdateOK();
}

void CCardServerDialog::OnChangeTo() 
{
   UpdateOK();
}

void CCardServerDialog::OnChangeTitle() 
{
   UpdateOK();
}

/*
// Update the OK button.
// The criteria is that the "From" and "To" entries must be non-empty.
*/

void CCardServerDialog::UpdateOK(void)
{
   CString csFrom;
   CString csTo;
   CString csTitle;

   Util::GetDlgItemText(this, IDC_FROM, csFrom);
   Util::GetDlgItemText(this, IDC_TO, csTo);
   Util::GetDlgItemText(this, IDC_EDIT_TITLE, csTitle);

   Util::Trim(csFrom);
   Util::Trim(csTo);
   Util::Trim(csTitle);

   CWnd* pWnd = GetDlgItem(IDOK);
   if (pWnd != NULL)
   {
      pWnd->EnableWindow(!csFrom.IsEmpty() && !csTo.IsEmpty() && !csTitle.IsEmpty());
   }
}

void CCardServerDialog::OnSelchangeMonthcalendar1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_ctrlMonthCal.GetCurSel(&m_stTime); //m_ctTime);
	
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CCardServerDisclaimer dialog

CCardServerDisclaimer::CCardServerDisclaimer(CWnd* pParent /*=NULL*/)
   : CPmwDialog(CCardServerDisclaimer::IDD, pParent)
{
   //{{AFX_DATA_INIT(CCardServerDisclaimer)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void CCardServerDisclaimer::DoDataExchange(CDataExchange* pDX)
{
   CPmwDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCardServerDisclaimer)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCardServerDisclaimer, CPmwDialog)
   //{{AFX_MSG_MAP(CCardServerDisclaimer)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardServerDisclaimer message handlers

BOOL CCardServerDisclaimer::OnInitDialog() 
{
   CPmwDialog::OnInitDialog();

   // Set the text.
   CWnd* pText;
   if ((pText = GetDlgItem(IDC_LEGAL_TEXT)) != NULL)
   {
      HRSRC hrsrc;   
      HINSTANCE hInstance = AfxGetResourceHandle();

      if ((hrsrc = FindResource(hInstance, MAKEINTRESOURCE(IDR_CARD_SERVER_DISCLAIMER), "TEXT")) != NULL)
      {
         HGLOBAL  hGlobal;
         if ((hGlobal = LoadResource(hInstance, hrsrc)) != NULL)
         {
            LPCSTR lpRes;

            if ((lpRes = (LPCSTR)LockResource(hGlobal)) != NULL)
            {
               pText->SetWindowText(lpRes);

               UnlockResource(hGlobal);
            }

            FreeResource(hGlobal);
         }
      }
   }
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCardServerProgressDialog dialog

CCardServerProgressDialog::CCardServerProgressDialog(CWnd* pParent /*=NULL*/)
   : CDialog(CCardServerProgressDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CCardServerProgressDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   pParent->EnableWindow(FALSE);
   Create(CCardServerProgressDialog::IDD, pParent);
}

CCardServerProgressDialog::~CCardServerProgressDialog()
{
   if (m_hWnd != NULL)
   {
      DestroyWindow();
   }
}

void CCardServerProgressDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCardServerProgressDialog)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCardServerProgressDialog, CDialog)
   //{{AFX_MSG_MAP(CCardServerProgressDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardServerProgressDialog message handlers

BOOL CCardServerProgressDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   standard_dialog_startup(this);

   m_ProgressBar.Initialize(IDC_PROGRESS, this);

   // Set progress bar.
   m_ProgressBar.SetRange(0.0, 1.0);
   m_ProgressBar.SetPosition(0.0);

   return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CCardServerProgressDialog::DestroyWindow() 
{
   GetParent()->EnableWindow(TRUE);
   return CDialog::DestroyWindow();
}

void CCardServerProgressDialog::SetStatus(UINT uStatus, double dPosition /*=0.0*/)
{
   SetDlgItemText(IDC_STATUS, GET_PMWAPP()->GetResourceStringPointer(uStatus));
   m_ProgressBar.SetPosition(dPosition);
}

