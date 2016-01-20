/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                initthrd.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "ppafx.h"
#include <process.h>
#include "twain.h"
#include "dca_glue.h"
#include "dca_acq.h"
#include "custools.h"
#include "initthrd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern HINSTANCE	hInstHintLib;   
extern char szAppName[MAX_STR_LEN];

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */
/* ********************************** Macros ******************************** */
/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */
/* *************************** Private Functions **************************** */
/* *************************** Private Methods **************************** */
/* ***************************** Public Methods ***************************** */

/////////////////////////////////////////////////////////////////////////////
// CInitializationThread

IMPLEMENT_DYNAMIC(CInitializationThread, CWinThread)

CInitializationThread::CInitializationThread(CPictPubApp *pPPApp)
{
    ASSERT(pPPApp);
    ASSERT(pPPApp->m_pMainWnd);
    m_pPictPubApp = pPPApp;
	m_pMainWnd = pPPApp->m_pMainWnd;

    m_bAutoDelete = FALSE;

    m_hSetupAppMainMenuEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hSetupAppSharedMenuEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hHintsLoadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hCMSEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hTWAINEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hInitThreadDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    m_bSetupAppMainMenuEvent  = FALSE;
    m_bSetupAppSharedMenuEvent  = FALSE;
    m_bHintsLoadEvent  = FALSE;
    m_bCMSEvent  = FALSE;
    m_bTWAINEvent  = FALSE;
    m_bInitThreadDoneEvent = FALSE;
}

CInitializationThread::~CInitializationThread()
{
	CloseHandle(m_hSetupAppMainMenuEvent);
	CloseHandle(m_hSetupAppSharedMenuEvent);
	CloseHandle(m_hHintsLoadEvent);
	CloseHandle(m_hCMSEvent);
	CloseHandle(m_hTWAINEvent);
	CloseHandle(m_hInitThreadDoneEvent);
}

BOOL CInitializationThread::InitInstance()
{
    HWND hWnd = m_pMainWnd->GetSafeHwnd();
    FNAME szFileName;
    BOOL bRunningEmbedded = PictPubApp.RunEmbedded();
	BOOL  bRunningAutomated = PictPubApp.RunAutomated();

    lstrcpy(szFileName, Control.ProgHome);
    lstrcat(szFileName, "string.dll");

    // add and/or delete items to menus ( any one-time ( permanent ) menu changes )
    // setup IDR_PPMAIN
    m_pPictPubApp->SetupAppMainMenu( CMenu::FromHandle( GetMenu( hWnd)));
    VERIFY(SetEvent(m_hSetupAppMainMenuEvent));

    // setup IDR_PPSVRTYPE_PP
    POSITION TemplatePos = m_pPictPubApp->GetFirstDocTemplate();
    if (TemplatePos)
    {
        CPPMultiDocTemplate *pTemplate = (CPPMultiDocTemplate *)m_pPictPubApp->GetNextDocTemplate (TemplatePos);
    	m_pPictPubApp->SetupAppSharedMenu(CMenu::FromHandle(pTemplate->m_hMenuShared));
        VERIFY(SetEvent(m_hSetupAppSharedMenuEvent));
    }
                    
    lstrcpy(szFileName, Control.ProgHome);
    lstrcat(szFileName, "hint.dll");
    if( ( hInstHintLib = (HINSTANCE)AstralLoadLibrary( szFileName )))
    {
        if( Hints.fStatusHintsOn )
            MessageStatus( IDS_WELCOME, (LPSTR)szAppName );
        VERIFY(SetEvent(m_hHintsLoadEvent));
    }
    else
        //ERROR CONDITION !! 
        ;

    /* This is TWAIN Stuff */
    if (!bRunningEmbedded)
    {
        TW_IDENTITY identity;
    
        identity.Id = (TW_UINT32)hWnd;
        identity.Version.MajorNum = 3;
        identity.Version.MinorNum = 1;
        identity.Version.Language = TWLG_USA;
        identity.Version.Country  = TWCY_USA;
        lstrcpy(identity.Version.Info,  "3.1 Beta Release");
    
        identity.ProtocolMajor    = TWON_PROTOCOLMAJOR;
        identity.ProtocolMinor    = TWON_PROTOCOLMINOR;
        identity.SupportedGroups  = DG_CONTROL | DG_IMAGE;
        lstrcpy(identity.Manufacturer,  "MicroGrafX Inc.");
        lstrcpy(identity.ProductFamily, "Windows Apps");
        lstrcpy(identity.ProductName,   szAppName );
        
        DCInitialize( &identity, hWnd );
    } /* End of TWAIN Stuff */
    VERIFY(SetEvent(m_hTWAINEvent));

    // Finished Initialization !
    VERIFY(SetEvent(m_hInitThreadDoneEvent));

	// avoid entering standard message loop by returning FALSE
	return FALSE;
}

BOOL CInitializationThread::StartThread( )
{
	m_pThreadParams = NULL;

	// Create Thread in a suspened state so we can set the Priority 
	// before it starts gets away from us
	if (!CreateThread(CREATE_SUSPENDED))
	{
		Delete();
		return FALSE;
	}

	VERIFY(SetThreadPriority(THREAD_PRIORITY_NORMAL));
	// Now the thread can run wild
	ResumeThread();
    
    return TRUE;
}


BOOL CInitializationThread::SetupAppMainMenuComplete(BOOL bWaitInfinite) 
{
    if (!m_bSetupAppMainMenuEvent)
    {
        m_bSetupAppMainMenuEvent = 
            (WaitForSingleObject(m_hSetupAppMainMenuEvent, (bWaitInfinite?INFINITE:0)) == WAIT_OBJECT_0);
    }
    return m_bSetupAppMainMenuEvent;
}

BOOL CInitializationThread::SetupAppSharedMenuComplete(BOOL bWaitInfinite) 
{
    if (!m_bSetupAppSharedMenuEvent)
    {
        m_bSetupAppSharedMenuEvent = 
            (WaitForSingleObject(m_hSetupAppSharedMenuEvent, (bWaitInfinite?INFINITE:0)) == WAIT_OBJECT_0);
    }
    return m_bSetupAppSharedMenuEvent;
}

BOOL CInitializationThread::HintLoadComplete(BOOL bWaitInfinite) 
{
    if (!m_bHintsLoadEvent)
    {
        m_bHintsLoadEvent = 
            (WaitForSingleObject(m_hHintsLoadEvent, (bWaitInfinite?INFINITE:0)) == WAIT_OBJECT_0);
    }
    return m_bHintsLoadEvent;
}

BOOL CInitializationThread::CMSInitComplete(BOOL bWaitInfinite) 
{
    if (!m_bCMSEvent)
    {
        m_bCMSEvent =
            (WaitForSingleObject(m_hCMSEvent, (bWaitInfinite?INFINITE:0)) == WAIT_OBJECT_0);
    }
    return m_bCMSEvent;
}

BOOL CInitializationThread::TWAINInitComplete(BOOL bWaitInfinite) 
{
    if (!m_bTWAINEvent)
    {
        m_bTWAINEvent =
            (WaitForSingleObject(m_hTWAINEvent, (bWaitInfinite?INFINITE:0)) == WAIT_OBJECT_0);
    }
    return m_bTWAINEvent;
}

BOOL CInitializationThread::InitThreadComplete(BOOL bWaitInfinite) 
{
    if (!m_bInitThreadDoneEvent)
    {
        m_bInitThreadDoneEvent =
            (WaitForSingleObject(m_hInitThreadDoneEvent, (bWaitInfinite?INFINITE:0)) == WAIT_OBJECT_0);
    }
    return m_bInitThreadDoneEvent;
}
