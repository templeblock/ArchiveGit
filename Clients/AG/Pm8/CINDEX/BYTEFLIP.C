/*     byteflip.c  -  set correct order of bytes in words
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

/* create all the following functions if there IS word conversion */
#ifndef NO_WORD_CONVERSION

/* -------------------- convert machine dependent data -------------------- */

/* convert node header to native machine format */
void FDECL nodetonative(nod, wordorder)
struct node *nod;			/* address of node to convert */
INT32 wordorder;			/* order of bytes in word */
{
		if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
			return;

      nod->level	= inteltos(nod->level, wordorder);
      nod->count	= inteltos(nod->count, wordorder);
      nod->free	= inteltos(nod->free, wordorder);
      nod->p0		= intelton(nod->p0, wordorder);
      nod->forwrd	= intelton(nod->forwrd, wordorder);
      nod->revrse	= intelton(nod->revrse, wordorder);
#ifdef COMPRESSION
      nod->compfree	= inteltos(nod->compfree, wordorder);
#endif
}


/* convert node header to intel machine format */
void FDECL nodetointel(nod, wordorder)
struct node *nod;			/* address of node to convert */
INT32 wordorder;			/* order of bytes in word */
{
		if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
			return;

   nod->level = stointel(nod->level, wordorder);
   nod->count = stointel(nod->count, wordorder);
   nod->free = stointel(nod->free, wordorder);
   nod->p0 = ntointel(nod->p0, wordorder);
   nod->forwrd = ntointel(nod->forwrd, wordorder);
   nod->revrse = ntointel(nod->revrse, wordorder);
#ifdef COMPRESSION
   nod->compfree = stointel(nod->compfree, wordorder);
#endif
}

void FDECL hdrtonative(hdr, wordorder)
struct hdrrec *hdr;			/* address of header to convert */
INT32 wordorder;			/* order of bytes in word */
{
	int i;

	if (	(INTELFMT && isintel(wordorder))
		|| (!INTELFMT && !isintel(wordorder)) )
		return;

   hdr->ck1 = inteltol(hdr->ck1, wordorder);
   hdr->hsingleroot = intelton(hdr->hsingleroot, wordorder);
   hdr->ck2 = inteltol(hdr->ck2, wordorder);
   hdr->ck3 = inteltol(hdr->ck3, wordorder);
   hdr->ck4 = inteltol(hdr->ck4, wordorder);
   hdr->hsinglelevels = inteltol(hdr->hsinglelevels, wordorder);
   hdr->ck5 = inteltol(hdr->ck5, wordorder);
   hdr->hnext = intelton(hdr->hnext, wordorder);
   hdr->ck6 = inteltol(hdr->ck6, wordorder);
   hdr->hnxtdrec = inteltol(hdr->hnxtdrec, wordorder);
   hdr->ck7 = inteltol(hdr->ck7, wordorder);
   hdr->hendofile = intelton(hdr->hendofile, wordorder);
   hdr->ck8 = inteltol(hdr->ck8, wordorder);
   hdr->updatnum = inteltol(hdr->updatnum, wordorder);
   hdr->hfreenodes = intelton(hdr->hfreenodes, wordorder);
#ifdef TRANSACTION
   hdr->htranschain = intelton(hdr->htranschain, wordorder);
   hdr->htransback  = intelton(hdr->htransback,  wordorder);
#endif	/* TRANSACTION */
	hdr->hdelcnt = inteltol(hdr->hdelcnt, wordorder);
	hdr->hreccnt = inteltol(hdr->hreccnt, wordorder);
	hdr->hcustlen = inteltol(hdr->hcustlen, wordorder);

	for (i = 0; i < MAXROOTS; i++)
	   hdr->hrootlist[i] = intelton(hdr->hrootlist[i], wordorder);
}


/* convert node header to intel machine format */
void FDECL hdrtointel(hdr, wordorder)
struct hdrrec *hdr;			/* address of node to convert */
INT32 wordorder;			/* order of bytes in word */
{
	int i;

	if (	(INTELFMT && isintel(wordorder))
		|| (!INTELFMT && !isintel(wordorder)) )
		return;

   hdr->ck1 = ltointel(hdr->ck1, wordorder);
   hdr->hsingleroot = ntointel(hdr->hsingleroot, wordorder);
   hdr->ck2 = ltointel(hdr->ck2, wordorder);
   hdr->ck3 = ltointel(hdr->ck3, wordorder);
   hdr->ck4 = ltointel(hdr->ck4, wordorder);
   hdr->hsinglelevels = ltointel(hdr->hsinglelevels, wordorder);
   hdr->ck5 = ltointel(hdr->ck5, wordorder);
   hdr->hnext = ntointel(hdr->hnext, wordorder);
   hdr->ck6 = ltointel(hdr->ck6, wordorder);
   hdr->hnxtdrec = ltointel(hdr->hnxtdrec, wordorder);
   hdr->ck7 = ltointel(hdr->ck7, wordorder);
   hdr->hendofile = ntointel(hdr->hendofile, wordorder);
   hdr->ck8 = ltointel(hdr->ck8, wordorder);
   hdr->updatnum = ltointel(hdr->updatnum, wordorder);
   hdr->hfreenodes = ntointel(hdr->hfreenodes, wordorder);
#ifdef TRANSACTION
   hdr->htranschain = ntointel(hdr->htranschain, wordorder);
   hdr->htransback  = ntointel(hdr->htransback,  wordorder);
#endif	/* TRANSACTION */
   hdr->hdelcnt = ltointel(hdr->hdelcnt, wordorder);
   hdr->hreccnt = ltointel(hdr->hreccnt, wordorder);
   hdr->hcustlen = ltointel(hdr->hcustlen, wordorder);

	for (i = 0; i < MAXROOTS; i++)
	   hdr->hrootlist[i] = ntointel(hdr->hrootlist[i], wordorder);
}

/* converts a short integer from intel format */
short EDECL inteltos(si, wordorder)
int si;
INT32 wordorder;			/* order of bytes in word */
{
   unsigned short s1,s2,sx;

	sx = (unsigned short)si;

	if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
		return(sx);

   s1 = (unsigned short) (sx << 8);
   s2 = (unsigned short) ((sx >> 8) & 0x00ff);
   s1 = (unsigned short) (s1 | s2);

   return(s1);
}

/* converts an integer from intel format */
int FDECL inteltoi(si, wordorder)
int si;
INT32 wordorder;			/* order of bytes in word */
{
#ifdef INTISSHORT					/* 16 bit ints */
   return(inteltos(si, wordorder));
#else
   return(inteltol(si, wordorder));
#endif
}

/* converts a long integer from intel format */
long FDECL finteltol(lint, wordorder)
long lint;
INT32 wordorder;			/* order of bytes in word */
{
   unsigned long l1,l2,l3,l4,lres;

	if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
		return(lint);

   l1 = lint << 24;
   l2 = (lint & 0x0000ff00) << 8;
   l3 = (lint & 0x00ff0000L) >> 8;
	l4 = (lint >> 24) & 0xff;

   lres = l1 | l2 | l3 | l4;

   return(lres);

}

/* converts a short integer to intel format */
short FDECL stointel(si, wordorder)
int si;
INT32 wordorder;			/* order of bytes in word */
{
   unsigned short s1,s2,sx;

	sx = (unsigned short)si;

	if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
		return(sx);

   s1 = (unsigned short)(sx << 8);
   s2 = (unsigned short)((sx >> 8) & 0x00ff);
   s1 = (unsigned short)(s1 | s2);

   return(s1);
}

/* converts a long integer to intel format */
long FDECL ltointel(lint, wordorder)
long lint;
INT32 wordorder;			/* order of bytes in word */
{
   unsigned long l1,l2,l3,l4,lres;

	if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
		return(lint);

   l1 = lint << 24;
   l2 = (lint & 0x0000ff00) << 8;
   l3 = (lint & 0x00ff0000L) >> 8;
   l4 = (lint >> 24) & 0xff;
   lres = l1 | l2 | l3 | l4;

   return(lres);
}


/* converts a float to intel format */
#ifdef ANSI
void FDECL ftointel(float *fvalptr, INT32 wordorder)
#else
void FDECL ftointel(fvalptr, wordorder)
float *fvalptr;
INT32 wordorder;			/* order of bytes in word */
#endif
{
   unsigned long lint, l1,l2,l3,l4,lres;

	if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
		return;

	trnslong(&lint, (long *)fvalptr);
   l1 = lint << 24;
   l2 = (lint & 0x0000ff00) << 8;
   l3 = (lint & 0x00ff0000L) >> 8;
   l4 = (lint >> 24) & 0xff;
   lres = l1 | l2 | l3 | l4;
	trnslong((long *)fvalptr, &lres);

   return;
}

/* converts a float from intel format */
#ifdef ANSI
void FDECL inteltof(float *fvalptr, INT32 wordorder)
#else
void FDECL inteltof(fvalptr, wordorder)
float *fvalptr;
INT32 wordorder;			/* order of bytes in word */
#endif
{
   unsigned long l1,l2,l3,l4,lres;
	long lint;

	if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
		return;

	trnslong(&lint, (long *)fvalptr);
   l1 = lint << 24;
   l2 = (lint & 0x0000ff00) << 8;
   l3 = (lint & 0x00ff0000L) >> 8;
	l4 = (lint >> 24) & 0xff;

   lres = l1 | l2 | l3 | l4;
	trnslong((long *)fvalptr, &lres);

   return;

}


/* converts a double to intel format */
void FDECL dtointel(dvalptr, wordorder)
double *dvalptr;
INT32 wordorder;			/* order of bytes in word */
{
   unsigned long lint, l1,l2,l3,l4,lres;
	double dres;
	char *hptr;

	if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
		return;

	trnslong(&lint, (long *)dvalptr);
   l1 = lint << 24;
   l2 = (lint & 0x0000ff00) << 8;
   l3 = (lint & 0x00ff0000L) >> 8;
   l4 = (lint >> 24) & 0xff;
   lres = l1 | l2 | l3 | l4;
	hptr = (char *)&dres;
	hptr += 4;
	trnslong(hptr, &lres);

	hptr = (char *)dvalptr;
	hptr += 4;
	trnslong(&lint, hptr);
   l1 = lint << 24;
   l2 = (lint & 0x0000ff00) << 8;
   l3 = (lint & 0x00ff0000L) >> 8;
   l4 = (lint >> 24) & 0xff;
   lres = l1 | l2 | l3 | l4;
	trnslong(&dres, &lres);

	movdouble(dvalptr, &dres);
   return;
}

/* converts a double from intel format */
void FDECL inteltod(dvalptr, wordorder)
double *dvalptr;
INT32 wordorder;			/* order of bytes in word */
{
   unsigned long lint, l1,l2,l3,l4,lres;
	double dres;
	char *hptr;

	if (	(INTELFMT && isintel(wordorder))
			|| (!INTELFMT && !isintel(wordorder)) )
		return;

	trnslong(&lint, dvalptr);
   l1 = lint << 24;
   l2 = (lint & 0x0000ff00) << 8;
   l3 = (lint & 0x00ff0000L) >> 8;
   l4 = (lint >> 24) & 0xff;
   lres = l1 | l2 | l3 | l4;
	hptr = (char *)&dres;
	hptr += 4;
	trnslong(hptr, &lres);

	hptr = (char *)dvalptr;
	hptr += 4;
	trnslong(&lint, hptr);
   l1 = lint << 24;
   l2 = (lint & 0x0000ff00) << 8;
   l3 = (lint & 0x00ff0000L) >> 8;
   l4 = (lint >> 24) & 0xff;
   lres = l1 | l2 | l3 | l4;
	trnslong(&dres, &lres);

	movdouble(dvalptr, &dres);
   return;
}


/* convert key from one bytemode to another, in place */
void FDECL cikeyflip(psp, keyn, keyptr, keytype, inbytemode, outbytemode)
CFILE PDECL *psp;			/* file parameter structure */
int keyn;					/* index number of key */
KEYPTR keyptr;				/* ptr to key being flipped */
int keytype;				/* type of key to flip */
INT32 inbytemode;			/* mode of key before flipping */
INT32 outbytemode;		/* mode of key after flipping */
{
	short shortkey;
	long longkey;
	double dblkey;
	float floatkey;
	int entlen;						/* length of entry binary key */
	int entseglen;					/* length of entry key segment */
	unsigned char *entwrkptr;	/* ptr into entry segmented key */
	KEYSEGLIST *seginfo;				/* segment info list */
	int i;
	INT32 format;
	char *dblptr;

	if ((keyn < 1) || (keyn > MAXINDXS))
		return;

	if (  (isintel(outbytemode))			/* !intel --> intel */
		&& (!isintel(inbytemode)))
	{
		format = 0x1000000L;
		switch(keytype & INDTYPEMASK)
		{
			case INTIND:							/* integer key */
#ifdef INTISSHORT
					trnsshort(&shortkey, (char *)keyptr);
					shortkey = stointel(shortkey, format);
					trnsshort((char *)keyptr, &shortkey);
#else
					trnslong(&longkey, (char *)keyptr);
					longkey = ltointel(longkey, format);
					trnslong((char *)keyptr, &longkey);
#endif	/* INTISSHORT */
				break;

			case SHORTIND:							/* short key */
				trnsshort(&shortkey, (char *)keyptr);
				shortkey = stointel(shortkey, format);
				trnsshort((char *)keyptr, &shortkey);
				break;

			case USHORTIND:							/* short key */
				trnsshort(&shortkey, (char *)keyptr);
				shortkey = stointel(shortkey, format);
				trnsshort((char *)keyptr, &shortkey);
				break;

			case LONGIND:							/* long key */
				trnslong(&longkey, (char *)keyptr);
				longkey = ltointel(longkey, format);
				trnslong((char *)keyptr, &longkey);
				break;

			case FLOATIND:							/* float key */
				trnsfloat(&floatkey, (char *)keyptr);
				ftointel(&floatkey, format);
				trnslong((char *)keyptr, &floatkey);
				break;

			case DOUBLEIND:						/* double key */
				dblptr = (char *)&dblkey;
				trnslong(dblptr, (char *)keyptr);
				trnslong(dblptr+4, ((char *)keyptr)+4);
				dtointel(&dblkey, format);
				trnslong((char *)keyptr, dblptr);
				trnslong(((char *)keyptr)+4, dblptr+4);
				break;

			case SEGIND:												/* segmented index */
				entlen = (int)(*((unsigned char *)keyptr));	/* 1st byte is length */
				entwrkptr = ((unsigned char *)keyptr) + 1;	/* set wrk pointers */
				seginfo = psp->segptr[keyn];						/* key segment info */
				for (	i = 0;
						seginfo[i].keytype != ENDSEGLIST;
						i++)
				{
					cikeyflip(psp, keyn, entwrkptr, seginfo[i].keytype, inbytemode, outbytemode);
					entseglen = _cikeylen(seginfo[i].keytype, entwrkptr);
					entwrkptr += entseglen;			/* skip to next key segment */
					entlen -= entseglen;				/* check past end of whole key */
					if (entlen <= 0)
						break;		/* past end of key segments, done flipping */
				}
				break;

			case STRINGIND:
			case NOCASEIND:
			case BINARYIND:											/* binary key */
				break;

			/* assume FIXIND value is sequence of byte values */
			case FIXIND:
			case CUST1IND:												/* custom index type 1*/
			case CUST2IND:												/* custom index type 2*/
			case CUST3IND:												/* custom index type 3*/
			case CUST4IND:												/* custom index type 4*/
			case CUST5IND:												/* custom index type 5*/
				cicustflip(psp, keyptr, keytype, inbytemode, outbytemode);
				break;
		}
	}
	else
	if	(  (!isintel(outbytemode))			/* if intel --> !intel */
	  	&& (isintel(inbytemode)))
	{
		format = 0x1000000L;
		switch(keytype & INDTYPEMASK)
		{
			case INTIND:							/* integer key */
#ifdef INTISSHORT
					trnsshort(&shortkey, (char *)keyptr);
					shortkey = inteltos(shortkey, format);
					trnsshort((char *)keyptr, &shortkey);
#else
				trnslong(&longkey, (char *)keyptr);
				longkey = inteltol(longkey, format);
				trnslong((char *)keyptr, &longkey);
#endif	/* INTISSHORT */
				break;

			case SHORTIND:							/* short key */
				trnsshort(&shortkey, (char *)keyptr);
				shortkey = inteltos(shortkey, format);
				trnsshort((char *)keyptr, &shortkey);
				break;

			case USHORTIND:							/* short key */
				trnsshort(&shortkey, (char *)keyptr);
				shortkey = inteltos(shortkey, format);
				trnsshort((char *)keyptr, &shortkey);
				break;

			case LONGIND:							/* long key */
				trnslong(&longkey, (char *)keyptr);
				longkey = inteltol(longkey, format);
				trnslong((char *)keyptr, &longkey);
				break;

			case FLOATIND:							/* float key */
				trnsfloat(&floatkey, (char *)keyptr);
				inteltof(&floatkey, format);
				trnslong((char *)keyptr, &floatkey);
				break;

			case DOUBLEIND:						/* double key */
				dblptr = (char *)&dblkey;
				trnslong(dblptr, (char *)keyptr);
				trnslong(dblptr+4, ((char *)keyptr)+4);
				inteltod(&dblkey, format);
				trnslong((char *)keyptr, dblptr);
				trnslong(((char *)keyptr)+4, dblptr+4);
				break;

			case SEGIND:												/* segmented index */
				entlen = (int)(*((unsigned char *)keyptr));	/* 1st byte is length */
				entwrkptr = ((unsigned char *)keyptr) + 1;	/* set wrk pointers */
				seginfo = psp->segptr[keyn];						/* key segment info */
				for (	i = 0;
						seginfo[i].keytype != ENDSEGLIST;
						i++)
				{
					cikeyflip(psp, keyn, entwrkptr, seginfo[i].keytype, inbytemode, outbytemode);
					entseglen = _cikeylen(seginfo[i].keytype, entwrkptr);
					entwrkptr += entseglen;			/* skip to next key segment */
					entlen -= entseglen;				/* check past end of whole key */
					if (entlen <= 0)
						break;		/* past end of key segments, done flipping */
				}
				break;

			case STRINGIND:
			case NOCASEIND:
			case BINARYIND:											/* binary key */
				break;


			case FIXIND:												/* fixed binary (custom) */
			case CUST1IND:												/* custom index type 1*/
			case CUST2IND:												/* custom index type 2*/
			case CUST3IND:												/* custom index type 3*/
			case CUST4IND:												/* custom index type 4*/
			case CUST5IND:												/* custom index type 5*/
				cicustflip(psp, keyptr, keytype, inbytemode, outbytemode);
				break;
		}
	}
}

void EDECL cipspflip(psp, outbytemode)
CFILE PDECL *psp;			/* file parameter structure */
INT32 outbytemode;			/* mode of key after flipping */
{
	if (psp->key)
	{
		cikeyflip(psp, psp->keyn, psp->key, psp->indtyp[psp->keyn] & INDTYPEMASK, psp->keyorder, outbytemode);
		psp->keyorder = outbytemode;
	}
}


#endif	/* !NO_WORD_CONVERSION */

