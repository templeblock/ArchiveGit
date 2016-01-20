/****************************************************************************
 *                   Copyright  1995  INSO Corporation                      *
 *                International CorrectSpell[TM] Software                   *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************
*/

#ifndef _CMUTIL_DEF
#define _CMUTIL_DEF



#define CHAR_BITS 8

#define INT4B_BITS 32

#define INT2B_BITS 16

#define _3BYTES_BITS 24


#define _2BYTEOFFS 2
#define _4BYTEOFFS 4



#define GET4BYTES(tab) (UINT4B)((UINT4B) (tab)[3] << 24 | (UINT4B) (tab)[2] << 16 |\
                (UINT4B) (tab)[1] <<  8 | (UINT4B) (tab)[0])

#define GET3BYTES(tab) (UINT4B)((UINT4B) (tab)[2] << 16 |\
                (UINT4B) (tab)[1] <<  8 | (UINT4B) (tab)[0])

#define GET2BYTES(tab) (UINT2B)((UINT2B) (tab)[1] <<  8 | (UINT2B) (tab)[0])

#define PUTINT2B(tab,int_val)		((tab)[0] = (UCHAR) (((int_val) >> 8) & 0xFF), \
		(tab)[1] = (UCHAR) ((int_val) & 0xFF))
		
#define PUTINT4B(tab,long_val)		((tab)[0] = (UCHAR) (((long_val) >> 24) & 0xFF), \
		(tab)[1] = (UCHAR) (((long_val) >> 16) & 0xFF), \
		(tab)[2] = (UCHAR) (((long_val) >> 8) & 0xFF), \
		(tab)[3] = (UCHAR) ((long_val) & 0xFF))


#define PUTSWINT2B(tab,int_val)		((tab)[1] = (UCHAR) (((int_val) >> 8) & 0xFF), \
		(tab)[0] = (UCHAR) ((int_val) & 0xFF))
		
#define PUTSWINT4B(tab,long_val)		((tab)[3] = (UCHAR) (((long_val) >> 24) & 0xFF), \
		(tab)[2] = (UCHAR) (((long_val) >> 16) & 0xFF), \
		(tab)[1] = (UCHAR) (((long_val) >> 8) & 0xFF), \
		(tab)[0] = (UCHAR) ((long_val) & 0xFF))


#define StrCover(n, ss1, ss2) {\
    for ((n) = ZERO; *ss1 == *ss2 && *ss1 && *ss2;\
           ++(n), ++ss1, ++ss2);\
}

#define _StrCover(n, s1, s2) {\
    SLREG PFUCHAR ss1, ss2;\
    for ((n) = ZERO, ss1 = s1, ss2 = s2; *ss1 == *ss2 && *ss1 && *ss2;\
           ++(n), ++ss1, ++ss2);\
}

#endif



