
#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
//		To not overload the new and delete operators, comment out
//		the following define.
#define CUSTOMIZE_NEW 1

#ifdef	CUSTOMIZE_NEW
	#define	OVERLOAD_NEW						\
		public:										\
			void*	operator new ( size_t t );	\
			void  operator delete ( void* );
#else
	#define	OVERLOAD_NEW
#endif

class RPolyPoint : public RPoint<YFloatType>
{
public:
	RPolyPoint*	m_pNext;
	RPolyPoint( const RRealPoint& pt ) :
		RPoint<YFloatType>( pt ),
		m_pNext( NULL )
		{
		}
	//	Macro defined above based on CUSTOMIZE_NEW value
OVERLOAD_NEW
};


class RClipPolygon
{
//
// @Access Public types
public:
	//
	// @Struct Container for a local minima entry
	class LMTEntry
		{
		public:
			RRealPoint		pt;				// @Field the bottom point of the local minima
			unsigned int	clip:1;			// @Field TRUE if this entry is from a clip polygon else FALSE (from subject)
			RPolyPoint*		pLeft;			// @Field pointer to the next point in the left edge
			RPolyPoint*		pRight;			// @Field pointer to the next point in the right edge
#ifdef TPSDEBUG
			RClipPolygon*	pPolygon;		// @Field pointer to the parent polygon (debug only)
#endif
			LMTEntry*		pNextLocal;		// @Field pointer to the next entry in the local table
			LMTEntry*		pNextGlobal;	// @Field pointer to the next entry in the global (clip) table

	//	Macro defined above based on CUSTOMIZE_NEW value
OVERLOAD_NEW
		};

//
// @Access implementation data
private:
	//
	// @CMember pointer to the head of the local LMT
	LMTEntry*	m_pLMTLocal;
	//
	// @CMember pointer to the head of the global (clip) LMT
	LMTEntry*	m_pLMTGlobal;
#ifdef TPSDEBUG
	const RIntPoint*	m_pptInt;
	const RRealPoint*	m_pptReal;
	const int*			m_pcnt;
	int					m_npoly;
#endif

//
// @Access Implementation support
protected:
	//
	// @CMember Build an edge a local minima
	const RRealPoint*		BuildEdge( RPolyPoint* pPrevPolyPoint, const RRealPoint* pPrevSrcPoint, const RRealPoint* pLastSrcPoint, const RRealPoint*	pSrcPoints, int nPoints, BOOLEAN fBackward );
	//
	// @CMember Build an edge a local minima
	const RIntPoint*		BuildEdge( RPolyPoint* pPrevPolyPoint, const RIntPoint* pPrevSrcPoint, const RIntPoint* pLastSrcPoint, const RIntPoint*	pSrcPoints, int nPoints, BOOLEAN fBackward );
	//
	// @CMember Remove any empty edges from the table
	LMTEntry*	ReduceLocalTable( LMTEntry* pLocalTable );
	//
	// @CMember Remove any redundant segments of the given edge
	void			ReduceEdge( RRealPoint* pptPrev, RPolyPoint** ppptNext );
	//
	// @CMember Insert an entry into the local table
	void			InsertLocal( LMTEntry*& pHead, LMTEntry* pEntry );
	//
	// @CMember Insert an entry into the global (clip) table
	void			InsertGlobal( LMTEntry*& pHead, LMTEntry* pEntry );
	//
	// @CMember Merge the given table into this one's clip table
	void			MergeGlobal( LMTEntry* pOtherHead );
	//
	// @CMember Merge the given table into this one's local table
	void			MergeLocal( LMTEntry*& pOtherHead );
	//
	// @CMember Destroy a local minima edge
	void			DestroyEdge( RPolyPoint*& pPoint );
	//
	// @CMember Destroy an LMT entry
	void			DestroyEntry( LMTEntry*& pLocalMin );
	//
	// @CMember Destroy the entire local table
	void			DestroyTable();
	//
	// @CMember Reinitialize the global(clip) pointers and assign the entry type
	void			Reinit( BOOLEAN fClip );
	//
	// @CMember Create a simple table (single polygon)
	LMTEntry*	CreateTable( const RRealPoint* pPoints, int nPoints );
	//
	// @CMember Create a simple table (single polygon)
	LMTEntry*	CreateTable( const RIntPoint* pPoints, int nPoints );
	//
	// @CMember Create a complex table (poly polygon)
	void			CreatePolyTable( const RRealPoint* pPoints, const int* pcntPoints, int nPolys );
	//
	// @CMember Create a complex table (poly polygon)
	void			CreatePolyTable( const RIntPoint* pPoints, const int* pcntPoints, int nPolys );

protected:
	//
	// @CMember append polygon node points
	BOOLEAN		AppendPolyNodePoints( RPolyPoint* pPoints );

//
// @Access Construction/Destruction
public:
	//
	// @CMember Construct an empty LMT
	RClipPolygon();
	//
	// @CMember Construct a simple LMT (single polygon)
	RClipPolygon( const RRealPoint* pPoints, int nPoints );
	//
	// @CMember Construct a simple LMT (single polygon)
	RClipPolygon( const RIntPoint* pPoints, int nPoints );
	//
	// @CMember Construct a complex LMT (poly polygon)
	RClipPolygon( const RRealPoint* pPoints, const int* pnPoints, int nPolys );
	//
	// @CMember Construct a complex LMT (poly polygon)
	RClipPolygon( const RIntPoint* pPoints, const int* pnPoints, int nPolys );
	//
	// @CMember Destruct an RClipPolygon
	~RClipPolygon();

//
// @Access Extended construction
public:
	//
	// @CMember Append the given polypolygon to this clippolygon
	void	AppendPolyPolygon( const RRealPoint* pPoints, const int* pnCounts, int nPolys );
	void	AppendPolyPolygon( const RIntPoint* pPoints, const int* pnCounts, int nPolys );
	//
	// @CMember Append the given polygon to this clippolygon
	void	AppendPolygon( const RRealPoint* pPoints, int nPoints );
	void	AppendPolygon( const RIntPoint* pPoints, int nPoints );
	//
	// @CMember Extract the polygons from this RClipPolygon
	int	ExtractPolygons( int** pCounts, RRealPoint** pPoints );
	int	ExtractPolygons( int** pCounts, RIntPoint** pPoints );

	//
	// @CMember Empty this clippolygon
	void	Empty();

//
// @Access Simplified operators
public:
	//
	// @CMember OR the given polygon onto this one
	const RClipPolygon& operator|=( const RClipPolygon& rhs );
	//
	// @CMember Subtract the given polygon from this one
	const RClipPolygon& operator-=( const RClipPolygon& rhs );
	//
	// @CMember XOR the given polygon onto this one
	const RClipPolygon& operator^=( const RClipPolygon& rhs );
	//
	// @CMember AND this polygon with the given one
	const RClipPolygon& operator&=( const RClipPolygon& rhs );

//
// @Access Iteration
protected:
	//
	// @CMember Get the first entry
	LMTEntry*	GetFirstEntry() const;
	//
	// @CMember Get the next entry
	LMTEntry*	GetNextEntry( LMTEntry* pCur ) const;
	friend class RPolygonClipper;

#ifdef TPSDEBUG
public:
	void	Dump() const;
#endif
};


class RPolygonClipper
{
public:
	enum EOperation
		{
		kLogicalAnd,
		kLogicalOr,
		kLogicalXor,
		kLogicalDiff
		};

protected:

	enum EVertexClass
		{
		kLocalMaxVert,
		kLeftIntermediate,
		kRightIntermediate
		};

protected:
#ifdef TPSDEBUG
	RClipPolygon&	m_ClipPolygon;
	RClipPolygon&	m_SubjectPolygon;
#endif
	class RPolyNode;

	//
	// @Struct Container for an active edge entry
	struct ActiveEdge
		{
		unsigned int	horz:1;		// @Field TRUE if this edge is horizontal else FALSE
		unsigned int	left:1;		// @Field TRUE if this is a left edge else FALSE
		unsigned int	clip:1;		// @Field TRUE if this edge is from a clip polygon else FALSE (from subject)
#ifdef TPSDEBUG
		int				nEdge;		// @Field edge number (debug only)
#endif
		float				xBottom;		// @Field the X coordinate at the bottom of this edge segment
		float				xTop;			// @Field the X coordinate at the current top of this edge segment
		float				xIncr;		// @Field the X increment between unit Y rows
		float				yMax;			// @Field the Y coordinate at the extreme top of this edge segment
		RPolyPoint*		pptNextTop;	// @Field pointer to the point at the top of the next segment of this edge
		RPolyNode*		pPolygon;	// @Field pointer to the polygon to which this edge is contributing
		ActiveEdge*		pNextNew;	// @Field pointer to the next newly inserted edge
		ActiveEdge*		paePrev;		// @Field pointer to the previous ActiveEdge entry
		ActiveEdge*		paeNext;		// @Field pointer to the next ActiveEdge entry
		};

	typedef void	(*IntersectionHandler)(RPolygonClipper*,ActiveEdge*,ActiveEdge*,const RRealPoint&);
	static IntersectionHandler	g_pfnHandleIntAND[16];
	static IntersectionHandler	g_pfnHandleIntOR[16];
	static IntersectionHandler	g_pfnHandleIntXOR[16];
	static IntersectionHandler	g_pfnHandleIntDIFF[16];

	//
	// @Class Container for a clip polygon node
	class RPolyNode
		{
		//
		// @Access Implementation data
		protected:
			//
			// @CMember the number of points in this node
			int				m_nPoints;
			//
			// @CMember pointer to the left(first) point in the node
			RPolyPoint*		m_pLeft;
			//
			// @CMember pointer to the right(last) point in the node
			RPolyPoint*		m_pRight;
			//
			// @CMember pointer to the previous right point in the node
			RPolyPoint*		m_pPrevRight;
			//
			// @CMember pointer to the edge currently contributing to the left of the node
			ActiveEdge*		m_pLeftEdge;
			//
			// @CMember pointer to the edge currently contributing to the right of the node
			ActiveEdge*		m_pRightEdge;
			//
			// @CMember pointer to the next node
			RPolyNode*		m_pNext;
			//
			// @CMember pointer to continued node on the left
			RPolyNode*		m_pNodeLeft;
			//
			// @CMember pointer to continued node on the right
			RPolyNode*		m_pNodeRight;

#ifdef TPSDEBUG
			//
			// @CMember counter for the number of nodes created (use for naming)
			static int		m_gnPolys;
			//
			// @CMember the name of this node (automatically generated)
			CString			m_Name;
#endif
		//
		// @Access Construction/Destruction
		public:
			//
			// @CMember Construct an RPolyNode starting at the given point with the given edges contributing
			RPolyNode( ActiveEdge* pLeft, ActiveEdge* pRight, const RRealPoint& pt );
			//
			// @CMember Destruct this RPolyNode
			~RPolyNode();
		//
		// @Access Implementation
		public:
			//
			// @CMember return a pointer to the leftmost node
			RPolyNode*	LeftmostNode();
			//
			// @CMember return a pointer to the rightmost node
			RPolyNode*	RightmostNode();
			//
			// @CMember trim the leftmost node of the given polynode if it's empty
			static void	TrimLeftnode( RPolyNode*& pNode );
			//
			// @CMember trim the rightmost node of the given polynode if it's empty
			static void TrimRightnode( RPolyNode*& pNode );
			//
			// @CMember determine if this node can be appended
			BOOLEAN		CanAppendTo( const RPolyPoint* ppt, int* pnPointsInNode );
			//
			// @CMember determine if this node can be prepended
			BOOLEAN		CanPrependTo( const RPolyPoint* ppt, int* pnPointsInNode );
			//
			// @CMember Remove the leftmost point
			void			RemoveLeft();
			//
			// @CMember Add the given point to the left side of this polynode
			void			AddLeft( const RRealPoint& pt );
			//
			// @CMember Add the given point to the left of this node
			BOOLEAN		AddLeftPoint( const RRealPoint& pt );
			//
			// @CMember Add the given point to the right side of this node
			void			AddRight( const RRealPoint& pt );
			//
			// @CMember Add the given point to the right of this node
			BOOLEAN		AddRightPoint( const RRealPoint& pt );
			//
			// @CMember Append the given node to this one
			void			AppendPolyNode( RPolyNode* pNode );
			//
			// @CMember Prepend the given node onto this one
			void			PrependPolyNode( RPolyNode* pNode );
			//
			// @CMember Reverse the direction of this node
			void			Reverse();
#ifdef TPSDEBUG
		public:
			void			Dump( char* why ) const;
			void			Validate() const;
#endif
		friend RPolygonClipper;
		};

	//
	// @Struct Container for entries in the scan beam table
	class SBTEntry
		{
		public:
			float			y;			// @Field the Y coordinate of the scanline
			SBTEntry*	pNext;	// @Field pointer to the next entry

	//	Macro defined above based on CUSTOMIZE_NEW value
OVERLOAD_NEW
		};

	//
	// @Class Container for an entry in the intersection table
	class RITEntry : public RPoint<YFloatType>
		{
		//
		// @Access Implementation data
		public:
			//
			// @CMember pointer to the leading intersection edge
			ActiveEdge*	m_pEdge1;
			//
			// @CMember pointer to the trailing intersection edge
			ActiveEdge*	m_pEdge2;
			//
			// @CMember pointer to the next entry in the intersection table
			RITEntry*	m_pNext;
		//
		// @Access Construction
		public:
			//
			// Construct an intersectio table entry
			RITEntry( ActiveEdge* pEdge1, ActiveEdge* pEdge2, const RRealPoint& pt ) :
				RPoint<YFloatType>( pt ),
				m_pEdge1( pEdge1 ),
				m_pEdge2( pEdge2 ),
				m_pNext( NULL )
				{
				}
		};

//
// @Access Implementation data
protected:
#ifdef TPSDEBUG
	int									m_nEdge;
#endif
	//
	// @CMember pointer to the intersection handler dispatch array
	IntersectionHandler*				m_pfnHandleIntersection;
	//
	// @CMember the local minima table
	RClipPolygon					m_LMT;
	//
	// @CMember pointer to the head of the active edge table
	ActiveEdge*							m_pAET;
	//
	// @CMember pointer to the head of the newly inserted active edge table
	ActiveEdge*							m_pNewEdges;
	//
	// @CMember pointer to the head of the scanbeam table
	SBTEntry*							m_pSBT;
	//
	// @CMember pointer to the head of the polygon node table
	RPolyNode*							m_pClipPolyNodes;
	//
	// @CMember the current clipping operation
	EOperation							m_operation;
	//
	// @CMember pointer to the current entry in the scanbeam table
	SBTEntry*							m_pCurSBT;			// current scanbeam entry
	//
	// @CMember pointer to the current entry in the LMT
	RClipPolygon::LMTEntry*	m_pCurLMT;
	//
	// @CMember the bottom of the current scanbeam
	float									m_yb;
	//
	// @CMember the top of the current scanbeam
	float									m_yt;
	//
	// @CMember pointer to the head of the intersection table
	RITEntry*							m_pIT;
	//
	// @CMember the number of horizontal edges currently in the active edge table
	int									m_nHorzEdges;

//
// @Access Internal implementation
private:
	//
	// @CMember Add an intersection to the intersection table
	void				AddIntersection( ActiveEdge* pEdge1, ActiveEdge* pEdge2, const RRealPoint& pt );
	//
	// @CMember Get the first intersection table entry
	RITEntry*		GetFirstITEntry();
	//
	// @CMember Get the next intersection table entry
	RITEntry*		GetNextITEntry( RITEntry* pent );
	//
	// @CMember Delete the intersection table
	void				DeleteIT();
	//
	// @CMember Initialize the ScanBeam table
	void				InitSBT();
	//
	// @CMember Get the first entry in the ScanBeam table
	SBTEntry*		GetFirstSBTEntry();
	//
	// @CMember Get the next entry in the ScanBeam table
	SBTEntry*		GetNextSBTEntry( SBTEntry* pent );
	//
	// @CMember Delete the ScanBeam table
	void				DeleteSBT();
	//
	// @CMember Insert an entry into the scanbeam table
	void				InsertSBTEntry( float y );
	//
	// @CMember Link the given ActiveEdge entries
	void				LinkAETEntries( ActiveEdge* pLeft, ActiveEdge* pRight );
	//
	// @CMember Swap the given ActiveEdge entries in the table
	void				SwapAETEntries( ActiveEdge* pEdge1, ActiveEdge* pEdge2 );
	//
	// @CMember Advance the given ActiveEdge to the next segment
	void				AdvanceAETEdge( ActiveEdge* pEdge );
	//
	// @CMember Create a new ActiveEdge
	ActiveEdge*		CreateAETEntry( const RRealPoint& ptBottom, RPolyPoint* pptTop, unsigned int clip );
	//
	// @CMember Insert an ActiveEdge into the table
	void				InsertAETEntry( ActiveEdge* pEdge );
	//
	// @CMember Remove an ActiveEdge from the table
	void				RemoveAETEntry( ActiveEdge* pEdge );
	//
	// @CMember Delete the entire active edge table
	void				DeleteAET();
	//
	// @CMember Get the first entry in the ActiveEdge table
	ActiveEdge*		GetFirstAETEntry();
	//
	// @CMember Get the next entry in the ActiveEdge table
	ActiveEdge*		GetNextAETEntry( ActiveEdge* pEdge );
	//
	// @CMember Get the previous entry in the ActiveEdge table
	ActiveEdge*		GetPrevAETEntry( ActiveEdge* pEdge );
	//
	// @CMember Create a polygon node and add it to the table
	void				CreatePolyNode( ActiveEdge* pLeft, ActiveEdge* pRight, const RRealPoint& pt );
	//
	// @CMember Delete the polygon node table
	void				DeletePolyNodes();
	//
	// @CMember Get the first polygon node
	RPolyNode*		GetFirstPolyNode();
	//
	// @CMember Get the next polygon ndoe
	RPolyNode*		GetNextPolyNode( RPolyNode* pNode );
	//
	// @CMember Correct the ActiveEdge table prior to processing a local max
	void				PreprocessLocalMax( ActiveEdge* aetEdge, ActiveEdge* aetOther );
	//
	// @CMember Top level intersection handler
	void				HandleIntersection( ActiveEdge* edge1, ActiveEdge* edge2, const RRealPoint& pt );
	//
	// @CMember Handle the end of a segment in an edge
	ActiveEdge*		HandleEndOfSegment( ActiveEdge* pEdge1, ActiveEdge* pEdge2 );
	//
	// @CMember Swap the polygons of the given edges
	void				SwapEdgePolygons( ActiveEdge* pEdge1, ActiveEdge* pEdge2 );
	//
	// @CMember Swap the sides of the given edges
	void				SwapEdgeSides( ActiveEdge* pEdge1, ActiveEdge* pEdge2 );
	//
	// @CMember Close a polygon at the given edges
	void				ClosePolygon( ActiveEdge* pEdge1, ActiveEdge* pEdge2 );
	//
	// @CMember Add the given point to the polygon contributed to by the given edge
	void				AddPolyPoint( ActiveEdge* pEdge, const RRealPoint& pt );
	//
	// @CMember Add both edges from the LMT entry to the AET
	void				AddEdges( RClipPolygon::LMTEntry* pent );
	//
	// @CMember Determine if the local minimum with the given left edge should contribute to a polygon
	BOOLEAN			IsContributingLocalMin( ActiveEdge* pLeft, ActiveEdge* pRight );
	//
	// @CMember Get the number of polygons and total number of points generated
	YPolygonCount	GetCount( YPointCount& nTotalPoints );

//
// @Access Action implementations
private:
	//
	// @CMember handle intersection errors
	static void	IntersectionError( RPolygonClipper* pClipper, ActiveEdge* pEdge1, ActiveEdge* pEdge2, const RRealPoint& pt );
	//
	// @CMember Process intersection of edges at a local minimum
	static void	ProcessLocalMinIntersection( RPolygonClipper* pClipper, ActiveEdge* pEdge1, ActiveEdge* pEdge2, const RRealPoint& pt );
	//
	// @CMember Process intersection of edges at a local maximum
	static void	ProcessLocalMaxIntersection( RPolygonClipper* pClipper, ActiveEdge* pEdge1, ActiveEdge* pEdge2, const RRealPoint& pt );
	//
	// @CMember Process intersection of like sides of different polygons
	static void	ProcessLikeSideIntersection( RPolygonClipper* pClipper, ActiveEdge* pEdge1, ActiveEdge* pEdge2, const RRealPoint& pt );
	//
	// @CMember Process intersection of edges of the same polygon
	static void	ProcessLikeEdgeIntersection( RPolygonClipper* pClipper, ActiveEdge* pEdge1, ActiveEdge* pEdge2, const RRealPoint& pt );
	//
	// @CMember Process an intermediate intersection
	static void	ProcessIntermediateIntersection( RPolygonClipper* pClipper, ActiveEdge* pEdge1, ActiveEdge* pEdge2, const RRealPoint& pt );
	//
	// @CMember Activate any edges in the LMT that start at the current scanbeam bottom
	void	QueueEdges();
	//
	// @CMember Handle any horizontal edges at the given Y coordinate
	void	ProcessHorizontalSegments( float y );
	//
	// @CMember Handle any horizontal edges at the bottom of the current scanbeam
	void	PreprocessHorizontalSegments();
	//
	// @CMember Handle any horizontal edges at the top of the current scanbeam
	void	PostprocessHorizontalSegments();
	//
	// @CMember Compute intersections between active edges and build the intersection table
	void	ComputeIntersections();
	//
	// @CMember Update polygons from intersections and edge ends in the active edge table
	void	UpdatePolygons();

//
// @Access Construction/Destruction
public:
	//
	// @CMember Construct an RPolygonClipper for the given clip and subject polygons(LMT's)
	RPolygonClipper( RClipPolygon& lmtClip, RClipPolygon& lmtSubject );
	//
	// @CMember Destruct this RPolygonClipper
	~RPolygonClipper();

//
// @Access Protected APIs
protected:
	friend class RClipPolygon;
	RPolygonClipper( RClipPolygon& lmt );
//
// @Access Public APIs
public:
	//
	// @CMember Initialize the polygon generation
	void	StartPolygonGeneration( EOperation eop );
	//
	// @CMember Process one scanbeam (returns FALSE when done, TRUE if more needs to be done)
	BOOL	ContinuePolygonGeneration();
	//
	// @CMember End polygon generation
	void	EndPolygonGeneration( int& nPolygons, int& nTotalPoints );
	//
	// @CMember Cancel polygon generation
	void	CancelPolygonGeneration();
	//
	// @CMember Generate the polygons for the given operation
	void	GeneratePolygons( EOperation eop, int& nPolygons, int& nTotalPoints );
	//
	// @CMember Store the polygons into the given polypolygon
	int	StorePolygons( RRealPoint* pPoints, int* pCounts );
	int	StorePolygons( RIntPoint* pPoints, int* pCounts );

protected:
	//
	// @CMember Store the polygons into the given clippolygon
	void	StorePolygons( RClipPolygon& poly );

#ifdef TPSDEBUG
public:
	void DumpAET( char* why );
#endif
};

#ifdef MAC
#include	<stdio.h>
#endif	// Mac

#ifdef TPSDEBUG
#include	<stdarg.h>
extern int clipdebugindent;
inline void DoIndent()
	{
	char	tmp[200];
	if( clipdebugindent<0 )
		return;
	sprintf( tmp, "%.*s", clipdebugindent*4, "                                                                                    ");
	::_tdb_MsgOut( tmp );
	}
inline void StartDump( const char* fmt, ... )
	{
	char	tmp[200];
	if( clipdebugindent<0 )
		return;
	va_list	argv;
	DoIndent();
	va_start( argv, fmt );
	vsprintf( tmp, fmt, argv );
	va_end( argv );
	++clipdebugindent;
	::_tdb_MsgOut( tmp );
	}
inline void EndDump( const char* fmt, ... )
	{
	char	tmp[200];
	if( clipdebugindent<0 )
		return;
	va_list	argv;
	--clipdebugindent;
	DoIndent();
	va_start( argv, fmt );
	vsprintf( tmp, fmt, argv );
	va_end( argv );
	::_tdb_MsgOut( tmp );
	}
inline void DumpOut( const char* fmt, ... )
	{
	char	tmp[200];
	if( clipdebugindent<0 )
		return;
	va_list	argv;
	DoIndent();
	va_start( argv, fmt );
	vsprintf( tmp, fmt, argv );
	va_end( argv );
	::_tdb_MsgOut( tmp );
	}
#define	STARTFUNC(a)		StartDump##a
#define	ENDFUNC(a)			EndDump##a
#define	CONTINUEFUNC(a)	DumpOut##a
#else
#define	STARTFUNC(a)
#define	ENDFUNC(a)
#define	CONTINUEFUNC(a)
#endif

inline YPolygonCount ClipPolygon( const RIntPoint* pSubjVert, const YPointCount* pSubjCnts, YPolygonCount nSubjPolys,
											 const RIntPoint* pClipVert, const YPointCount* pClipCnts, YPolygonCount nClipPolys,
											 RIntPoint* pOutVert, YPointCount nMaxOutVert, YPointCount* pOutCnt, YPolygonCount nMaxOutPoly )
{
	YPolygonCount		nOutPolys;
	YPointCount			nOutPoints;
#ifdef TPSDEBUG
once_more_for_dump:
	try
		{
#endif
		RClipPolygon		clipPoly( pClipVert, pClipCnts, nClipPolys );
		RClipPolygon		subjPoly( pSubjVert, pSubjCnts, nSubjPolys );
		RPolygonClipper	clipper( clipPoly, subjPoly );
		clipper.GeneratePolygons( RPolygonClipper::kLogicalAnd, nOutPolys, nOutPoints );
		if( nOutPolys <= nMaxOutPoly && nOutPoints <= nMaxOutVert )
			clipper.StorePolygons( pOutVert, pOutCnt );
		else
			nOutPolys = -1;
#ifdef TPSDEBUG
		}
	catch( YException /*e*/ )
		{
		if( clipdebugindent < 0 )
			{
			//
			// turn on debug output
			clipdebugindent	= 0;
			goto once_more_for_dump;
			}
		//
		// turn off debug output...
		clipdebugindent = -1;
		throw;
		}
#endif
	return nOutPolys;
}

template<class ClipPoint,class SubjectPoint, class OutputPoint>
int MergePolygons( ClipPoint* ppt1, int* pcnt1, int npoly1,
						 SubjectPoint* ppt2, int* pcnt2, int npoly2,
						 OutputPoint* pPoints, int* pCounts,
						 RPolygonClipper::EOperation eop )
{
	STARTFUNC( ("MergePolysXor\n") );
	RClipPolygon	lmtClip( ppt1, pcnt1, npoly1 );
	RClipPolygon	lmtSubject( ppt2, pcnt2, npoly2 );
#ifdef TPSDEBUG
	CONTINUEFUNC( ("Clip polygon\n") );
	lmtClip.Dump();
	CONTINUEFUNC( ("Subject polygon\n") );
	lmtSubject.Dump();
#endif
	RPolygonClipper	state( lmtClip, lmtSubject );
	int					nPoints;
	int					nPolys;
	state.GeneratePolygons( eop, nPolys, nPoints );
	nPolys = state.StorePolygons( pPoints, pCounts );
	ENDFUNC( ("\n") );
	return nPolys;
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif
