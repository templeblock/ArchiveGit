/*
// $Workfile: WebPubQ.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WebPubQ.cpp $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     7/17/98 5:45p Johno
// Created
// 
//
*/

#include "stdafx.h"
#include "pmw.h"
#include "WebPubQ.h"
#include "pmwcfg.h"
#include "webdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebPublishQuery dialog


CWebPublishQuery::CWebPublishQuery(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CWebPublishQuery::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWebPublishQuery)
		// NOTE: the ClassWizard will add member initialization here
	m_fPublishWarning = !WebPublishWarnGet();
	//}}AFX_DATA_INIT
}


void CWebPublishQuery::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebPublishQuery)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Check(pDX, IDC_DONT_ASK, m_fPublishWarning);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebPublishQuery, CPmwDialog)
	//{{AFX_MSG_MAP(CWebPublishQuery)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebPublishQuery message handlers
