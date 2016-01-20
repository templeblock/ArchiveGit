// toolbarx.h : interface of the CToolBarEx, CBubble, and CFrame classes
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// NOTE: MFC Undocumented feature usage
//
//  This code relies on internal knowledge of the class and structures
//  used to implement the MFC 2.0/2.5 CToolBar, and CFrame classes.
//
//  It relies on the following undocumented member functions from CToolBar
//    CToolBar::HitTest()
//
//  It uses the following undocumented messages and defines from afxpriv.h
//    WM_SETMESSAGESTRING
//    AFX_IDS_IDLEMESSAGE
//
//  This code is compatible with:
//    - MFC verions 2.0 and 2.5 for Win16 and Win32
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(TBX_FRAME) && !defined(TBX_MDIFRAME) && !defined(TBX_OLEIPFRAME)
#error Either TBX_FRAME,TBX_MDIFRAME, or TBX_OLEIPFRAME must be globally defined to include the proper CFrame class
#endif  


/////////////////////////////////////////////////////////////////////////////
// control defines

#define TBX_TIMER_OFF -1
#define TBX_TIMER_PENDING 1

#define TBX_TIMER_ID 2010
#define TBX_TIMER_TIME 1000

#define TBX_FONT_SIZE -8

#define TBX_BUBBLE_DELAY 100



/////////////////////////////////////////////////////////////////////////////
// CBubble

class CBubble : public CWnd
{
  DECLARE_DYNAMIC(CBubble);

// Construction
public:
  CBubble();

// Attributes
private:  
  static CString near m_strClassName;  // bubble window class name
  
  static CFont   near m_fntFont;       // bubble window font
  static int     near m_nBubbleHeight; // bubble window height
  
  BOOL    m_bCreated;             // bubble window creation flag
  BOOL    m_bBubbleOn;            // bubble window ON/OFF status flag
  
  int     m_nTimerMode;           // current timer mode
  int     m_nID;                  // ID of the item we're tracking

public:
  BOOL IsCreated() { return m_bCreated; }   // creation status


// Operations
public:
  BOOL Create();      // create the bubble window
  virtual BOOL DestroyWindow();  // destroy the window

  void Off();         // turn the bubble OFF
  
  // request the bubble window to track with the specified text and string resource ID
  void Track(CPoint pt, const CString& string, int nID);
  
  void Hide();        // hide the bubble window
  
private:  
  void StartTimer();              // start the timer
  void StopTimer();               // stop timer


// Implementation
public:
  virtual ~CBubble();

  // add GetOwner() and SetOwner() for MFC < 2.5
#if (_MFC_VER < 0x0250)
  // GetOwner() and SetOwner() are provided in MFC >= 2.5 only.
  // We use them for OLE 2.0 in-place activation support
  // Since OLE 2.0 is only in MFC 2.5, we add these functions
  // to allow our code to still work in MFC < 2.5
  HWND hwndOwner;  // use HWND because the CWnd might be temporary
  void SetOwner(CWnd* pOwner) { hwndOwner = pOwner->GetSafeHwnd(); }
  CWnd* GetOwner() const { return (hwndOwner ? FromHandle(hwndOwner) : GetParent()); }
#endif
  
  // Generated message map functions
protected:
  //{{AFX_MSG(CBubble)
  afx_msg void OnPaint();
  afx_msg LONG OnSetText(UINT, LONG lParam);
  afx_msg void OnTimer(UINT nIDEvent);
   //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CToolBarEx

class CToolBarEx : public CToolBar
{

  DECLARE_DYNAMIC(CToolBarEx);
  

// contruction/destruction  
public:
  CToolBarEx();
  virtual ~CToolBarEx();
  
// attributes
public:

protected:
  // NOTE: One bubble window is shared by all CToolBarEx's
  static CBubble  near m_Bubble;  // CBubble window
  
  void
  NotifyMainWindow (void)
{
 POINT   point;
 ::GetCursorPos(&point);  // in screen coorinates
 CWnd *pWnd = AfxGetMainWnd ();
 pWnd->ScreenToClient (&point);
 pWnd->PostMessage (WM_PARENTNOTIFY, WM_LBUTTONDOWN, MAKELONG (point.x, point.y));
}
  
private:  
  // these are static so the hook can get to them
  static BOOL     near m_bCaptured;      // we mouse captured flag
  static int      near m_nButtonIndex;   // current index of button we're over
  static int      near m_nChildID;       // current ID of the child window we're tracking
  static HHOOK    near m_hHook;          // handle to the keyboard hook installed
  static int      near m_nToolBars;      // number of toolbar we have created
  static BOOL     near m_bWin31;         // window version flag
  
  
// operations   
public:
  // create the toolbar
  BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP, UINT nID = AFX_IDW_TOOLBAR);
  
  // pretranslate messages
  BOOL PreTranslateMessage(MSG* msg);
  
protected:
  // show and track the bubble window
  void TrackBubble(BOOL bTrackingButton = TRUE);   
  
  // cancel the bubble window
  static void CancelBubble();      // static so the hook can use it

  // determine if the window we're over is a candidate for
  // tranfering the bubble window to
  BOOL IsTransferable(CWnd* pcwndOver);
 
// implementation  
private:  
   
  // pass the bubble control to another window
  void TransferBubble(BOOL bMode = TRUE);
  
  // get the first descendant window for the CToolBarEx from a child window
  CWnd* GetFirstDescendantWindow(CWnd* pcwndChild);
  
  // install and remove a keyboard hook
  void InstallKeyboardHook();
  void RemoveKeyboardHook();
  
  // keyboard hook procedure (used to disable bubble when a key is pressed)
  // is static so the callback can be a member of this class
  static LRESULT CALLBACK AFX_EXPORT KeyboardHookCallback(int code, WPARAM wParam, LPARAM lParam);
  
  // add GetOwner() and SetOwner() for MFC < 2.5
#if (_MFC_VER < 0x0250)
  // GetOwner() and SetOwner() are provided in MFC >= 2.5 only.
  // We use them for OLE 2.0 in-place activation support
  // Since OLE 2.0 is only in MFC 2.5, we add these functions
  // to allow our code to still work in MFC < 2.5
  HWND hwndOwner;  // use HWND because the CWnd might be temporary
  void SetOwner(CWnd* pOwner) { hwndOwner = pOwner->GetSafeHwnd(); }
  CWnd* GetOwner() const { return (hwndOwner ? FromHandle(hwndOwner) : GetParent()); }
#endif
  
  
// Generated message map functions
protected:
  //{{AFX_MSG(CToolBarEx)
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnCancelMode();
   //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  
};
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CFrameWndEx

#if defined(TBX_FRAME) && (_MFC_VER >= 0x0250)
// only used for MFC 2.5 and greater

class CFrameWndEx : public CFrameWnd
{
  DECLARE_DYNCREATE(CFrameWndEx)
public:
  CFrameWndEx();

// Implementation
public:
  virtual ~CFrameWndEx();
  
  // Generated message map functions
protected:
  //{{AFX_MSG(CFrameWndEx)
  afx_msg LONG OnSetMessageString(UINT wParam, LONG lParam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};
// automatic class substitution
#define CFrameWnd CFrameWndEx

#endif  // TBX_FRAME && _MFC_VER >= 0x0250


/////////////////////////////////////////////////////////////////////////////
// CMDIFrameWndEx

#if defined(TBX_MDIFRAME) && (_MFC_VER >= 0x0250)
// only used for MFC 2.5 and greater

class CMDIFrameWndEx : public CMDIFrameWnd
{
  DECLARE_DYNCREATE(CMDIFrameWndEx)
public:
  CMDIFrameWndEx();

// Implementation
public:
  virtual ~CMDIFrameWndEx();
  
  // Generated message map functions
protected:
  //{{AFX_MSG(CMDIFrameWndEx)
  afx_msg LONG OnSetMessageString(UINT wParam, LONG lParam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};
// automatic class substitution
#define CMDIFrameWnd CMDIFrameWndEx

#endif  // TBX_MDIFRAME && _MFC_VER >= 0x0250


/////////////////////////////////////////////////////////////////////////////
// COleIPFrameWndEx

#if defined(TBX_OLEIPFRAME) && (_MFC_VER >= 0x0250)
// only used with MFC 2.5 and greater

class COleIPFrameWndEx : public COleIPFrameWnd
{
  DECLARE_DYNCREATE(COleIPFrameWndEx)
public:
  COleIPFrameWndEx();

// Implementation
public:
  virtual ~COleIPFrameWndEx();
  
  // Generated message map functions
protected:
  //{{AFX_MSG(COleIPFrameWndEx)
  afx_msg LONG OnSetMessageString(UINT wParam, LONG lParam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};
// automatic class substitution
#define COleIPFrameWnd COleIPFrameWndEx

#endif  // TBX_OLEIPFRAME && _MFC_VER >= 0x0250


/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx window

#if (_MFC_VER < 0x0250)
// only used with MFC 2.0

class CStatusBarEx : public CStatusBar
{
  DECLARE_DYNCREATE(CStatusBarEx)
// Construction
public:
  CStatusBarEx();

// Attributes
public:

// Operations
public:

// Implementation
public:
  virtual ~CStatusBarEx();

  // Generated message map functions
protected:
  //{{AFX_MSG(CStatusBarEx)
  afx_msg LONG OnSetText(UINT, LONG lParam);
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};
// automatic class substitution
#define CStatusBar CStatusBarEx

#endif // _MFC_VER < 0x0250

/////////////////////////////////////////////////////////////////////////////


// automatic class substitution
#define CToolBar CToolBarEx
