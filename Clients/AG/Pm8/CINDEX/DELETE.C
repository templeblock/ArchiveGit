/*  delete.c - cindex base code source file for writing functions
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




void FDECL kdel(p, ndea)		/* deletes key at p in node ndea */
ENTRY *p;					/* pointer to entry to delete */
struct node *ndea;		/* node to delete key from */
{
	register int dlen;
	register char *kend;
	char *nptr;

	dlen = gtentlen(p) + 1;
	nptr = ((char *)p) + dlen;
	kend = ndea->keystr + (unsigned int)((KEYAREA * MAXCOMP) - ndea->free);	/* end of keyarea */
#ifdef REMOVED
	kend = ndea->keystr + (unsigned int)(KEYAREA - ndea->free);	/* end of keyarea */
#endif
	cmovelt(nptr, (int) (kend - nptr), dlen);	/* move rest back */
	ndea->free += (INT16)dlen;						/* alter free space */
	ndea->count--;										/* decrement count */
	cclear(kend - dlen, dlen, 0);					/* and clear end */
}


int FDECL delnde(p, psp, databyte)	/* cleans all keys of data at p */
register ENTRY *p;	/* where to start */
struct passparm *psp;
int databyte;		/* last used data byte */
{
	register int gd = 0;					/* start with high bit off */
	char adjflag;

	if (!gtentlen(p))						/* no keys in this node */
	   return(databyte);					/* not done */

	adjflag = 'N';							/* not yet adjusted */
	while (gtentlen(p) && gd < 128)	/* while some left and not last dbyte */
	{
	   gd = gtdbyte(p);					/* get data byte */

		/* test for error in ordering of sub-entries */
	   if (	((gd & 127) < (databyte & 127))
			||	(!(gd & 127) && !(databyte & 128)))
	      return(-1);	/* then assume done */

		/* if already adjusted and back for more */
		/* move it up for next part */
	   if (adjflag == 'Y')
	      putkeynum(psp, psp->keyn, (getkeynum(psp, psp->keyn) + 1));

	   databyte = gd;

#ifdef TRANSACTION
		psp->retcde = translog(psp, TRANSDEL, (char *)p);
		if (psp->retcde != CCOK)
			return(-1);					/* error loggin transaction */
#endif	/* TRANSACTION */

	   kdel(p, &nodea);				/* delete entry */
	   ciadjust(psp, DELJST);		/* adjust current pointers */
	   adjflag = 'Y';					/* now have adjusted */	   
	}
	if (gd > 127)						/* high bit on ? (last key) */
	   return(-1);						/* done */

   return(databyte);					/* not done */
}


/* delete all keys for this data */
int FDECL kysdel(psp, entptr, nde)
struct passparm *psp;
ENTRY *entptr;						/* where key is */
NDEPTR nde;							/* node number of current node */
{
	int ret;							/* 1 = change of nodes, -1 = DSKERR */
	int newflg;						/* 1 = changed nodes */
	int dbyte;						/* data byte */
	NDEPTR fwdnde;					/* forward node ptr */
	/* new 4/21/86 */
	NDEPTR tndenum;				/* save the node number of first data part */
	int tkeynum;					/* save the key number */

	/**/

	tndenum = getndenum(psp, psp->keyn); /* save pointer to current node */
	tkeynum = getkeynum(psp, psp->keyn); /* and key in node */

	newflg = 0;
	dbyte = 128;						/* simulate last dbyte was end of record */
	fwdnde = nodea.forwrd;			/* next node # */
	ret = CCOK;
	psp->retcde = CCOK;

	/* more keys in following nodes */
	while ((dbyte > -1) && (ret == CCOK))
	{
		dbyte = delnde(entptr, psp, dbyte); 	/* delete keys  */
		if (psp->retcde != CCOK)
		{
			ret = psp->retcde;
			break;
		}

		if (psp->fulldel == TRUE)					/* if full tree delete */
		{
			ret = deltree(psp, nde);		/* 7/6/91 */
		}
		else
		{
			ret = nodewrte(psp, &nodea, nde);	/* write out leaf node */
		}

		if ((dbyte > -1) && ((nde = fwdnde) > -1) && (ret == CCOK))
		{
			newflg = 1;
		   ret = noderead(psp, &nodea, nde);		/* read next node */
			if (ret == CCOK)
			{
				fwdnde = nodea.forwrd;				/* next node # */
				entptr = (ENTRY *)nodea.keystr;	/* next node key area */
				putndenum(psp, psp->keyn, nde);	/* pnt to new node # for adjust */
			}
		}
	}

	/* restore pointer to where delete used to be */
	putndenum(psp, psp->keyn, tndenum);
	putkeynum(psp, psp->keyn, --tkeynum);

	if (ret != CCOK)
		ret = -1;				/* disk error */
	else
		ret = newflg;			/* normal return */

	return(ret);				/* return change or not */
}


int FDECL deltree(psp, nde)
struct passparm *psp;
NDEPTR nde;							/* node number of current node */
{
	int ret;

	psp->lowernde = nde;
	psp->curlevel = nodea.level;			/* save level for move up tree */
	psp->entries = nodea.count;
	psp->fwdptr = nodea.forwrd;			/* save fwd/rev for patching */
	psp->revptr = nodea.revrse;
	psp->chghighkey = FALSE;				/* assume no change to high keys */

	if ((!psp->entries) && (gtroot(psp, -1) != psp->lowernde))	/* if empty leaf node */
		ret = sethfree(psp, &nodea, psp->lowernde);	/* add free node to header */
	else
		ret = CCOK;

	if (ret == CCOK)
		ret = nodewrte(psp, &nodea, psp->lowernde);	/* write out leaf node */

	if (ret == CCOK)
		ret = patchleaf(psp);			/* patch out empty leaf node */

	if (ret == CCOK)
		ret = deluptree(psp);			/* delete up tree structure */

	if (ret == CCOK)
		ret = propuptree(psp);			/* propagate high key changes up tree */

	return(ret);
}

/* patch the forward and reverse ptrs of a deleted leaf node */
int FDECL patchleaf(psp)
struct passparm *psp;
{
	int ret = CCOK;

	if (	(!nodea.count)						/* if empty leaf node */
		&& (gtroot(psp, -1) != psp->lowernde))		/* and leaf is not root */
	{
		if (psp->fwdptr != -1)
		{
		   ret = noderead(psp, &nodea, psp->fwdptr);
			if (ret == CCOK)
			{
				nodea.revrse = psp->revptr;			/* patch out reverse ptr */
				psp->curlevel = nodea.level;			/* save level for move up tree */
				ret = nodewrte(psp, &nodea, psp->fwdptr);
			}
		}

		if ((psp->revptr != -1) && (ret == CCOK))
		{
		   ret = noderead(psp, &nodea, psp->revptr);
			if (ret == CCOK)
			{
				nodea.forwrd = psp->fwdptr;			/* patch out forward ptr */
				psp->curlevel = nodea.level;			/* save level for move up tree */
				ret = nodewrte(psp, &nodea, psp->revptr);
			}
		}
	}
	else
		ret = CCOK;

	return(ret);
}

/* work up tree to delete higher nodes that are emptied */
int FDECL deluptree(psp)
struct passparm *psp;
{
	ENTRY *delp;					/* ptr to entry to delete */
	int ret = CCOK;

	/* delete up tree if node empty */
	while (	(ret == CCOK)							/* while no disk errors */
			&& (psp->entries == 0)							/* and empty node */
			&& (psp->curlevel < gtlevels(psp) - 1))		/* and not at root */
	{
		psp->curlevel++;
		psp->uppernde = psp->path[psp->curlevel];			/* read up tree 1 level */
		psp->treeindex = -1;	 						/* invalidate tree for future use */
		ret = patchinode(psp);						/* remove reference to empty lower node */

		if (ret == CCOK)
		{
			if (	(psp->uppernde == gtroot(psp, -1))	/* if at root */
				&& (psp->entries == 0))						/* and no ptr left */
			{													/* turn into leaf */
				nodea.level = 0;							/* make it a leaf node */
				nodea.count = 0;							/* no keys */
				nodea.free = (KEYAREA * MAXCOMP);	/* all free space */
#ifdef REMOVED
				nodea.free = KEYAREA;					/* all free space */
#endif
				nodea.p0 = 0;								/* no p0 in leaf */
				nodea.forwrd = -1;						/* clear fwd/rev ptrs */
				nodea.revrse = -1;

				psp->entries = 1;								/* not empty as root */
				ptlevels(psp, 1);								/* one level tree */
			}
			else
			if (	(psp->uppernde == gtroot(psp, -1))	/* if at root */
				&& (psp->entries == 1))						/* and one ptr left */
			{
				psp->entries = 0;				/* treat as empty for space reclaim */
				ptroot(psp, nodea.p0);		/* move root down 1 level */
				ptlevels(psp, gtlevels(psp)-1);	/* reduce number of levels by 1 */

				ret = sethfree(psp, &nodea, psp->uppernde);

				/* write out index node */
				if (ret == CCOK)
					ret = nodewrte(psp, &nodea, psp->uppernde);

				if (ret != CCOK)
					break;

				/* update new root node */
			   ret = noderead(psp, &nodea, gtroot(psp, -1));
				if (ret == CCOK)
				{
					nodea.forwrd = -1;			/* clear fwd/rev ptrs */
					nodea.revrse = -1;
					ret = nodewrte(psp, &nodea, gtroot(psp, -1));
				}

				break;				/* done with read up */
			}

			/* if empty index node, add node to free chain */
			if (!psp->entries)
			{
				psp->fwdptr = nodea.forwrd;		/* save fwd/rev for patching */
				psp->revptr = nodea.revrse;
				psp->highkeynde = nodea.p0;		/* high key nde # of prev node */
				ret = sethfree(psp, &nodea, psp->uppernde);
			}

			/* write out index node */
			if (ret == CCOK)
				ret = nodewrte(psp, &nodea, psp->uppernde);
			else
				break;

			/* patch the forward and reverse ptrs of a deleted node */
			if (!psp->entries)
			{
				if (psp->fwdptr != -1)
				{
				   ret = noderead(psp, &nodea, psp->fwdptr);
					if (ret == CCOK)
					{
						nodea.revrse = psp->revptr;			/* patch out reverse ptr */
						ret = nodewrte(psp, &nodea, psp->fwdptr);
					}
				}

				if ((psp->revptr != -1) && (ret == CCOK))
				{
				   ret = noderead(psp, &nodea, psp->revptr);
					if (ret == CCOK)
					{
						nodea.forwrd = psp->fwdptr;			/* patch out forward ptr */

						/* if rightmost node, remove high key from prev node */
						if (nodea.forwrd == -1)
						{
							/* find high key */
							delp = scandkey(psp, &nodea, psp->highkeynde, TRUE);
							if (delp)
							{
								kdel(delp, &nodea);				/* delete high key */
#ifdef REMOVED
								ret = chkinode(psp, &nodea);
#endif
							}
							else
								ret = BADNODE;						/* missing entry */
						}

						if (ret == CCOK)
							ret = nodewrte(psp, &nodea, psp->revptr);
					}
				}
			}

			psp->lowernde = psp->uppernde;					/* save for next loop */
		}
	}

	return(ret);
}

/* work up tree to propagate new high key through rest of upper levels */
int FDECL propuptree(psp)
struct passparm *psp;
{
	ENTRY *delp;					/* ptr to entry to delete */
	int ret = CCOK;
	NDEPTR curnde;					/* current node ptr for changed entry */
	char newkeybuf[KEYSZE];		/* buffer for building new high key */

	/* propagate new high key up tree, if any */
	while (	(ret == CCOK)								/* while no disk errors */
			&& (psp->chghighkey == TRUE)				/* and changed high key */
			&& (psp->curlevel < gtlevels(psp) - 1))	/* and not at root */
	{
		psp->curlevel++;
		psp->uppernde = psp->path[psp->curlevel];			/* read up tree 1 level */
		psp->treeindex = -1;	 						/* invalidate tree for future use */

	   ret = noderead(psp, &nodea, psp->uppernde);
		if (ret == CCOK)
		{
			/* if propagating node ptr in p0, update 1st entry in node */
			if (psp->lowernde == nodea.p0)
			{
				delp = (ENTRY *)nodea.keystr;	/* use first key in node */
				if ((nodea.count == 1) && (nodea.forwrd != -1))
					psp->chghighkey = TRUE;	/* first entry is high key */
				else
					psp->chghighkey = FALSE;
			}
			else		/* update an entry in middle of node */
			{
				delp = scandkey(psp, &nodea, psp->lowernde, FALSE);
				if (delp)
					delp = (ENTRY *)moveup((char *)delp);	/* ptr to next key */
			}

			if (!delp || !gtentlen(delp))
				ret = BADNODE;						/* missing entry */
			else
			{
				curnde = gtnptr(psp, delp);	/* get current node ptr */
				kdel(delp, &nodea);				/* delete invalid key entry */

				/* add back new entry with updated key (new high key from below */
				bldikey(psp, (ENTRY *)psp->highkeybuf, curnde, (ENTRY *)newkeybuf, psp->wordorder);
			   addata((ENTRY *)newkeybuf, &nodea, delp);		/* add data to node */
				if (nodea.free < 0)
				{
					ret = insrtikey((ENTRY *)newkeybuf,&nodea,delp,psp->uppernde,psp,TRUE);
				}
				else
				{
					ret = nodewrte(psp, &nodea, psp->uppernde);	/* done - write modified node */
				}
			}
		}

#ifdef REMOVED
		ret = chkinode(psp, &nodea);
		if (ret != CCOK)
			break;
#endif

		/* save for next loop */
		psp->lowernde = psp->path[psp->curlevel];	/* <------- NEW release 4.0H */
	}

	return(ret);
}


/* patch index node to remove reference to lower level node that became empty */
int FDECL patchinode(psp)
struct passparm *psp;
{
	ENTRY *delp;					/* ptr to entry to delete */
	ENTRY *highdelp;				/* ptr to high key entry to delete */
	int ret = CCOK;
	NDEPTR highnde;				/* high key node number in native format */

	while (ret == CCOK)					/* loop continuously looking */
	{
	   ret = noderead(psp, &nodea, psp->uppernde);
		if (ret == CCOK)
		{
			/* if deleting node ptr in p0, use node # from 1st entry as p0 */
			if (psp->lowernde == nodea.p0)
			{
				if (nodea.count)					/* if any entries left */
				{
					delp = (ENTRY *)nodea.keystr;		/* use first key in node */
				   psp->highkeynde = gtnptr(psp, delp);

					/* update the high key in previous node */
					/* uses nodeb for left-hand node */
					if (nodea.revrse != -1)
					{
						ret = noderead(psp, &nodeb, nodea.revrse);
						if (ret != CCOK)
							break;

						/* find high key and replace node pointer */
						/* with new P0 node pointer of right hand node */
						highdelp = scandkey(psp, &nodeb, nodea.p0, TRUE);
						if (highdelp)
						{
							/* set new nde val, equivalent to transnde()*/
							highnde = ntointel(psp->highkeynde, psp->wordorder);	/* correct word order */
#ifdef REQUIRES_WORD_ALIGNMENT
							blockmv((char *) gtdataptr(psp, highdelp), (char *) &highnde, SZENDE);
#else
							*(NDEPTR *) (gtdataptr(psp, highdelp)) = *(NDEPTR *) (&highnde);
#endif
						}
						else
						{
							ret = BADNODE;					/* missing entry */
							break;
						}

						/* write out updated high key node */
						ret = nodewrte(psp, &nodeb, nodea.revrse);
						if (ret != CCOK)
							break;
					}

					nodea.p0 = psp->highkeynde;		/* shift down nde # to p0 */
					kdel(delp, &nodea);					/* delete first key entry */
					psp->chghighkey = FALSE;			/* not high key */

					/* if only high key left in node, node is empty */
					if ((nodea.forwrd != -1) && (nodea.count == 0))
					{
						psp->path[nodea.level] = nodea.forwrd;	/* new path */
						psp->entries = 0;					/* 0 node ptr entries */
					}
					else
					{
						psp->entries = nodea.count + 1;	/* # of node ptr entries */
					}
				}
				else
				{
					psp->entries = 0;						/* # of node ptr entries */
				}

				break;									/* found match, done */
			}
			else		/* else remove key entry containg node # */
			{
				delp = scandkey(psp, &nodea, psp->lowernde, FALSE);
				if (delp)
				{
					/* merge deleted entry with next higher entry */
					highdelp = (ENTRY *)moveup((char *)delp);	/* ptr to higher key */
					if (gtentlen(highdelp))		/* if have higher entry */
					{
						psp->highkeynde = gtnptr(psp, highdelp);
						/* set highkey ndeptr into next lower key, equivalent to transnde()*/
						highnde = ntointel(psp->highkeynde, psp->wordorder);	/* correct word order */
#ifdef REQUIRES_WORD_ALIGNMENT
						blockmv((char *) gtdataptr(psp, delp), (char *) &highnde, SZENDE);
#else
						*(NDEPTR *) (gtdataptr(psp, delp)) = *(NDEPTR *) (&highnde);
#endif
						blockmv(psp->highkeybuf, (char *)delp, (gtentlen(delp) + 1));
						kdel(highdelp, &nodea);				/* delete old high key */
					}
					else
					{
						kdel(delp, &nodea);				/* delete last reference */
					}

					psp->entries = nodea.count + 1;		/* # of node ptr entries */
					break;
				}
				else
				{
					if (nodea.forwrd == -1)				/* past end */
					{
						ret = BADNODE;						/* missing entry */
						break;
					}
				}

				/* move right looking for match */
				psp->uppernde = nodea.forwrd;
			}
		}
	}

	return(ret);
}

/* scans index node and returns key pointer - place to delete */
ENTRY * FDECL scandkey(psp, ndea, leftnode, highflg)
struct passparm *psp;	/* file key info */
struct node *ndea;		/* node to scan */
NDEPTR leftnode;			/* node number of node a below */
int highflg;				/* if TRUE, include high keys for deletions */
{
	register int i;	
	register char *ptr;		/* current key */
	NDEPTR nptr;				/* current node pointer */
	ENTRY *fndptr;				/* ptr to matching entry */

	/**/

	ptr = ndea->keystr;							/* ptr to first key */
   nptr = gtnptr(psp, (ENTRY *)ptr);		/* set to node pointer of key */
	fndptr = (ENTRY *)0;

	for (i = 0;i < ndea->count;i++) 
	{
	   if (nptr == leftnode)					/* look for old node pointer */ 
		{
			if (	(i != (ndea->count-1))
				|| (ndea->forwrd == -1)
				|| (highflg == TRUE)
				)
				fndptr = (ENTRY *)ptr;			/* match (else high key) */

	      break;									/* done searching */
		}

	   ptr = moveup(ptr);						/* and point to next key */
	   nptr = gtnptr(psp, (ENTRY *)ptr);	/* set to node pointer of key */
	}

	if (highflg == FALSE)
	{
		if (fndptr && (i == (ndea->count-2)) && (ndea->forwrd != -1))
			psp->chghighkey = TRUE;			/* this will require changing high key */
		else
			psp->chghighkey = FALSE;		/* no change to high key */
	}

	return(fndptr);
}



int FDECL delkey(psp)		/* delete key */
register struct passparm *psp;
{
	ENTRY *ptr;
	int ret;
	char keybuf[KEYSZE];				/* buffer to save deleted key value */

	/**/

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	ptr = treesrch(psp, psp->keyn,psp->key, psp->rec, 0);
	/* find key */

	if (!ptr)
	   return(psp->retcde = BADNODE);

	ret = setparm(psp, ptr);

	if (ret == CCOK)			/* found exact key */
	{
#ifdef REMOVED
		strcpy(keybuf, gtkeyptr(ptr));		/* save full key for treesrch */
#endif
		cikeycpy(psp, psp->keyn, keybuf, gtkeyptr(ptr));	/* save full key for treesrch */
	   ret = kysdel(psp, ptr, getndenum(psp, psp->keyn));	/* delete all keys */
		if (ret == -1)
			ret = DSKERR;
		else
			ret = CCOK;

		/* relocate tree path if delete upper portions of index */
		if (psp->fulldel && (psp->treeindex != psp->keyn) && (ret == CCOK))
		{
			ptr = treesrch(psp, psp->keyn, keybuf, psp->rec, 0);
			if (!ptr)
			   ret = BADNODE;
			else
				ret = CCOK;

			/* move back one key to be in right place for cnext */
			if ((ptr = getprev(psp, psp->keyn)) == 0)    /* if no more */
			{   
#ifdef REMOVED
			   psp->curindx[psp->keyn] = psp->curindx[psp->keyn]-1;  
#endif
			   putcurindx(psp, psp->keyn, (getcurindx(psp, psp->keyn) - 1));  
			   /* set current index to one less than before */
			   setnull(psp);		/* null data */
			}
			else 
			{	/* else there is a key */
			   cmovedata(ptr, psp);								/* move in data */
			   putcurindx(psp, psp->keyn, gtkeyn(ptr));	/* current index */

			   /* MU */
			   setcurkey(psp, ptr);
			   psp->retcde = CCOK;	/* good return */
			}

			if (psp->key != psp->retkey)
			{
#ifndef NO_WORD_CONVERSION
				cipspflip(psp, INTELFMT);		/* reset existing key */
#endif	/* !NO_WORD_CONVERSION */
				psp->key = psp->retkey;		/* set for caller */
			}
		}
	}
	else					/* if not exact match */
	   ret = FAIL;		/* no good */

	return(psp->retcde = ret);
}
   

int FDECL delcur(psp)		/* delete current key */
struct passparm *psp;
{
	ENTRY *entptr;
	int ret;
	char keybuf[KEYSZE];				/* buffer to save deleted key value */
	int keynsave;

	/**/

	ret = noderead(psp, &nodea, getndenum(psp, psp->keyn));	/* get leaf */
	if (ret == CCOK)
	{
		if (getdelflag(psp, psp->keyn) == DELON	/* delflag on */
			|| (entptr = cigetkey(&nodea, getkeynum(psp, psp->keyn))) == 0) 
		{	 /* or no valid current */
		   ret = FAIL;	/* no current key */
		}
		else 
		{
			keynsave = gtkeyn(entptr);					/* save for treesrch */
#ifdef REMOVED
			strcpy(keybuf, gtkeyptr(entptr));		/* save full key for treesrch */
#endif
			cikeycpy(psp, psp->keyn, keybuf, gtkeyptr(entptr));	/* save full key for treesrch */
		   ret = kysdel(psp, entptr, getndenum(psp, psp->keyn));	/* else delete cur */
			if (ret == -1)
				ret = DSKERR;
			else
				ret = CCOK;	/* good return */

			/* relocate tree path if delete upper portions of index */
			if (psp->fulldel && (psp->treeindex != psp->keyn))
			{
				entptr = treesrch(psp, keynsave, keybuf, psp->rec, 0);
				if (!entptr)
				   ret = BADNODE;
				else
					ret = CCOK;

				/* move back one key to be in right place for cnext */
				if ((entptr = getprev(psp, psp->keyn)) == 0)    /* if no more */
				{   
				   putcurindx(psp, psp->keyn, (getcurindx(psp, psp->keyn) - 1));
				   /* set current index to one less than before */
				   setnull(psp);		/* null data */
				}
				else 
				{	/* else there is a key */
				   cmovedata(entptr, psp);								/* move in data */
				   putcurindx(psp, psp->keyn, gtkeyn(entptr));	/* current index */
				   setcurkey(psp, entptr);							   /* MU */
				   psp->retcde = CCOK;								/* good return */
				}

				if (psp->key != psp->retkey)
				{
#ifndef NO_WORD_CONVERSION
					cipspflip(psp, INTELFMT);		/* reset existing key */
#endif	/* !NO_WORD_CONVERSION */
					psp->key = psp->retkey;		/* set for caller */
				}
			}
		}
	}

	return(psp->retcde = ret);
}


