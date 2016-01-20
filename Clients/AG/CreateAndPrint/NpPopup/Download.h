#pragma once

#include <vector>

struct DOWNLOAD;
class CPlugin;

typedef bool (CALLBACK * FNDOWNLOADCALLBACK)(CPlugin* pPlugin, void* pDownload);

typedef std::vector<DOWNLOAD> DOWNLOADLIST;

struct DOWNLOAD
{
	CString strSrcURL;
	CString strDstPath;
	int iData;
	BYTE* pData;
	DWORD dwSize;
};

class CDownload
{
public:
	CDownload(CPlugin* pPlugin);
	~CDownload();
	bool Init(int iFileType, FNDOWNLOADCALLBACK fnCallback);
	void AddFile(CString& strSrcURL, CString& strDstPath, int iData);
	bool Start();
	bool Next();
	void FileData(BYTE* pBytes, DWORD dwLen);
	void FileEnd();
	bool InProgress()
		{ return (m_iFileType >= 0); }
protected:
	void Free();
	void FileSave();

protected:
	CPlugin* m_pPlugin;
	int m_iFileType;
	DOWNLOADLIST m_List;
	BYTE* m_pData;
	DWORD m_dwSize;
	FNDOWNLOADCALLBACK m_fnCallback;
};
