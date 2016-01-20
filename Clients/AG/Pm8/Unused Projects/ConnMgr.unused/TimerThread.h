#if !defined(AFX_TIMERTHREAD_H__B5F0B243_583F_11D2_9298_00207812C40C__INCLUDED_)
#define AFX_TIMERTHREAD_H__B5F0B243_583F_11D2_9298_00207812C40C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimerThread.h : header file
//
class CTimerThreadWnd;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CTimerThread thread

class CTimerThread : public CWinThread
{
	DECLARE_DYNCREATE(CTimerThread)
public:
	CTimerThread();           // protected constructor used by dynamic creation
	virtual ~CTimerThread();

// Attributes
public:

// Operations
public:
   enum TimerStatus
   {
      tsSuccess,
      tsTimerExpired,
      tsUserAbort,
      tsInitFailed,
      tsInternalError
   };

   void        Init(DWORD dwTimeOut, DWORD dwTimerInterval, HWND hwndNotify, UINT uiMessageID, UINT uiTimerID=0);
   TimerStatus GetStatus();
   void        KillThread(void);
   HANDLE      GetEventDead();
   void operator delete(void* p);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimerThread)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// Helpers
protected:
   void KillThread(TimerStatus enTimerStatus);
   BOOL StopTimer(void);
   void SetStatus(TimerStatus enTimerStatus);
   void SendMessage(BOOL bSendNow=FALSE);
   virtual void Delete(void);


// Static Methods
static void CALLBACK EXPORT TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwCurTime);


// Data Members
protected:
   UINT        m_uiTimerID;
   UINT        m_uiMessageID;

   HWND        m_hwndNotify;
   CTimerThreadWnd *m_pTimerWnd;     // Window that owns timer
   BOOL        m_bActiveTimer;  // Is Timer Active?
   BOOL        m_bInTimerProc;
   DWORD       m_dwStartTime;   // Time 1st call to timer occurred
   DWORD       m_dwTimeOut;     // when to TimeOut connect attempt
   DWORD       m_dwTimerInterval; // How often to check connection
   DWORD       m_dwTimeElapsed; // How much time has elapsed since
   BOOL        m_bInitialized;   
   BOOL        m_bStoppingTimer;
   BOOL        m_bThreadDead;
   TimerStatus m_enTimerStatus;
   HANDLE      m_hEventDead;
   HANDLE      m_hEventKill;

// Static Data Members
   static CMapWordToPtr sm_thisList;
};

class CTimerThreadWnd : public CWnd
{
// Construction
public:
	CTimerThreadWnd();

protected:
	//{{AFX_MSG(CMySocketWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Helpers
protected:

// Data Members
protected:
};

inline HANDLE CTimerThread::GetEventDead()
{
   return m_hEventDead;
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMERTHREAD_H__B5F0B243_583F_11D2_9298_00207812C40C__INCLUDED_)
