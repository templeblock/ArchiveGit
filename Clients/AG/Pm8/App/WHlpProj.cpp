//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHlpProj.cpp 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/WHlpProj.cpp $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:09p Hforman
// Changed name form CWProj.cpp
// 
// 8     6/03/98 9:39p Hforman
// helpful hint
// 
// 7     6/01/98 8:25p Hforman
// new UpdatePreview() functionality
// 
// 6     5/31/98 3:31p Rlovejoy
// Included code to help strip out unsupporetd project types.
// 
// 5     5/29/98 6:11p Hforman
// calling proper parent OnInitDialog()
// 
// 4     5/27/98 5:51p Hforman
// 
// 3     5/27/98 5:41p Hforman
// remove SEND_WELCOME call
// 
// 2     5/26/98 3:02p Rlovejoy
// Massive revisions.
// 
//    Rev 1.0   14 Aug 1997 15:19:52   Fred
// Initial revision.
//   
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hint.h"
#include "WHlpProj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWProjectType dialog

CWProjectType::CWProjectType()
   : CWHelpCommonPropertyPage(NULL, 0)
{
   //{{AFX_DATA_INIT(CWProjectType)
	//}}AFX_DATA_INIT
}


void CWProjectType::DoDataExchange(CDataExchange* pDX)
{
   CWHelpCommonPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CWProjectType)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWProjectType, CWHelpCommonPropertyPage)
   //{{AFX_MSG_MAP(CWProjectType)
	ON_LBN_SELCHANGE(IDC_CHOICE_LIST, OnSelchangeChoiceList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWProjectType message handlers

BOOL CWProjectType::OnInitDialog() 
{
   CWHelpCommonPropertyPage::OnInitDialog();

	//  Show Helpful Hint
	GET_PMWAPP()->ShowHelpfulHint(PLACE_HINT, PLACE_WithHelp, this);

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CWProjectType::OnSetActive() 
{
	// Remove active pages (if necessary)
	CWHelpWizPropertySheet* pSheet = GetSheet();
	if (pSheet->GetActiveContainer() != -1)
	{
		pSheet->RemoveActivePages();
	}

	// Weed out porject types not supported by this version
	EliminateProjectTypes();

	return CWHelpCommonPropertyPage::OnSetActive();
}

LRESULT CWProjectType::OnWizardNext() 
{
	// Add the active pages
	GetSheet()->SetActiveContainer(m_listChoices.GetCurSel());
	GetSheet()->AddActivePages();
	
	return CWHelpCommonPropertyPage::OnWizardNext();
}

void CWProjectType::OnSelchangeChoiceList() 
{
	int nChoice = m_listChoices.GetCurSel();

	// Set the active page container
	GetSheet()->SetActiveContainer(nChoice);
	
	// Update preview window
	SetCurSelection(nChoice);
	UpdatePreview();
}

