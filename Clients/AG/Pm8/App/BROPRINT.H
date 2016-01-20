/*
// $Workfile: BROPRINT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BROPRINT.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:46   Fred
// Initial revision.
// 
//    Rev 1.4   08 Apr 1997 16:11:18   Jay
// Print Instructions control
// 
//    Rev 1.3   07 Aug 1996 08:51:40   Jay
//  
// 
//    Rev 1.2   06 Aug 1996 14:36:36   Jay
//  
// 
//    Rev 1.1   06 Aug 1996 11:17:08   Jay
//  
// 
//    Rev 1.0   27 Jul 1996 10:09:26   Jay
// Initial revision.
*/

#ifndef __BROPRINT_H__
#define __BROPRINT_H__

#include "pmwprint.h"
#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CBrochurePrintDialog dialog

class CBrochurePrintDialog : public CPmwPrint
{
// Construction
public:
	CBrochurePrintDialog(CPmwView* pView, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBrochurePrintDialog)
	enum { IDD = IDD_PRINT_BROCHURE };
	int		m_nPrintWhat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrochurePrintDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrochurePrintDialog)
	afx_msg void OnPrintBoth();
	afx_msg void OnPrintBack();
	afx_msg void OnPrintFront();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void SetPages(int nFromPage, int nToPage);
};

#endif
