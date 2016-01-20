/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                suminfo.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "suminfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */
/* ********************************** Macros ******************************** */
/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */

static const TCHAR szSummaryInfo[] = _T("\005SummaryInformation");

/* ******************************* Public Data ****************************** */

/* *************************** Private Functions **************************** */

/* *************************** Private Methods **************************** */

BOOL CSummaryInfo::accessSummaryStream(LPSTORAGE lpRootStg,COleStreamFile &file)
{
    BOOL bResult = TRUE;
	CFileException fe;

TRY_OPEN_AGAIN:
	if (!file.OpenStream(lpRootStg,szSummaryInfo,
    	CFile::modeReadWrite|CFile::shareExclusive, &fe))
	{
		if (!file.CreateStream(lpRootStg, szSummaryInfo,
			CFile::modeReadWrite|CFile::shareExclusive|CFile::modeCreate, &fe))
				AfxThrowFileException(fe.m_cause, fe.m_lOsError);
        {
            // Write the current stuff and commit it
            bResult = write_summinfo (lpRootStg,file,TRUE);
            if (bResult)
                goto TRY_OPEN_AGAIN;
        }
	}
	return bResult;
}

BOOL CSummaryInfo::getDateTime (LPSUMINFO lpSummaryInfo, DWORD pid, CTime &tm)
{
	int yr,mo,dy,hours,minutes,seconds;
	DWORD dwSeconds;

	OleStdGetDateProperty(lpSummaryInfo, pid, &yr, &mo, &dy, &dwSeconds); 
	if (yr==0 || mo==0 || dy==0 || dwSeconds==0)
		return FALSE;

	hours = dwSeconds/(60*60);
	dwSeconds %= (60*60);
	minutes = dwSeconds/60; 
	dwSeconds %= 60;
	seconds = dwSeconds;

	tm = CTime( yr, mo, dy, hours, minutes, seconds );	
	return TRUE;
}				

BOOL CSummaryInfo::init_membervars(BOOL bTestThumbNail)
{
	CTime tm = CTime::GetCurrentTime();	
	DWORD len = 100;
	if (!::GetUserName( m_Author.GetBuffer(len),&len ))
		m_Author = "Unknown";
	m_Author.ReleaseBuffer();    			
	m_LastAuthor = m_Author;			
	m_Title = "";
	m_Subject = "";
	m_Keywords = "";
	m_Comments = "";
	m_AppName = "Micrografx Picture Publisher 5.0";
	m_RevNumber = "";
	m_CreateTime = tm;
	m_LastSavedTime = tm;
	m_bPrinted = FALSE;
	m_LastPrinted = tm;

//    if (bTestThumbNail && m_hThumbnail)
//    {
//        LPMETAFILEPICT lpmfp;
//        lpmfp = (LPMETAFILEPICT)GlobalLock(m_hThumbnail);
//        ::DeleteMetaFile(lpmfp->hMF);
//        GlobalUnlock(m_hThumbnail);
//        GlobalFree(m_hThumbnail);        
//    }
    m_hThumbnail = NULL;

	m_lpSummaryInfo = NULL;

    return TRUE;
}

BOOL CSummaryInfo::write_summinfo (LPSTORAGE lpRootStg,COleStreamFile & file,BOOL bCommitStg)
{
	DWORD len = 100;
	if (!::GetUserName( m_LastAuthor.GetBuffer(len),&len ))
		m_LastAuthor = "Unknown";
	m_LastSavedTime = CTime::GetCurrentTime();	

	OleStdSetStringProperty(m_lpSummaryInfo, PID_TITLE, m_Title.GetBuffer(m_Title.GetLength()) );
	OleStdSetStringProperty(m_lpSummaryInfo, PID_SUBJECT, m_Subject.GetBuffer(m_Subject.GetLength()) );
	OleStdSetStringProperty(m_lpSummaryInfo, PID_AUTHOR, m_Author.GetBuffer(m_Author.GetLength()) );
	OleStdSetStringProperty(m_lpSummaryInfo, PID_LASTAUTHOR, m_LastAuthor.GetBuffer(m_LastAuthor.GetLength()) );
	OleStdSetStringProperty(m_lpSummaryInfo, PID_KEYWORDS, m_Keywords.GetBuffer(m_Keywords.GetLength()) );
	OleStdSetStringProperty(m_lpSummaryInfo, PID_COMMENTS, m_Comments.GetBuffer(m_Comments.GetLength()) );
	OleStdSetStringProperty(m_lpSummaryInfo, PID_APPNAME, m_AppName.GetBuffer(m_AppName.GetLength()) );
	OleStdSetDateProperty(m_lpSummaryInfo, PID_CREATE_DTM_RO, m_CreateTime.GetYear(),m_CreateTime.GetMonth(),
		m_CreateTime.GetDay(),m_CreateTime.GetHour(),m_CreateTime.GetMinute(),m_CreateTime.GetSecond()); 
	OleStdSetDateProperty(m_lpSummaryInfo, PID_LASTSAVE_DTM, m_LastSavedTime.GetYear(),m_LastSavedTime.GetMonth(),
		m_LastSavedTime.GetDay(),m_LastSavedTime.GetHour(),m_LastSavedTime.GetMinute(),m_LastSavedTime.GetSecond()); 
	if (m_bPrinted)
		OleStdSetDateProperty(m_lpSummaryInfo, PID_LASTPRINTED, m_LastPrinted.GetYear(),m_LastPrinted.GetMonth(),
			m_LastPrinted.GetDay(),m_LastPrinted.GetHour(),m_LastPrinted.GetMinute(),m_LastPrinted.GetSecond()); 
    if (m_hThumbnail)
    {
        LPMETAFILEPICT lpmfp = (LPMETAFILEPICT)GlobalLock(m_hThumbnail);
        if (lpmfp)
        {
                DWORD clipFormatNo = CF_METAFILEPICT;
                DWORD sz = sizeof(METAFILEPICT); 
#ifdef WIN32
                sz += GetMetaFileBitsEx(lpmfp->hMF,0,NULL);
                OleStdSetThumbNailProperty(file.m_lpStream,m_lpSummaryInfo, VT_CF_WIN,
                    CF_METAFILEPICT, NULL, (THUMBNAIL)m_hThumbnail, sz);
#else 
/ not implemented
#endif            
            GlobalUnlock(m_hThumbnail);
        }
    }

	OleStdWriteSummaryInfo(file.m_lpStream,m_lpSummaryInfo);

	file.Close();	

	// commit the root storage
	if (bCommitStg)
	{
		SCODE sc = lpRootStg->Commit(STGC_ONLYIFCURRENT);
		if (sc != NOERROR)
			AfxThrowOleException(sc);
	}
    return (TRUE);
}

/* *************************** Public Methods **************************** */

CSummaryInfo::CSummaryInfo ()
{
    init_membervars();
}

CSummaryInfo::~CSummaryInfo ()
{
	if (m_lpSummaryInfo!= NULL)
		OleStdFreeSummaryInfo(&m_lpSummaryInfo);

//    if (m_hThumbnail)
//    {
//        LPMETAFILEPICT lpmfp;
//        lpmfp = (LPMETAFILEPICT)GlobalLock(m_hThumbnail);
//        ::DeleteMetaFile(lpmfp->hMF);
//        GlobalUnlock(m_hThumbnail);
//        GlobalFree(m_hThumbnail);        
//    }

}

BOOL CSummaryInfo::Init()
{
	BOOL bResult = FALSE;

	if (m_lpSummaryInfo!= NULL)
		OleStdFreeSummaryInfo(&m_lpSummaryInfo);

    init_membervars(TRUE);

	// CoInitialize must be called BEFORE OleStdInitSummaryInfo	 ; however already done in OleInitialize
	if (m_lpSummaryInfo = OleStdInitSummaryInfo(0/*reserved for future -- must be 0*/))
	{
		OleStdClearSummaryInfo(m_lpSummaryInfo);
		bResult = TRUE;
	}
	return bResult;
}

BOOL CSummaryInfo::ReadSummInfo(LPSTORAGE lpRootStg)
{
    if (lpRootStg == NULL)
        return TRUE; // no change to the current info

	ASSERT(m_lpSummaryInfo);

	// open or create Contents stream
	COleStreamFile file;
	if (!accessSummaryStream(lpRootStg,file))
		return FALSE;
	
	OleStdReadSummaryInfo(file.m_lpStream,m_lpSummaryInfo);
	
	m_Title = OleStdGetStringProperty(m_lpSummaryInfo, PID_TITLE);
	m_Subject = OleStdGetStringProperty(m_lpSummaryInfo, PID_SUBJECT);
	m_Author = OleStdGetStringProperty(m_lpSummaryInfo, PID_AUTHOR);
	m_LastAuthor = OleStdGetStringProperty(m_lpSummaryInfo, PID_LASTAUTHOR);
	m_Keywords = OleStdGetStringProperty(m_lpSummaryInfo, PID_KEYWORDS);
	m_Comments = OleStdGetStringProperty(m_lpSummaryInfo, PID_COMMENTS);
	m_AppName = OleStdGetStringProperty(m_lpSummaryInfo, PID_APPNAME);

	CTime tm;
	if (getDateTime(m_lpSummaryInfo,PID_CREATE_DTM_RO,tm))
		m_CreateTime = tm; 
	if (getDateTime(m_lpSummaryInfo,PID_LASTSAVE_DTM,tm))
		m_LastSavedTime = tm;
	if (getDateTime(m_lpSummaryInfo,PID_LASTPRINTED,tm))
	{
		m_bPrinted = TRUE;
		m_LastPrinted = tm;
	}
    DWORD clipFormatNo;
    DWORD byteCount;
    int vtcfNo = OleStdGetThumbNailProperty(file.m_lpStream,m_lpSummaryInfo, 
        &clipFormatNo,NULL,(THUMBNAIL FAR *)&m_hThumbnail,
        &byteCount,FALSE);

    ASSERT(! (vtcfNo==VT_CF_WIN && clipFormatNo==CF_METAFILEPICT));
    if (vtcfNo!=VT_CF_WIN || clipFormatNo!=CF_METAFILEPICT)
        m_hThumbnail = NULL;

	file.Close();	

	return TRUE;
}

BOOL CSummaryInfo::WriteSummInfo(LPSTORAGE lpRootStg,BOOL bCommitStg/*TRUE*/)
{
    if (lpRootStg == NULL)
        return TRUE;

	ASSERT(m_lpSummaryInfo);

	// open or create Contents stream
	COleStreamFile file;
	if (!accessSummaryStream(lpRootStg,file))
		return FALSE;

    // Update the Saved By  and Saved Time fields	
	DWORD len = 100;
	if (!::GetUserName( m_Author.GetBuffer(len),&len ))
		m_Author = "Unknown";
	m_LastSavedTime = CTime::GetCurrentTime();	

    return write_summinfo (lpRootStg,file,bCommitStg);
}

void CSummaryInfo::SetThumbNail(HANDLE hMetafilePict)
{
    if (!hMetafilePict)
        return;

//    if (m_hThumbnail)
//    {
//        LPMETAFILEPICT lpmfp;
//        lpmfp = (LPMETAFILEPICT)GlobalLock(m_hThumbnail);
//        ::DeleteMetaFile(lpmfp->hMF);
//        GlobalUnlock(m_hThumbnail);
//        GlobalFree(m_hThumbnail);        
//    }
    m_hThumbnail = hMetafilePict;   
}

/* ***************************THE END************************************* */

