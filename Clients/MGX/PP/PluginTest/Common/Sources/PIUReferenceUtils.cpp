//-------------------------------------------------------------------------------
//
//	File:
//		PIUReferenceUtils.cpp
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
//		PIUReferenceUtils is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	4/22/1998	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#include "PIUReferenceUtils.h"
#include "PIUSuites.h" // Include here to prevent circular errors.
	
//-------------------------------------------------------------------------------
//
//	Make/Free
//
//	This provides simpler Make and Free methods for references.
//
//-------------------------------------------------------------------------------
PIActionReference PIUReferenceUtils_t::Make(void)
	{
	PIActionReference reference = 0;
	
	if (sPSActionReference.IsValid())
		{
		SPErr error = sPSActionReference->Make(&reference);
		}
		
	return reference;
	}
	
void PIUReferenceUtils_t::Free(PIActionReference* reference)
	{
	if (sPSActionReference.IsValid() && reference != NULL && *reference != NULL)
		{
		SPErr error = sPSActionReference->Free(*reference);
		*reference = 0;
		}
	}

//-------------------------------------------------------------------------------
// end PIUReferenceUtils.cpp
