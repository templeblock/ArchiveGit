/*  read.c - cindex base code source file for reading functions
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

/* include modules */
#include "cndx.h"				/* general information */
#include "cindexwk.h"			/* internal work information */

int errcde = 0;					/* global error code */
struct node nodea;				/* node to do all operations in */
struct node nodeb;				/* node b in split */
struct node ebuffer;				/* for use by extend */

#ifdef TRANSACTION
struct node nodet;				/* node buffer for transaction logging */
struct node noder;				/* node buffer for transaction rollback */
#endif	/* TRANSACTION */

#ifdef IMAGEBACK
#ifdef MIXED_MODEL_BUFFERS
struct imgbcb localbcb;				/* local address bcb for mixed model write */
#endif	/* MIXED_MODEL_BUFFERS */
#endif	/* IMAGEBACK */

#ifdef COMPRESSION
struct node nodec;				/* node to compress into */
#endif

char *entlist[MAXENTRIES];		/* ptr list to each entry in node for index scan */
char *leafentlist[MAXENTRIES];/* ptr list to each entry in node for leaf scan */
 



/* copyright notice */
char * FDECL triocopyright()
{
	return("C-Index/II Database Library (c) Copyright Trio Systems LLC 1983-1996");
}


/* verify internal size of psp matches size passed by caller */
int EDECL cipspsize(size)
int size;
{
	int ret;

	if (size != sizeof(CFILE))
		ret = FAIL;
	else
		ret = CCOK;

	return(ret);
}


int FDECL chkhdr(hdr)		/* check header record */
struct hdrrec *hdr;		/* header record to check */
{
	int ret;

	/* verify that index header has not been corrupted */
   if (	hdr->ck1 != 0x56		/* 'V' */
		|| hdr->ck2 != 0x41		/* 'A' */
		|| hdr->ck3 != 0x4c		/* 'L' */
		|| hdr->ck4 != 0x49		/* 'I' */
		|| hdr->ck5 != 0x44		/* 'D' */
		|| hdr->ck6 != 0x41		/* 'A' */
		|| hdr->ck7 != 0x54		/* 'T' */
		|| hdr->ck8 != 0x31)		/* '1' */
      ret = BADINDEX;  /* doesnt check out */
	else
		ret = CCOK;

   return(ret);
}

/* scans index node and returns node ptr - using modified binary search */
NDEPTR FDECL scaniptr(ndea, keyn, key, rec, dbyte, psp, tstorder)
struct node *ndea;	/* node to scan */
int keyn;
char *key;			/* key to compare against */
long rec;			/* record number of key */
int dbyte;			/* data byte of key to compare */
CFILE *psp;			/* in case of read right */
INT32 tstorder;	/* word order for passed key value */
{
	register int i;
	NDEPTR  xnode;					/* value returned - node ptr */
	register char *ptr;
#ifdef NO_FAST_TREESRCH		/* causes a problem with searching buffers directly */
	char *overptr;
#endif
	int cc;

	int leftlmt;					/* left search limit number */
	int rightlmt;					/* right search limit number */
	int cmpnum;						/* comparison item number */
	int res;							/* result of comparison */

	/**/

	if (ndea->count > MAXENTRIES)
	   return(-1);

	if (!ndea->count)  		/* follow p0 on empty nodes */
		return(ndea->p0);

	/* build list of pointers to each entry in node */
	ptr = ndea->keystr;	/* ptr to first key */
	for (i = 0;i < ndea->count;i++) 
	{
#ifdef NO_FAST_TREESRCH		/* causes a problem with searching buffers directly */
		overptr = (char *)(ndea->keystr) + CI_BIGBUF;
	   if (ptr >= overptr)	/* past end */
	      return(-1);
#endif
		entlist[i] = ptr;
	   ptr = moveup(ptr);								/* point to next key */
	}

	leftlmt = 0;						/* leftmost entry number */
	rightlmt = ndea->count - 1;	/* rightmost entry number */
	cmpnum = rightlmt;				/* make first comparison at right end */

	for (;;)
	{
	    /* if key is >= ptr */
	   res = cikeytest(psp, (ENTRY *)entlist[cmpnum], keyn, key, rec, dbyte, tstorder);
		if (res == 0)			/* if found equal match, done */
			break;

		if (res >= 0)			/* if key < entry */
		{
			if (leftlmt == cmpnum)
				break;

			rightlmt = cmpnum - 1;
		}
		else						/* else key > entry */
		{
			if (rightlmt == cmpnum)
			{
				cmpnum++;		/* <= to next key in node */
				break;
			}

			leftlmt = cmpnum + 1;
		}

		cmpnum = ((leftlmt+rightlmt) / 2);
	}

	/* starting point for linear search */
	if (cmpnum)
	   xnode = gtnptr(psp, (ENTRY *)entlist[cmpnum-1]);/* prev node ptr */
	else
		xnode = ndea->p0;						/* set to p0 */


	if (cmpnum == ndea->count)	/* passed high key */
	{
	   if (ndea->forwrd != -1)	/* more to right */
	   {
	      cc = noderead(psp, ndea, ndea->forwrd);	/* move right */
			if (cc != CCOK)
				return (-1);		/* error */
			else						/* else search right node with recursive call */
				xnode = scaniptr(ndea, keyn, key, rec, dbyte, psp, tstorder);
	   }
	}
	else				/* check for index number of node to right */
	{
		if (ndea->count)
			psp->nextkeyn = gtkeyn((ENTRY *)entlist[cmpnum]);
	}

	return(xnode);		/* return node pointer */
}




/* scans leaf node and returns key pointer */
ENTRY * FDECL scanlnde(ndea, keyn, key, rec, dbyte, cnt, psp, tstorder)
struct node *ndea;	/* node to scan */
int keyn;				/* index number */
char PDECL *key;		/* key to compare against */
long rec;				/* record number to compare */
int dbyte;				/* data byte to compare */
int PDECL *cnt;		/* where to put key count of key found */
CFILE PDECL *psp;		/* in case of read right */
INT32 tstorder;		/* word order of passed key value */
{
	register int i;	
	register char PDECL *ptr;		/* ptr into node */

	int leftlmt;					/* left search limit number */
	int rightlmt;					/* right search limit number */
	int cmpnum;						/* comparison item number */
	int res;							/* result of comparison */

	/**/

	if (ndea->count > MAXENTRIES)
	   return((ENTRY PDECL *)NULL);

	if (!ndea->count)				/* if empty node */
	{
		psp->keycmpres = 1;		/* assume key is larger */
		psp->strcmpres = 1;
		*cnt = 0;					/* ptr to beg of empty node */
		return((ENTRY PDECL *)ndea->keystr);
	}

	/* build list of pointers to each entry in node */
	ptr = ndea->keystr;	/* ptr to first key */
	for (i = 0;i <= ndea->count;i++) 
	{
#ifdef NO_FAST_TREESRCH		/* causes a problem with searching buffers directly */
	   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
	      return((ENTRY *)NULL);
#endif
		leafentlist[i] = ptr;
	   ptr = moveup(ptr);								/* point to next key */
	}
#ifdef REMOVED
	scancnt = ndea->count;			/* number of items in scanned list */
#endif

	leftlmt = 0;						/* leftmost entry number */
	rightlmt = ndea->count - 1;	/* rightmost entry number */
	cmpnum = rightlmt;				/* make first comparison at right end */

	for (;;)
	{
	    /* if key is >= ptr */
	   res = cikeytest(psp, (ENTRY *)leafentlist[cmpnum], keyn, key, rec, dbyte, tstorder);
		if (res == 0)			/* if found equal match, done */
			break;

		if (res >= 0)			/* if key < entry */
		{
			if (leftlmt == cmpnum)
				break;

			rightlmt = cmpnum - 1;
		}
		else						/* else key > entry */
		{
			if (rightlmt == cmpnum)
			{
				cmpnum++;		/* <= to next key in node */
				/* retest key comparison with next key in node */
			   cikeytest(psp, (ENTRY *)leafentlist[cmpnum], keyn, key, rec, dbyte, tstorder);
				break;
			}

			leftlmt = cmpnum + 1;
		}

		cmpnum = ((leftlmt+rightlmt) / 2);
	}


	*cnt = cmpnum;										/* set key count */
	return((ENTRY *)leafentlist[cmpnum]);		/* return pointer to desired item */
}


		
 void FDECL setnull(psp)	/* set psp parms when no key found */
register struct passparm *psp;	
{
	psp->dlen = 0;		/* no data - used internally */
	psp->retlen = 0;	/* no data found or returned */
	psp->rec = 0;		/* record number = 0 */
#ifdef REMOVED
	*psp->retkey = '\0';	/* no key found */
#endif
	cclear(psp->retkey, sizeof(psp->retkey), 0);
	psp->keyorder = psp->wordorder;
	psp->retcde = FAIL;	/* ret code is fail */

	/* MU */
   setcurkey(psp, NULL);

	return;
}


/* sets parameters after treesearch */
int FDECL setparm(psp, entptr)
struct passparm *psp;
ENTRY *entptr;	/* key found */
{
	int ret;
#ifndef NO_WORD_CONVERSION
	INT32 ordersav;				/* save wordorder before reset */
#endif	/* NO_WORD_CONVERSION */

#ifndef NO_WORD_CONVERSION
	if (psp->key != psp->retkey)
	{
		ordersav = psp->keyorder;				/* save internal key order */
		cipspflip(psp, INTELFMT);				/* reset order of passed parameter */
		psp->key = psp->retkey;					/* set key to value in psp */
		psp->keyorder = ordersav;				/* reset order of internal key */
	}
#else
	psp->key = psp->retkey;					/* set key */
#endif	/* NO_WORD_CONVERSION */

	if (!gtentlen(entptr))		/* at end of node */
	{
	   setnull(psp);				/* null data */
	}
	else  							/* else have found something */
	{

	   psp->retcde = psp->keycmpres;		/* set ret code */
	   cmovedata(entptr, psp);				/* move in data */
	}

	if (psp->retcde > 0)		/* if found higher key */
	{
	   if (psp->strcmpres == 0)	/* and key matches but not rec# */
	      ret = KEYMATCH;			/* return KEYMATCH */
	   else
	      ret = GREATER;			   /* else return greater */
	}
	else
	if (psp->retcde < 0)				/* if past end  */
	   ret = FAIL;						/* failed */
	else
		ret = CCOK;						/* else must be exact match */

	return(psp->retcde = ret);		/* else must be exact match */
}


/* searches tree - returns ptr to key or 0 if error - sets current info */
ENTRY * FDECL treesrch(psp, keyn, key, rec, dbyte)
register struct passparm *psp;
int keyn;	/* index # of key */
char *key;	/* key to search for */
long rec;	/* record number to search for */
int dbyte;	/* data byte to search for */
{
	int cnt;						/* count of key found */
	register NDEPTR nde;		/* node number of root */
	ENTRY *entptr;
	int oldlevel;
	int cc;
	int fndkeyn;
#ifndef NO_FAST_TREESRCH
	char *ptr;
#endif

	/* test for adding past end of file to speed up processing */
	if (	(nodea.forwrd == -1)					/* last leaf node in file */
		&& (!nodea.level)
		&& (nodea.count)							/* have entries in this node */
		&& (nodea.npsp == psp)					/* for this file's psp */
		&& (nodea.nfd == psp->fd)				/* for this file's fd */
		&& (nodea.nde != -1)						/* and have a good node */
		&& (psp->path[0] == nodea.nde)		/* and same as search path */
		&& (psp->treeindex == psp->keyn))	/* and the search tree is OK */
	{
		psp->nextkeyn = keyn;					/* assume next node has same index */
		nde = nodea.nde;   						/* set to last leaf node */
		entptr = scanlnde(&nodea, keyn, key, rec, dbyte, &cnt, psp, psp->wordorder);  /* find key */
	}
	else
	{
		entptr = (ENTRY *)NULL;		/* force normal scan */
		nde = -1;						/* prevent compiler error msg */
	}

	if (!entptr || gtentlen(entptr))			/* if need normal scan */
	{
		nodea.npsp= NULL;			/* clear nodea file psp */
		nodea.nfd = NULLFD;		/* clear nodea file fd */
		nodea.nde = NULLNDE;		/* clear nodea node number */

		/* normal tree scan from top of index */
		psp->nextkeyn = keyn;					/* assume next node has same index */
		nde = gtroot(psp, keyn);				/* set to root node to start */
#ifndef NO_FAST_TREESRCH
		cc = noderead(psp, (struct node *)0, nde);	/* read root - don't require copy to nodea */
#else
		cc = noderead(psp, &nodea, nde);	/* read root - read into nodea */
#endif
		if (cc != CCOK)
			return(0);

#ifndef NO_FAST_TREESRCH
		/* must copy leaf node into nodea for compatibility */
		if (!psp->speednod->smlevel && psp->speednod != (struct smallnode *)&nodea)
		{
			/* copy info to nodea */
			blockmv((char *) &nodea, (char *) psp->speednod, sizeof(struct smallnode));

			/* clear area past end of disk node (CI_BIGBUF extra space) */
			ptr = (char *)(nodea.keystr);
			ptr += (KEYAREA * MAXCOMP);
			cclear(ptr, (CI_BIGBUF - (KEYAREA * MAXCOMP)), 0);

			nodea.npsp = psp;			/* set the work buffers psp address */
			nodea.nfd  = psp->fd;	/* set the work buffers fd */
			nodea.nde = nde;			/* set the work buffers node number */
		}
#endif

		while (psp->speednod && psp->speednod->smlevel) 	/* while not a leaf node */
		{	
		   oldlevel = psp->speednod->smlevel;
		   psp->path[psp->speednod->smlevel] = nde;	/* set search path for this level */
		   nde = scaniptr((struct node *)psp->speednod, keyn, key, rec, dbyte, psp, psp->wordorder);   /* find lower node */
		   if (nde == -1)	/* error in scan */
		      return(0);	/* return error */

			/* read lower node - leaf node must go into nodea */
		   cc = noderead(	psp, 
#ifndef NO_FAST_TREESRCH
								(oldlevel > 1) ? (struct node *)0 : &nodea,
#else
								&nodea,
#endif
								nde);
		   if ((cc != CCOK) || (oldlevel != psp->speednod->smlevel + 1))	/* bad scan */
		      return(0);			/* error */

#ifndef NO_FAST_TREESRCH
			/* must copy leaf node into nodea for compatibility */
			if (!psp->speednod->smlevel && psp->speednod != (struct smallnode *)&nodea)
			{
				/* copy info to nodea */
				blockmv((char *) &nodea, (char *) psp->speednod, sizeof(struct smallnode));

				/* clear area past end of disk node (CI_BIGBUF extra space) */
				ptr = (char *)(nodea.keystr);
				ptr += (KEYAREA * MAXCOMP);
				cclear(ptr, (CI_BIGBUF - (KEYAREA * MAXCOMP)), 0);

				nodea.npsp = psp;			/* set the work buffers psp address */
				nodea.nfd  = psp->fd;	/* set the work buffers fd */
				nodea.nde = nde;			/* set the work buffers node number */
			}
#endif
		}	
		/* now at leaf level */
	   psp->path[0] = nde;	/* set search path for leaf level */
		entptr = scanlnde(&nodea, keyn, key, rec, dbyte, &cnt, psp, psp->wordorder);  /* find key */
		if (entptr == 0)
		   return(0);		/* bad leaf scan */

								   	   /* what looking at or if nothing, cur index */
		psp->treeindex = keyn;		/* index that tree applies to */
	}

	putkeynum(psp, keyn, cnt);		/* set key count */
	putndenum(psp, keyn, nde);		/* set last node read */
#ifdef REMOVED
	putcurindx(psp, keyn, ((*p) ? *(p + 1) : (char)(keyn)));		/* set current index */
#endif
	/* set current index */
	fndkeyn = gtkeyn(entptr);
	putcurindx(psp, keyn, ((fndkeyn) ? (char)fndkeyn : (char)(keyn)));

	/* MU */
	setcurkey(psp, entptr);	/* set current key and record number */

	return(entptr);
}
 		
					
/* searches tree - returns ptr to key or 0 if error - sets current info */
int FDECL treesrchsp(psp, keyn, key, rec, dbyte)
register struct passparm *psp;
int keyn;	/* index # of key */
char *key;	/* key to search for */
long rec;	/* record number to search for */
int dbyte;	/* data byte to search for */
{
	int cnt;						/* count of key found */
	register NDEPTR nde;		/* node number of root */
	ENTRY *entptr;
	int oldlevel;
	int fndkeyn;

	/* NOTE: these two lines do not do anything, but are required by some
    * defective compilers to allow this file to be compiled
    */
	entptr = (ENTRY *)0;			/* force normal scan */
	nde = -1;						/* prevent compiler error msg */

	/* normal tree scan from top of index */
	psp->nextkeyn = keyn;					/* assume next node has same index */
	nde = gtroot(psp, keyn);				/* set to root node to start */
	psp->speednod = nodereadsp(psp, nde);		/* read root */

	if (!psp->speednod)
	{
		psp->speedentptr = (ENTRY *)0;
		return(NODENOTINMEM);
	}

	psp->treeindex = -1;			/* invalidate tree path, speedread cannot be used for writes */

	while (psp->speednod->smlevel) 	/* while not a leaf node */
	{	
		oldlevel = psp->speednod->smlevel;
		nde = scaniptr((struct node *)psp->speednod, keyn, key, rec, dbyte, psp, psp->wordorder);	/* find lower node */
		if (nde == -1)	/* error in scan */
		{
			psp->speedentptr = (ENTRY *)0;
			return(CCOK);	/* return error */
		}

		psp->speednod = nodereadsp(psp, nde);		/* and read lower node */
		if (!psp->speednod)
		{
			psp->speedentptr = (ENTRY *)0;
			return(NODENOTINMEM);
		}


		if (oldlevel != psp->speednod->smlevel + 1)	/* bad scan */
		{
			psp->speedentptr = (ENTRY *)0;			/* error */
			return(CCOK);
		}
	}	

	/* now at leaf level */
	entptr = scanlnde((struct node *)psp->speednod, keyn, key, rec, dbyte, &cnt, psp, psp->wordorder);  /* find key */
	if (entptr == 0)
	{
		psp->speedentptr = (ENTRY *)0;		/* bad leaf scan */
		return(CCOK);
	}
#ifdef REMOVED
										/* what looking at or if nothing, cur index */
	psp->treeindex = keyn;		/* index that tree applies to */
#endif

	putkeynum(psp, keyn, cnt);		/* set key count */
	putndenum(psp, keyn, nde);		/* set last node read */

	/* set current index */
	fndkeyn = gtkeyn(entptr);
	putcurindx(psp, keyn, ((fndkeyn) ? (char)fndkeyn : (char)(keyn)));

	/* MU */
	setcurkey(psp, entptr);	/* set current key and record number */
	psp->speedentptr = entptr;

	return(CCOK);
}
 		
					
void FDECL setfile(psp, hdr)	/* moves header info into psp */
register struct passparm *psp;	
register struct hdrrec *hdr;	/* header record */
{
	psp->pupdatnum = hdr->updatnum;		/* current update number */
	psp->wordorder = hdr->hwordorder;	/* word byte order flag */
	psp->singleroot = hdr->hsingleroot;					/* set root */
	psp->singlelevels = (int)hdr->hsinglelevels;		/* set levels */
	psp->next = hdr->hnext;					/* set next node */
	psp->nxtdrec = hdr->hnxtdrec;			/* set next data rec # */
	psp->endofile = hdr->hendofile;		/* set end of file */
	psp->updtflag = CLEAN;					/* start with clean header */
	psp->freenodes = hdr->hfreenodes;	/* free node chain */
	psp->transchain = hdr->htranschain;	/* transaction log chain */
	psp->transback = hdr->htransback;	/* transaction log backup chain */
	psp->delcnt = hdr->hdelcnt;			/* deleted record count */
	psp->reccnt = hdr->hreccnt;			/* active record count */
	psp->custlen = (int)hdr->hcustlen;	/* length of custom info */
	if (	(psp->custlen > 0)
		&& (psp->custlen <= MAXCUSTINFO))	/* move custom info */
		blockmv(psp->custinfo, hdr->hcustinfo, psp->custlen);
	else
		psp->custlen = 0;							/* else no custom info */

	blockmv(psp->indtyp, hdr->hindtyp, NUMINDXS);	/* index type info */
	blockmv(psp->rootnum, hdr->hrootnum, NUMINDXS);	/* root # for this index */
	blockmv(psp->levelslist, hdr->hlevelslist, sizeof(psp->levelslist));	/* tree levels list */
	blockmv(psp->rootlist, hdr->hrootlist, sizeof(psp->rootlist));	/* root node list */
}


void FDECL sethdr(psp, hdr)	/* moves psp into header record */
register struct passparm *psp;
register struct hdrrec *hdr;	/* header record */
{
	hdr->hwordorder = psp->wordorder;		/* byte order of words */
	hdr->hsingleroot = psp->singleroot;		/* set root */
	hdr->hsinglelevels = psp->singlelevels;				/* set levels */
	hdr->hnext = psp->next;						/* set next node number */
	hdr->hnxtdrec = psp->nxtdrec;				/* set next data record */
	hdr->hendofile = psp->endofile;			/* set end of file */
	hdr->hfreenodes = psp->freenodes;		/* node freechain */
	hdr->hdelcnt = psp->delcnt;				/* deleted record count */
	hdr->hreccnt = psp->reccnt;				/* active record count */
	hdr->htranschain = psp->transchain;		/* transaction log chain */
	hdr->htransback  = psp->transback;		/* transaction log backup chain */
	hdr->hres1 = 0;								/* clear reserved items */
	hdr->hres2 = 0;
	hdr->hres3 = 0;
	hdr->hres4 = 0;
#ifndef PRE41
	hdr->hres5 = 0;
#endif
	hdr->hcustlen = psp->custlen;				/* length of custom info */
	if ((psp->custlen <= MAXCUSTINFO)
		&& (psp->custlen > 0))					/* move custom info */
	{
		blockmv(hdr->hcustinfo, psp->custinfo, psp->custlen);
		if (psp->custlen < MAXCUSTINFO)	/* clear rest of custom info */
			cclear(hdr->hcustinfo+psp->custlen, MAXCUSTINFO-psp->custlen, 0);		/* clear custom header info area */
	}
	else
	{
		hdr->hcustlen = 0;
	 	cclear(hdr->hcustinfo, MAXCUSTINFO, 0);		/* clear custom header info area */
	}

	blockmv(hdr->hindtyp, psp->indtyp, NUMINDXS);	/* index type info */
	blockmv(hdr->hrootnum, psp->rootnum, NUMINDXS);	/* root # for this index */
	blockmv(hdr->hlevelslist, psp->levelslist, sizeof(hdr->hlevelslist));	/* tree levels list */
	blockmv(hdr->hrootlist, psp->rootlist, sizeof(hdr->hrootlist));	/* root node list */


	/* now set check bytes so header is good */
   hdr->ck1 = 0x56;   /* V */
   hdr->ck2 = 0x41;   /* A */
   hdr->ck3 = 0x4c;   /* L */
   hdr->ck4 = 0x49;   /* I */
   hdr->ck5 = 0x44;   /* D */
   hdr->ck6 = 0x41;   /* A */
   hdr->ck7 = 0x54;   /* T */
   hdr->ck8 = 0x31;   /* 1 */
}



ENTRY * FDECL cigetkey(ndea, cnt)    /* gets key num cnt from nde */
struct node *ndea;		/* node to get key from */
int cnt;			/* key number to get */
{
	register int i;
	register char *p;

	if (cnt == -1 || cnt >= ndea->count)      /* past end */
	   return ((ENTRY *) 0);

	p = ndea->keystr;		/* start at first key */
	
	for (i = 0; i < cnt; i++)	/* go to key count */
	   p = moveup(p);		/* move up until key is found */

	return((ENTRY *)p);			/* and return pointer */
}


ENTRY * FDECL next(psp, keyn)	
/* used when at end of node, this returns pointer to first key in next node */
register struct passparm *psp;
register int keyn;		/* index number */ 
{
	int cc;

	/**/

	if (nodea.forwrd == -1)		/* no more nodes */
	   return(NULL);			/* return null */

	putkeynum(psp, psp->keyn, 0);	/* first key of next node */
	psp->treeindex = -1;			/* invalidate tree path since moving right */

	while (nodea.forwrd > -1) 	/* while nodes left ahead */
	{
	   putndenum(psp, keyn, nodea.forwrd);	/* set node num to forward */
	   cc = noderead(psp, &nodea, nodea.forwrd);	/* read forward */
		if (cc != CCOK)
			return(NULL);				/* error */

	   if (nodea.count)			/* if node has any keys */
	      break;				/* stop */
	}

	/* return ptr to first key (or blank area) */
	return((ENTRY *)nodea.keystr);
}
		

int FDECL nextsp(psp, keyn)	
/* used when at end of node, this returns pointer to first key in next node */
register struct passparm *psp;
register int keyn;		/* index number */ 
{

	/**/

	if (!psp->speednod || (psp->speednod->smforwrd == -1))	/* no more nodes */
		return(FAIL);

	putkeynum(psp, psp->keyn, 0);	/* first key of next node */
	psp->treeindex = -1;			/* invalidate tree path since moving right */

 	/* while nodes left ahead */
	while (psp->speednod && psp->speednod->smforwrd > -1)
	{
	   putndenum(psp, keyn, psp->speednod->smforwrd);	/* set node num to forward */
	   psp->speednod = nodereadsp(psp, psp->speednod->smforwrd);	/* read forward */
		if (!psp->speednod)
		{
			psp->speedentptr = (ENTRY *) 0;
	   	return(NODENOTINMEM);			/* return error */
		}

		/* if node has any keys - stop */
	   if (psp->speednod && psp->speednod->smcount)
	      break;
	}

	/* return ptr to first key (or blank area) */
	psp->speedentptr = (ENTRY *)((psp->speednod) ? psp->speednod->smkeystr : 0);

	return(CCOK);
}
		

/* used when at beginning of node, this returns first previous key */	
ENTRY * FDECL prev(psp, keyn)
struct passparm *psp;
int keyn;		/* index number */
{
	ENTRY *entptr;
	int cc;

	/**/

	if (nodea.revrse == -1)		/* no more nodes */
	   return(0);

	putkeynum(psp, psp->keyn, 0);	/* first key of next node */
	psp->treeindex = -1;				/* invalidate tree path since moving left */
	entptr = (ENTRY *)NULL;			/* set to null at start */
	
	while (nodea.revrse > -1) 		/* while nodes left */
	{		
#ifdef REMOVED
	   psp->ndenum[keyn] = nodea.revrse;	/* set node num to prev */
#endif
	   putndenum(psp, keyn, nodea.revrse);	/* set node num to prev */
	   cc = noderead(psp, &nodea, nodea.revrse);	/* read prev */
		if (cc != CCOK)
			return(0);			/* error */

	   if (nodea.count) 		/* if node has keys in it */
	   {
#ifdef REMOVED
	      psp->keynum[keyn] = nodea.count - 1;	/* set keynum */
#endif
			putkeynum(psp, keyn, (nodea.count - 1));	/* set keynum */
	      entptr = cigetkey(&nodea, nodea.count - 1); /* get ptr to last key */
	      break;					/* end */
	   }
	}

	return(entptr);		/* return pointer to last key or null */
}
		

/* used when at beginning of node, this returns first previous key */	
/* this is the speed read method of prev() */
int FDECL prevsp(psp, keyn)
struct passparm *psp;
int keyn;		/* index number */
{
	ENTRY *entptr;

	/**/

	/* if no more nodes */
	if (!psp->speednod || psp->speednod->smrevrse == -1)
	{
		psp->speedentptr = 0;
	   return(CCOK);
	}

	putkeynum(psp, psp->keyn, 0);	/* first key of next node */
	psp->treeindex = -1;				/* invalidate tree path since moving left */
	entptr = (ENTRY *)NULL;			/* set to null at start */
	
	while (psp->speednod->smrevrse > -1) 		/* while nodes left */
	{		
	   putndenum(psp, keyn, psp->speednod->smrevrse);	/* set node num to prev */

		/* read previous node */
	   psp->speednod = nodereadsp(psp, psp->speednod->smrevrse);
		if (!psp->speednod)
		{
			psp->speedentptr = (ENTRY *) 0;
	   	return(NODENOTINMEM);			/* return error */
		}

	   if (psp->speednod->smcount) 		/* if node has keys in it */
	   {
			putkeynum(psp, keyn, (psp->speednod->smcount - 1));	/* set keynum */
	      entptr = cigetkey((struct node *)psp->speednod, psp->speednod->smcount - 1); /* get ptr to last key */
	      break;					/* end */
	   }
	}

	psp->speedentptr = entptr;		/* return pointer to last key or null */
	return(CCOK);
}
		

ENTRY * FDECL getnxtkey(psp, keyn)	/* get next key in order */
register struct passparm *psp;	
register int keyn;			/* index number */
{
	int knum;

	knum = getkeynum(psp, keyn) + 1;
	putkeynum(psp, keyn, knum);
	if (knum >= nodea.count)			/* if at end */
	   return(next(psp, keyn));		/* return key in next node */

	/* else return next key in this node */
	return(cigetkey(&nodea, getkeynum(psp, keyn)));
}
			

/* get next key in order - speed read mode */
int FDECL getnxtkeysp(psp, keyn)
register struct passparm *psp;	
register int keyn;					/* index number */
{
	int knum;

	knum = getkeynum(psp, keyn) + 1;
	putkeynum(psp, keyn, knum);
	if (!psp->speednod || (knum >= psp->speednod->smcount))		/* if at end */
	   return(nextsp(psp, keyn));		/* return key in next node */

	/* else return next key in this node */
	if (psp->speednod)
		psp->speedentptr = cigetkey((struct node *)psp->speednod, getkeynum(psp, keyn));
	else
		psp->speedentptr = (ENTRY *)0;

	return(CCOK);
}
			

/* get previous key in sorted order */
ENTRY * FDECL getprev(psp, keyn)
struct passparm *psp;	
int keyn;							/* index number */
{
	int knum;

	knum = getkeynum(psp, keyn);
	if (knum == -1 && nodea.revrse == -1) /* if past end */
	   return(0);		/* then return 0 */

	if (knum == -1)					/* if before beginnin of node */
	{
	   return (prev(psp, keyn));	/* get first key in prev node */
	}

	knum--;
	putkeynum(psp, keyn, knum);
	if (knum == -1)			/* if end of node */
	{
	   return (prev(psp, keyn));	/* get last key in prev node */
	}

	/* else get prev key in this node */
#ifdef REMOVED
   return(cigetkey(&nodea,getkeynum(psp, keyn)));
#endif
   return(cigetkey(&nodea, knum));
}
		


/* get previous key in sorted order - speed read method */
int FDECL getprevsp(psp, keyn)
struct passparm *psp;	
int keyn;							/* index number */
{
	int knum;

	knum = getkeynum(psp, keyn);
	if (	!psp->speednod
		|| (knum == -1 && psp->speednod->smrevrse == -1)) /* if past end */
	{
		psp->speedentptr = (ENTRY *)0;
	   return(CCOK);		/* then return 0 */
	}

	if (knum == -1)					/* if before beginning of node */
	{
	   return (prevsp(psp, keyn));	/* get last key in prev node */
	}

	knum--;
	putkeynum(psp, keyn, knum);
	if (knum == -1)			/* if end of node */
	{
	   return (prevsp(psp, keyn));	/* get first key in prev node */
	}

	/* else get prev key in this node */
	psp->speedentptr = cigetkey((struct node *)psp->speednod, knum);
   return(CCOK);
}
		


int FDECL findkey(psp)		/* generic find */
register struct passparm *psp;
{
	ENTRY *ptr;
	int fndkeyn;

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	ptr = treesrch(psp, psp->keyn, psp->key, psp->rec, psp->dbyte);
	/* find something */

	if (!ptr)
	   return(psp->retcde = BADNODE);

	if (!gtentlen(ptr)) 			/* if no key found */
	{
	   ptr = next(psp, psp->keyn); 	/* get next key */
		if (ptr)
		{
#ifndef NO_WORD_CONVERSION
			cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
			cikeytest(psp, ptr, psp->keyn, psp->key, psp->rec, psp->dbyte, INTELFMT);	/* retest */
		}
		else
		{
			psp->strcmpres = -1;			/* no next, assume key larger */
			psp->keycmpres = -1;
		}

	   /* set current index */
		fndkeyn = gtkeyn((ENTRY *)ptr);
		putcurindx(psp, psp->keyn, ((fndkeyn) ? fndkeyn : psp->keyn + 1));

	   /* MU */
	   setcurkey(psp, ptr);						/* set current key and record num */
	}

	putdelflag(psp, psp->keyn, DELOFF);		/* del flag now clear */
	return(setparm(psp, ptr));					/* set parms for whatever found */
}		


int FDECL findkeysp(psp)		/* generic find - speed read method */
register struct passparm *psp;
{
	int fndkeyn;
	int cc;

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */

	/* try to find something in the tree */
	cc = treesrchsp(psp, psp->keyn, psp->key, psp->rec, psp->dbyte);
	if (cc == CCOK)
	{
		if (!psp->speedentptr)
		   return(psp->retcde = BADNODE);

		if (!gtentlen(psp->speedentptr)) 			/* if no key found */
		{
		   cc = nextsp(psp, psp->keyn); 	/* get next key */
			if (cc == NODENOTINMEM)
				return(cc);

			if (psp->speedentptr)
			{
#ifndef NO_WORD_CONVERSION
				cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
				cikeytest(psp, psp->speedentptr, psp->keyn, psp->key, psp->rec, psp->dbyte, INTELFMT);	/* retest */
			}
			else
			{
				psp->strcmpres = -1;			/* no next, assume key larger */
				psp->keycmpres = -1;
			}

			/* set current index */
			fndkeyn = gtkeyn((ENTRY *)psp->speedentptr);
			putcurindx(psp, psp->keyn, ((fndkeyn) ? fndkeyn : psp->keyn + 1));

			/* MU */
			setcurkey(psp, psp->speedentptr);						/* set current key and record num */
		}

		putdelflag(psp, psp->keyn, DELOFF);		/* del flag now clear */
		cc = setparm(psp, psp->speedentptr);	/* set parms for whatever found */
	}
	
	return(psp->retcde = cc);
}		


int FDECL cicurkey(psp)		/* get current key */
register struct passparm *psp;
{
	register ENTRY *entptr;
	int cc;

	/**/

#ifdef REMOVED
	cc = noderead(psp, &nodea, psp->ndenum[psp->keyn]);	/* get leaf */
#endif
	cc = noderead(psp, &nodea, getndenum(psp, psp->keyn));	/* get leaf */
	if (cc != CCOK)
		return(psp->retcde = cc);				/* error */

	if (getdelflag(psp, psp->keyn) == DELON	/* delflag on */
		|| (entptr = cigetkey(&nodea, getkeynum(psp, psp->keyn))) == 0) 
	{	 /* or no more keys */
	   setnull(psp);		/* null data */
	   psp->retcde = FAIL;	/* no current key */
	}
	else		/* else get current */ 
	{
	   cmovedata(entptr, psp);	/* move in data */
#ifdef REMOVED
	   psp->curindx[psp->keyn] = *(ptr + 1);  /* set current index */
#endif
	   putcurindx(psp, psp->keyn, gtkeyn(entptr));	/* current index */

	   /* MU */
	   setcurkey(psp, entptr);
	   psp->retcde = CCOK;			  /* good return */
	}

#ifndef NO_WORD_CONVERSION
	if (psp->key != psp->retkey)
		cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
	psp->key = psp->retkey;			  /* set for caller */
	psp->keyorder = psp->wordorder;

	return(psp->retcde);			  /* set return code */
}


int FDECL cicurkeysp(psp)		/* get current key - speed read method */
register struct passparm *psp;
{
	register ENTRY *entptr;

	/**/

	psp->speednod = nodereadsp(psp, getndenum(psp, psp->keyn));	/* get leaf */
	if (!psp->speednod)
	{
		psp->speedentptr = (ENTRY *) 0;
   	return(psp->retcde = NODENOTINMEM);			/* return error */
	}

	if (getdelflag(psp, psp->keyn) == DELON	/* delflag on */
		|| (entptr = cigetkey((struct node *)psp->speednod, getkeynum(psp, psp->keyn))) == 0) 
	{	 /* or no more keys */
	   setnull(psp);		/* null data */
		entptr = (ENTRY *)0;
	   psp->retcde = FAIL;	/* no current key */
	}
	else		/* else get current */ 
	{
	   cmovedata(entptr, psp);	/* move in data */
	   putcurindx(psp, psp->keyn, gtkeyn(entptr));	/* current index */

	   /* MU */
	   setcurkey(psp, entptr);
	   psp->retcde = CCOK;			  /* good return */
	}

#ifndef NO_WORD_CONVERSION
	if (psp->key != psp->retkey)
		cipspflip(psp, INTELFMT);		/* reset order of existing key */
#endif	/* !NO_WORD_CONVERSION */
	psp->key = psp->retkey;			  /* set for caller */
	psp->keyorder = psp->wordorder;
	psp->speedentptr = entptr;

	return(psp->retcde);			  /* set return code */
}


int FDECL nextkey(psp)		/* get next key in sequence */
register struct passparm *psp;
{
	register ENTRY *entptr;
	int cc;
	INT32 rcval;
	int curdb;

	/**/

	/* MU */
	/* if need to position */
	if ((psp->processmode != EXCL) && (getupdkeyinfo(psp, psp->keyn)))
	{
	   char tempkey[MAXMKLEN];
#ifdef REMOVED
	   static char end[] = { 1, 0 };
#endif

		getcurkey(psp, psp->keyn, tempkey);
	   psp->key = tempkey;
		psp->keyorder = psp->wordorder;

		curdb = getcurdbyte(psp, psp->keyn);
	   if ((curdb + 1) < curdb)
	   /* cant increment */
	   {
			rcval = getcurrcval(psp, psp->keyn);
	      if ((rcval + 1) < rcval)
	      /* cant increment */
	      {
#ifdef REMOVED
	         strcat(tempkey, end);	/* one bigger now */
#endif
				incrkeyval(psp, psp->keyn, psp->indtyp[psp->keyn], tempkey);
	         psp->rec = 0;
	         psp->dbyte = 0;
	      }
	      else
	      {
	         psp->rec = rcval + 1;
	         psp->dbyte = 0;
	      }
	   }
	   else
	   {
			psp->dbyte = getcurdbyte(psp, psp->keyn) + 1;
	      psp->rec = getcurrcval(psp, psp->keyn);	      /* just bump up */
	   }

		putupdkeyinfo(psp, psp->keyn, FALSE);	/* EXCL key info is current */
	   return(findkey(psp));  /* actual find */
	}
	/* else proceed as usual for EXCL mode of operation */

	cc = noderead(psp, &nodea, getndenum(psp, psp->keyn));	/* get leaf */
	if (cc != CCOK)
		return(psp->retcde = cc);				/* error */

	entptr = getnxtkey(psp, psp->keyn);
	if (!gtentlen(entptr))
	{	/* if no more keys in this index */
	   putcurindx(psp, psp->keyn, (getcurindx(psp, psp->keyn) + 1));
	   /* set current index to one more than before */
	   setnull(psp);		/* null data */
	}
	else 
	{
	   cmovedata(entptr, psp);	/* move in data */
#ifdef REMOVED
	   psp->curindx[psp->keyn] = *(ptr + 1); /* set current index */
#endif
	   putcurindx(psp, psp->keyn, gtkeyn(entptr));	/* current index */
	   setcurkey(psp, entptr);	   						/* MU */
	   psp->retcde = CCOK;									/* good return */
	}

#ifndef NO_WORD_CONVERSION
	if (psp->key != psp->retkey)
		cipspflip(psp, INTELFMT);		/* reset order of existing key */
#endif	/* !NO_WORD_CONVERSION */
	psp->key = psp->retkey;									/* set for caller */
	psp->keyorder = psp->wordorder;
	putdelflag(psp, psp->keyn, DELOFF);					/* now clear */
	return(psp->retcde);
}


/* get next key in sequence - speed read method */
int FDECL nextkeysp(psp)
register struct passparm *psp;
{
	register ENTRY *entptr;
	int cc;

	/**/

	/* get leaf node from in-memory buffers */
	psp->speednod = nodereadsp(psp, getndenum(psp, psp->keyn));
	if (!psp->speednod)
		return(psp->retcde = NODENOTINMEM);		/* error - can only do this if in memory */

	cc = getnxtkeysp(psp, psp->keyn);
	if (cc)
	{
		if (cc == FAIL)			/* end of file */
		{	/* if no more keys in this index */
		   putcurindx(psp, psp->keyn, (getcurindx(psp, psp->keyn) + 1));
	   	/* set current index to one more than before */
		   setnull(psp);		/* null data */
		}

		return(psp->retcde = cc);
	}

	entptr = psp->speedentptr;		/* value passed back by getnxtkeysp */
	if (entptr && gtentlen(entptr))
	{
	   cmovedata(entptr, psp);	/* move in data */
	   putcurindx(psp, psp->keyn, gtkeyn(entptr));	/* current index */
	   setcurkey(psp, entptr);	   						/* MU */
	   psp->retcde = CCOK;									/* good return */
	}
	else
	{
	   psp->retcde = FAIL;									/* no key found */
	}

	return(psp->retcde);
}


/* get previous key */
int FDECL prevkey(psp)
struct passparm *psp;
{
	ENTRY *entptr;
	int cc;

	/**/

	if (getdelflag(psp, psp->keyn) == DELON		/* just deleted */
		&& getkeynum(psp, psp->keyn) > -1)		/* and not between nodes */
	{
	   putdelflag(psp, psp->keyn, DELOFF);		/* clear delete flag */
	   cicurkey(psp);	/* since one back, get this one */
	   return(psp->retcde);	/* and leave */
	}

	/* else a normal prev */
	cc = noderead(psp, &nodea, getndenum(psp, psp->keyn));	/* get leaf */
	if (cc != CCOK)
		return(psp->retcde = cc);				/* error */

	if ((entptr = getprev(psp, psp->keyn)) == 0)    /* if no more */
	{   
	   /* set current index to one less than before */
	   putcurindx(psp, psp->keyn, (getcurindx(psp, psp->keyn) - 1));  
	   setnull(psp);		/* null data */
	}
	else	/* else there is a key */
	{
	   cmovedata(entptr, psp);								/* move in data */
#ifdef REMOVED
	   psp->curindx[psp->keyn] = *(ptr + 1);	/* set current */
#endif
	   putcurindx(psp, psp->keyn, gtkeyn(entptr));	/* current index */
	   setcurkey(psp, entptr);	   						/* MU */
	   psp->retcde = CCOK;									/* good return */
	}

#ifndef NO_WORD_CONVERSION
	if (psp->key != psp->retkey)
		cipspflip(psp, INTELFMT);		/* reset order of existing key */
#endif	/* !NO_WORD_CONVERSION */
	psp->key = psp->retkey;								/* set for caller */
	psp->keyorder = psp->wordorder;
	putdelflag(psp, psp->keyn, DELOFF);				/* del flag now clear */
	return(psp->retcde);
}

/* get previous key - speed read method */
int FDECL prevkeysp(psp)
struct passparm *psp;
{
	/**/

	if (getdelflag(psp, psp->keyn) == DELON		/* just deleted */
		&& getkeynum(psp, psp->keyn) > -1)		/* and not between nodes */
	{
	   putdelflag(psp, psp->keyn, DELOFF);		/* clear delete flag */
	   cicurkeysp(psp);	/* since one back, get this one */
	   return(psp->retcde);	/* and leave */
	}

	/* else a normal prev */
	/* get leaf node from in-memory buffers */
	psp->speednod = nodereadsp(psp, getndenum(psp, psp->keyn));
	if (!psp->speednod)
		return(psp->retcde = NODENOTINMEM);		/* error - can only do this if in memory */

	psp->retcde = getprevsp(psp, psp->keyn);
	if (psp->retcde == CCOK)
	{
		if (psp->speedentptr == 0)    /* if no more */
		{   
		   /* set current index to one less than before */
		   putcurindx(psp, psp->keyn, (getcurindx(psp, psp->keyn) - 1));  
		   setnull(psp);		/* null data */
		}
		else	/* else there is a key */
		{
		   cmovedata(psp->speedentptr, psp);							/* move in data */
		   putcurindx(psp, psp->keyn, gtkeyn(psp->speedentptr));	/* current index */
		   setcurkey(psp, psp->speedentptr);							/* MU */
		   psp->retcde = CCOK;									/* good return */
		}
	}

#ifndef NO_WORD_CONVERSION
	if (psp->key != psp->retkey)
		cipspflip(psp, INTELFMT);		/* reset order of existing key */
#endif	/* !NO_WORD_CONVERSION */
	psp->key = psp->retkey;								/* set for caller */
	psp->keyorder = psp->wordorder;
	putdelflag(psp, psp->keyn, DELOFF);				/* del flag now clear */
	return(psp->retcde);
}

int FDECL checkpsp(psp)		/* validates psp struct */
struct passparm *psp;
/* passes back INVALID, PSPOPEN or PSPCLOSED */
{
	if (	psp->check1 != 'V' 
		|| psp->check2 != 'A' 
		|| psp->check3 != 'L' 
		||	psp->check4 != 'I' 
		|| psp->check5 != 'D' 
		|| psp->check6 != 'A' 
		|| psp->check7 != 'T' 
		|| psp->check8 != 'E' 
		|| (psp->status != PSPOPEN && psp->status != PSPCLOSED)) /* not valid */
	   return(INVALID);
	return((int) psp->status);
}
	

void FDECL setpsp(psp, status)		/* set psp structure */
struct passparm *psp;
int status;
{
	psp->check1 = 'V';
	psp->check2 = 'A';
	psp->check3 = 'L';
	psp->check4 = 'I';
	psp->check5 = 'D';
	psp->check6 = 'A';
	psp->check7 = 'T';
	psp->check8 = 'E';
	psp->status = (char)status;
}


int FDECL cigetdata(psp, p, len)  	/* gets data after a find, puts in p */
register struct passparm *psp;	/* psp struct */
char *p;	/* where to put data */
int len;	/* max len. returns -1 if data exceeds this */
{
	int tlen;	/* for keeping track of current length */
	int tkeynum;
	NDEPTR tndenum;
	int tcurindx;
	int tret;
	char *tkey;
	char tretkey[DATAAREA];		/* temps */
	INT32 tkeyorder;

	int dflag;		/* if got all the data */
	int oldbyte;	/* old data byte for comparison */
	int sequence;	/* good (0) or bad (-1) seq of data */

	/* MU */
	long trecval;			/* save current record val */
	int tdbval;				/* save current data byte val */
	char tempkey[MAXMKLEN];	/* save current key val */
	int ret;
	int cc;

	/**/

	/* reset to normal key comparison (in case first/last search) */
	psp->srchorder = 0;

	if (!psp->dlen || !len)		/* no data */
	{
		psp->retlen = 0;			/* always report found len */
	   return(CCOK);					/* good return */
	}

	if (strtread(psp) != CCOK)
	   return(FILELOCKED);

	/* save important information */
	/* MU */
	getcurkey(psp, psp->keyn, tempkey);
	trecval = getcurrcval(psp, psp->keyn);
	tdbval  = getcurdbyte(psp, psp->keyn);
	tkeynum = getkeynum(psp, psp->keyn);
	tndenum = getndenum(psp, psp->keyn);
	tcurindx = getcurindx(psp, psp->keyn);
	tret = psp->retcde;
	tkey = psp->key;
	blockmv(tretkey, psp->retkey, DATAAREA);	/* save ret key */
	tkeyorder = psp->keyorder;


	if (psp->dbyte & 127)		/* if not first part of data, incomplete */
		sequence = -1;				/* start bad */
	else
		sequence = 0;				/* start good */

	dflag = 1;		/* assume good */
	tlen = 0;		/* no data yet */
	while (psp->retcde > -1) 
	{
	   tlen += psp->dlen;	/* add data here */
	   if (dflag)		/* still adding data */
	   {
	      if (tlen > len) 	/* exceeded available length */
	      {
				/* move in what we can */
	         blockmv(p, psp->data, (unsigned int)(psp->dlen - (tlen-len)));	
	         dflag = 0;		/* overflow */
	      }
	      else		/* else move in all */
	         blockmv(p, psp->data, psp->dlen);
	   }
	   if (psp->dbyte > 127)	/* if last portion */
	      break;			/* stop */
	   p += psp->dlen;		/* else advance data ptr */
    	   oldbyte = psp->dbyte;	/* set oldbyte */
	   nextkey(psp); 		/* get next key */
	   if ((psp->dbyte&127) != oldbyte + 1)  /* if not next one */
  	      sequence = -1;	/* now bad */
	}

#ifdef REMOVED
	/* restore important information */
	psp->keynum[psp->keyn] = tkeynum;
	psp->ndenum[psp->keyn] = tndenum;
	psp->curindx[psp->keyn] = tcurindx;
#endif
	/* restore important information */
	putkeynum(psp, psp->keyn, tkeynum);
	putndenum(psp, psp->keyn, tndenum);
	putcurindx(psp, psp->keyn, tcurindx);
	psp->retcde = tret;

	if (psp->key != tkey)
	{
#ifndef NO_WORD_CONVERSION
		cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
		psp->key = tkey;
	}
	blockmv(psp->retkey, tretkey, DATAAREA);
	psp->keyorder = tkeyorder;

	/* MU */
	putcurrcval(psp, psp->keyn, trecval);			/* restore current info */
	putcurdbyte(psp, psp->keyn, tdbval);
	putcurkey(psp, psp->keyn, tempkey);

	psp->dlen = (dflag) ? tlen : len;   /* if over, use max len */
	psp->retlen = tlen;		/* always report found len */
	cc = endread(psp);				/* release file after read */
	if (cc != CCOK)
		ret = cc;
	else
		ret = (sequence == 0) ? CCOK : INCOMPLETE;

	return(ret);
}

	
/* gets data after a find, puts in p - speed read method */
int FDECL cigetdatasp(psp, p, len)
register struct passparm *psp;	/* psp struct */
char *p;	/* where to put data */
int len;	/* max len. returns -1 if data exceeds this */
{
	int tlen;	/* for keeping track of current length */
	int tkeynum;
	NDEPTR tndenum;
	int tcurindx;
	int tret;
	char *tkey;
	char tretkey[DATAAREA];		/* temps */
	INT32 tkeyorder;

	int dflag;		/* if got all the data */
	int oldbyte;	/* old data byte for comparison */
	int sequence;	/* good (0) or bad (-1) seq of data */

	/* MU */
	long trecval;			/* save current record val */
	int tdbval;				/* save current data byte val */
	char tempkey[MAXMKLEN];	/* save current key val */
	int ret;
	int cc;

	/**/

	/* reset to normal key comparison (in case first/last search) */
	psp->srchorder = 0;

	if (!psp->dlen || !len)		/* no data */
	{
		psp->retlen = 0;			/* always report found len */
	   return(CCOK);					/* good return */
	}

	/* save important information */
	/* MU */
	getcurkey(psp, psp->keyn, tempkey);
	trecval = getcurrcval(psp, psp->keyn);
	tdbval  = getcurdbyte(psp, psp->keyn);
	tkeynum = getkeynum(psp, psp->keyn);
	tndenum = getndenum(psp, psp->keyn);
	tcurindx = getcurindx(psp, psp->keyn);
	tret = psp->retcde;
	tkey = psp->key;
	blockmv(tretkey, psp->retkey, DATAAREA);	/* save ret key */
	tkeyorder = psp->keyorder;


	if (psp->dbyte & 127)		/* if not first part of data, incomplete */
		sequence = -1;				/* start bad */
	else
		sequence = 0;				/* start good */

	dflag = 1;		/* assume good */
	tlen = 0;		/* no data yet */
	while (psp->retcde > -1) 
	{
	   tlen += psp->dlen;	/* add data here */
	   if (dflag)		/* still adding data */
	   {
	      if (tlen > len) 	/* exceeded available length */
	      {
				/* move in what we can */
	         blockmv(p, psp->data, (unsigned int)(psp->dlen - (tlen-len)));	
	         dflag = 0;		/* overflow */
	      }
	      else		/* else move in all */
	         blockmv(p, psp->data, psp->dlen);
	   }
	   if (psp->dbyte > 127)	/* if last portion */
	      break;			/* stop */
	   p += psp->dlen;		/* else advance data ptr */
    	   oldbyte = psp->dbyte;	/* set oldbyte */
	   cc = nextkeysp(psp); 		/* get next key */
		if (cc == NODENOTINMEM)
			return(cc);					/* error getting nodes from memory buffers */

	   if ((psp->dbyte&127) != oldbyte + 1)  /* if not next one */
  	      sequence = -1;	/* now bad */
	}

	/* restore important information */
	putkeynum(psp, psp->keyn, tkeynum);
	putndenum(psp, psp->keyn, tndenum);
	putcurindx(psp, psp->keyn, tcurindx);
	psp->retcde = tret;
	if (psp->key != tkey)
	{
#ifndef NO_WORD_CONVERSION
		cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
		psp->key = tkey;
	}
	blockmv(psp->retkey, tretkey, DATAAREA);
	psp->keyorder = tkeyorder;

	/* MU */
	putcurrcval(psp, psp->keyn, trecval);			/* restore current info */
	putcurdbyte(psp, psp->keyn, tdbval);
	putcurkey(psp, psp->keyn, tempkey);

	psp->dlen = (dflag) ? tlen : len;   /* if over, use max len */
	psp->retlen = tlen;		/* always report found len */
	ret = (sequence == 0) ? CCOK : INCOMPLETE;

	return(ret);
}

int FDECL cispeedon(psp, readahead)
struct passparm PDECL *psp;
int readahead;
{
	int ret;

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	{
	   ret = PSPERR;
	}
	else
	{
		ret = initbcb(psp);							/* init buffer bcbs if needed */
		if (ret == CCOK)
		{
			/* limit read ahead to number of avail buffers */
			if (readahead >= bcbnum)
				psp->speedahead = bcbnum -1;
			else
					psp->speedahead = readahead;

			psp->speedread = TRUE;
		}
	}

	return(ret);
}


int FDECL cispeedoff(psp)
struct passparm PDECL *psp;
{
	int ret;

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	{
	   ret = PSPERR;
	}
	else
	{
		psp->speedahead = 0;
		psp->speedreq = 0;
		psp->speedread = FALSE;
		ret = CCOK;
	}

	return(ret);
}

	
