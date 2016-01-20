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


#ifndef _MLEXIC_DEF
#define _MLEXIC_DEF

#include "icfun.h"
#include "cmglob.h"


#define MAX_SEARCH_STRLEN STRINGMAX


#define FIRST_LETTER(str) (*(str))


#define MIN_NBUF_SIZE 128L


#ifdef MINIM_MEM

#define NODE_BUF_SIZE MIN_NBUF_SIZE   /* must be >= MIN_NBUF_SIZE */

#else

#define NODE_BUF_SIZE 1000L   /* must be >= MIN_NBUF_SIZE */

#endif


typedef enum LEXCLASSTYPE {
        lexnoun, lexadjective, lexnumeral, lexadverb,
        lexpartofcompound, lexpropernoun, lexdiffbasicform, lexabbrev,
        lexverb, lex1pers, lexweakgrade, lexstronggrade,
        lexneutral, lexdot, lexbackvowels, lexfrontvowels,
        lexnosuffix, lexrelaxvowels, lex0capc,  lex1capc,
        lex2capc, lex3capc, lexerror
/* 23 items */         
} lexclasstype;


#ifdef CM_PLEX

#define SCHRET(n) (cmCtrl->plex ? AllPLexSearch(cmCtrl->plex, searchstr, nonbase, nonbaselen,\
                                       _mholewordsearch, n, cmCtrl) : (n))



#else

#define SCHRET(n) (n)

#endif

/* prototypes */
#ifdef PROTOS

boolean ReadLexicon(PFUCHAR lexname, pcmCS cmCtrl);
void EndLexicon(pcmCS cmCtrl);
UCHAR SearchFromLexicon(PFUCHAR searchstr, PFUCHAR nonbase,
          UCHAR nonbaselen, boolean _mholewordsearch, pcmCS cmCtrl);

#endif

#endif
