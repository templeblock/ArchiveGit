/*
// $Workfile: NFX.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
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
// $Log: /PM8/App/NFX.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:46   Fred
// Initial revision.
// 
//    Rev 1.3   26 Jun 1997 12:18:20   Jay
// Got rid of a hanging (race) condition.
// 
//    Rev 1.2   09 Apr 1997 17:12:58   Jay
// DoWinExec to Util
// 
//    Rev 1.1   31 Mar 1997 17:12:38   Jay
// Start of NFX code
// 
//    Rev 1.0   28 Mar 1997 17:10:20   Jay
// Initial revision.
*/

#ifndef __NFX_H__
#define __NFX_H__

/////////////////////////////////////////////////////////////////////////////
// The NFX CartoonOMatic wrapper class.

class CCartoonOMatic
{
public:
	CCartoonOMatic();
	~CCartoonOMatic();

	// Create a new toon.
	BOOL NewToon(void);

	// Edit an existing toon.
	BOOL EditToon(LPCSTR pszModelName, LPCSTR pszModelState);

	// Get output parameters.
	LPCSTR GetModelName(void) const
		{ return m_csModelName; }

	LPCSTR GetModelState(void) const
		{ return m_csModelState; }

	int GetUserResult(void) const
		{ return m_nUserResult; }

	LPCSTR GetOutputFile(void) const
		{ return m_csOutputFile; }

	static UINT m_uTerminationMessage;

   static void GotTerminationMessage(void);

protected:
	int m_nUserResult;
	CString m_csModelName;
	CString m_csModelState;

	CString m_csIniFile;
	CString m_csOutputFile;

	static BOOL m_fTerminated;
	static BOOL m_fDisabled;

	// Process a toon.
	BOOL DoToon(void);
	void MakeTempName(CString& csName);
   void ReleaseFile(CString& csFile);
};

#endif
