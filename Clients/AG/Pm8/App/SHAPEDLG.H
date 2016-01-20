/*
// $Workfile: SHAPEDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SHAPEDLG.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:52   Fred
// Initial revision.
// 
//    Rev 1.1   28 Jan 1997 14:10:42   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.0   14 Mar 1996 13:30:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:14   FRED
// Initial revision.
// 
//    Rev 1.2   18 May 1995 13:20:56   FRED
// Shape file/dialog stuff
// 
//    Rev 1.1   16 May 1995 12:00:00   JAY
// 
//    Rev 1.0   10 May 1995 15:17:44   JAY
// Initial revision.
*/

#ifndef __SHAPEDLG_H__
#define __SHAPEDLG_H__

#include "pmwdlg.h"
#include "shapesel.h"
#include "shapfile.h"

/////////////////////////////////////////////////////////////////////////////
// CShapeDialog dialog

class CShapeDialog : public CPmwDialog
{
	INHERIT(CShapeDialog, CPmwDialog)

// Construction
public:
	CShapeDialog(CShapeFile* pFile, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CShapeDialog)
	enum { IDD = IDD_SHAPE_DIALOG };
	CShapeSelectListBox m_List;
	int m_nChosen;
	//}}AFX_DATA

	CShapeFile* m_pFile;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CShapeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkShapes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void BuildShapeList(void);
};

#endif
