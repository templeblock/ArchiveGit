/*
// $Workfile: SMACKER.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SMACKER.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:28   Fred
// Initial revision.
// 
//    Rev 1.5   17 Jul 1997 09:08:50   Jay
// Now processes paint messages for the main window.
// 
//    Rev 1.4   02 Jun 1997 15:21:34   Jay
// Won't interrupt on mouse up messages.
// 
//    Rev 1.3   28 May 1997 12:02:40   johno
// Don't interrupt movie for WM_KEYUP
// 
//    Rev 1.2   25 Apr 1997 11:06:26   Fred
// Better interruption
// 
//    Rev 1.1   31 Mar 1997 12:59:04   Fred
// Start of 16-bit movie support
// 
//    Rev 1.0   31 Mar 1997 10:37:54   Fred
// Initial revision.
*/

#include "stdafx.h"
#include <mmsystem.h>

#include "Smacker.h"


IMPLEMENT_DYNAMIC(CSmacker, CObject)

CSmacker::CSmacker(CWnd* pParent, LPCSTR pszFilename)
{
   m_pParent = pParent;
   m_csPathname = pszFilename;

   m_bMaxFrameRate = FALSE;
   m_bSoundOff = FALSE;
}

CSmacker::~CSmacker()
{
}

///////////////////////////////////////////////////////////////////////////
// SetMaxFrameRate()
//
// call this before calling PlayMovie() if you want the flik to play
// at maximum speed (or to reset to normal speed after setting to max)
//
///////////////////////////////////////////////////////////////////////////
void CSmacker::SetMaxFrameRate(BOOL bMax)
{
   m_bMaxFrameRate = bMax;
}

///////////////////////////////////////////////////////////////////////////
// SetSoundOff()
//
// call this before calling PlayMovie() if you want to set the sound off
// (or back on after having turned it off)
//
///////////////////////////////////////////////////////////////////////////
void CSmacker::SetSoundOff(BOOL bOff /* = TRUE */)
{
   m_bSoundOff = bOff;
}

int CSmacker::PlayMovie(BOOL bNoInterrupt)
{
   BOOL bInterrupt = FALSE;

   u32 flags = m_bSoundOff ? NULL : SMACKTRACKS;
   m_smk = SmackOpen((LPTSTR)(LPCSTR)m_csPathname, flags, SMACKAUTOEXTRA);
   if (!m_smk)
   {
      return SmkFailure;
   }

   CRect crBackground;
   m_pParent->GetClientRect(crBackground);

   CWnd BackgroundWindow;
   BOOL fBackgroundCreated = BackgroundWindow.Create(
                                 AfxRegisterWndClass(
                                       CS_PARENTDC,
                                       0,
                                       (HBRUSH)::GetStockObject(BLACK_BRUSH),
                                       0),
                                 "",
                                 WS_CHILD|WS_VISIBLE,
                                 crBackground,
                                 m_pParent,
                                 12345,
                                 NULL);

   if (fBackgroundCreated)
   {
      CDC* pDC = BackgroundWindow.GetDC();
      BackgroundWindow.Invalidate(); // force window to draw.
      BackgroundWindow.UpdateWindow();

      HCURSOR hArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
      if (hArrow != NULL)
      {
         ::SetCursor(hArrow);
      }

      m_sbuf = SmackBufferOpen(
                  BackgroundWindow.m_hWnd,
                  SMACKAUTOBLIT,
                  (WORD)m_smk->Width,
                  (WORD)m_smk->Height,
                  (WORD)m_smk->Width,
                  (WORD)m_smk->Height);

      if (m_bMaxFrameRate)
      {
         SmackFrameRate(/*m_smk,*/ 0);  // max frame playback speed
      }

      if (m_sbuf != NULL)
      {
         SmackToBuffer(
            m_smk,
            0,
            0,
            m_sbuf->Width,
            m_sbuf->Height,
            m_sbuf->Buffer,
            (u8)m_sbuf->Reversed);
      }

      int nXOffset = (crBackground.Width()-(int)(m_sbuf->Width))/2;
      int nYOffset = (crBackground.Height()-(int)(m_sbuf->Height))/2;

      for (;;) 
      {
          if (!SmackWait(m_smk))
          {
            if (!DoAdvance(pDC, nXOffset, nYOffset))
            {
               break;
            }
          }

         if (!bNoInterrupt)
         {
            // check for mouse click or keystroke so we can truncate movie 
#if 0
            MSG msg;
            if (::PeekMessage(&msg, m_pParent->m_hWnd, WM_KEYDOWN, WM_KEYDOWN, PM_NOREMOVE))
            {
               if (msg.message == WM_KEYDOWN)
               {
                  // Eat the message.
                  GetMessage(&msg, m_pParent->m_hWnd, msg.message, msg.message);
                  bInterrupt = TRUE;
                  break;
               }
            }
            if (::PeekMessage(&msg, m_pParent->m_hWnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN, PM_NOREMOVE))
            {
               // Eat the message.
               if (msg.message == WM_LBUTTONDOWN)
               {
                  GetMessage(&msg, m_pParent->m_hWnd, msg.message, msg.message);
                  bInterrupt = TRUE;
                  break;
               }
            }
#else
            MSG msg;
            if (::PeekMessage(&msg, m_pParent->m_hWnd, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
            {
               if (msg.message != WM_KEYUP)  // JRO I don't know why this seems to be needed for 16 bit
               {
                  bInterrupt = TRUE;
                  break;
               }   
            }
            if (::PeekMessage(&msg, m_pParent->m_hWnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
            {
               if (msg.message != WM_MOUSEMOVE
							&& msg.message != WM_LBUTTONUP
							&& msg.message != WM_MBUTTONUP
							&& msg.message != WM_RBUTTONUP)
               {
                  bInterrupt = TRUE;
                  break;
               }
            }
            if (::PeekMessage(&msg, m_pParent->m_hWnd, WM_NCMOUSEMOVE, WM_NCMBUTTONDBLCLK, PM_REMOVE))
            {
               if (msg.message != WM_NCMOUSEMOVE)
               {
                  bInterrupt = TRUE;
                  break;
               }
            }
            if (::PeekMessage(&msg, m_pParent->m_hWnd, WM_PAINT, WM_PAINT, PM_REMOVE))
            {
               if (msg.hwnd == m_pParent->m_hWnd)
               {
                  UpdateWindow(msg.hwnd);
               }
            }
#endif
         }
      }

      if (m_sbuf != NULL)
      {
          SmackBufferClose(m_sbuf);
      }

      BackgroundWindow.ReleaseDC(pDC);

      // Force the window to draw so we won't see any palette change.
      BackgroundWindow.Invalidate();
      BackgroundWindow.UpdateWindow();
      BackgroundWindow.DestroyWindow();
   }

   SmackClose(m_smk);

   return bInterrupt ? SmkInterrupt : SmkDone;
}


int CSmacker::DoAdvance(CDC* pDC, int nX, int nY)
{
   if (m_smk->NewPalette)
   {
      SmackBufferNewPalette(m_sbuf, m_smk->Palette, (u16)m_smk->PalType);
   }

   SmackDoFrame(m_smk);

   while (SmackToBufferRect(m_smk, (WORD)m_sbuf->SurfaceType))
   {
      if (SmackBufferBlit(
            m_sbuf,
            pDC->m_hDC,
            nX,
            nY,
            (WORD)m_smk->LastRectx,
            (WORD)m_smk->LastRecty,
            (WORD)m_smk->LastRectw,
            (WORD)m_smk->LastRecth))
      {
         break;
      }
   }

   if (m_smk->FrameNum == (m_smk->Frames-1))
   {
      return 0;
   }

   SmackNextFrame(m_smk);

   return 1;
}
