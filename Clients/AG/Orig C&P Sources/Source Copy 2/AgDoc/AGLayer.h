#ifndef __AGLAYER_H_
#define __AGLAYER_H_

#include "AGDoc.h"
#include "AGSym.h"
#include "AGDC.h"

class CAGSym;

#define MAX_AGSYMBOLS 100



class CAGLayer	
{
public:
	CAGLayer();
	~CAGLayer();

	void AppendSymbol(CAGSym *pSym);
	void Draw(CAGDC &dc);
	CAGSym *FindFirstSymbolByType(SYMTYPE SymType) const;
	CAGSym *FindSymbolByPoint(POINT Pt, SYMTYPE SymType = ST_ANY) const;
	void GetFonts(LOGFONTARRAY &lfArray);
	bool IsEmpty();
	bool Read(CAGDocIO *pInput);
	bool Write(CAGDocIO *pOutput);

protected:

	CAGSym	*m_pSymbols[MAX_AGSYMBOLS];

};

#endif //__AGLAYER_H_
