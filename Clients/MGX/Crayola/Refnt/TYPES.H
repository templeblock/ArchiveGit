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

#include <limits.h>

// Miscellaneous definitions
#ifndef WIN32
#define	MAX_PATH 80
#endif
#define	MAX_FNAME_LEN	MAX_PATH

#define	MAX_STR_LEN	80
#define 	MAX_EXTNAME_LEN	32
#define	MAX_IMAGE_LINE	16384

//**********************************************************************
// Type definitions
//**********************************************************************
#ifdef WIN32

#ifndef NEAR
#define NEAR
#endif

#undef FAR
#undef _far
#define FAR
#define _far

#define _export
#define __export

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
typedef LPCSTR LPCTSTR; // unicode typedef - not in win16 sdk
typedef LPSTR LPTSTR; // unicode typedef - not in win16 sdk
typedef BOOL FAR *LPBOOL; // in win32 and not in win16 sdk
typedef int INT; // in win32 and not in win16 sdk
#define APIENTRY    int PASCAL // in win32 and not in win16 sdk 
typedef MINMAXINFO FAR *LPMINMAXINFO; // in win32 and not in win16 sdk
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

#undef LPTR

typedef char * P_STR;
typedef unsigned char *PTR;

typedef unsigned char FAR *LPTR;
typedef unsigned char HUGE *HPTR;
typedef DWORD HUGE *HPDWORD;
typedef WORD HUGE *HPWORD;
typedef LPPOINT FAR *LPPPOINT;
typedef LPTR FAR *LPPTR;
typedef LPSTR FAR *LPPSTR;
typedef LPVOID FAR *LPPVOID;
typedef void HUGE *HPVOID;
typedef LPINT FAR *LPPINT;
typedef LPPINT FAR *LPPPINT;
typedef LPWORD FAR *LPPWORD;
typedef LPPWORD FAR *LPPPWORD;
typedef char FNAME[MAX_FNAME_LEN];
typedef char EXTNAME[MAX_EXTNAME_LEN];
typedef char STRING[MAX_STR_LEN];
typedef char SZNUM[16];
typedef int ITEMID;
typedef ITEMID FAR *LPITEMID;
typedef unsigned long PFIXED;
typedef unsigned long COLOR;
typedef COLOR FAR *LPCOLOR;
typedef int (FAR *LPROC)();
typedef void (FAR *LVPROC)();
typedef long (FAR *LPFL)();
typedef unsigned char FAR * (FAR *LLPTRPROC)();
typedef struct _iobuf * INIPTR;

///
/// C++ 
///

typedef void ( FAR *SHARPSMOOTHPROC )( int, LPTR, LPTR, LPTR, LPTR );

//**********************************************************************
// Constant definitions
//**********************************************************************
#ifdef _MAC
#define WINPROC		CALLBACK	// mac uses pascal calling convention
#else
#define WINPROC		WINAPI
#endif

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

//**********************************************************************
// Hue, Saturation, and Luminace color definition
//**********************************************************************
typedef struct _hsl
	{
	BYTE hue;
	BYTE sat;
	BYTE lum;
} HSLS;
typedef HSLS FAR *LPHSL;

//**********************************************************************
// Red, Green, and Blue color definition
//**********************************************************************
typedef struct _rgb
	{
	BYTE red;
	BYTE green;
	BYTE blue;
} RGBS;
typedef RGBS FAR *LPRGB, HUGE *HPRGB;

//**********************************************************************
// Cyan, Magenta, Yellow, and Black color definition
//**********************************************************************
typedef struct _cmyk
	{
	BYTE c;
	BYTE m;
	BYTE y;
	BYTE k;
} CMYKS;
typedef CMYKS FAR *LPCMYK, HUGE *HPCMYK;

//********************************************************************
// Active/Alternate Color structure
//********************************************************************

typedef enum _colorspace
{
	CS_GRAY  = 1,
	CS_HSL   = 2,
	CS_RGB   = 3,
	CS_CMYK  = 4
} COLORSPACE;

typedef struct _colorinfo
{
	COLORSPACE ColorSpace;
	int   gray;
	RGBS  rgb;
	HSLS  hsl;
	CMYKS cmyk;
	BYTE  index; // convenient place to store this
} COLORINFO, FAR *LPCOLORINFO;

//**********************************************************************
// 4 Corner Distortion definition
//**********************************************************************
typedef struct _distort
	{
	RECT RectOrig;
	RECT Rect;
	POINT p[4];
} DISTORT;
typedef DISTORT FAR *LPDISTORT;

//**********************************************************************
// Drop and drag definition
//**********************************************************************
typedef struct _drag
	{
	HWND	hWnd;
	WORD	wType;
	LPTR	lpData;
	POINT	pos;
	HRGN	hRgn;
	HCURSOR	hOldCursor;
	HCURSOR	hDragMove;
	HCURSOR hDragCopy;
	HCURSOR hDragNot;
	HWND	hDragWnd;
	BOOL	fAccept;
} DRAG;
typedef DRAG FAR *LPDRAG;

#define DAD_RGB			0
#define DAD_FILENAME	1
#define DAD_ID			2

#endif





