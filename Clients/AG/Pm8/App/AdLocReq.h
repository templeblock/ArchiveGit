//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/AdLocReq.h 1     3/03/99 6:02p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/AdLocReq.h $
// 
// 1     3/03/99 6:02p Gbeddow
// 
// 18    5/26/98 6:37p Psasse
// Helpful hints sound support
// 
// 17    5/20/98 9:19p Psasse
// Support for larger bitmaps in Helpful hints window and asynchronous
// progress control becomes more like IE
// 
// 16    5/18/98 7:40p Psasse
// asynchronous progress dialog support
// 
// 15    5/16/98 9:20p Psasse
// 
// 14    5/15/98 8:34p Psasse
// Asynchronous progress dialog support
// 
// 13    5/11/98 9:07p Psasse
// 
// 12    5/09/98 9:08p Psasse
// 
// 11    5/07/98 9:22p Psasse
// Minor syntax fixes to OnLine Server code
// 
// 10    5/06/98 7:24p Psasse
// Added support for basic authentication for data and graphic servers
// 
// 9     5/01/98 6:25p Psasse
// 
// 8     4/30/98 7:18p Psasse
// 
// 7     4/29/98 9:47p Psasse
// 
// 6     4/28/98 9:11p Psasse
// 
// 5     4/27/98 9:17p Psasse
// 
// 4     4/25/98 7:54p Psasse
// 
// 3     4/24/98 10:50a Hforman
// added standard header and #ifdef so file only gets included once
// 
//////////////////////////////////////////////////////////////////////////////
#ifndef __ADLOCREQ_H__
#define __ADLOCREQ_H__

class CBasicRequestInfo;

class CGetAdInfoRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CGetAdInfoRequest)

public:
	CGetAdInfoRequest();           // protected constructor used by dynamic creation
	CGetAdInfoRequest(CString& strItemId);   
	virtual ~CGetAdInfoRequest();

protected:
	CString m_strAdId;
	CString m_strFileName;
	CString m_strLocale;
	CString m_strDescription;
	CString m_strURLLink;

public:
	CString& GetAdID ( void )
	{  return m_strAdId;}
	void SetAdID ( const CString& NewID )
	{  m_strAdId = NewID;}

	CString& GetFileName ( void )
	{  return m_strFileName;}
	void SetFileName ( const CString& NewID )
	{  m_strFileName = NewID;}

	CString& GetLocale ( void )
	{  return m_strLocale;}
	void SetLocale ( const CString& NewID )
	{  m_strLocale = NewID;}

	CString& GetDescription ( void )
	{  return m_strDescription;}
	void SetDescription ( const CString& NewID )
	{  m_strDescription = NewID;}

	CString& GetURLLink ( void )
	{  return m_strURLLink;}
	void SetURLLink ( const CString& NewID )
	{  m_strURLLink = NewID;}
};


/////////////////////////////////////////////////////////////////////////////
// CGetAdvtLocsListRequest command target

class CGetAdvtLocsListRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CGetAdvtLocsListRequest)

	CGetAdvtLocsListRequest();           // protected constructor used by dynamic creation
	CGetAdvtLocsListRequest(CString& strItemId);   

// Attributes
public:

protected:
	CStringList m_strAdInfoList;
	CString m_strSelector;
	CString m_strIgnoreLocale;

	CGetAdInfoRequest* m_pAdInfo;
	// Operations
public:

	
	CStringList& GetAdInfoList (void)
	{  return m_strAdInfoList;}

// Implementation
public:
	virtual ~CGetAdvtLocsListRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CGetAdvtLocRequest command target

class CGetAdvtLocRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CGetAdvtLocRequest)

	CGetAdvtLocRequest();           // protected constructor used by dynamic creation
	CGetAdvtLocRequest(CString& strItemId);   

// Attributes
public:

protected:
	CStringList m_strLocationsList;
	CString m_strAdID;

	CString m_strItemID;
	CString m_strDescription;
	CString m_strURLLink;

	// Operations
public:
	
	CStringList& GetLocationsList (void)
	{  return m_strLocationsList;}

// Implementation
public:
	virtual ~CGetAdvtLocRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMarkAdvtSeenRequest command target

class CMarkAdvtSeenRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CMarkAdvtSeenRequest)

	CMarkAdvtSeenRequest();           // protected constructor used by dynamic creation
	CMarkAdvtSeenRequest(CString& strItemId);   

// Attributes
public:

protected:
	CString m_strAdID;

	// Operations
public:
	
// Implementation
public:
	virtual ~CMarkAdvtSeenRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __ADLOCREQ_H__
