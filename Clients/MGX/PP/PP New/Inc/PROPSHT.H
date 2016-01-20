// propsht.h : header file
//
#ifndef _PROPSHT_H_
#define _PROPSHT_H_

///////////////////////////////////////////////////////////////////////
// CPPPropertySheet Class


//	CPPPropertySheet differs slightly from of MFC's CPropertySheet.
//	It's only real difference is that 
// 	(1) It must be used with CPPPropertyPage
// 	(2) It has two member functions HandleOK() and HandleCancel()
//		which calls the member function of the same name in every
//		CPPPropertyPage that has ever been made active.
class CPPPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPPPropertySheet)

public:
	CPPPropertySheet(UINT nIDCaption, CWnd *pParentWnd = NULL, UINT nIDHelp = 0, UINT iSelectPage = 0);
	CPPPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT nIDHelp = 0, UINT iSelectPage = 0);
	~CPPPropertySheet();
   
	virtual void HandleOK();
 	virtual void HandleCancel();
	virtual void OnHelp();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
protected:
	UINT m_nIDHelp;
};


class CPPPropertyPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPPropertyPage)

public:
	CPPPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0);

	virtual BOOL OnSetActive();     // called when this page gets the focus
	// you should not override these functions unless you have to
	// OK and Cancel should be handled by HandleOK() and HandleCancel()
	virtual void OnOK();
	virtual void OnCancel();
	virtual void HandleOK();
	virtual void HandleCancel();
	virtual void OnHelp();
	BOOL PageEverActive() { return m_fPageEverActive; }
	// lets the property sheet activate the apply now/ok buttons
	virtual void SetModified(BOOL bHaveApplyNow = FALSE, BOOL bChanged = TRUE);
   
protected:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnDeltaPos(UINT idFrom, NMHDR FAR *pnmhdr, LRESULT *pResult);
	afx_msg void OnHScroll(UINT uCode, UINT iPos, CScrollBar *pScrollBar);
	afx_msg void OnVScroll(UINT uCode, UINT iPos, CScrollBar *pScrollBar);
	DECLARE_MESSAGE_MAP()

protected:
	BOOL	m_fPageEverActive;
	UINT	m_nIDTemplate;
};

#endif // _PROPSHT_H_
