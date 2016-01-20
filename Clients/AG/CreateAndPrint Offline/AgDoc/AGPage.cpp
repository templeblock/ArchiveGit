#include "stdafx.h"
#include "AGPage.h"
#include "AGDC.h"
#include "AGDib.h"
#include "ScaleImage.h"
#include "ConvertDibToJpg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

/////////////////////////////////////////////////////////////////////////////
CAGPage::CAGPage(int cx, int cy)
{
	m_PageSize.cx = cx;
	m_PageSize.cy = cy;
	m_szPageName = "";

	m_nActiveLayer = 1;
	m_nLayers = 0;
	for (int i = 0; i < MAX_AGLAYER; i++)
		m_pLayers[i] = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CAGPage::~CAGPage()
{
	for (int i = 0; i < m_nLayers; i++)
		delete m_pLayers[i];
}

/////////////////////////////////////////////////////////////////////////////
void CAGPage::AddLayer(CAGLayer* pLayer)
{
	if (!pLayer)
		return;

	if (m_nLayers >= MAX_AGLAYER)
		return;

	m_pLayers[m_nLayers++] = pLayer;
}

/////////////////////////////////////////////////////////////////////////////
void CAGPage::Draw(CAGDC& dc, DWORD dwFlags) const
{
	// If the master layer is active, it is the only thing that is drawn
	if (GetActiveLayer() == GetMasterLayer())
	{
		if (m_pLayers[0])
			m_pLayers[0]->Draw(dc, dwFlags);
	}
	else
	{
		for (int i = 0; i < m_nLayers; i++)
		{
			if (m_pLayers[i])
				m_pLayers[i]->Draw(dc, dwFlags);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CAGPage::GetFonts(LOGFONTLIST& lfList)
{
	for (int i = 0; i < m_nLayers; i++)
		m_pLayers[i]->GetFonts(lfList);
}

//////////////////////////////////////////////////////////////////////
static void BumpNumericString(CString& strName, int iNewNumber)
{
	// If the string ends with a number, bump it
	int len = strName.GetLength();
	while (len > 0 && isdigit(strName[len-1]))
		len--;
	if (len <= 0)
		return;

	CString strBase = strName.Left(len);
	CString strNumber = strName.Mid(len);

	if (lstrlen(strNumber) > 0)
	{
		if (!iNewNumber) iNewNumber = atoi(strNumber) + 1;
		strName.Format("%s%d", strBase, iNewNumber);
	}
	else
	{
		if (!iNewNumber) iNewNumber = 2;
		strName.Format("%s %d", strBase, iNewNumber);
	}
}

/////////////////////////////////////////////////////////////////////////////
CAGPage* CAGPage::Duplicate(bool bCopySymbols, bool bFullPage, int iNewPageNumber, const CString& strNewPageName)
{
	SIZE PageSize = m_PageSize;
	if (bFullPage)
	{
		bool bPortrait = IsPortrait();
		double fSheetWidth = CAGDocSheetSize::GetWidth();
		double fSheetHeight = CAGDocSheetSize::GetHeight();
		PageSize.cx = (bPortrait ? INCHES(fSheetWidth) : INCHES(fSheetHeight));
		PageSize.cy = (bPortrait ? INCHES(fSheetHeight) : INCHES(fSheetWidth));
	}
		
	CAGPage* pNewPage = new CAGPage(PageSize.cx, PageSize.cy);
	if (!pNewPage)
		return NULL;

	if (!strNewPageName.IsEmpty())
		pNewPage->SetPageName(strNewPageName);
	else
	{
		CString strPage = m_szPageName;
		BumpNumericString(strPage, iNewPageNumber);
		pNewPage->SetPageName(strPage);
	}

	for (int i = 0; i < m_nLayers; i++)
	{
		CAGLayer* pLayer = GetLayer(i);
		if (!pLayer)
			continue;

		CAGLayer* pNewLayer = pLayer->Duplicate(bCopySymbols);
		if (pNewLayer)
			pNewPage->AddLayer(pNewLayer);
	}

	return pNewPage;
}

/////////////////////////////////////////////////////////////////////////////
CAGLayer* CAGPage::GetLayer(int nLayer) const
{
	if (nLayer < 0 || nLayer >= m_nLayers)
		return NULL;

	return m_pLayers[nLayer];
}

/////////////////////////////////////////////////////////////////////////////
CAGLayer* CAGPage::GetActiveLayer() const
{
	return GetLayer(m_nActiveLayer);
}

/////////////////////////////////////////////////////////////////////////////
CAGLayer* CAGPage::GetMasterLayer() const
{
	return GetLayer(0);
}

/////////////////////////////////////////////////////////////////////////////
CAGLayer* CAGPage::GetUserLayer() const
{
	return GetLayer(1);
}

/////////////////////////////////////////////////////////////////////////////
CAGLayer* CAGPage::SetActiveLayer(int nLayer)
{
	if (nLayer < 0 || nLayer >= m_nLayers)
		return NULL;

	m_nActiveLayer = nLayer;
	return m_pLayers[m_nActiveLayer];
}

//////////////////////////////////////////////////////////////////////
bool CAGPage::IsPortrait() const
{
	SIZE PageSize = {0,0};
	GetPageSize(PageSize);
	return (PageSize.cx <= PageSize.cy);
}

/////////////////////////////////////////////////////////////////////////////
bool CAGPage::IsEmpty() const
{
	for (int i = 0; i < m_nLayers; i++)
	{
		if (!m_pLayers[i]->IsEmpty())
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAGPage::Read(CAGDocIO* pInput) 
{
	if (!pInput)
		return false; // should never happen

	if (m_nLayers)
		return false; // should never happen

	BYTE cLength = 0;
	pInput->Read(&cLength, sizeof(cLength));
	if (cLength > 0)
	{
		char strPageName[256];
		pInput->Read(strPageName, cLength);
		strPageName[cLength] = '\0';
		m_szPageName = strPageName;
	}
	else
		m_szPageName = "";

	pInput->Read(&m_PageSize, sizeof(m_PageSize));
	if (m_PageSize.cx <= 0 || m_PageSize.cx > INCHES(120))
		return false; // should never happen
	if (m_PageSize.cy <= 0 || m_PageSize.cy > INCHES(120))
		return false; // should never happen

	pInput->Read(&m_nLayers, sizeof(m_nLayers));
	if (m_nLayers <= 0 || m_nLayers > 2)
		return false; // should never happen

	// Each page has exactly 2 layers
	for (int i = 0; i < m_nLayers; i++)
	{
		m_pLayers[i] = new CAGLayer();
		if (!m_pLayers[i]->Read(pInput))
			return false; // should never happen
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAGPage::Write(CAGDocIO* pOutput) const
{
	BYTE cLength = (BYTE)m_szPageName.GetLength();
	pOutput->Write(&cLength, sizeof(cLength));
	if (cLength > 0)
		pOutput->Write(m_szPageName, cLength);
	pOutput->Write(&m_PageSize, sizeof(m_PageSize));

	// Each page has exactly 2 layers
	pOutput->Write(&m_nLayers, sizeof(m_nLayers));
	for (int i = 0; i < m_nLayers; i++)
	{
		m_pLayers[i]->Write(pOutput);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CAGPage::CreateThumb(const SIZE& MaxSize, LPCSTR pszFileName, LPCSTR pszType, HBITMAP* phBitmap)
{
	SIZE PageSize = {0,0};
	GetPageSize(PageSize);

	double xScale = (double)(2 * MaxSize.cx) / PageSize.cx;
	double yScale = (double)(2 * MaxSize.cy) / PageSize.cy;
	double fScale = min(xScale, yScale);

	BITMAPINFO bi;
	::ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth  = dtoi(PageSize.cx * fScale);
	bi.bmiHeader.biHeight = dtoi(PageSize.cy * fScale);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biXPelsPerMeter = 3937; // 100 pixels/inch
	bi.bmiHeader.biYPelsPerMeter = 3937;

	CAGDIBSectionDC dc(bi, DIB_RGB_COLORS);
	if (!dc.GetHDC())
		return;

	RECT DibRect = {0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight};
	::FillRect(dc.GetHDC(), &DibRect, CAGBrush(WHITE_BRUSH, eStock));

	CAGMatrix Matrix(fScale, 0, 0, fScale);
	dc.SetDeviceMatrix(Matrix);

	Draw(dc, 0/*dwFlags*/);

	BITMAPINFOHEADER* pDibDouble = dc.ExtractDib();
	if (!pDibDouble)
		return;

	BITMAPINFOHEADER Dib = *pDibDouble;
	Dib.biWidth /= 2;
	Dib.biHeight /= 2;
	Dib.biSizeImage = 0;
	BITMAPINFOHEADER* pDib = (BITMAPINFOHEADER*)malloc(DibSize(&Dib));
	if (!pDib)
	{
		free(pDibDouble);
		return;
	}

	*pDib = Dib;

	ScaleImage(pDibDouble, pDib);

	if (phBitmap)
	{
		// Need to create BITMAPINFO struct for scaled version
		BITMAPINFO biScaled;
		::ZeroMemory(&biScaled, sizeof(biScaled));
		biScaled.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		biScaled.bmiHeader.biWidth  = Dib.biWidth;
		biScaled.bmiHeader.biHeight = Dib.biHeight;
		biScaled.bmiHeader.biPlanes = 1;
		biScaled.bmiHeader.biBitCount = 24;
		biScaled.bmiHeader.biCompression = BI_RGB;
		biScaled.bmiHeader.biXPelsPerMeter = 3937; // 100 pixels/inch
		biScaled.bmiHeader.biYPelsPerMeter = 3937;

		*phBitmap = ::CreateDIBitmap(
			dc.GetHDC(),	// handle to device context 
			pDib,			// pointer to bitmap size and format data 
			CBM_INIT,		// initialization flag 
			DibPtr(pDib),	// pointer to initialization data 
			&biScaled,		// pointer to bitmap color-format data 
			DIB_RGB_COLORS);
	}

	if (pszFileName)
	{
		CString strType(pszType);
		strType.MakeUpper();
		if (strType != "JPG")
			bool bOK = DibWrite(pDib, pszFileName);
		else if (strType == "JPG")
			ConvertDibToJpg(pDib, 50, pszFileName);
	}

	free(pDibDouble);
	free(pDib);
}
