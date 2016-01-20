#ifndef __types
#define __types

#ifndef RC_INVOKED
// Since we use -W# warnings, lets turn off any annoying ones
#pragma warning( disable : 4018 4101 4135 4704)
//	4018 signed/unsigned mismatch
//	4101 unreferenced local variable
//	4135 conversion between different integral types
//	4704 in-line assemble precludes optimization
#endif

#include <ole.h>

// Miscellaneous definitions
#define	MAX_FNAME_LEN	80
#define	MAX_STR_LEN	80
#define	MAX_IMAGE_LINE	16384

//**********************************************************************
// Type definitions
//**********************************************************************
#undef LPTR
typedef LPPOINT far *LPPPOINT;
typedef unsigned char near *PTR;
typedef unsigned char _far *LPTR;
typedef LPTR _far *LPPTR;
typedef LPINT _far *LPPINT;
typedef LPPINT _far *LPPPINT;
typedef unsigned char huge *HPTR;
typedef LPWORD _far *LPPWORD;
typedef LPPWORD _far *LPPPWORD;
typedef char FNAME[MAX_FNAME_LEN];
typedef char STRING[MAX_STR_LEN];
typedef char SZNUM[16];
typedef unsigned int UINT16;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef long LFIXED;
typedef LFIXED _far *LPLFIXED;
typedef LPLFIXED _far *LPPLFIXED;
typedef int ITEMID;
typedef unsigned long PFIXED;
typedef unsigned long COLOR;
typedef COLOR _far *LPCOLOR;
typedef int (_far *LPROC)();
typedef void (_far *LVPROC)();
typedef long (_far *LPFL)();
typedef unsigned char _far * (_far *LLPTRPROC)();
typedef struct _iobuf * INIPTR;

//**********************************************************************
// Constant definitions
//**********************************************************************
#define WINPROC		_far _pascal
#ifndef EXPORT
#define EXPORT		_export
#endif
#define LOCAL		static
#define REG			register
#define YES			1
#define NO			0
#define MAYBE		-1
#define ON			1
#define OFF			0
#define UNITY		65536L
#define HALF		32768L

//**********************************************************************
// Fixed-Point Point definition
//**********************************************************************
typedef struct _fpoint
	{
	LFIXED	fx;
	LFIXED	fy;
} FPOINT;
typedef FPOINT _far *LPFPOINT;

//**********************************************************************
// Hue, Saturation, and Luminace color definition
//**********************************************************************
typedef struct _hsl
	{
	BYTE hue;
	BYTE sat;
	BYTE lum;
} HSLS;
typedef HSLS _far *LPHSL;

//**********************************************************************
// Red, Green, and Blue color definition
//**********************************************************************
typedef struct _rgb
	{
	BYTE red;
	BYTE green;
	BYTE blue;
} RGBS;
typedef RGBS _far *LPRGB;

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
typedef CMYKS _far *LPCMYK;

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
} COLORINFO, _far *LPCOLORINFO;

//**********************************************************************
// Transformation matrix definition
//**********************************************************************
typedef struct _tform
	{
	LFIXED ax, ay;			//  ax  ay  0
	LFIXED bx, by;			//  bx  by  0
	LFIXED cx, cy;			//  cx  cy  1
} TFORM;
typedef TFORM _far *LPTFORM;

//**********************************************************************
// 4 Corner Distortion definition
//**********************************************************************
typedef struct _distort
	{
	RECT RectOrig;
	RECT Rect;
	POINT p[4];
} DISTORT;
typedef DISTORT _far *LPDISTORT;

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
typedef DRAG _far *LPDRAG;

#define DAD_RGB			0
#define DAD_FILENAME	1
#define DAD_ID			2

#endif

