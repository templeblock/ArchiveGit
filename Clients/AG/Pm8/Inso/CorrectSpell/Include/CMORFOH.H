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


# ifndef _CMORFOH_
# define _CMORFOH_

/* included Morfo header files */
# include "cmglob.h"        /* FAR, MSC */
# include "cmstruc.h"


/* define error strings: */

#define MORFOERROR 2

/* cmbuckio.c: */

#define LEX_OPEN_ERR    "Lexicon open error"          /* fatal */
#define FRQ_SEEK_ERR    "FRQ LEX seek error"          /* fatal */
#define FRQ_READ_ERR    "FRQ lEX read error"          /* fatal */
#define LEX_SEEK_ERR    "Lexicon seek error"          /* fatal */
#define LEX_READ_ERR    "Lexicon read error"          /* fatal */
#define LOCKING_ERR             "Locking error"     /* not used */
/* cmjaotin.c: */
#define JAOTIN_OPEN_ERR "Jaotin open error"            /* fatal */
#define JAOTIN_READ_ERR "Jaotin read error"            /* fatal */
#define JAOTIN_OVERFLOW_ERR     "Jaotin overflow"   /* not fatal */
/* cmlexic.c: */
#define LEXICON_OVERFLOW_ERR   "Leksicon overflow"    /* not fatal */
#define ADDED_OPEN_ERR        "Added.txt open error"
#define ADDED_WRITE_ERR       "Added.txt write error"
/* cmmuokka.c: */
#define MUOKKAIN_OPEN_ERR       "Muokkain open error"     /* fatal */
#define MUOKKAIN_READ_ERR       "Muokkain read error"     /* fatal */
#define MUOKKAIN_OVERFLOW_ERR   "Muokkaimen overflow"     /* not fatal */
/* cmorfo.c: */
#define PRODUCT_OVERFLOW_ERR      "Product overflow"       /* not fatal */
#define STACK_OVERFLOW_ERR      "Stack overflow"    /* fatal */
/* cmstdmem.c: */
#define MEMORY_ALLOC_ERR        "Memory allocation error"  /* fatal */
/* cmimlex.c: */
#define INTERMLEX_OPEN_ERR    "Intermediate lex. open error"                  /* fatal */
#define VSK_READ_ERR    "Intermediate lex. read error"                  /* fatal */

#define FIL_VERSION_ERR    "Version error in file"        /* fatal */


/* --- type definitions --- */
typedef enum AFFIXNAMES {
        noun, verb, particle,

        substantive, adjective, numeral, pronoun, adverb,
        preposition, conjunction, interjection, ordinal, propernoun,

        active, passive,

        nonnominal,
/* 1 */
        iparticiple, iiparticiple,
        iinfinitive, iiinfinitive, iiiinfinitive,
        ivinfinitive, vinfinitive,

        _comparative, superlative,

        singular, oblsingular, plural,

        indicative, imperative, _mconditional, potential,
/* 2 */
        nominative, essive, translative, partitive,
        genetive, accusative, genom, inessive,
        elative, illative, adessive, ablative,
        allative, abessive, comitative, intructive,
/* 3 */
        preesens, imperfect,

        sg1pers, sg2pers, sg3pers, thirdperson,
        pl1pers, pl2pers, pl3pers,

        negation,

        jsti, jttain, jminen, juus, jllinen, jlainen,
        
/* 4 */
        jmainen, jton,

        kaan, kin, pa, ko, han, s,

        asaset,

        ptyhnom, ptyhpos, ptyhimp,
        pta, pxa, pn,
        pen,
        
/* 5 */
        pden, pten, ptten,
        phvn, pvn, pseen, psiin,
        pt, pi, pj,
        pttu, ptu, pxu,
        pko, pkaa,
        pinfv, pinfdv, pinflv, pinfnv, pinfrv, pinftv,

        code_, abbrev, yhdosa, xka   /* note! */
} affixnames;


#define SLASHCHAR '/'
#define HYPHENCHAR '-'


#define NUMBER 0x0001
#define CODE   0x0002

#define COLONFL  0x0004

#define ENDHYPHEN 0x0008
#define LEADINGHYPHEN 0x0010
#define COMMON 0x0020
#define ENDDOT 0x0040
#define VOWELHYPHEN 0x0080
#define WLEN1 0x0100
#define LEN1 0x0200 

#define ENDSLASH 0x0400

#define APOSTRFL 0x0800

#define DOTFL 0x1000



/* --- product types needed in the interface, external: --- */

# define SETBITS 15


# define LEXMEMBER(m, sp)  ((sp)[(UINT2B)((m) / 8)] & (1 << ((UINT2B)((m) % 8))))

# define MEMBER(m, sp)  (((UINT2B)((UINT2B)m/(UINT2B)(SETBITS+1))<(UINT2B)(*(sp)))&&\
                         ((sp)[(UINT2B)(m)/(UINT2B)(SETBITS+1)+1]&\
                          (1<<((UINT2B)(m)%(UINT2B)(SETBITS+1)))))

# define ADDLEXMEMBER(m, sp) ((sp)[(UINT2B)(m) / 8] |= \
                                1<<(UINT2B)(m) % 8)

# define ADDMEMBER(m, sp) ((sp)[(UINT2B)(m)/(UINT2B)(SETBITS+1)+1] |= \
                                1<<(UINT2B)(m)%(UINT2B)(SETBITS+1))

# define DELMEMBER(m, sp) ((sp)[(UINT2B)(m)/(UINT2B)(SETBITS+1)+1] &= \
                                ~(1<<(UINT2B)(m)%(UINT2B)(SETBITS+1)))


typedef struct PRODUCTJOUKKO {
        setword S[8];
} productset;


/*************    FUNCTIONS FOR INTERFACING WITH MORFO     **************/
#ifdef PROTOS

INT2B     MorfoInit(modes, PFUCHAR, PF_SLPATH, pcmCS);

/* Must be called once to initialize Morfo.  First          *
 * parameter gives the processing mode:                                 *
 *      all   - normal processing                                    *
 *      debug    - trace analysis to stdout                             *
 *      checkspelling - check spelling only                                  *
 *      addword - add word to lexicon                                  *
 *      removeword   - remove words from lexicon                            *
 *      relax    - relaxed analysis (only for special use)              *
 */


boolean Morfo_Interpretation( pcmCS, PFUCHAR , INT2B , productptr , productind FAR * );


/* Main function of Morfo.  First parameter contains the word form to   *
 * analyze (a null terminated ascii character string).  Second parameter *
 * contains the result array having the number of                       *
 * items returned by the fourth parameter.  Fields of the item          *
 * record are:                                                          *
 *      basicform      - basic form                                    *
 *      interpretation        - interpretation                                *
 *      startingpoint       - morpheme starting pos in input word           *
 *      contpart        - index of succeeding morpheme in array         *
 * Here morpheme means part of a compound word: for non-compounds       *
 * startingpoint=1 and contpart=0.  Interpretation is a bitarray having     *
 * from the second item onwards the bits set according to enum          *
 * AFFIXNAMES, as defined in this file.  The Member -macro in           *
 * cmmakrot.h should be used for membership tests.                      *
 * Return value is false if analysis failed.                            *
 */

INT2B     MorfoEnd(pcmCS cmCtrl);

/* Should be called to terminate Morfo.  Closes all internal files      *
 * and releases dynamic memory used by Morfo.                           *
 */

void    MorfoError( PCHAR );

/* This function must be supplied by the driver program.  Morfo calls   *
 * the function when a fatal internal error occurs.  Parameter is set   *
 * to the error message.                                                *
 */

#ifdef BASICFORM_MODE

boolean NewProduct(productptr mproducts, productrecord *ptr,
                   producttable cmprod);

void _SetAffixJoint (boolean setflag, pcmCS cmCtrl);

#endif

#endif   /* PROTOS */



/************************************************************************/


#endif  /* _CMORFOH_ */

