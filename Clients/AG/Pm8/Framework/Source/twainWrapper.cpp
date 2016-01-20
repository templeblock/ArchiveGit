///////////////////////////////////////////////////////////////////////////////
//
//	twainWrapper.cpp
//
//	methods for the BTwainWrapper class, which encapsulates TWAIN functionality
//
//	Copyright (c) 1997, Broderbund Software, Inc.
//	All rights reserved.
//
//	Written by John D. Fleischhauer Software Engineering
//
///////////////////////////////////////////////////////////////////////////////

#include "FrameworkIncludes.h"

#include "twainWrapper.h"                       // class deeclarations
#include "twainWrapRC.h"                        // error message string ID

UINT gWM_BTWAIN_IMAGEXFER_ID = 0;           // global pseudo-Windows message ID


///////////////////////////////////////////////////////////////////////////////
//
//  constructor / destructor
//
///////////////////////////////////////////////////////////////////////////////
BTwainWrapper::BTwainWrapper()
{
    m_hDSMLib    = NULL;                            // init members
    m_pDSMEntry  = NULL;
    m_hParentWnd = NULL;
    m_hBitmap    = NULL;

    m_eState  = BTS_PRE_SESSION;                    // initial state
    m_bShowUI = TRUE;                               // use supplied UI's

    gWM_BTWAIN_IMAGEXFER_ID = 0;
}


BTwainWrapper::~BTwainWrapper()
{
    endSourceConnection();                              // clean up
    closeSourceManager();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize()
//
//  Loads and opens Source Manager, gets the ID of the current Source device,
//  and records the passed parameters, if any, for later negotiation with the
//  device
//
//  Capability values are passed via the struct pointer; if NULL is passed the
//  Source's defaults will be used
//
//  NOTE: Currently defaults are used, passed parameters are ignored  -  jdf
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::Initialize( HWND hWnd,
                                BTwainAppInfo *pInfo, BTwainParams *pParams )
{
    if ( m_eState >= BTS_SOURCE_MANAGER_OPEN )      // been here before
    {
        return BTR_OK;
    }

	HCURSOR oldCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
    m_hParentWnd = hWnd;                        // remember the window handle

    BTwainResult retcode = loadSourceManager();     // fire up the TWAIN Src Mgr
    if ( retcode == BTR_OK )
    {
        retcode = openSourceManager( pInfo );
        if ( retcode == BTR_OK )
        {
            recordParams( pParams );            // remember our settings

            // register a unique Windows message for our image transfers
            gWM_BTWAIN_IMAGEXFER_ID =
                            RegisterWindowMessage( kXferMsgIDRegString );
            if ( ! gWM_BTWAIN_IMAGEXFER_ID )                // bad sign
            {
                retcode = BTR_NO_WINDOWS_MESSAGE;
            }
        }
    }

	SetCursor( oldCursor );
    return retcode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  SelectSource()
//
//  Invokes the Source Manager's device selection UI
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::SelectSource()
{
    if ( m_eState < BTS_SOURCE_MANAGER_OPEN )           // who's gonna help?
    {
        return BTR_SOURCE_MANAGER_NOT_OPEN;
    }

    BTwainResult retcode = getCurrentSource();      // make sure we're current
    if ( retcode == BTR_OK )
    {
        TW_UINT16 rc = (*m_pDSMEntry)( &m_AppID,            // call UI
                                       NULL,
                                       DG_CONTROL,
                                       DAT_IDENTITY,
                                       MSG_USERSELECT,
                                       (TW_MEMREF)&m_SourceID );
        if ( rc != TWRC_SUCCESS )
        {
            retcode = getErrorCondition();
        }
    }

    return retcode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Acquire()
//
//  Triggers an acquisition by the Source device
//
//  From this point on, the Source is in control.  It communicates with us via
//  Windows messages.  The messages are intercepted in the applications message
//  loop and passed to the ProcessMessage() method to determine if they apply
//  to us.  See that method for further info.
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::Acquire()
{
    if ( m_eState < BTS_SOURCE_MANAGER_OPEN )       // got to have a manager
    {
        return BTR_SOURCE_MANAGER_NOT_OPEN;
    }

    BTwainResult retcode = getCurrentSource();      // make sure we're current
    if ( retcode == BTR_OK )
    {
        retcode = openSource();                     // open the device
        if ( retcode == BTR_OK )
        {
            retcode = beginAcquire();                   // go to it
        }
    }
    return retcode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  ProcessMessage()
//
//  Examines the passed Windows message.  If it is intended for us, handles it.
//
//  Once an acquire has been triggered, the Source Manager is sending messages
//  to Windows.  It will let us know of a scan error, user abort or completed
//  acquisition.
//
//  If the Source reports a success, we call transferNativeImage() to handle
//  the data transfer.
//
///////////////////////////////////////////////////////////////////////////////
BOOL BTwainWrapper::ProcessMessage( LPMSG lpMsg )
{
    TW_UINT16 rc = TWRC_NOTDSEVENT;
	HCURSOR oldCursor = NULL;

    if ( m_eState >= BTS_SOURCE_ENABLED )
    {
        //  a Source provides a dialog box as its UI; the following call relays
        //  Windows messages down to the Source's UI that were intended for its
        //  dialog box; it also retrieves messages sent from the Source to us
        TW_EVENT  twEvent;
        twEvent.pEvent = (TW_MEMREF)lpMsg;

        rc = (*m_pDSMEntry)( &m_AppID, 
                             &m_SourceID, 
                             DG_CONTROL, 
                             DAT_EVENT,
                             MSG_PROCESSEVENT, 
                             (TW_MEMREF)&twEvent );

        switch ( twEvent.TWMessage )
        {
            case MSG_XFERREADY:             // successful acquisition
				oldCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
                transferNativeImage();      // lets get the data
				SetCursor( oldCursor );
                break;

            case MSG_CLOSEDSREQ:            // an error, or user cancelled
                endSourceConnection();          // just get the heck out
				if ( oldCursor )
				{
					SetCursor( oldCursor );
				}
                break;

            case MSG_NULL:                  // no message from the Source
            default:
                break;
        }   
    } 

    return (rc == TWRC_DSEVENT);     // tell the whether or not we handled it
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close()
//
//  Shuts everything down systematically, returning to state 1.
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::Close()
{
    BTwainResult retcode1 = endSourceConnection();
    BTwainResult retcode2 = closeSourceManager();

    return (retcode2 == BTR_OK) ? retcode1 : retcode2;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Data()
//
//  Returns a void pointer to the acquired image data.
//
//  NOTE: This is an alternative way to get the acquired data.  It is also
//        passed as the wParam argument with the WM_BTWAIN_IMAGEXFER message,
//        which is the way it would be retreived normally.
//
//        Since the application is responsible for deallocating the image data
//        which TWAIN has allocated, beware of when and where you use this
//        method.  Make sure your application has not already acquired and
//        deleted the image data.
//
///////////////////////////////////////////////////////////////////////////////
void *BTwainWrapper::Data()
{
    return (void *)m_hBitmap;
}


///////////////////////////////////////////////////////////////////////////////
//
//  begin private methods:
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//  loadSourceManager
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::loadSourceManager()
{
    if ( m_eState >= BTS_SOURCE_MANAGER_LOADED )    // already done that
    {
        return BTR_OK;
    }

    char dllName[_MAX_FNAME+1];                     // set the DLL path and name

    GetWindowsDirectory( dllName, _MAX_FNAME );
    strcat( dllName, "\\" );
    strcat( dllName, kTWAIN_DLL_NAME );

    OFSTRUCT of;
    if ( OpenFile( dllName, &of, OF_EXIST ) == -1 )     // does it exist?
    {
        return BTR_TWAIN_DLL_NOT_FOUND;
    }

    m_hDSMLib = ::LoadLibrary( dllName );               // now let's load it

    if ( m_hDSMLib < (HINSTANCE)HINSTANCE_ERROR )
    {
        m_hDSMLib = NULL;
        return BTR_TWAIN_DLL_NOT_LOADED;
    }

    m_eState = BTS_SOURCE_MANAGER_LOADED;               // jump to state 2

    return BTR_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
//  openSourceManager()
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::openSourceManager( BTwainAppInfo *pInfo )
{
    if ( m_eState >= BTS_SOURCE_MANAGER_OPEN )          // already done that
    {
        return BTR_OK;
    }

    // find the DLL entry point.
    m_pDSMEntry = (DSMENTRYPROC)GetProcAddress( m_hDSMLib, kTWAIN_DSM_ENTRY_NAME );
    if ( m_pDSMEntry == NULL )
    {
        ::FreeLibrary( m_hDSMLib );
        m_hDSMLib = NULL;
        m_eState = BTS_PRE_SESSION;
        return BTR_TWAIN_ENTRY_NOT_FOUND;
    }
  
    registerAppInfo( pInfo );

    BTwainResult retcode = BTR_OK;
    TW_UINT16 rc = (*m_pDSMEntry)( &m_AppID,        // open the Source Manager
                                   NULL,
                                   DG_CONTROL,
                                   DAT_PARENT,
                                   MSG_OPENDSM,
                                   (TW_MEMREF)&m_hParentWnd);
    if ( rc == TWRC_SUCCESS )
    {
        m_eState = BTS_SOURCE_MANAGER_OPEN;             // jump to state 3
    }
    else
    {
        retcode = getErrorCondition();
        ::FreeLibrary( m_hDSMLib );
        m_hDSMLib = NULL;
        m_pDSMEntry = NULL;
        m_eState = BTS_PRE_SESSION;
    }

	return retcode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  getCurrentSource
//
//  Polls the Source Manager for the ID of the current ("default") source
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::getCurrentSource()
{
    if ( m_eState < BTS_SOURCE_MANAGER_OPEN )           // don't go there
    {
        return BTR_SOURCE_MANAGER_NOT_OPEN;
    }

    BTwainResult retcode = BTR_OK;
    TW_UINT16 rc = (*m_pDSMEntry)( &m_AppID,            // ok, what is it?
                                   NULL,
                                   DG_CONTROL,
                                   DAT_IDENTITY,
                                   MSG_GETDEFAULT,
                                   (TW_MEMREF)&m_SourceID );
    if ( rc != TWRC_SUCCESS )
    {
        retcode = getErrorCondition();
    }

	return retcode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  recordParams()
//
//  If capability parameters were passed to Initialize(), we store them here
//  for future negotiation
//
//  NOTE: Currently not implemented  -  3/26/97 jdf
//
///////////////////////////////////////////////////////////////////////////////
void BTwainWrapper::recordParams( BTwainParams *pParams )
{
    if ( ! pParams )
    {
        return;
    }
    return;
}


///////////////////////////////////////////////////////////////////////////////
//
//  registerAppInfo()
//
//  Put the user defined application info into a TWAIN identity struct.
//
///////////////////////////////////////////////////////////////////////////////
void BTwainWrapper::registerAppInfo( BTwainAppInfo *pInfo )
{
    m_AppID.Id = 0;	                        // source Manager will assign value

    m_AppID.Version.Language = TWLG_USA;    // for now, we define these
    m_AppID.Version.Country  = TWCY_USA;
    m_AppID.ProtocolMajor    = TWON_PROTOCOLMAJOR;
    m_AppID.ProtocolMinor    = TWON_PROTOCOLMINOR;
    m_AppID.SupportedGroups  = DG_IMAGE | DG_CONTROL;

    // set some defaults, in case the user left some out
    m_AppID.Version.MajorNum = 0;
    m_AppID.Version.MinorNum = 0;
    lstrcpy ( m_AppID.Version.Info,  "No version" );
    lstrcpy ( m_AppID.Manufacturer,  "" );
    lstrcpy ( m_AppID.ProductFamily, "" );
    lstrcpy ( m_AppID.ProductName,   "Unnamed Application" );

    if ( ! pInfo )                          // user didn't pass any
    {
        return;
    }

    // now copy over the user defined values
    m_AppID.Version.MajorNum = pInfo->m_nVersionMajor;
    m_AppID.Version.MinorNum = pInfo->m_nVersionMinor;
    if ( pInfo->m_sVersionInfo )
    {
        lstrcpyn( m_AppID.Version.Info, pInfo->m_sVersionInfo, sizeof(TW_STR32) );
    }
    if ( pInfo->m_sManufacturer )
    {
        lstrcpyn( m_AppID.Manufacturer, pInfo->m_sManufacturer, sizeof(TW_STR32) );
    }
    if ( pInfo->m_sProductFamily )
    {
        lstrcpyn( m_AppID.ProductFamily,pInfo->m_sProductFamily, sizeof(TW_STR32) );
    }
    if ( pInfo->m_sProductName )
    {
        lstrcpyn( m_AppID.ProductName,  pInfo->m_sProductName, sizeof(TW_STR32) );
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//  openSource()
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::openSource()
{
    if ( m_eState < BTS_SOURCE_MANAGER_OPEN )       // but we already did that
    {
        return BTR_SOURCE_MANAGER_NOT_OPEN;
    }
    
    BTwainResult retcode = BTR_OK;
    TW_UINT16 rc = (*m_pDSMEntry)( &m_AppID,        // open the current Source
                                   NULL,
                                   DG_CONTROL,
                                   DAT_IDENTITY,
                                   MSG_OPENDS,
                                   (TW_MEMREF)&m_SourceID );
    if ( rc == TWRC_SUCCESS )
    {
        m_eState = BTS_SOURCE_OPEN;                 // jump to state 4
    }
    else
    {
        retcode = getErrorCondition();
    }

	return retcode;
#if 0
    //
    // NOTE: According to the TWAIN 1.6 spec, every Source is required to
    //       respond to an application's attempt to determine the maximum number
    //       of transfers it is willing to handle, and every application is
    //       required to support the negotiation of this capability.  However,
    //       most of the TWAIN application implementations I have seen do not
    //       do this, and my test of the Kodak DC120 camera disclosed that
    //       they don't either (it returned an error).  Therefore, I'm leaving
    //       this out for now.  -  3/26/97 jdf
    //
    TW_CAPABILITY twCap;
    twCap.Cap        = CAP_XFERCOUNT;
    twCap.ConType    = TWON_ONEVALUE;
    twCap.hContainer = GlobalAlloc( GHND, sizeof(TW_ONEVALUE) );

    pTW_ONEVALUE pVal = (pTW_ONEVALUE)GlobalLock( twCap.hContainer );
    pVal->ItemType = TWTY_INT16;
    pVal->Item = 1;
    GlobalUnlock( twCap.hContainer );

    rc = (*m_pDSMEntry)( &m_AppID,
                         &m_SourceID,
                         DG_CONTROL,
                         DAT_CAPABILITY,
                         MSG_SET,
                         (TW_MEMREF)&twCap );

    GlobalFree( (HANDLE)twCap.hContainer );

    if ( rc == TWRC_CHECKSTATUS )                       // may not need this
    {
        // need more something here - see spec p. 3-xx
        twCap.Cap        = CAP_XFERCOUNT;
        twCap.ConType    = TWON_DONTCARE16;
        twCap.hContainer = NULL;

        rc = (*m_pDSMEntry)( &m_AppID,
                             &m_SourceID,
                             DG_CONTROL,
                             DAT_CAPABILITY,
                             MSG_GET,
                             (TW_MEMREF)&twCap );

        pVal = (pTW_ONEVALUE)GlobalLock( twCap.hContainer );
        int count = pVal->Item;
        GlobalFree( (HANDLE)twCap.hContainer );
    }
    else if ( rc == TWRC_FAILURE )
    {
        retcode = getErrorCondition();
    }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  beginAcquire()
//
//  Enable the TWAIN source and let 'er rip
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::beginAcquire()
{
    if ( m_eState < BTS_SOURCE_MANAGER_OPEN )       // oh yeah, well...
    {
        return BTR_SOURCE_MANAGER_NOT_OPEN;
    }

    if ( m_eState < BTS_SOURCE_OPEN )               // and of course...
    {
        return BTR_SOURCE_NOT_OPEN;
    }

    BTwainResult retcode = BTR_OK;

    m_twUI.ShowUI  = m_bShowUI;                     // info for Source UI dialog
    m_twUI.hParent = m_hParentWnd;

    TW_UINT16 rc = (*m_pDSMEntry)( &m_AppID,        // here we go
                                   &m_SourceID,
                                   DG_CONTROL,
                                   DAT_USERINTERFACE,
                                   MSG_ENABLEDS,
                                   (TW_MEMREF)&m_twUI );
    if ( rc == TWRC_SUCCESS )
    {
        m_eState = BTS_SOURCE_ENABLED;      // jump to state 5
    }                                       // you're on your own now, Source
    else
    {
        retcode = getErrorCondition();
    }

	return retcode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  transferNativeImage()
//
///////////////////////////////////////////////////////////////////////////////
void BTwainWrapper::transferNativeImage()
{
    TW_UINT16 rc, rc2;
    TW_PENDINGXFERS twPendingXfer;

    twPendingXfer.Count = 0;
    do                              // until there are no more pending transfers
    {
    	TW_IMAGEINFO twImageInfo;        // request image info (triggers xfer)
        rc = (*m_pDSMEntry)( &m_AppID, 
                             &m_SourceID, 
                             DG_IMAGE,
                             DAT_IMAGEINFO,
                             MSG_GET,
                             (TW_MEMREF)&twImageInfo);
        if ( rc != TWRC_SUCCESS )
        {
            rc2 = (*m_pDSMEntry)( &m_AppID,                 // abort the image
                                  &m_SourceID, 
                                  DG_CONTROL,
                                  DAT_PENDINGXFERS, 
                                  MSG_ENDXFER,
                                  (TW_MEMREF)&twPendingXfer); 
            if ( rc2 == TWRC_SUCCESS )
            {
                m_eState = BTS_SOURCE_ENABLED;          // drop to state 5

                if ( twPendingXfer.Count == 0 )         // nothing else coming
                {
                    endSourceConnection();              // we're done
                }

                SendMessage( m_hParentWnd, gWM_BTWAIN_IMAGEXFER_ID, NULL, 0 );
            }
            if ( twPendingXfer.Count == 0 )
            {
                break;
            }

            continue;
        }
                                                    // Source says it's ready
        m_eState = BTS_TRANSFER_READY;              // jump to state 6
        m_hBitmap = NULL;
        rc = (*m_pDSMEntry)( &m_AppID,              // let's see what we can get
                             &m_SourceID, 
                             DG_IMAGE,
                             DAT_IMAGENATIVEXFER, 
                             MSG_GET, 
                             (TW_MEMREF)&m_hBitmap );
        switch ( rc )
        {
            case TWRC_XFERDONE:                         //successful acquisition
            {
                rc2 = (*m_pDSMEntry)( &m_AppID,         // clean up
                                      &m_SourceID, 
                                      DG_CONTROL,
                                      DAT_PENDINGXFERS, 
                                      MSG_ENDXFER,
                                      (TW_MEMREF)&twPendingXfer );
                if ( rc2 == TWRC_SUCCESS )
                {
                    // we got data; send it to the app via our special message
                    m_eState = BTS_TRANSFERRING;        // jump to state 7
                    SendMessage( m_hParentWnd, gWM_BTWAIN_IMAGEXFER_ID,
                                                        (WPARAM)m_hBitmap, 0 );
                }
                break;
            }

            case TWRC_CANCEL:                       // Source (or user) aborted
            case TWRC_FAILURE:                      // other failure
            default:
            {
                rc2 = (*m_pDSMEntry)( &m_AppID,             // abort the image
                                      &m_SourceID, 
                                      DG_CONTROL,
                                      DAT_PENDINGXFERS, 
                                      MSG_ENDXFER,
                                      (TW_MEMREF)&twPendingXfer); 
                if ( rc2 == TWRC_SUCCESS )
                {
                    m_eState = BTS_SOURCE_ENABLED;      // drop to state 5

                    if ( twPendingXfer.Count == 0 )     // nothing else coming
                    {
                        endSourceConnection();          // we're done
                    }

                    SendMessage( m_hParentWnd, gWM_BTWAIN_IMAGEXFER_ID, NULL, 0 );
                }
                break;
            }
        }   
    
    } while ( twPendingXfer.Count != 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  disableSource()
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::disableSource()
{
    BTwainResult retcode = BTR_OK;
    if ( m_eState >= BTS_SOURCE_ENABLED )
    {
        m_twUI.hParent = m_hParentWnd;
        m_twUI.ShowUI = TWON_DONTCARE8;

        TW_UINT16 rc = (*m_pDSMEntry)( &m_AppID,        // disable it
                                       &m_SourceID,
                                       DG_CONTROL,
                                       DAT_USERINTERFACE,
                                       MSG_DISABLEDS,
                                       (TW_MEMREF)&m_twUI );
        if ( rc == TWRC_SUCCESS )
        {   
            m_eState = BTS_SOURCE_OPEN;                 // drop to state 4
		}
		else
		{
            retcode = getErrorCondition();
		}
    }    	

	return retcode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  closeSource()
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::closeSource()
{
    BTwainResult retcode = BTR_OK;
    if ( m_eState > BTS_SOURCE_OPEN )                       // out of sequence
    {
        return BTR_SEQERROR;
    }    	
    else
    {
        if ( m_eState == BTS_SOURCE_OPEN )
	    {
            TW_UINT16 rc = (*m_pDSMEntry)( &m_AppID,        // close it
                                           NULL,
                                           DG_CONTROL,
                                           DAT_IDENTITY,
                                           MSG_CLOSEDS,
                                           (TW_MEMREF)&m_SourceID );
            if ( rc == TWRC_SUCCESS )
            {   
                m_eState = BTS_SOURCE_MANAGER_OPEN;         // drop to state 3
                m_SourceID.Id = 0;
			    m_SourceID.ProductName[0] = 0;
    		}
    		else
    		{
                retcode = getErrorCondition();
    		}
		}
	}

	return retcode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  endSourceConnection()
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::endSourceConnection()
{
    BTwainResult retcode1 = disableSource();
    BTwainResult retcode2 = closeSource();

    return (retcode2 == BTR_OK) ? retcode1 : retcode2;
}



///////////////////////////////////////////////////////////////////////////////
//
//  closeSourceManager()
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::closeSourceManager()
{
    if ( m_eState > BTS_SOURCE_MANAGER_OPEN )
    {
        return BTR_SEQERROR;
    }

    BTwainResult retcode = BTR_OK;
    if ( m_eState == BTS_SOURCE_MANAGER_OPEN )
	{
        TW_UINT16 rc = (*m_pDSMEntry)( &m_AppID,            // close it
                                       NULL,
                                       DG_CONTROL,
                                       DAT_PARENT,
                                       MSG_CLOSEDSM,
                                       (TW_MEMREF) &m_hParentWnd);
        if ( rc != TWRC_SUCCESS )
        {
            retcode = getErrorCondition();
        }
        else
        {
            m_eState = BTS_SOURCE_MANAGER_LOADED;           // drop to state 2
            if ( m_hDSMLib )
            {
                ::FreeLibrary( m_hDSMLib );
                m_hDSMLib = NULL;        
                m_eState = BTS_PRE_SESSION;                 // drop to state 1
            }

            m_SourceID.Id = 0;                              // just to be sure
            m_SourceID.ProductName[0] = 0;
        }
    }

	return retcode;
}



///////////////////////////////////////////////////////////////////////////////
//
//  getErrorCondition()
//
//  If a TWAIN operation fails, call this to return a specific error code.
//
///////////////////////////////////////////////////////////////////////////////
BTwainResult BTwainWrapper::getErrorCondition()
{
    BTwainResult retcode = BTR_UNKNOWN_ERROR;       // at the very least

    TW_STATUS status;
    TW_UINT16 rc = (*m_pDSMEntry)( &m_AppID,        // TWAIN status call
                                   NULL,
                                   DG_CONTROL,
                                   DAT_STATUS,
                                   MSG_GET,
                                   (TW_MEMREF)&status );
    if ( rc == TWRC_SUCCESS )
    {
        retcode = (BTwainResult)(status.ConditionCode);

        if ( ( retcode <= BTR_ERROR_CUSTOM_MIN ) ||     // if not recognized
             ( retcode >= BTR_ERROR_CUSTOM_MAX &&
               retcode <= BTR_ERROR_TWAIN_MIN )  ||
             ( retcode <= BTR_ERROR_TWAIN_MAX )  )
        {
            retcode = BTR_UNKNOWN_ERROR;
        }    
    }

    return retcode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  ErrorMessage()
//
//  Given an error (BTwainResult) code, returns a string which may be displayed
//  (or not) at the application's discretion.
//
//  The strings are in a resource string table in twainWrapper.rc; that file
//  must be included in the application resource file for this to work.
//
///////////////////////////////////////////////////////////////////////////////
char *BTwainWrapper::ErrorMessage( BTwainResult errorID )
{
    WORD stringID;                  // get the resource ID for the error string
    switch ( errorID )
    {
        case BTR_NO_WINDOWS_MESSAGE:
            stringID = IDS_BTM_NO_WINDOWS_MESSAGE;
            break;

        case BTR_TWAIN_DLL_NOT_FOUND:
            stringID = IDS_BTM_TWAIN_DLL_NOT_FOUND;
            break;

        case BTR_TWAIN_ENTRY_NOT_FOUND:
            stringID = IDS_BTM_TWAIN_ENTRY_NOT_FOUND;
            break;

        case BTR_TWAIN_DLL_NOT_LOADED:
            stringID = IDS_BTM_TWAIN_DLL_NOT_LOADED;
            break;

        case BTR_SOURCE_MANAGER_NOT_OPEN:
            stringID = IDS_BTM_SOURCE_MANAGER_NOT_OPEN;
            break;

        case BTR_SOURCE_NOT_OPEN:
            stringID = IDS_BTM_SOURCE_NOT_OPEN;
            break;

        case BTR_OK:
            stringID = IDS_BTM_OK;
            break;

        case BTR_LOWMEMORY:
            stringID = IDS_BTM_LOWMEMORY;
            break;

        case BTR_NO_SOURCE:
            stringID = IDS_BTM_NO_SOURCE;
            break;

        case BTR_MAXCONNECTIONS:
            stringID = IDS_BTM_MAXCONNECTIONS;
            break;

        case BTR_OPERATIONERROR:
            stringID = IDS_BTM_OPERATIONERROR;
            break;

        case BTR_BADCAP:
            stringID = IDS_BTM_BADCAP;
            break;

        case BTR_BADPROTOCOL:
            stringID = IDS_BTM_BADPROTOCOL;
            break;

        case BTR_BADVALUE:
            stringID = IDS_BTM_BADVALUE;
            break;

        case BTR_SEQERROR:
            stringID = IDS_BTM_SEQERROR;
            break;

        case BTR_BADDEST:
            stringID = IDS_BTM_BADDEST;
            break;

        case BTR_CAPUNSUPPORTED:
            stringID = IDS_BTM_CAPUNSUPPORTED;
            break;

        case BTR_CAPBADOPERATION:
            stringID = IDS_BTM_CAPBADOPERATION;
            break;

        case BTR_CAPSEQERROR:
            stringID = IDS_BTM_CAPSEQERROR;
            break;

        case BTR_UNKNOWN_ERROR:
        default:
            stringID = IDS_BTM_UNKNOWN_ERROR;
            break;
    }

    // retrieve the error string from the resource
    HINSTANCE hInst = (HINSTANCE)GetWindowLong( m_hParentWnd, GWL_HINSTANCE );
    if ( hInst )
    {
        if ( LoadString( hInst, IDS_BTM_PREFIX, m_sMessageString, kMAX_MSG_LEN ) )
        {
            char msgBuf[kMAX_MSG_LEN-20];
            if ( LoadString( hInst, stringID, msgBuf, kMAX_MSG_LEN - 20 ) )
            {
                strcat( m_sMessageString, msgBuf );
            }
        }
    }

    return m_sMessageString;
}
