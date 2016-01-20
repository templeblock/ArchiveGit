/*
 * Name:
 *	PIUIHooksSuite.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	This file contains all the public definitions and
 *	structures related to user interface elements.
 *
 *	Use the UIHooks suite to get the main application
 *	window (on Windows), access a handful of Photoshop
 *	cursors, and similar utilities.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *		Version 1.0.0	1/1/1998	JF		Created for Photoshop 5.0.
 *			Compiled by Josh Freeman.
 */

#ifndef __PIUIHooksSuite__
#define __PIUIHooksSuite__

//-------------------------------------------------------------------------------
//	Includes.
//-------------------------------------------------------------------------------
#include "PIGeneral.h"

//-------------------------------------------------------------------------------
//	C++ wrapper.
//-------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------------
//	UI hooks suite.
//-------------------------------------------------------------------------------
#define kPSUIHooksSuite 			"Photoshop UIHooks Suite for Plug-ins"

//-------------------------------------------------------------------------------
//	MainAppWindow.  Returns Windows parent window HWND, NULL on Mac.
//-------------------------------------------------------------------------------
typedef SPAPI long (*MainAppWindowProc) (void);

//-------------------------------------------------------------------------------
//	SetCursor.  Sets cursor to some popular Photoshop cursors.  Mac and Win.
//-------------------------------------------------------------------------------

// Cursor IDs:
#define kPICursorNone				NULL
#define kPICursorArrow				"Photoshop Cursor Arrow"
#define kPICursorWatch				"Photoshop Cursor Watch"
#define kPICursorWatchContinue		"Photoshop Cursor Watch Continue"
#define kPICursorMove				"Photoshop Cursor Move"
#define kPICursorHand				"Photoshop Cursor Hand"
#define kPICursorGrab				"Photoshop Cursor Grab"
#define kPICursorZoomIn				"Photoshop Cursor Zoom In"
#define kPICursorZoomOut			"Photoshop Cursor Zoom Out"
#define kPICursorZoomLimit			"Photoshop Cursor Zoom Limit"
#define kPICursorPencil				"Photoshop Cursor Pencil"
#define kPICursorEyedropper			"Photoshop Cursor Eyedropper"
#define kPICursorEyedropperPlus		"Photoshop Cursor Eyedropper Plus"
#define kPICursorEyedropperMinus	"Photoshop Cursor Eyedropper Minus"
#define kPICursorBucket				"Photoshop Cursor Bucket"
#define kPICursorCrosshair			"Photoshop Cursor Crosshair"
#define kPICursorType				"Photoshop Cursor Type"
#define kPICursorPathArrow			"Photoshop Cursor Path Arrow"

typedef char* PICursor_t;
	
typedef SPAPI SPErr (*HostSetCursorProc) (const PICursor_t cursorID);
	
//-------------------------------------------------------------------------------
//	TickCount.  Gets the tick count as 60 ticks per second.  Mac and Win.
//-------------------------------------------------------------------------------
typedef SPAPI unsigned long (*HostTickCountProc) (void);


//-------------------------------------------------------------------------------
//	GetPluginName.  Returns the name of the plugins specified by pluginRef.
//-------------------------------------------------------------------------------
typedef SPAPI SPErr (*PluginNameProc) (SPPluginRef pluginRef, ASZString *pluginName);


//-------------------------------------------------------------------------------
//	UI hooks suite version 1.
//-------------------------------------------------------------------------------

#define kPSUIHooksSuiteVersion1		1

typedef struct 
	{
	ProcessEventProc 				processEvent;
	DisplayPixelsProc				displayPixels;
	ProgressProc					progressBar;
	TestAbortProc					testAbort;
	MainAppWindowProc				MainAppWindow;
	HostSetCursorProc				SetCursor;
	HostTickCountProc				TickCount;
	PluginNameProc					GetPluginName;

	} PSUIHooksSuite1;

//-------------------------------------------------------------------------------
//	Error codes.
//-------------------------------------------------------------------------------

#define kPSCursorError				'!cur'

//-------------------------------------------------------------------------------
//	C++ wrapper.
//-------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif

//-------------------------------------------------------------------------------
#endif	// PIUIHooksSuite
