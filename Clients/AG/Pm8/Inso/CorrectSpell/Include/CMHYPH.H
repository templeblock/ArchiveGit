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

#ifndef _CMHYPH_DEF
#define _CMHYPH_DEF

                      
#define COMPTOLEFTM 2
#define COMPTORIGHTM 2
#define HYLEFTM 2
#define HYRIGHTM 2
                      
#define MAX_HYP_CATS 2


typedef struct _hyp_cat {
    UCHAR min;
    UCHAR max;
    UCHAR lmargin, rmargin;
} hyp_cat,
  * phyp_cat,
  FAR * pfhyp_cat;
  
typedef struct _HYP_BUFF {

  UCHAR letters[ HYPSTRLEN ];
  INT2B numcodes[ HYPSTRLEN ];
  INT2B hyphens[ HYPSTRLPLUS ];
  INT2B wordstarts[ HYPSTRLEN ];
  INT2B wordends[ HYPSTRLEN ];
  INT2B ground;

  UCHAR hyphchar;
  CHAR vocbind;
  CHAR beware;
  UCHAR bewchar;
  CHAR paramsalreadyset;

  hyp_cat hc[MAX_HYP_CATS];
  
} hyphen_buff,
  * phyp_buff,
  FAR *pfhyp_buff;


boolean inithyphen( PF_SLPATH path, PFCHAR name, pfhyp_buff phyp );

void StrHyphenate(PFUCHAR outword, PFUCHAR inword, PFUINT4B hymap, 
		  INT2B startind, UCHAR hychar);

INT2B Hyphenate(PFUCHAR inword, PFUCHAR outword,  PFUINT4B hymap, INT2B startind, 
		pfhyp_buff phyp, UCHAR lmargin, UCHAR rmargin);

void EndHyphen(void);
		
UCHAR LMargin(pfhyp_cat hc, boolean qualifierpart, INT2B wlen);
UCHAR RMargin(pfhyp_cat hc, boolean qualifierpart, INT2B wlen);


#endif

