//////////////////////////////////////////////////////////////////////////////
// $Workfile: WHlpView.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/WHlpView.cpp $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:12p Hforman
// Changed name from CWView.cpp
// 
// 16    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 15    7/14/98 3:24p Psasse
// Fixed the multiple replacement of a given string
// 
// 14    6/05/98 2:55p Psasse
// deletion of unnecessary assert and better base class implementation
// 
// 13    6/04/98 9:50p Hforman
// fix string replacement, remove unused stuff
// 
// 12    6/04/98 7:19p Psasse
// modal helpful hint support
// 
// 11    6/03/98 9:17p Psasse
// Receiver Block support in With Help - labels
// 
// 10    6/03/98 2:26p Psasse
// interim WithHelp Label support
// 
// 9     6/01/98 11:26p Hforman
// change baseclass to CWHelpPropertyPage; other cleanup
// 
// 8     6/01/98 8:24p Hforman
// add m_fReplaceNullString
// 
// 7     5/29/98 6:28p Rlovejoy
// Took out unused code.
// 
// 6     5/29/98 6:10p Hforman
// replacing user-supplied strings
// 
// 5     5/28/98 8:38p Hforman
// remove title string
// 
// 4     5/27/98 6:58p Rlovejoy
// Made this the final page.
// 
// 3     5/27/98 1:35p Rlovejoy
// Show the project preview.
// 
// 2     5/26/98 3:02p Rlovejoy
// Massive revisions.
// 
//    Rev 1.0   14 Aug 1997 15:19:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:00   Fred
// Initial revision.
// 
//    Rev 1.16   31 Jul 1997 13:18:58   Jay
// Clips preview
// 
//    Rev 1.15   15 Jul 1997 10:29:30   Jay
// Fixed text replacement
// 
//    Rev 1.14   01 Jul 1997 09:32:22   Jay
// MRU / Open document changes
// 
//    Rev 1.13   17 Jun 1997 13:20:56   doberthur
// Add multiple temp project files.
//   

#include "stdafx.h"
#include "pmw.h"
#include "Pmwdoc.h"
#include "ctxp.h"
#include "findtext.h"
#include "WHlpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// static int, incremented for each WH_TMPx file
int CWView::m_nTmpFileNumber = 1;

/////////////////////////////////////////////////////////////////////////////
// CWView dialog

CWView::CWView(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
 : CWHelpPropertyPage(pContainer, nIDContainerItem, CWView::IDD)
{
	//{{AFX_DATA_INIT(CWView)
	//}}AFX_DATA_INIT

   m_pDoc = NULL;
}


void CWView::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWView)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWView, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CWView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWView message handlers

BOOL CWView::OnInitDialog() 
{
	CWHelpPropertyPage::OnInitDialog();

	// Always going to be showing a template
	m_preview.SetType(CWithHelpPreview::TEMPLATE_PREVIEW);

	return FALSE;  
}

BOOL CWView::OnSetActive() 
{
	GetSheet()->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	// a document must have been selected in the choose dialog
	CString csDocName = GetSheet()->GetSelProject();
	if (csDocName.IsEmpty())
		OnWizardBack();

	// open the selected project
	OpenDocument(csDocName);
	if (m_pDoc == NULL)
	{
		OnWizardBack(); // nothing to view so go back
	}
	else  // OK   
	{
		// replace the strings if needed
		CString str1, str2;
		int nValidStrs = 0;
		if (m_pContainer->UserText1Valid())
		{
			str1 = m_pContainer->GetUserText1();
		StripReturnsFromStr(str1);
			nValidStrs++;
		}
		if (m_pContainer->UserText2Valid())
		{
			str2 = m_pContainer->GetUserText2();
		StripReturnsFromStr(str2);
			nValidStrs++;
		}

		CStringArray searchStrs;
		GetSheet()->RetrieveSearchStrings(searchStrs);

		if (nValidStrs > 0 && searchStrs.GetSize() > 0)
		{
			if (m_pContainer->UserText1Valid())
					FindAndReplace(m_pDoc, searchStrs[0], str1);
			else if (m_pContainer->UserText2Valid())
					FindAndReplace(m_pDoc, searchStrs[0], str2);

			if (m_pContainer->UserText2Valid() && searchStrs.GetSize() == 2)
					FindAndReplace(m_pDoc, searchStrs[1], str2);
		}
		// Set up preview
		m_preview.UpdateDocPreview(m_pDoc);
	}

	// don't call baseclass
//	return CWHelpPropertyPage::OnSetActive();
	return CPropertyPage::OnSetActive();
}

BOOL CWView::OnWizardFinish() 
{
   CString sPath;
   CString pcsDrive;
   CString pcsDirectory;
   CString pcsFile;
   CString pcsExtension;
   CString sTmpFileName;
   CString pcsTmp;

   UpdateData(TRUE);
   if (m_pDoc) {
      
		// reset panel to front if this is a card
		/*
      if (m_pDoc->IsKindOf(RUNTIME_CLASS(CCardDoc))) {
         // need to cast to CCardDoc
         CCardDoc *pCardDoc = (CCardDoc*)m_pDoc;
		   pCardDoc->set_panel(CARD_PANEL_Front);
      }
		*/
      
		// save modified doc in tmp file
		CString csDocName = GetSheet()->GetSelProject();
      sPath = m_pDoc->GetPathManager()->GetWorkingDirectory();
      Util::SplitPath(csDocName, &pcsDrive, &pcsDirectory, &pcsFile, &pcsExtension);

      // Use a wtemp file name that is unique, _WTEMPxx where xx = 1 - 99
      pcsTmp.Format("%d", m_nTmpFileNumber++);  // m_nTmpFileNumber is a static class int
      sPath += "_WTEMP";
      sPath += pcsTmp;
      sPath += pcsExtension;
      m_pDoc->SaveToFile(LPCTSTR(sPath), FALSE);
      GetSheet()->SetTmpProject(sPath);  // save tmp file name to be opened in project space
      if (m_nTmpFileNumber < 1 || m_nTmpFileNumber > 99) {
         m_nTmpFileNumber = 1;
      }

      CloseDocument();
   }

	// Bypass everybody
	return CPropertyPage::OnWizardFinish();
}

void CWView::StripReturnsFromStr(CString& strToStrip)
{
	// strip all /r's (carrage returns) from string
	CString csResult;
	CString csTmp = strToStrip;
	while (!csTmp.IsEmpty())
	{
		// Extract the next component from the path. 
		CString csComponent = csTmp.SpanExcluding("\r");
		if (!csComponent.IsEmpty()) // add to result
			csResult += csComponent;

		int nRemain = csTmp.GetLength(); // how many chars left
		int i = nRemain - (csComponent.GetLength()+1); // 

		csTmp = csTmp.Right(i < 0 ? 0 : i);  // remove up & including /n 
		ASSERT(csTmp.GetLength() < nRemain); 
	}

	strToStrip = csResult;
}

const enum eFindReplaceState2
{
   STATE_Error,
   STATE_Start,
   STATE_Find,
   STATE_Found,
   STATE_Done
};

void CWView::FindAndReplace(CPmwDoc* pDoc,       // open doc
                            LPCSTR searchText,  LPCSTR insertText) // string 
{
   int               idx;
   CString           Text;
   CHARACTER         c;
   DB_RECORD_NUMBER  Frame;
   CHARACTER_INDEX   SearchIndexEnd;
   CHARACTER_INDEX   SearchIndexStart;

   BOOL docase = FALSE;
   BOOL wholeword = FALSE;
   enum eFindReplaceState2  FindReplaceState = STATE_Find;
   CStoryTracker StoryTracker(pDoc);

   SearchIndexStart = StoryTracker.Tell ();
   // Look through stories for text
   while (FindReplaceState == STATE_Find)
   {
      Text.Empty ();
      for (idx = 0; searchText [idx] != 0; idx ++)
      {
         c = StoryTracker.GetNextCharacter ();
         // Not enough text left, can't be a match
         if (c == TrackerEOF)
            break;
         if (c < ' ')
            break;
         if (c > 255)
            break;
         Text += (TCHAR)c;
      }
      // If we didn't run out of story text...
      if (c != TrackerEOF)
      {
         if (searchText [idx] == 0)   // Did we get a string the size of input string?
         {
            c = StoryTracker.GetNextCharacter (FALSE);
            // Did we find it?
            if (
                  (
                     ((docase == TRUE) && (Text.Compare (searchText) == 0))
                     ||
                     ((docase == FALSE) && (Text.CompareNoCase (searchText) == 0))
                  )
                  &&
                  (
                     ((wholeword == FALSE) || ((c == ' ') || (ispunct (c) != 0)))
                  )
               )
            {
               //FindReplaceState = STATE_Found  - NO! THERE MIGHT BE MULTIPLES OF THIS TEXT!;
               SearchIndexEnd = StoryTracker.Tell ();
         
               if ((Frame = StoryTracker.GetFrame ()) != 0)
					{
                  // replace text
                  CTxp txp(NULL);
                  txp.Init(Frame, pDoc->get_database(), SearchIndexStart);
                  txp.CalcStyle ();
                  // Anything to insert?
						int nLength = strlen(insertText);
				  if (nLength > 0){
					   txp.InsertString (insertText);
				  }
                  txp.SelectText(SearchIndexStart+nLength, SearchIndexEnd+nLength);
                  txp.DeleteText(1);
						StoryTracker.Seek (SearchIndexStart + nLength);
               }
				else
				{
					StoryTracker.Seek (SearchIndexStart + 1);
				}
               //break - NO! THERE MIGHT BE MULTIPLES OF THIS TEXT!;
            }
            else
               StoryTracker.Seek (++SearchIndexStart);
         }
         else
         {  // We bailed because of a wierd character - start is now just after it
            SearchIndexStart = StoryTracker.Tell ();
         }
      }
      else
      {  // Move on to next story (or FindReplaceState will = STATE_Done)
         if (StoryTracker.GetStoryNext () == FALSE)
         {
            FindReplaceState = STATE_Done;
         }
         else
         {
            FindReplaceState = STATE_Find;
         }
         SearchIndexStart = StoryTracker.Tell ();
      }
   }
}

void CWView::FindAndReplace(CPmwDoc* pDoc,       // open doc
                            LPCSTR searchText,  CHARACTER* insertText, int strlength) // string 
{
   int               idx;
   CString           Text;
   CHARACTER         c;
   DB_RECORD_NUMBER  Frame;
   CHARACTER_INDEX   SearchIndexEnd;
   CHARACTER_INDEX   SearchIndexStart;

   BOOL docase = FALSE;
   BOOL wholeword = FALSE;
   enum eFindReplaceState2  FindReplaceState = STATE_Find;
   CStoryTracker StoryTracker(pDoc);

   SearchIndexStart = StoryTracker.Tell ();
   // Look through stories for text
   while (FindReplaceState == STATE_Find)
   {
      Text.Empty ();
      for (idx = 0; searchText [idx] != 0; idx ++)
      {
         c = StoryTracker.GetNextCharacter ();
         // Not enough text left, can't be a match
         if (c == TrackerEOF)
            break;
         if (c < ' ')
            break;
         if (c > 255)
            break;
         Text += (TCHAR)c;
      }
      // If we didn't run out of story text...
      if (c != TrackerEOF)
      {
         if (searchText [idx] == 0)   // Did we get a string the size of input string?
         {
            c = StoryTracker.GetNextCharacter (FALSE);
            // Did we find it?
            if (
                  (
                     ((docase == TRUE) && (Text.Compare (searchText) == 0))
                     ||
                     ((docase == FALSE) && (Text.CompareNoCase (searchText) == 0))
                  )
                  &&
                  (
                     ((wholeword == FALSE) || ((c == ' ') || (ispunct (c) != 0)))
                  )
               )
            {
               FindReplaceState = STATE_Found;
               SearchIndexEnd = StoryTracker.Tell ();
         
               if ((Frame = StoryTracker.GetFrame ()) != 0)
					{
                  // replace text
                  CTxp txp(NULL);
                  txp.Init(Frame, pDoc->get_database(), SearchIndexStart);
                  txp.CalcStyle ();
                  // Anything to insert?
				  int nLength = strlength;
				  if (nLength > 0){
					   txp.InsertText(insertText, nLength);
				  }
                  txp.SelectText(SearchIndexStart+nLength, SearchIndexEnd+nLength);
                  txp.DeleteText(1);
						StoryTracker.Seek (SearchIndexStart + nLength);
               }
					else
					{
						StoryTracker.Seek (SearchIndexStart + 1);
					}
               break;
            }
            else
               StoryTracker.Seek (++SearchIndexStart);
         }
         else
         {  // We bailed because of a wierd character - start is now just after it
            SearchIndexStart = StoryTracker.Tell ();
         }
      }
      else
      {  // Move on to next story (or FindReplaceState will = STATE_Done)
         if (StoryTracker.GetStoryNext () == FALSE)
         {
            FindReplaceState = STATE_Done;
         }
         else
         {
            FindReplaceState = STATE_Find;
         }
         SearchIndexStart = StoryTracker.Tell ();
      }
   }
}

