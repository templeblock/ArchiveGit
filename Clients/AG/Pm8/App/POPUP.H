/*
// $Header: /PM8/App/POPUP.H 1     3/03/99 6:10p Gbeddow $
//
// popup.h : Pop-up window classes
//
// $Log: /PM8/App/POPUP.H $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 11    2/09/99 4:44p Rgrenfel
// Added support for the Made To Order toolbar popup menu for seals and
// timepieces.
// 
// 10    1/27/99 1:44p Lwilson
// Added RColor support to color palette.
 * 
 * 9     1/19/99 2:57p Lwilson
 * Print Shop color dialog integration.
// 
// 8     1/14/99 3:44p Jfleischhauer
// added CPPhotoWorkshopMenuPalette class
// 
// 7     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 6     5/05/98 4:44p Rlovejoy
// Gradient button switch.
// 
// 5     4/29/98 6:32p Rlovejoy
// Updated for new fill format.
// 
// 4     4/24/98 5:37p Rlovejoy
// Exteneded to show a gradient button.
// 
// 3     3/18/98 6:31p Fredf
// Added color button top to of popup menus.
// 
// 2     3/17/98 7:09p Fredf
// Added color button to shadow popup.
// 
//    Rev 1.0   14 Aug 1997 15:24:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:22   Fred
// Initial revision.
// 
//    Rev 1.10   02 Jul 1997 12:58:36   Fred
// Fixed problem with object/text popup duality
// 
//    Rev 1.9   23 May 1997 17:06:48   Fred
// New effects
// 
//    Rev 1.8   15 May 1997 16:48:48   Fred
// New text effects
// 
//    Rev 1.7   17 Mar 1997 16:09:28   Jay
// Changed drawing of None for Text Shape popup.
// 
//    Rev 1.6   17 Mar 1997 14:48:26   Jay
// Warp shape palette and support.
// 
//    Rev 1.5   17 Mar 1997 09:40:16   Jay
// Popup warp field palette
// 
//    Rev 1.4   28 Jan 1997 14:10:38   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.3   24 Jan 1997 17:05:48   Jay
// Shadow popup.
// 
//    Rev 1.2   24 Jan 1997 14:56:06   Jay
// Line palette
// 
//    Rev 1.1   24 Jan 1997 12:39:26   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.0   23 Jan 1997 11:05:52   Jay
// New popup palettes
*/

#ifndef __POPUP_H__
#define __POPUP_H__

#include "pagedefs.h"
#include "shapfile.h"
#include "warpfile.h"

/////////////////////////////////////////////////////////////////////////////
// CPopupWnd : Base popup window class

class CPopupWnd : public CWnd
{
public:
	CPopupWnd();

   BOOL Create(CRect& r, CWnd* pParent);

protected:
	// Generated message map functions
	//{{AFX_MSG(CPopupWnd)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnCancel(void);

	static CFont m_Font;				// The font for general use.
	virtual CFont* GetPopupFont(void);
	void GetPopupFontMetrics(TEXTMETRIC& tm);
};

/////////////////////////////////////////////////////////////////////////////
// CPopupPalette : Popup palette window class

class CPopupPalette : public CPopupWnd
{
public:
	CPopupPalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	CPoint ItemCount(void)
		{ return m_cpItemCount; }
	CSize ItemSize(void)
		{ return m_czItemSize; }

	virtual void InitItems(CWnd* pParent) = 0;
	virtual void DrawItemBackground(CDC* pDC) = 0;
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected) = 0;

	virtual void OnOK(void);
	virtual void OnMore(void);
	virtual void OnColor(void);
	virtual void OnGradient(void);

	void SelectItem(CPoint cpItem);			// By x, y
	void SelectItem(int nSelection);			// By index
	
	void ShowGradient(BOOL fShowGradient)
	{
		m_fShowGradient = fShowGradient;
	}
	// Special item numbers.
	enum
	{
		ITEM_None = 1000,
		ITEM_More,
		ITEM_Color,
		ITEM_Gradient
	};

protected:
	// Generated message map functions
	//{{AFX_MSG(CPopupPalette)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	// Variables set by InitItems();
	CPoint m_cpItemCount;
	CSize m_czItemSize;
	BOOL m_fHasNone;
	BOOL m_fHasMore;
	BOOL m_fHasColor;
	BOOL m_fHasGradient;

	// Other variables.
	CRect m_crItems;
	CRect m_crNone;			// Rect for "none" button.
	CRect m_crMore;			// Rect for "more" button.
	CRect m_crColor;			// Rect for "color" button.
	CRect m_crGradient;		// Rect for "gradient" button.
	BOOL m_fShowGradient;

	int m_nSelectedItem;

	UINT m_uID;					// The ID for the WM_COMMAND message.

	void RedrawItem(int nItem, CDC* pDC = NULL);
	void NotifyOwner(void);
	virtual void DrawString(CDC* pDC, CString& csText, CRect crDraw, int nMethod);		// Utility function
	virtual void DrawNone(CDC* pDC, CRect crDraw);		// Utility function
	virtual void DrawMore(CDC* pDC, CRect crDraw);		// Utility function
	virtual void DrawColor(CDC* pDC, CRect crDraw);		// Utility function
	virtual void DrawGradient(CDC* pDC, CRect crDraw);	// Utility function

	virtual int SizeNone(int nDefaultHeight);
	virtual int SizeMore(int nDefaultHeight);
	virtual int SizeColor(int nDefaultHeight);
	virtual int SizeGradient(int nDefaultHeight);

	CPoint ItemToXY(int nItem);
	int XYToItem(CPoint p);
};

/////////////////////////////////////////////////////////////////////////////
// CColorPalette : Color palette window class

class CColorPalette : public CPopupPalette
{
public:
	CColorPalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual void InitItems(CWnd* pParent);
	virtual void DrawItemBackground(CDC* pDC);
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected);

	COLOR	 GetColor(void);
	RColor GetRColor(void);
	COLOR  GetBlendColor(void);
	SHORT  GetFillType(void);

	void SetColor(COLOR Color);
	void SetColor(const RColor& Color);

	BOOL IsNone(void);
	void SetNone(void);

	virtual void OnOK(void);
	virtual void OnMore(void);
	virtual void OnGradient(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CColorPalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
//	static COLORREF m_clColors[];
	static COLOR m_Colors[];

	RColor	m_crColor;
	COLOR		m_Color;				// The current color.
	COLOR		m_BlendColor;		// The current blend color (for gradients).
	SHORT		m_nFillType;		// The current fill type
};

/////////////////////////////////////////////////////////////////////////////
// CGradientColorPalette : Gradient color palette window class

class CGradientColorPalette : public CColorPalette
{
public:
	CGradientColorPalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual void InitItems(CWnd* pParent);

protected:
	// Generated message map functions
	//{{AFX_MSG(CGradientColorPalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
};

/////////////////////////////////////////////////////////////////////////////
// CLineWidthPalette : Object line width palette window class

class CLineWidthPalette : public CPopupPalette
{
public:
	CLineWidthPalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual void InitItems(CWnd* pParent);
	virtual void DrawItemBackground(CDC* pDC);
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected);

	void GetWidth(SHORT& nStyle, SHORT& nWidthType, CFixed& lWidth);
	void SetWidth(SHORT nStyle, SHORT nWidthType, CFixed lWidth);

	BOOL IsNone(void);
	void SetNone(void);

	virtual void OnOK(void);
	virtual void OnMore(void);
	virtual void OnColor(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CLineWidthPalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	struct LineWidth
	{
		UINT m_uName;
		SHORT m_nStyle;
		SHORT m_nWidthType;
		CFixed m_lWidth;
	};

protected:
	LineWidth* m_pLineWidths;
	int m_nLineWidths;

	SHORT m_nStyle;		// The current style.
	SHORT m_nWidthType;	// The current width type.
	CFixed m_lWidth;		// The current width.
};

/////////////////////////////////////////////////////////////////////////////
// CTextLineWidthPalette : Text line width palette window class

class CTextLineWidthPalette : public CLineWidthPalette
{
public:
	CTextLineWidthPalette();

protected:
	// Generated message map functions
	//{{AFX_MSG(CTextLineWidthPalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CShadowPalette : Object shadow palette window class

class CShadowPalette : public CPopupPalette
{
public:
	CShadowPalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual void InitItems(CWnd* pParent);
	virtual void DrawItemBackground(CDC* pDC);
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected);

	void GetShadow(SHORT& nStyle, SHORT& nOffsetType, CFixed& lXOffset, CFixed& lYOffset);
	void SetShadow(SHORT nStyle, SHORT nOffsetType, CFixed lXOffset, CFixed lYOffset);

	BOOL IsNone(void);
	void SetNone(void);

	virtual void OnOK(void);
	virtual void OnColor(void);

	enum
	{
		ShadowTileWidth = 23,
		ShadowTileHeight = 23
	};

protected:
	// Generated message map functions
	//{{AFX_MSG(CShadowPalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	struct ShadowInformation
	{
		UINT m_uName;
		SHORT m_nStyle;
		SHORT m_nOffsetType;
		CFixed m_lXOffset;
		CFixed m_lYOffset;
	};

protected:
	ShadowInformation* m_pShadowInformation;
	int m_nShadowInformation;

	SHORT m_nStyle;		// The current style
	SHORT m_nOffsetType;	// The current offset type
	CFixed m_lXOffset;	// The current X offset
	CFixed m_lYOffset;	// The current Y offset
};

/////////////////////////////////////////////////////////////////////////////
// CTextShadowPalette : Text shadow palette window class

class CTextShadowPalette : public CShadowPalette
{
public:
	CTextShadowPalette();

protected:
	// Generated message map functions
	//{{AFX_MSG(CTextShadowPalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CShapePalette : Shape palette window class

class CShapePalette : public CPopupPalette
{
public:
	CShapePalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual void InitItems(CWnd* pParent);
	virtual void DrawItemBackground(CDC* pDC);
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected);

	virtual void OnOK(void);
	virtual void OnMore(void);

	enum
	{
		ShapeTileWidth = 24,
		ShapeTileHeight = 24
	};

protected:
	// Generated message map functions
	//{{AFX_MSG(CShapePalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int m_nShape;		// Shape number
	// The shape definition.
	CShapeEntry m_ShapeEntry;

	void NotifyOwner(void);

	static void LoadShapes(void);
	static CShapeFile m_ShapeFile;
};

/////////////////////////////////////////////////////////////////////////////
// CTextShapePalette : Text shape palette window class

class CTextShapePalette : public CPopupPalette
{
public:
	CTextShapePalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual void InitItems(CWnd* pParent);
	virtual void DrawItemBackground(CDC* pDC);
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected);

	virtual void OnOK(void);
	virtual void OnMore(void);

	void SetShape(LPCSTR pName);
	CWarpField* GetShape(void);

	BOOL IsNone(void);
	void SetNone(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CTextShapePalette)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int m_nShape;		// Shape number

	virtual int SizeNone(int nDefaultHeight);

	static void LoadShapes(void);
	static CWarpFile m_ShapeFile;
};


/////////////////////////////////////////////////////////////////////////////
//
// CPPhotoWorkshopMenuPalette : A "palette" to handle PhotoWorkshop items
//
/////////////////////////////////////////////////////////////////////////////
class CPPhotoWorkshopMenuPalette : public CPopupPalette
{
public:
	CPPhotoWorkshopMenuPalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual void InitItems(CWnd* pParent);
	virtual void DrawItemBackground(CDC* pDC);
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected);

	int GetMenuItem() { return m_nChoiceIndex; }

	virtual void OnOK(void);

private:
	static const int m_nMenuChoices[];
	static const int m_nNumberOfChoices;

	int m_nChoiceIndex;
};


/////////////////////////////////////////////////////////////////////////////
//
// CPPhotoWorkshopMenuPalette : A "palette" to handle PhotoWorkshop items
//
/////////////////////////////////////////////////////////////////////////////
class CPMadeToOrderMenuPalette : public CPopupPalette
{
public:
	CPMadeToOrderMenuPalette();

   virtual BOOL Create(CWnd* pParent, UINT uID = 0);

	virtual void InitItems(CWnd* pParent);
	virtual void DrawItemBackground(CDC* pDC);
	virtual void DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected);

	int GetMenuItem() { return m_nChoiceIndex; }

	virtual void OnOK(void);

private:
	static const int m_nMenuChoices[];
	static const int m_nNumberOfChoices;

	int m_nChoiceIndex;
};

#endif
