/*
// $Workfile: easyprintsdoc.h $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
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
// $Log: /PM8/App/easyprintsdoc.h $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 4     2/15/99 1:32p Psasse
// m_flScaleFactor, DrawPreview() and PreviewBound() for Thumbs
// 
// 3     1/30/99 11:31p Psasse
// Updated and fairly stable EasyPrints code
// 
// 2     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 1     12/23/98 5:43p Psasse
// 
*/

#ifndef __EASYPRINTSDOC_H__
#define __EASYPRINTSDOC_H__

/////////////////////////////////////////////////////////////////////////////
// CEasyPrintsDoc document
#include "photoprj.h"

class CLabelDoc;

class CEasyPrintsDoc : public CLabelDoc
{
	DECLARE_SERIAL(CEasyPrintsDoc)
protected:
	CEasyPrintsDoc();			// protected constructor used by dynamic creation

	int m_nCurrentSubArea;

	int m_nType;

	float m_flScaleFactor;

// Attributes
public:

	static ProjectInfo StandardInfo;
// Operations
public:
	CPaperInfo* GetPaperInfo();
	CPaperInfo* GetPaperInfo(int x);
	void ResetPaperInfo(CPaperInfo* pPaperInfo);

	int GetTotalSlots (void);
	virtual void SizeToInternalPaper(void);

	int GetCurrentSubArea(void) const
	{  return m_nCurrentSubArea;  }

	void  SetCurrentSubArea(const int x)
	{   m_nCurrentSubArea = x;  }

	int GetType(void) const
	{  return m_nType;  }

	void  SetType(const int x)
	{   m_nType = x;  }

	float GetScaleFactor(void) const
	{  return m_flScaleFactor;  }

	void  SetScaleFactor(const float x)
	{   m_flScaleFactor = x;  }

	void FillInSubAreaInfo(void);

	CPhotoPrjData* GetPhotoProjectInfo(void)
	{	return m_pPhotoProjectInfo; }

// Implementation
protected:
	virtual ~CEasyPrintsDoc();
	virtual	BOOL OnNewDocument();
	virtual BOOL OnOpenDocument( LPCTSTR lpszPathName );
	virtual BOOL DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc);

	// Get the preview bound.
	virtual void GetPreviewBound(PBOX* pBound);

	CPhotoPrjList m_List;
	CPhotoPrjData*		m_pPhotoProjectInfo;

	// Generated message map functions
protected:
	//{{AFX_MSG(CEasyPrintsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif
