/*
// $Workfile: TOOLBARX.CPP $
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/TOOLBARX.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:50   Fred
// Initial revision.
// 
//    Rev 1.3   02 Jun 1997 16:14:36   johno
// Send LBUTTONDOWN notifications to main window
// 
// 
*/

// toolbarx.cpp : implementation of the CToolBarEx, CBubble, and CFrame classes
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


#include "stdafx.h"
#include "resource.h"

#if 0
#else

#include "afxpriv.h"  //  NOTE: private to MFC

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// no automatic class substitution for this file!
#ifdef CToolBar
#undef CToolBar
#endif

#ifdef CFrameWnd
#undef CFrameWnd
#endif

#ifdef CMDIFrameWnd
#undef CMDIFrameWnd
#endif

#ifdef COleIPFrameWnd
#undef COleIPFrameWnd
#endif

#ifdef CStatusBar
#undef CStatusBar
#endif


/////////////////////////////////////////////////////////////////////////////
// CBubble

// static members
CString near CBubble::m_strClassName = "";
CFont near CBubble::m_fntFont;
int near CBubble::m_nBubbleHeight = 0;

IMPLEMENT_DYNAMIC(CBubble, CWnd);

CBubble::CBubble()
{
  // set initial timer mode
  m_nTimerMode = TBX_TIMER_OFF;
  
  // set initial mode
  m_bBubbleOn = FALSE;
  
  // set creation flag
  m_bCreated = FALSE;

  // initialize hwndOwner for GetOwner() and SetOwner() support in MFC < 2.5
#if (_MFC_VER < 0x0250)
  hwndOwner = NULL;
#endif

}
                 
CBubble::~CBubble()
{
}

BOOL CBubble::Create()
{
  // create our bubble window but leave it invisible

  // do we need to register the class?
  if(m_strClassName == "")
  {
    // yes

    // first, create the background brush
    CBrush  brBrush;
    TRY
    {
      brBrush.CreateSolidBrush(RGB(255,255,112));
    }
    CATCH(CResourceException, e)
    {
      // brush creation failed
      return 0;
    }
    END_CATCH
  
    // register the class name
    m_strClassName = ::AfxRegisterWndClass(CS_BYTEALIGNCLIENT | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW, 0 ,(HBRUSH)brBrush.Detach());
    
    // we're we successful?
    if(m_strClassName == "")
      return 0;  // return failed
  }
  
  // create the bubble window and set the created flag
  CRect rect;
  rect.SetRectEmpty();
  m_bCreated = CreateEx(0, m_strClassName, "", WS_POPUP | WS_BORDER,
                           rect.left, rect.top, rect.right, rect.bottom, 
                           NULL, (HMENU)NULL);

  // do we need to create the font?
  if(m_fntFont.GetSafeHandle() == NULL)
  {
    // create the font

    CClientDC dc(this);  // used to calculate the font size
    
    TRY 
    {
      LOGFONT        lf;
    
      memset(&lf, 0, sizeof(LOGFONT));
      
      lf.lfHeight = -::MulDiv(TBX_FONT_SIZE, dc.GetDeviceCaps(LOGPIXELSY), 72);
      lf.lfCharSet = ANSI_CHARSET;
      lf.lfQuality = DEFAULT_QUALITY;
      lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS;
      lf.lfPitchAndFamily = FF_SWISS;

      m_fntFont.CreateFontIndirect(&lf);
    }
    CATCH(CResourceException, e)
    {
      // font creation failed
      return 0;
    }
    END_CATCH

    // compute the bubble window size based on the font    
  
    // select the font we're using
    CFont* oldFont = dc.SelectObject(&m_fntFont);
      
    // get the text metrics
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);
      
    // select the old font
    dc.SelectObject(oldFont);
      
    // calc bubble window height now
    m_nBubbleHeight = tm.tmHeight + tm.tmExternalLeading + (6*GetSystemMetrics(SM_CYBORDER));
  }

  // return status
  return m_bCreated;
}

BOOL CBubble::DestroyWindow()
{
  // clear our creation flag
  m_bCreated = FALSE;

  // call the base to destroy the window
  return CWnd::DestroyWindow();
}

void CBubble::Track(CPoint point, const CString& string, int nID)
{
  // track the bubble window postion and set it's text
  
  // hide it first so it moves cleanly
  ShowWindow(SW_HIDE);
  UpdateWindow();  // update now
  
  // set the text
  SetWindowText(string);
  
  // wait a few milliseconds before we display it
  DWORD now = ::GetTickCount();
  while(::GetTickCount() < now+TBX_BUBBLE_DELAY)
    ;  // do nothing, just wait awhile
    
   CRect rect;
   GetWindowRect(&rect);

  // if the x-position is negative we must center the bubble window
  // around the absolute value of the position specified.
  if(point.x < 0)
  {
    // calc the center point
    point.x = abs(point.x)-(rect.Width()/2);
  }
          
   if (point.x < 0)
   {
      point.x = 0;
   }
   int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
   if (point.x + rect.Width() > nScreenWidth)
   {
      point.x = nScreenWidth - rect.Width();
   }

  // move the window
  SetWindowPos(&wndTop, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
  
  // save the ID for later 
  // (used in the OnTimer handler)
  m_nID = nID;

  // is the bubble currently on?
  if(m_bBubbleOn)
  {
    // yes, show the window
    ShowWindow(SW_SHOWNOACTIVATE);
    
    // tell the parent to update the CStatusBar message
    GetOwner()->SendMessage(WM_SETMESSAGESTRING, (WPARAM)m_nID);
  }
  else
  {
    // no, start the timer to display the bubble is the requested position
    StartTimer();
  }
}

void CBubble::StartTimer()
{
  // activate the delay timer if we haven't already done so
  
  // is it currently OFF
  if(m_nTimerMode == TBX_TIMER_OFF)
  {
    // start it
    if(!SetTimer(TBX_TIMER_ID, TBX_TIMER_TIME, NULL))
    {
      // the start timer has failed, so activate the bubble immediately

      // make sure the timer is off
      m_nTimerMode = TBX_TIMER_OFF;  // flag as OFF
    
      // activate the bubble window now
      ShowWindow(SW_SHOWNOACTIVATE);
    
      // tell the parent to update the CStatusBar message
      GetOwner()->SendMessage(WM_SETMESSAGESTRING, (WPARAM)m_nID);
        
      // flag as on
      m_bBubbleOn = TRUE;

    }
    else
      // flag as timer pending
      m_nTimerMode = TBX_TIMER_PENDING;  
  }
}

void CBubble::StopTimer()  
{
  // cancel a pending timer
  
  // is a timer is pending?
  if(m_nTimerMode == TBX_TIMER_PENDING)
  {
    // kill it
    KillTimer(TBX_TIMER_ID);
    
    // flag it as no longer pending
    m_nTimerMode = TBX_TIMER_OFF;
  }
}

  
void CBubble::Hide()
{
  // hide the bubble window
  ShowWindow(SW_HIDE); 
  
  // stop any pending timers
  StopTimer();
  
  // clear the CStatusBar message
  GetOwner()->SendMessage(WM_SETMESSAGESTRING, (WPARAM)AFX_IDS_IDLEMESSAGE);
}

void CBubble::Off()
{
  // hide the bubble
  Hide();
  
  // flag as off
  m_bBubbleOn = FALSE;
}





BEGIN_MESSAGE_MAP(CBubble, CWnd)
  //{{AFX_MSG_MAP(CBubble)
  ON_WM_PAINT()
  ON_MESSAGE(WM_SETTEXT, OnSetText)
  ON_WM_TIMER()
   ON_WM_LBUTTONDOWN()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBubble message handlers


void CBubble::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  
  // paint our text, centered in the window
  CRect rect;
  GetClientRect(rect);
  
  // select our font and setup for text painting
  CFont *oldFont = dc.SelectObject(&m_fntFont);
  dc.SetTextAlign(TA_CENTER);
  dc.SetBkMode(TRANSPARENT);
  dc.SetTextColor(RGB(0,0,0));  // black text
  
  // paint our text
  CString strText;
  GetWindowText(strText);
  CSize size = dc.GetTextExtent(strText, strText.GetLength());
  dc.TextOut(rect.right/2, (rect.bottom-size.cy)/2, strText);
  
  // clean up the DC
  dc.SelectObject(oldFont);
}

afx_msg LONG CBubble::OnSetText(UINT, LONG lParam)
{
  // resize the bubble window to fit the new string
  
  CRect rect;
  GetWindowRect(rect);  // get current size and position
  
  // compute new size based on string x extent
  CClientDC dc(this);
  
  CFont *oldFont = dc.SelectObject(&m_fntFont);
  CSize size = dc.GetTextExtent((LPCSTR)lParam, lstrlen((LPCSTR)lParam));
  
  // compute width
  rect.right = rect.left + size.cx + (6*GetSystemMetrics(SM_CXBORDER));
  
  // set height
  rect.bottom = rect.top + m_nBubbleHeight;
  
  // set window size
  MoveWindow(&rect);
  
  // clean up
  dc.SelectObject(oldFont);
  
  // do the default processing
  return CWnd::Default();
}

void CBubble::OnTimer(UINT nIDEvent)
{
  // first kill the timer
  KillTimer(TBX_TIMER_ID);
  
  // was a timer really pending?
  if(m_nTimerMode == TBX_TIMER_PENDING)
  {
    // yes
    m_nTimerMode = TBX_TIMER_OFF;  // flag as OFF

    // activate the bubble window now
    ShowWindow(SW_SHOWNOACTIVATE);

    // tell the parent to update the CStatusBar message
    GetOwner()->SendMessage(WM_SETMESSAGESTRING, (WPARAM)m_nID);
    
    // flag as on
    m_bBubbleOn = TRUE;
  }
  
  CWnd::OnTimer(nIDEvent);
}








/////////////////////////////////////////////////////////////////////////////
// CToolBarEx

// class static member intializers
CBubble near CToolBarEx::m_Bubble;
BOOL near CToolBarEx::m_bCaptured = FALSE;
int near CToolBarEx::m_nButtonIndex = -1;
int near CToolBarEx::m_nChildID = -1;
HHOOK near CToolBarEx::m_hHook = NULL;
int near CToolBarEx::m_nToolBars = 0;
BOOL near CToolBarEx::m_bWin31 = (BOOL)(LOWORD(::GetVersion()) >= 0x030A);
            

IMPLEMENT_DYNAMIC(CToolBarEx, CToolBar);

CToolBarEx::CToolBarEx()
{
  // install the keyboard hook
  InstallKeyboardHook();
  
  // increase toolbar created counter
  m_nToolBars++;

  // initialize hwndOwner for GetOwner() and SetOwner() support in MFC < 2.5
#if (_MFC_VER < 0x0250)
  hwndOwner = NULL;
#endif

}  

CToolBarEx::~CToolBarEx()
{
  // decrease the toolbar created counter
  m_nToolBars--;

  // remove the keyboard hook if we're destoying the last toolbar
  if(!m_nToolBars)
  {
    RemoveKeyboardHook();

    // If we're destroying the last toolbar, also destroy 
    // the CBubble's window
    // (We do this here because we cannot call DestroyWindow()
    //  in a static CWnd's destructor)
    m_Bubble.DestroyWindow();
  }
}

BOOL CToolBarEx::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
  // create the toolbar 
  
  // create the base class toolbar first
  if(!CToolBar::Create(pParentWnd, dwStyle, nID))
    return 0;

  // create the bubble window (but leave it invisible)
  // (but only if we don't already have one)
  // NOTE: One CBubble is created for all CToolBarEx's
  if(m_Bubble.IsCreated() == FALSE)  
  {
    // create bubble
    if(!m_Bubble.Create())
      return 0;
      
    // set the initial owner
    m_Bubble.SetOwner(pParentWnd);
  }
  
  return TRUE;
}
    
void CToolBarEx::TrackBubble(BOOL bTrackingButton /* = TRUE */ )
{
  // NOTE: default parameter for nID is -1.  This signals that we're
  //       tracking a CToolBarEx button.  Otherwise, nID is the child
  //       window ID we're tracking.

  // track the bubble window
  
  // get the string resource ID and calc the new window position to track the cursor
  UINT nID;
  CPoint point;
  
  // Are we tracking a button?
  if(bTrackingButton)
  {
    // yes, calc the position based on the current screen cursor position
    ::GetCursorPos(&point);  // in screen coorinates
    
    // move half the cursor size down
    point.y += GetSystemMetrics(SM_CYCURSOR)/2;
    
    // use the m_nButtonIndex to get the ID for a CToolBar button
    ASSERT(m_nButtonIndex != -1);
    nID = GetItemID(m_nButtonIndex);
  }
  
  else  // we're tracking a child window
  {
    // calc the position based on the location of the child window we're tracking
    CWnd* pcwnd;
    CRect rect;
    
    // get the window we're tracking
    pcwnd = GetDescendantWindow(m_nChildID);
    ASSERT(pcwnd != NULL);
    
    // get the child's window rect (in screen coordinates)
    pcwnd->GetWindowRect(&rect);
    
    // set the bubble position based on the child window position
    point.y = rect.bottom;
    // use a -negative value to signal to center the bubble
    // (we cannot calculate the center point here because we don't know
    //  the size of the bubble window until after the new text has been set)
    point.x = -(rect.left+(rect.Width()/2));  

    // set nID to the child ID
    nID = m_nChildID;
  }
  
  // get the message to send

  // load the resource string and set the bubble window text
  CString  str;
  str.LoadString(nID);
  
   if (str.IsEmpty())
   {
      CancelBubble();
      return;
   }

  // if the string has a newline delimiter in it use that part of the string,
  // otherwise use the full string.
  int nNewLine = str.ReverseFind('\n');
  
  // do we have a delimiter?
  if(nNewLine != -1)
  {
    str = str.Mid(nNewLine+1); // use just the delimited part
  }
  else
  {
    // output a trace error for the missing bubble help text
    TRACE1("Warning: no bubble help text for ID 0x%04X\n", nID);
  }

  // set the bubble postion and text and track it
  m_Bubble.Track(point, str, nID);
}  

void CToolBarEx::CancelBubble()
{
  // cancel and hide the bubble window
  
  // release our capture
  if(m_bCaptured)  // do we really have the capture?
    ::ReleaseCapture();
  
  // flag as released
  m_bCaptured = FALSE;
  
  // clear the button index
  m_nButtonIndex = -1;
  
  // clear the child ID
  m_nChildID = -1;
  
  // turn the bubble window OFF
  m_Bubble.Off();
}

BOOL CToolBarEx::IsTransferable(CWnd* pcwndOver)
{
  // Determine if we can tranfer the bubble window.
  // Return TRUE if we can, otherwise FALSE.
  
  // Is the window we're over another CToolBarEx (or a derived class)?
  if(pcwndOver->IsKindOf(RUNTIME_CLASS(CToolBarEx)))
    return TRUE;
  
  //  Is it one of our children?  
  if(IsChild(pcwndOver))
    return TRUE;
  
  // Is it a child of another CToolBarEx (or a derived class)?
  for(CWnd* pcwndTemp = pcwndOver->GetParent(); 
      pcwndTemp != NULL; pcwndTemp = pcwndTemp->GetParent())
  {
    // is it's parent a CToolbarEx (or a derived class)?
    if(pcwndTemp->IsKindOf(RUNTIME_CLASS(CToolBarEx)))
      return TRUE;
  }
  
  return FALSE;
}

void CToolBarEx::TransferBubble(BOOL bMode /*=TRUE*/)
{
  // pass the bubble window control to another window without turing it off
                
    // release our hold 
    ::ReleaseCapture();
    
    // clear captured flag
    m_bCaptured = FALSE;

  // if mode is TRUE, prepare to change what the bubble is tracking
  if(bMode)
  {
    // clear the button index
    m_nButtonIndex = -1;
                
    // clear the child ID
    m_nChildID = -1;
                    
    // hide the bubble
    m_Bubble.Hide();
  }
}  
  
CWnd* CToolBarEx::GetFirstDescendantWindow(CWnd* pcwndChild)
{
  // Given a child window, find the parent of the child which
  // is a direct descendant (the first child) of the CToolBarEx.
  
  CWnd* pcwndFirstDescendant;  // place to hold the first descendant
  CWnd* pcwndDescendant = pcwndChild;  // place to hold the window we're testing
  
  // walk down the parent chain until we reach ourself
  while(pcwndDescendant != this)
  {
    // save this one as a possible first descendant
    pcwndFirstDescendant = pcwndDescendant;
           
    //  descend to the next parent
    pcwndDescendant = pcwndDescendant->GetParent();
    
    // if we reach NULL, the argument was not a child of ours
    ASSERT(pcwndDescendant != NULL);
  }
  
  // pcwndFirstDescendant is the one we want
  return pcwndFirstDescendant;
}  

void CToolBarEx::InstallKeyboardHook()
{
  // install a keyboard hook so we can disable the 
  // bubble when we see a keypress.
  
  // NOTE: This function will not work in a DLL!
  
  // have we already installed the hook?
  if(m_hHook == NULL)
  {
    // no, install it
    if (!m_bWin31)
    {
      // Win30 method
      m_hHook = (HHOOK)::SetWindowsHook(WH_KEYBOARD,
        (HOOKPROC)KeyboardHookCallback);
    }
    else
    {
    #ifndef _WIN32
      // Win31 method
      m_hHook = ::SetWindowsHookEx(WH_KEYBOARD,
        (HOOKPROC)KeyboardHookCallback, AfxGetInstanceHandle(),
        ::GetCurrentTask());
    #else
      // Win32 method
      m_hHook = ::SetWindowsHookEx(WH_KEYBOARD,
        (HOOKPROC)KeyboardHookCallback, NULL,
        GetCurrentThreadId());
    #endif
    }
  }
}

void CToolBarEx::RemoveKeyboardHook()
{
  // remove our keyboard hook
  
  // is it really installed?
  if(m_hHook != NULL)
  {
    if (!m_bWin31)
      // win30
      ::UnhookWindowsHook(WH_KEYBOARD, (HOOKPROC)KeyboardHookCallback);
    else
      // win31 & NT
      ::UnhookWindowsHookEx(m_hHook);
      
    // clear hook handle
    m_hHook = NULL;
  }
}

LRESULT CALLBACK AFX_EXPORT CToolBarEx::KeyboardHookCallback(int code, WPARAM wParam, LPARAM lParam)
{
  // static member function for the keybaord hook callback
  
  // process the hook code
  if(code >= 0)
    // if we're currently capturing the mouse, 
    // release it when we get a keypress
    if(m_bCaptured)
      CancelBubble();
  
  // let the system always process all hooks
  if (!m_bWin31)
  {
#ifdef STRICT
    return ::DefHookProc(code, wParam, lParam, &m_hHook);
#else
    return ::DefHookProc(code, wParam, lParam,
      (HOOKPROC FAR*)&m_nHook);
#endif
  }
  else
  {
    return ::CallNextHookEx(m_hHook, code, wParam, lParam);
  }
}

BOOL CToolBarEx::PreTranslateMessage(MSG* msg)
{
  // handle a few messages for our children
  
  // get a temp CWnd object for the message we're processing
  CWnd* pcwndMsg = FromHandle(msg->hwnd);
  
  // is the message for a child or ourself?
  if(IsChild(pcwndMsg))
  {
    // yes, it's a child of ours
    
    // is it a message we want to process?
    // (NOTE: we have to do old style mesage handling here)
    switch(msg->message)
    {
      case WM_LBUTTONDOWN:
      if(m_bCaptured)
      {
         CancelBubble();
         // Send a notification to parent if capture was on, as this prevents it
         NotifyMainWindow ();
      } 
      break;
      
      case WM_RBUTTONDOWN:
      case WM_CANCELMODE:
        // is the bubble currently on?
        if(m_bCaptured)
          CancelBubble();
        break;
        
    
      case WM_MOUSEMOVE:
        // is our application the active application?
        // (is this CToolBarEx a child of the active application)
        CWnd* pActiveWnd = GetActiveWindow();
        if(pActiveWnd            // also check for NULL for Win32
            && pActiveWnd->IsChild(this)
            && !(msg->wParam & (MK_LBUTTON|MK_RBUTTON)))
        {
          // yes we're the active application
          
          // get the first descendant from the CToolBarEx
          // (so we can treat children that have children as one control
          //  i.e. like a Combo box control)

          CWnd* pcwndFirstDescendant = GetFirstDescendantWindow(pcwndMsg);
          
          // is the window with the focus a child?
          CWnd* pFocus = GetFocus();
          if(pFocus == NULL || !IsChild(pFocus)) // also check for NULL for Win32
          {
            // none of our children has the focus
          
            // do WE have the capture already?
            if(m_bCaptured)
            {
              // yes, but what are we over now?
              
              // find the window we're over
              CPoint point(msg->pt);
              CWnd* pcwndOver = WindowFromPoint(point);
                    
              // are we over ourself or another window?
              if(pcwndMsg != pcwndOver)
              {
                // we're over another window now
                      
                // Is the window we're over one we can tranfer the bubble to?
                if(IsTransferable(pcwndOver))
                {
                  // yes we're over another CToolBarEX (or a derived class)
                  // or another child of ours or a child of another CToolBarEx.
                  
                  // but is the window we're over a child of the FirstDescendant?
                  // or are we now over the actual FirstDescendant window itself?
                  if(pcwndFirstDescendant->IsChild(pcwndOver) || pcwndFirstDescendant == pcwndOver)
                  {
                    // yes, either we're over a another child of the FirstDescendant
                    // or the FirstDescendant window itself.
                    
                    // Do a special pass of the bubble to the window we're over
                    // without hiding the bubble window and without changing the ChildID
                    // during the pass of the bubble.
                    // (since the control we're tracking really isn't changing, 
                    //  it's postion and caption will not change either)
                    
                    TransferBubble(FALSE);  // FALSE parameter signals the special pass
                    
                    // NOTE: We're still tracking the same control at this point.
                    //       All that has changed is the owner of the mouse capture.
                  }
                  
                  else
                  {
                    // no, we really are over another CToolBarEX (or a derived class)
                    // or another child of ours.
                    
                    // Pass the bubble window to him, leaving it's status as ON
                    TransferBubble();
                    
                    // NOTE: The window we're over will now receive a mouse move.
                    //       It will then capture the mouse, and resume tracking the bubble.
                  }
                }
                      
                else  // we're not over ourself or another CToolBarEx bar anymore
                {
                  // but are we over the bubble window?
                  if(pcwndOver->IsKindOf(RUNTIME_CLASS(CBubble)))
                  {
                    // yes we're over the bubble window, just hide it,
                    // but don't turn it off
                    m_Bubble.Hide();
                    
                    // clear the index and ID so the bubble always gets re-shown on the next mouse move
                    m_nButtonIndex = -1;
                    m_nChildID = -1;
                  }
                        
                  else  // we're not over the bubble window or another CToolBarEx
                  {
                    // We don't know what we're over.
                    // Most likely we left the toolbar area
                    // so cancel the bubble window now
                    CancelBubble();
                  }
                }
              }
            }
                  
            else  // WE currently don't have the capture
            {
              // It must be the first time we're over the window

              // capture the mouse
              
              // use the real child window we're over, 
              // not the tracking FirstDescendant
              // so it can manage the cursor
              pcwndMsg->SetCapture();
                      
              /// flag as WE took the capture
              m_bCaptured = TRUE;
              
              // is the ChildID isn't currently set, get it and track the bubble.
              // (NOTE: It will be already set when we do a special bubble pass
              //        from sibling to sibling.  See above for more details.)
              if(m_nChildID == -1)
              {
                // save the current child ID of the FirstDescandant
                m_nChildID = pcwndFirstDescendant->GetDlgCtrlID();
                
                // set the CBubble owner to this CToolBarEx's owner
                // so the notification messages get routed to the right
                // frame window.
                m_Bubble.SetOwner(GetOwner());
              
                // track the bubble
                TrackBubble(FALSE);
              }
            }
          }
          
          else  // yes, one of our children has the focus
          {
            // disable the bubble
                
            // (NOTE: this will happen when another CToolbarEx directly passes
            //        the bubble to us.  We must turn it off at this point now
            //        since it will not be when it leaves our window [because we're not
            //        capturing while a child has the focus])
            
            CancelBubble();
          }
        }
        
        else  // no, we're not the active application
        {
          // disable the bubble
            
          // (NOTE: this could happen when something else in the system becomes the
          //        active application by means other that the user.

          CancelBubble();
        }
      break;
    }
  }
  
  // not handled by us, only borrowed
  return 0;
}


BEGIN_MESSAGE_MAP(CToolBarEx, CToolBar)
  //{{AFX_MSG_MAP(CToolBarEx)
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_CANCELMODE()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CToolBarEx message handlers

void CToolBarEx::OnMouseMove(UINT nFlags, CPoint point)
{
  // is our application the active application?
  // (is this CToolBarEx a child of the active application)
  CWnd* pActiveWnd = GetActiveWindow();
  if(pActiveWnd && pActiveWnd->IsChild(this))// also check for NULL for Win32
  {
    // yes we're the active application
    
    // does one of our children have the focus?
    // (NOTE: we disable the bubble as long as 
    //        a child of ours has the focus)
    CWnd* pFocus = GetFocus();
    if(pFocus == NULL || !IsChild(pFocus)) // also check for NULL for Win32
    {
      // none of our children has the focus
      
      // do WE have the capture already?
      if(m_bCaptured)
      {
        // yes, but what are we over now?
      
        // find the window we're over
        ClientToScreen(&point);
        CWnd* pcwndOver = WindowFromPoint(point);
            
        // are we over ourself or another window?
        if(this != pcwndOver)
        {
          // we're over another window now
              
          // Is the window we're over one we can tranfer the bubble to?
          if(IsTransferable(pcwndOver))
          {
            // yes we're over another CToolBarEX (or a derived class)
            // or another child of ours or a child of another CToolBarEx.
            
            // Pass the bubble window to him, leaving it's status as ON
            TransferBubble();
            
            // NOTE: The window we're over will now receive a mouse move.
            //       It will then capture the mouse, and resume tracking the bubble.
          }
              
          else  // we're not over ourself or another CToolBarEx bar anymore or a child of ours
          {
            // but are we over the bubble window?
            if(pcwndOver->IsKindOf(RUNTIME_CLASS(CBubble)))
            {
              // yes we're over the bubble window, just hide it,
              // but don't turn it off
              m_Bubble.Hide();

              // clear the index and ID so the bubble always gets re-shown on the next mouse move
              m_nButtonIndex = -1;
              m_nChildID = -1;
            }
                
            else  // we're not over the bubble window or another CToolBarEx
            {
              // We don't know what we're over.
              // Most likely we left the toolbar area
              // so cancel the bubble window now
              CancelBubble();
            }
          }
        }
            
        else  // the mouse is still over our window
        {
          // what are we over?
          ScreenToClient(&point);  // back to client coordinates first
              
          // get the index for what we're over
          //(either button or separator)
//          int nNewIndex = CToolBar::HitTest(point);  // NOTE: relies on internal CToolBar implementation
          int nNewIndex = HitTest(point);  // NOTE: relies on internal CToolBar implementation
              
          // has the index changed since the last time?
          if(nNewIndex != m_nButtonIndex)
          {
            // yes
                
            // save the new index
            m_nButtonIndex = nNewIndex;
                
            // is it over a button?
            if(m_nButtonIndex != -1)
            {
              // yes, show and track it
                TrackBubble();
            }
                  
            else  // we're not over a button
            {
              // hide the window
              m_Bubble.Hide();
            }
          }
        }
      }
          
      else  // WE currently don't have the capture
      {
        // It must be the first time we're over the window
            
        // but does the base class already have the capture?
        if(GetCapture() != this)
        {
          // no it doesn't, so we can safely capture for ourself
          
          // capture the mouse capture
          SetCapture();
          
          // set the CBubble owner to this CToolBarEx's owner
          // so the notification messages get routed to the right
          // frame window.
          m_Bubble.SetOwner(GetOwner());
              
          // flag as WE took the capture
          m_bCaptured = TRUE;
              
          // check for over a button
//          m_nButtonIndex = CToolBar::HitTest(point);  // NOTE: relies on internal CToolBar implementation
          m_nButtonIndex = HitTest(point);  // NOTE: relies on internal CToolBar implementation
              
          // is it over a valid button?
          if(m_nButtonIndex != -1)
          {
            // yes, show the bubble now
            TrackBubble();
          }
        }
      }
      
    }
    
    else  // yes, one of our children has the focus
    {
      // disable the bubble
          
      // (NOTE: this will happen when another CToolbarEx directly passes
      //        the bubble to us.  We must turn it off at this point now
      //        since it will not be when it leaves our window [because we're not
      //        capturing while a child has the focus])
      
      CancelBubble();
    }
  }
    
  else  // no, we're not the active application
  {
    // disable the bubble
      
    // (NOTE: this could happen when something else in the system becomes the
    //        active application by means other that the user.
    
    CancelBubble();
  }
  
  
  CToolBar::OnMouseMove(nFlags, point);
}

void CToolBarEx::OnLButtonDown(UINT nFlags, CPoint point)
{
  // if WE currently have the capture, release it so the base class (CToolBar)
  // can own the capture

  // is the bubble currently on?
  if(m_bCaptured)
  {
    CancelBubble();
    // Send a notification to parent if capture was on, as this prevents it
    NotifyMainWindow ();
  } 
  
  CToolBar::OnLButtonDown(nFlags, point);
}

void CToolBarEx::OnCancelMode()
{
  // is the bubble currently on?
  if(m_bCaptured)
    CancelBubble();

  CToolBar::OnCancelMode();
}





/////////////////////////////////////////////////////////////////////////////
// CFrameWndEx

#if defined(TBX_FRAME) && (_MFC_VER >= 0x0250)
// only used with MFC 2.5 and greater

IMPLEMENT_DYNCREATE(CFrameWndEx, CFrameWnd)

BEGIN_MESSAGE_MAP(CFrameWndEx, CFrameWnd)
  //{{AFX_MSG_MAP(CFrameWndEx)
  ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CFrameWndEx::CFrameWndEx()
{
}

CFrameWndEx::~CFrameWndEx()
{
}

afx_msg LONG CFrameWndEx::OnSetMessageString(UINT wParam, LONG lParam)
{
  // strip the bubble help text from the resource ID strings
  
  // are we processing a ID request?
  if(wParam)
  {
    // yes
    CString  str;
    
    // load the resource string
    str.LoadString(wParam);
    
    // check for the bubble text
    int nNewLine = str.ReverseFind('\n');
  
    // do we have a delimiter?
    if(nNewLine != -1)
    {
      // strip the newline delimited text from the string
      str = str.Left(nNewLine); // up to the delimited part
      
      // copy to a tempoaray char buffer for CString safety
      char* pstrTmp = new char[str.GetLength()+1];
      strcpy(pstrTmp, str);
      
      // call the base class to handle the modified message
      CFrameWnd::OnSetMessageString(0, (LONG)(char FAR*)pstrTmp);
      
      // free the temporary buffer
      delete pstrTmp;
      
      // return that we processed this message
      return TRUE;
    }
  }

  // call the base class
  return CFrameWnd::OnSetMessageString(wParam, lParam);
}   

#endif  // TBX_FRAME


/////////////////////////////////////////////////////////////////////////////
// CMDIFrameWndEx

#if defined(TBX_MDIFRAME) && (_MFC_VER >= 0x0250)
// only used with MFC 2.5 and greater

IMPLEMENT_DYNCREATE(CMDIFrameWndEx, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMDIFrameWndEx, CMDIFrameWnd)
  //{{AFX_MSG_MAP(CMDIFrameWndEx)
  ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMDIFrameWndEx::CMDIFrameWndEx()
{
}

CMDIFrameWndEx::~CMDIFrameWndEx()
{
}

afx_msg LONG CMDIFrameWndEx::OnSetMessageString(UINT wParam, LONG lParam)
{
  // strip the bubble help text from the resource ID strings
  
  // are we processing a ID request?
  if(wParam)
  {
    // yes
    CString  str;
    
    // load the resource string
    str.LoadString(wParam);
    
    // check for the bubble text
    int nNewLine = str.ReverseFind('\n');
  
    // do we have a delimiter?
    if(nNewLine != -1)
    {
      // strip the newline delimited text from the string
      str = str.Left(nNewLine); // up to the delimited part
      
      // copy to a tempoaray char buffer for CString safety
      char* pstrTmp = new char[str.GetLength()+1];
      strcpy(pstrTmp, str);
      
      // call the base class to handle the modified message
      CMDIFrameWnd::OnSetMessageString(0, (LONG)(char FAR*)pstrTmp);
      
      // free the temporary buffer
      delete pstrTmp;
      
      // return that we processed this message
      return TRUE;
    }
  }

  // call the base class
  return CMDIFrameWnd::OnSetMessageString(wParam, lParam);
}   

#endif  // TBX_MDIFRAME



/////////////////////////////////////////////////////////////////////////////
// COleIPFrameWndEx

#if defined(TBX_OLEIPFRAME) && (_MFC_VER >= 0x0250)
// only used with MFC 2.5 and greater

IMPLEMENT_DYNCREATE(COleIPFrameWndEx, COleIPFrameWnd)

BEGIN_MESSAGE_MAP(COleIPFrameWndEx, COleIPFrameWnd)
  //{{AFX_MSG_MAP(COleIPFrameWndEx)
  ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

COleIPFrameWndEx::COleIPFrameWndEx()
{
}

COleIPFrameWndEx::~COleIPFrameWndEx()
{
}

afx_msg LONG COleIPFrameWndEx::OnSetMessageString(UINT wParam, LONG lParam)
{
  // strip the bubble help text from the resource ID strings
  
  // are we processing a ID request?
  if(wParam)
  {
    // yes
    CString  str;
    
    // load the resource string
    str.LoadString(wParam);
    
    // check for the bubble text
    int nNewLine = str.ReverseFind('\n');
  
    // do we have a delimiter?
    if(nNewLine != -1)
    {
      // strip the newline delimited text from the string
      str = str.Left(nNewLine); // up to the delimited part
      
      // copy to a tempoaray char buffer for CString safety
      char* pstrTmp = new char[str.GetLength()+1];
      strcpy(pstrTmp, str);
      
      // call the base class to handle the modified message
      COleIPFrameWnd::OnSetMessageString(0, (LONG)(char FAR*)pstrTmp);
      
      // free the temporary buffer
      delete pstrTmp;
      
      // return that we processed this message
      return TRUE;
    }
  }

  // call the base class
  return COleIPFrameWnd::OnSetMessageString(wParam, lParam);
}   

#endif  // TBX_OLEIPFRAME && _MFC_VER >= 0x0250


/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx

#if (_MFC_VER < 0x0250)
// only used with MFC 2.0

IMPLEMENT_DYNCREATE(CStatusBarEx, CStatusBar)

BEGIN_MESSAGE_MAP(CStatusBarEx, CStatusBar)
  //{{AFX_MSG_MAP(CStatusBarEx)
  ON_MESSAGE(WM_SETTEXT, OnSetText)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CStatusBarEx::CStatusBarEx()
{
}

CStatusBarEx::~CStatusBarEx()
{
}

/////////////////////////////////////////////////////////////////////////////
// CStatusBarEx message handlers

afx_msg LONG CStatusBarEx::OnSetText(UINT, LONG lParam)
{
  // strip the bubble help text from string

  // make sure lparam is valid
  if(lParam != 0)
  {
    // get a string pointer to work with
    char far* pszText = (char*)lParam;

    // check for the bubble text
    char far* pNewLine = strrchr(pszText, '\n');

    // did we find it?
    if(pNewLine != NULL)
    {
      // yes, so make a copy of the string
      int nSize = (int)(pNewLine-pszText);
      char far * pszNewText = new char[nSize+1];

      // copy it over (but not the newline)
      strncpy(pszNewText, pszText, nSize);

      // null terminate it
      pszNewText[nSize] = NULL;

      // use the modifed text to set the window text
      CStatusBar::OnSetText(0, (LPARAM)pszNewText);

      // delete the modified string
      delete pszNewText;

      // return message status
      return TRUE;
    }
  }

  // do the default processing
  return CStatusBar::OnSetText(0, lParam);
}

#endif // _MFC_VER < 0x0250

#endif

