// TimerThread.cpp : implementation file
//

#include "stdafx.h"
#include "TimerThread.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Static class member initialization
CMapWordToPtr     CTimerThread::sm_thisList;

/////////////////////////////////////////////////////////////////////////////
// CTimerThread

IMPLEMENT_DYNCREATE(CTimerThread, CWinThread)

CTimerThread::CTimerThread()
{
   m_bActiveTimer = FALSE;
   m_bInitialized = FALSE;
   m_hwndNotify = NULL;
   m_bAutoDelete = FALSE;
   m_bStoppingTimer = FALSE;
	m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
   m_bThreadDead = TRUE;
}

CTimerThread::~CTimerThread()
{
	CloseHandle(m_hEventKill);
	CloseHandle(m_hEventDead);
}

void CTimerThread::Init(
   DWORD dwTimeOut, 
   DWORD dwTimerInterval,
   HWND hwndNotify,
   UINT uiMessageID, 
   UINT uiTimerID)
{
   m_uiTimerID = uiTimerID;
   m_hwndNotify = hwndNotify;
   m_uiMessageID = uiMessageID;
   m_dwTimeOut = dwTimeOut;
   m_dwTimerInterval = dwTimerInterval;
   m_pTimerWnd = NULL;
   m_dwStartTime = 0;
   m_dwTimeElapsed = 0;
   m_bInTimerProc = FALSE;
   ASSERT(m_hwndNotify != NULL && m_dwTimeOut != 0 && m_dwTimerInterval != 0);
   if(m_hwndNotify == NULL || m_dwTimeOut == 0 || m_dwTimerInterval == 0)
      return;
   m_enTimerStatus = tsSuccess;
   m_bInitialized = TRUE;
}

BOOL CTimerThread::InitInstance()
{
   if(!m_bInitialized)
   {
      ASSERT(0);
      m_enTimerStatus = tsInitFailed;
      return FALSE;
   }
   m_bThreadDead = FALSE;

   // Create hidden window to receive Socket notifications and Timer Messages
	m_pTimerWnd = new CTimerThreadWnd;
	m_pTimerWnd->m_hWnd = NULL;
	if (!m_pTimerWnd->CreateEx(0, AfxRegisterWndClass(0),
		_T("Timer Thread Notification Window"),
		WS_OVERLAPPED, 0, 0, 0, 0, NULL /* Parent */, NULL))
	{
		TRACE0("Warning: unable to create socket notify window!\n");
      ASSERT(0);
      m_enTimerStatus = tsInitFailed;
      return FALSE;
	}
//	m_pMainWnd = m_pTimerWnd;

   // if timer ID is zero, automatically pick a Timer ID
   if(m_uiTimerID == 0)
   {
      UINT  uiTimerID;
      BOOL  bSuccess = FALSE;
      for(uiTimerID=1;!bSuccess && uiTimerID < 100;uiTimerID++)
      {
         m_uiTimerID = m_pTimerWnd->SetTimer(
            uiTimerID, 
            m_dwTimerInterval, 
            CTimerThread::TimerProc);
         if(m_uiTimerID != 0)
            bSuccess = TRUE;
      }
   }
   else
   {
      m_uiTimerID = m_pTimerWnd->SetTimer(
         m_uiTimerID, 
         m_dwTimerInterval, 
         CTimerThread::TimerProc);
   }
   if(m_uiTimerID == 0)
   {
      ASSERT(0);
      m_enTimerStatus = tsInitFailed;
      return FALSE;
   }
   sm_thisList.SetAt(m_uiTimerID, (LPVOID) this); // Store ID and This pointer
   m_bActiveTimer = TRUE;

   while(WaitForSingleObject(m_hEventKill, 0) == WAIT_TIMEOUT)
   {
      Util::YieldToWindows();
   }

   StopTimer();
   SendMessage(TRUE);

   PostQuitMessage(0);
	
	return TRUE;
}

CTimerThread::TimerStatus CTimerThread::GetStatus()
{
   return m_enTimerStatus;
}

// This Method allows client to kill this thread
void CTimerThread::KillThread()
{
   // Indicate client was cause of termination
   KillThread(tsUserAbort);
}

// Helper functions
int CTimerThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

void CTimerThread::KillThread(TimerStatus enTimerStatus)
{
   SetStatus(enTimerStatus);

   if(m_bThreadDead)
      return;


	// reset the m_hEventKill which signals the thread to shutdown
	VERIFY(SetEvent(m_hEventKill));

	// allow thread to run at higher priority during kill process
	SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);

//	WaitForSingleObject(m_hEventDead, INFINITE);
}

BOOL CTimerThread::StopTimer()
{
   m_bStoppingTimer = TRUE;

   // Stop Timer
   if(m_bActiveTimer && m_uiTimerID != 0)
      m_pTimerWnd->KillTimer(m_uiTimerID);

   // Now Destroy Window
   if(m_pTimerWnd)
   {
      m_pTimerWnd->DestroyWindow();
      delete m_pTimerWnd;
      m_pTimerWnd = NULL;
   }
   sm_thisList.RemoveKey(m_uiTimerID);   // Remove this object from list
   m_bStoppingTimer = FALSE;
   return TRUE;
}

void CTimerThread::Delete()
{
	// calling the base here won't do anything but it is a good habit
	CWinThread::Delete();

	// acknowledge receipt of kill notification
	VERIFY(SetEvent(m_hEventDead));
   m_bThreadDead = TRUE;
}

void CTimerThread::SendMessage(BOOL bSendNow)
{
   if(m_hwndNotify)
   {
//      if(bSendNow)
//        ::SendMessage(m_hwndNotify, m_uiMessageID , GetStatus(), m_dwTimeElapsed);
//      else
        ::PostMessage(m_hwndNotify, m_uiMessageID , GetStatus(), m_dwTimeElapsed);
   }
}

void CTimerThread::SetStatus(TimerStatus enTimerStatus)
{
   m_enTimerStatus = enTimerStatus;
}

void CTimerThread::operator delete(void* p)
{
	// The exiting main application thread waits for this event before completely
	// terminating in order to avoid a false memory leak detection.  See also
	// CBounceWnd::OnNcDestroy in bounce.cpp.

//	SetEvent(m_hEventBounceThreadKilled);

	CWinThread::operator delete(p);
}

void CALLBACK EXPORT CTimerThread::TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwCurTime)
{
   CTimerThread *pThis = NULL;

   // Lookup this pointer for object by Timer ID in CMapWordToPtr list
   if(sm_thisList.Lookup(nIDEvent, (LPVOID &) pThis) == 0)
   {
      // Failed to find Timer ID in thisList
      ASSERT(0);
      ::KillTimer(hWnd, nIDEvent);
      return;
   }

   ASSERT(pThis);

   // If already in timer proc, wait until whatever is going on completes
   if(pThis->m_bInTimerProc || pThis->m_bStoppingTimer)
      return;

   pThis->m_bInTimerProc = TRUE;

   // Make sure window handle is valid before doing anything
   if(pThis->m_pTimerWnd == NULL)
   {
      ASSERT(0);
      pThis->KillThread(tsInternalError);
      return;
   }

   // If no start time has been set, then this must be first call to timer proc
   if(pThis->m_dwStartTime == 0)
   {
      pThis->m_dwStartTime = dwCurTime;
      pThis->m_dwTimeElapsed = pThis->m_dwTimerInterval;
   }
   else
   {
      // Compute time elapsed
      pThis->m_dwTimeElapsed = dwCurTime - pThis->m_dwStartTime + pThis->m_dwTimerInterval;
   }

#if 0
   // Check if we should start checking for internet connection
   if(pThis->m_dwTimeElapsed < pThis->m_dwFirstCheck)
   {
      pThis->m_bInTimerProc = FALSE;
      return;                 // bail, not time yet!
   }
#endif

   // Check if Timed Out
   if(pThis->m_dwTimeElapsed > pThis->m_dwTimeOut)
   {
      pThis->KillThread(tsTimerExpired);
      return;
   }

   pThis->SetStatus(tsSuccess);   // Signal Time Period
   pThis->SendMessage();

   pThis->m_bInTimerProc = FALSE;
}

BEGIN_MESSAGE_MAP(CTimerThread, CWinThread)
	//{{AFX_MSG_MAP(CTimerThread)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerThread message handlers

BOOL CTimerThread::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinThread::OnIdle(lCount);
}

/////////////////////////////////////////////////////////////////////////////
// CTimerThreadWndimplementation

CTimerThreadWnd::CTimerThreadWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// Message table implementation

BEGIN_MESSAGE_MAP(CTimerThreadWnd, CWnd)
	//{{AFX_MSG_MAP(CWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


