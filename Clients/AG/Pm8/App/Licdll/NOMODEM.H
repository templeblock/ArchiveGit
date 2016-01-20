/*
// $Workfile: NOMODEM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/NOMODEM.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:30   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:48   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/NOMODEM.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:56   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:38   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:44   JAY
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:52   FRED
//  
*/ 

// nomodem.h : header file
//

class CNoModemDialog;

#ifndef __NOMODEM_H__
#define __NOMODEM_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNoModemDialog dialog

class CNoModemDialog : public CBaseDialog
{
// Construction
public:
	CNoModemDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNoModemDialog)
	enum { IDD = IDD_NO_MODEM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoModemDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNoModemDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
