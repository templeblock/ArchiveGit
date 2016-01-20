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

# ifndef _CMIMLEX_
# define _CMIMLEX_

#include "cmglob.h"
#include "cmimstru.h"


#ifdef PROTOS
/* gets the basic form of a word from quicklexicon */
extern  void PoolSearch(poolindex pointer,stringtype FAR *queue, pcmCS cmCtrl);

/* initializing function. Opens and reads lexicon file, allocates memory */
extern boolean GetIntermLex(intermlexptr FAR *nwords, pcmCS cmCtrl);

/* closing function, frees memory */
extern  void CloseIntermLex(pcmCS cmCtrl);

/* searches word from quicklexicon, makes lexicon products if found */
extern  void IntermLexSearch(boolean         beforejaotin,
                                 _producttype FAR *product,
                                 stringtype      root,
                                 voweltypes      vowelsinword,
                                 lex_producttype FAR *firstpartinterp,
                                 boolean         qualifierpart,
                                 boolean   FAR   *becameready,
                                 boolean FAR    *cmintermlexhomonym,
                                 pcmCS           cmCtrl);
#endif /* PROTOS */

#endif /* _CMIMLEX_ */

