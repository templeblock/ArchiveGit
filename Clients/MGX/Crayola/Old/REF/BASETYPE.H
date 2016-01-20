/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from MICROGRAFX, 1303 Arapaho Rd., Richardson,
   Texas  75081.

*******************************************************************************
*******************************************************************************

                                    BaseType

*******************************************************************************
*******************************************************************************

This header file declares various base level typedefs and macros which are
included by most .C and .CPP files.  The definitions in this module are those
that are general purpose and not related to a specific operating system or
graphics environment.

*/

#ifndef _BASETYPE_H_
#define _BASETYPE_H_

#ifdef _INC_WINDOWS
#define __WINDOWS_H
#endif

#ifndef __WINDOWS_H

#define TRUE                1
#define FALSE               0
#define NULL                0

#define FAR                 far
#define NEAR                near
#define LONG                long
#define VOID                void
#define PASCAL              pascal

#define LOWORD(Value)       ((word)(Value))
#define HIWORD(Value)       ((word)(((dword)(Value) >> 16) & 0xFFFF))
#define LOBYTE(Value)       ((byte)(Value))
#define HIBYTE(Value)       ((byte)(((word)(Value) >> 8) & 0xFF))
#define MAKELONG(a,b)       ((dword)(((word)(a)) | ((dword)((word)(b))) << 16))

typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned int        WORD;
typedef unsigned long       DWORD;

typedef char near           *PSTR;
typedef char near           *NPSTR;
typedef char far            *LPSTR;

typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;

typedef int near            *PINT;
typedef int far             *LPINT;

typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;

typedef long near           *PLONG;
typedef long far            *LPLONG;

typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;

typedef void far            *LPVOID;

typedef int (far pascal *FARPROC)();
typedef int (near pascal *NEARPROC)();

#endif

/* ******************************** Constants ****************************** */
/* ******************************** Typedefs ******************************* */

typedef int            bool;
typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef unsigned int   uint;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef uint           handle;

typedef bool       *PBOOL;
typedef bool near  *NPBOOL;
typedef bool far   *LPBOOL;

typedef byte near  *NPBYTE;
typedef word near  *NPWORD;
typedef dword near *NPDWORD;

typedef short      *PSHORT;
typedef short near *NPSHORT;
typedef short far  *LPSHORT;

typedef long near  *NPLONG;
typedef long far   *LPLONG;

typedef void near  *NPVOID;
typedef void far   *LPVOID;

/* ********************************* Macros ******************************** */

#endif
