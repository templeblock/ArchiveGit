// ****************************************************************************
// @Doc
// @Module Grid.inl |
//
//  File Name:			Grid.inl
//
//  Project:			Framework
//
//  Author:				Michael G. Dunlap
//
//  Description:		implementations of the inline RGrid class members
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /Development/CalendarComp/Source/MonthlyCalendarDocument.cpp           $
//   $Author:: Mgd                                                            $
//     $Date:: 12/12/96 7:00a                                                 $
// $Revision:: 1                                                              $
//
// ****************************************************************************


#ifdef _DEBUG
#define	INLINE
#else
#define	INLINE inline
#endif // defined(_DEBUG)

//
// @MFunc Construct an RGrid
//
// @Syntax RGrid()
// @Syntax RGrid( const RIntSize& cellCounts )
// @Syntax RGrid( const RGrid& src )
// @Parm const RIntSize& | cellCounts | dimensions of the grid in cells
// @Parm const RGrid& | src | RGrid to copy
//
INLINE RGrid::RGrid() :
	m_CellCounts								( kDefaultGridCellCounts ),
	m_pctTitleHeight							( kDefaultGridTitleHeight ),
	m_pctHeaderHeight							( kDefaultGridHeaderHeight ),
	m_pctLabelWidth							( kDefaultGridLabelWidth ),
	m_nGridHitBoundary						( kDefaultGridHitBoundary ),
	m_pFillColors								( NULL )
{
	SetLineWidth( kGridNothing, kDefaultGridLineWidth );
	SetLineWidth( kGridOutline, kDefaultGridLineWidth );
	SetLineWidth( kGridHorizontalLine, kDefaultGridLineWidth );
	SetLineWidth( kGridVerticalLine, kDefaultGridLineWidth );
	SetLineWidth( kGridTitleSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridHeaderSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridHeaderColumnSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridLabelSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridLabelRowSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridCellAreaOutline, kDefaultGridCellAreaOutlineWidth );
	SetupDefaultColors();
}

INLINE RGrid::RGrid( const RIntSize& cellCounts ) :
	m_CellCounts								( cellCounts ),
	m_pctTitleHeight							( kDefaultGridTitleHeight ),
	m_pctHeaderHeight							( kDefaultGridHeaderHeight ),
	m_pctLabelWidth							( kDefaultGridLabelWidth ),
	m_nGridHitBoundary						( kDefaultGridHitBoundary ),
	m_pFillColors								( NULL )
{
	SetLineWidth( kGridNothing, kDefaultGridLineWidth );
	SetLineWidth( kGridOutline, kDefaultGridLineWidth );
	SetLineWidth( kGridHorizontalLine, kDefaultGridLineWidth );
	SetLineWidth( kGridVerticalLine, kDefaultGridLineWidth );
	SetLineWidth( kGridTitleSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridHeaderSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridHeaderColumnSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridLabelSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridLabelRowSeparator, kDefaultGridLineWidth );
	SetLineWidth( kGridCellAreaOutline, kDefaultGridCellAreaOutlineWidth );
	SetupDefaultColors();
}


INLINE RGrid::RGrid( const RGrid& grid ) :
	m_pFillColors( NULL )
{
	//
	// Use the assignment operator...
	*this = grid;
}

//
// @MFunc Set the cell counts (number of rows and columns) for the grid
//
// @RDesc nothing
//
// @Comm minimum of (1,1)
//
INLINE void RGrid::SetCellCounts(
			const RIntSize& cellCounts )	// @Parm number of cells across and down for the grid
{
	TpsAssert( cellCounts.m_dx>0, "Must have at least one column in a grid" );
	TpsAssert( cellCounts.m_dy>0, "Must have at least one row in a grid" );
	m_CellCounts	= cellCounts;
	SetupDefaultColors();
}


//
// @MFunc Get the cell counts (number of rows and columns) for the grid
//
// @RDesc the dimensions of the grid
//
INLINE const RIntSize& RGrid::GetCellCounts() const
{
	return m_CellCounts;
}


//
// @MFunc Set the title area height
//
// @RDesc nothing
//
// @Comm 0.0 means there is no title area
//
INLINE void RGrid::SetTitleHeight(
			YPercentage	pctHeight )	// @Parm the height of the title area as a percentage of cell coordinate dimensions
{
	m_pctTitleHeight = pctHeight;
}


//
// @MFunc Get the title area height
//
// @RDesc the height of the title area as a percentage of cell coordinate dimensions
//
// @Comm 0.0 means there is no title area
//
INLINE YPercentage RGrid::GetTitleHeight() const
{
	return m_pctTitleHeight;
}


//
// @MFunc Set the header area height
//
// @RDesc nothing
//
// @Comm 0.0 means there is no header area
//
INLINE void RGrid::SetHeaderHeight(
			YPercentage	pctHeight )	// @Parm the height of the header area as a percentage of cell coordinate dimensions
{
	m_pctHeaderHeight	= pctHeight;
}


//
// @MFunc Get the header area height
//
// @RDesc the height of the header area as a percentage of cell coordinate dimensions
//
// @Comm 0.0 means there is no header area
//
INLINE YPercentage RGrid::GetHeaderHeight() const
{
	return m_pctHeaderHeight;
}


//
// @MFunc Set the label area width
//
// @RDesc nothing
//
// @Comm 0.0 means there is no label area
//
INLINE void RGrid::SetLabelWidth(
			YPercentage	pctWidth )	// @Parm the width of the label area as a percentage of cell coordinate dimensions
{
	m_pctLabelWidth = pctWidth;
}


//
// @MFunc Get the label area width
//
// @RDesc the width of the label area as a percentage of cell coordinate dimensions
//
// @Comm 0.0 means there is no label area
//
INLINE YPercentage RGrid::GetLabelWidth() const
{
	return m_pctLabelWidth;
}


//
// @MFunc Set the logical width of the specified grid line type
//
// @RDesc nothing
//
INLINE void RGrid::SetLineWidth(
			EGridItem		idItem,				// @Parm identifier of the type of line
			YRealDimension	nLogicalWidth )	// @Parm the new logical width for that type
{
	m_LineWidthTable[ idItem ]	= nLogicalWidth;
}


//
// @MFunc Get the logical width of the specified grid line type
//
// @RDesc the logical width
//
INLINE YRealDimension RGrid::GetLineWidth(
			EGridItem	eItem )	// @Parm the line type to get the width of
			const
{
	return m_LineWidthTable[ eItem ];
}


//
// @MFunc Set the color of the specified grid line type
//
// @RDesc nothing
//
INLINE void RGrid::SetLineColor(
			EGridItem		eItem,	// @Parm identifier of the type of line
			const RColor&	rgb )		// @Parm the new color for that type
{
	m_LineColorTable[eItem]	= rgb;
}

//
// @MFunc Get the color of the specified grid line type
//
// @RDesc the color
//
INLINE const RColor& RGrid::GetLineColor(
			EGridItem	eItem )	// @Parm identifier of the type of line
			const
{
	return m_LineColorTable[eItem];
}


//
// @MFunc Set the grid hit test boundary
//
// @RDesc nothing
//
// @Comm set to 0 to disable hitting the grid (hit cells/title only)
//
INLINE void RGrid::SetGridHitBoundary(
			YIntDimension nHitBoundary )	// @Parm hit test boundary logical units
{
	TpsAssert( nHitBoundary>=0, "Negative hit test boundary!" );
	m_nGridHitBoundary	= ::Max( nHitBoundary, YIntDimension(0) );
}


//
// @MFunc Get the grid hit test boundary
//
// @RDesc the gridline hit boundary logical units
//
INLINE YIntDimension RGrid::GetGridHitBoundary() const
{
	return m_nGridHitBoundary;
}


//
// @MFunc Get the given cell's position
//
// @RDesc TRUE if the cell exists within the grid else FALSE
//
INLINE BOOLEAN RGrid::GetCellPosition(
			int			nCell,		// @Parm the index of the cell to retrieve the row/column position for
			RIntSize&	cellPos )	// @Parm storage for the cell's row/column position
			const
{
	cellPos.m_dx	= nCell%m_CellCounts.m_dx;
	cellPos.m_dy	= nCell/m_CellCounts.m_dx;
	return BOOLEAN( cellPos.m_dy < m_CellCounts.m_dy );
}


//
// @MFunc Get the given cell's index
//
// @RDesc TRUE if the cell exists within the grid else FALSE
//
// @Comm cells are indexed starting at 0 as (row * nColumns) + column
//
INLINE BOOLEAN RGrid::GetCellIndex(
			const RIntSize&	cellPos,	// @Parm the row/column position of the cell
			int&					nCell )	// @Parm storage for the index of the cell
			const
{
	nCell = (cellPos.m_dy * m_CellCounts.m_dx) + cellPos.m_dx;
	return BOOLEAN( nCell>=0 && nCell<(m_CellCounts.m_dx*m_CellCounts.m_dy) );
}


//
// @MFunc Get the number of entries in the fill color array
//
// @RDesc the number of entries
//
INLINE int RGrid::GetFillColorCount() const
{
	return (m_CellCounts.m_dx+1) * (m_CellCounts.m_dy+1) + 1;
}

//
// @MFunc Compute the required grid size given the desired cell size
//
// @RDesc the required grid size
//
INLINE RRealSize RGrid::ComputeGridDimensions(
			const RRealSize&	cellSize )	// @Parm the desired cell dimensions
			const
{
	//
	// the interior of a cell in the grid
	YRealDimension	nCellsHigh	= YRealDimension( m_CellCounts.m_dy + m_pctHeaderHeight + m_pctTitleHeight );
	YRealDimension	nCellsWide	= YRealDimension( m_CellCounts.m_dx + m_pctLabelWidth );
	return RRealSize( cellSize.m_dx*nCellsWide, cellSize.m_dy*nCellsHigh );
}

//
// @MFunc Get the value of the flag for the given cell
//
// @RDesc	the value of the flag in the cell
//
INLINE BOOLEAN	RGrid::GetCellFlag( int , uLONG ) const
{
	return FALSE;	//	No flags in the base class
}

//
// @MFunc Set the value of the flag into the given cell
//
//	@RDesc	nothing
//
INLINE void RGrid::SetCellFlag( int , uLONG , BOOLEAN )
{
	;	//	No flags in the base class
}
