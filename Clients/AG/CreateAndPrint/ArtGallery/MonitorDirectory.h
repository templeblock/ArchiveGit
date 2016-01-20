#pragma once

class CMonitorDirectory
{
public:
	CMonitorDirectory(HWND hWndParent, UINT idMessage);
	~CMonitorDirectory();
	static DWORD WINAPI MonitorDirectory(void* pWnd);

protected:
	HANDLE m_hMonitorThread;
	HANDLE m_hMonitorThreadExitEvent;
};
