// FindText.h : header file
//

#include "pmw.h"
#include "findrep.h"
#include "textrec.h"

static   CString  LastSearchText;
static   CString  LastReplaceText;

const DWORD StoryInvalid = (DWORD)-1;
const int   TrackerEOF   = (CHARACTER)-1;

/////////////////////////////////////////////////////////////////////////////
// CStoryTracker - class to help track PrintMaster stories
class CStoryTracker
{
public:
   
   CStoryTracker (CPmwDoc  *pDoc)
   {
      m_pDoc = pDoc;
      ASSERT (m_pDoc != NULL);

      m_pDocRec = m_pDoc->DocumentRecord();
      ASSERT (m_pDocRec != NULL);

      m_pText = NULL;
   
      Init ();
   }

   ~CStoryTracker ()
   {
      KillText ();
   }

   void
   Init (void)
   {
      KillText ();
      m_StoryCurrent = StoryInvalid;
      m_StoryStart   = StoryInvalid;
   }
   // Causes GetStoryNext to loop back to current story
   void
   ReStart (void)
   {
      m_StoryStart = m_StoryCurrent;
      m_CharacterIndex = 0;
   }

   DWORD
   GetStoryNumber (void)
   {
      return m_StoryCurrent;
   }

   BOOL
   GetStoryNext (void);

   void
   Seek (CHARACTER_INDEX ci)
   {
      m_CharacterIndex = ci;
   }

   CHARACTER_INDEX
   Tell (void)
   {
      return m_CharacterIndex; 
   }

   CHARACTER
   GetNextCharacter (BOOL seek = TRUE);

   BOOL
   IsSkipText (void);

   BOOL
   IsSkipText (DB_RECORD_NUMBER Frame);

   DB_RECORD_NUMBER
   GetFrame (void)
   {
      DB_RECORD_NUMBER  dbrn = 0;
      CTextRecord*      pTR;

      if ((pTR = GetStoryTextPointer ()) != NULL)
         dbrn = pTR->GetFrameOfCharacter (m_CharacterIndex);

      return dbrn;
   }

private:

   DWORD             m_StoryCurrent, m_StoryStart;
   DocumentRecord    *m_pDocRec;
   CTextRecord       *m_pText;
   CPmwDoc           *m_pDoc;
   CHARACTER_INDEX   m_CharacterIndex;

   DWORD
   NextAfter (DWORD rn)
   {
      DWORD  x, ns;

      x = rn;
      if (x == StoryInvalid)
         x = 0;
      else
         x += 1;

      ns = m_pDocRec->NumberOfStories ();

      if (x >= ns)
         x = 0;

      if (x >= ns)
         x = StoryInvalid;

      return x;
   }

   CTextRecord*
   GetStoryTextPointer (void);

   void
   KillText (void)
   {
      if (m_pText != NULL)
      {
         m_pText->release();
         m_pText = NULL;
      }
      
      m_CharacterIndex = 0;
   }
};

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceText dialog

class CFindReplaceText : public CFindReplaceDialogModal
{
// Construction
public:
	CFindReplaceText(CWnd* pParent = NULL);   // standard constructor

// Overrides

   virtual int
   DoModal (CPmwView* pView, BOOL FindOnly, DWORD flags = FR_DOWN);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindReplaceText)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual long
   OnFindReplace (WPARAM wParam, LPARAM lParam);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindReplaceText)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   CPmwView                *m_pView;
   CPmwDoc                 *m_pDoc;
   CStoryTracker           *m_pStoryTracker;
   enum eFindReplaceState  FindReplaceState;
   CHARACTER_INDEX         m_SearchIndexStart;

   void
   NextStory (void);

   void
   FindNextText (LPCSTR Text, BOOL docase, BOOL wholeword);

   void
   ReplaceText (LPCSTR Text);

};
