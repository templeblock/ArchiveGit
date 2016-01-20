/*  cndxnt.h - nt using visual c 32 bit nt compiler - release 1.1
 *   ---------------------------------------------------------
 *  |                                                         |
 *  |  C-INDEX/II                                             |
 *  |  Version 5.0                                            |
 *  |                                                         |
 *  |  Trio Systems LLC                                       |
 *  |  (c) Copyright 1983 - 1996                              |
 *  |                                                         |
 *   ---------------------------------------------------------
 */


/*
 *     Include files will differ with each compiler.
 */

#ifndef NO_INCLUDES				/* includes needed for library compilation */
#include <windows.h>
#include <dos.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <memory.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\locking.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <winbase.h>
#endif	/* !NO_INCLUDES */

/*
 *     Flags that control inclusion of compiler specific code
 *     can be put here.
 */

#define MSC5						/* include code that works with MSC 5.0/5.1 */

/* define this to treat buffers as far space items in mixed model code */
/* #define MIXED_MODEL_BUFFERS	*/ /* do not use for normal memory models */

/* include macros and code for computers requiring work alignment */
#define REQUIRES_WORD_ALIGNMENT			/* force word alignment code */

/*
 *   If the compiler being used is ANSI compatible, this
 *   definition is required to force inclusion of
 *   the correct function prototypes.
 */

#define ANSI						/* ANSI compatible compiler */


/*
 * All functions in C-Index have the FDECL type of function declaration.
 * By defining this as _pascal, _fastcall, or _cdecl, the system
 * can be compiled to use different function calling conventons.
 * This can improve performance and reduce code size with some compilers.
 */

#ifdef REMOVED
#define FDECL _fastcall
#endif

#define FDECL

/*
 *   When using Unix style low level disk I/O, the open
 *   function returns -1 for failing to open or create.
 *   When using high level Unix style disk I/O, the fopen
 *   function returns 0 (NULL) when failing to open or
 *   create.
 */

#define NULLFD   INVALID_HANDLE_VALUE				/* null file descriptor */


/*
 *   This flag indicates to C-Index whether or not the
 *   machine the code is running on stores number in intel
 *   format, that is low to high byte order, or non-intel
 *   high to low byte order (such as 68000 processors).
 */

#define INTELFMT	(1)			/* intel machine == 1, non-intel = 0 */

/*
 *   This flag indicates if integers are represented as 16 bit
 *   values (same as short).  Comment out this define if
 *   int is 32 bytes (same as long).
 */

/* #define INTISSHORT */



/*
 * define this for the compressing version (not currently support under NT)
 *
 * #define COMPRESSION
 */


#ifdef COMPRESSION
/*
 *   The internal node work buffers must be much larger than
 *   the file node size to accomodate processing the node after
 *   expansion.  MAXCOMP can be adjusted to optimize for memory
 *   usage vs. maximum number of bytes compressed into one node.
 *   MINCOMP determines the minimum level of compression that will
 *   be attempted.  This can be adjusted for trade-off between 
 *   optimum speed and compression ratios.
 */

#define MAXCOMP 4						/* maximum compression ratio */
#define MINCOMP MAXCOMP				/* min compression ratio desired */

/* buffer size for handling overflow due to insert and compress/expand */
#define CI_BIGBUF ((NODESZE * MAXCOMP) + KEYSZE + 1)
#endif		/* COMPRESSION */


/*
 * Enable this define to include the transaction logging features
 */

#define TRANSACTION


/*
 * Enable this define to include the image backup feature
 */

#define IMAGEBACK

/*
 * Enable this define to compile with current reading feature
 */

#define READSHARE

#ifdef READSHARE
#define NEWLOWLOCK					/* use compiler dependent low lock */
#endif	/* READSHARE */



/* COMPILER SPECIFIC TYPEDEFS */

/*
 *   When using the Unix style low level disk I/O (open and
 *   create) the value returned for the file handle is of
 *   type int.  When using Unix style high level disk I/O
 *   (fopen and fcreate) the file handle is of type FILE *.
 */

typedef HANDLE FILHND;							/* file handle for WIN32 disk operations */


/*
 *   Most compilers will use 16 bit shorts and 32 bit longs.
 *   For compilers using other word lengths for these
 *   variable types, these two definitions will need
 *   changing.
 */

typedef short INT16;							/* 16 bit signed integers */
#if _MSC_VER < 1200	// GCB 11/9/98 - VC6 typedefs INT32 as int
	typedef long  INT32;							/* 32 bit signed integers */
#endif


/*
 *   The default node pointer in C-Index/II is 32 bits
 *   (long).  If the compiler uses other than 32 bits for
 *   longs, or 16 bit node pointers are desired (as used in
 *   C-Index/Plus), NDEPTR will need to redefined.
 */

#ifdef PLUS_COMPATIBLE						/* if using C-Index plus files */
typedef short NDEPTR;						/* 16 bit node numbers */
#else
typedef long NDEPTR;							/* 32 bit node numbers */
#endif


/*
 *   Some preprocessor names are potentially in conflict
 *   with other header files (for the compiler libraries or
 *   other add-on libraries).  These are removed by defining
 *   the NEW_FLDTYPES flag.  This will cause programs written 
 *   according to the C-Index/PC manual to be incorrect, so 
 *   some changes to programs converted from C-Index/PC or 
 *   C-Index/Plus may require changing.
 */

#define NEW_FLDTYPES

/* If you specify NEW_FLDTYPES it may be neccessary to add back some of
 * the following old field types if converting programs from C-Index/Plus
 * or C-Index/PC:
 *
 * #define OK CCOK
 * #define STRING STRINGFLD
 * #define BINARY BINARYFLD
 * #define INT INTFLD
 * #define SHORTINT SHORTFLD
 * #define LONG LONGFLD
 * #define FLOAT FLOATFLD
 * #define DOUBLE DOUBLEFLD
 * #define FIELD DATALIST
 */

/* MISC. SIZE DEFINITIONS */

/*
 *   The default node size in C-Index/II is 1K.  The minimum
 *   size node is 1K.  It may be desirable for other
 *   operating systems or media to use larger node sizes.
 *   For example, speed accessing CD-ROM media may be
 *   increased by using 10K nodes.
 */

#define NODESZE 1024							/* size of node */


/*
 *   This hard coded number is used to determine the size of
 *   data chunks stored in sub-entries.  This size is
 *   dictated by:
 *
 *        A. The node size (default 1024 bytes).
 *        B. The overhead of the node header information
 *        (default 18 bytes for 32 bit node pointers).
 *        C. The overhead of each subentry (7 bytes).
 *        D. The maximum sub-entry length (255 bytes).
 *
 *   The computation for the default value is as follows:
 *
 *        Key area size = 1024 - 18 = 1006 bytes
 *        Maximum Sub-Entry Size = 244 + 7 = 251 bytes
 *        Total Sub-Entry Space = 4 * 251 = 1004 bytes
 *        Optimal wasted space = 1006 - 1004 = 2 bytes
 *
 *
 *   The objective is to minimize the wasted space.  If the
 *   node size or the node pointer size is changed, DATAPART
 *   will need to be recomputed.
 */

#define DATAPART 244							/* size of data chunks */


/*
 *   These two numbers should not be changed.  The maximum
 *   data record size is the product of how many sub-entries
 *   a record may have (128) and the maximum size of data in
 *   each entry (DATAPART).  For the default DATAPART value
 *   of 244, the maximum record (when using a null key) is
 *   31232 bytes.  The actual maximum is determined by macro
 *   gooddlen() in CINDEXWK.H which takes into account the
 *   size of the key.
 */

#define MAXFLDLEN (128 * DATAPART)		/* max single-key data rec size */
#define MAXDLEN MAXFLDLEN					/* max multi-key data rec size */

/*
 *   The default maximum number of indexes is the same as C-Index/Plus
 *   (20), however, the last five indexes are reserved.  If you are
 *   using any of indexes 16-20 with single key routines, the maximum
 *   number of keys should be changed so that the highest key in use
 *   is 5 less than the maximum.
 */

#define MAXINDXS 20							/* max number of indices */


/*
 *   These macros invoke compiler specific functions to perform
 *   block move and clear operations in C-Index.
 */

/*
 *   blockmv moves a single block of non-overlapping memory.  Be
 *   sure to call a machine coded function for this because
 *   performance will be cut by approximately 30% when
 *   coding this function in C.
 */

#define blockmv(des, src, len)	memcpy(des, src, (unsigned)len)

/*
 *   Clears a block of memory to a specific value.  Should
 *   call a machine coded function for maximum speed.
 */

#define cclear(des, len, val)		memset(des, val, (unsigned)len)

/*
 *   Move a block of bytes to right in a node.  Must handle
 *   overlapping memory locations.
 */

#define cmovert(p, len, farval)	memmove(p+farval, p, (unsigned)len)

/*
 *   Move a block of bytes to left in a node.  Although
 *   memory overlaps, this function will work correctly if
 *   the memory is copied from the lowest to highest memory
 *   location.  Pick a machine coded function which either
 *   works this way or which correctly handles overlapping
 *   memory locations in general.
 */

#define cmovelt(p, len, farval)	memmove(p-farval, p, (unsigned)len)


/*   These macros are used to control the modification of byte order
 *   in the index control structure numeric values stored in the file.
 *   Do not changes these macros.
 */

#define isintel(wordorder)	(*((char *)&wordorder) == 1)
#define inteltol(lint, wordorder)	((	(INTELFMT && (*((char *)&wordorder) == 1)) \
			|| (!INTELFMT && !(*((char *)&wordorder) == 1)) ) \
			? lint \
			: finteltol(lint, wordorder))

/*
 *   Node information is the size of NDEPTR, which is 32 bits by default.
 *   For compilers with long intergers of some other size, these macros
 *   should be modified.
 */

#ifdef PLUS_COMPATIBLE						/* if using C-Index plus files */
#define intelton(lint, order) inteltos(lint, order)/* intel node ptr to native */
#define ntointel(lint, order) stointel(lint, order)/* native node ptr to intel */
#else
#define intelton(lint, order) inteltol(lint, order)/* intel node ptr to native */
#define ntointel(lint, order) ltointel(lint, order)/* native node ptr to intel */
#endif


/*
 *   Intel processors do not require that word values (ints,
 *   and longs) be aligned on word boundaries.  Other
 *   processors, such as 68000 and 370 computers, require
 *   word boundary alignment.  The trnsrec and trnsnde
 *   macros must be defined using the correct method for the
 *   processor.
 */

#ifdef REQUIRES_WORD_ALIGNMENT
/* transfer macros for machines that DO require word boundary alignment */
#define trnsrec(des, src) blockmv((char *) des, (char *) src, SZEREC)			/* transfers a data record number */
#define trnsnde(des, src) blockmv((char *) des, (char *) src, SZENDE)			/* transfers an index node pointer */
#define trnsshort(des, src) blockmv((char *) des, (char *) src, sizeof(INT16))/* transfers a 16 bit int */
#define trnsint(des, src) blockmv((char *) des, (char *) src, sizeof(int))		/* transfers a machine size int */
#define trnslong(des, src) blockmv((char *) des, (char *) src, sizeof(long))	/* transfers a long */

#ifdef NO_WORD_CONVERSION
#define trnsfloat(des, src) *(float *) (des) = *(float *) (src);	/* transfers a float */
#define trnsdbl(des, src) *(double *) (des) = *(double *) (src);	/* transfers a double */
#else
#define trnsfloat(des, src) blockmv((char *) des, (char *) src, sizeof(float))/* transfers a float */
#define trnsdbl(des, src) blockmv((char *) des, (char *) src, sizeof(double))	/* transfers a double */
#endif	/* NO_WORD_CONVERSION */


#else
/* transfer macros for machines that DO NOT require word boundary alignment */

#define trnsrec(des, src) *(INT32 *) (des) = *(INT32 *) (src);		/* transfers a data record number */
#define trnsnde(des, src) *(NDEPTR *) (des) = *(NDEPTR *) (src);	/* transfers an index node pointer */
#define trnsshort(des, src) *(INT16 *) (des) = *(INT16 *) (src);	/* transfers a 16 bit integer */
#define trnsint(des, src) *(int *) (des) = *(int *) (src);			/* transfers a machine size integer */
#define trnslong(des, src) *(INT32 *) (des) = *(INT32 *) (src);	/* transfers a long */
#define trnsfloat(des, src) *(float *) (des) = *(float *) (src);	/* transfers a float */
#define trnsdbl(des, src) *(double *) (des) = *(double *) (src);	/* transfers a double */

#endif		/* REQUIRES_WORD_ALIGNMENT and not REQUIRES_WORD_ALIGNMENT */




/* ------- memory management macros for current key information ------ */

/*
 *   The memory management macros speed up processing of
 *   current key information.
 */

/*
 *   The tstcuradr and lockcuradr macros will only require
 *   changing if NULL does not indicate that there has not
 *   been space allocated.
 */

#define tstcuradr(psp, keyn)	(psp->curinfoptr[keyn])
#define lockcuradr(psp, keyn)	((psp->curinfoptr[keyn]) \
											? (CURINFO *)psp->curinfoptr[keyn] \
											: xlockcuradr(psp, keyn))

/* do not use these macros for moveable memory */
#ifndef MEMORY_MANAGEMENT_REQUIRED
#define getcurrcval(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (INT32)(((CURINFO *)(psp->curinfoptr[keyn]))->currcval) \
												: fgetrcval(psp, keyn))

#define getcurdbyte(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFO *)(psp->curinfoptr[keyn]))->curdbyte) \
												: fgetdbyte(psp, keyn))

#define getkeynum(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFO *)(psp->curinfoptr[keyn]))->keynum) \
												: fgetkeynum(psp, keyn))

#define getndenum(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (NDEPTR)(((CURINFO *)(psp->curinfoptr[keyn]))->ndenum) \
												: fgetndenum(psp, keyn))

#define getcurindx(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFO *)(psp->curinfoptr[keyn]))->curindx) \
												: fgetindx(psp, keyn))

#define getdelflag(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFO *)(psp->curinfoptr[keyn]))->delflag) \
												: fgetdelflag(psp, keyn))

#define getupdkeyinfo(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFO *)(psp->curinfoptr[keyn]))->updkeyinfo) \
												: fgetupdkeyinfo(psp, keyn))

#define putcurrcval(psp, keyn, val)	if(psp->curinfoptr[keyn]) \
												 ((CURINFO *)(psp->curinfoptr[keyn]))->currcval = (INT32)val; \
												else \
												 fputrcval(psp, keyn, val);

#define putcurdbyte(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFO *)(psp->curinfoptr[keyn]))->curdbyte = (char)val; \
												else \
												 fputdbyte(psp, keyn, val);

#define putkeynum(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFO *)(psp->curinfoptr[keyn]))->keynum = val; \
												else \
												 fputkeynum(psp, keyn, val);

#define putndenum(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFO *)(psp->curinfoptr[keyn]))->ndenum = val; \
												else \
												 fputndenum(psp, keyn, val);

#define putcurindx(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFO *)(psp->curinfoptr[keyn]))->curindx = val; \
												else \
												 fputindx(psp, keyn, val);

#define putdelflag(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFO *)(psp->curinfoptr[keyn]))->delflag = (char)val; \
												else \
												 fputdelflag(psp, keyn, val);

#define putupdkeyinfo(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFO *)(psp->curinfoptr[keyn]))->updkeyinfo = (char)val; \
												else \
												 fputupdkeyinfo(psp, keyn, val);
#endif	/* !MEMORY_MANAGEMENT_REQUIRED */

/*
 *   This do nothing macro replaces the function call that
 *   unlocks the current key handle.  The unlockcuradr
 *   function is required only for systems that use memory
 *   management, such as Microsoft Windows and Macintosh.
 *   For those systems, define MEMORY_MANAGEMENT_REQUIRED
 *   at the top of this header file.
 */

#ifndef MEMORY_MANAGEMENT_REQUIRED
#define unlockcuradr(psp, keyn)		/* do nothing macro */
#endif

/*
 *   Various definitions that are required. Do not change.
 */

#ifdef MEMORY_MANAGEMENT_REQUIRED		/* use this typedef for Window or Mac */
typedef Handle ALLOCHND;					/* allocated space handle */
typedef Handle CURINFOLST;					/* list of handles to cur key info */
#else
#ifdef MIXED_MODEL_BUFFERS
typedef char far * ALLOCHND;				/* allocated far space pointer */
#else
typedef char * ALLOCHND;					/* allocated space pointer */
#endif	/* MIXED_MODEL_BUFFERS */
typedef void *CURINFOLST;					/* list of ptrs to cur key info */
#define cilockbuf(psp, adr) ((BCBPTR)(adr))	/* dummy lock buffer routine */
#define ciunlockbuf(psp, adr)				/* dummy unlock buffer routine */
#endif	/* MEMORY_MANAGEMENT_REQUIRED */

/*
 *   These definitions move short and int values to and from
 *   unaligned memory locations.  These routines should be
 *   used for machines that do not require word alignment
 *   because they use assignment statements which are much
 *   faster than calls to block move.
 */

#ifndef REQUIRES_WORD_ALIGNMENT
#define getshort(dstvar, srcadr)		dstvar = *((short *)srcadr)
#define putint(dstadr, srcvar)		*((int *) dstadr) = srcvar
#define ciputshort(dstadr, srcvar)		*((short *) dstadr) = srcvar
#define ciputlong(dstadr, srcvar)		*((long *) dstadr) = srcvar

#ifdef NO_WORD_CONVERSION
#define putfloat(dstadr, srcvar)		*((float *) dstadr) = srcvar
#define putdouble(dstadr, srcvar)	*((double *) dstadr) = srcvar
#endif	/* NO_WORD_CONVERSION */

#define movint(dstadr, srcadr)		*((int *) dstadr) = *((int *) srcadr)
#define movshort(dstadr, srcadr)		*((short *) dstadr) = *((short *) srcadr)
#define movlong(dstadr, srcadr)		*((long *) dstadr) = *((long *) srcadr)
#define retint(srcadr)					(*((int *) srcadr))
#define retshort(srcadr)				(*((short *) srcadr))
#define retlong(srcadr)					(*((long *) srcadr))

#ifdef NO_WORD_CONVERSION
#define retfloat(srcadr)				(*((float *) srcadr))
#define retdouble(srcadr)				(*((double *) srcadr))
#endif	/* NO_WORD_CONVERSION */

#ifdef NO_WORD_CONVERSION
#define movfloat(dstadr, srcadr)		*((float *) dstadr) = *((float *) srcadr)
#define movdouble(dstadr, srcadr)	*((double *) dstadr) = *((double *) srcadr)
#else
#define movfloat(dstadr, srcadr)		memcpy(dstadr, srcadr, sizeof(float))
#define movdouble(dstadr, srcadr)	memcpy(dstadr, srcadr, sizeof(double))
#endif	/* NO_WORD_CONVERSION */

#endif	/* !REQUIRES_WORD_ALIGNMENT */


/*
 *   These macros are used for machines that require access to words
 *   on word boundaries.
 */

#ifdef REQUIRES_WORD_ALIGNMENT
#define getshort(dstvar, srcadr)		memcpy(&dstvar, srcadr, sizeof(short))
#define movint(dstadr, srcadr)		memcpy(dstadr, srcadr, sizeof(int))
#define movshort(dstadr, srcadr)		memcpy(dstadr, srcadr, sizeof(short))
#define movlong(dstadr, srcadr)		memcpy(dstadr, srcadr, sizeof(long))
#define movfloat(dstadr, srcadr)		memcpy(dstadr, srcadr, sizeof(float))
#define movdouble(dstadr, srcadr)	memcpy(dstadr, srcadr, sizeof(double))

extern void FDECL putint(void *dstadr, int srcvar);
extern void FDECL ciputshort(void *dstadr, short srcvar);
extern void FDECL ciputlong(void *dstadr, long srcvar);
extern void FDECL putfloat(void *dstadr, float srcvar);
extern void FDECL putdouble(void *dstadr, double srcvar);

extern int FDECL retint(void *valptr);
extern short FDECL retshort(void *valptr);
extern long FDECL retlong(void *valptr);
extern float FDECL retfloat(void *valptr);
extern double FDECL retdouble(void *valptr);
#endif	/* REQUIRES_WORD_ALIGNMENT */


