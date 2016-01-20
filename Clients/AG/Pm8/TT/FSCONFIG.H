/*
$Header: /PM8/TT/FSCONFIG.H 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/FSCONFIG.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:14:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:16   Fred
// Initial revision.
// 
//    Rev 1.2   01 Jul 1997 09:08:26   Fred
// More changes for PMG 4.0
// 
//    Rev 1.1   18 Apr 1997 12:50:00   Fred
// Changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:26   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:15:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:41:48   FRED
// Initial revision.
// 
//    Rev 1.0   29 Sep 1995 13:37:08   FRED
// Initial revision.
 * 
 *    Rev 1.46   18 Jan 1993 08:32:22   JAY
 * Reenabled od_mark define bacause it now works.
 * 
 *    Rev 1.45   15 Jan 1993 17:11:58   JAY
 * Turned off od_mark define temporarily cause it doesn't work!
 * 
 *    Rev 1.44   15 Jan 1993 15:22:02   JAY
 * Added assembly version of od_mark() and define for it.
 * 
 *    Rev 1.43   15 Jan 1993 11:20:04   MACDUFF
 * Define MLS_ASM_SC_ISORT for TTDOS.
 * 
 *    Rev 1.42   14 Jan 1993 16:58:04   JAY
 * Assembly versions/enhancements and defines for FracSqrt and FracDiv.
 * 
 *    Rev 1.41   14 Jan 1993 15:05:06   JAY
 * Added define for MLS_ASM_FNT_IUP which turns on assembly form of fnt_IUP.
 * 
 *    Rev 1.40   13 Jan 1993 15:26:56   JAY
 * Added MLS_ASM_SC_MARK enabling new assemblified sc_mark() routine.
 * 
 *    Rev 1.39   13 Jan 1993 09:08:40   JAY
 * Added a macro for defining labels in asm blocks.
 * 
 *    Rev 1.38   11 Jan 1993 09:22:00   MACDUFF
 * FSCFG_NO_TRACE, MLS_ASM_FNT_InnerExecute, MLS_ASM_FNT_MIRP
 * 
 *    Rev 1.37   08 Jan 1993 09:28:46   MACDUFF
 * Three more MLS_ASM definitions
 * 
 *    Rev 1.36   14 Oct 1992 10:05:30   MACDUFF
 * ASM definitions for fnt_XMovePoint, fnt_YMovePoint, fnt_SkipPushCrap
 * 
 *    Rev 1.35   13 Oct 1992 09:17:52   MACDUFF
 * Define MLS_ASM_SC_wnNrowFill.  This controls assembler version of
 * sc_wnNrowFill and also causes patterns to be fetched 16 bits at a time
 * rather than 32 bits at a time, thus affecting fill_pattern_run and
 * the functions in PATTERNS.ASM
 * 
 *    Rev 1.34   07 Oct 1992 10:51:10   MACDUFF
 * Define MLS_ASM_SFNT_UnfoldCurve
 * 
 *    Rev 1.33   06 Oct 1992 12:09:32   MACDUFF
 * Defines FNT_ASM_BinaryOperand if TURBOPASCAL is zero.
 * 
 *    Rev 1.32   15 Sep 1992 15:58:52   ADAM
 * Declared these assembler externals:
 * NEAR_LXMUL,
 * NEAR_LDIV,
 * NEAR_LXLSH,
 * NEAR_LXRSH,
 * NEAR_LUDIV,
 * NEAR_LXURSH,
 * NEAR_SCOPY
 * 
 *    Rev 1.31   01 Sep 1992 10:58:44   FRED
 * Reenabled MLS_ASM_FONTMATH_CompDiv.
 * 
 * 
 *    Rev 1.30   01 Sep 1992 10:11:54   FRED
 * Disabled MLS_ASM_FONTMATH_CompDiv for a while.
 * 
 * 
 *    Rev 1.29   01 Sep 1992 02:51:30   FRED
 * Added MLS_ASM_FONTMATH_CompDiv
 * 
 * 
 *    Rev 1.28   01 Sep 1992 01:08:54   FRED
 * Added MLS_ASM_... #define's
 * 
 * 
 *    Rev 1.27   31 Aug 1992 22:44:28   FRED
 * Added MLS_ASM_FONTMATH_ShortFracDot and
 * MLS_ASM_FONTMATH_ShortFracDiv.
 * 
 * 
 *    Rev 1.26   31 Aug 1992 18:41:52   FRED
 * Added MLS_ASM_FONTMATH_CompMul.
 * 
 * 
 *    Rev 1.25   31 Aug 1992 12:01:46   FRED
 * Added #define MLS_ASM_SC_sc_DrawParabola.
 * 
 * 
 *    Rev 1.24   30 Aug 1992 22:23:20   MACDUFF
 * Defines for FNT.C: ALIGNPTS, RC, WC
 * 
 *    Rev 1.23   30 Aug 1992 16:35:40   MACDUFF
 * MLS_ASM_FNT_ALIGNRP
 * 
 *    Rev 1.22   29 Aug 1992 22:27:04   MACDUFF
 * Define for FNT.C PUSH functions in assembler.
 * 
 *    Rev 1.21   29 Aug 1992 21:20:38   MACDUFF
 * Defines for five assembly routines in FNT.C
 * 
 *    Rev 1.20   29 Aug 1992 11:26:58   FRED
 * Enabled MLS_ASM_SC_fill_pattern_run.
 * 
 * 
 *    Rev 1.19   29 Aug 1992 10:57:38   FRED
 * Added MLS_ASM_... #defines.
 * 
 * 
 *    Rev 1.18   28 Aug 1992 13:01:12   FRED
 * Defines FIXROUND() as a call to a function which is
 * fixed up by ASMPP.
 * 
 * 
 *    Rev 1.17   25 Aug 1992 10:54:48   FRED
 * Added MLS_ASM definition. If you add assmble code use:
 * 
 * #ifndef MLS_ASM
 *    C Code
 * #else
 *    Assemby Code
 * #endif
 * 
 * 
 *    Rev 1.16   23 Aug 1992 15:06:38   ADAM
 * Breathing stuff.
 * 
 *    Rev 1.15   22 Aug 1992 14:21:26   FRED
 * Changed IFDEF SMALL_TURBO_PASCAL to IF.
 * 
 * 
 *    Rev 1.14   21 Aug 1992 16:50:12   ADAM
 * Made some code for FSCALER.C. This code is for the overlay manager in
 * TurboPascal 6.0. It allows access to CS offsets.
 * 
 *    Rev 1.13   19 Aug 1992 15:47:26   MACDUFF
 * Defines MLS_PRECISION, except in FNT.C.  This turns variable-precision
 * math on.
 * 
 *    Rev 1.12   18 Aug 1992 18:50:44   MACDUFF
 * Support for global _Precision
 * 
 *    Rev 1.11   18 Aug 1992 09:57:50   MACDUFF
 * Defines _PRECISION
 * 
 *    Rev 1.10   11 Aug 1992 13:36:36   FRED
 * Removed "test" RV definition.
 * 
 * 
 *    Rev 1.9   10 Aug 1992 17:42:40   ADAM
 * Removed ReadTable. Now that the client has access to the offset & length
 * table in the key, it seems unecessary.
 * 
 *    Rev 1.8   04 Aug 1992 12:54:58   ADAM
 * New font scaler function: fs_ReadTable.
 * 
 *    Rev 1.7   29 Jul 1992 12:01:44   ADAM
 * Global reference to spline key.
 * RV test macro.
 * 
 *    Rev 1.6   17 Jul 1992 11:07:16   ADAM
 * New macros for the swap functions.
 * 
 *    Rev 1.5   13 Jul 1992 18:37:44   ADAM
 * assembler include of FSCONTXT.DEF. This allows structure inspecting
 * in TurboDebugger and allows the assembler post processor to use
 * some canned values.
 * 
 *    Rev 1.4   10 Jul 1992 14:37:20   ADAM
 * 
 *    Rev 1.3   10 Jul 1992 11:03:44   ADAM
 * 
 *    Rev 1.2   07 Jul 1992 19:42:12   ADAM
 * 
 *    Rev 1.1   07 Jul 1992 12:19:12   ADAM
 * No change.
 * 
 *    Rev 1.0   07 Jul 1992 12:19:00   ADAM
 * Initial revision.
*/

/*
 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³ FSCONFIG.H								³
 ³									³
 ³		     Small model TrueType configuration.		³
 ³									³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
 ³									³
 ³									³
 ³            Copyright (C) MicroLogic Software, Inc. 1988-92		³
 ³                           All Rights Reserved                        ³
 ³									³
 ³									³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/


/*
    File:       fsconfig.h : (Portable "Standard C" version)

    Written by: Lenox Brassell

    Contains:   #define directives for FontScaler build options

    Copyright:  c 1989-1990 by Microsoft Corp., all rights reserved.

    Change History (most recent first):
        <1>      8/27/91    LB      Created file.

    Usage:  This file is "#include"-ed as the first statement in
            "fscdefs.h".  This file contains platform-specific
            override definitions for the following #define-ed data
	    types and macros, which have default definitions in
	    "fscdefs.h":
                ArrayIndex
                F26Dot6
                FAR
                FS_MAC_PASCAL
                FS_MAC_TRAP
                FS_PC_PASCAL
                LoopCount
                MEMCPY
                MEMSET
                NEAR 
                SHORTDIV
                SHORTMUL
                SWAPL
                SWAPW
                SWAPWINC
                TMP_CONV
                boolean

            This file gives the integrator a place to override the
            default definitions of these items, as well as a place
            to define other configuration-specific macros.
*/


// Types for PrintMaster TrueType Engine:
//
//		Type						16-bit								32-bit
//		---------------------------------------------------------------------------
// 	Fixed						long (16.16)						long (16.16)
// 	Fract						long (2.30)							long (2.30)
//		F26Dot6					long (26.6)							long (26.6)
// 	ShortFrac	   		short (2.14)						short (2.14)
//		LoopCount				int									int
//		ArrayIndex				int									int
//		transMatrix				Fixed[3][3]							Fixed[3][3]
//		vectorType				Fixed[2]								Fixed[2]
//		BBOX						int32[4]								int32[4]
//		VECTORTYPE				ShortFrac							ShortFrac
//		VECTOR					VECTORTYPE[2]						VECTORTYPE[2]
//		FUnit						short									short
//		uFUnit               unsigned short						unsigned short

#ifndef __FSCONFIG_H__
#define __FSCONFIG_H__

#define PC_OS

#define RELEASE_MEM_FRAG

#ifdef PMW
//#define FSCFG_REENTRANT
#endif

#define F26Dot6		long
#define LoopCount		int
#define ArrayIndex	int

// MH 1/8/93 added this define so we can omit code used for the "trace
// function," which we don't use.

#define FSCFG_NO_TRACE

// Do not #define FSCFG_REENTRANT. The small model TT engine only requires
// LocalSC (SC.C) & LocalGS (FNT.C) to be moved into the context for full
// reentrant operation.

// For the following setjmp & longjmp def's to work, FNTJMP.H must be modified.

//#ifdef WIN32
//#include <setjmp.h>
//#define fs_setjmp(a)    setjmp(a)
//#define fs_longjmp(a,b) longjmp(a,b)
//#endif


// Allow access to the key from any module.

extern struct fsg_SplineKey* GlobalKey;

#ifdef WIN32
#define	FS_PC_PASCAL	__stdcall
#else
#define	FS_PC_PASCAL	pascal
#endif

#endif
