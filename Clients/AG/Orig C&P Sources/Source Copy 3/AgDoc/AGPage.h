#ifndef __AGPAGE_H_
#define __AGPAGE_H_

#include "AGDoc.h"
#include "AGSym.h"
#include "AGDC.h"

class CAGLayer;

#define MAX_AGLAYER 10

class CAGPage  
{
public:
	CAGPage(int cx = 0, int cy = 0);
	~CAGPage();

	void AppendLayer(CAGLayer* pLayer)
		{ m_pLayers[m_nLayers++] = pLayer; }
	void Draw(CAGDC& dc) const;
	CAGSym* FindSymbolByPoint(POINT Pt, SYMTYPE SymType = ST_ANY) const;
	void GetFonts(LOGFONTARRAY& lfArray);
	CAGLayer* GetLayer(int nLayer) const;
	int GetNumLayers() const
		{ return (m_nLayers); }
	const char* GetPageName() const
		{ return (m_szPageName); }
	void GetPageSize(SIZE* pSize) const
		{ *pSize = m_PageSize; }
	bool IsEmpty() const;
	bool Read(CAGDocIO* pInput);
	void SetPageName(const char* pszPageName)
		{ lstrcpyn(m_szPageName, pszPageName, sizeof(m_szPageName)); }
	bool Write(CAGDocIO* pOutput) const;

protected:
	SIZE m_PageSize;
	char m_szPageName[255];
	int m_nLayers;
	CAGLayer* m_pLayers[MAX_AGLAYER];
};

#endif //__AGPAGE_H_
