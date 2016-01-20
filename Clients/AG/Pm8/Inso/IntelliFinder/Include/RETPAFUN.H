/****************************************************************************
 *                     Copyright 1995 INSO Corporation                      *
 *                    IntelliFinder(TM) Reference Engine                    *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************/

/*--------------------------------------------------------------------------*
 *                                                                          *
 * retpafun.h                                                               *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:


Creation Date:
   6-17-1992     LT

Change Record:
   Date      Author      Comment

*---------------------------------------------------------------------------*/

#ifndef RETPAFUN_H

#define RETPAFUN_H

RETCODE REetpar1 (PFRE_ETPAR_IO , RE_OPERATION, RE_OPERATION);
UCHAR FAR PASCAL  Parse2EntBuf (PFRE_ETPAR_IO, RE_OPERATION, PFRE_STRTAB_IO, PFUINT4B,ET_PARSEDATA FAR *);
UCHAR FAR PASCAL  Parse1EntBuf (PFRE_ETPAR_IO, UINT2B, PFRE_MNG_ELE FAR *, PFRE_STRTAB_IO, PFUINT4B, ET_PARSEDATA FAR *);
/* Used internally by REetpar1() and REetpar2()                         */

VOID    FAR PASCAL REErrClear   (PFRE_STATUS);
VOID    FAR PASCAL CheckStatus  (PFRE_STATUS);

#ifdef RE_SF_ETPAR 

RETCODE FAR PASCAL REtpatok     (PFUCHAR, PFUCHAR, PFUINT2B, PFSCHAR);

VOID    FAR PASCAL ChkChrMac    (PFET_TOKNODE, PFUINT2B, PFUCHAR, INT2B,
                                 UINT2B);

UCHAR   FAR PASCAL CrtEntLineTokens (PFUCHAR, PFET_TOKNODE FAR *,
                                     PFRE_STATUS);

VOID    FAR PASCAL CvtChrMac    (PFUCHAR, PFUCHAR, PFET_TOKNODE FAR *,
                                 UCHAR, INT2B, UINT2B);

VOID    FAR PASCAL CvtToUnaccented (PFUCHAR, PFUINT4B);

VOID    FAR PASCAL FreeEtParMem (INT2B, PFRE_SYN_HDR FAR *);

VOID    FAR PASCAL FreEntLineTokens (PFET_TOKNODE FAR *);

UCHAR   FAR PASCAL GetCurSynPtr (PFRE_MNG_ELE, PFET_SYNPTR FAR *,
                                 PFET_SYNPTR FAR *, PFRE_STATUS);

UCHAR   FAR PASCAL GetDbStr     (PFRE_DB_IO, UCHAR, UCHAR, PFUCHAR FAR *, PFRE_STRTAB_IO );

UCHAR   FAR PASCAL GetOneLine (PFUCHAR FAR *, PFUCHAR FAR *, PFUCHAR FAR *,
                               FLAG, PFRE_STATUS);

UCHAR   FAR PASCAL GetStrCode   (SCHAR);

UCHAR   FAR PASCAL InitEtParMem (PFRE_SYN_HDR FAR *, PFRE_DB_IO, PFRE_STRTAB_IO, 
                                 PFRE_STATUS);

VOID    FAR PASCAL InitMngElmt  (PFRE_MNG_ELE);

VOID    FAR PASCAL InitSynElmt  (PFRE_SYN_ELE);

UCHAR   FAR PASCAL IsContentMacro (SCHAR);

UCHAR   FAR PASCAL PrimStrPrs   (PFUCHAR FAR *, PFET_TOKNODE FAR *, PFUCHAR, 
                                 PFRE_STATUS, INT2B, UINT2B, PFUINT4B);

UCHAR   FAR PASCAL SetpSynArray (PFRE_MNG_ELE, PFET_SYNPTR FAR *,
                                 PFRE_STATUS);

VOID    FAR PASCAL SkipMacStr   (PFET_TOKNODE FAR *);

UCHAR   FAR PASCAL SynonymPrs   (PFET_TOKNODE, PFUCHAR, PFRE_MNG_ELE,
                                 PFET_SYNPTR FAR *, PFET_SYNPTR FAR *,
                                 PFRE_ETPAR_IO, PFRE_STRTAB_IO, PFUINT4B);

#endif /* RE_SF_ETPAR */ 

#ifdef RE_SF_PDICTIONARY
FLAG PASCAL  PDMreviseSynData     (PFRE_ETPAR_IO);
RETCODE FAR PASCAL REPDWalkSyntax (PFDB_CTRL_RE, PFPD_CTRL_RE, PFVPD_CTRL_RE, PFUCHAR, PFRE_ETPAR_IO,
                                   INT4B, INT2B, INT2B, FLAG, RE_OPERATION, PFRE_STATUS, PFUINT4B);
RETCODE FAR PASCAL PDMaddSyn      (PFRE_STATUS, PFDB_CTRL_RE, PFPD_CTRL_RE, PFVPD_CTRL_RE, PFUCHAR,
                                   INT4B, INT2B);
RETCODE FAR PASCAL PDMdelSyn      (PFRE_STATUS, PFDB_CTRL_RE, PFPD_CTRL_RE, PFVPD_CTRL_RE, PFUCHAR,
                                   INT4B, INT2B);
RETCODE FAR PASCAL PDMaddGrp      (PFRE_STATUS, PFDB_CTRL_RE, PFPD_CTRL_RE, PFVPD_CTRL_RE, PFUCHAR,
                                   INT4B, INT2B, FLAG);
RETCODE FAR PASCAL PDMdelGrp      (PFRE_STATUS, PFDB_CTRL_RE, PFPD_CTRL_RE, PFVPD_CTRL_RE, PFUCHAR,
                                   INT4B, INT2B, FLAG);
FLAG FAR PASCAL    PDMReviseSyn   (PFRE_STATUS, PFRE_ETPAR_IO, FLAG, PFUCHAR, PFUCHAR, PFUCHAR, 
                                   UINT2B);
FLAG FAR PASCAL    PDMReviseMng   (PFRE_STATUS, PFRE_ETPAR_IO, PFUCHAR, PFUCHAR, UINT2B);
#endif

/* These are temporary tables
*/
extern PUCHAR ContMacTab[ET_CONMAC_MAX][3];
extern PUCHAR LangTab[];

/***************************************************************************/

#endif /* RETPAFUN_H */

/* End of file - REtpafun.h
*/
