//****************************************************************************
//
// File Name:		AddPageWiz.cpp
//
// Project:			Renaissance Framework Component
//
// Author:			Lisa Wu
//
// Description:		Add Page Wizard   
//
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "AddPageWiz.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef _FRAMEWORKRESOURCEIDS_H_
#include "FrameworkResourceIDs.h"
#endif

#ifndef	_WINDOWS
	#error	This file must be compiled only on Windows
#endif	//	_WINDOWS

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// AddPageWizard

IMPLEMENT_DYNAMIC(AddPageWizard, CPropertySheet)

// ****************************************************************************
//
//  Function Name:	AddPageWizard::AddPageWizard
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageWizard::AddPageWizard(int applType, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage),
	m_applType		(applType),
	m_curPageType	(kAddBlankPage)
{
	InitPages( );
}

// ****************************************************************************
//
//  Function Name:	AddPageWizard::~AddPageWizard
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageWizard::~AddPageWizard( )
{
}

// ****************************************************************************
//
//  Function Name:	AddPageWizard::InitPages
//
//  Description:		Prep the sheet for use.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPageWizard::InitPages( )
{
	//	Add our pages.
	AddPage( &m_PgLayoutSelPage );
	AddPage( &m_ConfPgLayoutPage );
	AddPage( &m_SelPgLayoutPage );
	AddPage( &m_SelQSLPage );
	AddPage( &m_PgNamePage );

	//	Display in wizard mode.
	SetWizardMode( );
}

// ****************************************************************************
//
//  Function Name:	AddPageWizard::AddPage
//
//  Description:		Add the given page to the sheet and set ourselves as its parent.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPageWizard::AddPage( AddPgWizPage* pPage )
{
	pPage->SetParentSheet( this );
	CPropertySheet::AddPage( pPage );
}


// ****************************************************************************
//
//  Function Name:	AddPageWizard::OnInitDialog
//
//  Description:		Prep the dialog.
//
//  Returns:			TRUE: we set the input focus.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL AddPageWizard::OnInitDialog( ) 
{
	CPropertySheet::OnInitDialog( );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ****************************************************************************
//
//  Function Name:	AddPageWizard::UpdateWizBtns
//
//  Description:	
//
//  Returns:		none 
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPageWizard::UpdateWizBtns( BOOL bFirstPage, BOOL bDone )
{ 
	GetDlgItem( ID_WIZBACK )->EnableWindow( !bFirstPage );

	CWnd* pNext;
	CWnd* pFinish;

	pNext = GetDlgItem( ID_WIZNEXT );
	pFinish = GetDlgItem(ID_WIZFINISH);
	ASSERT(pNext || pFinish);

	if (bDone)
	{
		CString strBtnName;
		strBtnName.LoadString( STRING_DONE );
		pFinish->SetWindowText(strBtnName);
		pFinish->ShowWindow(TRUE);
		
		pNext->ShowWindow(FALSE);
	}
	else
	{ 
		pNext->ShowWindow(TRUE);
		pFinish->ShowWindow(FALSE);
	}
}

/*

// ****************************************************************************
//
//  Function Name:	AddPageWizard::GetUpdatedPortraitPrintableArea
//
//  Description:		Return the updated printable area specified by the user on page three.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect AddPageWizard::GetUpdatedPortraitPrintableArea( )
{
	//	Prep the printer with the appropriate orientation, get the new printable area.
	m_PageTwo.SetOrientation( RPrinter::kPortrait );
	return m_PageThree.GetUpdatedPrintableArea( );
}

// ****************************************************************************
//
//  Function Name:	AddPageWizard::GetUpdatedLandscapePrintableArea
//
//  Description:		Return the updated printable area specified by the user on page four.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect AddPageWizard::GetUpdatedLandscapePrintableArea( )
{
	//	Prep the printer with the appropriate orientation, get the new printable area.
	m_PageTwo.SetOrientation( RPrinter::kLandscape );
	return m_PageFour.GetUpdatedPrintableArea( );
}

// ****************************************************************************
//
//  Function Name:	AddPageWizard::GetUpdatedLandscapePrintableArea
//
//  Description:		Return the updated printable area specified by the user on page four.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPageWizard::SetPrinterOrientation( RPrinter::EOrientation orientation )
{
	m_PageTwo.SetOrientation( orientation);
}
*/

BEGIN_MESSAGE_MAP(AddPageWizard, CPropertySheet)
	//{{AFX_MSG_MAP(AddPageWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )


/////////////////////////////////////////////////////////////////////////////
// AddPgWizPage property page

IMPLEMENT_DYNCREATE(AddPgWizPage, CPropertyPage)


// ****************************************************************************
//
//  Function Name:	AddPgWizPage::AddPgWizPage
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPgWizPage::AddPgWizPage(  )
 : IDD( 0 ), m_pParentSheet( NULL ), CPropertyPage( )
{
}

// ****************************************************************************
//
//  Function Name:	AddPgWizPage::AddPgWizPage
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPgWizPage::AddPgWizPage( int idd )
 : IDD( idd ), m_pParentSheet( NULL ), CPropertyPage(idd)
{
}

// ****************************************************************************
//
//  Function Name:	AddPgWizPage::~AddPgWizPage
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPgWizPage::~AddPgWizPage( )
{
}


// ****************************************************************************
//
//  Function Name:	AddPgWizPage::OnCancel
//
//  Description:	
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPgWizPage::OnCancel( ) 
{
	CPropertyPage::OnCancel( );
}

// ****************************************************************************
//
//  Function Name:	AddPgWizPage::SetParentSheet
//
//  Description:		Use the given parent sheet.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPgWizPage::SetParentSheet( AddPageWizard* pParent ) 
{
	m_pParentSheet = pParent;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// AddPageLayoutSel property page

IMPLEMENT_DYNCREATE(AddPageLayoutSel, AddPgWizPage)

// ****************************************************************************
//
//  Function Name:	AddPageLayoutSel::AddPageLayoutSel
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageLayoutSel::AddPageLayoutSel( )
 : AddPgWizPage(DIALOG_ADDPAGE_TYPE)
{
	//{{AFX_DATA_INIT(AddPageLayoutSel)
	//}}AFX_DATA_INIT

}

// ****************************************************************************
//
//  Function Name:	AddPageLayoutSel::~AddPageLayoutSel
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageLayoutSel::~AddPageLayoutSel( )
{
}

// ****************************************************************************
//
//  Function Name:	AddPageLayoutSel::DoDataExchange
//
//  Description:		Transfer our dialog data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPageLayoutSel::DoDataExchange(CDataExchange* pDX)
{
	AddPgWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddPageLayoutSel)
	DDX_Radio(pDX, IDC_BLANK_RADIO, m_eAddPageType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddPageLayoutSel, AddPgWizPage)
	//{{AFX_MSG_MAP(AddPageLayoutSel)
	ON_BN_CLICKED(	IDC_BLANK_RADIO,	OnRadioBlankPage	)
	ON_BN_CLICKED(	IDC_DUPEDOC_RADIO,	OnRadioDupeDoc		)
	ON_BN_CLICKED(	IDC_QSL_RADIO,		OnRadioQSL			)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

/////////////////////////////////////////////////////////////////////////////
// AddPageLayoutSel message handlers

// ****************************************************************************
//
//  Function Name:	AddPageLayoutSel::OnInitDialog
//
//  Description:		Prep the dialog.
//
//  Returns:			TRUE: we set the input focus.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL AddPageLayoutSel::OnInitDialog( ) 
{

	AddPgWizPage::OnInitDialog( );

	switch (m_pParentSheet->GetCurPageType())
	{
		case	kAddBlankPage:
			OnRadioBlankPage();
			break;

		case	kAddDupeDoc:
			OnRadioDupeDoc();
			break;
		
		case	kAddQSL:
			OnRadioQSL();
			break;
	}
	
	// send member variables to dialog
	UpdateData (FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ****************************************************************************
//
//  Function Name:	AddPageLayoutSel::OnSetActive
//
//  Description:		Disable the back button since can't use it.
//
//  Returns:			TRUE: continue
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL AddPageLayoutSel::OnSetActive( ) 
{
	m_pParentSheet->UpdateWizBtns(TRUE, FALSE);

	return AddPgWizPage::OnSetActive( );
}

// ****************************************************************************
//
//  Function Name:	AddPageLayoutSel::OnRadioBlankPage
//					AddPageLayoutSel::OnRadioDupeDoc
//					AddPageLayoutSel::OnRadioQSL
//
//  Description:	set the member variable
//
//  Returns:		none
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPageLayoutSel::OnRadioBlankPage() 
{
	m_eAddPageType = kAddBlankPage;
}
void AddPageLayoutSel::OnRadioDupeDoc() 
{
	m_eAddPageType = kAddDupeDoc;
}
void AddPageLayoutSel::OnRadioQSL() 
{
	m_eAddPageType = kAddQSL;
}

// ****************************************************************************
//
//  Function Name:	AddPageLayoutSel::OnWizardNext
//
//  Description:	handle the next page to go
//
//  Returns:		Cancel or proceed code.
//
//  Exceptions:		None
//
// ****************************************************************************
LRESULT AddPageLayoutSel::OnWizardNext( ) 
{
	// tell the parent of the current radio button set
	m_pParentSheet->SetCurPageType(m_eAddPageType);

	// set the next page to go to
	switch (m_eAddPageType)
	{
		case	kAddBlankPage:
			return DIALOG_ADDPAGE_CONFIRM;

		case	kAddDupeDoc:
			return DIALOG_ADDPAGE_DUPEDOC;

		case	kAddQSL:
			return DIALOG_ADDPAGE_QSL;

		default:
			return -1; // do cancel for now
	}

}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// AddPageConfLayout property page

IMPLEMENT_DYNCREATE(AddPageConfLayout, AddPgWizPage)

// ****************************************************************************
//
//  Function Name:	AddPageConfLayout::AddPageConfLayout
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageConfLayout::AddPageConfLayout( )
	: AddPgWizPage(DIALOG_ADDPAGE_CONFIRM)
{
	//{{AFX_DATA_INIT(AddPageConfLayout)
	m_iNumPage = kMinNumPages;
	//}}AFX_DATA_INIT

}

// ****************************************************************************
//
//  Function Name:	AddPageConfLayout::~AddPageConfLayout
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageConfLayout::~AddPageConfLayout( )
{
}

// ****************************************************************************
//
//  Function Name:	AddPageConfLayout::DoDataExchange
//
//  Description:		Transfer our dialog data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPageConfLayout::DoDataExchange(CDataExchange* pDX)
{
	AddPgWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddPageConfLayout)
	DDX_Control(pDX, CONTROL_ADDPAGE_CONF_STRING4, m_strNameStatic);
	DDX_Control(pDX, IDC_CONFPAGENAME_EDIT, m_ctlPageName);

	DDX_Control(pDX, CONTROL_ADDPAGE_CONF_STRING3, m_strNumPageStatic);
	DDX_Control(pDX, IDC_CONF_EDIT_PAGE, m_ctlNumPage_Edit);
	DDX_Control(pDX, IDC_CONF_SPIN_PAGE, m_ctlNumPage_Spin);
	//}}AFX_DATA_MAP


	if ( pDX->m_bSaveAndValidate != FALSE )
	{
		// the edit control may contain a comma so have to remove it manually...
		CString szText;

		// save into CString
		DDX_Text( pDX, IDC_CONF_EDIT_PAGE, szText );

		CString szCommaFreeText;
		TCHAR tComma[2];
		
		if ( ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, (LPTSTR) &tComma, 2 ) != 0 )
		{
			TCHAR tItem;
			for (int n = 0; n < szText.GetLength(); n++ )
			{
				tItem = szText.GetAt(n);
				if ( tItem != tComma[0] )
					szCommaFreeText+= tItem;
			}

			m_iNumPage = atoi( szCommaFreeText );
		}
		else
			m_iNumPage = 1;
	}
}


BEGIN_MESSAGE_MAP(AddPageConfLayout, AddPgWizPage)
	//{{AFX_MSG_MAP(AddPageConfLayout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

/////////////////////////////////////////////////////////////////////////////
// AddPageLayoutSel message handlers

// ****************************************************************************
//
//  Function Name:	AddPageConfLayout::OnInitDialog
//
//  Description:		Prep the dialog.
//
//  Returns:			TRUE: we set the input focus.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL AddPageConfLayout::OnInitDialog( ) 
{

	AddPgWizPage::OnInitDialog( );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ****************************************************************************
//
//  Function Name:	AddPageConfLayout::OnSetActive
//
//  Description:	
//
//  Returns:			TRUE: continue
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL AddPageConfLayout::OnSetActive( ) 
{
	BOOL bNextIsDone = m_pParentSheet->GetApplType() == kPressWriterApplType;
	if ( bNextIsDone )
	{
		m_strNumPageStatic.ShowWindow(TRUE);
		m_ctlNumPage_Edit.ShowWindow(TRUE);
		m_ctlNumPage_Spin.ShowWindow(TRUE);

		// establish range and "position" for spin control
		m_ctlNumPage_Spin.SetRange (kMinNumPages, kMaxNumPages);
		m_ctlNumPage_Spin.SetPos (m_iNumPage);
	}
	else
	{
		// PS and PSWeb, show these two controls
		m_strNameStatic.ShowWindow(TRUE);
		m_ctlPageName.ShowWindow(TRUE);
	}

	m_pParentSheet->UpdateWizBtns(FALSE, bNextIsDone);

	return AddPgWizPage::OnSetActive( );
}

// ****************************************************************************
//
//  Function Name:	AddPageLayoutSel::OnWizardBack
//
//  Description:	handle going back
//
//  Returns:		Cancel or proceed code.
//
//  Exceptions:		None
//
// ****************************************************************************
LRESULT AddPageConfLayout::OnWizardBack( ) 
{
	return DIALOG_ADDPAGE_TYPE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// AddPageDupeLayout property page

IMPLEMENT_DYNCREATE(AddPageDupeLayout, AddPgWizPage)

// ****************************************************************************
//
//  Function Name:	AddPageDupeLayout::AddPageDupeLayout
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageDupeLayout::AddPageDupeLayout( )
 : AddPgWizPage(DIALOG_ADDPAGE_DUPEDOC)
{
	//{{AFX_DATA_INIT(AddPageDupeLayout)
	m_iNumOfPages = kMinNumPages;
	m_iPageNum = 1;				// get this from the document later
	//}}AFX_DATA_INIT

}

// ****************************************************************************
//
//  Function Name:	AddPageDupeLayout::~AddPageDupeLayout
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageDupeLayout::~AddPageDupeLayout( )
{
}

// ****************************************************************************
//
//  Function Name:	AddPageDupeLayout::DoDataExchange
//
//  Description:		Transfer our dialog data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPageDupeLayout::DoDataExchange(CDataExchange* pDX)
{
	AddPgWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddPageDupeLayout)
	DDX_Control(pDX, CONTROL_ADDPAGE_DUPE_STRING3, m_strPageNameStatic);
	DDX_Control(pDX, IDC_PAGENAME_COMBO, m_ctlPageNameCombo);

	DDX_Control(pDX, CONTROL_ADDPAGE_DUPE_STRING4, m_strPageNumStatic);
	DDX_Control(pDX, IDC_EDIT_PAGENUM, m_ctlPageNum_Edit);
	DDX_Control(pDX, IDC_SPIN_PAGENUM, m_ctlPageNum_Spin);

	DDX_Control(pDX, CONTROL_ADDPAGE_DUPE_STRING5, m_strNumOfPagesStatic);
	DDX_Control(pDX, IDC_DUPEDOC_EDIT_NUMPAGES, m_ctlNumOfPages_Edit);
	DDX_Control(pDX, IDC_DUPEDOC_SPIN_NUMPAGES, m_ctlNumOfPages_Spin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddPageDupeLayout, AddPgWizPage)
	//{{AFX_MSG_MAP(AddPageDupeLayout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

/////////////////////////////////////////////////////////////////////////////
// AddPageDupeLayout message handlers

// ****************************************************************************
//
//  Function Name:	AddPageDupeLayout::OnInitDialog
//
//  Description:		Prep the dialog.
//
//  Returns:			TRUE: we set the input focus.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL AddPageDupeLayout::OnInitDialog( ) 
{

	AddPgWizPage::OnInitDialog( );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ****************************************************************************
//
//  Function Name:	AddPageDupeLayout::OnSetActive
//
//  Description:		Disable the back button since can't use it.
//
//  Returns:			TRUE: continue
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL AddPageDupeLayout::OnSetActive( ) 
{
	
	BOOL bNextIsDone = m_pParentSheet->GetApplType() == kPressWriterApplType;
	if ( bNextIsDone )
	{
		m_strPageNumStatic.ShowWindow(TRUE);
		m_ctlPageNum_Edit.ShowWindow(TRUE);
		m_ctlPageNum_Spin.ShowWindow(TRUE);

		// establish range and "position" for spin control
		m_ctlPageNum_Spin.SetRange (kMinNumPages, kMaxNumPages);	// gotta get this from doc later
		m_ctlPageNum_Spin.SetPos (m_iPageNum);

		m_strNumOfPagesStatic.ShowWindow(TRUE);
		m_ctlNumOfPages_Edit.ShowWindow(TRUE);
		m_ctlNumOfPages_Spin.ShowWindow(TRUE);
		
		// establish range and "position" for spin control
		m_ctlNumOfPages_Spin.SetRange (kMinNumPages, kMaxNumPages);
		m_ctlNumOfPages_Spin.SetPos (m_iNumOfPages);
	}
	else
	{
		m_strPageNameStatic.ShowWindow(TRUE);
		m_ctlPageNameCombo.ShowWindow(TRUE);
	}

	m_pParentSheet->UpdateWizBtns(FALSE, bNextIsDone);

	return AddPgWizPage::OnSetActive( );
}
// ****************************************************************************
//
//  Function Name:	AddPageDupeLayout::OnWizardNext
//
//  Description:	handle going back
//
//  Returns:		Cancel or proceed code.
//
//  Exceptions:		None
//
// ****************************************************************************
LRESULT AddPageDupeLayout::OnWizardNext( ) 
{
	return DIALOG_ADDPAGE_PAGENAME;
}

// ****************************************************************************
//
//  Function Name:	AddPageDupeLayout::OnWizardBack
//
//  Description:	handle going back
//
//  Returns:		Cancel or proceed code.
//
//  Exceptions:		None
//
// ****************************************************************************
LRESULT AddPageDupeLayout::OnWizardBack( ) 
{
	return DIALOG_ADDPAGE_TYPE;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// AddPageSelQSL property page

IMPLEMENT_DYNCREATE(AddPageSelQSL, AddPgWizPage)

// ****************************************************************************
//
//  Function Name:	AddPageSelQSL::AddPageSelQSL
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageSelQSL::AddPageSelQSL( )
 : AddPgWizPage(DIALOG_ADDPAGE_QSL)
{
	//{{AFX_DATA_INIT(AddPageSelQSL)
	//}}AFX_DATA_INIT

}

// ****************************************************************************
//
//  Function Name:	AddPageSelQSL::~AddPageSelQSL
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPageSelQSL::~AddPageSelQSL( )
{
}

// ****************************************************************************
//
//  Function Name:	AddPageSelQSL::DoDataExchange
//
//  Description:		Transfer our dialog data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPageSelQSL::DoDataExchange(CDataExchange* pDX)
{
	AddPgWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddPageSelQSL)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddPageSelQSL, AddPgWizPage)
	//{{AFX_MSG_MAP(AddPageSelQSL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

/////////////////////////////////////////////////////////////////////////////
// AddPageSelQSL message handlers

// ****************************************************************************
//
//  Function Name:	AddPageSelQSL::OnInitDialog
//
//  Description:		Prep the dialog.
//
//  Returns:			TRUE: we set the input focus.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL AddPageSelQSL::OnInitDialog( ) 
{

	AddPgWizPage::OnInitDialog( );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ****************************************************************************
//
//  Function Name:	AddPageSelQSL::OnSetActive
//
//  Description:		Disable the back button since can't use it.
//
//  Returns:			TRUE: continue
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL AddPageSelQSL::OnSetActive( ) 
{
	m_pParentSheet->UpdateWizBtns(FALSE, (m_pParentSheet->GetApplType() == kPressWriterApplType));

	return AddPgWizPage::OnSetActive( );
}

// ****************************************************************************
//
//  Function Name:	AddPageSelQSL::OnWizardBack
//
//  Description:	handle going back
//
//  Returns:		Cancel or proceed code.
//
//  Exceptions:		None
//
// ****************************************************************************
LRESULT AddPageSelQSL::OnWizardBack( ) 
{
	return DIALOG_ADDPAGE_TYPE;
}
// ****************************************************************************
//
//  Function Name:	AddPageSelQSL::OnWizardNext
//
//  Description:	handle going next
//
//  Returns:		Cancel or proceed code.
//
//  Exceptions:		None
//
// ****************************************************************************
LRESULT AddPageSelQSL::OnWizardNext( ) 
{
	return DIALOG_ADDPAGE_PAGENAME;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// AddPagePgName property page

IMPLEMENT_DYNCREATE(AddPagePgName, AddPgWizPage)

// ****************************************************************************
//
//  Function Name:	AddPagePgName::AddPagePgName
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPagePgName::AddPagePgName( )
 : AddPgWizPage(DIALOG_ADDPAGE_PAGENAME)
{
	//{{AFX_DATA_INIT(AddPagePgName)
	//}}AFX_DATA_INIT

}

// ****************************************************************************
//
//  Function Name:	AddPagePgName::~AddPagePgName
//
//  Description:		dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
AddPagePgName::~AddPagePgName( )
{
}

// ****************************************************************************
//
//  Function Name:	AddPagePgName::DoDataExchange
//
//  Description:		Transfer our dialog data.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void AddPagePgName::DoDataExchange(CDataExchange* pDX)
{
	AddPgWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddPagePgName)
	DDX_Control(pDX, IDC_PAGENAME_EDIT, m_ctlPageName_Edit);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddPagePgName, AddPgWizPage)
	//{{AFX_MSG_MAP(AddPagePgName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP( )

/////////////////////////////////////////////////////////////////////////////
// AddPagePgName message handlers

// ****************************************************************************
//
//  Function Name:	AddPagePgName::OnInitDialog
//
//  Description:		Prep the dialog.
//
//  Returns:			TRUE: we set the input focus.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL AddPagePgName::OnInitDialog( ) 
{

	AddPgWizPage::OnInitDialog( );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// ****************************************************************************
//
//  Function Name:	AddPagePgName::OnSetActive
//
//  Description:		Disable the back button since can't use it.
//
//  Returns:			TRUE: continue
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL AddPagePgName::OnSetActive( ) 
{
	m_pParentSheet->UpdateWizBtns(FALSE, TRUE);

	return AddPgWizPage::OnSetActive( );
}

// ****************************************************************************
//
//  Function Name:	AddPagePgName::OnWizardBack
//
//  Description:	handle going back
//
//  Returns:		Cancel or proceed code.
//
//  Exceptions:		None
//
// ****************************************************************************
LRESULT AddPagePgName::OnWizardBack( ) 
{
	// set the back page to go to
	switch (m_pParentSheet->GetCurPageType())
	{
		case	kAddBlankPage:
			return DIALOG_ADDPAGE_CONFIRM;

		case	kAddDupeDoc:
			return DIALOG_ADDPAGE_DUPEDOC;

		case	kAddQSL:
			return DIALOG_ADDPAGE_QSL;

		default:
			return -1; // do cancel for now
	}
}

