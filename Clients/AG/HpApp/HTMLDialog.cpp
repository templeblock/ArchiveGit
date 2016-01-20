#include "stdafx.h"
#include "HTMLDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

/*
/////////////////////////////////////////////////////////////////////////////////
// Javascript methods that can be used in your Html Dialogs views
/////////////////////////////////////////////////////////////////////////////////

	<script language="javascript"> 
	<!-- 
		// Example: window.dialogArguments = "Button3=Test;Button1=Crap";

		function load() 
		{
			SetControlValue(Button1);
		}

		function unload() 
		{
			ReturnControlValue(Button1);
		}

		function ReturnControlValue(control)
		{
			if (!window.returnValue)
				return;
			window.returnValue += (control.name + "=" + control.innerText + ";");
		}

		function SetControlValue(control)
		{
			// Get the dialog argument list and parse it for the control
			if (!window.dialogArguments)
				return;
			var args = new Array();
			if (!args)
				return;
			var params = new Array();
			if (!params)
				return;

			args = window.dialogArguments.split(";");
			for (nIndex = 0; nIndex < args.length; nIndex++)
			{
				params = args[nIndex].split("=");
				if (control.name == params[0])
					control.innerText = params[1];
			}
		}
	--> 
	</script> 

/////////////////////////////////////////////////////////////////////////////////
// This is a code snipet showing the usage of CHtmlDialog
/////////////////////////////////////////////////////////////////////////////////

	// If using resource
	CHtmlDialog dlg("PRINT_HTML_RESOURCE", TRUE, hWndCtl);
	// If referencing html file
	CHtmlDialog dlg(_T("C:\\American Greetings\\HTML_Dialog\\HDDemo\\res\\html1.htm"), FALSE, hWndCtl);

	// Set initial values by calling SetValue(ControlName, ControlValue);
	dlg.SetValue(_T("UserName"), "Mike D");

	// Then call DoModal to display window. If successful will return IDOK, otherwise IDCANCEL
	if (dlg.DoModal() == IDOK)
	{
		// If successful call GetControlValue for each contol. 
		// Order does not matter.
		CString szUserNameValue;
		dlg.GetControlValue(_T("UserName"),   szUserNameValue);
	}
*/

/* Options:
dialogHeight:sHeight Sets the height of the dialog window (see Remarks for default unit of measure). 
dialogLeft:sXPos Sets the left position of the dialog window relative to the upper-left corner of the desktop. 
dialogTop:sYPos Sets the top position of the dialog window relative to the upper-left corner of the desktop. 
dialogWidth:sWidth Sets the width of the dialog window (see Remarks for default unit of measure). 
center:{ yes | no | 1 | 0 | on | off } Specifies whether to center the dialog window within the desktop. The default is yes. 
dialogHide:{ yes | no | 1 | 0 | on | off } Specifies whether the dialog window is hidden when printing or using print preview. This feature is only available when a dialog box is opened from a trusted application. The default is no. 
edge:{ sunken | raised } Specifies the edge style of the dialog window. The default is raised. 
help:{ yes | no | 1 | 0 | on | off } Specifies whether the dialog window displays the context-sensitive Help icon. The default is yes. 
resizable:{ yes | no | 1 | 0 | on | off } Specifies whether the dialog window has fixed dimensions. The default is no. 
scroll:{ yes | no | 1 | 0 | on | off } Specifies whether the dialog window displays scrollbars. The default is yes. 
status:{ yes | no | 1 | 0 | on | off } Specifies whether the dialog window displays a status bar. The default is yes for untrusted dialog windows and no for trusted dialog windows. 
unadorned:{ yes | no | 1 | 0 | on | off } Specifies whether the dialog window displays the border window chrome. This feature is only available when a dialog box is opened from a trusted application. The default is no. 

Remarks:
The default unit of measure for dialogHeight and dialogWidth in Internet Explorer 4.0 is the pixel;
in Internet Explorer 5 it is the em. The value can be an integer or floating-point number, followed
by an absolute units designator (cm, mm, in, pt, pc, or px) or a relative units designator (em or ex).
For consistent results, specify the dialogHeight and dialogWidth in pixels when designing modal dialog boxes.
*/
//////////////////////////////////////////////////////////////////////
CHtmlDialog::CHtmlDialog(UINT nResID, HWND hWnd)
{
	m_hWndParent = hWnd;
	CString strTemp;
	strTemp.Format("%d", nResID);	
	ResourceToURL(strTemp);
	CommonConstruct();
}

//////////////////////////////////////////////////////////////////////
CHtmlDialog::CHtmlDialog(const CString& strURL, BOOL bRes, HWND hWnd)
{
	m_hWndParent = hWnd;

	if (bRes)
		ResourceToURL(strURL);
	else
		m_strURL = strURL;
	CommonConstruct();
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::CommonConstruct()
{
	m_hInstMSHTML = ::LoadLibrary("MSHTML.DLL");
	m_strOptions = "";
//j	m_varReturn
//j	SetWindowSize(400, 250);
	SetResizable(false);
	SetStatusBar(false);
	SetHelpButton(false);
	SetCenter(true);
	SetRaisedEdge(true);
}

//////////////////////////////////////////////////////////////////////
CHtmlDialog::~CHtmlDialog()
{
	// Free the Mshtml.dll
	if (m_hInstMSHTML)
		::FreeLibrary(m_hInstMSHTML);
}

//////////////////////////////////////////////////////////////////////
bool CHtmlDialog::DoModal()
{
	if (!m_hInstMSHTML)
		return false;

	// First get the proc ShowHTMLDialog
	typedef HRESULT STDAPICALLTYPE SHOWHTMLDIALOGFN(HWND hwndParent, IMoniker *pmk, VARIANT *pvarArgIn, WCHAR* pchOptions, VARIANT *pvArgOut);
	SHOWHTMLDIALOGFN* pfnShowHTMLDialog = (SHOWHTMLDIALOGFN*)::GetProcAddress(m_hInstMSHTML, TEXT("ShowHTMLDialog"));
//j	typedef HRESULT STDAPICALLTYPE SHOWHTMLDIALOGEXFN(HWND hwndParent, IMoniker *pmk, DWORD dwDialogFlags, VARIANT *pvarArgIn, WCHAR* pchOptions, VARIANT *pvArgOut);
//j	SHOWHTMLDIALOGEXFN* pfnShowHTMLDialog = (SHOWHTMLDIALOGEXFN*)::GetProcAddress(m_hInstMSHTML, TEXT("ShowHTMLDialogEx"));
	if (!pfnShowHTMLDialog)
		return false;
	
	// Now create a URL Moniker
	IMoniker* pmk = NULL;
	::CreateURLMoniker(NULL, CComBSTR(m_strURL), &pmk);
	if (!pmk)
		return false;
	
	// Now show the HTML Dialog
	HWND hWndFocus = ::GetFocus();
	CComVariant varArgs = m_strArguments;
	HWND hWndParent = (::IsWindow(m_hWndParent) && ::IsWindowVisible(m_hWndParent) ? m_hWndParent : NULL);
	HRESULT hr = pfnShowHTMLDialog(hWndParent, pmk, &varArgs, CComBSTR(m_strOptions), &m_varReturn);
//j	CComVariant vstrOptions = m_strOptions;
//j	HRESULT hr = pfnShowHTMLDialog(hWndParent, pmk, HTMLDLG_MODAL, &varArgs, vstrOptions, &m_varReturn);
	pmk->Release();
	::SetFocus(hWndFocus);

	return (FAILED(hr) ? false : true);
}

//////////////////////////////////////////////////////////////////////
IHTMLWindow2* CHtmlDialog::DoModeless()
{
	if (!m_hInstMSHTML)
		return false;

	// First get the proc ShowModelessHTMLDialog
	typedef HRESULT STDAPICALLTYPE SHOWMODELESSHTMLDIALOGFN(HWND hwndParent, IMoniker *pmk, VARIANT *pvarArgIn, VARIANT* pvarOptions, IHTMLWindow2 ** ppWindow);
	SHOWMODELESSHTMLDIALOGFN* pfnShowHTMLDialog = (SHOWMODELESSHTMLDIALOGFN*)::GetProcAddress(m_hInstMSHTML, TEXT("ShowModelessHTMLDialog"));
	if (!pfnShowHTMLDialog)
		return false;
	
	// Now create a URL Moniker
	IMoniker* pmk = NULL;
	::CreateURLMoniker(NULL, CComBSTR(m_strURL), &pmk);
	if (!pmk)
		return false;
	
	// Now show the HTML Dialog
	HWND hWndFocus = ::GetFocus();
	CComVariant varArgs = m_strArguments;
	HWND hWndParent = (::IsWindow(m_hWndParent) && ::IsWindowVisible(m_hWndParent) ? m_hWndParent : NULL);
	IHTMLWindow2* pModelessWindow = NULL;
	CComVariant vstrOptions = m_strOptions;
	HRESULT hr = pfnShowHTMLDialog(hWndParent, pmk, &varArgs, &vstrOptions, &pModelessWindow);
	pmk->Release();
	::SetFocus(hWndFocus);

	return pModelessWindow;
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::ResourceToURL(const CString& strURL)
{
#ifdef _AFXDLL
	HMODULE hModule = afxCurrentResourceHandle;
#else
	HMODULE hModule = _AtlBaseModule.GetResourceInstance();
#endif

	LPTSTR lpszModule = new TCHAR[MAX_PATH];
	if (GetModuleFileName(hModule, lpszModule, MAX_PATH))
		m_strURL.Format(_T("res://%s/%s"), lpszModule, strURL);
	
	delete [] lpszModule;
}

//////////////////////////////////////////////////////////////////////
CString CHtmlDialog::GetReturnString()
{
	if (!m_varReturn.bstrVal)
		return CString("");

	return CString(m_varReturn.bstrVal);
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetDlgOptions(const CString& strOptions)
{
	m_strOptions += strOptions;
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetScrollbars(bool bScrollbars)
{
	CString szOption;
	szOption.Format("scroll:%d;", bScrollbars);
	SetDlgOptions(szOption);
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetResizable(bool bResizable)
{
	CString szOption;
	szOption.Format("resizable:%d;", bResizable);
	SetDlgOptions(szOption);
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetStatusBar(bool bStatusBar)
{
	CString szOption;
	szOption.Format("status:%d;", bStatusBar);
	SetDlgOptions(szOption);
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetHelpButton(bool bHelpButton)
{
	CString szOption;
	szOption.Format("help:%d;", bHelpButton);
	SetDlgOptions(szOption);
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetCenter(bool bCenter)
{
	CString szOption;
	szOption.Format("center:%d;", bCenter);
	SetDlgOptions(szOption);
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetRaisedEdge(bool bRaisedEdge)
{
	CString szOption;
	szOption.Format("edge:%s;", (bRaisedEdge? "raised" : "sunken"));
	SetDlgOptions(szOption);
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetWindowSize(int nWidth, int nHeight)
{
	CString szOption;
	szOption.Format("dialogWidth:%dpx; dialogHeight:%dpx;", nWidth, nHeight);
	SetDlgOptions(szOption);
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetWindowPosition(int nLeft, int nTop)
{
	CString szOption;
	szOption.Format("dialogTop:%dpx; dialogLeft:%dpx", nTop, nLeft);
	SetDlgOptions(szOption);
}

//////////////////////////////////////////////////////////////////////
void CHtmlDialog::SetValue(const CString& strControlName, const CString& strValue)
{
	CString szItem = strControlName;
	szItem += "=";
	szItem += strValue;

	if (m_strArguments.GetLength())
		m_strArguments += ";";

	m_strArguments += szItem;
}

//////////////////////////////////////////////////////////////////////
bool CHtmlDialog::GetControlValue(const CString& strControlName, CString& szValue)
{
	if (!m_varReturn.bstrVal)
		return false;

	CString szReturnString(m_varReturn.bstrVal);

	do
	{
		if (szReturnString == _T("\0") || !szReturnString.GetLength())
			break;

		CString szControlValuePair = szReturnString.SpanExcluding(_T(";"));
		CString szControlName = szControlValuePair.SpanExcluding(_T("="));

		if (!szControlName.CompareNoCase(strControlName))
		{
			szValue = szControlValuePair.Right(szControlValuePair.GetLength() - (szControlName.GetLength() + 1));
			return true;
		}
		
		CString szTemp = szReturnString.Right(szReturnString.GetLength() - (szControlValuePair.GetLength() + 1));
		szReturnString = szTemp;

	} while (true);

	return false;
}
