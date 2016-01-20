#ifndef __AGDC_H_
#define __AGDC_H_

#include "AGMatrix.h"
#include "sctypes.h" //j
#include "scapptyp.h" //j

typedef DWORD Fixed;
#define IntToFixed(i)(Fixed)(((DWORD)(i)) << 16)
#define FixedToInt(f)(WORD)(((DWORD)f) >> 16)
#define BLTBUFSIZE 65535
#define MAX_LINESPERBLT 20
#define PALETTERGB_FLAG 0x02000000

class CAGDCInfo
{
public:
	CAGDCInfo();
	void Init(HDC hDC);

public:
	bool m_bRasDisplay;
	bool m_bPalette;
	int m_Technology;
	int m_nHorzSize;
	int m_nVertSize;
	int m_nHorzRes;
	int m_nVertRes;
	int m_nLogPixelsX;
	int m_nLogPixelsY;
	SIZE m_PhysPageSize;
	SIZE m_PrintOffset;
};


class CAGDC  
{
public:
	CAGDC(const char* pszDriver, const char* pszDevice, const char* pszOutput, const DEVMODE* pDevMode);
	CAGDC(HDC hDC);
	virtual ~CAGDC();

	void DPtoVP(POINT* pPts, int nCount) const
			{ GetDeviceMatrix().Inverse().Transform(pPts, nCount); }
	void DPtoVP(RECT* pRect) const
			{ DPtoVP((POINT*) pRect, 2); }
	void DPAtoVPA(POINT* pPts, int nCount) const
			{ GetDeviceMatrix().Inverse().Transform(pPts, nCount, false); }
	void DPAtoVPA(RECT* pRect) const
			{ DPAtoVPA((POINT*) pRect, 2); }
	void DPtoMP(POINT* pPts, int nCount)
			{ GetVxD().Inverse().Transform(pPts, nCount); }
	void DPtoMP(RECT* pRect)
			{ DPtoMP((POINT*) pRect, 2); }
	void DPAtoMPA(POINT* pPts, int nCount)
			{ GetVxD().Inverse().Transform(pPts, nCount, false); }
	void DPAtoMPA(RECT* pRect)
			{ DPAtoMPA((POINT*) pRect, 2); }
	void DPtoLP(POINT* pPts, int nCount)
			{ GetCTM().Inverse().Transform(pPts, nCount); }
	void DPtoLP(RECT* pRect)
			{ DPtoLP((POINT*) pRect, 2); }
	void DPAtoLPA(POINT* pPts, int nCount)
			{ GetCTM().Inverse().Transform(pPts, nCount, false); }
	void DPAtoLPA(RECT* pRect)
			{ DPAtoLPA((POINT*) pRect, 2); }
	void VPtoMP(POINT* pPts, int nCount) const
			{ GetViewingMatrix().Inverse().Transform(pPts, nCount); }
	void VPtoMP(RECT* pRect) const
			{ VPtoMP((POINT*) pRect, 2); }
	void VPAtoMPA(POINT* pPts, int nCount) const
			{ GetViewingMatrix().Inverse().Transform(pPts, nCount, false); }
	void VPAtoMPA(RECT* pRect) const
			{ VPAtoMPA((POINT*) pRect, 2); }
	void VPtoLP(POINT* pPts, int nCount)
			{ GetMxV().Inverse().Transform(pPts, nCount); }
	void VPtoLP(RECT* pRect)
			{ VPtoLP((POINT*) pRect, 2); }
	void VPAtoLPA(POINT* pPts, int nCount)
			{ GetMxV().Inverse().Transform(pPts, nCount, false); }
	void VPAtoLPA(RECT* pRect)
			{ VPAtoLPA((POINT*) pRect, 2); }
	void MPtoLP(POINT* pPts, int nCount) const
			{ GetModelingMatrix().Inverse().Transform(pPts, nCount); }
	void MPtoLP(RECT* pRect) const
			{ MPtoLP((POINT*) pRect, 2); }
	void MPAtoLPA(POINT* pPts, int nCount) const
			{ GetModelingMatrix().Inverse().Transform(pPts, nCount, false); }
	void MPAtoLPA(RECT* pRect) const
			{ MPAtoLPA((POINT*) pRect, 2); }
	void LPtoMP(POINT* pPts, int nCount) const
			{ GetModelingMatrix().Transform(pPts, nCount); }
	void LPtoMP(RECT* pRect) const
			{ LPtoMP((POINT*) pRect, 2); }
	void LPAtoMPA(POINT* pPts, int nCount) const
			{ GetModelingMatrix().Transform(pPts, nCount, false); }
	void LPAtoMPA(RECT* pRect) const
			{ LPAtoMPA((POINT*) pRect, 2); }
	void LPtoVP(POINT* pPts, int nCount)
			{ GetMxV().Transform(pPts, nCount); }
	void LPtoVP(RECT* pRect)
			{ LPtoVP((POINT*) pRect, 2); }
	void LPAtoVPA(POINT* pPts, int nCount)
			{ GetMxV().Transform(pPts, nCount, false); }
	void LPAtoVPA(RECT* pRect)
			{ LPAtoVPA((POINT*) pRect, 2); }
	void LPtoDP(POINT* pPts, int nCount)
			{ GetCTM().Transform(pPts, nCount); }
	void LPtoDP(RECT* pRect)
			{ LPtoDP((POINT*) pRect, 2); }
	void LPAtoDPA(POINT* pPts, int nCount)
			{ GetCTM().Transform(pPts, nCount, false); }
	void LPAtoDPA(RECT* pRect)
			{ LPAtoDPA((POINT*) pRect, 2); }
	void MPtoVP(POINT* pPts, int nCount) const
			{ GetViewingMatrix().Transform(pPts, nCount); }
	void MPtoVP(RECT* pRect) const
			{ MPtoVP((POINT*) pRect, 2); }
	void MPAtoVPA(POINT* pPts, int nCount) const
			{ GetViewingMatrix().Transform(pPts, nCount, false); }
	void MPAtoVPA(RECT* pRect) const
			{ MPAtoVPA((POINT*) pRect, 2); }
	void MPtoDP(POINT* pPts, int nCount)
			{ GetVxD().Transform(pPts, nCount); }
	void MPtoDP(RECT* pRect)
			{ MPtoDP((POINT*) pRect, 2); }
	void MPAtoDPA(POINT* pPts, int nCount)
			{ GetVxD().Transform(pPts, nCount, false); }
	void MPAtoDPA(RECT* pRect)
			{ MPAtoDPA((POINT*) pRect, 2); }
	void VPtoDP(POINT* pPts, int nCount) const
			{ GetDeviceMatrix().Transform(pPts, nCount); }
	void VPtoDP(RECT* pRect) const
			{ VPtoDP((POINT*) pRect, 2); }
	void VPAtoDPA(POINT* pPts, int nCount) const
			{ GetDeviceMatrix().Transform(pPts, nCount, false); }
	void VPAtoDPA(RECT* pRect) const
			{ VPAtoDPA((POINT*) pRect, 2); }

	bool AbortDoc() const
			{ return (::AbortDoc(m_hDC) >= 0); }
	bool EndDoc() const
			{ return (::EndDoc(m_hDC) >= 0); }
	bool EndPage() const
			{ return (::EndPage(m_hDC) >= 0); }
	void ExtTextOut(int x, int y, UINT nOptions, const RECT* pRect,
	const TCHAR* pszString, UINT nCount, const int* pDxWidths);
	static void Free();
	const CAGMatrix& GetCTM();
	HDC GetHDC() const
			{ return (m_hDC); }
	const CAGDCInfo& GetDeviceInfo() const
			{ return (m_Info); }
	const CAGMatrix& GetDeviceMatrix() const
			{ return (m_MatrixD); }
	int GetMinAWidth(UINT uFirstChar, UINT uLastChar);
	void GetTextMetrics(TEXTMETRIC* ptm) const;
	HWND GetWnd() const
			{ return (m_hWnd); }
	const CAGMatrix& GetModelingMatrix() const
			{ return (m_MatrixM); }
	const CAGMatrix& GetMxV();
	void GetTextExtent(const TCHAR* pString, int nCount, SIZE* pSize);
	const CAGMatrix& GetViewingMatrix() const
			{ return (m_MatrixV); }
	const CAGMatrix& GetVxD();
	void InvertLine(POINT ptFrom, POINT ptTo);
	void InvertRect(const RECT& Rect);
	void Polygon(const POINT* pPts, int nPoints);
	void PopModelingMatrix()
			{ m_MatrixM.Unity(); m_bUpdateMxV = true; m_bUpdateCTM = true; }
	void PushModelingMatrix(const CAGMatrix& Matrix)
			{ m_MatrixM = Matrix; m_bUpdateMxV = true; m_bUpdateCTM = true; }
	void Rectangle(const RECT& Rect);
	void SetDeviceMatrix(CAGMatrix& Matrix)
			{ m_MatrixD = Matrix; m_bUpdateVxD = true; m_bUpdateCTM = true; }
	void SetFont(const UFont& Font);
//j	void SetFont(const LOGFONT& Font);
	void SetTextColor(COLORREF Color) const;
	bool SetTransformMode(bool bOn)
			{ bool bCur = m_bDoTransform; m_bDoTransform = bOn; return (bCur); }
	void SetViewingMatrix(CAGMatrix& Matrix)
			{ m_MatrixV = Matrix; m_bUpdateVxD = true; m_bUpdateMxV = true; m_bUpdateCTM = true; }
	bool StartDoc(const char* pszDocName) const;
	bool StartPage();
	void StretchBlt(RECT DestRect, const void* pvBits, const BITMAPINFO* pbi);


protected:
	CAGDC()
			{}
	void CleanUp();
	void CreatePalette();
	void Dither(const BYTE* pSrcBits, BYTE* pDstBits, int nSrcBitCount,
	int nDstWidth, int y, Fixed fixSrcStepX, const RGBQUAD* pColors) const;
	void FlipDIB(const BYTE* pBits, BYTE* pNewBits,
	const BITMAPINFOHEADER* pbih, bool bFlipX, bool bFlipY) const;
	void StretchBlt2(int nDstX, int nDstY, int nDstWidth, int nDstHeight,
	const void* pvBits, const BITMAPINFO* pbi) const;
	Fixed FixedDivide(Fixed Dividend, Fixed Divisor) const;
	void Init();

protected:
	bool m_bGivenDC;
	HDC m_hDC;
	HWND m_hWnd;
	static BYTE* m_pBltBuf;
	static HPALETTE m_hPalette;
	HPALETTE m_hOldPalette;
	CAGDCInfo m_Info;
	CAGMatrix m_MatrixM;
	CAGMatrix m_MatrixV;
	CAGMatrix m_MatrixD;
	CAGMatrix m_MatrixVxD;
	CAGMatrix m_MatrixMxV;
	CAGMatrix m_MatrixCTM;
	bool m_bUpdateVxD;
	bool m_bUpdateMxV;
	bool m_bUpdateCTM;
	bool m_bDoTransform;
	HFONT m_hOldFont;
	UFont m_CurFont;
//j	LOGFONT m_CurFont;
};


class CAGPaintDC : public CAGDC
{
public:
	CAGPaintDC(HWND hWnd);
	~CAGPaintDC();

protected:
	PAINTSTRUCT m_PaintStruct;
};


class CAGClientDC : public CAGDC
{
public:
	CAGClientDC(HWND hWnd);
	~CAGClientDC();
};


class CAGIC : public CAGDC
{
public:
	CAGIC(const char* pszDriver, const char* pszDevice = NULL, const char* pszOutput = NULL, const DEVMODE* pDevMode = NULL);
};


class CAGDIBSectionDC : public CAGDC
{
public:
	CAGDIBSectionDC(const BITMAPINFO* pbmi, UINT iUsage, BYTE* *ppvBits);
	~CAGDIBSectionDC();

protected:
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
};

#endif //__AGDC_H_
