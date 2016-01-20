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

#ifndef _CMMUSTRU_DEF
#define _CMMUSTRU_DEF


/* --- muokkain product types, external: --- */

#define MPRODUCTMAX   64   /* was 128 */

/*typedef uint8   productind;*/



typedef enum GRADETYPE {
        ha, va, ne, et
} gradetype;

typedef enum CONSONANTGRADIATIONTYPE {
        norm, neutral, pathological
} consonantgradiationtype;

typedef struct BASICFORMTYPE {
        stringtype      word;
        nameset      wordclass;
        stringlen       baselen;
        consonantgradiationtype      _mconsonantgrad;
        gradetype      grade;
        productind        jaotinnumber;
        UINT2B          mrulenumber;
        lextype         ok;
} __basicformtype;

typedef __basicformtype      FAR  *(mproducttable[MPRODUCTMAX]);
typedef __basicformtype      FAR  *(FAR *_mproductptr);


#define AFFIXMAXLEN     5
#define MTABLESIZE     1024    /* Ei saa muuttaa */
#define MTAILLEN      6

typedef CHAR    stemtype;

typedef struct AFFIXTYPE {
        CHAR    A[AFFIXMAXLEN];
} affixtype;

typedef UINT2B  mparamptr;

typedef UCHAR   shortconsgradtype;
typedef UCHAR   shortgradetype;
typedef UCHAR   affixptr;

typedef struct MPARAMTYPE {
        stemtype   endofstem;
        stemtype   lengthcond;
        stemtype   blankplace;
        shortconsgradtype   _consonantgradation;
        shortgradetype gradeofword;
        affixptr    affix;
        setpointer   setconds;
        setpointer   gradeconds;
} mparamtype;

#define MPARAMSIZE 8

typedef UINT2B  ptrtoparameters;

typedef struct REFERENCETOPARAMS {
        ptrtoparameters    next;
        mparamptr       parameters;
} referencetoparams;

#define REFSIZE 4

typedef UINT2B  mhtptr;

#define MHTPSIZE 3

typedef struct MHTRECORD  {
        UINT4B                  ident;
        mhtptr              collisions;
        ptrtoparameters    listofparams;
} mhtrecord;

#define MHTRSIZE 8

typedef mhtrecord                FAR *mhttabletype;
typedef referencetoparams   FAR *_referencetabletype;
typedef mparamtype        FAR *paramtabletype;
typedef mhtptr              FAR *mtabletype;

#endif
