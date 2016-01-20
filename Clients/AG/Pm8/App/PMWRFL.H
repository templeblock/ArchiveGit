/*
// $Workfile: PMWRFL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
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
// $Log: /PM8/App/PMWRFL.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:18   Fred
// Initial revision.
// 
//    Rev 1.1   01 Jul 1997 15:21:30   Jay
// 16-bit changes, etc.
// 
//    Rev 1.0   01 Jul 1997 10:52:42   Jay
// New MRU stuff
// 
//    Rev 1.0   26 Mar 1997 15:17:42   Fred
// Initial revision.
*/

#ifndef __PMWRFL_H__
#define __PMWRFL_H__

/////////////////////////////////////////////////////////////////////////////
// CPMWRecentFileList

#ifdef WIN32 
class CPMWRecentFileList : public CRecentFileList
{
public:
   CPMWRecentFileList(UINT nStart, LPCTSTR lpszSection,
      LPCTSTR lpszEntryFormat, int nSize,
      int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);
   virtual void Add(LPCTSTR lpszPathName);
   virtual void UpdateMenu(CCmdUI* pCmdUI);
};
#else

#define AFX_ABBREV_FILENAME_LEN 30

class CPMWRecentFileList
{
// Constructors
public:
   CPMWRecentFileList(UINT nStart, LPCTSTR lpszSection,
      LPCTSTR lpszEntryFormat, int nSize,
      int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);

// Attributes
   int GetSize() const;
   CString& operator[](int nIndex);

// Operations
   virtual void Remove(int nIndex);
   virtual void Add(LPCTSTR lpszPathName);
   BOOL GetDisplayName(CString& strName, int nIndex,
      LPCTSTR lpszCurDir, int nCurDir, BOOL bAtLeastName = TRUE) const;
   virtual void UpdateMenu(CCmdUI* pCmdUI);
   virtual void ReadList();    // reads from registry or ini file
   virtual void WriteList();   // writes to registry or ini file

// Implementation
   virtual ~CPMWRecentFileList();

   int m_nSize;                // contents of the MRU list
   CString* m_arrNames;
   CString m_strSectionName;   // for saving
   CString m_strEntryFormat;
   UINT m_nStart;              // for displaying
   int m_nMaxDisplayLength;
   CString m_strOriginal;      // original menu item contents
};

inline int CPMWRecentFileList::GetSize() const
   { return m_nSize; }
inline CString& CPMWRecentFileList::operator[](int nIndex)
   { ASSERT(nIndex < m_nSize); return m_arrNames[nIndex]; }

#endif

#endif
