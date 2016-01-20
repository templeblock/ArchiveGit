#include "stdafx.h"
#include "Download.h"
#include "Plugin.h"
#include <strstream>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

/////////////////////////////////////////////////////////////////////////////
CDownload::CDownload(CPlugin* pPlugin)
{
	m_pPlugin = pPlugin;

	m_iFileType = -1;
	m_List.clear();
	m_pData = NULL;
	m_dwSize = 0;
	m_fnCallback = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CDownload::~CDownload()
{
	Free();
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::Free()
{
	m_iFileType = -1;
	m_List.clear();

	if (m_pData)
		free(m_pData);

	m_pData = NULL;
	m_dwSize = 0;
	m_fnCallback = NULL;
}

/////////////////////////////////////////////////////////////////////////////
bool CDownload::Init(int iFileType, FNDOWNLOADCALLBACK fnCallback)
{
	if (InProgress())
	{
		if (::MessageBox(m_pPlugin->m_hWnd, "Download already in progress.\n\nDo you want to abort the current download?", "", MB_YESNO) == IDNO)
			return false;
	}
	
	Free();

	m_iFileType = iFileType;
	m_fnCallback = fnCallback;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::AddFile(CString& strSrcURL, CString& strDstPath, int iData)
{
	DOWNLOAD d;
	d.strSrcURL = strSrcURL;
	d.strDstPath = strDstPath;
	d.iData = iData;
	d.pData = NULL;
	d.dwSize = 0;
	m_List.push_back(d);
}

/////////////////////////////////////////////////////////////////////////////
bool CDownload::Start()
{
	if (Next())
		return true;

	::MessageBox(m_pPlugin->m_hWnd, "Error starting the download.  Please try again", "", MB_OK);
	Free();
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CDownload::Next()
{
	if (m_List.empty())
		return false;

	DOWNLOAD& Download = m_List.front();
	CString strSrcURL = Download.strSrcURL;
	if (strSrcURL.GetLength() <= 0)
		return false;

	NPN_GetURL(m_pPlugin->m_pNPPInstance, strSrcURL, NULL);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::FileData(BYTE* pBytes, DWORD dwLen)
{
	if (!InProgress())
		return;
	if (!pBytes || !dwLen)
		return;

	if (!m_pData)
		m_pData = (BYTE*)malloc(dwLen);
	else
		m_pData = (BYTE*)realloc(m_pData, m_dwSize + dwLen);

	if (!m_pData)
		return;

	::memcpy(m_pData + m_dwSize, pBytes, dwLen);
	m_dwSize += dwLen;
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::FileEnd()
{
	if (!InProgress())
		return;

	// Is there any data to process?
	if (m_pData)
	{
		FileSave();
		free(m_pData);
		m_pData = NULL;
		m_dwSize = 0;
	}

	// Pop the download item off the list
	m_List.erase(m_List.begin());

	// Are there any more files in the list to download?
	if (Next())
		return; // Yes, more files to download

	// No more files to download, so make the final callback
	if (m_fnCallback)
		m_fnCallback(m_pPlugin, NULL);

	// No more files to download or process
	Free();
}

/////////////////////////////////////////////////////////////////////////////
void CDownload::FileSave()
{
	if (!InProgress())
		return;

	// Is there any file data to process?
	if (!m_pData)
		return;

	// If there is a destination file defined, write the data to disk
	DOWNLOAD Download = m_List.front();
	CString& strDstPath = Download.strDstPath;
	if (!strDstPath.IsEmpty())
	{
		// Write the file to the destination path
		HANDLE hFile = ::CreateFile(strDstPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			char strMsg[256];
			wsprintf(strMsg, "Error creating file '%s' after download.", strDstPath);
			::MessageBox(m_pPlugin->m_hWnd, strMsg, "", MB_OK);
			return;
		}

		DWORD dwBytesWritten = 0;
		bool bFailed = !::WriteFile(hFile, (LPCVOID)m_pData, m_dwSize, &dwBytesWritten, NULL);
		::CloseHandle(hFile);

		if (bFailed)
		{
			char strMsg[256];
			wsprintf(strMsg, "Error writing to '%s' after download.", strDstPath);
			::MessageBox(m_pPlugin->m_hWnd, strMsg, "", MB_OK);
			return;
		}
	}

	// If there is a callback defined, make the call
	if (m_fnCallback)
	{
		Download.pData = m_pData;
		Download.dwSize = m_dwSize;
		m_fnCallback(m_pPlugin, (void*)&Download);
	}
}
