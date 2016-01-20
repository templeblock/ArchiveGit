/****************************************************************************
*  FILE:        CTWAIN.CPP                                                  *
*  SUMMARY:     Twain Class Wrapper.                                        *
*  AUTHOR:      Dennis Plotzke                                              *
*  -------------------------------------------------------------------------*
*  Initials Legend:                                                         *
*     DGP       Dennis Plotzke                                              *
*  -------------------------------------------------------------------------*
*  Revision History                                                         *
*                                                                           *
*  Date     Initials  Description of Change                                 *
*  -------------------------------------------------------------------------*
*  10/07/96 DGP       Added DisplayError().                                 *
*  09/25/95 DGP       Added SetError call when image acquire is canceled.   *
*                     Removed WriteImage().                                 *
*  08/22/96 DGP       Started History.                                      *
*                                                                           *
****************************************************************************/
#include "stdafx.h"
#include "resource.h"
#include "CTwain.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define       WINDIRPATHSIZE 160

static TW_USERINTERFACE  twUI;    // Fix as per Courisimault: 940518 - bd

/////////////////////////////////////////////////////////////////////////////
// CTwain

CTwain::CTwain()
  {
    char          szFullFilename[WINDIRPATHSIZE], *pszDSMName;

    GetWindowsDirectory (szFullFilename, WINDIRPATHSIZE);
    #ifdef WIN32
      pszDSMName = "twain_32.dll";
    #else
      pszDSMName = "twain.dll";
    #endif
                
    if (szFullFilename[strlen(szFullFilename)-1] != '\\')
      lstrcat (szFullFilename, "\\");
    lstrcat (szFullFilename, pszDSMName);
    m_strSMFilename = szFullFilename;     // Assign to CString Member
    Init();
  }

void CTwain::Init()
{
  m_AppIdentity.Id = 0;                                 // init to 0, but Source Manager will assign real value
  m_AppIdentity.Version.MajorNum = 1;
  m_AppIdentity.Version.MinorNum = 5;
  m_AppIdentity.Version.Language = TWLG_USA;
  m_AppIdentity.Version.Country  = TWCY_USA;

  // Strings should be set by client
  lstrcpy (m_AppIdentity.Version.Info,  "Twain Class Wrapper");
  lstrcpy (m_AppIdentity.ProductName,   "PrintMaster");

  m_AppIdentity.ProtocolMajor =    TWON_PROTOCOLMAJOR;
  m_AppIdentity.ProtocolMinor =    TWON_PROTOCOLMINOR;
  m_AppIdentity.SupportedGroups =  DG_IMAGE | DG_CONTROL;

  // Strings should be set by client
  lstrcpy (m_AppIdentity.Manufacturer,  "Mindscape");
  lstrcpy (m_AppIdentity.ProductFamily, "Multimedia");

  m_lpDSM_Entry = NULL;
  m_hDll = NULL;
  m_hBitmap = NULL;
  m_bManagerOpen = m_bSourceOpen = m_bSourceEnabled = FALSE;
  m_bShow = TRUE;
  m_bAcquiredImage = TRUE;
  m_hWndClient = NULL;
  memset(&m_SourceID, 0, sizeof(m_SourceID));
}

void CTwain::DeInit()
  {
    if(m_hBitmap)
      {
        ::GlobalFree(m_hBitmap);
        m_hBitmap = NULL;
      }
    CloseManager();
  }

CTwain::~CTwain()
{
  DeInit();
}

BOOL CTwain::IsManagerInstalled()
  {
    OFSTRUCT      OpenFiles;

    // Check for file existance
    return (OpenFile(m_strSMFilename.GetBuffer(1), &OpenFiles, OF_EXIST) != HFILE_ERROR);
  }

BOOL CTwain::SetHandler(HWND hWndToReceive, UINT nClientStartMessage)
{
	if(hWndToReceive == NULL || nClientStartMessage < WM_USER)
		return FALSE;
	m_hWndClient = hWndToReceive;
	m_nClientStartMessage = nClientStartMessage;
	return TRUE;
}

// Checks if any source device is available (does not open source)
BOOL CTwain::IsSourceAvailable(HWND hWnd)
{
	BOOL fAvailable = TRUE;

	// Some source is already open. If, so, then a source must be available.
	if (!m_bSourceOpen)
	{
		fAvailable = FALSE;

		if (OpenManager(hWnd))
		{
			TW_UINT16 twRC = TWRC_FAILURE;
			m_ErrorInfo.Clear();

			// Can assume m_bSourceOpen == FALSE now

			// This will check for a Source.
			TW_IDENTITY twIdentity;
			twRC = SourceManager(NULL,
									  DG_CONTROL,
									  DAT_IDENTITY,
									  MSG_GETFIRST,
									  &twIdentity);
                  
			switch (twRC)
			{
			  case TWRC_SUCCESS:
				  fAvailable = TRUE;
				 break;
			  case TWRC_FAILURE:
				 // Trouble opening the Source
				 // Determine Condition Code
				 m_ErrorInfo.SetError(EE_LOCATION, CTRC_SMGETFIRST_FAIL, &m_twStatus);
				 fAvailable = FALSE;
				 break;
        
			  default:
				 // Undefined error
				 m_ErrorInfo.SetError(EE_LOCATION, CTRC_FAIL, &m_twStatus);
				 fAvailable = FALSE;
				 break;
			}

			CloseManager();
		}
	}

	return fAvailable;
}

BOOL CTwain::IsSourceInstalled(HWND hWnd)
  {
    if(!OpenManager(hWnd))
      {
        return FALSE;
      }
    if(!OpenSource())
      {
        CloseManager();
        return FALSE;
      }
    CloseSource();
    return TRUE;
  }

BOOL CTwain::OpenManager(HWND hWnd)
  {
    TW_UINT16     twRC;

    m_ErrorInfo.Clear();

    m_hWnd = hWnd;
    m_bAcquiring = FALSE;

//    Hook(hWnd);   // Take over this windows messages

    if(m_bManagerOpen)
      return TRUE;    // already open

    m_bManagerOpen = FALSE;   // Default to closed

        
    // Check for file existance
    if ( IsManagerInstalled() &&
        (m_hDll =  LoadLibrary(m_strSMFilename.GetBuffer(1))) != NULL &&
//        (m_hDll >= (HANDLE)VALID_HANDLE) &&
        (m_lpDSM_Entry = (DSMENTRYPROC)GetProcAddress(m_hDll, MAKEINTRESOURCE (1))) != NULL)
        {
          // This call performs four important functions:
          //      - opens/loads the SM
          //    - passes the handle to the app's window to the SM
          //    - returns the SM assigned appID.id field
          //    - be sure to test the return code for SUCCESSful open of SM
                       
          twRC = SourceManager(NULL,
                              DG_CONTROL,
                              DAT_PARENT,
                              MSG_OPENDSM,
                              (TW_MEMREF)&m_hWnd);
        
          switch (twRC)
            {
              case TWRC_SUCCESS:
                m_bManagerOpen = TRUE;
                break;

              case TWRC_FAILURE:
                // Init. error
                m_ErrorInfo.SetError(EE_LOCATION, CTRC_SMOPEN_FAIL, &m_twStatus);
                break;
              default:
                m_ErrorInfo.SetError(EE_LOCATION, CTRC_FAIL, &m_twStatus); // ?
                break;
            }
        }
      else
        {
          // Failed to find .DLL
          m_ErrorInfo.SetError(EE_LOCATION, CTRC_FINDLIB_FAIL);
        }

    // Let the caller know what happened
    return(m_bManagerOpen);
  }

BOOL CTwain::CloseManager()
  {
    TW_UINT16   twRC;

    m_ErrorInfo.Clear();
    if (!m_bManagerOpen)
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_SEQUENCE_ERR);
        return FALSE;       // Manager not open
      }
    if(!m_hDll)
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_SMLIB_NOTLOADED);
        return FALSE;       // Manager DLL not loaded
      }

    if (m_bSourceOpen)
      CloseSource();

    // This call performs one important function:
    // - tells the SM which application, m_AppIdentity.id, is requesting SM to close
    // - be sure to test return code, failure indicates SM did not close !!
            
    twRC = SourceManager( NULL,
                          DG_CONTROL,
                          DAT_PARENT,
                          MSG_CLOSEDSM,
                          (TW_MEMREF)&m_hWnd);
                    
    if (twRC != TWRC_SUCCESS)
      // Trouble closing the SM, inform the user
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_SMCLOSE_FAIL, &m_twStatus);
        return FALSE;
      }

    m_bManagerOpen = FALSE;

    // Explicitly free the SM library
    FreeLibrary (m_hDll);
    m_hDll = NULL;
    m_lpDSM_Entry = NULL;

    // the data source id will no longer be valid after
    // twain is killed.  If the id is left around the
    // data source can not be found or opened
    m_AppIdentity.Id = 0;  

    // Let the caller know what happened
    return (TRUE);
  }

BOOL CTwain::OpenSource()
  {
    TW_UINT16 twRC=TWRC_FAILURE;
    
    m_ErrorInfo.Clear();
    if(!m_bManagerOpen)
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_SEQUENCE_ERR);
        return FALSE;     // Manager must be open to communicate with Twain
      }

    if(m_bSourceOpen)
      return TRUE;      // Source already open

    // Can assume m_bSourceOpen == FALSE now

    // This will open the Source.
    twRC = SourceManager( NULL,
                          DG_CONTROL,
                          DAT_IDENTITY,
                          MSG_OPENDS,
                          &m_SourceID);
                        
    switch (twRC)
      {
        case TWRC_SUCCESS:
          // do not change flag unless we successfully open
          m_bSourceOpen = TRUE;
          break;
        case TWRC_FAILURE:
          // Trouble opening the Source
          // Determine Condition Code
          m_ErrorInfo.SetError(EE_LOCATION, CTRC_DSOPEN_FAIL, &m_twStatus);
          break;
                    
        default:
          // Undefined error
          m_ErrorInfo.SetError(EE_LOCATION, CTRC_FAIL, &m_twStatus);
          break;
      }
    return m_bSourceOpen;
  }

BOOL CTwain::CloseSource()
  {
    TW_UINT16         twRC=TWRC_FAILURE;

    m_ErrorInfo.Clear();
    if(!m_bSourceOpen)
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_SEQUENCE_ERR);
        return FALSE;   // No Source open for closing
      }

    // Can assume m_bSourceOpen == TRUE now

    // Automatically disable source if enabled
    if(m_bSourceEnabled)
      DisableSource();    // Cannot close enabled source so disable it

    // Close an open Source
    twRC = SourceManager(   NULL,
                            DG_CONTROL,
                            DAT_IDENTITY,
                            MSG_CLOSEDS,
                            &m_SourceID);


    // report error on close
    if (twRC != TWRC_SUCCESS)
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_DSCLOSE_FAIL, &m_twStatus);
        return FALSE;
      }

    m_bSourceOpen = FALSE;
    m_SourceID.Id = 0;
//  m_AppIdentity.ProductName[0] = 0;

    return TRUE;
  }

BOOL CTwain::EnableSource()
  {
    BOOL              Result=FALSE;
    TW_UINT16         twRC=TWRC_FAILURE;

    m_ErrorInfo.Clear();
    if(!m_bSourceOpen)
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_SEQUENCE_ERR);
        return FALSE;     // Source must be open
      }

    // only enable open Source's
    if(m_bSourceEnabled)
      return TRUE;       // Already enabled
        
    // This will display the Source User Interface. The Source should only display
    // a user interface that is compatible with the group defined
    // by appID.SupportedGroups (in our case DG_IMAGE | DG_CONTROL)
    twUI.hParent = m_hWnd;
    twUI.ShowUI  = m_bShow;
    twRC = SourceManager( &m_SourceID,
                          DG_CONTROL,
                          DAT_USERINTERFACE,
                          MSG_ENABLEDS,
                          (TW_MEMREF)&twUI);

    if (twRC!=TWRC_SUCCESS)
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_DSENABLE_FAIL, &m_twStatus);
        return FALSE;         // Error
      }

    m_bSourceEnabled = TRUE;
    return TRUE;
  }

BOOL CTwain::DisableSource()
  {
    TW_UINT16         twRC=TWRC_FAILURE;
    TW_USERINTERFACE  twUI;
    
    m_ErrorInfo.Clear();

    // only disable enabled Source's
    if(!m_bSourceEnabled)
      return TRUE;       // Nothing to disable

    twUI.hParent = m_hWnd;
    twUI.ShowUI = TWON_DONTCARE8;
    twRC = SourceManager( &m_SourceID,
                          DG_CONTROL,
                          DAT_USERINTERFACE,
                          MSG_DISABLEDS,
                          (TW_MEMREF)&twUI);

    m_bSourceEnabled = FALSE;
    if (twRC != TWRC_SUCCESS)
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_DSDISABLE_FAIL, &m_twStatus);
        return FALSE;
      }

    return TRUE;
  }

BOOL CTwain::SelectSource()
  {
    TW_UINT16     twRC;
    TW_IDENTITY   NewDSIdentity;
    
    m_ErrorInfo.Clear();
    if(m_bSourceOpen)
      CloseSource();    // Close currently open source
    
    // I will settle for the system default.  Shouldn't I get a highlight
    // on system default without this call?
    twRC = SourceManager(       NULL,
                                DG_CONTROL,
                                DAT_IDENTITY,
                                MSG_GETDEFAULT,
                                (TW_MEMREF)&NewDSIdentity);
                
    // This call performs one important function:
    // - should cause SM to put up dialog box of available Source's
    // - tells the SM which application, appID.id, is requesting, REQUIRED
    // - returns the SM assigned NewDSIdentity.id field, you check if changed
    //  (needed to talk to a particular Data Source)
    // - be sure to test return code, failure indicates SM did not close !!
    //
    twRC = SourceManager(   NULL,
                            DG_CONTROL,
                            DAT_IDENTITY,
                            MSG_USERSELECT,
                            (TW_MEMREF)&NewDSIdentity);
        
    // Check if the user changed the Source and react as apporpriate.
    // - TWRC_SUCCESS, log in new Source
    // - TWRC_CANCEL,  keep the current Source
    // - default,      check down the codes in a status message, display result
    //
        
    switch (twRC)
      {
        case TWRC_SUCCESS:
          m_SourceID = NewDSIdentity;
          break;

        case TWRC_CANCEL:
        case TWRC_FAILURE:              
        default:
          m_ErrorInfo.SetError(EE_LOCATION, CTRC_DSSELECT_FAIL, &m_twStatus);
          return FALSE;
          break;
      }
    return TRUE;
  }

BOOL CTwain::Acquire(HWND hWnd)
  {
    m_ErrorInfo.Clear();
    DeInit();

	  ASSERT(m_hBitmap == NULL);

    if(!OpenManager(hWnd))
      return FALSE;

    // Please note that another Source may change the system default while
    // your not looking and simply getting the default will not guarentee
    // you get what you want.  Suggest saving the dsID structure privately
    // after you open it to assure subsequent connections are to your
    // intended Source -- ed
    // Also note the the DSM will maintain a static list of all Sources in
    // the system at the moment it is opened.  Changes in available Sources
    // while the DSM is open may cause unpredictable results. -- ed
        
    if(!OpenSource())
      return FALSE;

    if(!SelectTransferType())
      return FALSE;

	 return EnableSource();
  }

BOOL CTwain::SelectTransferType(short nType)
{
    TW_CAPABILITY   cap;
    pTW_ONEVALUE    pval;
    TW_INT16        status;   
//    OPENFILENAME    ofn;
    char *Filter[] = { "Bitmap(*.BMP)\0", "*.bmp\0\0"};

    m_ErrorInfo.Clear();
    cap.Cap = ICAP_XFERMECH;
    cap.ConType = TWON_ONEVALUE;

    // alloc the container
    if (!(cap.hContainer = GlobalAlloc(GHND, sizeof(TW_ONEVALUE))))
      return FALSE;   // Allocation failed!

    pval = (pTW_ONEVALUE)GlobalLock(cap.hContainer);
    pval->ItemType = TWTY_UINT16;

    // memory transfer
    pval->Item = nType;

    switch(nType)
      {
        case TWSX_MEMORY:
          break;
        case TWSX_FILE:
          // file transfer currently fixed to bitmap format
          // get the filename to save as
          // check formats supported by the source            
#if 0
          ofn.lStructSize = sizeof(OPENFILENAME);
          ofn.hwndOwner = m_hWnd;
          ofn.lpstrFilter = Filter[0];
          ofn.lpstrCustomFilter = NULL;
          ofn.nMaxCustFilter = 0L;
          ofn.nFilterIndex = 0L;
          ofn.lpstrFile = Filename;
          ofn.nMaxFile = sizeof(Filename);
          ofn.lpstrFileTitle = NULL;
          ofn.nMaxFileTitle = 0L;
          ofn.lpstrInitialDir = NULL;
          ofn.lpstrTitle = NULL;
          ofn.Flags = 0;
          ofn.nFileOffset = 0;
          ofn.nFileExtension = 0;
          ofn.lpstrDefExt = "BMP";
          if (GetSaveFileName(&ofn)==FALSE)
              return(TWRC_FAILURE);
#endif
          break;
      }
    GlobalUnlock(cap.hContainer);

    status = SourceManager (&m_SourceID,
                            DG_CONTROL, 
                            DAT_CAPABILITY, 
                            MSG_SET,
                            (TW_MEMREF)&cap);

    GlobalFree((HANDLE)cap.hContainer);

    if (status != TWRC_SUCCESS)
      {
        m_ErrorInfo.SetError(EE_LOCATION, CTRC_DSXFERTYPE_FAIL, &m_twStatus);
        return FALSE;
      }


    return TRUE;
}

BOOL CTwain::ProcessMessage(MSG *pMsg)
  {
    TW_UINT16  twRC=TWRC_NOTDSEVENT;
    TW_EVENT   twEvent;

    if(!m_bManagerOpen || !m_bSourceOpen)
      return FALSE;
    
    twEvent.pEvent = (TW_MEMREF) pMsg;
    twRC = SourceManager( &m_SourceID, 
                          DG_CONTROL, 
                          DAT_EVENT,
                          MSG_PROCESSEVENT, 
                          (TW_MEMREF)&twEvent);

    switch (twEvent.TWMessage)
      {
        case MSG_XFERREADY:
          m_ErrorInfo.Clear();
          TransferImage();
          break;

        case MSG_CLOSEDSREQ:
          //Disable, CloseDS, CloseDSM    
          DisableSource();
          CloseSource();
          CloseManager();
          m_ErrorInfo.SetError(EE_LOCATION, CTRC_XFER_CANCELED, &m_twStatus);
          break;

        // No message from the Source to the App break;
        // possible new message
        case MSG_NULL:
        default:
            break;
      }   
    return twRC == TWRC_DSEVENT;
  }

BOOL CTwain::TransferImage()
  {
    int nRetVal;
   
    m_ErrorInfo.Clear();
    nRetVal = DoNativeTransfer();
//    nRetVal = DoMemTransfer();
//    nRetVal = DoFileTransfer(CString("coast.bmp"));
    if(nRetVal)
      m_bAcquiredImage = TRUE;
    if(m_hWndClient)
      ::SendMessage(m_hWndClient, m_nClientStartMessage + CTMSG_TRANSFER, (WPARAM) nRetVal, 
        (LPARAM) (UINT)m_hBitmap);
    return nRetVal;
  }

BOOL CTwain::DoNativeTransfer()
  {
    TW_PENDINGXFERS     twPendingXfer;
    TW_UINT16           twRC;
    TW_UINT16           twRC2;
    TW_UINT32           hBitMap;
    LPBITMAPINFOHEADER  lpdib;
    BOOL                bSuccess=FALSE;
    
    m_ErrorInfo.Clear();
    // Do until there are no more pending transfers
    // explicitly initialize the our flags
    twPendingXfer.Count = 0;
    do
      {
//        if (AutoFeedOn())
//            at.ADFRunning = TRUE;

        // Initiate Native Transfer
        twRC = SourceManager(   &m_SourceID, 
                                DG_IMAGE,
                                DAT_IMAGENATIVEXFER, 
                                MSG_GET, 
                                (TW_MEMREF)&hBitMap);

        
        switch (twRC)
          {
            case TWRC_XFERDONE:  // Session is in State 7
              m_hBitmap = (HANDLE)hBitMap;
                
              //Acknowledge the end of the transfer 
              // and transition to state 6/5
              twRC2 = SourceManager(&m_SourceID, 
                                  DG_CONTROL,
                                  DAT_PENDINGXFERS, 
                                  MSG_ENDXFER,
                                  (TW_MEMREF)&twPendingXfer);
                
              // close the DSM and DS
              if (twPendingXfer.Count == 0)
                {       
                  if ((lpdib = (LPBITMAPINFOHEADER)GlobalLock(m_hBitmap))!=NULL)
                    {
                      DisableSource();
                      CloseSource();
                      CloseManager();
                      GlobalUnlock(m_hBitmap);
                      bSuccess = TRUE;
                    }
                }
              // Set error info here since closing manager can overwrite info.
              if (twRC2 != TWRC_SUCCESS)
                m_ErrorInfo.SetError(EE_LOCATION, CTRC_XFEREND_FAIL,
                  &m_twStatus);
              break;

            // the user canceled or wants to rescan the image
            // something wrong, abort the transfer and delete the image

            case TWRC_CANCEL:   // Session is in State 7
            case TWRC_FAILURE:  // Session is in State 6
              // transistion to state 6/5
              twRC2 = SourceManager(  &m_SourceID, 
                                      DG_CONTROL,
                                      DAT_PENDINGXFERS, 
                                      MSG_ENDXFER,
                                      (TW_MEMREF)&twPendingXfer);
            
              if (twRC2 != TWRC_SUCCESS && m_ErrorInfo.m_nCode)
                m_ErrorInfo.SetError(EE_LOCATION, CTRC_XFEREND_FAIL,
                  &m_twStatus);
                            
              // close the DSM and DS
              if (twPendingXfer.Count == 0)
                {
                  DisableSource();
                  CloseSource();
                  CloseManager();
                }
              if(twRC == TWRC_CANCEL)
                m_ErrorInfo.SetError(EE_LOCATION, CTRC_XFER_CANCELED, &m_twStatus);
              else
                m_ErrorInfo.SetError(EE_LOCATION, CTRC_XFER_FAILED, &m_twStatus);
              break;

            default:    //Sources should never return any other RC
              m_ErrorInfo.SetError(EE_LOCATION, CTRC_FAIL); // undefined
              break;
          }   
    
      }
    while (twPendingXfer.Count != 0);
    return bSuccess;
  }

BOOL CTwain::DoFileTransfer(CString &strFilename)
{
    TW_PENDINGXFERS     twPendingXfer;
    TW_UINT16           twRC;
    TW_UINT16           twRC2;
    TW_SETUPFILEXFER    setup;
    HFILE               hFile;
    OFSTRUCT            of;

    //=====Do until there are no more pending transfers
    // explicitly initialize the our flags
    twPendingXfer.Count = 0;  
    do
    {
        // NOTE: Twacker and sample source only support BMP transfers at this
        // time - SMC 11 MAY 95
        lstrcpyn(setup.FileName,strFilename.GetBuffer(1),strFilename.GetLength());
        setup.Format = TWFF_BMP;
        setup.VRefNum = 0;

        // create the file and close
        if ((hFile = OpenFile(setup.FileName, &of, OF_CREATE)) == HFILE_ERROR)
          return FALSE;   // Failed to create file

        _lclose(hFile);    // Close Created file
            
        if ((twRC = SourceManager(&m_SourceID, 
                                  DG_CONTROL,
                                  DAT_SETUPFILEXFER, 
                                  MSG_SET, 
                                  (TW_MEMREF)&setup)) != TWRC_SUCCESS)
          return FALSE; // Error in file transfer setup

//            if (AutoFeedOn())
//                at.ADFRunning = TRUE;
        
        // Initiate File Transfer
        twRC = SourceManager(   &m_SourceID, 
                                DG_IMAGE,
                                DAT_IMAGEFILEXFER, 
                                MSG_GET, 
                                (TW_MEMREF)NULL);

        switch (twRC)
        {
            case TWRC_XFERDONE:
            {
                //Successful Transfer
                OFSTRUCT            of;
                BITMAPFILEHEADER    header;
                TW_INT16            hfile;
                TW_UINT16           num=0x8000;
                TW_UINT32           count;
                unsigned char TW_HUGE   *ptr;

                // read the bitmap header and verify the transfer is a valid bmp
                // and create a handle to that bitmap

                m_hBitmap = NULL;
                if ((hfile = OpenFile(setup.FileName, &of, OF_READ)) != -1)
                {
                    _lread(hfile, (LPSTR)&header, sizeof(BITMAPFILEHEADER));
                    if (m_hBitmap = (HBITMAP) GlobalAlloc(GHND, header.bfSize))
                    {
                        ptr = (unsigned char *) GlobalLock(m_hBitmap);
                        for (count=header.bfSize-sizeof(BITMAPFILEHEADER)
                            ; count; count-=num, ptr+=num)
                        {
                            if (count < num) 
                                num = (TW_UINT16) count;
                            _lread(hfile, ptr, num);
                        }   
                        GlobalUnlock(m_hBitmap);
                    }   
                    _lclose(hfile);
                }   
                           
                //Acknowledge the end of the transfer
                // and transition to state 6/5
                twRC2 = SourceManager(&m_SourceID, 
                                    DG_CONTROL,
                                    DAT_PENDINGXFERS, 
                                    MSG_ENDXFER,
                                    (TW_MEMREF)&twPendingXfer);
                
                if (twRC2 != TWRC_SUCCESS)
                  {
                    CloseManager();
                    return FALSE;
                  }
                
                // close the DSM and DS
                if (twPendingXfer.Count == 0)
                    CloseManager();

               
//                if (!AutoFeedOn())
//                    AcqFlag = 1;
                
            }
            break;

            // the user canceled or wants to rescan the image
            // something wrong, abort the transfer and delete the image
            // pass a null ptr back to App
            case TWRC_CANCEL:
                //The Source is in state 7
                // transistion to state 6/5
                twRC2 = SourceManager(&m_SourceID, 
                                    DG_CONTROL,
                                    DAT_PENDINGXFERS, 
                                    MSG_ENDXFER,
                                    (TW_MEMREF)&twPendingXfer); 
                
                // close the DSM and DS
                if (twPendingXfer.Count == 0)
                    CloseManager();

                break;

            case TWRC_FAILURE:
                //The transfer failed
                //Determine Condition Code

                // Abort the image
                // Enhancement: Check Condition Code and attempt recovery instead
                twRC2 = SourceManager(&m_SourceID, 
                                    DG_CONTROL,
                                    DAT_PENDINGXFERS, 
                                    MSG_ENDXFER,
                                    (TW_MEMREF)&twPendingXfer); 
                
//                if (twRC2 != TWRC_SUCCESS)
                
                // close the DSM and DS
                if (twPendingXfer.Count == 0)
                    CloseManager();

                break;

            default:
                // Abort the image
                twRC2 = SourceManager(&m_SourceID, 
                                    DG_CONTROL,
                                    DAT_PENDINGXFERS, 
                                    MSG_ENDXFER,
                                    (TW_MEMREF)&twPendingXfer);
                
//                if (twRC2 != TWRC_SUCCESS)
                
                // close the DSM and DS
                if (twPendingXfer.Count == 0)
                    CloseManager();
                break;
        }   

    } while (twPendingXfer.Count != 0);
    return TRUE;
}   


////////////////////////////////////////////////////////////
// DoMemTransfer --
//
BOOL CTwain::DoMemTransfer()
{
    TW_PENDINGXFERS     twPendingXfer;
    TW_UINT16           twRC2;
    TW_UINT16           twRC;
    TW_IMAGEMEMXFER     xfer;
    TW_SETUPMEMXFER     setup;
    TW_IMAGEINFO        info;
    TW_PALETTE8         pal;
    LPBITMAPINFO        pdib;
    unsigned char TW_HUGE  *ptr;
    TW_UINT16           index;
    TW_UINT32           size;
    TW_CAPABILITY       cap;
    TW_UINT16           PixelFlavor;   
    pTW_ONEVALUE        pOneV;
    TW_UINT16           Units;
    float               XRes, YRes;
    int                 blocks;

    // set the cursor to wait as a memory transfer may take a long time based on
    // the size of the transfered chunks.  Smaller buffers will produce very slow
    // transfers, especially when thunking

    //=====Do until there are no more pending transfers
    // explicitly initialize the our flags
    twPendingXfer.Count = 0;
    do
    {
//        if (AutoFeedOn())
//            at.ADFRunning = TRUE;

      twRC = SourceManager(&m_SourceID, 
                          DG_IMAGE, 
                          DAT_IMAGEINFO,
                          MSG_GET, 
                          (TW_MEMREF)&info);
        
      if (twRC != TWRC_SUCCESS)
        return FALSE;             // Failed to get Image Info

      //Limited to 256 colors in Palette
      size = (((((TW_INT32)info.ImageWidth*info.BitsPerPixel+31)/32)*4)
                * info.ImageLength);

      // make the size an integral of the preferred transfer size
      twRC = SourceManager(&m_SourceID, 
                          DG_CONTROL,
                          DAT_SETUPMEMXFER, 
                          MSG_GET, 
                          (TW_MEMREF)&setup);

      blocks = (int)(size / setup.Preferred);
      size = (blocks +1) * setup.Preferred;

      // Allocate memory for bitmap
      m_hBitmap = (HBITMAP) GlobalAlloc(GHND,
          size + sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD));

      // If memory allocation failed, bail
      if (m_hBitmap == NULL)
        return FALSE;

      //Lock the Memory
      pdib = (LPBITMAPINFO)GlobalLock(m_hBitmap);

      // fill in the image information 
      pdib->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      pdib->bmiHeader.biWidth = info.ImageWidth;
      pdib->bmiHeader.biHeight = info.ImageLength;
      //Only 1 is supported
      pdib->bmiHeader.biPlanes = 1;
      pdib->bmiHeader.biBitCount = info.BitsPerPixel;
      //This application does not support compression
      pdib->bmiHeader.biCompression = BI_RGB;
      pdib->bmiHeader.biSizeImage = size;
      
      //Get Units and calculate PelsPerMeter
      cap.Cap = ICAP_UNITS;
      cap.ConType = TWON_DONTCARE16;
      cap.hContainer = NULL;
      
      twRC = SourceManager(   &m_SourceID, 
                              DG_CONTROL, 
                              DAT_CAPABILITY, 
                              MSG_GETCURRENT, 
                              (TW_MEMREF)&cap);

      // If capability call failed, default Pels info to 0
      // Is this OK?
      if (twRC != TWRC_SUCCESS)
        {
            pdib->bmiHeader.biXPelsPerMeter = 0;
            pdib->bmiHeader.biYPelsPerMeter = 0;
        }
      else
        {
          pOneV = (pTW_ONEVALUE)GlobalLock(cap.hContainer);
          Units = (TW_UINT16)(pOneV->Item);
          GlobalUnlock(cap.hContainer);
          GlobalFree((HANDLE)cap.hContainer);
          
          XRes = FIX32ToFloat(info.XResolution);
          YRes = FIX32ToFloat(info.YResolution);
          
          switch(Units)
          {
              case TWUN_INCHES:
                  pdib->bmiHeader.biXPelsPerMeter = (LONG)((XRes*2.54)*100);
                  pdib->bmiHeader.biYPelsPerMeter = (LONG)((YRes*2.54)*100);
                  break;
              case TWUN_CENTIMETERS:
                  pdib->bmiHeader.biXPelsPerMeter = (LONG)(XRes*100);
                  pdib->bmiHeader.biYPelsPerMeter = (LONG)(YRes*100);
                  break;
              case TWUN_PICAS:
              case TWUN_POINTS:
              case TWUN_TWIPS:
              case TWUN_PIXELS:
              default:
                  pdib->bmiHeader.biXPelsPerMeter = 0;
                  pdib->bmiHeader.biYPelsPerMeter = 0;
                  break;
          }
        }
      
      // Setup Palette -- if the palettes are B/W or shades of gray, the color
      // table is built here.  If the image is 8 bit color, a call to the
      // source is made to retrieve the correct set of colors.  If the call 
      // fails, the color table is constructed with 256 shades of gray inorder
      // to provide some image reference
      switch (info.PixelType)
      {
          case TWPT_BW:
              pdib->bmiHeader.biClrUsed = 2;
              pdib->bmiHeader.biClrImportant = 0;
                                              
              //Get CAP_PIXELFLAVOR to determine colors
              //fill in the palette information
              cap.Cap = ICAP_PIXELFLAVOR;
              cap.ConType = TWON_DONTCARE16;
              cap.hContainer = NULL;
              
              twRC = SourceManager(   &m_SourceID, 
                                      DG_CONTROL, 
                                      DAT_CAPABILITY, 
                                      MSG_GETCURRENT, 
                                      (TW_MEMREF)&cap);
  
              if (twRC != TWRC_SUCCESS)
                {   
                  PixelFlavor = TWPF_CHOCOLATE;
                }
              else
                {
                  if (cap.ConType != TWON_ONEVALUE)
                    {
                      PixelFlavor = TWPF_CHOCOLATE;
                    }
                  else
                    {                            
                      pOneV = (pTW_ONEVALUE)GlobalLock(cap.hContainer);
                      PixelFlavor = (TW_UINT16)(pOneV->Item);
                      GlobalUnlock(cap.hContainer);
                    }                       
                  GlobalFree((HANDLE)cap.hContainer);
              }
              if (PixelFlavor == 0)
                {
                  //0=Black
                  pdib->bmiColors[0].rgbGreen = 0x0000;
                  pdib->bmiColors[0].rgbBlue = 0x0000;
                  pdib->bmiColors[0].rgbReserved = 0;

                  pdib->bmiColors[1].rgbRed = 0x00FF;
                  pdib->bmiColors[1].rgbGreen = 0x00FF;
                  pdib->bmiColors[1].rgbBlue = 0x00FF;
                  pdib->bmiColors[1].rgbReserved = 0;
                }
              else
                {
                  //ICAP_PIXELFLAVOR == 1
                  //0=White
                  pdib->bmiColors[0].rgbRed = 0x0000;
                  pdib->bmiColors[0].rgbRed = 0x00FF;
                  pdib->bmiColors[0].rgbGreen = 0x00FF;
                  pdib->bmiColors[0].rgbBlue = 0x00FF;
                  pdib->bmiColors[0].rgbReserved = 0;

                  pdib->bmiColors[1].rgbRed = 0x0000;
                  pdib->bmiColors[1].rgbGreen = 0x0000;
                  pdib->bmiColors[1].rgbBlue = 0x0000;
                  pdib->bmiColors[1].rgbReserved = 0;
               }
              break;
          
          case TWPT_GRAY:
              pdib->bmiHeader.biClrUsed = 256;
              for (index=0; index<256; index++)
              {
                  pdib->bmiColors[index].rgbRed = (BYTE) index;
                  pdib->bmiColors[index].rgbGreen = (BYTE) index;
                  pdib->bmiColors[index].rgbBlue = (BYTE) index;
                  pdib->bmiColors[index].rgbReserved = 0;
              }
              break;

          case TWPT_RGB:
              pdib->bmiHeader.biClrUsed = 0;
              break;
          
          case TWPT_PALETTE:
          case TWPT_CMY:
          case TWPT_CMYK:
          case TWPT_YUV:
          case TWPT_YUVK:
          case TWPT_CIEXYZ:
          default:
              //fill in the palette information
              twRC = SourceManager(   &m_SourceID, 
                                      DG_IMAGE, 
                                      DAT_PALETTE8, 
                                      MSG_GET, 
                                      (TW_MEMREF)&pal);
              
              if (twRC != TWRC_SUCCESS)
              {
                  // defaulting to 256 gray image palette
                  
                  pdib->bmiHeader.biClrImportant = 0;
                  pdib->bmiHeader.biClrUsed = 256;
                  for (index=0; index<pal.NumColors; index++)
                  {
                      pdib->bmiColors[index].rgbRed = (BYTE)index;
                      pdib->bmiColors[index].rgbGreen = (BYTE)index;
                      pdib->bmiColors[index].rgbBlue = (BYTE)index;
                      pdib->bmiColors[index].rgbReserved = 0;
                  }   
              }
              else
              {
                  pdib->bmiHeader.biClrUsed = pal.NumColors;
                  pdib->bmiHeader.biClrImportant = 0;
                  for (index=0; index<pal.NumColors; index++)
                  {
                      pdib->bmiColors[index].rgbRed = pal.Colors[index].Channel1;
                      pdib->bmiColors[index].rgbGreen = pal.Colors[index].Channel2;
                      pdib->bmiColors[index].rgbBlue = pal.Colors[index].Channel3;
                      pdib->bmiColors[index].rgbReserved = 0;
                  }   
              }
              break;              
      }   //end switch(PixelType)

      // locate the start of the bitmap data 
      ptr = (unsigned char TW_HUGE *) pdib;
      ptr += sizeof(BITMAPINFOHEADER);
      ptr += pdib->bmiHeader.biClrUsed * sizeof(RGBQUAD);
      
      // determine the buffer size 
      twRC = SourceManager(   &m_SourceID, 
                              DG_CONTROL,
                              DAT_SETUPMEMXFER, 
                              MSG_GET, 
                              (TW_MEMREF)&setup);
      
      if (twRC != TWRC_SUCCESS)
      {
        // Error Setting up transfer
      }
      else
      {
          // we will use a pointer to shared memory
          xfer.Memory.Flags = TWMF_APPOWNS | TWMF_POINTER;
          xfer.Memory.Length = setup.Preferred;
          xfer.Memory.TheMem = ptr;
          
          // transfer the data -- loop until done or canceled 
          do
          {
              twRC = SourceManager(   &m_SourceID, 
                                      DG_IMAGE,
                                      DAT_IMAGEMEMXFER, 
                                      MSG_GET, 
                                      (TW_MEMREF)&xfer);
              switch (twRC)
              {
                  case TWRC_SUCCESS:
                      ptr += xfer.BytesWritten;
                      xfer.Memory.TheMem = ptr;
                      break;

                  case TWRC_XFERDONE:
                      //Successful Transfer
                      GlobalUnlock(m_hBitmap);

                      // since the sample source is reading a DIB, the bitmap
                      // is already right side up.  A memory transfer from a real
                      // source will be the reverse and needs to be flipped
                      FlipBitMap(info.PixelType);
                      
                      //Acknowledge the end of the transfer
                      // and transition to state 6/5
                      twRC2 = SourceManager(&m_SourceID, 
                                            DG_CONTROL,
                                            DAT_PENDINGXFERS, 
                                            MSG_ENDXFER,
                                            (TW_MEMREF)&twPendingXfer);

                      // Error Ending Tranfer
//                      if (twRC2 != TWRC_SUCCESS)
          
                                                      
                      // close the DSM and DS
                      if (twPendingXfer.Count == 0)
                          CloseManager();

                      // showRC_CC is a safe operation here since there will be no triplet
                      // calls generated 
                      break;

                  case TWRC_CANCEL:
                      //The Source is in state 7
                      // transistion to state 6/5
                      
                      twRC2 = SourceManager(  &m_SourceID, 
                                              DG_CONTROL,
                                              DAT_PENDINGXFERS, 
                                              MSG_ENDXFER,
                                              (TW_MEMREF)&twPendingXfer);

                      // Error ending transfer
//                      if (twRC2 != TWRC_SUCCESS)
                                                                  
                      GlobalUnlock(m_hBitmap);
                      GlobalFree(m_hBitmap);
                      m_hBitmap = NULL;
                      
                      // close the DSM and DS
                      if (twPendingXfer.Count == 0)
                          CloseManager();
                      break;
                  
                  case TWRC_FAILURE:
                      //The transfer failed
                      // Enhancement: Check Condition Code and attempt recovery

                      twRC2 = SourceManager(&m_SourceID, 
                                            DG_CONTROL,
                                            DAT_PENDINGXFERS, 
                                            MSG_ENDXFER,
                                            (TW_MEMREF)&twPendingXfer);
                      
//                      if (twRC2 != TWRC_SUCCESS)
          
                      GlobalUnlock(m_hBitmap);
                      GlobalFree(m_hBitmap);
                      m_hBitmap = NULL;
                      
                      // close the DSM and DS
                      if (twPendingXfer.Count == 0)               
                          CloseManager();
                      break;
                  
                  default:
                      // Abort the image
                      twRC2 = SourceManager(  &m_SourceID, 
                                              DG_CONTROL,
                                              DAT_PENDINGXFERS, 
                                              MSG_ENDXFER,
                                              (TW_MEMREF)&twPendingXfer);
                      
//                      if (twRC2 != TWRC_SUCCESS)
                                          
                      GlobalUnlock(m_hBitmap);
                      GlobalFree(m_hBitmap);
                      m_hBitmap = NULL;
                      
                      // close the DSM and DS
                      if (twPendingXfer.Count == 0)
                          CloseManager();
                      
                      // showRC_CC is a safe operation here since there will be no triplet
                      // calls generated 
                      break;
              } // switch
          } while (twRC == TWRC_SUCCESS);
      } 
    } while (twPendingXfer.Count != 0);
  return TRUE;
}   


//////////////////////////////////////////////////////////////////////////////
// FlipBitMap -- Takes a memory transfer buffer and changes it to a DIB format
//
//    i.e.    Memory Format
//                         1  2  3  4  5
//                         6  7  8  9 10
//                        11 12 13 14 15
//
//            DIB bitmap Format
//                        11 12 13 14 15
//                         6  7  8  9 10
//                         1  2  3  4  5                      
//
//    Memory RGBQuad order: RGB
//    Windows DIB RGBQuad order: BGR
//        
BOOL CTwain::FlipBitMap(TW_INT16 PixType)
{
    HANDLE                  temp;
    LPBITMAPINFO            pdib;
    BYTE*                   pDib;
    unsigned char TW_HUGE   *pbuffer;
    unsigned char TW_HUGE   *tempptr;
    unsigned char TW_HUGE   *tempptrsave;
    LONG                    Width;
    LONG                    Height;
    LONG                    Linelength;
    LONG                    indexH;
    DWORD                   Size;
    DWORD                   SizeImage;
    WORD                    BitCount;
    DWORD                   ClrUsed;   
    DWORD                   offset;
    TW_UINT16               pixels;
    TW_INT32                items;
    TW_UINT32               i;
    BYTE                    SaveRed;
    BYTE                    SaveBlue;
    
    pDib = (BYTE*)GlobalLock(m_hBitmap);
    pdib = (LPBITMAPINFO)pDib;

    Width = pdib->bmiHeader.biWidth;
    Height = pdib->bmiHeader.biHeight;
    Size = pdib->bmiHeader.biSize;
    SizeImage = pdib->bmiHeader.biSizeImage;
    BitCount = pdib->bmiHeader.biBitCount;
    ClrUsed = pdib->bmiHeader.biClrUsed;

    if ( !(temp = GlobalAlloc(GHND, SizeImage)) )
      {
        GlobalUnlock(m_hBitmap);
        return FALSE;
      }

    tempptr = (unsigned char *) GlobalLock(temp);
    tempptrsave = tempptr;

    // calculate offset to start of the bitmap data
    offset = sizeof(BITMAPINFOHEADER);
    offset += pdib->bmiHeader.biClrUsed * sizeof(RGBQUAD);

    Linelength = (((Width*BitCount+31)/32)*4);

    //Goto Last line in bitmap
    offset += (Linelength * (Height-1));
    
    #ifdef WIN32
        pDib = pDib + offset - Linelength;
    #endif

    //For each line
    for (indexH = 1; indexH < Height; indexH++)
    {
        #ifdef WIN32
            memcpy(tempptr, pDib, Linelength);
            pDib -= (Linelength);
        #else 
            _fmemcpy(tempptr, pDib, (size_t)Linelength);
            pDib -= (Linelength);
            //::MemoryRead(m_hBitmap, offset, tempptr, Linelength);
            //offset -= (Linelength);
        #endif
        tempptr += Linelength;
    }

    // Copy temp over bitmap
    pbuffer = (unsigned char TW_HUGE *) pdib;
    pbuffer += sizeof(BITMAPINFOHEADER);
    pbuffer += pdib->bmiHeader.biClrUsed * sizeof(RGBQUAD);
    
    #ifdef WIN32
        memcpy(pbuffer, tempptrsave, SizeImage);
    #else
//        MemoryRead(temp, 0, pbuffer, SizeImage);
        _fmemcpy(pbuffer, tempptrsave, (size_t)SizeImage);
    #endif
    
    //Flip RGB color table
    if (PixType == TWPT_RGB)
    {
        pbuffer = (unsigned char TW_HUGE *) pdib;
        pbuffer += sizeof(BITMAPINFOHEADER);
        pbuffer += pdib->bmiHeader.biClrUsed * sizeof(RGBQUAD);
        
        pixels = (TW_UINT16)pdib->bmiHeader.biWidth;
        for (items = 0; items < Height; items++)
        {
            tempptr = pbuffer;
            for (i=0; i<pixels; i++)
            {
                //Switch Red byte and Blue byte
                SaveRed = (BYTE)*tempptr;
                SaveBlue = (BYTE)*(tempptr+2);
                (BYTE)*tempptr = SaveBlue;
                (BYTE)*(tempptr+2) = SaveRed;
                //increment to next triplet
                tempptr += 3;
            }
            pbuffer += Linelength;
        }
    } 
    
    //Unlock
    GlobalUnlock(m_hBitmap);
    GlobalUnlock(temp);

    //Free
    GlobalFree(temp);

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// FIX32ToFloat -- Convert a FIX32 value into a floating point value
//
float CTwain::FIX32ToFloat (TW_FIX32 fix32)
{
    float   floater;
    floater = (float) fix32.Whole + (float) (fix32.Frac / 65536.0);
    return(floater);
}

TW_UINT16 CTwain::SourceManager(pTW_IDENTITY pSourceID, TW_UINT32 dwDataGroup, TW_UINT16 usDataType, TW_UINT16 usMessage, TW_MEMREF pData)
{
  TW_UINT16     twRC, twERC;
  pTW_IDENTITY  pSourceIDParm;

  memset(&m_twStatus, 0, sizeof(m_twStatus)); // Clear Twain Error struct

  if(m_lpDSM_Entry == NULL)
      return TWRC_FAILURE;

  twRC = (*m_lpDSM_Entry)(&m_AppIdentity,
                          pSourceID,
                          dwDataGroup,
                          usDataType,
                          usMessage,
                          pData);
  // If operation failed, get detailed info on why
  if(twRC == TWRC_FAILURE)
    {
      if (m_bSourceOpen)
        pSourceIDParm = pSourceID;  // Get Source status 
      else if(m_bManagerOpen)
        pSourceIDParm = NULL;       // Get manager status
      else
        // nothing open, not a good time to get condition code!
        {
          m_ErrorInfo.SetError(EE_LOCATION, twRC, TWCC_SEQERROR);
          return TWCC_SEQERROR;
        }
        // get status if source or manager are open
      twERC = (*m_lpDSM_Entry)(&m_AppIdentity,
                              pSourceIDParm,
                              DG_CONTROL,
                              DAT_STATUS,
                              MSG_GET,
                              &m_twStatus);
    }

  return twRC;
}

void CTwain::MakeBMPHeader()
{
  LPBITMAPINFOHEADER  pBitmapInfoHeader;

  m_BitmapFileHeader.bfType = 0x4D42;      // "BM"
  m_BitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)
    + sizeof(BITMAPINFOHEADER);

  // If scanned image 8 bit, assume it has a palette
  pBitmapInfoHeader = (LPBITMAPINFOHEADER)::GlobalLock(m_hBitmap);
  if(pBitmapInfoHeader->biBitCount <= 8)
    m_BitmapFileHeader.bfOffBits +=
      (pBitmapInfoHeader->biClrUsed * sizeof(RGBQUAD));
  m_BitmapFileHeader.bfSize = m_BitmapFileHeader.bfOffBits +
    ::GlobalSize(m_hBitmap);
  ::GlobalUnlock(m_hBitmap);

  m_BitmapFileHeader.bfReserved1 = 0;
  m_BitmapFileHeader.bfReserved2 = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CModalTwain

CModalTwain::CModalTwain()
{
}

CModalTwain::~CModalTwain()
{
}

HANDLE CModalTwain::Acquire(HWND hWnd)
{
	HANDLE hDib = NULL;

	if (INHERITED::Acquire(hWnd))
	{
		CWnd* pParentWnd = CWnd::FromHandle(hWnd);
		BOOL fContinue = TRUE;
		while (fContinue)
		{
			MSG Msg;
			while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (!::GetMessage(&Msg, NULL, NULL, NULL))
				{
					fContinue = FALSE;
					::PostQuitMessage(0);
					break;
				}

				// Note that fContinue may be set to FALSE, but we continue to process message
				// until we run out and then we return from DoModal.
				if (!ProcessMessage(&Msg))
				{
					::TranslateMessage(&Msg);
					::DispatchMessage(&Msg);
				}

				if(!m_bManagerOpen || !m_bSourceOpen)
				{
					fContinue = FALSE;
					break;
				}
			}
			if (fContinue)
			{
				// Ran out of messages. Process idle message.
				if (pParentWnd != NULL &&
						!(pParentWnd->GetStyle() & DS_NOIDLEMSG))
				{
#ifdef WIN32
					pParentWnd->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)NULL);
#else
					pParentWnd->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, MAKELPARAM(NULL, 0));
#endif
				}
			}
		}

		hDib = m_hBitmap;
		m_hBitmap = NULL;
	 }

    return hDib;
}

BOOL CModalTwain::EnableSource(void)
{
	if (INHERITED::EnableSource())
	{
		if (m_hWnd != NULL)
		{
			::EnableWindow(m_hWnd, FALSE);
#ifndef WIN32
			// Check out \msvc\mfc\src\winfrm.cpp CFrameWnd::OnSetCursor().
			::SetProp(AfxGetMainWnd()->GetSafeHwnd(), "StayDisabled", (HANDLE)1);
#endif
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CModalTwain::DisableSource(void)
{
	if (m_hWnd != NULL)
	{
#ifndef WIN32
			// Check out \msvc\mfc\src\winfrm.cpp CFrameWnd::OnSetCursor().
		::RemoveProp(AfxGetMainWnd()->GetSafeHwnd(), "StayDisabled");
#endif
		::EnableWindow(m_hWnd, TRUE);
	}
	return INHERITED::DisableSource();
}

void CModalTwain::DisplayError()
{
  CString   csMessage, csError, csCode, csSource;

  csCode.Empty();   // Gaurantee default empty string
  switch(m_ErrorInfo.m_nCode)
  {
    case CTRC_SUCCESS:
      csError.LoadString(IDS_ERR_TWAIN_NONE);
      break;            // Why are we here?
    case CTRC_FAIL:
      csError.LoadString(IDS_ERR_TWAIN);
      break;
    case CTRC_SEQUENCE_ERR:
      csError.LoadString(IDS_ERR_TWAIN_SEQUENCE);
      break;
    case CTRC_FINDLIB_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_FINDLIB);
      break;
    case CTRC_SMOPEN_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_SMOPEN);
      break;
    case CTRC_SMCLOSE_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_SMCLOSE);
      break;
    case CTRC_SMLIB_NOTLOADED:
      csError.LoadString(IDS_ERR_TWAIN_NOTLOADED);
      break;
    case CTRC_DSOPEN_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_DSOPEN);
      break;
    case CTRC_DSCLOSE_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_DSCLOSE);
      break;
    case CTRC_DSENABLE_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_DSENABLE);
      break;
    case CTRC_DSDISABLE_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_DSDISABLE);
      break;
    case CTRC_DSSELECT_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_DSSELECT);
      break;
    case CTRC_DSXFERTYPE_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_DSXFERTYPE);
      break;
    case CTRC_XFEREND_FAIL:
      csError.LoadString(IDS_ERR_TWAIN_XFEREND);
      break;
    case CTRC_XFER_CANCELED:
      csError.LoadString(IDS_ERR_TWAIN_XFER_CANCELED);
      break;
    case CTRC_XFER_FAILED:
      csError.LoadString(IDS_ERR_TWAIN_XFER);
      break;
    default:
      break;
  }
#ifdef MORE_TWAIN_ERROR_INFO
  if(m_ErrorInfo.m_pCustomCode != NULL)
  {
	  TW_STATUS* pStatus = (TW_STATUS*)m_ErrorInfo.m_pCustomCode;
    if(pStatus->ConditionCode)
      csCode.Format(", Code:%i", pStatus->ConditionCode);
  }
  csSource.Format("\nFile: %s Line:%i", m_ErrorInfo.m_szFileName, m_ErrorInfo.m_nLineNum);
#endif

  // JRO csMessage = "Image Acquire Failed\n\n" + csError;
  csMessage.LoadString (IDS_ACQUIRE_FAIL);

#ifdef MORE_TWAIN_ERROR_INFO
  csMessage += csCode + csSource;  // Add location of error
#endif

  AfxMessageBox(csMessage);
}
