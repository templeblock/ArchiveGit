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

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

/************************************************************************
 * FUNCTION FileFDOpenRdr
 *	Initialize a control block for doing buffered reads.
 */

void		FileFDOpenRdr (
LPFILEBUF	ofd,	/* control block to initialize */
int			ofh,	/* handle for file being buffered */
LPTR		buf,	/* pointer to buffer to use */
int			bsiz)	/* size of buffer */
{
    ofd->fh = ofh;
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

void		FileFDOpenWrt (
LPFILEBUF	ofd,	/* control block to initialize */
int			ofh,	/* handle for file being buffered */
LPTR		buf,	/* pointer to buffer to use */
int			bsiz)	/* size of buffer */
{
    ofd->fh = ofh;
    ofd->buf = buf;
    ofd->bsiz = bsiz;
    ofd->valid = 0;
    ofd->offset = 0;
    ofd->mode = 1;
    ofd->pos = 0;
    ofd->err = 0;
}


/************************************************************************
 * FUNCTION FileFlush
 *	Flush buffer for buffered file.
 */

int		FileFlush (LPFILEBUF ofd)
{
/* just report an error, if one has happened */
    if (ofd->err)
	return (-1);

/* if mode is write, write any data in buffer */
    if (ofd->mode) {
	if (ofd->offset) {
	    if (_lwrite (ofd->fh, (LPSTR)ofd->buf, ofd->offset) != ofd->offset)
		ofd->err = -1;
	}
    }

/* set byte offset for first byte in buffer */
    ofd->pos += ofd->offset;

/* set next location in buffer to use */
    ofd->offset = 0;
    return (ofd->err);
}


/************************************************************************
 * FUNCTION FileSeek
 *	Change position in file.
 */

long		FileSeek (
LPFILEBUF	ofd,	/* control block */
long		offset,
int			whence)	/* 0 - start, 1 - current, 2 - end */
{
    long	lret;

    FileFlush (ofd);

/* Adjust offset for seek_from_current to take buffering into acount */
    if (whence == 1) {
	offset += ofd->pos;
	whence = 0;
    }

    lret = _llseek (ofd->fh, offset, whence);
    if (lret == -1)
	ofd->err = -1;
    else {
	ofd->pos = lret;
	ofd->valid = 0;
    }

    return (lret);
}


/************************************************************************
 * FUNCTION FileWrite
 *	Write bytes to buffered file.
 */

int		FileWrite (
LPFILEBUF	ofd,	/* control block */
LPTR		buf,	/* buffer containing data */
int			len)	/* number of bytes to write */
{
    int		i;

/* null write? */
    if (len == 0)
	return (0);

/* no write if error condition set */
    if (ofd->err)
	return (-1);

/* will all data will fit in buffer? */
    if (ofd->offset + len < ofd->bsiz) {
	for (i = 0; i < len; i++)
	    ofd->buf [ofd->offset++] = *buf++;
	return (len);
    }

/* all data won't fit, flush the buffer */
    if (FileFlush (ofd) == -1)
	return (-1);

/* will all data now will fit in buffer? */
    if (ofd->offset + len < ofd->bsiz) {
	for (i = 0; i < len; i++)
	    ofd->buf [ofd->offset++] = *buf++;
	return (len);
    }

/* just write the data directly from the callers buffer */
    if (_lwrite (ofd->fh, (LPSTR)buf, len) != len) {
	ofd->err = -1;
	return (-1);
    }

/* set byte offset for first byte in buffer */
    ofd->pos = _llseek (ofd->fh, 0L, 1);

    return (len);
}


/************************************************************************
 * FUNCTION FileRead
 *	Read bytes to buffered file.
 */

int		FileRead (
LPFILEBUF	ifd,	/* control block */
LPTR		buf,	/* buffer containing data */
int			len)	/* number of bytes to write */
{
    int		i;

/* null read? */
    if (len == 0)
	return (0);

/* no read if error condition set */
    if (ifd->err)
	return (-1);

/* is buffer empty? */
    if (ifd->valid == 0) {
	ifd->valid = _lread (ifd->fh, ifd->buf, ifd->bsiz);
	if (ifd->valid == -1) {
	    ifd->err = -1;
	    return (-1);
	}
    }

/* is all data in buffer? */
    if (ifd->offset + len < ifd->valid) {
	for (i = 0; i < len; i++)
	    *buf++ = ifd->buf [ifd->offset++];
	return (len);
    }

/* give caller the data that is in the buffer */
    i = len;
    while (ifd->offset < ifd->valid) {
	*buf++ = ifd->buf [ifd->offset++];
	i--;
    }

/* indicate that all data has been read and buffer is empty */
    ifd->valid = 0;
    ifd->offset = 0;

/* just read the rest of the data directly into the callers buffer */
    if (_lread (ifd->fh, buf, i) != i) {
	ifd->err = -1;
	return (-1);
    }

/* set byte offset for first byte in buffer */
    ifd->pos = _llseek (ifd->fh, 0L, 1);

    return (len);
}


/************************************************************************
 * FUNCTION intelWriteWord
 */

void		intelWriteWord (
LPFILEBUF	ofd,	/* control block */
WORD		w)
{
    if (FileWrite (ofd, (LPTR) &w, sizeof (WORD)) != sizeof (WORD))
	ofd->err = -1;
}


/************************************************************************
 * FUNCTION intelWriteDWord
 */

void		intelWriteDWord (
LPFILEBUF	ofd,	/* control block */
DWORD		dw)
{
    if (FileWrite (ofd, (LPTR) &dw, sizeof (DWORD)) != sizeof (DWORD))
	ofd->err = -1;
}


/************************************************************************
 * FUNCTION intelReadWord
 */

void		intelReadWord (
LPFILEBUF	ifd,	/* control block */
WORD FAR	*w)
{
    if (FileRead (ifd, (LPTR) w, sizeof (WORD)) != sizeof (WORD))
	ifd->err = -1;
}


/************************************************************************
 * FUNCTION intelReadDWord
 */

void		intelReadDWord (
LPFILEBUF	ifd,	/* control block */
DWORD FAR	*dw)
{
    if (FileRead (ifd, (LPTR) dw, sizeof (DWORD)) != sizeof (DWORD))
	ifd->err = -1;
}


/************************************************************************
 * FUNCTION motWriteWord
 */

void		motWriteWord (
LPFILEBUF	ofd,	/* control block */
WORD		w)
{
    swapw (&w);
    if (FileWrite (ofd, (LPTR) &w, sizeof (WORD)) != sizeof (WORD))
	ofd->err = -1;
}


/************************************************************************
 * FUNCTION motWriteDWord
 */

void		motWriteDWord (
LPFILEBUF	ofd,	/* control block */
DWORD		dw)
{
    swapl (&dw);
    if (FileWrite (ofd, (LPTR) &dw, sizeof (DWORD)) != sizeof (DWORD))
	ofd->err = -1;
}


/************************************************************************
 * FUNCTION motReadWord
 */

void		motReadWord (
LPFILEBUF	ifd,	/* control block */
WORD FAR	*w)
{
    if (FileRead (ifd, (LPTR) w, sizeof (WORD)) != sizeof (WORD))
	ifd->err = -1;
    swapw (w);
}


/************************************************************************
 * FUNCTION motReadDWord
 */

void		motReadDWord (
LPFILEBUF	ifd,	/* control block */
DWORD FAR	*dw)
{
    if (FileRead (ifd, (LPTR) dw, sizeof (DWORD)) != sizeof (DWORD))
	ifd->err = -1;
    swapl (dw);
}
