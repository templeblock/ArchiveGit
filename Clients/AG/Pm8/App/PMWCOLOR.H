/*
// $Workfile: PMWCOLOR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWCOLOR.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:14   Fred
// Initial revision.
// 
//    Rev 1.5   11 Jun 1997 11:41:46   Jay
// 
//    Rev 1.4   10 Jun 1997 15:29:48   Jay
// New color dialog; saves custom colors.
// 
//    Rev 1.3   03 Apr 1997 14:31:34   Jay
// Resource cleanup
// 
//    Rev 1.2   31 Jul 1996 11:58:00   Fred
// 16-bit compatibility
// 
//    Rev 1.1   28 Jul 1996 10:19:52   Jay
// Text color, new color dialog
*/

#ifndef __PMWCOLOR_H__
#define __PMWCOLOR_H__

/////////////////////////////////////////////////////////////////////////////
// CPmwColorDialog dialog

class CPmwColorDialog : public CColorDialog
{
// Construction
public:
	CPmwColorDialog(COLORREF Color, DWORD dwFlags = CC_FULLOPEN, CWnd* pParent = NULL);
	int DoModal(void);

// Dialog Data
	//{{AFX_DATA(CPmwColorDialog)
	//}}AFX_DATA

	// The custom colors.
	COLORREF	m_CustomColors[16];

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPmwColorDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
