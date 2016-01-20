#pragma once
#include "resource.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// IUnusedInterface
[
	object,
	uuid("91B336EE-A725-4A27-AC48-D1B0E17121AC"),
	helpstring("IUnusedInterface Interface"),
	pointer_default(unique)
]
__interface IUnusedInterface : IUnknown
{
};

// CShellExtension
[
	coclass,
	com_interface_entry("COM_INTERFACE_ENTRY(IShellExtInit)"),
	com_interface_entry("COM_INTERFACE_ENTRY_IID(IID_IContextMenu, IContextMenu)"),
	threading(apartment),
	aggregatable(never),
	vi_progid("a3GPlugin.ShellExtension"),
	progid("a3GPlugin.ShellExtension.1"),
	version(1.0),
	uuid("89D72376-67AA-4021-B089-AF4FC1458BAC"),
	helpstring("ShellExtension Class")
]
class ATL_NO_VTABLE CShellExtension :
//j	public IUnusedInterface,
	public IShellExtInit,
	public IContextMenu2
{
public:
	CShellExtension();
	~CShellExtension();
	static const TCHAR* GetObjectFriendlyName() 
	{
		return _T("Mobilize desktop content to your 3Guppies locker!");
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	// IShellExtInit
	STDMETHODIMP IShellExtInit::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID);

	// IContextMenu
	STDMETHODIMP IContextMenu::QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags);
	STDMETHODIMP IContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo);
	STDMETHODIMP IContextMenu::GetCommandString(UINT idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax);

	// IContextMenu2
	STDMETHODIMP IContextMenu2::HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	bool FileIsSupported(LPCTSTR pszFilePath);

private:
	HBITMAP m_hMenuBitmap;
	CSimpleArray<CString> m_arrFiles;
};
