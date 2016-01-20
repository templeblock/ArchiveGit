#pragma once

struct BITMAPINFOEX
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
};

//////////////////////////////////////////////////////////////////////
inline DWORD DibHeaderSize(const BITMAPINFOHEADER* pDIB)
{
	return (pDIB->biSize + (pDIB->biCompression == BI_BITFIELDS ? sizeof(DWORD) * 3 : 0));
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibNumColors(const BITMAPINFOHEADER* pDIB)
{
	return ((pDIB->biClrUsed || pDIB->biBitCount > 12) ? pDIB->biClrUsed : (1L << pDIB->biBitCount));
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibDepth(const BITMAPINFOHEADER* pDIB)
{
	return (pDIB->biBitCount == 24 ? 3 : (pDIB->biBitCount == 8 ? 1 : 0));
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibPaletteSize(const BITMAPINFOHEADER* pDIB)
{
	return (DibNumColors(pDIB) * sizeof(RGBQUAD));
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibWidthBytes(const BITMAPINFOHEADER* pDIB)
{
	return (((pDIB->biWidth * pDIB->biBitCount + 31L) / 32) * 4);
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibSizeImage(const BITMAPINFOHEADER* pDIB)
{
	return (pDIB->biSizeImage ? pDIB->biSizeImage : DibWidthBytes(pDIB) * pDIB->biHeight);
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibSize(const BITMAPINFOHEADER* pDIB)
{
	return (DibHeaderSize(pDIB) + DibPaletteSize(pDIB) + DibSizeImage(pDIB));
}

//////////////////////////////////////////////////////////////////////
inline RGBQUAD* DibColors(const BITMAPINFOHEADER* pDIB)
{
	return ((RGBQUAD*)(((BYTE*)pDIB) + DibHeaderSize(pDIB)));
}

//////////////////////////////////////////////////////////////////////
inline BYTE* DibPtr(const BITMAPINFOHEADER* pDIB)
{
	return ((BYTE*)(DibColors(pDIB) + DibNumColors(pDIB)));
}

//////////////////////////////////////////////////////////////////////
inline BYTE* DibPtrXY(const BITMAPINFOHEADER* pDIB, DWORD x, DWORD y, BYTE* pBits = NULL)
{
	if (!pBits) pBits = DibPtr(pDIB);
	return pBits + (y * DibWidthBytes(pDIB)) + (x * DibDepth(pDIB));
}

//////////////////////////////////////////////////////////////////////
inline WORD DibBitCount(const BITMAPINFOHEADER* pDIB)
{
	return pDIB->biBitCount;
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibWidth(const BITMAPINFOHEADER* pDIB)
{
	return pDIB->biWidth;
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibHeight(const BITMAPINFOHEADER* pDIB)
{
	return pDIB->biHeight;
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibResolutionX(const BITMAPINFOHEADER* pDIB)
{
	DWORD dwResolution = (DWORD)(((double)pDIB->biXPelsPerMeter / 39.37) + 0.5);
	if (!dwResolution)
		return 96;
	return dwResolution;
}

//////////////////////////////////////////////////////////////////////
inline DWORD DibResolutionY(const BITMAPINFOHEADER* pDIB)
{
	DWORD dwResolution = (DWORD)(((double)pDIB->biYPelsPerMeter / 39.37) + 0.5);
	if (!dwResolution)
		return 96;
	return dwResolution;
}

/////////////////////////////////////////////////////////////////////////////
inline bool DibWrite(BITMAPINFOHEADER* pDIB, LPCTSTR strImageFile)
{
	if (!pDIB)
		return false;

	if (!pDIB->biSizeImage)
		pDIB->biSizeImage = DibSizeImage(pDIB);

	BITMAPFILEHEADER bf;
	::ZeroMemory(&bf, sizeof(bf));
	bf.bfType = 0x4D42;
	bf.bfOffBits = sizeof(bf) + DibHeaderSize(pDIB) + DibPaletteSize((pDIB));
	bf.bfSize = bf.bfOffBits + DibSizeImage(pDIB);

	HANDLE hFile = ::CreateFile(strImageFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwBytesWritten = 0;
	bool bOK = true;
	bOK &= !!::WriteFile(hFile, (LPCVOID)&bf, sizeof(bf), &dwBytesWritten, NULL);
	bOK &= !!::WriteFile(hFile, pDIB, bf.bfSize - sizeof(bf), &dwBytesWritten, NULL);
	::CloseHandle(hFile);
	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
inline HGLOBAL BitmapToDib(HDC hDC, HBITMAP hBitmap, HPALETTE hPal)
{
	BITMAP bm;
	if (!::GetObject(hBitmap, sizeof(BITMAP), (LPVOID)&bm))
		return NULL;

	BITMAPINFOHEADER Dib;
	::ZeroMemory(&Dib, sizeof(Dib));
 	Dib.biSize = sizeof(BITMAPINFOHEADER);
	Dib.biWidth = bm.bmWidth;
	Dib.biHeight = bm.bmHeight;
	Dib.biPlanes = 1;
	Dib.biCompression = BI_RGB;
	Dib.biXPelsPerMeter = 3937; // 100 pixels/inch
	Dib.biYPelsPerMeter = 3937;
	Dib.biBitCount = bm.bmPlanes * bm.bmBitsPixel;
	if (Dib.biBitCount >= 16)
		Dib.biBitCount = 24;
	if (Dib.biBitCount <= 8)
		Dib.biClrUsed = (int)(1 << (int)Dib.biBitCount);
	else
		Dib.biClrUsed = 0;

	Dib.biSizeImage = DibSizeImage(&Dib);

	// Alloc the buffer to be big enough to hold all the bits
	HGLOBAL hMemory = (BITMAPINFOHEADER*)::GlobalAlloc(GMEM_MOVEABLE, DibSize(&Dib));
	if (!hMemory)
		return NULL;

	BITMAPINFOHEADER* pDIB = (BITMAPINFOHEADER*)::GlobalLock(hMemory);
	*pDIB = Dib;

	bool bDCPassedIn = true;
	if (!hDC)
	{
		bDCPassedIn = false;
		hDC = ::GetDC(NULL);
	}

	HPALETTE hOldPal = NULL;
	if (hPal)
	{
		hOldPal = ::SelectPalette(hDC, hPal, false);
		::RealizePalette(hDC);
	}

	// Call GetDIBits and get the bits
	BYTE* pBits = DibPtr(pDIB);
	if (!::GetDIBits(hDC, hBitmap, 0, (WORD)bm.bmHeight, pBits, (BITMAPINFO*)pDIB, DIB_RGB_COLORS))
	{
		::GlobalUnlock(hMemory);
		::GlobalFree(hMemory);
		return NULL;
	}

	if (hOldPal)
	{
		::SelectPalette(hDC, hOldPal, false);
		::RealizePalette(hDC);
	}

	if (!bDCPassedIn)
		::ReleaseDC(NULL, hDC);

	::GlobalUnlock(hMemory);
	return hMemory;
}

//////////////////////////////////////////////////////////////////////
inline HPALETTE DibCreatePalette(const BITMAPINFOHEADER* pDIB)
{
	int nColors = DibNumColors(pDIB);
	if (!nColors)
		return NULL; // There is no palette

	RGBQUAD* pRGB = DibColors(pDIB);
	if (!pRGB)
		return NULL;

	LOGPALETTE* pPal = (LOGPALETTE*)new char[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*(nColors-1)];
	pPal->palVersion = 0x0300;
	pPal->palNumEntries = nColors;
	for (int i=0; i<nColors; i++)
	{
		pPal->palPalEntry[i].peRed   = pRGB[i].rgbRed;
		pPal->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
		pPal->palPalEntry[i].peBlue  = pRGB[i].rgbBlue;
		pPal->palPalEntry[i].peFlags = NULL;
	}

	HPALETTE hPal = ::CreatePalette(pPal);
	delete pPal;
	return hPal;
}

//////////////////////////////////////////////////////////////////////
inline BITMAPINFOHEADER* DibCopy(const BITMAPINFOHEADER* pDIB, BYTE* pBits = NULL)
{
	BITMAPINFOHEADER* pDIBCopy = (BITMAPINFOHEADER*)malloc(DibSize(pDIB));
	if (!pDIBCopy)
		return NULL;

	*pDIBCopy = *pDIB;
	if (DibNumColors(pDIBCopy))
		memcpy((void*)DibColors(pDIBCopy), (void*)DibColors(pDIB), DibPaletteSize(pDIB));

	if (!pBits) pBits = DibPtr(pDIB);
	memcpy(DibPtr(pDIBCopy), pBits, DibSizeImage(pDIB));
	return pDIBCopy;
}

//////////////////////////////////////////////////////////////////////
inline HGLOBAL DibCopyGlobal(const BITMAPINFOHEADER* pDIB, BYTE* pBits = NULL)
{
	HGLOBAL hMemoryDIB = ::GlobalAlloc(GMEM_MOVEABLE, DibSize(pDIB));
	if (!hMemoryDIB)
		return NULL;

	BITMAPINFOHEADER* pDIBCopy = (BITMAPINFOHEADER*)::GlobalLock(hMemoryDIB);
	if (!pDIBCopy)
		return NULL;

	*pDIBCopy = *pDIB;
	if (DibNumColors(pDIBCopy))
		memcpy((void*)DibColors(pDIBCopy), (void*)DibColors(pDIB), DibPaletteSize(pDIB));

	if (!pBits) pBits = DibPtr(pDIB);
	memcpy(DibPtr(pDIBCopy), pBits, DibSizeImage(pDIB));
	::GlobalUnlock(hMemoryDIB);
	return hMemoryDIB;
}

static bool bSkipBlur = true;

//////////////////////////////////////////////////////////////////////
inline void DibBlur(const BITMAPINFOHEADER* pSrcDIB)
{
	if (bSkipBlur)
		return;

	if (!pSrcDIB)
		return;

	if (DibDepth(pSrcDIB) != 1)
		return;

	RGBQUAD* pSrcColors = DibColors(pSrcDIB);
	if (!pSrcColors)
		return;

	for (int i = 0; i < 256; i++)
	{
		BYTE cValue = i;
		pSrcColors[i].rgbRed		= cValue;
		pSrcColors[i].rgbGreen		= cValue;
		pSrcColors[i].rgbBlue		= cValue;
		pSrcColors[i].rgbReserved	= 0;
	}

	int nSrcHeight = DibHeight(pSrcDIB);
	int nSrcWidth = DibWidth(pSrcDIB);

	BYTE* pPrev = NULL;
	BYTE* pCurr = DibPtrXY(pSrcDIB, 0, 0);
	BYTE* pNext = DibPtrXY(pSrcDIB, 0, 1);
	for (int y = 1; y < nSrcHeight-1; y++)
	{
		pPrev = pCurr;
		pCurr = pNext;
		pNext = DibPtrXY(pSrcDIB, 0, y+1);

		BYTE* pP = pPrev;
		BYTE* pC = pCurr;
		BYTE* pN = pNext;
		for (int x = 1; x < nSrcWidth-1; x++)
		{
			int iValue = 0;
			iValue += (*pP + *(pP+1) + *(pP+2));
			iValue += (*pC + *(pC+1) + *(pC+2));
			iValue += (*pN + *(pN+1) + *(pN+2));
			iValue /= 9;
			BYTE* pDst = pC+1;
			*pDst = iValue;
			pP++;
			pC++;
			pN++;
		}
	}
}

#ifdef NOTUSED
//////////////////////////////////////////////////////////////////////
inline void GrayBlt(const BITMAPINFOHEADER* pSrcDIB, const BITMAPINFOHEADER* pDstDIB, COLORREF TransparentColor)
{
	if (!pSrcDIB || !pDstDIB)
		return;

	if (DibHeight(pSrcDIB) != DibHeight(pDstDIB))
		return;

	if (DibWidth(pSrcDIB) != DibWidth(pDstDIB))
		return;

	if ((DibDepth(pSrcDIB) != 1 && DibDepth(pSrcDIB) != 3) ||
		(DibDepth(pDstDIB) != 1 && DibDepth(pDstDIB) != 3))
		return;

	int nDstDepth = DibDepth(pDstDIB);
	int nSrcDepth = DibDepth(pSrcDIB);
	RGBQUAD* pSrcColors = DibColors(pSrcDIB);

	DWORD dwSrcWidthBytes = DibWidthBytes(pSrcDIB);
	DWORD dwDstWidthBytes = DibWidthBytes(pDstDIB);
	int nDstHeight = DibHeight(pDstDIB);

	for (int y = 0; y < nDstHeight; y++)
	{
		BYTE* pSrc = DibPtrXY(pSrcDIB, 0, y);
		BYTE* pDst = DibPtrXY(pDstDIB, 0, y);
		int nDstWidth = DibWidth(pDstDIB);

		while (--nDstWidth >= 0)
		{
			bool bGray;
			if (TransparentColor == CLR_NONE)
				bGray = true;
			else
			{
				COLORREF SrcColor = CLR_NONE;
				if (nSrcDepth == 3)
				{
					BYTE b = *pSrc++;
					BYTE g = *pSrc++;
					BYTE r = *pSrc++;
					SrcColor = RGB(r,g,b);
				}
				else
				{
					BYTE c = *pSrc++;
					RGBQUAD* p = &pSrcColors[c];
					SrcColor = RGB(p->rgbRed, p->rgbGreen, p->rgbBlue);
				}

				bGray = (SrcColor != TransparentColor);
			}

			BYTE gray = (bGray ? 128 : 255);
			*pDst++ = gray;
			if (nDstDepth == 3)
			{
				*pDst++ = gray;
				*pDst++ = gray;
			}
		}
	}
}
#endif NOTUSED
