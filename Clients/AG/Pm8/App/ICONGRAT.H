/*
// $Workfile: ICONGRAT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ICONGRAT.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:08   Fred
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 07:49:26   Fred
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:34   JAY
// Reverted. New series.
// 
//    Rev 1.0   16 Jun 1994 18:16:30   FRED
// Initial revision.
*/ 

#ifndef _ICONGRAT_H_
#define _ICONGRAT_H_

#include "pmwdlg.h"
#include "installr.h"

/////////////////////////////////////////////////////////////////////////////
// CInstallCongratulationsDialog dialog

class CInstallCongratulationsDialog : public CPmwDialog
{
// Construction
public:
	CInstallCongratulationsDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInstallCongratulationsDialog)
	enum { IDD = IDD_INSTALL_CONGRATULATIONS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInstallCongratulationsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	virtual void FreeAllocatedData(void) {}
};

#endif

