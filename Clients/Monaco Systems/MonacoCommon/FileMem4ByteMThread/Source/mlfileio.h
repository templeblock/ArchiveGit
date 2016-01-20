#ifndef ML_FILEIO_H
#define ML_FILEIO_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct _FILEIO FILEIO;

typedef void   *(*FILEIO_CLOSE)      (FILEIO *pBlock); //Closes file and frees(FILEIO)
typedef long    (*FILEIO_READ)       (void *fh, void *buf, long num_bytes); //Same as _read in C Runtime Library
typedef long    (*FILEIO_WRITE)      (void *fh, void *buf, long num_bytes); //Same as _write in CRT Lib
typedef long    (*FILEIO_LSEEK)      (void *fh, long offset, long origin);	//Same as _lseek in CRT Lib
typedef long    (*FILEIO_TELL)       (void *fh);	//Same as _tell in CRT Lib
typedef long    (*FILEIO_EOF)        (void *fh);	//Same as _eof in CRT Lib
typedef long    (*FILEIO_FILELENGTH) (void *fh); 	//Same as _filelength in CRT Lib

struct _FILEIO {
	FILEIO_CLOSE      close;
	FILEIO_READ       read;
	FILEIO_WRITE      write;
	FILEIO_LSEEK      lseek;
	FILEIO_TELL       tell;
	FILEIO_EOF        eof;
	FILEIO_FILELENGTH filelength;

	//Private Data
	void *fh;
};

/*************************************************************
 * Define Macros to simplify File I/O access
 *************************************************************/
#define FileClose(fileio)	((fileio)->close(fileio))
#define FileRead(fileio, buf, num)	((fileio)->read((fileio)->fh,buf, num))
#define FileWrite(fileio, buf, num)	((fileio)->write((fileio)->fh, buf, num))
#define FileSeek(fileio,offset, origin) ((fileio)-seek((fileio)->fh, buf, num))
#define FileTell(fileio)	((fileio)->tell((fileio)->fh))
#define FileEof(fileio)	((fileio)->eof((fileio)->fh))
#define FileFileLength(fileio)	((fileio)->filelength((fileio)->fh))

#endif