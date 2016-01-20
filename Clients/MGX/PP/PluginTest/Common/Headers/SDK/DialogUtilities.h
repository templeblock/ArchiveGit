//-------------------------------------------------------------------------------
//
//	File:
//		DialogUtilities.h
//
//	Copyright 1993-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the prototypes, macros, and defines
//		related to Macintosh dialog manipulation. 
//
//	Use:
//		DialogUtilities is intended to make Macintosh dialog
//		manipulation a little simpler by providing a standard
//		suite of functions.  Use these routines for whatever you
//		need, and feel free to tweak them for your own work.  They
//		expect A4-globals to be set-up already, and for gStuff to
//		be a valid pointer to your global structure's parameter
//		block.
//
//	Version history:
//		Version 1.0.0	3/1/1993	Created for Photoshop 2.5
//			Written by Thomas Knoll
//
//		Version 1.0.1	4/8/1997	Updated for Photoshop 4.0.1
//			Comments fattened out.  Reorganized for clarity.
//
//-------------------------------------------------------------------------------
 
#ifndef __DialogUtilities_H__ 	// Has this not been defined yet?
#define __DialogUtilities_H__ 	// Only include this once by predefining it

#include <Dialogs.h>			// Macintosh standard dialogs
#include <Types.h>				// Macintosh standard types
#include <Gestalt.h>			// Macintosh gestalt routines
#include <TextUtils.h>			// Macintosh text utilities (GetString())
#include <OSUtils.h>			// Macintosh OS Utilities (Delay())
#include <Menus.h>				// Macintosh Menu Manager routines (DelMenuItem())

#include "PITypes.h"			// Photoshop types
#include "PIGeneral.h"			// Photoshop general routines
#include "PIUtilities.h"		// SDK Utility routines

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//-------------------------------------------------------------------------------
//	Prototypes -- Alerts
//-------------------------------------------------------------------------------

// Display a centered alert:
short ShowAlert (const short alertID);

// Display a cross-platform alert with a version number:
short ShowVersionAlert (Handle hDllInstance,
						DialogPtr dp,
						const short alertID, 
						const short stringID,
						Str255 versText1,
						Str255 versText2);

// Display a cross-platform caution alert from a string:
short ShowAlertType (Handle hDllInstance,
				 	 DialogPtr dp,
				     short alertID, 
				     short stringID, 
				     Str255 minText, 
				     Str255 maxText,
				     const short alertType);

//-------------------------------------------------------------------------------
//	Alert constants and macros
//-------------------------------------------------------------------------------

enum
{ // constants defined for alert routine

	PIAlertCaution,
	PIAlertStop
};

// Macro to pop a cross-platform caution alert:
#define ShowCaution(h, dp, alertID, stringID, min, max)	\
	ShowAlertType(h, dp, alertID, stringID, min, max, PIAlertCaution)
	
// Macro to pop a cross-platform stop alert:
#define ShowStop(h, dp, alertID, stringID, min, max)	\
	ShowAlertType(h, dp, alertID, stringID, min, max, PIAlertStop)

//-------------------------------------------------------------------------------
//	Prototypes -- About dialogs
//-------------------------------------------------------------------------------

// pop an about box:
void ShowAbout (const OSType hostSign, const short dialogID);

//-------------------------------------------------------------------------------
//	Prototypes -- Dialog initialization and handling
//-------------------------------------------------------------------------------

// center a dialog template 1/3 of the way down on the main screen:
void CenterDialog (DialogTHndl dt);

// setup a moveable modal
void SetUpMoveableModal (DialogTHndl dt, const OSType hostSig);
						 
// Run a moveable modal dialog (use instead of ModalDialog)						 
// DON'T CALL THIS WITH NON-MOVEABLE MODALS, IT WILL CRASH
void MoveableModalDialog (DialogPtr dp,
						  ProcessEventProc processEvent,
						  ModalFilterProcPtr filter,
						  short *item);
						  
// Except within the filter procedure for such moveable modal dialogs, we need
// to call the following routine to get the window refcon since
// MoveableModalDialog stomps it.
long GetMoveableWRefCon (DialogPtr dp);

// Dialog filter proc to handle and dispatch basic events:
static pascal Boolean DialogFilter (DialogPtr dp,
									EventRecord *event,
									short *item);
									
//-------------------------------------------------------------------------------
//	Dialog handling -- Constants and macros
//-------------------------------------------------------------------------------

// Used by MoveableModalDialog, PerformStandardDialogItemHandling:
#define RETURN	0x0D
#define ENTER	0x03
#define PERIOD	'.'
#define ESCAPE	0x1B
#define	TAB		'\t'

// Standard MenuBar height, used by CenterDialog:
#define MenuHeight	20

// Used in DialogFilter, MoveableModalDialog, GetMoveableWRefCon:
typedef struct ModalData
{
	long oldRefCon; // old reference value
	
	// ModalFilterProcPtr is defined as: typedef pascal Boolean
	// (*ModalFilterProcPtr)(DialogPtr theDialog,
	//						 EventRecord *theEvent,
	//						 short *itemHit)
	ModalFilterProcPtr filter;
	
	ProcessEventProc processEvent; // Basic event processor in Events.h
	
} ModalData;

//-------------------------------------------------------------------------------
//	Prototypes -- Dialog outline buttons and groups
//-------------------------------------------------------------------------------

// Set an item hook so that it will outline the OK button in a dialog:
void SetOutlineOK (DialogPtr dp, const short item);

// Dialog proc to outline the OK button:
static pascal void OutlineOK (DialogPtr dp, const short item);

// The following routine sets a user item to be a group box.  It expects
// the next item to be the title for the group box:
void SetOutlineGroup (DialogPtr dp, const short item);

// Dialog filter proc to outline a group:
static pascal void OutlineGroup (DialogPtr dp, const short item);

//-------------------------------------------------------------------------------
//	Prototypes -- Text manipulation
//-------------------------------------------------------------------------------

// The following routine selects an edit text item:
void SelectTextItem (DialogPtr dp, const short item);

// The following routine extracts the text of a text item:
void FetchText (DialogPtr dp, const short item, Str255 text);

// The following routine sets the text of a text item:
void StuffText (DialogPtr dp, const short item, Str255 text);

//-------------------------------------------------------------------------------
//	Prototypes -- Number manipulation, range checking and reporting
//-------------------------------------------------------------------------------

// The following routine stuffs a long value into a text field:
void StuffNumber (DialogPtr dp, const short item, const long value);

// The following routine retrieves the value from a text field.
// It will do range checking and pin the number to bounds.  It
// returns noErr (0) if it gets a valid value in the field:
short FetchNumber (DialogPtr dp,
 					  const short item,
					  const long min,
					  const long max,
					  long *value);

// This corresponding alert routine will pop a dialog telling the user the
// range was not valid:
void AlertNumber (DialogPtr dp,
				  const short item,
				  const long min,
				  const long max,
				  long *value,
				  Handle hDllInstance,
				  const short alertID,
				  const short numberErr);
				  				 
// Stuffs a double value into a text field:
void StuffDouble (DialogPtr dp, 
				  const short item,
				  const double value,
				  const short precision);
 
// The following routine retrieves a double value from a text field.
// It will do range checking and pin the number to bounds.  It
// returns noErr (0) if it gets a valid value in the field:
short FetchDouble (DialogPtr dp,
					 const short item,
					 const double min,
					 const double max,
					 double *value);

// This corresponding alert routine will pop a dialog telling the user the
// range was not valid:
void AlertDouble (DialogPtr dp,
				  const short item,
				  const double min,
				  const double max,
				  double *value,
				  Handle hDllInstance,
				  const short alertID,
				  const short numberErr);
					 
//-------------------------------------------------------------------------------
//  Number manipulation, range checking and reporting -- constants
//-------------------------------------------------------------------------------

enum
{ // return values for Fetch routines (FetchNumber, FetchDouble, etc.)

	// noErr = 0
	errOutOfRange = 1,
	errNotANumber
};

//-------------------------------------------------------------------------------
//	Prototypes -- Setting check box, radio button, pop-up menu and group states
//-------------------------------------------------------------------------------

// Perform standard handling for check boxes and radio buttons. For radio
// buttons, we assume that the group for the radio button extends forward
// and backward in the DITL as long as the item type is radio button:
void ProcessChecksAndRadios (DialogPtr dp, const short item);
						  
// Set the state of a check box (or radio button):
void SetCheckBoxState (DialogPtr dp, const short item, const Boolean checkIt);

// Determine the state of a check box (or radio button):
Boolean GetCheckBoxState (DialogPtr dp, const short item);

// Toggle a check box and return the new state:
Boolean ToggleCheckBoxState (DialogPtr dp, const short item);

// Set a radio group (from first to last item) to reflect the selection:
void SetRadioGroupState (DialogPtr dp,
						 const short first,
						 const short last,
						 const short item);
						 
// Get the selected button within a radio group:
short GetRadioGroupState (DialogPtr dp,
						  const short first,
						  const short last);

// Set the value for a pop-up menu:
void SetPopUpMenuValue (DialogPtr dp,
						const short item,
						const short newValue);

// Get the value for a pop-up menu:
short GetPopUpMenuValue (DialogPtr dp,
						 const short item);

//-------------------------------------------------------------------------------
//	Prototypes -- Showing, hiding, enabling and disabling items
//-------------------------------------------------------------------------------

// Utility routine to show or hide an item:
void ShowHideItem (DialogPtr dp, const short inItem, const Boolean inState);

// Utility routine to disable (grey) a control:
void DisableControl (DialogPtr dp, const short inItem);

// Utility routine to enable a control:
void EnableControl (DialogPtr dp, const short inItem);

// Utility routine to enable (TRUE) or disable (FALSE) a control:
void EnableDisableControl (DialogPtr dp, const short inItem, const Boolean inState);

//-------------------------------------------------------------------------------
//	Prototypes -- misc
//-------------------------------------------------------------------------------

// Utility routine to invalidate an item:
void InvalItem (DialogPtr dp, short item);

// Little routine to flash a button set off by a keystroke:
void FlashDialogButton (DialogPtr dp, short item);

// Locate the QuickDraw globals:
QDGlobals *GetQDGlobals (void);

// Set cursor to arrow cursor (requires QDGlobals()):
void SetArrowCursor (void);

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
} // End of extern "C" block.
#endif // __cplusplus

//-------------------------------------------------------------------------------

#endif // __DialogUtilities_H__
