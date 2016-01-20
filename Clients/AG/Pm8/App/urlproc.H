/******************************************************************************
*  FILE:        URLPROC.H                                                     *
*  SUMMARY:     Class for sending URL commands to web browsers.               *
*  AUTHOR:      Aaron Evans (AE)                                              *
*  ---------------------------------------------------------------------------*
*  Initials Legend:                                                           *
*     AE    Aaron Evans                                                       *
*  ---------------------------------------------------------------------------*
*  Revision History                                                           *
*                                                                             *
*  Date     Init  Description of Change                                       *
*  ---------------------------------------------------------------------------*
* 12/24/97  DGP   Replaced Pietrek code with more generic code for locating   *
*                 processes under Windows 95 and NT.                          *
* 04-08-96  AE    Created                                                     *
*                                                                             *
******************************************************************************/
#ifndef _URLPROC_INC
#define _URLPROC_INC

#include <ddeml.h>
#ifdef WIN32
#include "tlhelp32.h"
#endif
/*****************************************************************
Optional Defines: (used in #ifdef statements)
USING_CALLBACK -- enables logic in call-back fn.  To enable this properly,
		also take a look at XOPT_CLOSECONV below.  GetObjectPtr() in the call-back
		was causing G.P.F. when compiled for 286 processors.
*******************************************************************/



// transaction option flags - for fsOptions field and DefOptions global.
// AE: some of these may not be used.  The one's I'm sure of 
// have comments next to them.

#define XOPT_NODATA             0x0001
#define XOPT_ACKREQ             0x0002
#define XOPT_DISABLEFIRST       0x0004 //as client, disables XTYP_ADVDATA and XTYP_XACT_COMPLETE transactions.
#define XOPT_ABANDONAFTERSTART  0x0008 //if Async, calls DdeAbandonTrans(...). Not sure what the advantage is, but the system seems to respond faster.  
#define XOPT_BLOCKRESULT        0x0010
#define XOPT_ASYNC              0x0020 //Asynchronous transactions.
#define XOPT_CLOSECONV          0x0040 //Do not keep conv open; close it after passing URL string.
#define XOPT_COMPLETED          0x8000      // used internally only.


#define DDEFALSE ((HDDEDATA)FALSE)
#define NOTHING ((HDDEDATA)NULL)

 
enum URLProcessorRetCode
   {
      enURLErrorNone,
      enURLErrorNoDefaultBrowser,   // No .EXE associated with .HTM(L) extension
      enURLErrorInit,               // Initialization failed
      enURLErrorDdeConnect,
      enURLErrorDdeStringHandle,
      enURLErrorDdeProcessTrans,
      enURLErrorNotConfigured,      // Non-standard default browser
      enURLBrowserNotFound          // Failed to find running browser
   };

/*****************************************************************************
* CLASS:    CConv                                                            *
*                                                                            *
* PURPOSE:  It is an open DDE conversation.  Handles all transactions        *
*           of the conversation.  Delete the class to close the conversation *
*                                                                            *
* AUTHOR:   Aaron J. Evans (situs@sirius.com).  3/27/96                      *                                                                             *
*****************************************************************************/
class CConv
{    
public:
    CConv(DWORD ulTimeOut,WORD wType,WORD wFmt,WORD fsOptions);
    ~CConv();
    BOOL Connected();
    HCONV Connect(DWORD idInst,HSZ hszApp,HSZ hszTopic,WORD * Error);
    URLProcessorRetCode ProcessTransaction(DWORD idInst,HSZ hszItem);
    WORD fsOptions;
private:
//functions
    HCONV CreateConv(DWORD idInst,HSZ hszApp,HSZ hszTopic,BOOL fList,WORD *pError);
//data
    HDDEDATA ret;   
    DWORD Result;
    DWORD ulTimeout;
    WORD wType;
    HCONV hConv;
    WORD wFmt;

//so that callbacks can access private data  
friend BOOL CALLBACK EnumWindowsProc(HWND  hwnd,    LPARAM  lParam);
friend 
#ifdef _WIN32
HDDEDATA CALLBACK DdeCallBack(UINT,UINT,HCONV,HSZ,HSZ,HDDEDATA,DWORD,DWORD);
#else
HDDEDATA FAR PASCAL _loadds _export DdeCallBack(UINT,UINT,HCONV,HSZ,HSZ,HDDEDATA,DWORD,DWORD);
#endif    
};


/*****************************************************************************
* CLASS:    CURLProcessor                                                    *
*                                                                            *
* PURPOSE:  Processes DDE conversations with goal of passsing URL strings    *
*           to the Netscape, Mosaic, and MS-Explorer web browsers.           *
*                                                                            *
* AUTHOR:   Aaron J. Evans (situs@sirius.com).  3/27/96                      *                                                                             *
*****************************************************************************/
class CURLProcessor
{
public:
  CURLProcessor();
  ~CURLProcessor();
//  static CURLProcessor FAR * GetObjPtr();//{return this_obj;}
  URLProcessorRetCode OpenURL(CString,BOOL bIsWebSearch = FALSE);

private:
  URLProcessorRetCode DoOpenURL(CString,BOOL bIsWebSearch = FALSE);

protected:
//functions:
  virtual URLProcessorRetCode Initialize();
  unsigned char * GetTextData(HDDEDATA);
  HWND FAR PASCAL AddConv(HSZ hszApp,HSZ hszTopic,HCONV hConv,BOOL fList);
  void GetWin95Filename( DWORD dwProcessId,char * lpstr );
  void GetWin95Index( char *pszCounter, char *szIndex );
 
protected:
//data  
  CConv * m_pConv;
  DWORD m_idInst;
  BOOL m_bInitialized;
  CString m_csBrowserDDEID;
  CString m_csWebSearchSpacer;//parameter that replaces spaces for web search.
  FARPROC m_lpDdeProc;
  static CURLProcessor FAR * m_this_obj;//needed because this is not accessable in static functions. 
  //static CString m_BrowserPath;//entire path to browser 
  CString m_csBrowserPathAndName;//complete path name including .exe
  CString m_csBrowserName;//executable name only
  CString m_csBrowserPath;//path w/o the .exe name
  CString m_csBrowserTitlebarName;//identifier to look for on titlebar needed when 16-bit browser is ran under win95
  CString m_csURL;

  virtual BOOL BringBrowserToForeground();

//so that callbacks can access private data  
friend BOOL CALLBACK EnumWindowsProc(HWND  hwnd,    LPARAM  lParam);
friend 
#ifdef _WIN32
HDDEDATA CALLBACK DdeCallBack(UINT,UINT,HCONV,HSZ,HSZ,HDDEDATA,DWORD,DWORD);
#else
HDDEDATA FAR PASCAL _loadds _export DdeCallBack(UINT,UINT,HCONV,HSZ,HSZ,HDDEDATA,DWORD,DWORD);
#endif  
};

//callback fns.
#ifdef USING_CALLBACK
#ifdef _WIN32
HDDEDATA CALLBACK DdeCallBack(UINT type, UINT fmt,
    HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1,
    DWORD dwData2);
#else
HDDEDATA FAR PASCAL _loadds _export DdeCallBack(
      UINT type,    /* transaction type */
      UINT fmt, /* clipboard data format    */
      HCONV hconv,  /* handle of conversation   */
      HSZ hsz1, /* handle of string */
      HSZ hsz2, /* handle of string */
      HDDEDATA hData,   /* handle of global memory object   */
      DWORD dwData1,    /* transaction-specific data    */
      DWORD dwData2 /* transaction-specific data    */);
#endif
#endif

#endif
