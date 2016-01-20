/*
 * Name:
 *	IADMNotifier.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	IADMNotifier Wrapper Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/6/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __IADMNotifier_hpp__
#define __IADMNotifier_hpp__

/*
 * Includes
 */
 
#ifndef __ADMNotifier__
#include "ADMNotifier.h"
#endif

/*
 * Global Suite Pointer
 */

extern "C" ADMNotifierSuite *sADMNotifier;

/*
 * Wrapper Class
 */

class IADMNotifier
{
private:
	ADMNotifierRef fNotifier;

public:	
	IADMNotifier() 												{}
	IADMNotifier(ADMNotifierRef notifier) 						{ fNotifier = notifier; }
	
	operator ADMNotifierRef() 									{ return fNotifier; }
	
	ADMItemRef GetItem()										{ return sADMNotifier->GetItem(fNotifier); }	
	ADMDialogRef GetDialog()									{ return sADMNotifier->GetDialog(fNotifier); }

	ASBoolean IsNotifierType(const char *notifierType)			{ return sADMNotifier->IsNotifierType(fNotifier, (char *)notifierType); }

	void GetNotifierType(char *notifierType, int maxLength = 0)	{ sADMNotifier->GetNotifierType(fNotifier, notifierType, maxLength); }

};

#endif
