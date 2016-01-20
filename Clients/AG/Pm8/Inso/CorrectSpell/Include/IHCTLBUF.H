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

*-----------------------------------------------------------------------*
*                                                                       *
*    IHctlbuf.h - User Structure Defines for the internal control       *
*                 buffer for the International Hyphenator Software      *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *

     Description:
     This header contains the typdef for the internal control buffer 
     for the International Hyphenator Software.

     This header should be included by all modules of IH.

     Cautions:

*-----------------------------------------------------------------------*/
#ifndef IHCTLBUF_H
#define IHCTLBUF_H

/*
 *  The following structure is used to hold the ram pointers
*/

typedef struct _rampointers {
    UCHAR First3Chars[3];
    UINT2B   RamIndex;
    UCHAR BytePos;
} RAMPOINTERS, FAR * PRAMPOINTERS;

SLF_TYPDEF_OR (SLF_HDL_OBJ, PRAMPOINTERS, _ORF_RAMPOINTERS_IH) ORF_RAMPOINTERS_IH;

/*
 *  This structure contains the information for the compressed data
*/

typedef struct _compressinfo {
    ORF_2BUINT ofcmp_starts;    /* The pointers to the start/left codes */
    ORF_BUFF   ofcmp_lengths;   /* The pointers to the length/right codes */
    ORF_BUFF   ofcmp_strings;   /* The pointers to the big string*/
    ORH_BUFF   ofRamImage;      /* The pointers to the compressed data */
    ORF_BUFF   ofNonRMCC;        /* The pointers to the hash table of non RMCC's*/
} COMPRESSINFO, FAR * PCOMPRESSINFO;

/*     Hyphenator Control Buffer                                 */

typedef struct _IHCTLBUFF
{
    INT4B  xor_map;         /* Exclusive or map         */

    UINT2B ngram_siz,  first_pos,  last_pos;

    ORF_2BUINT ofcachetab;          /* Cache table for words in disk file */

    UCHAR  hyp_version;               /* Preferred, Syllabic or Standard */

    COMPRESSINFO   ram;

    UCHAR    lmcc_found;
    INT4B    lmcc_map[2];         /* The map indicating the LMCC hypen point */    
    UINT2B   lmcc_length;

    UCHAR    exception_found;     /* We found an exception */
    INT4B    exception_map;

    UCHAR    alternate_found;     /* Is there an alternate hyphenation */

    UINT2B   non_rmcc;    /* The length of the non rmcc hash table */
    UINT2B   NoofRamPointers;
    ORF_RAMPOINTERS_IH  ofRamPointers;

    UINT4B  disk_size;   /*  The size The size of the compressed data */
    UINT4B  RamIndex;    /*  After the bsearch what is its index*/
    UCHAR   BytePos;     /*  After the bsearch what is its bypte pos*/
    UINT4B  NextRamIndex;/*  After the bsearch what is its index of the next entry*/
    UCHAR    NextBytePos; /*  After the bserach what is the byte pos of the next entry*/
    INT2B    stage1;
    INT2B    stage2;
    UINT4B    map;        /*  Contains the map found in the data base*/
    UINT2B    pre_post_map;    /*  Contains the map for the pre and post cliics*/
    INT2B    length;         /*  The length of the ngram used for mandatory hyph */
   UCHAR   alt_spell[IHMAXHYPLEN+1];/* alternate spelling used inmandatory hyph             */
#ifdef MAC_HANDLES
    FLAG bHHndlLck;          /* True if handles are currently locked */
#endif
    
} IHCTLBUFF, NEAR *PIHCTLBUFF, FAR * PFIHCTLBUFF;

SLF_TYPDEF_OR (SLF_HDL_OBJ, PFIHCTLBUFF, _ORF_IHCTLBUFF_IH) ORF_IHCTLBUFF_IH;

#endif /* IHCTLBUF_H */

/*  end file ihctlbuf.h */
