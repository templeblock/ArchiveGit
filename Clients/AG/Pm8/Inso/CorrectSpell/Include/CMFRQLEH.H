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
*/

#ifndef _CMFRQLEXH_
#define _CMFRQLEXH_

#ifdef PROTOS

boolean
GetFrqLex(SL_FHANDLE fskt
#ifdef FRQ_LEX
, pcmCS cmCtrl
#endif
);

void CloseFrqLex(pcmCS cmCtrl);

boolean 
FrqLexSearch(strtype FAR *key, UCHAR keylen, strtype FAR *nonbase, UCHAR nonbaselen, pcmCS cmCtrl);

#endif

#endif
