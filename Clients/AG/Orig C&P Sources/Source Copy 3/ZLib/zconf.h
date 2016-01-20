/* zconf.h -- configuration of the zlib compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* @(#) $Id$ */

#ifndef _ZCONF_H
#define _ZCONF_H

#define STDC

/* Maximum value for memLevel in deflateInit2 */
#define MAX_MEM_LEVEL 8

/* Maximum value for windowBits in deflateInit2 and inflateInit2.
 * WARNING: reducing MAX_WBITS makes minigzip unable to extract .gz files
 * created by gzip. (Files created by minigzip can still be extracted by
 * gzip.)
 */
#define MAX_WBITS   15 /* 32K LZ77 window */

/* The memory requirements for deflate are (in bytes):
			(1 << (windowBits+2)) +  (1 << (memLevel+9))
 that is: 128K for windowBits=15  +  128K for memLevel = 8	(default values)
 plus a few kilobytes for small objects. For example, if you want to reduce
 the default memory requirements from 256K to 128K, compile with
	 make CFLAGS="-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7"
 Of course this will generally degrade compression (there's no free lunch).

   The memory requirements for inflate are (in bytes) 1 << windowBits
 that is, 32K for windowBits=15 (default value) plus a few kilobytes
 for small objects.
*/

/* Type declarations */

#define ZEXPORT
#define ZEXTERN extern
#ifndef FAR
	#define FAR
#endif

typedef unsigned char  Byte;  /* 8 bits */
typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */
typedef Byte Bytef;

typedef char  charf;
typedef int   intf;
typedef uInt  uIntf;
typedef uLong uLongf;

typedef void* voidpf;
typedef void* voidp;

#define  z_off_t long

#endif /* _ZCONF_H */
