/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*

*-----------------------------------------------------------------------*
*                                                                       *
*    SLmacros.h - Various SL macros are defined here.                   *
*                                                                       *
*-----------------------------------------------------------------------*

    Descripton:
        This file contains the defines and macros used for
        reading and setting various types of maps, masking language 
        settings and converting single characters.

    Cautions:
        None.

    Creation Date:
        18-jul-89   


*-----------------------------------------------------------------------*/
#ifndef SLMACROS_H

#define SLMACROS_H

/************************************************************************/
/*                                                                      */
/*    'SLREADMAP' reads delimiter and hyphen bitmaps                    */
/*    'SLSETMAP'  sets the word's hyphenation bitmap                    */
/*    'SLCLEARMAP' clears a bit in the given bitmap                     */
/*                                                                      */
/* For all three of there macros:                                       */
/*    'p' points to the first 32-bit element in the bitmap              */
/*    ( p + the_offset_into_p ) gives the proper array element to be    */
/*    worked upon.                                                      */
/*                                                                      */
/*    'x' is the offset into the bitmap array.  If 'x' is 0-31, then    */
/*        we deal with the first element in the array (p + 0).          */
/*        If 'x' == 32, then it's the first bit in the second           */
/*        array-element (p + 1).                                        */
/*        Thus, the rightmost 5 bits define which bit in the            */
/*        element, and the higher order bits define the element         */
/*        in the array.                                                 */
/*    SLONEBIT_shifted_left    gives the appropriate bit within the     */
/*                             32-bit element                           */ 
/*                                                                      */
/*    Avoid the use of LURSHFT by shifting a right most bit to the      */
/*    left by 31-x bits rather than shifting a leftmost bit right by    */
/*    x bits.                                                           */
/*                                                                      */
/************************************************************************/

#define SLMAPSIZE        31    /* Zero relative                     */
#define SLOFFSETBITS      5    /* Number of bits to specify 31      */
#define SLOFFSETMASK 0x001F    /* Mask for offset into map          */
                               /* Note: These are 5 bits to specify */
                               /*       up to 31 places.            */
#define SLONEBIT    0x00000001    /* One: bit to shifted            */

#define SLSETMAP( p, x )   \
    *(p + ( (UINT2B)((UINT2B) x) >> SLOFFSETBITS ) ) |= ( (UINT4B) SLONEBIT << (SLMAPSIZE - (x & SLOFFSETMASK)) )

#define SLCLEARMAP( p, x ) \
    *(p + ( (UINT2B)((UINT2B) x) >> SLOFFSETBITS ) ) &= ~( (UINT4B) SLONEBIT << (SLMAPSIZE - (x & SLOFFSETMASK)) )

#define SLREADMAP( p, x )  \
    *(p + ( (UINT2B)((UINT2B) x) >> SLOFFSETBITS ) ) & ( (UINT4B) SLONEBIT << (SLMAPSIZE - (x & SLOFFSETMASK)) )

#define SLwild1(c)    (c == SLWILD1)
#define SLwilds(c)    (c == SLWILDS)

#define SLWwild1(c)   (c == SLWWILD1)     /* for wide (2-byte) characters */
#define SLWwilds(c)   (c == SLWWILDS)

#define SLLANGMASK(x) ((x) ? (0x1UL << ((UINT4B) (((UINT4B)x)-1))) : 0xFFFFFFFFL)

/* SLBYTCNV() is a macro which, given a character and an sl character
   conversion table, returns the converted version of that character
*/
#define SL_CHARHIBIT 0x80

#define SLBYTCNV(uInChar, lpCnvTab)   \
   (((uInChar) & SL_CHARHIBIT)?(lpCnvTab)[(uInChar) - SL_CHARHIBIT]:(uInChar))

#endif /* SLMACROS_H */

/* End of file - SLmacros.h */
