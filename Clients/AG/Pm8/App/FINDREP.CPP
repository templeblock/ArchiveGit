// CFindReplaceDialogModal.cpp : implementation file
//

/*
// $Workfile: FINDREP.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FINDREP.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:26   Fred
// Initial revision.
// 
//    Rev 1.0   03 Feb 1997 17:28:00   johno
// Initial revision.
// 
//    Rev 1.0   02 Feb 1997 13:46:00   JRO
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "findrep.h"

#ifndef WIN32
#include <auxdata.h>       // for _AfxGetSafeOwner()
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// implementation helpers

static BOOL PASCAL WalkPreTranslateTree(HWND hWndStop, MSG* pMsg)
{
   ASSERT(hWndStop == NULL || ::IsWindow(hWndStop));
   ASSERT(pMsg != NULL);

   // walk from the target window up to the hWndStop window checking
   //  if any window wants to translate this message

   for (HWND hWnd = pMsg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd))
   {
      CWnd* pWnd = CWnd::FromHandlePermanent(hWnd);
      if (pWnd != NULL)
      {
         // target window is a C++ window
         if (pWnd->PreTranslateMessage(pMsg))
            return TRUE; // trapped by target window (eg: accelerators)
      }

      // got to hWndStop window without interest
      if (hWnd == hWndStop)
         break;
   }
   return FALSE;       // no special processing
}

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDialogModal

CFindReplaceDialogModal::CFindReplaceDialogModal (CWnd *pParent)
{
   m_pParentWnd = pParent;
   m_fParentDisabled = FALSE;
}

CFindReplaceDialogModal::~CFindReplaceDialogModal()
{
}


BEGIN_MESSAGE_MAP(CFindReplaceDialogModal, CWnd)
   //{{AFX_MSG_MAP(CFindReplaceDialogModal)
      // NOTE - the ClassWizard will add and remove mapping macros here.
   ON_REGISTERED_MESSAGE (wm_findreplace, InternalFindReplace)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
int
CFindReplaceDialogModal::DoModal (BOOL FindOnly, LPCSTR FindWhat, LPCSTR ReplaceWidth, DWORD flags)
{
#if 0
   int                  m_nID = IDOK;
   CString              strClassName = AfxRegisterWndClass (0);
   CWnd                 *pParentWnd;
   CFindReplaceDialog   *pDialog = new CFindReplaceDialog;

   m_fDone = FALSE;

#ifdef WIN32
      pParentWnd = CWnd::GetSafeOwner(m_pParentWnd);
#else
      pParentWnd = CWnd::FromHandle(_AfxGetSafeOwner(m_pParentWnd));
#endif

   if (CWnd::CreateEx (0, strClassName, "FindReplace message target", WS_POPUP, 0, 0, 16, 16, NULL, NULL) != 0)
   {
      if (pDialog->Create (FindOnly, FindWhat, ReplaceWidth, flags, this) != 0)
      {
         while (m_fDone == FALSE)
         {
            if (!PumpMessage())
            {
               ::PostQuitMessage((int)msg.wParam);
               m_nID = IDCANCEL;
               break;
            }
         }
      }
   }

   return m_nID;
#endif

   // cannot call DoModal on a dialog already constructed as modeless
   ASSERT(m_hWnd == NULL);
   
   int                  nResult = IDABORT;
   CString              strClassName = AfxRegisterWndClass (0);
   CFindReplaceDialog   *pDialog = new CFindReplaceDialog;
   
   m_fDone = FALSE;

   if (m_hWnd == NULL)
   {
      // allow OLE servers to disable themselves
      CWinApp* pApp = AfxGetApp();
      pApp->EnableModeless(FALSE);
   
      // find parent HWND
#ifdef WIN32
      CWnd* pParentWnd = CWnd::GetSafeOwner(m_pParentWnd);
#else
      CWnd* pParentWnd = CWnd::FromHandle(_AfxGetSafeOwner(m_pParentWnd));
#endif
      // create the hidden window, then the dialog, then enter modal loop
      HWND  hParent;
      if (pParentWnd != NULL)
         hParent = pParentWnd->m_hWnd;
      else
         hParent = NULL;    

      if (CWnd::CreateEx (0, strClassName, "FindReplace message target", WS_CHILD, 0, 0, 16, 16, hParent, NULL) != 0)
      {
         if (pDialog->Create (FindOnly, FindWhat, ReplaceWidth, flags, this) != 0)
         {
            // disable parent (should not disable this window)
            m_fParentDisabled = FALSE;
            if (pParentWnd != NULL && pParentWnd->IsWindowEnabled())
            {
               pParentWnd->EnableWindow(FALSE);
               m_fParentDisabled = TRUE;
   #ifdef WIN32
               pDialog->EnableWindow(TRUE);
   #endif
            }
            
            ASSERT (pDialog->IsWindowEnabled());  // should not be disabled to start!
            
            //pDialog->SetActiveWindow();

            // for tracking the idle time state
            BOOL fShown = (GetStyle() & WS_VISIBLE) != 0;
            m_nID = -1;
      
            // acquire and dispatch messages until a WM_QUIT message is received.
            MSG msg;
            while (m_nID == -1 && m_hWnd != NULL && m_fDone == FALSE)
            {
               // phase1: check to see if we can do idle work
               if (!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
               {
                  // send WM_ENTERIDLE since queue is empty
                  if (pParentWnd != NULL &&
                        !(pParentWnd->GetStyle() & DS_NOIDLEMSG))
                  {
   #ifdef WIN32
                     pParentWnd->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)m_hWnd);
   #else
                     pParentWnd->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, MAKELONG(m_hWnd, 0));
   #endif
                  }
      
                  if (!fShown)
                  {
                     // show and activate the window
                     fShown = TRUE;
                     ShowWindow(SW_SHOWNORMAL);
                  }
               }
      
               // phase2: pump messages while available
               do
               {
                  // pump message -- if WM_QUIT assume cancel and repost
                  if (!PumpMessage())
                  {
                     ::PostQuitMessage((int)msg.wParam);
                     m_nID = IDCANCEL;
                     break;
                  }
      
               } while (m_nID == -1 && m_hWnd != NULL &&
                  ::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));
            }
      
            nResult = m_nID;
            if (m_hWnd != NULL)
               EndDialog(nResult);
         }
      }
   
      // allow OLE servers to enable themselves
      pApp->EnableModeless(TRUE);
   }

   return nResult;
}

void CFindReplaceDialogModal::EndDialog (int nEndID)
{
   ASSERT_VALID(this);

   m_nID = nEndID;
   DestroyWindow();
}

BOOL CFindReplaceDialogModal::DestroyWindow()
{
// GET_PMWAPP()->SaveWindowPosition(this, WindowPositionSection());

   // re-enable parent if it was disabled
   CWnd* pParentWnd = m_pParentWnd != NULL ? m_pParentWnd : GetParent();
   if (m_fParentDisabled && pParentWnd != NULL)
      pParentWnd->EnableWindow();

   // transfer the focus to ourselves to give the active control
   //  a chance at WM_KILLFOCUS
   if (::GetActiveWindow() == m_hWnd && ::IsChild(m_hWnd, ::GetFocus()))
   {
      SetFocus();
   }
   // hide this window and move activation to the parent
   SetWindowPos(NULL, 0, 0, 0, 0,
      SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE |
      SWP_NOSIZE | SWP_NOZORDER);

   //pParentWnd = GetParent();
   if (pParentWnd != NULL)
      pParentWnd->SetActiveWindow();

   // finally, destroy this window
   BOOL fResult = CWnd::DestroyWindow();

   return fResult;
}

long 
CFindReplaceDialogModal::OnFindReplace (WPARAM wParam, LPARAM lParam)
{
   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDialogModal message handlers
long CFindReplaceDialogModal::InternalFindReplace (WPARAM wParam, LPARAM lParam)
{
   if (((LPFINDREPLACE)lParam)->Flags & FR_DIALOGTERM)
   {
      m_fDone = TRUE;
   }

   return OnFindReplace (wParam, lParam);
}

BOOL 
CFindReplaceDialogModal::PumpMessage()
{
   ASSERT_VALID(this);

   MSG Msg;
   if (!::GetMessage(&Msg, NULL, NULL, NULL))
      return FALSE;

   //  let's see if the message should be handled at all
   if (CallMsgFilter(&Msg, MSGF_DIALOGBOX))
      return TRUE;
      
   // process this message
   if (!WalkPreTranslateTree(m_hWnd, &Msg))
   {
      ::TranslateMessage(&Msg);
      ::DispatchMessage(&Msg);
   }
   
   return TRUE;
}
