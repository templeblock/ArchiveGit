//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/LoginReq.h 1     3/03/99 6:07p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/LoginReq.h $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 24    9/15/98 4:20p Psasse
// Better error checking upon server failure
// 
// 23    7/26/98 2:21p Psasse
// Creating a new account while logged in now rezeroes the Graphic Server
// List
// 
// 22    5/26/98 6:37p Psasse
// Helpful hints sound support
// 
// 21    5/20/98 9:19p Psasse
// Support for larger bitmaps in Helpful hints window and asynchronous
// progress control becomes more like IE
// 
// 20    5/18/98 7:40p Psasse
// asynchronous progress dialog support
// 
// 19    5/16/98 9:22p Psasse
// 
// 18    5/15/98 8:34p Psasse
// Asynchronous progress dialog support
// 
// 17    5/11/98 9:07p Psasse
// 
// 16    5/09/98 9:08p Psasse
// 
// 15    5/07/98 9:22p Psasse
// Minor syntax fixes to OnLine Server code
// 
// 14    5/06/98 7:24p Psasse
// Added support for basic authentication for data and graphic servers
// 
// 13    5/01/98 6:25p Psasse
// 
// 12    4/30/98 7:18p Psasse
// 
// 11    4/29/98 9:47p Psasse
// 
// 10    4/28/98 9:11p Psasse
// 
// 9     4/27/98 9:17p Psasse
// 
// 8     4/25/98 7:55p Psasse
// 
// 7     4/24/98 10:53a Hforman
// added AdLocReq.h include, added standard header and #ifdef so file only
// gets included once.
// 
//////////////////////////////////////////////////////////////////////////////
#ifndef __LOGINREQ_H__
#define __LOGINREQ_H__

#include "AdLocReq.h"

class CBasicRequestInfo;
class CGetGraphicsLocsRequest;
class CGetAdInfoRequest;

/////////////////////////////////////////////////////////////////////////////
// CLoginRequest command target

class CLoginRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CLoginRequest)

	CLoginRequest();           // protected constructor used by dynamic creation

// Attributes
public:

protected:
	CString	m_strItemID;
	CString	m_strCollID;
	CString m_strFile;
	CString m_strLoginID;
	CString m_strPasswordID;
	CString m_strSessionID;

	CString	m_strReturnedItemInfo;
	CString m_strAdInfo;
	CString m_strCollInfo;

	CStringList m_strServerList;
	CStringList m_strGraphicLocationList;
	CGetGraphicsLocsRequest m_GetGraphicsLocsRequest;
	CStringList m_strShipMethodsList;
	CStringList m_strCreditCardTypesList;

	CGetAdInfoRequest m_AdInfo;
// Operations
public:

	void Initialize ( void );

	CString& GetSessionID ( void )
	{  return m_strSessionID;}
	void SetSessionID ( const CString& NewID )
	{  m_strSessionID = NewID;}
	
	CString& GetCollId (void)
	{  return m_strCollID;}
	void SetCollId (const CString& NewString)
	{  m_strCollID = NewString;}
	
	CString& GetItemId (void)
	{  return m_strItemID;}
	void SetItemId (const CString& NewString)
	{  m_strItemID = NewString;}
	
	CString& GetFile (void)
	{  return m_strFile;}
	void SetFile (const CString& NewString)
	{  m_strFile = NewString;}
	
	CString& GetLoginID (void)
	{  return m_strLoginID;}
	void SetLoginID (const CString& NewString)
	{  m_strLoginID = NewString;}
	
	CString& GetPasswordID (void)
	{  return m_strPasswordID;}
	void SetPasswordID (const CString& NewString)
	{  m_strPasswordID = NewString;}
	
	CStringList& GetServerList (void)
	{  return m_strServerList;}

	CStringList& GetGraphicServerList (void)
	{  return m_strGraphicLocationList;}

	CStringList& GetShipMethodsList (void)
	{  return m_strShipMethodsList;}

	CStringList& GetCreditCardTypesList (void)
	{  return m_strCreditCardTypesList;}

	CString& GetCollInfo (void)
	{  return m_strCollInfo;}

	CGetAdInfoRequest* GetAdInfo (void)
	{  return &m_AdInfo;}

// Implementation
public:
	virtual ~CLoginRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);
	void GetDownloadInfo(CString& strString);
	void FillAdParams(CString& strList, const char* seps);

	//UINT DoRequest(void);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __LOGINREQ_H__
