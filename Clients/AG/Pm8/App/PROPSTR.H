/*
// $Workfile: PROPSTR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROPSTR.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     2/18/99 4:44p Jfleischhauer
// added AsResourceIDFromString()
// 
//    Rev 1.0   14 Aug 1997 15:25:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:40   Fred
// Initial revision.
// 
//    Rev 1.0   05 Jun 1996 08:54:36   Fred
// Initial revision.
// 
//    Rev 1.0   19 Dec 1995 15:29:08   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:44   JAY
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
	virtual BOOL AsString(int nIndex, CString& csString, LPCSTR pszDefault = NULL, BOOL fUppercase = FALSE);
	virtual BOOL AsInt(int nIndex, int& nInteger, int nDefault = -1);
	virtual BOOL AsColor(int nIndex, COLORREF& clColor, COLORREF clDefault = RGB(0,0,0));
	virtual BOOL AsResourceIDFromString(int nIndex, int *nID, int nDefault = -1);

protected:
	CStringArray m_PropertyStrings;
};

#endif
