#ifndef _DIB_H_
#define _DIB_H_

#ifndef BI_BITFIELDS
	#define BI_BITFIELDS 3
#endif	

#ifndef HALFTONE
	#define HALFTONE COLORONCOLOR
#endif

class FAR CDib
{
public:
	// constructors/destructor
	CDib();
	CDib(class CDib FAR *, LPTR lp, BOOL fCopyBits = TRUE);
	CDib(LPBITMAPINFOHEADER lpbmiHeader, LPRGBQUAD lpColors, LPTR lp);
	~CDib();

	// Reference count management
	AddRef();
	Release();

	// Function to get dib, this function handles decoding RLE's and
	// returning a new dib if necessary
	CDib FAR *GetDib(BOOL fDecodeRLE = TRUE);

	// static dib load function
	static CDib FAR *LoadDibFromFile(LPCSTR szFile, HPALETTE hPal, BOOL bMapToLevels = FALSE, BOOL fDecodeRLE = FALSE);
	static CDib FAR *LoadDibFromResource(HINSTANCE hInstance, LPCSTR szResource, HPALETTE hPal, BOOL bMapToLevels = FALSE, BOOL fDecodeRLE = FALSE);

	// DIB I/O
	BOOL LoadFromFile(LPCSTR szFile, BOOL fDecodeRLE = FALSE);
	BOOL WriteFile( LPCSTR szFile );
	BOOL LoadFromResource(HINSTANCE hInstance, LPCSTR szResource, BOOL fDecodeRLE = FALSE);

	// DIB creation
	BOOL Create( int bits, int dx, int dy );
	BOOL Create( HBITMAP hbm, DWORD biStyle, UINT biBits, HPALETTE hpal, UINT wUsage );
	BOOL Create( CDib FAR *pDib);
	class CDib FAR * Convert( int BitCount, DWORD biCompression );
	class CDib FAR * DibCopy(BOOL fCopyBits = TRUE);
	HBITMAP CreateBitmap( HPALETTE hpal, UINT wUsage );
	class CDib FAR * HalftoneDIB( HPALETTE hPal );

	// DIB manipulation
	HPALETTE CreatePalette();
	void MapToPalette( HPALETTE hpal ); 
	void ConvertToLevels();
	BOOL SetColorTable( HPALETTE hpal, UINT wUsage ); 
	void CopyColorTable( CDib FAR *pdibSrc );
	BOOL Dwindle();
	void SetPlanes(WORD biPlanes) { m_bmiHeader.biPlanes = biPlanes; }
	void SetSizeImage(DWORD biSizeImage) { m_bmiHeader.biSizeImage = biSizeImage; }
	void SetColorsUsed(DWORD biClrUsed) { m_bmiHeader.biClrUsed = biClrUsed; }
	void SetColorsImportant(DWORD biClrImportant) { m_bmiHeader.biClrImportant = biClrImportant; }
	void SetWidth(LONG biWidth) { m_bmiHeader.biWidth = biWidth; }
	void SetHeight(LONG biHeight) { m_bmiHeader.biHeight = biHeight; }
	void SetBitCount(WORD biBitCount) { m_bmiHeader.biBitCount = biBitCount; }
	void SetCompression(DWORD biCompression) { m_bmiHeader.biCompression = biCompression; }
	void SetPtr(LPTR lp) { m_lp = lp; }
	void FixHeader();
	BOOL DibFlip();
	void SwapLines( int y1, int y2, int iCount );
			
	// DIB drawing			 
	void DibBlt(class CDib FAR * pdibDst, int dstLeft, int dstTop, int dstWidth, int dstHeight,
				  int srcLeft, int srcTop, int srcWidth, int srcHeight,
				  BOOL bTransparent, LPRGBTRIPLE lpRGB = NULL, LPTR lpLut = NULL, HPALETTE hPal = NULL );
	void DCBlt(	 HDC hDC, int dstLeft, int dstTop, int dstWidth, int dstHeight,
				int srcLeft, int srcTop, int srcWidth, int srcHeight, DWORD rop = SRCCOPY );

	// DIB access
	LPBITMAPINFO FAR GetInfo() { return (LPBITMAPINFO)&m_bmiHeader; }
	LPBITMAPINFOHEADER FAR GetInfoHeader() { return &m_bmiHeader; }
	int GetWidth() { return (int) m_bmiHeader.biWidth; }
	int GetHeight() { return (int) m_bmiHeader.biHeight; }
	UINT GetBitCount() { return (UINT)m_bmiHeader.biBitCount; }
	DWORD GetCompression() { return m_bmiHeader.biCompression; }
	LPRGBQUAD GetColors() { return m_rgbQuad; }
	LPTR GetPtr() { return m_lp; }

	HPTR GetXY( int x, int y );
	UINT GetWidthBytes();
	DWORD GetSizeImage();
	UINT GetNumColors();
	UINT GetColorTableType();
	int FlipY(int y );

protected:
	BOOL ReadBitmapInfo( HFILE fh );
	BOOL CheckDecodeRLE(BOOL fDecodeRLE);
	void DecodeRLE(HPTR pb);

protected:
	BITMAPINFOHEADER	m_bmiHeader;
	RGBQUAD				m_rgbQuad[256];
	LPTR				m_lp;
	int					m_iRefCount;
};

typedef CDib FAR * PDIB;
typedef PDIB FAR * LPPDIB;

#endif // _DIB_H_
