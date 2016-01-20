//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHPoster.cpp 1     3/03/99 6:14p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/WHPoster.cpp $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:06p Hforman
// Changed name from CWPoster.cpp
// 
// 8     6/04/98 10:18p Rlovejoy
// Specialized code for OnSetActive().
// 
// 7     6/04/98 9:44p Hforman
// add CATEGORY_HOME, fix string replacement
// 
// 6     6/01/98 11:18p Hforman
// fixing IDs, etc.
// 
// 4     5/29/98 6:24p Hforman
// finishing basic functionality
// 
// 3     5/28/98 8:39p Hforman
// work in progress
// 
// 2     5/28/98 12:56p Hforman
// 
// 1     5/28/98 12:36p Hforman
// initial revision
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WHlpwiz.h"
#include "WHlpCmn.h"
#include "WHPoster.h"

/////////////////////////////////////////////////////////////////////////////
// CWPosterPageContainer

CWPosterPageContainer::CWPosterPageContainer(CWHelpWizPropertySheet* pSheet)
	: CWHelpPageContainer(pSheet)
{
	// Build property pages for each step in the wizard process
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_POSTER_CATEGORY));
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_POSTER_DESIGN));
	m_aPages.Add(new CWPosterInfoPage(this, ID_POSTER_INFO)); // IDD_PERSON1
	m_aPages.Add(new CWPosterView(this, ID_POSTER_HERE_IT_IS));
}
	
CWPosterPageContainer::~CWPosterPageContainer()
{

}

void CWPosterPageContainer::StoreUserInfo(UINT nIDContainerItem, int nChoice)
{
	switch (nIDContainerItem)
	{
		case ID_POSTER_CATEGORY:
			m_nCategory = nChoice;
			break;
		default:
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWPosterInfoPage dialog

CWPosterInfoPage::CWPosterInfoPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
	: CWHelpPropertyPage(pContainer, nIDContainerItem, IDD)
{
	//{{AFX_DATA_INIT(CWPosterInfoPage)
	m_strCaption1 = _T("");
	m_strCaption2 = _T("");
	m_strText1 = _T("");
	m_strText2 = _T("");
	//}}AFX_DATA_INIT
}


void CWPosterInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWPosterInfoPage)
	DDX_Control(pDX, IDC_WH_PROMPT1, m_ctrlCaption1);
	DDX_Control(pDX, IDC_WH_PROMPT2, m_ctrlCaption2);
	DDX_Text(pDX, IDC_WH_PROMPT1, m_strCaption1);
	DDX_Text(pDX, IDC_WH_PROMPT2, m_strCaption2);
	DDX_Control(pDX, IDC_WH_EDIT1, m_edit1);
	DDX_Control(pDX, IDC_WH_EDIT2, m_edit2);
	DDX_Text(pDX, IDC_WH_EDIT1, m_strText1);
	DDX_Text(pDX, IDC_WH_EDIT2, m_strText2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWPosterInfoPage, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CWPosterInfoPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWPosterInfoPage message handlers

BOOL CWPosterInfoPage::OnSetActive() 
{
	m_strTopText.LoadString(IDS_W_PPTEXT1);

	// Display correct text for the style that was chosen.
	// Also, tell container if strings are valid or not.
	int nCategory = ((CWPosterPageContainer*)m_pContainer)->GetCategory();
	switch(nCategory)
	{
	case CWPosterPageContainer::CATEGORY_EVENT:
	case CWPosterPageContainer::CATEGORY_HOME:
		m_ctrlCaption1.ShowWindow(TRUE);
		m_edit1.ShowWindow(TRUE);
		m_strCaption1.LoadString(IDS_WH_HEADLN_PROMPT);
		m_pContainer->SetUserText1Valid(TRUE);

		m_ctrlCaption2.ShowWindow(TRUE);
		m_edit2.ShowWindow(TRUE);
		m_strCaption2.LoadString(IDS_WH_BODY_PROMPT);
		m_pContainer->SetUserText2Valid(TRUE);
		break;

	case CWPosterPageContainer::CATEGORY_FORSALE:
		m_ctrlCaption1.ShowWindow(FALSE);
		m_edit1.ShowWindow(FALSE);
		m_pContainer->SetUserText1Valid(FALSE);

		m_ctrlCaption2.ShowWindow(TRUE);
		m_edit2.ShowWindow(TRUE);
		m_strCaption2.LoadString(IDS_W_PITEMS);
		m_pContainer->SetUserText2Valid(TRUE);
		break;

	case CWPosterPageContainer::CATEGORY_KIDSTUFF:
		m_ctrlCaption1.ShowWindow(TRUE);
		m_edit1.ShowWindow(TRUE);
		m_strCaption1.LoadString(IDS_W_PNAME);
		m_pContainer->SetUserText1Valid(TRUE);

		m_edit2.ShowWindow(FALSE);
		m_ctrlCaption2.ShowWindow(FALSE);
		m_pContainer->SetUserText2Valid(FALSE);
		break;

	default:
		ASSERT(0);
		break;
	}

	UpdateData(FALSE);

	UpdatePreview(FALSE);
	return CPropertyPage::OnSetActive();
}

LRESULT CWPosterInfoPage::OnWizardNext() 
{
	UpdateData();

	m_pContainer->SetUserText1(m_strText1);
	m_pContainer->SetUserText2(m_strText2);

	return CWHelpPropertyPage::OnWizardNext();
}

/////////////////////////////////////////////////////////////////////////////
// CWPosterView dialog

CWPosterView::CWPosterView(CWHelpPageContainer* pContainer, UINT nIDContainerItem) :
	CWView(pContainer, nIDContainerItem)
{

}

BEGIN_MESSAGE_MAP(CWPosterView, CWView)
	//{{AFX_MSG_MAP(CWPosterView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWPosterView message handlers

BOOL CWPosterView::OnSetActive() 
{
	// Open the document
	if (!CWView::OnSetActive())
		return FALSE;

	// Show final text
	m_strTopText.LoadString(IDS_WH_POSTER_FINAL);
	UpdateData(FALSE);

	return TRUE;
}

