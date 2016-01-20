/*****************************************************************************

          Copyright (c) Micrografx, Inc., 1991.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
             from Micrografx, 1303 Arapaho, Richardson, TX. 75081.

******************************************************************************
******************************************************************************

                                 mgxutils.h

******************************************************************************
******************************************************************************

*****************************************************************************/

#ifndef _MGXUTILS_H_
#define _MGXUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32

#undef _far
#undef FAR
#undef NEAR
#undef huge
#undef HUGE
#undef _export
#undef __export
#undef _pascal

#define _far
#define FAR
#define NEAR
#define huge
#define HUGE
#define _export
#define __export
#define _pascal

#endif	// WIN32

/******************************** Includes **********************************/
/******************************** Constants *********************************/

#define MGXUTILS_VERSION_STRING "MGXUTILS 2.12 07/10/93"

#ifndef UTILAPI
    #define UTILAPI _far _pascal
#endif

/********************************** Types ***********************************/
#if WINVER < 0x030A
    typedef unsigned UINT;
#endif
#define SHORT   short
#define LONG    long

typedef BOOL            *PBOOL;
typedef BOOL near       *NPBOOL;
typedef BOOL far        *LPBOOL;
typedef BYTE near       *NPBYTE;
typedef WORD near       *NPWORD;
typedef DWORD near      *NPDWORD;
typedef SHORT           *PSHORT;
typedef SHORT near      *NPSHORT;
typedef SHORT far       *LPSHORT;
typedef LONG near       *NPLONG;
typedef void near       *NPANY;
typedef void far        *LPANY;
typedef void near       *NPVOID;

#ifndef WIN32
typedef WORD CHAR;         /* Double-byte character support type          */
#endif

typedef struct             /* BUFFER DATA STRUCTURE                       */
{
    LPBYTE   lpData;       /* handle to globally allocated buffer         */
    unsigned ItemSize;     /* size of individual elements in buffer       */
    unsigned MinItems;     /* minimum number of elements to add to buffer */
    unsigned nItems;       /* current number of elements in buffer        */
    unsigned BufferSize;   /* # of elements for which space is allocated  */
    WORD Flags;            /* memory allocation flags                     */
} BUFFER,FAR *LPBUFFER,NEAR *PBUFFER;

typedef const BUFFER FAR *LPCBUFFER;

/******************************** Public Data *******************************/

extern BOOL bDebug;             /* Enables/disables debug output          */
extern BOOL bKanjiSupport;      /* Enables/disables Kanji support         */

/****************************** Public Functions ****************************/

/****************************************************************************
 *  Library Functions
 ****************************************************************************/

void UTILAPI InitMGXUTILS (void);
int  UTILAPI MGXUTILS_Version (LPSTR lpVersion,int nSize);


/****************************************************************************
 *  Buffer Data Structure functions
 ****************************************************************************/

BOOL  UTILAPI Fit (LPBUFFER lpBuffer,unsigned Count);

DWORD UTILAPI FreeBuffer (LPBUFFER lpBuffer);

void  UTILAPI InitBuffer (LPBUFFER lpBuffer,
                          WORD ItemSize,
                          WORD MinItems,
                          WORD Flags);

BOOL  UTILAPI ShrinkBuffer (LPBUFFER lpBuffer);


/****************************************************************************
 *  Buffer Extended API functions
 ****************************************************************************/

LPVOID UTILAPI AccessBuffer     (LPCBUFFER lpBuffer,WORD wIndex);

BOOL   UTILAPI AddBufferItem    (LPBUFFER lpBuffer,const void FAR *lpItem);

BOOL   UTILAPI DeleteBufferItem (LPBUFFER lpBuffer,WORD wIndex);

BOOL   UTILAPI GetBufferItem    (LPCBUFFER lpBuffer,
                                 WORD wIndex,
                                 LPVOID   lpItem);


/****************************************************************************
 *  Byte-manipulation (raw data) functions
 ****************************************************************************/

int    UTILAPI CompareBytes  (const void FAR *,const void FAR *,WORD);
LPVOID UTILAPI CopyHugeBytes (LPVOID,LPVOID,DWORD);
HANDLE UTILAPI GlobalCopy    (HANDLE hMem,WORD wMemFlags);
LPVOID UTILAPI MoveBytes     (LPVOID,LPVOID,WORD);
LPVOID UTILAPI MoveHugeBytes (LPVOID,LPVOID,DWORD);
LPVOID UTILAPI SetBytes      (LPVOID,BYTE,WORD);
LPVOID UTILAPI SetHugeBytes  (LPVOID,BYTE,DWORD);


/****************************************************************************
 *  Conversion functions
 ****************************************************************************/

int   UTILAPI FixedToStr (long,LPSTR,int,int);
int   UTILAPI IntToStr (int,LPSTR,int);
int   UTILAPI LongToStr (long,LPSTR,int);
LPCSTR UTILAPI StrToFixed (LPCSTR,long FAR *,int);
LPCSTR UTILAPI StrToInt (LPCSTR,LPINT);


/****************************************************************************
 *  Debug functions
 ****************************************************************************/

/*
   This section declares several macros used in debugging applications.  The
   macros expand to expression evaluations and function calls if the constant
   DEBUG is defined at the point of invocation.  If DEBUG is not defined, the
   macros have no effect.  All of these macros result in no-ops when not
   running the debugging version of Windows.

       DEBUG_COORD (lpLabel,X,Y)
            displays a comma-separated coordinate pair preceded by the given
            label
                Example:  DEBUG_COORD ("Current Position:",X,Y);
                Output:   Current Position: 480,960

       DEBUG_INT (lpLabel,Value)
            displays the given value preceded by the specified label
                Example:  DEBUG_INT ("Buffer Size:",Count);
                Output:   BufferSize: 10

       DEBUG_STRING (lpLabel,lpStr)
            sends a message
                Example:  DEBUG_STRING ("WinMain:","Start Application");
                Output:   WinMain: Start Application

   The makefile for the application should define a debug parameter on the
   command line.  The value of the parameter should be appropriate for the
   desired debug mode.  For example, if a debugging version of the application
   is being created, the make command line would look like:

           MAKE debug="-D DEBUG" app

   For a non-debugging version, the command line would look like:

           MAKE debug="" app

   Inside the make file, invocations of the C compiler should reference the
   debug make parameter.  A typical C compiler invocation in the make file would
   look like:

           cl -c -AM -Gsw -Oas -Zpe $(debug) -NT _TEXT addwin.c

   Debugging information is only displayed if using the debugging version of
   Windows.
*/

#ifdef DEBUG
    #define Debug(Statement)        {Statement;}

    #define DEBUG_CODE(Statements) Statements
    #define DEBUG_COORD(lpStr,X,Y) DebugStr ("%s (%i,%i)",(LPSTR)(lpStr),X,Y)
    #define DEBUG_DWORD(lpStr,Value) DebugStr ("%s %lx",(LPSTR)(lpStr),Value)
    #define DEBUG_INT(lpStr,Value) DebugStr ("%s %i",(LPSTR)(lpStr),Value)
    #define DEBUG_LONG(lpStr,Value) DebugStr ("%s %li",(LPSTR)(lpStr),Value)
    #define DEBUG_MESSAGE(lpStr,Message,wParm,lParm) DebugMessage (lpStr,\
              Message,wParm,lParm)
    #define DEBUG_POINTS(lpStr,lpPoints,nPoints) DebugPoints (lpStr,\
              lpPoints,nPoints)
    #define DEBUG_RECT(lpStr,lpRect) DebugStr ("%s %R",(LPSTR)(lpStr), \
              (LPRECT)lpRect)
    #define DEBUG_STRING(lpLabel,lpStr) DebugStr ("%s %s",(LPSTR)(lpLabel), \
              (LPSTR)(lpStr))
    #define DEBUG_WORD(lpStr,Value) DebugStr ("%s %x",(LPSTR)(lpStr),Value)

#else
    #define Debug(Statement)

    #define DEBUG_CODE(Statements)
    #define DEBUG_COORD(lpStr,X,Y)
    #define DEBUG_DWORD(lpStr,Value)
    #define DEBUG_INT(lpStr,Value)
    #define DEBUG_LONG(lpStr,Value)
    #define DEBUG_MESSAGE(lpStr,Message,wParm,lParm)
    #define DEBUG_POINTS(lpStr,lpPoints,nPoints)
    #define DEBUG_RECT(lpStr,lpRect)
    #define DEBUG_STRING(lpLabel,lpStr)
    #define DEBUG_WINMSG(hWnd,wMsg,wParm,lParm)
    #define DEBUG_WORD(lpStr,Value)
#endif

void  UTILAPI Assert (LPCSTR,LPCSTR,LPCSTR,int);
void  UTILAPI DebugPoints (LPCSTR,const POINT FAR *,int);

#if defined (MGXUTILS_FLOAT) || defined (DEBUG_FLOAT)
    int FAR _cdecl DebugStr (LPCSTR,...);
#else
    #define DebugStr    iDebugStr
    int FAR _cdecl iDebugStr (LPCSTR,...);
#endif


/****************************************************************************
 *  Dialog functions
 ****************************************************************************/

BOOL UTILAPI EnablePopups (HWND hParent,BOOL bEnable);

BOOL UTILAPI GetDlgItemFixed (HWND hDlg,
                              int  Control,
                              LPLONG     lpResult,
                              int  Precision);

BOOL UTILAPI GetDlgInt (HWND hDlg,int Control,LPINT lpValue);

void UTILAPI SetDlgItemFixed (HWND hDlg,
                              int  Control,
                              long Value,
                              int  Precision);


/****************************************************************************
 *  Disk functions
 ****************************************************************************/
#define DRIVE_UNIQUE    (DRIVE_REMOVABLE+DRIVE_FIXED+DRIVE_REMOTE)
#ifndef WIN32
#define DRIVE_CDROM     (DRIVE_UNIQUE+1)
#define DRIVE_RAM       (DRIVE_UNIQUE+2)

UINT UTILAPI GetExtendedDriveType (int Drive);

#endif // ndef WIN32


/****************************************************************************
 *  Double-byte character functions  (See also String functions)
 ****************************************************************************/
#define UPPERCASE(C)            ((BYTE)AnsiUpper ((LPSTR)(DWORD)(BYTE)(C)))
#define LOWERCASE(C)            ((BYTE)AnsiLower ((LPSTR)(DWORD)(BYTE)(C)))
#define CHRNEXT(lpCur)          (AnsiNext (lpCur))
#define CHRPREV(lpBeg,lpCur)    (AnsiPrev (lpBeg,lpCur))
#define CHRSKIP(lpCur,Chr)      (lpCur += ((WORD)Chr > 0x00FF)+1)
#define CHRBACK(lpCur,Chr)      (lpCur -= ((WORD)Chr > 0x00FF)+1)
#define CHRSIZE(wChar)          (((WORD)wChar & 0xFF00) ? 2 : 1)

LPSTR UTILAPI ChrAt      (LPSTR,int);
CHAR  UTILAPI GetChr     (LPCSTR);
CHAR  UTILAPI GetLastChr (LPCSTR);
int   UTILAPI lchrlen    (LPCSTR);
int   UTILAPI lchrnlen   (LPCSTR,int);
LPSTR UTILAPI SetChr     (LPSTR,CHAR);


/****************************************************************************
 *  Filename functions
 ****************************************************************************/
#ifndef WIN32 
#define MAX_PATH        80              /* Maximum path/filename byte sizes */
#endif	// WIN32

#define MAX_DRIVE       3               /*  (Sizes include zero-terminator) */
#define MAX_DIRECTORY   66
#define MAX_FILENAME    9
#define MAX_EXTENSION   5

#define PS_DRIVE        0x01            /* PathSplit return flags           */
#define PS_DIRECTORY    0x02
#define PS_FILENAME     0x04
#define PS_EXTENSION    0x08
#define PS_WILDCARD     0x10

int  UTILAPI Exists (LPCSTR lpFilename);
BOOL UTILAPI IsDirectory (LPCSTR);
BOOL UTILAPI IsWildcard  (LPCSTR);

void UTILAPI PathCat   (LPSTR lpResult,LPCSTR lpPrefix,LPCSTR lpSuffix);
void UTILAPI PathMerge (LPSTR lpPath,
                        LPCSTR lpDrive,
                        LPCSTR lpDir,
                        LPCSTR lpFile,
                        LPCSTR lpExt);

int  UTILAPI PathSplit (LPCSTR lpPath,
                        LPSTR lpDrive,
                        LPSTR lpDir,
                        LPSTR lpFile,
                        LPSTR lpExt);


/****************************************************************************
 *  Formatted output functions
 ****************************************************************************/

#ifdef MGXUTILS_FLOAT
    int FAR _cdecl FormatStr (LPSTR,int,LPCSTR,...);
    int UTILAPI   vFormatStr (LPSTR,int,LPCSTR,LPVOID FAR *);
#else
    #define FormatStr   iFormatStr
    #define vFormatStr viFormatStr
    int FAR _cdecl iFormatStr (LPSTR,int,LPCSTR,...);
    int UTILAPI   viFormatStr (LPSTR,int,LPCSTR,LPVOID FAR *);
#endif

char  UTILAPI GetDecimalSeparator (void);
char  UTILAPI GetThousandSeparator (void);

LPSTR UTILAPI ParmString (LPSTR lpResult,
                          LPCSTR lpString,
                          LPCSTR lpParm,
                          unsigned MaxSize);


/****************************************************************************
 *  Search Functions
 ****************************************************************************/

LPVOID UTILAPI FindByte  (LPVOID lpArray,
                          DWORD  dwArraySize,
                          BYTE   cTarget);
LPVOID UTILAPI FindBytes (LPVOID lpArray,
                          DWORD  dwArraySize,
                          LPVOID lpTarget,
                          WORD   wTargetLength);
LPVOID UTILAPI FindWord  (LPVOID lpArray,
                          DWORD  dwArraySize,
                          WORD   wTarget);

/****************************************************************************
 *  String functions (See also DBCS functions)
 *
 *  All of these functions have double-byte char. support.  The "nChars"
 *  parameter to functions like lstrncat() refers to the # of characters NOT
 *  necessarily the # of bytes.
 ****************************************************************************/

LPSTR UTILAPI fstrncat  (LPSTR lpDst,LPCSTR lpSrc,int nBytes);
LPSTR UTILAPI fstrncpy  (LPSTR lpDst,LPCSTR lpSrc,int nBytes);
LPSTR UTILAPI lchrchr   (LPSTR,CHAR);   // for backwards compatibility only!
LPSTR UTILAPI lstrchr   (LPSTR,CHAR);
LPSTR UTILAPI lstrncat  (LPSTR lpDst,LPCSTR lpSrc,int nChars);
int   UTILAPI lstrncmpx (LPCSTR lpStr1,
                         LPCSTR lpStr2,
                         int   nChars,
                         BOOL  bCase);
LPSTR UTILAPI lstrncpy  (LPSTR lpDst,LPCSTR lpSrc,int nChars);
LPSTR UTILAPI lstrrchr  (LPCSTR,CHAR);
LPSTR UTILAPI lstrrev   (LPSTR);
LPSTR UTILAPI lstrstrx  (LPSTR lpString,
                         LPCSTR lpSub,
                         BOOL  bCase);
int   UTILAPI lstrtrm   (LPSTR lpString,CHAR Front,CHAR Back);

#define lstrncmp(s1,s2,n)   lstrncmpx (s1,s2,n,TRUE)
#define lstrncmpi(s1,s2,n)  lstrncmpx (s1,s2,n,FALSE)
#define lstrstr(Str,Sub)    lstrstrx (Str,Sub,TRUE)
#define lstrstri(Str,Sub)   lstrstrx (Str,Sub,FALSE)

/****************************************************************************
 *  Windows functions
 ****************************************************************************/

#define AW_CENTER       0x0000          /* AlignWindow options              */
#define AW_LEFT         0x0001
#define AW_RIGHT        0x0002
#define AW_MIDDLE       0x0000
#define AW_TOP          0x0010
#define AW_BOTTOM       0x0020
#define AW_BYWINDOW     0x0000
#define AW_BYCLIENT     0x0100
#define AW_FRAMEOFFSET  0x0800
#define AW_NOOBSCURE    0x8000

void     UTILAPI AlignWindow (HWND hWnd,
                              WORD fAlignment,
                              HWND hRefWnd);
HANDLE   UTILAPI BringAppToTop (LPCSTR lpModule);
HWND     UTILAPI FindAppWindow (LPCSTR lpModule);
HANDLE   UTILAPI LoadApp (LPCSTR lpAppName,
                          LPCSTR lpCommandLine,
                          WORD  ShowCommand);
BOOL     UTILAPI WriteProfileInt (LPCSTR lpAppName,
                                  LPCSTR lpKey,
                                  int   Value);

/****************************************************************************
 *  Miscellaneous Functions
 ****************************************************************************/
#define MW_ESCAPES      0x0001          /* MatchWildcard options            */
#define MW_CASE         0x0002

BOOL UTILAPI MatchWildcard (LPSTR,LPCSTR,WORD);

/****************************************************************************/
#ifdef __cplusplus
}
#endif

#endif  /* Header file already included */

/****************************************************************************/
