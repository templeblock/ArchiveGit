#pragma once

#include "AutoUpdate.h"

class CHpAutoUpdate : public CAutoUpdate
{
public:
	CHpAutoUpdate(CCtp* pCtp);
	~CHpAutoUpdate(){};

	bool Start(bool bSilent, CString& strManifestURL);

protected:
	bool FileUpToDate(const CString& strFilePath, const CString& strManifestVersion, bool bTestRegistryVersion);
	bool RegisterSilentUpdate();
	bool IsSilentUpdateAllowed();
	bool SaveRegistryVersion(const CString& strFilePath, const CString& strVersion);
	void GetDestLocation(CString strFilePath, CString& strDestPath);

	CString m_strKeyName;
	CString m_strAppPath;
	CString m_strAppName;
};


