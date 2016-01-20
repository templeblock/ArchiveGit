// $Header: /PM8/App/PMWDLG.H 1     3/03/99 6:09p Gbeddow $
//
// pmwdlg.h : Standard PMW dialog definitions.
//
// $Log: /PM8/App/PMWDLG.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 5     3/01/99 1:40p Mwilson
// changed base class
// 
// 4     7/28/98 9:39a Jayn
// INSTALL ifdefs
// 
// 3     7/26/98 7:11p Hforman
// moved OnQueryNewPalette() and OnPaletteChanged() here from
// CPmwDialogColor
// 
// 2     12/16/97 5:39p Hforman
// Helpful Hint removal
// 
//    Rev 1.0   14 Aug 1997 15:24:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:14   Fred
// Initial revision.
// 
//    Rev 1.4   10 Jul 1997 12:31:24   Jay
// Card server, etc.
// 
//    Rev 1.3   11 Apr 1997 11:23:48   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.2   28 Mar 1997 17:11:22   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.1   21 Jun 1996 17:50:46   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:29:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:36   FRED
// Initial revision.
// 
//    Rev 2.6   09 Jun 1995 07:43:38   FRED
// Sound Manager changes
// 
//    Rev 2.5   07 Jun 1995 16:45:46   FRED
// Album dialog and Project album dialog.
// 
// 
//    Rev 2.4   05 Jun 1995 16:52:14   JAY
// Now can resize.
// 
//    Rev 2.3   27 Apr 1995 13:29:04   FRED
// 
//    Rev 2.2   19 Apr 1995 12:32:26   JAY
//  
// 
//    Rev 2.1   08 Feb 1995 13:33:42   JAY
// Reverted. New series.
// 
//    Rev 1.3   05 May 1994 14:16:18   JAY
// Added standard HelpHitTest
// 
//    Rev 1.2   29 Apr 1994 14:56:42   JAY
// Added a HELPHITTEST method so we can get dialog control ids.
// 
//    Rev 1.1   27 Apr 1994 14:49:00   JAY
// Added standard_dialog_shutdown
// 
//    Rev 1.0   30 Mar 1994 09:11:10   JAY
// Initial revision.

#ifndef __PMWDLG_H__
#define __PMWDLG_H__

#include "HybridBaseDlg.h"
class CHybridBaseDlgManagerState;

extern void standard_dialog_startup(CWnd *pDlg, UINT sound_id = 0, CHybridBaseDlgManagerState* pState = NULL);
extern void standard_dialog_shutdown(CWnd *pDlg, UINT sound_id, CHybridBaseDlgManagerState* pState = NULL);
extern void standard_dialog_activate(UINT sound_id, UINT nState, CWnd* pWndOther, BOOL bMinimized);
extern void standard_dialog_resize(CWnd *pDlg, UINT sound_id, CHybridBaseDlgManagerState* pState = NULL);
extern LRESULT standard_dialog_hit_test(CWnd *pDlg, UINT sound_id, LPARAM lParam);

#ifndef INSTALL
extern CPalette *pOurPal;     // THE PrintMaster Palette (from PMWVIEW.CPP)
#endif

/////////////////////////////////////////////////////////////////////////////
// CPmwDialog dialog

class CPmwDialog : public CHybridBaseDlg
{
// Construction
public:
	CPmwDialog(UINT idd, CWnd* pParent = NULL);	// standard constructor
	~CPmwDialog();

// Dialog Data
	//{{AFX_DATA(CPmwDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	UINT m_sound_id;
	// Generated message map functions
	//{{AFX_MSG(CPmwDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
   afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	//}}AFX_MSG
#ifndef INSTALL
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
#endif
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);

	int m_nNormalWidth;
	int m_nNormalHeight;
	
	int m_nTargetWidth;
	int m_nTargetHeight;
	
	CMapPtrToPtr m_ChildPosition;
	
	LOGFONT m_lfFont;
	HFONT m_hFont;
	
	virtual void OnSetFont(CFont* pFont);
	virtual void SizeDialog(int nWidth, int nHeight);

	static BOOL CALLBACK RecordChildWindow(HWND hWindow, LPARAM lData);
	static BOOL CALLBACK SizeChildWindow(HWND hWindow, LPARAM lData);

	// Data exchange helpers.
	void GetDoubleValue(CDataExchange* pDX, UINT uID,
							  double& dValue, BOOL& fValue,
							  double dMin, double dMax);
	void SetDoubleValue(UINT uID, double dValue, int nPrecision, BOOL fValue = TRUE);
	void GetIntValue(CDataExchange* pDX, UINT uID,
						  int& dValue, BOOL& fValue,
						  int dMin, int dMax);
	void SetIntValue(UINT uID, int nValue, BOOL fValue = TRUE);

public:
	void set_sound_id(UINT sound_id)
		{ m_sound_id = sound_id; }
	UINT get_sound_id(void)
		{ return m_sound_id; }
	void EnableDlgItem(int nId, BOOL fEnable);
};

#endif					// #ifndef __PMWDLG_H__
