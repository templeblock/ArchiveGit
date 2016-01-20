/*
// $Header: /PM8/App/FMTBAR.H 1     3/03/99 6:05p Gbeddow $
//
// barcombo.h : Combo box and edit classes for use on dialog bar.
//
// $Log: /PM8/App/FMTBAR.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:54   Fred
// Initial revision.
// 
//    Rev 1.3   13 Mar 1997 15:17:42   Jay
// Fixed format bar for warp text editing
// 
//    Rev 1.2   15 Jan 1997 10:49:38   Jay
// New toolbars
// 
//    Rev 1.1   23 Jul 1996 10:40:30   Jay
// Added newsletters and brochures; removed backgrounds
// 
//    Rev 1.0   14 Mar 1996 13:37:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:18   FRED
// Initial revision.
// 
//    Rev 1.5   04 Sep 1995 09:01:06   JAY
// Sound fixes
// 
//    Rev 1.4   22 Aug 1995 19:21:36   FRED
//  
// 
//    Rev 1.3   20 Aug 1995 15:40:52   FRED
// Rotate object control
// 
//    Rev 1.2   10 Aug 1995 17:06:18   JAY
// Added new format bars.
// 
//    Rev 1.1   05 May 1995 17:09:54   FRED
//  
// 
//    Rev 1.0   19 Apr 1995 12:38:04   JAY
// Initial revision.
*/

#ifndef __FMTBAR_H__
#define __FMTBAR_H__

#include "barcombo.h"
#include "pmwtool.h"

/////////////////////////////////////////////////////////////////////////////
// CFormatBar

class CFormatBar : public CPmwToolBar
{
public:
	CFormatBar();

	CBarCombo   m_TypefaceBox;
	CBarCombo   m_SizeBox;
	CBarCombo   m_RotateBox;
	CFont       m_font;
	
	int m_nFontAverageWidth;

	BOOL Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID);
	BOOL HasFocus(void);

	// Particular incarnations of the format bar.
	void ShowTextEdit(void);			// For text editing.
	void ShowWarpText(void);			// For warp text editing.
	void ShowBlank(void);				// When nothing applies.

	void ShowPicture(void);
	void ShowPictureNoRotate(void);
	void ShowTextBox(void);
	void ShowTitleTextBox(void);
	void ShowGroup(void);
	void ShowShape(void);
	void ShowMultiple(void);
	void ShowCalendarDate(void);

	void BuildTypefaceList(void);
	void BuildPointSizeList(void);
	void BuildRotateList(void);

	int GetDefaultHeight(void);

protected:
	// Generated message map functions
	//{{AFX_MSG(CFormatBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Helper functions.
	void SetFormat(UINT* pIds, int nIds);
	virtual const UINT* GetBitmapIds(void);
	static UINT m_uBitmapIds[];
};

/////////////////////////////////////////////////////////////////////////////
// CDialogFormatBar
//
// Use this class when you want to use a CFormatBar in a dialog. It overrides
// the OnIdleUpdateCmdUI method to route the update stuff to the format bar
// owner/parent rather than the main frame.

class CDialogFormatBar : public CFormatBar
{
public:
	CDialogFormatBar();
};
	
#endif					// #ifndef __FMTBAR_H__
