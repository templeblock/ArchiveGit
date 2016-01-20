#pragma once

// Definitions required for convolution image filtering
#define KERNEL3_SIZE 3
#define KERNEL3_ELEMENTS (KERNEL3_SIZE * KERNEL3_SIZE)

typedef struct
{
	int Element[KERNEL3_ELEMENTS];
	int Divisor;
	int Bias;
} KERNEL3;

class CEffects
{
public:
	CEffects()
	{
	}
	
	~CEffects()
	{
	}

	bool Sharpen(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
	bool Blur(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
	bool Edges(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
	bool Emboss(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
	bool Pixelize(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
	bool Posterize(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
	bool Prism(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
	bool OilPainting(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
	bool Antiqued(int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
	bool Texturize(LPCTSTR pstrType, int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);

protected:
	bool Convolve(const KERNEL3& Kernel, int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib);
};
