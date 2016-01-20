/*
// $Workfile: WCONFIRM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WCONFIRM.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:37:02   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:36:48   JAY
// Reverted. New series.
// 
//    Rev 1.1   23 Jun 1994 11:45:12   FRED
// First testing version.
// 
// 
//    Rev 1.0   22 Jun 1994 18:56:00   FRED
// Initial revision.
*/ 

#ifndef _WCONFIRM_H_
#define _WCONFIRM_H_

/////////////////////////////////////////////////////////////////////////////
// CWorkstationConfirmDialog dialog

class CWorkstationConfirmDialog : public CDialog
{
// Construction
public:
	CWorkstationConfirmDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWorkstationConfirmDialog)
	enum { IDD = IDD_INSTALL_CONFIRM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CWorkstationConfirmDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
