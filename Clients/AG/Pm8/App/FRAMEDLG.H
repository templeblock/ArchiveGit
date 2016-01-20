/*
// $Header: /PM8/App/FRAMEDLG.H 1     3/03/99 6:05p Gbeddow $
//
// Frame attributes dialog
//
// $Log: /PM8/App/FRAMEDLG.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:56   Fred
// Initial revision.
// 
//    Rev 1.2   06 Jan 1997 10:15:20   Fred
// Start of new text object
// 
//    Rev 1.1   24 Jun 1996 18:06:52   Jay
//  
// 
//    Rev 1.0   21 Jun 1996 17:50:42   Jay
//  
*/

#ifndef __FRAMEDLG_H__
#define __FRAMEDLG_H__

#include "warpsel.h"
#include "warpfile.h"
#include "pmwdlg.h"
#include "text.h"

struct CFrameAttributes;

/////////////////////////////////////////////////////////////////////////////
// CFrameAttributesDialog dialog

class CFrameAttributesDialog : public CPmwDialog
{
// Construction
public:
	CFrameAttributesDialog(const CFrameAttributes* pAttributes, CWnd* pParent = NULL);   // standard constructor

	void GetExitAttributes(CFrameAttributes* pAttributes);

// Dialog Data
	//{{AFX_DATA(CFrameAttributesDialog)
	enum { IDD = IDD_FRAME_ATTRIBUTES };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameAttributesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	int m_nVerticalAlignment;

	int m_nColumns;
	BOOL m_fColumns;

	double m_dLeftMargin;
	BOOL m_fLeftMargin;
	double m_dTopMargin;
	BOOL m_fTopMargin;
	double m_dRightMargin;
	BOOL m_fRightMargin;
	double m_dBottomMargin;
	BOOL m_fBottomMargin;

	double m_dColumnSpacing;
	BOOL m_fColumnSpacing;

	// Generated message map functions
	//{{AFX_MSG(CFrameAttributesDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CWarpSelectListBox m_WarpShape;
	CWarpFile m_WarpFile;
	CWarpField* m_pWarpField;
};

#endif					// #ifndef __FRAMEDLG_H__
