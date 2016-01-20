/*
// $Header: /PM8/App/CFIXED.H 1     3/03/99 6:04p Gbeddow $
//
// $Workfile: CFIXED.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CFIXED.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:00   Fred
// Initial revision.
// 
//    Rev 1.6   12 May 1997 11:09:02   Fred
// Added MakeFixed() validation to debug build
// 
//    Rev 1.5   11 Feb 1997 14:53:56   Jay
// 16-bit changes
// 
//    Rev 1.4   07 Feb 1997 08:57:40   Fred
// Added FixedRound() method.
// 
//    Rev 1.3   20 Jan 1997 10:02:28   Fred
// Speedups
// 
//    Rev 1.2   14 Jun 1996 17:05:46   Jay
//  
// 
//    Rev 1.1   13 Jun 1996 15:53:20   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:44:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:40   FRED
// Initial revision.
// 
//    Rev 1.7   28 Jul 1995 12:27:56   JAY
// Added == and != operators to CFixedRect.
// 
//    Rev 1.6   23 Jun 1995 11:38:52   JAY
// Added OneThirdFixed and MidPointFixed.
// 
//    Rev 1.5   23 Jun 1995 08:34:50   JAY
// Added a warning comment.
// 
//    Rev 1.4   18 May 1995 10:51:48   FRED
// Code to get bounding box of path.
// 
// 
//    Rev 1.3   18 May 1995 07:44:48   JAY
// Added operator !=.
// 
//    Rev 1.2   09 May 1995 08:03:02   JAY
// Added Tangent().
// 
//    Rev 1.1   05 May 1995 16:49:30   JAY
// Exciting new additions to OutlinePaths!
// 
//    Rev 1.0   04 May 1995 17:14:32   JAY
// Initial revision.
*/

#ifndef __CFIXED_H__
#define __CFIXED_H__

typedef long CFixed;

struct CFixedPoint
{
	CFixed x;
	CFixed y;
	BOOL operator==(const CFixedPoint& p) const
		{ return memcmp(&p, this, sizeof(*this)) == 0; }
	BOOL operator!=(const CFixedPoint& p) const
		{ return !(operator==(p)); }
};

/*
// CFixedRect
// Important note:
// *** Do NOT add any virtual functions to this struct.
// *** It is used as a struct (i.e. it is used as if its memory image is
// *** exactly its definition) in at least one place. There can NOT be a
// *** v-table or things will break!
*/

struct CFixedRect
{
	CFixed Left;
	CFixed Top;
	CFixed Right;
	CFixed Bottom;
	CFixed Width(void) const
		{ return Right-Left; }
	CFixed Height(void) const
		{ return Bottom-Top; }
	BOOL operator==(const CFixedRect& r) const
		{ return memcmp(&r, this, sizeof(*this)) == 0; }
	BOOL operator!=(const CFixedRect& r) const
		{ return !(operator==(r)); }
};

#ifndef _DEBUG
inline CFixed MakeFixed(long v)
	{ return MAKELONG(0, (int)v); }
inline CFixed MakeFixed(int v)
	{ return MAKELONG(0, v); }
inline CFixed MakeFixed(int v, unsigned int f)
	{ return MAKELONG(f, v); }
inline CFixed MakeFixed(double v)
	{ return long(v*65536.0); }
inline double MakeDouble(CFixed f)
	{ return double(f)/65536.0; }
inline unsigned FixedFraction(CFixed f)
	{ return (unsigned)LOWORD(f); }
inline int FixedInteger(CFixed f)
	{ return (int)(short int)HIWORD(f); }
inline int FixedRound(CFixed f)
	{ return (int)(short int)HIWORD((f)+0x8000L); }
#else
CFixed MakeFixed(long v);
CFixed MakeFixed(int v);
CFixed MakeFixed(int v, unsigned int f);
CFixed MakeFixed(double v);
double MakeDouble(CFixed f);
unsigned FixedFraction(CFixed f);
int FixedInteger(CFixed f);
int FixedRound(CFixed f);
#endif

extern CFixed Cosine(CFixed lDegrees);
extern CFixed Sine(CFixed lDegrees);
extern CFixed Tangent(CFixed lDegrees);
extern CFixed ArcTangent2(CFixed lDY, CFixed lDX);

// From db386.asm

extern "C" CFixed pascal MulFixed(CFixed A, CFixed B);
extern "C" CFixed pascal DivFixed(CFixed A, CFixed B);
extern "C" CFixed pascal MulDivFixed(CFixed A, CFixed B, CFixed C);
// Compute (2*A+B)/3.
extern "C" CFixed pascal OneThirdFixed(CFixed A, CFixed B);
extern "C" CFixed pascal MidPointFixed(CFixed A, CFixed B);

// A value when you do not want a value. Used here and there.
#define UNDEFINED_CFIXED		0x80000000

#endif				// #ifndef __CFIXED_H__
