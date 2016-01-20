#pragma once

class CMessageBox
{
public:
	static int Message(UINT uMessage, DWORD dwStyle = MB_OK, UINT uIcon = NULL);
	static int Message(LPCTSTR pMessage, DWORD dwStyle = MB_OK, UINT uIcon = NULL);
	static void NoObjectSelected();
	static void ObjectIsLocked();
	static void SetDefaults(HWND hWndParent, LPCSTR strTitle, LPCSTR strHtmlFile);
protected:
	static bool ComputeMessageBoxRect(CRect& rMessageBox, LPCTSTR pMessage);

	static HWND m_hWndParent;
	static CString m_strTitle;
	static CString m_strHtmlFile;
};
