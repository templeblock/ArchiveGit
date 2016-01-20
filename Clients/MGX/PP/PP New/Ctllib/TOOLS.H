/*-------------------------------------------------------------------------
    %%File:  tools.h
    %%Owner: stevebu

    tools library public header file

    To use this libary you will need to link with the following:
    	DEBUG: toolsd.lib, toolhelp.lib
    	SHIP:  tools.lib

-------------------------------------------------------------------------*/
#ifndef _TOOLSENV
#define _TOOLSENV
#include <windows.h>
#include <windowsx.h>

// Hungarian Definitions
#define fTrue	1
#define fFalse	0

// Version Definitions
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG

#ifdef _NDEBUG
#define SHIP
#endif // _NDEBUG

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned short BF;
typedef unsigned char BFC;
typedef unsigned long BFL;
typedef short DOC;


// Utility Macros
#ifndef WIN32
#define EXPORT	__export
#define SegConst(type, seg)      static type _based(_segname("seg"))
#else
#define EXPORT
#define SegConst(type, seg)      static type
#endif
#define SegConstCode(type, seg)  SegConst(type, seg)
#define CsConst(type)   			SegConstCode(type, "_CODE")
#define cbof(type)					(sizeof(type))
// both of these mean:  "a >= b && a <= c"
#define FBetween(a, b, c)	((unsigned)((a) - (b)) <= (c) - (b))
#define FInRange(a, b, c)	FBetween(a, b, c)

#ifdef DEBUG
#define CommSzLib(lsz) (OutputDebugString((LPCSTR)lsz))
#define CommSz(lsz) (CommSzLib(lsz))
#define DEBUGASSERTSZ CsConst(uchar) szAssertFile[] = __FILE__;
#else
#define CommSz(lsz)
#define DEBUGASSERTSZ
#endif /* DEBUG */

// Debug Macros
#ifdef DEBUG
int AssertSzProc(uchar FAR *, uchar FAR *, int);
#define AssertSz(f, lsz) ((f) ? 0 : AssertSzProc((uchar FAR *)lsz, (uchar FAR *)szAssertFile,(int)__LINE__))
#define Assert(f) AssertSz(f, "")
#else
#define AssertSz(f, lsz)
#define Assert(f)
#endif // DEBUG

// string defines
#define cbMaxSz 	256
#define cchMaxSz	255
#define cbMaxSt	256
#define cchMaxSt	255
#define cbMaxStz	257
#define cchMaxStz	256


// Prototypes
void SetBytes(void FAR *, uint, uint);
#define ClearBytes(_lpb_, _cb_) (SetBytes(_lpb_, 0, _cb_))
void FAR *LpbCopyLprgb(void FAR *, void FAR *, uint);
uint CchLsz(const uchar FAR *);

// String Macros                
#define LszToLstz(_lsz1_, _lstz2_) 		(LprgchToLstz(_lsz1_, CchLsz(_lsz1_),_lstz2_))
#define LstzToLsz(_lstz1_, _lsz2_)		(LpbCopyLprgb(_lstz1_+1,_lsz2_,CchLstz(_lstz1_)+1))
#define LstToLstz(_lst1_, _lstz2_)		(LprgchToLstz(_lst1_+1,CchLst(_lst1_),_lstz2_))
#define LstzToLst(_lstz1_, _lst2_) 		(LpbCopyLprgb(_lstz1_, _lst2_, CchLstz(_lstz1_)+1))
#define LszToLst(_lsz_, _lst_)  	  	(LpbCopyLprgb(_lsz_, _lst_+1, (_lst_[0] = CchLsz(_lsz_))))
#define LstToLsz(_lst_, _lsz_) 			(*(uchar FAR *)LpbCopyLprgb((_lst_)+1, (_lsz_), CchLst((_lst_))) = '\0')
#define LpchToLsz(_lpch_, _cch_, _lsz_)	(*(uchar FAR *)LpbCopyLprgb((_lpch_), (_lsz), _cch_) = '\0')
#define CbLsz(_lsz_)					(CchLsz((_lsz_))+1)
#define CopyLsz(_lszSrc_, _lszDst_) 	(LpbCopyLprgb((_lszSrc_), (_lszDst_), CbLsz((_lszSrc_))))

// Memory Defines
#define lpNil	((void FAR *)0)

// Memory Allocation Routines
#define LpvAllocCb(_cb_, _flags_) ((void FAR *)GlobalAllocPtr(_flags_, _cb_))
#define LpvReallocCb(_lp_, _cbNew_, _flags_) ((void FAR *)GlobalReAllocPtr(_lp_, _cbNew_, _flags_))
#define FreeLpv(_lp_) (GlobalFreePtr(_lp_))
#define FreePlpv(_plpv_) (FreeLpv(*_plpv_), *_plpv_ = lpNil)

DEBUGASSERTSZ
#endif // _TOOLSENV
