//-------------------------------------------------------------------------------
//
//	File:
//		PIUNotifyUtils.cpp
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
//		PIUNotifyUtils is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	12/14/1997	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#include "PIUNotifyUtils.h"

#ifdef __PIMac__

PIUNotifyUtils_t::PIUNotifyUtils_t()
	{
	// Intentionally left blank.
	}

PIUNotifyUtils_t::
	PIUNotifyUtils_t
	(
	const PIUNotifyUtils_t & source
	)
	{
	#pragma unused (source)
	// Intentionally left blank.
	}

PIUNotifyUtils_t & PIUNotifyUtils_t::operator=
	(
	const PIUNotifyUtils_t & source
	)
	{
	#pragma unused (source)
	return *this;
	}

QDGlobals* PIUNotifyUtils_t::GetQDGlobals (void)
{
	
	return (QDGlobals *) ((* (char **) SetCurrentA5 ()) -
						 (sizeof (QDGlobals) - sizeof (GrafPtr)));
	
}

void PIUNotifyUtils_t::SetArrowCursor (void)
{
	
	QDGlobals *qd = GetQDGlobals();
	
	SetCursor (&(qd->arrow));
	
}

#endif // __PIMac__
