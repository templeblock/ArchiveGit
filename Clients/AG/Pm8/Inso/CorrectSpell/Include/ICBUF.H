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
 
*-----------------------------------------------------------------------*
*                                                                       *
*    ICbuf.h - User Structure Defines for the International             *
*              CorrectSpell(TM) Software                                *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
    Description:
    This header contains the typdef for the user buffer for the
    International CorrectSpell(TM) Software. This header should be 
    included by all modules of ICS and by the User Application.

    Members with the comment 'IN' should be set by the User Application
    before calling ICspl.  Members with the comment 'OUT' should be interpreted
    by the User Application after calling ICspl.  Members with 'IN/OUT' may be
    set either by the User Application or by ICspl.


    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICBUF_H
#define ICBUF_H

#ifndef ICCTLBUF_H
typedef VOID FAR * PICCTLBUFF;
#endif

typedef struct _ICBUFF
{
                       /* Tasks to perform */ 
  int           task;              /* INPUT parameter                         */
                                   /* Initialize; Terminate; Verify; Wildcard; 
                                    Anagram; Hyphenate; Smudge Verification;
                                    Correct                                   */


         /* Value returned by last function call to ICspl */
  int           retcode;          /* OUT;Return code                          */


                    /* Language Specifications */
  unsigned char lang;             /* IN;Language Code defined in sllang.h     */
  int           avail_dialect;    /* OUT;Dialects supported by this database
                                         defined in sllang.h                  */
  int           avail_mode;       /* OUT;Modes for this database supported    */
                                  /*     by this version of the code. defined */
                                  /*     in sllang.h                          */

  int           lang_dialect;     /* IN;Language Dialect def'd in sllang.h    */
  int           lang_mode;        /* IN;Language Mode    def'd in sllang.h    */


               /* Spelling Database file information */
  SLPATH        mstr_path;                /* IN; Path or Volume ref. number   */
  char          mstr_fn [ICFNAMEMAX + 1]; /* IN; Spell Database file name     */


             /* Hyphenation Database file information */
  SLPATH        hyp_path;              /* IN; Path or Volume ref. number   */
  char          hyp_fn[ICFNAMEMAX+1];  /* IN; Hyphen Database file name    */
  int           hyp_flag;              /* IN/OUT;Request hyphenation       */


         /* Additional Language Processors and Data Files */
   PFICLNGBUF   iclngbuf;                 /* IN; Structure to be allocated and
                                                 filled by OEM                */ 
   

   /* The values of the following flags are described in the file IC.H */
  short         init_flags;       /* OUT;Initialization flags                 */
  short         process_flags;    /* OUT;See ic.h for value descriptions      */
  long          error_flags;      /* OUT;See ic.h for value descriptions      */
  short         dial_flags;       /* OUT;Dialect Information flags            */


           /* Run time selectable soft hyphen character */
  unsigned char soft_hyp_char;    /* IN;Input soft hyphen character           */


                     /* Prior Word Information */
  unsigned char prev_word[ICMAX + 1]; /* IN/OUT;Previous word                 */

                     /* Prior Word Error Flag */
  short         nPrevWrdErr;      /* OUT;See ic.h for value descriptions      */
                                  /* set only if error_flags & IC_PREV_WRD_ERR*/

                        /* Parsing Information */
  int           parse_flag;       /* IN    ;Enable Parser                     */
  unsigned char per_flag;         /* IN/OUT;Terminal period stripped          */
  unsigned char trail_hyp;        /* IN/OUT;Trailing hyphen flag              */
  unsigned char lead_hyp;         /* IN/OUT;Leading  hyphen flag              */
  unsigned char trail_apo;        /* IN/OUT;Trailing apostrophe flag          */
  unsigned char lead_apo;         /* IN/OUT;Leading  apostrophe flag          */

  unsigned char prestrip  [ICSTRIPMAX+1]; /*OUT; Punctuation removed from the 
                                                 left side of word.           */

  unsigned char poststrip [ICSTRIPMAX+1]; /*OUT; Punctuation removed from the
                                                 right side of word.          */

                    /* Current Word Information */
  unsigned char word[ICMAX + 1];     /* OUT;Current word                      */
  short         len;                 /* OUT;Length of the current word        */
  unsigned char preclt [SFPREMAX+1]; /* OUT;Preclitic                         */
  unsigned char postclt[SFPOSTMAX+1];/* OUT;Postclitic                        */
  SF_MAP        hypmap;              /* OUT;Database mand/hyphenation map     */
  SF_MAP        emmap;               /* OUT;Map of em-dashes in input word    */
  SF_MAP        elmap;               /* OUT;Map of ellipses in input word     */

  SF_MAP  smap;                /* OUT;Map of slashes in input word      */
  SF_MAP  hmap;                /* OUT;Map of hard hyphens - input word  */
  
                      /* Correction Controls */
  unsigned short all_steps;          /* IN; Apply all possible correction 
                                             steps                            */
  unsigned short ret_every_step;     /* IN; Correction returns after every 
                                             step                             */

                /* Additional controls over returns */
  unsigned short subset_anagrm;      /* IN; Minimum for shortest anagram      */


                /* Alternative and suggested words */
  short         num_alt;             /* OUT;The number of alternatives        */
  short         next_alt;            /* OUT;The next available character      */
  unsigned char list  [ICLISTSIZE];  /* OUT;Alternative/Correction list       */
  short         corptr[ICMAXALT];    /* OUT;Pointer to alternatives           */

              /* Additional verification alternatives */
  unsigned char alt_word[ICMAX + 1]; /* OUT;Alternate word                    */
  SF_MAP        altmap;              /* OUT;alternate hyphenation map         */

                /* Cache size used during verification */
  unsigned long int cache_siz;       /* IN; Cache table size                  */
 
#ifdef ICS_PDICTIONARY
  UINT2B   wDID;           /* OUT; ID num of PD's which provide */
  UINT2B   wPID;           /* IN; Personal Dictionary process ID    */
#endif  
 
  /*==========================================================================*/

   /* The following element is for internal use in ICspl */
  PICCTLBUFF   pctlbuff;     /* General control    - Not For OEM use   */

} ICBUFF, 
* PICBUFF,                       /* Buffer and pointer to buffer        */
FAR * PFICBUFF;                  /* Buffer and pointer to buffer        */

#endif /* ICBUF_H */

/* End of file ICbuf.h */
