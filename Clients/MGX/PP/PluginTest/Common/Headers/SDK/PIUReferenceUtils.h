//-------------------------------------------------------------------------------
//
//	File:
//		PIUReferenceUtils.h
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
//		PIUReferenceUtils is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	4/14/1998	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#ifndef __PIUReferenceUtils_h__
#define __PIUReferenceUtils_h__

#include "PIActions.h"

//-------------------------------------------------------------------------------
//	Reference Utilities.
//-------------------------------------------------------------------------------
class PIUReferenceUtils_t
	{
	public:
		// Constructor:
		PIUReferenceUtils_t()
			{
			// Intentionally left blank.
			}
		
		// Destructor:
		~PIUReferenceUtils_t()
			{
			// Intentionally left blank.
			}
		
		// Simpler methods for making and freeing a reference:
		PIActionReference Make(void);
		void Free(PIActionReference* reference);
					
	private:
		// None.
	
	protected:
		// None.
	};

//-------------------------------------------------------------------------------

#endif // __PIUReferenceUtils_h__
