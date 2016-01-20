#pragma once

// The flat dib interface - not need by the CDib class
#include "AGDib.h"

class HISTOGRAM;

class CDib
{
public:
	CDib(const BITMAPINFOHEADER* pDib = NULL);
	~CDib();

	operator BITMAPINFOHEADER*() const
	{
		return m_pDib;
	}

	operator BITMAPINFOHEADER() const
	{
		return *m_pDib;
	}

	CDib& operator=(BITMAPINFOHEADER* pDib)
	{
		Cleanup();
		m_pDib = pDib;
		return *this;
	}

	BITMAPINFOHEADER* operator&()
	{
		return m_pDib;
	}

	BITMAPINFOHEADER* operator->()
	{
		return m_pDib;
	}

	void Cleanup();
	void Attach(BITMAPINFOHEADER* pDib);
	BITMAPINFOHEADER* Detach();
	bool Create(const BITMAPINFOHEADER* pSrcDib, RGBQUAD* pColors = NULL);
	bool Create(int iBitCount, int iWidth, int iHeight, RGBQUAD* pColors = NULL);
	inline BITMAPINFOHEADER* Alloc(DWORD dwSize)
	{
		return (BITMAPINFOHEADER*)malloc(dwSize);
	}

	void Free();
	DWORD HeaderSize();
	DWORD NumColors();
	DWORD Depth();
	DWORD PaletteSize();
	DWORD WidthBytes();
	DWORD SizeImageRecompute();
	DWORD SizeImage();
	DWORD Size();
	RGBQUAD* Colors();
	WORD BitCount();
	DWORD Compression();
	int Width();
	int Height();
	void IntersectRect(CRect& Rect);
	int ResolutionX();
	int ResolutionY();
	BYTE* Ptr();
	BYTE* PtrXY(DWORD x, DWORD y, BYTE* pBits = NULL);
	BYTE* PtrXYExact(DWORD x, DWORD y, BYTE* pBits = NULL);
	bool Write(LPCTSTR strImageFile);
	bool DuplicateData(BITMAPINFOHEADER* pDIBCopy, BYTE* pBits = NULL);
	BITMAPINFOHEADER* Copy(BYTE* pBits = NULL);
	HGLOBAL CopyGlobal(BYTE* pBits = NULL);
	BITMAPINFOHEADER* OrientRestore(BYTE iState);
	BITMAPINFOHEADER* Orient(int iRotateDirection, bool bFlipX, bool bFlipY, BYTE* pSrcBits = NULL);
	BITMAPINFOHEADER* Crop(const CRect& CropRect, BYTE* pSrcBits = NULL);
	BITMAPINFOHEADER* Resize(const CSize& Size, BYTE* pSrcBits = NULL);
	BITMAPINFOHEADER* Blur(int iStrength, BYTE* pSrcBits = NULL);
	BITMAPINFOHEADER* Rotate(double fAngle, BYTE* pSrcBits = NULL);
	BITMAPINFOHEADER* MergeMask(const BITMAPINFOHEADER* pMaskDib, COLORREF ColorForMask255);
	BITMAPINFOHEADER* MakeGray();
	BITMAPINFOHEADER* Invert();
	bool Histogram(HISTOGRAM& Histogram);

	static void OrientChangeState(BYTE& iState, int iRotateDirection, bool bFlipX, bool bFlipY);
	static bool OrientDecode(BYTE iState, int& iRotateDirection, bool& bFlipX, bool& bFlipY);

private:
	BITMAPINFOHEADER* m_pDib;
	bool m_bManaged;
};

class HISTOGRAM
{
public:
	int R[256];
	int G[256];
	int B[256];
	int MinIndexR;
	int MinIndexG;
	int MinIndexB;
	int MinIndexGray;
	int MaxIndexR;
	int MaxIndexG;
	int MaxIndexB;
	int MaxIndexGray;

	int MinValueR;
	int MinValueG;
	int MinValueB;
	int MinValueGray;
	int MaxValueR;
	int MaxValueG;
	int MaxValueB;
};
