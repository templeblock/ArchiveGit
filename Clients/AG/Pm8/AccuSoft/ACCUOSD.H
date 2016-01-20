/***************************************************************************/
/* 																								*/
/* MODULE: AccuOSD.h - OS Declarators. 												*/
/* 																								*/
/* 																								*/
/* CREATION DATE:  2/10/96 																*/
/* 																								*/
/* 	$Date:: 3/03/99 6:00p  $															*/
/* 	$Revision:: 1      $															*/
/* 																								*/
/* Copyright (c) 1996-97, AccuSoft Corporation.  All rights reserved.		*/
/* 																								*/
/***************************************************************************/


#ifndef __ACCUOSD_H__
#define __ACCUOSD_H__


#if defined(WIN32)

/****************************************************************************/
/* 32 Bit Windows (Windows NT and Windows 95)										 */
/****************************************************************************/

#ifdef FAR
#undef FAR
#endif

#ifdef FAR32
#undef FAR32
#endif

#ifdef NEAR
#undef NEAR
#endif

#define FAR
#define FAR32
#define HUGE
#define NEAR
#define ACCUAPI	__stdcall
#define LACCUAPI	__stdcall
#define CACCUAPI	__cdecl

#ifdef _DEBUG
#define INLINE
#else
#define INLINE
#endif

#elif defined(__unix)

/****************************************************************************/
/* Unix (SunOS, Solaris, HP-UX, AIX, SCO) 											 */
/****************************************************************************/

#define FAR
#define FAR32
#define HUGE
#define NEAR
#define ACCUAPI
#define LACCUAPI
#define CACCUAPI
#define INLINE

#elif defined(macintosh)

/****************************************************************************/
/* Macintosh																					 */
/****************************************************************************/

#define FAR
#define FAR32
#define HUGE
#define NEAR
#define ACCUAPI
#define LACCUAPI
#define CACCUAPI
#define INLINE

#elif defined(__WINDOWS_386__)

/****************************************************************************/
/* 32 Bit Watcom Windows (Windows 3.1 and Windows for Workgroups) 			 */
/****************************************************************************/

/***************************************************************************/
/* Platform specific includes 															*/
/***************************************************************************/

#define INCLUDE_COMMDLG_H
#define INCLUDE_SHELLAPI_H
#include <windows.h> 	/* Special include for 32-bit Windows 3.x 			*/

#ifdef FAR
#undef FAR
#endif

#ifdef FAR32
#undef FAR32
#endif

#define FAR
#define FAR32	far

#ifndef NEAR
#define NEAR
#endif

#define HUGE

#define ACCUAPI	__pascal
#define LACCUAPI	__pascal
#define CACCUAPI	__cdecl
#define INLINE

#elif defined(_OS2)

/****************************************************************************/
/* 			OS/2																				 */
/****************************************************************************/

#define INCL_WINSCROLLBARS
#include <os2.h>
#include <math.h>

#ifdef FAR32
#undef FAR32
#endif


#define FAR32
#define FAR
#define NEAR
#define ACCUAPI APIENTRY
#if defined(__IBMC__) || defined(__IBMCPP__)
#define LACCUAPI _Optlink
#else
#define LACCUAPI
#endif
#define CACCUAPI __cdecl

#ifndef INLINE
#define INLINE
#endif

#ifdef NULL
#undef NULL
#endif
#define NULL 0

#else

/****************************************************************************/
/* 16 Bit Windows (Windows 3.1 and Windows for Workgroups)						 */
/****************************************************************************/

#ifndef FAR
#define FAR 		__far
#endif

#ifndef FAR32
#define FAR32		__far
#endif

#define HUGE		__huge

#ifndef NEAR
#define NEAR		__near
#endif

#define ACCUAPI	__far __pascal

#ifdef AM_OCX16
#define LACCUAPI	__loadds __far __pascal
#else
#define LACCUAPI	__pascal
#endif

#define CACCUAPI	__far __cdecl

#ifdef _DEBUG
#define INLINE
#else
#define INLINE
#endif

/* #if defined(WIN32) */
#endif

#if defined(_OS2) && (defined(__IBMC__) || defined(__IBMCPP__))
#define LPACCUAPI *ACCUAPI
#define LPLACCUAPI *LACCUAPI
#define LPCACCUAPI *CACCUAPI
#else
#define LPACCUAPI ACCUAPI*
#define LPLACCUAPI LACCUAPI*
#define LPCACCUAPI CACCUAPI*
#endif

/* #ifndef __ACCUOSD_H__ */
#endif
