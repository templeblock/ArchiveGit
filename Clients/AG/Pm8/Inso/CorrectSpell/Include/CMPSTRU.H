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

#ifndef _CMPSTRU_DEF
#define _CMPSTRU_DEF


/* mode, IN-parameter */
typedef enum MODES {
        all, checkspelling, addword, debug, removeword
} modes;


typedef enum LEXTYPE {
        nolexic, mainlex, intermediate, addlex, E6_compound, cachelex, frqlex
} lextype;

/* interpretation set type */
typedef struct NIMIJOUKKO {
        unsigned short int S[8];
} nameset;


#define STRINGMAX       64

/* result type */
#define PRODUCTMAX    32
                                    

typedef struct {
    UCHAR dict_cap;
    UINT4B dict_map[2];
} cap_struct; 
                                    

typedef struct PRODUCTTIETUE {
        unsigned char basicform[STRINGMAX];
        nameset      interpretation;
        unsigned char      startingpoint;
        unsigned short int  contpart;
        cap_struct capcode;
        unsigned char lexnum;
				lextype lexicon;        
} productrecord;

typedef productrecord     *(producttable[PRODUCTMAX]);

#endif
