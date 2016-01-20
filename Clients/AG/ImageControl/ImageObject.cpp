#include "StdAfx.h"
#include "ImageObject.h"
#include "ImageControlConstants.h"
#include "Image.h"
#include "Utility.h"
#include "MessageBox.h"
#include "math.h"
#include "FloodFill.h"
#include "ColorMap.h"
#include "Effects.h"
#include "Gradient.h"
#include "Gif.h"
#include "PngToDib.h"
#include "Gdip.h"

#define _HITTEST 3

//////////////////////////////////////////////////////////////////////
CImageObject::CImageObject(const CRect* pPageRect)
{
	Init();

	m_pPageRect = pPageRect;
	m_pDibOrig = NULL;
}

//////////////////////////////////////////////////////////////////////
CImageObject::CImageObject(const CRect* pPageRect, CDib pDIB)
{
	Init();

	m_pPageRect = pPageRect;
	m_pDibOrig = pDIB;

	LoadDIB(pDIB, false/*bTakeOverMemory*/);

//j	// The source data will be written to disk so we can recall it when we need it
//j	// This will set m_strSourceType
//j	m_strSourceFile = RandomFileName();
//j	HGLOBAL hMemory = DibCopyGlobal(m_pDibForEdit);
//j	WriteFile(m_strSourceFile, hMemory, &m_strSourceType);
//j	m_bSourceFileIsTemporary = true;
}

//////////////////////////////////////////////////////////////////////
CImageObject::CImageObject(const CRect* pPageRect, HGLOBAL hMemory)
{
	Init();

	m_pPageRect = pPageRect;
	m_pDibOrig = NULL;

	// Build the object's DIB
	CImage Image(hMemory);
	CDib pDIB = Image.GetDibPtr();
	if (pDIB)
		LoadDIB(pDIB, false/*bTakeOverMemory*/);

//j	// The source data will be written to disk so we can recall it when we need it
//j	// This will set m_strSourceType
//j	m_strSourceFile = RandomFileName();
//j	WriteFile(m_strSourceFile, hMemory, &m_strSourceType);
//j	m_bSourceFileIsTemporary = true;
}

//////////////////////////////////////////////////////////////////////
CImageObject::~CImageObject()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
void CImageObject::Init()
{
//	m_pPageRect = NULL; // Passed into the constructor - do not clear on Init()
	m_nID = 0;
	m_Matrix.Unity();
	m_MatrixForUndo.Unity();
	m_bMatrixForUndo = false;
	m_DestRect.SetRectEmpty();
	m_CropRect.SetRectEmpty();
	m_TransparentColor = CLR_NONE;
	m_LastHitColor = CLR_NONE;
	m_strSourceFile.Empty();
	m_strSourceType.Empty();
	m_bHidden = true;
	m_bDeleted = false;
	m_bModified = false;
	m_bCoverDraw = false;
	m_bSourceFileIsTemporary = false;
	m_bEditStarted = false;
//	m_pDibOrig = NULL; // Passed into the constructor - do not clear on Init()
	m_pDibForEdit = NULL;
	m_pDibForDraw = NULL;
	m_pDibForUndo = NULL;
	m_fDibAngle = 0;
	m_strEffectOption.Empty();
	m_strBorderType.Empty();
	m_nBorderSize = 0;
	m_nBorderSoftness = 0;
	m_bBorderStretch = false;
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
	FreeDibForDraw();

	if (m_pDibForEdit)
	{
		DibFree(m_pDibForEdit);
		m_pDibForEdit = NULL;
	}

	if (m_pDibForUndo)
	{
		DibFree(m_pDibForUndo);
		m_pDibForUndo = NULL;
	}

	m_bHidden = true;
}

//////////////////////////////////////////////////////////////////////
CImageObject* CImageObject::Duplicate(bool bCopyDib)
{
	CImageObject* pImageObject = new CImageObject(m_pPageRect);
	if (!pImageObject)
		return NULL;

	pImageObject->m_nID = m_nID;
	pImageObject->m_Matrix = m_Matrix;
	pImageObject->m_MatrixForUndo = m_MatrixForUndo;
	pImageObject->m_bMatrixForUndo = m_bMatrixForUndo;
	pImageObject->m_pPageRect = m_pPageRect;
	pImageObject->m_DestRect = m_DestRect;
	pImageObject->m_CropRect = m_CropRect;
	pImageObject->m_TransparentColor = m_TransparentColor;
	pImageObject->m_LastHitColor = m_LastHitColor;
	pImageObject->m_strSourceFile = DuplicateSourceFile();
	pImageObject->m_strSourceType = m_strSourceType;
	pImageObject->m_bHidden = m_bHidden;
	pImageObject->m_bDeleted = m_bDeleted;
	pImageObject->m_bModified = m_bModified;
	pImageObject->m_bCoverDraw = m_bCoverDraw;
	pImageObject->m_bSourceFileIsTemporary = m_bSourceFileIsTemporary;
	pImageObject->m_bEditStarted = m_bEditStarted;
	pImageObject->m_pDibForEdit = NULL;
	pImageObject->m_pDibForDraw = NULL;
	pImageObject->m_pDibForUndo = NULL;
	pImageObject->m_fDibAngle = 0;

	pImageObject->m_bSourceFileIsTemporary = (!m_strSourceFile.IsEmpty() ? true : m_bSourceFileIsTemporary);
	pImageObject->m_strEffectOption = m_strEffectOption;
	pImageObject->m_strBorderType = m_strBorderType;
	pImageObject->m_nBorderSize = m_nBorderSize;
	pImageObject->m_nBorderSoftness = m_nBorderSoftness;
	pImageObject->m_bBorderStretch = m_bBorderStretch;

	if (bCopyDib)
		pImageObject->LoadDIB(m_pDibForEdit, false/*bTakeOverMemory*/);

	return pImageObject;
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
void CImageObject::SetRotatedDibForDraw()
{
	double fAngle = m_Matrix.GetAngle();
	if (fAngle == m_fDibAngle)
		return;

	FreeDibForDraw();
	m_pDibForDraw = m_pDibForEdit.Rotate(fAngle);
	m_fDibAngle = fAngle;
}

//////////////////////////////////////////////////////////////////////
void CImageObject::SetNormalDibForDraw()
{
	if (m_pDibForDraw && m_pDibForDraw != m_pDibForEdit)
		DibFree(m_pDibForDraw);

	m_pDibForDraw = m_pDibForEdit;
	m_fDibAngle = 0;
}

//////////////////////////////////////////////////////////////////////
void CImageObject::FreeDibForDraw()
{
	if (m_pDibForDraw && m_pDibForDraw != m_pDibForEdit)
		DibFree(m_pDibForDraw);

	m_pDibForDraw = NULL;;
	m_fDibAngle = 0;
}

//////////////////////////////////////////////////////////////////////
void CImageObject::Draw(CAGDC& dc, bool bOverrideHidden)
{
	if (IsHidden(bOverrideHidden))
		return;

	if (m_Matrix.IsRotated())
		SetRotatedDibForDraw();
	else
		SetNormalDibForDraw();
	
	CDib pDib = m_pDibForDraw;
	if (!pDib)
		return;

	CMatrix MatrixSaved = dc.SetSymbolMatrix(m_Matrix);
	CRect DestRect = GetDestRect();
	HRGN hClipRegion = dc.SaveClipRegion();
	dc.SetClipRectangle(&DestRect, true/*bReduceByOne*/);
	dc.StretchBlt(pDib, GetCropRect(), DestRect, m_TransparentColor);
	dc.RestoreClipRegion(hClipRegion);

	// If necessary, draw the 4 cover rectangles around the image
	if (m_bCoverDraw && dc.GetCoverAllowed())
	{
		CMatrix Matrix = dc.GetViewToDeviceMatrix();
		CRect PageRect = dc.GetViewRect();
		CRect ClipRect = PageRect;
		Matrix.Transform(ClipRect);

		HRGN hClipRegion = dc.SaveClipRegion();
		dc.SetClipRect(ClipRect, true/*bCombine*/);

		CRect DestRectT = GetDestRectTransformed();
		CRect RectL(PageRect.left, DestRectT.top, DestRectT.left, DestRectT.bottom);
		CRect RectT(PageRect.left, PageRect.top, PageRect.right, DestRectT.top);
		CRect RectR(DestRectT.right, DestRectT.top, PageRect.right, DestRectT.bottom);
		CRect RectB(PageRect.left, DestRectT.bottom, PageRect.right, PageRect.bottom);

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
bool CImageObject::HitTest(CPoint Pt, bool bOverrideHidden)
{
	#define SHIFT (GetAsyncKeyState(VK_SHIFT)<0)
	#define RBUTTON (GetAsyncKeyState(VK_RBUTTON)<0)

	if (IsHidden(bOverrideHidden))
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

	CDib pDib = m_pDibForEdit;
	if (!pDib)
		return true;

	if (pDib.BitCount() != 8 && pDib.BitCount() != 24)
		return true;

	DestRect = m_DestRect;
	CMatrix Matrix;
	double xScale = (double)DibWidth(pDib) / DestRect.Width();
	double yScale = (double)DibHeight(pDib) / DestRect.Height();
	Matrix.Translate(-DestRect.left, -DestRect.top);
	Matrix.Scale(xScale, yScale, 0, 0);
	Matrix.Transform(Pt);
	if (Pt.x < 0 || Pt.x >= (long)DibWidth(pDib) || Pt.y < 0 || Pt.y >= (long)DibHeight(pDib))
		return true;

	COLORREF Color = CLR_NONE;
	BYTE* pPixel = DibPtrXYExact(pDib, Pt.x, Pt.y);
	if (pDib.BitCount() == 24)
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

	if (m_TransparentColor != CLR_NONE && m_TransparentColor == Color)
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
void CImageObject::GetDestInflateSize(SIZE& Size)
{
	Size.cx = 0;
	Size.cy = 0;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::LoadDIB(CDib pDIB, bool bTakeOverMemory)
{
	if (!pDIB)
		return false;

	FreeDIB();

	if (pDIB.Compression() != BI_RGB)
		return false;

	if (pDIB.BitCount() != 1 && pDIB.BitCount() != 4 &&
		pDIB.BitCount() != 8 && pDIB.BitCount() != 24)
		return false;

	if (bTakeOverMemory)
		SetDib(pDIB, false/*bKeepCurrentAsUndo*/);
	else
		SetDib(DibCopy(pDIB), false/*bKeepCurrentAsUndo*/);

	CDib pDib = m_pDibForEdit;
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

	// Fit the image rectangle within the page rectangle
	int iWidth = DibWidth(pDIB);
	int iHeight = DibHeight(pDIB);
	int iResolution = DibResolutionX(pDIB);

	CRect PageRect;
	if (!m_pPageRect || m_pPageRect->IsRectEmpty())
		PageRect.SetRect(0, 0, INCHES(iWidth, iResolution), INCHES(iHeight, iResolution));
	else
	{
		PageRect = *m_pPageRect;

		// Make sure the image does not hang off the page
		if (INCHES(iWidth, iResolution) > PageRect.Width()) 
			iResolution = INCHES(iWidth, PageRect.Width());
		if (INCHES(iHeight, iResolution) > PageRect.Height())
			iResolution = INCHES(iHeight, PageRect.Height());
	}

	// Compute the image size in page units
	iWidth = INCHES(iWidth, iResolution);
	iHeight = INCHES(iHeight, iResolution);

	// Setup the DestRect to be centered within the page rectangle
	int cx = (PageRect.right + PageRect.left - iWidth) / 2;
	int cy = (PageRect.bottom + PageRect.top - iHeight) / 2;
	m_DestRect.SetRect(0, 0, iWidth, iHeight);
	m_DestRect.OffsetRect(cx, cy);
	m_CropRect.SetRectEmpty();

	m_bHidden = false;

	return true;
}

//////////////////////////////////////////////////////////////////////
int CImageObject::ProcessEndEdit(int iValue)
{
	if (iValue != EDIT_END || !EditCanUndo())
		return iValue;

	int iResponse = CMessageBox::Message(String("Would you like to apply your changes?"), MB_YESNO);
	if (iResponse == IDYES)
		return EDIT_APPLY;

	return iValue;
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

/////////////////////////////////////////////////////////////////////////////
COLORREF CImageObject::GetTransparentColor(LPCTSTR pFileName)
{
	CString strExtension = StrExtension(pFileName);
	COLORREF TransparentColor = CLR_NONE;
	if (strExtension == ".gif")
		TransparentColor = GetGifTransparentColor(pFileName);
	else
	if (strExtension == ".png")
		TransparentColor = CPng::GetPngTransparentColor(pFileName, true/*bDefaultToUpperLeft*/);

	return TransparentColor;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Open(LPCTSTR strFileName)
{
	if (!strFileName)
		return false;

	CString strExtension = StrExtension(strFileName);
	if (strExtension == ".png")
	{
		// If it is PNG, convert to a DIB.  We could read it in like other file
		// formats below, but the transparent color seems to get converted to black
		HGLOBAL hMemory = CPng::ReadPNG(strFileName, true/*bDefaultToUpperLeft*/);
		if (!hMemory)
			return false;

		CDib pDIB = (BITMAPINFOHEADER*)::GlobalLock(hMemory);
		bool bOK = LoadDIB(pDIB, false/*bTakeOverMemory*/);
		::GlobalUnlock(hMemory);
		::GlobalFree(hMemory);
		if (!bOK)
			return false;

		m_strSourceFile = strFileName;
		m_strSourceType = "PNG";

		return true;
	}

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

	// Build the object's DIB
	CImage Image(hMemory);
	CDib pDIB = Image.GetDibPtr();
	if (!pDIB || !LoadDIB(pDIB, false/*bTakeOverMemory*/))
	{
		::GlobalFree(hMemory);
		return false;
	}

	::GlobalFree(hMemory);

//j	COLORREF TransparentColor = GetTransparentColor(strFileName);
//j	if (TransparentColor != CLR_NONE && m_TransparentColor == CLR_NONE)
//j		m_TransparentColor = TransparentColor;

	m_strSourceFile = strFileName;
	m_strSourceType = strSourceType;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::ReOpen()
{
	if (m_pDibOrig)
	{
		Close();
		return LoadDIB(m_pDibOrig, false/*bTakeOverMemory*/);
	}

	if (!m_strSourceFile.IsEmpty())
	{
		Close();
		return Open(m_strSourceFile);
	}

	return false;
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

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::GetDibCropRect(CRect& DibCropRect)
{
	DibCropRect.SetRectEmpty();

	CDib pDib = m_pDibForEdit;
	if (!pDib)
		return false;

	CRect DestRect = GetDestRect();
	CRect CropRect = GetCropRect();
	m_Matrix.Transform(DestRect);
	m_Matrix.Transform(CropRect);

	double fx = (double)pDib.Width() / DestRect.Width();
	double fy = (double)pDib.Height() / DestRect.Height();

	CropRect.OffsetRect(-DestRect.left, -DestRect.top);
	DibCropRect.left   = dtoi(fx * CropRect.left);
	DibCropRect.top    = dtoi(fy * CropRect.top);
	DibCropRect.right  = dtoi(fx * CropRect.right);
	DibCropRect.bottom = dtoi(fy * CropRect.bottom);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::MakeGray()
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(false/*bEditInPlace*/))
		return false;

	CDib pDibNew = m_pDibForEdit.MakeGray();
	if (!pDibNew)
		return false;

	// Adjust the object's vitals
	SetDib(pDibNew, false/*bKeepCurrentAsUndo*/);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::Invert()
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(false/*bEditInPlace*/))
		return false;

	CDib pDibNew = m_pDibForEdit.Invert();
	if (!pDibNew)
		return false;

	// Adjust the object's vitals
	SetDib(pDibNew, false/*bKeepCurrentAsUndo*/);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::FlipRotateTab(int iValue)
{
	if (iValue == EDIT_END)
		iValue = ProcessEndEdit(iValue);

	if (iValue == EDIT_START)
	{
		return true;
	}
	else
	if (iValue == EDIT_APPLY)
	{
		EditApply();
		return true;
	}
	else
	if (iValue == EDIT_RESET || iValue == EDIT_END)
	{
		EditUndo();
		return true;
	}
	else
	if (iValue >= 0)
	{
		double fAngle = iValue;
		return Rotate(fAngle);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::FlipXY(bool bFlipX, bool bFlipY)
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(false/*bEditInPlace*/))
		return false;

	CDib pDibNew = m_pDibForEdit.Orient(0/*iRotateDirection*/, bFlipX, bFlipY);
	if (!pDibNew)
		return false;

	// Adjust the object's vitals
	SetDib(pDibNew, false/*bKeepCurrentAsUndo*/);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::Rotate(double fAngle)
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(true/*bEditInPlace*/))
		return false;

	// Adjust the object's matrix
	CRect DestRect = GetDestRect();
	CMatrix Matrix = m_MatrixForUndo;
	Matrix.Transform(DestRect);
	fAngle -= Matrix.GetAngle();
	Matrix.Rotate(fAngle, fAngle,
		(DestRect.left + DestRect.right) / 2,
		(DestRect.top + DestRect.bottom) / 2);

	// Change the object by applying the matrix
	SetMatrix(Matrix);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::CropTab(int iValue)
{
	if (iValue == EDIT_END)
		iValue = ProcessEndEdit(iValue);

	if (iValue == EDIT_START)
	{
		return true;
	}
	else
	if (iValue == EDIT_APPLY)
	{
		EditApply();
		return true;
	}
	else
	if (iValue == EDIT_RESET || iValue == EDIT_END)
	{
		EditUndo();
		return true;
	}
	else
	if (iValue >= 0)
	{
		return true;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::CropApply(const CRect& CropRectInPixels)
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(true/*bEditInPlace*/))
		return false;

	CDib pDibNew = m_pDibForEdit.Crop(CropRectInPixels);
	if (!pDibNew)
		return false;

	// Get the crop rect before we unselect the image and it is reset
	CRect CropRect = GetCropRect();

	// Adjust the object's vitals
	CRect DestRect = CropRect;
	SetDestRect(DestRect);
	SetCropRect(DestRect);
	SetDib(pDibNew, true/*bKeepCurrentAsUndo*/);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::ResizeTab(int iValue, const CSize& Size)
{
	if (iValue == EDIT_END)
		iValue = ProcessEndEdit(iValue);

	if (iValue == EDIT_START)
	{
		return true;
	}
	else
	if (iValue == EDIT_APPLY)
	{
		if (!ResizeApply(Size))
			return false;

		EditApply();
		return true;
	}
	else
	if (iValue == EDIT_RESET || iValue == EDIT_END)
	{
		EditUndo();
		return true;
	}
	else
	if (iValue >= 0)
	{
		return true;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::ResizeApply(const CSize& Size)
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(true/*bEditInPlace*/))
		return false;

	CDib pDibNew = m_pDibForEdit.Resize(Size);
	if (!pDibNew)
		return false;

	// Adjust the object's vitals
	CRect DestRect(0, 0, Size.cx, Size.cy);
	SetDestRect(DestRect);
	SetCropRect(DestRect);
	SetDib(pDibNew, true/*bKeepCurrentAsUndo*/);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::EffectsTab(int iValue)
{
	if (iValue == EDIT_END)
		iValue = ProcessEndEdit(iValue);

	if (iValue == EDIT_START)
	{
		return true;
	}
	else
	if (iValue == EDIT_APPLY)
	{
		EditApply();
		return true;
	}
	else
	if (iValue == EDIT_RESET || iValue == EDIT_END)
	{
		EditUndo();
		return true;
	}
	else
	if (iValue >= 0)
	{
		return EffectsExecute(iValue);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::EffectsExecute(int iValue)
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(false/*bEditInPlace*/))
		return false;

	CEffects Effects;
	if (m_strEffectOption == "NONE")
		return DibDuplicateData(m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "SHARPEN")
		return Effects.Sharpen(iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "BLUR")
		return Effects.Blur(iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "EDGES")
		return Effects.Edges(iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "EMBOSS")
		return Effects.Emboss(iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "PIXELIZE")
		return Effects.Pixelize(iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "POSTERIZE")
		return Effects.Posterize(iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "PRISM")
		return Effects.Prism(iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "OIL PAINTING")
		return Effects.OilPainting(iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "ANTIQUED")
		return Effects.Antiqued(iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);
	if (m_strEffectOption == "TEXTURIZE")
		return Effects.Texturize("PAPER", iValue/*iStrength*/, m_pDibForUndo, m_pDibForEdit);

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::EffectsTabSubCommand(CString& strCommand)
{
	m_strEffectOption = strCommand;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::BordersTab(int iValue)
{
	if (iValue == EDIT_END)
		iValue = ProcessEndEdit(iValue);

	if (iValue == EDIT_START)
	{
		return true;
	}
	else
	if (iValue == EDIT_APPLY)
	{
		EditApply();
		return true;
	}
	else
	if (iValue == EDIT_RESET || iValue == EDIT_END)
	{
		EditUndo();
		return true;
	}
	else
	if (iValue >= 0)
	{
		return GenerateBorder();
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::GenerateBorder()
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(false/*bEditInPlace*/))
		return false;

	CDib pDib = m_pDibForUndo;
	if (!pDib)
		return false;

	COLORREF ColorFrom = RGB(0,0,0);
	COLORREF ColorTo = RGB(255,255,255);
	COLORREF ColorForMask255 = RGB(253,254,255); // Near white - will become transparent
//j	COLORREF ColorForMask255 = (IsTransparent() ? GetTransparentColor() : RGB(254,254,254));
	bool bSetTransparentColor = true; //j !IsTransparent();

	// Generate the corresponding gradient
	CGradient Gradient(8/*iBitCount*/, pDib.Width(), pDib.Height(), ColorFrom, ColorTo, m_nBorderSize, m_nBorderSoftness, m_bBorderStretch);
	CDib pDIBMask;
	if (!m_strBorderType.CompareNoCase(_T("Rectangle")))
		pDIBMask = Gradient.CreateRectangle();
	else
	if (!m_strBorderType.CompareNoCase(_T("Oval")))
		pDIBMask = Gradient.CreateCircle();
	else
	if (!m_strBorderType.CompareNoCase(_T("Diamond")))
		pDIBMask = Gradient.CreateDiamond();
	else
	if (!m_strBorderType.CompareNoCase(_T("Left")))
	{
		pDIBMask = Gradient.CreateCurtain(false/*fVertical*/, false/*fForward*/);
		ColorForMask255 = ColorTo;
		bSetTransparentColor = false;
	}
	else
	if (!m_strBorderType.CompareNoCase(_T("Right")))
	{
		pDIBMask = Gradient.CreateCurtain(false/*fVertical*/, true/*fForward*/);
		ColorForMask255 = ColorTo;
		bSetTransparentColor = false;
	}
	else
	if (!m_strBorderType.CompareNoCase(_T("Top")))
	{
		pDIBMask = Gradient.CreateCurtain(true/*fVertical*/, false/*fForward*/);
		ColorForMask255 = ColorTo;
		bSetTransparentColor = false;
	}
	else
	if (!m_strBorderType.CompareNoCase(_T("Bottom")))
	{
		pDIBMask = Gradient.CreateCurtain(true/*fVertical*/, true/*fForward*/);
		ColorForMask255 = ColorTo;
		bSetTransparentColor = false;
	}
	else
		pDIBMask = Gradient.CreateShape(m_strBorderType);

	if (!pDIBMask)
		return false;

#ifdef DEBUG_MASK
	CDib pDibNew = pDIBMask.Copy();
#else
	CDib pDibNew = pDib.MergeMask(pDIBMask, ColorForMask255);
#endif DEBUG_MASK
	if (!pDibNew)
		return false;

	// Adjust the object's vitals
	SetDib(pDibNew, false/*bKeepCurrentAsUndo*/);
	if (bSetTransparentColor)
		SetTransparentColor(ColorForMask255);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::BorderType(LPCTSTR pstrType)
{
	m_strBorderType = pstrType;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::BorderSize(int nSize)
{
	m_nBorderSize = nSize;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::BorderSoftness(int nSoftness)
{
	m_nBorderSoftness = nSoftness;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::BorderStretch(bool bStretch)
{
	m_bBorderStretch = bStretch;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::BorderImageMerge(const CString& strFileName, CRect& PageRect)
{
	if (strFileName.IsEmpty())
		return false;

	CImageObject* pBorderObject = new CImageObject(&PageRect); // PageRect might be empty
	if (!pBorderObject)
		return false;

	if (!pBorderObject->Open(strFileName))
	{
		delete pBorderObject;
		return false;
	}

	if (!pBorderObject->IsTransparent())
		pBorderObject->SetTransparentColor(RGB(255,255,255));

	if (IsPortrait() != pBorderObject->IsPortrait())
	{
		CDib DibSrc = pBorderObject->GetDib();
		CDib pDibNew = DibSrc.Orient(1/*iRotateDirection*/, false/*bFlipX*/, false/*bFlipY*/);
		if (pDibNew)
			pBorderObject->SetDib(pDibNew, false/*bKeepCurrentAsUndo*/);
	}

	bool bOK = false;
//j	bOK = pBorderObject->Maximize(GetDestRect()/*PageRect*/, true/*bWidth*/, true/*bHeight*/);
	bOK = Merge(pBorderObject);
	pBorderObject->Close();
	delete pBorderObject;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::Merge(CImageObject* pObject)
{
	if (!pObject)
	{
		CMessageBox::NoObjectSelected();
		return false;
	}

	// Before we modify the image, make sure we have an undo image
	EditUndo();
	if (!EditStart(false/*bEditInPlace*/))
		return false;

	BITMAPINFO BitmapInfo;
	::ZeroMemory(&BitmapInfo, sizeof(BitmapInfo));
	BitmapInfo.bmiHeader = *m_pDibForUndo;
	CAGDIBSectionDC MergeDC(BitmapInfo, DIB_RGB_COLORS);
	if (!MergeDC.GetHDC())
	{
		CMessageBox::Message("Failed to get the handle to the Merge device context");
		return false;
	}

	CRect EmptyCropRect(0,0,0,0);
	MergeDC.StretchBlt(m_pDibForUndo, EmptyCropRect, GetDestRect(), GetTransparentColor());
	MergeDC.StretchBlt(pObject->GetDib(), EmptyCropRect, GetDestRect(), pObject->GetTransparentColor());

	CDib pDibNew = MergeDC.ExtractDib();
	if (!pDibNew)
		return false;

	// Adjust the object's vitals
	SetDib(pDibNew, false/*bKeepCurrentAsUndo*/);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageObject::Maximize(const CRect& PageRect, bool bWidth, bool bHeight)
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(true/*bEditInPlace*/))
		return false;

	CRect DestRect = GetDestRect();
	CMatrix Matrix = m_MatrixForUndo;
	Matrix.Transform(DestRect);

	double fScaleX = (double)PageRect.Width() / (double)max(DestRect.Width(),1);
	double fScaleY = (double)PageRect.Height() / (double)max(DestRect.Height(),1);
	if (!bWidth)
		fScaleX = 1.0;
	if (!bHeight)
		fScaleY = 1.0;
	Matrix.Scale(fScaleX, fScaleY,
		(DestRect.left + DestRect.right) / 2,
		(DestRect.top + DestRect.bottom) / 2);

	int dx = ((PageRect.right + PageRect.left) - (DestRect.right + DestRect.left)) / 2;
	int dy = ((PageRect.bottom + PageRect.top) - (DestRect.bottom + DestRect.top)) / 2;
	if (!bWidth)
		dx = 0;
	if (!bHeight)
		dy = 0;
	Matrix.Translate(dx, dy);

	// Change the object by applying the matrix
	SetMatrix(Matrix);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool HistogramImage(HISTOGRAM& Histogram, CDib pSrcDib)
{
	if (!pSrcDib)
		return false;

	CDib DibSrc = pSrcDib;
	return DibSrc.Histogram(Histogram);
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::ContrastBrightnessTab(int iValue)
{
	if (iValue == EDIT_END)
		iValue = ProcessEndEdit(iValue);

	if (iValue == EDIT_START)
	{
		return true;
	}
	else
	if (iValue == EDIT_APPLY)
	{
		EditApply();
		return true;
	}
	else
	if (iValue == EDIT_RESET || iValue == EDIT_END)
	{
		EditUndo();
		return true;
	}
	else
	if (iValue >= 0)
	{
		WORD wValue = LOWORD(iValue);
		int iContrast = (int)LOBYTE(wValue);
		int iBrightness = (int)HIBYTE(wValue);

		// Before we modify the image, make sure we have an undo image
		if (!EditStart(false/*bEditInPlace*/))
			return false;

		CColorMap ColorMap;
		ColorMap.MakeContrastBrightness(iContrast, iBrightness);
		return ColorMap.ProcessImage(m_pDibForUndo, m_pDibForEdit);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::AutoCorrect()
{
	// Before we modify the image, make sure we have an undo image
	if (!EditStart(false/*bEditInPlace*/))
		return false;

	HISTOGRAM Histogram;
	if (!HistogramImage(Histogram, m_pDibForUndo))
		return false;

	int iMinIndex = Histogram.MinIndexGray;
	int iMaxIndex = Histogram.MaxIndexGray;

	CColorMap ColorMap;
	ColorMap.MakeStretch(iMinIndex, iMaxIndex);
	return ColorMap.ProcessImage(m_pDibForUndo, m_pDibForEdit);
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::ColorsTab(int iValue)
{
	if (iValue == EDIT_END)
		iValue = ProcessEndEdit(iValue);

	if (iValue == EDIT_START)
	{
		return true;
	}
	else
	if (iValue == EDIT_APPLY)
	{
		EditApply();
		return true;
	}
	else
	if (iValue == EDIT_RESET || iValue == EDIT_END)
	{
		EditUndo();
		return true;
	}
	else
	if (iValue >= 0)
	{
		WORD wValue = LOWORD(iValue);
		int iAmountValue = (int)HIWORD(iValue);
		int iHueValue = (int)LOBYTE(wValue);
		int iSaturationValue = (int)HIBYTE(wValue);

		// Before we modify the image, make sure we have an undo image
		if (!EditStart(false/*bEditInPlace*/))
			return false;

		CColorMap ColorMap;
		ColorMap.MakeHueSaturation(iAmountValue, iHueValue, iSaturationValue);
		return ColorMap.ProcessImage(m_pDibForUndo, m_pDibForEdit);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::RedEyeTab(int iValue)
{
	if (iValue == EDIT_END)
		iValue = ProcessEndEdit(iValue);

	if (iValue == EDIT_START)
	{
		return true;
	}
	else
	if (iValue == EDIT_APPLY)
	{
		EditApply();
		return true;
	}
	else
	if (iValue == EDIT_RESET || iValue == EDIT_END)
	{
		EditUndo();
		return true;
	}
	else
	if (iValue >= 0)
	{
		return true;
	}

	return true;
}

#ifdef NOTUSED
//////////////////////////////////////////////////////////////////////
bool RedEyeGdip(CDib pDib, CRect& rArea)
{
	if (!CGdip::IsOK())
		return false;

	try
	{
		Status status = Ok;

		RedEyeCorrectionParams redEyeParams;
		redEyeParams.numberOfAreas = 1;
		redEyeParams.areas = &rArea;

		RedEyeCorrection redEye;
		redEye.SetParameters(&redEyeParams);

		Bitmap image((const BITMAPINFO*)pDib, (void*)DibPtr(pDib));
		CheckOk(image.ApplyEffect(&redEye, &rArea));
	}
	catch(Status status)
	{
		return (status == Ok);
	}
	catch(...)
	{
		return false;
	}

	return true;
}
#endif NOTUSED

//////////////////////////////////////////////////////////////////////
bool CImageObject::RedEyeClicked(CPoint Pt, const CToolSettings& ToolSettings)
{
	CDib pDib = m_pDibForEdit;
	if (!pDib)
		return false;

	// Only supports 24 bit Dib's
	if (pDib.BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", pDib.BitCount()));
		return false;
	}

	// Before we modify the image, make sure we have an undo image
	if (!EditStart(false/*bEditInPlace*/))
		return false;

	// Modeled after HitTest()
	m_Matrix.Inverse().Transform(Pt);
	CRect DestRect = GetDestRect();
	CMatrix Matrix;
	double xScale = (double)DibWidth(pDib) / DestRect.Width();
	double yScale = (double)DibHeight(pDib) / DestRect.Height();
	Matrix.Translate(-DestRect.left, -DestRect.top);
	Matrix.Scale(xScale, yScale, 0, 0);
	Matrix.Transform(Pt);
	if (Pt.x < 0 || Pt.x >= (long)DibWidth(pDib) || Pt.y < 0 || Pt.y >= (long)DibHeight(pDib))
		return false;

	// Reduce the size of the rect to search
	int nWidth = DibWidth(pDib) / 10;
	int nHeight = DibHeight(pDib) / 10;
	CRect FillRect;
	FillRect.left = Pt.x - (nWidth/2);
	FillRect.right = FillRect.left + nWidth;
	FillRect.top = Pt.y - (nHeight/2);
	FillRect.bottom = FillRect.top + nHeight;

	CFloodFill FloodFill;
	FloodFill.Init(pDib/*pDibForEdit*/, pDib/*pDibForRead*/, ToolSettings.m_SelectedTool, ToolSettings.m_nTolerance);
	FloodFill.FillDryRun(&FillRect, Pt.x, Pt.y, ToolSettings.m_SelectedTool, CLR_NONE, CLR_NONE);
	FloodFill.Fill(&FillRect, Pt.x, Pt.y, ToolSettings.m_SelectedTool, CLR_NONE, CLR_NONE);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::RecolorTab(int iValue)
{
	if (iValue == EDIT_END)
		iValue = ProcessEndEdit(iValue);

	if (iValue == EDIT_START)
	{
		return true;
	}
	else
	if (iValue == EDIT_APPLY)
	{
		EditApply();
		return true;
	}
	else
	if (iValue == EDIT_RESET || iValue == EDIT_END)
	{
		EditUndo();
		return true;
	}
	else
	if (iValue >= 0)
	{
		return true;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CImageObject::RecolorClicked(CPoint Pt, const CToolSettings& ToolSettings)
{
	CDib pDib = m_pDibForEdit;
	if (!pDib)
		return false;

	// Only supports 24 bit Dib's
	if (pDib.BitCount() != 24)
	{
		CMessageBox::Message(String("Cannot operate on an image that is %d bits deep", pDib.BitCount()));
		return false;
	}

	// Before we modify the image, make sure we have an undo image
	if (!EditStart(false/*bEditInPlace*/))
		return false;

	// Modeled after HitTest()
	m_Matrix.Inverse().Transform(Pt);
	CRect DestRect = GetDestRect();
	CMatrix Matrix;
	double xScale = (double)DibWidth(pDib) / DestRect.Width();
	double yScale = (double)DibHeight(pDib) / DestRect.Height();
	Matrix.Translate(-DestRect.left, -DestRect.top);
	Matrix.Scale(xScale, yScale, 0, 0);
	Matrix.Transform(Pt);
	if (Pt.x < 0 || Pt.x >= (long)DibWidth(pDib) || Pt.y < 0 || Pt.y >= (long)DibHeight(pDib))
		return false;

	COLORREF Color1 = ToolSettings.m_PrimaryColor;
	COLORREF Color2 = ToolSettings.m_SecondaryColor;

	CRect FillRect = DestRect;

	CFloodFill FloodFill;
	FloodFill.Init(pDib/*pDibForEdit*/, m_pDibForUndo/*pDibForRead*/, ToolSettings.m_SelectedTool, ToolSettings.m_nTolerance);
	FloodFill.FillDryRun(&FillRect, Pt.x, Pt.y, ToolSettings.m_SelectedTool, Color1, Color2);
	FloodFill.Fill(&FillRect, Pt.x, Pt.y, ToolSettings.m_SelectedTool, Color1, Color2);
	return true;
}
