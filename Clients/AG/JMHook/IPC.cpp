#include <tchar.h>
//#include <crtdbg.h>
#include "StdAfx.h"
#include "IPC.h"

//***********************************************
CIPC::CIPC(LPCTSTR szName) : m_hFileMap(NULL), m_hMutex(NULL), m_szMMF(szName)
{
	// Create unique name for mutex
	m_szMutex = m_szMMF + _T("Mutex");
}

//***********************************************
CIPC::~CIPC()
{
	CloseIPCMMF();
	Unlock();
}

//***********************************************
bool CIPC::CreateIPCMMF()
{
	bool bCreated = false;

	try
	{
		if(m_hFileMap != NULL)
			return false;	// Already created

		// Create an in-memory 4KB memory mapped file to share data
		m_hFileMap = ::CreateFileMapping((HANDLE)0xFFFFFFFF,
			NULL,
			PAGE_READWRITE,
			0,
			4096,
			m_szMMF);
		if(m_hFileMap != NULL)
		{
			bCreated = true;
		}
	}
	catch(...) {}

	return bCreated;
}

//***********************************************
bool CIPC::OpenIPCMMF(void)
{
	bool bOpened = false;

	try
	{
		if(m_hFileMap != NULL)
			return true;	// Already opened

		m_hFileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
			FALSE,
			m_szMMF);
		if(m_hFileMap != NULL)
			bOpened = true;
	}
	catch(...) {}

	return bOpened;
}

//***********************************************
void CIPC::CloseIPCMMF(void)
{
	try
	{
		if(m_hFileMap != NULL)
			CloseHandle(m_hFileMap), m_hFileMap = NULL;
	}
	catch(...) {}
}

//***********************************************
bool CIPC::ReadIPCMMF(LPBYTE pBuf, DWORD &dwBufSize)
{
	ATLASSERT(pBuf);

	bool bSuccess = true;

	try
	{
		if(m_hFileMap == NULL)
			return false;

		DWORD dwBaseMMF = (DWORD)MapViewOfFile(m_hFileMap,
			FILE_MAP_READ | FILE_MAP_WRITE,
			0, 0, 0);
		ATLASSERT(dwBaseMMF);

		// The first DWORD in the MMF contains the size of the data
		DWORD dwSizeofInBuf = dwBufSize;
		CopyMemory(&dwBufSize, (LPVOID)dwBaseMMF, sizeof(DWORD));

		if(dwSizeofInBuf != 0)
		{
			if(dwBufSize > dwSizeofInBuf)
				bSuccess = false;
			else
				CopyMemory(pBuf, (LPVOID)(dwBaseMMF + sizeof(DWORD)), dwBufSize);
		}

		UnmapViewOfFile((LPVOID)dwBaseMMF);
	}
	catch(...) {}

	return bSuccess;
}

//***********************************************
bool CIPC::WriteIPCMMF(const LPBYTE pBuf, const DWORD dwBufSize)
{
	ATLASSERT(pBuf);

	bool bSuccess = true;

	try
	{
		if(m_hFileMap == NULL)
			return false;

		DWORD dwBaseMMF = (DWORD)MapViewOfFile(m_hFileMap,
			FILE_MAP_READ | FILE_MAP_WRITE,
			0, 0, 0);
		ATLASSERT(dwBaseMMF);

		// The first DWORD in the MMF contains the size of the data
		CopyMemory((LPVOID)dwBaseMMF, &dwBufSize, sizeof(DWORD));
		CopyMemory((LPVOID)(dwBaseMMF + sizeof(DWORD)), pBuf, dwBufSize);

		UnmapViewOfFile((LPVOID)dwBaseMMF);
	}
	catch(...) {}

	return bSuccess;
}

//***********************************************
bool CIPC::Lock(void)
{
	bool bLocked = false;

	try
	{
		// First get the handle to the mutex
		m_hMutex = CreateMutex(NULL, FALSE, m_szMutex);
		if(m_hMutex != NULL)
		{
			// Wait to get the lock on the mutex
			if(WaitForSingleObject(m_hMutex, INFINITE) == WAIT_OBJECT_0)
				bLocked = true;
		}
	}
	catch(...) {}

	return bLocked;
}

//***********************************************
void CIPC::Unlock(void)
{
	try
	{
		if(m_hMutex != NULL)
		{
			ReleaseMutex(m_hMutex);
			CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
	}
	catch(...) {}
}
