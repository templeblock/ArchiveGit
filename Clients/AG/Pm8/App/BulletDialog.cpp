// BulletDialog.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "BulletDialog.h"
//#include "popup.h"
#include "InsertSymbolDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPBulletNumberedPage property page

IMPLEMENT_DYNCREATE(CPBulletNumberedPage, CPropertyPage)

CPBulletNumberedPage::CPBulletNumberedPage() 
		:	CPropertyPage(CPBulletNumberedPage::IDD),
			m_pStyle( NULL )
{
	//{{AFX_DATA_INIT(CPBulletNumberedPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPBulletNumberedPage::~CPBulletNumberedPage()
{
}

void CPBulletNumberedPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPBulletNumberedPage)
	DDX_Control(pDX, IDC_BULLET_OUTLINE_2, m_NumberedOutlineButton2);
	DDX_Control(pDX, IDC_BULLET_OUTLINE_1, m_NumberedOutlineButton1);
	DDX_Control(pDX, IDC_BULLET_NUMBERED_4, m_NumberedButton4);
	DDX_Control(pDX, IDC_BULLET_NUMBERED_3, m_NumberedButton3);
	DDX_Control(pDX, IDC_BULLET_NUMBERED_2, m_NumberedButton2);
	DDX_Control(pDX, IDC_BULLET_NUMBERED_1, m_NumberedButton1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPBulletNumberedPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPBulletNumberedPage)
	ON_BN_CLICKED(IDC_BULLET_NONE, OnBulletNone)
	ON_BN_DOUBLECLICKED(IDC_BULLET_NONE, OnDoubleclickedBulletNone)
	ON_BN_CLICKED(IDC_BULLET_NUMBERED_1, OnBulletNumbered1)
	ON_BN_DOUBLECLICKED(IDC_BULLET_NUMBERED_1, OnDoubleclickedBulletNumbered1)
	ON_BN_CLICKED(IDC_BULLET_NUMBERED_2, OnBulletNumbered2)
	ON_BN_DOUBLECLICKED(IDC_BULLET_NUMBERED_2, OnDoubleclickedBulletNumbered2)
	ON_BN_CLICKED(IDC_BULLET_NUMBERED_3, OnBulletNumbered3)
	ON_BN_DOUBLECLICKED(IDC_BULLET_NUMBERED_3, OnDoubleclickedBulletNumbered3)
	ON_BN_CLICKED(IDC_BULLET_NUMBERED_4, OnBulletNumbered4)
	ON_BN_DOUBLECLICKED(IDC_BULLET_NUMBERED_4, OnDoubleclickedBulletNumbered4)
	ON_BN_CLICKED(IDC_BULLET_OUTLINE_1, OnBulletOutline1)
	ON_BN_DOUBLECLICKED(IDC_BULLET_OUTLINE_1, OnDoubleclickedBulletOutline1)
	ON_BN_CLICKED(IDC_BULLET_OUTLINE_2, OnBulletOutline2)
	ON_BN_DOUBLECLICKED(IDC_BULLET_OUTLINE_2, OnDoubleclickedBulletOutline2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPBulletSymbolPage property page

IMPLEMENT_DYNCREATE(CPBulletSymbolPage, CPropertyPage)

CPBulletSymbolPage::CPBulletSymbolPage() 
	: CPropertyPage(CPBulletSymbolPage::IDD),
	m_pStyle( NULL )
{
	//{{AFX_DATA_INIT(CPBulletSymbolPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPBulletSymbolPage::~CPBulletSymbolPage()
{
}

void CPBulletSymbolPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPBulletSymbolPage)
	DDX_Control(pDX, IDC_BULLET_SYMBOL_OUTLINE_1, m_SymbolOutlineButton1);
	DDX_Control(pDX, IDC_BULLET_SYMBOL_1, m_SymbolButton1);
	DDX_Control(pDX, IDC_BULLET_SYMBOL_2, m_SymbolButton2);
	DDX_Control(pDX, IDC_BULLET_SYMBOL_3, m_SymbolButton3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPBulletSymbolPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPBulletSymbolPage)
	ON_BN_CLICKED(IDC_BULLET_CUSTOM, OnBulletCustom)
	ON_BN_DOUBLECLICKED(IDC_BULLET_CUSTOM, OnDoubleclickedBulletCustom)
	ON_BN_CLICKED(IDC_BULLET_NONE, OnBulletNone)
	ON_BN_DOUBLECLICKED(IDC_BULLET_NONE, OnDoubleclickedBulletNone)
	ON_BN_CLICKED(IDC_BULLET_SYMBOL_1, OnBulletSymbol1)
	ON_BN_DOUBLECLICKED(IDC_BULLET_SYMBOL_1, OnDoubleclickedBulletSymbol1)
	ON_BN_CLICKED(IDC_BULLET_SYMBOL_2, OnBulletSymbol2)
	ON_BN_DOUBLECLICKED(IDC_BULLET_SYMBOL_2, OnDoubleclickedBulletSymbol2)
	ON_BN_CLICKED(IDC_BULLET_SYMBOL_3, OnBulletSymbol3)
	ON_BN_DOUBLECLICKED(IDC_BULLET_SYMBOL_3, OnDoubleclickedBulletSymbol3)
	ON_BN_CLICKED(IDC_BULLET_SYMBOL_OUTLINE_1, OnBulletSymbolOutline1)
	ON_BN_DOUBLECLICKED(IDC_BULLET_SYMBOL_OUTLINE_1, OnDoubleclickedBulletSymbolOutline1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPBulletPropertySheet message handlers
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CPBulletPropertySheet, CPropertySheet)

CPBulletPropertySheet::CPBulletPropertySheet(CTextStyle* pStyle, 
											 PMGFontServer* pServer, 
											 UINT nIDCaption, 
											 CWnd* pParentWnd, 
											 UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage),
	m_Style( *pStyle )
{
	// add pages
	AddPage(&m_SymbolPage);
	m_SymbolPage.SetStyle( &m_Style );
	m_SymbolPage.SetFontServer( pServer );
	AddPage(&m_NumberedPage);
	m_NumberedPage.SetStyle( &m_Style );
}

CPBulletPropertySheet::CPBulletPropertySheet(CTextStyle* pStyle, 
											 PMGFontServer* pServer,
											 LPCTSTR pszCaption, 
											 CWnd* pParentWnd, 
											 UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage),
	m_Style( *pStyle )
{
	AddPage(&m_SymbolPage);
	m_SymbolPage.SetStyle( &m_Style );
	m_SymbolPage.SetFontServer( pServer );
	AddPage(&m_NumberedPage);
	m_NumberedPage.SetStyle( &m_Style );
}

BOOL CPBulletPropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
	// set the active tab in the dialog
	if( m_Style.BulletType() == BULLET_numbered || m_Style.BulletType() == BULLET_multilevel )
		SetActivePage( &m_NumberedPage );
	else
		SetActivePage( &m_SymbolPage );
	return bResult;
}

CPBulletPropertySheet::~CPBulletPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CPBulletPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPBulletPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPBulletSymbolPage message handlers

BOOL CPBulletSymbolPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	//square bullet button
	m_SymbolButton1.LoadBitmaps( IDB_CIRBULU, IDB_CIRBULD );
	//diamond bullet button
	m_SymbolButton2.LoadBitmaps( IDB_DIABULU, IDB_DIABULD );
	// circle bullet button
	m_SymbolButton3.LoadBitmaps( IDB_SQRBULU, IDB_SQRBULD );
	// outline style symbol bullets
	m_SymbolOutlineButton1.LoadBitmaps( IDB_CIROTBULU, IDB_CIROTBULD );

//	return FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPBulletSymbolPage::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	// assign the bullet character font
	// for now we only use the PressWriter symbol font
	int nFace = typeface_server.find_face(kSymbolFont, FALSE);
	if( nFace != -1 )
	{
		DB_RECORD_NUMBER Face = m_pFontServer->font_face_to_record(nFace);
		m_pStyle->BulletFont(Face);
	}
	else
		m_pStyle->BulletFont( m_pStyle->Font() );

	// set all buttons inactive
	m_SymbolButton1.SetState(FALSE);
	m_SymbolButton2.SetState(FALSE);
	m_SymbolButton3.SetState(FALSE);
	m_SymbolOutlineButton1.SetState(FALSE);

	// depress the appropriate button
	if( m_pStyle->BulletStyle() == BULLET_symbol_mix )
		m_SymbolOutlineButton1.SetState(TRUE);
	else if( m_pStyle->BulletCharacter() == BULLET_circle )
		m_SymbolButton1.SetState(TRUE);			
	else if( m_pStyle->BulletCharacter() == BULLET_square )
		m_SymbolButton3.SetState(TRUE);
	else if( m_pStyle->BulletCharacter() == BULLET_diamond )
			m_SymbolButton2.SetState(TRUE);
	return CPropertyPage::OnSetActive();
}

void CPBulletSymbolPage::OnBulletCustom() 
{
	ASSERT( m_pStyle );	
	CBInsertSymbol dialog( this, m_pStyle->BulletCharacter(), IDS_SELECT_BULLET );
	if( dialog.DoModal() == IDOK )
	{
		m_pStyle->BulletType( BULLET_symbol );
		m_pStyle->BulletCharacter( dialog.GetChar() );
		ASSERT( !strcmp( kSymbolFont, dialog.GetFont() ) );
		
		EndDialog( IDOK );
	}
}

void CPBulletSymbolPage::OnDoubleclickedBulletCustom() 
{
	OnBulletCustom();
}

void CPBulletSymbolPage::OnBulletNone() 
{
	ASSERT( m_pStyle );
	m_pStyle->BulletType( BULLET_none );
	m_pStyle->BulletStyle( BULLET_none );

	m_SymbolButton1.SetState(FALSE);
	m_SymbolButton2.SetState(FALSE);
	m_SymbolButton3.SetState(FALSE);
	m_SymbolOutlineButton1.SetState(FALSE);
}

void CPBulletSymbolPage::OnDoubleclickedBulletNone() 
{
	OnBulletNone();
	
	EndDialog( IDOK );
}

void CPBulletSymbolPage::OnBulletSymbol1() 
{
	ASSERT( m_pStyle );
	m_pStyle->BulletType( BULLET_symbol );
	m_pStyle->BulletStyle( BULLET_circle );

	m_SymbolButton1.SetState(TRUE);
	m_SymbolButton2.SetState(FALSE);
	m_SymbolButton3.SetState(FALSE);
	m_SymbolOutlineButton1.SetState(FALSE);
}

void CPBulletSymbolPage::OnDoubleclickedBulletSymbol1() 
{
	OnBulletSymbol1();
	
	EndDialog( IDOK );
}

void CPBulletSymbolPage::OnBulletSymbol2() 
{
	m_pStyle->BulletType( BULLET_symbol );
	m_pStyle->BulletStyle( BULLET_diamond );

	m_SymbolButton1.SetState(FALSE);
	m_SymbolButton2.SetState(TRUE);
	m_SymbolButton3.SetState(FALSE);
	m_SymbolOutlineButton1.SetState(FALSE);
}

void CPBulletSymbolPage::OnDoubleclickedBulletSymbol2() 
{
	OnBulletSymbol2();
	
	EndDialog( IDOK );
}

void CPBulletSymbolPage::OnBulletSymbol3() 
{
	m_pStyle->BulletType( BULLET_symbol );
	m_pStyle->BulletStyle( BULLET_square );	

	m_SymbolButton1.SetState(FALSE);
	m_SymbolButton2.SetState(FALSE);
	m_SymbolButton3.SetState(TRUE);
	m_SymbolOutlineButton1.SetState(FALSE);
}

void CPBulletSymbolPage::OnDoubleclickedBulletSymbol3() 
{
	OnBulletSymbol3();
	
	EndDialog( IDOK );
}

void CPBulletSymbolPage::OnBulletSymbolOutline1() 
{
	m_pStyle->BulletType( BULLET_symbol );
	m_pStyle->BulletStyle( BULLET_symbol_mix );

	m_SymbolButton1.SetState(FALSE);
	m_SymbolButton2.SetState(FALSE);
	m_SymbolButton3.SetState(FALSE);
	m_SymbolOutlineButton1.SetState(TRUE);
}

void CPBulletSymbolPage::OnDoubleclickedBulletSymbolOutline1() 
{
	OnBulletSymbolOutline1();
	
	EndDialog( IDOK );
}

void CPBulletSymbolPage::OnOK() 
{
	CPropertyPage::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CPBulletNumberedPage message handlers

BOOL CPBulletNumberedPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here
	m_NumberedButton1.LoadBitmaps( IDB_1NUMBU, IDB_1NUMBD );
	m_NumberedButton2.LoadBitmaps( IDB_4NUMBU, IDB_4NUMBD );
	m_NumberedButton3.LoadBitmaps( IDB_2NUMBU, IDB_2NUMBD );
	m_NumberedButton4.LoadBitmaps( IDB_5NUMBU, IDB_5NUMBD );
	m_NumberedOutlineButton1.LoadBitmaps( IDB_3NUMBU, IDB_3NUMBD );
	m_NumberedOutlineButton2.LoadBitmaps( IDB_6NUMBU, IDB_6NUMBD );

//	return FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPBulletNumberedPage::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	// assign the font for numbered lists. It is the same as the text font.
	m_pStyle->BulletFont( m_pStyle->Font() );
	
	// set all controls to unselected
	m_NumberedButton1.SetState( FALSE );
	m_NumberedButton2.SetState( FALSE );
	m_NumberedButton3.SetState( FALSE );
	m_NumberedButton4.SetState( FALSE );
	m_NumberedOutlineButton1.SetState( FALSE );
	m_NumberedOutlineButton2.SetState( FALSE );

	// depress the appropriate control
	if( m_pStyle->BulletType() == BULLET_numbered )
	{	
		if( m_pStyle->BulletStyle() == BULLET_arabic )
			m_NumberedButton1.SetState(TRUE);
		else if( m_pStyle->BulletStyle() == BULLET_arabic_paren )
			m_NumberedButton2.SetState(TRUE);
		else if( m_pStyle->BulletStyle() == BULLET_letter )
			m_NumberedButton3.SetState(TRUE);
		else if( m_pStyle->BulletStyle() == BULLET_letter_paren )
			m_NumberedButton4.SetState(TRUE);
	}
	else if( m_pStyle->BulletType() == BULLET_multilevel )
	{
		if( m_pStyle->BulletStyle() == BULLET_arabic )
			m_NumberedOutlineButton1.SetState(TRUE);
		else if( m_pStyle->BulletStyle() == BULLET_roman_mix )
			m_NumberedOutlineButton2.SetState(TRUE);
	}
	
	return CPropertyPage::OnSetActive();
}

void CPBulletNumberedPage::OnBulletNone() 
{
	ASSERT( m_pStyle );
	m_pStyle->BulletType( BULLET_none );
	m_pStyle->BulletStyle( BULLET_none );

	m_NumberedButton1.SetState( FALSE );
	m_NumberedButton2.SetState( FALSE );
	m_NumberedButton3.SetState( FALSE );
	m_NumberedButton4.SetState( FALSE );
	m_NumberedOutlineButton1.SetState( FALSE );
	m_NumberedOutlineButton2.SetState( FALSE );
}

void CPBulletNumberedPage::OnDoubleclickedBulletNone() 
{
	OnBulletNone();
	
	EndDialog( IDOK );

}

void CPBulletNumberedPage::OnBulletNumbered1() 
{
	ASSERT( m_pStyle );
	m_pStyle->BulletType( BULLET_numbered );
	m_pStyle->BulletStyle( BULLET_arabic );

	m_NumberedButton1.SetState( TRUE );
	m_NumberedButton2.SetState( FALSE );
	m_NumberedButton3.SetState( FALSE );
	m_NumberedButton4.SetState( FALSE );
	m_NumberedOutlineButton1.SetState( FALSE );
	m_NumberedOutlineButton2.SetState( FALSE );
}

void CPBulletNumberedPage::OnDoubleclickedBulletNumbered1() 
{
	OnBulletNumbered1();
	
	EndDialog( IDOK );
}

void CPBulletNumberedPage::OnBulletNumbered2() 
{
	ASSERT( m_pStyle );
	m_pStyle->BulletType( BULLET_numbered );
	m_pStyle->BulletStyle( BULLET_arabic_paren );

	m_NumberedButton1.SetState( FALSE );
	m_NumberedButton2.SetState( TRUE );
	m_NumberedButton3.SetState( FALSE );
	m_NumberedButton4.SetState( FALSE );
	m_NumberedOutlineButton1.SetState( FALSE );
	m_NumberedOutlineButton2.SetState( FALSE );
}

void CPBulletNumberedPage::OnDoubleclickedBulletNumbered2() 
{
	OnBulletNumbered2();
	
	EndDialog( IDOK );
}

void CPBulletNumberedPage::OnBulletNumbered3() 
{
	ASSERT( m_pStyle );
	m_pStyle->BulletType( BULLET_numbered );
	m_pStyle->BulletStyle( BULLET_letter );

	m_NumberedButton1.SetState( FALSE );
	m_NumberedButton2.SetState( FALSE );
	m_NumberedButton3.SetState( TRUE );
	m_NumberedButton4.SetState( FALSE );
	m_NumberedOutlineButton1.SetState( FALSE );
	m_NumberedOutlineButton2.SetState( FALSE );
}

void CPBulletNumberedPage::OnDoubleclickedBulletNumbered3() 
{
	OnBulletNumbered3();
	
	EndDialog( IDOK );	
}

void CPBulletNumberedPage::OnBulletNumbered4() 
{
	ASSERT( m_pStyle );
	m_pStyle->BulletType( BULLET_numbered );
	m_pStyle->BulletStyle( BULLET_letter_paren );

	m_NumberedButton1.SetState( FALSE );
	m_NumberedButton2.SetState( FALSE );
	m_NumberedButton3.SetState( FALSE );
	m_NumberedButton4.SetState( TRUE );
	m_NumberedOutlineButton1.SetState( FALSE );
	m_NumberedOutlineButton2.SetState( FALSE );
}

void CPBulletNumberedPage::OnDoubleclickedBulletNumbered4() 
{
	OnBulletNumbered4();
	
	EndDialog( IDOK );
}

void CPBulletNumberedPage::OnBulletOutline1() 
{
	ASSERT( m_pStyle );
	m_pStyle->BulletType( BULLET_multilevel );
	m_pStyle->BulletStyle( BULLET_arabic );

	m_NumberedButton1.SetState( FALSE );
	m_NumberedButton2.SetState( FALSE );
	m_NumberedButton3.SetState( FALSE );
	m_NumberedButton4.SetState( FALSE );
	m_NumberedOutlineButton1.SetState( TRUE );
	m_NumberedOutlineButton2.SetState( FALSE );
}

void CPBulletNumberedPage::OnDoubleclickedBulletOutline1() 
{
	OnBulletOutline1();
	
	EndDialog( IDOK );	
}

void CPBulletNumberedPage::OnBulletOutline2() 
{
	ASSERT( m_pStyle );
	m_pStyle->BulletType( BULLET_multilevel );
	m_pStyle->BulletStyle( BULLET_roman_mix );	

	m_NumberedButton1.SetState( FALSE );
	m_NumberedButton2.SetState( FALSE );
	m_NumberedButton3.SetState( FALSE );
	m_NumberedButton4.SetState( FALSE );
	m_NumberedOutlineButton1.SetState( FALSE );
	m_NumberedOutlineButton2.SetState( TRUE );
}

void CPBulletNumberedPage::OnDoubleclickedBulletOutline2() 
{
	OnBulletOutline2();
	
	EndDialog( IDOK );	
}


void CPBulletNumberedPage::OnOK() 
{
	CPropertyPage::OnOK();
}



