/*
// $Workfile: SIGNVIEW.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
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
// $Log: /PM8/App/SIGNVIEW.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 6     8/17/98 5:36p Mwilson
// changed Tshirts to be based off of newsletters instead of posters
// 
// 5     6/29/98 2:37p Mwilson
// added gadgets to mulitpage projects
// 
// 4     4/21/98 3:39p Fredf
// Reworked "reverse" printing to apply to all projects.
// 
// 3     4/18/98 4:25p Fredf
// T-Shirt printing.
// 
// 2     4/17/98 4:47p Fredf
// T-Shirts
*/

#ifndef __SIGNVIEW_H__
#define __SIGNVIEW_H__

/////////////////////////////////////////////////////////////////////////////
// CPosterView view

class CPosterView : public CPmwView
{
	DECLARE_DYNCREATE(CPosterView)
protected:
	CPosterView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual ~CPosterView();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view

	// Generated message map functions
protected:
	//{{AFX_MSG(CPosterView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CCertificateView view

class CCertificateView : public CPosterView
{
	DECLARE_DYNCREATE(CCertificateView)
protected:
	CCertificateView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual ~CCertificateView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCertificateView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CFaxCoverView view

class CFaxCoverView : public CPosterView
{
	DECLARE_DYNCREATE(CFaxCoverView)
protected:
	CFaxCoverView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual ~CFaxCoverView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFaxCoverView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CStationeryView view

class CStationeryView : public CPosterView
{
	DECLARE_DYNCREATE(CStationeryView)
protected:
	CStationeryView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual ~CStationeryView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStationeryView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
