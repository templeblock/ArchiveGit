/*   dfile.c - multi-key file create, open, close
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

/* multi-user multi-key create file */
int EDECL dbcreate(psp, fle, buf, buflen, filemode, bytemode, indexinfo)
CFILE *psp;									/* parameter structure */
char *fle;									/* file name */
void *buf;									/* data record buffer */
int buflen;									/* len of buffer */
int filemode;								/* mode of create (SHARED or EXCL) */
int bytemode;								/* order of bytes in word */
NDXLIST *indexinfo;						/* index info list */
{
	int ret;

	ret = bcreate(psp, fle, filemode, bytemode, indexinfo);
	if (ret == CCOK)
	{
	   psp->currec = -1;			/* no current record */
	   psp->lockrec = -1;		/* no currently locked record */
	   psp->dbufptr = buf;
	   psp->dbuflen = buflen;	/* set psp values */
	}
	return(ret);
}


/* multi-user multi-key open */
int EDECL dbopen(psp, fle, buf, buflen, filemode, rdmode, indexinfo)
CFILE *psp;								/* parameter structure */
char *fle;								/* file name */
void *buf;								/* data record buffer */
int buflen;								/* len of buffer */
int filemode;							/* mode to open file in (SHARED or EXCL) */
int rdmode;								/* CRDWRITE or CRDONLY */
NDXLIST *indexinfo;					/* index info list */
{
	int ret;

	ret = bopen(psp, fle, filemode, rdmode, indexinfo);
	if (ret == CCOK)
	{
	   psp->currec = -1;		/* no current record */
	   psp->lockrec = -1;		/* no currently locked record */
	   psp->dbufptr = buf;
	   psp->dbuflen = buflen;	/* set psp values */
	}

	return(ret);
}


/* 
  cidclose() is defined as dclose() in cndx.h.  This function name change
  is for maintaining compatibility with other versions of C-Index.
*/

int EDECL cidclose(psp)				/* alias dclose() */
CFILE *psp;
{
	return(mclose(psp));
}

int EDECL dmclose(psp)				/* multi-user close */
CFILE *psp;
{
	return(mclose(psp));
}


