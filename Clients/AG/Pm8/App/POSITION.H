// position.h : header file
//

#include "pmwprop.h"
#include "dlgmgr.h"

#include "poslist.h"
#include "util.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

#define  PAGE_BASE   CPositionDlgPageBase

inline  void CDECL TraceBeGone(LPCSTR, ...){}
#define  JTRACE   1 ? (void)0 :  ::TraceBeGone  // Disable tracing
//#define  JTRACE   TRACE    // Enable tracing

/////////////////////////////////////////////////////////////////////////////
class CPositionEdit : public CEdit
{
public:
   BOOL
   GetTypeIn (double *pd)
   {
      BOOL     ret = FALSE;
      double   d;
      CString  Text;
      
      GetWindowText (Text);
      if (Util::ValidDouble ((const char *)Text) == TRUE)
      {
         d = atof ((const char *)Text);
         
         if (d > 10000.0)
            d = 10000.0;
         
         d = d * 1800.0;
         *pd = d;
         ret = TRUE;
      }
   
      return ret;
   }

   BOOL
   GetTypeInPerCent (double *pd)
   {
      BOOL     ret = FALSE;
      double   d;
      CString  Text;
      
      GetWindowText (Text);
      if (Util::ValidDouble ((const char *)Text) == TRUE)
      {
         d = atof ((const char *)Text);
         
         if (d > 10000.0)
            d = 10000.0;
         
         d = d / 100.0;
         *pd = d;
         ret = TRUE;
      }
   
      return ret;
   }

   void
   SetText (double d)
   {
      CString  str;

      JTRACE ("Setting text\n");

#ifdef LOCALIZE
      Util::FormatDouble (d, -(CurrentLocale.ConvertPrecisionFromInches(3)), str);
#else
      Util::FormatDouble (d, -3, str);
#endif
      
      SetWindowText (str);
   }

   void
   SetTextPerCent (double d)
   {
      double x = d * 100;

      SetText (x);
   }
private:
};

/////////////////////////////////////////////////////////////////////////////
//// Contains code common to the pages used here
class CPositionDlgPageBase : public CPmwPropertyPage
{
// Construction
public:
   CPositionDlgPageBase(int ID);
   virtual ~CPositionDlgPageBase();

// Dialog Data
   //{{AFX_DATA(CPositionDlgPageBase)
   //}}AFX_DATA

   void
   //SetObjectList (TPositionList *pCurrent, TPositionList *pOriginal)
   SetObjectList (TPositionList *pCurrent)
   {
      m_pObjectListCurrent = pCurrent;
      //m_pObjectListOriginal = pOriginal;
   }

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CPositionDlgPageBase)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnSetActive ();
   virtual BOOL OnKillActive ();
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CPositionDlgPageBase)
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   TPositionList  *m_pObjectListCurrent, *m_pObjectListOriginal, *m_pObjectListSave;
protected:
   void
   UncheckButtons (int id [])
   {
      int      i = 0;
      CButton  *pBut;
   
      JTRACE ("Unchecking buttons\n");
      
      while (id [i] != 0)
      {
         if ((pBut = (CButton*)GetDlgItem (id [i])) != NULL)
            pBut->SetCheck (0);
         i ++;
      }
   }

   BOOL
   SetControlText (int id, const char *Text)
   {
      CWnd  *pWindow;

      if ((pWindow = GetDlgItem (id)) != NULL)
      {
         pWindow->SetWindowText ("");
         return TRUE;
      }

      return FALSE;
   }
private:
};

/////////////////////////////////////////////////////////////////////////////
// CPagePosition dialog

class CPagePosition : public PAGE_BASE
{
   //DECLARE_DYNCREATE(CPagePosition)

// Construction
public:
   CPagePosition();
   virtual ~CPagePosition();

// Dialog Data
   //{{AFX_DATA(CPagePosition)
   enum { IDD = IDD_POSITION_POSITION };
   int      m_nPositionSelectionHoriz;
   int      m_nPositionSelectionVert;
   //}}AFX_DATA

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CPagePosition)
   public:
   virtual BOOL OnSetActive();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CPagePosition)
   afx_msg void OnPositionChange();
   afx_msg void OnChangePositionInchv();
   afx_msg void OnChangePositionInchh();
   afx_msg void OnClickedPositionAth();
   afx_msg void OnClickedPositionAtv();
   afx_msg void OnSetfocusPositionInchh();
   afx_msg void OnSetfocusPositionInchv();
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   static int  ButtonIDLeft [];
   static int  ButtonIDRight [];
   
   void
   ApplyPosition ();

   BOOL
   DoInchH (void);

   BOOL
   DoInchV (void);

   void
   UncheckAll (void)
   {
      UncheckButtons (ButtonIDLeft);
      UncheckButtons (ButtonIDRight);
   }

   void
   SetEditText (void);
};

/////////////////////////////////////////////////////////////////////////////
// CPageSize dialog

class CPageSize : public PAGE_BASE
{
   //DECLARE_DYNCREATE(CPageSize)

// Construction
public:
   CPageSize();
   ~CPageSize();

// Dialog Data
   //{{AFX_DATA(CPageSize)
   enum { IDD = IDD_POSITION_SIZE };
   int      m_nSizeSelectionWidth;
   int      m_nSizeSelectionHeight;
   //}}AFX_DATA

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CPageSize)
   public:
   virtual BOOL OnSetActive();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CPageSize)
   afx_msg void OnSizeRadioChange();
   virtual BOOL OnInitDialog();
   
   afx_msg void OnSetfocusPositionSizeWEdit();
   afx_msg void OnChangePositionSizeWEdit();
   afx_msg void OnClickedWidth();

   afx_msg void OnSetfocusPositionSizeWScaleEdit();
   afx_msg void OnChangePositionSizeWScaleEdit();
   afx_msg void OnClickedScaleW();

   afx_msg void OnSetfocusPositionSizeHEdit();
   afx_msg void OnChangePositionSizeHEdit();
   afx_msg void OnClickedHeight();

   afx_msg void OnSetfocusPositionSizeHScaleEdit();
   afx_msg void OnChangePositionSizeHScaleEdit();
   afx_msg void OnClickedScaleH();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   BOOL        m_bLocalUpdate;
   static int  ButtonIDLeft [];
   static int  ButtonIDRight [];

   void
   ApplySize (void);

   BOOL
   GetLocalUpdate (void)
   {
      return m_bLocalUpdate;
   }
   
   void
   SetLocalUpdate (BOOL b)
   {
      m_bLocalUpdate = b;
   }
   void
   SetEditTextAll (void);

   void
   UncheckAll (void)
   {
      UncheckButtons (ButtonIDLeft);
      UncheckButtons (ButtonIDRight);
   }
};

/////////////////////////////////////////////////////////////////////////////
// CPageRelative dialog

class CPageRelative : public PAGE_BASE
{
   //DECLARE_DYNCREATE(CPageRelative)

// Construction
public:
   CPageRelative();
   ~CPageRelative();

// Dialog Data
   //{{AFX_DATA(CPageRelative)
   enum { IDD = IDD_POSITION_RELATIVE };
   int      m_nRelativeSelectionHoriz;
   int      m_nRelativeSelectionVert;
   int      m_nRelativeSelectionHeight;
   int      m_nRelativeSelectionWidth;
   //}}AFX_DATA

// Overrides
   // ClassWizard generate virtual function overrides
   //{{AFX_VIRTUAL(CPageRelative)
   public:
   virtual BOOL OnSetActive();
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   // Generated message map functions
   //{{AFX_MSG(CPageRelative)
   afx_msg void OnRelativeChange();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
   static int  ButtonIDs [];

   void
   ApplyAlignment (void);
};

/////////////////////////////////////////////////////////////////////////////
// CPositionObjectDialog dialog

class CPositionObjectDialog : public CPmwPropertySheet
{
   DECLARE_DYNAMIC(CPositionObjectDialog)

// Construction
public:
   CPositionObjectDialog (UINT nIDCaption, CWnd* pParent = NULL, UINT iSelectPage = 0);

// Dialog Data
   //{{AFX_DATA(CPositionObjectDialog)
      // NOTE: the ClassWizard will add data members here
   //}}AFX_DATA

// Implementation
protected:
   virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support

// Attributes
public:

// Operations
public:
  int
  DoModal (CPositionData* pData);

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CPositionObjectDialog)
   public:
   //}}AFX_VIRTUAL

#ifdef WIN32
   virtual BOOL OnInitDialog();
#else
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
#endif


// Implementation
public:
// virtual ~CPositionObjectDialog();

   // Generated message map functions
   //{{AFX_MSG(CPositionObjectDialog)
   afx_msg void OnReset();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   
   double         NormalizedPageWidth;
   double         NormalizedPageHeight;

   TPositionList  *m_pList, *m_pListOriginal;
   CPagePosition  m_PagePosition;
   CPageSize      m_PageSize;
   CPageRelative  m_PageRelative;

   void
   ToPositionList (CPositionData* pData);
   
   void
   FromPositionList (CPositionData* pData);
};
