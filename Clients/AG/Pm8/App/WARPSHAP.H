/*
// $Workfile: WARPSHAP.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPSHAP.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:37:00   FRED
// Initial revision.
// 
//    Rev 1.7   03 Sep 1995 17:58:00   FRED
// Warp text dialog works with new tabbed dialog code
// 
//    Rev 1.6   02 Sep 1995 15:04:54   FRED
// More changes to make help work
// 
//    Rev 1.5   16 Jul 1995 14:53:46   FRED
// Destructor is virtual
// 
//    Rev 1.4   13 May 1995 11:10:26   FRED
// More warp preview stuff
// 
//    Rev 1.3   12 May 1995 14:55:26   FRED
// More warp effects preview stuff
// 
//    Rev 1.2   12 May 1995 10:35:16   FRED
// Warp text preview
// 
//    Rev 1.1   28 Apr 1995 13:50:24   FRED
// 
//    Rev 1.0   27 Apr 1995 13:30:04   FRED
// Initial revision.
*/

class CWarpTextTextPage;

#ifndef __WARPSHAP_H__
#define __WARPSHAP_H__

class WarpTextObject;
class CPmwView;

#include "warpsel.h"
#include "warpfile.h"
#include "pmwprop.h"

/////////////////////////////////////////////////////////////////////////////
// CWarpTextShapePage dialog

class CWarpTextShapePage : public CPmwPropertyPage
{
// Construction
public:
	CWarpTextShapePage(CPmwView* pView);	// standard constructor
	virtual ~CWarpTextShapePage();

// Dialog Data
	//{{AFX_DATA(CWarpTextShapePage)
	enum { IDD = IDD_WARP_SHAPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	void Object(WarpTextObject* pObject);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CWarpTextShapePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeShapeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	CWarpSelectListBox m_WarpShape;
	WarpTextObject* m_pObject;
	HWND m_hPreview;
	CWarpFile m_WarpFile;
	void UpdatePreview(void);
};

#endif
