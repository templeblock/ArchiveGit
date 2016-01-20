#if !defined(AFX_SOCKETHELPERTHREAD_H__BE83A541_546E_11D2_9298_00207812C40C__INCLUDED_)
#define AFX_SOCKETHELPERTHREAD_H__BE83A541_546E_11D2_9298_00207812C40C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SocketHelperThread.h : header file
//

#include <AfxSock.h>


/////////////////////////////////////////////////////////////////////////////
// CSocketHelperThread thread

class CSocketHelperThread : public CWinThread
{
	DECLARE_DYNCREATE(CSocketHelperThread)
protected:

// Attributes
public:

// Operations
public:
	CSocketHelperThread(HWND hwndToNotify=NULL, UINT uiMessageID=0);
	virtual ~CSocketHelperThread();
   BOOL IsConnected()
   {
      return m_bConnected;
   }
   void SetSocket(SOCKET hSocket)
   {
      m_hSocket = hSocket;
   }
   int KillThread();
   HANDLE   GetEventDead()
   {
      return m_hEventDead;
   }
   virtual void Delete();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketHelperThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSocketHelperThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
// Data members
protected:
   BOOL     m_bConnected;
   SOCKET   m_hSocket;
	HANDLE   m_hEventKill;
   HANDLE   m_hEventDead;
   HANDLE   m_hEventExitingGracefully;
   int      m_nError;
   HWND     m_hwndClient;
   UINT     m_uiMessageID;
   #ifdef   USE_ASYN_CCONNECT
      CAsyncSocket   m_socketMain;
   #else
      CSocket        m_socketMain;
   #endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKETHELPERTHREAD_H__BE83A541_546E_11D2_9298_00207812C40C__INCLUDED_)
