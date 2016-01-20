/***********************************************************************
 TWAIN source code:
 Copyright (C) '91-'92 TWAIN Working Group:
 Aldus, Caere, Eastman-Kodak, Logitech,
 Hewlett-Packard Corporations.
 All rights reserved.
*************************************************************************/

#define DEBUG           // Show errors that result from application
						// calls to Source Manager

#include "pp.h"
#include "ppafx.h"
#include "twain.h"      // for TW data type defines
#include "dca_glue.h"   // for function prototypes
#include "dcd_com.h"    // common lib of container routines
#include "id.h"
#include "framelib.h"

// Globals to this module ONLY
static BOOL DCDSMOpen = FALSE;    // glue code flag for an Open Source Manager
static HMODULE hDSMDLL = 0;        // handle to Source Manager for explicit load
static HWND hWnd;                 // global for window handle

// Globals
BOOL DCDSOpen  = FALSE;           // glue code flag for an Open Source
TW_IDENTITY appID, dsID;          // storage for App and DS (Source) states
DSMENTRYPROC lpDSM_Entry;         // entry point to the SM
BOOL bTwainUIEnabled = FALSE;

int DCItemSize[]= { 
	sizeof(TW_INT8),
	sizeof(TW_INT16),
	sizeof(TW_INT32),
	sizeof(TW_UINT8),
	sizeof(TW_UINT16),
	sizeof(TW_UINT32),
				
	sizeof(TW_BOOL),
	sizeof(TW_FIX32),
	sizeof(TW_FRAME),
	sizeof(TW_STR32),
	sizeof(TW_STR64),
	sizeof(TW_STR128),
	sizeof(TW_STR255),
};

/***********************************************************************
 * FUNCTION: DCInitialize
 * 
 * ARGS:    pIdentity information about the App
 *          hMainWnd  handle to App main window
 * 
 * RETURNS: none
 *
 * NOTES:   This simple copy to a static structure, appID, does not support
 *          multiple connections. TODO, upgrade.
 *
 */

VOID DCInitialize (pTW_IDENTITY pIdentity, HWND hMainWnd)
{
	appID = *pIdentity;    // get copy of app info
	hWnd = hMainWnd;       // get copy of app window handle
}


/***********************************************************************
 * FUNCTION:    DCTerminate
 * 
 * ARGS:    none
 * 
 * RETURNS: none
 *
 * NOTES:    To properly back out of a connection to a Source:
 *                DCDisableDS      disable the Source
 *                DCCloseDS        close the Source
 *                DCCloseDSM       close the Source Manager
 *
 * The simple parameterless calls do not support multiple connections.
 */

VOID DCTerminate (VOID)
{
	DCDisableDS ();     // this is required by 'the rules'
	DCCloseDS ();
	DCCloseDSM ();
}

/***********************************************************************
 * FUNCTION: DCOpenDSM
 * 
 * ARGS:    none
 * 
 * RETURNS: current state of the Source Manager
 *
 * NOTES:     1). be sure SM is not already open
 *            2). explicitly load the .DLL for the Source Manager
 *            3). call Source Manager to:
 *                - opens/loads the SM
 *                - pass the handle to the app's window to the SM
 *                - get the SM assigned appID.id field
 *
 */

BOOL DCOpenDSM (VOID)
{
	#define       WINDIRPATHSIZE 160

	char          WinDir[WINDIRPATHSIZE];
	TW_UINT16     dcRC;
	TW_STR32      DSMName;

	// Only open SM if currently closed
	if (DCDSMOpen!=TRUE){
		// Open the SM, Refer explicitly to the library so we can post a nice
		// message to the the user in place of the "Insert TWAIN32.DLL in drive A:"
		// posted by Windows if the SM is not found.

		GetWindowsDirectory (WinDir, WINDIRPATHSIZE);
#ifdef WIN32
		lstrcpy (DSMName, "TWAIN32.DLL");
#else
		lstrcpy (DSMName, "TWAIN.DLL");
#endif
		lstrcat (WinDir, "\\");
		lstrcat (WinDir, DSMName);


		if (FileExists(WinDir) &&
			 (hDSMDLL =     (HMODULE)LoadLibrary(DSMName)) != NULL &&
#ifdef WIN32
			 (lpDSM_Entry = (DSMENTRYPROC)GetProcAddress(hDSMDLL, "DSM_Entry")) != NULL){
#else
			 (lpDSM_Entry = (DSMENTRYPROC)GetProcAddress(hDSMDLL, "DSM_ENTRY")) != NULL){
#endif WIN32

			/* This call performs four important functions:
			- opens/loads the SM
			- passes the handle to the app's window to the SM
			- returns the SM assigned appID.id field
			- be sure to test the return code for SUCCESSful open of SM
			*/
			dcRC = (*lpDSM_Entry)(&appID, NULL,
				DG_CONTROL, DAT_PARENT, MSG_OPENDSM, (TW_MEMREF)&hWnd);


			switch (dcRC){
				case TWRC_SUCCESS:
					// Needed for house keeping.  Do single open and do not
					// close SM which is not already open ....
					DCDSMOpen = TRUE;
				break;

				case TWRC_FAILURE:
				default:
					// Trouble opening the SM, inform the user
					DCDSMOpen = FALSE;
					Explain_Error (hWnd, NULL);
				break;
			}
		} else {
			Explain_Error (hWnd, NULL);
		}
	}

	return (DCDSMOpen);
}


/***********************************************************************
 * FUNCTION: DCCloseDSM
 * 
 * ARGS:    none
 * 
 * RETURNS: current state of Source Manager
 *
 * NOTES:    1). be sure SM is already open
 *           2). call Source Manager to:
 *               - request closure of the Source identified by appID info
 *
 */

BOOL DCCloseDSM (VOID)
{
	TW_UINT16 dcRC;

	// Only close something which is already open
	if (DCDSMOpen==TRUE){
		/* This call performs one important function:
		- tells the SM which application, appID.id, is requesting SM to close
		- be sure to test return code, failure indicates SM did not close !!
		*/

		dcRC = (*lpDSM_Entry)(&appID,   NULL,
			DG_CONTROL, DAT_PARENT, MSG_CLOSEDSM, (TW_MEMREF)&hWnd);

		if (dcRC != TWRC_SUCCESS){
			// Trouble closing the SM, inform the user
			Explain_Error (hWnd, E_CLOSEDSM);

		} else {
			DCDSMOpen = FALSE;

			// Explicitly free the SM library
			if (hDSMDLL){
				FreeLibrary (hDSMDLL);
				hDSMDLL=0;
			}
		}
	}

	return (DCDSMOpen);
}


/***********************************************************************
 * FUNCTION: DCIsDSMOpen
 * 
 * ARGS:    none
 * 
 * RETURNS: current state of Source Manager (open/closed)
 *
 * NOTES:   Just a way to reduce the number of global vars and keep the
 *          state of SM information local to this module.  Let the caller,
 *          app, know current state of the SM.
 */

BOOL DCIsDSMOpen (VOID)
{
	return (DCDSMOpen);
}

/***********************************************************************
 * FUNCTION: DCOpenDS
 *
 * ARGS:    none
 *
 * RETURNS: current state of select Source
 *
 * NOTES:    1). only attempt to open a source if it is currently closed
 *           2). call Source Manager to:
 *                - open the Source indicated by info in dsID
 *                - SM will fill in the unique .Id field
 */

BOOL DCOpenDS (VOID)
{
	TW_UINT16 dcRC=TWRC_FAILURE;

	if (DCDSOpen==FALSE){
		// This will open the Source.
		dcRC = (*lpDSM_Entry)(&appID,   NULL,
			DG_CONTROL, DAT_IDENTITY, MSG_OPENDS, &dsID);

		if (dcRC == TWRC_SUCCESS){
			// do not change flag unless we successfully open
			DCDSOpen = TRUE;
		} else {
			// Trouble opening the Source
			Explain_Error (hWnd, NULL);
		}
	}

	return DCDSOpen;
}


/***********************************************************************
 * FUNCTION: DCCloseDS
 *
 * ARGS:    none
 *
 * RETURNS: none
 *
 * NOTES:    1). only attempt to close an open Source
 *           2). call Source Manager to:
 *                - ask that Source identified by info in dsID close itself
 */

VOID DCCloseDS (VOID)
{
	TW_UINT16         dcRC=TWRC_FAILURE;

	if (DCDSOpen==TRUE){
		// Close an open Source
		dcRC = (*lpDSM_Entry)(&appID, NULL,
			DG_CONTROL, DAT_IDENTITY, MSG_CLOSEDS, &dsID);

		// show error on close
		if (dcRC!= TWRC_SUCCESS) {
			Explain_Error (hWnd, E_CLOSEDS);
		} else {
			DCDSOpen = FALSE;
		}
	}
	return;
}

/***********************************************************************
 * FUNCTION: DCEnableDS
 *
 * ARGS:    none
 *
 * RETURNS: BOOL for TRUE=open; FALSE=not open/fail
 *
 * NOTES:    1). only enable an open Source
 *           2). call the Source Manager to:
 *                - bring up the Source's User Interface
 */

BOOL DCEnableDS (VOID)
{
	BOOL      Result=FALSE;
	TW_UINT16 dcRC=TWRC_FAILURE;

	TW_USERINTERFACE dcUI;

	// only enable open Source's
	if (DCDSOpen==TRUE){

		// This will display the Source User Interface. The Source should only display
		// a user interface that is compatible with the group defined
		// by appID.SupportedGroups (in our case DG_IMAGE | DG_CONTROL)
		dcUI.hParent = hWnd;
		dcUI.ShowUI  = TRUE;
		dcRC = (*lpDSM_Entry)(&appID, &dsID,
			DG_CONTROL, DAT_USERINTERFACE, MSG_ENABLEDS, 
			(TW_MEMREF)&dcUI);

		if (dcRC != TWRC_SUCCESS) {
			Explain_Error (hWnd, E_ENABLEDS);
		} else {
			bTwainUIEnabled = TRUE;
			Result = TRUE;            
		}
	}

	return Result;
}


/***********************************************************************
 * FUNCTION: DCDisableDS
 *
 * ARGS:    none
 *
 * RETURNS: none
 *
 * NOTES:    1). only disable an open Source
 *           2). call Source Manager to:
 *                - ask Source to hide it's User Interface
 */

VOID DCDisableDS (VOID)
{
	TW_UINT16        dcRC=TWRC_FAILURE;
	TW_USERINTERFACE dcUI;

	// only disable open Source's
	if (DCDSOpen==TRUE){
		// Hide the Source UI
		dcUI.hParent = hWnd;
		dcUI.ShowUI  = FALSE;

		dcRC = (*lpDSM_Entry)(&appID, &dsID,
			DG_CONTROL, DAT_USERINTERFACE, MSG_DISABLEDS,
			(TW_MEMREF)&dcUI);

		if (dcRC!= TWRC_SUCCESS) {
			Explain_Error (hWnd, E_DISABLEDS);
		}
		bTwainUIEnabled = FALSE;
	}

	return;
}

/***********************************************************************
 * FUNCTION: DCIsDSOpen
 *
 * ARGS:    none
 *
 * RETURNS: current state of the Source
 *
 * NOTES:    Just a way to reduce the number of global vars and keep the
 *           state of Source information local to this module.  Let the caller,
 *           app, know current state of the Source.
 */
BOOL DCIsDSOpen (VOID)
{
	return (DCDSOpen);
}

/***********************************************************************
 * FUNCTION: DCGetPixelType
 *
 * ARGS:    none
 *
 * RETURNS: the current pixel type
 *
 * NOTES:   1). selects the CAP type, ICAP_PIXELTYPE
 *          2). calls the Source Manager to:
 *              - have Source build a container who's contents will be the
 *                current pixel type of the selected Source
 *          3). use common lib routine to extract value from the container
 *              type built, Source's choice, by the Source
 */

TW_UINT32 DCGetPixelType (VOID)
{
	TW_CAPABILITY   dcCap;
	TW_UINT16       dcRC = TWRC_FAILURE;
	TW_INT32        back=0L;

	// App ONLY fills in the CAP_ of interest
	dcCap.Cap = ICAP_PIXELTYPE;

	// Have Source build a container and fill it with the CAP_ of interest
	dcRC = (*lpDSM_Entry)(&appID, &dsID,
		DG_CONTROL, DAT_CAPABILITY, MSG_GET,
		(TW_MEMREF)&dcCap);

	if (dcRC!= TWRC_SUCCESS) {
		Explain_Error (hWnd, E_CAPPIXELGET);
	}

	// Add code to do a switch on contype.  Then call the appropriate extract
	// routine from the common container code routines.

	switch (dcCap.ConType){
		case TWON_ONEVALUE:
			ExtractOneValue (&dcCap, &back);
		break;

		case TWON_ENUMERATION:
			// get a single value from the enumeration container
			ExtractEnumerationValue (&dcCap, &back, 1);
		break;

		default:
		break;
	}

	// App is ALWAYS responsible for cleaning up the container
	if (dcCap.hContainer) {
		GlobalFree (dcCap.hContainer);
	}
	return (back);
}

/***********************************************************************
 * FUNCTION: DCSetPixelType
 *
 * ARGS:    ItemType    DCTY_xxx, type of the ItemValue
 *          ItemValue   the none DCPT_xxx constant for pixel type, see DC.H
 *
 * RETURNS: dcRC TWAIN status return code
 *
 * NOTES:   1). build up a container of type OneValue filled with ItemType
 *              and ItemValue
 *          2). call Source Manager to:
 *              - give the Source access to the pixel type you wish to set
 *                the Source to, assumes you have asked the Source previously
 *                if it can handle the pixel type you are now setting
 */

TW_UINT16 DCSetPixelType (TW_UINT16 ItemType, TW_INT32 ItemValue)
{
	TW_CAPABILITY     dcCap;
	TW_UINT16         dcRC = TWRC_FAILURE;

	dcCap.Cap        = ICAP_PIXELTYPE;   // id of cap you want
	dcCap.ConType    = TWON_ONEVALUE;    // container type

	// App must build the container & pass the container handle to the Source
	BuildUpOneValue (&dcCap, ItemType, ItemValue);

	// It is assumed that the Source will read the container NOW
	dcRC = (*lpDSM_Entry)(&appID, &dsID,
		DG_CONTROL, DAT_CAPABILITY, MSG_SET,
		(TW_MEMREF)&dcCap);

	if (dcRC!= TWRC_SUCCESS) {
		Explain_Error (hWnd, E_CAPPIXELSET);
	}

	// NOTE: the App ALWAYS is required to Free the container
	if (dcCap.hContainer) {
		GlobalFree (dcCap.hContainer);
	}
	return dcRC;
}

/***********************************************************************
 * FUNCTION: DCSelectDS
 *
 * ARGS:    none
 *
 * RETURNS: dcRC TWAIN status return code
 *
 * NOTES:   1). call the Source Manager to:
 *              - have the SM put up a list of the available Sources
 *              - get information about the user selected Source from
 *                NewDSIdentity, filled by Source
 */
BOOL DCSelectDS (VOID)
{
	TW_UINT16   dcRC;
	TW_IDENTITY NewDSIdentity;

	// I will settle for the system default.  Shouldn't I get a highlight
	// on system default without this call?
	dcRC = (*lpDSM_Entry)(&appID, NULL,
		DG_CONTROL, DAT_IDENTITY, MSG_GETDEFAULT,
		(TW_MEMREF)&NewDSIdentity);

	/* This call performs one important function:
	- should cause SM to put up dialog box of available Source's
	- tells the SM which application, appID.id, is requesting, REQUIRED
	- returns the SM assigned NewDSIdentity.id field, you check if changed
	- (needed to talk to a particular Data Source)
	- be sure to test return code, failure indicates SM did not close !!
	*/
	dcRC = (*lpDSM_Entry)(&appID, NULL,
		DG_CONTROL, DAT_IDENTITY, MSG_USERSELECT,
		(TW_MEMREF)&NewDSIdentity);

	/* Check if the user changed the Source and react as apporpriate.
	- TWRC_SUCCESS, log in new Source
	- TWRC_CANCEL,  keep the current Source
	- default,      check down the codes in a status message, display result
	*/

	switch (dcRC){
		case TWRC_SUCCESS:
			dsID = NewDSIdentity;
		break;

		case TWRC_CANCEL:
		break;

		default:
			Explain_Error (hWnd, E_USERSELECT);
		break;
	}

	return (dcRC);
}


/***********************************************************************
 * FUNCTION: Explain_Error
 *
 * ARGS:    hWnd    handle to a display window
 *          *state  ptr to a character string from dca_glue.h which defines
 *                  these error strings.  Iff state is NULL then no extra 
 *                  text, probably using the standard App error messages,
 *                  (See dcastrg.rc, stringtable)
 *
 * RETURNS: none
 *
 */

VOID Explain_Error(HWND hWnd, char *state)
{
#ifdef DEBUG
	TW_STATUS dcStatus;
	TW_UINT16 dcRC;
	char      Details [255];
	char         twainErrorString[255];
	int       code;

	// Put in detail of current operation
	lstrcpy (Details, state);

	if (*lpDSM_Entry){
		// determine details of failure from SM
		dcRC = (*lpDSM_Entry)(&appID, NULL,
			DG_CONTROL, DAT_STATUS, MSG_GET,
			(TW_MEMREF)&dcStatus);

		if (dcRC == TWRC_SUCCESS){
			switch (dcStatus.ConditionCode){
				case TWCC_BADCAP:           code = IDS_BADCAP;         break;
				case TWCC_BADPROTOCOL:    code = IDS_BADPROTOCOL;    break;
				case TWCC_BADVALUE:       code = IDS_BADVALUE;       break;
				case TWCC_BUMMER:         code = IDS_BUMMER;         break;

				// Error in the DS or DSM which was already reported to 
				// the user.  The App should not be redundant.  
				// The operation failed.
				case TWCC_OPERATIONERROR: return; break;

				case TWCC_LOWMEMORY:      code = IDS_LOWMEMORY;      break;
				case TWCC_MAXCONNECTIONS: code = IDS_MAXCONNECTIONS; break;
				case TWCC_NODS:           code = IDS_NODS;           break;
				case TWCC_SEQERROR:       code = IDS_SEQERROR;       break;
				case TWCC_SUCCESS:        code = IDS_SUCCESS;        break;

				default:                  code = IDS_DEFAULT;        break;
			}
		} else {
			code = IDS_NODETAIL;
		}
	} else {
		code = IDS_NODSM;
	}

	// Now send out the text
	AstralStrEx(code, Details, 255);
	AstralStrEx(IDS_TWAINERROR, twainErrorString, 255);

	// show the error
	MessageBox(PictPubApp.Get_hWndAstral(), Details, twainErrorString, MB_OK | MB_ICONEXCLAMATION );

#endif /* DEBUG */
	return;
}



