//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/InCompdb.h 1     3/03/99 6:06p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/InCompdb.h $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 1     2/05/99 8:51a Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __INCOMPDB_H__
#define __INCOMPDB_H__

#include "dbbld.h"
#include "cindex.h"
#include "UIIMage.h"

class CPUIPageComponentManager
{
public:
// Construction
   CPUIPageComponentManager();
   virtual ~CPUIPageComponentManager();

// Operations
   int LoadPage(CPCompPage &refPage, LPCSTR szPageName);

protected:
	int Init();

	StorageFile			m_storageFile;
	CCIndexFile			m_cindexFile;
	CSortedDatabaseObject		m_componentDB;
	CCIndexIndexList	m_cindexList;
	BOOL					m_bInited;

// Data Members
private:
};

class CPUIImageComponentManager
{
public:
// Construction
   CPUIImageComponentManager(){}
   virtual ~CPUIImageComponentManager() {}

// Operations
   virtual int LoadResource(CPUIImageComponentDataEntry &refImage, LPCSTR szFileName)=0;
   virtual int LoadResource(CPUIImageComponentDataEntry &refImage, UINT uiID)=0;

protected:


// Data Members
private:
};

class CPUIImageComponentManagerIndexedDB : public CPUIImageComponentManager
{
public:
// Construction
   CPUIImageComponentManagerIndexedDB();
   virtual ~CPUIImageComponentManagerIndexedDB();

// Operations
   virtual int LoadResource(CPUIImageComponentDataEntry &refImage, LPCSTR szFileName);
   virtual int LoadResource(CPUIImageComponentDataEntry &refImage, UINT uiID);
protected:
	int Init();
	StorageFile			m_storageFile;
	CCIndexFile			m_cindexFile;
	CSortedDatabaseObject		m_componentDB;
	CCIndexIndexList	m_cindexList;
	BOOL					m_bInited;

};

class CPUIImageComponentManagerStdResource : public CPUIImageComponentManager
{
public:
// Construction
   CPUIImageComponentManagerStdResource() {}
   virtual ~CPUIImageComponentManagerStdResource() {}

// Operations
   virtual int LoadResource(CPUIImageComponentDataEntry &refImage, LPCSTR szFileName);
   virtual int LoadResource(CPUIImageComponentDataEntry &refImage, UINT uiID);
   private:
};

#endif
