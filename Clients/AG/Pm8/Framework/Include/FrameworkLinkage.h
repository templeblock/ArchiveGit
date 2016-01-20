// ****************************************************************************
//
//  File Name:			FrameworkLinkage.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the FrameworkLinkage macro
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
//  $Logfile:: /PM8/Framework/Include/FrameworkLinkage.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#undef FrameworkLinkage

#ifdef _WINDOWS
	#ifdef FRAMEWORK
		#define FrameworkLinkage	__declspec( dllexport )
	#else
		#define FrameworkLinkage	__declspec( dllimport )
	#endif
#else
	#define FrameworkLinkage
#endif
