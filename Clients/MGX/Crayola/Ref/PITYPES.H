/* Copyright 1992 by Adobe Systems, Inc.  All rights reserved. */

#ifndef __PITypes__
#define __PITypes__

/* Here are the type definitions used in the interface files.       */

/******************************************************************************/

// Set up the preprocessor flags to indicate the target platform.

#ifdef macintosh
#define Macintosh 1
#endif

#ifdef THINK_C
#define Macintosh 1
#endif

#ifdef MSDOS
#define MSWindows 1
#endif

#ifndef Macintosh
#define Macintosh 0
#endif

#ifndef MSWindows
#define MSWindows 0
#endif

/******************************************************************************/
/* Pragma to byte align structures; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a-
#endif

/******************************************************************************/

#if Macintosh

#ifndef __PSWorld__

#include <Types.h>
#include <Memory.h>
#include <Resources.h>
#include <Quickdraw.h>
#include <Dialogs.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <Packages.h>
#include <Errors.h>
#include <Files.h>

#endif

#ifndef TRUE
#define TRUE	true
#endif

#ifndef FALSE
#define FALSE	false
#endif

#ifndef MACPASCAL
#define MACPASCAL pascal
#endif

#ifndef WINPASCAL
#define WINPASCAL
#endif

#ifndef far
#define far
#endif

#ifndef huge
#define huge
#endif

#ifndef __cplusplus
#define const
#endif

#ifndef __PSWorld__

typedef signed long  int32;
typedef signed short int16;

typedef unsigned long  unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char  unsigned8;

typedef unsigned8 LookUpTable [256];

typedef struct
	{
	LookUpTable R;
	LookUpTable G;
	LookUpTable B;
	} RGBLookUpTable;
	
#endif

typedef struct PlatformData {

	short dummy;                    /* dummy variable required for some compilers. */

	} PlatformData;


#define PISetRect	SetRect		/* SetRect prototype conflict with windows sdk version */

#endif    /* Macintosh */

/******************************************************************************/

#if MSWindows

#define MACPASCAL
#define WINPASCAL pascal 

#ifndef __PSWorld__
#include <windows.h>

/* Error codes. */
#define memFullErr (-108)
#define userCanceledErr 1
#define noErr 0
#define readErr (-19)
#define writErr (-20)
#define openErr (-23)
#define ioErr	(-36)
#define memWZErr  (-111)
#define nilHandleErr (-109)
#define dskFulErr	(-34)
#define eofErr	(-39)

#define nil		   NULL
#define true       TRUE
#define false      FALSE

#ifndef __cplusplus
#define const
#endif

typedef signed long  int32;
typedef signed short int16;

typedef unsigned long  unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char  unsigned8;

typedef long Fixed;
typedef long Fract;
typedef long (*ProcPtr)();
typedef struct
   {
	  short v;
	  short h;
   } Point;

typedef struct
   {
	  short top;
	  short left;
	  short bottom;
	  short right;
   } Rect;

typedef struct
   {
	  WORD red;        /* Magnitude of red   component, 16 bit significant.*/
	  WORD green;      /* Magnitude of green component, 16 bit significant.*/
	  WORD blue;       /* Magnitude of blue  component, 16 bit significant.*/
   } RGBColor;

typedef LPSTR Ptr;
typedef LPSTR *Handle;
typedef BYTE  Boolean;
typedef DWORD OSType;
typedef short OSErr;
typedef unsigned long ResType;

typedef unsigned char Str255[256];  /*  first byte length of string.
										The string is zero terminated. */

typedef unsigned8 LookUpTable [256];

typedef struct
	{
	LookUpTable R;
	LookUpTable G;
	LookUpTable B;
	} RGBLookUpTable;
	
typedef struct
	{
	short vRefNum;
	long parID;
	Str255 name;	/* Pascal String, for Windows files. */
	} FSSpec;

#endif

typedef struct PlatformData {

	unsigned short hwnd;        /* window to own dialogs. */

	} PlatformData;

/* Mac Toolbox memory management functions */

#define SysBeep(x) MessageBeep(0)

Handle NewHandle (long size);
long   GetHandleSize (Handle handle);
void   SetHandleSize (Handle handle, long newSize);
void   DisposHandle (Handle handle);
void   HLock (Handle h);
void   HUnlock (Handle h);
void   MoveHHi (Handle h);
Ptr    NewPtr (long size);
void   DisposPtr (Ptr p);
OSErr  MemError(void);
void 	 PISetRect (Rect *rect, short left, short top, short right, short bottom);

short	 ShowAlert(short stringID);	// Developer implements this



#endif    /* Windows */

/******************************************************************************/

#ifndef __PSWorld__

//typedef Rect CRect;

typedef struct
	{
	int32 top;
	int32 left;
	int32 bottom;
	int32 right;
	} VRect;
	
#if Macintosh

typedef char TVolumeName [32];

#else

typedef char TVolumeName [256];

#endif
	
#endif

/******************************************************************************/
/* turn off the byte align pragma back to its original state; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a.
#endif

/******************************************************************************/


#endif
