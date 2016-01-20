/*
// $Header: /PM8/App/SPACEDLG.H 1     3/03/99 6:11p Gbeddow $
//
// Character spacing dialog
//
// $Log: /PM8/App/SPACEDLG.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:58   Fred
// Initial revision.
// 
//    Rev 1.3   15 Jan 1997 10:48:52   Jay
// Integration
// 
//    Rev 1.2   08 Jan 1997 16:35:46   Jay
// More spacing changes.
// 
//    Rev 1.1   08 Jan 1997 15:15:40   Jay
// Character spacing
// 
//    Rev 1.0   06 Jan 1997 17:08:38   Jay
// Initial revision.
*/

class CCharacterSpacingDialog;

#ifndef __SPACEDLG_H__
#define __SPACEDLG_H__

#include "pmwdlg.h"
#include "text.h"

/////////////////////////////////////////////////////////////////////////////
// CCharacterSpacingDialog dialog

#define MIN_CHARACTER_SPACING		(MakeFixed(0.50))
#define MAX_CHARACTER_SPACING		(MakeFixed(2.00))

class CCharacterSpacingDialog : public CPmwDialog
{
// Construction
public:
	CCharacterSpacingDialog(CTextStyle& Style, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCharacterSpacingDialog)
	enum { IDD = IDD_CHARACTER_SPACING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CFixed GetSpacing(void) const
		{ return m_Style.Spacing(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCharacterSpacingDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
	void PreviewFont(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void FramePreview(HDC hdc, HPEN hPen, HBRUSH hBrush);
	void RefreshPreview(void);
	void SetSpacing(CFixed lSpacing);
	void OnRadioChange(UINT uID);
	CFixed GetSpacingValue(void);
protected:

	// Generated message map functions
	//{{AFX_MSG(CCharacterSpacingDialog)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSpacing();
	afx_msg void OnVeryTight();
	afx_msg void OnVeryLoose();
	afx_msg void OnTight();
	afx_msg void OnNormal();
	afx_msg void OnLoose();
	afx_msg void OnCustom();
	afx_msg void OnKillfocusSpacing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:

	CTextStyle m_Style;
	CRect m_crFullPreview;
	CRect m_crPreview;
	CFixed m_lOriginalSpacing;
	BOOL m_fSelfSet;
};

#endif
