/*     curkey.c  -  current key information functions
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

INT32 FDECL fgetrcval(psp, keyn)
CFILE *psp;
int keyn;
{
	INT32 ret;
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		ret = infoadr->currcval;
	else
		ret = 0;							/* default on error */

	unlockcuradr(psp, keyn);		/* release lock on memory */
	return(ret);
}

/* return current databyte for index keyn */
int FDECL fgetdbyte(psp, keyn)
CFILE *psp;
int keyn;
{
	int ret;
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		ret = infoadr->curdbyte;
	else
		ret = 0;							/* default on error */

	unlockcuradr(psp, keyn);		/* release lock on memory */
	return(ret);
}

/* get current key number */
int FDECL fgetkeynum(psp, keyn)
CFILE *psp;
int keyn;
{
	int ret;
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		ret = infoadr->keynum;
	else
		ret = 0;							/* default on error */

	unlockcuradr(psp, keyn);		/* release lock on memory */
	return(ret);
}

/* get current node number */
NDEPTR FDECL fgetndenum(psp, keyn)
CFILE *psp;
int keyn;
{
	NDEPTR ret;
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		ret = infoadr->ndenum;
	else
		ret = NULLNDE;					/* default on error */

	if (ret > psp->endofile)
		ret = NULLNDE;					/* error */

	unlockcuradr(psp, keyn);		/* release lock on memory */
	return(ret);
}

/* get current index number */
int FDECL fgetindx(psp, keyn)
CFILE *psp;
int keyn;
{
	int ret;
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		ret = infoadr->curindx;
	else
		ret = -1;						/* default on error */

	unlockcuradr(psp, keyn);		/* release lock on memory */
	return(ret);
}

/* get delete status flag */
int FDECL fgetdelflag(psp, keyn)
CFILE *psp;
int keyn;
{
	int ret;
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		ret = infoadr->delflag;
	else
		ret = FALSE;						/* default on error */

	unlockcuradr(psp, keyn);		/* release lock on memory */
	return(ret);
}

/* get update key info flag */
int FDECL fgetupdkeyinfo(psp, keyn)
CFILE *psp;
int keyn;
{
	int ret;
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		ret = infoadr->updkeyinfo;
	else
		ret = TRUE;						/* default on error */

	unlockcuradr(psp, keyn);		/* release lock on memory */
	return(ret);
}


KEYSEGLIST * FDECL getcurseg(psp, keyn)
CFILE *psp;
int keyn;
{
	KEYSEGLIST *ret;
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		ret = infoadr->segptr;
	else
		ret = 0;							/* default on error */

	unlockcuradr(psp, keyn);		/* release lock on memory */
	return(ret);
}

/* set currectn record number value */
void FDECL fputrcval(psp, keyn, value)
CFILE *psp;
int keyn;
INT32 value;
{
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		infoadr->currcval = value;

	unlockcuradr(psp, keyn);		/* release lock on memory */
}


/* save current key information */
void FDECL cisavecurinfo(psp, keyn, savebuf)
CFILE *psp;
int keyn;
CURINFOPTR savebuf;
{
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		blockmv((char *)savebuf, (char *)infoadr, sizeof(CURINFO));

	unlockcuradr(psp, keyn);		/* release lock on memory */
}


/* restore current key information */
void FDECL cirestcurinfo(psp, keyn, savebuf)
CFILE *psp;
int keyn;
CURINFOPTR savebuf;
{
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		blockmv((char *)infoadr, (char *)savebuf, sizeof(CURINFO));

	unlockcuradr(psp, keyn);		/* release lock on memory */
}



/*****************************************/

/* set current databyte value for keyn */
void FDECL fputdbyte(psp, keyn, value)
CFILE *psp;
int keyn;
int value;
{
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		infoadr->curdbyte = (char)value;

	unlockcuradr(psp, keyn);		/* release lock on memory */
}

/* save current key number */
void FDECL fputkeynum(psp, keyn, num)
CFILE *psp;
int keyn;
int num;
{
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		infoadr->keynum = num;

	unlockcuradr(psp, keyn);		/* release lock on memory */
}

/* save current node number */
void FDECL fputndenum(psp, keyn, nde)
CFILE *psp;
int keyn;
NDEPTR nde;
{
	CURINFOPTR infoadr;

	if (nde > psp->endofile)
		nde = NULLNDE;						/* error */

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		infoadr->ndenum = nde;

	unlockcuradr(psp, keyn);		/* release lock on memory */
}

/* save current index number */
void FDECL fputindx(psp, keyn, indx)
CFILE *psp;
int keyn;
int indx;
{
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		infoadr->curindx = indx;

	unlockcuradr(psp, keyn);		/* release lock on memory */
}

/* save delete status flag */
void FDECL fputdelflag(psp, keyn, flag)
CFILE *psp;
int keyn;
int flag;
{
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		infoadr->delflag = (char)flag;

	unlockcuradr(psp, keyn);		/* release lock on memory */
}

/* save update key info flag */
void FDECL fputupdkeyinfo(psp, keyn, flag)
CFILE *psp;
int keyn;
int flag;
{
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		infoadr->updkeyinfo = (char)flag;

	unlockcuradr(psp, keyn);		/* release lock on memory */
}

/* save current segment info ptr */
void FDECL fputcurseg(psp, keyn, seginfoptr)
CFILE *psp;
int keyn;
KEYSEGLIST *seginfoptr;
{
	CURINFOPTR infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		infoadr->segptr = seginfoptr;

	unlockcuradr(psp, keyn);		/* release lock on memory */
}



/* adjust current ptrs after altered */
/* note - assumes that curkeyn and curndenum are set already */
void FDECL ciadjust(psp, flag)
struct passparm *psp;
int flag;					/* add or delete */
{
	int i;
	NDEPTR nde;				/* current node number */
	int keynm;				/* current key number */
	CURINFOPTR infoptrkeyn;		/* ptr to current key info */
	CURINFOPTR infoptri;			/* ptr to current key info */

	infoptrkeyn = lockcuradr(psp, psp->keyn);	/* get adr of this info */
	if (!infoptrkeyn)
		return;											/* error */
	nde = infoptrkeyn->ndenum;						/* to save time */
	keynm = infoptrkeyn->keynum;					/* "" */
	unlockcuradr(psp, psp->keyn);					/* release lock on this */

	if (flag == ADDJST) 	/* after add */
	{
	   for (i = psp->keyn; i < NUMINDXS; i++)   	/* for every index */
	   {
			if (!tstcuradr(psp, i))
				continue;				/* skip indexes that have not been used */

			infoptri = lockcuradr(psp, i);
			if (infoptri->ndenum == nde) 	/* if node is same */ 
			{
				/* if keynum > key we added */
				if (infoptri->keynum > keynm)
					infoptri->keynum++;		/* bump up by one */
				else
				{
					/* if lesser index and >= key num, bring up straggler */
					if (	(psp->keyn < infoptri->curindx)
						&& (keynm >= infoptri->keynum))
						infoptri->keynum++;	/* do it */
				}
			}

			unlockcuradr(psp, i);
	   }
	}
	else		/* must be delete */
	{
		infoptrkeyn = lockcuradr(psp, psp->keyn);
		if (!infoptrkeyn)
			return;											/* error */
	   infoptrkeyn->delflag = DELON;		 			/* set flag */
		unlockcuradr(psp, psp->keyn);					/* release lock on this */

	   for (i = psp->keyn; i < NUMINDXS; i++)  /* for every index */
		{
			if (!tstcuradr(psp, i))
				continue;				/* skip indexes that have not been used */

			infoptri = lockcuradr(psp, i);
	      if (	(infoptri->ndenum == nde)
				&& (infoptri->keynum >= keynm)
				&& (infoptri->keynum > -1))			/* dont move a -1 back */ 
	      {													/* if need to alter */
	         infoptri->keynum--;						/* move down */
	      }

			unlockcuradr(psp, i);
		}
	}
}

/* adjust current ptrs after split */
void FDECL splitadj(psp, ndea, ndeb, cnt)
struct passparm *psp;		/* psp */
NDEPTR ndea;					/* node number of node a */
NDEPTR ndeb;					/* node number of node b */
int cnt;							/* count of node a */
{
	int i;

	for (i = 1; i < NUMINDXS; i++) 	/* for every index */
	{
		if (!tstcuradr(psp, i))
			continue;				/* skip indexes that have not been used */

	   if ((getndenum(psp, i) == ndea) && (getkeynum(psp, i) >= cnt))
	   {		/* if key is >= key we just inserted */
	      putndenum(psp, i, ndeb);							/* its now in b */
	      putkeynum(psp, i, (getkeynum(psp, i) - cnt));/* and count is minus a's total */
	   }
	}
}
