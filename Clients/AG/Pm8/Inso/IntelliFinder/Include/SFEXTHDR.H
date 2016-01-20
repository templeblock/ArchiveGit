/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*

*-----------------------------------------------------------------------*
*                                                                       *
* SFexthdr.h -                                                          *
*                                                                       *
*-----------------------------------------------------------------------*/

#ifndef SFEXTHDR_H

#define SFEXTHDR_H


#ifdef SF_BASE_CLITIC

INT2B SFcltstp(PFUCHAR,PFUCHAR,PFFLAG,PFSF_DCTRL,PFSF_DVCTRL);
RETCODE SFcltchk(PFUCHAR,INT2B,INT2B,PFUINT2B,PFUINT2B,UINT2B,PFUCHAR);

#ifdef SF_CLITIC_PROCESS
RETCODE  SFcltcmp   (PFSF_DCTRL, PFSF_DVCTRL);
FLAG     SFcltdb    (PFSF_DCTRL, PFSF_DVCTRL);
RETCODE  SFcltscr   (PFUCHAR, INT2B, PFSF_DCTRL, PFSF_CLITIC);
#endif

#endif  /* SF_BASE_CLITIC */

#ifdef SF_WILDCARD
RETCODE SFwild      (PFUCHAR, INT2B, PFSF_DCTRL, PFSF_DVCTRL);
#endif

#ifdef SF_CORRECTION
RETCODE SFcorqbr    (PFSF_DCTRL, PFSF_DVCTRL, PFUCHAR, INT2B, PFUCHAR, INT2B, INT2B, 
                     INT2B, INT2B, INT2B );
RETCODE SFcorrec    (PFUCHAR, INT2B, PFSF_DCTRL, PFSF_DVCTRL);
RETCODE SFcor1qd    (PFUCHAR, PFSF_DCTRL, PFSF_DVCTRL);
RETCODE SFcor2qd    (PFUCHAR, PFSF_DCTRL, PFSF_DVCTRL);
RETCODE SFcor3qd    (PFUCHAR, PFSF_DCTRL, PFSF_DVCTRL);
RETCODE SFcor6qd    (PFUCHAR, PFSF_DCTRL, PFSF_DVCTRL);
RETCODE SFcor8qd    (PFUCHAR, PFSF_DCTRL, PFSF_DVCTRL);
RETCODE SFcor9qd    (PFUCHAR, PFSF_DCTRL, PFSF_DVCTRL);
RETCODE SFcorrig    (PFUCHAR, PFSF_DCTRL, PFSF_DVCTRL);
RETCODE SFcorsrt    (PFUCHAR, INT2B, INT2B, PFSF_DCTRL, PFSF_DVCTRL);
INT2B SFpreglob	(PFUCHAR, INT2B, PFUCHAR);
INT2B SFprematch(PFUCHAR, PFSF_DCTRL);
#endif

#ifdef SF_ANAGRAM
RETCODE SFanagrm    (PFUCHAR, INT2B, PFSF_DVCTRL, PFSF_DCTRL);
RETCODE SFanaqua    (PFUCHAR, INT2B, PFSF_DVCTRL, PFSF_DCTRL);
#endif

#ifdef SF_ENT_LIST
VOID FAR PASCAL SFClrPOS (PFSF_DVCTRL);
#endif

PFSF_VER SFGetVersion(VOID);
PFSF_ATTRIB SFGetAttributes(VOID);

INT2B   SFaccent    (PFUCHAR, INT2B, PFSF_DCTRL, PFSF_DVCTRL);
INT2B   SFbisrch    (PFUCHAR, INT2B, INT2B, PFINT2B, PFSF_DCTRL);

#ifdef SF_CIRCLE_INDEX
RETCODE SFnbis      (INT4B, PFINT2B, PFINT2B, PFSF_DCTRL); 
#endif

RETCODE FAR PASCAL SFadd (PFUCHAR, INT2B, PFINT2B, PFINT2B, INT2B, INT2B, PFUCHAR, PFINT2B);
RETCODE FAR PASCAL SFadd1(PFUCHAR, INT2B, PFSF_DVCTRL, FLAG);
RETCODE FAR PASCAL SFlstini (PFINT2B, PFINT2B, PFUCHAR, PFINT2B);

RETCODE SFdecbit (UCHAR, UCHAR, INT2B, PFUCHAR, PFSF_DCTRL, PFSF_DVCTRL);

FLAG FAR PASCAL SFanachk (PFUCHAR, UINT2B, PFUCHAR);

RETCODE  SFchkwrd   (PFSF_DCTRL, PFSF_DVCTRL, PFUCHAR, INT2B, UCHAR, PFUINT4B);

FLAG FAR PASCAL SFcorbr8 (PFSF_DCTRL, PFSF_DVCTRL, PFUCHAR, INT2B, PFUCHAR, INT2B);
FLAG FAR PASCAL SFcorbru (PFSF_DCTRL, PFSF_DVCTRL, PFUCHAR, INT2B, PFUCHAR, INT2B, INT2B, INT2B, INT2B, INT2B, INT2B);

RETCODE FAR PASCAL SFhdrchk(PFUCHAR,UINT2B,UINT2B,UINT2B,UINT2B);

RETCODE SFremacc    (PFUCHAR, PFUINT4B, UCHAR);

RETCODE SFusr       (PFUCHAR, INT2B, PFSF_DCTRL, PFSF_DVCTRL, UCHAR, PFUINT4B, PFUCHAR, PFUINT4B);

RETCODE SFdecode    (PFUCHAR, INT2B, INT2B, INT2B, PFSF_DCTRL, PFSF_DVCTRL);

RETCODE SFclose     (SL_FHANDLE);

RETCODE SFlseek     (SL_FHANDLE, INT4B, INT2B);

RETCODE SFfarread   (SL_FHANDLE, PFCHAR, INT2B);

RETCODE SFread      (SL_FHANDLE, PCHAR, INT2B);

FLAG    SFmemory    (PSF_VARS FAR *, INT2B, UCHAR);

#ifdef SF_BASEFORM
VOID FAR PASCAL addBase (PFSF_DVCTRL, UCHAR);
#endif

#ifdef SF_CLOSEST_MATCH
VOID FAR PASCAL CMsimtst (PFUCHAR , PFUCHAR , PFINT2B , PFINT2B );
#endif

#ifdef SF_TABLE_OF_CONTENTS

FLAG FAR PASCAL SFtoclix (PFSF_DVCTRL, INT4B);

#endif

#ifdef SF_WILDPAGE
VOID FAR PASCAL storeBatch (PFSF_DVCTRL , INT2B , INT2B );
#endif

#ifdef SF_APPLY_DC_RULES
RETCODE SFdc(PFUCHAR, INT2B, PFSF_DCTRL, PFSF_DVCTRL);
#endif

#endif /* SFEXTHDR_H */

/* End of file - SFexthdr.h */
