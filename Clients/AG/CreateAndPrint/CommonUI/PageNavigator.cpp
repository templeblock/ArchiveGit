#include "stdafx.h"
#include "PageNavigator.h"
#include "AGPage.h"
#include "MessageBox.h"
#include "commctrl.h"
#include "Image.h"
#include "Ctp.h"

#define THUMBNAIL_HT	28
#define THUMBNAIL_WT	97
#define THUMBNAIL_GAP	2

static int idList_PortraitCards1[]  = { IDR_PAGE_CP1,  IDR_PAGE_CP2,  IDR_PAGE_CP3,  IDR_PAGE_CP4,  IDR_PAGE_P1  };
static int idList_PortraitCards2[]  = { IDR_PAGE_CP1S, IDR_PAGE_CP2S, IDR_PAGE_CP3S, IDR_PAGE_CP4S, IDR_PAGE_P1S };
static int idList_LandscapeCards1[] = { IDR_PAGE_CL1,  IDR_PAGE_CL2,  IDR_PAGE_CL3,  IDR_PAGE_CL4,  IDR_PAGE_L1  };
static int idList_LandscapeCards2[] = { IDR_PAGE_CL1S, IDR_PAGE_CL2S, IDR_PAGE_CL3S, IDR_PAGE_CL4S, IDR_PAGE_L1S };
static int idList_PortraitPages1[]  = { IDR_PAGE_P1  }; 
static int idList_PortraitPages2[]  = { IDR_PAGE_P1S };
static int idList_LandscapePages1[] = { IDR_PAGE_L1  };
static int idList_LandscapePages2[] = { IDR_PAGE_L1S };

//////////////////////////////////////////////////////////////////////
CPageNavigator::CPageNavigator(CCtp* pMainWnd)
{
	m_pCtp = pMainWnd;

	m_hWndPrevButton = NULL;
	m_hWndNextButton = NULL;
	m_nPage = -1;
	m_dxIcon = THUMBNAIL_WT;
	m_dyIcon = THUMBNAIL_HT;
	m_pAGDoc = NULL;
	m_yTop = 0;
	m_xLeft = 0;
	m_bCapture = false;
	m_bDown = false;
	m_bIsCard = false;
}

//////////////////////////////////////////////////////////////////////
CPageNavigator::~CPageNavigator()
{
	m_ImageList.Destroy();
}

//////////////////////////////////////////////////////////////////////
void CPageNavigator::InitDialog(HWND hWndListCtrl, HWND hWndLabel, HWND hWndPrevButton, HWND hWndNextButton)
{
	bool bHpUI = m_pCtp->IsHpUI();
	COLORREF rgbLabelText = (bHpUI ? RGB(255,255,255) : RGB(0,0,0));
	COLORREF rgbLabelBackground = (bHpUI ? RGB(147,165,182) : RGB(171,170,202));
	LPCSTR pFont = (bHpUI ? "Arial" : "Verdana");
	bool bBold = (bHpUI ? false : true);
	int iSize = (bHpUI ? 14 : 12);

	if (hWndListCtrl)
		SubclassWindow(hWndListCtrl);

	if (hWndLabel)
	{
		m_LabelPage.InitDialog(hWndLabel);
		m_LabelPage.SetTextColor(rgbLabelText);
		m_LabelPage.SetBackgroundColor(rgbLabelBackground);
		m_LabelPage.SetFontName(pFont);
		m_LabelPage.SetFontBold(bBold);
		m_LabelPage.SetFontSize(iSize);
	}

	m_hWndPrevButton = hWndPrevButton;
	m_hWndNextButton = hWndNextButton;
}

//////////////////////////////////////////////////////////////////////
static void DrawTextOnBitmap(HBITMAP hBitmap, LPCSTR pText, int iSize, COLORREF rgb, int iOffsetX, int iOffsetY)
{
	HDC hRawDC = ::GetDC(NULL);
	HDC hDC = ::CreateCompatibleDC(hRawDC); // Create the offscreen memory DC
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, rgb);
	LOGFONT Font;
	::ZeroMemory(&Font, sizeof(Font));
	Font.lfWeight = FW_BOLD;
	Font.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET
	Font.lfHeight = iSize;
	lstrcpy(Font.lfFaceName, "Verdana");
	HFONT hFont = ::CreateFontIndirect(&Font);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, hBitmap);
	BITMAP bm;
	::GetObject(hBitmap, sizeof(bm), &bm);
	RECT Rect = {iOffsetX + 0, iOffsetY + 0, iOffsetX + bm.bmWidth-1, iOffsetY + bm.bmHeight-1};
	int nOptions = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
	::DrawText(hDC, pText, -1, &Rect, nOptions);
	::SelectObject(hDC, hOldBitmap);
	::SelectObject(hDC, hOldFont);
	::DeleteObject(hFont);
	::DeleteDC(hDC);
	::ReleaseDC(NULL, hRawDC);
}

//////////////////////////////////////////////////////////////////////
void CPageNavigator::ActivateNewDoc(CAGDoc* pAGDoc, int nDefaultPage)
{
	if (!m_hWnd)
		return;
		
	m_pAGDoc = pAGDoc;
	m_nPage = -1;

	// Stop all window updates to avoid flickering
	SetRedraw(false);

	// Remove any previously loaded items from the list control
	DeleteAllItems();

	// Detach the image list from the list control
	RemoveImageList(LVSIL_NORMAL);

	// Destroy the image list
	m_ImageList.Destroy();

	// Get out if no document to attach to
	if (!m_pAGDoc)
		return;

	::SendMessage(m_hWnd, LVM_SETBKCOLOR, (WPARAM)0, (LPARAM)(COLORREF)RGB(241,243,245));

	// Re-create the image list at the proper size
	bool bPortrait = m_pAGDoc->IsPortrait();
	m_dxIcon = THUMBNAIL_WT;
	m_dyIcon = THUMBNAIL_HT;
	bool bOK = !!m_ImageList.Create(m_dxIcon, m_dyIcon, ILC_COLOR24, 1/*nInitial*/, 1/*nGrow*/);

	// Determine the document appropriate images to add to the image list
	AGDOCTYPE DocType = m_pAGDoc->GetDocType();
	int nImages = 0;
	int* pidList1 = NULL;
	int* pidList2 = NULL;
	m_bIsCard = false;
	switch (DocType)
	{
		case DOC_BANNER:
		case DOC_BROCHURE:
		case DOC_BUSINESSCARD:
		case DOC_CDLABEL:
		case DOC_ENVELOPE:
		case DOC_FULLSHEET:
		case DOC_LABEL:
		case DOC_POSTCARD:
		case DOC_IRONON:
		case DOC_GIFTNAMECARD:
		case DOC_TRIFOLD:
		case DOC_HOLIDAYCARD:
		case DOC_PHOTOCARD:
		default:
		{
			nImages = (bPortrait ? sizeof(idList_PortraitPages1) : sizeof(idList_LandscapePages1)) / sizeof(int);
			pidList1 = (bPortrait ? idList_PortraitPages1 : idList_LandscapePages1);
			pidList2 = (bPortrait ? idList_PortraitPages2 : idList_LandscapePages2);
			m_bIsCard = false;
			break;
		}
		case DOC_HALFCARD:
		case DOC_QUARTERCARD:
		case DOC_NOTECARD:
		case DOC_CDBOOKLET:
		{
			nImages = (bPortrait ? sizeof(idList_PortraitCards1) : sizeof(idList_LandscapeCards1)) / sizeof(int);
			pidList1 = (bPortrait ? idList_PortraitCards1 : idList_LandscapeCards1);
			pidList2 = (bPortrait ? idList_PortraitCards2 : idList_LandscapeCards2);
			m_bIsCard = true;
			break;
		}
	}
	
	int nPages = m_pAGDoc->GetNumPages();

	// Add the document appropriate images to the image list
	for (int nPass = 0; nPass < 2; nPass++)
	{
		for (int i = 0; i < nPages; i++)
		{
			int iIcon = (i < nImages ? i : nImages-1);
			int* pidList = (nPass == 0 ? pidList1 : pidList2);
			CImage Image(_AtlBaseModule.GetResourceInstance(), pidList[iIcon], "GIF");
			HBITMAP hBitmap = Image.GetBitmapHandle();
//j			HBITMAP hBitmap = ::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(pidList[iIcon]));
			if (!m_bIsCard)
			{
				CString strNumber;
				strNumber.Format("%d", i+1);
				DrawTextOnBitmap(hBitmap, strNumber, 16, RGB(0,0,0), 0, 2);
			}
			else
			if (i >= 4)
			{
				CString strNumber;
				strNumber.Format("+%d", i-3);
				DrawTextOnBitmap(hBitmap, strNumber, 16, RGB(0,0,0), 0, 2);
			}

			m_ImageList.Add(hBitmap);
//j			::DeleteObject(hBitmap);
		}
	}
	
	// Attach the image list to the list control
	SetImageList(m_ImageList, LVSIL_NORMAL);

	// Set the icon spacing arbitrarily large to ensure that our icons are placed where we want them
	SetIconSpacing(1000, 1000);

	// Compute the spacing required on the top
	RECT ClientRect = {0,0,0,0};
	GetClientRect(&ClientRect);
	m_yTop = (HEIGHT(ClientRect) - m_dyIcon) / 2;

	// Compute the spacing required on the left
	if (!m_bIsCard)
		m_xLeft = 2;
	else
	{
		int nIcons = 4; //(bPortrait ? 4 : 3);
		m_xLeft = (WIDTH(ClientRect) - (nIcons * m_dxIcon) - (2 * THUMBNAIL_GAP)) / 2;
	}

	// Insert the new list items
	int xLocation = m_xLeft;
	for (int i = 0; i < nPages; i++)
	{
		InsertItem(i/*nItem*/, "", i/*nImage*/);

		// shift the thumbnail to desired position
		int yCorrection = 0;

		POINT pt = {xLocation, m_yTop + yCorrection};
		SetItemPosition(i, pt);
		xLocation += (m_dxIcon + THUMBNAIL_GAP);
	}

	SelectPage(nDefaultPage, true/*bSetState*/);
	SetRedraw(true); 
}

//////////////////////////////////////////////////////////////////////
void CPageNavigator::SetPage(int nPage)
{
	SelectPage(nPage, true/*bSetState*/);
}

//////////////////////////////////////////////////////////////////////
int CPageNavigator::GetPage()
{
	return m_nPage;
}

//////////////////////////////////////////////////////////////////////
bool CPageNavigator::AddPage(LPCSTR strPageName)
{
	ActivateNewDoc(m_pAGDoc, m_nPage);
	return false;
}

//////////////////////////////////////////////////////////////////////
bool CPageNavigator::DeleteCurrentPage()
{
	ActivateNewDoc(m_pAGDoc, m_nPage);
	return false;
}

//////////////////////////////////////////////////////////////////////
int CPageNavigator::ChangePage(int iDelta)
{
	if (!m_hWnd || !m_pAGDoc)
		return -1;
		
	int nPage = m_nPage + iDelta;
	int nPages = m_pAGDoc->GetNumPages();
	bool bHasAddItem = m_ImageList.GetImageCount() & 1; // If the image count is odd
	if (bHasAddItem && nPage == nPages)
	{
		// Scroll the add button into position, if necessary
		EnsureIconVisible(nPage);
		return -1;
	}

	if (nPage < 0 || nPage >= nPages)
		return -1;

	SetRedraw(false); 
	SelectPage(nPage, true/*bSetState*/);
	SetRedraw(true); 
	return m_nPage; // If successful, this is set in SelectPage()
}

//////////////////////////////////////////////////////////////////////
LRESULT CPageNavigator::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bCapture)
		return S_OK;
		
	SetCapture();
	m_bCapture = true;
	m_bDown = true;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPageNavigator::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bCapture)
		return S_OK;
		
	ReleaseCapture();
	m_bCapture = false;

	if (!m_bDown)
		return S_OK;
		
	m_bDown = false;

	int nPage = -1;
	POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	for (int i = 0; i < GetItemCount(); i++)
	{
		RECT ItemRect = {0,0,0,0};
		GetIconRect(i, &ItemRect);
		if (::PtInRect(&ItemRect, ptCursor))
		{
			nPage = i;
			break;
		}
	}

	if (::GetFocus() != m_hWnd)
		::SetFocus(m_hWnd);

	if (nPage < m_pAGDoc->GetNumPages())
	{
		int nLastPage = GetPage();
		if (SelectPage(nPage, true/*bSetState*/))
			SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED/*wNotifyCode*/), MAKELPARAM(nLastPage,nPage));
	}
	else
	{ // It must be the add button
		SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDC_DOC_PAGEADD, BN_CLICKED/*wNotifyCode*/), 0L);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPageNavigator::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bCapture)
		return S_OK;

	POINT ptCursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	ClientToScreen(&ptCursor);
	CRect WindowRect;
	GetWindowRect(WindowRect);
	if (m_bDown != !!WindowRect.PtInRect(ptCursor))
		m_bDown = !m_bDown;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPageNavigator::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(ID_HAND)));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPageNavigator::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPageNavigator::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return S_OK;
}

#ifdef NOTUSED //j
//////////////////////////////////////////////////////////////////////
LRESULT CPageNavigator::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_hWnd || !m_pAGDoc)
		return E_FAIL;
		
	NMHDR* pNMHDR = (NMHDR*)lParam;

	// don't handle messages from odd places
	if (pNMHDR->hwndFrom != m_hWnd && pNMHDR->hwndFrom != ::GetParent(m_hWnd))
	{
		bHandled = false;
		return 1;
	}

	LRESULT lResult = 0;
	switch (pNMHDR->code)
	{
		case NM_CLICK:
		{
			NMITEMACTIVATE* pItemAct = (NMITEMACTIVATE*)pNMHDR;
			if (!pItemAct)
				break;

			int nPage = -1;
			for (int i = 0; i < GetItemCount(); i++)
			{
				RECT ItemRect = {0,0,0,0};
				GetIconRect(i, &ItemRect);
				if (::PtInRect(&ItemRect, pItemAct->ptAction))
				{
					nPage = i;
					break;
				}
			}

			if (nPage < m_pAGDoc->GetNumPages())
			{
				int nLastPage = GetPage();
				if (SelectPage(nPage, false/*bSetState*/))
					SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED/*wNotifyCode*/), MAKELPARAM(nLastPage,nPage));
			}
			else
			{ // It must be the add button
				SendMessage(GetParent(), WM_COMMAND, MAKEWPARAM(IDC_DOC_PAGEADD, BN_CLICKED/*wNotifyCode*/), 0L);
			}

			break;
		}

		case NM_SETFOCUS:
		case NM_KILLFOCUS:
		case LVN_KEYDOWN:
		case LVN_ITEMCHANGING:
		case LVN_ITEMCHANGED:
		case LVN_INSERTITEM:
		case LVN_DELETEITEM:
		case LVN_DELETEALLITEMS:
		case LVN_ITEMACTIVATE: // happens on a double-click
		case LVN_BEGINDRAG:
		{
			bHandled = false;	// not handled
			break;
		}

		// These message do not occur normally
		case LVN_BEGINRDRAG:
		case LVN_COLUMNCLICK:
		case LVN_ODCACHEHINT:
		case LVN_ODSTATECHANGED:
		case LVN_ODFINDITEM:
		case LVN_HOTTRACK:
		case LVN_BEGINLABELEDIT:
		case LVN_ENDLABELEDIT:
		case LVN_GETDISPINFO:
		case LVN_SETDISPINFO:
		case LVN_MARQUEEBEGIN:
		case LVN_GETINFOTIP:
		default:
		{
			bHandled = false;	// not handled
			break;
		}
	}

	return lResult;
}
#endif NOTUSED //j

//////////////////////////////////////////////////////////////////////
bool CPageNavigator::SelectPage(int nPage, bool bSetState)
{
	if (!m_hWnd || !m_pAGDoc)
		return false;

	int nPages = m_pAGDoc->GetNumPages();
	if (nPage < 0 || nPage >= nPages)
		return false;

	if (m_nPage == nPage)
		return false;

	// Reset the image of the old selected page
	if (m_nPage >= 0)
	{
		LV_ITEM lvItem;
		::ZeroMemory(&lvItem, sizeof(lvItem));
		lvItem.mask = LVIF_IMAGE;
		lvItem.iItem = m_nPage;
		GetItem(&lvItem);
		if (lvItem.iImage >= nPages)
		{
			lvItem.mask = LVIF_IMAGE;
			lvItem.iImage -= nPages;
			SetItem(&lvItem);
		}
	}

	m_nPage = nPage;

	// Set the image of the newly selected page
	if (m_nPage >= 0)
	{
		LV_ITEM lvItem;
		::ZeroMemory(&lvItem, sizeof(lvItem));
		lvItem.mask = LVIF_IMAGE;
		lvItem.iItem = m_nPage;
		GetItem(&lvItem);
		if (lvItem.iImage < nPages)
		{
			lvItem.mask = LVIF_IMAGE;
			lvItem.iImage += nPages;
			SetItem(&lvItem);
		}
	}

	// Set the page name into the static label control
	CString strPageName;
	CAGPage* pPage = m_pAGDoc->GetPage(m_nPage);
	if (pPage)
		pPage->GetPageName(strPageName);

	if (::IsWindow(m_LabelPage.m_hWnd))
		m_LabelPage.SetText(strPageName);

	// Scroll the page into position, if necessary
	EnsureIconVisible(m_nPage);

	// Select the list control item
	if (bSetState)
		SetItemState(m_nPage, LVIS_SELECTED, LVIS_SELECTED|LVIS_FOCUSED);

	// Show or hide the next/prev buttons as necessary
	bool bNormalCard = m_bIsCard && (nPages <= 4);
	int nCmdShowPrev = (bNormalCard ? SW_HIDE : (nPage <= 0 ? SW_HIDE : SW_SHOW));
	int nCmdShowNext = (bNormalCard ? SW_HIDE : (nPage >= nPages - 1 ? SW_HIDE : SW_SHOW));
	::ShowWindow(m_hWndPrevButton, nCmdShowPrev);
	::ShowWindow(m_hWndNextButton, nCmdShowNext);

	return true;
}

//////////////////////////////////////////////////////////////////////
void CPageNavigator::GetIconRect(int nItem, RECT* pRect)
{
	if (!pRect)
		return;
		
	POINT pt = {0,0};
	GetItemPosition(nItem, &pt);

	pRect->left   = pt.x;
	pRect->top    = pt.y;
	pRect->right  = pRect->left + m_dxIcon;
	pRect->bottom = pRect->top  + m_dyIcon;

	RECT TestItemRect = {0,0,0,0};
	bool bOK = !!GetItemRect(nItem, &TestItemRect, LVIR_ICON);
}

//////////////////////////////////////////////////////////////////////
void CPageNavigator::EnsureIconVisible(int nItem)
{
	RECT ItemRect = {0,0,0,0};
	GetIconRect(nItem, &ItemRect);
	RECT ClientRect = {0,0,0,0};
	GetClientRect(&ClientRect);

	int iDelta = 0;
	int dx = (ClientRect.left + m_xLeft) - ItemRect.left;
	if (dx > 0)
		iDelta = dx;
	dx = (ClientRect.right - m_xLeft) - ItemRect.right;
	if (dx < 0)
		iDelta = dx;
	if (!iDelta)
		return;
		
	for (int i = 0; i < GetItemCount(); i++)
	{
		POINT pt = {0,0};
		GetItemPosition(i, &pt);
		pt.x += iDelta;
		SetItemPosition(i, pt);
	}
}
