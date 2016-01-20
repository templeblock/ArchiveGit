/*
// $Workfile: TblObj.h $
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
// $Log: /PM8/App/TblObj.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 25    7/19/98 6:27p Jayn
// Merge cells. Applying font attributes to cells.
// 
// 24    7/18/98 6:24p Jayn
// Table fixes.
// 
// 23    7/12/98 5:36p Jayn
// Various fixes.
// 
// 22    7/04/98 2:04p Jayn
// New table undo commands.
// 
// 21    6/29/98 12:30p Jayn
// Added assign() and original_dims().
// 
// 20    6/17/98 11:09a Jayn
// 
// 19    6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 18    6/03/98 1:46p Jayn
// Table Autoformat changes
// 
// 17    6/02/98 3:00p Jayn
// 
// 16    5/31/98 2:35p Jayn
// Delete row/columns
// 
// 15    5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 14    5/29/98 6:39p Jayn
// Insert column, etc.
// 
// 13    5/28/98 4:11p Jayn
// More cool table stuff (cell formatting, etc.)
// 
// 12    5/26/98 2:39p Jayn
// New table code.
// 
// 11    4/30/98 4:48p Jayn
// Resizing of table rows and columns.
// 
// 10    4/28/98 3:39p Jayn
// New table cursors.
// 
// 9     4/22/98 6:27p Jayn
// Table development (primarily selection).
// 
// 8     4/14/98 4:41p Jayn
// Changes for the new table object.
*/

#ifndef __TBLOBJ_H__
#define __TBLOBJ_H__

//
// The table object.
// Notes:
// - A table is a group. It has sub-groups containing the parts of the table.
// Currently, the table has a single sub-group (the "cell" group). This cell
// group contains all the cell elements (which are themselves groups contain-
// ing the actual target object, text frame or maybe picture in the future).
// Being a group, the cell group has a list of objects. The cells are ordered
// within this list in row-major order (by rows first, then by columns).
// So, you can find cell (column, row) by accessing element row*nColumns+column.
// In order to make access within the list faster, a pointer to the starting
// object in each row is cached (this prevents needed to step through all
// objects to get to a desired cell).
//
// - The column widths and row heights are stored separately. This allows
// for consistency while merging and splitting cells. No matter how many cells
// get merged, there will still be an underlying idea of where the non-merged
// boundaries are. The table is "rigid" and predictable in that respect.
//
// - The widths and heights are stored as fractions of the table dimensions.
// This makes the code work when the table is resized (otherwise, you have
// absolute sizes that need to be recalculated). These fractions are stored
// as doubles.
//
// - The selected cells for a table are managed by the table. Currently, the
// select is rectangular. This should be sufficient for what we want to do.
//
// - Border parts are broken down into horizontal (in HBorders) and vertical
// (in VBorders) border segments. Horizontal borders are arranged by rows,
// with rows running from top to bottom. There are nColumns border pieces in
// each row, and nRows+1 rows. Vertical borders are arranged by columns,
// with columns running from left to right. There are nRows border pieces in
// each column, and nColumns+1 columns.
//

#include "grpobj.h"
#include "tbldef.h"
#include "text.h"

class CCellObject;

enum
{
	// The panel for the cells.
	CELL_PANEL			= MAIN_TABLE_PANEL,
	// The panel for objects contained by cells.
	OBJECT_IN_CELL_PANEL = CELL_PANEL+1
};

//
// The main table object record (structure contained within CTableObject).
//

struct CTableObjectRecord
{
	TABLE_INFO	m_Info;
	short			m_fFitToText;
	double		m_dExtraHeight;		// For "fit to text" case
};

/*
// The class for saving a GroupObject's state.
*/

class CTableObjectState : public GroupObjectState
{
public:
	// The column widths and row heights.
	CTableObjectRecord m_TRecord;		// The table record.
	CDoubleArray m_DesiredRowHeights; // Desired heights of each row (not merged)
	CDoubleArray m_RowHeights;			// Heights of each row (not merged)
	CDoubleArray m_ColumnWidths;		// Widths of each column (not merged)
};

//
// The table object.
//

class CTableObject : public GroupObject
{
	INHERIT(CTableObject, GroupObject);
protected:
	// See if a point is within the object of a group.

	BOOL pt_in_group(GroupObject* group, PPNT p, RedisplayContextPtr rc, PMGPageObject* far *sub_object);

	CTableObjectRecord m_TRecord;
	CDoubleArray m_RowHeights;			// Heights of each row (not merged)
	CDoubleArray m_ColumnWidths;		// Widths of each column (not merged)
	CDoubleArray m_DesiredRowHeights;// Heights of each row (not merged)
	CTableCellInfoArray m_CellInfo;	// info for each cell
	CTableBorderSegmentArray m_HBorders;	// horizontal border elements
	CTableBorderSegmentArray m_VBorders;	// vertical border elements
	CTextStyle m_SelectStyle;

	// Save area when resizing columns or rows.
	CDoubleArray m_RCSave;

	// The calculated first object in each row (an optimization).
	CPtrArray m_cpaRowStarts;

	CRect m_crSelect;

	// Normalize a point (move it into the space of our unrotated bound).
	void NormalizePoint(PPNT& p);
	void NormalizeBox(PBOX& b);

	// Helper functions. Operate on normalized coordinates.
	int ColumnOfX(PCOORD x);
	int RowOfY(PCOORD y);

	// Text editing helper functions.

	// Add a frame object to a calendar.
	ERRORCODE AddFrame(PBOX_PTR Bound, LPCSTR pText, ALIGN_TYPE nAlign, VERT_ALIGN_TYPE nVAlign, SHORT nPanel, FLAGS wFlags, FLAGS wSelFlags, ObjectListPtr pList, DB_OBJECT_TYPE nType, CTextStyle* pStyle = NULL);

	// Grow the cells to fit them to the text.
	BOOL FitCellsToText(PBOX* pRefreshExtent = NULL);

	// Update the desired row heights from the current ones.
	void UpdateDesiredRowHeights(void);

	// Copy a frame to a cell, replacing what is there currently.
	void CopyFrameToCell(CFrameObject* pFrame, CPoint cpCell);

public:
	CTableObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

	void Rebuild(const TABLE_INFO* pInfo = NULL);

	void SetInfo(const TABLE_INFO& Info)
		{ m_TRecord.m_Info = Info; }

	// Fit the table cells to the contained text.
	void FitToText(void);

	// Are we fitting to text?
	BOOL GetFitToText(void) const
		{ return m_TRecord.m_fFitToText; }
	// Set whether we are fitting to text.
	void SetFitToText(BOOL fFitToText);

	short GetRows(void) const
		{ return m_TRecord.m_Info.m_nRows; }
	short GetColumns(void) const
		{ return m_TRecord.m_Info.m_nColumns; }
	void SetRows(short nRows)
		{ m_TRecord.m_Info.m_nRows = nRows; }
	void SetColumns(short nColumns)
		{ m_TRecord.m_Info.m_nColumns = nColumns; }

	PCOORD GetRowHeight(int nRow);
	PCOORD GetColumnWidth(int nColumn);
	void SetColumnWidth(int nColumn, PCOORD Width);
	void SetRowHeight(int nRow, PCOORD Height);

	int ColumnOfPoint(PPNT p);
	int RowOfPoint(PPNT p);
	CPoint CellOfPoint(PPNT p);

	int GetColumnResizer(PBOX box, BOOL fNormalize = TRUE);
	int GetRowResizer(PBOX box, BOOL fNormalize = TRUE);

	// Add a column
	void AddColumn(int nNewColumn, CTableNewRCInfo* pNewRCInfo);
	// Add a row
	void AddRow(int nNewRow, CTableNewRCInfo* pNewInfo);

	// Delete a column
	void DeleteColumn(int nDeleteColumn);
	// Delete a row
	void DeleteRow(int nDeleteRow);

	// Get new column information from the passed column.
	void GetNewColumnInfo(int nColumn, CTableNewRCInfo* pNewInfo);
	// Get new row information from the passed column.
	void GetNewRowInfo(int nRow, CTableNewRCInfo* pNewInfo);

	// Get the cells within an area that are merged.
	// This is used for "split cells" undo.
	void GetMergedCells(const CRect& crArea, CArray<CRect,CRect&>& arRects);

	// Merge the selected cells.
	void MergeCells(void);
	void MergeCells(CRect crArea, BOOL fMergeContents = TRUE);
	BOOL CanMergeCells(void);
	void MergeContents(CRect crArea);

	// Split all merged cells within the current selection.
	void SplitCells(CArray<CRect, CRect&>& arMerged);
	BOOL CanSplitCells(void);
	// Helper for SplitCells.
	void SplitCell(CPoint cpCell);

	// Fill down.
	void FillDown(void);
	BOOL CanFillDown(void);

	// Fill right.
	void FillRight(void);
	BOOL CanFillRight(void);

	// Return various border elements.
	// The first two are straight access.
	CTableBorderSegment* GetHBorder(int n)
		{ return m_HBorders.GetAt(n); }
	CTableBorderSegment* GetVBorder(int n)
		{ return m_VBorders.GetAt(n); }

	// The last four are relative to a cell.
	CTableBorderSegment* GetLeftBorder(CPoint cpCell);
	CTableBorderSegment* GetRightBorder(CPoint cpCell);
	CTableBorderSegment* GetTopBorder(CPoint cpCell);
	CTableBorderSegment* GetBottomBorder(CPoint cpCell);

	// Cell info access routines.
	// Get cell info for an index.
	CTableCellInfo* GetCellInfo(int nIndex)
		{ return m_CellInfo.GetAt((short)nIndex); }

	// Get cell info for a cell.
	CTableCellInfo* GetCellInfo(CPoint cpCell);

	void SetSelect(const CRect& crSelect, BOOL fToggleSelect = TRUE);
	CRect GetSelect(void) const
		{ return m_crSelect; }
	void DeselectAll(BOOL fToggleSelect = TRUE);
	BOOL SomethingSelected(void) const
		{ return m_crSelect.left != -1; }
	void InvalidateSelection(void);
	void InvalidateCells(const CRect& crInvalidate);
	void MergeExpandSelect(CRect& crSelect);
	CTextStyle& GetSelectStyle(void)
		{ return m_SelectStyle; }

	// Keyboard navigation.
#if 0
	void SelectRight(void);
	void SelectLeft(void);
	void SelectUp(void);
	void SelectDown(void);
	void SelectNext(void);
	void SelectPrevious(void);
#endif

	// Routines to get and set cell formatting (user-interface related).
	BOOL GetCellFormatting(CTableCellFormatting* pFormatting);
	void SetCellFormatting(const CTableCellFormatting& Formatting);

	// Routines to save and restore cell formatting (undo-related).
	BOOL SaveCellFormatting(CTableCellFormattingSave* pFormatting, LPCRECT pcrArea = NULL);
	void RestoreCellFormatting(CTableCellFormattingSave& Formatting, BOOL fHandleHidden = FALSE);

	// Routine to set table formatting (auto-format related).
	void SetTableFormatting(CTableFormatting& Formatting);
	void SetCellAlignmentFormatting(CTableFormatting& Formatting);
	void SetCellFontFormatting(CTableFormatting& Formatting);
	void ReflowCellFrames(CRect crArea);

	// Routines to save and restore cells (undo-related).
	BOOL SaveCells(CTableCellSave* pCellSave);
	void RestoreCells(CTableCellSave& CellSave);
	void SwapCells(CTableCellSave& CellSave);

	// Row and column selection.
	void SelectRows(int nStart, int nEnd);
	void SelectColumns(int nStart, int nEnd);

	// Text editing functions.
	CFrameObject* GetCellFrame(CPoint cpCell, BOOL fHonorMerging = TRUE);

	// Delete the contents of the selected cells.
	void DeleteSelectedCells(void);

	// Delete the contents of cells in a range.
	void DeleteCells(CRect crCells);

	// Change the font of the current selection.
	void ChangeSelectFont(CTextStyle& NewStyle);

	// Compute the text style for the selected cells.
	void ComputeSelectTextStyle(void);

	// This is a method invoked by the read method to handle object-specific data.
	ERRORCODE read_data(StorageDevicePtr device);

	// This is a method invoked by the write method to handle object-specific data.
	ERRORCODE write_data(StorageDevicePtr device);

	// This is a method invoked by the size method to handle object-specific data.
	ST_MAN_SIZE size_data(StorageDevicePtr device);

	// Update an object into a redisplay context.
	virtual UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

	virtual BOOL SetObjectProperties(CObjectProperties& Properties);
	virtual void GetObjectProperties(CObjectProperties& Properties);

	// Return whether a point is in an object or not.
	virtual BOOL pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object);
	virtual BOOL pt_on_move_area (PPNT p, RedisplayContextPtr rc);
	virtual PMG_OBJECT_ACTION move_object(DRAG_STATE state, PPNT vector, PPNT current_xy, LPVOID far *data = NULL, BOOL fPanning = FALSE);
	virtual OBJECT_HANDLE box_on_handle(PBOX_PTR box, RedisplayContextPtr rc);
	virtual PMG_OBJECT_ACTION move_handle(DRAG_STATE state, PPNT pos, OBJECT_HANDLE handle, FLAGS shift_status, LPVOID far *data = NULL, BOOL fCropping = FALSE);
	virtual void OnBeginHandleMove(PPNT pos, OBJECT_HANDLE handle, BOOL fCropping);

	virtual BOOL toggle_selects(RedisplayContextPtr rc, FLAGS which);
	virtual void ToggleBounds(RedisplayContext* rc, POINT* p);

	// Recalc after a size or movement.
	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);

	// Assign an object to this.
	virtual ERRORCODE assign(PageObjectRef sobject);

	// Get the original dimensions of this group.
	virtual BOOL original_dims(PPNT_PTR p);

	// Return the refresh bounds for this object.
	virtual VOID get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

	// Create a state for saving.
	// We return a pointer to a new object. This object needs to be freed by
	// the caller at some point.
	virtual PageObjectState* CreateState(void);

	// Save our state.
	virtual void SaveState(PageObjectState* pState);

	// Restore our state.
	// The state passed in is NOT deleted by us. It is still the responsibility
	// of the caller (or whomever).
	virtual void RestoreState(PageObjectState* pState);

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
	virtual void OnSelectChange(BOOL fSelected = TRUE);

	//
	// Table operations.
	//
	ERRORCODE BuildTable(void);
	void CalculateTable(void);
//	GroupObjectPtr FindGroup(short nPanel);
//	void DeleteGroup(short nPanel);
	CCellObject* GetCellAt(CPoint cpCell);

	static int m_nMaxTableRows;
	static int m_nMaxTableColumns;

	static int GetMaxRows(void) { return m_nMaxTableRows; }
	static int GetMaxColumns(void) { return m_nMaxTableColumns; }

protected:
	ERRORCODE AddCell(const PBOX& Bound, const CPoint& cpCell, SHORT panel, FLAGS sel_flags, ObjectListPtr pList);
	// Add a group object to a table
	GroupObjectPtr AddGroup(SHORT panel, FLAGS flags = 0);
	// Ensure all table groups exist.
	BOOL BuildTableGroups(GroupObjectPtr *ppCellGroup);

	void CreateDefaultCellInfo(void);
	void CreateDefaultBorderSegments(void);

	BOOL GetSelectPoints(PPNT v[], CRect crToggle, BOOL fAddBorderWidths = FALSE);
	BOOL GetSelectRegion(CRgn& rgnSelect, CRect crToggle, RedisplayContext* rc);
	void ToggleSquare(RedisplayContext* rc, CRect crToggle);

	// Resizing helpers.
	void ResizeColumn(int nResizeColumn, DRAG_STATE state, PPNT pos, FLAGS shift_status);
	void ResizeRow(int nResizeRow, DRAG_STATE state, PPNT pos, FLAGS shift_status);

	// Reposition the cells based on the row and column dimensions.
	void LayoutCells(const CRect* pcrArea = NULL);

	void DrawGrid(RedisplayContext* rc);
	void GetRowAndColumnPositions(int*& nColumnX, int*&nRowY, RedisplayContext* rc);

	void DumpCells(void);
};

#endif
