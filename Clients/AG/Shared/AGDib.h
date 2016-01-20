#pragma once

struct BITMAPINFOEX
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
};

//////////////////////////////////////////////////////////////////////
inline BITMAPINFOHEADER* DibAlloc(DWORD dwSize)
{
	return (BITMAPINFOHEADER*)malloc(dwSize);
}

//////////////////////////////////////////////////////////////////////
inline void DibFree(BITMAPINFOHEADER* pDIB)
{
	return free(pDIB);
}

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
inline DWORD DibSizeImageRecompute(BITMAPINFOHEADER* pDIB)
{
	return pDIB->biSizeImage = DibWidthBytes(pDIB) * pDIB->biHeight;
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
inline WORD DibBitCount(const BITMAPINFOHEADER* pDIB)
{
	return pDIB->biBitCount;
}

//////////////////////////////////////////////////////////////////////
inline int DibWidth(const BITMAPINFOHEADER* pDIB)
{
	return pDIB->biWidth;
}

//////////////////////////////////////////////////////////////////////
inline int DibHeight(const BITMAPINFOHEADER* pDIB)
{
	return pDIB->biHeight;
}

//////////////////////////////////////////////////////////////////////
inline void DibIntersectRect(const BITMAPINFOHEADER* pDIB, CRect& Rect)
{
	CRect DibRect(0, 0, pDIB->biWidth, pDIB->biHeight);
	Rect.IntersectRect(Rect, DibRect);
}

//////////////////////////////////////////////////////////////////////
inline int DibResolutionX(const BITMAPINFOHEADER* pDIB)
{
	int iResolution = (int)(((double)pDIB->biXPelsPerMeter / 39.37) + 0.5);
	if (!iResolution)
		return 96;
	return iResolution;
}

//////////////////////////////////////////////////////////////////////
inline int DibResolutionY(const BITMAPINFOHEADER* pDIB)
{
	int iResolution = (int)(((double)pDIB->biYPelsPerMeter / 39.37) + 0.5);
	if (!iResolution)
		return 96;
	return iResolution;
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
inline BYTE* DibPtrXYExact(const BITMAPINFOHEADER* pDIB, DWORD x, DWORD y, BYTE* pBits = NULL)
{
	// Flip the y value because DIB's are stored last line first
	y = (DibHeight(pDIB) - 1) - y;
	return DibPtrXY(pDIB, x, y, pBits);
}

/////////////////////////////////////////////////////////////////////////////
inline bool DibWrite(BITMAPINFOHEADER* pDIB, LPCTSTR strImageFile)
{
	if (!pDIB)
		return false;

	if (!pDIB->biSizeImage)
		DibSizeImageRecompute(pDIB);

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
	if (Dib.biBitCount > 8)
		Dib.biBitCount = 24;
	if (Dib.biBitCount <= 8)
		Dib.biClrUsed = (int)(1 << (int)Dib.biBitCount);
	else
		Dib.biClrUsed = 0;

	DibSizeImageRecompute(&Dib);

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
inline bool DibDuplicateData(const BITMAPINFOHEADER* pDIB, BITMAPINFOHEADER* pDIBCopy, BYTE* pBits = NULL)
{
	if (!pDIB || !pDIBCopy)
		return false;

	// Copy the dib header, and the palette
	*pDIBCopy = *pDIB;
	if (DibNumColors(pDIBCopy))
		::CopyMemory(DibColors(pDIBCopy), DibColors(pDIB), DibPaletteSize(pDIB));

	if (!pBits) pBits = DibPtr(pDIB);
	::CopyMemory(DibPtr(pDIBCopy), pBits, DibSizeImage(pDIB));
	return true;
}

//////////////////////////////////////////////////////////////////////
inline BITMAPINFOHEADER* DibCopy(const BITMAPINFOHEADER* pDIB, BYTE* pBits = NULL)
{
	if (!pDIB)
		return NULL;

	BITMAPINFOHEADER* pDIBCopy = DibAlloc(DibSize(pDIB));
	if (!pDIBCopy)
		return NULL;

	DibDuplicateData(pDIB, pDIBCopy, pBits);
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

	DibDuplicateData(pDIB, pDIBCopy, pBits);
	::GlobalUnlock(hMemoryDIB);
	return hMemoryDIB;
}
