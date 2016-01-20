/*   dcheck.c  -  check and fix multi-key records
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


/* dbuild - rebuild multi-key records
 * If file was corrupted, it must have been rebuilt using bbuild first.
 * Old and new file passed in already opened.
 * The datalist must be supplied either by the datalist parameter or
 * in response to the buildproc callback (where a different datalist
 * can be supplied for each record based on the supplied idbyte value).
 * The optional buildproc function also serves as a status indicator,
 * showing the number of records processed.
 */

int EDECL dbuild(oldpsp, newpsp, databuf, buflen, defdlist, buildproc)
CFILE PDECL *oldpsp;					/* open psp of new file */
CFILE PDECL *newpsp;					/* open psp of new file */
char *databuf;							/* work buffer for each record */
int buflen;								/* length of work buffer */
struct flddef PDECL *defdlist;	/* default datalist (or NULL) */
BUILDPROCTYPE buildproc;			/* datalist and status callback (or NULL) */
{
	int ret;
	int cc;
	long dstatcnt = 0;
	int idbyte;							/* id byte of data record */
	long recnum;						/* new record number */
	int reclen;							/* found record length */
	DATALIST PDECL *recdlist;		/* datalist for record */
	long addcnt;						/* count of records added */

	/**/

	/* must either have default datalist or datalist callback function */
	if ((defdlist == NULL) && (buildproc == NULL))
		return(PARMERR);

	if (oldpsp->wordorder != newpsp->wordorder)
		return(BYTEMISMATCH);

	ret = csetfile(newpsp, 0);				/* set to fastest processing */
	if (ret)
		return(ret);

	if ((cc = strtwrit(newpsp)) != CCOK)
	   return(cc);

	/* position to first multi-key record */
	ret = cfirst(oldpsp, DATAINDX, databuf, buflen);
	if ((ret == CCOK) && (oldpsp->dlen != oldpsp->retlen))
		ret = BUFOVER;							/* buffer overflow */

	reclen = oldpsp->dlen;
	recnum = oldpsp->rec;
	addcnt = 0;

	if (ret == FAIL)
	{
		ret = CCOK;			/* no records, return OK */
	}
	else
	{
		while (ret == CCOK)
		{
			/* extract idbyte of record */
#ifndef NO_CHECKSUM
		   idbyte = *(databuf + sizeof(short) + sizeof (short));
#else
		   idbyte = *databuf;
#endif	/* NO_CHECKSUM */

			/* datalist and status callback */
			if (buildproc)
			{
				recdlist = (buildproc)(oldpsp, idbyte, dstatcnt++);
				if (recdlist == NULL)
				{
					ret = PROCINTR;		/* callback requested to stop */
					break;
				}
			}

			if (defdlist)
				recdlist = defdlist;		/* use default datalist */

			if (oldpsp->checkflg)
			{
			   ret = chkbuilddlist(oldpsp, recdlist);		/* check datalist for read/write */
				if (ret != CCOK)
					break;
			}

			/* call civerifyrec to make sure record is good */
			ret = civerifyrec(oldpsp, databuf, recdlist, buflen);
			if (ret == CCOK)
			{
				ret = addkeys(newpsp, databuf, recnum, recdlist);
				if (ret != CCOK)
				{
				   /* delete the keys */
				   delkeylist(newpsp, databuf, recnum, recdlist, reclen);
				   cputrec(newpsp, recnum);	/* put back rec */
				}
				else	
				{
				   ret = cdupadd(newpsp, DATAINDX, "", recnum, databuf, reclen);
				   if (ret == CCOK)
					{
				      oldpsp->currec = recnum;
						addcnt++;
					}
				   else
					{
				      ret = DATAERR;
					}
				}  
			}

			ret = cnext(oldpsp, DATAINDX, databuf, buflen);
			if (ret == FAIL)
			{
				ret = CCOK;			/* last record */
				break;
			}
			else
			if ((ret == CCOK) && (oldpsp->dlen != oldpsp->retlen))
			{
				ret = BUFOVER;							/* buffer overflow */
				break;
			}

			reclen = oldpsp->dlen;
			recnum = oldpsp->rec;
		}
	}

	/* set # of recs in file */
	if (ret == CCOK)
		ret = csetcnt(newpsp, addcnt);

	if (ret == CCOK)
		ret = csetrec(newpsp, oldpsp->nxtdrec);	/* set next rec # to old file */

	cc = endwrit(newpsp);
	if (cc != CCOK)
		ret = cc;

	return(ret);
}


/* validate datalist for dbuild operations */
int FDECL chkbuilddlist(psp, datalist)
struct passparm *psp;
DATALIST *datalist;
{
	while (datalist->fldtype != ENDLIST)
	{
	   if (chkftype(datalist->fldtype))
	      return(BADFLDTYP);
	   if (chkktype(psp, datalist->fldindex, datalist->keytype))
	      return(BADKEYTYP);
	   if (chkdflag(datalist->dupflag))
	      return(BADDUPFLG);
	   if (chkfindex(datalist->fldindex))
	      return(BADFLDINX);
	   datalist++;
	}
	return(CCOK);
}





/*
 *  Make sure the record is valid before using it in dbuild
 */
 
int FDECL civerifyrec(psp, buf, datalist, buflen)
struct passparm PDECL *psp;
char PDECL *buf;	/* where built datarec is */
DATALIST PDECL *datalist;
int buflen;
{
	char *p;
	DATALIST *dptr;
	short fieldnum;
	short newfieldnum;
	short	totfields;

	totfields = 0;
	while (datalist[totfields++].fldtype != ENDLIST)
		;
		
#ifndef NO_CHECKSUM
	p = buf + 1 + sizeof (short) + sizeof (short);
#else
	p = buf + 1;
#endif

	getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
	fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	while (fieldnum != -1)	/* end of the line */
	{
		if (fieldnum >= totfields)
		{
			return(FAIL);
		}
		
		newfieldnum = fieldnum;				/* field # of this key item */
	   dptr = &datalist[fieldnum];

	   /* point p to next field in buffer */
	   p = fldmoveup(psp, p, dptr);
		if (p - buf > buflen)
		{
			return(FAIL);
		}
		
		getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
		fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
		
		if (fieldnum != -1 && fieldnum < newfieldnum)
		{
			return(FAIL);
		}
	}		/* while */

	return(CCOK);
}


/* check multi-key records for matching index entries */
int FDECL dcheck(psp,  databuf, buflen, defdlist, buildproc)
CFILE PDECL *psp;					/* open psp of new file */
char *databuf;							/* work buffer for each record */
int buflen;								/* length of work buffer */
struct flddef PDECL *defdlist;	/* default datalist (or NULL) */
BUILDPROCTYPE buildproc;			/* datalist and status callback (or NULL) */
{
	int ret;
	int cc;
	long dstatcnt = 0;
	int idbyte;							/* id byte of data record */
	long recnum;						/* new record number */
	int reclen;							/* found record length */
	DATALIST PDECL *recdlist;		/* current datalist for record */
	DATALIST PDECL *prevdlist = NULL;	/* previous datalist for record */
	long reccnt;						/* count of records in DATAINDX */
	char keycnts[NUMINDXS];			/* count of keys in each index */
	int keyn;							/* index number to check */
	char keyval[MAXKLEN];			/* current test key value */
#ifndef NO_CHECKSUM
	short checks1;
	short checks2;
#endif

	/**/

	/* must either have default datalist or datalist callback function */
	if ((defdlist == NULL) && (buildproc == NULL))
		return(PARMERR);

	if (strtread(psp) != CCOK)					/* start reading operation */
		return(psp->retcde = FILELOCKED);

	/* clear used index flags to zero */
	cclear(keycnts, sizeof(keycnts), 0);

	/* check that all records have a matching index key */
	/* position to first multi-key record */
	ret = cfirst(psp, DATAINDX, databuf, buflen);
	if ((ret == CCOK) && (psp->dlen != psp->retlen))
		ret = BUFOVER;							/* buffer overflow */

	reclen = psp->dlen;
	recnum = psp->rec;
	reccnt = 0;

	if (ret == FAIL)
	{
		ret = CCOK;			/* no records, return OK */
	}
	else
	{
		while (ret == CCOK)
		{
			/* extract idbyte of record */
#ifndef NO_CHECKSUM
		   idbyte = *(databuf + sizeof(short) + sizeof (short));
#else
		   idbyte = *databuf;
#endif	/* NO_CHECKSUM */

			/* datalist and status callback */
			if (buildproc)
			{
				recdlist = (buildproc)(psp, idbyte, dstatcnt++);
				if (recdlist == NULL)
				{
					ret = PROCINTR;		/* callback requested to stop */
					break;
				}
			}

			if (defdlist)
				recdlist = defdlist;		/* use default datalist */

			if (psp->checkflg)
			{
			   ret = chkbuilddlist(psp, recdlist);		/* check datalist for errors */
				if (ret != CCOK)
					break;
			}

			/* call civerifyrec to make sure record is good */
			ret = civerifyrec(psp, databuf, recdlist, buflen);
			if (ret == CCOK)
			{
				ret = findkeylist(psp, databuf, recnum, recdlist, reclen, keycnts);
				if (ret == CCOK)		/* if did find matching keys */
				{
			      psp->currec = recnum;		/* set as current record number */
					reccnt++;						/* incr record count */
				}  
			}

			if (ret != CCOK)
				break;			/* stop if any error in record */

			ret = cnext(psp, DATAINDX, databuf, buflen);
			if (ret == FAIL)
			{
				ret = CCOK;			/* last record */
				break;
			}
			else
			if ((ret == CCOK) && (psp->dlen != psp->retlen))
			{
				ret = BUFOVER;							/* buffer overflow */
				break;
			}

			reclen = psp->dlen;
			recnum = psp->rec;
		}
	}

	/* check each index key value to make sure it matches record */
	if (ret == CCOK)
	{
		for (keyn = 1; (keyn <= MAXDINDX) && (ret == CCOK); keyn++)
		{
			if (!keycnts[keyn])		/* if didn't have keys in this index, skip check */
				continue;

			/* position to first key of this index */
			ret = cfirst(psp, keyn, NULL, 0);
			if (ret == FAIL)
			{
				ret = CCOK;			/* no records in this index, keep going */
				continue;
			}
			else
			{
				while (ret == CCOK)
				{
					cikeycpy(psp, keyn, keyval, psp->key);	/* save key for test */
					recnum = psp->rec;			/* record number found */
					psp->currec = psp->rec;
					ret = dgetid(psp);				/* get idbyte value */
					if (ret != CCOK)
					{
						break;
					}

					idbyte = psp->idbyte;

					/* datalist and status callback */
					if (buildproc)
					{
						recdlist = (buildproc)(psp, idbyte, dstatcnt++);
						if (recdlist == NULL)
						{
							ret = PROCINTR;		/* callback requested to stop */
							break;
						}
					}

					if (defdlist)
						recdlist = defdlist;		/* use default datalist */

					if (psp->checkflg && (recdlist != prevdlist))
					{
					   ret = chkbuilddlist(psp, recdlist);		/* check datalist for errors */
						if (ret != CCOK)
							break;
					}

					psp->keyn = DATAINDX;
					psp->key = "";
					psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
					psp->rec = recnum;
					psp->data = databuf;
					psp->dlen = buflen;
					ret = _cfind(psp);
					if ((ret == CCOK) && (psp->dlen != psp->retlen))
						ret = BUFOVER;							/* buffer overflow */
					if (ret > 0)
						ret = FAIL;				/* translate GREATER and KEYMATCH */

					if (ret != CCOK)
						break;			/* stop if any error in record */

#ifndef NO_CHECKSUM
					if (psp->checkflg && (recdlist != prevdlist))
					{
						getshort(checks1, databuf);
#ifndef NO_WORD_CONVERSION
						checks1 = inteltos(checks1, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
						checks2 = (short)checkdlist(recdlist);

						if (checks1 != checks2)
						{
							endread(psp);				/* release file after read */
						   return(CHKSUMERR);
						}
					}
#endif	/* NO_CHECKSUM */

					prevdlist = recdlist;

					/* test data record to validate that key value is correct */
					ret = civalidatedkey(psp, databuf, psp->dlen, recdlist, keyval, keyn);
					if (ret != CCOK)
					{
						break;			/* stop if any error in record */
					}

					ret = cnext(psp, keyn, NULL, 0);
					if (ret == FAIL)
					{
						ret = CCOK;			/* past last key in index */
						break;
					}
				}
			}
		}

		if (ret != CCOK)
		{
			/* set return error information */
			psp->keyn = keyn;						/* index that has bad key */
			cikeycpy(psp, keyn, psp->retkey, keyval);	/* bad key */
			psp->key = psp->retkey;
			psp->keyorder = INTELFMT;
			psp->rec = recnum;
		}
	}

	cc = endread(psp);				/* release file after read */
	if (ret == CCOK)
		ret = cc;

	return(ret);
}






/* find keys based on record */
int FDECL findkeylist(psp, buf, rec, datalist, len, keycounts)
CFILE PDECL *psp;
char PDECL *buf;					/* where built datalist is */
long rec;							/* number of record to test */
DATALIST PDECL *datalist;		/* datalist for this record */
int len;								/* length of found data */
char keycounts[NUMINDXS];		/* count of keys found in each index */
{
	int ret = CCOK;
	char *p = buf;
	DATALIST *dptr;
	DATALIST key;
	short fieldnum;
	short newfieldnum;
	short fldlen;
	unsigned char fbuf[260];
#ifndef NO_CHECKSUM
	short checks1;
	short checks2;
#endif
	char *keyptr;

	/**/

#ifndef NO_CHECKSUM
	getshort(checks1, (buf + sizeof (short)));
#ifndef NO_WORD_CONVERSION
	checks1 = inteltos(checks1, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	if(tstchksumdata(buf, len, checks1) != CCOK)
	   return(CHKSUMDERR);

	if (psp->checkflg)
	{
		getshort(checks1, buf);
#ifndef NO_WORD_CONVERSION
		checks1 = inteltos(checks1, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
		checks2 = (short)checkdlist(datalist);

		if (checks1 != checks2)
		   return(CHKSUMERR);
	}
#endif	/* NO_CHECKSUM */

	key.fldptr = (void *)fbuf;

#ifndef NO_CHECKSUM
	p += 1 + sizeof (short) + sizeof (short);
#else
	p += 1;
#endif

	getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
	fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	while (fieldnum != -1)	/* end of the line */
	{
		newfieldnum = fieldnum;					/* field # of this key */
	   dptr = &datalist[fieldnum];
	   key.fldtype = dptr->fldtype;
	   key.keytype = dptr->keytype;
	   key.fldindex = dptr->fldindex;
		if (dptr->fldlen < sizeof(fbuf))
	   	key.fldlen = dptr->fldlen;
		else
			key.fldlen = sizeof(fbuf);			/* limit key length to internal buffer size */
	   key.dupflag = dptr->dupflag;

	   if (dptr->keytype == NONKEY)
	   {
	      p = fldmoveup(psp, p, &key);
			getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	      continue;
	   }

		if (dptr->keytype == SEGIND)				/* segmented key item */
		{
			ret = crtsegkey(psp, buf, datalist, newfieldnum, fbuf);
		   if (ret != CCOK)
		      return(ret);	/* error building segmented key */

			psp->keyorder = psp->wordorder;	/* seg key is in file byte order */
			p = fldmoveup(psp, p, &key);		/* move up */
			getshort(fieldnum, p);				/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
		}
		else
		if (	(dptr->keytype == BINARYIND)	  /* binary key item */
			  												/* or custom key item */
			|| ((dptr->keytype >= CUST1IND) && (dptr->keytype <= MAXINDTYP)))
		{
			/* build a binary key which has 1 byte length followed by value */
			keyptr = p+sizeof(short);				/* field with 2 byte length */
			getshort(fldlen, keyptr);				/* length of binary field */
#ifndef NO_WORD_CONVERSION
			fldlen = inteltos(fldlen, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
			if (fldlen >= MAXKLEN)
				fldlen = MAXKLEN - 1;				/* limit length of binary key */
			keyptr += sizeof(short);				/* pnt to binary key value */
			fbuf[0] = (unsigned char) fldlen;	/* length byte for key */
			blockmv(fbuf+1, keyptr, fldlen);		/* move field to key buffer */

			p = fldmoveup(psp, p, &key);					/* move up */
			getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
			psp->keyorder = psp->wordorder;	/* seg key is in file byte order */
#endif	/* !NO_WORD_CONVERSION */
		}
		else
		{
			p = fieldtrans(psp, p, &key);				/* trans key and move up */
			getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
			psp->keyorder = INTELFMT;	/* seg key is in machine native byte order */
#endif	/* !NO_WORD_CONVERSION */
		}

		/* find key that was in record buffer */
		psp->keyn = key.fldindex;
		psp->key = key.fldptr;
		psp->rec = rec;
		psp->data = NULL;
		psp->dlen = 0;
		ret = _cfind(psp);
	   if (ret != CCOK)	/* if did not find key, this is a bad file */
		{
			/* set return error information */
			psp->keyn = key.fldindex;		/* index that is missing key */
			psp->rec = rec;					/* record number that is missing */
			cikeycpy(psp, psp->keyn, psp->retkey, key.fldptr);	/* missing key */
			psp->key = psp->retkey;
			psp->keyorder = psp->wordorder;
			if (ret > 0)
				ret = FAIL;		/* translate KEYMATCH and GREATER into FAIL */

	      break;	/* stop on error */
		}
		else
		{
			keycounts[key.fldindex] = TRUE;	/* count good key in this index */
		}
	}

	return(ret);
}


/* validate the key value found for record in buffer */
int FDECL civalidatedkey(psp, buf, len, datalist, testkey, testind)
CFILE PDECL *psp;
char PDECL *buf;					/* where built datalist is */
int len;								/* length of found data */
DATALIST PDECL *datalist;		/* datalist for this record */
void *testkey;						/* test key value */
int testind;						/* index number to test */
{
	int ret = FAIL;				/* assume failure */
	char *p = buf;
	DATALIST *dptr;
	DATALIST key;
	short fieldnum;
	short newfieldnum;
	short fldlen;
	unsigned char fbuf[MAXMKLEN];
#ifndef NO_CHECKSUM
	short checks1;
#endif
	char *keyptr;
	int res;					/* result of key comparison */

	/**/

#ifndef NO_CHECKSUM
	getshort(checks1, (buf + sizeof (short)));
#ifndef NO_WORD_CONVERSION
	checks1 = inteltos(checks1, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	if(tstchksumdata(buf, len, checks1) != CCOK)
	   return(CHKSUMDERR);
#endif	/* NO_CHECKSUM */

	key.fldptr = (void *)fbuf;

#ifndef NO_CHECKSUM
	p += 1 + sizeof (short) + sizeof (short);
#else
	p += 1;
#endif

	getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
	fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	while (fieldnum != -1)	/* end of the line */
	{
		newfieldnum = fieldnum;					/* field # of this key */
	   dptr = &datalist[fieldnum];
	   key.fldtype = dptr->fldtype;
	   key.keytype = dptr->keytype;
		if (key.keytype == STRINGFLD)
			key.keytype = STRINGIND;
	   key.fldindex = dptr->fldindex;
		if (dptr->fldlen < sizeof(fbuf))
	   	key.fldlen = dptr->fldlen;
		else
			key.fldlen = sizeof(fbuf);			/* limit key length to internal buffer size */
	   key.dupflag = dptr->dupflag;

	   if ((dptr->keytype == NONKEY) && (key.fldindex == testind))
			return(FAIL);

	   if ((dptr->keytype == NONKEY) || (key.fldindex != testind))
	   {
	      p = fldmoveup(psp, p, &key);
			getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	      continue;
	   }

		if (dptr->keytype == SEGIND)				/* segmented key item */
		{
			ret = crtsegkey(psp, buf, datalist, newfieldnum, fbuf);
		   if (ret != CCOK)
		      return(ret);	/* error building segmented key */

			p = fldmoveup(psp, p, &key);					/* move up */
			getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);

			/* make sure key is in native byte ordering before comparing */
			cikeyflip(psp, key.fldindex, key.fldptr, key.keytype, psp->wordorder, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

		}
		else
		if (	(dptr->keytype == BINARYIND)	  /* binary key item */
			  												/* or custom key item */
			|| ((dptr->keytype >= CUST1IND) && (dptr->keytype <= MAXINDTYP)))
		{
			/* build a binary key which has 1 byte length followed by value */
			keyptr = p+sizeof(short);				/* field with 2 byte length */
			getshort(fldlen, keyptr);				/* length of binary field */
#ifndef NO_WORD_CONVERSION
			fldlen = inteltos(fldlen, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
			if (fldlen >= MAXKLEN)
				fldlen = MAXKLEN - 1;				/* limit length of binary key */
			keyptr += sizeof(short);				/* pnt to binary key value */
			fbuf[0] = (unsigned char) fldlen;	/* length byte for key */
			blockmv(fbuf+1, keyptr, fldlen);		/* move field to key buffer */
			p = fldmoveup(psp, p, &key);					/* move up */
			getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);

			/* make sure key is in native byte ordering before comparing */
			cikeyflip(psp, key.fldindex, key.fldptr, key.keytype, psp->wordorder, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

		}
		else
		{
			p = fieldtrans(psp, p, &key);				/* trans key and move up */
			getshort(fieldnum, p);					/* next field number */

#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
			/* NOTE: key is in native byte ordering after fieldtrans */
#endif	/* !NO_WORD_CONVERSION */
		}

		/* compare key value of field in buffer with test value */
		res = cikeycmp(psp, testind, key.keytype, key.fldptr, testkey);
		if (res)		/* if value in record not equal to key in index, error */
		{
			ret = FAIL;
			break;
		}
		else
			ret = CCOK;
	}

	return(ret);
}


