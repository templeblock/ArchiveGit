/////////////////////////////////////////////////////////////////////////////
//
// transbmp.h : interface of the CTransBitmap class
//
/////////////////////////////////////////////////////////////////////////////

class CTransBmp : public CBitmap
{
public:
    CTransBmp();
    ~CTransBmp();
    void Draw(HDC hDC, int x, int y);
    void Draw(CDC* pDC, int x, int y);
    void DrawTrans(HDC  hDC, int x, int y, int cx, int cy, int dx, int dy);
	void DrawTransStretched(HDC hDC, int srcX, int srcY, int destX, int destY, int destWidth, int destHeight);
    int GetWidth();
    int GetHeight();

//private:
    int m_iWidth;
    int m_iHeight;
    HBITMAP m_hbmMask;    // handle to mask bitmap
    HBITMAP m_hbmOffScr;
    HDC     m_hdcMask;
    HDC     m_hdcImage;
    HDC     m_hdcOffScr;

    void GetMetrics();
    void CreateMask(HDC hDC);
};

/////////////////////////////////////////////////////////////////////////////
