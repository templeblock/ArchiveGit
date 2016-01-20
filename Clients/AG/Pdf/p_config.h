/*---------------------------------------------------------------------------*
 |              PDFlib - A library for generating PDF on the fly             |
 +---------------------------------------------------------------------------+
 | Copyright (c) 1997-2002 PDFlib GmbH and Thomas Merz. All rights reserved. |
 +---------------------------------------------------------------------------+
 |    This software is NOT in the public domain.  It can be used under two   |
 |    substantially different licensing terms:                               |
 |                                                                           |
 |    The commercial license is available for a fee, and allows you to       |
 |    - ship a commercial product based on PDFlib                            |
 |    - implement commercial Web services with PDFlib                        |
 |    - distribute (free or commercial) software when the source code is     |
 |      not made available                                                   |
 |    Details can be found in the file PDFlib-license.pdf.                   |
 |                                                                           |
 |    The "Aladdin Free Public License" doesn't require any license fee,     |
 |    and allows you to                                                      |
 |    - develop and distribute PDFlib-based software for which the complete  |
 |      source code is made available                                        |
 |    - redistribute PDFlib non-commercially under certain conditions        |
 |    - redistribute PDFlib on digital media for a fee if the complete       |
 |      contents of the media are freely redistributable                     |
 |    Details can be found in the file aladdin-license.pdf.                  |
 |                                                                           |
 |    These conditions extend to ports to other programming languages.       |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* $Id: p_config.h,v 1.19.2.10 2002/01/28 18:12:16 tm Exp $
 *
 * PDFlib portability and configuration definitions
 *
 */

#ifndef P_CONFIG_H
#define P_CONFIG_H

/* ---------------------------- platform definitions ------------------------ */

/* #undef this if your platform doesn't support environment variables */
#define HAVE_ENVVARS

/* Compilers which are not strictly ANSI conforming can set PDF_VOLATILE
 * to an empty value.
 */
#ifndef PDF_VOLATILE
#define PDF_VOLATILE	volatile
#endif


/* ---------------------------------- WIN32  -------------------------------- */

/* try to identify Windows compilers */

#if (defined _WIN32 || defined __WATCOMC__ || defined __BORLANDC__ ||	\
	(defined(__MWERKS__) && defined(__INTEL__))) && !defined WIN32
#define	WIN32
#endif	/* <Windows compiler>  && !defined WIN32 */

#ifdef	WIN32
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"

#if defined(_WIN32_WCE) && (_WIN32_WCE >= 300)
#define PDF_PLATFORM    "Windows CE"
#define WINCE
#undef HAVE_SETLOCALE
#undef HAVE_ENVVARS
#else
#define PDF_PLATFORM    "Win32"
#endif

#endif	/* WIN32 */

/* --------------------------------- Cygnus  -------------------------------- */

#ifdef __CYGWIN__
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"
#ifdef DLL_EXPORT
    #define PDFLIB_EXPORTS
#endif

#endif /* __CYGWIN__ */

/* ---------------------------------- DJGPP  -------------------------------- */

#ifdef __DJGPP__
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"
#define PDF_PLATFORM	"Win32/DJGPP"
#endif /* __DJGPP__ */

/* ----------------------------------- OS/2  -------------------------------- */

/*
 * Try to identify OS/2 compilers.
 */

#if (defined __OS2__ || defined __EMX__) && !defined OS2
#define OS2
#endif

#ifdef	OS2
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"
#define PDF_PLATFORM	"OS/2"
#endif	/* OS2 */

/* --------------------------------- Macintosh ------------------------------ */

/* try to identify Mac compilers */

#if (defined macintosh || defined __POWERPC__ || defined __CFM68K__ || \
	defined __MC68K__) && !defined MAC
#define MAC
#endif

#ifdef	MAC
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"
#define PATHSEP		":"

#define DEBUG_TRACE_FILE	"PDFlib.trace"

#undef HAVE_ENVVARS

#define PDF_PLATFORM	"Mac OS"
#endif	/* MAC */

/* --------------------------------- AS/400 --------------------------------- */

/* try to identify the AS/400 compiler */

#if	defined __ILEC400__ && !defined AS400
#define	AS400
#endif

#ifdef AS400

#pragma comment(copyright, \
	"(C) PDFlib GmbH, Muenchen, Germany (www.pdflib.com)")

#define WRITEMODE	"wb"
#define READMODE	"rb"
#define APPENDMODE	"ab"

#define DEBUG_TRACE_FILE	"PDFlib.trace"

#define PDF_PLATFORM	"AS/400"

/* The resource files are located at /pdflib/<version>/fonts */
#define RESOURCEROOT		"/pdflib"
#define RESOURCEDIR		"/fonts"

#endif	/* AS400 */

/* --------------------- S/390 with Unix System Services -------------------- */

#ifdef	OS390

#define WRITEMODE	"wb"
#define APPENDMODE	"ab" 	/* check this */
#ifdef PDF_OS390_MVS_RESOURCE
#define PATHSEP		"("	/* for MVS */
#else
#define PATHSEP		"/"	/* for Unix System Services */
#endif

/* try to identify MVS (__MVS__ is #defined on USS and MVS!) */

#if	!defined(OS390) && defined __MVS__ && !defined MVS
#define	MVS
#endif

#ifdef	MVS

#undef PATHSEP
#define PATHSEP		"("
#undef PDF_PLATFORM
#define PDF_PLATFORM	"MVS"

#define WORDS_BIGENDIAN

#endif	/* MVS */
#endif	/* OS390 */

/* ------------------------------------ VMS --------------------------------- */

/* Not much support yet */

#ifdef	VMS
#define	PATHSEP 	"/"	/* what should we reasonably do here? */
#define READMODE	"rb", "mbc=60", "ctx=stm", "rfm=fix", "mrs=512"
#define WRITEMODE	"wb", "mbc=60", "ctx=stm", "rfm=fix", "mrs=512"
#define APPENDMODE	"ab", "mbc=60", "ctx=stm", "rfm=fix", "mrs=512"
#define PDF_PLATFORM	"VMS"
#endif	/* VMS */

/* --------------------------------- Defaults ------------------------------- */

#ifndef READMODE
#define READMODE	"r"
#endif	/* !READMODE */

#ifndef WRITEMODE
#define WRITEMODE	"w"
#endif	/* !WRITEMODE */

#ifndef APPENDMODE
#define APPENDMODE	"a"
#endif	/* !APPENDMODE */

#ifndef PATHSEP
#define PATHSEP		"/"
#endif	/* !PATHSEP */

#ifdef	_DEBUG
#define DEBUG
#endif	/* _DEBUG */

#ifndef PDF_PLATFORM
#define PDF_PLATFORM	"Unknown OS"
#endif

#endif	/* P_CONFIG_H */
