#if !defined(AFX_COLORCBO_H__1D029841_A2D8_11D1_A666_00A024EA69C0__INCLUDED_)
#define AFX_COLORCBO_H__1D029841_A2D8_11D1_A666_00A024EA69C0__INCLUDED_

/***************************************************************************
*  FILE:        COLORCBO.H                                                 *
*  SUMMARY:     OwnerDraw Combo class for choosing a color.                *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*  2/12/98  DGP      Created                                               *

   See colorcbo.cpp for revision history

***************************************************************************/

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ColorCbo.h : header file
//

#include "pagedefs.h"   // for COLOR
#include "popup.h"

/////////////////////////////////////////////////////////////////////////////
// CColorCombo window

class CColorCombo : public CComboBox
{
// Construction
public:
	CColorCombo();
   void Init();

   void           SetColor(COLORREF crNewColor);
   COLOR          GetColor() const;

protected:
// Helpers
	virtual void   DoDropDown();
   void           SizeItems(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorCombo)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorCombo)
	afx_msg void OnColorPalette();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnCancelMode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
   // Helpers
      BOOL FontMetrics(TEXTMETRIC& Metrics);

   // Static Methods
      static BOOL CreateItemFont(CRect &crDims);

protected:
   // Data Members
   COLOR    m_color;
   BOOL     m_bShowPalette;
   BOOL     m_bSizedItems;
	CColorPalette* m_pColorPaletteWnd;
	BOOL		m_bWndAllocated;

   // Static Data
   static CFont sm_cfontText;
};

/////////////////////////////////////////////////////////////////////////////
// CGradientColorCombo window

class CGradientColorCombo : public CColorCombo
{
// Construction
public:
	CGradientColorCombo();

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCBO_H__1D029841_A2D8_11D1_A666_00A024EA69C0__INCLUDED_)

