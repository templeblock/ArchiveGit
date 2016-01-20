//****************************************************************************
//
// File Name:			TextInterface.h
//
// Project:				Renaissance Text Component
//
// Author:				Mike Houle
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
//  $Logfile:: /PM8/Interfaces/TintInterface.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************
#ifndef _TINTINTERFACE_H_
#define _TINTINTERFACE_H_

#ifndef	_SETTINGSINTERFACE_H_
#include "SettingsInterface.h"
#endif

const YInterfaceId	kTintSettingsInterfaceId	= 1;

// ****************************************************************************
//
//  Class Name:	RTintSettingsInterface
//
//  Description:	Interface for setting the tint of an object.
//
// ****************************************************************************
class FrameworkLinkage RTintSettingsInterface : public RSettingsInterface
	{
	//	Construction & Destruction
	public :
													RTintSettingsInterface( const RComponentView* pView ) : RSettingsInterface( pView ) { ; }
	};


#endif //_TINTINTERFACE_H_
