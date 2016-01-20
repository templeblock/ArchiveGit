/****************************************************************************
 *                     Copyright 1995 INSO Corporation                      *
 *                    IntelliFinder(TM) Reference Engine                    *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************/


/*-----------------------------------------------------------------------*
 *                                                                       *
 * REtocrle.h - run length encoding header for RE table of contents.     *
 *                                                                       *
 *-----------------------------------------------------------------------*
*/
#ifndef RETOCRLE_H

#define RETOCRLE_H

BFTYPE move_decompress(PFUCHAR to, BFTYPE to_len, PFUCHAR from, BFTYPE from_len);
BFTYPE move_compress(PFUCHAR to,ENTTYPE to_len, PFUCHAR from, ENTTYPE len);
BFTYPE find_last_true_byte(PFUCHAR field, UINT2B array_size);
BFTYPE find_first_true_byte(PFUCHAR field, UINT2B array_size);

/* compression header types */
#define SOLID_0    0x40
#define SOLID_1    0x80
#define MIXED      0xc0
/* the foll is specific to these assignments, for speed */
#define IS_SOLID_OR_MIXED(_type) (_type)

#define HEADER_TYPE_MASK 0xC0
#define HEADER_COUNT_MASK 0x3f

#define COMPRESSION_HEADER_SIZE 1
#define COMPRESSION_DATA_SIZE 63

/* small code change needed if the foll changes cf note below */
#define RUN_THRESHHOLD 3

/*#define WANTNEW*/
#ifdef WANTNEW /* not yet debugged */

#define NOUGHTS "'0x00''0x00''0x00'"
#define ONES    "'0x01''0x01''0x01'"

/* CHANGE THESE IF RUN_THRESHHOLD CHANGES -- WRITE LOOPS*/
#define ENOUGH_NOUGHTS(_count) \
              ((buf_end-buf_cursor>=_count) &&\
               !Cfmemcmp(buf_cursor, (PFUCHAR) NOUGHTS, _count) \
              )
              
#define ENOUGH_ONES(_count) \
              ((buf_end-buf_cursor>= _count) &&\
               !Cfmemcmp(buf_cursor, (PFUCHAR) ONES, _count) \
              )
#define UNIFORMBITS \
         (ch == 0x00 || ch == 0xff) && \
         ( ch == *(buf_cursor + 1) && ch == *(buf_cursor + 2)  )
#else /* old not wantnew */

/* CHANGE THESE IF RUN_THRESHHOLD CHANGES -- WRITE LOOPS*/
#define ENOUGH_NOUGHTS(_count) \
              ((buf_end-buf_cursor>= _count) &&\
               (*buf_cursor == 0x00 && *(buf_cursor+1) == 0x00 && *(buf_cursor+2)==0x00) \
              )
#define ENOUGH_ONES(_count) \
              ((buf_end-buf_cursor>=_count) &&\
               (*buf_cursor == 0xff && *(buf_cursor+1) == 0xff && *(buf_cursor+2)==0xff) \
              )
#define UNIFORMBITS \
         (ch == 0x00 || ch == 0xff) && \
         ( ch == *(buf_cursor + 1) && ch == *(buf_cursor + 2)  )
#endif /* old not want new */


#define NONESET -1

#endif /* RETOCRLE_H */

/* End of file - REtocrle.h
*/

