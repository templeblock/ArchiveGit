// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifndef __bsrtypes
#define __bsrtypes

#include "types.h"
#include "framelib.h"  
#include "browser.h"

#ifdef WIN32
#define MAX_FENTRY_LEN (MAX_PATH + 19) // FNAME + enough space for File list extra info
#else
#define MAX_FENTRY_LEN 99 // FNAME + enough space for File list extra info
#endif

#define DBFILE_EXT	_T(".DBF")	// the directory dbf extension
#define ALBUM_EXT 	_T(".ALB")	// the album dbf extension

#define PP4DBFTYPE 	0x0001		// the dbf flag for pp 40 type (packed, short paths)
#define NTBDBFTYPE 	0x0002		// the dbf flag forNT Beta (Not packed)
#define DBFTYPE 	0x0004		// the dbf flag for pp 50 (packed, long paths)

#define THMINFO_40		0x0004	// the type flag for 40 ship (packed) 
#define THMINFO_NT_BETA	0x0001	// the type flag for nt beta (non packed) // NOT COMPATABLE
#define THMINFO_50	0x0002		// the type flag for pp50 (non packed)

#define THUMBNAIL_EXT _T(".THM")	// the thumbnail extension

// for DBFINFO.moveType
#define DBFM_LOCAL 0		// all new thumbs stay local.
#define DBFM_REMOTE 1		// all new thumbs go on save drive as original file.
#define DBFM_REMOTE_S 2		// new thumbs in this or any subdir go remote.

#define DEF_THM_X 	64
#define DEF_THM_Y	64
 
// DBF types
#define ID_DIRECTORY	0x0001 	
#define ID_ALBUM		0x0002

typedef TCHAR FENTRY[MAX_FENTRY_LEN];
typedef FENTRY FAR *LPFENTRY;

#define NUM_DEPTHS 4					  
typedef struct _rdrdef {
	TCHAR	szDLL[26];
	BOOL	bTypes[7];			// IDC_SAVECT, IDC_SAVELA, IDC_SAVESP,
								// IDC_SAVE4BITCOLOR, IDC_SAVE8BITCOLOR, 
								// IDC_SAVE24BITCOLOR, IDC_SAVE32BITCOLOR
	BOOL	bOpen;				// can we open these files
	BOOL 	bCanSave;  			// can we save these files
	BOOL	bSave[NUM_DEPTHS];  // Save Depths supported
	BOOL	bCompressed;		// Optional compression?
	} RDRDEF;

//	contains all the info needed to identify a file
typedef struct _filelocation {
	FNAME		szFile;		
	VOLUMEID	volume;
} FILELOC;
typedef FILELOC FAR *LPFILELOC;

// 	dbf header
#define PP4DBFINFOSIZE	(5*2+2*80+2*4)	// packed size for 4.0 dbf
#define DBFINFOSIZE	(5*2+2*sizeof(FNAME)+2*4)	// packed size
typedef struct _dbfinfo {
	WORD	fDBFType;		// 	version of dbf (for future changes)
	FNAME	szName;			//	path name or album name
	VOLUMEID volume;		// 	the volume containg the directory (dirctory dbf only)
	WORD	idType;			// 	album or path
	WORD	moveType;		// 	Where do new thumbs added go?
	WORD 	lData;			// 	offset from begining of file of first entry
	WORD	numEntry;		// 	number of entries in this dbf
	FNAME 	szDescription;	//	album description field
	} DBFINFO;
typedef DBFINFO FAR *LPDBFINFO;

// 	dbf structure
typedef struct _dbfs {
	DBFINFO		info;  
	FNAME		szFile;		// file name and path of dbf
	BOOL		bOpen;		// is the file open??
	BOOL		bWrite;		// is it  writable (if its open)?
	FILE_HANDLE file;		// file handle
	int 		pos;		// current entry (which the file is open to)
	int 		EntrySize;	// The size of the entries on disk.
	int 		PathSize;	// The size of the file names and paths on disk.
	} DBFS;
typedef DBFS FAR *LPDBF;


// dbf entry
#define DBFENTRYSIZE (2*sizeof(FNAME)+16)	// packed size
typedef struct _entry {
	FILELOC	File;			// original file full path.
	FILELOC	Thumb;			// thumbnail full path.
	} ENTRY;
typedef	ENTRY FAR *LPENTRY;

// dbf doubly linked list structure
typedef struct _dbfll {
	DBFS					dbf;
	struct _dbfll	FAR * 	previous;
	struct _dbfll	FAR * 	next;
}	DBFLL;
typedef DBFLL FAR *LPDBFLL;

// file description
typedef FNAME DESC;
typedef DESC FAR *LPDESC; 

// thumbnail header info
#define THMINFOSIZE  70 // packed size (15*sizeof(LONG)+5*sizeof(WORD))
		
#define THM_NOCOMPRESS	0;
#define THM_RLE1	 	1;

typedef struct _thumbinfo {
	LONG 		fileXSize;	// size of OriginalFile(OF) (note: actually stored later in the file)
	LONG 		fileYSize;	// size of OF
	LONG 		fileSize;	// size of OF in bytes
	LONG		fileRes;	// resolution of OF 
	LONG		fileDepth;	// data depth of OF
	LONG		dataType;	// data type of OF (based at 0, same order as IDC_dtype)
	LONG 		fileType;	// file type of OF (based at 0, same order as IDN_ftype)
	ULONG		ctime;		// time of file creation
	ULONG 		time;		// time of file when last checked
	ULONG		thmtime;	// time on OF when thumbnail created		
	WORD 		thmXSize;	// size of thumbnail data
	WORD 		thmYSize;	// size of thumbnail data
	WORD 		thmDepth; 	// depth of thumbnail data (also can be type flag for compression)
	LONG		lFree;		// position of first available space after info
	LONG		lThumb;		// position from begining of file of thumbnail image data
	LONG 		lDescr;		// position of description data
	WORD		descrLen;	// length of description
	LONG 		lFlags;		// thumb type flags (note: it is stored at begining of file)
	WORD 		wCompress;	//	The compression type of thumb on disk
	LONG		thmSize;	//	the size in bythes ofthumb info on disk
	} THMINFO;
typedef THMINFO FAR *LPTHMINFO;

typedef struct _thumbnail {
	FILELOC		Thumb;		// name and volume of thumb file
	THMINFO		info;		// thumbnail info
	LPFRAME		frame;		// frame data for thumbnail (may be NULL)
	BOOL 		bDescr;		// have we loaded the description?
	DESC		szDescr;	// thumbnail description (may be "")
	BOOL		bSpecial;	// Special thumbnail (ie PCD or 3.1) Read olny
	} THMNAIL;
typedef THMNAIL FAR *LPTHMNAIL;

typedef struct _cthumbnail {
	THMNAIL thm;						// actual data
	int iOut;							// num of times currently checked out
	struct _cthumbnail FAR *next;		// only used by CTHM functions
	struct _cthumbnail FAR *previous;	// only used by CTHM functions
	} CTHMNAIL;
typedef CTHMNAIL FAR *LPCTHMNAIL;


// The master object which contains all 'global' & 'static' data			 
typedef struct _bsrstate {
	// globals 
	OPENSTATE OpenState;		// structure used for external communication
	LPOPENSTATE lpOpenState; 	// the original structure passed into the dialog
	BOOL bFileNameChange; 		// set to true during setting of file name text
	FNAME szLastDirDBF;			// the name of the last directory dbf used
								// if [0] = -1 then there is none.
								// if [0] = 0 then must check if there is one.
	FILELOC LastPath;			// the path for lpBsr->szLastDirDBF filled in thmbsr.c
	BOOL bDisableCancel;
	// From Browser & ExtBrowser
	// For oldthm.c
	FNAME szOldDBDir;
	FNAME OldThumbnailDir;
	// For bsrutil.c
	LPINT SelBuf;	// for the file list...MUST BE SET UP
	int max_sel_items;			// size of SelBuf
	int NumSelBufItems;			// for the file list
	int CurrentSelBufItem;		// for the file list
	HWND hFileCombo;			// for the file list
	LPDBFLL lpAlbList; 			// current album in album double linked list
	UINT FAR *DriveTypes;		// cached drive types
	LPVOLUMEID DriveVolumes;	// cached drive volumes
	TCHAR sDate[4];				// for international date& time
	TCHAR sTime[4];				// for international date& time
	TCHAR sAMPM[4][10];			// for international date& time
	int iDate;					// for international date& time
	int iTime;					// for international date& time
} BSRSTATE, FAR *LPBSRSTATE;

#endif

