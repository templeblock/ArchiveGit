#include "stdafx.h"
#include "RegKeys.h"
#include "EMDesignTab.h"
#include "EMTabControl.h"
#include "EMDlg.h"

#define	SCROLL_BAR_WIDTH	30

//////////////////////////////////////////////////////////////////////
// CPictureListView
// This is the listbox that will hold the pictures
CPictureListView::CPictureListView()
{
	m_pParent = NULL;
	m_nThumbnailSize = 0;
}

//////////////////////////////////////////////////////////////////////
CPictureListView::~CPictureListView()
{
	m_ImageList.Destroy();
}

//////////////////////////////////////////////////////////////////////
void CPictureListView::SetParent(CEMDesignTab* pParent)
{
	m_pParent = pParent;
}

//////////////////////////////////////////////////////////////////////
void CPictureListView::InitDialog(HWND hListView)
{
	if (hListView)
		SubclassWindow(hListView);

	RECT rect;
	GetClientRect(&rect);
	m_nThumbnailSize = (rect.right - rect.left) - SCROLL_BAR_WIDTH;
	SetIconSpacing(m_nThumbnailSize, m_nThumbnailSize);
	m_ImageList.Create(m_nThumbnailSize, m_nThumbnailSize, ILC_COLOR24|ILC_MASK, 0, 1);
	SetImageList(m_ImageList, LVSIL_NORMAL);
}

//////////////////////////////////////////////////////////////////////
void  CPictureListView::DrawListViewThumbnails()
{
	// Set the length of the space between thumbnails
	// You can also calculate and set it based on the length of your list control

	// Hold the window update to avoid flicking
	SetRedraw(FALSE);

	// Reset the image list
	for (int i=0; i<m_ImageList.GetImageCount(); i++)
		m_ImageList.Remove(i);	

	// Remove all items from list view
	if (GetItemCount())
		DeleteAllItems();

	// Draw the thumbnails
	COLORREF cTransparentColor = CLR_NONE;
	int i = 0;
	for (int nIndex = 0; nIndex < MAX_NUM_PICTURES; nIndex++)
	{
		CAGSymImage* pImage =  m_pParent->m_pGraphics[nIndex];
		if (!pImage)
			continue;

		BITMAPINFOHEADER* pDib = pImage->GetDib();
		if (!pDib)
			continue;
		

		// Borrow our dib header to create our thumbnail bitmap
		int nWidth = pDib->biWidth;
		int nHeight = pDib->biHeight;
		pDib->biWidth = m_nThumbnailSize;
		pDib->biHeight = m_nThumbnailSize;

		// Create thumbnail bitmap section
		HBITMAP hBitmap = ::CreateDIBSection(NULL, (BITMAPINFO*)pDib, DIB_RGB_COLORS, NULL, NULL, 0); 

		// Restore the dib header
		pDib->biWidth = nWidth;
		pDib->biHeight = nHeight;

		// Select the thumbnail bitmap into screen dc
		HDC	hMemDC = ::CreateCompatibleDC(NULL);	
		HGDIOBJ	hOldObj = ::SelectObject(hMemDC, hBitmap);

		// Set stretch mode
		::SetStretchBltMode(hMemDC, COLORONCOLOR);

		int dx = m_nThumbnailSize;
		int dy = m_nThumbnailSize;
		ScaleToFit(&dx, &dy, nWidth, nHeight, true/*bUseSmallerFactor*/);

		// Populate the thumbnail bitmap bits
		RECT rect = {0, 0, m_nThumbnailSize, m_nThumbnailSize};
		::FillRect(hMemDC, &rect, CAGBrush(RGB(255,255,255)));
		::StretchDIBits(hMemDC,
						(m_nThumbnailSize - dx)/2, (m_nThumbnailSize - dy)/2,
						dx, dy, 0, 0, nWidth, nHeight, DibPtr(pDib), 
						(BITMAPINFO*)pDib, DIB_RGB_COLORS, SRCCOPY);

		// Restore DC object
		::SelectObject(hMemDC, hOldObj);
	  
		// Clean up
		::DeleteObject(hMemDC);
		
		// Check for transparency
		cTransparentColor = pImage->GetTransparentColor();
		if (cTransparentColor != CLR_NONE)
			m_ImageList.Add(hBitmap, cTransparentColor);
		else
			m_ImageList.Add(hBitmap);

		// Set the image file name as item text
		InsertItem(i, "", i);

		// Get the current item position	 
		POINT pt;
		GetItemPosition(i, &pt);	 
	  
		// Shift the thumbnail to desired position
		pt.x = 0; 
		pt.y = i * m_nThumbnailSize;
		SetItemPosition(i, pt);
		i++;
	}

	// Show the new thumbnails
//j	Arrange(LVA_ALIGNLEFT);
	SetRedraw(TRUE); 
}
//////////////////////////////////////////////////////////////////////
// CEMDesignTab
/////////////////////////////////////////////////////////////////////////////
CEMDesignTab::CEMDesignTab(CEMDlg * pEMDlg)
{
	m_pEMDlg = pEMDlg;
	m_LastColor = 0xFFFFFFFF;
	m_PictureList.SetParent(this);
	m_EnableBackground = false;
	m_nDragImage = 0;
	m_PaperType = -1;
}

/////////////////////////////////////////////////////////////////////////////
CEMDesignTab::~CEMDesignTab()
{
	for (int nIndex = 0; nIndex < MAX_NUM_PICTURES; nIndex++)
	{
		delete m_pGraphics[nIndex];
		m_pGraphics[nIndex] = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEMDesignTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	CPaperTemplates::StuffCombo(DOC_ENVELOPE, 0, NULL, GetDlgItem(IDC_NEWDOC_PAPERTYPE), 0, 0);
    InitPictureList();
	m_pDocWindow = m_pEMDlg->GetDocWin();

	GetDlgItem(IDC_STATIC_DESIGNHEADER).SetFont(m_LabelFont2);
	GetDlgItem(IDC_STATIC_ENV_SIZE).SetFont(m_LabelFont1);
	GetDlgItem(IDC_STATIC_IMAGE).SetFont(m_LabelFont1);
	GetDlgItem(IDC_ENABLE_BACKGROUND).SetFont(m_LabelFont1);
	if (m_PaperType > -1)
		SendDlgItemMessage(IDC_NEWDOC_PAPERTYPE, CB_SETCURSEL, m_PaperType, 0);

	if (!m_pGraphics[0])
	{
		CWindow wnd = GetDlgItem(IDC_EMPTY_STATIC);
		if (wnd.IsWindow())
		{
			wnd.SetWindowText(_T("Sorry, no images are available."));
			wnd.SetFont(m_LabelFont1);
			wnd.ShowWindow(SW_SHOW);
		}
	}

	SetMsgHandled(false);

	return true;  // return true  unless you set the focus to a control
}
//////////////////////////////////////////////////////////////////////
LRESULT CEMDesignTab::OnCtlColor(HDC hDC, HWND hwnd)
{
	HWND hCtl = hwnd;
	::SetBkMode(hDC, TRANSPARENT);

	if (hCtl == GetDlgItem(IDC_EMPTY_STATIC).m_hWnd)
	{
		COLORREF colorBG = RGB(255,255,255);
		::SetTextColor(hDC, RGB(255,0,0));
		::SetBkColor(hDC, colorBG);
		return (LRESULT)GetBgBrush(colorBG);
	}

	return DefWindowProc();
}
//////////////////////////////////////////////////////////////////////
void CEMDesignTab::InitGraphics(CAGDoc* pPrevDoc)
{
	for (int nIndex = 0; nIndex < MAX_NUM_PICTURES; nIndex++)
		m_pGraphics[nIndex] = NULL;

	if (!pPrevDoc)
		return;

	if (pPrevDoc->HasLockedImages() || pPrevDoc->IsContextRestricted())
		return;

	int nGraphicItem = 0;
	for (int nIndex = 0; nIndex < MAX_PAGES_TO_CHECK; nIndex++)
	{
		CAGPage* pPage = pPrevDoc->GetPage(nIndex);
		if (!pPage)
			continue;

		int nLayers = pPage->GetNumLayers();
		for (int l = 0; l < nLayers; l++)
		{
			CAGLayer* pLayer = pPage->GetLayer(l);
			if (!pLayer)
				continue;

			int nSymbols = pLayer->GetNumSymbols();
			for (int i = 0; i < nSymbols; i++)
			{
				CAGSym* pSym = (CAGSym*)pLayer->GetSymbol(i);
				if (!pSym)
					continue;

				if (!pSym->IsImage())
					continue;

				CAGSymImage* pImage = (CAGSymImage*)pSym;
				if (pImage->IsLocked() || pImage->IsDeleted())
					continue;

				CAGSymImage* pNewImage = (CAGSymImage*)pImage->Duplicate();
				if (!pNewImage)
					return;

				// Add a copy of the image symbol to the list
				// Be sure to set its location to (0,0) to make it easier to add to the envelope
				CRect DestRect = pNewImage->GetDestRectTransformed();
				DestRect.MoveToXY(0,0);
				pNewImage->SetDestRect(DestRect);
				pNewImage->SetMatrix(CAGMatrix());
				pNewImage->SetHidden(false);
				m_pGraphics[nGraphicItem] = pNewImage;
				nGraphicItem++;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CEMDesignTab::InitPictureList()
{
	m_PictureList.InitDialog(GetDlgItem(IDC_PICTURE_SELECT));
	m_PictureList.SetParent(this);
	m_PictureList.DrawListViewThumbnails();
	return;
}
//////////////////////////////////////////////////////////////////////
bool CEMDesignTab::UpdatePaperType(int iPaperType)
{
	if (iPaperType < 0)
		return false;
	
	SetPaperType(iPaperType);
	SendDlgItemMessage(IDC_NEWDOC_PAPERTYPE, CB_SETCURSEL, m_PaperType, 0);
	
	return true;
}
//////////////////////////////////////////////////////////////////////
void CEMDesignTab::AddDroppedGraphic(int nDragImage, POINT point)
{
	CAGDoc * pEnvelopeDoc = m_pEMDlg->GetDoc();
	CDocWindow * pDocWindow = m_pEMDlg->GetDocWin();

	if (!pEnvelopeDoc || !pDocWindow)
		return;

	CAGPage* pPage = pEnvelopeDoc->GetPage(0);
	if (!pPage)
		return;

	CAGLayer* pUserLayer = pPage->GetUserLayer();
	if (!pUserLayer)
		return;

	CAGSymImage* pImage = m_pGraphics[nDragImage];
	if (!pImage)
		return;

	CAGSymImage* pNewImage = (CAGSymImage*)pImage->Duplicate();
	if (!pNewImage)
		return;

	m_pEMDlg->SetImageSize(pNewImage, &point);
	
	pUserLayer->AddSymbol(pNewImage);
	pDocWindow->SymbolUnselect(true);
	pDocWindow->SymbolSelect(pNewImage);
	pDocWindow->SymbolInvalidate(pNewImage);
}

//////////////////////////////////////////////////////////////////////
void CEMDesignTab::FinishImageDrop(bool fDrop, POINTL pt)
{
	if (!fDrop)
		return;

	POINT point = {pt.x, pt.y};
	::ScreenToClient(m_pDocWindow->m_hWnd, &point);
	m_pDocWindow->GetClientDC()->GetViewToDeviceMatrix().Inverse().Transform(point);
	AddDroppedGraphic(m_nDragImage, point);
}
//////////////////////////////////////////////////////////////////////
void CEMDesignTab::SelectBackground()
{
	if (!m_EnableBackground)
		return;

	CAGDoc * pDoc = m_pEMDlg->GetDoc();
	if (!pDoc)
		return;

	CAGSymGraphic* pBackground = (CAGSymGraphic*)pDoc->FindSymbolAnywhereByID(ID_ENVELOPE_BACKGROUND, PROCESS_HIDDEN);
	if (!pBackground)
		return;

	m_pDocWindow->SymbolUnselect(true);
	m_pDocWindow->SymbolSelect(pBackground);
}

//////////////////////////////////////////////////////////////////////
void CEMDesignTab::OnEnableBackground(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	CAGDoc * pDoc = m_pEMDlg->GetDoc();
	if (!pDoc)
		return;

	CAGSymGraphic* pBackground = (CAGSymGraphic*)pDoc->FindSymbolAnywhereByID(ID_ENVELOPE_BACKGROUND, PROCESS_HIDDEN);
	if (!pBackground)
		return;

	m_EnableBackground = !m_EnableBackground;
	if (m_EnableBackground)
	{
		pBackground->SetHidden(false);
		m_pDocWindow->SymbolUnselect(true);
		m_pDocWindow->SymbolSelect(pBackground);
		m_pDocWindow->MaximizeSymbolSize(true/*bWidth*/, true/*bHeight*/);
		m_pDocWindow->SymbolInvalidate(pBackground);
	}
	else
	{
		if (m_pDocWindow->GetSelectedItem() == pBackground)
			m_pDocWindow->SymbolUnselect(true);

		m_pDocWindow->SymbolInvalidate(pBackground);
		pBackground->SetHidden(true);
	}

	return;
}
//////////////////////////////////////////////////////////////////////
void CEMDesignTab::OnPaperType(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	int iPaperType = ::SendMessage(hWndCtl, CB_GETCURSEL, 0, 0);
	m_PaperType = ::SendMessage(hWndCtl, CB_GETITEMDATA, iPaperType, 0);
	const PaperTemplate* pPaper = NULL;
	CPaperTemplates::GetTemplate(DOC_ENVELOPE, m_PaperType, &pPaper);
	if (!pPaper)
		return;

	SIZE PageSize;
	PageSize.cx = INCHES(pPaper->fWidth);
	PageSize.cy = INCHES(pPaper->fHeight);
	m_pEMDlg->SetPageSize(PageSize);
	m_pEMDlg->SetWidthHeightControls();

	// The page size has changed - recalc the text rects and save pos in data struct
	int nWidth = PageSize.cx;
	int nHeight = PageSize.cy;

	int nMaxImageSize = nHeight / 2;
	m_pEMDlg->SetMaxImageSize(nMaxImageSize);

	m_pEMDlg->RecalcGraphic();
	m_pEMDlg->RecalcBackgroundRect(nWidth, nHeight);
	m_pEMDlg->RecalcTextRects(nWidth, nHeight);

	return;
}
//////////////////////////////////////////////////////////////////////
LRESULT CEMDesignTab::OnLvnBegindragPictureSelect(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (!pNMLV)
		return S_OK;
	
	if (idCtrl != IDC_PICTURE_SELECT)
		return S_OK;

	// Need to select cursor
	// Need to copy graphic to clipboard
	m_nDragImage = pNMLV->iItem;
	m_pDocWindow->m_pCtp->DoDragDrop();

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
void CEMDesignTab::OnEnterTab(bool bFirstTime)
{
}

/////////////////////////////////////////////////////////////////////////////
void CEMDesignTab::OnLeaveTab()
{
}
