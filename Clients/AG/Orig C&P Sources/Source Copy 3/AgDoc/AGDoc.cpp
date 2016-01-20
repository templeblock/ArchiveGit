//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "AGDoc.h"
#include "AGPage.h"
#include "AGDC.h"

#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] =__FILE__;
#define new DEBUG_NEW
#endif
#endif

#define AGDOC_ID		"Ag"
#define AGDOC_VERSION	1
#define Z_BUFSIZE		16384
#define Z_SLOP			100

typedef struct
{
	BYTE abId[2];
	BYTE bVer;
} AGHDR;


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDoc::CAGDoc(AGDOCTYPE AGDocType)
{
	m_AGDocType = AGDocType;
	m_nCurPage = 0;
	m_nPages = 0;
	for (int i = 0; i < MAX_AGPAGE; i++)
		m_pPages[i] = NULL;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDoc::~CAGDoc()
{
	Free();
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
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


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDoc::GetFonts(LOGFONTARRAY& lfArray)
{
	for (int i = 0; i < m_nPages; i++)
		m_pPages[i]->GetFonts(lfArray);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGPage* CAGDoc::GetPage(int nPage)
{
	if (nPage <= 0 || nPage > m_nPages)
		return (NULL);
	else
		return (m_pPages[nPage - 1]);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDoc::GetQFPageRect(int nPage, bool bPortrait, bool bMaxMargin, CAGDC* pDC, CAGDCInfo& di, RECT& DestRect, bool& bFlip, bool& bRotated)
{
	//----------------------------------------------------------------------//
	// Set a small minimum margin because HP DeskJets report a .04 top		//
	// margin but obviously can't print that close to the edge of the page. //
	//----------------------------------------------------------------------//
	int nMinMarginX = di.m_nLogPixelsX / 10;
	int nMinMarginY = di.m_nLogPixelsY / 10;

	RECT Margin =
	{
		max(di.m_PrintOffset.cx, nMinMarginX),
		max(di.m_PrintOffset.cy, nMinMarginY),
		max((di.m_PhysPageSize.cx - di.m_nHorzRes - di.m_PrintOffset.cx), nMinMarginX),
		max((di.m_PhysPageSize.cy - di.m_nVertRes - di.m_PrintOffset.cy), nMinMarginY)
	};

	SIZE MarginMax =
	{
		max(Margin.left, Margin.right),
		max(Margin.top, Margin.bottom)
	};

	bRotated = false;
	if ((bPortrait && (Margin.top < Margin.bottom)) || (!bPortrait && (Margin.left < Margin.right)))
	{
		nPage = ((nPage + 1) % 4) + 1;
		bRotated = true;
	}

	bFlip = false;

	switch (nPage)
	{
		case 1:
		{
			if (bMaxMargin)
			{
				DestRect.left = di.m_PhysPageSize.cx / 2 + MarginMax.cx;
				DestRect.right = di.m_PhysPageSize.cx - MarginMax.cx;
			}
			else
			{
				DestRect.left = di.m_PhysPageSize.cx / 2 + Margin.right;
				DestRect.right = di.m_PhysPageSize.cx - Margin.right;
			}

			DestRect.top = di.m_PhysPageSize.cy / 2 + Margin.bottom;
			DestRect.bottom = di.m_PhysPageSize.cy - Margin.bottom;
			break;
		}

		case 2:
		{
			if (bPortrait)
			{
				if (bMaxMargin)
				{
					DestRect.left = di.m_PhysPageSize.cx / 2 + MarginMax.cx;
					DestRect.right = di.m_PhysPageSize.cx - MarginMax.cx;
				}
				else
				{
					DestRect.left = di.m_PhysPageSize.cx / 2 + Margin.right;
					DestRect.right = di.m_PhysPageSize.cx - Margin.right;
				}
				DestRect.top = Margin.top;
				DestRect.bottom = di.m_PhysPageSize.cy / 2 - Margin.top;
				bFlip = true;
			}
			else
			{
				DestRect.left = Margin.left;
				DestRect.right = di.m_PhysPageSize.cx / 2 - Margin.left;
				if (bMaxMargin)
				{
					DestRect.top = di.m_PhysPageSize.cy / 2 + MarginMax.cy;
					DestRect.bottom = di.m_PhysPageSize.cy - MarginMax.cy;
				}
				else
				{
					DestRect.top = di.m_PhysPageSize.cy / 2 + Margin.bottom;
					DestRect.bottom = di.m_PhysPageSize.cy - Margin.bottom;
				}
				if (!bRotated)
					bFlip = true;
			}
			break;
		}

		case 3:
		{
			if (bPortrait)
			{
				if (bMaxMargin)
				{
					DestRect.left = MarginMax.cx;
					DestRect.right = di.m_PhysPageSize.cx / 2 - MarginMax.cx;
				}
				else
				{
					DestRect.left = Margin.left;
					DestRect.right = di.m_PhysPageSize.cx / 2 - Margin.left;
				}
				DestRect.top = Margin.top;
				DestRect.bottom = di.m_PhysPageSize.cy / 2 - Margin.top;
			}
			else
			{
				DestRect.left = Margin.left;
				DestRect.right = di.m_PhysPageSize.cx / 2 - Margin.left;
				if (bMaxMargin)
				{
					DestRect.top = MarginMax.cy;
					DestRect.bottom = di.m_PhysPageSize.cy / 2 - MarginMax.cy;
				}
				else
				{
					DestRect.top = Margin.top;
					DestRect.bottom = di.m_PhysPageSize.cy / 2 - Margin.top;
				}
			}

			bFlip = true;
			break;
		}

		case 4:
		{
			if (bPortrait)
			{
				if (bMaxMargin)
				{
					DestRect.left = MarginMax.cx;
					DestRect.right = di.m_PhysPageSize.cx / 2 - MarginMax.cx;
				}
				else
				{
					DestRect.left = Margin.left;
					DestRect.right = di.m_PhysPageSize.cx / 2 - Margin.left;
				}
				DestRect.top = di.m_PhysPageSize.cy / 2 + Margin.bottom;
				DestRect.bottom = di.m_PhysPageSize.cy - Margin.bottom;
			}
			else
			{
				if (bMaxMargin)
				{
					DestRect.left = di.m_PhysPageSize.cx / 2 + MarginMax.cx;
					DestRect.right = di.m_PhysPageSize.cx - MarginMax.cx;
				}
				else
				{
					DestRect.left = di.m_PhysPageSize.cx / 2 + Margin.right;
					DestRect.right = di.m_PhysPageSize.cx - Margin.right;
				}
				DestRect.top = Margin.top;
				DestRect.bottom = di.m_PhysPageSize.cy / 2 - Margin.top;
				if (!bRotated)
					bFlip = true;
			}
			break;
		}
	}

	pDC->DPAtoVPA(&DestRect);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDoc::GetSFPageRect(int nPage, bool bPortrait, bool bMaxMargin, CAGDC* pDC, CAGDCInfo& di, RECT& DestRect, bool& bFlip, bool& bRotated)
{
	//----------------------------------------------------------------------//
	// Set a small minimum margin because HP DeskJets report a .04 top		//
	// margin but obviously can't print that close to the edge of the page. //
	//----------------------------------------------------------------------//
	int nMinMarginX = di.m_nLogPixelsX / 10;
	int nMinMarginY = di.m_nLogPixelsY / 10;

	RECT Margin =
	{
		max(di.m_PrintOffset.cx, nMinMarginX),
		max(di.m_PrintOffset.cy, nMinMarginY),
		max((di.m_PhysPageSize.cx - di.m_nHorzRes - di.m_PrintOffset.cx), nMinMarginX),
		max((di.m_PhysPageSize.cy - di.m_nVertRes - di.m_PrintOffset.cy), nMinMarginY)
	};

	SIZE MarginMax =
	{
		max(Margin.left, Margin.right),
		max(Margin.top, Margin.bottom)
	};

	bRotated = false;
	if ((bPortrait && (Margin.top < Margin.bottom)) || (!bPortrait && (Margin.left < Margin.right)))
	{
		nPage = 4 - nPage + 1;
		bRotated = true;
	}

	bFlip = false;

	switch (nPage)
	{
		case 1:
		case 3:
		{
			if (bPortrait)
			{
				DestRect.left = MarginMax.cx;
				DestRect.right = di.m_PhysPageSize.cx - MarginMax.cx;
				if (bMaxMargin)
				{
					DestRect.top = di.m_PhysPageSize.cy / 2 + MarginMax.cy;
					DestRect.bottom = di.m_PhysPageSize.cy - MarginMax.cy;
				}
				else
				{
					DestRect.top = di.m_PhysPageSize.cy / 2 + Margin.bottom;
					DestRect.bottom = di.m_PhysPageSize.cy - Margin.bottom;
				}
				if (bRotated && nPage == 3)
					bFlip = true;
			}
			else
			{
				if (bMaxMargin)
				{
					DestRect.left = di.m_PhysPageSize.cx / 2 + MarginMax.cx;
					DestRect.right = di.m_PhysPageSize.cx - MarginMax.cx;
				}
				else
				{
					DestRect.left = di.m_PhysPageSize.cx / 2 + Margin.right;
					DestRect.right = di.m_PhysPageSize.cx - Margin.right;
				}
				DestRect.top = MarginMax.cy;
				DestRect.bottom = di.m_PhysPageSize.cy - MarginMax.cy;
				bFlip = bRotated;
			}
			break;
		}

		case 2:
		case 4:
		{
			if (bPortrait)
			{
				DestRect.left = MarginMax.cx;
				DestRect.right = di.m_PhysPageSize.cx - MarginMax.cx;
				if (bMaxMargin)
				{
					DestRect.top = MarginMax.cy;
					DestRect.bottom = di.m_PhysPageSize.cy / 2 - MarginMax.cy;
				}
				else
				{
					DestRect.top = Margin.top;
					DestRect.bottom = di.m_PhysPageSize.cy / 2 - Margin.top;
				}
				if (nPage == 4 || bRotated)
					bFlip = true;
			}
			else
			{
				if (bMaxMargin)
				{
					DestRect.left = MarginMax.cx;
					DestRect.right = di.m_PhysPageSize.cx / 2 - MarginMax.cx;
				}
				else
				{
					DestRect.left = Margin.left;
					DestRect.right = di.m_PhysPageSize.cx / 2 - Margin.left;
				}
				DestRect.top = MarginMax.cy;
				DestRect.bottom = di.m_PhysPageSize.cy - MarginMax.cy;

				bFlip = bRotated;
			}
			break;
		}
	}

	pDC->DPAtoVPA(&DestRect);
}


//--------------------------------------------------------------------------//
// Print quarter-fold card. 												//
//--------------------------------------------------------------------------//
bool CAGDoc::PrintCardQuarter(char* pszDriver, char* pszDevice, char* pszOutput, DEVMODE* pDevMode, const char* pszFileName)
{
	if (m_nPages != 4)
		return (false);

	bool bPortrait = (m_AGDocType == DOC_CARD_QUARTERFOLD_PORTRAIT || m_AGDocType == DOC_CARD_SINGLEFOLD_PORTRAIT);

	if (pDevMode)
	{
		if (bPortrait)
			pDevMode->dmOrientation = DMORIENT_PORTRAIT;
		else
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
		pDevMode->dmFields |= DM_ORIENTATION;
	}

	CAGDC* pDC = new CAGDC(pszDriver, pszDevice, pszOutput, pDevMode);
	CAGDCInfo di = pDC->GetDeviceInfo();

	if (pDevMode && pDevMode->dmOrientation == DMORIENT_PORTRAIT && di.m_nHorzSize > di.m_nVertSize)
	{
		delete pDC;
		pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
		pDC = new CAGDC(pszDriver, pszDevice, pszOutput, pDevMode);
		di = pDC->GetDeviceInfo();
	}
	else
	if (pDevMode && pDevMode->dmOrientation == DMORIENT_LANDSCAPE && di.m_nVertSize > di.m_nHorzSize)
	{
		delete pDC;
		pDevMode->dmOrientation = DMORIENT_PORTRAIT;
		pDC = new CAGDC(pszDriver, pszDevice, pszOutput, pDevMode);
		di = pDC->GetDeviceInfo();
	}

	RECT DestRect;
	RECT SrcRect;
	SIZE sizePage;
	CAGMatrix ViewMatrix;
	bool bFlip;
	bool bRotated;

	if (!pDC->StartDoc("Create and Print"))
	{
		delete pDC;
		return (false);
	}

	if (!pDC->StartPage())
	{
		pDC->AbortDoc();
		delete pDC;
		return (false);
	}

	//----------------------------------------------------------------------//
	// Front																//
	//----------------------------------------------------------------------//
	CAGPage* pPage = GetPage(1);
	pPage->GetPageSize(&sizePage);
	::SetRect(&SrcRect, 0, 0, sizePage.cx, sizePage.cy);
	GetQFPageRect(1, bPortrait, false, pDC, di, DestRect, bFlip, bRotated);
	ViewMatrix.ScaleAndCenter(DestRect, SrcRect, bFlip);
	pDC->SetViewingMatrix(ViewMatrix);
	pPage->Draw(*pDC);

	bool bPage2Empty = GetPage(2)->IsEmpty();
	bool bPage3Empty = GetPage(3)->IsEmpty();

	//----------------------------------------------------------------------//
	// Inside Left / Top													//
	//----------------------------------------------------------------------//
	pPage = GetPage(2);
	pPage->GetPageSize(&sizePage);
	::SetRect(&SrcRect, 0, 0, sizePage.cx, sizePage.cy);
	GetQFPageRect(2, bPortrait, !bPage3Empty, pDC, di, DestRect, bFlip,
	  bRotated);
	ViewMatrix.ScaleAndCenter(DestRect, SrcRect, bFlip);
	pDC->SetViewingMatrix(ViewMatrix);
	pPage->Draw(*pDC);

	//----------------------------------------------------------------------//
	// Inside Right / Bottom												//
	//----------------------------------------------------------------------//
	pPage = GetPage(3);
	pPage->GetPageSize(&sizePage);
	::SetRect(&SrcRect, 0, 0, sizePage.cx, sizePage.cy);
	GetQFPageRect(3, bPortrait, !bPage2Empty, pDC, di, DestRect, bFlip,
					bRotated);
	ViewMatrix.ScaleAndCenter(DestRect, SrcRect, bFlip);
	pDC->SetViewingMatrix(ViewMatrix);
	pPage->Draw(*pDC);

	//----------------------------------------------------------------------//
	// Back 																//
	//----------------------------------------------------------------------//
	pPage = GetPage(4);
	pPage->GetPageSize(&sizePage);
	::SetRect(&SrcRect, 0, 0, sizePage.cx, sizePage.cy);
	GetQFPageRect(4, bPortrait, false, pDC, di, DestRect, bFlip, bRotated);
	ViewMatrix.ScaleAndCenter(DestRect, SrcRect, bFlip);
	pDC->SetViewingMatrix(ViewMatrix);
	pPage->Draw(*pDC);

	//----------------------------------------------------------------------//
	// BatchPrint filename. 												//
	//----------------------------------------------------------------------//
	if (pszFileName)
	{
		LOGFONT Font;
		memset(&Font, 0, sizeof(Font));
		Font.lfHeight = -(12 * APP_RESOLUTION / 72);
		lstrcpy(Font.lfFaceName, "Times New Roman");
		pDC->SetFont(&Font);
//j		pDC->SetFont(Font);
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->ExtTextOut(APP_RESOLUTION / 8, SrcRect.bottom -(APP_RESOLUTION / 8), 0, NULL, pszFileName, lstrlen(pszFileName), NULL);
	}

	if (!pDC->EndPage())
	{
		pDC->AbortDoc();
		delete pDC;
		return (false);
	}

	if (!pDC->EndDoc())
	{
		pDC->AbortDoc();
		delete pDC;
		return (false);
	}

	delete pDC;
	return true;
}


//--------------------------------------------------------------------------//
// Print single-fold card.													//
//--------------------------------------------------------------------------//
bool CAGDoc::PrintCardSingle(PRINTSIDE PrintSide, char* pszDriver, char* pszDevice, char* pszOutput, DEVMODE* pDevMode, bool& bRotated, const char* pszFileName)
{
	if (m_nPages != 4)
		return (false);

	bool bPortrait = (m_AGDocType == DOC_CARD_SINGLEFOLD_LANDSCAPE || m_AGDocType == DOC_CARD_QUARTERFOLD_LANDSCAPE);

	if (pDevMode)
	{
		if (bPortrait)
			pDevMode->dmOrientation = DMORIENT_PORTRAIT;
		else
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
		pDevMode->dmFields |= DM_ORIENTATION;
	}

	CAGDC* pDC = new CAGDC(pszDriver, pszDevice, pszOutput, pDevMode);
	CAGDCInfo di = pDC->GetDeviceInfo();

	if (pDevMode && pDevMode->dmOrientation == DMORIENT_PORTRAIT && di.m_nHorzSize > di.m_nVertSize)
	{
		delete pDC;
		pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
		pDC = new CAGDC(pszDriver, pszDevice, pszOutput, pDevMode);
		di = pDC->GetDeviceInfo();
	}
	else
	if (pDevMode && pDevMode->dmOrientation == DMORIENT_LANDSCAPE && di.m_nVertSize > di.m_nHorzSize)
	{
		delete pDC;
		pDevMode->dmOrientation = DMORIENT_PORTRAIT;
		pDC = new CAGDC(pszDriver, pszDevice, pszOutput, pDevMode);
		di = pDC->GetDeviceInfo();
	}

	RECT DestRect;
	RECT SrcRect;
	SIZE sizePage;
	CAGMatrix ViewMatrix;
	bool bFlip;

	if (!pDC->StartDoc("Create and Print"))
	{
		delete pDC;
		return (false);
	}

	if (PrintSide == PRINT_OUTSIDE || PrintSide == PRINT_BOTH)
	{
		if (!pDC->StartPage())
		{
			pDC->AbortDoc();
			delete pDC;
			return (false);
		}

		//------------------------------------------------------------------//
		// Front															//
		//------------------------------------------------------------------//
		CAGPage* pPage = GetPage(1);
		pPage->GetPageSize(&sizePage);
		::SetRect(&SrcRect, 0, 0, sizePage.cx, sizePage.cy);
		GetSFPageRect(1, bPortrait, false, pDC, di, DestRect, bFlip, bRotated);
		ViewMatrix.ScaleAndCenter(DestRect, SrcRect, bFlip);
		pDC->SetViewingMatrix(ViewMatrix);
		pPage->Draw(*pDC);

		//------------------------------------------------------------------//
		// Back 															//
		//------------------------------------------------------------------//
		pPage = GetPage(4);
		pPage->GetPageSize(&sizePage);
		::SetRect(&SrcRect, 0, 0, sizePage.cx, sizePage.cy);
		GetSFPageRect(4, bPortrait, false, pDC, di, DestRect, bFlip, bRotated);
		ViewMatrix.ScaleAndCenter(DestRect, SrcRect, bFlip);
		pDC->SetViewingMatrix(ViewMatrix);
		pPage->Draw(*pDC);

		//------------------------------------------------------------------//
		// BatchPrint filename. 											//
		//------------------------------------------------------------------//
		if (pszFileName)
		{
			LOGFONT Font;
			memset(&Font, 0, sizeof(Font));
			Font.lfHeight = -(12 * APP_RESOLUTION / 72);
			lstrcpy(Font.lfFaceName, "Times New Roman");
			pDC->SetFont(&Font);
//j			pDC->SetFont(Font);
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->ExtTextOut(APP_RESOLUTION / 8, SrcRect.bottom -(APP_RESOLUTION / 8), 0, NULL, pszFileName, lstrlen(pszFileName), NULL);
		}

		if (!pDC->EndPage())
		{
			pDC->AbortDoc();
			delete pDC;
			return (false);
		}
	}

	if (PrintSide == PRINT_INSIDE || PrintSide == PRINT_BOTH)
	{
		if (!pDC->StartPage())
		{
			pDC->AbortDoc();
			delete pDC;
			return (false);
		}

		bool bPage2Empty = GetPage(2)->IsEmpty();
		bool bPage3Empty = GetPage(3)->IsEmpty();

		//------------------------------------------------------------------//
		// Inside Left / Top												//
		//------------------------------------------------------------------//
		CAGPage* pPage = GetPage(2);
		pPage->GetPageSize(&sizePage);
		::SetRect(&SrcRect, 0, 0, sizePage.cx, sizePage.cy);
		GetSFPageRect(2, bPortrait, !bPage3Empty, pDC, di, DestRect, bFlip, bRotated);
		ViewMatrix.ScaleAndCenter(DestRect, SrcRect, bFlip);
		pDC->SetViewingMatrix(ViewMatrix);
		pPage->Draw(*pDC);

		//------------------------------------------------------------------//
		// BatchPrint filename. 											//
		//------------------------------------------------------------------//
		if (pszFileName)
		{
			LOGFONT Font;
			memset(&Font, 0, sizeof(Font));
			Font.lfHeight = -(12 * APP_RESOLUTION / 72);
			lstrcpy(Font.lfFaceName, "Times New Roman");
			pDC->SetFont(&Font);
//j			pDC->SetFont(Font);
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->ExtTextOut(APP_RESOLUTION / 8, SrcRect.bottom -(APP_RESOLUTION / 8), 0, NULL, pszFileName, lstrlen(pszFileName), NULL);
		}

		//------------------------------------------------------------------//
		// Inside Right / Bottom											//
		//------------------------------------------------------------------//
		pPage = GetPage(3);
		pPage->GetPageSize(&sizePage);
		::SetRect(&SrcRect, 0, 0, sizePage.cx, sizePage.cy);
		GetSFPageRect(3, bPortrait, !bPage2Empty, pDC, di, DestRect, bFlip, bRotated);
		ViewMatrix.ScaleAndCenter(DestRect, SrcRect, bFlip);
		pDC->SetViewingMatrix(ViewMatrix);
		pPage->Draw(*pDC);

		if (!pDC->EndPage())
		{
			pDC->AbortDoc();
			delete pDC;
			return (false);
		}
	}

	if (!pDC->EndDoc())
	{
		pDC->AbortDoc();
		delete pDC;
		return (false);
	}
	delete pDC;

	return true;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGDoc::Read(istream& input)
{
	bool bReturn = true;

	Free();

	CAGDocIO DocIO(&input);

	AGHDR Hdr;
	DocIO.Read(&Hdr, sizeof(Hdr));
	if (!memcmp(Hdr.abId, AGDOC_ID, sizeof(Hdr.abId)) && Hdr.bVer <= AGDOC_VERSION)
	{
		DocIO.Read(&m_AGDocType, sizeof(m_AGDocType));
		DocIO.Read(&m_nPages, sizeof(m_nPages));
		for (int i = 0; i < m_nPages; i++)
		{
			CAGPage* pPage = new CAGPage();
			pPage->Read(&DocIO);
			m_pPages[i] = pPage;
		}
		if (m_nPages > 1)
			m_nCurPage = 1;
	}
	else
		bReturn = false;

	DocIO.Close();

	return bReturn;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGDoc::Write(ostream& output)
{
	bool bReturn = true;

	CAGDocIO DocIO(&output);

	AGHDR Hdr;
	memcpy(Hdr.abId, AGDOC_ID, sizeof(Hdr.abId));
	Hdr.bVer = AGDOC_VERSION;
	DocIO.Write(&Hdr, sizeof(Hdr));
	DocIO.Write(&m_AGDocType, sizeof(m_AGDocType));
	DocIO.Write(&m_nPages, sizeof(m_nPages));

	for (int i = 0; i < m_nPages; i++)
	{
		CAGPage* pPage = m_pPages[i];
		pPage->Write(&DocIO);
	}

	DocIO.Close();

	return bReturn;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDocIO::CAGDocIO(istream* pInput)
{
	m_bInput = true;
	m_bEOF = false;
	m_pInput = pInput;
	m_pOutput = NULL;
	m_zBuf = new BYTE [Z_BUFSIZE + Z_SLOP];

	memset(&m_zstream, 0, sizeof(m_zstream));
	m_zstream.next_in = m_zBuf;
	m_zErr = inflateInit2(&m_zstream, -MAX_WBITS);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDocIO::CAGDocIO(ostream* pOutput)
{
	m_bInput = false;
	m_bEOF = false;
	m_pInput = NULL;
	m_pOutput = pOutput;
	m_zBuf = new BYTE [Z_BUFSIZE + Z_SLOP];

	memset(&m_zstream, 0, sizeof(m_zstream));
	m_zErr = deflateInit2(&m_zstream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);

	m_zstream.next_out = m_zBuf;
	m_zstream.avail_out = Z_BUFSIZE;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDocIO::~CAGDocIO()
{
	delete [] m_zBuf;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDocIO::Close()
{
	if (m_bInput)
	{
		inflateEnd(&m_zstream);
	}
	else
	{
		bool bDone = false;
		int len = 0;

		m_zstream.avail_in = 0;
		while (1)
		{
			len = Z_BUFSIZE - m_zstream.avail_out;

			if (len)
			{
				m_pOutput->write((char*)m_zBuf, len);
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


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDocIO::Read(void* pData, DWORD dwSize)
{
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


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDocIO::Write(const void* pData, DWORD dwSize)
{
	m_zstream.next_in = (Bytef*)pData;
	m_zstream.avail_in = dwSize;

	while (m_zstream.avail_in)
	{
		if (!m_zstream.avail_out)
		{
			m_zstream.next_out = m_zBuf;
			m_pOutput->write((char*)m_zBuf, Z_BUFSIZE);
			m_zstream.avail_out = Z_BUFSIZE;
		}

		deflate(&m_zstream, Z_NO_FLUSH);
	}
}
