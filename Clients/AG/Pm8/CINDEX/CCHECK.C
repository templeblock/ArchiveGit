/*   ccheck.c  -  check record in file for correctness
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



#include "cndx.h"
#include "cindexwk.h"

/* globals for ccheck and bcheck */
static struct passparm *errorpsp;	/* ptr to CFILE for error output file */
CFILE oldfile;								/* info for the old file */
CFILE newfile;								/* info for the new file */
struct node buf;							/* node work buffer */

int EDECL ccheck(chkfile, errfile)	/* check a file for complete data records */
char PDECL *chkfile;		/* name of file to check */
char PDECL *errfile;		/* name of file to put bad records in */
{
	int ret;

	if (!chkfile || !errfile)		/* require file names */
		ret = PARMERR;
	else
		ret = _ccheck(chkfile, errfile, NULL, (STATPROCTYPE)NULL);

	return(ret);
}

int EDECL _ccheck(chkfile, errfile, indlist, statproc)	/* check a file for complete data records */
char PDECL *chkfile;		/* name of file to check */
char PDECL *errfile;		/* name of file to put bad records in */
NDXLIST PDECL *indlist;	/* optional index list (for seg keys), or NULL */
STATPROCTYPE statproc;	/* optional status callback function address */
{
	int ret;
	int cc;
	int i;
	int good;		/* good(CCOK) until bad data found(BAD) */
	NDXLIST indexlist[MAXDINDX+1];/* list of index types */
	int order;							/* byte word order flag */
	NDEPTR nde;
	int oldkeyn;
	char oldkey[KEYSZE];
	long oldrec;
	int olddbyte;

	ENTRY *startptr;		/* starting point of 1st sub-entry of current key */
	NDEPTR startnde;

	ENTRY *entptr;
	ENTRY *nxtentptr;
	
	struct passparm check;		/* for check file */
	struct passparm error;		/* for error file */

	long statcnt;								/* counter indicating progress */

	statcnt = 0;					/* counts 1 for each node read */
#ifdef MAC
	check.dirID = 0;
	check.vRefNum = 0;			/* use default directory */
#endif	/* MAC */
	cc = _bopen(&check, chkfile, EXCL, CRDWRITE);	/* open the input file */
	if (cc != CCOK)
		return(cc);								/* if copen error */

	check.alterr = CCOK;					/* clear alternate error */
	if (!indlist)
	{
		/* read into list the input files index types */
		for (i = 0; i < MAXDINDX; i++)
		{
			indexlist[i].keyn = (unsigned char)(i+1);
			indexlist[i].keytype = check.indtyp[i+1];
			indexlist[i].segptr = NULL;
		}
		indexlist[i].keyn = 0;					/* terminate list */
		indlist = indexlist;						/* use list from check file */
	}

	order = (isintel(check.wordorder)) ? INTELMODE : NATIVEMODE;

	/* for every index set the correct segmented key type */
	for (i = 0; indlist[i].keyn && (i < MAXINDXS); i++)
	{
		check.segptr[indlist[i].keyn] = indlist[i].segptr;
	}

	/* if specified, create the new error file */
	if (errfile)
	{
#ifdef MAC
		error.dirID = 0;
		error.vRefNum = 0;			/* use default directory */
		error.creator = 'CI2 ';
		error.fileType = 'TEST';
#endif	/* MAC */
		cc = bcreate(&error, errfile, EXCL, order, indlist);
		errorpsp = &error;
	}
	else
	{
		errorpsp = NULL;
	}

	if (cc != CCOK)							/* if error in ccreate */
	{
		mclose(&check);						/* close input file */
		return(cc);
	}
	else
	{
		good = CCOK;
		ret = CCOK;

		/* position to first leaf of each root */
		for (i = 0; !check.alterr && (i <= MAXROOTS); i++)
		{
			if (i && !check.rootlist[i-1])
				continue;

			/* status callback */
			if (statproc)
			{
				(statproc)(&check, statcnt++);
		 		if (check.alterr)
		 		{
		 			good = check.alterr;
		 			ret = FAIL;
		 			break;
		 		}
			}

			nde = gtroot(&check, i);
         cc = noderead(&check, &nodea, nde);
			if (cc != CCOK)
			{
            good = BAD;				/* set bad flag */
				ret = cc;
			}
			else
			if (gtlevels(&check) != nodea.level+1)
			{
            good = BAD;				/* set bad flag */
				ret = FAIL;
			}
			else
			{
				/* read down p0 side of tree and locate first key to search */
				while (nodea.level)
				{
					/* status callback */
					if (statproc)
					{
						(statproc)(&check, statcnt++);
						if (check.alterr)
						{
							good = check.alterr;
							ret = FAIL;
							break;
						}
					}

					nde = nodea.p0;
				   cc = noderead(&check, &nodea, nde);
					if (cc != CCOK)
					{
			         good = BAD;				/* set bad flag */
						ret = FAIL;
						break;
					}
				}
			}

			/* clear "previous" record to null */
			oldkeyn = 0;
			cclear(oldkey, sizeof(oldkey), 0);
			oldrec = 0;
			olddbyte = 128;					/* pretend finished prev data */

			/* force this ent to start of 1st rec */
			startptr = (ENTRY *)nodea.keystr;
			startnde = nde;

			while (ret == CCOK)
			{
				entptr = (ENTRY *)nodea.keystr;	/* ptr to first key */
				while (gtentlen(entptr))
				{
				   nxtentptr = (ENTRY *)moveup((char *)entptr);	/* point to next key */

					/* test valid ascending key and not incomplete key at end of tree */
					/* test last entry of previous node */
					cc = bcheckkeytest(&check, entptr, oldkeyn, oldkey, oldrec, olddbyte);
					if ((cc != CCOK) 
						|| (!gtentlen(nxtentptr) 
							&& !(gtdbyte(entptr) & 128) 
							&& (nodea.forwrd == -1)))
					{
				      good = BAD;				/* set bad flag */
						if (olddbyte & 128)	/* prev record OK, delete this subentry */
						{
							cc = cchecksavebad(&check, (char *)entptr, nde);
							if (cc != CCOK)
								return(cc);

							kdel(entptr, &nodea);
							ret = nodewrte(&check, &nodea, nde);
						}
						else
						{
							ret = ccheckdelete(&check, startptr, startnde, &entptr, nde);
						}
						if (ret != CCOK)
							return(ret);
						startptr = entptr;		/* force this ent to start of new rec */
						startnde = nde;
						olddbyte = 128;			/* prev entry was complete */
						continue;
					}

					if (!(gtdbyte(entptr) & 127))
					{
						startptr = entptr;		/* this entry is start of new record */
						startnde = nde;
					}

					oldkeyn = gtkeyn(entptr);
					cikeycpy(&check, oldkeyn, oldkey, gtkeyptr(entptr));	/* save key */
					oldrec = gtkrec(entptr, check.wordorder);
					olddbyte = gtdbyte(entptr);
					entptr = (ENTRY *)moveup((char *)entptr);	/* point to next key */
				}

				nde = nodea.forwrd;
				if (nde != -1)
				{
					/* status callback */
					if (statproc)
					{
						(statproc)(&check, statcnt++);
		 				if (check.alterr)
				 		{
				 			good = check.alterr;
				 			ret = FAIL;
				 			break;
				 		}
					}

		      	ret = noderead(&check, &nodea, nde);
				}
				else
					break;
			}
		}

		if (ret == CCOK)
			ret = cclose(&check);		/* close files */
		else
			cclose(&check);				/* close files without error check */

		if ((good == CCOK) && (ret != CCOK))
			good = ret;

		if (errorpsp)
			ret = mclose(errorpsp);

		if ((good == CCOK) && (ret != CCOK))
			good = ret;
	}

	return(good);
}

/* delete a set of subentries that are not part of a complete record */
int FDECL ccheckdelete(psp, startptr, startnde, endptrvar, endnde)
struct passparm PDECL *psp;
ENTRY PDECL *startptr;
NDEPTR startnde;
ENTRY PDECL * PDECL *endptrvar;
NDEPTR endnde;
{
	int cc;
	NDEPTR nde;
	char *entptr;
	int clrcnt;
	int clrlen;
	char *nptr;
	char *kend;
	ENTRY *endptr;

	endptr = *endptrvar;
	for(	nde = startnde, entptr = (char *)startptr, cc = CCOK;
			cc == CCOK; 
			nde = nodea.forwrd, entptr = nodea.keystr)
	{
		cc = noderead(psp, &nodea, nde);
		if (cc != CCOK)
			break;

		/* calc number of entries and bytes to remove */
		for (	clrcnt = 0, clrlen = 0;
				*entptr && ((nde != endnde) || (entptr != (char *)endptr));
				entptr = moveup(entptr)
			 )
		{
			cc = cchecksavebad(psp, entptr, nde);
			if (cc != CCOK)
				return(cc);

			clrcnt++;
			clrlen += gtentlen(((ENTRY *)entptr)) + 1;
		}

		nptr = entptr;
		kend = nodea.keystr + (unsigned int)(KEYAREA - nodea.free);	/* end of keyarea */
		cmovelt(nptr, (int) (kend - nptr), clrlen);	/* move rest back */
		nodea.free += (INT16)clrlen;						/* alter free space */
		nodea.count -= (INT16)clrcnt;						/* decrement count */
		cclear(kend - clrlen, clrlen, 0);				/* and clear end */

		cc = nodewrte(psp, &nodea, nde);
		
		if (endnde == nde)
		{
			*endptrvar = (ENTRY *)((char *)endptr - clrlen); /* shift down ptr */
			break;					/* done deleting */
		}
	}

	if (cc == CCOK)
		cc = noderead(psp, &nodea, endnde);		/* restore work node */


	return(cc);
}


int FDECL cchecksavebad(psp, entptr, nde)
struct passparm *psp;
char PDECL *entptr;
NDEPTR nde;
{
	char keybuf[KEYSZE];
	char databuf[KEYSZE];
	int dlen;
	int cc;

	if (!errorpsp)
		return(CCOK);			/* no error file, skip this step */

	errorpsp->keyn = psp->keyn = gtkeyn((ENTRY *)entptr);
	if ((errorpsp->keyn < 1) || (errorpsp->keyn > MAXINDXS))
		return(BADNODE);

	dlen = gtentlen(((ENTRY *)entptr));
	if ((dlen == 0) || (dlen > 255))		/* changed 5/20/87 */
		return(BADNODE);

	cikeycpy(errorpsp, errorpsp->keyn, keybuf, gtkeyptr((ENTRY *)entptr));
	errorpsp->rec = gtkrec((ENTRY *)entptr, psp->wordorder);	/* return rec # */
	errorpsp->dbyte = gtdbyte((ENTRY *)entptr);					/* get data byte */
	errorpsp->dlen = goodata(psp, (ENTRY *)entptr);
	if ((errorpsp->dlen < 0) || (errorpsp->dlen > 255))
		return(BADNODE);

	if ((errorpsp->dlen) != 0)  /* if data also present */
	{
		blockmv(	databuf, 
					gtdataptr(psp, (ENTRY *)entptr), 
					errorpsp->dlen);
	   errorpsp->data = databuf;
	}
	else
	   errorpsp->data = NULL;		/* clear ptr */

	errorpsp->key = keybuf;			/* set key */
	errorpsp->keyorder = errorpsp->wordorder;
	cc = addkey(errorpsp, UNQ);	/* add it */
	if (cc != CCOK)
		return(cc);

	/* re-read node being fixed */
	cc = noderead(psp, &nodea, nde);
	return(cc);
}


/* bcheck function ---------------------- test file integrity */

/* globals */
#define BTREELEVELS	20					/* max number of levels in btree */
NDEPTR pathfwd[BTREELEVELS];			/* fwd ptrs on path of tree search */

#ifdef TRANSACTION
struct smallnode nodetold;				/* transaction node being rebuilt */
#endif	/* TRANSACTION */

/* key value structure */
struct keyval
{
	int keyn;
	char key[KEYSZE];
	long rec;
	int dbyte;
};

struct keyval *checkptr;
struct keyval *checkoldptr;

int EDECL bcheck(psp)
struct passparm PDECL *psp;
{
	return(_bcheck(psp, (STATPROCTYPE)NULL));
}

int EDECL _bcheck(psp, statproc)
struct passparm PDECL *psp;
STATPROCTYPE statproc;
{
	int ret;
	int cc;
	NDEPTR nde = 0;
	int i;
	struct keyval checkval;
	struct keyval checkoldval;
	long statcnt;								/* counter indicating progress */

	/**/

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);
	
	/* point to key values for testing */
	checkptr = &checkval;
	checkoldptr = &checkoldval;
	cclear(&checkval, sizeof(checkval), 0);
	cclear(&checkoldval, sizeof(checkoldval), 0);
	statcnt = 0;	/* beginning status count */
	psp->alterr = CCOK;	/* clear alternate error */

	/* assume success */
	psp->badnode = 0;
	psp->badtyp  = 0;
	ret = CCOK;
	checkptr->dbyte = 128;					/* pretend finished prev data entry */

	/* position to first leaf of each root */
	for (i = 0; (i <= MAXROOTS) && (ret == CCOK); i++)
	{
		if (!(checkptr->dbyte & 128))
		{
			psp->badnode = nde;
			psp->badtyp  = BADNDEKEYSEQ;		/* incomplete record */
			ret = FAIL;
			break;
		}

		if (i && !psp->rootlist[i-1])
			continue;

		psp->curroot = i;

		/* clear search path arrays */
		cclear(psp->path, sizeof(psp->path), 0xFF);
		cclear(pathfwd, sizeof(pathfwd), 0xFF);

		/* clear old key info */
		cclear(&checkoldval, sizeof(checkoldval), 0);
		checkoldptr->dbyte = 128;					/* pretend finished prev data */


		/* check header */
		cc = bcheckhdr(psp);
		if (cc != CCOK)
		{
			ret = cc;
		}
		else
		{
			cc = bcheckroot(psp);
			if (cc != CCOK)
			{
				ret = FAIL;
			}
			else
			{
				while (checkptr->keyn)
				{
					while (!nodea.count && (nodea.forwrd != -1))
					{
						/* status callback */
						if (statproc)
						{
							(statproc)(psp, statcnt++);
							if (psp->alterr)
							{
								ret = psp->alterr;
								checkptr->keyn = 0;
								break;
							}
						}

						/* read right in leaf chain */
						nde = nodea.forwrd;
					   cc = noderead(psp, &nodea, nde);
						if (cc != CCOK)	
						{
							psp->badnode = nodea.forwrd;
							psp->badtyp  = BADNDEREAD;
						   cc = endwrit(psp);					/* unlock file */
							if (cc != CCOK)
								ret = cc;								/* use latest error */
							else
								ret = FAIL;
							return(ret);
						}

						if (!nodea.count)
						{
							cclear(pathfwd, sizeof(pathfwd), 0xFF);	/* skips in path */
							cclear(psp->path, sizeof(psp->path), 0xFF);
						}

						checkptr->keyn = 0;
					   cc = bchecknde(psp, &nodea, nde);
						if (cc != CCOK)	
						{
						   cc = endwrit(psp);					/* unlock file */
							if (cc != CCOK)
								ret = cc;								/* use latest error */
							else
								ret = FAIL;
							return(ret);
						}
					}

					if (!checkptr->keyn)
						break;					/* stop processing at last leaf */

					cc = treecheck(psp, checkptr->keyn, checkptr->key, checkptr->rec, checkptr->dbyte);
					if ((cc == CCOK) && (nodea.forwrd != -1))
					{
						nodea.count = 0;			/* force skip to next node */
					}
					else
					{
						if (cc != CCOK)
							ret = FAIL;

						break;						/* stop on error or end of file */
					}

					checkoldptr->keyn = checkptr->keyn;
					cikeycpy(psp, checkptr->keyn, checkoldptr->key, checkptr->key);	/* save key */
					checkoldptr->rec = checkptr->rec;
					checkoldptr->dbyte = checkptr->dbyte;
				}		/* endwhile of tree check */

			}
		}
	}

	/* test deleted node chain */
	nde = psp->freenodes;
	while ((nde != -1) && (ret == CCOK))
	{
		/* status callback */
		if (statproc)
		{
			(statproc)(psp, statcnt++);
			if (psp->alterr)
			{
				ret = psp->alterr;
				break;
			}
		}

		/* read through delete chain */
		cc = noderead(psp, &nodea, nde);
		if (cc != CCOK)	
		{
			psp->badnode = nde;
			psp->badtyp  = BADNDEREAD;
			ret = FAIL;
			break;
		}

		if (	(nodea.level != -1)
			|| (nodea.p0 != -1)
			|| (nodea.count != 0)
			|| (nodea.free != (KEYAREA * MAXCOMP))
			|| (nodea.revrse != -1)
			)
		{
			psp->badnode = nde;
			psp->badtyp  = BADFREECHAIN;
			ret = FAIL;
			break;
		}

		nde = nodea.forwrd;			/* next node in freechain */
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;								/* use latest error */

	return(ret);
}


/* check integrity of header */
int FDECL bcheckhdr(psp)
struct passparm PDECL *psp;
{
	int ret;
	int i;
	int rootnum;

	/**/

	if (	(checkpsp(psp) != PSPOPEN)		/* "VALIDATE" missing */
		||	(gtlevels(psp) > 19)				/* too many levels in tree */
		||	(psp->freenodes < -1)			/* invalid freenode pointer */
		||	(psp->custlen > MAXCUSTINFO)	/* invalid custlen */
		||	(psp->next > psp->endofile)	/* invalid next avail node pointer */
		||	(gtroot(psp, -1) > psp->next)			/* invalid root pointer */
		)
	{
		psp->badnode = -1;				/* bad header */
		psp->badtyp  = BADHDRINFO;		/* failed reading */
		ret = FAIL;
	}
	else
		ret = CCOK;

	for (i = 0; i < NUMINDXS; i++)
	{
		rootnum = psp->rootnum[i];
		if (	(rootnum < 0) 
			|| (rootnum > MAXROOTS)
			|| (rootnum && (psp->levelslist[rootnum-1] == 0))
			|| (rootnum && (psp->levelslist[rootnum-1] > 19))
			|| (rootnum && (psp->rootlist[rootnum-1] == 0))
			|| (rootnum && (psp->rootlist[rootnum-1] > psp->next))
			)
		{
			psp->badnode = -1;				/* bad header */
			psp->badtyp  = BADHDRINFO;		/* failed reading */
			ret = FAIL;
		}
	}

	return(ret);
}


/* read and check root node, then extract ptr info */
int FDECL bcheckroot(psp)
struct passparm PDECL *psp;
{
	int ret;
	int cc;
	NDEPTR nde;

	ret = CCOK;

	/* check root node */
	nde = gtroot(psp, -1);
   cc = noderead(psp, &nodea, nde);
	if (cc != CCOK)
	{
		psp->badnode = -1;		/* something wrong with header ? */
		psp->badtyp  = BADROOT;	/* failed reading root */
		ret = FAIL;
	}
	else
	if (gtlevels(psp) != nodea.level+1)
	{
		psp->badnode = nde;		/* something wrong with header ? */
		psp->badtyp  = BADROOT;	/* failed reading root */
		ret = FAIL;
	}
	else
	{
		/* read down p0 side of tree and locate first key to search */
		while (nodea.level)
		{
			nde = nodea.p0;
		   cc = noderead(psp, &nodea, nde);
			if (cc != CCOK)
			{
				psp->badnode = nde;			/* error reading node */
				psp->badtyp  = BADNDEREAD;
				ret = FAIL;
				break;
			}
		}

		if ((!nodea.level) && (ret == CCOK))			/* if at leaf */
		{
			if (!nodea.count)		/* if empty left hand leaf */
			{
				while (!nodea.count && (nodea.forwrd != -1))
				{
					/* read right in leaf chain */
				   cc = noderead(psp, &nodea, nodea.forwrd);
					if (cc != CCOK)	
					{
						psp->badnode = nodea.forwrd;
						psp->badtyp  = BADNDEREAD;
						ret = FAIL;
						break;
					}

				   cc = bchecknde(psp, &nodea, nodea.forwrd);
					if (cc != CCOK)	
					{
						ret = FAIL;
						break;
					}
				}
			}
			else
			{
			   cc = bchecknde(psp, &nodea, nde);
				if (cc != CCOK)	
				{
					ret = FAIL;
				}
			}
		}
	}

	return(ret);
}

/* searches tree - returns ptr to key or 0 if error - sets current info */
int FDECL treecheck(psp, keyn, key, rec, dbyte)
register struct passparm *psp;
int keyn;	/* index # of key */
char PDECL *key;	/* key to search for */
long rec;	/* record number to search for */
int dbyte;	/* data byte to search for */
{
	int cnt;						/* count of key found */
	register NDEPTR nde;		/* node number of root */
	ENTRY *entptr;
	int oldlevel;
	int cc;

	/* normal tree scan from top of index */
	psp->nextkeyn = keyn;					/* assume next node has same index */
	nde = gtroot(psp, -1);					/* set to root node to start */
	cc = noderead(psp, &nodea, nde);		/* read root */
	if (cc != CCOK)
	{
		psp->badnode = nde;
		psp->badtyp  = BADROOT;
		return(cc);
	}

	while (nodea.level) 	/* while not a leaf node */
	{	
	   oldlevel = nodea.level;
		if (psp->path[oldlevel] != nde)
		{
			if (	(pathfwd[oldlevel] != -1)
				&& (pathfwd[oldlevel] != nde))
			{
				psp->badnode = psp->path[oldlevel];
				psp->badtyp  = BADNDEFWD;
				return(FAIL);
			}

			if (	(psp->path[oldlevel] != -1)
				&&	(nodea.revrse != psp->path[oldlevel]))
			{
				psp->badnode = nde;
				psp->badtyp  = BADNDEREV;
				return(FAIL);
			}
		}

	   psp->path[oldlevel] = nde;	/* set search path for this level */

	   nde = scaniptr(&nodea, keyn, key, rec, dbyte, psp, psp->wordorder);   /* find lower node */
	   if (nde == -1)	/* error in scan */
		{
			psp->badnode = nde;
			psp->badtyp  = BADNDESCAN;
			return(FAIL);
		}

	   cc = noderead(psp, &nodea, nde);		/* and read lower node */
	   if (cc != CCOK)							/* if error reading */
		{
			psp->badnode = nde;
			psp->badtyp  = BADNDEREAD;
	      return(FAIL);			/* error */
		}

	   if (oldlevel != nodea.level + 1)		/* if bad path down tree */
		{
			psp->badnode = nde;
			psp->badtyp  = BADNDEDOWN;
	      return(FAIL);			/* error */
		}
	}

	/* now at leaf level */

	/* test fwd and rev pointers */
	if (psp->path[nodea.level] != nde)
	{
		if (	(pathfwd[nodea.level] != -1)
			&& (pathfwd[nodea.level] != nde))
		{
			psp->badnode = psp->path[nodea.level];
			psp->badtyp  = BADNDEFWD;
			return(FAIL);
		}

		if (	(psp->path[nodea.level] != -1)
			&&	(nodea.revrse != psp->path[nodea.level]))
		{
			psp->badnode = nde;
			psp->badtyp  = BADNDEREV;
			return(FAIL);
		}
	}
	psp->path[nodea.level] = nde;	/* set search path for leaf level */
	entptr = bcheckscanlnde(&nodea, keyn, key, rec, dbyte, &cnt, psp);  /* find key */
	/* if bad leaf scan, or key doesn't match */
	if ((entptr == 0) || psp->keycmpres)
	{
		psp->badnode = nde;
		psp->badtyp  = BADNDESCAN;
		return(FAIL);
	}


	return(CCOK);
}
 		
					
/* check integrity of individual node - used by bcheck */
int FDECL bchecknde(psp, nod, nde)
struct passparm PDECL *psp;
struct node PDECL *nod;
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
	if ((nod->free < 0) || (nod->free > (KEYAREA * MAXCOMP)))
	{
		psp->badtyp  = BADNDEFREE;
		errflg = TRUE;
	}
	else
	if ((nod->count < 0) || (nod->count > MAXENTRIES))
	{
		psp->badtyp  = BADNDECOUNT;
		errflg = TRUE;
	}
	else	/* forward and reverse pointers point to each other */
	if (	(nod->forwrd != -1)
		&& (nod->forwrd == nod->revrse)
		&& (nod->level  >= 0) )
	{
		psp->badtyp  = BADNDESAME;
		errflg = TRUE;
	}
	else	/* test the forward node pointer in node header */
	if (nod->forwrd < -1)
	{
		psp->badtyp  = BADNDEFWD;
		errflg = TRUE;
	}
	else	/* test the reverse node pointer in node header */
	if (nod->revrse < -1)
	{
		psp->badtyp  = BADNDEREV;
		errflg = TRUE;
	}
	else
	{
		p = nod->keystr;
		for (i = 0; i < nod->count; i++)
		{
			checkptr->keyn = psp->keyn = gtkeyn((ENTRY *)p);
			if ((checkptr->keyn < 1) || (checkptr->keyn > MAXINDXS))
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

			if (nod->level)
			{
				ndeptr = gtnptr(psp, (ENTRY *)p);
				if ((ndeptr >= psp->next) || (ndeptr < 0))
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

				/* save last leaf entry for next tree search */
				if (i == nod->count-1)
				{
					psp->keyn = checkptr->keyn;
					cikeycpy(psp, checkptr->keyn, checkptr->key, gtkeyptr((ENTRY *)p));	/* move in key */
					checkptr->rec = gtkrec((ENTRY *)p, psp->wordorder);
					checkptr->dbyte = gtdbyte((ENTRY *)p);
				}
			}

			p = moveup(p);
			if ((p - nod->keystr) > (KEYAREA * MAXCOMP))
			{
				/* too many keys in node, or corrupted key values */
				psp->badtyp = BADNDECOUNT;
				errflg = TRUE;
				break;
			}
		}

		if (!errflg)
		{
			for (;(p < (nod->keystr + KEYAREA)); p++)
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

/* scans leaf node and returns key pointer */
ENTRY PDECL * FDECL bcheckscanlnde(ndea, keyn, key, rec, dbyte, cnt, psp)
struct node PDECL *ndea;	/* node to scan */
int keyn;				/* index number */
char PDECL *key;				/* key to compare against */
long rec;				/* record number to compare */
int dbyte;				/* data byte to compare */
int *cnt;				/* where to put key count of key found */
CFILE PDECL *psp;				/* in case of read right */
{
	register int i;	
	register char PDECL *ptr;		/* ptr into node */
	ENTRY PDECL *prevptr;
	char PDECL *leafentlist[MAXENTRIES];/* ptr list to each entry in node for leaf scan */

	int leftlmt;					/* left search limit number */
	int rightlmt;					/* right search limit number */
	int cmpnum;						/* comparison item number */
	int res;							/* result of comparison */
	int cc;

	if (ndea->count > MAXENTRIES)
	   return((ENTRY PDECL *)NULL);

	if (!ndea->count)				/* if empty node */
	{
		psp->keycmpres = 0;		/* assume non-existent key is OK */
		psp->strcmpres = 0;
		*cnt = 0;					/* ptr to beg of empty node */
		return((ENTRY PDECL *)ndea->keystr);
	}

	/* build list of pointers to each entry in node */
	ptr = ndea->keystr;	/* ptr to first key */
	for (i = 0;i <= ndea->count;i++) 
	{
		/* test valid ascending key */
		if (i == 0)
		{
			cc = bcheckkeytest(psp, (ENTRY PDECL *)ptr, checkoldptr->keyn, checkoldptr->key, checkoldptr->rec, checkoldptr->dbyte);
			if (cc != CCOK)
			{
				psp->badnode = ndea->nde;
				psp->badtyp  = BADNDEKEYSEQ;
		      return((ENTRY PDECL *)NULL);
			}
		}
		else
		if (i < ndea->count)
		{
			prevptr = (ENTRY PDECL *)leafentlist[i-1];
			cc = bcheckkeytest(psp, (ENTRY *)ptr, 
						gtkeyn(prevptr), gtkeyptr(prevptr), 
						gtkrec(prevptr, psp->wordorder), gtdbyte(prevptr));
			if (cc != CCOK)
			{
				psp->badnode = ndea->nde;
				psp->badtyp  = BADNDEKEYSEQ;
		      return((ENTRY PDECL *)NULL);
			}
		}

	   if (ptr > ndea->keystr + (KEYAREA * MAXCOMP))	/* past end */
	      return((ENTRY PDECL *)NULL);

		leafentlist[i] = ptr;
	   ptr = moveup(ptr);								/* point to next key */
	}

	leftlmt = 0;						/* leftmost entry number */
	rightlmt = ndea->count - 1;	/* rightmost entry number */
	cmpnum = rightlmt;				/* make first comparison at right end */

	for (;;)
	{
	    /* if key is >= ptr */
	   res = cikeytest(psp, (ENTRY *)leafentlist[cmpnum], 
								keyn, key, rec, dbyte, psp->wordorder);
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
			   cikeytest(psp, (ENTRY *)leafentlist[cmpnum], 
								keyn, key, rec, dbyte, psp->wordorder);
				break;
			}

			leftlmt = cmpnum + 1;
		}

		cmpnum = ((leftlmt+rightlmt) / 2);
	}


	*cnt = cmpnum;										/* set key count */
	return((ENTRY PDECL *)leafentlist[cmpnum]);		/* return pointer to desired item */
}


/* compares entry at ptr to key parameters */
/* returns CCOK if "ptr" next key is next in sequence */
int FDECL bcheckkeytest(psp, ptr, keyn, key, rec, dbyte)
struct passparm PDECL *psp;	/* file info */
register ENTRY PDECL *ptr;		/* ptr to node entry (next key in sequence) */
int keyn;					/* index number from prev node */
void PDECL *key;					/* search key from prev node */
long rec;					/* record number from prev node */
int dbyte;					/* data byte from prev node */
{
	register int res;
	register long rres;
	register int ret = CCOK;
	int newdbyte;
#ifndef NO_WORD_CONVERSION
	char fliptstkey[MAXMKLEN];		/* space to put byte flipped key from caller */
	char flipentkey[MAXMKLEN];		/* space to put byte flipped key from index */
#endif	/* !NO_WORD_CONVERSION */

	newdbyte = gtdbyte(ptr);
	res = gtkeyn(ptr) - keyn;
	if (res < 0)
	{
		ret = FAIL;					/* cannot go down in index number */
	}
	else
	if (res > 0)
	{
		if ((dbyte & 128) && !(newdbyte & 127))
			ret = CCOK;					/* next higher key, OK */
		else
			ret = FAIL;					/* was in middle of record */
	}
	else										/* test data byte */
	{
#ifndef NO_WORD_CONVERSION
		if (	(INTELFMT && !isintel(psp->wordorder))
			|| (!INTELFMT && isintel(psp->wordorder)) )
		{
			/* flip test and entry keys first */
			cikeycpy(psp, keyn, fliptstkey, key);
			cikeyflip(psp, keyn, fliptstkey, psp->indtyp[keyn] & INDTYPEMASK, psp->wordorder, INTELFMT);
			cikeycpy(psp, keyn, flipentkey, gtkeyptr(ptr));
			cikeyflip(psp, keyn, flipentkey, psp->indtyp[keyn] & INDTYPEMASK, psp->wordorder, INTELFMT);
			res = cikeycmp(psp, keyn, (psp->indtyp[keyn] & INDTYPEMASK), flipentkey, fliptstkey);
		}
		else
		{
			res = cikeycmp(psp, keyn, (psp->indtyp[keyn] & INDTYPEMASK), gtkeyptr(ptr), key);
		}
#else		/* no flipping code */
		res = cikeycmp(psp, keyn, (psp->indtyp[keyn] & INDTYPEMASK), gtkeyptr(ptr), key);
#endif	/* !NO_WORD_CONVERSION */

		if (res < 0)
		{
			ret = FAIL;					/* cannot go down in key value */
		}
		else
		if (res > 0)
		{
			if ((dbyte & 128) && !(newdbyte & 127))
				ret = CCOK;					/* next higher key, OK */
			else
				ret = FAIL;					/* was in middle of record */
		}
		else										/* test data byte */
		{
			rres = gtkrec(ptr, psp->wordorder) - rec;
			if (rres < 0)
			{
				ret = FAIL;					/* cannot go down in key value */
			}
			else
			if (rres > 0)
			{
				if ((dbyte & 128) && !(newdbyte & 127))
					ret = CCOK;					/* next higher key, OK */
				else
					ret = FAIL;					/* was in middle of record */
			}
			else										/* test data byte */
			{
				if (dbyte & 128)
				{
					ret = FAIL;			/* can't have finished same key */
				}
				else
				{
					res = (newdbyte & 127) - dbyte;
					if (res <= 0)
					{
						ret = FAIL;		/* key can't be less than or equal to prev */
					}
					else
					if (res != 1)
					{
						ret = FAIL;					/* can't skip data piece */
					}
					else
						ret = CCOK;
				}
			}
		}
	}

	return(ret);
}

		


/* rebuild utility functions */

int EDECL cbuild(old, new, indextypes)	/* rebuild file */
char PDECL *old;								/* old file name - file to read */
char PDECL *new;								/* new file name - file to write to */
NDXLIST PDECL *indextypes;					/* index type list (or NULL) */
{
	FILHND oldfd;	/* for old file */
	int ret;
	int cc;
	NDEPTR nde;
	int i;
	struct hdrrec hdr;
	NDXLIST indexlist[MAXDINDX+1];	/* list of index types from header */
	unsigned int j;
	int order;								/* byte word order flag */
	int altroot;							/* alternate root number */

	/**/

#ifdef MAC
	oldfile.dirID = 0;
	oldfile.vRefNum = 0;			/* use default directory */
#endif	/* MAC */
	oldfd =mexlopen(&oldfile, old, CRDWRITE);
	if (oldfd == NULLFD)							/* if bad open */
	   return(OLDERR);

	ret = CCOK;
	cc = CCOK;

	/* transfer header information from old file to new file */
	if (!fileSeek(&oldfile, 0L, 0, 0L))		/* if no error, read header */
	{
		/* process header info */
	   if (!ciread(&oldfile, (char *) &hdr, HDRAREA))
		{
			/* word order of input file */
			order = (isintel(hdr.hwordorder)) ? INTELMODE : NATIVEMODE;

			/* create new file */
			if (indextypes)
			{
				/* create file with user supplied list */
#ifdef MAC
				newfile.dirID = 0;
				newfile.vRefNum = 0;			/* use default directory */
				newfile.creator = 'CI2 ';
				newfile.fileType = 'TEST';
#endif	/* MAC */
				if (bcreate(&newfile, new, EXCL, order, indextypes) != CCOK)
				   return(NEWERR);
			}
			else
			{
				/* create file with list from file header */

				/* read into list the input files index types */
				for (j = 0; j < MAXDINDX; j++)
				{
					indexlist[j].keyn = (unsigned char)(j+1);
					indexlist[j].keytype = hdr.hindtyp[j+1];
					indexlist[j].segptr = NULL;
				}
				indexlist[j].keyn = 0;					/* terminate list */

#ifdef MAC
				newfile.dirID = 0;
				newfile.vRefNum = 0;			/* use default directory */
				newfile.creator = 'CI2 ';
				newfile.fileType = 'TEST';
#endif	/* MAC */
				if (bcreate(&newfile, new, EXCL, order, indexlist) != CCOK)
				   return(NEWERR);
			}

			if (cc != CCOK)
				ret = cc;			/* error changing index types */

			newfile.nxtdrec = hdr.hnxtdrec;				/* next avail rec # */
			newfile.delcnt = hdr.hdelcnt;					/* delete count */
			newfile.reccnt = hdr.hreccnt;					/* record count */
			
			/* copy custom header info */
			newfile.custlen = (int)hdr.hcustlen;	/* length of custom info */
			if (	(newfile.custlen > 0)
				&& (newfile.custlen <= MAXCUSTINFO))	/* move custom info */
				blockmv(newfile.custinfo, hdr.hcustinfo, newfile.custlen);
			else
				newfile.custlen = 0;							/* else no custom info */

			/* create alternate roots in new file like original file */
			for (i = 1; ((i <= MAXINDXS) && (ret == CCOK)); i++)
			{
				altroot = hdr.hrootnum[i-1];
				if (altroot && (altroot > 1) && (altroot <= MAXROOTS))
					ret = addroot(&newfile, i, altroot);
			}
		}
	}

	for (nde = 0; diskread(&oldfile, &buf, nde) == CCOK; nde++)	
	{	/* while nodes left */
		/* convert node header to local format */
		nodetonative(&buf, newfile.wordorder);
	   if (buf.level == 0)		/* leaf node */
	      cc = addall(&buf, &newfile);	/* add all keys */
		if (cc != CCOK)
			ret = cc;
	}

	cc = cclose(&newfile);		/* close new file */
	if (ret == CCOK)
		ret = cc;

	mexlclose(&oldfile);			/* close old file */

	return(ret);
}


/* rebuild file into existing file (created by application), no callback */
int EDECL bbuild(oldname, newpsp, oldbytemode)
char PDECL *oldname;	/* old file name - file to read */
CFILE PDECL *newpsp;	/* open psp of new file */
int oldbytemode;		/* INTELMODE, NONINTELMODE or NATIVEMODE of old file */
{
	return(_bbuild(oldname, newpsp, oldbytemode, (STATPROCTYPE)NULL));
}

/*
 * rebuild file into existing file (created by application)
 * optional callback for status
 */

int EDECL _bbuild(oldname, newpsp, oldbytemode, statproc)
char PDECL *oldname;	/* old file name - file to read */
CFILE PDECL *newpsp;	/* open psp of new file */
int oldbytemode;		/* INTELMODE, NONINTELMODE or NATIVEMODE of old file */
STATPROCTYPE statproc;	/* optional status callback (or NULL) */
{
	CFILE oldfile;	/* info for the old file */
	FILHND oldfd;	/* for old file */
	int ret;
	int cc;
	struct node buf;
	NDEPTR nde;
	struct hdrrec hdr;
	long statcnt = 0;
#ifdef TRANSACTION
	TRANSENT *transptr;		/* ptr to 1st item in transaction log node */
	long transseq;				/* sequence number of item in transaction node */
	long transact;				/* transaction number of item in node */
	int transcnt;				/* count of transaction nodes encountered */
	long ndecnt;				/* count of nodes processed */
	char *ptr;					/* work pointer */
	int lastseq;				/* prev transaction sequence # */
	int i;
	int transhdrlen;
	TRANSENT wrkent;					/* to build entry */
	char *lowptr;
	char *hiptr;
#endif	/* TRANSACTION */

	/**/

	ret = csetfile(newpsp, 0);				/* set to fastest processing */
	if (ret)
		return(ret);

	if (oldbytemode == INTELMODE)			/* set header to intel value of 1 */
	{
		oldfile.wordorder = 0;					/* clear all parts of flag */
		*((char *)&oldfile.wordorder) = 1;	/* set first byte */
	}
	else
	if (oldbytemode == NONINTELMODE)			/* set header to non-intel value of 1 */
	{
		oldfile.wordorder = 0;					/* clear all parts of flag */
		*((char *)&oldfile.wordorder + 3) = 1;	/* set last byte */
	}
	else
	if (oldbytemode == NATIVEMODE)			/* set header to native value of 1 */
	{
		oldfile.wordorder = 1;
	}
	else
		return(BADBYTEMODE);

	if (oldfile.wordorder != newpsp->wordorder)
		return(BYTEMISMATCH);

#ifdef TRANSACTION
	/* determine size of transaction header */
	lowptr = (char *)&wrkent;
	hiptr = (char *)&(wrkent.transent);
	transhdrlen =  (int)(hiptr - lowptr);
#endif	/* TRANSACTION */

#ifdef MAC
	oldfile.dirID = 0;
	oldfile.vRefNum = 0;			/* use default directory */
#endif	/* MAC */
	oldfd =mexlopen(&oldfile, oldname, CRDWRITE);
	if (oldfd == NULLFD)							/* if bad open */
	   return(OLDERR);

#ifdef REMOVED
	ret = CCOK;
	cc = CCOK;
#endif
	oldfile.alterr = CCOK;		/* clear alternate error (for PROCINTR) */
	strncpy(oldfile.filename, oldname, MAXNAMELEN);		/* put file name in psp */

	/* transfer header information from old file to new file */
	ret = fileSeek(&oldfile, 0L, 0, 0L);
	if (!ret)		/* if no error, read header */
	{
		/* process header info */
	   if (!ciread(&oldfile, (char *) &hdr, HDRAREA))
		{
			hdrtonative(&hdr, oldfile.wordorder);	/* convert intel header to native */
			newpsp->nxtdrec = hdr.hnxtdrec;				/* next avail rec # */
			newpsp->delcnt = hdr.hdelcnt;					/* delete count */
			newpsp->reccnt = hdr.hreccnt;					/* record count */
			
			/* copy custom header info */
			newpsp->custlen = (int)hdr.hcustlen;	/* length of custom info */
			if (	(newpsp->custlen > 0)
				&& (newpsp->custlen <= MAXCUSTINFO))	/* move custom info */
				blockmv(newpsp->custinfo, hdr.hcustinfo, newpsp->custlen);
			else
				newpsp->custlen = 0;							/* else no custom info */
		}
	}

#ifdef TRANSACTION
	transcnt = 0;
#endif	/* TRANSACTION */
	for (nde = 0; diskread(&oldfile, &buf, nde) == CCOK; nde++)	
	{	/* while nodes left */
		/* status callback */
		if (statproc)
		{
			(statproc)(&oldfile, statcnt++);
			if (oldfile.alterr)
			{
				ret = oldfile.alterr;
				break;
			}
		}

		/* convert node header to local format */
		nodetonative(&buf, oldfile.wordorder);
	   if (buf.level == 0)		/* leaf node */
		{
	      cc = addall(&buf, newpsp);	/* add all keys */
			if ((cc != CCOK) && !ret)
				ret = cc;
		}
		else
	   if ((buf.level == -2) || (buf.level == -3))		/* transaction log */
		{
#ifdef TRANSACTION
			if (buf.count > 0)			/* if anything in log node */
			{
				transptr = (TRANSENT *)(buf.keystr);
				transseq = (long)transptr->sequence;	/* get 1st sequence # in node */
				transseq = inteltol(transseq, newpsp->wordorder);
				transact = (long)transptr->transnum;	/* transaction # */
				transact = inteltol(transact, newpsp->wordorder);

				if (transact)		/* if really have something in log node */
				{
					cc = cdupadd(newpsp, TRANSINDX, "TRANSRBLD", transseq, &nde, sizeof(nde));
					if (cc != CCOK)
						ret = TRANSERR;
					else
						transcnt++;
				}
			}
#endif	/* TRANSACTION */
		}
	}

#ifdef TRANSACTION
	/* process each transaction log node in order, adding to new file */
	lastseq = 0;
	for (	ndecnt = 0, cc = CCOK; 
			!oldfile.alterr && (cc == CCOK) && (ndecnt < transcnt); 
			ndecnt++)
	{
		if (ndecnt == 0)
		{
			ret = csetfile(newpsp, 4);	/* set to transaction compatible level */
			if (ret != CCOK)
			{
				ret = TRANSERR;
				break;
			}

			/* locate first transaction node to read */
			newpsp->keyn = TRANSINDX;
			newpsp->key = "TRANSRBLD";
			newpsp->keyorder = INTELFMT;			/* reset key byte ordering to native */
			newpsp->rec = 0;
			newpsp->data = (void *)&nde;
			newpsp->dlen = sizeof(nde);
			ret = _cfind(newpsp);
			if (ret != KEYMATCH)
			{
				ret = DSKERR;
				break;
			}

			ret = transbeg(newpsp, TRANSCHANGE, NULL);
			if (ret != CCOK)
			{
				ret = TRANSERR;
				break;
			}
		}
		else
		{
			/* locate next transaction node to read */
			ret = cnext(newpsp, TRANSINDX, &nde, sizeof(nde));
			if ((ret != CCOK) || strcmp(newpsp->key, "TRANSRBLD"))
			{
				ret = DSKERR;			/* something is wrong with transactions */
				break;
			}
		}

		ret = diskread(&oldfile, &buf, nde);			/* read next trans node */
		if (ret != CCOK)
			break;

		/* add each log entry into new log, checking for correct sequence */
		ptr = buf.keystr;	/* ptr to first key */
		for (i = 0; i < buf.count; i++) 
		{
		   if (ptr >= buf.keystr + KEYAREA)		/* if past end, error */
			{
		      cc = ret = TRANSERR;
				break;
			}

			transptr = (TRANSENT *)ptr;
			transseq = (long)transptr->sequence;	/* get 1st sequence # in node */
			transseq = inteltol(transseq, newpsp->wordorder);
			if (transseq != (lastseq + 1))
			{
		      cc = ret = TRANSERR;
				break;
			}
			lastseq++;

			/* add to new log */
			ret = translog(newpsp, transptr->transtype, (char *)(&(transptr->transent)));
			if (ret != CCOK)
			{
		      cc = ret = TRANSERR;
				break;
			}

			ptr += transhdrlen;							/* skip trans log header */
		   ptr = moveup(ptr);							/* point to next entry */
		}

	}
#endif	/* TRANSACTION */

	cc = cclose(newpsp);		/* close new file */
	if (ret == CCOK)
		ret = cc;

	mexlclose(&oldfile);			/* close old file */

	return(ret);
}





/* add all keys in this node for rebuild */
int FDECL addall(buf, newfile)
struct node *buf;		/* where node is */
struct passparm *newfile;	/* new file passparm */
{
	int dlen;
	int i;
	char *p;
	char keybuf[KEYSZE];
	char databuf[KEYSZE];
	int ret;

	/**/

	if (buf->count > MAXENTRIES)
	   return(BADNODE);

	ret = CCOK;

	for (i = 0, p = buf->keystr; i < buf->count; i++)
	{		/* for every key */
		newfile->keyn = gtkeyn((ENTRY *)p);
		if ((newfile->keyn < 1) || (newfile->keyn > MAXINDXS))
		{
			ret = BADNODE;
			break;
		}

		dlen = ((ENTRY *)p)->entlen;
	   if ((dlen == 0) || (dlen > 255))		/* changed 5/20/87 */
	   {
			ret = BADNODE;
			break;
		}

		cikeycpy(newfile, newfile->keyn, keybuf, gtkeyptr((ENTRY *)p));

	   newfile->rec = gtkrec((ENTRY *)p, newfile->wordorder);	/* return rec # */
	   newfile->dbyte = gtdbyte((ENTRY *)p); 						/* get data byte */
		newfile->dlen = goodata(newfile, (ENTRY *)p);
	   if ((newfile->dlen < 0) || (newfile->dlen > 255))/* changed 5/20/87 */
	   {
			ret = BADNODE;
			break;
		}


	   if ((newfile->dlen) != 0)  /* if data also present */
		{
			blockmv(	databuf, 
						gtdataptr(newfile, (ENTRY *)p), 
						newfile->dlen);
	      newfile->data = databuf;
		}
	   else
	      newfile->data = NULL;		/* clear ptr */

	   newfile->key = keybuf;			/* set key */
		newfile->keyorder = newfile->wordorder;
		if (newfile->keyn != TRANSINDX)		/* add any non-transaction index sub-entries */
		{
		   ret = addkey(newfile, UNQ);		/* add it */
			if (ret != CCOK)
				break;
		}

	   p = moveup(p);	/* advance to next key */

	   if (p > buf->keystr + (KEYAREA * MAXCOMP))
	   {
			ret = BADNODE;
			break;
		}
	}

	return(ret);
}


