/*
// $Workfile: WARPFX.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPFX.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:56   FRED
// Initial revision.
// 
//    Rev 1.9   02 Sep 1995 15:06:12   FRED
// More changes to make help work
// 
//    Rev 1.8   21 Aug 1995 09:34:46   FRED
// Fixed bug
// 
//    Rev 1.7   19 Aug 1995 17:49:28   FRED
// Maximum line sizes
// 
//    Rev 1.6   16 Jul 1995 14:53:46   FRED
// Destructor is virtual
// 
//    Rev 1.5   12 May 1995 14:55:26   FRED
// More warp effects preview stuff
// 
//    Rev 1.4   12 May 1995 10:34:34   FRED
// Warp text preview
// 
//    Rev 1.3   10 May 1995 12:33:26   FRED
// Shadows for warp text objects.
// 
// 
//    Rev 1.2   03 May 1995 10:41:48   FRED
//  colorsel.cpp
// 
//    Rev 1.1   02 May 1995 15:37:54   FRED
// 
//    Rev 1.0   27 Apr 1995 13:30:08   FRED
// Initial revision.
*/

class CWarpTextEffectsPage;

#ifndef __WARPFX_H__
#define __WARPFX_H__

#include "colorsel.h"
#include "widthsel.h"
#include "pmwprop.h"

class WarpTextObject;
class CPmwView;

/////////////////////////////////////////////////////////////////////////////
// CWarpTextEffectsPage dialog

class CWarpTextEffectsPage : public CPmwPropertyPage
{
// Construction
public:
	CWarpTextEffectsPage(CPmwView* pView);	// standard constructor
	virtual ~CWarpTextEffectsPage();

// Dialog Data
	//{{AFX_DATA(CWarpTextEffectsPage)
	enum { IDD = IDD_WARP_EFFECTS };
	//}}AFX_DATA

public:
	void Object(WarpTextObject* pObject);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CWarpTextEffectsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnTextFillColorCustom();
	afx_msg void OnTextLineWidthCustom();
	afx_msg void OnTextLineColorCustom();
	afx_msg void OnTextShadowColorCustom();
	afx_msg void OnBackgroundFillColorCustom();
	afx_msg void OnBackgroundLineWidthCustom();
	afx_msg void OnBackgroundLineColorCustom();
	afx_msg void OnBackgroundShadowColorCustom();
	afx_msg void OnSelchangeBackgroundFillColor();
	afx_msg void OnSelchangeBackgroundLineWidth();
	afx_msg void OnSelchangeBackgroundLineColor();
	afx_msg void OnSelchangeBackgroundShadowColor();
	afx_msg void OnSelchangeTextFillColor();
	afx_msg void OnSelchangeTextLineWidth();
	afx_msg void OnSelchangeTextLineColor();
	afx_msg void OnSelchangeTextShadowColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	CColorSelectComboBox m_TextFillColor;
	CWidthSelectComboBox m_TextLineWidth;
	CColorSelectComboBox m_TextLineColor;
	CColorSelectComboBox m_TextShadowColor;
	CColorSelectComboBox m_BackgroundFillColor;
	CWidthSelectComboBox m_BackgroundLineWidth;
	CColorSelectComboBox m_BackgroundLineColor;
	CColorSelectComboBox m_BackgroundShadowColor;
	WarpTextObject* m_pObject;
	HWND m_hPreview;
	void UpdatePreview(void);

public:	
	int m_nInitialTextLineWidth;
	int m_nTextLineWidth;
	int m_nInitialBackgroundLineWidth;
	int m_nBackgroundLineWidth;
};

#endif
