/*   dread.c
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

/*
  cidread() is defined as dread() in cindexsb.h.  This function name change
  is for maintaining compatibility with other versions of C-Index.
*/

/* alias is dread() (for compatibility with Lattice compiler) */
int EDECL cidread(psp, datalist)
CFILE *psp;	
DATALIST *datalist;
{
	int ret;
	int cc;

	/**/

	ret = CCOK;

	if (psp->checkflg)
		ret = chkrdlist(psp, datalist);		/* check datalist for read */

	if (ret != CCOK)
	   return(ret);

	if (strtread(psp) != CCOK)
	   return(FILELOCKED);

	/* if currec has changed */
	if ((psp->currec != psp->lockrec) && (psp->lockrec != -1))
	{
		ret = mclrrec(psp);			/* clear current record lock */
		/* error if already unlocked */
		if ((ret == NOCUR) || (ret > CCOK))
		{
			psp->lockrec = -1;		/* reset lockrec to not locked */
			ret = CCOK;
		}
	}
	else
	{
		ret = CCOK;
	}

	if (ret == CCOK)
	{
		psp->srecurs = TRUE;			/* else prevent clearing of record lock */

		if (psp->currec == -1)	/* no current */
		   ret = NOCUR;
	}

	if (ret == CCOK)
	{
		/* locate the data record */
		psp->keyn = DATAINDX;
		psp->key = "";
		psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
		psp->rec = psp->currec;
		psp->data = psp->dbufptr;
		psp->dlen = psp->dbuflen;
		ret = _cfind(psp);
		if (ret == CCOK)
		{
			/* put data into memory locations indicated by datalist */
			if (psp->dlen == psp->retlen)
			   ret = putdrec(psp->dbufptr, datalist, psp, psp->retlen);
			else
				ret = BUFOVER;
		}
		else
		   ret = FAIL;
	}

	psp->srecurs = FALSE;

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
	
	cc = endread(psp);
	if (cc != CCOK)
		ret = cc;

	return(ret);
}


/*
  cidfind() is defined as dfind() in cindexsb.h.  This function name change
  is for maintaining compatibility with other versions of C-Index.
*/

int EDECL cidfind(psp, index, key, keytype, type)		/* alias dfind() */
CFILE *psp;	
int index;		/* index to search on */
KEYPTR key;		/* key to use */
int keytype;		/* for later use */
int type;		/* type of find */
{
	int ret;
	int cc;
	/**/

	if (type != EQUAL)
	{
		if (strtread(psp) != CCOK)
		   return(FILELOCKED);
	}

	/* check for valid key type */
	if (chkktype(psp, index, keytype))
	   ret = BADKEYTYP;
	else
	{
		/* do find operation */
		psp->keyn = index;
		psp->key = key;
		psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
		psp->rec = 0L;
		psp->data = NULL;
		psp->dlen = 0;
		ret = _cfind(psp);
		switch (type)
		{
			case EQUAL:
			   if (ret == KEYMATCH)
			      ret = CCOK;
			   else
			      ret = FAIL;
			   break;
			case GREAT:
					if (ret == GREATER)
						ret = CCOK;
					else
					if ((ret == KEYMATCH) || (ret == CCOK))
					{
						/* if found matching entry, move past duplicates */
						do
						{
							ret = cnext(psp, index, NULL, 0);	/* look forward */
#ifndef NO_WORD_CONVERSION
							cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */

						}
						while ((ret == CCOK) 
								&& (!cikeycmp(	psp,
													index, 
													(psp->indtyp[index] & INDTYPEMASK),
													psp->key,
													key)));
					}
				break;
			case GREATEQ:
			   if (ret == KEYMATCH)
			      ret = CCOK;
			   else if (ret == GREATER)
			      ;		/* no action */
			   else
			      ret = FAIL;
			   break;
			case LESS:
			   ret = cprev(psp, index, "", 0);
			   break;
			case LESSEQ:
			   if (ret == KEYMATCH)
			      ret = CCOK;
			   else if ((ret = cprev(psp, index, "", 0)) == CCOK)
			      ret = LESSTHAN;
			   break;
		}
	}
	
	if (ret == CCOK || ret == GREATER || ret == LESSTHAN)
	   psp->currec = psp->rec;
	else
	   psp->currec = -1;

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
	
	if (type != EQUAL)
	{
		cc = endread(psp);
		if (cc != CCOK)
			ret = cc;
	}

	return(ret);
}


/* sequential search of key index */
int EDECL dseq(psp, index, type)	
CFILE *psp;
int index;		/* index # */
int type;		/* type of sequential read */
{
	int ret = PARMERR;

	switch (type)
	{
	   case NEXT:	/* next */
	      ret = cnext(psp, index, "", 0);	/* get next key */
	      break;
	   case PREV:	/* prev */
	      ret = cprev(psp, index, "", 0);
	      break;
	   case FIRST:	/* first */
	      ret = cfirst(psp, index, "", 0);
	      break;
	   case LAST:	/* last */
	      ret = clast(psp, index, "", 0);
	      break;
	}

	if (ret == CCOK)	/* successful */
	   psp->currec = psp->rec;
	else
	   psp->currec = -1;

#ifndef NO_WORD_CONVERSION
	cipspflip(psp, INTELFMT);
#endif	/* !NO_WORD_CONVERSION */
	
	return(ret);
}

/* return the id byte of current record in psp->idbyte */
int EDECL dgetid(psp)
CFILE *psp;
{
	char buf[10];
	int ret;

	if (psp->currec == -1)	/* no current */
	   return(NOCUR);

	psp->srecurs = TRUE;						/* prevent clearing of record lock */

	psp->keyn = DATAINDX;
	psp->key = "";
	psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
	psp->rec = psp->currec;
	psp->data = buf;
	psp->dlen = 10;
	ret = _cfind(psp);
	if (ret == CCOK)
	{
		/* extract idbyte of record */
#ifndef NO_CHECKSUM
	   psp->idbyte = *(buf + sizeof(short) + sizeof(short));
#else
	   psp->idbyte = *buf;
#endif	/* NO_CHECKSUM */

		psp->srecurs = FALSE;					/* prevent clearing of record lock */
	   return(ret);
	}

	psp->srecurs = FALSE;					/* prevent clearing of record lock */

   return(FAIL);
}

/* read the number of records from the file header */
/* returns the number in psp->rec */
int EDECL dgetcnt(psp)
struct passparm *psp;
{
	return(cgetcnt(psp));
}


/* create a segmented key for adding, deleting, and updating keys */
int FDECL bldsegkey(psp, datalist, segoff, binkey)
CFILE *psp;
DATALIST *datalist;					/* ptr to datalist  */
int segoff;								/* number of field to build */
unsigned char *binkey;				/* buffer to build key in */
{
	int i;							/* loop counter */
	short fldlen;					/* length of a field */
	KEYSEGLIST *seginfo;				/* ptr to segment list */
	unsigned char *entwrkptr;	/* ptr into entry segmented key */
	int entlen;						/* length of entry binary key */
	int entseglen;					/* length of entry key segment */
	unsigned char *srcptr;		/* ptr to source field */
	DATALIST *dptr;
	int maxseglen;					/* limit to segment length set in seglist */

	/**/

	/* build each keytype of segmented key */
	dptr = datalist + segoff;					/* datalist entry for seg key */
	seginfo = psp->segptr[dptr->fldindex];	/* ptr to segment list */
	entwrkptr = binkey + 1;						/* build seg key in buffer */
	entlen = 0;										/* clear cummulative length */
	for (	i = 0, entseglen = 0;
			seginfo[i].keytype != ENDSEGLIST;
			i++)
	{
		/* point to key field in buffer */
	   srcptr = (unsigned char *)datalist[seginfo[i].fldoff].fldptr;
		if (!srcptr)
			return(BADDLIST);			/* no pointer to field */

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
				/* copy limited ASCIIZ string key */
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
				fldlen = datalist[seginfo[i].fldoff].fldlen; /* len of bin fld */
				if (fldlen > maxseglen)								/* limit to maxlen */
					fldlen = (short)maxseglen;
				entseglen = fldlen + 1;							/* length of segment */
				if ((entlen + entseglen) >= MAXKLEN)
					return(BADSTRLEN);							/* key too long */
				*entwrkptr = (unsigned char)fldlen;			/* set key length */
				blockmv(entwrkptr+1, srcptr, fldlen);		/* move field to buf */
				break;

			case FIXIND:
				if ((entlen + FIXINDLEN) >= MAXKLEN)
					return(BADSTRLEN);							/* key too long */
				blockmv(entwrkptr, srcptr, FIXINDLEN);		/* move field to buf */
				entseglen = FIXINDLEN;							/* length of segment */
				break;

			case INTIND:
				if ((entlen + sizeof(int)) >= MAXKLEN)
					return(BADSTRLEN);							/* key too long */
				trnsint(entwrkptr, srcptr);
				entseglen = sizeof(int);			/* length of segment */
				break;

			case SHORTIND:
			case USHORTIND:
				if ((entlen + sizeof(short)) >= MAXKLEN)
					return(BADSTRLEN);							/* key too long */
				trnsshort(entwrkptr, srcptr);
				entseglen = sizeof(short);			/* length of segment */
				break;

			case LONGIND:
				if ((entlen + sizeof(long)) >= MAXKLEN)
					return(BADSTRLEN);							/* key too long */
				trnslong(entwrkptr, srcptr);
				entseglen = sizeof(long);			/* length of segment */
				break;

			case FLOATIND:
				if ((entlen + sizeof(float)) >= MAXKLEN)
					return(BADSTRLEN);							/* key too long */
				trnsfloat(entwrkptr, srcptr);
				entseglen = sizeof(float);			/* length of segment */
				break;

			case DOUBLEIND:
				if ((entlen + sizeof(double)) >= MAXKLEN)
					return(BADSTRLEN);							/* key too long */
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

		entwrkptr += entseglen;			/* skip to next key segment */
		entlen += entseglen;				/* increase total key length */

		if (entlen >= MAXKLEN)
			return(BADSTRLEN);			/* key too long */

	}		/* end of for loop to create each key segment */

	*binkey = (unsigned char)entlen;			/* total length of segmented key */

	return(CCOK);
}


/* set key buffer to key search value based on datalist info */
int EDECL dsetkey(psp, keyn, datalist, fieldnum, keybuf)
struct passparm *psp;				/* file info */
int keyn;								/* build key for index number keyn */
struct flddef *datalist;			/* datalist pointing to key information */
int fieldnum;							/* number of datalist field (or -1) */
KEYPTR keybuf;							/* buffer to hold finished key value */
{
	int i;
	int ret;
	char *keyptr;
	unsigned char *bufptr = (unsigned char *)keybuf;
	int fldtype;

	/**/

	if (psp->checkflg)
		ret = chkrdlist(psp, datalist);		/* check datalist for read */
	else
		ret = CCOK;									/* assume good datalist */

	if (ret == CCOK)
	{
		if (fieldnum < 0)
		{
			/* locate the datalist item with keyn */
			for (i=0; datalist[i].fldtype != ENDLIST; i++)
			{
				if (datalist[i].fldindex == (char)keyn)
					break;
			}

			fieldnum = i;
		}

		if (datalist[fieldnum].fldtype == ENDLIST)
			ret = BADDLIST;						/* keyn not found in datalist */
		else
		{
			keyptr = (char *)datalist[fieldnum].fldptr;
			fldtype = FLDTYPE(datalist[fieldnum].fldtype);
			switch (fldtype)
			{
				case STRINGFLD:							/* copy string to buffer */
					if (!keyptr)
						ret = BADDLIST;			/* no pointer to data */
					else
						strcpy((char *)bufptr, (char *)keyptr);
					break;

				case BINARYFLD:	/* add length byte and block move binary key */
					if (!keyptr)
						ret = BADDLIST;			/* no pointer to data */
					else
					{
						*bufptr = (unsigned char)datalist[fieldnum].fldlen;
						blockmv(bufptr+1, keyptr, datalist[fieldnum].fldlen);
					}
					break;

				case FIXFLD:		/* block move fixed length binary key */
					if (!keyptr)
						ret = BADDLIST;			/* no pointer to data */
					else
						blockmv(bufptr, keyptr, FIXINDLEN);
					break;

				case SEGFLD:
					ret = bldsegkey(psp, datalist, fieldnum, (unsigned char *)keybuf);
					break;

				case INTFLD:
					if (!keyptr)
						ret = BADDLIST;			/* no pointer to data */
					else
						trnsint(keybuf, keyptr);
					break;

				case SHORTFLD:
				case USHORTFLD:
					if (!keyptr)
						ret = BADDLIST;			/* no pointer to data */
					else
						trnsshort(keybuf, keyptr);
					break;

				case LONGFLD:
					if (!keyptr)
						ret = BADDLIST;			/* no pointer to data */
					else
						trnslong(keybuf, keyptr);
					break;

				case FLOATFLD:
					if (!keyptr)
						ret = BADDLIST;			/* no pointer to data */
					else
						trnsfloat(keybuf, keyptr);
					break;

				case DOUBLEFLD:
					if (!keyptr)
						ret = BADDLIST;			/* no pointer to data */
					else
						trnsdbl(keybuf, keyptr);
					break;

				case ENDLIST:
					ret = BADDLIST;				/* invalid datalist */
					break;

				default:
					ret = BADDLIST;				/* invalid datalist */
			}
		}
	}

	return(ret);
}

