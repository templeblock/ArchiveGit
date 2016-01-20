/*  cndxnt.c - nt using visual c 32 bit nt compiler - release 1.1
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


/*
 *   The C-Index/II header files must be included (which
 *   will in turn include the compiler specific header
 *   file).
 */

#include "cndx.h"
#include "cindexwk.h"


/*
 *   The nearseg global variable is used to store the value
 *   of the segment register for speeding up transfer of
 *   buffers from far addresses to near static work buffers.
 */

#ifdef MIXED_MODEL_BUFFERS
unsigned int nearseg;							/* near segment address */
#endif	/* MIXED_MODEL_BUFFERS */


/*
 * initialize system for compiler specific operations
 *
 */

int FDECL initcindex(psp)
struct passparm *psp;
{
	return(CCOK);
}


/*
 *   This set of functions must be used for computers that
 *   require word boundary alignment access to ints and
 *   longs.
 */

#ifdef REQUIRES_WORD_ALIGNMENT
void FDECL putint(void *dstadr, int srcvar)
	{ memcpy(dstadr, &srcvar, sizeof(int)); }
void FDECL ciputshort(void *dstadr, short srcvar)
	{ memcpy(dstadr, &srcvar, sizeof(short)); }
void FDECL ciputlong(void *dstadr, long srcvar)
	{ memcpy(dstadr, &srcvar, sizeof(long)); }
void FDECL putfloat(void *dstadr, float srcvar)
	{ memcpy(dstadr, &srcvar, sizeof(float)); }
void FDECL putdouble(void *dstadr, double srcvar)
	{ memcpy(dstadr, &srcvar, sizeof(double)); }

int FDECL retint(void *valptr)			{ return(*((int *)valptr)); }
short FDECL retshort(void *valptr)		{ return(*((short *)valptr)); }
long FDECL retlong(void *valptr)			{ return(*((long *)valptr)); }
float FDECL retfloat(void *valptr)		{ return(*((float *)valptr)); }
double FDECL retdouble(void *valptr)	{ return(*((double *)valptr)); }
#else

#ifndef NO_WORD_CONVERSION
void putfloat(void *dstadr, float srcvar)
	{ memcpy(dstadr, &srcvar, sizeof(float)); }
void putdouble(void *dstadr, double srcvar)
	{ memcpy(dstadr, &srcvar, sizeof(double)); }

float retfloat(valptr) void *valptr;
  { float wrkval; memcpy(&wrkval, valptr, sizeof(wrkval)); return(wrkval); }
double retdouble(valptr) void *valptr;
  { double wrkval; memcpy(&wrkval, valptr, sizeof(wrkval)); return(wrkval);}
#endif	/* !NO_WORD_CONVERSION */

#endif	/* REQUIRES_WORD_ALIGNMENT */

/*
 *   This function forces the buffers managed by the
 *   operating system to be written to disk.  The method for
 *   performing this operation is very operating system
 *   dependent.
 */

int FDECL osbflush(psp)
struct passparm *psp;
{
	if (psp->fd);					/* do nothing to fake out compiler warnings */
   FlushFileBuffers(psp->fd);
  	/* bdos(0xD, 0, 0); */
	return(CCOK);
}



/* isnetfh() - return true if file handle is on remote disk
 *
 *             Coding this routine to actually return this information
 *             will improve performance when running under PCNet software
 *             where the file is on the local disk.  Otherwise, there
 *             is no need for this routine to be modified.
 */

int FDECL isnetfh(psp)
struct passparm *psp;
{
	if (psp->fd);					/* do-nothing to fake out compiler warnings */
	return (1);
}

/* upddisk - update disk
 *   For MSDOS 3.1 and 3.2 this function closes and reopens
 *   file to update the disk FAT table.  For MSDOS 3.3 it
 *   performs a disk commit operation.  This function may
 *   simply return CCOK for operating systems which
 *   automatically update file allocation information.
 */

int FDECL upddisk(psp)			/* update FAT table on disk */
CFILE *psp;
{
   if (TRUE == FlushFileBuffers(psp->fd))
   {
      return(CCOK);
   }
   return(FAIL);
}


/* ---------------------- */
/* file locking functions */
/* ---------------------- */
/*
 *   The following routines lock, unlock, or test the lock
 *   status of a specific byte in the file.  The functions
 *   noted as "blocking" do not return until the appropriate
 *   status is encountered.
 */

int FDECL lockbyte(psp, byte)				/* lock specific byte in file, blocking */
struct passparm *psp;
int byte;
{



	/**/

   while(FALSE == LockFile(psp->fd, (DWORD)byte, (DWORD)0, (DWORD)1, (DWORD)0))
      Sleep(0);

   return(CCOK);
}


int FDECL tlockbyte(psp, byte)			/* lock specific byte in file, non-blocking */
struct passparm *psp;
int byte;
{
	int ret = CCOK;

	/**/

   if (FALSE == LockFile(psp->fd, (DWORD)byte, (DWORD)0, (DWORD)1, (DWORD)0))
   {
      ret = FAIL;
   }

   return(ret);
}


int FDECL unlockbyte(psp, byte)				/* unlock specific byte in file */
struct passparm *psp;
int byte;
{
	int ret = CCOK;

	/**/

   if (FALSE == UnlockFile(psp->fd, (DWORD)byte, (DWORD)0l, (DWORD)1L, (DWORD)0L))
   {
      ret = FAIL;
   }

   return(ret);
}


int FDECL tstlockbyte(psp, byte)			/* test lock of specific byte in file */
struct passparm *psp;
int byte;
{
	int ret = FAIL;
#ifdef READSHARE
	char    test;
#endif

	/**/

#ifdef READSHARE
	/* test lock status by reading byte, if can't read, must be locked */
	ret = fileSeek(psp, (long) byte, 0, 0L);
	if (ret == CCOK)
	{
		ret = ciread(psp,&test,1);
		if (ret)
			ret = FAIL;
	}
#else
   if (TRUE == LockFile(psp->fd, (DWORD)byte, (DWORD)0, (DWORD)1, (DWORD)0))
   {
      if(TRUE == UnlockFile(psp->fd, (DWORD)byte, (DWORD)0, (DWORD)1, (DWORD)0))
      ret = CCOK;
   }
#endif	/* READSHARE */

   return(ret);				/* FAIL = must have lock or other problem */
}


#ifdef READSHARE
/* lock all read bytes in header */
int FDECL cireadlock(psp, allflag)
struct passparm PDECL *psp;
int allflag;
{
	int ret = CCOK;

	if (psp->readstat != NOREADLOCK)
	{
		if (psp->readstat == ALLREADLOCK)
			ret = (FALSE == UnlockFile(psp->fd, (DWORD)(READOFF), (DWORD)0L, (DWORD)MAXUSERS, (DWORD)0L)) ? FILELOCKED : CCOK;
		else
			ret = (FALSE == UnlockFile(psp->fd, (DWORD)(psp->sharenum+READOFF), (DWORD)0L, (DWORD)1L, (DWORD)0L)) ? FILELOCKED : CCOK;

		psp->readstat = NOREADLOCK;		/* clear lock status */
	}

	if (ret == CCOK)
	{
		if (allflag)
		{
		   while(FALSE == LockFile(psp->fd, (DWORD)(READOFF), (DWORD)0L, (DWORD)MAXUSERS, (DWORD)0L))
			   Sleep(0);
		}
		else
		{
		   while(FALSE == LockFile(psp->fd, (DWORD)(psp->sharenum+READOFF), (DWORD)0L, (DWORD)1, (DWORD)0L))
			   Sleep(0);
		}

		if (allflag)
			psp->readstat = ALLREADLOCK;			/* locked all read bytes */
		else
			psp->readstat = ONEREADLOCK;			/* locked one read byte */
	}
	else
		psp->retcde = ret;

	return(ret);
}


int FDECL cireadunlock(psp)
struct passparm *psp;
{
	int ret;

	if (psp->readstat == ALLREADLOCK)
		ret = (FALSE == UnlockFile(psp->fd, (DWORD)(READOFF), (DWORD)0L, (DWORD)MAXUSERS, (DWORD)0L)) ? FILELOCKED : CCOK;
	else
		ret = (FALSE == UnlockFile(psp->fd, (DWORD)(psp->sharenum+READOFF), (DWORD)0L, (DWORD)1L, (DWORD)0L)) ? FILELOCKED : CCOK;

	if (ret == CCOK)
	{
		psp->readstat = NOREADLOCK;			/* unlocked read bytes */
	}
	else
		psp->retcde = ret;

	return(ret);
}
#endif		/* READSHARE */


/* replace low lock code with more efficient compiler specific version */
#ifdef NEWLOWLOCK
int FDECL lowlock(psp)	/* lock file for shared file access */
CFILE PDECL *psp;
{
	int ret;
	int ec;

	/**/

	if (psp->processmode != EXCL && psp->filestat == UNLOCKED)
	/* need to lock it and not locked by high level functions */
	{
		if (psp->writeque)
		{
			/* look for first available position in queue */
			ec = -1;
			psp->curquepos = psp->endque;		/* start where end of que was las time */
			ret = CCOK;								/* assume success */
			while (ec)
			{
				ec = tstlockbyte(psp, FLOCKBYTE + psp->curquepos);
				if (ec)
				{
					psp->curquepos++;		/* keep looking for empty pos in que */
					psp->endque++;
					if (psp->endque > MAXUSERS)
					{
						ret = USRERR;			/* seem to be too many users in file */
						break;
					}
				}
				else
				{
					ec = tlockbyte(psp, FLOCKBYTE + psp->curquepos);
				}
			}

			if (ret == CCOK)
			{
				/* now attempt to work up to first position of queue */
				while (psp->curquepos)
				{
#ifdef READSHARE
					ec = tstlockbyte(psp, FLOCKBYTE + psp->curquepos - 1);	/* try locking lower byte */
					if (!ec)
					{
						psp->endque = psp->curquepos - 1;		/* found end of que faster */
					}
#endif	/* READSHARE */

					ec = lockbyte(psp, FLOCKBYTE + psp->curquepos - 1);	/* lock lower que position (wait) */
					if (ec)
					{
						unlockbyte(psp, FLOCKBYTE + psp->curquepos);  
						ret = BADQUE;			/* problem in maintaining que */
						break;
					}
					else
					{
						ec = unlockbyte(psp, FLOCKBYTE + psp->curquepos);	/* unlock higher que position (moved up) */
						if (ec)
						{
							ret = BADQUE;
							break;
						}

						psp->curquepos--;		/* keep moving up que */
					}
				}
			}

			if (ret == CCOK)
			{
				psp->filestat = LOCKED;
			}
		}
		else		/* else use single byte write lock */
		{
			ret = lockbyte(psp, FLOCKBYTE);

			if (ret == CCOK)
				psp->filestat = LOCKED;
		}
	}
	else
	{
		ret = CCOK;
	}

	return(ret);
}


int FDECL lowunlock(psp)	/* unlock file for low level functions */
CFILE PDECL *psp;
{
	int ret;

	/**/

	/* if need to unlock it */
	if (psp->processmode != EXCL && psp->filestat != UNLOCKED)
	{
		ret = unlockbyte(psp, FLOCKBYTE);

		if (ret == CCOK)
		   psp->filestat = UNLOCKED;

		return(ret);
	}

	return(CCOK);
}


/* wait for low lock by another process to end (permit start of reading) */
int FDECL waitlowlock(psp)
struct passparm PDECL *psp;
{
	int ret;

#ifdef READSHARE
	ret = tstlockbyte(psp, FLOCKBYTE);
	if (ret)		/* if failed read test - use normal method for locking file */
	{
		ret = lowlock(psp);						/* do low level lock of file */
		if (ret == CCOK)
			ret = lowunlock(psp);				/* do low level unlock of file */
	}
#else
	ret = lowlock(psp);						/* do low level lock of file */
	if (ret == CCOK)
		ret = lowunlock(psp);				/* do low level unlock of file */
#endif	/* READSHARE */

	return(ret);
}


#endif	/* NEWLOWLOCK */



/*
 *   This function returns true (1) if the operating system
 *   is able to perform byte level record locking.  For
 *   MSDOS, this function tests which version of the
 *   operating system is in use and returns the correct
 *   status.  If the operating system does not ever support
 *   byte level record locking, this function can always
 *   return false (0) and the above locking functions can be
 *   do-nothing functions returning CCOK, since they will
 *   never get called under those circumstances.
 */

int FDECL mtstos()			/* test if running os that can do locking */
{
	return(CCOK);
}


	

/* ------------------------------ */
/* file open and create functions */
/* ------------------------------ */

/*
 *   Create, open and close files in "exclusive mode".  This
 *   is the same as saying to have the file open with a lock
 *   on the entire file.
 */

FILHND FDECL mexlcreate(psp, name)	/* create file in exclusive mode */
struct passparm *psp;
char *name;
{
	FILHND ret;

	ret = CreateFile(
      name,
      GENERIC_READ | GENERIC_WRITE,
      0,
      (LPSECURITY_ATTRIBUTES)NULL,
      CREATE_NEW,
      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
      (HANDLE)NULL
      );


   return(psp->fd = ret);
}


FILHND FDECL mexlopen(psp, name, rdmode)	/* open file in exclusive mode */
struct passparm *psp;
char *name;
int rdmode;
{
	FILHND ret;

   /* WIN32 */
   if (rdmode == CRDWRITE)
   {
      ret = CreateFile(
         name,
         GENERIC_READ | GENERIC_WRITE,
         0,
         (LPSECURITY_ATTRIBUTES)NULL,
         OPEN_EXISTING,
         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
         (HANDLE)NULL
         );
   }
   else
   {
      ret = CreateFile(
         name,
         GENERIC_READ,
         0,
         (LPSECURITY_ATTRIBUTES)NULL,
         OPEN_EXISTING,
         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
         (HANDLE)NULL
         );


   }

	return(psp->fd = ret);
}


int FDECL mexlclose(psp)		/* close file from exclusive mode */
struct passparm *psp;
{
	return((CloseHandle(psp->fd) == TRUE) ? CCOK : FAIL);
}


/*
 *   Open and close files in "shared" mode, which allows
 *   other processes to access the file.  Note that there is
 *   no shared mode create.  All files are created in
 *   exclusive mode and then reopened in either exclusive
 *   mode or shared mode.
 */

FILHND FDECL mshropen(psp, name, rdmode)	/* open file in shared mode */
struct passparm *psp;
char *name;
int rdmode;
{
	FILHND ret;
	DWORD exterr;

	if (rdmode == CRDWRITE)		/* if normal read/write */
   {

      ret = CreateFile(
         name,
         GENERIC_READ | GENERIC_WRITE,
         FILE_SHARE_READ | ((psp->filemode == DENYWRITE) ? 0 : FILE_SHARE_WRITE),
         (LPSECURITY_ATTRIBUTES)NULL,
         OPEN_EXISTING,
         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
         (HANDLE)NULL
         );

   }
	else								/* else read-only */
   {

      ret = CreateFile(
         name,
         GENERIC_READ,
         FILE_SHARE_READ | FILE_SHARE_WRITE,
         (LPSECURITY_ATTRIBUTES)NULL,
         OPEN_EXISTING,
         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
         (HANDLE)NULL
         );
   }

	if (ret == NULLFD)
	{
		exterr = GetLastError();
		psp->portlong = exterr;
	}

	return(psp->fd = ret);
}


int FDECL mshrclose(psp)		/* close a file from shared mode */
struct passparm *psp;
{
	return((CloseHandle(psp->fd) == TRUE) ? CCOK : FAIL);
}


/*
 *   Deletes the named file.  Normally the Unix style unlink
 *   function.
 */

int FDECL ciunlink(name)
char *name;						/* file name to unlink */
{
	return(DeleteFile(name));
}


/*
 *   Positions the file for performing reads, writes, or
 *   locks.  This routine is designed to allow for very
 *   large file addresses consisting of a 32 bit node
 *   location (adr) multiplied by a 16 bit node size value
 *   (recsize).  The offset value is used to position past
 *   the file header (usually 1K).
 */

int FDECL fileSeek(psp, adr, recsize, offset)
struct passparm *psp;
long adr;			/* address to seek */
int recsize;		/* record size multiplier */
long offset;		/* offset from beginning of file (skip over header) */
{
	int ret;
	long pos;

	if (recsize)
		pos = (adr * (long)recsize) + offset;	/* record size multiplier */
	else
		pos = adr + offset;

	ret = CCOK;
   if(-1 == SetFilePointer(
      psp->fd,
      pos,
      NULL,
      FILE_BEGIN
      ))
     {
        ret = FAIL;
     }

	return(ret);
}

/* READ OR WRITE BLOCK OF BYTES */

/*
 *   Low level disk read and write routines.  Normally these
 *   will use the Unix style read() and write() functions.
 *   The following Microsoft C code contains
 *   special handling for out of disk space conditions
 *   (NOSPACE error) which will work only with MSDOS and
 *   OS/2 systems.
 *
 *   It is also possible to use fread and fwrite functions
 *   for systems that do not support low level disk I/O.
 *   This will also require changing the create, open and
 *   close routines and the typedef of FILHND specified in
 *   the library header file.
 */

int FDECL ciread(psp, adr, sze)
struct passparm *psp;
void *adr;
unsigned sze;
{
	DWORD szeread;
	int ret = CCOK;

   if (FALSE == ReadFile(
      psp->fd,
      adr,
      sze,
      &szeread,
      NULL
      ))
   {
      ret = FAIL;
   }
   else
   {
   	if (szeread == sze)
	   	ret = CCOK;
	   else
		   ret = READERR;
   }
	return(ret);
}

/* write block of bytes returns CCOK or FAIL  */
int FDECL ciwrite(psp, adr, sze)
struct passparm *psp;
void *adr;
unsigned sze;
{
	DWORD szewrite;
	int ret;

   if (FALSE == WriteFile(
      psp->fd,
      adr,
      sze,
      &szewrite,
      NULL
      ))
   {
      ret = NOSPACE;
   }
	else
   {
   	if (szewrite == sze)
	   	ret = CCOK;
	   else
		   ret = NOSPACE;
   }
	return(ret);
}

/* -------------------- current key processing --------------------- */

/*
 *   All of the current key functions will need to be modified to
 *   handle memory allocation for other types of operating
 *   systems, such as Windows or Macintosh.  A current key
 *   structure is stored in allocated space for each index that
 *   is accessed by an application.  When the file is opened, the
 *   pointers to allocated space are all set to NULL.  With each
 *   access to an index, space is allocated in the lockcuradr
 *   function if it is not already allocated.  When the file is
 *   closed the space is deallocated by the endcur function.  For
 *   MSDOS, Unix and many other systems that do not require
 *   locking allocated memory before access, the unlockcuradr
 *   function is not used by those systems and the function is
 *   defined as a null function in the library header (such as
 *   cndxmsc.h).  All of the current key processing functions will
 *   require modification for systems that allocate memory in
 *   movable blocks.
 */

/* ------------------------------------- */
/* current pointer string initialization */
/* ------------------------------------- */

/*
 *   Initializes the current key pointers to NULL.  If NULL
 *   does not correctly indicate space that has not been
 *   allocated, this routine will need to be changed.
 */

int FDECL initcurs(psp)			/* initialize current pointers */
CFILE *psp;
{
	int i;

	/**/

	for (i = 1; i < NUMINDXS; i++)
	{
		psp->curinfoptr[i] = NULL;		/* clear all ptrs to cur key info */
		psp->curlock[i] = UNLOCKED;	/* start with indexes all unlocked */
	}

	return(CCOK);
}


/*
 *   Deallocated all space allocated while the file was
 *   open.  This uses the Unix style free function for most
 *   systems.
 */

int FDECL endcurs(psp)			/* deallocate all current information */
CFILE *psp;
{
	int i;

	/**/

	for (i = 1; i < NUMINDXS; i++)
	{
		if (psp->curinfoptr[i])				/* if space allocated for key info */
		{
			free(psp->curinfoptr[i]);		/* free the space */
			psp->curinfoptr[i] = NULL;		/* clear all ptrs to cur key info */
		}
	}

	return(CCOK);
}

/*
 *   Locates and locks the address of current key info.
 *   For normal MSDOS and UNIX type systems, use the malloc
 *   function for dynamically allocating space.  For Windows
 *   and Macintosh type systems, the curkyptr array in the
 *   psp must be a handle instead of a pointer. The
 *   lockcuradr function will need to locate the actual
 *   address and lock it.
 */

CURINFO * FDECL xlockcuradr(psp, keyn)
CFILE *psp;
int keyn;
{
	CURINFO *ret;

	/* if we have already allocated a key info record */
	if (psp->curinfoptr[keyn])
	{
		ret = psp->curinfoptr[keyn];		/* return pointer to existing info */
	}
	else
	{
		/* allocate space and set ptrs to current record */
		ret = malloc(sizeof(CURINFO));
		if (ret)
		{
			psp->curinfoptr[keyn] = ret;		/* return pointer to new info */

			/* initialize the new current information record */
			*(ret->keystr) = '\0';		/* null string key */
	      ret->currcval = 0;			/* zero record # */
			ret->curdbyte = 0;			/* zero dbyte */
			ret->keynum = 0;				/* zero current key number */
			ret ->ndenum = NULLNDE;		/* no node number */
	      ret->curindx = -1;			/* no current index # */
			ret ->delflag = FALSE;		/* no delete just performed */
			ret->updkeyinfo = TRUE;		/* EXCL key info may not be current */
		}
		else
			psp->memerr = OUTOFMEM;		/* out of memory error */
	}

	return(ret);
}


/*
 *   This function is required only for Windows and
 *   Macintosh type systems.  The function should unlock the
 *   allocated space for the keyn index.
 */

#ifdef MEMORY_MANAGEMENT_REQUIRED
void FDECL unlockcuradr(psp, keyn)
CFILE *psp;
int keyn;
{
}
#endif

/* The following two functions copy key information from the allocated
 * current key structure into local memory.  If you are using mixed
 * memory models (such as a medium model Windows program with a far heap
 * for allocating the current key structures) you will need to changes
 * these routines to copy from a far segment to a near segment.
 */

/* copy current key information from allocated memory */
void FDECL getcurkey(psp, keyn, dstadr)
CFILE *psp;
int keyn;
char *dstadr;
{
	CURINFO *infoadr;

	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
		cikeycpy(psp, keyn, dstadr, infoadr->keystr);
	else
		*dstadr = '\0';					/* null string default on error */

	unlockcuradr(psp, keyn);		/* release lock on memory */
}

/* copy current key information to allocated memory */
void FDECL putcurkey(psp, keyn, srcadr)
CFILE *psp;
int keyn;
char *srcadr;
{
	CURINFO *infoadr;
	
	infoadr = lockcuradr(psp, keyn);
	if (infoadr)
	{
#ifdef REMOVED
		if (strlen(srcadr) >= MAXMKLEN)
			*infoadr->keystr = '\0';		/* error, null string */
		else
			strcpy(infoadr->keystr, srcadr);
#endif
		if (cikeylen(psp, keyn, srcadr) > MAXMKLEN)
			*infoadr->keystr = '\0';		/* error, null string */
		else
			cikeycpy(psp, keyn, infoadr->keystr, srcadr);
	}
	unlockcuradr(psp, keyn);		/* release lock on memory */
}


/* ------ routines for allocating and accessing buffer locations ------- */

/*
 *   This allocates one bcb space out of a heap.  For most compilers
 *   this requires using the normal malloc function.  For mixed model
 *   programming where the buffers will be allocated from the
 *   far heap, this must use _fmalloc.  For Windows and Macintosh
 *   programs, this needs to be modified to use the appropriate
 *   allocate call for that system.
 */

/* allocate one buffer bcb space */
ALLOCHND FDECL ciallocbuf(psp, size)
struct passparm *psp;
int size;								/* number of bytes to allocate for buffers */
{
#ifdef MIXED_MODEL_BUFFERS
	char far *retadr;

	retadr = _fmalloc(size);			/* allocate out of far heap */
#else
	char *retadr;

	retadr = malloc(size);				/* allocate out of system heap */
#endif	/* MIXED_MODEL_BUFFERS */

	if (!retadr)
			psp->memerr = OUTOFMEM;		/* out of memory error */

	return(retadr);
}

/*
 *   This deallocates one bcb space from the heap.  For most compilers
 *   this requires using the normal malloc function.  For mixed model
 *   programming where the buffers will be allocated from the
 *   far heap, this must use _fmalloc.  For Windows and Macintosh
 *   programs, this funciton will need to be modified to use the appropriate
 *   allocate call for that system.
 */

/* deallocate one buffer bcb space */
int FDECL cideallocbuf(psp, handle)
struct passparm *psp;
ALLOCHND handle;					/* handle to space to deallocate */
{
	if (psp);						/* do nothing to fool compiler */
   free(handle);
	return(CCOK);
}



/*
 *   These two function lock and unlock buffers allocated in
 *   movable heap space.  These need to be modified to support
 *   that operation by calling the appropriate functions for
 *   Windows or Macintosh environments.
 */

#ifdef MEMORY_MANAGEMENT_REQUIRED
/* lock buffer handle and return address */
BCBPTR FDECL cilockbuf(psp, hnd)
struct passparm *psp;					/* file parameters */
ALLOCHND hnd;								/* handle to lock */
{
	if (psp);								/* do nothing to fool compiler */

	return((struct bcb *)hnd);
}


/* unlock buffer handle */
void FDECL ciunlockbuf(psp, hnd)
struct passparm *psp;					/* file parameters */
ALLOCHND hnd;								/* handle to lock */
{
	if (psp);								/* do nothing to fool compiler */

	return;
}
#endif		/* MEMORY_MANAGEMENT_REQUIRED */


/*
 * These routines are required to move buffers in mixed model systems,
 * such as under MSWindows where the code is in medium model but the
 * buffers are allocated from the far heap.
 */

#ifdef MIXED_MODEL_BUFFERS
/* copy buffer from far bcb buffer to near temporary buffer */
void FDECL buftonear(nearbuf, farbcb, bufsize)
struct node *nearbuf;				/* destination buffer */
BCBPTR farbcb;						 	/* source buffer bcb */
unsigned bufsize;						/* size of buffer to write */
{
	char far *farbuf = (char far *)&farbcb->bbuffer;
	movedata(FP_SEG(farbuf), FP_OFF(farbuf),
				nearseg, (unsigned) nearbuf,
				bufsize);
}

/* copy buffer from near temporary buffer to far bcb buffer */
void FDECL buftofar(farbcb, nearbuf, bufsize)
BCBPTR farbcb;							/* source buffer bcb */
struct node *nearbuf;				/* destination buffer */
unsigned bufsize;						/* size of buffer to write */
{
	char far *farbuf = (char far *)&farbcb->bbuffer;
	movedata(nearseg, (unsigned) nearbuf,
				FP_SEG(farbuf), FP_OFF(farbuf),
				bufsize);
}
#endif	/* MIXED_MODEL_BUFFERS */


/*
 *   The key comparison operation is called by the keytest() function
 *   from within C-Index.  It may require modification because the
 *   the Microsoft C/ANSI/Unix style functions are not supported by
 *   some compilers.  In addition, when defining custom key types,
 *   the key comparison aspect of the custom key must be defined
 *   in this routine.
 */

/* performs comparison on key values */
int FDECL cikeycmp(psp, keyn, keytype, entptr, tstptr)
struct passparm *psp;			/* file info */
int keyn;							/* index number being tested */
int keytype;						/* type of key to compare */
void *entptr;						/* ptr to entry key value */
void *tstptr;						/* ptr to test key value */
{
	int ret;							/* result of key comparison */
	int entlen;						/* length of entry binary key */
	int tstlen;						/* length of test binary key */
	int entseglen;					/* length of entry key segment */
	int tstseglen;					/* length of test key segment */
	int keylen;						/* length of binary key to use in comparison */
	unsigned char *entwrkptr;	/* ptr into entry segmented key */
	unsigned char *tstwrkptr;	/* ptr into test segmented key */
	KEYSEGLIST *seginfo;				/* segment info list */
	int i;

	ret = 0;					/* prevent compiler warning */

	switch(keytype & INDTYPEMASK)
	{
		case STRINGIND:
			ret = strcmp(entptr, tstptr);		/* ASCIIZ string key */
			break;

		case NOCASEIND:
			ret = strcmpi(entptr, tstptr);	/* ASCIIZ key, ignore case */
			break;

		case BINARYIND:											/* binary key */
			entlen = (int)(*((unsigned char *)entptr));	/* 1st byte is length */
			tstlen = (int)(*((unsigned char *)tstptr));	/* 1st byte is length */
			keylen = (entlen < tstlen) ? entlen : tstlen;	/* use smaller len */
			ret = memcmp(((char *)entptr+1), ((char *)tstptr+1), keylen);
			if (!ret)
				ret = entlen - tstlen;		/* longer key is larger */
			break;

		/* assume FIXIND value is sequence of byte values */
		case FIXIND:
			ret = memcmp((char *)entptr, (char *)tstptr, FIXINDLEN);
			break;

		case INTIND:							/* integer key */
			ret = ((*((int *)entptr) > *((int *)tstptr)))
					? 1
					: (((*((int *)entptr) < *((int *)tstptr)))
						? -1
						: 0);
			break;

		case SHORTIND:							/* short key */
			ret = ((*((INT16 *)entptr) > *((INT16 *)tstptr)))
					? 1
					: (((*((INT16 *)entptr) < *((INT16 *)tstptr)))
						? -1
						: 0);
			break;

		case USHORTIND:							/* short key */
			ret = ((*((unsigned short *)entptr) > *((unsigned short *)tstptr)))
					? 1
					: (((*((unsigned short *)entptr) < *((unsigned short *)tstptr)))
						? -1
						: 0);
			break;

		case LONGIND:							/* long key */
			ret = ((*((INT32 *)entptr) > *((INT32 *)tstptr)))
					? 1
					: (((*((INT32 *)entptr) < *((INT32 *)tstptr)))
						? -1
						: 0);
			break;

		case FLOATIND:							/* float key */
			ret = ((*((float *)entptr) > *((float *)tstptr)))
					? 1
					: (((*((float *)entptr) < *((float *)tstptr)))
						? -1
						: 0);
			break;

		case DOUBLEIND:						/* double key */
			ret = ((*((double *)entptr) > *((double *)tstptr)))
					? 1
					: (((*((double *)entptr) < *((double *)tstptr)))
						? -1
						: 0);
			break;

		case SEGIND:												/* segmented index */
			entlen = (int)(*((unsigned char *)entptr));	/* 1st byte is length */
			tstlen = (int)(*((unsigned char *)tstptr));	/* 1st byte is length */
			entwrkptr = ((unsigned char *)entptr) + 1;	/* set wrk pointers */
			tstwrkptr = ((unsigned char *)tstptr) + 1;
			seginfo = psp->segptr[keyn];						/* key segment info */
			for (	i = 0;
					seginfo[i].keytype != ENDSEGLIST;
					i++)
			{
				ret = cikeycmp(psp, keyn, seginfo[i].keytype, entwrkptr, tstwrkptr);
				if (ret)
				{
					if (seginfo[i].descending)			/* if descending key order */
						ret = (ret > 0) ? -1 : 1;		/* reverse comparison order */
					break;			/* done, test not equal */
				}

				entseglen = _cikeylen(seginfo[i].keytype, entwrkptr);
				tstseglen = _cikeylen(seginfo[i].keytype, tstwrkptr);
				entwrkptr += entseglen;			/* skip to next key segment */
				tstwrkptr += tstseglen;
				entlen -= entseglen;				/* check past end of whole key */
				tstlen -= tstseglen;
				if ((entlen <= 0) || (tstlen <= 0))
					break;		/* past end of key segments, must keys be equal */
			}
			break;

		case CUST1IND:												/* custom index type 1*/
		case CUST2IND:												/* custom index type 2*/
		case CUST3IND:												/* custom index type 3*/
		case CUST4IND:												/* custom index type 4*/
		case CUST5IND:												/* custom index type 5*/
			/* this routine must be written for each custom key required */
			/* defaults to binary key type */
			entlen = (int)(*((unsigned char *)entptr));	/* 1st byte is length */
			tstlen = (int)(*((unsigned char *)tstptr));	/* 1st byte is length */
			keylen = (entlen < tstlen) ? entlen : tstlen;	/* use smaller len */
			ret = memcmp(((char *)entptr+1), ((char *)tstptr+1), keylen);
			if (!ret)
				ret = entlen - tstlen;		/* longer key is larger */
			break;

		default:
			ret = 0;					/* should not get here, assume equal */
			psp->keyn = keyn;		/* dummy code */
			psp->retcde = FAIL;	/* dummy code */
			psp->memerr = TRUE;	/* fake memory error to cause it to stop */
	}

	return(ret);
}


/*
 *   This routine takes a custom key value that is passed and
 *   returns a new key value that is one unit greater, that is,
 *   the next custom key value that could exist in the index.
 */

/* create a new key value that is one greater that value passed */
void FDECL custincrkeyval(psp, keyn, keyptr)
struct passparm *psp;				/* file info needed to tell type of index */
int keyn;								/* index number being searched */
KEYPTR keyptr;							/* key buffer */
{
	unsigned char keylen;

	switch(psp->indtyp[keyn] & INDTYPEMASK)
	{
		case CUST1IND:												/* custom index type 1*/
		case CUST2IND:												/* custom index type 2*/
		case CUST3IND:												/* custom index type 3*/
		case CUST4IND:												/* custom index type 4*/
		case CUST5IND:												/* custom index type 5*/
		/* this routine must be created for each custom key type */
		/* defaults to binary key type action */
		keylen = (*((unsigned char *)keyptr));			/* 1st byte is length */
		*((char *)keyptr + keylen) = 0;					/* clear end byte */
		*((unsigned char *)keyptr) = ++keylen;			/* incr length */
		break;
	}
}

/* convert key from one bytemode to another, in place */
void FDECL cicustflip(psp, keyptr, keytype, inbytemode, outbytemode)
CFILE PDECL *psp;			/* file parameter structure */
KEYPTR keyptr;				/* ptr to key being flipped */
int keytype;				/* type of key to flip */
INT32 inbytemode;			/* mode of key before flipping */
INT32 outbytemode;		/* mode of key after flipping */
{
	/* put custom byte flipping here for FIXIND and CUSTxIND keys */

}

