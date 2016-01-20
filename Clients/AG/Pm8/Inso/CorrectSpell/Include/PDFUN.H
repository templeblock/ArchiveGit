/***************************************************************************
*                   Copyright 1995 INSO Corporation                        *
*                Personal Dictionary Component Software                    *
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
*        PDfun.h - Personal Dictonary Defines                           *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:
        
        This header should be included in each product main header file
        immediatley after SLlibhdr.h


        Cautions:
        None.

        Creation Date:
        01-Jul-93   DLB     Ver. 1.0.0

*-----------------------------------------------------------------------*/

#ifndef PDFUN_H
#define PDFUN_H

/* OEM Compile Time Options (CTO's) */

/* Allows RD's to automatically expand in size when full condition reached */
#define PD_EXPAND_AUTO

#ifdef PD_EXPAND_AUTO
#define PD_AUTOEXPAND_INCREMENT_SIZE  10   /* Kbytes to increase an RD by  */
#endif

/* Allows RD's to automatically delete words randomly to free space when full */
#undef PD_DELETE_AUTO

#ifdef PD_DELETE_AUTO
#define PD_AUTODEL_MINWORDS   10   /* Min # words in a PD to allow Autodel */
#endif

/* Allows the RD to be saved and listed in sorted order */
#define PD_SORT_SUPPORT

/* Allows support of multiple dictionaries */
#define PD_DATABASE_MULTI

/* Allows support of Alternative information per entry */
#define PD_ALTWORD_SUPPORT

/* Undef for implementations where dictionaries are used only in RAM, 
   not saved to disk, not saved to temporary files, and not supporting 
   Alternative word entries  */
#define PD_FILEIO_SUPPORT

/* Conditional compile for Germanic Compounding Processing  */
#define PD_COMPOUND_PROCESS

/* Conditional compile for Romance Language Clitic Processing */
#define PD_BASE_CLITIC

/* Undef to remove all correction processing support */
#define PD_CORRECTION_SUPPORT

/* Undef to remove all ANAGRAMMING support */
#define PD_ANAGRAM_SUPPORT

/* Undef to remove all WILDCARDING support */
#define PD_WILDCARD_SUPPORT

/* Undef to remove all Supplemental dictionary support */
#define PD_SD_SUPPORT

/* Includes compression/decompression and merging functionality */
#define PD_COMP_MERGE

/* Undef to remove ability to read Version 1.0 SDs within PDcomp() 
   when PD_COMP_MERGE and PD_SD_SUPPORT are available */
#if defined(PD_COMP_MERGE) && defined(PD_SD_SUPPORT)
#define PD_V10_SD_READ
#endif

/* Apply Database Customizer rules after finding the word in Personal Dict. */
#define PD_APPLY_DC_RULES

/* undef to remove accent folding during correction */
#define PD_ACCENT_FOLD

/*-------------- Start of OEM modifiable defines -----------------------*/

/* Define Maximums */
#define PD_MAXDICTS	25	      /* Maximum number of DID's                  */
#define PD_MAXPROCS	5	      /* Maximum number of PID's                  */

/* Define RD full confitions */

#define PD_NEARFL	1000        /* Number of bytes for Near Full warning    */
#define PD_ISFULL   100	        /* Number of bytes for FULL error           */
#define PD_INDEXFL  5           /* Min # of indexes before full error       */

/* RD internal limits */

#define PD_RDBKSZ 1024        /* RD block size; sizeof(RD)=size*PD_RDBKSZ */

#define PD_RDMAX  95          /* Maximum # RD blocks; MAX SIZE (95*1024)  */
#define PD_RDMIN  1		      /* Minimum # RD blocks; MIN SIZE 512 Kbytes */
#define PD_RDDEF  10          /* Default # RD blocks                      */

/* Maximum Threshold of ASCII parsing errors */
#define PD_LINEERR_MAX  20    /* File read error after LINEERR_MAX errors */

/* PD task return maximum limits */
#define PD_MAXRET 20          /* Maximum number of PD returns             */

/* defines to control the size of the alternate buffer */
#define PDALTBUFSIZE 2048

/* we use this to increment the size of the read buffer */
#define PDALTBUFSIZEINCR 1024

/* this is the maximum size of an rd line including the alternate buffer */
#define PDALTMAXBUFSIZE (PDALTBUFSIZE + (2 * PDWRDMAX) + 1)

/*-------------- End of OEM modifiable defines  ------------------------*/

/*-------------- Start of INSO Product Defines -------------------------*/

#ifndef PD_DLL /* PD DLL's cannot have product driven CTO's             */

#if defined(SF_ICS) && !defined(SF_IPR)
#endif

#ifdef SF_II
#undef PD_ANAGRAM_SUPPORT
#undef PD_WILDCARD_SUPPORT
#undef PD_BASE_CLITIC
#endif

#ifdef SF_IPR
#endif

#ifdef SF_RE
#undef PD_APPLY_DC_RULES
#undef PD_V10_SD_READ
#endif

#ifdef SF_GCS
#endif

#endif /* PD_DLL */

/*-------------- End of INSO Product Defines -------------------------*/

#define PD_DEFAIM	0           /* Product ID default                       */

#ifndef PD_FILEIO_SUPPORT
#ifdef PD_ALTWORD_SUPPORT
#undef PD_ALTWORD_SUPPORT
#endif
#endif

#ifdef PD_APPLY_DC_RULES
#ifndef SF_APPLY_DC_RULES
#define SF_APPLY_DC_RULES
#endif
#endif

#ifdef PD_COMPOUND_PROCESS
#ifndef SF_COMPOUND_PROCESS
#define SF_COMPOUND_PROCESS
#endif
#endif

#ifdef PD_BASE_CLITIC
#ifndef SF_BASE_CLITIC
#define SF_BASE_CLITIC
#endif
#ifndef SF_CLITIC_PROCESS
#define SF_CLITIC_PROCESS
#endif
#endif

#ifdef PD_CORRECTION_SUPPORT
#ifndef SF_CORRECTION
#define SF_CORRECTION
#endif
#endif

#ifdef PD_ANAGRAM_SUPPORT
#ifndef SF_ANAGRAM
#define SF_ANAGRAM
#endif
#endif

#ifdef PD_WILDCARD_SUPPORT
#ifndef SF_WILDCARD
#define SF_WILDCARD
#endif
#endif

#if (defined SF_CORRECTION || defined SF_ANAGRAM || defined SF_WILDCARD || defined SF_BASE_CLITIC)
#ifndef SF_XTRA_DECODE
#define SF_XTRA_DECODE
#endif
#endif

#ifdef PD_ACCENT_FOLD
#ifndef SF_ACCENT
#define SF_ACCENT
#endif
#endif


#endif /* PDFUN_H */

/* End of File - PDfun.h */
