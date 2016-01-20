/*
// $Workfile: ENVVIEW.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVVIEW.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:46   Fred
// Initial revision.
// 
//    Rev 1.1   24 Mar 1997 10:35:28   Jay
// Print preview for labels and envelopes
// 
//    Rev 1.0   14 Mar 1996 13:37:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:08   FRED
// Initial revision.
// 
//    Rev 1.3   03 Oct 1995 10:03:30   JAY
// Label/envelope printing fixes
// 
//    Rev 1.2   16 Aug 1995 18:02:36   JAY
// Envelope type.
// 
//    Rev 1.1   28 Jul 1995 12:31:22   JAY
// Printer info. Envelope printing.
// 
//    Rev 1.0   20 Jul 1995 12:52:20   JAY
// Initial revision.
*/

#ifndef __ENVVIEW_H__
#define __ENVVIEW_H__

/////////////////////////////////////////////////////////////////////////////
// CEnvelopeView view

class CEnvelopeView : public CPmwView
{
	DECLARE_DYNCREATE(CEnvelopeView)
protected:
	CEnvelopeView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual ~CEnvelopeView();

	virtual CPmwPrint* CreatePrintDialog(void);

	virtual void OnDraw(CDC* pPC);  // overridden to print this view 

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnvelopeView)
	afx_msg void OnEnvelopeType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void FlipProject(void);
};

/////////////////////////////////////////////////////////////////////////////

#endif
