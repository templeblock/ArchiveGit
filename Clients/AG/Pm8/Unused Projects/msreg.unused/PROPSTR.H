/*
// $Workfile: PROPSTR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/PROPSTR.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
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
