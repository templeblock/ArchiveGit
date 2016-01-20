//-------------------------------------------------------------------------------
//
//	File:
//		PIUActionControlUtils.cpp
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the source and
//		routines to simplify the use of suites and also some
//		helpful common plug-in functions. 
//
//	Use:
//		PIUActionControlUtils is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	4/5/1998	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#include "PIUActionControlUtils.h"

#include "PIUSuites.h"

//-------------------------------------------------------------------------------
//
//	PIUActionControlUtils_t::ResolveID
//
//	Resolves string IDs that could be either 4-character longs (in string form)
//	or actual UUID strings.  Returns runtime ID or standard ID.
//
//	Resolves runtime IDs that could either be 4-character actual longs or
//	4-character runtime-assigned IDs.  Returns 4-characters as string or
//	UUID.
//
//-------------------------------------------------------------------------------

DescriptorTypeID PIUActionControlUtils_t::ResolveStringID (const char* const idString)
	{
	DescriptorTypeID id = typeNull;
	
	SPErr error = kSPNoError;
	
	const unsigned short length = PIUstrlen(idString);
	
	if (length > 0)
		{
		// We'll leave it eventNull if its a null string.
		
		if (length <= 4)
			{
			// We're not going to take any chances, in the off case where
			// the incoming idString is less than 4 characters, we don't
			// want to trash memory by accessing past the end of the string.
			// So we'll copy it over to a 33 length string and put nulls
			// for 5 characters.  (Trailing null is always good.)
			char convertString[kConvertStrLen] = "";
			PIUCopy(convertString, idString, length);
			for (unsigned short loop = length; loop < 5; loop++)
				convertString[loop] = '\0';
				
			// Assume its already a real runtime id and just convert it.
			id = PIUCharToID(convertString);
			}
		else
			{
			// Assume its a true UUID string and find its runtime ID:
			if (sPSActionControl.IsValid())
				{
				// Cool, we're an automation plug-in with the control
				// suite.  Use that:
				error = sPSActionControl->StringIDToTypeID
					(
					(char*)idString,
					&id
					);
				}
			else if (sPSBasicActionControl.IsValid())
				{
				// Okay, we're a different kind of plug-in, but we should at
				// least have the basic control suite:
				error = sPSBasicActionControl->StringIDToTypeID
					(
					(char*)idString,
					&id
					);
				}
			}
		}
		
	return id;
	
	} // ResolveID (string->runtime)
		
char* PIUActionControlUtils_t::ResolveTypeID (DescriptorTypeID id)
	{
	char* stringID = NULL;	
	
	SPErr error = kSPNoError;
	 
	char idString[kMaxStr255Len] = "";
	
	if (sPSActionControl.IsValid())
		{
		// Cool, we have the control suite, which means we're in an
		// automation plug-in.
		
		error = sPSActionControl->TypeIDToStringID
			(
			id,
			idString,
			kMaxStr255Len
			);
		}
	else if (sPSBasicActionControl.IsValid())
		{
		// Okay, we have the basic control suite, which is available
		// to all plug-ins.  Use that.
		
		error = sPSBasicActionControl->TypeIDToStringID
			(
			id,
			idString,
			kMaxStr255Len
			);
			
		}
	else
		{
		// The suites aren't available.  Convert the id in place.
		PIUIDToChar(id, idString);
		}
	
	if (error != kSPNoError)
		{
		// Couldn't find the ID.  Convert in place.
		PIUIDToChar(id, idString);
		}
		
	// Now create a heap-allocated string we can return:
	
	const unsigned short kIdString_size = PIUstrlen(idString);
	
	stringID = PIUCopyStringToBlock
		(
		idString,
		kIdString_size
		);
		
	return stringID;
	
	} // end ResolveID (runtime->string)

//-------------------------------------------------------------------------------
//
//	PIUActionControlUtils_t::ParseStringID
//
//	Parses a string for a 'key_', or "UUID_String", and returns just the
//	interesting chars.  It is the callers responsibility to dispose the
//	string that is returned.
//
//-------------------------------------------------------------------------------
char* PIUActionControlUtils_t::ParseStringID (const char* const source)
	{
	char* result = NULL;
	
	const unsigned short length = PIUstrlen(source);
	
	const short startSingleQuote = PIUMatch(source, "\'");
	const short startDoubleQuote = PIUMatch(source, "\"");
	
	short endSingleQuote = kNoMatch;
	
	if (startSingleQuote+1 < length && startSingleQuote != kNoMatch)
		endSingleQuote = PIUMatch(&source[startSingleQuote+1], "\'");
	
	short endDoubleQuote = kNoMatch;
	
	if (startDoubleQuote+1 < length && startDoubleQuote != kNoMatch)
		endDoubleQuote = PIUMatch(&source[startDoubleQuote+1], "\"");
		
	if (startSingleQuote != kNoMatch && endSingleQuote != kNoMatch)
		{
		// Have a valid single quote.  Compare to double quote:
		if (startDoubleQuote != kNoMatch && endDoubleQuote != kNoMatch)
			{
			// Have a valid double quote.  Find first one in:
			if (startSingleQuote < startDoubleQuote)
				{
				// Single quote is first in.  Grab that chunk.
				result = PIUCopyStringToBlock
					(
					&source[startSingleQuote+1], 
					endSingleQuote
					);
				}
			else
				{
				// Double quote came in first!
				result = PIUCopyStringToBlock
					(
					&source[startDoubleQuote+1],
					endDoubleQuote
					);
				}
			}
		else
			{
			// double quotes weren't even valid.
			result = PIUCopyStringToBlock
				(
				&source[startSingleQuote+1], 
				endSingleQuote
				);
			}
		}
	else
		{
		// Single quote wasn't even valid.
		if (startDoubleQuote != kNoMatch && endDoubleQuote != kNoMatch)
			{
			result = PIUCopyStringToBlock
				(
				&source[startDoubleQuote+1],
				endDoubleQuote
				);
				
			}
		else
			{
			// We got no quotes, so just copy the whole thing.
			result = PIUCopyStringToBlock
				(
				source,
				length
				);
			}
		}

	return result;
	}


//-------------------------------------------------------------------------------

// end PIUActionControlUtils.cpp
