// maskedbm.h : header file
//

#ifndef _MASKEDBM_H_
#define _MASKEDBM_H_

class CMaskedBitmap
{
// Construction
public:
	CMaskedBitmap();

// Operations
public:
	virtual BOOL LoadBitmap(LPCSTR lpszResource, COLORREF crBackgroundColor, CDC *pDC = NULL);
	virtual BOOL DeleteObject();
	virtual CBitmap *GetImage() { return &m_bmImage; }
	virtual CBitmap *GetMask() { return &m_bmMask; }
	virtual GetWidth() { return m_nBitmapWidth; }
	virtual GetHeight() { return m_nBitmapHeight; }

// Implementation
public:
	virtual ~CMaskedBitmap();

private:
	int m_nBitmapWidth;
	int m_nBitmapHeight;
	
	CBitmap m_bmImage;
	CBitmap m_bmMask;
	
	BOOL m_fBitmapsLoaded;
};

/////////////////////////////////////////////////////////////////////////////

#endif

