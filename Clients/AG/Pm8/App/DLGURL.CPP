/*
// $Workfile: DLGURL.CPP $
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/DLGURL.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     6/10/98 4:09p Dennis
// Changes for AG
// 
//    Rev 1.0   14 Aug 1997 15:20:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:08   Fred
// Initial revision.
// 
//    Rev 1.0   07 May 1997 14:47:18   johno
// Initial revision.
// 
*/

// DlgURL.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwcfg.h"
#include "DlgURL.h"
#include "colordef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogURL dialog


CDialogURL::CDialogURL(CWnd* pParent /*=NULL*/)
	: CPmwDialogColor(CDialogURL::IDD, pParent, INTERNET_COLOR)
{
	//{{AFX_DATA_INIT(CDialogURL)
	m_csSiteDescription = _T("");
	//}}AFX_DATA_INIT
}


void CDialogURL::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogURL)
	DDX_Control(pDX, IDC_URL, m_URL);
	DDX_Text(pDX, IDC_PMWURL_DESC, m_csSiteDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogURL, CPmwDialogColor)
	//{{AFX_MSG_MAP(CDialogURL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogURL message handlers

BOOL CDialogURL::OnInitDialog() 
{
	CPmwDialogColor::OnInitDialog();

	
   GetConfiguration()->ReplaceDialogText(this);

	CString csUrl;
   csUrl = GetConfiguration()->GetProductURL();
	m_URL.SetWindowText(csUrl);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
