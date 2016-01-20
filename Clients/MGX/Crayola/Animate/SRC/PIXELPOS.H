/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#ifndef _PIXELPOS_H_
#define _PIXELPOS_H_

	   
class PixelPos                              // a pixel may be various widths
{
public:

	BYTE far*   m_lpPixel;                  // point to the byte we are working on NOW
	BYTE        m_Mask;
    BYTE        m_BytesPixel;
	BYTE        m_Pos;                      // 0..7 position in the byte where pixel starts

public:
	PixelPos();                             // constructor
	long GetPixel();                        // get the pixel at this position
	void PutPixel(long c, BYTE iPos);       // save the pixel at this position
	void operator++();                      // go to next position
	void operator--();                      // go to previous pixel pos
	void SetPosition(BYTE far *lpStart, BYTE bitPos);
	void SetBytesPixel(BYTE BytesPixel) { m_BytesPixel = BytesPixel; }
	void SetBitsPixel(BYTE BitsPixel);

private:
	void MakeMask(BYTE pos);                // mask the bits of the pixel
};

#endif


