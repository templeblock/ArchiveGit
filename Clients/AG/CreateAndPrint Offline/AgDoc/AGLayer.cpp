#include "stdafx.h"
#include "AGDoc.h"
#include "AGLayer.h"
#include "AGSym.h"
#include "AGDC.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

/////////////////////////////////////////////////////////////////////////////
CAGLayer::CAGLayer()
{
	m_nSymbols = 0;
	for (int i = 0; i < MAX_AGSYMBOLS; i++)
		m_pSymbols[i] = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CAGLayer::~CAGLayer()
{
	for (int i = 0; i < m_nSymbols; i++)
		delete m_pSymbols[i];
}

/////////////////////////////////////////////////////////////////////////////
int CAGLayer::GetNumSymbolsVisible() const
{
	int nSymbols = 0;
	for (int i = 0; i < m_nSymbols; i++)
	{
		CAGSym* pSym = m_pSymbols[i];
		if (!pSym)
			continue; // should never happen
		if (pSym->IsHidden())
			continue;

		nSymbols++;
	}

	return nSymbols;
}

/////////////////////////////////////////////////////////////////////////////
CAGLayer* CAGLayer::Duplicate(bool bCopySymbols)
{
	CAGLayer* pNewLayer = new CAGLayer();
	if (!pNewLayer)
		return NULL;

	if (!bCopySymbols)
		return pNewLayer;

	for (int i = 0; i < m_nSymbols; i++)
	{
		CAGSym* pSym = GetSymbol(i);
		if (!pSym)
			continue;

		CAGSym* pNewSym = pSym->Duplicate();
		if (pNewSym)
			pNewLayer->AddSymbol(pNewSym, false/*bAddToFront*/);
	}

	return pNewLayer;
}

/////////////////////////////////////////////////////////////////////////////
CAGSym* CAGLayer::GetSymbol(int iIndex) const
{
	if (iIndex < 0 || iIndex >= m_nSymbols)
		return NULL;

	return m_pSymbols[iIndex];
}

/////////////////////////////////////////////////////////////////////////////
void CAGLayer::AddSymbol(CAGSym* pSym, bool bAddToFront)
{
	if (!pSym)
		return;

	if (m_nSymbols >= MAX_AGSYMBOLS)
		return;

	if (bAddToFront)
	{
		// Move the affected symbols down one in the list
		for (int i = m_nSymbols; i > 0; i--)
			m_pSymbols[i] = m_pSymbols[i-1];

		// Add the symbol to the front
		m_pSymbols[0] = pSym;
	}
	else
	{
		// Add the symbol to the back
		m_pSymbols[m_nSymbols] = pSym;
	}

	m_nSymbols++;
}

/////////////////////////////////////////////////////////////////////////////
void CAGLayer::OrderSymbol(CAGSym* pSym, int iOffset)
{
	if (!pSym)
		return;

	int iPosition = FindSymbol(pSym);
	if (iPosition < 0)
		return;

	int iNewPosition = iPosition + iOffset;
	if (iNewPosition < 0)
		iNewPosition = 0;
	if (iNewPosition > m_nSymbols - 1)
		iNewPosition = m_nSymbols - 1;
	if (iNewPosition == iPosition)
		return;

	CAGSym* pSymTemp = m_pSymbols[iPosition];

	// Delete the symbol from the list
	m_pSymbols[iPosition] = NULL;
	m_nSymbols--;
	for (int i = iPosition; i < m_nSymbols; i++)
		m_pSymbols[i] = m_pSymbols[i+1];

	// Move the affected symbols down one in the list
	for (i = m_nSymbols; i > iNewPosition; i--)
		m_pSymbols[i] = m_pSymbols[i-1];

	// Add the symbol back in
	m_pSymbols[iNewPosition] = pSymTemp;
	m_nSymbols++;
}

/////////////////////////////////////////////////////////////////////////////
void CAGLayer::DeleteSymbol(CAGSym* pSym, bool bFromListOnly)
{
	if (!pSym)
		return;

	int i = FindSymbol(pSym);
	if (i < 0)
		return;

	if (!bFromListOnly)
		delete m_pSymbols[i];

	// Delete the symbol from the list
	m_pSymbols[i] = NULL;
	m_nSymbols--;
	for (; i < m_nSymbols; i++)
		m_pSymbols[i] = m_pSymbols[i+1];
}

/////////////////////////////////////////////////////////////////////////////
void CAGLayer::Draw(CAGDC& dc, DWORD dwFlags)
{
	// If there are AddAPhoto symbols on the page,
	// allow transparent images to cover to the edge when they draw
	bool bCoverAllowed = !!FindFirstSymbolByType(ST_ADDAPHOTO, dwFlags);
	dc.SetCoverAllowed(bCoverAllowed);

	// Draw back to front
	for (int i = m_nSymbols - 1; i >= 0; i--)
		m_pSymbols[i]->Draw(dc, dwFlags);
}

/////////////////////////////////////////////////////////////////////////////
int CAGLayer::FindSymbol(const CAGSym* pSym) const
{
	if (!pSym)
		return -1;

	for (int i = 0; i < m_nSymbols; i++)
	{
		if (pSym == m_pSymbols[i])
			return i;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
CAGSym* CAGLayer::FindNextSymbol(const CAGSym* pSymStart, int iDirection, DWORD dwFlags, bool bIgnoreLocked) const
{
	int iIndex = FindSymbol(pSymStart);
	if (iIndex < 0)
		return GetSymbol(0);

	for (int i = 0; i < m_nSymbols; i++)
	{
		if (iDirection < 0)
		{
			if (--iIndex < 0)
				iIndex += m_nSymbols;
		}
		else
		{
			if (++iIndex >= m_nSymbols)
				iIndex -= m_nSymbols;
		}

		CAGSym* pSym = m_pSymbols[iIndex];
		if (pSym->IsHidden(dwFlags))
			continue;

		if (bIgnoreLocked && pSym->IsImage() && ((CAGSymImage*)pSym)->IsLocked())
			continue;

		return pSym;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
CAGSym* CAGLayer::FindFirstSymbolByID(int nID, DWORD dwFlags) const
{
	for (int i = 0; i < m_nSymbols; i++)
	{
		CAGSym* pSym = m_pSymbols[i];
		if (pSym->IsHidden(dwFlags))
			continue;
		if (nID == pSym->GetID())
			return pSym;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
CAGSym* CAGLayer::FindFirstSymbolByType(SYMTYPE SymType, DWORD dwFlags) const
{
	for (int i = 0; i < m_nSymbols; i++)
	{
		CAGSym* pSym = m_pSymbols[i];
		if (pSym->IsHidden(dwFlags))
			continue;
		if ((SymType == pSym->GetType() || SymType == ST_ANY))
			return pSym;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
CAGSym* CAGLayer::FindAddAPhotoSymByPoint(POINT Pt, DWORD dwFlags) const
{
	for (int nSymbol = m_nSymbols-1; nSymbol >= 0; nSymbol--)
	{
		CAGSym* pSym = m_pSymbols[nSymbol];
		if (pSym->IsHidden(dwFlags))
			continue;
		if ((pSym->GetType() == ST_ADDAPHOTO) && pSym->HitTest(Pt, dwFlags))
			return pSym;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
CAGSym* CAGLayer::FindSymbolByPoint(POINT Pt, SYMTYPE SymType, DWORD dwFlags) const
{
	for (int i = 0; i < m_nSymbols; i++)
	{
		CAGSym* pSym = m_pSymbols[i];
		if (pSym->IsHidden(dwFlags))
			continue;
		if ((SymType == pSym->GetType() || SymType == ST_ANY) && pSym->HitTest(Pt, dwFlags))
			return pSym;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CAGLayer::GetFonts(LOGFONTLIST& lfList)
{
	for (int i = 0; i < m_nSymbols; i++)
	{
		CAGSym* pSym = m_pSymbols[i];
		if (pSym->IsText() || pSym->IsAddAPhoto())
			((CAGSymText*)pSym)->GetFonts(lfList);
		else
		if (pSym->IsCalendar())
			((CAGSymCalendar*)pSym)->GetFonts(lfList);
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CAGLayer::IsEmpty()
{
	return (m_nSymbols <= 0);
}

/////////////////////////////////////////////////////////////////////////////
bool CAGLayer::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	if (m_nSymbols)
		return false; // should never happen

	m_nSymbols = 0;
	pInput->Read(&m_nSymbols, sizeof(m_nSymbols));
	if (m_nSymbols < 0 || m_nSymbols > MAX_AGSYMBOLS)
		return false; // should never happen

	for (int i = 0; i < m_nSymbols; i++)
	{
		BYTE cType = 0;
		pInput->Read(&cType, sizeof(cType));

		SYMTYPE SymType = (SYMTYPE)cType;
		CAGSym* pSym = NULL;
		switch (SymType)
		{
			case ST_TEXT:
			case ST_ADDAPHOTO:
			{
				pSym = new CAGSymText(SymType);
				break;
			}
			
			case ST_IMAGE:
			case ST_IMAGE_OLD: // Obsolete - if found in an file, create a ST_IMAGE instead
			{
				bool bOldReader = (SymType == ST_IMAGE_OLD);
				pSym = new CAGSymImage(bOldReader);
				break;
			}

			case ST_RECTANGLE:
			case ST_COLORBOX_OLD: // Obsolete - if found in an file, create a ST_RECTANGLE instead
			{
				bool bReadAsObsoleteColorbox = (SymType == ST_COLORBOX_OLD);
				pSym = new CAGSymRectangle(bReadAsObsoleteColorbox);
				break;
			}

			case ST_ELLIPSE:
			{
				pSym = new CAGSymEllipse();
				break;
			}

			case ST_LINE:
			{
				pSym = new CAGSymLine();
				break;
			}

			case ST_DRAWING:
			{
				pSym = new CAGSymDrawing();
				break;
			}

			case ST_CALENDAR:
			{
				pSym = new CAGSymCalendar();
				break;
			}
		}

		if (!pSym || !pSym->Read(pInput))
		{
			m_nSymbols = i - 1;
			return false; // should never happen
		}

		m_pSymbols[i] = pSym;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CAGLayer::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	// Get the number symbols that have not been deleted
	int nSymbols = 0;
	for (int i = 0; i < m_nSymbols; i++)
	{
		CAGSym* pSym = m_pSymbols[i];
		if (!pSym)
			continue; // should never happen
		if (pSym->IsDeleted())
			continue;

		nSymbols++;
	}

	pOutput->Write(&nSymbols, sizeof(nSymbols));

	for (i = 0; i < m_nSymbols; i++)
	{
		CAGSym* pSym = m_pSymbols[i];
		if (!pSym)
			continue; // should never happen
		if (pSym->IsDeleted())
			continue;

		pSym->Write(pOutput);
	}

	return true;
}
