//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/InCompdb.CPP 2     3/04/99 9:37a Lwu $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/InCompdb.CPP $
// 
// 2     3/04/99 9:37a Lwu
// set init flag
// 
// 2     3/04/99 9:30a Lwu
// set init flag to true at init function
// 
// 1     2/05/99 8:51a Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//    Rev 1.0   14 Aug 1997 15:21:16   Fred
// Initial revision.
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "InCompdb.h"
#include "pathmgr.h"

CPUIPageComponentManager::CPUIPageComponentManager():
	m_componentDB(&m_cindexFile,&m_storageFile),
	m_bInited(FALSE)
{
}

CPUIPageComponentManager::~CPUIPageComponentManager()
{

}

int CPUIPageComponentManager::LoadPage(CPCompPage &refPage, LPCSTR szPageName)
{
	int nRetVal;
	TRY
	{
		if(!m_bInited)
			Init();

		CPUIImageComponentDirEntry		componentDirEntry;
		CPUIPageComponentDataEntry		componentDataEntry;

		componentDirEntry.SetKey(szPageName);
		// Note that Find() will only locate data and fill in ComponentDirEntry object
		// Subsequent Read will fill in componentDataEntry object
		nRetVal = m_componentDB.Find(&componentDirEntry,&componentDataEntry);
		if(nRetVal == ERRORCODE_None)
		{
			// Read image data from database into componentDataEntry object
			nRetVal = m_componentDB.Read();
			if(nRetVal == ERRORCODE_None)
			{
				CMemFile mf;
				mf.Attach((unsigned char *)componentDataEntry.GetData(), componentDataEntry.GetDataSize());
				CArchive ar(&mf, CArchive::load);
				refPage.Serialize(ar);
				ar.Close();
				mf.Detach();
			}
		}
	}
	CATCH_ALL(e)
	{
		return ERRORCODE_Fail;
	}
	END_CATCH_ALL;
		
	return nRetVal;
}

int CPUIPageComponentManager::Init()
{
	int nRetVal;

	IPathManager* pPathManager = GetGlobalPathManager();

	CString csIndexFilePath = 	pPathManager->ExpandPath("[[S]]\\PageLayout.IDX");
	CString csDataFilePath = pPathManager->ExpandPath("[[S]]\\PageLayout.DB");

	nRetVal = m_cindexFile.Open( csIndexFilePath, &m_cindexList, CCIndexFile::modeRead);
	if(nRetVal != CCIndexFile::success)
	{
		AfxThrowUserException();
	}
	// Create User Interface Component Database
	m_storageFile.set_name(csDataFilePath);

	m_bInited = TRUE;
	return nRetVal;

}





//
//	class CPUIImageComponentManagerIndexedDB
//
//////////////////////////////////////////
CPUIImageComponentManagerIndexedDB::CPUIImageComponentManagerIndexedDB():
	m_componentDB(&m_cindexFile,&m_storageFile),
	m_bInited(FALSE)
{

}

CPUIImageComponentManagerIndexedDB::~CPUIImageComponentManagerIndexedDB()
{

}

int CPUIImageComponentManagerIndexedDB::LoadResource(CPUIImageComponentDataEntry& refImage, UINT uiID)
{
	ASSERT(0);
   return ERRORCODE_Fail;
}

int CPUIImageComponentManagerIndexedDB::LoadResource(CPUIImageComponentDataEntry& refImage, LPCSTR szFileName)
{
	int nRetVal;
	TRY
	{
		if(!m_bInited)
			Init();

		CPUIImageComponentDirEntry		componentDirEntry;

		componentDirEntry.SetKey(szFileName);
		// Note that Find() will only locate data and fill in ComponentDirEntry object
		// Subsequent Read will fill in componentDataEntry object
		nRetVal = m_componentDB.Find(&componentDirEntry,&refImage);
		if(nRetVal == ERRORCODE_None)
		{
			// Read image data from database into componentDataEntry object
			nRetVal = m_componentDB.Read();
			if(nRetVal != ERRORCODE_None)
			{
				refImage.FreeData();
			}
		}
	}
	CATCH_ALL(e)
	{
		return ERRORCODE_Fail;
	}
	END_CATCH_ALL;
		
	return nRetVal;
}

int CPUIImageComponentManagerIndexedDB::Init()
{
	int nRetVal;
	IPathManager* pPathManager = GetGlobalPathManager();

	CString csIndexFilePath = 	pPathManager->ExpandPath("[[S]]\\TestColl.IDX");
	CString csDataFilePath = pPathManager->ExpandPath("[[S]]\\TestColl.DB");

	nRetVal = m_cindexFile.Open( csIndexFilePath, &m_cindexList, CCIndexFile::modeRead);
	if(nRetVal != CCIndexFile::success)
	{
		AfxThrowUserException();
	}
	// Create User Interface Component Database
	m_storageFile.set_name(csDataFilePath);
	m_bInited = TRUE;
	return nRetVal;

}

