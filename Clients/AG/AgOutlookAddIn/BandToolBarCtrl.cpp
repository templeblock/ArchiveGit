// BandToolBarCtrl.cpp: implementation of the CBandToolBarCtrl class.
//
//////////////////////////////////////////////////////////////////////
// ============================================================================
// Sample by Rashid Thadha Aug 2001
// ============================================================================

#include "stdafx.h"
#include "BandToolBarCtrl.h"
#include "JMToolband.h"
#include "JMExplorerBand.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// index of buttons
#define JAZZYMAIL_BUTTON		0
#define SEPARATOR_1				1
#define EDIT_CONTROL			2
#define SEARCH_BUTTON			3


CBandToolBarCtrl::CBandToolBarCtrl() : m_RebarContainer(NULL, this, 1), m_bInitialSeparator(false), m_nResourceID(0), m_spWebBrowser(NULL)
{
	//m_hImageList = NULL;
	m_bExplorerbarVisible = false;
}

CBandToolBarCtrl::~CBandToolBarCtrl()
{
	m_spWebBrowser = NULL;

	// Don't forget to UnSubclass the rebar otherwise IE could crash
	m_RebarContainer.UnsubclassWindow();

	m_ImageList.Destroy();
	//ImageList_Destroy(m_hImageList);
    if (::IsWindow(m_hWnd))
        DestroyWindow();
}


/**************************************************************************

   CBandToolBarCtrl::SetToolBand()
   
**************************************************************************/
void CBandToolBarCtrl::SetToolBand(CJMToolband *pToolband)
{
}
/**************************************************************************

   CBandToolBarCtrl::SetWebBrowser()
   
**************************************************************************/
void CBandToolBarCtrl::SetWebBrowser(IWebBrowser2* pWebBrowser)
{
	m_spWebBrowser = pWebBrowser;
	m_ctlBandComboBox.SetWebBrowser(pWebBrowser);
}
/**************************************************************************

   CBandToolBarCtrl::SetToolBarButtonInfo()
   
**************************************************************************/
void CBandToolBarCtrl::SetToolBarButtonInfo(int nID, DWORD dwMask, BYTE fsStyle)
{
    TBBUTTONINFO tbi;	
    tbi.cbSize = sizeof(TBBUTTONINFO);
    tbi.dwMask = dwMask;
    tbi.fsStyle = fsStyle;	
	//tbi.cx = 100;
    SetButtonInfo(nID, &tbi);
}
/**************************************************************************

   CBandToolBarCtrl::OnCreate()
   
**************************************************************************/
HWND CBandToolBarCtrl::CreateToolBarCtrl(HWND hWndParent, RECT &rect, DWORD dwStyle, DWORD dwExStyle)
{
	BOOL bSuccess;

	HWND hWnd = CWindowImpl<CBandToolBarCtrl, CToolBarCtrl>::Create(hWndParent,rect,NULL, dwStyle);

	// Button size
	//bSuccess  = SetButtonSize(CSize(64,22));

	// buttons with images and text
	SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_MIXEDBUTTONS);
	// Sets the size of the TBBUTTON structure.
	SetButtonStructSize(sizeof(TBBUTTON));
	// Set the maximum number of text rows and bitmap size.
	SetMaxTextRows(1);


	// load our button's icon and create the image list to house it.
	HICON hIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_LOGO));
	m_ImageList.Create(16,16, ILC_COLOR32, 1, 0);
	int iImageIndex = m_ImageList.AddIcon(hIcon);
	DestroyIcon(hIcon);
	m_ImageList.SetBkColor(CLR_NONE);


	// Set the toolbar's image
	SetImageList(m_ImageList);

	// add our button's caption to the toolbar window
	int iIndex = AddStrings(_T("Creata Mail\0"));
	iIndex = AddStrings(_T("\0"));
	iIndex = AddStrings(_T("\0"));
	iIndex = AddStrings(_T("Search\0"));

	// add the button for the toolbar to the window
	TBBUTTON Buttons[4];
	Buttons[JAZZYMAIL_BUTTON].idCommand = ID_JMBTN1_CMD;
	Buttons[JAZZYMAIL_BUTTON].fsState = TBSTATE_ENABLED;
	Buttons[JAZZYMAIL_BUTTON].fsStyle = TBSTYLE_DROPDOWN | TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
	Buttons[JAZZYMAIL_BUTTON].dwData = 0;
	Buttons[JAZZYMAIL_BUTTON].iString = JAZZYMAIL_BUTTON;
	Buttons[JAZZYMAIL_BUTTON].iBitmap = 0;

	// Add seperator
	Buttons[SEPARATOR_1].idCommand = 0;
	Buttons[SEPARATOR_1].fsState = 0;
	Buttons[SEPARATOR_1].fsStyle = TBSTYLE_SEP;
	Buttons[SEPARATOR_1].dwData = 0;
	Buttons[SEPARATOR_1].iString = 0;
	Buttons[SEPARATOR_1].iBitmap = -1;

	// Add edit control place holder button
	Buttons[EDIT_CONTROL].idCommand = ID_JMEDIT_CMD;
	Buttons[EDIT_CONTROL].fsState = TBSTATE_ENABLED;;
	Buttons[EDIT_CONTROL].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
	Buttons[EDIT_CONTROL].dwData = 0;
	Buttons[EDIT_CONTROL].iString = EDIT_CONTROL;
	Buttons[EDIT_CONTROL].iBitmap = -1;//I_IMAGENONE;

	// Add search button
	Buttons[SEARCH_BUTTON].idCommand = ID_JMSEARCH_CMD;
	Buttons[SEARCH_BUTTON].fsState = TBSTATE_ENABLED;;
	Buttons[SEARCH_BUTTON].fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;
	Buttons[SEARCH_BUTTON].dwData = 0;
	Buttons[SEARCH_BUTTON].iString = SEARCH_BUTTON;
	Buttons[SEARCH_BUTTON].iBitmap = I_IMAGENONE;

	bSuccess = AddButtons(4, Buttons);

	// Adjust our edit place holder button to the right size.
	TBBUTTONINFO tbi;			
    tbi.cbSize = sizeof(TBBUTTONINFO);		
	tbi.dwMask = TBIF_STYLE | TBIF_SIZE;
	tbi.fsStyle = Buttons[EDIT_CONTROL].fsStyle;
	tbi.cx = (unsigned short)EDIT_WIDTH;
	bSuccess = SetButtonInfo(ID_JMEDIT_CMD, &tbi); 						


   
	// Aditional Initialization
	CClientDC dc(m_hWnd);
	dc.SelectFont((HFONT) GetStockObject( DEFAULT_GUI_FONT ));		
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int cxChar = tm.tmAveCharWidth;
	int cyChar = tm.tmHeight + tm.tmExternalLeading;
	int cx = (LF_FACESIZE + 4) * cxChar;
	int cy = 16 * cyChar;
    m_ncy = cy;

    // Create an Edit Control		
	RECT rc1, rc2, rc3,rc4;
	GetItemRect(SEPARATOR_1, &rc1);
	GetItemRect(SEARCH_BUTTON, &rc3);
	GetItemRect(JAZZYMAIL_BUTTON, &rc4);
	rc2.top = 0;
	rc2.left = rc1.right;
	rc2.right = rc2.left + EDIT_WIDTH;
	rc2.bottom = COMBO_HEIGHT;//cy;

	m_ctlBandComboBox.Create(m_hWnd, rc2, NULL, WS_VSCROLL | CBS_DROPDOWN | WS_CHILD | WS_TABSTOP | WS_VISIBLE | CBS_AUTOHSCROLL);
	m_ctlBandComboBox.SetFont(static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)));
	m_ctlBandComboBox.InsertHistory();


	// Additional Styles should be set here
   // SetToolBarButtonInfo(ID_JMBTN1_CMD, TBIF_STYLE, TBSTYLE_DROPDOWN | Button[JAZZYMAIL_BUTTON].fsStyle);

	
	// make sure the control is where it should be
    ::SendMessage(m_hWnd, WM_SIZE, 0, 0);

	return hWnd;

}

/**************************************************************************

   CBandToolBarCtrl::ShowExplorerbar()
   
**************************************************************************/
bool CBandToolBarCtrl::ShowExplorerbar(LPCTSTR szCLSID, bool bShow)
{
	if (!m_spWebBrowser)
		return false;

	CComVariant varCLSID = CComVariant(szCLSID);
	CComVariant varShow = CComVariant(false);
	CComVariant varSize = VT_NULL;
	if (bShow)
		varShow = CComVariant(true);
	
	HRESULT hr = m_spWebBrowser->ShowBrowserBar(&varCLSID, &varShow, &varSize);
	if (FAILED(hr))
		return false;

	return true;
}
/**************************************************************************

   CBandToolBarCtrl::HandleOptionsButton()
   
**************************************************************************/
bool CBandToolBarCtrl::HandleOptionsButton(CRect &rc)
{

	// If not registered, ignore button clicks and show Register prompt
	if (!VerifyUserStatus())
		return false;

	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(IDR_OPTIONSMENU);
	if (!bOK)
		return false;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return false;

	// Make sure the menu items are set correctly
	WORD wMenuFlags = (m_StationeryMgr.NeedToAdd() ? MF_ENABLED : MF_DISABLED | MF_GRAYED);
	Popup.EnableMenuItem(ID_OPTIONS_CLEARDEFAULTSTATIONERY, MF_BYCOMMAND | wMenuFlags);

	if (m_Authenticate.IsUserPaidMember())
		Popup.RemoveMenu(ID_OPTIONS_UPGRADENOW, MF_BYCOMMAND);

	BOOL bShow = true;
	GetTrayIconStatus(bShow);
	Popup.ModifyMenu(ID_OPTIONS_SHOWTRAYICON, MF_BYCOMMAND, ID_OPTIONS_SHOWTRAYICON, (bShow ? "Hide Tray Icon" : "Show Tray Icon"));

//	POINT Point;
//	::GetCursorPos(&Point);
//	HWND hWnd = ::WindowFromPoint(Point);
//	CRect rRect;
//	::GetWindowRect(hWnd, rRect);
//	Point.x = (m_bExpress ? rRect.left + 625 : rRect.right - 65);  //j Skip the Favorties button for now
////j	Point.x = (m_bExpress ? rRect.left + 715 : rRect.right - 65);
//	Point.y = rRect.bottom - 1;
	WORD wCommand = Popup.TrackPopupMenu(TPM_RETURNCMD|TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON, rc.left, rc.bottom, *this);
	Menu.DestroyMenu();

	if (!wCommand)
		return false;

	return HandleOptionsCommand(wCommand, SOURCE_TOOLBAR);
}
/**************************************************************************

   CBandToolBarCtrl::OnCommand()
   
**************************************************************************/
LRESULT CBandToolBarCtrl::OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//if (!HIWORD(wParam))
	//{
	//	long lCmd = LOWORD(wParam);
	//	if ( lCmd == ID_JMBTN1_CMD)
	//	{
	//		if (m_bExplorerbarVisible)
	//			ShowExplorerbar(CLSID_JMEXPLORERBAND_STRING, false);
	//		else
	//			ShowExplorerbar(CLSID_JMEXPLORERBAND_STRING, true);
	//		
	//		m_bExplorerbarVisible = !m_bExplorerbarVisible;
	//		return 0;
	//	}
	//}
	bHandled = FALSE;
	return 1;
}
/**************************************************************************

   CBandToolBarCtrl::OnJMButton1()
   
**************************************************************************/
LRESULT CBandToolBarCtrl::OnJMButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	if (m_bExplorerbarVisible)
		ShowExplorerbar(CLSID_JMEXPLORERBAND_STRING, false);
	else
		ShowExplorerbar(CLSID_JMEXPLORERBAND_STRING, true);
	
	m_bExplorerbarVisible = !m_bExplorerbarVisible;
	return S_OK;
}
/**************************************************************************

   CBandToolBarCtrl::OnJMSearch()
   
**************************************************************************/
LRESULT CBandToolBarCtrl::OnJMSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString szValue;
    m_ctlBandComboBox.GetWindowText(szValue);
    m_ctlBandComboBox.Process(szValue);
	return S_OK;
}
/**************************************************************************

   CBandToolBarCtrl::OnToolbarDropDown()
   
**************************************************************************/
LRESULT CBandToolBarCtrl::OnToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)	
{
	NMTOOLBAR* ptb = (NMTOOLBAR *) pnmh;		
    if (ptb->iItem == ID_JMBTN1_CMD)
    {
        SetFocus();
		
	    CRect rc;
	    CToolBarCtrl tbar(pnmh->hwndFrom);	
	    tbar.GetItemRect(tbar.CommandToIndex(ptb->iItem), &rc);	
		tbar.MapWindowPoints(HWND_DESKTOP, &rc);
		HandleOptionsButton(rc);
        bHandled = true;
        return 0;
    }
	
    bHandled = false;
    return 0;
}
/**************************************************************************

   CBandToolBarCtrl::OnToolbarNeedText()
   
**************************************************************************/
LRESULT CBandToolBarCtrl::OnToolbarNeedText(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)	
{
    CString sToolTip;    
	//-- make sure this 1is not a seperator
    if (idCtrl != 0) 
    {
        if (!sToolTip.LoadString(idCtrl))
        {
	        bHandled = FALSE;
            return 0;
        }
    }

    LPNMTTDISPINFO pttdi = reinterpret_cast<LPNMTTDISPINFO>(pnmh);

	pttdi->lpszText = MAKEINTRESOURCE(idCtrl);
	pttdi->hinst = _Module.GetResourceInstance();
	pttdi->uFlags = TTF_DI_SETITEM;

	//-- message processed
	return 0;
}
/**************************************************************************

   CBandToolBarCtrl::OnSize()
   
**************************************************************************/
LRESULT CBandToolBarCtrl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	// wParam values: SIZE_MAXIMIZED, SIZE_MINIMIZED, SIZE_RESTORED
	if (wParam == SIZE_MINIMIZED)
		return S_OK;
	
	
    if (m_ctlBandComboBox.m_hWnd)
	{
		// Get the new client size
		int iWidth = LOWORD(lParam);
		int iHeight = HIWORD(lParam);


        CRect rcEdit;
		rcEdit.bottom = iHeight;		
        if (iHeight == 0)
            rcEdit.bottom = m_ncy;
        rcEdit.top = 0;
		CRect rcButton;
		
       //  get the button or separator before the edit control
        GetItemRect(SEPARATOR_1, &rcButton);
		rcEdit.left = rcButton.right;
		rcEdit.right = rcEdit.left + EDIT_WIDTH;
        
       //  remove the next line if you are using an edit control
        rcEdit.bottom = COMBO_HEIGHT;
		m_ctlBandComboBox.MoveWindow(rcEdit);
	}

	return 0;
}
/**************************************************************************

   CBandToolBarCtrl::TranslateAcceleratorIO()
   
**************************************************************************/
STDMETHODIMP CBandToolBarCtrl::TranslateAcceleratorIO(LPMSG pMsg)
{
	if (::GetWindow(m_ctlBandComboBox.m_hWnd, GW_CHILD) == GetFocus())
		return m_ctlBandComboBox.TranslateAcceleratorIO(pMsg);
	
	return S_FALSE;
}
/**************************************************************************

   CBandToolBarCtrl::OnMenuSelect()
   
**************************************************************************/
LRESULT CBandToolBarCtrl::OnMenuSelect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{ 
    WORD nID = LOWORD(wParam);
    WORD wFlags = HIWORD(wParam);

    //-- make sure this is not a seperator
    CString sStatusBarDesc;

    if ( !(wFlags & MF_POPUP) )
    {
        if (nID != 0) 
        {
            if (!sStatusBarDesc.LoadString(nID))
            {
	            bHandled = FALSE;
                return 0;
            }

            int nPos = sStatusBarDesc.Find(_T("\n"));
            if (nPos != -1)
            {
                sStatusBarDesc = sStatusBarDesc.Left(nPos+1);
                m_spWebBrowser->put_StatusText(_bstr_t(sStatusBarDesc));
                return 0;
            }
        }
    }

    return 0;
}
/**************************************************************************

   CBandToolBarCtrl::UpdateToolBarElement()
   
**************************************************************************/
void CBandToolBarCtrl::UpdateToolBarElement(int nID, bool bEnable)
{
	// Note: only handles enabled/disabled, checked state, and radio (press)
	TBBUTTONINFO tbi;	
				
	GetButtonInfo(nID, &tbi); 
	tbi.fsState ^= TBSTATE_ENABLED;
    SetButtonInfo(nID, &tbi);
}
/**************************************************************************

   CBandToolBarCtrl::FindRebar()
   
**************************************************************************/
HWND CBandToolBarCtrl::FindRebar(HWND hwndStart)
{
	HWND hwndFound=NULL;
	if (((hwndFound = 
		FindWindowEx(hwndStart, NULL, REBARCLASSNAME, NULL)) != NULL))
		return hwndFound;

	HWND hwndEnum=NULL;
	while ((hwndEnum = FindWindowEx(hwndStart, hwndEnum, NULL, NULL)) != NULL){
		if ((hwndFound = FindRebar(hwndEnum)) != NULL)
			return hwndFound;
	}

	return NULL;
}
/**************************************************************************

   CBandToolBarCtrl::SetBandRebar()
   
**************************************************************************/
BOOL CBandToolBarCtrl::SetBandRebar() 
{
	HWND hWnd(NULL);
	m_spWebBrowser->get_HWND((long*)&hWnd);
	
	if (hWnd == NULL) 
		return FALSE;

	m_ctlRebar.m_hWnd = FindRebar(hWnd);
	if (m_ctlRebar.m_hWnd == NULL) 
		return FALSE;

	m_RebarContainer.SubclassWindow(m_ctlRebar);

	return TRUE;
}
