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
*        ICheader.h -   International CorrectSpell(TM) Software         *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:
        This header contains all defines for the International 
        CorrectSpell(TM) Software database headers. 

        This header should be included by all modules of the ICS.

        Cautions:
        None.
*-----------------------------------------------------------------------*/
#ifndef ICHEADER_H
#define ICHEADER_H

             /* Location of information in the headers */

#define ICHDBCLASS   700   /* UINT2B DB class. Current class is 302 (src)     */
#define ICHEXTUPD    702   /* UCHAR Number of times that Extend touched file  */
#define ICHEXTDATE   704   /* The last date that ICS DB Extend touched file   */
#define ICHPOSTMAX   712   /* UCHAR Size of the largest postclitic            */
#define ICHPREMAX    714   /* UCHAR Size of the largest preclitic             */
#define ICHCLITLANG  716   /* FLAG Does this DB want clitic processing?       */
#define ICHCMPLANG   718   /* FLAG Does this DB want compound processing?     */
#define ICHSCANLANG  720   /* FLAG Is this a Scandanavian Lang?               */
#define ICHSLATTGLB  722   /* UCHAR Which SLATTGLB table to use.              */
#define ICHCHARSET   724   /* UCHAR Which character set to use.               */
#define ICHADDITDB   726   /* FLAG  Is there an additional Database appended? */
#define ICHAVLMODE   728   /* Modes avaliable in this database */
/*                   730                                                      */

#endif /* ICHEADER_H */

/* End of file ICheader.h */
