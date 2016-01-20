/*   dfunc.c     basic code for high level functions
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


long FDECL getnextrec(psp)	/* get next data recrd number */
CFILE PDECL *psp;
{
	long rec;
	int cc;

	/**/

	if (psp->setdrec > 0)
	{
		rec = psp->setdrec;			/* use assigned record number */
		psp->setdrec = -1;			/* clear after using */
	}
	else
	{
		/* if not reusing rec #'s or don't have one to reuse, assign next */
		if (	(!psp->reuse)
			|| (!psp->delcnt)
			|| (cfirst(psp, DELINDX, "", 0) != CCOK))
		{
		   cc = cnextrec(psp);
			if (cc != CCOK)
				rec = -1;						/* error */
			else
			{
				rec = psp->rec;				/* next record number from header */
			}
		}
		else
		{
			psp->delcnt--;						/* one less deleted record number */
		   rec = psp->rec;					/* reuse deleted record number */
		   cc = cdelcur(psp, DELINDX);	/* remove number from delete list */
			if (cc != CCOK)
				rec = -1;
		}	
	}

	return(rec);
}


int FDECL cputrec(psp, rec)	/* put deleted record number in index */
CFILE PDECL *psp;
long rec;
{
	int cc;

	if (psp->reuse)
	{
		cc = cdupadd(psp, DELINDX, "", rec, "", 0);
		if (cc == CCOK)
			psp->delcnt++;				/* incr number of deleted record numbers */
	}
	else
		cc = CCOK;						/* don't reuse rec numbers */

	return(cc);
}


/* create a segmented key for adding, deleting, and updating keys */
int FDECL crtsegkey(psp, buf, datalist, segoff, binkey)
CFILE PDECL *psp;
char PDECL *buf;								/* where built datarec is */
DATALIST PDECL *datalist;					/* ptr to datalist */
int segoff;								/* datalist offset of segment field */
unsigned char *binkey;				/* buffer to build key in */
{
	int i;							/* loop counter */
	short fldlen;					/* length of a field */
	short fieldnum;				/* datalist offset of a field */
	char *q;							/* ptr into buffer containing record */
	KEYSEGLIST *seginfo;				/* ptr to segment list */
	unsigned char *entwrkptr;	/* ptr into entry segmented key */
	int entlen;						/* length of entry binary key */
	int entseglen = 0;			/* length of entry key segment */
	unsigned char *srcptr;		/* ptr to source field */
	int maxseglen;					/* max segment length specified by user */

	int xint = 0;					/* null field values */
	short xshort = 0;
	long xlong = 0;
	float xfloat = (float)0;
	double xdouble = (double)0;

	/**/

	/* build each keytype of segmented key */
	seginfo = psp->segptr[datalist[segoff].fldindex];	/* ptr to segment list */
	entwrkptr = binkey + 1;						/* build seg key in buffer */
	entlen = 0;										/* clear cummulative length */
	for (	i = 0;
			seginfo[i].keytype != ENDSEGLIST;
			i++)
	{
		/* search in record buffer for segment field */
#ifndef NO_CHECKSUM
		q = buf + 1 + sizeof (short) + sizeof (short);
#else
		q = buf + 1;
#endif

		getshort(fieldnum, q);
#ifndef NO_WORD_CONVERSION
		fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
		while ((fieldnum < seginfo[i].fldoff) && (fieldnum != -1))
		{
		   /* point q to next field in buffer */
		   q = fldmoveup(psp, q, &datalist[fieldnum]);
			getshort(fieldnum, q);
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
		}

		/* if field not in record, must be zero or null */
		if (fieldnum != seginfo[i].fldoff)
		{
			/* copy zero or null value into key */
			switch (seginfo[i].keytype)
			{
				case STRINGIND:
				case NOCASEIND:
					*(entwrkptr) = '\0';					/* null string */
					entseglen = 1;							/* length of segment */
					break;

				case BINARYIND:
				case CUST1IND:	
				case CUST2IND:
				case CUST3IND:
				case CUST4IND:
				case CUST5IND:
					*(entwrkptr) = (unsigned char)0;	/* null binary or custom */
					entseglen = 1;							/* length of segment */
					break;
	
				case FIXIND:
					entseglen = FIXINDLEN;
					cclear(entwrkptr, entseglen, 0);
					break;

				case INTIND:
					trnsint(entwrkptr, &xint);
					entseglen = sizeof(int);			/* length of segment */
					break;

				case SHORTIND:
				case USHORTIND:
					trnsshort(entwrkptr, &xshort);
					entseglen = sizeof(short);			/* length of segment */
					break;

				case LONGIND:
					trnslong(entwrkptr, &xlong);
					entseglen = sizeof(long);			/* length of segment */
					break;

				case FLOATIND:
					trnsfloat(entwrkptr, &xfloat);
					entseglen = sizeof(float);			/* length of segment */
					break;

				case DOUBLEIND:
					trnsdbl(entwrkptr, &xdouble);
					entseglen = sizeof(double);		/* length of segment */
					break;

				case SEGIND:
					return(BADSEGLIST);			/* invalid setment keytype */

				case RES1IND:
				case RES2IND:
				case RES3IND:
				case RES4IND:
				case RES5IND:
					return(BADSEGLIST);			/* invalid setment keytype */

				default:
					return(BADSEGLIST);			/* invalid setment keytype */
			}	/* end switch */
		}
		else				/* have value in buffer, copy to key */
		{
			/* point to key field in buffer */
		   srcptr = (unsigned char *)q + sizeof(short);
			maxseglen = seginfo[i].maxlen;
			if (!maxseglen)
				maxseglen = MAXKLEN;		/* no max specified, assume all of key */

			/* transfer field to key buffer based on key type */
			switch (seginfo[i].keytype)
			{
				case STRINGIND:
				case NOCASEIND:
					entseglen = (int) strlen((char *)srcptr);	/* length of segment */
					if (entseglen > maxseglen)
						entseglen = maxseglen;
					entseglen++;								/* plus 1 for terminator */
					if ((entlen + entseglen) >= MAXKLEN)
						return(BADSTRLEN);				/* key too long */
					/* copy limited number of chars from ASCIIZ string key */
					strncpy((char *)entwrkptr, (char *)srcptr, (unsigned int)entseglen);
					*(entwrkptr + entseglen - 1) = '\0';	/* null terminator */
					break;

				case BINARYIND:
				case CUST1IND:	
				case CUST2IND:
				case CUST3IND:
				case CUST4IND:
				case CUST5IND:
					/* build a binary key which has 1 byte length followed by value */
					getshort(fldlen, srcptr);				/* length of bin field */
#ifndef NO_WORD_CONVERSION
					fldlen = inteltos(fldlen, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
					if (fldlen > maxseglen)					/* limit to maxlen */
						fldlen = (short)maxseglen;
					entseglen = fldlen + 1;					/* length of key segment */
					if ((entlen + entseglen) >= MAXKLEN)
						return(BADSTRLEN);					/* key too long */
					*entwrkptr = (unsigned char)fldlen;	/* set key length */
					blockmv(entwrkptr+1, srcptr+sizeof(short), fldlen);/* move field to key buffer */
					break;

				case FIXIND:
					entseglen = FIXINDLEN;
					blockmv(entwrkptr, srcptr, entseglen);/* move field to key buffer */
					break;

				case INTIND:
					trnsint(entwrkptr, srcptr);
					entseglen = sizeof(int);			/* length of segment */
					break;

				case SHORTIND:
				case USHORTIND:
					trnsshort(entwrkptr, srcptr);
					entseglen = sizeof(short);			/* length of segment */
					break;

				case LONGIND:
					trnslong(entwrkptr, srcptr);
					entseglen = sizeof(long);			/* length of segment */
					break;

				case FLOATIND:
					trnsfloat(entwrkptr, srcptr);
					entseglen = sizeof(float);			/* length of segment */
					break;

				case DOUBLEIND:
					trnsdbl(entwrkptr, srcptr);
					entseglen = sizeof(double);		/* length of segment */
					break;

				case SEGIND:
					return(BADSEGLIST);			/* invalid setment keytype */

				case RES1IND:
				case RES2IND:
				case RES3IND:
				case RES4IND:
				case RES5IND:
					return(BADSEGLIST);			/* invalid setment keytype */

				default:
					return(BADSEGLIST);			/* invalid setment keytype */

			}		/* end switch */

		}			/* end if (don't/do have field in buf) */

		entwrkptr += entseglen;			/* skip to next key segment */
		entlen += entseglen;				/* increase total key length */

		if ((entlen >= MAXKLEN) || !entseglen)
			return(BADSTRLEN);			/* key too long or zero length */

	}		/* end of for loop to create each key segment */

	*binkey = (unsigned char)entlen;			/* total length of segmented key */

	return(CCOK);
}

/* do key adds for dfunc functions */
int EDECL cidunqadd(psp, keyn, key, rec, data, dlen)
struct passparm *psp;
int keyn;		/* index number */
KEYPTR key;		/* key */
long rec;		/* record number */
void *data;		/* data to add */
int dlen;		/* data length */
{
	/* set for lower level routine */
	psp->keyn = keyn;
	psp->key = key;
	psp->rec = rec;
	psp->data = data;
	psp->dlen = dlen;
	return(_cunqadd(psp));	/* return */
}



int FDECL addkeys(psp, buf, rec, datalist)	/* add keys from keylist */
CFILE PDECL *psp;
char PDECL *buf;	/* where built datarec is */
long rec;
DATALIST PDECL *datalist;
{
	int ret = CCOK;
	char *p;
	DATALIST *dptr;
	short fieldnum;
	short newfieldnum;
	unsigned char *keyptr;					/* pointer to key to add */
	unsigned char binkey[MAXMKLEN+sizeof(double)];/* work space for binary key */
	short fldlen;

	/**/
	
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
		newfieldnum = fieldnum;				/* field # of this key item */
	   dptr = &datalist[fieldnum];
	   keyptr = (unsigned char *)p + sizeof(short);/* point to key in buffer */

	   /* point p to next field in buffer */
	   p = fldmoveup(psp, p, dptr);
		getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
		fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */

	   if (dptr->keytype == NONKEY)	  /* non key item */
	      continue;


		if (dptr->keytype == SEGIND)				/* segmented key item */
		{
			ret = crtsegkey(psp, buf, datalist, newfieldnum, binkey);
		   if (ret != CCOK)
		      return(ret);	/* error building segmented key */
			keyptr = binkey;							/* use key in buffer */
		}
		else
	   if (	(dptr->keytype == BINARYIND)		/* binary key item */
			  												/* or custom key item */
			|| ((dptr->keytype >= CUST1IND) && (dptr->keytype <= MAXINDTYP)))
		{
			/* build a binary key which has 1 byte length followed by value */
			getshort(fldlen, keyptr);				/* length of binary field */
#ifndef NO_WORD_CONVERSION
			fldlen = inteltos(fldlen, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
			if (fldlen >= MAXKLEN)
				return(BADSTRLEN);			/* key too long */
			binkey[0] = (unsigned char) fldlen;		/* set key length */
			blockmv(binkey+1, keyptr+sizeof(short), fldlen);/* move field to key buffer */
			keyptr = binkey;							/* use key in buffer */
		}
#ifndef NO_WORD_CONVERSION
		else
		{
			/* copy value from data buffer to protect in case of byteflip */
			cikeycpy(psp, dptr->fldindex, binkey, keyptr);
			keyptr = binkey;
		}

		/* make sure key is in native byte ordering before adding */
		cikeyflip(psp, dptr->fldindex, binkey, dptr->keytype, psp->wordorder, INTELFMT);
		psp->keyorder = INTELFMT;
#endif	/* !NO_WORD_CONVERSION */

	   if (dptr->dupflag == DUPKEY)
	      ret = cdupadd(psp, dptr->fldindex, keyptr, rec, "", 0);
	   else
	      ret = cidunqadd(psp, dptr->fldindex, keyptr, rec, "", 0);

	   if (ret != CCOK)
	      return(ret);	/* will need to delete all added ones */
	}
	return(ret);
}

/* delete keys from keylist */
int FDECL delkeylist(psp, buf, rec, datalist, len)
CFILE PDECL *psp;
char PDECL *buf;	/* where built datalist is */
long rec;
DATALIST PDECL *datalist;
int len;	/* length of found data */
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

			p = fldmoveup(psp, p, &key);					/* move up */
			getshort(fieldnum, p);					/* next field number */
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
#endif	/* !NO_WORD_CONVERSION */
		}
		else
		{
			p = fieldtrans(psp, p, &key);				/* trans key and move up */
			getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
		}

		/* delete key that was in record buffer */
#ifndef NO_WORD_CONVERSION
		psp->keyorder = psp->wordorder;	/* key from field in file byteorder */
#endif
		psp->keyn = key.fldindex;
		psp->key = key.fldptr;
		psp->rec = rec;
		ret = _cdelete(psp);
	   if (ret == FAIL)
	      ret = CCOK;			/* force not found keys to be CCOK */

	   if (ret != CCOK)
	      return(ret);	/* stop on fatal error */
	}

	return(ret);
}


/* change keys pointing to a data record entry */
int FDECL chgkeylist(psp, buf, rec, datalist, len)
CFILE PDECL *psp;
char PDECL *buf;	/* where built datalist is */
long rec;
DATALIST PDECL *datalist;
int len;	/* length of found data */
{
	int ret = CCOK;
	char *p = buf;
	DATALIST *dptr, key, *curdlist;
	short fieldnum;
   short fldcnt;				/* offset into datalist */
   short chgfld;				/* TRUE == updating field, FALSE == adding field */
	short newfieldnum;
	short fldlen;
	unsigned char *oldkeyptr;
	unsigned char *newkeyptr;
	unsigned char oldkey[MAXMKLEN];
	unsigned char newkey[MAXMKLEN];
#ifndef NO_CHECKSUM
	short checks1;
	short checks2;
#endif	/* NO_CHECKSUM */

	/**/

#ifndef NO_CHECKSUM
	getshort(checks1, (buf + sizeof(short)));
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
	
	key.fldptr = (void *)oldkey;
	oldkeyptr = oldkey;							/* location on old key */

#ifndef NO_CHECKSUM
	p += 1 + sizeof (short) + sizeof (short);
#else
	p += 1;
#endif

	getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
	fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	for (fldcnt = 0, curdlist = datalist; curdlist->fldtype != ENDLIST; fldcnt++, curdlist++)
	{
      while ((fieldnum < fldcnt) && (fieldnum != -1))
		{
			p = fldmoveup(psp, p, &key);			/* move up */
			getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
		}

      if (fieldnum == fldcnt)	/* if this field is in old rec */
      	chgfld = TRUE;			/* update or delete the old key */
      else
      	chgfld = FALSE;		/* might be adding a new key */

		newfieldnum = fldcnt;
	   dptr = &datalist[fldcnt];
	   key.fldtype = dptr->fldtype;
	   key.keytype = dptr->keytype;
		if (key.keytype == STRINGFLD)
			key.keytype = STRINGIND;
	   key.fldindex = dptr->fldindex;
		if (dptr->fldlen < sizeof(oldkey))
		   key.fldlen = dptr->fldlen;
		else
			key.fldlen = sizeof(oldkey);			/* limit key length to internal buffer size */
	   key.dupflag = dptr->dupflag;

		if (dptr->keytype == NONKEY)
		{
			if (chgfld)
			{
				p = fldmoveup(psp, p, &key);
				getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
				fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
			}

			continue;
		}

      if (chgfld)		/* if we are changing a key field, calc old key value */
		{
			if (dptr->keytype == SEGIND)				/* segmented key item */
			{
				ret = crtsegkey(psp, buf, datalist, newfieldnum, oldkey);
			   if (ret != CCOK)
			      return(ret);	/* error building segmented key */

				p = fldmoveup(psp, p, &key);					/* move up */
				getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
				fieldnum = inteltos(fieldnum, psp->wordorder);
				/* make sure old key is in native byte ordering before adding */
				cikeyflip(psp, key.fldindex, oldkeyptr, key.keytype, psp->wordorder, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
			}
			else
			if (	(dptr->keytype == BINARYIND)	  /* binary key item */
				  												/* or custom key item */
				|| ((dptr->keytype >= CUST1IND) && (dptr->keytype <= MAXINDTYP)))
			{
				/* build a binary key which has 1 byte length followed by value */
				oldkeyptr = (unsigned char *)p+sizeof(short);/* field with 2 byte length */
				getshort(fldlen, oldkeyptr);			/* length of binary field */
#ifndef NO_WORD_CONVERSION
				fldlen = inteltos(fldlen, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
				if (fldlen >= MAXKLEN)
					fldlen = MAXKLEN - 1;				/* limit length of binary key */
				oldkeyptr += sizeof(short);			/* pnt to binary key value */
				oldkey[0] = (unsigned char) fldlen;	/* length byte for key */
				blockmv(oldkey+1, oldkeyptr, fldlen);	/* move field to key buffer */
				p = fldmoveup(psp, p, &key);					/* move up */
				getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
				fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
			}
			else
			{
				p = fieldtrans(psp, p, &key);				/* trans key and move up */
				getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
				fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
			}
		}

		oldkeyptr = oldkey;							/* location of old key */
		newkeyptr = newkey;							/* location of new key */

		/* if not deleted field, test to see if changed */
		if (!DELFLD(key.fldtype))
		{
			/* build new key from datalist */
			ret = dsetkey(psp, key.fldindex, datalist, newfieldnum, newkey);
			if (ret != CCOK)
				return(ret);				/* error building new key */

			/* if key not changed, skip update */
			if (chgfld && cikeycmp(psp, key.fldindex, key.keytype, newkeyptr, oldkeyptr) == 0)
				continue;
		}

      if (chgfld)
      {
#ifndef NO_WORD_CONVERSION
			psp->keyorder = INTELFMT;
#endif
			psp->keyn = key.fldindex;
			psp->key = (void *)oldkeyptr;
			psp->rec = rec;
			ret = _cdelete(psp);
		   if (ret != CCOK)
		   {
		      rvchkeylist(psp, buf, rec, datalist, fldcnt - 2);
		      return(ret);
		   }
		}

#ifndef NO_WORD_CONVERSION
		psp->keyorder = INTELFMT;
#endif

		/* if not deleted field or if new field, add field value key */
		if (!DELFLD(key.fldtype))
		{
		   if (key.dupflag == DUPKEY)
		      ret = cdupadd(psp, key.fldindex, newkeyptr, rec, "", 0);
		   else
		      ret = cidunqadd(psp, key.fldindex, newkeyptr, rec, "", 0);
		   if (ret != CCOK)	
		   {
		      rvchkeylist(psp, buf, rec, datalist, fldcnt - 1);
		      return(ret);
		   }
		   if (ret != CCOK)
		      return(ret);	/* stop on fatal error */
		}
	}

	return(ret);
}

/* reverse changes in key entries when an update fails */
int FDECL rvchkeylist(psp, buf, rec, datalist, fnum)
CFILE PDECL *psp;
char PDECL *buf;	/* where built datalist is */
long rec;
DATALIST PDECL *datalist;
int fnum;	/* field number to go until */
{
	int ret = CCOK;
	char *p = buf;
	DATALIST *dptr, key;
	short fieldnum;
	short newfieldnum;
	short fldlen;
	unsigned char *oldkeyptr;
	unsigned char *newkeyptr;
	unsigned char oldkey[MAXMKLEN];
	unsigned char newkey[MAXMKLEN];

	/**/

	key.fldptr = (void *)oldkey;
	oldkeyptr = oldkey;									/* location on old key */

#ifndef NO_CHECKSUM
	p += 1 + sizeof (short) + sizeof (short);
#else
	p += 1;
#endif

	getshort(fieldnum, p);
#ifndef NO_WORD_CONVERSION
	fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	while (fieldnum != -1 && fieldnum <= fnum)	/* end of the line */
	{
		newfieldnum = fieldnum;
	   dptr = &datalist[fieldnum];
	   key.fldtype = dptr->fldtype;
	   key.keytype = dptr->keytype;
	   key.fldindex = dptr->fldindex;
		if (dptr->fldlen < sizeof(oldkey))
		   key.fldlen = dptr->fldlen;
		else
			key.fldlen = sizeof(oldkey);			/* limit key length to internal buffer size */
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
			ret = crtsegkey(psp, buf, datalist, newfieldnum, oldkey);
		   if (ret != CCOK)
		      return(ret);	/* error building segmented key */

			p = fldmoveup(psp, p, &key);					/* move up */
			getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
			/* make sure old key is in native byte ordering before adding */
			cikeyflip(psp, key.fldindex, oldkeyptr, key.keytype, psp->wordorder, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
		}
	   if (	(dptr->keytype == BINARYIND)	  /* binary key item */
			  												/* or custom key item */
			|| ((dptr->keytype >= CUST1IND) && (dptr->keytype <= MAXINDTYP)))
		{
			/* build a binary key which has 1 byte length followed by value */
			oldkeyptr = (unsigned char *)p+sizeof(short); /* field with 2 byte length */
			getshort(fldlen, oldkeyptr);			/* length of binary field */
#ifndef NO_WORD_CONVERSION
			fldlen = inteltos(fldlen, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
			if (fldlen >= MAXKLEN)
				fldlen = MAXKLEN - 1;				/* limit length of binary key */
			oldkeyptr += sizeof(short);			/* pnt to binary key value */
			oldkey[0] = (unsigned char) fldlen;	/* length byte for key */
			blockmv(oldkey+1, oldkeyptr, fldlen);	/* move field to key buffer */
	      p = fldmoveup(psp, p, &key);					/* move up */
			getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
		}
		else
		{
			p = fieldtrans(psp, p, &key);				/* trans key and move up */
			getshort(fieldnum, p);					/* next field number */
#ifndef NO_WORD_CONVERSION
			fieldnum = inteltos(fieldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
		}

		oldkeyptr = oldkey;									/* location on old key */
		newkeyptr = newkey;							/* location of new key */

		/* if not a deleted field, remove the new key value */
		if (!DELFLD(dptr->fldtype))
		{
			/* build new key from datalist */
			ret = dsetkey(psp, key.fldindex, datalist, newfieldnum, newkey);
			if (ret != CCOK)
				return(ret);				/* error building new key */

			/* if key not changed, skip update */
			if (cikeycmp(psp, key.fldindex, key.keytype, oldkeyptr, newkeyptr) == 0)
				continue;

#ifndef NO_WORD_CONVERSION
			psp->keyorder = INTELFMT;
#endif
			psp->keyn = key.fldindex;
			psp->key = (void *)newkeyptr;
			psp->rec = rec;
			ret = _cdelete(psp);
		}

		/* re-add the old key value */
		if ((ret == CCOK) || (ret == FAIL))
		{
#ifndef NO_WORD_CONVERSION
			psp->keyorder = INTELFMT;
#endif
		   if (key.dupflag == DUPKEY)
		      ret = cdupadd(psp, key.fldindex, oldkeyptr, rec, "", 0);
		   else
		      ret = cidunqadd(psp, key.fldindex, oldkeyptr, rec, "", 0);
		}
	}

	return(ret);
}

/* transfer field from buffer to location indicated by "field" */
char * FDECL fieldtrans(psp, p, field)
struct passparm PDECL *psp;
char PDECL *p;
DATALIST PDECL *field;
{
	char *strptr;
	short sval;
	int ival;
	long lval;
	float fval;
	double dval;
	char fixbuf[FIXINDLEN];

	p += sizeof (short);	/* field number */
	if (FLDTYPE(field->fldtype) == BINARYFLD)
	{
		getshort(field->fldlen, p);
#ifndef NO_WORD_CONVERSION
		field->fldlen = inteltos(field->fldlen, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	   p += sizeof (short);
	}

	switch (FLDTYPE(field->fldtype))
	{
		case STRINGFLD:	/* string */
			if (field->fldptr)
			{
				strptr = (char *)field->fldptr;
				strncpy(strptr, p, field->fldlen);
				*(strptr + field->fldlen - 1) = '\0';
			}
			p += strlen(p) + 1;
			break;

		case BINARYFLD:	/* variable number of bytes */
			if (field->fldptr)
			{
			   blockmv(field->fldptr, p, field->fldlen);
			}
		   p += field->fldlen;
		   break;

		case FIXFLD:		/* fixed number of bytes */
			if (field->fldptr)
			{
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
				   blockmv(field->fldptr, p, FIXINDLEN);
				}
				else
				{
				   blockmv(fixbuf, p, FIXINDLEN);
					cicustflip(psp, fixbuf, FIXIND, psp->wordorder, INTELFMT);
				   blockmv(field->fldptr, fixbuf, FIXINDLEN);
				}
#else
			   blockmv(field->fldptr, p, FIXINDLEN);
#endif	/* !NO_WORD_CONVERSION */
			}
		   p += FIXINDLEN;
		   break;

		case INTFLD:	/* integer */
			if (field->fldptr)
			{
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movint(field->fldptr, p);
				}
				else
				{
					movint(&ival, p);
					ival = inteltoi(ival, psp->wordorder);
					movint(field->fldptr, &ival);
				}
#else
				movint(field->fldptr, p);
#endif	/* !NO_WORD_CONVERSION */
			}
		   p += sizeof (int);
		   break;

		case SHORTFLD:	/* short integer */
		case USHORTFLD:	/* unsigned short integer */
			if (field->fldptr)
			{
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movshort(field->fldptr, p);
				}
				else
				{
					movshort(&sval, p);
					sval = inteltos(sval, psp->wordorder);
					movshort(field->fldptr, &sval);
				}
#else
				movshort(field->fldptr, p);
#endif	/* !NO_WORD_CONVERSION */
			}
		   p += sizeof (short);
		   break;

		case LONGFLD:	/* long */
			if (field->fldptr)
			{
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movlong(field->fldptr, p);
				}
				else
				{
					movlong(&lval, p);
					lval = inteltol(lval, psp->wordorder);
					movlong(field->fldptr, &lval);
				}
#else
				movlong(field->fldptr, p);
#endif	/* !NO_WORD_CONVERSION */
			}
		   p += sizeof (long);
		   break;

		case FLOATFLD:	/* float */
			if (field->fldptr)
			{
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movfloat(field->fldptr, p);
				}
				else
				{
					movfloat(&fval, p);
					inteltof(&fval, psp->wordorder);
					movfloat(field->fldptr, &fval);
				}
#else
				movfloat(field->fldptr, p);
#endif	/* !NO_WORD_CONVERSION */
			}
		   p += sizeof (float);
		   break;

		case DOUBLEFLD:	/* double */
			if (field->fldptr)
			{
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movdouble(field->fldptr, p);
				}
				else
				{
					movdouble(&dval, p);
					inteltod(&dval, psp->wordorder);
					movdouble(field->fldptr, &dval);
				}
#else
				movdouble(field->fldptr, p);
#endif	/* !NO_WORD_CONVERSION */
			}
		   p += sizeof (double);
		   break;
	}

	return(p);
}


/* advance pointer past field in buffer */
char * FDECL fldmoveup(psp, p, dptr)
struct passparm PDECL *psp;
char PDECL *p;
DATALIST PDECL *dptr;		/* pointer to current datalist item */
{
	short fldlen;

	p += sizeof (short);					/* move past field number */

	if (FLDTYPE(dptr->fldtype) == BINARYFLD)
	{
		getshort(fldlen, p);
#ifndef NO_WORD_CONVERSION
		fldlen = inteltos(fldlen, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	   p += sizeof (short);				/* move past binary field (incl length) */
	   p += fldlen;	
	   return(p);
	}

	switch (FLDTYPE(dptr->fldtype))
	{
		case STRINGFLD:	/* string */
		   p += strlen(p) + 1;
		   break;
		case FIXFLD:	/* fixed binary "mid-sized" field */
		   p += FIXINDLEN;
		   break;
		case INTFLD:	/* integer */
		   p += sizeof (int);
		   break;
		case SHORTFLD:		/* short integer */
		case USHORTFLD:	/* unsigned short */
		   p += sizeof (short);
		   break;
		case LONGFLD:	/* long */
		   p += sizeof (long);
		   break;
		case FLOATFLD:	/* float */
		   p += sizeof (float);
		   break;
		case DOUBLEFLD:	/* double */
		   p += sizeof (double);
		   break;
	}

	return(p);
}


/* build data record in buffer */
int FDECL builddrec(psp, buf, datalist, idbyte, maxlen)
struct passparm PDECL *psp;
char PDECL *buf;		/* where to put it */
DATALIST PDECL *datalist;	/* defined data list */
int idbyte;		/* id byte to store with it */
int maxlen;
{
	short i;
	short sval;
	int ival;
	long lval;
	float fval;
	double dval;
	char *p;
	int store;
	int j;
	char fixbuf[FIXINDLEN];

#ifndef NO_CHECKSUM
	short checksum;
#endif	/* !NO_CHECKSUM */

	p = buf;

#ifndef NO_CHECKSUM
	checksum = (short)checkdlist(datalist);

#ifndef NO_WORD_CONVERSION
	checksum = stointel(checksum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	ciputshort(p, checksum);
	p += sizeof (short);		/* advance ptr */
	p += sizeof (short);		/* advance ptr */
#endif	/* !NO_CHECKSUM */

	*p++ = (char) idbyte;
	for (i = 0; datalist->fldtype != ENDLIST; i++, datalist++)
	{
		if ((int)(p - buf) > maxlen)
			return(-1);		/* overflow */

		if (DELFLD(datalist->fldtype))
			store = 0;							/* never store if deleted field */
		else
		if (datalist->keytype != NONKEY)
			store = 2;							/* always store if key field */
		else
		{
			store = 0;	/* dont store unless non-zero data in field */
			switch (datalist->fldtype)	/* check for null field */
			{
				case STRINGFLD:	/* string */
					if (datalist->fldptr && *((char *)datalist->fldptr))	/* non null */
						store = 1;  	      	
					break;
				case BINARYFLD:	/* number of bytes */
					if (datalist->fldptr && datalist->fldlen > 0)
						store = 1;
					break;
				case FIXFLD:	/* integer */
					if (datalist->fldptr)
					{
						for (j = 0; j < FIXINDLEN; j++)
						{
							if (*(((char *)datalist->fldptr)+j))
							{
								store = 1;
								break;
							}
						}
					}
					break;
				case INTFLD:	/* integer */
					if (datalist->fldptr && retint(datalist->fldptr))
						store = 1;
					break;
				case SHORTFLD:	/* integer */
				case USHORTFLD:	/* integer */
					if (datalist->fldptr && retshort(datalist->fldptr))
						store = 1;
					break;
				case LONGFLD:	/* long */
					if (datalist->fldptr && retlong(datalist->fldptr))
						store = 1;
					break;
				case FLOATFLD:	/* float */
					if (datalist->fldptr && (retfloat(datalist->fldptr) != 0))
						store = 1;
					break;
				case DOUBLEFLD:	/* double */
					if (datalist->fldptr && (retdouble(datalist->fldptr) != 0))
						store = 1;
			   	break;
			}
		}

		if (store == 0)	/* dont store */
			continue;

#ifndef NO_WORD_CONVERSION
		sval = stointel(i, psp->wordorder);
		ciputshort(p, sval);						/* put field number in rec */
#else
		ciputshort(p, i);							/* put field number in rec */
#endif	/* !NO_WORD_CONVERSION */
	   p += sizeof (short);						/* advance ptr */
	   if (datalist->fldtype == BINARYFLD)	/* binary field uses len */
	   {
#ifndef NO_WORD_CONVERSION
			sval = stointel(datalist->fldlen, psp->wordorder);
			ciputshort(p, sval);
#else
			ciputshort(p, datalist->fldlen);
#endif	/* !NO_WORD_CONVERSION */
	      p += sizeof (short);		/* advance ptr */
	   }

	   switch (FLDTYPE(datalist->fldtype))
	   {
			case STRINGFLD:	/* string */
				strcpy(p, datalist->fldptr);
				p += strlen(p) + 1;
				break;
			case BINARYFLD:	/* number of bytes */
				blockmv(p, datalist->fldptr, datalist->fldlen);
				p += datalist->fldlen;
				break;

			case FIXFLD:	/* number of bytes */
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					blockmv(p, datalist->fldptr, FIXINDLEN);
				}
				else
				{
				   blockmv(fixbuf, datalist->fldptr, FIXINDLEN);
					cicustflip(psp, fixbuf, FIXIND, INTELFMT, psp->wordorder);
					blockmv(p, fixbuf, FIXINDLEN);
				}
#else
				blockmv(p, datalist->fldptr, FIXINDLEN);
#endif	/* !NO_WORD_CONVERSION */
				p += FIXINDLEN;
				break;

			case SEGFLD:		/* segment field marker */
				break;			/* do not put any info in seg field */

			case INTFLD:		/* integer */
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movint(p, datalist->fldptr);
				}
				else
				{
					movint(&ival, datalist->fldptr);
					ival = inteltoi(ival, psp->wordorder);
					movint(p, &ival);
				}
#else
				movint(p, datalist->fldptr);
#endif	/* !NO_WORD_CONVERSION */
				p += sizeof (int);
				break;

			case SHORTFLD:	/* short integer */
			case USHORTFLD:	/* short integer */
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movshort(p, datalist->fldptr);
				}
				else
				{
					movshort(&sval, datalist->fldptr);
					sval = inteltos(sval, psp->wordorder);
					movshort(p, &sval);
				}
#else
				movshort(p, datalist->fldptr);
#endif	/* !NO_WORD_CONVERSION */
				p += sizeof (short);
				break;

			case LONGFLD:	/* long */
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movlong(p, datalist->fldptr);
				}
				else
				{
					movlong(&lval, datalist->fldptr);
					lval = inteltol(lval, psp->wordorder);
					movlong(p, &lval);
				}
#else
				movlong(p, datalist->fldptr);
#endif	/* !NO_WORD_CONVERSION */
				p += sizeof (long);
				break;

			case FLOATFLD:	/* float */
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movfloat(p, datalist->fldptr);
				}
				else
				{
					movfloat(&fval, datalist->fldptr);
					ftointel(&fval, psp->wordorder);
					movfloat(p, &fval);
				}
#else
				movfloat(p, datalist->fldptr);
#endif	/* !NO_WORD_CONVERSION */
				p += sizeof (float);
				break;

			case DOUBLEFLD:	/* double */
#ifndef NO_WORD_CONVERSION
				if (	(INTELFMT && isintel(psp->wordorder))
						|| (!INTELFMT && !isintel(psp->wordorder)) )
				{
					movdouble(p, datalist->fldptr);
				}
				else
				{
					movdouble(&dval, datalist->fldptr);
					dtointel(&dval, psp->wordorder);
					movdouble(p, &dval);
				}
#else
				movdouble(p, datalist->fldptr);
#endif	/* !NO_WORD_CONVERSION */
				p += sizeof (double);
				break;
		}
	}			/* end of for loop storing field values in buffer */

	ciputshort(p, (short)-1);							/* record terminator */
	p += sizeof (short);								/* last field - just in case */
#ifndef NO_CHECKSUM
	checksum = (short)chksumdata(buf, (int)(p - buf));	/* calc rec checksum */
#ifndef NO_WORD_CONVERSION
	sval = stointel(checksum, psp->wordorder);
	ciputshort((buf + sizeof (short)), sval);	/* store checksum */
#else
	ciputshort((buf + sizeof (short)), checksum);	/* store checksum */
#endif	/* !NO_WORD_CONVERSION */
#endif	/* !NO_CHECKSUM */

	return((int)(p - buf));								/* return record length */
}


/* move data record from buffer into locations in datalist */
int FDECL putdrec(buf, datalist, psp, len)
char PDECL *buf;				/* where it is */
DATALIST PDECL *datalist;	/* datalist to store into */
CFILE PDECL *psp;				/* for id byte */
int len;					/* length of found data */
{
	char *p;
	int flds;
#ifndef NO_CHECKSUM
	short checks1, checks2;
#endif
	short fldnum;
	DATALIST *dptr;
	short lastfld = -1;		/* ADDED 4.1C */

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
#endif	/* !NO_CHECKSUM */
	
	flds = nulldlist(datalist);	/* null datalist */

#ifndef NO_CHECKSUM
	p = buf + sizeof (short) + sizeof (short);
#else
	p = buf;
#endif	/* !NO_CHECKSUM */

	psp->idbyte = *p++;	/* set */

	getshort(fldnum, p);
#ifndef NO_WORD_CONVERSION
	fldnum = inteltos(fldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	while (fldnum != -1)	/* not end */
	{
	   dptr = &datalist[fldnum];
#ifdef REMOVED
	   if (fldnum < flds)	/* ok */
	      p = fieldtrans(psp, p, dptr);
	   else
 	      p = fldmoveup(psp, p, dptr);
#endif	/* REMOVED */

		/* NEW WAY - 4.1c */
	   if (fldnum < flds && fldnum >= 0 && fldnum > lastfld)	/* ok */
	      p = fieldtrans(psp, p, dptr);
	   else
		   return(CHKSUMERR);		/* Corrupted Record Error */
		lastfld = fldnum;

		getshort(fldnum, p);
#ifndef NO_WORD_CONVERSION
	fldnum = inteltos(fldnum, psp->wordorder);
#endif	/* !NO_WORD_CONVERSION */
	}
	return(CCOK);
}


/* set fields pointed to by datalist to null or zero */
int FDECL nulldlist(datalist)
DATALIST PDECL *datalist;
{
	int i;
	char *strptr;			/* string type pointer */

	for (i = 0; datalist->fldtype != ENDLIST; i++, datalist++)
	{	
		if (!datalist->fldptr)
			continue;				/* skip fields with null field pointer */

		/* set field to null string of zero value */
	   switch (FLDTYPE(datalist->fldtype))
	   {
	   case STRINGFLD:	/* string */
			strptr = datalist->fldptr;
	      *strptr = '\0';  	      	
	      break;
	   case FIXFLD:	/* integer */
				cclear(datalist->fldptr, FIXINDLEN, 0);
	      break;
	   case INTFLD:	/* integer */
				putint(datalist->fldptr, 0);
	      break;
	   case SHORTFLD:	/* integer */
	   case USHORTFLD:	/* integer */
				ciputshort(datalist->fldptr, 0);
	      break;
	   case LONGFLD:	/* long */
				ciputlong(datalist->fldptr, 0L);
	      break;
	   case FLOATFLD:	/* float */
				putfloat(datalist->fldptr, (float)0);
	      break;
	   case DOUBLEFLD:	/* double */
				putdouble(datalist->fldptr, (double)0);
	      break;
	   }
	}
	return(i);
}


int FDECL checkdlist(datalist)	/* do a checksum of datalist */
DATALIST PDECL *datalist;
{
	short sum = 0;

	while (datalist->fldtype != ENDLIST)
	{
	   sum += FLDTYPE(datalist->fldtype);		/* field type */
	   sum <<= 1;				/* shift left */
	   sum += datalist->keytype;		/* key type */
	   sum <<= 1;				/* shift left */
	   sum += datalist->dupflag;		
	   sum <<= 1;				/* shift left */
	   sum += datalist->fldindex;
	   sum <<= 1;				/* shift left */
	   datalist++;		  /* advance */
	}
	return(sum);
}

#ifndef NO_CHECKSUM
int FDECL chksumdata(buf, len)	/* do a checksum of data record */
char PDECL *buf;		/* beginning of data record */
int len;					/* length of found data */
{
	register short sum;
	register int i;
	/**/

	for (sum = 0, i = 2 * sizeof (short); i < len; i++)
	{
	   sum += *(buf + i);   
	   sum <<= 1;
	}
	return((int) sum);
}


/* test the checksum of data record - returns CCOK if good, FAIL if bad */
int FDECL tstchksumdata(buf, len, testval)
char PDECL *buf;	/* beginning of data record */
int len;				/* length of found data */
short testval;		/* value of checksum stored in record to test against */
{
	register short ssum;			/* signed char sum */
	register short usum;			/* unsighed char sum */
	register int i;				/* offset into record */
	register short uchar;		/* unsigned char value */
	register short schar;		/* signed char value */
	register int ret;				/* returned result */

	/**/

	for (usum = 0, ssum = 0, i = 2 * sizeof (short); i < len; i++)
	{
		/* get character to add to checksum */
		uchar = *(buf + i);		/* extract character, signed or unsigned */
		schar = uchar;

		uchar &= 0xFF;				/* force unsigned char value */
		if ((schar & 0x80) && !(schar & 0xFF00))
			schar |= 0xFF;			/* force signed char value */

		/* do unsigned checksum */
	   usum += uchar;   
	   usum <<= 1;

		/* do signed checksum */
	   ssum += schar;
	   ssum <<= 1;
	}

	if ((testval == usum) || (testval == ssum))
		ret = CCOK;
	else
		ret = FAIL;

	return(ret);
}
#endif	/* NO_CHECKSUM */


/* validate datalist for read/write operations */
int FDECL chkrwdlist(psp, datalist)
struct passparm PDECL *psp;
DATALIST PDECL *datalist;
{
	while (datalist->fldtype != ENDLIST)
	{
	   if (chkftype(datalist->fldtype))
	      return(BADFLDTYP);
	   if (chkktype(psp, datalist->fldindex, datalist->keytype))
	      return(BADKEYTYP);
	   if (chkdflag(datalist->dupflag))
	      return(BADDUPFLG);
	   if (chkflen(datalist->keytype, datalist->fldlen))
	      return(BADFLDLEN);
	   if (FLDTYPE(datalist->fldtype) == STRINGFLD
			&& datalist->fldptr
			&& (((int)strlen(datalist->fldptr) + 1) > (int)datalist->fldlen))
	      return(BADSTRLEN);
	   if (chkfindex(datalist->fldindex))
	      return(BADFLDINX);
	   datalist++;
	}
	return(CCOK);
}


/* validate datalist for read only operations */
int FDECL chkrdlist(psp, datalist)	
struct passparm PDECL *psp;
DATALIST PDECL *datalist;
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


/* check for valid field type */
int FDECL chkftype(type)
int type;
{
	int fldtype;

	fldtype = FLDTYPE(type);		/* strip off deleted field flag */

	if (	fldtype != ENDLIST 
		&& fldtype != STRINGFLD 
		&& fldtype != BINARYFLD 
		&& fldtype != FIXFLD 
		&& fldtype != SEGFLD 
		&& fldtype != INTFLD
		&& fldtype != SHORTFLD 
		&& fldtype != USHORTFLD 
		&& fldtype != LONGFLD
		&& fldtype != FLOATFLD
		&& fldtype != DOUBLEFLD)
	   return(-1);
	else
           return(0);
}


/* check for valid key type */
int FDECL chkktype(psp, keyn, type)
struct passparm *psp;
int keyn;
int type;
{
	if	(	(type == (int)psp->indtyp[keyn])
		|| (type == NONKEY)
		|| (type == ENDLIST)
		|| ((type == STRINGFLD) && (psp->indtyp[keyn] == STRINGIND)))
		return(0);

   return(-1);
}


/* check for valid dup flag */
int FDECL chkdflag(flag)	
int flag;
{
	if (flag != DUPKEY && flag != UNQKEY && flag != NONKEY)
	   return(-1);

   return(0);
}


/* check for valid field len */
int FDECL chkflen(type, len)
int type;
int len;
{
	if (	(len < 0)
		|| (len > MAXFLDLEN)
		|| (isindextype(type) && (len > MAXKLEN)))
	   return(-1);

   return(0);
}


/* test to see if type keytype from the datalist is an index type */
int FDECL isindextype(type)
int type;								/* index type code from datalist */
{
	int ret;

	switch(type)
	{
		case NONKEY:
		case ENDLIST:
			ret = 0;				/* not an indexed field in datalist */
			break;

		default:
			ret = 1;				/* is an index type code */
			break;
	}

	return(ret);
}

/* check for valid index # */
int FDECL chkfindex(index)
int index;
{
	if (index < 0 || index > MAXDINDX)
	   return(-1);

   return(0);
}

/* update the data part of a changed record */
int FDECL chgdlist(psp, datalist, rec)
CFILE PDECL *psp;
DATALIST PDECL *datalist;
long rec;
{
	int len;
	int ret;

	len = builddrec(psp, psp->dbufptr, datalist, psp->idbyte, psp->dbuflen);
	if (len == -1)
	   return(BUFOVER);	/* cant go on */

	if (psp->processmode == EXCL)
	{
		ret = __cadd(psp, DATAINDX, "", rec, psp->dbufptr, len, CHANGE);
	}
	else			/* shared mode */
	{
		/* set for lower level routine */
		psp->keyn = DATAINDX;
		psp->key = "";
		psp->rec = rec;
		psp->data = psp->dbufptr;
		psp->dlen = len;

		ret = _cchange(psp);				/* do change */
	}

	return(ret);
}

