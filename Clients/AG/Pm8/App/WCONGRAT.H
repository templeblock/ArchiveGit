/*
// $Workfile: WCONGRAT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WCONGRAT.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:37:04   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:36:48   JAY
// Reverted. New series.
// 
//    Rev 1.0   22 Jun 1994 18:55:58   FRED
// Initial revision.
*/ 

#ifndef _WCONGRAT_H_
#define _WCONGRAT_H_

/////////////////////////////////////////////////////////////////////////////
// CWorkstationCongratulationsDialog dialog

class CWorkstationCongratulationsDialog : public CDialog
{
// Construction
public:
	CWorkstationCongratulationsDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWorkstationCongratulationsDialog)
	enum { IDD = IDD_INSTALL_CONGRATULATIONS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CWorkstationCongratulationsDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	virtual void FreeAllocatedData(void) {}
};

#endif

