/*
// $Workfile: CWMFCTL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CWMFCTL.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     12/12/97 11:24a Jayn
// Fixed Class Wizard problem.
// 
//    Rev 1.0   14 Aug 1997 15:19:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:46   FRED
// Initial revision.
// 
//    Rev 1.1   01 Jul 1995 09:27:58   FRED
// More "Make Your Own" stuff.
// 
//    Rev 1.0   30 Jun 1995 13:26:40   FRED
// Initial revision.
*/ 

#ifndef _CWMFCTL_H_
#define _CWMFCTL_H_

#include "cwmf.h"

class CWindowsMetaFileControl;

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
	//{{AFX_MSG(CWindowsMetaFileControl)
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
	BOOL m_fShapeShadow;
	COLORREF m_clShapeShadowColor;
	
	CWindowsMetaFile* m_pMetaFile;
	
	CString m_csResourceName;
	
	BOOL CollectWord(CString& csText, CString& csWord);
	BOOL CollectName(CString& csText, CString& csName);
	BOOL CollectAlignmentFlags(CString& csText, int& nAlignmentFlags, BOOL fUseDefault = TRUE, int nDefault = ASPECT_PRESERVE|HORIZONTAL_ALIGN_CENTER|VERTICAL_ALIGN_MIDDLE);
	BOOL CollectShape(CString& csText, int& nShape, BOOL fUseDefault = TRUE, int nDefault = SHAPE_SQUARE);
	BOOL CollectColor(CString& csText, COLORREF& clColor, BOOL fUseDefault = TRUE, COLORREF clDefault = RGB(0,0,0));
	BOOL CollectType(CString& csText, int& nType, BOOL fUseDefault = TRUE, int nDefault = TYPE_SHAPE);
	
	void AlignBounds(CRect& crBounds);
	void DrawShape(CDC* pDC, int nShape, CRect* pcrBounds, COLORREF clColor);
};

#endif
