// expirat.h : header file
//
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/EXPIRAT.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:20   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:36   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CExpirationDateDialog dialog

class CExpirationDateDialog : public CBaseDialog
{
// Construction
public:
	CExpirationDateDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CExpirationDateDialog)
	enum { IDD = IDD_EXPIRATION_DATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CExpirationDateDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
