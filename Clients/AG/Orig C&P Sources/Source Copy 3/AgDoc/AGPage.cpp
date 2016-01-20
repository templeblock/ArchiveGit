//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "AGPage.h"
#include "AGLayer.h"

#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] =__FILE__;
#define new DEBUG_NEW
#endif
#endif

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGPage::CAGPage(int cx, int cy)
{
	m_PageSize.cx = cx;
	m_PageSize.cy = cy;
	m_szPageName[0] = 0;

	m_nLayers = 0;
	for (int i = 0; i < MAX_AGLAYER; i++)
		m_pLayers[i] = NULL;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGPage::~CAGPage()
{
	for (int i = 0; i < m_nLayers; i++)
		delete m_pLayers[i];
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGPage::Draw(CAGDC& dc) const
{
	for (int i = 0; i < m_nLayers; i++)
		m_pLayers[i]->Draw(dc);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSym* CAGPage::FindSymbolByPoint(POINT Pt, SYMTYPE SymType) const
{
	for (int i = GetNumLayers(); i > 0; i--)
	{
		CAGLayer* pAGLayer = GetLayer(i);
		if (!pAGLayer)
			continue;
		CAGSym* pSym = pAGLayer->FindSymbolByPoint(Pt, SymType);
		if (pSym)
			return pSym;
	}

	return NULL;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGPage::GetFonts(LOGFONTARRAY& lfArray)
{
	for (int i = 0; i < m_nLayers; i++)
		m_pLayers[i]->GetFonts(lfArray);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGLayer* CAGPage::GetLayer(int nLayer) const
{
	if (nLayer <= 0 || nLayer > m_nLayers)
		return NULL;

	return m_pLayers[nLayer - 1];
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGPage::IsEmpty() const
{
	for (int i = 0; i < m_nLayers; i++)
	{
		if (!m_pLayers[i]->IsEmpty())
			return false;
	}

	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGPage::Read(CAGDocIO* pInput) 
{
	BYTE bPageLen = 0;
	pInput->Read(&bPageLen, sizeof(bPageLen));
	if (bPageLen > 0)
	{
		pInput->Read(m_szPageName, bPageLen);
		m_szPageName[bPageLen] = 0;
	}
	else
		m_szPageName[0] = 0;

	pInput->Read(&m_PageSize, sizeof(m_PageSize));
	pInput->Read(&m_nLayers, sizeof(m_nLayers));
	for (int i = 0; i < m_nLayers; i++)
	{
		CAGLayer* pLayer = new CAGLayer();
		pLayer->Read(pInput);
		m_pLayers[i] = pLayer;
	}

	return true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGPage::Write(CAGDocIO* pOutput) const
{
	BYTE bPageLen = (BYTE)lstrlen(m_szPageName);
	pOutput->Write(&bPageLen, sizeof(bPageLen));
	if (bPageLen > 0)
		pOutput->Write(m_szPageName, bPageLen);
	pOutput->Write(&m_PageSize, sizeof(m_PageSize));
	pOutput->Write(&m_nLayers, sizeof(m_nLayers));

	for (int i = 0; i < m_nLayers; i++)
	{
		CAGLayer* pLayer = m_pLayers[i];
		pLayer->Write(pOutput);
	}

	return true;
}
