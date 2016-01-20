#include "StdAfx.h"
#include "ImageObject.h"
#include "Image.h"
#include "Utility.h"
#include "MessageBox.h"
#include "math.h"
#include "HSL.h"

#define _HITTEST 3

//////////////////////////////////////////////////////////////////////
CImageObject::CImageObject(const CRect& PageRect)
{
	Init();

	m_PageRect = PageRect;
}

//////////////////////////////////////////////////////////////////////
CImageObject::CImageObject(const CRect& PageRect, const BITMAPINFOHEADER* pDIB)
{
	Init();

	m_PageRect = PageRect;
	LoadDIB(pDIB, false/*bTakeOverMemory*/);

	// The source data will be written to disk so we can recall it when we need it
	// This will set m_strSourceType
	m_strSourceFile = RandomFileName();
	HGLOBAL hMemory = DibCopyGlobal(m_pDIB);
	WriteFile(m_strSourceFile, hMemory, &m_strSourceType);
	m_bSourceFileIsTemporary = true;
}

//////////////////////////////////////////////////////////////////////
CImageObject::CImageObject(const CRect& PageRect, HGLOBAL hMemory)
{
	Init();

	m_PageRect = PageRect;

	// Build the symbol's DIB
	CImage Image(hMemory);
	BITMAPINFOHEADER* pDIB = Image.GetDibPtr();
	if (pDIB)
		LoadDIB(pDIB, false/*bTakeOverMemory*/);

	// The source data will be written to disk so we can recall it when we need it
	// This will set m_strSourceType
	m_strSourceFile = RandomFileName();
	WriteFile(m_strSourceFile, hMemory, &m_strSourceType);
	m_bSourceFileIsTemporary = true;
}

//////////////////////////////////////////////////////////////////////
CImageObject::~CImageObject()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
void CImageObject::Init()
{
	m_nID = 0;
//	m_Matrix
	m_PageRect.SetRectEmpty();
	m_DestRect.SetRectEmpty();
	m_CropRect.SetRectEmpty();
	m_TransparentColor = CLR_NONE;
	m_LastHitColor = CLR_NONE;
	m_cOrientation = 0;
	m_strSourceFile.Empty();
	m_strSourceType.Empty();
	m_bHidden = true;
	m_bDeleted = false;
	m_bModified = false;
	m_bCoverAllowed = false;
	m_bSourceFileIsTemporary = false;
	m_pDIB = NULL;
	m_pDIBPrevious = NULL;
}

//////////////////////////////////////////////////////////////////////
void CImageObject::Close()
{
	FreeDIB();

	// Delete any archived file data
	if (m_bSourceFileIsTemporary && !m_strSourceFile.IsEmpty())
		::DeleteFile(m_strSourceFile);

	Init();
}

//////////////////////////////////////////////////////////////////////
void CImageObject::FreeDIB()
{
	if (!m_pDIB)
		return;

	free(m_pDIB);
	m_pDIB = NULL;
	m_bHidden = true;
}

//////////////////////////////////////////////////////////////////////
CImageObject* CImageObject::Duplicate()
{
	CImageObject* pSym = new CImageObject(m_PageRect);
	if (!pSym)
		return NULL;

	pSym->m_nID = m_nID;
	pSym->m_Matrix = m_Matrix;
	pSym->m_PageRect = m_PageRect;
	pSym->m_DestRect = m_DestRect;
	pSym->m_CropRect = m_CropRect;
	pSym->m_TransparentColor = m_TransparentColor;
	pSym->m_LastHitColor = m_LastHitColor;
	pSym->m_cOrientation = m_cOrientation;
	pSym->m_strSourceFile = DuplicateSourceFile();
	pSym->m_strSourceType = m_strSourceType;
	pSym->m_bHidden = m_bHidden;
	pSym->m_bDeleted = m_bDeleted;
	pSym->m_bModified = m_bModified;
	pSym->m_bCoverAllowed = m_bCoverAllowed;
	pSym->m_bSourceFileIsTemporary = (!m_strSourceFile.IsEmpty() ? true : m_bSourceFileIsTemporary);

	pSym->m_pDIB = DibCopy(m_pDIB);
	pSym->m_pDIBPrevious = DibCopy(m_pDIBPrevious);
//j	pSym->LoadDIB(m_pDIB, false/*bTakeOverMemory*/);

	return pSym;
}

//////////////////////////////////////////////////////////////////////
CString CImageObject::DuplicateSourceFile()
{
	if (m_strSourceFile.IsEmpty())
		return "";

	CString strNewSourceFile = RandomFileName();
	if (!::CopyFile(m_strSourceFile, strNewSourceFile, false/*bFailIfExists*/))
		return "";

	return strNewSourceFile;
}

//////////////////////////////////////////////////////////////////////
void CImageObject::Draw(CAGDC& dc, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return;

	if (!m_pDIB)
		return;

	CAGMatrix MatrixSaved = dc.SetSymbolMatrix(m_Matrix);

	dc.StretchBlt(m_pDIB, m_CropRect, m_DestRect, m_TransparentColor);

	// If necessary, draw the 4 cover rectangles around the image
	m_bCoverAllowed = DoCoverDraw() && dc.GetCoverAllowed();
	if (m_bCoverAllowed)
	{
		CAGMatrix Matrix = dc.GetViewToDeviceMatrix();
		CRect PageRect = dc.GetViewRect();

		HRGN hClipRegion = dc.SaveClipRegion();
		CRect ClipRect = PageRect;
		Matrix.Transform(ClipRect);
		dc.SetClipRect(ClipRect);

		CRect DestRect = GetDestRectTransformed();
		CRect RectL(PageRect.left, DestRect.top, DestRect.left, DestRect.bottom);
		CRect RectT(PageRect.left, PageRect.top, PageRect.right, DestRect.top);
		CRect RectR(DestRect.right, DestRect.top, PageRect.right, DestRect.bottom);
		CRect RectB(PageRect.left, DestRect.bottom, PageRect.right, PageRect.bottom);

		COLORREF FillColor = RGB(255,255,255);
		CPoint pt[4];
		Matrix.TransformRectToPolygon(RectL, pt); dc.FillRectangle(pt, FillColor);
		Matrix.TransformRectToPolygon(RectT, pt); dc.FillRectangle(pt, FillColor);
		Matrix.TransformRectToPolygon(RectR, pt); dc.FillRectangle(pt, FillColor);
		Matrix.TransformRectToPolygon(RectB, pt); dc.FillRectangle(pt, FillColor);

		dc.RestoreClipRegion(hClipRegion);
	}

	dc.SetSymbolMatrix(MatrixSaved);
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::HitTest(CPoint Pt, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return false;

	CRect HitRect(Pt.x-_HITTEST, Pt.y-_HITTEST, Pt.x+_HITTEST, Pt.y+_HITTEST);
	m_Matrix.Inverse().Transform(HitRect);
	m_Matrix.Inverse().Transform(Pt);

	CRect DestRect = m_DestRect;
	if (DestRect.Width() < INCHES(1,8))
		DestRect.InflateRect(INCHES(1,16), 0);
	if (DestRect.Height() < INCHES(1,8))
		DestRect.InflateRect(0, INCHES(1,16));

	bool bHit = !!::IntersectRect(&HitRect, &HitRect, &DestRect);
	if (!bHit)
		return false;

	if (!m_pDIB)
		return true;

	if (m_pDIB->biBitCount != 8 && m_pDIB->biBitCount != 24)
		return true;

	DestRect = m_DestRect;
	CAGMatrix Matrix;
	double xScale = (double)DibWidth(m_pDIB) / DestRect.Width();
	double yScale = (double)DibHeight(m_pDIB) / DestRect.Height();
	Matrix.Translate(-DestRect.left, -DestRect.top);
	Matrix.Scale(xScale, yScale, 0, 0);
	Matrix.Transform(Pt);
	if (Pt.x < 0 || Pt.x >= (long)DibWidth(m_pDIB) || Pt.y < 0 || Pt.y >= (long)DibHeight(m_pDIB))
		return true;

	// Flip the y value because DIB's are stored last line first
	Pt.y = (DibHeight(m_pDIB) - 1) - Pt.y;

	int nBytesPerPixel = m_pDIB->biBitCount / 8;
	BYTE* pPixel = DibPtr(m_pDIB) + (Pt.y * DibWidthBytes(m_pDIB)) + (Pt.x * nBytesPerPixel);
	if (m_pDIB->biBitCount == 24)
	{
		int b = pPixel[0];
		int g = pPixel[1];
		int r = pPixel[2];
		m_LastHitColor = RGB(r,g,b);
	}
	else
	{
		RGBQUAD* pRGB = DibColors(m_pDIB);
		DWORD nColors = DibNumColors(m_pDIB);
		BYTE cIndex = pPixel[0];
		if (pRGB && cIndex < nColors)
		{
			pRGB += cIndex;
			int b = pRGB->rgbBlue;
			int g = pRGB->rgbGreen;
			int r = pRGB->rgbRed;
			m_LastHitColor = RGB(r,g,b);
		}
	}

	if ((m_TransparentColor != CLR_NONE) && (m_TransparentColor == m_LastHitColor))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::ReadStream(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	BYTE cFlags = 0;
	DWORD dwSize = 0;

	pInput->Read(&cFlags, sizeof(cFlags));
	pInput->Read(&m_cOrientation, sizeof(m_cOrientation));
	pInput->Read(&m_TransparentColor, sizeof(m_TransparentColor));
	pInput->Read(&m_DestRect, sizeof(m_DestRect));
	pInput->Read(&m_CropRect, sizeof(m_CropRect));
	pInput->Read(&dwSize, sizeof(dwSize));

	if (cFlags & SYMIMAGE_DIB_WRITTEN)
	{
		BITMAPINFOHEADER* pDIB = (BITMAPINFOHEADER*)malloc(dwSize);
		if (pDIB)
		{
			pInput->Read(pDIB, dwSize);
			m_strSourceType = "DIB";
			BITMAPINFOHEADER* pNewDIB = NULL;
			if (m_cOrientation && (pNewDIB = CDib::OrientRestore(pDIB, m_cOrientation)))
			{
				LoadDIB(pNewDIB, true/*bTakeOverMemory*/);
				free(pDIB);
			}
			else
				LoadDIB(pDIB, true/*bTakeOverMemory*/);
		}
	}
	else
	{
		HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
		if (hMemory)
		{
			void* pMemory = ::GlobalLock(hMemory);
			pInput->Read(pMemory, dwSize);
			::GlobalUnlock(hMemory);

			// Build the symbol's DIB
			CImage Image(hMemory);
			BITMAPINFOHEADER* pDIB = Image.GetDibPtr();
			if (pDIB)
			{
				BITMAPINFOHEADER* pNewDIB = NULL;
				if (m_cOrientation && (pNewDIB = CDib::OrientRestore(pDIB, m_cOrientation)))
					LoadDIB(pNewDIB, true/*bTakeOverMemory*/);
				else
					LoadDIB(pDIB, false/*bTakeOverMemory*/);
			}

			// The source data will be written to disk so we can recall it when we need it
			// This will set m_strSourceType
			m_strSourceFile = RandomFileName();
			WriteFile(m_strSourceFile, hMemory, &m_strSourceType);
			m_bSourceFileIsTemporary = true;
			::GlobalFree(hMemory);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::WriteStream(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	BYTE cFlags = SYMIMAGE_DIB_WRITTEN;
	DWORD dwSize = DibSize(m_pDIB);
	void* pMemory = (void*)m_pDIB;

	pOutput->Write(&cFlags, sizeof(cFlags));
	pOutput->Write(&m_cOrientation, sizeof(m_cOrientation));
	pOutput->Write(&m_TransparentColor, sizeof(m_TransparentColor));
	pOutput->Write(&m_DestRect, sizeof(m_DestRect));
	pOutput->Write(&m_CropRect, sizeof(m_CropRect));
	pOutput->Write(&dwSize, sizeof(dwSize));
	pOutput->Write(pMemory, dwSize);
	
	return true;
}

//////////////////////////////////////////////////////////////////////
void CImageObject::GetDestInflateSize(SIZE& Size)
{
	Size.cx = 0;
	Size.cy = 0;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::LoadDIB(const BITMAPINFOHEADER* pDIB, bool bTakeOverMemory)
{
	if (!pDIB)
		return false;

	FreeDIB();

	if (pDIB->biCompression != BI_RGB)
		return false;

	if (pDIB->biBitCount != 1 && pDIB->biBitCount != 4 &&
		pDIB->biBitCount != 8 && pDIB->biBitCount != 24)
		return false;

	if (bTakeOverMemory)
	{
		m_pDIB = (BITMAPINFOHEADER*)pDIB;
	}
	else
	{
		m_pDIB = DibCopy(pDIB);
		if (!m_pDIB)
			return false;
	}

	if (!m_pDIB->biSizeImage)
		m_pDIB->biSizeImage = DibSizeImage(m_pDIB);
	if (!m_pDIB->biClrUsed)
		m_pDIB->biClrUsed = DibNumColors(m_pDIB);

	// Fit the image rectangle within the page rectangle
	int iWidth = DibWidth(pDIB);
	int iHeight = DibHeight(pDIB);
	int iResolution = DibResolutionX(pDIB);
	if (m_PageRect.IsRectEmpty())
		m_PageRect.SetRect(0, 0, INCHES(iWidth, iResolution), INCHES(iHeight, iResolution));
	else
	{
		// Make sure the image does not hang off the page
		if (INCHES(iWidth, iResolution) > m_PageRect.Width()) 
			iResolution = INCHES(iWidth, m_PageRect.Width());
		if (INCHES(iHeight, iResolution) > m_PageRect.Height())
			iResolution = INCHES(iHeight, m_PageRect.Height());
	}

	// Compute the image size in page units
	iWidth = INCHES(iWidth, iResolution);
	iHeight = INCHES(iHeight, iResolution);

	// Setup the DestRect to be centered within the page rectangle
	int cx = (m_PageRect.right + m_PageRect.left - iWidth) / 2;
	int cy = (m_PageRect.bottom + m_PageRect.top - iHeight) / 2;
	m_DestRect.SetRect(0, 0, iWidth, iHeight);
	m_DestRect.OffsetRect(cx, cy);

	m_CropRect.SetRectEmpty();

	m_bHidden = false;
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::WriteFile(LPCTSTR strFileName, HGLOBAL hMemory, CString* pstrSourceType)
{
	if (!hMemory)
		return false;

	if (!strFileName)
		return false;

	// Create the output file
	HANDLE hFile = ::CreateFile(strFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		::GlobalUnlock(hMemory);
		return false;
	}

	DWORD dwBytesWritten = 0;
	DWORD dwSize = ::GlobalSize(hMemory);
	void* pMemory = ::GlobalLock(hMemory);
	bool bOK = !!::WriteFile(hFile, pMemory, dwSize, &dwBytesWritten, NULL);
	if (pstrSourceType)
		*pstrSourceType = DetermineSourceType((BYTE*)pMemory, dwSize);
	::GlobalUnlock(hMemory);
	::CloseHandle(hFile);

	if (!bOK)
	{
		::DeleteFile(strFileName);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Open(LPCTSTR strFileName)
{
	if (!strFileName)
		return false;

	// Open the source image file
	HANDLE hFile = ::CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	// Allocate enough memory to hold the image
	HGLOBAL hMemory = NULL;
	DWORD dwSize = ::GetFileSize(hFile, NULL);
	if (dwSize <= 0 || !(hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize)))
	{
		::CloseHandle(hFile);
		return false;
	}

	// Read the file
	void* pMemory = ::GlobalLock(hMemory);
	DWORD dwBytesRead = 0;
	bool bOK = !!::ReadFile(hFile, pMemory, dwSize, &dwBytesRead, NULL);
	::CloseHandle(hFile);
	CString strSourceType = DetermineSourceType((BYTE*)pMemory, dwSize);
	::GlobalUnlock(hMemory);

	if (!bOK)
	{
		::GlobalFree(hMemory);
		return false;
	}

	// Build the symbol's DIB
	CImage Image(hMemory);
	BITMAPINFOHEADER* pDIB = Image.GetDibPtr();
	if (!pDIB || !LoadDIB(pDIB, false/*bTakeOverMemory*/))
	{
		::GlobalFree(hMemory);
		return false;
	}

	::GlobalFree(hMemory);
	m_strSourceFile = strFileName;
	m_strSourceType = strSourceType;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::ReOpen()
{
	CString strFileName = m_strSourceFile;
	if (strFileName.IsEmpty())
		return false;

	Close();
	return Open(strFileName);
}

//////////////////////////////////////////////////////////////////////
LPCSTR CImageObject::DetermineSourceType(BYTE* pMemory, DWORD dwSize)
{
	if (!pMemory)
		return "";
	if (dwSize < 10)
		return "";
	
	if (pMemory[6] == 'J'  && pMemory[7] == 'F'  && pMemory[8] == 'I'  && pMemory[9] == 'F')
		return "JPG";
	else
	if (pMemory[0] == 0x01 && pMemory[1] == 0x00 && pMemory[2] == 0x09 && pMemory[3] == 0x00)
		return "WMF";
	else
	if (pMemory[0] == 0xD7 && pMemory[1] == 0xCD && pMemory[2] == 0xC6 && pMemory[3] == 0x9A)
		return "WMF";
	else
	if (pMemory[1] == 'P'  && pMemory[2] == 'N'  && pMemory[3] == 'G')
		return "PNG";
	else
	if (pMemory[0] == 'G'  && pMemory[1] == 'I'  && pMemory[2] == 'F')
		return "GIF";
	else
	if (*(DWORD*)pMemory == sizeof(BITMAPINFOHEADER))
		return "DIB";

	return "";
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::MakeGray()
{
	BITMAPINFOHEADER* pSrcDib = m_pDIB;
	if (!pSrcDib)
		return false;

	if (pSrcDib->biBitCount == 8)
	{
		CMessageBox::Message(String("This image is already gray"));
		return false;
	}

	// Only supports 24 bit Dib's
	if (pSrcDib->biBitCount != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", pSrcDib->biBitCount));
		return false;
	}

	BITMAPINFOHEADER* pDstDib = pSrcDib;
	int iDepth = DibDepth(pDstDib);

	// Setup the values that we use to walk through  the destination dib
	int xDst = 0;
	int yDst = 0;
	int xDstIncr = iDepth;
	int yDstIncr = (int)DibWidthBytes(pDstDib);

	// Setup the dib pointers
	BYTE* pSrcStart = DibPtrXY(pSrcDib, 0, 0, NULL/*pSrcBits*/);
	BYTE* pDstStart = DibPtrXY(pDstDib, xDst, yDst);
	if (!pSrcStart || !pDstStart)
		return false;

	// Walk through the source dib and move pixels to the destination dib
	int xSrcIncr = iDepth;
	int ySrcIncr = DibWidthBytes(pSrcDib);
	int yCount = DibHeight(pSrcDib);
	while (--yCount >= 0)
	{
		BYTE* pDst = pDstStart;
		pDstStart += yDstIncr;
		BYTE* pSrc = pSrcStart;
		pSrcStart += ySrcIncr;

		int xCount = DibWidth(pSrcDib);
		while (--xCount >= 0)
		{
			BYTE B = *(pSrc);
			BYTE G = *(pSrc+1);
			BYTE R = *(pSrc+2);
			BYTE Gray = (BYTE)(((int)(R + G + B + 2)) / 3);

			*(pDst)   = Gray;
			*(pDst+1) = Gray;
			*(pDst+2) = Gray;

			pSrc += xSrcIncr;
			pDst += xDstIncr;
		}
	}

	return true;
}

// Definitions required for convolution image filtering
#define KERNELCOLS 3
#define KERNELROWS 3
#define KERNELELEMENTS (KERNELCOLS * KERNELROWS)

typedef struct
{
	int Element[KERNELELEMENTS];
	int Divisor;
} KERNEL;

// The following kernel definitions are for convolution filtering. Kernel entries are specified
// with a divisor to get around the requirement for floating point numbers in the low pass filters. 

KERNEL KSharpen = {	// Sharpen
  {-1, -1, -1,
   -1,  9, -1,
   -1, -1, -1},
    1				// Divisor
};

KERNEL KSmooth = {	// Smooth
  { 1,  1,  1,
    1,  1,  1,
    1,  1,  1},
    9				// Divisor
};

KERNEL KEdges = {	// Edges
  { 2,  2,  2,
    2, -4,  2,
   -2, -2, -2},
    1				// Divisor
};

KERNEL HP1 = {		// HP filter #1
  {-1, -1, -1,
   -1,  9, -1,
   -1, -1, -1},
    1				// Divisor
};

KERNEL HP2 = {		// HP filter #2
  { 0, -1,  0,
   -1,  5, -1,
    0, -1,  0},
    1				// Divisor
};

KERNEL HP3 = {		// HP filter #3
  { 1, -2,  1,
   -2,  5, -2,
    1, -2,  1},
    1				// Divisor
};

KERNEL LP1 = {		// LP filter #1
  { 1,  1,  1,
    1,  1,  1,
    1,  1,  1},
    9				// Divisor
};

KERNEL LP2 = {		// LP filter #2
  { 1,  1,  1,
    1,  2,  1,
    1,  1,  1},
    10				// Divisor
};

KERNEL LP3 = {		// LP filter #3
  { 1,  2,  1,
    2,  4,  2,
    1,  2,  1},
    16				// Divisor
};

KERNEL EdgeNorth = {// North gradient
  { 1,  1,  1,
    1, -2,  1,
   -1, -1, -1},
    1				// Divisor
};

KERNEL EdgeNorthEast = {	// North East gradient
  { 1,  1,  1,
   -1, -2,  1,
   -1, -1,  1},
    1				// Divisor
};

KERNEL EdgeEast = {	// East gradient
  {-1,  1,  1,
   -1, -2,  1,
   -1,  1,  1},
    1				// Divisor
};

KERNEL EdgeSouthEast = {// South East gradient
  {-1, -1,  1,
   -1, -2,  1,
    1,  1,  1},
    1				// Divisor
};

KERNEL EdgeSouth = {// South gradient
  {-1, -1, -1,
    1, -2,  1,
    1,  1,  1},
    1				// Divisor
};

KERNEL EdgeSouthWest = {// South West gradient
  { 1, -1, -1,
    1, -2, -1,
    1,  1,  1},
    1				// Divisor
};

KERNEL EdgeWest = {	// West gradient
  { 1,  1, -1,
    1, -2, -1,
    1,  1, -1},
    1				// Divisor
};

KERNEL EdgeNorthWest = {// North West gradient
  { 1,  1,  1,
    1, -2, -1,
    1, -1, -1},
    1				// Divisor
};

//////////////////////////////////////////////////////////////////////
bool ConvolveImage(const KERNEL& Kernel, int iStrength, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	if (!pSrcDib || !pDstDib)
		return false;

	// Setup the dib pointers
	BYTE* pSrcStart = DibPtrXY(pSrcDib, 0, 0, NULL/*pSrcBits*/);
	BYTE* pDstStart = DibPtrXY(pDstDib, 0, 0);
	if (!pSrcStart || !pDstStart)
		return false;

	int ySrcIncr = DibWidthBytes(pSrcDib);
	int xCount = DibWidth(pSrcDib);
	int yCount = DibHeight(pSrcDib);

	if (iStrength < 1)
		iStrength = 1;
	if (iStrength > 100)
		iStrength = 100;

	// Compensate for edge effects
	WORD xStart = (KERNELCOLS / 2);
	WORD yStart = (KERNELROWS / 2);
	WORD dx = xCount - (KERNELCOLS - 1);
	WORD dy = yCount - (KERNELROWS - 1);

	for (int y = 0; y < dy; y++)
	{
		// Get ptr to destination data
		BYTE* pDst = pDstStart + (ySrcIncr * (y + yStart)) + (xStart * 3);

		// Setup the pointers to the source data
		BYTE* pSrc1 = pSrcStart + (ySrcIncr * y);
		BYTE* pSrc2 = pSrc1 + ySrcIncr;
		BYTE* pSrc3 = pSrc2 + ySrcIncr;

		for (int x = 0; x < dx; x++)
		{
			// Point at first number in kernel
			const int* pElement = Kernel.Element;
			int Element = 0;
			int R = 0;
			int G = 0;
			int B = 0;

			Element = *pElement++;
			B += *pSrc1++ * Element;
			G += *pSrc1++ * Element;
			R += *pSrc1++ * Element;

			Element = *pElement++;
			B += *pSrc1++ * Element;
			G += *pSrc1++ * Element;
			R += *pSrc1++ * Element;

			Element = *pElement++;
			B += *pSrc1++ * Element;
			G += *pSrc1++ * Element;
			R += *pSrc1++ * Element;

			pSrc1 -= 6;

			Element = *pElement++;
			B += *pSrc2++ * Element;
			G += *pSrc2++ * Element;
			R += *pSrc2++ * Element;

			// Save the center source pixel value
			int OB = *pSrc2++;
			int OG = *pSrc2++;
			int OR = *pSrc2++;

			Element = *pElement++;
			B += OB * Element;
			G += OG * Element;
			R += OR * Element;

			Element = *pElement++;
			B += *pSrc2++ * Element;
			G += *pSrc2++ * Element;
			R += *pSrc2++ * Element;

			pSrc2 -= 6;

			Element = *pElement++;
			B += *pSrc3++ * Element;
			G += *pSrc3++ * Element;
			R += *pSrc3++ * Element;

			Element = *pElement++;
			B += *pSrc3++ * Element;
			G += *pSrc3++ * Element;
			R += *pSrc3++ * Element;

			Element = *pElement++;
			B += *pSrc3++ * Element;
			G += *pSrc3++ * Element;
			R += *pSrc3++ * Element;

			pSrc3 -= 6;

			if (Kernel.Divisor != 1 || iStrength != 100)
			{
				int iDivisor = Kernel.Divisor;
				int ORex = OR * iDivisor;
				int OGex = OG * iDivisor;
				int OBex = OB * iDivisor;

				iDivisor *= 100;
				R  = OR + (((R - ORex) * iStrength) / iDivisor);
				G  = OG + (((G - OGex) * iStrength) / iDivisor);
				B  = OB + (((B - OBex) * iStrength) / iDivisor);
			}

			// Range check the data before storing it in output buffer.
			R = (R > 255 ? 255 : R);
			R = (R <   0 ?   0 : R);
			G = (G > 255 ? 255 : G);
			G = (G <   0 ?   0 : G);
			B = (B > 255 ? 255 : B);
			B = (B <   0 ?   0 : B);

			// Now store the data
			*pDst++ = B;
			*pDst++ = G;
			*pDst++ = R;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Sharpen(int iValue)
{
	// The OnEditStart call
	if (iValue == -1)
	{
		// Only supports 24 bit Dib's
		if (m_pDIB->biBitCount != 24)
		{
			CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", m_pDIB->biBitCount));
			return false;
		}

		if (!m_pDIBPrevious)
			m_pDIBPrevious = DibCopy(m_pDIB);
		return (m_pDIBPrevious != NULL);
	}
	else
	// The OnEditApply call
	if (iValue == -2)
	{
		free(m_pDIBPrevious);
		m_pDIBPrevious = NULL;
		return true;
	}
	else
	// The OnEditCancel call
	if (iValue == -3)
	{
		if (m_pDIBPrevious)
		{
			free(m_pDIB);
			m_pDIB = m_pDIBPrevious;
			m_pDIBPrevious = NULL;
		}
		return true;
	}

	return ConvolveImage(KSharpen, iValue/*iStrength*/, m_pDIBPrevious, m_pDIB);
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Smooth(int iValue)
{
	// The OnEditStart call
	if (iValue == -1)
	{
		// Only supports 24 bit Dib's
		if (m_pDIB->biBitCount != 24)
		{
			CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", m_pDIB->biBitCount));
			return false;
		}

		if (!m_pDIBPrevious)
			m_pDIBPrevious = DibCopy(m_pDIB);
		return (m_pDIBPrevious != NULL);
	}
	else
	// The OnEditApply call
	if (iValue == -2)
	{
		free(m_pDIBPrevious);
		m_pDIBPrevious = NULL;
		return true;
	}
	else
	// The OnEditCancel call
	if (iValue == -3)
	{
		if (m_pDIBPrevious)
		{
			free(m_pDIB);
			m_pDIB = m_pDIBPrevious;
			m_pDIBPrevious = NULL;
		}
		return true;
	}

	return ConvolveImage(KSmooth, iValue/*iStrength*/, m_pDIBPrevious, m_pDIB);
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Edges(int iValue)
{
	// The OnEditStart call
	if (iValue == -1)
	{
		// Only supports 24 bit Dib's
		if (m_pDIB->biBitCount != 24)
		{
			CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", m_pDIB->biBitCount));
			return false;
		}

		if (!m_pDIBPrevious)
			m_pDIBPrevious = DibCopy(m_pDIB);
		return (m_pDIBPrevious != NULL);
	}
	else
	// The OnEditApply call
	if (iValue == -2)
	{
		free(m_pDIBPrevious);
		m_pDIBPrevious = NULL;
		return true;
	}
	else
	// The OnEditCancel call
	if (iValue == -3)
	{
		if (m_pDIBPrevious)
		{
			free(m_pDIB);
			m_pDIB = m_pDIBPrevious;
			m_pDIBPrevious = NULL;
		}
		return true;
	}

	return ConvolveImage(KEdges, iValue/*iStrength*/, m_pDIBPrevious, m_pDIB);
}

typedef struct
{
	BYTE R[256];
	BYTE G[256];
	BYTE B[256];
	bool bUsingHSL;
} RGBCOLORMAP;

//////////////////////////////////////////////////////////////////////
void MakeContrastColormap(RGBCOLORMAP& ColorMap, int iValue)
{
	ColorMap.bUsingHSL = false;
	iValue = (iValue * 5) - 250; // Convert 0 thru 100 to -250 thru +250
	double fAngle = (3.14/4) * (1 - (double)iValue/250); // Convert -250 thru +250 to pi/2 to 0
	double fTanAngle = tan(fAngle);
	int iTanAngle = (int)(128.0 * fTanAngle);
	for (int i=0; i<256; i++)
	{
		int iNewValue = 0;
		int j = i - 128;
		if (j > -iTanAngle && j < iTanAngle)
			iNewValue = 128 + (int)(j / fTanAngle);
		else
		if (j >= iTanAngle)
			iNewValue = 255;
		else
			iNewValue = 0;

		iNewValue = bound(iNewValue, 0, 255);
		ColorMap.R[i] = iNewValue;
		ColorMap.G[i] = iNewValue;
		ColorMap.B[i] = iNewValue;
	}
}

//////////////////////////////////////////////////////////////////////
void MakeBrightnessColormap(RGBCOLORMAP& ColorMap, int iValue)
{
	ColorMap.bUsingHSL = false;
	iValue = (iValue * 5) - 250; // Convert 0 thru 100 to -250 thru +250
	for (int i=0; i<256; i++)
	{
		int iNewValue = iValue + i;
		iNewValue = bound(iNewValue, 0, 255);
		ColorMap.R[i] = iNewValue;
		ColorMap.G[i] = iNewValue;
		ColorMap.B[i] = iNewValue;
	}
}

//////////////////////////////////////////////////////////////////////
void MakeHueColormap(RGBCOLORMAP& ColorMap, int iValue)
{
	static bool bColorize = true;
	ColorMap.bUsingHSL = true;
//j	int iPercent = iValue; // Attempt at Office Picture Manager results
	iValue = (iValue * 5) - 250; // Convert 0 thru 100 to -250 thru +250
	int iHueShift = iValue / 2;
//j	int iHueShift = 128; // Attempt at Office Picture Manager results
	for (int i=0; i<256; i++)
	{
		int iNewValue = iHueShift + (bColorize ? 0 : i);
//j		int iNewValue = ((i * (100-iPercent)) + (iHueShift * iPercent)) / 100; // Attempt at Office Picture Manager results
		if (iNewValue < 0)
			iNewValue += HMAX;
		if (iNewValue > HMAX)
			iNewValue -= HMAX;
		ColorMap.R[i] = iNewValue;
		ColorMap.G[i] = i;
		ColorMap.B[i] = i;
	}
}

//////////////////////////////////////////////////////////////////////
void MakeSaturationColormap(RGBCOLORMAP& ColorMap, int iValue)
{
	ColorMap.bUsingHSL = true;
	iValue = (iValue * 5) - 250; // Convert 0 thru 100 to -250 thru +250
	for (int i=0; i<256; i++)
	{
		int iNewValue = iValue + i;
		iNewValue = bound(iNewValue, 0, 255);
		ColorMap.R[i] = i;
		ColorMap.G[i] = iNewValue;
		ColorMap.B[i] = i;
	}
}

//////////////////////////////////////////////////////////////////////
bool ColormapImage(const RGBCOLORMAP& ColorMap, BITMAPINFOHEADER* pSrcDib, BITMAPINFOHEADER* pDstDib)
{
	if (!pSrcDib || !pDstDib)
		return false;

	// Only supports 24 bit Dib's
	if (pSrcDib->biBitCount != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", pSrcDib->biBitCount));
		return false;
	}

	// Setup the values that we use to walk through  the destination dib
	int xDst = 0;
	int yDst = 0;
	int yDstIncr = (int)DibWidthBytes(pDstDib);

	// Setup the dib pointers
	BYTE* pSrcStart = DibPtrXY(pSrcDib, 0, 0, NULL/*pSrcBits*/);
	BYTE* pDstStart = DibPtrXY(pDstDib, xDst, yDst);
	if (!pSrcStart || !pDstStart)
		return false;

	// Walk through the source dib and move pixels to the destination dib
	int ySrcIncr = DibWidthBytes(pSrcDib);
	int yCount = DibHeight(pSrcDib);
	while (--yCount >= 0)
	{
		BYTE* pDst = pDstStart;
		pDstStart += yDstIncr;
		BYTE* pSrc = pSrcStart;
		pSrcStart += ySrcIncr;

		int xCount = DibWidth(pSrcDib);
		int B = 0;
		int G = 0;
		int R = 0;
		while (--xCount >= 0)
		{
			if (ColorMap.bUsingHSL)
			{
				B = *pSrc++;
				G = *pSrc++;
				R = *pSrc++;

				HSL hsl;
				RGBtoHSL(R, G, B, &hsl);
				hsl.hue = ColorMap.R[hsl.hue];
				hsl.sat = ColorMap.G[hsl.sat];
				hsl.lum = ColorMap.B[hsl.lum];
				RGBS rgb;
				HSLtoRGB(hsl.hue, hsl.sat, hsl.lum, &rgb);
				R = rgb.red;
				G = rgb.green;
				B = rgb.blue;
			}
			else
			{
				B = ColorMap.B[*pSrc++];
				G = ColorMap.G[*pSrc++];
				R = ColorMap.R[*pSrc++];
			}

			*pDst++ = B;
			*pDst++ = G;
			*pDst++ = R;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::AutoCorrect()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Contrast(int iValue)
{
	// The OnEditStart call
	if (iValue == -1)
	{
		// Only supports 24 bit Dib's
		if (m_pDIB->biBitCount != 24)
		{
			CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", m_pDIB->biBitCount));
			return false;
		}

		if (!m_pDIBPrevious)
			m_pDIBPrevious = DibCopy(m_pDIB);
		return (m_pDIBPrevious != NULL);
	}
	else
	// The OnEditApply call
	if (iValue == -2)
	{
		free(m_pDIBPrevious);
		m_pDIBPrevious = NULL;
		return true;
	}
	else
	// The OnEditCancel call
	if (iValue == -3)
	{
		if (m_pDIBPrevious)
		{
			free(m_pDIB);
			m_pDIB = m_pDIBPrevious;
			m_pDIBPrevious = NULL;
		}
		return true;
	}

	RGBCOLORMAP ColorMap;
	MakeContrastColormap(ColorMap, iValue);
	return ColormapImage(ColorMap, m_pDIBPrevious, m_pDIB);
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Brightness(int iValue)
{
	// The OnEditStart call
	if (iValue == -1)
	{
		// Only supports 24 bit Dib's
		if (m_pDIB->biBitCount != 24)
		{
			CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", m_pDIB->biBitCount));
			return false;
		}

		if (!m_pDIBPrevious)
			m_pDIBPrevious = DibCopy(m_pDIB);
		return (m_pDIBPrevious != NULL);
	}
	else
	// The OnEditApply call
	if (iValue == -2)
	{
		free(m_pDIBPrevious);
		m_pDIBPrevious = NULL;
		return true;
	}
	else
	// The OnEditCancel call
	if (iValue == -3)
	{
		if (m_pDIBPrevious)
		{
			free(m_pDIB);
			m_pDIB = m_pDIBPrevious;
			m_pDIBPrevious = NULL;
		}
		return true;
	}

	RGBCOLORMAP ColorMap;
	MakeBrightnessColormap(ColorMap, iValue);
	return ColormapImage(ColorMap, m_pDIBPrevious, m_pDIB);
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::HSLAmount(int iValue)
{
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Hue(int iValue)
{
	// The OnEditStart call
	if (iValue == -1)
	{
		// Only supports 24 bit Dib's
		if (m_pDIB->biBitCount != 24)
		{
			CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", m_pDIB->biBitCount));
			return false;
		}

		if (!m_pDIBPrevious)
			m_pDIBPrevious = DibCopy(m_pDIB);
		return (m_pDIBPrevious != NULL);
	}
	else
	// The OnEditApply call
	if (iValue == -2)
	{
		free(m_pDIBPrevious);
		m_pDIBPrevious = NULL;
		return true;
	}
	else
	// The OnEditCancel call
	if (iValue == -3)
	{
		if (m_pDIBPrevious)
		{
			free(m_pDIB);
			m_pDIB = m_pDIBPrevious;
			m_pDIBPrevious = NULL;
		}
		return true;
	}

	RGBCOLORMAP ColorMap;
	MakeHueColormap(ColorMap, iValue);
	return ColormapImage(ColorMap, m_pDIBPrevious, m_pDIB);
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Saturation(int iValue)
{
	// The OnEditStart call
	if (iValue == -1)
	{
		// Only supports 24 bit Dib's
		if (m_pDIB->biBitCount != 24)
		{
			CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", m_pDIB->biBitCount));
			return false;
		}

		if (!m_pDIBPrevious)
			m_pDIBPrevious = DibCopy(m_pDIB);
		return (m_pDIBPrevious != NULL);
	}
	else
	// The OnEditApply call
	if (iValue == -2)
	{
		free(m_pDIBPrevious);
		m_pDIBPrevious = NULL;
		return true;
	}
	else
	// The OnEditCancel call
	if (iValue == -3)
	{
		if (m_pDIBPrevious)
		{
			free(m_pDIB);
			m_pDIB = m_pDIBPrevious;
			m_pDIBPrevious = NULL;
		}
		return true;
	}

	RGBCOLORMAP ColorMap;
	MakeSaturationColormap(ColorMap, iValue);
	return ColormapImage(ColorMap, m_pDIBPrevious, m_pDIB);
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::RedEyeReset()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::RedEyeFix()
{
	return true;
}

