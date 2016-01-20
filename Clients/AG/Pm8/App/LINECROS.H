/*
// $Header: /PM8/App/LINECROS.H 1     3/03/99 6:07p Gbeddow $
//
// Definitions for the line crossing array.
//
// $Log: /PM8/App/LINECROS.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:30   Fred
// Initial revision.
// 
//    Rev 1.3   13 Mar 1997 12:02:38   Fred
// Support for rotated text flowaround
// 
//    Rev 1.2   22 Jul 1996 18:27:14   Jay
// Can pass NULL path. Zero crossings go away.
// 
//    Rev 1.1   12 Jul 1996 16:47:54   Jay
// Text wrap
// 
//    Rev 1.0   12 Jul 1996 10:41:58   Jay
// Initial revision.
*/

struct CLineCrossing;
class CLineCrossings;

#ifndef __LINECROS_H__
#define __LINECROS_H__

class COutlinePath;

#include "cfixed.h"

struct CLineCrossing
{
	CFixed m_lPosition;
	int m_nCount;
};

class CLineCrossings : public CPtrArray
{
public:
	CLineCrossings();
	virtual ~CLineCrossings();

	// Compute all the crossings of the path with the band defined by the
	// passed rect
	void Compute(const CFixedRect& Band, COutlinePath* pPath);

	// How many crossing do we have?
	int Crossings(void)
		{ return GetSize(); }

	// Get me a particular crossing.
	CLineCrossing* Crossing(int nCrossing)
		{ return (CLineCrossing*)GetAt(nCrossing); }

	// Add the crossings for an outline path.
	void AddPathCrossings(COutlinePath& Path, CFixed lTop, CFixed lBottom);

	// Add a crossing.
	void AddCrossing(CFixed lPosition, int nCount);

	// Remove all crossings.
	void RemoveAll(void);

protected:
	void AddBandCrossing(int nEntryPlace, int nExitPlace,
								CFixed lEntryX, CFixed lExitX,
								CFixed lMin, CFixed lMax);
};

#endif
