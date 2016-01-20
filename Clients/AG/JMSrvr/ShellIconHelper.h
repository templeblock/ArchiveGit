#pragma	once

#include <atlwin.h>

template <typename TT>
class CShellIconHelper : public	CWindowImpl<TT>
{
public:
	/////////////////////////////////////////////////////////////////////////////
	CShellIconHelper() 
	{
		m_bVisible = false;
		m_bTimerActive = false;
		m_TimerId =	0;
		m_wTimerDuration = 0;
		m_CurrentIconResource =	0;
		m_hIcon = NULL;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	~CShellIconHelper() 
	{
		if (m_hIcon)
			::DestroyIcon(m_hIcon);
	}

	/////////////////////////////////////////////////////////////////////////////
	virtual	WORD ShowPopupMenu(WORD	PopupMenuResource)
	{
		CMenu Menu;
		bool bOK = !!Menu.LoadMenu(PopupMenuResource);
		if (!bOK)
			return 0;

		CMenu Popup(Menu.GetSubMenu(0));
		if (!Popup.IsMenu())
			return 0;

		CAuthenticate Authenticate;
		if (Authenticate.IsUserPaidMember())
			Popup.RemoveMenu(ID_TRAY_UPGRADENOW, MF_BYCOMMAND);

		// See if the AOL Browser is running. If not then remove Launch option.
		if (!::FindWindowEx(NULL, NULL, _T("AOL Frame25"), NULL))
			Popup.RemoveMenu(ID_TRAY_LAUNCH, MF_BYCOMMAND);

		// To display a	context	menu for a notification	icon, the 
		// current window must be the foreground window	before the 
		// application calls TrackPopupMenu	or TrackPopupMenuEx. Otherwise,	
		// the menu	will not disappear when	the	user clicks	outside	of the menu	
		// or the window that created the menu (if it is visible). 
		::SetForegroundWindow(m_hWnd);

		POINT pt;
		::GetCursorPos(&pt);
		WORD cmd = Popup.TrackPopupMenu(TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, m_hWnd);
		
		// See MS KB article Q135788
		::PostMessage(m_hWnd, WM_NULL, 0, 0);

		// Destroy the menu, we're	not	longer using it.
		Menu.DestroyMenu();

		return cmd;		
	}

	/////////////////////////////////////////////////////////////////////////////
	virtual	void SetShellTipText(LPCSTR TipText)
	{
		// Save	this text for when we update
		m_TipText =	TipText;
		ShellNotify(NIM_MODIFY);
	}

	/////////////////////////////////////////////////////////////////////////////
	virtual	void SetShellIcon(WORD IconResource)
	{
		// Save	this icon resource for when	we update
		m_CurrentIconResource =	IconResource;
		m_hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(),	(LPCTSTR)MAKEINTRESOURCE(m_CurrentIconResource), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		ShellNotify(NIM_MODIFY);
	}

	/////////////////////////////////////////////////////////////////////////////
	virtual	void SetShellTimer(BOOL	bEnabled, WORD wTimerDuration =	1000)
	{
		if (m_bTimerActive)
			::KillTimer(m_hWnd,	m_TimerId);

		if (bEnabled) // User wants	to start a timer
			m_TimerId =	::SetTimer(m_hWnd, 1, wTimerDuration, NULL);

		m_wTimerDuration = wTimerDuration;
		m_bTimerActive = !!bEnabled;
	}

	/////////////////////////////////////////////////////////////////////////////
	virtual	void SetShellVisible(BOOL bVisible)
	{
		if (bVisible) // User wants	to show	the	icon in	the	shell
		{
			if (!m_bVisible)
				CreateShell();

			SetShellTimer(m_bTimerActive, m_wTimerDuration);
		}
		else //	User wants rid of the icon
		{ 
			if (m_bVisible)
			{
				if (m_bTimerActive)
					::KillTimer(m_hWnd,	m_TimerId);

				DestroyShell();
			}
		}

		m_bVisible = !!bVisible;
	}

private:
	/////////////////////////////////////////////////////////////////////////////
	virtual	HRESULT	CreateShell()
	{
		RECT rect;
		rect.left =	rect.right = rect.top =	rect.bottom	= 0;

		// Create a	hidden window (using CWindowImpl)
		HWND hWnd =	Create(NULL, rect, m_TipText, WS_POPUP);
		if (hWnd)
		{
			// Add the icon	into the shell
			ShellNotify(NIM_ADD);
			return S_OK;
		}

		return HRESULT_FROM_WIN32(GetLastError());
	}
	  
	/////////////////////////////////////////////////////////////////////////////
	virtual	void DestroyShell()
	{
		ShellNotify(NIM_DELETE); //	Remove the icon
		if (m_hWnd)
			DestroyWindow(); // Get rid of the hidden window
	}

	/////////////////////////////////////////////////////////////////////////////
	void ShellNotify(DWORD msg)
	{
		NOTIFYICONDATA notifyIconData;
		notifyIconData.cbSize =	sizeof(notifyIconData);
		notifyIconData.hWnd	= m_hWnd;
		notifyIconData.uFlags =	NIF_ICON | NIF_MESSAGE | NIF_TIP;
		notifyIconData.uCallbackMessage	= WM_USER;
		notifyIconData.uID = 0;	
		notifyIconData.hIcon = m_hIcon;
		::lstrcpyn(notifyIconData.szTip, m_TipText,	64); //	Limit to 64	chars
		::Shell_NotifyIcon(msg,	&notifyIconData);	 
	}

private:
	bool m_bVisible;
	bool m_bTimerActive;
	UINT_PTR m_TimerId;
	WORD m_wTimerDuration;
	int	m_CurrentIconResource;
	CString	m_TipText;
	HICON m_hIcon;
};
