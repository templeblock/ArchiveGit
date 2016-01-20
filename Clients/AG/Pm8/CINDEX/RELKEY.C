/*  relkey.c  -  relative key positioning routines
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


/* get the relative percentage of current single-key position */
int EDECL cigetrel(psp, keyn)
struct passparm PDECL *psp;
int keyn;					/* index number to get position */
{
	int ret;
	int percentage = 0; 

	/* set for lower level */
	psp->keyn = keyn;
	psp->data = NULL;
	psp->dlen = 0;
	psp->keyorder = INTELFMT;			/* default key byte ordering is native */

	ret = _cgetcur(psp);
	if (ret == CCOK)
	{
#ifndef NO_WORD_CONVERSION
		cipspflip(psp, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */

		ret = cigetreltreesrch(psp, keyn, psp->key, psp->rec, 0, &percentage);

		if (ret != CCOK)
		{
			psp->relpercent = -1;
		}
		else
		{
			psp->relpercent = percentage;
		}
	}

	return(psp->retcde = ret);
}


/*
 * searches tree by relative key position
 * - returns ptr to key or 0 if error - sets current info
 */

int FDECL cigetreltreesrch(psp, keyn, key, rec, dbyte, percentptr)
register struct passparm *psp;
int keyn;								/* index # of key */
char *key;								/* key to search for */
long rec;								/* record number to search for */
int dbyte;								/* data byte to search for */
int *percentptr;						/* ptr to integer to pass back percentage */
{
	int cnt;								/* count of key found */
	NDEPTR nde;				/* node number of root */
	ENTRY *entptr;
	int oldlevel;
	int cc;
	int fndkeyn;
	long enttot;			/* estimated total number of entries in tree */
	long entoff;			/* estimated offset of this entry in tree */

	psp->nextkeyn = keyn;					/* assume next node has same index */
	enttot = 0;									/* clear entry total count */
	entoff = 0;									/* clear offset total count */

	nde = gtroot(psp, keyn);				/* set to root node to start */
	cc = noderead(psp, &nodea, nde);		/* read root */
	if (cc != CCOK)
		return(BADNODE);

	while (nodea.level) 	/* while not a leaf node */
	{	
	   oldlevel = nodea.level;
	   psp->path[nodea.level] = nde;	/* set search path for this level */

	   /* find lower node */
	   cc = ciscangetrelptr(&nodea, keyn, key, rec, dbyte, psp, psp->wordorder, &enttot, &entoff, &nde);
	   if (cc != CCOK)	/* error in scan */
			return(cc);

	   cc = noderead(psp, &nodea, nde);		/* and read lower node */
	   if ((cc != CCOK) || (oldlevel != nodea.level + 1))	/* bad scan */
			return(BADNODE);
	}	
	/* now at leaf level */
	psp->path[nodea.level] = nde;	/* set search path for leaf level */

	/* find key */
	cc = ciscangetrelnde(&nodea, keyn, key, rec, dbyte, &cnt, psp, 
									psp->wordorder, &enttot, &entoff, &entptr);
	if (cc)
		return(cc);		/* bad leaf scan */

	psp->treeindex = -1;			/* invalidate tree path, rel search cannot be used for writes */
	fndkeyn = gtkeyn(entptr);
	if (fndkeyn && fndkeyn != keyn)
		return(ERRSHAREDIND);	/* found key for another index in this tree */

#ifdef REMOVED
//		putkeynum(psp, keyn, cnt);		/* set key count */
//		putndenum(psp, keyn, nde);		/* set last node read */
//		/* set current index */
//		fndkeyn = gtkeyn(entptr);
//		putcurindx(psp, keyn, ((fndkeyn) ? (char)fndkeyn : (char)(keyn)));

	/* MU */
//		setcurkey(psp, entptr);	/* set current key and record number */
#endif	/* REMOVED */

	/* return percentage location of this entry */
	if (percentptr)
	{
		if (enttot)
			*percentptr = (int)((100 * entoff) / enttot);
		else
			*percentptr = 0;
	}

	return(CCOK);
}
 		
					


/* scans index node and returns node ptr */
int FDECL ciscangetrelptr(ndea, keyn, key, rec, dbyte, psp, tstorder, totptr, offptr, ndeptr)
struct node *ndea;	/* node to scan */
int keyn;
char *key;			/* key to compare against */
long rec;			/* record number of key */
int dbyte;			/* data byte of key to compare */
CFILE *psp;			/* in case of read right */
INT32 tstorder;	/* word order for passed key value */
long *totptr;		/* ptr to integer to pass back est total entries */
long *offptr;		/* ptr to integer to pass back est offset to this entry */
NDEPTR *ndeptr;	/* ptr to NDEPTR value to return result to */
{
	register int i;
	NDEPTR  noderet;				/* value returned - node ptr */
	register char *ptr;
	int cc;
	int res;							/* result of comparison */
	int indexcnt;					/* count of keys in search index */
	int indexoff;					/* offset of located path in this index */
	NDEPTR xnode;					/* current node ptr */
	int curind;
	NDEPTR revnode;				/* reverse node pointer from current */
	NDEPTR fwdnode;				/* forward node pointer from current */
	int revcnt;						/* count of index matches in reverse node */
	int fwdcnt;						/* count of index matches in forward node */
	int adjcnt;						/* adjusted index count, based on fwd or rev */

	xnode = ndea->p0;				/* set to p0 initially */
	noderet = NULLNDE;			/* set to null initially */
	ptr = ndea->keystr;			/* ptr to first key */
	indexcnt = 0;
	indexoff = 0;
		
	if (ndea->count > MAXENTRIES)
	{
		*ndeptr = NULLNDE;
	   return(BADNODE);
	}

	for (i = 0; i < ndea->count; i++) 
	{
		curind = gtkeyn(ptr);
		if (curind && curind != keyn)
			return(ERRSHAREDIND);	/* found key for another index in this tree */

		indexcnt++;					/* found a key in this index */

	   res = cikeytest(psp, (ENTRY *)ptr, keyn, key, rec, dbyte, tstorder);
	   if ((res > -1) && (noderet == NULLNDE))    /* key is <= ptr */
		{
			noderet = xnode;				/* ptr to node lower in tree */
			indexoff = indexcnt - 1;	/* offset into entries of this index */
		}

	   xnode = gtnptr(psp, (ENTRY *)ptr);	/* set new node ptr */
	   ptr = moveup(ptr);	/* point to next key */

	   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
		{
			*ndeptr = NULLNDE;
		   return(BADNODE);
		}
	}

	if (noderet == NULLNDE)	/* if didn't find match */
	{
	   if (ndea->forwrd != -1)	/* more to right */
	   {
	      cc = noderead(psp, ndea, ndea->forwrd);	/* move right */
			if (cc != CCOK)
			{
				*ndeptr = NULLNDE;
			   return(BADNODE);
			}
			else		/* else make recursive call */
	      	return(ciscangetrelptr(ndea, keyn, key, rec, dbyte, psp, tstorder, totptr, offptr, ndeptr));
	   }
		else
		{
			noderet = xnode;		/* at end of tree, use last node pointer */
			if (curind != keyn)	/* must have more keys in node that is being followed */
				indexcnt++;
			indexoff = indexcnt;			/* if didn't find key, set to last */
		}
	}

	/* return total and offset of key path in keys of this node */
	if (totptr && offptr && indexcnt)
	{
		/* if following the PO pointer, must have one more than counted */
		if (noderet == ndea->p0)
		{
			indexoff = 1;
			indexcnt++;
		}
		else	/* if 1st key is in index, assume more nodes to left */
		if (gtkeyn(ndea->keystr) == keyn)
		{
			indexoff++;
			indexcnt++;
		}

#ifdef REMOVED
		if (indexoff == 0)
			indexoff = indexcnt;			/* if didn't find key, set to last */
#endif

		revnode = ndea->revrse;
		fwdnode = ndea->forwrd;
		adjcnt = indexcnt;				/* start with actual count of entries in desired index */
		if (revnode != NULLNDE)
		{
	      cc = noderead(psp, ndea, revnode);	/* read left node */
			if (cc != CCOK)
			{
				*ndeptr = NULLNDE;
			   return(cc);
			}

			if (ndea->count > MAXENTRIES)
			{
				*ndeptr = NULLNDE;
			   return(BADNODE);
			}

			revcnt = 0;
			ptr = ndea->keystr;	/* ptr to first key */
			for (i = 0; i < ndea->count; i++) 
			{
				curind = gtkeyn(ptr);
				if (curind && curind != keyn)
					return(ERRSHAREDIND);	/* found key for another index in this tree */

				revcnt++;					/* found a key in this index */

			   ptr = moveup(ptr);	/* point to next key */
			   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
				{
					*ndeptr = NULLNDE;
				   return(BADNODE);
				}
			}

			if (gtkeyn(ndea->keystr) == keyn)
				revcnt++;		/* PO ptr may lead to index entries */

			if (revcnt > adjcnt)
				adjcnt = revcnt;
		}

		if (fwdnode != NULLNDE)
		{
	      cc = noderead(psp, ndea, fwdnode);	/* read right node */
			if (cc != CCOK)
			{
				*ndeptr = NULLNDE;
			   return(cc);
			}

			if (ndea->count > MAXENTRIES)
			{
				*ndeptr = NULLNDE;
			   return(BADNODE);
			}

			fwdcnt = 0;
			ptr = ndea->keystr;	/* ptr to first key */
			for (i = 0; i < ndea->count; i++) 
			{
				curind = gtkeyn(ptr);
				if (curind && curind != keyn)
					return(ERRSHAREDIND);	/* found key for another index in this tree */

				fwdcnt++;					/* found a key in this index */

			   ptr = moveup(ptr);	/* point to next key */
			   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
				{
					*ndeptr = NULLNDE;
				   return(BADNODE);
				}
			}

			if (gtkeyn(ndea->keystr) == keyn)
				fwdcnt++;		/* PO ptr will lead to index entries */

			if (fwdcnt > adjcnt)
				adjcnt = fwdcnt;
		}

		if (*totptr)
			*totptr = (*totptr - 1) * adjcnt + indexcnt;
		else
			*totptr = adjcnt;

		if (*offptr)
			*offptr = (*offptr - 1) * adjcnt + indexoff;
		else
			*offptr = indexoff;
	}

	*ndeptr = noderet;		/* return node pointer */
	return(CCOK);
}




/* scans leaf node and returns key pointer */
int FDECL ciscangetrelnde(ndea, keyn, key, rec, dbyte, cnt, psp, tstorder, totptr, offptr, entptrptr)
struct node *ndea;	/* node to scan */
int keyn;				/* index number */
char PDECL *key;		/* key to compare against */
long rec;				/* record number to compare */
int dbyte;				/* data byte to compare */
int PDECL *cnt;		/* where to put key count of key found */
CFILE PDECL *psp;		/* in case of read right */
INT32 tstorder;		/* word order of passed key value */
long *totptr;		/* ptr to integer to pass back est total entries */
long *offptr;		/* ptr to integer to pass back est offset to this entry */
ENTRY **entptrptr;	/* ptr to ptr to entry */
{
	register int i;	
	register char *ptr;		/* ptr into node */
	ENTRY *retptr;				/* found ptr into node */
	int res;
	int indexcnt;					/* count of keys in search index */
	int indexoff;					/* offset of located path in this index */
	int retcnt;						/* count return value of found key */
	int curind;
	NDEPTR revnode;				/* reverse node pointer from current */
	NDEPTR fwdnode;				/* forward node pointer from current */
	int revcnt;						/* count of index matches in reverse node */
	int fwdcnt;						/* count of index matches in forward node */
	int adjcnt;						/* adjusted index count, based on fwd or rev */
	int cc;
	NDEPTR curnde;					/* nodea node number when entering this func */

	/**/

	if (ndea->count > MAXENTRIES)
	{
		*entptrptr = (ENTRY *)NULL;
	   return(BADNODE);
	}

	if (!ndea->count)				/* if empty node */
	{
		psp->keycmpres = 1;		/* assume key is larger */
		psp->strcmpres = 1;
		*cnt = 0;					/* ptr to beg of empty node */
		*entptrptr = (ENTRY *)ndea->keystr;
		return(CCOK);
	}

	indexcnt = 0;
	indexoff = 0;
	retcnt = 0;
	ptr = ndea->keystr;	/* ptr to first key */
	retptr = (ENTRY *)0;
	curnde = ndea->nde;
	for (i = 0; i < ndea->count; i++) 
	{
		curind = gtkeyn(ptr);
		if (curind && curind != keyn)
			return(ERRSHAREDIND);	/* found key for another index in this tree */

		indexcnt++;					/* found a key in this index */

	   res = cikeytest(psp, (ENTRY *)ptr, keyn, key, rec, dbyte, tstorder);
	   if ((res > -1) && (retptr == (ENTRY *)0))  /* key is <= ptr */
		{
			indexoff = indexcnt;			/* offset into entries of this index */
			retptr = (ENTRY *)ptr;		/* return this pointer */
			retcnt = i;
		}

	   ptr = moveup(ptr);	/* point to next key */
	   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
		{
			*entptrptr = (ENTRY *)NULL;
		   return(BADNODE);
		}
	}

	/* if last key in last node, force to 100% */
	if ((indexoff == indexcnt) && (ndea->forwrd == NULLNDE))
	{
		if (*totptr)
			*totptr = *totptr * indexcnt;
		else
			*totptr = indexcnt;

		if (*offptr)
			*offptr = *totptr;
	}
	else	/* else if first key in first node, force to 0% */
	if ((indexoff == 1) && (ndea->revrse == NULLNDE))
	{
		if (*totptr)
			*totptr = *totptr * indexcnt;
		else
			*totptr = indexcnt;

		if (*offptr)
			*offptr = 0;
	}
	else	/* else return relative percentage found in search */
	if (totptr && offptr)
	{
		revnode = ndea->revrse;
		fwdnode = ndea->forwrd;
		adjcnt = indexcnt;				/* start with actual count of entries in desired index */
		if (revnode != NULLNDE)
		{
	      cc = noderead(psp, ndea, revnode);	/* read left node */
			if (cc != CCOK)
			{
				*entptrptr = (ENTRY *)NULL;
				return (cc);
			}

			if (ndea->count > MAXENTRIES)
			{
				*entptrptr = (ENTRY *)NULL;
			   return(BADNODE);
			}

			revcnt = 0;
			ptr = ndea->keystr;	/* ptr to first key */
			for (i = 0; i < ndea->count; i++) 
			{
				curind = gtkeyn(ptr);
				if (curind && curind != keyn)
					return(ERRSHAREDIND);	/* found key for another index in this tree */

				revcnt++;					/* found a key in this index */

			   ptr = moveup(ptr);	/* point to next key */
			   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
				{
					*entptrptr = (ENTRY *)NULL;
				   return(BADNODE);
				}
			}

			if (revcnt > adjcnt)
				adjcnt = revcnt;
		}

		if (fwdnode != NULLNDE)
		{
	      cc = noderead(psp, ndea, fwdnode);	/* read right node */
			if (cc != CCOK)
			{
				*entptrptr = (ENTRY *)NULL;
				return (cc);
			}

			if (ndea->count > MAXENTRIES)
			{
				*entptrptr = (ENTRY *)NULL;
			   return(BADNODE);
			}

			fwdcnt = 0;
			ptr = ndea->keystr;	/* ptr to first key */
			for (i = 0; i < ndea->count; i++) 
			{
				curind = gtkeyn(ptr);
				if (curind && curind != keyn)
					return(ERRSHAREDIND);	/* found key for another index in this tree */

				fwdcnt++;					/* found a key in this index */

			   ptr = moveup(ptr);	/* point to next key */
			   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
				{
					*entptrptr = (ENTRY *)NULL;
				   return(BADNODE);
				}
			}

			if (fwdcnt > adjcnt)
				adjcnt = fwdcnt;
		}

		if (adjcnt)
		{
			if (*totptr)
				*totptr = (*totptr - 1) * adjcnt + indexcnt;
			else
				*totptr = adjcnt;

			if (*offptr)
				*offptr = (*offptr - 1) * adjcnt + indexoff;
			else
				*offptr = indexoff;
		}
	}

	if (ndea->nde != curnde)
	{
      cc = noderead(psp, ndea, curnde);	/* re-read key node */
		if (cc != CCOK)
		{
			*entptrptr = (ENTRY *)NULL;
			return (cc);
		}
	}

	*cnt = retcnt;		/* set key count */
	*entptrptr = retptr;
	return(CCOK);
}

/*******************************************/

/* set the current single-key position based on relative percentage of index */
int EDECL cisetrel(psp, keyn, percent)
struct passparm PDECL *psp;
int keyn;
int percent;
{
	int cnt;								/* count of key found */
	NDEPTR nde;				/* node number of root */
	ENTRY *entptr = (ENTRY *)0;
	int oldlevel;
	int cc;
	int fndkeyn;
	long enttot;			/* estimated total number of entries in tree */
	long entoff;			/* estimated offset of this entry in tree */

	/* percentage is an interger in range of 0 - 100 */
	if ((percent < 0) || (percent > 100))
		return(PARMERR);

	psp->nextkeyn = keyn;					/* assume next node has same index */
	enttot = 0;									/* clear entry total count */
	entoff = 0;									/* clear offset total count */

	nde = gtroot(psp, keyn);				/* set to root node to start */
	cc = noderead(psp, &nodea, nde);		/* read root */
	if (cc != CCOK)
		return(BADNODE);

	while (nodea.level) 	/* while not a leaf node */
	{	
	   oldlevel = nodea.level;
	   psp->path[nodea.level] = nde;	/* set search path for this level */

	   /* find lower node */
	   cc = ciscansetrelptr(&nodea, keyn, psp, &enttot, &entoff, &nde, percent);
	   if (cc != CCOK)	/* error in scan */
			return(cc);

	   cc = noderead(psp, &nodea, nde);		/* and read lower node */
	   if ((cc != CCOK) || (oldlevel != nodea.level + 1))	/* bad scan */
			return(BADNODE);
	}	
	/* now at leaf level */
	psp->path[nodea.level] = nde;	/* set search path for leaf level */

	/* find key */
	cc = ciscansetrelnde(&nodea, keyn, &cnt, psp, 
									&enttot, &entoff, &entptr, percent);
	if (cc)
		return(cc);		/* bad leaf scan */

	psp->treeindex = -1;			/* invalidate tree path, rel search cannot be used for writes */
	fndkeyn = gtkeyn(entptr);
	if (fndkeyn && fndkeyn != keyn)
		return(ERRSHAREDIND);	/* found key for another index in this tree */

	if (!gtentlen(entptr))		/* if no key found */
	{
	   entptr = next(psp, keyn); 	/* get next key */

	   /* set current index */
		fndkeyn = gtkeyn((ENTRY *)entptr);
		putcurindx(psp, keyn, ((fndkeyn) ? fndkeyn : keyn + 1));

	   /* MU */
	   setcurkey(psp, entptr);				/* set current key and record num */

		if (!entptr || !gtentlen(entptr))
			return(FAIL);
	}
	else
	{
		putkeynum(psp, keyn, cnt);		/* set key count */
		putndenum(psp, keyn, nde);		/* set last node read */

		/* set current index */
		fndkeyn = gtkeyn(entptr);
		putcurindx(psp, keyn, ((fndkeyn) ? (char)fndkeyn : (char)(keyn)));

		/* MU */
		setcurkey(psp, entptr);	/* set current key and record number */

		if (!entptr)
			return(FAIL);		/* did not find any entry */
	}

	return(CCOK);
}


/* scans index node for correct percentage offset and returns node ptr */
int FDECL ciscansetrelptr(ndea, keyn, psp, totptr, offptr, ndeptr, percent)
struct node *ndea;	/* node to scan */
int keyn;
CFILE *psp;			/* in case of read right */
long *totptr;		/* ptr to integer to pass back est total entries */
long *offptr;		/* ptr to integer to pass back est offset to this entry */
NDEPTR *ndeptr;	/* ptr to NDEPTR value to return result to */
int percent;		/* percentage position being searched for */
{
	register int i;
	register char *ptr;
	int cc;
	int indexcnt;					/* count of keys in search index */
	NDEPTR xnode;					/* current node ptr */
	int curind;
	NDEPTR revnode;				/* reverse node pointer from current */
	NDEPTR fwdnode;				/* forward node pointer from current */
	int revcnt;						/* count of index matches in reverse node */
	int fwdcnt;						/* count of index matches in forward node */
	int adjcnt;						/* adjusted index count, based on fwd or rev */
	NDEPTR curnde;					/* nodea node number when entering this func */
	long curindcnt;				/* current indexcnt value */
	long newoffset;					/* offsets to entries in this node */
	long tstoffset;				/* test indexoff value to evaluate percentage */
	int tstpercent;				/* test percentage location */

	xnode = ndea->p0;				/* set to p0 initially */
	ptr = ndea->keystr;			/* ptr to first key */
		
	if (ndea->count > MAXENTRIES)
	{
		*ndeptr = NULLNDE;
	   return(BADNODE);
	}

	indexcnt = 1;		/* always count p0 */

	/* count the keys in the node and test for invalid conditions */
	for (i = 0; i < ndea->count; i++) 
	{
		curind = gtkeyn(ptr);
		if (curind && curind != keyn)
			return(ERRSHAREDIND);	/* found key for another index in this tree */

		indexcnt++;					/* found a key in this index */

	   ptr = moveup(ptr);							/* point to next key */
	   if (ptr >= ndea->keystr + CI_BIGBUF)	/* if past end */
		{
			*ndeptr = NULLNDE;
		   return(BADNODE);
		}
	}

	/* improve key count by looking at reverse and forward nodes */
	revnode = ndea->revrse;
	fwdnode = ndea->forwrd;
	adjcnt = indexcnt;			/* start with actual count of entries */
	curnde = ndea->nde;
	if (revnode != NULLNDE)
	{
	   cc = noderead(psp, ndea, revnode);	/* read left node */
		if (cc != CCOK)
		{
			*ndeptr = NULLNDE;
		   return(cc);
		}

		if (ndea->count > MAXENTRIES)
		{
			*ndeptr = NULLNDE;
		   return(BADNODE);
		}

		/* count the keys in the reverse node and test for invalid conditions */
		revcnt = 0;
		ptr = ndea->keystr;	/* ptr to first key */
		for (i = 0; i < ndea->count; i++) 
		{
			curind = gtkeyn(ptr);
			if (curind && curind != keyn)
				return(ERRSHAREDIND);	/* found key for another index in this tree */

			revcnt++;					/* found a key in this index */

		   ptr = moveup(ptr);	/* point to next key */
		   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
			{
				*ndeptr = NULLNDE;
			   return(BADNODE);
			}
		}

		if (gtkeyn(ndea->keystr) == keyn)
			revcnt++;		/* PO ptr may lead to index entries */

		if (revcnt > adjcnt)
			adjcnt = revcnt;
	}

	if (fwdnode != NULLNDE)
	{
	   cc = noderead(psp, ndea, fwdnode);	/* read right node */
		if (cc != CCOK)
		{
			*ndeptr = NULLNDE;
		   return(cc);
		}

		if (ndea->count > MAXENTRIES)
		{
			*ndeptr = NULLNDE;
		   return(BADNODE);
		}

		/* count the keys in the forward node and test for invalid conditions */
		fwdcnt = 0;
		ptr = ndea->keystr;	/* ptr to first key */
		for (i = 0; i < ndea->count; i++) 
		{
			curind = gtkeyn(ptr);
			if (curind && curind != keyn)
				return(ERRSHAREDIND);	/* found key for another index in this tree */

			fwdcnt++;					/* found a key in this index */

		   ptr = moveup(ptr);	/* point to next key */
		   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
			{
				*ndeptr = NULLNDE;
			   return(BADNODE);
			}
		}

		if (gtkeyn(ndea->keystr) == keyn)
			fwdcnt++;		/* PO ptr will lead to index entries */

		if (fwdcnt > adjcnt)
			adjcnt = fwdcnt;
	}

	/* need correct node from path in work buffer */
	if (ndea->nde != curnde)
	{
      cc = noderead(psp, ndea, curnde);	/* re-read key node */
		if (cc != CCOK)
		{
			*ndeptr = NULLNDE;
			return (cc);
		}
	}

	/* update the total number of estimated key entries in index */
	if (*totptr)
		curindcnt = (*totptr - 1) * adjcnt + indexcnt;
	else
		curindcnt = adjcnt;
	*totptr = curindcnt;

	/* if requested 0%, force to p0 pointer */
	if (percent == 0)
	{
		tstoffset = 0;
	   xnode = ndea->p0;
	}
	else
	{
		/* look for path which satisfies the percentage requested */
		if (*offptr)
			newoffset = (*offptr - 1) * adjcnt;			/* offset from previous search level */
		else
			newoffset = 0;
		if (ndea->forwrd == NULLNDE)
		{
			/* offset is # of entries total minus number of entries in this node */
			newoffset = *totptr - ndea->count - 1;
		}

		xnode = ndea->p0;				/* set to p0 initially */
		tstoffset = newoffset;
		ptr = ndea->keystr;			/* ptr to first key */

		/* test each entry to see if it is over desired percentage */
		for (i = 1; i <= ndea->count; i++) 
		{
			tstoffset = newoffset + i;

			/* if requested 100%, force to last entry in last node */
			if (percent == 100)
			{
				if (i == ndea->count)
				{
					tstoffset = i + 1;
				   xnode = gtnptr(psp, (ENTRY *)ptr);	/* point to last node path */
					break;
				}
			}
			else	/* if exceed desired percentage, go down left index pointer */
			{
				if (curindcnt)
					tstpercent = (int)((100 * tstoffset) / curindcnt);
				else
					tstpercent = 0;

				if (tstpercent >= percent)
					break;
			}
		   xnode = gtnptr(psp, (ENTRY *)ptr);	/* set new node ptr */
		   ptr = moveup(ptr);	/* point to next key */
		}
	}

	*offptr = tstoffset;
	*ndeptr = xnode;		/* return node pointer */
	return(CCOK);
}




/* scans leaf node and returns key pointer */
int FDECL ciscansetrelnde(ndea, keyn, cnt, psp, totptr, offptr, entptrptr, percent)
struct node *ndea;	/* node to scan */
int keyn;				/* index number */
int PDECL *cnt;		/* where to put key count of key found */
CFILE PDECL *psp;		/* in case of read right */
long *totptr;		/* ptr to integer to pass back est total entries */
long *offptr;		/* ptr to integer to pass back est offset to this entry */
ENTRY **entptrptr;	/* ptr to ptr to entry */
int percent;		/* percentage position being searched for */
{
	register int i;	
	register char *ptr;		/* ptr into node */
	ENTRY *retptr;				/* found ptr into node */
	int indexcnt;					/* count of keys in search index */
	int retcnt;						/* count return value of found key */
	int curind;
	NDEPTR revnode;				/* reverse node pointer from current */
	NDEPTR fwdnode;				/* forward node pointer from current */
	int revcnt;						/* count of index matches in reverse node */
	int fwdcnt;						/* count of index matches in forward node */
	int adjcnt;						/* adjusted index count, based on fwd or rev */
	int cc;
	NDEPTR curnde;					/* nodea node number when entering this func */
	long newindcnt;				/* current number of entries in whole index */
	long newoffset;				/* current indexoff value */
	long tstoffset;				/* test indexoff value to evaluate percentage */
	double tstpercent;			/* test percentage location */
	long retoffset;				/* selected offset */
	int scanrev = FALSE;			/* TRUE == had to look left for keys */

	/**/

	if (ndea->count > MAXENTRIES)
	{
		*entptrptr = (ENTRY *)NULL;
	   return(BADNODE);
	}

	/* search forward if no keys in this node */
	while (!ndea->count && (ndea->forwrd != NULLNDE))
	{
		cc = noderead(psp, ndea, ndea->forwrd);	/* read next right node */
		if (cc != CCOK)
		{
			*entptrptr = (ENTRY *)NULL;
			return (cc);
		}

		if (ndea->count > MAXENTRIES)
		{
			*entptrptr = (ENTRY *)NULL;
		   return(BADNODE);
		}
	}

	/* search reverse if no keys at end of file */
	while (!ndea->count && (ndea->revrse != NULLNDE))
	{
		cc = noderead(psp, ndea, ndea->revrse);	/* read next left node */
		if (cc != CCOK)
		{
			*entptrptr = (ENTRY *)NULL;
			return (cc);
		}

		if (ndea->count > MAXENTRIES)
		{
			*entptrptr = (ENTRY *)NULL;
		   return(BADNODE);
		}

		scanrev = TRUE;
	}

	/* if no keys in index at all, return not found */
	if (!ndea->count)
	{
		psp->keycmpres = 1;		/* assume key is larger */
		psp->strcmpres = 1;
		*cnt = 0;					/* ptr to beg of empty node */
		*entptrptr = (ENTRY *)ndea->keystr;
		return(CCOK);
	}



	/* if requested 0%, force to first key in node */
	if (percent == 0)
	{
		if (*totptr)
			*totptr = *totptr * ndea->count;
		else
			*totptr = ndea->count;

		if (*offptr)
			*offptr = 0;

		*cnt = 0;		/* set key count */
		*entptrptr = (ENTRY *)ndea->keystr;		/* ptr to first entry */
		return(CCOK);
	}

	/* count keys in node and test for invalid conditions */

	indexcnt = 0;
	retcnt = 0;
	retptr = (ENTRY *)0;
	curnde = ndea->nde;
	ptr = ndea->keystr;	/* ptr to first key */

	for (i = 0; i < ndea->count; i++) 
	{
		curind = gtkeyn(ptr);
		if (curind && curind != keyn)
			return(ERRSHAREDIND);	/* found key for another index in this tree */

		indexcnt++;					/* found a key in this index */

		/* if requested 100%, force to last entry in last node */
		if ((percent == 100) && (i == ndea->count-1))
		{
			if (*totptr)
				*totptr = *totptr * indexcnt;
			else
				*totptr = indexcnt;

			if (*offptr)
				*offptr = *totptr;

			*cnt = i;		/* set key count */
			*entptrptr = (ENTRY *)ptr;		/* return this pointer */
			return(CCOK);
		}

	   ptr = moveup(ptr);	/* point to next key */
	   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
		{
			*entptrptr = (ENTRY *)NULL;
		   return(BADNODE);
		}
	}

	/* improve count by search in forward and reverse nodes */
	revnode = ndea->revrse;
	fwdnode = ndea->forwrd;
	adjcnt = indexcnt;				/* start with actual count of entries in desired index */
	if (revnode != NULLNDE)
	{
	   cc = noderead(psp, ndea, revnode);	/* read left node */
		if (cc != CCOK)
		{
			*entptrptr = (ENTRY *)NULL;
			return (cc);
		}

		if (ndea->count > MAXENTRIES)
		{
			*entptrptr = (ENTRY *)NULL;
		   return(BADNODE);
		}

		revcnt = 0;
		ptr = ndea->keystr;	/* ptr to first key */
		for (i = 0; i < ndea->count; i++) 
		{
			curind = gtkeyn(ptr);
			if (curind && curind != keyn)
				return(ERRSHAREDIND);	/* found key for another index in this tree */

			revcnt++;					/* found a key in this index */

		   ptr = moveup(ptr);	/* point to next key */
		   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
			{
				*entptrptr = (ENTRY *)NULL;
			   return(BADNODE);
			}
		}

		if (revcnt > adjcnt)
			adjcnt = revcnt;
	}

	if (fwdnode != NULLNDE)
	{
	   cc = noderead(psp, ndea, fwdnode);	/* read right node */
		if (cc != CCOK)
		{
			*entptrptr = (ENTRY *)NULL;
			return (cc);
		}

		if (ndea->count > MAXENTRIES)
		{
			*entptrptr = (ENTRY *)NULL;
		   return(BADNODE);
		}

		fwdcnt = 0;
		ptr = ndea->keystr;	/* ptr to first key */
		for (i = 0; i < ndea->count; i++) 
		{
			curind = gtkeyn(ptr);
			if (curind && curind != keyn)
				return(ERRSHAREDIND);	/* found key for another index in this tree */

			fwdcnt++;					/* found a key in this index */

		   ptr = moveup(ptr);	/* point to next key */
		   if (ptr >= ndea->keystr + CI_BIGBUF)	/* past end */
			{
				*entptrptr = (ENTRY *)NULL;
			   return(BADNODE);
			}
		}

		if (fwdcnt > adjcnt)
			adjcnt = fwdcnt;
	}

	if (adjcnt)
	{
		if (*totptr)
			*totptr = (*totptr - 1) * adjcnt + indexcnt;
		else
			*totptr = adjcnt;
	}

	if (ndea->nde != curnde)
	{
      cc = noderead(psp, ndea, curnde);	/* re-read key node */
		if (cc != CCOK)
		{
			*entptrptr = (ENTRY *)NULL;
			return (cc);
		}
	}

	/* look for entry which satisfies the percentage requested */
	newindcnt = *totptr;			/* total number of entries in entire index */
	newoffset = ((*offptr - 1) * adjcnt);	/* offset from previous search level */
	/* adjust to precise offset if in last node or last node with keys */
	if ((ndea->forwrd == NULLNDE) || (scanrev == TRUE))
	{
		/* offset is # of entries total minus number of entries in this node */
		newoffset = *totptr - ndea->count;
	}

	tstoffset = newoffset;
	ptr = ndea->keystr;			/* ptr to first key */

	/* test each entry to see if it is closest to desired percentage */
	for (i = 1; i <= ndea->count; i++) 
	{
		tstoffset = newoffset + i;

		if (newindcnt)
			tstpercent = ((100.0 * (double)(tstoffset)) / (double)newindcnt);
		else
			tstpercent = 0.0;

		retoffset  = tstoffset;
		retptr     = (ENTRY *)ptr;
		retcnt     = i - 1;

		/* if this entry's percentage is closer than currently selected, this is it */
		if (	(tstpercent >= (double)percent) 
			|| (	(percent < 100) 
				&& (i == ndea->count-1) 
				&& ((ndea->forwrd == NULLNDE) || (scanrev == TRUE)) ) )
		{
			break;
		}

	   ptr = moveup(ptr);	/* point to next key */
	}

	*offptr = retoffset;
	*cnt = retcnt;		/* set key count */

	if (*totptr || gtkeyn(retptr))
		*entptrptr = retptr;
	else
		*entptrptr = (ENTRY *)0;

	return(CCOK);
}
