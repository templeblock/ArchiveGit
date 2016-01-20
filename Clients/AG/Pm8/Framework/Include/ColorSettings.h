//****************************************************************************
//
// File Name:			ColorSettings.h
//
// Project:				Renaissance Framework
//
// Author:				Mike Houle
//
// Description:		Definition of Color Settings and Color Interface classes
//
// Portability:		Platform Independant
//
// Developed by:		Turning Point Software
//						   One Gateway Center, Suite 800
//						   Newton, Ma 02158
//							(617)332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/ColorSettings.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _COLORSETTINGS_H_
#define _COLORSETTINGS_H_

#ifndef	_SETTINGS_H_
#include "Settings.h"
#endif

#ifndef	_SETTINGSINTERFACE_H_
#include "SettingsInterface.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

const YInterfaceId	kColorSettingsInterfaceId	= 2;

// ****************************************************************************
//
//  Class Name:	RColorSettings
//
//  Description:	The class for storing color change information.
//
// ****************************************************************************
class FrameworkLinkage RColorSettings : public RSettings
{
	//	Construction
	public:
													RColorSettings();
													RColorSettings( const RComponentView* pView );
	//	Operations
	public :
		virtual void							MergeSetting( RSettings* pMerge );

	//	Member Data
	public :
		ESettingState							m_eState;
		RColor									m_Color;
};

// ****************************************************************************
//
//  Class Name:	RColorSettingsInterface
//
//  Description:	Interface for setting the color of an component.
//
// ****************************************************************************
class FrameworkLinkage RColorSettingsInterface : public RSettingsInterface
	{
	//	Construction & Destruction
	public :
													RColorSettingsInterface( const RComponentView* );

		static RInterface*					CreateInterface( const RComponentView* );
	//
	//	Operations
	public :
		RSettings*								GetData( );
		void										SetData( RComponentView* pView );
		BOOLEAN									AlterSettings( YCommandID commandId );
		void										ApplySettings( RComponentView* pView );
		void										SettingsUI( RCommandUI& commandUI ) const;
		void										GetUndoRedoStringIds( uWORD& uwUndoId, uWORD& uwRedoId ) const;
	//
	//	Access to members
	public :
		RColor									GetColor( ) const;
		void										SetColor( const RColor& color );

	//	Member Data
	private :
		RColorSettings							m_Settings;
		RComponentView*						m_pView;
	};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_COLORSETTINGS_H_
