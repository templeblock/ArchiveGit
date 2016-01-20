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
*    ICsplext.h - return codes and extern declarations                  *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
    Description:
    This header contains all return codes and extern declarations
    for the spell module.  This header must be included to call
    any functions in this module or get access to its global data.

    Cautions:
    None.

*-----------------------------------------------------------------------*/
#ifndef ICSPLEXT_H
#define ICSPLEXT_H

RETCODE ICadjpost ( PWFUCHAR poststrip);
RETCODE ICreadjpo ( PWFUCHAR poststrip);
RETCODE ICcapcod  ( PFUCHAR, PICBUFF );
RETCODE ICcchadd  ( PWFUCHAR, INT2B, UCHAR, INT2B, FLAG, PICBUFF );
RETCODE ICcchver  ( PWFUCHAR, INT2B, PFFLAG, PICBUFF );
RETCODE ICdblver  ( PWFUCHAR, PICBUFF );
RETCODE ICfndchk  ( PICBUFF );
RETCODE ICget     ( PWFUCHAR, INT2B, PICBUFF );
RETCODE ICint     ( PICBUFF );
RETCODE ICintget  ( SWORD, PUCHAR, PICBUFF );
RETCODE ICpre     ( PWFUCHAR, PICBUFF );
UINT4B  ICpar     (PFUCHAR, PFUCHAR, PICBUFF);
VOID    ICchkprepos(PICBUFF); 
RETCODE ICprever  ( PWFUCHAR, PICBUFF);
RETCODE ICremacc  ( PWFUCHAR, INT2B, PICBUFF );
RETCODE ICtrm     ( PICBUFF );
RETCODE ICverify  ( PWFUCHAR, INT2B, PICBUFF );
UCHAR   ICcomcap  ( UCHAR, PFSF_MAP, INT2B, UCHAR, PFSF_MAP, PFSF_MAP, INT2B);

#ifdef ICS_CATALAN
VOID period_to_puntvolat(PICBUFF, PFUCHAR);
VOID puntvolat_to_period_list(PFICBUFF);
#endif

#ifdef ICS_RUSSIAN
RETCODE ICfoldio  ( PWFUCHAR, INT2B, PICBUFF );
#endif

#ifdef ICS_GREEK
RETCODE ICgreek  ( PICBUFF, FLAG );
#endif

#if defined ICS_HUNGARIAN || defined ICS_POLISH
#if defined(WINDOWS) || defined(WIN32) || defined(OS2)   
RETCODE ICHE_dll ( PFICBUFF, PFUCHAR);
#else
RETCODE SLAPI_PASCAL IChu ( PFICBUFF, PFUCHAR);
#endif                     
#endif

#ifdef   ICS_FINNISH_MORPHO
RETCODE ICfinver(PFRET_BUF, PICBUFF, PFSF_DVCTRL, PFUCHAR);
#endif

#ifdef ICS_PREV_WORD_CHECK
RETCODE ICdblchk    ( PICBUFF );
UCHAR   ICEndToken  ( PICBUFF );
#endif

#ifdef ICS_CLITIC_PROCESS
RETCODE ICclt          ( PWFUCHAR, PICBUFF );
RETCODE ICcltacc       ( PWFUCHAR, PICSPFLAGS, PWFUCHAR, PICBUFF );
RETCODE ICcltcap       ( PICBUFF );
RETCODE ICcltstm       ( PWFUCHAR, UINT2B, PICSPFLAGS, PWFUCHAR, PICBUFF );
RETCODE ICcltrp        ( PWFUCHAR, PWFUCHAR, PICBUFF );
RETCODE ICcltuna       ( PWFUCHAR, INT2B, PICBUFF );
#endif

#ifdef ICS_COMPOUND_PROCESS
RETCODE ICcmp       ( PICBUFF );
RETCODE ICcmpalt    ( INT2B, PWFUCHAR, INT2B, FLAG, PICBUFF );
#ifdef SF_IPR
RETCODE ICcmpdbl    ( PICBUFF, INT2B, INT2B, FLAG );
RETCODE ICcmpexc    ( PICBUFF, INT2B, INT2B, PWFINT2B, PWFINT2B, FLAG );
#else
RETCODE ICcmpdbl    ( PICBUFF, INT2B, INT2B );
RETCODE ICcmpexc    ( PICBUFF, INT2B, INT2B, PWFINT2B, PWFINT2B );
#endif
RETCODE ICcmpfnd    ( PICBUFF );
RETCODE ICcmphyp    ( PWFUCHAR, PICBUFF );
RETCODE ICcmphhy    ( PICBUFF );
INT2B   ICcmplmc    ( PICBUFF );
#ifdef ICS_MAND_HYPHEN_CHECK
RETCODE ICcmpmnd    ( PICBUFF );
#endif
RETCODE ICcmpnum    ( PWFUCHAR, UCHAR, PICBUFF );
RETCODE ICcmprmc    ( PICBUFF, INT2B, INT2B );
RETCODE ICcmpsft    ( PWFUCHAR, INT2B, PICBUFF );
RETCODE ICcmpsmh    ( PWFUCHAR, PWFUCHAR, PFUINT4B, INT2B, INT2B, PICBUFF );
RETCODE ICcmpspc    ( PWFUCHAR, UCHAR, PICBUFF );
RETCODE ICcmpsup    ( PWFUINT4B, INT2B, INT2B );
RETCODE ICcmpver    ( PICBUFF );
RETCODE ICcmpvow    ( PICBUFF, INT2B, FLAG, FLAG );
RETCODE ICcmpprm    ( PICBUFF, INT2B, INT2B, INT2B);
#endif

#ifdef ICS_CORRECTION
RETCODE ICcorspl    ( PFUCHAR, PFSF_DVCTRL, PICBUFF);
#endif

#ifdef ICS_UCF_CORRECT
RETCODE ICcorucf (PWFUCHAR, PICBUFF);
#endif

#ifdef ICS_BACK_COMPATIBLE
#ifdef ICS_RAM_VERIFICATION
RETCODE ICramchk    ( PWFUCHAR, INT2B, PICBUFF );
RETCODE ICramver    ( PWFUCHAR, INT2B, PICBUFF );
#endif
#endif

#ifdef ICS_IPDICTIONARY
RETCODE ICusr       ( PWFUCHAR, INT2B, PICBUFF, UCHAR, PWFINT4B, PWFUCHAR, PWFINT4B );

#ifdef ICS_CORRECTION
RETCODE ICusrcor    ( PICBUFF );
RETCODE ICusrchk    ( PWFUCHAR, INT2B, PICBUFF );
INT2B   ICusrdel    ( PWFUCHAR, INT2B, PICBUFF );
INT2B   ICusrir     ( PWFUCHAR, INT2B, PICBUFF, FLAG );
INT2B   ICusrtrn    ( PWFUCHAR, INT2B, PICBUFF );
#endif

RETCODE ICusrver    ( PWFUCHAR, INT2B, PICBUFF );
#endif

#ifdef MAC_HANDLES
RETCODE IChndlck (PICBUFF, FLAG);
#endif

#endif /* ICSPLEXT_H */

/* End of file ICsplext.h */
