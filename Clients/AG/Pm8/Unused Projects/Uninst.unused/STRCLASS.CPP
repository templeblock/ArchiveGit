/*
// $Workfile: STRCLASS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:36p $
//
*/

/*
// Revision History:
//
// $Log: /PM8/UNINST/STRCLASS.CPP $
// 
// 1     3/03/99 6:36p Gbeddow
// 
// 1     6/24/98 7:38a Mwilson
// 
// 1     6/22/98 10:03a Mwilson
// 
// 2     12/17/97 2:15p Jayn
// Commented out some TRACEs.
// 
//    Rev 1.0   14 Aug 1997 15:26:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:36   Fred
// Initial revision.
// 
//    Rev 1.3   05 Nov 1996 17:32:24   johno
// Tweak
// 
//    Rev 1.2   05 Nov 1996 15:04:20   johno
// Removed some inlines
// 
//    Rev 1.1   04 Nov 1996 16:07:00   johno
// Tweak
// 
//    Rev 1.0   04 Nov 1996 10:32:30   johno
// Initial revision.
*/

#include "stdafx.h"
#include "strclass.h"

const char *CResourceString::m_ErrorStr = "Error: String not found.";

ResourceElement*
CResourceString::Find (UINT id)
{
   int               i;
   ResourceElement   *pre;

   for (i = 0; i < m_Strings.GetSize (); i ++)
   {
      if (m_FindIndex >= m_Strings.GetSize ())
         m_FindIndex = 0;
      
      pre = (ResourceElement*)m_Strings.GetAt (m_FindIndex);
      if (pre->m_id == id)
      {
//         TRACE ("CResourceString found a string\n");
         return pre;
      }
      
      m_FindIndex ++;
   }
   
   return NULL;
}

ResourceElement*
CResourceString::Create (UINT id)
{
   int               i, len = 0;
   ResourceElement   *pre = NULL;

   if (m_max > 0)
   {
      i = m_Strings.GetSize ();
      while (i >= m_max)
      {
         Kill0 ();
         i --;
      }
   }

   TRY
   {
      pre = new ResourceElement (id);
      pre->cs = new CString;

      do
      {
         len += STRING_LENGTH;   
//         TRACE1 ("CResourceString loading a string of %d bytes\n", len);
         i = LoadString (m_ResourceInstance, id, pre->cs->GetBuffer (len), len);
         pre->cs->ReleaseBuffer ();

      } while (i >= (len - 1));  // Keep loading more of the string until we get it all
   
      if (i > 0)
      {
//         TRACE ("CResourceString adding a string\n");
         m_Strings.Add (pre);
         return pre;
      }
   }
   END_TRY

   if (pre != NULL)
   {
      if (pre->cs != NULL)
         delete pre->cs;
      delete pre;
   }

   return NULL;
}

void
CResourceString::SetInstanceHandle (HINSTANCE hi)
{
   if (m_ResourceInstance != hi)
   {
      Kill ();
      m_ResourceInstance = hi;
   }
}

void
CResourceString::Kill (void)
{
//   TRACE ("CResourceString killing all strings\n");
   while (m_Strings.GetSize () > 0)
   {
      Kill0 ();
   }
   m_FindIndex = 0;
}

void
CResourceString::Kill0 (void)
{
   ResourceElement   *pre;
   
   pre = (ResourceElement*)m_Strings.GetAt (0);
   delete pre->cs;
   delete pre;  
   m_Strings.RemoveAt (0);
//   TRACE ("CResourceString killing a string\n");
}
