/*
 * Name:
 *	ADMDialog.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM Dialog Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.3 3/10/1996	DL	Third version.
 *		Created by Dave Lazarony.
 */

#ifndef __ADMDialog__
#define __ADMDialog__

/*
 * Includes
 */
 
#ifndef __ADMItem__
#include "ADMItem.h"
#endif

#ifndef __ADMTypes__
#include "ADMTypes.h"
#endif

#ifndef __ASHelp__
#include "ASHelp.h"
#endif

#ifndef __SPInterface__
#include "SPInterf.h"
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

#define kADMDialogSuite					"ADM Dialog Suite"

#define kADMDialogSuiteVersion2			2
#define kADMDialogSuiteVersion3			3
#define kADMDialogSuiteVersion			kADMDialogSuiteVersion3

/*
 * ADM Dialog Message
 */

#define kADMCaller						"Adobe Dialog Manager"
#define kADMCreateCustomItem			"Create Custom Item"

typedef struct
{
	SPMessageData d;
	struct ADMDialog *dialog;
	int itemID;
	ADMItemType itemType;
	ASRect boundsRect;
	ADMItemInitProc initProc;
	ADMUserData data;
	struct ADMItem *item;
}
ADMCreateCustomItemMessage;

/*
 * Dialog Styles
 */
 
typedef enum
{
	kADMModalDialogStyle 					= 0,
	kADMAlertDialogStyle					= 1,
	kADMFloatingDialogStyle					= 2,
	kADMTabbedFloatingDialogStyle			= 3,
	kADMResizingFloatingDialogStyle			= 4,
	kADMTabbedResizingFloatingDialogStyle	= 5,
	kADMPopupDialogStyle					= 6,
	kADMNoCloseFloatingDialogStyle			= 7,
	kADMSystemAlertDialogStyle				= 8,
	kADMDummyDialogStyle 					= 0xFFFFFFFF
}
ADMDialogStyle;

 
/* 
 * Standard Dialog Item IDs
 */

typedef enum
{
	kADMUniqueItemID = 0,
	kADMFirstItemID = -1,
	kADMLastItemID = -2,
	kADMDefaultItemID = -3,
	kADMCancelItemID = -4,
	kADMMenuItemID = -5,
	kADMResizeItemID = -6,
	kADMDummyItemID = 0xFFFFFFFF
}
ADMStandardDialogItemID;


/*
 * Types
 */

/* ADMItemRef and ADMDialogRef are declared in ADMTypes.h */

typedef ASErr ASAPI (*ADMDialogInitProc)(ADMDialogRef dialog);
typedef void ASAPI (*ADMDialogDrawProc)(ADMDialogRef dialog, ADMDrawerRef drawer);
typedef ASBoolean ASAPI (*ADMDialogTrackProc)(ADMDialogRef dialog, ADMTrackerRef tracker);
typedef void ASAPI (*ADMDialogNotifyProc)(ADMDialogRef dialog, ADMNotifierRef notifier);
typedef void ASAPI (*ADMDialogDestroyProc)(ADMDialogRef dialog);
typedef ASBoolean ASAPI (*ADMDialogTimerProc)(ADMDialogRef dialog, ADMTimerRef timer);
typedef void ASAPI (*ADMDialogTimerAbortProc)(ADMDialogRef dialog, ADMTimerRef timer, ADMAction abortAction);

/*
 * Dialog Suite
 */
typedef struct ADMDialogSuite3
{
	ADMDialogRef ASAPI (*Create)(SPPluginRef pluginRef, char *name, int dialogID, ADMDialogStyle style, ADMDialogInitProc initProc, ADMUserData data);
	int ASAPI (*Modal)(SPPluginRef pluginRef, char *name, int dialogID, ADMDialogStyle style, ADMDialogInitProc initProc, ADMUserData data);
	int ASAPI (*DisplayAsModal)(ADMDialogRef dialog);		// This will disable all floating palettes too. -jro
	void ASAPI (*Destroy)(ADMDialogRef dialog);
	
	void ASAPI (*SetDrawProc)(ADMDialogRef dialog, ADMDialogDrawProc drawProc);
	ADMDialogDrawProc ASAPI (*GetDrawProc)(ADMDialogRef dialog);
	void ASAPI (*DefaultDraw)(ADMDialogRef dialog, ADMDrawerRef drawer);
	
	void ASAPI (*SetTrackProc)(ADMDialogRef dialog, ADMDialogTrackProc trackProc);
	ADMDialogTrackProc ASAPI (*GetTrackProc)(ADMDialogRef dialog);
	ASBoolean ASAPI (*DefaultTrack)(ADMDialogRef dialog, ADMTrackerRef tracker);
	void ASAPI (*SetMask)(ADMDialogRef dialog, ADMActionMask mask);
	ADMActionMask ASAPI (*GetMask)(ADMDialogRef dialog);

	void ASAPI (*SetNotifyProc)(ADMDialogRef dialog, ADMDialogNotifyProc notifyProc);
	ADMDialogNotifyProc ASAPI (*GetNotifyProc)(ADMDialogRef dialog);
	void ASAPI (*DefaultNotify)(ADMDialogRef dialog, ADMNotifierRef notifier);
	void ASAPI (*SendNotify)(ADMDialogRef dialog, char *notifierType);
	void ASAPI (*SetNotifierData)(ADMDialogRef dialog, ADMUserData data);
	ADMUserData ASAPI (*GetNotifierData)(ADMDialogRef dialog);

	void ASAPI (*SetDestroyProc)(ADMDialogRef dialog, ADMDialogDestroyProc destroyProc);
	ADMDialogDestroyProc ASAPI (*GetDestroyProc)(ADMDialogRef dialog);
	
	ADMTimerRef ASAPI (*CreateTimer)(ADMDialogRef dialog, unsigned long milliseconds,
									 ADMActionMask abortMask,
									 ADMDialogTimerProc timerProc,
									 ADMDialogTimerAbortProc abortProc);
	
	void ASAPI (*AbortTimer)(ADMDialogRef dialog, ADMTimerRef timer);

	void ASAPI (*SetUserData)(ADMDialogRef dialog, ADMUserData data);
	ADMUserData ASAPI (*GetUserData)(ADMDialogRef dialog);

	ASAPI char* (*GetDialogName)(ADMDialogRef dialog);
	int ASAPI (*GetID)(ADMDialogRef dialog);
	SPPluginRef ASAPI (*GetPluginRef)(ADMDialogRef dialog);
	ASWindowRef ASAPI (*GetWindowRef)(ADMDialogRef dialog);
	
	void ASAPI (*SetDialogStyle)(ADMDialogRef dialog, ADMDialogStyle style);
	ADMDialogStyle ASAPI (*GetDialogStyle)(ADMDialogRef dialog);
	
	void ASAPI (*Show)(ADMDialogRef dialog, ASBoolean show);
	ASBoolean ASAPI (*IsVisible)(ADMDialogRef dialog);
	
	void ASAPI (*Enable)(ADMDialogRef dialog, ASBoolean enable);
	ASBoolean ASAPI (*IsEnabled)(ADMDialogRef dialog);
	
	void ASAPI (*Activate)(ADMDialogRef dialog, ASBoolean activate);
	ASBoolean ASAPI (*IsActive)(ADMDialogRef dialog);
	
	void ASAPI (*SetLocalRect)(ADMDialogRef dialog, ASRect *localRect);
	void ASAPI (*GetLocalRect)(ADMDialogRef dialog, ASRect *localRect);
	
	void ASAPI (*SetBoundsRect)(ADMDialogRef dialog, ASRect *boundsRect);
	void ASAPI (*GetBoundsRect)(ADMDialogRef dialog, ASRect *boundsRect);
	
	void ASAPI (*Move)(ADMDialogRef dialog, int x, int y);
	void ASAPI (*Size)(ADMDialogRef dialog, int width, int height);
	
	void ASAPI (*LocalToScreenPoint)(ADMDialogRef dialog, ASPoint *point);
	void ASAPI (*ScreenToLocalPoint)(ADMDialogRef dialog, ASPoint *point);
	
	void ASAPI (*LocalToScreenRect)(ADMDialogRef dialog, ASRect *rect);
	void ASAPI (*ScreenToLocalRect)(ADMDialogRef dialog, ASRect *rect);
	
	void ASAPI (*Invalidate)(ADMDialogRef dialog);
	void ASAPI (*InvalidateRect)(ADMDialogRef dialog, ASRect *invalRect);
	void ASAPI (*Update)(ADMDialogRef dialog);
	
	ASBoolean ASAPI (*SetCursorID)(ADMDialogRef dialog, SPPluginRef pluginRef, int cursorID);
	void ASAPI (*GetCursorID)(ADMDialogRef dialog, SPPluginRef *pluginRef, int *cursorID);

	void ASAPI (*SetFont)(ADMDialogRef dialog, ADMFont font);
	ADMFont ASAPI (*GetFont)(ADMDialogRef dialog);

	void ASAPI (*SetText)(ADMDialogRef dialog, char *text);
	void ASAPI (*GetText)(ADMDialogRef dialog, char *text, int maxLen);
	int ASAPI (*GetTextLength)(ADMDialogRef dialog);
	
	void ASAPI (*SetMinWidth)(ADMDialogRef dialog, int width);
	int ASAPI (*GetMinWidth)(ADMDialogRef dialog);
	
	void ASAPI (*SetMinHeight)(ADMDialogRef dialog, int height);
	int ASAPI (*GetMinHeight)(ADMDialogRef dialog);
	
	void ASAPI (*SetMaxWidth)(ADMDialogRef dialog, int width);
	int ASAPI (*GetMaxWidth)(ADMDialogRef dialog);
	
	void ASAPI (*SetMaxHeight)(ADMDialogRef dialog, int height);
	int ASAPI (*GetMaxHeight)(ADMDialogRef dialog);
	
	void ASAPI (*SetHorizontalIncrement)(ADMDialogRef dialog, int increment);
	int ASAPI (*GetHorizontalIncrement)(ADMDialogRef dialog);

	void ASAPI (*SetVerticalIncrement)(ADMDialogRef dialog, int increment);
	int ASAPI (*GetVerticalIncrement)(ADMDialogRef dialog);

	ADMItemRef ASAPI (*GetItem)(ADMDialogRef dialog, int itemID);
	
	ADMItemRef ASAPI (*CreateItem)(ADMDialogRef dialog, int itemID, ADMItemType itemType, ASRect *boundsRect, ADMItemInitProc initProc, ADMUserData data);
	void ASAPI (*DestroyItem)(ADMDialogRef dialog, ADMItemRef item);
	
	ADMItemRef ASAPI (*GetNextItem)(ADMDialogRef dialog, ADMItemRef item);
	ADMItemRef ASAPI (*GetPreviousItem)(ADMDialogRef dialog, ADMItemRef item);

	void ASAPI (*SetDefaultItemID)(ADMDialogRef dialog, int itemID);
	int ASAPI (*GetDefaultItemID)(ADMDialogRef dialog);
	void ASAPI (*SetCancelItemID)(ADMDialogRef dialog, int itemID);
	int ASAPI (*GetCancelItemID)(ADMDialogRef dialog);
	
	ASBoolean  ASAPI (*EndModal)(ADMDialogRef dialog, int modalResultID, ASBoolean cancelling);
	
	ASErr ASAPI (*RegisterItemType)(SPPluginRef pluginRef, ADMItemType itemType);
	ASErr ASAPI (*UnregisterItemType)(SPPluginRef pluginRef, ADMItemType itemType);

	void ASAPI (*LoadToolTips)(ADMDialogRef dialog, int stringID);

	void ASAPI (*SetHelpID)(ADMDialogRef dialog, ASHelpID helpID);
	ASHelpID ASAPI (*GetHelpID)(ADMDialogRef dialog);
	void ASAPI (*Help)(ADMDialogRef dialog);

	ASBoolean ASAPI (*IsCollapsed)(ADMDialogRef dialog);
	
} ADMDialogSuite3;

typedef ADMDialogSuite3 ADMDialogSuite; 	// Current version

#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif








