/* 
####################################################################################
#                              Animator                                            #
#                    Copyright 1991 - 1993 Ray White                               #
#                         3804 Seventh Street                                      #
#                        Sachse, Texas  75048                                      #
####################################################################################
*/

#ifndef _BITMAPS_H_
#define _BITMAPS_H_


class BitObject                         // a bitmap to move on the screen
{       
    friend class CAnimateObj;
    friend class CAnimator;

	BitObject       *next;              // chain all the bitmaps together
	HBITMAP         hObjectBmap;
	HBITMAP         hMaskBmap;
    bool            m_bActive;          // Activate/Deactivate frames
    int             m_FrameNum;
    int             m_xOff;             // offset from 0, 0 to draw frame
    int             m_yOff;
    int             m_Width;            // Each frame can have a different size
    int             m_Height;

	BitObject();
	~BitObject();
	void FlipVert();                    // flip a bitmap
	void FlipHorz();
	void FlipOneVert(HBITMAP hBitmap);
	void FlipOneHorz(HBITMAP hBitmap);
};

#endif

 

