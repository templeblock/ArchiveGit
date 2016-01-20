// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
/************************************************************************
 * MODULE intelio.c
 *	This module provides routines to read and write integer values
 *	in intel format from/to files.  This is used by atleast the
 *	following file conversions:
 *		BMP and PCX.
 */

#undef INTELIO_DEBUG

#include "stdafx.h"
#include "intelio.h"

/************************************************************************
 * FUNCTION FileFDOpenRdr
 *	Initialize a control block for doing buffered reads.
 */

void		FileFDOpenRdr( LPFILEBUF ofd, CFile *pFile, LPTR buf, int bsiz )
{
    ofd->pFile = pFile;
    ofd->buf = buf;
    ofd->bsiz = bsiz;
    ofd->valid = 0;
    ofd->offset = 0;
    ofd->mode = 0;
    ofd->pos = 0;
    ofd->err = 0;
}


/************************************************************************
 * FUNCTION FileFDOpenWrt
 *	Initialize a control block for doing buffered writes.
 */

void		FileFDOpenWrt( LPFILEBUF ofd, CFile *pFile, LPTR buf, int bsiz )
{
    ofd->pFile = pFile;
    ofd->buf = buf;
    ofd->bsiz = bsiz;
    ofd->valid = 0;
    ofd->offset = 0;
    ofd->mode = 1;
    ofd->pos = 0;
    ofd->err = 0;
}


/************************************************************************
 * FUNCTION FileFDFlush
 *	Flush buffer for buffered file.
 */

int		FileFDFlush( LPFILEBUF ofd )
{
	/* just report an error, if one has happened */
    if (ofd->err)
	return (-1);

	/* if mode is write, write any data in buffer */
    if (ofd->mode)
	{
		if (ofd->offset)
		{
			TRY
			{
	    		ofd->pFile->Write (ofd->buf, ofd->offset);
			}
			CATCH_ALL(e)
			{
				ofd->err = -1;
			}
			END_CATCH_ALL
		}
    }

	/* set byte offset for first byte in buffer */
    ofd->pos += ofd->offset;

	/* set next location in buffer to use */
    ofd->offset = 0;
    return (ofd->err);
}


/************************************************************************
 * FUNCTION FileFDSeek
 *	Change position in file.
 */

long		FileFDSeek( LPFILEBUF ofd, DWORD offset, int whence )
{
    long	lret;

    FileFDFlush (ofd);

/* Adjust offset for seek_from_current to take buffering into acount */
    if (whence == 1)
	{
		offset += ofd->pos;
		whence = 0;
    }

	TRY
	{
    	lret = ofd->pFile->Seek (offset, whence);
		ofd->pos = lret;
		ofd->valid = 0;
	}
	CATCH_ALL(e)
	{
		ofd->err = -1;
		lret = -1;
	}
	END_CATCH_ALL

    return (lret);
}


/************************************************************************
 * FUNCTION FileFDWrite
 *	Write bytes to buffered file.
 */

int		FileFDWrite( LPFILEBUF ofd, LPTR buf, int len )
{
    int		i;

	/* null write? */
    if (len == 0)
		return (0);

	/* no write if error condition set */
    if (ofd->err)
	return (-1);

	/* will all data will fit in buffer? */
    if (ofd->offset + len < ofd->bsiz)
	{
		for (i = 0; i < len; i++)
	    	ofd->buf [ofd->offset++] = *buf++;
		return (len);
    }

	/* all data won't fit, flush the buffer */
    if (FileFDFlush (ofd) == -1)
	return (-1);

	/* will all data now will fit in buffer? */
    if (ofd->offset + len < ofd->bsiz)
	{
		for (i = 0; i < len; i++)
	    	ofd->buf [ofd->offset++] = *buf++;
		return (len);
    }

	/* just write the data directly from the callers buffer */
	TRY
	{
	    ofd->pFile->Write (buf, len);
		/* set byte offset for first byte in buffer */
    	ofd->pos = ofd->pFile->Seek (0L, 1);
	    return (len);
	}
	CATCH_ALL(e)
	{
		ofd->err = -1;
		return (-1);
	}
	END_CATCH_ALL
}


/************************************************************************
 * FUNCTION FileFDRead
 *	Read bytes to buffered file.
 */

int		FileFDRead( LPFILEBUF ifd, LPTR buf, int len )
{
    int		i;

	TRY
	{
		/* null read? */
    	if (len == 0)
			return (0);

		/* no read if error condition set */
    	if (ifd->err)
			return (-1);

		/* is buffer empty? */
    	if (ifd->valid == 0)
		{
			ifd->valid = ifd->pFile->Read (ifd->buf, ifd->bsiz);
			if (ifd->valid == -1)
			{
	    		ifd->err = -1;
		    	return (-1);
			}
    	}

		/* is all data in buffer? */
    	if (ifd->offset + len < ifd->valid)
		{
			for (i = 0; i < len; i++)
	    		*buf++ = ifd->buf [ifd->offset++];
			return (len);
    	}

		/* give caller the data that is in the buffer */
    	i = len;
    	while (ifd->offset < ifd->valid)
		{
			*buf++ = ifd->buf [ifd->offset++];
			i--;
    	}

		/* indicate that all data has been read and buffer is empty */
    	ifd->valid = 0;
    	ifd->offset = 0;

		/* just read the rest of the data directly into the callers buffer */
    	if (ifd->pFile->Read (buf, i) != i)
		{
			ifd->err = -1;
			return (-1);
    	}

		/* set byte offset for first byte in buffer */
    	ifd->pos = ifd->pFile->Seek (0L, 1);

    	return (len);
	}
	CATCH_ALL(e)
	{
		ifd->err = -1;
		return (-1);
	}
	END_CATCH_ALL
}


/************************************************************************
 * FUNCTION intelWriteWord
 */

void		intelWriteWord( LPFILEBUF ofd, WORD w )
{
    if (FileFDWrite (ofd, (LPTR) &w, sizeof (WORD)) != sizeof (WORD))
		ofd->err = -1;
}


/************************************************************************
 * FUNCTION intelWriteDWord
 */

void		intelWriteDWord( LPFILEBUF ofd, DWORD dw )
{
    if (FileFDWrite (ofd, (LPTR) &dw, sizeof (DWORD)) != sizeof (DWORD))
		ofd->err = -1;
}		


/************************************************************************
 * FUNCTION intelReadWord
 */

void		intelReadWord( LPFILEBUF ifd, WORD FAR *w )
{
    if (FileFDRead (ifd, (LPTR) w, sizeof (WORD)) != sizeof (WORD))
		ifd->err = -1;
}


/************************************************************************
 * FUNCTION intelReadDWord
 */

void		intelReadDWord( LPFILEBUF ifd, DWORD FAR *dw )
{
    if (FileFDRead (ifd, (LPTR) dw, sizeof (DWORD)) != sizeof (DWORD))
		ifd->err = -1;
}


/************************************************************************
 * FUNCTION motWriteWord
 */

void		motWriteWord( LPFILEBUF ofd, WORD w )
{
    swapw (&w);
    if (FileFDWrite (ofd, (LPTR) &w, sizeof (WORD)) != sizeof (WORD))
		ofd->err = -1;
}


/************************************************************************
 * FUNCTION motWriteDWord
 */

void		motWriteDWord( LPFILEBUF ofd, DWORD dw )
{
    swapl (&dw);
    if (FileFDWrite (ofd, (LPTR) &dw, sizeof (DWORD)) != sizeof (DWORD))
		ofd->err = -1;
}


/************************************************************************
 * FUNCTION motReadWord
 */

void		motReadWord( LPFILEBUF ifd, WORD FAR *w )
{
    if (FileFDRead (ifd, (LPTR) w, sizeof (WORD)) != sizeof (WORD))
		ifd->err = -1;
    swapw (w);
}


/************************************************************************
 * FUNCTION motReadDWord
 */

void		motReadDWord( LPFILEBUF ifd, DWORD FAR *dw )
{
    if (FileFDRead (ifd, (LPTR) dw, sizeof (DWORD)) != sizeof (DWORD))
		ifd->err = -1;
    swapl (dw);
}

