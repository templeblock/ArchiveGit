/*
// $Workfile: DRAWDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DRAWDLG.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:27:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:58   FRED
// Initial revision.
// 
//    Rev 1.3   18 Aug 1995 11:46:26   FRED
// Line attributes
// 
//    Rev 1.2   17 May 1995 16:57:04   FRED
// Draw object shadows
// 
//    Rev 1.1   16 May 1995 12:00:20   JAY
// 
//    Rev 1.0   10 May 1995 08:01:38   JAY
// Initial revision.
*/

#ifndef __DRAWDLG_H__
#define __DRAWDLG_H__

#include "pmwdlg.h"
#include "colorsel.h"
#include "widthsel.h"

class DrawingObject;

/////////////////////////////////////////////////////////////////////////////
// CDrawingAttributesDialog dialog

class CDrawingAttributesDialog : public CPmwDialog
{
// Construction
public:
	CDrawingAttributesDialog(DrawingObject* pObject, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDrawingAttributesDialog)
	enum { IDD = IDD_DRAWING_OBJECT_ATTRIBUTES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CDrawingAttributesDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnFillColorCustom();
	afx_msg void OnLineColorCustom();
	afx_msg void OnLineWidthCustom();
	afx_msg void OnShadowColorCustom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CColorSelectComboBox m_FillColor;
	CWidthSelectComboBox m_LineWidth;
	CColorSelectComboBox m_LineColor;
	CColorSelectComboBox m_ShadowColor;

	DrawingObject* m_pObject;
};

/////////////////////////////////////////////////////////////////////////////
// CLineAttributesDialog dialog

class CLineAttributesDialog : public CPmwDialog
{
// Construction
public:
	CLineAttributesDialog(DrawingObject* pObject, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLineAttributesDialog)
	enum { IDD = IDD_LINE_ATTRIBUTES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLineAttributesDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnLineColorCustom();
	afx_msg void OnLineWidthCustom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CWidthSelectComboBox m_LineWidth;
	CColorSelectComboBox m_LineColor;

	DrawingObject* m_pObject;
};
#endif
