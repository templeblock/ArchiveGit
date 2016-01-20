// ****************************************************************************
//
//  File Name:			BackgroundColorSettings.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RBackgroundColorSettings & RBackgroundColorSettingsInterface class
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
//  $Logfile:: /PM8/Framework/Source/BackgroundColorSettings.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"BackgroundColorSettings.h"

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
//  Function Name:	RBackgroundColorSettings::RBackgroundColorSettings()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RBackgroundColorSettings::RBackgroundColorSettings()
{
	m_eState = kNotSet;
	m_Color = RSolidColor( kBlack );
}

// ****************************************************************************
//
//  Function Name:	RBackgroundColorSettings::RBackgroundColorSettings()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RBackgroundColorSettings::RBackgroundColorSettings( const RComponentView* pView )
{
	//	REVIEW ESV - could this ever be anything else? RColorSettings tests the
	//	return value of GetColor but GetColor always returns TRUE.
	m_Color	= pView->GetBackgroundColor( );
	m_eState = kSet;
}


// ****************************************************************************
//
//  Function Name:	RBackgroundColorSettings::MergeSetting()
//
//  Description:		Merge the Settings structures
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RBackgroundColorSettings::MergeSetting( RSettings* pMerge )
{
	TpsAssertIsObject( RBackgroundColorSettings, pMerge );
	RBackgroundColorSettings*	pBackgroundColorSettings	= static_cast<RBackgroundColorSettings*>( pMerge );

	if (m_eState == kNotSet)
	{
		// No component has set their color value yet, so set it here
		m_Color	= pBackgroundColorSettings->m_Color;
		m_eState	= kSet;
	}
	else if (m_eState == kSet)
	{
		// Some component has already set its color value, so compare
		if ( m_Color != pBackgroundColorSettings->m_Color )
			m_eState = kMultiState;
	}

}

// ****************************************************************************
//
//  Function Name:	RBackgroundColorSettingsInterface::RBackgroundColorSettingsInterface()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RBackgroundColorSettingsInterface::RBackgroundColorSettingsInterface( const RComponentView* pView )
	: RSettingsInterface( pView ),
	  m_Settings( pView ),
	  m_pView( const_cast<RComponentView*>( pView ) )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RBackgroundColorSettingsInterface::RBackgroundColorSettingsInterface()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RInterface* RBackgroundColorSettingsInterface::CreateInterface( const RComponentView* pView )
{
	return new RBackgroundColorSettingsInterface( pView );
}

// ****************************************************************************
//
//  Function Name:	RBackgroundColorSettingsInterface::GetData()
//
//  Description:		Returns a pointer to the Settings data
//
//  Returns:			m_Settings;
//
//  Exceptions:		None
//
// ****************************************************************************
RSettings* RBackgroundColorSettingsInterface::GetData( )
{
	return &m_Settings;
}

// ****************************************************************************
//
//  Function Name:	RBackgroundColorSettingsInterface::AlterSettings()
//
//  Description:		Alters the settings based on the commandId
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RBackgroundColorSettingsInterface::AlterSettings( YCommandID commandId )
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
//  Function Name:	RBackgroundColorSettingsInterface::ApplySettings()
//
//  Description:		Applies the settings to the 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RBackgroundColorSettingsInterface::ApplySettings( RComponentView* pView )
{
	pView->SetBackgroundColor( m_Settings.m_Color );
}

// ****************************************************************************
//
//  Function Name:	RBackgroundColorSettingsInterface::SettingsUI()
//
//  Description:		Set the commandUI as appropriate for the stored settings
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RBackgroundColorSettingsInterface::SettingsUI( RCommandUI& commandUI ) const
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
//  Function Name:	RBackgroundColorSettingsInterface::GetUndoRedoStringIds()
//
//  Description:		Returns the Undo/Redo string ids for the interface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RBackgroundColorSettingsInterface::GetUndoRedoStringIds( uWORD& uwUndoId, uWORD& uwRedoId ) const
{
	uwUndoId	= STRING_UNDO_CHANGE_COLOR;
	uwRedoId	= STRING_REDO_CHANGE_COLOR;
}


// ****************************************************************************
//
//  Function Name:	RBackgroundColorSettingsInterface::GetColor()
//
//  Description:		Returns the color
//
//  Returns:			m_Settings.m_Color
//
//  Exceptions:		None
//
// ****************************************************************************
RColor RBackgroundColorSettingsInterface::GetColor( ) const
	{
	return m_Settings.m_Color;
	}

// ****************************************************************************
//
//  Function Name:	RBackgroundColorSettingsInterface::SetColor()
//
//  Description:		Set the color
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RBackgroundColorSettingsInterface::SetColor( const RColor& color )
	{
	m_Settings.m_Color	= color;
	m_Settings.m_eState	= RBackgroundColorSettings::kSet;
	m_pView->SetBackgroundColor( m_Settings.m_Color );
	}
