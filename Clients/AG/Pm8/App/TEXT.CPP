/*
// $Header: /PM8/App/TEXT.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TEXT.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 17    2/01/99 2:23p Cschendel
// fixed BulletIndent for multilevel lists
// 
// 16    1/21/99 3:09p Cschendel
// added special handling for the bullet font type face ID in = operator
// method
// 
// 15    1/14/99 9:24a Cschende
// added  PreviousParagraphStart() function
// 
// 14    12/15/98 5:44p Cschende
// played with the bullet indent logic
// 
// 13    12/10/98 4:11p Cschende
// Added bullet, numbered list and multilevel list support in text
// 
// 12    6/03/98 1:43p Jayn
// Added MakeTextUndefined()
// 
// 11    4/30/98 4:48p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 10    4/29/98 6:32p Rlovejoy
// Updated for new fill format.
// 
// 9     4/16/98 4:03p Jayn
// Fixed "disappearing hyperlink" problem when text attribute changed.
// 
// 8     4/07/98 8:43a Jayn
// Eliminated extra incs on face and hyperlink records.
// 
// 7     4/01/98 5:31p Johno
// Added #include "hyperrec.h" and #include "htmldoc.h"
// for new header file scheme to reduce compiles
// 
// 6     3/31/98 10:16a Johno
// More changes for reference counts
// 
// 5     3/27/98 5:48p Johno
// Changes in CTextStyle::operator =  for reference counts
// 
// 4     3/25/98 6:37p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 3     3/25/98 4:03p Johno
// Added GetHyperlinkData
 * 
 * 2     10/30/97 5:20p Dennis
 * Added NextParagraphStart() and GetTextRecord() methods.
// 
//    Rev 1.0   14 Aug 1997 15:26:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:42   Fred
// Initial revision.
// 
//    Rev 1.31   15 Jul 1997 10:05:30   Jay
// Fixed last fix.
// 
//    Rev 1.30   15 Jul 1997 09:32:58   Jay
// Fixed OldValue for size style changes.
// 
//    Rev 1.29   07 Jul 1997 10:44:56   Jay
// Fixes old style changes
// 
//    Rev 1.28   26 Jun 1997 16:14:56   Jay
// Fixed old text style changes
// 
//    Rev 1.27   29 May 1997 08:53:02   Fred
// Improved support for old 3.0 effects
// 
//    Rev 1.26   23 May 1997 17:09:44   Fred
// New effects
// 
//    Rev 1.25   15 May 1997 16:53:44   Fred
// New text effects
// 
//    Rev 1.24   09 Apr 1997 14:51:40   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.23   03 Apr 1997 09:31:28   Fred
// More text cleanup
// 
//    Rev 1.22   26 Mar 1997 15:57:46   Fred
// Start of new effects
// 
//    Rev 1.21   06 Jan 1997 10:13:36   Jay
// Optimizations
// 
//    Rev 1.20   01 Sep 1996 09:23:12   Jay
//  
// 
//    Rev 1.19   10 Jul 1996 10:25:48   Jay
// Text states no longer allocated.
// 
//    Rev 1.18   05 Jul 1996 10:47:12   Jay
//  
// 
//    Rev 1.17   03 Jul 1996 14:30:30   Jay
//  
// 
//    Rev 1.16   02 Jul 1996 08:55:08   Jay
//  
// 
//    Rev 1.15   28 Jun 1996 16:18:18   Jay
//  
// 
//    Rev 1.14   27 Jun 1996 17:20:48   Jay
//  
// 
//    Rev 1.13   27 Jun 1996 13:27:42   Jay
//  
// 
//    Rev 1.12   26 Jun 1996 09:06:54   Jay
//  
// 
//    Rev 1.11   21 Jun 1996 14:30:28   Jay
// Stretch text, etc.
// 
//    Rev 1.10   20 Jun 1996 17:40:02   Jay
// Some speedups and diagnostics.
// 
//    Rev 1.9   19 Jun 1996 13:45:50   Jay
//  
// 
//    Rev 1.8   18 Jun 1996 16:14:50   Jay
//  
// 
//    Rev 1.7   17 Jun 1996 12:01:26   Jay
//  
// 
//    Rev 1.6   14 Jun 1996 17:05:40   Jay
//  
// 
//    Rev 1.5   13 Jun 1996 17:46:08   Jay
//  
// 
//    Rev 1.4   13 Jun 1996 15:53:08   Jay
//  
// 
//    Rev 1.3   04 Jun 1996 17:38:20   Jay
//  
// 
//    Rev 1.2   24 May 1996 09:27:02   Jay
// New text code.
// 
//    Rev 1.1   23 Apr 1996 08:07:48   Jay
// More new stuff
// 
//    Rev 1.0   12 Apr 1996 13:08:54   Jay
// New text stuff
*/

#include "stdafx.h"
#include "text.h"
#include "textrec.h"
#include "pmgdb.h"
#include "frameobj.h"
#include "utils.h"
#include "pmgfont.h"
#include "style.h"
#include "stylehlp.h"
#include "fcache.h"
#include "pmgfont.h"
#include "hyperrec.h"
#include "htmldoc.h"
/////////////////////////////////////////////////////////////////////////////
// CTextParagraph

CTextParagraph::CTextParagraph()
{
	memset(this, 0, sizeof(*this));
	m_StyleChange.m_wToken = TOKEN_Paragraph;
}

/////////////////////////////////////////////////////////////////////////////
// CTextTab

/*
// Tabs are stored in style changes sometimes.
// The mapping is:
//
// Tab							Style change
// --------------------------------------------
// m_nType						LOWORD(m_lOldValue)
// m_DecimalCharacter		HIWORD(m_lOldValue)
// m_Position					m_lValue
// m_Leader[0]					LOWORD(m_lCharacters)
// m_Leader[1]					HIWORD(m_lCharacters)
//
// You don't need to remember this. Just use the To and From functions
// below.
*/

// Set the default values for this tab.
void CTextTab::Default(void)
{
	m_nType = TAB_left;
	m_DecimalCharacter = '.';
	m_Position = -1;
	m_Leader[0] = 
		m_Leader[1] = 0;
}

// Store this tab stop in the passed style change.
void CTextTab::ToStyleChange(CTextStyleChange* pChange) const
{
	pChange->m_wToken = TOKEN_TabStop;
	pChange->m_lOldValue = MAKELONG((WORD)m_nType, (WORD)m_DecimalCharacter);
	pChange->m_lValue = m_Position;
	pChange->m_lCharacters = MAKELONG((WORD)m_Leader[0], (WORD)m_Leader[1]);
}

// Extract this tab stop from the passed style change.
void CTextTab::FromStyleChange(const CTextStyleChange* pChange)
{
	ASSERT(pChange->m_wToken == TOKEN_TabStop);
	m_nType = (SHORT)LOWORD(pChange->m_lOldValue);
	m_DecimalCharacter = (CHARACTER)HIWORD(pChange->m_lOldValue);
	m_Position = pChange->m_lValue;
	m_Leader[0] = (CHARACTER)LOWORD(pChange->m_lCharacters);
	m_Leader[1] = (CHARACTER)HIWORD(pChange->m_lCharacters);
}

// Compute the position of this tab stop.
PCOORD CTextTab::ComputePosition(const PBOX& Extent, PCOORD Position)
{
	PCOORD BasePosition;
	PCOORD BaseDimension;

	switch (m_nType)
	{
		case TAB_left:
		case TAB_center:
		case TAB_right:
		case TAB_decimal:
		case TAB_repeating:
		{
         /*
         // Horizontal tab:  The base position is the left edge of the extent
         // and the base dimension is the width of the extent.
         */

			BasePosition = Extent.x0;
			BaseDimension = Extent.x1-BasePosition;

			break;
		}
		default:
		{
			// Invalid tab type!
			ASSERT(FALSE);
			return 0;
		}
	}

	/*
	// If the tab is a repeating tab, compute the offset we want.
	*/

	PCOORD AnchorOffset = (PCOORD)MulFixed(m_Position, PAGE_RESOLUTION);

	if (m_nType == TAB_repeating)
	{
		Position -= BasePosition;

		if (AnchorOffset > 0)
		{
			AnchorOffset = AnchorOffset * ((Position/AnchorOffset)+1);
		}

#if 0
		if ((BasePosition+AnchorOffset) > Extent.x1)
		{
			AnchorOffset = Position;
		}
#endif
   }

   // Return the absolute position of the tab stop.

	return BasePosition+AnchorOffset;
}

/////////////////////////////////////////////////////////////////////////////
// CTabArray

CTabArray::CTabArray() : Array(sizeof(CTextTab))
{
	m_Record.m_lDefaultTabSize = MakeFixed(1)/2;		// 1/2 inch.
}

CTabArray& CTabArray::operator = (const CTabArray &Array)
{
	// Assign the base array.
	assign(Array);

	// Assign the extras.
	DefaultTabSize(Array.DefaultTabSize());

	// Return this.
	return *this;
}

/*
// Get a tab given its position.
*/
int CTabArray::GetTabAt(CFixed Position)
{
	int nTabs = NumberOfTabs();
	for (int nTab = 0; nTab < nTabs; nTab++)
	{
		// Get the tab to be checked..
		CTextTab* pTab = GetAt(nTab);

		// Compare the position.
		if (pTab->m_Position == Position)
		{
			return nTab;
		}
	}
	return -1;
}

/*
// Remove all tabs at the passed position.
// If Position == UNDEFINED_CFIXED, then all tabstops are removed.
*/

void CTabArray::RemoveTabAt(CFixed Position)
{
	// Special case the "remove all" case for speed.
	if (Position == UNDEFINED_CFIXED)
	{
		RemoveAll();
	}
	else
	{
		for (int nTab = 0; nTab < NumberOfTabs(); )
		{
			// Get the tab to be checked..
			CTextTab* pTab = GetAt(nTab);

			// Compare the position.
			if (pTab->m_Position == Position)
			{
				// Positions match. Remove this tab, and leave the index here.
				RemoveAt(nTab);
			}
			else
			{
				// Positions do not match. Advance to the next tab.
				nTab++;
			}
		}
	}
}

/*
// Find a tab given an x offset and an extent.
// The extent is the anchor extent for the tab, usually the frame.
*/

BOOL CTabArray::FindTab(const PBOX& Extent, PCOORD Position, CTextTab* pResultTab)
{
	int nFoundTab = -1;
	PCOORD TabDelta;

	// Look for a tab with position beyond the passed Position.
	// Look for the closest such tab.

	int nTabs = NumberOfTabs();
	for (int nTab = 0; nTab < nTabs; nTab++)
	{
		CTextTab* pTab = GetAt(nTab);
		PCOORD Delta;
		if ((Delta = pTab->ComputePosition(Extent, Position) - Position) > 0)
		{
			/*
			// Check if the current tab stop is closer than all the previous
			// tab stops.
			//
			// Note: If two or more tab stops are positioned at the same
			// location, and the location is the closest match to the given
			// position, then the first tab stop in the tab stop array will
			// be found.
			*/

			if ((nFoundTab == -1) || (Delta < TabDelta))
			{
				nFoundTab = nTab;
				TabDelta = Delta;
			}
		}
	}

	// The search is over. If we found a tab, return it.
	if (nFoundTab != -1)
	{
		*pResultTab = *(GetAt(nFoundTab));
		return TRUE;
	}

	// We did not find a tab. See if a default tabstop will suffice.
	CTextTab Tab;
	memset(&Tab, 0, sizeof(Tab));
	Tab.m_nType = CTextTab::TAB_repeating;
	Tab.m_Position = DefaultTabSize();
	PCOORD Delta;
	if ((Delta = Tab.ComputePosition(Extent, Position) - Position) > 0)
	{
		*pResultTab = Tab;
		return TRUE;
	}

	// No tab sufficed. Return FALSE.
	return FALSE;
}

ERRORCODE CTabArray::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error;

	if ((error = read(pDevice)) == ERRORCODE_None)
	{
		error = pDevice->read_record(&m_Record, sizeof(m_Record));
	}
	return error;
}

ERRORCODE CTabArray::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error;

	if ((error = write(pDevice)) == ERRORCODE_None)
	{
		error = pDevice->write_record(&m_Record, sizeof(m_Record));
	}
	return error;
}

ST_MAN_SIZE CTabArray::SizeofData(StorageDevice* pDevice)
{
	return size(pDevice) + pDevice->size_record(sizeof(m_Record));
}

/////////////////////////////////////////////////////////////////////////////
// CTextStyle

// Constructor.

CTextStyle::CTextStyle(PMGDatabase* pDatabase /*=NULL*/, BOOL fTemporary /*=TRUE*/)
{
	Database(pDatabase);
	m_fTemporary = fTemporary;

	memset(&m_Paragraph, 0, sizeof(m_Paragraph));
	memset(&m_Character, 0, sizeof(m_Character));
	memset(&m_Metrics, 0, sizeof(m_Metrics));
	memset(&m_Frame, 0, sizeof(m_Frame));
}

CTextStyle::~CTextStyle()
{
	if (m_fTemporary)
	{
		Destroy();
	}
}

/*
// Copy constructor.
*/

CTextStyle::CTextStyle(const CTextStyle far &s)
{
	m_pDatabase = NULL;
	m_fTemporary = TRUE;						// This should cover most cases.

	*this = s;                          
}

/*
// Return the index in the style helper table for the token passed.
*/

int CTextStyle::TokenToIndex(int nToken) const
{
	int nIndex = (nToken & TOKEN_Mask);
	if (nIndex >= FIRST_TEXT_TOKEN && nIndex < LAST_TEXT_TOKEN)
	{
		nIndex -= FIRST_TEXT_TOKEN;
	}
	else if (nIndex >= FIRST_PARAGRAPH_TOKEN && nIndex < LAST_PARAGRAPH_TOKEN)
	{
		nIndex -= (FIRST_PARAGRAPH_TOKEN-LAST_TEXT_TOKEN);
	}
	else
	{
		nIndex = -1;
		TRACE("Bad token: %x\n", nToken);
	}
	return nIndex;
}

/*
// Apply a style change.
*/

void CTextStyle::ApplyChange(const CTextStyleChange* pStyleChange, BOOL fFixStyle /*=TRUE*/)
{
	int nIndex = TokenToIndex(pStyleChange->m_wToken);
	if (nIndex != -1)
	{
		if (!(pStyleChange->m_wToken & TOKEN_Overridden))
		{
			StyleHelpers[nIndex]->ApplyChange(this, pStyleChange);
			if (fFixStyle)
			{
				UpdateFontMetrics();
			}
		}
	}
}

/*
// Build a style change. 'this' is the old style.
*/

void CTextStyle::BuildChange(CTextStyleChange* pStyleChange, const CTextStyle* pNewStyle) const
{
	int nIndex = TokenToIndex(pStyleChange->m_wToken);
	if (nIndex != -1)
	{
		StyleHelpers[nIndex]->BuildChange(pStyleChange, pNewStyle, this);
	}
}

/*
// Say whether the style is defined or undefined for this token.
*/

BOOL CTextStyle::TestStyle(int nToken) const
{
	int nIndex = TokenToIndex(nToken);
	if (nIndex == -1)
	{
		// Not defined.
		return FALSE;
	}
	return StyleHelpers[nIndex]->TestStyle(this);
}

/*
// Say whether the passed style matches us for this token.
*/

BOOL CTextStyle::CompareStyles(int nToken, const CTextStyle* pStyle) const
{
	int nIndex = TokenToIndex(nToken);
	if (nIndex == -1)
	{
		// Alway matches
		return TRUE;
	}
	return StyleHelpers[nIndex]->CompareStyles(this, pStyle);
}

/*
// Make a style attribute undefined.
*/

void CTextStyle::MakeUndefined(int nToken)
{
	int nIndex = TokenToIndex(nToken);
	if (nIndex != -1)
	{
		StyleHelpers[nIndex]->MakeUndefined(this);
	}
}

/*
// Make all text attributes be undefined.
*/

void CTextStyle::MakeTextUndefined(void)
{
	for (int nToken = FIRST_TEXT_TOKEN; nToken < LAST_TEXT_TOKEN; nToken++)
	{
		MakeUndefined(nToken);
	}
}

/*
// Apply an old style change.
*/

void CTextStyle::ApplyOldChange(const STYLE_CHANGE* pStyleChange, BOOL fFixStyle /*=TRUE*/)
{
	int nIndex = TokenToIndex(pStyleChange->token);
	if (nIndex != -1)
	{
		if (!(pStyleChange->token & TOKEN_Overridden))
		{
			StyleHelpers[nIndex]->ApplyOldChange(this, pStyleChange);
			if (fFixStyle)
			{
				UpdateFontMetrics();
			}
		}
	}
}

/*
// Make the paragraph section undefined.
*/

void CTextStyle::MakeParagraphUndefined(void)
{
	for (int nToken = FIRST_PARAGRAPH_TOKEN; nToken < LAST_PARAGRAPH_TOKEN; nToken++)
	{
		StyleHelpers[TokenToIndex(nToken)]->MakeUndefined(this);
	}
}

/*
// Set the font for this style.
*/

void CTextStyle::Font(DB_RECORD_NUMBER lFont)
{
	m_pDatabase->free_face(m_Character.m_lFont, m_fTemporary);
	m_pDatabase->inc_face(m_Character.m_lFont = lFont, m_fTemporary);
}

/*
// Reset this text style.
// This should remove all references.
*/

void CTextStyle::Reset(void)
{
	Font(0); 
   HyperlinkStyle(0);
}

/*
// This text style is being destroyed.
// Remove all references.
*/

void CTextStyle::Destroy(void)
{
	Reset();
}

/*
// Convert from an old style.
*/

void CTextStyle::FromOldStyle(const TextStyle& style)
{
	// Set some defaults.
	SetDefault();

	// Copy the style information over.
	Font(style.get_face());
	Size(MakeFixed(style.get_size(), style.get_size_fraction()));
	BaseSize(MakeFixed(style.get_base_size(), style.get_base_size_fraction()));
	Expansion(DivFixed(MakeFixed(style.get_base_size(), style.get_base_size_fraction()),
							 MakeFixed(FONT_EXPANSION_UNIT)));
	Fill(style.get_pattern(), style.get_color());
	Outline(style.get_outline(), style.get_color());
	Shadow(style.get_shadow(), style.get_color());
	m_Character.m_nEffectsVersion = 1;
	XFlipped(style.get_xflipped());
	YFlipped(style.get_yflipped());
//	Color(style.get_color());

	Alignment(style.get_line_alignment());
	VerticalAlignment(style.get_vertical_alignment());
	// Left and right margin should be zero (default) unless set by user.
	// This fixes a problem converting old warp text boxes - they should
	// always have zero margins!
	LeftMargin(0);
	RightMargin(0);
//	LeftMargin(PageToInches(style.get_left_margin()));
//	RightMargin(PageToInches(style.get_right_margin()));
	LeadingType(LEADING_lines);
	Leading(MakeFixed(0.875));

	Underline(style.UnderlineStyle());

	// Update our metrics.
	UpdateFontMetrics();
}

void CTextStyle::SetDefault(void)
{
	m_pDatabase->get_font_server()->SetDefault(this);
}

/*
// Update our font metrics.
*/

void CTextStyle::UpdateFontMetrics(void)
{
	if (Size() < MakeFixed(1))
	{
		Size(MakeFixed(1));
	}
	m_pDatabase->get_font_server()->FontMetrics(this);
}

CFixed CTextStyle::GetEffectsRotation(void)
{
	ASSERT(m_pFrame != NULL);
	return MakeFixed(m_pFrame->get_rotation()*RAD2DEGC);
}

/*
// The assignment operator.
// This is necessary to handle reference counts.
//
// Assignment of a style with a different database will operate correctly.
// (But the style will be owned by the new database afterwards).
//
// Added same logic to teh bullet style for regular text styles
*/

CTextStyle& CTextStyle::operator = (const CTextStyle& s)
{
	BOOL fSameDatabase = (m_pDatabase == s.m_pDatabase);
	BOOL fSameFont = fSameDatabase && (s.Font() == Font());

	BOOL fSameBulletFont = fSameDatabase && (s.BulletFont() == BulletFont());

	BOOL fSameHyperlink = fSameDatabase && (s.HyperlinkStyle() == HyperlinkStyle());


   // Free any old face.

	if (!fSameFont && m_pDatabase != NULL)
	{
		// Free the old face
		if (Font() > 0)
		{
			m_pDatabase->free_face(Font(), m_fTemporary);
		}
	}

	if (!fSameBulletFont && m_pDatabase != NULL)
	{
		// Free the old face
		if (BulletFont() > 0)
		{
			m_pDatabase->free_face(BulletFont(), m_fTemporary);
		}
	}

   // Free any old Hyperlink.

	if (!fSameHyperlink && m_pDatabase != NULL)
	{
		// Free the old face
		if (HyperlinkStyle() > 0)
		{
			m_pDatabase->FreeHyperlinkRecord(HyperlinkStyle(), m_fTemporary);
		}
	}

	// Copy the data.

	m_Paragraph = s.m_Paragraph;
	m_Tabs = s.m_Tabs;
	m_Character = s.m_Character;
	m_Metrics = s.m_Metrics;
	m_Frame = s.m_Frame;

	// Do we copy the database? I think so.

	if (m_fTemporary || m_pDatabase == NULL)
	{
		// Temporary or uninitialized styles always assume the database assigned.
		m_pDatabase = s.m_pDatabase;
	}

	if (fSameDatabase)
	{
		// Increment the new face ref count.
		if (!fSameFont)
		{
			if (Font() > 0)
			{
				m_pDatabase->inc_face(Font(), m_fTemporary);
			}
         
		}
      
		if (!fSameBulletFont)
		{
			if (BulletFont() > 0)
			{
				m_pDatabase->inc_face(BulletFont(), m_fTemporary);
			}
         
		}

      if (!fSameHyperlink)
      {
         if (HyperlinkStyle() > 0)
         {
            m_pDatabase->IncrementHyperlinkRecord(HyperlinkStyle(), m_fTemporary);   
         }
      }
	}
	else
	{
		// Database to database transfer.
		// We need to transfer the font record by name.

		if (Font() > 0)
		{
			char cbName[80];
			s.m_pDatabase->font_data_name(Font(), cbName);
			m_Character.m_lFont = m_pDatabase->new_font_record(cbName, m_fTemporary);
		}

		if (BulletFont() > 0)
		{
			char cbName[80];
			s.m_pDatabase->font_data_name(BulletFont(), cbName);
			m_Paragraph.m_lBulletFont = m_pDatabase->new_font_record(cbName, m_fTemporary);
		}

      if (HyperlinkStyle() > 0)
      {
         HyperlinkData Data;
         s.m_pDatabase->GetHyperlinkData(HyperlinkStyle(), Data);
         m_Character.m_lHyperlinkStyle = m_pDatabase->NewHyperlinkRecord(&Data, m_fTemporary);   
      }
	}

	return *this;
}

/*
// The equality operator.
*/

BOOL CTextStyle::operator == (const CTextStyle& s)
{
	return memcmp(&m_Paragraph, &s.m_Paragraph, sizeof(m_Paragraph)) == 0
		&& memcmp(&m_Character, &s.m_Character, sizeof(m_Character)) == 0
		&& memcmp(&m_Metrics, &s.m_Metrics, sizeof(m_Metrics)) == 0
		&& memcmp(&m_Frame, &s.m_Frame, sizeof(m_Frame)) == 0
		&& (m_pDatabase == NULL
			|| s.m_pDatabase == NULL
			|| m_pDatabase == s.m_pDatabase);
}

/*
// Read the data for a text style.
//
// We read each component separately so that they can independently change
// size at a later time.
*/

void CTextStyle::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error;
	if ((error = m_Paragraph.ReadData(pDevice)) != ERRORCODE_None
			|| (error = m_Tabs.ReadData(pDevice)) != ERRORCODE_None
			|| (error = m_Character.ReadData(pDevice)) != ERRORCODE_None
			|| (error = m_Metrics.ReadData(pDevice)) != ERRORCODE_None
			|| (error = m_Frame.ReadData(pDevice)) != ERRORCODE_None
			|| (error = pDevice->read_record(&m_Record, sizeof(m_Record))) != ERRORCODE_None)
	{
		ThrowErrorcodeException(error);
	}

	// Translate old effects to new effects.
	if (m_Character.m_nEffectsVersion == 0)
	{
		m_Character.m_nEffectsVersion = 1;

		// White out flag.
		m_Character.m_fWhiteOut = TRUE;

		// Fill
		m_Character.m_Fill.FromOld(m_Character.m_nOldFill, m_Character.m_OldColor);

		// Outline
		m_Character.m_Outline.FromOld(m_Character.m_nOldOutline, m_Character.m_OldColor);

		// Shadow
		m_Character.m_Shadow.FromOld(m_Character.m_nOldShadow, m_Character.m_OldColor);
	}
}

/*
// Write the data for a text style.
//
// We write each component separately so that they can independently change
// size at a later time.
*/

void CTextStyle::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error;
	if ((error = m_Paragraph.WriteData(pDevice)) != ERRORCODE_None
			|| (error = m_Tabs.WriteData(pDevice)) != ERRORCODE_None
			|| (error = m_Character.WriteData(pDevice)) != ERRORCODE_None
			|| (error = m_Metrics.WriteData(pDevice)) != ERRORCODE_None
			|| (error = m_Frame.WriteData(pDevice)) != ERRORCODE_None
			|| (error = pDevice->write_record(&m_Record, sizeof(m_Record))) != ERRORCODE_None)
	{
		ThrowErrorcodeException(error);
	}
}

/*
// Size the data for a text style.
//
// We size each component separately so that they can independently change
// size at a later time.
*/

ST_MAN_SIZE CTextStyle::SizeofData(StorageDevice* pDevice)
{
	return m_Paragraph.SizeofData(pDevice)
			+ m_Tabs.SizeofData(pDevice)
			+ m_Character.SizeofData(pDevice)
			+ m_Metrics.SizeofData(pDevice)
			+ m_Frame.SizeofData(pDevice)
			+ pDevice->size_record(sizeof(m_Record));
}

// Initialize a CFontDescription object with the attributes that
// make a font unique.
BOOL CTextStyle::GetFontDescription(CFontDescription* pFontDescription)
{
	BOOL fResult = FALSE;

	// Get the typeface number.
	int nTypeface = ((PMGFontServer*)(Database()->get_font_server()))->font_record_to_face(Font());
	if (nTypeface != -1)
	{
		// Compute the style (bold, italic) of the typeface.
		int nFontStyle = 0;
		if (Bold())
		{
			nFontStyle |= FONT_STYLE_Bold;
		}
		if (Italic())
		{
			nFontStyle |= FONT_STYLE_Italic;
		}

		// Initialize the font description.
		pFontDescription->m_nTypeface = typeface_for_fstyle(nTypeface, (FONT_STYLE)nFontStyle);
		pFontDescription->m_lPointSize = Size();
		pFontDescription->m_lHorizontalExpansion = Expansion();
		pFontDescription->m_Fill = Fill();
		pFontDescription->m_Outline = Outline();
		pFontDescription->m_Shadow = Shadow();

		fResult = TRUE;
	}

	return fResult;
}

BOOL
CTextStyle::GetHyperlinkData(HyperlinkData &Data)
{
   DB_RECORD_NUMBER  RecordNumber;

   if ((RecordNumber = HyperlinkStyle()) != 0)
   {
      CHTMLTagLink   Link;
      
      PMGDatabase *pd;
      if ((pd = Database()) != NULL)
      {
         HyperlinkRecord   *pRec;
         if ((pRec = (HyperlinkRecord*)pd->get_record(RecordNumber, NULL, RECORD_TYPE_Hyperlink)) != NULL)
         {
            pRec->GetData(&Data);
            pRec->release();
            return TRUE;
         }
      }
   }
   
   return FALSE;
}

void 
CTextStyle::HyperlinkStyle(DB_RECORD_NUMBER nHyperlinkStyle)
{ 
   //m_Character.m_lHyperlinkStyle = nHyperlinkStyle;
	m_pDatabase->FreeHyperlinkRecord(m_Character.m_lHyperlinkStyle, m_fTemporary);
	m_pDatabase->IncrementHyperlinkRecord(m_Character.m_lHyperlinkStyle = nHyperlinkStyle, m_fTemporary);
}


/*
// CTextStyle::BulletIndent( )
//
// Returns the amount of space to indent after the bullet character.
// Takes into account point size, bullet type and bullet level
*/
CFixed CTextStyle::BulletIndent( ) const
{
	// find the amount of space appropriate for the point size of the font
	int nSpace = 0;
	// only do this if the bullet level is within our fixed range
	if( BulletLevel() >= 0 && BulletLevel() < BULLET_LEVEL_MAX )
	{
		if( BulletType()!= BULLET_none )
		{
			int nSize = FixedInteger( Size() );

			if( nSize < 14 )
				nSpace = 450;				// 1/4 inch
			else if( nSize < 24 )
				nSpace = 900;				// half inch
			else if( nSize < 39 )
				nSpace = 1350;				// 3/4 inch
			else if( nSize < 54 )
				nSpace = 1800;				// one inch
			else if( nSize < 69 )
				nSpace = 2250;				// 1 1/4 inch
			else if( nSize < 89 )
				nSpace = 2700;				// 1 and 1/2 inch
			else if( nSize < 109 )
				nSpace = 3150;				// 1 and 3/4 inch
			else
				nSpace = 3600;				// 2 inches

			int nExtra = 0;
			// a multilevel list may need additional space.
			// this tweek may need to be expanded if we add more list styles.
			if( BulletType() == BULLET_multilevel )
			{
				// in a multilevel mixed list levels 0 and 4 are roman numerals
				if( BulletStyle() == BULLET_roman_mix )
				{
					for( int i = 1; i < BulletLevel(); i++ )
					{
						if( BulletLevel() == 0 || BulletLevel == 4)
							nExtra += nSpace >> 1;
					}
				}
				// as the level increses, need more space for the number (1.1.1 ...)
				else if( BulletStyle() == BULLET_arabic )
				{
					PMGDatabase *pDataBase = Database();
					ASSERT( pDataBase );
					if( pDataBase )
					{
						FontServer* pFontServer = pDataBase->get_font_server();
						CHARACTER str[3] = { '1','.', 0 };
						CTextStyle style = *this;
						PCOORD width = pFontServer->GetAdvanceWidth(&style, 5);
						// need to accumulate the extra space for each level
						for( int i = 1; i <= BulletLevel(); i++ )
						{
							nExtra += width * i;
						}
					}
					else
					{
						// need to accumulate the extra space for each level
						for( int i = 1; i < BulletLevel(); i++ )
						{
							nExtra += MulDiv( nSize, PAGE_RESOLUTION, 72 ) * i;
						}
					}
				}
			}
			TRACE( "bullet level %d\nspace = %d, extra = %d", BulletLevel(), nSpace, nExtra );
			// multiply the space need by the bullet level + 1 to get the total indent
			nSpace *= BulletLevel() + 1;
			TRACE( "space * bulletlevel = %d\n", nSpace );
			nSpace += nExtra;
		}
	}
	return nSpace;	// total indent need from left margin for bulleted text 
}
/////////////////////////////////////////////////////////////////////////////
// CTextState

#if 0
// Assignment operator.
// This exists so the ID will not get copied.

CTextState& CTextState::operator = (const CTextState& s)
{
	m_lCharacter = s.m_lCharacter;
	m_lParagraph = s.m_lParagraph;
	m_lCharacterInParagraph = s.m_lCharacterInParagraph;

	return *this;
}
#endif

void CTextState::Reset(void)
{
	m_lCharacter = 0;

	m_lParagraph = 0;
	m_lCharacterInParagraph = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CTextIterator

CTextIterator::CTextIterator(CTextRecord* pRecord /*=NULL*/)
	: m_Style(NULL, TRUE)
{
	m_pRecord = NULL;
	m_State.Reset();

	SetRecord(pRecord);
}

CTextIterator::~CTextIterator(void)
{
}

/*
// Set the text record.
*/
  
void CTextIterator::SetRecord(CTextRecord* pRecord)
{
	// Reset the style.
	m_Style.Reset();

	// No change yet.
	m_pCurrentChange = NULL;

	// In with the new.
	if ((m_pRecord = pRecord) != NULL)
	{
		m_Style.Database((PMGDatabase*)pRecord->GetDatabase());
		m_State.Reset();
		UpdateStyle(TRUE);
	}
}

/*
// Set the current position.
*/

void CTextIterator::SetPosition(CHARACTER_INDEX Index)
{
	CTextState* pState = &State();

	ASSERT(pState != NULL);
	ASSERT(m_pRecord != NULL);
	if (pState != NULL && m_pRecord != NULL)
	{
		m_pRecord->InitializeTextState(pState, Index, FALSE);
		UpdateStyle();
	}
}

/*
// Update the style member given a new text state.
*/

void CTextIterator::UpdateStyle(BOOL fReset /*=FALSE*/)
{
	ASSERT(m_pRecord != NULL);
	if (m_pRecord == NULL)
	{
		return;
	}

	CTextState* pState = &State();
	if (pState == NULL)
	{
		return;
	}

	// Unpack some variables for faster access.
	PARAGRAPH_INDEX lParagraph = pState->m_lParagraph;
	CHARACTER_INDEX lCharacter = pState->m_lCharacter;

	if (fReset
			|| m_LastStyleState.m_lParagraph != lParagraph
			|| m_LastStyleState.m_lCharacter > lCharacter)
	{
	/* Start at the beginning of the paragraph. */
		ResetToParagraphStyle();
	}

	BOOL fChanged = FALSE;

	while (m_pCurrentChange != NULL)
	{
		// Compute the character index for the next change.
		if (NextChangeStart() > lCharacter)
		{
			break;
		}
		// Move to that change.
		AdvanceCurrentChange();
		// And grab it.
		GrabCurrentChange();

		fChanged = TRUE;
	}

	if (fChanged)
	{
		m_Style.UpdateFontMetrics();
	}

	// Remember this state for next time.
	m_LastStyleState = *pState;
}

/*
// Reset to the paragraph style.
*/

void CTextIterator::ResetToParagraphStyle(void)
{
	CTextState* pState = &State();

	PARAGRAPH_INDEX lParagraph = pState->m_lParagraph;
	CTextParagraph* pParagraph = m_pRecord->GetParagraph(lParagraph);
	ASSERT(pParagraph != NULL);

	m_pRecord->GetParagraphStyle(lParagraph, m_Style);

	// Current change is first.
	m_pCurrentChange = &pParagraph->m_StyleChange;

	// First character in paragraph.
	m_lCurrentChangeStart = pState->m_lCharacter - pState->m_lCharacterInParagraph;
	m_lCurrentChange = -1;

	// Last style change (parent of current).
	m_lLastChange = 0;

	m_LastStyleState.m_lParagraph = pState->m_lParagraph;
	m_LastStyleState.m_lCharacter = 0;
	m_LastStyleState.m_lCharacterInParagraph = 0;
}

/*
// Reload the current change after a modification to the change array.
*/

CTextStyleChange* CTextIterator::ReloadCurrentChange(void)
{
	CTextState* pState = &State();
	if (m_lCurrentChange == 0)
	{
		m_pCurrentChange = NULL;
	}
	else
	{
		m_pCurrentChange = m_pRecord->GetStyleChange(pState->m_lParagraph, m_lCurrentChange);
	}
	return m_pCurrentChange;
}

/*
// Return the starting character index of the next style change.
*/

CHARACTER_INDEX CTextIterator::NextChangeStart(void)
{
	return (m_pCurrentChange == NULL)
					? -1
					: m_lCurrentChangeStart + m_pCurrentChange->m_lCharacters;
}

CHARACTER_INDEX CTextIterator::NextParagraphStart(void)
{
   CHARACTER_INDEX   charIndexNextParagraph = -1;
   CHARACTER_COUNT   charcountParagraph;
   PARAGRAPH_INDEX   paraIndex;
   CTextParagraph *  pTextParagraph;
   CTextRecord *     pTextRecord = GetTextRecord();

   // Check if we have a text record
   if(pTextRecord)
      {
         // Get Current Paragraph
         paraIndex = ParagraphIndex();

         // Check if current paragraph is set and we have another paragraph
         if(paraIndex >= 0 && (paraIndex+1) < pTextRecord->NumberOfParagraphs())
            {
               pTextParagraph = pTextRecord->GetParagraph(paraIndex);
               if(pTextParagraph)
                  {
                     charcountParagraph = pTextParagraph->m_lCharacters;
                     // Char Index to next paragraph is relative to stream start.
                     charIndexNextParagraph = CharacterIndex() + 
                        charcountParagraph - CharacterInParagraph();
                  }
            }
      }

   return charIndexNextParagraph;
}

CHARACTER_INDEX CTextIterator::PreviousParagraphStart(void)
{
   CHARACTER_INDEX   charIndexPrevParagraph = -1;
   CHARACTER_COUNT   charcountParagraph;
   PARAGRAPH_INDEX   paraIndex;
   CTextParagraph *  pTextParagraph;
   CTextRecord *     pTextRecord = GetTextRecord();

   // Check if we have a text record
	if(pTextRecord)
	{
		// Get Current Paragraph
		paraIndex = ParagraphIndex();

		// Check if current paragraph is set and we have another paragraph
		if( paraIndex - 1 >= 0 )
		{
			pTextParagraph = pTextRecord->GetParagraph(paraIndex -1);
			if(pTextParagraph)
			{
				charcountParagraph = pTextParagraph->m_lCharacters;
				// Char Index to paragraph is relative to stream start.
				charIndexPrevParagraph = 
							CharacterIndex() - CharacterInParagraph() - charcountParagraph;
			}
		}
	}
   return charIndexPrevParagraph;
}

/*
// Advance to the next style change.
*/

void CTextIterator::AdvanceCurrentChange(void)
{
	ASSERT(m_pRecord != NULL);

	if (m_pCurrentChange != NULL)
	{
		// Remember this as the last.
		m_lLastChange = m_lCurrentChange;

		// Update the character of the next style change.
		m_lCurrentChangeStart += m_pCurrentChange->m_lCharacters;

		// Compute the index of the next style change.
		m_lCurrentChange = m_pCurrentChange->m_lNext;

		// Move to the next style change.
		m_pCurrentChange = (m_lCurrentChange == 0)
								? NULL
								: m_pRecord->GetStyleChange(m_lCurrentChange);
	}
	else
	{
		// Trying to advance past the end.
		ASSERT(FALSE);
	}
}

/*
// Backup a change. This can only be done once. And it does not revert
// m_Style. Used primarily for change traversal.
*/

void CTextIterator::BackupAChange(void)
{
	if (m_lLastChange == 0)
	{
		ASSERT(FALSE);
	}
	else
	{
		m_pCurrentChange = m_pRecord->GetStyleChange(ParagraphIndex(), m_lLastChange);
		m_lCurrentChange = m_lLastChange;
		m_lCurrentChangeStart -= m_pCurrentChange->m_lCharacters;

		// Not again.
		m_lLastChange = 0;
	}
}

/*
// Grab the current style change.
*/

void CTextIterator::GrabCurrentChange(BOOL fFixStyle /*=FALSE*/)
{
/* Just apply the change to the current style. */
	if (m_pCurrentChange != NULL && m_lCurrentChange > 0)
	{
		m_Style.ApplyChange(m_pCurrentChange, fFixStyle);
	}
}

/*
// Insert a style change here.
*/

void CTextIterator::InsertStyleChange(const CTextStyleChange& Change)
{
	CTextState* pState = &State();

	ASSERT(m_pRecord != NULL);
	ASSERT(pState != NULL);
	ASSERT(m_pCurrentChange != NULL);

	if (m_pRecord == NULL || pState == NULL)
	{
		return;
	}

	if (pState->m_lCharacterInParagraph == 0)
	{
		// We are at the front of the paragraph.
		// We don't want to add a style change here. We want to add it
		// into the paragraph's "paragraph" style changes.

		CTextParagraph* pParagraph = m_pRecord->GetParagraph(ParagraphIndex());
		// Search the paragraph changes for the matching change.
		STYLE_INDEX lLastChange = 0;
		STYLE_INDEX* plChange = &pParagraph->m_lFirstParagraphStyleChange;
		STYLE_INDEX lChange;
		while ((lChange = *plChange) != 0)
		{
			CTextStyleChange* pChange = m_pRecord->GetStyleChange(lChange);
			if (pChange->m_wToken == Change.m_wToken)
			{
				// If this is a face style change, increment its count.
				if (pChange->m_wToken == TOKEN_Face)
				{
					Database()->inc_face((DB_RECORD_NUMBER)Change.m_lValue);
					Database()->free_face((DB_RECORD_NUMBER)pChange->m_lValue);
				}
            else
				if (pChange->m_wToken == TOKEN_Hyperlink)
            {
					Database()->IncrementHyperlinkRecord((DB_RECORD_NUMBER)Change.m_lValue);
					Database()->FreeHyperlinkRecord((DB_RECORD_NUMBER)pChange->m_lValue);
            }

				// Replace the style change values.
				if (pChange->m_wToken == TOKEN_Size)
				{
					// The Size change is a ratio (which we need to preseve).
					// The m_lOldValue of the new style change is the resulting
					// value of the old style change. Run the style change backward
					// to get the original value (the true "old value"). I don't
					// know why the original old value isn't correct; I just know
					// there are documents that are this way and need to be dealt
					// with correctly.

					pChange->m_lOldValue = MulDivFixed(Change.m_lOldValue,
																  pChange->m_lOldValue,
																  pChange->m_lValue);
				}
				pChange->m_wFlags = Change.m_wFlags;
				pChange->m_lValue = Change.m_lValue;

				// The style change has been modified.
				m_pRecord->ModifyStyleChange(lChange);
				break;
			}
			else
			{
				// Pick up the next style change.
				lLastChange = *plChange;
				plChange = &pChange->m_lNext;
			}
		}

		if (lChange == 0)
		{
			// We did not find the style change. Add it.
			STYLE_INDEX lNewIndex;
			CTextStyleChange* pChange = m_pRecord->NewStyleChange(&lNewIndex, &Change);

			// Hook us in.
			pChange->m_lNext = pParagraph->m_lFirstParagraphStyleChange;
			pParagraph->m_lFirstParagraphStyleChange = lNewIndex;

			// The paragraph has been modified.
			m_pRecord->ModifyParagraph(ParagraphIndex());

			// Reget the current style change in case things moved.
			ASSERT(m_lCurrentChange != 0);
			m_pCurrentChange = m_pRecord->GetStyleChange(ParagraphIndex(), m_lCurrentChange);
		}
	}
	else
	{
		// Allocate a new style change from the paragraph.
		STYLE_INDEX lNewIndex;
		CTextStyleChange* pChange = m_pRecord->NewStyleChange(&lNewIndex, &Change);

		// Reget the current style change in case things moved.
		ASSERT(m_lCurrentChange != 0);
		m_pCurrentChange = m_pRecord->ModifyStyleChange(ParagraphIndex(), m_lCurrentChange);

		CHARACTER_INDEX lCharacter = pState->m_lCharacter;
		// Verify that we are indeed within the current change.
		ASSERT(lCharacter >= m_lCurrentChangeStart
					&& lCharacter <= m_lCurrentChangeStart + m_pCurrentChange->m_lCharacters);
		CHARACTER_COUNT lTotalCount = m_pCurrentChange->m_lCharacters;

		// Split the current style change.
		m_pCurrentChange->m_lCharacters = pState->m_lCharacter - m_lCurrentChangeStart;
		pChange->m_lCharacters = lTotalCount - m_pCurrentChange->m_lCharacters;

		// Hook it in to the chain.
		pChange->m_lNext = m_pCurrentChange->m_lNext;
		m_pCurrentChange->m_lNext = lNewIndex;


#if 0
		// We don't need to do this. NewStyleChange already inc'd it.

		// If this is a face style change, increment its count.
		if (pChange->m_wToken == TOKEN_Face)
		{
			Database()->inc_face((DB_RECORD_NUMBER)pChange->m_lValue);
		}
		else
      if (pChange->m_wToken == TOKEN_Hyperlink)
      {
         Database()->IncrementHyperlinkRecord((DB_RECORD_NUMBER)pChange->m_lValue);
      }
#endif

		// Move us to that change (without grabbing it).
		AdvanceCurrentChange();

		ASSERT(m_pCurrentChange == pChange);
	}
}

/*
// Delete the current style change here.
*/

void CTextIterator::DeleteCurrentChange(void)
{
	if (m_lCurrentChange == -1)
	{
		return;
	}

	// Validate the current state.
	if (m_lCurrentChange <= 0 || m_pCurrentChange == NULL)
	{
		ASSERT(FALSE);
		ThrowErrorcodeException(ERRORCODE_IntError);
	}

	// Get a pointer to the last change so we can delete the current one.
	ASSERT(m_lLastChange != 0);
	CTextStyleChange* pLastChange = NULL;
	pLastChange = m_pRecord->ModifyStyleChange(ParagraphIndex(), m_lLastChange);

	// Merge the characters in this style change into the previous one.
	m_lCurrentChangeStart -= pLastChange->m_lCharacters;	// Since we move back.
	pLastChange->m_lCharacters += m_pCurrentChange->m_lCharacters;

	// Unhook us from the list.
	pLastChange->m_lNext = m_pCurrentChange->m_lNext;

	// Delete us.
	m_pRecord->DeleteStyleChange(m_lCurrentChange);

	// And we end up on the last one.
	m_lCurrentChange = m_lLastChange;
	ReloadCurrentChange();

	// We can only back up once.
	m_lLastChange = 0;
}

/*
// ComputeStyleRange()
//
//		Compute a "composite" style for a range of characters.  A composite
// style is one which has a defined value for for each field which is the
// same for all characters in a range, and undefined for all fields which
// change within the range.
//
// Arguments:
//		lStart		First character (inclusive) in range to get style for.
//		lEnd			Last character (inclusive).
//		pStyle		STYLE structure to hold resulting composite style
// Returns:
//		Nothing
*/

void CTextIterator::ComputeStyleRange(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd, CTextStyle* pStyle)
{
	CTextState* pState = &State();

	// We need to figure out the starting paragraph.
	SetPosition(lStart);

	// Loop on paragraphs until done.
	BOOL fFirst = TRUE;
	PARAGRAPH_INDEX lParagraph = pState->m_lParagraph;
	CHARACTER_INDEX lParagraphStart = pState->m_lCharacter - pState->m_lCharacterInParagraph;
	PARAGRAPH_COUNT lParagraphs = 0;
	CTextParagraph* pParagraph = NULL;

	do
	{
		// Refill our pointer if we ran out.
		if (lParagraphs == 0)
		{
			pParagraph = m_pRecord->GetParagraph(lParagraph, &lParagraphs);
		}
		// Got the paragraph?
		ASSERT(pParagraph != NULL);
		// Compute the end of the range to compute.
		CHARACTER_INDEX lParagraphEnd = lParagraphStart + pParagraph->m_lCharacters;
		if (lParagraphEnd > lEnd-1)
		{
			lParagraphEnd = lEnd-1;
		}
		// Compute this range.
		ComputeParagraphStyleRange(lStart, lParagraphEnd, pStyle, !fFirst);
		// Move on to the next paragraph.
		lParagraphStart += pParagraph->m_lCharacters;
		lStart = lParagraphEnd;

		pParagraph++;
		lParagraph++;
		lParagraphs--;

		fFirst = FALSE;
	} while ((lStart < lEnd) && (lParagraph < m_pRecord->NumberOfParagraphs()));
}

/*
// Compute the style for a paragraph.
// The range is guaranteed to be within the paragraph.
*/

void CTextIterator::ComputeParagraphStyleRange(CHARACTER_INDEX lStart, CHARACTER_INDEX lEnd, CTextStyle* pStyle, BOOL fMergeStyles)
{
	// Get the start style.
	SetPosition(lStart);

	// Handle the style changes in the starting style.

	if (fMergeStyles)
	{
		int nToken;
		// Check paragraph style changes.
		for (nToken = FIRST_PARAGRAPH_TOKEN; nToken < LAST_PARAGRAPH_TOKEN; nToken++)
		{
			if (!pStyle->CompareStyles(nToken, &m_Style))
			{
				pStyle->MakeUndefined(nToken);
			}
		}
		// Check text (character) style changes.
		for (nToken = FIRST_TEXT_TOKEN; nToken < LAST_TEXT_TOKEN; nToken++)
		{
			if (!pStyle->CompareStyles(nToken, &m_Style))
			{
				pStyle->MakeUndefined(nToken);
			}
		}
	}
	else
	{
		*pStyle = m_Style;
	}

	// Pick up all character style changes in this range.

	while (NextChangeStart() <= lEnd)
	{
		AdvanceCurrentChange();
		if (CurrentChange() == NULL)
		{
			break;
		}
		pStyle->MakeUndefined(CurrentChange()->m_wToken);
	}
}
