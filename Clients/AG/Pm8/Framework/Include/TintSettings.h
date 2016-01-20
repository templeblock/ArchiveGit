//****************************************************************************
//
// File Name:			TextSettings.h
//
// Project:				Renaissance Text Component
//
// Author:				Mike Heydlauf
//
// Description:		Definition of Text Settings classes
//
// Portability:		Windows Specific
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
//  $Logfile:: /PM8/Framework/Include/TintSettings.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************
#ifndef _TINTSETTINGS_H_
#define _TINTSETTINGS_H_

#ifndef	_SETTINGS_H_
#include "Settings.h"
#endif

#ifndef	_TINTINTERFACE_H_
#include "TintInterface.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RTintSettings
//
//  Description:	The class for storing character change information.
//
// ****************************************************************************
class FrameworkLinkage RTintSettings : public RSettings
	{
	//	Construction and Destruction
	public:
													RTintSettings( );
													RTintSettings( const RComponentView* pView );

	//	Operations
	public :
		virtual void							MergeSetting( RSettings* pMerge );

	//	Member Data
	public :
		ESettingState							m_eTintState;
		YTint										m_Tint;
	};

// ****************************************************************************
//
//  Class Name:	RTintSettingsInterface
//
//  Description:	Interface for setting the tint of an object.
//
// ****************************************************************************
class FrameworkLinkage RTintSettingsInterfaceImp : public RTintSettingsInterface
	{
	//	Construction & Destruction
	public :
													RTintSettingsInterfaceImp( const RComponentView* pView );

		static RInterface*					CreateInterface( const RComponentView* );

	//	Operations
	public :
		RSettings*								GetData( );
		BOOLEAN									AlterSettings( YCommandID commandId );
		void										ApplySettings( RComponentView* pView );
		void										SettingsUI( RCommandUI& commandUI ) const;
		void										GetUndoRedoStringIds( uWORD& uwUndoId, uWORD& uwRedoId ) const;

	//	Member Data
	private :
		RTintSettings							m_Settings;
	};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_TINTSETTINGS_H_
