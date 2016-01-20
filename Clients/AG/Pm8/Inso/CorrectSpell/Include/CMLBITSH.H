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


#ifndef _MLBITS_DEF
#define _MLBITS_DEF

#include "cmglob.h"
#include "cmutil.h"

#ifdef LEX_IN_RAM

#if defined(MSDOS)|defined(WINDOWS)

#define CHECKPTR(bp, offs)\
    if ((bp)->two_blocks && (INT4B)offs > (bp)->bytesleft) {\
        (bp)->cp = bp->start2 + (UINT2B)((INT4B)sizeof(UINT4B) - (bp)->bytesleft);\
        (bp)->two_blocks = FALSE;\
    }

#else

#define CHECKPTR(bp, offs)

#endif


#else

#define CHECKPTR(bp, offs)\
    if ((INT4B)offs > (bp)->bytesleft)\
         CheckPtr((bp), cmCtrl)

#endif


#ifndef SPELLER_ONLY

#define SkipBasicForm(bitskipsize, bitskipsize2, curr_node, curr_bp, curr_tree)\
{\
    if (cmCtrl->prolex) {\
      \
    (curr_node).diffbasicform = UCGetNBits(1, &(curr_bp), cmCtrl);\
    \
    if ((curr_node).diffbasicform) {\
        bitskipsize2 = (UCHAR)((curr_tree)->_mbasicformbits + 1);\
    } else {\
      bitskipsize2 = 1;\
    }\
        IncrNBits((UINT4B)bitskipsize2, &(curr_bp));\
        bitskipsize += (INT4B)bitskipsize2;\
\
    }\
}

#else

#define SkipBasicForm(bitskipsize, bitskipsize2, cn, cb, ct)

#endif


#ifndef SPELLER_ONLY

#ifdef BASICFORM_MODE

#define PTRTOBASICFORM(ptr) cmCtrl->curr_word->basicform = (ptr);

#else

#define PTRTOBASICFORM(ptr)

#endif

#define FillBasicFormOfNode(bitsize, curr_node, curr_bp, curr_tree)\
{\
  (curr_node).diffbasicform = UCGetNBits(1, &(curr_bp), cmCtrl);\
\
  IncrNBits(1, &(curr_bp));\
\
  if ((curr_node).diffbasicform) {\
    \
    bitsize = (curr_tree)->_mbasicformbits;\
\
    (curr_node)._cmbasicformoffs = UI2BGetNBits(bitsize, &(curr_bp), cmCtrl);\
\
    IncrNBits(bitsize, &(curr_bp));\
\
    PTRTOBASICFORM(cmCtrl->basicformitems[(curr_node)._cmbasicformoffs]);\
\
    bitsize++;\
  } else {\
    cmCtrl->curr_word->basicform.len = ZERO;\
    bitsize = 1;\
  }\
}
 
#endif



boolean CheckPtr(pfbitpointer cbp, pcmCS cmCtrl);

UCHAR FillSizeOfSubtree(pfexpanded_node curr_node, pfbitpointer curr_bp,
                       tree_data FAR *curr_tree, UCHAR reducedbits,
                        pcmCS cmCtrl);

void StartBits(pfbitpointer bp, treepool_struct FAR *treetop);

void IncrNBits(UINT4B n, pfbitpointer bp);

UINT4B UI4BGetNBits(UCHAR n, pfbitpointer bp, pcmCS cmCtrl);

UINT2B UI2BGetNBits(UCHAR n, pfbitpointer bp, pcmCS cmCtrl);

UCHAR UCGetNBits(UCHAR n, pfbitpointer bp, pcmCS cmCtrl);

#endif

