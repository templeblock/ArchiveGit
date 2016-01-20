/*   entproc.c  -   node entry processing routines
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




#ifdef REQUIRES_WORD_ALIGNMENT
/* get record number after key at ptr */
long FDECL fgtkrec(ptr, wordorder)
ENTRY *ptr;		/* pointer to key */
INT32 wordorder;			/* byte ordering of words */
{
	long p;									/* to hold value */
	register char *bptr;					/* byte level pointer */

	bptr = (char *)ptr;
	bptr += SZELEN + SZENDX + SZEDBYTE;		/* ptr into structure */
	p = 0;											/* avoids the compiler error msg */
	trnsrec(&p, bptr);							/* put into long value */
	return(inteltol(p, wordorder));			/* return record number */
}
#endif	/* REQUIRES_WORD_ALIGNMENT for gtkrec */


#ifndef MACRO_FUNCS
/* move up to next entry in node */
char * FDECL fmoveup(p)
char *p;
{
	return(p + ((ENTRY *)p)->entlen + 1);
}


/* get the length of an entry */
int FDECL fgtentlen(ptr)
ENTRY *ptr;
{
	return((ptr) ? ptr->entlen : 0);
}

int FDECL fgtdbyte(ptr)	/* gets data byte after key at ptr */
ENTRY *ptr;	/* pointer to key */
{
	int dbyte;
	register unsigned char *bptr;					/* byte level pointer */

	bptr = (unsigned char *)ptr;
	bptr += SZELEN + SZENDX;	/* pnt to the data byte in node entry struct */
	dbyte = (int)(*bptr);
	dbyte &= 0xFF;					/* make sure char is unsigned */
	return(dbyte);					/* return data byte */
}

/* return index number of entry */
int FDECL fgtkeyn(ptr)
ENTRY *ptr;						/* ptr to entry */
{
	unsigned char *bptr;			/* byte pointer */
	int ret;

	if ((ptr) && ptr->entlen)
	{
		bptr = (unsigned char *)ptr;
		bptr += SZELEN;			/* pnt to the index num in node entry struct */
		ret = *bptr;
		ret &= 0xFF;				/* make sure char is unsigned */
	}
	else
		ret = 0;						/* no entry, no keyn */

	return(ret);
}


/* return a pointer to the key/data area in a node entry */
char * FDECL fgtkeyptr(ptr)
ENTRY *ptr;
{
	register char *bptr;

	bptr = (char *)ptr;
	if (ptr->entlen)
		bptr += SZELEN + SZENDX + SZEDBYTE + SZEREC;	/* bump past entry header */

	return(bptr);
}



#endif		/* not MACRO_FUNCS */

/* get pointer to data part of sub-entry */
char * FDECL gtdataptr(psp, ptr)
struct passparm *psp;
ENTRY *ptr;
{
	register char *bptr;

	bptr = (char *)gtkeyptr(ptr);						/* ptr to key */
	if (ptr->entlen)
	{
		bptr += cikeylen(psp, gtkeyn(ptr), bptr);	/* bump past key to the data */
	}

	return(bptr);
}

/* get length of key based on index number */
int FDECL cikeylen(psp, keyn, keyptr)
struct passparm *psp;				/* file info needed to tell type of index */
int keyn;								/* index number being searched */
KEYPTR keyptr;
{
	return(_cikeylen(psp->indtyp[keyn], keyptr));
}

/* get length of key based on key type */
int FDECL _cikeylen(keytyp, keyptr)
int keytyp;								/* type of key pointed to */
KEYPTR keyptr;							/* ptr to key value */
{
	int retlen = 0;							/* return length of key */

	if (!keyptr)						/* if null key, return 0 length */
		return(0);

	switch(keytyp & INDTYPEMASK)
	{
		case STRINGIND:
		case NOCASEIND:
			retlen = (int) strlen((char *)keyptr) + 1;	/* ASCIIZ string key */
			break;

		case BINARYIND:											/* binary key */
			retlen = (int)(*((unsigned char *)keyptr) + 1);	/* 1st byte is len */
			break;

		case FIXIND:										/* fixed length binary key */
			retlen = FIXINDLEN;							/* FIXINDLEN length */
			break;

		case INTIND:
			retlen = sizeof(int);			/* integer key */
			break;

		case SHORTIND:
		case USHORTIND:
			retlen = sizeof(short);			/* short key */
			break;

		case LONGIND:
			retlen = sizeof(long);			/* long key */
			break;

		case FLOATIND:
			retlen = sizeof(float);			/* float key */
			break;

		case DOUBLEIND:
			retlen = sizeof(double);		/* double key */
			break;

		case SEGIND:												/* segmented index */
			retlen = (int)(*((unsigned char *)keyptr) + 1);	/* 1st byte is length */
			break;

		case CUST1IND:												/* custom index type 1*/
		case CUST2IND:												/* custom index type 2*/
		case CUST3IND:												/* custom index type 3*/
		case CUST4IND:												/* custom index type 4*/
		case CUST5IND:												/* custom index type 5*/
			retlen = (int)(*((unsigned char *)keyptr) + 1);	/* 1st byte is length */
			break;
	}

	return(retlen);
}

/* copy key from one location to another */
void FDECL cikeycpy(psp, keyn, dstptr, keyptr)
struct passparm *psp;				/* file info needed to tell type of index */
int keyn;								/* index number being searched */
KEYPTR dstptr;							/* where to move key to */
KEYPTR keyptr;							/* where to move key from */
{
	int keylen;							/* length of key */
	union									/* union of all null key values */
	{
		char bkey[1];					/* string and binary null key */
		int  ikey;						/* integer null key */
		short shrtkey;					/* short null key */
		long lngkey;					/* long null key */
		float fltkey;					/* float null key */
		double dblkey;					/* double null key */
	} nullkey;

	if (!keyptr)		/* create null/zero key value */
	{
		cclear(&nullkey, sizeof(nullkey), 0);		/* create null key */
		keyptr = (KEYPTR)&nullkey;
	}

	switch(psp->indtyp[keyn] & INDTYPEMASK)
	{
		case STRINGIND:
			strcpy(dstptr, keyptr);		/* ASCIIZ string key */
			break;

		case NOCASEIND:
			strcpy(dstptr, keyptr);		/* ASCIIZ string key */
			break;

		case BINARYIND:											/* binary key */
			keylen = (int)(*((unsigned char *)keyptr) + 1);	/* 1st byte is length */
			blockmv(dstptr, keyptr, keylen);
			break;

		case FIXIND:											/* fixed binary key */
			blockmv(dstptr, keyptr, FIXINDLEN);
			break;

		case INTIND:
			trnsint(dstptr, keyptr);
			break;

		case SHORTIND:
		case USHORTIND:
			trnsshort(dstptr, keyptr);
			break;

		case LONGIND:
			trnslong(dstptr, keyptr);
			break;

		case FLOATIND:
			trnsfloat(dstptr, keyptr);
			break;

		case DOUBLEIND:
			trnsdbl(dstptr, keyptr);
			break;

		case SEGIND:												/* segmented index */
			keylen = (int)(*((unsigned char *)keyptr) + 1);	/* 1st byte is length */
			blockmv(dstptr, keyptr, keylen);
			break;

		case CUST1IND:												/* custom index type 1*/
		case CUST2IND:												/* custom index type 2*/
		case CUST3IND:												/* custom index type 3*/
		case CUST4IND:												/* custom index type 4*/
		case CUST5IND:												/* custom index type 5*/
			keylen = (int)(*((unsigned char *)keyptr) + 1);	/* 1st byte is length */
			blockmv(dstptr, keyptr, keylen);
			break;
	}
}

/* increment value of key for next processing */
void FDECL incrkeyval(psp, keyn, keytyp, keyptr)
struct passparm *psp;				/* file info needed to tell type of index */
int keyn;								/* index number being searched */
int keytyp;								/* index key type being searched */
KEYPTR keyptr;							/* key buffer */
{
	unsigned char keylen;			/* length of key */
   static char end[] = { 1, 0 };	/* for concatenation to string */
	int intval;							/* temp numeric values */
	short shortval;
	unsigned short ushortval;
	long longval;
	float floatval;
	double doubleval;
	int i;
	KEYSEGLIST *seginfo;					/* segment key info from index list */
	unsigned char *wrkptr;

#ifndef NO_WORD_CONVERSION
	cikeyflip(psp, keyn, keyptr, psp->indtyp[keyn] & INDTYPEMASK, psp->wordorder, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
	switch(keytyp & INDTYPEMASK)
	{
		case STRINGIND:
         strcat(keyptr, end);				/* one bigger now */
			break;

		case NOCASEIND:
         strcat(keyptr, end);				/* one bigger now */
			break;

		case BINARYIND:											/* binary key */
			keylen = (*((unsigned char *)keyptr));			/* 1st byte is length */
			*((char *)keyptr + keylen + 1) = 0;					/* clear end byte */
			*((unsigned char *)keyptr) = (unsigned char)++keylen;	/* incr length */
			break;

		case INTIND:
			trnsint(&intval, keyptr);
			if (intval < (intval + 1))					/* incr value if not at max */
			{
				intval++;
				trnsint(keyptr, &intval);
			}
			break;

		case SHORTIND:
			trnsshort(&shortval, keyptr);
			if (shortval + 1 > shortval)				/* incr value if not at max */
			{
				shortval++;
				trnsshort(keyptr, &shortval);
			}
			break;

		case USHORTIND:
			trnsshort(&ushortval, keyptr);
			if (ushortval + 1 > ushortval)				/* incr value if not at max */
			{
				ushortval++;
				trnsshort(keyptr, &ushortval);
			}
			break;

		case LONGIND:
			trnslong(&longval, keyptr);
			if (longval + 1 > longval)				/* incr value if not at max */
			{
				longval++;
				trnslong(keyptr, &longval);
			}
			break;

		case FLOATIND:
			trnsfloat(&floatval, keyptr);
			floatval++;
			trnsfloat(keyptr, &floatval);
			break;

		case DOUBLEIND:
			trnsdbl(&doubleval, keyptr);
			doubleval++;
			trnsdbl(keyptr, &doubleval);
			break;

		case SEGIND:												/* segmented index */
			/* find the last segment */
			for (	i = 0, seginfo = psp->segptr[keyn], wrkptr = (unsigned char *)keyptr;
					(seginfo[i].keytype != ENDSEGLIST) && (i < MAXKLEN);
					i++)
			{
				if (seginfo[i+1].keytype == ENDSEGLIST)		/* if last segment */
				{
					/* incr last segment key part */
					incrkeyval(psp, keyn, seginfo[i].keytype, wrkptr);
					break;
				}

				/* point to next key segment */
				wrkptr += _cikeylen(seginfo[i].keytype, wrkptr);
			}
			break;

		case CUST1IND:												/* custom index type 1*/
		case CUST2IND:												/* custom index type 2*/
		case CUST3IND:												/* custom index type 3*/
		case CUST4IND:												/* custom index type 4*/
		case CUST5IND:												/* custom index type 5*/
		case FIXIND:												/* fixed binary */
			custincrkeyval(psp, keyn, keyptr);
			break;
	}

#ifndef NO_WORD_CONVERSION
	cikeyflip(psp, keyn, keyptr, psp->indtyp[keyn] & INDTYPEMASK, INTELFMT, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
}

/* gets node ptr from node - must be in index node */
NDEPTR FDECL gtnptr(psp, ptr)
struct passparm *psp;						/* file info about indexes */
ENTRY *ptr;										/* pointer to key */
{
	NDEPTR p;									/* to hold value */
	register char *bptr;						/* byte level pointer */

	if (ptr->entlen)
	{
		/* ptr to nptr area */
		bptr = (char *)ptr + ptr->entlen - sizeof(NDEPTR) + 1;

		/* NOTE: the following line does not do anything, but is required 
   	 * by some defective compilers to allow this file to be compiled
	    */
		p = 0;										/* avoids the compiler error msg */
		trnsnde(&p, bptr);						/* transfer node ptr to variable */
		p = intelton(p, psp->wordorder);
		if (p < 0)
			p = NULLNDE;
	}
	else
		p = NULLNDE;								/* no entry, no node number */

	return(p);										/* return node ptr number */
}


/* returns length of data in entry */
int FDECL goodata(psp, p)
struct passparm *psp;
ENTRY *p;			/* pointer to key */
{
	int len;
	if (p->entlen)
#ifdef REMOVED
		len = p->entlen - strlen(gtkeyptr(p)) - SZELEN - SZENDX - SZEDBYTE - SZEREC;
#endif
		len = p->entlen - cikeylen(psp, psp->keyn, gtkeyptr(p)) - SZENDX - SZEDBYTE - SZEREC;
	else
		len = 0;

	return(len);
}

	
/* moves located entry into psp */
void FDECL cmovedata(p, psp)
ENTRY *p;							/* pointer to entry */
struct passparm *psp;			/* file parameters */
{
	int len;

	len = p->entlen;										/* entry length */
	if (len)
	{
		psp->rec = gtkrec(p, psp->wordorder);			/* return rec # */
		psp->dbyte = gtdbyte(p);	 						/* get data byte */
		blockmv(psp->retkey, gtkeyptr(p), len);		/* return key */
		psp->keyorder = psp->wordorder;
		if ((psp->dlen = goodata(psp, p)) != 0)   			/* if data also present */
		  psp->data = psp->retkey+cikeylen(psp, psp->keyn, psp->retkey);	/* then point to it */
		else
		  psp->data = NULL;									/* else clear ptr */
	}
}

/* build new key for insert into index node */
void FDECL bldikey(psp, hkey, nde, data, wordorder)
struct passparm *psp;	/* file psp */
ENTRY *hkey;				/* pointer to old key */
NDEPTR nde;					/* node number to put with old key */
ENTRY *data;				/* where to put it */
INT32 wordorder;			/* order of bytes in word */
{
	register int keylen;	/* size of key */
	NDEPTR nodenum;		/* for trnsnode */
	int keyn;				/* index number to work with */
#ifdef REMOVED
	char *ndeptr;			/* pointer to node number location */
#endif
	char *srcptr;			/* source for block move */
	char *dstptr;			/* destination for block move */

	nodenum = nde;

	keyn = gtkeyn(hkey);										/* index # of entry */
	keylen = cikeylen(psp, keyn, gtkeyptr(hkey));	/* key length */

	/* move key and rec in data area */
	srcptr = (char *)hkey;
	dstptr = (char *)data;
	blockmv(dstptr + 1, srcptr + 1, keylen + SZENDX + SZEDBYTE + SZEREC);	

	/* move node pointer in */
   nodenum = ntointel(nodenum, wordorder);
	trnsnde(dstptr + SZELEN + SZENDX + SZEDBYTE + SZEREC + keylen, &nodenum);   

	/* set length byte */
	data->entlen = (unsigned char)(keylen + SZENDX + SZEDBYTE + SZEREC + SZENDE);
}

				
/* build key for insert into leaf node */
void FDECL bldlkey(psp, inkey, keyn, key, rec, dbyte, data, dlen, wordorder)	
struct passparm *psp;	/* file psp */
ENTRY *inkey;				/* where to put it */
int keyn;					/* index of this entry */
char *key;					/* where key is */
long rec;					/* rec # */
int dbyte;					/* data byte */
char *data;					/* where data is */
int dlen;					/* length of data */
INT32 wordorder;			/* order of bytes in word */
{
	int keylen;						/* for overall length of entry */
	unsigned char *dstptr;		/* destination ptr for moving info */

	keylen = cikeylen(psp, keyn, key);		/* length of key */
	inkey->entlen = (unsigned char) (keylen + SZENDX + SZEDBYTE + SZEREC + dlen); 	/* set len byte */
	dstptr = (unsigned char *)inkey;
	dstptr += SZELEN;
	*dstptr = (unsigned char)keyn;					/* set index number */
	dstptr += SZENDX;
	*dstptr = (unsigned char)dbyte;					/* set data byte */
	dstptr += SZEDBYTE;
   rec = ltointel(rec, wordorder);					/* translate rec # */
	trnsrec(dstptr, &rec);								/* set rec # */
	dstptr += SZEREC;
	blockmv(dstptr, key, keylen);						/* move in key */
	dstptr += keylen;
	blockmv(dstptr, data, dlen);						/* move in data */
}

				
void FDECL setcurkey(psp, p)	/* set current key value */
CFILE *psp;
ENTRY *p;
{
	if (p == 0 || p->entlen == 0)
	{
		putcurkey(psp, psp->keyn, NULL);				/* save null key */
		putcurrcval(psp, psp->keyn, 0L);				/* zero record num */
		putcurdbyte(psp, psp->keyn, 0);				/* zero data byte */
	}
	else
	{
		putcurkey(psp, psp->keyn, (char *)gtkeyptr(p));		/* key in buffer */
		putcurrcval(psp, psp->keyn, gtkrec(p, psp->wordorder));
		putcurdbyte(psp, psp->keyn, gtdbyte(p));
	}
}



/* ------------------- key comparison routine ------------------------- */


/* compares entry at ptr to key parameters */
/* returns -1 if ptr<key  0 if ptr=key  1 if ptr>key */
int FDECL cikeytest(psp, ptr, keyn, key, rec, dbyte, bytemode)
struct passparm *psp;	/* file info */
register ENTRY *ptr;		/* ptr to node entry */
int keyn;					/* index number being searched */
KEYPTR key;					/* pointer to search key */
long rec;					/* record number of search key */
int dbyte;					/* data byte of search key */
INT32 bytemode;			/* byte mode of key passed into cikeytest */
{
	register int res;
	register long rres;
	register int ret;
	void *tstptr;
	void *entptr;
#ifndef NO_WORD_CONVERSION
	char fliptstkey[MAXMKLEN];		/* space to put byte flipped key from caller */
	char flipentkey[MAXMKLEN];		/* space to put byte flipped key from index */
#endif	/* !NO_WORD_CONVERSION */

	res = (unsigned)gtkeyn(ptr) - (unsigned)keyn;
	if (res)
	{
		ret = logicval(res);				/* index number doesn't match */
		psp->strcmpres = ret;
	}
	else										/* test data byte */
	{
		if (!psp->srchorder)					/* if normal search */
		{
#ifndef NO_WORD_CONVERSION
			/* flip test key if neccessary */
			if (	(INTELFMT && !isintel(bytemode))
				|| (!INTELFMT && isintel(bytemode)) )
			{
				/* flip test and entry keys first */
				cikeycpy(psp, keyn, fliptstkey, key);
				cikeyflip(psp, keyn, fliptstkey, psp->indtyp[keyn] & INDTYPEMASK, bytemode, INTELFMT);
				tstptr = fliptstkey;
			}
			else
			{
				tstptr = key;
			}

			/* flip entry key if neccessary */
			if (	(INTELFMT && !isintel(psp->wordorder))
				|| (!INTELFMT && isintel(psp->wordorder)) )
			{
				/* flip test and entry keys first */
				cikeycpy(psp, keyn, flipentkey, gtkeyptr(ptr));
				cikeyflip(psp, keyn, flipentkey, psp->indtyp[keyn] & INDTYPEMASK, psp->wordorder, INTELFMT);
				entptr = flipentkey;
			}
			else
			{
				entptr = (char *)gtkeyptr(ptr);
			}

#else		/* no flipping code */
				tstptr = key;
				entptr = (char *)gtkeyptr(ptr);
#endif	/* !NO_WORD_CONVERSION */

			res = cikeycmp(psp, keyn, (psp->indtyp[keyn] & INDTYPEMASK), 
								entptr, tstptr);
		}
		else
		if (psp->srchorder == FIRSTKEY)	/* if searching for first entry */
			res = 1;								/* force node key to be smaller */
		else
		if (psp->srchorder == LASTKEY)	/* if searching for last entry */
			res = -1;							/* force node key to be smaller */

		psp->strcmpres = res;
		if (res)
		{
			ret = logicval(res);				/* string doesn't match */
		}
		else										/* test record number */
		{
			rres = gtkrec(ptr, psp->wordorder) - rec;
			if (rres)
			{
				ret = logicval(rres);			/* record number doesn't match */
			}
			else										/* test data byte */
			{
				res = (gtdbyte(ptr) & 127) - (dbyte & 127);
				if (res)
				{
					ret = logicval(res);				/* data byte doesn't match */
				}
				else
					ret = 0;								/* otherwise, key is same */
			}
		}
	}
		
	return(psp->keycmpres = ret);
}

/* return the correct root for this index */
NDEPTR FDECL gtroot(psp, keyn)
struct passparm PDECL *psp;
int keyn;
{
	NDEPTR ret;
	unsigned int wrkroot;

	if (keyn == -1)
		wrkroot = psp->curroot;
	else
		wrkroot = psp->rootnum[keyn-1];

	if (wrkroot > MAXROOTS)
		ret = -1;
	else
	{
		psp->curroot = wrkroot;

		if (!wrkroot)		/* if 0th root number, use standard shared root node */
		{
			ret = psp->singleroot;
		}
		else
		{
			ret = psp->rootlist[wrkroot-1];
		}
	}

	return(ret);
}



int FDECL gtlevels(psp)
struct passparm PDECL *psp;
{
	int ret;
	int wrkroot;

	wrkroot = psp->curroot;
	if (!wrkroot)	/* if 0th root number, use standard shared root node */
	{
		ret = psp->singlelevels;
	}
	else
	{
		ret = psp->levelslist[wrkroot-1];
	}

	return(ret);
}

void ptroot(psp, newroot)
struct passparm PDECL *psp;
NDEPTR newroot;
{
	int wrkroot;

	wrkroot = psp->curroot;
	if (!wrkroot)	/* if 0th root number, use standard shared root node */
	{
		psp->singleroot = newroot;
	}
	else
	{
		psp->rootlist[wrkroot-1] = newroot;	/* else set current root pointer */
	}
}

void ptlevels(psp, levelcnt)
struct passparm PDECL *psp;
int levelcnt;
{
	int wrkroot;

	wrkroot = psp->curroot;
	if (!wrkroot)	/* if 0th root number, set default root level */
	{
		psp->singlelevels = levelcnt;
	}
	else
	{
		psp->levelslist[wrkroot-1] = (unsigned char)levelcnt;	/* else set current root level */
	}
}
