/*
// $Workfile: phoprjprvw.h $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/phoprjprvw.h $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 2     1/30/99 11:32p Psasse
// Updated and fairly stable EasyPrints code
// 
//    Rev 1.0   20 Jul 1995 12:52:48   JAY
// Initial revision.
*/ 
#include "lblprvw.h"

#ifndef _PHOPRJPRVW_H_
#define _PHOPRJPRVW_H_

class CPaperInfo;
class CPhotoPrjData;

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectPreview window

class CPhotoProjectPreview : public CLabelPreview  //CWnd
{
public:
// Construction
public:
	CPhotoProjectPreview();

// Implementation
public:
	virtual ~CPhotoProjectPreview();

	void Invalidate(void)
		{ InvalidateRect(NULL); }

	/*
	// Set the PhotoProject data.
	*/

	void SetPhotoProject(CPhotoPrjData* pPhotoData);

	/*
	// Do we want to be able to select a PhotoProject in this preview?
	*/

	void WantSelect(BOOL fWantSelect = TRUE)
		{ m_fWantSelect = fWantSelect; }

	/*
	// Select a PhotoProject.
	*/

	void SelectPhotoProject(int nSubSection, int nPhoto);

	/*
	// Get the selected photo project.
	*/

	int SelectedPhoto(void) const
		{ return m_nSelectedPhoto; }

	int SelectedSubSection(void) const
		{ return m_nSelectedSubSection; }

protected:
	// Generated message map functions
	//{{AFX_MSG(CPhotoProjectPreview)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_fWantSelect;
	CPhotoPrjData* m_pPhotoPrjData;
	int m_nSelectedPhoto;
	int m_nSelectedSubSection;
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
