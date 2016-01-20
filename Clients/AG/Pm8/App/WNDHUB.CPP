/*
// $Workfile: WNDHUB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/WNDHUB.CPP $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 19    11/17/98 6:03p Johno
// Changes for dynamic art store buttons
// 
// 18    7/26/98 2:59p Hforman
// palette flash fixes
// 
// 17    7/24/98 7:45p Johno
// Added OnParentNotify() for noisy buttons!
// 
// 16    7/21/98 12:11p Rlovejoy
// Code to allow runtime type checking.
// 
// 15    6/29/98 11:41a Hforman
// fix problem w/Helpful Hint not going away
// 
// 14    6/25/98 4:09p Johno
// Removed OnKeyUp() - OnCommand handles it.
// 
// 13    6/19/98 12:25p Johno
// Clean up
// 
// 12    6/18/98 4:34p Johno
// New "safe" hub query code
// 
// 11    6/18/98 11:19a Johno
// New "modal only" functionality
// 
// 10    6/17/98 2:54p Johno
// OnDestroy() calls KillHubDLL()
// 
// 9     6/01/98 9:12p Psasse
// an attempt to get helpful hints on top of border editor
// 
// 8     5/31/98 7:27p Psasse
// Workspace Helpful Hint for AddressBook
// 
// 7     5/31/98 5:20p Psasse
// fixed initial daily tip coming in under the hub
// 
// 6     5/26/98 6:38p Psasse
// Helpful hints sound support
// 
// 5     4/18/98 4:25p Fredf
// Uses ReplaceText() to get hub caption.
// 
// 4     4/14/98 12:07p Rlovejoy
// Implemented 'ESC' for cancel.
// 
// 3     4/10/98 4:58p Rlovejoy
// Changes for new hub implementation.
// 
// 2     4/07/98 9:33a Fredf
// Start of new Hub window.
// 
//    Rev 1.1   17 Sep 1997 16:30:40   johno
// See below (from 4.0)
// 
//    Rev 1.32   12 Sep 1997 13:29:24   johno
// Changes for hiding hub buttons
// 
// 
//    Rev 1.31   05 Aug 1997 15:51:54   johno
// Changed code to eliminate negative numbers passed to bitblt.
// This caused a crashing bug in the Direct X 5 S3 driver.
// 
// 
//    Rev 1.30   30 Jul 1997 12:11:08   johno
// Removed included (but not needed) ngsres.h
// 
//    Rev 1.29   24 Jun 1997 11:07:12   johno
// Now use GetScreenIndexByRect(CRect &Rect);
// in CHubResourceManager from GetBitmapIndexBySize(void)
// 
// 
// 
//    Rev 1.28   23 Jun 1997 17:30:50   johno
// Moved hub tables to resource DLLs
// 
//    Rev 1.27   20 Jun 1997 13:29:08   johno
// Removed unused #ifdef stuff
// 
//    Rev 1.26   19 Jun 1997 18:07:48   johno
// Remarked out ngi 2 stuff
// 
//    Rev 1.25   16 Jun 1997 17:01:22   johno
// Changed name of ngi dll #2
// 
//    Rev 1.24   09 Jun 1997 10:42:32   johno
// For new art
// 
//    Rev 1.23   05 Jun 1997 18:08:24   johno
// More multiple hub stuff
// 
//    Rev 1.22   05 Jun 1997 14:35:08   johno
// Changes for multiple hub support
// 
// 
//    Rev 1.21   04 Jun 1997 17:28:34   johno
// Support for multiple hubs
// 
// 
//    Rev 1.20   01 Jun 1997 11:54:04   johno
// Some simplifications
// Renamed SizeToRectangle to SizeToParent
// 
//    Rev 1.19   30 May 1997 16:24:18   johno
// New hub button positions
// 
//    Rev 1.18   29 May 1997 12:55:52   johno
// Free fonts registration
// 
// 
//    Rev 1.17   28 May 1997 18:18:32   johno
// 
//    Rev 1.16   28 May 1997 18:09:48   johno
// Font registration button
// 
//    Rev 1.15   27 May 1997 17:08:48   johno
// Activated multiple sizes code
// 
// 
//    Rev 1.14   23 May 1997 15:59:56   johno
// Creation with ID
// (needed for sound effects)
// 
// 
//    Rev 1.13   23 May 1997 10:21:30   johno
// Added OnHelpHitTest; used with OnParentNotify
// in mainfrm for identifying pressed button for sound fx
// 
// 
//    Rev 1.12   04 Apr 1997 09:59:28   Jay
// Use PMG palette when making CDDBs.
// 
//    Rev 1.11   03 Apr 1997 17:09:38   johno
// Compile option for use of global Printmaster palette
// 
// 
//    Rev 1.10   03 Apr 1997 15:39:26   johno
// Uses colordef.h
// 
//    Rev 1.9   31 Mar 1997 17:53:28   johno
// Brand new button does ID_FILE_NEW stuff
// 
//    Rev 1.8   28 Mar 1997 14:36:00   johno
// Realise palette for OnEraseBkgnd
// 
//    Rev 1.7   18 Mar 1997 16:34:56   johno
// Removed focus button bitmap
// 
//    Rev 1.6   17 Mar 1997 15:14:58   johno
// Made GetMinSize static so it can be called
// with an uncreated object.
// GetSmallestWindowSizeAndIndex is no longer in line.
// 
//    Rev 1.5   14 Mar 1997 16:19:00   johno
// Changes for single HUB window size
// 
//    Rev 1.4   11 Mar 1997 16:22:26   johno
// 
// 
//    Rev 1.3   07 Mar 1997 17:36:08   johno
// Tweaks for HUB in PM
// 
// 
//    Rev 1.2   06 Mar 1997 17:05:18   johno
// Tweak
// 
//    Rev 1.0   28 Feb 1997 11:37:54   johno
// Initial revision.
*/

// WndHub.cpp : implementation file
// Handles specifics for thr hub screen

#include "stdafx.h"
#include "resource.h"
#include "WndHub.h"
#include "pmw.h"
#include "pmwcfg.h"
#include "hint.h"

#define  JTRACE_ON
#include "jtrace.h"
#include "colordef.h"

#include <afxpriv.h>    // for WM_HELPHITTEST
#include "pmwdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////
extern CPalette *pOurPal;     // THE PrintMaster Palette(from PMWVIEW.CPP)

#define  PALETTE_VALID  ((pOurPal != NULL) && (pOurPal->m_hObject != NULL))
#define  PALETTE_NAME   pOurPal


BOOL						CWndHub::m_fDailyTipShown = FALSE;
CHubResourceManager	CWndHub::m_HubResourceManager;

/////////////////////////////////////////////////////////////////////////////
BitmapButtonWithPosition::BitmapButtonWithPosition(ButtonSet *p)
   : CPmwBitmapButton()

{
   Origin.x = p->Origin.x;
   Origin.y = p->Origin.y;
}
/////////////////////////////////////////////////////////////////////////////
// CWndHub
IMPLEMENT_DYNAMIC(CWndHub, CWnd)

CWndHub::CWndHub(LPCSTR DLLName)
{
   m_pBitmap = NULL;

   m_CurBitmapIndex = -1;
   m_pLastFocusButton = NULL;
   m_fInModalLoop = FALSE;
	m_HubResourceManager.SetDLLName(DLLName);
}

CWndHub::~CWndHub()
{
   if (IsWindow(m_hWnd) != FALSE)
      DestroyWindow();
}


BEGIN_MESSAGE_MAP(CWndHub, CWnd)
   //{{AFX_MSG_MAP(CWndHub)
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_DESTROY()
   ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEACTIVATE()
	ON_WM_KEYUP()
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	//}}AFX_MSG_MAP
   ON_WM_PARENTNOTIFY()
   ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

int
CWndHub::GetBitmapIndexBySize(void)const
{
   UINT  i;
   CRect ClientRect;

   GetClientRect(&ClientRect);

   i = m_HubResourceManager.GetScreenIndexByRect(ClientRect);

JTRACE("Selected bitmap %d\n", i);

   return i;
}

BOOL
CWndHub::MoveButton(BitmapButtonWithPosition  *pButton)
{
   int               dx, dy;
   CRect             RectButton;

   // Get button rectangle
   pButton->GetClientRect(&RectButton);
   // Move to position in bitmap
   RectButton.OffsetRect(pButton->Origin);
   // Move to position on screen
   GetBitmapOffset(&dx, &dy);
   RectButton.OffsetRect(dx, dy);
   
   pButton->MoveWindow(&RectButton);

   return TRUE;
}

void
CWndHub::DoButtons(void)
{
   HINSTANCE         OldResourceHandle;
   UINT              i, id, idUp, idDown;
   CRect             RectButton(0, 0, 16, 16);
   BitmapButtonWithPosition  *pButton;
	struct ScreenInfo	*pScreenInfo;
	
JTRACE("Creating buttons\n");

   KillButtons();
   
	if ((pScreenInfo = m_HubResourceManager.GetTable(IDR_HUBTABLE)) == NULL)
	{
JTRACE("NULL ScreenInfo\n");		
		return;
	}

   if (m_CurBitmapIndex >= 0)
   {
      OldResourceHandle = AfxGetResourceHandle();
      AfxSetResourceHandle(GetResourceHandle());

      for (i = 0; i < pScreenInfo->nNumButton; i ++)
      {
         id     = pScreenInfo->pScreen [m_CurBitmapIndex].pButton [i].idButton;
         idUp   = pScreenInfo->pScreen [m_CurBitmapIndex].pButton [i].idBitmapUp;
         idDown = pScreenInfo->pScreen [m_CurBitmapIndex].pButton [i].idBitmapDn;
      
         if (IsSkippedButton(id) == FALSE)
         {
            if ((pButton = new BitmapButtonWithPosition(&pScreenInfo->pScreen [m_CurBitmapIndex].pButton [i])) != NULL)
            {
               if (
                     (pButton->Create(NULL, BS_OWNERDRAW | BS_PUSHBUTTON | WS_TABSTOP, RectButton, this, id) != FALSE)
                     &&
                     (pButton->LoadBitmaps(MAKEINTRESOURCE(idUp), MAKEINTRESOURCE(idDown)) != FALSE)
                  )
               {
                  BitmapButtons.Add(pButton);
                  pButton->SizeToContent();
                  // Get the area of the bitmap covered by the button
                  pButton->GetClientRect(&RectButton);
                  RectButton.OffsetRect(pScreenInfo->pScreen [m_CurBitmapIndex].pButton [i].Origin.x,
                                          pScreenInfo->pScreen [m_CurBitmapIndex].pButton [i].Origin.y);
                  // Make sure we have a background bitmap
                  if (m_pBitmap != NULL)
                  {
                     pButton->ConvertToTransparent(m_pBitmap, RectButton);
                  }

                  MoveButton (pButton);
                  pButton->ShowWindow(SW_SHOW);
               }
               else
               {
   JTRACE1("Failed to make button %d\n", i);            
                  KillButton(pButton);
               }
            }
         }
      }

      AfxSetResourceHandle(OldResourceHandle);
   }
}

BOOL
CWndHub::GetBitmap(void)
{
   BOOL  GotNewBitmap = FALSE;
   int   idx;
	struct ScreenInfo	*pScreenInfo;

	if ((pScreenInfo = m_HubResourceManager.GetTable(IDR_HUBTABLE)) == NULL)
	{
JTRACE("NULL ScreenInfo\n");		
		return GotNewBitmap;
	}

   if ((idx = GetBitmapIndexBySize()) >= 0)
   {
      if ((m_pBitmap == NULL) || (idx != m_CurBitmapIndex)) // No bitmap or need new bitmap
      {
         KillBitmap();
         KillButtons();

         CDC*         pDC;
         HINSTANCE   hi = GetResourceHandle();

         if ((pDC = GetDC()) != NULL)
         {
            CPalette* pOldPalette = pDC->SelectPalette(pOurPal, FALSE);
            pDC->RealizePalette();

            CDDB  *pDDB = new CDDB(hi, MAKEINTRESOURCE(pScreenInfo->pScreen [idx].idScreen), RT_BITMAP, pDC->GetSafeHdc(), FALSE);
            if (pDDB->IsValid() == TRUE)
            {
               if ((m_pBitmap = new CBitmap) != NULL)
               {
                  HBITMAP     hBitmap;
      
                  hBitmap = pDDB->GetBitmap();

                  pDDB->Detach(TRUE);

                  if (m_pBitmap->Attach(hBitmap) != 0)
                  {
                     m_CurBitmapIndex = idx;
JTRACE("Loaded bitmap\n");
                     //DoButtons();
                     GotNewBitmap = TRUE;
                  }
                  else
                     KillBitmap();
               }
            }
            
            delete pDDB;

            if (pOldPalette != NULL)
            {
               pDC->SelectPalette(pOldPalette, FALSE);
            }

            ReleaseDC(pDC);
         }
      }
   
      if (GotButton() == FALSE)
         DoButtons();
   
   }

   return GotNewBitmap;
}

/////////////////////////////////////////////////////////////////////////////
// CWndHub message handlers

LRESULT CWndHub::OnHelpHitTest(WPARAM, LPARAM lParam)
{
   return standard_dialog_hit_test(this, 0, lParam);
   //return -1;
}

void CWndHub::OnParentNotify(UINT message, LPARAM lParam)
{
	if (WM_LBUTTONDOWN == LOWORD (message))
	{
		if (GET_PMWAPP()->m_fSoundEffectsState == FALSE)
			return;

		CPoint   Point (lParam);
		CWnd* pChildWnd = ChildWindowFromPoint(Point, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED);
		HWND hWnd = pChildWnd->GetSafeHwnd();
		if (hWnd != NULL && hWnd != m_hWnd)
		{
			UINT  IDButton = ::GetDlgCtrlID(hWnd);
      
			if ((IDButton != 0) && (IDButton != (UINT)(SHORT)-1))
			{
				if (IDButton != ID_HUB_WINDOW)	// I probably don't need to do this...
				{
					GET_PMWAPP()->m_MciWnd.PlaySoundFX (IDButton, ID_HUB_WINDOW);
				}
			}
		}
	}
} 

BOOL 
CWndHub::Create(CWnd *pParent) 
{
   if (m_HubResourceManager.GetDLLHandle() == NULL)
      return FALSE;
   
	struct ScreenInfo	*pScreenInfo;
   pScreenInfo = m_HubResourceManager.GetTable(IDR_HUBTABLE);

   ASSERT(pScreenInfo != NULL);

   if (pScreenInfo == NULL)
      return FALSE;

	CRect crRect(0, 0, pScreenInfo->pScreen[0].Width, pScreenInfo->pScreen[0].Height);
	
	CRect crParent;
	pParent->GetWindowRect(crParent);
	crRect.OffsetRect(
		(crParent.left+crParent.right-crRect.Width())/2,
		(crParent.top+crParent.bottom-crRect.Height())/2);

	// Adjust for caption
	DWORD dwStyle = WS_VISIBLE | WS_POPUP | WS_CAPTION;
	::AdjustWindowRect(&crRect, dwStyle, FALSE);

	CString csCaption;
	csCaption.LoadString(IDS_HUB_CAPTION);
   GetConfiguration()->ReplaceText(csCaption);

	return CWnd::CreateEx(
		NULL,
		AfxRegisterWndClass(0),
		csCaption,
		dwStyle,
		crRect.left,
		crRect.top,
		crRect.Width(),
		crRect.Height(),
		pParent->GetSafeHwnd(),
		NULL,
		NULL);
}

void CWndHub::OnPaint() 
{
   int      dx, dy;
   int      nDestDX, nDestDY;
   int      nSourceDX, nSourceDY;
   CRect    rect;
   CPalette *pPaletteOld = NULL;
   CPaintDC dc(this); // device context for painting
   CDC      SourceDC;
   CBitmap  *pOldBitmap;
   
   // Do not call CWnd::OnPaint() for painting messages

JTRACE("CWndHub::OnPaint \n");
   // Will select, and load bitmap, if required
   GetBitmap();
   if (m_pBitmap != NULL)
   {
      GetBitmapOffset(&dx, &dy);
      dc.GetClipBox(&rect);

      if (PALETTE_VALID)
      {
         pPaletteOld = dc.SelectPalette(PALETTE_NAME, FALSE);
         dc.RealizePalette();
      }

      if ((SourceDC.CreateCompatibleDC(&dc)) == TRUE)
      {
         pOldBitmap = SourceDC.SelectObject(m_pBitmap);

         nDestDX = rect.left;
         nDestDY = rect.top;
         nSourceDX = rect.left - dx;
         nSourceDY = rect.top - dy;

         if (nSourceDX < 0)
         {
            nDestDX -= nSourceDX;
            nSourceDX = 0;
         }

         if (nSourceDY < 0)
         {             
            nDestDY -= nSourceDY;
            nSourceDY = 0;
         }

dc.BitBlt(nDestDX, nDestDY, rect.Width(), rect.Height(), &SourceDC, nSourceDX, nSourceDY, SRCCOPY);
//dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &SourceDC, rect.left - dx, rect.top - dy, WHITENESS);
         //dc.BitBlt(RectClient.left, RectClient.top, RectClient.Width(), RectClient.Height(), &SourceDC, RectClient.left, RectClient.top, SRCCOPY);
         
         SourceDC.SelectObject(pOldBitmap);
      }

      if (pPaletteOld != NULL)
      {
         dc.SelectPalette(pPaletteOld, FALSE);
      }
   }

	if (!m_fDailyTipShown)
	{
		GET_PMWAPP()->ShowHelpfulHint(PLACE_HINT, PLACE_Hub);
		m_fDailyTipShown = TRUE;
	}
}
// Send all commands to mom
BOOL
CWndHub::OnCommand(WPARAM wParam, LPARAM lParam)
{
   CWnd  *pWnd = GetParent();
	// We want to work only modal
   if (!m_fInModalLoop)
	{
		ASSERT(FALSE);
		return PostMessage(WM_CLOSE);	// Abort, something wrong
	}
   if (pWnd == NULL)
	{
		ASSERT(FALSE);
		EndModalLoop(0);
		return TRUE;
	}
	// "Ask" mom what to do
	CHubQueryInfo HubQueryInfo(wParam);
	pWnd->SendMessage(WM_HUB_QUERY_ACTION, 0, (LPARAM)&HubQueryInfo);
		
	if (HubQueryInfo.RetVal() != 0)
		EndModalLoop(HubQueryInfo.RetVal());	// I was told to return this to caller
	else
		pWnd->PostMessage(WM_COMMAND, wParam, lParam);
	
	return TRUE;
}

BOOL CWndHub::PreTranslateMessage(MSG* pMsg) 
{
	if (IsDialogMessage(pMsg))
		return TRUE;
	else
		return CWnd::PreTranslateMessage(pMsg);
}
/*
void CWndHub::DoActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CWnd  *pWnd;

   if (nState == WA_INACTIVE)
   {
      if ((pWnd = GetFocus()) != NULL)
      {
         if (pWnd->GetParent() == this)        // If one of my guys is losing focus...
            m_pLastFocusButton = pWnd->m_hWnd;  // Save the button with the focus
         else
            m_pLastFocusButton = NULL;
      }
   }
   else
   if (m_pLastFocusButton != NULL)              // If we had a button with focus, restore it
   {
      ::SetFocus(m_pLastFocusButton);
      m_pLastFocusButton = NULL;
   }   
}

void
CWndHub::DoPaletteChanged (CWnd* pFocusWnd)
{
   if (pFocusWnd != this)
   {
      DoQueryNewPalette();
   }
}
*/

BOOL CWndHub::DoQueryNewPalette()
{
   int i = 0;
   CDC *pDC;

   if (PALETTE_VALID)
   {
      if ((pDC = GetDC()) != NULL)
      {
         CPalette *pOldPal = pDC->SelectPalette(PALETTE_NAME, FALSE);
         // Find out how many colors changed
         if (pOldPal != NULL)
         {
            i = pDC->RealizePalette();
   
            pDC->SelectPalette(pOldPal, FALSE);
         }

         ReleaseDC(pDC);
      }
      
      if (i > 0)
      {
         RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
         AfxGetMainWnd()->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
      }
   }

   return i;
}

void CWndHub::OnSize(UINT nType, int cx, int cy) 
{
JTRACE2("OnSize: %d x, %d y\n", cx, cy); 

   BOOL  b;
   
   CWnd::OnSize(nType, cx, cy);
   
   b = GetBitmap();
   // If a new bitmap was NOT chosen, pusition the buttons
   // If a new bitmap WAS chosen, new buttons were created and positioned
   if (b == FALSE)
      MoveButtons();
}

void CWndHub::OnDestroy() 
{	
	CWnd::OnDestroy();
	
   KillButtons();
   KillBitmap();
	m_HubResourceManager.KillHubDLL();
}

int
CWndHub::StartModalLoop()
{
	// Disable parent, re-enable child
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		pParent->EnableWindow(FALSE);
	}
	EnableWindow(TRUE);

	// Run the loop...
	SetFocus();
	m_fInModalLoop = TRUE;
	return RunModalLoop(MLF_SHOWONIDLE);
}

void CWndHub::EndModalLoop(int result)
{
   // Terminate the modal loop
   if (m_fInModalLoop)
   {		
		// Enable the parent window
		CWnd* pParent = GetParent();
		if (pParent != NULL)
		{
			pParent->EnableWindow(TRUE);

			// Bring parent to the top
			pParent->SetForegroundWindow();
		}

		// End the modal loop
		m_fInModalLoop = FALSE;
		CWnd::EndModalLoop(result);
   }
}

BOOL CWndHub::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CWndHub::OnClose() 
{
	EndModalLoop(IDCANCEL);
	
	CWnd::OnClose();
}

// Ok, I don't know why the helpful hint isn't getting the capture,
// but meanwhile, I'm going to make sure that darned thing goes away! (hal)
void CWndHub::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GET_PMWAPP()->RemoveHelpfulHint();	
	CWnd::OnLButtonDown(nFlags, point);
}
void CWndHub::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GET_PMWAPP()->RemoveHelpfulHint();	
	CWnd::OnRButtonDown(nFlags, point);
}
void CWndHub::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	GET_PMWAPP()->RemoveHelpfulHint();	
	CWnd::OnNcLButtonDown(nHitTest, point);
}
int CWndHub::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	GET_PMWAPP()->RemoveHelpfulHint();	
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CWndHub::OnPaletteChanged(CWnd* pFocusWnd) 
{
	CWnd::OnPaletteChanged(pFocusWnd);
	
   if (pFocusWnd != this)
   {
      DoQueryNewPalette();
   }
}

BOOL CWndHub::OnQueryNewPalette() 
{
	DoQueryNewPalette();
	return CWnd::OnQueryNewPalette();
}

BOOL
CWndHub::SubstituteBitmap (UINT BitmapIDOld, UINT BitmapIDNew)
{
	struct ScreenInfo *pScreenInf = m_HubResourceManager.GetTable(IDR_HUBTABLE);
	// Make sure we are initialized
	if (pScreenInf != NULL)
	{
      HINSTANCE	InstDLL = m_HubResourceManager.GetDLLHandle();
		if (InstDLL != NULL)
		{	// Make sure we actually have this resource
			if (FindResource(InstDLL, MAKEINTRESOURCE(BitmapIDNew), RT_BITMAP) != NULL)
			{	// Search through all buttons 
				for (int s = 0; s < pScreenInf->nNumScreen; s ++)
				{
					for (int i = 0; i < pScreenInf->nNumButton; i ++)
					{
						if (
								(pScreenInf->pScreen[s].pButton[i].idBitmapUp == BitmapIDOld)
								||
								(pScreenInf->pScreen[s].pButton[i].idBitmapDn == BitmapIDOld)
							)
						{	// Replace bitmap
							if (pScreenInf->pScreen[s].pButton[i].idBitmapUp == BitmapIDOld)
								pScreenInf->pScreen[s].pButton[i].idBitmapUp = BitmapIDNew;
							else
								pScreenInf->pScreen[s].pButton[i].idBitmapDn = BitmapIDNew;
JTRACE("Replacing bitmap with %d\n", BitmapIDNew);
							DoButtons();

							return TRUE;
						}
					}
				}
			}
		}
	}

	return FALSE;
}

