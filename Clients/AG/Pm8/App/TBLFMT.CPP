//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/TBLFMT.CPP 1     3/03/99 6:12p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/TBLFMT.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 4     7/22/98 4:24p Johno
// Added 10 table formats
// 
// 3     6/03/98 1:46p Jayn
// Table Autoformat changes
// 
// 2     6/02/98 3:00p Jayn
// 
// 1     6/01/98 4:15p Jayn
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "tblobj.h"
#include "tblfmt.h"

#include "pagedefs.h"	// For COLOR_FROM_COLORREF
#include "utils.h"

#define	LINE_THIN	0
#define	LINE_FAT		2

#define	COLOR_GRID1	(COLOR_FROM_COLORREF(RGB(0, 0, 128)))
#define	COLOR_ROWS1	(COLOR_FROM_COLORREF(RGB(192, 192, 192)))

inline BOOL CTableFormatRule::EntryMatches(int nRule, int nCell, int nMax) const
{
	switch (nRule)
	{
		case CELL_ANY:
		{
			return TRUE;
		}
		case CELL_LAST:
		{
			return nCell == nMax-1;
		}
		case CELL_NEXT_TO_LAST:
		{
			return nCell == (nMax-1)-1;
		}
		case CELL_EVEN:
		{
			return (nCell & 1) == 0;
		}
		case CELL_ODD:
		{
			return (nCell & 1) != 0;
		}
		default:
		{
			// Must be an exact match.
			return (nRule == nCell);
		}
	}
}

//
// Return whether a rule matches or not.
//

BOOL CTableFormatRule::RuleMatches(CPoint cpCell, CPoint cpMax) const
{
	return EntryMatches(m_nColumn, cpCell.x, cpMax.x)
				&& EntryMatches(m_nRow, cpCell.y, cpMax.y);
}

/////////////////////////////////////////////////////////////////////////////
// CTableFormatter

BOOL CTableFormatter::FormatTable(CTableObject* pTable)
{
	// Make sure we have a good table.
	if (pTable == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	// Get the row and column count.
	int nRows = pTable->GetRows();
	int nColumns = pTable->GetColumns();
	if (nRows != 0 && nColumns != 0)
	{
		CTableFormatting Formatting;

		// Set the total rows and columns in the table.
		Formatting.m_cpTableDims = CPoint(nColumns, nRows);

		// Establish the area to be changed.
		Formatting.m_crArea.SetRect(0, 0, nColumns-1, nRows-1);

		// Create the table formatting.
		if (GetTableFormatting(Formatting))
		{
			// And set it in the table.
			pTable->SetTableFormatting(Formatting);
			return TRUE;
		}
	}

	// Failed for some reason.
	return FALSE;
}

BOOL CTableFormatter::GetTableFormatting(CTableFormatting& Formatting)
{
	CTableFormatRuleSet* pRuleSet = GetRuleSet();
	if (pRuleSet != NULL)
	{
		ApplyFormattingRules(Formatting, *pRuleSet);
		return TRUE;
	}
	// By default, nothing is set!
	return FALSE;
}

// Get the rule set for this table. Override this!
CTableFormatRuleSet* CTableFormatter::GetRuleSet(void)
{
	return NULL;
}

void CTableFormatter::ApplyFormattingRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet)
{
	ApplyCellInfoRules(Formatting, RuleSet);
	ApplyHBorderRules(Formatting, RuleSet);
	ApplyVBorderRules(Formatting, RuleSet);
	ApplyCellAlignmentRules(Formatting, RuleSet);
	ApplyCellFontRules(Formatting, RuleSet);
}

void CTableFormatter::ApplyCellInfoRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet)
{
	CPoint cpMax = Formatting.m_cpTableDims;
	CRect crArea = Formatting.m_crArea;

	CTableCellInfo CellInfo;
	CellInfo.m_nSpanX = 1;
	CellInfo.m_nSpanY = 1;

	CPoint cpCell;
	// Run through all the cells and apply the rules.
	for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
		{
			// Apply the cell rules.
			const CTableCellInfoRule* pRule = RuleSet.m_pCellInfoRules;
			for (int nRule = 0; nRule < RuleSet.m_nCellInfoRules; nRule++, pRule++)
			{
				if (pRule->RuleMatches(cpCell, cpMax))
				{
					CellInfo.m_Color = pRule->m_CellColor;
					break;
				}
			}
			// We need to find some matching rule.
			if (nRule == RuleSet.m_nCellInfoRules)
			{
				// We did not find a rule.
				ASSERT(FALSE);
				CellInfo.m_Color = TRANSPARENT_COLOR;
			}
			// Add the cell info.
			Formatting.m_CellInfo.Add(&CellInfo);
		}
	}
}

//
// Apply horizontal border rules.
//

void CTableFormatter::ApplyHBorderRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet)
{
	CPoint cpCell;
	CPoint cpMax = Formatting.m_cpTableDims;
	cpMax.y++;

	CRect crArea = Formatting.m_crArea;
	crArea.bottom++;		// We have an extra edge on the bottom.

	// Run through all the edges and apply the rules.
	for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
		{
			// Apply the horizontal border rules.
			const CTableBorderSegmentRule* pRule = RuleSet.m_pHBorderRules;
			for (int nRule = 0; nRule < RuleSet.m_nHBorderRules; nRule++, pRule++)
			{
				if (pRule->RuleMatches(cpCell, cpMax))
				{
					Formatting.m_HBorders.Add(&pRule->m_BorderSegment);
					break;
				}
			}
			// We need to find some matching rule.
			if (nRule == RuleSet.m_nHBorderRules)
			{
				// We did not find a rule.
				ASSERT(FALSE);
				CTableBorderSegment Segment(BORDER_SEGMENT_Solid, 0, TRANSPARENT_COLOR);
				Formatting.m_HBorders.Add(&Segment);
			}
		}
	}
}

//
// Apply vertical border rules.
//

void CTableFormatter::ApplyVBorderRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet)
{
	CPoint cpCell;
	CPoint cpMax = Formatting.m_cpTableDims;
	cpMax.x++;

	CRect crArea = Formatting.m_crArea;
	crArea.right++;		// We have an extra edge on the right.

	// Run through all the edges and apply the rules.
	for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
	{
		for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
		{
			// Apply the vertical border rules.
			const CTableBorderSegmentRule* pRule = RuleSet.m_pVBorderRules;
			for (int nRule = 0; nRule < RuleSet.m_nVBorderRules; nRule++, pRule++)
			{
				if (pRule->RuleMatches(cpCell, cpMax))
				{
					Formatting.m_VBorders.Add(&pRule->m_BorderSegment);
					break;
				}
			}
			// We need to find some matching rule.
			if (nRule == RuleSet.m_nVBorderRules)
			{
				// We did not find a rule.
				ASSERT(FALSE);
				CTableBorderSegment Segment(BORDER_SEGMENT_Solid, 0, TRANSPARENT_COLOR);
				Formatting.m_VBorders.Add(&Segment);
			}
		}
	}
}

void CTableFormatter::ApplyCellAlignmentRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet)
{
	CPoint cpMax = Formatting.m_cpTableDims;
	CRect crArea = Formatting.m_crArea;

	CPoint cpCell;
	// Run through all the cells and apply the rules.
	for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
		{
			// Apply the cell rules.
			const CTableCellAlignmentRule* pRule = RuleSet.m_pCellAlignmentRules;
			for (int nRule = 0; nRule < RuleSet.m_nCellAlignmentRules; nRule++, pRule++)
			{
				if (pRule->RuleMatches(cpCell, cpMax))
				{
					Formatting.m_CellAlignments.Add(&pRule->m_CellAlignment);
					break;
				}
			}
			// We need to find some matching rule.
			if (nRule == RuleSet.m_nCellAlignmentRules)
			{
				// We did not find a rule.
				ASSERT(FALSE);
				CTableCellAlignment Alignment(ALIGN_left, ALIGN_top);
				Formatting.m_CellAlignments.Add(&Alignment);
			}
		}
	}
}

void CTableFormatter::ApplyCellFontRules(CTableFormatting& Formatting, const CTableFormatRuleSet& RuleSet)
{
	CPoint cpMax = Formatting.m_cpTableDims;
	CRect crArea = Formatting.m_crArea;

	CPoint cpCell;
	// Run through all the cells and apply the rules.
	for (cpCell.y = crArea.top; cpCell.y <= crArea.bottom; cpCell.y++)
	{
		for (cpCell.x = crArea.left; cpCell.x <= crArea.right; cpCell.x++)
		{
			// Apply the cell rules.
			const CTableCellFontRule* pRule = RuleSet.m_pCellFontRules;
			for (int nRule = 0; nRule < RuleSet.m_nCellFontRules; nRule++, pRule++)
			{
				if (pRule->RuleMatches(cpCell, cpMax))
				{
					Formatting.m_CellFonts.Add(&pRule->m_CellFont);
					break;
				}
			}
			// We need to find some matching rule.
			if (nRule == RuleSet.m_nCellFontRules)
			{
				// We did not find a rule.
				ASSERT(FALSE);
				CTableCellFont Font(FALSE, FALSE, COLOR_BLACK);
				Formatting.m_CellFonts.Add(&Font);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// "None" table formatter.

// The border rules (the same for both H and V).
static CTableBorderSegmentRule NoneBorderRules[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, TRANSPARENT_COLOR)
};
// The fill rules.
static CTableCellInfoRule NoneCellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule NoneAlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_left, ALIGN_top)
};
// The font rules.
static CTableCellFontRule NoneFontRules[] =
{
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};
// The rule set.
static CTableFormatRuleSet NoneRuleSet =
{
	sizeof(NoneCellRules)/sizeof(NoneCellRules[0]),
	NoneCellRules,
	sizeof(NoneBorderRules)/sizeof(NoneBorderRules[0]),
	NoneBorderRules,
	sizeof(NoneBorderRules)/sizeof(NoneBorderRules[0]),
	NoneBorderRules,
	sizeof(NoneAlignmentRules)/sizeof(NoneAlignmentRules[0]),
	NoneAlignmentRules,
	sizeof(NoneFontRules)/sizeof(NoneFontRules[0]),
	NoneFontRules
};

static CRuleBasedTableFormatter NoneTableFormatter(IDS_NoneTableFormat, IDS_TableEntries1, &NoneRuleSet);

/////////////////////////////////////////////////////////////////////////////
// "Simple" table formatter.
// The border rules (the same for both H and V).
static CTableBorderSegmentRule SimpleBorderRules[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The fill rules.
static CTableCellInfoRule SimpleCellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule SimpleAlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule SimpleFontRules[] =
{
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, TRUE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet SimpleRuleSet =
{
	sizeof(SimpleCellRules)/sizeof(SimpleCellRules[0]),
	SimpleCellRules,
	sizeof(SimpleBorderRules)/sizeof(SimpleBorderRules[0]),
	SimpleBorderRules,
	sizeof(SimpleBorderRules)/sizeof(SimpleBorderRules[0]),
	SimpleBorderRules,
	sizeof(SimpleAlignmentRules)/sizeof(SimpleAlignmentRules[0]),
	SimpleAlignmentRules,
	sizeof(SimpleFontRules)/sizeof(SimpleFontRules[0]),
	SimpleFontRules
};

static CRuleBasedTableFormatter SimpleTableFormatter(IDS_SimpleTableFormat, IDS_TableEntries1, &SimpleRuleSet);

/////////////////////////////////////////////////////////////////////////////
// "Checkbook" table formatter.

// The border rules (the same for both H and V).
static CTableBorderSegmentRule CheckbookBorderRules[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The fill rules.
static CTableCellInfoRule CheckbookCellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ODD, COLOR_FROM_COLORREF(RGB(240, 240, 240))),
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_EVEN, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule CheckbookAlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_left, ALIGN_top)
};
// The font rules.
static CTableCellFontRule CheckbookFontRules[] =
{
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet CheckbookRuleSet =
{
	sizeof(CheckbookCellRules)/sizeof(CheckbookCellRules[0]),
	CheckbookCellRules,
	sizeof(CheckbookBorderRules)/sizeof(CheckbookBorderRules[0]),
	CheckbookBorderRules,
	sizeof(CheckbookBorderRules)/sizeof(CheckbookBorderRules[0]),
	CheckbookBorderRules,
	sizeof(CheckbookAlignmentRules)/sizeof(CheckbookAlignmentRules[0]),
	CheckbookAlignmentRules,
	sizeof(CheckbookFontRules)/sizeof(CheckbookFontRules[0]),
	CheckbookFontRules
};

static CRuleBasedTableFormatter CheckbookTableFormatter(IDS_CheckbookTableFormat, IDS_TableEntries1, &CheckbookRuleSet);

/////////////////////////////////////////////////////////////////////////////
// "Totals" table formatter.
// The fill rules.
static CTableCellInfoRule TotalsCellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The border rules for H.
static CTableBorderSegmentRule TotalsBorderRulesH[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 1,                          BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_None,  LINE_THIN, COLOR_BLACK)
};
// The border rules for V.
static CTableBorderSegmentRule TotalsBorderRulesV[] =
{
	CTableBorderSegmentRule(1,                          CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_None,  LINE_THIN, COLOR_BLACK)
};
// The alignment rules.
static CTableCellAlignmentRule TotalsAlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule TotalsFontRules[] =
{
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet TotalsRuleSet =
{
	sizeof(TotalsCellRules)/sizeof(TotalsCellRules[0]),
	TotalsCellRules,
	sizeof(TotalsBorderRulesH)/sizeof(TotalsBorderRulesH[0]),
	TotalsBorderRulesH,
	sizeof(TotalsBorderRulesV)/sizeof(TotalsBorderRulesV[0]),
	TotalsBorderRulesV,
	sizeof(TotalsAlignmentRules)/sizeof(TotalsAlignmentRules[0]),
	TotalsAlignmentRules,
	sizeof(TotalsFontRules)/sizeof(TotalsFontRules[0]),
	TotalsFontRules
};

static CRuleBasedTableFormatter TotalsTableFormatter(IDS_TableFormatTotals, IDS_TableEntries1, &TotalsRuleSet);

/////////////////////////////////////////////////////////////////////////////
// "SimpleGrid1" table formatter.
// The border rules (the same for both H and V).
static CTableBorderSegmentRule SimpleGrid1BorderRules[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The fill rules.
static CTableCellInfoRule SimpleGrid1CellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule SimpleGrid1AlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule SimpleGrid1FontRules[] =
{
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet SimpleGrid1RuleSet =
{
	sizeof(SimpleGrid1CellRules)/sizeof(SimpleGrid1CellRules[0]),
	SimpleGrid1CellRules,
	sizeof(SimpleGrid1BorderRules)/sizeof(SimpleGrid1BorderRules[0]),
	SimpleGrid1BorderRules,
	sizeof(SimpleGrid1BorderRules)/sizeof(SimpleGrid1BorderRules[0]),
	SimpleGrid1BorderRules,
	sizeof(SimpleGrid1AlignmentRules)/sizeof(SimpleGrid1AlignmentRules[0]),
	SimpleGrid1AlignmentRules,
	sizeof(SimpleGrid1FontRules)/sizeof(SimpleGrid1FontRules[0]),
	SimpleGrid1FontRules
};

static CRuleBasedTableFormatter SimpleGrid1TableFormatter(IDS_TableFormatSimpleGrid1, IDS_TableEntries1, &SimpleGrid1RuleSet);

/////////////////////////////////////////////////////////////////////////////
// "SimpleGrid2" table formatter.
// The border rules for H.
static CTableBorderSegmentRule SimpleGrid2BorderRulesH[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 0,                          BORDER_SEGMENT_None,  LINE_THIN, COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The border rules for V.
static CTableBorderSegmentRule SimpleGrid2BorderRulesV[] =
{
	CTableBorderSegmentRule(0,                          CTableFormatRule::CELL_ANY, BORDER_SEGMENT_None,  LINE_THIN, COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The fill rules.
static CTableCellInfoRule SimpleGrid2CellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule SimpleGrid2AlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule SimpleGrid2FontRules[] =
{
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet SimpleGrid2RuleSet =
{
	sizeof(SimpleGrid2CellRules)/sizeof(SimpleGrid2CellRules[0]),
	SimpleGrid2CellRules,
	sizeof(SimpleGrid2BorderRulesH)/sizeof(SimpleGrid2BorderRulesH[0]),
	SimpleGrid2BorderRulesH,
	sizeof(SimpleGrid2BorderRulesV)/sizeof(SimpleGrid2BorderRulesV[0]),
	SimpleGrid2BorderRulesV,
	sizeof(SimpleGrid2AlignmentRules)/sizeof(SimpleGrid2AlignmentRules[0]),
	SimpleGrid2AlignmentRules,
	sizeof(SimpleGrid2FontRules)/sizeof(SimpleGrid2FontRules[0]),
	SimpleGrid2FontRules
};

static CRuleBasedTableFormatter SimpleGrid2TableFormatter(IDS_TableFormatSimpleGrid2, IDS_TableEntries1, &SimpleGrid2RuleSet);

/////////////////////////////////////////////////////////////////////////////
// "SimpleGrid3" table formatter.
// The border rules for H.
static CTableBorderSegmentRule SimpleGrid3BorderRulesH[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 0,                           BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 1,                           BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_LAST, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY,  BORDER_SEGMENT_None,  LINE_THIN, COLOR_BLACK)
};
// The border rules for V.
static CTableBorderSegmentRule SimpleGrid3BorderRulesV[] =
{
	CTableBorderSegmentRule(0,                           CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(1,                           CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_LAST, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY,  CTableFormatRule::CELL_ANY, BORDER_SEGMENT_None,  LINE_THIN, COLOR_BLACK)
};
// The fill rules.
static CTableCellInfoRule SimpleGrid3CellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule SimpleGrid3AlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule SimpleGrid3FontRules[] =
{
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet SimpleGrid3RuleSet =
{
	sizeof(SimpleGrid3CellRules)/sizeof(SimpleGrid3CellRules[0]),
	SimpleGrid3CellRules,
	sizeof(SimpleGrid3BorderRulesH)/sizeof(SimpleGrid3BorderRulesH[0]),
	SimpleGrid3BorderRulesH,
	sizeof(SimpleGrid3BorderRulesV)/sizeof(SimpleGrid3BorderRulesV[0]),
	SimpleGrid3BorderRulesV,
	sizeof(SimpleGrid3AlignmentRules)/sizeof(SimpleGrid3AlignmentRules[0]),
	SimpleGrid3AlignmentRules,
	sizeof(SimpleGrid3FontRules)/sizeof(SimpleGrid3FontRules[0]),
	SimpleGrid3FontRules
};

static CRuleBasedTableFormatter SimpleGrid3TableFormatter(IDS_TableFormatSimpleGrid3, IDS_TableEntries1, &SimpleGrid3RuleSet);

/////////////////////////////////////////////////////////////////////////////
// "SimpleGrid4" table formatter.
// The border rules for H.
static CTableBorderSegmentRule SimpleGrid4BorderRulesH[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 0,                           BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 1,                           BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_LAST, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY,  BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The border rules for V.
static CTableBorderSegmentRule SimpleGrid4BorderRulesV[] =
{
	CTableBorderSegmentRule(0,                           CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_LAST, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY,  CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The fill rules.
static CTableCellInfoRule SimpleGrid4CellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule SimpleGrid4AlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule SimpleGrid4FontRules[] =
{
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet SimpleGrid4RuleSet =
{
	sizeof(SimpleGrid4CellRules)/sizeof(SimpleGrid4CellRules[0]),
	SimpleGrid4CellRules,
	sizeof(SimpleGrid4BorderRulesH)/sizeof(SimpleGrid4BorderRulesH[0]),
	SimpleGrid4BorderRulesH,
	sizeof(SimpleGrid4BorderRulesV)/sizeof(SimpleGrid4BorderRulesV[0]),
	SimpleGrid4BorderRulesV,
	sizeof(SimpleGrid4AlignmentRules)/sizeof(SimpleGrid4AlignmentRules[0]),
	SimpleGrid4AlignmentRules,
	sizeof(SimpleGrid4FontRules)/sizeof(SimpleGrid4FontRules[0]),
	SimpleGrid4FontRules
};

static CRuleBasedTableFormatter SimpleGrid4TableFormatter(IDS_TableFormatSimpleGrid4, IDS_TableEntries1, &SimpleGrid4RuleSet);

/////////////////////////////////////////////////////////////////////////////
// "ColorGrid" table formatter.
// The border rules (the same for both H and V).
static CTableBorderSegmentRule ColorGridBorderRules[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_GRID1)
};
// The fill rules.
static CTableCellInfoRule ColorGridCellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, 0,									COLOR_GRID1),
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule ColorGridAlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule ColorGridFontRules[] =
{
	CTableCellFontRule(0,                          0,									TRUE,  FALSE, COLOR_WHITE),
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_WHITE),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet ColorGridRuleSet =
{
	sizeof(ColorGridCellRules)/sizeof(ColorGridCellRules[0]),
	ColorGridCellRules,
	sizeof(ColorGridBorderRules)/sizeof(ColorGridBorderRules[0]),
	ColorGridBorderRules,
	sizeof(ColorGridBorderRules)/sizeof(ColorGridBorderRules[0]),
	ColorGridBorderRules,
	sizeof(ColorGridAlignmentRules)/sizeof(ColorGridAlignmentRules[0]),
	ColorGridAlignmentRules,
	sizeof(ColorGridFontRules)/sizeof(ColorGridFontRules[0]),
	ColorGridFontRules
};

static CRuleBasedTableFormatter ColorGridTableFormatter(IDS_TableFormatColorGrid, IDS_TableEntries1, &ColorGridRuleSet);

/////////////////////////////////////////////////////////////////////////////
// "Rows2" table formatter.
// The border rules for H.
static CTableBorderSegmentRule Rows2BorderRulesH[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 0,                          BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 1,                          BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The border rules for V.
static CTableBorderSegmentRule Rows2BorderRulesV[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_None, 0, 0)
};
// The fill rules.
static CTableCellInfoRule Rows2CellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule Rows2AlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule Rows2FontRules[] =
{
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet Rows2RuleSet =
{
	sizeof(Rows2CellRules)/sizeof(Rows2CellRules[0]),
	Rows2CellRules,
	sizeof(Rows2BorderRulesH)/sizeof(Rows2BorderRulesH[0]),
	Rows2BorderRulesH,
	sizeof(Rows2BorderRulesV)/sizeof(Rows2BorderRulesV[0]),
	Rows2BorderRulesV,
	sizeof(Rows2AlignmentRules)/sizeof(Rows2AlignmentRules[0]),
	Rows2AlignmentRules,
	sizeof(Rows2FontRules)/sizeof(Rows2FontRules[0]),
	Rows2FontRules
};

static CRuleBasedTableFormatter Rows2TableFormatter(IDS_TableFormatRows2, IDS_TableEntries1, &Rows2RuleSet);

/////////////////////////////////////////////////////////////////////////////
// "Rows1" table formatter.
// The border rules for H.
static CTableBorderSegmentRule Rows1BorderRulesH[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 0,                           BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 1,                           BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_LAST, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY,  BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The border rules for V.
static CTableBorderSegmentRule Rows1BorderRulesV[] =
{
	CTableBorderSegmentRule(0,                           CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_LAST, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY,  CTableFormatRule::CELL_ANY, BORDER_SEGMENT_None,  0,         0)
};
// The fill rules.
static CTableCellInfoRule Rows1CellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, 0,									COLOR_ROWS1),
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule Rows1AlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule Rows1FontRules[] =
{
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet Rows1RuleSet =
{
	sizeof(Rows1CellRules)/sizeof(Rows1CellRules[0]),
	Rows1CellRules,
	sizeof(Rows1BorderRulesH)/sizeof(Rows1BorderRulesH[0]),
	Rows1BorderRulesH,
	sizeof(Rows1BorderRulesV)/sizeof(Rows1BorderRulesV[0]),
	Rows1BorderRulesV,
	sizeof(Rows1AlignmentRules)/sizeof(Rows1AlignmentRules[0]),
	Rows1AlignmentRules,
	sizeof(Rows1FontRules)/sizeof(Rows1FontRules[0]),
	Rows1FontRules
};

static CRuleBasedTableFormatter Rows1TableFormatter(IDS_TableFormatRows1, IDS_TableEntries1, &Rows1RuleSet);

/////////////////////////////////////////////////////////////////////////////
// "SimpleRow" table formatter.
// The border rules for H.
static CTableBorderSegmentRule SimpleRowBorderRulesH[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, 1,                          BORDER_SEGMENT_Solid, LINE_FAT,  COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The border rules for V.
static CTableBorderSegmentRule SimpleRowBorderRulesV[] =
{
	CTableBorderSegmentRule(0,                           CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_LAST, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK),
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY,  CTableFormatRule::CELL_ANY, BORDER_SEGMENT_None, 0, 0)
};
// The fill rules.
static CTableCellInfoRule SimpleRowCellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule SimpleRowAlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule SimpleRowFontRules[] =
{
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet SimpleRowRuleSet =
{
	sizeof(SimpleRowCellRules)/sizeof(SimpleRowCellRules[0]),
	SimpleRowCellRules,
	sizeof(SimpleRowBorderRulesH)/sizeof(SimpleRowBorderRulesH[0]),
	SimpleRowBorderRulesH,
	sizeof(SimpleRowBorderRulesV)/sizeof(SimpleRowBorderRulesV[0]),
	SimpleRowBorderRulesV,
	sizeof(SimpleRowAlignmentRules)/sizeof(SimpleRowAlignmentRules[0]),
	SimpleRowAlignmentRules,
	sizeof(SimpleRowFontRules)/sizeof(SimpleRowFontRules[0]),
	SimpleRowFontRules
};

static CRuleBasedTableFormatter SimpleRowTableFormatter(IDS_TableFormatSimpleRow, IDS_TableEntries1, &SimpleRowRuleSet);

/////////////////////////////////////////////////////////////////////////////
// "SimpleGrid5" table formatter.
// The border rules (the same for both H and V).
static CTableBorderSegmentRule SimpleGrid5BorderRules[] =
{
	CTableBorderSegmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, BORDER_SEGMENT_Solid, LINE_THIN, COLOR_BLACK)
};
// The fill rules.
static CTableCellInfoRule SimpleGrid5CellRules[] =
{
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, 0,									COLOR_BLACK),
	CTableCellInfoRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, COLOR_WHITE)
};
// The alignment rules.
static CTableCellAlignmentRule SimpleGrid5AlignmentRules[] =
{
	CTableCellAlignmentRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, ALIGN_center, ALIGN_middle)
};
// The font rules.
static CTableCellFontRule SimpleGrid5FontRules[] =
{
	CTableCellFontRule(0,                          0,									TRUE,  FALSE, COLOR_WHITE),
	CTableCellFontRule(0,                          CTableFormatRule::CELL_ANY, TRUE,  FALSE, COLOR_BLACK),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, 0,									TRUE,  FALSE, COLOR_WHITE),
	CTableCellFontRule(CTableFormatRule::CELL_ANY, CTableFormatRule::CELL_ANY, FALSE, FALSE, COLOR_BLACK)
};

// The rule set.
static CTableFormatRuleSet SimpleGrid5RuleSet =
{
	sizeof(SimpleGrid5CellRules)/sizeof(SimpleGrid5CellRules[0]),
	SimpleGrid5CellRules,
	sizeof(SimpleGrid5BorderRules)/sizeof(SimpleGrid5BorderRules[0]),
	SimpleGrid5BorderRules,
	sizeof(SimpleGrid5BorderRules)/sizeof(SimpleGrid5BorderRules[0]),
	SimpleGrid5BorderRules,
	sizeof(SimpleGrid5AlignmentRules)/sizeof(SimpleGrid5AlignmentRules[0]),
	SimpleGrid5AlignmentRules,
	sizeof(SimpleGrid5FontRules)/sizeof(SimpleGrid5FontRules[0]),
	SimpleGrid5FontRules
};

static CRuleBasedTableFormatter SimpleGrid5TableFormatter(IDS_TableFormatSimpleGrid5, IDS_TableEntries1, &SimpleGrid5RuleSet);

/////////////////////////////////////////////////////////////////////////////
// The table formatters.

CTableFormatter* TableFormatters[] =
{
	&NoneTableFormatter,
	&SimpleTableFormatter,
	&CheckbookTableFormatter,
	&TotalsTableFormatter,
	&SimpleGrid1TableFormatter,
	&SimpleGrid2TableFormatter,
	&SimpleGrid3TableFormatter,
	&SimpleGrid4TableFormatter,
	&SimpleGrid5TableFormatter,
	&ColorGridTableFormatter,
	&SimpleRowTableFormatter,
	&Rows1TableFormatter,
	&Rows2TableFormatter,
	NULL
};

CTableFormatter** CTableFormatter::GetTableFormatters(void)
{
	return TableFormatters;
}
