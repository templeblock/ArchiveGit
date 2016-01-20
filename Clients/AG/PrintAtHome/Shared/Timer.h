#pragma once

#include "mmsystem.h"
#pragma comment(lib, "winmm.lib") // for timeSetEvent and timeKillEvent

class CTimer
{
private:
	const static int TIMER_RESOLUTION = 33; // 1/30 of a second
public:
	//////////////////////////////////////////////////////////////////////////////
	CTimer()
	{
		m_wTimerID = 0;
		m_bOneShot = false;
	}

	//////////////////////////////////////////////////////////////////////////////
	~CTimer()
	{
		Stop();
	}

	//////////////////////////////////////////////////////////////////////////////
	bool Start(UINT msInterval, bool bOneShot, LPTIMECALLBACK pTimerProc, int iCountdown, DWORD dwData1, DWORD dwData2 = NULL, DWORD dwData3 = NULL)
	{ 
		if (m_wTimerID)
			Stop();

		m_iCountdown = iCountdown;
		m_dwData1 = dwData1;
		m_dwData2 = dwData2;
		m_dwData3 = dwData3;

		UINT uTimerEvent = (bOneShot ? TIME_ONESHOT : TIME_PERIODIC);
		m_wTimerID = ::timeSetEvent(msInterval, TIMER_RESOLUTION, pTimerProc, (DWORD)this, uTimerEvent);
		return m_wTimerID != NULL;
	}

	//////////////////////////////////////////////////////////////////////////////
	bool Stop()
	{
		if (!m_wTimerID)
			return true;

		UINT wTimerID = m_wTimerID;
		m_wTimerID = 0;
		if (!m_bOneShot)
			return ::timeKillEvent(wTimerID) != TIMERR_NOERROR;
		return true;
	}
	
	//////////////////////////////////////////////////////////////////////////////
	int GetCountdown()
	{
		return --m_iCountdown;
	}

	//////////////////////////////////////////////////////////////////////////////
	DWORD GetData1()
	{
		return m_dwData1;
	}

	//////////////////////////////////////////////////////////////////////////////
	DWORD GetData2()
	{
		return m_dwData2;
	}

	//////////////////////////////////////////////////////////////////////////////
	DWORD GetData3()
	{
		return m_dwData3;
	}

private:
	UINT m_wTimerID;
	DWORD m_dwData1;
	DWORD m_dwData2;
	DWORD m_dwData3;
	bool m_bOneShot;
	int m_iCountdown;
};
