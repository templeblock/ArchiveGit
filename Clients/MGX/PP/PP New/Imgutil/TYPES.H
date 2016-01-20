#ifndef __types
#define __types

#ifdef _NTWIN
#ifndef WIN32
#define WIN32
#endif
#endif

#ifndef RC_INVOKED
// Since we use -W# warnings, lets turn off any annoying ones
//#ifndef _PPC_
#pragma warning( disable : 4135 4018 )
//#endif
//	4101 unreferenced local variable
//	4135 conversion between different integral types
//	4018 signed/unsigned mismatch
#endif

#ifdef WIN32
#ifdef NO_MFC
#include <windows.h>
#include <tchar.h>
#else
#include <afxwin.h>
#include <afxcmn.h>
#endif
#include <limits.h>
#else
#include <ole2.h>
#include <limits.h>
#endif

// Miscellaneous definitions
#ifndef WIN32
#define	MAX_PATH 80
#endif
#define	MAX_FNAME_LEN	MAX_PATH
#define	MAX_TNAME_LEN	256

#define	MAX_STR_LEN	80
#define	MAX_EXTNAME_LEN	32
#define	MAX_IMAGE_LINE	16384

//**********************************************************************
// Type definitions
//**********************************************************************
#ifdef WIN32
#define itoa	    _itoa
#define ltoa	    _ltoa
#ifndef NEAR
#define NEAR
#endif
#ifndef FAR
#define FAR
#endif

#ifdef HUGE
#undef HUGE
#endif
#define HUGE

#ifndef _huge
#define _huge
#endif
#else
#define HUGE _huge
typedef unsigned long ULONG;
//typedef LPCSTR LPCTSTR; 				// unicode typedef - not in win16 sdk
//typedef LPSTR LPTSTR; 			 	// unicode typedef - not in win16 sdk
typedef BOOL FAR *LPBOOL; 				// in win32 and not in win16 sdk
typedef int INT; 						// in win32 and not in win16 sdk
#define APIENTRY    int PASCAL 			// in win32 and not in win16 sdk 
typedef MINMAXINFO FAR *LPMINMAXINFO; 	// in win32 and not in win16 sdk
#endif

#ifndef INT8
typedef char INT8;
typedef INT8 FAR *LPINT8;
typedef INT8 NEAR *PINT8;
#endif
#ifndef UINT8
typedef unsigned char UINT8;
typedef UINT8 FAR *LPUINT8;
typedef UINT8 NEAR *PUINT8;
#endif
#ifndef INT16
typedef short INT16;
typedef INT16 FAR *LPINT16;
typedef INT16 NEAR *PINT16;
#endif
#ifndef UINT16
typedef unsigned short UINT16;
typedef UINT16 FAR *LPUINT16;
typedef UINT16 NEAR *PUINT16;
#endif
#ifndef INT32
typedef long INT32;
typedef INT32 FAR *LPINT32;
typedef INT32 NEAR *PINT32;
#endif
#ifndef UINT32
typedef unsigned long UINT32;
typedef UINT32 FAR *LPUINT32;
typedef UINT32 NEAR *PUINT32;
#endif

typedef char * P_STR;
typedef unsigned char *PTR;

#ifndef LPTR_DEFINED
#define LPTR_DEFINED
#undef LPTR
typedef unsigned char FAR *LPTR;
typedef LPTR FAR *LPPTR;
#endif

typedef unsigned char HUGE *HPTR;
typedef DWORD HUGE *HPDWORD;
typedef WORD HUGE *HPWORD;
typedef LPPOINT FAR *LPPPOINT;
typedef LPSTR FAR *LPPSTR;
typedef LPTSTR FAR *LPPTSTR;
typedef LPVOID FAR *LPPVOID;
typedef void HUGE *HPVOID;
typedef LPINT FAR *LPPINT;
typedef LPPINT FAR *LPPPINT;
typedef LPWORD FAR *LPPWORD;
typedef LPPWORD FAR *LPPPWORD;
typedef TCHAR FNAME[MAX_FNAME_LEN];
typedef TCHAR TNAME[MAX_TNAME_LEN];
typedef TCHAR EXTNAME[MAX_EXTNAME_LEN];
typedef TCHAR STRING[MAX_STR_LEN];
typedef TCHAR SZNUM[16];
typedef int ITEMID;
typedef ITEMID FAR *LPITEMID;
typedef unsigned long PFIXED;
typedef unsigned long COLOR;
typedef COLOR FAR *LPCOLOR;
typedef int (FAR *LPROC)();
typedef void (FAR *LVPROC)();
typedef long (FAR *LPFL)();
typedef unsigned char FAR * (FAR *LLPTRPROC)();

///
/// C++ 
///

//**********************************************************************
// Constant definitions
//**********************************************************************
#define WINPROC		WINAPI

#ifdef WIN32
#define EXPORT
#else
#ifndef EXPORT
#define EXPORT		__export
#endif
#endif

#define LOCAL		static
#define REG			register
#define YES			1
#define NO			0
#define MAYBE		-1
#define ON			1
#define OFF			0

typedef enum
	{
	SC_SUCCESS,		
	SC_MEMERROR,		
	SC_CANCELLED,
	SC_FILENOTFOUND,
	SC_READERROR,
	SC_UNDOERROR,
	SC_BADPARMS,
	SC_WRITEERROR
	} STATUS_CODE;

#endif

