#include "stdafx.h"
#include "SmartCache.h"
#include "Regkeys.h"
#include "Authenticate.h"
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

// IE's cache control setting is stored in the registry key called SyncMode
#define SYNCMODE_NEVER 0
#define SYNCMODE_ONCE_A_DAY 2
#define SYNCMODE_EVERY_PAGE 3
#define SYNCMODE_AUTO 4

/////////////////////////////////////////////////////////////////////////////
CSmartCache::CSmartCache(void)
{
	m_dwOldSyncMode = -1;
	m_dwOldSyncMode5 = -1;
	m_bUseSmartCache = true;
	SetRestoreSmartCacheSetting(false/*bSave*/, m_bUseSmartCache);
}

/////////////////////////////////////////////////////////////////////////////
CSmartCache::~CSmartCache(void)
{
	RestoreSyncMode();
}

/////////////////////////////////////////////////////////////////////////////
bool CSmartCache::IsUseSmartCache()
{
	SetRestoreSmartCacheSetting(false/*bSave*/, m_bUseSmartCache);
	return m_bUseSmartCache;
}

/////////////////////////////////////////////////////////////////////////////
bool CSmartCache::UpdateSmartCache()
{
	m_bUseSmartCache = !m_bUseSmartCache;
	SetRestoreSmartCacheSetting(true/*bSave*/, m_bUseSmartCache);
	SetRestoreSyncMode(m_bUseSmartCache);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CSmartCache::SetRestoreSmartCacheSetting(bool bSave, bool& bUseSmartCache)
{
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return;

	DWORD dwValue = bUseSmartCache;
	if (bSave)
	{
		if (regkey.SetDWORDValue(REGVAL_SMARTCACHE, dwValue) == ERROR_SUCCESS)
			return;
	}
	else
	{
		if (regkey.QueryDWORDValue(REGVAL_SMARTCACHE, dwValue) == ERROR_SUCCESS)
			bUseSmartCache = (bool)(dwValue != 0);
	}

	m_bUseSmartCache = bUseSmartCache;
}

/////////////////////////////////////////////////////////////////////////////
void CSmartCache::SetRestoreSyncMode(bool bSet)
{
	if (bSet)
		SetSyncMode();
	else
		RestoreSyncMode();
}

/////////////////////////////////////////////////////////////////////////////
// Set the IE registry settings that control caching
// Never check for new content unless it doesn't exist in the cache
void CSmartCache::SetSyncMode()
{
	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, REGKEY_IE) != ERROR_SUCCESS)
		return;	

	DWORD dwOldSyncMode = SYNCMODE_AUTO;
	DWORD dwOldSyncMode5 = SYNCMODE_AUTO;

	DWORD dwLength = sizeof(DWORD);
	regkey.QueryBinaryValue(REGVAL_SYNCMODE, &dwOldSyncMode, &dwLength);
	regkey.QueryDWORDValue(REGVAL_SYNCMODE5, dwOldSyncMode5);

	if (dwOldSyncMode == SYNCMODE_NEVER && dwOldSyncMode5 == SYNCMODE_NEVER)
		return;

	DWORD dwNever = SYNCMODE_NEVER;
	dwLength = sizeof(DWORD);
	regkey.SetBinaryValue(REGVAL_SYNCMODE, &dwNever, dwLength);
	regkey.SetDWORDValue(REGVAL_SYNCMODE5, dwNever);

	if (m_dwOldSyncMode == -1)
		m_dwOldSyncMode = (dwOldSyncMode != SYNCMODE_NEVER ? dwOldSyncMode : SYNCMODE_AUTO);

	if (m_dwOldSyncMode5 == -1)
		m_dwOldSyncMode5 = (dwOldSyncMode5 != SYNCMODE_NEVER ? dwOldSyncMode5 : SYNCMODE_AUTO);
}

/////////////////////////////////////////////////////////////////////////////
void CSmartCache::RestoreSyncMode()
{
	if (m_dwOldSyncMode == -1 && m_dwOldSyncMode5 == -1)
		return;

	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, REGKEY_IE) != ERROR_SUCCESS)
		return;

	if (m_dwOldSyncMode != -1)
	{
		DWORD dwLength = sizeof(DWORD);
		regkey.SetBinaryValue(REGVAL_SYNCMODE, &m_dwOldSyncMode, dwLength);
	}

	if (m_dwOldSyncMode5 != -1)
		regkey.SetDWORDValue(REGVAL_SYNCMODE5, m_dwOldSyncMode5);

	m_dwOldSyncMode = -1;
	m_dwOldSyncMode5 = -1;
}

/////////////////////////////////////////////////////////////////////////////
INTERNET_CACHE_ENTRY_INFO* AllocateCacheEntry(INTERNET_CACHE_ENTRY_INFO* pCacheEntryInfo, DWORD dwEntrySize)
{
	delete [] pCacheEntryInfo;
	pCacheEntryInfo = (INTERNET_CACHE_ENTRY_INFO*) new char[dwEntrySize];
	::ZeroMemory(pCacheEntryInfo, dwEntrySize);
	pCacheEntryInfo->dwStructSize = dwEntrySize;
	return pCacheEntryInfo;
}

typedef void (CALLBACK *ENUMERATECACHE_CALLBACK)(IN INTERNET_CACHE_ENTRY_INFO* pCacheEntryInfo);

/////////////////////////////////////////////////////////////////////////////
void CALLBACK MyEnumerateCacheCallback(INTERNET_CACHE_ENTRY_INFO* pCacheEntryInfo)
{
	if (!pCacheEntryInfo)
		return;

	CString strUrlName = pCacheEntryInfo->lpszSourceUrlName;
	int iPos = strUrlName.Find("?");
	if (iPos >= 0)
		strUrlName = strUrlName.Left(iPos);
	strUrlName += "\n";
	ATLTRACE(strUrlName);
	BOOL bOK = DeleteUrlCacheEntry(pCacheEntryInfo->lpszSourceUrlName);
}

/////////////////////////////////////////////////////////////////////////////
bool EnumerateCache(LPCSTR pstrSearch, ENUMERATECACHE_CALLBACK pfnEnumerateCacheCallback)
{
	DWORD dwEntrySize = sizeof(INTERNET_CACHE_ENTRY_INFO);
	INTERNET_CACHE_ENTRY_INFO* pCacheEntryInfo = NULL;
	if (!(pCacheEntryInfo = AllocateCacheEntry(pCacheEntryInfo, dwEntrySize)))
		return false;

	bool bDone = false;
	HANDLE hCacheDir = NULL;
	while (!bDone && !(hCacheDir = ::FindFirstUrlCacheEntry(NULL, pCacheEntryInfo, &dwEntrySize)))
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			if (!(pCacheEntryInfo = AllocateCacheEntry(pCacheEntryInfo, dwEntrySize)))
				bDone = true;
		}
		else
			bDone = true;
	}

	int nCount = 0;
	while (!bDone)
	{
		CString strUrlName = pCacheEntryInfo->lpszSourceUrlName;
		bool bFound = (!pstrSearch || (strUrlName.Find(pstrSearch) >= 0));
		if (bFound)
		{
			if (pfnEnumerateCacheCallback)
				pfnEnumerateCacheCallback(pCacheEntryInfo);
			nCount++;
		}

		while (!bDone && !::FindNextUrlCacheEntry(hCacheDir, pCacheEntryInfo, &dwEntrySize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (!(pCacheEntryInfo = AllocateCacheEntry(pCacheEntryInfo, dwEntrySize)))
					bDone = true;
			}
			else
				bDone = true;
		}
	}

	::FindCloseUrlCache(hCacheDir);
	delete [] pCacheEntryInfo;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CSmartCache::GetUrlCacheInfo(LPCTSTR pstrURL)
{
	DWORD dwEntrySize = sizeof(INTERNET_CACHE_ENTRY_INFO);
	INTERNET_CACHE_ENTRY_INFO* pCacheEntryInfo = NULL;
	if (!(pCacheEntryInfo = AllocateCacheEntry(pCacheEntryInfo, dwEntrySize)))
		return false;

	bool bDone = false;
	while (!bDone && !GetUrlCacheEntryInfoEx(pstrURL, pCacheEntryInfo, &dwEntrySize, NULL, NULL, NULL, NULL))
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			if (!(pCacheEntryInfo = AllocateCacheEntry(pCacheEntryInfo, dwEntrySize)))
				bDone = true;
		}
		else
			bDone = true;
	}

	bool bSuccess = !bDone;
	if (bSuccess)
	{
		COleDateTime SyncTime = pCacheEntryInfo->LastSyncTime;
		CString strSyncDate;
		strSyncDate.Format("%d/%d/%d", SyncTime.GetMonth(), SyncTime.GetDay(), SyncTime.GetYear());
		COleDateTime AccessTime = pCacheEntryInfo->LastAccessTime;
		CString strAccessDate;
		strAccessDate.Format("%d/%d/%d", AccessTime.GetMonth(), AccessTime.GetDay(), AccessTime.GetYear());
	}
	
	delete [] pCacheEntryInfo;
	
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
bool CSmartCache::IsUrlCacheUpToDate(LPCTSTR pstrURL)
{
	bool bOK = GetUrlCacheInfo(pstrURL);
	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
bool CSmartCache::DeleteCacheEntries(LPCSTR pstrSearch)
{
	return EnumerateCache(pstrSearch, MyEnumerateCacheCallback);
}
/////////////////////////////////////////////////////////////////////////////
bool CSmartCache::CheckLineListCache()
{
	CAuthenticate Auth;

	// Get the latest line-list date from registry. This is the date passed to
	// plugin during authentiication.
	CString szLineListDate;
	if (!Auth.ReadLineListDate(szLineListDate))
		return false;
	
	// Convert line-list date to COleDateTime.
	COleDateTime dtLineListDate;
	if (!dtLineListDate.ParseDateTime(szLineListDate, VAR_DATEVALUEONLY))
		return false;

	// Get the date the line list was last updated, i.e. the date corresponding to line list in cache.
	// This is the date that smartcache writes to the registry when a clear cache event is triggered.
	// Note: the very first time this is called the update date may not be in the registry. In this case, 
	// the update date is set to 0 forcing the cache to refresh.
	CString szLineListUpdate;
	Auth.ReadLineListUpdate(szLineListUpdate);
	COleDateTime dtLineListUpdate;
	if (!dtLineListUpdate.ParseDateTime(szLineListUpdate, VAR_DATEVALUEONLY))
		dtLineListUpdate = (DATE)0;


	// Compare current line list date with the date it was last updated. If the line list date
	// exceeds the update date then its time to refresh the cache.
	if (dtLineListDate > dtLineListUpdate)
	{
		// refresh cache
		CString szHost = Auth.GetBaseUrl();
		if (szHost.IsEmpty())
			return false;

		if (!DeleteCacheEntries(szHost))
			return false;

		// If cache refresh was successful then save the latest line list date as the update date
		// in registry.
		Auth.WriteLineListUpdate(szLineListDate);
	};

	return true;
}
