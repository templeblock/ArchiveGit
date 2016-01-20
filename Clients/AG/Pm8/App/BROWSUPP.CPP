/*
// $Workfile: BROWSUPP.CPP $
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
// $Log: /PM8/App/BROWSUPP.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 4     4/30/98 10:23p Fredf
// Support for "Special Collection" button used for American Greetings.
// 
// 3     3/12/98 4:39p Hforman
// added CEditUserTitle
// 
// 2     1/22/98 7:15p Hforman
// add CMsgBoxYesAll dialog
// 
//    Rev 1.0   14 Aug 1997 15:18:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:16   Fred
// Initial revision.
// 
//    Rev 1.1   06 Jun 1997 11:51:16   Jay
// Licensing changes
// 
//    Rev 1.0   30 May 1997 19:16:10   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "Browsupp.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrowserPropertiesDialog dialog

CBrowserPropertiesDialog::CBrowserPropertiesDialog(UINT uID, CWnd* pParent /*=NULL*/)
	: CPmwDialog(uID, pParent)
{
	//{{AFX_DATA_INIT(CBrowserPropertiesDialog)
	m_csKeywords = _T("");
	m_csFriendlyName = _T("");
	m_csFileName = _T("");
	//}}AFX_DATA_INIT
}


void CBrowserPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrowserPropertiesDialog)
	DDX_Control(pDX, IDC_DESCRIPTION, m_ctrlFriendlyName);
	DDX_Text(pDX, IDC_KEYWORDS, m_csKeywords);
	DDX_Text(pDX, IDC_DESCRIPTION, m_csFriendlyName);
	DDX_Text(pDX, IDC_NAME, m_csFileName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBrowserPropertiesDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CBrowserPropertiesDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowserPropertiesDialog message handlers

BOOL CBrowserPropertiesDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	m_ctrlFriendlyName.LimitText(MAX_FRIENDLY_NAME_LEN);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserProgressDialog dialog

CBrowserProgressDialog::CBrowserProgressDialog(UINT uID, int nCount, CWnd* pParent /*=NULL*/, UINT uAbortMessage /*=0*/)
	: CDialog(uID, pParent)
{
	//{{AFX_DATA_INIT(CBrowserProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nCount = nCount;
	m_fAbort = FALSE;
	m_uAbortMessage = uAbortMessage;

	pParent->EnableWindow(FALSE);
	Create(uID, pParent);
}

CBrowserProgressDialog::~CBrowserProgressDialog()
{
	if (m_hWnd != NULL)
	{
		DestroyWindow();
	}
}

void CBrowserProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrowserProgressDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBrowserProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CBrowserProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CBrowserProgressDialog::SetPosition(int nPosition, LPCSTR pszFileName)
{
	// Set the progress indicator.
	m_ProgressBar.SetPosition((double)nPosition);

	// Format the filename text.
	CString csText;
	csText.Format(m_csFilenameFormat, pszFileName);
	SetDlgItemText(IDC_FILENAME, csText);
}

BOOL CBrowserProgressDialog::CheckForAbort(void)
{
	if (!m_fAbort)
	{
		MSG Msg;
		while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!AfxGetApp()->PumpMessage())
			{
				::PostQuitMessage(0);
				m_fAbort = TRUE;
				break;
			}
		}
	}

	return m_fAbort;
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserProgressDialog message handlers

BOOL CBrowserProgressDialog::OnInitDialog() 
{
	INHERITED::OnInitDialog();

	m_ProgressBar.Initialize(IDC_PROGRESS, this);

	// Set progress bar.
	m_ProgressBar.SetRange(0.0, (double)m_nCount);
	m_ProgressBar.SetPosition(0.0);

	Util::GetDlgItemText(this, IDC_FILENAME, m_csFilenameFormat);
	SetDlgItemText(IDC_FILENAME, "");

	CenterWindow();

	return TRUE;
}

void CBrowserProgressDialog::OnCancel() 
{
	if (!m_fAbort)
	{
		if (AfxMessageBox(m_uAbortMessage, MB_YESNO) == IDYES)
		{
			m_fAbort = TRUE;
		}
	}
}

BOOL CBrowserProgressDialog::DestroyWindow() 
{
	GetParent()->EnableWindow(TRUE);
	return INHERITED::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CUnlicensedDialog dialog


CUnlicensedDialog::CUnlicensedDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CUnlicensedDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUnlicensedDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUnlicensedDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnlicensedDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnlicensedDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CUnlicensedDialog)
	ON_BN_CLICKED(IDC_ORDER_FORM, OnOrderForm)
	ON_BN_CLICKED(IDC_VIEW_SET, OnViewSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnlicensedDialog message handlers

BOOL CUnlicensedDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CUnlicensedDialog::OnOrderForm() 
{
	EndDialog(IDC_ORDER_FORM);
}

void CUnlicensedDialog::OnViewSet() 
{
	EndDialog(IDC_VIEW_SET);
}


/////////////////////////////////////////////////////////////////////////////
// CMsgBoxYesAll dialog

CMsgBoxYesAll::CMsgBoxYesAll(LPCTSTR msg, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CMsgBoxYesAll::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMsgBoxYesAll)
	m_csMessage = msg;
	//}}AFX_DATA_INIT
}

void CMsgBoxYesAll::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgBoxYesAll)
	DDX_Control(pDX, IDC_WARNING_ICON, m_icon);
	DDX_Text(pDX, IDC_MESSAGE, m_csMessage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMsgBoxYesAll, CPmwDialog)
	//{{AFX_MSG_MAP(CMsgBoxYesAll)
	ON_BN_CLICKED(IDC_YESALL, OnYesall)
	ON_BN_CLICKED(IDNO, OnNo)
	ON_BN_CLICKED(IDYES, OnYes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgBoxYesAll message handlers

BOOL CMsgBoxYesAll::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	// show product name in titlebar
	CString name = GET_PMWAPP()->m_pszAppName;
	SetWindowText(name);

	// load Window's "Warning" icon
	HICON hIcon = ::LoadIcon(NULL, IDI_WARNING);
	m_icon.SetIcon(hIcon);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMsgBoxYesAll::OnYesall() 
{
	EndDialog(IDYESALL);
}
void CMsgBoxYesAll::OnNo() 
{
	EndDialog(IDNO);
}
void CMsgBoxYesAll::OnYes() 
{
	EndDialog(IDYES);
}

/////////////////////////////////////////////////////////////////////////////
// CEditUserTitle

CEditUserTitle::CEditUserTitle()
{
}

CEditUserTitle::~CEditUserTitle()
{
}


BEGIN_MESSAGE_MAP(CEditUserTitle, CEdit)
	//{{AFX_MSG_MAP(CEditUserTitle)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditUserTitle message handlers

void CEditUserTitle::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_RETURN)
	{
		SendMessage(WM_KILLFOCUS, NULL, NULL);
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}
