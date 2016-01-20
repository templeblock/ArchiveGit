#include "stdafx.h"
#include "EMDlg.h"
#include "RegKeys.h"

//////////////////////////////////////////////////////////////////////
// CEMDlg
// This is the parent dialog
CEMDlg::CEMDlg(HWND hWnd, CDocWindow* pDocWindow, CAGDoc* pPrevDoc, AGDOCTYPE DocType, UINT iTabID): 
			  m_TabControl(this)
{
	m_pDocWindow = pDocWindow;
	m_pEnvelopeDoc = NULL;
	m_nMaxImageSize = 0;
	m_iTabID = iTabID;
	if (pPrevDoc)
		m_pPrevDoc = pPrevDoc->Duplicate();

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
	
	m_TabControl.GetDesignTab()->InitGraphics(pPrevDoc);
	
	Create(hWnd);
}
//////////////////////////////////////////////////////////////////////
CEMDlg::~CEMDlg()
{
	if (m_pPrevDoc)
		delete m_pPrevDoc;
	// Destroy the wizard and all of the child views
	if (::IsWindow(m_hWnd))
		DestroyWindow();
}

//////////////////////////////////////////////////////////////////////
LRESULT CEMDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{  
	SetMsgHandled(false);
	DoDataExchange(DDX_LOAD);
	if (m_pDocWindow && m_pDocWindow->m_pCtp)
		m_pDocWindow->m_pCtp->SetWindowIcon(m_hWnd);

	m_TabControl.Init(m_iTabID);

	CToolTipCtrl ToolTip = GetToolTipCtrl();
	ToolTip.SetTitle(1/*Info*/, _T("Envelope Manager"));
    
	InitEnvelopeSize();
	
	return true;  // Let the system set the focus
}
//////////////////////////////////////////////////////////////////////
void CEMDlg::InitEnvelopeSize()
{
	int iPaperType = CPaperTemplates::FindSizeMatch(DOC_ENVELOPE, m_PageSize);
	if (iPaperType < 0)
		iPaperType = 0;

	const PaperTemplate* pPaper = NULL;
	CPaperTemplates::GetTemplate(DOC_ENVELOPE, iPaperType, &pPaper);
	if (pPaper)
	{
		m_PageSize.cx = INCHES(pPaper->fWidth);
		m_PageSize.cy = INCHES(pPaper->fHeight);
		m_nMaxImageSize = m_PageSize.cy / 2;
	}

	SetWidthHeightControls();

	m_TabControl.GetDesignTab()->UpdatePaperType(iPaperType);
	
	return; 
}
//////////////////////////////////////////////////////////////////////
LRESULT CEMDlg::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    UpdateControls();
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CEMDlg::OnFinished(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (IDC_BUTTON_OK == wID)
        WriteDialogData();

	ShowWindow(SW_HIDE);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CEMDlg::OnPrintPreveiw(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pDocWindow->DoCommand(_T("PRINTPREVIEW"));
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CEMDlg::OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)this;
}
//////////////////////////////////////////////////////////////////////
bool CEMDlg::WriteDialogData()
{
	CString szAddr = m_TabControl.GetAddressTab()->GetSendAddress();
	WriteRegistryString(REGVAL_SENDADDRESS, szAddr);
	szAddr = m_TabControl.GetAddressTab()->GetReturnAddress();
	WriteRegistryString(REGVAL_RETURNADDRESS, szAddr);
	
	return true;
}
//////////////////////////////////////////////////////////////////////
void CEMDlg::UpdateMailingAddress()
{
	m_TabControl.GetAddressTab()->UpdateMailingAddress();
	return;
}
//////////////////////////////////////////////////////////////////////
void CEMDlg::FinishImageDrop(bool fDrop, POINTL pt)
{
	m_TabControl.GetDesignTab()->FinishImageDrop(fDrop, pt);
}
//////////////////////////////////////////////////////////////////////
void CEMDlg::SetImageSize(CAGSymImage* pImage, POINT* pCenterPoint)
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
bool CEMDlg::SetEnvelopeDoc(CAGDoc* pAGDoc)
{
	m_pEnvelopeDoc = pAGDoc;
	return true;
}
//////////////////////////////////////////////////////////////////////
void CEMDlg::SetCurrentTab(UINT iTabID)
{
	m_TabControl.SetCurrentTab(iTabID);
}
//////////////////////////////////////////////////////////////////////
void CEMDlg::CreateEnvelope(CAGDoc** ppAGDoc)
{
    m_pEnvelopeDoc = new CAGDoc();

	// Let the caller take control over this document
	if (ppAGDoc)
		*ppAGDoc = m_pEnvelopeDoc;

	if (!m_pEnvelopeDoc)
		return;
	
	m_pEnvelopeDoc->SetPortrait(m_PageSize.cx <= m_PageSize.cy);
	m_pEnvelopeDoc->SetDocType(DOC_ENVELOPE);
	m_pEnvelopeDoc->SetFileName(CString("Untitled Envelope"));
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
	if (!pMasterLayer)
		return;

	m_nMaxImageSize = m_PageSize.cy / 2;

	// Add the Background Item
	CAGSymRectangle* pBackground = new CAGSymRectangle();
	if (pBackground)
	{
		RECT BackgroundRect;
		::SetRect(&BackgroundRect, 0, 0, m_PageSize.cx, m_PageSize.cy);
		pBackground->SetDestRect(BackgroundRect);
		pBackground->SetID(ID_ENVELOPE_BACKGROUND);
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

		pSendAddr->SetID(ID_ENVELOPE_SEND);
		CString szSendAddr = m_TabControl.GetAddressTab()->GetSendAddress();
		SetAddressText(pSendAddr, szSendAddr, false);
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

		pReturnAddr->SetID(ID_ENVELOPE_RETURN);
		CString szReturnAddr = m_TabControl.GetAddressTab()->GetReturnAddress();
		SetAddressText(pReturnAddr, szReturnAddr, false);
		pReturnAddr->SelectAll(false);
		pReturnAddr->SetHorzJust(eFlushLeft);
		pReturnAddr->SetVertJust(eVertTop);
		pReturnAddr->SetEmphasis(false, false, false);
		pUserLayer->AddSymbol(pReturnAddr);
	}

	// get the userLayer frm pPrevDoc
	CAGPage * prevPage = m_pPrevDoc->GetPage(0);
	CAGLayer * prevLayer = prevPage->GetUserLayer();
	
	// get the biggest rectangle
	CRect DestRect(-1,-1,-1,-1);
	CAGSym *pNewImage = NULL;
	for (int i = 0; ; i++)
	{
		CAGSym* pSym = prevLayer->GetSymbol(i);
		if (!pSym)
			break;
		CRect r = pSym->GetDestRect();
		if ((r.Width()*r.Height()) > (DestRect.Width()*DestRect.Height()))
		{
			DestRect = r;
			pNewImage = pSym;
		}
	}
		// get overall scaling factor and transaltions in NewMatrix
	CAGMatrix NewMatrix;
	if (DestRect.Width() > m_nMaxImageSize || DestRect.Height() > m_nMaxImageSize)
	{
		int dx = m_nMaxImageSize;
		int dy = m_nMaxImageSize;
		double fScale = ScaleToFit(&dx, &dy, DestRect.Width(), DestRect.Height(), true/*bUseSmallerFactor*/);
		NewMatrix.Scale(fScale, fScale);
		pNewImage->SetMatrix(NewMatrix)	;
	}
	pUserLayer->AddSymbol(pNewImage, false/*bAddToFront*/);
	m_pDocWindow->SymbolUnselect(true);
	m_pDocWindow->SymbolSelect(pNewImage);
	m_pDocWindow->Position(POS_LEFT, POS_BOTTOM, 0/*id*/);
	m_pDocWindow->SymbolUnselect(true);
	pUserLayer->DeleteSymbol(pNewImage, true/*bFromListOnly*/);
	NewMatrix = pNewImage->GetMatrix();

	int nSymbols = 0;
	for (int i = 0; ; i++)
	{
		CAGSym* pSym = prevLayer->GetSymbol(i);
		if (!pSym)
			break;

		//if (!pSym->IsImage())
		//  continue;
		
		pUserLayer->AddSymbol(pSym, false/*bAddToFront*/);
		pSym->SetID(ID_ENVELOPE_PIC_0);
		pSym->SetMatrix(NewMatrix)	;
		m_pDocWindow->SymbolInvalidate(pSym);
		nSymbols++;
	}
	// clean up the prevLayer so that the objects 
	// do not get deleted twice
	for (int i = nSymbols-1; i >=0 ; --i)
	{
		CAGSym* pSym = prevLayer->GetSymbol(i);
		if (!pSym)
			break;
		prevLayer->DeleteSymbol(pSym, true/*bFromListOnly*/);
	}
	
}
//////////////////////////////////////////////////////////////////////
void CEMDlg::RecalcGraphic()
{
	if (!m_pEnvelopeDoc || !m_pDocWindow)
		return;

	CAGSymImage* pNewImage = (CAGSymImage*)m_pEnvelopeDoc->FindSymbolAnywhereByID(ID_ENVELOPE_PIC_0, 0/*dwFlags*/);
	if (!pNewImage)
		return;

	SetImageSize(pNewImage);

	m_pDocWindow->SymbolUnselect(true);
	m_pDocWindow->SymbolSelect(pNewImage);
	m_pDocWindow->Position(POS_LEFT, POS_BOTTOM, 0/*id*/);
	m_pDocWindow->SymbolInvalidate(pNewImage);
}
//////////////////////////////////////////////////////////////////////
void CEMDlg::RecalcTextRects(int nWidth, int nHeight)
{
	if (!m_pEnvelopeDoc)
		return;

	int xCenter = nWidth / 2;
	int yCenter = nHeight / 2;
	RECT rMargins = { INCHES(0.375), INCHES(0.375), nWidth - INCHES(0.375), nHeight - INCHES(0.375)};

	CAGSymText* pSendAddr = (CAGSymText*)m_pEnvelopeDoc->FindSymbolAnywhereByID(ID_ENVELOPE_SEND, PROCESS_HIDDEN);
	if (pSendAddr)
	{
		RECT SendRect;
		SendRect.left   = xCenter - (INCHES(1.75)/2);
		SendRect.top    = yCenter - (INCHES(1)/2);
		SendRect.right  = SendRect.left + INCHES(3.0);
		SendRect.bottom = SendRect.top + INCHES(1.1);
		pSendAddr->SetDestRect(SendRect);
	}

	CAGSymText* pReturnAddr = (CAGSymText*)m_pEnvelopeDoc->FindSymbolAnywhereByID(ID_ENVELOPE_RETURN, PROCESS_HIDDEN);
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
void CEMDlg::RecalcBackgroundRect(int nWidth, int nHeight)
{
	if (!m_pEnvelopeDoc)
		return;

	CAGSymGraphic* pBackground = (CAGSymGraphic*)m_pEnvelopeDoc->FindSymbolAnywhereByID(ID_ENVELOPE_BACKGROUND, 0/*dwFlags*/);
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
void CEMDlg::SetPageSize()
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
void CEMDlg::SetWidthHeightControls()
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
void CEMDlg::UpdateControls()
{
}
