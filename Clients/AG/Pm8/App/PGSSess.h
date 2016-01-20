//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/PGSSess.h 1     3/03/99 6:09p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/PGSSess.h $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 4     10/09/98 5:54p Jayn
// New "get graphic" implementation.
// 
// 3     7/13/98 3:43p Psasse
// Corrupted files are no longer saved and cancellation of downloading
// enabled
// 
// 2     7/12/98 5:31p Jayn
// Improved graphic download. Cleanup of code in general.
//////////////////////////////////////////////////////////////////////////////

#ifndef __PGSSESS_H__
#define __PGSSESS_H__

// PGSSess.h : class definitions for CPGSSession

class CDownloadStatusDialog;

class CPGSSession : public CInternetSession
{
public:
	CPGSSession(LPCTSTR pszAppName, int nMethod);
	virtual void OnStatusCallback(DWORD dwContext,
											DWORD dwInternetStatus,
											LPVOID lpvStatusInfomration,
											DWORD dwStatusInformationLen);
	void SetStatusDialog(CDownloadStatusDialog* pStatusDialog)
		{ m_pStatusDialog = pStatusDialog; }

	BOOL GetAbortStatus ()
	{ return m_fAbort; }

protected:
	CDownloadStatusDialog* m_pStatusDialog;
	BOOL m_fAbort;
};

#endif

