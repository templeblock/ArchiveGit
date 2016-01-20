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
*      IH.h - Module header for INSO Corporation                        *
*                  Hyphenator                                           *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
         Description:
         IH.h contains typedefs and defines that can be utilized
         by other modules in the International Hyphenator.

         Cautions:
         Check sl.h and see if there is already a define that can
         be used.

*-----------------------------------------------------------------------*/

#ifndef IH_H
#define IH_H

/* error flags for RETURN codes  (ihbuff -> hyperr_flags)               */

#define IHINITERR   0x02 /* data file initialization ERROR              */
#define IHILLWLEN   0x01 /* word len < 0 or > IHMAXHYPLEN               */
#define IHLOCKERR   0x04 /* handles not locked before use               */

/* process flags for RETURN codes  (ihbuff -> hyppro_flags)             */

#define IHNOHYPH   0x04 /* word cannot be hyphenated, it                */
                                      /* has a colon (:) or period (.)           */
#define IHSLASHBR  0x02 /* slash break point in word                    */
#define IHHYPBR    0x01 /* Hard Hyphen break point in word              */

/* init flags for RETURN codes  (ihbuff -> hypinit_flags)               */

#define IHSEEKERR   0x20 /* Seek Error                                  */
#define IHHEADERR   0x10 /* data file header ERROR                      */
#define IHREADERR   0x08 /* Read Error                                  */
#define IHALLOCERR  0x04 /* Allocation Error                            */
#define IHOPENERR   0x02 /* File Open Error                             */
#define IHCLOSERR   0x01 /* File Close Error                            */

/* Task to perform                                                      */
#define    IHINITIALIZE    0
#define    IHHYPHENATE     1
#define    IHTERMINATE     2
#define    IHNGRAMS        3
#define    IHEXCEPTION     4
#define    IHPRECLITIC     5
#define    IHPOSTCLITIC    6
#define    IHLOCK          7
#define    IHUNLOCK        8

/* file open in binary */
#define IHUNTRANS OPEN_RB

/* Flags for the cache table routine                                    */

#define IHCACHE_ADD    1
#define IHCACHE_SEARCH 2

/* Non-standard return codes                                            */

#define IHNORMALHYP 0
#define IHALTHYP    1

/* The code function is wrapped down to a lower value                   */

#define IHCODEWRAP        32749

/* Initial state for internal maps                                      */
#define IHZEROL   0L

#define IHBIT31           0x80000000L
#define IHBITS0_30        0x7FFFFFFFL

/* Size to read at one time                                             */
#define IHCHUNK   16384

/* Maximum number of rehashes in disk filter table                      */
#define IHHYPREHASH 20

/* MAX FILE NAME LENGTH                                                 */
#ifdef IBMPC
#define IHFNAMEMAX 64
#else
#define IHFNAMEMAX 128
#endif

/* MAX LENGTH OF WORDS FOR HYPHENATION                                  */
#define IHMAXHYPLEN      64

/* MIN LENGTH OF WORDS FOR HYPHENATION                                  */
#define IHMINHYPLEN      3

/* SIZE OF THE DISK BLOCKS                                              */
#define IHDISKBLOCK      512

/* General HEADER information (0 - 448 should NOT be modified)          */

#define CNVHEAD(h, o)  ( *(h+o) << 8 | *(h+o+1) & 0xFF )

/* RETURN CODES FROM DECODE */
#define IHRETNOTFOUND   0
#define IHRETFOUND      1
#define IHMANDHYPH      2
#define IHMANDHYPH1     3
#define IHALTHYPH       4

/* Product identification information                                   */
#define IHPRODCODE   7   /* The product code                            */
#define IHFSVERSION  3   /* The file structure version                  */
#define IHRAMFILE    1   /* file type                                   */
#define IHDISKFILE   2   /* disk file type                              */

#define IHHEADER     1024 /* Header size                                */

#define IHFILESIZ    700 /* Size of file in blocks.                     */
                         /* Disk portion ONLY if hdr/disk/ram           */
#define IHSCVERSION  702 /* Source Code version number                  */
#define IHLANGVER    712 /* Language Version                            */

#define IHFIRSTPOS   714 /* The first available hyph pos                */
#define IHLASTPOS    716 /* The last possible hyph pos                  */
#define IHNGRAMSIZ   718 /* The size of the max size of the ngram       */
#define IHSTRING1    724 /* The length of the master string for DB1     */
#define IHDISK_LEN1  726 /* The size of the compressed data for DB1     */
#define IHDISK_LEN2  728 /* Is there more then 64k of compressed data   */
#define IHNUMRAMPOINTERS 732 /* The number of ram pointers              */
#define IHHCLITLANG  734 /* The clitic processing flag */
#define IHNONRMCC    736 /* The size of the hash table for non RMCC's.*/

#endif /* IH_H */
/*  End of File - IH.H */
