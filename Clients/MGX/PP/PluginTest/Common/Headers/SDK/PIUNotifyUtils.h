//-------------------------------------------------------------------------------
//
//	File:
//		PIUNotifyUtils.h
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
//		PIUNotifyUtils is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	12/14/1997	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#ifndef __PIUNotifyUtils_h__
#define __PIUNotifyUtils_h__

#include "PIDefines.h"

#ifdef __PIMac__

#include <Quickdraw.h>
#include <OSUtils.h>

//-------------------------------------------------------------------------------
//	Macintosh Notifier Utilities.
//-------------------------------------------------------------------------------
class PIUNotifyUtils_t
	{
	public:
		// Constructor:
		PIUNotifyUtils_t();
		
		// Copy constructor:
		PIUNotifyUtils_t
			(
			const PIUNotifyUtils_t & source
			);
		
		// Assignment operator:
		PIUNotifyUtils_t & operator= 
			(
			const PIUNotifyUtils_t & source
			);
			
		// Destructor:
		~PIUNotifyUtils_t() {};	// Specified as nothing.
		
		// Get QuickDraw globals:
		QDGlobals* GetQDGlobals();

		// Use Quickdraw globals to get arrow cursor:
		void SetArrowCursor();
		
	private:
		// None.
	
	protected:
		// None.
	};

//-------------------------------------------------------------------------------

#endif // __PIMac__

#endif // __PIUNotifyUtils_h__
