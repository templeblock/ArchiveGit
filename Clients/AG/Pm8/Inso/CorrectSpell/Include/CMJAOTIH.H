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

# ifndef _CMJAOTINH_
# define _CMJAOTINH_

#include "cmglob.h"
#include "cmlexich.h"
#include "cmjastru.h"


extern __conditionset       vowels;

/* prototypes */
#ifdef PROTOS
/* initialization function. Reads jaotin rules to memory.
 * Allocates memory to jaotin products. Inits global sets.
 */
extern boolean GetJaotin(nodeptr FAR *solmulkm,dataptr FAR *tietolkm, pcmCS cmCtrl);

/* frees space used by jaotin products */
extern  void CloseJaotin(pcmCS cmCtrl);

/* function returns the location in a word where stem ends */
extern  UCHAR FindStemBoundary(axis divisiontype,stringlen length);

/* function gets a word to be interpreted and makes jaotin products. Calls
 * intermediate lexicon and muokkain to check the suggestions.
 */
extern  void Interpret(stringtype FAR *converted,_conditionqueue FAR *condword,stringtype FAR *upperword,
                        lex_producttype FAR *firstpartinterp,voweltypes vowelsinword,
                        boolean qualifierpart,productset FAR *planeset, pcmCS cmCtrl);
#endif /* PROTOS */
#endif  /* _CMJAOTINH_ */

