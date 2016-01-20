///////////////////////////////////////////////////////////////////////////////
//
//  twainWrapper.h
//
//  declarations for the BTwainWrapper class, which encapsulates TWAIN
//  functionality
//
//  Copyright (c) 1997, Broderbund Software, Inc.
//  All rights reserved.
//
//  Written by John D. Fleischhauer Software Engineering
//
//  NOTE: The ultimate intention is that this class be portable.  However, for
//        the time being, this version is mostly Windows-specific, and the
//        Mac stuff will have to be handled later.   Note that twain.h itself
//        must also be modified for Mac.  -  3/26/97 jdf
//
///////////////////////////////////////////////////////////////////////////////

#ifndef TWAINWRAPPER_H
#define TWAINWRAPPER_H

#ifndef STRICT
#define STRICT
#endif

#include <windows.h>                    // required by twain.h Windows version
#include "twain.h"                      // TWAIN API declarations

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
//  The following string is passed to RegisterWindowMessage() to create a
//  unique message ID.  This ID is stored in the global gWM_BTWAIN_IMAGEXFER_ID.
//
//  A message with this ID is sent to the host application when image
//  acquisition is complete; the wParam contains an HBITMAP, a handle to the
//  acquired image in native (DIB) format
//
#define kXferMsgIDRegString "BTWAIN_IMAGEXFER message ID registration string"
extern UINT gWM_BTWAIN_IMAGEXFER_ID;

// name of the source manager resource file to load
#ifdef _MSWIN_
#define kTWAIN_DLL_NAME        "TWAIN_32.DLL"
#define kTWAIN_DSM_ENTRY_NAME  "DSM_Entry"
#endif

#define kMAX_MSG_LEN 120                    // max length for error messages


///////////////////////////////////////////////////////////////////////////////
//
//  types used by the TWAIN wrapper class
//
///////////////////////////////////////////////////////////////////////////////
typedef enum                                // status / error codes
{
    BTR_ERROR_CUSTOM_MIN        = -100,             // range delimiter
        BTR_NO_WINDOWS_MESSAGE,                     // our own unique codes
        BTR_TWAIN_DLL_NOT_FOUND,
        BTR_TWAIN_ENTRY_NOT_FOUND,
        BTR_TWAIN_DLL_NOT_LOADED,
        BTR_SOURCE_MANAGER_NOT_OPEN,
        BTR_SOURCE_NOT_OPEN,
    BTR_ERROR_CUSTOM_MAX,                           // range delimiter

    // from here on, error codes map to TWAIN condition codes
    BTR_ERROR_TWAIN_MIN = TWCC_SUCCESS - 1,         // range delimiter
        BTR_OK              = TWCC_SUCCESS, // 0
        BTR_UNKNOWN_ERROR   = TWCC_BUMMER,
        BTR_LOWMEMORY       = TWCC_LOWMEMORY,
        BTR_NO_SOURCE       = TWCC_NODS,
        BTR_MAXCONNECTIONS  = TWCC_MAXCONNECTIONS,
        BTR_OPERATIONERROR  = TWCC_OPERATIONERROR,
        BTR_BADCAP          = TWCC_BADCAP,
        BTR_BADPROTOCOL     = TWCC_BADPROTOCOL,
        BTR_BADVALUE        = TWCC_BADVALUE,
        BTR_SEQERROR        = TWCC_SEQERROR,
        BTR_BADDEST         = TWCC_BADDEST,
        BTR_CAPUNSUPPORTED  = TWCC_CAPUNSUPPORTED,
        BTR_CAPBADOPERATION = TWCC_CAPBADOPERATION,
        BTR_CAPSEQERROR     = TWCC_CAPSEQERROR,
    BTR_ERROR_TWAIN_MAX                             // range delimiter

} BTwainResult;    


typedef enum                        //    TWAIN processing states
{
    BTS_PRE_SESSION = 1,            // 1 - ground state
    BTS_SOURCE_MANAGER_LOADED,      // 2 - Source Manager loaded but not open
    BTS_SOURCE_MANAGER_OPEN,        // 3 - Source Manager open
    BTS_SOURCE_OPEN,                // 4 - Source device open
    BTS_SOURCE_ENABLED,             // 5 - acquisition started
    BTS_TRANSFER_READY,             // 6 - data ready to transfer
    BTS_TRANSFERRING                // 7 - transfer started

} BTwainState;


typedef struct                      // application identification info
{
    int m_nVersionMajor;            // application major version #
    int m_nVersionMinor;            // application minor version #
    char *m_sVersionInfo;           // version info as a string
    char *m_sManufacturer;          // application product info
    char *m_sProductFamily;
    char *m_sProductName;
    // int m_nLanguage;             // later
    // int m_nCountry;

} BTwainAppInfo;


typedef struct              // rectangle to define scan area (in current units)
{
    double top;
    double bottom;
    double left;
    double right;

} BTwainScanRect;


typedef enum                                // pixel types
{
    BPT_RGB24,
    BPT_INDEX8,
    BPT_GRAY8,
    BPT_GRAY4,
    BPT_BW2

} BTwainPixelType;


typedef enum                                // units of measure
{
    BTU_INCHES,
    BTU_CENTIMETERS,
    BTU_PICAS,
    BTU_PIXELS

} BTwainUnits;


typedef struct                              // initialization parameters
{
    double          m_maxResolution;
    double          m_minResolution;
    double          m_desiredResolution;
    BTwainUnits     m_units;
    BTwainPixelType m_pixelType;

} BTwainParams;    


///////////////////////////////////////////////////////////////////////////////
//
//  BTwainWrapper class declaration
//
///////////////////////////////////////////////////////////////////////////////
class BTwainWrapper
{
    public:
        BTwainWrapper();
        ~BTwainWrapper();

        BTwainResult SelectSource();

        BTwainResult Initialize( HWND hWnd, BTwainAppInfo *pInfo,
                                                BTwainParams *pParams = NULL );

        // check method for menu enabling:
        BOOL ReadyToAcquire() { return (m_eState == BTS_SOURCE_MANAGER_OPEN)
                                    && (getCurrentSource() == BTR_OK); }

        BOOL ProcessMessage( LPMSG lpMsg );

        BTwainResult Acquire();
        void *Data();

        BTwainResult Close();

        BTwainState State() { return m_eState; }

        void ShowRemoteUI( BOOL showUI ) { m_bShowUI = showUI; }
        BOOL ShowRemoteUI() { return m_bShowUI; }

        char *ErrorMessage( BTwainResult errorID );


    private:
        HINSTANCE    m_hDSMLib;
        DSMENTRYPROC m_pDSMEntry;

        HWND         m_hParentWnd;

        TW_IDENTITY  m_AppID;
        TW_IDENTITY  m_SourceID;

        TW_UINT32    m_hBitmap;

        BTwainState  m_eState;

        BOOL         m_bShowUI;
        char         m_sMessageString[kMAX_MSG_LEN];

        TW_USERINTERFACE m_twUI;

        BTwainResult loadSourceManager();
        BTwainResult openSourceManager(  BTwainAppInfo *pInfo );
        void registerAppInfo( BTwainAppInfo *pInfo );
        void recordParams( BTwainParams *pParams );

        BTwainResult getCurrentSource();
        BTwainResult openSource();
        BTwainResult beginAcquire();

        void transferNativeImage();

        BTwainResult disableSource();
        BTwainResult closeSource();
        BTwainResult endSourceConnection();
        BTwainResult closeSourceManager();

        BTwainResult getErrorCondition();
};    

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif  // TWAINWRAPPER_H
