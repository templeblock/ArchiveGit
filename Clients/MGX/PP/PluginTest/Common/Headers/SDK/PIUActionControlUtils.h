//-------------------------------------------------------------------------------
//
//	File:
//		PIUActionControlUtils.h
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
//		PIUActionControlUtils is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	12/14/1997	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#ifndef __PIUActionControlUtils_h__
#define __PIUActionControlUtils_h__

#include "PIActions.h"

//-------------------------------------------------------------------------------
//	Macintosh Notifier Utilities.
//-------------------------------------------------------------------------------
class PIUActionControlUtils_t
	{
	public:
		// Constructor:
		PIUActionControlUtils_t()
			{
			// Intentionally left blank.
			}
		
		// Destructor:
		~PIUActionControlUtils_t()
			{
			// Intentionally left blank.
			}
		
		// Takes a string ID and resolves it to its 4-char long ID or
		// runtime ID:
		DescriptorTypeID ResolveStringID (const char* const idString);		
		
		// Takes a 4-character ID and resolves it to its string or
		// 4-char string.  It is the callers responsibility to
		// dispose the string:
		char* ResolveTypeID (DescriptorTypeID id);
		
		// Takes a string and parses it for 'key_' or "UUID_String" and
		// returns only the characters of interest.  It is the callers
		// responsibility to dispose the string:
		char* ParseStringID (const char* const source);
		
	private:
		// None.
	
	protected:
		// None.
	};

//-------------------------------------------------------------------------------

#endif // __PIUActionControlUtils_h__
