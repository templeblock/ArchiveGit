// ****************************************************************************
// @Doc
// @Module Grid.h |
//
//  File Name:			Grid.h
//
//  Project:			Framework
//
//  Author:				Michael G. Dunlap
//
//  Description:		Declaration of the RGrid class
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
//  $Logfile:: /PM8/Framework/Include/Grid.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _Grid_H_
#define _Grid_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// @Const Default size of a new grid
const RIntSize			kDefaultGridCellCounts				= RIntSize( 1, 1 );
//
// @Const Default logical(1440/inch) width of the grid outline
const YRealDimension	kDefaultGridOutlineWidth			= 10.0;
//
// @Const Default logical width of the grid cell area outline
const YRealDimension	kDefaultGridCellAreaOutlineWidth	= 0.0;
//
// @Const Default logical width of the grid lines
const YRealDimension	kDefaultGridLineWidth				= 20.0;
//
// @Const Default height of the grid's title area as a percentage of computed cell height
const YPercentage		kDefaultGridTitleHeight				= 0.0;
//
// @Const Default height of the grid's header area as a percentage of computed cell height
const YPercentage		kDefaultGridHeaderHeight			= 0.0;
//
// @Const Default width of the grid's row label area as a percentage of computed cell width
const YPercentage		kDefaultGridLabelWidth				= 0.0;
//
// @Const Default boundary around grid lines for hit testing in device units
const YIntDimension	kDefaultGridHitBoundary				= 2;
//
// @Const Default color for the grid outline
const RColor			kDefaultGridFrameColor				= kBlack;
//
// @Const Default color for the grid lines
const RColor			kDefaultGridLineColor				= kBlack;

//
// @Enum Types of grid items for hit testing/bounding rect retrieval
enum EGridItem
{
	kGridNothing = 0,					// @EMem nothing within the grid was hit
	kGridOutline,						// @EMem the grid outline bounding rect (all inclusive)
	kGridHorizontalLine,				// @EMem a horizontal grid line
	kGridVerticalLine,				// @EMem a vertical grid line
	kGridTitleSeparator,				// @EMem the horizontal separator between the title area and the header area
	kGridHeaderSeparator,			// @EMem the horizontal separator between the header area and the grid cells
	kGridHeaderColumnSeparator,	// @EMem the vertical separator between column headers
	kGridLabelSeparator,				// @EMem the vertical separator between the row labels and the grid cells
	kGridLabelRowSeparator,			// @EMem the horizontal separator between the row labels
	kGridCellAreaOutline,			// @EMem the outline of the cell area
	kGridCellInterior,				// @EMem the interior of a cell in the grid
	kGridTitleInterior,				// @EMem the interior of the grid title area
	kGridHeaderInterior,				// @EMem the interior of a column header
	kGridLabelInterior,				// @EMem the interior of a row label
	kGridUser,							// @EMem generic item id for user defined items
	kGridItemCount,					// @EMem tag for the number of items in the enum
								//	The GridItemCount is the number of items of the above that
								//		are stored in the file.  It cannot change.  The next
								//		few values are for retrieving CellContent information
	kGridCellContentText,			// @EMem cell contents Text
	kGridCellContentSquare,			//	@EMem cell contents preview size (Union of CellInterior & CellLabel)
	kGridCellContentPreview
};

//
// forward references...
class RDrawingSurface;
class	RStorage;

//
// @Class Support for the month grid
class FrameworkLinkage RGrid
{
//
// @Access Construction/Destruction
public:
	//
	// @CMember default ctor
	RGrid();
	//
	// @CMember ctor with cell dimensions
	RGrid( const RIntSize& cellcounts );
	//
	// @CMember copy ctor
	RGrid( const RGrid& grid );
	//
	// @CMember dtor
	virtual ~RGrid();

//
// @Access Operations
public:
	//
	// @CMember assignment operator
	const RGrid& operator=( const RGrid& rhs );
	//
	// @CMember read the grid settings from the given storage object
	virtual void		Read( RArchive& ar );
	//
	// @CMember write the grid settings to the given storage object
	virtual void		Write( RArchive& ar ) const;
	//
	// @CMember Set the cell counts for the grid
	void					SetCellCounts( const RIntSize& cellCounts );
	//
	// @CMember Set the logical width of a line type
	void					SetLineWidth( EGridItem eItem, YRealDimension nWidth );
	//
	// @CMember Set the color for a line type
	void					SetLineColor( EGridItem eItem, const RColor& rgb );
	//
	// @CMember Set the height of the title area
	void					SetTitleHeight( YPercentage pctHeight );
	//
	// @CMember Set the height of the header area
	void					SetHeaderHeight( YPercentage pctHeight );
	//
	// @CMember Set the width of the row label area
	void					SetLabelWidth( YPercentage pctWidth );
	//
	// @CMember Set the grid hit test boundary
	void					SetGridHitBoundary( YIntDimension nHitBoundary );
	//
	// @CMember Set the fill color specification for the given item
	void					SetItemFill( const RColor& clr, EGridItem eItem, int idxItem=0 );
	//
	// @CMember Set or clear the given flag for the given cell
	virtual void		SetCellFlag( int nCell, uLONG ulFlag, BOOLEAN fSet );
	//
	// @CMember Render the grid
	virtual void		Render( RDrawingSurface& drawingSurface,
										const RRealSize& gridSize,
										const R2dTransform& transform ) const;
	//
	// @CMember Render a single line
	virtual void		RenderLine( RDrawingSurface& drawingSurface,
											const RRealSize& gridSize,
											const R2dTransform& transform,
											YRealDimension dInsetEnds,
											EGridItem eItem,
											int idxItem ) const;
	//
	// @CMember Render a single frame
	virtual void		RenderFrame( RDrawingSurface& drawingSurface,
											 const RRealSize& gridSize,
											 const R2dTransform& transform,
											 EGridItem eItem,
											 int idxItem ) const;
	//
	// @CMember Render the fill for a single item
	virtual void		RenderFill( RDrawingSurface& drawingSurface,
											const RRealSize& gridSize,
											const R2dTransform& transform,
											EGridItem eItem,
											int idxItem=0 ) const;
//
// @Access Accessors
public:
	//
	// @CMember Get the cell counts for the grid
	const RIntSize&		GetCellCounts() const;
	//
	// @CMember Get the logical width for a given type of line
	YRealDimension			GetLineWidth( EGridItem eItem ) const;
	//
	// @CMember Get the color for a given type of line
	const RColor&			GetLineColor( EGridItem eItem ) const;
	//
	// @CMember Set the height of the title area
	YPercentage				GetTitleHeight() const;
	//
	// @CMember Set the height of the header area
	YPercentage				GetHeaderHeight() const;
	//
	// @CMember Get the width of the row label area
	YPercentage				GetLabelWidth() const;
	//
	// @CMember Get the grid hit test boundary
	YIntDimension			GetGridHitBoundary() const;
	//
	// @CMember Get the fill color specification for the given item
	const RColor&			GetItemFill( EGridItem eItem, int idxItem=0 ) const;
	//
	// @CMember Get the bounds of the given item
	virtual RRealRect		GetItemRect( const RRealSize& gridSize, EGridItem eItem, int idxItem=0 ) const;
	//
	// @CMember Get the dimensions of the given item
	RRealSize				GetItemDimensions( const RRealSize& gridSize, EGridItem eItem ) const;
	//
	// @CMember Compute the required grid dimensions given the desired cell size
	RRealSize				ComputeGridDimensions( const RRealSize& cellSize ) const;
	//
	// @CMember Get the given cell's position
	BOOLEAN					GetCellPosition( int nCell, RIntSize& cellPos ) const;
	//
	// @CMember Get the given cell's index
	BOOLEAN					GetCellIndex( const RIntSize& cellPos, int& nCell ) const;
	//
	// @CMember Get the given flag for the given cell
	virtual BOOLEAN		GetCellFlag( int nCell, uLONG ulFlag ) const;
	//
	// @CMember Test if the given item is visible
	BOOLEAN					IsVisible( RDrawingSurface& drawingSurface, const RRealSize& gridSize, const R2dTransform& transform, EGridItem eItem, int idxItem=0 ) const;
	//
	// @CMember Hit test the grid
	virtual EGridItem		HitTestGrid( const RRealSize& gridSize, const R2dTransform& xToDevice, const RIntPoint& ptTest, int& nItem ) const;
	//
	// @CMember Fill in a font specification structure
	virtual const YFont&	CreateFontSpec( const RRealSize& gridSize,
													 EGridItem eArea,
													 YFont& font,
													 const RMBCString& szTypeface,
													 const RSolidColor& rgb,
													 YPercentage pctHeight ) const;

//
// @Access Grid Data
protected:
	//
	// @CMember Line width table
	YRealDimension		m_LineWidthTable[ kGridItemCount ];
	//
	// @CMember Line color table
	RColor				m_LineColorTable[ kGridItemCount ];
	//
	// @CMember Create the fill color array
	void					SetupDefaultColors();
	//
	// @CMember Get the number of entries in the fill color array
	int					GetFillColorCount() const;
	//
	// @CMember Dimensions of the grid in cells
	RIntSize				m_CellCounts;
	//
	// @CMember Height of the title area as a percentage of cell height
	YPercentage			m_pctTitleHeight;
	//
	// @CMember Height of the header area as a percentage of cell height
	YPercentage			m_pctHeaderHeight;
	//
	// @CMember Width of the row label area as a percentage of cell width
	YPercentage			m_pctLabelWidth;
	//
	// @CMember Grid hit test range as logical units
	YIntDimension		m_nGridHitBoundary;
	//
	// @CMember Array of background fill colors for the cells and areas
	RColor*			m_pFillColors;
};

#ifndef _DEBUG
#include	"Grid.inl"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // !defined(_Grid_H_)