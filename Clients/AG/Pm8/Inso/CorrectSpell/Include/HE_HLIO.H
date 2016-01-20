/*
Helyes-e?
Hungarian Spelling Checker and Hyphenation Program
(C) Copyright 1991-93, MorphoLogic. All	rights reserved.

This work is protected by the Hungarian	and International Copyright Laws.
Solely for use as permitted by the written license from	MorphoLogic.
Disclosure of contents or parts	of this	material are prohibited.

Module:		he_hlio.h
Version:	5.05
Purpose:	header file for	he_hlio.c
Created	by:	Miklos Pal
Creation date:	10-25-1993
Changes:	<name>	<date>	<purpose>
*/

#ifdef HE_ANSI_IO	/* { */

#include <stdio.h>

#ifdef __GNUC__	/* { */
#include <unistd.h>
#endif	/* } */

typedef	FILE *FILE_ID;
#define	FILE_RO		"rb"	/* read	only, binary mode */
#define	FILE_WO		"wb"	/* write only, binary mode */
#define	FILE_RW		"r+b"	/* read-write, binary mode */
#define	FILE_AP		"a+b"	/* append, binary mode */
#define	FILE_WO_CR	"wb"	/* write only, create, binary mode */
#define	FILE_RW_CR	"r+b"	/* read_write, create, binary mode */
#define	FILE_AP_CR	"a+b"	/* append, create, binary mode */
#define	FILE_RO_TM	"rt"	/* read	only, text mode	*/
#define	FILE_WO_TM	"wt"	/* write only, text mode */
#define	FILE_RW_TM	"r+t"	/* read_write, text mode */
#define	FILE_AP_TM	"a+t"	/* append, text	mode */
#define	FILE_WO_CR_TM	"wt"	/* write only, create, text_mode */
#define	FILE_RW_CR_TM	"r+t"	/* read_write, create, text_mode */
#define	FILE_AP_CR_TM	"a+t"	/* append, create, text_mode */
#define	FILE_OK		0	/* ok */
#define	FILE_EOF	(-1)	/* eof */
#define	FILE_NULL	NULL	/* can not be opened */
#define	FILE_SEEK_ERROR	(-1L)	/* update (write) error	*/
#define	FILE_SEEK_SET	SEEK_SET
#define	FILE_SEEK_CUR	SEEK_CUR
#define	FILE_SEEK_END	SEEK_END

#define	f_open(pn,m)		fopen(pn,m)
#define	f_close(id)		fclose(id)
#define	f_update(id)		fflush(id)
#define	f_seek(id,n,o)		fseek(id,n,o)
#define	f_tell(id)		ftell(id)
#define	f_getc(id)		fgetc(id)
#define	f_putc(c,id)		fputc(c,id)
#define	f_getc_txt(id)		fgetc(id)
#define	f_putc_txt(c,id)	fputc(c,id)
#define	f_getc_bin(id)		fgetc(id)
#define	f_putc_bin(c,id)	fputc(c,id)
#define	f_gets(p,s,id)		fgets(p,s,id)
#define	f_puts(p,id)		fputs(p,id)
#define	f_read(p,s,n,id)	fread(p,s,n,id)
#define	f_write(p,s,n,id)	fwrite(p,s,n,id)
#define	f_eof(id)		feof(id)
#define	f_error(id)		ferror(id)

#else	/* }{ */

typedef	struct filebuff_tag
	{
	memhandle_t hself;	/* handle for the object itself	*/
	int mode;		/* open	mode FILE_ constants */
	int currop;		/* current operation, FILE_READ, */
				/* FILE_WRITE or 0 */
	int eof;		/* status tu be	returned by file_eof */
	int error;		/* status tu be	returned by file_error */
	long buffsize;		/* size	of memory buffer */
	memhandle_t hmembuff;	/* handle of memory buffer */
	char FAR_PTR *pmembuff;	/* pointer to the memory buffer	*/
	LL_HANDLE hfile;	/* handle of file, it must be */
				/* opened in binary mode */
	long fileptr;		/* virtual file	pointer	*/
	long buffref;		/* buffer referenced to	file position */
	long buffuse;		/* number of valid characters in buffer	*/
	/*int (FAR_PTR *p_getc)(struct filebuff_tag FAR_PTR *);*/
	/*int (FAR_PTR *p_putc)(int, struct filebuff_tag FAR_PTR *);*/
	} FILEBUFF;

typedef	FILEBUFF FAR_PTR *FILE_ID;

#define	FILE_CR		8	/* used	internally */
#define	FILE_TM		16	/* used	internally */
#define	FILE_RO		1	/* read	only, binary mode */
#define	FILE_WO		2	/* write only, binary mode */
#define	FILE_RW		3	/* read-write, binary mode */
#define	FILE_AP		4	/* append, binary mode */
#define	FILE_WO_CR	2+8	/* write only, create, binary mode */
#define	FILE_RW_CR	3+8	/* read_write, create, binary mode */
#define	FILE_AP_CR	4+8	/* append, create, binary mode */
#define	FILE_RO_TM	1+16	/* read	only, text mode	*/
#define	FILE_WO_TM	2+16	/* write only, text mode */
#define	FILE_RW_TM	3+16	/* read_write, text mode */
#define	FILE_AP_TM	4+16	/* append, text	mode */
#define	FILE_WO_CR_TM	2+8+16	/* write only, create, text_mode */
#define	FILE_RW_CR_TM	3+8+16	/* read_write, create, text_mode */
#define	FILE_AP_CR_TM	4+8+16	/* append, create, text_mode */
#define	FILE_OK		0	/* ok */
#define	FILE_EOF	(-1)	/* eof */
#define	FILE_NULL	FAR_NULL/* can not be opened */
#define	FILE_SEEK_ERROR	(-1L)	/* update (write) error	*/
#define	FILE_SEEK_SET	LL_SEEK_SET
#define	FILE_SEEK_CUR	LL_SEEK_CUR
#define	FILE_SEEK_END	LL_SEEK_END

#ifndef	HE_ANSI_IO	/* { */

int f_getc_bin(FILEBUFF FAR_PTR *pfb);
int f_putc_bin(int cc, FILEBUFF FAR_PTR *pfb);
int f_getc_txt(FILEBUFF FAR_PTR *pfb);
int f_putc_txt(int cc, FILEBUFF FAR_PTR *pfb);

#endif	/* } HE_ANSI_IO	*/

#define	f_getc(pfb)		((pfb->mode&FILE_TM)?(f_getc_txt(pfb)):(f_getc_bin(pfb)))
#define	f_putc(cc,pfb)		((pfb->mode&FILE_TM)?(f_putc_txt(cc,pfb)):(f_putc_bin(cc,pfb)))

#define	f_getc_txt(pfb)		(f_getc_txt(pfb))
#define	f_putc_txt(cc,pfb)	(f_putc_txt(cc,pfb))

#define	f_getc_bin(pfb)		(f_getc_bin(pfb))
#define	f_putc_bin(cc,pfb)	(f_putc_bin(cc,pfb))

#define	f_tell(pfb)	(pfb->fileptr)
#define	f_eof(pfb)	(pfb->eof)
#define	f_error(pfb)	(pfb->error)

FILE_ID	f_open(char FAR_PTR *pathname, int mode);
int f_close(FILE_ID handle);
int f_update(FILEBUFF FAR_PTR *pfb);
long f_seek(FILE_ID pfb, long offset, int origin);
char FAR_PTR *f_gets(char FAR_PTR *buff, int buffsize, FILE_ID pfb);
int f_puts(char	FAR_PTR	*buff, FILE_ID pfb);
int f_read(char	FAR_PTR	*p, size_t s, size_t n,	FILE_ID	pfb);
int f_write(char FAR_PTR *p, size_t s, size_t n, FILE_ID pfb);

#endif /* } */

/* end */
