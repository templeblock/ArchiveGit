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
 * REmaccvt.h - Macintosh character conversion file.                        *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:

Creation Date:
    5-31-1989   L Tang

Change Record:
    Date    Author      Comment
    21-feb-92	RAS	ED names -> RE

*---------------------------------------------------------------------------*/
                                 /* Convert macros for typesetting  */
#ifndef REMACCVT_H

#define REMACCVT_H

#define  CVT_E_UPLTBD_M   "</LB>"/* end of uppercase light bold     */
#define  CVT_S_UPLTBD_M   "< LB>"/* following character(s) in   */
                                 /* uppercase light bold    */

                                 /* 'e' in italic;      */
#define  CVT_LOE_IT_M     "< IT>e</IT>"
                                  /* italic th */
#define  CVT_LOTH_IT_M    "< IT>th</IT>" 
                                 /* italic ae ligature */
#define  CVT_LOAELIG_IT_M "< IT>ae</IT>"


#define  CVT_EMDASH_M     "--"   /* substitute for emdash   */

/* See slascii.h for macro conversion in standard ASCII */

/* See slchrmcs.h for macro conversion in DEC extended character set    */

#endif /* REMACCVT_H */
/* End of file - REmaccvt.h
*/
