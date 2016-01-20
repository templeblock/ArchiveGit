#pragma once
#include <vector>

using namespace std;

// File flag values
#define UPDATE_TESTREGISTRYVERSION	0x00000001	// Test the version from the registry and not from a file
#define UPDATE_SAVEREGISTRYVERSION	0x00000002	// Save the version in the registry
#define UPDATE_RESOURCES			0x00000004	// Indicates that the file is a resource DLL to be loaded after download
#define UPDATE_EXECUTE				0x00000008	// Indicates that the file is an EXE to be run after download
#define UPDATE_REGISTER				0x00000010	// Indicates that the file is a COM DLL to be registered after download

struct DOWNLOAD_COMMAND_STRUCT
{
	CString strType;
	CString strName;
	CString strVersion;
	DWORD dwFlags;
};
typedef vector<DOWNLOAD_COMMAND_STRUCT> COMMANDLIST;

class CCpDialog;
class CDownload;
class CExtensions;

class CAutoUpdate
{
public:
	CAutoUpdate(CCpDialog* pCpDlg);
	~CAutoUpdate();

	bool Start(bool bSilent, CString& strManifestURL);

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
	bool m_bSilent;
	CCpDialog* m_pCpDlg;
	CString m_strManifestPath;
	CString m_strManifestFile;
	COMMANDLIST m_CommandList;
};
