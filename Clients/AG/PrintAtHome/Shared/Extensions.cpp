#include "stdafx.h"
#include "Extensions.h"
#include "Utility.h"

/////////////////////////////////////////////////////////////////////////////
CExtensions::CExtensions()
{
	m_hResLib = NULL;
	m_hOldResLib = NULL;

	Load();
}

/////////////////////////////////////////////////////////////////////////////
CExtensions::~CExtensions()
{
	Unload();
}

/////////////////////////////////////////////////////////////////////////////
void CExtensions::Load(LPCSTR pMessageTitle)
{
	if (m_hResLib)
		return;

	CString strResourceDLL = GetDllLocation() + _T("Extensions.dll");
	m_strResourceDLL = RandomFileName("res", ".dll");
	bool bOK = !!::CopyFile(strResourceDLL, m_strResourceDLL, false/*bFailIfExists*/);
	if (!bOK)
		m_strResourceDLL.Empty();

	m_hResLib = (m_strResourceDLL.IsEmpty() ? NULL : ::LoadLibrary(m_strResourceDLL));
	if (!m_hResLib)
	{
		if (pMessageTitle)
			::MessageBox(NULL, "Not using extended resources", pMessageTitle, MB_OK | MB_ICONINFORMATION);
		return;
	}

	m_hOldResLib = _AtlBaseModule.GetResourceInstance();
	_AtlBaseModule.SetResourceInstance(m_hResLib);
}

/////////////////////////////////////////////////////////////////////////////
void CExtensions::Unload()
{
	if (m_hResLib)
	{
		_AtlBaseModule.SetResourceInstance(m_hOldResLib);
		::FreeLibrary(m_hResLib);
		m_hResLib = NULL;
	}

	if (!m_strResourceDLL.IsEmpty())
	{
		::DeleteFile(m_strResourceDLL);
		m_strResourceDLL.Empty();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CExtensions::Reload()
{
	Unload();
	Load();
}

/////////////////////////////////////////////////////////////////////////////
bool CExtensions::IsLoaded()
{
	return (m_hResLib != NULL);
}

/////////////////////////////////////////////////////////////////////////////
CString CExtensions::GetDllLocation()
{
	char strModule[MAX_PATH];
	::GetModuleFileName(_AtlBaseModule.GetModuleInstance(), strModule, sizeof(strModule));
	CString strFolder = CString(strModule);
	strFolder = strFolder.Left(strFolder.ReverseFind(_T('\\')) + 1);
	return strFolder;
}

/////////////////////////////////////////////////////////////////////////////
bool CExtensions::GetString(LPCTSTR strName, LPCTSTR strResourceType, CString& strResource)
{
	HINSTANCE hResLib = _AtlBaseModule.GetResourceInstance();

	int nSize = 0;
	HGLOBAL hMemory = GetData(strName, strResourceType, &nSize);
	if (!hMemory)
		return false;

	char* pMemory = (char*)::LockResource(hMemory);
	if (!pMemory)
	{
		::FreeResource(hMemory);
		return false;
	}
	
	strResource = CString(pMemory, nSize);

	UnlockResource(hMemory);
	::FreeResource(hMemory);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CExtensions::GetData(LPCTSTR strName, LPCTSTR strResourceType, int* pSize)
{
	HINSTANCE hResLib = _AtlBaseModule.GetResourceInstance();

	HRSRC hResource = ::FindResource(hResLib, strName, strResourceType);
	if (!hResource)
		return NULL;

	*pSize = ::SizeofResource(hResLib, hResource);

	// Load the resource, user must free
	return ::LoadResource(hResLib, hResource);
}
