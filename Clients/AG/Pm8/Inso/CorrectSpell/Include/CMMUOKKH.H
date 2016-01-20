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


# ifndef _CMMUOKKAH_
# define _CMMUOKKAH_

#include "cmglob.h"
#include "cmlexich.h"
#include "cmmustru.h"


#ifdef PROTOS
/* initialize muokkain. Open and read muokkain rules, allocate memory */
extern boolean GetMuokkain(INT2B FAR *numofsymbols,INT2B FAR *numofrules, pcmCS cmCtrl);

/* close muokkain, free memory */
extern  void CloseMuokkain(pcmCS cmCtrl);

/* make one global muokkain product, return TRUE if succeeded */
extern  boolean MakeMuokkainRecord(pcmCS cmCtrl);

/* main function of module. Walk through rule structure and make products */
extern  void Muokkain(stringtype FAR *word,lex_producttype FAR *firstpartinterp,
#ifndef SPELLER_ONLY
                      boolean FAR *qualifierpart,productset FAR *planeset,
#endif                      
                     pcmCS cmCtrl);
                     
#endif /* PROTOS */

#endif   /* _CMMUOKKAH_ */

