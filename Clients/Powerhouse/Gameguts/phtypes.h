#ifndef _PHTYPES_H_
#define _PHTYPES_H_

#include <windows.h>
#include <windowsx.h>

#ifndef RC_INVOKED
// Since we use -W# warnings, lets turn off any annoying ones
//#pragma warning( disable : 4018 4101 4135 4704)
//	4018 signed/unsigned mismatch
//	4101 unreferenced local variable
//	4135 conversion between different integral types
//	4704 in-line assemble precludes optimization
#endif

// SHORT version of a point.  "Win16 version"
#ifndef WIN32
typedef struct tagPOINTS
{
	short x;
	short y;
} POINTS;
typedef POINTS* PPOINTS;
typedef POINTS NEAR* NPPOINTS;
typedef POINTS FAR* LPPOINTS;
typedef BOOL FAR* LPBOOL;
#endif

// Miscellaneous definitions
#ifdef WIN32
	#define MAX_FNAME_LEN 260
	#define MAX_STR_LEN 260
#else
	#define MAX_FNAME_LEN 80
	#define MAX_STR_LEN 80
#endif

//**********************************************************************
// Type definitions
//**********************************************************************
#ifdef WIN32
#define HUGE
typedef RGBTRIPLE FAR* LPRGBTRIPLE;
#else
#define HUGE huge
typedef LPCSTR LPCTSTR;
typedef LPSTR LPTSTR;
#endif

#undef LPTR
typedef LPPOINT FAR *LPPPOINT;
typedef unsigned char NEAR *PTR;
typedef unsigned char FAR *LPTR;
typedef short FAR *LPSHORT;
typedef unsigned char HUGE *HPTR;
typedef LPTR FAR *LPPTR;
typedef LPVOID FAR *LPPVOID;
typedef LPINT FAR *LPPINT;
typedef LPPINT FAR *LPPPINT;
typedef LPWORD FAR *LPPWORD;
typedef LPPWORD FAR *LPPPWORD;
typedef char FNAME[MAX_FNAME_LEN];
typedef char STRING[MAX_STR_LEN];
typedef char SZNUM[16];
typedef unsigned int UINT16;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef long LFIXED;
typedef LFIXED FAR *LPLFIXED;
typedef LPLFIXED FAR *LPPLFIXED;
typedef unsigned int ITEMID;
typedef unsigned int HMCI;
typedef unsigned long COLOR;
typedef double FAR *LPDOUBLE;
typedef COLOR FAR *LPCOLOR;
typedef int (FAR *PROCINT)();
typedef void (FAR *PROCVOID)();
typedef long (FAR *PROCLONG)();
typedef LPTR (FAR *PROCLPTR)();

//**********************************************************************
// Constant definitions
//**********************************************************************
#ifdef WIN32
#define EXPORT
#else
#ifndef EXPORT
#define EXPORT		__export
#endif
#endif
#define WINPROC		FAR pascal EXPORT
#define LOCAL		static
#define YES			1
#define NO			0
#define ON			1
#define OFF			0

#define HALF				0x00008000L
#define UNITY				0x00010000L
#define FMUL(num, fixed)	fmul( (long)num, (LFIXED)fixed)
#define FGET(n, d)			fget((long)n, (long)d)
#define FDIVWHOLE(f1, f2)	((f1) / (f2))
#define ROUND(f)			((short)(((f) + HALF) >> 16))
#define WHOLE(f)			((short)HIWORD(f))
#define FRAC(f)				((unsigned short)LOWORD(f))
#define TOFIXED(n)			((n)>0 ? ((long)(n))<<16 : -(((long)-(n))<<16))

//**********************************************************************
// Red, Green, and Blue color definition
//**********************************************************************
typedef struct _rgb
	{
	BYTE red;
	BYTE green;
	BYTE blue;
} RGBS;
typedef RGBS FAR *LPRGB;

//**********************************************************************
// The drawn crack structure definition
//**********************************************************************
typedef struct _crack
	{
	POINT pt1, pt2;
	BOOL bxpos, bypos, bxlty;
	int iMaxLen;
	struct _crack FAR * lpNext;
} CRACK, FAR *LPCRACK;;

#endif // _PHTYPES_H_
