#include "stdafx.h"
#include "Image.h"
#include "Dib.h"

#define HIMETRIC_INCH 2540

//////////////////////////////////////////////////////////////////////
CDibSectionDC::CDibSectionDC(const BITMAPINFO& BitmapInfo, UINT iUsage, HDC hCompatibleDC)
{
	m_hDC = ::CreateCompatibleDC(hCompatibleDC);
	m_BitmapInfo = BitmapInfo;
	m_pBits = NULL;

	if (m_hDC)
	{
		// When using CreateDIBSection() with DIB_PAL_COLORS, the GDI will use the color indices in the
		// bmiColors to populate the color table of the DIB section by resolving the indices to the RGB
		// values from the palette selected into the device context passed into CreateDIBSection().
		// Consequently, after a program calls CreateDIBSection(), the color table will contain RGB
		// values rather than palette indices. Specifying DIB_PAL_COLORS affects only how the color
		// table is initialized, not how it is subsequently used.
		m_hBitmap = ::CreateDIBSection(m_hDC, &m_BitmapInfo, iUsage, (void**)&m_pBits, NULL, 0);
		m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_hBitmap);

	}
	else
	{
		m_hBitmap = NULL;
		m_hOldBitmap = NULL;
	}

	Init();
}

//////////////////////////////////////////////////////////////////////
CDibSectionDC::~CDibSectionDC()
{
	if (m_hDC)
	{
		::SelectObject(m_hDC, m_hOldBitmap);
		::DeleteDC(m_hDC);
		m_hDC = NULL;
	}

	if (m_hBitmap)
		::DeleteObject(m_hBitmap);
}

//////////////////////////////////////////////////////////////////////
void CDibSectionDC::Init()
{
	if (!m_hDC)
		return;

	::SetMapMode(m_hDC, MM_TEXT);
	::SetTextAlign(m_hDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
	::SetBkMode(m_hDC, TRANSPARENT);
	::SetStretchBltMode(m_hDC, COLORONCOLOR);
	::SetPolyFillMode(m_hDC, WINDING); // vs. the default ALTERNATE

	// Fill background 
	RECT DibRect = {0, 0, m_BitmapInfo.bmiHeader.biWidth, m_BitmapInfo.bmiHeader.biHeight};
	::FillRect(m_hDC, &DibRect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
}

//////////////////////////////////////////////////////////////////////
HANDLE CDibSectionDC::ExtractDibGlobal()
{
	if (!m_pBits)
		return NULL;

	return DibCopyGlobal(&m_BitmapInfo.bmiHeader, m_pBits);
}

/////////////////////////////////////////////////////////////////////////////
CImage::CImage(HGLOBAL hNativeMemory)
{
	m_hNativeMemory = hNativeMemory;
	m_bOwnNativeMemory = false;
	m_hDIB = NULL;
	m_pDIB = NULL;
	m_hBitmap = NULL;
	m_pPicture = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CImage::CImage(LPCSTR strFileName)
{
	m_hNativeMemory = ReadNativeImage(strFileName);
	m_bOwnNativeMemory = true;
	m_hDIB = NULL;
	m_pDIB = NULL;
	m_hBitmap = NULL;
	m_pPicture = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CImage::CImage(HINSTANCE hInstance, int idResource, LPCSTR strResourceType)
{
	m_hNativeMemory = ReadNativeImage(hInstance, idResource, strResourceType);
	m_bOwnNativeMemory = true;
	m_hDIB = NULL;
	m_pDIB = NULL;
	m_hBitmap = NULL;
	m_pPicture = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CImage::~CImage()
{
	if (m_pPicture)
		m_pPicture->Release();

	if (m_hBitmap)
		::DeleteObject(m_hBitmap);
	
	if (m_pDIB)
		::GlobalUnlock(m_hDIB);

	if (m_hDIB)
		::GlobalFree(m_hDIB);

	if (m_hNativeMemory && m_bOwnNativeMemory)
		::GlobalFree(m_hNativeMemory);
}

/////////////////////////////////////////////////////////////////////////////
bool CImage::GetNativeImageSize(long& lWidth, long& lHeight)
{
	if (!m_hNativeMemory)
		return false;
	if (!m_pPicture)
		m_pPicture = NativeImageToPicture(m_hNativeMemory);
	
	long hmWidth = 0;
	long hmHeight = 0;
	return PictureGetSize(m_pPicture, lWidth, lHeight, hmWidth, hmHeight);
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CImage::GetNativeImageHandle()
{
	return m_hNativeMemory;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CImage::GetDibHandle(bool bTakeOver)
{
	if (!m_hNativeMemory)
		return NULL;
	if (!m_pPicture)
		m_pPicture = NativeImageToPicture(m_hNativeMemory);
	if (!m_hDIB)
		m_hDIB = PictureToDib(m_pPicture);
	
	HGLOBAL hDIB = m_hDIB;
	if (bTakeOver)
	{
		if (m_pDIB)
			::GlobalUnlock(m_hDIB);
		m_pDIB = NULL;
		m_hDIB = NULL;
	}

	return hDIB;
}

/////////////////////////////////////////////////////////////////////////////
HBITMAP CImage::GetBitmapHandle(bool bTakeOver)
{
	if (!m_hNativeMemory)
		return NULL;
	if (!m_pPicture)
		m_pPicture = NativeImageToPicture(m_hNativeMemory);
	if (!m_hBitmap)
		m_hBitmap = PictureToBitmap(m_pPicture);

	HBITMAP hBitmap = m_hBitmap;
	if (bTakeOver)
		m_hBitmap = NULL;
	return hBitmap;
}

/////////////////////////////////////////////////////////////////////////////
BITMAPINFOHEADER* CImage::GetDibPtr()
{
	if (!m_hNativeMemory)
		return NULL;
	if (!m_pPicture)
		m_pPicture = NativeImageToPicture(m_hNativeMemory);
	if (!m_hDIB)
		m_hDIB = PictureToDib(m_pPicture);
	if (!m_pDIB)
		m_pDIB = (BITMAPINFOHEADER*)::GlobalLock(m_hDIB);
	return m_pDIB;
}

/////////////////////////////////////////////////////////////////////////////
IPicture* CImage::GetPicture()
{
	if (!m_hNativeMemory)
		return NULL;
	if (!m_pPicture)
		m_pPicture = NativeImageToPicture(m_hNativeMemory);
	return m_pPicture;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CImage::ReadNativeImage(LPCSTR strFileName)
{
	// Get the image from a file, whatever the type
	HANDLE hFile = ::CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	// Get the file size
	DWORD dwSize = ::GetFileSize(hFile, NULL);
	if (dwSize <= 0)
	{
		::CloseHandle(hFile);
		return NULL;
	}

	// Allocate memory based on file size
	HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (!hMemory)
	{
		::CloseHandle(hFile);
		return NULL;
	}

	// Lock down the memory and get a pointer
	char* pMemory = (char*)::GlobalLock(hMemory);
	if (!pMemory)
	{
		::GlobalFree(hMemory);
		::CloseHandle(hFile);
		return NULL;
	}

	// Read the file and store it in memory
	DWORD dwBytesRead = 0;
	if (!::ReadFile(hFile, pMemory, dwSize, &dwBytesRead, NULL))
	{
		::GlobalUnlock(hMemory);
		::GlobalFree(hMemory);
		::CloseHandle(hFile);
		return NULL;
	}

	::GlobalUnlock(hMemory);
	::CloseHandle(hFile);

	return hMemory;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CImage::ReadNativeImage(HINSTANCE hInstance, int idResource, LPCSTR strResourceType)
{
	// Get the image from a resource, whatever the type
	HRSRC hResource = ::FindResource(hInstance, MAKEINTRESOURCE(idResource), strResourceType);
	if (!hResource)
		return NULL;

	// Get the resource size
	DWORD dwSize = ::SizeofResource(hInstance, hResource);
	if (dwSize <= 0)
		return NULL;

	HGLOBAL hResMemory = ::LoadResource(hInstance, hResource);
	if (!hResMemory)
		return NULL;

	// Lock down the resource memory and get a pointer
	char* pResMemory = (char*)::LockResource(hResMemory);
	if (!pResMemory)
	{
		::FreeResource(hResMemory);
		return NULL;
	}

	// Allocate memory based on the size of the resource
	HGLOBAL hMemory = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (!hMemory)
	{
		UnlockResource(hResMemory);
		::FreeResource(hResMemory);
		return NULL;
	}

	// Lock down the memory and get a pointer
	char* pMemory = (char*)::GlobalLock(hMemory);
	if (!pMemory)
	{
		::GlobalFree(hMemory);
		UnlockResource(hResMemory);
		::FreeResource(hResMemory);
		return NULL;
	}

	// Copy the resource data into the memory block
	::CopyMemory(pMemory, pResMemory, dwSize);

	::GlobalUnlock(hMemory);
	UnlockResource(hResMemory);
	::FreeResource(hResMemory);

	return hMemory;
}

/////////////////////////////////////////////////////////////////////////////
IPicture* CImage::NativeImageToPicture(HGLOBAL hMemory)
{
	if (!hMemory)
		return NULL;

	// Create an IStream from the file in memory
	IStream* pStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(hMemory, false/*fDeleteOnRelease*/, &pStream);
	if (FAILED(hr) || !pStream)
		return NULL;

	// Create an IPicture from the IStream; the function opens JPG's, BMP's, GIF's, WMF's, EMF's, and ICO's!!
	DWORD dwSize = (DWORD)::GlobalSize(hMemory);
	IPicture* pPicture = NULL;
	hr = ::OleLoadPicture(pStream, dwSize, false, IID_IPicture, (void**)&pPicture);

	if (FAILED(hr) || !pPicture)
		pPicture = NativeImageToPictureWithGdiPlus(pStream);

	// Either way we are done with the IStream and the memory
	pStream->Release();

	return pPicture;
}

// No need to create an instance of the CGdip object
// Just call CGdip::IsOK() and then use GDI+ classes
#include "Gdip.h" 

/////////////////////////////////////////////////////////////////////////////
IPicture* CImage::NativeImageToPictureWithGdiPlus(IStream* pStream)
{
	if (!pStream)
		return NULL;

	if (!CGdip::IsOK())
		return false;

	// Create a GDI+ bitmap from the image stream
	GpBitmap* pBitmap = NULL;
	GpStatus status = GdipCreateBitmapFromStream(pStream, &pBitmap);
	if (status != Ok || !pBitmap)
		return NULL;

	// Create a GDI bitmap from the GDI+ bitmap
	HBITMAP hBitmap = NULL;
	status = GdipCreateHBITMAPFromBitmap(pBitmap, &hBitmap, 0);
	if (status != Ok || !pBitmap)
	{
		status = GdipDisposeImage(pBitmap);
		return NULL;
	}

	// Initialize the PICTDESC structure
	PICTDESC PictDesc;
	::ZeroMemory(&PictDesc, sizeof(PictDesc));
	PictDesc.cbSizeofstruct = sizeof(PictDesc);
	PictDesc.picType = PICTYPE_BITMAP;
	PictDesc.bmp.hbitmap = hBitmap;
	PictDesc.bmp.hpal = NULL/*hPal*/;

	// Create the Picture object
	IPicture* pPicture = NULL;
	::OleCreatePictureIndirect(&PictDesc, IID_IPicture, true/*fOwn*/, (LPVOID*)&pPicture);
	
	status = GdipDisposeImage(pBitmap);

	return pPicture;
}

/////////////////////////////////////////////////////////////////////////////
bool CImage::IsGdiPlusInstalled()
{
	HMODULE hGdiPlusLib = ::LoadLibrary("gdiplus.dll");
	if (!hGdiPlusLib)
		return false;

	::FreeLibrary(hGdiPlusLib);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CImage::PictureGetSize(IPicture* pPicture, long& lWidth, long& lHeight, long& hmWidth, long& hmHeight)
{
	if (!pPicture)
		return false;

	// Get the screen dimensions for the HIMETRIC conversion
	HDC hDC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
	long lResolutionX = GetDeviceCaps(hDC, LOGPIXELSX);
	long lResolutionY = GetDeviceCaps(hDC, LOGPIXELSY);
	::DeleteDC(hDC);

	// Get width and height of the picture
	hmWidth = 0;
	HRESULT hr = pPicture->get_Width(&hmWidth);
	lWidth	= ::MulDiv(hmWidth, lResolutionX, HIMETRIC_INCH);

	hmHeight = 0;
	hr = pPicture->get_Height(&hmHeight);
	lHeight	= ::MulDiv(hmHeight, lResolutionY, HIMETRIC_INCH);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
HBITMAP CImage::PictureToBitmap(IPicture* pPicture)
{
	if (!pPicture)
		return NULL;

	short PicType = 0;
	pPicture->get_Type(&PicType);

	HBITMAP hBitmap = NULL;
	if (PicType == PICTYPE_BITMAP)
		pPicture->get_Handle((OLE_HANDLE*)&hBitmap);
	
	return hBitmap;
}

/////////////////////////////////////////////////////////////////////////////
HGLOBAL CImage::PictureToDib(IPicture* pPicture)
{
	if (!pPicture)
		return NULL;

	long lWidth = 0;
	long lHeight = 0;
	long hmWidth = 0;
	long hmHeight = 0;
	if (!PictureGetSize(pPicture, lWidth, lHeight, hmWidth, hmHeight))
		return NULL;

	// Get the picture type
	short PicType = PICTYPE_UNINITIALIZED;
	HRESULT hr = pPicture->get_Type(&PicType);

//j	OLE_HANDLE hPal = NULL;
//j	hr = pPicture->get_hPal(&hPal);
//j	LOGPALETTE* pPal = (LOGPALETTE*)::GlobalLock(hPal);
//j	::GlobalUnlock(hPal);
//j	::GlobalFree(hPal);
	
	// Create an empty DIB section to write to it as a DC
	BITMAPINFO bi;
	::ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = lWidth;
	bi.bmiHeader.biHeight = lHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biXPelsPerMeter = 3937; // 100 pixels/inch
	bi.bmiHeader.biYPelsPerMeter = 3937;
	CDibSectionDC dc(bi, DIB_RGB_COLORS);
	if (!dc.GetHDC())
		return NULL;

	// Render the picture into the DIB
	hr = pPicture->Render(
		dc.GetHDC(),// HDC the device context on which to render the image
		0,			// long left position of destination in HDC
		0,			// long top position of destination in HDC
		lWidth,		// long width of destination in HDC
		lHeight,	// long height of destination in HDC
		0,			// OLE_XPOS_HIMETRIC left position in source picture
		hmHeight,	// OLE_YPOS_HIMETRIC top position in source picture
		hmWidth,	// OLE_XSIZE_HIMETRIC width in source picture
		-hmHeight,	// OLE_YSIZE_HIMETRIC height in source picture
		NULL		// LPCRECT pointer to destination for a metafile hdc
	);

	// Return a copy of the DibSection's Dib
	return dc.ExtractDibGlobal();
}

