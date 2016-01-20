/*
// $Workfile: PROPSTR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PROPSTR.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:56   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:41:04   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:32   JAY
// Initial revision.
// 
//    Rev 1.0   22 Nov 1995 08:51:52   FRED
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:42   FRED
//  
*/ 

class CMlsPropertyString;

#ifndef __PROPSTR_H__
#define __PROPSTR_H__

class CMlsPropertyString
{
public:
	CMlsPropertyString();
	virtual ~CMlsPropertyString();

public:
	virtual BOOL ExtractProperties(CString& csString);
	virtual BOOL SetProperties(const CString& csString);
	virtual int Count(void);
	virtual BOOL AsString(int nIndex, CString& csString, LPCSTR pszDefault = NULL);
	virtual BOOL AsInt(int nIndex, int& nInteger, int nDefault = -1);
	virtual BOOL AsColor(int nIndex, COLORREF& clColor, COLORREF clDefault = RGB(0,0,0));

protected:
	CStringArray m_PropertyStrings;
};

#endif
