#include "stdafx.h"
#include "MonitorDirectory.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define TERMINATE_EVENT 0
#define MAX_EVENTS 10
struct MONITOR_STRUCT
{
	HWND hMainWnd;
	UINT idMessage;
	HANDLE hEvents[MAX_EVENTS];
	int nEvents;
};

/////////////////////////////////////////////////////////////////////////////
CMonitorDirectory::CMonitorDirectory(HWND hWndParent, UINT idMessage)
{
	m_hMonitorThread = NULL;
	m_hMonitorThreadExitEvent = ::CreateEvent(NULL/*pEventAttributes*/, false/*bManualReset*/, false/*bInitialState*/, NULL/*lpName*/);
	if (m_hMonitorThreadExitEvent <= 0)
	{
		m_hMonitorThreadExitEvent = NULL;
		return;
	}

	MONITOR_STRUCT Monitor;
	Monitor.hMainWnd = hWndParent;
	Monitor.idMessage = idMessage;
	Monitor.hEvents[TERMINATE_EVENT] = m_hMonitorThreadExitEvent;
	Monitor.nEvents = 1;

	char szTempPath[MAX_PATH];
	::GetTempPath(sizeof(szTempPath), szTempPath);
	CString szPath = CString(szTempPath) + CString(_T("AgDownloads\\"));
	::CreateDirectory(szPath, NULL);

	HANDLE hEventResult = ::FindFirstChangeNotification(szPath, false, FILE_NOTIFY_CHANGE_SIZE /*FILE_NOTIFY_CHANGE_FILE_NAME*/);
	if (hEventResult > 0)
	{
		Monitor.hEvents[Monitor.nEvents++] = hEventResult;
	}

	MONITOR_STRUCT* pMonitor = new MONITOR_STRUCT;
	if (pMonitor)
	{
		// Make a copy of the Monitor structure on the heap, and pass it the new thread
		// MonitorDirectory will delete it after it makes its own copy
		*pMonitor = Monitor;
		DWORD tid;
		m_hMonitorThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MonitorDirectory, pMonitor, 0, &tid);
	}
}

/////////////////////////////////////////////////////////////////////////////
CMonitorDirectory::~CMonitorDirectory() 
{
	if (m_hMonitorThreadExitEvent)
	{
		bool bSet = !!::SetEvent(m_hMonitorThreadExitEvent);
		m_hMonitorThreadExitEvent = NULL;
	}

	if (m_hMonitorThread)
	{
		DWORD dwWaitState = WaitForSingleObject(m_hMonitorThread, 5000);
		::CloseHandle(m_hMonitorThread);
	}
}

/////////////////////////////////////////////////////////////////////////////
DWORD WINAPI CMonitorDirectory::MonitorDirectory(void* pMonitorDirectory)
{
	// Make a copy of the structure passed from the main thread, and delete it right away
	MONITOR_STRUCT Monitor = *(MONITOR_STRUCT*)pMonitorDirectory;
	delete pMonitorDirectory;

	int nLastEvent = Monitor.nEvents - 1;
	DWORD dwFirstObject = WAIT_OBJECT_0 + 1;
	DWORD dwLastObject = WAIT_OBJECT_0 + nLastEvent;
	DWORD dwTerminate = WAIT_OBJECT_0 + TERMINATE_EVENT;
	DWORD dwFirstAbandoned = WAIT_ABANDONED_0;
	DWORD dwLastAbandoned = WAIT_ABANDONED_0 + nLastEvent;
	do
	{
		DWORD dwWaitObject = ::WaitForMultipleObjects(Monitor.nEvents, Monitor.hEvents, false, INFINITE);
		if (dwWaitObject == dwTerminate || dwWaitObject == WAIT_FAILED)
			break;

		if (dwWaitObject >= dwFirstAbandoned && dwWaitObject <= dwLastAbandoned)
			break;

		if (dwWaitObject >= dwFirstObject && dwWaitObject <= dwLastObject)
		{
			::SendMessage(Monitor.hMainWnd, WM_COMMAND, Monitor.idMessage, 0);
			::FindNextChangeNotification(Monitor.hEvents[1]);
		}
	}
	while (true);

	::CloseHandle(Monitor.hEvents[TERMINATE_EVENT]);
	for (int nEvents = 1; nEvents < Monitor.nEvents; nEvents++)
		::FindCloseChangeNotification(Monitor.hEvents[nEvents]);

	return true;
}
