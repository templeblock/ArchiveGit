//#pragma warning ( disable : 4786 )

#include "stdafx.h"
#include "MenuSuper.h"

const CMenuSuper::DEFAULT_COLUMN_COUNT = 1;
const CMenuSuper::DEFAULT_COLUMN_WIDTH = 50;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMenuSuper::CMenuSuper(HMENU hMenu)
: CMenu(hMenu),
  m_pImageList(NULL),
  m_bUseImage(TRUE)
{
	m_vecColumnWidth.resize(DEFAULT_COLUMN_COUNT);
	for (int i=0; i<DEFAULT_COLUMN_COUNT;i++)
	{
		m_vecColumnWidth[i] = DEFAULT_COLUMN_WIDTH;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMenuSuper::~CMenuSuper()
{
	Cleanup();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMenuSuper::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (GetMenuItemCount() == 0 || (int)lpDrawItemStruct->itemID == CB_ERR) return;

	ItemData* pItemData = NULL;

	if (lpDrawItemStruct->itemData != NULL && lpDrawItemStruct->itemData != CB_ERR)
	{
		pItemData = (ItemData*)lpDrawItemStruct->itemData;
	}

	CString str;
	GetMenuString(lpDrawItemStruct->itemID, str, MF_BYCOMMAND);
	CDCHandle dc;
	BOOL bSelected = FALSE;

	dc.Attach(lpDrawItemStruct->hDC);

	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();

	MENUITEMINFO menuInfo;
	GetMenuItemInfo(lpDrawItemStruct->itemID, false, &menuInfo);
		

	if(menuInfo.fType & MFT_SEPARATOR)
	{
		// draw separator
		RECT rc = lpDrawItemStruct->rcItem;
		rc.top += (rc.bottom - rc.top) / 2;	// vertical center
		dc.DrawEdge(&rc, EDGE_ETCHED, BF_TOP);	// draw separator line
	}
	else // not a separator
	{
		// If this item is selected, set the background color and the text color to appropriate 
		// values. Erase the rect by filling it with the background color.
		if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
			(lpDrawItemStruct->itemState & ODS_SELECTED))
		{
			dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
			bSelected = TRUE;
		}
		else
		{
			dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
		}

		CRect rect(lpDrawItemStruct->rcItem);
		rect.DeflateRect(1,0);

		// If we use images, and there is data, and the index is valid:
		if (m_pImageList && m_bUseImage && pItemData && pItemData->nImageIndex!=-1)
		{
			DrawIconEx(dc.m_hDC,rect.left,rect.top, m_pImageList->ExtractIcon(pItemData->nImageIndex),0, 0, 0, NULL, DI_NORMAL);
		}
		

		// If we use images - move text to the right:
		if (m_bUseImage && m_pImageList)
		{
			IMAGEINFO sImageInfo;
			m_pImageList->GetImageInfo(0, &sImageInfo);
			rect.left += sImageInfo.rcImage.right;
		}

		CFontHandle OldFontHandle;
		CFont boldFont;
		if (pItemData && pItemData->bBold)
		{
			CFontHandle curFont(dc.GetCurrentFont());
			LOGFONT lf;
			curFont.GetLogFont(&lf);
			lf.lfWeight = FW_BOLD;
			boldFont.CreateFontIndirect(&lf);
			OldFontHandle = dc.SelectFont(boldFont.m_hFont);
		}

		// If the item has its own color, replace text color (exception - color is black, and
		// the item is selected - then we leave the highlight text color)
		if (pItemData && (!bSelected || (bSelected && pItemData->crTextColor != RGB(0,0,0))))
		{
			dc.SetTextColor(pItemData->crTextColor);
		}

		// If we need to display columns - a bit more complicated...
		if (m_vecColumnWidth.size()>1)
		{
			CPen linePen;
			linePen.CreatePen(PS_SOLID, 0, RGB(192,192,192));
			CPenHandle OldPenHandle = dc.SelectPen(linePen.m_hPen);
			int nCurX=0;
			for (UINT i=0; i<m_vecColumnWidth.size(); i++)
			{
				if (i!=m_vecColumnWidth.size()-1)
				{
					dc.MoveTo(rect.left+nCurX+m_vecColumnWidth[i],rect.top);
					dc.LineTo(rect.left+nCurX+m_vecColumnWidth[i],rect.bottom);
				}
				CRect rc(rect);
				rc.left=rect.left+nCurX+1;
				if (i!=m_vecColumnWidth.size()-1)
				{
					rc.right=rect.left+nCurX+m_vecColumnWidth[i];
				}
				else
				{
					rc.right = rect.right;
				}
				nCurX += m_vecColumnWidth[i];

				if (i==0)
				{
					dc.DrawText(str, -1, &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
				}
				else if (pItemData)
				{
					if (pItemData->mapStrings.find(i)!=pItemData->mapStrings.end())
					{
						dc.DrawText(pItemData->mapStrings[i], -1, &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
					}
				}
			}

			dc.SelectPen(OldPenHandle);
		}
		else
		{
			// Normal one column text display:
			dc.DrawText(str, -1, &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER);
		}

		if (pItemData && pItemData->bBold)
		{
			dc.SelectFont(OldFontHandle);
			//boldFont.DeleteObject();
		}
	}
	

	// Reset the background color and the text color back to their original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);

	dc.Detach();
}
void CMenuSuper::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	MENUITEMINFO menuInfo;
	GetMenuItemInfo(lpMeasureItemStruct->itemID, false, &menuInfo);
		

	if(menuInfo.fType & MFT_SEPARATOR)
	{
		lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYMENU) / 2;
		lpMeasureItemStruct->itemWidth  = 0;
	}
	else
	{
		
		NONCLIENTMETRICS info;
		info.cbSize = sizeof(info);
		::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
		HFONT hFontMenu = ::CreateFontIndirect(&info.lfMenuFont);
		CFontHandle curFont;
		curFont.Attach(hFontMenu);

		// compute size of text - use DrawText with DT_CALCRECT
		CWindowDC dc(NULL);
		HFONT hOldFont;
		if(menuInfo.fState & MFS_DEFAULT)
		{
			// need bold version of font
			LOGFONT lf;
			curFont.GetLogFont(&lf);
			lf.lfWeight += 200;
			CFont fontBold;
			fontBold.CreateFontIndirect(&lf);
			ATLASSERT(fontBold.m_hFont != NULL);
			hOldFont = dc.SelectFont(fontBold);
		}
		else
		{
			hOldFont = dc.SelectFont(curFont);
		}

		CString szItem;
		GetMenuString(lpMeasureItemStruct->itemID, szItem, MF_BYCOMMAND);
		RECT rcText = { 0, 0, 0, 0 };
		dc.DrawText(szItem, -1, &rcText, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_CALCRECT);
		int cx = rcText.right - rcText.left;
		dc.SelectFont(hOldFont);

		LOGFONT lf;
		curFont.GetLogFont(lf);
		int cy = lf.lfHeight;
		if(cy < 0)
			cy = -cy;
		cy += 8;

		// height of item is the bigger of these two
		lpMeasureItemStruct->itemHeight = cy;//(cy, (int)m_szButton.cy);

		// width is width of text plus a bunch of stuff
		//cx += 2 * s_kcxTextMargin;	// L/R margin for readability
		//cx += s_kcxGap;			// space between button and menu text
		//cx += 2 * m_szButton.cx;	// button width (L=button; R=empty margin)

		// Windows adds 1 to returned value
		cx -= ::GetSystemMetrics(SM_CXMENUCHECK) - 1;
		lpMeasureItemStruct->itemWidth = cx;		// done deal
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMenuSuper::SetItemBold(int nItemIndex,  BOOL bBold)
{
	ItemData* pItemData = GetOrCreateItemData(nItemIndex);
	if (pItemData)
	{
		pItemData->bBold = bBold;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString CMenuSuper::GetItemText(int nItemIndex,int nColumn)
{
	if (0==nColumn)
	{
		CString str;
		GetMenuString(nItemIndex, str, MF_BYPOSITION);
		return str;
	}

	ItemData* pItemData = GetOrCreateItemData(nItemIndex);
	if (pItemData)
	{
		return pItemData->mapStrings[nColumn];
	}
	else
	{
		return "";
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMenuSuper::SetItemImage(int nItemIndex, int nImageIndex)
{
	ItemData* pItemData = GetOrCreateItemData(nItemIndex);
	if (pItemData)
	{
		pItemData->nImageIndex = nImageIndex;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMenuSuper::SetItemColor(int nItemIndex, COLORREF rcTextColor)
{
	ItemData* pItemData = GetOrCreateItemData(nItemIndex);
	if (pItemData)
	{
		pItemData->crTextColor = rcTextColor;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMenuSuper::SetItemText(int nItemIndex, int nColumn, CString str)
{
	ItemData* pItemData = GetOrCreateItemData(nItemIndex);
	if (pItemData)
	{
		pItemData->mapStrings[nColumn] = str;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMenuSuper::SetColumnWidth(int nColumnIndex, int nWidth)
{
	if (nColumnIndex<0 || (UINT)nColumnIndex >= m_vecColumnWidth.size()) return;

	m_vecColumnWidth[nColumnIndex] = nWidth;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMenuSuper::ItemData* CMenuSuper::GetOrCreateItemData(int nItemIndex)
{
	if (nItemIndex<0 || nItemIndex>=GetMenuItemCount()) return NULL;

	CMenuItemInfo mii;
	if (!GetMenuItemInfo(nItemIndex, TRUE, &mii))
		return NULL;

	ItemData* pItemData = (ItemData*)mii.dwItemData;

	if (NULL == pItemData || CB_ERR == (int)pItemData)
	{
		pItemData = new ItemData;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_DATA;
		mii.dwItemData = (ULONG_PTR)pItemData;
		if (!SetMenuItemInfo(nItemIndex, TRUE, &mii))
		{
			delete pItemData;
			pItemData = NULL;
		}
	}
	
	return pItemData;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMenuSuper::SetItemData(int nItemIndex, DWORD dwData)
{
	ItemData* pItemData = GetOrCreateItemData(nItemIndex);
	if (pItemData)
	{
		pItemData->dwItemData = dwData;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CMenuSuper::GetItemData(int nItemIndex)
{
	ItemData* pItemData = GetOrCreateItemData(nItemIndex);
	if (pItemData)
	{
		return pItemData->dwItemData;
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMenuSuper::SetColumnCount(int nColumnCount )
{
	int nPrevColumnCount = m_vecColumnWidth.size();
	m_vecColumnWidth.resize(nColumnCount);

	for (int i=nPrevColumnCount; i< (int)m_vecColumnWidth.size(); i++)
	{
		m_vecColumnWidth[i] = DEFAULT_COLUMN_WIDTH;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CMenuSuper::Cleanup()
{
	if (!IsMenu())
		return false;

	CMenuItemInfo mii;
	int nCount = GetMenuItemCount();
	for (int i=0; i < nCount; i++)
	{
		GetMenuItemInfo(i, TRUE, &mii);
		ItemData* pItemData = (ItemData*)mii.dwItemData;
		if (NULL != pItemData && -1 != (int)pItemData)
		{
			delete pItemData;
		}
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CMenuSuper::MakeOwnerDrawn()
{
	if (!IsMenu())
		return false;

	CMenuItemInfo mii;
	int nCount = GetMenuItemCount();
	for (int i=0; i < nCount; i++)
	{
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_TYPE;
		if (!GetMenuItemInfo(i, TRUE, &mii))
			return false;

		mii.fType |= MFT_OWNERDRAW;
		if (!SetMenuItemInfo(i, TRUE, &mii))
			return false;
	}

	return true;
}