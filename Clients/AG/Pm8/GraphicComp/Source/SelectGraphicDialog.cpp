// ****************************************************************************
//
//  File Name:			SelectGraphicDialog.cpp
//
//  Project:			Renaissance Graphic Imager
//
//  Author:				S. Athanas
//
//  Description:		Definition of the CSelectGraphicDialog class
//
//  Portability:		MFC dependent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Source/SelectGraphicDialog.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#include "SelectGraphicDialog.h"
#include "GraphicCompResource.h"

// Message Map
BEGIN_MESSAGE_MAP( CSelectGraphicDialog, CDialog )
	ON_LBN_DBLCLK( CONTROL_GRAPHIC_LIST, OnGraphicListBoxDoubleClick )
END_MESSAGE_MAP( )

// ****************************************************************************
//
// Function Name:		CSelectGraphicDialog::CSelectGraphicDialog( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
CSelectGraphicDialog::CSelectGraphicDialog( char** pGraphicNames, short numGraphics )
	: CDialog( ),
	  m_nGraphic( 0 ),
	  m_pGraphicNames( pGraphicNames ),
	  m_numGraphics( numGraphics )
	{
	TpsAssert( numGraphics > 0, "No graphics to select" );
	}

// ****************************************************************************
//
// Function Name:		CSelectGraphicDialog::DoDataExchange( )
//
// Description:		Called to transfer data between the MFC dialog object and
//							the Windows dialog.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void CSelectGraphicDialog::DoDataExchange( CDataExchange* pDX )
	{
	// Create the control objects
	DDX_Control( pDX, CONTROL_GRAPHIC_LIST, m_wndGraphicListBox );
	DDX_LBIndex( pDX, CONTROL_GRAPHIC_LIST, m_nGraphic );
	}

// ****************************************************************************
//
// Function Name:		CSelectGraphicDialog::OnInitDialog( )
//
// Description:		Initializes the dialog
//
// Returns:				See CDialog::OnInitDialog
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOL CSelectGraphicDialog::OnInitDialog( )
	{
	// Call the base method
	BOOL fReturn = CDialog::OnInitDialog( );

	// Populate the graphic names listbox
	for( short i = 0; i < m_numGraphics; i++ )
		m_wndGraphicListBox.AddString( m_pGraphicNames[ i ] );

	// Select the first graphic
	m_wndGraphicListBox.SetCurSel( 0 );

	return fReturn;
	}

// ****************************************************************************
//
// Function Name:		CSelectGraphicDialog::OnGraphicListBoxDoubleClick( )
//
// Description:		Called when the user double clicks in the graphic list box.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
afx_msg void CSelectGraphicDialog::OnGraphicListBoxDoubleClick( )
	{
	OnOK( );
	}