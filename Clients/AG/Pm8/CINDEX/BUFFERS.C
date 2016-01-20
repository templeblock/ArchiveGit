/*   ---------------------------------------------------------
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

#define BUFFERS 10

int bcbnum = BUFFERS;						/* number of buffers */

static ALLOCHND topbcbhnd;					/* top bcb in chain */
static ALLOCHND curbcbhnd;					/* current bcb in chain */
static ALLOCHND prvbcbhnd;					/* previous bcb pointer in search */
int bcbinitflag = 0;				/* 1 == buffers have be initialized */

/* #define TSTBCB  */ /* define if need to include tstbcb debuging code */

#ifdef TSTBCB
/* test for bad buffer info */
int FDECL tstbcb(struct passparm *psp);
int tstbcb(psp)
struct passparm *psp;
{
	int ret;
	BCBPTR testbcb;					/* top bcb in chain */
	ALLOCHND curhnd;					/* handle to current bcb */
	ALLOCHND nxthnd;					/* handle to next bcb */

	if (topbcbhnd)
	{
		nxthnd = topbcbhnd;					/* start at top of bcb chain */
		ret = CCOK;								/* assume OK results */
		do
		{
			curhnd = nxthnd;
		   testbcb = cilockbuf(psp, curhnd);

			/* if bad buffer # */
		   if ((testbcb->bnde < -1) || (testbcb->bnde > psp->endofile))
			{
				ciunlockbuf(psp, curhnd);
				ret = BADNODE;
				break;
			}

			nxthnd = testbcb->bnext;	/* and set current */
			ciunlockbuf(psp, curhnd);
		} while (nxthnd != NULLNEXT);
	}

	return(ret);		/* buffers must be ok */
}
#endif	/* TSTBCB */


/* set number of bcb buffers */
int EDECL cibufnum(cnt)
int cnt;
{
	struct passparm dummypsp;
	int cc;

	if (bcbinitflag)
		cc = endbcb(&dummypsp);		/* remove prior buffers */
	else
		cc = CCOK;

	if (cc == CCOK)
	{
		bcbnum = cnt;
		cc = initbcb(&dummypsp);		/* init new buffers */
	}

	return(cc);
}


/* initialize bcb chain the first time through request */
int FDECL initbcb(psp)
struct passparm *psp;
{
	int i;
	int ret;
	int cc;
	BCBPTR curbcb;				/* work pointer to bcb */
	ALLOCHND curhnd;			/* ptr to allocated buffer location */
	ALLOCHND nxthnd;			/* ptr to allocated buffer location */

	if (bcbinitflag)			/* buffers already initialized ? */
		return(CCOK);

	ret = initcindex(psp);
	if (ret)
		return(ret);

	/* initialize nodea and nodeb file and node information */
	nodea.npsp= NULL;			/* clear nodea file psp */
	nodea.nfd = NULLFD;		/* clear nodea file fd */
	nodea.nde = NULLNDE;		/* clear nodea node number */
	nodeb.npsp= NULL;			/* clear nodeb file psp */
	nodeb.nfd = NULLFD;		/* clear nodeb file fd */
	nodeb.nde = NULLNDE;		/* clear nodeb node number */

	ret = CCOK;					/* start with no memory errors */

#ifdef IMAGEBACK
	if (!bcbnum)
		return(DSKERR);
#endif	/* IMAGEBACK */

	/* allocate space for buffers */
	if (bcbnum)
	{
		curhnd = ciallocbuf(psp, sizeof(struct bcb));		/* allocate one bcb */
		if (!curhnd)
		{
			topbcbhnd = NULLNEXT;			/* no buffer handles */
			curbcb = NULL;						/* no current bcb */
			ret = OUTOFMEM;					/* error return */
		}
		else
		{
			topbcbhnd = curhnd;					/* top points to first buf */
			curbcb = cilockbuf(psp, curhnd);	/* lock down buffer for processing */
		}

		for (i = 0; (i < bcbnum) && topbcbhnd; i++) 
		{
			/* clear buffer including hangover bytes */
			cclear((char *)curbcb, sizeof(struct bcb), 0);

		   curbcb->bpsp = NULL;					/* init psp ptr */
		   curbcb->bfd  = NULLFD;				/* init file descriptor */
		   curbcb->bnde = NULLNDE;				/* init node num of buffer */
		   curbcb->bstatus = CLEAN;			/* clear status flag */
			curbcb->bupdatnum = 0;				/* clear update # indicator */

			if (i < (bcbnum - 1))				/* if not last buffer */
			{
				/* allocate next bcb */
				nxthnd = ciallocbuf(psp, sizeof(struct bcb));
				if (!nxthnd)
				{
					topbcbhnd = NULLNEXT;		/* no buffer handles */
					ciunlockbuf(psp, curhnd);	/* release handle */
					curbcb->bnext = NULLNEXT;	/* terminate list */

					/* clear all allocated buffers */
					nxthnd = topbcbhnd;
					do 
					{
						curhnd = nxthnd;
					   curbcb = cilockbuf(psp, curhnd);

						nxthnd = curbcb->bnext;			/* and set current */
#ifdef MEMORY_MANAGEMENT_REQUIRED	/* use this typedef for Window or Mac */
						ciunlockbuf(psp, curhnd);
#endif
						cc = cideallocbuf(psp, curhnd);	/* deallocate buffer space */
						if (cc)
							break;

					} while (nxthnd != NULLNEXT);		/* traverse bcb chain */

					ret = OUTOFMEM;				/* error return */
					break;							/* stop processing */
				}
				else
				{
					curbcb->bnext = nxthnd;		/* point to next bcb in chain */
					ciunlockbuf(psp, curhnd);	/* release handle */
					curhnd = nxthnd;				/* use next handle */
					curbcb = cilockbuf(psp, curhnd);	/* lock for processing */
				}
			}
			else
			{
				curbcb->bnext = NULLNEXT;		/* terminate list */
				ciunlockbuf(psp, curhnd);		/* release handle */
			}
		}
	}

#ifdef TSTBCB
	tstbcb(psp);
#endif

	if (ret == CCOK)
		bcbinitflag = 1;							/* initialization done */

	return(ret);									/* return any memory errors */
}


/* remove all allocated space in bcb chain when finished using cindex */
int EDECL endbcb(psp)
struct passparm *psp;
{
	BCBPTR bcbptr;					/* pointer to buffer */
	ALLOCHND curhnd;						/* ptr to allocated buffer location */
	ALLOCHND nxthnd;						/* ptr to allocated buffer location */
	int cc;

	/* clear nodea and nodeb work buffers */
	nodea.npsp = NULL;				/* clear nodea of file psp */
	nodea.nfd  = NULLFD;				/* clear nodea of file fd */
	nodea.nde = NULLNDE;				/* clear nodea of node number */
	nodeb.npsp = NULL;				/* clear nodeb of file psp */
	nodeb.nfd  = NULLFD;				/* clear nodeb of file fd */
	nodeb.nde = NULLNDE;				/* clear nodeb of node number */

	if (bcbnum == 0 || topbcbhnd == 0 || bcbinitflag == 0)	/* no buffers */
		return(CCOK);

	/* check for open files */
	nxthnd = topbcbhnd;
	do 
	{
		curhnd = nxthnd;
	   bcbptr = cilockbuf(psp, curhnd);

	   if (bcbptr->bpsp != NULL)		/* still have file open */
			return(FAIL);

		nxthnd = bcbptr->bnext;			/* and set current */
#ifdef MEMORY_MANAGEMENT_REQUIRED	/* use this typedef for Window or Mac */
		ciunlockbuf(psp, curhnd);
#endif
	} while (nxthnd != NULLNEXT);		/* traverse bcb chain */

	/* clear all allocated buffers */
	nxthnd = topbcbhnd;
	do 
	{
		curhnd = nxthnd;
	   bcbptr = cilockbuf(psp, curhnd);

		nxthnd = bcbptr->bnext;			/* and set current */
#ifdef MEMORY_MANAGEMENT_REQUIRED	/* use this typedef for Window or Mac */
		ciunlockbuf(psp, curhnd);
#endif
		cc = cideallocbuf(psp, curhnd);	/* deallocate buffer space */
		if (cc)
			return(cc);

	} while (nxthnd != NULLNEXT);		/* traverse bcb chain */

	bcbinitflag = 0;							/* initialization undone */

	return(CCOK);
}


/* clear active buffers of this fd */
void FDECL clrbcb(psp)
struct passparm *psp;
{
	BCBPTR bcbptr;					/* pointer to buffer */
	ALLOCHND curhnd;						/* ptr to allocated buffer location */
	ALLOCHND nxthnd;						/* ptr to allocated buffer location */

	/* clear nodea work buffer if it contains something from this file */
   if ((nodea.npsp == psp) && (nodea.nfd == psp->fd))			/* matches */
	{
      nodea.npsp = NULL;				/* clear nodea of this file psp */
      nodea.nfd  = NULLFD;				/* clear nodea of this file fd */
		nodea.nde = NULLNDE;				/* clear nodea of node number */
	}

   if ((nodeb.npsp == psp) && (nodeb.nfd == psp->fd))			/* matches */
	{
      nodeb.npsp = NULL;				/* clear nodeb of this file psp */
      nodeb.nfd  = NULLFD;				/* clear nodeb of this file fd */
		nodeb.nde = NULLNDE;				/* clear nodeb of node number */
	}

#ifdef TRANSACTION
   if ((nodet.npsp == psp) && (nodet.nfd == psp->fd))			/* matches */
	{
      nodet.npsp = NULL;				/* clear nodet of this file psp */
      nodet.nfd  = NULLFD;				/* clear nodet of this file fd */
		nodet.nde = NULLNDE;				/* clear nodet of node number */
	}
#endif	/* TRANSACTION */

	if (bcbnum == 0 || topbcbhnd == 0)	/* no buffers */
		return;

	nxthnd = topbcbhnd;
	do 
	{
		curhnd = nxthnd;
	   bcbptr = cilockbuf(psp, curhnd);

		/* if bcb matches */
	   if ((bcbptr->bpsp == psp) && (bcbptr->bfd == psp->fd))
		{
	      bcbptr->bpsp = NULL;			/* clear bcb of this file */
			bcbptr->bfd  = NULLFD;
			bcbptr->bnde = NULLNDE;
			bcbptr->bstatus = 0;
			bcbptr->bupdatnum = 0;
		}

		nxthnd = bcbptr->bnext;			/* and set current */
#ifdef MEMORY_MANAGEMENT_REQUIRED	/* use this typedef for Window or Mac */
		ciunlockbuf(psp, curhnd);
#endif
	} while (nxthnd != NULLNEXT);		/* traverse bcb chain */
}


#ifdef TRANSACTION
/* clear active buffer of this node */
int FDECL clrtransbcb(psp, zapnde)
struct passparm *psp;
NDEPTR zapnde;						/* node number to delete from buffers */
{
	BCBPTR bcbptr;					/* pointer to buffer */
	ALLOCHND curhnd;						/* ptr to allocated buffer location */
	ALLOCHND nxthnd;						/* ptr to allocated buffer location */
	int ret = CCOK;

	/* clear nodea work buffer if it contains something from this file */
	/* if matches nodea */
   if ((nodea.npsp == psp) && (nodea.nfd == psp->fd) && (nodea.nde == zapnde))
	{
      nodea.npsp = NULL;				/* clear nodea of this file psp */
      nodea.nfd  = NULLFD;				/* clear nodea of this file fd */
		nodea.nde = NULLNDE;				/* clear nodea of node number */
	}

	/* if matches node b*/
   if ((nodeb.npsp == psp) && (nodeb.nfd == psp->fd) && (nodeb.nde == zapnde))
	{
      nodeb.npsp = NULL;				/* clear nodeb of this file psp */
      nodeb.nfd  = NULLFD;				/* clear nodeb of this file fd */
		nodeb.nde = NULLNDE;				/* clear nodeb of node number */
	}


	if (bcbnum == 0 || topbcbhnd == 0)	/* no buffers */
		return(CCOK);

	nxthnd = topbcbhnd;
	do 
	{
		curhnd = nxthnd;
	   bcbptr = cilockbuf(psp, curhnd);

		/* if bcb matches */
	   if ((bcbptr->bpsp == psp) && (bcbptr->bfd == psp->fd) && (bcbptr->bnde == zapnde))
		{
#ifdef IMAGEBACK
			/* if doing image backup, back up trans node before clearing from memory */
			if (psp->imagepsp)
			{
				bcbptr->bupdatnum = psp->pupdatnum;	/* set current update number */
				ret = bcbback(psp, bcbptr);			/* write backup copy of buffer */
			}
#endif

	      bcbptr->bpsp = NULL;			/* clear bcb of this file */
			bcbptr->bfd  = NULLFD;
			bcbptr->bnde = NULLNDE;
			bcbptr->bstatus = 0;
			bcbptr->bupdatnum = 0;		/* clear update # indicator */
		}

		nxthnd = bcbptr->bnext;			/* and set current */
#ifdef MEMORY_MANAGEMENT_REQUIRED	/* use this typedef for Window or Mac */
		ciunlockbuf(psp, curhnd);
#endif
	} while (nxthnd != NULLNEXT);		/* traverse bcb chain */

	return(ret);
}
#endif	/* TRANSACTION */


int FDECL bcbwrite(psp, bcbptr)	/* absolute write of buffer */
struct passparm *psp;	/* parameter structure for file */
BCBPTR bcbptr;		/* pointer to buffer bcb to write	*/
{
	int ec;			/* return error code	*/

#ifdef IMAGEBACK
	if (psp->imagepsp && (psp->backnde != psp->backlast))
	{
		ec = upddisk(psp->imagepsp);
		if (ec == CCOK)
			psp->backlast = psp->backnde;
		else
			return(ec);
	}
#endif	/* IMAGEBACK */

#ifdef TSTBCB
	tstbcb(psp);
#endif

#ifdef MIXED_MODEL_BUFFERS
	/* move buffer to temp near buffer */
	buftonear(&ebuffer, bcbptr, sizeof(bcbptr->bbuffer));

	/* convert node header to intel format */
	nodetointel(&ebuffer, psp->wordorder);

#ifdef COMPRESSION
	ec = diskcompwrte(bcbptr, &ebuffer);
#else
	/* write buffer to disk via near buffer */
	ec=diskwrte(bcbptr->bpsp, &ebuffer, bcbptr->bnde);
#endif	/* COMPRESSION */

#else		/* !MIXED_MODEL_BUFFERS */
	/* convert node header to intel format */
	nodetointel((struct node *)&bcbptr->bbuffer, psp->wordorder);

	/* write buffer directly to disk */
#ifdef COMPRESSION
	ec = diskcompwrte(bcbptr, (struct node *) &bcbptr->bbuffer);
#else
	/* write buffer to disk via near buffer */
	ec=diskwrte(bcbptr->bpsp, (struct node *) &bcbptr->bbuffer,bcbptr->bnde);
#endif	/* COMPRESSION */

	/* convert node back to local format */
	nodetonative((struct node *)&bcbptr->bbuffer, psp->wordorder);
#endif	/* MIXED_MODEL_BUFFERS */
 	/* if good write */
	if (ec == CCOK)
	{
 	   bcbptr->bstatus = CLEAN;		/* clean buffer */
		ec = mhdrupdte(psp);
	}

	return(ec);
}

#ifdef COMPRESSION
int FDECL diskcompwrte(bcbptr, nod)
BCBPTR bcbptr;				/* pointer to buffer bcb to write	*/
struct node *nod;			/* location of node to compress */
{
	int ret;

	ret = cishrink(bcbptr->bpsp, &nodec, nod);	/* compress the node */
	if (ret == CCOK)
	{
		bcbptr->bbuffer.compfree = nodec.compfree;

		/* if matches nodea, update compressed free space */
	   if ((nodea.npsp == bcbptr->bpsp) && (nodea.nfd == bcbptr->bfd))
			nodea.compfree = inteltos(nodec.compfree, bcbptr->bpsp->wordorder);

		/* if matches nodeb, update compressed free space */
	   if ((nodeb.npsp == bcbptr->bpsp) && (nodeb.nfd == bcbptr->bfd))
			nodeb.compfree = inteltos(nodec.compfree, bcbptr->bpsp->wordorder);		

		ret = diskwrte(bcbptr->bpsp, &nodec, bcbptr->bnde);
	}

	return(ret);
}
#endif	/* COMPRESSION */

#ifdef IMAGEBACK
/* backup write of bcb buffer */
int FDECL bcbback(psp, bcbptr)
struct passparm PDECL *psp;	/* parameter structure for file */
BCBPTR bcbptr;		/* pointer to buffer bcb to write	*/
{
	int ec;								/* return error code	*/

#ifndef MIXED_MODEL_BUFFERS
	struct bcbhead bcbHeadSave;	/* save area for header values */

	/* information in bcb to save before it gets changed for image file */
	blockmv((char *)&bcbHeadSave, (char *)bcbptr, sizeof(bcbHeadSave));

	if (psp->imagebackproc)
	{
		ec = (psp->imagebackproc)(psp, (struct imgbcb *)bcbptr);
		if (ec)
			return(ec);
	}
#endif	/* !MIXED_MODEL_BUFFERS */

#ifdef TSTBCB
	tstbcb(psp);
#endif

#ifdef MIXED_MODEL_BUFFERS
	/* move buffer to temp near buffer */
	buftonear(&localbcb.bbuffer, bcbptr, sizeof(bcbptr->bbuffer));
	localbcb.bpsp = psp;
	localbcb.bnde = bcbptr->bnde;
	localbcb.bstatus = bcbptr->bstatus;
	localbcb.bnext = bcbptr->bnext;
	localbcb.bupdatnum = bcbptr->bupdatnum;
	
	/* convert node header to intel format */
	nodetointel(&localbcb.bbuffer, psp->wordorder);

	ec = fileSeek(psp->imagepsp, psp->backnde++, sizeof(struct imgbcb), (long)HDRSZE);
	if (!ec)				 	/* if no error, write buffer */
		ec = ciwrite(psp->imagepsp, (char *)&localbcb, sizeof(struct bcb));

	/* convert node back to local format */
	nodetonative((struct node *)&bcbptr->bbuffer, psp->wordorder);
#else		/* !MIXED_MODEL_BUFFERS */
	/* convert node header to intel format */
	nodetointel((struct node *)&bcbptr->bbuffer, psp->wordorder);

	/* write buffer directly to disk */
	ec = fileSeek(psp->imagepsp, psp->backnde++, sizeof(struct imgbcb), (long)HDRSZE);
	if (!ec)				 	/* if no error, write buffer */
		ec = ciwrite(psp->imagepsp, (char *)bcbptr, sizeof(struct bcb));

	/* information in bcb to save before it gets changed for image file */
	blockmv((char *)bcbptr, (char *)&bcbHeadSave, sizeof(bcbHeadSave));

#ifdef REMOVED
		/* convert node back to local format */
		nodetonative((struct node *)&bcbptr->bbuffer, psp->wordorder);
#endif
#endif	/* MIXED_MODEL_BUFFERS */

	return(ec);
}

/* read bcb record from image backup file into temp buffer */
int FDECL bcbread(psp, bcbptr, nde)
struct passparm PDECL *psp;				/* psp for image backup file */
struct bcb *bcbptr;							/* pointer to local bcb buffer */
NDEPTR nde;										/* buffer number to write */
{
	int ec;			/* return error code	*/
	NDEPTR ndenum;

	/* clear buffer including hangover bytes */
	cclear((char *)bcbptr, sizeof(struct bcb), 0);

	ndenum = nde - NODEBIAS;				/* remove test node bias */
	ec = fileSeek(psp, ndenum, sizeof(struct imgbcb), (long)HDRSZE);
	if (!ec)				 	/* if no error, read buffer */
		ec = ciread(psp, (char *)bcbptr, sizeof(struct imgbcb));

	/* convert node back to local format */
	nodetonative((struct node *)&bcbptr->bbuffer, psp->wordorder);


	if (!ec && psp->imagerestproc)
	{
		ec = (psp->imagerestproc)(psp, (struct imgbcb *)bcbptr);
	}

	if (!ec)				 	/* if no error, read buffer */
	{
		ec = imgchecknde(psp, &(bcbptr->bbuffer), ndenum);
		if (ec != CCOK)
		{
			psp->alterr = ec;
		}
	}

	return(ec);
}

#endif	/* IMAGEBACK */

int FDECL noderead(psp, nod, nde)		/* gets node from buffer or disk */
struct passparm *psp;		/* pointer to parameter struct */
struct node *nod;			/* where to put it */
NDEPTR nde;			/* node number */
{
	char *ptr;
	int err = CCOK;			/* return from bcbs */
	ALLOCHND curhnd;			/* handle to current bcb */
	BCBPTR bcbptr;				/* pointer to actual bcb location */
	NDEPTR ndereq;				/* read ahead/behind node */
	struct node *nodbuf;		/* node to read into */

	/* if requested reading past end of file info, must be error */
	if (nde >= psp->next)
		return(INVNODEREAD);

#ifdef TSTBCB
	tstbcb(psp);
#endif

#ifdef DEMO
   if (nde > DEMOLIMIT)		/* greater than allowed */
         return(NOSPACE);
#endif

#ifdef NO_FAST_TREESRCH
	if (!nod)
		nod = &nodea;		/* must use nodea for movable buffers */
#endif

	/* if work buffer contains fd and node number, no read required */
	if (	nod								/* have node pointer */
		&&	(nod->nde == nde)				/* same node number */
		&&	(nod->npsp == psp)			/* same file psp */
		&&	(nod->nfd == psp->fd)		/* same file fd */
#ifdef TRANSACTION
		&&	((nod == &nodea) || (nod == &nodet) || (nod == &noder))) /* reading into nodea, nodet or noder */
#else
		&&	(nod == &nodea)) 			/* reading into nodea */
#endif	/* TRANSACTION */
	{
		err = CCOK;
	}
	else
	if (bcbnum == 0)
	{
		if (!nod)
		{
			nod = &nodea;			/* must use nodea if no buffers */
			psp->speednod = (struct smallnode *)nod;
		}

		/* clear area past end of disk node (CI_BIGBUF extra space) */
		ptr = (char *)(nod->keystr);
		ptr += (KEYAREA * MAXCOMP);
		cclear(ptr, (CI_BIGBUF - (KEYAREA * MAXCOMP)), 0);

#ifdef COMPRESSION
		err = DSKERR;	 					/* must have buffers when compressing */
#else
#ifdef IMAGEBACK
		err = DSKERR;							/* must have buffers for image backup */
#else
		err = diskread(psp, nod, nde);		/* read into work buffer */
#endif	/* IMAGEBACK */
#endif	/* COMPRESSION */

		nodetonative(nod, psp->wordorder);		/* convert to machine format */
	}
	else
	{
		/* MU */
		if (goodbufs(psp) != CCOK && topbcbhnd != 0) /* buffers no good */
			clrbcb(psp);										/* clear buffers */	    

		if (!locnode(psp, nde))    /* not in buffer */
		{	
			curhnd = topbcbhnd;
			bcbptr = cilockbuf(psp, curhnd);

			if (bcbptr->bstatus == DIRTY)  /* dirty buffer at top */
			{
				err = bcbwrite(psp, bcbptr);		/* write out */
			}

			if (err == CCOK) 			/* continue if good */
			{
				if (!nod)
				{
					/* read directly to virtual buffer */
					nodbuf = (struct node *)&bcbptr->bbuffer;
					psp->speednod = &bcbptr->bbuffer;
				}
				else
				{
					/* read into work buffer */
					nodbuf = nod;
					psp->speednod = (struct smallnode *)nod;
				}

				/* read in node */ 
#ifdef COMPRESSION
				err = diskcompread(psp, nodbuf, nde);		/* read compressed node */
#else
				err = diskread(psp, nodbuf, nde);		/* read into work buffer */
#endif	/* COMPRESSION */
				if (err == CCOK)  						/* if good read, put in bcb */
				{
					/* convert node header to local format */
					nodetonative(nodbuf, psp->wordorder);

#ifdef MIXED_MODEL_BUFFERS		/* move buffer read from disk into far bcb */
					buftofar(bcbptr, nod, sizeof(bcbptr->bbuffer));
#else									/* move buffer read from disk into near bcb */
					if (nod)
						blockmv((char *) &bcbptr->bbuffer, (char *) nod, sizeof(bcbptr->bbuffer));
#endif	/* MIXED_MODEL_BUFFERS */

				   bcbptr->bnde = nde;			/* set node number */
				   bcbptr->bpsp = psp;			/* set file psp */
				   bcbptr->bfd  = psp->fd;		/* set file descriptor */
			 	   bcbptr->bstatus = CLEAN;	/* clean buffer */
					bcbptr->bupdatnum = 0;		/* clear update # indicator */
				}
			}
		}
		else			/* already in top buffer */
		{
			curhnd = topbcbhnd;
			bcbptr = cilockbuf(psp, curhnd);

			if (!nod)
			{
				psp->speednod = &bcbptr->bbuffer;
			}
			else
			{
				psp->speednod = (struct smallnode *)nod;
			}

#ifdef MIXED_MODEL_BUFFERS		/* move far bcb into near work buffer */
			buftonear(nod, bcbptr, sizeof(bcbptr->bbuffer));
#else									/* move bcb into work buffer */
			if (nod)
				blockmv((char *) nod, (char *) &bcbptr->bbuffer, sizeof(bcbptr->bbuffer));
#endif	/* MIXED_MODEL_BUFFERS */
		}

		if (err == CCOK) 
		{
			if (nod)
			{
				/* clear area past end of disk node (CI_BIGBUF extra space) */
				ptr = (char *)(nod->keystr);
				ptr += (KEYAREA * MAXCOMP);
				cclear(ptr, (CI_BIGBUF - (KEYAREA * MAXCOMP)), 0);

				nod->npsp = psp;			/* set the work buffers psp address */
				nod->nfd  = psp->fd;		/* set the work buffers fd */
				nod->nde = nde;			/* set the work buffers node number */
			}
		}

		ciunlockbuf(psp, curhnd);				/* unlock handle of buffer */
	}

#ifndef MIXED_MODEL_BUFFERS		/* cannot do read-ahead with mixed model buffers */
	/* if need to do read ahead/behind for speed reading features */
	if (psp->speednod && !err)
	{
		ndereq = (psp->speedreq > 0)
					? psp->speednod->smforwrd		/* cnext, read forward */
					: psp->speednod->smrevrse;		/* cprev, read back */
	}
	else
	{
		ndereq = -1;
	}
	while(!err && psp->speedreq && (ndereq != -1) && (psp->speednod->smlevel == 0))
	{
		/* read next or previous node */
		if (!locnode(psp, ndereq))    /* if not in buffer, read from disk */
		{	
			curhnd = topbcbhnd;
			bcbptr = cilockbuf(psp, curhnd);

			if (bcbptr->bstatus == DIRTY)  /* dirty buffer at top */
			{
				err = bcbwrite(psp, bcbptr);		/* write out */
			}

			if (err == CCOK) 			/* continue if good */
			{
				/* read in node */ 
#ifdef COMPRESSION
				err = diskcompread(psp, (struct node *)&bcbptr->bbuffer, ndereq);		/* read compressed node */
#else
				err = diskread(psp, (struct node *)&bcbptr->bbuffer, ndereq);		/* read into buffer */
#endif	/* COMPRESSION */
				if (err == CCOK)  						/* if good read, put in bcb */
				{
					/* convert node header to local format */
					nodetonative((struct node *)&bcbptr->bbuffer, psp->wordorder);

				   bcbptr->bnde = ndereq;		/* set node number */
				   bcbptr->bpsp = psp;			/* set file psp */
				   bcbptr->bfd  = psp->fd;		/* set file descriptor */
			 	   bcbptr->bstatus = CLEAN;	/* clean buffer */
					bcbptr->bupdatnum = 0;		/* clear update # indicator */
				}
			}

		}
		else
		{
			curhnd = topbcbhnd;
			bcbptr = cilockbuf(psp, curhnd);
		}

		if (psp->speedreq > 0)
		{
			ndereq = bcbptr->bbuffer.smforwrd;
			psp->speedreq--;
		}
		else
		{
			ndereq = bcbptr->bbuffer.smrevrse;
			psp->speedreq++;
		}

		ciunlockbuf(psp, curhnd);				/* unlock handle of buffer */
	}
#endif	/* MIXED_MODEL_BUFFERS */

	return(err);
}


/* gets node from buffer for speed read functions */
struct smallnode PDECL * FDECL nodereadsp(psp, nde)
struct passparm *psp;		/* pointer to parameter struct */
NDEPTR nde;			/* node number */
{
	char *ptr;
	ALLOCHND curhnd;						/* handle to current bcb */
	BCBPTR bcbptr;							/* pointer to actual bcb location */
	struct smallnode PDECL *ret;		/* return node structure pointer */

#ifdef TSTBCB
	tstbcb(psp);
#endif

#ifdef DEMO
   if (nde > DEMOLIMIT)		/* greater than allowed */
         return(NOSPACE);
#endif


	/* if work buffer contains fd and node number, no read required */
	if (	(nodea.nde == nde)			/* same node number */
		&&	(nodea.npsp == psp)			/* same file psp */
		&&	(nodea.nfd == psp->fd))		/* same file fd */
	{
		/* return ptr to nodea which contains nde */
		ret = (struct smallnode *) &nodea;
	}
	else
	if (bcbnum == 0)
	{
		ret = (struct smallnode *) 0;
	}
	else
	{
		if (!locnode(psp, nde))    /* not in buffer */
		{	
			/* cannot find node from buffers - this is an error */
			ret = (struct smallnode *) 0;
		}
		else			/* already in top buffer */
		{
			curhnd = topbcbhnd;
			bcbptr = cilockbuf(psp, curhnd);

#ifdef MIXED_MODEL_BUFFERS		/* move far bcb into near work buffer */
			buftonear(&nodea, bcbptr, sizeof(bcbptr->bbuffer));
			ret = (struct smallnode *) &nodea;
#else									/* move bcb into work buffer */
#ifdef MEMORY_MANAGEMENT_REQUIRED	/* use this typedef for Window or Mac with moveable memory */
			blockmv((char *) &nodea, (char *) &bcbptr->bbuffer, sizeof(bcbptr->bbuffer));
			ret = (struct smallnode *) &nodea;
#else
			/* just return pointer into buffer containing nde */
			ret = (struct smallnode *) &(bcbptr->bbuffer);
#endif	/* MEMORY_MANAGEMENT_REQUIRED */
#endif	/* MIXED_MODEL_BUFFERS */
		}

		if (ret == (struct smallnode *) &nodea) 
		{
			/* clear area past end of disk node (CI_BIGBUF extra space) */
			ptr = (char *)(nodea.keystr);
			ptr += (KEYAREA * MAXCOMP);
			cclear(ptr, (CI_BIGBUF - (KEYAREA * MAXCOMP)), 0);

			nodea.npsp = psp;			/* set the work buffers psp address */
			nodea.nfd  = psp->fd;		/* set the work buffers fd */
			nodea.nde = nde;			/* set the work buffers node number */
		}

#ifdef MEMORY_MANAGEMENT_REQUIRED	/* use this typedef for Window or Mac with moveable memory */
		ciunlockbuf(psp, curhnd);				/* unlock handle of buffer */
#endif	/* MEMORY_MANAGEMENT_REQUIRED */
	}

	return(ret);
}


#ifdef COMPRESSION
/* read compressed node into work node */
int FDECL diskcompread(psp, nod, nde)
struct passparm *psp;
struct node *nod;		/* where to put information */
NDEPTR nde;		/* node number */
{
	int ret;

	ret = diskread(psp, &nodec, nde);
	if (ret == CCOK)
	{
		ret = ciexpand(psp, nod, &nodec);		/* decompress node */

		/* if first time, fake compressed free */
		if ((ret == CCOK) && !nod->count && (nod->compfree == 0))
			nod->compfree = stointel(KEYAREA, psp->wordorder);
	}

	return(ret);

}
#endif	/* COMPRESSION */


int FDECL nodewrte(psp, nod, nde)	/* puts node in buffer - dirty */
struct passparm *psp;			/* pointer to parameter struct */
struct node *nod;					/* where the node is */
NDEPTR nde;							/* node number */
{
	ALLOCHND curhnd;			/* handle to current bcb */
	BCBPTR bcbptr;		/* pointer to actual bcb location */
	int err = CCOK;

#ifdef TSTBCB
	tstbcb(psp);
#endif

	if ((nod->free < 0) || (nod->count < 0))
		return(BADNODE);

	psp->updtflag = DIRTY;		/* file now modified */

	nod->npsp = psp;			/* set the work buffers psp, fd and node number */
	nod->nfd  = psp->fd;
	nod->nde  = nde;

	/* if this is nodeb and nodea same node as nodeb, clear nodea indicators */
	if ((nod == &nodeb) && (nodea.nde == nodeb.nde))
	{
		nodea.nde = NULLNDE;
		nodea.npsp = NULL;
		nodea.nfd  = NULLFD;
	}

	/* if no buffers, write directly to disk */
   if (bcbnum == 0)
	{
		nodetointel(nod, psp->wordorder);	/* convert node header to intel format */
#ifdef COMPRESSION
		err = DSKERR;							/* must have buffers for compression */
#else
#ifdef IMAGEBACK
		err = DSKERR;							/* must have buffers for image backup */
#else
		err = diskwrte(psp, nod, nde);	/* write direct to disk */
#endif	/* IMAGEBACK */
#endif	/* COMPRESSION */
		nodetonative(nod, psp->wordorder);	/* convert node back to local format */
      return(err);
   }

	/* MU */
	if (goodbufs(psp) != CCOK && topbcbhnd != 0) /* buffers no good */
	   clrbcb(psp);										/* clear buffers */	    
  
	if (!locnode(psp, nde)) /* not found in buffer */
	{
		curhnd = topbcbhnd;
		bcbptr = cilockbuf(psp, curhnd);

	   if (bcbptr->bstatus == DIRTY)  	/* dirty buffer at top */
	   {
	      /* MU */
			err = bcbwrite(psp, bcbptr);		/* write out */
	   }
	}
	else
	{
		curhnd = topbcbhnd;
		bcbptr = cilockbuf(psp, curhnd);

#ifdef IMAGEBACK
		/* if this node has not been backuped up yet, do it now */
		if (psp->imagepsp && (bcbptr->bupdatnum != psp->pupdatnum))
		{
			bcbptr->bupdatnum = psp->pupdatnum;	/* set current update number */
			err = bcbback(psp, bcbptr);			/* write backup copy of buffer */
		}
#endif
	}

	if (err == CCOK)  
	   err = bcbadd(nod, bcbptr, nde, psp); /* put in buf */

	ciunlockbuf(psp, curhnd);				/* unlock handle of buffer */

	return(err);
}


/* put bcb pointed to by bcbptr at top - fix chain */
void FDECL puttop(psp, bcbhnd)
struct passparm *psp;	/* current psp */
ALLOCHND bcbhnd;			/* handle to bcb */
{
	BCBPTR curbcb;		/* pointer to actual bcb location */
	BCBPTR prvbcb;		/* pointer to actual bcb location - previous */

	if (bcbhnd != topbcbhnd) 				/* dont need this if already there */
	{
		curbcb = cilockbuf(psp, bcbhnd);			/* lock current and prev buffers */
		prvbcb = cilockbuf(psp, prvbcbhnd);

	   prvbcb->bnext = curbcb->bnext;	/* fix chain */
	   curbcb->bnext = topbcbhnd;			/* point to old top */
	   topbcbhnd = bcbhnd;					/* new top */

		ciunlockbuf(psp, bcbhnd);					/* unlock buffers */
		ciunlockbuf(psp, prvbcbhnd);
	}
}

/* locates node in buffer */
/* returns 1 if found */
/* returns 0 if not found */
/* puts appropriate node at top (pointed to by topbcbhnd) */
int FDECL locnode(psp, nde)
struct passparm *psp;
NDEPTR nde;		/* node number */
{
	int ret;

	ret = scanbuf(psp, nde);	/* looks for node */
	puttop(psp, curbcbhnd);		/* put at top */
	return(ret);
}	 


/* sets curbcb to pointed at buffer */
/* returns 1 if found - 0L if not found */
int FDECL scanbuf(psp, nde)
struct passparm *psp;
NDEPTR nde;	/* node number to find */
{
	BCBPTR curbcbptr;			/* ptr to current bcb */
	ALLOCHND nxthnd;					/* handle to next bcb */
	int ret;

	prvbcbhnd = curbcbhnd = topbcbhnd;		/* all start out the same */
	curbcbptr = cilockbuf(psp, curbcbhnd);	/* get ptr to buffer */

	ret = 1;											/* assume success */
	/* loop through bcb chain until match is found */
	while (	curbcbptr->bpsp != psp 
			|| curbcbptr->bnde != nde
			|| curbcbptr->bfd  != psp->fd) 
	{
	   if (curbcbptr->bnext == NULLNEXT)	/* out of buffers ? */
		{
	      ret = 0;									/* no match */
			break;
		}

	   prvbcbhnd = curbcbhnd;					/* set prev pointer */
	   nxthnd = curbcbptr->bnext;				/* and set current */
		ciunlockbuf(psp, curbcbhnd);			/* unlock current buffer */
	   curbcbhnd = nxthnd;
		curbcbptr = cilockbuf(psp, curbcbhnd);		/* lock next buffer in chain */
	}

	ciunlockbuf(psp, curbcbhnd);				/* unlock current buffer */
	return(ret);		/* cant get here without match */
}

	
/* moves node info into buf of located bcb */
int FDECL bcbadd(nod, bcbptr, nde, psp)
struct node *nod;							/* where node is */
BCBPTR bcbptr;						/* where to put it */
NDEPTR nde;									/* node number */
struct passparm *psp;	/* passparm - needed for security reasons */
{
	int err;

	/**/

#ifdef TSTBCB
	tstbcb(psp);
#endif

	err = CCOK;						/* assume OK */

	bcbptr->bpsp = psp;			/* set file psp */
	bcbptr->bfd  = psp->fd;		/* set file descriptor */
	bcbptr->bnde = nde;			/* set node number */

	if (nde > psp->endofile)
		err = BADNODE;

#ifdef MIXED_MODEL_BUFFERS		/* move near work buffer into far bcb */
					buftofar(bcbptr, nod, sizeof(bcbptr->bbuffer));
#else									/* move work buffer into bcb */
					blockmv((char *) &bcbptr->bbuffer, (char *) nod, sizeof(bcbptr->bbuffer));
#endif	/* MIXED_MODEL_BUFFERS */

	switch (psp->seclev)	/* now act according to security level */
	{
		case 0:		/* no security */
		case 1:		/* close on extend */
		case 4:		/* only write out at end */
		   bcbptr->bstatus = DIRTY;	/* just set */
		   break;

		case 2:		/* write out all leaf nodes */
			if (nod->level == 0)	/* leaf node */
			{
				err = bcbwrite(psp, bcbptr);		/* write out buffer */
			}
			else
				bcbptr->bstatus = DIRTY;   /* else make dirty */

		   break;

		case 3:		/* write out any node */
		   err = bcbwrite(psp, bcbptr);	/* do it */
		   bcbptr->bstatus = CLEAN;	/* now clean */
		   break;
	}

	return(err);
}


/* close active buffers from bcb before (closing file) */
int FDECL clsbcb(psp)
CFILE *psp;		/* so we can update header if neccessary */
{
	int ec;									/* write error code */
	BCBPTR bcbptr;					/* pointer to buffer */
	ALLOCHND curhnd;						/* ptr to allocated buffer location */
	ALLOCHND nxthnd;						/* ptr to allocated buffer location */

	/* clear work buffer if it contains something from this file */
   if ((nodea.npsp == psp) && (nodea.nfd == psp->fd))		/* matches */
	{
      nodea.npsp = NULL;				/* clear nodea of this file */
      nodea.nfd  = NULLFD;				/* clear nodea of this file */
		nodea.nde  = NULLNDE;			/* clear nodea of node number */
	}

   if ((nodeb.npsp == psp) && (nodeb.nfd == psp->fd))		/* matches */
	{
      nodeb.npsp = NULL;			/* clear nodeb of this file psp */
      nodeb.nfd  = NULLFD;			/* clear nodeb of this file fd */
		nodeb.nde  = NULLNDE;		/* clear nodeb of node number */
	}


#ifdef TRANSACTION
   if ((nodet.npsp == psp) && (nodet.nfd == psp->fd))		/* matches */
	{
      nodet.npsp = NULL;			/* clear nodet of this file psp */
      nodet.nfd  = NULLFD;			/* clear nodet of this file fd */
		nodet.nde  = NULLNDE;		/* clear nodet of node number */
	}
#endif	/* TRANSACTION */

	if (bcbnum == 0)						/* no buffers */
		return(CCOK);

#ifdef TSTBCB
	tstbcb(psp);
#endif

	ec = CCOK;

	nxthnd = topbcbhnd;							/* start at top of bcb chain */
	while ((nxthnd != NULLNEXT))				/* loop to end of chain */
	{
		curhnd = nxthnd;
		bcbptr = cilockbuf(psp, curhnd);		/* lock buffer for processing */

		/* if bcb matches */
	   if ((bcbptr->bpsp == psp) && (bcbptr->bfd == psp->fd))
	   {
	      if (bcbptr->bstatus == DIRTY)		/* need to write to disk */
	      {
	         /* MU */
	         ec = bcbwrite(psp, bcbptr);	/* write the dirty buffer */
	         if (ec != CCOK) 					/* if error occured */
				{
					ciunlockbuf(psp, curhnd);	/* unlock current buffer */
					break;							/* and stop processing */
				}
	      }
	      bcbptr->bpsp = NULL;					/* clear bcb of this file */
	      bcbptr->bfd  = NULLFD;
			bcbptr->bupdatnum = 0;				/* clear update # indicator */
	   }

		nxthnd = bcbptr->bnext;					/* next bcb in chain */
		ciunlockbuf(psp, curhnd);				/* unlock current buffer */
	}

	if (!ec)
		ec = osbflush(psp);						/* flush the os buffers */

	return(ec);

}


int FDECL bflush(psp)   /* flush buffers  */
CFILE *psp;		/* in case header needs to be updated */
{
	int ec;							/* write error code */
	BCBPTR bcbptr;			/* pointer to buffer */
	ALLOCHND curhnd;				/* ptr to allocated buffer location */
	ALLOCHND nxthnd;				/* ptr to allocated buffer location */

	if (bcbnum == 0 || topbcbhnd == 0)			/* no buffers */
	{
		ec = osbflush(psp);					/* flush the os buffers */
		return(ec);
	}

#ifdef TSTBCB
	tstbcb(psp);
#endif

	ec = CCOK;

	nxthnd = topbcbhnd;							/* start at top of bcb chain */
	do 
	{
		curhnd = nxthnd;
	   bcbptr = cilockbuf(psp, curhnd);		/* lock buffer for processing */

		/* if bcb matches */
	   if ((bcbptr->bpsp == psp) && (bcbptr->bfd == psp->fd))
	   {
	      if (bcbptr->bstatus == DIRTY) 	/* need to update */
	      {
	         /* MU */
	         ec = bcbwrite(psp, bcbptr);	/* write the dirty buffer */
	         if (ec != CCOK) 					/* if error occurred */
				{
					ciunlockbuf(psp, curhnd);	/* unlock the buffer */
					break;							/* and stop processing */
				}
	      }
	      bcbptr->bstatus = CLEAN;			/* make buffer clean */
	   }

		nxthnd = bcbptr->bnext;					/* next item in chain */
		ciunlockbuf(psp, curhnd);
	} while (nxthnd != NULLNEXT);				/* traverse bcb chain */

	/* always flush buffers to disk here, regardless of security level */
	if (!ec)
		ec = osbflush(psp);					/* flush the os buffers */

	return(ec);
}

