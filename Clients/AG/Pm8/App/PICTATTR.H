/*
// $Workfile: PICTATTR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PICTATTR.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:06   Fred
// Initial revision.
// 
//    Rev 1.1   29 Jul 1996 10:14:48   Jay
// Picture color -> picture attributes
// 
//    Rev 1.1   28 Jul 1996 10:19:52   Jay
// Text color, new color dialog
*/

#ifndef __PICTATTR_H__
#define __PICTATTR_H__

#include "pmwdlg.h"
#include "colorsel.h"

/////////////////////////////////////////////////////////////////////////////
// CPictureAttrDlg dialog

class CPictureAttrDlg : public CPmwDialog
{
// Construction
public:
	CPictureAttrDlg(GraphicObjectPtr object, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPictureAttrDlg)
	enum { IDD = IDD_PICTURE_ATTRIBUTES };
	BOOL	m_coloring_book;
	int		m_render_method;
	//}}AFX_DATA
	GraphicObjectPtr m_object;
	BOOL m_outline;
	COLOR m_ChosenColor;
// Implementation
protected:
	VOID enable_window(int id, BOOL enable);

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CColorSelectComboBox m_Color;

	// Generated message map functions
	//{{AFX_MSG(CPictureAttrDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedColorMore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
