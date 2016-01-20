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

*---------------------------------------------------------------------------*
*                                                                           *
*     ihbuff.h - Module header for INSO Corporation       Hyphenator        *
*                                                                           *
*---------------------------------------------------------------------------*
*                                                                           *
       Description:
       ihbuff.h contains the definition of the work structure.

       Cautions:
       Check sl.h and see if there is already a define that can
       be used.

*--------------------------------------------------------------------------*/

#ifndef IHBUFF_H
#define IHBUFF_H

/*      Hyphenator Work Buffer                                     */

typedef struct _IHBUFF
{
     INT2B   task;                   /* Task to perform:                   */
                                     /* 0     -     Initialize             */
                                     /* 1     -     Hyphenate              */
                                     /* 2     -     Terminate              */

     RETCODE retcode;                /* Return code                        */

     SF_MAP  hypmap,                 /* 64 bit hyphenation map             */
             alt_map,                /* alternate hyphenation map
                                       if mand_flag is set then this contains
                                        mandatory hyphenation, otherwise
                                        it contains suppressed hyphenation */
             hardhyp_map,            /* hard hyphen map                    */
             cap_map;                /* input cap map                      */

    SLPATH   tab_path;              /* Path or Volume ref. number          */
    CHAR tab_fn[IHFNAMEMAX + 1];    /* RAM info                            */
                                  
   UCHAR   alt_spell[IHMAXHYPLEN+1],/* alternate spelling                 */
           lang,                    /* Language selected                  */
           cap_code,                /* input cap code                     */
           mand_flag,               /* mandatory hyphen flag              */
           hyperr_flags,            /* IHhyp() error codes                */
           hyppro_flags,            /* IHhyp() process codes              */
           hypinit_flags;           /* IHhyp() initialization error codes */

   PFUINT4B  att_tab_ptr;           /* Character attribute table         */
 
    ORF_IHCTLBUFF_IH    ofihtlbuff;  /* Pointers to internal control buffer */

} IHBUFF, NEAR *PIHBUFF, FAR * PFIHBUFF;

#endif /* IHBUFF_H */
/* end file ihbuff.h */
