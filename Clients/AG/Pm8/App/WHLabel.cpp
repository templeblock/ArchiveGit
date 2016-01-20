//////////////////////////////////////////////////////////////////////////////
// $Heading: WHLabel.cpp$
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"

#include "namerec.h"
#include "macrostd.h"
#include "addrbook.h"
#include "lbldoc.h"
#include "Fielddlg.h"
#include "pmwcfg.h"

#include "WHlpView.h"
#include "WHlpWiz.h"
#include "WHlpCmn.h"
#include "WHLabel.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

#ifndef LOCALIZE
static LPCSTR
ReceiverStrings[] =
{
	"First Name", " ", "Last Name", "\n",
	"Title", "\n",
	"Company", "\n",
	"Address1", "\n",
	"Address2", "\n",
	"City", ", ", "State", " ", "Zip",
	NULL
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelPageContainer

CLabelPageContainer::CLabelPageContainer(CWHelpWizPropertySheet* pSheet)
	: CWHelpPageContainer(pSheet)
{
	// Build property pages for each step in the wizard process
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_WH_LABEL_TYPE));
	m_aPages.Add(new CWHelpAveryPropertyPage(this, ID_WH_LABEL_AVERYTYPE));
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_WH_LABEL_CATEGORY));
	m_aPages.Add(new CWHelpCommonPropertyPage(this, ID_WH_LABEL_STYLE));
	m_aPages.Add(new CWHCustomizeLabelPage(this, ID_WH_LABEL_CUSTOMIZE));
	m_aPages.Add(new CWHLabelTextLabelPage(this, ID_WH_LABEL_LABELTEXT));
	m_aPages.Add(new CWHLabelView(this, ID_WH_LABEL_HERE_IT_IS));

	m_nTextOption = 0;
	m_strLabelText = "";
	m_pSheet = pSheet;
}
	
CLabelPageContainer::~CLabelPageContainer()
{

}

/////////////////////////////////////////////////////////////////////////////
// CWHCustomizeLabelPage dialog

CWHCustomizeLabelPage::CWHCustomizeLabelPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
	: CWHelpPropertyPage(pContainer, nIDContainerItem, CWHCustomizeLabelPage::IDD)
{
	m_pContainer = (CLabelPageContainer*)pContainer;
}

void CWHCustomizeLabelPage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWHCustomizeLabelPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWHCustomizeLabelPage, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CWHCustomizeLabelPage)
	ON_BN_CLICKED(IDC_SAMETEXTRADIO, OnTextOptionClick)
	ON_BN_CLICKED(IDC_ADDRESSBOOKRADIO, OnTextOptionClick)
	ON_BN_CLICKED(IDC_NOTEXTRADIO, OnTextOptionClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHCustomizeLabelPage message handlers

void CWHCustomizeLabelPage::OnTextOptionClick()
{
	CButton* pSameText = (CButton*)GetDlgItem(IDC_SAMETEXTRADIO);
	CButton* pAddressBook = (CButton*)GetDlgItem(IDC_ADDRESSBOOKRADIO);
	CButton* pNoText = (CButton*)GetDlgItem(IDC_NOTEXTRADIO);

	int nOption = 0;

	if(pAddressBook && pAddressBook->GetCheck())
	{
		nOption = CLabelPageContainer::ADDRESS_BOOK;
		m_strLowerText.LoadString(IDS_WH_LABEL_TEXT_ADDR);
	}
	else if(pNoText && pNoText->GetCheck())
	{
		nOption = CLabelPageContainer::NO_TEXT;
		m_strLowerText.LoadString(IDS_WH_LABEL_TEXT_NONE);
	}
	else
	{
		m_strLowerText.LoadString(IDS_WH_LABEL_TEXT_SAME);
	}

	UpdateData(FALSE);

	// we have to force a repaint
	CWnd* pLowerText = GetDlgItem(IDC_WH_LOWER_TEXT);
	CRect rctLowerBounds;
	pLowerText->GetWindowRect(rctLowerBounds);
	ScreenToClient(rctLowerBounds);
	InvalidateRect(rctLowerBounds, FALSE);
	
	m_pContainer->SetTextOption(nOption);

}


BOOL CWHCustomizeLabelPage::OnInitDialog()
{
	CWHelpPropertyPage::OnInitDialog();
	CButton* pButton;

	switch(m_pContainer->GetTextOption())
	{
	case 0:
		pButton = (CButton*)GetDlgItem(IDC_SAMETEXTRADIO);
		pButton->SetCheck(TRUE);
		break;
	case 1:
		pButton = (CButton*)GetDlgItem(IDC_ADDRESSBOOKRADIO);
		pButton->SetCheck(TRUE);
		break;
	case 2:
		pButton = (CButton*)GetDlgItem(IDC_NOTEXTRADIO);
		pButton->SetCheck(TRUE);
		break;
	default:
		break;
	}

	m_strTopText.LoadString(IDS_WH_LABEL_TEXT_OPTION);
	m_strLowerText.LoadString(IDS_WH_LABEL_TEXT_SAME);
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CWHCustomizeLabelPage::OnWizardNext() 
{
	if(m_pContainer->GetTextOption() >= CLabelPageContainer::ADDRESS_BOOK)
	{
		m_pContainer->SetUserText1(m_pContainer->GetLabelText());
		m_pContainer->GetSheet()->SetActivePage(m_pContainer->GetSheet()->GetPageIndex(m_pContainer->GetSheet()->GetActivePage()) + 1);
	}

	if(m_pContainer->GetTextOption() == CLabelPageContainer::NO_TEXT)
	{
		m_pContainer->SetUserText1("");
		m_pContainer->GetSheet()->SetActivePage(m_pContainer->GetSheet()->GetPageIndex(m_pContainer->GetSheet()->GetActivePage()) + 1);
	}


	return CWHelpPropertyPage::OnWizardNext();
}

LRESULT CWHCustomizeLabelPage::OnWizardBack() 
{	
	return CWHelpPropertyPage::OnWizardBack();
}

BOOL CWHCustomizeLabelPage::OnSetActive() 
{
	UpdatePreview(FALSE);

	return CPropertyPage::OnSetActive();
}
/////////////////////////////////////////////////////////////////////////////
// CWHLabelTextLabelPage dialog

CWHLabelTextLabelPage::CWHLabelTextLabelPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
	: CWHelpPropertyPage(pContainer, nIDContainerItem, CWHLabelTextLabelPage::IDD)
{
	m_pContainer = (CLabelPageContainer*)pContainer;
}

void CWHLabelTextLabelPage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWHLabelTextLabelPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWHLabelTextLabelPage, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CWHLabelTextLabelPage)
	ON_EN_KILLFOCUS(IDC_WH_LABELTEXTEDIT, OnKillFocusLabelTextEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHLabelTextLabelPage message handlers

void CWHLabelTextLabelPage::OnKillFocusLabelTextEdit()
{
	CEdit* pLabelText = (CEdit*)GetDlgItem(IDC_WH_LABELTEXTEDIT);

	pLabelText->GetWindowText(m_pContainer->GetLabelText());
}


BOOL CWHLabelTextLabelPage::OnInitDialog()
{
	CWHelpPropertyPage::OnInitDialog();

	CEdit* pLabelText = (CEdit*)GetDlgItem(IDC_WH_LABELTEXTEDIT);
	if (pLabelText)
		pLabelText->SetWindowText(m_pContainer->GetLabelText());

	m_strTopText.LoadString(IDS_W_PPTEXT1);

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CWHLabelTextLabelPage::OnWizardNext() 
{
	m_pContainer->SetUserText1(m_pContainer->GetLabelText());

	return CWHelpPropertyPage::OnWizardNext();
}

LRESULT CWHLabelTextLabelPage::OnWizardBack() 
{	
	return CWHelpPropertyPage::OnWizardBack();
}

BOOL CWHLabelTextLabelPage::OnSetActive() 
{
	UpdatePreview(FALSE);

	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// CWHLabelView dialog

CWHLabelView::CWHLabelView(CWHelpPageContainer* pContainer, UINT nIDContainerItem) :
	CWView(pContainer, nIDContainerItem)
{
	m_pContainer = (CLabelPageContainer*)pContainer;
}

BEGIN_MESSAGE_MAP(CWHLabelView, CWView)
	//{{AFX_MSG_MAP(CWHLabelView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHLabelView message handlers

BOOL CWHLabelView::OnSetActive() 
{
	if(m_pContainer->GetTextOption() == CLabelPageContainer::ADDRESS_BOOK)
	{
		m_pContainer->SetUserText1Valid(FALSE);
		m_pContainer->SetUserText2Valid(FALSE);
	}
	else
	{
		m_pContainer->SetUserText1Valid(TRUE);
		m_pContainer->SetUserText2Valid(FALSE);

		if (m_pContainer->GetTextOption() == CLabelPageContainer::NO_TEXT)
			m_pContainer->SetUserText1("");
	}

	// Open the document
	if (!CWView::OnSetActive())
		return FALSE;

	if (m_pContainer->GetTextOption() == CLabelPageContainer::ADDRESS_BOOK)
		InsertReceiverBlock();
	
	// Show final text
	m_strTopText.LoadString(IDS_WH_LABEL_FINAL);
	UpdateData(FALSE);

	return TRUE;
}

BOOL CWHLabelView::OnWizardFinish() 
{	
	if(m_pContainer->GetTextOption() == CLabelPageContainer::ADDRESS_BOOK)
	{
		CWnd *p; 
		if ((p = AfxGetMainWnd ()) != NULL) 
		{
			//make sure the label draws first!
			m_pContainer->GetSheet()->SetDocumentFlags(CPmwDoc::FLAG_BringUpReceiverList, 0);
		}
	}

	return CWView::OnWizardFinish();
}

LRESULT CWHLabelView::OnWizardBack() 
{	
	if(m_pContainer->GetTextOption() >= CLabelPageContainer::ADDRESS_BOOK)
		m_pContainer->GetSheet()->SetActivePage(m_pContainer->GetSheet()->GetPageIndex(m_pContainer->GetSheet()->GetActivePage()) - 1);

	return CWView::OnWizardBack();
}

void CWHLabelView::InsertReceiverBlock()
{
#ifdef LOCALIZE
	CString csAddressFormat;
	csAddressFormat.LoadString(IDS_RECEIVER_ADDRESS_FORMAT);
	CString csTemp = csAddressFormat;
	LPSTR lpszTemp = csTemp.GetBuffer(csTemp.GetLength()+1);
	InsertBlock((LPCSTR *) ParseAddressFormat(lpszTemp));
	csTemp.ReleaseBuffer();
#else
	InsertBlock(ReceiverStrings);
#endif
}

void CWHLabelView::InsertBlock(LPCSTR* pStrings)
{
	ASSERT(pStrings);
	if (!pStrings) return;
	
	CWordArray wArray;

	int nMacros = m_pDoc->GetMacroServer()->GetMacroList()->Macros();

	while (*pStrings != NULL)
	{
		LPCSTR pString = *pStrings++;

	/* Look for a macro matching this string. */
		for (int nMacro = 0; nMacro < nMacros; nMacro++)
		{
			CMacro* pMacro = m_pDoc->GetMacroServer()->GetMacroList()->Macro(nMacro);
			if (pMacro != NULL && pMacro->Name() == pString)
			{
				
				CHARACTER c = m_pDoc->GetMacroServer()->GetMacroCharacter(nMacro);

				wArray.Add(c);

				break;
			}
		}

		if (nMacro >= nMacros)
		{
		/* Insert the string character by character. */
			while (*pString != '\0')
			{
				wArray.Add(*pString++);
			}
		}
	}
	
	CStringArray searchStrs;
	GetSheet()->RetrieveSearchStrings(searchStrs);
	FindAndReplace(m_pDoc, searchStrs[0] /*"<label text>"*/, wArray.GetData(), wArray.GetSize());
}


/////////////////////////////////////////////////////////////////////////////
// CWHelpAveryPropertyPage property page

CWHelpAveryPropertyPage::CWHelpAveryPropertyPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
 : CWHelpCommonPropertyPage(pContainer, nIDContainerItem)

{
	//{{AFX_DATA_INIT(CWHelpAveryPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_LabelList.ReadList("LABELS.DAT");
}

CWHelpAveryPropertyPage::~CWHelpAveryPropertyPage()
{
}

void CWHelpAveryPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpCommonPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWHelpAveryPropertyPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWHelpAveryPropertyPage, CWHelpCommonPropertyPage)
	//{{AFX_MSG_MAP(CWHelpAveryPropertyPage)
	ON_LBN_SELCHANGE(IDC_CHOICE_LIST, OnSelchangeChoiceList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHelpAveryPropertyPage message handlers

BOOL CWHelpAveryPropertyPage::OnInitDialog() 
{
	CWHelpCommonPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CWHelpAveryPropertyPage::SetLabelData(int nChoice)
{
	CString csData, csName, csDescription;
	GetSheet()->RetrieveSelectedData(nChoice, csData, csName, csDescription);

	CLabelData* pLabelData = m_LabelList.FindLabel(csName);
	if (pLabelData)
	{
		m_preview.m_pLabelData = pLabelData;
	}
	else
	{
		ASSERT(0);
		m_preview.m_pLabelData = NULL;
	}
}

BOOL CWHelpAveryPropertyPage::OnSetActive()
{
	// must call baseclass first
	BOOL fOK = CWHelpCommonPropertyPage::OnSetActive();
	if (fOK)
	{
		int nChoice = m_listChoices.GetCurSel();
		SetLabelData(nChoice);
	}

	return fOK;
}

void CWHelpAveryPropertyPage::OnSelchangeChoiceList() 
{
	int nChoice = m_listChoices.GetCurSel();
	SetLabelData(nChoice);

	CWHelpCommonPropertyPage::OnSelchangeChoiceList();
}
