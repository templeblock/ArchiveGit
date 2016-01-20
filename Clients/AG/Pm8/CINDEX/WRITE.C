 /*  write.c - cindex base code source file for writing functions
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

/* scans index node and returns key pointer - place to insert */
ENTRY * FDECL scanikey(psp, ndea, leftnode)
struct passparm *psp;	/* file info about keys */
struct node *ndea;		/* node to scan */
NDEPTR leftnode;			/* node number of node a below */
{
	int i;					/* loop counter */
	char *ptr;				/* current key */
	NDEPTR nptr;						/* current node pointer */

	ptr = ndea->keystr;	/* ptr to first key */
	nptr = ndea->p0;	/* set to node pointer */
		
	for (i = 0;i < ndea->count;i++) 
	{
	   if (nptr == leftnode)					/* look for old node pointer */ 
	      break;									/* found it */

	   nptr = gtnptr(psp, (ENTRY *)ptr);	/* set to node pointer of key */
	   ptr = moveup(ptr);								/* and point to next key */
	}
	return((ENTRY *)ptr);
}

#ifdef REMOVED
/* compression version of findsplt */
/* find high key location in split and set key count of node a  */
char * FDECL findsplt(psp, ndea, cnt)
struct passparm *psp;
struct node *ndea;		/* node to scan */
int *cnt;			/* where to put count */
{
	register int i;
	register char *p;
	register char *oldp;
	int split;					/* set limit (last node has higher slpit limit) */
	char *splitest;

	psp;		/* do-nothing to fool compiler into thinking we use psp */


	split = ((KEYAREA * MAXCOMP) - ndea->free) / 2;	/* halfway split point */
	p = ndea->keystr;											/* start with first key */
	i = 0;
	oldp = NULL;
	splitest = moveup(p);

	/* loop until past split limit.
	 * limit is one less than count for end nodes
    * and half of data for middle of the tree nodes
	 */
	while (	((ndea->forwrd == -1) && (i < (ndea->count-1)))
			|| ((ndea->forwrd != -1) && (splitest - ndea->keystr < split)))
	{
	   oldp = p;	/* remember for high key set */
	   p = moveup(p);	/* move pointer to next key */
	   splitest = moveup(p);
	   i++;
	}

	*cnt = i;		/* set count - one more for hkey */
	return(oldp);		/* high key pointer */
}

#endif	/* REMOVED */

#ifdef COMPRESSION
/* compression version of findsplt */
/* find high key location in split and set key count of node a  */
char * FDECL findsplt(psp, ndea, cnt)
struct passparm *psp;
struct node *ndea;		/* node to scan */
int *cnt;			/* where to put count */
{
	register int i;
	register char *p;
	register char *oldp;
	int split;					/* set limit (last node has higher slpit limit) */
	char *splitest;
	int cursize;				/* size of data in keystr */

   /* if last node of tree want to make just fit, else cut in half */
	cursize = (KEYAREA * MAXCOMP) - ndea->free;
	split = (ndea->forwrd == -1)	
					? cursize - (COMPMARGIN - ndea->compfree) 	/* equiv. to ENDLMT */
					: cursize / 2;								/* equiv. to SPLITLMT */
	if (split > ENDLMT)
		split = ENDLMT;			/* make sure does not exceed end of node */

	p = ndea->keystr;		/* start with first key */
   
	i = 0;
	oldp = NULL;
	splitest = moveup(p);
	while (splitest - ndea->keystr < split) /* until past limit */ 
	{
	   oldp = p;	/* remember for high key set */
	   p = moveup(p);	/* move pointer to next key */
	   splitest = moveup(p);
	   i++;
	}

	*cnt = i;		/* set count - one more for hkey */
	return(oldp);		/* high key pointer */
}
#else		/* !COMPRESSION */
/* non-compression version of findsplt */
/* find high key location in split and set key count of node a  */
char * FDECL findsplt(psp, ndea, cnt)
struct passparm *psp;
struct node *ndea;		/* node to scan */
int *cnt;			/* where to put count */
{
	register int i;
	register char *p;
	register char *oldp;
	int split;					/* set limit (last node has higher slpit limit) */
	char *splitest;

	/* split at end of file */
	split = (ndea->forwrd == -1)	? ENDLMT : SPLITLMT;
   /* if not last want to cut exactly in half */
	p = ndea->keystr;		/* start with first key */
   
	i = 0;
	oldp = NULL;
	splitest = moveup(p);
	while (splitest - ndea->keystr < split) /* until past limit */ 
	{
	   oldp = p;	/* remember for high key set */
	   p = moveup(p);	/* move pointer to next key */
	   splitest = moveup(p);
	   i++;
	}

	*cnt = i;		/* set count - one more for hkey */
	return(oldp);		/* high key pointer */
}
#endif	/* COMPRESSION */


/* adds data to node at p, adjusts node count and free space */
void FDECL addata(data, ndea, p)
ENTRY *data;							/* where key is */
struct node *ndea;					/* node to insert into */
ENTRY *p;								/* where to put the key */
{
	int bytes;	/* number of bytes to move */
	register int dlen;
	char *kend;	/* pointer to end of keyarea */

	dlen = gtentlen(data) + 1;								/* total len of entry */
	kend = ndea->keystr + (unsigned int)((KEYAREA * MAXCOMP) - ndea->free);   /* end of key area */
#ifdef REMOVED
	kend = ndea->keystr + (unsigned int)(KEYAREA - ndea->free);   /* end of key area */
#endif
 
	bytes = (int)(kend - (char *)p);						/* amount to move */
	cmovert((char *)p, bytes, dlen);						/* make room */
	blockmv((char *)p, (char *)data, dlen);			/* put in data */
	ndea->count++;												/* increment key count */
	ndea->free -= (INT16)dlen;								/* calc free space */
#ifdef COMPRESSION
	ndea->compfree -= dlen;
#endif	/* COMPRESSION */
}


int FDECL hdrupdte(psp)		/* updates header */
register struct passparm *psp;
{
	int ret = CCOK;
	struct hdrrec hdr;
	INT16 flag;

	/* first time write occurs in lazy write, set write flag */
	if ((psp->seclev >= 4) && psp->lazywrite && !psp->writeactive)
	{
#ifdef READSHARE
		/* if current read enabled, lock all read bytes before writes */
		if (psp->processmode != EXCL)
			ret = cireadlock(psp, TRUE);	/* lock all read bytes */
#endif		/* READSHARE */
		if (!ret)
		{
			ret = fileSeek(psp, (long)WRTBYTE, 0, 0L);
			if (ret != CCOK)	/* seek error */
			{
			   ret = DSKERR;
			}
			else
			{
				/* set flag in case of bad write */
				flag = SETUPD;
				ret = ciwrite(psp, (char *)&flag, sizeof(flag));

				if (!ret)
				{
					ret = osbflush(psp);
					psp->writeflag = TRUE;	/* set flag indicating writing to disk */
				}

				psp->writeactive = TRUE;
			}
		}
	}
	else
	{
		ret = CCOK;
	}

	if (!ret)
	{
		hdr.updatnum = psp->pupdatnum;	/* keep current */
		if (!hdr.updatnum)					/* wrap count around, skipping 0 */
			hdr.updatnum = psp->pupdatnum = 1L;

		sethdr(psp, &hdr);	/* move current info into header */

		ret = hdrwrte(psp, &hdr);	/* and write to disk */
		if (ret == CCOK)
			psp->updtflag = CLEAN;	/* now clean */
	}

	return(ret);	/* MU added in development */
}
		

NDEPTR FDECL gtnwnde(psp)		/* gets new node number from file */
register struct passparm *psp;
{
	NDEPTR newnext;	/* get current node num before incrementing */
	int cc;

	/**/

#ifdef RSP
	if (psp->freenodes != -1)
	{
		newnext = psp->freenodes;					/* pick up new node number */
		cc = noderead(psp, &nodeb, newnext);	/* read top free node */
		if (cc == CCOK)
			psp->freenodes = nodeb.forwrd;		/* next free node is top */
	}
	else
	{
		cc = CCOK;
		newnext = psp->next;				/* pick up new node number */
		psp->next++;						/* increment */
	}

	if (	(psp->seclev >= HDRUPLEV)
		&& (psp->seclev < 4) 
		&& (cc == CCOK))
	{
	   cc = hdrupdte(psp);	/* need to keep header current */
	}
	else		
	{
		cc = CCOK;
	   psp->updtflag = DIRTY;	/* else now dirty */
	}

#else

	newnext = psp->next;				/* pick up new node number */
	psp->next++;						/* increment */
	if ((psp->seclev >= HDRUPLEV)
		 && (psp->seclev < 4))
	{
	   cc = hdrupdte(psp);	/* need to keep header current */
	}
	else		
	{
		cc = CCOK;
	   psp->updtflag = DIRTY;	/* else now dirty */
	}
#endif		/* RSP */


	if (cc != CCOK)
		newnext = -1;				/* error in writing header */

	return(newnext);	/* and return next node */
}


#ifdef RSP
/* set info of node being freed and set header free chain value */
int FDECL sethfree(psp, ndea, freende)
register struct passparm *psp;
struct node *ndea;							/* ptr to node that is being freed */
NDEPTR freende;								/* node # to set on free chain */
{
	int cc;

	/**/

	cc = CCOK;								/* start with ok */
	ndea->level = -1;						/* clear level */
	ndea->p0 = -1;							/* clear p0 */
	ndea->revrse = -1;					/* clear reverse */
	ndea->forwrd = psp->freenodes;	/* connect to free chain */
	psp->freenodes = freende;

	if (	(psp->seclev >= HDRUPLEV)
		&& (psp->seclev < 4) 
		&& (cc == CCOK))
	{
	   cc = hdrupdte(psp);	/* need to keep header current */
	}
	else		
	{
		cc = CCOK;
	   psp->updtflag = DIRTY;	/* else now dirty */
	}

	return(cc);
}
#endif		/* RSP */


/* returns pointer to high key area */
ENTRY * FDECL creatnab(psp, ndea, ndeb) /* creates node a and new node b */
struct passparm *psp;
struct node *ndea;	/* pointer to node a - holds all keys now */
struct node *ndeb;	/* where to put node b */
{
	int cnt;		/* key count */
	unsigned int bytesa, bytesb;	/* bytes left in each of nodes after move */
	register char *ptr;	/* split location */
	char *hkey;				/* high key location */
	char *bptr;
#ifdef REMOVED
	char *xptr;				/* extra pointer for clearing space */
#endif
#ifdef COMPRESSION
	int compfact;			/* compression factor for current node */

	/* multiplier rate of data before compression */
	if (ndea->compfree != KEYAREA)
	{
		compfact = ((KEYAREA * MAXCOMP) - ndea->free) / (KEYAREA - ndea->compfree);
		if (compfact < 1)
			compfact = 1;
	}
	else
		compfact = 1;

#endif

	hkey = findsplt(psp, ndea, &cnt);  /* find hkey loc - set cnt */
	ptr = hkey + gtentlen(((ENTRY *)hkey)) + 1;	/* split loc */ 
	bytesa = (unsigned int)(ptr - ndea->keystr);				/* set bytes really in a */
#ifdef REMOVED
	bptr = (ndea->keystr + KEYAREA);
#endif
	bptr = (ndea->keystr + (KEYAREA * MAXCOMP));
	bytesb = (unsigned int)(bptr - ptr);
	bytesb -= ndea->free;  							/* bytes in b */
#ifdef REMOVED
	cclear(ndeb->keystr, KEYAREA, 0);			/* clear b */	
#endif
	cclear(ndeb->keystr, (KEYAREA * MAXCOMP), 0);			/* clear b */	
	blockmv(ndeb->keystr, ptr, bytesb);			/* move b */
	ndeb->count = (INT16)(ndea->count - cnt);	/* set node b count */
	ndea->count = (INT16)cnt;						/* set node a count */
#ifdef REMOVED
	ndea->free = KEYAREA - bytesa;				/* set free in a */
	ndeb->free = KEYAREA - bytesb;				/* set free in b */
#endif
	ndea->free = (INT16)((KEYAREA * MAXCOMP) - bytesa);	/* set free in a */
	ndeb->free = (INT16)((KEYAREA * MAXCOMP) - bytesb);	/* set free in b */
#ifdef COMPRESSION
	/* set compressed free space to estimate based on compression rate */
	ndea->compfree = KEYAREA - (bytesa / compfact);
	ndeb->compfree = KEYAREA - (bytesb / compfact);
#endif

	if (ndea->level)											/* if index node */
	   ndeb->p0 = gtnptr(psp, (ENTRY *)hkey);			/* set b p0 ptr */
	else															/* else */
	   ndeb->p0 = 0;											/* set b p0 to 0 */

	/* clear area past end of disk node (CI_BIGBUF extra space) */
#ifdef REMOVED
X	xptr = (char *)ndea;
X	xptr += sizeof(struct smallnode);
X	cclear(xptr, (CI_BIGBUF - (KEYAREA * MAXCOMP)), 0);
X	cclear(ptr, ndea->free, 0);					/* clear rest of node a */
#endif
	cclear(ptr, ndea->free+KEYSZE+1, 0);	/* clear rest of node a */

	ndeb->level = ndea->level;						/* make b level like a level */
	return((ENTRY *)hkey);							/* return high key pointer */
}	
		

void FDECL creatrt(ndea)		/* create new root in nde */
register struct node *ndea;	/* where to put root */
{
	cclear(ndea, sizeof(struct node), 0);		/* clear node */
	ndea->count = 0;		/* no keys */
	ndea->forwrd = ndea->revrse = -1;  			/* null forward pointers = -1 */
	ndea->free = (KEYAREA * MAXCOMP);			/* all free */
#ifdef COMPRESSION
	ndea->compfree = KEYAREA;						/* compressed free space */
#endif
}
		

/* reads node in next level up - creates if nec */
NDEPTR FDECL readup(psp, ndea)
register struct passparm *psp;
register struct node *ndea;	/* where to read into */
{
	NDEPTR newnde;
	int ret;
	int cc;

	/**/

	if (ndea->level == (gtlevels(psp) - 1))    /* if root split */
	{	/* then must make new root */
	   newnde = gtnwnde(psp);	/* get next node */

		if (newnde != -1)
		{
			creatrt(ndea);							/* make new root */
		   ndea->level = (INT16)gtlevels(psp);		/* level of this node */
			ptlevels(psp, ndea->level+1);		/* incr levels */
		   ndea->p0 = gtroot(psp, -1);		/* set p0 */
		   ptroot(psp, newnde);					/* set root ptr */

		   ret = nodewrte(psp,ndea,newnde); 	/* extend file */
			if (ret != CCOK)
			{
				newnde = -1;				/* disk error */
			}
			else
			{
				if ((psp->seclev >= HDRUPLEV)
					 && (psp->seclev < 4))
				{
			      ret = hdrupdte(psp);   /* sec lev says update */
					if (ret != CCOK)
						newnde = -1;				/* disk error */
				}
				else
				   psp->updtflag = DIRTY;	/* else now header dirty */
			}
		}
	}
	else	/* otherwise just read upper node */
	{
		newnde=psp->path[ndea->level+1];
	   cc = noderead(psp,ndea,newnde);
		if (cc != CCOK)
			newnde = -1;				/* error */
	}

	return(newnde);		/* return node number of node read */
}

int FDECL insrtkey(data, ndea, entptr, nde, psp, knum)
ENTRY *data;							/* key to add */
register struct node *ndea;		/* pointer to node to insert in */
ENTRY *entptr;							/* where to put it */ 
NDEPTR nde;								/* node number of node to insert into */
register struct passparm *psp;	/* file to use */
int knum;								/* key number */
{
	NDEPTR newnde;
	int sflag;
	ENTRY *hkey;
	int ret;
	NDEPTR lowernde;
#ifdef COMPRESSION
	int compfact;			/* compression factor for current node */
#endif

	sflag = 0;										/* not split yet */
	addata(data, ndea, entptr);				/* add data to node */
	putcurindx(psp, psp->keyn, psp->keyn);	/* set current index */
	putkeynum(psp, psp->keyn, knum);			/* set key num */
	setcurkey(psp, entptr);					/* set current key and record num */
	ciadjust(psp, ADDJST);					/* adjust current ptrs */
	ret = CCOK;													/* assume ok return */

#ifdef COMPRESSION
	while (((ndea->free < 0) || (ndea->compfree < COMPMARGIN))
			&& (ret == CCOK)) 		/* while need to split */
	{
		/* if uncompressed size is less than compressed data area, must be ok */
		if (((KEYAREA * MAXCOMP) - ndea->free) < COMPAREA)
			break;

		/* if might not be out of space, try compressing first */
		if (ndea->free > 0)
		{
			/* rate of data compression */
			if (ndea->compfree != KEYAREA)
			{
				compfact = ((KEYAREA * MAXCOMP) - ndea->free) / (KEYAREA - ndea->compfree);
				if (compfact < 1)
					compfact = 1;
			}
			else
				compfact = 1;

			if (compfact < MINCOMP)
			{
				/* try shrinking node again */
				ndea->free = stointel(ndea->free, psp->wordorder);
				ret = cishrink(psp, &nodec, ndea);
				ndea->free = inteltos(ndea->free, psp->wordorder);
				ndea->compfree = inteltos(nodec.compfree, psp->wordorder);
				if (ndea->compfree > COMPMARGIN)	/* if compressed enough */
					break;						/* don't need to split now */
			}
		}
#else
	while ((ndea->free < 0) && (ret == CCOK)) 		/* while need to split */
	{
#endif	/* COMPRESSION */

		ret = ciextend(psp);					/* try to extend file before doing split */
	   if (!sflag && ret != CCOK)			/* if cant extend */
			break;								/* stop processing */

	   newnde = gtnwnde(psp);				/* get new node */
		if (newnde == -1)
		{
			ret = DSKERR;						/* disk error getting new node */
		}
		else
		if (newnde > psp->next)
		{
			ret = BADHDR;						/* header error getting new node */
		}
		else
		{
			/* make nodes a and b */
		   /* creates two new nodes - sets hkey ptr */
			hkey = creatnab(psp, ndea, &nodeb);

		   nodeb.forwrd = ndea->forwrd;		/* new forward */
		   ndea->forwrd = newnde;		/* point to b */
		   nodeb.revrse = nde;			/* pt back to a */
		   if (!sflag)				/* if at leaf level */
		      splitadj(psp, nde, newnde, ndea->count);  
		   /* adjust current pointers */
 
		   ret = nodewrte(psp, &nodeb, newnde); 	/* write node b */
			if (ret == CCOK)
				ret = nodewrte(psp, ndea, nde); 		/* write node a */

#ifdef COMPRESSION
			/* make sure nodea and nodeb have current compfree */
			if ((ret == CCOK) && (ndea->compfree <= 0))
			{
				ndea->free = stointel(ndea->free, psp->wordorder);
				ret = cishrink(psp, &nodec, ndea);
				ndea->free = inteltos(ndea->free, psp->wordorder);
				ndea->compfree = inteltos(nodec.compfree, psp->wordorder);
			}

			if ((ret == CCOK) && (nodeb.compfree <= 0))
			{
				nodeb.free = stointel(nodeb.free, psp->wordorder);
				ret = cishrink(psp, &nodec, &nodeb);
				nodeb.free = inteltos(nodeb.free, psp->wordorder);
				nodeb.compfree = inteltos(nodec.compfree, psp->wordorder);
			}
#endif	/* COMPRESSION */

			if (ret == CCOK)
			{
				bldikey(psp, hkey, newnde, data, psp->wordorder);	/* make high key */

			   if (nodeb.forwrd > -1)  	/* if need to fix reverse pointer */
			   {  
			      ret = noderead(psp, ndea, nodeb.forwrd); /* read forward node */  
					if (ret != CCOK)
						break;

			      ndea->revrse = newnde;   		/* set reverse pointer */
			      ret = nodewrte(psp, ndea, nodeb.forwrd); 	/* and write node */ 
					if (ret != CCOK)
						break;
			   }  

				lowernde = nodeb.revrse;		/* save node number of nodea */

			   nde = readup(psp, ndea);		/* get level above */
				if (nde == -1)
				{
					ret = DSKERR;
					break;
				}

				/* find place to insert high key */
			   entptr = scanikey(psp, ndea, lowernde);
			   addata(data, ndea, entptr);		/* add data to node */
			   sflag = 1;			/* now have split */
			}
		}
	}				/* and do again if neccessary */

	if (ret == CCOK)
	{
		ret = nodewrte(psp, ndea, nde);	/* done - write final node */
		if (sflag)								/* altered header ? */
		   psp->updtflag = DIRTY;			/* header now dirty */
	}

	return(ret);
}   
			 			


/* insert index level key */
int FDECL insrtikey(data, ndea, entptr, nde, psp, sflag)
ENTRY *data;							/* key to add */
register struct node *ndea;		/* pointer to node to insert in */
ENTRY *entptr;							/* where to put it */ 
NDEPTR nde;								/* node number of node to insert into */
register struct passparm *psp;	/* file to use */
int sflag;								/* TRUE == already split past leaf node */
{
	NDEPTR newnde;
	ENTRY *hkey;
	int ret;
	NDEPTR lowernde;
#ifdef COMPRESSION
	int compfact;			/* compression factor for current node */
#endif

	ret = CCOK;													/* assume ok return */

#ifdef COMPRESSION
	while (((ndea->free < 0) || (ndea->compfree < COMPMARGIN))
			&& (ret == CCOK)) 		/* while need to split */
	{
		/* if uncompressed size is less than compressed data area, must be ok */
		if (((KEYAREA * MAXCOMP) - ndea->free) < KEYAREA)
			break;

		/* if might not be out of space, try compressing first */
		if (ndea->free > 0)
		{
			/* rate of data compression */
			if (ndea->compfree != KEYAREA)
			{
				compfact = ((KEYAREA * MAXCOMP) - ndea->free) / (KEYAREA - ndea->compfree);
				if (compfact < 1)
					compfact = 1;
			}
			else
				compfact = 1;

			if (compfact < MINCOMP)
			{
				/* try shrinking node again */
				ndea->free = stointel(ndea->free, psp->wordorder);
				ret = cishrink(psp, &nodec, ndea);
				ndea->free = inteltos(ndea->free, psp->wordorder);
				ndea->compfree = inteltos(nodec.compfree, psp->wordorder);
				if (ndea->compfree > COMPMARGIN)	/* if compressed enough */
					break;								/* don't need to split now */
			}
		}
#else
	while ((ndea->free < 0) && (ret == CCOK)) 		/* while need to split */
	{
#endif
		ret = ciextend(psp);					/* try to extend file before doing split */
	   if (!sflag && ret != CCOK)			/* if cant extend */
			break;								/* stop processing */

	   newnde = gtnwnde(psp);				/* get new node */
		if (newnde == -1)
		{
			ret = DSKERR;						/* disk error getting new node */
		}
		else
		if (newnde > psp->next)
		{
			ret = BADHDR;						/* header error getting new node */
		}
		else
		{
			/* make nodes a and b */
		   /* creates two new nodes - sets hkey ptr */
			hkey = creatnab(psp, ndea, &nodeb);

			/* in case called from delete, save path to high key side of split */
			if (hkey <= entptr)			/* <------- NEW release 4.0H */
				psp->path[nodea.level] = newnde;

		   nodeb.forwrd = ndea->forwrd;		/* new forward */
		   ndea->forwrd = newnde;		/* point to b */
		   nodeb.revrse = nde;			/* pt back to a */
		   if (!sflag)				/* if at leaf level */
		      splitadj(psp, nde, newnde, ndea->count);  
		   /* adjust current pointers */
 
		   ret = nodewrte(psp, &nodeb, newnde); 	/* write node b */
			if (ret == CCOK)
				ret = nodewrte(psp, ndea, nde); 		/* write node a */

			if (ret == CCOK)
			{
				bldikey(psp, hkey, newnde, data, psp->wordorder);	/* make high key */

			   if (nodeb.forwrd > -1)  	/* if need to fix reverse pointer */
			   {  
			      ret = noderead(psp, ndea, nodeb.forwrd); /* read forward node */  
					if (ret != CCOK)
						break;

			      ndea->revrse = newnde;   		/* set reverse pointer */
			      ret = nodewrte(psp, ndea, nodeb.forwrd); 	/* and write node */ 
					if (ret != CCOK)
						break;
			   }  

				lowernde = nodeb.revrse;		/* save node number of nodea */

			   nde = readup(psp, ndea);		/* get level above */
				if (nde == -1)
				{
					ret = DSKERR;
					break;
				}

				/* find place to insert high key */
			   entptr = scanikey(psp, ndea, lowernde);
			   addata(data, ndea, entptr);		/* add data to node */
			   sflag = TRUE;			/* now have split */
			}
		}
	}				/* and do again if neccessary */

	if (ret == CCOK)
	{
		ret = nodewrte(psp, ndea, nde);	/* done - write final node */
		if (sflag)								/* altered header ? */
		   psp->updtflag = DIRTY;			/* header now dirty */
	}

	return(ret);
}   
			 			



int FDECL addkey(psp, flag)				/*  add key */
register struct passparm *psp;	/* file header */
int flag;								/* add, change, or save */
{
	ENTRY *ptr;
	int ret;
	char kdata[KEYSZE];
	char keybuf[KEYSZE];				/* buffer to save deleted key value */
	int knum;							/* key position offset */

	/**/

	ret = CCOK;
	/* search for place to put key */
#ifndef NO_WORD_CONVERSION
	cipspflip(psp, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	ptr = treesrch(psp, psp->keyn, psp->key, psp->rec, psp->dbyte);
	if (!ptr)
	   return(psp->retcde = BADNODE);
		
	knum = getkeynum(psp, psp->keyn);	/* key position offset */

	if ((psp->dbyte & 127) == 0) 	/* if first portion */
	{	
	   switch (flag) 
	   {
	   case DUP:		/* add duplicate key */
			/* see what we found */
	      if (psp->keycmpres == 0) 	/* if key already exists */
	         ret = FAIL;		/* cant do that */
	      break;

	   case UNQ:		/* add unique key */
	      break;		/* already have checked for match at higher level */

	   case CHGCUR:		/* change existing current key */
	   case CHANGE:		/* change existing key */
	   case SAVE:			/* add or change */
			/* see what we found */
	      if ((psp->keycmpres != 0) && (flag != SAVE))	/* key not found */
	         return(psp->retcde = FAIL);			/* error for change */
	      else 		/* key found, delete existing */
			{
		      if ((psp->keycmpres != 0) && (flag == SAVE))	/* key not found */
					break;

				/* delete entry */
				cikeycpy(psp, psp->keyn, keybuf, gtkeyptr(ptr));	/* save full key for treesrch */
				ret = kysdel(psp, ptr, getndenum(psp, psp->keyn));
				if (ret == -1)			/* if error */
				{
					ret = DSKERR;
				}
				else
				{
					/* if deleted position, search again for place to put key */
					if (psp->fulldel && (psp->treeindex != psp->keyn))
					{
						ptr = treesrch(psp, psp->keyn, keybuf, psp->rec, 0);
						if (!ptr)
						   ret = BADNODE;
						else
							ret = CCOK;

						knum = getkeynum(psp, psp->keyn);	/* key position offset */
					}
					else /* if changed nodes during delete, re-read original node */
					{
						knum = getkeynum(psp, psp->keyn);	/* key position offset */
						knum++;							/* move back to deleted spot */

						if (ret == 1)
			            ret = noderead(psp, &nodea, getndenum(psp, psp->keyn));
						else
							ret = CCOK;
					}
				}

		      break;
			}
	   }		/* end of switch */
	}		/* end of if */

	/* make key */
#ifndef NO_WORD_CONVERSION
	cipspflip(psp, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	bldlkey(psp, (ENTRY *)kdata, psp->keyn, psp->key, psp->rec, psp->dbyte, psp->data, psp->dlen, psp->wordorder);

#ifdef TRANSACTION
	if (ret == CCOK)
	{
		ret = translog(psp, TRANSADD, kdata);
	}
#endif	/* TRANSACTION */

	if (ret == CCOK)
	{
		/* insert the new key */
		ret = insrtkey((ENTRY *)kdata,&nodea,ptr,getndenum(psp, psp->keyn),psp,knum);
		putdelflag(psp, psp->keyn, DELOFF);		/* delete flag now clear */
	}

	return(psp->retcde = ret);			/* return ret code */
}


int FDECL __cadd(psp, keyn, key, rec, data, dlen, flag)  /* high level add */
struct passparm *psp;
int keyn;
char *key;
long rec;
char *data;
int dlen;
int flag;		/* add, change or save */
{
	/* set for lower level routine */
	psp->keyn = keyn;
#ifndef NO_WORD_CONVERSION
	if (psp->key != key)
		cipspflip(psp, INTELFMT);				/* reset existing key value */
#endif	/* !NO_WORD_CONVERSION */
	psp->key = key;
	psp->rec = rec;
	psp->data = data;
	psp->dlen = dlen;
	return(___cadd(psp, flag));
}


int FDECL ___cadd(psp, flag) 	/* add data record - lowest high level */
register struct passparm *psp;
int flag;		/* add, change or save */
{
	int i;
	int dspace;
	int tl;		/* for save of data len */
	int tmpl;
	int ret;
#ifdef REMOVED
	char buf[256];
#endif
	char *td;
	char *tmpd;
	char *tp;
	INT32 tm;
	char *dend;
	int cc;
	char tempkey[MAXMKLEN];			/* temp key storage */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	if (	!goodindex(psp->keyn)
		|| !gooddlen(psp, psp->key, psp->dlen) 
		|| !goodklen(psp, psp->key))
	{
		cc = endwrit(psp);
		if (cc != CCOK)
			ret = cc;
		else
			ret = PARMERR;
	   return(psp->retcde = ret); 
	}

	ret = FAIL;				/* initialize return value to FAIL (did nothing) */

	/* always unlock current for any operation, except change */
	if (flag != CHGCUR)
	{
		cc = _mclrlck(psp, psp->keyn);
		if (cc != CCOK)
		{
			psp->alterr = endwrit(psp);	/* release file */
			return(psp->retcde = cc);		/* return error from _mclrlck */
		}
	}

	dend = ((char *)psp->data) + psp->dlen;	/* find end of data */
	psp->errcde = 0;	/* start out good */

	tp = psp->key;					/* save for caller */
	cikeycpy(psp, psp->keyn, tempkey, psp->key);		/* save key */
	tm = psp->keyorder;
	td = psp->data;				/* ditto */
	tl = psp->dlen;				/* "" */

	if (!psp->dlen) 						/* if no data */
	{											/* then just add on entry */	
		psp->dbyte = 128;					/* last data byte */
		ret = addkey(psp, flag);		/* add key */
		psp->data = td;					/* and restore */
		psp->key = tp;  
		if (psp->key)
			cikeycpy(psp, psp->keyn, psp->key, tempkey);	/* restore key in case in psp */
		psp->keyorder = tm;
		psp->dlen = tl;

	   cc = endwrit(psp);					/* unlock file */
		if (cc != CCOK && ret != DSKERR)
			ret = cc;

		return(ret);
	}

#ifdef REMOVED
	dspace = DATAPART - strlen(psp->key);
#endif
	dspace = DATAPART - cikeylen(psp, psp->keyn, psp->key);


	for (i = 0; (char *)psp->data < dend && !psp->errcde; i++)	
	{		/* while there is still a portion left to add */
	   psp->dbyte = i;		/* portion of data rec */
	   if ((dend - (char *)psp->data) > dspace) 	/* if not end of data */
	      psp->dlen = dspace;		/* max data space */
	   else 
	   {	/* else last portion */
	      psp->dlen = (int)(dend - (char *)psp->data);
	      psp->dbyte |= 128;	   /* high bit on */
	   } 

		tmpd = psp->data;				/* save data ptr */
		tmpl = psp->dlen;				/* save length */
	   ret = addkey(psp, flag);	/* add key */
		psp->data = tmpd;				/* restore */
		psp->dlen = tmpl;

	   if (ret != CCOK)					/* if error */
	      break;						/* end */

		/* advance ptr to next part of data */
		psp->data = (void *) (((char *)psp->data) + psp->dlen);
	}

	/* reposition if more than one data part has been successfully written */
	if ((ret == CCOK) && ((psp->dbyte & 127) > 0))
	{
		/* relocate to first portion of record */
		while (((ret = prevkey(psp)) > -1) && psp->dbyte & 127)
		      /* looking for first data portion */
		    ;

#ifdef NOTSURE
		if (ret == -1)
			ret = CCOK;
#endif
	}

	psp->key = tp;		/* restore */
	if (psp->key)
		cikeycpy(psp, psp->keyn, psp->key, tempkey);	/* restore key in case in psp */
	psp->keyorder = tm;
	psp->data = td;
	psp->dlen = tl;

	psp->errcde = ret;

	if (ret == NOSPACE)	/* ran out of space in add */
	{
	   cc = __cdlt(psp);		/* get rid of partial key */
		if ((cc != CCOK) && (cc != FAIL))
			ret = cc;
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);		   
}
	

/* add an index to an alternate root node, create new root if needed */
int EDECL addroot(psp, keyn, rootnum)
struct passparm PDECL *psp;					/* file to add to */
int keyn;											/* index to add to root */
int rootnum;										/* root number to use for root */
{
	int ret;
	NDEPTR newnde;
	int cc;

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	if ((rootnum < 1) || (rootnum > MAXROOTS))
		ret = PARMERR;
	else
		ret = CCOK;

	psp->curroot = rootnum;

	/* if need to create new root for this alternate root number */
	if ((!psp->rootlist[rootnum-1]) && (ret == CCOK))
	{
	   newnde = gtnwnde(psp);	/* get next node */
		if (newnde != -1)
		{
			ptlevels(psp, 1);						/* number of levels in this root */
			ptroot(psp, newnde);					/* set root ptr */
			ret = ciextend(psp);					/* extend file */
			if (ret == CCOK)
			{
				crtnwrt(&ebuffer);					/* make new root */
				ret = nodewrte(psp,&ebuffer,newnde); 	/* write new empty root */
			}
			else
			{
				ptlevels(psp, 0);						/* number of levels in this root */
				ptroot(psp, 0);						/* set root ptr */
			}
		}
		else
		{
			ret = FAIL;
		}
	}

	/* set index to use the alternate root number */
	if (ret == CCOK)
	{
		psp->rootnum[keyn-1] = (unsigned char)rootnum;
	   ret = hdrupdte(psp);   /* update changes to roots */
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);
}


/* drop an alternate root, include all indexes and entries */
/* puts all dropped nodes into the deleted node list */
int EDECL droproot(psp, rootnum)
struct passparm PDECL *psp;					/* file to add to */
unsigned int rootnum;							/* root number to use for root */
{
	NDEPTR nde;		/* node number of root */
	NDEPTR fwdnde;		/* node number of root */
	int levels;		/* levels in tree */
	int ret;
	int i;
	int cc;

	if ((rootnum < 1) || (rootnum > MAXROOTS))
		return(PARMERR);

	if ((ret = strtwrit(psp)) != CCOK)
	   return(ret);

	psp->curroot = rootnum;					/* set current root */
	nde = psp->rootlist[rootnum-1];		/* get to root node to drop */
	levels = gtlevels(psp);					/* get number of levels in tree */
	ret = noderead(psp, &nodea, nde);	/* read root */
	if (ret != CCOK)
	{
		psp->badnode = nde;
		psp->badtyp  = BADROOT;
	}

	/* scan down left side of tree to get leftmost node of each level */
	while ((nodea.level) && (ret == CCOK)) /* while not a leaf node */
	{	
	   psp->path[nodea.level] = nde;			/* set search path for this level */
		nde = nodea.p0;							/* leftmost path */
	   ret = noderead(psp, &nodea, nde);	/* and read lower node */
	   if (ret != CCOK)							/* if error reading */
		{
			psp->badnode = nde;
			psp->badtyp  = BADNDEREAD;
			break;
		}
	}

	if (ret == CCOK)
	{
		/* now at leaf level */
	   psp->path[0] = nde;	/* left most leaf node */
		ptroot(psp, 0);		/* clear root */
		ptlevels(psp, 0);		/* clear levels */

		/* clear all index references to dropped root */
		for (i = 0; i < NUMINDXS; i++)
		{
			if (psp->rootnum[i] == (unsigned char)rootnum)
				psp->rootnum[i] = 0;
		}
	}

	for (i = 0; ((i < levels) && (ret == CCOK)); i++)
	{
		fwdnde = psp->path[i];

		/* read right through all nodes of same level */
		while (fwdnde != -1)
		{
		   ret = noderead(psp, &nodea, fwdnde);		/* and read lower node */
			if (ret != CCOK)
				break;

			nde = fwdnde;
			fwdnde = nodea.forwrd;

			ret = sethfree(psp, &nodea, nde);			/* free the node */
			if (ret != CCOK)
				break;

			nodea.count = 0;
			nodea.free  = KEYAREA * MAXCOMP;
#ifdef COMPRESSION
			nodea.compfree = KEYAREA;
#endif
			cclear(nodea.keystr, sizeof(nodea.keystr), 0);	/* and clear keyarea */
			ret = nodewrte(psp, &nodea, nde);	/* write out empty node */
		}
	}

	/* clear nodea file and node information */
	nodea.npsp= NULL;			/* clear nodea file psp */
	nodea.nfd = NULLFD;		/* clear nodea file fd */
	nodea.nde = NULLNDE;		/* clear nodea node number */
	
   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);
}


#ifdef TRANSACTION
/* transaction beginning */
int EDECL transbeg(psp, operation, mirrorpsp)
struct passparm PDECL *psp;		/* psp of file to process */
int operation;							/* type of logging, TRANSALL, TRANSMIRROR */
struct passparm PDECL *mirrorpsp;	/* open psp of mirror cindex file */
{
	int cc;
	int ret;
	NDEPTR newnde;

	/* must be in security level 4 to do transaction logging */
	if (psp->seclev < 4)
		return(BADSECLEV);

	if (mirrorpsp)
	{
		if (operation == TRANSCHANGE)
			return(PARMERR);
		else
		if (checkpsp(mirrorpsp) != PSPOPEN)	/* not an open psp */
		   return(PSPERR);
	}

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	ret = transtst(psp);			/* test for busy or interupted transaction */
	if (ret == TRANSALLOC)
		ret = CCOK;

	if (ret == CCOK)
	{
		if (psp->transchain < 0)		/* create new chain */
		{
			ret = ciextend(psp);			/* try to extend file before allocating */
			if (ret == CCOK)
			{
			   newnde = gtnwnde(psp);	/* get next node */

				/* initialize 1st trasaction log node */
				cclear(&nodet, sizeof(nodet), 0);		/* clear key area */	
				nodet.p0 = psp->transchain;				/* insert into trans chain */
				nodet.forwrd = 0;
				nodet.revrse = psp->sharenum;				/* share # for this transaction process */
				nodet.level = TRANS1NDE;								/* trans log 1st node */
				nodet.count = 0;
				nodet.free = (KEYAREA * MAXCOMP);
#ifdef COMPRESSION
				nodet.compfree = KEYAREA;
#endif
			   ret = transwrte(psp, &nodet, newnde); 	/* commit to disk */
			}

			if (ret == CCOK)
			{
				psp->transchain = newnde;				/* set node on trans chain */
	         ret = hdrupdte(psp);   					/* update header */

			   cc = endwrit(psp);					/* unlock file */
				if (cc != CCOK)
					ret = cc;

				if (cc == CCOK)
				{
					/* initialize info about new transaction for this psp */
					psp->transop     = operation;			/* type of logging */
					psp->begtransnde = newnde;				/* 1st node in transaction */
					psp->curtransnde = newnde;				/* start with 1st node */
					psp->transndeptr = (unsigned char *)nodet.keystr;		/* ptr into trans node */
					psp->transnum    = psp->pupdatnum;	/* # of this transaction */
					psp->transseq    = 1;					/* trans sequence number */
					psp->transpsp    = mirrorpsp;			/* psp of mirror file */
					psp->strttransnde = psp->curtransnde;	/* ptr to likely strtwrit log entry */
					psp->strttransndeptr = psp->transndeptr;
				}

				return(ret);
			}
		}
		else
		{
			/* use existing transaction log node */
			newnde = psp->transchain;
			psp->transop     = operation;			/* type of logging */
			psp->begtransnde = newnde;				/* 1st node in transaction */
			psp->curtransnde = newnde;				/* start with 1st node */
			psp->transndeptr = (unsigned char *)nodet.keystr;		/* ptr into trans node */
			psp->transnum    = psp->pupdatnum;	/* # of this transaction */
			psp->transseq    = 1;					/* trans sequence number */
			psp->transpsp    = mirrorpsp;			/* psp of mirror file */
			psp->strttransnde = psp->curtransnde;	/* ptr to likely strtwrit log entry */
			psp->strttransndeptr = psp->transndeptr;
		}
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(psp->retcde = ret);
}


/* transaction end - successful */
int EDECL transend(psp, endtype)
struct passparm PDECL *psp;
int endtype;							/* TRANSDONE, TRANSCLEAR, TRANSBACK */
{
	int cc;
	int ret;
	int transshare;						/* transaction share number */
	NDEPTR fwdnde;							/* node number of next trans node */
	NDEPTR nde;								/* node number of current trans node */
	INT16 emptyent;						/* null entry */
	struct hdrrec hdr;					/* file header for mirror update */
	struct passparm * mirrorpsp;		/* psp of open mirror file */
	int endtransop;						/* transop at start of transend */

	if (	(endtype != TRANSDONE) 
		&& (endtype != TRANSCLEAR)
		&& (endtype != TRANSBACK))
		return(PARMERR);

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	ret = CCOK;

	if (psp->transchain > -1)
	{
		/* mark end of transaction */
		emptyent = 0;
		cc = translog(psp, TRANSEND, (char *)&emptyent);
		if (cc != CCOK)
		{
		   endwrit(psp);					/* unlock file */
			return(cc);						/* return error */
		}

      cc = muflush(psp);				/* clear out buffers now that trans complete */
		if (cc != CCOK)
		{
		   endwrit(psp);					/* unlock file */
			return(cc);						/* return error */
		}

		/* clear info about ended transaction for this psp */
		psp->begtransnde = -1;					/* 1st node in transaction */
		psp->curtransnde = -1;					/* start with 1st node */
		psp->transndeptr = NULL;				/* ptr into trans node */
		psp->transnum    = 0;					/* # of this transaction */
		psp->transseq    = 0;					/* trans sequence number */
		endtransop       = psp->transop;		/* save for mirror update */
		psp->transop     = 0;					/* no transaction operation */

		fwdnde = psp->transchain;
		/* read in first transaction chain node */
		cc = transread(psp, &nodet, fwdnde);
		if (cc != CCOK)
		{
		   endwrit(psp);					/* unlock file */
			return(cc);						/* return error */
		}

		if ((endtransop == TRANSALL) || (endtransop == TRANSMIRROR))
		{
			mirrorpsp = psp->transpsp;						/* mirror psp */
			if ((cc = strtwrit(mirrorpsp)) != CCOK)
			{
			   endwrit(psp);					/* unlock file */
			   return(cc);
			}
		}

		transshare = (int)nodet.revrse;
		if (transshare != psp->sharenum)
		{
			ret = TRANSBUSY;
		}
		else
		{
			if (endtype == TRANSDONE)
			{
				psp->transchain = -1;	/* release node from trans chain */
				ret = hdrupdte(psp);   			/* update header */

				/* read through all nodes of transaction and put on freechain */
				while ((fwdnde > 0) && (ret == CCOK))
				{
				   ret = transread(psp, &nodet, fwdnde);		/* read next trans node */
					if (ret != CCOK)
						break;

					nde = fwdnde;
					fwdnde = nodet.forwrd;

					ret = sethfree(psp, &nodet, nde);			/* free the node */
					if (ret != CCOK)
						break;

					nodet.count = 0;
					nodet.free  = KEYAREA * MAXCOMP;
#ifdef COMPRESSION
					nodet.compfree = KEYAREA;
#endif
					cclear(nodet.keystr, KEYAREA, 0);	/* and clear keyarea */
					ret = transwrte(psp, &nodet, nde);	/* write out empty node */

					/* if mirroring active, write out empty node to mirror file */
					if (	((endtransop == TRANSALL) || (endtransop == TRANSMIRROR))
						&& (ret == CCOK))
						ret = transwrte(mirrorpsp, &nodet, nde);
				}
			}
			else
			if (endtype == TRANSCLEAR)		/* clear log, don't remove space */
			{
				/* read through all nodes of transaction and put on freechain */
				while ((fwdnde > 0) && (ret == CCOK))
				{
				   ret = transread(psp, &nodet, fwdnde);		/* read next trans node */
					if (ret != CCOK)
						break;

					nde = fwdnde;
					fwdnde = nodet.forwrd;

					/* clear log entries from node */
					nodet.count = 0;
					nodet.free  = KEYAREA * MAXCOMP;
#ifdef COMPRESSION
					nodet.compfree = KEYAREA;
#endif
					cclear(nodet.keystr, KEYAREA, 0);	/* and clear keyarea */
					ret = transwrte(psp, &nodet, nde);	/* write out empty node */

					/* if mirroring active, write out empty node to mirror file */
					if (	((endtransop == TRANSALL) || (endtransop == TRANSMIRROR))
						&& (ret == CCOK))
						ret = transwrte(mirrorpsp, &nodet, nde);
				}
			}
			else
			if (endtype == TRANSBACK)		/* put log onto backup log chain */
			{
				psp->transchain = -1;	/* release node from trans chain */

				/* p0 now points down to prev transactions in backup chain */
				nodet.p0 = psp->transback;
				psp->transback = fwdnde;
				ret = hdrupdte(psp);   			/* update header */

				if (ret == CCOK)
					ret = transwrte(psp, &nodet, fwdnde);	/* write out node with chain info */

				/* if mirroring active, write out empty node to mirror file */
				if (	((endtransop == TRANSALL) || (endtransop == TRANSMIRROR))
					&& (ret == CCOK))
					ret = transwrte(mirrorpsp, &nodet, fwdnde);
			}
		}
	}
	else
	{
		ret = FAIL;			/* no transaction pending */
	}

	/* if mirroring file */
	if ((ret == CCOK)
		&& ((endtransop == TRANSALL) || (endtransop == TRANSMIRROR)))
	{
		hdr.updatnum = psp->pupdatnum;		/* keep current */
		sethdr(psp, &hdr);						/* move current info into header */
		setfile(mirrorpsp, &hdr);				/* update mirror psp header info */

		ret = hdrwrte(mirrorpsp, &hdr);		/* write new header to mirror file */
		if (ret == CCOK)
		{
			mirrorpsp->updtflag = CLEAN;		/* now clean */

		   cc = endwrit(mirrorpsp);			/* unlock mirror file */
			if (cc != CCOK)
				ret = cc;
		}
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(psp->retcde = ret);
}



/* roll back outstanding transactions, if any */
int EDECL transroll(psp, rollbackproc, rolltransop, mirrorpsp)
struct passparm PDECL *psp;
LOGPROCTYPE rollbackproc;
int rolltransop;					/* transaction logging type during rollback */
struct passparm PDECL *mirrorpsp;	/* mirror file for rollback */
{
	int cc;
	int ret = CCOK;
	int i;
	NDEPTR fwdnde;							/* node number of next trans node */
	NDEPTR nde;								/* node number of current trans node */
	NDEPTR ndeflip;						/* node number after byte flipping */
	long rolltot;							/* total number of entries to roll back */
	long rollcnt;							/* count of entries rolled back so far */
	double progress;						/* fraction complete (0.0 - 1.0) */
	long ndetot;							/* total count of nodes to process */
	long ndecnt;							/* count of nodes processed */
	unsigned char *entptr;				/* ptr to current log entry */
	unsigned char *transentlist[MAXENTRIES];	/* ptr list to each entry in node */
	char *ptr;								/* work pointer */
	NDEPTR rolltranschain;				/* top of chain to roll back */
	NDEPTR rollcurtransnde;				/* current node in memory */
	int curtransop;						/* transop before starting transroll */
	char nodeval[20];						/* description of node # written for log */
	int rollenable = FALSE;				/* enable rollback flag */
	TRANSENT wrkent;					/* to build entry */
	int transhdrlen;
	char *lowptr;
	char *hiptr;

	/* locate all nodes of transaction log to roll back */
	curtransop       = psp->transop;		/* save status of transop */
	rolltranschain = psp->transchain;		/* first node of log */
	rollcurtransnde= psp->curtransnde;		/* current logging node */
	fwdnde = rolltranschain;
	rolltot = 0;
	ndecnt = 0;
	ndetot = 0;

	/* determine size of transaction header */
	lowptr = (char *)&wrkent;
	hiptr = (char *)&(wrkent.transent);
	transhdrlen =  (int)(hiptr - lowptr);

	/* stop transaction logging */
	psp->begtransnde = -1;
	psp->curtransnde = -1;
	psp->transndeptr = NULL;
	psp->transnum    = 0;
	psp->transseq    = 0;
	psp->transop     = 0;

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	/* if was in middle of logged activity (rollback forced by application) */
	if (curtransop)
	{
		ret = transwrte(psp, &nodet, rollcurtransnde);	/* write left over log to disk */
	}
	else
	{
		cc = transtst(psp);			/* make sure have something to roll back */
		if (cc != TRANSINTR)
		{
		   cc = endwrit(psp);				/* unlock file */
			if (cc != CCOK)
				ret = cc;
			else
				ret = TRANSNULL;				/* nothing to roll back */
	
			return(ret);
		}
	}

	if (ret == CCOK)
	{
		psp->transchain  = -1;	/* release 1st trans node from transaction chain */
		ret = hdrupdte(psp);
	}

	/* if logging transactions during rollback, restart logging now */
	if ((ret == CCOK) && rolltransop)
	{
		ret = transbeg(psp, rolltransop, mirrorpsp);
	}

	while ((fwdnde > 0) && (ret == CCOK))
	{
	   ret = transread(psp, &noder, fwdnde);		/* read next trans node */
		if (ret != CCOK)
			break;

		nde = fwdnde;
		fwdnde = noder.forwrd;
		ndetot++;						/* incr total node count */
		rolltot += noder.count;		/* add up # of entries to roll back */

		/* call rollbackproc with each entry in log (forward direction) */
		if (rollbackproc)
		{
			ptr = noder.keystr;	/* ptr to first key */
			for (i = 0; i < noder.count; i++) 
			{
				/* if past end, error */
			   if (ptr >= noder.keystr + (KEYAREA * MAXCOMP))
				{
			      ret = BADNODE;
					break;
				}

				/* callback */
				ret = (rollbackproc)(psp, -1.0, (TRANSENT *)ptr);
				if (ret != CCOK)
					break;

				ptr += transhdrlen;							/* skip trans log header */
			   ptr = moveup(ptr);							/* point to next entry */
			}
		}

		/* save location of this node in transaction work index */
		if (ret == CCOK)
		{
			ret = cdupadd(psp, TRANSINDX, "TRANSNDE", ndetot, &nde, sizeof(nde));
			if (ret != CCOK)
				break;
		}
	}

	/* process each node in reverse order from last trans log node */
	rollcnt = 0;
	for (ndecnt = ndetot; (ret == CCOK) && ndecnt; ndecnt--)
	{
		/* locate next transaction node to read */
		psp->keyn = TRANSINDX;
		psp->key = "TRANSNDE";
		psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
		psp->rec = ndecnt;
		psp->data = (void *)&nde;
		psp->dlen = sizeof(nde);
		ret = _cfind(psp);
		if (ret != CCOK)
		{
			ret = DSKERR;
			break;
		}

	   ret = transread(psp, &noder, nde);			/* read next trans node */
		if (ret != CCOK)
			break;

		/* build list of pointers to each entry in node */
		ptr = noder.keystr;	/* ptr to first key */
		for (i = 0; i < noder.count; i++) 
		{
			/* if past end, error */
		   if (ptr >= noder.keystr + (KEYAREA * MAXCOMP))
			{
		      ret = BADNODE;
				break;
			}

			transentlist[i] = (unsigned char *)ptr;/* save ptr to this entry */
			ptr += transhdrlen;							/* skip trans log header */
		   ptr = moveup(ptr);							/* point to next entry */
		}

		/* roll back each entry in reverse order */
		for (i = noder.count; (ret == CCOK) && i; i--)
		{
			rollcnt++;
			if (rolltot)
				progress = (double)rollcnt / (double)rolltot;
			else
				progress = 0;

			entptr = transentlist[i-1];

			/* status callback */
			if (rollbackproc)
				ret = (rollbackproc)(psp, progress, (TRANSENT *)entptr);

			if (ret != CCOK)
				break;

			if (rollenable)
			{
				ret = transundo(psp, (TRANSENT *)entptr); /* reverse this entry */
			}
			else
			{
				/* enable rollback when see TRANSEND or TRANSENDWRIT */
				switch (((TRANSENT *)entptr)->transtype)
				{
					case TRANSEND:
					case TRANSENDWRIT:
						rollenable = TRUE;
						break;
				}
			}
		}
	}

	/* delete all node references in work index */
	for (ndecnt = ndetot; (ret == CCOK) && ndecnt; ndecnt--)
	{
		/* locate next transaction node to read */
		psp->keyn = TRANSINDX;
		psp->key = "TRANSNDE";
		psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
		psp->rec = ndecnt;
		psp->data = (void *)&nde;
		psp->dlen = sizeof(nde);
		ret = _cfind(psp);
		if (ret != CCOK)
		{
			ret = DSKERR;
			break;
		}

		ret = cdelcur(psp, TRANSINDX);			/* delete this item */
		if (ret != CCOK)
		{
			ret = DSKERR;
			break;
		}
	}

	if (ret == CCOK)
	{
		/* clear out buffers now that roll back is complete */
      ret = muflush(psp);
		if (ret != CCOK)
		{
		   endwrit(psp);					/* unlock file */
			return(ret);					/* return error */
		}

		/* read through all nodes of transaction and put on freechain */
		while ((fwdnde > 0) && (ret == CCOK))
		{
		   ret = transread(psp, &noder, fwdnde);		/* read next trans node */
			if (ret != CCOK)
				break;

			nde = fwdnde;
			fwdnde = noder.forwrd;

			ret = sethfree(psp, &noder, nde);	/* set node header to "free" */
			if (ret != CCOK)
				break;

			noder.count = 0;
			noder.free  = KEYAREA * MAXCOMP;
#ifdef COMPRESSION
			noder.compfree = KEYAREA;
#endif
			cclear(noder.keystr, KEYAREA, 0);	/* and clear keyarea */
			ret = transwrte(psp, &noder, nde);	/* write out empty node */

			if ((ret == CCOK) && ((psp->transop == TRANSALL) || (psp->transop == TRANSMIRROR)))
			{
				*nodeval = (char)sizeof(ndeflip);
				ndeflip = ltointel(nde, psp->wordorder);
				blockmv(nodeval+1, (char *)&ndeflip, sizeof(ndeflip));
				ret = translog(psp, TRANSNDEWRIT, nodeval);
			}
		}
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(psp->retcde = ret);
}

/* transfer updated nodes to mirror file */
int FDECL transmirror(psp)
struct passparm PDECL *psp;
{
	int cc;
	int ret;
	int i;
	NDEPTR fwdnde;							/* node number of next trans node */
	NDEPTR nde;								/* node number of current trans node */
	NDEPTR tmpnde;							/* node number before byte flipping */
	TRANSENT *entptr;
	unsigned char *transentlist[MAXENTRIES];	/* ptr list to each entry in node */
	char *ptr;								/* work pointer */
	int startoff;							/* transentlist offset for strtwrit log */
	int firstflg;							/* first node to process */
	struct passparm * mirrorpsp;		/* psp of open mirror file */
	struct hdrrec hdr;
	TRANSENT wrkent;					/* to build entry */
	int transhdrlen;
	char *lowptr;
	char *hiptr;

	/* set up mirror file to receive changes */
	mirrorpsp = psp->transpsp;
	if (mirrorpsp->seclev != 4)				/* force to security level 4 */
	   mirrorpsp->seclev = 4;					/* set level directly */

	if ((cc = strtwrit(mirrorpsp)) != CCOK)
	   return(cc);

	/* determine size of transaction header */
	lowptr = (char *)&wrkent;
	hiptr = (char *)&(wrkent.transent);
	transhdrlen =  (int)(hiptr - lowptr);

	/* process nodes of transaction log to extract mirror entries */
	fwdnde = psp->strttransnde;		/* first node of strtwrit in log */
	firstflg = TRUE;

	ret = CCOK;
	while ((fwdnde > 0) && (ret == CCOK))
	{
	   ret = transread(psp, &nodet, fwdnde);		/* read next trans node */
		if (ret != CCOK)
			break;

		/* build list of pointers to each entry in node */
		ptr = nodet.keystr;	/* ptr to first key */
		startoff = 0;
		for (i = 0; i < nodet.count; i++) 
		{
			/* if past end, error */
		   if (ptr >= nodet.keystr + (KEYAREA * MAXCOMP))
			{
		      ret = BADNODE;
				break;
			}

			/* test for match with starting point */
			if (firstflg && ptr == (char *)(psp->strttransndeptr))
			{
				startoff = i;
				firstflg = FALSE;
			}

			transentlist[i] = (unsigned char *)ptr;/* save ptr to this entry */
			ptr += transhdrlen;							/* skip trans log header */
		   ptr = moveup(ptr);							/* point to next entry */
		}

		/* transfer each modified node to mirror file */
		for (i = startoff; (ret == CCOK) && i < nodet.count; i++)
		{
			entptr = (TRANSENT *)transentlist[i];
			if (entptr->transtype == TRANSNDEWRIT)
			{
				blockmv((char *)&tmpnde, (char *)(&(entptr->transent)) + 1, sizeof(tmpnde));
				nde = inteltol(tmpnde, psp->wordorder);
				ret = noderead(psp, &nodeb, nde);		/* read changed node */
				if (ret == CCOK)
            {
					if (nde > mirrorpsp->endofile)
					{
						mirrorpsp->endofile = nde+1;
						mirrorpsp->next     = nde+1;
					}
					ret = nodewrte(mirrorpsp, &nodeb, nde);	/* write to mirror file */
				}
			}
		}

		/* write log node to mirror file */
		if (ret == CCOK)
			ret = transwrte(mirrorpsp, &nodet, fwdnde);

		fwdnde = nodet.forwrd;			/* next log node */
	}

	/* copy header portion of file */
	if (ret == CCOK)
	{
		hdr.updatnum = psp->pupdatnum;		/* keep current */
		sethdr(psp, &hdr);						/* move current info into header */
		setfile(mirrorpsp, &hdr);				/* update mirror psp header info */

		ret = hdrwrte(mirrorpsp, &hdr);		/* write new header to mirror file */
		if (ret == CCOK)
			mirrorpsp->updtflag = CLEAN;		/* now clean */
	}
	
   cc = endwrit(mirrorpsp);				/* unlock mirror file */
	if (cc != CCOK)
		ret = cc;

	return(psp->retcde = ret);
}

/* undo transaction in log */
int FDECL transundo(psp, entptr)
struct passparm PDECL *psp;
TRANSENT *entptr;
{
	int ret;
	ENTRY *ptr;
	char kdata[KEYSZE];
	char keybuf[KEYSZE];				/* buffer to save deleted key value */
	int knum;							/* key position offset */
	char *dataptr;						/* ptr to data in log record */


	ret = CCOK;
	switch (entptr->transtype)		/* process according to type */
	{
		case TRANSADD:			/* adds are reversed by doing delete */
			/* set up psp with entry values */
			ptr = &entptr->transent;
			psp->keyn = gtkeyn(ptr);
			cikeycpy(psp, psp->keyn, keybuf, gtkeyptr(ptr));
			psp->rec = gtkrec(ptr, psp->wordorder);
			psp->dbyte = gtdbyte(ptr);
			psp->dlen = goodata(psp, ptr);
			dataptr = gtdataptr(psp, ptr);

			/* search for key */
			ptr = treesrch(psp, psp->keyn, keybuf, psp->rec, psp->dbyte);
  	      if (ptr && (psp->keycmpres == 0))	/* if key in file, delete it */
			{
				kdel(ptr, &nodea);
			   ciadjust(psp, DELJST);		/* adjust current pointers */
				ret = nodewrte(psp, &nodea, getndenum(psp, psp->keyn));	/* write out leaf node */
			}		
			break;

		case TRANSDEL:			/* deletes are reversed by doing add */
			/* set up psp with entry values */
			ptr = &entptr->transent;
			psp->keyn = gtkeyn(ptr);
			cikeycpy(psp, psp->keyn, keybuf, gtkeyptr(ptr));
			psp->rec = gtkrec(ptr, psp->wordorder);
			psp->dbyte = gtdbyte(ptr);
			psp->dlen = goodata(psp, ptr);
			dataptr = gtdataptr(psp, ptr);

			/* search for place to put key */
			ptr = treesrch(psp, psp->keyn, keybuf, psp->rec, psp->dbyte);

			/* if already key in file, delete it */
  	      if (ptr && (psp->keycmpres == 0))
			{
				kdel(ptr, &nodea);
			   ciadjust(psp, DELJST);		/* adjust current pointers */
			}		

			if (!ptr)
			   ret = BADNODE;

			/* put log item back into file */
			if (ret == CCOK)
			{
				/* make key */
				bldlkey(psp, (ENTRY *)kdata, psp->keyn, keybuf, psp->rec, psp->dbyte, dataptr, psp->dlen, psp->wordorder);

				/* insert the new key */
				knum = getkeynum(psp, psp->keyn);	/* key position offset */
				ret = insrtkey((ENTRY *)kdata,&nodea,ptr,getndenum(psp, psp->keyn),psp,knum);
				putdelflag(psp, psp->keyn, DELOFF);		/* delete flag now clear */
			}
			break;

		case TRANSEND:			/* skip these items (do-nothing info) */
			break;

		case TRANSSTRTWRIT:
			break;

		case TRANSENDWRIT:
			break;

		case TRANSNDEWRIT:
			break;

		default:
			if (entptr->transtype < TRANSUSR)	/* if invalid type, return error */
				ret = TRANSERR;
			break;
	}

	return(ret);
}


/* test transaction log for incomplete transaction or transaction in use */
int EDECL transtst(psp)
struct passparm PDECL *psp;
{
	int cc;
	int ret;
	int transshare;			/* transaction share number */

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	if (psp->transchain > -1)
	{
		/* read in first transaction chain node */
		ret = transread(psp, &nodet, psp->transchain);
		transshare = (int)nodet.revrse;	/* revrse has share # */
		if (ret == CCOK)
		{
			if ((psp->processmode != EXCL) && (transshare != psp->sharenum))
			{
				cc = testshrbyte(psp, transshare);
				if (cc == FAIL)
					ret = TRANSINTR;			/* transaction was interupted */
				else
					ret = TRANSBUSY;			/* transaction processing is in use */
			}
			else
			{
				if (nodet.count > 0)
					ret = TRANSINTR;
				else
					ret = TRANSALLOC;
			}
		}
	}
	else
	{
		ret = CCOK;
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(psp->retcde = ret);
}

/* add log entry into transaction chain */
int EDECL translog(psp, transtype, entptr)
struct passparm *psp;
unsigned int transtype;
char *entptr;
{
	int translen;						/* length of transaction to put in log */
	int entrylen;						/* length of entry in transaction */
	NDEPTR newnde;						/* new node for transactions */
	int ret = CCOK;
	int cc;
	TRANSENT wrkent;					/* to build entry */
	int transhdrlen;
	char *lowptr;
	char *hiptr;

	/*
	 *	don't log semaphore changes and only log entry changes 
	 *	if TRANSALL or TRANSCHANGE logging set 
	 */
	if ((transtype == TRANSADD) || (transtype == TRANSDEL))
	{
		if (	(gtkeyn(entptr) == SEMAINDEX)
			|| ((psp->transop != TRANSALL) && (psp->transop != TRANSCHANGE)))
			return(CCOK);
	}

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	/* determine size of transaction header */
	lowptr = (char *)&wrkent;
	hiptr = (char *)&(wrkent.transent);
	transhdrlen =  (int)(hiptr - lowptr);

	/* if logging transactions, put add key into log */
	if (psp->curtransnde > 0)
	{
		entrylen = gtentlen((ENTRY *)entptr);
		translen = entrylen + 1 + transhdrlen;

		ret = transread(psp, &nodet, psp->curtransnde);	/* get log in memory */
		if (ret == CCOK)
		{
			/* test for space in this log node */
#ifdef COMPRESSION
			if (nodet.compfree < translen)		/* if might be out of space */
			{
				/* try shrinking node again */
				nodet.free = stointel(nodet.free, psp->wordorder);
				ret = cishrink(psp, &nodec, &nodet);
				nodet.free = inteltos(nodet.free, psp->wordorder);
				nodet.compfree = inteltos(nodec.compfree, psp->wordorder);
			}

			if (	((translen > nodet.free) || (translen > nodet.compfree))
				&& (ret == CCOK)) 		/* if out of room */
#else
			if (translen > nodet.free)
#endif
			{
				/* get new transaction node */
				ret = ciextend(psp);			/* try to extend file before allocating */
				if (ret == CCOK)
				{
				   newnde = gtnwnde(psp);	/* get next node */

					if (newnde > 0)
					{
						/* commit current trans log node to disk */
						nodet.forwrd = newnde;			/* insert into trans chain */
						ret = transwrte(psp, &nodet, psp->curtransnde);
					}
					else
						ret = DSKERR;			/* error reading/writing header */

					if (ret == CCOK)
					{
						/* initialize next trasaction log node */
						cclear(&nodet, sizeof(nodet), 0);	/* clear key area */	
						nodet.p0 = 0;								/* not used */
						nodet.forwrd = 0;							/* no fwd chain yet */
						nodet.revrse = psp->sharenum;			/* share # for this transaction process */
						nodet.level = TRANS2NDE;							/* trans log additional node */
						nodet.count = 0;
						nodet.free = (KEYAREA * MAXCOMP);
#ifdef COMPRESSION
						nodet.compfree = KEYAREA;
#endif

						psp->curtransnde = newnde;
						psp->transndeptr = (unsigned char *)nodet.keystr;
						ret = transwrte(psp, &nodet, psp->curtransnde);	/* write to disk */
						if (ret == CCOK)
			         	ret = hdrupdte(psp);   					/* update header */
					}
				}
			}
		}

		/* put entry into log */
		if (ret == CCOK)
		{
			/* build entry header */
			wrkent.transnum = ltointel(psp->transnum, psp->wordorder);
			wrkent.sequence = stointel(psp->transseq, psp->wordorder);
			wrkent.updatnum = ltointel(psp->pupdatnum, psp->wordorder);
			wrkent.transtype = (unsigned char)transtype;
			blockmv(psp->transndeptr, &wrkent, transhdrlen); /* move in entry header */
			psp->transndeptr += transhdrlen;
			blockmv(psp->transndeptr, entptr, entrylen+1);	/* entry */
			psp->transndeptr += entrylen + 1;

			nodet.count++;								/* bump up count */
			psp->transseq++;							/* bump up sequence # */
			nodet.free -= (INT16)translen;		/* decrease free space */
#ifdef COMPRESSION
			nodet.compfree -= translen;		/* decrease compressed free space */
#endif
			if (transtype == TRANSENDWRIT)	/* force write at end of operation only */
				ret = transwrte(psp, &nodet, psp->curtransnde);	/* write to disk */
		}
	}
	else
		ret = CCOK;					/* transaction log turned off */

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);
}

#endif	/* TRANSACTION */


#ifdef IMAGEBACK
/* routine to initiate image backup */
int EDECL imageback(psp, backpsp)
struct passparm *psp;
struct passparm *backpsp;
{
	/* if not an open psp, error */
	if (	(checkpsp(psp) != PSPOPEN)
		|| (checkpsp(backpsp) != PSPOPEN))
	   return(PSPERR);

	if (bcbnum == 0)
		return(PSPERR);

	psp->imagepsp = backpsp;			/* use backup file for image backup */
	return(CCOK);
}


/* routine to restore file using image backup */
int EDECL imagerest(psp, backpsp)
struct passparm *psp;
struct passparm *backpsp;
{
	int i;
	int cc;
	struct bcb wrkbcb;
	int ec;
	int ndecnt;
	struct hdrrec hdr;
	NDEPTR nptr;
	struct passparm *pspsave;
	int readerr;

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	/* restore header */
	if ((cc = hdrread(backpsp, &hdr)) != CCOK)	/* read backup header */
	{
		endwrit(psp);
	   return(cc);
	}

	pspsave = psp->imagepsp;				/* save image backkup file pointer */
	psp->imagepsp = (struct passparm *)NULL;	/* turn off image backup during restore */
	clrbcb(psp);								/* remove old copy of nodes from buffers */

	setfile(psp, &hdr);						/* reset rest of backup info */
	psp->pupdatnum = --hdr.updatnum;		/* reset update number */
	cc = hdrwrte(psp, &hdr);				/* write restored header to file */
	if (cc == CCOK)
		psp->updtflag = CLEAN;				/* now clean */

	for (ndecnt = 0, i = 0; cc == CCOK; i++)
	{
	   cc = bcbread(backpsp, &wrkbcb, i);
		if (cc == PROCINTR)
			break;			/* stopped by bcbread callback */

		if (cc != CCOK)		/* if past end of file or bad node */
		{
			cc = CCOK;			/* this is ok, use what was good in file */
			break;
		}

		/* if past current backup nodes */
		if (wrkbcb.bupdatnum != (psp->pupdatnum+1)) 
			break;

		/* test for bad information in node */
		if (	(wrkbcb.bbuffer.smfree < 0)
			|| (wrkbcb.bbuffer.smfree > (KEYAREA * MAXCOMP))
			||	((wrkbcb.bbuffer.smcount < 0) || (wrkbcb.bbuffer.smcount > MAXENTRIES))
			||	(	(wrkbcb.bbuffer.smforwrd != -1)
				&& (wrkbcb.bbuffer.smforwrd == wrkbcb.bbuffer.smrevrse) )
			||	(wrkbcb.bbuffer.smforwrd < -1)
			||	(wrkbcb.bbuffer.smrevrse < -1))
		{
			cc = CCOK;		/* must be past current backup info */
			break;
		}

		ndecnt++;			/* this is a node that needs to be restored */
	}

	/* clear content indicators for nodea - used as scratch area */
	nodea.npsp= NULL;			/* clear nodea file psp */
	nodea.nfd = NULLFD;		/* clear nodea file fd */
	nodea.nde = NULLNDE;		/* clear nodea node number */

	/* restore nodes in reverse order */	
	for (i = ndecnt-1, readerr = CCOK; (i >= 0) && (cc == CCOK); i--)
	{
		psp->alterr = CCOK;						/* start with no image node errors */
	   cc = bcbread(backpsp, &wrkbcb, i);
		if (cc != CCOK)
			break;

		/* if this image backup node is bad, skip it */
		if (psp->alterr != CCOK)
		{
			readerr = psp->alterr;
			continue;
		}

		/* if node is past last node, skip writing (will be zeroed below) */
		if (wrkbcb.bnde > psp->next)
			continue;

		/* write (restore) backup node image to original file */
		cclear(&nodea, sizeof(nodea), 0);
		blockmv(&nodea, &wrkbcb.bbuffer, sizeof(wrkbcb.bbuffer));
		cc = nodewrte(psp, &nodea, wrkbcb.bnde);
 	}

	/* clear all nodes past current information */
	for (nptr = psp->next; cc == CCOK; nptr++)
	{
		/* try to position to and read nodes past end of valid data */
		cc = fileSeek(psp, nptr, NODESZE, (long)HDRSZE);
		if (cc == CCOK)				 	/* if no error, read buffer */
		{
		   cc = ciread(psp, (char *)&ebuffer, NODESZE);	/* read into buffer */
			if (cc == CCOK)			/* can read it, clear it */
			{
				cclear((char *) &ebuffer, sizeof(ebuffer), 0);	/* make empty node */
				cc = fileSeek(psp, nptr, NODESZE, (long)HDRSZE);
				if (cc == CCOK)
					cc = ciwrite(psp, (char *)&ebuffer, NODESZE);
			}
		}

		if (cc != CCOK)
		{
			cc = CCOK;			/* reached end of operating system file */
			break;				/* stop clearing file */
		}
	}

	psp->backnde = 0;					/* reset image backup node counter */
	psp->backlast = 0;
	psp->imagepsp = pspsave;		/* restore image backup file pointer */

	ec = endwrit(psp);
	if (ec != CCOK)
		cc = ec;

	/* return bad image node error, if any */
	psp->alterr = readerr;
	if ((cc == CCOK) && (readerr != CCOK))
		cc = BADNODE;

	return(cc);
}


/* check integrity of node - used by imagerest */
int FDECL imgchecknde(psp, nod, nde)
struct passparm PDECL *psp;
struct smallnode PDECL *nod;
NDEPTR nde;
{
	NDEPTR ndeptr;
	char *p;
	int i;
	int ll;
	int errflg;			/* TRUE == have encountered error in scanning node */
	int ret;
	int tstlen;

	/**/

	/* assume OK node */
	errflg = FALSE;
	ret = CCOK;

	/* test for bad information in node */
	if ((nod->smfree < 0) || (nod->smfree > (KEYAREA * MAXCOMP)))
	{
		psp->badtyp  = BADNDEFREE;
		errflg = TRUE;
	}
	else
	if ((nod->smcount < 0) || (nod->smcount > MAXENTRIES))
	{
		psp->badtyp  = BADNDECOUNT;
		errflg = TRUE;
	}
	else	/* forward and reverse pointers point to each other */
	if (	(nod->smforwrd != -1)
		&& (nod->smforwrd == nod->smrevrse)
		&& (nod->smlevel  >= 0) )
	{
		psp->badtyp  = BADNDESAME;
		errflg = TRUE;
	}
	else	/* test the forward node pointer in node header */
	if (nod->smforwrd < -1)
	{
		psp->badtyp  = BADNDEFWD;
		errflg = TRUE;
	}
	else	/* test the reverse node pointer in node header */
	if (nod->smrevrse < -1)
	{
		psp->badtyp  = BADNDEREV;
		errflg = TRUE;
	}
	else
	{
		p = nod->smkeystr;
		for (i = 0; i < nod->smcount; i++)
		{
			psp->keyn = gtkeyn((ENTRY *)p);
			if ((psp->keyn < 1) || (psp->keyn > MAXINDXS))
			{
				psp->badtyp = BADNDESCAN;
				errflg = TRUE;
				break;
			}

			tstlen = gtentlen(((ENTRY *)p));
			if (tstlen < 6)
			{
				psp->badtyp = BADNDESCAN;
				errflg = TRUE;
				break;
			}

			if (nod->smlevel)
			{
				ndeptr = gtnptr(psp, (ENTRY *)p);
				if (ndeptr < 0)
				{
					psp->badtyp = BADNDENPTR;
					errflg = TRUE;
					break;
				}
			}
			else
			{
				/* test good key length */
			   ll = goodata(psp, (ENTRY *)p);
			   if (ll < 0)
				{
					psp->badtyp = BADNDEKEYLEN;
					errflg = TRUE;
					break;
				}
			}

			p = moveup(p);
			if ((p - nod->smkeystr) > (KEYAREA * MAXCOMP))
			{
				/* too many keys in node, or corrupted key values */
				psp->badtyp = BADNDECOUNT;
				errflg = TRUE;
				break;
			}
		}

		if (!errflg)
		{
			for (;(p < (nod->smkeystr + KEYAREA)); p++)
			{
				if (*p)			/* rest of node should be zeros */
				{
					/* too many keys in node, or corrupted key values */
					psp->badtyp = BADNDECOUNT;
					errflg = TRUE;
					break;
				}
			}
		}
	}

	if (errflg == TRUE)
	{
		psp->badnode = nde;
		ret = FAIL;
	}

	return(ret);
}

#endif	/* IMAGEBACK */

