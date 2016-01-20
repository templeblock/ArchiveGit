// ****************************************************************************
//
//  File Name:			TintSettings.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RTintSettings & RTintSettingsInterfaceImp Class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/TintSettings.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"TintSettings.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentView.h"
#include "ComponentDocument.h"
#include "FrameworkResourceIds.h"

// ****************************************************************************
//
//  Function Name:	RTintSettings::RTintSettings()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RTintSettings::RTintSettings()
{
	m_eTintState	= kNotSet;
	m_Tint			= kMaxTint;
}

// ****************************************************************************
//
//  Function Name:	RTintSettings::RTintSettings()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RTintSettings::RTintSettings( const RComponentView* pView )
{
	m_eTintState	= kSet;
	m_Tint			= pView->GetComponentDocument( )->GetTint( );
}


// ****************************************************************************
//
//  Function Name:	RTintSettings::MergeSetting()
//
//  Description:		Merge the contents of two settings structures
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RTintSettings::MergeSetting( RSettings* pMerge )
{
	TpsAssertIsObject( RTintSettings, pMerge );
	RTintSettings*			pTintSettings	= static_cast<RTintSettings*>( pMerge );

	if (m_eTintState == kNotSet)
	{
		// No component has set their tint value yet, so set it here
		m_Tint			= pTintSettings->m_Tint;
		m_eTintState	= kSet;
	}
	else if (m_eTintState == RSettings::kSet)
	{
		// Some component has already set its tint value, so compare
		if (m_Tint != pTintSettings->m_Tint)
			m_eTintState = kMultiState;
	}

}

// ****************************************************************************
//
//  Function Name:	RTintSettingsInterfaceImp::RTintSettingsInterfaceImp()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RTintSettingsInterfaceImp::RTintSettingsInterfaceImp( const RComponentView* pView )
	: RTintSettingsInterface( pView ),
	  m_Settings( pView )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RTintSettingsInterfaceImp::RTintSettingsInterfaceImp()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RInterface* RTintSettingsInterfaceImp::CreateInterface( const RComponentView* pView )
{
	return new RTintSettingsInterfaceImp( pView );
}

// ****************************************************************************
//
//  Function Name:	RTintSettingsInterfaceImp::GetData()
//
//  Description:		Returns a pointer to the Settings data
//
//  Returns:			m_Settings;
//
//  Exceptions:		None
//
// ****************************************************************************
RSettings* RTintSettingsInterfaceImp::GetData( )
{
	return &m_Settings;
}

// ****************************************************************************
//
//  Function Name:	RTintSettingsInterfaceImp::AlterSettings()
//
//  Description:		Alters the settings based on the commandId
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RTintSettingsInterfaceImp::AlterSettings( YCommandID commandId )
{
	YTint yTint = (YTint)((commandId - COMMAND_MENU_EFFECTS_TINT_0) * 10);	
	if ( (m_Settings.m_eTintState != RSettings::kSet) || (m_Settings.m_Tint != yTint) )
	{
			m_Settings.m_Tint = yTint;
			return TRUE;
	}
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RTintSettingsInterfaceImp::ApplySettings()
//
//  Description:		Applies the settings to the 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RTintSettingsInterfaceImp::ApplySettings( RComponentView* pView )
{
	pView->ApplyTint( m_Settings.m_Tint );
}

// ****************************************************************************
//
//  Function Name:	RTintSettingsInterfaceImp::SettingsUI()
//
//  Description:		Set the commandUI as appropriate for the stored settings
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RTintSettingsInterfaceImp::SettingsUI( RCommandUI& commandUI ) const
{
	YCommandID	commandId	= commandUI.GetCommandID( );
	YTint			commandTint	= (YTint)((commandId - COMMAND_MENU_EFFECTS_TINT_0) * 10);	
	commandUI.Enable( );
	if ( (m_Settings.m_eTintState == RSettings::kSet) && (m_Settings.m_Tint == commandTint) )
		commandUI.SetCheck( RCommandUI::kCheckMark );
	else
		commandUI.SetCheck( RCommandUI::kNoCheck );
}

// ****************************************************************************
//
//  Function Name:	RTintSettingsInterfaceImp::GetUndoRedoStringIds()
//
//  Description:		Returns the Undo/Redo string ids for the interface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RTintSettingsInterfaceImp::GetUndoRedoStringIds( uWORD& uwUndoId, uWORD& uwRedoId ) const
{
	uwUndoId	= STRING_UNDO_CHANGE_TINT;
	uwRedoId	= STRING_REDO_CHANGE_TINT;
}
