/*
// $Workfile: LBLPRVW.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LBLPRVW.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:38:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:40   FRED
// Initial revision.
// 
//    Rev 1.1   10 Aug 1995 13:23:18   JAY
//  
// 
//    Rev 1.0   20 Jul 1995 12:52:48   JAY
// Initial revision.
*/ 

#ifndef _LBLPRVW_H_
#define _LBLPRVW_H_

class CPaperInfo;

/////////////////////////////////////////////////////////////////////////////
// CLabelPreview window

class CLabelPreview : public CWnd
{
public:
// Construction
public:
	CLabelPreview();

// Implementation
public:
	virtual ~CLabelPreview();

	void Invalidate(void)
		{ InvalidateRect(NULL); }

	/*
	// Set the label data.
	*/

	void SetLabel(CPaperInfo* pLabelData);

	/*
	// Do we want to be able to select a label in this preview?
	*/

	void WantSelect(BOOL fWantSelect = TRUE)
		{ m_fWantSelect = fWantSelect; }

	/*
	// Select a label.
	*/

	void SelectLabel(int nLabel);

	/*
	// Get the selected label.
	*/

	int SelectedLabel(void) const
		{ return m_nSelectedLabel; }

protected:
	// Generated message map functions
	//{{AFX_MSG(CLabelPreview)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_fWantSelect;
	CPaperInfo* m_pLabelData;
	int m_nSelectedLabel;
	CRect m_crDrawn;				// Last drawn rect.

//	void HideSelection(void);
	void ShowSelection(CDC* pDC = NULL, BOOL fHighlight = TRUE);

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
