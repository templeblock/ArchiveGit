//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _INTELIO_H_
#define _INTELIO_H_

#include "imgutil.h"

/************************************************************************/
/* Buffered IO structure */
/************************************************************************/
typedef struct _filebuf
	{
	LPTR	buf;	/* pointer to buffer */
	DWORD	bsiz;	/* size of buffer */
	DWORD	valid;	/* number of valid bytes in buffer (read) */
	DWORD	offset;	/* next byte to read/write in buffer */
	CFile 	*pFile;	/* CFile pointer */
	int	mode;	/* 1 = write, 0 = read */
	DWORD	pos;	/* file position for first byte in buffer */
	int	err;	/* error status (-1 = error) */
} FILEBUF;
typedef FILEBUF *PFILEBUF;
typedef FILEBUF FAR *LPFILEBUF;

void FileFDOpenRdr (LPFILEBUF ofd, CFile *pFile, LPTR buf, int bsiz);
void FileFDOpenWrt (LPFILEBUF ofd, CFile *pFile, LPTR buf, int bsiz);
int FileFDFlush (LPFILEBUF ofd);
long FileFDSeek (LPFILEBUF ofd, DWORD offset, int whence);
int FileFDWrite (LPFILEBUF ofd, LPTR buf, int len);
int FileFDRead (LPFILEBUF ifd, LPTR buf, int len);
void intelWriteWord (LPFILEBUF ofd, WORD w);
void intelWriteDWord (LPFILEBUF ofd, DWORD dw);
void intelReadWord (LPFILEBUF ifd, WORD FAR * w);
void intelReadDWord (LPFILEBUF ifd, DWORD FAR * dw);
void motWriteWord (LPFILEBUF ofd, WORD w);
void motWriteDWord (LPFILEBUF ofd, DWORD dw);
void motReadWord (LPFILEBUF ifd, WORD FAR * w);
void motReadDWord (LPFILEBUF ifd, DWORD FAR * dw);

#endif


