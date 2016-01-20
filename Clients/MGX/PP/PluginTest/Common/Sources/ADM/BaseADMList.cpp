/*
 * Name:
 *	BaseADMList.cpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe List Manager List Interface Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.1 1/26/1996	DT	Second version.
 *		Created by Darin Tomack.
 */

/*
 * Includes
 */

#include "BaseADMList.hpp"
#include "IADMDrawer.hpp"
#include "IADMNotifier.hpp"
#include "IADMTracker.hpp"
#include "IASGlobalContext.hpp"

/*
 * Constructor
 */

BaseADMList::BaseADMList(IADMList list, ADMUserData data) 
: IADMList(list)
{
	if (list)
	{
		sADMList->SetUserData(list, this);
	}
}


/*
 * Destructor
 */

BaseADMList::~BaseADMList()
{
	if (sADMList)
	{
		SetUserData(nil);
			
		/* Don't touch these. If this list class object
		   is deleted in the plug-in before associated ADMList
		   object is deleted in ADM, we'll need these procs
		   still set on that side of ADM to make sure any entry
		   data on this side is cleaned up.

		sADMList->SetUserData(fList, nil);
		sADMList->SetDrawProc(fList, nil);
		sADMList->SetNotifyProc(fList, nil);
		sADMList->SetDestroyProc(fList, nil);

		*/
	}
			
//	fList = nil;
}

