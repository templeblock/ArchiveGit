#include "stdafx.h"
#include "AGDoc.h"
#include "AGPage.h"
#include "AGDC.h"
#include "PaperTemplates.h"
#include "MyAtlUrl.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define AGDOC_ID0		'A'
#define AGDOC_ID1		'g'
#define AGDOC_VERSION	3
#define Z_BUFSIZE		16384
#define Z_SLOP			100

struct AGHDR
{
	BYTE Id0;
	BYTE Id1;
	BYTE Ver;
};

// Because this has to be static, it is shared between instances
bool CAGDocSheetSize::m_bMetric = false;
double CAGDocSheetSize::m_fWidth = L1W;
double CAGDocSheetSize::m_fHeight = L1H;

//////////////////////////////////////////////////////////////////////
CAGDoc::CAGDoc(AGDOCTYPE AGDocType)
{
	m_bModified = false;
	m_bPortrait = false;
	m_AGDocType = AGDocType;
	m_nCurPage = 0;
	m_nPages = 0;
	for (int i = 0; i < MAX_AGPAGE; i++)
		m_pPages[i] = NULL;
}

//////////////////////////////////////////////////////////////////////
CAGDoc::~CAGDoc()
{
	Free();
}

//////////////////////////////////////////////////////////////////////
bool CAGDoc::AllowNewPages() const
{
	if (
		m_AGDocType == DOC_DEFAULT		||
		m_AGDocType == DOC_BANNER		||
		m_AGDocType == DOC_BROCHURE		||
//		m_AGDocType == DOC_BUSINESSCARD	||
		m_AGDocType == DOC_CDBOOKLET	||
//		m_AGDocType == DOC_CDLABEL		||
//		m_AGDocType == DOC_ENVELOPE		||
//		m_AGDocType == DOC_FULLSHEET	||
//		m_AGDocType == DOC_GIFTNAMECARD	||
		m_AGDocType == DOC_HALFCARD		||
//		m_AGDocType == DOC_HOLIDAYCARD	||
//		m_AGDocType == DOC_IRONON		||
//		m_AGDocType == DOC_LABEL		||
		m_AGDocType == DOC_NOTECARD		||
//		m_AGDocType == DOC_PHOTOCARD	||
		m_AGDocType == DOC_POSTCARD		||
		m_AGDocType == DOC_QUARTERCARD	||
		m_AGDocType == DOC_TRIFOLD		||
		0)
			return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAGDoc::AllowEnvelopeWizard() const
{
	if (m_AGDocType == DOC_HALFCARD		||
		m_AGDocType == DOC_HOLIDAYCARD	||
		m_AGDocType == DOC_NOTECARD		||
		m_AGDocType == DOC_PHOTOCARD	||
		m_AGDocType == DOC_QUARTERCARD)
			return !HasLockedImages();

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CAGDoc::NeedsCardBack() const
{
	if (m_nPages < 4)
		return false;

	if (m_AGDocType == DOC_HALFCARD		||
		m_AGDocType == DOC_QUARTERCARD	||
		m_AGDocType == DOC_NOTECARD		||
		m_AGDocType == DOC_CDBOOKLET)
			return true;

	return false;
}

//////////////////////////////////////////////////////////////////////
void CAGDoc::SetFileName(CString& strFullDocPath)
{
	UnescapeUrl(strFullDocPath, m_strFullDocPath);
}

//////////////////////////////////////////////////////////////////////
void CAGDoc::GetFileName(CString& strFullDocPath, CString& strPath, CString& strFileName) const
{
	strFullDocPath = m_strFullDocPath;

	int i = max(strFullDocPath.ReverseFind('/'), strFullDocPath.ReverseFind('\\'));
	strFileName = strFullDocPath.Mid(i+1);
	strPath = strFullDocPath.Left(i);
	if (strPath.Left(7) == "file://")
		strPath = strPath.Mid(7);
	if (strPath.Left(7) == "http://" || strPath.Find("product") >= 0)
		strPath.Empty(); // We don't want users to be able to overwrite web files or production files
}

//////////////////////////////////////////////////////////////////////
void CAGDoc::Free() 
{
	for (int i = 0; i < m_nPages; i++)
	{
		delete m_pPages[i];
		m_pPages[i] = NULL;
	}

	m_nCurPage = 0;
	m_nPages = 0;
}

//////////////////////////////////////////////////////////////////////
CAGDoc* CAGDoc::Duplicate()
{
	CAGDoc* pNewDoc = new CAGDoc();
	if (!pNewDoc)
		return NULL;

	for (int i = 0; i < m_nPages; i++)
	{
		CAGPage* pPage = GetPage(i);
		if (!pPage)
			continue;

		CString strPageName;
		pPage->GetPageName(strPageName);
		CAGPage* pNewPage = pPage->Duplicate(true/*bCopySymbols*/, true/*bFullPage*/, i/*iNewPageNumber*/, strPageName);
		if (pNewPage)
			pNewDoc->AddPage(pNewPage);
	}

	return pNewDoc;
}

//////////////////////////////////////////////////////////////////////
bool CAGDoc::IsPortrait(int nPage) const
{
	CAGPage* pPage = GetPage(nPage);
	return (!pPage ? m_bPortrait : pPage->IsPortrait()); 
}

//////////////////////////////////////////////////////////////////////
void CAGDoc::AddPage(CAGPage* pPage)
{
	if (!pPage)
		return;

	m_nCurPage = m_nPages;
	m_pPages[m_nPages++] = pPage;
}

//////////////////////////////////////////////////////////////////////
bool CAGDoc::DeleteCurrentPage()
{
	// Cannot delete the only page in a document
	if (m_nPages <= 1)
		return false;

	bool bDeleted = false;
	for (int i = 0; i < m_nPages; i++)
	{
		if (i == m_nCurPage)
		{
			delete m_pPages[i];
			m_pPages[i] = NULL;
			bDeleted = true;
		}
		else			
		if (bDeleted)
		{
			m_pPages[i-1] = m_pPages[i];
			m_pPages[i] = NULL;
		}
	}

	if (bDeleted)
		m_nPages--;

	return bDeleted;
}

//////////////////////////////////////////////////////////////////////
void CAGDoc::GetFonts(LOGFONTLIST& lfList) const
{
	for (int i = 0; i < m_nPages; i++)
		m_pPages[i]->GetFonts(lfList);
}

//////////////////////////////////////////////////////////////////////
CAGPage* CAGDoc::GetPage(int nPage) const
{
	if (nPage < 0 || nPage >= m_nPages)
		return NULL;
	else
		return m_pPages[nPage];
}

//////////////////////////////////////////////////////////////////////
void CAGDoc::GetPageSize(SIZE& PageSize) const
{
	PageSize.cx = 0;
	PageSize.cy = 0;
	CAGPage* pPage = GetPage(0);
	if (pPage)
		pPage->GetPageSize(PageSize);
}

//////////////////////////////////////////////////////////////////////
void CAGDoc::GetPaperSize(SIZE& PaperSize, double fWidth, double fHeight) const
{ // return the paper size required by the document (in tenths of a millimeter)
	if (!fWidth && !fHeight)
	{
		if (m_AGDocType == DOC_BANNER || m_AGDocType == DOC_ENVELOPE)
		{
			SIZE PageSize;
			GetPageSize(PageSize);
			fWidth = DINCHES(PageSize.cx);
			fHeight = DINCHES(PageSize.cy);
		}
		else
		{
			fWidth = CAGDocSheetSize::GetWidth();
			fHeight = CAGDocSheetSize::GetHeight();
		}
	}

	PaperSize.cx = dtoi(254.0 * fWidth);
	PaperSize.cy = dtoi(254.0 * fHeight);
}

//////////////////////////////////////////////////////////////////////
void CAGDoc::SetMasterLayer(bool bMasterLayer)
{
	int nActiveLayer = (bMasterLayer ? 0 : 1);
	for (int i = 0; i < m_nPages; i++)
	{
		m_pPages[i]->SetActiveLayer(nActiveLayer);
	}
}

/////////////////////////////////////////////////////////////////////////////
int CAGDoc::FindSymbolAnywhere(const CAGSym* pSym, CAGPage** pPageFound, CAGLayer** pLayerFound) const
{
	if (!pSym)
		return -1;

	int nPages = GetNumPages();
	for (int i = 0; i < nPages; i++)
	{
		CAGPage* pPage = GetPage(i);
		if (!pPage)
			return -1;

		int nLayers = pPage->GetNumLayers();
		for (int j = 0; j < nLayers; j++)
		{
			CAGLayer* pLayer = pPage->GetLayer(j);
			if (!pLayer)
				return -1;

			int iPosition = pLayer->FindSymbol(pSym);
			if (iPosition >= 0)
			{
				if (pPageFound)
					*pPageFound = pPage;
				if (pLayerFound)
					*pLayerFound = pLayer;
				return iPosition;
			}
		}
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
CAGSym* CAGDoc::FindSymbolAnywhereByID(int nID, DWORD dwFlags) const
{
	int nPages = GetNumPages();
	for (int i = 0; i < nPages; i++)
	{
		CAGPage* pPage = GetPage(i);
		if (!pPage)
			return NULL;

		int nLayers = pPage->GetNumLayers();
		for (int j = 0; j < nLayers; j++)
		{
			CAGLayer* pLayer = pPage->GetLayer(j);
			if (!pLayer)
				return NULL;

			CAGSym* pSym = pLayer->FindFirstSymbolByID(nID, dwFlags);
			if (pSym)
				return pSym;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
bool CAGDoc::HasLockedImages() const
{
	int nPages = GetNumPages();
	for (int i = 0; i < nPages; i++)
	{
		CAGPage* pPage = GetPage(i);
		if (!pPage)
			return false;

		int nLayers = pPage->GetNumLayers();
		for (int j = 0; j < nLayers; j++)
		{
			CAGLayer* pLayer = pPage->GetLayer(j);
			if (!pLayer)
				return false;

			int nSymbols = pLayer->GetNumSymbols();
			for (int i = 0; i < nSymbols; i++)
			{
				CAGSym* pSym = pLayer->GetSymbol(i);
				if (!pSym || !pSym->IsImage())
					continue;

				CAGSymImage* pSymImage = (CAGSymImage*)pSym;
				if (pSymImage->IsLocked())
					return true;
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CAGDoc::HasTransparentImages() const
{
	int nPages = GetNumPages();
	for (int i = 0; i < nPages; i++)
	{
		CAGPage* pPage = GetPage(i);
		if (!pPage)
			return false;

		int nLayers = pPage->GetNumLayers();
		for (int j = 0; j < nLayers; j++)
		{
			CAGLayer* pLayer = pPage->GetLayer(j);
			if (!pLayer)
				return false;

			int nSymbols = pLayer->GetNumSymbols();
			for (int i = 0; i < nSymbols; i++)
			{
				CAGSym* pSym = pLayer->GetSymbol(i);
				if (!pSym || !pSym->IsImage())
					continue;

				CAGSymImage* pSymImage = (CAGSymImage*)pSym;
				bool bTransparent = (pSymImage->GetTransparentColor() != CLR_NONE);
				if (bTransparent)
					return true;
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CAGDoc::Read(istream& input, bool& bAdjusted, bool bAllowAdjustment)
{
	Free();

	CAGDocIO DocIO(&input);
	AGHDR FileHeader;
	DocIO.Read(&FileHeader, sizeof(FileHeader));

	bool bSignatureMatch = (FileHeader.Id0 == AGDOC_ID0 && FileHeader.Id1 == AGDOC_ID1 && FileHeader.Ver <= AGDOC_VERSION);
	if (!bSignatureMatch)
		return false; // should never happen

	DocIO.Read(&m_AGDocType, sizeof(m_AGDocType));
	if (m_AGDocType < 0 || m_AGDocType > DOC_LAST)
		return false; // should never happen

	DocIO.Read(&m_nPages, sizeof(m_nPages));
	if (m_nPages <= 0 || m_nPages > 500)
		return false; // should never happen

	for (int i = 0; i < m_nPages; i++)
	{
		m_pPages[i] = new CAGPage();
		if (!m_pPages[i]->Read(&DocIO))
		{
			for (int j = 0; j <= i; j++)
			{
				delete m_pPages[j];
				m_pPages[j] = NULL;
			}
				
			return false; // should never happen
		}
	}

	DocIO.Close();
	m_bModified = false;

	SIZE PageSize = {0,0};
	GetPageSize(PageSize);
	SetOrigPageSize(PageSize);

	if (m_nPages >= 1)
		m_nCurPage = 0;

	// Set the portrait flag
	if (m_nPages <= 0)
		m_bPortrait = false;
	else
		m_bPortrait = m_pPages[0]->IsPortrait();

	// Version 1 document fix-ups
	if (FileHeader.Ver <= 1)
	{
		// Fixup for obsolete document types
		if (m_AGDocType == DOC_NOTECARD_OLD)
			m_AGDocType =  DOC_QUARTERCARD;
		if (m_AGDocType == DOC_QUARTERCARD_OLD)
			m_AGDocType =  DOC_QUARTERCARD;
		if (m_AGDocType == DOC_HALFCARD_OLD)
			m_AGDocType =  DOC_HALFCARD;

		// Fixup to move all symbols from layer 0 to layer 1,
		// since layer 0 is now the 'Master' layer
		for (int n = 0; n < GetNumPages(); n++)
		{
			CAGPage* pPage = GetPage(n);
			if (!pPage)
				continue;

			CAGLayer* pMasterLayer = pPage->GetMasterLayer();
			if (!pMasterLayer)
				continue;

			CAGLayer* pUserLayer = pPage->GetUserLayer();
			if (!pUserLayer)
				continue;

			int nSymbols = pMasterLayer->GetNumSymbols();
			for (int i = 0; i < nSymbols; i++)
			{
				// Find the first symbol each time since they are being deleted as we go
				CAGSym* pSym = pMasterLayer->GetSymbol(0);
				if (pSym)
				{
					pUserLayer->AddSymbol(pSym, false/*bAddToFront*/);
					pMasterLayer->DeleteSymbol(pSym, true/*bFromListOnly*/);
				}
			}
		}
	}

	// Version 3 document document fix-ups
	if (FileHeader.Ver <= 3)
	{
		// Fixup for obsolete document types
		if (m_AGDocType == DOC_NOTECARD_OLD)
			m_AGDocType =  DOC_QUARTERCARD;

		if (m_AGDocType == DOC_STICKER_OLD)
			m_AGDocType =  DOC_LABEL;

		if (m_AGDocType == DOC_BLANKPAGE_OLD	||
			m_AGDocType == DOC_CALENDAR_OLD		||
			m_AGDocType == DOC_CERTIFICATE_OLD	||
			m_AGDocType == DOC_CRAFT_OLD		||
			m_AGDocType == DOC_STATIONERY_OLD	||
			m_AGDocType == DOC_NEWSLETTER_OLD	||
			m_AGDocType == DOC_POSTER_OLD		||
			m_AGDocType == DOC_WEBPUB_OLD)
			m_AGDocType =  DOC_FULLSHEET;

		if (!bAllowAdjustment)
			return true;

		// Fixup to make sure all documents of a given type match one of its template sizes
		for (int n = 0; n < GetNumPages(); n++)
		{
			CAGPage* pPage = GetPage(n);
			if (!pPage)
				continue;

			SIZE PageSize = {0,0};
			pPage->GetPageSize(PageSize);
			SIZE ClosestSize = PageSize;
			bool bNeedsAdjustment = CPaperTemplates::AdjustSize(m_AGDocType, ClosestSize);
			if (!bNeedsAdjustment)
				continue;

			// Offset all symbols, on all layers, on all pages
			bAdjusted = true;

			int dx = (ClosestSize.cx - PageSize.cx) / 2;
			int dy = (ClosestSize.cy - PageSize.cy) / 2;

			pPage->SetPageSize(ClosestSize);
			int nLayers = pPage->GetNumLayers();
			for (int l = 0; l < nLayers; l++)
			{
				CAGLayer* pLayer = pPage->GetLayer(l);
				if (!pLayer)
					continue;

				int nSymbols = pLayer->GetNumSymbols();
				for (int i = 0; i < nSymbols; i++)
				{
					CAGSym* pSym = pLayer->GetSymbol(i);
					if (!pSym)
						continue;

					CAGMatrix Matrix = pSym->GetMatrix();
					Matrix.Translate(dx, dy);
					pSym->SetMatrix(Matrix);
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAGDoc::Write(ostream& output) const
{
	CAGDocIO DocIO(&output);

	AGHDR FileHeader;
	FileHeader.Id0 = AGDOC_ID0;
	FileHeader.Id1 = AGDOC_ID1;
	FileHeader.Ver = AGDOC_VERSION;
	DocIO.Write(&FileHeader, sizeof(FileHeader));
	DocIO.Write(&m_AGDocType, sizeof(m_AGDocType));
	DocIO.Write(&m_nPages, sizeof(m_nPages));

	for (int i = 0; i < m_nPages; i++)
	{
		CAGPage* pPage = m_pPages[i];
		pPage->Write(&DocIO);
	}

	DocIO.Close();

	return true;
}

//////////////////////////////////////////////////////////////////////
DWORD CAGDoc::GetCompressedSize() const
{
	CAGDocIO DocIO((ostream*)NULL);

	AGHDR FileHeader;
	FileHeader.Id0 = AGDOC_ID0;
	FileHeader.Id1 = AGDOC_ID1;
	FileHeader.Ver = AGDOC_VERSION;
	DocIO.Write(&FileHeader, sizeof(FileHeader));
	DocIO.Write(&m_AGDocType, sizeof(m_AGDocType));
	DocIO.Write(&m_nPages, sizeof(m_nPages));

	for (int i = 0; i < m_nPages; i++)
	{
		CAGPage* pPage = m_pPages[i];
		pPage->Write(&DocIO);
	}

	DocIO.Close();
	DWORD dwCompressedSize = DocIO.GetOutputCount();

	return dwCompressedSize;
}

//////////////////////////////////////////////////////////////////////
CAGDocIO::CAGDocIO(istream* pInput)
{
	m_bEOF = false;
	m_bClosed = false;
	m_pInput = pInput;
	m_pOutput = NULL;
	m_dwOutputCount = 0;
	m_zBuf = new BYTE[Z_BUFSIZE + Z_SLOP];

	memset(&m_zstream, 0, sizeof(m_zstream));
	m_zstream.next_in = m_zBuf;
	m_zErr = inflateInit2(&m_zstream, -MAX_WBITS);
}

//////////////////////////////////////////////////////////////////////
CAGDocIO::CAGDocIO(ostream* pOutput)
{
	m_bEOF = false;
	m_bClosed = false;
	m_pInput = NULL;
	m_pOutput = pOutput;
	m_dwOutputCount = 0;
	m_zBuf = new BYTE[Z_BUFSIZE + Z_SLOP];

	memset(&m_zstream, 0, sizeof(m_zstream));
	m_zErr = deflateInit2(&m_zstream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);

	m_zstream.next_out = m_zBuf;
	m_zstream.avail_out = Z_BUFSIZE;
}

//////////////////////////////////////////////////////////////////////
CAGDocIO::~CAGDocIO()
{
	Close();
	delete [] m_zBuf;
}

//////////////////////////////////////////////////////////////////////
void CAGDocIO::Close()
{
	if (m_bClosed)
		return;

	m_bClosed = true;

	if (m_pInput)
	{
		inflateEnd(&m_zstream);
	}
	else
	{
		bool bDone = false;

		m_zstream.avail_in = 0;
		while (1)
		{
			int len = Z_BUFSIZE - m_zstream.avail_out;
			if (len)
			{
				if (m_pOutput)
					m_pOutput->write((char*)m_zBuf, len);
				m_dwOutputCount += len;
				m_zstream.next_out = m_zBuf;
				m_zstream.avail_out = Z_BUFSIZE;
			}

			if (bDone)
				break;

			m_zErr = deflate(&m_zstream, Z_FINISH);

			bDone = (m_zstream.avail_out || m_zErr == Z_STREAM_END);
 
			if (m_zErr != Z_OK && m_zErr != Z_STREAM_END)
				break;
		}

		deflateEnd(&m_zstream);
	}
}

//////////////////////////////////////////////////////////////////////
void CAGDocIO::Read(void* pData, DWORD dwSize)
{
	if (!m_pInput)
		return;
	if (m_zErr == Z_DATA_ERROR || m_zErr == Z_ERRNO)
		return;
	if (m_zErr == Z_STREAM_END)
		return;

	m_zstream.next_out = (Bytef*) pData;
	m_zstream.avail_out = dwSize;

	while (m_zstream.avail_out)
	{
		if (!m_zstream.avail_in && !m_bEOF)
		{
			m_pInput->read((char*)m_zBuf, Z_BUFSIZE);

			m_zstream.avail_in = m_pInput->gcount();
			if (!m_zstream.avail_in)
				m_bEOF = true;

			m_zstream.next_in = m_zBuf;
		}

		m_zErr = inflate(&m_zstream, Z_NO_FLUSH);

		if (m_zErr != Z_OK || m_bEOF)
			break;
	}
}

//////////////////////////////////////////////////////////////////////
void CAGDocIO::Write(const void* pData, DWORD dwSize)
{
	m_zstream.next_in = (Bytef*)pData;
	m_zstream.avail_in = dwSize;

	while (m_zstream.avail_in)
	{
		if (!m_zstream.avail_out)
		{
			int len = Z_BUFSIZE;
			if (m_pOutput)
				m_pOutput->write((char*)m_zBuf, len);
			m_dwOutputCount += len;
			m_zstream.next_out = m_zBuf;
			m_zstream.avail_out = Z_BUFSIZE;
		}

		deflate(&m_zstream, Z_NO_FLUSH);
	}
}
