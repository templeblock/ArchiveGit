/*
// $Workfile: PHONEORD.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PHONEORD.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:52   Fred
// Initial revision.
// 
//    Rev 1.1   14 Oct 1996 13:34:52   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:41:00   Fred
// Initial revision.
// 
//    Rev 1.1   01 Mar 1996 13:43:58   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:58:46   JAY
// Initial revision.
// 
//    Rev 1.12   24 Feb 1995 14:34:48   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.11   19 Jan 1995 14:20:36   JAY
// New free pictures!
// 
//    Rev 1.10   13 Dec 1994 18:29:06   FRED
//  
// 
//    Rev 1.9   05 Dec 1994 14:13:40   JAY
//  
// 
//    Rev 1.8   02 Dec 1994 14:51:08   FRED
//  
// 
//    Rev 1.7   17 Nov 1994 14:30:14   JAY
//  
// 
//    Rev 1.6   11 Nov 1994 17:03:34   JAY
// LAC stuff now in place.
// 
//    Rev 1.5   10 Nov 1994 15:00:22   JAY
//  
// 
//    Rev 1.4   10 Nov 1994 08:09:34   JAY
//  
// 
//    Rev 1.3   09 Nov 1994 17:23:36   JAY
//  
// 
//    Rev 1.2   09 Nov 1994 17:16:40   JAY
//  
// 
//    Rev 1.1   08 Nov 1994 18:27:50   FRED
//  
*/

class COrderByPhoneDialog;

#ifndef __PHONEORD_H__
#define __PHONEORD_H__

#include "salestax.h"
#include "phonelb.h"
#include "basedlg.h"

class CUpgradeDialog;
class CInstallInfo;

/////////////////////////////////////////////////////////////////////////////
// COrderByPhoneDialog dialog

class COrderByPhoneDialog : public CBaseDialog
{
// Construction
public:
	COrderByPhoneDialog(CInstallInfo* pInfo, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(COrderByPhoneDialog)
	enum { IDD = IDD_ORDERBYPHONE };
	CSalesTaxComboBox	m_TaxList;
	CEdit	m_CodeCtrl;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(COrderByPhoneDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnOK();
	afx_msg void OnSelchangeTaxList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void DrawPrice(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void UpdateDialog(void);
	void TotalItems(CPhoneListBox* pList, long& lPriceTotal);
	
	CPhoneListBox m_ProductList;
	int m_nTaxIndex;
	
	CInstallInfo* m_pInfo;
	CString m_csLRC;

public:
	CUpgradeDialog* m_pOrderForm;
	
	void SetTaxIndex(int nTaxIndex);
	int GetTaxIndex(void)
		{ return m_nTaxIndex; }

protected:
	CReadOnlyControl m_OfferCode;
	CReadOnlyControl m_RequestCode;
};

#endif
