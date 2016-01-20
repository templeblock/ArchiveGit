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


#ifndef _CMICS2_DEF
#define _CMICS2_DEF



/* chkflags */

#define NOLEN1  1
#define NOCODES 2



typedef struct _MORFO_BUF {

  int task;

  int retcode;

  SLPATH mstr_path; /* path of the database */

  char mstr_fn[ICFNAMEMAX+1]; /*  name of the database */

  long error_flags; /* in IC.H */  

  SLPATH hypmstr_path; /* path of the hyphenation database */

  char _hypmstr_fn[ICFNAMEMAX+1]; /*  name of the hyphenation database */

  unsigned char word[ICMAX+1]; /* input word */

  short len;

#ifdef CM_HYPHENATION

  SF_MAP hymap; /* hyphenation map */

  FLAG hyp_flag; /* if hyphenation is desired */
  
  FLAG hyreduce; /* avoid "stranding" a letter */
  
#endif

  short num_alt;

  unsigned char list[ICLISTSIZE];

  PFUINT4B att_tab;

  pcmCS cmCtrl;

  UINT4B inmap[2];
  UCHAR incap;

  INT4B outmap[2][3][ICMAXALT];
  UCHAR outcap[3][ICMAXALT];

  UCHAR ncapalt[ICMAXALT];

  UCHAR chkflags; /* set bit 1 if words of length 1 are ignored */
                   /* set bit 2 if code words are ignored */
   
  FLAG trail_colon;
  FLAG trail_period;

  FLAG found_morpho;

  INT4B memsize; /* determines how much memory is left for the
                  database (if == 0 then use the default value,
                  which is about 1 K) */
  
} MORFO_BUF,
   * ret_buf,
FAR * PFRET_BUF; 

#ifdef PROTOS

CHAR SLAPI_PASCAL Morfo(PFRET_BUF icpmbuf);
 
#endif

#endif
