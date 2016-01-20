/*
// $Workfile: CWMFCTL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CWMFCTL.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:38   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:48   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:34   JAY
// Initial revision.
// 
//    Rev 1.0   22 Nov 1995 08:51:54   FRED
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:50   FRED
//  
// 
//    Rev 1.0   25 Oct 1995 10:06:18   JAY
// Initial revision.
*/ 

class CMlsPropertyString;
class CWindowsMetaFileControl;

#ifndef __CWMFCTL_H__
#define __CWMFCTL_H__

#ifdef USEWMF
#include "cwmf.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindowsMetaFileControl window

class CWindowsMetaFileControl : public CWnd
{
// Construction
public:
	CWindowsMetaFileControl();

// Implementation
public:
	virtual ~CWindowsMetaFileControl();

protected:
	// Generated message map functions
	//{{AFX_MSG(CAlbumControl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// stuff for supporting creation by class name
public:
	// Member function to register a window class
	static BOOL RegisterMyClass();

protected:
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }

private:
	static LONG CALLBACK AFX_EXPORT FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

	BOOL m_fCreatedByClassName;
	virtual void PostNcDestroy();
// Implementation
protected:
	BOOL Unpack(void);
	BOOL m_fUnpacked;

	enum
	{
		ASPECT_SHIFT				=	0,
		ASPECT_WIDTH				=	1,
		ASPECT_PRESERVE			=	(0<<ASPECT_SHIFT),
		ASPECT_STRETCH				=	(1<<ASPECT_SHIFT),
		ASPECT_MASK					=	(((1<<ASPECT_WIDTH)-1)<<ASPECT_SHIFT),

		HORIZONTAL_ALIGN_SHIFT	=	ASPECT_SHIFT+ASPECT_WIDTH,
		HORIZONTAL_ALIGN_WIDTH	=	2,
		HORIZONTAL_ALIGN_LEFT	=	(0<<HORIZONTAL_ALIGN_SHIFT),
		HORIZONTAL_ALIGN_CENTER	=	(1<<HORIZONTAL_ALIGN_SHIFT),
		HORIZONTAL_ALIGN_RIGHT	=	(2<<HORIZONTAL_ALIGN_SHIFT),
		HORIZONTAL_ALIGN_MASK	=	(((1<<HORIZONTAL_ALIGN_WIDTH)-1)<<HORIZONTAL_ALIGN_SHIFT),

		VERTICAL_ALIGN_SHIFT		=	HORIZONTAL_ALIGN_SHIFT+HORIZONTAL_ALIGN_WIDTH,
		VERTICAL_ALIGN_WIDTH		=	2,
		VERTICAL_ALIGN_TOP		=	(0<<VERTICAL_ALIGN_SHIFT),
		VERTICAL_ALIGN_MIDDLE	=	(1<<VERTICAL_ALIGN_SHIFT),
		VERTICAL_ALIGN_BOTTOM	=	(2<<VERTICAL_ALIGN_SHIFT),
		VERTICAL_ALIGN_MASK		=	(((1<<VERTICAL_ALIGN_WIDTH)-1)<<VERTICAL_ALIGN_SHIFT),
	};

	int m_nAlignmentFlags;

	int Aspect(void)
		{ return m_nAlignmentFlags & ASPECT_MASK; }
	int HorizontalAlignment(void)
		{ return m_nAlignmentFlags & HORIZONTAL_ALIGN_MASK; }
	int VerticalAlignment(void)
		{ return m_nAlignmentFlags & VERTICAL_ALIGN_MASK; }

	enum
	{
		TYPE_SHAPE	=	0,
		TYPE_WMF		=	1,
		TYPE_BMP		=	2
	};

	int m_nType;

	enum
	{
		SHAPE_SQUARE			=	0,
		SHAPE_CIRCLE			=	1,
		SHAPE_TRIANGLEUP		=	2,
		SHAPE_TRIANGLEDOWN	=	3,
		SHAPE_TRIANGLELEFT	=	4,
		SHAPE_TRIANGLERIGHT	=	5
	};

	int m_nShape;
	COLORREF m_clShapeColor;

	COLORREF m_clStrokeColor;
	int m_nStrokeWidth;

	BOOL m_fShapeShadow;
	COLORREF m_clShapeShadowColor;

#ifdef USEWMF
	CWindowsMetaFile* m_pMetaFile;
#endif

	CString m_csResourceName;

	BOOL CollectName(CMlsPropertyString& Properties, CString& csName);
	BOOL CollectAlignmentFlags(CMlsPropertyString& Properties, int& nAlignmentFlags, BOOL fUseDefault = TRUE, int nDefault = ASPECT_PRESERVE|HORIZONTAL_ALIGN_CENTER|VERTICAL_ALIGN_MIDDLE);
	BOOL CollectShape(CMlsPropertyString& Properties, int& nShape, BOOL fUseDefault = TRUE, int nDefault = SHAPE_SQUARE);
	BOOL CollectType(CMlsPropertyString& Properties, int& nType, BOOL fUseDefault = TRUE, int nDefault = TYPE_SHAPE);

	void AlignBounds(CRect& crBounds);
	void DrawShape(CDC* pDC, int nShape, CRect* pcrBounds, COLORREF clColor, COLORREF clStroke = COLORREF(-1), int nStrokeWidth = 0);
};

#endif
