/*
 * Name:
 *	IADMIcon.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	IADMIcon Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 6/15/1996	DL	First version.
 *		Created by Paul Asente.
 */

#ifndef __IADMIcon_hpp__
#define __IADMIcon_hpp__

/*
 * Includes
 */
 
#ifndef __ADMIcon__
#include "ADMIcon.h"
#endif

/*
 * Global Suite Pointer
 */

extern ADMIconSuite *sADMIcon;

/*
 * Wrapper Class
 */

class IADMIcon
{
private:
	ADMIconRef fIcon;

public:	
	IADMIcon(ADMIconType type, int width, int height, void *data)
		{ fIcon = sADMIcon->Create(type, width, height, data); }

	IADMIcon(SPPluginRef pluginRef, int iconID, int iconIndex)
		{ fIcon = sADMIcon->GetFromResource(pluginRef, iconID, iconIndex); }

	~IADMIcon()						{ sADMIcon->Destroy(fIcon); }

	IADMIcon(ADMIconRef icon)		{ fIcon = icon; }

	operator ADMIconRef()			{ return fIcon; }

	ADMIconType GetType()			{ return sADMIcon->GetType(fIcon); }
	int GetWidth()					{ return sADMIcon->GetWidth(fIcon); }
	int GetHeight()					{ return sADMIcon->GetHeight(fIcon); }
	void *GetData()					{ return sADMIcon->GetData(fIcon); }
	ASBoolean IsFromResource()		{ return sADMIcon->IsFromResource(fIcon); }
};

#endif








