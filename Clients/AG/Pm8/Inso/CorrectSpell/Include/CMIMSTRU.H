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

#ifndef _CMIMSTRU_DEF
#define _CMIMSTRU_DEF


typedef UINT2B  poolindex;
typedef UINT2B  intermlexptr;


/* own settype */
typedef struct NAMEBSET {
        setword S[6];
} namebset;

#define NAMEBSSIZE 12

typedef enum ILEXSEARCHTYPE {
        unfit, indivisible, noclitics, nosuffix, nocase
} ilexsearchtype;

typedef UCHAR wordlength;

typedef struct ILEXLEXIMETYPE  {
        unsigned int next : 11;
        unsigned int jlength : 5;
        UINT2B rootword;
#ifdef BASICFORM_MODE
        UINT2B baseword;
        UCHAR plength;
#endif        
        setpointer   interpretation;
        
} ilexleximetype;


#define ILEXSIZE 9

#define CMIL_BSIZE ICMAX 

typedef ilexleximetype       FAR *ilextabletype;

typedef UINT2B  hashvalue;
typedef UCHAR   firstchar;
typedef UCHAR   charbetween;
typedef UCHAR   lastchar;
 
#define HSPAC1 18 + 1
#define HSPAC2 18 + 1
#define HSPAC3 7 + 1

typedef struct {
        struct {
                struct {
                        intermlexptr A[HSPAC3];
                } A[HSPAC2];
        } A[HSPAC1];
} space;

#define SPACESIZE ((19*19*8) * 2)

#define OPTTSIZE (']' - 'A' + 1)

typedef struct OPTTABLE {
        firstchar      A[OPTTSIZE];
} opttable;


typedef CHAR            FAR *pooltype;


#endif
