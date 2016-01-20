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


# ifndef _CMSETSH_
# define _CMSETSH_

#include "cmglob.h"

#ifdef CM10
#include "ocmorfo.h"
#else
#include "cmorfoh.h"
#endif



# define NE(p1, p2) (!Eq(p1, p2))
# define CLAIMSET() (cmCtrl->SetTop = cmCtrl->SetSpace)
# define NEWSET() (*cmCtrl->SetTop = ZERO,cmCtrl->SetTop)

extern setptr   Conset[];

#ifdef PROTOS

/* returns pointer to union of two sets */
extern  setptr Union(setptr p1,setptr p2, pcmCS cmCtrl);

/* returns pointer to difference of two sets */
extern  setptr Diff(setptr p1,setptr p2, pcmCS cmCtrl);

/* returns pointer to interjection of two sets */
extern  setptr Inter(setptr p1,setptr p2, pcmCS cmCtrl);

/* returns true if sets are equal, otherwise false */
extern  boolean Eq(setptr p1,setptr p2);

/* returns true if first set is subset of the other, otherwise false */
extern  boolean Le(setptr p1,setptr p2);

/*
 * adds member m to set sp. Differs from ADDMEMBER in that if set
 * is too small for m, it is expanded. ADDMEMBER does not do that.
 */
extern  setptr Insmem(UINT2B m,setptr sp);

/* ? */
extern  setptr Saveset(setptr sp, pcmCS cmCtrl);

/* copies N bytes of set S2 to set S1 */
extern  void Setncpy(setptr S1,setptr S2,UINT2B N);

#endif

#endif  /* _CMSETSH_ */





