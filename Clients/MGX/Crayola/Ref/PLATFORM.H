/*  $Header:   L:/dev/img/win32/lib/pvcs/platform.h_v   1.0   13 Jul 1993 12:31:22   tedr  $     */
/*  $Log:   L:/dev/img/win32/lib/pvcs/platform.h_v  $
 * 
 *    Rev 1.0   13 Jul 1993 12:31:22   tedr
 * Initial revision.
 * 
 *    Rev 1.1   17 May 1993 15:58:52   michaelb
 * Corrected model problems.
 * 
 *    Rev 1.0   09 May 1993 16:23:06   michaelb
 * Initial revision.
 * 
 *    Rev 1.13   29 Apr 1992 16:39:04   peterm
 * 
 *    Rev 1.12   29 Apr 1992 16:35:52   peterm
 * 
 *    Rev 1.11   21 Apr 1992 11:51:12   peterm
 * fixed str255 length problem
 * 
 *    Rev 1.10   07 Apr 1992 16:36:02   giggey
 * for Mac typedef Fract PIFract; for Windows typedef long PIFract;
 * 
 *    Rev 1.9   07 Apr 1992 10:01:12   giggey
 * PLATFORM Macintosh #define huge
 * 
 *    Rev 1.7   09 Mar 1992 11:18:14   peterm
 * No change.
 * 
 *    Rev 1.6   17 Feb 1992 12:42:46   peterm
 * 
 *    Rev 1.5   11 Feb 1992 13:14:24   peterm
 * added str255 for windows
 * 
 *    Rev 1.4   15 Jan 1992 08:08:26   peterm
 * 
 *    Rev 1.3   10 Jan 1992 10:03:58   giggey
 * 
 *    Rev 1.2   12 Dec 1991 12:36:26   peterm
 * ps126
 * 
 *    Rev 1.1   12 Dec 1991 11:04:28   peterm
 * put return at end of line of last statement.
 * 
 *    Rev 1.0   25 Nov 1991 15:09:50   peterm
 * Initial revision.
*/
/*
	File: Platform.h

	Copyright 1990-92 by Thomas Knoll and Adobe Systems Inc..

	Platform specific definitions for Photoshop plugins.
*/
#ifndef __PLATFORM_H
#define __PLATFORM_H

#define Macintosh 1
#define MSWindows 2

/*#define PLATFORM Macintosh*/
#define PLATFORM MSWindows

#if PLATFORM == Macintosh
#include <Types.h>
#include <Memory.h>
#include <Resources.h>
#include <Quickdraw.h>
#include <Dialogs.h>
#include <OSUtils.h>
#include <Packages.h>
#include <Errors.h>
#include <ToolUtils.h>

#ifdef _MPW_
#define TRUE	true
#define FALSE	false
#endif

#define MACPASCAL pascal
#define WINPASCAL 
#define far
#define huge
typedef Fixed PIFixed;
typedef Fract PIFract;
typedef ProcPtr PIProcPtr;
typedef Point PIPoint;
typedef Rect  PIRect;
typedef RGBColor PIRGBColor;

typedef Ptr      PIPtr;
typedef Handle   PIHandle;
typedef Boolean  PIBoolean;
typedef OSType   PIOSType;
typedef Str255   PIStr255;
typedef OSErr    PIOSErr;

typedef struct PlatformData {

	short dummy;                    /* dummy variable required for some compilers. */

	} PlatformData;

#endif


#if PLATFORM == MSWindows

#include <windows.h>

typedef long PIFixed;
typedef long PIFract;
typedef long (far *PIProcPtr)();
typedef struct
   {
	  short v;
	  short h;
   } PIPoint;

typedef struct
   {
	  short top;
	  short left;
	  short bottom;
	  short right;
   } PIRect;

typedef struct
   {
	  WORD red;                 /*magnitude of red   component, 16 bit significant.*/
	  WORD green;               /*magnitude of green component, 16 bit significant.*/
	  WORD blue;                /*magnitude of blue  component, 16 bit significant.*/
   } PIRGBColor;

typedef LPSTR PIPtr;
typedef LPSTR far *PIHandle;
typedef BYTE  PIBoolean;
typedef DWORD PIOSType;
typedef short PIOSErr;

typedef unsigned char PIStr255[256];  /*  first byte length of string.
										The string is zero terminated. */

typedef struct PlatformData {

	unsigned short hwnd;        /* window to own dialogs. */

	} PlatformData;


#define MACPASCAL
#define WINPASCAL pascal
#define nil NULL

  /* Error codes. */
#define memFullErr -108
#define userCanceledErr -128
#define noErr 0
#define writErr -20
#define openErr -23

#define true       TRUE
#define false      FALSE

#define SysBeep(x) MessageBeep(0)
#endif


#endif
