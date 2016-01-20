#ifndef __AGDOC_H_
#define __AGDOC_H_

#include <vector>
typedef std::vector<LOGFONT> LOGFONTARRAY;

#include <iostream.h>
#include "zutil.h"

#include "AGDC.h"
class CAGPage;

#define MAX_AGPAGE	4
#define Z_BUFSIZE	16384


enum AGDOCTYPE
{
	DOC_DEFAULT,
	DOC_CARD_SINGLEFOLD_PORTRAIT,
	DOC_CARD_SINGLEFOLD_LANDSCAPE,
	DOC_CARD_QUARTERFOLD_PORTRAIT,
	DOC_CARD_QUARTERFOLD_LANDSCAPE,
};

enum PRINTSIDE
{
	PRINT_OUTSIDE,
	PRINT_INSIDE,
	PRINT_BOTH
};

class CAGDocIO
{
public:
	CAGDocIO(istream *pInput);
	CAGDocIO(ostream *pOutput);

	void Close();
	void Read(void *pData, DWORD dwSize);
	void Write(const void *pData, DWORD dwSize);

protected:
	bool		m_bInput;
	bool		m_bEOF;
	istream 	*m_pInput;
	ostream 	*m_pOutput;
	z_stream	m_zstream;
	BYTE		m_zBuf[Z_BUFSIZE];
	int 		m_zErr;
};




class CAGDoc  
{
public:
	CAGDoc(AGDOCTYPE AGDocType = DOC_DEFAULT);
	~CAGDoc();

	void AppendPage(CAGPage *pPage)
		{ m_pPages[m_nPages++] = pPage; m_nCurPage = m_nPages; };
	void Free();
	AGDOCTYPE GetDocType()
		{ return (m_AGDocType); }
	int GetNumPages()
		{ return (m_nPages); }
	CAGPage *GetCurrentPage()
		{ return (GetPage(m_nCurPage)); }
	int GetCurrentPageNum()
		{ return (m_nCurPage); }
	void GetFonts(LOGFONTARRAY &lfArray);
	CAGPage *GetPage(int nPage);
	bool PrintCardQuarter(char *pszDriver, char *pszDevice, char *pszOutput,
	  DEVMODE *pDevMode, const char *pszFileName = NULL);
	bool PrintCardSingle(PRINTSIDE PrintSide, char *pszDriver,
	  char *pszDevice, char *pszOutput, DEVMODE *pDevMode, bool &bRotated,
	  const char *pszFileName = NULL);
	bool Read(istream &input);
	void SetCurrentPage(int nPage)
		{ if (nPage > 0 && nPage <= m_nPages) m_nCurPage = nPage; }
	bool Write(ostream &output);

protected:
	void GetQFPageRect(int nPage, bool bPortrait, bool bMaxMargin,
	  CAGDC *pDC, CAGDCInfo &di, RECT &DestRect, bool &bFlip, bool &bRotated);
	void GetSFPageRect(int nPage, bool bPortrait, bool bMaxMargin,
	  CAGDC *pDC, CAGDCInfo &di, RECT &DestRect, bool &bFlip, bool &bRotated);

protected:
	AGDOCTYPE	m_AGDocType;
	int 		m_nPages;
	int 		m_nCurPage;
	CAGPage 	*m_pPages[MAX_AGPAGE];
};

#endif //__AGDOC_H_
