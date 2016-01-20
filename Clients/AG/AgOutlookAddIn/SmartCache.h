#pragma once

class CSmartCache
{
public:
	CSmartCache(void);
	virtual ~CSmartCache(void);
	
public:
	void SetRestoreSmartCacheSetting(bool bSave, bool& bUseSmartCache);
	void SetRestoreSyncMode(bool bSet);
	void SetSyncMode();
	void RestoreSyncMode();
	bool IsUseSmartCache();
	bool UpdateSmartCache();
	bool IsUrlCacheUpToDate(LPCTSTR pstrURL);
	bool DeleteCacheEntries(LPCSTR pstrSearch);
	bool CheckLineListCache();

private:
	bool GetUrlCacheInfo(LPCTSTR pstrURL);

private:
	bool m_bUseSmartCache;
	DWORD m_dwOldSyncMode;
	DWORD m_dwOldSyncMode5;
};
