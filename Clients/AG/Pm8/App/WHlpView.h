/*
// $Workfile: WHlpView.h $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
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
// $Log: /PM8/App/WHlpView.h $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:47p Hforman
// name change
// 
// 13    6/04/98 9:49p Hforman
// remove unused stuff
// 
// 12    6/04/98 7:20p Psasse
// modal helpful hint support
// 
// 11    6/03/98 9:17p Psasse
// Receiver Block support in With Help - labels
// 
// 10    6/03/98 2:26p Psasse
// interim WithHelp Label support
// 
// 9     6/01/98 11:26p Hforman
// change baseclass to CWHelpPropertyPage; other cleanup
// 
// 8     6/01/98 8:24p Hforman
// add m_fReplaceNullString
// 
// 7     5/29/98 6:28p Rlovejoy
// Took out unused variables.
// 
// 6     5/29/98 6:10p Hforman
// replacing user-supplied strings
// 
// 5     5/28/98 8:38p Hforman
// remove title string
// 
// 4     5/27/98 6:58p Rlovejoy
// Uses OnWizardFinish().
// 
// 3     5/26/98 3:02p Rlovejoy
// Massive revisions.
// 
// 2     4/16/98 5:58p Hforman
// remove obsolete header inclue
// 
//    Rev 1.0   14 Aug 1997 15:19:54   Fred
// Initial revision.
// 
*/

#ifndef __WHLPVIEW_H__
#define __WHLPVIEW_H__

#include "WHlpCmn.h"

/////////////////////////////////////////////////////////////////////////////
// CWView dialog

// We need to inherit from CWHelpCommonPropertyPage in order to get
// project preview services.
class CWView : public CWHelpPropertyPage
{
// Construction
public:
	CWView(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

	void StripReturnsFromStr(CString& strToStrip);
	void FindAndReplace(CPmwDoc* m_pDoc, LPCSTR searchText, LPCSTR insertText);
	void FindAndReplace(CPmwDoc* pDoc,       // open doc
                            LPCSTR searchText,  CHARACTER* insertText, int strlength);

// Dialog Data
	//{{AFX_DATA(CWView)
	enum { IDD = IDD_WH_VIEW };
	//}}AFX_DATA

public:
   // incremented for each WH_TMPx file
   static int m_nTmpFileNumber;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWView)
	public:
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


protected:
	// Generated message map functions
	//{{AFX_MSG(CWView)
	virtual BOOL OnInitDialog();
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __WHLPVIEW_H__
