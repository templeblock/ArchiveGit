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
*    SFheader.h  - Standard Library Main Header File.                   *
*                                                                       *
*-----------------------------------------------------------------------*/

#ifndef SFHEADER_H

#define SFHEADER_H

/* The size, in bytes, of header blocks.  Multiply the value in SFHHEADSIZ
 * by this to determine # bytes total in header.
*/
#define SFHHEADBLOCKSIZE    512

/*                  Single Byte Hashing Macro
 *
 * Description:
 * This macro is used to generate an index and code value 
 * for a string of characters given the code and index of the 
 * string which consists of all but the last character, and the
 * last character.  It is used to get hash codes of progressively
 * longer pieces of a string.  If it is called on the 1st character
 * of a string, *pwCode and *pwIndex should be set to 0 before the
 * call is made.
*/
#define SFBytHsh(cInByte, pwCode, pwIndex)                                 \
{                                                                          \
   UINT2B wAlpha;                                                          \
                                                                           \
   wAlpha = *(pwCode) >> 7;                                                \
   *(pwCode) = wAlpha + (wAlpha << 1) + (wAlpha << 4) +                    \
    (UINT2B) (cInByte) + ((*(pwCode) & 0x007F) << 8);                      \
   if (*(pwCode) > 32748)                                                  \
       *(pwCode) -= 32749;                                                 \
   *(pwIndex) ^= *(pwCode);                                                \
                                                                           \
   /* ENSURE THAT THE VALUE IS PROPERLY LIMITED                          */\
   *(pwIndex) = *(pwIndex) & 0x7FFF;                                       \
}                                                                          

/*                  Header n-byte value fetch macros
 *
 * N.B.: The tables which you pass to these macros MUST be of unsigned char
 * type or cast to such, else sign extension will occur for tab[loc], 
 * yielding erroneous results.
*/
/* To get two byte values from a header.
*/
#define SFHHEAD2(tab,loc)   ((UINT2B) (tab)[loc] << 8 | (UINT2B) (tab)[(loc)+1])
/* To get four byte values from the header.
*/
#define SFHHEAD4(tab,loc)   ((INT4B) (tab)[loc] << 24 | (INT4B) (tab)[(loc)+1] << 16 |\
                (INT4B) (tab)[(loc)+2] <<  8 | (INT4B) (tab)[(loc)+3])


/* To get SWAPPED two byte values from the header.
*/
#define SF2HHEADSWAP(tab,loc)  ((INT2B) tab[(loc)+1] <<  8 | (INT2B) tab[loc])
/* To get SWAPPED four byte values from the header.
*/
#define SFHHEADSWAP4(tab, loc)  \
    ((INT4B)(   \
        tab[loc] |  \
        ((UINT2B)tab[(loc)+1] << 8) |   \
        ((UINT4B)tab[(loc)+2] << 16)|   \
        ((UINT4B)tab[(loc)+3] << 24)    \
    ))

/* Macros for creating header fields from integers
*/
#define SFHHEAD2_MAKE(tab,loc,int_val)      (tab)[loc] = (UCHAR) (((int_val) >> 8) & 0xFF), \
        (tab)[(loc) + 1] = (UCHAR) ((int_val) & 0xFF)
#define SFHHEAD4_MAKE(tab,loc,long_val)     (tab)[loc] = (UCHAR) (((long_val) >> 24) & 0xFF), \
        (tab)[(loc) + 1] = (UCHAR) (((long_val) >> 16) & 0xFF), \
        (tab)[(loc) + 2] = (UCHAR) (((long_val) >> 8) & 0xFF), \
        (tab)[(loc) + 3] = (UCHAR) ((long_val) & 0xFF)


/*                  Header positions for fields.  
 * To determine the size of a field, subtract the field's offset from the 
 * next field's offset.
*/

/* Standard fields for all Inso product files.
*/
#define SFHCOPYRIGHT    0       /* Company copyright notice, ^Z terminated  */
#define SFHPRODUCT      384     /* Product ID; which prod this file runs w/ */
#define SFHLANGUAGE     386     /* Native written language of lexicon       */
#define SFHFILESTRUCT   388     /* Version of decoding form needed for data */
#define SFHFILETYPE     390     /* How to store/access/interpret file data  */
#define SFHBLOCKSIZ     392     /* Logical block size of encoded data       */
#define SFHHEADSIZ      394     /* This hdr's size * BLOCKSIZ (#blks in hdr)*/

/* Defines for ICS-compatible Inso products.  If a product has additional
 * data besides ICS-encoded data or wishes to describe a lexicon in greater
 * detail, extra fields can be added in the product-specific region below.
 * E.g., publisher & date of printed source, tables for other encoding
 * schemes, descriptions of other-than-ICS lexicons.
 *
 * For descriptions of various tables' organizations and uses, refer to
 * internal documentation on ICS.
*/
#define SFHCRDATE       396     /* Creation date of ICS-encoded lexicon     */
#define SFHDBVERSION    404     /* Version number of ICS-encoded lexicon    */
#define SFHDATADESC     414     /* Textual description of ICS-encoded lex.  */
#define SFHMSTRID       446     /* ID of master wordlist of ICS lexicon     */
#define SFAVLDIALECT    478     /* Dialects avaliable in this database      */
#define SFSECVERSIZ     480     /* Secondary verification table             */
#define SFHDBLSIZ       488     /* Double-word table size                   */
#define SFHPTRSIZ       490     /* Pointer table size                       */
#define SFHVTABSIZ      492     /* Verification table size                  */
#define SFHLANGINFOSIZ  492     /* Size of the lang info table              */
#define SFHCLITSIZ      494     /* Clitic table size                        */
#define SFHCOMPSIZ      496     /* Compound table size                      */
#define SFHLINKSIZ      498     /* Link table size                          */
#define SFHBISKEYSIZ    500     /* Binary search key size                   */
#define SFHBISLMT       502     /* Binary search limit                      */
#define SFHMAXLEN       504     /* Maximum length of word in wordlist       */
#define SFHEXCSIZ       506     /* Exception table size                     */
#define SFHZEROSIZ      508     /* Zero-exception table size                */
#define SFHSTRSIZ       510     /* String table size                        */
#define SFH4GRAMSIZ     512     /* Quadgram table size                      */
#define SFHRIGSIZ       514     /* Rigged correction table size             */
#define SFHSTEPSIZ      516     /* Step table size                          */
#define SFHSTRINGCMPSIZ 518     /* Compression Strings size                 */
#define SFHSTARTCMPSIZ  520     /* Compression string pointers size         */
#define SFHLENGTHCMPSIZ 522     /* Lengths of strings size                  */
#define SFHCMPBITSPERCOD 524    /* Number of bits/compression code          */
#define SFHCCHINITSIZ   526     /* Cache initialization size                */

/* Reserved regions of ICS-compatible headers.
*/
#define SFICSEXTENS     530     /* Future expansion to be in all ICS-compat */
#define SFPRODSPECIFS   700     /* Mapped particular to ICS-compat product  */

#define SFNEXTHEADER    1000    /* Link, next header/file.  x BLOCKSIZ      */
#define SFRESERVED      1002    /* Future expansion                         */

/* Product codes for all Inso products, to be fit in the SFHPRODUCT field
 * of product files.  Compare the SFHPRODUCT contents against the define
 * for particular product to insure the file belongs to the product
 * executing.
*/
#define ICPRODCODE  6             /* International CorrectSpell */
#define IHPRODCODE  7             /* International Hyphenator   */
#define GRPRODCODE  8             /* Grammer Correction System  */
#define REPRODCODE  9             /* Reference Engine           */
#define IEPRODCODE  10            /* International Electronic Thesaurus */
#define PRPRODCODE  11            /* International Proof Reader */
#define PDPRODCODE  12            /* Personal Dictionary        */
#define IIPRODCODE  13            /* Inso IntelliScope (II)     */


/* File structure version - which version of decompression must be used to
 * decode compressed files.  Read SFHFILESTRUCT from header and compare 
 * against this define to insure correct decompression of files by the code.
 *
 * SFFILESTRUCT pertains to files with ICS-like encoding.  If a product has
 * additional files using other types of encoding, add another file structure
 * version field in the product-specific area of the files.
*/
#define ICFILESTRUCT    5
#define SFFILESTRUCT    5

/* File types.  Indicates general nature/use of file and determines how the
 * file will be processed or what routines apply to the file.  Read 
 * SFHFILETYPE from header and compare against this define to dictate how
 * the software should handle the file's data.
 * 
 * SFDISKFILE simply indicates a file is not an IH file.  If a product has
 * additional files type classifications it needs to make, add another file 
 * type field in the product-specific area of the files.
*/
#define IHRAMFILE   1
#define IHDISKFILE  2
#define SFDISKFILE  2

/* For SFDISKFILE types, these masks determine which fields are contained in
 * each word's record in the index list.  Fields are synonymous with states
 * in SFdecode.  FMASK stands for Format MASK.
*/
#define POS_FMASK_SF        0x0001  /* part of speech */
#define BASEFORM_FMASK_SF   0x0002  /* baseforms */
#define POI_FMASK_SF        0x0004  /* parts of inflection */
#define IRRINF_FMASK_SF     0x0008  /* irregular inflection */
#define SYN_FMASK_SF        0X0010  /* synonyms */

/* For copyright check in header
*/
#define SFCRIGHT_STR    "Copyright [c] 1995  INSO Corporation\x1a"

#define SFLEN_CRIGHT    37                 /* length of above string */

#endif /* SFHEADER_H */

/* End of file - SFheader.h */
