/*
 * Name:
 *	BaseADMHierarchyList.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Dialog Manager Entry Base Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 2/2/1998	DL	First version.
 *		Created by Darin Tomack.
 */

#ifndef __BaseADMHierarchyList_hpp__
#define __BaseADMHierarchyList_hpp__

/*
 * Includes
 */
 
#ifndef __IADMHierarchyList_hpp__
#include "IADMHierarchyList.hpp"
#endif

class IADMDrawer;
class IADMNotifier;
class IADMTracker;


/*
 * Base Class
 */

class BaseADMHierarchyList : public IADMHierarchyList
{
protected:
	ADMUserData fUserData;

public:	
	BaseADMHierarchyList(IADMHierarchyList list, ADMUserData data = nil);
	virtual ~BaseADMHierarchyList();

	void SetUserData(ADMUserData userData);
	ADMUserData GetUserData();
};


inline void BaseADMHierarchyList::SetUserData(ADMUserData userData)
{
	fUserData = userData;
}

inline ADMUserData BaseADMHierarchyList::GetUserData()
{
	return fUserData;
}

#endif
