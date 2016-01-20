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
*    DClib.h - return codes and extern declarations                     *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
    Description: Main header file for the Database Customizer.

    Cautions:
    None.

    Creation Date: 02/16/93  STT

*-----------------------------------------------------------------------*/
#ifndef DCLIB_H
#define DCLIB_H

/* this structure to hold a list of constant strings and thier length */
   typedef struct _STRINGS
{
PUCHAR   string;
UCHAR   len;
}
STRINGS, *PSTRINGS;

#ifdef DC
#include "dc.h"
#endif

#ifdef DC
VOID     DCoutrul(UCHAR, PLINEINFO, PUCHAR);
VOID     DCoutdlt(UCHAR, PUCHAR, PUCHAR);
VOID     DCdefrul(UCHAR, PLINEINFO, PUCHAR);
VOID     DCdefdlt(UCHAR, PUCHAR);
FLAG     DCvarspl(PLINEINFO);
FLAG     DCallow (UCHAR, PLINEINFO, PUCHAR);
VOID     DCoutmor(PLINEINFO);
VOID     DCnullnk(PLINEINFO);
#endif /* SF_DC */

VOID     DCengan(PWFUCHAR, UCHAR, PFUINT4B, PWFUCHAR);

#ifdef SF_BASE_CLITIC
VOID     DCposclt(UCHAR, PWFUCHAR, UCHAR, PWFUCHAR);
VOID     DCposcls(UCHAR, PWFUCHAR, UCHAR, PFUINT4B, PWFUCHAR);
VOID     DCpreclt(PWFUCHAR, PFUINT4B, PWFUCHAR);
VOID     DCprecls(UCHAR, PWFUCHAR);
#endif

VOID     DCposacc(PWFUCHAR, UCHAR, PFUINT4B, PWFUCHAR);
VOID     DCcalacc(PWFUCHAR, UCHAR, PFUINT4B, PWFUCHAR);

#ifdef SF_COMPOUND_PROCESS
VOID     DCrmcc(UCHAR, UCHAR, PWFUCHAR);
VOID     DClnkcod(UCHAR, PUCHAR);
UCHAR    DCdblend(UCHAR, PWFUCHAR, PFUINT4B, UCHAR);
#endif

UCHAR    char_in(UCHAR, PWFUCHAR, UCHAR);
FLAG     cmp_end_char(PWFUCHAR, UCHAR, PWFUCHAR, UCHAR);
FLAG     cmp_strings(PWFUCHAR, UCHAR, PWFUCHAR WFAR *, UCHAR);

#endif /* DCLIB_H */

/* End of file - DClib.h */
