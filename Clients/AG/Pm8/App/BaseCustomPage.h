//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/BaseCustomPage.h 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/BaseCustomPage.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 6     3/02/99 3:41p Lwu
// changed to use the apply's page components
// 
// 5     2/19/99 12:32p Mwilson
// modified constructor to take page name
// 
// 4     2/16/99 5:29p Mwilson
// added scaling for tiles
// 
// 3     2/11/99 12:05p Lwu
// handle preTranslate message and message handler for mouse moves
// 
// 2     2/11/99 11:37a Mwilson
// Updated
// 
// 1     2/10/99 3:07p Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __BASECUSTOMPAGE_H__
#define __BASECUSTOMPAGE_H__

#include "resource.h"
#include "bitmapimage.h"
#include "dbbld.h"
#include "incompdb.h"

class CPControlInfo;
class CPImageControl;
typedef CTypedPtrList<CObList, CPImageControl*> ImgCtrlList;
typedef CTypedPtrList<CObList, CWnd*> WndCtrlList;
typedef CTypedPtrList<CObList, CPControlInfo*> CtrlInfoList;

class CPBaseCustomPage : public CDialog
{
public:
   CPBaseCustomPage(CString csPageName, CWnd* pParent = NULL);
	~CPBaseCustomPage();
	BOOL ChangeResolution() ;

// Dialog Data
   //{{AFX_DATA(CPBaseCustomPage)
	enum { IDD = IDD_EMPTY_DLG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPBaseCustomPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   //{{AFX_MSG(CPBaseCustomPage)
   virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize( UINT nType, int cx, int cy );
   //}}AFX_MSG
	afx_msg LRESULT OnItemMouseMove( WPARAM wParam, LPARAM lParam );
   DECLARE_MESSAGE_MAP()
	
	CPUIImageComponentManagerIndexedDB* m_pAppImageManager;

	ImgCtrlList m_listImageControls;
	WndCtrlList m_listControls;
	
	CPCompPage  m_Page;
	CPImageControl* m_pImageControl;
private:
	void ScaleRect(CRect& rcScale, BOOL bOffset = TRUE);
	CSize m_czDesignedRes;
	CSize m_czCurRes;
	CString m_csPageName;
	float m_fXScaleFactor;
	float m_fYScaleFactor;

	CWnd*		m_pWndMouseEntered;
	void ValidatePoint(UINT message, POINT pt);

};

#endif
