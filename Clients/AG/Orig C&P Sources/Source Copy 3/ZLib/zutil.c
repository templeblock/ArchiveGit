/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* @(#) $Id$ */

#include "zutil.h"

#define Z_SLOP		100

const char* z_errmsg[] =
{
	"need dictionary",	   /* Z_NEED_DICT		2  */
	"stream end",		   /* Z_STREAM_END		1  */
	"", 				   /* Z_OK				0  */
	"file error",		   /* Z_ERRNO		  (-1) */
	"stream error", 	   /* Z_STREAM_ERROR  (-2) */
	"data error",		   /* Z_DATA_ERROR	  (-3) */
	"insufficient memory", /* Z_MEM_ERROR	  (-4) */
	"buffer error", 	   /* Z_BUF_ERROR	  (-5) */
	"incompatible version",/* Z_VERSION_ERROR (-6) */
	""
};

voidpf zcalloc(voidpf opaque, unsigned items, unsigned size)
{
//j	voidpf p = malloc((long)items * size);
	voidpf p = malloc(((long)items * size) + Z_SLOP);
//j	if (p) memset(p, 0, ((long)items * size) + Z_SLOP);
	return p;
}

void zcfree(voidpf opaque, voidpf ptr)
{
	free(ptr);
}
