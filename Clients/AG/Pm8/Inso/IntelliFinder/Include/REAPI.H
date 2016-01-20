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

/*--------------------------------------------------------------------------*
 *                                                                          *
 * REapi.h - External Reference Engine function prototypes and              *
 *           definitions for the Aplication Programming Interface (API)     *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
    RE external forward function prototypes and declarations for the
    Application Programming Interface (API).

    This replaces the previous file EDfwdref.c

Creation Date:
    19-mar-92   ECH

Change Record:
    Date        Author  Comment
    06-nov-92   JCC     SF_RE safeguarding

*----------------------------------------------------------------------------*/

#ifndef REAPI_H

#define REAPI_H

/* This flag checks for SF_RE being defined in SFFUN.H, which is essential
 * to the RE getting the right CTOs turned on.  If SF_RE isn't defined, the
 * RE will compile nonsensically because REFUN.H won't get included.
 * This file, REAPI.H, is included independently from SFFUN.H, afterward, so
 * the check for SF_RE is done here.
*/
#ifndef SF_RE
You have not defined SF_RE in SFFUN.H.

/* Please do so by uncommenting the #define SF_RE line in that file or by 
 * defining SF_RE on the command line.
 * 
 * Do not remove this comment.  It insures proper compilation of the RE, 
 * which is reliant upon SF_RE's definition in SFFUN.H.
*/
#endif


/****************************************************************************
 *
 * API Function Declarations
 *
 ****************************************************************************
*/

REAPI REapp    (PFRE_APP_IO,     RE_OPERATION, RE_OPERATION);
REAPI REdb     (PFRE_DB_IO,      RE_OPERATION, RE_OPERATION);
REAPI REidxlst (PFRE_IDXLIST_IO, RE_OPERATION, RE_OPERATION);
REAPI REentlst (PFRE_ENTLIST_IO, RE_OPERATION, RE_OPERATION);
REAPI REentry  (PFRE_ENTRY_IO,   RE_OPERATION, RE_OPERATION);
REAPI REsearch (PFRE_SEARCH_IO,  RE_OPERATION, RE_OPERATION);
REAPI REstrtab (PFRE_STRTAB_IO,  RE_OPERATION, RE_OPERATION);
REAPI REtoken  (PFRE_TOKEN_IO,   RE_OPERATION, RE_OPERATION);
REAPI REetpar  (PFRE_ETPAR_IO,   RE_OPERATION, RE_OPERATION);
REAPI REPDdb   (PFREPD_DB_IO,    RE_OPERATION, RE_OPERATION);
REAPI REPDedit (PFREPD_EDIT_IO,  RE_OPERATION, RE_OPERATION);

/* Auxiliary functions
*/
REAPI REmemory  (VOID FAR * FAR *, VOID FAR *, RE_OPERATION, RE_OPERATION, UINT2B);
VOID  FAR PASCAL RErecap (PWFUCHAR, PWFUCHAR, UCHAR, UINT2B);


#endif /* REAPI_H */

/* End of file - REapi.h
*/
