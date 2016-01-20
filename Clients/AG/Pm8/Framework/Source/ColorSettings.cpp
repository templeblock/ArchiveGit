// ****************************************************************************
//
//  File Name:			ColorSettings.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RColorSettings & RColorSettingsInterface class
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
//  $Logfile:: /PM8/Framework/Source/ColorSettings.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ColorSettings.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentView.h"
#include "ComponentDocument.h"
#include "FrameworkResourceIds.h"

#ifdef	_WINDOWS
#include "WinColorDlg.h"
#endif
// ****************************************************************************
//
//  Function Name:	RColorSettings::RColorSettings()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RColorSettings::RColorSettings()
{
	m_eState = kNotSet;
	m_Color = RSolidColor( kBlack );
}

// ****************************************************************************
//
//  Function Name:	RColorSettings::RColorSettings()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RColorSettings::RColorSettings( const RComponentView* pView )
{
	m_eState = ( pView->GetColor( m_Color )? kSet : kMultiState );
}


// ****************************************************************************
//
//  Function Name:	RColorSettings::MergeSetting()
//
//  Description:		Merge the Settings structures
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RColorSettings::MergeSetting( RSettings* pMerge )
{
	TpsAssertIsObject( RColorSettings, pMerge );
	RColorSettings*	pColorSettings	= static_cast<RColorSettings*>( pMerge );

	if (m_eState == kNotSet)
	{
		// No component has set their color value yet, so set it here
		m_Color	= pColorSettings->m_Color;
		m_eState	= kSet;
	}
	else if (m_eState == kSet)
	{
		// Some component has already set its color value, so compare
		if ( m_Color != pColorSettings->m_Color )
			m_eState = kMultiState;
	}

}

// ****************************************************************************
//
//  Function Name:	RColorSettingsInterface::RColorSettingsInterface()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RColorSettingsInterface::RColorSettingsInterface( const RComponentView* pView )
	: RSettingsInterface( pView ),
	  m_Settings( pView ),
	  m_pView( const_cast<RComponentView*>( pView ) )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RColorSettingsInterface::RColorSettingsInterface()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RInterface* RColorSettingsInterface::CreateInterface( const RComponentView* pView )
{
	return new RColorSettingsInterface( pView );
}

// ****************************************************************************
//
//  Function Name:	RColorSettingsInterface::GetData()
//
//  Description:		Returns a pointer to the Settings data
//
//  Returns:			m_Settings;
//
//  Exceptions:		None
//
// ****************************************************************************
RSettings* RColorSettingsInterface::GetData( )
{
	return &m_Settings;
}

// ****************************************************************************
//
//  Function Name:	RColorSettingsInterface::AlterSettings()
//
//  Description:		Alters the settings based on the commandId
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RColorSettingsInterface::AlterSettings( YCommandID commandId )
{
	RColor	tmpColor = m_Settings.m_Color;

	switch( commandId )
	{
		case COMMAND_MENU_EFFECTS_COLOR_DLG:
		case COMMAND_MENU_EFFECTS_COLOR_OBJECT:
		{
#ifdef	_WINDOWS
			//	Get the color dialog
			RWinColorDlg getColorDlg( NULL, 0 );
			//	Select the current color.
			getColorDlg.SetColor( m_Settings.m_Color );
			if ( getColorDlg.DoModal() == IDOK )
				tmpColor = getColorDlg.SelectedSolidColor( );
#endif
#ifdef	MAC
			tmpColor = RSolidColor( kBlack );
#endif
			break;
		}
		case COMMAND_MENU_EFFECTS_COLOR_BLACK:
			tmpColor = RSolidColor( kBlack );
			break;
		case COMMAND_MENU_EFFECTS_COLOR_WHITE:
			tmpColor = RSolidColor( kWhite ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_YELLOW:
			tmpColor = RSolidColor( kYellow ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_MAGENTA:
			tmpColor = RSolidColor( kMagenta ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_RED:
			tmpColor = RSolidColor( kRed ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_CYAN:
			tmpColor = RSolidColor( kCyan ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_GREEN:
			tmpColor = RSolidColor( kGreen ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_BLUE:
			tmpColor = RSolidColor( kBlue ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_LTGRAY:
			tmpColor = RSolidColor( kLightGray ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_GRAY:
			tmpColor = RSolidColor( kGray ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_DKGRAY:
			tmpColor = RSolidColor( kDarkGray ); 
			break;
	}

	if ( (m_Settings.m_eState != RSettings::kSet) || (m_Settings.m_Color != tmpColor) )
	{
			m_Settings.m_Color = tmpColor;
			return TRUE;
	}
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RColorSettingsInterface::ApplySettings()
//
//  Description:		Applies the settings to the 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RColorSettingsInterface::ApplySettings( RComponentView* pView )
{
	pView->ApplyColor( m_Settings.m_Color );
}

// ****************************************************************************
//
//  Function Name:	RColorSettingsInterface::SettingsUI()
//
//  Description:		Set the commandUI as appropriate for the stored settings
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RColorSettingsInterface::SettingsUI( RCommandUI& commandUI ) const
{
	RColor		tmpColor;
	YCommandID	commandId	= commandUI.GetCommandID( );

	commandUI.Enable( );

	switch( commandId )
	{
			// Intentional fall-throughs
		case COMMAND_MENU_EFFECTS_COLOR_DLG:
		case COMMAND_MENU_EFFECTS_COLOR_OBJECT:
			return;

		case COMMAND_MENU_EFFECTS_COLOR_BLACK:
			tmpColor = RSolidColor( kBlack );
			break;
		case COMMAND_MENU_EFFECTS_COLOR_WHITE:
			tmpColor = RSolidColor( kWhite ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_YELLOW:
			tmpColor = RSolidColor( kYellow ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_MAGENTA:
			tmpColor = RSolidColor( kMagenta ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_RED:
			tmpColor = RSolidColor( kRed ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_CYAN:
			tmpColor = RSolidColor( kCyan ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_GREEN:
			tmpColor = RSolidColor( kGreen ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_BLUE:
			tmpColor = RSolidColor( kBlue ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_LTGRAY:
			tmpColor = RSolidColor( kLightGray ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_GRAY:
			tmpColor = RSolidColor( kGray ); 
			break;
		case COMMAND_MENU_EFFECTS_COLOR_DKGRAY:
			tmpColor = RSolidColor( kDarkGray ); 
			break;
	}

	if ( (m_Settings.m_eState == RSettings::kSet) && (m_Settings.m_Color == tmpColor) )
		commandUI.SetCheck( RCommandUI::kCheckMark );
	else
		commandUI.SetCheck( RCommandUI::kNoCheck );
}

// ****************************************************************************
//
//  Function Name:	RColorSettingsInterface::GetUndoRedoStringIds()
//
//  Description:		Returns the Undo/Redo string ids for the interface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RColorSettingsInterface::GetUndoRedoStringIds( uWORD& uwUndoId, uWORD& uwRedoId ) const
{
	uwUndoId	= STRING_UNDO_CHANGE_COLOR;
	uwRedoId	= STRING_REDO_CHANGE_COLOR;
}


// ****************************************************************************
//
//  Function Name:	RColorSettingsInterface::GetColor()
//
//  Description:		Returns the color
//
//  Returns:			m_Settings.m_Color
//
//  Exceptions:		None
//
// ****************************************************************************
RColor RColorSettingsInterface::GetColor( ) const
	{
	return m_Settings.m_Color;
	}

// ****************************************************************************
//
//  Function Name:	RColorSettingsInterface::SetColor()
//
//  Description:		Set the color
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RColorSettingsInterface::SetColor( const RColor& color )
	{
	m_Settings.m_Color	= color;
	m_Settings.m_eState	= RColorSettings::kSet;
	m_pView->ApplyColor( m_Settings.m_Color );
	}
