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
*       IHhypext.h - International Hyphenator external data and retcodes*
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
          Description:
          This header contains everything needed to link to the
          English Hyphenator

          Cautions:
          None.

*-----------------------------------------------------------------------*/

#ifndef IHHYPEXT_H
#define IHHYPEXT_H

RETCODE     IHdecode   (PWFUCHAR, INT2B, INT2B, INT2B, PWFUCHAR, PFIHBUFF );
RETCODE     IHbsearch     (PWFUCHAR, PFIHBUFF );
RETCODE     ScrambleAndFind( PWFUCHAR, INT2B, INT2B, CHAR,PWFUCHAR, PFIHBUFF);
RETCODE     IHcache       (PWFUCHAR, INT2B, INT2B, PFIHBUFF);
RETCODE     IHclean       (PWFUCHAR, INT2B, PFIHBUFF);
RETCODE     IHintget      (PFIHBUFF);
RETCODE     IHgetmap      (PWFUCHAR, INT2B, PFIHBUFF);
RETCODE     IHterm        (PFIHBUFF);
RETCODE     IHhyp         (PFUCHAR, PFIHBUFF);
RETCODE     IHinit        (PFIHBUFF);
RETCODE     IHintchk      (PFUCHAR, UCHAR);
RETCODE     CloseRamFile  (SL_FHANDLE, PFIHBUFF, RETCODE);
#ifdef MAC_HANDLES
RETCODE     IHhndlck      (PFIHBUFF, FLAG);
#endif

#endif /* IHHYPEXT_H */
/* end file ihhypext.h */

