//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/TBLFMT.H 1     3/03/99 6:12p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/TBLFMT.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 3     6/03/98 1:46p Jayn
// Table Autoformat changes
// 
// 2     6/02/98 3:00p Jayn
// 
// 1     6/01/98 4:15p Jayn
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __TBLFMT_H__
#define __TBLFMT_H__

//
// Table rules.
//

class CTableFormatRule
{
public:
	CTableFormatRule(int nColumn, int nRow)
			: m_nColumn(nColumn), m_nRow(nRow) {}

	// Positive values indicate absolute row/column values.
	enum
	{
		CELL_ANY = -1,					// Matches any row/column
		CELL_LAST = -2,				// The last row/column
		CELL_NEXT_TO_LAST = -3,		// The next-to-last row/column
		CELL_EVEN = -4,				// Even row/columns
		CELL_ODD = -5,					// Odd row/columns
	};
	int m_nColumn;
	int m_nRow;
	// Call this one!
	BOOL RuleMatches(CPoint cpCell, CPoint cpMax) const;
protected:
	// Helper function.
	BOOL EntryMatches(int nRule, int nCell, int nMax) const;
};

//
// A cell info rule.
//

class CTableCellInfoRule : public CTableFormatRule
{
public:
	// This adds cell info (just a color is all we need).
	CTableCellInfoRule(int nColumn, int nRow, COLOR CellColor)
			: CTableFormatRule(nColumn, nRow), m_CellColor(CellColor) {}
	COLOR m_CellColor;
};

//
// A border rule.
//

class CTableBorderSegmentRule : public CTableFormatRule
{
public:
	// This adds a border segment.
	CTableBorderSegmentRule(int nColumn, int nRow, BYTE bType, BYTE bWidth, COLOR Color)
			: CTableFormatRule(nColumn, nRow), m_BorderSegment(bType, bWidth, Color) {}

	CTableBorderSegment m_BorderSegment;
};

//
// A cell alignment rule.
//

class CTableCellAlignmentRule : public CTableFormatRule
{
public:
	// This adds a cell alignment
	CTableCellAlignmentRule(int nColumn, int nRow, int nHAlign, int nVAlign)
			: CTableFormatRule(nColumn, nRow), m_CellAlignment(nHAlign, nVAlign) {}

	CTableCellAlignment m_CellAlignment;
};

//
// A cell font rule.
//

class CTableCellFontRule : public CTableFormatRule
{
public:
	// This adds a cell font.
	CTableCellFontRule(int nColumn, int nRow, BOOL fBold, BOOL fItalic, COLOR Color)
			: CTableFormatRule(nColumn, nRow), m_CellFont(fBold, fItalic, Color) {}

	CTableCellFont m_CellFont;
};

//
// A standard rule set for generating a table format.
// 

class CTableFormatRuleSet
{
public:
	// Cell info rules.
	int m_nCellInfoRules;
	const CTableCellInfoRule* m_pCellInfoRules;

	// Horizontal borders rules
	int m_nHBorderRules;
	const CTableBorderSegmentRule* m_pHBorderRules;

	// Vertical border rules
	int m_nVBorderRules;
	const CTableBorderSegmentRule* m_pVBorderRules;

	// Cell alignment rules
	int m_nCellAlignmentRules;
	const CTableCellAlignmentRule* m_pCellAlignmentRules;

	// Cell font rules
	int m_nCellFontRules;
	const CTableCellFontRule* m_pCellFontRules;
};

//
// Base table auto-format class.
//

class CTableFormatter
{
public:
	CTableFormatter(UINT uNameID = 0, UINT uDataID = 0) { m_uNameID = uNameID; m_uDataID = uDataID; }
	virtual ~CTableFormatter() {}

	// Format a table.
	virtual BOOL FormatTable(CTableObject* pTable);

	// Return a string resource ID for the name of this table formatter.
	UINT GetFormatterName(void) const
		{ return m_uNameID; }

	// Return a string resource ID for the cell data for this table formatter.
	UINT GetCellData(void) const
		{ return m_uDataID; }

	// Return the global array of table formatters.
	static CTableFormatter** GetTableFormatters(void);

protected:
	// The ID for the name.
	UINT m_uNameID;
	// The ID for the cell data.
	UINT m_uDataID;

	// Get the rule set for this table. Override this!
	virtual CTableFormatRuleSet* GetRuleSet(void);

	// The standard way to create formatting for a table.
	virtual BOOL GetTableFormatting(CTableFormatting& Formatting);
	// Apply cell formatting rules. This is a helper function called
	// by derived classes.
	void ApplyFormattingRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet);

	// Helper function to apply cell info rules.
	void CTableFormatter::ApplyCellInfoRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet);
	// Helper function to apply horizontal border rules.
	void CTableFormatter::ApplyHBorderRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet);
	// Helper function to apply vertical border rules.
	void CTableFormatter::ApplyVBorderRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet);
	// Helper function to apply cell alignment rules.
	void CTableFormatter::ApplyCellAlignmentRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet);
	// Helper function to apply cell font rules.
	void CTableFormatter::ApplyCellFontRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet);
};

//
// Implement a rule-based formatter.
//

class CRuleBasedTableFormatter : public CTableFormatter
{
public:
	CRuleBasedTableFormatter(UINT uNameID, UINT uDataID, CTableFormatRuleSet* pRuleSet)
		: CTableFormatter(uNameID, uDataID), m_pRuleSet(pRuleSet) {}
protected:
	CTableFormatRuleSet* m_pRuleSet;

	// Get the rule set for this table.
	virtual CTableFormatRuleSet* GetRuleSet(void)
		{ return m_pRuleSet; }
};

#endif
