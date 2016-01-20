//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CATDATA.CPP 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/CATDATA.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 1     12/01/98 9:12a Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//    Rev 1.0   14 Aug 1997 15:21:16   Fred
// Initial revision.
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "catdata.h"
#include "cdeflate.h"

IMPLEMENT_SERIAL(CProjectData, CObject, 1)

CProjectData::CProjectData()
{

}

CProjectData::~CProjectData()
{

}

void CProjectData::SetData(CString csProjType, CString csFilePath)
{
	m_csProjectType = csProjType;
	m_csFilePath = csFilePath;
}

void CProjectData::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_csProjectType;
		ar << m_csFilePath;
	}
	else
	{
		ar >> m_csProjectType;
		ar >> m_csFilePath;
	}
}


IMPLEMENT_SERIAL(CSubCategoryData, CObject, 1)
CSubCategoryData::CSubCategoryData():
	m_pBitmap(NULL)
{

}

CSubCategoryData::~CSubCategoryData()
{
	while(!m_cplProjData.IsEmpty())
	{
		delete m_cplProjData.RemoveHead();
	}
	if(m_pBitmap)
		CDeflate::FreeData(m_pBitmap);
	
}

void CSubCategoryData::SetSubCategoryData(CString csSubCategoryName, CString csBmpPath)
{
	m_csSubCategoryName = csSubCategoryName;
	m_csBmpPath = csBmpPath;
}

void CSubCategoryData::SetProjData(CProjectData* pProjData)
{
	m_cplProjData.AddTail(pProjData);
}

void CSubCategoryData::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_csSubCategoryName;
		CDeflate deflate;
		DWORD dwCompressedSize;
		deflate.Compress(m_csBmpPath, &m_pBitmap, &dwCompressedSize);
		ar << dwCompressedSize;
		ar.Write(m_pBitmap, dwCompressedSize);
		
	}
	else
	{
		ar >> m_csSubCategoryName;
		CDeflate deflate;
		DWORD dwCompressedSize, dwDecompressedSize;
		ar >> dwCompressedSize;
		BYTE* pData = new BYTE[dwCompressedSize];
		ar.Read(pData, dwCompressedSize);
		deflate.Decompress(pData, dwCompressedSize, &m_pBitmap, &dwDecompressedSize);
		delete pData;
		
	}

	m_cplProjData.Serialize(ar);
}

IMPLEMENT_SERIAL(CCatData, CObject, 1)
CCatData::CCatData()
{

}

CCatData::~CCatData()
{
	while(!m_cplSubCatData.IsEmpty())
	{
		delete m_cplSubCatData.RemoveHead();
	}
	
}

void CCatData::AddSubCategory(CSubCategoryData* pSubCategoryData)
{
	m_cplSubCatData.AddTail(pSubCategoryData);
}

void CCatData::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_csCategoryName;
	}
	else
	{
		ar >> m_csCategoryName;
	}

	m_cplSubCatData.Serialize(ar);
}

void CCatData::SetName(CString csCatName)
{
	m_csCategoryName = csCatName;
}
