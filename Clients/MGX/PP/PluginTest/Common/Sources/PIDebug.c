//-------------------------------------------------------------------------------
//
//	File:
//		PIDebug.c
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains resources for debugging
//		and error control for plug-ins.
//
//	Use:
//		Use these routines when for any general plug-in
//		debugging.  They're designed to not require a lot
//		of debugger interaction, but you should get familiar
//		with your debugger for serious bug stomping, which
//		these won't handle.
//
//	Version history:
//		Version 1.0.0	11/26/1997	Created for Photoshop 5.0.
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#include "PIDefines.h"

#ifdef _DEBUG

// Do this only if _DEBUG is defined.

#include "PIUtilities.h"
#include <stdlib.h>

#ifdef __PIMac__
	#include <Dialogs.h>
#else // Windows
	//
#endif

//-------------------------------------------------------------------------------
//
//	PIDebugStr
//	
//	Shows a string in a dialog for debugging.
//
//-------------------------------------------------------------------------------

// If there isn't a predefined AlertID resource, this will error when
// trying to compile.  The AlertID resource is created in DialogUtilities.r.

#ifdef AlertID

void _PIDebugStr
	( 
	const char * const errorString		// C string to display.
	)
	{
	#ifdef __PIMac__
		// Mac needs Pascal string.
		Handle alertHandle = GetResource('ALRT', AlertID);
		Str255 string = "";
		size_t size = PIstrlen(errorString);	

		if (size > 255)
			size = 255;

		PICopy(&string[1], errorString, size);

		string[0] = size;

		HNoPurge (alertHandle);
		
		CenterDialog ((DialogTHndl) alertHandle);
		ParamText(string, "\p", "\p", "\p");

		SetArrowCursor();
		CautionAlert (AlertID, nil);
		// I'm ignoring the result.

		HPurge (alertHandle);
		ReleaseResource(alertHandle);

	#elif defined(__PIWin__)
		char caption[60] = "Debug"; // Title
		
		MessageBox
			(
			NULL, 			// hDLLInstance
			errorString,
			caption,
			MB_APPMODAL | MB_ICONINFORMATION | MB_OK
			);

	#endif
	
	}
	
#endif // AlertID

//-------------------------------------------------------------------------------
//
//	PIDebugNum
//	
//	Outputs a dialog with a number and a description for that number.
//
//-------------------------------------------------------------------------------
void _PIDebugNum
	(
	const char * const tag,		// Description for number.
	const long value			// Value to output.
	)
	{
	Str255 source		= "_^0^1";
	Str255 tagstr		= "";
	Str255 valuestr		= "";
	
	const char * const debugstr = (char *)&source[1];
	
	size_t size = PIstrlen(tag);
	
	if (size > 255)
		size = 255; // max size.
	
	// Convert tag to Pascal:
	PICopy(&tagstr[1], tag, size);
	tagstr[0] = size;
	
	// Convert value to Pascal string:
	PINumberToString(value, valuestr);
	
	// Convert source to pascal string:
	source[0] = PIstrlen((char *)source)-1;

	// Update output string:
	PIParamText(source, tagstr, valuestr, NULL);
	
	// Send it on its way as CString:
	if (source[0] > 254)
		source[0] = 255;
		
	source[ source[0]+1 ] = '\0'; // null terminate
	PIDebugStr(debugstr);
	}	
	
//-------------------------------------------------------------------------------
//
//	PIAssert
//	
//	Compares a value and, if false, reports an error and exits.
//
//-------------------------------------------------------------------------------
void _PIAssert
	(
	const char * const condition,	// Usually precompiler #condition.
	const char * const file,		// Usually precompiler __FILE__.
	const unsigned long line		// Usually precompiler __LINE__.
	)
	{
	// Have to define string with hard hex because Windows doesn't
	// understand "\p":
	Str255 source 		= "_Assertion failed: ^0\r^1 @ line ^2";
	Str255 conditionstr	= "";
	Str255 filestr		= "";
	Str255 linestr		= "";

	const char * const debugstr = (char *)&source[1];

	size_t size = PIstrlen(file);
	
	// Convert file to Pascal:
	PICopy(&filestr[1], file, size);
	filestr[0] = size;
	
	// Convert condition to Pascal:
	size = PIstrlen(condition);	
	PICopy(&conditionstr[1], condition, size);
	conditionstr[0] = size;
	
	// Convert line to Pascal string:
	PINumberToString(line, linestr);
	
	// Convert source to pascal string:
	source[0] = PIstrlen((char *)source)-1;
	
	// Update output string:
	PIParamText(source, conditionstr, filestr, linestr);

	// Send it on its way as CString:
	if (source[0] > 254)
		source[0] = 255;
		
	source[ source[0]+1 ] = '\0'; // null terminate
	PIDebugStr(debugstr);
	
	// Exit:
	exit(1);
	}
	
//-------------------------------------------------------------------------------
#endif // _DEBUG was not defined.
