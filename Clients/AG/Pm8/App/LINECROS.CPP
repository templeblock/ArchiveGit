/*
// Textflow routines.
//
// $Header: /PM8/App/LINECROS.CPP 1     3/03/99 6:07p Gbeddow $
//
// $Log: /PM8/App/LINECROS.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 3     1/21/99 12:04p Cschendel
// In AddPathCrossing() changed the logic for beziers to accept a bezier
// which represents a straight line. COutlinePath converts lines to
// beziers when it inflates the path.
// 
// 2     2/05/98 11:04a Jayn
// Changed 0x07ffffff to 0x7fffffff.
// 
//    Rev 1.0   14 Aug 1997 15:22:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:02   Fred
// Initial revision.
// 
//    Rev 1.2   22 Jul 1996 18:27:14   Jay
// Can pass NULL path. Zero crossings go away.
// 
//    Rev 1.1   12 Jul 1996 16:47:54   Jay
// Text wrap
// 
//    Rev 1.0   12 Jul 1996 10:42:00   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "linecros.h"
#include "outpath.h"

/*
// Constructor.
*/

CLineCrossings::CLineCrossings()
{
}

/*
// Destructor.
*/

CLineCrossings::~CLineCrossings()
{
	RemoveAll();
}

// Compute all the crossings of the path with the band defined by the
// passed rect

void CLineCrossings::Compute(const CFixedRect& Band, COutlinePath* pPath)
{
	// Add in the initial crossings to account for the universe and the band.
	// The band is a hole in the middle of the (solid) universe.

	AddCrossing(-0x7fffffff, 1);				// Left of universe.
	AddCrossing(Band.Left, -1);				// Left of band.
	AddCrossing(Band.Right, 1);				// Right of band.
	AddCrossing(0x7fffffff, -1);				// Right of universe.

	if (pPath != NULL)
	{
		AddPathCrossings(*pPath, Band.Top, Band.Bottom);
	}
}

static inline int ComputePlace(CFixed y, CFixed lTop, CFixed lBottom)
{
	return (y <= lTop)
					? -1
					: (y >= lBottom)
							? 1
							: 0;
}

/*
// Add the crossings for an outline path (where it crosses the top and bottom).
*/

void CLineCrossings::AddPathCrossings(COutlinePath& Path, CFixed lTop, CFixed lBottom)
{
	if (lTop == lBottom)
	{
		return;
	}

	// Current variables.
	CFixedPoint cp;
	CFixed lCurrentMin, lCurrentMax;
	int nCurrentPlace;

	// First variables.
	BOOL fFirst;
	CFixed lFirstExitX;
	CFixed lFirstMin, lFirstMax;
	int nFirstExitPlace;

	// Entry variables.
	CFixed lEntryX;
	int nEntryPlace;

	// Loop on elements.
	long* pElement = (long*)Path.GetElement(0);
	while (pElement != NULL)
	{
		int type = *(pElement++);
		switch (type)
		{
			case COutlinePathArray::OPE_End:
			{
				pElement = NULL;
				break;
			}

			case COutlinePathArray::OPE_Close:
			{
				if (nCurrentPlace == 0)
				{
					// We are ending in the band. This means we started in the
					// band. We need to add some crossings here.
					if (fFirst)
					{
						// We never left the band!
						AddBandCrossing(nCurrentPlace, nCurrentPlace,
											 lCurrentMin, lCurrentMax,
											 lCurrentMin, lCurrentMax);
					}
					else
					{
						// We left the band and came back.
						// Combine the first piece and this piece.
						if (lFirstMin < lCurrentMin)
						{
							lCurrentMin = lFirstMin;
						}
						if (lFirstMax > lCurrentMax)
						{
							lCurrentMax = lFirstMax;
						}
						AddBandCrossing(nEntryPlace, nFirstExitPlace,
											 lEntryX, lFirstExitX,
											 lCurrentMin, lCurrentMax);
					}
				}
				break;
			}

			case COutlinePathArray::OPE_MoveTo:
			{
				// The start of a new path.
				cp.x = *(pElement++);
				cp.y = *(pElement++);

				nCurrentPlace = ComputePlace(cp.y, lTop, lBottom);

				fFirst = FALSE;
				if (nCurrentPlace == 0)
				{
					fFirst = TRUE;
					// We are starting within the band.
					// We will not be able to complete this piece of the path
					// until the path comes around again.
					// We will remember the extent for this first piece until
					// it leaves the band (if it ever does). When we catch up
					// with it on the back end, we will combine this piece with
					// the last piece. The big assumption here is that the
					// path is terminate by a Close token. If not, this piece
					// is lost.

					lCurrentMin =
						lCurrentMax = cp.x;
				}
				break;
			}

			case COutlinePathArray::OPE_BezierTo:
			case COutlinePathArray::OPE_LineTo:
			{
				// unpack the data
				CFixedPoint np;
				if( type == COutlinePathArray::OPE_BezierTo )
				{
					long ax = *(pElement++);
					long ay = *(pElement++);
					long bx = *(pElement++);
					long by = *(pElement++);
					long cx = *(pElement++);
					long cy = *(pElement++);
					
					
					// have a line converted to a bezier (done when a path is inflated)
					if( ax == bx && ay == by )
					{
						np.x = ax;
						np.y = by;
					}
					else // true bezier not supported
					{
						ASSERT(FALSE);
						break;
					}
				}
				else
				{
					np.x = *(pElement++);
					np.y = *(pElement++);
				}
				// Determine if this line intersects the band.

				// Compute where we are in relation to the band at the end
				// of this line.

				int nNewPlace = ComputePlace(np.y, lTop, lBottom);
				if (nNewPlace == nCurrentPlace)
				{
					// Still on same side of the band.
					if (nCurrentPlace == 0)
					{
						// We are currently in the band.
						// Update the min and max.
						if (np.x < lCurrentMin)
						{
							lCurrentMin = np.x;
						}
						if (np.x > lCurrentMax)
						{
							lCurrentMax = np.x;
						}
					}
				}
				else
				{
					// A crossing is happening.

					// Compute deltas for the slope. The slope itself is not
					// calculated, because I think it will be more accurate to do
					// a MulDiv later than a Div now and a Mul later using the
					// quotient. If necessary, you can pick up some speed by doing
					// it the other way.
					CFixed lDX = np.x - cp.x;
					CFixed lDY = np.y - cp.y;

					if (nCurrentPlace == 0)
					{
						// We are exiting the band.

						// Compute the point of exit.
						CFixed lExitX;
						if (nNewPlace == -1)
						{
							// Compute the intersection of the line with the
							// top of the band.
							lExitX = cp.x + MulDivFixed(lTop - cp.y, lDX, lDY);
						}
						else
						{
							// Compute the intersection of the line with the
							// bottom of the band.
							lExitX = cp.x + MulDivFixed(lBottom - cp.y, lDX, lDY);
						}

						if (lExitX < lCurrentMin)
						{
							lCurrentMin = lExitX;
						}
						else if (lExitX > lCurrentMax)
						{
							lCurrentMax = lExitX;
						}

						if (fFirst)
						{
							// We can not do anything yet.
							// Remember this stuff for later.
							lFirstExitX = lExitX;
							lFirstMin = lCurrentMin;
							lFirstMax = lCurrentMax;
							nFirstExitPlace = nNewPlace;

							fFirst = FALSE;
						}
						else
						{
							// Not special first piece.
							// We have entered and exited. Add the band crossing.
							AddBandCrossing(nEntryPlace, nNewPlace,
												 lEntryX, lExitX,
												 lCurrentMin, lCurrentMax);
						}
					}
					else if (nNewPlace == 0)
					{
						// We are entering the band.
						// Remember the point of entry.
						if ((nEntryPlace = nCurrentPlace) == -1)
						{
							// Compute the intersection of the line with the
							// top of the band.
							lEntryX = cp.x + MulDivFixed(lTop - cp.y, lDX, lDY);
						}
						else
						{
							// Compute the intersection of the line with the
							// bottom of the band.
							lEntryX = cp.x + MulDivFixed(lBottom - cp.y, lDX, lDY);
						}

						// Establish the min and max.
						if (lEntryX < np.x)
						{
							lCurrentMin = lEntryX;
							lCurrentMax = np.x;
						}
						else
						{
							lCurrentMin = np.x;
							lCurrentMax = lEntryX;
						}

						// Entering from out of band should never have this.
						ASSERT(!fFirst);
					}
					else
					{
						// We are crossing the band. This is a special case.
						// Compute the top and bottom x intersection values.
						CFixed lTopX = cp.x + MulDivFixed(lTop - cp.y, lDX, lDY);
						CFixed lBottomX = cp.x + MulDivFixed(lBottom - cp.y, lDX, lDY);

						if (lTopX > lBottomX)
						{
							CFixed l = lTopX;
							lTopX = lBottomX;
							lBottomX = l;
		}

						AddBandCrossing(nCurrentPlace, nNewPlace,
											 lTopX, lBottomX,			// ignored
											 lTopX, lBottomX);
					}
				}

				cp = np;
				nCurrentPlace = nNewPlace;
				break;
			}
			case COutlinePathArray::OPE_SplineTo:
			{
				long AX = *(pElement++);
				long AY = *(pElement++);
				long BX = *(pElement++);
				long BY = *(pElement++);
				// Someday!
				ASSERT(FALSE);
				break;
			}

			default:
			{
				ASSERT(FALSE);
				pElement = NULL;
				break;
			}
		}
	}
}

void CLineCrossings::AddBandCrossing(int nEntryPlace, int nExitPlace,
												CFixed lEntryX, CFixed lExitX,
												CFixed lMin, CFixed lMax)
{
	if (nEntryPlace == nExitPlace)
	{
		// The path has entered and exited on the same
		// side of the band. If the entry and exit cause
		// a "hump" (a piece of interior in empty space),
		// then we add it. If it causes a "negative hump"
		// (a piece of empty space in the interior), then
		// we ignore it.

		BOOL fHump = (lExitX > lEntryX);
		if (nExitPlace == -1)
		{
			fHump = !fHump;
		}
		if (fHump)
		{
			AddCrossing(lMin, 1);
			AddCrossing(lMax, -1);
		}
	}
	else
	{
		// The path has crossed the band.
		// We need to add one crossing which depends on
		// the crossing direction.
		if (nEntryPlace == 1)
		{
   			ASSERT(nExitPlace == -1);
			// Crossing from bottom to top.
			AddCrossing(lMin, 1);
		}
		else
		{
			ASSERT(nExitPlace == 1);
			// Crossing from top to bottom.
			AddCrossing(lMax, -1);
		}
	}
}

/*
// Remove all crossings.
*/

void CLineCrossings::RemoveAll(void)
{
	int nCrossings = Crossings();
	for (int nCrossing = 0; nCrossing < nCrossings; nCrossing++)
	{
		delete Crossing(nCrossing);
	}

	CPtrArray::RemoveAll();
}

/*
// Add a crossing.
// The crossings are kept sorted.
*/

void CLineCrossings::AddCrossing(CFixed lPosition, int nCount)
{
	int nCrossings = Crossings();
	int nCrossing;

	// Look for the crossing in the array.
	// This will also determine the place to insert if it is not found.
	for (nCrossing = 0; nCrossing < nCrossings; nCrossing++)
	{
		// Get the crossing to check where we are.
		CLineCrossing* pCrossing = Crossing(nCrossing);

		// If this crossing is at our position, merge the counts.
		if (pCrossing->m_lPosition == lPosition)
		{
			// Update the existing one.
			if ((pCrossing->m_nCount += nCount) == 0)
			{
				// The crossing went away! Get rid of it.
				delete pCrossing;
				RemoveAt(nCrossing);
			}
			return;
		}
		if (pCrossing->m_lPosition > lPosition)
		{
			// Insert here.
			break;
		}
	}

	// The crossing was not found. Create a new one.
	CLineCrossing* pCrossing = new CLineCrossing;

	// Fill in the values.
	pCrossing->m_lPosition = lPosition;
	pCrossing->m_nCount = nCount;

	// Insert it where we stopped searching.
	InsertAt(nCrossing, pCrossing);
}
