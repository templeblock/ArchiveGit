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


#ifndef _CMJASTRU_DEF
#define _CMJASTRU_DEF

typedef UCHAR   nodeptr;
typedef UCHAR   dataptr;

typedef enum CONDITIONCHARS {
        illegal, morigin, pseparator,
        maa, mee, mii, moo, muu,
        map, mep, mip, mop, mup,
        ma, mb, mc, md, me, mf, mg, mh, mi, mj, E2_mk, ml,
        mm, mn, mo, mp, mq, mr, ms, E3_mt, mu, mv, mw, mx, mz,
        extrach
} conditionchars;

typedef struct _MCONDITIONSET {
        setword S[4];
} __conditionset;

#define CONDSSIZE 8

typedef struct _CONDITIONQUEUE {
        conditionchars      A[STRINGMAX+1];
} _conditionqueue;



/* --- jaotin product types, external: --- */

#define JPRODUCTMAX    32  /* was 64 */


typedef enum VOWELTYPES {
        neutralvowel, frontvowel, backvowel
} voweltypes;

typedef enum ENDINGTYPE {
        stem, classending, mainending, nominalending, affixending,
        comparatending, numberending, formending, suffixending, cliticsending
} endingtype;

typedef struct AXIS {
        setword S[9];
} axis;

typedef struct ENDINGTABLE {
        stringlen       A[cliticsending + 1];
} endingtable;

typedef struct _PRODUCTTYPE {
        axis          divisiontype;
        nameset      partitioninterp;
        endingtable      paatteet;
        voweltypes       vokharm;
        boolean         ok;
} _producttype;

typedef _producttype     FAR *(jaotinproducttable[JPRODUCTMAX]);
typedef _producttype     FAR *(FAR *_jaotinproductptr);


typedef enum RULECLASSES {
        obligatory,
        ynom,
        cl1,
        pp,
        ten1, ten2, ten3, ten4, ten5, ten0,
        ca1, ca2, ca3,
        num1, num2, num3, num4,
        pas1, pas2, pas3,
        par1, par2, par3, par4, par5, par6,
        inf1, inf2, inf3, inf31, inf4, inf41, inf42, inf5,
        komp1, komp2, komp3, komp4, komp5,
        der1, der2, der3,
        ord1, ord2, ord3, ord4, ord5,
        col1,
        autoend
} ruleclasses;

typedef struct _RULECLASSET {
        setword S[5];
} _ruleclasset;

#define RULECLSSIZE 10

typedef UCHAR   ruleclptr;
typedef UCHAR   condsetptr;
typedef UCHAR   namesetptr;

#define MAXNODES ('Z' - '?' + 1)

typedef struct ALPHABETTABLE {
        nodeptr    A[MAXNODES];
} alphabettable;

typedef struct NODETYPE  {
        dataptr    data;
        alphabettable     next;
        ruleclptr  lock;
        ruleclptr  gate;
} nodetype;

#define NODESIZE (MAXNODES + 3)

typedef struct DATATYPE {
        ruleclptr  ruleclasses;
        condsetptr   leftcond;
        condsetptr   left2cond;
        ruleclptr  contin;
        namesetptr   interpretation;
        endingtype     ending;
        dataptr    secondinterp;
} datatype;

#define DATASIZE 8

typedef nodetype     FAR *nodetabletype;
typedef datatype     FAR *datatabletype;


#endif

