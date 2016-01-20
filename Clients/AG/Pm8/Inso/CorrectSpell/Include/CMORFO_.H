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

# ifndef _CMORFO_H_
# define _CMORFO_H_

/* included Morfo header files */

# include "cmorfoh.h"
# include "cmics.h"
# include "cmstrinh.h"
# include "cmmuokkh.h"
# include "cmjaotih.h"

/* timer definitions */
#ifdef TIMER
extern INT4B     jaotim, muoktim, lextim, totaltime;
#endif


typedef struct ASCIISET {
        setword S[9];
} asciiset;



/* --- external variables: --- */

extern asciiset      asciivowels, _frontvowel_, _backvowel_;
extern nameset       classet, subset, mainset, nominalset,
                        _mcomparatset, numberset, timeset, formset,
                        suffixset, Affixset, _cliticset,
                        numset, jset, empty;


#if defined(CM_PLEX) | defined(BASICFORM_MODE)
                        
#ifdef PROTOS
extern  boolean MakeProductRecord(stringtype FAR *syote,lex_producttype FAR *sktietue,__basicformtype FAR *mproduct,
                                  boolean qualifierpart,productset FAR *planeset, pcmCS cmCtrl);
#endif /* PROTOS */

#endif

#endif  /* _CMORFO_H_ */

