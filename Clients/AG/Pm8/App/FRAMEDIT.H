/*
// $Workfile: FRAMEDIT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FRAMEDIT.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:56   Fred
// Initial revision.
// 
//    Rev 1.3   10 Sep 1996 09:11:20   Jay
// Sets default point size of frame edit.
// 
//    Rev 1.2   26 Jun 1996 14:23:10   Jay
//  
// 
//    Rev 1.1   05 Jun 1996 08:21:46   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:37:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:22   FRED
// Initial revision.
// 
//    Rev 1.6   14 Aug 1995 17:18:54   JAY
// Cut/paste/undo from warp text dialog.
// 
//    Rev 1.5   03 May 1995 18:09:48   FRED
//  
// 
//    Rev 1.4   02 May 1995 17:09:24   JAY
// Fixed some smart fields bugs.
// 
//    Rev 1.3   02 May 1995 15:39:38   JAY
// More smart fields stuff.
// 
//    Rev 1.2   25 Apr 1995 15:24:36   JAY
//  
// 
//    Rev 1.1   25 Apr 1995 09:58:58   JAY
// Get and put warp text.
// 
//    Rev 1.0   25 Apr 1995 09:26:22   JAY
// Initial revision.
*/

#ifndef __FRAMEDIT_H__
#define __FRAMEDIT_H__

class CFrameEdit;
class CFrameObject;
class CMacroServer;
class PMGFontServer;

#include "redisp.h"
#include "ctxp.h"
#include "event.h"
#include "dbmap.h"
#include "command.h"

/////////////////////////////////////////////////////////////////////////////
// CFrameEdit window

class CFrameEdit : public CWnd
{
// Construction
public:
	CFrameEdit();

// Attributes
public:

// Operations
public:
	virtual BOOL Init(CMacroServer* pMacroServer, int nDefaultPointSize = -1);

	void SetTextTo(CFrameObject* pObject);
	void GetTextFrom(CFrameObject* pObject);

	CFrameObject* GetEditFrame(void)
		{ return m_pFrame; }

	CTxp* GetTxp(void)
		{ return &m_Txp; }

	void ReframeOnTxp(void);

	void InsertCharacter(CHARACTER c);

	void ReflowMacros(void);
	
	PMGDatabase* GetDatabase(void)
		{ return m_Mapping.database; }
		
	PMGFontServer* GetFontServer(void)
		{ return (PMGFontServer*)(m_Mapping.font_server); }

	CCommandList* GetCommandList(void)
		{ return &m_CommandList; }
// Implementation
public:
	virtual ~CFrameEdit();

protected:
	// Generated message map functions
	//{{AFX_MSG(CFrameEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void ToggleTxpCursor(void);
	VOID toggle_square(PBOX pbox);
	VOID toggle_select(CTxp* pTxp);
	void SignalMouseEvent(FLAGS nFlags, CPoint point, EVENT event);
	void InvokeEventHandler(void);
	
	void Keystroke(EVENT_INFO *pInfo);
	BOOL StateInit(EVENT_INFO *pInfo);
	BOOL StateNormal(EVENT_INFO *pInfo);
	BOOL StateRelease(EVENT_INFO *pInfo);
	BOOL StateDragging(EVENT_INFO *pInfo);

	void TxpCursorOn(void);
	void PositionView(PCOORD new_x0, PCOORD new_y0, PCOORD new_x1, PCOORD new_y1, FLAGS flags, int nXRound = 0, int nYRound = 0);
	void ReframeView(PBOX cursor);
	void ChangeView(int new_view, BOOL force = FALSE, PBOX_PTR bound = NULL);
	void UpdateScrolls(RECT *client, PBOX *pbox, BOOL horz, BOOL vert);
	void ViewScale(PCOORD numerator, PCOORD denominator, PPNT *dimensions, PBOX *pbox);
	void OnScroll(int nBar, UINT nSBCode, UINT nPos);

	void GetEditRect(CRect& crEdit);

	BOOL UpdateUndoTyping(CTxp* pTxp, CHARACTER c, int nCount);
	BOOL UpdateUndoDelete(CTxp* pTxp, int nCount);
	BOOL UpdateUndoBackspace(CTxp* pTxp, int nCount);

	static void TxpToggleFunction(CTxp* pTxp, int nWhich, LPVOID pData);
	static BOOL RedispToggleFunction(RedisplayContext* rc, BOOL turn_on, LPRECT clip, VOIDPTR pData);
	static VOID RefreshNotify(LPVOID pData, PBOX_PTR pExtent, REFRESH_TYPE nType, PMGPageObjectPtr pObject, LPRECT pExtraPixels);

protected:
	typedef BOOL (CFrameEdit::* STATE_HANDLER)(EVENT_INFO *);

/* Mode number bases. Leave room for range! */

	enum
	{
		STATE_INIT = 0,			/* All modes support this. */
		STATE_NORMAL = 1,			/* All modes support this. */
		STATE_RELEASE = 2,		/* All modes support this. */
		STATE_BASE_END = 3,		/* Last common mode. */

		TEXT_EDIT_STATE_BASE = 120,
		TEXT_EDIT_STATE_DRAGGING = TEXT_EDIT_STATE_BASE,
	};
	enum { FRAME_EDIT_CURSOR_TIMER = 101 };

	BOOL m_fValid;
	CFrameObject* m_pFrame;
	RedisplayContext m_rc;
	CTxp m_Txp;
	BOOL m_fCursorVisible;
	BOOL m_fCursorOn;
	EVENT_INFO m_EventInfo;
	int m_nEditState;
	DatabaseMapping m_Mapping;
	PBOX m_TextCursorPBox;		// To tell if cursor moves (do we need reframe?)
	CCommandList m_CommandList;
	
	/*
	// Scroll support.
	*/
	BOOL m_fHorizontalScrollShown;
	BOOL m_fVerticalScrollShown;

	static STATE_HANDLER States[];
	static char m_cbFileTemplate[];

// stuff for supporting creation by class name
public:
	// Member function to register a window class
	static BOOL RegisterMyClass();

protected:	
	static WNDPROC m_pfnSuperWndProc ;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
private:		
	static LONG CALLBACK AFX_EXPORT FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

	BOOL m_fCreatedByClassName ;
	virtual void PostNcDestroy();
};

#endif			// #infdef __FRAMEDIT_H__
