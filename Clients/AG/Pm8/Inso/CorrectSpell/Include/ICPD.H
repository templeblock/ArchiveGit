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
*        ICpd.h - Personal Dictonary Cover Api                          *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:
        This header includes all defines for the personal Dictionary
        API cover function ICPD()        


        Cautions:
        None.

*-----------------------------------------------------------------------*/


#include "pdexthdr.h"

#ifdef ICS_API_PD

#ifndef ICPD_H
#define ICPD_H

/* PDic Defines (task defines) */

#define PDIC_ADD                   1 /* To add a word to a RAM Database */
#define PDIC_DELETE                2 /* To delete a word from a RAM Database */
#define PDIC_DEFRAG                3
#define PDIC_GETWORD_SORT_START    4 /* Locks for the Database for Getword calls */
#define PDIC_GETWORD_NOSORT_START  5 /* Same as above, but not sorted */
#define PDIC_GETWORD               6 /* retrieve next set of (sorted/unsorted) words */
#define PDIC_GETWORD_STOP          7 /* unlocks the database for continued use */
#define PDIC_GETWORD_NOSORT_STOP   8 /* unlocks the database for continued use */
#define PDIC_GETALT                9

#define PDIC_START_DICT            10 /* Initialize Personal DB */
#define PDIC_SAVE                  11 /* Save but don't close the selected dictionary */
#define PDIC_SAVE_NOSORT           12 /* Save (unsorted) but don't close the selected dictionary */
#define PDIC_STOP_DICT             13 /* Terminate Personal DB */
#define PDIC_NOSAVE                14 /* Discard changes made to the Personal database */

#define PDIC_INIT_PD               15 /* Initialize the PD Application */
#define PDIC_TERM_PD               16 /* Terminate the PD Application */

/* pReturns in pIcIo */
typedef struct ICPD_rets
{

   UCHAR    szRetWord[PDWRDMAX+1];
   UINT2B   wDID;
   FLAG     bNegFlag;
   UCHAR    byCapCode;
   SF_MAP   mHypMap;
   UCHAR    pAltBuff[PDWRDMAX+1];

}  ICPD_RET,
   *PICPD_RET,
   NEAR *PNICPD_RET,
   FAR *PFICPD_RET;


/* PDic() IO structure */

typedef struct PD_ic_io
{
    UCHAR   task;     /* INPUT; Task to perform, defined obove */
    UINT2B      wPID; /* PID for task */
    UINT2B      wDID; /* DID for task */

    /* Dictionary task members */

    /* INPUT;Name of the file (including extension) that contains
       Personal Database. If creating an empty RAM database this
       should be set to NULL */
    UCHAR      szOpenFilename[PD_MAXFILENAME+1];

    /* INPUT;The path (or volume reference number in Macintosh environments)
       of the personal database */
    SLPATH     OpenPath;

    CHAR       byOtype; /* INPUT; Type of open DB file in pddefine.h */
    CHAR       byCtype; /* INPUT; Type of Close DB file in pddefine.h */

    /* INPUT;Name of the file (including extension) will be used
       to save the contents of the RAM Database. 
       should be set to NULL */
    UCHAR      szCloseFilename[PD_MAXFILENAME+1];

    /* INPUT;The path (or volume reference number in Macintosh environments)
       of the directory of szCloseFilename */
    SLPATH     ClosePath;

    /* INPUT;The path (or volume reference number in Macintosh environments)
       of the temporary files */
    SLPATH     tmpPath;

    UINT2B     wDictSize;    /* INPUT;The initial size of the RAM database */
    UCHAR      byDictlang;    /* INPUT;Language Code defined in sllang.h  */
    UCHAR      bySoftHypChar; /* INPUT;Input soft hyphen character */

    /* Add and Delete Members */
   UCHAR    szWord[PDWRDMAX+1]; /* INPUT;The word to be added or deleted
                                         from a RAM databse */
    
    /* Add members */
   FLAG        bNegWordFlag; /* INPUT;Flag to indicate whether the input
                                      word should be marked as negative */

    /* Add members and GetAlt Members */
    /* INPUT;Pointer to the buffer which contains a replacement
             word or a phrase for a negative word input */
   PFUCHAR     pAltBuff;
   UINT2B      wAltBuffSize; /* INPUT;The size of the AltBuff obove */

    /* Getword Return members */

   UCHAR       list_num; /* INPUT;The number to list for each GETWORD call*/


   /* Status Information */
   SL_STATUS   xStatus; /* OTPUT;Embedded status structure and possible 
                           error codes for the last task performed */

   UINT2B     byNumRets;       /* number of words returned in pReturns */
   PFICPD_RET   FAR *pReturns; /* An array of FAR PD_RET, which will hold */
                               /* PD alternatives */


}  PD_IC_IO,
   *PPD_IC_IO,
   FAR *PFPD_IC_IO;

#endif
#endif
/*  End of File - ICpd.h */
