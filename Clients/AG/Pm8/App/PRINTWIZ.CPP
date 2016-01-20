/*
// $Workfile: PRINTWIZ.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PRINTWIZ.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:06   Fred
// Initial revision.
// 
//    Rev 1.10   16 Jul 1997 10:15:14   Jay
// Don't memset fcs; it has a constructor
// 
//    Rev 1.9   23 Sep 1996 10:42:14   Jay
// Reversed printout fixes.
// 
//    Rev 1.8   22 Aug 1996 18:03:20   Jay
// Adds in DBLPRN_PrintOnBlank by default.
// 
//    Rev 1.7   16 Aug 1996 12:28:26   Fred
// Fixed 16-bit bug where choices were not recorded
// 
//    Rev 1.6   15 Aug 1996 19:10:46   Fred
// Fixed 16-bit OnSetActive() crashes
// 
//    Rev 1.5   15 Aug 1996 15:55:44   Fred
// 256 color wizard bitmaps
// 
//    Rev 1.4   08 Aug 1996 17:30:18   Fred
// More 16-bit changes
// 
//    Rev 1.3   06 Aug 1996 14:37:32   Jay
//  
// 
//    Rev 1.2   06 Aug 1996 11:17:42   Jay
//  
// 
//    Rev 1.1   01 Aug 1996 09:23:10   Fred
// More 16-bit changes
// 
//    Rev 1.0   24 Jul 1996 18:24:48   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "printwiz.h"
#include "util.h"
#include "paper.h"
#include "printer.h"
#include "frameobj.h"
#include "typeface.h"
#include "pmgfont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette *pOurPal;		// THE PrintMaster Palette (from PMWVIEW.CPP)

/////////////////////////////////////////////////////////////////////////////
// Utility

static void CreateFrame(CPmwView* pView, LPCSTR pszText, const PBOX& Bound, ALIGN_TYPE nHAlign, VERT_ALIGN_TYPE nVAlign)
{
	CPmwDoc* pDoc = pView->GetDocument();
	PMGDatabase* pDatabase = pDoc->get_database();

	FRAMEOBJ_CREATE_STRUCT fcs;

	fcs.bound = Bound;

	fcs.alignment = nHAlign;
	fcs.vert_alignment = nVAlign;
	fcs.flags = 0;
	fcs.pStyle = NULL;

	CFrameObject* pFrame = pDoc->create_frame_object(&fcs);
	if (pFrame != NULL)
	{
		pDoc->append_object(pFrame);

		CTextStyle Style(pDatabase);
		Style.SetDefault();

		// Set the face to "Geneva".
		int nFace = typeface_server.find_face("Geneva", FALSE);
		if (nFace != -1)
		{
			PMGFontServer* pFontServer = (PMGFontServer*)pDoc->get_font_server();

			DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nFace);
			if (lFace != 0)
			{
				Style.Font(lFace);
				pDatabase->free_font_record(lFace, TRUE);
			}
		}
		Style.Size(MakeFixed(150));
		Style.BaseSize(MakeFixed(150));
		Style.UpdateFontMetrics();

		CTxp Txp(pDatabase);

		Txp.Init(pFrame, 0);
		Txp.SetHorizontalAlignment(fcs.alignment);
		Txp.Style(Style);
		Txp.InsertString(pszText);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintButton

CDoubleSidedPrintButton::CDoubleSidedPrintButton()
{
	m_fChecked = FALSE;
}

void CDoubleSidedPrintButton::SetChecked(BOOL fChecked)
{
	if (m_fChecked != fChecked)
	{
		m_fChecked = fChecked;
		Invalidate();
	}
}

void CDoubleSidedPrintButton::PaintCustomize(CDC& dcButton, CRect crButton)
{
	if (m_fChecked)
	{
		crButton.InflateRect(-4, -4);
		if (GetSelect())
		{
			crButton.OffsetRect(1, 1);
		}
		Util::DrawBorderWithColor(dcButton, crButton, ::GetSysColor(COLOR_HIGHLIGHT), 2);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintWizard

CDoubleSidedPrintWizard::CDoubleSidedPrintWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPmwWizard(nIDCaption, pParentWnd, iSelectPage)
{
	CommonConstruct();
}

CDoubleSidedPrintWizard::CDoubleSidedPrintWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPmwWizard(pszCaption, pParentWnd, iSelectPage)
{
	CommonConstruct();
}

void CDoubleSidedPrintWizard::CommonConstruct(void)
{
	Util::LoadResourceBitmap(m_Bitmap, "DOUBLEPRINT", pOurPal);
}

CDoubleSidedPrintWizard::~CDoubleSidedPrintWizard()
{
}

void CDoubleSidedPrintWizard::InitDialog() 
{
	INHERITED::InitDialog();
	
	CString csTitle;
	TRY
	{
		csTitle.LoadString(IDS_DOUBLE_SIDED_PRINT_WIZARD);
		SetWindowText(csTitle);
	}
	END_TRY
}

BEGIN_MESSAGE_MAP(CDoubleSidedPrintWizard, CPmwWizard)
	//{{AFX_MSG_MAP(CDoubleSidedPrintWizard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDoubleSidedPrintWizard::Run(CWnd* pParent)
{
	BOOL fResult = FALSE;

	CDoubleSidedPrintWizard Wizard("", pParent, 0);
	
#ifdef WIN32
	Wizard.m_psh.dwFlags &= ~PSH_HASHELP;
	Wizard.m_Introduction.m_psp.dwFlags &= ~PSP_HASHELP;
	Wizard.m_PrePrint1.m_psp.dwFlags &= ~PSP_HASHELP;
	Wizard.m_PostPrint1.m_psp.dwFlags &= ~PSP_HASHELP;
	Wizard.m_PrePrint2.m_psp.dwFlags &= ~PSP_HASHELP;
	Wizard.m_PostPrint2.m_psp.dwFlags &= ~PSP_HASHELP;
	Wizard.m_Finish.m_psp.dwFlags &= ~PSP_HASHELP;
#endif

	Wizard.AddPage(&Wizard.m_Introduction);
	Wizard.AddPage(&Wizard.m_PrePrint1);
	Wizard.AddPage(&Wizard.m_PostPrint1);
	Wizard.AddPage(&Wizard.m_PrePrint2);
	Wizard.AddPage(&Wizard.m_PostPrint2);
	Wizard.AddPage(&Wizard.m_Finish);
	Wizard.SetWizardMode();

	if (Wizard.DoModal() == ID_WIZFINISH)
	{
		if (Wizard.m_PostPrint1.m_nChosen != -1 && Wizard.m_PostPrint2.m_nChosen != -1)
		{
			int nIndex = Wizard.m_PostPrint1.m_nChosen*4 + Wizard.m_PostPrint2.m_nChosen;
			ASSERT(nIndex < 8);
			static WORD wDoubleSidedTypes[] =
			{
				// A on top first.
				DBLPRN_UpDown,				// Then AC / blank visible
				DBLPRN_DownDown,			// Then B / C visible
				DBLPRN_UpDownReversed,	// Then AC / blankk visible, C reversed
				DBLPRN_DownDownReversed,// Then B / C visible, C reversed
				// B on top first.
				DBLPRN_UpUp,				// Then AD / blank visible
				DBLPRN_DownUp,				// Then B / D visible
				DBLPRN_UpUpReversed,		// Then AD / blank visible, D reversed
				DBLPRN_DownUpReversed,	// Then B / D visible, D reversed
			};
			WORD wDoubleSidedType = wDoubleSidedTypes[nIndex] + DBLPRN_Valid + DBLPRN_PrintOnBlank;

			// Set the value in the printer info.
			CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(PORTRAIT);
			pPrinterInfo->SetDoubleSidedType(wDoubleSidedType);
			GET_PMWAPP()->GetPrinterInfo(LANDSCAPE)->SetDoubleSidedType(wDoubleSidedType);

			// Portrait and landscape share in INI file.
			pPrinterInfo->WriteSettings();
		}
		else
		{
			// Some Chosen value is not set!
			ASSERT(FALSE);
		}
		fResult = TRUE;
	}

	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintWizard message handlers

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintIntroduction property page

CDoubleSidedPrintIntroduction::CDoubleSidedPrintIntroduction() :
	CPmwWizardPage(CDoubleSidedPrintIntroduction::IDD)
{
	//{{AFX_DATA_INIT(CDoubleSidedPrintIntroduction)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDoubleSidedPrintIntroduction::~CDoubleSidedPrintIntroduction()
{
}

void CDoubleSidedPrintIntroduction::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoubleSidedPrintIntroduction)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDoubleSidedPrintIntroduction, CPmwWizardPage)
	//{{AFX_MSG_MAP(CDoubleSidedPrintIntroduction)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintIntroduction message handlers

BOOL CDoubleSidedPrintIntroduction::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		SetWizardButtons(PSWIZB_NEXT);
	}
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPrePrint1 property page

CDoubleSidedPrintPrePrint1::CDoubleSidedPrintPrePrint1() :
	CPmwWizardPage(CDoubleSidedPrintPrePrint1::IDD)
{
	//{{AFX_DATA_INIT(CDoubleSidedPrintPrePrint1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDoubleSidedPrintPrePrint1::~CDoubleSidedPrintPrePrint1()
{
}

void CDoubleSidedPrintPrePrint1::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoubleSidedPrintPrePrint1)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDoubleSidedPrintPrePrint1, CPmwWizardPage)
	//{{AFX_MSG_MAP(CDoubleSidedPrintPrePrint1)
	ON_BN_CLICKED(IDC_PRINT, OnClickedPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPrePrint1 message handlers

BOOL CDoubleSidedPrintPrePrint1::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
	return fResult;
}

void CDoubleSidedPrintPrePrint1::OnClickedPrint()
{
	CPmwApp* pApp = GET_PMWAPP();

	// Create a newsletter project.
	pApp->last_new_info.type = PROJECT_TYPE_Newsletter;
	pApp->last_new_info.subtype = 0;
	pApp->last_new_info.orientation = PORTRAIT;

	// Paper info is printer info.
	*(pApp->last_new_info.m_pPaperInfo) = *(pApp->GetPaperInfo(pApp->last_new_info.orientation));

	WORD wFlags = CPmwDoc::m_wGlobalDocumentFlags;
	CPmwDoc::m_wGlobalDocumentFlags |= CPmwDoc::FLAG_TestPrintDocument;
	CPmwDoc* pDoc = (CPmwDoc*)(pApp->m_pScratchTemplate->OpenDocumentFile(NULL, FALSE));
	CPmwDoc::m_wGlobalDocumentFlags = wFlags;

	if (pDoc != NULL)
	{
#ifndef WIN32
		pDoc->DoNewDocument(-1, FALSE);
#endif

		CPmwView* pView = pDoc->GetCurrentView();
		// We have one page so far. Add the key text to it.

		// Create the key frame with text saying "FACE UP".
		WORD wDoubleSidedType = DBLPRN_Valid;
		pView->CreateKeyFrame(wDoubleSidedType);
		pView->CreateKeyArrow(wDoubleSidedType);
		CreateLeftFrame(pView, "A");

		ASSERT(pDoc->NumberOfPages() == 1);

		// Create the second page.
		pDoc->AddPage(1);
		pDoc->GotoPage(1);

		pView->CreateKeyFrame(wDoubleSidedType);
		pView->CreateKeyArrow(wDoubleSidedType);
		CreateLeftFrame(pView, "B");

		// Set the title for the print-out.
		CString csTitle;
		TRY
			csTitle.LoadString(IDS_PrintTest1);
		END_TRY
		pDoc->SetTitle(csTitle);

		// Do the print-out.
		pView->PrintNoDialog();

		pDoc->OnCloseDocument();
	}
}

void CDoubleSidedPrintPrePrint1::CreateLeftFrame(CPmwView* pView, LPCSTR pszText)
{
	PBOX Bound;
	Bound.x0 = PAGE_RESOLUTION/2;
	Bound.x1 = PAGE_RESOLUTION*4;
	Bound.y0 = PAGE_RESOLUTION/2;
	Bound.y1 = PAGE_RESOLUTION*4;

	CreateFrame(pView, pszText, Bound, ALIGN_left, ALIGN_top);
}

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPostPrint1 property page

CDoubleSidedPrintPostPrint1::CDoubleSidedPrintPostPrint1() :
	CPmwWizardPage(CDoubleSidedPrintPostPrint1::IDD)
{
	//{{AFX_DATA_INIT(CDoubleSidedPrintPostPrint1)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nChosen = -1;
}

CDoubleSidedPrintPostPrint1::~CDoubleSidedPrintPostPrint1()
{
}

void CDoubleSidedPrintPostPrint1::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoubleSidedPrintPostPrint1)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

LRESULT CDoubleSidedPrintPostPrint1::OnWizardNext() 
{
	m_nChosen = m_Output1.GetChecked() ? 0 : 1;
	return 0; // move to next page
}

BEGIN_MESSAGE_MAP(CDoubleSidedPrintPostPrint1, CPmwWizardPage)
	//{{AFX_MSG_MAP(CDoubleSidedPrintPostPrint1)
	ON_BN_CLICKED(IDC_PRINT_PAGE_1, OnClickedPage1)
	ON_BN_CLICKED(IDC_PRINT_PAGE_2, OnClickedPage2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPostPrint1 message handlers

BOOL CDoubleSidedPrintPostPrint1::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		UpdateButtons();
	}
	return fResult;
}

BOOL CDoubleSidedPrintPostPrint1::OnInitDialog() 
{
	INHERITED::OnInitDialog();

	m_Output1.Initialize(IDC_PRINT_PAGE_1, this);
	m_Output2.Initialize(IDC_PRINT_PAGE_2, this);

	// Load the bitmaps for the buttons.
	CBitmap* pBitmap = NULL;
	TRY
	{
		pBitmap = new CBitmap;
		pBitmap->LoadBitmap(IDB_PRINT1);
		m_Output1.SetBitmap(pBitmap, TRUE, FALSE);
		pBitmap = NULL;

		pBitmap = new CBitmap;
		pBitmap->LoadBitmap(IDB_PRINT2);
		m_Output2.SetBitmap(pBitmap, TRUE, FALSE);
		pBitmap = NULL;
	}
	CATCH_ALL(e)
	{
		delete pBitmap;
	}
	END_CATCH_ALL

	return TRUE;
}

void CDoubleSidedPrintPostPrint1::OnClickedPage1()
{
	m_Output2.SetChecked(FALSE);
	m_Output1.SetChecked(TRUE);
	UpdateButtons();
	ResetPostPrint2();
}

void CDoubleSidedPrintPostPrint1::OnClickedPage2()
{
	m_Output1.SetChecked(FALSE);
	m_Output2.SetChecked(TRUE);
	UpdateButtons();
	ResetPostPrint2();
}

void CDoubleSidedPrintPostPrint1::UpdateButtons(void)
{
	DWORD dwButtons = PSWIZB_BACK;
	if (m_Output1.GetChecked() || m_Output2.GetChecked())
	{
		dwButtons |= PSWIZB_NEXT;
	}
	SetWizardButtons(dwButtons);
}

void CDoubleSidedPrintPostPrint1::ResetPostPrint2(void)
{
	// Turn off all settings in the post print 2 page.
	CDoubleSidedPrintWizard* pWizard = (CDoubleSidedPrintWizard*)GetParent();
	pWizard->m_PostPrint2.m_Output1.SetChecked(FALSE);
	pWizard->m_PostPrint2.m_Output2.SetChecked(FALSE);
	pWizard->m_PostPrint2.m_Output3.SetChecked(FALSE);
	pWizard->m_PostPrint2.m_Output4.SetChecked(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPrePrint2 property page

CDoubleSidedPrintPrePrint2::CDoubleSidedPrintPrePrint2() :
	CPmwWizardPage(CDoubleSidedPrintPrePrint2::IDD)
{
	//{{AFX_DATA_INIT(CDoubleSidedPrintPrePrint2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDoubleSidedPrintPrePrint2::~CDoubleSidedPrintPrePrint2()
{
}

void CDoubleSidedPrintPrePrint2::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoubleSidedPrintPrePrint2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDoubleSidedPrintPrePrint2, CPmwWizardPage)
	//{{AFX_MSG_MAP(CDoubleSidedPrintPrePrint2)
	ON_BN_CLICKED(IDC_PRINT, OnClickedPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPrePrint2 message handlers

BOOL CDoubleSidedPrintPrePrint2::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
	return fResult;
}

void CDoubleSidedPrintPrePrint2::OnClickedPrint()
{
	CPmwApp* pApp = GET_PMWAPP();

	// Create a newsletter project.
	pApp->last_new_info.type = PROJECT_TYPE_Newsletter;
	pApp->last_new_info.subtype = 0;
	pApp->last_new_info.orientation = PORTRAIT;

	// Paper info is printer info.
	*(pApp->last_new_info.m_pPaperInfo) = *(pApp->GetPaperInfo(pApp->last_new_info.orientation));

	WORD wFlags = CPmwDoc::m_wGlobalDocumentFlags;
	CPmwDoc::m_wGlobalDocumentFlags |= CPmwDoc::FLAG_TestPrintDocument;
	CPmwDoc* pDoc = (CPmwDoc*)(pApp->m_pScratchTemplate->OpenDocumentFile(NULL, FALSE));
	CPmwDoc::m_wGlobalDocumentFlags = wFlags;

	if (pDoc != NULL)
	{
#ifndef WIN32
		pDoc->DoNewDocument(-1, FALSE);
#endif

		CPmwView* pView = pDoc->GetCurrentView();
		// We have one page so far. Add the key text to it.

		// Create the key frame with text saying "FACE UP".
		WORD wDoubleSidedType = DBLPRN_Valid;
		CreateRightFrame(pView, "C");

		ASSERT(pDoc->NumberOfPages() == 1);

		// Create the second page.
		pDoc->AddPage(1);
		pDoc->GotoPage(1);
		CreateRightFrame(pView, "D");

		// Set the title for the print-out.
		CString csTitle;
		TRY
			csTitle.LoadString(IDS_PrintTest2);
		END_TRY
		pDoc->SetTitle(csTitle);

		// Do the print-out.
		pView->PrintNoDialog();

		pDoc->OnCloseDocument();
	}
}

void CDoubleSidedPrintPrePrint2::CreateRightFrame(CPmwView* pView, LPCSTR pszText)
{
	PPNT Dims = pView->GetDocument()->get_dimensions();

	PBOX Bound;
	Bound.x0 = Dims.x - PAGE_RESOLUTION*4;
	Bound.x1 = Dims.x - PAGE_RESOLUTION/2;
	Bound.y0 = PAGE_RESOLUTION/2;
	Bound.y1 = PAGE_RESOLUTION*4;

	CreateFrame(pView, pszText, Bound, ALIGN_right, ALIGN_top);
}

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPostPrint2 property page

CDoubleSidedPrintPostPrint2::CDoubleSidedPrintPostPrint2() :
	CPmwWizardPage(CDoubleSidedPrintPostPrint2::IDD)
{
	//{{AFX_DATA_INIT(CDoubleSidedPrintPostPrint2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nChosen = -1;
}

CDoubleSidedPrintPostPrint2::~CDoubleSidedPrintPostPrint2()
{
}

void CDoubleSidedPrintPostPrint2::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoubleSidedPrintPostPrint2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

LRESULT CDoubleSidedPrintPostPrint2::OnWizardNext() 
{
	m_nChosen = m_Output1.GetChecked()
						? 0
						: m_Output2.GetChecked()
							? 1
							: m_Output3.GetChecked()
								? 2
								: 3;
	return 0; // move to next page
}

BEGIN_MESSAGE_MAP(CDoubleSidedPrintPostPrint2, CPmwWizardPage)
	//{{AFX_MSG_MAP(CDoubleSidedPrintPostPrint2)
	ON_BN_CLICKED(IDC_PRINT_PAGE_1, OnClickedPage1)
	ON_BN_CLICKED(IDC_PRINT_PAGE_2, OnClickedPage2)
	ON_BN_CLICKED(IDC_PRINT_PAGE_3, OnClickedPage3)
	ON_BN_CLICKED(IDC_PRINT_PAGE_4, OnClickedPage4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPostPrint2 message handlers

BOOL CDoubleSidedPrintPostPrint2::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		UpdateBitmaps();
		UpdateButtons();
	}
	return fResult;
}

BOOL CDoubleSidedPrintPostPrint2::OnInitDialog() 
{
	INHERITED::OnInitDialog();

	m_Output1.Initialize(IDC_PRINT_PAGE_1, this);
	m_Output2.Initialize(IDC_PRINT_PAGE_2, this);
	m_Output3.Initialize(IDC_PRINT_PAGE_3, this);
	m_Output4.Initialize(IDC_PRINT_PAGE_4, this);

	return TRUE;
}

void CDoubleSidedPrintPostPrint2::OnClickedPage1()
{
	m_Output4.SetChecked(FALSE);
	m_Output3.SetChecked(FALSE);
	m_Output2.SetChecked(FALSE);
	m_Output1.SetChecked(TRUE);
	UpdateButtons();
}

void CDoubleSidedPrintPostPrint2::OnClickedPage2()
{
	m_Output4.SetChecked(FALSE);
	m_Output3.SetChecked(FALSE);
	m_Output1.SetChecked(FALSE);
	m_Output2.SetChecked(TRUE);
	UpdateButtons();
}

void CDoubleSidedPrintPostPrint2::OnClickedPage3()
{
	m_Output4.SetChecked(FALSE);
	m_Output2.SetChecked(FALSE);
	m_Output1.SetChecked(FALSE);
	m_Output3.SetChecked(TRUE);
	UpdateButtons();
}

void CDoubleSidedPrintPostPrint2::OnClickedPage4()
{
	m_Output3.SetChecked(FALSE);
	m_Output2.SetChecked(FALSE);
	m_Output1.SetChecked(FALSE);
	m_Output4.SetChecked(TRUE);
	UpdateButtons();
}

void CDoubleSidedPrintPostPrint2::UpdateButtons(void)
{
	DWORD dwButtons = PSWIZB_BACK;
	if (m_Output1.GetChecked()
		 || m_Output2.GetChecked()
		 || m_Output3.GetChecked()
		 || m_Output4.GetChecked())
	{
		dwButtons |= PSWIZB_NEXT;
	}
	SetWizardButtons(dwButtons);
}

void CDoubleSidedPrintPostPrint2::UpdateBitmaps(void)
{
	BOOL fVisible2 = ((CDoubleSidedPrintWizard*)GetParent())->m_PostPrint1.m_Output2.GetChecked();
	// Load the bitmaps for the buttons.
	CBitmap* pBitmap = NULL;
	TRY
	{
		pBitmap = new CBitmap;
		pBitmap->LoadBitmap(fVisible2 ? IDB_PRINT14 : IDB_PRINT13);
		m_Output1.SetBitmap(pBitmap, TRUE, FALSE);
		pBitmap = NULL;

		pBitmap = new CBitmap;
		pBitmap->LoadBitmap(fVisible2 ? IDB_PRINT2N4 : IDB_PRINT2N3);
		m_Output2.SetBitmap(pBitmap, TRUE, FALSE);
		pBitmap = NULL;

		pBitmap = new CBitmap;
		pBitmap->LoadBitmap(fVisible2 ? IDB_PRINT14R : IDB_PRINT13R);
		m_Output3.SetBitmap(pBitmap, TRUE, FALSE);
		pBitmap = NULL;

		pBitmap = new CBitmap;
		pBitmap->LoadBitmap(fVisible2 ? IDB_PRINT2R4 : IDB_PRINT2R3);
		m_Output4.SetBitmap(pBitmap, TRUE, FALSE);
		pBitmap = NULL;
	}
	CATCH_ALL(e)
	{
		delete pBitmap;
	}
	END_CATCH_ALL
}

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintFinish property page

CDoubleSidedPrintFinish::CDoubleSidedPrintFinish() :
	CPmwWizardPage(CDoubleSidedPrintFinish::IDD)
{
	//{{AFX_DATA_INIT(CDoubleSidedPrintFinish)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDoubleSidedPrintFinish::~CDoubleSidedPrintFinish()
{
}

void CDoubleSidedPrintFinish::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoubleSidedPrintFinish)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDoubleSidedPrintFinish, CPmwWizardPage)
	//{{AFX_MSG_MAP(CDoubleSidedPrintFinish)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintFinish message handlers

BOOL CDoubleSidedPrintFinish::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	}
	return fResult;
}

