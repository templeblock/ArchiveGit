//-------------------------------------------------------------------------------
//
//	File:
//		PIUTools.h
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the headers for the macros, and
//		routines to simplify the use of suites and also some
//		helpful common plug-in functions for file access. 
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
 
#ifndef __PIUFile_h__	// Defined yet?
#define __PIUFile_h__	// Only define once.

#include "SPFiles.h"

#include "PIUSuites.h"
#include "PIUTools.h"
#include "PIUConstants.h"

//-------------------------------------------------------------------------------
// 	Platform specific includes.
//-------------------------------------------------------------------------------
#ifdef __PIMac__
	#include <Aliases.h>
	//#include <Files.h>
#endif

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//-------------------------------------------------------------------------------
// 	Constants.
//-------------------------------------------------------------------------------
const unsigned short kMacMaxFileSpecNameLength = 64;
// kMaxPathLength is defined in SPFiles.h.

//-------------------------------------------------------------------------------
// 	Prototypes.
//-------------------------------------------------------------------------------

// Return whether a file spec is valid or not:
bool PIUValidFileSpec
	(
	/* IN */		SPPlatformFileSpecification fileSpec
	);

// Reset a file spec to null and clear:
SPErr PIUClearFileSpec
	(
	/* IN/OUT */	SPPlatformFileSpecification* fileSpec
	);

// Force a file spec to a default filename, if not valid:
SPErr PIUForceFileSpec
	(
	/* IN */		const char* const fileName,
	/* IN/OUT */	SPPlatformFileSpecification* fileSpec
	);

// From a SweetPea file spec, find a displayable file name and return it:
SPErr PIUParseFilenameToDisplay
	(
	/* IN */	const SPPlatformFileSpecification fileSpec,
	/* OUT */	char displayString[kMaxStr255Len]
	);

// Record a PlatformFilePath into a descriptor from a SweetPea file spec:
SPErr PIURecordFileSpec
	(
	/* IN/OUT */	PIActionDescriptor 					descriptor,
	/* IN */		const DescriptorKeyID				key,
	/* IN */		const SPPlatformFileSpecification 	fileSpec
	);

// Read a PlatformFilePath out of a descriptor and restore the file spec
// for it:
SPErr PIURestoreFileSpec
	(
	/* IN */	PIActionDescriptor 					descriptor,
	/* IN */	const DescriptorKeyID				key,
	/* OUT */	SPPlatformFileSpecification*		fileSpec
	);
	
typedef long PIUFileRef_t;

// Open existing file or create if none.  Will return a file handle for
// windows, a file reference number for Mac, or 0 if failed.  Will even
// try to massage the fileSpec to at least create something:
PIUFileRef_t PIUFileOpen
	(
	/* IN/OUT */	SPPlatformFileSpecification* fileSpec
	);
	
typedef enum PIUFilePosition_t
	{
	kFromStart = 0,
	kFromPosition,
	kFromEnd
	} PIUFilePosition_t;
	
// Seek to a file position, relative to the PIUFilePosition:
SPErr PIUFileSeek
	(
	/* IN */	const PIUFileRef_t 		fileRefNum,
	/* IN */	const PIUFilePosition_t	relativePosition,
	/* IN */	const long				byteOffset
	);
	
// Write to file at current position, returning the count of
// how many bytes were written:
SPErr PIUFileWrite
	(
	/* IN */		const PIUFileRef_t		fileRefNum,
	/* IN */		const char* const		string,
	/* IN/OUT */	unsigned long*			inOutCount
	);
	
// Reads characters from file at current position, returning the
// count of how many bytes were read:
SPErr PIUFileRead
	(
	/* IN */		const PIUFileRef_t		fileRefNum,
	/* IN/OUT */	char*					string,
	/* IN/OUT */	unsigned long*			inOutCount
	);

// Closes an open file, resetting its ref number:
SPErr PIUFileClose
	(
	/* IN/OUT */	PIUFileRef_t*			fileRefNum
	);
	
//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
} // End of extern "C" block.
#endif // __cplusplus

//-------------------------------------------------------------------------------

#endif // __PIUFile_h__
