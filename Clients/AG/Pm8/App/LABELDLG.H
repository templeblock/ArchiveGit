/*
// $Workfile: LABELDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LABELDLG.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:24   Fred
// Initial revision.
// 
//    Rev 1.2   15 Apr 1997 16:56:18   johno
// Changes for "New Project" path
// 
// 
//    Rev 1.1   10 Apr 1997 17:15:18   johno
// Now a PmwDialogColor
// 
// 
//    Rev 1.0   14 Mar 1996 13:38:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:34   FRED
// Initial revision.
// 
//    Rev 1.2   17 Aug 1995 07:49:28   JAY
// Added double-click handler for the list.
// 
//    Rev 1.1   16 Aug 1995 16:22:26   JAY
// New stuff.
// 
//    Rev 1.0   20 Jul 1995 12:52:52   JAY
// Initial revision.
*/

#ifndef __LABELDLG_H__
#define __LABELDLG_H__

#include "labels.h"
#include "pdlgclr.h"

class CLabelPreview;

/////////////////////////////////////////////////////////////////////////////
// CLabelTypeDialog dialog

class CLabelTypeDialog : public CPmwDialogColor
{
// Construction
public:
	CLabelTypeDialog(CPaperInfo* pOldInfo, CWnd* pParent = NULL, DWORD WizFlags = WIZBUT_OK | WIZBUT_CANCEL | WIZBUT_HELP, COLORREF cr = INVALID_DIALOG_COLOR);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLabelTypeDialog)
	enum { IDD = IDD_LABEL_TYPE };
	//}}AFX_DATA

	CLabelData* GetChosenLabel(void);

// Implementation
protected:
	DWORD       m_WizFlags;
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void BuildList(void);

	// Generated message map functions
	//{{AFX_MSG(CLabelTypeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLabelList();
	afx_msg void OnLabelTypeChanged();
	virtual void OnOK();
	afx_msg void OnDblclkLabelList();
	afx_msg void OnBack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CLabelList m_List;
	CLabelPreview* m_pPreview;
	CPaperInfo* m_pOldInfo;

	CLabelData* m_pChosenLabel;
};

#endif
