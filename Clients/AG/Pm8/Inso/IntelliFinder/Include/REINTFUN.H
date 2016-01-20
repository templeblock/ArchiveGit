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
 * REintfun.h - Internal Reference Engine function prototypes and           *
 *              definitions.                                                *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
    RE internal forward function prototypes and declarations.

Creation Date:
    31-may-89   L Tang

Change Record:
    Date        Author  Comment
    14-jun-90   JCC     Removed prototype for EDformat(), added for 
                        EDbsrch() & other bool search functions
    01-may-91   JCC     EDsimila takes FAR pointers
    06-jun-91   ECH     Changed some pointer parameters to FAR to
                        avoid DS != SS problems (single instance DLL). ECH01
    30-Oct-91   ECH     All functions now PASCAL.  ECH02
    30-Oct-91   ECH     Added new functions with the Database Manager. ECH02
    21-feb-92   RAS     ED names -> RE
    25-Mar-92   ECH     NEW API, several old API functions now internal. ECH05
    22-jun-92   JCC     single file refworks, header reorganization
    06-Jul-92   ECH     SF spelling correction.
    29-jan-93   JCC     Centralized baseform processing, inflection
    03-Aug-93   ECH     Removed non-protoype defs, REverify & REget use FARs

*---------------------------------------------------------------------------*/

#ifndef REINTFUN_H

#define REINTFUN_H

/* Function prototypes and declarations for all functions
 * internal to the Reference Engine.
*/

#define SCHAUX_NEAR

INT2B   FAR PASCAL REOpenDatabase     (PFRE_DB_IO);
RETCODE FAR PASCAL RECloseDatabase    (PFRE_DB_IO);
RETCODE FAR PASCAL REWakeDatabase     (INT2B, INT2B, PFDB_CTRL_RE FAR *, PFVDB_CTRL_RE FAR *, PFRE_STATUS);
RETCODE FAR PASCAL RESleepDatabase    (INT2B, INT2B, PFRE_STATUS);
RETCODE FAR PASCAL RECloseDBApp       (PFRE_APP_IO);
INT2B   FAR PASCAL REOpenApplication  (PFRE_STATUS);
RETCODE FAR PASCAL RECloseApplication (INT2B);
RETCODE FAR PASCAL RECloseDBInt       (INT2B,PFRE_STATUS,PFRE_DB_IO);
RETCODE FAR PASCAL REWakeApplication  (INT2B, PFAPP_CTRL_RE FAR *, PFRE_STATUS);
#ifdef RE_SF_PDICTIONARY
INT2B   FAR PASCAL REPDadd            (PFREPD_DB_IO);
RETCODE FAR PASCAL REPDwake           (INT2B, INT2B, PFPD_CTRL_RE FAR *, PFVPD_CTRL_RE FAR *, PFRE_STATUS);
RETCODE FAR PASCAL REPDSleep          (INT2B, INT2B, PFRE_STATUS);                                    
RETCODE FAR PASCAL REPDdelete         (INT2B);
/* IF PD Manager Functions */  
RETCODE FAR PASCAL REPDMOpen      (PFREPD_DB_IO, PFPD_CTRL_RE, PFVPD_CTRL_RE, INT2B, FLAG, FLAG, FLAG, PFUINT2B);
RETCODE FAR PASCAL REPDMClose     (PFREPD_DB_IO, PFPD_CTRL_RE, PFVPD_CTRL_RE, FLAG);
RETCODE FAR PASCAL REPDMSave      (PFREPD_DB_IO, PFPD_CTRL_RE, PFVPD_CTRL_RE, FLAG);
RETCODE FAR PASCAL REPDMAdd       (PFREPD_EDIT_IO, PFPD_CTRL_RE, PFVPD_CTRL_RE, FLAG);
RETCODE FAR PASCAL REPDMModify    (PFREPD_EDIT_IO, PFPD_CTRL_RE, PFVPD_CTRL_RE, RE_OPERATION);
RETCODE FAR PASCAL REPDMVerify    (PFRE_STATUS, PFUCHAR, PFDB_CTRL_RE, PFVDB_CTRL_RE, PFSF_DCTRL,
                                   PFSF_DVCTRL, PFRE_ENTLIST_IO);
VOID FAR PASCAL    REPDFreeWordIO (PFPD_WORD_IO);
VOID FAR           REPDitoa       (UINT4B, PFUCHAR);
VOID FAR           REPDitoaHex    (UINT4B, PFUCHAR);
INT4B FAR PASCAL   SLFileLen      (SLPATH, PFUCHAR);
FLAG FAR PASCAL    REFileAppend   (PFUCHAR, PF_SLPATH, PFUCHAR, PF_SLPATH, INT4B, INT4B, INT4B, SL_FHANDLE);
FLAG FAR PASCAL    PDMfillHwd     (PFRE_STATUS, PFUCHAR, PFUCHAR, PFDB_CTRL_RE, PFVDB_CTRL_RE, INT4B);
INT4B FAR PASCAL        PDMmakeStoreNum  (INT4B);
PFPD_WORD_IO FAR PASCAL REPDAllocWordIO  (VOID);
#endif
RETCODE FAR PASCAL RELFSeek           (INT2B,INT2B,SL_FHANDLE,PFDB_CTRL_RE,INT4B,INT2B);
VOID    FAR PASCAL iniDecodeForRE     (PFDB_CTRL_RE,PFVDB_CTRL_RE);

VOID    FAR PASCAL REErrClear (PFRE_STATUS);
VOID    FAR PASCAL REErrSet   (PFRE_STATUS, RETCODE, INT2B, UINT2B);
VOID    FAR PASCAL REErrAPI   (PFRE_STATUS, RETCODE, INT2B, UINT2B);

RETCODE FAR PASCAL REintrdb (PFRE_FILENAMES, PFDB_CTRL_RE, PFRE_STATUS);
RETCODE FAR PASCAL REintvdb (PFDB_CTRL_RE, PFVDB_CTRL_RE, PFRE_DB_IO);
RETCODE FAR PASCAL REstprdb (PFDB_CTRL_RE);
RETCODE FAR PASCAL REstpvdb (PFVDB_CTRL_RE, PFRE_DB_IO);
RETCODE FAR PASCAL RErlock  (PFDB_CTRL_RE);
RETCODE FAR PASCAL REvlock  (PFVDB_CTRL_RE, PFRE_STATUS);
RETCODE FAR PASCAL RErunlok (PFDB_CTRL_RE);
RETCODE FAR PASCAL REvunlok (PFVDB_CTRL_RE, PFRE_STATUS);

RETCODE FAR PASCAL REgetini (INT2B, INT4B, UINT2B, PFUCHAR, UCHAR, UCHAR, PFRE_STATUS, INT2B);
RETCODE FAR PASCAL REgetrwd (INT2B, FLAG);
RETCODE FAR PASCAL REgetstp (INT2B, PFRE_STATUS, INT2B);

RETCODE FAR PASCAL REbrute  (PFDB_CTRL_RE, PFVDB_CTRL_RE, PUCHAR, INT2B, PUCHAR, INT2B, INT2B, INT2B,
                             INT2B, INT2B, INT2B);
RETCODE FAR PASCAL REcnvspc (PUCHAR, FLAG);
RETCODE FAR PASCAL REdbchek (UCHAR, PFDB_CTRL_RE *, PFVDB_CTRL_RE *);
RETCODE FAR PASCAL REdbunlk (PFDB_CTRL_RE, PFVDB_CTRL_RE, UCHAR );
RETCODE FAR PASCAL REdecode (PUCHAR, INT2B, INT2B, INT2B, INT2B, PFDB_CTRL_RE, PFVDB_CTRL_RE );
/* lrsv23 */
RETCODE FAR PASCAL REget    (PFUCHAR, INT2B, PFDB_CTRL_RE, PFVDB_CTRL_RE,
                              PFRE_ENTLIST_IO);
/* lrsv23 */
RETCODE FAR PASCAL getBaseST(PFDB_CTRL_RE,PFVDB_CTRL_RE);
RETCODE FAR PASCAL REhdrchk (PNUCHAR, SL_FHANDLE, UINT2B, PFDB_CTRL_RE, PFRE_STATUS);
RETCODE FAR PASCAL REsufchk (INT2B, PFINT2B, INT2B, INT2B, PFDB_CTRL_RE ); 
RETCODE FAR PASCAL REsufmat (PUCHAR, INT2B, PFINT2B, PFDB_CTRL_RE );
RETCODE FAR PASCAL REverify (PFDB_CTRL_RE, PFVDB_CTRL_RE, PFRE_ENTLIST_IO, PFUCHAR);
RETCODE FAR PASCAL REinflst( PFRE_IDXLIST_IO , INT2B );

RETCODE FAR PASCAL REtabini (VOID);
VOID    FAR stripMacros (PFUCHAR, PFUCHAR);
RETCODE FAR extractWord (PFUCHAR, PFUCHAR, INT2B, INT2B, INT2B, PFINT2B);

#ifdef RE_SF_ENTRY_VERIFICATION
INT2B   FAR PASCAL REdecomp (PF_GETINFO_RE, PCHAR, UINT2B, INT2B, PFDB_CTRL_RE, PFVDB_CTRL_RE);
RETCODE FAR PASCAL REgetdat (PFRE_ENTRY_IO, RE_OPERATION, RE_OPERATION);
RETCODE FAR PASCAL REhrtini (PFRE_ENTLIST_IO);
#endif
#ifdef RE_FN_ENTRY
RETCODE FAR PASCAL REhwdetn (PFDB_CTRL_RE, PFVDB_CTRL_RE, INT2B, PFSF_ENTLIST);
#endif

#ifdef RE_SF_CIRCLE_INDEX
RETCODE FAR PASCAL REcircent (PFDB_CTRL_RE, PFVDB_CTRL_RE, PFRE_ENTRY_IO, RE_OPERATION, RE_OPERATION); 
RETCODE FAR PASCAL REcirctok (PFDB_CTRL_RE, PFVDB_CTRL_RE, PFRE_TOKEN_IO, RE_OPERATION, RE_OPERATION); 
#endif

#ifdef RE_FN_ALTERN
RETCODE FAR PASCAL REaltern (PFRE_IDXLIST_IO, RE_OPERATION);
#endif /* RE_FN_ALTERN */

#ifdef RE_FN_ANAGRM
RETCODE FAR PASCAL REanagrm (PFRE_IDXLIST_IO);
#endif /* RE_FN_ANAGRM */

#ifdef RE_SF_CLOMCH
RETCODE FAR PASCAL REclomch (PFRE_ENTLIST_IO);
RETCODE FAR PASCAL REidxmch (PFRE_IDXLIST_IO);
#endif

#ifdef RE_FN_INFLECT
RETCODE FAR PASCAL REinflec(PFRE_IDXLIST_IO,FLAG);
#endif

#ifdef RE_SF_PRON
RETCODE FAR PASCAL REpron (PFRE_ENTRY_IO);
#endif

#ifdef RE_SF_TOKEN
RETCODE FAR PASCAL REtknget (PFRE_TOKEN_IO, RE_OPERATION);
RETCODE FAR PASCAL REtknstp (INT2B, PFRE_STATUS);
RETCODE FAR PASCAL REtknini (INT2B, INT4B, UINT2B, PFRE_STATUS, RE_OPERATION);
RETCODE FAR PASCAL REtknize (PN_TOKENINFO_RE, PFDB_CTRL_RE, PFVDB_CTRL_RE);
#endif

#ifdef THINK_C
RETCODE PASCAL REmcopen (Point, OSErr *, PFRE_FILENAMES);
#endif

#ifdef RE_SF_OTHER_DECODE
FLAG    FAR PASCAL REadd    (PUCHAR, INT2B, PFVDB_CTRL_RE);
RETCODE FAR PASCAL REbrute8 (PFDB_CTRL_RE, PFVDB_CTRL_RE, PUCHAR, INT2B, PUCHAR, INT2B);
RETCODE FAR PASCAL REisanag (PUCHAR, UINT2B, PFVDB_CTRL_RE);
#endif

#ifdef RE_SF_FLDSEL
RETCODE FAR PASCAL REfldsel (PF_FIELD_RE, UCHAR, PUCHAR, PFRE_STATUS);
VOID    FAR PASCAL REfldend (PF_FIELD_RE, PFRE_STATUS);
VOID    FAR PASCAL REfldini (PF_FIELD_RE, PFRE_STATUS);
RETCODE FAR PASCAL REfldall (UWORD,PFDB_CTRL_RE,PFVDB_CTRL_RE);
VOID    FAR PASCAL REfldrst (PFDB_CTRL_RE,PFVDB_CTRL_RE);
RETCODE FAR PASCAL REfldidx (PFDB_CTRL_RE,PFUCHAR,PFUINT2B, PFRE_STATUS);
RETCODE FAR PASCAL REselfld (INT2B, PFUCHAR, FLAG, PFRE_STATUS);
RETCODE FAR PASCAL REselsub (INT2B, PFUCHAR, FLAG, PFRE_STATUS);
#endif

#ifdef RE_SF_GETPRT
RETCODE FAR PASCAL REpartsp (INT2B, PFUCHAR, PFRE_STATUS);
#endif

#ifdef RE_SF_BOLSCH
RETCODE FAR PASCAL REbsrch  (PFDB_CTRL_RE, PFVDB_CTRL_RE, PFRE_SEARCH_IO);
RETCODE FAR PASCAL REbolsch (PFRE_SEARCH_IO, RE_OPERATION);

/* Boolean search auxillary functions. */
INT2B   FAR PASCAL parse       (PCHAR, INT2B, PUCHAR FAR *, PF_BOOLINFO_RE);
VOID    FAR PASCAL key         (PUCHAR FAR *,INT2B,PF_BOOLINFO_RE);
RETCODE FAR PASCAL getInterestingCodes (PFDB_CTRL_RE, PFVDB_CTRL_RE ,PCHAR, PF_BOOLINFO_RE);
INT2B   FAR PASCAL sblock_eval (PUCHAR FAR *,PF_BOOLINFO_RE);
INT2B   FAR        eval        (PUCHAR FAR *,UINT2B [],INT2B);
INT2B   FAR PASCAL macrocmp    (PUCHAR,PUCHAR,INT2B,INT2B,INT2B);
#endif /* RE_SF_BOLSCH */


#ifdef RE_FN_TOCHA
#ifdef RE_FN_ENTRY
RETCODE FAR  PASCAL REtoclst (PFRE_ENTLIST_IO, RE_OPERATION);
#endif
RETCODE FAR  PASCAL REtocini (PFDB_CTRL_RE, PFVDB_CTRL_RE, PFRE_DB_IO);
RETCODE FAR  PASCAL RETocBlkToTab (ORF_IBUFF, PFDB_CTRL_RE, INT2B);
RETCODE FAR  PASCAL REtocsel (PFDB_CTRL_RE,PFVDB_CTRL_RE,PFRE_DB_IO,PFRE_TOCFLT);
RETCODE FAR  PASCAL REtocstp (PFDB_CTRL_RE, PFVDB_CTRL_RE, PFRE_DB_IO, FLAG);

FLAG FAR PASCAL IFPDdeletedEntNo(PFVDB_CTRL_RE pREVCtrl, INT4B lEntNo);
FLAG FAR PASCAL REPDvalidEntNo(PFVDB_CTRL_RE pREVCtrl, INT4B lEntNo);
FLAG FAR PASCAL REtoclix (PFVDB_CTRL_RE, INT4B);
INT2B           IsNodeTerminal (PF_TOCNODE_RE, UINT2B, UINT2B);
PFUCHAR         CreateTermNodeBMap(PFDB_CTRL_RE, PFVDB_CTRL_RE, PFUCHAR,
                       UINT2B, PF_TOCNODE_RE, UINT2B);
#endif /* RE_FN_TOCHA */

#if defined(RE_FN_TOCHA) || defined (RE_SF_TOKEN)
RETCODE FAR  PASCAL REtoctop (PFDB_CTRL_RE, PFVDB_CTRL_RE, INT4B, PFINT2B, PFRE_STATUS);
#endif

#ifdef RE_FN_WILDCH
RETCODE FAR PASCAL REwildch  (PFRE_IDXLIST_IO, RE_OPERATION);
#endif


#ifdef RE_FN_ROM
VOID REromhdr ( PFDB_CTRL_RE , SL_FHANDLE );
#endif

RETCODE FAR PASCAL RECacheAllocObj(ORF_POSCACHE_RE FAR *, UINT2B, UINT2B);
VOID FAR * FAR PASCAL RECacheGet(PF_POSCACHE_RE, PFUINT2B);
VOID FAR * FAR PASCAL RECacheLast(PF_POSCACHE_RE);
UINT2B FAR PASCAL RECacheEntryToPos(PF_POSCACHE_RE, VOID FAR *);
INT2B FAR PASCAL RECacheAdd(PF_POSCACHE_RE, UINT2B, VOID FAR *);
VOID FAR PASCAL RECacheClear(PF_POSCACHE_RE);
VOID FAR RECacheFreeObj(ORF_POSCACHE_RE FAR *);

#ifndef RE_SF_1RDB
ORFDB_IENTRY_DM     FAR PASCAL createDBElement (VOID);
RETCODE             FAR PASCAL deleteDBElement (INT2B);
ORFDB_IENTRY_DM     FAR PASCAL getDBElement (INT2B, PFRE_FILENAMES);
ORFVDB_IENTRY_DM    FAR PASCAL createVDBElement (VOID);
RETCODE             FAR PASCAL deleteVDBElement (INT2B);
ORFVDB_IENTRY_DM    FAR PASCAL getVDBElement (INT2B);
#ifdef RE_SF_PDICTIONARY
ORFPD_IENTRY_DM     FAR PASCAL createPDElement (VOID);
RETCODE             FAR PASCAL deletePDElement (INT2B);
ORFPD_IENTRY_DM     FAR PASCAL getPDElement (INT2B, PFRE_FILENAMES);
ORFVPD_IENTRY_DM    FAR PASCAL createVPDElement (VOID);
RETCODE             FAR PASCAL deleteVPDElement (INT2B);
ORFVPD_IENTRY_DM    FAR PASCAL getVPDElement (INT2B);
#endif
#endif

#endif /* REINTFUN_H */
/* End of file - REintfun.h
*/
