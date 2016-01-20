/*  cndxcvc.h - visual c dll header
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




/* include files */
#ifndef NO_INCLUDES				/* includes needed for library compilation */
#include <windows.h>				/** MS Windows version **/
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>
#include <malloc.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\locking.h>
#include <direct.h>
#include <errno.h>
#include <string.h>
#endif	/* !NO_INCLUDES */

#define WIN3						/* for windows 3.0 */
#define ANSI						/* ANSI compatible compiler */
#define NULLFD   -1				/* null file descriptor */
#define INTELFMT	(1)			/* intel machine == 1, non-intel = 0 */

/*
 *   This flag indicates if integers are represented as 16 bit
 *   values (same as short).  Comment out this define if
 *   int is 32 bytes (same as long).
 */

#define INTISSHORT


/*
 * For DLL, turn on PASCAL type calls to FAR proceedures
 */
#define FDECL FAR PASCAL

/*
 * For DLL, force API functions to be exported
 */
#define EDECL FAR PASCAL __export

/*
 * For DLL, make all ptrs into far ptrs
 */
#define PDECL	far


/* compiler specific typdefs */
typedef int FILHND;							/* file handle for disk operations */

typedef short INT16;							/* 16 bit signed integers */
#if _MSC_VER < 1200	// GCB 11/9/98 - VC6 typedefs INT32 as int
	typedef long  INT32;							/* 32 bit signed integers */
#endif
typedef long NDEPTR;							/* 32 bit node numbers */

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


/*
 * Enable this define to include the transaction logging features
 */

#define TRANSACTION


/*
 * Enable this define to include the image backup feature
 */

#define IMAGEBACK


/*
 * Enable this define to compile with concurrent reading feature
 */

#define READSHARE

#ifdef READSHARE
#define NEWLOWLOCK					/* use compiler dependent low lock */
#endif	/* READSHARE */

/*
 * define this for the compressing version of the DLL
 */

/* #define COMPRESSION */

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



/* misc. size definitions */
#define NODESZE 1024							/* size of node */
#define DATAPART 244							/* size of data chunks */
#define MAXFLDLEN (128 * DATAPART)		/* max single-key data rec size */
#define MAXDLEN MAXFLDLEN					/* max multi-key data rec size */
#define MAXINDXS 20							/* max number of indices */

/* in-line macros to define block move and clear functions */
#define blockmv(des, src, len)	memcpy(des, src, (unsigned)len)
#define cclear(des, len, val)		memset(des, val, (unsigned)len)
#define cmovert(p, len, farval)	memmove(p+farval, p, (unsigned)len)
#define cmovelt(p, len, farval)	memmove(p-farval, p, (unsigned)len)

/* modify order of bytes in the node pointer number */
/* modify order of bytes in the node pointer number */
#ifndef NO_WORD_CONVERSION
#define isintel(wordorder)	(*((char *)&wordorder) == 1)
#define inteltol(lint, wordorder)	((	(INTELFMT && (*((char *)&wordorder) == 1)) \
			|| (!INTELFMT && !(*((char *)&wordorder) == 1)) ) \
			? lint \
			: finteltol(lint, wordorder))
#define intelton(lint, order) inteltol(lint, order)/* intel node ptr to native */
#define ntointel(lint, order) ltointel(lint, order)/* native node ptr to intel */
#else		/* redefine to return same */
#define isintel(wordorder)	(1)
#define inteltol(lint, wordorder)	lint
#define intelton(lint, order) lint
#define ntointel(lint, order)	lint
#endif	/* NO_WORD_CONVERSION */

/* transfer macros for machines that DO NOT require word boundary alignment */
#define trnsrec(des, src) *(INT32 *) (des) = *(INT32 *) (src);		/* transfers a data record number */
#define trnsnde(des, src) *(NDEPTR *) (des) = *(NDEPTR *) (src);	/* transfers an index node pointer */
#define trnsshort(des, src) *(INT16 *) (des) = *(INT16 *) (src);	/* transfers a 16 bit integer */
#define trnsint(des, src) *(int *) (des) = *(int *) (src);			/* transfers a machine size integer */
#define trnslong(des, src) *(INT32 *) (des) = *(INT32 *) (src);	/* transfers a long */

#ifdef NO_WORD_CONVERSION
#define trnsfloat(des, src) *(float *) (des) = *(float *) (src);	/* transfers a float */
#define trnsdbl(des, src) *(double *) (des) = *(double *) (src);	/* transfers a double */
#else
#define trnsfloat(des, src) blockmv((char *) des, (char *) src, sizeof(float))/* transfers a float */
#define trnsdbl(des, src) blockmv((char *) des, (char *) src, sizeof(double))	/* transfers a double */
#endif	/* NO_WORD_CONVERSION */



/* ------- memory management macros for current key information ------ */
#define tstcuradr(psp, keyn)	(psp->curinfoptr[keyn])
#define lockcuradr(psp, keyn)	((psp->curinfoptr[keyn]) \
											? (CURINFOPTR)psp->curinfoptr[keyn] \
											: xlockcuradr(psp, keyn))
/* do not use these macros for moveable memory */
#ifndef MEMORY_MANAGEMENT_REQUIRED
#define getcurrcval(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (INT32)(((CURINFOPTR)(psp->curinfoptr[keyn]))->currcval) \
												: fgetrcval(psp, keyn))

#define getcurdbyte(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFOPTR)(psp->curinfoptr[keyn]))->curdbyte) \
												: fgetdbyte(psp, keyn))

#define getkeynum(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFOPTR)(psp->curinfoptr[keyn]))->keynum) \
												: fgetkeynum(psp, keyn))

#define getndenum(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (NDEPTR)(((CURINFOPTR)(psp->curinfoptr[keyn]))->ndenum) \
												: fgetndenum(psp, keyn))

#define getcurindx(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFOPTR)(psp->curinfoptr[keyn]))->curindx) \
												: fgetindx(psp, keyn))

#define getdelflag(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFOPTR)(psp->curinfoptr[keyn]))->delflag) \
												: fgetdelflag(psp, keyn))

#define getupdkeyinfo(psp, keyn)	((psp->curinfoptr[keyn]) \
												? (int)(((CURINFOPTR)(psp->curinfoptr[keyn]))->updkeyinfo) \
												: fgetupdkeyinfo(psp, keyn))

#define putcurrcval(psp, keyn, val)	if(psp->curinfoptr[keyn]) \
												 ((CURINFOPTR)(psp->curinfoptr[keyn]))->currcval = (INT32)val; \
												else \
												 fputrcval(psp, keyn, val);

#define putcurdbyte(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFOPTR)(psp->curinfoptr[keyn]))->curdbyte = (char)val; \
												else \
												 fputdbyte(psp, keyn, val);

#define putkeynum(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFOPTR)(psp->curinfoptr[keyn]))->keynum = val; \
												else \
												 fputkeynum(psp, keyn, val);

#define putndenum(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFOPTR)(psp->curinfoptr[keyn]))->ndenum = val; \
												else \
												 fputndenum(psp, keyn, val);

#define putcurindx(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFOPTR)(psp->curinfoptr[keyn]))->curindx = val; \
												else \
												 fputindx(psp, keyn, val);

#define putdelflag(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFOPTR)(psp->curinfoptr[keyn]))->delflag = (char)val; \
												else \
												 fputdelflag(psp, keyn, val);

#define putupdkeyinfo(psp, keyn, val)	if (psp->curinfoptr[keyn]) \
												 ((CURINFOPTR)(psp->curinfoptr[keyn]))->updkeyinfo = (char)val; \
												else \
												 fputupdkeyinfo(psp, keyn, val);
#endif	/* !MEMORY_MANAGEMENT_REQUIRED */

/* this do nothing macros and defines replace the function calls needed
 * by systems that use memory management like Windows and Macintosh
 */
#define unlockcuradr(psp, keyn)			/* do nothing macro */

#ifdef MEMORY_MANAGEMENT_REQUIRED		/* use this typedef for Window or Mac */
typedef Handle ALLOCHND;					/* allocated space handle */
typedef Handle CURINFOLST;					/* list of handles to cur key info */
#else
#ifdef MIXED_MODEL_BUFFERS
typedef char far * ALLOCHND;				/* allocated far space pointer */
#else
typedef char PDECL * ALLOCHND;			/* allocated space pointer */
#endif	/* MIXED_MODEL_BUFFERS */
typedef void PDECL *CURINFOLST;			/* list of ptrs to cur key info */
#define cilockbuf(psp, adr) ((BCBPTR)(adr))	/* dummy lock buffer routine */
#define ciunlockbuf(psp, adr)				/* dummy unlock buffer routine */
#endif	/* MEMORY_MANAGEMENT_REQUIRED */


/* move short and int numbers to and from unaligned memory locations */
/* for machines that do not require alignment, use assignment */

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

#ifdef NO_WORD_CONVERSION
#define movfloat(dstadr, srcadr)		*((float *) dstadr) = *((float *) srcadr)
#define movdouble(dstadr, srcadr)	*((double *) dstadr) = *((double *) srcadr)
#else
#define movfloat(dstadr, srcadr)		memcpy(dstadr, srcadr, sizeof(float))
#define movdouble(dstadr, srcadr)	memcpy(dstadr, srcadr, sizeof(double))
#endif	/* NO_WORD_CONVERSION */

#define retint(srcadr)					(*((int *) srcadr))
#define retshort(srcadr)				(*((short *) srcadr))
#define retlong(srcadr)					(*((long *) srcadr))

#ifdef NO_WORD_CONVERSION
#define retfloat(srcadr)				(*((float *) srcadr))
#define retdouble(srcadr)				(*((double *) srcadr))
#endif	/* NO_WORD_CONVERSION */



#ifdef COMPRESSION
extern int cishrink(struct passparm *psp, struct node *dstnode, struct node *srcnode);
extern int ciexpand(struct passparm *psp, struct node *dstnode, struct node *srcnode);
extern int allocCompBuf(void);
extern int deallocCompBuf(void);
extern unsigned far pascal ReadBuff(char far *buff, unsigned short int far *size);
extern void far pascal WriteBuff(char far *buff, unsigned short int far *size);
#endif	/* COMPRESSION */


