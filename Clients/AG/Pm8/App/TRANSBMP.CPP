/* 
   @doc
   @module TransBMP.cpp - Transparent bitmap class implementation |
   
    Implementation for the CTransBMP class. This class is used for Drawing
    DDB's to a device context using the (0,0) pixel in the bitmap as its
    transparent color.
   
   <cp>Copyright 1995 Brøderbund Software, Inc., all rights reserved
*/

/*	Modifications

	1/26/99	LWu	Added DrawTransStretched() to use StretchBlt 
*/

#include "stdafx.h"
#include "transbmp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Colors
#define rgbWhite RGB(255,255,255)

/////////////////////////////////////////////////////////////////////////////
// CTransBmp construction/destruction

CTransBmp::CTransBmp()
{
    m_iWidth    = 0;
    m_iHeight   = 0;
    m_hbmMask   = NULL;
    m_hbmOffScr = NULL;
    m_hdcMask   = NULL;
    m_hdcImage  = NULL;
    m_hdcOffScr = NULL;
}

CTransBmp::~CTransBmp()
{
    if (m_hbmMask != NULL)
    {
        ::DeleteObject(m_hbmMask);
        m_hbmMask = NULL;
    }
    if (m_hdcMask != NULL)
    {
        ::DeleteDC(m_hdcMask);
        m_hdcMask = NULL;
    }
    if (m_hdcImage != NULL)
    {
        ::DeleteDC(m_hdcImage);
        m_hdcImage = NULL;
    }
    if (m_hbmOffScr != NULL)
    {
        ::DeleteObject(m_hbmOffScr);
        m_hbmOffScr = NULL;
    }
    if (m_hdcOffScr != NULL)
    {
        ::DeleteDC(m_hdcOffScr);
        m_hdcOffScr = NULL;
    }

}

void CTransBmp::GetMetrics()
{
    // Get the width and height
    BITMAP bm;
    GetObject(sizeof(bm), &bm);
    m_iWidth = bm.bmWidth;
    m_iHeight = bm.bmHeight;
}


int CTransBmp::GetWidth()
{
    if ((m_iWidth == 0) || (m_iHeight == 0)){
        GetMetrics();
    }
    return m_iWidth;
}

int CTransBmp::GetHeight()
{
    if ((m_iWidth == 0) || (m_iHeight == 0)){
        GetMetrics();
    }
    return m_iHeight;
}

void CTransBmp::Draw(HDC hDC, int x, int y)
{
    ASSERT(hDC);
    // Create a memory DC
    HDC hdcMem = ::CreateCompatibleDC(hDC);
    // Select the bitmap into the mem DC
    HBITMAP hbmold = 
        (HBITMAP)::SelectObject(hdcMem, (HBITMAP)(m_hObject));
    // Blt the bits
    ::BitBlt(hDC,
             x, y,
             GetWidth(), GetHeight(),
             hdcMem,
             0, 0,
             SRCCOPY);
    ::SelectObject(hdcMem, hbmold);
    ::DeleteDC(hdcMem); 
}

void CTransBmp::Draw(CDC* pDC, int x, int y)
{
    ASSERT(pDC);
    HDC hDC = pDC->GetSafeHdc();
    Draw(hDC, x, y);
}

void CTransBmp::CreateMask(HDC hDC)
{
    // Nuke any existing mask
    if (m_hbmMask) 
    {
        ::DeleteObject(m_hbmMask);
    }

    // Create memory DCs to work with
    HDC hdcMask  = ::CreateCompatibleDC(hDC);
    HDC hdcImage = ::CreateCompatibleDC(hDC);

    // Create a monochrome bitmap for the mask
    m_hbmMask = ::CreateBitmap(GetWidth(),
                               GetHeight(),
                               1,
                               1,
                               NULL);

    // Select the mono bitmap into its DC
    HBITMAP hbmOldMask = (HBITMAP)::SelectObject(hdcMask, m_hbmMask);
    // Select the image bitmap into its DC
    HBITMAP hbmOldImage = (HBITMAP)::SelectObject(hdcImage, m_hObject);
    // Set the transparency color to be the top-left pixel
    ::SetBkColor(hdcImage, ::GetPixel(hdcImage, 0, 0));
    // Make the mask
    ::BitBlt(hdcMask,
             0, 0,
             GetWidth(), GetHeight(),
             hdcImage,
             0, 0,
             SRCCOPY);
    // Tidy up
    ::SelectObject(hdcMask, hbmOldMask);
    ::SelectObject(hdcImage, hbmOldImage);
    ::DeleteDC(hdcMask);
    ::DeleteDC(hdcImage);
}


void CTransBmp::DrawTransStretched(HDC hDC, int srcX, int srcY, int destX, int destY, int destWidth, int destHeight)
{
    if (m_hbmMask == NULL)
        CreateMask(hDC);

    int srcWidth   = GetWidth();
    int srcHeight  = GetHeight();

    // Create a memory DC to do the drawing to
    if (m_hdcOffScr == NULL)
        m_hdcOffScr = ::CreateCompatibleDC(hDC);

    // Create a bitmap for the off-screen DC that is really
    // color compatible with the destination DC.
    if (m_hbmOffScr == NULL)
    {
        m_hbmOffScr = ::CreateBitmap( srcWidth, 
                                      srcHeight, 
                               (BYTE) GetDeviceCaps(hDC, PLANES),
                               (BYTE) GetDeviceCaps(hDC, BITSPIXEL),
                                      NULL );
    }

    // Select the buffer bitmap into the off-screen DC
    HBITMAP hbmOldOffScr = (HBITMAP)::SelectObject(m_hdcOffScr, m_hbmOffScr);

/*BOOL StretchBlt(  HDC hdcDest,      // handle to destination device context
  int nXOriginDest, // x-coordinate of upper-left corner of dest. rectangle
  int nYOriginDest, // y-coordinate of upper-left corner of dest. rectangle
  int nWidthDest,   // width of destination rectangle
  int nHeightDest,  // height of destination rectangle
  HDC hdcSrc,       // handle to source device context
  int nXOriginSrc,  // x-coordinate of upper-left corner of source rectangle
  int nYOriginSrc,  // y-coordinate of upper-left corner of source rectangle
  int nWidthSrc,    // width of source rectangle
  int nHeightSrc,   // height of source rectangle
  DWORD dwRop       // raster operation code);
*/  
  
	// Copy the image of the destination rectangle to the
    // off-screen buffer DC so we can play with it
    ::StretchBlt(m_hdcOffScr, 0, 0, destWidth, destHeight, 
			hDC, srcX, srcY, destWidth, destHeight, SRCCOPY);

    // Create a memory DC for the source image
    if ( m_hdcImage == NULL )
    {
        m_hdcImage    = ::CreateCompatibleDC(hDC); 
    }
    HBITMAP hbmOldImage = (HBITMAP)::SelectObject(m_hdcImage, m_hObject);

    // Create a memory DC for the mask
    if ( m_hdcMask == NULL )
    {
        m_hdcMask     = ::CreateCompatibleDC(hDC);
    }
    HBITMAP hbmOldMask  = (HBITMAP)::SelectObject(m_hdcMask, m_hbmMask);

    // XOR the image with the destination
    ::SetBkColor(m_hdcOffScr, rgbWhite);
	::StretchBlt(m_hdcOffScr, 0, 0, destWidth, destHeight, 
			m_hdcImage, 0, 0, srcWidth, srcHeight, SRCINVERT);
    
	// AND the destination with the mask
	::StretchBlt(m_hdcOffScr, 0, 0, destWidth, destHeight, 
			m_hdcMask, 0, 0, srcWidth, srcHeight, SRCAND);
    
	// XOR the destination with the image again
	::StretchBlt(m_hdcOffScr, 0, 0, destWidth, destHeight, 
			m_hdcImage, 0, 0, srcWidth, srcHeight, SRCINVERT);

    // Copy the resultant image back to the screen DC
	::StretchBlt(hDC, destX, destY, destWidth, destHeight, 
			m_hdcOffScr, srcX, srcY, destWidth, destHeight, SRCCOPY);

    // Tidy up
    ::SelectObject(m_hdcOffScr, hbmOldOffScr);
    ::SelectObject(m_hdcImage,  hbmOldImage);
    ::SelectObject(m_hdcMask,   hbmOldMask);
}


void CTransBmp::DrawTrans(HDC hDC, int x, int y, int cx, int cy, int dx, int dy)
{
    if (m_hbmMask == NULL)
        CreateMask(hDC);

    int width   = GetWidth();
    int height  = GetHeight();

    // Create a memory DC to do the drawing to
    if (m_hdcOffScr == NULL)
        m_hdcOffScr = ::CreateCompatibleDC(hDC);

    // Create a bitmap for the off-screen DC that is really
    // color compatible with the destination DC.
    if (m_hbmOffScr == NULL)
    {
        m_hbmOffScr = ::CreateBitmap( width, 
                                      height, 
                               (BYTE) GetDeviceCaps(hDC, PLANES),
                               (BYTE) GetDeviceCaps(hDC, BITSPIXEL),
                                      NULL );
    }

    // Select the buffer bitmap into the off-screen DC
    HBITMAP hbmOldOffScr = (HBITMAP)::SelectObject(m_hdcOffScr, m_hbmOffScr);

    // Copy the image of the destination rectangle to the
    // off-screen buffer DC so we can play with it
    ::BitBlt(m_hdcOffScr, 0, 0, width, height, hDC, x, y, SRCCOPY);

    // Create a memory DC for the source image
    if ( m_hdcImage == NULL )
    {
        m_hdcImage    = ::CreateCompatibleDC(hDC); 
    }
    HBITMAP hbmOldImage = (HBITMAP)::SelectObject(m_hdcImage, m_hObject);

    // Create a memory DC for the mask
    if ( m_hdcMask == NULL )
    {
        m_hdcMask     = ::CreateCompatibleDC(hDC);
    }
    HBITMAP hbmOldMask  = (HBITMAP)::SelectObject(m_hdcMask, m_hbmMask);

    // XOR the image with the destination
    ::SetBkColor(m_hdcOffScr, rgbWhite);
    ::BitBlt(m_hdcOffScr, 0, 0, width, height ,m_hdcImage, 0, 0, SRCINVERT);
    // AND the destination with the mask
    ::BitBlt(m_hdcOffScr, 0, 0, width, height, m_hdcMask,  0, 0, SRCAND);
    // XOR the destination with the image again
    ::BitBlt(m_hdcOffScr, 0, 0, width, height, m_hdcImage, 0, 0, SRCINVERT);

    // Copy the resultant image back to the screen DC
    ::BitBlt(hDC, x, y, cx, cy, m_hdcOffScr, dx, dy, SRCCOPY);

    // Tidy up
    ::SelectObject(m_hdcOffScr, hbmOldOffScr);
    ::SelectObject(m_hdcImage,  hbmOldImage);
    ::SelectObject(m_hdcMask,   hbmOldMask);
}
