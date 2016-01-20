//////////////////////////////////////////////////////////////////////////////
// $CATDATA: /PMW/CATDATA.H 2     5/31/98 5:19p Hforman $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/CATDATA.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 1     12/01/98 9:12a Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CATDATA_H__
#define __CATDATA_H__

#include "afxtempl.h"


class CProjectData : public CObject
{
public:
	DECLARE_SERIAL(CProjectData);

	CProjectData();
	~CProjectData();
	void SetData(CString csProjType, CString csFilePath);
	void Serialize(CArchive& ar);

	CString m_csFilePath;
	CString m_csProjectType;

};

class CSubCategoryData : public CObject
{
public:
	DECLARE_SERIAL(CSubCategoryData);

	CSubCategoryData();
	~CSubCategoryData();
	void SetSubCategoryData(CString csSubCategoryName, CString csBmpPath);
	void SetProjData(CProjectData* pProjData);
	void Serialize(CArchive& ar);

	CTypedPtrList<CObList, CProjectData*> m_cplProjData;
	CString m_csSubCategoryName;
	CString m_csBmpPath;

	BYTE* m_pBitmap;
};


class CCatData : public CObject
{
public:
	DECLARE_SERIAL(CCatData);

	CCatData();
	~CCatData();
	void SetName(CString csCatName);
	void AddSubCategory(CSubCategoryData* pSubCategoryData);
	void Serialize(CArchive& ar);

	CTypedPtrList<CObList, CSubCategoryData*> m_cplSubCatData;
	CString m_csCategoryName;
};


#endif
