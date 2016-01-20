/*
// $Workfile: TblObj.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TblObj.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 31    12/23/98 1:51p Psasse
// Code clean up
// 
// 30    7/28/98 9:41a Jayn
// 
// 29    7/25/98 1:51p Jayn
// Better (no exceptions!) style computation.
// 
// 28    7/23/98 8:44a Jayn
// Uses a better method of setting the text style that works for empty
// cells.
// 
// 27    7/19/98 6:27p Jayn
// Merge cells. Applying font attributes to cells.
// 
// 26    7/18/98 6:24p Jayn
// Table fixes.
// 
// 25    7/12/98 5:36p Jayn
// Various fixes.
// 
// 24    7/04/98 2:04p Jayn
// New table undo commands.
// 
// 23    6/29/98 12:30p Jayn
// Added assign() and original_dims().
// 
// 22    6/17/98 11:09a Jayn
// 
// 21    6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 20    6/03/98 1:46p Jayn
// Table Autoformat changes
// 
// 19    6/02/98 3:00p Jayn
// 
// 18    5/31/98 2:35p Jayn
// Delete row/columns
// 
// 17    5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 16    5/29/98 6:39p Jayn
// Insert column, etc.
// 
// 15    5/28/98 4:11p Jayn
// More cool table stuff (cell formatting, etc.)
// 
// 14    5/26/98 2:39p Jayn
// New table code.
// 
// 13    4/30/98 4:48p Jayn
// Resizing of table rows and columns.
// 
// 12    4/28/98 3:39p Jayn
// New table cursors.
// 
// 11    4/22/98 6:27p Jayn
// Table development (primarily selection).
// 
// 10    4/14/98 4:41p Jayn
// Changes for the new table object.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "tblobj.h"
#include "celobj.h"
#include "tblprpd.h"
#include "util.h"
#include "utils.h"
#include "action.h"
#include "framerec.h"
#include "textrec.h"
#include "frameobj.h"
#include "ctxp.h"
#include "textflow.h"
#include "clip.h"

//
// See tblobj.h for implementation notes.
//

/////////////////////////////////////////////////////////////////////////////
// CTableBorderSegment

CBitmap CTableBorderSegment::m_cbmUndefined;

void CTableBorderSegment::SetDefault(void)
{
	m_bType = BORDER_SEGMENT_Solid;
	m_bWidth = 0;
	m_Color = TRANSPARENT_COLOR;
}

BOOL CTableBorderSegment::SetupToDraw(RedisplayContext* rc, CPen& cpEdgePen, CBrush& cbEdgeBrush, int& nWidth)
{
	// Spanned cells do not draw.
	if (IsSpanned())
	{
		return FALSE;
	}

	BYTE bType = m_bType;
	BYTE bWidth = m_bWidth;
	COLOR Color = m_Color;

	// Handle the undefined (multiply-defined) case.
	if (bType == BORDER_SEGMENT_Undefined
		 || Color == UNDEFINED_COLOR
		 || bWidth == (BYTE)-1)
	{
		nWidth = 4;
		if (m_cbmUndefined.GetSafeHandle() == NULL)
		{
			m_cbmUndefined.LoadBitmap(IDB_GRAYHATCH);
		}
		return cbEdgeBrush.CreatePatternBrush(&m_cbmUndefined);
	}

	if (bType != BORDER_SEGMENT_None)
	{
		if (Color == TRANSPARENT_COLOR)
		{
			bType = BORDER_SEGMENT_None;
		}
	}

	switch (bType)
	{
		case BORDER_SEGMENT_None:
		{
			if (rc->IsScreen()
				 && !rc->is_metafile
				 && !rc->is_print_preview)
			{
				return cpEdgePen.CreatePen(PS_DOT, 1, RGB(192, 192, 192));
			}
			break;
		}
		case BORDER_SEGMENT_Solid:
		{
			COLORREF c = COLORREF_FROM_COLOR(Color);
			// Compute the edge width (from points to pixels).
			nWidth = rc->page_x_to_screen(scale_number(bWidth, PAGE_RESOLUTION, 72))
						- rc->page_x_to_screen(0);
			if (nWidth <= 1)
			{
				// Single line. Use a pen.
				return cpEdgePen.CreatePen(PS_SOLID, nWidth, c);
			}
			else
			{
				// Fat line. No pen, but width is returned.
				return cbEdgeBrush.CreateSolidBrush(c);
			}
		}
		case BORDER_SEGMENT_Dashed:
		{
			return cpEdgePen.CreatePen(PS_DASH, 1, COLORREF_FROM_COLOR(Color));
		}
		default:
		{
			// Invalid border segment type.
			ASSERT(FALSE);
			break;
		}
	}
	return FALSE;
}

int CTableBorderSegment::GetWidth(RedisplayContext* rc)
{
	if (IsSpanned())
	{
		// Spanned edges are non-existent.
		return 0;
	}
	else if (m_bType == BORDER_SEGMENT_Solid
			&& m_Color != TRANSPARENT_COLOR
			&& m_bWidth != (BYTE)-1)
	{
		return rc->page_x_to_screen(scale_number(m_bWidth, PAGE_RESOLUTION, 72))
					- rc->page_x_to_screen(0);
	}
	else
	{
		// Special/undefined lines are a single pixel.
		return 1;
	}
}

void CTableBorderSegment::CombineWith(const CTableBorderSegment& s)
{
	if (!s.IsSpanned())
	{
		if (m_bType != s.m_bType)
		{
			m_bType = (BYTE)BORDER_SEGMENT_Undefined;
		}
		if (m_bWidth != s.m_bWidth)
		{
			m_bWidth = (BYTE)-1;
		}
		if (m_Color != s.m_Color)
		{
			m_Color = UNDEFINED_COLOR;
		}
	}
}

void CTableBorderSegment::ReplaceWith(const CTableBorderSegment& s)
{
	if (!IsSpanned())
	{
		if (s.m_bType != (BYTE)BORDER_SEGMENT_Undefined)
		{
			m_bType = s.m_bType;
		}
		if (s.m_bWidth != (BYTE)-1)
		{
			m_bWidth = s.m_bWidth;
		}
		if (s.m_Color != UNDEFINED_COLOR)
		{
			m_Color = s.m_Color;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTableCellInfo

BOOL CTableCellInfo::SetupToDraw(CBrush& cbBrush)
{
	if (m_Color == TRANSPARENT_COLOR)
	{
		return FALSE;
	}
	else
	{
		return cbBrush.CreateSolidBrush(COLORREF_FROM_COLOR(m_Color));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTableNewRCInfo

CTableNewRCInfo::CTableNewRCInfo()
{
	m_pSegments = NULL;
	m_pFillColors = NULL;
}

CTableNewRCInfo::~CTableNewRCInfo()
{
	delete [] m_pSegments;
	delete [] m_pFillColors;
}

void CTableNewRCInfo::AllocateInfo(int nCells)
{
	// Allocate the segments.
	delete [] m_pSegments;
	m_pSegments = new CTableBorderSegment[nCells+1];

	// Allocate the colors.
	delete [] m_pFillColors;
	m_pFillColors = new COLOR[nCells];
}

/////////////////////////////////////////////////////////////////////////////
// CTableObject

int CTableObject::m_nMaxTableRows = 30;
int CTableObject::m_nMaxTableColumns = 30;

BOOL IsBoxOnBoxEdge(PBOX * bound, PBOX * box)
{
	return	(  (bound->x0 < box->x0
					 && box->x1 < bound->x1
					 && box->y0 < bound->y0
					 && bound->y0 < box->y1)
				|| (box->x0 < bound->x1
					 && bound->x1 < box->x1
					 && bound->y0 < box->y0
					 && box->y1 < bound->y1)
			   || (box->y0 < bound->y1
					 && bound->y1 < box->y1
					 && bound->x0 < box->x0
					 && box->x1 < bound->x1)
				||	(box->x0 < bound->x0
					 && bound->x0 < box->x1
					 && bound->y0 < box->y0
					 && box->y1 < bound->y1));
}

// Is a "box" over one of the row/column select areas?
// returns handle or 0
static OBJECT_HANDLE IsBoxOnRCSelectZone(const PBOX& bound, const PBOX& box)
{
	// zone is one box width above the table
	PPNT zone;
	zone.x = box.x1 - box.x0;
	zone.y = box.y1 - box.y0;

	PPNT p;
	p.x = (box.x0 + box.x1)/2;
	p.y = (box.y0 + box.y1)/2;

	PBOX ColumnZone;
	ColumnZone.x0 = bound.x0;
	ColumnZone.x1 = bound.x1;
	ColumnZone.y0 = bound.y0 -	zone.y;
	ColumnZone.y1 = bound.y0;

	PBOX RowZone;
	RowZone.y0 = bound.y0;
	RowZone.y1 = bound.y1;
	RowZone.x0 = bound.x0 - zone.x;
	RowZone.x1 = bound.x0;

	if (IsPtInBox(p.x, p.y, &ColumnZone))
		return OBJECT_HANDLE_SELECT_COL;
	else if (IsPtInBox(p.x, p.y, &RowZone))
		return OBJECT_HANDLE_SELECT_ROW;
	else
		return OBJECT_HANDLE_NONE;
}

// constructor
CTableObject::CTableObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner)
		:  GroupObject(type, owner)
{
//	record.primary_action = ACTION_SELECT_CELLS;
	record.primary_action = ACTION_DEFAULT_EDIT;
	memset(&m_TRecord, 0, sizeof(m_TRecord));
	// Initialize the select variables.
	m_crSelect = CRect(-1, -1, -1, -1);
	add_select_flags(SELECT_FLAG_subselect);		// We have a subselect.
//	m_TRecord.m_fFitToText = TRUE;
	m_TRecord.m_fFitToText = FALSE;
}

//
// The table object creation method.
// This is registered with the database.
//

ERRORCODE CTableObject::create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *record)
{
	CTableObject* pObject;
	// Create the table object. */

	if ((pObject = new CTableObject(type, owner)) == NULL)
	{
		return ERRORCODE_Memory;
	}

	*record = pObject;

	if (creation_data != NULL)
	{
		TABLE_CREATE_STRUCT* tcs = (TABLE_CREATE_STRUCT*)creation_data;
		pObject->set_unrotated_bound(tcs->bound);
		pObject->Rebuild(&tcs->info);
		pObject->calc();
	}

	return ERRORCODE_None;
}

void CTableObject::Rebuild(const TABLE_INFO* pInfo)
{
	if (pInfo != NULL)
	{
		SetInfo(*pInfo);
	}

	// do building here
	BuildTable();

	establish(FALSE);
}

/*
// Build the groups used by a table
*/

// create and add cell objects to table
//
ERRORCODE CTableObject::BuildTable(void)
{
	ERRORCODE error = ERRORCODE_None;

	// Compute the average cell dimensions.
	// This gives us the default size for the new cells.
	PBOX Bound = get_unrotated_bound();
	PPNT AvgCellDims;
	AvgCellDims.x = (Bound.x1 - Bound.x0) / GetColumns();
	AvgCellDims.y = (Bound.y1 - Bound.y0) / GetRows();

	int nRows = GetRows();
	int nColumns = GetColumns();

	// Set all the default widths and heights.
	double dRowHeight = 1.0/nRows;
	for (int nRow = 0; nRow < nRows; nRow++)
	{
		m_RowHeights.Add(dRowHeight);
	}
	double dColumnWidth = 1.0/nColumns;
	for (int nColumn = 0; nColumn < nColumns; nColumn++)
	{
		m_ColumnWidths.Add(dColumnWidth);
	}

	// The desired row heights are the same at this point.
	m_DesiredRowHeights = m_RowHeights;

	// Note the cell creation order. This is important.
	CPoint cpCell;
	PBOX b;
	b.y1 = Bound.y0;

	for (cpCell.y = 0; cpCell.y < nRows; cpCell.y++) // create row
	{
		b.y0 = b.y1;
		b.y1 += AvgCellDims.y;

		b.x1 = Bound.x0;

		for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++) // then create all the columns in row
		{
			// Update bounds.
			b.x0 = b.x1;
			b.x1 += AvgCellDims.x;

			// Create the cell.
			if ((error = AddCell(b,
										cpCell,
										CELL_PANEL,
										SELECT_FLAG_boundary | SELECT_FLAG_draw_boundary,
										object_list())) == ERRORCODE_None)
			{
			}
		}
	}
	establish();

	// Create all the cell info.
	CreateDefaultCellInfo();

	// Create the default border segments.
	CreateDefaultBorderSegments();

	// Compute table parameters (row starts, etc).
	CalculateTable();

	return error;
}	

void CTableObject::CreateDefaultCellInfo(void)
{
	// Create the default cell info element.
	CTableCellInfo Info;
	Info.m_nSpanX = 1;
	Info.m_nSpanY = 1;
	Info.m_Color = TRANSPARENT_COLOR;

	// We have nColumns*nRows cell info elements.
	int nCount = GetColumns()*GetRows();
	while (nCount-- > 0)
	{
		m_CellInfo.Add(&Info);
	}
}

void CTableObject::CreateDefaultBorderSegments(void)
{
	// Create the default segment.
	CTableBorderSegment Segment;
	Segment.SetDefault();

	// We have nColumns*(nRows+1) horizontal segments.
	int nCount = GetColumns()*(GetRows()+1);
	while (nCount-- > 0)
	{
		m_HBorders.Add(&Segment);
	}

	// We have nRows*(nColumns+1) vertical segments.
	nCount = GetRows()*(GetColumns()+1);
	while (nCount-- > 0)
	{
		m_VBorders.Add(&Segment);
	}
}

PCOORD CTableObject::GetRowHeight(int nRow)
{
	if (nRow >= 0 && nRow < m_RowHeights.count())
	{
		PBOX Bound = get_unrotated_bound();
		return (PCOORD)(m_RowHeights.GetAt(nRow)*(Bound.Height()));
	}
	ASSERT(FALSE);
	return 0;
}

PCOORD CTableObject::GetColumnWidth(int nColumn)
{
	if (nColumn >= 0 && nColumn < m_ColumnWidths.count())
	{
		PBOX Bound = get_unrotated_bound();
		return (PCOORD)(m_ColumnWidths.GetAt(nColumn)*(Bound.Width()));
	}
	ASSERT(FALSE);
	return 0;
}

void CTableObject::SetColumnWidth(int nColumn, PCOORD Width)
{
	if (nColumn >= 0 && nColumn < m_ColumnWidths.count())
	{
		PBOX Bound = get_unrotated_bound();
		m_ColumnWidths.SetAt(nColumn, (double)Width/(double)(Bound.x1-Bound.x0));
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CTableObject::SetRowHeight(int nRow, PCOORD Height)
{
	if (nRow >= 0 && nRow < m_RowHeights.count())
	{
		PBOX Bound = get_unrotated_bound();
		m_RowHeights.SetAt(nRow, (double)Height/(double)(Bound.y1-Bound.y0));
	}
	else
	{
		ASSERT(FALSE);
	}
}

//
// Return the cell containing this point.
//

CPoint CTableObject::CellOfPoint(PPNT p)
{
	NormalizePoint(p);
	return CPoint(ColumnOfX(p.x), RowOfY(p.y));
}

//
// Return the column containing this point.
// (Full point is needed for the rotated case).
//

int CTableObject::ColumnOfPoint(PPNT p)
{
	NormalizePoint(p);
	return ColumnOfX(p.x);
}

//
// Return the row containing this point.
// (Full point is needed for the rotated case).
//

int CTableObject::RowOfPoint(PPNT p)
{
	NormalizePoint(p);
	return RowOfY(p.y);
}

//
// Normalize a point (move it into the space of our unrotated bound).
//

void CTableObject::NormalizePoint(PPNT& p)
{
	// Change coordinate to our rotated space if necessary.
	if (get_rotation() != 0.0)
	{
		/*
		// We have a rotated bound.
		// Rotate the point into our rotated space. Then compare to our
		// unrotated bound.
		*/
		rotate_points(&p, 1, get_origin(), -get_rotation());
	}

	// Normalize the point relative to the bound.
	// We handle flipping here.
	PBOX Bound = get_unrotated_bound();

	FLAGS Flags = get_flags();

	if (Flags & OBJECT_FLAG_xflipped)
	{
		p.x = Bound.x1 - p.x;
	}
	else
	{
		p.x -= Bound.x0;
	}

	if (Flags & OBJECT_FLAG_yflipped)
	{
		p.y = Bound.y1 - p.y;
	}
	else
	{
		p.y -= Bound.y0;
	}
}

void CTableObject::NormalizeBox(PBOX& b)
{
	// Break down the box into dimensions and center.
	PPNT Dims;
	Dims.x = b.x1 - b.x0;
	Dims.y = b.y1 - b.y0;

	PPNT p;
	p.x = (b.x0 + b.x1)/2;
	p.y = (b.y0 + b.y1)/2;

	// Normalize the center.
	NormalizePoint(p);

	// Reconstruct the box around the new center.
	b.x0 = p.x - Dims.x/2;
	b.x1 = b.x0 + Dims.x;
	b.y0 = p.y - Dims.y/2;
	b.y1 = b.y0 + Dims.y;
}

//
// Return the column for a normalized x coordinate.
//

int CTableObject::ColumnOfX(PCOORD x)
{
	if (x < 0)
	{
		return 0;
	}

	for (int nColumn = 0; nColumn < GetColumns(); nColumn++)
	{
		PCOORD Width = GetColumnWidth(nColumn);
		if (x >= 0 && x < Width)
		{
			return nColumn;
		}
		x -= Width;
	}
	return GetColumns()-1;
}

//
// Return the row for a normalized y coordinate.
//

int CTableObject::RowOfY(PCOORD y)
{
	if (y < 0)
	{
		return 0;
	}

	for (int nRow = 0; nRow < GetRows(); nRow++)
	{
		PCOORD Height = GetRowHeight(nRow);
		if (y >= 0 && y < Height)
		{
			return nRow;
		}
		y -= Height;
	}
	return GetRows()-1;
}

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

ERRORCODE CTableObject::read_data(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = INHERITED::read_data(pDevice)) == ERRORCODE_None
		 && (error = pDevice->read_record(&m_TRecord, sizeof(m_TRecord))) == ERRORCODE_None
		 && (error = m_DesiredRowHeights.read(pDevice)) == ERRORCODE_None
		 && (error = m_RowHeights.read(pDevice)) == ERRORCODE_None
		 && (error = m_ColumnWidths.read(pDevice)) == ERRORCODE_None
		 && (error = m_CellInfo.read(pDevice)) == ERRORCODE_None
		 && (error = m_HBorders.read(pDevice)) == ERRORCODE_None
		 && (error = m_VBorders.read(pDevice)) == ERRORCODE_None)
	{
		CalculateTable();
	}

	return error;
}

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

ERRORCODE CTableObject::write_data(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = INHERITED::write_data(pDevice)) == ERRORCODE_None
		 && (error = pDevice->write_record(&m_TRecord, sizeof(m_TRecord))) == ERRORCODE_None
		 && (error = m_DesiredRowHeights.write(pDevice)) == ERRORCODE_None
		 && (error = m_RowHeights.write(pDevice)) == ERRORCODE_None
		 && (error = m_ColumnWidths.write(pDevice)) == ERRORCODE_None
		 && (error = m_CellInfo.write(pDevice)) == ERRORCODE_None
		 && (error = m_HBorders.write(pDevice)) == ERRORCODE_None
		 && (error = m_VBorders.write(pDevice)) == ERRORCODE_None)
	{
	}
	return error;
}

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

ST_MAN_SIZE CTableObject::size_data(StorageDevicePtr pDevice)
{
	return INHERITED::size_data(pDevice)
			+ pDevice->size_record(sizeof(m_TRecord))
			+ m_DesiredRowHeights.size(pDevice)
			+ m_RowHeights.size(pDevice)
			+ m_ColumnWidths.size(pDevice)
			+ m_CellInfo.size(pDevice)
			+ m_HBorders.size(pDevice)
			+ m_VBorders.size(pDevice);
}

// Update an object into a redisplay context.
UpdateStatePtr CTableObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
{
	if (ustate == NULL)
	{
		// First time draw.
		DrawGrid(rc);
	}
	return INHERITED::update(rc, extent, clip, ustate, refresh_type);
}

/*
// Get the properties for this object.
*/

void CTableObject::GetObjectProperties(CObjectProperties& Properties)
{
#if 0
	if (SomethingSelected())
	{
		int nColumns = GetColumns();
		// Grab the properties for the selected cells.
		CPoint cpCell;
		for (cpCell.y = m_crSelect.top; cpCell.y <= m_crSelect.bottom; cpCell.y++)
		{
			cpCell.x = m_crSelect.left;
			CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
			for (; cpCell.x <= m_crSelect.right; cpCell.x++, pCellInfo++)
			{
				if (!pCellInfo->IsSpanned())
				{
					if (pCellInfo->m_Color == TRANSPARENT_COLOR)
					{
						Properties.SetFillType(FillFormatV2::FillNone);
					}
					else
					{
						Properties.SetFillForegroundColor(pCellInfo->m_Color);
						Properties.SetFillType(FillFormatV2::FillSolid);
					}
				}
			}
		}
	}
#endif
}

/*
// Set the properties for this object.
*/

BOOL CTableObject::SetObjectProperties(CObjectProperties& Properties)
{
#if 0
	if (SomethingSelected())
	{
		SHORT nType;
		if (Properties.ApplyFillType(nType))
		{
			COLOR Color;
			if (nType == FillFormatV2::FillNone)
			{
				Color = TRANSPARENT_COLOR;
			}
			else
			{
				Properties.ApplyFillForegroundColor(Color);
			}

			int nColumns = GetColumns();
			// Grab the properties for the selected cells.
			for (int nRow = m_crSelect.top; nRow <= m_crSelect.bottom; nRow++)
			{
				CTableCellInfo* pCellInfo = GetCellInfo(CPoint(m_crSelect.left, nRow));
				for (int nColumn = m_crSelect.left; nColumn <= m_crSelect.right; nColumn++, pCellInfo++)
				{
					if (!pCellInfo->IsSpanned())
					{
						pCellInfo->m_Color = Color;
					}
				}
			}

			// Make the selection redraw.
			InvalidateSelection();
		}
	}
	return TRUE;
#endif
	return FALSE;
}

/*
// Look to see if a point is within the object of a calendar group.
*/

BOOL CTableObject::pt_in_group(GroupObjectPtr group, PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object)
{
	if (group == NULL)
	{
		return FALSE;
	}

	ObjectListPtr list;
	PMGPageObjectPtr object;

	// Look in the group for a match.
	list = group->object_list();

	for (object = (PMGPageObjectPtr)list->first_object(); object != NULL; object = (PMGPageObjectPtr)object->next_object())
	{
		if (!(object->get_flags() & OBJECT_FLAG_locked))
		{
			if (object->pt_in_object(p, rc, NULL))
			{
				*sub_object = object;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CTableObject::pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object)
{
	return INHERITED::pt_in_object(p, rc, sub_object);
}

/*
// Return whether a point is over an object's move area or not.
// By default, the content area of an object *is* the move area, so the
// default return value is FALSE here.
*/

BOOL CTableObject::pt_on_move_area(PPNT p, RedisplayContextPtr rc)
{
	BOOL fInBound = FALSE;

	POINT sp;
	rc->ppnt_to_screen(p, &sp);

	PBOX Box;
	rc->mouse_box(sp, &Box);

	// Not in our regular bound.
	// See if we are on the edge.

	if (record.rotc.rotation != 0.0)
	{
   /*
   // We have a rotated bound.
   // Rotate the box into our rotated space. Then compare to our
   // unrotated bound.
   */
		compute_rotated_bound(Box, -record.rotc.rotation, &Box, record.rotc.origin);
	}

	return IsBoxOnBox(&Box, &rrecord.unrotated_bound);
}

PMG_OBJECT_ACTION CTableObject::move_object(DRAG_STATE state, PPNT vector, PPNT current_xy, LPVOID far *data /*=NULL*/, BOOL fPanning /*=FALSE*/)
{
	if (state == DRAG_STATE_begin)
	{
		DeselectAll();
	}
	return INHERITED::move_object(state, vector, current_xy, data, fPanning);
}

OBJECT_HANDLE CTableObject::box_on_handle(PBOX_PTR pBox, RedisplayContextPtr rc)
{
	OBJECT_HANDLE handle = RectPageObject::box_on_handle(pBox, rc);

	PBOX box = *pBox;
	if (handle == OBJECT_HANDLE_NONE)
	{
		// Normalize the box.
		NormalizeBox(box);

		// Get the unrotated bound.
		PBOX bound;
		bound = get_unrotated_bound();

		// Normalize the unrotated bound.
		bound.x1 -= bound.x0;
		bound.y1 -= bound.y0;
		bound.x0 = 0;
		bound.y0 = 0;

		if (box.y1 > bound.y0 && box.y0 < bound.y1)
		{
			int nColumn = GetColumnResizer(box, FALSE);
			if (nColumn != -1)
				return (OBJECT_HANDLE)(OBJECT_HANDLE_RESIZE_COL | (nColumn << 8));
		}

		if (box.x1 > bound.x0 && box.x0 < bound.x1)
		{
			int nRow = GetRowResizer(box, FALSE);
			if (nRow != -1)
				return (OBJECT_HANDLE)(OBJECT_HANDLE_RESIZE_ROW | (nRow << 8));
		}

		if ((handle = IsBoxOnRCSelectZone(bound, box)) != OBJECT_HANDLE_NONE)
			return handle;

		// See if we are on the edge of the bound (not select).
		if (IsBoxOnBoxEdge(&bound, &box))
			return OBJECT_HANDLE_MOVE;
	}

	return handle;
}

//
// Move a handle.
//

PMG_OBJECT_ACTION CTableObject::move_handle(DRAG_STATE state, PPNT pos, OBJECT_HANDLE handle, FLAGS shift_status, LPVOID far *data /*=NULL*/, BOOL fCropping /*=FALSE*/)
{
	PMG_OBJECT_ACTION Action = ACTION_DO_NOTHING;
	switch (handle & 0x00ff)
	{
		case OBJECT_HANDLE_RESIZE_COL:
		{
			int nColumn = (handle >> 8);
			ResizeColumn(nColumn, state, pos, shift_status);
			break;
		}
		case OBJECT_HANDLE_RESIZE_ROW:
		{
			int nRow = (handle >> 8);
			ResizeRow(nRow, state, pos, shift_status);
			break;
		}
		default:
		{
			Action = INHERITED::move_handle(state, pos, handle, shift_status, data, fCropping);
			break;
		}
	}

	if (state == DRAG_STATE_end)		// But not abort...
	{
		if (GetFitToText())
		{
			LayoutCells();				// Must do this first.
//			m_TRecord.m_dExtraHeight = 0;
			FitCellsToText();
		}
	}
	return Action;
}

//
// Notification of beginning of handle movement.
//

void CTableObject::OnBeginHandleMove(PPNT pos, OBJECT_HANDLE handle, BOOL fCropping)
{
	// We do not want a selection anymore.
	DeselectAll();

	INHERITED::OnBeginHandleMove(pos, handle, fCropping);
}

//
// Return the column resizer contained by the passed box (x coordinates).
//

int CTableObject::GetColumnResizer(PBOX box, BOOL fNormalize /*=TRUE*/)
{
	if (fNormalize)
	{
		NormalizeBox(box);
	}
	// Check if we are over a column resizer.
	PCOORD Column = 0;
	for (int nColumn = 0; nColumn < GetColumns()-1; nColumn++)
	{
		Column += GetColumnWidth(nColumn);
		if (Column >= box.x0 && Column <= box.x1)
			return nColumn;
	}
	return -1;
}

//
// Return the row resizer contained by the passed box (y coordinates).
//

int CTableObject::GetRowResizer(PBOX box, BOOL fNormalize /*=TRUE*/)
{
	if (fNormalize)
	{
		NormalizeBox(box);
	}
	// Check if we are over a row resizer.
	PCOORD Row = 0;
	for (int nRow = 0; nRow < GetRows()-1; nRow++)
	{
		Row += GetRowHeight(nRow);
		if (Row >= box.y0 && Row <= box.y1)
			return nRow;
	}
	return -1;
}

void CTableObject::DumpCells(void)
{
	for (int nRow = 0; nRow < GetRows(); nRow++)
	{
		CCellObject* pCell = (CCellObject*)m_cpaRowStarts[nRow];
		for (int nColumn = 0; nColumn < GetColumns(); nColumn++)
		{
			TRACE("(%d, %d) ", pCell->GetColumn(), pCell->GetRow());
			pCell = (CCellObject*)pCell->next_object();
		}
		TRACE("\n");
	}
}

//
// Add a row to a table.
// The new row number should be between 0 and GetRows() (inclusive).
// e.g. 0 adds the row at the top, GetRows() adds the row at the bottom,
// something in between creates a row of that index.
//

void CTableObject::AddRow(int nNewRow, CTableNewRCInfo* pNewRCInfo)
{
	// Get the current number of rows.
	int nRows = GetRows();
	int nColumns = GetColumns();

	// Validate the parameter passed.
	if (nNewRow < 0 || nNewRow > nRows)
	{
		ASSERT(FALSE);
		return;
	}

	// Bump the number of rows
	SetRows(++nRows);

	// Add a new row height.
	m_RowHeights.InsertAt(nNewRow, 0);
	m_DesiredRowHeights.InsertAt(nNewRow, 0);
	ASSERT(m_RowHeights.count() == nRows);
	ASSERT(m_DesiredRowHeights.count() == nRows);

	// Add horizontal border segments for the new row.
	// We duplicate the existing horizontal segments to preserve the surrounding
	// cell formatting.
	int nSegment = nNewRow*nColumns;
	int nDestSegment = nSegment;
	for (int n = 0; n < nColumns; n++)
	{
		CTableBorderSegment Segment = *GetHBorder(nSegment);
		Segment.m_bType &= ~BORDER_SEGMENT_Spanned;
		m_HBorders.InsertAt(nDestSegment, &Segment);
		nDestSegment++;
		nSegment += 2;
	}

	// Flesh out the rest of the formatting. This is passed in.
	// Add vertical border segments for the new row.
	nDestSegment = nNewRow /*+0*nRows*/;
	for (n = 0; n <= nColumns; n++)
	{
		m_VBorders.InsertAt(nDestSegment, &pNewRCInfo->m_pSegments[n]);
		nDestSegment += nRows;
	}

	// Add cells for the new row.
	int nDestCell = nNewRow*nColumns;
	CTableCellInfo CellInfo;
	CellInfo.m_nSpanX = 1;
	CellInfo.m_nSpanY = 1;
	for (n = 0; n < nColumns; n++)
	{
		CellInfo.m_Color = pNewRCInfo->m_pFillColors[n];
		m_CellInfo.InsertAt(nDestCell, &CellInfo);
		nDestCell++;
	}

	// Get an array of the current row sizes.
	// This includes the one we just set.
	PCOORD* pRows = new PCOORD[nRows];
	int nRow;
	for (nRow = 0; nRow < nRows; nRow++)
	{
		pRows[nRow] = GetRowHeight(nRow);
	}

	// Set the new row size.
	ASSERT(pRows[nNewRow] == 0);
	pRows[nNewRow] = pNewRCInfo->m_Size;

	// Compute the new bound size.
	PBOX Bound = get_unrotated_bound();
	Bound.y1 += pNewRCInfo->m_Size;

	// Set the new bound.
	PPNT Anchor;
	get_anchor_point(OBJECT_HANDLE_LOWER, &Anchor);

	set_unrotated_bound(Bound);

	// Set the new row sizes. We need to do this since the bound
	// changed.
	for (nRow = 0; nRow < GetRows(); nRow++)
	{
		SetRowHeight(nRow, pRows[nRow]);
	}
	compute_handles();
	move_to_anchor(OBJECT_HANDLE_LOWER, Anchor);

	record.flags |= OBJECT_FLAG_needs_calc;

	// Compute bound and vertices.
	compute_handles();

	delete [] pRows;

	// Update the desired row heights from the current ones.
	UpdateDesiredRowHeights();

	// Add a new row of cells.
	ObjectList* pList = object_list();
	CPoint cpCell;
	PMGPageObject* pInsert;
	if (nNewRow == nRows-1)		// Last row
	{
		// Append.
		pInsert = NULL;
	}
	else
	{
		pInsert = (PMGPageObject*)m_cpaRowStarts[nNewRow];
	}

	// Insert all the new cells.
	cpCell.y = nNewRow;
	for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++)
	{
		// Create the new cell object.
		CCellObject* pNewCell = (CCellObject*)get_database()->new_object(OBJECT_TYPE_Cell, NULL);
		if (pNewCell == NULL)
		{
			// Holy cow! Things in this table are partially changed.
			// It will be left in a bad state.
			ASSERT(FALSE);
			return;
		}
		pNewCell->set_panel(CELL_PANEL);
		pNewCell->set_select_flags(SELECT_FLAG_boundary | SELECT_FLAG_draw_boundary);
		pNewCell->SetCell(cpCell);
		pList->insert_before(pNewCell, pInsert);		// Add it to the list.

		// If this is the first object, this is the row start.
		if (cpCell.x == 0)
		{
			// Add this as the new row start.
			m_cpaRowStarts.InsertAt(nNewRow, pNewCell);
		}
	}

	ASSERT(m_cpaRowStarts.GetSize() == nRows);

	// Now, run through the rest of the rows and bump the y values.
	for (cpCell.y = nNewRow+1; cpCell.y < nRows; cpCell.y++)
	{
		// Seek across to find where to insert the cell.
		PMGPageObject* pObject = (PMGPageObject*)m_cpaRowStarts[cpCell.y];
		for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++)
		{
			CCellObject* pCell = (CCellObject*)pObject;
			ASSERT(pCell != NULL);
			ASSERT(pCell->GetRow() == cpCell.y-1);
			ASSERT(pCell->GetColumn() == cpCell.x);
			pCell->SetRow((short)cpCell.y);

			pObject = (PMGPageObject*)pObject->next_object();
		}
	}
	
	// Now go through and set the sizes for all the cells.
	calc();
}

//
// Add a column to a table.
// The new column number should be between 0 and GetColumns() (inclusive).
// e.g. 0 adds the column at the left, GetColumns() adds the column at the right,
// something in between creates a column of that index.
// The formatting information passed in must be consistent with the number of
// rows. You need (nRows+1) HSegments, and nRows fill colors. The VSegments
// are a duplication of the left segments (to preserve the surrounding columns).
//

void CTableObject::AddColumn(int nNewColumn, CTableNewRCInfo* pNewRCInfo)
{
	// Get the current number of columns.
	int nColumns = GetColumns();
	int nRows = GetRows();

	// Validate the parameter passed.
	if (nNewColumn < 0 || nNewColumn > nColumns)
	{
		ASSERT(FALSE);
		return;
	}

	// Bump the number of columns
	SetColumns(++nColumns);

	// Add a new column width.
	m_ColumnWidths.InsertAt(nNewColumn, 0);
	ASSERT(m_ColumnWidths.count() == nColumns);

	// Add vertical border segments for the new column.
	// We duplicate the existing vertical segments to preserve the surrounding
	// cell formatting.
	int nSegment = nNewColumn*nRows;
	int nDestSegment = nSegment;
	for (int n = 0; n < nRows; n++)
	{
		CTableBorderSegment Segment = *GetVBorder(nSegment);
		Segment.m_bType &= ~BORDER_SEGMENT_Spanned;
		m_VBorders.InsertAt(nDestSegment, &Segment);
		nDestSegment++;
		nSegment += 2;
	}

	// Flesh out the rest of the formatting. This is passed in.
	// Add horizontal border segments for the new column.
	nDestSegment = nNewColumn /*+0*nColumns*/;
	for (n = 0; n <= nRows; n++)
	{
		m_HBorders.InsertAt(nDestSegment, &pNewRCInfo->m_pSegments[n]);
		nDestSegment += nColumns;
	}

	// Add cells for the new column.
	int nDestCell = nNewColumn /*0*nColumns*/;
	CTableCellInfo CellInfo;
	CellInfo.m_nSpanX = 1;
	CellInfo.m_nSpanY = 1;
	for (n = 0; n < nRows; n++)
	{
		CellInfo.m_Color = pNewRCInfo->m_pFillColors[n];
		m_CellInfo.InsertAt(nDestCell, &CellInfo);
		nDestCell += nColumns;
	}

	// Get an array of the current column sizes.
	// This includes the one we just set.
	PCOORD* pColumns = new PCOORD[nColumns];
	int nColumn;
	for (nColumn = 0; nColumn < nColumns; nColumn++)
	{
		pColumns[nColumn] = GetColumnWidth(nColumn);
	}

	// Set the new column size.
	ASSERT(pColumns[nNewColumn] == 0);
	pColumns[nNewColumn] = pNewRCInfo->m_Size;

	// Compute the new bound size.
	PBOX Bound = get_unrotated_bound();
	Bound.x1 += pNewRCInfo->m_Size;

	// Set the new bound.
	PPNT Anchor;
	get_anchor_point(OBJECT_HANDLE_RIGHT, &Anchor);

	set_unrotated_bound(Bound);

	// Set the new column sizes. We need to do this since the bound
	// changed.
	for (nColumn = 0; nColumn < GetColumns(); nColumn++)
	{
		SetColumnWidth(nColumn, pColumns[nColumn]);
	}
	compute_handles();
	move_to_anchor(OBJECT_HANDLE_RIGHT, Anchor);

	record.flags |= OBJECT_FLAG_needs_calc;

	// Compute bound and vertices.
	compute_handles();

	delete [] pColumns;

	// Add new cells for each row.
	ObjectList* pList = object_list();
	CPoint cpCell;
	for (cpCell.y = 0; cpCell.y < nRows; cpCell.y++)
	{
		// Seek across to find where to insert the cell.
		PMGPageObject* pObject = (PMGPageObject*)m_cpaRowStarts[cpCell.y];
		for (cpCell.x = 0; cpCell.x < nNewColumn; cpCell.x++)
		{
			pObject = (PMGPageObject*)pObject->next_object();
		}
		// Now, insert the new cell. We insert the cell before this object.
		// Note: pObject may be NULL at this time if we are inserting a cell
		// in the last column of the last row. This is OK - the list code handles
		// this special case.

		// Create the new cell object.
		CCellObject* pNewCell = (CCellObject*)get_database()->new_object(OBJECT_TYPE_Cell, NULL);
		if (pNewCell == NULL)
		{
			// Holy cow! Things in this table are partially changed.
			// It will be left in a bad state.
			ASSERT(FALSE);
			return;
		}
		pNewCell->set_panel(CELL_PANEL);
		pNewCell->set_select_flags(SELECT_FLAG_boundary | SELECT_FLAG_draw_boundary);
		pNewCell->SetCell(cpCell);
		pList->insert_before(pNewCell, pObject);		// Add it to the list.

		// If this is the first cell in the row, update the row start entry.
		if (cpCell.x == 0)
		{
			m_cpaRowStarts[cpCell.y] = pNewCell;
		}

		// Now, bump the column numbers for the cells after this one.
		while (++cpCell.x < nColumns)
		{
			CCellObject* pCell = (CCellObject*)pObject;
			ASSERT(pCell != NULL);
			ASSERT(pCell->GetColumn() == cpCell.x-1);
			ASSERT(pCell->GetRow() == cpCell.y);
			pCell->SetColumn((short)cpCell.x);

			// Advance to the next object.
			pObject = (PMGPageObject*)pObject->next_object();
		}
	}

	// Now go through and set the sizes for all the cells.
	calc();
}

//
// Delete a column
//

void CTableObject::DeleteColumn(int nDeleteColumn)
{
	// Get the current number of columns.
	int nColumns = GetColumns();
	int nRows = GetRows();

	// Validate the parameter passed.
	if (nDeleteColumn < 0 || nDeleteColumn >= nColumns)
	{
		ASSERT(FALSE);
		return;
	}

	// Bump the number of columns
	SetColumns(--nColumns);

	// Add a new column width.
	PCOORD OldWidth = GetColumnWidth(nDeleteColumn);		// Save this!
	m_ColumnWidths.DeleteAt(nDeleteColumn);
	ASSERT(m_ColumnWidths.count() == nColumns);

	// Remove vertical border segments for the deleted column.
	int nSegment = nDeleteColumn*nRows;
	for (int n = 0; n < nRows; n++)
	{
		m_VBorders.DeleteAt(nSegment);
	}

	// Delete horizontal border segments for the deleted column.
	nSegment = nDeleteColumn /*+0*nColumns*/;
	for (n = 0; n <= nRows; n++)
	{
		m_HBorders.DeleteAt(nSegment);
		nSegment += nColumns;
	}

	// Delete cells for the new column.
	int nDestCell = nDeleteColumn /*0*nColumns*/;

	for (n = 0; n < nRows; n++)
	{
		m_CellInfo.DeleteAt(nDestCell);
		nDestCell += nColumns;
	}

	// Get an array of the current column sizes.
	PCOORD* pColumns = new PCOORD[nColumns];
	int nColumn;
	for (nColumn = 0; nColumn < nColumns; nColumn++)
	{
		pColumns[nColumn] = GetColumnWidth(nColumn);
	}

	// Compute the new bound size.
	PBOX Bound = get_unrotated_bound();
	Bound.x1 -= OldWidth;

	// Set the new bound.
	PPNT Anchor;
	get_anchor_point(OBJECT_HANDLE_RIGHT, &Anchor);

	set_unrotated_bound(Bound);

	// Set the new column sizes. We need to do this since the bound
	// changed.
	for (nColumn = 0; nColumn < GetColumns(); nColumn++)
	{
		SetColumnWidth(nColumn, pColumns[nColumn]);
	}
	compute_handles();
	move_to_anchor(OBJECT_HANDLE_RIGHT, Anchor);

	record.flags |= OBJECT_FLAG_needs_calc;

	// Compute bound and vertices.
	compute_handles();

	delete [] pColumns;

	// Remove cells from each row.
	ObjectList* pList = object_list();
	CPoint cpCell;
	for (cpCell.y = 0; cpCell.y < nRows; cpCell.y++)
	{
		// Seek across to find where to insert the cell.
		PMGPageObject* pObject = (PMGPageObject*)m_cpaRowStarts[cpCell.y];
		for (cpCell.x = 0; cpCell.x < nDeleteColumn; cpCell.x++)
		{
			pObject = (PMGPageObject*)pObject->next_object();
		}

		// Grab the next object. We need this to fix things up later.
		PMGPageObject* pNext = (PMGPageObject*)pObject->next_object();

		// If this is the first cell in the row, update the row start entry.
		if (cpCell.x == 0)
		{
			m_cpaRowStarts[cpCell.y] = pNext;
		}
		pList->detach(pObject);
		pObject->destroy();
		delete pObject;

		// We have one less object now.
		cpCell.x--;

		// Now, decrement the column numbers for the cells after this one.
		while (++cpCell.x < nColumns)
		{
			CCellObject* pCell = (CCellObject*)pNext;
			ASSERT(pCell != NULL);
			ASSERT(pCell->GetColumn() == cpCell.x+1);
			ASSERT(pCell->GetRow() == cpCell.y);
			pCell->SetColumn((short)cpCell.x);

			// Advance to the next object.
			pNext = (PMGPageObject*)pNext->next_object();
		}
	}

	// Now go through and set the sizes for all the cells.
	calc();
}

//
// Delete a row
//

void CTableObject::DeleteRow(int nDeleteRow)
{
	// Get the current number of rows.
	int nRows = GetRows();
	int nColumns = GetColumns();

	// Validate the parameter passed.
	if (nDeleteRow < 0 || nDeleteRow > nRows)
	{
		ASSERT(FALSE);
		return;
	}

	// Decrease the number of rows
	SetRows(--nRows);

	// Remove a old row height.
	PCOORD OldHeight = GetRowHeight(nDeleteRow);
	m_RowHeights.DeleteAt(nDeleteRow);
	m_DesiredRowHeights.DeleteAt(nDeleteRow);
	ASSERT(m_RowHeights.count() == nRows);
	ASSERT(m_DesiredRowHeights.count() == nRows);

	// Delete horizontal border segments for the deleted row.
	int nSegment = nDeleteRow*nColumns;
	for (int n = 0; n < nColumns; n++)
	{
		m_HBorders.DeleteAt(nSegment);
	}

	// Delete vertical border segments for the deleted row.
	nSegment = nDeleteRow /*+0*nRows*/;
	for (n = 0; n <= nColumns; n++)
	{
		m_VBorders.DeleteAt(nSegment);
		nSegment += nRows;
	}

	CPoint cpCell;
#ifdef HANDLE_THIS_SOMEDAY
	cpCell.y = nDeleteRow;
	// Update any spans that cross us.
	for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++)
	{
		CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
		if (pCellInfo != NULL)
		{
			if (pCellInfo->m_nSpanY < 0)
			{
				// We are in the middle of a merge.
				CPoint cpMaster = cpCell;
				cpMaster.y += pCellInfo->m_nSpanY;
				CTableCellInfo* pMaster = GetCellInfo(cpMaster);
				if (pMaster != NULL)
				{
					// We are being removed.
					pMaster->m_nSpan--;
				}
				// Handle any cells after us.
			}
			else if (pCellInfo->m_nSpanX > 1)
			{
				// We are the start of a span.
				
			}
		}
	}
#endif

	// Delete cells for the deleted row.
	int nDestCell = nDeleteRow*nColumns;
	for (n = 0; n < nColumns; n++)
	{
		m_CellInfo.DeleteAt(nDestCell);
	}

	// Get an array of the current row sizes.
	PCOORD* pRows = new PCOORD[nRows];
	int nRow;
	for (nRow = 0; nRow < nRows; nRow++)
	{
		pRows[nRow] = GetRowHeight(nRow);
	}

	// Compute the new bound size.
	PBOX Bound = get_unrotated_bound();
	Bound.y1 -= OldHeight;

	// Set the new bound.
	PPNT Anchor;
	get_anchor_point(OBJECT_HANDLE_LOWER, &Anchor);

	set_unrotated_bound(Bound);

	// Set the new row sizes. We need to do this since the bound
	// changed.
	for (nRow = 0; nRow < GetRows(); nRow++)
	{
		SetRowHeight(nRow, pRows[nRow]);
	}
	compute_handles();
	move_to_anchor(OBJECT_HANDLE_LOWER, Anchor);

	record.flags |= OBJECT_FLAG_needs_calc;

	// Compute bound and vertices.
	compute_handles();

	delete [] pRows;

	// Update the desired row heights from the current ones.
	UpdateDesiredRowHeights();

	// Delete the row of cells.
	ObjectList* pList = object_list();

	// Delete all the new cells.
	cpCell.y = nDeleteRow;
	PMGPageObject* pObject = (PMGPageObject*)m_cpaRowStarts[cpCell.y];
	for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++)
	{
		PMGPageObject* pNext = (PMGPageObject*)pObject->next_object();

		pList->detach(pObject);
		pObject->destroy();
		delete pObject;
		pObject = pNext;
	}

	// Delete the fated row start.
	m_cpaRowStarts.RemoveAt(cpCell.y);
	ASSERT(m_cpaRowStarts.GetSize() == nRows);

	// Now, run through the rest of the rows and bump the y values.
	for (; cpCell.y < nRows; cpCell.y++)
	{
		PMGPageObject* pObject = (PMGPageObject*)m_cpaRowStarts[cpCell.y];
		for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++)
		{
			CCellObject* pCell = (CCellObject*)pObject;
			ASSERT(pCell != NULL);
			ASSERT(pCell->GetRow() == cpCell.y+1);
			ASSERT(pCell->GetColumn() == cpCell.x);
			pCell->SetRow((short)cpCell.y);

			pObject = (PMGPageObject*)pObject->next_object();
		}
	}

	// Now go through and set the sizes for all the cells.
	calc();
}

// Get new column information from the passed column.
void CTableObject::GetNewColumnInfo(int nColumn, CTableNewRCInfo* pNewInfo)
{
	pNewInfo->m_Size = GetColumnWidth(nColumn);

	int nRows = GetRows();
	pNewInfo->AllocateInfo(nRows);

	// Gather all the important data.
	CPoint cpCell;
	cpCell.x = nColumn;

	CTableBorderSegment* pSegment = pNewInfo->m_pSegments;
	COLOR* pColor = pNewInfo->m_pFillColors;

	for (cpCell.y = 0; cpCell.y < nRows; cpCell.y++)
	{
		*pSegment = *GetTopBorder(cpCell);
		pSegment->m_bType &= ~BORDER_SEGMENT_Spanned;
		*pColor = GetCellInfo(cpCell)->m_Color;

		pSegment++;
		pColor++;
	}
	// This is a hack, but it works. Even though there is no cell corresponding
	// to the current value of cpCell (which is off the bottom), getting
	// the top of it works due to the calculations involved.
	*pSegment = *GetTopBorder(cpCell);
	pSegment->m_bType &= ~BORDER_SEGMENT_Spanned;
}

// Get new row information from the passed column.
void CTableObject::GetNewRowInfo(int nRow, CTableNewRCInfo* pNewInfo)
{
	pNewInfo->m_Size = GetRowHeight(nRow);

	int nColumns = GetColumns();
	pNewInfo->AllocateInfo(nColumns);

	// Gather all the important data.
	CPoint cpCell;
	cpCell.y = nRow;

	CTableBorderSegment* pSegment = pNewInfo->m_pSegments;
	COLOR* pColor = pNewInfo->m_pFillColors;

	for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++)
	{
		*pSegment = *GetLeftBorder(cpCell);
		pSegment->m_bType &= ~BORDER_SEGMENT_Spanned;
		*pColor = GetCellInfo(cpCell)->m_Color;

		pSegment++;
		pColor++;
	}
	// This is a hack, but it works. Even though there is no cell corresponding
	// to the current value of cpCell (which is off the right), getting
	// the left of it works due to the calculations involved.
	*pSegment = *GetLeftBorder(cpCell);
	pSegment->m_bType &= ~BORDER_SEGMENT_Spanned;
}

// Get the cells within an area that are merged.
void CTableObject::GetMergedCells(const CRect& crArea, CArray<CRect,CRect&>& arRects)
{
	CPoint cpCell;
	for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
	{
		cpCell.x = crArea.left;
		CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
		for (; cpCell.x <= crArea.right; cpCell.x++, pCellInfo++)
		{
			if (pCellInfo->m_nSpanX != 1 || pCellInfo->m_nSpanY != 1)
			{
				if (pCellInfo->m_nSpanX >= 1 && pCellInfo->m_nSpanY >= 1)
				{
					CRect crCell;
					crCell.left = cpCell.x;
					crCell.top = cpCell.y;
					crCell.right = crCell.left + pCellInfo->m_nSpanX-1;
					crCell.bottom = crCell.top + pCellInfo->m_nSpanY-1;
					arRects.Add(crCell);
				}
				else
				{
					// Spanned cell.
					ASSERT(pCellInfo->IsSpanned());
				}
			}
		}
	}
}

// Merge the selected cells.
void CTableObject::MergeCells(void)
{
	if (!SomethingSelected())
	{
		ASSERT(FALSE);
		return;
	}

	// Merge the cells in the selection.
	MergeCells(m_crSelect);
}

void CTableObject::MergeCells(CRect crArea, BOOL fMergeContents /*=TRUE*/)
{
	// We want to make the upper left cell the "master" and all the rest
	// "spanned".

	// Set the spans for all the contained cells to point back to the master.
	// We set the master cell here as well, but fix it up later.
	CPoint cpCell;
	for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
	{
		cpCell.x = crArea.left;
		CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
		for ( ; cpCell.x <= crArea.right; cpCell.x++, pCellInfo++)
		{
			if (cpCell.x == crArea.left && cpCell.y == crArea.top)
			{
				// First cell is now the "master".
				pCellInfo->m_nSpanX = crArea.Width()+1;
				pCellInfo->m_nSpanY = crArea.Height()+1;
			}
			else
			{
				// All the rest of the cells are spanned.
				pCellInfo->m_nSpanX = (short)(crArea.left - cpCell.x);
				pCellInfo->m_nSpanY = (short)(crArea.top - cpCell.y);
				// Hide the cell at this point.
				CCellObject* pCell = GetCellAt(cpCell);
				if (pCell != NULL)
				{
					pCell->add_flags(OBJECT_FLAG_hidden);
				}
			}
		}
	}

	//
	// Now, hide the edges which are now spanned.
	//

	if (crArea.Width() > 0)
	{
		// We need to knock out the vertical lines between the merged cells.
		CPoint cpCell;
		for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
		{
			for (cpCell.x = crArea.left; cpCell.x < crArea.right; cpCell.x++)
			{
				GetRightBorder(cpCell)->m_bType |= BORDER_SEGMENT_Spanned;
			}
		}
	}

	if (crArea.Height() > 0)
	{
		// We need to knock out the horizontal lines between the merged cells.
		CPoint cpCell;
		for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
		{
			for (cpCell.y = crArea.top; cpCell.y < crArea.bottom; cpCell.y++)
			{
				GetBottomBorder(cpCell)->m_bType |= BORDER_SEGMENT_Spanned;
			}
		}
	}

	// Layout the cells that changed.
	LayoutCells(&crArea);

	// If we want to merge the contents, do it now.
	if (fMergeContents)
	{
		MergeContents(crArea);
	}

	// And redraw.
	InvalidateCells(crArea);
}

BOOL CTableObject::CanMergeCells(void)
{
	if (SomethingSelected())
	{
		CTableCellInfo* pCellInfo = GetCellInfo(m_crSelect.TopLeft());
		if (m_crSelect.left + pCellInfo->m_nSpanX - 1 != m_crSelect.right
			 || m_crSelect.top + pCellInfo->m_nSpanY - 1 != m_crSelect.bottom)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//
// Merge the contents of the specified cells into the upper left corner.
// All current frames are detached (including the upper left corner) so
// that undo can work.
//

void CTableObject::MergeContents(CRect crArea)
{
	// Get the master frame we are going to be putting the text into.
	CFrameObject* pFrame = GetCellFrame(crArea.TopLeft(), FALSE);
	if (pFrame != NULL)
	{
		// Initialize a Txp so we can access the text.
		CTxp Txp(NULL);
		Txp.Init(pFrame, 0);
		// Go to the end of the text.
		Txp.Eot(FALSE);

		// Copy all the rest of the text to this frame.
		CPoint cpCell;
		for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
		{
			for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
			{
				// Skip the first one.
				if (cpCell != crArea.TopLeft())
				{
					// Get the frame for this cell.
					CFrameObject* pCellFrame = GetCellFrame(cpCell, FALSE);
					if (pCellFrame != NULL)
					{
						// Paste the text to the first cell.
						PasteTextFromText(&Txp,
												pCellFrame->get_database(),
												pCellFrame->TextRecord());
						Txp.Eot(FALSE);

						// Delete the text we just copied.
						CTxp CellTxp(NULL);
						CellTxp.Init(pCellFrame, 0);
						CellTxp.SelectAll();
						if (CellTxp.SomethingSelected())
						{
							// Delete the selection.
							CellTxp.DeleteText(1);
						}
					}
				}
				// Next slot.
				if (cpCell.x != crArea.right)
				{
					Txp.CalcStyle();
					Txp.InsertString(" ");
				}
			}
			// Next line.
			if (cpCell.y != crArea.bottom)
			{
				Txp.CalcStyle();
				Txp.InsertString("\n");
			}
		}
	}
}

// Split all merged cells within the current selection.
void CTableObject::SplitCells(CArray<CRect, CRect&>& arMerged)
{
	int nMerged = arMerged.GetSize();
	CRect crTotal;
	for (int n = 0; n < nMerged; n++)
	{
		CPoint cpCell = arMerged.GetAt(n).TopLeft();
		SplitCell(cpCell);
	}
}

BOOL CTableObject::CanSplitCells(void)
{
	if (SomethingSelected())
	{
		// Look through all the cells for something merged or spanned.
		// Theoretically, since we are searching from the upper left corner down,
		// we should hit the master cells first.
		CPoint cpCell;
		for (cpCell.y = m_crSelect.top; cpCell.y <= m_crSelect.bottom; cpCell.y++)
		{
			cpCell.x = m_crSelect.left;
			CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
			for (; cpCell.x <= m_crSelect.right; cpCell.x++, pCellInfo++)
			{
				if (pCellInfo->m_nSpanX != 1 || pCellInfo->m_nSpanY != 1)
				{
					// Found a master (or spanned) cell.
					// Since we should hit the master, spanning cell first,
					// ASSERT that that is the case.
					ASSERT(pCellInfo->m_nSpanX >= 1 && pCellInfo->m_nSpanY >= 1);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void CTableObject::SplitCell(CPoint cpMergedCell)
{
	// We want to make the upper left cell the "master" and all the rest
	// "spanned".

	CTableCellInfo* pCellInfo = GetCellInfo(cpMergedCell);
	CRect crSpan;
	crSpan.TopLeft() = cpMergedCell;
	crSpan.right = crSpan.left + pCellInfo->m_nSpanX - 1;
	crSpan.bottom = crSpan.top + pCellInfo->m_nSpanY - 1;
	// Set the spans for all cells to (1, 1).
	CPoint cpCell;
	for (cpCell.y = crSpan.top; cpCell.y <= crSpan.bottom; cpCell.y++)
	{
		cpCell.x = crSpan.left;
		CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
		for ( ; cpCell.x <= crSpan.right; cpCell.x++, pCellInfo++)
		{
			pCellInfo->m_nSpanX = 1;
			pCellInfo->m_nSpanY = 1;
			// Remove the hidden flag from all objects.
			// This happens to the "master", too, but it has no ill effect.
			CCellObject* pCell = GetCellAt(cpCell);
			if (pCell != NULL)
			{
				pCell->remove_flags(OBJECT_FLAG_hidden);
			}
		}
	}

	//
	// Now, show the edges which are no longer spanned.
	//

	if (crSpan.Width() > 0)
	{
		// We need to knock out the vertical lines between the merged cells.
		CPoint cpCell;
		for (cpCell.y = crSpan.top; cpCell.y <= crSpan.bottom; cpCell.y++)
		{
			for (cpCell.x = crSpan.left; cpCell.x < crSpan.right; cpCell.x++)
			{
				GetRightBorder(cpCell)->m_bType &= ~BORDER_SEGMENT_Spanned;
			}
		}
	}

	if (crSpan.Height() > 0)
	{
		// We need to knock out the horizontal lines between the merged cells.
		CPoint cpCell;
		for (cpCell.x = crSpan.left; cpCell.x <= crSpan.right; cpCell.x++)
		{
			for (cpCell.y = crSpan.top; cpCell.y < crSpan.bottom; cpCell.y++)
			{
				GetBottomBorder(cpCell)->m_bType &= ~BORDER_SEGMENT_Spanned;
			}
		}
	}
	// TODO:
	// Split the cell text into all the little cells.

	// Layout the cells that changed.
	LayoutCells(&crSpan);

	// And redraw.
	InvalidateCells(crSpan);
}

//
// Fill down.
//
void CTableObject::FillDown(void)
{
	CPoint cpCell;
	for (cpCell.x = m_crSelect.left; cpCell.x <= m_crSelect.right; cpCell.x++)
	{
		cpCell.y = m_crSelect.top;
		CFrameObject* pFrame = GetCellFrame(cpCell);
		while (++cpCell.y <= m_crSelect.bottom)
		{
			// Copy the contents to this cell.
			CopyFrameToCell(pFrame, cpCell);
		}
	}
	CRect crCells = m_crSelect;
	crCells.top++;
	LayoutCells(&crCells);
	InvalidateCells(crCells);
}

//
// Can we fill down?
//

BOOL CTableObject::CanFillDown(void)
{
	return SomethingSelected() && m_crSelect.Height() > 0;
}

//
// Fill right.
//
void CTableObject::FillRight(void)
{
	CPoint cpCell;
	for (cpCell.y = m_crSelect.top; cpCell.y <= m_crSelect.bottom; cpCell.y++)
	{
		cpCell.x = m_crSelect.left;
		CFrameObject* pFrame = GetCellFrame(cpCell);
		while (++cpCell.x <= m_crSelect.right)
		{
			// Copy the contents to this cell.
			CopyFrameToCell(pFrame, cpCell);
		}
	}
	CRect crCells = m_crSelect;
	crCells.left++;
	LayoutCells(&crCells);
	InvalidateCells(crCells);
}

//
// Can we fill right?
//
BOOL CTableObject::CanFillRight(void)
{
	return SomethingSelected() && m_crSelect.Width() > 0;
}

//
// Copy the contents of a text frame to a cell.
//

void CTableObject::CopyFrameToCell(CFrameObject* pFrame, CPoint cpCell)
{
	CCellObject* pCell = GetCellAt(cpCell);
	if (pCell != NULL)
	{
		// Handle the cell contents.
		ObjectList* pList = pCell->object_list();

		// Detach all the current cell content objects.
		ASSERT(pList->count() <= 1);
		PMGPageObject* pOldObject = (PMGPageObject*)pList->first_object();
		if (pOldObject != NULL)
		{
			pList->detach(pOldObject);
			pOldObject->OnPage(FALSE);
		}

		// Now, create a duplicate for the frame if one was passed.
		if (pFrame != NULL)
		{
			CFrameObject* pDupFrame = (CFrameObject*)pFrame->duplicate();
			if (pDupFrame != NULL)
			{
				pList->append(pDupFrame);
				pDupFrame->OnPage(TRUE);		// Now on the page.
			}
		}
	}
}

ERRORCODE CTableObject::AddCell(const PBOX& Bound, const CPoint& cpCell, SHORT panel, FLAGS sel_flags, ObjectListPtr pList)
{
	CCellObject* pObject;

	// Create the object.
	if ((pObject = (CCellObject*)get_database()->new_object(OBJECT_TYPE_Cell, NULL)) == NULL)
	{
		return get_database()->last_creation_error();
	}

	pObject->set_bound(Bound);
	pObject->set_panel(panel);
	pObject->set_select_flags(sel_flags);
	pObject->SetCell(cpCell);

	// Add it to the list.
	pList->append(pObject);

	return ERRORCODE_None;
}

//
// return cell located at (x,y)
// return NULL if cell can't be found, doesn't exist, etc.
// indices are zero based
//

CCellObject* CTableObject::GetCellAt(CPoint cpCell)
{
	CCellObject* pCell = NULL;
	// Validate arguments.
	if (cpCell.y >= 0
		 && cpCell.y < GetRows()
		 && cpCell.x >= 0
		 && cpCell.x < GetColumns())
	{
		if (cpCell.y < m_cpaRowStarts.GetSize())
		{
			PageObject* pObject = (PageObject*)m_cpaRowStarts.GetAt(cpCell.y);
			ASSERT(pObject != NULL);
			// Advance to the desired column.
			for (int nC = 0; (nC < cpCell.x) && (pObject != NULL); nC++)
			{
				pObject = pObject->next_object();
			}

			// Do some validation.
			// This is important in case the table ever gets corrupted somehow.
			if (pObject == NULL)
			{
				// This is a bad state. Not enough columns.
				ASSERT(FALSE);
			}
			else
			{
				pCell = (CCellObject*)pObject;
				pCell->set_container(this);
				if (pCell->GetRow() != cpCell.y || pCell->GetColumn() != cpCell.x)
				{
					ASSERT(FALSE);
					pCell = NULL;
				}
			}
		}
	}

	return pCell;
}	

//
// resizes a column of cells, call from any cell, will resize all cells in it's row
// moves the right edge of this cell and the left edge of the adjoining cell
//
void CTableObject::ResizeColumn(int nResizeColumn, DRAG_STATE state, PPNT p, FLAGS shift_status)
{
	// Normalize the point.
	NormalizePoint(p);

	TRACE("Resize column %d: %ld\n", nResizeColumn, p.x);

	ASSERT(nResizeColumn < GetColumns());

	switch (state)
	{
		case DRAG_STATE_begin:
		{
			// No more select.
			DeselectAll();

			// Save the current column widths.
			bound_save = rrecord.unrotated_bound;
			m_RCSave = m_ColumnWidths;

			// Turn off sizing handles.
			turn_select_off(SELECT_FLAG_size_handles | SELECT_FLAG_rotate_handle | SELECT_FLAG_subselect);
			add_refresh_flags(REFRESH_FLAG_panning);

         // Toggle the boundary just for symmetry with DRAG_STATE_end.
         // This has no real effect for the current code (since the code only
         // does the toggle when the object is different from initial conditions).

         get_database()->toggle_object(this, SELECT_FLAG_boundary);
			break;
		}
		case DRAG_STATE_continue:
		{
			// Get an array of the current column sizes.
			// Compute the left of the column we are resizing while we are at it.
			PCOORD* pColumns = new PCOORD[GetColumns()];
			PCOORD Left = 0;
			int nColumn;
			for (nColumn = 0; nColumn < GetColumns(); nColumn++)
			{
				pColumns[nColumn] = GetColumnWidth(nColumn);
				if (nColumn < nResizeColumn)
					Left += pColumns[nColumn];
			}

			static PCOORD MinWidth = PAGE_RESOLUTION/10;

			PCOORD NewWidth = p.x - Left;
			if (NewWidth < MinWidth)
			{
				NewWidth = MinWidth;
			}

			if (shift_status & MK_SHIFT)
			{
				// Shifted. This means only adjust within the column.
				// The object does not change size.
				PCOORD Right = Left + GetColumnWidth(nResizeColumn);
				PCOORD NextWidth = GetColumnWidth(nResizeColumn+1);
				PCOORD NextRight = Right + NextWidth;
				if (Left + NewWidth + MinWidth > NextRight)
				{
					if (NextRight - Left < MinWidth*2)
					{
						// Not enough room to enforce MinWidth limit.
						// Pick half of what is available.
						NewWidth = (NextRight - Left)/2;
					}
					else
					{
						// Enforce MinWidth limit.
						NewWidth = (NextRight - MinWidth) - Left;
					}
				}
				if (NewWidth != pColumns[nResizeColumn])
				{
					PCOORD Delta = NewWidth - pColumns[nResizeColumn];
					pColumns[nResizeColumn] += Delta;
					pColumns[nResizeColumn+1] -= Delta;

					// Set the new bound.
					get_database()->toggle_object(this, 0);

					// Set the new column sizes. We need to do this since the bound
					// changed.
					SetColumnWidth(nResizeColumn, pColumns[nResizeColumn]);
					SetColumnWidth(nResizeColumn+1, pColumns[nResizeColumn+1]);

					record.flags |= OBJECT_FLAG_needs_calc;

					// Compute bound and vertices.
					compute_handles();

					get_database()->toggle_object(this, 0);
				}
			}
			else
			{
				// Normal. This means move the remaining columns, growing the object
				// as needed
				if (NewWidth != pColumns[nResizeColumn])
				{
					PBOX Bound = get_unrotated_bound();
					Bound.x1 += (NewWidth - pColumns[nResizeColumn]);

					// Set the new size.
					pColumns[nResizeColumn] = NewWidth;
					PPNT Anchor;
					get_anchor_point(OBJECT_HANDLE_RIGHT, &Anchor);

					// Set the new bound.
					get_database()->toggle_object(this, 0);
					set_unrotated_bound(Bound);

					// Set the new column sizes. We need to do this since the bound
					// changed.
					for (int nColumn = 0; nColumn < GetColumns(); nColumn++)
					{
						SetColumnWidth(nColumn, pColumns[nColumn]);
					}
					compute_handles();
					move_to_anchor(OBJECT_HANDLE_RIGHT, Anchor);

					record.flags |= OBJECT_FLAG_needs_calc;

					// Compute bound and vertices.
					compute_handles();

					get_database()->toggle_object(this, 0);
				}
			}

			delete [] pColumns;
			break;
		}
		case DRAG_STATE_abort:
		{
			// Restore the object.
			get_database()->toggle_object(this, 0);

			// Restore the current column widths.
			rrecord.unrotated_bound = bound_save;
			m_ColumnWidths = m_RCSave;
			record.flags &= ~OBJECT_FLAG_needs_calc;

			// Compute bound and vertices from this.
			calc();

			get_database()->toggle_object(this, 0);
			// Fall through to...
		}
		case DRAG_STATE_end:
		{
         // Toggle the boundary off.
         // Most likely, the user has changed the object somehow so
         // that the boundary is now visible. We want it to end up off.
         // If the object has not been moved, this has no effect.

         get_database()->toggle_object(this, SELECT_FLAG_boundary);

			// Restore handles.
			remove_refresh_flags(REFRESH_FLAG_panning);
			restore_select();
			break;
		}
		default:
		{
			break;
		}
	}
}

void CTableObject::ResizeRow(int nResizeRow, DRAG_STATE state, PPNT p, FLAGS shift_status)
{		
	// Normalize the point.
	NormalizePoint(p);

	TRACE("Resize row %d: %ld\n", nResizeRow, p.y);

	ASSERT(nResizeRow < GetRows());

	switch (state)
	{
		case DRAG_STATE_begin:
		{
			// No more select.
			DeselectAll();

			// Save the current column widths.
			bound_save = rrecord.unrotated_bound;
			m_RCSave = m_ColumnWidths;

			// Turn off sizing handles.
			turn_select_off(SELECT_FLAG_size_handles | SELECT_FLAG_rotate_handle | SELECT_FLAG_subselect);
			add_refresh_flags(REFRESH_FLAG_panning);

         // Toggle the boundary just for symmetry with DRAG_STATE_end.
         // This has no real effect for the current code (since the code only
         // does the toggle when the object is different from initial conditions).

         get_database()->toggle_object(this, SELECT_FLAG_boundary);
			break;
		}
		case DRAG_STATE_continue:
		{
			// Get an array of the current row sizes.
			// Compute the top of the row we are resizing while we are at it.
			PCOORD* pRows = new PCOORD[GetRows()];
			PCOORD Top = 0;
			int nRow;
			for (nRow = 0; nRow < GetRows(); nRow++)
			{
				pRows[nRow] = GetRowHeight(nRow);
				if (nRow < nResizeRow)
					Top += pRows[nRow];
			}

			static PCOORD MinHeight = PAGE_RESOLUTION/10;

			PCOORD NewHeight = p.y - Top;
			if (NewHeight < MinHeight)
			{
				NewHeight = MinHeight;
			}

			if (shift_status & MK_SHIFT)
			{
				// Shifted. This means only adjust within the row.
				// The object does not change size.
				PCOORD Bottom = Top + GetRowHeight(nResizeRow);
				PCOORD NextHeight = GetRowHeight(nResizeRow+1);
				PCOORD NextBottom = Bottom + NextHeight;
				if (Top + NewHeight + MinHeight > NextBottom)
				{
					if (NextBottom - Top < MinHeight*2)
					{
						// Not enough room to enforce MinHeight limit.
						// Pick half of what is available.
						NewHeight = (NextBottom - Top)/2;
					}
					else
					{
						// Enforce MinHeight limit.
						NewHeight = (NextBottom - MinHeight) - Top;
					}
				}
				if (NewHeight != pRows[nResizeRow])
				{
					PCOORD Delta = NewHeight - pRows[nResizeRow];
  					pRows[nResizeRow] += Delta;
					pRows[nResizeRow+1] -= Delta;

					get_database()->toggle_object(this, 0);

					// Set the new row sizes.
					SetRowHeight(nResizeRow, pRows[nResizeRow]);
					SetRowHeight(nResizeRow+1, pRows[nResizeRow+1]);

					record.flags |= OBJECT_FLAG_needs_calc;

					// Compute bound and vertices.
					compute_handles();

					get_database()->toggle_object(this, 0);
				}
			}
			else
			{
				// Normal. This means move the remaining rows, growing the object
				// as needed
				if (NewHeight != pRows[nResizeRow])
				{
					PBOX Bound = get_unrotated_bound();
					Bound.y1 += (NewHeight - pRows[nResizeRow]);

					// Set the new size.
					pRows[nResizeRow] = NewHeight;

					PPNT Anchor;
					get_anchor_point(OBJECT_HANDLE_LOWER, &Anchor);

					// Set the new bound.
					get_database()->toggle_object(this, 0);
					set_unrotated_bound(Bound);

					// Set the new row sizes. We need to do this since the bound
					// changed.
					for (int nRow = 0; nRow < GetRows(); nRow++)
					{
						SetRowHeight(nRow, pRows[nRow]);
					}

               compute_handles();
					move_to_anchor(OBJECT_HANDLE_LOWER, Anchor);

					record.flags |= OBJECT_FLAG_needs_calc;

					// Compute bound and vertices.
					compute_handles();

					get_database()->toggle_object(this, 0);
				}
			}

			// Update the desired row heights.
			UpdateDesiredRowHeights();

			delete [] pRows;
			break;
		}
		case DRAG_STATE_abort:
		{
			// Restore the object.
			get_database()->toggle_object(this, 0);

			// Restore the current row heights.
			rrecord.unrotated_bound = bound_save;
			m_RowHeights = m_RCSave;
			record.flags &= ~OBJECT_FLAG_needs_calc;

			// Compute bound and vertices from this.
			calc();

			get_database()->toggle_object(this, 0);
			// Fall through to...
		}
		case DRAG_STATE_end:
		{
         // Toggle the boundary off.
         // Most likely, the user has changed the object somehow so
         // that the boundary is now visible. We want it to end up off.
         // If the object has not been moved, this has no effect.

         get_database()->toggle_object(this, SELECT_FLAG_boundary);

			// Restore handles.
			remove_refresh_flags(REFRESH_FLAG_panning);
			restore_select();
			break;
		}
		default:
		{
			break;
		}
	}
}

//
// Recompute all the cell sizes after a grid change.
// Alternatively, you can pass just the area that changed.
//

void CTableObject::LayoutCells(const CRect* pcrArea /*=NULL*/)
{
	int nColumns = GetColumns();
	int nRows = GetRows();

	PBOX Bound = get_unrotated_bound();
	BOOL fXFlipped = (get_flags() & OBJECT_FLAG_xflipped);
	BOOL fYFlipped = (get_flags() & OBJECT_FLAG_yflipped);
	int nXSign = fXFlipped ? -1 : 1;
	int nYSign = fYFlipped ? -1 : 1;

	// Loop on rows.
	PPNT Org;
	Org.y = fYFlipped ? Bound.y1 : Bound.y0;
	CPoint cpCell;
	for (cpCell.y = 0; cpCell.y < nRows; cpCell.y++)
	{
		PCOORD RowHeight = GetRowHeight(cpCell.y)*nYSign;

		// Get the cell info for this row. We can access it sequentially
		// from there for all the columns in this row.
		cpCell.x = 0;
		if (pcrArea == NULL
				|| (cpCell.y >= pcrArea->top && cpCell.y <= pcrArea->bottom))
		{
			CTableCellInfo* pCellInfo = GetCellInfo(cpCell);

			// Loop on columns.
			Org.x = fXFlipped ? Bound.x1 : Bound.x0;
			for (; cpCell.x < nColumns; cpCell.x++, pCellInfo++)
			{
				PCOORD ColumnWidth = GetColumnWidth(cpCell.x)*nXSign;

				if (pcrArea == NULL
						|| (cpCell.x >= pcrArea->left && cpCell.x <= pcrArea->right))
				{
					CCellObject* pCell = GetCellAt(cpCell);
					if (pCell != NULL)
					{
						// Compute the new bound.
						PBOX NewBound;
						NewBound.x0 = NewBound.x1 = Org.x;
						NewBound.y0 = NewBound.y1 = Org.y;

						if (pCellInfo->IsSpanned())
						{
							// Pick the nominal size.
							NewBound.x1 += ColumnWidth;
							NewBound.y1 += RowHeight;
						}
						else
						{
							int n;
							int nRC;
							// Compute the width of the cell.
							for (nRC = cpCell.x, n = 0; n < pCellInfo->m_nSpanX; nRC++, n++)
							{
								NewBound.x1 += GetColumnWidth(nRC)*nXSign;
							}
							// Compute the height of the cell.
							for (nRC = cpCell.y, n = 0; n < pCellInfo->m_nSpanY; nRC++, n++)
							{
								NewBound.y1 += GetRowHeight(nRC)*nYSign;
							}
						}

						// Normalize the bound if necessary.
						if (NewBound.x0 > NewBound.x1)
						{
							PCOORD t = NewBound.x0;
							NewBound.x0 = NewBound.x1;
							NewBound.x1 = t;
						}
						if (NewBound.y0 > NewBound.y1)
						{
							PCOORD t = NewBound.y0;
							NewBound.y0 = NewBound.y1;
							NewBound.y1 = t;
						}

						double dAngle = get_rotation();
						// Handle offset caused by rotation.
						if (dAngle != 0.0)
						{
							// We need to move the bound.
							// Compute the center of the bound.
							PPNT Center, Delta;
							Center.x = NewBound.CenterX();
							Center.y = NewBound.CenterY();
							Delta = Center;
							rotate_points(&Center, 1, get_origin(), dAngle);
							Delta.x = Center.x - Delta.x;
							Delta.y = Center.y - Delta.y;
							NewBound.x0 += Delta.x;
							NewBound.y0 += Delta.y;
							NewBound.x1 += Delta.x;
							NewBound.y1 += Delta.y;
						}
						// Set the new bound.
						pCell->set_unrotated_bound(NewBound);
						pCell->set_rotation(dAngle);
						pCell->calc();
					}
				}
				Org.x += ColumnWidth;
			}
		}
		Org.y += RowHeight;
	}
}

// Calculate various things for a table.
void CTableObject::CalculateTable(void)
{
	//
	// Figure out where the rows start.
	//
	ObjectList* pList = object_list();

	// Make sure we have no current row starts.
	m_cpaRowStarts.RemoveAll();

	// Set up to loop.
	int nRows = GetRows();
	int nColumns = GetColumns();
	if (pList->count() != nRows*nColumns)
	{
		ASSERT(FALSE);
		return;
	}

	PageObject* pObject = pList->first_object();
	// We keep our cells in row-major format.
	for (int nRow = 0; nRow < nRows; nRow++)
	{
		// Add the starting object.
		m_cpaRowStarts.Add(pObject);

		// Skip the rest of this row.
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			pObject = pObject->next_object();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Cell Selection

void CTableObject::SetSelect(const CRect& crSelect, BOOL fToggleSelect /*=TRUE*/)
{
	CRect crNewSelect = crSelect;
	// Expand the select to include all merged cells.
	if (crNewSelect.left != -1)
	{
		MergeExpandSelect(crNewSelect);
	}

	if (crNewSelect != m_crSelect)
	{
		// We can someday optimize this to toggle parts of the object
		// as the select changes size. Someday...

		// Turn selects off.
		if (fToggleSelect)
		{
			get_database()->toggle_object(this, SELECT_FLAG_subselect);
		}

		// Set the new select.
		m_crSelect = crNewSelect;

		// Turn selects back on.
		if (fToggleSelect)
		{
			get_database()->toggle_object(this, SELECT_FLAG_subselect);
		}
		// Update the select style.
		ComputeSelectTextStyle();
	}
}

void CTableObject::DeselectAll(BOOL fToggleSelect /*=TRUE*/)
{
	SetSelect(CRect(-1, -1, -1, -1), fToggleSelect);
}

//
// Invalidate (redraw) some cells.
//

void CTableObject::InvalidateCells(const CRect& crInvalidate)
{
	PPNT v[4];
	if (GetSelectPoints(v, crInvalidate, TRUE))
	{
		PBOX Bound;
		// Compute the bound enclosing these objects.
		compute_enclosing_bound(v, 4, &Bound);
		// Do the refresh.
		get_database()->do_refresh_notify(&Bound, REFRESH_ALL, NULL);
	}
}

//
// Invalidate (redraw) the current selection.
//

void CTableObject::InvalidateSelection(void)
{
	// For now, do the whole thing. This should be changed to be more selective.
	if (SomethingSelected())
	{
		InvalidateCells(m_crSelect);
	}
}

//
// Expand the passed select rectangle to include the full extent of all
// merged cells.
//

void CTableObject::MergeExpandSelect(CRect& crNewSelect)
{
	BOOL fChanged;
	// Expand the select while we keep running into merged cells.
	do
	{
		fChanged = FALSE;
		CRect crSelect = crNewSelect;
		CPoint cpCell;
		for (cpCell.y = crSelect.top; cpCell.y <= crSelect.bottom; cpCell.y++)
		{
			cpCell.x = crSelect.left;
			CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
			for ( ; cpCell.x <= crSelect.right; cpCell.x++, pCellInfo++)
			{
				if (pCellInfo->IsSpanned())
				{
					// We are spanned.
					int nLeft = cpCell.x + pCellInfo->m_nSpanX;
					int nTop = cpCell.y + pCellInfo->m_nSpanY;
					if (nLeft < crNewSelect.left)
					{
						crNewSelect.left = nLeft;
						fChanged = TRUE;
					}
					if (nTop < crNewSelect.top)
					{
						crNewSelect.top = nTop;
						fChanged = TRUE;
					}
				}
				else
				{
					// We are spanning (perhaps only ourselves).
					int nRight = cpCell.x + (pCellInfo->m_nSpanX-1);
					int nBottom = cpCell.y + (pCellInfo->m_nSpanY-1);
					if (nRight > crNewSelect.right)
					{
						crNewSelect.right = nRight;
						fChanged = TRUE;
					}
					if (nBottom > crNewSelect.bottom)
					{
						crNewSelect.bottom = nBottom;
						fChanged = TRUE;
					}
				}
			}
		}
	} while (fChanged);
}

#if 0
void CTableObject::SelectRight(void)
{
}

void CTableObject::SelectLeft(void)
{
}

void CTableObject::SelectUp(void)
{
}

void CTableObject::SelectDown(void)
{
}

void CTableObject::SelectNext(void)
{
}

void CTableObject::SelectPrevious(void)
{
}
#endif

BOOL CTableObject::toggle_selects(RedisplayContextPtr rc, FLAGS which)
{
	// Toggle the rest.
	INHERITED::toggle_selects(rc, which);

	// A 'which' of 0 means all we can toggle.
	if (which == 0)
	{
		which = record.select_flags;
	}

	// Toggle the select if necessary.
	if (which & SELECT_FLAG_subselect)
	{
		// We need to toggle the subselect.
		if (m_crSelect.left != -1)
		{
			ToggleSquare(rc, m_crSelect);
		}
	}

	return TRUE;
}

static void CombineColor(COLOR& Color0, COLOR Color1)
{
	if (Color0 != Color1)
	{
		Color0 = UNDEFINED_COLOR;
	}
}

static void ReplaceColor(COLOR& Color0, COLOR Color1)
{
	if (Color1 != UNDEFINED_COLOR)
	{
		Color0 = Color1;
	}
}

//
// Get the formatting for the selected cells.
//

BOOL CTableObject::GetCellFormatting(CTableCellFormatting* pFormatting)
{
	if (!SomethingSelected())
	{
		return FALSE;
	}

	// Set the type.
	pFormatting->m_nType = 0;

	// Set the left and right segments.
	CTableBorderSegment& RightSegment = pFormatting->m_Segment[CTableCellFormatting::SEGMENT_Right];
	CTableBorderSegment& LeftSegment = pFormatting->m_Segment[CTableCellFormatting::SEGMENT_Left];

	CPoint cpLeftCell(m_crSelect.left, m_crSelect.top);
	CPoint cpRightCell(m_crSelect.right, m_crSelect.top);

	// Prime it with the first cells.
	LeftSegment = *GetLeftBorder(cpLeftCell);
	RightSegment = *GetRightBorder(cpRightCell);

	// Merge in the left/right formatting for the rest of the cells in the selection.
	while (++cpRightCell.y, ++cpLeftCell.y <= m_crSelect.bottom)
	{
		LeftSegment.CombineWith(*GetLeftBorder(cpLeftCell));
		RightSegment.CombineWith(*GetRightBorder(cpRightCell));
	}

	// We need to handle the inner vertical segments.
	CPoint cpCell;
	// Set the first.
	BOOL fFirst = TRUE;
	for (cpCell.y = m_crSelect.top; cpCell.y <= m_crSelect.bottom; cpCell.y++)
	{
		// See if this row has multiple cells.
		if (m_crSelect.left + GetCellInfo(CPoint(m_crSelect.left, cpCell.y))->m_nSpanX - 1
													!= m_crSelect.right)
		{
			// We have multiple across.
			pFormatting->m_nType |= CTableCellFormatting::MultipleAcross;
			CTableBorderSegment& InnerSegment = pFormatting->m_Segment[CTableCellFormatting::SEGMENT_InnerVertical];

			for (cpCell.x = m_crSelect.left; cpCell.x < m_crSelect.right; cpCell.x++)
			{
				CTableBorderSegment* pSegment = GetRightBorder(cpCell);
				if (!pSegment->IsSpanned())
				{
					if (fFirst)
					{
						fFirst = FALSE;
						InnerSegment = *pSegment;
					}
					else
					{
						InnerSegment.CombineWith(*pSegment);
					}
				}
			}
		}
	}

	// Set the top and bottom segments.
	CTableBorderSegment& TopSegment = pFormatting->m_Segment[CTableCellFormatting::SEGMENT_Top];
	CTableBorderSegment& BottomSegment = pFormatting->m_Segment[CTableCellFormatting::SEGMENT_Bottom];

	CPoint cpTopCell(m_crSelect.left, m_crSelect.top);
	CPoint cpBottomCell(m_crSelect.left, m_crSelect.bottom);

	// Prime it with the first cells.
	TopSegment = *GetTopBorder(cpTopCell);
	BottomSegment = *GetBottomBorder(cpBottomCell);

	// Merge in the top/bottom formatting for the rest of the cells in the selection.
	while (++cpBottomCell.x, ++cpTopCell.x <= m_crSelect.right)
	{
		TopSegment.CombineWith(*GetTopBorder(cpTopCell));
		BottomSegment.CombineWith(*GetBottomBorder(cpBottomCell));
	}

	// We need to handle the inner horizontal segments.

	// Set the first.
	fFirst = TRUE;
	for (cpCell.x = m_crSelect.left; cpCell.x <= m_crSelect.right; cpCell.x++)
	{
		if (m_crSelect.top + GetCellInfo(CPoint(cpCell.x, m_crSelect.top))->m_nSpanY - 1
													!= m_crSelect.bottom)
		{
			pFormatting->m_nType |= CTableCellFormatting::MultipleDown;
			CTableBorderSegment& InnerSegment = pFormatting->m_Segment[CTableCellFormatting::SEGMENT_InnerHorizontal];
			for (cpCell.y = m_crSelect.top; cpCell.y < m_crSelect.bottom; cpCell.y++)
			{
				CTableBorderSegment* pSegment = GetBottomBorder(cpCell);
				if (!pSegment->IsSpanned())
				{
					if (fFirst)
					{
						fFirst = FALSE;
						InnerSegment = *pSegment;
					}
					else
					{
						InnerSegment.CombineWith(*pSegment);
					}
				}
			}
		}
	}

	// Get all the cell colors.
	fFirst = TRUE;
	for (cpCell.y = m_crSelect.top; cpCell.y <= m_crSelect.bottom; cpCell.y++)
	{
		for (cpCell.x = m_crSelect.left; cpCell.x <= m_crSelect.right; cpCell.x++)
		{
			CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
			if (!pCellInfo->IsSpanned())
			{
				COLOR Color = pCellInfo->m_Color;
				if (fFirst)
				{
					fFirst = FALSE;
					pFormatting->m_FillColor = Color;
				}
				else
				{
					CombineColor(pFormatting->m_FillColor, Color);
				}
			}
		}
	}
	return TRUE;
}

void CTableObject::SetCellFormatting(const CTableCellFormatting& Formatting)
{
	if (!SomethingSelected())
	{
		return;
	}

	// Set the left and right segments.
	const CTableBorderSegment& RightSegment = Formatting.m_Segment[CTableCellFormatting::SEGMENT_Right];
	const CTableBorderSegment& LeftSegment = Formatting.m_Segment[CTableCellFormatting::SEGMENT_Left];

	CPoint cpLeftCell(m_crSelect.left, m_crSelect.top);
	CPoint cpRightCell(m_crSelect.right, m_crSelect.top);

	// Prime it with the first cells.
	GetLeftBorder(cpLeftCell)->ReplaceWith(LeftSegment);
	GetRightBorder(cpRightCell)->ReplaceWith(RightSegment);

	// Merge in the left/right formatting for the rest of the cells in the selection.
	while (++cpRightCell.y, ++cpLeftCell.y <= m_crSelect.bottom)
	{
		GetLeftBorder(cpLeftCell)->ReplaceWith(LeftSegment);
		GetRightBorder(cpRightCell)->ReplaceWith(RightSegment);
	}

	if (m_crSelect.left != m_crSelect.right)
	{
		// We need to handle the inner vertical segments.
		const CTableBorderSegment& InnerSegment = Formatting.m_Segment[CTableCellFormatting::SEGMENT_InnerVertical];

		CPoint cpCell;
		for (cpCell.y = m_crSelect.top; cpCell.y <= m_crSelect.bottom; cpCell.y++)
		{
			for (cpCell.x = m_crSelect.left; cpCell.x < m_crSelect.right; cpCell.x++)
			{
				GetRightBorder(cpCell)->ReplaceWith(InnerSegment);
			}
		}
	}

	// Set the top and bottom segments.
	const CTableBorderSegment& TopSegment = Formatting.m_Segment[CTableCellFormatting::SEGMENT_Top];
	const CTableBorderSegment& BottomSegment = Formatting.m_Segment[CTableCellFormatting::SEGMENT_Bottom];

	CPoint cpTopCell(m_crSelect.left, m_crSelect.top);
	CPoint cpBottomCell(m_crSelect.left, m_crSelect.bottom);

	// Prime it with the first cells.
	GetTopBorder(cpTopCell)->ReplaceWith(TopSegment);
	GetBottomBorder(cpBottomCell)->ReplaceWith(BottomSegment);

	// Merge in the top/bottom formatting for the rest of the cells in the selection.
	while (++cpBottomCell.x, ++cpTopCell.x <= m_crSelect.right)
	{
		GetTopBorder(cpTopCell)->ReplaceWith(TopSegment);
		GetBottomBorder(cpBottomCell)->ReplaceWith(BottomSegment);
	}

	if (m_crSelect.top != m_crSelect.bottom)
	{
		// We need to handle the inner horizontal segments.
		const CTableBorderSegment& InnerSegment = Formatting.m_Segment[CTableCellFormatting::SEGMENT_InnerHorizontal];

		CPoint cpCell;
		for (cpCell.x = m_crSelect.left; cpCell.x <= m_crSelect.right; cpCell.x++)
		{
			for (cpCell.y = m_crSelect.top; cpCell.y < m_crSelect.bottom; cpCell.y++)
			{
				GetBottomBorder(cpCell)->ReplaceWith(InnerSegment);
			}
		}
	}

	// Set all the cell colors.
	COLOR Color = Formatting.m_FillColor;
	CPoint cpCell;
	for (cpCell.y = m_crSelect.top; cpCell.y <= m_crSelect.bottom; cpCell.y++)
	{
		for (cpCell.x = m_crSelect.left; cpCell.x <= m_crSelect.right; cpCell.x++)
		{
			CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
			if (!pCellInfo->IsSpanned())
			{
				ReplaceColor(pCellInfo->m_Color, Color);
			}
		}
	}
}

// Routines to save and restore cell formatting (undo-related).
BOOL CTableObject::SaveCellFormatting(CTableCellFormattingSave* pFormatting, LPCRECT pcrArea /*=NULL*/)
{
	// If an area is not passed in, use the selection.
	CRect crArea;
	if (pcrArea == NULL)
	{
		// We need to have something selected.
		if (!SomethingSelected())
		{
			ASSERT(FALSE);
			return FALSE;
		}

		// Remember the selection rectangle.
		crArea = m_crSelect;
	}
	else
	{
		// Use what has been passed.
		crArea = *pcrArea;
	}

	pFormatting->m_crArea = crArea;

	// Save all the elements.
	CPoint cpCell;
	for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom+1; cpCell.y++)
	{
		for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
		{
			pFormatting->m_HBorders.Add(GetTopBorder(cpCell));
			if (cpCell.y <= crArea.bottom)
			{
				pFormatting->m_CellInfo.Add(GetCellInfo(cpCell));
			}
		}
	}

	for (cpCell.x = crArea.left; cpCell.x <= crArea.right+1; cpCell.x++)
	{
		for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
		{
			pFormatting->m_VBorders.Add(GetLeftBorder(cpCell));
		}
	}
	return TRUE;
}

void CTableObject::RestoreCellFormatting(CTableCellFormattingSave& Formatting, BOOL fHandleHidden /*=FALSE*/)
{
	int nHIndex = 0;
	int nVIndex = 0;
	int nCellIndex = 0;

	// Restore all the elements.
	CPoint cpCell;
	for (cpCell.y = Formatting.m_crArea.top; cpCell.y <= Formatting.m_crArea.bottom+1; cpCell.y++)
	{
		for (cpCell.x = Formatting.m_crArea.left; cpCell.x <= Formatting.m_crArea.right; cpCell.x++)
		{
			*GetTopBorder(cpCell) = *Formatting.m_HBorders.GetAt(nHIndex++);
			if (cpCell.y <= Formatting.m_crArea.bottom)
			{
				CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
				*pCellInfo = *Formatting.m_CellInfo.GetAt(nCellIndex++);
				if (fHandleHidden)
				{
					CCellObject* pCell = GetCellAt(cpCell);
					if (pCellInfo->IsSpanned())
					{
						pCell->add_flags(OBJECT_FLAG_hidden);
					}
					else
					{
						pCell->remove_flags(OBJECT_FLAG_hidden);
					}
				}
			}
		}
	}

	// Restore the vertical borders.
	for (cpCell.x = Formatting.m_crArea.left; cpCell.x <= Formatting.m_crArea.right+1; cpCell.x++)
	{
		for (cpCell.y = Formatting.m_crArea.top; cpCell.y <= Formatting.m_crArea.bottom; cpCell.y++)
		{
			*GetLeftBorder(cpCell) = *Formatting.m_VBorders.GetAt(nVIndex++);
		}
	}
}

//
// Routine to set table formatting (auto-format related).
//
void CTableObject::SetTableFormatting(CTableFormatting& Formatting)
{
	// Set all the cell formatting.
	RestoreCellFormatting(Formatting);
	// Set the cell text formatting.
	SetCellAlignmentFormatting(Formatting);
	SetCellFontFormatting(Formatting);
	ReflowCellFrames(Formatting.m_crArea);
}

//
// Set the cell alignment formatting.
//

void CTableObject::SetCellAlignmentFormatting(CTableFormatting& Formatting)
{
	// Set all the elements.
	int nIndex = 0;
	CPoint cpCell;
	for (cpCell.y = Formatting.m_crArea.top; cpCell.y <= Formatting.m_crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = Formatting.m_crArea.left; cpCell.x <= Formatting.m_crArea.right; cpCell.x++)
		{
			CTableCellAlignment& CellAlignment = *Formatting.m_CellAlignments.GetAt(nIndex++);
			CFrameObject* pFrame = GetCellFrame(cpCell);
			if (pFrame != NULL)
			{
				if (CellAlignment.m_nHAlign != -1)
				{
					// We have something to set.
					CTextStyle Style(get_database());
					Style.MakeParagraphUndefined();
					Style.Alignment(CellAlignment.m_nHAlign);
					pFrame->SetParagraphStyle(&Style);
				}

				if (CellAlignment.m_nVAlign != -1)
				{
					CFrameRecord* pFrameRecord = pFrame->LockFrameRecord();
					if (pFrameRecord != NULL)
					{
						pFrameRecord->VerticalAlignment(CellAlignment.m_nVAlign);
						pFrameRecord->release();
					}
				}
			}
		}
	}
}

//
// Set the cell font formatting.
//

void CTableObject::SetCellFontFormatting(CTableFormatting& Formatting)
{
	// Set all the elements.
	int nIndex = 0;
	CPoint cpCell;
	for (cpCell.y = Formatting.m_crArea.top; cpCell.y <= Formatting.m_crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = Formatting.m_crArea.left; cpCell.x <= Formatting.m_crArea.right; cpCell.x++)
		{
			CTableCellFont& CellFont = *Formatting.m_CellFonts.GetAt(nIndex++);
			CFrameObject* pFrame = GetCellFrame(cpCell);
			if (pFrame != NULL)
			{			
				CTextStyle Style(get_database());
				Style.MakeTextUndefined();
				if (CellFont.m_fBold != -1)
				{
					Style.Bold(CellFont.m_fBold);
				}
				if (CellFont.m_fItalic != -1)
				{
					Style.Italic(CellFont.m_fItalic);
				}
				if (CellFont.m_Color != UNDEFINED_COLOR)
				{
					Style.FillForegroundColor(CellFont.m_Color);
					Style.OutlineForegroundColor(CellFont.m_Color);
				}

				pFrame->SetTextStyle(&Style);
			}
		}
	}
}

//
// Reflow all the cell frames.
//

void CTableObject::ReflowCellFrames(CRect crArea)
{
	CPoint cpCell;
	for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
		{
			CFrameObject* pFrame = GetCellFrame(cpCell);
			if (pFrame != NULL)
			{
				// Reflow the text to accomodate the new style.
				CTextFlow TextFlow(pFrame, 0, -1);
				TextFlow.FlowText();
			}
		}
	}
}

// Routines to save and restore cells (undo-related).
BOOL CTableObject::SaveCells(CTableCellSave* pCellSave)
{
	// Save all the cell formatting.
	if (SaveCellFormatting(pCellSave))
	{
		// Now, save pointers to all the contents of the cells.
		// Some of the pointers may be NULL if the cell is empty.

		CPoint cpCell;
		for (cpCell.y = pCellSave->m_crArea.top; cpCell.y <= pCellSave->m_crArea.bottom; cpCell.y++)
		{
			for (cpCell.x = pCellSave->m_crArea.left; cpCell.x <= pCellSave->m_crArea.right; cpCell.x++)
			{
				void* pObject = NULL;
				CCellObject* pCell = GetCellAt(cpCell);
				if (pCell != NULL)
				{
					pObject = pCell->object_list()->first_object();
				}
				pCellSave->m_cpaCellContents.Add(pObject);
			}
		}
		return TRUE;
	}
	return FALSE;
}

//
// Restore the content of cells.
// The old contents are NOT deleted. You must pre-delete them if they
// need to be deleted. This is due to the way undo works (where object
// pointers need to be kept around for undo/redo purposes).
// 

void CTableObject::RestoreCells(CTableCellSave& CellSave)
{
	// Restore the formatting for the cells.
	RestoreCellFormatting(CellSave);

	// Now, restore all the contents of the cells.
	// Some of the pointers may be NULL if the cell was empty.

	int nCell = 0;
	CPoint cpCell;
	for (cpCell.y = CellSave.m_crArea.top; cpCell.y <= CellSave.m_crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = CellSave.m_crArea.left; cpCell.x <= CellSave.m_crArea.right; cpCell.x++, nCell++)
		{
			// Get this cell object.
			CCellObject* pCell = GetCellAt(cpCell);
			if (pCell != NULL)
			{
				// Handle the hidden-ness of the cell.
				CTableCellInfo* pCellInfo = CellSave.m_CellInfo.GetAt(nCell);
				if (pCellInfo->IsSpanned())
				{
					pCell->add_flags(OBJECT_FLAG_hidden);
				}
				else
				{
					pCell->remove_flags(OBJECT_FLAG_hidden);
				}

				// Handle the cell contents.
				ObjectList* pList = pCell->object_list();

				// Detach all the current cell content objects.
				while (pList->count() > 0)
				{
					PMGPageObject* pObject = (PMGPageObject*)pList->first_object();
					ASSERT(pObject != NULL);
					pList->detach(pObject);
					pObject->OnPage(FALSE);		// No longer on the page.
				}

				// Append ours if we have one.
				PMGPageObject* pObject = (PMGPageObject*)CellSave.m_cpaCellContents.GetAt(nCell);
				if (pObject != NULL)
				{
					pList->append(pObject);
					pObject->OnPage(TRUE);		// Now on the page.
				}
			}
		}
	}

	// Layout the cells in case they changed.
	LayoutCells(&CellSave.m_crArea);
}

//
// Exchange the cells in the table with what is in the save state.
//
void CTableObject::SwapCells(CTableCellSave& CellSave)
{
	// Restore the formatting for the cells.
	RestoreCellFormatting(CellSave);

	// Now, restore all the contents of the cells.
	// Some of the pointers may be NULL if the cell was empty.

	int nCell = 0;
	CPoint cpCell;
	for (cpCell.y = CellSave.m_crArea.top; cpCell.y <= CellSave.m_crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = CellSave.m_crArea.left; cpCell.x <= CellSave.m_crArea.right; cpCell.x++, nCell++)
		{
			// Get this cell object.
			CCellObject* pCell = GetCellAt(cpCell);
			if (pCell != NULL)
			{
				// Handle the hidden-ness of the cell.
				CTableCellInfo* pCellInfo = CellSave.m_CellInfo.GetAt(nCell);
				if (pCellInfo->IsSpanned())
				{
					pCell->add_flags(OBJECT_FLAG_hidden);
				}
				else
				{
					pCell->remove_flags(OBJECT_FLAG_hidden);
				}

				// Handle the cell contents.
				ObjectList* pList = pCell->object_list();

				// Detach all the current cell content objects.
				ASSERT(pList->count() <= 1);
				PMGPageObject* pOldObject = (PMGPageObject*)pList->first_object();
				if (pOldObject != NULL)
				{
					pList->detach(pOldObject);
					pOldObject->OnPage(FALSE);
				}

				// Append ours if we have one.
				PMGPageObject* pObject = (PMGPageObject*)CellSave.m_cpaCellContents.GetAt(nCell);
				if (pObject != NULL)
				{
					pList->append(pObject);
					pObject->OnPage(TRUE);		// Now on the page.
					pCell->establish(FALSE);
				}

				// Save the old object.
				CellSave.m_cpaCellContents.SetAt(nCell, pOldObject);
			}
		}
	}

	// Layout the cells in case they changed.
	LayoutCells(&CellSave.m_crArea);
}

CTableBorderSegment* CTableObject::GetLeftBorder(CPoint cpCell)
{
	return GetVBorder(cpCell.x*GetRows()+cpCell.y);
}
CTableBorderSegment* CTableObject::GetRightBorder(CPoint cpCell)
{
	return GetVBorder((cpCell.x+1)*GetRows()+cpCell.y);
}
CTableBorderSegment* CTableObject::GetTopBorder(CPoint cpCell)
{
	return GetHBorder(cpCell.y*GetColumns()+cpCell.x);
}
CTableBorderSegment* CTableObject::GetBottomBorder(CPoint cpCell)
{
	return GetHBorder((cpCell.y+1)*GetColumns()+cpCell.x);
}

CTableCellInfo* CTableObject::GetCellInfo(CPoint cpCell)
{
	return GetCellInfo(cpCell.y*GetColumns()+cpCell.x);
}

// Recalc after a size or movement.
void CTableObject::calc(PBOX_PTR panel_world /*=NULL*/, FLAGS panel_flags /*=0*/)
{
	RectPageObject::calc(panel_world, panel_flags);
	LayoutCells();
}

/*
// Assign an object to this.
*/

ERRORCODE CTableObject::assign(PageObjectRef sobject)
{
	ERRORCODE error;
	CTableObject& tobject = (CTableObject&)sobject;

	// Assign the base object.
	if ((error = GroupObject::assign(sobject)) == ERRORCODE_None)
	{
		// Copy the record.
		m_TRecord = tobject.m_TRecord;
		m_RowHeights = tobject.m_RowHeights;
		m_ColumnWidths = tobject.m_ColumnWidths;
		m_DesiredRowHeights = tobject.m_DesiredRowHeights;
		m_CellInfo = tobject.m_CellInfo;
		m_HBorders = tobject.m_HBorders;
		m_VBorders = tobject.m_VBorders;

		CalculateTable();
	}
	return error;
}

/*
// Return the original dimensions of an object.
*/

BOOL CTableObject::original_dims(PPNT_PTR p)
{
	return FALSE;
}

// Return the refresh bounds for this object.
void CTableObject::get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc /*=NULL*/)
{
	RectPageObject::get_refresh_bound(refresh_bound, rc);
//	INHERITED::get_refresh_bound(refresh_bound, rc);

	refresh_bound->x0 -= PAGE_RESOLUTION/10;
	refresh_bound->y0 -= PAGE_RESOLUTION/10;
	refresh_bound->x1 += PAGE_RESOLUTION/10;
	refresh_bound->y1 += PAGE_RESOLUTION/10;
}

// Create a state for saving.
// We return a pointer to a new object. This object needs to be freed by
// the caller at some point.
PageObjectState* CTableObject::CreateState(void)
{
	return new CTableObjectState;
}

// Save our state.
void CTableObject::SaveState(PageObjectState* pState)
{
	INHERITED::SaveState(pState);
	CTableObjectState* pTableState = (CTableObjectState*)pState;
	pTableState->m_TRecord = m_TRecord;
	pTableState->m_DesiredRowHeights = m_DesiredRowHeights;
	pTableState->m_RowHeights = m_RowHeights;
	pTableState->m_ColumnWidths = m_ColumnWidths;
}

// Restore our state.
// The state passed in is NOT deleted by us. It is still the responsibility
// of the caller (or whomever).
void CTableObject::RestoreState(PageObjectState* pState)
{
	INHERITED::RestoreState(pState);
	CTableObjectState* pTableState = (CTableObjectState*)pState;
	m_TRecord = pTableState->m_TRecord;
	m_DesiredRowHeights = pTableState->m_DesiredRowHeights;
	m_RowHeights = pTableState->m_RowHeights;
	m_ColumnWidths = pTableState->m_ColumnWidths;

	calc();
}

/*
// We are being selected or deselected.
//
// This is mostly a notification. You should be careful if you use this to set 
// a state since the state could change without this function being called
// (e.g. if the document is saved while an object is selected).
//
// This function will be called with TRUE before the object has been toggled
// on, and it will called with FALSE before the object is toggled off.
*/

void CTableObject::OnSelectChange(BOOL fSelected /*=TRUE*/)
{
	DeselectAll(!fSelected);
}

//
// Get a region corresponding to an area of the table.
//

BOOL CTableObject::GetSelectPoints(PPNT v[], CRect crToggle, BOOL fAddBorderWidths /*=FALSE*/)
{
	// Get the unrotated bound.
	PBOX Bound = get_unrotated_bound();
	int nRows = GetRows();
	int nColumns = GetColumns();

	// Make sure we are oriented correctly.
	crToggle.NormalizeRect();

	// Make sure we have something to do.
	if (crToggle.right < 0
			|| crToggle.bottom < 0
			|| crToggle.left >= nColumns
			|| crToggle.top >= nRows)
	{
		// Totally off the table.
		return FALSE;
	}

	int nRight = GetColumns()-1;
	int nBottom = GetRows()-1;

	// Clip to the table extent.
	if (crToggle.left < 0)
	{
		crToggle.left = 0;
	}

	if (crToggle.right > nRight)
	{
		crToggle.right = nRight;
	}

	if (crToggle.top < 0)
 	{
		crToggle.top = 0;
	}

	if (crToggle.bottom > nBottom)
	{
		crToggle.bottom = nBottom;
	}

	if (get_flags() & OBJECT_FLAG_xflipped)
	{
		int t = nRight - crToggle.left;
		crToggle.left = nRight - crToggle.right;
		crToggle.right = t;
	}

	if (get_flags() & OBJECT_FLAG_yflipped)
	{
		int t = nBottom - crToggle.top;
		crToggle.top = nBottom - crToggle.bottom;
		crToggle.bottom = t;
	}

	ASSERT(crToggle.left <= crToggle.right);
	ASSERT(crToggle.top <= crToggle.bottom);

	// Construct the unrotated select bound.
	PBOX SelectBound;

	SelectBound.x0 = Bound.x0;
	int nColumn = 0;
	while (nColumn < crToggle.left)
	{
		SelectBound.x0 += GetColumnWidth(nColumn++);
	}
	SelectBound.x1 = SelectBound.x0;
	while (nColumn <= crToggle.right)
	{
		SelectBound.x1 += GetColumnWidth(nColumn++);
	}

	SelectBound.y0 = Bound.y0;
	int nRow = 0;
	while (nRow < crToggle.top)
	{
		SelectBound.y0 += GetRowHeight(nRow++);
	}
	SelectBound.y1 = SelectBound.y0;
	while (nRow <= crToggle.bottom)
	{
		SelectBound.y1 += GetRowHeight(nRow++);
	}

	if (fAddBorderWidths)
	{
		// We want to expand the bound to include the widths of the borders.
		int n;

		// Compute the points on the left and right.
		int nLeftBorder = 0;
		int nRightBorder = 0;
		CTableBorderSegment* pLeftBorder = GetLeftBorder(CPoint(crToggle.left, crToggle.top));
		CTableBorderSegment* pRightBorder = GetRightBorder(CPoint(crToggle.right, crToggle.top));
		for (n = crToggle.top; n <= crToggle.bottom; n++)
		{
			if (pLeftBorder->m_bWidth > nLeftBorder)
			{
				nLeftBorder = pLeftBorder->m_bWidth;
			}
			if (pRightBorder->m_bWidth > nRightBorder)
			{
				nRightBorder = pRightBorder->m_bWidth;
			}
			pLeftBorder++;
			pRightBorder++;
		}

		// Compute the points on the top and bottom.
		int nTopBorder = 0;
		int nBottomBorder = 0;
		CTableBorderSegment* pTopBorder = GetTopBorder(CPoint(crToggle.left, crToggle.top));
		CTableBorderSegment* pBottomBorder = GetBottomBorder(CPoint(crToggle.left, crToggle.bottom));
		for (n = crToggle.left; n <= crToggle.right; n++)
		{
			if (pTopBorder->m_bWidth > nTopBorder)
			{
				nTopBorder = pTopBorder->m_bWidth;
			}
			if (pBottomBorder->m_bWidth > nBottomBorder)
			{
				nBottomBorder = pBottomBorder->m_bWidth;
			}
			pTopBorder++;
			pBottomBorder++;
		}
		// Expand the bound.
		SelectBound.x0 -= scale_number(nLeftBorder, PAGE_RESOLUTION, 72);
		SelectBound.y0 -= scale_number(nTopBorder, PAGE_RESOLUTION, 72);
		SelectBound.x1 += scale_number(nRightBorder, PAGE_RESOLUTION, 72);
		SelectBound.y1 += scale_number(nBottomBorder, PAGE_RESOLUTION, 72);
	}

	v[0].x = SelectBound.x0;
	v[0].y = SelectBound.y0;
	v[1].x = SelectBound.x1;
	v[1].y = SelectBound.y0;
	v[2].x = SelectBound.x1;
	v[2].y = SelectBound.y1;
	v[3].x = SelectBound.x0;
	v[3].y = SelectBound.y1;

	double dAngle = get_rotation();
	if (dAngle != 0.0)
	{
		rotate_points(v, 4, get_origin(), dAngle);
	}
	return TRUE;
}

BOOL CTableObject::GetSelectRegion(CRgn& rgnSelect, CRect crToggle, RedisplayContext* rc)
{
	PPNT v[4];

	if (GetSelectPoints(v, crToggle))
	{
		CPoint cpVertices[5];
		for (int i = 0; i < 4; i++)
		{
			cpVertices[i].x = rc->page_x_to_screen(v[i].x);
			cpVertices[i].y = rc->page_y_to_screen(v[i].y);
		}
		// Close it.
		cpVertices[4] = cpVertices[0];
		rgnSelect.CreatePolygonRgn(cpVertices, 5, ALTERNATE);
		return TRUE;
	}
	return FALSE;
}

void CTableObject::ToggleSquare(RedisplayContext* rc, CRect crToggle)
{
	CRgn rgnToggle;
	if (GetSelectRegion(rgnToggle, crToggle, rc))
	{
		// Invert this region.
		::InvertRgn(rc->destination_hdc, rgnToggle);
	}
}

void CTableObject::SelectRows(int nStart, int nEnd)
{
	CRect crSelect(0, nStart, GetColumns()-1, nEnd);
	crSelect.NormalizeRect();
	SetSelect(crSelect);
}

void CTableObject::SelectColumns(int nStart, int nEnd)
{
	CRect crSelect(nStart, 0, nEnd, GetRows()-1);
	crSelect.NormalizeRect();
	SetSelect(crSelect);
}

void CTableObject::ToggleBounds(RedisplayContext* rc, POINT* p)
{
	INHERITED::ToggleBounds(rc, p);

	HDC hdc = rc->destination_hdc;

	HPEN hPen = ::CreatePen(PS_DOT, 1, SELECT_COLOR);
	if (hPen != NULL)
	{
		HPEN hOldPen = (HPEN)::SelectObject(hdc, hPen);
		if (hOldPen != NULL)
		{
			int nColumns = GetColumns();
			int nRows = GetRows();

			int* nColumnX;
			int* nRowY;
			GetRowAndColumnPositions(nColumnX, nRowY, rc);
			double dAngle = get_rotation();

			// Compute the center for the rotated case.
			CPoint cpCenter;
			cpCenter.x = rc->page_x_to_screen(get_origin().x);
			cpCenter.y = rc->page_y_to_screen(get_origin().y);

			// Draw the column lines.
			CPoint cpCell;
			for (cpCell.x = 1; cpCell.x < nColumns; cpCell.x++)
			{
				int x = nColumnX[cpCell.x];
				// Run through all the edges in this column and draw those that are
				// not spanned. To start, get the first segment in the column. The
				// rest follow sequentially.
				CTableBorderSegment* pSegment = GetLeftBorder(CPoint(cpCell.x, 0));
				for (cpCell.y = 0; cpCell.y < nRows; cpCell.y++, pSegment++)
				{
					// Find the next non-spanned cell.
					if (!pSegment->IsSpanned())
					{
						// We can start drawing!
						int nStartY = cpCell.y;
						while (cpCell.y < nRows-1)
						{
							if ((pSegment+1)->IsSpanned())
							{
								break;
							}
							cpCell.y++;
							pSegment++;
						}
						// Draw the segment we found.
						POINT p[2];
						p[0].x = x;
						p[0].y = nRowY[nStartY];
						p[1].x = x;
						p[1].y = nRowY[cpCell.y+1];

						if (dAngle != 0.0)
						{
							RotatePoints(p, 2, cpCenter, dAngle);
						}
						::MoveToEx(hdc, p[0].x, p[0].y, NULL);
						::LineTo(hdc, p[1].x, p[1].y);
					}
				}
			}
			// Draw the row lines.
			for (cpCell.y = 1; cpCell.y < nRows; cpCell.y++)
			{
				int y = nRowY[cpCell.y];
				// Run through all the edges in this row and draw those that are
				// not spanned. To start, get the first segment in the row. The
				// rest follow sequentially.
				CTableBorderSegment* pSegment = GetTopBorder(CPoint(0, cpCell.y));
				for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++, pSegment++)
				{
					// Find the next non-spanned cell.
					if (!pSegment->IsSpanned())
					{
						// We can start drawing!
						int nStartX = cpCell.x;
						while (cpCell.x < nColumns-1)
						{
							if ((pSegment+1)->IsSpanned())
							{
								break;
							}
							pSegment++;
							cpCell.x++;
						}
						// Draw the segment we found.
						POINT p[2];
						p[0].x = nColumnX[nStartX];
						p[0].y = y;
						p[1].x = nColumnX[cpCell.x+1];
						p[1].y = y;

						if (dAngle != 0.0)
						{
							RotatePoints(p, 2, cpCenter, dAngle);
						}
						::MoveToEx(hdc, p[0].x, p[0].y, NULL);
						::LineTo(hdc, p[1].x, p[1].y);
					}
				}
			}

			delete [] nRowY;
			delete [] nColumnX;

			::SelectObject(hdc, hOldPen);
		}
		::DeleteObject(hPen);
	}
}

static void ComputeEnclosingBound(const POINT* pPoints, int nPoints, CRect& crBound)
{
	if (nPoints == 0)
	{
		crBound.SetRectEmpty();
	}
	else
	{
		crBound.left = crBound.right = pPoints[0].x;
		crBound.top = crBound.bottom = pPoints[0].y;
		pPoints++;

		for (int i = 1; i < nPoints; i++)
		{
			POINT p = *pPoints++;
			if (crBound.left > p.x) crBound.left = p.x;
			if (crBound.right < p.x) crBound.right = p.x;
			if (crBound.top > p.y) crBound.top = p.y;
			if (crBound.bottom < p.y) crBound.bottom = p.y;
		}
	}
}

static void FillTableRectangle(RedisplayContext* rc, CPoint* cp, HBRUSH hBrush, double dAngle, CPoint cpCenter)
{
	HDC hDC = rc->destination_hdc;
	if (dAngle == 0.0)
	{
		// Fill as a rectangle.
		CRect crFill(cp[0], cp[1]);

		rc->toggle(FALSE, &crFill);
		::FillRect(hDC, crFill, hBrush);
		rc->toggle(TRUE, &crFill);
	}
	else
	{
		// We need to fill the rotated block.
		// Use a polygon.
		CPoint cpPoly[4];
		cpPoly[0] = cp[0];
		cpPoly[1].x = cp[1].x;
		cpPoly[1].y = cp[0].y;
		cpPoly[2] = cp[1];
		cpPoly[3].x = cp[0].x;
		cpPoly[3].y = cp[1].y;
		RotatePoints(cpPoly, 4, cpCenter, dAngle);
		HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);
		if (hOldBrush != NULL)
		{
			HPEN hNullPen = (HPEN)::GetStockObject(NULL_PEN);	// No outer edge.
			HPEN hOldPen = (HPEN)::SelectObject(hDC, hNullPen);
			if (hOldPen != NULL)
			{
				CRect crToggle;
				ComputeEnclosingBound(cpPoly, 4, crToggle);
				rc->toggle(FALSE, &crToggle);
				::Polygon(hDC, cpPoly, 4);
				rc->toggle(TRUE, &crToggle);
				::SelectObject(hDC, hOldPen);
			}
			::SelectObject(hDC, hOldBrush);
		}
	}
}

// Draw the table grid.

void CTableObject::DrawGrid(RedisplayContextPtr rc)
{
	if (get_select_flags() & SELECT_FLAG_boundary)
	{
      HDC hDC = rc->destination_hdc;
		double dAngle = get_rotation();

		int nColumns = GetColumns();
		int nRows = GetRows();
		int nColumn, nRow;

		// Compute the center for the rotated case.
		CPoint cpCenter;
		cpCenter.x = rc->page_x_to_screen(get_origin().x);
		cpCenter.y = rc->page_y_to_screen(get_origin().y);

		//
		// Compute destination (unrotated) coordinates for the rows and columns.
		//

		int* nColumnX;
		int* nRowY;
		GetRowAndColumnPositions(nColumnX, nRowY, rc);

		// Loop through all the cell info, and draw the cell backgrounds.
		CTableCellInfo* pCellInfo = GetCellInfo(0);
		for (nRow = 0; nRow < nRows; nRow++)
		{
			for (nColumn = 0; nColumn < nColumns; nColumn++)
			{
				CBrush cbBrush;
				if (!pCellInfo->IsSpanned() && pCellInfo->SetupToDraw(cbBrush))
				{
					// We have something to draw.
					// Compute the points to draw.
					CPoint cp[2];
					cp[0].x = nColumnX[nColumn];
					cp[0].y = nRowY[nRow];
					cp[1].x = nColumnX[nColumn+pCellInfo->m_nSpanX];
					cp[1].y = nRowY[nRow+pCellInfo->m_nSpanY];

					FillTableRectangle(rc, cp, cbBrush, dAngle, cpCenter);
				}
				pCellInfo++;
			}
		}

		// Loop through the horizontal pieces.
		// We can grab the first segment and access the rest sequentially.
		CTableBorderSegment* pSegment = m_HBorders.GetAt(0);
		for (nRow = 0; nRow <= nRows; nRow++)
		{
			// Compute the next run to draw.
			for (int nEndColumn = 0; nEndColumn < nColumns;)
			{
				// Figure out how many columns have the same style.
				CTableBorderSegment* pStartSegment = pSegment;
				int nStartColumn = nEndColumn;

				while (nEndColumn < nColumns-1)
				{
					nEndColumn++;
					pSegment++;
					if (*pSegment != *pStartSegment)
					{
						// Back off the non-matching one.
						nEndColumn--;
						pSegment--;
						break;
					}
				}
				// Now, draw the column(s) we accumulated.
				CPoint cp[2];
				cp[0].x = nColumnX[nStartColumn];
				cp[1].x = nColumnX[nEndColumn+1];
				cp[0].y = cp[1].y = nRowY[nRow];

				// Construct a pen to use.
				CPen cpEdgePen;
				CBrush cbEdgeBrush;
				int nWidth;
				if (pStartSegment->SetupToDraw(rc, cpEdgePen, cbEdgeBrush, nWidth))
				{
					if (((HBRUSH)cbEdgeBrush) != NULL)
					{
						// We have a brush to draw with.
						ASSERT(nWidth >= 2);
						cp[0].y -= nWidth/2;
						cp[1].y = cp[0].y + nWidth;

						// For a thick line, we need to possibly expand it to make
						// it fit with the perpendicular segments.
						// Strategy: There are two possible strategies (I can
						// currently think of) - choose the maximum width of
						// the perpendicular edges, or choose the minimum width of
						// the perpendicaular edges. I chose the former.

						int nLeftMax = 0;
						int nRightMax = 0;

						if (nRow > 0)
						{
							// Grab the top edge to the left.
							int nLeftIndex = nStartColumn*nRows + (nRow-1);
							// Grab the top edge to the right.
							int nRightIndex = (nEndColumn+1)*nRows + (nRow-1);

							nLeftMax = m_VBorders.GetAt(nLeftIndex)->GetWidth(rc);
							nRightMax = m_VBorders.GetAt(nRightIndex)->GetWidth(rc);
						}
						if (nRow < nRows)
						{
							// Grab the bottom edge to the left.
							int nLeftIndex = nStartColumn*nRows+ nRow;
							// Grab the bottom edge to the right.
							int nRightIndex = (nEndColumn+1)*nRows + nRow;

							int n = m_VBorders.GetAt(nLeftIndex)->GetWidth(rc);
							if (n > nLeftMax) nLeftMax = n;
							n = m_VBorders.GetAt(nRightIndex)->GetWidth(rc);
							if (n > nRightMax) nRightMax = n;
						}

						// Adjust the ends.
						cp[0].x -= nLeftMax/2;
						cp[1].x += nRightMax/2;

						FillTableRectangle(rc, cp, cbEdgeBrush, dAngle, cpCenter);
					}
					else if (((HPEN)cpEdgePen) != NULL)
					{
						// We have a pen. Draw.
						HPEN hOldPen = (HPEN)::SelectObject(hDC, cpEdgePen);
						if (hOldPen != NULL)
						{
							if (dAngle != 0.0)
							{
								RotatePoints(cp, 2, cpCenter, dAngle);
							}
							CRect crToggle;
							ComputeEnclosingBound(cp, 2, crToggle);
							crToggle.InflateRect(1, 1);
							rc->toggle(FALSE, &crToggle);
							::MoveToEx(hDC, cp[0].x, cp[0].y, NULL);
							::LineTo(hDC, cp[1].x, cp[1].y);
							rc->toggle(TRUE, &crToggle);
							::SelectObject(hDC, hOldPen);
						}
					}
				}

				// Next segment.
				nEndColumn++;
				pSegment++;
			}
		}

		// Loop through the vertical pieces.
		// We can grab the first segment and access the rest sequentially.
		pSegment = m_VBorders.GetAt(0);
		for (nColumn = 0; nColumn <= nColumns; nColumn++)
		{
			// Compute the next run to draw.
			for (int nEndRow = 0; nEndRow < nRows;)
			{
				// Figure out how many rows have the same style.
				CTableBorderSegment* pStartSegment = pSegment;
				int nStartRow = nEndRow;

				while (nEndRow < nRows-1)
				{
					nEndRow++;
					pSegment++;
					if (*pSegment != *pStartSegment)
					{
						// Back off the non-matching one.
						nEndRow--;
						pSegment--;
						break;
					}
				}
				// Now, draw the row(s) we accumulated.
				CPoint cp[2];
				cp[0].y = nRowY[nStartRow];
				cp[1].y = nRowY[nEndRow+1];
				cp[0].x = cp[1].x = nColumnX[nColumn];

				// Construct a pen to use.
				CPen cpEdgePen;
				CBrush cbEdgeBrush;
				int nWidth;
				if (pStartSegment->SetupToDraw(rc, cpEdgePen, cbEdgeBrush, nWidth))
				{
					if (((HBRUSH)cbEdgeBrush) != NULL)
					{
						// We have a brush to draw with.
						ASSERT(nWidth >= 2);
						cp[0].x -= nWidth/2;
						cp[1].x = cp[0].x + nWidth;

						// For a thick line, we need to possibly expand it to make
						// it fit with the perpendicular segments.
						// Strategy: There are two possible strategies (I can
						// currently think of) - choose the maximum width of
						// the perpendicular edges, or choose the minimum width of
						// the perpendicaular edges. I chose the former.

						int nTopMax = 0;
						int nBottomMax = 0;

						if (nColumn > 0)
						{
							// Grab the left edge to the top.
							int nTopIndex = nStartRow*nColumns + (nColumn-1);
							// Grab the left edge to the bottom.
							int nBottomIndex = (nEndRow+1)*nColumns + (nColumn-1);

							nTopMax = m_HBorders.GetAt(nTopIndex)->GetWidth(rc);
							nBottomMax = m_HBorders.GetAt(nBottomIndex)->GetWidth(rc);
						}
						if (nColumn < nColumns)
						{
							// Grab the right edge to the top.
							int nTopIndex = nStartRow*nColumns+ nColumn;
							// Grab the right edge to the bottom.
							int nBottomIndex = (nEndRow+1)*nColumns + nColumn;

							int n = m_HBorders.GetAt(nTopIndex)->GetWidth(rc);
							if (n > nTopMax) nTopMax = n;
							n = m_HBorders.GetAt(nBottomIndex)->GetWidth(rc);
							if (n > nBottomMax) nBottomMax = n;
						}

						// Adjust the ends.
						cp[0].y -= nTopMax/2;
						cp[1].y += nBottomMax/2;

						FillTableRectangle(rc, cp, cbEdgeBrush, dAngle, cpCenter);
					}
					else if (((HPEN)cpEdgePen) != NULL)
					{
						// We have a pen. Draw.
						HPEN hOldPen = (HPEN)::SelectObject(hDC, cpEdgePen);
						if (hOldPen != NULL)
						{
							if (dAngle != 0.0)
							{
								RotatePoints(cp, 2, cpCenter, dAngle);
							}
							CRect crToggle;
							ComputeEnclosingBound(cp, 2, crToggle);
							crToggle.InflateRect(1, 1);
							rc->toggle(FALSE, &crToggle);
							::MoveToEx(hDC, cp[0].x, cp[0].y, NULL);
							::LineTo(hDC, cp[1].x, cp[1].y);
							rc->toggle(TRUE, &crToggle);
							::SelectObject(hDC, hOldPen);
						}
					}
				}

				// Next segment.
				nEndRow++;
				pSegment++;
			}
		}

		delete [] nRowY;
		delete [] nColumnX;

#if 0
      // First, draw the frame rectangle. This is drawn one pixel outside
      // of the content area.

      // Get the unrotated bound and move the upper left corner out one
      // pixel. We don't have to move the lower right corner out because
      // Lineto() is used to draw the frame and it automatically will draw
      // to the right and below the computed coordinates (unlike Rectangle).

      PBOX UnrotatedBound = get_unrotated_bound();
      UnrotatedBound.x0 -= rc->redisplay_x_pixel;
      UnrotatedBound.y0 -= rc->redisplay_y_pixel;

      // Initialize v[] with the coordinates of the rotated corners.
      PBOX RotatedBound;
      PPNT v[4];
      compute_rotated_bound(UnrotatedBound, get_rotation(), &RotatedBound, get_origin(), v);

      // Convert the PPNT coordinates to screen coordinates in p[]. Along the way
      // we accumulate the bounding rectangle which will be used for the toggle.
      POINT p[4];
      RECT r;
      int i;

      for (i = 0; i < 4; i++)
      {
         // Convert point to screen coordinates.
         p[i].x = rc->page_x_to_screen(v[i].x);
         p[i].y = rc->page_y_to_screen(v[i].y);

         // Accumulate point into bounding rectangle.
         if (i == 0)
         {
            r.left = r.right = p[i].x;
            r.top = r.bottom = p[i].y;
         }
         else
         {
            if (p[i].x < r.left) r.left = p[i].x;
            if (p[i].x > r.right) r.right = p[i].x;
            if (p[i].y < r.top) r.top = p[i].y;
            if (p[i].y > r.bottom) r.bottom = p[i].y;
         }
      }

      // Bump out the lower right corner of the toggle rectangle since the frame
      // will be drawn with LineTo which draws to the right and below the computed
      // coordinates.
      r.right++;
      r.bottom++;

      // Draw the frame using a gray "dotted" pen.
      HPEN hPen = ::CreatePen(PS_DOT, 1, RGB(192, 192, 192));
      if (hPen != 0)
      {
         HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
         if (hOldPen != NULL)
         {
            // Erase any toggles.
            rc->toggle(FALSE, &r);

            // Draw opaque.
            int oldROP2 = ::SetROP2(hDC, R2_COPYPEN);
            int oldBkMode = ::SetBkMode(hDC, OPAQUE);

            // Draw the frame.
            ::MoveToEx(hDC, p[0].x, p[0].y, NULL);
            ::LineTo(hDC, p[1].x, p[1].y);
            ::LineTo(hDC, p[2].x, p[2].y);
            ::LineTo(hDC, p[3].x, p[3].y);
            ::LineTo(hDC, p[0].x, p[0].y);

#if 0
            // Draw the columns.
            CFrameRecord* pFrame;

            // Column information is in frame record.
            if ((pFrame = LockFrameRecord()) != NULL)
            {
               int nColumns;
               PCOORD ColumnSpacing;
               PCOORD ColumnWidth;

               // Get unrotated column positions.
               UnrotatedBound = get_unrotated_bound();
               pFrame->GetColumnData(&nColumns, &ColumnSpacing, &ColumnWidth, &UnrotatedBound);

               if (nColumns > 1)
               {
                  // For now, we just draw nColumns-1 lines.
                  // This is considerably less clutter than showing all
                  // the margins. Maybe if we use another color...

                  PCOORD ColumnLeft = UnrotatedBound.x0;
                  
                  // Draw each column.
                  for (int nColumn = 1; nColumn < nColumns; nColumn++)
                  {
                     ColumnLeft += ColumnWidth;

                     // Compute rotated coordinates of column marker.
                     v[0].x = ColumnLeft;
                     v[0].y = UnrotatedBound.y0;
                     v[1].x = ColumnLeft;
                     v[1].y = UnrotatedBound.y1;
                     rotate_points(v, 2, get_origin(), get_rotation());

                     // Convert to screen coordinates.
                     for (i = 0; i < 2; i++)
                     {
                        p[i].x = rc->page_x_to_screen(v[i].x);
                        p[i].y = rc->page_y_to_screen(v[i].y);
                     }
                     
                     // Draw the column marker.
                     ::MoveToEx(hDC, p[0].x, p[0].y, NULL);
                     ::LineTo(hDC, p[1].x, p[1].y);
                  }
               }

               // All done with the frame record.
               pFrame->release();
            }
#endif

            // Restore old DC settings.
            ::SetBkMode(hDC, oldBkMode);
            ::SetROP2(hDC, oldROP2);

            // Turn any toggles back on.
            rc->toggle(TRUE, &r);

            ::SelectObject(hDC, hOldPen);
         }

         ::DeleteObject(hPen);
		}
#endif
	}
}

void CTableObject::GetRowAndColumnPositions(int*& nColumnX, int*& nRowY, RedisplayContext* rc)
{
	int nColumns = GetColumns();
	int nRows = GetRows();

	nColumnX = new int[nColumns+1];
	nRowY = new int[nRows+1];

	PBOX Bound = get_unrotated_bound();		// Get the unrotated bound.
	FLAGS Flags = get_flags();
	BOOL fXFlipped = Flags & OBJECT_FLAG_xflipped;
	BOOL fYFlipped = Flags & OBJECT_FLAG_yflipped;

	// Compute unrotated x coordinates for the column edges.
	int nColumn;
	PCOORD x = fXFlipped ? Bound.x1 : Bound.x0;

	for (nColumn = 0; nColumn < nColumns; nColumn++)
	{
		nColumnX[nColumn] = rc->page_x_to_screen(x);
		PCOORD Width = GetColumnWidth(nColumn);
		if (fXFlipped) Width = -Width;
		x += Width;
	}
	nColumnX[nColumn] = rc->page_x_to_screen(x);

	// Compute unrotated y coordinates for the row edges.
	int nRow;
	PCOORD y = fYFlipped ? Bound.y1 : Bound.y0;

	for (nRow = 0; nRow < nRows; nRow++)
	{
		nRowY[nRow] = rc->page_y_to_screen(y);
		PCOORD Height = GetRowHeight(nRow);
		if (fYFlipped) Height = -Height;
		y += Height;
	}
	nRowY[nRow] = rc->page_y_to_screen(y);
}

//
// Get the text of a cell.
//

CFrameObject* CTableObject::GetCellFrame(CPoint cpCell, BOOL fHonorMerging /*=TRUE*/)
{
	CFrameObject* pFrame = NULL;
	CCellObject* pCell = GetCellAt(cpCell);
	if (pCell == NULL)
	{
		// Could not find the requested cell.
		ASSERT(FALSE);
	}
	else
	{
		// If we want to honor merging, do it now.
		if (fHonorMerging)
		{
			CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
			if (pCellInfo->IsSpanned())
			{
				cpCell.x += pCellInfo->m_nSpanX;
				cpCell.y += pCellInfo->m_nSpanY;
				pCell = GetCellAt(cpCell);
				ASSERT(!GetCellInfo(cpCell)->IsSpanned());
				if (pCell == NULL)
				{
					ASSERT(FALSE);
					return NULL;
				}
			}
		}

		// Get the child of this cell.
		ObjectList* pList = pCell->object_list();
		PMGPageObject* pObject = (PMGPageObject*)pList->first_object();
		if (pObject == NULL)
		{
			// We do not have an object yet.
			// Create one!
			PBOX Bound = pCell->get_unrotated_bound();
			SHORT nFactor = 3;		// arbitrary factor.
			SHORT nSize = (SHORT)scale_pcoord(Bound.y1-Bound.y0,
										(72*3)/4, nFactor*PAGE_RESOLUTION);

			CTextStyle Style(get_database(), TRUE);

			Style.SetDefault();
			Style.Size(MakeFixed(nSize));
			Style.BaseSize(MakeFixed(nSize));
			Style.UpdateFontMetrics();

			if (AddFrame(&Bound,
							 NULL,
							 ALIGN_left,
							 ALIGN_top,
							 OBJECT_IN_CELL_PANEL,
							 0,
							 0,
							 pList,
							 OBJECT_TYPE_Frame,
							 &Style) == ERRORCODE_None)
			{
				// We have a frame.
				pFrame = (CFrameObject*)pList->last_object();

				// Make sure we have the right position.
				pFrame->set_unrotated_bound(Bound);
				pFrame->set_rotation(pCell->get_rotation());

				// Make the grouping official.
				pCell->establish();
				get_database()->document_modified();
			}
		}
		else if (pObject->type() == OBJECT_TYPE_Frame)
		{
			pFrame = (CFrameObject*)pObject;
		}
		else
		{
			// There's an object attached to this cell that's not a text frame.
			// Do nothing.
		}
	}
	// Make sure the container is set correctly.
	if (pFrame != NULL)
	{
		pFrame->set_container(pCell);
	}
	return pFrame;
}

ERRORCODE CTableObject::AddFrame(PBOX_PTR Bound, LPCSTR pText, ALIGN_TYPE nAlign, VERT_ALIGN_TYPE nVAlign, SHORT nPanel, FLAGS wFlags, FLAGS wSelFlags, ObjectListPtr pList, DB_OBJECT_TYPE nType, CTextStyle* pStyle /*=NULL*/)
{
	// Fill out the frame object creation structure.

	FRAMEOBJ_CREATE_STRUCT fcs;
	fcs.bound = *Bound;
//	fcs.text = pText;
	fcs.alignment = nAlign;
	fcs.vert_alignment = nVAlign;
	fcs.flags = wFlags;
//	fcs.pStyle = pStyle;
	fcs.LeftMargin =
		fcs.TopMargin =
		fcs.RightMargin =
		fcs.BottomMargin = PAGE_RESOLUTION/20;		// 0.05"

	// Create the object.
	CFrameObject* pObject;
	if ((pObject = (CFrameObject*)database->new_object(nType, &fcs)) == NULL)
	{
		return database->last_creation_error();
	}
	pObject->set_panel(nPanel);
	pObject->set_select_flags(wSelFlags);

	// Add it to the list.
	pList->append(pObject);

	// Stick in the text.
	CTxp Txp(get_database());
	Txp.Init(pObject, 0);
	Txp.SetHorizontalAlignment(nAlign);

	if (pText == NULL || *pText == '\0')
	{
		// No text. Set the base paragraph style.
		if (pStyle != NULL)
		{
			pObject->SetTextStyle(pStyle);
		}
	}
	else
	{
		// Set the style for insertion.
		if (pStyle == NULL)
		{
			Txp.CalcStyle();
		}
		else
		{
			Txp.Style(*pStyle);
		}
		// Insert the string.
		Txp.InsertString(pText);
	}

	return ERRORCODE_None;
}

// Fit the table cells to the text.
void CTableObject::FitToText(void)
{
	PBOX ChangedExtent;
	if (FitCellsToText(&ChangedExtent))
	{
		// Calc the new cell layouts.
		calc();
		// This is a hack. It may need to change for thicker lines.
		ChangedExtent.x0 -= PAGE_RESOLUTION/10;
		ChangedExtent.y0 -= PAGE_RESOLUTION/10;
		ChangedExtent.x1 += PAGE_RESOLUTION/10;
		ChangedExtent.y1 += PAGE_RESOLUTION/10;
		// And refresh what changed.
		double dAngle = get_rotation();
		if (dAngle != 0.0)
		{
			compute_rotated_bound(ChangedExtent, dAngle, &ChangedExtent, get_origin());
		}
		// Do the refresh.
		get_database()->do_refresh_notify(&ChangedExtent, REFRESH_ALL, NULL);
	}
}

// Set whether we are fitting to text.
void CTableObject::SetFitToText(BOOL fFitToText)
{
	if (m_TRecord.m_fFitToText != fFitToText)
	{
		m_TRecord.m_fFitToText = fFitToText;
		FitToText();
	}
}

void CTableObject::UpdateDesiredRowHeights(void)
{
	m_DesiredRowHeights = m_RowHeights;
	m_TRecord.m_dExtraHeight = 0.0;
}

// Grow the cells to fit them to the text.
BOOL CTableObject::FitCellsToText(PBOX* pChangedExtent)
{
	BOOL fChanged = FALSE;
	BOOL fYFlipped = FALSE;

	int nRows = GetRows();
	int nColumns = GetColumns();

	if (GetFitToText() && nRows != 0 && nColumns != 0)
	{
		// Compute the size of the bound minus the extra added on.
		PBOX CurrentBound = get_unrotated_bound();
		if (CurrentBound.y1 < CurrentBound.y0)
		{
			fYFlipped = TRUE;
			PCOORD t = CurrentBound.y0;
			CurrentBound.y0 = CurrentBound.y1;
			CurrentBound.y1 = t;
		}
		PCOORD CurrentHeight = CurrentBound.Height();
		PBOX DesiredBound = CurrentBound;
		DesiredBound.y1 -= (PCOORD)(DesiredBound.Height()*m_TRecord.m_dExtraHeight);

		// Allocate an array of row heights and an array of "desired" row heights.
		PCOORD* pDesiredRowHeights = new PCOORD[nRows];
		PCOORD* pCurrentRowHeights = new PCOORD[nRows];
		PCOORD* pRowHeights = new PCOORD[nRows];

		// Compute all the "normal" heights.
		PCOORD DesiredHeight = DesiredBound.Height();
		int nRow;
		for (nRow = 0; nRow < nRows; nRow++)
		{
			pDesiredRowHeights[nRow] = (PCOORD)(m_DesiredRowHeights.GetAt(nRow)*DesiredHeight);
			pCurrentRowHeights[nRow] = GetRowHeight(nRow);
		}

		// Run through all the text boxes and compute their stretched sizes.
		PCOORD NewBoundHeight = 0;
		CPoint cpCell;
		double dMinExpansion = -1;
		for (cpCell.y = 0; cpCell.y < nRows; cpCell.y++)
		{
			PCOORD MaxHeight = pDesiredRowHeights[cpCell.y];
			for (cpCell.x = 0; cpCell.x < nColumns; cpCell.x++)
			{
				CFrameObject* pFrameObject = GetCellFrame(cpCell);
				if (pFrameObject != NULL)
				{
					PCOORD Height = pFrameObject->ComputeTextHeight();
					if (MaxHeight < Height)
					{
						MaxHeight = Height;
					}
				}
			}
			// We have the maximum height for this row.
			pRowHeights[cpCell.y] = MaxHeight;
			if (MaxHeight <= pCurrentRowHeights[cpCell.y])
			{
				dMinExpansion = 1.0;
			}
			else
			{
				// We are getting bigger.
				// Accumulate the minimum row expansion.
				double dExpansion = (double)MaxHeight/(double)pCurrentRowHeights[cpCell.y];
				if (dMinExpansion == -1 || dExpansion < dMinExpansion)
				{
					dMinExpansion = dExpansion;
				}
			}
			NewBoundHeight += MaxHeight;
		}
		// Compute the "extra" height (new over what is desired).
		ASSERT(dMinExpansion >= 1.0);
//		PCOORD RealDesiredHeight = (PCOORD)(DesiredHeight*dMinExpansion);
		PCOORD RealDesiredHeight = DesiredHeight;
//		ASSERT(RealDesiredHeight <= NewBoundHeight);
		m_TRecord.m_dExtraHeight = (double)(NewBoundHeight-RealDesiredHeight)/(double)NewBoundHeight;

		// See if the rows have changed size.
		PCOORD y0 = CurrentBound.y0;
		for (nRow = 0; nRow < nRows; nRow++)
		{
			if (pCurrentRowHeights[nRow] != pRowHeights[nRow])
			{
				fChanged = TRUE;
				if (pChangedExtent != NULL)
				{
					// First time.
					pChangedExtent->x0 = CurrentBound.x0;
					pChangedExtent->x1 = CurrentBound.x1;
					pChangedExtent->y0 = y0;
					pChangedExtent->y1 = CurrentBound.y0
							+ ((NewBoundHeight > CurrentHeight) ? NewBoundHeight : CurrentHeight);
				}
				break;
			}
			// Advance start of changes downward.
			y0 += pRowHeights[nRow];
		}

		// If the row heights changed, change us now.
		if (fChanged)
		{
			// We need to change the current bound.
			PPNT Anchor;
			get_anchor_point(OBJECT_HANDLE_LOWER, &Anchor);

			// Set the new bound.
			get_database()->toggle_object(this, 0);
			CurrentBound.y1 = CurrentBound.y0 + NewBoundHeight;
			if (fYFlipped)
			{
				PCOORD t = CurrentBound.y0;
				CurrentBound.y0 = CurrentBound.y1;
				CurrentBound.y1 = t;
			}
			set_unrotated_bound(CurrentBound);

			// Set the new row sizes. We need to do this since the bound
			// changed.
			for (int nRow = 0; nRow < GetRows(); nRow++)
			{
				SetRowHeight(nRow, pRowHeights[nRow]);
			}

			compute_handles();
			move_to_anchor(OBJECT_HANDLE_LOWER, Anchor);

			record.flags |= OBJECT_FLAG_needs_calc;

			// Compute bound and vertices.
			compute_handles();
			get_database()->toggle_object(this, 0);
		}

		delete [] pDesiredRowHeights;
		delete [] pCurrentRowHeights;
		delete [] pRowHeights;
	}
	return fChanged;
}

// Delete the contents of the selected cells.
void CTableObject::DeleteSelectedCells(void)
{
	if (SomethingSelected())
	{
		DeleteCells(m_crSelect);
		InvalidateSelection();
	}
}

void CTableObject::DeleteCells(CRect crCells)
{
	CPoint cpCell;
	for (cpCell.y = crCells.top; cpCell.y <= crCells.bottom; cpCell.y++)
	{
		for (cpCell.x = crCells.left; cpCell.x <= crCells.right; cpCell.x++)
		{
			CCellObject* pCell = GetCellAt(cpCell);
			if (pCell != NULL)
			{
				pCell->destroy_objects();
			}
		}
	}
}

//
// Compute the text style for the selected cells.
//

void CTableObject::ComputeSelectTextStyle(void)
{
	// Make all undefined by default.
	m_SelectStyle.MakeParagraphUndefined();
	m_SelectStyle.MakeTextUndefined();

	// If we have some cells selected, compute the style.
	if (SomethingSelected())
	{
		// Compute the composite text style for all the cells.
		BOOL fFirst = TRUE;
		CPoint cpCell;
		CRect crArea = GetSelect();
		for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
		{
			for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
			{
				CFrameObject* pFrame = GetCellFrame(cpCell);
				CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
				if ((pFrame != NULL) && ((pCellInfo != NULL) & !pCellInfo->IsSpanned()))
				{
					// Setup a Txp to access the text style.
					CTxp Txp(NULL);
					// Compute the style for all the text.
					CTextRecord* pText = pFrame->LockTextRecord();
					if (pText != NULL)
					{
						// Initialize our style.
						CTextIterator Iterator(pText);
						Iterator.SetPosition(0);
						CTextStyle Style;
						int nCharacters = pText->NumberOfCharacters();
						if (nCharacters == 0)
						{
							// Use the iterator style.
							Style = Iterator.Style();
						}
						else
						{
							Iterator.ComputeStyleRange(0, nCharacters, &Style);
						}

						if (fFirst)
						{
							// Set the style the first time.
							m_SelectStyle = Style;
							fFirst = FALSE;		// Not first anymore.
						}
						else
						{
							// Merge the styles.
							int nToken;
							// Check paragraph style changes.
							for (nToken = FIRST_PARAGRAPH_TOKEN; nToken < LAST_PARAGRAPH_TOKEN; nToken++)
							{
								if (!m_SelectStyle.CompareStyles(nToken, &Style))
								{
									m_SelectStyle.MakeUndefined(nToken);
								}
							}
							// Check text (character) style changes.
							for (nToken = FIRST_TEXT_TOKEN; nToken < LAST_TEXT_TOKEN; nToken++)
							{
								if (!m_SelectStyle.CompareStyles(nToken, &Style))
								{
									m_SelectStyle.MakeUndefined(nToken);
								}
							}
						}
						pText->release();
					}
				}
			}
		}
	}
}

//
// Change the font of the current selection.
//

void CTableObject::ChangeSelectFont(CTextStyle& NewStyle)
{
	// Run through all cells and set the font to what is passed.
	// Naturally, not all of the passed style must be defined, to allow
	// partial changes (like bold, italic, face, size, etc).
	CPoint cpCell;
	CRect crArea = GetSelect();
	for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
		{
			// Get the frame for this cell.
			CFrameObject* pFrame = GetCellFrame(cpCell, FALSE);
			CTableCellInfo* pCellInfo = GetCellInfo(cpCell);
			if ((pFrame != NULL) && ((pCellInfo != NULL) & !pCellInfo->IsSpanned()))
			{
				pFrame->SetTextStyle(&NewStyle);
			}
		}
	}
}
