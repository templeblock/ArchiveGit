/*
 * Name:
 *	BaseADMEntry.hpp
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

#ifndef __BaseADMEntry_hpp__
#define __BaseADMEntry_hpp__

/*
 * Includes
 */
 
#ifndef __IADMEntry_hpp__
#include "IADMEntry.hpp"
#endif

#include "IADMList.hpp"

class IADMDrawer;
class IADMNotifier;
class IADMTracker;


/*
 * Base Class
 */

class BaseADMEntry : public IADMEntry
{
private:
	static ASErr ASAPI InitProc(ADMEntryRef entry);
	static void ASAPI DrawProc(ADMEntryRef entry, ADMDrawerRef drawer);
	static ASBoolean ASAPI TrackProc(ADMEntryRef entry, ADMTrackerRef tracker);
	static void ASAPI NotifyProc(ADMEntryRef entry, ADMNotifierRef notifier);
	static void ASAPI DestroyProc(ADMEntryRef entry);

protected:
//	IADMEntry fEntry;
	IADMList fList;
	ADMUserData fUserData;
	
	virtual ASErr Init();
	virtual void Draw(IADMDrawer drawer);
	virtual ASBoolean Track(IADMTracker tracker);
	virtual void Notify(IADMNotifier notifier);

	virtual void BaseDraw(IADMDrawer drawer);

public:	
	BaseADMEntry(IADMEntry entry, ADMUserData data = nil);
	BaseADMEntry(IADMList list, ADMUserData entryUserData = nil);
	virtual ~BaseADMEntry();

	virtual ADMEntryRef Create(ADMListRef list, ADMUserData data = nil);

	void SetUserData(ADMUserData userData);
	ADMUserData GetUserData();
	
//	IADMEntry GetEntry();

	ASErr DoInit();
};


inline void BaseADMEntry::SetUserData(ADMUserData userData)
{
	fUserData = userData;
}

inline ADMUserData BaseADMEntry::GetUserData()
{
	return fUserData;
}

/*
inline IADMEntry BaseADMEntry::GetEntry()
{
	return fEntry;
}
*/
inline ASErr BaseADMEntry::DoInit()
{ 
	return Init(); 
}

#endif
