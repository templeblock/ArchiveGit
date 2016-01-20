/*
 * Name:
 *	ADMWindow.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	ADM Window Suite
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 1/1/95	MF	First version.
 *		Created by Matt Foster.
 */


#ifndef __ADMWindow__
#define __ADMWindow__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#ifndef __ADMTypes__
#include "ADMTypes.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

#pragma PRAGMA_ALIGN_BEGIN
#pragma PRAGMA_IMPORT_BEGIN


/*******************************************************************************
 **
 ** Constants
 **
 **/

#define kADMWindowSuite			"ADM Window Suite"
#define kADMWindowSuiteVersion	1

typedef enum
{
	// Note: these first four correspond to ADMDialogStyles
	// and are all variants of type kADMWindow
	kADMFloatingWindowStyle					= 2,
	kADMTabbedFloatingWindowStyle			= 3,
	kADMResizingFloatingWindowStyle			= 4,
	kADMTabbedResizingFloatingWindowStyle	= 5,
	
	kADMDocumentWindowStyle					= 100,	// for searches only, to find all document window types
	kADMGrowNoZoomDocumentWindowStyle		= 100,	// for new window calls, use a specific type
	kADMNoGrowNoZoomDocumentWindowStyle		= 104,
	kADMGrowZoomDocumentWindowStyle			= 108,
	kADMNoGrowZoomDocumentWindowStyle		= 112,
	
	kADMPopupWindowStyle					= 300,
	
	kADMDummyWindowStyle 					= 0xFFFFFFFF
}
ADMWindowStyle;

/*******************************************************************************
 **
 **	Suite
 **
 **/


typedef struct ADMWindowSuite1 {
	ASAPI ASWindowRef (*NewLayerWindow)(ADMWindowStyle windowKind, unsigned char *wStorage, ASRect * boundsRect, 
									char *title, ASBoolean visible, ASBoolean goAwayFlag, long refCon);
	ASAPI ASWindowRef (*GetNewLayerWindow)(SPPluginRef pluginRef, ADMWindowStyle windowKind, 
									short windowID,  unsigned char *wStorage);
	ASAPI void (*DisposeLayerWindow)(ASWindowRef theWindow);

	ASAPI void (*HideLayerWindow)(ASWindowRef theWindow);
	ASAPI void (*ShowAndSelectLayerWindow)(ASWindowRef theWindow);
	ASAPI void (*SelectLayerWindow)(ASWindowRef theWindow);
	ASAPI void (*ShowLayerWindow)(ASWindowRef theWindow);
	
	ASAPI void (*DragLayerWindow)(ASWindowRef theWindow, ASPoint startPoint, ASRect *draggingBounds);

	ASAPI ASWindowRef (*GetFrontWindow)(ADMWindowStyle windowKind);
	ASAPI void (*UpdateWindowOrder)(void);
} ADMWindowSuite1;

typedef ADMWindowSuite1 ADMWindowSuite;


#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif

#endif
