#include "stdafx.h"
#include "EnvelopeWizDlg.h"
#include "RegKeys.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define	MAX_PAGES_TO_CHECK	3
#define	SCROLL_BAR_WIDTH	30
#define ADDRESS_SIZE		-POINTUNITS(14)

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
void CPictureListView::SetParent(CEnvelopeWizard* pParent)
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
	m_ImageList.Create(m_nThumbnailSize, m_nThumbnailSize, ILC_COLOR24, 0, 1);
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
	i = 0;
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
		
		// Add the bitmap to our image list
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
// CEnvelopeWizard
// This is the parent dialog
CEnvelopeWizard::CEnvelopeWizard(HWND hWnd, CDocWindow* pDocWindow, CAGDoc* pPrevDoc, AGDOCTYPE DocType)
{
	m_pDocWindow = pDocWindow;
	m_pEnvelopeDoc = NULL;
    m_EnableBackground = false;
    m_nSelectedSym = IDR_ENVELOPE_SEND;
    m_nDragImage = 0;
	m_PictureList.SetParent(this);

	m_DocType = DocType;
	SIZE PageSize = {0,0};
	if (m_DocType == DOC_QUARTERCARD)
	{
		PageSize.cx = INCHES(CAGDocSheetSize::GetWidth()) / 2;
		PageSize.cy = INCHES(CAGDocSheetSize::GetHeight()) / 2;
	}
	else
	if (m_DocType == DOC_HALFCARD)
	{
		PageSize.cx = INCHES(CAGDocSheetSize::GetWidth());
		PageSize.cy = INCHES(CAGDocSheetSize::GetHeight()) / 2;
	}
	else
	if (pPrevDoc)
		pPrevDoc->GetPageSize(PageSize);

	m_PageSize = PageSize;
	m_nMaxImageSize = 0;
	m_PaperType = -1;

	// Set the default addresses and then check the registry for any saved ones.
	m_strSendAddress = "FirstName  LastName\nStreetAddress\nCity/Town,  ";
	m_strSendAddress += (!CAGDocSheetSize::GetMetric() ? "State  Zip" : "County  PostalCode  Country");
	m_strReturnAddress = m_strSendAddress;

	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		char strValue[MAX_PATH];
		DWORD dwLength = MAX_PATH;
		if (regkey.QueryStringValue("SendAddress", strValue, &dwLength) == ERROR_SUCCESS)
			m_strSendAddress = strValue;

		regkey.Close();
	}

	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		char strValue[MAX_PATH];
		DWORD dwLength = MAX_PATH;
		if (regkey.QueryStringValue("ReturnAddress", strValue, &dwLength) == ERROR_SUCCESS)
			m_strReturnAddress = strValue;

		regkey.Close();
	}

	InitGraphics(pPrevDoc);
	Create(hWnd);
}
//////////////////////////////////////////////////////////////////////
CEnvelopeWizard::~CEnvelopeWizard()
{
	for (int nIndex = 0; nIndex < MAX_NUM_PICTURES; nIndex++)
	{
		delete m_pGraphics[nIndex];
		m_pGraphics[nIndex] = NULL;
	}

	// Destroy the wizard and all of the child views
	if (::IsWindow(m_hWnd))
		DestroyWindow();

    DeleteObject(m_hHdrFont);
    DeleteObject(m_hHdrFont2);

}
//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::InitGraphics(CAGDoc* pPrevDoc)
{
	for (int nIndex = 0; nIndex < MAX_NUM_PICTURES; nIndex++)
		m_pGraphics[nIndex] = NULL;

	if (!pPrevDoc)
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
void CEnvelopeWizard::InitEnvelopeSize()
{
	CPaperTemplates::StuffCombo(DOC_ENVELOPE, 0, NULL, GetDlgItem(IDC_NEWDOC_PAPERTYPE), 0, 0);

	int iPaperType = CPaperTemplates::FindSizeMatch(DOC_ENVELOPE, m_PageSize);
	if (iPaperType < 0)
		iPaperType = 0;

	const PaperTemplate* pPaper = NULL;
	CPaperTemplates::GetTemplate(DOC_ENVELOPE, iPaperType, &pPaper);
	if (pPaper)
	{
		m_PageSize.cx = INCHES(pPaper->fWidth);
		m_PageSize.cy = INCHES(pPaper->fHeight);
	}

	SetWidthHeightControls();

	SendDlgItemMessage(IDC_NEWDOC_PAPERTYPE, CB_SETCURSEL, iPaperType, 0);
	SendDlgItemMessage(IDC_NEWDOC_WIDTH,  EM_SETREADONLY, true, 0);
	SendDlgItemMessage(IDC_NEWDOC_HEIGHT, EM_SETREADONLY, true, 0);

	return; 
}
//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::InitEnvelopeAddress()
{
	m_nSelectedSym = IDR_ENVELOPE_SEND;
	CString strAddressDisplay;
	strAddressDisplay = m_strSendAddress;
	strAddressDisplay.Replace("\n", "\r\n");
	SendMessage(GetDlgItem(IDC_SEND_ADDRESS), WM_SETTEXT, strAddressDisplay.GetLength(), (LPARAM)(LPCSTR)strAddressDisplay);
	strAddressDisplay = m_strReturnAddress;
	strAddressDisplay.Replace("\n", "\r\n");
	SendMessage(GetDlgItem(IDC_RETURN_ADDRESS), WM_SETTEXT, strAddressDisplay.GetLength(), (LPARAM)(LPCSTR)strAddressDisplay);

	return;
}
//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::InitPictureList()
{
	m_PictureList.InitDialog(GetDlgItem(IDC_PICTURE_SELECT));
	m_PictureList.SetParent(this);
	return;
}
//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LOGFONT lf;
    ::ZeroMemory(&lf, sizeof(lf));
    ::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

    strcpy(lf.lfFaceName, "Microsoft Sans Serif");
    lf.lfHeight = -13;
    lf.lfWeight = FW_BOLD;
    m_hHdrFont = ::CreateFontIndirect(&lf);
    GetDlgItem(IDC_ENVELOPEWIZ_STATIC).SetFont(m_hHdrFont);

    lf.lfHeight = -12;
	lf.lfWeight = FW_SEMIBOLD;
	m_hHdrFont2 = ::CreateFontIndirect(&lf);
	GetDlgItem(IDC_MAILING_STATIC).SetFont(m_hHdrFont2);
    GetDlgItem(IDC_RETURN_STATIC).SetFont(m_hHdrFont2);
    GetDlgItem(IDC_ENABLE_BACKGROUND).SetFont(m_hHdrFont2);
    

    InitEnvelopeSize();
    InitEnvelopeAddress();
    InitPictureList();
	
	return true;  // Let the system set the focus
}
//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UpdateControls();
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnFinished(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (IDOK == wID)
        WriteDialogData();

	ShowWindow(SW_HIDE);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnEnableBackground(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!GetDoc())
		return S_OK;

	CAGSymGraphic* pBackground = (CAGSymGraphic*)GetDoc()->FindSymbolAnywhereByID(IDR_ENVELOPE_BACKGROUND, PROCESS_HIDDEN);
	if (!pBackground)
		return S_OK;

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

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnPaperType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int iPaperType = ::SendMessage(hWndCtl, CB_GETCURSEL, 0, 0);
	m_PaperType = ::SendMessage(hWndCtl, CB_GETITEMDATA, iPaperType, 0);
	const PaperTemplate* pPaper = NULL;
	CPaperTemplates::GetTemplate(DOC_ENVELOPE, m_PaperType, &pPaper);
	if (!pPaper)
		return S_OK;

	m_PageSize.cx = INCHES(pPaper->fWidth);
	m_PageSize.cy = INCHES(pPaper->fHeight);

	SetWidthHeightControls();

	// The page size has changed - recalc the text rects and save pos in data struct
	int nWidth = m_PageSize.cx;
	int nHeight = m_PageSize.cy;

	m_nMaxImageSize = nHeight / 2;

	RecalcGraphic();
	RecalcBackgroundRect(nWidth, nHeight);
	RecalcTextRects(nWidth, nHeight);

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnApplySendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char strAddress[MAX_PATH];
	strAddress[0] = '\0';
	LRESULT lResult = SendMessage(GetDlgItem(IDC_SEND_ADDRESS), WM_GETTEXT, MAX_PATH, (LPARAM)(LPCSTR)strAddress);
	m_strSendAddress = strAddress;
	m_strSendAddress.Replace("\r", "");

	CAGSymText* pText = (CAGSymText*)GetDoc()->FindSymbolAnywhereByID(IDR_ENVELOPE_SEND, PROCESS_HIDDEN);
	SetAddressText(pText, m_strSendAddress);
	m_pDocWindow->SymbolInvalidate(pText);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnApplyReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	char strAddress[MAX_PATH];
	strAddress[0] = '\0';
	LRESULT lResult = SendMessage(GetDlgItem(IDC_RETURN_ADDRESS), WM_GETTEXT, MAX_PATH, (LPARAM)(LPCSTR)strAddress);
	m_strReturnAddress = strAddress;
	m_strReturnAddress.Replace("\r", "");

	CAGSymText* pText = (CAGSymText*)GetDoc()->FindSymbolAnywhereByID(IDR_ENVELOPE_RETURN, PROCESS_HIDDEN);
	SetAddressText(pText, m_strReturnAddress);
	m_pDocWindow->SymbolInvalidate(pText);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnSelectSendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_nSelectedSym = IDR_ENVELOPE_SEND;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnSelectReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_nSelectedSym = IDR_ENVELOPE_RETURN;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CEnvelopeWizard::OnLvnBegindragPictureSelect(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
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
//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::CreateEnvelope(CAGDoc** ppAGDoc)
{
	m_pEnvelopeDoc = new CAGDoc();

	// Let the caller take control over this document
	if (ppAGDoc)
		*ppAGDoc = m_pEnvelopeDoc;
	if (!m_pEnvelopeDoc)
		return;
	
	m_pEnvelopeDoc->SetPortrait(m_PageSize.cx <= m_PageSize.cy);
	m_pEnvelopeDoc->SetDocType(DOC_ENVELOPE);
	m_pEnvelopeDoc->SetFileName(CString("Untitled"));
	m_pEnvelopeDoc->SetModified(true);

	CAGPage* pPage = new CAGPage(m_PageSize.cx, m_PageSize.cy);
	if (!pPage)
		return;

	m_pEnvelopeDoc->AddPage(pPage);

	int nLayers = 2;
	for (int j = 0; j < nLayers; j++)
		pPage->AddLayer(new CAGLayer());

	CAGLayer* pUserLayer = pPage->GetUserLayer();
	if (!pUserLayer)
		return;

	CAGLayer* pMasterLayer = pPage->GetMasterLayer();
	if (!pUserLayer)
		return;

	m_nMaxImageSize = m_PageSize.cy / 2;

	GetPictureList()->DrawListViewThumbnails();

	// Add the Background Item
	CAGSymGraphic* pBackground = new CAGSymRectangle();
	if (pBackground)
	{
		RECT BackgroundRect;
		::SetRect(&BackgroundRect, 0, 0, m_PageSize.cx, m_PageSize.cy);
		pBackground->SetDestRect(BackgroundRect);
		pBackground->SetID(IDR_ENVELOPE_BACKGROUND);
		pBackground->SetFillColor(RGB(240,240,255));
		pBackground->SetFillColor2(RGB(255,255,255));
		pBackground->SetFillType(FT_Solid);
		pBackground->SetHidden(true);
		pUserLayer->AddSymbol(pBackground);
	}

	int xCenter = m_PageSize.cx / 2;
	int yCenter = m_PageSize.cy / 2;
	RECT rMargins = { INCHES(0.375), INCHES(0.375), m_PageSize.cx - INCHES(0.375), m_PageSize.cy - INCHES(0.375)};

	// Add the Send Address Text Item
	CAGSymText* pSendAddr = new CAGSymText(ST_TEXT);
	if (pSendAddr)
	{
		RECT SendRect;
		SendRect.left   = xCenter - (INCHES(1.75)/2);
		SendRect.top    = yCenter - (INCHES(1)/2);
		SendRect.right  = SendRect.left + INCHES(3.0);
		SendRect.bottom = SendRect.top + INCHES(1.1);
		pSendAddr->Create(SendRect);

		pSendAddr->SetID(IDR_ENVELOPE_SEND);
		SetAddressText(pSendAddr, m_strSendAddress);
		pSendAddr->SelectAll(false);
		pSendAddr->SetHorzJust(eFlushLeft);
		pSendAddr->SetVertJust(eVertTop);
		pSendAddr->SetEmphasis(false, false, false);
		pUserLayer->AddSymbol(pSendAddr);
	}

	// Add the Return Address Text Item
	CAGSymText* pReturnAddr = new CAGSymText(ST_TEXT);
	if (pReturnAddr)
	{
		RECT ReturnRect;
		ReturnRect.left   = rMargins.left;
		ReturnRect.top    = rMargins.top;
		ReturnRect.right  = ReturnRect.left + INCHES(3.0);
		ReturnRect.bottom = ReturnRect.top + INCHES(1.1);
		pReturnAddr->Create(ReturnRect);

		pReturnAddr->SetID(IDR_ENVELOPE_RETURN);
		SetAddressText(pReturnAddr, m_strReturnAddress);
		pReturnAddr->SelectAll(false);
		pReturnAddr->SetHorzJust(eFlushLeft);
		pReturnAddr->SetVertJust(eVertTop);
		pReturnAddr->SetEmphasis(false, false, false);
		pUserLayer->AddSymbol(pReturnAddr);
	}

	// Add the first image to the document
	CAGSymImage* pImage = m_pGraphics[0];
	if (!pImage)
		return;

	CAGSymImage* pNewImage = (CAGSymImage*)pImage->Duplicate();
	if (!pNewImage)
		return;

	SetImageSize(pNewImage);

	pNewImage->SetID(IDC_ENVELOPE_PIC_0);

	pUserLayer->AddSymbol(pNewImage);
	m_pDocWindow->SymbolUnselect(true);
	m_pDocWindow->SymbolSelect(pNewImage);
	m_pDocWindow->Position(POS_LEFT, POS_BOTTOM, 0/*id*/);
}

//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::SetImageSize(CAGSymImage* pImage, POINT* pCenterPoint)
{
	if (!pImage)
		return;

	// The image symbol should be at (0,0) and have a unity matrix
	CRect DestRect = pImage->GetDestRect();
	DestRect.MoveToXY(0,0);
	pImage->SetDestRect(DestRect);

	// Maintain a maximum image size using the symbol's matrix
	CAGMatrix NewMatrix;
	if (DestRect.Width() > m_nMaxImageSize || DestRect.Height() > m_nMaxImageSize)
	{
		int dx = m_nMaxImageSize;
		int dy = m_nMaxImageSize;
		double fScale = ScaleToFit(&dx, &dy, DestRect.Width(), DestRect.Height(), true/*bUseSmallerFactor*/);
		NewMatrix.Scale(fScale, fScale);
		NewMatrix.Transform(DestRect);
	}

	if (pCenterPoint)
		NewMatrix.Translate(pCenterPoint->x - DestRect.Width()/2, pCenterPoint->y - DestRect.Height()/2);

	pImage->SetMatrix(NewMatrix);
}

//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::AddDroppedGraphic(int nDragImage, POINT point)
{
	if (!m_pEnvelopeDoc)
		return;

	CAGPage* pPage = m_pEnvelopeDoc->GetPage(0);
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

	SetImageSize(pNewImage, &point);
	
	pUserLayer->AddSymbol(pNewImage);
	m_pDocWindow->SymbolUnselect(true);
	m_pDocWindow->SymbolSelect(pNewImage);
	m_pDocWindow->SymbolInvalidate(pNewImage);
}

//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::RecalcGraphic()
{
	if (!m_pEnvelopeDoc || !m_pDocWindow)
		return;

	CAGSymImage* pNewImage = (CAGSymImage*)m_pEnvelopeDoc->FindSymbolAnywhereByID(IDC_ENVELOPE_PIC_0, 0/*dwFlags*/);
	if (!pNewImage)
		return;

	SetImageSize(pNewImage);

	m_pDocWindow->SymbolUnselect(true);
	m_pDocWindow->SymbolSelect(pNewImage);
	m_pDocWindow->Position(POS_LEFT, POS_BOTTOM, 0/*id*/);
	m_pDocWindow->SymbolInvalidate(pNewImage);
}

//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::RecalcBackgroundRect(int nWidth, int nHeight)
{
	if (!m_pEnvelopeDoc)
		return;

	CAGSymGraphic* pBackground = (CAGSymGraphic*)m_pEnvelopeDoc->FindSymbolAnywhereByID(IDR_ENVELOPE_BACKGROUND, 0/*dwFlags*/);
	if (!pBackground)
		return;

	RECT BackgroundRect;
	::SetRect(&BackgroundRect, 0, 0, nWidth, nHeight);
	pBackground->SetDestRect(BackgroundRect);

	m_pDocWindow->SymbolUnselect(true);
	m_pDocWindow->SymbolSelect(pBackground);
	m_pDocWindow->MaximizeSymbolSize(true/*bWidth*/, true/*bHeight*/);
	m_pDocWindow->SymbolInvalidate(pBackground);
}

//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::RecalcTextRects(int nWidth, int nHeight)
{
	if (!m_pEnvelopeDoc)
		return;

	int xCenter = nWidth / 2;
	int yCenter = nHeight / 2;
	RECT rMargins = { INCHES(0.375), INCHES(0.375), nWidth - INCHES(0.375), nHeight - INCHES(0.375)};

	CAGSymText* pSendAddr = (CAGSymText*)m_pEnvelopeDoc->FindSymbolAnywhereByID(IDR_ENVELOPE_SEND, PROCESS_HIDDEN);
	if (pSendAddr)
	{
		RECT SendRect;
		SendRect.left   = xCenter - (INCHES(1.75)/2);
		SendRect.top    = yCenter - (INCHES(1)/2);
		SendRect.right  = SendRect.left + INCHES(3.0);
		SendRect.bottom = SendRect.top + INCHES(1.1);
		pSendAddr->SetDestRect(SendRect);
	}

	CAGSymText* pReturnAddr = (CAGSymText*)m_pEnvelopeDoc->FindSymbolAnywhereByID(IDR_ENVELOPE_RETURN, PROCESS_HIDDEN);
	if (pReturnAddr)
	{
		RECT ReturnRect;
		ReturnRect.left   = rMargins.left;
		ReturnRect.top    = rMargins.top;
		ReturnRect.right  = ReturnRect.left + INCHES(3.0);
		ReturnRect.bottom = ReturnRect.top + INCHES(1.1);
		pReturnAddr->SetDestRect(ReturnRect);
	}
}

//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::SetPageSize()
{
	if (!m_pEnvelopeDoc)
		return;

	CAGPage* pPage = m_pEnvelopeDoc->GetCurrentPage();
	if (!pPage)
		return;

	pPage->SetPageSize(m_PageSize);
	m_pDocWindow->ActivateNewPage();
}
//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::SetAddressText(CAGSymText* pText, CString& strText)
{
	if (!pText)
		return;

	LOGFONT Font;
	memset(&Font, 0, sizeof(Font));
	Font.lfWeight = FW_NORMAL;
	Font.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET
	lstrcpy(Font.lfFaceName, "Arial");
	Font.lfHeight = ADDRESS_SIZE;

	// Initialize the type specs
	CAGSpec* pSpecs[] = { new CAGSpec(
		Font,			// LOGFONT& Font
		eRagRight,		// eTSJust HorzJust
		0,				// short sLineLeadPct
		LT_None,		// int LineWidth
		CLR_NONE,		// COLORREF LineColor
		FT_Solid,		// FillType Fill
		RGB(0,0,0),		// COLORREF FillColor
		CLR_NONE)		// COLORREF FillColor2
	};

	pText->SelectAll(true);
	pText->Delete();
	const int pOffsets[] = {0};
//j	pText->PasteText((char*)strText, pSpecs[0]);
	pText->SetText(strText, 1, pSpecs, pOffsets);
}
//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::SetWidthHeightControls()
{
	CString strWidth;
	CString strHeight;
	if (CAGDocSheetSize::GetMetric())
	{
		strWidth.Format("%0.4G mm", Inches2MM(DINCHES(m_PageSize.cx)));
		strHeight.Format("%0.4G mm", Inches2MM(DINCHES(m_PageSize.cy)));
	}
	else
	{
		strWidth.Format("%0.5G inches", DINCHES(m_PageSize.cx));
		strHeight.Format("%0.5G inches", DINCHES(m_PageSize.cy));
	}

	SetDlgItemText(IDC_NEWDOC_WIDTH, strWidth);
	SetDlgItemText(IDC_NEWDOC_HEIGHT, strHeight);

	SetPageSize();
}
//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::SelectBackground()
{
	if (!m_EnableBackground)
		return;

	if (!GetDoc())
		return;

	CAGSymGraphic* pBackground = (CAGSymGraphic*)GetDoc()->FindSymbolAnywhereByID(IDR_ENVELOPE_BACKGROUND, PROCESS_HIDDEN);
	if (!pBackground)
		return;

	m_pDocWindow->SymbolUnselect(true);
	m_pDocWindow->SymbolSelect(pBackground);
}
//////////////////////////////////////////////////////////////////////
bool CEnvelopeWizard::WriteDialogData()
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		bool bOK = (regkey.SetStringValue("SendAddress", m_strSendAddress) == ERROR_SUCCESS);
		regkey.Close();
	}

	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		bool bOK = (regkey.SetStringValue("ReturnAddress", m_strReturnAddress) == ERROR_SUCCESS);
		regkey.Close();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::UpdateControls()
{
	// Need to get the text from addresses
}
//////////////////////////////////////////////////////////////////////
void CEnvelopeWizard::FinishImageDrop(bool fDrop, POINTL pt)
{
	if (!fDrop)
		return;

	POINT point = {pt.x, pt.y};
	::ScreenToClient(m_pDocWindow->m_hWnd, &point);
	m_pDocWindow->GetClientDC()->GetViewToDeviceMatrix().Inverse().Transform(point);
	AddDroppedGraphic(m_nDragImage, point);
}
