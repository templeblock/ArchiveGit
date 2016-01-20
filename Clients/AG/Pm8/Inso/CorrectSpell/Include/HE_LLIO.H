/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_llio.h
Version:	5.05
Purpose:	header file for	he_llio.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

#if defined(HE_ISL_IO) /* { */

#if !defined(MSDOS)   \
 && !defined(VAX)     \
 && !defined(UNIX)    \
 && !defined(MAC)     \
 && !defined(POWERMAC)\
 && !defined(WINDOWS) \
 && !defined(WIN32)   \
 && !defined(OS2)
#error undefined target	environment
#endif	/* } */

#include "sllibhdr.h"	/* Inso include file */

#ifdef HE_FAR_POINTERS		/* { */

#define	LL_HANDLE		SL_FHANDLE
#define	LL_SIZE_T		SWORD
#define	LL_ERROR		NEGONE
#define	LL_SEEK_ERROR		NEGONE
#define	LL_SEEK_SET		SEEK_BOF
#define	LL_SEEK_CUR		SEEK_COF
#define	LL_SEEK_END		SEEK_EOF
#define	ll_read(h,b,s)		Cfarread(h,(PFCHAR)b,(UWORD)s)
#define	ll_write(h,b,s)		Cfarwrite(h,(PFCHAR)b,(UWORD)s)
#define	ll_seek(h,n,o)		Cfarlseek(h,(INT4B)n,(INT2B)o)
#define	ll_length(h)		Cfarlseek(h,0L,LL_SEEK_END)
#define	LL_RO			OPEN_RB
#define	LL_WO			OPEN_CWEB
#define	LL_RW			OPEN_RWB
#define	ll_creat(p)		Cfaropen((PFCHAR)p,(SWORD)OPEN_CWB)
#define	ll_open(p,m)		Cfaropen((PFCHAR)p,(SWORD)m)
#define	ll_close(h)		Cfarclose(h)
#define	ll_rename(o,n)		isl__rename(o,n)
#define	ll_unlink(n)		isl__unlink(n)
LL_SIZE_T isl__rename(char FAR_PTR *oldname, char FAR_PTR *newname);
LL_SIZE_T isl__unlink(char FAR_PTR *name);

#else /* default pointers */	/* }{ */

#define	LL_HANDLE		SL_FHANDLE
#define	LL_SIZE_T		SWORD
#define	LL_ERROR		NEGONE
#define	LL_SEEK_ERROR		NEGONE
#define	LL_SEEK_SET		SEEK_BOF
#define	LL_SEEK_CUR		SEEK_COF
#define	LL_SEEK_END		SEEK_EOF
#define	ll_read(h,b,s)		Cfarread(h,(PFCHAR)b,(UWORD)s)
#define	ll_write(h,b,s)		Cfarwrite(h,(PFCHAR)b,(UWORD)s)
#define	ll_seek(h,n,o)		Cfarlseek(h,(INT4B)n,(INT2B)o)
#define	ll_length(h)		Cfarlseek(h,0L,LL_SEEK_END)
#define	LL_RO			OPEN_RB
#define	LL_WO			OPEN_CWEB
#define	LL_RW			OPEN_RWB
#define	ll_creat(p)		Cfaropen((PFCHAR)p,(SWORD)OPEN_CWB)
#define	ll_open(p,m)		Cfaropen((PFCHAR)p,(SWORD)m)
#define	ll_close(h)		Cfarclose(h)
#define	ll_rename(o,n)		rename(o,n)
#define	ll_unlink(n)		Cremove(n)	/*_unlink(n)*/

#endif				/* } */

#elif defined(HE_WIN_IO) \
   || defined(HE_WIN32_IO)	/* }{ */

#include <windows.h>
#include <io.h>

#define	LL_HANDLE		HFILE
#define	LL_SIZE_T		UINT
#define	LL_ERROR		HFILE_ERROR
#define	LL_SEEK_ERROR		(long)HFILE_ERROR
#define	LL_SEEK_SET		0
#define	LL_SEEK_CUR		1
#define	LL_SEEK_END		2
#define	ll_read(h,b,s)		_lread(h,(LPVOID)b,(LL_SIZE_T)s)
#define	ll_write(h,b,s)		_lwrite(h,(LPVOID)b,(LL_SIZE_T)s)
#define	ll_seek(h,n,o)		_llseek(h,n,o)
#define	ll_length(h)		_llseek(h,0L,LL_SEEK_END)
#define	LL_RO			OF_READ
#define	LL_WO			OF_WRITE
#define	LL_RW			OF_READWRITE
#define	ll_creat(p)		_lcreat((LPCSTR)p,0)
#define	ll_open(p,m)		_lopen((LPCSTR)p,m)
#define	ll_close(h)		_lclose(h)
#if defined (HE_FAR_POINTERS)	/* { */
#define	ll_rename(o,n)		win__rename(o,n)
#define	ll_unlink(n)		win__unlink(n)
unsigned win__rename(char FAR_PTR *oldname, char FAR_PTR *newname);
unsigned win__unlink(char FAR_PTR *name);
#else				/* }{ */
#ifdef HE_WIN32_IO		/* { */
#ifdef _MAC
#define	ll_rename(o,n)		rename(o,n)
#define	ll_unlink(n)		(DeleteFile(n)?(LL_ERROR+1):(LL_ERROR))
#else
#define	ll_rename(o,n)		(MoveFile(o,n)?(LL_ERROR+1):(LL_ERROR))
#define	ll_unlink(n)		(DeleteFile(n)?(LL_ERROR+1):(LL_ERROR))
#endif
#else				/* }{ */
#define	ll_rename(o,n)		rename(o,n)
#define	ll_unlink(n)		remove(n)	/*_unlink(n)*/
#endif				/* } */
#endif				/* } */

#else /* DOS/UNIX I/O */	/* }{ */

#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef __GNUC__	/* { */
#include <unistd.h>
#endif	/* } */

#if defined(HE_FAR_POINTERS)	/* { */

#define	LL_HANDLE		unsigned
#define	LL_SIZE_T		unsigned
#define	LL_ERROR		(0xffffU)
#define	LL_SEEK_ERROR		(-1L)
#define	LL_SEEK_SET		0
#define	LL_SEEK_CUR		1
#define	LL_SEEK_END		2
#define	ll_read(h,b,s)		dos__read(h,(void _far *)b,(LL_SIZE_T)s)
#define	ll_write(h,b,s)		dos__write(h,(void _far	*)b,(LL_SIZE_T)s)
#define	ll_seek(h,n,o)		dos__seek(h,n,o)
#define	ll_length(h)		dos__seek(h,0L,LL_SEEK_END)
#define	LL_RO			0
#define	LL_WO			1
#define	LL_RW			2
#define	ll_creat(p)		dos__creat(p,0)
#define	ll_open(p,m)		dos__open(p,m)
#define	ll_close(h)		dos__close(h)
#define	ll_rename(o,n)		dos__rename(o,n)
#define	ll_unlink(n)		dos__unlink(n)
long dos__seek(int h, long n, int o);
unsigned dos__read(int h, void FAR_PTR *b, unsigned s);
unsigned dos__write(int	h, void	FAR_PTR	*b, unsigned s);
unsigned dos__creat(char FAR_PTR *p, int a);
unsigned dos__open(char	FAR_PTR	*p, int	m);
unsigned dos__close(int	h);
unsigned dos__rename(char FAR_PTR *oldname, char FAR_PTR *newname);
unsigned dos__unlink(char FAR_PTR *name);

#else /* default pointers */	/* }{ */

#define	LL_HANDLE		int
#define	LL_SIZE_T		int
#define	LL_ERROR		(-1)
#define	LL_SEEK_ERROR		(-1L)
#define	LL_SEEK_SET		SEEK_SET
#define	LL_SEEK_CUR		SEEK_CUR
#define	LL_SEEK_END		SEEK_END
#define	ll_read(h,b,s)		_read(h,(void *)b,(unsigned)s)
#define	ll_write(h,b,s)		_write(h,(void *)b,(unsigned)s)
#define	ll_seek(h,n,o)		lseek(h,n,o)
#define	ll_length(h)		lseek((int)h,0L,LL_SEEK_END)
#define	LL_RO			(_O_RDONLY|_O_BINARY)
#define	LL_WO			(_O_WRONLY|_O_BINARY)
#define	LL_RW			(_O_RDWR|_O_BINARY)
#define	ll_creat(p)		_creat(p,0)
#define	ll_open(p,m)		_open(p,m)
#define	ll_close(h)		_close(h)
#define	ll_rename(o,n)		rename(o,n)
#define	ll_unlink(n)		remove(n)	/*_unlink(n)*/

#endif				/* } */

#endif				/* } */

/* end */
