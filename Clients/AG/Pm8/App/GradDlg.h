#if !defined(AFX_GRADDLG_H__27692A21_DB5D_11D1_8680_0060089672BE__INCLUDED_)
#define AFX_GRADDLG_H__27692A21_DB5D_11D1_8680_0060089672BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GradDlg.h : header file
//

#include "colorcbo.h"   // For CColorCombo
#include "simplelb.h"
#include "fill.h"
#include "PmwDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CGradientStyleListBoxItem

class CGradientStyleListBoxItem : public CSimpleListBoxItem
{
	INHERIT(CGradientStyleListBoxItem, CSimpleListBoxItem)

protected:
	CGradientFill* m_pFillObject;

public:
	CGradientStyleListBoxItem(int nType);
	~CGradientStyleListBoxItem();

	CGradientFill* GetFillObject()
	{
		return m_pFillObject;
	}
public:
	virtual void MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct);

protected:
	virtual void PaintItem(COwnerDrawListBox* pListBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);

};

/////////////////////////////////////////////////////////////////////////////
// CGradientStyleListBox window

class CGradientStyleListBox : public COwnerDrawListBox
{
	INHERIT(CGradientStyleListBox, COwnerDrawListBox)
	
// Construction
public:
	CGradientStyleListBox();

// Implementation
public:
	virtual ~CGradientStyleListBox();
	
public:
	void BuildList(COLOR BaseColor, COLOR BlendColor);

protected:
	// Generated message map functions
	//{{AFX_MSG(CGradientStyleListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	virtual WNDPROC* GetSuperWndProcAddr(void);
};

/////////////////////////////////////////////////////////////////////////////
// CGradientDlg dialog

class CGradientDlg : public CPmwDialog
{
// Construction
public:
	CGradientDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGradientDlg)
	enum { IDD = IDD_GRADIENT };
	CGradientStyleListBox	m_listStyles;
	CGradientColorCombo	m_comboBlendColor;
	CGradientColorCombo	m_comboBaseColor;
	//}}AFX_DATA

// Attributes
public:
	COLOR GetBaseColor();
	COLOR GetBlendColor();
	SHORT GetFillType();
	
	void SetBaseColor(COLOR BaseColor);
	void SetBlendColor(COLOR BlendColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGradientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Base & blend colors
	static COLOR m_BaseColor;
	static COLOR m_BlendColor;

	// Chosen fill type
	static SHORT m_nFillType;

	// Generated message map functions
	//{{AFX_MSG(CGradientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeBaseColor();
	afx_msg void OnSelchangeBlendColor();
	afx_msg void OnDblclkGradientStyle();
	virtual void OnOK();
	afx_msg void OnSwapColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRADDLG_H__27692A21_DB5D_11D1_8680_0060089672BE__INCLUDED_)
