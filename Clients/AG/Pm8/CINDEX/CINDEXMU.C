/*  cindexmu.c - primary source file for cindex
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


/* compiler dependent includes */
#include "cndx.h"			/* general information */
#include "cindexwk.h"		/* internal work information */


#ifdef COMPM				/* want to compile multi-user part of file */
/* multi-user defs */

#define MREAD
#define MWRITE
#define MOPEN
#define MCLOSE
#define MCREATE


#endif



/* multi-user specific code */


#ifdef MREAD

int FDECL testshrbyte(psp, share)		/* tests a share byte of file */
CFILE *psp;		/* open psp */
int share;		/* byte to test */
{
	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (testbyte(psp, (share+SHAREOFF)) == LOCKED)
	      return(CCOK);   /* now have a locked byte */
	return(FAIL);	   /* failed */
}


int FDECL testbyte(psp, byte)	/* test a byte of file with descriptor fd */
struct passparm *psp;
int byte;					/* byte to test */
{
	if (tstlockbyte(psp, byte) == CCOK)	/* must have been unlocked */
		return(UNLOCKED);

	return(LOCKED);
}




int FDECL setshrbyte(psp)		/* returns a share byte of file */
CFILE *psp;		/* open psp */
{
	int i;
	int ret;
	int cc;

	/**/

	if (psp->processmode == EXCL)				/* if in exclusive use mode */
		return(CCOK);								/* don't need to do semaphore op */

	psp->sharenum = -1;				/* assume won't find share byte */

	/* try locking each sharebyte to find one that is not used */
	for (i = 0; i < MAXUSERS; i++)
	{
#ifdef READSHARE
		/* try locking with byte read method first */
		cc = tstlockbyte(psp, (i+SHAREOFF));
		if (cc)
			continue;			/* if already locked, skip to next sharebyte */
#endif	/* READSHARE */

	   if (tlockbyte(psp, (i+SHAREOFF)) == CCOK)
		{
			psp->sharenum = i;				/* now have a locked byte */
			if (psp->readmode == CRDONLY)
			{
				cc = mzapsema(psp);				/* check for stray locks */
				if (cc != BADSHARENUM)			/* if none, use this sharenum */
					break;

				cc = unlockbyte(psp, (i+SHAREOFF));	/* unlock unusable share byte */
				if (cc != CCOK)
					return(FILELOCKED);		/* error unlocking share byte */

				psp->sharenum = -1;				/* didn't find good share byte */
			}
			else
			{
				break;								/* use this sharenum */
			}
		}
	}


	if (psp->sharenum == -1)
		ret = USRERR;				/* too many users, cannot assign share # */
	else
		ret = CCOK;

	return(ret);
}


int FDECL clrshrbyte(psp, share)	/* clears a share byte of file */
CFILE *psp;		/* open psp */
int share;		/* byte to test */
{
	if (psp->processmode == EXCL)				/* if in exclusive use mode */
		return(CCOK);								/* don't need to do semaphore op */

	if (unlockbyte(psp, (share+SHAREOFF)) == 0)
	      return(CCOK);   /* now have a locked byte */
	return(FAIL);	   /* failed */
}

#endif



#ifdef MWRITE

int EDECL msetsema(psp, key, rec, shareval)	/* set semaphore */
CFILE *psp;			/* open psp */
KEYPTR key;			/* key to lock */
long rec;			/* record number */
int shareval;		/* share number */
{
	int ret;
	short sharefnd;			/* share # of semaphore found */
	int   cc;
	int semaind;				/* semaphore index number */
	short sharenum;			/* semaphore value for sharenum */
	short shortshare;			/* work variable for share num */

	KEYPTR tempkptr;			/* temporary psp save variables */
	INT32 tempkmode;
	int   tempindx;
	long  temprec;
	void *tempdptr;
	int   tempdlen;
	char  tempkey[MAXMKLEN];
	int   tempsrchorder;				/* save psp->srchorder before cfind */

	/**/

	semaind = SEMAINDEX;
	shortshare = (short)shareval;		/* convert share # to standard format */
	sharenum = stointel(shortshare, psp->wordorder);

	if (psp->processmode == EXCL)				/* if in exclusive use mode */
		return(CCOK);								/* don't need to do semaphore op */

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	psp->srecurs = TRUE;						/* say we are in semaphore operation */

	tempkptr = psp->key;						/* save psp values */
	tempkmode= psp->keyorder;
	tempindx = psp->keyn;
	temprec = psp->rec;
	tempdptr = psp->data;
	tempdlen = psp->dlen;
	tempsrchorder = psp->srchorder;
	psp->srchorder = 0;					/* force search order to normal */

#ifdef REMOVED
	if ((psp->key == psp->retkey) || (psp->key == (psp->retkey+1)))
		strcpy(tempkey, psp->key);			/* save key value in case in psp */
#endif
	if ((psp->key == psp->retkey) || (psp->key == (psp->retkey+1)))
		cikeycpy(psp, psp->keyn, tempkey, psp->key);	/* save key value in case in psp */

	/* attempt to add semaphore to index */

	ret = cdupadd(psp, semaind, key, rec, &sharenum, (int)sizeof(sharenum));

	if (ret == FAIL)
	{	/* sema exists */
		/* check if process has exited */
	
		psp->keyn = semaind;
		psp->key = key;
		psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
		psp->rec = rec;
		psp->data = (void *)&sharefnd;
		psp->dlen = (int)sizeof(sharefnd);
		ret = _cfind(psp);
		/* if not really locked */
	   if ((ret == CCOK) && (testshrbyte(psp, (int)sharefnd) == FAIL))
	   {
			psp->keyn = semaind;
			psp->key = key;
			psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
			psp->rec = rec;
			ret = _cdelete(psp);
			if (ret == CCOK)										/* retry adding sema */
				ret = cdupadd(psp, semaind, key, rec, &sharenum, (int)sizeof(sharenum));
#ifdef TRANSACTION
			if (ret == CCOK)
				ret = transtst(psp);			/* test for incomplete transactions */
			if (ret == TRANSALLOC)
				ret = CCOK;
#endif	/* TRANSACTION */
	   }
		else
		{
			ret = RECSHLK;				/* record locked by sharing process */
		}
	}

	psp->keyn = tempindx;
	psp->key = tempkptr;
	psp->keyorder = tempkmode;
	psp->rec = temprec;
	psp->data = tempdptr;
	psp->dlen = tempdlen;
	psp->srchorder = tempsrchorder;

	if ((psp->key == psp->retkey) || (psp->key == (psp->retkey+1)))
		cikeycpy(psp, psp->keyn, psp->key, tempkey);	/* restore key value in case in psp */


	cc = endwrit(psp);
	if (cc != CCOK)
		ret = cc;

	psp->srecurs = FALSE;							/* not in semaphore operation */

	return(ret);
}


int EDECL mclrsema(psp, key, rec, shareval)	/* clear semaphore */
CFILE *psp;			/* open psp */
KEYPTR key;			/* key to lock */
long rec;			/* record number */
int shareval;		/* share number */
{
	int ret;
	int cc;
	int semaind;				/* semaphore index number */
	short sharenum;			/* semaphore value for sharenum */
	short shortshare;			/* work variable for share num */
	short sharefnd;			/* share number of semaphore found */

	KEYPTR tempkptr;			/* temporary psp save variables */
	INT32 tempkmode;
	int tempindx;
	long temprec;
	void *tempdptr;
	int tempdlen;
	char  tempkey[MAXMKLEN];
	int   tempsrchorder;				/* save psp->srchorder before cfind */

	/**/

	semaind = SEMAINDEX;
	shortshare = (short)shareval;		/* convert share # to standard format */
	sharenum = stointel(shortshare, psp->wordorder);

	if (psp->processmode == EXCL)				/* if in exclusive use mode */
		return(CCOK);								/* don't need to do semaphore op */

	if (psp->srecurs == TRUE)				/* stop recursion */
		return(CCOK);

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	psp->srecurs = TRUE;						/* say we are in semaphore operation */

	tempkptr = psp->key;						/* save psp values during operation */
	tempkmode= psp->keyorder;
	tempindx = psp->keyn;
	temprec = psp->rec;
	tempdptr = psp->data;
	tempdlen = psp->dlen;
	tempsrchorder = psp->srchorder;
	psp->srchorder = 0;					/* force search order to normal */

	if ((psp->key == psp->retkey) || (psp->key == (psp->retkey+1)))
		cikeycpy(psp, psp->keyn, tempkey, psp->key);	/* save key value in case in psp */

	psp->keyn = semaind;
	psp->key = key;
	psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
	psp->rec = rec;
	psp->data = (void *)&sharefnd;
	psp->dlen = (int)sizeof(sharefnd);
	ret = _cfind(psp);
	if (ret == CCOK)
	{
		if (sharenum == sharefnd)
			ret = cdelcur(psp, semaind);		/* delete the semaphore */
		else
			ret = RECSHLK;
	}

	psp->key  = tempkptr;					/* restore psp values */
	psp->keyorder = tempkmode;
	psp->keyn = tempindx;
	psp->rec  = temprec;
	psp->data = tempdptr;
	psp->dlen = tempdlen;
	psp->srchorder = tempsrchorder;

	if ((psp->key == psp->retkey) || (psp->key == (psp->retkey+1)))
		cikeycpy(psp, psp->keyn, psp->key, tempkey);	/* restore key value in case in psp */


	psp->srecurs = FALSE;					/* not in semaphore operation */

	cc = endwrit(psp);
	if (cc != CCOK)
		ret = cc;

	return(ret);
}


int EDECL mtstsema(psp, key, rec, shareval)	/* test semaphore */
CFILE *psp;			/* open psp */
KEYPTR key;			/* key to lock */
long rec;			/* record number */
int shareval;		/* share number */
{
	int ret;
	int semaind;				/* semaphore index number */
	short sharenum;			/* semaphore value for sharenum */
	short shortshare;			/* work variable for share num */
	short sharefnd;			/* share number found in semaphore */

	KEYPTR tempkptr;			/* temporary psp save variables */
	INT32 tempkmode;
	int   tempindx;
	long  temprec;
	void *tempdptr;
	int   tempdlen;
	char  tempkey[MAXMKLEN];
	int cc;
	int   tempsrchorder;				/* save psp->srchorder before cfind */

	/**/

	semaind = SEMAINDEX;
	shortshare = (short)shareval;		/* convert share # to standard format */
	sharenum = stointel(shortshare, psp->wordorder);

	if (psp->processmode == EXCL)				/* if in exclusive use mode */
		return(RECSFLK);						/* assume record is locked by self */

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	psp->srecurs = TRUE;						/* say we are in semaphore operation */

	tempkptr = psp->key;						/* save psp values during operation */
	tempkmode = psp->keyorder;
	tempindx = psp->keyn;
	temprec = psp->rec;
	tempdptr = psp->data;
	tempdlen = psp->dlen;
	tempsrchorder = psp->srchorder;
	psp->srchorder = 0;					/* force search order to normal */

	if ((psp->key == psp->retkey) || (psp->key == (psp->retkey+1)))
		cikeycpy(psp, psp->keyn, tempkey, psp->key);	/* save key value in case in psp */

	psp->keyn = semaind;
	psp->key = key;
	psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
	psp->rec = rec;
	psp->data = (void *)&sharefnd;
	psp->dlen = (int)sizeof(sharefnd);
	ret = _cfind(psp);
	if ((ret == CCOK) && (sharefnd != sharenum))
	{	
		/* sema exists */
		/* check if process has exited */
	   if (testshrbyte(psp, (int)sharefnd) == FAIL)	/* not really locked */
	   {
			if (psp->readmode == CRDONLY)
			{
				psp->key = tempkptr;						/* restore psp values */
				psp->keyorder = tempkmode;
				psp->keyn = tempindx;
				psp->rec = temprec;
				psp->data = tempdptr;
				psp->dlen = tempdlen;
				psp->srchorder = tempsrchorder;

				/* cannot clear in read-only mode, just return unlocked */
				cc = endwrit(psp);
				if (cc != CCOK)
					ret = cc;
				else
					ret = RECUNLK;							/* record unlocked */

				return(ret);
			}

			psp->keyn = semaind;
			psp->key = key;
			psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
			psp->rec = rec;
			ret = _cdelete(psp);
#ifdef TRANSACTION
			if (ret == CCOK)
				ret = transtst(psp);		/* test for incomplete transactions */
			if (ret == TRANSALLOC)
				ret = CCOK;
#endif	/* TRANSACTION */
			if (ret == CCOK)
	      	ret = RECUNLK;								/* record not locked */
	   }
		else
		{
			ret = RECSHLK;					/* record locked by someone else */
		}
	}
	else
	if ((ret == CCOK) && (sharefnd == sharenum))
		ret = RECSFLK;							/* record locked by self */
	else
	if (ret == FAIL || ret == GREATER || ret == KEYMATCH)
		ret = RECUNLK;							/* record unlocked */

	psp->key = tempkptr;						/* restore psp values */
	psp->keyorder = tempkmode;
	psp->keyn = tempindx;
	psp->rec = temprec;
	psp->data = tempdptr;
	psp->dlen = tempdlen;
	psp->srchorder = tempsrchorder;

	if ((psp->key == psp->retkey) || (psp->key == (psp->retkey+1)))
		cikeycpy(psp, psp->keyn, psp->key, tempkey);	/* restore key value in case in psp */

	psp->srecurs = FALSE;					/* not in semaphore operation */

	cc = endwrit(psp);
	if (cc != CCOK)
		ret = cc;

	return(ret);
}

int FDECL mzapsema(psp)				/* remove (zap) all semaphores for this share # */
CFILE *psp;
{
	int ret;
	short sharefnd;			/* share number found in semaphore */
	int tmpindx;				/* save index # in psp */

	/**/

	if (psp->processmode == EXCL)				/* if in exclusive use mode */
		return(CCOK);								/* don't need to do semaphore op */

	tmpindx = psp->keyn;						/* save keyn index number */

	psp->srecurs = TRUE;						/* say we are in semaphore operation */

	/* find first key in semaphore index */
	ret = cfirst(psp, SEMAINDEX, &sharefnd, sizeof(sharefnd));

	while (ret == CCOK)
	{
		if ((int)sharefnd == psp->sharenum)		/* if have left over semaphore */
			if (psp->readmode == CRDONLY)
				ret = BADSHARENUM;					/* can't use this sharenum */
			else
				ret = cdelcur(psp, SEMAINDEX);	/* delete it from file */
		else
			ret = CCOK;

		if (ret != CCOK)
		{
			psp->srecurs = FALSE;					/* not in semaphore operation */
			psp->keyn = tmpindx;						/* restore index # */
 			return(ret);
		}
		else
		{
			ret = cnext(psp, SEMAINDEX, &sharefnd, sizeof(sharefnd));
		}
	}

	if (ret == FAIL)				/* ignore last search failure */
		ret = CCOK;

	psp->srecurs = FALSE;					/* not in semaphore operation */
	psp->keyn = tmpindx;						/* restore index # */

	return(ret);
}



void FDECL mbldlock(psp, newkey, keyn)
CFILE *psp;
char *newkey;
int keyn;
{

	/**/

	*(newkey+1) = (char)keyn;									/* set for index # */
	getcurkey(psp, keyn, newkey+2);							/* actual key value */
	*(newkey) = (char)cikeylen(psp, keyn, newkey+2);	/* set length */

#ifndef NO_WORD_CONVERSION
	/* force key to be in native format */
	cikeyflip(psp, keyn, newkey+2, psp->indtyp[keyn] & INDTYPEMASK, psp->wordorder, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
}



int EDECL msetlock(psp, keyn)	/* lock current entry */
CFILE *psp;		/* open psp */
int keyn;		/* index number */
{
	char newkey[MAXMKLEN];
	int ret;
	int cc;
	void *tempdata;					/* temp data ptr storage */
	int tempdlen;						/* temp dlen storage */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(keyn)) 
	   return(psp->retcde = PARMERR); 

	if (psp->processmode == EXCL)				/* if in exclusive use mode */
	{
		psp->curlock[keyn] = LOCKED;
		return(CCOK);								/* don't need to do semaphore op */
	}

	if (psp->srecurs == TRUE)				/* stop recursion */
		return(CCOK);

	if (psp->curlock[keyn] == LOCKED)
		return(CCOK);								/* record is locked */

#ifdef REMOVED
	/* if not right index or just deleted, no cur */	
	if (psp->curindx[keyn] != keyn || psp->delflag[keyn] == DELON)
	   return(NOCUR);
#endif
	/* if not right index or just deleted, no cur */	
	if (getcurindx(psp, keyn) != keyn || getdelflag(psp, keyn) == DELON)
	   return(NOCUR);

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	tempdata = psp->data;			/* save data and dlen before cfind */
	tempdlen = psp->dlen;

	if (cgetcur(psp, keyn, "", 0) != CCOK)
	{
		ret = NOCUR;
	}
	else
	{
		mbldlock(psp, newkey, keyn);					/* build the lock key */
		ret = msetsema(psp,
							newkey,
							getcurrcval(psp, keyn),
							psp->sharenum);

		if (ret == CCOK)
			psp->curlock[keyn] = LOCKED;
#ifdef REMOVED
		if (ret == CCOK)
			putcurlock(psp, keyn, LOCKED);
#endif
	}

	psp->data = tempdata;			/* restore data and dlen */
	psp->dlen = tempdlen;

	cc = endwrit(psp);
	if (cc != CCOK)
		ret = cc;

	return(ret);
}


int FDECL _mclrlck(psp, keyn)	/* internal clear of lock for reposition */
CFILE *psp;		/* open psp */
int keyn;		/* index number */
{
	int ret;

	/**/

	if (psp->curlock[keyn] == UNLOCKED)
	{
		ret = CCOK;
	}
	else
	{
		ret = mclrlock(psp, keyn);					/* is self-locked, do unlock */
		if (ret != CCOK)
			ret = CLRLOCKERR;				/* failed to unlock for some reason */
	}

	return(ret);
}


int EDECL mclrlock(psp, keyn)	/* clear lock on current entry */
CFILE *psp;		/* open psp */
int keyn;		/* index number */
{
	char newkey[MAXMKLEN];
	int ret;
	int tstret;				/* return from test lock */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(keyn)) 
	   return(psp->retcde = PARMERR); 

	if (psp->processmode == EXCL)				/* if in exclusive use mode */
	{
		psp->curlock[keyn] = UNLOCKED;
		return(CCOK);								/* don't need to do semaphore op */
	}

	if (psp->srecurs == TRUE)				/* stop recursion */
		return(CCOK);

	if (psp->curlock[keyn] == UNLOCKED)
	{
		if ((tstret = mtstlock(psp, keyn)) != RECUNLK)
			return(tstret);					/* record must be locked by other */
		else
			return(CCOK);							/* record not locked */
	}

	/* if not right index or just deleted, no cur */	
	if (getcurindx(psp, keyn) != keyn || getdelflag(psp, keyn) == DELON)
	   return(NOCUR);

	mbldlock(psp, newkey, keyn);					/* build the lock key */
	ret = mclrsema(psp, newkey, getcurrcval(psp, keyn), psp->sharenum);

	if (ret == CCOK)
		psp->curlock[keyn] = UNLOCKED;

	return(ret);
}


int EDECL mtstlock(psp, keyn)	/* test lock of current entry */
CFILE *psp;		/* open psp */
int keyn;		/* index number */
{
	char newkey[MAXMKLEN];
	int ret;

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(keyn)) 
	   return(psp->retcde = PARMERR); 

#ifdef REMOVED
	/* if not right index or just deleted, no cur */	
	if (psp->curindx[keyn] != keyn || psp->delflag[keyn] == DELON)
	   return(NOCUR);
#endif
	/* if not right index or just deleted, no cur */	
	if (getcurindx(psp, keyn) != keyn || getdelflag(psp, keyn) == DELON)
	   return(NOCUR);

	if (psp->processmode == EXCL)				/* if in exclusive use mode */
	{
#ifdef REMOVED
		if (getcurlock(psp, keyn) == UNLOCKED)
#endif
		if (psp->curlock[keyn] == UNLOCKED)
			ret = RECUNLK;						/* assume record unlocked */
		else
			ret = RECSFLK;						/* or assume self lock */

		return(ret);
	}

#ifdef REMOVED
	if (getcurlock(psp, keyn) == LOCKED)
#endif
	if (psp->curlock[keyn] == LOCKED)
		return(RECSFLK);									/* record is locked */

#ifdef REMOVED
	/* if not right index or just deleted, no cur */	
	if (psp->curindx[keyn] != keyn || psp->delflag[keyn] == DELON)
	   return(NOCUR);
#endif
	/* if not right index or just deleted, no cur */	
	if (getcurindx(psp, keyn) != keyn || getdelflag(psp, keyn) == DELON)
	   return(NOCUR);

	mbldlock(psp, newkey, keyn);					/* build the lock key */
	ret = mtstsema(psp, newkey, getcurrcval(psp, keyn), psp->sharenum);

	return(ret);
}


int FDECL muflush(psp)		/* flush buffers after shared access */
struct passparm *psp;
{
	int ret = CCOK;
	int cc;

	/**/

	psp->errcde = 0;

	if (psp->updtflag)		/* if need to write out header */
	{
	   ret = hdrupdte(psp);		/* then do it */
	}

	cc = bflush(psp);		/* flush buffers */
	if ((cc != CCOK) && (ret == CCOK))
		ret = cc;

	return(psp->retcde = ret);
}



/*
 * lowlock and lowunlock implement a write lock queue to sequence
 * the access to the file in an almost perfect queue.
 *
 */

#ifndef NEWLOWLOCK
int FDECL lowlock(psp)	/* lock file for shared file access */
CFILE PDECL *psp;
{
	int ret;
	int ec;

	/**/

	if (psp->processmode != EXCL && psp->filestat == UNLOCKED)
	/* need to lock it and not locked by high level functions */
	{
		if (psp->writeque)
		{
			/* look for first available position in queue */
			ec = -1;
			psp->curquepos = psp->endque;		/* start where end of que was las time */
			ret = CCOK;								/* assume success */
			while (ec)
			{
				ec = 	tlockbyte(psp, FLOCKBYTE + psp->curquepos);
				if (ec)
				{
					psp->curquepos++;		/* keep looking for empty pos in que */
					psp->endque++;
					if (psp->endque > MAXUSERS)
					{
						ret = USRERR;			/* seem to be too many users in file */
						break;
					}
				}
			}

			/* now attempt to work up to first position of queue */
			while (psp->curquepos)
			{
				ec = 	lockbyte(psp, FLOCKBYTE + psp->curquepos - 1);	/* lock lower que position (wait) */
				if (ec)
				{
					ret = BADQUE;			/* problem in maintaining que */
					break;
				}
				else
				{
					ec = 	unlockbyte(psp, FLOCKBYTE + psp->curquepos);	/* unlock higher que position (moved up) */
					if (ec)
					{
						ret = BADQUE;
						break;
					}

					psp->curquepos--;		/* keep moving up que */
				}
			}

			if (ret == CCOK)
			{
 			   psp->filestat = LOCKED;
			}
		}
		else		/* else use single byte write lock */
		{
			ret = lockbyte(psp, FLOCKBYTE);

			if (ret == CCOK)
	 		   psp->filestat = LOCKED;
		}
	}
	else
	{
		ret = CCOK;
	}

	return(ret);
}


int FDECL lowunlock(psp)	/* unlock file for low level functions */
CFILE PDECL *psp;
{
	int ret;

	/**/

	/* if need to unlock it */
	if (psp->processmode != EXCL && psp->filestat != UNLOCKED)
	{
		ret = unlockbyte(psp, FLOCKBYTE);

		if (ret == CCOK)
		   psp->filestat = UNLOCKED;

		return(ret);
	}

	return(CCOK);
}

/* wait for low lock by another process to end (permit start of reading) */
int FDECL waitlowlock(psp)
struct passparm PDECL *psp;
{
	int ret;

	ret = lowlock(psp);						/* do low level lock of file */
	if (ret == CCOK)
		ret = lowunlock(psp);				/* do low level unlock of file */

	return(ret);
}

#endif	/* !NEWLOWLOCK */


/* -------------------------------------------------- */
/* before each open, read or write, locks file        */
/* after all reads or writes done, unlocks file       */
/* -------------------------------------------------- */

int FDECL strtopen(psp)				/* start preparations for opening file */
CFILE *psp;
{
	int ret;

	/**/

	psp->readcnt++;					/* incr count of write start actions */
   psp->writeactive = FALSE;		/* no write has yet occured */
	psp->writeflag = FALSE;			/* clear flag indicating writing to disk */
	psp->memerr = CCOK;						/* clear memory allocation errors */
	psp->alterr = CCOK;						/* clear alternate error */
	psp->keyorder = INTELFMT;	/* reset key byte ordering to native */

   /* if exclusive, no need for lock */
	if (psp->processmode == EXCL)
   {
   	ret = CCOK;
   }
   else
   {
#ifdef READSHARE
		if (psp->filestat == UNLOCKED && psp->readstat == NOREADLOCK)
		{
			ret = waitlowlock(psp);
			if (ret == CCOK)
			{
				ret = cireadlock(psp, FALSE);		/* lock single read byte */
			}

			if (ret != CCOK)
				ret = FILELOCKED;
		}
		else
		{
			ret = CCOK;
		}
#else
		ret = lowlock(psp);						/* lock file for writes */
#endif	/* READSHARE */
   }

	return(ret);
}


int FDECL endopen(psp)				/* ending actions for opening file */
CFILE *psp;
{
	int ret;

	/**/

	psp->readcnt--;						/* decr count of write start actions */

	if (psp->processmode == EXCL)
   {
		ret = osbflush(psp);				/* flush the os buffers */
   }
   else
   {
		ret = muflush(psp);					/* clear out buffers */
													/* when all open actions are done */
#ifdef READSHARE
		if (psp->processmode != EXCL)
		{
			ret = cireadunlock(psp);		/* lock single read byte */
		}
#else
		ret = lowunlock(psp);				/* unlock file */
#endif	/* READSHARE */

		if (psp->memerr)
			ret = psp->memerr;				/* memory allocation problem */
	}

	return(ret);
}



int EDECL strtread(psp)				/* start preparations for reading file */
CFILE *psp;
{
	int ret;

	/**/

	ret = initbcb(psp);							/* init buffer bcbs if needed */
	if (ret != CCOK)
		return(ret);								/* if error in init buffers */

	if (!psp->readcnt)
	{
		psp->memerr = CCOK;				/* clear memory allocation errors */
		psp->alterr = CCOK;				/* clear alternate error */

		if (!psp->writcnt)
			psp->keyorder = INTELFMT;	/* reset key byte ordering to native */
	}

	/* if exclusive, no need for preps */
	if (psp->processmode == EXCL)
	{
		ret = CCOK;
	}
	else
	{
#ifdef READSHARE
		if (psp->processmode != EXCL && psp->filestat == UNLOCKED && psp->readstat == NOREADLOCK)
		{
			ret = waitlowlock(psp);
			if (ret == CCOK)
			{
				ret = cireadlock(psp, FALSE);		/* lock single read byte */
				if (ret == CCOK)
				{
					ret = strtbufs(psp);			/* read header and clear buffers */
				}
			}

			if (ret != CCOK)
				ret = FILELOCKED;
		}
		else
		{
			ret = CCOK;
		}
#else
		if (psp->processmode != EXCL && psp->filestat == UNLOCKED)
		{
			ret = lowlock(psp);						/* do low level lock of file */
			if (ret == CCOK)
			{
				ret = strtbufs(psp);			/* read header and clear buffers */
			}
			else
			{
				ret = FILELOCKED;
			}
		}
		else
		{
			ret = CCOK;
		}
#endif	/* READSHARE */
	}

	if (ret == CCOK)
		psp->readcnt++;						/* incr count of read start actions */

	return(ret);
}


int EDECL endread(psp)				/* ending actions for reading file */
CFILE *psp;
{
	int ret;

	/**/

	ret = CCOK;

	psp->readcnt--;						/* decr count of read start actions */
	if ((psp->writcnt == 0) && (psp->readcnt == 0))
	{
		if (psp->processmode != EXCL)			/* if exclusive, no need for preps */
		{
#ifdef READSHARE
			ret = cireadunlock(psp);		/* lock single read byte */
#else
			ret = lowunlock(psp);			/* do low level unlock of file */
#endif	/* READSHARE */
		}

		if (psp->memerr)
			ret = psp->memerr;			/* memory allocation problem */

#ifndef NO_WORD_CONVERSION
		cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	}

	return(ret);
}



int EDECL strtwrit(psp)				/* start preparations for writing file */
CFILE *psp;
{
	int ret;
	INT16 flag;
#ifdef TRANSACTION
	INT16 emptyent;		/* null entry */
#endif

#ifdef IMAGEBACK
	struct passparm *backpsp;
	struct hdrrec hdr;
#endif

	/**/

	ret = initbcb(psp);							/* init buffer bcbs if needed */
	if (ret != CCOK)
		return(ret);								/* if error in init buffers */

	if (!psp->writcnt)
	{
		psp->memerr = CCOK;				/* clear memory allocation errors */
		psp->alterr = CCOK;				/* clear alternate error */

		if (!psp->readcnt)
			psp->keyorder = INTELFMT;	/* reset key byte ordering to native */
	}

	if (psp->processmode != EXCL && psp->filestat == UNLOCKED)
	{
#ifdef READSHARE
		if (psp->readstat != NOREADLOCK)		/* remove read byte locks first */
			ret = cireadunlock(psp);
		else
			ret = CCOK;
#else
		ret = CCOK;
#endif	/* READSHARE */

		if (ret == CCOK)
			ret = lowlock(psp);						/* do low level lock of file */
		if (ret == CCOK)
		{
			ret = strtbufs(psp);			/* read header and clear buffers */
		}
		else
		{
			return(errcde = FILELOCKED);
		}
	}
	else
	{
		ret = CCOK;
	}

	if (ret == CCOK)
	{
		/* if first time strtwrit and have security level of 4, set write flag */
		if (!psp->writcnt && (psp->seclev >= 4))
		{
		   psp->writeactive = FALSE;	/* no write has yet occured */
			psp->writeflag = FALSE;		/* clear flag indicating writing to disk */

			ret = readwrt(psp);
			if ((ret == CCOK) && !psp->lazywrite)
			{
#ifdef READSHARE
				if (psp->processmode != EXCL)
					ret = cireadlock(psp, TRUE);	/* lock all read bytes */
#endif		/* READSHARE */
				if (ret == CCOK)
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
					}
				}
			}
		}
		else
		{
#ifdef READSHARE
			if (!psp->writcnt && (psp->processmode != EXCL))
				ret = cireadlock(psp, TRUE);	/* lock all read bytes */
#endif		/* READSHARE */
		}
	}

	/* increment update number to indicate a change will be made */
	if (!psp->writcnt)
	{
		psp->pupdatnum++;
	   psp->updtflag = DIRTY;			/* header now dirty */
	}

#ifdef IMAGEBACK
	/* if backup imaging active, write current header to backup file */
	backpsp = psp->imagepsp;
	if ((ret == CCOK) && backpsp && !psp->writcnt)
	{
		psp->backnde = 0;					/* start at beginning of backup area */
		psp->backlast = 0;
		hdr.updatnum = psp->pupdatnum;		/* keep current */
		sethdr(psp, &hdr);						/* move current info into header */
		setfile(backpsp, &hdr);					/* update backup psp header info */

		ret = hdrwrte(backpsp, &hdr);			/* write new header to backup file */
		if (ret == CCOK)
			backpsp->updtflag = CLEAN;		/* now clean */
	}
#endif	/* IMAGEBACK */

	if (ret == CCOK)
		psp->writcnt++;			/* incr count of write start actions */
	else
		lowunlock(psp);			/* if failed start, unlock file */

#ifdef TRANSACTION
	/* 1st time through startwrit, log the occassion */
	if (psp->transop && (ret == CCOK) && (psp->writcnt == 1))
	{
		psp->strttransnde = psp->curtransnde;	/* ptr to strtwrit log entry */
		psp->strttransndeptr = psp->transndeptr;
		emptyent = 0;
		ret = translog(psp, TRANSSTRTWRIT, (char *)&emptyent);
	}
#endif	/* TRANSACTION */

	return(errcde = ret);
}

/* clear the write interupt flag from the file header */
int EDECL clrwrit(psp)
CFILE PDECL *psp;
{
	int ret;
	INT16 flag;

	errcde = fileSeek(psp, (long)WRTBYTE, 0, 0L);
	if (errcde != CCOK)	/* seek error */
	{
	   ret = DSKERR;
	}
	else
	{
		/* set flag in case of bad write */
		flag = RESETUPD;
		ret = ciwrite(psp, (char *)&flag, sizeof(flag));
		if (!ret)
		{
			ret = osbflush(psp);
			psp->writeflag = FALSE;			/* clear flag indicating writing to disk */
		}
	}

	return(errcde = ret);
}

int EDECL endwrit(psp)				/* ending actions for writing file */
CFILE *psp;
{
	int ret;
	int tempret;				/* save psp return code */
	int cc;
#ifdef TRANSACTION
	INT16 emptyent;			/* null transaction entry */
#endif

	/**/

	ret = CCOK;
	tempret = psp->retcde;

	if (psp->writcnt == 1)
	{
		if ( (psp->seclev >= 4)
			|| (psp->processmode != EXCL))
		{
      	ret = muflush(psp);		/* clear out buffers */
											/* when all write actions are done */
		}
		else
      if (psp->processmode == EXCL)
      {
			if (psp->seclev > 0)
      		ret = osbflush(psp);			/* flush the os buffers */
			else
				ret = CCOK;
		}

#ifdef TRANSACTION
		/* force current transaction log node to be written first */
		if ((psp->curtransnde > 0) && (ret == CCOK))
		{
			emptyent = 0;
			ret = translog(psp, TRANSENDWRIT, (char *)&emptyent);
		}
#endif	/* TRANSACTION */

#ifdef TRANSACTION
		if ((psp->transop == TRANSALL) || (psp->transop == TRANSMIRROR))
		{
			ret = transmirror(psp);
		}
#endif	/* TRANSACTION */

		/* if have extended and level > 0 and no disk errors */
		if (psp->extflag && psp->seclev && ret == CCOK)	
		{
			ret = upddisk(psp);		/* update disk FAT table (or equiv) */
			psp->extflag = FALSE;	/* clear flag */
		}

#ifdef READSHARE
		if ((ret == CCOK) && (psp->seclev >= 4) && (psp->writeflag))
		{
			ret = clrwrit(psp);
			psp->writeactive = FALSE;

		}

		if (psp->processmode != EXCL)
		{
			/* remove read byte locks first */
			if (psp->readstat != NOREADLOCK)
			{
				cc = cireadunlock(psp);
				if (ret == CCOK)
					ret = cc;
			}

			if (psp->readcnt != 0)
			{
				cc = cireadlock(psp, FALSE);			/* re-lock single read byte */
				if (ret == CCOK)
					ret = cc;
			}

			cc = lowunlock(psp);			/* do low level unlock of file */
			if (ret == CCOK)
				ret = cc;
		}
#else
		if ((ret == CCOK) && (psp->writeflag))
		{
			ret = clrwrit(psp);
			psp->writeactive = FALSE;
		}

		if ((psp->processmode != EXCL) && (psp->readcnt == 0))
		{
			cc = lowunlock(psp);			/* do low level unlock of file */
			if (ret == CCOK)
				ret = cc;
		}
#endif		/* READSHARE */

#ifndef NO_WORD_CONVERSION
		if (!psp->readcnt)
		{
			cipspflip(psp, INTELFMT);
		}
#endif	/* !NO_WORD_CONVERSION */

		if (psp->memerr)
			ret = psp->memerr;			/* memory allocation problem */
	}

	psp->writcnt--;						/* decr count of write start actions */
	psp->retcde = tempret;				/* restore return code */

	return(ret);
}


int FDECL sethdrinfo(psp)		/* sets all header information in psp */
CFILE *psp;
{
	struct hdrrec hdr;
	int ret;

	if (psp->processmode == EXCL)	/* no need for this */
	   return(CCOK);

	if ((ret = hdrread(psp, &hdr)) != CCOK)
	   return(ret);

	setfile(psp, &hdr);	/* otherwise set info */
	return(CCOK);
}
		

int FDECL mhdrupdte(psp)	/* multi user header update - only needed if MU  */
CFILE *psp;
{
	if (psp->processmode == EXCL)	/* no need for this */
	   return(CCOK);

	return(CCOK);					/* no header update when file always locked */
}


#endif


#ifdef MREAD

int FDECL fgoodbufs(psp)	/* check if buffers are still valid */
CFILE *psp;
{
	int i;
	INT32 updnum;

#ifdef READSHARE
	/* no need for this test if: */
	if (	(psp->processmode == EXCL)		/* EXCL or DENYWRITE/CIRDONLY */
		|| (psp->filestat == LOCKED)		/* already tested when file locked */
		|| (psp->readstat != NOREADLOCK)	/* tested when locked for read */
		|| (psp->skipupdread))				/* DENYWRITE/CIRDWRITE */
		return(CCOK);
#else
	/* no need for this test if: */
	if (	(psp->processmode == EXCL)		/* EXCL or DENYWRITE/CIRDONLY */
		||(psp->filestat == LOCKED)		/* already tested when file locked */
		|| (psp->skipupdread))				/* DENYWRITE/CIRDWRITE */
		return(CCOK);
#endif	/* READSHARE */

	updnum = updread(psp);
	if (!updnum)
		return(DSKERR);

	if (psp->pupdatnum != updnum)			/* not current buffers */
	{
	   psp->pupdatnum = updnum;
#ifdef REMOVED
		for (i = 1; i < NUMINDXS; i++)	/* for every index */
			psp->updkeyinfo[i] = TRUE;		/* EXCL key info not current */
#endif
		for (i = 1; i < NUMINDXS; i++)	/* for every index */
			putupdkeyinfo(psp, i, TRUE);	/* EXCL key info not current */

	   return(FAIL);
	}

   return(CCOK);
}

		
int FDECL strtbufs(psp)	/* read and set header and clear buffers if not valid */
CFILE *psp;		/* similar to goodbufs and sethdrinfo combined */
{
	int ret;
	int i;
	INT32 updnum;

	/**/

	/* no need for this if EXCL or DENYWRITE/CIRDWRITE*/
	if ((psp->processmode == EXCL) || (psp->skipupdread))
	   return(CCOK);

	updnum = updread(psp);
	if (!updnum)
		ret = DSKERR;							/* error reading update number */
	else
	if (psp->pupdatnum != updnum)			/* not current buffers */
	{
   	clrbcb(psp);							/* clear buffers */
   	ret = sethdrinfo(psp);	 
	   psp->pupdatnum = updnum;
		for (i = 1; i < NUMINDXS; i++)	/* for every index */
			if (tstcuradr(psp, i))				/* if active index */
				putupdkeyinfo(psp, i, TRUE);	/* EXCL key info not current */
	}
	else
#ifdef REMOVED								/**********************************/
		ret = sethdrinfo(psp);			/******* bug fix 7/26/93 **********/
#endif										/**********************************/
		ret = CCOK;							/**********************************/

	return(ret);
}

	
#endif



/* ------------------------------------ */
/* multi-user open and create and close */
/* ------------------------------------ */

#ifdef MOPEN

#ifdef WRTUPD

/* write byte to header to indicate file is being updated to disk */

int FDECL wrtupd(psp, flag)
struct passparm PDECL *psp;
int flag;					/* flag to set into header area */
{
	INT16 flag16 = flag;

	/**/

	if (psp->processmode == EXCL)
		return(CCOK);

	errcde = fileSeek(psp, (long)UPDBYTE, 0, 0L);
	if (errcde != CCOK)	/* seek error */
	   return(errcde = DSKERR);

	/* if bad write */
	errcde = ciwrite(psp, (char *)&flag16, sizeof(flag16));

  	return(errcde);
}



/* read the header area to see if in the upddisk routine in a shared file
 * (only if in shared mode)
 */

int FDECL readupd(psp)
CFILE *psp;
{
	int ec;
	int flag;					/* upddisk flag */
	int ret;
	int i;

	/**/

	/* if exclusive mode */
	if (psp->processmode == EXCL)
		return(CCOK);

	errcde = CCOK;
	flag = SETUPD;				/* fake out compiler checking */

	for (i = 0; i < UPDRETRY; i++)
	{
		if (fileSeek(psp, (long)UPDBYTE, 0, 0L) != CCOK)	/* seek error */
		   return(errcde = DSKERR);

		/* read upddisk flag to see if in middle of update */
		ec = ciread(psp, (char *)&flag, sizeof(int));
		if (ec != CCOK)
		{
		  	return(ec);
		}

		if (flag != SETUPD)				/* found the file is available */
			return(CCOK);
	}

	ret = wrtupd(psp, (INT16)RESETUPD);			/* force clear of update flag */

	return(ret);
}

#endif	/* WRTUPD */


/* read the header area to see if in the strtwrit was interupted */
int FDECL readwrt(psp)
CFILE *psp;
{
	INT16 flag;
	INT16 flipflag;
	int ret;

	/**/

	errcde = CCOK;
	flag = SETUPD;				/* fake out compiler checking */

	if (fileSeek(psp, (long)WRTBYTE, 0, 0L) != CCOK)	/* seek error */
	   return(errcde = DSKERR);

	/* check strtwrit flag to see if writing to file was interupted */
	ret = ciread(psp, (char *)&flag, sizeof(flag));
	flipflag = inteltos(flag, psp->wordorder);
	if ((ret == CCOK) && (flipflag == SETUPD))	/* found the file is in bad state */
		ret = WRITEINTR;

	return(ret);
}



/* opens file and readies for access */
int EDECL __bopen(psp, fle, filemode, rdmode, opentype)
struct passparm *psp;							/* file psp */
char *fle;											/* file name */
int filemode;										/* exclusive or shared */
int rdmode;											/* read mode */
int opentype;										/* LOWOPEN, NORMOPEN */
{	
	struct hdrrec hdr;
	int ret;
	int cc;
	int i;

	/**/

	setpsp(psp, PSPCLOSED);		/* set file to closed in case of problem */

	psp->peekmsg  = FALSE;			/* default - no peek message (MSwindows) */
	psp->writeque = FALSE;			/* default - no write queue */
	psp->lazywrite= TRUE;			/* default - always write cache in sec lev 4 */
	psp->writcnt = 0;					/* clear write lock count */
	psp->readcnt = 0;					/* clear read lock count */
	psp->memerr = CCOK;				/* assume no memory errors */
	psp->alterr = CCOK;				/* clear alternate error */
	psp->fliprep = TRUE;				/* flip keys passed to cnextrep callback */
	psp->speedread = FALSE;			/* disable speed read by default */
	psp->speedahead = 0;
	psp->speedreq = 0;
	psp->speednod = (struct smallnode *)0;
	psp->speedentptr = (ENTRY *)0;
	psp->srchorder = 0;				/* default - normal key comparison */
	psp->key = 0;

#ifndef NO_BLOCK_EXTEND
	psp->minextend = MINDATAEXTEND;	/* minimum number of nodes to extend when enlarging file */
#endif	/* !NO_BLOCK_EXTEND */

	strncpy(psp->filename, fle, MAXNAMELEN);   /* save file name */

	/* if the os cannot do locking functions, force exclusive mode */
	if (mtstos() != CCOK)
		filemode = EXCL;

	psp->errcde = 0;
	psp->filemode = (char)filemode;	/* shared or exclusive file opens */
	psp->processmode = (char)filemode;	/* shared or exclusive processing */

	ret = initbcb(psp);							/* init buffer bcbs if needed */
	if (ret != CCOK)
		return(ret);								/* if error in init buffers */

	if (filemode == EXCL)
	{
		psp->fd = mexlopen(psp, fle, rdmode);		/* exclusive open */
	}
	else
	{
		psp->fd = mshropen(psp, fle, rdmode);		/* shared open */
		if (psp->fd == NULLFD) 						/* if bad open */
		{
			filemode = EXCL;
			psp->filemode = (char)filemode;	/* shared or exclusive file opens */
			psp->processmode = (char)filemode;	/* shared or exclusive processing */
			psp->fd = mexlopen(psp, fle, rdmode);	/* try again in exclusive mode */
		}
	}

	if (psp->fd != NULLFD) 						/* if good open */
	{
		setpsp(psp, PSPOPEN);				/* set file to open */
		psp->srecurs = FALSE;				/* turn of recursion flag */
		psp->filestat = UNLOCKED;			/* file is unlocked */
		psp->keyorder = INTELFMT;			/* reset key byte ordering to native */

#ifdef READSHARE
	psp->readstat = NOREADLOCK;			/* no read locks yet */
#endif
		psp->endque = 1;						/* assume another user */
		psp->extflag = FALSE;				/* clear extend flag */
		psp->readmode = (char)rdmode;		/* read-only or read/write */
		psp->pupdatnum = 0;					/* clear update count */
		psp->currec = -1;						/* clear current record number */
		psp->keyn = 0;							/* no key processing at the moment */

		/* skip reading update number if deny write and read/write */
		if ((psp->processmode == DENYWRITE) && (psp->readmode == CRDWRITE))
			psp->skipupdread = TRUE;		/* can't be updated by another process */
		else
			psp->skipupdread = FALSE;		/* do complete checking of file update status */


#ifdef TRANSACTION
		psp->transop     = 0;				/* clear transaction status vars */
		psp->begtransnde = 0;
		psp->curtransnde = 0;
		psp->transndeptr = NULL;
		psp->strttransnde = 0;
		psp->strttransndeptr = NULL;
		psp->transnum    = 0;
		psp->transseq    = 0;
		psp->transpsp    = NULL;			/* psp of mirror file */
#endif	/* TRANSACTION */

#ifdef IMAGEBACK
		psp->backnde = 0;
		psp->backlast = 0;
		psp->imagepsp = NULL;				/* no backup image file */

		/* clear image backup callback proceedures */
		psp->imagebackproc = (IMGPROCTYPE)NULL;
		psp->imagerestproc = (IMGPROCTYPE)NULL;
#endif

		for (i = 1; i < NUMINDXS; i++)	/* for every index */
		{
			psp->segptr[i] = NULL;			/* clear segment info ptr */
			psp->curlock[i]  = UNLOCKED;	/* set unlocked record status */
		}

		ret = initcurs(psp);							/* initialize current pointers */
		if (ret != CCOK)
		{
			endopen(psp);						/* unlock file */
			if (psp->filemode == EXCL) 	/* close the file */
				mexlclose(psp);
			else
				mshrclose(psp);
			setpsp(psp, PSPCLOSED);			/* set file to closed */
			return(psp->retcde = ret);
		}

		if (opentype == LOWOPEN)
		{
			psp->readmode = (char)CRDWRITE;	/* force read/write to prevent mzapsema check */
			ret = setshrbyte(psp);				/* assign share byte */
			psp->readmode = (char)rdmode;		/* read-only or read/write */
		}
		else
		{
			ret = setshrbyte(psp);				/* assign share byte */
		}

		if (ret == CCOK)
			ret = strtopen(psp);

		if (ret == BADQUE)
		{
			clrshrbyte(psp, psp->sharenum);		/* release share byte */
			mshrclose(psp);					/* close the file */
			setpsp(psp, PSPCLOSED);		/* set file to closed */
			return(ret);
		}

		if (ret != CCOK)
		{
			if (filemode != EXCL)		/* SHARE module appears to not be loaded */
			{
				mshrclose(psp);					/* close the file */
				setpsp(psp, PSPCLOSED);		/* set file to closed */
				filemode = EXCL;						/* set exclusive mode */
				psp->processmode = (char)filemode;
				psp->filemode = (char)filemode;	/* shared or exclusive file opens */
				psp->fd = mexlopen(psp, fle, rdmode);	/* try again in exclusive mode */

				if (psp->fd == NULLFD) 					/* if bad open */
					return(FILENOTLOCKED);

				setpsp(psp, PSPOPEN);				/* set file to open */
			}
			else
				return(FILENOTLOCKED);
		}

#ifdef WRTUPD									/* requires wrtupd and readupd */
		ret = readupd(psp);					/* check to see if in upddisk */
		if (ret != CCOK)
		{
			endopen(psp);						/* unlock file */
			if (psp->filemode == EXCL) 	/* close the file */
				mexlclose(psp);
			else
				mshrclose(psp);

			setpsp(psp, PSPCLOSED);		/* set file to closed */
			return(ret);
		}
#endif	/* WRTUPD */

		if (filemode == EXCL)
		{
			ret = hdrread(psp, &hdr);

	   	if (ret != CCOK)	/* bad read */
		   {
		      psp->errcde = errcde;		/* set error code */
				endopen(psp);					/* unlock file */
				mexlclose(psp);				/* close errant file */
				setpsp(psp, PSPCLOSED);		/* set file to closed */
	   	   return(psp->retcde = ret);
		   }

			setfile(psp, &hdr);			/* move header info in */

		}
		else
		{
			if (ret == CCOK)
		   	ret = sethdrinfo(psp);	 

			if (ret != CCOK)
			{
		      psp->errcde = errcde;		/* set error code */
				endopen(psp);					/* unlock file */
				mshrclose(psp);				/* close errant file */
				setpsp(psp, PSPCLOSED);		/* set file to closed */
				return(psp->retcde = ret);
			}

		}

		if (filemode == EXCL)
			psp->seclev = DEFLEV;	/* start out at single user security level */
		else
			psp->seclev = DEFLEVMU;	/* start out at multi-user security level */

		/* set default flags */
		psp->checkflg = TRUE;			/* default - always check datalist */
#ifdef REMOVED			/* temporarily removed to default to old delete code */
		psp->fulldel  = TRUE;			/* default - always delete up tree */
		psp->reuse    = FALSE;			/* default - don't reuse record numbers */
#endif	/* REMOVED */
		psp->fulldel  = FALSE;			/* default - never delete up tree */
		psp->reuse    = TRUE;			/* default - reuse record numbers */
		psp->setdrec  = -1;				/* use internal record # for dadd() */

		psp->nolock   = FALSE;			/* default - require record lock */
		psp->srchorder= 0;				/* default - normal key comparison */
		psp->key = 0;
	}


	if (psp->fd == NULLFD)
	{
		ret = FAIL;						/* failed file open */
		setpsp(psp, PSPCLOSED);		/* set file to closed */
	}
	else
	{
		if (opentype == LOWOPEN)
		{
			cc = endopen(psp);					/* unlock file */
			if (ret == CCOK)
				ret = cc;
		}
	}

	/* force exclusive mode processing in this case 
	 * since file cannot be modified by any process
	 */
	if ((psp->processmode == DENYWRITE) && (psp->readmode == CRDONLY))
		psp->processmode = EXCL;

	return(psp->retcde = ret);
}

/* opens file and readies for access */
int EDECL _bopen(psp, fle, filemode, rdmode)
struct passparm *psp;							/* file psp */
char *fle;											/* file name */
int filemode;										/* exclusive or shared */
int rdmode;											/* read mode */
{	
	return(__bopen(psp, fle, filemode, rdmode, LOWOPEN));
}

/* opens file and readies for access */
int EDECL bopen(psp, fle, filemode, rdmode, indexinfo)
struct passparm *psp;							/* file psp */
char *fle;											/* file name */
int filemode;										/* exclusive or shared */
int rdmode;											/* read mode */
NDXLIST *indexinfo;								/* index info list */
{	
	int cc;
	int i;
	int j;
	KEYSEGLIST *seginfo;
	int ret;

	cc = __bopen(psp, fle, filemode, rdmode, NORMOPEN);
	if (cc == CCOK)
	{
		cc = readwrt(psp);			/* check to see if in strtwrit */
		if (cc != CCOK)
		{
			endopen(psp);						/* unlock file */
			if (psp->filemode == EXCL) 	/* close the file */
				mexlclose(psp);
			else
				mshrclose(psp);

			setpsp(psp, PSPCLOSED);		/* set file to closed */
			return(cc);
		}

		if (indexinfo)
		{
			/* for every index check for the correct index type */
			for (i = 0; indexinfo[i].keyn && (i < MAXINDXS); i++)
			{
				if (indexinfo[i].keyn > MAXDINDX)
			      cc = BADFLDINX;	/* return error */
				else
				if (indexinfo[i].keytype > MAXINDTYP) 
			      cc = BADINDTYP;	/* return error */
				else
				if (psp->indtyp[indexinfo[i].keyn] != indexinfo[i].keytype)
			      cc = BADINDTYP;	/* return error */
				else
				/* if have segment list, check it */
				if (indexinfo[i].segptr)
				{
					for (	j = 0, seginfo = indexinfo[i].segptr;
							seginfo[j].keytype != ENDSEGLIST;
							j++)
					{
						if (seginfo[j].keytype > MAXINDTYP)
						{
							cc = BADSEGLIST;
							break;
						}
							if (j > MAXKLEN)
							cc = BADSEGLIST;
					}

					if (j == 0)
						cc = BADSEGLIST;
				}

				if (cc == CCOK)		/* save ptr to segment info for this index */
					psp->segptr[indexinfo[i].keyn] = indexinfo[i].segptr;
			}
		}
		else				/* null index info, check for string types */
		{
			/* for every index set the correct index type */
			for (i = 0; i < MAXDINDX; i++)
			{
				if (psp->indtyp[i] != STRINGIND)
				{
					cc = BADINDTYP;
					break;
				}

				psp->segptr[i] = NULL;			/* clear segment info ptrs */
			}
		}
	}

	/* if error, close the file */
	if (cc)
	{
		if (psp->status == PSPOPEN)
			mclose(psp);
	}
	else
	{
		/* delete left over semaphores on this share number */
	   if (psp->readmode != CRDONLY)
			cc = mzapsema(psp);		/* only if a read/write file */

		ret = endopen(psp);
		if (cc == CCOK)
			cc = ret;
	}

	return(psp->retcde = cc);
}


#endif



#ifdef MCREATE

/* create new file */
int EDECL bcreate(psp, file, filemode, bytemode, indexinfo)
char *file;											/* file name */
register struct passparm *psp;				/* file psp */
int filemode;										/* shared/excl */
int bytemode;										/* mode of byte flipping */
NDXLIST *indexinfo;								/* index info list */
{
	struct hdrrec hdr;
	int ret;
	FILHND ofd;
	NDEPTR rootnde;								/* starting root node number */
	int i;
	int clrsze;										/* space to clear in header */

	if (bytemode == INTELMODE)			/* set header to intel value of 1 */
	{
		psp->wordorder = 0;					/* clear all parts of flag */
		*((char *)&psp->wordorder) = 1;	/* set first byte */
	}
	else
	if (bytemode == NONINTELMODE)			/* set header to non-intel value of 1 */
	{
		psp->wordorder = 0;					/* clear all parts of flag */
		*((char *)&psp->wordorder + 3) = 1;	/* set last byte */
	}
	else
	if (bytemode == NATIVEMODE)			/* set header to native value of 1 */
	{
		psp->wordorder = INTELFMT;
	}
	else
		return(BADBYTEMODE);

	psp->errcde = 0;
	psp->filemode = EXCL;				/* exclusive mode for create */
	psp->processmode = EXCL;
	psp->skipupdread = FALSE;
	psp->filestat = UNLOCKED;		/* file is unlocked */
	psp->keyorder = INTELFMT;			/* reset key byte ordering to native */

#ifdef READSHARE
	psp->readstat = NOREADLOCK;			/* no read locks yet */
#endif
	psp->endque = 1;					/* assume another user */
	psp->memerr = CCOK;				/* assume no memory errors */
	psp->alterr = CCOK;				/* clear alternate error */
	psp->peekmsg  = FALSE;			/* default - no peek message (MSwindows) */
	psp->writeque = FALSE;			/* default - no write queue */
	psp->lazywrite= TRUE;			/* default - always write cache in sec lev 4 */
	psp->curroot = 0;					/* start out using default root */
	psp->keyn = 0;						/* no key processing at the moment */
	psp->writcnt = 0;					/* clear write lock count */
	psp->readcnt = 0;					/* clear read lock count */
	psp->fliprep = TRUE;				/* flip keys passed to cnextrep callback */
	psp->speedread = FALSE;			/* disable speed read by default */
	psp->speedahead = 0;
	psp->speedreq = 0;
	psp->speednod = (struct smallnode *)0;
	psp->speedentptr = (ENTRY *)0;

#ifndef NO_BLOCK_EXTEND
	psp->minextend = MINDATAEXTEND;	/* minimum number of nodes to extend when enlarging file */
#endif	/* !NO_BLOCK_EXTEND */

#ifdef TRANSACTION
		psp->transop     = 0;				/* clear transaction status vars */
		psp->begtransnde = 0;
		psp->curtransnde = 0;
		psp->transndeptr = NULL;
		psp->strttransnde = 0;
		psp->strttransndeptr = NULL;
		psp->transnum    = 0;
		psp->transseq    = 0;
		psp->transpsp    = NULL;			/* psp of mirror file */
#endif	/* TRANSACTION */

#ifdef IMAGEBACK
		psp->backnde = 0;
		psp->backlast = 0;
		psp->imagepsp = NULL;				/* no backup image file */

		/* clear image backup callback proceedures */
		psp->imagebackproc = (IMGPROCTYPE)NULL;
		psp->imagerestproc = (IMGPROCTYPE)NULL;
#endif

#ifdef REMOVED
	ret = CCOK;
#endif
	strncpy(psp->filename, file, MAXNAMELEN);   /* save file name */

	ret = initbcb(psp);							/* init buffer bcbs if needed */
	if (ret != CCOK)
		return(ret);								/* if error in init buffers */

	ofd = mshropen(psp, file, CRDWRITE);
	if (ofd != NULLFD)	/* file exists */
	{
	   mshrclose(psp);		/* close it */
	   return(FILEXIST);		/* dont create */
	}

	psp->fd = mexlcreate(psp, file);
	if (psp->fd != NULLFD)	/* good creat */
	{
#ifdef REMOVED
	   initcurs(psp);							/* initialize current pointers */
#endif
		psp->srecurs = FALSE;				/* turn off recursion flag */
		psp->writcnt = 0;						/* clear write lock count */
		psp->readcnt = 0;						/* clear read lock count */
		psp->extflag = 0;						/* clear extend flag */
		psp->pupdatnum = 1;					/* clear update count */
		psp->currec = -1;						/* clear current record number */
		psp->readmode = CRDWRITE;			/* read/write mode */

	   crtnwhdr(&hdr);	   /* creates new header record */
		hdr.hwordorder = psp->wordorder;

		if (indexinfo)
		{
			/* for every index set the correct index type */
			for (i = 0; indexinfo[i].keyn && (i < MAXINDXS); i++)
			{
				if (indexinfo[i].keyn > MAXDINDX)
				{
			      mexlclose(psp);				/* close bad */
			      ciunlink(file);
			      return(psp->retcde = BADFLDINX);	/* and return */
				}

				if (indexinfo[i].keytype > MAXINDTYP)
				{
			      mexlclose(psp);				/* close bad */
			      ciunlink(file);
			      return(psp->retcde = BADINDTYP);	/* and return */
				}

				hdr.hindtyp[indexinfo[i].keyn] = indexinfo[i].keytype;
			}
		}

		/* clear header area to zeros */
		cclear((char *) &ebuffer, sizeof(ebuffer), 0);
		for (clrsze = HDRSZE, ret = CCOK; clrsze && ret == CCOK;)
		{
			if (clrsze >= sizeof(ebuffer))
			{
				ret = ciwrite(psp, &ebuffer, sizeof(ebuffer));
				clrsze -= sizeof(ebuffer);
			}
			else
			{
				ret = ciwrite(psp, &ebuffer, clrsze);
				break;
			}
		}					

	   if (ret == CCOK) 
			ret = hdrwrte(psp, &hdr);			/* write out header */

	   if (ret != CCOK)  /* if bad header write */
	   {
	      psp->errcde = ret;				/* set error code */
	      mexlclose(psp);				/* close bad */
	      ciunlink(file);
	      return(psp->retcde = ret);	/* and return */
	   }

	   setfile(psp, &hdr);	 	/* move hdr info into file */
	   psp->seclev = 0;			/* start out at 0 */
	   if (ciextend(psp) != 0)	/* not even enough to start */
	   {
	      mexlclose(psp);		/* close it */
	      ciunlink(file);
	      return(psp->retcde = NOSPACE);
	   }

	   crtnwrt(&ebuffer);			/* make new root */
		rootnde = 0 + NODEBIAS;		/* root node number */
		ret = nodewrte(psp, &ebuffer, rootnde);
	   if (ret != CCOK) /* if bad root write */
	   {
	      psp->errcde = ret;
	      mexlclose(psp);		/* close it */
	      ciunlink(file);
	      return(psp->retcde = ret);	/* set error and return */
	   }
	}

	if (psp->fd != NULLFD)	/* if good creat */
   {
		ret = initcurs(psp);							/* initialize current pointers */
		if (ret != CCOK)
		{
 			mexlclose(psp);
	      ciunlink(file);
			return(ret);
		}

	   setpsp(psp, PSPOPEN);	/* set file to open */
		psp->updtflag = 1;		/* mark file as changed */

   	ret = mclose(psp);

		if (ret != CCOK)
		{
	      mexlclose(psp);		/* close it */
	      ciunlink(file);
			return(ret);
		}

		/* now dos updated and seclev set to DEF */
	   ret = bopen(psp, file, filemode, CRDWRITE, indexinfo);
		if (ret != CCOK)
		{
	      mexlclose(psp);		/* close it */
	      ciunlink(file);
			return(ret);
		}
   }

	return((psp->fd == NULLFD) ? FAIL : CCOK);
}

void FDECL crtnwhdr(hdr)		/* creates new header */
register struct hdrrec *hdr;
{
	hdr->hsingleroot = 0 + NODEBIAS;				/* root node number */
	hdr->hsinglelevels = 1;							/* number of levels in tree */
	hdr->hnext = 1 + NODEBIAS;				/* next available node */
	hdr->hnxtdrec = 1;						/* next data record number */
	hdr->hendofile = 0 + NODEBIAS;		/* so extend will do its thing */
	hdr->hfreenodes = -1;					/* node freechain */
	hdr->updatnum = 1;						/* starting update number */
	hdr->hdelcnt = 0;							/* deleted record count */
	hdr->hreccnt = 0;							/* active record count */
	hdr->hcustlen = 0;						/* custom header info length */
	hdr->htranschain = -1;					/* clear transaction log chain */
	hdr->htransback = -1;					/* clear transaction log backup chain */
	hdr->hres1 = 0;							/* reserved */
	hdr->hres2 = 0;
	hdr->hres3 = 0;
	hdr->hres4 = 0;

	cclear(hdr->hcustinfo, MAXCUSTINFO, 0);	/* clear custom info space */

	cclear(hdr->hindtyp, NUMINDXS, STRINGIND);/* all indexes are string type */
	cclear(hdr->hrootnum, sizeof(hdr->hrootnum), 0);/* all indexes use default root */
	cclear(hdr->hlevelslist, sizeof(hdr->hlevelslist), 0);
	cclear(hdr->hrootlist, sizeof(hdr->hrootlist), 0);
	hdr->hindtyp[SEMAINDEX] = BINARYIND;			/* set semaphore ind to binary */

   hdr->ck1 = 0x56;   /* 'V' */
   hdr->ck2 = 0x41;   /* 'A' */
   hdr->ck3 = 0x4c;   /* 'L' */
   hdr->ck4 = 0x49;   /* 'I' */
   hdr->ck5 = 0x44;   /* 'D' */
   hdr->ck6 = 0x41;   /* 'A' */
   hdr->ck7 = 0x54;   /* 'T' */
   hdr->ck8 = 0x31;   /* '1' */
}	

void FDECL crtnwrt(ndea)			/* creates new root */
register struct node *ndea;
{
	creatrt(ndea);		/* basic create */
	ndea->level = 0;
	ndea->p0 = 0;
}


int EDECL cchgndx(psp, indexinfo)
struct passparm *psp;
NDXLIST *indexinfo;
{
	int ret;
	int cc;
	int i;
	int j;
	KEYSEGLIST *seginfo;

	if ((ret = strtwrit(psp)) != CCOK)
	   return(ret);

	if (indexinfo)
	{
		/* for every index check for the correct index type */
		for (	i = 0, ret = CCOK; 
				indexinfo[i].keyn && (i < NUMINDXS) && (ret == CCOK); 
				i++)
		{
			if (indexinfo[i].keyn > MAXDINDX)
			{
		      ret = BADFLDINX;	/* index number too high */
			}
			else
			if (indexinfo[i].keytype > MAXINDTYP)
			{
		      ret = BADINDTYP;	/* key type is invalid */
			}
			else
			if (psp->indtyp[indexinfo[i].keyn] != indexinfo[i].keytype)
			{
				cc = cfirst(psp, indexinfo[i].keyn, NULL, 0);
				if (cc == FAIL)	/* if index is empty */
				{
					/* if have segment list, check it */
					if (indexinfo[i].segptr)
					{
						for (	j = 0, seginfo = indexinfo[i].segptr;
								seginfo[j].keytype != ENDSEGLIST;
								j++)
						{
							if (seginfo[j].keytype > MAXINDTYP)
							{
								ret = BADSEGLIST;
								break;
							}

							if (j > MAXKLEN)
								ret = BADSEGLIST;
						}

						if (j == 0)
							ret = BADSEGLIST;
					}

					if (ret == CCOK)			/* proceed to change type */
					{
						psp->indtyp[indexinfo[i].keyn] = indexinfo[i].keytype;
						ret = hdrupdte(psp);	/* need to keep header current */
					}
				}
				else
				{
					if (cc == CCOK)
						ret = INDEXIST;			/* index not be empty */
					else
						ret = cc;					/* some other error */
				}
			}
		}
	}
	else
		ret = BADINDTYP;					/* must have indexlist */


	if (ret == CCOK)
	{
		for (i = 1; i < NUMINDXS; i++)	/* for every index */
			psp->segptr[i] = NULL;			/* clear segment info ptr */

		/* for every index type, set segment info in psp */
		for (i = 0; indexinfo[i].keyn && (i < MAXDINDX); i++)
		{
			/* save ptr to segment info for this index */
			psp->segptr[indexinfo[i].keyn] = indexinfo[i].segptr;
		}
	}

	cc = endwrit(psp);
	if (cc != CCOK)
		ret = cc;

	return(ret);
}

		
#endif


#ifdef IMAGEBACK

/* create new file */
int EDECL imgcreate(psp, file, filemode)
char *file;											/* file name */
register struct passparm *psp;				/* file psp */
int filemode;										/* shared/excl */
{
	struct hdrrec hdr;
	int ret;
	FILHND ofd;
	NDEPTR rootnde;								/* starting root node number */
	int clrsze;										/* space to clear in header */

	/* set word order to native format (actually ignored) */
	{
		psp->wordorder = INTELFMT;
	}

	psp->errcde = 0;
	psp->filemode = EXCL;				/* exclusive mode for create */
	psp->processmode = EXCL;
	psp->skipupdread = FALSE;
	psp->filestat = UNLOCKED;		/* file is unlocked */
	psp->keyorder = INTELFMT;			/* reset key byte ordering to native */

#ifdef READSHARE
	psp->readstat = NOREADLOCK;			/* no read locks yet */
#endif
	psp->endque = 1;					/* assume another user */
	psp->memerr = CCOK;				/* assume no memory errors */
	psp->alterr = CCOK;				/* clear alternate error */
	psp->peekmsg  = FALSE;			/* default - no peek message (MSwindows) */
	psp->writeque = FALSE;			/* default - no write queue */
	psp->lazywrite= TRUE;			/* default - always write cache in sec lev 4 */
	psp->curroot = 0;					/* start out using default root */
	psp->keyn = 0;						/* no key processing at the moment */
	psp->writcnt = 0;					/* clear write lock count */
	psp->readcnt = 0;					/* clear read lock count */
	psp->fliprep = TRUE;				/* flip keys passed to cnextrep callback */
	psp->speedread = FALSE;			/* disable speed read by default */
	psp->speedahead = 0;
	psp->speedreq = 0;
	psp->speednod = (struct smallnode *)0;
	psp->speedentptr = (ENTRY *)0;

#ifndef NO_BLOCK_EXTEND
	psp->minextend = MINIMGEXTEND;	/* minimum number of nodes to extend when enlarging file */
#endif	/* !NO_BLOCK_EXTEND */

#ifdef TRANSACTION
		psp->transop     = 0;				/* clear transaction status vars */
		psp->begtransnde = 0;
		psp->curtransnde = 0;
		psp->transndeptr = NULL;
		psp->strttransnde = 0;
		psp->strttransndeptr = NULL;
		psp->transnum    = 0;
		psp->transseq    = 0;
		psp->transpsp    = NULL;			/* psp of mirror file */
#endif	/* TRANSACTION */

#ifdef IMAGEBACK
		psp->backnde = 0;
		psp->backlast = 0;
		psp->imagepsp = NULL;				/* no backup image file */

		/* clear image backup callback proceedures */
		psp->imagebackproc = (IMGPROCTYPE)NULL;
		psp->imagerestproc = (IMGPROCTYPE)NULL;
#endif

#ifdef REMOVED
	ret = CCOK;
#endif
	strncpy(psp->filename, file, MAXNAMELEN);   /* save file name */

	ret = initbcb(psp);							/* init buffer bcbs if needed */
	if (ret != CCOK)
		return(ret);								/* if error in init buffers */

	ofd = mshropen(psp, file, CRDWRITE);
	if (ofd != NULLFD)	/* file exists */
	{
	   mshrclose(psp);		/* close it */
	   return(FILEXIST);		/* dont create */
	}

	psp->fd = mexlcreate(psp, file);
	if (psp->fd != NULLFD)	/* good creat */
	{
#ifdef REMOVED
	   initcurs(psp);							/* initialize current pointers */
#endif
		psp->srecurs = FALSE;				/* turn off recursion flag */
		psp->writcnt = 0;						/* clear write lock count */
		psp->readcnt = 0;						/* clear read lock count */
		psp->extflag = 0;						/* clear extend flag */
		psp->pupdatnum = 1;					/* clear update count */
		psp->currec = -1;						/* clear current record number */
		psp->readmode = CRDWRITE;			/* read/write mode */

	   crtnwhdr(&hdr);	   /* creates new header record */
		hdr.hwordorder = psp->wordorder;

		/* clear header area to zeros */
		cclear((char *) &ebuffer, sizeof(ebuffer), 0);
		for (clrsze = HDRSZE, ret = CCOK; clrsze && ret == CCOK;)
		{
			if (clrsze >= sizeof(ebuffer))
			{
				ret = ciwrite(psp, &ebuffer, sizeof(ebuffer));
				clrsze -= sizeof(ebuffer);
			}
			else
			{
				ret = ciwrite(psp, &ebuffer, clrsze);
				break;
			}
		}					

	   if (ret == CCOK) 
			ret = hdrwrte(psp, &hdr);			/* write out header */

	   if (ret != CCOK)  /* if bad header write */
	   {
	      psp->errcde = ret;				/* set error code */
	      mexlclose(psp);				/* close bad */
	      ciunlink(file);
	      return(psp->retcde = ret);	/* and return */
	   }

	   setfile(psp, &hdr);	 	/* move hdr info into file */
	   psp->seclev = 0;			/* start out at 0 */
	   if (ciextend(psp) != 0)	/* not even enough to start */
	   {
	      mexlclose(psp);		/* close it */
	      ciunlink(file);
	      return(psp->retcde = NOSPACE);
	   }

	   crtnwrt(&ebuffer);			/* make new root */
		rootnde = 0 + NODEBIAS;		/* root node number */
		ret = nodewrte(psp, &ebuffer, rootnde);
	   if (ret != CCOK) /* if bad root write */
	   {
	      psp->errcde = ret;
	      mexlclose(psp);		/* close it */
	      ciunlink(file);
	      return(psp->retcde = ret);	/* set error and return */
	   }
	}

	if (psp->fd != NULLFD)	/* if good creat */
   {
		ret = initcurs(psp);							/* initialize current pointers */
		if (ret != CCOK)
		{
 			mexlclose(psp);
	      ciunlink(file);
			return(ret);
		}

	   setpsp(psp, PSPOPEN);	/* set file to open */
		psp->updtflag = 1;		/* mark file as changed */

   	ret = mclose(psp);

		if (ret != CCOK)
		{
	      mexlclose(psp);		/* close it */
	      ciunlink(file);
			return(ret);
		}

		/* now dos updated and seclev set to DEF */
	   ret = imgopen(psp, file, filemode, CRDWRITE);
		if (ret != CCOK)
		{
	      mexlclose(psp);		/* close it */
	      ciunlink(file);
			return(ret);
		}
   }

	return((psp->fd == NULLFD) ? FAIL : CCOK);
}


/* opens image backup file */
int EDECL imgopen(psp, fle, filemode, rdmode)
struct passparm *psp;							/* file psp */
char *fle;											/* file name */
int filemode;										/* exclusive or shared */
int rdmode;											/* read mode */
{	
	struct hdrrec hdr;
	int ret;
	int cc;
	int i;

	/**/

	setpsp(psp, PSPCLOSED);		/* set file to closed in case of problem */

	psp->peekmsg  = FALSE;			/* default - no peek message (MSwindows) */
	psp->writeque = FALSE;			/* default - no write queue */
	psp->lazywrite= TRUE;			/* default - always write cache in sec lev 4 */
	psp->writcnt = 0;					/* clear write lock count */
	psp->readcnt = 0;					/* clear read lock count */
	psp->memerr = CCOK;				/* clear memory allocation errors */
	psp->alterr = CCOK;				/* clear alternate error number */
	psp->fliprep = TRUE;				/* flip keys passed to cnextrep callback */
	psp->speedread = FALSE;			/* disable speed read by default */
	psp->speedahead = 0;
	psp->speedreq = 0;
	psp->speednod = (struct smallnode *)0;
	psp->speedentptr = (ENTRY *)0;

#ifndef NO_BLOCK_EXTEND
	psp->minextend = MINIMGEXTEND;	/* minimum number of nodes to extend when enlarging file */
#endif	/* !NO_BLOCK_EXTEND */

	strncpy(psp->filename, fle, MAXNAMELEN);   /* save file name */

	/* if the os cannot do locking functions, force exclusive mode */
	if (mtstos() != CCOK)
		filemode = EXCL;

	psp->errcde = 0;
	psp->filemode = (char)filemode;	/* shared or exclusive file opens */
	psp->processmode = (char)filemode;	/* shared or exclusive processing */

	ret = initbcb(psp);							/* init buffer bcbs if needed */
	if (ret != CCOK)
		return(ret);								/* if error in init buffers */

	if (filemode == EXCL)
	{
		psp->fd = mexlopen(psp, fle, rdmode);		/* exclusive open */
	}
	else
	{
		psp->fd = mshropen(psp, fle, rdmode);		/* shared open */
		if (psp->fd == NULLFD) 						/* if bad open */
		{
			filemode = EXCL;
			psp->filemode = (char)filemode;	/* shared or exclusive file opens */
			psp->processmode = (char)filemode;	/* shared or exclusive processing */
			psp->fd = mexlopen(psp, fle, rdmode);	/* try again in exclusive mode */
		}
	}

	if (psp->fd != NULLFD) 						/* if good open */
	{
		setpsp(psp, PSPOPEN);				/* set file to open */
#ifdef REMOVED
		initcurs(psp);							/* initialize current pointers */
#endif
		psp->srecurs = FALSE;				/* turn of recursion flag */
		psp->filestat = UNLOCKED;			/* file is unlocked */
		psp->keyorder = INTELFMT;			/* reset key byte ordering to native */

#ifdef READSHARE
	psp->readstat = NOREADLOCK;			/* no read locks yet */
#endif
		psp->endque = 1;						/* assume another user */
		psp->extflag = FALSE;				/* clear extend flag */
		psp->readmode = (char)rdmode;		/* read-only or read/write */
		psp->skipupdread = FALSE;
		psp->pupdatnum = 0;					/* clear update count */
		psp->currec = -1;						/* clear current record number */
		psp->keyn = 0;							/* no key processing at the moment */

#ifdef TRANSACTION
		psp->transop     = 0;				/* clear transaction status vars */
		psp->begtransnde = 0;
		psp->curtransnde = 0;
		psp->transndeptr = NULL;
		psp->strttransnde = 0;
		psp->strttransndeptr = NULL;
		psp->transnum    = 0;
		psp->transseq    = 0;
		psp->transpsp    = NULL;			/* psp of mirror file */
#endif	/* TRANSACTION */

		psp->backnde = 0;
		psp->backlast = 0;
		psp->imagepsp = NULL;				/* no backup image file */

		/* clear image backup callback proceedures */
		psp->imagebackproc = (IMGPROCTYPE)NULL;
		psp->imagerestproc = (IMGPROCTYPE)NULL;

		for (i = 1; i < NUMINDXS; i++)	/* for every index */
		{
			psp->segptr[i] = NULL;			/* clear segment info ptr */
			psp->curlock[i]  = UNLOCKED;	/* set unlocked record status */
		}

		ret = setshrbyte(psp);				/* assign share byte */

		if (ret == CCOK)
			ret = strtopen(psp);					/* SHARE module not loaded into mem */

		if (ret == BADQUE)
		{
			mshrclose(psp);					/* close the file */
			setpsp(psp, PSPCLOSED);		/* set file to closed */
			return(ret);
		}

		if (ret != CCOK)
		{
			if (filemode != EXCL)
			{
				mshrclose(psp);					/* close the file */
				setpsp(psp, PSPCLOSED);		/* set file to closed */
				filemode = EXCL;						/* set exclusive mode */
				psp->filemode = (char)filemode;	/* shared or exclusive file opens */
				psp->processmode = (char)filemode;
				psp->fd = mexlopen(psp, fle, rdmode);	/* try again in exclusive mode */

				if (psp->fd == NULLFD) 					/* if bad open */
					return(FILENOTLOCKED);

				setpsp(psp, PSPOPEN);				/* set file to open */
			}
			else
				return(FILENOTLOCKED);
		}

		if (filemode == EXCL)
		{
			ret = hdrread(psp, &hdr);

	   	if (ret != CCOK)	/* bad read */
		   {
		      psp->errcde = errcde;		/* set error code */
				endopen(psp);					/* unlock file */
				mexlclose(psp);				/* close errant file */
				setpsp(psp, PSPCLOSED);		/* set file to closed */
	   	   return(psp->retcde = ret);
		   }

			setfile(psp, &hdr);			/* move header info in */
		}
		else
		{
			if (ret == CCOK)
		   	ret = sethdrinfo(psp);	 

			if (ret != CCOK)
			{
		      psp->errcde = errcde;		/* set error code */
				endopen(psp);					/* unlock file */
				mshrclose(psp);				/* close errant file */
				setpsp(psp, PSPCLOSED);		/* set file to closed */
				return(psp->retcde = ret);
			}

		}

		if (filemode == EXCL)
			psp->seclev = DEFLEV;	/* start out at single user security level */
		else
			psp->seclev = DEFLEVMU;	/* start out at multi-user security level */

		/* set default flags */
		psp->checkflg = TRUE;			/* default - always check datalist */
		psp->fulldel  = FALSE;			/* default - never delete up tree */
		psp->reuse    = TRUE;			/* default - reuse record numbers */
		psp->setdrec  = -1;				/* use internal record # for dadd() */
		psp->nolock   = FALSE;			/* default - require record lock */
		psp->srchorder= 0;				/* default - normal key comparison */
		psp->key = 0;
	}


	if (psp->fd == NULLFD)
	{
		ret = FAIL;						/* failed file open */
		setpsp(psp, PSPCLOSED);		/* set file to closed */
	}
	else
	{
		ret = initcurs(psp);							/* initialize current pointers */
		if (ret != CCOK)
		{
			endopen(psp);						/* unlock file */
			if (psp->filemode == EXCL) 	/* close the file */
				mexlclose(psp);
			else
				mshrclose(psp);
			setpsp(psp, PSPCLOSED);			/* set file to closed */
			return(psp->retcde = ret);
		}

		cc = endopen(psp);					/* unlock file */
		if (ret == CCOK)
			ret = cc;
	}

	return(psp->retcde = ret);
}

/* closes image backup file - does not do clean-up of locks or buffers, etc */
int EDECL imgclose(psp)		
register struct passparm *psp;
{
	int ret;
	int closeret;

	/**/


	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);
	psp->lazywrite = FALSE;			/* cannot have lazy write after close */
	clrshrbyte(psp, psp->sharenum);		/* release share byte */

	closeret = (psp->filemode == EXCL) 	/* close the file */
						? mexlclose(psp) 
						: mshrclose(psp);

	if (closeret != 0)					/* couldnt close ? */
	   return(psp->retcde = FAIL);	/* return error */

	setpsp(psp, PSPCLOSED);	/* set file to close */

	ret = endcurs(psp);		/* deallocate memory for current information */

	if (ret == CCOK)
	{
		psp->backnde = 0;
		psp->backlast = 0;
		psp->imagepsp = NULL;				/* cancel backup image file */

		/* clear image backup callback proceedures */
		psp->imagebackproc = (IMGPROCTYPE)NULL;
		psp->imagerestproc = (IMGPROCTYPE)NULL;
	}

	return(ret);
}
#endif	/* IMAGEBACK */

#ifdef MCLOSE
int EDECL mclose(psp)		/* closes file */
register struct passparm *psp;
{
	int ret;
	int closeret;
	int i;
	int cc;

	/**/


	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	/* lock file for close processing */
   if (psp->readmode == CRDONLY)
		cc = strtread(psp);
	else
		cc = strtwrit(psp);

	if (cc != CCOK)		/* if failed to lock file */
	   return(cc);

   if (psp->readmode != CRDONLY)
	{
		for (i = 1; i <= MAXINDXS; i++)
		{
			if (tstcuradr(psp, i))				/* if active index */
				mclrlock(psp, i);
		}

		psp->errcde = 0;

		if (psp->updtflag)	/* header modified ? */
		{
		   if ((ret = hdrupdte(psp)) != CCOK)	/* if bad hdr write */
		   {
			   if (psp->readmode == CRDONLY)
					endread(psp);
				else
					endwrit(psp);
		      psp->errcde = ret;
		      return(psp->retcde = DSKERR);	/* set and ret error */
		   }
		}
	}

	if ((ret = clsbcb(psp)) != CCOK)  /* write out dirty bufs */
	{				/* if error */
		if (psp->readmode == CRDONLY)
			endread(psp);
		else
			endwrit(psp);
	   psp->errcde = ret;
	   return(psp->retcde = DSKERR);		/* set and return error */
	}

	psp->lazywrite = FALSE;			/* cannot have lazy write after close */
	if (psp->readmode == CRDONLY)
		ret = endread(psp);
	else
		ret = endwrit(psp);

	if (ret == CCOK)
	{
		clrshrbyte(psp, psp->sharenum);		/* release share byte */

		closeret = (psp->filemode == EXCL) 	/* close the file */
							? mexlclose(psp) 
							: mshrclose(psp);

		if (closeret != 0)					/* couldnt close ? */
		   return(psp->retcde = FAIL);	/* return error */

		setpsp(psp, PSPCLOSED);	/* set file to close */

		ret = endcurs(psp);		/* deallocate memory for current information */
	}

#ifdef IMAGEBACK
	if (ret == CCOK)
	{
		psp->backnde = 0;
		psp->backlast = 0;
		psp->imagepsp = NULL;				/* cancel backup image file */

		/* clear image backup callback proceedures */
		psp->imagebackproc = (IMGPROCTYPE)NULL;
		psp->imagerestproc = (IMGPROCTYPE)NULL;
	}
#endif

	return(ret);
}

#endif

