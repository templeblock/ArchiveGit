/*
// $Workfile: LBLVIEW.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LBLVIEW.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     7/24/98 10:29p Jayn
// Post card printing. Label count adjustment for name list.
// 
// 2     3/06/98 3:59p Jayn
// Post card print code (double-sided).
// 
//    Rev 1.3   02 Oct 1997 15:14:30   johno
// Vdiff said they were the same ?!
// 
//    Rev 1.2   26 Sep 1997 16:56:30   johno
// Assed PostCardView
// 
//    Rev 1.1   19 Sep 1997 17:34:58   johno
// Changes for new Post Card project type
// (preliminary)
// 
// 
//    Rev 1.0   14 Aug 1997 15:22:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:28   Fred
// Initial revision.
// 
//    Rev 1.1   24 Mar 1997 10:35:30   Jay
// Print preview for labels and envelopes
// 
//    Rev 1.0   14 Mar 1996 13:38:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:40   FRED
// Initial revision.
// 
//    Rev 1.4   19 Aug 1995 12:04:20   FRED
// Business card print dialogs
// 
//    Rev 1.3   31 Jul 1995 10:26:54   JAY
// New projects.
// 
//    Rev 1.2   21 Jul 1995 17:20:58   JAY
// Label printing
// 
//    Rev 1.1   20 Jul 1995 16:26:38   JAY
//  
// 
//    Rev 1.0   20 Jul 1995 12:52:18   JAY
// Initial revision.
*/

#ifndef __LBLVIEW_H__
#define __LBLVIEW_H__

class CLabelPrintDialog;

/////////////////////////////////////////////////////////////////////////////
// CLabelView view

class CLabelView : public CPmwView
{
	INHERIT(CLabelView, CPmwView)

	DECLARE_DYNCREATE(CLabelView)
protected:
	CLabelView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual ~CLabelView();

	virtual void OnDraw(CDC* pDC);				// overridden to draw this view 
	virtual void OnDraw(PrintContext* pPC);	// overridden to print this view 

	virtual void CreateEditDesktop(void);

	virtual CPmwPrint* CreatePrintDialog(void);

	virtual int GrindOutPrintJob(PrintContext& pc);
	virtual int OnPrintLabels(PrintContext &pc, int nCurrentPage, int nNumPages);
	int PrintLabelsFront(PrintContext &pc);
	int PrintLabelsBack(PrintContext &pc);
	int PrintLabelsForward(PrintContext &pc, int nSide);
	int PrintLabelsBackward(PrintContext &pc, int nSide);
	void SetLabelVariables(PrintContext& pc, int nPage);
	// Generated message map functions
protected:
	//{{AFX_MSG(CLabelView)
	afx_msg void OnLabelType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBizCardView view

class CBizCardView : public CLabelView
{
	INHERIT(CBizCardView, CLabelView)
	DECLARE_DYNCREATE(CBizCardView)
protected:
	CBizCardView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual ~CBizCardView();
	
	virtual CPmwPrint* CreatePrintDialog(void);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CBizCardView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPostCardView view

class CPostCardView : public CLabelView
{
	INHERIT(CPostCardView, CLabelView)
	DECLARE_DYNCREATE(CPostCardView)
protected:
	CPostCardView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

	// Gadget interface.
	virtual BOOL GadgetVisible(UINT uID);
	virtual void GadgetText(UINT uID, CString& csText);
	virtual LPCSTR GadgetMenuName(UINT uID, UINT uPart);

// Implementation
protected:
	virtual ~CPostCardView();
	
	virtual CPmwPrint* CreatePrintDialog(void);
	
	virtual void OnDraw(PrintContext* pPC);	// overridden to print this view 

	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

	// Generated message map functions
protected:
	//{{AFX_MSG(CPostCardView)
	afx_msg void OnViewFront();
	afx_msg void OnUpdateViewFront(CCmdUI* pCmdUI);
	afx_msg void OnViewBack();
	afx_msg void OnUpdateViewBack(CCmdUI* pCmdUI);
	afx_msg void OnPostCardType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
