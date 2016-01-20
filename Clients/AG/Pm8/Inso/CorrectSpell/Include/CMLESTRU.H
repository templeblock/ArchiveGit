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


#ifndef _CMLESTRU_DEF
#define _CMLESTRU_DEF


#ifdef MINIM_MEM

#define POOLSIZE 0
#define MAX_SEARCHES 0
#define MAX_MATCHES 20
#define MAX_MATCHES_PER_SEARCH MAX_MATCHES

#else

#define POOLSIZE 512
#define MAX_SEARCHES 35
#define MAX_MATCHES 130
#define MAX_MATCHES_PER_SEARCH 25

#endif


typedef struct {
  UCHAR s[LEXCLASSIZE];
} mlexclasset,
  FAR *pfmlexclasset;


#define MAX_ENDING 4

typedef struct {
  UCHAR ending[MAX_ENDING];
  UCHAR len;
  mlexclasset mlc;
} grammar_info;

#define MAX_DIFF_BFLEN 6

typedef struct {
  UCHAR diffs[MAX_DIFF_BFLEN];
  UCHAR nonbasicformlen, len;
} _basicform_data;

typedef struct {
  UINT4B size;
  UCHAR isleaf;
  UCHAR coverlen;
  UCHAR middlelen;
  UINT2B soffs;
  UINT2B groffs;
  UCHAR diffbasicform;
  UINT2B _cmbasicformoffs;
} expanded_node,
  * pexpanded_node,
  FAR * pfexpanded_node;


typedef struct {
  boolean holeword;
  UINT2B grammarind;
  grammar_info grammar;
#ifdef BASICFORM_MODE  
  _basicform_data basicform;
#endif  
  UCHAR baselen, hyphen;
  UINT4B capmap;
  lextype          lextype_;
  UCHAR lexind;
} foundwordtype,
  FAR * pffoundwordtype;

typedef struct {
  strtype FAR *word;
  foundwordtype FAR *first_word;
  foundwordtype FAR *last_word;
} searchtype;

typedef struct {
  UCHAR treebits;
  UCHAR mlenbits;
  UCHAR suffixbits;
  UCHAR grbits;
  UCHAR coverbits;
  UCHAR _mbasicformbits;
  INT4B size;
  INT4B bitsize;
  INT4B offs;
} tree_data;


#define TREETABSIZE 18

typedef struct {
  UCHAR numoflexclasses;
  UINT2B sizeofsufftab;
  UINT2B numsuffixes;
  UINT2B sizeofbasicformtab;
  UINT2B sizeofgrinfotab;
  UINT2B numgrinfos;
  UINT2B numbasicforms;
  UCHAR  numoftrees;
  UINT4B sizeoftrees;
  UINT2B sizeoffrqlex;
  UCHAR treemap[MAXCHARS];
} size_data;

#define SIZETABSIZE 276


typedef struct {
  CHAR ct8;
  PFUCHAR start;
  PFUCHAR start2;
  PFUCHAR cp;
  INT4B foffs;
  INT4B bytesleft;
  UCHAR two_blocks;
} bitpointer,
  * pbitpointer,
  FAR * pfbitpointer;


typedef struct {
  UINT4B size;
  INT4B foffs;
  PFUCHAR start;
  PFUCHAR start2;
  UCHAR two_blocks;
} treepool_struct;


typedef struct {
    SL_FHANDLE fp;
    UINT2B size;
    INT4B foffs;
    UCHAR FAR *buffp;
    INT2B boffs;
} node_buff;



/* --- lexicon product types, external: --- */
#define LEXPRODUCTMAX     40


typedef INT4B        charindex;

typedef struct LEX_PRODUCTTYPE {
        lextype          lextype_;
        nameset      interpretation;
        stringlen       hyphen;
        productind        muokkainnum;
        cap_struct capcode;
        
#ifdef BASICFORM_MODE
        _basicform_data basicform;
#endif
        charindex charptr;
} lex_producttype;


typedef lex_producttype    FAR *(__lexproducttable[LEXPRODUCTMAX + 1]);
typedef lex_producttype    FAR *(FAR *_lexproductptr);



typedef struct LEXRESULTTYPE {
        boolean                 lexcompound;
        stringlen               lexhyphen;
        foundwordtype        FAR  *lexcandidate;
        cap_struct lexcapc;
        struct LEXRESULTTYPE   FAR  *nextcandidate;
} lexresulttype;

typedef lexresulttype     FAR *lexptrtype;

typedef struct LEXPTRTABLE {
        lexptrtype       A[MAXLEXRESULTS + 1];
} lexptrtable;


#endif















