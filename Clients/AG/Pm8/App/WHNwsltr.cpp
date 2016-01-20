//////////////////////////////////////////////////////////////////////////////
// $Heading: $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/WHNwsltr.cpp $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:17p Hforman
// Changed name from Newslett.cpp
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "pagerec.h"
#include "pmwprint.h"
#include "newsview.h"
#include "newsdoc.h"

#include "WHlpView.h"
#include "WHlpWiz.h"
#include "WHlpCmn.h"
#include "WHNwsltr.h"

/////////////////////////////////////////////////////////////////////////////
// CNewsLetterPageContainer

CNewsLetterPageContainer::CNewsLetterPageContainer(CWHelpWizPropertySheet* pSheet)
	: CWHelpPageContainer(pSheet)
{
	// Build property pages for each step in the wizard process
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_NEWSLETTER_CATEGORY));
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_NEWSLETTER_STYLE));
	m_aPages.Add(new CWHNewsletterPage(this, ID_NEWSLETTER_CUSTOMIZE));
	m_aPages.Add(new CWHNewsletterView(this, ID_NEWSLETTER_HERE_IT_IS));

	m_fDoubleSided = FALSE;
	m_strNumPages = "1";
}
	
CNewsLetterPageContainer::~CNewsLetterPageContainer()
{

}

/////////////////////////////////////////////////////////////////////////////
// CWHNewsletterPage dialog

CWHNewsletterPage::CWHNewsletterPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
	: CWHelpPropertyPage(pContainer, nIDContainerItem, CWHNewsletterPage::IDD)
{
	//{{AFX_DATA_INIT(CWHNewsletterPage)
	m_strNumPages = "1";
	//}}AFX_DATA_INIT

	m_fDoubleSided = FALSE;
	m_pContainer = (CNewsLetterPageContainer*)pContainer;
}

void CWHNewsletterPage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWHNewsletterPage)
	DDX_Text(pDX, IDC_NUMPAGESEDIT, m_strNumPages);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWHNewsletterPage, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CWHNewsletterPage)
	ON_BN_CLICKED(IDC_DOUBLESIDEDCHECK, OnDoubleSidedClick)
	ON_EN_KILLFOCUS(IDC_NUMPAGESEDIT, OnNumPagesChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHNewsletterPage message handlers
void CWHNewsletterPage::OnNumPagesChange()
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_NUMPAGESEDIT);

	pEdit->GetWindowText(m_strNumPages);



	m_pContainer->SetNumPages(m_strNumPages);
}

void CWHNewsletterPage::OnDoubleSidedClick()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_DOUBLESIDEDCHECK);

	m_fDoubleSided = (BOOL)pButton->GetCheck();

	m_pContainer->SetDoubleSided(m_fDoubleSided);

}

BOOL CWHNewsletterPage::OnInitDialog()
{
	CWHelpPropertyPage::OnInitDialog();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_NUMPAGESEDIT);
	if(pEdit)
	{
		pEdit->SetWindowText(m_strNumPages);
		pEdit->LimitText(2);
	}

	CButton* pButton = (CButton*)GetDlgItem(IDC_DOUBLESIDEDCHECK);
	if (pButton)
		pButton->SetCheck(m_fDoubleSided);

	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN1);
	if (pSpin)
		pSpin->SetRange(1, 20);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CWHNewsletterPage::OnSetActive() 
{
	UpdatePreview(FALSE);
	return CPropertyPage::OnSetActive();
}

LRESULT CWHNewsletterPage::OnWizardNext() 
{
	return CWHelpPropertyPage::OnWizardNext();
}

LRESULT CWHNewsletterPage::OnWizardBack() 
{	
	return CWHelpPropertyPage::OnWizardBack();
}

/////////////////////////////////////////////////////////////////////////////
// CWHNewsletterView dialog

CWHNewsletterView::CWHNewsletterView(CWHelpPageContainer* pContainer, UINT nIDContainerItem) :
	CWView(pContainer, nIDContainerItem)
{
	m_pContainer = (CNewsLetterPageContainer*)pContainer;
}

BEGIN_MESSAGE_MAP(CWHNewsletterView, CWView)
	//{{AFX_MSG_MAP(CWHNewsletterView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHNewsletterView message handlers



BOOL CWHNewsletterView::OnSetActive() 
{
	// Open the document
	if (!CWView::OnSetActive())
		return FALSE;

	int nSided = 1;
	int n = 0;
	if(m_pContainer->GetDoubleSided())
		nSided = 2;

	m_pDoc->GotoPage(0);

	m_pDoc->DocumentRecord()->SetSided(nSided);

	int nPages = m_pDoc->DocumentRecord()->NumberOfPages();

	int RequestedNumPages = atoi(m_pContainer->GetNumPages());

	if(RequestedNumPages > nPages)
	{
		m_pDoc->GotoPage(0);
		for(n = nPages; n < RequestedNumPages; n++){
		  //make all other pages a copy of the second (index = 1)
			AddPages(m_pDoc, n, 1);
		}
	}
	else if(RequestedNumPages < nPages)
	{
		m_pDoc->GotoPage(0);
		for(n = (nPages - 1); n > (RequestedNumPages - 1); n--){
		  m_pDoc->DocumentRecord()->delete_page(m_pDoc->DocumentRecord()->GetPage(n));
		}
	}

	// Show final text
	m_strTopText.LoadString(IDS_WH_NEWS_FINAL);
	UpdateData(FALSE);

	return TRUE;
}


void CWHNewsletterView::AddPages(CPmwDoc* pDocument, int indexTo, int indexFrom)
{

      DWORD dwPages = 1; //atol(Dialog.m_csPages);
      DWORD dwAtPage = indexTo;
      PMGDatabase* pDatabase = pDocument->get_database();
      PageRecord* pSourcePage = NULL;

         // We want to lock the page record corresponding to the page number
         // they specified (before inserting).
         //DB_RECORD_NUMBER lPage = atol(Dialog.m_csCopyPage)-1;
         pSourcePage = (PageRecord*)pDatabase->get_record(pDocument->GetPage(indexFrom), NULL, RECORD_TYPE_Page);
         ASSERT(pSourcePage != NULL);

      DWORD dwStartPage = dwAtPage;

      while (dwPages-- != 0)
      {
         if (!pDocument->AddPage(dwAtPage))
         {
            break;
         }
         if (pSourcePage != NULL)
         {
            // Lock the new page.
            PageRecord* pNewPage = (PageRecord*)pDatabase->get_record(pDocument->GetPage(dwAtPage), NULL, RECORD_TYPE_Page);
            if (pNewPage != NULL)
            {
               // Copy the objects over.
               pDatabase->CopyObjectList(pNewPage->objects(), pSourcePage->objects(), pNewPage->Id());

					// Reflow text boxes as needed.
					PMGPageObject* pFirstObject = (PMGPageObject*)pNewPage->objects()->first_object();
					if (pFirstObject != NULL)
					{
						pDocument->MarkFramesBelow(NULL, pFirstObject);
						pDocument->UpdateMarkedFrames(TRUE, pFirstObject);
					}
               pNewPage->release(TRUE);
            }
            else
            {
               // Could not get the newly created page!
               ASSERT(FALSE);
            }
         }
         // Copy the items if so desired.
         dwAtPage++;
      }

      // If we had a source page, release it now.
      if (pSourcePage != NULL)
      {
         pSourcePage->release();
      }

      if (dwAtPage > dwStartPage)
      {

         // The document is now modified.
         pDocument->SetModifiedFlag();

         // Change to the last page added.
         //BeforePageChange();
         pDocument->GotoPage(dwAtPage-1);
      }
}