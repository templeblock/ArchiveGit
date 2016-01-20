/*
 * Name:
 *	BaseADMHierarchyList.cpp
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
 *	1.0.1 2/2/1998	DL	Second version.
 *		Created by Darin Tomack.
 */

/*
 * Includes
 */

#include "BaseADMHierarchyList.hpp"
#include "IASGlobalContext.hpp"

/*
 * Constructor
 */

BaseADMHierarchyList::BaseADMHierarchyList(IADMHierarchyList list, ADMUserData data)								
{
	fList = list;
	fUserData = data;

	if (fList)
	{
		sADMHierarchyList->SetUserData(fList, this);
	}
}


/*
 * Destructor
 */

BaseADMHierarchyList::~BaseADMHierarchyList()
{
	if (sADMHierarchyList)
	{
		sADMHierarchyList->SetUserData(fList, nil);
			
		/* Don't touch these. If this list class object
		   is deleted in the plug-in before associated ADMList
		   object is deleted in ADM, we'll need these procs
		   still set on that side of ADM to make sure any entry
		   data on this side is cleaned up.

		sADMHierarchyList->SetUserData(fList, nil);
		sADMHierarchyList->SetDrawProc(fList, nil);
		sADMHierarchyList->SetNotifyProc(fList, nil);
		sADMHierarchyList->SetDestroyProc(fList, nil);

		*/
	}
			
	fList = nil;
}

