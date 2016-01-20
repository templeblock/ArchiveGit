//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "AGLayer.h"
#include "AGSym.h"

#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] =__FILE__;
#define new DEBUG_NEW
#endif
#endif


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGLayer::CAGLayer()
{
	m_nSymbols = 0;
	for (int i = 0; i < MAX_AGSYMBOLS; i++)
		m_pSymbols[i] = NULL;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGLayer::~CAGLayer()
{
	for (int i = 0; i < m_nSymbols; i++)
		delete m_pSymbols[i];
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGLayer::AppendSymbol(CAGSym* pSym)
{
	m_pSymbols[m_nSymbols] = pSym;
	m_nSymbols++;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGLayer::Draw(CAGDC& dc)
{
	for (int i = 0; i < m_nSymbols; i++)
		m_pSymbols[i]->Draw(dc);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSym* CAGLayer::FindFirstSymbolByType(SYMTYPE SymType) const
{
	for (int i = 0; i < m_nSymbols; i++)
	{
		if ((SymType == m_pSymbols[i]->GetSymType() || SymType == ST_ANY))
			return m_pSymbols[i];
	}

	return NULL;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSym* CAGLayer::FindSymbolByPoint(POINT Pt, SYMTYPE SymType) const
{
	for (int i = 0; i < m_nSymbols; i++)
	{
		if ((SymType == m_pSymbols[i]->GetSymType() || SymType == ST_ANY) && m_pSymbols[i]->HitTest(Pt))
			return m_pSymbols[i];
	}

	return NULL;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGLayer::GetFonts(LOGFONTARRAY& lfArray)
{
	for (int i = 0; i < m_nSymbols; i++)
	{
		if (m_pSymbols[i]->GetSymType() == ST_TEXT)
			((CAGSymText*)m_pSymbols[i])->GetFonts(lfArray);
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGLayer::IsEmpty()
{
	return (m_nSymbols <= 0);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGLayer::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false;

	if (m_nSymbols)
		return false; // should never happen

	m_nSymbols = 0;
	pInput->Read(&m_nSymbols, sizeof(m_nSymbols));
	if (m_nSymbols < 0)
		m_nSymbols = 0; // should never happen
	if (m_nSymbols > MAX_AGSYMBOLS)
		m_nSymbols = MAX_AGSYMBOLS; // should never happen

	for (int i = 0; i < m_nSymbols; i++)
	{
		SYMTYPE SymType = ST_ANY;
		pInput->Read(&SymType, sizeof(SymType));

		CAGSym* pSym = NULL;
		switch (SymType)
		{
			case ST_IMAGE:
				pSym = new CAGSymImage();
				break;

			case ST_TEXT:
				pSym = new CAGSymText();
				break;
		}

		if (!pSym)
			return false; // should never happen

		pSym->Read(pInput);
		m_pSymbols[i] = pSym;
	}

	return true;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGLayer::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	pOutput->Write(&m_nSymbols, sizeof(m_nSymbols));

	for (int i = 0; i < m_nSymbols; i++)
	{
		CAGSym* pSym = m_pSymbols[i];
		if (!pSym)
			continue; // should never happen
		SYMTYPE SymType = pSym->GetSymType();
		pOutput->Write(&SymType, sizeof(SymType));
		pSym->Write(pOutput);
	}

	return true;
}
