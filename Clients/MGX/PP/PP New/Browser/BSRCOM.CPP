// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "id.h"
#include "browser.h"
#include "bsrport.h"
#include "bsrid.h"
#include "bsrtypes.h"
#include "handlebs.h"
#include "bsritems.h"
#include "bsrutil.h"
#include "bsrdlgs.h"
#include "thmbsr.h"
#include "cthmmgr.h"
#include "extbsr.h"
#include "ppafx.h"
#include "bsrcom.h"
#include <afxwin.h>

/***************************Static prototypes******************************/

/***************************Static vars******************************/

/* ********************************** Types ********************************* */

#define	IDC_THUMB				17101
#define	IDC_OPENMODE			17104

BEGIN_MESSAGE_MAP( CMgxFileDialog, CFileDialog )
	ON_COMMAND( IDC_VIEWTHUMBNAILS, OnViewThumbNails )
END_MESSAGE_MAP()

CMgxFileDialog::CMgxFileDialog( BOOL bOpenFileDialog, BOOL bWin95 ) : CFileDialog( bOpenFileDialog )
{

m_Rect.SetRectEmpty( );

if( bWin95 )
	{
	m_ComboRectInit.SetRect( 54, 142, 209, 178 );
	m_CheckRectInit.SetRect( 222, 146, 272, 160 );
	m_StaticRectInit.SetRect( 5, 144, 35, 153 );
	m_RectInit.bottom = 4;
	}
else
	{
	m_ComboRectInit.SetRect( 6, 140, 203, 176 );
	m_CheckRectInit.SetRect( 208, 139, 258, 153 );
	m_StaticRectInit.SetRect( 6, 130, 35, 139 );
	m_RectInit.bottom = 28;
	}

}

BOOL CMgxFileDialog::OnInitDialog()
{
CFont		*pFont;
TCHAR		string[255];

m_bThumbOpen = FALSE;

m_nOpenMode = OM_NORMAL;

m_Rect 			= m_RectInit;
m_ComboRect 	= m_ComboRectInit;
m_CheckRect		= m_CheckRectInit;
m_StaticRect 	= m_StaticRectInit;

MapDialogRect( &m_Rect );
MapDialogRect( &m_ComboRect );
MapDialogRect( &m_CheckRect );
MapDialogRect( &m_StaticRect );

pFont = GetFont( );

GetWindowRect( &m_DialogRect );

m_DialogRect.bottom += m_Rect.bottom;

MoveWindow( &m_DialogRect );
			    
m_ModeCombo.Create( CBS_AUTOHSCROLL | CBS_DROPDOWNLIST | WS_CHILD |
	WS_VISIBLE | WS_VSCROLL | WS_TABSTOP, 
	( RECT & )m_ComboRect, FromHandlePermanent( m_hWnd ), IDC_THUMB + 1 );

m_ModeCombo.SetFont( pFont, TRUE );

m_ModeCombo.ResetContent( );
					 
for( int i = IDS_CDG_OPENFIRST; i <= IDS_CDG_OPENLAST; i++ )				  
	{
	if( AstralStrEx( i, string, sizeof( string ) ) )
		m_ModeCombo.AddString( string );
	}

m_ModeCombo.SetCurSel( 0 );

m_ComboText.Create( _T("Mode"), SS_LEFT | WS_CHILD | WS_VISIBLE,
	( RECT & )m_StaticRect, FromHandlePermanent( m_hWnd ), IDC_THUMB + 2 );

m_ComboText.SetFont( pFont, TRUE );

m_ThumbButton.Create( _T("Thumbnails"), WS_VISIBLE | BS_PUSHBUTTON,  ( RECT & )m_CheckRect,  FromHandlePermanent( m_hWnd ),
	IDC_VIEWTHUMBNAILS );

m_ThumbButton.SetFont( pFont, TRUE );

return( CDialog::OnInitDialog() );
}

BOOL	CMgxFileDialog::OnFileNameOK( )
{

m_nOpenMode =  ( OPEN_MODE )m_ModeCombo.GetCurSel( );

return( CFileDialog::OnFileNameOK( ) );
}
 
void CMgxFileDialog::OnViewThumbNails( )
{

if ( !UpdateData( TRUE ) )
	{
		TRACE0(_T("UpdateData failed during dialog termination.\n"));
		// the UpdateData routine will set focus to correct item
		return;
	}
m_bThumbOpen = m_idThumbDialog;

EndDialog( IDCANCEL );
}
 
