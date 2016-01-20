/*
 * Name:
 *	ADMListEntry.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Standard Enviornment Configuration
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.1 10/17/97 WH	Second version.
 *		Created by Wei Huang.
 */

#ifndef __ADMListEntry__
#define __ADMListEntry__

/*
 * Includes
 */
 
#ifndef __ADMTypes__
#include "ADMTypes.h"
#endif

#ifndef __ADMTracker__
#include "ADMTracker.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma PRAGMA_ALIGN_BEGIN
#pragma PRAGMA_IMPORT_BEGIN


// Problematic Macintosh Headers #define GetItem.
#ifdef GetItem
#undef GetItem
#endif


/*
 * Constants
 */

#define kADMListEntrySuite			"ADM List Entry Suite"

#define kADMListEntrySuiteVersion	1

typedef ASBoolean ASAPI (*ADMListEntryTimerProc)(ADMListEntryRef entry, ADMTimerRef timer);
typedef void ASAPI (*ADMListEntryTimerAbortProc)(ADMListEntryRef entry, ADMTimerRef timer, ADMAction abortAction);

/*
 * Entry Suite
 */
 
typedef struct ADMListEntrySuite
{
	ADMListEntryRef ASAPI (*Create)(ADMHierarchyListRef list);
	void ASAPI (*Destroy)(ADMListEntryRef entry);
	
	int ASAPI (*GetIndex)(ADMListEntryRef entry);
	
	ADMItemRef ASAPI (*GetItem)(ADMListEntryRef entry);

	ADMHierarchyListRef ASAPI (*GetList)(ADMListEntryRef entry);
	
	ADMHierarchyListRef ASAPI (*GetChildList)(ADMListEntryRef entry);

	ADMHierarchyListRef ASAPI (*CreateChildList)(ADMListEntryRef entry);
	void ASAPI (*DeleteChildList)(ADMListEntryRef entry);

	void ASAPI (*ExpandHierarchy)(ADMListEntryRef entry, ASBoolean flag);
	ASBoolean ASAPI (*IsHierarchyExpanded)(ADMListEntryRef entry);

	void ASAPI (*HideEntryName)(ADMListEntryRef entry, ASBoolean flag);
	ASBoolean ASAPI (*IsEntryNameHidden)(ADMListEntryRef entry);

	void ASAPI (*EnableChildSelection)(ADMListEntryRef entry, ASBoolean flag);
	ASBoolean ASAPI (*IsChildSelectable)(ADMListEntryRef entry);

	int ASAPI (*GetHierarchyDepth)(ADMListEntryRef entry);
	int ASAPI (*GetVisualHierarchyDepth)(ADMListEntryRef entry);

	void ASAPI (*SetID)(ADMListEntryRef entry, int entryID);
	int ASAPI (*GetID)(ADMListEntryRef entry);
	
	void ASAPI (*SetUserData)(ADMListEntryRef entry, ADMUserData data);
	ADMUserData ASAPI (*GetUserData)(ADMListEntryRef entry);

	void ASAPI (*DefaultDraw)(ADMListEntryRef entry, ADMDrawerRef drawer);
	ASBoolean ASAPI (*DefaultTrack)(ADMListEntryRef entry, ADMTrackerRef tracker);
	void ASAPI (*DefaultNotify)(ADMListEntryRef entry, ADMNotifierRef notifier);
	void ASAPI (*SendNotify)(ADMListEntryRef entry, char *notifierType);
	
	void ASAPI (*Select)(ADMListEntryRef entry, ASBoolean select);
	ASBoolean ASAPI (*IsSelected)(ADMListEntryRef entry);
	ASBoolean ASAPI (*IsChildrenSelected)(ADMListEntryRef entry);
	
	void ASAPI (*Enable)(ADMListEntryRef entry, ASBoolean enable);
	ASBoolean ASAPI (*IsEnabled)(ADMListEntryRef entry);
	
	void ASAPI (*Activate)(ADMListEntryRef entry, ASBoolean activate);
	ASBoolean ASAPI (*IsActive)(ADMListEntryRef entry);
	
	void ASAPI (*Check)(ADMListEntryRef entry, ASBoolean check);
	ASBoolean ASAPI (*IsChecked)(ADMListEntryRef entry);
	
	void ASAPI (*MakeSeparator)(ADMListEntryRef entry, ASBoolean separator);
	ASBoolean ASAPI (*IsSeparator)(ADMListEntryRef entry);

	void ASAPI (*GetLocalRect)(ADMListEntryRef entry, ASRect *localRect);
	
	void ASAPI (*GetBoundsRect)(ADMListEntryRef entry, ASRect *boundsRect);
	
	void ASAPI (*LocalToScreenPoint)(ADMListEntryRef entry, ASPoint *point);
	void ASAPI (*ScreenToLocalPoint)(ADMListEntryRef entry, ASPoint *point);
	
	void ASAPI (*LocalToScreenRect)(ADMListEntryRef entry, ASRect *rect);
	void ASAPI (*ScreenToLocalRect)(ADMListEntryRef entry, ASRect *rect);
	
	void ASAPI (*Invalidate)(ADMListEntryRef entry);
	void ASAPI (*Update)(ADMListEntryRef entry);
	
	void ASAPI (*SetPictureID)(ADMListEntryRef entry, int pictureResID);
	int ASAPI (*GetPictureID)(ADMListEntryRef entry);
	
	void ASAPI (*SetSelectedPictureID)(ADMListEntryRef entry, int pictureResID);
	int ASAPI (*GetSelectedPictureID)(ADMListEntryRef entry);
	
	void ASAPI (*SetDisabledPictureID)(ADMListEntryRef entry, int pictureResID);
	int ASAPI (*GetDisabledPictureID)(ADMListEntryRef entry);
	
	void ASAPI (*SetText)(ADMListEntryRef entry, char *text);
	void ASAPI (*GetText)(ADMListEntryRef entry, char *text, int maxLen);
	int ASAPI (*GetTextLength)(ADMListEntryRef entry);
	
	
	ADMTimerRef ASAPI (*CreateTimer)(ADMListEntryRef entry, unsigned long milliseconds,
									 ADMActionMask abortMask,
									 ADMListEntryTimerProc timerProc,
									 ADMListEntryTimerAbortProc abortProc);

	void ASAPI (*AbortTimer)(ADMListEntryRef entry, ADMTimerRef timer);

	/* Move up to after Select procedures FIX_ME 1/29 pja */

	void ASAPI (*MakeInBounds)(ADMListEntryRef entry);
	ASBoolean ASAPI (*IsInBounds)(ADMListEntryRef entry);

} ADMListEntrySuite;



#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif








