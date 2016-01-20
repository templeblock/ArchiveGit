//****************************************************************************
//
// File Name:			BackgroundColorSettings.h
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
//  $Logfile:: /PM8/Framework/Include/BackgroundColorSettings.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _BACKGROUNDCOLORSETTINGS_H_
#define _BACKGROUNDCOLORSETTINGS_H_

#pragma pack(push, BYTE_ALIGNMENT)

#ifndef	_COLORSETTINGS_H_
#include "ColorSettings.h"
#endif


const YInterfaceId	kBackgroundColorSettingsInterfaceId	= 4;

// ****************************************************************************
//
//  Class Name:	RBackgroundColorSettings
//
//  Description:	The class for storing color change information.
//
// ****************************************************************************
class FrameworkLinkage RBackgroundColorSettings : public RColorSettings
{
	//	Construction
	public:
													RBackgroundColorSettings();
													RBackgroundColorSettings( const RComponentView* pView );
	//	Operations
	public :
		virtual void							MergeSetting( RSettings* pMerge );

	//	Member Data
	public :
};

// ****************************************************************************
//
//  Class Name:	RBackgroundColorSettingsInterface
//
//  Description:	Interface for setting the color of an component.
//
// ****************************************************************************
class FrameworkLinkage RBackgroundColorSettingsInterface : public RSettingsInterface
	{
	//	Construction & Destruction
	public :
													RBackgroundColorSettingsInterface( const RComponentView* );

		static RInterface*					CreateInterface( const RComponentView* );
	//
	//	Operations
	public :
		RSettings*								GetData( );
		void										SetData( RComponentView* pView );
		BOOLEAN									AlterSettings( YCommandID commandId );
		virtual void							ApplySettings( RComponentView* pView );
		void										SettingsUI( RCommandUI& commandUI ) const;
		void										GetUndoRedoStringIds( uWORD& uwUndoId, uWORD& uwRedoId ) const;
	//
	//	Access to members
	public :
		RColor									GetColor( ) const;
		void										SetColor( const RColor& color );

	//	Member Data
	private :
		RBackgroundColorSettings			m_Settings;
		RComponentView*						m_pView;
	};


#pragma pack(pop)

#endif	//	_BACKGROUNDCOLORSETTINGS_H_
