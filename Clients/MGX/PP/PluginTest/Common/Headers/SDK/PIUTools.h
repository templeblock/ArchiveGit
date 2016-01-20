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
//		helpful common plug-in functions. 
//
//	Use:
//		PIUBasic is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//		Most expect A4-globals to be set-up already, for
//		gStuff to be a valid pointer to your global structure's
//		parameter block, and, on Windows, for hDllInstance to
//		be a global handle reference to your plug-in DLL.
//
//	Version history:
//		Version 1.0.0	7/31/1997	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------
 
#ifndef __PIUTools_h__	// Defined yet?
#define __PIUTools_h__	// Only define once.

#include <stddef.h>
#include <limits.h>
#include "PITypes.h"
#include "PIDefines.h"
#include "SPTypes.h"
#include "PIUConstants.h"				// kMaxStr255Len declaration.
#include "PIActions.h"

#ifdef __PIMac__
	#include <TextUtils.h>
#endif

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//-------------------------------------------------------------------------------
//	Structures.
//-------------------------------------------------------------------------------

// How about a rectangle that can take floating point numbers?  Can be
// useful for using for offsets and things like that...
typedef struct PIUDRect_t
	{
	double	top;
	double	left;
	double	bottom;
	double	right;
	} PIUDRect_t;
	
// How about a point that can take floating point numbers?
typedef struct PIUDPoint_t
	{
	double	h;
	double	v;
	} PIUDPoint_t;

//-------------------------------------------------------------------------------
//	Utility functions.
//-------------------------------------------------------------------------------

const unsigned char kConvertStrLen = 33; // Minimum string length for conversions.

// Convert a long to a c string.  C string must be a minimum length:
SPErr PIULongToCString
	(
	/* IN */	const long value,
	/* OUT */	char outString[kConvertStrLen]
	);
	
// Rarely need more than this for precision, but you can
// specify it directly with a number from 0...255:
const unsigned char kPIUNoPrecision = 0;
const unsigned char kPIUSinglePrecision = 1;
const unsigned char kPIUDoublePrecision = 2;
const unsigned char kPIUQuadPrecision = 4;

// Convert a double to a C string.
SPErr PIUDoubleToCString
	(
	/* IN */	const double value,
	/* IN */	const unsigned char precision,
	/* OUT */	char outString[kConvertStrLen]
	);

// Sticks a null in the target character:
inline void PIUNullTerminate(char *c) { *c = '\0'; }

// Raise a base to an exponent power:
double PIUExponent
	(
	/* IN */	const unsigned long base, 
	/* IN */	const unsigned short raise
	);

// String size functions:
unsigned short PIUstrlen (const char *inString);

// Copy bytes from a source to a target:
SPErr PIUCopy
	(
	/* OUT */	void* target,
	/* IN */	const void* const source,
	/* IN */	const unsigned short length
	);

// Case sensitive values for match function:
typedef enum caseSensitive_t
	{
	kIgnoreCase = true,
	kCaseSensitive = false
	} caseSensitive_t;

// String match functions.  Returns with either noMatch, gotMatch, or
// match at char (short):
short _PIUMatch
	(
	const char *inSearch, 
	const char *inTarget, 
	const caseSensitive_t ignoreCase, 
	const unsigned short inLength
	);

// Pascal string version:
short _PIUStringMatch
	(
	Str255 s1, 
	const char* s2,
	const caseSensitive_t ignoreCase
	);

// Return values for match function:
const short kNoMatch = -1;
const short kGotMatch = 0;

// Returns how many hits of target occur in source:
unsigned short PIUCountHits
	(
	/* IN */	const char* const source,
	/* IN */	const char* const target,
	/* IN */	const caseSensitive_t ignoreCase
	);

// Append a series of chars onto the end of a null-terminated block:
SPErr PIUAppendBlock
	(
	/* IN/OUT */	char* block,
	/* IN */		const char* const append,
	/* IN */		const unsigned short append_size
	);

// Replace every occurrence of target with a replace string in
// null-terminated character block:
SPErr PIUReplaceCharsInBlock
	(
	/* IN */		const char* const target,
	/* IN */		const char* const replace,
	/* IN */		const caseSensitive_t ignoreCase,
	/* IN/OUT */	char** block
	);
		
// Convert 4 characters to unsigned long:
unsigned long PIUCharToID (const char *inChars);

// Convert unsigned long to 4 characters:
void PIUIDToChar (const unsigned long inType, /* OUT */ char *outChars);


// One character Upper to Lowercase:
char PIUMakeLower(const char inChar);

#define PIUMatch(inSearch, inTarget) \
	_PIUMatch(inSearch, inTarget, kCaseSensitive, PIUstrlen(inSearch))
	
#define PIUSMatch(inSearch, inTarget) \
	_PIUStringMatch(inSearch, inTarget, kCaseSensitive)

char* PIUCopyStringToBlock
	(
	const char* const string,
	const unsigned short size
	);

char * PIUGetString(long stringID);

double PIUFixed16ToDouble (const unsigned long inValue);

unsigned long PIUDoubleToFixed16 (const double inValue);

// Pin a double floating point number to a minimum or maximum value:
void PIUPinDouble
	(
	/* IN/OUT */	double*			value,
	/* IN */		const double	min,
	/* IN */		const double 	max
	);

// Get all properties of a particular class.  Common ones are
// classApplication, classDocument, classLayer, classChannel,
// classAction, classActionSet:
PIActionDescriptor PIUGetAllKeysForClass(const DescriptorClassID classID);
PIActionDescriptor PIUGetAllKeysForReference(const PIActionReference reference);

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
} // End of extern "C" block.
#endif // __cplusplus

//-------------------------------------------------------------------------------

#endif // __PIUTools_H__
