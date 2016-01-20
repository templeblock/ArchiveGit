/*
 * Name:
 *	BaseADMItem.cpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Item Manager Item Interface Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.1 3/11/1996	DL	Second version.
 *		Created by Dave Lazarony.
 */

/*
 * Includes
 */

#include "BaseADMItem.hpp"
#include "IADMDrawer.hpp"
#include "IADMNotifier.hpp"
#include "IADMTracker.hpp"
#include "IASGlobalContext.hpp"

/*
 * Constructor
 */

BaseADMItem::BaseADMItem(IADMItem item, ADMUserData data)	
: IADMItem(item)							
{
	fUserData = data;

	if (fItem)
	{
		sADMItem->SetUserData(fItem, this);
		sADMItem->SetDrawProc(fItem, DrawProc);
		sADMItem->SetTrackProc(fItem, TrackProc);
		sADMItem->SetNotifyProc(fItem, NotifyProc);
		sADMItem->SetDestroyProc(fItem, DestroyProc);
		sADMItem->SetTextToFloatProc(fItem, TextToFloatProc);
		sADMItem->SetFloatToTextProc(fItem, FloatToTextProc);
	}
}


/*
 * Destructor
 */

BaseADMItem::~BaseADMItem()
{
	if (sADMItem)
	{
		sADMItem->SetUserData(fItem, nil);
			
		sADMItem->SetUserData(fItem, nil);
		sADMItem->SetDrawProc(fItem, nil);
		sADMItem->SetNotifyProc(fItem, nil);
		sADMItem->SetDestroyProc(fItem, nil);
		sADMItem->SetTextToFloatProc(fItem, nil);
		sADMItem->SetFloatToTextProc(fItem, nil);
		
		sADMItem->Destroy(fItem);
	}
			
	fItem = nil;
}


ADMItemRef BaseADMItem::Create(ADMDialogRef dialog, const IASRect &boundsRect, ADMItemType itemType, int itemID)								
{
	fItem = sADMItem->Create(dialog, itemID, itemType, (ASRect *)&boundsRect, InitProc, this);
	
	if (fItem)
	{
		sADMItem->SetDrawProc(fItem, DrawProc);
		sADMItem->SetTrackProc(fItem, TrackProc);
		sADMItem->SetNotifyProc(fItem, NotifyProc);
		sADMItem->SetDestroyProc(fItem, DestroyProc);
		sADMItem->SetTextToFloatProc(fItem, TextToFloatProc);
		sADMItem->SetFloatToTextProc(fItem, FloatToTextProc);
	}

	return fItem;	
}


/*
 * Virtual Functions that call Defaults.
 */
	
ASErr BaseADMItem::Init()
{
	return kNoErr;
}

void BaseADMItem::Draw(IADMDrawer drawer)
{
	sADMItem->DefaultDraw(fItem, drawer);
}

ASBoolean BaseADMItem::Track(IADMTracker tracker)
{
	return sADMItem->DefaultTrack(fItem, tracker);
}

void BaseADMItem::Notify(IADMNotifier notifier)
{
	sADMItem->DefaultNotify(fItem, notifier);
}

ASBoolean BaseADMItem::TextToFloat(const char *text, float &value)
{
	return sADMItem->DefaultTextToFloat(fItem, (char *)text, &value);
}

ASBoolean BaseADMItem::FloatToText(float value, char *text, int textLength)
{
	return sADMItem->DefaultFloatToText(fItem, value, text, textLength);
}


/*
 * Static Procs that call Virtual Functions.
 */
	
ASErr ASAPI BaseADMItem::InitProc(ADMItemRef item)
{
	IASGlobalContext globalContext;
	BaseADMItem *admItem = (BaseADMItem *)sADMItem->GetUserData(item);
	
	admItem->fItem = item;

	if (admItem)
		return admItem->Init();
	else
		return kBadParameterErr;
}

void ASAPI BaseADMItem::DrawProc(ADMItemRef item, ADMDrawerRef drawer)
{
	IASGlobalContext globalContext;
	BaseADMItem *admItem = (BaseADMItem *)sADMItem->GetUserData(item);
	
	if (admItem)
		admItem->BaseDraw(drawer);
}

ASBoolean ASAPI BaseADMItem::TrackProc(ADMItemRef item, ADMTrackerRef tracker)
{
	IASGlobalContext globalContext;
	BaseADMItem *admItem = (BaseADMItem *)sADMItem->GetUserData(item);
	
	if (admItem)
		return admItem->Track(tracker);
	else
		return false;
}

void ASAPI BaseADMItem::NotifyProc(ADMItemRef item, ADMNotifierRef notifier)
{
	IASGlobalContext globalContext;
	BaseADMItem *admItem = (BaseADMItem *)sADMItem->GetUserData(item);
	
	if (admItem)
		admItem->Notify(notifier);
}

void ASAPI BaseADMItem::DestroyProc(ADMItemRef item)
{
	IASGlobalContext globalContext;
	BaseADMItem *admItem = (BaseADMItem *)sADMItem->GetUserData(item);
	
	if (admItem)
		delete admItem;
}

ASBoolean ASAPI BaseADMItem::TextToFloatProc(ADMItemRef item, char *text, float *value)
{
	IASGlobalContext globalContext;
	BaseADMItem *admItem = (BaseADMItem *)sADMItem->GetUserData(item);
	
	if (admItem)
		return admItem->TextToFloat(text, *value);
	else
		return false;
}

ASBoolean ASAPI BaseADMItem::FloatToTextProc(ADMItemRef item, float value, char *text, int textLength)
{
	IASGlobalContext globalContext;
	BaseADMItem *admItem = (BaseADMItem *)sADMItem->GetUserData(item);
	
	if (admItem)
		return admItem->FloatToText(value, text, textLength);
	else
		return false;
}

void BaseADMItem::BaseDraw(IADMDrawer drawer)
{
	Draw(drawer);
}
