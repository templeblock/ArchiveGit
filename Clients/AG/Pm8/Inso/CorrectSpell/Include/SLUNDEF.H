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
*    SLUNDEF.H - ifdefs & undefs to prepare SL.H for post-attribute     *
*                section definitions.                                   *
*                                                                       *
*-----------------------------------------------------------------------*


    Description:
        slundef.h undefines (if defined) all token strings that we take
        control of.  It also resolves minor discrepancies between 
        compilers of the same platform (e.g, Borland and Microsoft Windows
        compilers) by either defining or undefining.  It must be included in 
        SL.H *after* a platform has included all of its proprietary header 
        files, which occurs in the attributes section.

        The following token strings might have been defined in a platform 
        but we have our own uses for them.  We could simply define them 
        again without undefining them.  C will allow stacking of 
        definitions but will in some environments generate compiler 
        warnings.  To avoid this, all known conflicts are filtered here.

        Isolating #undefs in this file makes SL.H cleaner.

    Cautions:
        This file should be updated if new conflicts arise, or if a
        define moves from pre-attribute section to post-, or vice versa.

    Creation Date:
        08-aug-91   JCC

    Change Record:
        08-nov-92   JCC     Borland support
        23-mar-93   JCC     UNIX ANSI C support
        15-apr-93   JCC     Move MPC/THINK_C conflicts here from SLATTMAC.H

*-----------------------------------------------------------------------*/

#ifndef SLUNDEF_H

#define SLUNDEF_H

/* Following defines are those we define ourselves - remove any compiler's
 * definition.
*/
#ifdef NULL
#undef NULL
#endif

#ifdef PASCAL
#undef PASCAL
#endif

#ifdef _CDECL
#undef _CDECL
#endif

#ifdef FAR
#undef FAR
#endif

#ifdef NEAR
#undef NEAR
#endif

/*                          Compiler Discrepancies
 * The following defines resolve discrepancies between compilers of the
 * same platform, versions of compilers, or discrepancies between memory 
 * models.
*/

/* Microsoft compiler
*/
#ifdef MSC_VER
#if ( MSC_VER < 700 )
#undef SLA_UUCPASC_FC   /* MSC 6 & earlier only have 1 underscore           */
#undef SLA_DOSFILE_LF   /* MSC 6 & earlier don't have _dos file functions   */
#endif
#endif

/* Borland compiler
*/
#if defined ( __BORLANDC__ )

/* Borland can't create a medium memory model DLL for Windows.  Don't let
 * such code compile.
*/
#ifdef __DLL__
#ifdef __MEDIUM__
You may not build a medium model DLL for Windows using Borland C 3.1
#endif
#endif

/* SLATTWIN.H doesn't define this, but Borland needs it, so define if this
 * is Borland Windows.  This will place _loadds along with _export on 
 * functions that are labeled EXPORT (defined in SL.H).  Borland needs it 
 * because there's no compiler switch in Borland that equates to the _loadds 
 * keyword.  In Microsoft, we use the /GD switch, which will put the 
 * equivalent of __loadds code at each function that has __export.
*/
#ifdef WINDOWS
#define SLA_LOADDS_FD
#endif

/* Borland compiles in environments that by default have a near malloc
 * (_nmalloc) and therefore define SLA_NMALLOC_LF in their attribute files, 
 * but Borland doesn't have a near malloc, so undefine it.
*/
#undef SLA_NMALLOC_LF

/* Borland compiles in environments that by default don't prefix their
 * far alloc functions with 'far' (rather they prefix with _f) and therefore 
 * undef SLA_FARALLOC_LF in their attribute files, but Borland does prefix
 * with far, so define it.
*/
#define	SLA_FARALLOC_LF

/* Large memory model is flat, not segmented.
*/
#ifdef __LARGE__
#undef SLA_SEGMENT_AM       /* No near/far distinction */
#undef SLA_LOCALALLOC_LF    /* No LocalAlloc or nmalloc/nfree */
#endif

#endif /* __BORLANDC__ */

/* We support 2 UNIX compilers, one with ANSI prototyping and one without.
 * The ANSI one defines __STDC__, the non-ANSI one doesn't.  Turn off 
 * prototyping (it's on by default) if this isn't the ANSI compiler.
*/
#ifdef UNIX

#ifndef __STDC__
#undef SLA_PROTO_FD
#endif

#endif /* UNIX */

/* We support 3 Mac compilers. ThinkC, MPW, and Metrowerks.
   ThinkC is the default environment in SLATTMAC.H.  Because MPW and 
   Metrowerks use different file I/O routines, the TXTFILE_FO
   attribute needs to be turned off for these compilers.  Do so
   here.
*/
#ifdef MAC

#if defined(applec) || defined(__MWERKS__) 

#undef  SLA_TXTFILE_FO  /* Distinguishes bet. text & other files?   */

#endif /* applec || __MWERKS__*/

#endif /* MAC */

#ifdef __IBMC__
#undef  SLA_CPASC_FC
#undef  SLA_EXPORT_FD
#endif

#endif /* SLUNDEF_H */

/* End of file - slundef.h */
