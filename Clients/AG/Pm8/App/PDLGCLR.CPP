/*
// $Workfile: PDLGCLR.CPP $
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/PDLGCLR.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 7     11/16/98 5:29p Cboggio
// Changed some hard-coded strings
// 
// 6     7/26/98 7:09p Hforman
// moved OnQueryNewPalette() and OnPaletteChanged() to baseclass
// 
// 5     6/16/98 5:34p Dennis
// Enhancements to support any number of character frames and AG Specific
// Resource Loading.
// 
// 4     6/16/98 4:05p Dennis
// Character bitmap is now loaded into static member using
// LoadResourceBitmap()
// 
// 3     4/18/98 4:22p Fredf
// Got rid of help wizard button.
// 
// 2     2/20/98 3:25p Fredf
// Does not draw colored border.
// 
//    Rev 1.0   14 Aug 1997 15:23:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:40   Fred
// Initial revision.
// 
//    Rev 1.14   11 Jul 1997 11:10:12   Jay
// Duplicate line caused a selected pen to be deleted.
// 
//    Rev 1.13   29 May 1997 18:00:54   johno
// Sound effects call on WM_PARENTNOTIFY
// 
//    Rev 1.12   05 May 1997 14:23:20   johno
// Added DoModalNCMoveable so dialog can be moved
// by non client area stuff, if desired
// 
//    Rev 1.11   23 Apr 1997 16:25:02   johno
// 
//    Rev 1.10   23 Apr 1997 14:59:08   johno
// 
//    Rev 1.9   21 Apr 1997 16:54:24   johno
// Changes for welcome dialogs
// 
// 
//    Rev 1.8   21 Apr 1997 10:47:10   johno
// 
//    Rev 1.7   17 Apr 1997 15:13:34   johno
// Changes for the lil' PrintMaster guy
// 
// 
//    Rev 1.6   16 Apr 1997 14:21:26   johno
// Use MapDialogRect in CreateWizzardButtons
// to get typical resource button size
// 
// 
//    Rev 1.5   15 Apr 1997 16:52:48   johno
// Enhanced "wizzard" buttons
// 
// 
//    Rev 1.4   10 Apr 1997 15:17:04   johno
// Support for INVALID_DIALOG_COLOR
// 
//    Rev 1.3   04 Apr 1997 17:53:46   johno
// Added FourceBoundsRecalc for address book
// 
//    Rev 1.2   03 Apr 1997 15:52:50   johno
// Equalized color border option.
// 
//    Rev 1.1   03 Apr 1997 15:37:24   johno
// No longer fully colored
// 
//    Rev 1.0   02 Apr 1997 17:02:40   johno
// Initial revision.
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pdlgclr.h"
#include "util.h"
#include "rmagcom.h"    // For resource manager
#include "pmwcfg.h"     // For GetConfiguration

// For WM_HELPHITTEST    
#include <afxpriv.h>
#ifdef _WIN32
#include <..\src\afximpl.h>
#else
#include <auxdata.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define  COLOR_BOX_GAP     4
#define  COLOR_BOX_GAP_MAX 8

#define  GUY_WIDTH   120
#define  GUY_HEIGHT  260
#define  GUY_GAP     2
#define  GUY_NUM     5

/////////////////////////////////////////////////////////////////////////////
// CPmwDialogColor dialog

#ifndef LOCALIZE
const char     *CPmwDialogColor::TextFinish = "Finish";
const char     *CPmwDialogColor::TextNext = "&Next >";
#endif

// LOCALIZE TODO - add this to the resource file
const char     *CPmwDialogColor::TextOK = "OK";

int            CPmwDialogColor::GuyFrame = -1;

CBitmap CPmwDialogColor::sm_bmGuyFrames;
int     CPmwDialogColor::sm_nNumFrames = 0;

CPmwDialogColor::CPmwDialogColor(UINT idd, 
                                 CWnd* pParent,       // =NULL
                                 COLORREF cr,         // INVALID_DIALOG_COLOR 
                                 BOOL UseLittleGuy)   // FALSE
   : CPmwDialog(idd, pParent)
{
   //{{AFX_DATA_INIT(CPmwDialogColor)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   if(UseLittleGuy && sm_bmGuyFrames.GetSafeHandle() == NULL)
      {
         BOOL  bLoaded = FALSE;
         if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
            {
               CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());

               bLoaded = rlProductSpecific.LoadBitmap(sm_bmGuyFrames, CSharedIDResourceManager::riDialogCharacter, pOurPal);
            }
         else
      	   bLoaded = Util::LoadResourceBitmap(sm_bmGuyFrames, MAKEINTRESOURCE(IDB_MASTER), pOurPal);
         ASSERT(bLoaded);
         if(bLoaded)
            {
            	BITMAP   bmInfo;
         		sm_bmGuyFrames.GetBitmap(&bmInfo);
               sm_nNumFrames = bmInfo.bmWidth / GUY_WIDTH;
            }
      }

   MatteColor = cr;

   Brush.CreateSolidBrush (MatteColor);
   
   m_ControlBounds.SetRectEmpty ();
   
   GuyFrame ++;
   if (GuyFrame >= sm_nNumFrames)
      GuyFrame = 0;
   // Make and use a copy so another instance doesn't change it
   m_GuyFrame = GuyFrame;
   m_UseLittleGuy = UseLittleGuy; 
   m_NonClientMoveable = FALSE;
}

void
CPmwDialogColor::EnableWizardButtons (DWORD Flags)
{
   BOOL  Enable;
   
   if (IsWindow (m_Back.GetSafeHwnd ()) != FALSE)  
   {
      if (Flags & (WIZBUT_OK | WIZBUT_OKONLY))
         m_Back.ShowWindow (SW_HIDE);   
      else
      {
         if (Flags & WIZBUT_BACK)
            Enable = TRUE;
         else
            Enable = FALSE;
      
         m_Back.EnableWindow (Enable);
         m_Back.ShowWindow (SW_SHOW);   
      }
   }

   if (IsWindow (m_Next.GetSafeHwnd ()) != FALSE)  
   {
#ifdef LOCALIZE
	CString TextFinish;
	TextFinish.LoadString(IDS_WIZFINISH);
	CString TextNext;
	TextNext.LoadString(IDS_WIZNEXT);
#endif
      if (Flags & (WIZBUT_NEXT | WIZBUT_FINISH | WIZBUT_OK | WIZBUT_OKONLY))
      {
         Enable = TRUE;
      
         if (Flags & WIZBUT_FINISH)
            m_Next.SetWindowText (TextFinish);
         else
         if (Flags & (WIZBUT_OK | WIZBUT_OKONLY)) 
            m_Next.SetWindowText (TextOK);
         else
            m_Next.SetWindowText (TextNext);
      }
      else
      {
         m_Next.SetWindowText (TextFinish);
         Enable = FALSE;
      }
      
      m_Next.EnableWindow (Enable);
      
   }

   if (IsWindow (m_Cancel.GetSafeHwnd ()) != FALSE)  
   {
      if (Flags & WIZBUT_OKONLY)
         m_Cancel.ShowWindow (SW_HIDE);   
      else
      {
         if (Flags & WIZBUT_CANCEL)
            Enable = TRUE;
         else
            Enable = FALSE;
      
         m_Cancel.EnableWindow (Enable);
         m_Cancel.ShowWindow (SW_SHOW);   
      }
   }
}

#define  WIZBUT_WIDTH   50 // Typical dialog unit button width
#define  WIZBUT_HEIGHT  14 // Typical dialog unit button height
#define  WIZBUT_GAP     2
#define	WIZBUT_COUNT	3		// Back, Next(Finish), Cancel   (no help)

void
CPmwDialogColor::CreateWizzardButtons (void)
{
   int         s;
   DWORD       Style = WS_CHILD | WS_GROUP | WS_TABSTOP | BS_PUSHBUTTON | WS_DISABLED | WS_VISIBLE;
   CRect       RectClient, RectButton;
   CFont       *pFont;
#ifdef LOCALIZE
   CString	TextNext;
   TextNext.LoadString(IDS_WIZNEXT);
   CString TextBack;
   TextBack.LoadString(IDS_WIZBACK);
   CString TextCancel;
   TextCancel.LoadString(IDS_WIZCANCEL);
#endif

   GetClientRect (&RectClient);

   // Create rect for all buttons
   RectButton.top = 0;
   RectButton.bottom = WIZBUT_HEIGHT;
   RectButton.left = 0;
   RectButton.right = WIZBUT_WIDTH;

   // Convert to pixels
   MapDialogRect(&RectButton);
   
   // Make button smaller until they will fit in window
   ASSERT ((RectButton.Width()*WIZBUT_COUNT)+(WIZBUT_GAP*(WIZBUT_COUNT-1)) < RectClient.Width());
   while ((RectButton.Width()*WIZBUT_COUNT)+(WIZBUT_GAP*(WIZBUT_COUNT-1)) >= RectClient.Width())
	{
      RectButton.right--;
	}

   // Calculate space on left for centered buttons
   s = (RectClient.Width()-((RectButton.Width()*WIZBUT_COUNT)+(WIZBUT_GAP*(WIZBUT_COUNT-1))))/2;

   // Move rect to button position
   RectButton.OffsetRect(s, RectClient.Height()-(RectButton.Height()+COLOR_BOX_GAP+COLOR_BOX_GAP_MAX));

   pFont = GetFont();

#ifdef LOCALIZE
   if (m_Back.Create(TextBack, Style, RectButton, this, IDC_BACK) == TRUE)
#else
   if (m_Back.Create("< &Back", Style, RectButton, this, IDC_BACK) == TRUE)
#endif
	{
      m_Back.SetFont(pFont);   
	}
   
   RectButton.OffsetRect(RectButton.Width()+WIZBUT_GAP, 0);
   if (m_Next.Create(TextNext, Style | BS_DEFPUSHBUTTON, RectButton, this, IDOK) == TRUE)
	{
      m_Next.SetFont(pFont);
	}

   RectButton.OffsetRect(RectButton.Width()+WIZBUT_GAP, 0);
#ifdef LOCALIZE
   if (m_Cancel.Create(TextCancel, Style, RectButton, this, IDCANCEL) == TRUE)
#else
   if (m_Cancel.Create("Cancel", Style, RectButton, this, IDCANCEL) == TRUE)
#endif
	{
      m_Cancel.SetFont (pFont);
	}

   ForceColorBoxRecalc();
}

BOOL CPmwDialogColor::OnInitDialog (void)
{
   INHERITED::OnInitDialog ();

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPmwDialogColor::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPmwDialogColor)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPmwDialogColor, CPmwDialog)
   //{{AFX_MSG_MAP(CPmwDialogColor)
   ON_WM_ERASEBKGND()
   ON_WM_DESTROY()
   ON_WM_NCHITTEST()
	ON_WM_PARENTNOTIFY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void
CPmwDialogColor::GetControlBounds (CRect *Rect, BOOL Equalize)
{
   CRect RectTemp;
   
   GetClientRect (&RectTemp);
                                             // Hey, there's no sizing with dialogs
   //if (1)
   if ((m_ControlBounds.IsRectEmpty () != 0) || (RectTemp != m_RectClient))
   {
      UINT  y, x;
      CWnd  *child;

      m_RectClient = RectTemp;

      for (x = 0, child = GetTopWindow() ; child != NULL ; child = child->GetNextWindow())
      {
         if (child->IsWindowVisible () != FALSE)
         {
            child->GetWindowRect (&RectTemp);
            ScreenToClient (&RectTemp);
      
            if ((x == 0) || (RectTemp.left - COLOR_BOX_GAP < m_ControlBounds.left))
               m_ControlBounds.left = RectTemp.left - COLOR_BOX_GAP;

            if ((x == 0) || (RectTemp.top - COLOR_BOX_GAP < m_ControlBounds.top))
               m_ControlBounds.top = RectTemp.top - COLOR_BOX_GAP;

            if ((x == 0) || (RectTemp.right + COLOR_BOX_GAP > m_ControlBounds.right))
               m_ControlBounds.right = RectTemp.right + COLOR_BOX_GAP;

            if ((x == 0) || (RectTemp.bottom + COLOR_BOX_GAP > m_ControlBounds.bottom))
               m_ControlBounds.bottom = RectTemp.bottom + COLOR_BOX_GAP;
            
            x ++;
         }
      }

      // Make sure within limits
      if (m_ControlBounds.left < 0)
         m_ControlBounds.left = 0;
      
      if (m_ControlBounds.top < 0)
         m_ControlBounds.top = 0;

      if (m_ControlBounds.right < 0)
         m_ControlBounds.right = 0;

      if (m_ControlBounds.bottom < 0)
         m_ControlBounds.bottom = 0;

      if (m_ControlBounds.left > m_RectClient.left + COLOR_BOX_GAP_MAX)
         m_ControlBounds.left = m_RectClient.left + COLOR_BOX_GAP_MAX;
      
      if (m_ControlBounds.top > m_RectClient.top + COLOR_BOX_GAP_MAX)
         m_ControlBounds.top = m_RectClient.top + COLOR_BOX_GAP_MAX;

      if (m_ControlBounds.right < m_RectClient.right - COLOR_BOX_GAP_MAX)
         m_ControlBounds.right = m_RectClient.right - COLOR_BOX_GAP_MAX;

      if (m_ControlBounds.bottom < m_RectClient.bottom - COLOR_BOX_GAP_MAX)
         m_ControlBounds.bottom = m_RectClient.bottom - COLOR_BOX_GAP_MAX;
      
      //if (0)
      if (Equalize == TRUE)
      {
         // Find Smallest gap
         x = m_ControlBounds.left - m_RectClient.left;

         y = m_ControlBounds.top - m_RectClient.top;
         if (y < x)
            x = y;

         y = m_RectClient.right - m_ControlBounds.right;
         if (y < x)
            x = y;

         y = m_RectClient.bottom - m_ControlBounds.bottom;
         if (y < x)
            x = y;
         // Make bound rect equally smaller than the client
         m_ControlBounds.left = m_RectClient.left + x;
         m_ControlBounds.top = m_RectClient.top + x;
         m_ControlBounds.right = m_RectClient.right - x;
         m_ControlBounds.bottom = m_RectClient.bottom - x;
      }
   }

   *Rect = m_ControlBounds;
}

void
CPmwDialogColor::DrawColorBox (CDC* pDC)
{
   if (MatteColor != INVALID_DIALOG_COLOR)
   {
      TRY
      {
         CPen PenBlack (PS_SOLID, 1, BRAND_NEW_BOX_COLOR), PenColor (PS_SOLID, 1, MatteColor), *pPen;
      
         CBrush* pOldBrush = pDC->SelectObject (&Brush);
      
         CRect RectClient, Bounds;

         GetControlBounds (&Bounds);
         GetClientRect (&RectClient);
      
         // Left
         pPen = pDC->SelectObject (&PenColor);
         pDC->Rectangle (0, 0, Bounds.left, RectClient.bottom);
         pDC->SelectObject (&PenBlack);
         pDC->MoveTo (Bounds.left, Bounds.top);
         pDC->LineTo (Bounds.left, Bounds.bottom);
      
         // Top
         pDC->SelectObject (&PenColor);
         pDC->Rectangle (0, 0, RectClient.right, Bounds.top);
         pDC->SelectObject (&PenBlack);
         pDC->MoveTo (Bounds.left, Bounds.top);
         pDC->LineTo (Bounds.right, Bounds.top);
      
         // Right
         pDC->SelectObject (&PenColor);
         pDC->Rectangle (Bounds.right, 0, RectClient.right, RectClient.bottom);
         pDC->SelectObject (&PenBlack);
         pDC->MoveTo (Bounds.right, Bounds.top);
         pDC->LineTo (Bounds.right, Bounds.bottom);
      
         // Bottom
         pDC->SelectObject (&PenColor);
         pDC->Rectangle (0, Bounds.bottom, RectClient.right, RectClient.bottom);
         pDC->SelectObject (&PenBlack);
         pDC->MoveTo (Bounds.left, Bounds.bottom);
         pDC->LineTo (Bounds.right, Bounds.bottom);

         pDC->SelectObject (pOldBrush);
         pDC->SelectObject (pPen);
      }
      CATCH_ALL (e)
      {
      }
      END_CATCH_ALL
   }
}

void
CPmwDialogColor::DrawLittleGuy (CDC* pDC)
{
#if 0
   CBitmap  BitmapWhole;

   if (m_UseLittleGuy == FALSE)
      return;

   if (BitmapWhole.LoadBitmap (IDB_MASTER) != FALSE)
   {
      int   Left;
      CRect RectGuy (0, 0, GUY_WIDTH, GUY_HEIGHT), RectTV, RectClient;

      RectTV = RectGuy;

CRect Temp (0, 0, GUY_WIDTH / 2, GUY_HEIGHT / 2);
RectTV = Temp;

      GetClientRect (&RectClient);
   
      if (m_ControlBounds.IsRectEmpty () == 0)
         Left = m_ControlBounds.right - RectTV.Width () - GUY_GAP;
      else
         Left = RectClient.right - RectTV.Width () - GUY_GAP;

      RectTV.OffsetRect (Left, (RectClient.Height () / 2) - (RectTV.Height () / 2));
      RectGuy.OffsetRect (m_GuyFrame * GUY_WIDTH, 0);

      Util::TransparentBltBitmap (*pDC, RectTV, BitmapWhole, RectGuy, pOurPal);
      
      BitmapWhole.DeleteObject ();
   }
#else

   if (m_UseLittleGuy == FALSE)
      return;

   if (sm_bmGuyFrames.GetSafeHandle() && sm_nNumFrames > 0)
   {
      int      Left;
      BITMAP   sbitmap;
      sm_bmGuyFrames.GetObject (sizeof (sbitmap), &sbitmap);
      CRect RectGuy (0, 0, sbitmap.bmWidth / sm_nNumFrames, sbitmap.bmHeight), RectTV, RectClient;

      RectTV = RectGuy;

//CRect Temp (0, 0, RectTV.Width () / 2, RectTV.Height () / 2); RectTV = Temp;
//CRect Temp (0, 0, (RectTV.Width () / 2) + (RectTV.Width () / 4), (RectTV.Height () / 2) + (RectTV.Height () / 4)); RectTV = Temp;

      int   gap = GUY_GAP;

gap = 16 - (RectTV.Width () / 8);

      GetClientRect (&RectClient);
   
      if (m_ControlBounds.IsRectEmpty () == 0)
         Left = m_ControlBounds.right - RectTV.Width () - gap;
      else
         Left = RectClient.right - RectTV.Width () - gap;

      RectTV.OffsetRect (Left, (RectClient.Height () / 2) - (RectTV.Height () / 2));
      RectGuy.OffsetRect (m_GuyFrame * RectGuy.Width (), 0);

      Util::TransparentBltBitmap (*pDC, RectTV, sm_bmGuyFrames, RectGuy, pOurPal);
   }
#endif
}
   
/////////////////////////////////////////////////////////////////////////////
// CPmwDialogColor message handlers

BOOL CPmwDialogColor::OnEraseBkgnd(CDC* pDC) 
{
   CPalette *pPaletteOld;
   
   if ((pOurPal != NULL) && (pOurPal->m_hObject != NULL))
   {
      pPaletteOld = pDC->SelectPalette (pOurPal, FALSE);
      pDC->RealizePalette ();
   }

   INHERITED::OnEraseBkgnd(pDC);
   
//   DrawColorBox (pDC);
   DrawLittleGuy (pDC);

   if ((pOurPal != NULL) && (pOurPal->m_hObject != NULL))
   {
      pDC->SelectPalette (pPaletteOld, FALSE);
      pDC->RealizePalette ();
   }
   
   return TRUE;  // message handled
}

void CPmwDialogColor::OnDestroy() 
{
   INHERITED::OnDestroy();
}

UINT CPmwDialogColor::OnNcHitTest(CPoint point) 
{
   if ((m_NonClientMoveable == FALSE) || (GetWindowLong (GetSafeHwnd (), GWL_STYLE) & WS_CAPTION))
      return INHERITED::OnNcHitTest(point);
      
   if (m_ControlBounds.IsRectEmpty () == FALSE)
   {
      ScreenToClient (&point);

      if (point.x < m_ControlBounds.left)
         return HTCAPTION;
      if (point.x > m_ControlBounds.right)
         return HTCAPTION;
      if (point.y > m_ControlBounds.bottom)
         return HTCAPTION;
      if (point.y < m_ControlBounds.top)
         return HTCAPTION;
      
      return HTCLIENT;
   }
   
   return HTCAPTION;
}

void CPmwDialogColor::OnParentNotify(UINT message, LPARAM lParam) 
{
	CPmwDialog::OnParentNotify(message, lParam);
	
   CPmwApp  *pApp = GET_PMWAPP();

   if (pApp->m_fSoundEffectsState == FALSE)
      return;

//od ("CMainFrame Notification\n");

   if (WM_LBUTTONDOWN == LOWORD (message))
   {
//od ("CMainFrame WM_LBUTTONDOWN Notification\n");
      UINT  IDButton;//, IDWindow;
      HWND  hWnd;
      CPoint   Point (lParam);

      if ((hWnd = Util::ChildWindowFromPointEx (m_hWnd, Point)) != NULL)
      {
         ::ClientToScreen (m_hWnd, &Point);
         ::ScreenToClient (m_hWnd, &Point);
         DWORD d = MAKELONG(Point.x, Point.y);

         IDButton = (UINT)::SendMessage (m_hWnd, WM_HELPHITTEST, 0, d);
         IDButton = LOWORD(IDButton);
         
         if ((IDButton != 0) && (IDButton != (UINT)(SHORT)-1))
         {
            if (::IsWindowEnabled (hWnd) != FALSE)
            {
//od ("Button %u pressed\n", IDButton);               
               //pApp->m_MciWnd.PlaySoundFX (IDButton, m_sound_id);
               pApp->m_MciWnd.PlaySoundFX (IDButton, IDD_GENERIC_DIALOG_ID);
            }
         }
      }
   }
}
