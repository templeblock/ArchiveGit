/*
 * Name:
 *	BaseADMListEntry.cpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Dialog Manager List Entry Implementation.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 2/2/1998	DT	First version.
 *		Created by Darin Tomack.
 */

/*
 * Includes
 */

#include "BaseADMListEntry.hpp"
#include "IADMDrawer.hpp"
#include "IADMNotifier.hpp"
#include "IADMTracker.hpp"
#include "IADMList.hpp"
#include "IADMItem.hpp"
#include "IASGlobalContext.hpp"

/*
 * Constructor
 */

BaseADMListEntry::BaseADMListEntry(IADMListEntry entry, ADMUserData data)
: IADMListEntry(entry),
  fUserData(data),
  fHierarchyList(GetList())
{
	if (fEntry && fHierarchyList)
	{
		sADMListEntry->SetUserData(fEntry, this);
		sADMHierarchyList->SetDrawProc(fHierarchyList, DrawProc);
		sADMHierarchyList->SetTrackProc(fHierarchyList, TrackProc);
		sADMHierarchyList->SetNotifyProc(fHierarchyList, NotifyProc);
		sADMHierarchyList->SetDestroyProc(fHierarchyList, DestroyProc);
	}
}

BaseADMListEntry::BaseADMListEntry(IADMHierarchyList list, ADMUserData entryUserData)
: IADMListEntry(nil),
  fUserData(entryUserData),
  fHierarchyList(list)
{
	if (fHierarchyList){
		fEntry = sADMListEntry->Create( fHierarchyList );

		if (fEntry)
		{
			sADMListEntry->SetUserData(fEntry, this);
			sADMHierarchyList->SetDrawProc(fHierarchyList, DrawProc);
			sADMHierarchyList->SetTrackProc(fHierarchyList, TrackProc);
			sADMHierarchyList->SetNotifyProc(fHierarchyList, NotifyProc);
			sADMHierarchyList->SetDestroyProc(fHierarchyList, DestroyProc);
		}
	}
}

/*
 * Destructor
 */

BaseADMListEntry::~BaseADMListEntry()
{
	if (sADMListEntry)
	{
		SetUserData(nil);

		/* We can't do this since other entries depend on these
		   procs.

		sADMHierarchyList->SetDrawProc(fHierarchyList, nil);
		sADMHierarchyList->SetTrackProc(fHierarchyList, nil);
		sADMHierarchyList->SetNotifyProc(fHierarchyList, nil);
		sADMHierarchyList->SetDestroyProc(fHierarchyList, nil);
		*/

		sADMListEntry->Destroy(*this);
	}
			
//	fEntry = nil;
}


ADMListEntryRef BaseADMListEntry::Create(ADMHierarchyListRef list, ADMUserData data)								
{
	fHierarchyList = list;
	fUserData = data;

	if (fHierarchyList)
	{
		fEntry = sADMListEntry->Create( fHierarchyList );

		if (fEntry)
		{
			SetUserData(this);
			sADMHierarchyList->SetDrawProc(fHierarchyList, DrawProc);
			sADMHierarchyList->SetTrackProc(fHierarchyList, TrackProc);
			sADMHierarchyList->SetNotifyProc(fHierarchyList, NotifyProc);
			sADMHierarchyList->SetDestroyProc(fHierarchyList, DestroyProc);
		}
	}

	return fEntry;	
}


/*
 * Virtual Functions that call Defaults.
 */
	
ASErr BaseADMListEntry::Init()
{
	return kNoErr;
}

void BaseADMListEntry::Draw(IADMDrawer drawer)
{
	sADMListEntry->DefaultDraw(fEntry, drawer);
}

ASBoolean BaseADMListEntry::Track(IADMTracker tracker)
{
	return 	sADMListEntry->DefaultTrack(fEntry, tracker);
}

void BaseADMListEntry::Notify(IADMNotifier notifier)
{
	sADMListEntry->DefaultNotify(fEntry, notifier);
}



/*
 * Static Procs that call Virtual Functions.
 */
	
ASErr ASAPI BaseADMListEntry::InitProc(ADMListEntryRef entry)
{
	IASGlobalContext globalContext;
	BaseADMListEntry *admEntry = (BaseADMListEntry *)sADMListEntry->GetUserData(entry);
	
	admEntry->fEntry = entry;

	if (admEntry)
		return admEntry->Init();
	else
		return kBadParameterErr;
}

void ASAPI BaseADMListEntry::DrawProc(ADMListEntryRef entry, ADMDrawerRef drawer)
{
	IASGlobalContext globalContext;
	BaseADMListEntry *admEntry = (BaseADMListEntry *)sADMListEntry->GetUserData(entry);
	
	if (admEntry)
		admEntry->BaseDraw(drawer);
}

ASBoolean ASAPI BaseADMListEntry::TrackProc(ADMListEntryRef entry, ADMTrackerRef tracker)
{
	IASGlobalContext globalContext;
	BaseADMListEntry *admEntry = (BaseADMListEntry *)sADMListEntry->GetUserData(entry);
	
	if (admEntry)
		return admEntry->Track(tracker);
	else
		return false;
}

void ASAPI BaseADMListEntry::NotifyProc(ADMListEntryRef entry, ADMNotifierRef notifier)
{
	IASGlobalContext globalContext;
	BaseADMListEntry *admEntry = (BaseADMListEntry *)sADMListEntry->GetUserData(entry);
	
	if (admEntry)
		admEntry->Notify(notifier);
}

void ASAPI BaseADMListEntry::DestroyProc(ADMListEntryRef entry)
{
	IASGlobalContext globalContext;
	BaseADMListEntry *admEntry = (BaseADMListEntry *)sADMListEntry->GetUserData(entry);
	
	if (admEntry)
		delete admEntry;
}


void BaseADMListEntry::BaseDraw(IADMDrawer drawer)
{
	Draw(drawer);
}
