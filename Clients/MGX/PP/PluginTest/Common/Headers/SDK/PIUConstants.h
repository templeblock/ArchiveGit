//-------------------------------------------------------------------------------
//
//	File:
//		PIUConstants.h
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the constants and definitions for 
//		routines that simplify the use of suites and also some
//		helpful common plug-in functions. 
//
//	Use:
//		PIUBasic is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	1/11/1998	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------
 
#ifndef __PIUConstants_h__	// Defined yet?
#define __PIUConstants_h__	// Only define once.

#include <stddef.h>

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//-------------------------------------------------------------------------------
//	Definitions.
//-------------------------------------------------------------------------------
const size_t kMaxStr255Len = 255; // Maximum standard string length. (Pascal, etc.)

//-------------------------------------------------------------------------------
//	Errors.
//-------------------------------------------------------------------------------
static const SPErr errPIUMissingFunctionParameter = '!fxn';

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
} // End of extern "C" block.
#endif // __cplusplus

//-------------------------------------------------------------------------------

#endif // __PIUConstants_h__
