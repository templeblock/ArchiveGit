//////////////////////////////////////////////////////////////////////////////
// $Heading: Newslett.cpp$
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WHlpView.h"
#include "WHlpWiz.h"
#include "WHlpCmn.h"
#include "WHWebPg.h"

/////////////////////////////////////////////////////////////////////////////
// CWebPagePageContainer

CWebPagePageContainer::CWebPagePageContainer(CWHelpWizPropertySheet* pSheet)
	: CWHelpPageContainer(pSheet)
{
	// Build property pages for each step in the wizard process
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_WEBPAGE_CATEGORY));
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_WEBPAGE_STYLE));
	m_aPages.Add(new CWHWebPageView(this, ID_WEBPAGE_HERE_IT_IS));
}
	
CWebPagePageContainer::~CWebPagePageContainer()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWHWebPageView dialog

CWHWebPageView::CWHWebPageView(CWHelpPageContainer* pContainer, UINT nIDContainerItem) :
	CWView(pContainer, nIDContainerItem)
{
	m_pContainer = (CWebPagePageContainer*)pContainer;
}

BEGIN_MESSAGE_MAP(CWHWebPageView, CWView)
	//{{AFX_MSG_MAP(CWHWebPageView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHWebPageView message handlers

BOOL CWHWebPageView::OnSetActive() 
{
	// Open the document
	if (!CWView::OnSetActive())
		return FALSE;

	// Show final text
	m_strTopText.LoadString(IDS_WH_WEBPAGE_FINAL);
	UpdateData(FALSE);

	return TRUE;
}
