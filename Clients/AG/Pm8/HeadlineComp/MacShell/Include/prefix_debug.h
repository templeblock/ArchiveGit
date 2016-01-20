// ****************************************************************************
//
//  File Name:			prefix_debug.h
//
//  Project:			Renaissance Graphic Headline Renderer
//
//  Author:				R. Hood
//
//  Description:		Application prefix Header
//
//  Portability:		Platform Dependent
//
//  Developed by:		Turning Point Software.
//						One Gateway Center, Suite 800
//						Newton, MA 02158
//						(617) 332-0202
//
//  Client:         
//
//	Notes:
//			This file is the PreCompiled header for the core code
//			library that will be used for the Renaissance project
//
//			This file will be used for a Debug version
//			of the application.
//
//			It will include the System Header as appropriate for CW PowerPlant
//
//  Copyright (C) 1995, 1996 Turning Point Software. All Rights Reserved.
//
//  $Header:: /PM8/HeadlineComp/MacShell/Include/prefix_debug.h 1     3/03/99 $
//
// ****************************************************************************

#ifdef THINK_C
	#ifndef __TYPES__
	#error	"<MacHeaders> needs to be included first."
	#endif
#else
	#ifndef	powerc
		#include <MacHeaders68K>
	#else
		#include <MacHeadersPPC>
	#endif	// powerc
#endif	//	ifdef THINK_C

#define	MAC				1
#define	HEADLINER		1
#define	IMAGER			1
#define	RENAISSANCE		1
#define	TPSDEBUG			1
