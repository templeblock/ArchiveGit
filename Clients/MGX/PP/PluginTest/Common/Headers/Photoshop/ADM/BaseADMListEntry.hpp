/*
 * Name:
 *	BaseADMListEntry.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Dialog Manager Hier. List Entry Base Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 2/2/1998	First version.
 *		Created by Darin Tomack.
 */

#ifndef __BaseADMListEntry_hpp__
#define __BaseADMListEntry_hpp__

/*
 * Includes
 */
 
#ifndef __IADMEntry_hpp__
#include "IADMListEntry.hpp"
#endif

#include "IADMHierarchyList.hpp"

class IADMDrawer;
class IADMNotifier;
class IADMTracker;


/*
 * Base Class
 */

class BaseADMListEntry : public IADMListEntry
{
private:
	static ASErr ASAPI InitProc(ADMListEntryRef entry);
	static void ASAPI DrawProc(ADMListEntryRef entry, ADMDrawerRef drawer);
	static ASBoolean ASAPI TrackProc(ADMListEntryRef entry, ADMTrackerRef tracker);
	static void ASAPI NotifyProc(ADMListEntryRef entry, ADMNotifierRef notifier);
	static void ASAPI DestroyProc(ADMListEntryRef entry);

protected:
	IADMHierarchyList fHierarchyList;
	ADMUserData fUserData;
	
	virtual ASErr Init();
	virtual void Draw(IADMDrawer drawer);
	virtual ASBoolean Track(IADMTracker tracker);
	virtual void Notify(IADMNotifier notifier);

	virtual void BaseDraw(IADMDrawer drawer);

public:	
	BaseADMListEntry(IADMListEntry entry, ADMUserData data = nil);
	BaseADMListEntry(IADMHierarchyList list, ADMUserData entryUserData = nil);
	virtual ~BaseADMListEntry();

	virtual ADMListEntryRef Create(ADMHierarchyListRef list, ADMUserData data = nil);

	void SetUserData(ADMUserData userData);
	ADMUserData GetUserData();
	
	ASErr DoInit();
};


inline void BaseADMListEntry::SetUserData(ADMUserData userData)
{
	fUserData = userData;
}

inline ADMUserData BaseADMListEntry::GetUserData()
{
	return fUserData;
}

inline ASErr BaseADMListEntry::DoInit()
{ 
	return Init(); 
}

#endif // __BaseADMListEntry_hpp__
