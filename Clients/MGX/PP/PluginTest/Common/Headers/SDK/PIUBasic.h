//-------------------------------------------------------------------------------
//
//	File:
//		PIUBasic.h
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the headers for the macros, and
//		routines to simplify the use of suites and also some
//		helpful common plug-in functions. 
//
//	Use:
//		PIUBasic is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	7/31/1997	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------
 
#ifndef __PIUBasic_H__	// Defined yet?
#define __PIUBasic_H__	// Only define once.

//-------------------------------------------------------------------------------
//	Standard includes for both platforms.
//-------------------------------------------------------------------------------

#ifndef Rez

#include "PIUNew.h"					// Use this before any other file compiles.
#include "PIUSuites.h"				// All suites.
#include "PIProperties.h"			// Photoshop properties.
#include "PIAbout.h"				// AboutRecord structure.
#include "PIUDispatch.h"			// Entrypoint dispatch.
#include "PIUFile.h"				// File utilities.
#include "PIUCore.h"				// Core SDK utilities.

//-------------------------------------------------------------------------------
//	Includes specific to each platform.
//-------------------------------------------------------------------------------
#ifdef __PIWin__

	#include <stdlib.h>
	#include <winver.h>
	
	// May not need this, but define it for now and we'll check later:
	Fixed FixRatio(short numer, short denom);

#else // Macintosh or other:

	#include <LowMem.h>

	#define PISetRect	SetRect
	
	// Macintosh requires an entrypoint named "main":
	#define ENTRYPOINT main
	
#endif // Mac/Win

#endif // Rez

//-------------------------------------------------------------------------------
//	Resource string IDs.
//-------------------------------------------------------------------------------

// Some of these are generic.  Override if you need to in your .h files:

#ifndef StringResource
	#define StringResource	'STR '
#endif

#ifndef ResourceID
	#define ResourceID		16000
#endif

#ifndef AboutID
	#define AboutID			ResourceID
#endif

#ifndef uiID
	#define uiID			ResourceID+1
#endif

#ifndef ADMAboutID
	#define ADMAboutID		uiID+1
#endif

#ifndef AlertID
	#define AlertID			16990
#endif

#ifndef kBadNumberID
	#define kBadNumberID	AlertID
#endif

#ifndef kBadDoubleID
	#define kBadDoubleID	kBadNumberID+1
#endif

#ifndef kNeedVers
	#define kNeedVers		kBadDoubleID+1
#endif

#ifndef kWrongHost
	#define kWrongHost		kNeedVers+1
#endif

#ifndef kSuiteMissing
	#define kSuiteMissing	kWrongHost+1
#endif


// This should be defined in ASType, but for some odd
// reason is missing.  Use it just like userCanceledErr:
#ifndef kUserCancel
	#define kUserCancel		0x53544f50L		// 'STOP'
#endif

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
} // End of extern "C" block.
#endif // __cplusplus

//-------------------------------------------------------------------------------

#endif // __PIUBasic_H__
