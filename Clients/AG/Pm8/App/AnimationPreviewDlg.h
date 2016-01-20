/*	$Header: /PM8/App/AnimationPreviewDlg.h 1     3/03/99 6:03p Gbeddow $
//
//	Definition of the CPAnimationPreviewDlg class.
//
//	This class provides a wrapper for the GIF animation preview dialog.
//	
//	(c)Copyright 1999 The Learning Company, Inc., all rights reserved
//
// $Log: /PM8/App/AnimationPreviewDlg.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 1     1/26/99 3:24p Rgrenfell
// New dialog class for use in animation previews.
*/

#ifndef ANIMATIONPREVIEWDLG_H
#define ANIMATIONPREVIEWDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPGIFAnimator;

#define GIF_PREVIEW_TIMER 42	// Picked a value.

/////////////////////////////////////////////////////////////////////////////
// CPAnimationPreviewDlg dialog

/* This class provides a wrapper around the GIF animation preview dialog.
	This is a dialog with a preview region for an animated GIF which runs off
	of a Windows timer to supply the interupts necessary to run the animation.
*/
class CPAnimationPreviewDlg : public CDialog
{
// Construction
public:
	CPAnimationPreviewDlg(CPGIFAnimator *pGIFAnimator, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPAnimationPreviewDlg)
	enum { IDD = IDD_ANIMATION_PREVIEW };
	CButton	m_ctlPreviewFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPAnimationPreviewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Renders the next frame of the animated gif.
	UINT Update(LPDRAWITEMSTRUCT lpDrawItemStruct);

	// Generated message map functions
	//{{AFX_MSG(CPAnimationPreviewDlg)
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	// Stores the internal reference to the GIF animator to use.
	CPGIFAnimator	*m_pGIFAnimator;
	// Set when we are drawing our animation so we aren't re-entrant.
	bool m_bUpdating;
	// Stores the area where the image should be drawn.
	CRect m_rcViewingArea;
	// Tracks the allocated timer.
	int m_nTimer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // ANIMATIONPREVIEWDLG_H
