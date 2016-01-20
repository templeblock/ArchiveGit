/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_env.h
Version:	5.04
Purpose:	header file  for he_env.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

/* configuration control */

#include "he_defs.h"

/* TurBorland */

#if defined(__BORLANDC__) && !defined(__TURBOC__)
#define	__TURBOC__
#endif

#if defined(__TURBOC__)	&& !defined(__BORLANDC__)
#define	__BORLANDC__
#endif

/* definitions */

#define	ORD(c)	  ((int)(unsigned char)(c))	/* unsigned ordinal number */
#define	MAX(a,b)  ((a)>(b)?(a):(b))		/* max() macro */
#define	MIN(a,b)  ((a)<(b)?(a):(b))		/* min() macro */
#define	NCELLS(a) (sizeof(a)/sizeof(a[0]))	/* number of cells in an array */

#define	PN_MAX	  128				/* max pathname	length incl. a null character */
#define	TMP_MARK  '}'				/* in temporary	file names */
#define	BAK_MARK  '{'				/* in backup file names	*/

/* determine MS-DOS */

#if defined(__MSDOS__) || defined(MSDOS) \
 || defined(M_I86) || defined(M_I186) || defined(M_I286) || defined(M_I386) \
 || defined(_M_I86) || defined(_M_I186)	|| defined(_M_I286) || defined(_M_I386)
#define	HE_MSDOS_DETECTED
#endif

/* define HE_FAR_POINTERS if the default data pointer is near */

#if defined(_M_I86MM) || defined(_M_I86SM) || defined(_M_I86TM)	\
 || defined(M_I86MM) ||	defined(M_I86SM) || defined(M_I86TM) \
 || defined(__MEDIUM__)	|| defined(__SMALL__) || defined(__TINY__)
#if !defined(_M_I386) && !defined(M_I386)
#define	HE_FAR_POINTERS
#endif
#endif

/* HE_ANSI_IO */

#if defined(HE_ANSI_IO)	&& defined(HE_FAR_POINTERS)
#error HE_ANSI_IO and HE_FAR_POINTERS are incompatible
#endif

/* ANSI	C supports only	buffered I/O */

#if defined(HE_ANSI_IO)
#define	HE_BUFFERED_IO
#endif

#if defined(HE_ISL_MEM)	/* { */

#include "sllibhdr.h"	/* Inso include file */

#ifdef HE_FAR_POINTERS /* { */

#define	FAR_PTR			FAR				/* ISL */
#define	FAR_NULL		((void FAR *)0L)		/* ISL */
#define	far_memmove(d,s,n)	_fmemmove(d,s,n)		/* MS-DOS */
#define	far_memcpy(d,s,n)	Cfmemcpy(d,s,n)			/* ISL */
#define	far_memset(d,c,n)	Cfmemset(d,c,n)			/* ISL */
#define	far_strcmp(s1,s2)	Cfstrcmp(s1,s2)			/* ISL */
#define	far_strncmp(s1,s2,n)	Cfstrncmp(s1,s2,n)		/* ISL */
#define	far_strcpy(s1,s2)	Cfstrcpy(s1,s2)			/* ISL */
#define	far_strncpy(s1,s2,n)	Cfstrncpy(s1,s2,n)		/* ISL */
#define	far_strcat(s1,s2)	Cfstrcat(s1,s2)			/* ISL */
#define	far_strlen(s)		Cfstrlen(s)			/* ISL */
#define	far_strchr(s,c)		Cfstrchr(s,c)			/* ISL */
#define	far_strstr(s,r)		_fstrstr(s,r)			/* MS-DOS */
#define	far_strspn(s,r)		_fstrspn(s,r)			/* MS-DOS */
#define	far_strcspn(s,r)	_fstrcspn(s,r)			/* MS-DOS */
#ifdef HE_MSDOS_DETECTED		/* { */
#define	far_stricmp(s1,s2)	_fstricmp(s1,s2)		/* MS-DOS */
#define	far_strnicmp(s1,s2,n)	_fstrnicmp(s1,s2,n)		/* MS-DOS */
#else				/* }{ */
#define	far_stricmp(s1,s2)	_fstricomp(s1,s2)		/* local */
#define	far_strnicmp(s1,s2,n)	_fstrnicomp(s1,s2,n)		/* local */
#endif				/* } */
#define	far_stpcopy(s1,s2)	_fstpcopy(s1,s2)		/* local */
#define	far_stpncopy(s1,s2,n)	_fstpncopy(s1,s2,n)		/* local */
#define	far_atoi(s)		_fatoi(s)			/* local */
#define	far_atol(s)		_fatol(s)			/* local */

#else /* }{ */

#define	FAR_PTR							/* ignored! */
#define	FAR_NULL		NULL				/* standard */
#define	far_memmove(d,s,n)	memmove(d,s,n)			/* ANSI	*/
#define	far_memcpy(d,s,n)	Cmemcpy(d,s,n)			/* ISL */
#define	far_memset(d,c,n)	Cmemset(d,c,n)			/* ISL */
#define	far_strcmp(s1,s2)	Cstrcmp(s1,s2)			/* ISL */
#define	far_strncmp(s1,s2,n)	Cstrncmp(s1,s2,n)		/* ISL */
#define	far_strcpy(s1,s2)	Cstrcpy(s1,s2)			/* ISL */
#define	far_strncpy(s1,s2,n)	Cstrncpy(s1,s2,n)		/* ISL */
#define	far_strcat(s1,s2)	Cstrcat(s1,s2)			/* ISL */
#define	far_strlen(s)		Cstrlen(s)			/* ISL */
#define	far_strchr(s,c)		Cstrchr(s,c)			/* ISL */
#define	far_strstr(s,r)		strstr(s,r)			/* standard */
#define	far_strspn(s,r)		strspn(s,r)			/* standard */
#define	far_strcspn(s,r)	strcspn(s,r)			/* standard */
#ifdef HE_MSDOS_DETECTED		/* { */
#define	far_stricmp(s1,s2)	stricmp(s1,s2)			/* MS-DOS */
#define	far_strnicmp(s1,s2,n)	strnicmp(s1,s2,n)		/* MS-DOS */
#else				/* }{ */
#define	far_stricmp(s1,s2)	stricomp(s1,s2)			/* local */
#define	far_strnicmp(s1,s2,n)	strnicomp(s1,s2,n)		/* local */
#endif				/* } */
#define	far_stpcopy(s1,s2)	stpcopy(s1,s2)			/* local */
#define	far_stpncopy(s1,s2,n)	stpncopy(s1,s2,n)		/* local */
#define	far_atoi(s)		atoi(s)				/* standard */
#define	far_atol(s)		atol(s)				/* standard */

#endif	/* } */

#else	/* }{ */

/*#include <malloc.h>*/
#include <string.h>
#include <stdlib.h>

#ifdef HE_FAR_POINTERS /* { */

#define	FAR_PTR			__far				/* MS-DOS */
#define	FAR_NULL		((void __far *)0L)		/* MS-DOS */
#define	far_memmove(d,s,n)	_fmemmove(d,s,n)		/* MS-DOS */
#define	far_memcpy(d,s,n)	_fmemcpy(d,s,n)			/* MS-DOS */
#define	far_memset(d,c,n)	_fmemset(d,c,n)			/* MS-DOS */
#define	far_strcmp(s1,s2)	_fstrcmp(s1,s2)			/* MS-DOS */
#define	far_strncmp(s1,s2,n)	_fstrncmp(s1,s2,n)		/* MS-DOS */
#define	far_strcpy(s1,s2)	_fstrcpy(s1,s2)			/* MS-DOS */
#define	far_strncpy(s1,s2,n)	_fstrncpy(s1,s2,n)		/* MS-DOS */
#define	far_strcat(s1,s2)	_fstrcat(s1,s2)			/* MS-DOS */
#define	far_strlen(s)		_fstrlen(s)			/* MS-DOS */
#define	far_strchr(s,c)		_fstrchr(s,c)			/* MS-DOS */
#define	far_strstr(s,r)		_fstrstr(s,r)			/* MS-DOS */
#define	far_strspn(s,r)		_fstrspn(s,r)			/* MS-DOS */
#define	far_strcspn(s,r)	_fstrcspn(s,r)			/* MS-DOS */
#ifdef HE_MSDOS_DETECTED		/* { */
#define	far_stricmp(s1,s2)	_fstricmp(s1,s2)		/* MS-DOS */
#define	far_strnicmp(s1,s2,n)	_fstrnicmp(s1,s2,n)		/* MS-DOS */
#else				/* }{ */
#define	far_stricmp(s1,s2)	_fstricomp(s1,s2)		/* local */
#define	far_strnicmp(s1,s2,n)	_fstrnicomp(s1,s2,n)		/* local */
#endif				/* } */
#define	far_stpcopy(s1,s2)	_fstpcopy(s1,s2)		/* local */
#define	far_stpncopy(s1,s2,n)	_fstpncopy(s1,s2,n)		/* local */
#define	far_atoi(s)		_fatoi(s)			/* local */
#define	far_atol(s)		_fatol(s)			/* local */
#define	far_malloc(s)		_fmalloc(s)			/* MS-DOS */
#define	far_calloc(n,s)		_fcalloc(n,s)			/* MS-DOS */
#define	far_free(p)		_ffree(p)			/* MS-DOS */

#else /* }{ */

#define	FAR_PTR							/* ignored! */
#define	FAR_NULL		NULL				/* standard */
#define	far_memmove(d,s,n)	memmove(d,s,n)			/* standard */
#define	far_memcpy(d,s,n)	memcpy(d,s,n)			/* standard */
#define	far_memset(d,c,n)	memset(d,c,n)			/* standard */
#define	far_strcmp(s1,s2)	strcmp(s1,s2)			/* standard */
#define	far_strncmp(s1,s2,n)	strncmp(s1,s2,n)		/* standard */
#define	far_strcpy(s1,s2)	strcpy(s1,s2)			/* standard */
#define	far_strncpy(s1,s2,n)	strncpy(s1,s2,n)		/* standard */
#define	far_strcat(s1,s2)	strcat(s1,s2)			/* standard */
#define	far_strlen(s)		strlen(s)			/* standard */
#define	far_strchr(s,c)		strchr(s,c)			/* standard */
#define	far_strstr(s,r)		strstr(s,r)			/* standard */
#define	far_strspn(s,r)		strspn(s,r)			/* standard */
#define	far_strcspn(s,r)	strcspn(s,r)			/* standard */
#ifdef HE_MSDOS_DETECTED		/* { */
#define	far_stricmp(s1,s2)	stricmp(s1,s2)			/* MS-DOS */
#define	far_strnicmp(s1,s2,n)	strnicmp(s1,s2,n)		/* MS-DOS */
#else				/* }{ */
#define	far_stricmp(s1,s2)	stricomp(s1,s2)			/* local */
#define	far_strnicmp(s1,s2,n)	strnicomp(s1,s2,n)		/* local */
#endif				/* } */
#define	far_stpcopy(s1,s2)	stpcopy(s1,s2)			/* local */
#define	far_stpncopy(s1,s2,n)	stpncopy(s1,s2,n)		/* local */
#define	far_atoi(s)		atoi(s)				/* standard */
#define	far_atol(s)		atol(s)				/* standard */
#define	far_malloc(s)		malloc(s)			/* standard */
#define	far_calloc(n,s)		calloc(n,s)			/* standard */
#define	far_free(p)		free(p)				/* standard */

#endif	/* } */

#endif	/* } */

/* dynamic memory handler macros */

#if defined(HE_ISL_MEM)	/* { */

#include "sllibhdr.h"

SLF_PTYPDEF_OR(SLF_HDL_OBJ,VOID	FAR *,OPF_void)	memhandle_s;
typedef	SLF_HDL_OBJ memhandle_t;
#define	DM_FULL			NULLFH
#define	_dm_malloc(h,s)		SLF_PALLOC_OBJ(*(memhandle_s FAR *)&(h),s)
#define	_dm_calloc(h,n,s)	SLF_PALLOC_OBJ(*(memhandle_s FAR *)&(h),(n)*(s))
#define	_dm_free(h)		SLF_PFREE_OBJ(*(memhandle_s FAR	*)&(h))
#define	_dm_lock(h,p,t)		SLF_PLOCK_OBJ(*(memhandle_s FAR	*)&(h),p)
#define	_dm_unlock(h,p)		SLF_PUNLOCK_OBJ(*(memhandle_s FAR *)&(h),p)

#elif defined(HE_WIN_MEM) /* }{	*/

#include <windows.h>

typedef	HGLOBAL	memhandle_t;
#define	DM_FULL			0
#define	_dm_malloc(h,s)		(h=GlobalAlloc(GMEM_MOVEABLE,(s)))
#define	_dm_calloc(h,n,s)	(h=GlobalAlloc(GMEM_MOVEABLE,(n)*(s)))
#define	_dm_free(h)		GlobalFree(h)
#define	_dm_lock(h,p,t)		(p=(t)GlobalLock(h))
#define	_dm_unlock(h,p)		GlobalUnlock(h)

#elif defined(HE_CL52_MEM)

#include "vm.api"	/* defines NULL	as 0L */

typedef	HANDLE memhandle_t;
#define	DM_FULL			(HANDLE)0
#define	_dm_malloc(h,s)		(h=_xvalloc((USHORT)(s),(USHORT)0))
#define	_dm_calloc(h,n,s)	(h=_xvalloc((USHORT)(n)*(USHORT)(s),(USHORT)0))
#define	_dm_free(h)		_xvfree(h)
#define	_dm_lock(h,p,t)		(p=(t)_xvlock(h))
#define	_dm_unlock(h,p)		_xvunlock(h)

#else	/* }{ */

#include <malloc.h>

typedef	char FAR_PTR *memhandle_t;
#define	DM_FULL			FAR_NULL
#define	_dm_malloc(h,s)		(h=far_malloc(s))
#define	_dm_calloc(h,n,s)	(h=far_calloc(n,s))
#define	_dm_free(h)		far_free(h)
#define	_dm_lock(h,p,t)		(p=(t)(h))
#define	_dm_unlock(h,p)

#endif	/* } */

#ifdef HE_MEMDEBUG /* {{ */

#define	dm_malloc(h,s)		(_d0_dm_malloc(__FILE__,(long)__LINE__), (_dm_malloc(h,s)), _d1_dm_malloc(h), h)
#define	dm_calloc(h,n,s)	(_d0_dm_malloc(__FILE__,(long)__LINE__), (_dm_calloc(h,n,s)), _d1_dm_malloc(h),	h)
#define	dm_free(h)		_d0_dm_free(h,__FILE__,(long)__LINE__);	(_dm_free(h)); _d1_dm_free(h,__FILE__,(long)__LINE__); h=DM_FULL
#define	dm_lock(h,p,t)		(_d0_dm_lock(h,__FILE__,(long)__LINE__), (_dm_lock(h,p,t)))
#define	dm_unlock(h,p)		_d0_dm_unlock(h,__FILE__,(long)__LINE__); _dm_unlock(h,p); p=FAR_NULL

void _d0_dm_malloc(char	*fn, long line);
void _d1_dm_malloc(memhandle_t h);
void _d0_dm_free(memhandle_t h,	char *fn, long line);
void _d1_dm_free(memhandle_t h,	char *fn, long line);
void _d0_dm_lock(memhandle_t h,	char *fn, long line);
void _d0_dm_unlock(memhandle_t h, char *fn, long line);

#ifdef HE_MSDOS_DETECTED /* { */
#include <stdio.h>
void _d_fprint_map(FILE	*stream);
#endif	/* } */

#else	/* }{ */

#define	dm_malloc(h,s)		_dm_malloc(h,s)
#define	dm_calloc(h,n,s)	_dm_calloc(h,n,s)
#define	dm_free(h)		_dm_free(h)
#define	dm_lock(h,p,t)		_dm_lock(h,p,t)
#define	dm_unlock(h,p)		_dm_unlock(h,p)

#endif	/* }} */

/* prototypes */

#ifdef HE_FAR_POINTERS /* { */

#ifndef	HE_MSDOS_DETECTED /* { */
int _fstricomp(char FAR_PTR *s1, char FAR_PTR *s2);
int _fstrnicomp(char FAR_PTR *s1, char FAR_PTR *s2, int	n);
#endif	/* } */

char FAR_PTR *_fstpcopy(char FAR_PTR *to, char FAR_PTR *from);
char FAR_PTR *_fstpncopy(char FAR_PTR *to, char	FAR_PTR	*from, int n);
int _fatoi(const char FAR_PTR *str);
long _fatol(const char FAR_PTR *str);

#else	/* }{ */

#ifndef	HE_MSDOS_DETECTED /* { */
int stricomp(char *s1, char *s2);
int strnicomp(char *s1,	char *s2, int n);
#endif	/* } */

char *stpcopy(char *to,	char *from);
char *stpncopy(char *to, char *from, int n);

#endif	/* } */

/* compiler dependencies */

#ifdef __BORLANDC__ /* { */
#define	_REGS		REGS
#define	_SREGS		SREGS
#define	_intdos(q,r)	intdos(q,r)
#define	_intdosx(q,r,s)	intdosx(q,r,s)
/*#define _unlink(n)	unlink(n)*/
#define	_O_RDONLY	O_RDONLY
#define	_O_WRONLY	O_WRONLY
#define	_O_RDWR		O_RDWR
#define	_O_APPEND	O_APPEND
#define	_O_CREAT	O_CREAT
#define	_O_TRUNC	O_TRUNC
#define	_O_EXCL		O_EXCL
#define	_O_BINARY	O_BINARY
#define	_O_TEXT		O_TEXT
#endif	/* } */

#if (defined(M_I86) && !defined(_M_I86)) \
 || defined(__GNUC__)	/* { */			/* MSC 5.1 or GNU C/C++	*/
#define	_REGS		REGS
#define	_SREGS		SREGS
#define	_intdos		intdos
#define	_intdosx	intdosx
#define	_O_RDONLY	O_RDONLY
#define	_O_WRONLY	O_WRONLY
#define	_O_RDWR		O_RDWR
#define	_O_APPEND	O_APPEND
#define	_O_CREAT	O_CREAT
#define	_O_TRUNC	O_TRUNC
#define	_O_EXCL		O_EXCL
#define	_O_BINARY	O_BINARY
#define	_O_TEXT		O_TEXT
#define	_creat		creat
#define	_open		open
#define	_close		close
#define	_read		read
#define	_write		write
#endif	/* } */

/* end */
