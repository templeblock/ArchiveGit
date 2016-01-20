// ordfail.h : header file
//
 /*
// Revision History:
//
// $Log: /PM8/App/LICDLL/ORDFAIL.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:32   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:50   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// COrderFailedDialog dialog

class COrderFailedDialog : public CBaseDialog
{
// Construction
public:
	COrderFailedDialog(LPCSTR pszReason, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(COrderFailedDialog)
	enum { IDD = IDD_ORDER_FAILED };
	CString	m_csReason;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(COrderFailedDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
