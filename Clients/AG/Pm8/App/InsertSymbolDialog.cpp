//****************************************************************************
//
// File Name:  InsertSymbolDialog.cpp
//
// Project:    Renaissance Text Component
//
// Author:     Park Row -this file was yanked directly out of BroderBund's ParkRow 
//								 source code at their request.  The only things that have 
//								 changed are resource names and the file name (formerly InsertSymbol.cpp). -MWH
//
// Description: Implementation of Insert symbol dialog Class
//
// Portability: Windows Specific
//
// Adapted by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/InsertSymbolDialog.cpp                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************


// InsertSymbol.cpp : implementation file
//
#include "stdafx.h"

#include "InsertSymbolDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CBInsertSymbol dialog


CBInsertSymbol::CBInsertSymbol(CWnd* pParent /*=NULL*/, unsigned char c /*32*/, int nID /*IDS_INSERT_SYMBOL*/)
	: CDialog(CBInsertSymbol::IDD, pParent),
	m_curSymbolChar(c),
	m_nTitleID( nID )
{
	//{{AFX_DATA_INIT(CBInsertSymbol)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	if( m_curSymbolChar < 32 )
		m_curSymbolChar = 32;
}


void CBInsertSymbol::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBInsertSymbol)
//	DDX_Control(pDX, CONTROL_STATIC_SYMBOL_SIDEBAR, m_sidebar);
	DDX_Control(pDX, CONTROL_STATIC_SYMBOL_SYMBOLGRID, m_symbolGridCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBInsertSymbol, CDialog)
	//{{AFX_MSG_MAP(CBInsertSymbol)
	ON_BN_CLICKED(CONTROL_BUTTON_SYMBOL_INSERT, OnInsertSymbol)
	ON_WM_DRAWITEM()
	ON_MESSAGE( UM_SYMBOL_CHANGED, OnSymbolChange )
	ON_MESSAGE( UM_SYMBOL_DBLCLICKED, OnInsertSymbol )
//	ON_BN_CLICKED(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBInsertSymbol message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CBInsertSymbol::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//m_sidebarbmp.LoadBitmap( BITMAP_INSERT_SYMBOL_SIDEBAR );
    //m_sidebar.SetBitmap( (HBITMAP)m_sidebarbmp );

	// set up the font for preview
    LOGFONT     logFont;
    memset(&logFont, 0, sizeof(LOGFONT));

    // Should be SYMBOL_CHARSET
    logFont.lfCharSet  = SYMBOL_CHARSET;

    strcpy (logFont.lfFaceName, kSymbolFont );

    GetDlgItem( CONTROL_BUTTON_SYMBOL_PREVIEW )->GetWindowRect( &m_previewRect );
    ScreenToClient( &m_previewRect );

    logFont.lfHeight   = m_previewRect.Height() - 2;
    m_previewFont.CreateFontIndirect( &logFont ); 
    m_fontName = logFont.lfFaceName;

	CString strTitle;
	strTitle.LoadString( m_nTitleID );
	SetWindowText( strTitle );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ****************************************************************************
//
//  Function Name:	CBInsertSymbol::OnInsertSymbol()
//
//  Description:	
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
void CBInsertSymbol::OnInsertSymbol() 
{
    if (m_curSymbolChar != 0)
        EndDialog( IDOK );
    else
        ::MessageBeep((UINT)-1);
}

// ****************************************************************************
//
//  Function Name:	CBInsertSymbol::OnDrawItem
//
//  Description:	
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
void CBInsertSymbol::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    OnSymbolChange( 0, (LPARAM) m_curSymbolChar );
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

//void CBInsertSymbol::OnHelp() 
//{
    //::WinHelp(m_hWnd, "pspw.hlp", HELP_CONTEXT, IDD);
//}

// ****************************************************************************
//
//  Function Name:	CBInsertSymbol::OnSymbolChange
//
//  Description:	
//
//  Returns:		0L	
//
//  Exceptions:		None
//
// ****************************************************************************
//
LRESULT CBInsertSymbol::OnSymbolChange( WPARAM nID, LPARAM lParam )
{
    CBrush  grayBrush( RGB(255,255,255) );
    CBrush *pBrush;

    char buf[] = "\0\0";
 
    CDC * pDC = GetDC();

    pBrush = pDC->SelectObject( &grayBrush );
    pDC->Rectangle( &m_previewRect );
    pDC->SelectObject( pBrush );

	m_curSymbolChar = LOWORD(lParam);
    if (m_curSymbolChar != 0)
    {
        CFont * pOldFont = pDC->SelectObject( &m_previewFont );
        pDC->SetBkMode( TRANSPARENT );
        buf[0] = m_curSymbolChar;
        pDC->DrawText( buf, 1, &m_previewRect, DT_CENTER );
        pDC->SelectObject( pOldFont );
    }

    ReleaseDC( pDC );

	return 0L;

}