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


#ifndef _CMLEXIC_DEF
#define _CMLEXIC_DEF

#include "cmlexsch.h"
#include "cmglob.h"

#define MAX_NON_BASELEN 10

#define REMOVECHAR 1
#define INSERTCHAR 2
#define RECAP      3
#define CHVOWELS   4



#ifdef PROTOS

boolean MakeWordSearch(stringtype FAR *inputword,lextype lextype_,
                            lex_producttype  FAR *firstpartinterp,productind mind,
                                pcmCS cmCtrl);
boolean GetLexicon(PFUCHAR filename, pcmCS cmCtrl);
void CloseLex(pcmCS cmCtrl);
void InitSearch(pcmCS cmCtrl);

boolean
MatchWords(boolean _firstpart, __basicformtype FAR *wordcandidate, mlexclasset FAR *mlc,
           lex_producttype FAR *firstpartinterp, CHAR suffix, stringlen hyphen, cap_struct FAR *tmp_capc,
            pcmCS cmCtrl);
boolean MakeAlternative(stringlen pos, UCHAR ch, UCHAR task, pcmCS cmCtrl);
boolean MakeLexRecord(pcmCS cmCtrl);
#endif
#endif
