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

#ifndef _CMGLOBH_
#define _CMGLOBH_

#include "sllibhdr.h"
#include "icfun.h"


/* note: C++ for NT has a typedef for boolean. Code will
break if you change the typedef below to anything else */

typedef unsigned char   boolean;


#define TSIZ    256

#define PROTOS          /* Prototypes used */

#define FASTCALL

#define INLINE

#define CMICS

/* if you don't need internal cache and frequency dictionary,
  don't define these two items: */
  
#define FRQ_LEX

#define CACHE_LEX

#define SPELLER_ONLY

#define MINIM_MEM

#ifdef ICS_HYPHENATION
#define CM_HYPHENATION
#endif

/*

Don't define these if you don't want to:

- Use personal dictionaries (CM_PLEX)
- Read the database to RAM memory (LEX_IN_RAM)
- Produce base forms and grammatical interpretations of words (BASICFORM_MODE)
- Trace morphological analysis (DEBUG_MODE)
- Lock the personal dictionaries during the update (CMLOCK)


#undef MINIM_MEM

*/

#ifdef MINIM_MEM

#undef FRQ_LEX

#undef CACHE_LEX

#endif

#undef CM_PLEX

#undef LEX_IN_RAM

#undef BASICFORM_MODE

#undef DEBUG_MODE

#undef CMLOCK


/* Spelling verification alone is the detault mode */

#ifndef SPELLER_ONLY

#ifndef BASICFORM_MODE

#define SPELLER_ONLY

#endif

#endif


#ifdef SPELLER_ONLY

#ifdef BASICFORM_MODE

#undef BASICFORM_MODE

#endif

#endif


#ifdef BASICFORM_MODE

#ifdef SPELLER_ONLY

#undef SPELLER_ONLY

#endif

#ifdef CACHE_LEX

#undef CACHE_LEX

#endif

#ifdef FRQ_LEX

#undef FRQ_LEX

#endif

#endif



typedef UCHAR strtype;

#define REGISTER SLREG
#define REG SLREG

#ifdef __GNUC__
#define itoa(i, s, b) sprintf(s, "%d", i)
#endif


#endif   /* _CMGLOBH_ */

