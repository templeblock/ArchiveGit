/*
// Rich Text Format (RTF) CTxp-based destination
//
// $Header: /PM8/App/TXPDEST.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TXPDEST.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:52   Fred
// Initial revision.
// 
//    Rev 1.2   29 May 1997 08:45:36   Fred
// New fill color
// 
//    Rev 1.1   23 Dec 1996 17:01:18   Jay
// Better RTF paste and text paste.
// 
//    Rev 1.0   18 Dec 1996 09:10:36   Jay
// More RTF parsing
// 
//    Rev 1.1   16 Dec 1996 13:02:12   Jay
// Round 2
// 
//    Rev 1.0   13 Dec 1996 13:13:18   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "txpdest.h"
#include "utils.h"
#include "textrec.h"
#include "file.h"
#include "typeface.h"
#include "pmgfont.h"

/////////////////////////////////////////////////////////////////////////////
// CTxpRTFDestination
//   A CTxp-based destination for reading RTF data.

//
// Constructor.
//

CTxpRTFDestination::CTxpRTFDestination(CTxp* pTxp, int nBufferSize /*=2048*/)
{
	ASSERT(pTxp != NULL);
	m_pTxp = pTxp;
	m_pBuffer = NULL;
	m_nBufferSize = nBufferSize;
	m_nCharactersInBuffer = 0;

	TRY
	{
		m_pBuffer = new CHARACTER[m_nBufferSize];
	}
	END_TRY
}

//
// Destructor.
//

CTxpRTFDestination::~CTxpRTFDestination()
{
	delete [] m_pBuffer;
}

//
// Put a character.
//

void CTxpRTFDestination::PutChar(int c, const CRTFParser::CRTFState& State)
{
	UpdateStyle(State);

	CHARACTER ch = (CHARACTER)c;

   if (m_pBuffer == NULL)
	{
		// Output the characters one at a time.

		m_pTxp->InsertText(&ch, 1);
	}
	else
	{
		// Add it to the output buffer.
		m_pBuffer[m_nCharactersInBuffer++] = ch;

		// See if it is time to flush.
		if (m_nCharactersInBuffer == m_nBufferSize)
		{
			FlushBuffer();
		}
	}
}

void CTxpRTFDestination::Pard(const CRTFParser::CRTFState& State)
{
	FlushBuffer();

	CTextRecord* pText = NULL;

	TRY
	{
		// Lock the text.
		pText = m_pTxp->LockText();

		// Add the tab to our array.
		CTabArray Tabs(m_pTxp->Style().m_Tabs);

		Tabs.RemoveAll();

		// Compute the first and last paragraphs.
		PARAGRAPH_INDEX lStart, lEnd;
		m_pTxp->GetParagraphRange(&lStart, &lEnd);

		pText->ChangeParagraphTabs(lStart, lEnd, &Tabs);
	}
	END_TRY

	if (pText != NULL)
	{
		pText->release();
	}
}

void CTxpRTFDestination::Plain(const CRTFParser::CRTFState& State)
{
	FlushBuffer();

	m_pTxp->Style().SetDefault();
}

void CTxpRTFDestination::SetTab(const CRTFTab* pTab, const CRTFParser::CRTFState& State)
{
	CTextRecord* pText = NULL;

	TRY
	{
		// Lock the text.
		pText = m_pTxp->LockText();

		// Set up the tab.
		CTextTab Tab;

		Tab.Default();
		Tab.m_nType = pTab->m_nType;
		Tab.m_Position = TwipsToFixed(pTab->m_nPosition);
		Tab.m_Leader[0] = pTab->m_cLeader;

		// Add the tab to our array.
		CTabArray Tabs;

      Tabs.Add(&Tab);

		// Compute the first and last paragraphs.
		PARAGRAPH_INDEX lStart, lEnd;
		m_pTxp->GetParagraphRange(&lStart, &lEnd);

		// Include it in the paragraph tab list (relatively).
		pText->ChangeParagraphTabs(lStart, lEnd, &Tabs, TRUE);
	}
	END_TRY

	if (pText != NULL)
	{
		pText->release();
	}
}

//
// Flush our buffer.
//

void CTxpRTFDestination::Flush(void)
{
	FlushBuffer();
}

void CTxpRTFDestination::FlushBuffer(void)
{
	if (m_nCharactersInBuffer > 0)
	{
		ASSERT(m_pBuffer != NULL);

		// Output what is in the buffer.
		m_pTxp->InsertText(m_pBuffer, m_nCharactersInBuffer);

		// Reset the character count.
		m_nCharactersInBuffer = 0;
	}
}

CFixed CTxpRTFDestination::TwipsToFixed(int nTwips)
{
	return MakeFixed(nTwips)/1440;
}

CFixed CTxpRTFDestination::TwipsToPoints(int nTwips)
{
	return MakeFixed(nTwips)/20;
}

void CTxpRTFDestination::UpdateStyle(const CRTFParser::CRTFState& State)
{
	PMGDatabase* pDatabase = m_pTxp->Database();
	ASSERT(pDatabase != NULL);
	PMGFontServer* pFontServer = (PMGFontServer*)pDatabase->get_font_server();
	ASSERT(pFontServer != NULL);

	if (State.m_wChanged != CRTFParser::None_Changed)
	{
		FlushBuffer();
		CTextStyle& Style = m_pTxp->Style();

      if (State.m_wChanged & CRTFParser::CHP_Changed)
      {
//			TRACE("Set size: %d\n", State.m_chp.nSize);
         CFixed lSize = MakeFixed(State.m_chp.nSize)/2;
         Style.BaseSize(lSize);
         Style.Size(lSize);
		   Style.Bold(State.m_chp.fBold);
		   Style.Italic(State.m_chp.fItalic);
		   Style.Underline(State.m_chp.fUnderline);
         if (State.m_chp.nForeColor < State.m_ColorTable.GetSize())
         {
		      CRTFColorEntry* pEntry = (CRTFColorEntry*)State.m_ColorTable.GetAt(State.m_chp.nForeColor);
		      Style.FillForegroundColor(COLOR_FROM_COLORREF(RGB(pEntry->m_nRed, pEntry->m_nGreen, pEntry->m_nBlue)));
         }
         else
         {
            Style.FillForegroundColor(COLOR_BLACK);
         }

			//
			// Handle the font.
			//

			int nFont = State.m_chp.nFont;
			if (nFont < 0 || nFont >= State.m_FontTable.GetSize())
			{
				// Try font zero.
				nFont = 0;
			}

			// Make sure there is a font to use.
			if (nFont < State.m_FontTable.GetSize())
			{
				// We have a font to use.
				CRTFFontEntry* pEntry = (CRTFFontEntry*)State.m_FontTable.GetAt(nFont);
				int nIndex = typeface_server.find_face(pEntry->m_csName, FALSE);
				if (nIndex == -1)
				{
					// Try to map the font.
					LPCSTR pNewName = NULL;
					switch (pEntry->m_nFontFamily)
					{
						case CRTFFontEntry::froman:
						{
							pNewName = "Tiempo";
							break;
						}
						case CRTFFontEntry::fswiss:
						{
							pNewName = "Geneva";
							break;
						}
						case CRTFFontEntry::fmodern:
						{
							pNewName = "Journal";
							break;
						}
						case CRTFFontEntry::fscript:
						{
//							pNewName = "Script";		// A Windows font.
							break;
						}
						case CRTFFontEntry::fdecor:
						{
							pNewName = "Victorian";
							break;
						}
						case CRTFFontEntry::ftech:
						{
							pNewName = "Symbol";		// A Windows font.
							break;
						}
						case CRTFFontEntry::fbidi:
						{
							pNewName = "Miriam";		// A Windows font.
							break;
						}
						default:
						{
							break;
						}
					}

					// See if we have a new name to try.
					if (pNewName != NULL)
					{
						nIndex = typeface_server.find_face(pNewName, FALSE);
					}

					if (nIndex == -1)
					{
						nIndex = typeface_server.replacement_face;
					}
				}

				ASSERT(nIndex != -1);
				DB_RECORD_NUMBER lFace = pFontServer->font_face_to_record(nIndex);
				if (lFace != 0)
				{
					// Set the face.
					Style.Font(lFace);

					// Free our temporary ref on this.
					pDatabase->free_font_record(lFace, TRUE);
				}
			}
      }
      if (State.m_wChanged & CRTFParser::PAP_Changed)
		{
			CTextRecord* pText = NULL;

			TRY
			{
				// Lock the text.
				pText = m_pTxp->LockText();

				// Compute the first and last paragraphs.
				PARAGRAPH_INDEX lStart, lEnd;
				m_pTxp->GetParagraphRange(&lStart, &lEnd);

				// Handle alignment.
				static int nAlignmentMap[] =
				{
					ALIGN_left,
					ALIGN_right,
					ALIGN_center,
					ALIGN_justify_left
				};
				int nJust = State.m_pap.just;
				if (nJust < 0 || nJust >= sizeof(nAlignmentMap)/sizeof(nAlignmentMap[0]))
				{
					nJust = 0;
				}
				Style.Alignment(nAlignmentMap[nJust]);
				// Handle the left margin.
				Style.LeftMargin(TwipsToFixed(State.m_pap.xaLeft));
				// Handle the right margin.
				Style.RightMargin(TwipsToFixed(State.m_pap.xaRight));
				// Handle the first line indent.
				Style.FirstLineIndent(TwipsToFixed(State.m_pap.xaFirst));
				// Handle the space before
				Style.SpaceBefore(TwipsToFixed(State.m_pap.xaSpaceBefore));
				// Handle the space after
				Style.SpaceAfter(TwipsToFixed(State.m_pap.xaSpaceAfter));

				// Handle the leading value.
				int nLeading = State.m_pap.xaLineSpacing;
				if (nLeading == 1000)
				{
					nLeading = 0;
				}
				else if (nLeading < 0)
				{
					nLeading = -nLeading;
				}

				if (nLeading == 0)
				{
					Style.LeadingType(LEADING_lines);
					Style.Leading(MakeFixed(1));
				}
				else
				{
					Style.LeadingType(LEADING_points);
					Style.Leading(TwipsToPoints(nLeading));
				}

				Style.UpdateFontMetrics();
				pText->ChangeParagraphStyle(lStart, lEnd, &Style);
			}
			END_TRY

			if (pText != NULL)
			{
				pText->release();
			}
		}

		Style.UpdateFontMetrics();
	}
}
