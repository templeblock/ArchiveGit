#ifndef _DIB_H_
#define _DIB_H_

#ifndef BI_BITFIELDS
	#define BI_BITFIELDS 3
#endif	

#ifndef HALFTONE
	#define HALFTONE COLORONCOLOR
#endif

class CDib;
typedef CDib* PDIB;
typedef PDIB* LPPDIB;
typedef RGBTRIPLE FAR* LPRGBTRIPLE;
typedef BYTE* HPTR;
typedef char FNAME[256];

class CDib
{
public:
	// constructors/destructor
	CDib();
	CDib(CDib* pDib, HPTR lp, BOOL fCopyBits = TRUE);
	CDib(LPBITMAPINFOHEADER lpbmiHeader, LPRGBQUAD lpColors, HPTR lp);
	~CDib();

	// Reference count management
	int AddRef();
	int Release();

	// Function to get dib, this function handles decoding RLE's and
	// returning a new dib if necessary
	CDib * GetDib(BOOL fDecodeRLE = TRUE);

	// static dib load function
	static CDib * LoadDibFromFile(LPCSTR szFile, HPALETTE hPal, BOOL bMapToLevels = FALSE, BOOL fDecodeRLE = FALSE, BOOL bBrowse = TRUE);
	static CDib * LoadDibFromResource(HINSTANCE hInstance, LPCSTR szResource, HPALETTE hPal, BOOL bMapToLevels = FALSE, BOOL fDecodeRLE = FALSE);

	// DIB I/O
	BOOL LoadFromFile(LPCSTR szFile, BOOL fDecodeRLE = FALSE, BOOL bBrowse = TRUE);
	BOOL WriteFile( LPCSTR szFile );
	BOOL LoadFromResource(HINSTANCE hInstance, LPCSTR szResource, BOOL fDecodeRLE = FALSE);

	// DIB creation
	BOOL Create( int bits, int dx, int dy );
	BOOL Create( HBITMAP hbm, DWORD biStyle, UINT biBits, HPALETTE hpal, UINT wUsage );
	BOOL Create( CDib * pDib);
	class CDib * DibCopy(BOOL fCopyBits = TRUE);
	HBITMAP CreateBitmap( HPALETTE hpal, UINT wUsage );
	class CDib * HalftoneDIB( HPALETTE hPal );

	// DIB manipulation
	int MapToPalette( HPALETTE hpal, HPTR lpInUse = NULL ); 
	void ConvertToLevels();
	BOOL SetColorTable( HPALETTE hpal, UINT wUsage ); 
	void CopyColorTable( CDib * pdibSrc );
	BOOL Dwindle();
	void SetPlanes(WORD biPlanes) { m_bmiHeader.biPlanes = biPlanes; }
	void SetSizeImage(DWORD biSizeImage) { m_bmiHeader.biSizeImage = biSizeImage; }
	void SetColorsUsed(DWORD biClrUsed) { m_bmiHeader.biClrUsed = biClrUsed; }
	void SetColorsImportant(DWORD biClrImportant) { m_bmiHeader.biClrImportant = biClrImportant; }
	void SetWidth(LONG biWidth) { m_bmiHeader.biWidth = biWidth; }
	void SetHeight(LONG biHeight) { m_bmiHeader.biHeight = biHeight; }
	void SetBitCount(WORD biBitCount) { m_bmiHeader.biBitCount = biBitCount; }
	void SetCompression(DWORD biCompression) { m_bmiHeader.biCompression = biCompression; }
	void SetPtr(HPTR lp) { m_lp = lp; }
	void FixHeader();
	BOOL DibFlip();
	void SwapLines( int y1, int y2, int iCount );
			
	// DIB drawing			 
	BOOL DibClipRect( CDib * pdibDst, LPRECT prDst, LPRECT prDstClip,
					  CDib * pdibSrc, LPRECT prSrc, LPRECT prSrcClip );
	BOOL DibClip( CDib * pdibDst, LPINT pdstLeft, LPINT pdstTop, LPINT pdstWidth, LPINT pdstHeight,
				  CDib * pdibSrc, LPINT psrcLeft, LPINT psrcTop, LPINT psrcWidth, LPINT psrcHeight );
	void DibBlt(CDib * pdibDst, int dstLeft, int dstTop, int dstWidth, int dstHeight,
				  int srcLeft, int srcTop, int srcWidth, int srcHeight,
				  BOOL bTransparent, LPRGBTRIPLE lpRGB = NULL );
	void DCBlt(	 HDC hDC, int dstLeft, int dstTop, int dstWidth, int dstHeight,
				int srcLeft, int srcTop, int srcWidth, int srcHeight, DWORD rop = SRCCOPY );

	// DIB access
	LPBITMAPINFO GetInfo() { return (LPBITMAPINFO)&m_bmiHeader; }
	LPBITMAPINFOHEADER GetInfoHeader() { return &m_bmiHeader; }
	int GetWidth() { return (int) m_bmiHeader.biWidth; }
	int GetHeight() { return (int) m_bmiHeader.biHeight; }
	UINT GetBitCount() { return (UINT)m_bmiHeader.biBitCount; }
	DWORD GetCompression() { return m_bmiHeader.biCompression; }
	LPRGBQUAD GetColors() { return m_rgbQuad; }
	HPTR GetPtr() { return m_lp; }

	HPTR GetXY( int x, int y );
	HPTR GetXY_RLE( int x, int y );
	UINT GetWidthBytes();
	DWORD GetSizeImage();
	UINT GetNumColors();
	UINT GetColorTableType();
	int FlipY(int y );
	BOOL Print(HWND hWnd, HINSTANCE hInstance, int idPrintDlg, LPSTR lpszDocName);

protected:
	BOOL ReadBitmapInfo( HFILE fh );
	BOOL CheckDecodeRLE(BOOL fDecodeRLE);
	void DecodeRLELine(int yTarget, HPTR pb);
	void DecodeRLE(HPTR pb);
	BOOL BandToPrinter (HDC hPrnDC, LPRECT lpPrintRect);
	BOOL PrintABand (HDC hDC, LPRECT lpRectOut, LPRECT lpRectClip,
						BOOL fDoText, BOOL fDoGraphics);
	BOOL DeviceSupportsEscape (HDC hDC, int nEscapeCode);
	void GetPrintRect (HDC hDC, LPRECT lpPrintRect);
	HDC GetPrinterDC (void);

protected:
	BITMAPINFOHEADER m_bmiHeader;
	RGBQUAD m_rgbQuad[256];
	HPTR m_lp;
	int m_iRefCount;
	struct {
		int x;
		int y;
		int yInBuf;
		HPTR pBuf;
		HPTR pSrc;
	} Decode;
};

#endif // _DIB_H_
