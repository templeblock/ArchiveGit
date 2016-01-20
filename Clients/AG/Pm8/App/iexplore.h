/*
// $Workfile: iexplore.h $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/iexplore.h $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     3/26/98 10:29a Fredf
// Changes to install program to install Internet Explorer 4.0 and the Web
// Publishing Wizard.
*/

#ifndef __IEXPLORE_H__
#define __IEXPLORE_H__

///////////////////////////////////////////////////////////////////////////
// CInternetExplorerVersion

class CInternetExplorerVersion
{
public:
	CInternetExplorerVersion();
	~CInternetExplorerVersion();

public:
	BOOL IsValid(void) const
		{ return m_uMajorVersion != 0; }
	void Invalidate(void);

public:
	UINT GetMajorVersion(void) const
		{ return m_uMajorVersion; }
	UINT GetMinorVersion(void) const
		{ return m_uMinorVersion; }
	UINT GetBuild(void) const
		{ return m_uBuild; }
	UINT GetSubBuild(void) const
		{ return m_uSubBuild; }

	CString GetVersionString(void);

public:
	void SetMajorVersion(UINT uMajorVersion)
		{ m_uMajorVersion = uMajorVersion; }
	void SetMinorVersion(UINT uMinorVersion)
		{ m_uMinorVersion = uMinorVersion; }
	void SetBuild(UINT uBuild)
		{ m_uBuild = uBuild; }
	void SetSubBuild(UINT uSubBuild)
		{ m_uSubBuild = uSubBuild; }

public:
	BOOL IsVersion3(void) const;
	BOOL IsVersion4(void) const;

protected:
	UINT m_uMajorVersion;
	UINT m_uMinorVersion;
	UINT m_uBuild;
	UINT m_uSubBuild;
};

///////////////////////////////////////////////////////////////////////////
// CInternetExplorerHelper

class CInternetExplorerHelper
{
public:
	CInternetExplorerHelper();
	virtual ~CInternetExplorerHelper();

public:
	const CInternetExplorerVersion& GetVersion(BOOL fRefresh = FALSE);

protected:
	CInternetExplorerVersion m_Version;
};

#endif
