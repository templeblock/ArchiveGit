/***
*stdlib.h - declarations/definitions for commonly used library functions
****/

extern int __cdecl sscanf(const char *, const char *, ...);
extern int __cdecl isspace(int);
extern int __cdecl isupper(int);
extern int __cdecl islower(int);
extern int __cdecl isprint(int);
extern int __cdecl toupper(int);
extern int __cdecl tolower(int);
extern char * itoa( int, char *, int );
extern int atoi( char * );
extern long atol ( char *);

/***
*sys\types.h - types returned by system level calls for file and time info
****/

typedef unsigned short ino_t;	/* i-node number (not used on DOS) */
typedef long time_t;
typedef short dev_t;		/* device code */
typedef long off_t;		/* file offset value */
typedef unsigned int size_t;
typedef char *va_list;
typedef long fpos_t;
struct _iobuf {
	char *_ptr;
	int   _cnt;
	char *_base;
	char  _flag;
	char  _file;
	};
typedef struct _iobuf FILE;

/***
*sys\stat.h - define structure for returning status information
****/

struct stat {
	dev_t st_dev;
	ino_t st_ino;
	unsigned short st_mode;
	short st_nlink;
	short st_uid;
	short st_gid;
	dev_t st_rdev;
	off_t st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
	};

int _cdecl _stat(char *, struct stat *);

#define _S_IFMT 	0170000 	/* file type mask */
#define _S_IFDIR	0040000 	/* directory */
#define _S_IFCHR	0020000 	/* character special */
#define _S_IFREG	0100000 	/* regular */
#define _S_IREAD	0000400 	/* read permission, owner */
#define _S_IWRITE	0000200 	/* write permission, owner */
#define _S_IEXEC	0000100 	/* execute/search permission, owner */

/***
*string.h - declarations for string manipulation functions
****/

/* function prototypes */
void * _cdecl memccpy(void *, const void *, int, unsigned int);
void * _cdecl memchr(const void *, int, size_t);
void * _cdecl memcpy(void *, const void *, size_t);
void * _cdecl memmove(void *, const void *, size_t);
void * _cdecl memset(void *, int, size_t);
void _cdecl movedata(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
char * _cdecl strcat(char *, const char *);
char * _cdecl strchr(const char *, int);
char * _cdecl strrchr(const char *, int);
int _cdecl strcmp(const char *, const char *);
int _cdecl strcmpi(const char *, const char *);
char * _cdecl strcpy(char *, const char *);
size_t _cdecl strlen(const char *);
char * _cdecl strncat(char *, const char *, size_t);
int _cdecl strncmp(const char *, const char *, size_t);
int _cdecl strnicmp(const char *, const char *, size_t);
char * _cdecl strncpy(char *, const char *, size_t);

/***
*dos.h - definitions for MS-DOS interface routines
****/

/* _dos_findfirst structure */
struct _find_t {
	char reserved[21];
	char attrib;
	unsigned wr_time;
	unsigned wr_date;
	long size;
	char name[13];
	};

#define find_t _find_t

/* _dos_getdiskfree structure */
struct diskfree_t {
	unsigned total_clusters;
	unsigned avail_clusters;
	unsigned sectors_per_cluster;
	unsigned bytes_per_sector;
	};

/* File attribute constants */
#define _A_NORMAL	0x00	/* Normal file - No read/write restrictions */
#define _A_RDONLY	0x01	/* Read only file */
#define _A_HIDDEN	0x02	/* Hidden file */
#define _A_SYSTEM	0x04	/* System file */
#define _A_VOLID	0x08	/* Volume ID file */
#define _A_SUBDIR	0x10	/* Subdirectory */
#define _A_ARCH 	0x20	/* Archive file */

/* macros to break C "far" pointers into their segment and offset components */
#define FP_SEG(fp) (*((unsigned _far *)&(fp)+1))
#define FP_OFF(fp) (*((unsigned _far *)&(fp)))

/* function prototypes */
unsigned _cdecl _dos_findfirst(const char *, unsigned, struct find_t *);
unsigned _cdecl _dos_findnext(struct find_t *);
unsigned _cdecl _dos_getdiskfree(unsigned, struct diskfree_t *);
unsigned _cdecl _dos_setdrive(unsigned, unsigned *);
unsigned _cdecl _dos_getdrive(unsigned *);
unsigned _cdecl _dos_getfileattr(const char *, unsigned *);
unsigned __cdecl _dos_close(int);
unsigned __cdecl _dos_creatnew(const char *, unsigned, int *);
int _cdecl intdos(union REGS *, union REGS *);
int _cdecl intdosx(union REGS *, union REGS *, struct SREGS *);
void _cdecl segread(struct SREGS *);

int __cdecl _access(const char *, int);

/***
*io.h - declarations for low-level file handling and I/O functions
****/

/* function prototypes */
// I DON'T WANT ANYBODY TO USE THESE IN WINDOWS,
// SO I FUDGED THE ARG'S TO CAUSE ERRORS
int __cdecl chsize(int, long);
int	 _cdecl close(void);
int	 _cdecl creat(void);
int	 _cdecl eof(void);
long	 _cdecl lseek(void);
int	 _cdecl open(void);
int	 _cdecl read(void);
long	 _cdecl tell(void);
int	 _cdecl write(void);

/***
*stdio.h - definitions/declarations for standard I/O routines
****/

#define EOF	(-1)

/* fseek constants */
#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0

/* minimum guaranteed filename length, open file count, and unique
 * tmpnam filenames.
 */
#define FILENAME_MAX 63
#define FOPEN_MAX 20
#define SYS_OPEN 20
#define TMP_MAX 32767

/* standard file pointers */
#define stdin  (&_iob[0])
#define stdout (&_iob[1])

#define _IOREAD 	0x01
#define _IOWRT		0x02

#define _IOFBF		0x0
#define _IOLBF		0x40
#define _IONBF		0x04

#define _IOMYBUF	0x08
#define _IOEOF		0x10
#define _IOERR		0x20
#define _IOSTRG 	0x40
#define _IORW		0x80

/* function prototypes */
int _cdecl fclose(FILE *);
int _cdecl feof(FILE *);
int _cdecl ferror(FILE *);
int _cdecl fflush(FILE *);
int _cdecl fgetc(FILE *);
int _cdecl fgetchar(void);
int _cdecl fgetpos(FILE *, fpos_t *);
char * _cdecl fgets(char *, int, FILE *);
FILE * _cdecl fopen(const char *, const char *);
int _cdecl fprintf(FILE *, const char *, ...);
int _cdecl fputc(int, FILE *);
int _cdecl fputchar(int);
int _cdecl fputs(const char *, FILE *);
size_t _cdecl fread(void *, size_t, size_t, FILE *);
int _cdecl fscanf(FILE *, const char *, ...);
int _cdecl fsetpos(FILE *, const fpos_t *);
int _cdecl fseek(FILE *, long, int);
long _cdecl ftell(FILE *);
size_t _cdecl fwrite(const void *, size_t, size_t, FILE *);
int _cdecl getc(FILE *);
int _cdecl getchar(void);
char * _cdecl gets(char *);
int _cdecl getw(FILE *);
int _cdecl printf(const char *, ...);
int _cdecl putc(int, FILE *);
int _cdecl putchar(int);
int _cdecl puts(const char *);
int _cdecl putw(int, FILE *);
int _cdecl remove(const char *);
int _cdecl rename(const char *, const char *);
void _cdecl rewind(FILE *);
int _cdecl scanf(const char *, ...);
int _cdecl sprintf(char *, const char *, ...);
int _cdecl sscanf(const char *, const char *, ...);
int _cdecl unlink(const char *);
int _cdecl vfprintf(FILE *, const char *, va_list);
int _cdecl vprintf(const char *, va_list);
int _cdecl vsprintf(char *, const char *, va_list);
char * _cdecl mktemp(char *);
int _cdecl access(char *, int);
 
/* macro definitions */
#define feof(_stream)	  ((_stream)->_flag & _IOEOF)
#define ferror(_stream)   ((_stream)->_flag & _IOERR)
#define fileno(_stream)   ((int)(unsigned char)(_stream)->_file)
#define getc(_stream)	  (--(_stream)->_cnt >= 0 ? 0xff & *(_stream)->_ptr++ \
	: _filbuf(_stream))
#define putc(_c,_stream)  (--(_stream)->_cnt >= 0 \
	? 0xff & (*(_stream)->_ptr++ = (char)(_c)) :  _flsbuf((_c),(_stream)))
#define getchar()	  getc(stdin)
#define putchar(_c)	  putc((_c),stdout)

/* miscellaneous function prototypes */
int _cdecl rand(void);
long _cdecl  labs(long);
size_t _cdecl stackavail(void);

/***
*direct.h - function declarations for directory handling/creation
*
*	Copyright (c) 1985-1992, Microsoft Corporation.  All rights reserved.
*
*Purpose:
*	This include file contains the function declarations for the library
*	functions related to directory handling and creation.
*
****/

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

/* function prototypes */

int __cdecl _chdir(const char *);
int __cdecl _chdrive(int);
char * __cdecl _getcwd(char *, int);
char * __cdecl _getdcwd(int, char *, int);
int __cdecl _getdrive(void);
int __cdecl _mkdir(const char *);
int __cdecl _rmdir(const char *);

#ifndef __STDC__
/* Non-ANSI names for compatibility */
int __cdecl chdir(const char *);
char * __cdecl getcwd(char *, int);
int __cdecl mkdir(const char *);
int __cdecl rmdir(const char *);
#endif


