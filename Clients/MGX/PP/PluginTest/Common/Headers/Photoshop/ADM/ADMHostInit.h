/*
 * Name:
 *	ADMHostInit.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM Host Init Suite
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.4 8/7/1997	MF	Fourth version.
 *		Created by Matt Foster.
 */

#ifndef __ADMHostInit__
#define __ADMHostInit__

/* The host of ADM provides this suite for ADM to use at initialization.  
These values can also be set/reset later if desired using the ADM Host suites. */

#ifndef __ADMHost__
#include "ADMHost.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma PRAGMA_ALIGN_BEGIN
#pragma PRAGMA_IMPORT_BEGIN


/*
 * Constants
 */

#define kADMHostInitSuite			"ADM Host Init Suite"
#define kADMHostInitSuiteVersion1	1

#define kADMHostInitSuiteVersion	kADMHostInitSuiteVersion1	// The current version


/* Host Options */
#define kADMEnableFloatingPalettes			(1L<<0)		// 1 by default, palettes are enabled and
														// ADM is always in memory.  Disabling palettes
														// allows only modal operations and ADM can be unloaded
#define kADMUseFrontWindowAsAppWindow		(1L<<1)		// 1 by default, Windows only.
														// if app window is undefined, ADM will use whatever
														// window is frontmost at startup as the parent for 
														// all windows.  if 0, the host must set the app window
#define kADMUseMinMaxOnBadValue				(1L<<2)		// 0 by default, when a entered value exceeds its min
														// or max, it resets the last good value.  If this
														// option is set, it sets the value to the min or max
#define kADMUseMacGSAppearance				(1L<<3)		// 0 by default.  if 1 and Mac OS 8 is running, dialogs
														// will have the platinum look, else lighter grey look

/* 
 * Most suites must be fully initialized.  Since this suite is intended to be
 * used by ADM to communicate with the host to obtain overrides to its defaults,
 * any of these function pointers can be NULL and ADM will simply use the default
 * behavior until it is changed later.  The exception to this is the UI resource
 * directory on windows and the host options.  The are checked once at startup
 * and never again.
 */
 
typedef struct ADMHostInitSuite1
{
	void ASAPI (*GetADMHostAppContextProcs)(ADMHostAppContextProcs *appContextProcs);
	void ASAPI (*GetADMHostAppName)(char *hostName, short length);
	void ASAPI (*GetADMResourceAccessProcs)(ADMResourceAccessProcs *resProcs);
	void ASAPI (*GetADMHostOptions)(long *options);
	void ASAPI (*GetHostModifyPaletteLayoutBoundsProc)(ModifyPaletteLayoutBoundsProc *proc);
	
#ifdef MAC_ENV
	ADMWindowActivateProc ASAPI (*GetWindowActivateProc)();
	void ASAPI (*GetModalEventProc)(ADMModalEventProc *modalEventProc);
#endif

#ifdef WIN_ENV
	ASWindowRef ASAPI (*GetAppWindow)();
	void ASAPI (*GetUIResourceDirectory)(SPPlatformFileSpecification* resDir);	// default is 'System'
#endif
}
ADMHostInitSuite1;

typedef ADMHostInitSuite1 ADMHostInitSuite;

#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif
