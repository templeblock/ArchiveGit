/*
 * Name:
 *	ADMEntry.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM Entry Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.1 5/7/1996	DL	Second version.
 *		Created by Dave Lazarony.
 */

#ifndef __ADMEntry__
#define __ADMEntry__

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

#define kADMEntrySuite			"ADM Entry Suite"

#define kADMEntrySuiteVersion1	1
#define kADMEntrySuiteVersion2	2

#define kADMEntrySuiteVersion	kADMEntrySuiteVersion2


/*
 * Types
 */

typedef ASBoolean ASAPI (*ADMEntryTimerProc)(ADMEntryRef entry, ADMTimerRef timer);
typedef void ASAPI (*ADMEntryTimerAbortProc)(ADMEntryRef entry, ADMTimerRef timer, ADMAction abortAction);


/*
 * Entry Suite
 */
 
typedef struct ADMEntrySuite2
{
	ADMEntryRef ASAPI (*Create)(ADMListRef list);
	void ASAPI (*Destroy)(ADMEntryRef entry);
	
	void ASAPI (*DefaultDraw)(ADMEntryRef entry, ADMDrawerRef drawer);
	ASBoolean ASAPI (*DefaultTrack)(ADMEntryRef entry, ADMTrackerRef tracker);
	void ASAPI (*DefaultNotify)(ADMEntryRef entry, ADMNotifierRef notifier);
	void ASAPI (*SendNotify)(ADMEntryRef entry, char *notifierType);
	
	int ASAPI (*GetIndex)(ADMEntryRef entry);
	
	ADMListRef ASAPI (*GetList)(ADMEntryRef entry);
	
	void ASAPI (*SetID)(ADMEntryRef entry, int entryID);
	int ASAPI (*GetID)(ADMEntryRef entry);
	
	void ASAPI (*SetUserData)(ADMEntryRef entry, ADMUserData data);
	ADMUserData ASAPI (*GetUserData)(ADMEntryRef entry);

	void ASAPI (*Select)(ADMEntryRef entry, ASBoolean select);
	ASBoolean ASAPI (*IsSelected)(ADMEntryRef entry);
	
	void ASAPI (*MakeInBounds)(ADMEntryRef entry);
	ASBoolean ASAPI (*IsInBounds)(ADMEntryRef entry);

	void ASAPI (*Enable)(ADMEntryRef entry, ASBoolean enable);
	ASBoolean ASAPI (*IsEnabled)(ADMEntryRef entry);
	
	void ASAPI (*Activate)(ADMEntryRef entry, ASBoolean activate);
	ASBoolean ASAPI (*IsActive)(ADMEntryRef entry);
	
	void ASAPI (*Check)(ADMEntryRef entry, ASBoolean check);
	ASBoolean ASAPI (*IsChecked)(ADMEntryRef entry);
	
	void ASAPI (*MakeSeparator)(ADMEntryRef entry, ASBoolean separator);
	ASBoolean ASAPI (*IsSeparator)(ADMEntryRef entry);

	void ASAPI (*GetLocalRect)(ADMEntryRef entry, ASRect *localRect);
	
	void ASAPI (*GetBoundsRect)(ADMEntryRef entry, ASRect *boundsRect);
	
	void ASAPI (*LocalToScreenPoint)(ADMEntryRef entry, ASPoint *point);
	void ASAPI (*ScreenToLocalPoint)(ADMEntryRef entry, ASPoint *point);
	
	void ASAPI (*LocalToScreenRect)(ADMEntryRef entry, ASRect *rect);
	void ASAPI (*ScreenToLocalRect)(ADMEntryRef entry, ASRect *rect);
	
	void ASAPI (*Invalidate)(ADMEntryRef entry);
	void ASAPI (*InvalidateRect)(ADMEntryRef dialog, ASRect *invalRect);
	void ASAPI (*Update)(ADMEntryRef entry);
	
	void ASAPI (*SetPictureID)(ADMEntryRef entry, int pictureResID);
	int ASAPI (*GetPictureID)(ADMEntryRef entry);
	
	void ASAPI (*SetSelectedPictureID)(ADMEntryRef entry, int pictureResID);
	int ASAPI (*GetSelectedPictureID)(ADMEntryRef entry);
	
	void ASAPI (*SetDisabledPictureID)(ADMEntryRef entry, int pictureResID);
	int ASAPI (*GetDisabledPictureID)(ADMEntryRef entry);
	
	void ASAPI (*SetText)(ADMEntryRef entry, char *text);
	void ASAPI (*GetText)(ADMEntryRef entry, char *text, int maxLen);
	int ASAPI (*GetTextLength)(ADMEntryRef entry);
	
	
	ADMTimerRef ASAPI (*CreateTimer)(ADMEntryRef entry, unsigned long milliseconds,
									 ADMActionMask abortMask,
									 ADMEntryTimerProc timerProc,
									 ADMEntryTimerAbortProc abortProc);

	void ASAPI (*AbortTimer)(ADMEntryRef entry, ADMTimerRef timer);

} ADMEntrySuite2;	//	kADMEntrySuiteVersion2

typedef ADMEntrySuite2 ADMEntrySuite;	// The current version


#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif








