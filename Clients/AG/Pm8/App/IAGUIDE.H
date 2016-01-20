/*
// $Workfile: IAGUIDE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IAGUIDE.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:08   Fred
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 07:48:30   Fred
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:32   JAY
// Reverted. New series.
// 
//    Rev 1.0   18 Jul 1994 16:28:58   FRED
// Initial revision.
*/ 

#ifndef _IAGUIDE_H_
#define _IAGUIDE_H_

#include "installr.h"
#include "pmwdlg.h"
#include "ddb.h"
#include "sparkle.h"

/////////////////////////////////////////////////////////////////////////////
// CInstallAudioGuideDialog dialog

class CInstallAudioGuideDialog : public CPmwDialog
{
// Construction
public:
	CInstallAudioGuideDialog(CWnd* pParent = NULL, BOOL fMonochrome = FALSE);	// standard constructor
	~CInstallAudioGuideDialog(void);

// Dialog Data
	//{{AFX_DATA(CInstallAudioGuideDialog)
	enum { IDD = IDD_INSTALL_AUDIO_GUIDE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInstallAudioGuideDialog)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	CDDB *m_pDDB;
	CSparklers *m_pSparklers;
	UINT m_nTimerID;
	BOOL m_fMonochrome;
	
	enum { SPARKLE_TIMER_ID = 0 };
};

#endif
