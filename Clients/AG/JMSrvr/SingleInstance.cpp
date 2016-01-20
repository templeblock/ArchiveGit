#include "stdafx.h"
#include "SingleInstance.h"

//***********************************************
CSingleInstance::CSingleInstance() : m_hMutex(NULL)
{
}

//***********************************************
CSingleInstance::~CSingleInstance()
{
	if(m_hMutex != NULL)
	{
		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
	}
}

//***********************************************
bool CSingleInstance::Create(LPCTSTR szMutexName)
{
	_ASSERTE(szMutexName);
	_ASSERTE(lstrlen(szMutexName));

	bool bSuccess = false;

	try
	{
		// First get the handle to the mutex
		m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
		if(m_hMutex != NULL)
		{
			// Test the state of the mutex
			// If the state is signaled, we successfully opened the mutex
			//if (MsgWaitForMultipleObjects(1, &m_hMutex, FALSE, 0, QS_ALLEVENTS) == WAIT_OBJECT_0)
			if(WaitForSingleObject(m_hMutex, 0) == WAIT_OBJECT_0)
				bSuccess = true;
		}
	}
	catch(...) {}

	return bSuccess;
}
