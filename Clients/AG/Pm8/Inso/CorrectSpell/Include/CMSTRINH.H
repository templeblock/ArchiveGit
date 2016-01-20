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

# ifndef _CMSTRINGH_
# define _CMSTRINGH_

#include "cmglob.h"
#include "cmorfoh.h"
#include "cmststru.h"

/* defines */
#define PSTRLEN( s )  ( s->len )
#define SSTRLEN( s )  ( s.len )
#define EMPTYSTRING( s )  ( s.len = ZERO )

#define PSTR( s, i )  ( s->str.A[i] )
#define SSTR( s, i )  ( s.str.A[i] )

#define HARDHYPHEN '-'

#define DOTCHAR '.'

typedef UCHAR   stringind;


void SetCharflags(stringtype FAR *inputword, stringlen FAR *in_apostr, pcmCS cmCtrl);

/* prototypes */
#ifdef PROTOS
/* turns the word word around */
extern  void ReverseWord(PFUCHAR word);

/* returns true if strings str1 and str2 are equal, otherwise false */
extern  boolean Equalstring(stringtype FAR *str1,stringtype FAR *str2);

/* this function is replaced by macro */
/*extern  void Emptystring(stringtype *str1); */

/* concatenates ch1 to end of str1  */
extern  void Catchar(CHAR ch1,stringtype *str1);

/* concatenate C-string lit1 at end of Pascal string str1 */
extern  void Catctring(CHAR *lit1,stringtype FAR *str1);

/* concatenate str2 at end of str1 */
void Catstring(register stringtype FAR *str2, register stringtype FAR *str1);

/* append blancs to right-justify string at position 'pos'*/
void RightJustification(INT2B pos,stringtype *queue);
#endif /* PROTOS */

#endif /* _CMSTRINGH_ */

