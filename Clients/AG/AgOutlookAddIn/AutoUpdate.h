#pragma once
#include <vector>

using namespace std;

// File flag values
#define UPDATE_TESTREGISTRYVERSION	0x00000001	// Test the version from the registry and not from a file
#define UPDATE_SAVEREGISTRYVERSION	0x00000002	// Save the version in the registry
#define UPDATE_RESOURCES			0x00000004	// Indicates that the file is a resource DLL to be loaded after download
#define UPDATE_EXECUTE				0x00000008	// Indicates that the file is an EXE to be run after download
#define UPDATE_REGISTER				0x00000010	// Indicates that the file is a COM DLL to be registered after download

#define AUTOUPDATE_STATUS_DONE		0x0000
#define AUTOUPDATE_STATUS_STARTED	0x0001

struct DOWNLOAD_COMMAND_STRUCT
{
	CString strType;
	CString strName;
	CString strVersion;
	DWORD dwFlags;
};
typedef vector<DOWNLOAD_COMMAND_STRUCT> COMMANDLIST;

class CAsync;

class CAutoUpdate
{
public:
	CAutoUpdate(CAsync* pAddin);
	~CAutoUpdate();

	bool Start(bool bSilent);
	static bool SetAutoUpdateStatus(DWORD dwStatus);
	static bool GetAutoUpdateStatus(DWORD &dwStatus);
	void SetWndOwner(HWND hwnd);

private:
	bool IsSilentUpdateAllowed();
	bool RegisterSilentUpdate();
	void HandleManifest();
	void GetDestLocation(CString strFile, CString& strDest);
	static bool CALLBACK MyGetManifestCallback(LPARAM lParam, void* pDownloadVoid);
	static bool CALLBACK MyUpdateCallback(LPARAM lParam, void* pDownloadVoid);
	bool LoadManifest(CString& strManifestFile);
	bool FileUpToDate(const CString& strFilePath, const CString& strManifestVersion, bool bTestRegistryVersion);
	static bool SaveRegistryVersion(const CString& strFilePath, const CString& strVersion);
	bool ProcessManifest();

private:
	HWND m_hOwner;
	bool m_bSilent;
	CAsync* m_pAddin;
	CString m_strManifestPath;
	CString m_strManifestFile;
	COMMANDLIST m_CommandList;
};
