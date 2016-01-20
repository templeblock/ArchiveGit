// findrep.h : header file
//

static UINT wm_findreplace = ::RegisterWindowMessage (FINDMSGSTRING);

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDialogModal window

class CFindReplaceDialogModal : public CWnd
{
// Construction
public:
	CFindReplaceDialogModal (CWnd *pParent = NULL);

// Attributes
public:

// Operations
public:
   virtual BOOL 
   Create (LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL)
   {
      return 0;
   }

   BOOL 
   CreateEx (DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu, LPVOID lpParam = NULL)
   {
      return FALSE;
   }
  
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindReplaceDialogModal)
	//}}AFX_VIRTUAL

   virtual int
   DoModal (BOOL FindOnly, LPCSTR FindWhat, LPCSTR ReplaceWidth = NULL, DWORD flags = FR_DOWN);

// Implementation
public:
	virtual ~CFindReplaceDialogModal();
   
   virtual void 
   EndDialog (int nEndID); // used to terminate a modal dialog
	
   virtual BOOL 
   DestroyWindow();

	virtual long
   OnFindReplace (WPARAM wParam, LPARAM lParam);

   // Generated message map functions
protected:
	//{{AFX_MSG(CFindReplaceDialogModal)
		// NOTE - the ClassWizard will add and remove member functions here.
   afx_msg long InternalFindReplace (WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   int m_nID;

private:
   BOOL  m_fDone;
   BOOL  m_fParentDisabled;
   CWnd  *m_pParentWnd;
   
   BOOL 
   PumpMessage();
};

/////////////////////////////////////////////////////////////////////////////
