/*
 * Name:
 *	ADMHierarchyList.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM Hierarchy List Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 10/17/1997	WH	First version.
 *		Created by Wei Huang.
 */

#ifndef __ADMHierarchyList__
#define __ADMHierarchyList__

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

#define kADMHierarchyListSuite			"ADM Hierarchy List Suite"
#define kADMHierarchyListSuiteVersion	1

#define kMultiSelectAcrossHierarchyLevels	0x00000001


typedef struct ADMHierarchyList *ADMHierarchyListRef;

typedef struct ADMListEntry *ADMListEntryRef;

typedef ASErr ASAPI (*ADMListEntryInitProc)(ADMListEntryRef entry);
typedef void ASAPI (*ADMListEntryDrawProc)(ADMListEntryRef entry, ADMDrawerRef drawer);
typedef ASBoolean ASAPI (*ADMListEntryTrackProc)(ADMListEntryRef entry, ADMTrackerRef tracker);
typedef void ASAPI (*ADMListEntryNotifyProc)(ADMListEntryRef entry, ADMNotifierRef notifier);
typedef void ASAPI (*ADMListEntryDestroyProc)(ADMListEntryRef entry);
	
/*
 * List Suite
 */
typedef struct ADMHierarchyListSuite
{
	void ASAPI (*SetMenuID)(ADMHierarchyListRef list, int menuResID);
	int ASAPI (*GetMenuID)(ADMHierarchyListRef list);
	
	ADMItemRef ASAPI (*GetItem)(ADMHierarchyListRef list);
	
	void ASAPI (*SetUserData)(ADMHierarchyListRef list, ADMUserData data);
	ADMUserData ASAPI (*GetUserData)(ADMHierarchyListRef list);

	void ASAPI (*SetInitProc)(ADMHierarchyListRef list, ADMListEntryInitProc initProc);
	void ASAPI (*SetInitProcRecursive)(ADMHierarchyListRef list, ADMListEntryInitProc initProc);
	ADMListEntryInitProc ASAPI (*GetInitProc)(ADMHierarchyListRef list);
	
	void ASAPI (*SetDrawProc)(ADMHierarchyListRef list, ADMListEntryDrawProc drawProc);
	void ASAPI (*SetDrawProcRecursive)(ADMHierarchyListRef list, ADMListEntryDrawProc drawProc);
	ADMListEntryDrawProc ASAPI (*GetDrawProc)(ADMHierarchyListRef list);
	
	void ASAPI (*SetTrackProc)(ADMHierarchyListRef list, ADMListEntryTrackProc trackProc);
	void ASAPI (*SetTrackProcRecursive)(ADMHierarchyListRef list, ADMListEntryTrackProc trackProc);
	ADMListEntryTrackProc ASAPI (*GetTrackProc)(ADMHierarchyListRef list);
	
	void ASAPI (*SetMask)(ADMHierarchyListRef entry, ADMActionMask mask);
	void ASAPI (*SetMaskRecursive)(ADMHierarchyListRef entry, ADMActionMask mask);
	ADMActionMask ASAPI (*GetMask)(ADMHierarchyListRef entry);

	void ASAPI (*SetNotifyProc)(ADMHierarchyListRef list, ADMListEntryNotifyProc notifyProc);
	void ASAPI (*SetNotifyProcRecursive)(ADMHierarchyListRef list, ADMListEntryNotifyProc notifyProc);
	ADMListEntryNotifyProc ASAPI (*GetNotifyProc)(ADMHierarchyListRef list);
	
	void ASAPI (*SetNotifierData)(ADMHierarchyListRef entry, ADMUserData data);
	ADMUserData ASAPI (*GetNotifierData)(ADMHierarchyListRef entry);

	void ASAPI (*SetDestroyProc)(ADMHierarchyListRef list, ADMListEntryDestroyProc destroyProc);
	void ASAPI (*SetDestroyProcRecursive)(ADMHierarchyListRef list, ADMListEntryDestroyProc destroyProc);
	ADMListEntryDestroyProc ASAPI (*GetDestroyProc)(ADMHierarchyListRef list);
	
	void ASAPI (*SetEntryWidth)(ADMHierarchyListRef list, int width);
	void ASAPI (*SetEntryWidthRecursive)(ADMHierarchyListRef list, int width);
	int ASAPI  (*GetEntryWidth)(ADMHierarchyListRef list);
	int ASAPI  (*GetNonLeafEntryWidth)(ADMHierarchyListRef list);
	
	void ASAPI (*SetEntryHeight)(ADMHierarchyListRef list, int height);
	void ASAPI (*SetEntryHeightRecursive)(ADMHierarchyListRef list, int height);
	int ASAPI  (*GetEntryHeight)(ADMHierarchyListRef list);
	
	void ASAPI (*SetEntryTextRect)(ADMHierarchyListRef list, ASRect *rect);
	void ASAPI (*SetEntryTextRectRecursive)(ADMHierarchyListRef list, ASRect *rect);
	void ASAPI  (*GetEntryTextRect)(ADMHierarchyListRef list, ASRect *rect);

	void ASAPI (*SetNonLeafEntryTextRect)(ADMHierarchyListRef list, ASRect *rect);
	void ASAPI (*SetNonLeafEntryTextRectRecursive)(ADMHierarchyListRef list, ASRect *rect);
	void ASAPI (*GetNonLeafEntryTextRect)(ADMHierarchyListRef list, ASRect *rect);
	
	ADMListEntryRef ASAPI (*InsertEntry)(ADMHierarchyListRef list, int index);
	void ASAPI (*RemoveEntry)(ADMHierarchyListRef list, int index);
	
	ADMListEntryRef ASAPI (*GetEntry)(ADMHierarchyListRef list, int entryID);
	
	ADMListEntryRef ASAPI (*IndexEntry)(ADMHierarchyListRef list, int index);
	
	ADMListEntryRef ASAPI (*FindEntry)(ADMHierarchyListRef list, char *text);
	
	ADMListEntryRef ASAPI (*PickEntry)(ADMHierarchyListRef list, ASPoint *point);
	ADMListEntryRef ASAPI (*PickLeafEntry)(ADMHierarchyListRef list, ASPoint *point);
	
	ADMListEntryRef ASAPI (*GetActiveEntry)(ADMHierarchyListRef list);
	ADMListEntryRef ASAPI (*GetActiveLeafEntry)(ADMHierarchyListRef list);
	
	ADMListEntryRef ASAPI (*IndexSelectedEntry)(ADMHierarchyListRef list, int selectionIndex);
	ADMListEntryRef ASAPI (*IndexAllSelectedEntriesInHierarchy)(ADMHierarchyListRef list, int selectionIndex);
	ADMListEntryRef ASAPI (*IndexUnNestedSelectedEntriesInHierarchy)(ADMHierarchyListRef list, int selectionIndex);
	
	int ASAPI (*NumberOfEntries)(ADMHierarchyListRef list);
	
	int ASAPI (*NumberOfSelectedEntries)(ADMHierarchyListRef list);
	int ASAPI (*NumberOfAllSelectedEntriesInHierarchy)(ADMHierarchyListRef list);
	int ASAPI (*NumberOfUnNestedSelectedEntriesInHierarchy)(ADMHierarchyListRef list);
	
	ADMListEntryRef ASAPI (*GetParentEntry)(ADMHierarchyListRef list);
	void ASAPI (*GetLocalRect)(ADMHierarchyListRef list, ASRect *rect);

	void ASAPI (*LocalToScreenPoint)(ADMHierarchyListRef list, ASPoint *point);
	void ASAPI (*ScreenToLocalPoint)(ADMHierarchyListRef list, ASPoint *point);

	void ASAPI (*LocalToGlobalPoint)(ADMHierarchyListRef list, ASPoint *point);
	void ASAPI (*GlobalToLocalPoint)(ADMHierarchyListRef list, ASPoint *point);

	void ASAPI (*LocalToGlobalRect)(ADMHierarchyListRef list, ASRect *rect);
	void ASAPI (*GlobalToLocalRect)(ADMHierarchyListRef list, ASRect *rect);

	void ASAPI (*SetIndentationWidth)(ADMHierarchyListRef list, int width);
	void ASAPI (*SetIndentationWidthRecursive)(ADMHierarchyListRef list, int width);
	int ASAPI (*GetIndentationWidth)(ADMHierarchyListRef list);

	void ASAPI (*SetLocalLeftMargin)(ADMHierarchyListRef list, int width);
	int ASAPI (*GetLocalLeftMargin)(ADMHierarchyListRef list);
	int ASAPI (*GetGlobalLeftMargin)(ADMHierarchyListRef list);

	void ASAPI (*SetDivided)(ADMHierarchyListRef list, ASBoolean divided);
	void ASAPI (*SetDividedRecursive)(ADMHierarchyListRef list, ASBoolean divided);
	ASBoolean ASAPI (*GetDivided)(ADMHierarchyListRef list);

	void ASAPI (*SetFlags)(ADMHierarchyListRef list, int flags);
	void ASAPI (*SetFlagsRecursive)(ADMHierarchyListRef list, int flags);
	ASBoolean ASAPI (*GetFlags)(ADMHierarchyListRef list);

	void ASAPI (*Invalidate)(ADMHierarchyListRef list);

} ADMHierarchyListSuite;



#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif








