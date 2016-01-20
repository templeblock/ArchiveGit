//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHLtrhd.cpp 1     3/03/99 6:14p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/WHLtrhd.cpp $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:04p Hforman
// Changed name from CWLetter.cpp
// 
// 9     6/04/98 10:13p Rlovejoy
// Code to specialize OnSetActive().
// 
// 8     6/04/98 9:58p Hforman
// fix replacement strings
// 
// 7     6/01/98 11:18p Hforman
// fixing IDs, etc.
// 
// 6     5/29/98 6:24p Hforman
// finishing basic functionality
// 
// 5     5/28/98 8:39p Hforman
// work in progress
// 
// 4     5/27/98 7:03p Hforman
// 
// 3     5/26/98 5:50p Hforman
// work in progress
// 
// 2     5/26/98 3:02p Rlovejoy
// Massive revisions.
// 
//    Rev 1.0   14 Aug 1997 15:19:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:56   Fred
// Initial revision.
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "WHlpWiz.h"
#include "WHlpCmn.h"
#include "WHLtrhd.h"

/////////////////////////////////////////////////////////////////////////////
// CWLetterheadPageContainer

CWLetterheadPageContainer::CWLetterheadPageContainer(CWHelpWizPropertySheet* pSheet)
	: CWHelpPageContainer(pSheet)
{
	// Build property pages for each step in the wizard process
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_LETTER_CATEGORY));
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_LETTER_DESIGN));
	m_aPages.Add(new CWLetterheadInfoPage(this, ID_LETTER_INFO)); // IDD_PERSON1
	m_aPages.Add(new CWLetterheadView(this, ID_LETTER_HERE_IT_IS));
}
	
CWLetterheadPageContainer::~CWLetterheadPageContainer()
{

}

void CWLetterheadPageContainer::StoreUserInfo(UINT nIDContainerItem, int nChoice)
{
	switch (nIDContainerItem)
	{
		case ID_LETTER_CATEGORY:
			m_nCategory = nChoice;
			break;
		default:
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWLetterheadInfoPage dialog

CWLetterheadInfoPage::CWLetterheadInfoPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
	: CWHelpPropertyPage(pContainer, nIDContainerItem, IDD)
{
	//{{AFX_DATA_INIT(CWLetterheadInfoPage)
	m_strNameCaption = _T("");
	m_strAddressCaption = _T("");
	m_strName = _T("");
	m_strAddress = _T("");
	//}}AFX_DATA_INIT
}


void CWLetterheadInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWLetterheadInfoPage)
	DDX_Text(pDX, IDC_WH_PROMPT1, m_strNameCaption);
	DDX_Text(pDX, IDC_WH_PROMPT2, m_strAddressCaption);
	DDX_Text(pDX, IDC_WH_EDIT1, m_strName);
	DDX_Text(pDX, IDC_WH_EDIT2, m_strAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWLetterheadInfoPage, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CWLetterheadInfoPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWLetterheadInfoPage message handlers

BOOL CWLetterheadInfoPage::OnSetActive() 
{
	m_strTopText.LoadString(IDS_W_PLTEXT1);

	// Display correct text for the style that was chosen
	int nCategory = ((CWLetterheadPageContainer*)m_pContainer)->GetCategory();
	if (nCategory == CWLetterheadPageContainer::CATEGORY_BUSINESS)
	{
		m_strNameCaption.LoadString(IDS_W_PCNAME);
		m_strAddressCaption.LoadString(IDS_W_PCADD);
	}
	else
	{
		m_strNameCaption.LoadString(IDS_W_PNAME);
		m_strAddressCaption.LoadString(IDS_W_PADD);
	}

	UpdateData(FALSE);

	UpdatePreview(FALSE);
	return CPropertyPage::OnSetActive();
}

LRESULT CWLetterheadInfoPage::OnWizardNext() 
{
	// Tally up changes
	UpdateData();

	m_pContainer->SetUserText1Valid(TRUE);
	m_pContainer->SetUserText1(m_strName);
	m_pContainer->SetUserText2Valid(TRUE);
	m_pContainer->SetUserText2(m_strAddress);

	return CWHelpPropertyPage::OnWizardNext();
}

/////////////////////////////////////////////////////////////////////////////
// CWLetterheadView dialog

CWLetterheadView::CWLetterheadView(CWHelpPageContainer* pContainer, UINT nIDContainerItem) :
	CWView(pContainer, nIDContainerItem)
{
}

BEGIN_MESSAGE_MAP(CWLetterheadView, CWView)
	//{{AFX_MSG_MAP(CWLetterheadView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWLetterheadView message handlers

BOOL CWLetterheadView::OnSetActive() 
{
	// Open the document
	if (!CWView::OnSetActive())
		return FALSE;

	// Show final text
	m_strTopText.LoadString(IDS_WH_LETTERHD_FINAL);
	UpdateData(FALSE);

	return TRUE;
}

