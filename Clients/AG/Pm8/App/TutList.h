/*
// $Workfile: TutList.h $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TutList.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 5     6/15/98 2:58p Hforman
// reading HD first, then CD for tutorials; fixed sorting problem
// 
// 4     5/15/98 4:42p Fredf
// 
// 3     11/06/97 6:06p Hforman
// added new functionality: reading list of Tutorials from CD, checking if
// more exist on HD, generating CRC, etc.
 * 
 * 2     10/30/97 6:52p Hforman
 * finishing up functionality, etc.
*/
#ifndef __TUTLIST_H__
#define __TUTLIST_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CTutorialList class
class CTutorialList
{
public:
   CTutorialList();
   virtual ~CTutorialList();

// operations
   void        CreateList();
   void        DestroyList();

   int         GetSize();
   int         GetIDFromIndex(int nIndex);
   int         GetIndexFromID(int nID);
	BOOL			GetInfoFromIndex(int nIndex, LPCTSTR* pszCaption, LPCTSTR* pszType, LPCTSTR* pszTitle, LPCTSTR* pszPath);
	BOOL			GetInfoFromID(int nID, LPCTSTR* pszCaption, LPCTSTR* pszType, LPCTSTR* pszTitle, LPCTSTR* pszPath);

private:
   void        ReadInTutorialList(CString& filepath);
   void        CreateNewList(CString& dir);
   BOOL        CheckCrc(DWORD crc, CString& dir);
   DWORD       GenerateCrc(CString& dir);
   void        Sort();
	BOOL			IsInArray(int nID);
   static int  CompareTitle(const void* arg1, const void* arg2);
   static int  CompareType(const void* arg1, const void* arg2);
   
   struct Tutorial
   {
      int            nID;        // ID associated with this tutorial
      CString        csCaption;	// caption for tutorial (e.g., "Design Tutorial: Using Large Fonts")
      CString        csType;		// main menu text (e.g., "Design Tutorials")
      CString        csTitle;    // secondary menu text (e.g., "Using Large Fonts")
      CString        csPath;		// full pathname to DLL (including ".tut" at end)
      CStringArray   csAryPageName;// array of resource names for page bitmaps
   };

   CPtrArray  m_TutorialArray;  // array of Tutorial structs
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ifndef __TUTLIST_H__
