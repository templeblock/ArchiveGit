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
*    SFbfile.h -                                                        *
*                                                                       *
*-----------------------------------------------------------------------*/
#ifndef SFBFILE_H

#define SFBFILE_H

#ifndef SFBFILE_INC
#define SFBF_REALLOC 0x01   /*  Whole thing (BF + buf) allocated at once */

typedef struct bitinfo
{   CHAR    nbits,          /*  Number of bits per code             */
            left;           /*  Number of bits left in save         */
    UINT2B  nleft,          /*  Number of codes left in the buffer  */
            code,           /*  Temporary used to save the top part */
            save,           /*  Place for the saved bits to reside  */
            mask;           /*  a mask for the bottom n bits.       */
}   BINFO;


typedef struct      /*  This structure should be aligned for best speed */
{   BINFO   bi;             
    PUCHAR  ptr,        /*  points to the current byte of the buffer    */
            base;       /*  Points to the base of the buffer            */
    UINT2B  flags,      /*  allocation flags for the buffer             */
            blen;       /*  Length in bits of the allocated buffer      */
    UINT4B  fstart,     /*  Byte Position Start of the virtual file     */
            fpos,       /*  Current virtual position in the file        */
            flen,       /*  Length of the virtual file                  */
            btr,        /*  Bytes to Read for macro temporary storage   */
            br;         /*  Bytes Read for macro temporary storage      */
    SL_FHANDLE
            fd;         /*  The file descriptor for the file            */

}   BFILE, *PBFILE, NEAR *PNBFILE, FAR *PFBFILE;

SLN_TYPDEF_OR(SLN_HDL_OBJ, PNBFILE, ORS_bfile) ORN_BFILE;

/*  _GetWord() reads two bytes of data from the machine in the correct
 *  order.  BIGENDIAN is a machine dependent SL attribute that describes the
 *  ordering of the bytes in a word.  It will be defined if the high byte
 *  of a word appears first in memory (68000 as opposed to 80x86).
 *     16 bit number: 4a70h (19,056 decimal)
 *        Big endian: 4a70
 *     Little endian: 704a
 *     
 */
#ifdef  SLA_BIGENDIAN_AM
#define _GetWord(buf)   *((PNUINT2B)(buf))
#else
#define _GetWord(buf)   ((buf)[0] << 8) | (buf)[1]
#endif

/*  _SF_MaxBFLen() returns the maximum number of bytes that should be
 *  read into the buffer.  This value is the smaller of the size of the
 *  buffer, and the bytes remaining in the file.
 */
#define _SF_MaxBFLen(bf)                    \
    (   ( (bf->flen - bf->fpos) > bf->blen) \
        ?   bf->blen                        \
        :   (bf->flen - bf->fpos)           \
    )

/* The following macros were created because some platforms may have
 * a problem with negative shifting (arithmetic and/or logical).
 * VMS and SCO UNIX should both have this SL attribute undefined.
*/
#ifndef SLA_NEGBITSHIFT_AM

#define _SF_SHIFTL(l,r)   \
(                         \
(r < 0)                   \
? (l >> abs(r))           \
: (l << r)                \
)

#define _SF_SHIFTR(l,r)   \
(                         \
(r < 0)                   \
? (l << abs(r))           \
: (l >> r)                \
)

#else /* SLA_NEGBITSHIFT_AM defined */

#define _SF_SHIFTL(l,r) (l << r)
#define _SF_SHIFTR(l,r) (l >> r)

#endif

/*  _SF_FilBuf() fills the buffer with data from the file at the
 *  current position, and then returns the next code.
 */
#define _SF_FilBuf(bf)                              \
(   (bf->fpos >= bf->flen)                          \
    ?   -1                                          \
    :                                               \
    (   (   (bf->ptr = bf->base),                   \
            (bf->btr = _SF_MaxBFLen(bf)),           \
            (   (SLRead(bf->fd, bf->btr,            \
                        (PCHAR)(bf->ptr),           \
                        (PFINT4B)&((bf)->br)        \
                    ) == ERRET                      \
                ) || (bf->btr != bf->br)            \
            )                                       \
        )                                           \
        ?   -1                                      \
        :                                           \
        (   (   bf->fpos += bf->br),                \
            (   bf->bi.left -= bf->bi.nbits),       \
            (   bf->bi.code = _SF_SHIFTL(bf->bi.save, -bf->bi.left)),                      \
            (   bf->bi.save = _GetWord(bf->ptr)),   \
            (   bf->ptr += 2),                      \
            (   bf->bi.nleft =                      \
                ( (INT2B)((bf->br << 3) + bf->bi.left) \
                  / bf->bi.nbits)                   \
            ),                                      \
            (   bf->bi.left += 16),                 \
            (   bf->bi.code |= _SF_SHIFTR(bf->bi.save, bf->bi.left)),                     \
            (   bf->bi.code &= bf->bi.mask)            \
        )                                           \
    )                                               \
)

/*  If we need to get the code in two parts, _SF_GetTop() returns the top
 *  part of the code.  Otherwise it simply returns 0.
 */
#define _SF_GetIndex(bf)                                            \
    (   (   (   (bf->bi.left -= bf->bi.nbits) < 0)                  \
            ?                                                       \
            (bf->bi.code = _SF_SHIFTL(bf->bi.save, -bf->bi.left)),  \
            (bf->bi.save = _GetWord(bf->ptr)),                      \
            (bf->ptr += 2),                                         \
            (bf->bi.left += 16),                                    \
            (bf->bi.code | _SF_SHIFTR(bf->bi.save, bf->bi.left))  \
            :                                                       \
            (bf->bi.save >> bf->bi.left)                            \
        ) & bf->bi.mask                                             \
    )

/*  SFGetCode() retrives the current nBit code, filling the buffer
 *  if not enough data is available.
 */
#define        SFCodesLeft(bf)        ((bf)->bi.nleft)
#define SFGetCode(bf)       \
        (INT2B)( ((bf)->bi.nleft--) ?  _SF_GetIndex(bf) : _SF_FilBuf(bf) )

#define SFTellBF(bf)    \
    (   (   ((bf)->fpos << 3) / (bf)->bi.nbits) - (bf)->bi.nleft)

extern INT2B _SF_FilBuffer(PFBFILE);
extern PFBFILE SFInitBF(PFBFILE, SL_FHANDLE, UCHAR, UINT2B, UINT4B, UINT4B);
extern PNBFILE SFAllocBF(SL_FHANDLE, UCHAR, UINT2B, UINT4B, UINT4B);
extern RETCODE SFAllocBFObj(
    ORN_BFILE FAR *, SL_FHANDLE, UCHAR, UINT2B, UINT4B, UINT4B);
extern VOID SFFreeBFObj(ORN_BFILE FAR *obf);
extern VOID SFReleaseBF(PFBFILE);
extern INT4B SFSeekBF(PFBFILE, INT4B, CHAR);

#define SFBFILE_INC
#endif  /*  SFBFILE_INC */

#endif /* SFBFILE_H */

/* End of file - SFbfile.h */
