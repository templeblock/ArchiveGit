// ****************************************************************************
//
//  File Name:			FrameworkIncludes.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Common framework include files. All framework source files
//							and all modules using the framework should include this
//							file.
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
//  $Logfile:: /PM8/Framework/Include/FrameworkIncludes.h                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FRAMEWORKINCLUDES_H_
#define		_FRAMEWORKINCLUDES_H_

#define BYTE_ALIGNMENT	8

#pragma pack(push, BYTE_ALIGNMENT)

// Windows and MFC includes
#ifdef _WINDOWS
	#include "stdafx.h"
	#include "winspool.h"
	#include "SHLOBJ.H"

	#include	<malloc.h>

	// Smartheap includes
	#ifdef	USE_SMARTHEAP
		#include "smrtheap.hpp"
	#endif	//	USE_SMARTHEAP


#endif

#ifdef MAC
	#include <CPlusLib.h>
	#include <New.h>
#endif

// TPS includes
#include		"tpsdefs.h"
#include		"tpsmem.h"
#include		"tpsrect.h"
#include		"tpsbit.h"
#include		"tpsint.h"

// Various ANSI includes
#include		<typeinfo.h>
#include		<string.h>
#include		<limits.h>
#include		<math.h>
#include		<float.h>

#ifdef _WINDOWS
	#include		<process.h>
	#include		<sys/stat.h>

	// Turn off the stupid "identifier was truncated to '255' characters in the browser information" warning
	#pragma warning( disable : 4786 )

	// Turn off "nonstandard extension used : redefined extern to static" warning. This is giving me
	// problems with the message dispatch
	#pragma warning( disable : 4211 )

	#pragma warning( disable : 4305 )

	// REVIEW STA VC5 - Turn off the "unsafe mix of type 'unsigned char' and type 'bool' in operation" 
	// Maybe go to the bool type later
	#pragma warning( disable : 4805 )

	//	Turn off "Conversion from 'double' to 'const float' warning since our base type
	//	is float and 3.0 is a double according to C++
	#pragma warning( disable : 4244 )
#endif

#include		"FrameworkLinkage.h"

// Floating point support
#include		"FloatingPoint.h"

// Type definitions
#include		"FrameworkBaseTypes.h"
#include		"Object.h"
#include		"Archive.h"

// Base types
#include		"2dTransform.h"
#include		"Size.h"
#include		"Point.h"
#include		"Rect.h"
#include		"VectorRect.h"

// Unit conversion
#include		"UnitConversion.h"

// Exception Handling
#include		"ExceptionHandling.h"

// Collections and algorithms
#include		"Algorithms.h"
#include		"List.h"
#include		"Array.h"

// Various files that dont change much that we would like in the PCH
#include		"SolidColor.h"
#include		"Color.h"
#include		"CommandUI.h"
#include		"Character.h"
#include		"MbcString.h"
#ifdef _WINDOWS
#include		"ThreadSupport.h"
#endif
#include		"Action.h"
#include		"ArchiveSupport.h"
#include		"Utilities.h"
#include		"Font.h"
#include		"FontLists.h"
#include		"Cache.h"
#include		"CommandTarget.h"
#include		"CommandMap.h"
#include		"GlobalBuffer.h"
#include		"Cursor.h"
#include		"Alert.h"

// More complex types that couldnt be declared in BaseTypes because
// of dependencies on files listed above
#include		"FrameworkComplexTypes.h"

#pragma pack(pop)

#endif
