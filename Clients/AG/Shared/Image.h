#pragma once

// Because this will be linked into an ATL library, force the "consumer" to
// address CString's as CAtlString's so we can use the library in an MFC app
#ifdef _AFX
	#include <atlstr.h>
	#define CStringW ATL::CAtlStringW
	#define CStringA ATL::CAtlStringA
	#define CString ATL::CAtlString
#endif

class CDibSectionDC
{
public:
	CDibSectionDC(const BITMAPINFO& BitmapInfo, UINT iUsage, HDC hCompatibleDC = NULL);
	~CDibSectionDC();

	void Init();
	HANDLE ExtractDibGlobal();
	HDC GetHDC() const
		{ return m_hDC; }

protected:
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	BITMAPINFO m_BitmapInfo;
	BYTE* m_pBits;
	HDC m_hDC;
};

class CImage
{
public:
	CImage(HGLOBAL hNativeMemory);
	CImage(LPCSTR strFileName);
	CImage(HINSTANCE hInstance, int idResource, LPCSTR strResourceType);
	~CImage();
	
	bool GetNativeImageSize(long& lWidth, long& lHeight);
	HGLOBAL GetNativeImageHandle();
	HGLOBAL GetDibHandle(bool bTakeOver = false);
	HBITMAP GetBitmapHandle(bool bTakeOver = false);
	BITMAPINFOHEADER* GetDibPtr();
	IPicture* GetPicture();
	static bool IsGdiPlusInstalled();

private:
	static HGLOBAL ReadNativeImage(LPCSTR strFileName);
	static HGLOBAL ReadNativeImage(HINSTANCE hInstance, int idResource, LPCSTR strResourceType);
	static IPicture* NativeImageToPicture(HGLOBAL hMemory);
	static IPicture* NativeImageToPictureWithGdiPlus(IStream* pStream);
	static bool PictureGetSize(IPicture* pPicture, long& lWidth, long& lHeight, long& hmWidth, long& hmHeight);
	static HBITMAP PictureToBitmap(IPicture* pPicture);
	static HGLOBAL PictureToDib(IPicture* pPicture);

private:
	HGLOBAL m_hNativeMemory;
	HGLOBAL m_hDIB;
	BITMAPINFOHEADER* m_pDIB;
	HBITMAP m_hBitmap;
	IPicture* m_pPicture;
	bool m_bOwnNativeMemory;
};

#ifdef _AFX
	#undef CStringW
	#undef CStringA
	#undef CString
#endif
