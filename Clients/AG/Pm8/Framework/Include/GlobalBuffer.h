// ****************************************************************************
//
//  File Name:			GlobalBuffer.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Routines to manage the shared global buffers
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/GlobalBuffer.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GLOBALBUFFER_H_
#define		_GLOBALBUFFER_H_

//	Default Global Buffer Size
const uLONG		kDefaultGlobalBufferSize			= 0x0FFFF;

BOOLEAN 	StartupGlobalBuffer( );
void 		ShutdownGlobalBuffer( );

FrameworkLinkage uBYTE*		GetGlobalBuffer( uLONG ulRequestedSize = kDefaultGlobalBufferSize );
FrameworkLinkage void		ReleaseGlobalBuffer( uBYTE* pBuffer );
FrameworkLinkage uLONG		GetGlobalBufferSize( LPUBYTE pBuffer = NULL );

#endif		// _GLOBALBUFFER_H_

