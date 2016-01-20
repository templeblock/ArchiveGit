#pragma once

class CExtensions
{
public:
	CExtensions();
	~CExtensions();

	void Load(LPCSTR pMessageTitle = NULL);
	void Unload();
	void Reload();
	bool IsLoaded();
	CString GetDllLocation();
	bool GetString(LPCTSTR strName, LPCTSTR strResourceType, CString& strResource);

private:
	HGLOBAL GetData(LPCTSTR strName, LPCTSTR strResourceType, int* pSize);

private:
	CString m_strResourceDLL;
	HMODULE m_hResLib;
	HMODULE m_hOldResLib;
};
