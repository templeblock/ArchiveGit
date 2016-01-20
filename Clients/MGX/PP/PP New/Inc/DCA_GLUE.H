/***********************************************************************
 TWAIN source code:
 Copyright (C) '91-'92 TWAIN Working Group: 
 Aldus, Caere, Eastman-Kodak, Logitech,
 Hewlett-Packard Corporations.
 All rights reserved.
*************************************************************************/

/* Defines for the current_state table used in Explain_Error */
#define E_CLOSEDSM          (TCHAR *)_T("Error Closing DSM.  ")
#define E_CLOSEDS           (TCHAR *)_T("Error Closing DS.  ")
#define E_USERSELECT        (TCHAR *)_T("Error Accessing DS.  ")
#define E_SETUPMEMXFER      (TCHAR *)_T("Error Setting up memory transfer.  ")
#define E_DISABLEDS         (TCHAR *)_T("Error Disabling DS.  ")
#define E_ENABLEDS          (TCHAR *)_T("Error Enabling DS.  ")
#define E_GETFIRST          (TCHAR *)_T("Error Getting first Data Source.  ")
#define E_GETNEXT           (TCHAR *)_T("Error Getting Next Data Source.  ")
#define E_CAPPIXELGET       (TCHAR *)_T("Error Getting Cap Pixel Type.  ")
#define E_CAPPIXELSET       (TCHAR *)_T("Error Setting Cap Pixel Type.  ")


/***********************************************************************/
/* Function prototypes from module DCA_GLUE.C */
/***********************************************************************/

// Candy routines
VOID DCInitialize (pTW_IDENTITY, HWND);
VOID DCTerminate (VOID);

// Routines for DSM
BOOL DCOpenDSM (VOID);
BOOL DCCloseDSM (VOID);
BOOL DCIsDSMOpen (VOID);

// Routines for DS
BOOL DCOpenDS (VOID);
VOID DCCloseDS (VOID);
BOOL DCEnableDS (VOID);
VOID DCDisableDS (VOID);
BOOL DCIsDSOpen (VOID);
TW_UINT32 DCGetPixelType (VOID);
TW_UINT16 DCSetPixelType (TW_UINT16, TW_INT32);
BOOL DCSelectDS (VOID);

// Routines for Errors/Interface
VOID Explain_Error (HWND, TCHAR *);
VOID GreyMenu (int);

#define VALID_HANDLE    32      // valid windows handle SB >= 32

