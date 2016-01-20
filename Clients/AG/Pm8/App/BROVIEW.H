/*
// $Workfile: BROVIEW.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BROVIEW.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:46   Fred
// Initial revision.
// 
//    Rev 1.5   06 Aug 1996 11:17:08   Jay
//  
// 
//    Rev 1.4   28 Jul 1996 13:37:50   Jay
//  
// 
//    Rev 1.3   28 Jul 1996 11:28:40   Jay
// Brochure stuff
// 
//    Rev 1.2   27 Jul 1996 10:10:08   Jay
// New brochure print dialog.
// 
//    Rev 1.1   25 Jul 1996 11:16:58   Jay
// More page stuff
// 
//    Rev 1.0   23 Jul 1996 10:39:36   Jay
// Initial revision.
*/

#ifndef __BROVIEW_H__
#define __BROVIEW_H__

/////////////////////////////////////////////////////////////////////////////
// CBrochureView view

class CBrochureView : public CPmwView
{
	DECLARE_DYNCREATE(CBrochureView)
protected:
	CBrochureView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

public:
	// Gadget interface.
	virtual BOOL GadgetVisible(UINT uID);
	virtual void GadgetText(UINT uID, CString& csText);
	virtual LPCSTR GadgetMenuName(UINT uID, UINT uPart);

// Implementation
protected:
	virtual ~CBrochureView();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view

	virtual CPmwPrint* CreatePrintDialog(void);
	virtual void CreateEditDesktop(void);

	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrochureView)
	afx_msg void OnViewFront();
	afx_msg void OnUpdateViewFront(CCmdUI* pCmdUI);
	afx_msg void OnViewBack();
	afx_msg void OnUpdateViewBack(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
