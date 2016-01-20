//////////////////////////////////////////////////////////////////////////////
// $Workfile: WHlpWiz.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/WHlpWiz.cpp $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 1:57p Hforman
// Changed name from CWHlpWiz.cpp
// 
// 29    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 28    7/04/98 10:37a Jayn
// Fixed template loop bug (caused a crash)
// 
// 27    6/25/98 4:14p Jayn
// WITHHELP.TXT is gotten from the local directory.
// 
// 26    6/05/98 8:40a Fredf
// Document Flags
// 
// 25    6/04/98 9:52p Hforman
// add InsertNewlines()
// 
// 24    6/04/98 2:39p Rlovejoy
// Avoid checking for unsupported project types in title strings.
// 
// 23    6/04/98 12:12p Rlovejoy
// Fixed string assertion.
// 
// 22    6/04/98 11:08a Rlovejoy
// Strip off qoutes from description.
// 
// 21    6/03/98 9:39p Hforman
// 
// 20    6/03/98 9:16p Psasse
// Fixed initialization of new parameters problem
// 
// 19    6/03/98 7:06p Rlovejoy
// Code to get description string.
// 
// 18    6/01/98 8:22p Hforman
// moved preview window to CWHelpPropertyPage
// 
// 17    5/31/98 5:04p Rlovejoy
// Added InitializedSuccessfully() function.
// 
// 16    5/31/98 3:31p Rlovejoy
// Included code to strip out unsupporetd project types.
// 
// 15    5/30/98 6:05p Psasse
// Complete With Help WebPage support
// 
// 14    5/30/98 5:45p Psasse
// With Help Web Page support
// 
// 13    5/29/98 9:38p Hforman
// background drawing
// 
// 12    5/29/98 6:41p Rlovejoy
// Strip out quotes.
// 
// 11    5/29/98 6:23p Hforman
// fixed back/finish button problems; started working on background tiling
// 
// 10    5/29/98 3:45p Psasse
// With Help Label support
// 
// 9     5/28/98 8:31p Hforman
// added drawing of PrintMaster Man
// 
// 8     5/28/98 6:36p Rlovejoy
// Changes to learn more about branching direction.
// 
// 7     5/28/98 12:54p Hforman
// add Poster container
// 
// 6     5/27/98 9:07p Psasse
// intermediate check in for With Help - Newsletter
// 
// 5     5/27/98 5:24p Rlovejoy
// Added search string retrieval.
// 
// 4     5/26/98 5:48p Hforman
// cleanup, added dummy page containers
// 
// 3     5/26/98 4:09p Rlovejoy
// Took out some unnecessary code.
// 
// 2     5/26/98 3:02p Rlovejoy
// Massive revisions.
// 
//    Rev 1.0   14 Aug 1997 15:19:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:56   Fred
// Initial revision.
//
*/

#include "stdafx.h"
#include "pmw.h"
#include "Pmwdoc.h"
#include "bmptiler.h"
#include "pmwtempl.h"

#include "WHlpView.h"
#include "WHlpProj.h"
#include "WHlpWiz.h"

// The project types
#include "WHCalend.h"
#include "WHGreet.h"
#include "WHLtrhd.h"
#include "WHPoster.h"
#include "WHNwsltr.h"
#include "WHLabel.h"
#include "WHWebPg.h"

/////////////////////////////////////////////////////////////////////////////
// CWHelpPageContainer base class

CWHelpPageContainer::CWHelpPageContainer(CWHelpWizPropertySheet* pSheet)
{
	m_pSheet = pSheet;
	m_fText1Valid = FALSE;
	m_fText2Valid = FALSE;
}

CWHelpPageContainer::~CWHelpPageContainer()
{
	// Free memory allocated to the array
	int nItems = m_aPages.GetSize();
	for (int i = 0; i < nItems; i++)
	{
		delete (CWHelpPropertyPage*)m_aPages[i];
	}
}

void CWHelpPageContainer::AddPages()
{
	// Add the pages in order to the property sheet
	int nItems = m_aPages.GetSize();
	for (int i = 0; i < nItems; i++)
	{
		m_pSheet->AddPage((CPropertyPage*)m_aPages[i]);
	}
}

void CWHelpPageContainer::RemovePages(int nOffset)
{
	// Remove all but the first page from the property sheet
	int nItems = m_aPages.GetSize();
	for (int i = 0; i < nItems; i++)
	{
		m_pSheet->RemovePage((CPropertyPage*)m_aPages[i]);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWHTable

CWHTable::CWHTable(int nTitleStrings /*= N_WIZ_TITLE_STRINGS*/, int nColumns /*= N_WIZ_TABLE_COLUMNS*/, char cListEnd /*= '!'*/)
{
	m_nTitleStrings = nTitleStrings;
	m_nColumns = nColumns;
	m_cListEnd = cListEnd;
}

void CWHTable::LoadTable(CString csTableName)
{
   CStdioFile File;
   BOOL fOK = File.Open(csTableName, CFile::modeRead | CFile::typeText);
   if (!fOK)
   {
      TRACE("Couldn't open Withhelp table!\n");
      return;
   }

	// Store it
	CString csString;
	m_csaLine.RemoveAll();
	while (File.ReadString(csString))
	{
		m_csaLine.SetNames(csString, '\t', FALSE);	// TAB deliminited
   }

	// Init branching
	m_nIndex = 0;
	m_IndexStack.RemoveAll();
}

void CWHTable::BranchAhead(int nBranchIndex)
{
	// Put previous index on stack
	m_IndexStack.AddHead(m_nIndex);

	// Locate choice
	FindChoiceIndex(nBranchIndex, m_nIndex);
	
	// Table follows an over 1 column and down 1 row to branch ahead
	IncrementRow(m_nIndex);
	IncrementColumn(m_nIndex);
}

void CWHTable::BranchBack()
{
	if (!m_IndexStack.IsEmpty())
	{
		m_nIndex = m_IndexStack.RemoveHead();
	}
}

void CWHTable::GetColumnStrings(CStringArray& csStrings, BOOL fKillUnsupportedTypes)
{
	// Retrieve the strings in the current column.
	// Keep finding strings until an 'end choice' token is found.
	int nIndex = m_nIndex;
	int nTableItems = m_csaLine.GetUpperBound();
	int nFound = 0;
	while (nIndex < nTableItems)
	{
		// Find the next non empty string in the column
		while (m_csaLine[nIndex].IsEmpty())
		{
			IncrementRow(nIndex);
			if (nIndex > nTableItems)
				break;
		}

		// Add the non empty string to the array
		if (nIndex <= nTableItems)
		{
			// Add string
			BOOL f = AddFoundString(csStrings, nIndex);
			nFound++;

			// Check for an unsupported type
			if (fKillUnsupportedTypes && nFound > m_nTitleStrings)
			{
				int nLastIndex = csStrings.GetUpperBound();
				CString cs = csStrings[nLastIndex];

				// Search for a token
				int nBang = cs.Find(m_cListEnd);
				if (nBang != -1)
				{
					CString csProjType = cs.Right(cs.GetLength() - (nBang+1));
					int nProjType = atoi((const char*)csProjType);

					// Take off the project type info
					cs = cs.Left(nBang);
					csStrings[nLastIndex] = cs;

					// See if the project type is supported
					CPmwApp* pApp = GET_PMWAPP();
					CPmwDocTemplate* pTemplate = NULL;
					POSITION pos = pApp->GetFirstDocTemplatePosition();

					while (pos != NULL)
					{
						CPmwDocTemplate* pThisTemplate = (CPmwDocTemplate*)pApp->GetNextDocTemplate(pos);
						if (pThisTemplate->ProjectType() == nProjType)
						{
							pTemplate = pThisTemplate;
							break;
						}
					}

					if (pTemplate == NULL)
					{
						// Take it off the list, and empty it
						csStrings.RemoveAt(nLastIndex);
						m_csaLine[nIndex].Empty();

						// In theory, if this string was the last in the column,
						// we should put a m_cListEnd char in front of the next to
						// last string in the column because this one is now gone.
						// We don't really need to do this, however, because we're
						// tossing out project types which are in the first row.

					}
				}
			}	
			IncrementRow(nIndex);

			if (f)
				break;
		}
	}
}

void CWHTable::GetData(int nChoice, CString& csDataType, CString& csName, CString& csDescription)
{
	int nColumn;
	int nIndex = m_nIndex;

	// Locate the choice in the column
	FindChoiceIndex(nChoice, nIndex);

	// Current column is determined by # of items on branch stack
	nColumn = m_IndexStack.GetCount();

	// Data type is at m_nColumns-3 (0 based)
	int i = nColumn;
	while (i < m_nColumns-3)
	{
		IncrementColumn(nIndex);
		i++;
	}
	csDataType = m_csaLine[nIndex];

	// Name is right next to it
	IncrementColumn(nIndex);
	csName = m_csaLine[nIndex];

	// Then we have the description
	IncrementColumn(nIndex);
	csDescription = m_csaLine[nIndex];
	
	if (!csDescription.IsEmpty())
	{
		// Strip out quotes
		if (csDescription[0] == '"')
		{
			csDescription = csDescription.Mid(1, csDescription.GetLength()-2);
		}

		// Replace '|' character with '\n'
		InsertNewlines(csDescription);
	}
}

void CWHTable::GetSearchStrings(CStringArray& csSearchStrings)
{
	// Search strings are located after the last choice string.
	// Find them-- 
	// Retrieve the strings in the current column.
	// Keep finding strings until an 'end choice' token is found.
	int nIndex = m_nIndex;
	int nTableItems = m_csaLine.GetUpperBound();
	while (nIndex < nTableItems)
	{
		// Find the next non empty string in the column
		while (m_csaLine[nIndex].IsEmpty())
		{
			IncrementRow(nIndex);
			if (nIndex > nTableItems)
				break;
		}

		
		if (nIndex <= nTableItems)
		{	
			CString cs = m_csaLine[nIndex];
			IncrementRow(nIndex);

			// We're done if the string has the token
			if (cs[0] == m_cListEnd)
				break;
		}
	}
				
	// Might be done
	if (nIndex > nTableItems)
		return;		

	// Now put search strings into the array for as long as we can
	while (!m_csaLine[nIndex].IsEmpty())
	{
		// Add string
		BOOL f = AddFoundString(csSearchStrings, nIndex);	
		IncrementRow(nIndex);

		if (f)
			break;
	}			
}

BOOL CWHTable::AddFoundString(CStringArray& csStrings, int nIndex)
{
	CString cs = m_csaLine[nIndex];

	// Strip out quotes
	if (cs[0] == '"')
	{
		cs = cs.Mid(1, cs.GetLength()-2);
	}

	// If this is the first string (top text), replace '|' with '\n'
	if (csStrings.GetSize() == 0)
		InsertNewlines(cs);

	BOOL f;
	if ((f = (cs[0] == m_cListEnd)))
	{
		// Strip off the token
		cs = cs.Right(cs.GetLength()-1);
	}
	csStrings.Add(cs);

	return f;
}

void CWHTable::FindChoiceIndex(int nChoice, int& nIndex)
{
	// Bypass title strings
	for (int i = 0; i < m_nTitleStrings; i++)
	{
		IncrementRow(nIndex);
	}

	// Find the index at which to branch
	int nCnt = 0;
	int nTableItems = m_csaLine.GetUpperBound();
	while (nCnt < nChoice)
	{
		do
		{
			IncrementRow(nIndex);
			ASSERT(nIndex <= nTableItems);
		}
		while (m_csaLine[nIndex].IsEmpty());
		nCnt++;
	}
}

void CWHTable::InsertNewlines(CString& str)
{
	// Replace '|' character with '\n'
	int nPos = str.Find('|');
	while (nPos != -1)
	{
		str.SetAt(nPos, '\n');
		nPos = str.Find('|');
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWHelpWizPropertySheet

IMPLEMENT_DYNAMIC(CWHelpWizPropertySheet, CPropertySheet)

CWHelpWizPropertySheet::CWHelpWizPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	ConstructSheet();

}

CWHelpWizPropertySheet::CWHelpWizPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	ConstructSheet();
}

CWHelpWizPropertySheet::~CWHelpWizPropertySheet()
{
	// Delete the pages we own
	delete m_pProjectType;

	// Free memory allocated to the array
	int nItems = m_aPageContainers.GetSize();
	for (int i = 0; i < nItems; i++)
	{
		delete (CWHelpPageContainer*)m_aPageContainers[i];
	}	
}

void CWHelpWizPropertySheet::ConstructSheet()
{
	// Build the array of page containers
	m_nActiveContainer = -1;
	m_aPageContainers.Add(new CWCalendarPageContainer(this));
	m_aPageContainers.Add(new CWGreetingPageContainer(this));
	m_aPageContainers.Add(new CLabelPageContainer(this));
	m_aPageContainers.Add(new CWLetterheadPageContainer(this));
	m_aPageContainers.Add(new CNewsLetterPageContainer(this));
	m_aPageContainers.Add(new CWPosterPageContainer(this));
	m_aPageContainers.Add(new CWebPagePageContainer(this));

	// Create static pages
	m_pProjectType = new CWProjectType;

	// Add project selection page
	AddPage(m_pProjectType);

	// Make it wizard-like
	SetWizardMode();
	
	// Load the wizard-table
	CString csTableName = GetGlobalPathManager()->ExpandPath("WITHHELP\\WITHHELP.TXT");
	m_WizTable.LoadTable(csTableName);

	// Init table branching
	m_nBranchDirection = 0;			// -1 for back, 0 for none, 1 for ahead
	m_nBranchIndex = 0;
	m_pLastBranch = NULL;			// the page we were on last when m_nBranchDirection was set

	m_wDocumentFlags = CPmwDoc::FLAG_WithHelpDocument;
}

BOOL CWHelpWizPropertySheet::InitializedSuccessfully()
{
	return !m_WizTable.IsEmpty();
}

void CWHelpWizPropertySheet::AddActivePages()
{
	// Let the active container put its pages in the property sheet
	((CWHelpPageContainer*)m_aPageContainers[m_nActiveContainer])->AddPages();
}

void CWHelpWizPropertySheet::RemoveActivePages()
{
	// Let the active container take its pages out of the property sheet
	((CWHelpPageContainer*)m_aPageContainers[m_nActiveContainer])->RemovePages(1);
	m_nActiveContainer = -1;
}

void CWHelpWizPropertySheet::FindBranchAhead(int nBranchIndex)
{
	m_nBranchDirection = 1;
	m_nBranchIndex = nBranchIndex;
	m_pLastBranch = GetActivePage();
}

void CWHelpWizPropertySheet::FindBranchBack()
{
	m_nBranchDirection = -1;
	m_pLastBranch = GetActivePage();
}

int CWHelpWizPropertySheet::FollowBranch(CPropertyPage* pPage)
{
	int nDirectionTaken = -1;	// assume we're backing up
	if (pPage != m_pLastBranch)
	{
		if (m_nBranchDirection == 1)
		{
			m_WizTable.BranchAhead(m_nBranchIndex);
		}
		else if (m_nBranchDirection == -1)
		{
			m_WizTable.BranchBack();
		}
		nDirectionTaken = m_nBranchDirection;
	}
	m_nBranchDirection = 0;

	return nDirectionTaken;
}

void CWHelpWizPropertySheet::RetrieveSelectedData(int nChoice, CString& csDataType, CString& csName, CString& csDescription)
{
	// Get chosen item and keep table intact
	m_WizTable.GetData(nChoice, csDataType, csName, csDescription);
}

void CWHelpWizPropertySheet::RetrieveSearchStrings(CStringArray& csSearchStrings)
{
	// Find all search strings at the current position
	m_WizTable.GetSearchStrings(csSearchStrings);
}

BEGIN_MESSAGE_MAP(CWHelpWizPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CWHelpWizPropertySheet)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHelpWizPropertySheet message handlers

BOOL CWHelpWizPropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// Load our background bitmap
	Util::LoadResourceBitmap(m_bmpBkgrnd, MAKEINTRESOURCE(IDB_GALLERY_PREVIEW), pOurPal);
	
	// Disable the "Back" button
	CWnd* pWnd = GetDlgItem(ID_WIZBACK);
	if (pWnd)
		pWnd->EnableWindow(FALSE);
	
	return bResult;
}

void CWHelpWizPropertySheet::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Draw the background
	CBmpTiler bmpTiler;
	if ((HBITMAP)m_bmpBkgrnd)
	{
		CRect rect;
		GetClientRect(rect);
		bmpTiler.TileBmpInRect(dc, rect, m_bmpBkgrnd);
	}
	
	// Do not call CPropertySheet::OnPaint() for painting messages
}


