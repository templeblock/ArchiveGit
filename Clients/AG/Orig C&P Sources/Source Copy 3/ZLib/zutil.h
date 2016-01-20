/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* @(#) $Id$ */

#ifndef _Z_UTIL_H
#define _Z_UTIL_H

#include "zlib.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define local static

typedef unsigned char  uch;
typedef uch FAR uchf;
typedef unsigned short ush;
typedef ush FAR ushf;
typedef unsigned long  ulg;

extern const char *z_errmsg[10]; /* indexed by 2-zlib_error */

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]

/* To be used only when the state is known to be valid */
#define ERR_RETURN(strm,err) return (strm->msg = (char*)ERR_MSG(err), (err))

/* default windowBits for decompression. MAX_WBITS is for compression only */
#define DEF_WBITS MAX_WBITS

#define DEF_MEM_LEVEL 8 // MAX_MEM_LEVEL

/* The three kinds of block type */
#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES	 2

/* The minimum and maximum match lengths */
#define MIN_MATCH  3
#define MAX_MATCH  258

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

/* Diagnostic functions */
//j #define DEBUG

#ifdef DEBUG
	#include <stdio.h>
	#include <windows.h>

	#define z_verbose 0
	#define TraceOutput(x) { char str[256]; sprintf x; OutputDebugStringA(str); }
	#define Assert(cond, msg) { if(!(cond)) { Trace((str, msg)); exit(1); } }
	#define Trace(x)	{if (z_verbose >=0) TraceOutput(x) ;}
	#define Tracev(x)	{if (z_verbose >=1) TraceOutput(x) ;}
	#define Tracevv(x)	{if (z_verbose >=2) TraceOutput(x) ;}
#else
	#define Assert(cond, msg)
	#define Trace(x)
	#define Tracev(x)
	#define Tracevv(x)
#endif

#define ZALLOC(strm, items, size)  (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))

/* functions */
voidpf zcalloc(voidpf opaque, unsigned items, unsigned size);
void zcfree(voidpf opaque, voidpf ptr);

#endif /* _Z_UTIL_H */
