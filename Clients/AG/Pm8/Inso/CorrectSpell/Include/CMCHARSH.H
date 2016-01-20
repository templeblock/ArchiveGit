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

# ifndef _CMMERKITH_
# define _CMMERKITH_

#include "cmglob.h"
#include "cmstrinh.h"


#ifdef PROTOS
/* initialize conversion */
extern  void ConversionInit(void );

/* change input word to Morfo special words needed by jaotin and muokkain */
extern  void CnvInput(stringtype FAR *inputqueue,stringtype FAR *thisword,_conditionqueue  FAR *condword,stringtype FAR *upperword,
                          voweltypes FAR *vowelsinword, stringlen FAR  *out_apostr, pcmCS cmCtrl);
#endif /* PROTOS */

#endif /* _CMMERKITH_ */

