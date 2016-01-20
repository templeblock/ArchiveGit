/*
// $Workfile: CPFL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CPFL.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     8/05/98 10:55a Mwilson
// added a compresssion option to the installer
// 
//    Rev 1.0   14 Aug 1997 15:19:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:16   Fred
// Initial revision.
// 
//    Rev 1.2   01 Nov 1996 09:29:34   Jay
// Moved changes over from 3.01.17
// 
//    Rev 1.2   29 Oct 1996 15:25:48   Fred
// Twain install
// 
//    Rev 1.1   01 Oct 1996 16:04:14   DennisP
// Added WinGroup for Twain support
// 
//    Rev 1.0   14 Mar 1996 13:26:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:36   FRED
// Initial revision.
// 
//    Rev 2.4   27 Aug 1995 18:29:54   FRED
// Changes to make install program not reference OLE
// 
//    Rev 2.3   08 Aug 1995 09:01:00   FRED
// Publishing Suite Installation
// 
//    Rev 2.2   07 Aug 1995 17:11:28   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:39:16   JAY
// Reverted. New series.
// 
//    Rev 1.3   22 Jun 1994 14:50:52   FRED
// Supports program file lines (in PMW.PFL) with
// elements separated by white space instead of '|'
// characters. The '|' character is replaced by space
// in file names after they have been parsed.
// 
// 
//    Rev 1.2   21 Jun 1994 14:40:06   FRED
// Changed old "flags" to "groups".
// Added "groups" field.
// 
// 
//    Rev 1.1   16 Jun 1994 18:14:56   FRED
// Version that copies files.
// 
// 
//    Rev 1.0   15 Jun 1994 19:00:12   FRED
// Initial revision.
*/ 

#ifndef _CPFL_H_
#define _CPFL_H_

/////////////////////////////////////////////////////////////////////////////
// CProgramFile

class CProgramFile
{
public:
// Groups
	enum
	{
		ProgramGroup				=	0x0001,
		IdapiRequiredGroup		=	0x0002,
		IdapiConfigurationGroup	=	0x0004,
		IdapiOptionalGroup		=	0x0008,
		OleGroup						=	0x0010, 
		TwainGroup					=	0x0020
	};
	
// Flags
	enum
	{
		Shared						=	0x0001,
		IgnoreIfInUse				=	0x0002,
		IgnoreIfOlder				=	0x0004,
		IgnoreIfExists				=	0x0008,
		ForceWindowsDir			=	0x0010,
		ForceSystemDir				=	0x0020
	};
	
// Construction
public:
	CProgramFile();

// Attributes
protected:
	DWORD m_dwGroups;
	CString m_csSource;
	CString m_csDestination;
	DWORD m_dwSize;
	DWORD m_dwFlags;
	DWORD m_dwCompression;

// Operations
public:
	virtual BOOL Init(LPCSTR lpLine);
	virtual DWORD GetFileSize(DWORD dwClusterSize) const;
	virtual DWORD GetGroups(void) const
		{ return m_dwGroups; }
	virtual const CString &GetSource(void) const
		{ return m_csSource; }
	virtual const CString &GetDestination(void) const
		{ return m_csDestination; }
	virtual DWORD GetSize(void) const
		{ return m_dwSize; }
	virtual DWORD GetFlags(void) const
		{ return m_dwFlags; }
	virtual DWORD GetCompression(void) const
		{ return m_dwCompression; }

protected:
	virtual char ParseChar(LPCSTR &lpData);
	virtual BOOL ParseNumber(LPCSTR &lpData, DWORD &dwResult);
	virtual BOOL ParseString(LPCSTR &lpData, CString &dwResult);
	virtual BOOL IsWhite(char c);
	virtual void SkipWhite(LPCSTR &lpData);
	virtual void NormalizeFileName(CString &dwName);
	
// Implementation
public:
	virtual ~CProgramFile();
};

/////////////////////////////////////////////////////////////////////////////
// CProgramFileList

class CProgramFileList : public CPtrArray
{
// Construction
public:
	CProgramFileList();

// Attributes
protected:

// Operations
public:
	virtual BOOL Init(const CString &csFileName);
	virtual void DeleteAll(void);
	virtual DWORD GetFileSize(DWORD dwFlags, DWORD dwClusterSize) const;

// Implementation
public:
	virtual ~CProgramFileList();
};

/////////////////////////////////////////////////////////////////////////////

#endif
