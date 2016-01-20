//****************************************************************************
//
// File Name:  WinGridCtrl.cpp
//
// Project:    Renaissance
//
// Author:     Lance Wilson
//
// Description: Implementation for a multi-column grid control.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//	              500 Redwood Blvd.
//               Novato, CA 94948
//	              (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/WinGridCtrl.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"
ASSERTNAME

#ifndef	_WINDOWS
	#error	This file must be compilied only on Windows
#endif	//	_WINDOWS

#include "WinGridCtrl.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int kDefaultWidth  = 75 ;
const int kDefaultHeight = 50 ;
const int kDefaultColCount = 1 ;

//*****************************************************************************
//
// Function Name:  RGridCtrlBase::RGridCtrlBase
//
// Description:    Constructor (Default).
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
RGridCtrlBase::RGridCtrlBase( UINT uiStyles )
{
	m_cxCellHeight  = 0 ;
	m_cxCellWidth   = 0 ;
	m_cxItemCount   = 0 ;

	m_cxVisibleRows = 0 ;
	m_cxVisibleCols = 0 ;

	m_nTopCellRow   = 0 ;
	m_nTopCellCol   = 0 ;

	m_nCurCell = LB_ERR ;
	m_nNumCols = kDefaultColCount ;
	m_nNumRows = 0 ;

	m_uiFlags      = uiStyles ;
	m_fResizeCells = FALSE ;

	m_bSetRedraw = TRUE ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrlBase::RGridCtrlBase
//
// Description:    Destructor.
//
// Returns:		    Nothing
//
// Exceptions:	    None
//
//*****************************************************************************
RGridCtrlBase::~RGridCtrlBase()
{
}

BEGIN_MESSAGE_MAP(RGridCtrlBase, CWnd)
	//{{AFX_MSG_MAP(RGridCtrlBase)
	ON_MESSAGE( LB_GETSEL, OnLbGetSel )
	ON_MESSAGE( LB_GETCOUNT, OnLbGetCount )
	ON_MESSAGE( LB_SETCURSEL, OnLbSetCurSel )
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*****************************************************************************
//
// Function Name: RGridCtrlBase::CellToIndex( int nRow, int nCol )
//
// Description:   Determines the cell index from a row, column specifier.
//                Note, does not adjust for scrolling.  Cell(0,0) always
//						applies to the zeroth item in the list.
//
// Returns:	      item index of the cell specified by nRow, nCol if it
//						is a valid index; otherwise LB_ERR
//
// Exceptions:	   None
//
//*****************************************************************************
int RGridCtrlBase::CellToIndex( int nRow, int nCol ) const
{
	// Determine the cell index from grid coordinates
	nCol += (nRow * m_nNumCols) ;

	if (nCol < GetCount())
	{
		return nCol ;
	}

	return LB_ERR ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrlBase::GetCount()
//
// Description:    Retrieves the number of items in a list box. The
//                 returned count is one greater than the index 
//                 value of the last item (the index is zero-based).
//
// Returns:	       The number of items in the grid control, or 
//                 LB_ERR if an error occurs.
//
// Exceptions:	    None
//
//*****************************************************************************
int RGridCtrlBase::GetCount() const
{

	///////////////////////////////////////////////
	// The m_cxItemCount data member is only set //
	// when the control does not store the data  //
	// that is to be displayed.                  //
	///////////////////////////////////////////////

	// No data stored in the control, so
	// return the specified item count.
	return m_cxItemCount ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrlBase::GetCellHeight()
//
// Description:    An application calls the GetCellHeight member 
//                 function to determine the height of cells in a 
//                 grid control.
//
// Returns:	       The height, in pixels, of the cells in the grid.
//
// Exceptions:	    None
//
//*****************************************************************************
int RGridCtrlBase::GetCellHeight() const
{
	return m_cxCellHeight ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::GetCellRect( int nRow, int nCol, LPRECT lpRect )
//
// Description:   Determines a cell's rectangle, in pixels, regardless of
//						whether or not the cell is currently displayed
//
// Returns:	      None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::GetCellRect( int nRow, int nCol, LPRECT lpRect ) const
{
	lpRect->top    = nRow * m_cxCellHeight ;
	lpRect->left   = nCol * m_cxCellWidth ;
	lpRect->bottom = lpRect->top  + m_cxCellHeight - 1 ;
	lpRect->right  = lpRect->left + m_cxCellWidth  - 1 ;
}

//*****************************************************************************
//
// Function Name:	RGridCtrlBase::GetCellWidth()
//
// Description:   An application calls the GetCellWidth member 
//                function to determine the width of cells in a 
//                grid control.
//
// Returns:	      The width, in pixels, of the cells in the grid.
//
// Exceptions:	   None
//
//*****************************************************************************
int RGridCtrlBase::GetCellWidth() const
{
	return m_cxCellWidth ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::GetCurSel()
//
// Description:   Retrieves the zero-based index of the currently 
//                selected item, if any, in a single-selection 
//						grid control. 
//
// Returns:	      The zero-based index of the currently selected 
//                item. It is LB_ERR if no item is currently 
//                selected or if the grid control is a multiple-
//                selection list box.
//
// Exceptions:	   None
//
//*****************************************************************************
int RGridCtrlBase::GetCurSel() const
{
	return m_nCurCell ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::GetItemRect(int nIndex, LPRECT lpRect)
//
// Description:   Retrieves the dimensions of the rectangle that 
//                bounds a grid control item as it is currently 
//                displayed in the list-box window.
//
// Returns:	      LB_ERR if an error occurs.
//
// Exceptions:	   None
//
//*****************************************************************************
int RGridCtrlBase::GetItemRect( int nIndex, LPRECT lpRect ) const
{
	if (nIndex < GetCount())
	{
		nIndex -= GetTopIndex() ;
		GetCellRect( nIndex / m_nNumCols, nIndex % m_nNumCols, lpRect ) ;

		CRect rect, rectSect ;
		GetClientRect( rect ) ;

		if (m_uiFlags & kAddSpacing)
		{
			RECT& rect = *lpRect ;

			rect.top    += 2 ; 
			rect.left   += 2 ;
//			rect.right  -= 2 ;
//			rect.bottom -= 2 ;
		}

		if (rectSect.IntersectRect (&rect, lpRect))
		{
			//
			// If the integral flag is set, make sure the entire
			// item is visible; otherwise we need to return an error.
			//
			if (!(m_uiFlags & kIntegral) || rectSect == *lpRect)
			{
				return 0 ;
			}
		}
	}

	return LB_ERR ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::GetItemState( int nIndex )
//
// Description:   Determines the state of the item speicifed by nIndex
//
// Returns:	      The selection/focus state of the item specified by nIndex
//
// Exceptions:	   None
//
//*****************************************************************************
UINT RGridCtrlBase::GetItemState( int nIndex ) const
{
	UINT uiItemState = 0 ;
	int  nCurCell = GetCurSel() ;

	if ((CWnd *) this == GetFocus())
	{
		if ((LB_ERR == nCurCell && 0 == nIndex) || nIndex == nCurCell)
		{
			uiItemState |= ODS_FOCUS ;
		}
	}

	if (nIndex == nCurCell)
	{
		uiItemState |= ODS_SELECTED ;
	}

	return uiItemState ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::GetTopIndex()
//
// Description:   Retrieves the zero-based index of the top left 
//                visible item in a grid control. Initially, item 
//                0 is at the top left of the grid, but if the 
//                list box is scrolled, another item may be at the 
//                top.
//
// Returns:	      The zero-based index of the top-left visible 
//                item in the grid if successful, LB_ERR otherwise.
//
// Exceptions:	   None
//
//*****************************************************************************
int RGridCtrlBase::GetTopIndex() const
{
	return m_nTopCellRow * m_nNumCols + m_nTopCellCol ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::InitStorage( int nNumItems )
//
// Description:   Tells the grid control the number of items to be 
//                displayed.
//
// Returns:	      the number of items to be displayed
//
// Exceptions:	   None
//
//*****************************************************************************
int RGridCtrlBase::InitStorage( int nNumItems )
{
	int nOldCount = m_cxItemCount ;
	m_cxItemCount = nNumItems ;

	if (GetCurSel() >= nNumItems)
	{
		m_nCurCell = LB_ERR ;
	}

	OnItemAdded( min( nOldCount, nNumItems ) ) ;

	return m_cxItemCount ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::ItemFromPoint( CPoint& pt, BOOL& bOutside )
//
// Description:   Call this function to determine the grid item 
//                nearest the point specified in pt. You could 
//                use this function to determine which grid item 
//                the mouse cursor moves over.
//
// Returns:	      The index of the nearest item to the point 
//                specified in pt.
//
// Exceptions:	   None
//
//*****************************************************************************
UINT RGridCtrlBase::ItemFromPoint( CPoint& pt, BOOL& fOutside ) const
{
	CRect rect ;
	GetClientRect( rect ) ;
	fOutside = FALSE ;

	if (!rect.PtInRect( pt ))
		fOutside = TRUE ;

	if (!GetCount())
		return 0 ;

	// Determine the actual number of rows visible
	///////////////////////////////////////////////
	int nVisibleRows = GetCount() / m_nNumCols ;

	// Determine the row and column
	///////////////////////////////////////////////
	int nRow = min( m_cxVisibleRows, max( pt.y / m_cxCellHeight, 0 ) ) ;
	int nCol = min( m_cxVisibleCols, max( pt.x / m_cxCellWidth, 0 ) ) ;

	nCol = min( nCol, m_nNumCols - 1 ) ;
	nRow = min( nRow, GetCount() / m_nNumCols ) ;	// REVIEW: store num rows

	int    nOffset = CellToIndex( nRow, nCol ) ;

	if (LB_ERR == nOffset)
	{
		// Point is in empty space, so see if we are closer to 
		// the row above us, or the column to the left of us.
		////////////////////////////////////////////////////////

		int nLastCell = (GetTopIndex() % m_nNumCols)  ;
		int nColDiff  = pt.x - (nLastCell * m_cxCellWidth) ;

		if (GetCount() < m_cxVisibleCols || nColDiff <= pt.y % m_cxCellHeight )
		{
			nCol = nLastCell ;
		}
		else
		{
			nRow-- ;
		}
	}

	UINT nItem = GetTopIndex() + CellToIndex( nRow, nCol ) ;

	if (m_uiFlags & kIntegral)
	{
		if (LB_ERR == GetItemRect( nItem, rect ))
		{
			fOutside = TRUE ;
		}
	}

	return nItem ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::SendDrawItem( CDC& dc, UINT uiItemID, UINT uiItemAction )
//
// Description:   Helper function for calling the drawitem method.
//
// Returns:	      None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::SendDrawItem( CDC& dc, UINT uiItemID, UINT uiItemAction ) const
{
	DRAWITEMSTRUCT item ;

	if (LB_ERR == GetItemRect( uiItemID, &item.rcItem ))
		return ;

	if (m_bSetRedraw == FALSE)
		return;

	//
	// Draw the items
	//
	item.CtlType    = ODT_LISTBOX ; 
   item.CtlID	    = GetDlgCtrlID() ; 
   item.itemID     = uiItemID ; 
   item.itemAction = uiItemAction ; 
   item.itemState  = GetItemState( uiItemID ) ; 
   item.hwndItem   = m_hWnd ; 
   item.hDC		= (HDC) dc ; 

	if (m_uiFlags & kPushLike)
	{
		DrawPushLikeCell( &item ) ;
	}

	const_cast <RGridCtrlBase *>(this)->DrawItem( &item ) ;
}

//*****************************************************************************
//
// Function Name: DrawPushLikeCell( LPDRAWITEMSTRUCT lpDrawItemStruct )
//
// Description:   An application calls the SetCellDimensions member 
//                function to set the height and width of cells in 
//                a grid control. 
//
// Returns:	      None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::DrawPushLikeCell( LPDRAWITEMSTRUCT lpDrawItemStruct ) const
{
	CDC dc ;
	dc.Attach( lpDrawItemStruct->hDC ) ;

	RECT& rect = lpDrawItemStruct->rcItem ;
//	CRect rectTemp = rect ;

	//
	// Erase the background
	//
	dc.FillSolidRect( &rect, GetSysColor( COLOR_BTNFACE ) ) ;

	//
	// Draw the cell in a pushlike state
	//
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		dc.Draw3dRect( rect.left, rect.top,
			(rect.right - rect.left + 1), 
			(rect.bottom - rect.top + 1),
			RGB( 0, 0, 0 ),	GetSysColor( COLOR_BTNHILIGHT ) ) ;

		CPen pen(PS_SOLID, 1, GetSysColor( COLOR_BTNSHADOW )) ;
		CPen *pOldPen = dc.SelectObject( &pen ) ;

		dc.MoveTo( rect.left + 1, rect.bottom - 1 ) ;
		dc.LineTo( rect.left + 1, rect.top + 1 ) ;
		dc.LineTo( rect.right, rect.top + 1) ;

		dc.SelectObject( pOldPen ) ;

		// Update the drawing rect
		rect.top += 2 ;	rect.left += 2 ;
		rect.bottom-- ; rect.right-- ;

		try
		{
			/*static*/ CBitmap bmpPattern ;

//			if (!bmpPattern.GetSafeHandle())
//			{
				static BITMAP   bitmap = { 0, 8, 8, 2, 1, 1 } ;
				static WORD wPatternBits[] = 
				{ 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA } ;

				bitmap.bmBits = (LPSTR) wPatternBits ;
				bmpPattern.CreateBitmapIndirect (&bitmap) ;
//			}

			CBrush   brush (&bmpPattern) ;
			COLORREF crOldBackColor = dc.SetBkColor(GetSysColor (COLOR_BTNFACE)) ;
			COLORREF crOldTextColor = dc.SetTextColor(GetSysColor (COLOR_BTNHILIGHT)) ;

			CRect rectTemp(rect.left + 1, rect.top + 1, rect.right, rect.bottom) ;

			dc.FillRect (&rectTemp, &brush) ;
			dc.SetBkColor (crOldBackColor) ;
			dc.SetTextColor (crOldTextColor) ;
		}
		catch (CResourceException& /*e*/)
		{
			// REVIEW: Put some type of error message here?
		}
	}
	else 
	{
		dc.Draw3dRect( &rect, 
			GetSysColor( COLOR_BTNHILIGHT ),
			GetSysColor( COLOR_BTNSHADOW ) ) ;

		dc.MoveTo( rect.left, rect.bottom ) ;
		dc.LineTo( rect.right, rect.bottom ) ;
		dc.LineTo( rect.right, rect.top - 1 ) ;

		// Update the drawing rect
		rect.bottom -= 2 ;	rect.right -= 2 ;
		rect.top++ ; rect.left++ ;
	}

	dc.Detach() ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::SetCellDimensions( int cxWidth, int cxHeight )
//
// Description:   An application calls the SetCellDimensions member 
//                function to set the height and width of cells in 
//                a grid control. 
//
// Returns:	      None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::SetCellDimensions( int cxWidth, int cxHeight )
{
	CRect rect ;
	GetClientRect( rect ) ;

	m_cxCellWidth  = (WORD) cxWidth ;
	m_cxCellHeight = (WORD) cxHeight ;

	m_cxVisibleRows = (WORD)(rect.Height() / cxHeight) ;
	m_cxVisibleCols = (WORD)(rect.Width() / cxWidth) ;

	Invalidate( TRUE ) ;
	UpdateScrollBars( ) ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::SetCurSel()
//
// Description:   Selects an item and scrolls it into view, if 
//                necessary. When the new item is selected, the 
//                grid control removes the highlight from the 
//                previously selected item. 
//
//                Use this member function only with single-
//                selection grid controls. It cannot be used to 
//                set or remove a selection in a multiple-
//                selection grid control.
//
// Returns:	      LB_ERR if an error occurs.
//
// Exceptions:	   None
//
//*****************************************************************************
int RGridCtrlBase::SetCurSel( int nIndex )
{
	if (nIndex >= GetCount())
		return LB_ERR ;

	int nOldCell    = m_nCurCell ;

	CClientDC dc(this) ;

	CRect rect ;
	GetClientRect( rect ) ;
//	dc.IntersectClipRect( rect ) ;

	// Update the current selection
	m_nCurCell = nIndex ;

	if (LB_ERR != nOldCell)
	{
		// Reset the current selection
		SendDrawItem( dc, nOldCell, ODA_SELECT ) ;
	}
	else if ((CWnd *) this == GetFocus())
	{
		// When the list receives focus and there is no current
		// selection, the first item in the list is displayed
		// with focus, but unselected.  So in this case, we
		// need to go ahead and clear the input focus from this cell.
		SendDrawItem( dc, 0, ODA_FOCUS ) ;
	}

	if (LB_ERR != nIndex)
	{
		CRect rectItem ;

		
		if ((m_nTopCellRow * m_nNumCols) > nIndex)
		{
			// make the new item visible if it is before the existing
			// top index item
			SetTopIndex(nIndex);
		}

		GetItemRect( nIndex, rectItem ) ;

		int nTopRow = m_nTopCellRow ;
		int nTopCol = m_nTopCellCol ;

		if (rectItem.right > rect.right)
		{
			nTopCol += (rectItem.right - rect.right) / m_cxCellWidth + 1 ;
		}

		if (rectItem.bottom > rect.bottom)
		{
			nTopRow += (rectItem.bottom - rect.bottom) / m_cxCellHeight + 1 ;
		}

		if (nTopRow != m_nTopCellRow || nTopCol != m_nTopCellCol)
		{
			SetTopIndex( nTopRow * m_nNumCols + nTopCol ) ;
		}

		// Update the newly selected item
		SendDrawItem( dc, nIndex, ODA_SELECT ) ;
	}

//	dc.SelectClipRgn( NULL ) ;

	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::SizeCellsToClient( int nNumRows, int nNumCols )
//
// Description:   Adjusts the cell width and height so that the
//					   number of rows and columns, specified by nNumRows
//                and nNumCols respectively, will fit into the 
//                client area.
//
// Returns:	      None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::SizeCellsToClient( int nNumRows, int nNumCols )
{
	ASSERT( GetSafeHwnd() ) ;

	CRect rect ;
	GetClientRect( rect ) ;

	// Set the number of cells per row.  
	SetNumCols( nNumCols ) ;
	m_fResizeCells = TRUE ;

	// Reset the cell dimensions 
	// with the new calculations
	SetCellDimensions( 
		(rect.Width()  / nNumCols) - 1, 
		(rect.Height() / nNumRows) - 1 ) ;

	m_cxVisibleRows = WORD( rect.Height() / m_cxCellHeight ) ;
	m_cxVisibleCols = WORD( rect.Width()  / m_cxCellWidth ) ;

	UpdateMetrics() ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrlBase::SetNumCols( int nNumCols )
//
// Description:    Sets the number of columns to be displayed.  If the
//                 number of columns specified is more then can visibly
//                 be displayed, a horizontal scroll bar is added.
//
// Returns:	       None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::SetNumCols( int nNumCols )
{
	m_fResizeCells = FALSE ;

	if (m_nNumCols != nNumCols)
	{
		m_nNumCols   = (WORD) nNumCols ;
		UpdateScrollBars() ;

		if (m_nNumCols <= m_cxVisibleCols)
		{
			InvalidateRect( NULL, FALSE ) ;
		}
	}
}

//*****************************************************************************
//
// Function Name:  RGridCtrlBase::SetTopIndex()
//
// Description:    Ensures that a particular grid item is visible. 
//
//                 The system scrolls the grid until either the 
//                 item specified by nIndex appears in the top row 
//                 of the grid or the maximum scroll range has been 
//                 reached.  
//
//                 Note, GetTopIndex returns the top left visible
//				       item, where SetTopIndex sets the row of nIndex
//                 So, GetTopIndex may return a different value 
//                 then what it was "set" to using this method.
//
// Returns:	       Zero if successful, or LB_ERR if an error occurs.
//
// Exceptions:	    None
//
//*****************************************************************************
int RGridCtrlBase::SetTopIndex( int nIndex )
{
	if (nIndex >= GetCount())
	{
		return LB_ERR ;
	}

	//
	// Determine the old row, and the new row for use 
	// in determining whether or not the window can
	// be scrolled, and if so, how much.
	//
	int nOldTopRow = m_nTopCellRow ; 
	int nNewTopRow = nIndex / m_nNumCols ;

	if (nOldTopRow == nNewTopRow)
	{
		return 0 ;
	}

	CClientDC dc( this ) ;
	BOOLEAN fFocusRemoved = FALSE ;

	if ((CWnd *) this == GetFocus())
	{
		// Focus rect must first be removed before scrolling
		SendDrawItem( dc, max( 0, GetCurSel() ), ODA_FOCUS ) ;
		fFocusRemoved = TRUE ;
	}

	//
	// Make sure the top row is not beyond
	// the maximum scroll range.
	//
	int nMinPos, nMaxPos ;

	GetScrollRange( SB_VERT, &nMinPos, &nMaxPos ) ;

	if (nNewTopRow > nMaxPos)
		nNewTopRow = nMaxPos ;

	// Update the top index
	m_nTopCellRow = nNewTopRow ; 

	// Update the scroll bar
	SetScrollPos( SB_VERT, nNewTopRow ) ;

	int nOldTopCol = m_nTopCellCol ; //GetTopIndex() / m_nNumCols ;
	int nNewTopCol = nIndex % m_nNumCols ;

/*	if (m_uiFlags & kColMajorOrder)
	{
		nNewTopCol = nIndex / m_nNumCols ;
	}*/

	//
	// Make sure the top col is not beyond
	// the maximum scroll range.
	//
	GetScrollRange( SB_HORZ, &nMinPos, &nMaxPos ) ;

	if (nNewTopCol > nMaxPos)
		nNewTopCol = nMaxPos ;

	// Update the top index
	m_nTopCellCol = nNewTopCol ; //nNewTopRow * m_nNumCols ;

	// Update the scroll bar
	if (nMaxPos)
	{
		SetScrollPos( SB_HORZ, nNewTopCol ) ;
	}

	// Update the display
	///////////////////////

	CRect rect ;
	GetClientRect( rect ) ;

	//
	// Determine if the window can be scrolled.
	//
	int nRowDiff = nNewTopRow - nOldTopRow ;
	int nColDiff = nNewTopCol - nOldTopCol ;

	if (abs( nRowDiff ) < m_cxVisibleRows)
	{
		// Scroll the window
		ScrollWindow( 0, -(nRowDiff * m_cxCellHeight), rect, rect ) ;

		if (nRowDiff < 0)
		{
			// Scrolled up, so invalidate the top
			rect.bottom = rect.top + (nRowDiff * m_cxCellHeight) ;
		}
		else
		{
			// Scrolled down, so invalidate the bottom
			rect.top = (m_cxVisibleRows - nRowDiff) * m_cxCellHeight ;
		}
	}

	InvalidateRect( rect, TRUE ) ;
	GetClientRect( rect ) ;

	if (nColDiff && abs( nColDiff ) < m_cxVisibleCols)
	{
		// Scroll the window
		ScrollWindow( -(nColDiff * m_cxCellWidth), 0, rect, rect ) ;

		if (nColDiff < 0)
		{
			// Scrolled up, so invalidate the top
			rect.right = rect.left + (nColDiff * m_cxCellWidth) ;
		}
		else
		{
			// Scrolled down, so invalidate the bottom
			rect.top = (m_cxVisibleCols - nColDiff) * m_cxCellWidth ;
		}
	}

	if (fFocusRemoved)
	{
		// Focus rect must first be removed before scrolling
		SendDrawItem( dc, max( 0, GetCurSel() ), ODA_FOCUS ) ;
	}

	InvalidateRect( rect, TRUE ) ;
	UpdateWindow() ;

	return TRUE ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::UpdateScrollBars()
//
// Description:   Updates the scroll bars, as necessary, with the
//                appropriate data ranges.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::UpdateScrollBars()
{
	int cxItemCount = GetCount() ;
//	int nScrollMin, nScrollMax ;

	// Update horizontal scroll bar if necessary.
	/////////////////////////////////////////////

	// Determine the number of columns needed
	int nColsNeeded = max( 0, m_nNumCols - m_cxVisibleCols ) ;

	if (cxItemCount != m_cxVisibleCols && nColsNeeded >= 0)
	{ 
		SetScrollRange( SB_HORZ, 0, nColsNeeded ) ;
	}

	// Update vertical scroll bar if necessary.
	////////////////////////////////////////////

	// Determine the rows needed.  Note, by rounding up 
	// the last item position by m_nNumCols, we won't 
	// lose any data when using integer arithmetic.
	int nRowsNeeded = max(0, ((cxItemCount - 1 + m_nNumCols) / 
		m_nNumCols) - m_cxVisibleRows) ;

	SetScrollRange( SB_VERT, 0, nRowsNeeded ) ;

#if 0
	GetScrollRange( SB_VERT, &nScrollMin, &nScrollMax ) ;

	// Determine the rows needed.  Note, by rounding up 
	// the last item position by m_nNumCols, we won't 
	// lose any data when using integer arithmetic.
	int nRowsNeeded = max(0, ((cxItemCount - 1 + m_nNumCols) / 
		m_nNumCols) - m_cxVisibleRows) ;

	if (nRowsNeeded != nScrollMax)
	{

		SetScrollRange( SB_VERT, 0, nRowsNeeded ) ;

		if (nScrollMax == 0  && nRowsNeeded)/* ||
			 (nRowsNeeded == 0 && nScrollMax))*/
		{
			if (m_fResizeCells)
			{
				// The grid area has changed, so resize the cells
				SizeCellsToClient( m_cxVisibleRows, m_cxVisibleCols ) ;
			}
/*			else
			{
				// The grid area has changed, see
				// if cells should be resized
				UpdateMetrics() ;
			}
*/
		}
	}
#endif
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::UpdateMetrics()
//
// Description:   Determines the cell metrics from of first item in
//                the list.  This data is then used to update the
//                internal cell metric information.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::UpdateMetrics()
{
	if (GetCount() && !m_fResizeCells)
	{
		MEASUREITEMSTRUCT itemMeasure ; 

		itemMeasure.CtlType    = ODT_LISTBOX ;
		itemMeasure.CtlID      = GetDlgCtrlID() ;
		itemMeasure.itemID     = 0 ;
		itemMeasure.itemWidth  = kDefaultWidth ;
		itemMeasure.itemHeight = kDefaultHeight ;

		MeasureItem( &itemMeasure ) ;

		if (m_uiFlags & kPushLike)
		{
			itemMeasure.itemWidth  += 3 ;
			itemMeasure.itemHeight += 3 ;
		}

		if (m_uiFlags & kAddSpacing)
		{
			itemMeasure.itemWidth  += 2 ;
			itemMeasure.itemHeight += 2 ;
		}

		if (itemMeasure.itemWidth != m_cxCellWidth || itemMeasure.itemHeight != m_cxCellHeight)
		{
			SetCellDimensions( itemMeasure.itemWidth, itemMeasure.itemHeight ) ;
		}
	}
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnItemAdded( int nIndex )
//
// Description:   Helper function for performing all the necessary 
//                updates to the control each time an item is added.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnItemAdded( int nIndex )
{
	if (0 == nIndex)
	{
		// Measure the item.  This is only 
		// necessary on the first addition.
		UpdateMetrics() ;
	}

	// Update the scroll bars as necessary.
	UpdateScrollBars() ;

	//
	// Determine if the current selection needs moved.
	//
	if (GetCurSel() >= nIndex)
	{
		SetCurSel( GetCurSel() + 1 ) ;
	}

	//
	// Determine if the display needs updated
	//
	CRect rect ;

	if (LB_ERR != GetItemRect( nIndex, rect ))
	{
		InvalidateRect( rect, TRUE ) ;
	}

/*	int nItemRow = nIndex / m_nNumCols ;
	int nTopRow  = GetTopIndex() / m_nNumCols ;

	if (nItemRow <= nTopRow + m_cxVisibleRows)
	{
		InvalidateRect( NULL ) ;
	}
*/
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnKillFocus(CWnd* pNewWnd) 
//
// Description:   Message handler for WM_KILLFOCUS.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnKillFocus(CWnd* ) 
{
	if (::IsWindow( GetSafeHwnd() ))
	{
		CWnd* pParent = GetParent() ;

		if (pParent)
		{
			// Notify the parent that the grid control
			// is losing the keyboard input focus.
			pParent->PostMessage( WM_COMMAND, 
				MAKEWPARAM( GetDlgCtrlID(), LBN_KILLFOCUS ),
				(LPARAM) m_hWnd ) ;
		}

		CClientDC dc(this) ;
		SendDrawItem( dc, max( 0, GetCurSel() ), ODA_FOCUS ) ;
	}
	else
	{
		TpsAssertAlways( "Invalid Window Handle!" ) ;
	}
}

//*****************************************************************************
//
// Function Name: OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
//
// Description:   Message handler for WM_CHAR.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnChar( UINT nChar, UINT, UINT )
{
	CharToItem( nChar, GetCurSel() ) ;
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
//
// Description:   Message handler for WM_KEYDOWN.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnKeyDown(UINT nChar, UINT, UINT) 
{
	int nCurSel = GetCurSel() ;

	int nRowOffset = 0 ;
	int nColOffset = 0 ;

	int nCurRow = nCurSel / m_nNumCols ;
	int nCurCol = nCurSel % m_nNumCols ;

	switch (nChar)
	{
	case VK_UP:
		nRowOffset = -1;
		break ;

	case VK_DOWN:
		nRowOffset = 1 ;
		break ;

	case VK_LEFT:
		nColOffset = -1 ;
		break ;

	case VK_RIGHT:
		nColOffset = 1 ;
		break ;

	case VK_PRIOR:
		nRowOffset = -m_cxVisibleRows ;
		break ;

	case VK_HOME:
		nRowOffset = -nCurRow ;
		nColOffset = -nCurCol ;
		break ;

	case VK_NEXT:
		nRowOffset = m_cxVisibleRows ;
		break ;

	case VK_END:
		nRowOffset = GetCount() / m_nNumCols - nCurRow ;
		nColOffset = GetCount() % m_nNumCols - nCurCol ;
		break ;

	default:

		// See if we're suppose to send it off to the parent
		if (GetStyle() & LBS_WANTKEYBOARDINPUT)
		{
			VKeyToItem( nChar, GetCurSel() ) ;
		}

		return ;
	}

	if (nCurSel < 0)
	{
		//
		// If there was no previously selected item, default
		// to the first item in the list.
		//

		nCurRow = nCurCol = 0 ;
		nRowOffset = nColOffset = 0 ;
	}
	else
	{
		//
		// Determine the new row and column, making sure to
		// keep them within bounds
		//

		nCurRow += nRowOffset ;
		nCurRow = max( 0, min( nCurRow, GetCount() / m_nNumCols ) ) ;
		
		nCurCol += nColOffset ;
		nCurCol = max( 0, min( nCurCol, m_nNumCols - 1 ) ) ;

		if (nCurRow == GetCount() / m_nNumCols)
		{
			nCurCol = min( nCurCol, GetCount() % m_nNumCols - 1 ) ;
		}
	}

	int nNewSel = nCurRow * m_nNumCols + nCurCol ;

	if (nNewSel != nCurSel)
	{

		if (nCurRow < m_nTopCellRow || nCurRow >= m_nTopCellRow + m_cxVisibleRows ||
			 nCurCol < m_nTopCellCol || nCurCol >= m_nTopCellCol + m_cxVisibleCols)
		{
			SetTopIndex( max( 0, GetTopIndex() + (nNewSel - nCurSel) )) ;
		}

		SetCurSel( nNewSel ) ;

		if (GetStyle() & LBS_NOTIFY)
		{
			GetParent()->PostMessage(WM_COMMAND,
				MAKEWPARAM( GetDlgCtrlID(), LBN_SELCHANGE ), 
				(LPARAM) GetSafeHwnd()) ;
		}
	}
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
//
// Description:   Message handler for WM_LBUTTONDBLCLK.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnLButtonDblClk(UINT, CPoint point) 
{
	if (GetStyle() & LBS_NOTIFY)
	{
		BOOL fOutsideRect ;
		int  nIndex = ItemFromPoint( point, fOutsideRect ) ;

		if (nIndex == GetCurSel())
		{
			GetParent()->PostMessage(WM_COMMAND, 
				MAKEWPARAM( GetDlgCtrlID(), LBN_DBLCLK ), 
				(LPARAM) GetSafeHwnd()) ;
		}
	}
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
//
// Description:   Message handler for WM_LBUTTONDOWN.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnLButtonDown(UINT, CPoint point) 
{
	BOOL fOutsideRect ;
	int  nIndex = ItemFromPoint( point, fOutsideRect ) ;

//	CRect rect ;
//	GetItemRect( nIndex, rect ) ;

	if (!fOutsideRect /*rect.PtInRect( point )*/)
	{
		if (LB_ERR != nIndex)
		{
			SetCurSel( nIndex ) ;
		}

		if (GetStyle() & LBS_NOTIFY)
		{
			GetParent()->PostMessage(WM_COMMAND,
				MAKEWPARAM( GetDlgCtrlID(), LBN_SELCHANGE ), 
				(LPARAM) GetSafeHwnd()) ;
		}
	}

	if ((GetStyle() & WS_TABSTOP) && ((CWnd *) this != GetFocus()))
	{
		SetFocus() ;
	}
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnPaint() 
//
// Description:   Message handler for WM_PAINT
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (m_bSetRedraw == FALSE)
		return;

	// Get the invalid rect.
	CRect rectInvalid = dc.m_ps.rcPaint ;

	if (!rectInvalid.IsRectEmpty( ))
		GetClientRect( rectInvalid ) ;

//	dc.IntersectClipRect( rectInvalid ) ;

	if (!GetCount())
		return ;

	rectInvalid.top  -= rectInvalid.top % m_cxCellHeight ;
	int nLeftCoord = rectInvalid.left - rectInvalid.left ;

	int nStartRow = m_nTopCellRow + rectInvalid.top  / m_cxCellHeight ;
	int nStartCol = m_nTopCellCol + nLeftCoord / m_cxCellWidth ;

	while (rectInvalid.top < rectInvalid.bottom)
	{
		int nCol = nStartCol ;
		rectInvalid.left = nLeftCoord ;

		while (rectInvalid.left < rectInvalid.right)
		{
			if (nCol < m_nNumCols)
			{
				// Erase the cell background
				/////////////////////////////
/*				CRect rect ;

				GetCellRect( 
					nStartRow - m_nTopCellRow,
					nCol - m_nTopCellCol, rect ) ;

				COLORREF crColor = GetSysColor( COLOR_WINDOW ) ;

				if (m_uiFlags & k3dStyle)
				{
					crColor = GetSysColor( COLOR_BTNFACE ) ;
				}

				dc.FillSolidRect( rect.left, rect.top,
					rect.Width() + 1, rect.Height() + 1, crColor ) ;
*/
				// Draw the item
				/////////////////

				int nIndex = CellToIndex( nStartRow, nCol ) ;

				if (LB_ERR != nIndex)
				{
					SendDrawItem( dc, nIndex, ODA_DRAWENTIRE ) ;
				}
			}

			rectInvalid.left += m_cxCellWidth ;
			nCol++ ;

//			dc.MoveTo( rectInvalid.left - 1, rectInvalid.top ) ;
//			dc.LineTo( rectInvalid.left - 1, rectInvalid.bottom + 1 ) ;
		}

//		dc.MoveTo( 0, rectInvalid.top + m_cxCellHeight ) ;
//		dc.LineTo( rectInvalid.right + 1, rectInvalid.top + m_cxCellHeight ) ;

		rectInvalid.top += m_cxCellHeight ;
		nStartRow++ ;
	}

//	dc.SelectClipRgn( NULL ) ;
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnSetFocus(CWnd* pOldWnd)
//
// Description:   Message handler for WM_SETFOCUS
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnSetFocus( CWnd* ) 
{
	if (::IsWindow( GetSafeHwnd() ))
	{
		CWnd* pParent = GetParent() ;

		if (pParent)
		{
			// Notify the parent that the grid control
			// is gaining the keyboard input focus.
			GetParent()->PostMessage( WM_COMMAND, 
				MAKEWPARAM( GetDlgCtrlID(), LBN_SETFOCUS ),
				(LPARAM) m_hWnd ) ;
		}

		CClientDC dc(this) ;
		SendDrawItem( dc, max( GetCurSel(), 0 ), ODA_FOCUS ) ;
	}
	else
	{
		TpsAssertAlways( "Invalid Window Handle!" ) ;
	}
}

//*****************************************************************************
//
// Function Name: RGridCtrl::PreSubclassWindow()
//
// Description:   Virtual function override to handle an initialization
//                that needs to take place before the window is subclassed.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::PreSubclassWindow() 
{
	SetNumCols( 4 ) ;

	CWnd::PreSubclassWindow();
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnSize(UINT nType, int cx, int cy) 
//
// Description:   Message handler for WM_SIZE
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnSize(UINT /*nType*/, int cx, int cy) 
{
	if (m_fResizeCells) 
	{
		SizeCellsToClient( m_cxVisibleRows , m_nNumCols ) ;
	}
	else
	{
		m_cxVisibleRows = WORD( cy / m_cxCellHeight ) ;
		m_cxVisibleCols = WORD( cx / m_cxCellWidth ) ;
		UpdateMetrics() ;
	}
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnEraseBkgnd(CDC* pDC) 
//
// Description:   Message handler for WM_ERASEBKGND
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
BOOL RGridCtrlBase::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect ;
	GetClientRect( rect ) ;
	pDC->FillSolidRect( rect.left, rect.top,	rect.Width(), 
		rect.Height(), GetGridBackgroundColor() ) ;

	return TRUE ;
}

// ****************************************************************************
//
//  Function Name:	RGridCtrlBase::GetGridBackgroundColor()
//
//  Description:	Accessor
//
//  Returns:		The background color of this grid
//
//  Exceptions:		None
//
// ****************************************************************************
//
COLORREF RGridCtrlBase::GetGridBackgroundColor() const
{
	COLORREF crColor = GetSysColor( COLOR_WINDOW ) ;

	if (m_uiFlags & k3dStyle)
	{
		crColor = GetSysColor( COLOR_BTNFACE ) ;
	}

	return crColor;
}
//*****************************************************************************
//
// Function Name: RGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
//
// Description:   Message handler for WM_VSCROLL
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* ) 
{
	int nScrollMin, nScrollMax ;
//	int nOldPos = GetScrollPos( SB_VERT ) ; 
	int nNewPos = GetScrollPos( SB_VERT ) ;

	switch (nSBCode)
	{
	case SB_BOTTOM:
		nNewPos = GetCount() ;
		break ;

	case SB_LINEDOWN:
		nNewPos += 1 ;
		break ;
	
	case SB_LINEUP:
		nNewPos -= 1 ;
		break ;
	
	case SB_PAGEDOWN:
		nNewPos += m_cxVisibleRows ;
		break ;
	
	case SB_PAGEUP:
		nNewPos -= m_cxVisibleRows ;
		break ;
	
	case SB_THUMBPOSITION:
		nNewPos = nPos ;
		break ;
	
	case SB_THUMBTRACK:
		nNewPos = nPos ;
		break ;

	case SB_TOP:
		nNewPos = 0 ;
		break ;

	default:
		return ;
	}

	GetScrollRange( SB_VERT, &nScrollMin, &nScrollMax ) ;
	nNewPos = min( nScrollMax, max( nScrollMin, nNewPos ) ) ;

//	if (nNewPos != nOldPos )	
	SetTopIndex( nNewPos * m_nNumCols ) ;
}

//*****************************************************************************
//
// Function Name: RGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
//
// Description:   Message handler for WM_HSCROLL
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* ) 
{
	int nScrollMin, nScrollMax ;
	int nOldPos = GetScrollPos( SB_HORZ ) ; 
	int nNewPos = nOldPos ;

	switch (nSBCode)
	{
	case SB_BOTTOM:
		nNewPos = m_nNumCols - 1 ;
		break ;

	case SB_LINEDOWN:
		nNewPos += 1 ;
		break ;
	
	case SB_LINEUP:
		nNewPos -= 1 ;
		break ;
	
	case SB_PAGEDOWN:
		nNewPos += m_cxVisibleCols ;
		break ;
	
	case SB_PAGEUP:
		nNewPos -= m_cxVisibleCols ;
		break ;
	
	case SB_THUMBPOSITION:
		nNewPos = nPos ;
		break ;
	
	case SB_THUMBTRACK:
		nNewPos = nPos ;
		break ;

	case SB_TOP:
		nNewPos = 0 ;
		break ;

	default:
		return ;
	}

	GetScrollRange( SB_HORZ, &nScrollMin, &nScrollMax ) ;
	nNewPos = min( nScrollMax, max( nScrollMin, nNewPos ) ) ;

	if (nNewPos != nOldPos )
	{
		SetTopIndex( m_nTopCellRow * m_nNumCols + nNewPos ) ;
	}
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::GetGridStyle
//
// Description:   Determines the style flags of the grid control
//
// Returns:		   UINT containing grid control's style flags
//
// Exceptions:	   None
//
//*****************************************************************************
UINT RGridCtrlBase::GetGridStyle() const
{
	return m_uiFlags ;
}

//*****************************************************************************
//
// Function Name: RGridCtrlBase::SetStyle() 
//
// Description:   Sets the style flags for the grid control
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::SetGridStyle( UINT uiStyles ) 
{
	UINT uiFlags = m_uiFlags ;
	m_uiFlags = uiStyles ;

	if (GetSafeHwnd())
	{
		// if adding spacing or removing spacing, we
		// need to allow the control to resize itself
		if (uiStyles & kAddSpacing && !(uiFlags  & kAddSpacing) ||
			 uiFlags  & kAddSpacing && !(uiStyles & kAddSpacing))
		{
			UpdateMetrics() ;
		}

		// Update the display
		Invalidate() ;
	}
}


//*****************************************************************************
//
// Function Name: RGridCtrlBase::SetRedraw() 
//
// Description:   Sets the style flags for the grid control
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RGridCtrlBase::SetRedraw( BOOL bRedraw ) 
{
	m_bSetRedraw = bRedraw;
	CWnd::SetRedraw(bRedraw);
}

LRESULT RGridCtrlBase::OnLbGetSel( UINT, LONG )
{
	return GetCurSel();
}

LRESULT RGridCtrlBase::OnLbGetCount( UINT, LONG )
{
	return GetCount();
}

LRESULT RGridCtrlBase::OnLbSetCurSel( UINT wParam, LONG )
{
	return SetCurSel(wParam);
}


