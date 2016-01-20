// $Workfile: WHGreet.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
//
// Revision History:
//
// $Log: /PM8/App/WHGreet.cpp $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 1:49p Hforman
// changed name from CWHGreet.cpp
// 
// 11    6/04/98 11:02p Hforman
// 
// 10    6/04/98 10:34p Hforman
// validate text strings
// 
// 9     6/04/98 2:10p Rlovejoy
// Show the right title in personalize.
// 
// 8     6/02/98 4:46p Hforman
// fixed problem with view page not showing correct panel
// 
// 7     6/01/98 11:18p Hforman
// fixing IDs, etc.
// 
// 6     6/01/98 8:25p Hforman
// new UpdatePreview() functionality
// 
// 5     5/31/98 3:30p Rlovejoy
// Display the correct strings in view.
// 
// 4     5/31/98 12:35p Rlovejoy
// Update.
// 
// 3     5/28/98 6:35p Rlovejoy
// Incremental revision.
// 
// 2     5/26/98 3:01p Rlovejoy
// Massive revisions.
// 
//    Rev 1.0   14 Aug 1997 15:19:44   Fred
// Initial revision.
// 

#include "stdafx.h"
#include "pmwdoc.h"
#include "carddoc.h"

#include "WHlpCmn.h"
#include "WHlpView.h"
#include "WHGreet.h"

/////////////////////////////////////////////////////////////////////////////
// CWGreetingPageContainer

CWGreetingPageContainer::CWGreetingPageContainer(CWHelpWizPropertySheet* pSheet) :
	CWHelpPageContainer(pSheet)
{
	// Build property pages for each step in the wizard process
	// NOTE: need to allocate separate CWHelpCommonPropertyPage's because
	// property sheet won't allow you to add a pointer to the same property
	// page twice.
	m_aPages.Add((void*)new CWHelpCommonPropertyPage(this, IDe_GREETING_CATEGORY));
	m_aPages.Add((void*)new CWHelpCommonPropertyPage(this, IDe_GREETING_TYPE));
	m_aPages.Add((void*)new CWGreetingDesignPage(this, IDe_GREETING_ART));
	m_aPages.Add((void*)new CWPersonalize(this, IDe_GREETING_ENTER_NAMES));
	m_aPages.Add((void*)new CWGreetingView(this, IDe_GREETING_HERE_IT_IS));
}

CWGreetingPageContainer::~CWGreetingPageContainer()
{

}

void CWGreetingPageContainer::StoreUserInfo(UINT nIDContainerItem, int nChoice)
{
	switch (nIDContainerItem)
	{
		case IDe_GREETING_CATEGORY:
			m_nCategory = nChoice;
			break;
		case IDe_GREETING_TYPE:
			m_nType = nChoice;
			break;
		default:
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWGreetingDesignPage dialog

CWGreetingDesignPage::CWGreetingDesignPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
	: CWHelpCommonPropertyPage(pContainer, nIDContainerItem)
{
}

void CWGreetingDesignPage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpCommonPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CWHCalendarPicturePage)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWGreetingDesignPage, CWHelpCommonPropertyPage)
	//{{AFX_MSG_MAP(CWGreetingDesignPage)
	ON_BN_CLICKED(IDC_WH_VIEW_FRONT, OnViewFront)
	ON_BN_CLICKED(IDC_WH_VIEW_INSIDE, OnViewInside)
	ON_LBN_SELCHANGE(IDC_CHOICE_LIST, OnSelchangeChoiceList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWGreetingDesignPage message handlers

BOOL CWGreetingDesignPage::OnInitDialog() 
{
	CWHelpCommonPropertyPage::OnInitDialog();

	// Make room for the radio buttons under the list box
	CRect rctListBound;
	m_listChoices.GetClientRect(rctListBound);
	m_listChoices.SetWindowPos(
		NULL, 
		0, 0, 
		rctListBound.Width(), rctListBound.Height() * 3/4, 
		SWP_NOMOVE | SWP_NOZORDER);
	m_listChoices.GetWindowRect(rctListBound);
	ScreenToClient(rctListBound);
	CFont* pFont = m_listChoices.GetFont();

	// Create radio buttons
	CString cs;
	CRect rctPlacement;
	cs.LoadString(IDS_FrontOfCard);
	rctPlacement.SetRect(
		rctListBound.left, 
		rctListBound.bottom+20, 
		rctListBound.right, 
		rctListBound.bottom+35);
	m_btnFront.Create((const char*)cs, BS_RADIOBUTTON | WS_VISIBLE, rctPlacement, this, IDC_WH_VIEW_FRONT);
	m_btnFront.SetFont(pFont);
	m_btnFront.SetCheck(TRUE);
	cs.LoadString(IDS_InsideOfCard);
	rctPlacement.OffsetRect(0, 25);
	m_btnInside.Create((const char*)cs, BS_RADIOBUTTON | WS_VISIBLE, rctPlacement, this, IDC_WH_VIEW_INSIDE);
	m_btnInside.SetFont(pFont);

	// View front of card
	m_preview.ForcePreviewToFront(TRUE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWGreetingDesignPage::OnSetActive() 
{
	m_fRedrawPreview = FALSE;
	BOOL fSuccess = CWHelpCommonPropertyPage::OnSetActive();
	if (fSuccess)
	{
		if (m_btnInside.GetCheck())
		{
			SetCardPanel(CARD_PANEL_Inside);
		}
		else
		{
			SetCardPanel(CARD_PANEL_Front);
		}
	}

	return fSuccess;
}

void CWGreetingDesignPage::OnViewFront() 
{
	m_btnFront.SetCheck(!m_btnFront.GetCheck());
	m_btnInside.SetCheck(!m_btnFront.GetCheck());

	SetCardPanel(CARD_PANEL_Front);
}

void CWGreetingDesignPage::OnViewInside() 
{
	m_btnInside.SetCheck(!m_btnInside.GetCheck());
	m_btnFront.SetCheck(!m_btnInside.GetCheck());

	SetCardPanel(CARD_PANEL_Inside);
}

void CWGreetingDesignPage::OnSelchangeChoiceList() 
{
	// Send over list selection
	SetCurSelection(m_listChoices.GetCurSel());
	UpdatePreview();
	if (m_btnInside.GetCheck())
	{
		SetCardPanel(CARD_PANEL_Inside);
	}
}

void CWGreetingDesignPage::SetCardPanel(SHORT panel)
{
   // need to cast to CCardDoc
   CCardDoc *pDoc = (CCardDoc*)m_pDoc;
   if (pDoc->set_panel(panel)) {
		m_preview.ForcePreviewToFront((panel == CARD_PANEL_Front));
		m_preview.SizePreview();
      m_preview.Invalidate();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CWPersonalize dialog

CWPersonalize::CWPersonalize(CWHelpPageContainer* pContainer, UINT nIDContainerItem) : 
	CWHelpPropertyPage(pContainer, nIDContainerItem, CWPersonalize::IDD)
{
	//{{AFX_DATA_INIT(CWPersonalize)
	m_strCaption1 = _T("");
	m_strCaption2 = _T("");
	m_strText1 = _T("");
	m_strText2 = _T("");
	//}}AFX_DATA_INIT
}



void CWPersonalize::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWPersonalize)
	DDX_Text(pDX, IDC_WH_PROMPT1, m_strCaption1);
	DDX_Text(pDX, IDC_WH_PROMPT2, m_strCaption2);
	DDX_Control(pDX, IDC_WH_EDIT1, m_edit1);
	DDX_Control(pDX, IDC_WH_EDIT2, m_edit2);
	DDX_Text(pDX, IDC_WH_EDIT1, m_strText1);
	DDX_Text(pDX, IDC_WH_EDIT2, m_strText2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWPersonalize, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CWPersonalize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWPersonalize message handlers

BOOL CWPersonalize::OnInitDialog() 
{
	CWHelpPropertyPage::OnInitDialog();

	// Adjust edit box size
	CRect rctEditBound;
	m_edit1.GetClientRect(rctEditBound);
	rctEditBound.InflateRect(2, 2);
	m_edit2.SetWindowPos(
		NULL, 
		0, 0, 
		rctEditBound.Width(), rctEditBound.Height(), 
		SWP_NOMOVE | SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWPersonalize::OnSetActive() 
{
	m_strTopText.LoadString(IDS_W_PGTEXT1);

	CStringArray searchStrs;
	GetSheet()->RetrieveSearchStrings(searchStrs);

	// Set edit box captions
	m_strCaption1.LoadString(IDS_W_PRECIP);
	m_strCaption2.LoadString(IDS_W_PSENDER);

	// See if we need to only display one edit box
	m_edit1.ShowWindow(TRUE);
	m_pContainer->SetUserText1Valid(TRUE);

	BOOL fShow2nd = (searchStrs.GetSize() > 1);
	(GetDlgItem(IDC_TEXT3))->ShowWindow(fShow2nd);
	m_edit2.ShowWindow(fShow2nd);
	m_pContainer->SetUserText2Valid(fShow2nd);

	if (!fShow2nd)
	{
		// May need to change text in edit 1 for special cases
		if (((CWGreetingPageContainer*)m_pContainer)->GetCategory() == CWGreetingPageContainer::CATEGORY_INVITATION &&
			((CWGreetingPageContainer*)m_pContainer)->GetType() == CWGreetingPageContainer::INV_TYPE_NEWYEARS)
		{
			// Show only the sender's name
			m_strCaption1.LoadString(IDS_W_PNAME);
		}
	}

	UpdateData(FALSE);
	
	// Bypass
	UpdatePreview(FALSE);
	return CPropertyPage::OnSetActive();
}

LRESULT CWPersonalize::OnWizardNext() 
{
	// Tally up changes
	UpdateData();

	m_pContainer->SetUserText1(m_strText1);
	m_pContainer->SetUserText2(m_strText2);

	return CWHelpPropertyPage::OnWizardNext();
}

/////////////////////////////////////////////////////////////////////////////
// CWGreetingView dialog

CWGreetingView::CWGreetingView(CWHelpPageContainer* pContainer, UINT nIDContainerItem) :
	CWView(pContainer, nIDContainerItem)
{

}

void CWGreetingView::DoDataExchange(CDataExchange* pDX)
{
	CWView::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CWGreetingView)
   DDX_Control(pDX, IDC_WH_VIEW_FRONT, m_btnFront);
   DDX_Control(pDX, IDC_WH_VIEW_INSIDE, m_btnInside);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWGreetingView, CWView)
	//{{AFX_MSG_MAP(CWGreetingView)
   ON_BN_CLICKED(IDC_WH_VIEW_FRONT, OnViewFront)
   ON_BN_CLICKED(IDC_WH_VIEW_INSIDE, OnViewInside)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWGreetingView message handlers

BOOL CWGreetingView::OnInitDialog() 
{
	CWView::OnInitDialog();

	// Enable the radio button controls
	m_btnFront.ShowWindow(SW_SHOW);
	m_btnInside.ShowWindow(SW_SHOW);	
	m_btnFront.EnableWindow(TRUE);
	m_btnInside.EnableWindow(TRUE);	
	m_btnFront.SetCheck(TRUE);

	// View front of card
	m_preview.ForcePreviewToFront(TRUE);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWGreetingView::OnSetActive() 
{
	// Open the document
	if (!CWView::OnSetActive())
		return FALSE;

	if (m_btnInside.GetCheck())
	{
		SetCardPanel(CARD_PANEL_Inside);
	}
	else
	{
		SetCardPanel(CARD_PANEL_Front);
	}

	// Show final text
	m_strTopText.LoadString(IDS_WH_GREETING_FINAL);
	UpdateData(FALSE);

	return TRUE;
}

void CWGreetingView::OnViewFront() 
{
	//m_btnFront.SetCheck(!m_btnFront.GetCheck());
	//m_btnInside.SetCheck(!m_btnFront.GetCheck());

	SetCardPanel(CARD_PANEL_Front);
}

void CWGreetingView::OnViewInside() 
{
	//m_btnInside.SetCheck(!m_btnInside.GetCheck());
	//m_btnFront.SetCheck(!m_btnInside.GetCheck());

	SetCardPanel(CARD_PANEL_Inside);
}

void CWGreetingView::SetCardPanel(SHORT panel)
{
   // need to cast to CCardDoc
   CCardDoc *pDoc = (CCardDoc*)m_pDoc;
   if (pDoc->set_panel(panel)) {
		m_preview.ForcePreviewToFront((panel == CARD_PANEL_Front));
		m_preview.SizePreview();
      m_preview.Invalidate();
   }
}
