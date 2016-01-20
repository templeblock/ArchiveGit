/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                  suminfo.h

*****************************************************************************
*/


#ifndef _SUMINFO_H_
#define _SUMINFO_H_

#include "suminfo2.h"

/* ******************************* Public Data ***************************** */

/* ********************************* Constants ****************************** */

/* *************************** Class Declarations ************************** */

class CSummaryInfo
{
public:
	CSummaryInfo ();
	~CSummaryInfo ();

	BOOL Init();
	BOOL ReadSummInfo(LPSTORAGE lpRootStg);
	BOOL WriteSummInfo(LPSTORAGE lpRootStg,BOOL bCommitStg = TRUE);

	// Access functions 
	CString GetAuthor();
	void SetAuthor(CString Author);
	CString GetComments();
	void SetComments(CString Comments);
	CString GetKeywords();
	void SetKeywords(CString Keywords);
	CString GetSubject();
	void SetSubject(CString Subject);
	CString GetTitle();
  	void SetTitle(CString Title);
    CString GetCreated();
    CString GetLastAuthor();
    CString GetLastSaved();
    CString GetLastPrinted();
    HANDLE GetThumbNail();
    void SetThumbNail(HANDLE hMetafilePict);

	// call every time this doc is printed
	void MarkAsPrinted();

protected:

	BOOL accessSummaryStream(LPSTORAGE lpRootStg,COleStreamFile &file);
	BOOL getDateTime (LPSUMINFO lpSummaryInfo, DWORD pid, CTime &tm);
    BOOL init_membervars(BOOL bTestThumbNail = FALSE);
    BOOL write_summinfo (LPSTORAGE lpRootStg,COleStreamFile & file,BOOL bCommitStg);

private:
	CString m_Author;
	CString m_LastAuthor;
	CString m_Title;
	CString m_Subject;
	CString m_Keywords;
	CString m_Comments;
	CString m_AppName;
	CString m_RevNumber;
	CTime   m_CreateTime;
	CTime   m_LastSavedTime;
	CTime   m_LastPrinted;
	BOOL    m_bPrinted;
    HMETAFILEPICT  m_hThumbnail; // Handle to a METAFILEPICT

	LPSUMINFO m_lpSummaryInfo;
};

/* *************************** Inline Methods ****************************** */

inline CString CSummaryInfo::GetAuthor() {return m_Author;}
inline void CSummaryInfo::SetAuthor(CString Author) {m_Author = Author;}
inline CString CSummaryInfo::GetComments() {return m_Comments;}
inline void CSummaryInfo::SetComments(CString Comments) {m_Comments = Comments;}
inline CString CSummaryInfo::GetKeywords() {return m_Keywords;};
inline void CSummaryInfo::SetKeywords(CString Keywords) {m_Keywords = Keywords;};
inline CString CSummaryInfo::GetSubject() {return m_Subject;};
inline void CSummaryInfo::SetSubject(CString Subject) {m_Subject = Subject;};
inline CString CSummaryInfo::GetTitle() {return m_Title;};
inline void CSummaryInfo::SetTitle(CString Title) {m_Title = Title;};
inline CString CSummaryInfo::GetCreated() {return m_CreateTime.Format(_T("%A, %B, %d, %Y"));}
inline CString CSummaryInfo::GetLastAuthor() {return m_LastAuthor;}
inline CString CSummaryInfo::GetLastSaved() {return m_LastSavedTime.Format(_T("%A, %B, %d, %Y"));}
inline CString CSummaryInfo::GetLastPrinted() {return m_LastPrinted.Format(_T("%A, %B, %d, %Y"));}
inline HMETAFILEPICT CSummaryInfo::GetThumbNail() {return m_hThumbnail;};
inline void CSummaryInfo::MarkAsPrinted()
{
	m_bPrinted = TRUE;
	m_LastPrinted = CTime::GetCurrentTime();	 
};

/* ************************************************************************* */

#endif // _SUMINFO_H_

