/*  cindexwk.h - internal header
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

/* defines required to compile source (may also be set by makefile) */
#ifndef COMPB
#define COMPB
#endif

#ifndef COMPCB
#define COMPCB
#endif

#ifndef COMPS
#define COMPS
#endif

#ifndef COMPM
#define COMPM
#endif

#ifndef CCHECK
#define CCHECK
#endif

#ifndef DCHECK
#define DCHECK
#endif

#ifdef DOMEMCHECK
#include <memcheck.h>
#endif

/* internal flags */

/* general use */
#define TRUE 1
#define FALSE 0

#ifdef REMOVED
#define NODEBIAS (1234567L)			/* test bias added to node number */
#endif
#define NODEBIAS (0L)					/* test bias added to node number */

/* for different kinds of adds */

#define CHANGE 1	/* change existing key */
#define SAVE 2		/* add/change key - which ever is appropriate */ 
#define UNQ 3		/* add unique data record */
#define DUP 4		/* add duplicate record */
#define CHGCUR 5	/* change existing current key */

/* status of cindex flags */
#define ADDJST 1		/* ciadjust() flag - means just added to node */
#define DELJST 2		/* ciadjust() flag - means just deleted */
#define DELON 1		/* means current pointer is back one due to delete */
#define DELOFF 0		/* means current pointer is ok */
#define BEGOFNDE -1	/* means current  pointer is before keys in node */
#define FIRSTKEY -1	/* force search for first key in index */
#define LASTKEY  1	/* force search for last key in index */

/* buffering (bcb) defines */
#define NULLNDE  -1		/* null node number */
#define NULLNEXT NULL		/* null next pointer */
#define CLEAN	0		/* clean buffer - does not need disk write */
#define DIRTY	1		/* dirty bcb buffer - needs write to disk */

/* transaction log defines */
#ifdef TRANSACTION
#define TRANS1NDE	-2			/* starting transaction node (level #) */
#define TRANS2NDE	-3			/* continuation transaction node (level #) */
#endif	/* TRANSACTION */

/* miscellaneous */

#define NEWNDE 1	/* new node - used in nodewrite */
#define OLDNDE 0	/* old node - used in nodewrite */

/* sizes of objects */
/* node entry components */
#define SZELEN sizeof(unsigned char)	/* size of entry length */
#define SZENDX sizeof(unsigned char)	/* size of index number */
#define SZEDBYTE sizeof(unsigned char)	/* size of data byte */
#define SZEREC sizeof(INT32)  			/* record number long value */
#define SZENDE sizeof(NDEPTR)  			/* node pointer int value */

#ifndef HDRSZE
#define HDRSZE 1024							/* size of header */
#endif

#define HDRAREA sizeof(struct hdrrec)	/* amount to read into buf */

#ifndef MAXCOMP
#define MAXCOMP 1								/* default to no compression multiplier */
#endif

#ifndef COMPMARGIN
#define COMPMARGIN 258	/* keep min # bytes left in node when compressing */
								/* 256 + sizeof(INT16) */
#endif

#define DEFLEV   3							/* open set to 3 */
#define DEFLEVMU 1							/* open multi-user set to 1 */
#define HDRUPLEV 1							/* sec lev to update header */

#define DEMOLIMIT 50							/* file size of 50k on demo version */


/* macros */

#define logicval(a)	((a) > 0 ? 1 : -1)	/* for function returns */
#define FLDTYPE(a)	((a) & 0x7F)				/* strip off deleted field flag */
#define DELFLD(a)		(((a) & DELETEDFLD) != 0)	/* extract deleted field flag */

/* structure definitions */

#ifndef offsetof			/* make sure offsetof is defined */
#define offsetof(s,m)	(size_t)&(((s *)0)->m)
#endif

/* cannot do fast treesrch if have mixed model buffers or movable buffers */
#ifndef NO_FAST_TREESRCH
#ifdef MIXED_MODEL_BUFFERS
#define NO_FAST_TREESRCH
#else
#ifdef MEMORY_MANAGEMENT_REQUIRED
#define NO_FAST_TREESRCH
#endif
#endif
#endif	/* !NO_FAST_TREESRCH */

/* buffer control block - includes buffer info and node buffer */
/* if changes are made to bcb, must also be changed in bcbhead and imgbcb */
struct bcb
{			/* buffers for read and write of nodes */
	struct passparm PDECL *bpsp;		/* ptr to passparm for this file */
	FILHND bfd;						/* file descriptor for this file */
	NDEPTR bnde;					/* node number */	
	int bstatus;					/* dirty or clean */
	ALLOCHND bnext;				/* pointer to next buffer in chain */
	long bupdatnum;				/* last update number of this buffer */
#ifdef COMPRESSION
	struct node bbuffer;			/* big buffer for node */
#else
	struct smallnode bbuffer;	/* buffer for node */
#endif	/* COMPRESSION */
	/* extra zeros for searching directly from disk buffers 
	   and clearing end of image backup file */
	char endbuf[30];
};

struct bcbhead
{
	struct passparm PDECL *bpsp;		/* ptr to passparm for this file */
	FILHND bfd;						/* file descriptor for this file */
	NDEPTR bnde;					/* node number */	
	int bstatus;					/* dirty or clean */
	ALLOCHND bnext;				/* pointer to next buffer in chain */
	long bupdatnum;				/* last update number of this buffer */
};

struct hdrrec
{		/* header record on disk */
	INT32 updatnum;		/* last update number */
	INT32 ck1;				/* 'V' */
	INT32 hwordorder;		/* order of bytes in machine word (set to 1) */
	NDEPTR hsingleroot;			/* root node pointer */
	INT32 ck2;				/* 'A' */
	INT32 ck3;				/* 'L' */
	INT32 ck4;				/* 'I' */
	INT32 hsinglelevels;			/* number of levels in tree */
	INT32 ck5;				/* 'D' */
	NDEPTR hnext;			/* next available node */
	INT32 ck6;				/* 'A' */
	INT32 hnxtdrec;		/* next data record number */
	INT32 ck7;				/* 'T' */
	NDEPTR hendofile;		/* actual end of file */
	INT32 ck8;				/* 'E' */
	NDEPTR hfreenodes;	/* free chain list of empty nodes */
	INT32 hdelcnt;			/* deleted record count */
	INT32 hreccnt;			/* active record count */
	INT32 hcustlen;		/* custom info length */
	INT32 hres1;							/* reserved 1 */
	INT32 hres2;							/* reserved 2 */
	INT32 hres3;							/* reserved 3 */
	INT32 hres4;							/* reserved 4 */
	char hcustinfo[MAXCUSTINFO];		/* custom header information */
	char hindtyp[NUMINDXS];				/* type of each index */
#ifndef PRE41								/* early pre-release compatibility switch */
	INT32 hres5;							/* reserved 5 */
#endif
	unsigned char hrootnum[NUMINDXS];		/* root to use for this index */
	unsigned char hlevelslist[MAXROOTS];	/* list of multiple root levels */
	NDEPTR hrootlist[MAXROOTS];				/* list of multiple root pointers */
	NDEPTR htranschain;							/* ptr to transaction log chain */
	NDEPTR htransback;							/* ptr to transaction log backup chain */
};



/* ------- defines to control buffer access ------- */
#ifdef MIXED_MODEL_BUFFERS
typedef struct bcb far *BCBPTR;
#else
typedef struct bcb PDECL *BCBPTR;
#endif	/* MIXED_MODEL_BUFFERS */


/* conversion utility macros and structures */

/* advance pointer to next key in node */
#define convmoveup(p) (p + (((int) *p) & 0xFF) + 1)	

#define convtrnsrec(des, src) blockmv((char *) des, (char *) src, SZEREC)
  /* transfers a data record number */

#define CONVKEYAREA 1012						/* size of key area in node */
#define CONVSZEDBYTE 1							/* size of data byte */
#define CONVSZEREC sizeof (INT32)  			/* record number long value */

/* header record on disk, C-Index/Plus format */
struct oldhdrrec
{
	char ck1;
	INT16 hroot;	/* root node pointer */
	char ck2;
	INT16 hfirst;	/* first leaf node pointer */
	char ck3;
	INT16 hlast;	/* last leaf node pointer */ 
	char ck4;
	INT16 hlevels;	/* number of levels in tree */
	char ck5;
	INT16 hnext;	/* next available node */
	char ck6;
	long hnxtdrec;	/* next data record number */
	char ck7;
	INT16 hendofile;	/* actual end of file */
	char ck8;	
	long updatnum;		/* last update number */
};

/* C-Index/Plus node declaration */
struct oldnode
{
	INT16 level;			/* level of node */
	INT16 count;			/* key count */
	INT16 free;				/* free space */
	INT16 p0;				/* p0 pointer */
	INT16 forwrd;			/* forward pointer */
	INT16 revrse;			/* reverse pointer */
	char keystr[CONVKEYAREA];	/* area for keys */
}; 




/* ------------------- global variables -------------------- */
extern int errcde;				/* global error code */
extern struct node nodea;		/* node to do all operations in */
extern struct node nodeb;		/* node b in split */
extern struct node ebuffer;	/* for use by extend */
extern struct node nodet;		/* node buffer for transaction logging */
extern struct node noder;		/* node buffer for transaction rollback */
#ifdef COMPRESSION
extern struct node nodec;		/* node to compress into */
#endif
extern NDEPTR path[20];			/* path of tree search */
extern int bcbinitflag;			/* 1 == buffers have be initialized */


/* needed for high level words */

#define MAXLEVELS 4	/* level 4 is highest level of write "security" */

#define INVALID 'I'	/* for validation of psp */
#define PSPOPEN 'O'	/* means file is open */
#define PSPCLOSED 'C'	/* means file is closed */

/* flags for __bopen */
#define NORMOPEN	'N'	/* normal __bopen operation (used by bopen) */
#define LOWOPEN	'L'	/* low level __bopen operation (used by _bopen) */

/* use macro definition of internal functions for speed */
#ifdef REQUIRES_WORD_ALIGNMENT
#define gtkrec(ptr, wordorder)	fgtkrec(ptr, wordorder)
#else
#define gtkrec(ptr, ord)	(inteltol( *((INT32 *)(((unsigned char *)ptr) \
													+ SZELEN + SZENDX + SZEDBYTE)), ord))
#endif	/* REQUIRES_WORD_ALIGNMENT for gtkrec */


/* use macro definition of internal functions for speed */
#define MACRO_FUNCS

#ifdef MACRO_FUNCS
#define moveup(p)			(p + (((int) *p) & 0xFF) + 1)
#define gtentlen(ptr)	((ptr) ? (ptr)->entlen : 0)
#define gtkeyn(ptr)		((!(ptr)) ? 0 : (*(((unsigned char *)(ptr)) + SZELEN) & 0xFF))
#define gtdbyte(ptr)		((*(((unsigned char *)(ptr)) + SZELEN + SZENDX)) & 0xFF)


#define gtkeyptr(ptr)	((!((ENTRY *)ptr)->entlen) \
									? (unsigned char *)ptr \
									: ((unsigned char *)ptr) \
										+ SZELEN + SZENDX + SZEDBYTE + SZEREC)

#define goodbufs(psp)	((psp->processmode == EXCL) ? CCOK : fgoodbufs(psp))

#else			/* else use function definition of internal functions */
#define moveup(p)						fmoveup(p)
#define gtentlen(ptr)				fgtentlen(ptr)
#define gtkeyn(ptr)					fgtkeyn(ptr)
#define gtdbyte(ptr)					fgtdbyte(ptr)
#define gtkeyptr(ptr)				fgtkeyptr(ptr)
#define goodbufs(psp)				fgoodbufs(psp)
#endif	/* MACRO_FUNCS */

#define goodindex(i)     			(i >= 1 && i <= MAXINDXS)						/* one less */
#define gooddlen(psp, key, dlen)	((dlen >=0) && ((dlen / (DATAPART - (cikeylen(psp, psp->keyn, key))) <= 128)))   /* max data len */
#define goodklen(psp, s)			(cikeylen(psp, psp->keyn, s) <= MAXKLEN)	/* one less for '\0' */
#define goodlevel(lev)     		(lev >= 0 && lev <= MAXLEVELS)				/* good security level */

#ifdef REMOVED
#define goodat(data, len)   		((((unsigned char *)data)+len) >= (unsigned char *)data)		/* no overflow on address */
#endif

/* multi user defines */

#define WRTBYTE	(HDRSZE - 14)	/* flag byte in header indicating strtwrit in progress (usually byte 1010) */
#define UPDBYTE	(HDRSZE - 4)	/* flag byte in header indicating upddisk in progress (usually byte 1020) */
#define USRBYTE	(HDRSZE - 24)	/* 10 bytes (1000 - 1009) for user flags (usually byte 1000) */

#define EXCLBYTE  0			/* exclusive file use lock byte */

/*
 * MAP OF HEADER REGION:
 *
 *     EXCLBYTE   - exclusive file lock byte - overlaps hdrrec (1st byte)
 *     hdrrec		- first part of file contains header info
 *     SHAREOFF	- share byte lock region (1 byte for each user)
 *     READOFF		- concurrent read lock region (1 byte for each user)
 *     FLOCKBYTE	- write queue lock region (1 byte for each user)
 *     ROOTOFF		- separate root lock region (1 byte for each root)
 *
 */

#define SHAREOFF	(HDRAREA + 1)	/* offset for share bytes in header record */
#define MAXUSERS	((USRBYTE - MAXROOTS - SHAREOFF) / 3)	/* # of possible users */
#define READOFF	(SHAREOFF + MAXUSERS)	/* read share byte offset in header */
#define FLOCKBYTE (READOFF + MAXUSERS)		/* write queue region */
#define ROOTOFF	(FLOCKBYTE + MAXUSERS)	/* root share byte offset in header */

#define SETUPD		13245		/* flag indicating upddisk or strtwrit in progress */
#define RESETUPD	0			/* flag reseting upddisk or strtwrit in progress byte */
#define UPDRETRY	5000		/* number of retries before giving up on upddisk */

#define OFF 'O'
#define LOCKED 'L'
#define UNLOCKED 'U'

#ifdef READSHARE
#define NOREADLOCK	1		/* no read bytes locked on file */
#define ONEREADLOCK	2		/* one read byte locked (reading only) */
#define ALLREADLOCK	3		/* all read bytes locked (writing file) */
#endif	/* READSHARE */


/* ------------------------------------------------------- */

#ifdef ANSI
#ifdef __cplusplus
extern "C"
{
#endif	/* __cplusplus */
/* compiler specific function prototypes */
extern  int FDECL initcindex(struct passparm PDECL *psp);	/* compiler specific system init */
extern  int FDECL osbflush(struct passparm PDECL *psp);	/* flush operating system buffers */
extern  int FDECL isnetfh(struct passparm PDECL *psp);	/* test if file is on remote disk */
extern  int FDECL upddisk(struct passparm PDECL *psp);	/* update FAT table on disk */

extern  int FDECL lockbyte(struct passparm PDECL *psp, int byte);	/* lock byte in file, blocking */
extern  int FDECL tlockbyte(struct passparm PDECL *psp, int byte);	/* lock byte, non-blocking*/
extern  int FDECL unlockbyte(struct passparm PDECL *psp, int byte);	/* unlock specific byte in file */
extern  int FDECL tstlockbyte(struct passparm PDECL *psp, int byte);/* test lock of specific byte */
extern  int FDECL testshrbyte(struct passparm PDECL *psp, int share);/* test share byte of file */
extern  int FDECL mtstos(void );

extern  FILHND FDECL mexlcreate(struct passparm PDECL *psp, char PDECL *name);/* create file, exclusive mode */
extern  FILHND FDECL mexlopen(struct passparm PDECL *psp, char PDECL *name,int rdmode);/* open file in exclusive mode */
extern  FILHND FDECL mshropen(struct passparm PDECL *psp, char PDECL *name,int rdmode);/* open file in shared mode */

extern  int FDECL mexlclose(struct passparm PDECL *psp);	/* close file, exclusive mode */
extern  int FDECL mshrclose(struct passparm PDECL *psp);	/* close a file, shared mode */

extern  int FDECL fileSeek(struct passparm PDECL *psp, long adr, int recsize, long offset);/* seek to adr */
extern  int FDECL ciread(struct passparm PDECL *psp, void PDECL *hdr, unsigned sze);		/* read block from file */
extern  int FDECL ciwrite(struct passparm PDECL *psp, void PDECL *hdr, unsigned sze);	/* write block to file */

extern  long FDECL updread(struct passparm PDECL *psp);	/* read update number */
extern  int FDECL hdrread(struct passparm PDECL *psp,struct hdrrec PDECL *buf);/* read hdr */
extern  int FDECL hdrwrte(struct passparm PDECL *psp,struct hdrrec PDECL *buf);/* write hdr */
extern  int FDECL diskread(struct passparm PDECL *psp,struct node PDECL *nod,NDEPTR nde);/* read node */
extern  int FDECL diskcompread(struct passparm PDECL *psp,struct node PDECL *nod,NDEPTR nde);/* read node */
extern  int FDECL diskwrte(struct passparm PDECL *psp,struct node PDECL *nod,NDEPTR nde);/* write node */
extern  int FDECL transwrte(struct passparm PDECL *psp,struct node PDECL *nod,NDEPTR nde);/* write transaction node */
extern  int FDECL transread(struct passparm PDECL *psp,struct node PDECL *nod,NDEPTR nde);/* read transaction node */

extern  int FDECL cikeytest(struct passparm PDECL *psp, ENTRY PDECL *ptr,int keyn,KEYPTR key,long rec,int dbyte,INT32 bytemode);
extern  int FDECL cikeycmp(struct passparm PDECL *psp, int keyn, int keytype, void PDECL *entptr, void PDECL *keyptr);
extern  void FDECL custincrkeyval(struct passparm PDECL *psp, int keytype, KEYPTR keyptr);

#ifndef NO_WORD_CONVERSION
extern void FDECL nodetonative(struct node PDECL *nod, INT32 wordorder);	/* convert node header to native format */
extern void FDECL nodetointel(struct node PDECL *nod, INT32 wordorder);	/* convert node header to intel format */
extern void FDECL hdrtonative(struct hdrrec PDECL *hdr, INT32 wordorder);/* convert file hdr to native */
extern void FDECL hdrtointel(struct hdrrec PDECL *hdr, INT32 wordorder);	/* convert file hdr to intel format */
extern short EDECL inteltos(int si, INT32 wordorder);					/* intel short to native format */
extern int   FDECL inteltoi(int si, INT32 wordorder);					/* intel integer to native format */
extern long  FDECL finteltol(long lint, INT32 wordorder);				/* intel long to native format */
extern short FDECL stointel(int si, INT32 wordorder);					/* native short to intel format */
extern long  FDECL ltointel(long lint, INT32 wordorder);				/* native long to intel format */
extern void  FDECL ftointel(float *valptr, INT32 wordorder);
extern void  FDECL inteltof(float *valptr, INT32 wordorder);
extern void  FDECL inteltod(double *dvalptr, INT32 wordorder);
extern void  FDECL dtointel(double *dvalptr, INT32 wordorder);
extern void  FDECL cikeyflip(CFILE PDECL *psp, int keyn, KEYPTR keyptr, int keytype, INT32 inbytemode, INT32 outbytemode);
extern void  EDECL cipspflip(CFILE PDECL *psp, INT32 outbytemode);
extern void  FDECL cicustflip(CFILE PDECL *psp, KEYPTR keyptr,int keytype, INT32 inbytemode, INT32 outbytemode);

extern void FDECL putfloat(void *dstadr, float srcvar);
extern void FDECL putdouble(void *dstadr, double srcvar);
extern float FDECL retfloat(void *valptr);
extern double FDECL retdouble(void *valptr);
#else			/* no word convertion function calls */
#define nodetonative(nod, wordorder)
#define nodetointel(nod, wordorder)
#define hdrtonative(hdr, wordorder)
#define hdrtointel(hdr, wordorder)
#define inteltos(si, wordorder) si
#define finteltol(lint, wordorder) lint
#define stointel(si, wordorder) si
#define ltointel(lint, wordorder) lint
#endif

#ifdef MIXED_MODEL_BUFFERS
void FDECL buftonear(struct node PDECL *nearbuf, BCBPTR farbuf, unsigned bufsize);
void FDECL buftofar(BCBPTR farbuf, struct node PDECL *nearbuf, unsigned bufsize);
#endif

extern  CURINFOPTR FDECL xlockcuradr(struct passparm PDECL *psp, int keyn);

/* ------------------------------------------------------- */

/* internal function prototypes */
extern  char PDECL * FDECL triocopyright(void );

extern int FDECL convisintel(void);
extern short FDECL cvintos(int si);
extern long FDECL cvintol(long lint);

extern  void FDECL clrbcb(struct passparm PDECL *psp);
extern  int FDECL clrtransbcb(struct passparm PDECL *psp, NDEPTR zapnde);
extern  int FDECL chkhdr(struct hdrrec PDECL *hdr);
extern  int FDECL initbcb(struct passparm PDECL *psp);
extern  int FDECL bcbwrite(struct passparm PDECL *psp,BCBPTR bcbptr);
extern  int FDECL bcbback(struct passparm PDECL *psp,BCBPTR bcbptr);
extern  int FDECL bcbread(struct passparm PDECL *psp,struct bcb *bcbptr, NDEPTR nde);
extern  int FDECL diskcompwrte(BCBPTR bcbptr, struct node *nod);
extern  int FDECL noderead(struct passparm PDECL *psp,struct node PDECL *nod,NDEPTR nde);
extern  struct smallnode PDECL * FDECL nodereadsp(struct passparm PDECL *psp,NDEPTR nde);
extern  int FDECL nodewrte(struct passparm PDECL *psp,struct node PDECL *nod,NDEPTR nde);
extern  void FDECL puttop(struct passparm PDECL *psp, ALLOCHND bcbptr);
extern  int FDECL locnode(struct passparm PDECL *psp,NDEPTR nde);
extern  int FDECL scanbuf(struct passparm PDECL *psp,NDEPTR nde);
extern  int FDECL bcbadd(struct node PDECL *nod,BCBPTR bcbptr,NDEPTR nde,struct passparm PDECL *psp);
extern  int FDECL clsbcb(struct passparm PDECL *psp);
extern  int FDECL bflush(struct passparm PDECL *psp);
extern  int EDECL __bopen(struct passparm PDECL *psp,char PDECL *fle,int filemode,int rdmode,int opentype);

#ifdef REQUIRES_WORD_ALIGNMENT
extern  long FDECL fgtkrec(ENTRY PDECL *ptr, INT32 wordorder);
#endif	/* REQUIRES_WORD_ALIGNMENT for gtkrec */

#ifndef MACRO_FUNCS
extern  char PDECL * FDECL fmoveup(char PDECL *ptr);
extern  int FDECL fgtentlen(ENTRY PDECL *ptr);
extern  int FDECL fgtkeyn(ENTRY PDECL *ptr);
extern  int FDECL fgtdbyte(ENTRY PDECL *ptr);
extern  char PDECL * FDECL fgtkeyptr(ENTRY PDECL *ptr);
#endif

extern  char PDECL * FDECL gtdataptr(struct passparm PDECL *psp, ENTRY PDECL *ptr);
extern  NDEPTR FDECL gtroot(struct passparm PDECL *psp, int keyn);
extern  int FDECL gtlevels(struct passparm PDECL *psp);
extern  void ptroot(struct passparm PDECL *psp, NDEPTR newroot);
extern  void ptlevels(struct passparm PDECL *psp, int levelcnt);

extern  int FDECL cikeylen(struct passparm PDECL *psp, int keyn, KEYPTR keyptr);
extern  int FDECL _cikeylen(int keytype, KEYPTR keyptr);
extern  void FDECL cikeycpy(struct passparm PDECL *psp, int keyn, KEYPTR dstptr, KEYPTR keyptr);
extern  void FDECL incrkeyval(struct passparm PDECL *psp, int keyn, int keytype, KEYPTR keyptr);
extern  NDEPTR FDECL gtnptr(struct passparm PDECL *psp, ENTRY PDECL *ptr);

extern  int FDECL goodata(struct passparm PDECL *psp, ENTRY PDECL *p);
extern  void FDECL cmovedata(ENTRY PDECL *p,struct passparm PDECL *psp);
extern  void FDECL bldikey(struct passparm PDECL *psp, ENTRY PDECL *hkey,NDEPTR nde,ENTRY PDECL *data, INT32 wordorder);
extern  void FDECL bldlkey(struct passparm PDECL *psp, ENTRY PDECL *inkey,int keyn,char PDECL *key,long rec,int dbyte,char PDECL *data,int dlen, INT32 wordorder);
extern  void FDECL setcurkey(struct passparm PDECL *psp, ENTRY PDECL *p);
extern  NDEPTR FDECL scaniptr(struct node PDECL *ndea,int keyn,char PDECL *key,long rec,int dbyte,struct passparm PDECL *psp, INT32 tstorder);
extern  ENTRY PDECL * FDECL scanikey(struct passparm PDECL *psp, struct node PDECL *ndea,NDEPTR leftnode);
extern  ENTRY PDECL * FDECL scandkey(struct passparm PDECL *psp, struct node PDECL *ndea,NDEPTR leftnode,int highflg);
extern  ENTRY PDECL * FDECL scanlnde(struct node PDECL *ndea,int keyn,char PDECL *key,long rec,int dbyte,int PDECL *cnt, struct passparm PDECL *psp, INT32 wordorder);
extern  void FDECL setnull(struct passparm PDECL *psp);
extern  int FDECL setparm(struct passparm PDECL *psp, ENTRY PDECL *p);
extern  ENTRY PDECL * FDECL treesrch(struct passparm PDECL *psp,int keyn,char PDECL *key,long rec,int dbyte);
extern  int FDECL treesrchsp(struct passparm PDECL *psp,int keyn,char PDECL *key,long rec,int dbyte);
extern  int FDECL cigetreltreesrch(struct passparm PDECL *psp,int keyn,char *key, long rec, int dbyte, int *percentptr);
extern  int FDECL ciscangetrelptr(struct node PDECL *ndea,int keyn,char PDECL *key,long rec,
				int dbyte,struct passparm PDECL *psp, INT32 tstorder, long *totptr, long *offptr, NDEPTR *ndeptr);
extern  int FDECL ciscangetrelnde(struct node PDECL *ndea,int keyn,
				char PDECL *key,long rec,int dbyte,int PDECL *cnt, 
				struct passparm PDECL *psp, INT32 wordorder, long *totptr, 
				long *offptr, ENTRY ** entptrptr);
extern  int FDECL ciscansetrelptr(struct node PDECL *ndea,int keyn,struct passparm PDECL *psp,
				long *totptr, long *offptr, NDEPTR *ndeptr,int percent);
extern  int FDECL ciscansetrelnde(struct node PDECL *ndea,int keyn,int PDECL *cnt, 
				struct passparm PDECL *psp, long *totptr, 
				long *offptr, ENTRY ** entptrptr,int percent);
extern  void FDECL setfile(struct passparm PDECL *psp,struct hdrrec PDECL *hdr);
extern  void FDECL sethdr(struct passparm PDECL *psp,struct hdrrec PDECL *hdr);
extern  char PDECL * FDECL findsplt(struct passparm PDECL *psp, struct node PDECL *ndea,int PDECL *cnt);
extern  void FDECL addata(ENTRY PDECL *data,struct node PDECL *ndea,ENTRY PDECL *p);
extern  NDEPTR FDECL gtnwnde(struct passparm PDECL *psp);
extern  int FDECL sethfree(struct passparm PDECL *psp,struct node PDECL *ndea,NDEPTR freende);
extern  ENTRY PDECL * FDECL creatnab(struct passparm PDECL *psp, struct node PDECL *ndea,struct node PDECL *ndeb);
extern  void FDECL creatrt(struct node PDECL *ndea);
extern  NDEPTR FDECL readup(struct passparm PDECL *psp,struct node PDECL *ndea);
extern  int FDECL hdrupdte(struct passparm PDECL *psp);
extern  int FDECL ciextend(struct passparm PDECL *psp);
extern  int FDECL dextend(struct passparm PDECL *psp, DATALIST *datalist);
extern  void FDECL splitadj(struct passparm PDECL *psp,NDEPTR ndea,NDEPTR ndeb,int cnt);
extern  void FDECL ciadjust(struct passparm PDECL *psp,int flag);
extern  int FDECL insrtkey(ENTRY PDECL *data,struct node PDECL *ndea,ENTRY PDECL *p,NDEPTR nde,struct passparm PDECL *psp,int knum);
extern  int FDECL insrtikey(ENTRY PDECL *data,struct node PDECL *ndea,ENTRY PDECL *entptr,NDEPTR nde,struct passparm PDECL *psp,int sflag);
extern  ENTRY PDECL * FDECL cigetkey(struct node PDECL *ndea,int cnt);
extern  ENTRY PDECL * FDECL next(struct passparm PDECL *psp,int keyn);
extern  int FDECL nextsp(struct passparm PDECL *psp,int keyn);
extern  ENTRY PDECL * FDECL prev(struct passparm PDECL *psp,int keyn);
extern  int FDECL prevsp(struct passparm PDECL *psp,int keyn);
extern  ENTRY PDECL * FDECL getnxtkey(struct passparm PDECL *psp,int keyn);
extern  int FDECL getnxtkeysp(struct passparm PDECL *psp,int keyn);
extern  ENTRY PDECL * FDECL getprev(struct passparm PDECL *psp,int keyn);
extern  int FDECL getprevsp(struct passparm PDECL *psp,int keyn);
extern  void FDECL kdel(ENTRY PDECL *p,struct node PDECL *ndea);
extern  int FDECL delnde(ENTRY PDECL *p,struct passparm PDECL *psp,int databyte);
extern  int FDECL kysdel(struct passparm PDECL *psp,ENTRY PDECL *p,NDEPTR nde);
extern  int FDECL chkinode(struct passparm PDECL *psp, struct node PDECL *ndea);
extern  int FDECL deltree(struct passparm PDECL *psp, NDEPTR nde);
extern  int FDECL patchleaf(struct passparm PDECL *psp);
extern  int FDECL deluptree(struct passparm PDECL *psp);
extern  int FDECL propuptree(struct passparm PDECL *psp);
extern  int FDECL patchinode(struct passparm PDECL *psp);
extern  void FDECL crtnwhdr(struct hdrrec PDECL *hdr);
extern  void FDECL crtnwrt(struct node PDECL *ndea);
extern  int FDECL findkey(struct passparm PDECL *psp);
extern  int FDECL findkeysp(struct passparm PDECL *psp);
extern  int FDECL addkey(struct passparm PDECL *psp,int flag);
extern  int FDECL delkey(struct passparm PDECL *psp);
extern  int FDECL cicurkey(struct passparm PDECL *psp);
extern  int FDECL cicurkeysp(struct passparm PDECL *psp);
extern  int FDECL delcur(struct passparm PDECL *psp);
extern  int FDECL nextkey(struct passparm PDECL *psp);
extern  int FDECL nextkeysp(struct passparm PDECL *psp);
extern  int FDECL prevkey(struct passparm PDECL *psp);
extern  int FDECL prevkeysp(struct passparm PDECL *psp);
extern  int FDECL checkpsp(struct passparm PDECL *psp);
extern  void FDECL setpsp(struct passparm PDECL *psp,int status);
extern  int FDECL cigetdata(struct passparm PDECL *psp,char PDECL *p,int len);
extern  int FDECL cigetdatasp(struct passparm PDECL *psp,char PDECL *p,int len);
extern  int FDECL __cadd(struct passparm PDECL *psp,int keyn,char PDECL *key,long rec,char PDECL *data,int dlen,int flag);
extern  int FDECL ___cadd(struct passparm PDECL *psp,int flag);

extern  int EDECL cidunqadd(struct passparm PDECL *psp,int keyn,KEYPTR key,long rec,void PDECL *data,int dlen);
extern  int FDECL _cunqadd(struct passparm PDECL *psp);
extern  int FDECL _cdupadd(struct passparm PDECL *psp);
extern  int FDECL _cchange(struct passparm PDECL *psp);
extern  int FDECL _csave(struct passparm PDECL *psp);
extern  int FDECL _cfind(struct passparm PDECL *psp);
extern  int FDECL __cfind(struct passparm PDECL *psp);
extern  int FDECL __cfindsp(struct passparm PDECL *psp);
extern  int FDECL _cdelete(struct passparm PDECL *psp);
extern  int FDECL __cdlt(struct passparm PDECL *psp);
extern  int FDECL _cnext(struct passparm PDECL *psp);
extern  int FDECL __cnext(struct passparm PDECL *psp);
extern  int FDECL __cnextsp(struct passparm PDECL *psp);
extern  int FDECL _cprev(struct passparm PDECL *psp);
extern  int FDECL __cprev(struct passparm PDECL *psp);
extern  int FDECL __cprevsp(struct passparm PDECL *psp);
extern  int FDECL _cfirst(struct passparm PDECL *psp);
extern  int FDECL _clast(struct passparm PDECL *psp);

extern  int FDECL cispeedon(struct passparm PDECL *psp, int readahead);
extern  int FDECL cispeedoff(struct passparm PDECL *psp);

extern  int FDECL initcurs(struct passparm PDECL *psp);
extern  int FDECL endcurs(struct passparm PDECL *psp);

#ifdef MEMORY_MANAGEMENT_REQUIRED
extern  void FDECL unlockcuradr(struct passparm PDECL *psp, int keyn);
#endif

extern  int FDECL _cgetcur(struct passparm PDECL *psp);
extern  int FDECL __cgetcur(struct passparm PDECL *psp);
extern  int FDECL __cgetcursp(struct passparm PDECL *psp);
extern  int FDECL _cdelcur(struct passparm PDECL *psp);
extern  int FDECL _cchgcur(struct passparm PDECL *psp);
extern  int FDECL _csetrec(struct passparm PDECL *psp);
extern  int FDECL addall(struct node PDECL *buf,struct passparm PDECL *newfile);
extern  int FDECL dobuild(FILHND oldfd,struct passparm PDECL *newfile, NDXLIST PDECL *indextypes);
extern  int FDECL setshrbyte(struct passparm PDECL *psp);
extern  int FDECL clrshrbyte(struct passparm PDECL *psp,int share);
extern  int FDECL mzapsema(struct passparm PDECL *psp);
extern  void FDECL mbldlock(struct passparm PDECL *psp,char PDECL *newkey,int keyn);
extern  int FDECL _mclrlck(struct passparm PDECL *psp,int keyn);
extern  int FDECL lowlock(struct passparm PDECL *psp);
extern  int FDECL lowunlock(struct passparm PDECL *psp);
extern  int FDECL waitlowlock(struct passparm PDECL *psp);
extern  int FDECL cireadlock(struct passparm PDECL *psp, int allflag);
extern  int FDECL cireadunlock(struct passparm PDECL *psp);
extern  int FDECL muflush(struct passparm PDECL *psp);
extern  int FDECL testbyte(struct passparm PDECL *psp, int byte);	/* test byte of file fd */
extern  int FDECL strtopen(struct passparm PDECL *psp);
extern  int FDECL endopen(struct passparm PDECL *psp);
extern  int FDECL sethdrinfo(struct passparm PDECL *psp);
extern  int FDECL mhdrupdte(struct passparm PDECL *psp);
extern  int FDECL fgoodbufs(struct passparm PDECL *psp);
extern  int FDECL strtbufs(struct passparm PDECL *psp);

extern  void FDECL putcurkey(struct passparm PDECL *psp, int keyn, char PDECL *srcadr);
extern  void FDECL getcurkey(struct passparm PDECL *psp, int keyn, char PDECL *dstadr);

extern  INT32 FDECL fgetrcval(struct passparm PDECL *psp, int keyn);
extern  int FDECL fgetdbyte(struct passparm PDECL *psp, int keyn);
extern  int FDECL fgetkeynum(struct passparm PDECL *psp, int keyn);
extern  NDEPTR FDECL fgetndenum(struct passparm PDECL *psp, int keyn);
extern  int FDECL fgetindx(struct passparm PDECL *psp, int keyn);
extern  int FDECL fgetdelflag(struct passparm PDECL *psp, int keyn);
extern  int FDECL fgetupdkeyinfo(struct passparm PDECL *psp, int keyn);
extern  int FDECL getcurlock(struct passparm PDECL *psp, int keyn);
extern  KEYSEGLIST PDECL * FDECL getcurseg(struct passparm PDECL *psp, int keyn);

extern  void FDECL cisavecurinfo(struct passparm PDECL *psp, int keyn, CURINFOPTR savbuf);
extern  void FDECL cirestcurinfo(struct passparm PDECL *psp, int keyn, CURINFOPTR savbuf);

extern  void FDECL fputrcval(struct passparm PDECL *psp, int keyn, INT32 value);
extern  void FDECL fputdbyte(struct passparm PDECL *psp, int keyn, int value);
extern  void FDECL fputkeynum(struct passparm PDECL *psp, int keyn, int num);
extern  void FDECL fputndenum(struct passparm PDECL *psp, int keyn, NDEPTR nde);
extern  void FDECL fputindx(struct passparm PDECL *psp, int keyn, int indx);
extern  void FDECL fputdelflag(struct passparm PDECL *psp, int keyn, int flag);
extern  void FDECL fputupdkeyinfo(struct passparm PDECL *psp, int keyn, int flag);
extern  void FDECL fputcurseg(struct passparm PDECL *psp, int keyn, KEYSEGLIST PDECL *segptr);
extern  void FDECL putcurlock(struct passparm PDECL *psp, int keyn, int flag);

extern  int FDECL wrtupd(struct passparm PDECL *psp,int flag);
extern  int FDECL readupd(struct passparm PDECL *psp);
extern  int FDECL readwrt(struct passparm PDECL *psp);
extern  long FDECL getnextrec(struct passparm PDECL *psp);
extern  int FDECL cputrec(struct passparm PDECL *psp,long rec);
extern  int FDECL crtsegkey(struct passparm PDECL *psp, char PDECL *buf, DATALIST PDECL *dlist, int segoff, unsigned char PDECL *binkey);
extern  int FDECL bldsegkey(struct passparm PDECL *psp, DATALIST PDECL *datalist, int fldnum, unsigned char PDECL *binkey);
extern  int FDECL addkeys(struct passparm PDECL *psp,char PDECL *buf,long rec,struct flddef PDECL *datalist);
extern  int FDECL delkeylist(struct passparm PDECL *psp,char PDECL *buf,long rec,struct flddef PDECL *datalist,int len);
extern  char PDECL * FDECL fieldtrans(struct passparm PDECL *psp, char PDECL *p,struct flddef PDECL *key);
extern  char PDECL * FDECL fldmoveup(struct passparm PDECL *psp, char PDECL *p,struct flddef PDECL *dptr);
extern  int FDECL chgdlist(struct passparm PDECL *psp, struct flddef PDECL *datalist, long rec);
extern  int FDECL rvchkeylist(struct passparm PDECL *psp, char PDECL *buf, long rec, struct flddef PDECL *datalist, int fnum);
extern  int FDECL chgkeylist(struct passparm PDECL *psp, char PDECL *buf, long rec, struct flddef PDECL *datalist, int len);
extern  int FDECL builddrec(struct passparm PDECL *psp, char PDECL *buf,struct flddef PDECL *datalist,int idbyte,int maxlen);
extern  int FDECL civerifyrec(struct passparm PDECL *psp, char PDECL *buf, DATALIST PDECL *datalist, int buflen);
extern  int FDECL putdrec(char PDECL *buf,struct flddef PDECL *datalist,struct passparm PDECL *psp,int len);
extern  int FDECL nulldlist(struct flddef PDECL *datalist);
extern  int FDECL checkdlist(struct flddef PDECL *datalist);
extern  int FDECL chksumdata(char PDECL *buf,int len);
extern  int FDECL tstchksumdata(char PDECL *buf,int len, short testval);
extern  int FDECL chkrwdlist(struct passparm PDECL *psp, struct flddef PDECL *datalist);
extern  int FDECL chkbuilddlist(struct passparm PDECL *psp, struct flddef PDECL *datalist);
extern  int FDECL chkrdlist(struct passparm PDECL *psp, struct flddef PDECL *datalist);
extern  int FDECL chkftype(int type);
extern  int FDECL chkktype(struct passparm PDECL *psp, int keyn, int type);
extern  int FDECL chkdflag(int flag);
extern  int FDECL chkflen(int type, int len);
extern  int FDECL isindextype(int type);
extern  int FDECL chkfindex(int index);
extern  int FDECL transundo(struct passparm PDECL *psp, TRANSENT *entptr);
extern  int FDECL transmirror(struct passparm PDECL *psp);

/* file conversion functions */
extern  int FDECL convhdrread(struct passparm PDECL *psp, struct oldhdrrec PDECL *buf);
extern  int FDECL convdiskread(struct passparm PDECL *psp, struct oldnode PDECL *nod, NDEPTR nde);
extern  long FDECL convgtkrec(char PDECL *ptr);
extern  int FDECL convgtdbyte(char PDECL *ptr);
extern  int FDECL convgoodata(char PDECL *p);
extern  int FDECL convall(struct oldnode PDECL *buf, struct passparm PDECL *newfile);
extern  int FDECL doconv(struct passparm PDECL *psp, struct passparm PDECL *newfile);

extern  ALLOCHND FDECL ciallocbuf(struct passparm PDECL *psp, int size);
extern  int FDECL cideallocbuf(struct passparm PDECL *psp, ALLOCHND handle);

#ifdef MEMORY_MANAGEMENT_REQUIRED		/* use this typedef for Window or Mac */
extern  BCBPTR FDECL cilockbuf(struct passparm PDECL *psp, ALLOCHND hnd);
extern  void FDECL ciunlockbuf(struct passparm PDECL *psp, ALLOCHND hnd);
#endif

extern  int FDECL bcheckhdr(struct passparm PDECL *psp);
extern  int FDECL bcheckroot(struct passparm PDECL *psp);
extern  int FDECL bchecknde(struct passparm PDECL *psp,struct node PDECL *nod,NDEPTR nde);
extern  int FDECL imgchecknde(struct passparm PDECL *psp,struct smallnode PDECL *nod,NDEPTR nde);
extern  int FDECL treecheck(struct passparm PDECL *psp, int keyn, char PDECL *key, long rec, int dbyte);
extern  ENTRY PDECL * FDECL bcheckscanlnde(struct node PDECL *ndea,int keyn,char PDECL *key,long rec,int dbyte,int PDECL *cnt, struct passparm PDECL *psp);
extern  int FDECL bcheckkeytest(struct passparm PDECL *psp, ENTRY PDECL *ptr, int keyn, void PDECL *key, long rec, int dbyte);
extern  int FDECL ccheckdelete(struct passparm PDECL *psp, ENTRY PDECL *startptr, NDEPTR startnde, ENTRY PDECL * PDECL *endptr, NDEPTR endnde);
extern  int FDECL cchecksavebad(struct passparm PDECL *psp, char PDECL *entptr, NDEPTR nde);
extern  int FDECL findkeylist(struct passparm PDECL *psp, char PDECL *buf, long rec, DATALIST PDECL *datalist, int len, char usedflags[NUMINDXS]);
extern  int FDECL civalidatedkey(struct passparm PDECL *psp, char PDECL *buf, int len, DATALIST PDECL *datalist, void *testkey, int testind);
#ifdef __cplusplus
}
#endif	/* __cplusplus */


#else		/* non-ANSI, define function returns with parameter type info */

/* compiler specific function prototypes */
extern  int FDECL initcindex();	/* compiler specific system init */
extern  int FDECL osbflush();	/* flush operating system buffers */
extern  int FDECL isnetfh();	/* test if file is on remote disk */
extern  int FDECL upddisk();	/* update FAT table on disk */

extern  int FDECL lockbyte();	/* lock byte in file, blocking */
extern  int FDECL tlockbyte();	/* lock byte, non-blocking*/
extern  int FDECL unlockbyte();	/* unlock specific byte in file */
extern  int FDECL tstlockbyte();/* test lock of specific byte */
extern  int FDECL testshrbyte();/* test share byte of file */
extern  int FDECL mtstos();

extern  FILHND FDECL mexlcreate();/* create file, exclusive mode */
extern  FILHND FDECL mexlopen();/* open file in exclusive mode */
extern  FILHND FDECL mshropen();/* open file in shared mode */

extern  int FDECL mexlclose();	/* close file, exclusive mode */
extern  int FDECL mshrclose();	/* close a file, shared mode */
extern  int FDECL fileSeek();/* seek to adr */
extern  int FDECL ciread();		/* read block from file */
extern  int FDECL ciwrite();	/* write block to file */

extern  long FDECL updread();	/* read update number */
extern  int FDECL hdrread();/* read hdr */
extern  int FDECL hdrwrte();/* write hdr */
extern  int FDECL diskread();/* read node */
extern  int FDECL diskcompread();/* read node */
extern  int FDECL diskwrte();/* write node */

extern  int FDECL cikeytest();
extern  int FDECL cikeycmp();
extern  void FDECL custincrkeyval();

#ifndef NO_WORD_CONVERSION
extern void FDECL nodetonative();			/* convert node header to native format */
extern void FDECL nodetointel();			/* convert node header to intel format */
extern void FDECL hdrtonative();			/* convert file hdr to native */
extern void FDECL hdrtointel();			/* convert file hdr to intel format */
extern short EDECL inteltos();				/* intel short to native format */
extern long FDECL finteltol();				/* intel long to native format */
extern short FDECL stointel();				/* native short to intel format */
extern long FDECL ltointel();				/* native long to intel format */
extern void FDECL ftointel();
extern void FDECL inteltof();
extern void FDECL inteltod();
extern void FDECL dtointel();
extern void FDECL cikeyflip();
extern void EDECL cipspflip();
extern void FDECL cicustflip();
extern void FDECL putfloat();
extern void FDECL putdouble();
extern float FDECL retfloat();
extern double FDECL retdouble();
#else			/* no word convertion function calls */
#define nodetonative(nod, wordorder)
#define nodetointel(nod, wordorder)
#define hdrtonative(hdr, wordorder)
#define hdrtointel(hdr, wordorder)
#define inteltos(si, wordorder) si
#define finteltol(lint, wordorder) lint
#define stointel(si, wordorder) si
#define ltointel(lint, wordorder) lint
#endif

#ifdef MIXED_MODEL_BUFFERS
void FDECL buftonear();
void FDECL buftofar();
#endif

extern  CURINFOPTR FDECL xlockcuradr();

/* ------------------------------------------------------- */

/* internal function prototypes */
extern  char * FDECL triocopyright();

extern int FDECL convisintel();
extern short FDECL cvintos();
extern long FDECL cvintol();

extern  void FDECL clrbcb();
extern  int FDECL clrtransbcb();
extern  int FDECL chkhdr();
extern  int FDECL initbcb();
extern  int FDECL endbcb();
extern  int FDECL bcbread();
extern  int FDECL bcbwrite();
extern  int FDECL diskcompwrte();
extern  int FDECL bcbback();
extern  int FDECL bcbread();
extern  int FDECL noderead();
extern  struct smallnode PDECL * FDECL nodereadsp();
extern  int FDECL nodewrte();
extern  void FDECL puttop();
extern  int FDECL locnode();
extern  int FDECL scanbuf();
extern  int FDECL bcbadd();
extern  int FDECL clsbcb();
extern  int FDECL bflush();
extern  int EDECL __bopen();

#ifdef REQUIRES_WORD_ALIGNMENT
extern  long FDECL fgtkrec();
#endif	/* REQUIRES_WORD_ALIGNMENT for gtkrec */

#ifndef MACRO_FUNCS
extern  char * FDECL fmoveup();
extern  int FDECL fgtentlen();
extern  int FDECL fgtkeyn();
extern  int FDECL fgtdbyte();
extern  char * FDECL fgtkeyptr();
#endif

extern  char * FDECL gtdataptr();
extern  NDEPTR FDECL gtroot();
extern  int FDECL gtlevels();
extern  void ptroot();
extern  void ptlevels();

extern  int FDECL cikeylen();
extern  int FDECL _cikeylen();
extern  void FDECL cikeycpy();
extern  void FDECL incrkeyval();
extern  NDEPTR FDECL gtnptr();

extern  int FDECL goodata();
extern  void FDECL cmovedata();
extern  void FDECL bldikey();
extern  void FDECL bldlkey();
extern  void FDECL setcurkey();
extern  NDEPTR FDECL scaniptr();
extern  ENTRY * FDECL scanikey();
extern  ENTRY * FDECL scandkey();
extern  ENTRY * FDECL scanlnde();
extern  void FDECL setnull();
extern  int FDECL setparm();
extern  ENTRY * FDECL treesrch();
extern  int FDECL treesrchsp();
extern  int FDECL cigetreltreesrch();
extern  int FDECL ciscangetrelptr();
extern  int FDECL ciscangetrelnde();
extern  int FDECL ciscansetrelptr();
extern  int FDECL ciscansetrelnde();
extern  void FDECL setfile();
extern  void FDECL sethdr();
extern  char * FDECL findsplt();
extern  void FDECL addata();
extern  NDEPTR FDECL gtnwnde();
extern  int FDECL sethfree();
extern  ENTRY * FDECL creatnab();
extern  void FDECL creatrt();
extern  NDEPTR FDECL readup();
extern  int FDECL hdrupdte();
extern  int FDECL ciextend();
extern  int FDECL dextend();
extern  void FDECL splitadj();
extern  void FDECL ciadjust();
extern  int FDECL insrtkey();
extern  int FDECL insrtikey();
extern  ENTRY * FDECL cigetkey();
extern  ENTRY * FDECL next();
extern  int FDECL nextsp();
extern  ENTRY * FDECL prev();
extern  int FDECL prevsp();
extern  ENTRY * FDECL getnxtkey();
extern  int FDECL getnxtkeysp();
extern  ENTRY * FDECL getprev();
extern  int FDECL getprevsp();
extern  void FDECL kdel();
extern  int FDECL delnde();
extern  int FDECL kysdel();
extern  int FDECL chkinode();
extern  int FDECL deltree();
extern  int FDECL patchleaf();
extern  int FDECL deluptree();
extern  int FDECL propuptree();
extern  int FDECL patchinode();
extern  void FDECL crtnwhdr();
extern  void FDECL crtnwrt();
extern  int FDECL findkey();
extern  int FDECL findkeysp();
extern  int FDECL addkey();
extern  int FDECL delkey();
extern  int FDECL cicurkey();
extern  int FDECL cicurkeysp();
extern  int FDECL delcur();
extern  int FDECL nextkey();
extern  int FDECL nextkeysp();
extern  int FDECL prevkey();
extern  int FDECL prevkeysp();
extern  int FDECL checkpsp();
extern  void FDECL setpsp();
extern  int FDECL __cadd();
extern  int FDECL ___cadd();
extern  int EDECL cidunqadd();
extern  int FDECL _cunqadd();
extern  int FDECL _cdupadd();
extern  int FDECL _cchange();
extern  int FDECL _csave();
extern  int FDECL _cfind();
extern  int FDECL __cfind();
extern  int FDECL __cfindsp();
extern  int FDECL _cdelete();
extern  int FDECL __cdlt();
extern  int FDECL _cnxext();
extern  int FDECL __cnext();
extern  int FDECL __cnextsp();
extern  int FDECL _cprev();
extern  int FDECL __cprev();
extern  int FDECL __cprevsp();
extern  int FDECL _cfirst();
extern  int FDECL _clast();

extern  int FDECL cispeedon();
extern  int FDECL cispeedoff();

extern float FDECL retfloat();
extern double FDECL retdouble();

extern  int FDECL initcurs();
extern  int FDECL endcurs();

#ifdef MEMORY_MANAGEMENT_REQUIRED
extern  void FDECL unlockcuradr();
#endif

extern  int FDECL _cgetcur();
extern  int FDECL __cgetcur();
extern  int FDECL __cgetcursp();
extern  int FDECL _cdelcur();
extern  int FDECL _cchgcur();
extern  int FDECL cigetdata();
extern  int FDECL cigetdatasp();
extern  int FDECL _csetrec();
extern  int FDECL addall();
extern  int FDECL dobuild();
extern  int FDECL setshrbyte();
extern  int FDECL clrshrbyte();
extern  int FDECL mzapsema();
extern  void FDECL mbldlock();
extern  int FDECL _mclrlck();
extern  int FDECL lowlock();
extern  int FDECL lowunlock();
extern  int FDECL waitlowlock();
extern  int FDECL muflush();
extern  int FDECL testbyte();	/* test byte of file fd */
extern  int FDECL strtopen();
extern  int FDECL endopen();
extern  int FDECL sethdrinfo();
extern  int FDECL mhdrupdte();
extern  int FDECL fgoodbufs();
extern  int FDECL strtbufs();

extern  void FDECL putcurkey();
extern  void FDECL getcurkey();

extern  INT32 FDECL fgetrcval();
extern  int FDECL fgetdbyte();
extern  int FDECL fgetkeynum();
extern  NDEPTR FDECL fgetndenum();
extern  int FDECL fgetindx();
extern  int FDECL fgetdelflag();
extern  int FDECL fgetupdkeyinfo();
extern  int FDECL getcurlock();
extern  KEYSEGLIST * FDECL getcurseg();

extern  void FDECL cisavecurinfo();
extern  void FDECL cirestcurinfo();

extern  void FDECL fputrcval();
extern  void FDECL fputdbyte();
extern  void FDECL fputkeynum();
extern  void FDECL fputndenum();
extern  void FDECL fputindx();
extern  void FDECL fputdelflag();
extern  void FDECL fputupdkeyinfo();
extern  void FDECL fputcurseg();
extern  void FDECL putcurlock();

extern  int FDECL wrtupd();
extern  int FDECL readupd();
extern  int FDECL readwrt();
extern  long FDECL getnextrec();
extern  int FDECL cputrec();
extern  int FDECL crtsegkey();
extern  int FDECL bldsegkey();
extern  int FDECL addkeys();
extern  int FDECL delkeylist();
extern  char * FDECL fieldtrans();
extern  char * FDECL fldmoveup();
extern  int FDECL chgdlist();
extern  int FDECL rvchkeylist();
extern  int FDECL chgkeylist();
extern  int FDECL builddrec();
extern  int FDECL civerifyrec();
extern  int FDECL putdrec();
extern  int FDECL nulldlist();
extern  int FDECL checkdlist();
extern  int FDECL chksumdata();
extern  int FDECL tstchksumdata();
extern  int FDECL chkrwdlist();
extern  int FDECL chkrdlist();
extern  int FDECL chkftype();
extern  int FDECL chkktype();
extern  int FDECL chkdflag();
extern  int FDECL chkflen();
extern  int FDECL isindextype();
extern  int FDECL chkfindex();
extern  int FDECL transundo();
extern  int FDECL transmirror();

/* file conversion functions */
extern  int FDECL convhdrread();
extern  int FDECL convdiskread();
extern  long FDECL convgtkrec();
extern  int FDECL convgtdbyte();
extern  int FDECL convgoodata();
extern  int FDECL convall();
extern  int FDECL doconv();

extern  ALLOCHND FDECL ciallocbuf();
extern  int FDECL cideallocbuf();

#ifdef MEMORY_MANAGEMENT_REQUIRED		/* use this typedef for Window or Mac */
extern  BCBPTR FDECL cilockbuf();
extern  void FDECL ciunlockbuf();
#endif

extern  int FDECL bcheckhdr();
extern  int FDECL bchecknde();
extern  int FDECL imgchecknde();
extern  int FDECL bcheckroot();
extern  int FDECL treecheck();
extern  ENTRY PDECL * FDECL bcheckscanlnde();
extern  int FDECL bcheckkeytest();
extern  int FDECL ccheckdelete();
extern  int FDECL cchecksavebad();
extern  int FDECL findkeylist();
extern  int FDECL civalidatedkey();


#endif		/* ANSI */

