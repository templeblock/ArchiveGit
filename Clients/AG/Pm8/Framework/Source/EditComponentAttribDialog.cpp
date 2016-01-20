// ****************************************************************************
//
//  File Name:			EditComponentAttributesDialog.cpp
//
//  Project:			Renaissance Graphic Imager
//
//  Author:				S. Athanas
//
//  Description:		Definition of the REditComponentAttributesDialog class
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
//  $Logfile:: /PM8/Framework/Source/EditComponentAttribDialog.cpp            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "EditComponentAttribDialog.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIDs.h"
#include "ComponentAttributes.h"

// ****************************************************************************
//
// Function Name:		REditComponentAttributesDialog::REditComponentAttributesDialog( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
REditComponentAttributesDialog::REditComponentAttributesDialog( const RComponentAttributes& componentAttributes )
	: CDialog( DIALOG_EDIT_COMPONENT_ATTRIBUTES ),
	  m_fSelectable( componentAttributes.IsSelectable( ) ),
	  m_fMovable( componentAttributes.IsMovable( FALSE ) ),
	  m_fResizable( componentAttributes.IsResizable( FALSE ) ),
	  m_fRotatable( componentAttributes.IsRotatable( FALSE ) ),
	  m_ZLayer( componentAttributes.GetZLayer( ) )
	{
	;
	}

// ****************************************************************************
//
// Function Name:		REditComponentAttributesDialog::DoDataExchange( )
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
void REditComponentAttributesDialog::DoDataExchange( CDataExchange* pDX )
	{
	DDX_Check( pDX, CONTROL_SELECTABLE, m_fSelectable );
	DDX_Check( pDX, CONTROL_RESIZABLE, m_fResizable );
	DDX_Check( pDX, CONTROL_ROTATABLE, m_fRotatable );
	DDX_Check( pDX, CONTROL_MOVABLE, m_fMovable );
	DDX_Text( pDX, CONTROL_ZLAYER, m_ZLayer ); 
	}

// ****************************************************************************
//
// Function Name:		REditComponentAttributesDialog::DoDataExchange( )
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
void REditComponentAttributesDialog::FillInComponentAttributes( RComponentAttributes& componentAttributes ) const
	{
	componentAttributes.SetSelectable( static_cast<BOOLEAN>( m_fSelectable ) );
	componentAttributes.SetMovable( static_cast<BOOLEAN>( m_fMovable ) );
	componentAttributes.SetResizable( static_cast<BOOLEAN>( m_fResizable ) );
	componentAttributes.SetRotatable( static_cast<BOOLEAN>( m_fRotatable ) );
	componentAttributes.SetZLayer( m_ZLayer );
	}
