/******************************************************************************
*  FILE:        URLPROC.CPP                                                   *
*  SUMMARY:     Class for sending URL commands to web browsers.               *
*  AUTHOR:      Aaron Evans (AE)                                              *
*                                                                             *
// $Log: /PM8/App/urlproc.cpp $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 6     3/27/98 5:50p Johno
// Start log
*  ---------------------------------------------------------------------------*
*  Initials Legend:                                                           *
*     AE    Aaron Evans                                                       *
*  ---------------------------------------------------------------------------*
* 12/24/97  DGP   Replaced Pietrek code with more generic code for locating   *
*                 processes under Windows 95 and NT.                          *
* 04-08-96  AE    Created                                                     *
*                                                                             *
*  Revision History                                                           *
*                                                                             *
*  Date     Init  Description of Change                                       *
*  ---------------------------------------------------------------------------*
******************************************************************************/
/*****************************************************************
Optional Defines: (used in #ifdef statements)  Set these in the header.
USING_CALLBACK -- enables logic in call-back fn.  To enable this properly,
		also take a look at XOPT_CLOSECONV below.  GetObjectPtr() in the call-back
		was causing G.P.F. when compiled for 286 processors.
*******************************************************************/


#include "stdafx.h"
#include "urlproc.h"
#include "enumproc.h"
#include "util.h"
#include "procloc.h"
                     
#ifdef USING_CALLBACK                     
CURLProcessor FAR * CURLProcessor::m_this_obj = 0;//used by GetObjPtr
CURLProcessor FAR * CURLProcessor::GetObjPtr();//used by DDE callback fn. 
#endif


extern CWinApp theApp;
      
CURLProcessor::CURLProcessor()
{ 
  m_pConv = 0;
  m_idInst = 0;
  m_lpDdeProc = 0;
  m_bInitialized = FALSE;
#ifdef USING_CALLBACK
  m_this_obj = this; 
#endif
}


/******************************************************************************
* FUNCTION: Intialize()                                                       *
*                                                                             *
* PURPOSE:  To register this program with the DDEML (management lib)          *
*                                                                             *
* RETURNS:  DDE_OK if successful                                              *
*           DDE_ERR_MAKEPROCINST if error calling MakeProcInstance            *
*           DDE_ERR_INITIALIZE if error calling DdeInitialize                 *
*                                                                             *
******************************************************************************/
URLProcessorRetCode CURLProcessor::Initialize()
{
   HINSTANCE               hInst;
   char                    szBrowserEXE[_MAX_PATH];
   CString                 csPublishPath;
   CString                 csBrowserTitlebarName, csBrowserDDEName;

   Util::SplitPath(m_csURL, &csPublishPath, NULL);
   hInst = FindExecutable(m_csURL, csPublishPath, szBrowserEXE);
   if(hInst <= (HINSTANCE) 32)
      return enURLErrorNoDefaultBrowser;

   m_csBrowserPathAndName = szBrowserEXE;

   m_csBrowserPathAndName.MakeUpper();

   if(m_csBrowserPathAndName.Find("NETSCAPE.EXE") >= 0)
      {
         csBrowserTitlebarName = csBrowserDDEName = "NETSCAPE";
      }
   else if(m_csBrowserPathAndName.Find("IEXPLORE.EXE") >= 0)
      {
		   csBrowserDDEName = "IEXPLORE";
		   csBrowserTitlebarName = "INTERNET EXPLORER";
      }
   else if(m_csBrowserPathAndName.Find("AIRMOS.EXE") >= 0)
      {
		   csBrowserDDEName = "MOSAIC";
		   csBrowserTitlebarName = "MOSAIC";
      }
   else
      return enURLErrorNotConfigured;

   m_csBrowserTitlebarName = csBrowserTitlebarName;

   if(m_csBrowserTitlebarName.GetLength() == 0)
      return enURLErrorNotConfigured;
   m_csBrowserTitlebarName.MakeUpper();

   //to seperate out the path from the name, use this:
   int pos = m_csBrowserPathAndName.ReverseFind('\\');
   m_csBrowserPath = m_csBrowserPathAndName.Left(pos);
   m_csBrowserName = m_csBrowserPathAndName.Right(m_csBrowserPathAndName.GetLength()-pos-1); 

   m_csBrowserDDEID = csBrowserDDEName;

   if(m_csBrowserDDEID.GetLength() == 0)
      return enURLErrorNotConfigured;

   m_csWebSearchSpacer = "+";

   if(!(m_lpDdeProc = MakeProcInstance((FARPROC)DdeCallBack, AfxGetApp()->m_hInstance)))
      {
         ASSERT(0);
         return enURLErrorInit;
      }
   if(DdeInitialize((LPDWORD) &m_idInst, (PFNCALLBACK) m_lpDdeProc,APPCMD_CLIENTONLY, 0L) != DMLERR_NO_ERROR)
      {
         ASSERT(0);
         return enURLErrorInit;
      }

   m_bInitialized = TRUE;

   return enURLErrorNone;
} 
       
       
URLProcessorRetCode CURLProcessor::OpenURL(CString csURL,BOOL bIsWebSearch)
{   
   HINSTANCE            hInst;
	URLProcessorRetCode  retCode;
	
	retCode = DoOpenURL(csURL,bIsWebSearch);
   // If we failed to Find existing browser process or DDE failed, 
   // try to run default browser by executing URL document
	if(retCode != enURLErrorNone && retCode != enURLErrorNoDefaultBrowser)
   {
      // TODO - Have Initialize take optional parent parameter
      hInst = ShellExecute(NULL, "open", m_csURL, NULL, NULL, SW_SHOWDEFAULT);
      if(hInst <= (HINSTANCE) 32)
         retCode = enURLErrorNoDefaultBrowser;
      else
         retCode = enURLErrorNone;
   }
   return retCode;
} 


/******************************************************************************
* FUNCTION: OpenURL()                                                         *
*                                                                             *
* PURPOSE:  To pass a URL string to the Browser.  If we are not connected to  * 
*           the browser, will initiate the connection.  If connection fails,  *
*           will launch the app.                                              *
*                                                                             *
* RETURNS:  DDE_OK if successful.                                             *
*           DDE_ERR_STRHANDLE if error creating a string handle (low on       *
*                             resources).                                     *
*           DDE_ERR_CONNECT if error (besides DMLERR_NO_CONV_ESTABLISHED)     *
*                           occured while trying to connect.                  *
******************************************************************************/
URLProcessorRetCode CURLProcessor::DoOpenURL(CString csURL,BOOL bIsWebSearch)
{
   char        hWnd[9];
   char        dwFlags[2];
   char        szT[258];
   WORD        Error;//holds error value if conversation not established.
   HSZ         hszApp, hszTopic;
   HSZ         hszItem = 0;//handle to URL string
   CString     csTemp;
   URLProcessorRetCode  retCode;

   m_csURL = csURL;
   if(!m_bInitialized)
      {
         retCode = Initialize();
         if(retCode != enURLErrorNone)
            return retCode;
      }

   // if Browser was found, try to establish a DDE conversation
   if(BringBrowserToForeground())
      {
         if(!m_pConv)
            m_pConv = new CConv(1000,XTYP_REQUEST,1,XOPT_ASYNC|XOPT_DISABLEFIRST|XOPT_ABANDONAFTERSTART|XOPT_CLOSECONV);
         if(!m_pConv->Connected())//Connect to the browser.
         {
            if(!(hszApp = DdeCreateStringHandle(m_idInst, m_csBrowserDDEID, NULL)))
            {
               delete m_pConv;
               m_pConv = 0;
               return enURLErrorDdeStringHandle;
            }
            if(!(hszTopic = DdeCreateStringHandle(m_idInst,"WWW_OpenURL", NULL)))
            {
               delete m_pConv;
               m_pConv = 0;
               return enURLErrorDdeStringHandle;
            }
            m_pConv->Connect(m_idInst,hszApp, hszTopic,&Error);      
            DdeFreeStringHandle(m_idInst,hszApp);
            DdeFreeStringHandle(m_idInst,hszTopic);
            if(!m_pConv->Connected())
            {
               if(m_pConv)
                  delete m_pConv;
               m_pConv = 0;       
               return enURLErrorDdeConnect; 
            }
         }
         //create transaction string.
         //ADD ADDITIONAL QUOTES TO THE STRING: MOSIAC, EXPLORER REQUIRE THIS.
         CString temp = "\"" + csURL;
         csURL = temp + "\"";
  
         strncpy(szT,csURL,256);
         itoa(-1,hWnd,10);//use current browser window
         dwFlags[0] = 0x0;//ignore the document cache-- not sure why values is set to this
         dwFlags[1] = '\0';
         strcat(szT,",");
         strcat(szT,",");
         strcat(szT,hWnd);
         strcat(szT,",");
         strcat(szT,dwFlags);
         strcat(szT,",");
         strcat(szT,",");
         strcat(szT,",");
         if(!(hszItem = DdeCreateStringHandle(m_idInst, szT, NULL)))
         {
            if(m_pConv)
            delete m_pConv;
            m_pConv = 0;
            return enURLErrorDdeStringHandle;
         }
         retCode = m_pConv->ProcessTransaction(m_idInst,hszItem);
         DdeFreeStringHandle(m_idInst,hszItem);
         if (m_pConv->fsOptions & XOPT_CLOSECONV)//if callback not used, terminate the conversation.
         {
            delete m_pConv;
            m_pConv = 0;
         }      
      }
   else
      retCode = enURLBrowserNotFound;

   return retCode;
}
    
 /***************************************************************************
 *                                                                          *
 *  FUNCTION   : ProcessTransaction()  taken from ddeml sample app          *
 *                                                                          *
 *  PURPOSE    : Processes synchronous transactions entirely and starts     *
 *               async transactions. Pass in the handle to the URL string.  *
 *                                                                          *
 *  RETURNS    : TRUE  - If successful.                                     *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/
URLProcessorRetCode CConv::ProcessTransaction(DWORD idInst,HSZ hszItem)
{
    HWND hwndInfoCtrl = 0;

    /*
     * Disable callbacks for this conversation now if the XOPT_DISABLEFIRST
     * option is set.  This tests disabling asynchronous transactions
     * before they are completed.
     */
    if (fsOptions & XOPT_DISABLEFIRST)
        DdeEnableCallback(idInst, hConv, EC_DISABLE);
    /*
     * Adjust the timeout for asynchronous transactions.
     */
    if (fsOptions & XOPT_ASYNC)
        ulTimeout = (DWORD)TIMEOUT_ASYNC;

    /*
     * start transaction with DDEML here
     */
    //note: for future use, pass in value for first parameter to retrieve data from transaction.
    
    // ** Future DDE may not support Result Parameter **
    ret = DdeClientTransaction(NULL, (DWORD)-1,hConv, hszItem, wFmt,wType,
            ulTimeout, (LPDWORD)&Result);
    //UINT Error = DdeGetLastError(idInst);
    if(!ret)
      return enURLErrorDdeProcessTrans;//use DdeGetLastError(idInst) to get more details.
    else if (fsOptions & XOPT_ASYNC) 
     {
        /*
         * asynchronous successful start - link transaction to window.
         */
        //DdeSetUserHandle(hConv, Result, (DWORD)hwndInfoCtrl);

        /*
         * Abandon started async transaction after initiated if
         * XOPT_ABANDONAFTERSTART is chosen.  This tests the mid-transaction
         * abandoning code.
         */
        if (fsOptions & XOPT_ABANDONAFTERSTART)
            DdeAbandonTransaction(idInst, hConv, Result);
      } 
    return enURLErrorNone;
} 

BOOL CConv::Connected()
{
	return hConv ? TRUE: FALSE;
}





/****************************************************************************
 *                                                                          *
 *  FUNCTION   : GetTextData()  AE: taken from DDEML sample app.  Not       *
 *                              currently used.                             *
 *                                                                          *
 *  PURPOSE    : Allocates and returns a pointer to the data contained in   *
 *               hData.  This assumes that hData points to text data and    *
 *               will properly handle huge text data by leaving out the     *
 *               middle of the string and placing the size of the string    *
 *               into this string portion.                                  *
 *                                                                          *
 *  RETURNS    : A pointer to the allocated string.                         *
 *                                                                          *
 ****************************************************************************/
unsigned char * CURLProcessor::GetTextData(
HDDEDATA hData)
{
    unsigned char * psz;
    DWORD cb;

#define CBBUF  1024

    if (hData == NULL) {
        return(NULL);
    }

    cb = DdeGetData(hData, NULL, 0, 0);
    if (!hData || !cb)
        return NULL;

    if (cb > CBBUF) {            
        //warn_check_retval_of_malloc;
        psz = (unsigned char *)malloc(sizeof CBBUF);
        DdeGetData(hData, (unsigned char *)psz, CBBUF - 46, 0L);
    } else {
        psz = (unsigned char *)malloc((WORD)cb);
        DdeGetData(hData, (unsigned char *)psz, cb, 0L);
    }
    return psz;
#undef CBBUF
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : CreateConv()   AE: Taken from DDEML sample app             *
 *                                                                          *
 *  PURPOSE    :                                                            *
 *                                                                          *
 *  RETURNS    :                                                            *
 *                                                                          *
 ****************************************************************************/
HCONV CConv::CreateConv(DWORD idInst,HSZ hszApp,HSZ hszTopic,BOOL fList,WORD *pError)
{
    HCONV hConv;
    HWND hwndConv = 0;
    CONVINFO ci;

    hConv = DdeConnect(idInst, hszApp, hszTopic, 0);
    if (hConv) {
        if (fList) {
            ci.hszSvcPartner = hszApp;
            ci.hszTopic = hszTopic;
        } else {
            ci.cb = sizeof(CONVINFO);
        DdeQueryConvInfo(hConv, (DWORD)QID_SYNC, &ci);
        }
 //       hwndConv = AddConv(ci.hszSvcPartner, ci.hszTopic, hConv, fList);
        // HSZs get freed when window dies.
    }
/*    if (!hwndConv) {
        if (pError != NULL) {
            *pError = DdeGetLastError(m_idInst);
        }
    }*/
    return(hConv);
}


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : AddConv()  AE: Taken from DDEML sample app.                *
 *                          Does nothing; used when you do some sort of     *
 *                          wildcard connect and connect to all apps that   *
 *                          support a particular conversation topic         *
 *                                                                          *
 *  PURPOSE    : Creates an MDI window representing a conversation          *
 *               (fList = FALSE) or a set of MID windows for the list of    *
 *               conversations (fList = TRUE).                              *
 *                                                                          *
 *  EFFECTS    : Sets the hUser for the conversation to the created MDI     *
 *               child hwnd.  Keeps the hszs if successful.                 *
 *                                                                          *
 *  RETURNS    : created MDI window handle.                                 *
 *                                                                          *
 ****************************************************************************/
/*HWND FAR PASCAL CURLProcessor::AddConv(
HSZ hszApp,     // these parameters MUST match the MYCONVINFO struct.
HSZ hszTopic,
HCONV hConv,
BOOL fList)
{
    HWND hwnd;

    if (fList) {
        //
        // Create all child windows FIRST so we have info for list window.
         //
        CONVINFO ci;
        HCONV hConvChild = 0;

        ci.cb = sizeof(CONVINFO);
        while (hConvChild = DdeQueryNextServer((HCONVLIST)hConv, hConvChild)) {
        if (DdeQueryConvInfo(hConvChild, (DWORD)QID_SYNC, &ci)) {
                AddConv(ci.hszSvcPartner, ci.hszTopic, hConvChild, FALSE);
            }
        }
    }

    return hwnd;
}*/







CURLProcessor::~CURLProcessor()
{
  if(m_pConv)
    delete m_pConv;
  if (m_lpDdeProc)
    FreeProcInstance(m_lpDdeProc);
  delete m_pConv;
}


/************************************************************************************
* FUNCTION: DdeCallBack()    AE: Taken from DDEML sample app and Netscape's NTEST   *
*                                 sample app.                                       *
*                                                                                   *
* PURPOSE:  If m_pConv->fsOptions is not XOPT_DISABLEFIRST, this callback fn will   *
*           be called for many DDE messages passed by the browser.  I think there   *
*           are other flags that can be set in m_pConv to filter some messages out. *
*           One bug is that the "type" parameter passed in when DISCONNECTING is    *
*           different from 16 to 32 bit.  The only case that has been tested is     *
*           the DISCONNECT case for both 16 and 32 bit.  The actual cases were      *
*           taken from NTEST sample app and includes messages that could            *
*           be expected for both server and client.                                 *
*                                                                                   *
* RETURNS:  Not really sure.                                                        *
************************************************************************************/
#ifdef _WIN32
HDDEDATA CALLBACK DdeCallBack(UINT type, UINT fmt,
    HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1,
    DWORD dwData2)  {
#else  
HDDEDATA FAR PASCAL _loadds _export DdeCallBack(
      UINT type,    /* transaction type */
      UINT fmt, /* clipboard data format    */
      HCONV hconv,  /* handle of conversation   */
      HSZ hsz1, /* handle of string */
      HSZ hsz2, /* handle of string */
      HDDEDATA hData,   /* handle of global memory object   */
      DWORD dwData1,    /* transaction-specific data    */
      DWORD dwData2 /* transaction-specific data    */)
{
#endif // _WIN32
#ifdef USING_CALLBACK                  
  CURLProcessor * DdeProcessor = CURLProcessor::GetObjPtr();

    switch(type)    {               //  ReturnType          Receiver    

    case XTYP_CONNECT:  {           //  XCLASS_BOOL         server
        return(DDEFALSE);
    }
    
    case XTYP_CONNECT_CONFIRM:  {   //  XCLASS_NOTIFICATION server
        //  Find the object with the registered service name.
        return(NOTHING);
    }
                      
    
#ifdef _WIN32 
  case 32978:{//AE: case XTYP_DISCONNECT for Win32. Not sure why its different from Win16:
              //XCLASS_NOTIFICATION client/server  
        if(DdeProcessor != NULL)    {
     if(DdeProcessor->m_pConv && (DdeProcessor->m_pConv->hConv == hconv))
      {  
              delete DdeProcessor->m_pConv;
        DdeProcessor->m_pConv = 0;
      }
        }
        return(NOTHING);
    } 
#else
  case XTYP_DISCONNECT: {//this is 16 bit: 32962        //  XCLASS_NOTIFICATION client/server
        if(DdeProcessor != NULL)    {
     if(DdeProcessor->m_pConv && (DdeProcessor->m_pConv->hConv == hconv))
      {  
              delete DdeProcessor->m_pConv;
        DdeProcessor->m_pConv = 0;
      }
        }           
        return(NOTHING);
    }
#endif
    
    case XTYP_POKE: {               //  XCLASS_FLAGS        server
        return(DDE_FNOTPROCESSED);
    }
    
    case XTYP_REQUEST:  {           //  XCLASS_DATA         server
    }  
    }   //  end switch
    
    //  Not handled. 
#endif    
    return(NOTHING);  
}


/**************************************************************************************
* FUNCTION: GetObjPtr()                                                               *
*                                                                                     *
* PURPOSE:  Allows the global call-back fns to get a ptr to the DdeProcessor object.  *
*           This way, we don't need a global object in this module for the callbacks; *
*           they use static fns instead. Caused G.P.F. when compiled for the 286.     *
*                                                                                     *
* RETURNS:  Essentially the "this" pointer of the CURLProcessor object.               *
**************************************************************************************/
#ifdef USING_CALLBACK
  CURLProcessor * CURLProcessor::GetObjPtr()
  { 
    return m_this_obj;
  }                  
#endif
  
CConv::~CConv()
{
  if(hConv)
   { 
//    char note_remove_msg_box;
//    AfxMessageBox("attempting disconnect");
    DdeDisconnect(hConv);
   }
#ifdef WIN32
  if(ret)
   {        
    DdeFreeDataHandle(ret); //AE:5/22/96 causing GPF under win31. Apparently because these
							//are asynchronous transactions, there really isn't a valid data
							//handle returned.
    ret = NULL;
   }
#endif
}

CConv::CConv(DWORD aulTimeOut,WORD awType,WORD awFmt,WORD afsOptions)
{
  ulTimeout = aulTimeOut;
  wType = awType;
  wFmt = awFmt;
  fsOptions = afsOptions;
  Result = 0;
  ret = 0;
  hConv = 0;
}

HCONV CConv::Connect(DWORD idInst,HSZ hszApp,HSZ hszTopic,WORD * Error)
{
  hConv = CreateConv(idInst,hszApp, hszTopic,FALSE,Error);//if last param is not NULL, it will reflect more details on error condition.
  return hConv;
}  

 /*************************************************************************************
* FUNCTION: BringBrowserToForeground()                                                *
*                                                                                     *
* PURPOSE:  Attempts to bring the assigned browser to the foreground. Tries to match  *
*			      the browser's .exe path and name with that in the .ini.                   *
*           If its a 16-bit program running under win95, will need to search for      *
*           m_csBrowserTitlebarName with the the title on the browser's titlebar      *
*		                   														                                    *
* RETURNS:  TRUE if browser was found to be running.                                  *
*			FALSE if browser is not currently running(it should then probably be ran)       *
**************************************************************************************/
BOOL CURLProcessor::BringBrowserToForeground()
{
   CProcessLocater      processLocater;

   return processLocater.Find(m_csBrowserPathAndName);
}

