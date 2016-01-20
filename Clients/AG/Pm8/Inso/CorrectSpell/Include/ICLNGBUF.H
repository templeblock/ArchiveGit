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
*    IClngbuf.h - User Structure Defines for Supplemental International *
*                 CorrectSpell(TM) Software                             *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *

    Description:
    This header contains the typdef for the additonal language processing
    buffer for the International CorrectSpell(TM) Software. 
    This header should be included by all modules of ICS and by 
    the User Application.

    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICLNGBUF_H
#define ICLNGBUF_H

#ifdef ICS_ADD_LANG

typedef struct _ICLNGBUF
{
                 /* Code (DLL) path and file name */
  SLPATH        lng_path;                /* IN; Path or Volume ref. number   */
  char          lng_fn [ICFNAMEMAX + 1]; /* IN; Function file name           */


                    /* Database #1 file information */
  SLPATH        lngdb1_path;             /* IN; Path or Volume ref. number   */
  char          lngdb1_fn[ICFNAMEMAX+1]; /* IN; Database file name           */

} ICLNGBUF, 
* PICLNGBUF,                       /* Buffer and pointer to buffer          */
FAR * PFICLNGBUF;                  /* Buffer and pointer to buffer          */

#else

typedef VOID     *PICLNGBUF;
typedef VOID FAR *PFICLNGBUF;

#endif

#endif /* ICLNGBUF_H */

/* End of file IClngbuf.h */
