#pragma once

#include <iostream>
using namespace std;

#include "zutil.h"

#include <vector>
typedef std::vector<LOGFONT> LOGFONTLIST;

// US Letter paper sizes: L1, L2, L4
#define L1W		(8.5)		// 8.5
#define L1H		(11.0)		// 11.0
#define L2W		(11.0 / 2)	// 5.5
#define L2H		(8.5)		// 8.5
#define L4W		(8.5 / 2)	// 4.25
#define L4H		(11.0 / 2)	// 5.5

// Metric paper sizes: A4, A5, A6
#define InchesPerMM 0.03937
#define m(x)	(InchesPerMM * x)

#define A4W		m(210)		// 8.2677
#define A4H		m(297)		// 11.69289
#define A5W		m(148)		// 5.82676
#define A5H		m(210)		// 8.2677
#define A6W		m(105)		// 4.13385
#define A6H		m(148)		// 5.82676

// Metric envelopes: C5, C6, C65, DL
#define C5W		m(229)		// 9.01573 fits A5H
#define C5H		m(162)		// 6.37794 fits A5W
#define C6W		m(162)		// 6.37794 fits A6H
#define C6H		m(114)		// 4.48818 fits A6W
#define C65W	m(229)		// 9.01573 fits A5H
#define C65H	m(114)		// 4.48818 fits A6W
#define DLW		m(220)		// 8.6614 fits A4W
#define DLH		m(110)		// 4.3307 fits A4H in thirds

class CAGDCInfo;
class CAGDC;
class CAGPaintDC;
class CAGClientDC;
class CAGDIBSectionDC;
class CAGMatrix;
class CAGSpec;
class CAGText;
class CAGSym;
class CAGSymImage;
class CAGSymImageNative;
class CAGSymText;
class CAGSymRectangle;
class CAGSymEllipse;
class CAGSymLine;
class CAGSymDrawing;
class CAGSymCalendar;
class CAGLayer;
class CAGPage;
class CAGPrintDC;

class CAGDocSheetSize
{
public:
	static void SetMetric(bool bOn)
	{
		m_bMetric = bOn;
		if (m_bMetric)
		{
			m_fWidth = A4W;
			m_fHeight = A4H;
		}
		else
		{
			m_fWidth = L1W;
			m_fHeight = L1H;
		}
	}
	static bool GetMetric()
		{ return m_bMetric; }
	static void Set(double fWidth, double fHeight)
		{ m_fWidth = fWidth; m_fHeight = fHeight; }
	static double GetWidth()
		{ return m_fWidth; }
	static double GetHeight()
		{ return m_fHeight; }
private:
	static bool m_bMetric;
	static double m_fWidth;
	static double m_fHeight;
};

class CAGDocIO
{
public:
	CAGDocIO(istream* pInput);
	CAGDocIO(ostream* pOutput);
	~CAGDocIO();

	void Close();
	void Read(void* pData, DWORD dwSize);
	void Write(const void* pData, DWORD dwSize);
	DWORD GetOutputCount()
	{
		return m_dwOutputCount;
	}

protected:
	bool m_bEOF;
	bool m_bClosed;
	istream* m_pInput;
	ostream* m_pOutput;
	z_stream m_zstream;
	BYTE* m_zBuf;
	int m_zErr;
	DWORD m_dwOutputCount;
};

#define MAX_AGPAGE	30

enum AGDOCTYPE
{
	DOC_DEFAULT			 = 0,
	DOC_HALFCARD		 = 1,
	DOC_HALFCARD_OLD	 = 2,	// Obsolete
	DOC_QUARTERCARD		 = 3,
	DOC_QUARTERCARD_OLD	 = 4,	// Obsolete
	DOC_BANNER			 = 5,
	DOC_BLANKPAGE_OLD	 = 6,
	DOC_BROCHURE		 = 7,
	DOC_BUSINESSCARD	 = 8,
	DOC_CALENDAR_OLD	 = 9,	// Obsolete
	DOC_CDLABEL			 = 10,
	DOC_CERTIFICATE_OLD	 = 11,	// Obsolete
	DOC_CRAFT_OLD		 = 12,	// Obsolete
	DOC_ENVELOPE		 = 13,
	DOC_FULLSHEET		 = 14,
	DOC_LABEL			 = 15,
	DOC_STATIONERY_OLD	 = 16,	// Obsolete
	DOC_NOTECARD_OLD	 = 17,	// Obsolete
	DOC_NEWSLETTER_OLD	 = 18,	// Obsolete
	DOC_PHOTOCARD		 = 19,
	DOC_POSTCARD		 = 20,
	DOC_POSTER_OLD		 = 21,	// Obsolete
	DOC_STICKER_OLD		 = 22,	// Obsolete
	DOC_IRONON			 = 23,
	DOC_GIFTNAMECARD	 = 24,
	DOC_CDBOOKLET		 = 25,
	DOC_WEBPUB_OLD		 = 26,	// Obsolete
	DOC_TRIFOLD			 = 27,
	DOC_HOLIDAYCARD		 = 28,
	DOC_NOTECARD		 = 29,
	DOC_LAST			 = 29,
};

enum LineType   {LT_None = 0, LT_Hairline = -1, LT_Normal = 1};
enum FillType   {FT_None, FT_Solid, FT_SweepRight, FT_SweepDown, FT_RadialCenter, FT_RadialCorner};
enum ShadowType {ST_None, ST_Drop, ST_Extrude};
enum LineStart  {LS_LeftTop, LS_RightTop, LS_RightBottom, LS_LeftBottom};

class CAGDocTypes
{
public:
	static const CString Name(AGDOCTYPE DocType)
	{
		switch (DocType)
		{
			case DOC_BANNER:		return "Banner";
			case DOC_BROCHURE:		return "Brochure";
			case DOC_BUSINESSCARD:	return "Business Card";
			case DOC_CDBOOKLET:		return "CD Booklet";
			case DOC_CDLABEL:		return "CD Label";
			case DOC_ENVELOPE:		return "Envelope";
			case DOC_FULLSHEET:		return "Full Sheet";
			case DOC_GIFTNAMECARD:	return "Gift/Name Card";
			case DOC_HALFCARD:		return "Card ½ fold";
			case DOC_HOLIDAYCARD:	return "4x8 Photo Card";
			case DOC_IRONON:		return "Iron-on Transfer";
			case DOC_LABEL:			return "Label/Sticker";
			case DOC_NOTECARD:		return "Note Card";
			case DOC_PHOTOCARD:		return "Photo Card";
			case DOC_POSTCARD:		return "Postcard";
			case DOC_QUARTERCARD:	return "Card ¼ fold";
			case DOC_TRIFOLD:		return "Trifold";
			default:				return "Unknown";
		}
	}
};

class CAGDoc
{
public:
	CAGDoc(AGDOCTYPE AGDocType = DOC_DEFAULT);
	~CAGDoc();

	bool AllowNewPages() const;
	bool AllowEnvelopeWizard() const;
	bool NeedsCardBack() const;
	void Free();
	CAGDoc* Duplicate();
	bool IsModified() const
		{ return m_bModified; }
	void SetModified(bool bModified)
		{ m_bModified = bModified; }
	bool IsPortrait(int nPage = -1) const;
	void SetPortrait(bool bPortrait)
		{ m_bPortrait = bPortrait; }
	AGDOCTYPE GetDocType() const
		{ return m_AGDocType; }
	void SetDocType(AGDOCTYPE DocType)
		{ m_AGDocType = DocType; }
	int GetNumPages() const
		{ return m_nPages; }
	CAGPage* GetCurrentPage() const
		{ return GetPage(m_nCurPage); }
	int GetCurrentPageNum() const
		{ return m_nCurPage; }
	void AddPage(CAGPage* pPage);
	bool DeleteCurrentPage();
	void GetFonts(LOGFONTLIST& lfList) const;
	CAGPage* GetPage(int nPage) const;
	void GetPageSize(SIZE& PageSize) const;
	void GetOrigPageSize(SIZE& OrigPageSize) const
		{ OrigPageSize = m_OrigPageSize; }
	void SetOrigPageSize(SIZE& OrigPageSize)
		{ m_OrigPageSize = OrigPageSize; }
	void GetPaperSize(SIZE& PaperSize, double fWidth = NULL, double fHeight = NULL) const;
	void SetCurrentPageNum(int nPage)
		{ if (nPage >= 0 && nPage < m_nPages) m_nCurPage = nPage; }
	void SetFileName(CString& strFullDocPath);
	void GetFileName(CString& strFullDocPath, CString& strPath, CString& strFileName) const;
	void SetMasterLayer(bool bMasterLayer);
	int FindSymbolAnywhere(const CAGSym* pSym, CAGPage** pPageFound, CAGLayer** pLayerFound) const;
	CAGSym* FindSymbolAnywhereByID(int nID, DWORD dwFlags) const;
	bool HasLockedImages() const;
	bool HasTransparentImages() const;
	bool Read(istream& input, bool& bAdjusted, bool bAllowAdjustment = true);
	bool Write(ostream& output) const;
	DWORD GetCompressedSize() const;

protected:
	bool m_bModified;
	bool m_bPortrait;
	AGDOCTYPE m_AGDocType;
	int m_nPages;
	int m_nCurPage;
	CAGPage* m_pPages[MAX_AGPAGE];
	CString m_strFullDocPath;
	SIZE m_OrigPageSize;
};
