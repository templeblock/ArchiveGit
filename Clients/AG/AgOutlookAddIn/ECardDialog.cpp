#include "stdafx.h"
#include "ECardDialog.h"

struct LBITEMINFOTYPE
{
	int	BitmapID;
	int TextID;
	int UrlID;
};
const UINT LB_ITEM_COUNT = 2;
const LBITEMINFOTYPE LB_ITEM[LB_ITEM_COUNT] =  {{IDB_BLUE_16,		IDS_ECARD_SITE1,	IDS_ECARD_URL1},	// index = 0
												{IDB_EGREETS_16,	IDS_ECARD_SITE2,	IDS_ECARD_URL2}};	// index = 1
											  //{IDB_AG_16,	        IDS_ECARD_SITE3,	IDS_ECARD_URL3},	// index = 2
											  //{IDB_BEATGREETS_16, IDS_ECARD_SITE4,	IDS_ECARD_URL4}};	// index = 3															

/////////////////////////////////////////////////////////////////////////////
CECardDialog::CECardDialog()
{
}

/////////////////////////////////////////////////////////////////////////////
CECardDialog::~CECardDialog()
{
	if (::IsWindow(m_hWnd))
		DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CECardDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// list box
	m_ECardList.SubclassWindow(GetDlgItem(IDC_ECARD_LIST));
	m_ECardList.ModifyStyle(NULL, LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SINGLESEL);
	m_ECardList.SetExtendedListViewStyle(m_ECardList.GetExtendedListViewStyle()| LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE);
	InitListBox();
	PositionWindow();
	ShowWindow(SW_SHOW);
    return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
bool CECardDialog::InitListBox()
{	
	if (m_ImageList)
		m_ImageList.Destroy();

	if (!m_ImageList.Create(16,16, ILC_COLOR24 | ILC_MASK, LB_ITEM_COUNT, LB_ITEM_COUNT))
		return false;

	// Add LISTBOX images to image list
	int nIndex=0;
	for (int i=0; i<LB_ITEM_COUNT; i++)
	{
		nIndex = AddImage(LB_ITEM[i].BitmapID);
		if (nIndex < 0)
			return false;
	}
	
	// Give Image list to list ctrl.
	m_ECardList.SetImageList(m_ImageList.m_hImageList, LVSIL_SMALL);

	// Add LISTBOX items
	m_ECardList.InsertColumn( 0, _T("ECARDS"), LVCFMT_LEFT, 100, -1 );
	CString szText;
	for (int i=0; i<LB_ITEM_COUNT; i++)
	{
		szText.LoadString(LB_ITEM[i].TextID);
		m_ECardList.InsertItem(i,szText, i);
	}

	m_ECardList.SetColumnWidth (0, LVSCW_AUTOSIZE);
	m_ECardList.SetHoverTime(10);

	return true;
}
///////////////////////////////////////////////////////////////////////////////
int CECardDialog::AddImage(DWORD dwID)
{
	if (dwID == -1)
		return 0x8000;

	CBitmap bmp;
	bmp.LoadBitmap(dwID);
	HBITMAP hBitmapMask = ReplaceBitmapColor(bmp.m_hBitmap, CLR_DEFAULT, RGB(255,255,255), RGB(0,0,0));

	return m_ImageList.Add(bmp.m_hBitmap, hBitmapMask);
}
///////////////////////////////////////////////////////////////////////////////
bool CECardDialog::Display(HWND hParent)
{
	if (!::IsWindow(m_hWnd))
	{
		HWND hWndParent = hParent;
		if (NULL == hWndParent)
			hWndParent= ::GetDesktopWindow();
		if (!Create(hWndParent))
			return false;
	}

	PositionWindow();
	ShowWindow(SW_SHOW);
	
	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CECardDialog::PositionWindow()
{
	HWND hParent = GetParent();
	POINT ptCursor;
	::GetCursorPos(&ptCursor);
	
	RECT rParent;
	::GetWindowRect(hParent, &rParent);
	ClientToScreen(&rParent);

	RECT rRect;
	GetWindowRect(&rRect);
	ClientToScreen(&rRect);

	POINT ptNew;
	if (ptCursor.x < (rParent.left + rParent.right)/2)
		ptNew.x = ptCursor.x;
	else
		ptNew.x = ptCursor.x - WIDTH(rRect);

	if (ptCursor.y < (rParent.top + rParent.bottom)/2)
		ptNew.y = ptCursor.y + 10;
	else
		ptNew.y = ptCursor.y - HEIGHT(rRect) - 10;
	
	SetWindowPos(HWND_TOPMOST, ptNew.x, ptNew.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CECardDialog::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ((LOWORD(wParam) == WA_INACTIVE))
		ShowWindow(SW_HIDE);
	
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////
LRESULT CECardDialog::OnItemActivate(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMITEMACTIVATE pNMITEMACTIVATE = (LPNMITEMACTIVATE)pnmh; 
	int nIndex = pNMITEMACTIVATE->iItem;
	if (nIndex < 0 || nIndex >= LB_ITEM_COUNT)
		return S_OK;
	
	// hide ecard dialog - performance is faster then destroying
	ShowWindow(SW_HIDE);

	// Open IE with appropo url.
	CString szUrl;
	szUrl.LoadString(LB_ITEM[nIndex].UrlID);
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
	
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////
LRESULT CECardDialog::OnKillFocus(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ShowWindow(SW_HIDE);
	return S_OK;
}
