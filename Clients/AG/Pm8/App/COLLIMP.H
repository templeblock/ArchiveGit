//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/COLLIMP.H 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/COLLIMP.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 1     9/22/98 4:03p Jayn
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __COLLIMP_H__
#define __COLLIMP_H__

/*
// The criteria classes for item importing.
*/

class CPMWCollectionImportContext
{
public:
	CPMWCollectionImportContext();
	virtual ~CPMWCollectionImportContext();
	virtual BOOL IsMember(LPCSTR pszFileName) = 0;

	void ImportFromDirectory(void);

protected:
	void ImportContent(void);
	void CountContent(void);

public:
	CString m_csDirectory;
	CStringArray m_csaImport;
	UINT m_uConfirmMessage;
	CPMWCollection* m_pCollection;
};

class CPMWProjectImportContext : public CPMWCollectionImportContext
{
public:
	CPMWProjectImportContext();
	virtual BOOL IsMember(LPCSTR pszFileName);
};

class CPMWArtImportContext : public CPMWCollectionImportContext
{
public:
	CPMWArtImportContext();
	virtual BOOL IsMember(LPCSTR pszFileName);
};

#endif
