/*
// $Workfile: Tbldef.h $
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
// $Log: /PM8/App/Tbldef.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 8     7/04/98 2:04p Jayn
// New table undo commands.
// 
// 7     6/03/98 1:46p Jayn
// Table Autoformat changes
// 
// 6     6/02/98 3:00p Jayn
// 
// 5     5/31/98 2:35p Jayn
// Delete row/columns
// 
// 4     5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 3     5/29/98 6:39p Jayn
// Insert column, etc.
// 
// 2     5/28/98 4:11p Jayn
// More cool table stuff (cell formatting, etc.)
// 
// 1     5/26/98 1:33p Jayn
*/

#ifndef __TBLDEF_H__
#define __TBLDEF_H__

#include "pagedefs.h"

class RedisplayContext;

struct TABLE_INFO
{
	short m_nRows;
	short m_nColumns;
};

struct TABLE_CREATE_STRUCT
{
	TABLE_INFO info;
	PBOX bound;	
};

class CDoubleArray : public Array
{
public:
	CDoubleArray(void)
		: Array(sizeof(double), 0)
	{
	}

	double GetAt(SHORT n)
		{ return *(double*)get_element(n); }
	void SetAt(SHORT n, double d)
		{ *(double*)get_element(n) = d; }
	void Add(double d)
		{ add_element(&d); }
	void InsertAt(SHORT n, double d)
		{ insert_element(&d, n); }
	void DeleteAt(SHORT n)
		{ delete_element(n); }
};

enum
{
	BORDER_SEGMENT_None = 0,			// No line here
	BORDER_SEGMENT_Solid,				// Solid line
	BORDER_SEGMENT_Dashed,				// Dashed line (hairline ala GDI).
	BORDER_SEGMENT_Dotted,				// Dotted line (hairline ala GDI).

	BORDER_SEGMENT_Spanned = 0x40,	// Flag when a cell is spanned.
	BORDER_SEGMENT_Undefined = 0x80	// Multiply-defined value (or otherwise undefined)
};

struct CTableBorderSegment
{
	BYTE m_bType;			// type of segment (see enum above)
	BYTE m_bWidth;			// in points (only applies to Solid type)
	COLOR m_Color;			// color of segment

	CTableBorderSegment() {}
	CTableBorderSegment(BYTE bType, BYTE bWidth, COLOR Color)
		: m_bType(bType), m_bWidth(bWidth), m_Color(Color) {}

	void SetDefault(void);
	BOOL operator==(const CTableBorderSegment& s)
		{ return memcmp(this, &s, sizeof(*this)) == 0; }
	BOOL operator!=(const CTableBorderSegment& s)
		{ return !(operator==(s)); }
	BOOL SetupToDraw(RedisplayContext* rc, CPen& cpEdgePen, CBrush& cbEdgeBrush, int& nWidth);
	int GetWidth(RedisplayContext* rc);
	void CombineWith(const CTableBorderSegment& s);
	void ReplaceWith(const CTableBorderSegment& s);
	BOOL IsSpanned(void) const
		{ return !!(m_bType & BORDER_SEGMENT_Spanned); }

	static CBitmap m_cbmUndefined;
};

//
// An array of border segments.
//

class CTableBorderSegmentArray : public Array
{
public:
	CTableBorderSegmentArray()
		: Array(sizeof(CTableBorderSegment), 0)
	{
	}

	CTableBorderSegment* GetAt(SHORT n)
		{ return (CTableBorderSegment*)get_element(n); }
	void Add(const CTableBorderSegment* d)
		{ add_element((void*)d); }
	void InsertAt(SHORT n, const CTableBorderSegment* d)
		{ insert_element((void*)d, n); }
	void DeleteAt(SHORT n)
		{ delete_element(n); }
};

//
// Cell information.
//
// SpanX and SpanY control merging of cells:
// - normal value is 1.
// - if cell is spanning, value is number of cells spanned (positive, >0).
// - if cell is "spanned", value is negative or zero count to master spanning cell.
// - if either m_nSpanX is 0 or m_nSpanY is 0, the other value must be negative (a span).
//

struct CTableCellInfo
{
	short m_nSpanX;
	short m_nSpanY;
	COLOR m_Color;

	BOOL SetupToDraw(CBrush& cbBrush);
	BOOL IsSpanned(void) const
		{ return m_nSpanX <= 0; }
};

//
// An array of cell infos.
//

class CTableCellInfoArray : public Array
{
public:
	CTableCellInfoArray()
		: Array(sizeof(CTableCellInfo), 0)
	{
	}

	CTableCellInfo* GetAt(SHORT n)
		{ return (CTableCellInfo*)get_element(n); }
	void Add(const CTableCellInfo* d)
		{ add_element((void*)d); }
	void InsertAt(SHORT n, const CTableCellInfo* d)
		{ insert_element((void*)d, n); }
	void DeleteAt(SHORT n)
		{ delete_element(n); }
};

//
// Table alignment values.
// Used for auto-format.
//

class CTableCellAlignment
{
public:
	int m_nHAlign;
	int m_nVAlign;

	CTableCellAlignment() {}
	CTableCellAlignment(int nHAlign, int nVAlign)
		: m_nHAlign(nHAlign), m_nVAlign(nHAlign) {}
};

//
// An array of cell alignments.
//

class CTableCellAlignmentArray : public Array
{
public:
	CTableCellAlignmentArray()
		: Array(sizeof(CTableCellAlignment), 0)
	{
	}

	CTableCellAlignment* GetAt(SHORT n)
		{ return (CTableCellAlignment*)get_element(n); }
	void Add(const CTableCellAlignment* d)
		{ add_element((void*)d); }
	void InsertAt(SHORT n, const CTableCellAlignment* d)
		{ insert_element((void*)d, n); }
	void DeleteAt(SHORT n)
		{ delete_element(n); }
};

//
// Table font values.
// Used for auto-format.
//

class CTableCellFont
{
public:
	BOOL m_fBold;
	BOOL m_fItalic;
	COLOR m_Color;

	CTableCellFont() {}
	CTableCellFont(BOOL fBold, BOOL fItalic, COLOR Color)
		: m_fBold(fBold), m_fItalic(fItalic), m_Color(Color) {}
};

//
// An array of cell fonts.
//

class CTableCellFontArray : public Array
{
public:
	CTableCellFontArray()
		: Array(sizeof(CTableCellFont), 0)
	{
	}

	CTableCellFont* GetAt(SHORT n)
		{ return (CTableCellFont*)get_element(n); }
	void Add(const CTableCellFont* d)
		{ add_element((void*)d); }
	void InsertAt(SHORT n, const CTableCellFont* d)
		{ insert_element((void*)d, n); }
	void DeleteAt(SHORT n)
		{ delete_element(n); }
};

//
// A class for passing cell formatting information around.
//

class CTableCellFormatting
{
public:
	// The type of cell selection.
	enum
	{
		MultipleAcross = 1,
		MultipleDown = 2
	};
	int m_nType;

	// The segment properties.
	enum
	{
		SEGMENT_None = -1,
		SEGMENT_Left = 0,
		SEGMENT_Top,
		SEGMENT_Right,
		SEGMENT_Bottom,
		SEGMENT_InnerVertical,			// Only if MultipleAcross
		SEGMENT_InnerHorizontal,		// Only if MultipleDown
		NUM_SEGMENTS
	};
	CTableBorderSegment m_Segment[NUM_SEGMENTS];

	// The fill color.
	COLOR m_FillColor;
};

//
// Class used to save and restore formatting information.
// (Used by undo).
//

class CTableCellFormattingSave
{
public:
	CRect m_crArea;
	CTableCellInfoArray m_CellInfo;			// info for each cell
	CTableBorderSegmentArray m_HBorders;	// horizontal border elements
	CTableBorderSegmentArray m_VBorders;	// vertical border elements
};

class CTableCellSave : public CTableCellFormattingSave
{
public:
	// An array of pointers to cell contents.
	// Entries can be NULL if no contents are attached to a particular cell.
	CPtrArray m_cpaCellContents;
};

//
// Class used by the CTableFormatter class.
//

class CTableFormatting : public CTableCellFormattingSave
{
public:
	// Add more stuff when necessary.
	CPoint m_cpTableDims;

	CTableCellAlignmentArray m_CellAlignments;	// Cell alignment values
	CTableCellFontArray m_CellFonts;					// Cell font values
};

//
// Class used to pass formatting when creating a new row or column.
//

class CTableNewRCInfo
{
public:
	CTableNewRCInfo();
	~CTableNewRCInfo();
	void AllocateInfo(int nCells);

	CTableBorderSegment* m_pSegments;
	COLOR* m_pFillColors;
	PCOORD m_Size;
};

#endif
