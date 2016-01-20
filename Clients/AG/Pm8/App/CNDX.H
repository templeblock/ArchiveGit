/*  cndx.h - cindex header file
 *   ---------------------------------------------------------
 *  |                                                         |
 *  |  C-INDEX/II                                             |
 *  |  Version 5.0                                            |
 *  |  8/5/96                                                 |
 *  |                                                         |
 *  |  Trio Systems LLC                                       |
 *  |  936 E. Green St.  Suite 105                            |
 *  |  Pasadena, CA  91106                                    |
 *  |  818/584-9706                                           |
 *  |  818/584-0364 FAX                                       |
 *  |  mail@triosystems.com                                   |
 *  |                                                         |
 *  |  (c) Copyright 1983 - 1996                              |
 *  |                                                         |
 *   ---------------------------------------------------------
 */

#ifdef __cplusplus
extern "C"
{
#endif	/* __cplusplus */

/* this file must be included in all source modules that call C-Index */

/* MSDOS compiler specific include files */
#ifdef DOS

/* Microsoft C - DOS */
#ifdef MSC
#include "cndxmsc.h"
#define COMPDEFS					/* have good include file */
#endif	/* MSC */

/* Borland Turbo C */
#ifdef TC
#include "cndxtc.h"
#define COMPDEFS					/* have good include file */
#endif	/* TC */

#endif	/* endif DOS */


/* Phar Lap 386 Extended Dos ---------------------------------------- */
#ifdef PHARLAP

/* Metaware High C */
#ifdef HC
#include "cndxhc.h"
#define COMPDEFS					/* have good include file */
#endif	/* HC */

#endif	/* PHARLAP */


/* Metaware High C */
#ifdef HC
#include "cndxhc.h"
#define COMPDEFS					/* have good include file */
#endif	/* HC */


/* Microsoft Windows ------------------------------------------------ */
#ifdef WIN

#ifdef MSC						/* using MSC */
#include "cndxwin.h"
#define COMPDEFS					/* have good include file */
#endif	/* MSC */

#ifdef BOR						/* using Borland C++ */
#include "cndxwin.h"
#define COMPDEFS					/* have good include file */
#define TC
#define TCP
#endif	/* BOR */

#endif	/* endif WIN */

/* Microsoft Windows DLL ------------------------------------------------ */
#ifdef DLL

#ifdef BOR						/* using Borland C++ */
#include "cndxbc.h"
#define COMPDEFS					/* have good include file */
#define TC
#define TCP
#endif	/* BOR */

#ifdef VC						/* using microsoft visual c++ */
#include "cndxvc.h"
#define COMPDEFS					/* have good include file */
#endif	/* VC */

#endif	/* endif DLL */

/* Unix System V ---------------------------------------------------- */
#ifdef UNIX5
#include "cndxunix.h"			/* unix os and compiler */
#define COMPDEFS					/* have good include file */
#endif  /* endif UNIX5 */


/* Microsoft NT ---------------------------------------------------- */
#ifdef NT

#ifdef MSC							/* microsoft c compiler supplied with NT */
#include "cndxnt.h"				/* NT os and compiler */
#define COMPDEFS					/* have good include file */
#endif		/* endif MSC */

#ifdef BC							/* borland C++ 4.0 */
#include "cndxnt.h"				/* unix os and compiler */
#define COMPDEFS					/* have good include file */
#endif		/* endif MSC */

#endif  /* endif NT */



/* Macintosh ------------------------------------------------------- */
#ifdef MAC
#include "cndxmac.h"				/* mac specific header (MPW C) */
#define COMPDEFS					/* have good include file */
#endif		/* endif MAC */



/* ----------------------------------------------------------------------- */
/* --------- The following are for untested example ports. --------------- */

/* IBM 370 using IBM (Whitesmiths) C compiler ----------------------- */
#ifdef IBM370
#include "cndx9370.h"
#define COMPDEFS					/* have good include file */
#endif	/* endif IBM370 */


/* VAX/VMS ---------------------------------------------------------- */
#ifdef VMS
#include "cndxvms.h"				/* vax/vms using vaxc */
#define COMPDEFS					/* have good include file */
#endif  /* endif VAX */



/* OS/2 ------------------------------------------------------------- */
#ifdef OS2

/* Microsoft C - OS/2 */
#ifdef BOR
#include "cndxos2.h"
#define COMPDEFS					/* have good include file */
#endif	/* BOR */

#endif	/* endif OS/2 */


/* ----------------------------------------------------------------------- */

/* If no correct define for operating system and compiler were specified
 * you will get the "unable to locate <unknown.h>" error from the copiler.
 * Check your code to make sure that you have specified type defines
 * required.  For example, Microsoft C under DOS: -DMSC -DDOS.
 * See the C-Index/II documentation and the readme.doc file for more 
 * information.
 */
#ifndef COMPDEFS			/* if don't have good include file */
#include <unknown.h>		/* invalid flags specified for compilation */
#endif	/* !COMPDEFS */


#ifdef __cplusplus
}
#endif	/* __cplusplus */


/* always include code for deleting up index tree */
#define RSP

/*
 * In case compiler specific header does not have special declaration
 * of function calling convention type (pascal, fastcall, c, etc).
 */
#ifndef FDECL
#define FDECL			/* default to normal compiler function declaration */
#endif

/* for dll compilation, allow definition of exported function type */
#ifndef EDECL
#define EDECL
#endif



/*
 * In case of small ptr code calling a DLL
 * convert all pointer declarations to far
 * by defining PDECL in the compiler specific 
 * cndx???.h file. Otherwise, is defined as null.
 */
#ifndef PDECL
#define PDECL
#endif


/* size of objects */
#define NUMINDXS (MAXINDXS+2)	/* number of indices */
#define DATAAREA 256				/* max size of key + data */
#define MAXMKLEN (DATAPART)	/* maximum multi-user key length (for share byte) */
#define MAXKLEN (MAXMKLEN-2)	/* maximum key length */

/* maximum custom header info size */
#ifndef MAXCUSTINFO
#define MAXCUSTINFO 100
#endif

/* maximum path length - this may be defined by compiler specific headers */
#ifndef MAXNAMELEN
#define MAXNAMELEN	100			/* MAXNAMELEN is used for size of psp->filename */
#endif

/* default to one possible alternate root per index */
#ifndef MAXROOTS
#define MAXROOTS	MAXINDXS
#endif

/* default number of nodes to write when extending data file */
#ifndef MINDATAEXTEND
#define MINDATAEXTEND 10
#endif

/* default number of nodes to write when extending image file */
/* in this version, the extend only occurs when creating the file with imgcreate */
#ifndef MINIMGEXTEND
#define MINIMGEXTEND 50
#endif

/* function return codes */
#define KEYMATCH		 1		/* found key match but not rec match */
#define GREATER		 2		/* found next greater key - from greateq */ 
#define LESSTHAN		 3		/* found key less than - returned on lesseq */
#define FAIL			-1		/* failure on add */
#define PARMERR		-2		/* parameter error */
#define PSPERR			-3		/* bad psp structure */
#define DSKERR			-4		/* disk error */
#define BADINDEX		-5		/* bad header record */
#define WRITERR		-6		/* error in write of node */
#define SEEKERR		-7		/* error in lseek routine */
#define NOSPACE		-8		/* no more disk space */
#define READERR		-9		/* read error */
#define OLDERR			-10	/* build error on opening old file */
#define NEWERR			-11	/* error on copen of new file */
#define INCOMPLETE	-12	/* incomplete data record found */
#define BAD				-13	/* error found in ccheck */
#define BADDLIST		-14	/* bad datalist */
#define DATAERROR		-15	/* data read error after finding key */
#define NOCUR			-16	/* no current */
#define BADOLDDEL		-17	/* bad delete of old keys (update) */
#define DATAERR		-18	/* error reading or adding data portion */
#define BADDATDEL		-19	/* bad delete of old data */
#define CHKSUMERR		-20	/* check sums of dlist dont match */
#define CHKSUMDERR	-21	/* check sums of datarec dont match */
#define BADFLDTYP		-22	/* bad field type in datalist */
#define BADKEYTYP		-23	/* bad key type in datalist */
#define BADDUPFLG		-24	/* bad unq/dup flag in datalist */
#define BADFLDLEN		-25	/* bad field length in datalist */
#define BADSTRLEN		-26	/* string passed is longer than ->fldlen in datalist */
#define BADFLDINX		-27	/* bad field index number in datalist or indexlist */
#define BUFOVER		-28	/* overflow of buffer */
#define FILEXIST		-29	/* file existed on create */
#define BADNODE		-30	/* corrupted node */
#define BADHDR			-31	/* bad header information */
#define BADBYTEMODE	-32	/* invalid word order mode, bcreate or dbcreate */
#define BADWRITE		-33	/* attempted write to file opened as read only */
#define OUTOFMEM		-34	/* ran out of memory when allocating space */
#define BADINDTYP		-35	/* index type invalid or does not match header */
#define INDEXIST		-36	/* index entries already exist, cannot change type */
#define BADSEGLIST	-37	/* bad segment type, null list, missing terminator, etc */
#define CMPFAIL		-38	/* compression failed */
#define EXPFAIL		-39	/* expansion failed */
#define WRITEINTR		-40	/* write to file was interupted in security level 4 */
#define BADSECLEV		-41	/* invalid security level in effect for function (such as transbeg) */
#define PROCINTR		-42	/* processing was interupted by application */
#define INVALIDNDE	-43	/* invalid node number requested in diskread or diskwrte */
#define INVALIDOS		-44	/* invalid version of DOS, need WRTUPD */
#define BYTEMISMATCH	-45	/* input and output file byte orders do not match */
#define ERRSHAREDIND	-46	/* error in cigetrel or cisetrel, more than one index in btree */
#define INVNODEREAD	-47	/* request to read invalid node, probably corrupted header */

/* multi user errors */
#define FILENOTLOCKED -101		/* file not locked */
#define FILELOCKED	-102		/* file locked - internal error */
#define MODEERR		-103		/* open mode error */
#define RECSHLK		-104		/* locked by a shared process */
#define RECSFLK		-105		/* locked by self */
#define RECUNLK		-106		/* record unlocked */
#define RECNOLK		-107		/* record not locked before delete/change */
#define RSTERR			-108		/* error reseting the share byte after extend */
#define USRERR			-109		/* too many users logged onto file */
#define BADQUE			-110		/* problem maintaining write queue */

#define TRANSBUSY		-111		/* transaction processing already in use */
#define TRANSINTR		-112		/* transaction was interupted, needs rollback */
#define TRANSALLOC	-113		/* transaction empty, space is allocated for this process */
#define TRANSNULL		-114		/* no transaction active */
#define TRANSERR		-115		/* invalid transaction log entry */

/* additional misc errors */
#define CLRLOCKERR	-116		/* error clearing existing lock on record */

/* errors from corrupted tree structure */
#define BADHDRREAD	-200		/* failed reading header */
#define BADROOT		-201		/* error reading or tesing root */
#define BADNDEFREE	-202		/* invalid free space in node */
#define BADNDECOUNT	-203		/* invalid node entry count or corrupted keys */
#define BADNDESAME	-204		/* forward and rev ptrs are same */
#define BADNDEFWD		-205		/* invalid forward ptr */
#define BADNDEREV		-206		/* invalid reverse ptr */
#define BADNDEKEYLEN	-207		/* invalid key length */
#define BADNDENPTR	-208		/* invalid index node pointer */
#define BADNDEDOWN	-209		/* read down tree gets to wrong level */
#define BADNDESCAN	-210		/* error scanning node, invalid entry */
#define BADNDEREAD	-211		/* error reading node */
#define BADFREECHAIN	-212		/* invalid freechain node */
#define BADHDRINFO	-213		/* corrupted header */
#define BADNDEKEYSEQ	-214		/* key sequence is invalid */

/* misc internal errors - application will not see these errors */
#define BADKEYCMP		-301		/* invalid key type in cikeycmp */
#define BADSHARENUM	-302		/* sharenum has stray lock in file */
#define NODENOTINMEM	-303		/* speed read cannot proceed because node is not in memory */

/* last 5 indexes are reserved for use internally by C-Index */
#define MAXDINDX	(MAXINDXS - 5)		/* maximum multi-key index number */
#define TRANSINDX	(MAXINDXS - 4)		/* transaction log work index */
#define DICTINDX	(MAXINDXS - 3)		/* dictionary index (future use) */
#define DELINDX	(MAXINDXS - 2)		/* where deleted rec nums are */
#define DATAINDX 	(MAXINDXS - 1)		/* where data is stored */
#define SEMAINDEX	(MAXINDXS)			/* semaphore index for current locks */

#ifndef DATAROOT
#define DATAROOT	DATAINDX		/* alternate root number for data index */
#endif
#ifndef SEMAROOT
#define SEMAROOT	SEMAINDEX	/* alternate root number for semaphore index */
#endif

/* dfind types */
#define EQUAL 1
#define GREAT 2
#define GREATEQ 3
#define LESS 4
#define LESSEQ 5

/* dseq types */
#define NEXT 1
#define PREV 2
#define FIRST 3
#define LAST 4

#define CCOK 0		/* good return */

/* index types */
#define STRINGIND	0x00
#define NOCASEIND	0x01
#define BINARYIND	0x02
#define INTIND		0x03
#define SHORTIND	0x04
#define LONGIND	0x05
#define FLOATIND	0x06
#define DOUBLEIND	0x07
#define SEGIND		0x08
#define FIXIND		0x09
#define USHORTIND	0x0A
#define RES1IND	0x0B
#define RES2IND	0x0C
#define RES3IND	0x0D
#define RES4IND	0x0E
#define RES5IND	0x0F
#define CUST1IND	0x10
#define CUST2IND	0x11
#define CUST3IND	0x12
#define CUST4IND	0x13
#define CUST5IND	0x14
#define MAXINDTYP 0x14

#define INDTYPEMASK	0x1F				/* lower 5 bits are for key type */
#define ENDSEGLIST	INDTYPEMASK		/* keytype to terminate segment list */
#define ENDNDXLIST	0					/* keyn to terminate index list */
#define INDCOMPMASK	0x80				/* top bit indicates compression */
#define INDRESMASK	0x70				/* three index bits reserved */

#ifndef FIXINDLEN
#define FIXINDLEN		6					/* assume 6 byte fixed length */
#endif

/* multi-key field types */
#define STRINGFLD	'S'			/* null terminated string */
#define BINARYFLD	'B'			/* binary field (2 byte length) */
#define FIXFLD		'M'			/* fixed length binary */
#define INTFLD		'I'			/* integer (16 bit or 32 bit) */
#define SHORTFLD	'H'			/* 16 bit signed integer */
#define USHORTFLD	'U'			/* 16 bit unsigned integer */
#define LONGFLD	'L'			/* 32 bit signed integer */
#define FLOATFLD	'F'			/* float */
#define DOUBLEFLD	'D'			/* double */
#define SEGFLD		'X'			/* segmented key field */
#define ENDLIST	'E'			/* data list terminator */
#define DELETEDFLD	(0x80)	/* flag indicating field has been deleted */

/* these definitions are included for backwards compatibility
 * and may need to be disabled with the NEW_FLDTYPES	flag
 * for compilers and/or other add-on libraries that conflict.
 */
#ifndef NEW_FLDTYPES
#define OK CCOK
#define STRING STRINGFLD
#define BINARY BINARYFLD
#define INT INTFLD
#define SHORTINT SHORTFLD
#define LONG LONGFLD
#define FLOAT FLOATFLD
#define DOUBLE DOUBLEFLD
#define FIELD DATALIST
#endif


/* key types (not covered in field types) */
#define NONKEY 'N'					/* non key (data item) */
#define DUPKEY 'D'					/* duplicate */
#define UNQKEY 'U'					/* unique */

/* multi user open types */
#define SHARED		'S'				/* share file mode, anyone else can write */
#define DENYWRITE	'D'				/* share file mode, no one else can write */
#define EXCL		'E'				/* exclusive mode, no one else can access */

/* transaction begin codes */
#define TRANSALL			1			/* log transaction changes and mirror file */
#define TRANSMIRROR		2			/* mirror file only */
#define TRANSCHANGE		3			/* log transaction changes, no mirror */

/* transaction end codes */
#define TRANSDONE			1			/* end and free existing active transaction */
#define TRANSCLEAR		2			/* end and clear existing trans without free */
#define TRANSBACK			3			/* put transaction on backup log */

/* transaction type codes */
#define TRANSADD			1			/* add key transaction type code */
#define TRANSDEL			2			/* delete key transaction type code */
#define TRANSEND			3			/* end of transaction */
#define TRANSSTRTWRIT	4			/* start write */
#define TRANSENDWRIT		5			/* end write */
#define TRANSNDEWRIT		6			/* node write (backup or mirror) */
#define TRANSUSR	  		128		/* base number for user defined transaction types */

/* information for transaction processing */
/* node entry structure */
typedef struct nodeent
{
	unsigned char entlen;			/* total length of entry */
#ifdef SIMULATED_FIELDS				/* the following fields are simulated */
	unsigned char indnum;			/* index number for this entry */
	unsigned char databyte;			/* data byte */
	long recnum;						/* record number */
	char keydata[KEYSZE - 7];		/* key/data value */
#endif
} ENTRY;

/* transaction entry structure */
typedef struct transentinfo
{
	INT32	transnum;				/* this transaction number */
	INT16 sequence;				/* sequence in this series of transactions */
	INT32 updatnum;				/* update number from header at time of trans */
	unsigned char  transtype;	/* TRANSADD, TRANSDEL */
	ENTRY transent;				/* transaction entry value */
} TRANSENT;

/* key segment information list */
typedef struct _KEYSEGLIST
{
	/* keytype required for single-key and multi-key segmented keys */
	unsigned char keytype;		/* type of key stored in the index */

	/* fldoff and maxlen required for multi-key segmented keys */
	int fldoff;						/* datalist offset of field storing key value */
	unsigned char maxlen;		/* max key field length (optional, or 0) */

	/* order for sorting this key segment */
	unsigned char descending;	/* 0 = ascending order, 1 = descending order */

	/* reserved for future use */
	void PDECL *reservedinfo;			/* info list - reserved for future use */
} KEYSEGLIST;


/* index information list structure */
typedef struct _NDXLIST
{
	unsigned char keyn;			/* index number */
	unsigned char keytype;		/* type of key stored in the index */
	KEYSEGLIST PDECL *segptr;	/* segment pointer */
} NDXLIST;


/* byte flipping modes for mbcreate */
#define INTELMODE			1			/* store info using intel format */
#define NONINTELMODE		2			/* store info using non-intel format */
#define NATIVEMODE		3			/* store info using native format */

/* read mode */
#define CRDONLY			1			/* read only file open */
#define CRDWRITE			2			/* read/write file open */

/* misc. */
#define KEYSZE 256					/* 256 byte keys (including data) */

/* external variables accessable by all modules */
extern int bcbnum;				/* number of buffers */

/* node size info */
#ifdef COMPRESSION
#define NDEHDRSIZE ((4 * sizeof(NDEPTR)) + (4 * sizeof(INT16)))
#else
#define NDEHDRSIZE ((3 * sizeof(NDEPTR)) + (3 * sizeof(INT16)))
#endif	/* COMPRESSION */
#define KEYAREA (NODESZE-NDEHDRSIZE)	/* size of key area in node */
#define MAXENTRIES ((KEYAREA * MAXCOMP) / 7)	/* max # keys in a node possible */
#define SPLITLMT ((KEYAREA * MAXCOMP) / 2)  	/* limit for split */
#define ENDLMT (KEYAREA * MAXCOMP)			  	/* limit for split if last node */
#define COMPAREA	(KEYAREA - sizeof(INT16))	/* compressed key area size */

#ifndef CI_BIGBUF
#define CI_BIGBUF (NODESZE + KEYSZE + 1)		/* big buffer size for handling overflow due to insert */
#endif


/* node declaration used for work buffers */
struct node
{
	NDEPTR p0;				/* p0 pointer */
	NDEPTR forwrd;			/* forward pointer */
	NDEPTR revrse;			/* reverse pointer */
#ifdef COMPRESSION
	NDEPTR compover;		/* compression overflow node */
#endif	/* COMPRESSION */
	INT16 level;			/* level of node */
	INT16 count;			/* key count */
	INT16 free;				/* free space */
#ifdef COMPRESSION
	INT16 compfree;				/* free space after compression */
#endif	/* COMPRESSION */
	char keystr[CI_BIGBUF];	/* area for keys */

	struct passparm *npsp;	/* psp for this node */
	FILHND nfd;					/* file handle (descriptor) for this node */
	NDEPTR nde;					/* node number of contents of this work buffer */
}; 

struct smallnode
{	/* bcb node declaration */
	NDEPTR smp0;		/* p0 pointer */
	NDEPTR smforwrd;	/* forward pointer */
	NDEPTR smrevrse;	/* reverse pointer */
#ifdef COMPRESSION
	NDEPTR compover;		/* compression overflow node */
#endif	/* COMPRESSION */
	INT16 smlevel;		/* level of node */
	INT16 smcount;		/* key count */
	INT16 smfree;		/* free space */
#ifdef COMPRESSION
	INT16 compfree;				/* free space after compression */
#endif	/* COMPRESSION */
	char smkeystr[KEYAREA];	/* area for keys */
}; 

struct imgbcb
{			/* buffers for read and write of nodes */
	void *custinfo1;				/* space for custom info #1 */
	FILHND custinfo2;				/* space for custom info #2 */
	NDEPTR bnde;					/* node number - DO NOT MODIFY */	
	int custinfo3;					/* space for custom info #3 */
	ALLOCHND custinfo4;			/* space for custom info #4 */
	long bupdatnum;				/* last update number - DO NOT MODIFY */
#ifdef COMPRESSION
	struct node bbuffer;			/* big buffer for node */
#else
	struct smallnode bbuffer;	/* buffer for node */
#endif	/* COMPRESSION */
};

typedef struct curinfo
{
	char keystr[MAXMKLEN];		/* key value */
	long currcval;					/* current record number */
	char curdbyte;					/* current data byte - used by low */
	int  keynum;					/* current key number */
	NDEPTR ndenum; 				/* current node number */
	int curindx;					/* current index */
	char delflag;					/* was a delete just performed ? */
	char updkeyinfo;				/* TRUE == need to update EXCL key info */
	KEYSEGLIST PDECL *segptr;	/* ptr to key segment info */
} CURINFO;

#ifdef MIXED_MODEL_BUFFERS
typedef CURINFO far *CURINFOPTR;			/* far ptr to cur key info */
#else
typedef CURINFO PDECL *CURINFOPTR;		/* normal ptr to cur key info */
#endif

#ifdef ANSI		/* some non-ansi compilers will not allow this statement */
typedef void PDECL *KEYPTR;				/* pointer to a key value */
#else
typedef char PDECL *KEYPTR;				/* pointer to a key value */
#endif

struct flddef 
{
	char fldtype;		/* type of field */  
	char keytype;		/* key type */
	char dupflag;		/* unique or duplicate */ 
	short fldlen;		/* length of field CHANGED: 7/2/87 (was int) */
	char fldindex;		/* index number */
	void PDECL *fldptr;  
};

typedef struct flddef DATALIST;
typedef struct flddef PDECL *PDATALIST;

/* status callback function typedefs */
#ifdef ANSI
#ifdef __cplusplus
extern "C"
{
#endif	/* __cplusplus */

typedef void (EDECL *STATPROCTYPE)(struct passparm PDECL *, long);
typedef int  (EDECL *LOGPROCTYPE)(struct passparm PDECL *, double, TRANSENT *);
typedef int  (EDECL *NEXTPROCTYPE)(struct passparm PDECL *, int, void PDECL *, int);
typedef PDATALIST (EDECL *BUILDPROCTYPE)(struct passparm PDECL *, int, long);
typedef int (EDECL *IMGPROCTYPE)(struct passparm PDECL *psp, struct imgbcb *bcbptr);
#ifdef __cplusplus
}
#endif	/* __cplusplus */

#else
typedef void (EDECL *STATPROCTYPE)();
typedef int  (EDECL *LOGPROCTYPE)();
typedef int  (EDECL *NEXTPROCTYPE)();
typedef PDATALIST (EDECL *BUILDPROCTYPE)();
typedef int (EDECL *IMGPROCTYPE)();
#endif	/* ANSI */


/* file parameter information structure */
/* This structure is used to keep track of information about an open file */
struct passparm
{
	/* first check storage */
	char check1;		/* will say 'V' */
	char check2;		/* will say 'A' */

	/* parameter variables - seen by user */
	int keyn;			/* key number */
	KEYPTR key;			/* pointer to key */
	long rec;			/* record number */
	void PDECL *data;	/* pointer to data record */
	int dlen;			/* len of data */
	int relpercent;	/* relative percentage location of key (after cigetrel) */

	char status;		/* status byte - either 'O' open or 'C' closed */

	int retlen;			/* length of data found */
	int retcde;			/* return code */
	int errcde;			/* error code */
	int memerr;			/* memory error flag */

	/* second check storage */
	char check3;		/* will say 'L' */

	/* file header information variables - whats on disk */	
	INT32 wordorder;	/* order of bytes in machine word (set to 1) */
	NDEPTR singleroot;	/* root node pointer */

	char check4;		/* will say 'I' */

	/* more information from header */
	int singlelevels;			/* number of levels in tree */
	NDEPTR next;		/* next available node */
	long nxtdrec;		/* next record for data file */
	NDEPTR endofile;	/* end of file */
	NDEPTR freenodes;	/* free chain list of empty nodes */
	long delcnt;		/* count of deleted record numbers in file */
	long reccnt;		/* count of records in file */
	NDEPTR transchain;	/* transaction log top of chain */
	NDEPTR transback;		/* ptr to transaction log backup chain */
	int custlen;							/* custom info length */
	char custinfo[MAXCUSTINFO];		/* custom header information */
	unsigned char indtyp[NUMINDXS];	/* index type for each index */
	KEYSEGLIST PDECL *segptr[NUMINDXS];		/* ptr to list of seg info for each index */
	unsigned char rootnum[NUMINDXS];			/* number of root node */
	unsigned char levelslist[MAXROOTS];		/* list of multiple root levels */
	NDEPTR rootlist[MAXROOTS];					/* list of multiple root ptrs */

	/* third check storage */
	char check5;		/* will say 'D' */

	/* --------------------------------------------------------------------*/
	/* -------- user settable flags controlling internal operation --------*/
	/* --------------------------------------------------------------------*/

	int checkflg;		/* FALSE == don't perform check of datalist */
	int fulldel;		/* TRUE == delete up tree when node empty */
	int reuse;			/* TRUE == reuse record numbers */
	int nolock;			/* TRUE == don't require locking before update */
	int peekmsg;		/* TRUE == do peek message in MS Windows code */
	INT32 setdrec;		/* if != -1, force next dadd() record number to this */
	int writeque;		/* TRUE == use write queue method of locking file during write */
	int lazywrite;		/* TRUE == sec lev 4 write caching until any write */
	int alterr;			/* alternate error code (for additional error information) */
	int fliprep;		/* TRUE == byteflip keys passed to cnextrep callback to machine format */

#ifndef NO_BLOCK_EXTEND
	long minextend;	/* minimum number of nodes to extend when enlarging file */
#endif	/* !NO_BLOCK_EXTEND */


	/* Macintosh File Information */
#ifdef MAC
	long dirID;							/* directory ID */
	short vRefNum;						/* volume reference number */
	OSType creator;					/* file creator */
	OSType fileType;					/* file type */
	long seekpos;						/* desired seek position (from fileSeek) */
#endif	/* MAC */

	/* pointer to compiler specific information, for custom use in porting */
	void PDECL *portinfo;
	unsigned long portlong;

	/* --------------------------------------------------------------------*/
	/* -------- user settable callback routines for controlling operation -*/
	/* --------------------------------------------------------------------*/

#ifdef IMAGEBACK

	IMGPROCTYPE imagebackproc;		/* called by bcbwrite imageback */
	IMGPROCTYPE imagerestproc;		/* called by bcbread during imagerest */
#endif


	/* --------------------------------------------------------------------*/
	/* -------- do not modify any of the following variables --------------*/
	/* ----------------- internally used by cindex ------------------------*/	
	/* --------------------------------------------------------------------*/

	FILHND fd;			/* file descriptor */
	int updtflag;		/* tells if header has been modified */
	int srchorder;		/* tells key compare to treat test key as larger */
	int curroot;		/* current root number (from multiple roots) */

	char check6;	/* will say 'A' */

	int dbyte;						/* data byte */
	char retkey[DATAAREA];		/* area for key and data */
	int treeindex;					/* index number that path applies to */
	int nextkeyn;					/* keyn of entries in next node to right */
	NDEPTR path[20];				/* path of tree search */
	int keycmpres;					/* result of last key comparison */
	int strcmpres;					/* result of last key string comparison */

	char opstat;					/* status of current operation */
	char filename[MAXNAMELEN];	/* filename */
	int seclev;						/* security level */
	int extflag;					/* extend or not (for doing upddisk) */
	char PDECL *dbufptr;			/* pointer to data record buffer */
	int dbuflen;					/* data buffer length */ 
	char idbyte;					/* id for data record */
	long currec;					/* current record number */
	long lockrec;					/* currently locked record number */

	long pupdatnum;				/* last update number of header */
	int sharenum;					/* share number of psp */


	char processmode;					/* shared or exclusive use - controls processing */
	char readmode;					/* CRDONLY or CRDWRITE,read-only or read/write */
	char filestat;					/* LOCKED or UNLOCKED */
	char filemode;					/* EXCL or SHARED file open - only controls open/close */
	char skipupdread;				/* TRUE = skip reading the update number */
/* #ifdef READSHARE */
	char readstat;					/* type of current readbyte lock */
/* #endif */

	/* for managing speed read feature */
	char speedread;				/* TRUE == speed read is on */
	int speedahead;				/* number of buffers to read ahead */
	int speedreq;					/* read ahead requested by read operation */
	struct smallnode *speednod;	/* current speed read node being processed */
	ENTRY *speedentptr;			/* entptr for speed read next/prev processing */

	char readcnt;					/* nesting of high level ops doing read */
	char writcnt;					/* nesting of high level ops doing write */
	int writeactive;				/* write has occured since strtwrit */
	int writeflag;					/* TRUE == WRTBYTE set (writing to disk in Level 4+) */
	int srecurs;					/* recursion flag, 1= in sema routines */
	INT32 keyorder;					/* byte mode of key pointed to by psp->key */

	/* variables needed by delete tree subroutines */
	NDEPTR lowernde;				/* node number of lower node (being removed) */
	NDEPTR uppernde;				/* read up node number */
	int entries;					/* number of active entries in node */
	NDEPTR fwdptr;					/* forward node ptr for patching */
	NDEPTR revptr;					/* reverse node ptr for patching */
	int curlevel;					/* current node level (saved before setfree) */
	NDEPTR highkeynde;			/* high key node pointer */
	int chghighkey;				/* TRUE == have changed high key of lower node */
	char highkeybuf[KEYSZE];	/* buffer to save changed high key value */

	/* set by bcheck function */
	long badnode;					/* index node in error */
	int badtyp;						/* bad node error type */

/* #ifdef TRANSACTION */
	/* used by transaction logging */
	int transop;						/* transaction logging type */
	NDEPTR begtransnde;				/* first transaction log node in chain */
	NDEPTR curtransnde;				/* current transaction log node in use */
	unsigned char * transndeptr;	/* current pointer into nodet */
	NDEPTR strttransnde;				/* strtwrit transaction log node */
	unsigned char * strttransndeptr;	/* strtwrit pointer into nodet */
	INT32 transnum;					/* current transaction number */
	INT16 transseq;					/* transaction log sequence number */
	struct passparm *transpsp;		/* psp of transaction mirror file */
/* #endif	*/

/* #ifdef IMAGEBACK */
	struct passparm *imagepsp;		/* psp of image backup file */
	NDEPTR backnde;					/* backup node counter */
	NDEPTR backlast;					/* value of backnde at the last time did upddisk */
/* #endif */

	/* write lock queue processing */
	int endque;			/* last position in queue on previous access */
	int curquepos;		/* current position in lock queue */

	char check7;	/* 'T' */

	char curlock[NUMINDXS];					/* current index lock status */
	CURINFOLST curinfoptr[NUMINDXS];		/* ptr to current key info */

	char check8;	/* 'E' */
};

typedef struct passparm CFILE;			/* c-index file parameter structure */

#define dsetfile(a, b)  csetfile(a, b)
#define dflush(a)	cflush(a)

#define LASTFIELD ENDLIST, ENDLIST, 0, 0, 0, ""	  /* last field in declaration */


/* defines to maintain function name compatibility with Lattice 3.0 */
#define dcreate(psp, fle, buf, buflen)	dbcreate(psp, fle, buf, buflen, EXCL, INTELMODE, NULL)
#define dopen(psp, fle, buf, buflen)	dbopen(psp, fle, buf, buflen, EXCL, CRDWRITE, NULL)
#define dclose(psp)							cidclose(psp)
#define dfind(a, b, c, d, e)				cidfind(a, b, c, d, e)
#define dread(a, b)							cidread(a, b)

/* defines to maintain backwards version compatibility */
#define mcreate(psp, file, filemode)	bcreate(psp, file, filemode, INTELMODE, NULL)
#define mopen(psp, file, filemode)		bopen(psp, file, filemode, CRDWRITE, NULL)
#define cclose(psp)							mclose(psp)
#define dmcreate(psp, file, buf, len, filemode)	dbcreate(psp, file, buf, len, filemode, INTELMODE, NULL)
#define dmopen(psp, file, buf, len, filemode)	dbopen(psp, file, buf, len, filemode, CRDWRITE, NULL)


/* function prototypes */

#ifdef ANSI
#ifdef __cplusplus
extern "C"
{
#endif	/* __cplusplus */
extern  int EDECL ciunlink(char PDECL *name);				/* delete file */
extern  int EDECL copen(struct passparm PDECL *psp,char PDECL *fle);
extern  int EDECL ccreate(struct passparm PDECL *psp,char PDECL *file);
extern  int EDECL cflush(struct passparm PDECL *psp);
extern  int EDECL csetfile(struct passparm PDECL *psp,int level);
extern  int EDECL cunqadd(struct passparm PDECL *psp,int keyn,KEYPTR key,long rec,void PDECL *data,int dlen);
extern  int EDECL cdupadd(struct passparm PDECL *psp,int keyn,KEYPTR key,long rec,void PDECL *data,int dlen);
extern  int EDECL cchange(struct passparm PDECL *psp,int keyn,KEYPTR key,long rec,void PDECL *data,int dlen);
extern  int EDECL csave(struct passparm PDECL *psp,int keyn,KEYPTR key,long rec,void PDECL *data,int dlen);
extern  int EDECL cfind(struct passparm PDECL *psp,int keyn,KEYPTR key,long rec,void PDECL *data,int dlen);
extern  int EDECL cdelete(struct passparm PDECL *psp,int keyn,KEYPTR key,long rec);
extern  int EDECL cnext(struct passparm PDECL *psp,int keyn,void PDECL *data,int dlen);
extern  int EDECL cnextrep(struct passparm PDECL *psp,int keyn,void PDECL *data,int dlen, NEXTPROCTYPE nextproc);
extern  int EDECL cnextrep2(struct passparm PDECL *psp,int keyn,void PDECL *data,int dlen, NEXTPROCTYPE nextproc);
extern  int EDECL cprev(struct passparm PDECL *psp,int keyn,void PDECL *data,int dlen);
extern  int EDECL cfirst(struct passparm PDECL *psp,int keyn,void PDECL *data,int dlen);
extern  int EDECL clast(struct passparm PDECL *psp,int keyn,void PDECL *data,int dlen);
extern  int EDECL cgetcur(struct passparm PDECL *psp,int keyn,void PDECL *data,int dlen);
extern  int EDECL cdelcur(struct passparm PDECL *psp,int keyn);
extern  int EDECL cchgcur(struct passparm PDECL *psp,int keyn,void PDECL *data,int dlen);
extern  int EDECL cnextrec(struct passparm PDECL *psp);
extern  int EDECL csetrec(struct passparm PDECL *psp,long recnum);
extern  int EDECL cgetcnt(struct passparm PDECL *psp);
extern  int EDECL dgetcnt(struct passparm PDECL *psp);
extern  int EDECL csetcnt(struct passparm PDECL *psp, long newcnt);
extern  int EDECL cgetcust(struct passparm PDECL *psp, void PDECL *data, int datalen);
extern  int EDECL csetcust(struct passparm PDECL *psp, void PDECL *data, int datalen);
extern  int EDECL cbuild(char PDECL *oldname,char PDECL *newname, NDXLIST PDECL *indextypes);
extern  int EDECL bbuild(char PDECL *oldname, CFILE PDECL *newpsp, int oldbytemode);
extern  int EDECL _bbuild(char PDECL *oldname, CFILE PDECL *newpsp, int oldbytemode, STATPROCTYPE statproc);
extern  int EDECL dbuild(CFILE PDECL *, CFILE PDECL *, char *, int, struct flddef PDECL *, BUILDPROCTYPE);
extern  int EDECL dcheck(CFILE PDECL *, char *, int, struct flddef PDECL *, BUILDPROCTYPE);
extern  int EDECL msetsema(struct passparm PDECL *psp,KEYPTR key,long rec,int shareval);
extern  int EDECL mclrsema(struct passparm PDECL *psp,KEYPTR key,long rec,int shareval);
extern  int EDECL mtstsema(struct passparm PDECL *psp,KEYPTR key,long rec,int shareval);
extern  int EDECL msetlock(struct passparm PDECL *psp,int keyn);
extern  int EDECL mclrlock(struct passparm PDECL *psp,int keyn);
extern  int EDECL mtstlock(struct passparm PDECL *psp,int keyn);
extern  int EDECL strtread(struct passparm PDECL *psp);
extern  int EDECL endread(struct passparm PDECL *psp);
extern  int EDECL strtwrit(struct passparm PDECL *psp);
extern  int EDECL endwrit(struct passparm PDECL *psp);
extern  int EDECL bopen(struct passparm PDECL *psp,char PDECL *fle,int filemode,int rdmode, NDXLIST PDECL *indexinfo);
extern  int EDECL _bopen(struct passparm PDECL *psp,char PDECL *fle,int filemode,int rdmode);
extern  int EDECL bcreate(struct passparm PDECL *psp,char PDECL *file,int filemode,int bytemode, NDXLIST PDECL *indexinfo);
extern  int EDECL cchgndx(struct passparm PDECL *psp,NDXLIST PDECL *indexinfo);
extern  int EDECL mclose(struct passparm PDECL *psp);
extern  int EDECL imgcreate(struct passparm PDECL *psp,char PDECL *fle,int filemode);
extern  int EDECL imgopen(struct passparm PDECL *psp,char PDECL *fle,int filemode,int rdmode);
extern  int EDECL imgclose(struct passparm PDECL *psp);
extern  int EDECL dbopen(struct passparm PDECL *psp,char PDECL *fle,void PDECL *buf,int buflen,int filemode,int rdmode, NDXLIST PDECL *indexinfo);
extern  int EDECL dbcreate(struct passparm PDECL *psp,char PDECL *fle,void PDECL *buf,int buflen,int filemode,int wordorder, NDXLIST PDECL *indexinfo);
extern  int EDECL cidclose(struct passparm PDECL *psp);
extern  int EDECL dmclose(struct passparm PDECL *psp);
extern  int EDECL addroot(struct passparm PDECL *psp, int keyn, int rootnum);
extern  int EDECL droproot(struct passparm PDECL *psp, unsigned int rootnum);
extern  int EDECL dadd(struct passparm PDECL *psp,struct flddef PDECL *datalist);
extern  int EDECL cidfind(struct passparm PDECL *psp,int index,KEYPTR key,int keytype,int type);
extern  int EDECL dsetkey(struct passparm PDECL *psp,int index,struct flddef PDECL *datalist,int fieldnum,KEYPTR key);
extern  int EDECL cidread(struct passparm PDECL *psp,struct flddef PDECL *datalist);
extern  int EDECL ddelete(struct passparm PDECL *psp,struct flddef PDECL *datalist);
extern  int EDECL dupdate(struct passparm PDECL *psp,struct flddef PDECL *datalist);
extern  int EDECL dseq(struct passparm PDECL *psp,int index,int type);
extern  int EDECL dgetid(struct passparm PDECL *psp);
extern  int EDECL msetrec(struct passparm PDECL *psp);
extern  int EDECL mclrrec(struct passparm PDECL *psp);
extern  int EDECL mtstrec(struct passparm PDECL *psp);
extern  int EDECL ccheck(char PDECL *chkfile,char PDECL *errfile);
extern  int EDECL _ccheck(char PDECL *chkfile,char PDECL *errfile, NDXLIST PDECL *indexlist, STATPROCTYPE statproc);
extern  int EDECL bcheck(struct passparm PDECL *psp);
extern  int EDECL _bcheck(struct passparm PDECL *psp, STATPROCTYPE statproc);
extern  int EDECL clrwrit(struct passparm PDECL *psp);
extern  int EDECL cconvert(char PDECL *old, char PDECL *newfile);
extern  int EDECL endbcb(struct passparm PDECL *psp);
extern  int EDECL cibufnum(int cnt);
extern  int EDECL transbeg(struct passparm PDECL *psp, int operation, struct passparm PDECL *mirrorpsp);
extern  int EDECL transend(struct passparm PDECL *psp, int endtype);
extern  int EDECL transroll(struct passparm PDECL *psp, LOGPROCTYPE statproc, int rolltransop, struct passparm PDECL *mirrorpsp);
extern  int EDECL transtst(struct passparm PDECL *psp);
extern  int EDECL translog(struct passparm PDECL *psp, unsigned int transtype, char *entptr);
extern  int EDECL imageback(struct passparm PDECL *psp, struct passparm PDECL *backpsp);
extern  int EDECL imagerest(struct passparm PDECL *psp, struct passparm PDECL *backpsp);
extern  int EDECL cipspsize(int size);
extern  int EDECL cigetrel(struct passparm PDECL *psp, int keyn);
extern  int EDECL cisetrel(struct passparm PDECL *psp, int keyn, int percent);
#ifdef __cplusplus
}
#endif	/* __cplusplus */

#else			/* else old K&R prototypes */
extern  int EDECL ciunlink();
extern  int EDECL copen();
extern  int EDECL ccreate();
extern  int EDECL cflush();
extern  int EDECL csetfile();
extern  int EDECL cunqadd();
extern  int EDECL cdupadd();
extern  int EDECL cchange();
extern  int EDECL csave();
extern  int EDECL cfind();
extern  int EDECL cdelete();
extern  int EDECL cnext();
extern  int EDECL cnextrep();
extern  int EDECL cnextrep2();
extern  int EDECL cprev();
extern  int EDECL cfirst();
extern  int EDECL clast();
extern  int EDECL cgetcur();
extern  int EDECL cdelcur();
extern  int EDECL cchgcur();
extern  int EDECL cnextrec();
extern  int EDECL csetrec();
extern  int EDECL cgetcnt();
extern  int EDECL dgetcnt();
extern  int EDECL csetcnt();
extern  int EDECL cgetcust();
extern  int EDECL csetcust();
extern  int EDECL cbuild();
extern  int EDECL _cbuild();
extern  int EDECL bbuild();
extern  int EDECL _bbuild();
extern  int EDECL dbuild();
extern  int EDECL dcheck();
extern  int EDECL msetsema();
extern  int EDECL mclrsema();
extern  int EDECL mtstsema();
extern  int EDECL msetlock();
extern  int EDECL mclrlock();
extern  int EDECL mtstlock();
extern  int EDECL strtread();
extern  int EDECL endread();
extern  int EDECL strtwrit();
extern  int EDECL endwrit();
extern  int EDECL bopen();
extern  int EDECL _bopen();
extern  int EDECL bcreate();
extern  int EDECL cchgndx();
extern  int EDECL mclose();
extern  int EDECL imgcreate();
extern  int EDECL imgopen();
extern  int EDECL imgclose();
extern  int EDECL dbopen();
extern  int EDECL dbcreate();
extern  int EDECL cidclose();
extern  int EDECL dmclose();
extern  int EDECL addroot();
extern  int EDECL droproot();
extern  int EDECL dadd();
extern  int EDECL cidfind();
extern  int EDECL dsetkey();
extern  int EDECL cidread();
extern  int EDECL ddelete();
extern  int EDECL dupdate();
extern  int EDECL dseq();
extern  int EDECL dgetid();
extern  int EDECL msetrec();
extern  int EDECL mclrrec();
extern  int EDECL mtstrec();
extern  int EDECL ccheck();
extern  int EDECL _ccheck();
extern  int EDECL bcheck();
extern  int EDECL _bcheck();
extern  int EDECL clrwrit();
extern  int EDECL cconvert();
extern  int EDECL endbcb();
extern  int EDECL cibufnum();
extern  int EDECL transbeg();
extern  int EDECL transend();
extern  int EDECL transroll();
extern  int EDECL transtst();
extern  int EDECL translog();
extern  int EDECL imageback();
extern  int EDECL imagerest();
extern  int EDECL cipspsize();
extern  int EDECL cigetrel();
extern  int EDECL cisetrel();
#endif		/* ANSI */

