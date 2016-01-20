/*
 * Name:
 *	BaseADMEntry.cpp
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
 *	1.0.1 2/2/1998	DT	Second version.
 *		Created by Darin Tomack.
 */

/*
 * Includes
 */

#include "BaseADMEntry.hpp"
#include "IADMDrawer.hpp"
#include "IADMNotifier.hpp"
#include "IADMTracker.hpp"
#include "IADMList.hpp"
#include "IADMItem.hpp"
#include "IASGlobalContext.hpp"

/*
 * Constructor
 */

BaseADMEntry::BaseADMEntry(IADMEntry entry, ADMUserData data)
: IADMEntry(entry),
  fUserData(data),
  fList(GetList())
{
	if (fEntry && fList)
	{
		sADMEntry->SetUserData(fEntry, this);
		sADMList->SetDrawProc(fList, DrawProc);
		sADMList->SetTrackProc(fList, TrackProc);
		sADMList->SetNotifyProc(fList, NotifyProc);
		sADMList->SetDestroyProc(fList, DestroyProc);
	}
}

BaseADMEntry::BaseADMEntry(IADMList list, ADMUserData entryUserData)
: IADMEntry(nil),
  fUserData(entryUserData),
  fList(list)
{
	if (fList){
		fEntry = sADMEntry->Create( fList );

		if (fEntry)
		{
			sADMEntry->SetUserData(fEntry, this);
			sADMList->SetDrawProc(fList, DrawProc);
			sADMList->SetTrackProc(fList, TrackProc);
			sADMList->SetNotifyProc(fList, NotifyProc);
			sADMList->SetDestroyProc(fList, DestroyProc);
		}
	}
}

/*
 * Destructor
 */

BaseADMEntry::~BaseADMEntry()
{
	if (sADMEntry)
	{
		SetUserData(nil);

		/* We can't do this since other entries depend on these
		   procs.

		sADMList->SetDrawProc(fList, nil);
		sADMList->SetTrackProc(fList, nil);
		sADMList->SetNotifyProc(fList, nil);
		sADMList->SetDestroyProc(fList, nil);
		*/

		sADMEntry->Destroy(*this);
	}
			
//	fEntry = nil;
}


ADMEntryRef BaseADMEntry::Create(ADMListRef list, ADMUserData data)								
{
	fList = list;
	fUserData = data;

	if (fList)
	{
		fEntry = sADMEntry->Create( fList );

		if (fEntry)
		{
			SetUserData(this);
			sADMList->SetDrawProc(fList, DrawProc);
			sADMList->SetTrackProc(fList, TrackProc);
			sADMList->SetNotifyProc(fList, NotifyProc);
			sADMList->SetDestroyProc(fList, DestroyProc);
		}
	}

	return fEntry;	
}


/*
 * Virtual Functions that call Defaults.
 */
	
ASErr BaseADMEntry::Init()
{
	return kNoErr;
}

void BaseADMEntry::Draw(IADMDrawer drawer)
{
	sADMEntry->DefaultDraw(fEntry, drawer);
}

ASBoolean BaseADMEntry::Track(IADMTracker tracker)
{
	return 	sADMEntry->DefaultTrack(fEntry, tracker);
}

void BaseADMEntry::Notify(IADMNotifier notifier)
{
	sADMEntry->DefaultNotify(fEntry, notifier);
}



/*
 * Static Procs that call Virtual Functions.
 */
	
ASErr ASAPI BaseADMEntry::InitProc(ADMEntryRef entry)
{
	IASGlobalContext globalContext;
	BaseADMEntry *admEntry = (BaseADMEntry *)sADMEntry->GetUserData(entry);
	
	admEntry->fEntry = entry;

	if (admEntry)
		return admEntry->Init();
	else
		return kBadParameterErr;
}

void ASAPI BaseADMEntry::DrawProc(ADMEntryRef entry, ADMDrawerRef drawer)
{
	IASGlobalContext globalContext;
	BaseADMEntry *admEntry = (BaseADMEntry *)sADMEntry->GetUserData(entry);
	
	if (admEntry)
		admEntry->BaseDraw(drawer);
}

ASBoolean ASAPI BaseADMEntry::TrackProc(ADMEntryRef entry, ADMTrackerRef tracker)
{
	IASGlobalContext globalContext;
	BaseADMEntry *admEntry = (BaseADMEntry *)sADMEntry->GetUserData(entry);
	
	if (admEntry)
		return admEntry->Track(tracker);
	else
		return false;
}

void ASAPI BaseADMEntry::NotifyProc(ADMEntryRef entry, ADMNotifierRef notifier)
{
	IASGlobalContext globalContext;
	BaseADMEntry *admEntry = (BaseADMEntry *)sADMEntry->GetUserData(entry);
	
	if (admEntry)
		admEntry->Notify(notifier);
}

void ASAPI BaseADMEntry::DestroyProc(ADMEntryRef entry)
{
	IASGlobalContext globalContext;
	BaseADMEntry *admEntry = (BaseADMEntry *)sADMEntry->GetUserData(entry);
	
	if (admEntry)
		delete admEntry;
}


void BaseADMEntry::BaseDraw(IADMDrawer drawer)
{
	Draw(drawer);
}

