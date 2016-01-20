/*
// $Workfile: TEXTLINE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TEXTLINE.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:40   FRED
// Initial revision.
// 
//    Rev 1.0   15 Nov 1995 14:41:12   FRED
// Initial revision.
*/

class CTextLineFile;

#ifndef __TEXTLINE_H__
#define __TEXTLINE_H__

class CTextLineFile
{
public:
	CTextLineFile();
	virtual ~CTextLineFile();
	
public:
	BOOL Name(LPCSTR pszFileName);

public:	
	BOOL ReadLine(CString& csLine);
	void WriteLine(CString& csLine);
	BOOL CopyLine(void);
	void CopyRestOfLines(void);
	BOOL Done(BOOL fAbort = FALSE, CString* pcsBackupName = NULL);
	
protected:
	CString m_csRead;
	CStdioFile m_cfRead;
	CString m_csWrite;
	CStdioFile m_cfWrite;
	
	void CloseFiles(void);
	
	void StartReading(void);
	void StartWriting(void);
};

#endif					// #ifndef __TEXTLINE_H__
