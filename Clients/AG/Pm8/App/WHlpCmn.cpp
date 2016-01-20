//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/WHlpCmn.cpp 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/WHlpCmn.cpp $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 23    12/23/98 1:51p Psasse
// Code clean up
// 
// 22    9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 21    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 20    6/04/98 9:51p Hforman
// 
// 19    6/04/98 6:21p Rlovejoy
// Threw ditto strings into the engine.
// 
// 18    6/04/98 2:12p Rlovejoy
// Added flag to allow user to avoid updating the description string.
// 
// 17    6/04/98 10:41a Rlovejoy
// Fixed redraw flicker.
// 
// 16    6/03/98 9:46p Hforman
// add label previewing
// 
// 15    6/03/98 7:07p Rlovejoy
// Code to display description string.
// 
// 14    6/02/98 2:32p Rlovejoy
// Call virtual ModifyPreviewName().
// 
// 13    6/01/98 11:48p Hforman
// changed IDs
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "bmptiler.h"
#include "carddoc.h"
#include "utils.h"
#include "WHlpCmn.h"
#include "WHlpWiz.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWHelpPropertyPage dialog

int CWHelpPropertyPage::m_nCurSelection = 0;

CWHelpPropertyPage::CWHelpPropertyPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem, UINT nIDTemplate) : 
	CPropertyPage(nIDTemplate)
{
	//{{AFX_DATA_INIT(CWHelpPropertyPage)
	m_strTopText = _T("");
	m_strLowerText = _T("");
	//}}AFX_DATA_INIT

	m_pContainer = pContainer;
	m_nIDContainerItem = nIDContainerItem;

	m_fRedrawPreview = TRUE;
	m_fEliminateProjectTypes = FALSE;

	m_pDoc = NULL;
}

CWHelpPropertyPage::~CWHelpPropertyPage()
{

}

void CWHelpPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CWHelpPropertyPage)
	DDX_Control(pDX, IDC_WH_PREVIEW, m_preview);
	DDX_Text(pDX, IDC_WH_TOP_TEXT, m_strTopText);
	DDX_Text(pDX, IDC_WH_LOWER_TEXT, m_strLowerText);
	//}}AFX_DATA_MAP
}

void CWHelpPropertyPage::OpenDocument(CString& csDocName, BOOL fExternal /*= FALSE*/)
{
	BeginWaitCursor();
	
	// Deliver the goods
	if (!fExternal)
	{
		CString csFileName = "[[G:WITHHELP]]\\" + csDocName;

		LPCSTR lpszFileName = (const char*)csFileName;
		CString csName;
		CString csDestPath;
		CString csExtension;
		CString csFinalName;
      

		ReadOnlyFile Source(lpszFileName);

		Util::SplitPath(lpszFileName, NULL, NULL, &csName, &csExtension);
		csDestPath = GET_PMWAPP()->GetTransferDirectory();
		Util::AppendBackslashToPath(csDestPath);
		csDestPath += csName;
		csDestPath += csExtension;

		// We have to copy the data out of the content database to its
		// own file in order to open it.
		StorageFile Dest(csDestPath);

		if (copy_file(&Source, &Dest) == ERRORCODE_None)
		{
			Dest.flush();
			csFinalName = GetGlobalPathManager()->ExpandPath(Dest.get_name());
			m_pDoc = (CPmwDoc*)GET_PMWAPP()->OpenHiddenDocument(csFinalName);
		}

		Dest.flush();
		Dest.zap();
	}
	else
	{
		// Load external file
		m_pDoc = (CPmwDoc*)GET_PMWAPP()->OpenHiddenDocument(csDocName);
	}

   EndWaitCursor();

}

void CWHelpPropertyPage::CloseDocument() 
{
   if (m_pDoc) {
		m_pDoc->OnCloseDocument();		
      m_pDoc = NULL;
   }
}

void CWHelpPropertyPage::UpdatePreview(BOOL fShowDescription /* = TRUE*/) 
{
	// Retrieve data for the current selection
	CString csData, csName, csDescription;
	GetSheet()->RetrieveSelectedData(m_nCurSelection, csData, csName, csDescription);

	// Redraw preview
	BOOL fRedrawPreview = m_fRedrawPreview;
	m_preview.SetType(CWithHelpPreview::EMPTY_PREVIEW);
	if (csData == "CompressedBitmap")
	{
		// Load compressed bitmap
		if (m_pContainer != NULL)
		{
			m_pContainer->ModifyPreviewName(GetSheet()->GetActiveIndex(), csName);
		}
		fRedrawPreview = m_preview.VerifyUpdateCBitmap((const char*)csName);
		m_preview.SetType(CWithHelpPreview::CBITMAP_PREVIEW);
	}
	else if (csData == "LabelPreview")
	{
		m_preview.SetType(CWithHelpPreview::LABEL_PREVIEW);
	}
	else if (!csData.IsEmpty())
	{
		// It's a document template
		CloseDocument();
		OpenDocument(csData);
		m_preview.SetType(CWithHelpPreview::TEMPLATE_PREVIEW);
		m_preview.UpdateDocPreview(m_pDoc);
		GetSheet()->SetSelProject(csData);	// retain the selected project
	}

	// Check for choice description
	// Have to set text manually
	if (fShowDescription)
	{
		if (csDescription.IsEmpty() || csDescription[0] != '^')
		{
			m_strLowerText = csDescription;
			CWnd* pLowerText = GetDlgItem(IDC_WH_LOWER_TEXT);
			pLowerText->SetWindowText(m_strLowerText);
			CRect rctLowerBounds;
			pLowerText->GetWindowRect(rctLowerBounds);
			ScreenToClient(rctLowerBounds);
			InvalidateRect(rctLowerBounds, FALSE);
		}
	}

	if (fRedrawPreview)
	{
		CRect rctPreviewBounds;
		m_preview.GetWindowRect(rctPreviewBounds);
		ScreenToClient(rctPreviewBounds);
		InvalidateRect(rctPreviewBounds, FALSE);
	}
}

#define  GUY_NUM     5

void CWHelpPropertyPage::DrawFigure(CDC* pDC)
{
	CBitmap  BitmapWhole;
	if (BitmapWhole.LoadBitmap (IDB_MASTER_SMALL) == FALSE)
		return;

	BITMAP bm;
	BitmapWhole.GetObject(sizeof(bm), &bm);
	CRect rcGuy(0, 0, bm.bmWidth / GUY_NUM, bm.bmHeight);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcDest = rcGuy;
	rcDest.OffsetRect(rcClient.right - rcGuy.Width() - 20, rcClient.top);
	rcGuy.OffsetRect(2/*m_GuyFrame*/ * rcGuy.Width (), 0);

	Util::TransparentBltBitmap(*pDC, rcDest, BitmapWhole, rcGuy, pOurPal);

	BitmapWhole.DeleteObject();
}

BEGIN_MESSAGE_MAP(CWHelpPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWHelpPropertyPage)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWHelpPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Set up document picture preview
	m_preview.InitializeDocPreview(this, IDC_WH_PREVIEW);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CWHelpPropertyPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

   CPalette *pPaletteOld = NULL;
	pPaletteOld = dc.SelectPalette(pOurPal, FALSE);
	dc.RealizePalette();

	// Draw the background
	CBmpTiler bmpTiler;
	CBitmap* pBitmap = GetSheet()->GetBkgrndBmp();
	if (pBitmap)
	{
		CRect rect;
		GetClientRect(rect);
		bmpTiler.TileBmpInRect(dc, rect, *pBitmap);
	}

	DrawFigure(&dc);

	if (pPaletteOld)
		dc.SelectPalette(pPaletteOld, FALSE);
}

HBRUSH CWHelpPropertyPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_BTN)
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

LRESULT CWHelpPropertyPage::OnWizardNext() 
{
	// Close the document
	CloseDocument();

	// Enable the "Back" button
	CWnd* pWnd = GetSheet()->GetDlgItem(ID_WIZBACK);
	if (pWnd)
		pWnd->EnableWindow(TRUE);

	return CPropertyPage::OnWizardNext();
}

LRESULT CWHelpPropertyPage::OnWizardBack() 
{
	// Close the document
	CloseDocument();
	
	// Make sure we're not showing "Finish"
	CWHelpWizPropertySheet* pSheet = GetSheet();
	pSheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	// Disable the "Back" button if we're going to display page 1
	if (pSheet->GetActiveIndex() == 1)
	{
		CWnd* pWnd = pSheet->GetDlgItem(ID_WIZBACK);
		if (pWnd)
			pWnd->EnableWindow(FALSE);
	}

	return CPropertyPage::OnWizardBack();
}

void CWHelpPropertyPage::OnCancel() 
{
	// Close the document
	CloseDocument();
	
	CPropertyPage::OnCancel();
}

BOOL CWHelpPropertyPage::OnSetActive() 
{
	// Retrieve dialog strings and lay them out (currently only 1 is used)
	CStringArray csStrings;
	GetSheet()->GetDlgStrings(csStrings, m_fEliminateProjectTypes);

	// Set title
	m_strTopText = csStrings[0];
	UpdateData(FALSE);

	UpdatePreview();
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CWHelpCommonPropertyPage property page

CWHelpCommonPropertyPage::CWHelpCommonPropertyPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem, UINT nIDTemplate /*= CWHelpCommonPropertyPage::IDD*/)
 : CWHelpPropertyPage(pContainer, nIDContainerItem, nIDTemplate)
{
	//{{AFX_DATA_INIT(CWHelpCommonPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CWHelpCommonPropertyPage::~CWHelpCommonPropertyPage()
{
}

void CWHelpCommonPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CWHelpCommonPropertyPage)
	DDX_Control(pDX, IDC_CHOICE_LIST, m_listChoices);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWHelpCommonPropertyPage, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CWHelpCommonPropertyPage)
	ON_LBN_SELCHANGE(IDC_CHOICE_LIST, OnSelchangeChoiceList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHelpCommonPropertyPage message handlers

BOOL CWHelpCommonPropertyPage::OnInitDialog() 
{
	CWHelpPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CWHelpCommonPropertyPage::OnSetActive() 
{
	// Follow the branch
	int nDirectionTaken = GetSheet()->FollowBranch(this);

	// Set up project type list box if not backing in
	if (nDirectionTaken != -1)
	{
		// Retrieve dialog strings and lay them out
		CStringArray csStrings;
		GetSheet()->GetDlgStrings(csStrings, m_fEliminateProjectTypes);

		int nTypes = csStrings.GetSize();
		m_listChoices.ResetContent();
		for (int i = 2; i < nTypes; i++)
		{
			m_listChoices.AddString(csStrings[i]);
		}
		m_listChoices.SetCurSel(0);
	}

	SetCurSelection(m_listChoices.GetCurSel());
	UpdatePreview();

	return CWHelpPropertyPage::OnSetActive();
}

void CWHelpCommonPropertyPage::OnSelchangeChoiceList() 
{
	// Send over list selection
	SetCurSelection(m_listChoices.GetCurSel());
	UpdatePreview();
}

LRESULT CWHelpCommonPropertyPage::OnWizardNext() 
{
	// Store choice
	int nChoice = m_listChoices.GetCurSel();
	if (m_pContainer != NULL)
	{
		m_pContainer->StoreUserInfo(GetSheet()->GetActiveIndex(), nChoice);
	}

	// Branch ahead in the table
	GetSheet()->FindBranchAhead(nChoice);

	return CWHelpPropertyPage::OnWizardNext();
}

LRESULT CWHelpCommonPropertyPage::OnWizardBack() 
{
	// Branch back in the table
	GetSheet()->FindBranchBack();
	
	return CWHelpPropertyPage::OnWizardBack();
}

/////////////////////////////////////////////////////////////////////////////
// CWithHelpPreview

// Construction
CWithHelpPreview::CWithHelpPreview()
 : CProjectMakePreview(FALSE, FALSE)
{
	m_PreviewType = EMPTY_PREVIEW;
	m_pLabelData = NULL;
}

CWithHelpPreview::~CWithHelpPreview()
{

}

BOOL CWithHelpPreview::InitializeDocPreview(CWnd* pDialog, UINT uPrevID, UINT uDescID /*= 0*/)
{
	return m_ppDocPict.Initialize(pDialog, uPrevID);
}

void CWithHelpPreview::UpdateDocPreview(CPmwDoc* pDoc)
{
	m_ppDocPict.NewDocPreview(pDoc, FALSE);
}

void CWithHelpPreview::SizePreview()
{
	m_ppDocPict.SizeDocPreview();
}

void CWithHelpPreview::ForcePreviewToFront(BOOL fForce)
{
	m_ppDocPict.ForceToFront(fForce);
}

BOOL CWithHelpPreview::VerifyUpdateCBitmap(const char *cnName)
{
	if (m_csCBitmapName == cnName)
		return FALSE;

	UpdateCBitmap(cnName);
	return TRUE;
}
	
BEGIN_MESSAGE_MAP(CWithHelpPreview, CProjectMakePreview)
	//{{AFX_MSG_MAP(CWithHelpPreview)
   ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
	
void CWithHelpPreview::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_PreviewType == CBITMAP_PREVIEW)
	{
		CProjectMakePreview::DrawItem(lpDrawItemStruct);	
	}
	else if (m_PreviewType == TEMPLATE_PREVIEW)
	{
		m_ppDocPict.DrawPreview(lpDrawItemStruct);
	}
	else if (m_PreviewType == LABEL_PREVIEW)
	{
		if (m_pLabelData)
		{
			CRect crClient;
			GetClientRect(crClient);
			CDC* pDC = GetDC();
			m_pLabelData->DrawPage(pDC, crClient);//, FALSE, &m_crDrawn);
			ReleaseDC(pDC);
		}
	}
}

BOOL CWithHelpPreview::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
