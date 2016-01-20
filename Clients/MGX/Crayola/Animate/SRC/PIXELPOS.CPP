/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#include "app.h"
#include "pixelpos.h"
			  
 		  
PixelPos::PixelPos()        // constructor                                                                                          // constructor
{
	m_lpPixel    = 0;            // address of where it is
	m_Pos        = 0;            // where is currently is
	m_Mask       = 0;            // only where the bits are
    m_BytesPixel = 1; 
}


void PixelPos::SetBitsPixel(BYTE BitsPixel)
{
    switch (BitsPixel)
    {
        case 4: m_BytesPixel = 0;   break;
        case 8: m_BytesPixel = 1;   break;
        case 16: 
        case 24: 
            m_BytesPixel = 3;   
            break;
    }
}

void PixelPos::SetPosition (BYTE far *lpStart, BYTE bitPos)
{
	m_lpPixel = lpStart;
	m_Pos  = bitPos;
	MakeMask(m_Pos);
}




long PixelPos::GetPixel()                       // grab the one where you are now
{
    long    c;
    if (m_BytesPixel == 0)
        c = (*m_lpPixel & m_Mask);                  // only give this pixel info
    else if (m_BytesPixel == 1)
        c = *m_lpPixel;
    else if (m_BytesPixel == 3)
        c = *m_lpPixel | (*(m_lpPixel+1) << 8) | (*(m_lpPixel+2) << 16);
    return c;
}





void PixelPos::PutPixel(long c, BYTE iPos)      // place the pixel where you are now
{
    if (m_BytesPixel == 0)
    {
        if (iPos > m_Pos)
        	 c >>= (iPos - m_Pos);
        else c <<= (m_Pos  - iPos);
        *m_lpPixel = ((*m_lpPixel & ~m_Mask) | c);  // only change this part of the byte
    }
    else if (m_BytesPixel == 1)
        *m_lpPixel = c;
    else if (m_BytesPixel == 3)
    {
        *m_lpPixel = c;
        *(m_lpPixel+1) = c >> 8;
        *(m_lpPixel+2) = c >> 16;
    }    
}




void PixelPos::operator++()                     // next pixel
{
    if (m_BytesPixel == 0)
    {    
        m_Pos--;
        if (m_Pos < 0 || m_Pos > 8)             // advance RIGHT to next pixel within byte
        {
            m_Pos = 7;
            m_lpPixel++;                      // go on to next byte now
            m_Mask = 128;
        }
        else
            m_Mask >>= 1;
    }
    else
        m_lpPixel += m_BytesPixel;    
}




void PixelPos::operator--()                             // previous
{
    if (m_BytesPixel == 0)
    {    
        if (++m_Pos > 7)                          // advance LEFT to next pixel within byte
        {
            m_Pos = 0;
            m_lpPixel--;                              // previous byte
            m_Mask = 1;
        }
        else
            m_Mask <<= 1;
    }
    else
        m_lpPixel -= m_BytesPixel;    
}




void PixelPos::MakeMask(BYTE pos)
{
    if (m_BytesPixel == 0)
    {    
        m_Mask = 1;
        if (pos)
            m_Mask <<= pos;
    }
}












