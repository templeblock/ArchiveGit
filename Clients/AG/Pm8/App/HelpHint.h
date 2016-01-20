//
// $Workfile: HelpHint.h $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//
// Revision History:
//
// $Log: /PM8/App/HelpHint.h $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 25    9/28/98 7:24p Hforman
// Removed all references to "Daily Tip", since it's no longer used.
// 
// 24    7/30/98 4:39p Hforman
// 
// 23    7/30/98 4:36p Hforman
// fixed modal hints crash problems
// 
// 22    7/18/98 7:34p Psasse
// Added ON_WM_KEYDOWN  to the message maps of Modal and Modeless classes
// to handle destruction
// 
// 21    7/18/98 6:26p Jayn
// "Don't show hint/tip" addition (from Dennis)
// 
// 20    7/08/98 7:52p Hforman
// fixed crash bug: pressing tutorial or help button on Modal hint;
// fixed problem with workspace hints not being shown
// 
// 19    6/30/98 6:09p Hforman
// drag preview rect instead of whole window, other tweaks to wnd
// 
// 18    6/19/98 6:11p Hforman
// add m_fUserLButtonDown
// 
// 17    6/10/98 4:35p Psasse
// Fix modal helpful hints placement as well as continue button placement
// 
// 16    6/05/98 2:13p Rlovejoy
// Added flag to avoid getting mouse capture.
// 
// 15    6/04/98 7:20p Psasse
// modal helpful hint support
// 
// 14    6/03/98 1:51p Psasse
// Much better mouse and keyboard input closing of helpful hints
// 
// 13    5/28/98 1:47p Hforman
// add voice-over flag
// 
// 12    5/26/98 6:37p Psasse
// Helpful hints sound support
// 
// 11    4/20/98 7:39p Fredf
// New 256-color helpful hint artwork. Reworked layout some.
// 
// 10    1/20/98 3:44p Fredf
// Got rid of GetNowTime(), use GetLocalTime(SYSTEMTIME) instead.
// 
// 9     12/22/97 5:26p Hforman
// only show one hint per project type and place per session
// 
// 8     12/19/97 5:36p Hforman
// 
// 7     12/18/97 12:46p Hforman
// more W-I-P
// 
// 6     12/16/97 6:15p Hforman
// 
// 5     12/16/97 11:53a Hforman
// more work in progress
// 
// 4     12/12/97 5:19p Hforman
// work in progress
// 
// 3     12/05/97 5:40p Hforman
// work in progress
// 
// 2     12/04/97 12:50p Hforman
// work in progress
//

#ifndef __HELPHINT_H__
#define __HELPHINT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "btnwnd.h"
#include "BtnNRect.h"
#include "hint.h"
#include "timestmp.h"

// forward declarations
class CWndHlpHint;

// structure for loading/storing user's hint info file

// Version number
//		1: initial version, never shipped
//		2: Uses CTimeStamp instead of CTime which fails in 2038
//		3: Added VoiceOver flag
//		4: Removed "nLastDailyTip" - no longer used
const UINT	cUserFileVersion = 4;	// version of file

struct UserHintFileInfo
{
	UINT			nVersion;				// version number for this file structure
	CTimeStamp	HintsFileTime;			// used to check if Hints file has changed

	BOOL			fUserSetPos;			// did user reposition the hint window?
	CPoint		ptUserPos;				// if so, here's the position

	BOOL			fShowHelpfulHints;	// user want to see Helpful Hints?
	BOOL			fShowDesignTips;		// user want to see Design Tips?
	BOOL			fPlayVoiceOvers;		// user want to hear voice-overs?

	CByteArray	baProjectHintCnt;		// number of times each "Project" hint shown
	CByteArray	baPlaceHintCnt;		// ditto for "Place" hints
	CByteArray	baNamedHintCnt;		// ditto for individual hints

	void DoSerialize(CArchive& ar);
};

//////////////////////////////////////////////////////////////////////////////
// CHelpfulHint class
//
// Assumption: this class gets created at app startup and lives until shutdown
//
//////////////////////////////////////////////////////////////////////////////
class CHelpfulHint
{
public:
				CHelpfulHint();
	virtual  ~CHelpfulHint();

	BOOL		LoadHelpfulHints();
	BOOL		ShowHelpfulHint(const CString& strWhich, CWnd* pParent = NULL, BOOL fModal = FALSE);
	BOOL		ShowHelpfulHint(HINT_TYPE type, int nWhich, CWnd* pParent = NULL, BOOL fModal = FALSE);
	BOOL		CreateHint(CHint* pHint, CWnd* pParent, CPoint* pos = NULL, BOOL fModal = FALSE, BOOL fGrabCapture = TRUE);
	void		RemoveHelpfulHint();
	void		SaveHintsUserConfig();
	void		ShowRecentHintsDialog();
	void		ResetHints();
	void		SetUserHintPos(CPoint newpos);

	BOOL		HelpfulHintsEnabled()		{ return m_fShowHelpfulHints; }
	void		EnableHelpfulHints(BOOL enable = TRUE) { m_fShowHelpfulHints = enable; }
	BOOL		DesignTipsEnabled()			{ return m_fShowDesignTips; }
	void		EnableDesignTips(BOOL enable = TRUE) { m_fShowDesignTips = enable; }
	BOOL		HintsVoiceEnabled()			{ return m_fPlayVoiceOvers; }
	void		EnableHintsVoice(BOOL enable = TRUE) { m_fPlayVoiceOvers = enable; }
	CWndHlpHint* GetHelpfulHintWnd()        { return m_pWndHlpHint;	}// the Helpful Hint window

protected:
	void		FreeHints();
	CHint*	FindHintByName(const CString& strName);
	CHint*	FindNextHint(HINT_TYPE type, int nWhich);
	BOOL		UserWantsHintType(CHint* pHint);
	BOOL		AllHintsOff()	{ return !m_fShowHelpfulHints && !m_fShowDesignTips; }
	void		PlaySound (CHint* pHint);

	CWndHlpHint* m_pWndHlpHint;	// the Helpful Hint window

	BOOL		m_fHintsLoaded;		// hints successfully loaded?
	BOOL		m_fShowHelpfulHints;	// does user want to see Helpful Hints?
	BOOL		m_fShowDesignTips;	// does user want to see Design Tips?
	BOOL		m_fPlayVoiceOvers;	// does user want to hear voice-overs?
	BOOL		m_fShowingNamedHint;	// currently showing a specified hint?
	BOOL		m_fShowWorkspaceHint;// time to show a generic workspace project hint?

	BOOL		m_fUserSetHintPos;	// did user reposition hints window?
	CPoint	m_ptUserHintPos;		// user's window position (if m_fUserMovedWin == TRUE)

	// arrays of flags showing whether specific project or place hint has been
	// seen this session. We only want to show one per type per session.
	BOOL		m_ProjectHintSeenThisSession[PROJECT_TYPE_Last];
	BOOL		m_PlaceHintSeenThisSession[PLACE_Last];

	CArray<CHint*,CHint*>	m_ProjectHintsArray;	// list of Project Hints
	CArray<CHint*,CHint*>	m_PlaceHintsArray;	// list of Place Hints (art gallery, etc.)
	CArray<CHint*,CHint*>	m_NamedHintsArray;	// individual hints

	CArray<CHint*,CHint*>	m_HintsSeenArray;		// list of hints seen this session
};

/////////////////////////////////////////////////////////////////////////////
// CWndHlpHint window
class CWndHlpHint : public CWnd
{
public:
	CWndHlpHint();
	CWndHlpHint(CHelpfulHint* pHH);
	virtual	~CWndHlpHint();

// Operations
public:
	void		SetHint(const CHint& hint)		{ m_curHint = hint; }
	void		SetPosition(const CPoint& pt);
	void		GrabCapture(BOOL fGrabCapture);
	void		CancelMove();

   // Override this in derived class to Grow window height
   // Be sure to call parent classes GetExtraHeight() method 
   // and include it in return value
   virtual int GetExtraHeight(void);

	virtual BOOL	InModalLoop()	{ return FALSE; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndHlpHint)
	//}}AFX_VIRTUAL

// Implementation
protected:
	void		CreateFonts();
	void		StopSound();

	CHelpfulHint* m_pHelpfulHint;	// our parent class
	CPoint	m_ptLastCursorPos;	// last mouse position (screen coords)
	BOOL		m_fPosSet;				// position specified?
	BOOL		m_fUserLButtonDown;	// has user clicked in window (begin move)?
	BOOL		m_fUserMovedWin;		// has user moved window?
	BOOL		m_fGrabCapture;		// TRUE to grab mouse capture
	BOOL		m_fInModalLoop;		// are we in modal loop?

	CRect		m_crDragCur;			// used for dragging window around
	CRect		m_crDragLast;

	CPoint	m_ptHintPos;			// window position (if m_fPosSet == TRUE)

	CRect		m_crText;				// rectangle for hint text
	CRect		m_crTitle;				// rectangle for title text

	CHint		m_curHint;
	CBitmap	m_bmFace;
	CFont		m_fontText;
	CFont		m_fontTitle;

	CNonRectButton	m_btnClose;
	CNonRectButton	m_btnHelp;
	CNonRectButton	m_btnTutorial;
   CButton        m_btnShowHints;

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndHlpHint)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnHintClose();
	afx_msg void OnHintTutorial();
	afx_msg void OnHintHelp();
	afx_msg void OnHintsShowAgain();
	LRESULT OnDisableModal(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWndModalHlpHint window
class CWndModalHlpHint : public CWndHlpHint
{
public:
	CWndModalHlpHint();
	CWndModalHlpHint(CHelpfulHint* pHH);
	virtual			~CWndModalHlpHint();
	virtual int		GetExtraHeight(void);

	virtual BOOL	InModalLoop()	{ return m_fInModalLoop; }
	int				ExecuteModalLoop();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndModalHlpHint)
	//}}AFX_VIRTUAL

// Implementation
private:
	CNonRectButton	m_btnContinue;

	int	m_nModalRet;

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndModalHlpHint)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT , UINT, UINT);
	//}}AFX_MSG
	afx_msg void OnHintClose();
	afx_msg void OnHintContinue();
	afx_msg void OnHintTutorial();
	afx_msg void OnHintHelp();

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CRecentHintsDlg dialog

class CRecentHintsDlg : public CDialog
{
// Construction
public:
	CRecentHintsDlg(const CArray<CHint*,CHint*>& hints, CHelpfulHint* pHH);

// Dialog Data
	//{{AFX_DATA(CRecentHintsDlg)
	enum { IDD = IDD_HINTS_MOST_RECENT };
	CListBox	m_ctrlList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecentHintsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CHelpfulHint*				m_pHelpfulHint;
	CArray<CHint*,CHint*>	m_HintsArray;

	// Generated message map functions
	//{{AFX_MSG(CRecentHintsDlg)
	afx_msg void OnDblclkHintslist();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowHint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __HELPHINT_H__
