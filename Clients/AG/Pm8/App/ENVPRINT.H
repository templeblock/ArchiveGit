/*
// $Workfile: ENVPRINT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVPRINT.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:37:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:04   FRED
// Initial revision.
// 
//    Rev 1.4   20 Nov 1995 11:18:10   JAY
// Nre reverse landscape check
// 
//    Rev 1.3   10 Aug 1995 13:23:16   JAY
//  
// 
//    Rev 1.2   09 Aug 1995 13:11:26   JAY
// Misc changes.
// 
//    Rev 1.1   08 Aug 1995 11:01:42   JAY
// Envelope stuff.
// 
//    Rev 1.0   28 Jul 1995 12:29:38   JAY
// Initial revision.
*/

#ifndef __ENVPRINT_H__
#define __ENVPRINT_H__

class CEnvelopePreview;

#include "pmwdlg.h"
#include "pmwprint.h"

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePrintDialog dialog

class CEnvelopePrintDialog : public CPmwPrint
{
// Construction
public:
	CEnvelopePrintDialog(CPmwView* pView);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEnvelopePrintDialog)
	enum { IDD = IDD_ENVELOPE_PRINT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	virtual void ChangedPrinter(void);

	void UpdatePreview(void);

	CEnvelopePreview* m_pPreview;

	// Generated message map functions
	//{{AFX_MSG(CEnvelopePrintDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPosition();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePositionDialog dialog

class CEnvelopePositionDialog : public CPmwDialog
{
// Construction
public:
	CEnvelopePositionDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEnvelopePositionDialog)
	enum { IDD = IDD_ENVELOPE_POSITION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void SetEnvelopePosition(WORD wPosition)
		{ m_wEnvelopePosition = wPosition; }
	WORD GetEnvelopePosition(void)
		{ return m_wEnvelopePosition; }

	void SetReverseLandscape(BOOL fReverseLandscape)
		{ m_fReverseLandscape = fReverseLandscape; }
	BOOL GetReverseLandscape(void)
		{ return m_fReverseLandscape; }

	void SetTractor(BOOL fTractor)
		{ m_fTractor = fTractor; }

	void SetDefaultPosition(WORD wPosition)
		{ m_wDefaultPosition = wPosition; }
	void SetDefaultReverseLandscape(BOOL fReverseLandscape)
		{ m_fDefaultReverseLandscape = fReverseLandscape; }

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void UpdatePreview(BOOL fUpdatePosition = TRUE);
	void UpdatePosition(void);
	void SelectPreview(int nPreview, BOOL fFromClick = FALSE);
	void ShowPosition(void);

	int m_nSelectedPreview;
	WORD m_wEnvelopeBase;
	WORD m_wEnvelopePosition;
	WORD m_wDefaultPosition;
	BOOL m_fTractor;

	BOOL m_fReverseLandscape;
	BOOL m_fDefaultReverseLandscape;

	CEnvelopePreview* m_pPreview[6];

	// Generated message map functions
	//{{AFX_MSG(CEnvelopePositionDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnFlapDown();
	afx_msg void OnFlapUp();
	afx_msg void OnRotated();
	afx_msg void OnClickedPreviewLT();
	afx_msg void OnClickedPreviewCT();
	afx_msg void OnClickedPreviewRT();
	afx_msg void OnClickedPreviewLW();
	afx_msg void OnClickedPreviewCW();
	afx_msg void OnClickedPreviewRW();
	afx_msg void OnDblClickedPreviewLT();
	afx_msg void OnDblClickedPreviewCT();
	afx_msg void OnDblClickedPreviewRT();
	afx_msg void OnDblClickedPreviewLW();
	afx_msg void OnDblClickedPreviewCW();
	afx_msg void OnDblClickedPreviewRW();
	afx_msg void OnDefault();
	afx_msg void OnReverseLandscape();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
