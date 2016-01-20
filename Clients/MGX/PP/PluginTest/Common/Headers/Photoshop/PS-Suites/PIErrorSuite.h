//-------------------------------------------------------------------------------
//
//	File:
//		PIErrorSuite.h
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Distribution:
//		PUBLIC
//
//	Description:
//		This file contains the public definitions and structures
//		used by plug-ins to manage asserts, debugging, and returning
//		errors to the host.
//
//	Version history:
//		Version 1.0.0	1/8/1998	Ace		Photoshop 5.0.
//			Created by Andrew Coven.
//
//-------------------------------------------------------------------------------

#ifndef __PIErrorSuite_h__ // Already defined?
#define __PIErrorSuite_h__

//-------------------------------------------------------------------------------
//	Includes and alignment.
//-------------------------------------------------------------------------------
#include "ASZStringSuite.h"

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------------
//	Error suite version 1.
//-------------------------------------------------------------------------------
#define kPSErrorSuite			"Photoshop Error Suite for Plug-ins"
#define kPSErrorSuiteVersion1	1

//-------------------------------------------------------------------------------
//	Error suite typedefs.
//-------------------------------------------------------------------------------

// Hand the host a Pascal string containing the error string to display to the user.
// The host will make a copy:
typedef SPAPI SPErr (*SetErrorFromPStringProc) (/* IN */ const Str255 errorString);

// Hand the host a C string containing the error string to display to the user.
// The host will make a copy:
typedef SPAPI SPErr (*SetErrorFromCStringProc) (/* IN */ const char* errorString);

// Hand the host a ZString id containing the error string to display to the user.
// The host will make a copy:
typedef SPAPI SPErr (*SetErrorFromZStringProc) (/* IN */ const ASZString zString);

typedef struct PSErrorSuite1
	{
	SetErrorFromPStringProc	SetErrorFromPString;
	SetErrorFromCStringProc SetErrorFromCString;
	SetErrorFromZStringProc SetErrorFromZString;
	} PSErrorSuite1;
	
//-------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif


#endif // PIErrorSuite.h
