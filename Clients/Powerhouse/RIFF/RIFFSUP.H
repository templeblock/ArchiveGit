/** RIFFsup.h
 *
     (C) Copyright Microsoft Corp. 1991, 1992.  All rights reserved.

     You have a royalty-free right to use, modify, reproduce and 
     distribute the Sample Files (and/or any modified version) in 
     any way you find useful, provided that you agree that 
     Microsoft has no warranty obligations or liability for any 
     Sample Application Files which are modified. 
	 
     If you did not get this from Microsoft Sources, then it may not be the
     most current version.  This sample code in particular will be updated
     and include more documentation.  

     Sources are:
     	The MM Sys File Transfer BBS: The phone number is 206 936-4082.
	CompuServe: WINSDK forum, MDK section.
	Anonymous FTP from ftp.uu.net vendors\microsoft\multimedia
 *
 ** */

#ifndef _RIFFSUP_H_
#define _RIFFSUP_H_

//
//
//

#define IDD_INFOLIST		100
#define IDD_INFOINFO		101
#define	IDD_INFOTEXT		102

#ifdef RC_INVOKED

#define DLG_INFOEDIT		31

#else
			
#define DLG_INFOEDIT		MAKEINTRESOURCE(31)

#endif

////////////////////////////////////////////////////////////////////////////

typedef struct tCHUNK
{
    FOURCC  fcc;
    DWORD   cksize;
    BYTE    data[];
} CHUNK, * PCHUNK, far * LPCHUNK;



typedef struct tDISP
{
    DWORD   cfid;   // Clipboard id of data
    HANDLE  h;	    // handle to data
    struct tDISP *  next;    // next in list
} DISP;

typedef struct tINFODATA
{
    WORD    index;  // index into aINFO
    WORD    wFlags; // flags for chunk
    DWORD   dwINFOOffset;   // offset in file to INFO chunk
    
#define INFOCHUNK_MODIFIED  1
#define INFOCHUNK_REVERT    2	// command to revert to original text

    LPSTR   lpText; // text of modified chunk.  None if NULL.

    struct tINFODATA  near *  pnext; // next read sub-chunk
} INFODATA, * PINFODATA, FAR * LPINFODATA;

typedef struct tINFOCHUNK
{
    LPSTR   lpChunk;	// complete chunk in memory (GlobalPtr)
    DWORD   cksize;	// size of chunk data
    PINFODATA	pHead;	// first sub-chunk data
} INFOCHUNK, * PINFOCHUNK, FAR * LPINFOCHUNK;

////////////////////////////////////////////////////////////////////////////
//
//  error returns from RIFF functions
//
#define RIFFERR_BASE         (0)
#define RIFFERR_NOERROR      (0)
#define RIFFERR_ERROR        (RIFFERR_BASE+1)
#define RIFFERR_BADPARAM     (RIFFERR_BASE+2)
#define RIFFERR_FILEERROR    (RIFFERR_BASE+3)
#define RIFFERR_NOMEM        (RIFFERR_BASE+4)
#define RIFFERR_BADFILE	     (RIFFERR_BASE+5)

////////////////////////////////////////////////////////////////////////////
//
//  public function prototypes
//

#define RIFFAPI  FAR PASCAL


BOOL RIFFAPI riffCopyList(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck);
BOOL RIFFAPI riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck);

LRESULT RIFFAPI  riffInitINFO(INFOCHUNK FAR * FAR * lplpInfo);
LRESULT RIFFAPI  riffReadINFO(HMMIO hmmio, const LPMMCKINFO lpck, LPINFOCHUNK lpInfo);
LRESULT RIFFAPI  riffEditINFO(HWND hwnd, LPINFOCHUNK lpInfo, HINSTANCE hInst);
LRESULT RIFFAPI  riffFreeINFO(INFOCHUNK FAR * FAR * lpnpInfo);
LRESULT RIFFAPI riffWriteINFO(HMMIO hmmioDst, LPINFOCHUNK lpInfo);


LRESULT RIFFAPI  riffReadDISP(HMMIO hmmio, LPMMCKINFO lpck, DISP FAR * FAR * lpnpDisp);
LRESULT RIFFAPI  riffFreeDISP(DISP FAR * FAR * lpnpDisp);
LRESULT RIFFAPI riffWriteDISP(HMMIO hmmio, DISP FAR * FAR * lpnpDisp);


#endif

/** EOF: RIFFsup.h **/
