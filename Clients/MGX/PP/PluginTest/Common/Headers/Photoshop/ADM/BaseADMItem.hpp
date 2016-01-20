/*
 * Name:
 *	BaseADMItem.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Dialog Manager Item Base Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/6/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __BaseADMItem_hpp__
#define __BaseADMItem_hpp__

/*
 * Includes
 */
 
#ifndef __IADMItem_hpp__
#include "IADMItem.hpp"
#endif

#ifndef __IADMDialog_hpp__
#include "IADMDialog.hpp"
#endif

class IADMDrawer;
class IADMNotifier;
class IADMTracker;


/*
 * Base Class
 */

class BaseADMItem : public IADMItem
{
private:
	static ASErr ASAPI InitProc(ADMItemRef item);
	static void ASAPI DrawProc(ADMItemRef item, ADMDrawerRef drawer);
	static ASBoolean ASAPI TrackProc(ADMItemRef item, ADMTrackerRef tracker);
	static void ASAPI NotifyProc(ADMItemRef item, ADMNotifierRef notifier);
	static void ASAPI DestroyProc(ADMItemRef item);
	static ASBoolean ASAPI TextToFloatProc(ADMItemRef item, char *text, float *value);
	static ASBoolean ASAPI FloatToTextProc(ADMItemRef item, float value, char *text, int textLength);

protected:
	ADMUserData fUserData;
	
	virtual ASErr Init();
	virtual void Draw(IADMDrawer drawer);
	virtual ASBoolean Track(IADMTracker tracker);
	virtual void Notify(IADMNotifier notifier);
	virtual ASBoolean TextToFloat(const char *text, float &value);
	virtual ASBoolean FloatToText(float value, char *text, int textLength);

	virtual void BaseDraw(IADMDrawer drawer);

public:	
	BaseADMItem(IADMItem item, ADMUserData data = nil);
	
	virtual ~BaseADMItem();

	virtual ADMItemRef Create(ADMDialogRef dialog, const IASRect &boundsRect, ADMItemType itemType = kADMUserType, int itemID = kADMUniqueItemID);

	void SetUserData(ADMUserData userData);
	ADMUserData GetUserData();
	
	IADMItem GetItem();

	ASErr DoInit();
};

inline void BaseADMItem::SetUserData(ADMUserData userData)
{
	fUserData = userData;
}

inline ADMUserData BaseADMItem::GetUserData()
{
	return fUserData;
}

inline IADMItem BaseADMItem::GetItem()
{
	return fItem;
}

inline ASErr BaseADMItem::DoInit()
{ 
	return Init(); 
}

#endif
