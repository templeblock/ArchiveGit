/*
// $Workfile: NEWFONT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/NEWFONT.H $
// 
// 1     3/03/99 6:08p Gbeddow
 * 
 * 5     1/19/99 2:57p Lwilson
 * Print Shop color dialog integration.
// 
// 4     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 3     4/24/98 6:34p Fredf
// Font dialog uses new color combo boxes.
// 
// 2     4/16/98 7:32p Fredf
// "Preview Actual Size" control in font dialog (NEWFONT.CPP)
// 
//    Rev 1.0   14 Aug 1997 15:22:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:44   Fred
// Initial revision.
// 
//    Rev 1.0   15 May 1997 16:55:02   Fred
// Initial revision.
*/

struct OutlineDescription;
class COutlineElement;
class COutlineSelectComboBoxItem;
class COutlineSelectComboBox;
struct ShadowDescription;
class CShadowElement;
class CShadowSelectComboBoxItem;
class CShadowSelectComboBox;
class CFontDialogData;
class CFontTypefacePropertyPage;
class CFontEffectsPropertyPage;
class CFontPreviewButton;
class CFontPropertySheet;
class CFontPropertySheetPreviewButton;

#ifndef __NEWFONT_H__
#define __NEWFONT_H__

#include "pmwprop.h"
#include "colorcbo.h"
#include "simplecb.h"

// Framework support
#include "WinColorDlg.h"


/////////////////////////////////////////////////////////////////////////////
// OutlineDescription

struct OutlineDescription
{
	UINT m_uName;
	int m_nStyle;
	int m_nWidthType;
	CFixed m_lWidth;
};


/////////////////////////////////////////////////////////////////////////////
// COutlineElement

class COutlineElement
{
public:
	COutlineElement(LPCSTR pszName, const OutlineDescription* pDescription);
	~COutlineElement(void);
public:
	void Name(LPCSTR pszName)
		{ m_csName = pszName; }
	const CString& Name(void)
		{ return m_csName; }
	void Outline(const OutlineDescription* pDescription)
		{ m_Description = *pDescription; }
	const OutlineDescription* Description(void)
	{ return m_fDescriptionExists ? &m_Description : NULL; }
protected:
	CString m_csName;
	BOOL m_fDescriptionExists;
	OutlineDescription m_Description;
};


/////////////////////////////////////////////////////////////////////////////
// COutlineSelectComboBoxItem

class COutlineSelectComboBoxItem : public CSimpleComboBoxItem
{
public:
	COutlineSelectComboBoxItem(COutlineElement* pOutline);
	~COutlineSelectComboBoxItem(void);
public:
	virtual void PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
public:
	void Outline(COutlineElement* pOutline)
		{ m_pOutline = pOutline; }
	COutlineElement* Outline(void)
		{ return m_pOutline; }
protected:
	COutlineElement* m_pOutline;
};


/////////////////////////////////////////////////////////////////////////////
// COutlineSelectComboBox window

class COutlineSelectComboBox : public COwnerDrawComboBox
{
	INHERIT(COutlineSelectComboBox, COwnerDrawComboBox)

public:
	enum
	{
		DEFAULT_MAX_WIDTH = 36
	};
	
// Construction
public:
	COutlineSelectComboBox();

// Implementation
public:
	virtual ~COutlineSelectComboBox();
	
public:
	void Outlines(const OutlineDescription* pDescriptions, int nCount);
	void Outline(const OutlineDescription* pDescription);
	COutlineElement* Outline(void);
	void OutlineName(CString& csName, const OutlineDescription* pDescription, BOOL fCustom = FALSE);
	BOOL ChooseCustomOutline(int nMaximumWidth = DEFAULT_MAX_WIDTH);

protected:
	// Generated message map functions
	//{{AFX_MSG(COutlineSelectComboBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	virtual WNDPROC* GetSuperWndProcAddr(void);
	
protected:
	COutlineElement* m_pCustomOutline;
	int m_nCustomOutlineIndex;
};


/////////////////////////////////////////////////////////////////////////////
// ShadowDescription

struct ShadowDescription
{
	UINT m_uName;
	int m_nStyle;
	int m_nOffsetType;
	CFixed m_lXOffset;
	CFixed m_lYOffset;
};

	
/////////////////////////////////////////////////////////////////////////////
// CShadowElement

class CShadowElement
{
public:
	CShadowElement(LPCSTR pszName, const ShadowDescription* pDescription);
	~CShadowElement(void);
public:
	void Name(LPCSTR pszName)
		{ m_csName = pszName; }
	const CString& Name(void)
		{ return m_csName; }
	void Shadow(const ShadowDescription* pDescription)
		{ m_Description = *pDescription; }
	const ShadowDescription* Description(void)
	{ return &m_Description; }
protected:
	CString m_csName;
	ShadowDescription m_Description;
};


/////////////////////////////////////////////////////////////////////////////
// CShadowSelectComboBoxItem

class CShadowSelectComboBoxItem : public CSimpleComboBoxItem
{
public:
	CShadowSelectComboBoxItem(CShadowElement* pShadow);
	~CShadowSelectComboBoxItem(void);
public:
	virtual void PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground);
public:
	void Shadow(CShadowElement* pShadow)
		{ m_pShadow = pShadow; }
	CShadowElement* Shadow(void)
		{ return m_pShadow; }
protected:
	CShadowElement* m_pShadow;
};


/////////////////////////////////////////////////////////////////////////////
// CShadowSelectComboBox window

class CShadowSelectComboBox : public COwnerDrawComboBox
{
	INHERIT(CShadowSelectComboBox, COwnerDrawComboBox)

// Construction
public:
	CShadowSelectComboBox();

// Implementation
public:
	virtual ~CShadowSelectComboBox();
	
public:
	void Shadows(const ShadowDescription* pDescriptions, int nCount);
	void Shadow(const ShadowDescription* pDescription);
	CShadowElement* Shadow(void);
	void ShadowName(CString& csName, const ShadowDescription* pDescription, BOOL fCustom = FALSE);

protected:
	// Generated message map functions
	//{{AFX_MSG(CShadowSelectComboBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	virtual WNDPROC* GetSuperWndProcAddr(void);
	
protected:
	CShadowElement* m_pCustomShadow;
	int m_nCustomShadowIndex;
};


/////////////////////////////////////////////////////////////////////////////
// CFontDialogData

class CFontDialogData
{
public:
	CFontDialogData();
	virtual ~CFontDialogData();

public:
	struct
	{
		int m_nFace;
		int m_fBold;
		int m_fItalic;
		CFixed m_lSize;
		CFixed m_lBaseSize;
		CFixed m_lExpansion;
		CFixed m_lSpacing;
		FillFormatV1 m_Fill;
		OutlineFormat m_Outline;
		ShadowFormat m_Shadow;
		int m_fWhiteOut;
		int m_nUnderline;
	} Data;

public:
	void FromStyle(CPmwDoc* pDoc, CTextStyle* pStyle);
	void ToStyle(CPmwDoc* pDoc, CTextStyle* pStyle);
	BOOL operator!=(const CFontDialogData& FontData) const
		{ return memcmp(&(this->Data), &(FontData.Data), sizeof(Data)) != 0; }
};


/////////////////////////////////////////////////////////////////////////////
// CFontTypefacePropertyPage dialog

class CFontTypefacePropertyPage : public CPmwPropertyPage
{
// Construction
public:
	CFontTypefacePropertyPage(CFontDialogData* pFontData);
	virtual ~CFontTypefacePropertyPage();

// Dialog Data
	//{{AFX_DATA(CFontTypefacePropertyPage)
	enum { IDD = IDD_FONT_TYPEFACE_PAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFontTypefacePropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnColor();
	afx_msg void OnSelchangeFaceList();
	afx_msg void OnSelchangeFontSize();
	afx_msg void OnUnderline();
	afx_msg void OnWhiteBorder();
	afx_msg void OnSelchangeFontStyle();
	afx_msg void OnKillfocusFontSize();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	void RedrawPreview(void)
		{ GetParent()->GetDlgItem(IDC_FONT_PREVIEW)->Invalidate(); }
	void SetFillColor(COLOR Color);
	void UpdateEffectsFillColor(void);
	BOOL UpdateSize(void);

protected:
	CFontDialogData* m_pFontData;

protected:
	CListBox m_Typeface;
	CListBox m_Style;
	CComboBox m_Size;
	RWinColorBtn m_FillColor;
};


/////////////////////////////////////////////////////////////////////////////
// CFontEffectsPropertyPage dialog

class CFontEffectsPropertyPage : public CPmwPropertyPage
{
// Construction
public:
	CFontEffectsPropertyPage(CFontDialogData* pFontData);
	virtual ~CFontEffectsPropertyPage();

// Dialog Data
	//{{AFX_DATA(CFontEffectsPropertyPage)
	enum { IDD = IDD_FONT_EFFECTS_PAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFontEffectsPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnColor();
	afx_msg void OnOutlineColor();
	afx_msg void OnShadowColor();
	afx_msg void OnSelchangeFontOutline();
	afx_msg void OnSelchangeFontPattern();
	afx_msg void OnSelchangeFontShadow();
	afx_msg void OnOutlineMore();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	void RedrawPreview(void)
		{ GetParent()->GetDlgItem(IDC_FONT_PREVIEW)->Invalidate(); }
	void SetFillColor(COLOR Color);
	void UpdateTypefaceFillColor(void);

protected:
	CFontDialogData* m_pFontData;

protected:
	CComboBox m_Fill;
	RWinColorBtn m_FillColor;
	COutlineSelectComboBox m_Outline;
	RWinColorBtn m_OutlineColor;
	CShadowSelectComboBox m_Shadow;
	RWinColorBtn m_ShadowColor;
};


/////////////////////////////////////////////////////////////////////////////
// CFontPreviewButton

class CFontPreviewButton : public CButton
{
// Construction
public:
	CFontPreviewButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontPreviewButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFontPreviewButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFontPreviewButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CFontPropertySheet

class CFontPropertySheet : public CPmwPropertySheet
{
	DECLARE_DYNAMIC(CFontPropertySheet)

// Construction
public:
	CFontPropertySheet(CPmwDoc* pDoc, CTextStyle* pStyle, BOOL fShowActualSize, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFontPropertySheet();

	CFontTypefacePropertyPage* GetTypefacePage(void)
		{ return &m_TypefacePage; }

	CFontEffectsPropertyPage* GetEffectsPage(void)
		{ return &m_EffectsPage; }

	// "Show actual size" flag.
	BOOL GetShowActualSize(void)
		{ return m_fShowActualSize; }
	void SetShowActualSize(BOOL fShowActualSize);

	// Generated message map functions
protected:
	//{{AFX_MSG(CFontPropertySheet)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnShowActualSize(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnTabChange(UINT id, NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg LRESULT OnMovePreviewToTop(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();

// Property pages
protected:
	void AddPages(void);
	CFontTypefacePropertyPage m_TypefacePage;
	CFontEffectsPropertyPage m_EffectsPage;

protected:
	void DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct);
	CRect m_crPreview;
	CFontPreviewButton m_Preview;

	BOOL m_fShowActualSize;

protected:
	CPmwDoc* m_pDoc;
	CTextStyle* m_pStyle;

protected:
	CFontDialogData m_OriginalFontData;
	CFontDialogData m_NewFontData;

public:
	BOOL GetStyleChanges(CPmwDoc* pDoc, CTextStyle* pStyle);
};

/////////////////////////////////////////////////////////////////////////////

#endif
