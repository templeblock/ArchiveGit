/*  cindexs.c - single-key source file for C-Index
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

 
#ifdef COMPS			/* compile all single key functions in this source module*/
#define COPEN
#define CCREATE
#define CCLOSE
#define CFLUSH
#define CSETFILE
#define CUNQADD
#define CDUPADD
#define CCHANGE
#define CSAVE
#define CFIND
#define CDELETE
#define CNEXT
#define CPREV
#define CFIRST
#define CLAST
#define CGETCUR
#define CDELCUR
#define CCHGCUR
#define CNEXTREC
#define CSETREC
#define CBUILD
#define HDRCNT
#define HDRCUST

#endif



#ifdef COPEN

/* opens file and readies for access using multi-user exclusive open */
int EDECL copen(psp, fle)
char *fle;	/* file name */
register struct passparm *psp;
{	
	return(mopen(psp, fle, EXCL));
}

#endif

#ifdef CCREATE

/* create new file and readies for access using multi-user exclusive create */
int EDECL ccreate(psp, file)
char *file;			/* file name */
register struct passparm *psp;
{
	return(mcreate(psp, file, EXCL));	/* one user */
}

#endif


#ifdef CFLUSH

int EDECL cflush(psp)		/* flush buffers (psp) */
struct passparm *psp;
{
	int ret;
	int cc;

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

#ifdef REMOVED
	if (psp->processmode != EXCL)			/* if shared mode, don't need cflush */
		return(CCOK);
#endif

	psp->errcde = 0;

	if ((ret = strtwrit(psp)) != CCOK)
	   return(ret);

	ret = CCOK;

	if (psp->updtflag)			/* need to write out header */
	   ret = hdrupdte(psp);		/* then do it */

	if (ret == CCOK)
		ret = bflush(psp);		/* do it */

	if (psp->extflag && ret == CCOK)	
	{
		ret = upddisk(psp);		/* update disk FAT table (or equiv) */
		psp->extflag = FALSE;	/* clear flag */
	}

	cc = endwrit(psp);
	if (cc != CCOK)
		ret = cc;

	return(psp->retcde = ret);
}

#endif

#ifdef CSETFILE

int EDECL csetfile(psp, level)		/* set security level */
struct passparm *psp;
int level;
{
	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	/* in shared mode force level 1 from 2 or 3 */
	if ((psp->processmode != EXCL)
		 && (level > 1)
		 && (level < 4))
		level = 1;
	
	if (goodlevel(level))		/* if good level */
	{
	   psp->seclev = level;		/* set level */	
	   return(CCOK);
	}

   return(FAIL);
}

#endif

#ifdef CUNQADD

int EDECL cunqadd(psp, keyn, key, rec, data, dlen)	/* high level add */
struct passparm *psp;
int keyn;		/* index number */
KEYPTR key;		/* key */
long rec;		/* record number */
void *data;		/* data to add */
int dlen;		/* data length */
{
	int ret;

	/* set for lower level routine */
	psp->keyn = keyn;
	psp->key = key;
	psp->keyorder = INTELFMT;			/* key byte ordering is native */
	psp->rec = rec;
	psp->data = data;
	psp->dlen = dlen;

	ret = _cunqadd(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
	
	return(ret);
}


int FDECL _cunqadd(psp)		/* add new data record */
struct passparm *psp;
{
	int tkeyn;
	KEYPTR tkey;
	INT32 tmode;
	long trec;
	int tdlen;
	int ret;
	int cc;

	/**/

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	/* save passed parameters */
	tkeyn = psp->keyn;
	tkey = psp->key;
	tmode = psp->keyorder;
	trec = psp->rec;
	tdlen  = psp->dlen;

	psp->rec = 0L;
	psp->dlen = 0;		/* dont find any data */

	ret = _cfind(psp);

	/* restore parameters */
   psp->keyn = tkeyn;
	if (psp->key != tkey)
	{
	   psp->key = tkey;
		psp->keyorder = tmode;
	}
#ifndef NO_WORD_CONVERSION
	else
	{
		cipspflip(psp, INTELFMT);
	}
#endif	/* !NO_WORD_CONVERSION */
   psp->rec = trec;
   psp->dlen = tdlen;
	   
	if (ret == CCOK || ret == KEYMATCH) 
	/* if found key */  		
	{
		cc = endwrit(psp);
		if (cc != CCOK)
			ret = cc;
		else
			ret = FAIL;

	   return(psp->retcde = ret);	/* and return */
	}

   ret = ___cadd(psp, UNQ);

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

   return(ret);
}

#endif

#ifdef CDUPADD

int EDECL cdupadd(psp, keyn, key, rec, data, dlen)	/* high level add */
struct passparm *psp;
int keyn;
KEYPTR key;
long rec;
void *data;
int dlen;
{
	int ret;

	psp->keyorder = INTELFMT;			/* key byte ordering is native */
	ret = __cadd(psp, keyn, key, rec, data, dlen, DUP);
#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}


int FDECL _cdupadd(psp)		/* add new data record */
struct passparm *psp;
{
	return(___cadd(psp, DUP));
}

#endif

#ifdef CCHANGE

int EDECL cchange(psp, keyn, key, rec, data, dlen) /* high level change */
struct passparm *psp;
int keyn;
KEYPTR key;
long rec;
void *data;
int dlen;
{
	int ret;
	int cc;

	/**/

	psp->keyorder = INTELFMT;			/* key byte ordering is native */

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	if (psp->processmode == EXCL)
	{
		ret = __cadd(psp, keyn, key, rec, data, dlen, CHANGE);
	}
	else			/* shared mode */
	{
		/* set for lower level routine */
		psp->keyn = keyn;
		psp->key = key;
		psp->rec = rec;
		psp->data = data;
		psp->dlen = dlen;

		ret = _cchange(psp);				/* do change */
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}


int FDECL _cchange(psp)		
struct passparm *psp;
{
	int ret;
	void *tempdata;			/* temp storage of data pointer */
	int tempdlen;				/* temp storage of data length */
	int cc;

	/**/

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	if (psp->processmode == EXCL)
	{
		ret = ___cadd(psp, CHANGE);
	}
	else			/* shared mode */
	{
		tempdata = psp->data;							/* save data pointer */
		tempdlen = psp->dlen;							/* save data length */

		psp->data = 0;
		psp->dlen = 0;
		ret = _cfind(psp);								/* relocate key */

		if (ret > 0)
			ret = FAIL;					/* translate KEYMATCH and GREATER to CCOK */

		if (ret == CCOK)
		{
			if ((ret = msetlock(psp, psp->keyn)) == CCOK) /* lock the record */
			{
				psp->data = tempdata;					/* restore data info */
				psp->dlen = tempdlen;
				ret = _cchgcur(psp);						/* change record */

				cc = mclrlock(psp, psp->keyn);		/* remove lock on record */
				if (ret == CCOK)
					ret = cc;
			}
		}
	}

   cc = endwrit(psp);									/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);
}

#endif

#ifdef CSAVE
	
int EDECL csave(psp, keyn, key, rec, data, dlen)	/* high level save */
struct passparm *psp;
int keyn;
KEYPTR key;
long rec;
void *data;
int dlen;
{
	int ret;

	/* set for lower level routine */
	psp->keyn = keyn;
	psp->key = key;
	psp->keyorder = INTELFMT;			/* key byte ordering is native */
	psp->rec = rec;
	psp->data = data;
	psp->dlen = dlen;

	ret = _csave(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}


int FDECL _csave(psp)		/* add new data record */
struct passparm *psp;
{
	int ret;
	int cc;
	int tempkeyn;						/* temp index # */
	char tempkey[MAXMKLEN];			/* temp key storage */
	KEYPTR tempkptr;					/* temp key pointer */
	INT32 tempkmode;						/* temp key byte order */
	long temprec;						/* temp rec storage */
	void *tempdata;					/* temp data ptr storage */
	int tempdlen;						/* temp dlen storage */


	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	if (psp->processmode == EXCL)			/* no locking requirements */
	{
		ret = ___cadd(psp, SAVE);		/* change/add entry */
	}
	else
	{
		tempkeyn = psp->keyn;			/* save index number */
		tempkptr = psp->key;				/* before cfind save key ptr */
		tempkmode = psp->keyorder;
#ifdef REMOVED
		strcpy(tempkey, psp->key);		/* save key */
#endif
		if (psp->key)
			cikeycpy(psp, psp->keyn, tempkey, psp->key);		/* save key */
		temprec = psp->rec;										/* and save rec # */

		tempdata = psp->data;			/* save data and dlen before cfind */
		tempdlen = psp->dlen;

		psp->data = "";					/* no data to return from cfind */
		psp->dlen = 0;

		ret = _cfind(psp);				/* reposition for change */

		psp->keyn = tempkeyn;			/* restore index # */
		psp->key = tempkptr;				/* restore key ptr */
		psp->keyorder = tempkmode;		/* restore key byte order flag */
		if (psp->key)
			cikeycpy(psp, psp->keyn, psp->key, tempkey);	/* restore key in case in psp */
		psp->rec = temprec;				/* restore rec # */
		psp->data = tempdata;			/* restore data and dlen */
		psp->dlen = tempdlen;

		if (ret == CCOK)
		{
			ret = msetlock(psp, psp->keyn);

			if (ret == CCOK)
				ret = _cchgcur(psp);

			cc = mclrlock(psp, psp->keyn);
			if (ret == CCOK)
				ret = cc;
		}
		else
		{
			if (ret == FAIL || ret > 0)
			{
				ret = _cdupadd(psp);
			}
		}
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);
}

#endif

#ifdef CFIND

int EDECL cfind(psp, keyn, key, rec, data, dlen)	/* high level find */
struct passparm *psp;
int keyn;
KEYPTR key;
long rec;
void *data;
int dlen;
{
	int ret;

	/* set for lower level routine */
	psp->keyn = keyn;
	psp->key = key;
	psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
	psp->rec = rec;
	psp->data = data;
	psp->dlen = dlen;

	ret = _cfind(psp);
#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}	


int FDECL _cfind(psp)	/* get data rec - put into data */
register struct passparm *psp;
{
	int ret;

	/* if speed reading is turned on, 
		and not inside a read or write 
		and current key not locked */
	if (psp->speedread && !psp->readcnt && !psp->writcnt && (psp->curlock[psp->keyn] == UNLOCKED))
		ret = __cfindsp(psp);			/* do speed read cfind */
	else
		ret = __cfind(psp);				/* else do regular cfind */

	return(ret);
}

int FDECL __cfind(psp)	/* get data rec - put into data */
register struct passparm *psp;
{
#ifdef REMOVED
	char buf[256], *td;
#endif
	void *td;
	int tl;			/* temporary data length */
	int dataret;	/* return from cigetdata */
	int keyret;		/* return from findkey */
	int ret;
	int cc;

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	if (strtread(psp) != CCOK)					/* start reading operation */
		return(psp->retcde = FILELOCKED);

   cc = _mclrlck(psp, psp->keyn);	/* always unlock current for any operation */
	if (cc != CCOK)
	{
		psp->alterr = endread(psp);	/* release file */
		return(psp->retcde = cc);		/* return error from _mclrlck */
	}

	/* save for caller */
	td = psp->data;
	tl = psp->dlen;
	psp->dbyte = 0;		/* get first portion */
	psp->errcde = 0;	/* start out good */

	psp->retcde = findkey(psp);		/* find first portion */
	keyret = psp->retcde;

	if (psp->retcde < 0)					/* if FAIL or serious error */ 
	{
	   psp->data = td;					/* restore data pointer */

		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			psp->retcde = cc;

	   return(psp->retcde);
	}

#ifdef REMOVED
	if (psp->curindx[psp->keyn] != psp->keyn) /* if in wrong index */
#endif
	if (getcurindx(psp, psp->keyn) != psp->keyn)		/* if in wrong index */
	{   
	   setnull(psp);		/* nothing passed back */
	   psp->data = td;		/* restore data pointer */
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		else
			ret = FAIL;
	   return(psp->retcde = ret);
	}

	dataret = cigetdata(psp, td, tl);	
	/* get rest of data, set found length */
	psp->data = td;		/* restore data pointer */
#ifdef REMOVED
	psp->key++;   /* point to actual key (get past key number) */
#endif
 	if (dataret == CCOK)
	{
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		else
			ret = keyret;
	   return(psp->retcde = ret);
	}

	cc = endread(psp);				/* release file after read */
	if (cc != CCOK)
		ret = cc;
	else
		ret = dataret;
   return(psp->retcde = ret);	/* incomplete */
}

/* get data rec - put into psp->data buffer - speed read method */
int FDECL __cfindsp(psp)
register struct passparm *psp;
{
	void *td;
	int tl;							/* temporary data length */
	INT32 tkeymode;				/* key order for input key */
	int dataret;					/* return from cigetdata */
	int keyret;						/* return from findkey */
	char keysav[MAXMKLEN];		/* space to save key passed by application */
	long recsav;					/* save record number passed by application */

	int ret;

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	/* save for caller */
	td = psp->data;
	tl = psp->dlen;
	psp->dbyte = 0;		/* get first portion */
	psp->errcde = 0;	/* start out good */

	/* save key and rec from application in case findkeysp or cigetdatasp fails */
	cikeycpy(psp, psp->keyn, keysav, psp->key);
	tkeymode = psp->keyorder;
	recsav = psp->rec;

	psp->retcde = findkeysp(psp);		/* find first portion */
	if (psp->retcde == NODENOTINMEM)	/* if need to read disk, do regular cfind */
	{
		/* restore key from application */
		cikeycpy(psp, psp->keyn, psp->key, keysav);
		psp->keyorder = tkeymode;
		psp->rec = recsav;
		psp->dlen = tl;
		psp->data = td;
		psp->speedreq = 0;		/* no read ahead on cfind */
		return(__cfind(psp));
	}

	keyret = psp->retcde;

	if (psp->retcde < 0)					/* if FAIL or serious error */ 
	{
	   psp->data = td;					/* restore data pointer */
	   return(psp->retcde);
	}

	if (getcurindx(psp, psp->keyn) != psp->keyn)		/* if in wrong index */
	{   
	   setnull(psp);		/* nothing passed back */
	   psp->data = td;		/* restore data pointer */
	   return(psp->retcde = FAIL);
	}

	/* get rest of data, set found length */
	dataret = cigetdatasp(psp, td, tl);	

	psp->data = td;		/* restore data pointer */
 	if (dataret == CCOK)
	{
	   ret = keyret;
	}
	else
	if (dataret == NODENOTINMEM)		/* error reading bufs from memory */
	{
		/* restore key from application */
		cikeycpy(psp, psp->keyn, psp->key, keysav);
		psp->keyorder = tkeymode;
		psp->rec = recsav;
		psp->dlen = tl;
		psp->data = td;
		psp->speedreq = 0;
		ret = __cfind(psp);						/* do regular cfind */
	}
	else
	{
		ret = dataret;
	}

   return(psp->retcde = ret);	/* incomplete */
}

#endif

#ifdef CDELETE

int EDECL cdelete(psp, keyn, key, rec)	/* high level delete */
struct passparm *psp;
int keyn;
KEYPTR key;
long rec;
{
	int ret;

	/* set for lower level routine */
	psp->keyn = keyn;
	psp->key = key;
	psp->keyorder = INTELFMT;			/* key byte ordering is native */
	psp->rec = rec;

	ret = _cdelete(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}

int FDECL _cdelete(psp)	/* deletes data record */
register struct passparm *psp;
{
	int ret;
	int cc;
	void *tempdata;			/* temp data ptr storage */
	int tempdlen;				/* temp dlen storage */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if (!goodindex(psp->keyn))				/* if not good index */
	   return(psp->retcde = PARMERR); 	/* return error */

	if (strtwrit(psp) != CCOK)					/* start reading operation */
		return(psp->retcde = FILELOCKED);

	if (psp->processmode == EXCL)
	{
		ret = __cdlt(psp);			/* actual delete operation */
	}
	else
	{
		tempdata = psp->data;		/* save data and dlen before cfind */
		tempdlen = psp->dlen;

		psp->data = "";				/* no data to return from cfind */
		psp->dlen = 0;
		ret = _cfind(psp);			/* reposition before delete */

		if (ret > 0)
			ret = FAIL;					/* translate KEYMATCH and GREATER to FAIL */

		if (ret == CCOK)
		{
			if ((ret = mtstlock(psp, psp->keyn)) != RECSHLK)
				ret = __cdlt(psp);				/* delete record */
		}

		psp->data = tempdata;		/* restore data and dlen before return */
		psp->dlen = tempdlen;
	}

	cc = endwrit(psp);				/* release file after delete */
	if (cc != CCOK)
		ret = cc;

	return(ret);	
}


int FDECL __cdlt(psp)					/* internal function - deletes data record */
register struct passparm *psp;
{
	int ret;
#ifdef REMOVED
	char buf[256], *td, *tp;
#endif
	void *td;
	void *tp;
	INT32 tm;
	int ti;				/* temp index # */
	int cc;

	/**/

	psp->errcde = 0;	/* start out good */

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

   cc = _mclrlck(psp, psp->keyn);	/* always unlock current for any operation */
	if (cc != CCOK)
	{
		psp->alterr = endwrit(psp);		/* release file */
		return(cc);								/* return error from _mclrlck */
	}

	td = psp->data;					/* save for caller */
	tp = psp->key;						/* "" */
	tm = psp->keyorder;				/* "" */
	ti = psp->keyn;					/* "" */
	psp->dbyte = 0;					/* get first key */

	ret = delkey(psp);	/* delete key */

	psp->keyn = ti;		/* restore */
	psp->data = td;		/* restore */
	if (psp->key != tp)
	{
		psp->key = tp;			/* restore */
		psp->keyorder = tm;	/* restore */
	}
#ifndef NO_WORD_CONVERSION
	else
	{
		cipspflip(psp, INTELFMT);
	}
#endif	/* !NO_WORD_CONVERSION */

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);
}

#endif

#ifdef CNEXT

int EDECL cnext(psp, keyn, data, dlen)	/* high level next */
struct passparm *psp;
int keyn;
void *data;
int dlen;
{
	int ret;

	/* set for lower level routines */
	psp->keyn = keyn;
	psp->key  = 0;
	psp->data = data;
	psp->dlen = dlen;
	psp->keyorder = INTELFMT;			/* key byte ordering is native */

	ret = _cnext(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}	


int FDECL _cnext(psp)	/* gets next data record */
register struct passparm *psp;
{
	int ret;

	/* if speed reading is turned on and current key not locked */
	if (psp->speedread && (psp->curlock[psp->keyn] == UNLOCKED))
		ret = __cnextsp(psp);			/* do speed read cnext */
	else
		ret = __cnext(psp);				/* else do regular cnext */

	return(ret);
}


/* gets next data record - regular method */
int FDECL __cnext(psp)
register struct passparm *psp;
{
	void *td;
	int tl;
	int dataret;
	int ret;
   char tempkey[MAXMKLEN];				/* incremented key value */
	long temprec;							/* incremented record num */
#ifdef REMOVED
   static char end[] = { 1, 0 };
#endif
	int cc;
	int lastdbyte;
	INT32 rcval;

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	if (strtread(psp) != CCOK)					/* start reading operation */
		return(psp->retcde = FILELOCKED);

	cc = _mclrlck(psp, psp->keyn);	/* always unlock current for any operation */
	if (cc != CCOK)
	{
		psp->alterr = endread(psp);	/* release file */
		return(psp->retcde = cc);		/* return error from _mclrlck */
	}

	/* save for caller */
	tl = psp->dlen;
	td = psp->data;
	psp->errcde = 0;	/* start out good */

	if (getcurindx(psp, psp->keyn) == -1)	/* not yet intitialized */
	{
	   ret= _cfirst(psp);		/* then get first key */
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		return(ret);
	}

	/* if need to reposition */
	if ((psp->processmode != EXCL) && (getupdkeyinfo(psp, psp->keyn)))
	{
		rcval = getcurrcval(psp, psp->keyn);
	   if ((rcval + 1) < rcval)
	   /* cant increment */
	   {
			getcurkey(psp, psp->keyn, tempkey);
#ifdef REMOVED
	      strcat(tempkey, end);	/* one bigger now */
#endif
			incrkeyval(psp, psp->keyn, psp->indtyp[psp->keyn], tempkey);
			psp->key = tempkey;
			psp->keyorder = psp->wordorder;

			psp->rec = 0L;
	      ret = _cfind(psp);

			if (ret == GREATER || ret == KEYMATCH)
				ret = psp->retcde = CCOK;

			if (ret != CCOK)		/* past end, save key so nexts will stay at end */
			{
				putcurkey(psp, psp->keyn, tempkey);
				putcurrcval(psp, psp->keyn, 0L);
			}

			putupdkeyinfo(psp, psp->keyn, FALSE);	/* EXCL key info is current */

			cc = endread(psp);				/* release file after read */
			if (cc != CCOK)
				ret = cc;
			return(ret);
	   }
	   else
	   {
			getcurkey(psp, psp->keyn, tempkey);
			temprec = getcurrcval(psp, psp->keyn) + 1;
			psp->key = tempkey;
			psp->keyorder = psp->wordorder;
			psp->rec = temprec;
	      ret = _cfind(psp);
			if (ret == GREATER || ret == KEYMATCH)
				ret = psp->retcde = CCOK;

			/* past end, save key and rec vals so nexts will stay at end */
			if (ret != CCOK)
			{
				putcurkey(psp, psp->keyn, tempkey);
				putcurrcval(psp, psp->keyn, temprec);
			}

			putupdkeyinfo(psp, psp->keyn, FALSE);	/* EXCL key info is current */

			cc = endread(psp);				/* release file after read */
			if (cc != CCOK)
				ret = cc;
			return(ret);
	   }
	}

	if (getcurindx(psp, psp->keyn) > psp->keyn) /* if past key area */
	{	
	   setnull(psp);		/* no returned parms */
	   psp->data = td;		/* restore data pointer */
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		else
			ret = FAIL;
	   return(psp->retcde = ret);
	}

	lastdbyte = getcurdbyte(psp, psp->keyn);

	while (	(nextkey(psp) > -1)
			&& !(lastdbyte & 128)
			&& ((psp->dbyte & 127) > (lastdbyte & 127)) )
		lastdbyte = psp->dbyte;



	/* didnt find any next key OR key was incomplete */ 
	if ((psp->retcde == FAIL) || (psp->retcde == INCOMPLETE))
	{
	   psp->data = td;					/* restore data portion */
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			psp->retcde = cc;

	   return (psp->retcde);
	}

	/* if in wrong index, key not really found */
	if (getcurindx(psp, psp->keyn) != psp->keyn)
	{
	   setnull(psp);		/* set notfound conditions */
	   psp->data = td;
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		else
			ret = FAIL;
	   return (psp->retcde = ret);
	}

	dataret = cigetdata(psp, td, tl);	/* get rest of data */
	psp->data = td;

 	if (dataret == CCOK)
		ret = CCOK;
	else
		ret = dataret;

	cc = endread(psp);				/* release file after read */
	if (cc != CCOK)
		ret = cc;

   return(psp->retcde = ret);
} 		


/* gets next data record - speed read method */
int FDECL __cnextsp(psp)
register struct passparm *psp;
{
	void *td;
	int tl;
	int ret;
	int lastdbyte;
	CURINFO curkeysav;					/* space to save current key info */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	/* save for caller */
	tl = psp->dlen;
	td = psp->data;
	psp->errcde = 0;	/* start out good */

	/* not current key info yet intitialized */
	if (getcurindx(psp, psp->keyn) == -1)
	{
		psp->speedreq = psp->speedahead;		/* ask to read ahead */
		ret = __cnext(psp);						/* do regular cnext */
		psp->speedreq = 0;
		return(ret);
	}

	if (getcurindx(psp, psp->keyn) > psp->keyn) /* if past key area */
	{	
	   setnull(psp);		/* no returned parms */
	   psp->data = td;		/* restore data pointer */
		ret = FAIL;
	   return(psp->retcde = ret);
	}

	/* save current key info in case nextkeysp or cigetdatasp fails */
	cisavecurinfo(psp, psp->keyn, &curkeysav);

	lastdbyte = getcurdbyte(psp, psp->keyn);

	while (	((ret = nextkeysp(psp)) > -1)
			&& !(lastdbyte & 128)
			&& ((psp->dbyte & 127) > (lastdbyte & 127)) )
		lastdbyte = psp->dbyte;

	psp->key = psp->retkey;									/* set for caller */
	psp->keyorder = psp->wordorder;
#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
	putdelflag(psp, psp->keyn, DELOFF);					/* now clear */

	/* didnt find any next key OR key was incomplete */ 
	if (ret == INCOMPLETE)
	{
	   psp->data = td;					/* restore data portion */
	   return (psp->retcde);
	}

	/* if in wrong index, key not really found */
	if (	(ret != NODENOTINMEM) 
		&&	((ret == FAIL) || (getcurindx(psp, psp->keyn) != psp->keyn)))
	{
	   setnull(psp);		/* set notfound conditions */
	   psp->data = td;
		ret = FAIL;
	   return (psp->retcde = ret);
	}

	if (!ret)
		ret = cigetdatasp(psp, td, tl);	/* get rest of data */

	if (ret == NODENOTINMEM)		/* error reading bufs from memory */
	{
		cirestcurinfo(psp, psp->keyn, &curkeysav);	/* restore current key info */
		psp->dlen = tl;
		psp->data = td;
		psp->speedreq = psp->speedahead;		/* ask to read ahead */
#ifndef NO_WORD_CONVERSION
		cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
		ret = __cnext(psp);						/* do regular cnext */
		psp->speedreq = 0;
	}
	else
	{
		psp->data = td;
	}

   return(psp->retcde = ret);
} 		


/* gets next data records passing to application with callback */
int EDECL cnextrep(psp, keyn, data, dlen, nextproc)
register struct passparm *psp;
int keyn;
void *data;
int dlen;
NEXTPROCTYPE nextproc;
{
	int ret;
	int cc = CCOK;
	int curdbyte;
	int lastdbyte;
	int goflg;
	int knum;
	char *curptr;
	int curkeyn;
	char *curdataptr;
	int curdatalen;
	char *wrkdataptr;
	int wrkdatalen;
	unsigned int subentlen;

	/**/

	if (strtread(psp) != CCOK)					/* start reading operation */
		return(psp->retcde = FILELOCKED);

	psp->keyorder = INTELFMT;			/* default to key byte ordering is native */

	/* position to next key, but don't retrieve data */
	ret = cnext(psp, keyn, NULL, 0);
	if (ret != CCOK)
	{
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		return(ret);
	}

	/* position to physical location in index */
	curdbyte = getcurdbyte(psp, psp->keyn);
	ret = noderead(psp, &nodea, getndenum(psp, psp->keyn));	/* get leaf */
	if (ret != CCOK)
	{
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		return(psp->retcde = ret);				/* error */
	}

	knum = getkeynum(psp, psp->keyn) + 1;
	curptr = (char *)cigetkey(&nodea, getkeynum(psp, psp->keyn));
	if (!curptr)
	   curptr = (char *)next(psp, psp->keyn);		/* return key in next node */

	goflg = TRUE;
	while (goflg)
	{
		/* if in wrong index, or found nothing, next key failed */
		curkeyn = gtkeyn(curptr);
		if ((curkeyn != psp->keyn) || (cc == FAIL))
		{
		   putcurindx(psp, psp->keyn, (getcurindx(psp, psp->keyn) + 1));
		   setnull(psp);		/* set notfound conditions */
			cc = endread(psp);				/* release file after read */
			if (cc != CCOK)
				ret = cc;
			else
				ret = FAIL;
		   return (psp->retcde = ret);
		}

		/* do loop - extract key, rec#, data (if any) and move to next key */
		subentlen = *((unsigned char *)curptr);
		if (subentlen)
			blockmv(psp->retkey, gtkeyptr(curptr), subentlen);		/* return key */
		else
			cclear(psp->retkey, sizeof(psp->retkey), 0);				/* clear return key */
		psp->keyorder = psp->wordorder;
		psp->rec = gtkrec((ENTRY *)curptr, psp->wordorder);			/* return rec # */
		wrkdataptr = data;						/* set up where to put data */
		wrkdatalen = 0;
		cc = CCOK;
		do
		{
			/* extract sub-entry data piece, if desired */
			if (data && dlen)
			{
				curdatalen = goodata(psp, (ENTRY *)curptr);		/* length of sub-entry data */
				if (curdatalen > (dlen - wrkdatalen))
					curdatalen = dlen - wrkdatalen;		/* limit to space available */
				if (curdatalen)
				{
					curdataptr = (char *) (gtdataptr(psp, (ENTRY *)curptr));
					blockmv(wrkdataptr, curdataptr, curdatalen);
					wrkdataptr += curdatalen;
					wrkdatalen += curdatalen;
				}
			}

			/* move to next key */
			lastdbyte = curdbyte;
			if (knum >= nodea.count)			/* if at end */
			{
			   curptr = (char *)next(psp, psp->keyn);		/* return key in next node */

				if (!curptr)
					knum++;				/* point past end key */
				else
					knum = 1;

				if (!curptr || !*curptr)
				{
					goflg = FALSE;
					cc = FAIL;
					break;
				}

			}
			else
			{		
				knum++;
				curptr = moveup(curptr);
			}

			if (!*curptr)
				cc = FAIL;					/* past end of file */
			else
				curdbyte = gtdbyte(curptr);
		}
		while (	(cc > -1)
				&& !(lastdbyte & 128)
				&& ((curdbyte & 127) > (lastdbyte & 127)) );


		/* call callback proceedure with data */
		psp->key = psp->retkey;							/* set for caller */
		psp->keyorder = psp->wordorder;
		psp->keyn = keyn;
		if ((ret == CCOK) && (nextproc))
		{
#ifndef NO_WORD_CONVERSION
				if (psp->fliprep)		/* if byteflipping key value */
					cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
				goflg = (nextproc)(psp, keyn, data, wrkdatalen);
		}
		else
			break;
	}

	/* set current key info */
	putcurindx(psp, psp->keyn, curkeyn);
	putkeynum(psp, psp->keyn, (knum - 2));
   setcurkey(psp, (ENTRY *)curptr);

	cc = endread(psp);				/* release file after read */
	if (cc != CCOK)
		ret = cc;

   return(psp->retcde = ret);
} 		

#ifndef NO_CNEXTREP2
#ifdef ANSI
/* local prototypes */
ENTRY * FDECL repnext(struct passparm *psp, int keyn);
#endif

struct node noden;				/* work node for cnextrep2 */


/* used when at end of node, this returns pointer to first key in next node */
/* customized for use with cnextrep - uses noden */
ENTRY * FDECL repnext(psp, keyn)	
register struct passparm *psp;
register int keyn;		/* index number */ 
{
	int cc;

	/**/

	if (noden.forwrd == -1)		/* no more nodes */
	   return(NULL);			/* return null */

	putkeynum(psp, psp->keyn, 0);	/* first key of next node */
	psp->treeindex = -1;			/* invalidate tree path since moving right */

	while (noden.forwrd > -1) 	/* while nodes left ahead */
	{
	   putndenum(psp, keyn, noden.forwrd);	/* set node num to forward */
	   cc = noderead(psp, &noden, noden.forwrd);	/* read forward */
		if (cc != CCOK)
			return(NULL);				/* error */

	   if (noden.count)			/* if node has any keys */
	      break;				/* stop */
	}

	/* return ptr to first key (or blank area) */
	return((ENTRY *)noden.keystr);
}
		
/* gets next data records passing to application with callback */
int EDECL cnextrep2(psp, keyn, data, dlen, nextproc)
register struct passparm *psp;
int keyn;
void *data;
int dlen;
NEXTPROCTYPE nextproc;
{
	int ret;
	int cc = CCOK;
	int curdbyte;
	int lastdbyte;
	int goflg;
	int knum;
	char *curptr;				/* current pointer into noden */
	int curkeyn;				/* current index number */
	char *curdataptr;
	int curdatalen;
	char *wrkdataptr;
	int wrkdatalen;
	unsigned int subentlen;
	long updatnumsav;			/* updatnum before callback */
	int curkeynum;				/* keynum in current key info */

	/**/

	if (strtread(psp) != CCOK)					/* start reading operation */
		return(psp->retcde = FILELOCKED);

	psp->keyorder = INTELFMT;			/* default to key byte ordering is native */

	/* position to next key, but don't retrieve data */
	ret = cgetcur(psp, keyn, NULL, 0);
	if (ret == FAIL)
		ret = cfirst(psp, keyn, NULL, 0);

	if (ret != CCOK)
	{
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		return(ret);
	}

	/* position to physical location in index */
	curdbyte = getcurdbyte(psp, psp->keyn);
	ret = noderead(psp, &noden, getndenum(psp, psp->keyn));	/* get leaf */
	if (ret != CCOK)
	{
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		return(psp->retcde = ret);				/* error */
	}

	curkeynum = getkeynum(psp, keyn);
	knum = curkeynum + 1;
	curptr = (char *)cigetkey(&noden, curkeynum);
	if (!curptr)
	   curptr = (char *)repnext(psp, psp->keyn);		/* return key in next node */

	goflg = TRUE;
	while (goflg)
	{
		/* if in wrong index, or found nothing, next key failed */
		curkeyn = gtkeyn(curptr);
		if ((curkeyn != keyn) || (cc == FAIL))
		{
		   putcurindx(psp, keyn, curkeyn);
		   setnull(psp);		/* set notfound conditions */
			cc = endread(psp);				/* release file after read */
			if (cc != CCOK)
				ret = cc;
			else
				ret = FAIL;
		   return (psp->retcde = ret);
		}

		/* do loop - extract key, rec#, data (if any) and move to next key */
		subentlen = *((unsigned char *)curptr);
		if (subentlen)
			blockmv(psp->retkey, gtkeyptr(curptr), subentlen);		/* return key */
		else
			cclear(psp->retkey, sizeof(psp->retkey), 0);				/* clear return key */
		psp->keyorder = psp->wordorder;
		psp->rec = gtkrec((ENTRY *)curptr, psp->wordorder);			/* return rec # */
		wrkdataptr = data;						/* set up where to put data */
		wrkdatalen = 0;
		cc = CCOK;
		do
		{
			/* extract sub-entry data piece, if desired */
			if (data && dlen)
			{
				curdatalen = goodata(psp, (ENTRY *)curptr);		/* length of sub-entry data */
				if (curdatalen > (dlen - wrkdatalen))
					curdatalen = dlen - wrkdatalen;		/* limit to space available */
				if (curdatalen)
				{
					curdataptr = (char *) (gtdataptr(psp, (ENTRY *)curptr));
					blockmv(wrkdataptr, curdataptr, curdatalen);
					wrkdataptr += curdatalen;
					wrkdatalen += curdatalen;
				}
			}

			/* move to next key */
			lastdbyte = curdbyte;
			if (knum >= noden.count)			/* if at end */
			{
				/* return key in next node */
			   curptr = (char *)repnext(psp, keyn);

				if (!curptr)
					knum++;				/* point past end key */
				else
					knum = 1;

				if (!curptr || !*curptr)
				{
					goflg = FALSE;
					cc = FAIL;
					break;
				}

			}
			else
			{		
				knum++;
				curptr = moveup(curptr);
			}

			if (!*curptr)
				cc = FAIL;					/* past end of file */
			else
				curdbyte = gtdbyte(curptr);
		}
		while (	(cc > -1)
				&& !(lastdbyte & 128)
				&& ((curdbyte & 127) > (lastdbyte & 127)) );


		/* call callback proceedure with data */
		psp->key = psp->retkey;							/* set for caller */
		psp->keyorder = psp->wordorder;
		psp->keyn = keyn;
		if ((ret == CCOK) && (nextproc))
		{
			updatnumsav = psp->pupdatnum;	/* save update number */

#ifndef NO_WORD_CONVERSION
			if (psp->fliprep)		/* if byteflipping key value */
				cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

			goflg = (nextproc)(psp, keyn, data, wrkdatalen);

			/* if file has been changed by callback function, reset position */
			if ((updatnumsav != psp->pupdatnum) && goflg)
			{
				/* re-position to next key, but don't retrieve data */
				psp->keyn = keyn;
				psp->key = (void *)gtkeyptr(curptr);
				psp->keyorder = psp->wordorder;
				psp->rec = gtkrec((ENTRY *)curptr, psp->wordorder);
				psp->data = NULL;
				psp->dlen = 0;
				ret = _cfind(psp);
				if (ret >= CCOK)
				{
					/* position to physical location in index */
					curdbyte = getcurdbyte(psp, keyn);
					ret = noderead(psp, &noden, getndenum(psp, keyn));	/* get leaf */
				}

				if (ret != CCOK)
				{
					cc = endread(psp);				/* release file after read */
					if (cc != CCOK)
						ret = cc;
					return(psp->retcde = ret);				/* error */
				}
			
				curkeynum = getkeynum(psp, keyn);
				knum = curkeynum + 1;
				curptr = (char *)cigetkey(&noden, curkeynum);
				if (!curptr)
				   curptr = (char *)repnext(psp, keyn);		/* return key in next node */
			}
		}
		else
			break;
	}

	/* set current key info */
	psp->keyn = keyn;
	putcurindx(psp, keyn, curkeyn);
	putkeynum(psp, keyn, (knum - 2));
   setcurkey(psp, (ENTRY *)curptr);

	cc = endread(psp);				/* release file after read */
	if (cc != CCOK)
		ret = cc;

   return(psp->retcde = ret);
} 		


#endif	/* !NO_CNEXTREP2 */

#endif

#ifdef CPREV			

int EDECL cprev(psp, keyn, data, dlen)	/* high level prev */
struct passparm *psp;
int keyn;
void *data;
int dlen;
{
	int ret;

	/* set for lower level routines */
	psp->keyn = keyn;
	psp->key  = 0;
	psp->data = data;
	psp->dlen = dlen;
	psp->keyorder = INTELFMT;			/* default key byte ordering is native */

	ret = _cprev(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}


int FDECL _cprev(psp)	/* gets previous data record */
register struct passparm *psp;
{
	int ret;

	/* if speed reading is turned on and current key not locked */
	if (psp->speedread && (psp->curlock[psp->keyn] == UNLOCKED))
		ret = __cprevsp(psp);			/* do speed read cprev */
	else
		ret = __cprev(psp);				/* else do regular cprev */

	return(ret);
}


int FDECL __cprev(psp)		/* gets prev data record */
register struct passparm *psp;
{
	void *td;
	int tl;
	int dataret;
	int ret;
	int cc;

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	if (strtread(psp) != CCOK)					/* start reading operation */
		return(psp->retcde = FILELOCKED);

   cc = _mclrlck(psp, psp->keyn);	/* always unlock current for any operation */
	if (cc != CCOK)
	{
		psp->alterr = endread(psp);	/* release file */
		return(psp->retcde = cc);		/* return error from _mclrlck */
	}

	/* save for caller */
	td = psp->data;
	tl = psp->dlen;
	psp->errcde = 0;	/* start out good */

	/* if need to postion */
	if ((psp->processmode != EXCL) && (getupdkeyinfo(psp, psp->keyn)))
	{
		psp->data = NULL; /* added save and restore ***5/26/88*** */
		psp->dlen = 0;
	   _cgetcur(psp);		/* establish current pointer */
		psp->data = td;
		psp->dlen = tl;
		putupdkeyinfo(psp, psp->keyn, FALSE);	/* EXCL key info is current */
	}

	if (getcurindx(psp, psp->keyn) < psp->keyn) /* if before beginning of index */
	{			/* then set not found conditions */
	   setnull(psp);
		putcurkey(psp, psp->keyn, NULL);			/* clear current key to null */
		putcurrcval(psp, psp->keyn, -1L);		/* (before first record) */
	   psp->data = td;
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		else
			ret = FAIL;
	   return(psp->retcde = ret);
	}

	while (prevkey(psp) > -1 && psp->dbyte & 127)
	      /* looking for first data portion */
	    ;

	if (psp->retcde == FAIL) 	/* didnt find prev key */
	{
	   setnull(psp);
		putcurkey(psp, psp->keyn, NULL);			/* clear current key to null */
		putcurrcval(psp, psp->keyn, -1L);		/* (before first record) */

	   psp->data = td;			/* restore */
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		else
			ret = psp->retcde;
	   return (psp->retcde = ret);
	}

	/* key not really found */
	if (getcurindx(psp, psp->keyn) != psp->keyn) /* in wrong index */
	{			/* set not found conditions */
	   setnull(psp);
		putcurkey(psp, psp->keyn, NULL);			/* clear current key to null */
		putcurrcval(psp, psp->keyn, -1L);		/* (before first record) */

	   psp->data = td;
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		else
			ret = FAIL;
	   return (psp->retcde = ret);
	}

	dataret = cigetdata(psp, td, tl);		/* get rest of data */
	psp->data = td;			/* restore */
	if (dataret == CCOK)		/* if good data find */
	   ret = CCOK;
	else
	   ret = dataret;			/* incomplete */

	cc = endread(psp);				/* release file after read */
	if (cc != CCOK)
		ret = cc;

	return(psp->retcde = ret);
} 		

/* gets prev data record - speed read method */
int FDECL __cprevsp(psp)
register struct passparm *psp;
{
	void *td;
	int tl;
	int ret;
	CURINFO curkeysav;					/* space to save current key info */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	/* save for caller */
	tl = psp->dlen;
	td = psp->data;
	psp->errcde = 0;	/* start out good */

#ifdef REMOVED
	/* not yet intitialized or info is out of date */
	if (	(getcurindx(psp, psp->keyn) == -1)
		||	((psp->processmode != EXCL) && (getupdkeyinfo(psp, psp->keyn))))
#endif
	/* not current key info yet intitialized */
	if (getcurindx(psp, psp->keyn) == -1)
	{
		psp->speedreq = -psp->speedahead;		/* ask to read ahead */
		ret = __cprev(psp);						/* do regular cprev */
		psp->speedreq = 0;
		return(ret);
	}

	if (getcurindx(psp, psp->keyn) < psp->keyn) /* if before key area */
	{	
	   setnull(psp);		/* no returned parms */
		putcurkey(psp, psp->keyn, NULL);			/* clear current key to null */
		putcurrcval(psp, psp->keyn, -1L);		/* (before first record) */
	   psp->data = td;		/* restore data pointer */
		ret = FAIL;
	   return(psp->retcde = ret);
	}

	/* save current key info in case nextkeysp or cigetdatasp fails */
	cisavecurinfo(psp, psp->keyn, &curkeysav);

	while (	((ret = prevkeysp(psp)) > -1)
			&& (psp->dbyte & 127))
	      /* looking for first data portion */
		;

	psp->key = psp->retkey;									/* set for caller */
	psp->keyorder = psp->wordorder;
#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
	putdelflag(psp, psp->keyn, DELOFF);					/* now clear */

	/* didnt find any next key OR key was incomplete */ 
	if (ret == INCOMPLETE)
	{
	   psp->data = td;					/* restore data portion */
	   return (psp->retcde);
	}

	/* if in wrong index, key not really found */
	if (	(ret != NODENOTINMEM) 
		&&	((ret == FAIL) || (getcurindx(psp, psp->keyn) != psp->keyn)))
	{
	   setnull(psp);		/* set notfound conditions */
		putcurkey(psp, psp->keyn, NULL);			/* clear current key to null */
		putcurrcval(psp, psp->keyn, -1L);		/* (before first record) */
	   psp->data = td;
		ret = FAIL;
	   return (psp->retcde = ret);
	}

	if (!ret)
		ret = cigetdatasp(psp, td, tl);	/* get rest of data */

	if (ret == NODENOTINMEM)		/* error reading bufs from memory */
	{
		cirestcurinfo(psp, psp->keyn, &curkeysav);	/* restore current key info */
		psp->dlen = tl;
		psp->data = td;
		psp->speedreq = -psp->speedahead;		/* ask to read ahead */
		ret = __cprev(psp);						/* do regular cprev */
		psp->speedreq = 0;
	}
	else
	{
		psp->data = td;
	}

   return(psp->retcde = ret);
} 		


#endif

#ifdef CFIRST			

int EDECL cfirst(psp, keyn, data, dlen)	/* high level first */
struct passparm *psp;
int keyn;
void *data;
int dlen;
{
	int ret;

	/* set for lower level */
	psp->keyn = keyn;
	psp->data = data;
	psp->dlen = dlen;
	psp->keyorder = INTELFMT;			/* key byte ordering is native */

	ret = _cfirst(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}


int FDECL _cfirst(psp)		/* get first key */
struct passparm *psp;
{
	int ret;
	char buf[MAXKLEN];

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	cclear(buf, MAXKLEN-1, 0);			/* clear key to ignored value */
	psp->key = buf;						/* point to our key (ignored because of FIRSTKEY) */
	psp->rec = 0L;							/* 0th record # for first key */
	psp->srchorder = FIRSTKEY;			/* force all key tests to beg on index */
	ret = _cfind(psp);					/* find entry */
	psp->srchorder = 0;					/* restore normal key tests */
	return((ret >= 0) ? CCOK : psp->retcde);
}

#endif

#ifdef CLAST

int EDECL clast(psp, keyn, data, dlen)	/* high level last */
struct passparm *psp;
int keyn;
void *data;
int dlen;
{
	int ret;

	/* set for lower level */
	psp->keyn = keyn;
	psp->data = data;
	psp->dlen = dlen;
	psp->keyorder = INTELFMT;			/* default key byte ordering is native */

	ret = _clast(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}


int FDECL _clast(psp)		/* get last key in sub index */
struct passparm *psp;
{
	char buf[MAXKLEN];
	void *td;
	int tl;		/* temporary data length */
	int ret;
	int cc;
	int ti;		/* temp index number */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	if (strtread(psp) != CCOK)					/* start reading operation */
		return(psp->retcde = FILELOCKED);

   cc = _mclrlck(psp, psp->keyn);	/* always unlock current for any operation */
	if (cc != CCOK)
	{
		psp->alterr = endread(psp);	/* release file */
		return(psp->retcde = cc);		/* return error from _mclrlck */
	}

	/* save for caller */
	td = psp->data;
	tl = psp->dlen;
	ti = psp->keyn;

	cclear(buf, MAXKLEN-1, 0);			/* clear key to ignored value */
	psp->key = buf;						/* point to our key */
	psp->srchorder = LASTKEY;			/* force all key tests to past end */
	psp->errcde = 0;						/* start out good */
	ret = findkey(psp);					/* find next higher key */
	psp->srchorder = 0;					/* restore normal key tests */
	psp->data = td;						/* restore saved data */
	psp->dlen = tl;
	psp->keyn = ti;

	if (ret >= FAIL)	/* if no serious error, 
								get last key in prev(correct) index */
		ret = (_cprev(psp) >= 0) ? CCOK : psp->retcde;

	cc = endread(psp);				/* release file after read */
	if (cc != CCOK)
		ret = cc;

	return(ret);

}

#endif

/*	current functions */

#ifdef CGETCUR

int EDECL cgetcur(psp, keyn, data, dlen)	/* return current key and data */
struct passparm *psp;
int keyn;
void *data;
int dlen;
{
	int ret;

	/* set for lower level */
	psp->keyn = keyn;
	psp->key  = 0;
	psp->data = data;
	psp->dlen = dlen;
	psp->keyorder = INTELFMT;			/* default key byte ordering is native */

	ret = _cgetcur(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}	


/* get data rec - put into data */
int FDECL _cgetcur(psp)
register struct passparm *psp;
{
	int ret;

	if (psp->speedread)		/* if speed reading is turned on */
		ret = __cgetcursp(psp);			/* do speed read cgetcur */
	else
		ret = __cgetcur(psp);				/* else do regular cgetcur */

	return(ret);
}

/* get data rec - put into data - normal method */
int FDECL __cgetcur(psp)
register struct passparm *psp;
{
	void *td;	/* temp data area */
	int tl;		/* temporary data length */
	int dataret;	/* from cigetdata() */
	int ret;
	int cc;
   char tempkey[MAXMKLEN];			/* temp current key val */
	long temprec;						/* temp current rec num */
 	int tempflag;						/* temp recurs flag */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	/* save for caller */
	td = psp->data;
	tl = psp->dlen;
	psp->errcde = 0;	/* start out good */

	if (strtread(psp) != CCOK)					/* start reading operation */
		return(psp->retcde = FILELOCKED);

#ifdef REMOVED
	/* if not right index or just deleted, no cur */	
	if (psp->curindx[psp->keyn] != psp->keyn 
		|| psp->delflag[psp->keyn] == DELON)
#endif
	/* if not right index or just deleted, no cur */	
	if (getcurindx(psp, psp->keyn) != psp->keyn 
		|| getdelflag(psp, psp->keyn) == DELON)
	{			/* set not found conditions */
	   setnull(psp);
	   psp->data = td;
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		else
			ret = FAIL;
	   return(psp->retcde = ret);
	}

	/* if need to position */
#ifdef REMOVED
	if ((psp->processmode != EXCL) && (psp->updkeyinfo[psp->keyn]))
#endif
	if ((psp->processmode != EXCL) && (getupdkeyinfo(psp, psp->keyn)))
	{
		getcurkey(psp, psp->keyn, tempkey);
		temprec = getcurrcval(psp, psp->keyn);

		tempflag = psp->srecurs;				/* save recursion state */
	 	psp->srecurs = TRUE;						/* prevent clearing current */
		psp->key = tempkey;
		psp->keyorder = psp->wordorder;
		psp->rec = temprec;
      ret = _cfind(psp);

		psp->srecurs = tempflag;

		if (ret > CCOK)
		{
			/* if GREATER or KEYMATCH, no current key, restore ptrs */
			putcurkey(psp, psp->keyn, tempkey);				/* restore key */
			putcurrcval(psp, psp->keyn, temprec);
			ret = psp->retcde = NOCUR;
		}

		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;

#ifdef REMOVED
		psp->updkeyinfo[psp->keyn] = FALSE;		/* EXCL key info is current */
#endif
		putupdkeyinfo(psp, psp->keyn, FALSE);	/* EXCL key info is current */
		return(ret);
	}

	if (cicurkey(psp) == FAIL) /* no current, done */
	{
	   psp->data = td;	/* restore */
		cc = endread(psp);				/* release file after read */
		if (cc != CCOK)
			ret = cc;
		else
			ret = FAIL;
	   return(psp->retcde = ret);
	}

	dataret = cigetdata(psp, td, tl);	/* get rest of data */
	psp->data = td;			/* restore data pointer */
#ifdef REMOVED
	psp->key++;   /* point to actual key (get past key number) */
#endif

	cc = endread(psp);				/* release file after read */
	if (cc != CCOK)
		ret = cc;
	else
		ret = dataret;
   return(psp->retcde = ret);		/* return any errors from read */
}

/* get data rec - put into data - speed read method */
int FDECL __cgetcursp(psp)
register struct passparm *psp;
{
	void *td;	/* temp data area */
	int tl;		/* temporary data length */
	int ret;
	CURINFO curkeysav;					/* space to save current key info */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	/* save for caller */
	td = psp->data;
	tl = psp->dlen;
	psp->errcde = 0;	/* start out good */
	psp->keyorder = INTELFMT;

	/* if not right index or just deleted, no cur */	
	if (getcurindx(psp, psp->keyn) != psp->keyn 
		|| getdelflag(psp, psp->keyn) == DELON)
	{			/* set not found conditions */
	   setnull(psp);
	   psp->data = td;
	   return(psp->retcde = FAIL);
	}

	/* if need to position */
	if ((psp->processmode != EXCL) && (getupdkeyinfo(psp, psp->keyn)))
	{
		psp->speedreq = 0;						/* no read ahead */
		ret = __cgetcur(psp);					/* do regular cgetcur */
		return(ret);
	}

	/* save current key info in case nextkeysp or cigetdatasp fails */
	cisavecurinfo(psp, psp->keyn, &curkeysav);

	ret = cicurkeysp(psp);
	if (ret == NODENOTINMEM)		/* error reading bufs from memory */
	{
		psp->speedreq = 0;						/* no read ahead */
		cirestcurinfo(psp, psp->keyn, &curkeysav);	/* restore current key info */
		ret = __cgetcur(psp);					/* do regular cgetcur */
		return(ret);
	}

	if (ret == FAIL) /* no current, done */
	{
	   psp->data = td;	/* restore */
	   return(psp->retcde = ret);
	}

	ret = cigetdatasp(psp, td, tl);	/* get rest of data */
	if (ret == NODENOTINMEM)		/* error reading bufs from memory */
	{
		psp->speedreq = 0;						/* no read ahead */
		psp->data = td;
		psp->dlen = tl;
		cirestcurinfo(psp, psp->keyn, &curkeysav);	/* restore current key info */
		ret = __cgetcur(psp);					/* do regular cgetcur */
		return(ret);
	}

	psp->data = td;			/* restore data pointer */
   return(psp->retcde = ret);		/* return any errors from read */
}

#endif

#ifdef CDELCUR

int EDECL cdelcur(psp, keyn)	/* delete current key */
struct passparm *psp;
int keyn;
{
	int ret;

	/* set for lower level routines */
	psp->keyn = keyn;
	psp->key  = 0;
	psp->keyorder = INTELFMT;			/* default key byte ordering is native */

	ret = _cdelcur(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}


int FDECL _cdelcur(psp)		/* delete current key */
struct passparm *psp;
{
	int ret;
	int cc;
   char tempkey[MAXMKLEN];			/* temp current key val */
	long temprec;						/* temp current rec num */
	int tempflag;						/* temp recursion flag */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	/* record must be self-locked before deleting */
	if (		(psp->curlock[psp->keyn] != LOCKED)
			&& (psp->processmode != EXCL)
			&& (psp->keyn != SEMAINDEX)
			&& (psp->keyn != TRANSINDX)
			&& (psp->keyn != DELINDX)
			&& (psp->nolock == FALSE))
	{
	   return(psp->retcde = RECNOLK);
	}

#ifdef REMOVED
	if (psp->curindx[psp->keyn] != psp->keyn)  /* if not right index */	
	   return(psp->retcde = FAIL);
#endif
	if (getcurindx(psp, psp->keyn) != psp->keyn)  /* if not right index */	
	   return(psp->retcde = FAIL);

	if ((cc = strtwrit(psp)) != CCOK)
	   return(psp->retcde = cc);

	cc = _mclrlck(psp, psp->keyn);	/* always unlock current for any operation */
	if (cc != CCOK)
	{
		psp->alterr = endwrit(psp);	/* release file */
		return(psp->retcde = cc);		/* return error from _mclrlck */
	}

	/* need to use equiv of cdelete for shared mode or invalid tree */
	if (((psp->processmode != EXCL)
			&& (psp->keyn != SEMAINDEX)
			&& (getupdkeyinfo(psp, psp->keyn)))
		|| (psp->treeindex != psp->keyn))
	{
		getcurkey(psp, psp->keyn, tempkey);
		temprec = getcurrcval(psp, psp->keyn);
		tempflag = psp->srecurs;				/* save recursion flag */
	 	psp->srecurs = TRUE;						/* prevent clearing current again */
		psp->key = tempkey;
		psp->keyorder = psp->wordorder;
		psp->rec = temprec;
	   ret = __cdlt(psp);
		psp->srecurs = tempflag;				/* restore recursion flag */
		putupdkeyinfo(psp, psp->keyn, FALSE);	/* EXCL key info is current */
	}
	else
	{
		ret = delcur(psp);	/* do delete in EXCL mode */
	}

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);
}
		
#endif

#ifdef CCHGCUR

int EDECL cchgcur(psp, keyn, data, dlen)	
/* change data record at current location */
struct passparm *psp;
int keyn;
void *data;
int dlen;
{
	int ret;

	/* set for lower level */
	psp->keyn = keyn;
	psp->data = data;
	psp->dlen = dlen;
	psp->keyorder = INTELFMT;			/* default key byte ordering is native */

	ret = _cchgcur(psp);

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

	return(ret);
}


int FDECL _cchgcur(psp)	
struct passparm *psp;
{
	void *td;	/* for data pointer */
	int tl;		/* for data length */
	KEYPTR tk;	/* for key pointer */
	INT32 tm;	/* key byte order mode */
	int ret;
	int cc;
   char tempkey[MAXMKLEN];
	int tempflag;					/* temp recursion flag */

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if (!goodindex(psp->keyn))
	   return(psp->retcde = PARMERR); 

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	if (getcurindx(psp, psp->keyn) != psp->keyn)  /* if not right index */	
	{
		cc = endwrit(psp);
		if (cc != CCOK)
			ret = cc;
		else
			ret = FAIL;
	   return(psp->retcde = ret);
	}

	/* record must be self-locked before deleting */
	if (	(psp->curlock[psp->keyn] != LOCKED)
		&& (psp->processmode != EXCL)
		&& (psp->nolock == FALSE))
	{
		ret = RECNOLK;
		cc = endwrit(psp);
		if (cc != CCOK)
			ret = cc;
	   return(psp->retcde = ret);
	}

	/* save for later */
	td = psp->data;
	tl = psp->dlen;
	tk = psp->key;
	tm = psp->keyorder;

	if (psp->processmode == EXCL)
	{
		if ((ret = cicurkey(psp)) == FAIL)	/* if no current */
		{
			if (psp->key != tk)
			{
			   psp->key = tk;		/* restore for caller */
				psp->keyorder = tm;
			}
#ifndef NO_WORD_CONVERSION
			else
			{
				cipspflip(psp, INTELFMT);
			}
#endif	/* !NO_WORD_CONVERSION */
			cc = endwrit(psp);
			if (cc != CCOK)
				ret = cc;
			else
				ret = FAIL;
		   return(psp->retcde = ret);		/* end */
		}

		psp->data = td;		/* restore data ptr */
		psp->dlen = tl;		/* restore data len */
	}
	else		/* shared, use current information in psp to locate */
	{
		getcurkey(psp, psp->keyn, tempkey);
		psp->key = tempkey;
		psp->keyorder = psp->wordorder;
		psp->rec = getcurrcval(psp, psp->keyn);
	}

	/* key and rec already set */
	tempflag = psp->srecurs;			/* save recursion state */
	psp->srecurs = TRUE;					/* prevent clearing current */
	ret = ___cadd(psp, CHGCUR);		/* change the record */
	psp->srecurs = tempflag;

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);
}

#endif

#ifdef CNEXTREC

int EDECL cnextrec(psp)	/* gets and increments next data record */
register struct passparm *psp;
{
	int ret;
	int cc;

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	psp->rec = psp->nxtdrec++;	/* set and increment */

	if ((psp->seclev >= HDRUPLEV)
		 && (psp->seclev < 4))
	{
	   ret = hdrupdte(psp);	/* keep header current */
		if (ret != CCOK)
		   psp->updtflag = DIRTY;	/* error writing header, still dirty */
	}
	else
	{
		ret = CCOK;
	   psp->updtflag = DIRTY;	/* else still dirty */
	}

	cc = endwrit(psp);				/* release file after read */
	if (cc != CCOK)
		ret = cc;

	return(ret);
}

#endif

#ifdef CSETREC

int EDECL csetrec(psp, recnum)	/* sets rec num */
register struct passparm *psp; 
long recnum;
{
	int ret;

	/**/

	psp->rec = recnum;
	ret = _csetrec(psp);

	return(ret);
}

	
int FDECL _csetrec(psp)	/* set record number */
struct passparm *psp;
{
	int ret;
	int cc;

	/**/

	ret = CCOK;

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	psp->nxtdrec = psp->rec;	/* set next rec */
	if ((psp->seclev >= HDRUPLEV)
		 && (psp->seclev < 4))
	   ret = hdrupdte(psp);	/* keep header current */
	else
	   psp->updtflag = DIRTY;	/* else still dirty */

   cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);		/* else good */
}

#endif

#ifdef HDRCNT
/* get count of number of records in file from header */
int EDECL cgetcnt(psp)
register struct passparm *psp;
{
	int ret;

	if (checkpsp(psp) != PSPOPEN)		/* not an open psp */
	   return(PSPERR);

	if (strtread(psp) != CCOK)
	   return(FILELOCKED);

	psp->rec = psp->reccnt;					/* return record count */

	ret = endread(psp);
	return(ret);
}

/* set record count in header */
int EDECL csetcnt(psp, newcnt)
register struct passparm *psp; 
long newcnt;
{
	int ret;
	int cc;

	/**/

	ret = CCOK;

	if (checkpsp(psp) != PSPOPEN)		/* not an open psp */
	   return(PSPERR);

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	psp->reccnt = newcnt;				/* set new record count */
	if ((psp->seclev >= HDRUPLEV)
		 && (psp->seclev < 4))
	   ret = hdrupdte(psp);				/* keep header current */
	else
	   psp->updtflag = DIRTY;			/* else still dirty */

	cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);							/* else good */
}
#endif	/* HDRCNT */



#ifdef HDRCUST

/* get custom info from header */
int EDECL cgetcust(psp, data, datalen)
register struct passparm *psp;
void *data;									/* data to get from header */
int datalen;								/* length of data */
{
	int movlen;
	int ret;
	int cc;

	if (checkpsp(psp) != PSPOPEN)		/* not an open psp */
	   return(PSPERR);

	if (strtread(psp) != CCOK)
	   return(FILELOCKED);

	if (datalen < psp->custlen)		/* length of data to move */
	{
		movlen = datalen;
		ret = BUFOVER;
	}
	else
	{
		movlen = psp->custlen;
		ret = CCOK;
	}

	psp->dlen = psp->custlen;					/* actual length */
	psp->retlen = movlen;						/* returned length */
	blockmv(data, psp->custinfo, movlen);	/* return data */

	cc = endread(psp);							/* unlock file */
	if (cc != CCOK)
		ret = cc;
	return(ret);
}

/* set custom information in header */
int EDECL csetcust(psp, data, datalen)
register struct passparm *psp; 
void *data;									/* data to save in header */
int datalen;								/* length of data */
{
	int ret;
	int cc;

	/**/

	ret = CCOK;

	if (checkpsp(psp) != PSPOPEN)		/* not an open psp */
	   return(PSPERR);

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	if (datalen > MAXCUSTINFO)			/* exceeds max length */
		return(PARMERR);

	blockmv(psp->custinfo, data, datalen);		/* move in data */
	psp->custlen = datalen;

	if ((psp->seclev >= HDRUPLEV)
		 && (psp->seclev < 4))
	   ret = hdrupdte(psp);				/* keep header current */
	else
	   psp->updtflag = DIRTY;			/* else still dirty */

	cc = endwrit(psp);					/* unlock file */
	if (cc != CCOK)
		ret = cc;

	return(ret);							/* else good */
}
#endif	/* HDRCUST */



/* ----------------------------------------------------- */

