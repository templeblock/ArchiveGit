#if !defined(AFX_CALPICD_H__E12A8BA1_4E6A_11D1_90C4_00A0240C7400__INCLUDED_)
#define AFX_CALPICD_H__E12A8BA1_4E6A_11D1_90C4_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CalPicD.h : header file
//

#include "pdlgclr.h"
#include "calinfo.h"
#include "oriendlg.h"
#include "pmw.h"
/////////////////////////////////////////////////////////////////////////////
// CCalendarPictureDlg dialog

class CCalendarPictureDlg : public CPmwDialogColor
{
// Construction
public:
	CCalendarPictureDlg(NEWINFO * ni, COLORREF cr = INVALID_DIALOG_COLOR, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCalendarPictureDlg)
	enum { IDD = IDD_CALENDAR_PICTURE };
	int		m_iAbove;
	int		m_iPicture;
	//}}AFX_DATA
	short m_picture_style;	
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarPictureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CProjectMakePreview  m_Preview;

	// Generated message map functions
	//{{AFX_MSG(CCalendarPictureDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBack();
	afx_msg void OnPictureAbove();
	afx_msg void OnPictureNo();
	afx_msg void OnPictureSide();
	afx_msg void OnPictureYes();
	afx_msg void OnPictureSame();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	VOID enable_child(UINT id, BOOL enable);
	void FixControls();
private:
	  DWORD                m_WizFlags;
		short	m_type;
		short m_orientation;
		void	UpdateCBitmap();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALPICD_H__E12A8BA1_4E6A_11D1_90C4_00A0240C7400__INCLUDED_)
