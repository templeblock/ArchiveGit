#include "stdafx.h"
#include "ShellExtension.h"
#include "Mobilize.h"
#include "Utility.h"

//////////////////////////////////////////////////////////////////////
CShellExtension::CShellExtension()
{
	CoInitialize(NULL);
	m_hMenuBitmap = ::LoadBitmap(_AtlBaseModule.GetModuleInstance(), MAKEINTRESOURCE(IDB_MENUITEM));
}

//////////////////////////////////////////////////////////////////////
CShellExtension::~CShellExtension()
{
	CoUninitialize();
}

//////////////////////////////////////////////////////////////////////
// IShellExtInit
STDMETHODIMP CShellExtension::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
	// Look for CF_HDROP data in the data object
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	if (FAILED(pDataObj->GetData(&fmt, &stg)))
		return E_INVALIDARG;

	// Get a pointer to the actual data
	HDROP hDrop = (HDROP)::GlobalLock(stg.hGlobal);
	if (!hDrop)
	{
		::ReleaseStgMedium(&stg);
		return E_INVALIDARG;
	}

	// Sanity check - make sure there is at least one filename
	UINT uNumFiles = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
	if (!uNumFiles)
	{
		::GlobalUnlock(stg.hGlobal);
		::ReleaseStgMedium(&stg);
		return E_INVALIDARG;
	}

	// Get the name of the each file and store it in an array
	for (UINT uFile = 0; uFile < uNumFiles; uFile++)
	{
		TCHAR szFilePath[MAX_PATH];
		if (!::DragQueryFile(hDrop, uFile, szFilePath, MAX_PATH))
			continue;

		if (!FileIsSupported(szFilePath))
			continue;

		m_arrFiles.Add(szFilePath);
	}

	::GlobalUnlock(stg.hGlobal);
	::ReleaseStgMedium(&stg);
	return (m_arrFiles.GetSize() > 0 ? S_OK : E_INVALIDARG);
}

//////////////////////////////////////////////////////////////////////
bool CShellExtension::FileIsSupported(LPCTSTR pszFilePath)
{
	CString strExtension = StrExtension(pszFilePath);
	bool bIsImage = false;
	CString strContentType = CMobilize::GetContentType(strExtension, bIsImage);
	return !strContentType.IsEmpty();
}

//////////////////////////////////////////////////////////////////////
// IContextMenu
STDMETHODIMP CShellExtension::QueryContextMenu(HMENU hMenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	CMenuHandle menu = hMenu;
	if (!menu.IsMenu())
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	// Find the first menu item separator
	int nTargetPosition = 0;
	int nCount = menu.GetMenuItemCount();
	for (int i=0; i<nCount; i++)
	{
		UINT id = menu.GetMenuItemID(i);
		CString strMenuItem;
		int n = menu.GetMenuString(i, strMenuItem, MF_BYPOSITION/*uFlags*/);
	}

	::InsertMenu(hMenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd, _T("Mobilize This!"));
	if (m_hMenuBitmap)
		::SetMenuItemBitmaps(hMenu, 0/*uMenuIndex*/, MF_BYPOSITION, m_hMenuBitmap, NULL);
	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
}

//////////////////////////////////////////////////////////////////////
// IContextMenu
STDMETHODIMP CShellExtension::GetCommandString(UINT idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	// idCmd must be 0 since we have only one menu item
	if (idCmd)
		return E_INVALIDARG;

	// If Explorer is asking for a help string, copy our string into the supplied buffer
	if (uFlags & GCS_HELPTEXT)
	{
		LPCTSTR szText = _T("Mobilize This to www.3Guppies.com");
		USES_CONVERSION;
		if (uFlags & GCS_UNICODE)
			// We need to cast pszName to a Unicode string, and then use the Unicode string copy API
			lstrcpynW((LPWSTR) pszName, T2CW(szText), cchMax);
		else // Use the ANSI string copy API to return the help string
			lstrcpynA(pszName, T2CA(szText), cchMax);

		return S_OK;
	}

	return E_INVALIDARG;
}

//////////////////////////////////////////////////////////////////////
// IContextMenu2
STDMETHODIMP CShellExtension::HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg ==  WM_INITMENUPOPUP)
		int i = 0;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
DWORD WINAPI MobilizeThreadProc(LPVOID pParam)
{
	CoInitialize(NULL);

	bool bFail = true;
	CString* pstrFileName = (CString*)pParam;
	if (pstrFileName)
	{
		CSimpleArray<CString> arrFiles;
		arrFiles.Add(*pstrFileName);
		delete pstrFileName;

		CMobilize Mobilize;
		Mobilize.UploadFiles(arrFiles);
		bFail = false;
	}

	CoUninitialize();
    return bFail;
}

//////////////////////////////////////////////////////////////////////
// IContextMenu
STDMETHODIMP CShellExtension::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	// If lpVerb really points to a string, ignore this function call and bail out
	if (HIWORD(pCmdInfo->lpVerb))
		return E_INVALIDARG;

	// Get the command index - the only valid one is 0
	switch (LOWORD(pCmdInfo->lpVerb))
	{
		case 0:
		{
			CString strMsg;
			strMsg.Format(_T("The selected file(s) are:\n\n"));
			for (int i=0; i<m_arrFiles.GetSize(); i++)
			{
				if (i > 0)
					strMsg += _T("\n");
				strMsg += m_arrFiles[i];
			}

			SECURITY_ATTRIBUTES* psa = NULL;
			CString* pstrFileName = new CString(m_arrFiles[0]);
			DWORD dwThreadId = 0;
			LPTHREAD_START_ROUTINE pRoutine = MobilizeThreadProc;
			DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS;
			HANDLE hThread = CreateThread(psa, 0/*dwStackSize*/, pRoutine, (void*)pstrFileName, dwCreationFlags, &dwThreadId);
			return S_OK;
		}

		default:
		{
			return E_INVALIDARG;
		}
	}

	return S_OK;
}
