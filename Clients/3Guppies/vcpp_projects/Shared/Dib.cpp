#include "stdafx.h"
#include "Dib.h"
#include "Utility.h"
#include "MessageBox.h"
#include "ScaleImage.h"
#include "RotateImage.h"
#include "Matrix.h"
#include "Filter.h"

//////////////////////////////////////////////////////////////////////
CDib::CDib(const BITMAPINFOHEADER* pDib)
{
	m_pDib = (BITMAPINFOHEADER*)pDib;
	m_bManaged = false;
}

//////////////////////////////////////////////////////////////////////
CDib::~CDib()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////
void CDib::Cleanup()
{
	if (m_pDib && m_bManaged)
		Free();

	m_pDib = NULL;
	m_bManaged = false;
}

//////////////////////////////////////////////////////////////////////
void CDib::Attach(BITMAPINFOHEADER* pDib)
{
	Cleanup();
	m_pDib = pDib;
	m_bManaged = true;
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Detach()
{
	BITMAPINFOHEADER* pDib = m_pDib;
	m_pDib = NULL;
	m_bManaged = false;
	return pDib;
}

//////////////////////////////////////////////////////////////////////
bool CDib::Create(const BITMAPINFOHEADER* pSrcDib, RGBQUAD* pColors)
{
	if (!pSrcDib)
		return false;

	BITMAPINFOHEADER DibHeader = *pSrcDib;
	CDib Dib(&DibHeader);
	Dib.SizeImageRecompute();

	// Allocate memory for a new dib
	Cleanup();
	m_pDib = Alloc(Dib.Size());
	if (!m_pDib)
		return false;

	m_bManaged = true;

	// Copy the dib header
	*m_pDib = Dib;

	// Copy the palette
	if (NumColors())
	{
		CDib DibSrc(pSrcDib);
		::CopyMemory(Colors(), pColors, PaletteSize());
	}

	::ZeroMemory(Ptr(), SizeImage());
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CDib::Create(int iBitCount, int iWidth, int iHeight, RGBQUAD* pColors)
{
	// Only supports 8 and 24 bit Dib's
	if (iBitCount != 8 && iBitCount != 24)
		return false;

	BITMAPINFOHEADER Dib;
	::ZeroMemory(&Dib, sizeof(Dib));
	Dib.biSize = sizeof(BITMAPINFOHEADER);
	Dib.biWidth = iWidth;
	Dib.biHeight = iHeight;
	Dib.biPlanes = 1;
	Dib.biCompression = BI_RGB;
	Dib.biXPelsPerMeter = 3937; // 100 pixels/inch
	Dib.biYPelsPerMeter = 3937;
	Dib.biBitCount = iBitCount;
	if (Dib.biBitCount > 8)
		Dib.biBitCount = 24;
	if (Dib.biBitCount <= 8)
		Dib.biClrUsed = (int)(1 << (int)Dib.biBitCount);
	else
		Dib.biClrUsed = 0;

	return Create(&Dib, pColors);
}

#ifdef NOTUSED
//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Alloc(DWORD dwSize)
{
	return (BITMAPINFOHEADER*)malloc(dwSize);
}
#endif NOTUSED

//////////////////////////////////////////////////////////////////////
void CDib::Free()
{
	return free(m_pDib);
}

//////////////////////////////////////////////////////////////////////
DWORD CDib::HeaderSize()
{
	return (m_pDib->biSize + (m_pDib->biCompression == BI_BITFIELDS ? sizeof(DWORD) * 3 : 0));
}

//////////////////////////////////////////////////////////////////////
DWORD CDib::NumColors()
{
	return ((m_pDib->biClrUsed || m_pDib->biBitCount > 12) ? m_pDib->biClrUsed : (1L << m_pDib->biBitCount));
}

//////////////////////////////////////////////////////////////////////
DWORD CDib::Depth()
{
	return (BitCount() == 24 ? 3 : (BitCount() == 8 ? 1 : 0));
}

//////////////////////////////////////////////////////////////////////
DWORD CDib::PaletteSize()
{
	return (NumColors() * sizeof(RGBQUAD));
}

//////////////////////////////////////////////////////////////////////
DWORD CDib::WidthBytes()
{
	return (((Width() * BitCount() + 31L) / 32) * 4);
}

//////////////////////////////////////////////////////////////////////
DWORD CDib::SizeImageRecompute()
{
	return m_pDib->biSizeImage = WidthBytes() * Height();
}

//////////////////////////////////////////////////////////////////////
DWORD CDib::SizeImage()
{
	return (m_pDib->biSizeImage ? m_pDib->biSizeImage : WidthBytes() * Height());
}

//////////////////////////////////////////////////////////////////////
DWORD CDib::Size()
{
	return (HeaderSize() + PaletteSize() + SizeImage());
}

//////////////////////////////////////////////////////////////////////
RGBQUAD* CDib::Colors()
{
	return ((RGBQUAD*)(((BYTE*)m_pDib) + HeaderSize()));
}

//////////////////////////////////////////////////////////////////////
WORD CDib::BitCount()
{
	return m_pDib->biBitCount;
}

//////////////////////////////////////////////////////////////////////
DWORD CDib::Compression()
{
	return m_pDib->biCompression;
}

//////////////////////////////////////////////////////////////////////
int CDib::Width()
{
	return m_pDib->biWidth;
}

//////////////////////////////////////////////////////////////////////
int CDib::Height()
{
	return m_pDib->biHeight;
}

//////////////////////////////////////////////////////////////////////
void CDib::IntersectRect(CRect& Rect)
{
	CRect DibRect(0, 0, Width(), Height());
	Rect.IntersectRect(Rect, DibRect);
}

//////////////////////////////////////////////////////////////////////
int CDib::ResolutionX()
{
	int iResolution = (int)(((double)m_pDib->biXPelsPerMeter / 39.37) + 0.5);
	if (!iResolution)
		return 96;
	return iResolution;
}

//////////////////////////////////////////////////////////////////////
int CDib::ResolutionY()
{
	int iResolution = (int)(((double)m_pDib->biYPelsPerMeter / 39.37) + 0.5);
	if (!iResolution)
		return 96;
	return iResolution;
}

//////////////////////////////////////////////////////////////////////
BYTE* CDib::Ptr()
{
	return ((BYTE*)(Colors() + NumColors()));
}

//////////////////////////////////////////////////////////////////////
BYTE* CDib::PtrXY(DWORD x, DWORD y, BYTE* pBits)
{
	if (!pBits) pBits = Ptr();
	return pBits + (y * WidthBytes()) + (x * Depth());
}

//////////////////////////////////////////////////////////////////////
BYTE* CDib::PtrXYExact(DWORD x, DWORD y, BYTE* pBits)
{
	// Flip the y value because DIB's are stored last line first
	y = (Height() - 1) - y;
	return PtrXY(x, y, pBits);
}

/////////////////////////////////////////////////////////////////////////////
bool CDib::Write(LPCTSTR strImageFile)
{
	if (!m_pDib)
		return false;

	if (!m_pDib->biSizeImage)
		SizeImageRecompute();

	BITMAPFILEHEADER bf;
	::ZeroMemory(&bf, sizeof(bf));
	bf.bfType = 0x4D42;
	bf.bfOffBits = sizeof(bf) + HeaderSize() + PaletteSize();
	bf.bfSize = bf.bfOffBits + SizeImage();

	HANDLE hFile = ::CreateFile(strImageFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwBytesWritten = 0;
	bool bOK = true;
	bOK &= !!::WriteFile(hFile, (LPCVOID)&bf, sizeof(bf), &dwBytesWritten, NULL);
	bOK &= !!::WriteFile(hFile, m_pDib, bf.bfSize - sizeof(bf), &dwBytesWritten, NULL);
	::CloseHandle(hFile);
	return bOK;
}

//////////////////////////////////////////////////////////////////////
bool CDib::DuplicateData(BITMAPINFOHEADER* pDIBCopy, BYTE* pBits)
{
	if (!m_pDib || !pDIBCopy)
		return false;

	// Copy the dib header
	*pDIBCopy = *m_pDib;

	// Copy the palette
	CDib DibCopy(pDIBCopy);
	if (DibCopy.NumColors())
		::CopyMemory(DibCopy.Colors(), Colors(), PaletteSize());

	if (!pBits)
		pBits = Ptr();

	::CopyMemory(DibCopy.Ptr(), pBits, SizeImage());
	return true;
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Copy(BYTE* pBits)
{
	if (!m_pDib)
		return NULL;

	BITMAPINFOHEADER* pDIBCopy = Alloc(Size());
	if (!pDIBCopy)
		return NULL;

	DuplicateData(pDIBCopy, pBits);
	return pDIBCopy;
}

//////////////////////////////////////////////////////////////////////
HGLOBAL CDib::CopyGlobal(BYTE* pBits)
{
	HGLOBAL hMemoryDIB = ::GlobalAlloc(GMEM_MOVEABLE, Size());
	if (!hMemoryDIB)
		return NULL;

	BITMAPINFOHEADER* pDIBCopy = (BITMAPINFOHEADER*)::GlobalLock(hMemoryDIB);
	if (!pDIBCopy)
		return NULL;

	DuplicateData(pDIBCopy, pBits);
	::GlobalUnlock(hMemoryDIB);
	return hMemoryDIB;
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::OrientRestore(BYTE iState)
{
	int iRotateDirection = 0;
	bool bFlipX = false;
	bool bFlipY = false;
	bool bOK = OrientDecode(iState, iRotateDirection, bFlipX, bFlipY);
	if (!bOK)
		return NULL;

	return Orient(iRotateDirection, bFlipX, bFlipY);
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Orient(int iRotateDirection, bool bFlipX, bool bFlipY, BYTE* pSrcBits)
{
	if (!m_pDib)
		return NULL;

	// Only supports 8 and 24 bit Dib's
	if (BitCount() != 8 && BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot rotate an image that is %d bits deep", BitCount()));
		return NULL;
	}

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDstHeader = *m_pDib;
	if (iRotateDirection) // If rotating, swap the width and height
		SWAP(DibDstHeader.biWidth, DibDstHeader.biHeight);
	CDib DibDst;
	if (!DibDst.Create(&DibDstHeader, Colors()))
	{
		CMessageBox::Message("Cannot allocate memory during image rotation");
		return NULL;
	}

	// To make walking through the source dib simpler,
	// convert CW rotations into CCW rotations with an X and Y flip
	bool bCCW = (iRotateDirection < 0);
	if (iRotateDirection > 0) // bCW
	{
		bCCW = true;
		bFlipX = !bFlipX;
		bFlipY = !bFlipY;
	}

	// CCW rotations require an X flip
	if (bCCW)
		bFlipX = !bFlipX;

	// Setup the values that we use to walk through the destination dib
	int iDepth = DibDst.Depth();
	int xDst = (!bFlipX ? 0 : DibDst.Width() - 1);
	int yDst = (!bFlipY ? 0 : DibDst.Height() - 1);
	int xDstIncr = (!bFlipX ? iDepth : -iDepth);
	int yDstIncr = (!bFlipY ? (int)DibDst.WidthBytes() : -(int)DibDst.WidthBytes());

	// If doing a CCW rotation, swap the destination increments
	if (bCCW)
		SWAP(xDstIncr, yDstIncr);

	// Setup the dib pointers
	BYTE* pSrcStart = PtrXY(0, 0, pSrcBits);
	BYTE* pDstStart = DibDst.PtrXY(xDst, yDst);
	if (!pSrcStart || !pDstStart)
		return NULL;

	// Walk through the source dib and move pixels to the destination dib
	int xSrcIncr = iDepth;
	int ySrcIncr = WidthBytes();
	int yCount = Height();
	while (--yCount >= 0)
	{
		BYTE* pDst = pDstStart;
		pDstStart += yDstIncr;
		BYTE* pSrc = pSrcStart;
		pSrcStart += ySrcIncr;

		int xCount = Width();
		while (--xCount >= 0)
		{
			*(pDst) = *(pSrc);
			if (iDepth == 3)
			{
				*(pDst+1) = *(pSrc+1);
				*(pDst+2) = *(pSrc+2);
			}
			pSrc += xSrcIncr;
			pDst += xDstIncr;
		}
	}

	return DibDst.Detach();
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Crop(const CRect& CropRect, BYTE* pSrcBits)
{
	if (!m_pDib)
		return NULL;

	// Only supports 8 and 24 bit Dib's
	if (BitCount() != 8 && BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot crop an image that is %d bits deep", BitCount()));
		return NULL;
	}

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDstHeader = *m_pDib;
	DibDstHeader.biWidth = CropRect.Width();
	DibDstHeader.biHeight = CropRect.Height();
	CDib DibDst;
	if (!DibDst.Create(&DibDstHeader, Colors()))
	{
		CMessageBox::Message("Cannot allocate memory during crop");
		return NULL;
	}

	// Setup the values that we use to walk through the destination dib
	int iDepth = DibDst.Depth();
	int xDstIncr = iDepth;
	int yDstIncr = (int)DibDst.WidthBytes();

	// Setup the dib pointers
	BYTE* pSrcStart = PtrXY(CropRect.left, Height() - CropRect.bottom, pSrcBits);
	BYTE* pDstStart = DibDst.PtrXY(0, 0);
	if (!pSrcStart || !pDstStart)
		return NULL;

	// Walk through the source dib and move pixels to the destination dib
	int xSrcIncr = iDepth;
	int ySrcIncr = WidthBytes();
	int yCount = DibDst.Height();
	while (--yCount >= 0)
	{
		BYTE* pDst = pDstStart;
		pDstStart += yDstIncr;
		BYTE* pSrc = pSrcStart;
		pSrcStart += ySrcIncr;

		int xCount = DibDst.Width();
		while (--xCount >= 0)
		{
			*(pDst) = *(pSrc);
			if (iDepth == 3)
			{
				*(pDst+1) = *(pSrc+1);
				*(pDst+2) = *(pSrc+2);
			}
			pSrc += xSrcIncr;
			pDst += xDstIncr;
		}
	}

	return DibDst.Detach();
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Resize(const CSize& Size, BYTE* pSrcBits)
{
	if (!m_pDib)
		return NULL;

	// Only supports 24 bit Dib's
	if (BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot resize an image that is %d bits deep", BitCount()));
		return NULL;
	}

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDstHeader = *m_pDib;
	DibDstHeader.biWidth = Size.cx;
	DibDstHeader.biHeight = Size.cy;
	CDib DibDst;
	if (!DibDst.Create(&DibDstHeader, Colors()))
	{
		CMessageBox::Message("Cannot allocate memory during image resize");
		return NULL;
	}

	// Resize the Dib
	ScaleImage(m_pDib, DibDst);
	return DibDst.Detach();
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Blur(int iStrength, BYTE* pSrcBits)
{
	if (!m_pDib)
		return NULL;

	// Only supports 24 bit Dib's
	if (BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot blur an image that is %d bits deep", BitCount()));
		return NULL;
	}

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDstHeader = *m_pDib;
	CDib DibDst;
	if (!DibDst.Create(&DibDstHeader, Colors()))
	{
		CMessageBox::Message("Cannot allocate memory during image blur");
		return NULL;
	}

	// Don't let the smaller dimension become less than 6 pixels
	double fMinScale = 6.0 / min(Width(), Height());
	double fMaxScale = .25;
	double fDeltaScale = fMaxScale - fMinScale;

	double fScale = (1.0 - iStrength/100.0);
	fScale *= fScale; // A quadratic response
	fScale *= fDeltaScale;
	fScale += fMinScale;
	double fRadius = (1.0 / fScale) / 2;

#ifndef NOTUSED
	// Copy the source Dib header into a tmp Dib
	BITMAPINFOHEADER DibTmpHeader = *m_pDib;
	DibTmpHeader.biWidth = dtoi(fScale * Width());
	DibTmpHeader.biHeight = dtoi(fScale * Height());
	CDib DibTmp;
	if (!DibTmp.Create(&DibTmpHeader, Colors()))
	{
		CMessageBox::Message("Cannot allocate memory during image blur");
		return NULL;
	}

	// Blur the Dib by resizing down and then back up again
	CFilter Filter;
	CFilter::Lanczos3Filter Filter1;
	Filter.Resample(m_pDib, DibTmp, (CFilter::CResamplingFilter&)Filter1);
	CFilter::Lanczos3Filter Filter2;
	Filter.Resample(DibTmp, DibDst, (CFilter::CResamplingFilter&)Filter2);
#else
	// Blur the Dib; too slow to use right now
	CFilter Filter;
	CFilter::GaussianBlur BlurFilter(fRadius);
	Filter.Resample(m_pDib, DibDst, (CFilter::CResamplingFilter&)BlurFilter);
#endif NOTUSED

	return DibDst.Detach();
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Rotate(double fAngle, BYTE* pSrcBits)
{
	if (!m_pDib)
		return NULL;

	// Only supports 8 and 24 bit Dib's
	if (BitCount() != 8 && BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot rotate an image that is %d bits deep", BitCount()));
		return NULL;
	}

	// Rotate the 4 corners of the Dib rect to compute the size of the new, rotated Dib
	CRect DestRect(0, 0, Width(), Height());
	CMatrix Matrix;
	Matrix.Rotate(fAngle, fAngle,
		(DestRect.left + DestRect.right) / 2,
		(DestRect.top + DestRect.bottom) / 2);
	Matrix.Transform(DestRect);

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDstHeader = *m_pDib;
	DibDstHeader.biWidth = DestRect.Width();
	DibDstHeader.biHeight = DestRect.Height();
	CDib DibDst;
	if (!DibDst.Create(&DibDstHeader, Colors()))
	{
		CMessageBox::Message("Cannot allocate memory during image rotation");
		return NULL;
	}

	// Rotate the Dib
	RotateImage(m_pDib, DibDst, fAngle);
	return DibDst.Detach();
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::MergeMask(const BITMAPINFOHEADER* pMaskDib, COLORREF ColorForMask255)
{
	if (!pMaskDib)
		return NULL;

	CDib MaskDib(pMaskDib);
	if (MaskDib.BitCount() != 8)
		return NULL;

	if (!m_pDib)
		return NULL;

	// Only supports 8 and 24 bit Dib's
	if (BitCount() != 8 && BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot apply a border to an image that is %d bits deep", BitCount()));
		return NULL;
	}

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDstHeader = *m_pDib;
	CDib DibDst;
	if (!DibDst.Create(&DibDstHeader, Colors()))
	{
		CMessageBox::Message("Cannot allocate memory during the merge mask");
		return NULL;
	}

	// Setup the values that we use to walk through the dib
	int iDepth = Depth();
	int yIncr = (int)WidthBytes();

	BYTE* pSrcStart = Ptr();
	BYTE* pDstStart = DibDst.Ptr();
	BYTE* pMaskStart = MaskDib.Ptr();
	if (!pSrcStart || !pDstStart || !pMaskStart)
		return NULL;

	BYTE rMask255 = GetRValue(ColorForMask255);
	BYTE gMask255 = GetGValue(ColorForMask255);
	BYTE bMask255 = GetBValue(ColorForMask255);

	// Walk through the Dib and apply the the Mask values to the DibDst
	int yCount = Height();
	for (int y = 0; y < yCount; y++)
	{
		BYTE* pDst = pDstStart;
		BYTE* pSrc = pSrcStart;
		BYTE* pMask = pMaskStart;

		int xCount = Width();
		for (int x = 0; x < xCount; x++)
		{
			BYTE cMask = *pMask++;
			if (cMask == 255)
			{
				*pDst++ = bMask255; pSrc++;
				if (iDepth == 3)
				{
					*pDst++ = gMask255; pSrc++;
					*pDst++ = rMask255; pSrc++;
				}
			}
			else
			if (!cMask)
			{
				*pDst++ = *pSrc++;
				if (iDepth == 3)
				{
					*pDst++ = *pSrc++;
					*pDst++ = *pSrc++;
				}
			}
			else
			{
				*pDst++ = *pSrc + ((255 - *pSrc) * cMask / 255); pSrc++;
				if (iDepth == 3)
				{
					*pDst++ = *pSrc + ((255 - *pSrc) * cMask / 255); pSrc++;
					*pDst++ = *pSrc + ((255 - *pSrc) * cMask / 255); pSrc++;
				}
			}
		}

		pDstStart += yIncr;
		pSrcStart += yIncr;
		pMaskStart += MaskDib.WidthBytes();
	}

	return DibDst.Detach();
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::MakeGray()
{
	if (!m_pDib)
		return NULL;

	// Only supports 24 bit Dib's
	if (BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot make gray an image that is %d bits deep", BitCount()));
		return NULL;
	}

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDstHeader = *m_pDib;
	CDib DibDst;
	if (!DibDst.Create(&DibDstHeader, Colors()))
	{
		CMessageBox::Message("Cannot allocate memory during make gray");
		return NULL;
	}

	// Setup the values that we use to walk through the destination dib
	int iDepth = DibDst.Depth();
	int yDstIncr = (int)DibDst.WidthBytes();

	// Setup the dib pointers
	BYTE* pSrcStart = PtrXY(0, 0);
	BYTE* pDstStart = DibDst.PtrXY(0, 0);
	if (!pSrcStart || !pDstStart)
		return false;

	// Walk through the source dib and move pixels to the destination dib
	int ySrcIncr = WidthBytes();
	int yCount = Height();
	while (--yCount >= 0)
	{
		BYTE* pDst = pDstStart;
		pDstStart += yDstIncr;
		BYTE* pSrc = pSrcStart;
		pSrcStart += ySrcIncr;

		int xCount = Width();
		while (--xCount >= 0)
		{
			BYTE B = *pSrc++;
			BYTE G = *pSrc++;
			BYTE R = *pSrc++;
			BYTE Gray = (BYTE)dtoi((0.299 * R) + (0.587 * G) + (0.114 * B));
			*pDst++ = Gray;
			*pDst++ = Gray;
			*pDst++ = Gray;
		}
	}

	return DibDst.Detach();
}

//////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CDib::Invert()
{
	if (!m_pDib)
		return NULL;

	// Only supports 24 bit Dib's
	if (BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot invert an image that is %d bits deep", BitCount()));
		return NULL;
	}

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDstHeader = *m_pDib;
	CDib DibDst;
	if (!DibDst.Create(&DibDstHeader, Colors()))
	{
		CMessageBox::Message("Cannot allocate memory during image invert");
		return NULL;
	}

	// Setup the values that we use to walk through the destination dib
	int iDepth = DibDst.Depth();
	int yDstIncr = (int)DibDst.WidthBytes();

	// Setup the dib pointers
	BYTE* pSrcStart = PtrXY(0, 0);
	BYTE* pDstStart = DibDst.PtrXY(0, 0);
	if (!pSrcStart || !pDstStart)
		return false;

	// Walk through the source dib and move pixels to the destination dib
	int ySrcIncr = WidthBytes();
	int yCount = Height();
	while (--yCount >= 0)
	{
		BYTE* pDst = pDstStart;
		pDstStart += yDstIncr;
		BYTE* pSrc = pSrcStart;
		pSrcStart += ySrcIncr;

		int xCount = Width();
		while (--xCount >= 0)
		{
			*pDst++ = *pSrc++ ^ 0xFF;
			if (iDepth == 3)
			{
				*pDst++ = *pSrc++ ^ 0xFF;
				*pDst++ = *pSrc++ ^ 0xFF;
			}
		}
	}

	return DibDst.Detach();
}

//////////////////////////////////////////////////////////////////////
bool CDib::Histogram(HISTOGRAM& Histogram)
{
	if (!m_pDib)
		return false;

	// Only supports 24 bit Dib's
	if (BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", BitCount()));
		return false;
	}

	// Setup the dib pointers
	BYTE* pSrcStart = PtrXY(0, 0);
	if (!pSrcStart)
		return false;

	// Walk through the source dib and update the histogram
	::ZeroMemory(&Histogram, sizeof(Histogram));
	int ySrcIncr = WidthBytes();
	int yCount = Height();
	while (--yCount >= 0)
	{
		BYTE* pSrc = pSrcStart;
		pSrcStart += ySrcIncr;

		int xCount = Width();
		int B = 0;
		int G = 0;
		int R = 0;
		while (--xCount >= 0)
		{
			B = *pSrc++;
			G = *pSrc++;
			R = *pSrc++;

			Histogram.R[R]++;
			Histogram.G[G]++;
			Histogram.B[B]++;
		}
	}

	for (int j = 0; j < 256; j++)
	{
		if (Histogram.R[j] > 0)
		{
			Histogram.MinIndexR = j;
			break;
		}
	}

	for (int j = 0; j < 256; j++)
	{
		if (Histogram.G[j] > 0)
		{
			Histogram.MinIndexG = j;
			break;
		}
	}

	for (int j = 0; j < 256; j++)
	{
		if (Histogram.B[j] > 0)
		{
			Histogram.MinIndexB = j;
			break;
		}
	}

	for (int j = 255; j >= 0; j--)
	{
		if (Histogram.R[j] > 0)
		{
			Histogram.MaxIndexR = j;
			break;
		}
	}

	for (int j = 255; j >= 0; j--)
	{
		if (Histogram.G[j] > 0)
		{
			Histogram.MaxIndexG = j;
			break;
		}
	}

	for (int j = 255; j >= 0; j--)
	{
		if (Histogram.B[j] > 0)
		{
			Histogram.MaxIndexB = j;
			break;
		}
	}

	Histogram.MinIndexGray = min(min(Histogram.MinIndexR, Histogram.MinIndexG), Histogram.MinIndexB);
	Histogram.MaxIndexGray = max(max(Histogram.MaxIndexR, Histogram.MaxIndexG), Histogram.MaxIndexB);

	Histogram.MinValueR = MAXLONG;
	Histogram.MaxValueR = 0;
	for (int j = 0; j < 256; j++)
	{
		Histogram.MinValueR = min(Histogram.MinValueR, Histogram.R[j]);
		Histogram.MaxValueR = max(Histogram.MaxValueR, Histogram.R[j]);
	}

	Histogram.MinValueG = MAXLONG;
	Histogram.MaxValueG = 0;
	for (int j = 0; j < 256; j++)
	{
		Histogram.MinValueG = min(Histogram.MinValueG, Histogram.G[j]);
		Histogram.MaxValueG = max(Histogram.MaxValueG, Histogram.G[j]);
	}

	Histogram.MinValueB = MAXLONG;
	Histogram.MaxValueB = 0;
	for (int j = 0; j < 256; j++)
	{
		Histogram.MinValueB = min(Histogram.MinValueB, Histogram.B[j]);
		Histogram.MaxValueB = max(Histogram.MaxValueB, Histogram.B[j]);
	}

	return true;
}

// There are 8 possible orientations after any number of 90 degree rotations and X or Y flips:
//
//	0:Normal	1:FlipX		2:FlipY		3:FlipXY	4:CCWFlipXY	5:CCWFlipY	6:CCWFlipX	7:CCW
//													  (CW)		  (CWFlipX)	  (CWFlipY)	  (CWFlipXY)
//
//	XXXX		XXXX		X			   X		XXXXX		XXXXX		    X		X		
//	X			   X		XXX			 XXX		  X X		X X			  X X		X X		
//	XXX			 XXX		X			   X		  X X		X X			  X X		X X		
//	X			   X		XXXX		XXXX		    X		X			XXXXX		XXXXX

//////////////////////////////////////////////////////////////////////
void CDib::OrientChangeState(BYTE& iState, int iRotateDirection, bool bFlipX, bool bFlipY)
{
							// 0  1  2  3  4  5  6  7
	const BYTE MapCW[8]		= {4, 6, 5, 7, 3, 1, 2, 0};
	const BYTE MapCCW[8]	= {7, 5, 6, 4, 0, 2, 1, 3};
	const BYTE MapFlipX[8]	= {1, 0, 3, 2, 5, 4, 7, 6};
	const BYTE MapFlipY[8]	= {2, 3, 0, 1, 6, 7, 4, 5};

	if (iRotateDirection > 0) // CW
		iState = MapCW[iState];
	if (iRotateDirection < 0) // CCW
		iState = MapCCW[iState];
	if (bFlipX)
		iState = MapFlipX[iState];
	if (bFlipY)
		iState = MapFlipY[iState];
}

//////////////////////////////////////////////////////////////////////
bool CDib::OrientDecode(BYTE iState, int& iRotateDirection, bool& bFlipX, bool& bFlipY)
{
	if (iState < 1 || iState > 7)
		return false;

	// 0:Normal, 1:FlipX, 2:FlipY, 3:FlipXY, 4:CCW-FlipXY, 5:CCW-FlipY, 6:CCW-FlipX, 7:CCW
	iRotateDirection = (iState >= 4 ? -1 : 0);
	bFlipX = (iState == 1 || iState == 3 || iState == 4 || iState == 6 ? true : false);
	bFlipY = (iState == 2 || iState == 3 || iState == 4 || iState == 5 ? true : false);
	return true;
}
