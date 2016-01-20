// ****************************************************************************
// @Doc
// @Module Grid.cpp |
//
//  File Name:			Grid.cpp
//
//  Project:			Framework
//
//  Author:				Michael G. Dunlap
//
//  Description:		implementations of the RGrid class members
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
//  $Logfile:: /PM8/Framework/Source/Grid.cpp                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"Grid.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "DrawingSurface.h"

//
// @MFunc Destructor
//
RGrid::~RGrid()
{
	delete[] m_pFillColors;
}


//
// @MFunc read the grid settings from the given archive
//
// @RDesc nothing
//
void RGrid::Read(
			RArchive&	ar )	// @Parm the archive to read the settings from
{
	//
	// Read the cell counts...
	ar	>> m_CellCounts;
	//
	// Initialize the color tables
	SetupDefaultColors();
	//
	// Read the remaining singular items
	ar	>> m_pctTitleHeight
		>> m_pctHeaderHeight
		>> m_pctLabelWidth
		>> m_nGridHitBoundary;
	//
	// Read the arrays...
	int	i;
	for( i=0 ; i<sizeof(m_LineWidthTable)/sizeof(m_LineWidthTable[0]) ; ++i )
		ar >> m_LineWidthTable[i];
	for( i=0 ; i<sizeof(m_LineColorTable)/sizeof(m_LineColorTable[0]) ; ++i )
		ar >> m_LineColorTable[i];
	for( i=0 ; i<GetFillColorCount() ; ++i )
		ar >> m_pFillColors[i];
}


//
// @MFunc write the grid settings to the given archive
//
// @RDesc nothing
//
void RGrid::Write(
			RArchive&	ar )	// @Parm the archive to write the settings to
			const
{
	//
	// Write the cell counts...
	ar	<< m_CellCounts;
	//
	// Write the remaining singular items
	ar	<< m_pctTitleHeight
		<< m_pctHeaderHeight
		<< m_pctLabelWidth
		<< m_nGridHitBoundary;
	//
	// Write the arrays...
	int	i;
	for( i=0 ; i<sizeof(m_LineWidthTable)/sizeof(m_LineWidthTable[0]) ; ++i )
		ar << m_LineWidthTable[i];
	for( i=0 ; i<sizeof(m_LineColorTable)/sizeof(m_LineColorTable[0]) ; ++i )
		ar << m_LineColorTable[i];
	for( i=0 ; i<GetFillColorCount() ; ++i )
		ar << m_pFillColors[i];
}


//
// @MFunc Get the logical dimensions of the given item
//
// @RDesc the logical dimensions
//
RRealSize RGrid::GetItemDimensions(
			const RRealSize&	gridSize,	// @Parm the size of the grid
			EGridItem			eItem )		// @Parm type of the item
			const
{
	switch( eItem )
		{
		case kGridOutline:
			{
			//
			// the grid outline bounding rect (all inclusive)
			return gridSize;
			}

		case kGridCellInterior:
			{
			//
			// the interior of a cell in the grid
			YRealDimension	nCellsHigh	= YRealDimension( m_CellCounts.m_dy + m_pctHeaderHeight + m_pctTitleHeight );
			YRealDimension	nCellsWide	= YRealDimension( m_CellCounts.m_dx + m_pctLabelWidth );
			YRealDimension	cellHeight	= YRealDimension( gridSize.m_dy / nCellsHigh );
			YRealDimension	cellWidth	= YRealDimension( gridSize.m_dx / nCellsWide );
			return RRealSize( cellWidth, cellHeight );
			}

		case kGridTitleInterior:
			{
			//
			// the interior of the grid title area
			RRealSize	cellSize( GetItemDimensions(gridSize,kGridCellInterior) );
			return RRealSize( YRealDimension(gridSize.m_dx), cellSize.m_dy * m_pctTitleHeight );
			}

		case kGridHeaderInterior:
			{
			//
			// the interior of a column header
			RRealSize	cellSize( GetItemDimensions(gridSize,kGridCellInterior) );
			return RRealSize( cellSize.m_dx, cellSize.m_dy * m_pctHeaderHeight );
			}

		case kGridLabelInterior:
			{
			//
			// the interior of a row label
			RRealSize	cellSize( GetItemDimensions(gridSize,kGridCellInterior) );
			return RRealSize( cellSize.m_dx * m_pctLabelWidth, cellSize.m_dy );
			}

		case kGridTitleSeparator:
			{
			//
			// the horizontal separator between the title area and the header area
			if( GetTitleHeight() > 0 )
				{
				RRealSize	cellAreaSize( GetItemDimensions(gridSize,kGridCellAreaOutline) );
				return RRealSize( cellAreaSize.m_dx, YRealDimension(GetLineWidth(eItem)) );
				}
			return RRealSize(0.0,0.0);
			}

		case kGridHeaderSeparator:
			{
			//
			// the horizontal separator between the column headers and the cell area
			if( GetHeaderHeight() > 0 )
				{
				RRealSize	cellAreaSize( GetItemDimensions(gridSize,kGridCellAreaOutline) );
				return RRealSize( cellAreaSize.m_dx, YRealDimension(GetLineWidth(eItem)) );
				}
			return RRealSize(0.0,0.0);
			}

		case kGridLabelSeparator:
			{
			//
			// the vertical separator between the row labels and the cell area
			if( GetLabelWidth() > 0 )
				{
				RRealSize	cellAreaSize( GetItemDimensions(gridSize,kGridCellAreaOutline) );
				return RRealSize( YRealDimension(GetLineWidth(eItem)), cellAreaSize.m_dy );
				}
			return RRealSize(0.0,0.0);
			}

		case kGridHorizontalLine:
			{
			//
			// a horizontal grid line
			RRealSize	cellAreaSize( GetItemDimensions(gridSize,kGridCellAreaOutline) );
			return RRealSize( cellAreaSize.m_dx, YRealDimension(GetLineWidth(eItem)) );
			}

		case kGridVerticalLine:
			{
			//
			// a vertical grid line
			RRealSize	cellAreaSize( GetItemDimensions(gridSize,kGridCellAreaOutline) );
			return RRealSize( YRealDimension(GetLineWidth(eItem)), cellAreaSize.m_dy );
			}

		case kGridHeaderColumnSeparator:
			{
			//
			// the vertical separator between column headers
			RRealSize	hdrSize( GetItemDimensions(gridSize,kGridHeaderInterior) );
			return RRealSize( YRealDimension(GetLineWidth(eItem)), hdrSize.m_dy );
			}

		case kGridLabelRowSeparator:
			{
			//
			// the horizontal separator between row labels
			RRealSize	rowSize( GetItemDimensions(gridSize,kGridLabelInterior) );
			return RRealSize( rowSize.m_dx, YRealDimension(GetLineWidth(eItem)) );
			}

		case kGridCellAreaOutline:
			{
			//
			// the outline of the cell area
			RRealSize	cellSize( GetItemDimensions(gridSize,kGridCellInterior) );
			return RRealSize( cellSize.m_dx * m_CellCounts.m_dx, cellSize.m_dy * m_CellCounts.m_dy );
			}
		}
	TpsAssertAlways( "Attempting to get the dimensions of an invalid item" );
	return RRealSize( 0.0, 0.0 );
}

//
// @MFunc Get the bounds of the given item
//
// @RDesc the bounding rect of the item
//
RRealRect RGrid::GetItemRect(
			const RRealSize&	gridSize,	// @Parm the size of the grid
			EGridItem			eItem,		// @Parm type of the item
			int					nItemID )	// @Parm id of the item
			const
{
	switch( eItem )
		{
		case kGridOutline:
			//
			// the grid outline bounding rect (all inclusive)
			return RRealRect( GetItemDimensions(gridSize,eItem) );

		case kGridCellAreaOutline:
			{
			//
			// the outline of the cell area
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			RRealRect	rcItem( GetItemRect(gridSize,kGridOutline) );
			rcItem.m_Top	= rcItem.m_Bottom - itemSize.m_dy;
			rcItem.m_Left	= rcItem.m_Right - itemSize.m_dx;
			return rcItem;
			}

		case kGridTitleInterior:
			{
			//
			// the interior of the title area
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			RRealRect	rcItem( GetItemRect(gridSize,kGridOutline) );
			rcItem.m_Bottom	= rcItem.m_Top + itemSize.m_dy;
			rcItem.m_Left		= rcItem.m_Right - itemSize.m_dx;
			return rcItem;
			}

		case kGridHeaderInterior:
			{
			//
			// the interior of a column header
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			int			nAdjacentCell	= nItemID % m_CellCounts.m_dx;
			RRealRect	rcItem( GetItemRect(gridSize,kGridCellInterior,nAdjacentCell) );
			rcItem.m_Bottom	= rcItem.m_Top;
			rcItem.m_Top		-= itemSize.m_dy;
			return rcItem;
			}

		case kGridLabelInterior:
			{
			//
			// the interior of a row label
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			int			nAdjacentCell	= (nItemID % m_CellCounts.m_dy) * m_CellCounts.m_dx;
			RRealRect	rcItem( GetItemRect(gridSize,kGridCellInterior,nAdjacentCell) );
			rcItem.m_Right	= rcItem.m_Left;
			rcItem.m_Left	-= itemSize.m_dx;
			return rcItem;
			}

		case kGridCellInterior:
			{
			//
			// the interior of a cell in the grid
			RRealSize		cellSize( GetItemDimensions(gridSize,eItem) );
			RRealRect		rcItem( GetItemRect(gridSize,kGridCellAreaOutline) );
			YIntDimension	nRow( YIntDimension(nItemID/m_CellCounts.m_dx) );
			YIntDimension	nCol( YIntDimension(nItemID%m_CellCounts.m_dx) );
			rcItem.m_Top		+= YRealDimension( nRow * cellSize.m_dy );
			rcItem.m_Left		+= YRealDimension( nCol * cellSize.m_dx );
			rcItem.m_Right		= rcItem.m_Left + cellSize.m_dx;
			rcItem.m_Bottom	= rcItem.m_Top + cellSize.m_dy;
			return rcItem;
			}

		case kGridTitleSeparator:
			{
			//
			// the horizontal separator between the title area and the header area
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			RRealRect	rcItem( GetItemRect(gridSize,kGridTitleInterior) );
			rcItem.m_Top		= rcItem.m_Bottom - itemSize.m_dy / 2.0;
			rcItem.m_Bottom	= rcItem.m_Top + itemSize.m_dy;
			return rcItem;
			}

		case kGridHeaderSeparator:
			{
			//
			// the horizontal separator between the header area and the grid cells
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			RRealRect	rcItem( GetItemRect(gridSize,kGridCellAreaOutline) );
			rcItem.m_Top		-= itemSize.m_dy / 2.0;
			rcItem.m_Bottom	= rcItem.m_Top + itemSize.m_dy;
			return rcItem;
			}

		case kGridLabelSeparator:
			{
			//
			// the vertical separator between the label area and the grid cells
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			RRealRect	rcItem( GetItemRect(gridSize,kGridCellAreaOutline) );
			rcItem.m_Right	= rcItem.m_Left + itemSize.m_dx / 2.0;
			rcItem.m_Left	= rcItem.m_Right - itemSize.m_dx;
			return rcItem;
			}

		case kGridHorizontalLine:
			{
			//
			// the specified horizontal grid line
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			RRealRect	rcItem( GetItemRect(gridSize,kGridCellInterior,((nItemID%m_CellCounts.m_dy)+1)*m_CellCounts.m_dx) );
			rcItem.m_Top		-= itemSize.m_dy / 2.0;
			rcItem.m_Bottom	= rcItem.m_Top + itemSize.m_dy;
			rcItem.m_Right		= rcItem.m_Left + itemSize.m_dx;
			return rcItem;
			}

		case kGridVerticalLine:
			{
			//
			// the specified vertical grid line
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			RRealRect	rcItem( GetItemRect(gridSize,kGridCellInterior,nItemID+1) );
			rcItem.m_Left		-= itemSize.m_dx / 2.0;
			rcItem.m_Right		= rcItem.m_Left + itemSize.m_dx;
			rcItem.m_Bottom	= rcItem.m_Top + itemSize.m_dy;
			return rcItem;
			}

		case kGridHeaderColumnSeparator:
			{
			//
			// the vertical separator between column headers
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			RRealRect	rcItem( GetItemRect(gridSize,kGridHeaderInterior,nItemID) );
			rcItem.m_Left		= rcItem.m_Right - itemSize.m_dx/2.0;
			rcItem.m_Right		= rcItem.m_Left + itemSize.m_dx;
			return rcItem;
			}

		case kGridLabelRowSeparator:
			{
			//
			// the horizontal separator between label rows
			RRealSize	itemSize( GetItemDimensions(gridSize,eItem) );
			RRealRect	rcItem( GetItemRect(gridSize,kGridLabelInterior,nItemID) );
			rcItem.m_Top		= rcItem.m_Bottom - itemSize.m_dy / 2.0;
			rcItem.m_Bottom	= rcItem.m_Top + itemSize.m_dy;
			return rcItem;
			}
		}
	TpsAssertAlways( "Attempting to get the bounding rect of an invalid item" );
	return RRealRect(RRealSize(0.0,0.0));
}


//
// @MFunc Test if the given item is visible
//
// @RDesc TRUE if it's visible else FALSE
//
BOOLEAN RGrid::IsVisible(
			RDrawingSurface&		drawingSurface,	// @Parm the drawingsurface
			const RRealSize&		gridSize,			// @Parm the nominal size of the grid
			const R2dTransform&	transform,			// @Parm the output transformation
			EGridItem				eItem,				// @Parm identifier of the type of item
			int						idxItem )			// @Parm index of the item
			const
{
	return drawingSurface.IsVisible( GetItemRect(gridSize,eItem,idxItem), transform );
}


//
// @MFunc Hit test the grid
//
// @RDesc identifier of what was hit (if anything)
//
// @Comm See <mf RGrid::SetGridHitBoundary>, <mf RGrid::SetVerticalGridLineWidth>,
// <mf RGrid::SetHorizontalGridLineHeight>, and <mf RGrid::SetGridFrameWidth>
//
EGridItem RGrid::HitTestGrid(
			const RRealSize&		gridSize,	// @Parm the size of the grid
			const R2dTransform&	xToDevice,	// @Parm transformation to get to device units
			const RIntPoint&		ptTest,		// @Parm the point to test (upper left of grid is 0,0)
			int&						nItem )		// @Parm storage for the id if the hit item (if any)
			const
{
	YRealDimension		nBoundary	= YRealDimension( GetGridHitBoundary() );
	RRealSize			szInset( -nBoundary, -nBoundary );
	RRealSize			szInflate( nBoundary, nBoundary );
	RRealPoint			ptDevice( RRealPoint(ptTest) * xToDevice );

	//
	// Test the grid outline...
	if( GetLineWidth(kGridOutline) > 0 )
		{
		YRealDimension		nWidth( YRealDimension(GetLineWidth(kGridOutline)) );
		RRealSize			sizeLine( nWidth, nWidth );
		RRealVectorRect	rcOuter( RRealVectorRect(GetItemRect(gridSize,kGridOutline)) * xToDevice );
		RRealVectorRect	rcInner( RRealVectorRect(GetItemRect(gridSize,kGridOutline).Inset(sizeLine)) * xToDevice );
		rcOuter.Inflate( szInflate );
		rcInner.Inflate( szInset );
		if( rcOuter.PointInRect(ptDevice) && !rcInner.PointInRect(ptDevice) )
			{
			nItem	= 0;
			return kGridOutline;
			}
		}
	//
	// Test the title separator line...
	if( GetTitleHeight() > 0 && GetLineWidth(kGridTitleSeparator) > 0 )
		{
		RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridTitleSeparator)) * xToDevice );
		//	This have to check for absolute inequality with 0.0 anything else is ok
		if ( (rcTest.Width()!=0.0) && (rcTest.Height()!=0.0) )
			{
			rcTest.Inflate( szInflate );
			if( rcTest.PointInRect(ptDevice) )
				{
				nItem	= 0;
				return kGridTitleSeparator;
				}
			}
		}

	//
	// Test the header separator line...
	if( GetHeaderHeight() > 0 && GetLineWidth(kGridHeaderSeparator) > 0 )
		{
		RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridHeaderSeparator)) * xToDevice );
		//	This have to check for absolute inequality with 0.0 anything else is ok
		if ( (rcTest.Width()!=0.0) && (rcTest.Height()!=0.0) )
			{
			rcTest.Inflate( szInflate );
			if( rcTest.PointInRect(ptDevice) )
				{
				nItem	= 0;
				return kGridHeaderSeparator;
				}
			}
		}

	//
	// Test the label separator line...
	if( GetLabelWidth() > 0 && GetLineWidth(kGridLabelSeparator) > 0 )
		{
		RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridLabelSeparator)) * xToDevice );
		//	This have to check for absolute inequality with 0.0 anything else is ok
		if ( (rcTest.Width()!=0.0) && (rcTest.Height()!=0.0) )
			{
			rcTest.Inflate( szInflate );
			if( rcTest.PointInRect(ptDevice) )
				{
				nItem	= 0;
				return kGridLabelSeparator;
				}
			}
		}

	//
	// Test the horizontal grid lines...
	if( GetLineWidth(kGridHorizontalLine) > 0 )
		{
		nItem	= m_CellCounts.m_dy;
		while( --nItem >= 0 )
			{
			RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridHorizontalLine,nItem)) * xToDevice );
			//	This have to check for absolute inequality with 0.0 anything else is ok
			if ( (rcTest.Width()!=0.0) && (rcTest.Height()!=0.0) )
				{
				rcTest.Inflate( szInflate );
				if( rcTest.PointInRect(ptDevice) )
					return kGridHorizontalLine;
				}
			}
		}

	//
	// Test the vertical grid lines...
	if( GetLineWidth(kGridVerticalLine) > 0 )
		{
		nItem	= m_CellCounts.m_dx;
		while( --nItem >= 0 )
			{
			RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridVerticalLine,nItem)) * xToDevice );
			//	This have to check for absolute inequality with 0.0 anything else is ok
			if ( (rcTest.Width()!=0.0) && (rcTest.Height()!=0.0) )
				{
				rcTest.Inflate( szInflate );
				if( rcTest.PointInRect(ptDevice) )
					return kGridVerticalLine;
				}
			}
		}

	//
	// Test the header column separator lines
	if( GetHeaderHeight() > 0 && GetLineWidth(kGridHeaderColumnSeparator) > 0 )
		{
		nItem	= m_CellCounts.m_dx;
		while( --nItem >= 0 )
			{
			RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridHeaderColumnSeparator,nItem)) * xToDevice );
			//	This have to check for absolute inequality with 0.0 anything else is ok
			if ( (rcTest.Width()!=0.0) && (rcTest.Height()!=0.0) )
				{
				rcTest.Inflate( szInflate );
				if( rcTest.PointInRect(ptDevice) )
					return kGridHeaderColumnSeparator;
				}
			}
		}

	//
	// Test the label row separator lines
	if( GetLabelWidth() > 0 && GetLineWidth(kGridLabelRowSeparator) > 0 )
		{
		nItem	= m_CellCounts.m_dx;
		while( --nItem >= 0 )
			{
			RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridLabelRowSeparator,nItem)) * xToDevice );
			//	This have to check for absolute inequality with 0.0 anything else is ok
			if ( (rcTest.Width()!=0.0) && (rcTest.Height()!=0.0) )
				{
				rcTest.Inflate( szInflate );
				if( rcTest.PointInRect(ptDevice) )
					return kGridLabelRowSeparator;
				}
			}
		}

	//
	// Test the title area...
	if( GetTitleHeight() > 0 )
		{
		RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridTitleInterior)) * xToDevice );
		if( rcTest.PointInRect(ptDevice) )
			{
			nItem	= 0;
			return kGridTitleInterior;
			}
		}

	//
	// Test the header columns
	if( GetHeaderHeight() > 0 )
		{
		nItem	= m_CellCounts.m_dx;
		while( --nItem >= 0 )
			{
			RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridHeaderInterior,nItem)) * xToDevice );
			//	This have to check for absolute inequality with 0.0 anything else is ok
			if ( (rcTest.Width()!=0.0) && (rcTest.Height()!=0.0) )
				{
				rcTest.Inflate( szInflate );
				if( rcTest.PointInRect(ptDevice) )
					return kGridHeaderInterior;
				}
			}
		}

	//
	// Test the labels
	if( GetLabelWidth() > 0 )
		{
		nItem	= m_CellCounts.m_dy;
		while( --nItem >= 0 )
			{
			RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridLabelInterior,nItem)) * xToDevice );
			//	This have to check for absolute inequality with 0.0 anything else is ok
			if ( (rcTest.Width()!=0.0) && (rcTest.Height()!=0.0) )
				{
				rcTest.Inflate( szInflate );
				if( rcTest.PointInRect(ptDevice) )
					return kGridLabelInterior;
				}
			}
		}

	//
	// Test the cells...
	nItem	= m_CellCounts.m_dx * m_CellCounts.m_dy;
	while( --nItem >= 0 )
		{
		RRealVectorRect	rcTest( RRealVectorRect(GetItemRect(gridSize,kGridCellInterior,nItem)) * xToDevice );
		if( rcTest.PointInRect(ptDevice) )
			return kGridCellInterior;
		}

	//
	// didn't hit anything...
	nItem	= 0;
	return kGridNothing;
}


//
// @MFunc Render the given line if it has dimensions in this grid
//
// @RDesc nothing
//
void RGrid::RenderLine(
			RDrawingSurface&		drawingSurface,	// @Parm the drawing surface to render into
			const RRealSize&		gridSize,			// @Parm the size of the grid
			const R2dTransform&	transform,			// @Parm the output transformation
			YRealDimension			dInset,				// @Parm amount to inset the ends of the line
			EGridItem				eItem,				// @Parm the type of the line to draw
			int						idxItem )			// @Parm which of the lines of the given type to draw
			const
{
	YPenWidth	penWidth	= TransformedPenWidth( GetLineWidth(eItem), transform );
	if( penWidth > 0 )
		{
		RRealRect		rcItem( GetItemRect(gridSize,eItem,idxItem) );
		YRealDimension	nHalfPen( GetLineWidth(eItem)/2.0 );
		if( rcItem.Height() > 0.0 )
			{
			RRealPoint	ptCenter( rcItem.GetCenterPoint() );
			drawingSurface.SetPenStyle( kSolidPen );
			drawingSurface.SetPenColor( GetLineColor(eItem) );
			drawingSurface.SetPenWidth( penWidth );
			if( rcItem.Height() > rcItem.Width() )
				{
				drawingSurface.MoveTo( RRealPoint(ptCenter.m_x-nHalfPen,rcItem.m_Top+dInset), transform );
				drawingSurface.LineTo( RRealPoint(ptCenter.m_x-nHalfPen,rcItem.m_Bottom-dInset), transform );
				}
			else
				{
				drawingSurface.MoveTo( RRealPoint(rcItem.m_Left+dInset,ptCenter.m_y-nHalfPen), transform );
				drawingSurface.LineTo( RRealPoint(rcItem.m_Right-dInset,ptCenter.m_y-nHalfPen), transform );
				}
			}
		}

}

//
// @MFunc Frame the given item if it has dimensions in this grid
//
// @RDesc nothing
//
void RGrid::RenderFrame(
			RDrawingSurface&		drawingSurface,	// @Parm the drawing surface to render into
			const RRealSize&		gridSize,			// @Parm the size of the grid
			const R2dTransform&	transform,			// @Parm the output transformation
			EGridItem				eItem,				// @Parm the type of the item to frame
			int						idxItem )			// @Parm which of the items of the given type to frame
			const
{
	YPenWidth	penWidth	= TransformedPenWidth( GetLineWidth(eItem), transform );
	if( penWidth>0 )
		{
		RRealRect	rcItem( GetItemRect(gridSize,eItem,idxItem) );
		if( rcItem.Height() > 0.0 && rcItem.Width() > 0.0 )
			{
			drawingSurface.SetPenWidth( penWidth );
			drawingSurface.SetPenColor( GetLineColor( eItem ) );
			drawingSurface.FrameRectangleInside( rcItem, transform );
			}
		}
}


//
// @MFunc Render the fill for the given item
//
// @RDesc nothing
//
void RGrid::RenderFill(
			RDrawingSurface&		drawingSurface,	// @Parm the drawing surface to render into
			const RRealSize&		gridSize,			// @Parm the size of the grid
			const R2dTransform&	transform,			// @Parm the output transformation
			EGridItem				eItem,				// @Parm the type of the item to fill
			int						idxItem )			// @Parm which of the items of the given type to fill
			const
{
	if( IsVisible(drawingSurface,gridSize,transform,eItem,idxItem) )
		{
		RRealRect	rcItem( GetItemRect(gridSize,eItem,idxItem) );
		RColor		color( GetItemFill(eItem,idxItem) );
		color.SetBoundingRect( rcItem );
		drawingSurface.SetFillColor( color );
		drawingSurface.FillRectangle( rcItem, transform );
		}
}


//
// @MFunc Render the grid
//
// @RDesc nothing
//
void RGrid::Render(
			RDrawingSurface&		drawingSurface,	// @Parm the drawing surface to render into
			const RRealSize&		gridSize,			// @Parm the size of the grid
			const R2dTransform&	transform )			// @Parm the output transformation
			const
{
	int					i;
	YRealDimension		dGridInset		= YRealDimension( GetLineWidth(kGridOutline) / 2.0 );
	YRealDimension		dCellInset		= YRealDimension( GetLineWidth(kGridCellAreaOutline) / 2.0 );
	YRealDimension		dGridLineInset	= ::Max( dGridInset, dCellInset );

	//
	// Render the title background
	RenderFill( drawingSurface, gridSize, transform, kGridTitleInterior );

	//
	// Render the header backgrounds
	if( GetHeaderHeight() > 0.0 )
		{ 
		for( i=0 ; i<m_CellCounts.m_dx ; ++i )
			RenderFill( drawingSurface, gridSize, transform, kGridHeaderInterior, i );
		}

	//
	// Render the label backgrounds
	if( GetLabelWidth() > 0.0 )
		{
		for( i=0 ; i<m_CellCounts.m_dy ; ++i )
			RenderFill( drawingSurface, gridSize, transform, kGridLabelInterior, i );
		}

	//
	// Render the cell backgrounds
	for( i=m_CellCounts.m_dx*m_CellCounts.m_dy ; i-- ; )
		RenderFill( drawingSurface, gridSize, transform, kGridCellInterior, i );

	//
	// Render the vertical grid lines...
	for( i=1 ; i<m_CellCounts.m_dx ; ++i )
		RenderLine( drawingSurface, gridSize, transform, dGridLineInset, kGridVerticalLine, i-1 );

	//
	// Render the horizontal grid lines...
	for( i=1 ; i<m_CellCounts.m_dy ; ++i )
		RenderLine( drawingSurface, gridSize, transform, dGridLineInset, kGridHorizontalLine, i-1 );

	//
	// Render the header column separators
	if( GetHeaderHeight() > 0.0 )
		{
		for( i=1 ; i<m_CellCounts.m_dx ; ++i )
			RenderLine( drawingSurface, gridSize, transform, 0.0, kGridHeaderColumnSeparator, i-1 );
		}

	//
	// Render the label row separators
	if( GetLabelWidth() > 0.0 )
		{
		for( i=1 ; i<m_CellCounts.m_dy ; ++i )
			RenderLine( drawingSurface, gridSize, transform, 0.0, kGridLabelRowSeparator, i-1 );
		}

	//
	// Render the title separator line
	if( GetTitleHeight() > 0.0 )
		RenderLine( drawingSurface, gridSize, transform, dGridInset, kGridTitleSeparator, 0 );

	//
	// Render the header separator line
	if( GetHeaderHeight() > 0.0 )
		RenderLine( drawingSurface, gridSize, transform, dGridInset, kGridHeaderSeparator, 0 );

	//
	// Render the label separator line
	if( GetLabelWidth() > 0.0 )
		RenderLine( drawingSurface, gridSize, transform, dGridInset, kGridLabelSeparator, 0 );

	//
	// Render the cell area frame
	RenderFrame( drawingSurface, gridSize, transform, kGridCellAreaOutline, 0 );

	//
	// Render the grid frame
	RenderFrame( drawingSurface, gridSize, transform, kGridOutline, 0 );
}


//
// @CMember Fill in a font specification structure
const YFont& RGrid::CreateFontSpec(
			const RRealSize&		gridSize,
			EGridItem				eArea,
			YFont&					font,
			const RMBCString&		szTypeface,
			const RSolidColor&	rgb,
			YPercentage				pctHeight )
			const
{
	RRealSize		areaSize( GetItemDimensions(gridSize,eArea) );

	strcpy( font.info.sbName, szTypeface );
	font.info.attributes	= 0;
	font.info.ulPadding	= 0;
	font.color				= rgb;
	font.info.angle		= 0;
	font.info.height		= YFontSize( areaSize.m_dy * pctHeight );
	return font;
}


//
// @MFunc Set the fill color for the given item
//
// @RDesc nothing
//
void RGrid::SetItemFill(
			const RColor&	clr,			// @Parm the color to set for the item
			EGridItem			eItem,		// @Parm identifier of the type of item
			int					idxItem )	// @Parm index of the specific item to set the fill color for
{
	int	nCells				= m_CellCounts.m_dx * m_CellCounts.m_dy;
	int	nFirstCellColor	= 0;
	int	nFirstRowColor		= nFirstCellColor + nCells;
	int	nFirstColColor		= nFirstRowColor + m_CellCounts.m_dy;
	int	nTitleColor			= nFirstColColor + m_CellCounts.m_dx;

	switch( eItem )
		{
		case kGridTitleInterior:
			//
			// the interior of the grid title area
			m_pFillColors[ nTitleColor ] = clr;
			return;

		case kGridHeaderInterior:
			//
			// the interior of a column header
			if( idxItem>=0 && idxItem<m_CellCounts.m_dx )
				m_pFillColors[ nFirstColColor + idxItem ] = clr;
			return;

		case kGridLabelInterior:
			//
			// the interior of a row label
			if( idxItem>=0 && idxItem<m_CellCounts.m_dy )
				m_pFillColors[ nFirstRowColor + idxItem ] = clr;
			return;

		case kGridCellInterior:
			//
			// the interior of a cell in the grid
			if( idxItem>=0 && idxItem<nCells )
				m_pFillColors[ nFirstCellColor + idxItem ] = clr;
			return;

		}
	TpsAssertAlways( "Attempting to set the fill color for an invalid item" );
}

//
// @MFunc Get the fill color for the given item
//
// @RDesc const reference to the fill color
//
const RColor& RGrid::GetItemFill(
			EGridItem			eItem,		// @Parm identifier of the type of item
			int					idxItem )	// @Parm index of the specific item to set the fill color for
			const
{
	int	nCells				= m_CellCounts.m_dx * m_CellCounts.m_dy;
	int	nFirstCellColor	= 0;
	int	nFirstRowColor		= nFirstCellColor + nCells;
	int	nFirstColColor		= nFirstRowColor + m_CellCounts.m_dy;
	int	nTitleColor			= nFirstColColor + m_CellCounts.m_dx;
	static RColor	nocolor;

	switch( eItem )
		{
		case kGridTitleInterior:
			//
			// the interior of the grid title area
			return m_pFillColors[ nTitleColor ];

		case kGridHeaderInterior:
			//
			// the interior of a column header
			if( idxItem>=0 && idxItem<m_CellCounts.m_dx )
				return m_pFillColors[ nFirstColColor + idxItem ];
			break;

		case kGridLabelInterior:
			//
			// the interior of a row label
			if( idxItem>=0 && idxItem<m_CellCounts.m_dy )
				return m_pFillColors[ nFirstRowColor + idxItem ];
			break;

		case kGridCellInterior:
			//
			// the interior of a cell in the grid
			if( idxItem>=0 && idxItem<nCells )
				return m_pFillColors[ nFirstCellColor + idxItem ];
			break;
		}

	TpsAssertAlways( "Attempting to get the fill color for an invalid item" );
	return nocolor;
}

//
// @MFunc assignment operator
//
// @RDesc const reference to this RGrid
//
const RGrid& RGrid::operator=(
			const RGrid& rhs )	// @Parm RGrid to copy
{
	if( &rhs != this )
		{
		int	i;
		m_CellCounts								= rhs.m_CellCounts;
		m_pctTitleHeight							= rhs.m_pctTitleHeight;
		m_pctHeaderHeight							= rhs.m_pctHeaderHeight;
		m_pctLabelWidth							= rhs.m_pctLabelWidth;
		m_nGridHitBoundary						= rhs.m_nGridHitBoundary;
		SetupDefaultColors();
		for( i=sizeof(m_LineWidthTable)/sizeof(m_LineWidthTable[0]) ; i-- ; )
			m_LineWidthTable[i] = rhs.m_LineWidthTable[i];
		for( i=sizeof(m_LineColorTable)/sizeof(m_LineColorTable[0]) ; i-- ; )
			m_LineColorTable[i] = rhs.m_LineColorTable[i];
		for( i=GetFillColorCount() ; i-- ; )
			m_pFillColors[i] = rhs.m_pFillColors[i];
		}
	return *this;
}


//
// @MFunc Restore the defaults for the grid
//
// @RDesc nothing
//
void RGrid::SetupDefaultColors()
{
	delete[] m_pFillColors;
	m_pFillColors	= new RColor[ GetFillColorCount() ];

	SetLineColor( kGridNothing, kDefaultGridLineColor );
	SetLineColor( kGridOutline, kDefaultGridLineColor );
	SetLineColor( kGridHorizontalLine, kDefaultGridLineColor );
	SetLineColor( kGridVerticalLine, kDefaultGridLineColor );
	SetLineColor( kGridTitleSeparator, kDefaultGridLineColor );
	SetLineColor( kGridHeaderSeparator, kDefaultGridLineColor );
	SetLineColor( kGridHeaderColumnSeparator, kDefaultGridLineColor );
	SetLineColor( kGridLabelSeparator, kDefaultGridLineColor );
	SetLineColor( kGridLabelRowSeparator, kDefaultGridLineColor );
	SetLineColor( kGridCellAreaOutline, kDefaultGridLineColor );

#ifdef _DEBUG
	for( int i=GetFillColorCount() ; i-- ; )
		{
		switch( i%4 )
			{
			case 1:
				m_pFillColors[i] = RColor( kGreen );
				break;
			case 2:
				m_pFillColors[i] = RColor( kBlue );
				break;
			case 3:
				m_pFillColors[i] = RColor( kYellow );
				break;
			}
		}
#endif
}


#ifdef _DEBUG
#include	"Grid.inl"
#endif
