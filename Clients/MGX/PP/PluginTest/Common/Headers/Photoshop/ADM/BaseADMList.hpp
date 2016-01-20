/*
 * Name:
 *	BaseADMList.hpp
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
 *	1.0.0 2/2/1998	DT	First version.
 *		Created by Darin Tomack.
 */

#ifndef __BaseADMList_hpp__
#define __BaseADMList_hpp__

/*
 * Includes
 */
 
#ifndef __IADMList_hpp__
#include "IADMList.hpp"
#endif

class IADMDrawer;
class IADMNotifier;
class IADMTracker;


/*
 * Base Class
 */

class BaseADMList : public IADMList
{
protected:
//	IADMList fList;
	ADMUserData fUserData;

public:	
	BaseADMList(IADMList list, ADMUserData data = nil);
	virtual ~BaseADMList();

	void SetUserData(ADMUserData userData);
	ADMUserData GetUserData();
	
//	IADMList GetList();
};


inline void BaseADMList::SetUserData(ADMUserData userData)
{
	fUserData = userData;
}

inline ADMUserData BaseADMList::GetUserData()
{
	return fUserData;
}

/*
inline IADMList BaseADMList::GetList()
{
	return fList;
}
*/
#endif
