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
*    ICbufext.h - External Finnish Structure for the International      *
*              CorrectSpell(TM) Software                                *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
    Description:
    This Structue is used ONY when and where the External Finnish
    Morphological Verification is to be used in conjuction with ICS

    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICBUFEXT_H
#define ICBUFEXT_H

#ifdef ICS_FINNISH_MORPHO

#if defined (WINDOWS) || defined (WIN32)
#define   ICFINDLLFUN "Morfo" /* function export name from Finnish DLL */
#endif

typedef struct _ICFINBUF
{
                /* Tasks to perform */ 
  int           task;            /* Initialize; Terminate; Verify;  */
  int           retcode;         /* OUT;Return code                 */

                /* Spelling Database file information */
  SLPATH        mstr_path;                /* IN; Path or Volume ref. number*/
  char          mstr_fn[ICFNAMEMAX + 1];  /* IN; Spell Database file name  */

                /* May be requiring File handle and Offset Instead later */
  
  SL_FHANDLE    ics_database;
  long          ext_file_offset;

   /* The values of the following flags are described in the file IC.H */
  long          error_flags;      /* OUT;See ic.h for value descriptions   */

  long          minor_error_flags; /* error returns from outside           */

                /* Current Word Information */
  unsigned char word[ICMAX + 1];     /* OUT;Current word                   */
  short         len;                 /* OUT;Length of the current word     */
  SF_MAP        hypmap;              /* OUT;Database mand/hyphenation map  */

                /* Alternative and suggested words */
  short         num_alt;             /* OUT;The number of alternatives     */
  unsigned char list  [ICLISTSIZE];  /* OUT;Alternative/Correction list    */
  unsigned short int cache_siz;      /* Cache table size                   */

} ICFINBUF, 
*  PICFINBUF,                       /* Buffer and pointer to buffer        */
FAR * PFICFINBUF;                   /* Buffer and pointer to buffer        */

#endif

#endif /* ICBUFEXT_H */

/* End of file ICbufext.h */
