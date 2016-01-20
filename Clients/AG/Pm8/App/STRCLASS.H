/*
// $Workfile: STRCLASS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1996 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/STRCLASS.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:06   Fred
// Initial revision.
// 
//    Rev 1.6   12 Dec 1996 13:17:02   Fred
// Added comments
*/

class CResourceString;

#ifndef __STRCLASS_H__
#define __STRCLASS_H__

#define  STRING_LENGTH  64;

struct ResourceElement
{
   ResourceElement (UINT id)
   {
      cs = NULL;
      m_id = id;
   }

   ResourceElement ()
   {
      cs = NULL;
      m_id = (UINT)-1;
   }
   
   CString  *cs;
   UINT     m_id;
};


class CResourceString
{
public:
   CResourceString (int max = 0)
   {
      m_ResourceInstance = NULL;   
      m_FindIndex = 0;
      SetMax (max);
   }

   CResourceString (HINSTANCE hi, int max = 0)
   {
      SetInstanceHandle (hi);
      m_FindIndex = 0;
      SetMax (max);
   }

   ~CResourceString ()
   {
      Kill ();
   }

   void
   SetMax (int max)
   {
      if (max >= 0)
         m_max = max;
      else
         m_max = 0;
   }

   void
   SetInstanceHandle (HINSTANCE hi);

   LPCSTR
   GetPointer (UINT id)
   {
      ResourceElement   *pre;
      
      if ((pre = Find (id)) != NULL)
         return *pre->cs;

      if ((pre = Create (id)) != NULL)
         return *pre->cs;
      
      TRACE ("!!!! CResourceString FAILED !!!!\n");
      return m_ErrorStr;
   }
/*
   CString*
   GetString (int id)
   {
      ResourceElement   *pre;
      
      if ((pre = Find (id)) != NULL)
         return pre->cs;
      if ((pre = Create (id)) != NULL)
         return pre->cs;
      return NULL;
   }
*/
   void
   Kill (void);

private:
   static const char *m_ErrorStr;
   HINSTANCE         m_ResourceInstance;
   CPtrArray         m_Strings;
   int               m_FindIndex;
   int               m_max;

   void
   Kill0 (void);

   ResourceElement*
   Find (UINT id);

   ResourceElement*
   Create (UINT id);
};

#endif
