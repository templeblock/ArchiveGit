/*
// $Workfile: SIGNVIEW.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SIGNVIEW.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 6     8/17/98 5:36p Mwilson
// changed Tshirts to be based off of newsletters instead of posters
// 
// 5     6/29/98 2:37p Mwilson
// added gadgets to mulitpage projects
// 
// 4     4/21/98 3:39p Fredf
// Reworked "reverse" printing to apply to all projects.
// 
// 3     4/18/98 4:25p Fredf
// T-Shirt printing.
// 
// 2     4/17/98 4:47p Fredf
// T-Shirts
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "signview.h"
#include "util.h"
#include "nwsprint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPosterView

IMPLEMENT_DYNCREATE(CPosterView, CPmwView)

CPosterView::CPosterView()
{
}

CPosterView::~CPosterView()
{
}


BEGIN_MESSAGE_MAP(CPosterView, CPmwView)
	//{{AFX_MSG_MAP(CPosterView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPosterView drawing

void CPosterView::OnDraw(CDC* pDC)
{
	CPmwView::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CPosterView message handlers

/////////////////////////////////////////////////////////////////////////////
// CCertificateView

IMPLEMENT_DYNCREATE(CCertificateView, CPosterView)

CCertificateView::CCertificateView()
{
}

CCertificateView::~CCertificateView()
{
}


BEGIN_MESSAGE_MAP(CCertificateView, CPosterView)
	//{{AFX_MSG_MAP(CCertificateView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCertificateView message handlers

/////////////////////////////////////////////////////////////////////////////
// CFaxCoverView

IMPLEMENT_DYNCREATE(CFaxCoverView, CPosterView)

CFaxCoverView::CFaxCoverView()
{
}

CFaxCoverView::~CFaxCoverView()
{
}


BEGIN_MESSAGE_MAP(CFaxCoverView, CPosterView)
	//{{AFX_MSG_MAP(CFaxCoverView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaxCoverView message handlers

/////////////////////////////////////////////////////////////////////////////
// CStationeryView

IMPLEMENT_DYNCREATE(CStationeryView, CPosterView)

CStationeryView::CStationeryView()
{
}

CStationeryView::~CStationeryView()
{
}


BEGIN_MESSAGE_MAP(CStationeryView, CPosterView)
	//{{AFX_MSG_MAP(CStationeryView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationeryView message handlers

