/*     diskio.c  -  disk i/o related functions
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

/* extend file necessary amount to accomodate one split */
int FDECL ciextend(psp)
struct passparm *psp;
{
	int ret;
	NDEPTR nde;		/* node number */
	long extendcnt;	/* number of nodes to extend */

	/**/

	ret = CCOK;

	if ((long)(gtlevels(psp) + 1) > (psp->endofile - psp->next)) 
	{
		cclear((char *) &ebuffer, sizeof(ebuffer), 0);	/* make empty node */
	   psp->extflag = TRUE;		/* now have extended */

		/* write to end of file until enough spare room at end */
		extendcnt = (long)(gtlevels(psp) + 1) - (psp->endofile - psp->next);
		if (extendcnt < 0)
			extendcnt = 0;

#ifndef NO_BLOCK_EXTEND
		if (extendcnt && extendcnt < psp->minextend)
			extendcnt = psp->minextend;	/* force extend to be minimum amount */
#endif	/* !NO_BLOCK_EXTEND */

		while (extendcnt--)
		{
			nde = psp->endofile - NODEBIAS;				/* remove test node bias */
			ret = fileSeek(psp, nde, NODESZE, (long)HDRSZE);

			if (!ret)				 	/* if no error, write node */
				ret = ciwrite(psp, (char *)&ebuffer, NODESZE);

#ifdef TRANSACTION
			if (psp->transpsp)	/* if mirroring file, extend mirror file also */
			{
				ret = fileSeek(psp->transpsp, nde, NODESZE, (long)HDRSZE);

				if (!ret)				 	/* if no error, write node */
					ret = ciwrite(psp->transpsp, (char *)&ebuffer, NODESZE);	
			}
#endif	/* TRANSACTION */

		   if (ret != CCOK) 
		      break;		/* end if error */

#ifdef DEMO
		   if (psp->endofile > DEMOLIMIT)
		   {
		      ret = NOSPACE;
		      break;
		   }
#endif

	 	   psp->endofile++;	/* increment end of file pointer */
		}   
	}

	/* if have extended and level > 0 and no disk errors, and exclusive mode */
	if (	psp->extflag
			&& psp->seclev
			&& (psp->seclev < 4)
			&& (ret == CCOK)
			&& (psp->processmode == EXCL))
	{
		ret = upddisk(psp);			/* update disk FAT table (or equiv) */
		psp->extflag = FALSE;		/* clear flag */

		/* if changed header ? and in exclusive mode */
		if (psp->seclev >= HDRUPLEV && ret == CCOK)
		   ret = hdrupdte(psp);		/* need to keep header current */
	}

	return(psp->errcde = ret);
}

/* extend file necessary amount to accomodate multi-key splits */
int FDECL dextend(psp, datalist)
struct passparm *psp;
DATALIST *datalist;
{
	int ret;
	NDEPTR nde;		/* node number */
	int indcnt;
	long extlevels;
	long extendcnt;		/* number of nodes to extend */

	/**/

	ret = CCOK;
	indcnt = 0;

	/* count up the number of indexes to be accessed */
	while (datalist->fldtype != ENDLIST)
	{
		if (datalist->fldindex)			/* if indexed field */
			indcnt++;
		datalist++;
	}
	indcnt++;				/* one extra index for DATAINDX */
	extlevels = (long)((gtlevels(psp) + 1) * indcnt);
	extendcnt = extlevels - (psp->endofile - psp->next);
	if (extendcnt < 0)
		extendcnt = 0;

#ifndef NO_BLOCK_EXTEND
		if (extendcnt && extendcnt < psp->minextend)
			extendcnt = psp->minextend;	/* force extend to be minimum amount */
#endif	/* !NO_BLOCK_EXTEND */

	if (extendcnt)
	{
		cclear((char *) &ebuffer, sizeof(ebuffer), 0);	/* make empty node */
	   psp->extflag = TRUE;		/* now have extended */

		/* write to end of file until enough spare room at end */
		while (extendcnt--) 
		{
			nde = psp->endofile - NODEBIAS;				/* remove test node bias */
			ret = fileSeek(psp, nde, NODESZE, (long)HDRSZE);

			if (!ret)				 	/* if no error, write node */
				ret = ciwrite(psp, (char *)&ebuffer, NODESZE);

#ifdef TRANSACTION
			if (psp->transpsp)	/* if mirroring file, extend mirror file also */
			{
				ret = fileSeek(psp->transpsp, nde, NODESZE, (long)HDRSZE);

				if (!ret)				 	/* if no error, write node */
					ret = ciwrite(psp->transpsp, (char *)&ebuffer, NODESZE);	
			}
#endif	/* TRANSACTION */

		   if (ret != CCOK) 
		      break;		/* end if error */

#ifdef DEMO
		   if (psp->endofile > DEMOLIMIT)
		   {
		      ret = NOSPACE;
		      break;
		   }
#endif

	 	   psp->endofile++;	/* increment end of file pointer */
		}   
	}

	/* if have extended and level > 0 and no disk errors, and exclusive mode */
	if (	psp->extflag
			&& psp->seclev
			&& (psp->seclev < 4)
			&& (ret == CCOK)
			&& (psp->processmode == EXCL))
	{
		ret = upddisk(psp);			/* update disk FAT table (or equiv) */
		psp->extflag = FALSE;		/* clear flag */

		/* if changed header ? and in exclusive mode */
		if (psp->seclev >= HDRUPLEV && ret == CCOK)
		   ret = hdrupdte(psp);		/* need to keep header current */
	}

	return(psp->errcde = ret);
}


long FDECL updread(psp)			/* reads update number from header record */
register struct passparm *psp;	/* psp */
{
	long updnum;

	errcde = fileSeek(psp, 0L, 0, 0L);		/* seek to beginning of file */
	if (errcde == CCOK)	/* if good seek */
		errcde = ciread(psp, (char *)&updnum, sizeof(long));	/* read num */

	if (errcde)
		updnum = 0;							/* error reading */
	else
		updnum = inteltol(updnum, psp->wordorder);

	return(updnum);	
}


int FDECL hdrread(psp, buf)	/* reads header record into buf */
register struct passparm *psp;	/* psp */
register struct hdrrec *buf;		/* where to put it */
{
	errcde = fileSeek(psp, 0L, 0, 0L);		/* seek to beginning of file */
	if (errcde == CCOK)	/* if good seek */
		errcde = ciread(psp, (char *) buf, HDRAREA);	/* read header */

	if (!errcde)
	{
		hdrtonative(buf, buf->hwordorder);	/* convert intel header to native */
		errcde = chkhdr(buf);					/* check header for corruption */
	}

	return(errcde);	
}


int FDECL hdrwrte(psp, buf)	/* writes header record to disk */
register struct passparm *psp;
register struct hdrrec *buf;		/* where header record is */
{
	hdrtointel(buf, psp->wordorder);	/* convert native header to intel format */

	if (fileSeek(psp, 0L, 0, 0L))		/* seek to beginning of file */
	   errcde = DSKERR;
	else
		errcde = ciwrite(psp, (char *) buf, HDRAREA);

	/* convert header back to native in case used again */
	hdrtonative(buf, psp->wordorder);

   return(errcde);
}



/* disk read of node data, returns error code - CCOK if good */
int FDECL diskread(psp, nod, nde)
struct passparm *psp;
struct node *nod;		/* where to put information */
NDEPTR nde;		/* node number */
{
	nde = nde - NODEBIAS;				/* remove test node bias */
	if (nde < 0)
	{
		errcde = INVALIDNDE;				/* invalid node number requested */
	}
	else
	{
		errcde = fileSeek(psp, nde, NODESZE, (long)HDRSZE);
		if (errcde == CCOK) 							/* if no seek error */
		   errcde = ciread(psp, (char *)nod, NODESZE);	/* read into buffer */
	}

	return(errcde);
}


/* disk write of node data, returns error code - CCOK if good */
/* if compression is active, compresses node then writes to disk */
int FDECL diskwrte(psp, nod, nde)
struct passparm *psp;
struct node *nod;		/* where to get information */
NDEPTR nde;		/* node number */
{
	INT16 flag;

#ifdef TRANSACTION
	char nodeval[20];						/* description of node # written for log */
	NDEPTR ndeflip;
#endif	/* TRANSACTION */

	errcde = CCOK;							/* assume ok result */
	if (nde < 0)
	{
		errcde = INVALIDNDE;				/* invalid node number requested */
	}
	else
	{
		/* first time write occurs in lazy write, set write flag */
		if ((psp->seclev >= 4) && psp->lazywrite && !psp->writeactive)
		{
#ifdef READSHARE
			/* if current read enabled, lock all read bytes before writes */
			if (psp->processmode != EXCL)
				errcde = cireadlock(psp, TRUE);	/* lock all read bytes */
#endif		/* READSHARE */
			if (!errcde)
			{
				errcde = fileSeek(psp, (long)WRTBYTE, 0, 0L);
				if (errcde != CCOK)	/* seek error */
				{
				   errcde = DSKERR;
				}
				else
				{
					/* set flag in case of bad write */
					flag = SETUPD;
					errcde = ciwrite(psp, (char *)&flag, sizeof(flag));

					if (!errcde)
					{
						errcde = osbflush(psp);
						psp->writeflag = TRUE;	/* set flag indicating writing to disk */
					}

					psp->writeactive = TRUE;
				}
			}
		}
	}

	if (!errcde)
	{
		nde = nde - NODEBIAS;				/* remove test node bias */
		errcde = fileSeek(psp, nde, NODESZE, (long)HDRSZE);
	}

	if (!errcde)				 	/* if no error, write node */
		errcde = ciwrite(psp, (char *)nod, NODESZE);

#ifdef TRANSACTION
	if (	((psp->transop == TRANSALL) || (psp->transop == TRANSMIRROR))
		&& !errcde
		&& (nod->level != TRANS1NDE)
		&& (nod->level != TRANS2NDE))
	{
		*nodeval = (char)sizeof(nde);
		ndeflip = ltointel(nde, psp->wordorder);
		blockmv(nodeval+1, (char *)&ndeflip, sizeof(ndeflip));
		errcde = translog(psp, TRANSNDEWRIT, nodeval);
	}
#endif	/* TRANSACTION */

	return(errcde);
}

#ifdef TRANSACTION
/* write transaction node to disk */
int FDECL transwrte(psp, nod, nde)
struct passparm *psp;			/* pointer to parameter struct */
struct node *nod;					/* where the node is */
NDEPTR nde;							/* node number */
{
	int ret = CCOK;
	INT16 flag;

	if ((nod->free < 0) || (nod->count < 0))
		return(BADNODE);

	/* get rid of buffered copy of this node */
	ret = clrtransbcb(psp, nde);
	if (ret != CCOK)
		return(ret);

	psp->updtflag = DIRTY;		/* file now modified */

	nod->npsp = psp;			/* set the work buffers psp, fd and node number */
	nod->nfd  = psp->fd;
	nod->nde  = nde;

	nodetointel(nod, psp->wordorder);	/* convert node header to intel format */

	errcde = CCOK;							/* assume ok result */
	if (nde < 0)
	{
		errcde = INVALIDNDE;				/* invalid node number requested */
	}
	else
	{
		/* first time write occurs in lazy write, set write flag */
		if ((psp->seclev >= 4) && psp->lazywrite && !psp->writeactive)
		{
#ifdef READSHARE
			/* if current read enabled, lock all read bytes before writes */
			if (psp->processmode != EXCL)
				errcde = cireadlock(psp, TRUE);	/* lock all read bytes */
#endif		/* READSHARE */
			if (!errcde)
			{
				errcde = fileSeek(psp, (long)WRTBYTE, 0, 0L);
				if (errcde != CCOK)	/* seek error */
				{
				   errcde = DSKERR;
				}
				else
				{
					/* set flag in case of bad write */
					flag = SETUPD;
					errcde = ciwrite(psp, (char *)&flag, sizeof(flag));

					if (!errcde)
					{
						errcde = osbflush(psp);
						psp->writeflag = TRUE;	/* set flag indicating writing to disk */
					}

					psp->writeactive = TRUE;
				}
			}
		}
	}

	if (errcde)
	{
		return(errcde);
	}

#ifdef COMPRESSION
	ret = cishrink(psp, &nodec, nod);	/* compress the node */
	if (ret == CCOK)
	{
		nod->compfree = inteltos(nodec.compfree, psp->wordorder);
		nde = nde - NODEBIAS;				/* remove test node bias */
		ret = fileSeek(psp, nde, NODESZE, (long)HDRSZE);

		if (!ret)				 	/* if no error, write node */
			ret = ciwrite(psp, (char *)&nodec, NODESZE);
	}
#else
	nde = nde - NODEBIAS;				/* remove test node bias */
	ret = fileSeek(psp, nde, NODESZE, (long)HDRSZE);

	if (!ret)				 	/* if no error, write node */
		ret = ciwrite(psp, (char *)nod, NODESZE);
	nodetonative(nod, psp->wordorder);	/* convert node back to local format */
#endif	/* COMPRESSION */

	return(ret);
}

/* read transaction node from disk */
int FDECL transread(psp, nod, nde)
struct passparm *psp;		/* pointer to parameter struct */
struct node *nod;			/* where to put it */
NDEPTR nde;			/* node number */
{
	int ret;

	ret = noderead(psp, nod, nde);		/* read using buffer system */
	if (ret == CCOK)
		ret = clrtransbcb(psp, nde);		/* then clear out of buffers */

	return(ret);
}

#endif		/* TRANSACTION */


