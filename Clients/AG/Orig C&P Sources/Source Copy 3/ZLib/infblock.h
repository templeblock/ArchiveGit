/* infblock.h -- header to use infblock.c
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

typedef uLong (ZEXPORT *check_func)(uLong check, const Bytef *buf, uInt len);

struct inflate_blocks_state;
typedef struct inflate_blocks_state FAR inflate_blocks_statef;

extern inflate_blocks_statef * inflate_blocks_new(
	z_streamp z,
	check_func c,				/* check function */
	uInt w);					/* window size */

extern int inflate_blocks(
	inflate_blocks_statef *,
	z_streamp ,
	int);						/* initial return code */

extern void inflate_blocks_reset(
	inflate_blocks_statef *,
	z_streamp ,
	uLongf *); 					/* check value on output */

extern int inflate_blocks_free(
	inflate_blocks_statef *,
	z_streamp);

extern void inflate_set_dictionary(
	inflate_blocks_statef *s,
	const Bytef *d,  /* dictionary */
	uInt  n);		 /* dictionary length */

extern int inflate_blocks_sync_point(
	inflate_blocks_statef *s);