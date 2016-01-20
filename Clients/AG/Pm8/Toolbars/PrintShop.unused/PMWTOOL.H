/*
// $Header: /PM8/Toolbars/PrintShop/PMWTOOL.H 1     3/03/99 6:35p Gbeddow $
//
// pmwtool.h : PrintMaster toolbar subclass
//
// $Log: /PM8/Toolbars/PrintShop/PMWTOOL.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
// 1     2/24/99 12:24p Ddusich
// Print Shop Toolbar DLL
// 
// 1     1/20/99 9:56a Mwilson
// 
// 1     12/11/98 11:04a Mwilson
// 
// 14    12/08/98 2:47p Mwilson
// added yywb to creataparty
// 
// 12    11/19/98 3:17p Johno
// Move online art button to file toolbar
// 
// 11    9/25/98 11:07a Mwilson
// added support for instruction page
// 
// 10    6/20/98 11:42a Cboggio
// Changes to allow some toolbar buttons to be completely removed for
// certain localized configurations
// 
// 9     6/11/98 9:36a Dennis
// Added AG Specific Toolbars
// 
// 8     5/26/98 2:06p Dennis
// Added unique edit toolbar for American Greetings which is used based on
// Configuration.
// 
// 7     4/10/98 4:57p Rlovejoy
// Function to enable/disable combo boxes.
// 
// 6     3/24/98 2:16p Rlovejoy
// Changes for MRU font combo box.
// 
// 5     3/23/98 11:55a Fredf
// Special divider line for Edit toolbars.
// 
// 4     3/21/98 1:18p Fredf
// New menus and toolbar names.
// 
// 3     3/17/98 7:10p Fredf
// Removed unused code.
// 
// 2     3/17/98 2:43p Fredf
// New workspace toolbars.
// 
//    Rev 1.0   14 Aug 1997 15:24:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:20   Fred
// Initial revision.
// 
//    Rev 1.6   09 May 1997 15:40:28   Jay
// New global tools
// 
//    Rev 1.5   09 Apr 1997 17:14:14   Jay
// PhotoEnhancer
// 
//    Rev 1.4   03 Apr 1997 14:31:34   Jay
// Resource cleanup
// 
//    Rev 1.3   07 Feb 1997 09:17:48   Jay
// Draws line between add and modify bars.
// 
//    Rev 1.2   03 Feb 1997 15:55:16   Jay
// Custom tool buttons
// 
//    Rev 1.1   31 Jan 1997 15:51:12   Jay
// Some new toolbar bitmaps.
// 
//    Rev 1.0   15 Jan 1997 10:49:40   Jay
// New toolbars
*/

#ifndef __PMWTOOL_H__
#define __PMWTOOL_H__

#include "custbar.h"
#include "barcombo.h"
#include "typecmbo.h"

/////////////////////////////////////////////////////////////////////////////
// CPmwToolBar

class CPmwToolBar : public CCustomToolBar
{
public:
	CPmwToolBar();

	BOOL Create(CWnd* pParentWnd, DWORD dwStyle);

	enum
	{
		DropHeight = 150
	};

protected:
	// Generated message map functions
	//{{AFX_MSG(CPmwToolBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);

	// Helper functions.
	void SetupCombo(int nId, UINT pId, int nWidth, CComboBox& Combo);
	void HideCombo(CComboBox& Combo);
	int IndexFromId(UINT nId);
	virtual const UINT* GetBitmapIds(void) = 0;

	DWORD GetBarStyle(void) const
		{ return m_dwStyle; }

	// Variables.
	UINT m_uID;
	BOOL m_fInDialog;			// Are we in a dialog?

protected:
	// Overridable methods for drawing background, buttons, and separators.
	virtual void DrawBorder(CDC& dc, CRect& crBorder);
	virtual void DrawBackground(CDC& dc, const CRect& crBackground, const CSize& czOffset);
	virtual void DrawButton(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState);
	virtual void DrawSeparator(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState);

protected:
	// Helper functions for drawing group borders.
	int IsStartOfGroup(int nButtonIndex);
	int IsEndOfGroup(int nButtonIndex);
};

class CFileToolBar : public CPmwToolBar
{
public:
	CFileToolBar();

	BOOL Create(CWnd* pParentWnd, DWORD dwStyle);

	virtual const UINT* GetBitmapIds(void);

private:
	static UINT m_uBitmapIds[];
	static UINT m_uFormat[];
};

class CFormatToolBar : public CPmwToolBar
{
public:
	CFormatToolBar();

	BOOL Create(CWnd* pParentWnd, DWORD dwStyle);

	virtual const UINT* GetBitmapIds(void);

	BOOL HasFocus(void);
	void EnableFontBoxes(BOOL fEnable);
	void BuildTypefaceList(void);
	void BuildPointSizeList(void);
//	void BuildStyleList(void);

public:
#ifdef WANT_STYLE
	CBarCombo   m_StyleBox;
#endif
	CTypefaceCombo m_TypefaceBox;
	CBarCombo   m_SizeBox;

protected:
	// Generated message map functions
	//{{AFX_MSG(CFormatToolBar)
	afx_msg LRESULT OnGetSelFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFontSize(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateFonts(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateMRU(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT HasFocus(WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CFont       m_font;
	int m_nFontAverageWidth;

private:
	static UINT m_uBitmapIds[];
	static UINT m_uFormat[];
};

class CEditToolBar : public CPmwToolBar
{
public:
	CEditToolBar();

	virtual const UINT* GetFormatIds(void) = 0;
	virtual void SetFormat(void);

protected:
	virtual void DrawBackground(CDC& dc, const CRect& crBackground, const CSize& czOffset);
	virtual void DrawSeparator(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState);
};

class CEdit1ToolBar : public CEditToolBar
{
public:
	CEdit1ToolBar();

	BOOL Create(CWnd* pParentWnd, DWORD dwStyle);

	virtual const UINT* GetBitmapIds(void);
	virtual const UINT* GetFormatIds(void);


private:
#ifdef LOCALIZE
	int SetupFormatIds(void);
#endif

	static UINT m_uBitmapIds[];
	static UINT m_uFormat[];
};

class CEdit2ToolBar : public CEditToolBar
{
public:
	CEdit2ToolBar();

	BOOL Create(CWnd* pParentWnd, DWORD dwStyle);

	virtual const UINT* GetBitmapIds(void);
	virtual const UINT* GetFormatIds(void);

private:

#ifdef LOCALIZE
	int SetupFormatIds(void);
#endif

	static UINT m_uBitmapIds[];
	static UINT m_uFormat[];
};

#endif
