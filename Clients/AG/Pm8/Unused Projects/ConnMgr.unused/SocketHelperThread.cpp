// SocketHelperThread.cpp : implementation file
//

#include "stdafx.h"
#include "SocketHelperThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSocketHelperThread

IMPLEMENT_DYNCREATE(CSocketHelperThread, CWinThread)

CSocketHelperThread::CSocketHelperThread(HWND hwndClient, UINT uiMessageID)
{
	m_hEventExitingGracefully = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
   m_bConnected = FALSE;
   m_bAutoDelete = FALSE;
   m_nError = 0;
   m_hwndClient = hwndClient;
   m_uiMessageID = uiMessageID;
   m_hSocket = NULL;
}

CSocketHelperThread::~CSocketHelperThread()
{
   CloseHandle(m_hEventExitingGracefully);
   CloseHandle(m_hEventKill);
   CloseHandle(m_hEventDead);
}

BOOL CSocketHelperThread::InitInstance()
{
   m_nError = 0;
   // TODO - check for valid init.
   ASSERT(m_hSocket);
   m_socketMain.Attach(m_hSocket);
   m_bConnected = m_socketMain.Connect("www.yahoo.com", 80);
   
   LONG lParam = MAKELPARAM(0, !m_bConnected);
   if(m_hwndClient)
      ::PostMessage(m_hwndClient, m_uiMessageID,0,lParam);

   // Signal that thread exited gracefully
   SetEvent(m_hEventExitingGracefully);
   if(!m_bConnected)
      m_nError = m_socketMain.GetLastError();
   m_socketMain.Detach();

   WaitForSingleObject(m_hEventKill,INFINITE);

   PostQuitMessage(0);
   // Return FALSE to exit thread and avoid message processing
   return TRUE;
}

int CSocketHelperThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

void CSocketHelperThread::Delete()
{
	// calling the base here won't do anything but it is a good habit
	CWinThread::Delete();

	// acknowledge receipt of kill notification
	VERIFY(SetEvent(m_hEventDead));
}

int CSocketHelperThread::KillThread()
{
   WaitForSingleObject(m_hEventExitingGracefully, INFINITE);

   SetEvent(m_hEventKill);
   return 0;
}

BEGIN_MESSAGE_MAP(CSocketHelperThread, CWinThread)
	//{{AFX_MSG_MAP(CSocketHelperThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSocketHelperThread message handlers

BOOL CSocketHelperThread::OnIdle(LONG lCount) 
{
	return CWinThread::OnIdle(lCount);
}
