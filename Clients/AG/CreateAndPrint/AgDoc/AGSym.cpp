#include "stdafx.h"
#include "AGSym.h"
#include "AGDC.h"
#include "Image.h"
#include "Shape.h"
#include <math.h>
#include "ConvertDibToJpg.h"
#include "CalSupport.h"
#include "MessageBox.h"
#include "TrueType.h"
#include "Font.h"

#define _HITTEST 3

#define MyB(rgb)	((BYTE)(rgb))
#define MyG(rgb)	((BYTE)(((WORD)(rgb)) >> 8))
#define MyR(rgb)	((BYTE)((rgb)>>16))
#define MYRGB(hex)	RGB(MyR(hex), MyG(hex), MyB(hex))

//////////////////////////////////////////////////////////////////////
CAGSym::CAGSym(SYMTYPE SymType)
{
	m_SymType = SymType;
	m_nID = ID_NONE;
	m_bHidden = false;
	m_bDeleted = false;
	m_bLocked = false;
	m_pfnFSCallback = NULL;
	m_pCallbackParam = 0;
}

//////////////////////////////////////////////////////////////////////
CAGSym::~CAGSym()
{
	m_pfnFSCallback = NULL;
	m_pCallbackParam = 0;
}

//////////////////////////////////////////////////////////////////////
bool CAGSym::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	// The SymType is read by the layer
	BYTE cFlags = 0;
	BYTE cUnused = 0;

	pInput->Read(&cFlags, sizeof(cFlags));
	pInput->Read(&m_nID, sizeof(m_nID)); 
	pInput->Read(&cUnused, sizeof(cUnused)); // Reserved for future
	pInput->Read(&m_Matrix, sizeof(m_Matrix));

	m_bHidden = !!(cFlags & SYM_HIDDEN);
	m_bLocked = !!(cFlags & SYM_LOCKED);

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAGSym::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	BYTE cType = (BYTE)m_SymType;
	BYTE cFlags = 0;
	cFlags |= (m_bHidden ? SYM_HIDDEN : 0);
	cFlags |= (m_bLocked ? SYM_LOCKED : 0);
	BYTE cUnused = 0;

	pOutput->Write(&cType, sizeof(cType));
	pOutput->Write(&cFlags, sizeof(cFlags));
	pOutput->Write(&m_nID, sizeof(m_nID)); 
	pOutput->Write(&cUnused, sizeof(cUnused)); // Reserved for future
	pOutput->Write(&m_Matrix, sizeof(m_Matrix));
	return true;
}

//////////////////////////////////////////////////////////////////////
void CAGSym::RegisterFileSpecsCallback(FNFILESPECSCALLBACK fnFileSpecsCallback, LPARAM lParam)
{
	if (!fnFileSpecsCallback)
	{
		m_pfnFSCallback = NULL;
		return;
	}

	m_pfnFSCallback = fnFileSpecsCallback;
	m_pCallbackParam = lParam;
}

//////////////////////////////////////////////////////////////////////
void CAGSym::UnregisterFileSpecsCallback()
{
	m_pfnFSCallback = NULL;
	m_pCallbackParam = 0;
}

//////////////////////////////////////////////////////////////////////
void CAGSym::GetXMLData(double& xpos, double& ypos, double& width, double& ht, double& fAngle)
{
	RECT DestRect = GetDestRect();
	xpos = DINCHES(DestRect.left);
	ypos = DINCHES(DestRect.top);
	width = DINCHES(WIDTH(DestRect));
	ht = DINCHES(HEIGHT(DestRect));
	fAngle = m_Matrix.GetAngle();
}

//////////////////////////////////////////////////////////////////////
void CAGSym::GetXMLData(double& cx, double& cy, bool& bRotated)
{
	cx = m_Matrix.m_cx / APP_RESOLUTION;
	cy = m_Matrix.m_cy / APP_RESOLUTION;
	bRotated = m_Matrix.IsRotated();
}

//////////////////////////////////////////////////////////////////////
void CAGSym::DuplicateTo(CAGSym* pSym) 
{
	if (!pSym)
		return;

	pSym->m_SymType = m_SymType;
	pSym->m_nID = ID_NONE;
	pSym->m_bHidden = m_bHidden;
	pSym->m_bDeleted = m_bDeleted;
	pSym->m_Matrix = m_Matrix;
	pSym->m_bLocked = m_bLocked;

}

//////////////////////////////////////////////////////////////////////
CAGSymImage::CAGSymImage(bool bOldReader) : CAGSym(ST_IMAGE)
{
	Init();

	m_bOldReader = bOldReader;
}

//////////////////////////////////////////////////////////////////////
CAGSymImage::CAGSymImage(const BITMAPINFOHEADER* pDIB, const RECT& MaxBounds) : CAGSym(ST_IMAGE)
{
	Init();

	m_strSourceType = "DIB";

	LoadDIB(pDIB, false/*bTakeOverMemory*/, &MaxBounds);
}

//////////////////////////////////////////////////////////////////////
CAGSymImage::CAGSymImage(HGLOBAL hMemory, const RECT& MaxBounds) : CAGSym(ST_IMAGE)
{
	Init();

	// Build the symbol's DIB
	CImage Image(hMemory);
	BITMAPINFOHEADER* pDIB = Image.GetDibPtr();
	if (pDIB)
		LoadDIB(pDIB, false/*bTakeOverMemory*/, &MaxBounds);

	// The source file data will be archived to disk so we can recall it when we need it
	SaveSourceData(hMemory);
}

//////////////////////////////////////////////////////////////////////
CAGSymImage::CAGSymImage(CAGSymImage* pImageSym, int nQuality) : CAGSym(ST_IMAGE)
{
	Init();

	m_Matrix = pImageSym->m_Matrix;
	m_DestRect = pImageSym->m_DestRect;
	m_CropRect = pImageSym->m_CropRect;
	m_TransparentColor = pImageSym->m_TransparentColor;
	m_LastHitColor = pImageSym->m_LastHitColor;
	m_cOrientation = pImageSym->m_cOrientation;

	BITMAPINFOHEADER* pDIBOrig = pImageSym->GetDib();
	if (!pDIBOrig)
		return;

	CString strSourceFile = RandomFileName();
	if (ConvertDibToJpg(pDIBOrig, nQuality, strSourceFile))
	{
		// Read in the source file data
		m_strSourceFile = strSourceFile;
		m_strSourceType = "JPG";
		HGLOBAL hMemory = RestoreSourceData();
		if (hMemory)
		{
			// Build the symbol's DIB
			CImage Image(hMemory);
			BITMAPINFOHEADER* pDIB = Image.GetDibPtr();
			if (pDIB)
				LoadDIB(pDIB, false/*bTakeOverMemory*/, NULL/*pPageRect*/);

			::GlobalFree(hMemory);
		}
	}
}

//////////////////////////////////////////////////////////////////////
CAGSymImage::~CAGSymImage()
{
	FreeDIB();

	// Delete any archived file data
	if (!m_strSourceFile.IsEmpty())
		::DeleteFile(m_strSourceFile);
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::Init()
{
	::SetRectEmpty(&m_DestRect);
	::SetRectEmpty(&m_CropRect);
	m_TransparentColor = CLR_NONE;
	m_LastHitColor = CLR_NONE;
	m_cOrientation = 0;
	m_strSourceFile.Empty();
	m_strSourceType.Empty();
	m_dwCompressedSize = 0;
	m_bLocked = false;
	m_bOldReader = false;
	m_bCoverDraw = false;
	m_pDibForEdit = NULL;
	m_pDibForDraw = NULL;
	m_fDibAngle = 0;
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::FreeDIB()
{
	FreeDibForDraw();

	if (m_pDibForEdit)
	{
		DibFree(m_pDibForEdit);
		m_pDibForEdit = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::WriteFYSXml(FILE* output, int itabs)
{
	CString strTransparentColor;
	strTransparentColor.Format("FF%02x%02x%02x", GetRValue(m_TransparentColor), 
							GetGValue(m_TransparentColor), GetBValue(m_TransparentColor));
	strTransparentColor.MakeUpper();

	CString strTabs;
	for (int x=0; x<itabs; x++)
		strTabs += "\t";

	// Start with the final resting place of the image
	// This only works because images can only be rotated in 90 degree increments
	RECT DestRect = GetDestRectTransformed();

	// Compute what changes have been made to the image orientation
	int iRotateDirection = 0;
	bool bFlipX = false;
	bool bFlipY = false;
	bool bWasReoriented = GetOrient(iRotateDirection, bFlipX, bFlipY);
	
	// Compute the rectangle prior to any orientation changes, as well as the rotation angle
	double fAngle = 0;
	if (iRotateDirection)
	{
		// The questions is whether you would rather rotate the image 180 degrees, or do 2 flips; flipping is faster that rotation
		// A double flip is the same as a 180 degree rotation
		if (bFlipX && bFlipY)
		{
			iRotateDirection = -iRotateDirection;
			bFlipX = false;
			bFlipY = false;
		}

		fAngle = (iRotateDirection < 0 ? -90/*CCW*/ : 90/*CW*/);

		// Un-rotate the rectangle as if it was rotated around the left-top corner
		// This is because FYS is expecting to use the angle to rotate around the top-left
		CAGMatrix Matrix;
		double fUndoAngle = -fAngle;
		if (fUndoAngle > 0)
			Matrix.Rotate(fUndoAngle, fUndoAngle, DestRect.left, DestRect.bottom);
		else
			Matrix.Rotate(fUndoAngle, fUndoAngle, DestRect.right, DestRect.top);
		Matrix.Transform(DestRect);
	}

	double fWidth = DINCHES(WIDTH(DestRect));
	double fHeight = DINCHES(HEIGHT(DestRect));
	double fXPos = DINCHES(DestRect.left);
	double fYPos = DINCHES(DestRect.top);

	DWORD dwSrcSize = 0;
	HGLOBAL hMemory = RestoreSourceData();
	if (hMemory)
	{
		dwSrcSize = ::GlobalSize(hMemory);
		::GlobalFree(hMemory);
	}
					
	// If no source data was restored...
	if (dwSrcSize <= 0)
		dwSrcSize = sizeof(BITMAPFILEHEADER) + DibSize(m_pDibForEdit);

	CString strImageFile = RandomFileName("IMG", CString('.') + m_strSourceType);
	strImageFile = strImageFile.Mid(strImageFile.ReverseFind('\\')+1);

	CString strOptional;
	if (strTransparentColor != "FFFFFFFF")
		strOptional += String("TransparentColor='%s'", strTransparentColor);

	CString strFlipXY;
	strFlipXY.Format("FlipX='%s' FlipY='%s'", bFlipX?"true":"false", bFlipY?"true":"false");

	fprintf(output, "%s<ImageContent FileName='%s' FileSize='%d' Content-Type='%s' %s XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G' %s />\r\n", 
			strTabs, strImageFile, dwSrcSize, m_strSourceType, strOptional, fXPos, fYPos, fWidth, fHeight, fAngle, strFlipXY);

	if (m_pfnFSCallback)
	{
		FileStruct* pfs = new FileStruct();
		pfs->Name = strImageFile.Left(strImageFile.ReverseFind('.'));
		pfs->FileName = strImageFile;
		pfs->Type = "Image";
		pfs->dwSize = dwSrcSize;
		m_pfnFSCallback(pfs, m_pCallbackParam);
	}
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::WriteXmlRects(FILE* output, SIZE& PageSize, int itabs)
{
	RECT PageRect;
	PageRect.left = 0;
	PageRect.top = 0;
	PageRect.right = PageSize.cx;
	PageRect.bottom = PageSize.cy;

	RECT DestRect = m_DestRect;
	RECT RectL = {PageRect.left, DestRect.top, DestRect.left, DestRect.bottom};
	RECT RectT = {PageRect.left, PageRect.top, PageRect.right, DestRect.top};
	RECT RectR = {DestRect.right, DestRect.top, PageRect.right, DestRect.bottom};
	RECT RectB = {PageRect.left, DestRect.bottom, PageRect.right, PageRect.bottom};

	CString strTabs;
	for (int x=0; x<itabs; x++)
		strTabs += "\t";

	double fcx;
	double fcy;
	bool bRotated;
	GetXMLData(fcx, fcy, bRotated);

	COLORREF c = RGB(255,255,255);
	CString strFillColor;
	strFillColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
	strFillColor.MakeUpper();

	// do this for each rect
	for (int i=0; i<4; i++)
	{
		RECT Rect;
		if (i==0)
			Rect = RectL;
		else if (i==1)
			Rect = RectT;
		else if (i==2)
			Rect = RectR;
		else
			Rect = RectB;

		double fXPos = DINCHES(Rect.left);
		double fYPos = DINCHES(Rect.top);
		double fWidth = DINCHES(Rect.right-Rect.left);
		double fHeight = DINCHES(Rect.bottom-Rect.top);

		fprintf(output, "%s<ShapeContent ShapeName='Rectangle' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='0'>\r\n",
						strTabs, fXPos, fYPos, fWidth, fHeight);
			fprintf(output, "%s<Transformation X_mx='%0.5G' X_my='%0.5G' X_d='%0.5G' Y_mx='%0.5G' Y_my='%0.5G' Y_d='%0.5G' />\r\n",
					strTabs, m_Matrix.m_ax, m_Matrix.m_bx, fcx, m_Matrix.m_ay, m_Matrix.m_by, fcy);
			fprintf(output, "%s<Fill Type='Solid' StartColorARGB='%s' EndColorARGB='%s' />\r\n", 
					strTabs, strFillColor, strFillColor);
			fprintf(output, "%s<Outline Type='Solid' ColorARGB='%s' Thickness='0' ThicknessUnit='Point' />\r\n", 
					strTabs, strFillColor);
		fprintf(output, "</ShapeContent>\r\n");
	}
}

//////////////////////////////////////////////////////////////////////
CAGSym* CAGSymImage::Duplicate()
{
	return Duplicate(true/*bCopyImageData*/);
}

//////////////////////////////////////////////////////////////////////
CAGSym* CAGSymImage::Duplicate(bool bCopyImageData)
{
	CAGSymImage* pSym = new CAGSymImage(!!m_bOldReader);
	if (!pSym)
		return NULL;

	CAGSym::DuplicateTo(pSym);
	pSym->m_DestRect = m_DestRect;
	pSym->m_CropRect = m_CropRect;
	pSym->m_TransparentColor = m_TransparentColor;
	pSym->m_LastHitColor = m_LastHitColor;
	pSym->m_cOrientation = m_cOrientation;
	pSym->m_strSourceFile = (bCopyImageData ? DuplicateSourceFile() : "");
	pSym->m_strSourceType = (bCopyImageData ? m_strSourceType : "");
	pSym->m_dwCompressedSize = m_dwCompressedSize;
	pSym->m_bLocked = m_bLocked;
	pSym->m_bOldReader = m_bOldReader;
	pSym->m_bCoverDraw = m_bCoverDraw;
	pSym->m_pDibForEdit = NULL;
	pSym->m_pDibForDraw = NULL;
	pSym->m_fDibAngle = 0;
	pSym->m_nID	= m_nID;
	pSym->m_szProductInfo = m_szProductInfo;

	if (bCopyImageData)
		pSym->LoadDIB(m_pDibForEdit, false/*bTakeOverMemory*/, NULL/*pPageRect*/);

	return dynamic_cast<CAGSym*>(pSym);
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::SetRotatedDibForDraw()
{
	double fAngle = m_Matrix.GetAngle();
	if (fAngle == m_fDibAngle)
		return;

	FreeDibForDraw();
	CDib DibSrc = m_pDibForEdit;
	m_pDibForDraw = DibSrc.Rotate(fAngle);
	m_fDibAngle = fAngle;
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::SetNormalDibForDraw()
{
	if (m_pDibForDraw && m_pDibForDraw != m_pDibForEdit)
		DibFree(m_pDibForDraw);

	m_pDibForDraw = m_pDibForEdit;
	m_fDibAngle = 0;
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::FreeDibForDraw()
{
	if (m_pDibForDraw && m_pDibForDraw != m_pDibForEdit)
		DibFree(m_pDibForDraw);

	m_pDibForDraw = NULL;;
	m_fDibAngle = 0;
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::Draw(CAGDC& dc, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return;

	// See if a rotated display DIB needs to be created, or if necessary, destroy an old one
	if (m_Matrix.IsRotated())
		SetRotatedDibForDraw();
	else
		SetNormalDibForDraw();
	
	BITMAPINFOHEADER* pDib = m_pDibForDraw;
	if (!pDib)
		return;

	CAGMatrix MatrixSaved = dc.SetSymbolMatrix(m_Matrix);
	CRect DestRect = GetDestRect();
	HRGN hClipRegion = dc.SaveClipRegion();
	dc.SetClipRectangle(&DestRect, true/*bReduceByOne*/);
	dc.StretchBlt(pDib, GetCropRect(), DestRect, m_TransparentColor);
	dc.RestoreClipRegion(hClipRegion);

	// If necessary, draw the 4 cover rectangles around the image
	if (m_bCoverDraw && dc.GetCoverAllowed())
	{
		CAGMatrix Matrix = dc.GetViewToDeviceMatrix();
		RECT PageRect = dc.GetViewRect();
		RECT ClipRect = PageRect;
		Matrix.Transform(ClipRect);

		HRGN hClipRegion = dc.SaveClipRegion();
		dc.SetClipRect(ClipRect, true/*bCombine*/);

		CRect DestRectT = GetDestRectTransformed();
		RECT RectL = {PageRect.left, DestRectT.top, DestRectT.left, DestRectT.bottom};
		RECT RectT = {PageRect.left, PageRect.top, PageRect.right, DestRectT.top};
		RECT RectR = {DestRectT.right, DestRectT.top, PageRect.right, DestRectT.bottom};
		RECT RectB = {PageRect.left, DestRectT.bottom, PageRect.right, PageRect.bottom};

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
bool CAGSymImage::HitTest(POINT Pt, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return false;

	RECT HitRect = {Pt.x-_HITTEST, Pt.y-_HITTEST, Pt.x+_HITTEST, Pt.y+_HITTEST};
	m_Matrix.Inverse().Transform(HitRect);
	m_Matrix.Inverse().Transform(Pt);

	RECT DestRect = m_DestRect;
	if (WIDTH(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, INCHES(1,16), 0);
	if (HEIGHT(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, 0, INCHES(1,16));

	bool bHit = !!::IntersectRect(&HitRect, &HitRect, &DestRect);
	if (!bHit)
		return false;

	BITMAPINFOHEADER* pDib = m_pDibForEdit;
	if (!pDib)
		return true;

	if (pDib->biBitCount != 8 && pDib->biBitCount != 24)
		return true;

	DestRect = m_DestRect;
	CAGMatrix Matrix;
	double xScale = (double)DibWidth(pDib) / WIDTH(DestRect);
	double yScale = (double)DibHeight(pDib) / HEIGHT(DestRect);
	Matrix.Translate(-DestRect.left, -DestRect.top);
	Matrix.Scale(xScale, yScale, 0, 0);
	Matrix.Transform(Pt);
	if (Pt.x < 0 || Pt.x >= (long)DibWidth(pDib) || Pt.y < 0 || Pt.y >= (long)DibHeight(pDib))
		return true;

	COLORREF Color = CLR_NONE;
	BYTE* pPixel = DibPtrXYExact(pDib, Pt.x, Pt.y);
	if (pDib->biBitCount == 24)
	{
		int b = pPixel[0];
		int g = pPixel[1];
		int r = pPixel[2];
		Color = RGB(r,g,b);
	}
	else
	{
		RGBQUAD* pRGB = DibColors(pDib);
		DWORD nColors = DibNumColors(pDib);
		BYTE cIndex = pPixel[0];
		if (pRGB && cIndex < nColors)
		{
			pRGB += cIndex;
			int b = pRGB->rgbBlue;
			int g = pRGB->rgbGreen;
			int r = pRGB->rgbRed;
			Color = RGB(r,g,b);
		}
	}

//j	if (RBUTTON)
		m_LastHitColor = Color;

	if ((m_TransparentColor != CLR_NONE) && (m_TransparentColor == Color))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymImage::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	bool bReturn = CAGSym::Read(pInput);

	if (m_bOldReader)
	{
		DWORD dwSize = 0;
		pInput->Read(&dwSize, sizeof(dwSize));
		BITMAPINFOHEADER* pDIB = NULL;
		if (dwSize > 0 && (pDIB = DibAlloc(dwSize)))
		{
			pInput->Read(pDIB, dwSize);
			m_strSourceType = DetermineSourceType((BYTE*)pDIB, dwSize);
			m_dwCompressedSize = 0; // Don't know yet
			LoadDIB(pDIB, true/*bTakeOverMemory*/, NULL/*pPageRect*/);

			pInput->Read(&m_DestRect, sizeof(m_DestRect));

			bool bHasTransparentColor = ((m_pDibForEdit->biClrImportant & 0xFF000000) == 0xFF000000);
			if (bHasTransparentColor)
				m_TransparentColor = m_pDibForEdit->biClrImportant & 0x00FFFFFF;
			m_pDibForEdit->biClrImportant = 0;
		}
	}
	else
	{
		BYTE cFlags = 0;
		BYTE cUnused = 0;
		DWORD dwSize = 0;

		pInput->Read(&m_DestRect, sizeof(m_DestRect));
		pInput->Read(&m_cOrientation, sizeof(m_cOrientation));
		pInput->Read(&cFlags, sizeof(cFlags));
		pInput->Read(&m_nID, sizeof(m_nID)); 
		pInput->Read(&cUnused, sizeof(cUnused)); // Reserved for future
		pInput->Read(&m_TransparentColor, sizeof(m_TransparentColor));
		pInput->Read(&dwSize, sizeof(dwSize));

		m_bLocked = !!(cFlags & SYMIMAGE_LOCKED);
		
		if (cFlags & SYMIMAGE_DIB_WRITTEN)
		{
			BITMAPINFOHEADER* pDIB = DibAlloc(dwSize);
			if (pDIB)
			{
				pInput->Read(pDIB, dwSize);
				m_strSourceType = "DIB";
				m_dwCompressedSize = 0; // Don't know yet

				CDib DibSrc = pDIB;
				BITMAPINFOHEADER* pNewDIB = NULL;
				if (m_cOrientation && (pNewDIB = DibSrc.OrientRestore(m_cOrientation)))
				{
					LoadDIB(pNewDIB, true/*bTakeOverMemory*/, NULL/*pPageRect*/);
					DibFree(pDIB);
				}
				else
					LoadDIB(pDIB, true/*bTakeOverMemory*/, NULL/*pPageRect*/);
			}
		}
		else
		{
			HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
			if (hMemory)
			{
				void* pMemory = ::GlobalLock(hMemory);
				pInput->Read(pMemory, dwSize);
				m_strSourceType = DetermineSourceType((BYTE*)pMemory, dwSize);
				m_dwCompressedSize = dwSize;
				::GlobalUnlock(hMemory);

				// Build the symbol's DIB
				CImage Image(hMemory);
				BITMAPINFOHEADER* pDIB = Image.GetDibPtr();
				if (pDIB)
				{
					CDib DibSrc = pDIB;
					BITMAPINFOHEADER* pNewDIB = NULL;
					if (m_cOrientation && (pNewDIB = DibSrc.OrientRestore(m_cOrientation)))
						LoadDIB(pNewDIB, true/*bTakeOverMemory*/, NULL/*pPageRect*/);
					else
						LoadDIB(pDIB, false/*bTakeOverMemory*/, NULL/*pPageRect*/);
				}

				// The source file data will be archived to disk so we can recall it when we need it
				SaveSourceData(hMemory);
				::GlobalFree(hMemory);
			}
		}

		if (cFlags & SYMIMAGE_DIB_WRITTEN)
			pInput->Read(&m_CropRect, sizeof(m_CropRect));
	}

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymImage::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	void* pMemory = NULL;
	DWORD dwSize = 0;
	BYTE cFlags = 0;
	cFlags |= (!::IsRectEmpty(&m_CropRect) ? SYMIMAGE_DIB_WRITTEN : 0);
	cFlags |= (m_bLocked ? SYMIMAGE_LOCKED : 0);

	// Try to read in the archived file data
	HGLOBAL hMemory = RestoreSourceData();
	if (hMemory)
	{
		pMemory = ::GlobalLock(hMemory);
		dwSize = ::GlobalSize(hMemory);
	}
	
	// If no source data was restored...
	if (!pMemory || dwSize <= 0)
	{
		pMemory = (void*)m_pDibForEdit;
		dwSize = DibSize(m_pDibForEdit);
		cFlags |= SYMIMAGE_DIB_WRITTEN;
	}
	else
	{
		CString strSourceType = DetermineSourceType((BYTE*)pMemory, dwSize);
		if (strSourceType == "DIB")
			cFlags |= SYMIMAGE_DIB_WRITTEN;
	}

	BYTE cUnused = 0;

	bool bReturn = CAGSym::Write(pOutput);
	pOutput->Write(&m_DestRect, sizeof(m_DestRect));
	pOutput->Write(&m_cOrientation, sizeof(m_cOrientation));
	pOutput->Write(&cFlags, sizeof(cFlags));
	pOutput->Write(&m_nID, sizeof(m_nID)); 
	pOutput->Write(&cUnused, sizeof(cUnused)); // Reserved for future
	pOutput->Write(&m_TransparentColor, sizeof(m_TransparentColor));
	pOutput->Write(&dwSize, sizeof(dwSize));
	pOutput->Write(pMemory, dwSize);
	
	if (cFlags & SYMIMAGE_DIB_WRITTEN)
		pOutput->Write(&m_CropRect, sizeof(m_CropRect));

	if (hMemory)
	{
		::GlobalUnlock(hMemory);
		::GlobalFree(hMemory);
	}

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::GetDestInflateSize(SIZE& Size)
{
	Size.cx = 0;
	Size.cy = 0;
}

//////////////////////////////////////////////////////////////////////
HANDLE CAGSymImage::GetClipboardData(bool bCut)
{
	BITMAPINFOHEADER* pDib = m_pDibForEdit;
	if (!pDib)
		return NULL;

	HANDLE hMemory = ::GlobalAlloc(GMEM_MOVEABLE, DibSize(pDib));
	if (!hMemory)
		return NULL;

	BITMAPINFOHEADER* pDIB = (BITMAPINFOHEADER*)::GlobalLock(hMemory);
	*pDIB = *pDib;
	if (DibNumColors(pDib))
		memcpy((void*)DibColors(pDIB), (void*)DibColors(pDib), DibPaletteSize(pDib));

	memcpy(DibPtr(pDIB), DibPtr(pDib), DibSizeImage(pDib));
	::GlobalUnlock(hMemory);
	return hMemory;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymImage::LoadDIB(const BITMAPINFOHEADER* pDIB, bool bTakeOverMemory, const RECT* pPageRect)
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
		SetDib((BITMAPINFOHEADER*)pDIB);
	else
		SetDib(DibCopy(pDIB));

	BITMAPINFOHEADER* pDib = m_pDibForEdit;
	if (!pDib)
		return false;

	if (!pDib->biSizeImage)
		DibSizeImageRecompute(pDib);

	if (!pDib->biClrUsed)
		pDib->biClrUsed = DibNumColors(pDib);

	// See if the dib has a transparent color
	bool bHasTransparentColor = ((pDib->biClrImportant & 0xFF000000) == 0xFF000000);
	if (bHasTransparentColor && m_TransparentColor == CLR_NONE)
		m_TransparentColor = (pDib->biClrImportant & 0x00FFFFFF);
	pDib->biClrImportant = 0;

	// Fit the image rectangle within the bounding rectangle
	int iWidth = DibWidth(pDIB);
	int iHeight = DibHeight(pDIB);
	int iResolution = DibResolutionX(pDIB);

	if (pPageRect)
	{
		CRect PageRect = *pPageRect;

		// Make sure the image does not hang off the page
		if (INCHES(iWidth, iResolution) > PageRect.Width()) 
			iResolution = INCHES(iWidth, PageRect.Width());
		if (INCHES(iHeight, iResolution) > PageRect.Height())
			iResolution = INCHES(iHeight, PageRect.Height());

		// Make sure the image does not hang off the page
		if (INCHES(iWidth, iResolution) >= PageRect.Width())
			iResolution = INCHES(iWidth, PageRect.Width());
		if (INCHES(iHeight, iResolution) >= PageRect.Height())
			iResolution = INCHES(iHeight, PageRect.Height());

		// Compute the image size in page units
		iWidth = INCHES(iWidth, iResolution);
		iHeight = INCHES(iHeight, iResolution);

		// Setup the DestRect to be centered within the page rectangle
		int cx = (PageRect.right + PageRect.left - iWidth) / 2;
		int cy = (PageRect.bottom + PageRect.top - iHeight) / 2;
		m_DestRect.SetRect(0, 0, iWidth, iHeight);
		m_DestRect.OffsetRect(cx, cy);
		m_CropRect.SetRectEmpty();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void CAGSymImage::SaveSourceData(HGLOBAL hMemory)
{
	if (!hMemory)
		return;

	DWORD dwSize = ::GlobalSize(hMemory);
	void* pMemory = ::GlobalLock(hMemory);

	if (m_strSourceFile.IsEmpty())
		m_strSourceFile = RandomFileName();

	// Save the source file data
	HANDLE hFile = ::CreateFile(m_strSourceFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		m_strSourceFile.Empty();
	else
	{
		DWORD dwBytesWritten = 0;
		bool bFailed = !::WriteFile(hFile, pMemory, dwSize, &dwBytesWritten, NULL);
		::CloseHandle(hFile);
		if (bFailed)
		{
			::DeleteFile(m_strSourceFile);
			m_strSourceFile.Empty();
		}
	}

	m_strSourceType = DetermineSourceType((BYTE*)pMemory, dwSize);
	m_dwCompressedSize = dwSize;
	::GlobalUnlock(hMemory);
}

//////////////////////////////////////////////////////////////////////
HGLOBAL CAGSymImage::RestoreSourceData()
{
	if (m_strSourceFile.IsEmpty())
		return NULL;

	// Restore the source file data
	HANDLE hFile = ::CreateFile(m_strSourceFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	HGLOBAL hMemory = NULL;
	DWORD dwSize = ::GetFileSize(hFile, NULL);
	if (dwSize > 0 && (hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize)))
	{
		void* pMemory = ::GlobalLock(hMemory);
		DWORD dwBytesRead = 0;
		bool bFailed = !::ReadFile(hFile, pMemory, dwSize, &dwBytesRead, NULL);
		m_strSourceType = DetermineSourceType((BYTE*)pMemory, dwSize);
		m_dwCompressedSize = dwSize;
		::GlobalUnlock(hMemory);
		if (bFailed)
		{
			::GlobalFree(hMemory);
			hMemory = NULL;
		}
	}

	::CloseHandle(hFile);
	return hMemory;
}

//////////////////////////////////////////////////////////////////////
CString CAGSymImage::DuplicateSourceFile()
{
	if (m_strSourceFile.IsEmpty())
		return "";

	CString strNewSourceFile = RandomFileName();
	if (!::CopyFile(m_strSourceFile, strNewSourceFile, false/*bFailIfExists*/))
		return "";

	return strNewSourceFile;
}

//////////////////////////////////////////////////////////////////////
LPCSTR CAGSymImage::DetermineSourceType(BYTE* pMemory, DWORD dwSize)
{
	if (!pMemory)
		return "";
	if (dwSize < 10)
		return "";
	
	HENHMETAFILE hEMF = NULL;
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
CAGSymText::CAGSymText(SYMTYPE SymType) : CAGSym(SymType)
{
}


//////////////////////////////////////////////////////////////////////
CAGSymText::~CAGSymText()
{
}

//////////////////////////////////////////////////////////////////////
CAGSym* CAGSymText::Duplicate()
{
	CAGSymText* pSym = new CAGSymText(m_SymType);
	if (!pSym)
		return NULL;

	CAGSym::DuplicateTo(pSym);
	pSym->Create(m_DestRect); 
	pSym->CopyStream(m_pColumn);
	pSym->SetVertJust(GetVertJust());

	return dynamic_cast<CAGSym*>(pSym);
}

//////////////////////////////////////////////////////////////////////
void CAGSymText::Swap(CAGSymText* pSym)
{
	if (!pSym)
		return;

	CRect DestRect = pSym->GetDestRect();
	pSym->SetDestRect(m_DestRect);
	SetDestRect(DestRect);
	pSym->SwapStreams(m_pColumn);
}

//////////////////////////////////////////////////////////////////////
void CAGSymText::Draw(CAGDC& dc, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return;

	CAGMatrix MatrixSaved = dc.SetSymbolMatrix(m_Matrix);

	DrawAll(&dc);
	dc.SetSymbolMatrix(MatrixSaved);
}

//////////////////////////////////////////////////////////////////////
bool CAGSymText::HitTest(POINT Pt, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return false;

	RECT HitRect = {Pt.x-(2*_OFFSET), Pt.y-(2*_OFFSET), Pt.x+(2*_OFFSET), Pt.y+(2*_OFFSET)};
	m_Matrix.Inverse().Transform(HitRect);

	RECT DestRect = m_DestRect;
	if (WIDTH(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, INCHES(1,16), 0);
	if (HEIGHT(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, 0, INCHES(1,16));

	bool bHit = !!::IntersectRect(&HitRect, &HitRect, &DestRect);
	return bHit;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymText::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	bool bReturn = CAGSym::Read(pInput);
	ReadColumn(pInput);

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymText::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	bool bReturn = CAGSym::Write(pOutput);
	WriteColumn(pOutput);

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
void CAGSymText::WriteXmlFillType(FILE* output, CAGSpec& ExamineSpec, int iTabs)
{
	CString strTabs;

	for (int x=0; x<iTabs; x++)
		strTabs+= "\t";

	LPCSTR szFillTypes[] = {"Hollow", "Solid", "Gradient"/*right*/, "Gradient"/*down*/, "Radial"/*center*/, "Radial"/*corner*/ };
	FillType ft = ExamineSpec.m_FillType;
	if ((ft < FT_None) || (ft > FT_SweepDown)) ft = FT_Solid;

	CString strFillType = String("'%s'", szFillTypes[ft]);
	if (ft == FT_SweepDown)
		strFillType += " GradientAngle='90'";
	if (ft == FT_SweepRight)
		strFillType += " GradientAngle='0'";

	COLORREF c = ExamineSpec.m_FillColor;
	CString strFillColor;
	strFillColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
	strFillColor.MakeUpper();

	c = ExamineSpec.m_FillColor2;
	CString strFillColor2;
	strFillColor2.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
	strFillColor2.MakeUpper();

	fprintf(output, "%s<Fill Type=%s StartColorARGB='%s' EndColorARGB='%s'/>\r\n",
				strTabs, strFillType, strFillColor, strFillColor2);
}

//////////////////////////////////////////////////////////////////////
void CAGSymText::WriteXmlOutline(FILE* output, CAGSpec& ExamineSpec, int iTabs)
{
	CString strTabs;

	for (int x=0; x<iTabs; x++)
		strTabs+= "\t";

	DWORD dwOutlineThickness = ExamineSpec.m_LineWidth;
	LPCSTR strOutlineType = (dwOutlineThickness == LT_None ? "None" : "Solid");
	LPCSTR strOutlineThicknessUnit = (dwOutlineThickness == LT_Hairline ? "Pixel" : "Point");
	if (dwOutlineThickness == LT_Hairline)
		dwOutlineThickness = 1;

	COLORREF c = ExamineSpec.m_LineColor;
	CString strOutlineColor;
	strOutlineColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
	strOutlineColor.MakeUpper();

	fprintf(output, "%s<Outline Type='%s' ColorARGB='%s' Thickness='%0.5G' ThicknessUnit='%s' />\r\n", 
				strTabs, strOutlineType, strOutlineColor, POINTSIZE(dwOutlineThickness), strOutlineThicknessUnit);

}

//////////////////////////////////////////////////////////////////////
void CAGSymText::WriteXmlFontSpecs(FILE* output, SpecArray& cSpecArray, int iTabs)
{
	CString strTabs;

	for (int x=0; x<iTabs; x++)
		strTabs+= "\t";

	if (cSpecArray.GetSize() <= 0)
	{
		fprintf(output, "%s<RichTextSegment FontName='%s' FontPointSize='%0.5G' HorizontalJustification='%s' Bold='%s' Italic='%s' Underline='%s'>\r\n", 
					strTabs, "CAC One Seventy", 15.0, "Left", "false", "false", "false");

		fprintf(output, "%s<Fill Type='%s' StartColorARGB='%s' EndColorARGB='%s'/>\r\n",
					strTabs, "Solid", "FF000000", "FF000000");
		fprintf(output, "%s<Outline Type='%s' ColorARGB='%s' Thickness='%0.5G' ThicknessUnit='%s' />\r\n", 
					strTabs, "None", "FF000000", 0.0, "Point");

		fprintf(output, "%s\t<TextData>\r\n", strTabs);
		fprintf(output, "%s\t\t<![CDATA[]]>\r\n", strTabs);
		fprintf(output, "%s\t</TextData>\r\n", strTabs);

		fprintf(output, "%s</RichTextSegment>\r\n", strTabs);
		return;
	}

	for (int i=0; i<cSpecArray.GetSize(); i++)
	{
		CString strText = cSpecArray[i].strTxt;
		CAGSpec ExamineSpec = *cSpecArray[i].pSpec;
		CString strFontName = ExamineSpec.m_Font.lfFaceName;
		CString strBold = (ExamineSpec.m_Font.lfWeight != FW_NORMAL ? "true" : "false");
		CString strItalic = (ExamineSpec.m_Font.lfItalic ? "true" : "false");
		CString strUnderline = (ExamineSpec.m_Font.lfUnderline ? "true" : "false");
		int iFontPointSize = ExamineSpec.m_Font.lfHeight;

		LPCSTR szHorzJustTypes[] = {"Center", "Left", "Right", "Justified", "Illegal", "Illegal", "Illegal", "Illegal" };
		eTSJust hj = ExamineSpec.m_HorzJust;
		//if ((hj != eRagCentered) || (hj != eFlushLeft) || (hj != eFlushRight)) hj = eFlushLeft;
		CString strHorzJust = szHorzJustTypes[(hj&7)];

		fprintf(output, "%s<RichTextSegment FontName='%s' FontPointSize='%0.5G' HorizontalJustification='%s' Bold='%s' Italic='%s' Underline='%s'>\r\n", 
					strTabs, strFontName, POINTSIZE(iFontPointSize), strHorzJust, strBold, strItalic, strUnderline);

		WriteXmlFillType(output, ExamineSpec, iTabs+1);
		WriteXmlOutline(output, ExamineSpec, iTabs+1);
		fprintf(output, "%s\t<TextData>\r\n", strTabs);
		strText.Replace("\n", "\r\n");
		fprintf(output, "%s\t\t<![CDATA[%s]]>\r\n", strTabs, strText);
		fprintf(output, "%s\t</TextData>\r\n", strTabs);

		fprintf(output, "%s</RichTextSegment>\r\n", strTabs);

		// Check for AG fonts
		if (CFontList::IsOurFont(ExamineSpec.m_Font.lfFaceName))
			continue;

		// Check for restricted fonts
		CString strFontFile;
		CTrueType::GetFileNameFromDisplayName(ExamineSpec.m_Font.lfFaceName, strFontFile);
		CString strFontPath = CTrueType::GetPathFromFileName(strFontFile);

		TTF_PROPERTIES FontProps;
		if (!CTrueType::GetProperties(strFontPath, &FontProps) || FontProps.enumEmbed == TTF_Embed_None)
		{
			//CMessageBox::Message(String("Sorry, you cannot embed %s", ExamineSpec.m_Font.lfFaceName));
			continue;
		}

		if (m_pfnFSCallback)
		{
			FileStruct* pfs = new FileStruct();
			pfs->Name = ExamineSpec.m_Font.lfFaceName;
			pfs->Type = "Font";
			m_pfnFSCallback(pfs, m_pCallbackParam);
		}

	}
}

//////////////////////////////////////////////////////////////////////
void CAGSymText::WriteFYSXml(FILE* output, int itabs)
{
	CString strTabs;
	double fXPos, fYPos;
	double fWidth, fHeight;
	double fcx, fcy;
	double fAngle;
	bool bRotated;
	GetXMLData(fXPos, fYPos, fWidth, fHeight, fAngle);
	GetXMLData(fcx, fcy, bRotated);

	for (int x=0; x<itabs; x++)
		strTabs+= "\t";

	SaveSelection();
	SelectAll(false);
	SpecArray cSpecArray;
	GetSpecStructArray(cSpecArray);
	RestoreSelection();
	ShowSelection(true);

	LPCSTR szVertJustTypes[] = {"Illegal", "Middle", "Top", "Bottom", "Justified", "Justify" };
	eVertJust vj = GetVertJust();
	CString strVertJust = szVertJustTypes[vj];

	fprintf(output, "%s<RichTextContent XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G' VerticalJustification='%s'>\r\n",
				strTabs, fXPos, fYPos, fWidth, fHeight, fAngle, strVertJust);

	fprintf(output, "%s\t<Transformation X_mx='%0.5G' X_my='%0.5G' X_d='%0.5G' Y_mx='%0.5G' Y_my='%0.5G' Y_d='%0.5G' />\r\n",
				strTabs, m_Matrix.m_ax, m_Matrix.m_bx, fcx, m_Matrix.m_ay, m_Matrix.m_by, fcy);
	WriteXmlFontSpecs(output, cSpecArray, itabs+1);

	fprintf(output, "%s</RichTextContent>\r\n", strTabs);

	for (int i=0; i<cSpecArray.GetSize(); i++)
	{
		CAGSpec* pSpec = cSpecArray[i].pSpec;
		delete pSpec;
	}
}

//////////////////////////////////////////////////////////////////////
void CAGSymText::GetDestInflateSize(SIZE& Size)
{
	Size.cx = 0;
	Size.cy = 0;
}

//////////////////////////////////////////////////////////////////////
CAGSymGraphic::CAGSymGraphic(SYMTYPE SymType) : CAGSym(SymType)
{
	m_LineWidth = LT_None;
	m_LineColor = RGB(0,0,0);

	m_FillType = FT_Solid;
	m_FillColor = RGB(0,0,0);
	m_FillColor2 = RGB(0,0,0);
	m_FillColor3 = RGB(0,0,0);

	m_ShadowType = ST_None;
	m_ShadowColor = RGB(0,0,0);
	m_ShadowOffset.x = 0;
	m_ShadowOffset.y = 0;
}

//////////////////////////////////////////////////////////////////////
CAGSymGraphic::~CAGSymGraphic()
{
}

//////////////////////////////////////////////////////////////////////
void CAGSymGraphic::DuplicateTo(CAGSymGraphic* pSym)
{
	if (!pSym)
		return;

	pSym->m_LineWidth	= m_LineWidth;
	pSym->m_LineColor	= m_LineColor;

	pSym->m_FillType	= m_FillType;
	pSym->m_FillColor	= m_FillColor;
	pSym->m_FillColor2	= m_FillColor2;
	pSym->m_FillColor3	= m_FillColor3;

	pSym->m_ShadowType	= m_ShadowType;
	pSym->m_ShadowColor	= m_ShadowColor;
	pSym->m_ShadowOffset= m_ShadowOffset;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymGraphic::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	pInput->Read(&m_LineColor, sizeof(m_LineColor));
	pInput->Read(&m_FillColor, sizeof(m_FillColor));
	pInput->Read(&m_FillColor2, sizeof(m_FillColor2));
	pInput->Read(&m_FillColor3, sizeof(m_FillColor3));
	pInput->Read(&m_ShadowColor, sizeof(m_ShadowColor));
	pInput->Read(&m_FillType, sizeof(m_FillType));
	pInput->Read(&m_ShadowType, sizeof(m_ShadowType));
	pInput->Read(&m_LineWidth, sizeof(m_LineWidth));
	pInput->Read(&m_ShadowOffset, sizeof(m_ShadowOffset));

	return true;
}

//////////////////////////////////////////////////////////////////////
void CAGSymGraphic::WriteFYSXml(FILE* output, int itabs)
{
	double fcx;
	double fcy;
	bool bRotated;
	GetXMLData(fcx, fcy, bRotated);

	CString strTabs;
	for (int x=0; x<itabs; x++)
		strTabs += "\t";

	fprintf(output, "%s<Transformation X_mx='%0.5G' X_my='%0.5G' X_d='%0.5G' Y_mx='%0.5G' Y_my='%0.5G' Y_d='%0.5G' />\r\n",
				strTabs, m_Matrix.m_ax, m_Matrix.m_bx, fcx, m_Matrix.m_ay, m_Matrix.m_by, fcy);

	LPCSTR szFillTypes[] = {"Hollow", "Solid", "Gradient"/*right*/, "Gradient"/*own*/, "Radial"/*center*/, "Radial"/*corner*/ };
	FillType ft = GetFillType();
	CString strFillType = szFillTypes[ft];
	if (ft == FT_SweepRight)
		strFillType + " GradientAngle='0'";
	if (ft == FT_SweepDown)
		strFillType + " GradientAngle='90'";

	COLORREF c = GetFillColor();
	CString strFillColor;
	strFillColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
	strFillColor.MakeUpper();

	c = GetFillColor2();
	CString strFillColor2;
	strFillColor2.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
	strFillColor2.MakeUpper();

	fprintf(output, "%s<Fill Type='%s' StartColorARGB='%s' EndColorARGB='%s' />\r\n", 
				strTabs, strFillType, strFillColor, strFillColor2);

	DWORD dwOutlineThickness = GetLineWidth();
	LPCSTR strOutlineType = (dwOutlineThickness == LT_None ? "None" : "Solid");
	LPCSTR strOutlineThicknessUnit = (dwOutlineThickness == LT_Hairline ? "Pixel" : "Point");
	if (dwOutlineThickness == LT_Hairline)
		dwOutlineThickness = 1;

	c = GetLineColor();
	CString strOutlineColor;
	strOutlineColor.Format("FF%02x%02x%02x", GetRValue(c), GetGValue(c), GetBValue(c));
	strOutlineColor.MakeUpper();
	fprintf(output, "%s<Outline Type='%s' ColorARGB='%s' Thickness='%0.5G' ThicknessUnit='%s' />\r\n", 
				strTabs, strOutlineType, strOutlineColor, POINTSIZE(dwOutlineThickness), strOutlineThicknessUnit);
}

//////////////////////////////////////////////////////////////////////
bool CAGSymGraphic::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	pOutput->Write(&m_LineColor, sizeof(m_LineColor));
	pOutput->Write(&m_FillColor, sizeof(m_FillColor));
	pOutput->Write(&m_FillColor2, sizeof(m_FillColor2));
	pOutput->Write(&m_FillColor3, sizeof(m_FillColor3));
	pOutput->Write(&m_ShadowColor, sizeof(m_ShadowColor));
	pOutput->Write(&m_FillType, sizeof(m_FillType));
	pOutput->Write(&m_ShadowType, sizeof(m_ShadowType));
	pOutput->Write(&m_LineWidth, sizeof(m_LineWidth));
	pOutput->Write(&m_ShadowOffset, sizeof(m_ShadowOffset));

	return true;
}

//////////////////////////////////////////////////////////////////////
void CAGSymGraphic::SetShapeName(LPCTSTR pShapeName)
{
	if (pShapeName && *pShapeName != '\0')
		m_strShapeName = pShapeName;
	else
		m_strShapeName = _T("");
}

//////////////////////////////////////////////////////////////////////
LPCTSTR CAGSymGraphic::GetShapeName()
{
	if (m_strShapeName == _T(""))
		return NULL;
	else
		return m_strShapeName;
}

//////////////////////////////////////////////////////////////////////
CAGSymRectangle::CAGSymRectangle(bool bReadAsObsoleteColorbox) : CAGSymGraphic(ST_RECTANGLE)
{
	m_bReadAsObsoleteColorbox = bReadAsObsoleteColorbox;
	m_strShapeName = "Rectangle";
}

//////////////////////////////////////////////////////////////////////
CAGSymRectangle::~CAGSymRectangle()
{
}

//////////////////////////////////////////////////////////////////////
CAGSym* CAGSymRectangle::Duplicate()
{
	CAGSymRectangle* pSym = new CAGSymRectangle;
	if (!pSym)
		return NULL;

	CAGSym::DuplicateTo(pSym);
	CAGSymGraphic::DuplicateTo(pSym);
	pSym->m_DestRect = m_DestRect;

	return dynamic_cast<CAGSym*>(pSym);
}

//////////////////////////////////////////////////////////////////////
void CAGSymRectangle::Draw(CAGDC& dc, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return;

	CAGMatrix MatrixSaved = dc.SetSymbolMatrix(m_Matrix);
	CRect DestRect = GetDestRect();
	HRGN hClipRegion = dc.SaveClipRegion();
	dc.SetClipRectangle(&DestRect);

	CAGMatrix Matrix = dc.GetSymbolToDeviceMatrix();
	CPoint pPoints[4];
	Matrix.TransformRectToPolygon(DestRect, pPoints);

	COLORREF LineColor = (m_LineWidth == LT_None ? CLR_NONE : m_LineColor);
	COLORREF FillColor = (m_FillType == FT_None ? CLR_NONE : m_FillColor);
	bool bInvisible = (LineColor == CLR_NONE && FillColor == CLR_NONE);
	if (bInvisible) // don't let it be invisible
		LineColor = RGB(0,0,0);

	if (m_FillType != FT_None)
	{
		if (m_FillType == FT_SweepRight || m_FillType == FT_SweepDown)
			dc.FillLinearGradient(pPoints, m_FillColor, m_FillColor2, (m_FillType == FT_SweepRight)); 
		else
		if  (m_FillType == FT_RadialCenter || m_FillType == FT_RadialCorner)
			dc.FillRadialGradient(pPoints, m_FillColor, m_FillColor2, (m_FillType == FT_RadialCenter));
		else
			dc.FillRectangle(pPoints, m_FillColor);
	}

	// Draw the outline
	if (m_LineWidth >= LT_Normal)
	{
		int LineWidth = dtoi(dc.GetViewToDeviceMatrix().TransformDistance(2*m_LineWidth+1));
		dc.DrawRectangle(pPoints, LineColor, LineWidth, CLR_NONE);
	}

	dc.RestoreClipRegion(hClipRegion);

	if (m_LineWidth == LT_Hairline || bInvisible)
		dc.DrawRectangle(pPoints, LineColor, LT_Hairline, CLR_NONE);

	dc.SetSymbolMatrix(MatrixSaved);
}

//////////////////////////////////////////////////////////////////////
bool CAGSymRectangle::HitTest(POINT Pt, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return false;

	RECT HitRect = {Pt.x-_HITTEST, Pt.y-_HITTEST, Pt.x+_HITTEST, Pt.y+_HITTEST};
	m_Matrix.Inverse().Transform(HitRect);
	m_Matrix.Inverse().Transform(Pt);

	RECT DestRect = m_DestRect;
	if (WIDTH(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, INCHES(1,16), 0);
	if (HEIGHT(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, 0, INCHES(1,16));

	bool bHit = !!::IntersectRect(&HitRect, &HitRect, &DestRect);
	if (!bHit)
		return false;

	if (m_FillType == FT_None)
	{
		RECT InternalRect = DestRect;
		int nMinWidth = INCHES(1,8);
		int nNewWidth = (m_LineWidth <= nMinWidth) ? nMinWidth : m_LineWidth;
		::InflateRect(&InternalRect, -nNewWidth, -nNewWidth);
		bHit &= (!::PtInRect(&InternalRect, Pt));
	}

	return bHit;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymRectangle::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	bool bReturn = CAGSym::Read(pInput);
	if (m_bReadAsObsoleteColorbox)
	{
		pInput->Read(&m_DestRect, sizeof(m_DestRect));
		pInput->Read(&m_FillColor, sizeof(m_FillColor));
	}
	else
	{
		bReturn = CAGSymGraphic::Read(pInput);
		pInput->Read(&m_DestRect, sizeof(m_DestRect));
	}

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymRectangle::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	bool bReturn = CAGSym::Write(pOutput);
	bReturn = CAGSymGraphic::Write(pOutput);
	pOutput->Write(&m_DestRect, sizeof(m_DestRect));

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
void CAGSymRectangle::WriteFYSXml(FILE* output, int itabs)
{
	CString strTabs;

	for (int x=0; x<itabs; x++)
		strTabs += "\t";

	double fXPos, fYPos;
	double fWidth, fHeight;
	double fAngle;

	GetXMLData(fXPos, fYPos, fWidth, fHeight, fAngle);
	fprintf(output, "%s<ShapeContent ShapeName='%s' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G' IsAdvanced='true'>\r\n", 
				strTabs, m_strShapeName, fXPos, fYPos, fWidth, fHeight, fAngle);

	CAGSymGraphic::WriteFYSXml(output, itabs+1);

	fprintf(output, "%s</ShapeContent>\r\n", strTabs);
}

//////////////////////////////////////////////////////////////////////
void CAGSymRectangle::GetDestInflateSize(SIZE& Size)
{
	Size.cx = 0;
	Size.cy = 0;
}

//////////////////////////////////////////////////////////////////////
CAGSymEllipse::CAGSymEllipse() : CAGSymGraphic(ST_ELLIPSE)
{
	m_strShapeName = "Circle/Ellipse";
}

//////////////////////////////////////////////////////////////////////
CAGSymEllipse::~CAGSymEllipse()
{
}

//////////////////////////////////////////////////////////////////////
CAGSym* CAGSymEllipse::Duplicate()
{
	CAGSymEllipse* pSym = new CAGSymEllipse;
	if (!pSym)
		return NULL;

	CAGSym::DuplicateTo(pSym);
	CAGSymGraphic::DuplicateTo(pSym);
	pSym->m_DestRect = m_DestRect;

	return dynamic_cast<CAGSym*>(pSym);
}

//////////////////////////////////////////////////////////////////////
void CAGSymEllipse::Draw(CAGDC& dc, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return;

	// Convert the ellipse to a series of 4 bezier curves
	RECT DestRect = m_DestRect;
	int xMid = (DestRect.left + DestRect.right) / 2;
	int yMid = (DestRect.top + DestRect.bottom) / 2;

	int dxLeft   = xMid - DestRect.left;
	int dxRight  = DestRect.right - xMid;
	int dyTop    = yMid - DestRect.top;
	int dyBottom = DestRect.bottom - yMid;

	const double k = 1.0 - 0.5522847498; // 1.0 - (4/3 * (sqrt(2) - 1)) OR  (4/3 * (1-cos(45°))/sin (45°))
	int dLeft   = dtoi(k * dxLeft);
	int dRight  = dtoi(k * dxRight);
	int dTop    = dtoi(k * dyTop);
	int dBottom = dtoi(k * dyBottom);

	CPoint pPoints[13];
	pPoints[0].x  = xMid;					// Starting point
	pPoints[0].y  = DestRect.top;
	pPoints[1].x  = DestRect.right - dRight;	// Upper/Right
	pPoints[1].y  = DestRect.top;
	pPoints[2].x  = DestRect.right;
	pPoints[2].y  = DestRect.top + dTop;
	pPoints[3].x  = DestRect.right;
	pPoints[3].y  = yMid;
	pPoints[4].x  = DestRect.right;			// Lower/Right
	pPoints[4].y  = DestRect.bottom - dBottom;
	pPoints[5].x  = DestRect.right - dRight;
	pPoints[5].y  = DestRect.bottom;
	pPoints[6].x  = xMid;
	pPoints[6].y  = DestRect.bottom;
	pPoints[7].x  = DestRect.left + dLeft;	// Lower/Left
	pPoints[7].y  = DestRect.bottom;
	pPoints[8].x  = DestRect.left;
	pPoints[8].y  = DestRect.bottom - dBottom;
	pPoints[9].x  = DestRect.left;
	pPoints[9].y  = yMid;
	pPoints[10].x = DestRect.left;			// Upper/Left
	pPoints[10].y = DestRect.top + dTop;
	pPoints[11].x = DestRect.left + dLeft;
	pPoints[11].y = DestRect.top;
	pPoints[12].x = xMid;
	pPoints[12].y = DestRect.top;

	CAGMatrix MatrixSaved = dc.SetSymbolMatrix(m_Matrix);
	CAGMatrix Matrix = dc.GetSymbolToDeviceMatrix();
	Matrix.Transform(pPoints, 13);

	COLORREF LineColor = (m_LineWidth == LT_None ? CLR_NONE : m_LineColor);
	COLORREF FillColor = (m_FillType == FT_None ? CLR_NONE : m_FillColor);
	bool bInvisible = (LineColor == CLR_NONE && FillColor == CLR_NONE);
	if (bInvisible) // don't let it be invisible
		LineColor = RGB(0,0,0);

	HRGN hClipRegion = dc.SaveClipRegion();
	dc.SetClipEllipse(pPoints);

	// Fill the interior
	if (m_FillType != FT_None)
	{
		RECT DestRect = m_DestRect;
		CPoint pt[4];
		Matrix.TransformRectToPolygon(DestRect, pt);

		if (m_FillType == FT_SweepRight || m_FillType == FT_SweepDown)
			dc.FillLinearGradient(pt, m_FillColor, m_FillColor2, (m_FillType == FT_SweepRight)); 
		else
		if  (m_FillType == FT_RadialCenter || m_FillType == FT_RadialCorner)
			dc.FillRadialGradient(pt, m_FillColor, m_FillColor2, (m_FillType == FT_RadialCenter));
		else
			dc.FillRectangle(pt, m_FillColor);
	}

	// Draw the outline
	if (m_LineWidth >= LT_Normal)
	{
		int LineWidth = dtoi(dc.GetViewToDeviceMatrix().TransformDistance(2*m_LineWidth+1));
		dc.DrawEllipse(pPoints, LineColor, LineWidth, CLR_NONE);
	}

	dc.RestoreClipRegion(hClipRegion);

	if (m_LineWidth == LT_Hairline || bInvisible)
		dc.DrawEllipse(pPoints, LineColor, LT_Hairline, CLR_NONE);

	dc.SetSymbolMatrix(MatrixSaved);
}

//////////////////////////////////////////////////////////////////////
bool CAGSymEllipse::HitTest(POINT Pt, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return false;

	RECT HitRect = {Pt.x-_HITTEST, Pt.y-_HITTEST, Pt.x+_HITTEST, Pt.y+_HITTEST};
	m_Matrix.Inverse().Transform(HitRect);
	m_Matrix.Inverse().Transform(Pt);

	RECT DestRect = m_DestRect;
	if (WIDTH(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, INCHES(1,16), 0);
	if (HEIGHT(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, 0, INCHES(1,16));

	bool bHit = !!::IntersectRect(&HitRect, &HitRect, &DestRect);
	if (!bHit)
		return false;

	HRGN hRegion = ::CreateEllipticRgn(DestRect.left, DestRect.top, DestRect.right, DestRect.bottom);
	bHit = !!::PtInRegion(hRegion, Pt.x, Pt.y);
	::DeleteObject(hRegion);

	if (m_FillType == FT_None)
	{
		RECT InternalRect = DestRect;
		int nMinWidth = INCHES(1,8);
		int nNewWidth = (m_LineWidth <= nMinWidth) ? nMinWidth : m_LineWidth;
		::InflateRect(&InternalRect, -nNewWidth, -nNewWidth);
		HRGN hInnerRegion = ::CreateEllipticRgn(InternalRect.left, InternalRect.top, InternalRect.right, InternalRect.bottom);
		bHit &= (!::PtInRegion(hInnerRegion, Pt.x, Pt.y));
		::DeleteObject(hInnerRegion);
	}

	return bHit;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymEllipse::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	bool bReturn = CAGSym::Read(pInput);
	bReturn = CAGSymGraphic::Read(pInput);
	pInput->Read(&m_DestRect, sizeof(m_DestRect));

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymEllipse::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	bool bReturn = CAGSym::Write(pOutput);
	bReturn = CAGSymGraphic::Write(pOutput);
	pOutput->Write(&m_DestRect, sizeof(m_DestRect));

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
void CAGSymEllipse::WriteFYSXml(FILE* output, int itabs)
{
	CString strTabs;

	for (int x=0; x<itabs; x++)
		strTabs += "\t";

	double fXPos, fYPos;
	double fWidth, fHeight;
	double fAngle;

	GetXMLData(fXPos, fYPos, fWidth, fHeight, fAngle);
	fprintf(output, "%s<ShapeContent ShapeName='%s' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G' IsAdvanced='true'>\r\n", 
				strTabs, m_strShapeName, fXPos, fYPos, fWidth, fHeight, fAngle);

	CAGSymGraphic::WriteFYSXml(output, itabs+1);

	fprintf(output, "%s</ShapeContent>\r\n", strTabs);
}

//////////////////////////////////////////////////////////////////////
void CAGSymEllipse::GetDestInflateSize(SIZE& Size)
{
	Size.cx = 0;
	Size.cy = 0;
}

//////////////////////////////////////////////////////////////////////
CAGSymLine::CAGSymLine() : CAGSymGraphic(ST_LINE)
{
	m_LineStart = LS_LeftTop;
	m_strShapeName = "Line";
}

//////////////////////////////////////////////////////////////////////
CAGSymLine::~CAGSymLine()
{
}

//////////////////////////////////////////////////////////////////////
CAGSym* CAGSymLine::Duplicate()
{
	CAGSymLine* pSym = new CAGSymLine;
	if (!pSym)
		return NULL;

	CAGSym::DuplicateTo(pSym);
	CAGSymGraphic::DuplicateTo(pSym);
	pSym->m_DestRect = m_DestRect;

	return dynamic_cast<CAGSym*>(pSym);
}

//////////////////////////////////////////////////////////////////////
void CAGSymLine::Draw(CAGDC& dc, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return;

	CAGMatrix MatrixSaved = dc.SetSymbolMatrix(m_Matrix);
	CAGMatrix Matrix = dc.GetSymbolToDeviceMatrix();

	RECT DestRect = m_DestRect;
	CPoint pt[2];
	switch (m_LineStart)
	{
		case LS_LeftTop:
			pt[0].x = DestRect.left;
			pt[0].y = DestRect.top;
			pt[1].x = DestRect.right;
			pt[1].y = DestRect.bottom;
			break;
		case LS_RightTop:
			pt[0].x = DestRect.right;
			pt[0].y = DestRect.top;
			pt[1].x = DestRect.left;
			pt[1].y = DestRect.bottom;
			break;
		case LS_RightBottom:
			pt[0].x = DestRect.right;
			pt[0].y = DestRect.bottom;
			pt[1].x = DestRect.left;
			pt[1].y = DestRect.top;
			break;
		case LS_LeftBottom:
			pt[0].x = DestRect.left;
			pt[0].y = DestRect.bottom;
			pt[1].x = DestRect.right;
			pt[1].y = DestRect.top;
			break;
		default:
			return;
	}
	Matrix.Transform(pt, 2);

	COLORREF LineColor = (m_LineWidth == LT_None ? CLR_NONE : m_LineColor);
	bool bInvisible = (LineColor == CLR_NONE);
	if (bInvisible) // don't let it be invisible
		LineColor = RGB(0,0,0);

	int LineWidth = (m_LineWidth == LT_Hairline || bInvisible ? LT_Hairline : dtoi(dc.GetViewToDeviceMatrix().TransformDistance(m_LineWidth)));
	dc.DrawLine(pt, LineColor, LineWidth);
	dc.SetSymbolMatrix(MatrixSaved);
}

//////////////////////////////////////////////////////////////////////
bool PointOnLine(const POINT& ptC, const POINT& ptA, const POINT& ptB, int iCloseness)
{
	// From comp.graphics.algorithms FAQ
	// Let the point be C and the line be AB.
	// The squared length of the line segment AB is L2:

	double dxBA = ptB.x - ptA.x;
	double dyBA = ptB.y - ptA.y;
	double dxCA = ptC.x - ptA.x;
	double dyCA = ptC.y - ptA.y;
	double L2 = (dxBA * dxBA) + (dyBA * dyBA);

	double r = ((dxCA * dxBA) + (dyCA * dyBA)) / L2;
	// If r<0      I is on backward extension of AB
	// If r>1      I is on forward extension of AB
	// If 0<=r<=1  I is between A and B
	bool bIonAB = (r >= 0.0 && r <= 1.0);
	if (!bIonAB)
		return false;

	double s = ((dxCA * dyBA) - (dyCA * dxBA)) / L2;
	// If s<0      C is to the left of AB (you can just check the numerator)
	// If s>0      C is to the right of AB
	// If s=0      C is on the line AB
	bool bConAB = !s;

	// Compute the distance from C to I
	// double distCI = sqrt(L2) * s;
	double distCI2 = L2 * s * s;

	// Compute the distance from A to I
	// double distAI = sqrt(L2) * r;

	// Compute I to be the point of perpendicular projection of C onto AB
	// POINT ptI = {0,0};
	// ptI.x = ptA.x + dtoi(dxBA * r);
	// ptI.y = ptA.y + dtoi(dyBA * r);

	return (distCI2 < (double)(iCloseness * iCloseness));
}

//////////////////////////////////////////////////////////////////////
bool CAGSymLine::HitTest(POINT Pt, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return false;

	RECT HitRect = {Pt.x-_HITTEST, Pt.y-_HITTEST, Pt.x+_HITTEST, Pt.y+_HITTEST};
	m_Matrix.Inverse().Transform(HitRect);
	m_Matrix.Inverse().Transform(Pt);

	RECT DestRect = m_DestRect;

	int iCloseness = m_LineWidth / 2;
	if (iCloseness < INCHES(1,8))
		iCloseness = INCHES(1,8);

	bool bHit = PointOnLine(Pt, *(POINT*)&DestRect.left, *(POINT*)&DestRect.right, iCloseness);
	return bHit;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymLine::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	bool bReturn = CAGSym::Read(pInput);
	bReturn = CAGSymGraphic::Read(pInput);
	pInput->Read(&m_DestRect, sizeof(m_DestRect));
	pInput->Read(&m_LineStart, sizeof(m_LineStart));

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymLine::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	bool bReturn = CAGSym::Write(pOutput);
	bReturn = CAGSymGraphic::Write(pOutput);
	pOutput->Write(&m_DestRect, sizeof(m_DestRect));
	pOutput->Write(&m_LineStart, sizeof(m_LineStart));

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
void CAGSymLine::WriteFYSXml(FILE* output, int itabs)
{
	CString strTabs;
	double fXPos, fYPos;
	double fWidth, fHeight;
	double fAngle;

	for (int x=0; x<itabs; x++)
		strTabs += "\t";

	GetXMLData(fXPos, fYPos, fWidth, fHeight, fAngle);
	fprintf(output, "%s<ShapeContent ShapeName='%s' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G' IsAdvanced='true'>\r\n", 
				strTabs, m_strShapeName, fXPos, fYPos, fWidth, fHeight, fAngle);

	CAGSymGraphic::WriteFYSXml(output, itabs+1);

	fprintf(output, "%s</ShapeContent>\r\n", strTabs);
}

//////////////////////////////////////////////////////////////////////
void CAGSymLine::GetDestInflateSize(SIZE& Size)
{
	int iInflateSize = (m_LineWidth >= LT_Normal ? dtoi(1.414214 * m_LineWidth) : 0); // approx. sqrt(2) * m_LineWidth
	Size.cx = iInflateSize;
	Size.cy = iInflateSize;
}

//////////////////////////////////////////////////////////////////////
CAGSymDrawing::CAGSymDrawing() : CAGSymGraphic(ST_DRAWING)
{
	m_strShapeName = _T("");
	m_nPoints = 0;
	m_pPoints = NULL;
}

//////////////////////////////////////////////////////////////////////
CAGSymDrawing::~CAGSymDrawing()
{
	if (m_pPoints)
		delete [] m_pPoints;
}

//////////////////////////////////////////////////////////////////////
CAGSym* CAGSymDrawing::Duplicate()
{
	CAGSymDrawing* pSym = new CAGSymDrawing;
	if (!pSym)
		return NULL;

	CAGSym::DuplicateTo(pSym);
	CAGSymGraphic::DuplicateTo(pSym);
	pSym->m_DestRect = m_DestRect;
	pSym->SetShapeName(m_strShapeName);

	return dynamic_cast<CAGSym*>(pSym);
}

//////////////////////////////////////////////////////////////////////
void CAGSymDrawing::Draw(CAGDC& dc, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return;

	if (!m_nPoints)
		CShape::RegeneratePoints(this, false/*fAspect*/);

	int nCommands = 0;
	CPoint* pCommands = m_pPoints;
	if (m_nPoints > 0)
	{
		while (SYMBOL_COMMANDPOINT(m_pPoints[nCommands].x))
			nCommands++;
	}
	int nPoints = m_nPoints - nCommands;
	CPoint* pPoints = new CPoint[nPoints];
	for (int i = 0; i < nPoints; i++)
		pPoints[i] = m_pPoints[i + nCommands];

	CAGMatrix MatrixSaved = dc.SetSymbolMatrix(m_Matrix);
	CAGMatrix Matrix = dc.GetSymbolToDeviceMatrix();
	Matrix.Transform(pPoints, nPoints);

	HRGN hClipRegion = dc.SaveClipRegion();
	dc.SetClipShape(pCommands, nCommands, pPoints, nPoints);

	COLORREF LineColor = (m_LineWidth == LT_None ? CLR_NONE : m_LineColor);
	COLORREF FillColor = (m_FillType == FT_None ? CLR_NONE : m_FillColor);
	bool bInvisible = (LineColor == CLR_NONE && FillColor == CLR_NONE);
	if (bInvisible) // don't let it be invisible
		LineColor = RGB(0,0,0);

	// Fill the interior
	if (m_FillType != FT_None)
	{
		RECT DestRect = m_DestRect;
		CPoint pt[4];
		Matrix.TransformRectToPolygon(DestRect, pt);

		if (m_FillType == FT_SweepRight || m_FillType == FT_SweepDown)
			dc.FillLinearGradient(pt, m_FillColor, m_FillColor2, (m_FillType == FT_SweepRight)); 
		else
		if  (m_FillType == FT_RadialCenter || m_FillType == FT_RadialCorner)
			dc.FillRadialGradient(pt, m_FillColor, m_FillColor2, (m_FillType == FT_RadialCenter));
		else
			dc.FillRectangle(pt, m_FillColor);
	}

	// Draw the outline
	if (m_LineWidth >= LT_Normal)
	{
		int LineWidth = dtoi(dc.GetViewToDeviceMatrix().TransformDistance(2*m_LineWidth+1));
		dc.DrawShape(pCommands, nCommands, pPoints, nPoints, LineColor, LineWidth, CLR_NONE);
	}

	dc.RestoreClipRegion(hClipRegion);

	if (m_LineWidth == LT_Hairline || bInvisible)
		dc.DrawShape(pCommands, nCommands, pPoints, nPoints, LineColor, LT_Hairline, CLR_NONE);

	dc.SetSymbolMatrix(MatrixSaved);
	delete [] pPoints;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymDrawing::PtInShape(POINT Pt, POINT* pCommands, int nCommands, POINT* pPoints, int nPoints)
{
	HDC m_hDC = ::GetDC(NULL);
	if (!m_hDC)
		return false;

	::BeginPath(m_hDC);

	if (!nCommands)
	{	// Support the old way of doing things
		::Polygon(m_hDC, pPoints, nPoints);
	}
	else
	{
		int nCurrentPoint = 0;
		for (int i = 0; i < nCommands; i++)
		{
			if (pCommands->x == SYMBOL_MOVE)
			{
				::CloseFigure(m_hDC);
				::MoveToEx(m_hDC, pPoints[nCurrentPoint].x, pPoints[nCurrentPoint].y, NULL);
			}
			else
			if (pCommands->x == SYMBOL_POLYGON)
				::Polygon(m_hDC, &pPoints[nCurrentPoint], pCommands->y);
			else 
			if (pCommands->x == SYMBOL_BEZIER)
				::PolyBezierTo(m_hDC, &pPoints[nCurrentPoint], pCommands->y);
			else
			if (pCommands->x == SYMBOL_LINE)
				::LineTo(m_hDC, pPoints[nCurrentPoint].x, pPoints[nCurrentPoint].y);

			nCurrentPoint += pCommands->y;
			pCommands++;
		}
	}

	::CloseFigure(m_hDC);
	::EndPath(m_hDC);

	HRGN hRegion = ::PathToRegion(m_hDC);
	bool bHit = !!::PtInRegion(hRegion, Pt.x, Pt.y);
	::DeleteObject(hRegion);
	::DeleteDC(m_hDC);
	return bHit;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymDrawing::HitTest(POINT Pt, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return false;

	RECT HitRect = {Pt.x-_HITTEST, Pt.y-_HITTEST, Pt.x+_HITTEST, Pt.y+_HITTEST};
	m_Matrix.Inverse().Transform(HitRect);
	m_Matrix.Inverse().Transform(Pt);

	RECT DestRect = m_DestRect;
	if (WIDTH(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, INCHES(1,16), 0);
	if (HEIGHT(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, 0, INCHES(1,16));

	bool bHit = !!::IntersectRect(&HitRect, &HitRect, &DestRect);
	if (!bHit)
		return false;

	if (!m_nPoints)
		CShape::RegeneratePoints(this, false/*fAspect*/);

	int nCommands = 0;
	POINT* pCommands = m_pPoints;
	if (m_nPoints > 0)
	{
		while (SYMBOL_COMMANDPOINT(m_pPoints[nCommands].x))
			nCommands++;
	}
	int nPoints = m_nPoints - nCommands;
	POINT* pPoints = new POINT[nPoints];
	for (int i = 0; i < nPoints; i++)
		pPoints[i] = m_pPoints[i + nCommands];

	bHit = PtInShape(Pt, pCommands, nCommands, pPoints, nPoints);

	delete [] pPoints;

	return bHit;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymDrawing::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	bool bReturn = CAGSym::Read(pInput);
	bReturn = CAGSymGraphic::Read(pInput);
	pInput->Read(&m_DestRect, sizeof(m_DestRect));
	pInput->Read(&m_nPoints, sizeof(m_nPoints));

	if (m_pPoints)
	{
		delete [] m_pPoints;
		m_pPoints = NULL;
	}

	// If there are points available, read them in
	// Otherwise, read in the shape name
	if (m_nPoints > 0)
	{ // Read in the shape points
		m_pPoints = new CPoint[m_nPoints];
		for (int i = 0; i < m_nPoints; i++)
			pInput->Read(&m_pPoints[i], sizeof(POINT));
		SetShapeName(NULL);
	}
	else
	{ // Read in the shape name
		BYTE cLength = 0;
		pInput->Read(&cLength, sizeof(cLength));
		if (cLength > 0)
		{
			char strShapeName[256];
			pInput->Read(strShapeName, cLength);
			strShapeName[cLength] = '\0';
			SetShapeName(strShapeName);
		}
		else
			SetShapeName(NULL);
	}

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymDrawing::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	bool bReturn = CAGSym::Write(pOutput);
	bReturn = CAGSymGraphic::Write(pOutput);
	pOutput->Write(&m_DestRect, sizeof(m_DestRect));

	// If there are points available and no shape name is defined, write out the shape points
	// Otherwise, write out the shape name
	if (m_nPoints > 0 && m_strShapeName == _T(""))
	{ // Write out the shape points
		pOutput->Write(&m_nPoints, sizeof(m_nPoints));
		for (int i = 0; i < m_nPoints; i++)
			pOutput->Write(&m_pPoints[i], sizeof(POINT));
	}
	else
	{ // Otherwise, write out the shape name
		int nZeroPoints = 0;
		pOutput->Write(&nZeroPoints, sizeof(nZeroPoints));
		BYTE cLength = (BYTE)m_strShapeName.GetLength();
		pOutput->Write(&cLength, sizeof(cLength));
		if (cLength > 0)
			pOutput->Write(m_strShapeName, cLength);
	}

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
void CAGSymDrawing::WriteFYSXml(FILE* output, int itabs)
{
	CString strTabs;
	double fXPos, fYPos;
	double fWidth, fHeight;
	double fAngle;

	for (int x=0; x<itabs; x++)
		strTabs += "\t";

	GetXMLData(fXPos, fYPos, fWidth, fHeight, fAngle);
	POINT* pPoints = NULL;
	int nPoints = GetPoints(&pPoints);

	if (!nPoints)
	{
		CShape::RegeneratePoints(this, false/*fAspect*/);
		nPoints = GetPoints(&pPoints);
	}

	int nCommands = 0;
	if (nPoints)
	{
		while (SYMBOL_COMMANDPOINT(pPoints[nCommands].x))
			nCommands++;
	}

	if (!nPoints || !nCommands)
	{
		// treat shapes with no points as basic types - no Figure element
		fprintf(output, "%s<ShapeContent ShapeName='%s' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G'>\r\n", 
					strTabs, "Rectangle", fXPos, fYPos, fWidth, fHeight, fAngle);

		CAGSymGraphic::WriteFYSXml(output, itabs+1);

		fprintf(output, "%s</ShapeContent>\r\n", strTabs);
		return;
	}

	fXPos = fYPos = 0;
	fprintf(output, "%s<ShapeContent ShapeName='%s' XPos='%0.5G' YPos='%0.5G' Width='%0.5G' Height='%0.5G' Angle='%0.5G'>\r\n", 
				strTabs, m_strShapeName, fXPos, fYPos, fWidth, fHeight, fAngle);

	CAGSymGraphic::WriteFYSXml(output, itabs+1);

	nPoints -= nCommands;
	POINT* pCommands = pPoints;
	pPoints += nCommands;

	int nCurrentPoint = 0;
	fprintf(output, "%s\t<Figure>\r\n", strTabs);
	for (int i = 0; i < nCommands; i++)
	{
		if (pCommands->x == SYMBOL_MOVE)
		{
			if (i)
			{
				fprintf(output, "%s\t</Figure>\r\n", strTabs);
				fprintf(output, "%s\t<Figure>\r\n", strTabs);
			}
			double fx = DINCHES(pPoints[nCurrentPoint].x);
			double fy = DINCHES(pPoints[nCurrentPoint].y);
			fprintf(output, "%s\t\t<MoveTo XPos='%0.5G' YPos='%0.5G' />\r\n", strTabs, fx, fy);
		}
		else
		if (pCommands->x == SYMBOL_POLYGON)
		{
			fprintf(output, "%s\t\t<Polygon>\r\n", strTabs);
			for (int p = 0; p < pCommands->y; p++)
			{
				double fx = DINCHES(pPoints[nCurrentPoint+p].x);
				double fy = DINCHES(pPoints[nCurrentPoint+p].y);
				fprintf(output, "%s\t\t\t<Point XPos='%0.5G' YPos='%0.5G' />\r\n", strTabs, fx, fy);
			}
			fprintf(output, "%s\t\t</Polygon>\r\n", strTabs);
		}
		else 
		if (pCommands->x == SYMBOL_BEZIER)
		{
			fprintf(output, "%s\t\t<PolyBezierTo>\r\n", strTabs);
			for (int p = 0; p < pCommands->y; p++)
			{
				double fx = DINCHES(pPoints[nCurrentPoint+p].x);
				double fy = DINCHES(pPoints[nCurrentPoint+p].y);
				fprintf(output, "\t\t\t\t\t\t\t<Point XPos='%0.5G' YPos='%0.5G' />\r\n", fx, fy);
			}
			fprintf(output, "%s\t\t</PolyBezierTo>\r\n", strTabs);
		}
		else
		if (pCommands->x == SYMBOL_LINE)
		{
			double fx = DINCHES(pPoints[nCurrentPoint].x);
			double fy = DINCHES(pPoints[nCurrentPoint].y);
			fprintf(output, "%s\t\t<LineTo XPos='%0.5G' YPos='%0.5G' />\r\n", strTabs, fx, fy);
		}

		nCurrentPoint += pCommands->y;
		pCommands++;
	}

	fprintf(output, "%s\t</Figure>\r\n", strTabs);
	fprintf(output, "%s</ShapeContent>\r\n", strTabs);
}

//////////////////////////////////////////////////////////////////////
void CAGSymDrawing::GetDestInflateSize(SIZE& Size)
{
	Size.cx = 0;
	Size.cy = 0;
}

//////////////////////////////////////////////////////////////////////
void CAGSymDrawing::SetPoints(POINT* pPoints, int nPoints)
{
	m_nPoints = nPoints;
	if (m_pPoints)
		delete [] m_pPoints;
	m_pPoints = new CPoint[m_nPoints];
	for (int i = 0; i < m_nPoints; i++)
		m_pPoints[i] = pPoints[i];
}

//////////////////////////////////////////////////////////////////////
int CAGSymDrawing::GetPoints(POINT** ppPoints)
{
	if (!m_pPoints || !m_nPoints)
	{
		*ppPoints = NULL;
		return 0;
	}

	*ppPoints = m_pPoints;
	return m_nPoints;
}

//////////////////////////////////////////////////////////////////////
void CAGSymDrawing::SetShapeName(LPCTSTR pShapeName)
{
	if (pShapeName && *pShapeName != '\0')
	{
		m_strShapeName = pShapeName;
		m_nPoints = 0;
		if (m_pPoints)
		{
			delete [] m_pPoints;
			m_pPoints = NULL;
		}
	}
	else
	{
		m_strShapeName = _T("");
	}
}

//////////////////////////////////////////////////////////////////////
LPCTSTR CAGSymDrawing::GetShapeName()
{
	if (m_strShapeName == _T(""))
		return NULL;
	else
		return m_strShapeName;
}

//////////////////////////////////////////////////////////////////////
CAGSymCalendar::CAGSymCalendar() : CAGSymGraphic(ST_CALENDAR)
{
	COleDateTime Now = COleDateTime::GetCurrentTime();
	m_iMonth = Now.GetMonth();
	m_iYear = Now.GetYear();
	m_iMode = 0;

	// Initialize the calendar styles
	m_uCalStyles.dwData = 0;
	m_uCalStyles1.dwData = 0;

	m_uCalStyles.HideBoxes = true;
	m_uCalStyles.HideAllBoxes = false;
	m_uCalStyles.YearStyle = CALSTYLES::YR_UPPERCASE_YYYY;
	m_uCalStyles.WeekStyle = CALSTYLES::WK_ABBR_UPPERCASE;
	m_uCalStyles.TitleSize = CALSTYLES::TITLESIZE_1;
	m_uCalStyles.DayOfWkSize = CALSTYLES::WEEKSIZE_1;
	m_uCalStyles.WeekStart = CALSTYLES::SUNDAY_START;

	m_uCalStyles1.TitleHorzJust = eRagCentered;
	m_uCalStyles1.WeekHorzJust = eRagCentered;
	m_uCalStyles1.DateHorzJust = eFlushRight;
	m_uCalStyles1.TitleVertJust = eVertCentered;
	m_uCalStyles1.WeekVertJust = eVertCentered;
	m_uCalStyles1.DateVertJust = eVertTop;

	m_SpecMonth.m_LineColor = RGB(0,0,0);
	m_SpecMonth.m_FillType = FT_Solid;
	m_SpecMonth.m_FillColor = MYRGB(0x000000);
	m_SpecMonth.m_FillColor2 = RGB(255,255,0);
	lstrcpy(m_SpecMonth.m_Font.lfFaceName, "DomCasual BT");
	m_SpecMonth.m_Font.lfWeight = FW_NORMAL;
	m_SpecMonth.m_Font.lfItalic = false;
	m_SpecMonth.m_Font.lfHeight = -POINTUNITS(30);

	m_SpecWeek.m_FillColor = MYRGB(0x000000);
	lstrcpy(m_SpecWeek.m_Font.lfFaceName, "DomCasual BT");
	m_SpecWeek.m_Font.lfWeight = FW_NORMAL;
	m_SpecWeek.m_Font.lfUnderline = false;
	m_SpecWeek.m_HorzJust = eRagCentered;
	m_SpecWeek.m_Font.lfHeight = -POINTUNITS(14);

	m_SpecDays.m_FillColor = MYRGB(0x4682B4);
	lstrcpy(m_SpecDays.m_Font.lfFaceName, "DomCasual BT");
	m_SpecDays.m_Font.lfWeight = FW_NORMAL;
	m_SpecDays.m_Font.lfHeight = -POINTUNITS(14);

	m_LineWidth = POINTUNITS(1.0);
	m_LineColor = RGB(0,0,0);
	m_FillColor = MYRGB(0xB0C4DE);
	m_FillColor2 = MYRGB(0x87CEFA);
	m_FillColor3 = RGB(255,255,255);
}

//////////////////////////////////////////////////////////////////////
CAGSymCalendar::~CAGSymCalendar()
{
}

//////////////////////////////////////////////////////////////////////
CAGSym* CAGSymCalendar::Duplicate()
{
	CAGSymCalendar* pSym = new CAGSymCalendar();
	if (!pSym)
		return NULL;

	CAGSym::DuplicateTo(pSym);
	CAGSymGraphic::DuplicateTo(pSym);
	pSym->m_DestRect = m_DestRect;
	pSym->m_iMonth = m_iMonth;
	pSym->m_iYear = m_iYear;
	pSym->m_SpecMonth = m_SpecMonth;
	pSym->m_SpecWeek = m_SpecWeek;
	pSym->m_SpecDays = m_SpecDays;
	pSym->m_uCalStyles = m_uCalStyles;
	pSym->m_uCalStyles1 = m_uCalStyles1;

	return dynamic_cast<CAGSym*>(pSym);
}

//////////////////////////////////////////////////////////////////////
UINT CAGSymCalendar::GetVertJust()
{
	switch(m_iMode)
	{
		case 0:
			return m_uCalStyles1.TitleVertJust;
		case 1:
			return m_uCalStyles1.WeekVertJust;
		case 2:
			return m_uCalStyles1.DateVertJust;
		default:
			return m_uCalStyles1.TitleVertJust;
	}
}

//////////////////////////////////////////////////////////////////////
UINT CAGSymCalendar::GetHorzJust()
{
	switch(m_iMode)
	{
		case 0:
			return m_uCalStyles1.TitleHorzJust;
		case 1:
			return m_uCalStyles1.WeekHorzJust;
		case 2:
			return m_uCalStyles1.DateHorzJust;
		default:
			return m_uCalStyles1.TitleHorzJust;
	}
}

//////////////////////////////////////////////////////////////////////
CString CAGSymCalendar::GetActivePanelTitle()
{
	switch(m_iMode)
	{
		case 0:
			return "Month";
		case 1:
			return "Week";
		case 2:
			return "Days";
		default:
			return "Month";
	}
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::Draw(CAGDC& dc, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return;

	CAGMatrix MatrixSaved = dc.SetSymbolMatrix(m_Matrix);

	CCalSupport CalSupport(this, dc);
	CalSupport.Draw();

	dc.SetSymbolMatrix(MatrixSaved);
}

//////////////////////////////////////////////////////////////////////
bool CAGSymCalendar::HitTest(POINT Pt, DWORD dwFlags)
{
	if (IsHidden(dwFlags))
		return false;

	RECT HitRect = {Pt.x-_HITTEST, Pt.y-_HITTEST, Pt.x+_HITTEST, Pt.y+_HITTEST};
	m_Matrix.Inverse().Transform(HitRect);
	m_Matrix.Inverse().Transform(Pt);

	RECT DestRect = m_DestRect;
	if (WIDTH(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, INCHES(1,16), 0);
	if (HEIGHT(DestRect) < INCHES(1,8))
		::InflateRect(&DestRect, 0, INCHES(1,16));

	bool bHit = !!::IntersectRect(&HitRect, &HitRect, &DestRect);
	return bHit;
}

//////////////////////////////////////////////////////////////////////
bool CAGSymCalendar::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	bool bReturn = CAGSym::Read(pInput);
	bReturn = CAGSymGraphic::Read(pInput);
	pInput->Read(&m_DestRect, sizeof(m_DestRect));
	pInput->Read(&m_iMonth, sizeof(m_iMonth));
	pInput->Read(&m_iYear, sizeof(m_iYear));
	pInput->Read(&m_uCalStyles.dwData, sizeof(m_uCalStyles.dwData));
	pInput->Read(&m_uCalStyles1.dwData, sizeof(m_uCalStyles1.dwData));
	long lUnused = 0;	
	pInput->Read(&lUnused, sizeof(lUnused)); // Reserved for future
	pInput->Read(&lUnused, sizeof(lUnused)); // Reserved for future
	pInput->Read(&lUnused, sizeof(lUnused)); // Reserved for future
	pInput->Read(&lUnused, sizeof(lUnused)); // Reserved for future
	pInput->Read(&lUnused, sizeof(lUnused)); // Reserved for future
	pInput->Read(&lUnused, sizeof(lUnused)); // Reserved for future
	bReturn = m_SpecMonth.Read(pInput);
	bReturn = m_SpecWeek.Read(pInput);
	bReturn = m_SpecDays.Read(pInput);
	return bReturn;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::WriteFYSXml(FILE* output, int itabs)
{
}

//////////////////////////////////////////////////////////////////////
bool CAGSymCalendar::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	bool bReturn = CAGSym::Write(pOutput);
	bReturn = CAGSymGraphic::Write(pOutput);
	pOutput->Write(&m_DestRect, sizeof(m_DestRect));
	pOutput->Write(&m_iMonth, sizeof(m_iMonth));
	pOutput->Write(&m_iYear, sizeof(m_iYear));
	pOutput->Write(&m_uCalStyles.dwData, sizeof(m_uCalStyles.dwData));
	pOutput->Write(&m_uCalStyles1.dwData, sizeof(m_uCalStyles1.dwData));
	long lUnused = 0;
	pOutput->Write(&lUnused, sizeof(lUnused)); // Reserved for future
	pOutput->Write(&lUnused, sizeof(lUnused)); // Reserved for future
	pOutput->Write(&lUnused, sizeof(lUnused)); // Reserved for future
	pOutput->Write(&lUnused, sizeof(lUnused)); // Reserved for future
	pOutput->Write(&lUnused, sizeof(lUnused)); // Reserved for future
	pOutput->Write(&lUnused, sizeof(lUnused)); // Reserved for future
	bReturn = m_SpecMonth.Write(pOutput);
	bReturn = m_SpecWeek.Write(pOutput);
	bReturn = m_SpecDays.Write(pOutput);
	return bReturn;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::GetDestInflateSize(SIZE& Size)
{
	Size.cx = 0;
	Size.cy = 0;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::GetFonts(LOGFONTLIST& lfList)
{
	for (int i = 0; i < 3; i++)
	{ // Loop through all of the specs and get each logical font
		LOGFONT lf = (i == 0 ? m_SpecMonth.m_Font : (i == 1 ? m_SpecWeek.m_Font : m_SpecDays.m_Font));

		int nFonts = lfList.size();
		int j;
		for (j = 0; j < nFonts; j++)
		{ // Check to see if the logical font is already in the font list
			if (!lstrcmp(lf.lfFaceName, lfList[j].lfFaceName) &&
				lf.lfWeight == lfList[j].lfWeight &&
				!lf.lfItalic == !lfList[j].lfItalic)
				break;
		}

		// If it's not in the list, add it
		if (j >= nFonts)
			lfList.push_back(lf);
	}
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecFillType(FillType Fill)
{
	CAGSpec* pAGSpec = GetActiveSpec();
	if (!pAGSpec)
		return;

	pAGSpec->m_FillType = Fill;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecFillColor(COLORREF Color)
{
	CAGSpec* pAGSpec = GetActiveSpec();
	if (!pAGSpec)
		return;

	pAGSpec->m_FillColor = Color;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecFillColor2(COLORREF Color)
{
	CAGSpec* pAGSpec = GetActiveSpec();
	if (!pAGSpec)
		return;
		
	pAGSpec->m_FillColor2 = Color;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecLineWidth(int LineWidth)
{
	CAGSpec* pAGSpec = GetActiveSpec();
	if (!pAGSpec)
		return;
		
	pAGSpec->m_LineWidth = LineWidth;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecLineColor(COLORREF Color)
{
	CAGSpec* pAGSpec = GetActiveSpec();
	if (!pAGSpec)
		return;
		
	pAGSpec->m_LineColor = Color;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecTextSizes(int mTextSize, int wTextSize, int dTextSize)
{
	m_SpecMonth.m_Font.lfHeight = -mTextSize;
	m_SpecWeek.m_Font.lfHeight  = -wTextSize;
	m_SpecDays.m_Font.lfHeight  = -dTextSize;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecTextSize(int TextSize)
{
	CAGSpec* pAGSpec = GetActiveSpec();
	if (!pAGSpec)
		return;
		
	pAGSpec->m_Font.lfHeight = -TextSize;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecTypeface(const LOGFONT& Font)
{
	CAGSpec* pAGSpec = GetActiveSpec();
	if (!pAGSpec)
		return;
		

	lstrcpy(pAGSpec->m_Font.lfFaceName, Font.lfFaceName);
	pAGSpec->m_Font.lfWeight = Font.lfWeight;
	pAGSpec->m_Font.lfItalic = (Font.lfItalic != 0);
	pAGSpec->m_Font.lfCharSet = Font.lfCharSet;
	pAGSpec->m_Font.lfOutPrecision = Font.lfOutPrecision;
	pAGSpec->m_Font.lfClipPrecision = Font.lfClipPrecision;
	pAGSpec->m_Font.lfQuality = Font.lfQuality;
	pAGSpec->m_Font.lfPitchAndFamily = Font.lfPitchAndFamily;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecEmphasis(bool bBold, bool bItalic, bool bUnderline)
{
	CAGSpec* pAGSpec = GetActiveSpec();
	if (!pAGSpec)
		return;
		
	pAGSpec->m_Font.lfWeight = (bBold ? FW_BOLD : FW_NORMAL);
	pAGSpec->m_Font.lfItalic = bItalic;
	pAGSpec->m_Font.lfUnderline = bUnderline;
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecHorzJust(eTSJust HorzJust)
{
	switch (m_iMode)
	{
		case 0:
			m_uCalStyles1.TitleHorzJust = HorzJust;
			break;
		case 1:
			m_uCalStyles1.WeekHorzJust = HorzJust;
			break;
		case 2:
			m_uCalStyles1.DateHorzJust = HorzJust;
			break;
	}
}

//////////////////////////////////////////////////////////////////////
void CAGSymCalendar::SetSpecVertJust(eVertJust VertJust)
{
	switch (m_iMode)
	{
		case 0:
			m_uCalStyles1.TitleVertJust = VertJust;
			break;
		case 1:
			m_uCalStyles1.WeekVertJust = VertJust;
			break;
		case 2:
			m_uCalStyles1.DateVertJust = VertJust;
			break;
	}
}
