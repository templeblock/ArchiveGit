// FindText.cpp : implementation file
//

/*
// $Workfile: FINDTEXT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FINDTEXT.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 3     7/25/98 2:44p Rlovejoy
// Check for NULL object in IsSkipTest().
// 
// 2     1/30/98 11:13a Johno
// Compile update
// 
//    Rev 1.0   14 Aug 1997 15:20:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:28   Fred
// Initial revision.
// 
//    Rev 1.9   15 Jul 1997 10:29:32   Jay
// Fixed text replacement
// 
//    Rev 1.8   01 May 1997 14:52:34   johno
// Added IsSkipText, functions to test if found text should be skipped.
// For example, calendar date numbers and weekday names.
// 
// 
//    Rev 1.7   30 Apr 1997 11:58:54   johno
// Fixed infinite loop bug
// when doing "Replace All" on
// 's' with 'ss', for example.
// (Around line 265)
// 
//    Rev 1.6   10 Feb 1997 17:39:46   johno
// Save last replace text as well as last find text
// 
//    Rev 1.5   10 Feb 1997 13:37:44   johno
// Done ! (?)
// 
//    Rev 1.4   07 Feb 1997 15:46:38   johno
// Seems to work...
// 
//    Rev 1.3   06 Feb 1997 17:27:12   johno
// ...
// 
// 
//    Rev 1.2   06 Feb 1997 14:09:28   johno
// ...
// 
//    Rev 1.1   06 Feb 1997 14:07:58   johno
// Finds, displays text
// 
//    Rev 1.0   03 Feb 1997 17:29:04   johno
// Initial revision.
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "frameobj.h"
#include "calinfo.h"
#include "FindText.h"

//#define  JTRACE_ON
#include "jtrace.h"

#ifndef  WIN32
   #include <ctype.h>
#endif   

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const enum eFindReplaceState
{
   STATE_Error,
   STATE_Start,
   STATE_Find,
   STATE_Found,
   STATE_Done
};

/////////////////////////////////////////////////////////////////////////////
BOOL
CStoryTracker::GetStoryNext (void)
{
   KillText ();

   m_StoryCurrent = NextAfter (m_StoryCurrent);
   
   if (m_StoryCurrent == StoryInvalid) // Can't get next story
   {
      Init ();
      return FALSE;
   }
   
   if (m_StoryStart == StoryInvalid)   // Set end here if uninitialized
      m_StoryStart = m_StoryCurrent;
   else  
   if (m_StoryCurrent == m_StoryStart) // Reached end
   {
      Init ();
      return FALSE;
   }

   return TRUE;
}

CHARACTER
CStoryTracker::GetNextCharacter (BOOL seek)
{
   CHARACTER      c;
   CTextRecord*   pTR;
   // Can't get text pointer
   if ((pTR = GetStoryTextPointer ()) == NULL)
      return TrackerEOF;
   // At end of text
   if (m_CharacterIndex >= pTR->NumberOfCharacters ())
      return TrackerEOF;
   
   c = *pTR->GetCharacter (m_CharacterIndex);
   // "push" text pointer
   if (seek == TRUE)
      m_CharacterIndex ++;
   
   return c;
}

BOOL
CStoryTracker::IsSkipText (DB_RECORD_NUMBER Frame)
{
   SHORT          Panel;
   CFrameObject   *pObject;

   pObject = m_pDoc->get_database()->FindFrameObject (Frame);
   
	if (pObject != NULL)
	{
		Panel = pObject->get_panel ();

		if ((Panel >= LABEL_PANEL) && (Panel <= GRAPHIC_PANEL))
			return TRUE;
	}   
   return FALSE;
}

BOOL
CStoryTracker::IsSkipText (void)
{
   CTextRecord*      pTR;
   DB_RECORD_NUMBER  dbrn;
   // Can't get text pointer
   if ((pTR = GetStoryTextPointer ()) == NULL)
      return TRUE;

   dbrn = pTR->GetFrameOfCharacter (m_CharacterIndex);
   
   return IsSkipText (dbrn);
}

CTextRecord*
CStoryTracker::GetStoryTextPointer (void)
{
   if (m_pText == NULL)
   {
      if (GetStoryNumber () == StoryInvalid)
         GetStoryNext ();

      if (GetStoryNumber () != StoryInvalid)
      {
         DB_RECORD_NUMBER lStory = m_pDocRec->GetStory(GetStoryNumber ());
         m_pText = (CTextRecord*)m_pDoc->get_database()->get_record (lStory, NULL, RECORD_TYPE_Text);
      }
   }

   return m_pText;
}
/////////////////////////////////////////////////////////////////////////////
// CFindReplaceText dialog

CFindReplaceText::CFindReplaceText(CWnd* pParent /*=NULL*/)
   : CFindReplaceDialogModal (pParent)
{
   //{{AFX_DATA_INIT(CFindReplaceText)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   m_pView = NULL;
   m_pDoc = NULL;
}


void CFindReplaceText::DoDataExchange(CDataExchange* pDX)
{
   CFindReplaceDialogModal::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFindReplaceText)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindReplaceText, CFindReplaceDialogModal)
   //{{AFX_MSG_MAP(CFindReplaceText)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

int
CFindReplaceText::DoModal (CPmwView* pView, BOOL FindOnly, DWORD flags)
{
   int         i = IDCANCEL;
   const char  *FindWhat = NULL;
   const char  *ReplaceWith = NULL;

   m_pView = pView;

   ASSERT (m_pView != NULL);
   if (m_pView == NULL)
      return IDCANCEL;

   m_pDoc = pView->GetDocument();
   ASSERT (m_pDoc != NULL);

   if (m_pDoc == NULL)
      return IDCANCEL;

   if (LastSearchText.IsEmpty () == 0)
      FindWhat = LastSearchText;
   
   if (LastReplaceText.IsEmpty () == 0)
      ReplaceWith = LastReplaceText;

   TRY
   {
      m_pStoryTracker = new CStoryTracker (m_pDoc);

      FindReplaceState = STATE_Start;

      i = CFindReplaceDialogModal::DoModal (FindOnly, FindWhat, ReplaceWith, flags);
   
      delete m_pStoryTracker;
   }
   CATCH_ALL (e)
   {
   }
   END_CATCH_ALL

   m_pView = NULL;
   m_pDoc = NULL;
   
   return i;
}

void
CFindReplaceText::FindNextText (LPCSTR TextLook, BOOL docase, BOOL wholeword)
{
   int               idx;
   CString           Text;
   CHARACTER         c;
   DB_RECORD_NUMBER  Frame;
   CHARACTER_INDEX   SearchIndexEnd;

   JTRACE1 ("FindNextText, story is %d\n", m_pStoryTracker->GetStoryNumber ());

   m_SearchIndexStart = m_pStoryTracker->Tell ();
   // Look through stories for text
   while (FindReplaceState == STATE_Find)
   {
      Text.Empty ();

      for (idx = 0; TextLook [idx] != 0; idx ++)
      {
         c = m_pStoryTracker->GetNextCharacter ();
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
         if (TextLook [idx] == 0)   // Did we get a string the size of input string?
         {
            c = m_pStoryTracker->GetNextCharacter (FALSE);
            // Did we find it?
            if (
                  (
                     ((Frame = m_pStoryTracker->GetFrame ()) != 0)
                     &&
                     (m_pStoryTracker->IsSkipText (Frame) == FALSE)
                  )
                  &&
                  (
                     ((docase == TRUE) && (Text.Compare (TextLook) == 0))
                     ||
                     ((docase == FALSE) && (Text.CompareNoCase (TextLook) == 0))
                  )
                  &&
                  (
                     ((wholeword == FALSE) || ((c == ' ') || (ispunct (c) != 0)))
                  )
               )
            {
               JTRACE ("Text Found\n");
               FindReplaceState = STATE_Found;
               SearchIndexEnd = m_pStoryTracker->Tell ();
         
               m_pView->ViewText (Frame, m_SearchIndexStart, SearchIndexEnd, FALSE);

               // m_pStoryTracker->Seek (m_SearchIndexStart + 1);
               // JRO this causes an infinite loop doing 'Replace All'
               // on 's' with 'ss', for example.
               // Start next search AFTER replaced text.
               m_pStoryTracker->Seek (SearchIndexEnd + 1);
               break;
            }
            else
               m_pStoryTracker->Seek (++ m_SearchIndexStart);
         }
         else
         {  // We bailed because of a wierd character - start is now just after it
            m_SearchIndexStart = m_pStoryTracker->Tell ();
         }
      }
      else
      {  // Move on to next story (or FindReplaceState will = STATE_Done)
         NextStory ();
         m_SearchIndexStart = m_pStoryTracker->Tell ();
      }
   }
}

void
CFindReplaceText::NextStory (void)
{
   JTRACE ("NextStory\n");
   // Get the next story in our sequence
   if (m_pStoryTracker->GetStoryNext () == FALSE)
   {
      FindReplaceState = STATE_Done;
      JTRACE ("Done\n");
   }
   else
   {
      FindReplaceState = STATE_Find;
      JTRACE1 ("Story is %d\n", m_pStoryTracker->GetStoryNumber ());
   }
}

void
CFindReplaceText::ReplaceText (LPCSTR Text)
{
   CTxp* pTxp;

   JTRACE ("ReplaceText\n");
   // Delete selected text, insert new text
   if ((pTxp = m_pView->GetTxp()) != NULL)
   {
      if (pTxp->SomethingSelected ())
      {
         pTxp->CalcStyle ();

         // Anything to insert?
			int nLength = strlen(Text);
         if (nLength > 0)
            pTxp->InsertString (Text);
			else
				pTxp->DeleteText (1);

         m_pDoc->SetModifiedFlag ();
      }
   }
}

#define  DOCASE      (((pFR->Flags & FR_MATCHCASE) == 0) ? FALSE : TRUE)
#define  WHOLEWORD   (((pFR->Flags & FR_WHOLEWORD) == 0) ? FALSE : TRUE)

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceText message handlers
long 
CFindReplaceText::OnFindReplace (WPARAM wParam, LPARAM lParam)
{
   CString        TextFind;
   LPFINDREPLACE  pFR = (LPFINDREPLACE)lParam;

   JTRACE ("\n");

   TextFind = pFR->lpstrFindWhat;
   LastReplaceText = pFR->lpstrReplaceWith;
   
   switch (FindReplaceState)
   {
      case STATE_Start:
      LastSearchText = TextFind;
      NextStory ();
      break;
   
      case STATE_Done:
      if (TextFind != LastSearchText)
         LastSearchText = TextFind;
      //m_pStoryTracker->Init ();
      NextStory ();
      break; 

      case STATE_Found:
      if (TextFind != LastSearchText)  // User changed text - Current story is now the start
      {
         LastSearchText = TextFind;
         JTRACE ("Restarting\n");
         m_pStoryTracker->ReStart ();
      }
      break;
   }
   
   if (pFR->Flags & FR_DIALOGTERM)
   {
      FindReplaceState = STATE_Done;
   }

   if (pFR->Flags & FR_FINDNEXT)    // User clicked "Find Next"
   {
      // If we found text last time, switch to find
      if (FindReplaceState == STATE_Found)
         FindReplaceState = STATE_Find;
      
      FindNextText (TextFind, DOCASE, WHOLEWORD);
      
      if (FindReplaceState == STATE_Done)
         AfxMessageBox (IDS_SRCH_REP_DONE, MB_OK);
   }
   
   if (pFR->Flags & FR_REPLACE)     // User clicked "Replace"
   {
      if (FindReplaceState == STATE_Found)
      {
         ReplaceText (pFR->lpstrReplaceWith);
         FindReplaceState = STATE_Find;
      }

      FindNextText (TextFind, DOCASE, WHOLEWORD);
      
      if (FindReplaceState == STATE_Done)
         AfxMessageBox (IDS_SRCH_REP_DONE, MB_OK);
   }
   
   if (pFR->Flags & FR_REPLACEALL)  // User clicked "Replace All"
   {
      m_pStoryTracker->ReStart ();
      while (FindReplaceState != STATE_Done)
      {
         if (FindReplaceState == STATE_Found)
         {
            ReplaceText (pFR->lpstrReplaceWith);
            FindReplaceState = STATE_Find;
         }

         FindNextText (TextFind, DOCASE, WHOLEWORD);
      
         if (FindReplaceState == STATE_Done)
            AfxMessageBox (IDS_SRCH_REP_DONE, MB_OK);
      }
   }
   
#if 0
   // Function

   if (pFR->Flags & FR_ENABLEHOOK)
   {
      int i = 0;
   }

   if (pFR->Flags & FR_ENABLETEMPLATE)
   {
      int i = 0;
   }
   
   if (pFR->Flags & FR_ENABLETEMPLATEHANDLE)
   {
      int i = 0;
   }
   
   // Appearance

   if (pFR->Flags & FR_HIDEUPDOWN)
   {
      int i = 0;
   }
   
   if (pFR->Flags & FR_HIDEMATCHCASE)
   {
      int i = 0;
   }
   
   if (pFR->Flags & FR_HIDEWHOLEWORD)
   {
      int i = 0;
   }
   
   if (pFR->Flags & FR_NOMATCHCASE)
   {
      int i = 0;
   }
   
   if (pFR->Flags & FR_NOUPDOWN)
   {
      int i = 0;
   }
   
   if (pFR->Flags & FR_NOWHOLEWORD)
   {
      int i = 0;
   }
   
   if (pFR->Flags & FR_SHOWHELP)
   {
      int i = 0;
   }
   
   // Selections

   if (pFR->Flags & FR_DOWN)
   {
      int i = 0;
   }

   if (pFR->Flags & FR_MATCHCASE)
   {
      int i = 0;
   }
   
   if (pFR->Flags & FR_WHOLEWORD)
   {
      int i = 0;
   }
#endif      
   return 0;
}
/////////////////////////////////////////////////////////////////////////////
