// ****************************************************************************
//
//  File Name:			SettingsInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RSettingsInterface class
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
//  $Logfile:: /PM8/Interfaces/SettingsInterface.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SETTINGSINTERFACE_H_
#define		_SETTINGSINTERFACE_H_

#ifndef	_INTERFACE_H_
#include "Interface.h"
#endif

//	Forward References
class	RSettings;
class RCommandUI;

//	Structs and Supporting Classes
struct CommandIdToInterfaceId
	{
	YCommandID		commandId;
	YInterfaceId	interfaceId;
	} ;

// ****************************************************************************
//
//  Class Name:	RSettingsInterface
//
//  Description:	A basic class for Implementing various settings that may or
//						may not be applied across components and within components.
//
// ****************************************************************************
//
class RSettingsInterface : public RInterface
	{
	//	Construction and Destruction
	public :
													RSettingsInterface( const RComponentView* pView ) : RInterface( pView ) { ; }
	//	Operations
	public :
		virtual RSettings*					GetData( ) = 0;
		virtual BOOLEAN						AlterSettings( YCommandID commandId ) = 0;
		virtual void							ApplySettings( RComponentView* pView ) = 0;
		virtual void							SettingsUI( RCommandUI& commandUI ) const = 0;
		virtual void							GetUndoRedoStringIds( uWORD& uwUndoId, uWORD& uwRedoId ) const = 0;
	};


#endif	//	_SETTINGSINTERFACE_H_
