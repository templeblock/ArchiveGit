/*
// $Workfile: easyprintsview.h $
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
// $Log: /PM8/App/easyprintsview.h $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 4     2/15/99 1:31p Psasse
// m_flScaleFactor and ResetPaperInfo() move to the doc
// 
// 3     2/01/99 1:01a Psasse
// Improved PhotoProjects printing
// 
// 2     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 1     12/23/98 5:43p Psasse
// 
*/

#ifndef __EASYPRINTSVIEW_H__
#define __EASYPRINTSVIEW_H__

/////////////////////////////////////////////////////////////////////////////
// CEasyPrintsView view
class CLabelView;

class CEasyPrintsView : public CLabelView
{
	INHERIT(CEasyPrintsView, CPmwView)

	DECLARE_DYNCREATE(CEasyPrintsView)
protected:
	CEasyPrintsView();			// protected constructor used by dynamic creation

// Attributes
public:
	virtual  void DoPreviewDraw(CDC* pDC);

// Operations
public:

// Implementation
protected:
	int m_scaled_source_x0;
	int m_scaled_source_y0;

	virtual  ~CEasyPrintsView();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual  void OnDraw(PrintContext* pc);
	virtual	void generate_preview_vars(CDC *pDC, CPrintInfo* pInfo);		// overridden to draw this view
	virtual  VOID generate_print_vars(PrintContext *pc, RECT r);

	virtual CPmwPrint* CreatePrintDialog(void);
	
	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

	virtual int GrindOutPrintJob(PrintContext& pc);
	// Generated message map functions
protected:
	//{{AFX_MSG(CEasyPrintsView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
