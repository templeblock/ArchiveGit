/*   dwrite.c - multi-key write functions
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

/* add one data record and its keys */
int EDECL dadd(psp, datalist)
CFILE *psp;
DATALIST *datalist;
{
	long rec1;			/* from file */
	int len;	/* from build keylist & data record */
	int ret;
	int cc;

	/**/	

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	ret = CCOK;

	if (psp->checkflg)
	   ret = chkrwdlist(psp, datalist);		/* check datalist for read/write */

	if (ret != CCOK)
	   return(ret);

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	rec1 = getnextrec(psp);		/* for data */
	if (rec1 == -1)
	{
		ret = DSKERR;				/* error getting next record number */
	}
	else
	{
		len = builddrec(psp, psp->dbufptr, datalist, psp->idbyte, psp->dbuflen);
		if (len == -1)
		{
			cc = endwrit(psp);
			if (cc != CCOK)
				ret = cc;
			else
				ret = BUFOVER;
		   return(psp->retcde = ret);
		}

#ifdef REMOVED	/* 4.1b 8/5/93 */
		if ((ret = addkeys(psp, psp->dbufptr, rec1, datalist)) != CCOK)
		{
		   delkeylist(psp, psp->dbufptr, rec1, datalist, len);
			   /* delete the keys */
		   cputrec(psp, rec1);	/* put back rec */
		}
		else
		{
		   ret = cdupadd(psp, DATAINDX, "", rec1, psp->dbufptr, len);
		   if (ret == CCOK)
			{
		      psp->currec = rec1;
				/* incriment # of recs in file */
				ret = cgetcnt(psp);
			   if (ret == CCOK)
					ret = csetcnt(psp, psp->rec + 1);
			}
		   else
		      ret = DATAERR;
		}  
#endif	/* REMOVED 4.1b 8/5/93 */

/* ADD 4.1b 8/5/93 */
		ret = dextend(psp, datalist);
		if (ret == CCOK)
		{
			ret = addkeys(psp, psp->dbufptr, rec1, datalist);
			if (ret == CCOK)
			{
			   ret = cdupadd(psp, DATAINDX, "", rec1, psp->dbufptr, len);
			   if (ret == CCOK)
				{
			      psp->currec = rec1;
					/* incriment # of recs in file */
					ret = cgetcnt(psp);
				   if (ret == CCOK)
						ret = csetcnt(psp, psp->rec + 1);
				}
			   else
			   if (ret == FAIL)
			      ret = DATAERR;
			}  

			if (ret != CCOK)
			{
			   delkeylist(psp, psp->dbufptr, rec1, datalist, len);
				   /* delete the keys */
			   cputrec(psp, rec1);	/* put back rec */
			}
		}
/* end ADD 4.1b 8/5/93 */
	}

	cc = endwrit(psp);
	if (cc != CCOK)
		ret = cc;

	return(ret);
}


/* update currrent record */
int EDECL dupdate(psp, datalist)	
CFILE *psp;
DATALIST *datalist;
{
	int ret = 0;
	long rec;
	long oldrec;				/* record # being updated */
	int cc;

	/**/

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if (psp->checkflg)
	{
	   if ((ret = chkrwdlist(psp, datalist)) != CCOK) 
	      return(ret); /* check datalist for read/write */
	}

	if (psp->currec == -1)	/* no current */
	   return(NOCUR);

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	/* in shared mode, record must be locked before deleting */
	if (	(psp->processmode != EXCL) 
		&& (mtstrec(psp) != RECSFLK)
		&& (psp->nolock == FALSE))
	{
		ret = RECNOLK;
		cc = endwrit(psp);
		if (cc != CCOK)
		   ret = cc;
	   	return(psp->retcde = ret);
	}

/* ADD 4.1b 8/5/93 */
	/* ensure enough room in case of splits */
	ret = dextend(psp, datalist);
	if (ret != CCOK)
	{
		endwrit(psp);
		return(ret);			/* if no current record found */
	}
/* end ADD 4.1b 8/5/93 */

	psp->srecurs = TRUE;						/* prevent clearing of record lock */

	/* locate record to be updated */
	oldrec = psp->currec;						/* save record number */
	psp->keyn = DATAINDX;
	psp->key = "";
	psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
	psp->rec = oldrec;
	psp->data = psp->dbufptr;
	psp->dlen = psp->dbuflen;
	cc = _cfind(psp);
	if (cc != CCOK)
	{
		psp->srecurs = FALSE;
		endwrit(psp);
		return(DATAERR);			/* if no current record found */
	}

	rec = psp->rec;
	ret = chgkeylist(psp, psp->dbufptr, rec, datalist, psp->retlen);
	if (ret == CCOK)
	{
	   ret = chgdlist(psp, datalist, rec);		/* add new record */
	      /* note: only serious errors can cause this to fail - 
	      must quit with new keys still in file */
	}

	psp->srecurs = FALSE;
	cc = endwrit(psp);
	if (cc != CCOK)
	   ret = cc;
	return(ret);
}


/* delete current data record and keys */
int EDECL ddelete(psp, datalist)
CFILE *psp;
DATALIST *datalist;
{
	long rec;
	int ret;
	int cc;

	/**/

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if (psp->currec == -1)	/* no current */
	   return(NOCUR);

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	/* in shared mode, record must be locked before deleting */
	if (	(psp->processmode != EXCL) 
		&& (mtstrec(psp) != RECSFLK)
		&& (psp->nolock == FALSE))
	{
		ret = RECNOLK;
		cc = endwrit(psp);
		if (cc != CCOK)
			ret = cc;
	   return(psp->retcde = ret);
	}

	psp->keyn = DATAINDX;
	psp->key = "";
	psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
	psp->rec = psp->currec;
	psp->data = psp->dbufptr;
	psp->dlen = psp->dbuflen;
	psp->srecurs = TRUE;						/* prevent clearing of record lock */
	ret = _cfind(psp);
	psp->srecurs = FALSE;
	if (ret != CCOK)					/* if no current or disk error */
	{
		ret = DATAERR;
	}
	else
	{
		rec = psp->rec;
		ret = delkeylist(psp, psp->dbufptr, psp->rec, datalist, psp->retlen);	/* delete the keys */
		if (ret == CCOK)
			ret = msetlock(psp, DATAINDX);	/* lock record before delete */
		if (ret == CCOK)	
		   ret = cdelcur(psp, DATAINDX);		/* delete data */
		if (ret == CCOK)
		{
			ret = cgetcnt(psp);
		   if (ret == CCOK)
				ret = csetcnt(psp, psp->rec - 1);	/* decr # of recs in file */
		}
		if (ret == CCOK)
		   ret = cputrec(psp, rec);			/* put rec back */

		psp->currec = -1;
		psp->lockrec = -1;
	}

	cc = endwrit(psp);
	if (cc != CCOK)
		ret = cc;

	return(ret);
}


/* multi-user locking for multi-key routines */

/* lock the current multi-key routine */
int EDECL msetrec(psp)
CFILE *psp;		/* open psp */
{
	int ret;
	int cc;
	void *tempdata;					/* temp data ptr storage */
	int tempdlen;						/* temp dlen storage */

	/**/

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if ((cc = strtwrit(psp)) != CCOK)
	   return(cc);

	tempdata = psp->data;			/* save data and dlen before cfind */
	tempdlen = psp->dlen;

	psp->keyn = DATAINDX;
	psp->key = "";
	psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
	psp->rec = psp->currec;
	psp->data = NULL;
	psp->dlen = 0;
	ret = _cfind(psp);
	if (ret != CCOK)
	{
		if (ret >= FAIL)
			ret = NOCUR;			/* translate not found into NOCUR */
	}
	else
	{
		ret = msetlock(psp, DATAINDX);
	}

	if (ret == CCOK)
		psp->lockrec = psp->currec;		/* set currently locked rec # */
	else
		psp->lockrec = -1;					/* error, no locked rec */

	psp->data = tempdata;			/* restore data and dlen */
	psp->dlen = tempdlen;

	cc = endwrit(psp);
	if (cc != CCOK)
		ret = cc;

	return(ret);
}


/* clear lock on current entry */
int EDECL mclrrec(psp)
CFILE *psp;		/* open psp */
{
	int ret;

	/**/

	if (psp->readmode == CRDONLY)
		return(BADWRITE);
	
	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	ret = mclrlock(psp, DATAINDX);
	psp->lockrec = -1;

	return(ret);
}


/* test lock of current entry */
int EDECL mtstrec(psp)
CFILE *psp;		/* open psp */
{
	int ret;
	void *tempdata;
	int tempdlen;

	/**/

	if (checkpsp(psp) != PSPOPEN)	/* not an open psp */
	   return(PSPERR);

	if (psp->lockrec != psp->currec)				/* no currently locked record */
	{
		tempdata = psp->data;			/* save data and dlen before cfind */
		tempdlen = psp->dlen;

		psp->keyn = DATAINDX;
		psp->key = "";
		psp->keyorder = INTELFMT;			/* reset key byte ordering to native */
		psp->rec = psp->currec;
		psp->data = NULL;
		psp->dlen = 0;
		ret = _cfind(psp);
		if (ret != CCOK)
		{
			if (ret >= FAIL)
				ret = NOCUR;			/* translate not found into NOCUR */
		}

		psp->data = tempdata;			/* restore data and dlen */
		psp->dlen = tempdlen;
	}
	else
	{
		ret = CCOK;
	}

	if (ret == CCOK)
		ret = mtstlock(psp, DATAINDX);

	return(ret);
}









