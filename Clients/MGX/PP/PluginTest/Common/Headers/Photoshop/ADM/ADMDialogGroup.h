/*
 * Name:
 *	ADMDialogGroup.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM DialogGroup Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.6 9/28/1996	MF	Sixth version.
 *		Created by Matt Foster.
 */

#ifndef __ADMDialogGroup__
#define __ADMDialogGroup__

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

#define kADMDialogGroupSuite 			"ADM Dialog Group Suite"
#define kADMDialogGroupSuiteVersion1 	1
#define kADMDialogGroupSuiteVersion 	1
#define kADMDialogGroupVersion 			kADMDialogGroupSuiteVersion1



#define kDockHostConflictError	'DOCK'
#define kTabGroupNotFoundError	'T!FD'
#define kAlreadyDockedError		'DCKD'


 /*
 * Types
 */

/*
 * For the xxGroupInfo() functions, positionCode is a code to restore a 
 * dialog's position within a docked/tabbed group.  The group is 
 * referred to by name, this being the name of the ADM Dialog that is
 * the first tab in the top dcok of the group. 
 *
 * You don't need to know what positionCode means, but if you are curious:
 *
 * byte		  		meaning
 * ----		 		------------------------------
 *	1 (0x000000ff)	dock position. 0 is no dock, 1 is first docked (i.e. top dock), 2 etc.
 *	2 (0x0000ff00)	tab position, 0 is no tab group, 1 is the 1st tab, 2 etc.
 *	3 (0x00010000)	bit 16, boolean, 1 for front tab.
 *	3 (0x00020000)	bit 17, boolean, 0 is zoom up, 1 is zoom down.
 *	3 (0x00040000)	bit 18, boolean, 0 is in hidden dock, 1 is in visible dock.
 *	4 (0x00000000)	reserved. currently unused
 *
 *	So for stand alone palette, the tab, dock position code is 0,?,0,0 
 * 
 */

/* default position code for stand alone palette */
#define kADM_DPDefaultCode		0x00030000

/* DialogPosition (DP) Bit */
#define kADM_DPDockBit			0
#define kADM_DPTabBit			8
#define kADM_DPFrontTabBit		16
#define kADM_DPZoomBit			17
#define kADM_DPDockVisibleBit	18

/* DialogPosition (DP) mask */
#define kADM_DPDockMask				0x000000ff
#define kADM_DPTabMask				0x0000ff00
#define kADM_DPFrontTabMask			0x00010000
#define kADM_DPZoomMask				0x00020000
#define kADM_DPDockVisibleMask		0x00040000

/* Strings for saving palette state into preference file */
#define kADM_DPLocationStr		"Location"
#define kADM_DPSizeStr			"Size"			// save only for resizable standalone palettes
#define kADM_DPVisibleStr		"Visible"
#define kADM_DPDockCodeStr		"DockCode"
#define kADM_DPDockGroupStr		"DockGroup"


#define kADM_DPDockGroupStrMaxLen		65	// Group name max size = 64 chars. 

/***********************************************************
 **  The Suite
 **/

typedef struct ADMDialogGroupSuite1 {

	ASAPI ASErr (*GetDialogName) ( ADMDialogRef dialog, char **name );

	ASAPI ASErr (*GetDialogGroupInfo) ( ADMDialogRef dialog, char **groupName, long *positionCode );
	ASAPI ASErr (*SetDialogGroupInfo) ( ADMDialogRef dialog, char *groupName, long positionCode );

	// The queries below take a docking position code obtained from above functions or prefs

	// You should always create a palette that is not stand alone
	ASAPI ASBoolean (*IsStandAlonePalette)( long positionCode );
	// Your "show palette" menu would be "Show..." unless palette dock code returns true for
	// IsDockVisible() and IsFrontTab(). 
	ASAPI ASBoolean (*IsDockVisible)( long positionCode );
	ASAPI ASBoolean (*IsFrontTab)( long positionCode );
	// You probably won't ever use this, but it here for completeness.
	ASAPI ASBoolean (*IsCollapsed)( long positionCode );

	ASAPI ASErr (*SetTabGroup) (ADMDialogRef dialog, char *tabGroupName, ASBoolean bringToFront );
 
	ASAPI ASErr (*CountDialogs) ( long *count );
	ASAPI ASErr (*GetNthDialog) ( long n, ADMDialogRef *dialog );
	ASAPI ASErr (*GetNamedDialog) ( char *name, ADMDialogRef *dialog );

	ASAPI void (*ToggleAllFloatingDialogs)();

} ADMDialogGroupSuite1;

typedef ADMDialogGroupSuite1 ADMDialogGroupSuite;



#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif
