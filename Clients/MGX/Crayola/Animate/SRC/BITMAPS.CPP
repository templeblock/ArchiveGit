/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#include "app.h"
#include "bitmaps.h"
#include "pixelpos.h"




// ------------------------BitObject bitmaps --------------------------


BitObject::BitObject()
{
	next        = NULL;       // initialize the members
	hObjectBmap = NULL;
	hMaskBmap   = NULL;
    m_bActive   = TRUE;
    m_FrameNum  = 1;
    m_xOff      = 0;
    m_yOff      = 0;
    m_Width     = 0;
    m_Height    = 0;
}



BitObject::~BitObject()        // destructor - delete bitmaps
{
	DeleteObject(hObjectBmap);
	DeleteObject(hMaskBmap);
}





void BitObject::FlipOneVert(HBITMAP hBitmap)            // flip a bitmap
{
	BITMAP       bm;
	long         x, y;                // swap 16 bit values
	unsigned int c;                              // use c as temp swap int
	unsigned int *A;
	unsigned int *B;
	unsigned int *endBits;
	unsigned int *begBits;
	long         lastRow;
	DWORD        DestBytes;
	char         *lpBits;
	long         wordsPerLine;
	long         bytesPerLine;


	GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);        // get size of bitmaps
	DestBytes = bm.bmPlanes * bm.bmHeight * bm.bmWidthBytes;

	lpBits = new char[DestBytes];
	GetBitmapBits(hBitmap, DestBytes, lpBits);

	bytesPerLine = DestBytes / bm.bmHeight;
	wordsPerLine = bytesPerLine / 2;

	begBits = (unsigned int *) lpBits;
	endBits = (unsigned int *) (lpBits + DestBytes - bytesPerLine) ;
	lastRow = DestBytes / 2 / 2;                            // 2 byte ints

	for (y=0; y < lastRow; y += bytesPerLine / 2)           // half the rows
	{
		A = (unsigned int *)(begBits + y);
		B = (unsigned int *)(endBits - y);

		for (x=0; x<wordsPerLine; x++)
		{
			c = *A;                                         // c is temporary swap
			*A = *B;
			*B = c;

			A += 1;                                         // walk ahead 16 bits (int)
			B += 1;                                         // add 1 for one int.
		}
	}

	SetBitmapBits(hBitmap, DestBytes, lpBits);      // put the new one back
	delete lpBits;
}



void BitObject::FlipVert()                              // flip both bitmaps
{
	FlipOneVert(hObjectBmap);
	FlipOneVert(hMaskBmap);
}

void BitObject::FlipHorz()                              // flip both bitmaps
{
	FlipOneHorz(hObjectBmap);
	FlipOneHorz(hMaskBmap);
}






void BitObject::FlipOneHorz(HBITMAP hBitmap)            // flip a bitmap
{
    BITMAP    bm;             // the one to swap
    DWORD     DestBytes;      // how big is the bitmap
    char far  *lpBits;        // first byte of image
    long      i, x;
    long      c;
    PixelPos  pA;             // Pixel pointer
    PixelPos  pB;
    BYTE      lBitPos=0;        // where to start bits on each line
    BYTE      rBitPos=0;
    BYTE far  *lpNextLeft;    // start of flipping bits
    BYTE far  *lpNextRight;
    long      nPlaneLines;
    int       halfLine;
    HGLOBAL   hBits;
    int       nLineBytes;


	GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);        // get size of bitmaps
	DestBytes = bm.bmPlanes * bm.bmHeight * bm.bmWidthBytes;

	if (!(hBits = GlobalAlloc(GHND, DestBytes)))
		return;
	if (!(lpBits = (char far *)GlobalLock(hBits)))
	{
        GlobalFree(hBits);
        return;
    }

	GetBitmapBits(hBitmap, DestBytes, lpBits);
	nPlaneLines = bm.bmHeight * bm.bmPlanes;
	halfLine = bm.bmWidth / 2;
    nLineBytes = (bm.bmWidth * bm.bmBitsPixel) / 8;
    pA.SetBitsPixel(bm.bmBitsPixel);
    pB.SetBitsPixel(bm.bmBitsPixel);

    if (bm.bmBitsPixel < 8)
    {
	    lBitPos = 7;
	    rBitPos = 8 - (bm.bmWidth % 8);
	    if (rBitPos == 8)
		    rBitPos = 0;
    }

	lpNextLeft  = (BYTE far *)lpBits;
	lpNextRight = lpNextLeft + nLineBytes -1;


	for (i=0; i<nPlaneLines; i++)                   // scan lines
	{
		pA.SetPosition(lpNextLeft,  lBitPos);       // set at start, then ++
		pB.SetPosition(lpNextRight, rBitPos);

		for (x=0; x<halfLine; x++)                  // pixels in the line
		{
			c = pA.GetPixel();
			pA.PutPixel(pB.GetPixel(), pB.m_Pos);
			pB.PutPixel(c            , pA.m_Pos);

			pA++;                                   // go toward the middle
			pB--;
		}

		lpNextLeft += bm.bmWidthBytes;
		lpNextRight = lpNextLeft + nLineBytes -1;
	}

	SetBitmapBits(hBitmap, DestBytes, lpBits);      // put the new one back
    GlobalUnlock(hBits);
    GlobalFree(hBits);
}









 








