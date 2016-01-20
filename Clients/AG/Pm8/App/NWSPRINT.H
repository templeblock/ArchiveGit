/*
// $Workfile: NWSPRINT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/NWSPRINT.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 3     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 2     8/17/98 5:37p Mwilson
// added special dialog for printing tshirts
// 
//    Rev 1.0   14 Aug 1997 15:23:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:48   Fred
// Initial revision.
// 
//    Rev 1.4   08 Apr 1997 16:11:18   Jay
// Print Instructions control
// 
//    Rev 1.3   06 Aug 1996 14:36:36   Jay
//  
// 
//    Rev 1.2   06 Aug 1996 11:17:06   Jay
//  
// 
//    Rev 1.1   27 Jul 1996 10:09:38   Jay
// Fixed #ifndef sandwich
// 
//    Rev 1.0   24 Jul 1996 07:54:08   Jay
// Initial revision.
*/

#ifndef __NWSPRINT_H__
#define __NWSPRINT_H__

#include "pmwprint.h"
#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNewsletterPrintDialog dialog

class CNewsletterPrintDialog : public CPmwPrint
{
// Construction
public:
	CNewsletterPrintDialog(CPmwView* pView, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNewsletterPrintDialog)
	enum { IDD = IDD_PRINT_NEWSLETTER };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewsletterPrintDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewsletterPrintDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CNewsletterPrintDialog dialog

class CTShirtPrintDialog : public CNewsletterPrintDialog
{
// Construction
public:
	CTShirtPrintDialog(CPmwView* pView, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTShirtPrintDialog)
	enum { IDD = IDD_PRINT_NEWSLETTER };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTShirtPrintDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTShirtPrintDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
