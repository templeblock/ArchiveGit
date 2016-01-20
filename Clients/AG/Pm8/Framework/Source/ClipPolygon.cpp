#include	"FrameworkIncludes.h"

#include	"ClipPolygon.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"ExceptionHandling.h"

ASSERTNAME

#ifdef TPSDEBUG
int clipdebugindent = -1;
#define	VALIDATEPOLY(poly)	(poly).Validate()
#else
#define	VALIDATEPOLY(poly)
#endif


#ifdef	CUSTOMIZE_NEW

#include "FixedSizeAllocator.h"

FixedSizeAllocator<RPolyPoint>						RPolyPointAllocator;
FixedSizeAllocator<RClipPolygon::LMTEntry>		LMTEntryAllocator;
FixedSizeAllocator<RPolygonClipper::SBTEntry>	SBTEntryAllocator;

void* RPolyPoint::operator new( size_t )
{
	return ::RPolyPointAllocator.AllocateObject( );
}

void RPolyPoint::operator delete( void* pData )
{
	RPolyPointAllocator.FreeObject( pData );
}

void* RClipPolygon::LMTEntry::operator new( size_t )
{
	return LMTEntryAllocator.AllocateObject( );
}

void RClipPolygon::LMTEntry::operator delete( void* pData )
{
	LMTEntryAllocator.FreeObject( pData );
}

void* RPolygonClipper::SBTEntry::operator new( size_t )
{
	return SBTEntryAllocator.AllocateObject( );
}

void RPolygonClipper::SBTEntry::operator delete( void* pData )
{
	SBTEntryAllocator.FreeObject( pData );
}

#endif	// CUSTOMIZE_NEW


const YFloatType	kClipCoordEpsilon	= YFloatType(0.001);//0.015);
const YFloatType	kClipDeltaEpsilon	= YFloatType(0.001);//0.015);

inline BOOLEAN AreCoordsEqual(YFloatType a, YFloatType b)
	{
	return AreFloatsEqual(a,b,kClipCoordEpsilon);
	}

inline BOOLEAN AreCoordsEqual(int a, int b )
	{
	return a == b;
	}

inline BOOLEAN AreDeltasEqual(YFloatType a, YFloatType b)
	{
	return AreFloatsEqual(a,b,kClipDeltaEpsilon);
	}

inline BOOLEAN AreDeltasEqual(int a, int b)
	{
	return a == b;
	}

inline BOOLEAN IsCoordLT(YFloatType a, YFloatType b)
	{
	return AreFloatsLT(a,b,kClipCoordEpsilon);
	}

inline BOOLEAN IsCoordLT(int a, int b)
	{
	return a < b;
	}

template<class Point>
inline Point* _NextPoint( Point* ppt, Point* pPoints, int nPoints )
	{
	if( ppt == (pPoints+nPoints-1) )
		return pPoints;
	return ppt+1;
	}

template<class Point>
inline Point* _PrevPoint( Point* ppt, Point* pPoints, int nPoints )
	{
	if( ppt==pPoints )
		return pPoints+nPoints-1;
	return ppt-1;
	}

template<class Point>
inline Point* NextPoint( Point* ppt, Point* pPoints, int nPoints, Point* pptLast )
	{
	//
	// grab the next point.
	Point*	ppt1	= _NextPoint( ppt, pPoints, nPoints );
	pptLast;
	return ppt1;
	}

template<class Point>
inline Point* PrevPoint( Point* ppt, Point* pPoints, int nPoints, Point* pptLast )
	{
	//
	// grab the previous point.
	Point*	ppt1	= _PrevPoint( ppt, pPoints, nPoints );
	pptLast;
	return ppt1;
	}

template<class Point>
inline Point* AdvancePoint( Point* ppt, Point* pPoints, int nPoints, BOOLEAN fBackward, Point* pptLast )
	{
	return fBackward?
				PrevPoint(ppt,pPoints,nPoints,pptLast):
				NextPoint(ppt,pPoints,nPoints,pptLast);
	}

template<class Point>
inline BOOLEAN GetLeftDirection( Point* ppt, Point* pPoints, int nPoints )
	{
	Point*	pptPrev	= PrevPoint( ppt, pPoints, nPoints, ppt );
	Point*	pptNext	= NextPoint( ppt, pPoints, nPoints, ppt );
	//
	//	If the prev and next points are on different sides of the current X (ppt)
	//		or ON the same X as the current, then left is TRUE if next < current
	if ( ((pptPrev->m_x<=ppt->m_x) && (pptNext->m_x>=ppt->m_x)) ||
		  ((pptPrev->m_x>=ppt->m_x) && (pptNext->m_x<=ppt->m_x)) )
		return BOOLEAN(pptPrev->m_x < pptNext->m_x);
	//
	//	If the prev X lies on the same
	else
		{
		double	nextAngle	= double(pptNext->m_y-ppt->m_y)/double(pptNext->m_x-ppt->m_x);
		double	prevAngle	= double(pptPrev->m_y-ppt->m_y)/double(pptPrev->m_x-ppt->m_x);
		//
		//	If the prev and next are on the same side, check the angles 
		return BOOLEAN(prevAngle > nextAngle);
		}
	}

template<class Point>
Point* FindFirstMinimum( Point* pPoints, int nPoints )
	{
	Point*	pptEnd	= PrevPoint( pPoints, pPoints, nPoints, pPoints );
	pptEnd				= NextPoint( pptEnd, pPoints, nPoints, pptEnd );
	Point*	pptCur	= NextPoint( pptEnd, pPoints, nPoints, pptEnd );
	Point*	pptMin	= pptCur;
	while( pptCur != pptEnd )
		{
		pptCur	= NextPoint( pptCur, pPoints, nPoints, pptEnd );
		if( pptCur->m_y < pptMin->m_y )
			pptMin	= pptCur;
		else if( AreCoordsEqual(pptCur->m_y,pptMin->m_y) /*pptCur->m_y == pptMin->m_y*/ && pptCur->m_x < pptMin->m_x )
			pptMin	= pptCur;
		}
	return pptMin;
	}

template<class Point>
Point* FindNextMinimum( Point* pptMax, Point* pptEnd, Point* pPoints, int nPoints )
	{
	Point*	pptCur	= pptMax;
	Point*	pptMin	= pptCur;
	while( pptCur != pptEnd )
		{
		pptCur	= NextPoint( pptCur, pPoints, nPoints, pptEnd );
		if( pptCur->m_y < pptMin->m_y )
			pptMin	= pptCur;
		else if( AreCoordsEqual(pptCur->m_y,pptMin->m_y ) )//pptCur->m_y == pptMin->m_y )
			{
			if( pptCur->m_x < pptMin->m_x )	
				pptMin = pptCur;
			}
		else break;
		}
	return pptMin;
	}

template<class Point>
YPointCount CountPoints( Point* pPoints, int nPoints )
	{
	YPointCount	npts		= 1;
	Point*		ppt		= PrevPoint( pPoints, pPoints, nPoints, pPoints );
	ppt						= NextPoint( ppt, pPoints, nPoints, ppt );
	Point*		pptNext	= NextPoint( ppt, pPoints, nPoints, ppt );
	while( pptNext != ppt )
		{
		++npts;
		pptNext	= NextPoint( pptNext, pPoints, nPoints, ppt );
		}
	return npts;
	}

//
// local inlines
//

//
// @MFunc Get the first intersection table entry
// @RDesc pointer to the first entry
//
inline RPolygonClipper::RITEntry* RPolygonClipper::GetFirstITEntry()
	{
	return m_pIT;
	}

//
// @MFunc Get the next intersection table entry
// @RDesc pointer to the next entry in the table
//
inline RPolygonClipper::RITEntry* RPolygonClipper::GetNextITEntry(
			RPolygonClipper::RITEntry*	pent )	// @Parm pointer to the current entry
	{
	return pent? pent->m_pNext : NULL;
	}


//
// @MFunc Get the first entry in the ScanBeam table
// @RDesc pointer to the first entry
//
inline RPolygonClipper::SBTEntry* RPolygonClipper::GetFirstSBTEntry()
	{
	return m_pSBT;
	}

//
// @MFunc Get the next entry in the ScanBeam table
// @RDesc pointer to the next entry or NULL if there are no more
//
inline RPolygonClipper::SBTEntry* RPolygonClipper::GetNextSBTEntry(
			RPolygonClipper::SBTEntry*	pent )	// @Parm pointer to the current entry
	{
	return pent? pent->pNext : NULL;
	}


//
// @MFunc Link the given ActiveEdge entries
// @RDesc nothing
//
inline void RPolygonClipper::LinkAETEntries(
			RPolygonClipper::ActiveEdge*	pLeft,	// @Parm pointer to the left edge
			RPolygonClipper::ActiveEdge*	pRight )	// @Parm pointer to the right edge
	{
	if( pLeft ) pLeft->paeNext = pRight;
	else m_pAET = pRight;
	if( pRight ) pRight->paePrev = pLeft;
	}

//
// @MFunc Get the first entry in the ActiveEdge table
// @RDesc pointer to the first edge
//
inline RPolygonClipper::ActiveEdge* RPolygonClipper::GetFirstAETEntry()
	{
	return m_pAET;
	}

//
// @MFunc Get the next entry in the ActiveEdge table
// @RDesc pointer to the next edge
//
inline RPolygonClipper::ActiveEdge* RPolygonClipper::GetNextAETEntry(
			RPolygonClipper::ActiveEdge*	pEdge )	// @Parm pointer to the current edge
	{
	return pEdge? pEdge->paeNext : NULL;
	}

//
// @MFunc Get the previous entry in the ActiveEdge table
// @RDesc pointer to the previous edge
//
inline RPolygonClipper::ActiveEdge* RPolygonClipper::GetPrevAETEntry(
			RPolygonClipper::ActiveEdge*	pEdge )	// @Parm pointer to the current edge
	{
	return pEdge? pEdge->paePrev : NULL;
	}

//
// @MFunc Get the first polygon node
// @RDesc pointer to the first polygon node
//
inline RPolygonClipper::RPolyNode* RPolygonClipper::GetFirstPolyNode()
	{
	return m_pClipPolyNodes;
	}

//
// @MFunc Get the next polygon ndoe
// @RDesc pointer to the next polygon node
//
inline RPolygonClipper::RPolyNode* RPolygonClipper::GetNextPolyNode(
			RPolygonClipper::RPolyNode*	pNode )	// @Parm pointer to the current polygon node
	{
	return pNode? pNode->m_pNext : NULL;
	}

//
// @MFunc Swap the sides of the given edges
// @RDesc nothing
//
inline void RPolygonClipper::SwapEdgeSides(
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm pointer to the leading edge
			RPolygonClipper::ActiveEdge*	pEdge2 )	// @Parm pointer to the trailing edge
	{
	unsigned int	left1	= pEdge1->left;
	pEdge1->left			= pEdge2->left;
	pEdge2->left			= left1;
	}

//
// @MFunc Add the given point to the polygon contributed to by the given edge
// @RDesc nothing
//
inline void RPolygonClipper::AddPolyPoint(
			RPolygonClipper::ActiveEdge*	pEdge,	// @Parm pointer to the active edge containing the polygon to add a point to
			const RRealPoint&			pt )		// @Parm the point to add
	{
	if( pEdge->pPolygon )
		{
		if( pEdge == pEdge->pPolygon->m_pLeftEdge )
			pEdge->pPolygon->AddLeft( pt );
		else
			pEdge->pPolygon->AddRight( pt );
		}
	}


//
// Outlined implementation
//

#ifdef TPSDEBUG
void RClipPolygon::Dump() const
	{
	if( clipdebugindent<0 )
		return;
	STARTFUNC( ("RClipPolygon\n") );
	if( m_pptInt )
		{
		int					nPolys	= (m_pcnt? m_npoly : 1);
		const int*			pcnt		= (m_pcnt? m_pcnt : &m_npoly);
		const RIntPoint*	ppt		= m_pptInt;
		STARTFUNC( ("int polycnt[] = \n") );
		CONTINUEFUNC( ("{\n") );
		const int*	ptmp	= pcnt;
		int			np		= nPolys;
		while( np-- )
			CONTINUEFUNC( ("%d%s\n",*ptmp++,np?",":"") );
		ENDFUNC( ("}\n") );
		STARTFUNC( ("RIntPoint polypts[] = {\n") );
		CONTINUEFUNC( ("{\n") );
		while( nPolys-- )
			{
			int	npts	= *pcnt++;
			while( npts-- )
				{
				CONTINUEFUNC( ("RIntPoint(%d,%d)%s\n",ppt->m_x,ppt->m_y,(npts||nPolys)?",":"") );
				++ppt;
				}
			}
		ENDFUNC( ("}\n") );
		}
	else if( m_pptReal )
		{
		int					nPolys	= (m_pcnt? m_npoly : 1);
		const int*			pcnt		= (m_pcnt? m_pcnt : &m_npoly);
		const RRealPoint*	ppt		= m_pptReal;
		STARTFUNC( ("int polycnt[] = \n") );
		CONTINUEFUNC( ("{\n") );
		const int*	ptmp	= pcnt;
		int			np		= nPolys;
		while( np-- )
			CONTINUEFUNC( ("%d%s\n",*ptmp++,np?",":"") );
		ENDFUNC( ("}\n") );
		STARTFUNC( ("RRealPoint polypts[] = {\n") );
		CONTINUEFUNC( ("{\n") );
		while( nPolys-- )
			{
			int	npts	= *pcnt++;
			while( npts-- )
				{
				CONTINUEFUNC( ("RRealPoint(%ff,%ff)%s\n",ppt->m_x,ppt->m_y,(npts||nPolys)?",":"") );
				++ppt;
				}
			}
		ENDFUNC( ("}\n") );
		}
	LMTEntry*	pent = GetFirstEntry();
	while( pent )
		{
		CONTINUEFUNC( (" Local Minimum at (%.3f,%.3f) (%s)\n",pent->pt.m_x,pent->pt.m_y,pent->clip?"Clip":"Subject") );
		CONTINUEFUNC( ("      Left                  Right\n") );
		CONTINUEFUNC( (" -----------------    -----------------\n") );
		RPolyPoint*	pLeft		= pent->pLeft;
		RPolyPoint*	pRight	= pent->pRight;
		while( pLeft || pRight )
			{
			if( pLeft && pRight )
				CONTINUEFUNC( (" (%7.3f,%7.3f)    (%7.3f,%7.3f)\n", pLeft->m_x, pLeft->m_y, pRight->m_x, pRight->m_y) );
			else if( pLeft )
				CONTINUEFUNC( (" (%7.3f,%7.3f)\n", pLeft->m_x, pLeft->m_y) );
			else
				CONTINUEFUNC( ("                      (%7.3f,%7.3f)\n", pRight->m_x, pRight->m_y) );
			pLeft = (pLeft? pLeft->m_pNext : NULL);
			pRight = (pRight? pRight->m_pNext : NULL);
			}
		CONTINUEFUNC( ("\n") );
		pent = GetNextEntry( pent );
		}
	ENDFUNC( ("\n") );
	}
#endif

//
// @MFunc Build an edge for a local minima
// @RDesc pointer to the last point added to the edge
//
const RRealPoint* RClipPolygon::BuildEdge(
			RPolyPoint*			pPrevPolyPoint,	// @Parm pointer to the first poly point for the edge
			const RRealPoint*	pPrevSrcPoint,		// @Parm pointer to the src point corresponding to the poly point
			const RRealPoint*	pLastSrcPoint,		// @Parm pointer to the last src point to allow in the edge
			const RRealPoint*	pSrcPoints,			// @Parm pointer to the beginning of the src point array
			int					nPoints,				// @Parm the number of points in the src point array
			BOOLEAN				fBackward )			// @Parm TRUE if retrieving points from the src point array backwards else FALSE
{
	const RRealPoint*	pptNext	= AdvancePoint( pPrevSrcPoint, pSrcPoints, nPoints, fBackward, pLastSrcPoint );
	while( pPrevSrcPoint!=pLastSrcPoint && !IsCoordLT(pptNext->m_y,pPrevSrcPoint->m_y) )//pptNext->m_y >= pPrevSrcPoint->m_y )
		{
		//
		// add the point...
		pPrevPolyPoint->m_pNext	= new RPolyPoint( *pptNext );
		pPrevPolyPoint				= pPrevPolyPoint->m_pNext;
		pPrevSrcPoint				= pptNext;
		pptNext						= AdvancePoint( pPrevSrcPoint, pSrcPoints, nPoints, fBackward, pLastSrcPoint );
		}
	return pPrevSrcPoint;
}
const RIntPoint* RClipPolygon::BuildEdge(
			RPolyPoint*			pPrevPolyPoint,	// @Parm pointer to the first poly point for the edge
			const RIntPoint*	pPrevSrcPoint,		// @Parm pointer to the src point corresponding to the poly point
			const RIntPoint*	pLastSrcPoint,		// @Parm pointer to the last src point to allow in the edge
			const RIntPoint*	pSrcPoints,			// @Parm pointer to the beginning of the src point array
			int					nPoints,				// @Parm the number of points in the src point array
			BOOLEAN				fBackward )			// @Parm TRUE if retrieving points from the src point array backwards else FALSE
{
	const RIntPoint*	pptNext	= AdvancePoint( pPrevSrcPoint, pSrcPoints, nPoints, fBackward, pLastSrcPoint );
	while( pPrevSrcPoint!=pLastSrcPoint && pptNext->m_y >= pPrevSrcPoint->m_y )
		{
		//
		// add the point...
		pPrevPolyPoint->m_pNext	= new RPolyPoint( *pptNext );
		pPrevPolyPoint				= pPrevPolyPoint->m_pNext;
		pPrevSrcPoint				= pptNext;
		pptNext						= AdvancePoint( pPrevSrcPoint, pSrcPoints, nPoints, fBackward, pLastSrcPoint );
		}
	return pPrevSrcPoint;
}

//
// @MFunc Remove any redundant segments of the given edge
// @RDesc nothing
//
void RClipPolygon::ReduceEdge(
			RRealPoint*		pptPrev,		// @Parm pointer to the first point in the edge
			RPolyPoint**	ppptNext )	// @Parm pointer to the pointer to the next point in the edge
{
	RPolyPoint*	pptNext	= *ppptNext;
	while( pptNext->m_pNext )
		{
		RPolyPoint*	pptNNext		= pptNext->m_pNext;
		BOOLEAN		fRedundant	= FALSE;
		//	Horizontal Line segments, middle point is redundant
		if( AreCoordsEqual(pptPrev->m_y,pptNext->m_y) && AreCoordsEqual(pptNNext->m_y,pptNext->m_y) ) //pptPrev->m_y==pptNext->m_y && pptNNext->m_y==pptNext->m_y )
			{
			fRedundant = TRUE;
			}
		//	If X is the same, see if line segment is vertical
		else if( AreCoordsEqual(pptPrev->m_x,pptNext->m_x) )//pptPrev->m_x==pptNext->m_x )
			{
			if( AreCoordsEqual(pptNNext->m_x,pptNext->m_x) || AreCoordsEqual(pptPrev->m_y,pptNext->m_y) )//pptNNext->m_x==pptNext->m_x || pptPrev->m_y==pptNext->m_y )
				fRedundant = TRUE;
			}
		//	If Points doubles back on themselves...
		else if( AreCoordsEqual(pptPrev->m_x,pptNNext->m_x) && AreCoordsEqual(pptPrev->m_y,pptNNext->m_y) )//pptPrev->m_x==pptNNext->m_x && pptPrev->m_y==pptNNext->m_y )
			{
			fRedundant = TRUE;
			}
		//	Check if they are on the same slopes (in which case the point is redundant)
		else if ( AreParallel( *pptPrev, *pptNext, *pptNNext ) )
			{
			fRedundant = TRUE;
			}
		if( fRedundant )
			{
			//
			// pptNext is unnecessary...
			delete pptNext;
			*ppptNext	= pptNNext;
			pptNext		= pptNNext;
			}
		else
			{
			//
			// pptNext is necessary, so advance...
			pptPrev	= pptNext;
			ppptNext	= &pptNext->m_pNext;
			pptNext	= *ppptNext;
			}
		}
	if( AreCoordsEqual(pptNext->m_x,pptPrev->m_x) && AreCoordsEqual(pptNext->m_y,pptPrev->m_y) )//pptNext->m_x==pptPrev->m_x && pptNext->m_y==pptPrev->m_y )
		{
		//
		// next is unnecessary...
		delete pptNext;
		*ppptNext	= NULL;
		}
}


//
// @MFunc Insert an entry into the local table
// @RDesc nothing
//
void RClipPolygon::InsertLocal(
			RClipPolygon::LMTEntry*&	pHead,	// @Parm reference to the pointer to the head of the LMT
			RClipPolygon::LMTEntry*		pEntry )	// @Parm pointer to the entry to insert
	{
	LMTEntry*	pPrev	= NULL;
	LMTEntry*	pNext	= pHead;
#ifdef TPSDEBUG
	pEntry->pPolygon	= this;
#endif

	while( pNext && pEntry->pt.m_y > pNext->pt.m_y )
		{
		pPrev	= pNext;
		pNext	= pNext->pNextLocal;
		}
	while( pNext && pEntry->pt.m_y==pNext->pt.m_y && pEntry->pt.m_x>pNext->pt.m_x )
		{
		pPrev	= pNext;
		pNext	= pNext->pNextLocal;
		}
	//
	// insert between pPrev and pNext...
	pEntry->pNextLocal				= pNext;
	pEntry->pNextGlobal				= pNext;
	if( pPrev )
		{
		pPrev->pNextLocal		= pEntry;
		pPrev->pNextGlobal	= pEntry;
		}
	else pHead = pEntry;
	}
//
// @MFunc Insert an entry into the global (clip) table
// @RDesc nothing
void RClipPolygon::InsertGlobal(
			RClipPolygon::LMTEntry*&	pHead,	// @Parm reference to the pointer to the head of the LMT
			RClipPolygon::LMTEntry*	pEntry )	// @Parm pointer to the entry to insert
	{
	LMTEntry*	pPrev	= NULL;
	LMTEntry*	pNext	= pHead;
	while( pNext && pEntry->pt.m_y > pNext->pt.m_y )
		{
		pPrev	= pNext;
		pNext	= pNext->pNextGlobal;
		}
	while( pNext && pEntry->pt.m_y==pNext->pt.m_y && pEntry->pt.m_x>pNext->pt.m_x )
		{
		pPrev	= pNext;
		pNext	= pNext->pNextGlobal;
		}
	//
	// insert between pPrev and pNext...
	pEntry->pNextGlobal					= pNext;
	if( pPrev ) pPrev->pNextGlobal	= pEntry;
	else pHead	= pEntry;
	}

//
// @MFunc Merge the given table into this one's clip table
// @RDesc nothing
//
void RClipPolygon::MergeGlobal(
			RClipPolygon::LMTEntry*	pOtherHead )	// @Parm pointer to the head of the list to merge into this one
	{
	while( pOtherHead )
		{
		LMTEntry*	pnext	= pOtherHead->pNextLocal;
		InsertGlobal( m_pLMTGlobal, pOtherHead );
		pOtherHead = pnext;
		}
	}

//
// @MFunc Merge the given table into this one's local table
// @RDesc nothing
//
void RClipPolygon::MergeLocal(
			RClipPolygon::LMTEntry*&	pOtherHead )	// @Parm reference to the pointer to the list to merge into this one
	{
	while( pOtherHead )
		{
		LMTEntry*	pnext	= pOtherHead->pNextLocal;
		InsertLocal( m_pLMTLocal, pOtherHead );
		pOtherHead = pnext;
		}
	//
	// the pointer passed in is now null, since we own the entries...
	}

//
// @MFunc Destroy a local minima edge
// @RDesc nothing
//
void RClipPolygon::DestroyEdge(
			RPolyPoint*&	pPoint )	// @Parm reference to the pointer to the first point in the edge to delete
	{
	while( pPoint )
		{
		RPolyPoint*	pPrev	= pPoint;
		pPoint				= pPoint->m_pNext;
		delete pPrev;
		}
	}

//
// @MFunc Destroy an LMT entry
// @RDesc nothing
//
void RClipPolygon::DestroyEntry(
			RClipPolygon::LMTEntry*& pLocalMin )	// @Parm reference to the pointer to the entry to destroy
	{
	DestroyEdge( pLocalMin->pLeft );
	DestroyEdge( pLocalMin->pRight );
	delete pLocalMin;
	pLocalMin	= NULL;
	}

//
// @MFunc Destroy the entire local table
// @RDesc nothing
//
void RClipPolygon::DestroyTable()
	{
	while( m_pLMTLocal )
		{
		LMTEntry*	pPrev	= m_pLMTLocal;
		m_pLMTLocal			= m_pLMTLocal->pNextLocal;
		DestroyEntry( pPrev );
		}
	}

//
// @MFunc Reinitialize the global(clip) pointers and assign the entry type
// @RDesc nothing
//
void RClipPolygon::Reinit(
			BOOLEAN	fClip )	// @Parm TRUE if this LMT is for a clip polygon, FALSE if for subject
	{
	LMTEntry*	plmt	= m_pLMTLocal;
	m_pLMTGlobal		= plmt;
	while( plmt )
		{
		plmt->clip			= (fClip? 1 : 0);
		plmt->pNextGlobal	= plmt->pNextLocal;
		plmt					= plmt->pNextLocal;
		}
	}

//
// @MFunc Reduce the given local table by removing invalid entries
// @RDesc pointer to the reduced table
//
RClipPolygon::LMTEntry* RClipPolygon::ReduceLocalTable(
			RClipPolygon::LMTEntry* pLocalTable )	// @Parm pointer to the local table to reduce
	{
	LMTEntry**	ppEntry	= &pLocalTable;
	while( *ppEntry )
		{
		LMTEntry*	pEntry	= *ppEntry;
		RPolyPoint*	pLeft		= pEntry->pLeft;
		RPolyPoint*	pRight	= pEntry->pRight;
		TpsAssert( pLeft, "There is a LMTEntry with a NULL left leg! How did this get here?" );
		TpsAssert( pRight, "There is a LMTEntry with a NULL right leg! How did this get here?" );
		if( *pLeft == *pRight )
			{
			//
			// Entry has an invalid start point, so let's determine what we
			// should do about it...
			if( pLeft->m_pNext && pRight->m_pNext )
				{
				//
				// both sides continue, so let's reset the start point and
				// trim the edges...
				pEntry->pt		= *pLeft;
				pEntry->pLeft	= pLeft->m_pNext;
				pEntry->pRight	= pRight->m_pNext;
				delete pLeft;
				delete pRight;
				//
				// Don't advance to the next entry until we're sure this one is valid...
				}
#if 0
			else if( (!pLeft && !pRight) || (pLeft && pRight && !pLeft->m_pNext && !pRight->m_pNext) )
				{
				//
				// neither side continues, so this entry is unnecessary...
				*ppEntry	= pEntry->pNextLocal;
				DestroyEntry( pEntry );
				//
				// Storing the pointer to the next effectively advances us...
				}
#endif
			else
				{
				//
				// One side continues, but the other one doesn't...
				RPolyPoint*	pptCont		= (pLeft->m_pNext)? pLeft : pRight;
				LMTEntry*	pAdjacent	= pLocalTable;
				RPolyPoint*	pptAppendTo;
				while( pAdjacent && pptCont )
					{
					pptAppendTo	= pAdjacent->pLeft;
					while( pptAppendTo->m_pNext )
						pptAppendTo = pptAppendTo->m_pNext;
					if( *pptAppendTo == *pptCont )
						break;
					pptAppendTo	= pAdjacent->pRight;
					while( pptAppendTo->m_pNext )
						pptAppendTo = pptAppendTo->m_pNext;
					if( *pptAppendTo == *pptCont )
						break;
					pAdjacent	= pAdjacent->pNextLocal;
					//
					//	Cannot compare against ourself...
					if ( pAdjacent == pEntry )
						pAdjacent = pAdjacent->pNextLocal;
					//
					//	If we did not find a match, advance to next point in pEntry and restart
					//	looking for a match
					if ( !pAdjacent )
						{
						pptCont		= pptCont->m_pNext;
						pAdjacent	= pLocalTable;
						}
					}
				TpsAssert( (pAdjacent != NULL) || (pptCont == NULL), "Couldn't find adjacent entry!" );
				pptAppendTo->m_pNext	= pptCont->m_pNext;
				pptCont->m_pNext		= NULL;
				*ppEntry	= pEntry->pNextLocal;
				DestroyEntry( pEntry );
				//
				// Reset to the beginning of the table...
				ppEntry	= &pLocalTable;
				}// one side continues, the other doesn't
			}// both sides start at the same point
		else
			{
			//
			// the sides start at different points...
			ppEntry = &pEntry->pNextLocal;
			}
		}// for all entries...
	return pLocalTable;
	}

//
// @MFunc Create a simple table (single polygon)
// @RDesc pointer to the new table
//
RClipPolygon::LMTEntry* RClipPolygon::CreateTable(
			const RRealPoint*	pPoints,		// @Parm pointer to the REAL points making up the polygon
			int					nPoints )	// @Parm the number of points in the polygon
	{
	LMTEntry*		pLocalMinTable	= NULL;
	if( pPoints!=NULL && nPoints>0 )
		{
		STARTFUNC( ("CreateTable\n") );
		int	nUnique	= CountPoints( pPoints, nPoints );
		if( nUnique > 2 )
			{
			const RRealPoint*	pptMin	= FindFirstMinimum( pPoints, nPoints );
			BOOLEAN				fLftBack	= GetLeftDirection( pptMin, pPoints, nPoints );
			BOOLEAN				fRgtBack	= BOOLEAN( !fLftBack );
			const RRealPoint*	pptLft	= AdvancePoint( pptMin, pPoints, nPoints, fLftBack, pptMin );
			const RRealPoint*	pptRgt	= AdvancePoint( pptMin, pPoints, nPoints, fRgtBack, pptMin );
			//
			// Create the initial LMT entry...
			LMTEntry*	pLocalMin	= new LMTEntry;
			pLocalMin->pt				= *pptMin;
			pLocalMin->pLeft			= new RPolyPoint( *pptLft );
			pLocalMin->pRight			= new RPolyPoint( *pptRgt );
			//
			// Build the edges for the new entry...
			pptLft	= BuildEdge( pLocalMin->pLeft, pptLft, pptRgt, pPoints, nPoints, fLftBack );
			ReduceEdge( &pLocalMin->pt, &pLocalMin->pLeft );
			pptRgt	= BuildEdge( pLocalMin->pRight, pptRgt, pptLft, pPoints, nPoints, fRgtBack );
			ReduceEdge( &pLocalMin->pt, &pLocalMin->pRight );
			//
			// insert it if it's valid...
			if( pLocalMin->pRight && pLocalMin->pLeft )
				{
//				if( (*(pLocalMin->pRight)) != (*(pLocalMin->pLeft)) )
					{
					//
					// Insert it into the table...
					InsertLocal( pLocalMinTable, pLocalMin );
					}
				}
			//
			// Setup the start and end points for creating the remaining entries...
			const RRealPoint*	pptStart	= (fLftBack? pptRgt : pptLft);
			const RRealPoint*	pptEnd	= (fLftBack? pptLft : pptRgt);

			//
			// Finish it off...
			while( pptStart != pptEnd )
				{
				BOOLEAN		fInserted = FALSE;
				//
				// Find the next local minimum (searches forward)...
				pptMin		= FindNextMinimum( pptStart, pptEnd, pPoints, nPoints );
				//
				// Create an entry for it...
				pLocalMin		= new LMTEntry;
				pLocalMin->pt	= *pptMin;
				//
				// Determine which direction the edges follow
				if( GetLeftDirection(pptMin,pPoints,nPoints) )
					{
					//
					// left edge proceeds backward through the points (toward the previous right)
					pptLft				= PrevPoint( pptMin, pPoints, nPoints, pptStart );
					pLocalMin->pLeft	= new RPolyPoint( *pptLft );
					pptLft				= BuildEdge( pLocalMin->pLeft, pptLft, pptStart, pPoints, nPoints, TRUE );
					ReduceEdge( &pLocalMin->pt, &pLocalMin->pLeft );
					//
					// right edge proceeds forward through the points (toward the current end point)
					pptRgt				= NextPoint( pptMin, pPoints, nPoints, pptEnd );
					pLocalMin->pRight	= new RPolyPoint( *pptRgt );
					pptRgt				= BuildEdge( pLocalMin->pRight, pptRgt, pptEnd, pPoints, nPoints, FALSE );
					ReduceEdge( &pLocalMin->pt, &pLocalMin->pRight );
					//
					// the new start point is the last right point...
					pptStart	= pptRgt;
					}
				else
					{
					//
					// left edge proceeds forward through the points (toward the current end point)
					pptLft				= NextPoint( pptMin, pPoints, nPoints, pptEnd );
					pLocalMin->pLeft	= new RPolyPoint( *pptLft );
					pptLft				= BuildEdge( pLocalMin->pLeft, pptLft, pptEnd, pPoints, nPoints, FALSE );
					ReduceEdge( &pLocalMin->pt, &pLocalMin->pLeft );
					//
					// right edge proceeds backward through the points (toward the current start point)
					pptRgt				= PrevPoint( pptMin, pPoints, nPoints, pptStart );
					pLocalMin->pRight	= new RPolyPoint( *pptRgt );
					pptRgt				= BuildEdge( pLocalMin->pRight, pptRgt, pptStart, pPoints, nPoints, TRUE );
					ReduceEdge( &pLocalMin->pt, &pLocalMin->pRight );
					//
					// the new start point is the last left point...
					pptStart	= pptLft;
					}
				//
				// insert it if it's valid...
				if( pLocalMin->pRight && pLocalMin->pLeft )
					{
					//
					// Insert it into the table...
					//
					//		If this is not a valid leg, it will be fixed in the
					//		ReduceLocalTable below
					InsertLocal( pLocalMinTable, pLocalMin );
					fInserted = TRUE;
					}

				//	Destroy the LMTEntry if not inserted
				if ( !fInserted && pLocalMin )
					{
					DestroyEntry( pLocalMin );
					}
				}
			}// if there are enough points...
		ENDFUNC( ("\n") );
		}
	return ReduceLocalTable( pLocalMinTable );
	}

RClipPolygon::LMTEntry* RClipPolygon::CreateTable(
			const RIntPoint*	pPoints,		// @Parm pointer to the INTEGER points making up the polygon
			int					nPoints )	// @Parm the number of points in the polygon
	{
	LMTEntry*		pLocalMinTable	= NULL;
	if( pPoints!=NULL && nPoints>0 )
		{
		STARTFUNC( ("CreateTable\n") );
		int	nUnique	= CountPoints( pPoints, nPoints );
		if( nUnique > 2 )
			{
			const RIntPoint*	pptMin	= FindFirstMinimum( pPoints, nPoints );
			BOOLEAN				fLftBack	= GetLeftDirection( pptMin, pPoints, nPoints );
			BOOLEAN				fRgtBack	= BOOLEAN( !fLftBack );
			const RIntPoint*	pptLft	= AdvancePoint( pptMin, pPoints, nPoints, fLftBack, pptMin );
			const RIntPoint*	pptRgt	= AdvancePoint( pptMin, pPoints, nPoints, fRgtBack, pptMin );
			//
			// Create the initial LMT entry...
			LMTEntry*	pLocalMin	= new LMTEntry;
			pLocalMin->pt				= *pptMin;
			pLocalMin->pLeft			= new RPolyPoint( *pptLft );
			pLocalMin->pRight			= new RPolyPoint( *pptRgt );
			//
			// Build the edges for it...
			pptLft	= BuildEdge( pLocalMin->pLeft, pptLft, pptRgt, pPoints, nPoints, fLftBack );
			ReduceEdge( &pLocalMin->pt, &pLocalMin->pLeft );

			pptRgt	= BuildEdge( pLocalMin->pRight, pptRgt, pptLft, pPoints, nPoints, fRgtBack );
			ReduceEdge( &pLocalMin->pt, &pLocalMin->pRight );
			//
			// insert it if it's valid...
			if( pLocalMin->pRight && pLocalMin->pLeft )
				{
//				if( (*(pLocalMin->pRight)) != (*(pLocalMin->pLeft)) )
					{
					//
					// Insert it into the table...
					InsertLocal( pLocalMinTable, pLocalMin );
					}
				}
			//
			// Setup the start and end points for creating the remaining entries...
			const RIntPoint*	pptStart	= (fLftBack? pptRgt : pptLft);
			const RIntPoint*	pptEnd	= (fLftBack? pptLft : pptRgt);

			//
			// Finish it off...
			while( pptStart != pptEnd )
				{
				BOOLEAN		fInserted = FALSE;
				//
				// Find the next local minimum (searches forward)...
				pptMin		= FindNextMinimum( pptStart, pptEnd, pPoints, nPoints );
				//
				// Create an entry for it...
				pLocalMin		= new LMTEntry;
				pLocalMin->pt	= *pptMin;
				//
				// Determine which direction the edges follow
				if( GetLeftDirection(pptMin,pPoints,nPoints) )
					{
					//
					// left edge proceeds backward through the points (toward the previous right)
					pptLft				= PrevPoint( pptMin, pPoints, nPoints, pptStart );
					pLocalMin->pLeft	= new RPolyPoint( *pptLft );
					pptLft				= BuildEdge( pLocalMin->pLeft, pptLft, pptStart, pPoints, nPoints, TRUE );
					ReduceEdge( &pLocalMin->pt, &pLocalMin->pLeft );
					//
					// right edge proceeds forward through the points (toward the current end point)
					pptRgt				= NextPoint( pptMin, pPoints, nPoints, pptEnd );
					pLocalMin->pRight	= new RPolyPoint( *pptRgt );
					pptRgt				= BuildEdge( pLocalMin->pRight, pptRgt, pptEnd, pPoints, nPoints, FALSE );
					ReduceEdge( &pLocalMin->pt, &pLocalMin->pRight );
					//
					// the new start point is the last right point...
					pptStart	= pptRgt;
					}
				else
					{
					//
					// left edge proceeds forward through the points (toward the current end point)
					pptLft				= NextPoint( pptMin, pPoints, nPoints, pptEnd );
					pLocalMin->pLeft	= new RPolyPoint( *pptLft );
					pptLft				= BuildEdge( pLocalMin->pLeft, pptLft, pptEnd, pPoints, nPoints, FALSE );
					ReduceEdge( &pLocalMin->pt, &pLocalMin->pLeft );
					//
					// right edge proceeds backward through the points (toward the current start point)
					pptRgt				= PrevPoint( pptMin, pPoints, nPoints, pptStart );
					pLocalMin->pRight	= new RPolyPoint( *pptRgt );
					pptRgt				= BuildEdge( pLocalMin->pRight, pptRgt, pptStart, pPoints, nPoints, TRUE );
					ReduceEdge( &pLocalMin->pt, &pLocalMin->pRight );
					//
					// the new start point is the last left point...
					pptStart	= pptLft;
					}
				//
				// insert it if it's valid...
				if( pLocalMin->pRight && pLocalMin->pLeft )
					{
					//
					// Insert it into the table...
					//
					//		If this is not a valid leg, it will be fixed in the
					//		ReduceLocalTable below
					InsertLocal( pLocalMinTable, pLocalMin );
					fInserted = TRUE;
					}

				//	Destroy the LMTEntry if not inserted
				if ( !fInserted && pLocalMin )
					{
					DestroyEntry( pLocalMin );
					}
				}
			}// if there are enough points...
		ENDFUNC( ("\n") );
		}
	return ReduceLocalTable( pLocalMinTable );
	}

//
// @MFunc Create a complex table (poly polygon)
// @RDesc nothing
//
void RClipPolygon::CreatePolyTable(
			const RRealPoint*	pPoints,		// @Parm pointer to the REAL points making up the polygon
			const int*			pnCounts,	// @Parm pointer to the array of point counts
			int					nPolys )		// @Parm the number of polygons in the polypolygon
	{
	if( pPoints != NULL && pnCounts!=NULL && nPolys>0 )
		{
		while( nPolys-- )
			{
			LMTEntry*	pNew	= CreateTable( pPoints, *pnCounts );
			MergeLocal( pNew );
			pPoints += *pnCounts++;
			}
		}
	}
void RClipPolygon::CreatePolyTable(
			const RIntPoint*	pPoints,		// @Parm pointer to the INTEGER points making up the polygon
			const int*			pnCounts,	// @Parm pointer to the array of point counts
			int					nPolys )		// @Parm the number of polygons in the polypolygon

	{
	if( pPoints != NULL && pnCounts!=NULL && nPolys>0 )
		{
		while( nPolys-- )
			{
			LMTEntry*	pNew	= CreateTable( pPoints, *pnCounts );
			MergeLocal( pNew );
			pPoints += *pnCounts++;
			}
		}
	}


//
// @MFunc Construct an RClipPolygon
// @Syntax RClipPolygon()
// @Syntax RClipPolygon( RRealPoint* pPoints, int nPoints )
// @Syntax RClipPolygon( RIntPoint* pPoints, int nPoints )
// @Syntax RClipPolygon( RRealPoint* pPoints, int* pnPoints, int nPolys )
// @Syntax RClipPolygon( RIntPoint* pPoints, int* pnPoints, int nPolys )
// @Parm RRealPoint* | pPoints | pointer to the list of points making up the (poly)polygon
// @Parm RIntPoint* | pPoints | pointer to the list of points making up the (poly)polygon
// @Parm int* | pnPoints | pointer to the array of point counts for the polygons in the polypolygon
// @Parm int | nPoints | the number of points in the polygon
// @Parm int | nPolys | the number of polygons in the polypolygon
// @RDesc nothing
//
RClipPolygon::RClipPolygon() :
#ifdef TPSDEBUG
	m_pptInt( NULL ),
	m_pptReal( NULL ),
	m_pcnt( NULL ),
	m_npoly( 0 ),
#endif
	m_pLMTLocal( NULL ),
	m_pLMTGlobal( NULL )
	{
	}
RClipPolygon::RClipPolygon( const RRealPoint* pPoints, int nPoints ) :
#ifdef TPSDEBUG
	m_pptInt( NULL ),
	m_pptReal( pPoints ),
	m_pcnt( NULL ),
	m_npoly( nPoints ),
#endif
	m_pLMTLocal( NULL ),
	m_pLMTGlobal( NULL )
	{
	m_pLMTLocal		= CreateTable( pPoints, nPoints );
	m_pLMTGlobal	= m_pLMTLocal;
	}
RClipPolygon::RClipPolygon( const RIntPoint* pPoints, int nPoints ) :
#ifdef TPSDEBUG
	m_pptInt( pPoints ),
	m_pptReal( NULL ),
	m_pcnt( NULL ),
	m_npoly( nPoints ),
#endif
	m_pLMTLocal( NULL ),
	m_pLMTGlobal( NULL )
	{
	m_pLMTLocal		= CreateTable( pPoints, nPoints );
	m_pLMTGlobal	= m_pLMTLocal;
	}
RClipPolygon::RClipPolygon( const RRealPoint* pPoints, const int* pnPoints, int nPolys ) :
#ifdef TPSDEBUG
	m_pptInt( NULL ),
	m_pptReal( pPoints ),
	m_pcnt( pnPoints ),
	m_npoly( nPolys ),
#endif
	m_pLMTLocal( NULL ),
	m_pLMTGlobal( NULL )
	{
	CreatePolyTable( pPoints, pnPoints, nPolys );
	m_pLMTGlobal	= m_pLMTLocal;
	}
RClipPolygon::RClipPolygon( const RIntPoint* pPoints, const int* pnPoints, int nPolys ) :
#ifdef TPSDEBUG
	m_pptInt( pPoints ),
	m_pptReal( NULL ),
	m_pcnt( pnPoints ),
	m_npoly( nPolys ),
#endif
	m_pLMTLocal( NULL ),
	m_pLMTGlobal( NULL )
	{
	CreatePolyTable( pPoints, pnPoints, nPolys );
	m_pLMTGlobal	= m_pLMTLocal;
	}

//
// @MFunc Destruct an RClipPolygon
// @RDesc nothing
//
RClipPolygon::~RClipPolygon()
	{
	DestroyTable();
	}

//
// @MFunc Append the given polypolygon to this clippolygon
// @RDesc nothing
//
void RClipPolygon::AppendPolyPolygon( const RIntPoint* pPoints, const int* pnCounts, int nPolys )
{
	if( pPoints != NULL && pnCounts!=NULL && nPolys>0 )
		{
		while( nPolys-- )
			{
			LMTEntry*	pNew	= CreateTable( pPoints, *pnCounts );
			MergeLocal( pNew );
			pPoints += *pnCounts++;
			}
		}
}
void RClipPolygon::AppendPolyPolygon( const RRealPoint* pPoints, const int* pnCounts, int nPolys )
{
	if( pPoints != NULL && pnCounts!=NULL && nPolys>0 )
		{
		while( nPolys-- )
			{
			LMTEntry*	pNew	= CreateTable( pPoints, *pnCounts );
			MergeLocal( pNew );
			pPoints += *pnCounts++;
			}
		}
}

//
// @MFunc Append the given simple polygon to this clippolygon
// @RDesc nothing
//
void RClipPolygon::AppendPolygon( const RIntPoint* pPoints, int nPoints )
{
	AppendPolyPolygon( pPoints, &nPoints, 1 );
}
void RClipPolygon::AppendPolygon( const RRealPoint* pPoints, int nPoints )
{
	AppendPolyPolygon( pPoints, &nPoints, 1 );
}


//
// @MFunc Extract the polygons from this clippolygon
// @RDesc the number of polygons in the polypolygon
//
int RClipPolygon::ExtractPolygons(
			int**				ppCounts,		// @Parm pointer to storage for the polygon counts
			RRealPoint**	ppPoints )		// @Parm pointer to storage for the polygon points
	{
	RPolygonClipper	clipper( *this );
	int	nPolygons;
	int	nTotalPts;
	clipper.GeneratePolygons( RPolygonClipper::kLogicalOr, nPolygons, nTotalPts );
	*ppCounts	= new int[ nPolygons ];
	*ppPoints	= new RRealPoint[ nTotalPts ];
	return clipper.StorePolygons( *ppPoints, *ppCounts );
	}

int RClipPolygon::ExtractPolygons(
			int**			ppCounts,			// @Parm pointer to storage for the polygon counts
			RIntPoint**	ppPoints )		// @Parm pointer to storage for the polygon points
	{
	RPolygonClipper	clipper( *this );
	int	nPolygons;
	int	nTotalPts;
	clipper.GeneratePolygons( RPolygonClipper::kLogicalOr, nPolygons, nTotalPts );
	*ppCounts	= new int[ nPolygons ];
	*ppPoints	= new RIntPoint[ nTotalPts ];
	return clipper.StorePolygons( *ppPoints, *ppCounts );
	}

//
// @MFunc Empty this clippolygon
// @RDesc nothing
//
void RClipPolygon::Empty()
	{
	DestroyTable();
	#ifdef TPSDEBUG
	m_pptInt		= NULL;
	m_pptReal	= NULL;
	m_pcnt		= NULL;
	m_npoly		= 0;
	#endif
	m_pLMTGlobal	= NULL;
	}

//
// @MFunc OR the given polygon onto this one
// @RDesc const reference to this clippolygon
//
DWORD	dwTotalOld	= 0;
DWORD	dwTotalNew	= 0;
const RClipPolygon& RClipPolygon::operator|=( const RClipPolygon& rhs )
{
	int					nPolys;
	int					nPoints;
	RPolygonClipper	clipper( const_cast<RClipPolygon&>(rhs), *this );
	clipper.GeneratePolygons( RPolygonClipper::kLogicalOr, nPolys, nPoints );
	clipper.StorePolygons( *this );
	return *this;
}

//
// @MFunc Subtract the given polygon from this one
// @RDesc const reference to this clippolygon
//
const RClipPolygon& RClipPolygon::operator-=( const RClipPolygon& rhs )
{
	int					nPolys;
	int					nPoints;
	RPolygonClipper	clipper( const_cast<RClipPolygon&>(rhs), *this );
	clipper.GeneratePolygons( RPolygonClipper::kLogicalDiff, nPolys, nPoints );
	clipper.StorePolygons( *this );
	return *this;
}

//
// @MFunc XOR the given polygon onto this one
// @RDesc const reference to this clippolygon
//
const RClipPolygon& RClipPolygon::operator^=( const RClipPolygon& rhs )
{
	int					nPolys;
	int					nPoints;
	RPolygonClipper	clipper( const_cast<RClipPolygon&>(rhs), *this );
	clipper.GeneratePolygons( RPolygonClipper::kLogicalXor, nPolys, nPoints );
	clipper.StorePolygons( *this );
	return *this;
}

//
// @MFunc AND this polygon with the given one
// @RDesc const reference to this clippolygon
//
const RClipPolygon& RClipPolygon::operator&=( const RClipPolygon& rhs )
{
	int	nPolys;
	int	nPoints;
	RPolygonClipper	clipper( const_cast<RClipPolygon&>(rhs), *this );
	clipper.GeneratePolygons( RPolygonClipper::kLogicalAnd, nPolys, nPoints );
	clipper.StorePolygons( *this );
	return *this;
}

//
// @MFunc Get the first global entry
// @RDesc pointer to the first global entry
//
RClipPolygon::LMTEntry* RClipPolygon::GetFirstEntry() const
	{
	return m_pLMTGlobal;
	}
//
// @MFunc Get the next global entry
// @RDesc pointer to the next global entry
//
RClipPolygon::LMTEntry* RClipPolygon::GetNextEntry(
			LMTEntry*	pCur )	// @Parm pointer to the current entry
			const
	{
	return pCur? pCur->pNextGlobal : NULL;
	}


RPolygonClipper::IntersectionHandler	RPolygonClipper::g_pfnHandleIntAND[] =
	{
	RPolygonClipper::IntersectionError,				// LC1 LC2 --
	RPolygonClipper::ProcessLikeEdgeIntersection,	// LC1 RC2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// LC1 LS2 *
	RPolygonClipper::ProcessLocalMaxIntersection,	// LC1 RS2 *
	RPolygonClipper::ProcessLikeEdgeIntersection,	// RC1 LC2 *
	RPolygonClipper::IntersectionError,				// RC1 RC2 --
	RPolygonClipper::ProcessLocalMinIntersection,	// RC1 LS2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// RC1 RS2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// LS1 LC2 *
	RPolygonClipper::ProcessLocalMaxIntersection,	// LS1 RC2 *
	RPolygonClipper::IntersectionError,				// LS1 LS2 --
	RPolygonClipper::ProcessLikeEdgeIntersection,	// LS1 RS2 *
	RPolygonClipper::ProcessLocalMinIntersection,	// RS1 LC2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// RS1 RC2 *
	RPolygonClipper::ProcessLikeEdgeIntersection,	// RS1 LS2 *
	RPolygonClipper::IntersectionError					// RS1 RS2 --
	};
RPolygonClipper::IntersectionHandler	RPolygonClipper::g_pfnHandleIntOR[] =
	{
	RPolygonClipper::IntersectionError,				// LC1 LC2 --
	RPolygonClipper::ProcessLikeEdgeIntersection,	// LC1 RC2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// LC1 LS2 *
	RPolygonClipper::ProcessLocalMinIntersection,	// LC1 RS2 *
	RPolygonClipper::ProcessLikeEdgeIntersection,	// RC1 LC2 *
	RPolygonClipper::IntersectionError,				// RC1 RC2 --
	RPolygonClipper::ProcessLocalMaxIntersection,	// RC1 LS2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// RC1 RS2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// LS1 LC2 *
	RPolygonClipper::ProcessLocalMinIntersection,	// LS1 RC2 *
	RPolygonClipper::IntersectionError,				// LS1 LS2 --
	RPolygonClipper::ProcessLikeEdgeIntersection,	// LS1 RS2 *
	RPolygonClipper::ProcessLocalMaxIntersection,	// RS1 LC2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// RS1 RC2 *
	RPolygonClipper::ProcessLikeEdgeIntersection,	// RS1 LS2 *
	RPolygonClipper::IntersectionError					// RS1 RS2 --
	};
RPolygonClipper::IntersectionHandler	RPolygonClipper::g_pfnHandleIntDIFF[] =
	{
	RPolygonClipper::IntersectionError,				// LC1 LC2 --
	RPolygonClipper::ProcessLikeEdgeIntersection,	// LC1 RC2 
	RPolygonClipper::ProcessLocalMinIntersection,	// LC1 LS2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// LC1 RS2 *
	RPolygonClipper::ProcessLikeEdgeIntersection,	// RC1 LC2 
	RPolygonClipper::IntersectionError,				// RC1 RC2 --
	RPolygonClipper::ProcessLikeSideIntersection,	// RC1 LS2 *
	RPolygonClipper::ProcessLocalMaxIntersection,	// RC1 RS2 *
	RPolygonClipper::ProcessLocalMaxIntersection,	// LS1 LC2 *
	RPolygonClipper::ProcessLikeSideIntersection,	// LS1 RC2 *
	RPolygonClipper::IntersectionError,				// LS1 LS2 --
	RPolygonClipper::ProcessLikeEdgeIntersection,	// LS1 RS2 
	RPolygonClipper::ProcessLikeSideIntersection,	// RS1 LC2 *
	RPolygonClipper::ProcessLocalMinIntersection,	// RS1 RC2 *
	RPolygonClipper::ProcessLikeEdgeIntersection,	// RS1 LS2 
	RPolygonClipper::IntersectionError					// RS1 RS2 --
	};
RPolygonClipper::IntersectionHandler	RPolygonClipper::g_pfnHandleIntXOR[] =
	{
	RPolygonClipper::IntersectionError,					// LC1 LC2 --
	RPolygonClipper::ProcessLikeEdgeIntersection,		// LC1 RC2 *
	RPolygonClipper::ProcessLikeSideIntersection,		// LC1 LS2 *
	RPolygonClipper::ProcessIntermediateIntersection,	// LC1 RS2 *
	RPolygonClipper::ProcessLikeEdgeIntersection,		// RC1 LC2 *
	RPolygonClipper::IntersectionError,					// RC1 RC2 --
	RPolygonClipper::ProcessIntermediateIntersection,	// RC1 LS2 *
	RPolygonClipper::ProcessLikeSideIntersection,		// RC1 RS2 *
	RPolygonClipper::ProcessLikeSideIntersection,		// LS1 LC2 *
	RPolygonClipper::ProcessIntermediateIntersection,	// LS1 RC2 *
	RPolygonClipper::IntersectionError,					// LS1 LS2 --
	RPolygonClipper::ProcessLikeEdgeIntersection,		// LS1 RS2 *
	RPolygonClipper::ProcessIntermediateIntersection,	// RS1 LC2 *
	RPolygonClipper::ProcessLikeSideIntersection,		// RS1 RC2 *
	RPolygonClipper::ProcessLikeEdgeIntersection,		// RS1 LS2 *
	RPolygonClipper::IntersectionError						// RS1 RS2 --
	};

#ifdef TPSDEBUG
int	RPolygonClipper::RPolyNode::m_gnPolys	= 0;
#endif


//
// @MFunc Construct an RPolyNode starting at the given point
// @RDesc nothing
//
RPolygonClipper::RPolyNode::RPolyNode(
			RPolygonClipper::ActiveEdge*	pLeftEdge,	// @Parm pointer to the edge contributing to the left
			RPolygonClipper::ActiveEdge*	pRightEdge,	// @Parm pointer to the edge contributing to the right
			const RRealPoint&			pt ) :		// @Parm the point at which the polygon node starts
	m_pLeft( NULL ),
	m_pRight( NULL ),
	m_pPrevRight( NULL ),
	m_nPoints( 0 ),
	m_pLeftEdge( pLeftEdge ),
	m_pRightEdge( pRightEdge ),
	m_pNodeLeft( NULL ),
	m_pNodeRight( NULL ),
	m_pNext( NULL )
	{
	#ifdef TPSDEBUG
	m_Name.Format("%c", 'A'+m_gnPolys );
	++m_gnPolys;
	#endif
	AddLeft( pt );
	}

//
// @MFunc Destruct this RPolyNode
// @RDesc nothing
//
RPolygonClipper::RPolyNode::~RPolyNode()
	{
	#ifdef TPSDEBUG
	--m_gnPolys;
	#endif
	while( m_pLeft )
		{
		RPolyPoint*	pPrev	= m_pLeft;
		m_pLeft				= m_pLeft->m_pNext;
		delete pPrev;
		}
	}

//
// @MFunc return a pointer to the leftmost node
// @RDesc pointer to the leftmost node
//
inline RPolygonClipper::RPolyNode* RPolygonClipper::RPolyNode::LeftmostNode()
	{
	RPolyNode*	pNode	= this;
	while( pNode->m_pNodeLeft )
		pNode = pNode->m_pNodeLeft;
	return pNode;
	}

//
// @MFunc return a pointer to the rightmost node
// @RDesc pointer to the rightmost node
//
inline RPolygonClipper::RPolyNode* RPolygonClipper::RPolyNode::RightmostNode()
	{
	RPolyNode*	pNode	= this;
	while( pNode->m_pNodeRight )
		pNode = pNode->m_pNodeRight;
	return pNode;
	}

//
// @MFunc Remove the leftmost node if it's empty
// @RDesc nothing
//
void RPolygonClipper::RPolyNode::TrimLeftnode( RPolygonClipper::RPolyNode*& pNode )
	{
	RPolyNode*	pLeft	= pNode->LeftmostNode();
	if( !pLeft->m_pLeft )
		{
		if( pLeft->m_pNodeRight )
			{
			if( pLeft == pNode )
				{
				pNode									= pLeft->m_pNodeRight;
				pNode->m_pLeftEdge				= pLeft->m_pLeftEdge;
				pNode->m_pRightEdge				= pLeft->m_pRightEdge;
				pNode->m_pLeftEdge->pPolygon	= pNode;
				pNode->m_pRightEdge->pPolygon	= pNode;
				pNode->m_nPoints					= pLeft->m_nPoints;
				pLeft->m_nPoints					= 0;
				pLeft->m_pLeftEdge				= NULL;
				pLeft->m_pRightEdge				= NULL;
	#ifdef TPSDEBUG
				pNode->m_Name	= pLeft->m_Name;
				pLeft->m_Name	= "";
	#endif
				}
			pLeft->m_pNodeRight->m_pNodeLeft	= NULL;
			pLeft->m_pNodeRight					= NULL;
			}
		}
	}
//
// @MFunc Remove the rightmost node if it's empty
// @RDesc nothing
//
void RPolygonClipper::RPolyNode::TrimRightnode( RPolygonClipper::RPolyNode*& pNode )
	{
	RPolyNode*	pRight	= pNode->RightmostNode();
	if( !pRight->m_pLeft )
		{
		if( pRight->m_pNodeLeft )
			{
			if( pRight == pNode )
				{
				pNode									= pRight->m_pNodeLeft;
				pNode->m_pLeftEdge				= pRight->m_pLeftEdge;
				pNode->m_pRightEdge				= pRight->m_pRightEdge;
				pNode->m_pLeftEdge->pPolygon	= pNode;
				pNode->m_pRightEdge->pPolygon	= pNode;
				pNode->m_nPoints					= pRight->m_nPoints;
				pRight->m_nPoints					= 0;
				pRight->m_pLeftEdge				= NULL;
				pRight->m_pRightEdge				= NULL;
	#ifdef TPSDEBUG
				pNode->m_Name	= pRight->m_Name;
				pRight->m_Name	= "";
	#endif
				}
			pRight->m_pNodeLeft->m_pNodeRight	= NULL;
			pRight->m_pNodeLeft						= NULL;
			}
		}
	}

//
// @MFunc Remove the leftmost point
// @RDesc nothing
//
void RPolygonClipper::RPolyNode::RemoveLeft()
	{
	RPolyNode*	pLeft	= LeftmostNode();
	if( pLeft->m_pLeft )
		{
		RPolyPoint*	pNext	= pLeft->m_pLeft->m_pNext;
		if( pLeft->m_pPrevRight==pLeft->m_pLeft )
			{
			//
			// there were only two points in this node, so now there will only
			// be one.  There can be no previous right!
			pLeft->m_pPrevRight	= NULL;
			}
		delete pLeft->m_pLeft;
		pLeft->m_pLeft	= pNext;
		if( !pNext )
			pLeft->m_pRight = NULL;
		--m_nPoints;
		}
	}


//
// @MFunc Add the given point to the left side of this node
// @RDesc TRUE if the point was added, FALSE if it's a duplicate
//
BOOLEAN RPolygonClipper::RPolyNode::AddLeftPoint(
			const RRealPoint&	pt )	// @Parm the point to add to the left
	{
	if( !m_pLeft )
		{
		m_pLeft				= new RPolyPoint(pt);
		m_pLeft->m_pNext	= NULL;
		m_pRight				= m_pLeft;
		m_pPrevRight		= NULL;
		return TRUE;
		}
	//
	// there is already a left point...
	TpsAssert( IsCoordLT(m_pLeft->m_y,pt.m_y)||AreCoordsEqual(m_pLeft->m_y,pt.m_y)/*pt.m_y >= m_pLeft->m_y*/, "Adding left point below previous left!" );
	if( *m_pLeft == pt )
		{
		//
		// duplicate point, so don't add it...
		return FALSE;
		}
	RPolyPoint*	pptNext	= m_pLeft->m_pNext;
	if( pptNext )
		{
		//
		// there is another point.  if we're adding a point on a horizontal or
		// vertical line, then simply override the other coordinate...
		if( AreCoordsEqual(pt.m_y,m_pLeft->m_y) && AreCoordsEqual(pt.m_y,pptNext->m_y) )
			{
			//
			// it's on a horizontal line, so override the previous left point...
			m_pLeft->m_x	= pt.m_x;
			return FALSE;
			}
		if( AreCoordsEqual(pt.m_x,m_pLeft->m_x) && AreCoordsEqual(pt.m_x,pptNext->m_x) )
			{
			//
			// it's on a vertical line, so override the previous right point...
			m_pLeft->m_y	= pt.m_y;
			return FALSE;
			}
		}
	RPolyPoint*	ppt	= new RPolyPoint( pt );
	ppt->m_pNext		= m_pLeft;
	m_pLeft				= ppt;
	if( !m_pPrevRight )
		{
		//
		// there was only one point before...
		m_pPrevRight	= m_pLeft;
		}
	return TRUE;
	}

//
// @MFunc Add the given point to the left side of this poly node
// @RDesc nothing
//
void RPolygonClipper::RPolyNode::AddLeft(
			const RRealPoint&	pt )	// @Parm the point to add to the left
	{
	RPolyNode*	pNode		= LeftmostNode();
	if( pNode->AddLeftPoint(pt) )
		++m_nPoints;
	VALIDATEPOLY( *this );
	}

//
// @CMember Add the given point to the right side of this node
// @RDesc TRUE if the point was added, FALSE if it's a duplicate
//
BOOLEAN RPolygonClipper::RPolyNode::AddRightPoint(
			const RRealPoint&	pt )	// @Parm the point to add to the right
	{
	if( !m_pRight )
		{
		m_pLeft				= new RPolyPoint(pt);
		m_pLeft->m_pNext	= NULL;
		m_pRight				= m_pLeft;
		m_pPrevRight		= NULL;
		return TRUE;
		}
	//
	// there is already a right point...
	TpsAssert( IsCoordLT(m_pRight->m_y,pt.m_y)||AreCoordsEqual(m_pRight->m_y,pt.m_y)/*pt.m_y >= m_pRight->m_y*/, "Adding right point below previous right!" );
	if( *m_pRight == pt )
		{
		//
		// duplicate point, so don't add it...
		return FALSE;
		}
	if( m_pPrevRight )
		{
		//
		// there is another point.  if we're adding a point on a horizontal or
		// vertical line, then simply override the other coordinate...
		if( AreCoordsEqual(pt.m_y,m_pRight->m_y) && AreCoordsEqual(pt.m_y,m_pPrevRight->m_y) )
			{
			//
			// it's on a horizontal line, so override the previous left point...
			m_pRight->m_x	= pt.m_x;
			return FALSE;
			}
		if( AreCoordsEqual(pt.m_x,m_pRight->m_x) && AreCoordsEqual(pt.m_x,m_pPrevRight->m_x) )
			{
			//
			// it's on a vertical line, so override the previous right point...
			m_pRight->m_y	= pt.m_y;
			return FALSE;
			}
		}
	RPolyPoint*	ppt	= new RPolyPoint( pt );
	m_pRight->m_pNext	= ppt;
	m_pPrevRight		= m_pRight;
	m_pRight				= ppt;
	return TRUE;
	}

//
// @CMember Add the given point to the right side of this poly node
// @RDesc nothing
//
void RPolygonClipper::RPolyNode::AddRight(
			const RRealPoint&	pt )	// @Parm the point to add to the right
	{
	RPolyNode*	pNode	= RightmostNode();
	if( pNode->AddRightPoint(pt) )
		++m_nPoints;
	VALIDATEPOLY( *this );
	}


//
// @MFunc determine if this node can be appended
BOOLEAN RPolygonClipper::RPolyNode::CanAppendTo( const RPolyPoint* pptLast, int* pnPointsInNode )
	{
	RPolyNode*			pLeft			= LeftmostNode();
	RPolyPoint*			ppt			= pLeft->m_pLeft;
	const RPolyPoint*	pprv			= pptLast? pptLast : ppt;
	int					npts			= 0;
	BOOLEAN				fAscending	= TRUE;
	while( ppt )
		{
		if( fAscending )
			fAscending = BOOLEAN( IsCoordLT(pprv->m_y,ppt->m_y) || AreCoordsEqual(pprv->m_y,ppt->m_y) );//ppt->m_y>=pprv->m_y );
		++npts;
		pprv	= ppt;
		ppt	= ppt->m_pNext;
		}
	if( pnPointsInNode )
		*pnPointsInNode	= npts;
	return fAscending;
	}

//
// @CMember determine if this node can be prepended
BOOLEAN RPolygonClipper::RPolyNode::CanPrependTo( const RPolyPoint* pptLast, int* pnPointsInNode )
	{
	RPolyNode*			pRight		= RightmostNode();
	RPolyPoint*			ppt			= pRight->m_pLeft;
	RPolyPoint*			pprv			= ppt;
	int					npts			= 0;
	BOOLEAN				fDescending	= TRUE;
	while( ppt )
		{
		if( fDescending )
			fDescending = BOOLEAN( IsCoordLT(ppt->m_y,pprv->m_y)||AreCoordsEqual(ppt->m_y,pprv->m_y) );//ppt->m_y<=pprv->m_y );
		++npts;
		pprv	= ppt;
		ppt	= ppt->m_pNext;
		}
	if( pnPointsInNode )
		*pnPointsInNode	= npts;
	return BOOLEAN( !pptLast || (fDescending && (IsCoordLT(pptLast->m_y,pprv->m_y)||AreCoordsEqual(pptLast->m_y,pprv->m_y))) );//pprv->m_y>=pptLast->m_y) );
	}

//
// @MFunc Append the given node to this one
// @RDesc nothing
//
void RPolygonClipper::RPolyNode::AppendPolyNode(
			RPolyNode*	pNode )	// @Parm pointer to the node to append to this one
	{
	VALIDATEPOLY( *this );
	VALIDATEPOLY( *pNode );

nextnode:
	RPolyNode*	pBefore	= RightmostNode();
	RPolyNode*	pAfter	= pNode->LeftmostNode();
	RPolyPoint*	pptBefore	= pBefore->m_pRight;
	RPolyPoint*	pptAfter		= pAfter->m_pLeft;
	TpsAssert( pptBefore!=NULL, "Attempting to append to an empty node" );
	TpsAssert( pptAfter!=NULL, "Attempting to append an empty node" );
	//
	// Remove duplicates...
	if( *pptAfter == *pptBefore )
		{
		pNode->RemoveLeft();
		TrimLeftnode( pNode );
		VALIDATEPOLY( *this );
		pAfter	= pNode->LeftmostNode();
		pptAfter	= pAfter->m_pLeft;
		}
	//
	// merge...
	int	nPointsToMove;
	if( pAfter->CanAppendTo(pptBefore,&nPointsToMove) || pBefore->CanPrependTo(pptAfter,NULL) )
		{
		//
		// we can simply move the points from pAfter into
		// pBefore...
		if( pAfter->m_pLeft )
			{
			pBefore->m_pRight->m_pNext	= pAfter->m_pLeft;
			pBefore->m_pRight				= pAfter->m_pRight;
			pBefore->m_pPrevRight		= pAfter->m_pPrevRight;
			pAfter->m_pLeft				= NULL;
			pAfter->m_pRight				= NULL;
			pAfter->m_pPrevRight			= NULL;
			if( !pBefore->m_pPrevRight )
				{
				if( pBefore->m_pLeft != pBefore->m_pRight )
					{
					RPolyPoint*	ppt	= pBefore->m_pLeft;
					RPolyPoint*	prv	= NULL;
					while( ppt->m_pNext )
						{
						prv	= ppt;
						ppt	= ppt->m_pNext;
						}
					pBefore->m_pPrevRight	= prv;
					}
				}
			}
#ifdef TPSDEBUG
		else
			TpsAssert( nPointsToMove==0, "Pointers are all messed up" );
#endif
		pNode->m_nPoints				-= nPointsToMove;
		m_nPoints						+= nPointsToMove;
		if( pAfter->m_pNodeRight )
			{
			TrimLeftnode( pNode );
			VALIDATEPOLY( *this );
			goto nextnode;
			}
		VALIDATEPOLY( *this );
		}

	if( pNode->m_nPoints )
		{
		pBefore->m_pNodeRight	= pAfter;
		pAfter->m_pNodeLeft		= pBefore;
		m_nPoints					+= pNode->m_nPoints;
		pNode->m_nPoints			= 0;
		}
	//
	// The interior edges no longer contribute to anything
	m_pRightEdge->pPolygon			= NULL;
	pNode->m_pLeftEdge->pPolygon	= NULL;
	//
	// The outer edges now both contribute to this node...
	m_pRightEdge				= pNode->m_pRightEdge;
	m_pRightEdge->pPolygon	= this;
	#ifdef TPSDEBUG
	//
	// combine the names...
	m_Name += pNode->m_Name;
	pNode->m_Name	= "";
	#endif
	VALIDATEPOLY( *this );
	}

//
// @CMember Prepend the given node onto this one
// @RDesc nothing
//
void RPolygonClipper::RPolyNode::PrependPolyNode(
			RPolyNode*	pNode )	// @Parm pointer to the node to prepend onto this one
	{
	VALIDATEPOLY( *this );
	VALIDATEPOLY( *pNode );
	RPolyNode*	pTarget = this;

nextnode:
	RPolyNode*	pBefore		= pNode->RightmostNode();
	RPolyNode*	pAfter		= pTarget->LeftmostNode();
	RPolyPoint*	pptBefore	= pBefore->m_pRight;
	RPolyPoint*	pptAfter		= pAfter->m_pLeft;
	TpsAssert( pptBefore!=NULL, "Attempting to prepend an empty node" );
	TpsAssert( pptAfter!=NULL, "Attempting to prepend to an empty node" );
	//
	// Remove duplicates...
	if( *pptAfter == *pptBefore )
		{
		pTarget->RemoveLeft();
		TrimLeftnode( pTarget );
		pAfter	= pTarget->LeftmostNode();
		pptAfter	= pAfter->m_pLeft;
		}
	//
	// merge...
	int	nPointsToMove;
	if( pBefore->CanPrependTo(pptAfter,&nPointsToMove) || pAfter->CanAppendTo(pptBefore,NULL) )
		{
		//
		// we can simply move the points from pBefore into
		// pAfter...
		pBefore->m_pRight->m_pNext	= pAfter->m_pLeft;
		pAfter->m_pLeft				= pBefore->m_pLeft;
		if( !pAfter->m_pRight )
			pAfter->m_pRight = pBefore->m_pRight;
		pBefore->m_pLeft				= NULL;
		pBefore->m_pRight				= NULL;
		pBefore->m_pPrevRight		= NULL;
		pNode->m_nPoints				-= nPointsToMove;
		pTarget->m_nPoints			+= nPointsToMove;
		if( !pAfter->m_pPrevRight )
			{
			RPolyPoint*	ppt	= pAfter->m_pLeft;
			RPolyPoint*	prv	= NULL;
			while( ppt->m_pNext )
				{
				prv	= ppt;
				ppt	= ppt->m_pNext;
				}
			pAfter->m_pPrevRight	= prv;
			}
		if( pBefore->m_pNodeLeft )
			{
			TrimRightnode( pNode );
			VALIDATEPOLY( *pTarget );
			goto nextnode;
			}
		VALIDATEPOLY( *pTarget );
		}
	if( pNode->m_nPoints )
		{
		pBefore->m_pNodeRight	= pAfter;
		pAfter->m_pNodeLeft		= pBefore;
		pTarget->m_nPoints		+= pNode->m_nPoints;
		pNode->m_nPoints			= 0;
		}
	//
	// The interior edges no longer contribute to anything
	pTarget->m_pLeftEdge->pPolygon	= NULL;
	pNode->m_pRightEdge->pPolygon		= NULL;
	//
	// The outer edges now both contribute to this node...
	pTarget->m_pLeftEdge					= pNode->m_pLeftEdge;
	pTarget->m_pLeftEdge->pPolygon	= pTarget;
	#ifdef TPSDEBUG
	//
	// combine the names...
	pNode->m_Name		+= pTarget->m_Name;
	pTarget->m_Name	= pNode->m_Name;
	pNode->m_Name		= "";
	#endif
	VALIDATEPOLY( *pTarget );
	}

//
// @CMember Reverse the direction of this node
// @RDesc nothing
//
void RPolygonClipper::RPolyNode::Reverse()
	{
	VALIDATEPOLY( *this );
	RPolyNode*	pPrv	= NULL;
	RPolyNode*	pCur	= LeftmostNode();
	while( pCur )
		{
		RPolyPoint*	pptPrv		= NULL;
		RPolyPoint*	pptCur		= pCur->m_pLeft;
		pCur->m_pRight				= pptCur;
		pCur->m_pPrevRight		= pptCur? pptCur->m_pNext : NULL;
		while( pptCur )
			{
			RPolyPoint*	pptNext	= pptCur->m_pNext;
			pptCur->m_pNext		= pptPrv;
			pptPrv					= pptCur;
			pptCur					= pptNext;
			}
		pCur->m_pLeft			= pptPrv;
		pCur->m_pNodeLeft		= pCur->m_pNodeRight;
		pCur->m_pNodeRight	= pPrv;
		pPrv						= pCur;
		pCur						= pCur->m_pNodeLeft;
		}
	ActiveEdge*	pLeftEdge	= m_pLeftEdge;
	m_pLeftEdge					= m_pRightEdge;
	m_pRightEdge				= pLeftEdge;
	VALIDATEPOLY( *this );
	}

#ifdef TPSDEBUG
void RPolygonClipper::RPolyNode::Dump( char* why ) const
	{
	STARTFUNC( ("Polynode %s (%d points): %s\n",(LPCTSTR)m_Name,m_nPoints,why) );
	RPolyNode*	pNode	= const_cast<RPolygonClipper::RPolyNode*>(this)->LeftmostNode();
	while( pNode )
		{
		STARTFUNC( ("SubNode\n") );
		RPolyPoint*	ppt	= pNode->m_pLeft;
		while( ppt )
			{
			CONTINUEFUNC( ("(%.3f,%.3f)\n",ppt->m_x,ppt->m_y) );
			ppt = ppt->m_pNext;
			}
		ENDFUNC( ("\n") );
		pNode = pNode->m_pNodeRight;
		}
	ENDFUNC( ("\n") );
	}

void RPolygonClipper::RPolyNode::Validate() const
	{
	RPolyNode*	pNode	= const_cast<RPolyNode*>(this)->LeftmostNode();
	int			npts	= 0;
	while( pNode )
		{
		//
		// within each node, there should be a min point that is NOT on either end (unless it only
		// has one point)...
		RPolyPoint*	ppt			= pNode->m_pLeft;
		RPolyPoint*	prv			= ppt;
		RPolyPoint*	prvprv		= NULL;
		BOOLEAN		fAscending	= FALSE;
		TpsAssert( ppt!=NULL, "Empty node" );
		while( ppt )
			{
			if( IsCoordLT(prv->m_y,ppt->m_y) )//ppt->m_y > prv->m_y )
				fAscending = TRUE;
#ifdef TPSDEBUG
			else if( IsCoordLT(ppt->m_y,prv->m_y) )//ppt->m_y < prv->m_y )
				TpsAssert( !fAscending, "Found a local maximum" );
#endif
			prvprv	= prv;
			prv		= ppt;
			ppt		= ppt->m_pNext;
			++npts;
			}
		TpsAssert( prv==pNode->m_pRight, "Right pointer wrong" );
		TpsAssert( pNode->m_pLeft==pNode->m_pRight || prvprv==pNode->m_pPrevRight, "Previous Right pointer wrong" );
		pNode	= pNode->m_pNodeRight;
		}
	TpsAssert( npts==m_nPoints, "Point count wrong" );
	}
#endif

//
// @MFunc Add an intersection to the intersection table
// @RDesc nothing
//
void RPolygonClipper::AddIntersection(
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm pointer to the leading intersection edge
			RPolygonClipper::ActiveEdge*	pEdge2,	// @Parm pointer to the trailing intersection edge
			const RRealPoint&			pt )		// @Parm the point of intersection.
	{
	RITEntry*	pit	= new RITEntry( pEdge1, pEdge2, pt );
	RITEntry*	prev	= NULL;
	RITEntry*	next	= m_pIT;
	while( next && next->m_y < pt.m_y )
		{
		prev	= next;
		next	= next->m_pNext;
		}
	while( next && next->m_y==pt.m_y && next->m_x < pt.m_x )
		{
		prev	= next;
		next	= next->m_pNext;
		}
	pit->m_pNext	= next;
	if( prev )
		prev->m_pNext	= pit;
	else
		m_pIT = pit;
	}

//
// @MFunc Delete the intersection table
// @RDesc nothing
//
void RPolygonClipper::DeleteIT()
	{
	while( m_pIT )
		{
		RITEntry*	pPrev	= m_pIT;
		m_pIT					= m_pIT->m_pNext;
		delete pPrev;
		}
	}

//
// @MFunc Initialize the ScanBeam table
// @RDesc nothing
//
void RPolygonClipper::InitSBT()
	{
	RClipPolygon::LMTEntry*	plmt	= m_LMT.GetFirstEntry();
	while( plmt )
		{
		InsertSBTEntry( float(plmt->pt.m_y) );
		RPolyPoint*	pPoint	= plmt->pLeft;
		while( pPoint )
			{
			InsertSBTEntry( float(pPoint->m_y) );
			pPoint = pPoint->m_pNext;
			}
		pPoint	= plmt->pRight;
		while( pPoint )
			{
			InsertSBTEntry( float(pPoint->m_y) );
			pPoint = pPoint->m_pNext;
			}
		plmt = m_LMT.GetNextEntry( plmt );
		}
	}

//
// @MFunc Delete the ScanBeam table
// @RDesc nothing
//
void RPolygonClipper::DeleteSBT()
	{
	while( m_pSBT )
		{
		SBTEntry*	pPrev	= m_pSBT;
		m_pSBT				= m_pSBT->pNext;
		delete pPrev;
		}
	}

//
// @MFunc Insert an entry into the ScanBeam table
// @RDesc nothing
//
void RPolygonClipper::InsertSBTEntry(
			float	y )	// @Parm the Y coordinate to be inserted into the ScanBeam table
	{
	SBTEntry*	prev	= NULL;
	SBTEntry*	next	= m_pSBT;
//	while( next && next->y < y )
	while( next && IsCoordLT(next->y,y) )
		{
		prev	= next;
		next	= next->pNext;
		}
//	if( !next || next->y != y )
	if( !next || !AreCoordsEqual(next->y,y) )
		{
		SBTEntry*	pent	= new SBTEntry;
		pent->y				= y;
		pent->pNext			= next;
		if( prev )
			prev->pNext = pent;
		else
			m_pSBT = pent;
		}
	}

//
// @MFunc Swap the given ActiveEdge entries in the table
// @RDesc nothing
//
void RPolygonClipper::SwapAETEntries(
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm pointer to the first edge being swapped
			RPolygonClipper::ActiveEdge*	pEdge2 )	// @Parm pointer to the edge it's being swapped with
	{
	if( pEdge1->paeNext == pEdge2 )
		{
		ActiveEdge*	pPrev1	= pEdge1->paePrev;
		ActiveEdge*	pNext2	= pEdge2->paeNext;
		LinkAETEntries( pPrev1, pEdge2 );
		LinkAETEntries( pEdge2, pEdge1 );
		LinkAETEntries( pEdge1, pNext2 );
		}
	else if( pEdge2->paeNext == pEdge1 )
		{
		ActiveEdge*	pPrev2	= pEdge2->paePrev;
		ActiveEdge*	pNext1	= pEdge1->paeNext;
		LinkAETEntries( pPrev2, pEdge1 );
		LinkAETEntries( pEdge1, pEdge2 );
		LinkAETEntries( pEdge2, pNext1 );
		}
	else
		{
		ActiveEdge*	pPrev1	= pEdge1->paePrev;
		ActiveEdge*	pNext1	= pEdge1->paeNext;
		ActiveEdge*	pPrev2	= pEdge2->paePrev;
		ActiveEdge*	pNext2	= pEdge2->paeNext;
		LinkAETEntries( pPrev1, pEdge2 );
		LinkAETEntries( pEdge2, pNext1 );
		LinkAETEntries( pPrev2, pEdge1 );
		LinkAETEntries( pEdge1, pNext2 );
		}
	}

//
// @MFunc Advance the given ActiveEdge to the next segment
// @RDesc nothing
//
void RPolygonClipper::AdvanceAETEdge(
			RPolygonClipper::ActiveEdge*	pEdge )	// @Parm pointer to the edge to advance
	{
	float	yMin			= pEdge->yMax;
	if( pEdge->horz )
		--m_nHorzEdges;
	pEdge->xBottom		= pEdge->xTop;
	pEdge->xTop			= float(pEdge->pptNextTop->m_x);
	pEdge->yMax			= float(pEdge->pptNextTop->m_y);
	pEdge->pptNextTop	= pEdge->pptNextTop->m_pNext;
	pEdge->horz			= AreCoordsEqual(yMin,pEdge->yMax)?1:0;//(yMin==pEdge->yMax)? 1 : 0;
	if( !pEdge->horz )
		pEdge->xIncr = float((pEdge->xTop-pEdge->xBottom)/(pEdge->yMax-yMin));
	else
		++m_nHorzEdges;
	}

//
// @MFunc Create a new ActiveEdge
// @RDesc pointer to the new edge
//
RPolygonClipper::ActiveEdge* RPolygonClipper::CreateAETEntry(
			const RRealPoint&	ptBottom,	// @Parm point at the bottom of the edge
			RPolyPoint*			pptTop,		// @Parm pointer to the point at the top of the first edge segment
			unsigned int		clip )		// @Parm TRUE if the edge is from a clip polygon else FALSE (from a subject polygon)
	{
	ActiveEdge*	pEdge	= new ActiveEdge;
	#ifdef TPSDEBUG
	pEdge->nEdge		= ++m_nEdge;
	#endif
	pEdge->xBottom		= float(ptBottom.m_x);
	pEdge->xTop			= float(pptTop->m_x);
	pEdge->yMax			= float(pptTop->m_y);
	pEdge->pptNextTop	= pptTop->m_pNext;
	pEdge->clip			= clip;
	pEdge->pPolygon	= NULL;
	pEdge->horz			= AreCoordsEqual(ptBottom.m_y,pEdge->yMax)?1:0;//(ptBottom.m_y==pEdge->yMax)? 1 : 0;
	if( !pEdge->horz )
		pEdge->xIncr = float((pEdge->xTop-pEdge->xBottom)/(pEdge->yMax-ptBottom.m_y));
	return pEdge;
	}

//
// @MFunc Insert an ActiveEdge into the table
// @RDesc nothing
//
void RPolygonClipper::InsertAETEntry(
			RPolygonClipper::ActiveEdge*	pEdge )	// @Parm pointer to the edge to insert into the table
	{
	ActiveEdge*	pPrev	= NULL;
	ActiveEdge*	pNext	= m_pAET;
	if( pEdge->horz )
		++m_nHorzEdges;
	while( pNext && pEdge->xBottom>pNext->xBottom )
		{
		pPrev	= pNext;
		pNext	= pNext->paeNext;
		}
	BOOLEAN	fNext;
	do
		{
		fNext	= FALSE;
		if( pNext && pEdge->xBottom==pNext->xBottom )
			{
			if( pEdge->horz )
				{
				fNext	= BOOLEAN(pNext->horz? (pEdge->xTop>pNext->xTop) :
													  (pEdge->xTop>pNext->xBottom) );
				}
			else
				{
				fNext	= BOOLEAN(pNext->horz? (pEdge->xBottom>pNext->xTop) :
													  (pEdge->xIncr > pNext->xIncr) );
				}
			if( fNext )
				{
				pPrev	= pNext;
				pNext	= pPrev->paeNext;
				}
			}
		}
	while( fNext );
	LinkAETEntries( pPrev, pEdge );
	LinkAETEntries( pEdge, pNext );
	//
	// determine edge side...
	int	nEdges	= 0;
	pPrev	= pEdge->paePrev;
	while( pPrev )
		{
		if( pPrev->clip == pEdge->clip )
			++nEdges;
		pPrev = pPrev->paePrev;
		}
	pEdge->left	= (nEdges&1)? 0 : 1;
	}

//
// @MFunc Remove an ActiveEdge from the table
// @RDesc nothing
//
void RPolygonClipper::RemoveAETEntry(
			RPolygonClipper::ActiveEdge*	pEdge )	// @Parm pointer to the edge to be removed and deleted
	{
	LinkAETEntries( pEdge->paePrev, pEdge->paeNext );
	if( pEdge->horz )
		--m_nHorzEdges;
	delete pEdge;
	}

//
// @MFunc Delete the entire active edge table
// @RDesc nothing
void RPolygonClipper::DeleteAET()
	{
	while( m_pAET )
		{
		ActiveEdge*	pPrev	= m_pAET;
		m_pAET				= m_pAET->paeNext;
		delete pPrev;
		}
	}

//
// @MFunc Create a polygon node and add it to the table
// @RDesc nothing
//
void RPolygonClipper::CreatePolyNode(
			RPolygonClipper::ActiveEdge*	pLeftEdge,	// @Parm pointer to the left contributing edge
			RPolygonClipper::ActiveEdge*	pRightEdge,	// @Parm pointer to the right contributing edge
			const RRealPoint&			pt )			// @Parm the point at which the polygon starts
	{
	RPolyNode*	pNode	= new RPolyNode( pLeftEdge, pRightEdge, pt );
	pNode->m_pNext		= m_pClipPolyNodes;
	m_pClipPolyNodes	= pNode;
	pLeftEdge->pPolygon	= pNode;
	pRightEdge->pPolygon	= pNode;
	}

//
// @MFunc Delete the polygon node table
// @RDesc nothing
//
void RPolygonClipper::DeletePolyNodes()
	{
	while( m_pClipPolyNodes )
		{
		RPolyNode*	pPrev	= m_pClipPolyNodes;
		m_pClipPolyNodes	= m_pClipPolyNodes->m_pNext;
		delete pPrev;
		}
	}

//
// @MFunc Correct the ActiveEdge table prior to processing a local max
// @RDesc nothing
//
void RPolygonClipper::PreprocessLocalMax( ActiveEdge* aetEdge, ActiveEdge* aetOther )
	{
	STARTFUNC( ("PreprocessLocalMax between edge %d and edge %d\n",aetEdge->nEdge,aetOther->nEdge) );
	//
	// there is a localmax intersection between aetEdge and aetOther.
	// there are n possibilities...
	//		1. both aetEdge and aetOther are sans polygons
	//		2. both aetEdge and aetOther have polygons
	//		3. aetEdge has a polygon and aetOther does not
	//		4.	aetOther has a polygon and aetEdge does not
	//	if it's 1 or 2, then there's no problem.
	// if it's 3:
	//		if there is an intervening edge and it does not have a polygon:
	//			swap the polygon from aetEdge into the intervening edge
	//		if there is an intervening edge and it does have a polygon:
	//			swap the polygon from the intervening edge into aetOther
	//		if there is no intervening edge:
	//			
	// if it's 4:
	//		if there is an intervening edge and it has a polygon:
	//			swap the polygon from the intervening edge into aetEdge
	//		if there is an intervening edge and it does NOT have a polygon:
	//			swap the polygon from aetOther into the intervening edge
	//		if there is no intervening edge:
	ActiveEdge*	aetTmp	= GetNextAETEntry(aetEdge);
	while( aetTmp && aetTmp!=aetOther )
		aetTmp = GetNextAETEntry(aetTmp);
	if( !aetTmp )
		{
		aetTmp	= aetEdge;
		aetEdge	= aetOther;
		aetOther	= aetTmp;
		}
	if( aetEdge->pPolygon && !aetOther->pPolygon )
		{
		//
		// case 3: aetEdge has a polygon, but aetOther doesn't.
		// find the edge between that does not have a polygon...
		ActiveEdge*	aetNext	= GetNextAETEntry( aetEdge );
		while( aetNext!=aetOther && aetNext->pPolygon )
			aetNext = GetNextAETEntry( aetNext );
		if( aetNext != aetOther )
			{
			//
			// Swap the polygon out of aetEdge and into the intervening edge...
			CONTINUEFUNC( ("Intervening edge %d has no polygon: swapping it in from edge %d\n",aetNext->nEdge,aetEdge->nEdge) );
			//
			// First, add the point...
			AddPolyPoint( aetEdge, RRealPoint(aetEdge->xTop,aetEdge->yMax) );
			//
			// Then swap the polygons...
			SwapEdgePolygons( aetEdge, aetNext );
			}
		else if( aetEdge->paeNext != aetOther )
			{
			//
			// Swap the polygon out of the intervening edge and into aetOther...
			CONTINUEFUNC( ("No intervening edge is without a polygon: swapping edge %d's polygon into edge %d\n",aetEdge->paeNext->nEdge,aetOther->nEdge) );
			SwapEdgePolygons( aetEdge->paeNext,aetOther );
			}
		else
			{
			//
			// no intervening edge...
			CONTINUEFUNC( ("No intervening edge, checking for trailing duplicate edges\n") );
			//
			// remember: aetNext == aetOther...
			// also: aetOther doesn't have a polygon...
			BOOLEAN	fDuplicate	= TRUE;
			while( fDuplicate && (aetNext=GetNextAETEntry(aetNext)) != NULL )
				{
				fDuplicate	= BOOLEAN( AreCoordsEqual(aetNext->xBottom,aetOther->xBottom) && AreCoordsEqual(aetNext->xTop,aetOther->xTop) );
				if( fDuplicate && aetNext->pPolygon )
					break;
				}
			if( aetNext && fDuplicate )
				{
				//
				// then it has a polygon.
				// swap it with aetOther...
				CONTINUEFUNC( ("Duplicate of edge %d found at edge %d, swapping polygons\n",aetOther->nEdge,aetNext->nEdge) );
				SwapEdgePolygons( aetOther, aetNext );
				}
			else
				{
				//
				// there is no Duplicate to the next, so check for a Duplicate to the edge.
				CONTINUEFUNC( ("No trailing duplicate, checking for leading duplicate edges\n") );
				fDuplicate	= TRUE;
				aetNext	= aetEdge;
				while( fDuplicate && (aetNext=GetPrevAETEntry(aetNext))!=NULL )
					{
					fDuplicate = BOOLEAN( AreCoordsEqual(aetNext->xBottom,aetEdge->xBottom) && AreCoordsEqual(aetNext->xTop,aetEdge->xTop) );
					if( fDuplicate && !aetNext->pPolygon )
						break;
					}
				if( aetNext && fDuplicate )
					{
					//
					// then it has a polygon.
					// swap it with aetOther...
					CONTINUEFUNC( ("Duplicate of edge %d found at edge %d, swapping polygons\n",aetEdge->nEdge,aetNext->nEdge) );
					SwapEdgePolygons( aetEdge, aetNext );
					}
				else
					{
					//
					// bizarre case, so try to fix it by forcing the polygon into aetOther from the other edge...
					if( aetEdge == aetEdge->pPolygon->m_pLeftEdge )
						{
						CONTINUEFUNC( ("No duplicate, so swapping polygon edges between %d and %d\n",aetOther->nEdge,aetEdge->pPolygon->m_pRightEdge->nEdge) );
						SwapEdgePolygons( aetOther, aetEdge->pPolygon->m_pRightEdge );
						}
					else
						{
						CONTINUEFUNC( ("No duplicate, so swapping polygon edges between %d and %d\n",aetOther->nEdge,aetEdge->pPolygon->m_pLeftEdge->nEdge) );
						SwapEdgePolygons( aetOther, aetEdge->pPolygon->m_pLeftEdge );
						}
					}
				}
			}
		}
	else if( aetOther->pPolygon && !aetEdge->pPolygon )
		{
		//
		// case 4: aetOther has a polygon, but aetEdge doesn't.
		// find the edge between that has a polygon...
		ActiveEdge*	aetNext	= GetNextAETEntry( aetEdge );
		while( aetNext!=aetOther && !aetNext->pPolygon )
			aetNext = GetNextAETEntry( aetNext );
		if( aetNext!=aetOther )
			{
			//
			// Swap the polygon out of the intervening edge and into aetEdge...
			CONTINUEFUNC( ("Intervening edge %d has a polygon: swapping it into edge %d\n",aetNext->nEdge,aetEdge->nEdge) );
			SwapEdgePolygons( aetEdge, aetNext );
			}
		else if( aetEdge->paeNext != aetOther )
			{
			//
			// Swap the polygon out of aetOther and into the intervening edge...
			CONTINUEFUNC( ("No intervening edge has a polygon, swapping the polygon from edge %d into edge %d\n",aetOther->nEdge,aetEdge->paeNext->nEdge) );
			//
			// Add the point first...
			AddPolyPoint( aetOther, RRealPoint(aetOther->xTop,aetOther->yMax) );
			//
			// now swap the polygons...
			SwapEdgePolygons( aetOther,aetEdge->paeNext );
			}
		else
			{
			BOOLEAN	fDuplicate	= TRUE;
			while( fDuplicate && (aetNext=GetNextAETEntry(aetNext)) != NULL )
				{
				fDuplicate	= BOOLEAN( AreCoordsEqual(aetNext->xBottom,aetOther->xBottom) && AreCoordsEqual(aetNext->xTop,aetOther->xTop) );
				if( fDuplicate && !aetNext->pPolygon )
					break;
				}
			if( aetNext && fDuplicate )
				{
				//
				// then it has a polygon.
				// swap it with aetOther...
				CONTINUEFUNC( ("Duplicate of edge %d found at edge %d, swapping polygons\n",aetOther->nEdge,aetNext->nEdge) );
				SwapEdgePolygons( aetOther, aetNext );
				}
			else
				{
				//
				// there is no Duplicate to the next, so check for a Duplicate to the edge.
				CONTINUEFUNC( ("No trailing duplicate, checking for leading duplicate edges\n") );
				fDuplicate	= TRUE;
				aetNext	= aetEdge;
				while( fDuplicate && (aetNext=GetPrevAETEntry(aetNext))!=NULL )
					{
					fDuplicate = BOOLEAN( AreCoordsEqual(aetNext->xBottom,aetEdge->xBottom) && AreCoordsEqual(aetNext->xTop,aetEdge->xTop) );
					if( fDuplicate && aetNext->pPolygon )
						break;
					}
				if( aetNext && fDuplicate )
					{
					//
					// then it has a polygon.
					// swap it with aetOther...
					CONTINUEFUNC( ("Duplicate of edge %d found at edge %d, swapping polygons\n",aetEdge->nEdge,aetNext->nEdge) );
					SwapEdgePolygons( aetEdge, aetNext );
					}
				else
					{
					//
					// bizarre case, so try to fix it by forcing the polygon into aetEdge from the other edge...
					if( aetOther == aetOther->pPolygon->m_pLeftEdge )
						SwapEdgePolygons( aetEdge, aetOther->pPolygon->m_pRightEdge );
					else
						SwapEdgePolygons( aetEdge, aetOther->pPolygon->m_pLeftEdge );
					}
				}
			}
		}
	ENDFUNC( ("\n") );
	}

//
// @MFunc Top level intersection handler
// @RDesc nothing
//
void RPolygonClipper::HandleIntersection(
			RPolygonClipper::ActiveEdge*	edge1,	// @Parm pointer to the leading intersection edge
			RPolygonClipper::ActiveEdge*	edge2,	// @Parm pointer to the trailing intersection edge
			const RRealPoint&			pt )		// @Parm the point of intersection
	{
	//
	// classification rules:
	// edges are classified by a two letter word.  the first
	// letter indicates whether the edge is left(L) or right(R),
	// and the second letter indicates whether the edge belongs
	// to the subject(S) or the clip(C) polygon.
	// the vertex at the intersection of the edges is assigned
	// one of the four vertex classifications: local minimum (MN),
	// local maximum (MX), left intermediate (LI) and right
	// intermediate (RI).  if both edges belong to the same
	// polygon, then it's a like-edge intersection...
	const int	LC2	= 0;
	const int	RC2	= 1;
	const int	LS2	= 2;
	const int	RS2	= 3;
	const int	LC1	= 0;
	const int	RC1	= 4;
	const int	LS1	= 8;
	const int	RS1	= 12;
	int	nClass1	= edge1->left? (edge1->clip? LC1:LS1) : (edge1->clip? RC1:RS1);
	int	nClass2	= edge2->left? (edge2->clip? LC2:LS2) : (edge2->clip? RC2:RS2);
	(*m_pfnHandleIntersection[nClass1+nClass2])( this, edge1, edge2, pt );
	//
	// swap edge1/edge2 positions in the AET...
	SwapAETEntries( edge1, edge2 );
	#ifdef TPSDEBUG
	DumpAET( "Entries swapped after intersection" );
	#endif
	}

//
// @MFunc Handle the end of a segment in an edge
// @RDesc pointer to the next edge to be processed
//
RPolygonClipper::ActiveEdge* RPolygonClipper::HandleEndOfSegment(
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm pointer to the leading edge
			RPolygonClipper::ActiveEdge*	pEdge2 )	// @Parm pointer to the trailing edge
	{
#ifdef TPSDEBUG
	TpsAssert( pEdge1!=NULL, "HandleEndOfSegment: NULL edge1" );
	if( pEdge1==NULL )
		throw kUnknownError;
#endif
	RRealPoint			pt( pEdge1->xTop, pEdge1->yMax );
	ActiveEdge*			pNext	= pEdge2;
	RPolyPoint*			pNextTop	= pEdge1->pptNextTop;
	STARTFUNC( ("HandleEndOfSegment edge1=%d, edge2=%d\n",pEdge1->nEdge,pEdge2?pEdge2->nEdge:-1) );
	//
	// if this is the last sub-edge in the AET edge, then this is a local maximum...
	if( !pNextTop )
		{
		if( pEdge2 )
			{
			//
			// make sure we have the correct edge for edge2...
			while( pEdge2 )
				{
				if( pEdge2->clip==pEdge1->clip &&
					 pEdge2->pptNextTop==NULL )
					{
					if( pEdge2->yMax==pEdge1->yMax )
						break;
					TpsAssert( !AreCoordsEqual(pEdge2->yMax,pEdge1->yMax),"dammit" );
					}
				pEdge2 = GetNextAETEntry( pEdge2 );
				}
			if( !pEdge2 )
				{
				//
				// bizarre case where table was incorrect...
				pEdge2	= GetPrevAETEntry( pEdge1 );
				while( pEdge2 )
					{
					if( pEdge2->clip==pEdge1->clip &&
						pEdge2->pptNextTop==NULL )
						{
						if( pEdge2->yMax==pEdge1->yMax )
							break;
						TpsAssert( !AreCoordsEqual(pEdge2->yMax,pEdge1->yMax),"dammit" );
						}
					pEdge2 = GetPrevAETEntry( pEdge2 );
					}
				}
#ifdef TPSDEBUG
			TpsAssert( pEdge2 != NULL, "Couldn't find other ending edge" );
			if( pEdge2==NULL )
				throw kUnknownError;
#endif
			if( pEdge2==pNext )
				pNext	= GetNextAETEntry( pEdge2 );
			//
			// process the local maximum...
			ProcessLocalMaxIntersection( this, pEdge1, pEdge2, pt );
			//
			// They nolonger exist...
			RemoveAETEntry( pEdge1 );
			RemoveAETEntry( pEdge2 );
			}
		else
			{
			AddPolyPoint( pEdge1, pt );
			RemoveAETEntry( pEdge1 );
			}
		}
	else
		{
		//
		// it's an intermediate point.
		// Add the point to the polygon...
		AddPolyPoint( pEdge1, pt );
		#ifdef TPSDEBUG
		if( pEdge1->pPolygon ) pEdge1->pPolygon->Dump( "HandleEndOfSegment: Added Edge1 Point" );
		#endif
		//
		// Advance to the next segment in the edge...
		AdvanceAETEdge( pEdge1 );
		}// switch vertex class
	ENDFUNC( ("\n") );
	return pNext;
	}

//
// @MFunc Swap the polygons of the given edges
// @RDesc nothing
//
void RPolygonClipper::SwapEdgePolygons(
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm pointer to the leading edge
			RPolygonClipper::ActiveEdge*	pEdge2 )	// @Parm pointer to the trailing edge
	{
	RPolyNode*	poly1	= pEdge1->pPolygon;
	RPolyNode*	poly2	= pEdge2->pPolygon;
	if( poly1 )
		{
		if( poly1->m_pLeftEdge == pEdge1 )
			poly1->m_pLeftEdge	= pEdge2;
		else
			poly1->m_pRightEdge	= pEdge2;
		}
	if( poly2 )
		{
		if( poly2->m_pLeftEdge == pEdge2 )
			poly2->m_pLeftEdge	= pEdge1;
		else
			poly2->m_pRightEdge	= pEdge1;
		}
	pEdge1->pPolygon	= poly2;
	pEdge2->pPolygon	= poly1;
	}

//
// @MFunc Close a polygon at the given edges
// @RDesc nothing
//
void RPolygonClipper::ClosePolygon(
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm pointer to the leading intersecting edge
			RPolygonClipper::ActiveEdge*	pEdge2 )	// @Parm pointer to the trailing intersecting edge
	{
	RPolyNode*	poly1	= pEdge1->pPolygon;
	RPolyNode*	poly2	= pEdge2->pPolygon;
	STARTFUNC( ("ClosePolygon: edge 1 poly=%s, edge 2 poly=%s\n",(LPCTSTR)pEdge1->pPolygon->m_Name,(LPCTSTR)pEdge2->pPolygon->m_Name) );
	if( poly1 != poly2 )
		{
		//
		// different polygons, so we need to merge them...
		CONTINUEFUNC( ("Different polygons, merging\n") );
		BOOLEAN	fLeft1	= BOOLEAN( poly1->m_pLeftEdge == pEdge1 );
		BOOLEAN	fLeft2	= BOOLEAN( poly2->m_pLeftEdge == pEdge2 );
		if( fLeft1 == fLeft2 )
			{
			//
			// edge1 and edge2 contribute to the same side of their respective polygons.
			// reverse poly1...
			poly1->Reverse();
			fLeft1	= !fLeft1;
			}
		//
		// merge them...
		if( fLeft1 )
			{
			//
			// edge1 contributes to the left, edge2 contributes to the right.
			// append edge1 poly to edge2 poly...
			poly2->AppendPolyNode( poly1 );
			#ifdef TPSDEBUG
			poly2->Dump( "ClosePolygon: Polygon appended" );
			#endif
			}
		else
			{
			//
			// edge1 contributes to the right, edge2 contributes to the left.
			// prepend edge1 poly to edge2 poly...
			poly2->PrependPolyNode( poly1 );
			#ifdef TPSDEBUG
			poly2->Dump( "ClosePolygon: Polygon prepended" );
			#endif
			}
		}
	#ifdef TPSDEBUG
	else CONTINUEFUNC( ("Same polygon, terminating\n") );
	#endif
	//
	// these two edges nolonger contribute to any polygons...
	pEdge1->pPolygon	= NULL;
	pEdge2->pPolygon	= NULL;
	ENDFUNC( ("\n") );
	}

//
// @MFunc Add both edges from the LMT entry to the AET
// @RDesc nothing
//
void RPolygonClipper::AddEdges(
			RClipPolygon::LMTEntry*	pLocalMin )	// @Parm pointer to the LMT entry to add edges for
	{
	//
	// Create active edges for both sides of the local min...
	ActiveEdge*	pLft	= CreateAETEntry( pLocalMin->pt, pLocalMin->pLeft, pLocalMin->clip );
	ActiveEdge*	pRgt	= CreateAETEntry( pLocalMin->pt, pLocalMin->pRight, pLocalMin->clip );
	//
	// Insert them in the table...
	InsertAETEntry( pLft );
	pRgt->left	= !pLft->left;
	LinkAETEntries( pRgt, pLft->paeNext );
	LinkAETEntries( pLft, pRgt );
	if( pRgt->horz )
		++m_nHorzEdges;
	pRgt->pNextNew	= m_pNewEdges;
	pLft->pNextNew	= pRgt;
	m_pNewEdges		= pLft;
	}

//
// @MFunc Determine if the local minimum with the given left edge should contribute to a polygon
// @RDesc TRUE if this local min should contribute else FALSE
BOOLEAN RPolygonClipper::IsContributingLocalMin(
			RPolygonClipper::ActiveEdge*	pLeft,		// @Parm pointer to the left edge
			RPolygonClipper::ActiveEdge*	pRight )		// @Parm pointer to the right edge
	{
	unsigned int	clip		= pLeft->clip;
	int				nThis		= 1;
	int				nOther	= 0;
	//
	// if it's xor, the local min is contributing as long as it's
	// not completely duplicated (start point and slope)
	if( m_operation==kLogicalXor || m_operation==kLogicalDiff )
		{
#define AreSame(e1,e2)	(AreCoordsEqual(e1->xBottom,e2->xBottom) && AreDeltasEqual(e1->xIncr,e2->xIncr))
		ActiveEdge*	pNext	= GetNextAETEntry( pRight );
		if( pNext && AreSame(pLeft,pNext) )
			return FALSE;
		ActiveEdge*	pPrev	= GetPrevAETEntry( GetPrevAETEntry(pLeft) );
		if( pPrev && AreSame(pLeft,pPrev) )
			return FALSE;
#undef AreSame
		}
	while( (pLeft=GetPrevAETEntry(pLeft)) != NULL )
		{
		if( pLeft->clip == clip )
			++nThis;
		else
			++nOther;
		}
	switch( m_operation )
		{
		case kLogicalAnd:
			//
			// this edge starts the interior...
			// all local minima of a polygon that are interior to the
			// other polygon should be treated as contributing local
			// minima...
			return nOther&1;

		case kLogicalOr:
			//
			// this edge starts the interior...
			// all local minima of a polygon that are exterior to the
			// other polygon should be treated as contributing local
			// minima...
			return !(nOther&1);

		case kLogicalDiff:
			//
			// this edges starts the interior.
			// all local minima of subject polygon lying outside
			// the clip polygon should be treated as contributing
			// local minima...
			return (clip? nOther&1 : !(nOther&1));

		case kLogicalXor:
			//
			// this edge starts the interior.
			// all local minima should be treated as contributing
			// local minima...
			return TRUE;
		}
#ifdef TPSDEBUG
	TpsAssert( FALSE, "Ouch!" );
	throw kUnknownError;
#endif
	return FALSE;
	}

//
// @MFunc Get the number of polygons and total number of points generated
// @RDesc the number of polygons
//
YPolygonCount RPolygonClipper::GetCount(
			int&	nTotalPoints )	// @Parm storage for the total number of points
	{
	RPolyNode*	pnode		= m_pClipPolyNodes;
	int			nPolys	= 0;
	nTotalPoints			= 0;
	while( pnode )
		{
		if( pnode->m_nPoints>2 )
			{
			++nPolys;
			nTotalPoints += pnode->m_nPoints;
			}
		pnode = pnode->m_pNext;
		}
	return nPolys;
	}

//
// @MFunc handle intersection errors
// @RDesc nothing
//
void RPolygonClipper::IntersectionError(
		   RPolygonClipper*					pClipper,	 // @Parm pointer to the clipper
			RPolygonClipper::ActiveEdge*	pEdge1,		 // @Parm the leading intersection edge
			RPolygonClipper::ActiveEdge*	pEdge2,		 // @Parm the trailing intersection edge
			const RRealPoint&			/*pt*/ )				 // @Parm the point of intersection
	{
#if 1
	//
	// let's see if we can fix it by swapping the edges and their polygons...
	//SwapEdgePolygons( pEdge1, pEdge2 );
	pClipper;
	pEdge1;
	pEdge2;
	return;
#else
	TpsAssertAlways( "RPolygonClipper: intersection error" );
	throw kUnknownError;
#endif
	}

//
// @MFunc Process intersection of edges at a local minimum
// @RDesc nothing
//
void RPolygonClipper::ProcessLocalMinIntersection(
		   RPolygonClipper*					pClipper,	 // @Parm pointer to the clipper
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm the leading intersection edge
			RPolygonClipper::ActiveEdge*	pEdge2,	// @Parm the trailing intersection edge
			const RRealPoint&			pt )		// @Parm the point of intersection
	{
	//
	// Create a new polygon node starting at the given point with the two
	// given edges contributing to it...
	STARTFUNC( ("ProcessLocalMinIntersection( edge1=%d, edge2=%d, pt=(%.3f,%.3f) )\n",pEdge1->nEdge, pEdge2->nEdge, pt.m_x, pt.m_y) );
	if( pEdge1->pPolygon || pEdge2->pPolygon )
		{
		pClipper->AddPolyPoint( pEdge1, pt );
		#ifdef TPSDEBUG
		if( pEdge1->pPolygon )
			pEdge1->pPolygon->Dump( "ProcessLocalMinIntersection: Edge1 point added" );
		#endif
		pClipper->AddPolyPoint( pEdge2, pt );
		#ifdef TPSDEBUG
		if( pEdge2->pPolygon )
			pEdge2->pPolygon->Dump( "ProcessLocalMinIntersection: Edge2 point added" );
		#endif
		pClipper->SwapEdgePolygons( pEdge1, pEdge2 );
		ENDFUNC( ("\n") );
		return;
		}
	if( pEdge1->left )
		pClipper->CreatePolyNode( pEdge1, pEdge2, pt );
	else
		pClipper->CreatePolyNode( pEdge2, pEdge1, pt );
	#ifdef TPSDEBUG
	pEdge1->pPolygon->Dump( "ProcessLocalMinIntersection" );
	#endif
	ENDFUNC( ("\n") );
	}

//
// @MFunc Process intersection of edges at a local maximum
// @RDesc nothing
//
void RPolygonClipper::ProcessLocalMaxIntersection(
		   RPolygonClipper*					pClipper,	 // @Parm pointer to the clipper
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm the leading intersection edge
			RPolygonClipper::ActiveEdge*	pEdge2,	// @Parm the trailing intersection edge
			const RRealPoint&			pt )		// @Parm the point of intersection
	{
	//
	// Add the intersection point to one of the polygons.
	// If the polygons are different, merge them.
	// Stop the given edges from contributing further...
	STARTFUNC( ("ProcessLocalMaxIntersection( edge1=%d, edge2=%d, pt=(%.3f,%.3f) )\n",pEdge1->nEdge, pEdge2->nEdge, pt.m_x, pt.m_y) );
	pClipper->PreprocessLocalMax( pEdge1, pEdge2 );
	if( pEdge1->pPolygon )
		{
		TpsAssert( pEdge2->pPolygon != NULL, "ProcessLocalMaxIntersection: poly nopoly" );
	#ifdef TPSDEBUG
		if( pEdge2->pPolygon==NULL )
			throw kUnknownError;
	#endif
		pClipper->AddPolyPoint( pEdge1, pt );
	#ifdef TPSDEBUG
		pEdge1->pPolygon->Dump( "ProcessLocalMaxIntersection: Added Edge1 Point" );
	#endif
		pClipper->ClosePolygon( pEdge1, pEdge2 );
		}
#ifdef TPSDEBUG
	else
		{
		TpsAssert( pEdge2->pPolygon==NULL, "ProcessLocalMaxIntersection: nopoly poly" );
		if( pEdge2->pPolygon!=NULL )
			throw kUnknownError;
		}
#endif
	ENDFUNC( ("\n") );
	}

//
// @MFunc Process intersection of like sides of different polygons
// @RDesc nothing
//
void RPolygonClipper::ProcessLikeSideIntersection(
		   RPolygonClipper*					pClipper,	 // @Parm pointer to the clipper
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm the leading intersection edge
			RPolygonClipper::ActiveEdge*	pEdge2,	// @Parm the trailing intersection edge
			const RRealPoint&			pt )		// @Parm the point of intersection
	{
	STARTFUNC( ("ProcessLikeSideIntersection( edge1=%d, edge2=%d, pt=(%.3f,%.3f) )\n",pEdge1->nEdge, pEdge2->nEdge, pt.m_x, pt.m_y) );

	pClipper->AddPolyPoint( pEdge1, pt );
	#ifdef TPSDEBUG
	if( pEdge1->pPolygon ) pEdge1->pPolygon->Dump( "ProcessLikeSideIntersection: Edge1 point added" );
	#endif

	pClipper->AddPolyPoint( pEdge2, pt );
	#ifdef TPSDEBUG
	if( pEdge2->pPolygon ) pEdge2->pPolygon->Dump( "ProcessLikeSideIntersection: Edge2 point added" );
	#endif
	//
	// swap the polygon pointers
	pClipper->SwapEdgePolygons( pEdge1, pEdge2 );
	ENDFUNC( ("\n") );
	}

//
// @MFunc Process intersection of edges of the same polygon
// @RDesc nothing
//
void RPolygonClipper::ProcessLikeEdgeIntersection(
		   RPolygonClipper*					pClipper,	 // @Parm pointer to the clipper
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm the leading intersection edge
			RPolygonClipper::ActiveEdge*	pEdge2,	// @Parm the trailing intersection edge
			const RRealPoint&			pt )		// @Parm the point of intersection
	{
	pClipper->AddPolyPoint( pEdge1, pt );
	#ifdef TPSDEBUG
	if( pEdge1->pPolygon ) pEdge1->pPolygon->Dump( "ProcessLikeEdgeIntersection: Added Edge1 Point" );
	#endif

	pClipper->AddPolyPoint( pEdge2, pt );
	#ifdef TPSDEBUG
	if( pEdge2->pPolygon ) pEdge2->pPolygon->Dump( "ProcessLikeEdgeIntersection: Added Edge2 Point" );
	#endif
	//
	// swap side descriptors...
	pClipper->SwapEdgeSides( pEdge1, pEdge2 );
	//
	// swap the polygon pointers...
	pClipper->SwapEdgePolygons( pEdge1, pEdge2 );
	}

//
// @MFunc Process an intermediate intersection
// @RDesc nothing
//
void RPolygonClipper::ProcessIntermediateIntersection(
		   RPolygonClipper*					pClipper,	 // @Parm pointer to the clipper
			RPolygonClipper::ActiveEdge*	pEdge1,	// @Parm the leading intersection edge
			RPolygonClipper::ActiveEdge*	pEdge2,	// @Parm the trailing intersection edge
			const RRealPoint&			pt )		// @Parm the point of intersection
	{
	//
	// add the intersection point to both polygons
	// and swap the polygons...
	pClipper->AddPolyPoint( pEdge1, pt );
	#ifdef TPSDEBUG
	if( pEdge1->pPolygon ) pEdge1->pPolygon->Dump( "ProcessIntermediateIntersection: Added Edge1 Point" );
	#endif
	pClipper->AddPolyPoint( pEdge2, pt );
	#ifdef TPSDEBUG
	if( pEdge2->pPolygon ) pEdge2->pPolygon->Dump( "ProcessIntermediateIntersection: Added Edge2 Point" );
	#endif
	pClipper->SwapEdgePolygons( pEdge1, pEdge2 );
	}

//
// @MFunc Activate any edges in the LMT that start at the current scanbeam bottom
// @RDesc nothing
//
void RPolygonClipper::QueueEdges()
	{
	#ifdef TPSDEBUG
	int	n = 0;
	STARTFUNC( ("Queue Edges\n") );
	#endif
	m_pNewEdges	= NULL;
	while( m_pCurLMT && AreCoordsEqual(m_pCurLMT->pt.m_y,m_yb) )//m_pCurLMT->pt.m_y==m_yb )
		{
		AddEdges( m_pCurLMT );
		m_pCurLMT = m_LMT.GetNextEntry( m_pCurLMT );
		#ifdef TPSDEBUG
		++n;
		#endif
		}
	//
	// Check for new contributing local minimums...
	while( m_pNewEdges )
		{
		//
		// check each edge added along with it's adjacent edge...
		ActiveEdge*	pLeading		= m_pNewEdges;
		ActiveEdge*	pTrailing	= m_pNewEdges->pNextNew;
		if( IsContributingLocalMin(pLeading,pTrailing) )
			ProcessLocalMinIntersection( this, pLeading, pTrailing, RRealPoint(pLeading->xBottom,m_yb) );
		m_pNewEdges = pTrailing->pNextNew;
		}

	#ifdef TPSDEBUG
	if( n ) DumpAET( "Edges queued" );
	ENDFUNC( ("\n") );
	#endif
	}

//
// @MFunc Handle any horizontal edges at the given Y coordinate
// @RDesc nothing
//
void RPolygonClipper::ProcessHorizontalSegments(
			float y )	// @Parm the Y coordinate to test for horizontal segments at
	{
	//
	// deal with any intersections with horizontal edges...
	ActiveEdge*	aetEdge	= GetFirstAETEntry();
	#ifdef TPSDEBUG
	int	nprocessed = 0;
	DumpAET( "Before processing horizontal" );
	#endif
	while( aetEdge )
		{
		ActiveEdge*	aetEdgeNext	= GetNextAETEntry( aetEdge );
		if( aetEdge->horz )
			{
			CONTINUEFUNC( ("Horizontal segment %d %.3f->%.3f\n", aetEdge->nEdge, aetEdge->xBottom, aetEdge->xTop) );
			//
			// it's horizontal, so deal with any intersections...
			if( aetEdge->xBottom < aetEdge->xTop )
				{
				//
				// it's oriented left to right, so we only need to check forward...
				ActiveEdge*	aetNext	= GetNextAETEntry( aetEdge );
				while( aetNext )
					{
					ActiveEdge*	aetNextNext = GetNextAETEntry( aetNext );
					if( aetNext->horz )
						{
						//
						// it's horizontal...
						if( aetNext->xBottom<aetNext->xTop )
							{
							//
							// next is left to right
							if( aetNext->xTop < aetEdge->xTop )
								{
								//
								// intersection at aetNext->xtop...
								CONTINUEFUNC( ("Intersects with <horizontal> %d at (%.3f,%.3f)\n", aetNext->nEdge, aetNext->xTop, y) );
								HandleIntersection( aetEdge, aetNext, RRealPoint(aetNext->xTop,y) );
								}
							}
						else
							{
							//
							// next is right to left
							if( aetNext->xBottom < aetEdge->xTop )
								{
								//
								// intersection at aetNext->xBottom...
								CONTINUEFUNC( ("Intersects with <horizontal> %d at (%.3f,%.3f)\n", aetNext->nEdge, aetNext->xBottom, y) );
								HandleIntersection( aetEdge, aetNext, RRealPoint(aetNext->xBottom,y) );
								}
							}
						}
					else
						{
						//
						// not horizontal...
						float	x	= float(aetNext->xBottom+(y-m_yb)*aetNext->xIncr);
						if( x < aetEdge->xTop )
							{
							//
							// intersection...
							CONTINUEFUNC( ("Intersects with %d at (%.3f,%.3f)\n", aetNext->nEdge, x, y) );
							HandleIntersection( aetEdge, aetNext, RRealPoint(x,y) );
							}
						}
					aetNext = aetNextNext;
					}
				}
			else
				{
				//
				// it's oriented right to left, so we only need to check backward...
				ActiveEdge*	aetPrev	= GetPrevAETEntry( aetEdge );
				while( aetPrev )
					{
					ActiveEdge*	aetPrevPrev	= GetPrevAETEntry( aetPrev );
					if( !aetPrev->horz )
						{
						float	x	= float(aetPrev->xBottom+(y-m_yb)*aetPrev->xIncr);
						if( x>aetEdge->xTop )
							{
							//
							// intersection...
							CONTINUEFUNC( ("Intersects with %d at (%.3f,%.3f)\n", aetPrev->nEdge, x, y) );
//							HandleIntersection( aetEdge, aetPrev, RRealPoint(x,y) );
							HandleIntersection( aetPrev, aetEdge, RRealPoint(x,y) );
							}
						}
					aetPrev		= aetPrevPrev;
					aetPrevPrev	= GetPrevAETEntry( aetPrev );
					}
				}
			//
			// advance over the horizontal segment...
			AddPolyPoint( aetEdge, RRealPoint(aetEdge->xTop,y) );
#ifdef TPSDEBUG
			if( aetEdge->pPolygon ) aetEdge->pPolygon->Dump( "ProcessHorizontal: Added aetEdge Point" );
#endif
			if( aetEdge->pptNextTop && y==m_yb )
				{
				CONTINUEFUNC( ("Advancing %d\n", aetEdge->nEdge) );
				AdvanceAETEdge( aetEdge );
//				LinkAETEntries( GetPrevAETEntry(aetEdge), GetNextAETEntry(aetEdge) );
//				InsertAETEntry( aetEdge );
				}
			#ifdef TPSDEBUG
			++nprocessed;
			#endif
			}// edge was horizontal...
		aetEdge = aetEdgeNext;
		}
	#ifdef TPSDEBUG
	if( nprocessed )
		DumpAET( "Horizontal edges processed" );
	#endif
	}

//
// @MFunc Handle any horizontal edges at the bottom of the current scanbeam
// @RDesc nothing
//
void RPolygonClipper::PreprocessHorizontalSegments()
	{
	//
	// deal with any intersections with horizontal edges at the top
	// of the scanbeam...
	STARTFUNC( ("Preprocess Horizontal Segments\n") );
	ProcessHorizontalSegments( m_yb );
	ENDFUNC( ("\n") );
	}

//
// @MFunc Handle any horizontal edges at the top of the current scanbeam
// @RDesc nothing
//
void RPolygonClipper::PostprocessHorizontalSegments()
	{
	STARTFUNC( ("Postprocess Horizontal Segments\n") );
	//
	// deal with any intersections with horizontal edges at the top
	// of the scanbeam...
	//
	// first, advance any edges that have horizontal successors...
	ActiveEdge*	aetEdge	= GetFirstAETEntry();
	while( aetEdge )
		{
//		if( aetEdge->yMax==m_yt && aetEdge->pptNextTop && aetEdge->pptNextTop->m_y==m_yt )
		if( AreCoordsEqual(aetEdge->yMax,m_yt) && aetEdge->pptNextTop && AreCoordsEqual(aetEdge->pptNextTop->m_y,m_yt) )
			{
			//
			// successor is horizontal...
			AddPolyPoint( aetEdge, RRealPoint(aetEdge->xTop,aetEdge->yMax) );
#ifdef TPSDEBUG
			if( aetEdge->pPolygon ) aetEdge->pPolygon->Dump( "Postprocess Horizontal: Added aetEdge point" );
#endif
			AdvanceAETEdge( aetEdge );
			}
		aetEdge = GetNextAETEntry( aetEdge );
		}
	//
	// now process the edges...
	ProcessHorizontalSegments( m_yt );
	ENDFUNC( ("\n") );
	}

//
// @MFunc Compute intersections between active edges and build the intersection table
// @RDesc nothing
//
void RPolygonClipper::ComputeIntersections()
	{
	//
	// first, compute xtop's
	float			dy		= m_yt - m_yb;
	ActiveEdge*	pEdge	= GetFirstAETEntry();
	while( pEdge )
		{
		if( !pEdge->horz )
			pEdge->xTop = pEdge->xBottom + dy*pEdge->xIncr;
		pEdge	= GetNextAETEntry( pEdge );
		}
	ActiveEdge*	aetEdge	= GetFirstAETEntry();
	while( aetEdge )
		{
		float			xbot		= aetEdge->xBottom;
		float			xtop		= aetEdge->xTop;
		ActiveEdge*	aetNext	= aetEdge;
		while( (aetNext=GetNextAETEntry(aetNext)) != NULL )
			{
			//
			// compute the intersection between the two edges...
			if( IsCoordLT(aetNext->xTop,xtop) )
				{
				//
				// they intersect...
				// compute the intersection between them...
				// (paet->xbot,paet->ybot) -> (paet->xtop,paet->m_yt)
				// a->xbot+dy*a->delx = b->xbot+dy*b->delx
				// a->xbot - b->xbot = dy*b->delx - dy*a->delx
				// (a->xbot - b->xbot) / (b->delx - a->delx) = dy
				YFloatType	deltaIncr	= aetNext->xIncr - aetEdge->xIncr;
				YFloatType	yi;
				if( AreDeltasEqual(deltaIncr,YFloatType(0)) )
					{
					//
					// they're parallel (almost), so we'll force the intersection at the top
					yi	= m_yt - m_yb;
					}
				else
					{
					//
					// they're not parallel, so we can really compute the intersection...
					yi	= YFloatType( (xbot-aetNext->xBottom)/deltaIncr );
					}
				RRealPoint	ptIntersection( (xbot + yi*aetEdge->xIncr), m_yb+yi );
				if( ptIntersection.m_y<m_yb )
					{
					//
					// they intersect below the scanbeam, so they're basically parallel.
					// we'll force the intersection at the bottom...
					ptIntersection.m_y	= m_yb;
					ptIntersection.m_x	= xbot;
					}
				else if( ptIntersection.m_y>m_yt )
					{
					//
					// they intersect above the scanbeam, so they're basically parallel.
					// we'll force the intersection at the top...
					ptIntersection.m_y	= m_yt;
					ptIntersection.m_x	= xtop;
					}
				//
				// add the intersection to the intersection table
				AddIntersection( aetEdge, aetNext, ptIntersection );
				}
			}
		aetEdge = GetNextAETEntry( aetEdge );
		}

	#ifdef TPSDEBUG
	if( m_pIT )
		DumpAET( "Intersections Computed" );
	#endif
	}

//
// @MFunc Update polygons from intersections and edge ends in the active edge table
// @RDesc nothing
//
void RPolygonClipper::UpdatePolygons()
	{
	STARTFUNC( ("UpdatePolygons\n") );

	//
	// Preprocess horizontal segments...
	if( m_nHorzEdges )
		PreprocessHorizontalSegments();

	//
	// Compute the intersections...
	ComputeIntersections();
	STARTFUNC( ("Process Intersections\n") );
	//
	// Process the intersections...
	RITEntry*	pit	= GetFirstITEntry();
	int			n = 0;
	while( pit )
		{
		CONTINUEFUNC( ("Intersection of %d and %d at (%.3f,%.3f)\n", pit->m_pEdge1->nEdge, pit->m_pEdge2->nEdge, pit->m_x, pit->m_y) );
		HandleIntersection( pit->m_pEdge1, pit->m_pEdge2, *pit );
		//
		// on to the next intersection...
		pit = GetNextITEntry( pit );
		}
	//
	// done with the intersection table...
	DeleteIT();

	#ifdef TPSDEBUG
	DumpAET("Intersections processed");
	ENDFUNC( ("\n") );
	#endif
	//
	// Postprocess any horizontal segments...
	PostprocessHorizontalSegments();

	STARTFUNC( ("Process Segment Ends\n") );

	//
	// process any edges that end at the top of the scanbeam...
	ActiveEdge*	pEdge	= GetFirstAETEntry();
	while( pEdge )
		{
		ActiveEdge*	pEdge1	= pEdge;
		ActiveEdge*	pEdge2	= GetNextAETEntry( pEdge );
		//
		// for all edges in the AET...
		if( IsCoordLT(pEdge1->yMax,m_yt) || AreCoordsEqual(pEdge1->yMax,m_yt) )//pEdge1->yMax <= m_yt )
			{
			//
			// edge terminates at top of scanbeam...
			pEdge = HandleEndOfSegment( pEdge1, pEdge2 );
			#ifdef TPSDEBUG
			DumpAET( "Edge terminated" );
			#endif
			}// edge ends here
		else
			{
			//
			// update xbot...
			if( !pEdge->horz )
				pEdge->xBottom = pEdge->xTop;
			pEdge	= pEdge2;
			}
		}// for all entries in AET
	#ifdef TPSDEBUG
	ENDFUNC( ("\n") );
	DumpAET( "Polygons Updated" );
	ENDFUNC( ("\n") );
	#endif
	}


//
// @MFunc Construct an RPolygonClipper for the given clip and subject polygons(LMT's)
// @RDesc nothing
//
RPolygonClipper::RPolygonClipper(
			RClipPolygon&	lmtClip,
			RClipPolygon&	lmtSubject ) :
#ifdef TPSDEBUG
		m_nEdge( 0 ),
		m_ClipPolygon( lmtClip ),
		m_SubjectPolygon( lmtSubject ),
#endif
		m_pfnHandleIntersection( NULL ),
		m_LMT(),
		m_pAET( NULL ),
		m_pNewEdges( NULL ),
		m_pSBT( NULL ),
		m_pClipPolyNodes( NULL ),
		m_operation( kLogicalAnd ),
		m_pCurSBT( NULL ),
		m_pCurLMT( NULL ),
		m_yb( 0.0f ),
		m_yt( 0.0f ),
		m_pIT( NULL ),
		m_nHorzEdges( 0 )
	{
	lmtClip.Reinit( TRUE );
	lmtSubject.Reinit( FALSE );
#ifdef TPSDEBUG
	CONTINUEFUNC( ("Clip polygon\n") );
	lmtClip.Dump();
	CONTINUEFUNC( ("Subject polygon\n") );
	lmtSubject.Dump();
#endif
	m_LMT.Reinit( FALSE );
	m_LMT.MergeGlobal( lmtClip.GetFirstEntry() );
	m_LMT.MergeGlobal( lmtSubject.GetFirstEntry() );
#ifdef TPSDEBUG
	CONTINUEFUNC( ("Merged LMT\n") );
	m_LMT.Dump();
#endif
	InitSBT();
	}

//
// @MFunc Construct an RPolygonClipper for the given clip and subject polygons(LMT's)
// @RDesc nothing
//
RPolygonClipper::RPolygonClipper(
			RClipPolygon&	lmt ) :
#ifdef TPSDEBUG
		m_nEdge( 0 ),
		m_ClipPolygon( lmt ),
      m_SubjectPolygon( lmt ),
#endif
		m_pfnHandleIntersection( NULL ),
		m_LMT(),
		m_pAET( NULL ),
		m_pNewEdges( NULL ),
		m_pSBT( NULL ),
		m_pClipPolyNodes( NULL ),
		m_operation( kLogicalOr ),
		m_pCurSBT( NULL ),
		m_pCurLMT( NULL ),
		m_yb( 0.0f ),
		m_yt( 0.0f ),
		m_pIT( NULL ),
		m_nHorzEdges( 0 )
	{
	lmt.Reinit( FALSE );
#ifdef TPSDEBUG
	CONTINUEFUNC( ("polygon\n") );
	lmt.Dump();
#endif
	m_LMT.Reinit( FALSE );
	m_LMT.MergeGlobal( lmt.GetFirstEntry() );
#ifdef TPSDEBUG
	CONTINUEFUNC( ("Merged LMT\n") );
	m_LMT.Dump();
#endif
	InitSBT();
	}

//
// @MFunc Destruct this RPolygonClipper
// @RDesc nothing
//
RPolygonClipper::~RPolygonClipper()
	{
	DeleteIT();
	DeleteSBT();
	DeleteAET();
	DeletePolyNodes();
	}

#ifdef TPSDEBUG
void RPolygonClipper::DumpAET( char* why )
{
	ActiveEdge* pEdge = GetFirstAETEntry();
	STARTFUNC( ("Active Edge Table: %s\n", why) );
	CONTINUEFUNC( ("  edge    xbot    xtop    ytop   polyname left right next top\n") );
	CONTINUEFUNC( (" ------- ------- ------- ------- -------- ---- ----- --------\n") );

	while( pEdge )
		{
		CONTINUEFUNC( ("  %c%c%3d  %7.3f %7.3f %7.3f %-8.8s  %3d   %3d  (%7.3f,%7.3f)\n",
					pEdge->left? 'L':'R',
					pEdge->clip? 'C':'S',
					pEdge->nEdge,
					pEdge->xBottom,
					pEdge->xTop,
					pEdge->yMax,
					pEdge->pPolygon? (LPCTSTR)pEdge->pPolygon->m_Name : "",
					pEdge->pPolygon? (pEdge->pPolygon->m_pLeftEdge->nEdge) : -1,
					pEdge->pPolygon? (pEdge->pPolygon->m_pRightEdge->nEdge) : -1,
					pEdge->pptNextTop? pEdge->pptNextTop->m_x : -999.999f,
					pEdge->pptNextTop? pEdge->pptNextTop->m_y : -999.999f) );
		pEdge = GetNextAETEntry( pEdge );
		}
	ENDFUNC( ("\n") );
}
#endif

//
// Public APIs
//

//
// @MFunc Initialize the polygon generation
void RPolygonClipper::StartPolygonGeneration( EOperation eop )
	{
	m_operation		= eop;
	m_nHorzEdges	= 0;
	switch( m_operation )
		{
		case kLogicalAnd:
			m_pfnHandleIntersection	= g_pfnHandleIntAND;
			break;
		case kLogicalOr:
			m_pfnHandleIntersection	= g_pfnHandleIntOR;
			break;
		case kLogicalXor:
			m_pfnHandleIntersection	= g_pfnHandleIntXOR;
			break;
		case kLogicalDiff:
			m_pfnHandleIntersection	= g_pfnHandleIntDIFF;
			break;
		default:
			TpsAssert( FALSE, "Invalid operation" );
			return;
		}

	m_pCurSBT	= GetFirstSBTEntry();
	m_pCurLMT	= m_LMT.GetFirstEntry();
	m_yt			= (m_pCurSBT? m_pCurSBT->y : float(0));
	}

//
// @MFunc Process one scanbeam (returns FALSE when done, TRUE if more needs to be done)
BOOL RPolygonClipper::ContinuePolygonGeneration()
	{
	m_pCurSBT	= GetNextSBTEntry( m_pCurSBT );
	if( m_pCurSBT )
		{
		m_yb			= m_yt;
		m_yt			= m_pCurSBT->y;
		STARTFUNC( ("Scanbeam %.3f->%.3f\n",m_yb,m_yt) );
		QueueEdges();
		UpdatePolygons();
		ENDFUNC( ("\n",m_yb,m_yt) );
		return TRUE;
		}
	return FALSE;
	}

//
// @MFunc End polygon generation
void RPolygonClipper::EndPolygonGeneration( int& nPolygons, int& nTotalPoints )
	{
	nPolygons = GetCount( nTotalPoints );
	}

//
// @MFunc Cancel polygon generation
void RPolygonClipper::CancelPolygonGeneration()
	{
	m_pCurSBT	= NULL;
	m_pCurLMT	= NULL;
	DeleteIT();
	DeleteAET();
	DeletePolyNodes();
	}



//
// @MFunc Generate the polygons for the given operation
// @RDesc nothing
//
void RPolygonClipper::GeneratePolygons(
			RPolygonClipper::EOperation	eop,				// @Parm how the polygons should be clipped
			YPolygonCount&						nPolygons,		// @Parm storage for the number of polygons generated
			YPointCount&						nTotalPoints )	// @Parm storage for the total number of points generated
	{
	try
		{
		StartPolygonGeneration( eop );
		while( ContinuePolygonGeneration() )
			;
		EndPolygonGeneration( nPolygons, nTotalPoints );
		}
	catch( ... )
		{
#ifdef TPSDEBUG
		if( clipdebugindent<0 )
			{
			int	nOldIndent	= clipdebugindent;
			clipdebugindent = 0;
			STARTFUNC( ("Clip Polygon\r\n") );
			m_ClipPolygon.Dump();
			ENDFUNC( ("\r\n") );
			STARTFUNC( ("Subject Polygon\r\n") );
			m_SubjectPolygon.Dump();
			ENDFUNC( ("\r\n") );
			clipdebugindent = nOldIndent;
			}
#endif
		throw;
		}
	}

//
// @MFunc Store the polygons into the given polypolygon
// @Syntax StorePolygons( RRealPoint* pPoints, YPointCount* pCounts )
// @Syntax StorePolygons( RIntPoint* pPoints, YPointCount* pCounts )
// @Parm RRealPoint* | pPoints | storage for REAL points making up the resultant polypolygon
// @Parm RIntPoint* | pPoints | storage for INTEGER points making up the resultant polypolygon
// @Parm int* | pCounts | storage for the point counts
// @RDesc the number of polygons in the polypolygon
//
int RPolygonClipper::StorePolygons( RRealPoint* pPoints, int* pCounts )
	{
	RPolyNode*	pnode		= m_pClipPolyNodes;
	int			nPolys	= 0;
	while( pnode )
		{
		if( pnode->m_nPoints > 2 )
			{
			RPolyNode*	pLeft		= pnode->LeftmostNode();
			int			nPoints	= 0;
			int			nNodes	= 0;
			while( pLeft )
				{
				RPolyPoint*	ppt	= pLeft->m_pLeft;
				while( ppt )
					{
					++nPoints;
					*pPoints++ = *ppt;
					ppt = ppt->m_pNext;
					}
				pLeft = pLeft->m_pNodeRight;
				}
			TpsAssert( nPoints==pnode->m_nPoints, "Node is lying about the number of points it contains!" );
			*pCounts++ = nPoints-nNodes;
			++nPolys;
			}
		pnode = pnode->m_pNext;
		}
	return nPolys;
	}
int RPolygonClipper::StorePolygons( RIntPoint* pPoints, YPointCount* pCounts )
	{
	RPolyNode*	pnode		= m_pClipPolyNodes;
	int			nPolys	= 0;
	while( pnode )
		{
		if( pnode->m_nPoints > 2 )
			{
			RPolyNode*	pLeft		= pnode->LeftmostNode();
			RPolyPoint*	ppt		= pLeft->m_pLeft;
			*pPoints					= RIntPoint(*ppt);
			int			nPoints	= 1;
			while( pLeft )
				{
				while( ppt )
					{
					RIntPoint	pt( *ppt );
					if( *pPoints != pt )
						{
						++nPoints;
						*++pPoints = pt;
						}
					ppt = ppt->m_pNext;
					}
				pLeft = pLeft->m_pNodeRight;
				if( pLeft )
					ppt = pLeft->m_pLeft;
				}
			++pPoints;
			if( nPoints>2 )
				{
				*pCounts++ = nPoints;
				++nPolys;
				}
			else
				pPoints -= nPoints;
			}
		pnode = pnode->m_pNext;
		}
	return nPolys;
	}


//
// @MFunc Append polygon node points to the clip polygon
// @RDesc TRUE if successful
//
BOOLEAN RClipPolygon::AppendPolyNodePoints( RPolyPoint* pPoints )
	{
	//
	// Create an LMT entry from the given polynode
	// first, find the minimum point...
	RPolyPoint*	pptMin		= pPoints;
	RPolyPoint* ppt			= pptMin->m_pNext;
	//
	// find the local min and remove extraneous points on
	// horizontal or vertical segments...
	RPolyPoint*	pptPrev		= NULL;
	RPolyPoint*	pptPrevPrev	= NULL;
	while( ppt )
		{
		if( pptPrevPrev )
			{
			if( (ppt->m_y==pptPrev->m_y && pptPrev->m_y==pptPrevPrev->m_y) ||
				 (ppt->m_x==pptPrev->m_x && pptPrev->m_x==pptPrevPrev->m_x) )
				{
				//
				// pptPrev is not necessary...
				TpsAssert( pptMin != pptPrev, "Chose second point at given Y for local min" );
				delete pptPrev;
				pptPrev				= pptPrevPrev;
				pptPrev->m_pNext	= ppt;
				}
			}
		if( ppt->m_y < pptMin->m_y )
			pptMin = ppt;
		else if( pptMin==pPoints && AreCoordsEqual(ppt->m_y,pptMin->m_y) )
			pptMin = ppt;
		pptPrevPrev	= pptPrev;
		pptPrev		= ppt;
		ppt			= ppt->m_pNext;
		}
	TpsAssert( pptMin!=pPoints, "Minimum at leftmost end" );
	if( !pptMin->m_pNext )
		{
		//
		// we can't add this one, it belongs attached to the next one...
		return FALSE;
		}
	//
	// make sure we
	//
	// Now create an entry...
	LMTEntry*	pLocalMin	= new LMTEntry;
#ifdef TPSDEBUG
	pLocalMin->pPolygon		= this;
#endif
	pLocalMin->pt	= *pptMin;
	ppt				= pPoints;
	pptPrev			= NULL;
	while( ppt != pptMin )
		{
		RPolyPoint*	pptNext	= ppt->m_pNext;
		ppt->m_pNext			= pptPrev;
		pptPrev					= ppt;
		ppt						= pptNext;
		}
	//
	// we're at pptMin, so skip over it...
	ppt	= ppt->m_pNext;
	//
	// now store the left and right pointers...
	if( ppt->m_x < pptPrev->m_x )
		{
		pLocalMin->pLeft	= ppt;
		pLocalMin->pRight	= pptPrev;
		}
	else
		{
		pLocalMin->pLeft	= pptPrev;
		pLocalMin->pRight	= ppt;
		}
	//
	// delete the min point (it's no longer needed)
	delete pptMin;
	pLocalMin->pNextLocal	= NULL;
	pLocalMin->pNextGlobal	= NULL;
	InsertLocal( m_pLMTLocal, pLocalMin );
	return TRUE;
	}
//
// @MFunc Store the polygons into the given clippolygon
// @RDesc nothing
//
void RPolygonClipper::StorePolygons( RClipPolygon& poly )
{
	RPolyNode*	pnode		= m_pClipPolyNodes;
	poly.Empty();
	while( pnode )
		{
		if( pnode->m_nPoints>2 )
			{
			RPolyNode*	pLeft		= pnode->LeftmostNode();
			RPolyNode*	pRight	= pnode->RightmostNode();
			if( *pLeft->m_pLeft != *pRight->m_pRight )
				{
				//
				// end point needs to be duplicated for LMT entries...
				if( pLeft->m_pLeft->m_y < pRight->m_pRight->m_y )
					pnode->AddLeft( *pRight->m_pRight );
				else
					pnode->AddRight( *pLeft->m_pLeft );
				}
			while( pLeft )
				{
				//
				// Add the node to the clippolygon...
				if( pLeft->m_pNodeRight && *pLeft->m_pRight!=*pLeft->m_pNodeRight->m_pLeft )
					{
					//
					// they need to match...
					if( pLeft->m_pRight->m_y < pLeft->m_pNodeRight->m_pLeft->m_y )
						{
						//
						// add it to pLeft->m_pRight
						if( pLeft->AddRightPoint( *pLeft->m_pNodeRight->m_pLeft ) )
							++(pnode->m_nPoints);
						}
					else
						{
						//
						// add it to pLeft->m_pNodeRight->m_pLeft
						if( pLeft->m_pNodeRight->AddLeftPoint(*pLeft->m_pRight) )
							++(pnode->m_nPoints);
						}
					}
				if( !poly.AppendPolyNodePoints( pLeft->m_pLeft ) )
					{
					TpsAssert( pLeft->m_pNodeRight!=NULL, "Invalid polynode" );
					pLeft->m_pRight->m_pNext			= pLeft->m_pNodeRight->m_pLeft;
					pLeft->m_pNodeRight->m_pLeft		= pLeft->m_pLeft;
					pLeft->m_pNodeRight->m_nPoints	+= pLeft->m_nPoints;
					}
				//
				// remove the points from the node...
				pLeft->m_pLeft	= pLeft->m_pRight	= NULL;
				pLeft->m_nPoints	= 0;
				pLeft	= pLeft->m_pNodeRight;
				}
			}
		pnode = pnode->m_pNext;
		}
#ifdef TPSDEBUG
	STARTFUNC( ("Generated RClipPolygon\n") );
	poly.m_pLMTGlobal	= poly.m_pLMTLocal;
	poly.Dump();
	ENDFUNC( ("\n") );
#endif
}
