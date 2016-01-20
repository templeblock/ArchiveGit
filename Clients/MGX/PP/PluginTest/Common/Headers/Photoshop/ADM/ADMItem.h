/*
 * Name:
 *	ADM Item Suite.
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM Item Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.2 3/10/1996	DL	Second version.
 *		Created by Dave Lazarony.
 */

#ifndef __ADMItem__
#define __ADMItem__

/*
 * Includes
 */
 
#ifndef __ADMTypes__
#include "ADMTypes.h"
#endif

#ifndef __ADMTracker__
#include "ADMTracker.h"
#endif

#ifndef __ASHelp__
#include "ASHelp.h"
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

#define kADMItemSuite								"ADM Item Suite"

#define kADMItemSuiteVersion3						3
#define kADMItemSuiteVersion4						4

#define kADMItemSuiteVersion						kADMItemSuiteVersion4


/*
 * Item Types																	
 */																	 			
#define kADMFrameType								"ADM Frame Type"							
#define kADMListBoxType								"ADM List Box Type"		
#define kADMHierarchyListBoxType					"ADM Hierarchy List Box Type"			
#define kADMPictureCheckBoxType						"ADM Picture Check Box Button Type"		
#define kADMPicturePushButtonType					"ADM Picture Push Button Type"		
#define kADMPictureRadioButtonType					"ADM Picture Radio Button Type"		
#define kADMPictureStaticType						"ADM Picture Static Type"			
#define kADMPopupListType							"ADM Popup List Type"
#define kADMPopupMenuType							"ADM Popup Menu Type"
#define kADMResizeType								"ADM Resize Type"	
#define kADMScrollbarType							"ADM Scrollbar Type"
#define kADMScrollingPopupListType					"ADM Scrolling Popup List Type"
#define kADMSliderType								"ADM Slider Type"
#define kADMSpinEditType							"ADM Spin Edit Type"
#define kADMSpinEditPopupType						"ADM Spin Edit Popup Type"
#define kADMSpinEditScrollingPopupType				"ADM Spin Edit Scrolling Popup Type"
#define kADMTextCheckBoxType						"ADM Text Check Box Type"
#define kADMTextEditType							"ADM Text Edit Type"
#define kADMTextEditMultiLineType					"ADM Text Edit Multi Line Type"
#define kADMTextEditPopupType						"ADM Text Edit Popup Type"
#define kADMTextEditScrollingPopupType				"ADM Text Edit Scrolling Popup Type"
#define kADMTextPushButtonType						"ADM Text Push Button Type"
#define kADMTextRadioButtonType						"ADM Text Radio Button Type"
#define kADMTextStaticType							"ADM Text Static Type"
#define kADMTextStaticMultiLineType					"ADM Text Static Multi Line Type"
#define kADMUserType								"ADM User Type"


typedef char *ADMItemType;


/*
 * Item Styles
 */
typedef int ADMItemStyle;

typedef enum
{
	kADMBlackFrameStyle 							= 0,
	kADMGrayFrameStyle								= 1,
	kADMSunkenFrameStyle							= 2,
	kADMRaisedFrameStyle							= 3,
	kADMEtchedFrameStyle							= 4,
	kADMDummyFrameStyle								= 0xFFFFFFFF
} 
ADMFrameStyle;

typedef enum
{
	kADMBlackAndSunkenRectPictureButtonStyle		= 0,
	kADMBlackRectPictureButtonStyle					= 1,
	kADMDummyPictureButtonStyle						= 0xFFFFFFFF
}
ADMPictureButtonStyle;

/* Picture Static styles */
#define kADMDisableAutoActivatePictureStaticStyle	(1L << 1)

/* List box styles */
#define kADMMultiSelectListBoxStyle					(1L << 0)
#define kADMDividedListBoxStyle						(1L << 1)
#define kADMTileListBoxStyle						(1L << 2)
#define kADMEntryAlwaysSelectedListBoxStyle			(1L << 3)
#define kADMBlackRectListBoxStyle					(1L << 4)
#define kADMUseImageListBoxStyle					(1L << 5)

typedef enum
{
	kADMRightPopupMenuStyle 						= 0,
	kADMBottomPopupMenuStyle						= 1,
	kADMDummyPopupMenuStyle							= 0xFFFFFFFF
} 
ADMPopupMenuStyle;

typedef enum
{
	kADMVerticalSpinEditStyle 						= 0,
	kADMHorizontalSpinEditStyle						= 1,
	kADMDummySpinEditStyle	 						= 0xFFFFFFFF
}
ADMSpinEditStyle;

typedef enum
{
	kADMVerticalSpinEditPopupStyle 					= 0,
	kADMHorizontalSpinEditPopupStyle				= 4,
	kADMDummySpinEditPopupStyle 					= 0xFFFFFFFF
} 
ADMSpinEditPopupStyle;

typedef enum
{
	kADMSingleLineTextEditStyle						= 0,
	kADMNumericTextEditStyle 						= 2,
	kADMDummyTextEditStyle 							= 0xFFFFFFFF
} 
ADMTextEditStyle;

/* Text Static styles */
#define kADMClippedTextStaticStyle					(1L << 0)
#define kADMDisableAutoActivateTextStaticStyle		(1L << 1)
#define kADMTruncateEndTextStaticStyle				(1L << 2)	// clipped style has priority
#define kADMTruncateMiddleTextStaticStyle			(1L << 3)	// truncate end has priority

typedef enum
{
	kADMSingleLineEditPopupStyle 					= 0,
	kADMNumericEditPopupStyle						= 2,
	kADMExclusiveEditPopupStyle						= 5,
	kADMDummyTextEditPopupStyle 					= 0xFFFFFFFF
}
ADMTextEditPopupStyle;

/* Must set this style on all buttons in the radio group */
typedef enum
{
	kADMRadioButtonOneAlwaysSetStyle					= 0,
	kADMRadioButtonAllowNoneSetStyle					= 1,
	kADMDummyRadioButtonStyle						= 0xFFFFFFFF
}
ADMRadioButtonStyle;



/*
 * Child Item IDs
 */
typedef enum
{
	kADMListBoxScrollbarChildID						= 1,
	kADMListBoxListChildID							= 2,
	kADMListBoxDummyChildID							= 0xFFFFFFFF
} 
ADMListBoxChildID;

typedef enum
{
	kADMSpinEditUpButtonChildID 					= 1,
	kADMSpinEditDownButtonChildID					= 2,
	kADMSpinEditTextEditChildID						= 3,
	kADMSpinEditDummyChildID						= 0xFFFFFFFF
}
ADMSpinEditChildID;

typedef enum
{
	kADMSpinEditPopupUpButtonChildID 				= 1,
	kADMSpinEditPopupDownButtonChildID				= 2,
	kADMSpinEditPopupTextEditChildID				= 3,
	kADMSpinEditPopupPopupChildID					= 4,
	kADMSpinEditPopupDummyChildID					= 0xFFFFFFFF
}
ADMSpinEditPopupChildID;

typedef enum
{
	kADMTextEditPopupTextEditChildID				= 3,
	kADMTextEditPopupPopupChildID					= 4,
	kADMTextEditPopupDummyChildID					= 0xFFFFFFFF
}
ADMTextEditPopupChildID;



typedef ASErr ASAPI (*ADMItemInitProc)(ADMItemRef item);
typedef void ASAPI (*ADMItemDrawProc)(ADMItemRef item, ADMDrawerRef drawer);
typedef ASBoolean ASAPI (*ADMItemTrackProc)(ADMItemRef item, ADMTrackerRef tracker);
typedef void ASAPI (*ADMItemNotifyProc)(ADMItemRef item, ADMNotifierRef notifier);
typedef void ASAPI (*ADMItemDestroyProc)(ADMItemRef item);
typedef ASBoolean ASAPI (*ADMItemTimerProc)(ADMItemRef item, ADMTimerRef timer);
typedef void ASAPI (*ADMItemTimerAbortProc)(ADMItemRef item, ADMTimerRef timer, ADMAction abortAction);
typedef ASBoolean ASAPI (*ADMItemFloatToTextProc)(ADMItemRef item, float value, char *text, int textLength);
typedef ASBoolean ASAPI (*ADMItemTextToFloatProc)(ADMItemRef item, char *text, float *value);
	
	
/*
 * Item Suite
 */
 
typedef struct ADMItemSuite4
{
	ADMItemRef ASAPI (*Create)(ADMDialogRef dialog, int itemID, ADMItemType itemType, ASRect *boundsRect, ADMItemInitProc initProc, ADMUserData data);
	void ASAPI (*Destroy)(ADMItemRef item);
	
	void ASAPI (*SetDrawProc)(ADMItemRef item, ADMItemDrawProc drawProc);
	ADMItemDrawProc ASAPI (*GetDrawProc)(ADMItemRef item);
	void ASAPI (*DefaultDraw)(ADMItemRef item, ADMDrawerRef drawer);
	
	void ASAPI (*SetTrackProc)(ADMItemRef item, ADMItemTrackProc trackProc);
	ADMItemTrackProc ASAPI (*GetTrackProc)(ADMItemRef item);
	ASBoolean ASAPI (*DefaultTrack)(ADMItemRef item, ADMTrackerRef tracker);
	void ASAPI (*SetMask)(ADMItemRef item, ADMActionMask mask);
	ADMActionMask ASAPI (*GetMask)(ADMItemRef item);

	void ASAPI (*SetNotifyProc)(ADMItemRef item, ADMItemNotifyProc notifyProc);
	ADMItemNotifyProc ASAPI (*GetNotifyProc)(ADMItemRef item);
	void ASAPI (*DefaultNotify)(ADMItemRef item, ADMNotifierRef notifier);
	void ASAPI (*SendNotify)(ADMItemRef item, char *notifierType);
	void ASAPI (*SetNotifierData)(ADMItemRef item, ADMUserData data);
	ADMUserData ASAPI (*GetNotifierData)(ADMItemRef item);

	void ASAPI (*SetDestroyProc)(ADMItemRef item, ADMItemDestroyProc destroyProc);
	ADMItemDestroyProc ASAPI (*GetDestroyProc)(ADMItemRef item);
	
	ADMTimerRef ASAPI (*CreateTimer)(ADMItemRef item, unsigned long milliseconds,
									 ADMActionMask abortMask,
									 ADMItemTimerProc timerProc,
									 ADMItemTimerAbortProc abortProc);

	void ASAPI (*AbortTimer)(ADMItemRef item, ADMTimerRef timer);

	void ASAPI (*SetUserData)(ADMItemRef item, ADMUserData data);
	ADMUserData ASAPI (*GetUserData)(ADMItemRef item);

	int ASAPI (*GetID)(ADMItemRef item);
	ADMDialogRef ASAPI (*GetDialog)(ADMItemRef item);
	ASWindowRef ASAPI (*GetWindowRef)(ADMItemRef item);
	ADMListRef ASAPI (*GetList)(ADMItemRef item);
	ADMItemRef ASAPI (*GetChildItem)(ADMItemRef item, int childID);	

	void ASAPI (*SetPluginRef)(ADMItemRef item, SPPluginRef pluginRef);
	SPPluginRef ASAPI (*GetPluginRef)(ADMItemRef item);

	void ASAPI (*SetItemType)(ADMItemRef item, ADMItemType type);
	ADMItemType ASAPI (*GetItemType)(ADMItemRef item);
	
	void ASAPI (*SetItemStyle)(ADMItemRef item, ADMItemStyle style);
	ADMItemStyle ASAPI (*GetItemStyle)(ADMItemRef item);
	
	void ASAPI (*Show)(ADMItemRef item, ASBoolean show);
	ASBoolean ASAPI (*IsVisible)(ADMItemRef item);
	
	void ASAPI (*Enable)(ADMItemRef item, ASBoolean enable);
	ASBoolean ASAPI (*IsEnabled)(ADMItemRef item);
	
	void ASAPI (*Activate)(ADMItemRef item, ASBoolean activate);
	ASBoolean ASAPI (*IsActive)(ADMItemRef item);
	
	void ASAPI (*Known)(ADMItemRef item, ASBoolean known);
	ASBoolean ASAPI (*IsKnown)(ADMItemRef item);

	void ASAPI (*SetLocalRect)(ADMItemRef item, ASRect *localRect);
	void ASAPI (*GetLocalRect)(ADMItemRef item, ASRect *localRect);
	
	void ASAPI (*SetBoundsRect)(ADMItemRef item, ASRect *boundsRect);
	void ASAPI (*GetBoundsRect)(ADMItemRef item, ASRect *boundsRect);
	
	void ASAPI (*GetBestSize)(ADMItemRef item, ASPoint *size);

	void ASAPI (*Move)(ADMItemRef item, int x, int y);
	void ASAPI (*Size)(ADMItemRef item, int width, int height);
	
	void ASAPI (*LocalToScreenPoint)(ADMItemRef item, ASPoint *point);
	void ASAPI (*ScreenToLocalPoint)(ADMItemRef item, ASPoint *point);
	
	void ASAPI (*LocalToScreenRect)(ADMItemRef item, ASRect *rect);
	void ASAPI (*ScreenToLocalRect)(ADMItemRef item, ASRect *rect);
	
	void ASAPI (*Invalidate)(ADMItemRef item);
	void ASAPI (*InvalidateRect)(ADMItemRef dialog, ASRect *invalRect);
	void ASAPI (*Update)(ADMItemRef item);
	
	ASBoolean ASAPI (*SetCursorID)(ADMItemRef item, SPPluginRef pluginRef, int cursorID);
	void ASAPI (*GetCursorID)(ADMItemRef item, SPPluginRef *pluginRef, int *cursorID);

	void ASAPI (*SetPictureID)(ADMItemRef item, int pictureResID);
	int ASAPI (*GetPictureID)(ADMItemRef item);
	
	void ASAPI (*SetSelectedPictureID)(ADMItemRef item, int pictureResID);
	int ASAPI (*GetSelectedPictureID)(ADMItemRef item);
	
	void ASAPI (*SetDisabledPictureID)(ADMItemRef item, int pictureResID);
	int ASAPI (*GetDisabledPictureID)(ADMItemRef item);

	
	/*
	 * ToolTip
	 */

	void ASAPI (*SetTipString)(ADMItemRef item, char *tipStr);
	void ASAPI (*GetTipString)(ADMItemRef item, char *tipStr, int maxLen);
	
	int ASAPI (*GetTipStringLength)(ADMItemRef item);
	
	void ASAPI (*EnableTip)(ADMItemRef item, ASBoolean enable);
	ASBoolean ASAPI (*IsTipEnabled)(ADMItemRef item);
	
	void ASAPI (*ShowToolTip)(ADMItemRef item, ASPoint *where);
	void ASAPI (*HideToolTip)(ADMItemRef item);


	/*
	 * Text
	 */
	 
	void ASAPI (*SetFont)(ADMItemRef item, ADMFont font);
	ADMFont ASAPI (*GetFont)(ADMItemRef item);

	void ASAPI (*SetText)(ADMItemRef item, char *text);
	void ASAPI (*GetText)(ADMItemRef item, char *text, int maxLen);
	int ASAPI (*GetTextLength)(ADMItemRef item);
	
	void ASAPI (*SetMaxTextLength)(ADMItemRef item, int length);
	int ASAPI (*GetMaxTextLength)(ADMItemRef item);
	
	void ASAPI (*SelectAll)(ADMItemRef item);
	void ASAPI (*SetSelectionRange)(ADMItemRef item, int selStart, int selEnd);
	void ASAPI (*GetSelectionRange)(ADMItemRef item, int *selStart, int *selEnd);

	void ASAPI (*SetJustify)(ADMItemRef item, ADMJustify justify);
	ADMJustify ASAPI (*GetJustify)(ADMItemRef item);
	
	void ASAPI (*SetUnits)(ADMItemRef item, ADMUnits units);
	ADMUnits ASAPI (*GetUnits)(ADMItemRef item);
	
	
	/*
	 * Numerics
	 */
	 
	void ASAPI (*SetPrecision)(ADMItemRef item, int numberOfDecimalPlaces);
	int ASAPI (*GetPrecision)(ADMItemRef item);
	
	void ASAPI (*SetBooleanValue)(ADMItemRef item, ASBoolean value);
	ASBoolean ASAPI (*GetBooleanValue)(ADMItemRef item);
	
	void ASAPI (*SetIntValue)(ADMItemRef item, int value);
	int ASAPI (*GetIntValue)(ADMItemRef item);
		
	void ASAPI (*SetFixedValue)(ADMItemRef item, ASFixed value);	
	ASFixed ASAPI (*GetFixedValue)(ADMItemRef item);
	
	void ASAPI (*SetFloatValue)(ADMItemRef item, float value);	
	float ASAPI (*GetFloatValue)(ADMItemRef item);
	
	void ASAPI (*SetMinIntValue)(ADMItemRef item, int value);
	int ASAPI (*GetMinIntValue)(ADMItemRef item);
		
	void ASAPI (*SetMinFixedValue)(ADMItemRef item, ASFixed value);	
	ASFixed ASAPI (*GetMinFixedValue)(ADMItemRef item);
	
	void ASAPI (*SetMaxIntValue)(ADMItemRef item, int value);
	int ASAPI (*GetMaxIntValue)(ADMItemRef item);
	
	void ASAPI (*SetMinFloatValue)(ADMItemRef item, float value);	
	float ASAPI (*GetMinFloatValue)(ADMItemRef item);
	
	void ASAPI (*SetMaxFixedValue)(ADMItemRef item, ASFixed value);	
	ASFixed ASAPI (*GetMaxFixedValue)(ADMItemRef item);
	
	void ASAPI (*SetMaxFloatValue)(ADMItemRef item, float value);	
	float ASAPI (*GetMaxFloatValue)(ADMItemRef item);
	
	void ASAPI (*SetSmallIncrement)(ADMItemRef item, float increment);	
	float ASAPI (*GetSmallIncrement)(ADMItemRef item);
	
	void ASAPI (*SetLargeIncrement)(ADMItemRef item, float increment);
	float ASAPI (*GetLargeIncrement)(ADMItemRef item);
	
	void ASAPI (*SetAllowUnits)(ADMItemRef item, ASBoolean allow);
	ASBoolean ASAPI (*GetAllowUnits)(ADMItemRef item);
	void ASAPI (*SetAllowMath)(ADMItemRef item, ASBoolean allow);
	ASBoolean ASAPI (*GetAllowMath)(ADMItemRef item);
	
	void ASAPI (*SetFloatToTextProc)(ADMItemRef item, ADMItemFloatToTextProc proc);
	ADMItemFloatToTextProc ASAPI (*GetFloatToTextProc)(ADMItemRef item);
	ASBoolean ASAPI (*DefaultFloatToText)(ADMItemRef item, float value, char *text, int textLength);
	void ASAPI (*SetTextToFloatProc)(ADMItemRef item, ADMItemTextToFloatProc proc);
	ADMItemTextToFloatProc ASAPI (*GetTextToFloatProc)(ADMItemRef item);
	ASBoolean ASAPI (*DefaultTextToFloat)(ADMItemRef item, char *text, float *value);

	void ASAPI (*ShowUnits)(ADMItemRef item, ASBoolean show);
	ASBoolean ASAPI (*GetShowUnits)(ADMItemRef item);

	ASBoolean ASAPI (*WasPercentageChange)(ADMItemRef item);

	// Help
	void ASAPI (*SetHelpID)(ADMItemRef item, ASHelpID helpID);
	ASHelpID ASAPI (*GetHelpID)(ADMItemRef item);
	void ASAPI (*Help)(ADMItemRef dialog);

	// New Item type support
	ADMHierarchyListRef ASAPI (*GetHierarchyList)(ADMItemRef item);
	
} ADMItemSuite4;		// kADMItemSuiteVersion4

typedef ADMItemSuite4 ADMItemSuite;	//  The current version


#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif








