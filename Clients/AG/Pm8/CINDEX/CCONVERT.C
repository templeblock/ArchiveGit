/*   cconvert.c  -  convert C-Index/Plus file to new format
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
#include "cindexwk.h"		/* internal header */



int FDECL convisintel()
{
	return(1);				/* converted files are always in intel format */
}

/* converts a short integer from intel format */
short FDECL cvintos(si)
int si;
{
   unsigned short s1,s2,sx;

	sx = (unsigned short)si;

	if (	(INTELFMT && convisintel())
		|| (!INTELFMT && !convisintel()) )
		return(sx);

   s1 = (unsigned short)(sx << 8);
   s2 = (unsigned short)((sx >> 8) & 0x00ff);
   s1 = (unsigned short)(s1 | s2);

   return(s1);
}

/* converts a long integer from intel format */
long FDECL cvintol(lint)
long lint;
{
   unsigned long l1,l2,l3,l4,lres;

	if (	(INTELFMT && convisintel())
		|| (!INTELFMT && !convisintel()) )
		return(lint);

   l1 = lint << 24;
   l2 = (lint & 0x0000ff00) << 8;
   l3 = (lint & 0x00ff0000L) >> 8;
	l4 = (lint >> 24) & 0xff;

   lres = l1 | l2 | l3 | l4;

   return(lres);
}


/* conversion utility functions */

int FDECL convhdrread(psp, buf)				/* reads header record into buf */
struct passparm *psp;
register struct oldhdrrec *buf;		/* where to put it */
{
   struct oldhdrrec1 {
      short number;
      char  ch;
   } header1;
   struct oldhdrrec1 *hdr1;

   struct oldhdrrec2 {
      long number;
      char ch;
   } header2;
   struct oldhdrrec2 *hdr2;

   hdr1 = &header1;
   hdr2 = &header2;

	errcde = fileSeek(psp, 0L, 0, 0L);

	if (!errcde)
	{
	   errcde = ciread(psp, (char *)&buf->ck1, 1);
	}

	if (!errcde)
	{
	   errcde = ciread(psp,(char *) hdr1, 3);
	   buf->hroot = cvintos(hdr1->number);
	   buf->ck2 = hdr1->ch;
	}

	if (!errcde)
	{
	   errcde = ciread(psp,(char *) hdr1, 3);
	   buf->hfirst = cvintos(hdr1->number);
	   buf->ck3 = hdr1->ch;
	}

	if (!errcde)
	{
	   errcde = ciread(psp,(char *) hdr1, 3);
	   buf->hlast = cvintos(hdr1->number);
	   buf->ck4 = hdr1->ch;
	}

	if (!errcde)
	{
	   errcde = ciread(psp,(char *) hdr1, 3);
	   buf->hlevels = cvintos(hdr1->number);
	   buf->ck5 = hdr1->ch;
	}

	if (!errcde)
	{
	   errcde = ciread(psp,(char *) hdr1, 3);
	   buf->hnext = cvintos(hdr1->number);
	   buf->ck6 = hdr1->ch;
	}

	if (!errcde)
	{
	   errcde = ciread(psp,(char *) hdr2, 5);
	   buf->hnxtdrec = cvintol(hdr2->number);
	   buf->ck7 = hdr2->ch;
	}

	if (!errcde)
	{
	   errcde = ciread(psp,(char *) hdr1, 3);
	   buf->hendofile = cvintos(hdr1->number);
	   buf->ck8 = hdr1->ch;
	}

	if (!errcde)
	{
	   errcde = ciread(psp,(char *) hdr2, 4);
	   buf->updatnum = cvintol(hdr2->number);
	}

	return(errcde);
}

/* disk read of node data, returns error code - CCOK if good */
int FDECL convdiskread(psp, nod, nde)
struct passparm *psp;
struct oldnode *nod;		/* where to put information */
NDEPTR nde;		/* node number */
{
	errcde = fileSeek(psp, nde, 1024, 1024L);

	if (errcde == CCOK) 							/* if no seek error */
	   errcde = ciread(psp, (char *)nod, 1024);	/* read into buffer */

	return(errcde);
}


long FDECL convgtkrec(ptr)	/* gets record number after key at ptr */
register char *ptr;		/* pointer to key */
{
	long p;		/* to hold value */

	ptr += strlen(ptr) + 1;		/* get to record number */
	p = 0;							/* avoids the compiler error msg */
	convtrnsrec(&p, ptr);				/* put into long value */
	return(cvintol(p));			/* return record number */
}


int FDECL convgtdbyte(ptr)	/* gets data byte after key at ptr */
register char *ptr;	/* pointer to key */
{
	int dbyte;

	ptr += strlen(ptr) + 1 + SZEREC;	/* get to data byte */
	dbyte = *ptr;
	dbyte &= 0xFF;		/* make sure char is unsigned */
	return(dbyte);		/* return data byte */
}


int FDECL convgoodata(p)	/* checks for existence of data in entry */
register char *p;	/* pointer to key */
{
	int len;

	/* length of entry - key, rec#, databyte, and overhead */
	len = ((int) *p) & 0xFF;	/* no sign extension */
	return(len - strlen(p + 1) - SZEREC - SZEDBYTE - 1); 
}

	
/* add all keys in this node */
int FDECL convall(buf, newfile)
struct oldnode *buf;				/* ptr to node in memory */
struct passparm *newfile;		/* new file passparm */
{
	int dlen;
	int i;
	char *p;
	char key[256];
	int ret;

	/**/

	if (buf->count > 150)			/* MAXENTRIES for C-Index/Plus */
	   return(BADNODE);

	ret = CCOK;

	for (i = 0, p = buf->keystr; i < buf->count; i++)
	{		/* for every key */
		newfile->keyn = ((int) *(p+1)) & 0xFF;	/* index number */
		if ((newfile->keyn < 1) || (newfile->keyn > MAXINDXS))
		{
			ret = BADNODE;
			break;
		}

	   dlen = ((int) *p) & 0xFF;					/* entry length */
#ifdef REMOVED
	   blockmv(key, p + 1, dlen);					/* return key */
#endif
	   blockmv(key, p + 2, dlen);					/* return key */
	   newfile->rec = convgtkrec(p + 1);		/* return rec # */
	   newfile->dbyte = convgtdbyte(p + 1);	/* get data byte */
		newfile->dlen = convgoodata(p);
	   if ((newfile->dlen < 0) || (newfile->dlen > 255))/* changed 5/20/87 */
	   {
			ret = BADNODE;
			break;
		}

	   if ((newfile->dlen) != 0)  /* if data also present */
#ifdef REMOVED
	      newfile->data = key+strlen(p+1)+ CONVSZEREC + CONVSZEDBYTE + 1;
#endif
	      newfile->data = key+strlen(p+2)+ CONVSZEREC + CONVSZEDBYTE + 1;
	   else
	      newfile->data = NULL;		/* clear ptr */

	   newfile->key = key;					/* set key */
	   ret = addkey(newfile, UNQ);		/* add entry to new file format */
		if (ret != CCOK)
			break;

	   p = convmoveup(p);	/* advance to next key */

#ifndef SYS3
	   if (p > buf->keystr + CONVKEYAREA)
	   {
			ret = BADNODE;
			break;
		}
#endif
	}

	return(ret);
}

int FDECL doconv(oldfile, newfile)		/* do conversion */
struct passparm *oldfile;			/* old file psp */
struct passparm *newfile;			/* new file psp */
{
	static struct oldnode buf;
	int i;
	struct oldhdrrec hdr;
	int ret;
	int cc;

	/**/

	ret = CCOK;

	if ((cc = strtwrit(newfile)) != CCOK)
	   return(cc);
	
	for (i = 0; convdiskread(oldfile, &buf, (NDEPTR)i) == CCOK; i++)	
	{		/* while nodes left */
	   if (buf.level == 0)		/* leaf node */
	      cc = convall(&buf, newfile);	/* add all keys */
		if (cc != CCOK)
			ret = cc;								/* use latest error */
	}

	cc = convhdrread(oldfile, &hdr);		/* read hdr in old format */
	if (cc == CCOK)
		cc = csetrec(newfile, hdr.hnxtdrec);

	if (cc != CCOK)
		ret = cc;								/* use latest error */

   cc = endwrit(newfile);					/* unlock file */
	if (cc != CCOK)
		ret = cc;								/* use latest error */

	return(ret);
}


int EDECL cconvert(old, new)	/* convert old file format to new file */
char *old;		/* old file name - file to read */
char *new;		/* new file name - file to write to */
{
	CFILE oldfile;	/* for the old file */
	CFILE newfile;	/* for the new file */
	FILHND oldfd;	/* for old file */
	int ret;
	int cc;

	/**/

	oldfd = mexlopen(&oldfile, old, CRDWRITE);
	if (oldfd == NULLFD)							/* if bad open */
	   return(OLDERR);

	if (ccreate(&newfile, new) != CCOK)	/* new file error */
	   return(NEWERR);

	ret = doconv(&oldfile, &newfile);	/* do it */

	cc = cclose(&newfile);		/* close new file */
	if (ret == CCOK)
		ret = cc;

	mexlclose(&oldfile);			/* close old file */

	return(ret);
}

