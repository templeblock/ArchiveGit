/*
// $Workfile: ENVPRVW.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVPRVW.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     12/12/97 11:24a Jayn
// Fixed Class Wizard problem.
// 
//    Rev 1.0   14 Aug 1997 15:20:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:37:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:06   FRED
// Initial revision.
// 
//    Rev 1.2   10 Aug 1995 13:23:18   JAY
//  
// 
//    Rev 1.1   08 Aug 1995 11:01:44   JAY
// Envelope stuff.
// 
//    Rev 1.0   28 Jul 1995 12:31:40   JAY
// Initial revision.
*/ 

#ifndef __ENVPRVW_H__
#define __ENVPRVW_H__

class CPaperInfo;

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePreview window

class CEnvelopePreview : public CWnd
{
public:
// Construction
public:
	CEnvelopePreview();

// Implementation
public:
	virtual ~CEnvelopePreview();

	void Invalidate(void)
		{ InvalidateRect(NULL); }

	/*
	// A "position" is an 'int' which is the union of some of the values
	// in the enum above.
	*/

	void SetEnvelopePosition(WORD wPosition)
		{ m_wPosition = wPosition; Invalidate(); }
	WORD GetEnvelopePosition(void) const
		{ return m_wPosition; }

	/*
	// Is this tractor feed or not?
	*/

	void SetTractor(BOOL fTractor)
		{ m_fTractor = fTractor; }
	BOOL GetTractor(void) const
		{ return m_fTractor; }

	/*
	// Set our select state.
	*/

	void Select(BOOL fSelected = TRUE);

protected:
	// Generated message map functions
	//{{AFX_MSG(CEnvelopePreview)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	WORD m_wPosition;
	BOOL m_fSelected;
	BOOL m_fTractor;

protected:
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

/////////////////////////////////////////////////////////////////////////////

#endif
