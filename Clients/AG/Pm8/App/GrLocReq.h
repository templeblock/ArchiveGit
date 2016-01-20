
//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/GrLocReq.h 1     3/03/99 6:05p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/GrLocReq.h $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 5     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GRLOCREQ_H__
#define __GRLOCREQ_H__

/////////////////////////////////////////////////////////////////////////////
// CGetGraphicsLocsRequest command target

class CGetGraphicsLocsRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CGetGraphicsLocsRequest)

	CGetGraphicsLocsRequest();           // protected constructor used by dynamic creation
	CGetGraphicsLocsRequest(CString& strColl, CString& strItemId);   

// Attributes
public:

protected:
	CString	m_strCollID;
	CString	m_strItemID;
	CString m_strFile;
	CString m_strLoginID;
	CString	m_strPasswordID;

	CString	m_strReturnedItemInfo;

	// Operations
public:

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
	

// Implementation
public:
	virtual ~CGetGraphicsLocsRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);
	void GetDownloadInfo(CString& strString);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
