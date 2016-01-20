#pragma once

#include "AGDoc.h"
#include "AGSym.h"

#define MAX_AGSYMBOLS 500

class CAGLayer	
{
public:
	CAGLayer();
	~CAGLayer();

	int GetNumSymbols()
		{ return m_nSymbols; }
	int GetNumSymbolsVisible() const;
	CAGLayer* Duplicate(bool bCopySymbols);
	CAGSym* GetSymbol(int iIndex) const;
	void AddSymbol(CAGSym* pSym, bool bAddToFront = true);
	void DeleteSymbol(CAGSym* pSym, bool bFromListOnly = false);
	void OrderSymbol(CAGSym* pSym, int iOffset);
	void Draw(CAGDC &dc, DWORD dwFlags);
	int FindSymbol(const CAGSym* pSym) const;
	CAGSym* FindNextSymbol(const CAGSym* pSymStart, int iDirection, DWORD dwFlags, bool bIgnoreLocked) const;
	CAGSym* FindFirstSymbolByID(int nID, DWORD dwFlags) const;
	CAGSym* FindFirstSymbolByType(SYMTYPE SymType, DWORD dwFlags) const;
	CAGSym* FindSymbolByPoint(POINT Pt, SYMTYPE SymType, DWORD dwFlags) const;
	CAGSym* FindAddAPhotoSymByPoint(POINT Pt, DWORD dwFlags) const;
	void GetFonts(LOGFONTLIST &lfList);
	bool IsEmpty();
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput);

protected:

	int m_nSymbols;
	CAGSym* m_pSymbols[MAX_AGSYMBOLS];
};
