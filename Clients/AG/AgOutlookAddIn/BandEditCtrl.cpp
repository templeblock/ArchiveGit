// ============================================================================
// Sample by Rashid Thadha Aug 2001
// ============================================================================

#include "stdafx.h"
#include "BandEditCtrl.h"
#include "JMToolband.h"


#include <string>


CBandComboBoxCtrl::CBandComboBoxCtrl() : m_cRef(0), m_pBand(NULL), m_bAuto(true)
{
	m_spWebBrowser = NULL;
}

CBandComboBoxCtrl::~CBandComboBoxCtrl()
{
	m_spWebBrowser = NULL;
}
/**************************************************************************

   CBandComboBoxCtrl::SetToolBand()
   
**************************************************************************/
void CBandComboBoxCtrl::SetToolBand(CJMToolband *pToolband)
{
	m_pBand = pToolband;
}
/**************************************************************************

   CBandComboBoxCtrl::ProcessSearch()
   
**************************************************************************/
bool CBandComboBoxCtrl::ProcessSearch()
{
	if (!m_spWebBrowser)
		return false;

	if (!GetWindowTextLength())
		return false;
	
	CString szQuery = _T("http://www.google.com/search?hl=en&btnG=Google+Search&q=");
	CComBSTR bsQuery = NULL;
	GetWindowText(&bsQuery);
	szQuery += CString(bsQuery); 
	CComVariant varFlags = navAllowAutosearch;
	CComVariant varTargetFrameName = "_self";
	HRESULT hr = m_spWebBrowser->Navigate(CComBSTR(szQuery), &varFlags, &varTargetFrameName, NULL/*&varPostData*/, NULL/*&varHeaders*/);
	if (FAILED(hr))
		return false;

	return true;
}
/**************************************************************************

   CBandComboBoxCtrl::InsertHistory()
   
**************************************************************************/
bool CBandComboBoxCtrl::InsertHistory()
{
    ResetContent();

    for (RegIterator it; it; it++)
        InsertString(-1, it);


    return true;
}
/**************************************************************************

   CBandComboBoxCtrl::Process()
   
**************************************************************************/
// This will add the history of data used
LRESULT CBandComboBoxCtrl::Process(LPCTSTR szQuery) 
{
    CRegKey keyAppID;

	CString szValue(szQuery);
	if (szValue.IsEmpty())
		return S_OK;

    int h = GetCount();
    int pos = FindStringExact(-1,szValue);
    if(pos == CB_ERR) 
    {
        // not found
        if(h<50)
        {
            // normal
            InsertString(0,szValue);
        }
        else
        {
            // replace last
            DeleteString(h-1);
            InsertString(0,szValue);
        }
    }
    else
    {
        // move to last
        DeleteString(pos);
        InsertString(0,szValue);
    }

	// Update history
    if (keyAppID.Open(REGKEY_APPROOT, REGKEY_SEARCH_HISTORY) == ERROR_SUCCESS)
			keyAppID.RecurseDeleteKey(REGKEY_SEARCH_HISTORY);
	keyAppID.Close();

    if (keyAppID.Create(REGKEY_APPROOT, REGKEY_SEARCH_HISTORY) ==  ERROR_SUCCESS)
	{
		CString szItem;
		for(int i=0;i<GetCount();i++)
		{
			GetLBText(i, szItem);
			keyAppID.SetDWORDValue(szItem, 1);
		}
	}
	keyAppID.Close();

    SetCurSel(0);
  
	// Perform web search
	ProcessSearch();

    return S_OK;
}
/**************************************************************************

   CBandComboBoxCtrl::TranslateAcceleratorIO()
   
**************************************************************************/
// Because the host owns the message processing, it will get all keyboard messages
// first.
// 1. We want to use the ENTER key to signal that the user is done in the edit control,
//    so we need to watch for VK_RETURN on WM_KEYUP
// 2. We also want the BACKSPACE, DELETE, END, ARROW keys to go to the edit control and
//	  not to the standard IE interfaces. We translate/dispatch these messages ourselves.
STDMETHODIMP CBandComboBoxCtrl::TranslateAcceleratorIO(LPMSG pMsg)
{
	int nVirtKey = (int)(pMsg->wParam);
	if (WM_KEYUP == pMsg->message && VK_RETURN == nVirtKey)
	{
        CString szValue;
        GetWindowText(szValue);
        Process(szValue);
		return S_OK;
	}
	else if ((WM_KEYUP == pMsg->message || WM_KEYDOWN == pMsg->message) &&
			VK_BACK == nVirtKey || (VK_END <= nVirtKey && VK_DOWN >= nVirtKey) ||
			VK_DELETE == nVirtKey || ( (GetKeyState(VK_CONTROL)&0x80) && (nVirtKey=='C' || nVirtKey=='V' || nVirtKey=='X')))
	{
        m_bAuto = false;
        TranslateMessage(pMsg);
        DispatchMessage(pMsg);
        m_bAuto = true;
        if(WM_KEYDOWN == pMsg->message && VK_BACK == nVirtKey) 
            m_bAuto = false;
		return S_OK;
	}
	else
		return S_FALSE;
}


/**************************************************************************

   CBandComboBoxCtrl::OnTVSetFocus()
   
**************************************************************************/
LRESULT CBandComboBoxCtrl::OnTVSetFocus(WORD, WORD pnmh, HWND,BOOL& )
{
    m_pBand->FocusChange(true);
    return S_OK;
}

/**************************************************************************

   CBandComboBoxCtrl::OnTVKillFocus()
   
**************************************************************************/
LRESULT CBandComboBoxCtrl::OnTVKillFocus(WORD, WORD pnmh, HWND,BOOL& )
{
    m_pBand->FocusChange(false);
    return S_OK;
}
/**************************************************************************

   CBandComboBoxCtrl::OnTVSelChanged()
   
**************************************************************************/
LRESULT CBandComboBoxCtrl::OnTVSelChanged(WORD, WORD pnmh, HWND,BOOL& handled) 
{
    int iItem = GetCurSel();
    if(iItem>=0)
    {
        CString szValue;           
        GetLBText(iItem, szValue);
        Process(szValue);
    }

    return S_OK;
}
/**************************************************************************

   CBandComboBoxCtrl::OnTVSelUpdate()
   
**************************************************************************/
LRESULT CBandComboBoxCtrl::OnTVSelUpdate(WORD, WORD pnmh, HWND,BOOL&) 
{
    // Get the text in the edit box
    TCHAR str[200];
    GetWindowText(str, 200);
    int nLength = GetWindowTextLength();

    if (!m_bAuto)
    {
        return 0;
    }

    // Currently selected range
    DWORD dwCurSel = GetEditSel();
    WORD dStart = LOWORD(dwCurSel);
    WORD dEnd = HIWORD(dwCurSel);

    // Search for, and select in, and string in the combo box that is prefixed
    // by the text in the edit box
    if (SelectString(-1, _bstr_t(str)) == CB_ERR)
    {
        SetWindowText(str);		// No text selected, so restore what was there before
        if (dwCurSel != CB_ERR)
            SetEditSel(dStart, dEnd);	// restore cursor postion
    }

    // Set the text selection as the additional text that we have added
    if (dEnd < nLength && dwCurSel != CB_ERR)
        SetEditSel(dStart, dEnd);
    else
        SetEditSel(nLength, -1);

    return 0;
}
/**************************************************************************

   CBandComboBoxCtrl::OnCreate()
   
**************************************************************************/
LRESULT CBandComboBoxCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return S_OK;
}
/**************************************************************************

   CBandComboBoxCtrl::SetDropDownSize()
   
**************************************************************************/
void CBandComboBoxCtrl::SetDropDownSize(UINT LinesToDisplay)
{
    CRect cbSize;			// current size of combo box
    int Height;            // new height for drop-down portion of combo box

    GetClientRect(cbSize);
    Height = GetItemHeight(-1);      // start with size of the edit-box portion
    Height += GetItemHeight(0) * LinesToDisplay;	// add height of lines of text

    // Note: The use of SM_CYEDGE assumes that we're using Windows '95
    // Now add on the height of the border of the edit box
    Height += GetSystemMetrics(SM_CYEDGE) * 2; // top & bottom edges

    // The height of the border of the drop-down box
    Height += GetSystemMetrics(SM_CYEDGE) * 2; // top & bottom edges

    // now set the size of the window
    SetWindowPos(NULL,            // not relative to any other windows
        0, 0,                         // TopLeft corner doesn't change
        cbSize.right, Height,         // existing width, new height
        SWP_NOMOVE | SWP_NOZORDER     // don't move box or change z-ordering.
        );
}

