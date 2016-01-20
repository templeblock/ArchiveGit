/*
// $Workfile: HUBMGR.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/HUBMGR.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 5     11/17/98 5:59p Johno
// Changes for dynamic "new content" art store button
// Some things made static so buttons change only when required
// 
// 4     6/19/98 12:25p Johno
// Clean up
// 
// 3     6/17/98 2:51p Johno
// Hub now frees resources when destroyed (KillHubDLL())
// Removed some inlines
// 
// 2     4/02/98 5:02p Fredf
// New 7.0 Hub Bitmaps
// 
//    Rev 1.1   17 Sep 1997 16:33:02   johno
// 
//    Rev 1.2   30 Jul 1997 12:53:40   johno
// Added a DEBUG_ERRORBOX
// 
//    Rev 1.1   24 Jun 1997 11:05:12   johno
// Added GetScreenIndexByRect (CRect &Rect);
// (from a function in WNDHUB.CPP)
// 
//    Rev 1.0   24 Jun 1997 08:55:18   johno
// Initial revision.
//
*/

#include "stdafx.h"
#include "hubmgr.h"   
#include "util.h"

//#define  JTRACE_ON
#include "jtrace.h"

struct ScreenInfo* CHubResourceManager::m_ScreenInfo = NULL;

ScreenInfo *
CHubResourceManager::GetTable(UINT ID)
{
   HRSRC hrsrc;
   
   if (m_ScreenInfo == NULL)
   {
      if ((hrsrc = FindResource(m_InstDLL, MAKEINTRESOURCE(ID), "HUBTABLE")) != NULL)
      {
         DWORD    Size = SizeofResource(m_InstDLL, hrsrc);
               
         ASSERT(Size > 0);
   
         HGLOBAL  hglobal;
         if ((hglobal = LoadResource(m_InstDLL, hrsrc)) != NULL)
         {
            BYTE  *lpRes;
                  
            if ((lpRes = (unsigned char *)LockResource(hglobal)) != NULL)
            {
               TRY
               {
                  m_ScreenInfo = (ScreenInfo *)new BYTE [Size];
                  memcpy(m_ScreenInfo, lpRes, (int)Size);
                  SetTablePointers(m_ScreenInfo);
               }
               CATCH_ALL(e)
               {
               }
               END_CATCH_ALL
                     
               UnlockResource(hglobal);
            }
               
            FreeResource(hglobal);
         }
      }   
      else
         DEBUG_ERRORBOX("CHubResourceManager: Resource not found");
   }   
   
   return m_ScreenInfo;
}

void
CHubResourceManager::SetTablePointers(ScreenInfo * const si)
{
   struct ButtonSet  *pButtons;
   struct ScreenSet  *pScreens;
   
JTRACE1("struct ScreenInfo is %d bytes\n", sizeof(struct ScreenInfo));
   
   pScreens = (struct ScreenSet *)((DWORD)si + sizeof(struct ScreenInfo));
   si->pScreen = pScreens;

   pButtons = (struct ButtonSet *)((DWORD)pScreens + (sizeof(struct ScreenSet) * si->nNumScreen));
   
   for (UINT i = 0; i < si->nNumScreen; i ++)
   {
      pScreens->pButton = pButtons;
      pScreens = (struct ScreenSet *)((DWORD)pScreens + sizeof(struct ScreenSet));
      pButtons = (struct ButtonSet *)((DWORD)pButtons + sizeof(struct ButtonSet) * si->nNumButton);
   }
}

int
CHubResourceManager::GetSmallestWindowSizeAndIndex(CPoint *p)const
{
   int x, y, idx = 0;
   x = m_ScreenInfo->pScreen [idx].Width;
   y = m_ScreenInfo->pScreen [idx].Height;

   for (UINT i = 1; i < m_ScreenInfo->nNumScreen; i ++)
   {
      if (
            (m_ScreenInfo->pScreen [i].Width < x)
            ||
            (m_ScreenInfo->pScreen [i].Height < y)
         )
      {
         idx = i;
      }
   }
   
   p->x = m_ScreenInfo->pScreen [idx].Width;
   p->y = m_ScreenInfo->pScreen [idx].Height;
   
   return idx;
}

UINT
CHubResourceManager::GetScreenIndexByRect(CRect &Rect)const
{
   UINT  i;

   for (i = 0; i < m_ScreenInfo->nNumScreen; i ++)
   {
      if (
            ((m_ScreenInfo->pScreen [i].Width - Rect.Width()) <= m_ScreenInfo->pScreen [i].ClipLimit)
            &&
            ((m_ScreenInfo->pScreen [i].Height - Rect.Height()) <= m_ScreenInfo->pScreen [i].ClipLimit)
         )
         break;
   }

   if (i >= m_ScreenInfo->nNumScreen)
      i = m_ScreenInfo->nNumScreen - 1;

   return i;
}

void
CHubResourceManager::KillHubDLL(void)
{
#ifdef  WIN32
   if (m_InstDLL != NULL)
#else
   if (m_InstDLL >= HINSTANCE_ERROR)
#endif
   {
      FreeLibrary(m_InstDLL);
      m_InstDLL = NULL;
   }
}

void
CHubResourceManager::KillResource(void)
{
   KillHubDLL();
   if (m_ScreenInfo != NULL)
   {
      delete m_ScreenInfo;
      m_ScreenInfo = NULL;
   }
}

BOOL
CHubResourceManager::IsSkippedButton(int id)const
{
   for (int i = 0; i < SkipButtonIDs.GetSize(); i ++)
      if ((int)SkipButtonIDs.GetAt(i) == id)
         return TRUE;

   return FALSE;
}

BOOL
CHubResourceManager::SetSkipButton(int id)
{
   if (IsSkippedButton(id) == TRUE)
      return FALSE;
   
   SkipButtonIDs.Add(id);
   return TRUE;   
}

