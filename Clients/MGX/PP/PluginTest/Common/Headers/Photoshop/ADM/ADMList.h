/*
 * Name:
 *	ADMList.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM List Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.2 5/7/1996	DL	Second version.
 *		Created by Dave Lazarony.
 */

#ifndef __ADMList__
#define __ADMList__

/*
 * Includes
 */
 
#ifndef __ADMTypes__
#include "ADMTypes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma PRAGMA_ALIGN_BEGIN
#pragma PRAGMA_IMPORT_BEGIN


/*
 * Constants
 */

#define kADMListSuite					"ADM List Suite"
#define kADMListSuiteVersion1			1
#define kADMListSuiteVersion			kADMListSuiteVersion1


/*
 * Types
 */

/* ADMListRef and ADMEntryRef are declared in ADMTypes.h */

typedef ASErr ASAPI (*ADMEntryInitProc)(ADMEntryRef entry);
typedef void ASAPI (*ADMEntryDrawProc)(ADMEntryRef entry, ADMDrawerRef drawer);
typedef ASBoolean ASAPI (*ADMEntryTrackProc)(ADMEntryRef entry, ADMTrackerRef tracker);
typedef void ASAPI (*ADMEntryNotifyProc)(ADMEntryRef entry, ADMNotifierRef notifier);
typedef void ASAPI (*ADMEntryDestroyProc)(ADMEntryRef entry);
	
	
/*
 * List Suite
 */
typedef struct ADMListSuite1
{
	void ASAPI (*SetMenuID)(ADMListRef list, int menuResID);
	int ASAPI (*GetMenuID)(ADMListRef list);
	
	ADMItemRef ASAPI (*GetItem)(ADMListRef list);
	
	void ASAPI (*SetUserData)(ADMListRef list, ADMUserData data);
	ADMUserData ASAPI (*GetUserData)(ADMListRef list);

	void ASAPI (*SetInitProc)(ADMListRef list, ADMEntryInitProc initProc);
	ADMEntryInitProc ASAPI (*GetInitProc)(ADMListRef list);
	
	void ASAPI (*SetDrawProc)(ADMListRef list, ADMEntryDrawProc drawProc);
	ADMEntryDrawProc ASAPI (*GetDrawProc)(ADMListRef list);
	
	void ASAPI (*SetTrackProc)(ADMListRef list, ADMEntryTrackProc trackProc);
	ADMEntryTrackProc ASAPI (*GetTrackProc)(ADMListRef list);
	
	void ASAPI (*SetNotifyProc)(ADMListRef list, ADMEntryNotifyProc notifyProc);
	ADMEntryNotifyProc ASAPI (*GetNotifyProc)(ADMListRef list);
	
	void ASAPI (*SetDestroyProc)(ADMListRef list, ADMEntryDestroyProc destroyProc);
	ADMEntryDestroyProc ASAPI (*GetDestroyProc)(ADMListRef list);
	
	void ASAPI (*SetEntryWidth)(ADMListRef list, int width);
	int ASAPI  (*GetEntryWidth)(ADMListRef list);
	
	void ASAPI (*SetEntryHeight)(ADMListRef list, int height);
	int ASAPI  (*GetEntryHeight)(ADMListRef list);
	
	void ASAPI (*SetEntryTextRect)(ADMListRef list, ASRect *rect);
	void ASAPI  (*GetEntryTextRect)(ADMListRef list, ASRect *rect);
	
	ADMEntryRef ASAPI (*InsertEntry)(ADMListRef list, int index);
	void ASAPI (*RemoveEntry)(ADMListRef list, int index);
	
	ADMEntryRef ASAPI (*GetEntry)(ADMListRef list, int entryID);
	
	ADMEntryRef ASAPI (*IndexEntry)(ADMListRef list, int index);
	
	ADMEntryRef ASAPI (*FindEntry)(ADMListRef list, char *text);
	
	ADMEntryRef ASAPI (*PickEntry)(ADMListRef list, ASPoint *point);
	
	ADMEntryRef ASAPI (*GetActiveEntry)(ADMListRef list);
	
	ADMEntryRef ASAPI (*IndexSelectedEntry)(ADMListRef list, int selectionIndex);
	
	int ASAPI (*NumberOfEntries)(ADMListRef list);
	
	int ASAPI (*NumberOfSelectedEntries)(ADMListRef list);
	
	// Move up to after Tracker procedures FIX_ME pja
	void ASAPI (*SetMask)(ADMListRef entry, ADMActionMask mask);
	ADMActionMask ASAPI (*GetMask)(ADMListRef entry);

	// Move up to after Notifier procedures FIX_ME pja
	void ASAPI (*SetNotifierData)(ADMListRef entry, ADMUserData data);
	ADMUserData ASAPI (*GetNotifierData)(ADMListRef entry);

} ADMListSuite1;

typedef ADMListSuite1 ADMListSuite;


#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif








