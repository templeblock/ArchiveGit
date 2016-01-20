/*
// MISC.H
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// General include file for all PMW files.
//
// $Header: /PM8/REMIND32/MISC.H 1     3/03/99 6:26p Gbeddow $
// $Log: /PM8/REMIND32/MISC.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:54   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:44:48   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 09:54:42   Jay
// Changes in 2.1
// 
//    Rev 2.6   27 Feb 1996 16:42:14   JAY
// Added POINTS for 16-bit version.
// 
//    Rev 2.5   18 Sep 1995 09:30:06   JAY
// Fixed defs for CALLBACKPROC and uint16 under WIN32.
// 
//    Rev 2.4   14 Sep 1995 15:50:04   JAY
// Addes SIZES
// 
//    Rev 2.3   14 Sep 1995 10:01:52   JAY
// Added RECTS.
// 
//    Rev 2.2   14 Sep 1995 09:32:18   JAY
// 32-bit changes
// 
//    Rev 2.1   13 Sep 1995 11:33:48   JAY
// New 32-bit stuff
// 
//    Rev 2.0   07 Feb 1995 15:48:44   JAY
// Initial revision.
// 
//    Rev 1.4   04 Aug 1994 07:34:06   JAY
// Large model and 32-bit changes
// 
//    Rev 1.3   23 May 1994 08:20:10   JAY
// Added some constant pointers which aren't defined in WINDOWS.H.
// 
//    Rev 1.2   18 Feb 1994 12:13:08   MACDUFF
// Added pragmas to turn off some warnings, and the VCS header.
// 
*/

#ifndef __MISC_H__
#define __MISC_H__

#pragma warning(disable:4505)   // unreferenced local function
#pragma warning(disable:4699)   // creating precompiled header
#pragma warning(disable:4704)   // inline assembler precludes global optimization
#pragma warning(disable:4705)   // statement has no effect


#define	PRIVATE static
#define	PUBLIC

#define FARCLASS		class far

#ifndef FAR
#define FAR far
#endif

#define	VOID void
typedef short int SHORT;
typedef SHORT far *LPSHORT;
typedef unsigned short int USHORT;
typedef USHORT far *LPUSHORT;
typedef unsigned long int ULONG;
typedef char CHAR;
typedef unsigned char UCHAR;
#ifdef WIN32
typedef unsigned short int FLAGS;
#else
typedef unsigned int FLAGS;
#endif

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
#ifdef WIN32
typedef unsigned short uint16;
#else
typedef unsigned uint16;
#endif
typedef long int32;
typedef unsigned long uint32;

typedef VOID FAR *VOIDPTR;

#ifdef WIN32
typedef	unsigned char *PCDrawData;
#define	PCDRAWDATA	PCDrawData
#endif

struct RECTS
{
	short int left;
	short int top;
	short int right;
	short int bottom;
};

struct SIZES
{
	short int cx;
	short int cy;
};

#ifndef WIN32
struct POINTS
{
	short int x;
	short int y;
};
#endif

/* Additional types which aren't defined in WINDOWS.H */

typedef const char *PCSTR;
typedef const void *PCVOID;
typedef const void far *LPCVOID;

/*
// Some Windows stuff which is convenient to have defined.
*/

#ifdef _WIN32
#define __HUGE
#define	CALLBACKPROC CALLBACK
#else
#define __HUGE huge
#define	CALLBACKPROC	__far __pascal __export
#endif

#define	WINDOWPROC		long CALLBACKPROC
#define	DIALOGPROC		int CALLBACKPROC

#endif			// #ifndef __MISC_H__
