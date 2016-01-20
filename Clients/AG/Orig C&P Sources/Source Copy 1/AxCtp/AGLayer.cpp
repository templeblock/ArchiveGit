//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "AGLayer.h"
#include "AGSym.h"

#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGLayer::CAGLayer()
{
	for (int i = 0; i < MAX_AGSYMBOLS; i++)
		m_pSymbols[i] = NULL;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGLayer::~CAGLayer()
{
	for (int i = 0; m_pSymbols[i]; i++)
		delete m_pSymbols[i];
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGLayer::AppendSymbol(CAGSym *pSym)
{
	for (int i = 0; m_pSymbols[i]; i++)
		;

	m_pSymbols[i] = pSym;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGLayer::Draw(CAGDC &dc)
{
	for (int i = 0; m_pSymbols[i]; i++)
		m_pSymbols[i]->Draw(dc);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSym *CAGLayer::FindFirstSymbolByType(SYMTYPE SymType) const
{
	CAGSym *pSym = NULL;

	for (int i = 0; m_pSymbols[i] && pSym == NULL; i++)
	{
		if (m_pSymbols[i]->GetSymType() == SymType)
			pSym = m_pSymbols[i];
	}
	return (pSym);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSym *CAGLayer::FindSymbolByPoint(POINT Pt, SYMTYPE SymType) const
{
	CAGSym *pSym = NULL;

	for (int i = 0; m_pSymbols[i] && pSym == NULL; i++)
	{
		if (SymType == ST_ANY || m_pSymbols[i]->GetSymType() == SymType)
		{
			if (m_pSymbols[i]->HitTest(Pt))
				pSym = m_pSymbols[i];
		}
	}
	return (pSym);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGLayer::GetFonts(LOGFONTARRAY &lfArray)
{
	for (int i = 0; m_pSymbols[i]; i++)
	{
		if (m_pSymbols[i]->GetSymType() == ST_TEXT)
		{
			CAGSymText *pText = (CAGSymText *)m_pSymbols[i];
			pText->GetFonts(lfArray);
		}
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGLayer::IsEmpty()
{
	if (m_pSymbols[0])
		return false;
	else
		return true;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGLayer::Read(CAGDocIO *pInput)
{
	int nSymbols;
	pInput->Read(&nSymbols, sizeof(nSymbols));

	for (int i = 0; i < nSymbols; i++)
	{
		SYMTYPE SymType;
		pInput->Read(&SymType, sizeof(SymType));

		CAGSym *pSym = NULL;
		switch (SymType)
		{
			case ST_IMAGE:
				pSym = new CAGSymImage();
				break;

			case ST_TEXT:
				pSym = new CAGSymText();
				break;
		}

		if (pSym)
		{
			pSym->Read(pInput);
			m_pSymbols[i] = pSym;
		}
	}

	return true;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGLayer::Write(CAGDocIO *pOutput)
{
	bool bReturn = true;

	int nSymbols = 0;
	for (nSymbols; m_pSymbols[nSymbols]; nSymbols++)
		;
	
	pOutput->Write(&nSymbols, sizeof(nSymbols));

	for (int i = 0; i < nSymbols; i++)
	{
		CAGSym *pSym = m_pSymbols[i];
		SYMTYPE SymType = pSym->GetSymType();
		pOutput->Write(&SymType, sizeof(SymType));
		pSym->Write(pOutput);
	}

	return bReturn;
}

