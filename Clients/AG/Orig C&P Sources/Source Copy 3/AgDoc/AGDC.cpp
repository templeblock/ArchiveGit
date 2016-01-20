//==========================================================================//
//==========================================================================//
#include "stdafx.h"
#include "AGDC.h"
#include "AGDib.h"

#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] =__FILE__;
#define new DEBUG_NEW
#endif
#endif

#define SWAP(a,b)((a) ^= (b), (b) ^= (a), (a) ^= (b))

BYTE* CAGDC::m_pBltBuf = NULL;
HPALETTE CAGDC::m_hPalette = NULL;

static const BYTE aHalftone16x16[16][16] =
{
	 0, 44,  9, 41,  3, 46, 12, 43,  1, 44, 10, 41,  3, 46, 12, 43,
	34, 16, 25, 19, 37, 18, 28, 21, 35, 16, 26, 19, 37, 18, 28, 21,
	38,  6, 47,  3, 40,  9, 50,  6, 38,  7, 47,  4, 40,  9, 49,  6,
	22, 28, 13, 31, 25, 31, 15, 34, 22, 29, 13, 32, 24, 31, 15, 34,
	 2, 46, 12, 43,  1, 45, 10, 42,  2, 45, 11, 42,  1, 45, 11, 42,
	37, 18, 27, 21, 35, 17, 26, 20, 36, 17, 27, 20, 36, 17, 26, 20,
	40,  8, 49,  5, 38,  7, 48,  4, 39,  8, 48,  5, 39,  7, 48,  4,
	24, 30, 15, 33, 23, 29, 13, 32, 23, 30, 14, 33, 23, 29, 14, 32,
	 2, 46, 12, 43,  0, 44, 10, 41,  3, 47, 12, 44,  0, 44, 10, 41,
	37, 18, 27, 21, 35, 16, 25, 19, 37, 19, 28, 22, 35, 16, 25, 19,
	40,  9, 49,  5, 38,  7, 47,  4, 40,  9, 50,  6, 38,  6, 47,  3,
	24, 30, 15, 34, 22, 29, 13, 32, 25, 31, 15, 34, 22, 28, 13, 31,
	 1, 45, 11, 42,  2, 46, 11, 42,  1, 45, 10, 41,  2, 46, 11, 43,
	36, 17, 26, 20, 36, 17, 27, 21, 35, 16, 26, 20, 36, 18, 27, 21,
	39,  8, 48,  4, 39,  8, 49,  5, 38,  7, 48,  4, 39,  8, 49,  5,
	23, 29, 14, 33, 24, 30, 14, 33, 23, 29, 13, 32, 24, 30, 14, 33
};

static const BYTE aModulo51[256] =
{
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
	 0
};

static const BYTE aDividedBy51[256] =
{
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	 5
};

static const BYTE aTimes6[6] =
{
  0, 6, 12, 18, 24, 30
};

static const BYTE aTimes36[6] =
{
  0, 36, 72, 108, 144, 180
};


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDCInfo::CAGDCInfo()
{
	m_bRasDisplay = false;
	m_bPalette = false;
	m_Technology = 0;
	m_nHorzSize = 0;
	m_nVertSize = 0;
	m_nHorzRes = 0;
	m_nVertRes = 0;
	m_nLogPixelsX = 0;
	m_nLogPixelsY = 0;
	m_PhysPageSize.cx = m_PhysPageSize.cy = 0;
	m_PrintOffset.cx = m_PrintOffset.cy = 0;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDCInfo::Init(HDC hDC)
{
	m_Technology = ::GetDeviceCaps(hDC, TECHNOLOGY);
	m_bRasDisplay = (m_Technology == DT_RASDISPLAY);
	m_bPalette = ((::GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE) != 0);

	m_nHorzSize = ::GetDeviceCaps(hDC, HORZSIZE);
	m_nVertSize = ::GetDeviceCaps(hDC, VERTSIZE);
	m_nHorzRes = ::GetDeviceCaps(hDC, HORZRES);
	m_nVertRes = ::GetDeviceCaps(hDC, VERTRES);
	m_nLogPixelsX = ::GetDeviceCaps(hDC, LOGPIXELSX);
	m_nLogPixelsY = ::GetDeviceCaps(hDC, LOGPIXELSY);

	if (::Escape(hDC, GETPHYSPAGESIZE, NULL, NULL, &m_PhysPageSize) <= 0)
	{
		m_PhysPageSize.cx = m_nHorzRes;
		m_PhysPageSize.cy = m_nVertRes;
	}
	if (::Escape(hDC, GETPRINTINGOFFSET, NULL, NULL, &m_PrintOffset) <= 0)
		m_PrintOffset.cx = m_PrintOffset.cy = 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDC::CAGDC(const char* pszDriver, const char* pszDevice, const char* pszOutput, const DEVMODE* pDevMode)
{
	m_hDC = ::CreateDC(pszDriver, pszDevice, pszOutput, pDevMode);
	Init();
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDC::CAGDC(HDC hDC)
{
	m_hDC = hDC;
	Init();
	m_bGivenDC = true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDC::~CAGDC()
{
	if (m_hDC)
	{
		CleanUp();
		if (!m_bGivenDC)
			::DeleteDC(m_hDC);
		m_hDC = NULL;
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::CleanUp()
{
	if (m_hOldPalette)
		::SelectPalette(m_hDC, m_hOldPalette, true);
	if (m_hOldFont)
	{
		HFONT hFont = (HFONT)::SelectObject(m_hDC, m_hOldFont);
		::DeleteObject(hFont);
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::CreatePalette()
{
	m_hPalette = NULL;
	LPLOGPALETTE lpLogPalette;

	int nPaletteSize = sizeof(LOGPALETTE) + (216 * sizeof(PALETTEENTRY));
	if (lpLogPalette = (LPLOGPALETTE)malloc(nPaletteSize))
	{
		memset(lpLogPalette, 0, nPaletteSize);
		lpLogPalette->palVersion = 0x300;
		lpLogPalette->palNumEntries = 216;
		for (int r = 0, n = 0; r < 6; r++)
		{
			for (int g = 0; g < 6; g++)
			{
				for (int b = 0; b < 6; b++, n++)
				{
					lpLogPalette->palPalEntry[n].peRed = (BYTE)(r * 51);
					lpLogPalette->palPalEntry[n].peGreen = (BYTE)(g * 51);
					lpLogPalette->palPalEntry[n].peBlue = (BYTE)(b * 51);
				}
			}
		}
		m_hPalette = ::CreatePalette(lpLogPalette);
		free(lpLogPalette);
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::Dither(const BYTE* pSrcBits, BYTE* pDstBits, int nSrcBitCount, int nDstWidth, int y, Fixed fixSrcStepX, const RGBQUAD* pColors) const
{
	Fixed fixPosX = (fixSrcStepX >> 1);
	WORD wSrcPosX = 0;
	int Red = 0;
	int Green = 0;
	int Blue = 0;
	BYTE bIndex, bTemp, bRedTemp, bGreenTemp, bBlueTemp;

	for (int x = 0; x < nDstWidth; x++)
	{
		wSrcPosX = FixedToInt(fixPosX);
	
		if (nSrcBitCount == 8)
		{
			bIndex = pSrcBits[wSrcPosX];
			Red = pColors[bIndex].rgbRed;
			Green = pColors[bIndex].rgbGreen;
			Blue = pColors[bIndex].rgbBlue;
		}
		else
		if (nSrcBitCount == 24)
		{
			wSrcPosX *= 3;
			Blue = pSrcBits[wSrcPosX];
			Green = pSrcBits[wSrcPosX + 1];
			Red = pSrcBits[wSrcPosX + 2];
		}
		else
		if (nSrcBitCount == 4)
		{
			if (wSrcPosX % 2)
				bIndex = (BYTE)(pSrcBits[wSrcPosX >> 1] & 0x0f);
			else
				bIndex = (BYTE)(pSrcBits[wSrcPosX >> 1] >> 4);
			Red = pColors[bIndex].rgbRed;
			Green = pColors[bIndex].rgbGreen;
			Blue = pColors[bIndex].rgbBlue;
		}

		fixPosX += fixSrcStepX;

		bTemp = aHalftone16x16[(x & 0x0f)][(y & 0x0f)];
		bRedTemp = aDividedBy51[Red];
		bGreenTemp = aDividedBy51[Green];
		bBlueTemp = aDividedBy51[Blue];

		if (aModulo51[Red] > bTemp)
			bRedTemp++;
		if (aModulo51[Green] > bTemp)
			bGreenTemp++;
		if (aModulo51[Blue] > bTemp)
			bBlueTemp++;

		pDstBits[x] = (BYTE)(aTimes36[bRedTemp] + aTimes6[bGreenTemp] + bBlueTemp);
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::ExtTextOut( int x, int y, UINT nOptions, const RECT* pRect, const TCHAR* pszString, UINT nCount, const int* pDxWidths)
{
	if (m_bDoTransform)
	{
		int* pWidths = NULL;
		if (pDxWidths)
		{
			pWidths = new int[nCount];

			CAGMatrix CTM = GetCTM();
			int Angle = 0;
			if (CTM.GetRotation(Angle))
				CTM.Rotate(-Angle / 10);

			int nCurX = 0;
			int nPrevX = 0;
			for (UINT i = 0; i < nCount; i++)
			{
				nCurX += pDxWidths[i];
				POINT Pt = {nCurX, 0};

				if (Angle)
					CTM.Transform(&Pt, 1, false);
				else
					LPAtoDPA(&Pt, 1);

				pWidths[i] = Pt.x - nPrevX;
				nPrevX = Pt.x;
			}
		}

		POINT Pt = {x, y};
		LPtoDP(&Pt, 1);

		::ExtTextOut(m_hDC, Pt.x, Pt.y, nOptions, pRect, pszString, nCount,
		  pWidths);

		if (pWidths)
			delete [] pWidths;
	}
	else
	{
		::ExtTextOut(m_hDC, x, y, nOptions, pRect, pszString, nCount,
		  pDxWidths);
	}
}

//--------------------------------------------------------------------------//
// Fixed point division.													//
//--------------------------------------------------------------------------//
Fixed CAGDC::FixedDivide(Fixed Dividend, Fixed Divisor) const
{
	Fixed fixResult = 0;
	_asm
	{
		mov 	eax,Dividend
		mov 	ecx,Divisor

		rol 	eax,10h
		movsx	edx,ax
		xor 	ax,ax

		idiv	ecx
		shld	edx,eax,16
		mov 	[fixResult], eax
	};
	return (fixResult);
}

//--------------------------------------------------------------------------//
// Flip a DIB horizontally, vertically or both. 							//
//																			//
// Currently only supports 8 and 24 bit DIBs.								//
//--------------------------------------------------------------------------//
void CAGDC::FlipDIB(const BYTE* pBits, BYTE* pNewBits, const BITMAPINFOHEADER* pbih, bool bFlipX, bool bFlipY) const
{
	const BYTE* pSrc = pBits;
	int nWidth = DibWidthBytes(pbih);

	BYTE* pDest;
	int nDestInc;

	if (bFlipY)
	{
		pDest = pNewBits + (nWidth *(pbih->biHeight - 1));
		nDestInc = -nWidth;
	}
	else
	{
		pDest = pNewBits;
		nDestInc = nWidth;
	}

	for (int y = 0; y < pbih->biHeight; y++)
	{
		if (bFlipX)
		{
			if (pbih->biBitCount == 8)
			{
				for (int x = 0; x < pbih->biWidth; x++)
					pDest[x] = pSrc[pbih->biWidth - x - 1];
			}
			else
			if (pbih->biBitCount == 24)
			{
				for (int x = 0; x < pbih->biWidth; x++)
				{
					int nDstOffset = x * 3;
					int nSrcOffset = (pbih->biWidth - x - 1) * 3;
					pDest[nDstOffset] = pSrc[nSrcOffset];
					pDest[nDstOffset + 1] = pSrc[nSrcOffset + 1];
					pDest[nDstOffset + 2] = pSrc[nSrcOffset + 2];
				}
			}
		}
		else
			memcpy(pDest, pSrc, nWidth);

		pSrc += nWidth;
		pDest += nDestInc;
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::Free()
{
	if (m_hPalette)
	{
		::DeleteObject(m_hPalette);
		m_hPalette = NULL;
	}

	if (m_pBltBuf)
	{
		free(m_pBltBuf);
		m_pBltBuf = NULL;
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
const CAGMatrix& CAGDC::GetCTM()
{
	if (m_bUpdateCTM)
	{
		m_MatrixCTM = GetModelingMatrix() * GetVxD();
		m_bUpdateCTM = false;
	}

	return (m_MatrixCTM);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
int CAGDC::GetMinAWidth(UINT uFirstChar, UINT uLastChar)
{
	POINT MinAWidth = {0, 0};
	int nChars = uLastChar - uFirstChar + 1;
	ABC* pABC = new ABC[nChars];

	if (::GetCharABCWidths(m_hDC, uFirstChar, uLastChar, pABC))
	{
		for (int i = 0; i < nChars; i++)
			MinAWidth.x = min(MinAWidth.x, pABC[i].abcA);
	}
	delete [] pABC;

	if (m_bDoTransform)
		LPAtoDPA(&MinAWidth, 1);

	return (MinAWidth.x);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
const CAGMatrix& CAGDC::GetVxD()
{
	if (m_bUpdateVxD)
	{
		m_MatrixVxD = GetViewingMatrix() * GetDeviceMatrix();
		m_bUpdateVxD = false;
	}
	return (m_MatrixVxD);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
const CAGMatrix& CAGDC::GetMxV()
{
	if (m_bUpdateMxV)
	{
		m_MatrixVxD = GetModelingMatrix() * GetViewingMatrix();
		m_bUpdateMxV = false;
	}
	return (m_MatrixMxV);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::GetTextExtent(const TCHAR* pString, int nCount, SIZE* pSize)
{
	::GetTextExtentPoint32(m_hDC, pString, nCount, pSize);
	if (m_bDoTransform)
		LPAtoDPA((POINT*)pSize, 1);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::GetTextMetrics(TEXTMETRIC* ptm) const
{
	::GetTextMetrics(m_hDC, ptm);
	if (m_bDoTransform)
	{
		// todo
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::Init()
{
	m_Info.Init(m_hDC);

	m_bGivenDC = false;
	m_hWnd = NULL;
	m_hOldFont = NULL;
	m_hOldPalette = NULL;
	if (m_Info.m_bPalette)
	{
		if (!m_hPalette)
			CreatePalette();

		m_hOldPalette = ::SelectPalette(m_hDC, m_hPalette, true);
		::RealizePalette(m_hDC);
	}

	::SetMapMode(m_hDC, MM_TEXT);
	::SetTextAlign(m_hDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
	::SetBkMode(m_hDC, TRANSPARENT);

	m_MatrixD.SetMatrix((double)m_Info.m_nLogPixelsX / (double)APP_RESOLUTION,
	  0, 0, (double)m_Info.m_nLogPixelsY / (double)APP_RESOLUTION,
	  -m_Info.m_PrintOffset.cx, -m_Info.m_PrintOffset.cy);

	m_bUpdateVxD = true;
	m_bUpdateMxV = false;
	m_bUpdateCTM = true;
	m_bDoTransform = true;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::InvertLine(POINT ptFrom, POINT ptTo)
{
	HPEN hOldPen = (HPEN)::SelectObject(m_hDC, ::GetStockObject(BLACK_PEN));
	int OldROP2 = ::SetROP2(m_hDC, R2_NOT);

	if (m_bDoTransform)
	{
		LPtoDP(&ptFrom, 1);
		LPtoDP(&ptTo, 1);
	}

	::MoveToEx(m_hDC, ptFrom.x, ptFrom.y, NULL);
	::LineTo(m_hDC, ptTo.x, ptTo.y);

	::SetROP2(m_hDC, OldROP2);
	::SelectObject(m_hDC, hOldPen);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::InvertRect(const RECT& Rect)
{
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDC,
	  ::GetStockObject(BLACK_BRUSH));
	HPEN hOldPen = (HPEN)::SelectObject(m_hDC, ::GetStockObject(NULL_PEN));
	int OldROP2 = ::SetROP2(m_hDC, R2_NOT);

	Rectangle(Rect);

	::SetROP2(m_hDC, OldROP2);
	::SelectObject(m_hDC, hOldPen);
	::SelectObject(m_hDC, hOldBrush);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::Polygon(const POINT* pPts, int nPoints)
{
	POINT* pPoints = new POINT[nPoints];
	memcpy(pPoints, pPts, nPoints * sizeof(POINT));

	if (m_bDoTransform)
		LPtoDP(pPoints, nPoints);

	::Polygon(m_hDC, pPoints, nPoints);

	delete [] pPoints;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::Rectangle(const RECT& Rect)
{
	RECT r = Rect;

	if (m_bDoTransform)
	{
		int Angle = 0;
		if (GetCTM().GetRotation(Angle))
		{
			POINT Pts[4];
			Pts[0].x = Pts[1].x = r.left;
			Pts[2].x = Pts[3].x = r.right;
			Pts[0].y = Pts[3].y = r.top;
			Pts[1].y = Pts[2].y = r.bottom;

			Polygon(Pts, 4);
			return;
		}

		LPtoDP(&r);
	}

	::Rectangle(m_hDC, r.left, r.top, r.right, r.bottom);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::SetFont(const UFont& theFont)
//jvoid CAGDC::SetFont(const LOGFONT& theFont)
{
	LOGFONT lf = theFont;

	if (m_bDoTransform)
	{
		POINT Pt = { lf.lfWidth, abs(lf.lfHeight) };

		CAGMatrix CTM = GetCTM();
		int Angle = 0;
		if (CTM.GetRotation(Angle))
		{
			CTM.Rotate(-Angle / 10);
			CTM.Transform(&Pt, 1, false);
			lf.lfEscapement = Angle;
		}
		else
			LPAtoDPA(&Pt, 1);

		if (theFont.lfWidth)
//j		if (lf.lfWidth)
			lf.lfWidth = Pt.x;
		lf.lfHeight = (theFont.lfHeight < 0) ? -abs(Pt.y) : abs(Pt.y);
//j		lf.lfHeight = (lf.lfHeight < 0) ? -abs(Pt.y) : abs(Pt.y);
	}

	if (m_CurFont != lf)
//j	if (memcmp(&m_CurFont, &lf, sizeof(LOGFONT)))
	{
		HFONT hFont = ::CreateFontIndirect(&lf);
		HFONT hOldFont = (HFONT)::SelectObject(m_hDC, hFont);
		if (m_hOldFont)
			::DeleteObject(hOldFont);
		else
			m_hOldFont = hOldFont;
		m_CurFont = lf;
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::SetTextColor(COLORREF Color) const
{
	if (m_Info.m_bPalette)
		Color |= PALETTERGB_FLAG;

	::SetTextColor(m_hDC, Color);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGDC::StartDoc(const char* pszDocName) const
{
	DOCINFO di;
	memset(&di, 0, sizeof(di));
	di.cbSize = sizeof(di);
	di.lpszDocName = pszDocName;
	return (::StartDoc(m_hDC, &di) > 0);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGDC::StartPage()
{
	if (m_hOldFont)
	{
		HFONT hFont = (HFONT)::SelectObject(m_hDC, m_hOldFont);
		::DeleteObject(hFont);
		m_hOldFont = NULL;

		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));
		m_CurFont.setLogFont(lf);
//j		memset(&m_CurFont, 0, sizeof(m_CurFont));
	}

	bool bReturn = (::StartPage(m_hDC) > 0);

	::SetMapMode(m_hDC, MM_TEXT);
	::SetTextAlign(m_hDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
	::SetBkMode(m_hDC, TRANSPARENT);

	return (bReturn);
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::StretchBlt(RECT DestRect, const void* pvBits, const BITMAPINFO* pbi)
{
	if (m_bDoTransform)
		LPtoDP(&DestRect);
	
	if (!WIDTH(DestRect) || !HEIGHT(DestRect))
		return;

	if (m_Info.m_bRasDisplay)
	{
		if (!m_pBltBuf)
			m_pBltBuf = (BYTE*)malloc(BLTBUFSIZE);

		if (m_pBltBuf)
		{
			StretchBlt2(DestRect.left, DestRect.top,
			  WIDTH(DestRect), HEIGHT(DestRect), pvBits, pbi);
		}
	}
	else
	{
		bool bFlipX = (WIDTH(DestRect) < 0);
		bool bFlipY = (HEIGHT(DestRect) < 0);
		if (bFlipX || bFlipY)
		{
			BYTE* pNewBits = (BYTE*)malloc(DibSizeImage(&pbi->bmiHeader));
			FlipDIB((BYTE*)pvBits, pNewBits, &pbi->bmiHeader, bFlipX, bFlipY);
			if (bFlipX)
				SWAP(DestRect.left, DestRect.right);
			if (bFlipY)
				SWAP(DestRect.top, DestRect.bottom);
			::StretchDIBits(m_hDC, DestRect.left, DestRect.top,
			  WIDTH(DestRect), HEIGHT(DestRect),
			  0, 0, pbi->bmiHeader.biWidth, pbi->bmiHeader.biHeight,
			  pNewBits, pbi, DIB_RGB_COLORS, SRCCOPY);
			free(pNewBits);
		}
		else
		{
			::StretchDIBits(m_hDC, DestRect.left, DestRect.top,
			  WIDTH(DestRect), HEIGHT(DestRect),
			  0, 0, pbi->bmiHeader.biWidth, pbi->bmiHeader.biHeight,
			  pvBits, pbi, DIB_RGB_COLORS, SRCCOPY);
		}
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGDC::StretchBlt2(int nDstX, int nDstY, int nDstWidth, int nDstHeight, const void* pvBits, const BITMAPINFO* pbi) const
{
	struct
	{
		BITMAPINFOHEADER	bih;
		RGBQUAD 			Colors[256];
	} NewHdr;

	NewHdr.bih.biSize = sizeof(BITMAPINFOHEADER);
	NewHdr.bih.biWidth = nDstWidth;
	NewHdr.bih.biHeight = 0;
	NewHdr.bih.biPlanes = 1;
	NewHdr.bih.biCompression = BI_RGB;
	NewHdr.bih.biSizeImage = 0;
	NewHdr.bih.biXPelsPerMeter = 0;
	NewHdr.bih.biYPelsPerMeter = 0;
	NewHdr.bih.biClrUsed = 0;
	NewHdr.bih.biClrImportant = 0;

	UINT iUsage = DIB_RGB_COLORS; 

	if (m_Info.m_bPalette)
	{
		NewHdr.bih.biBitCount = 8;
		NewHdr.bih.biClrUsed = 256;
		for (WORD i = 0; i < 256; i++)
			((WORD*)NewHdr.Colors)[i] = i;
		iUsage = DIB_PAL_COLORS;
	}
	else
	{
		if (pbi->bmiHeader.biBitCount == 1 || pbi->bmiHeader.biBitCount == 4)
			NewHdr.bih.biBitCount = 8;
		else
			NewHdr.bih.biBitCount = pbi->bmiHeader.biBitCount;

		NewHdr.bih.biClrUsed = pbi->bmiHeader.biClrUsed;
		memcpy(NewHdr.Colors, pbi->bmiColors, pbi->bmiHeader.biClrUsed * sizeof(RGBQUAD));
	}

	DWORD dwSrcWidthBytes = DibWidthBytes(&pbi->bmiHeader);
	DWORD dwDstWidthBytes = DibWidthBytes(&NewHdr.bih);
	int nMaxLinesPerBlt = BLTBUFSIZE / dwDstWidthBytes;
	if (nMaxLinesPerBlt > MAX_LINESPERBLT)
		nMaxLinesPerBlt = MAX_LINESPERBLT;
	NewHdr.bih.biHeight = nMaxLinesPerBlt;

	BYTE* pSrcBitsStart = ((BYTE*)pvBits) + ((pbi->bmiHeader.biHeight - 1) * dwSrcWidthBytes);
	BYTE* pSrcBits = NULL;
	BYTE* pDstBits = m_pBltBuf + ((nMaxLinesPerBlt - 1) * dwDstWidthBytes);
	BYTE* pPrevDstBits = NULL;

	bool bNoXStretch = (nDstWidth == pbi->bmiHeader.biWidth && pbi->bmiHeader.biBitCount != 4 && pbi->bmiHeader.biBitCount != 1);
	int nLinesToBlt = 0;

	Fixed fixSrcStepX = FixedDivide(IntToFixed(pbi->bmiHeader.biWidth), IntToFixed(nDstWidth));
	Fixed fixSrcStepY = FixedDivide(IntToFixed(pbi->bmiHeader.biHeight), IntToFixed(nDstHeight));
	Fixed fixPosX = 0;
	Fixed fixPosY = (fixSrcStepY >> 1);
	WORD wSrcPosX = 0;
	WORD wSrcPosY = 0;
	WORD wPrevPosY = 0xffff;

	for (int y = 0; y < nDstHeight; y++)
	{
		wSrcPosY = FixedToInt(fixPosY);
		pSrcBits = pSrcBitsStart -(wSrcPosY * dwSrcWidthBytes);

		if (m_Info.m_bPalette)
		{
			Dither(pSrcBits, pDstBits, pbi->bmiHeader.biBitCount, nDstWidth, y, fixSrcStepX,
			  pbi->bmiColors);
		}
		else
		if (wSrcPosY == wPrevPosY)
			memcpy(pDstBits, pPrevDstBits, dwDstWidthBytes);
		else
		if (bNoXStretch)
			memcpy(pDstBits, pSrcBits, dwDstWidthBytes);
		else
		{
			fixPosX = (fixSrcStepX >> 1);

			if (pbi->bmiHeader.biBitCount == 8)
			{
				for (int x = 0; x < nDstWidth; x++)
				{
					pDstBits[x] = pSrcBits[FixedToInt(fixPosX)];
					fixPosX += fixSrcStepX;
				}
			}
			else
			if (pbi->bmiHeader.biBitCount == 24)
			{
				for (int x = 0; x < nDstWidth; x++)
				{
					wSrcPosX = (WORD)(FixedToInt(fixPosX) * 3);
					int xDstPos = x * 3;
					pDstBits[xDstPos] = pSrcBits[wSrcPosX];
					pDstBits[xDstPos + 1] = pSrcBits[wSrcPosX + 1];
					pDstBits[xDstPos + 2] = pSrcBits[wSrcPosX + 2];
					fixPosX += fixSrcStepX;
				}
			}
			else
			if (pbi->bmiHeader.biBitCount == 4)
			{
				for (int x = 0; x < nDstWidth; x++)
				{
					wSrcPosX = FixedToInt(fixPosX);
					if (wSrcPosX % 2)
						pDstBits[x] = (BYTE)(pSrcBits[wSrcPosX >> 1] & 0x0f);
					else
						pDstBits[x] = (BYTE)(pSrcBits[wSrcPosX >> 1] >> 4);
					fixPosX += fixSrcStepX;
				}
			}
			else
			if (pbi->bmiHeader.biBitCount == 1)
			{
				for (int x = 0; x < nDstWidth; x++)
				{
					wSrcPosX = FixedToInt(fixPosX);
					int nBitPos = wSrcPosX % 8;
					pDstBits[x] = (BYTE)((pSrcBits[wSrcPosX / 8] >> (7 - nBitPos)) & 0x01);
					fixPosX += fixSrcStepX;
				}
			}
		}

		wPrevPosY = wSrcPosY;
		pPrevDstBits = pDstBits;

		if (++nLinesToBlt >= nMaxLinesPerBlt)
		{
			::StretchDIBits(m_hDC, nDstX, nDstY, nDstWidth, nLinesToBlt,
			  0, 0, nDstWidth, nLinesToBlt, m_pBltBuf, (BITMAPINFO*)&NewHdr.bih,
			  iUsage, SRCCOPY);
			nDstY += nLinesToBlt;
			nLinesToBlt = 0;
			pDstBits = m_pBltBuf + ((nMaxLinesPerBlt - 1) * dwDstWidthBytes);
		}
		else
			pDstBits -= dwDstWidthBytes;

		fixPosY += fixSrcStepY;
	}
	
	if (nLinesToBlt)
	{
		NewHdr.bih.biHeight = nLinesToBlt;
		pDstBits = m_pBltBuf + ((nMaxLinesPerBlt - nLinesToBlt) * dwDstWidthBytes);
		::StretchDIBits(m_hDC, nDstX, nDstY, nDstWidth, nLinesToBlt,
		  0, 0, nDstWidth, nLinesToBlt, pDstBits, (BITMAPINFO*)&NewHdr.bih,
		  iUsage, SRCCOPY);
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGPaintDC::CAGPaintDC(HWND hWnd)
{
	m_hDC = ::BeginPaint(hWnd, &m_PaintStruct);
	Init();
	m_hWnd = hWnd;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGPaintDC::~CAGPaintDC()
{
	if (m_hDC)
	{
		CleanUp();
		::EndPaint(m_hWnd, &m_PaintStruct);
		m_hDC = NULL;
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGClientDC::CAGClientDC(HWND hWnd)
{
	m_hDC = ::GetDC(hWnd);
	Init();
	m_hWnd = hWnd;
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGClientDC::~CAGClientDC()
{
	if (m_hDC)
	{
		CleanUp();
		::ReleaseDC(m_hWnd, m_hDC);
		m_hDC = NULL;
	}
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGIC::CAGIC(const char* pszDriver, const char* pszDevice, const char* pszOutput, const DEVMODE* pDevMode)
{
	m_hDC = ::CreateIC(pszDriver, pszDevice, pszOutput, pDevMode);
	Init();
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDIBSectionDC::CAGDIBSectionDC(const BITMAPINFO* pbmi, UINT iUsage, BYTE** ppvBits)
{
	m_hDC = ::CreateCompatibleDC(NULL);
	m_hBitmap = ::CreateDIBSection(m_hDC, pbmi, iUsage, (void**)ppvBits, NULL, 0);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_hBitmap);
	Init();
}

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGDIBSectionDC::~CAGDIBSectionDC()
{
	if (m_hDC)
	{
		CleanUp();
		::SelectObject(m_hDC, m_hOldBitmap);
		::DeleteObject(m_hBitmap);
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}
}
