/*
// $Workfile: WHlpProj.h $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WHlpProj.h $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:47p Hforman
// name change
// 
// 3     5/26/98 3:02p Rlovejoy
// Massive revisions.
// 
// 2     4/16/98 5:58p Hforman
// remove obsolete header include
// 
//    Rev 1.0   14 Aug 1997 15:19:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:26   Fred
// Initial revision.
// 
//    Rev 1.1   29 Apr 1997 10:52:22   doberthur
// Use common buttons.
// 
//    Rev 1.0   21 Apr 1997 18:26:24   doberthur
// Initial revision.
// 
//    
*/

#ifndef __WHPROJ_H__
#define __WHPROJ_H__

#include "WHlpCmn.h"

/////////////////////////////////////////////////////////////////////////////
// CWProjectType dialog

class CWProjectType : public CWHelpCommonPropertyPage
{
// Construction
public:
	CWProjectType();
   CString GetSelProject() { return m_selectedDoc; }
// Dialog Data
	//{{AFX_DATA(CWProjectType)
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWProjectType)
	public:
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   CString m_selectedDoc;
	// Generated message map functions
	//{{AFX_MSG(CWProjectType)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeChoiceList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __WHPROJ_H__
