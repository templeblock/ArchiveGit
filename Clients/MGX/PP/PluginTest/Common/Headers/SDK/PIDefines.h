//-------------------------------------------------------------------------------
//
//	File:
//		PIDefines.h
//
//	Copyright 1996-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains the prototypes and definitions
//		generic for plug-ins.
//
//	Use:
//		This particular file will define the flags for
//		generic values needed by every plug-in.
//
//	Version history:
//		Version 1.0.0	5/29/1997	Created for Photoshop 4.0.1
//			Written by Andrew Coven
//
//-------------------------------------------------------------------------------

#ifndef __PIDefines_H__	// Has this not been defined yet?
#define __PIDefines_H__	// Only include this once by predefining it

// Create a definition if we're on a Windows machine:
#ifndef __PIWin__
	#if MSWindows
		#define __PIWin__			1
	#elif (defined(MSDOS) || defined(WIN32) || defined(_WIN32) || defined(__INTEL__))
		#define __PIWin__			1
	#endif
#endif

// Create a definition if we're under Metrowerks Codewarrior:
#ifndef __PIMWCW__
	#ifdef __MWERKS__
		#define __PIMWCW__			1
	#endif
#endif

// Create a definition if we're on a Macintosh:
#ifndef __PIMac__
	#if (defined(macintosh) || defined(__MC68K__) || defined(__POWERPC__) || defined(__powerc))
		#define __PIMac__			1
	#endif
#endif

// Create a definition if we're on a MC680x0 Macintosh:
#ifndef __PIMac68K__
	#if (defined(__PIMac__) && defined(__MC68K__))
		#define __PIMac68K__		1
	#endif
#endif

// Create a definition if we're on a PowerPC Macintosh:
#ifndef __PIMacPPC__
	#if (defined(__PIMac__) && (defined(__powerc) || defined(__POWERPC__)))
		#define __PIMacPPC__		1
	#endif
#endif

// Create a definition if we're on a Mac under Metrowerks Codewarrior:
#ifndef __PIMWCWMac__
	#if (defined(__PIMac__) && defined(__PIMWCW__))
		#define __PIMWCWMac__		1
	#endif
#endif

// Create a definition if we're on a 68K Macintosh and running Metrowerks:
#ifndef __PIMWCWMac68K__
	#if (defined(__PIMac68K__) && defined(__PIMWCW__))
		#define __PIMWCWMac68K__	1
	#endif
#endif

// Create a definition if we're on a PPC Macintosh and running Metrowerks:
#ifndef __PIMWCWMacPPC__
	#if (defined(__PIMacPPC__) && defined(__PIMWCW__))
		#define __PIMWCWMacPPC__	1
	#endif
#endif

//-------------------------------------------------------------------------------

#endif // __PIDefines_H__
