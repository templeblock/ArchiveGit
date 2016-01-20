/*
// $Workfile: UPGDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/UPGDLG.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:00   Fred
// Initial revision.
// 
//    Rev 1.1   15 Oct 1996 09:16:34   Fred
// Country selected from upgrade dialog
// 
//    Rev 1.0   08 Sep 1996 09:41:08   Fred
// Initial revision.
// 
//    Rev 1.1   01 Mar 1996 13:44:02   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:59:10   JAY
// Initial revision.
// 
//    Rev 1.18   18 Dec 1995 10:31:22   FRED
// Updates order form tax stuff along with reginfo
// 
//    Rev 1.17   05 Dec 1995 12:11:38   FRED
// Order by Modem
// 
//    Rev 1.16   29 Nov 1995 10:25:42   JAY
// Electronic Registration
// 
//    Rev 1.15   27 Feb 1995 12:26:32   FRED
// Hooked up Entire Library check box.
// 
//    Rev 1.14   24 Feb 1995 14:34:56   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.13   19 Jan 1995 14:21:10   JAY
// New free pictures!
// 
//    Rev 1.12   12 Jan 1995 16:53:58   JAY
// Now with sound!
// 
//    Rev 1.11   10 Jan 1995 14:25:02   JAY
// Order state reflected back into order dialog.
// 
//    Rev 1.10   15 Dec 1994 13:09:26   FRED
//  
// 
//    Rev 1.9   13 Dec 1994 18:28:46   FRED
//  
// 
//    Rev 1.8   11 Dec 1994 10:57:50   FRED
//  
// 
//    Rev 1.7   02 Dec 1994 08:37:56   FRED
//  
// 
//    Rev 1.6   01 Dec 1994 14:01:28   FRED
//  
// 
//    Rev 1.5   11 Nov 1994 16:52:32   FRED
//  
// 
//    Rev 1.4   08 Nov 1994 18:28:20   FRED
//  
*/

class COrderFormDialog;

#ifndef __UPGDLG_H__
#define __UPGDLG_H__

#include "salestax.h"
#include "orderlb.h"
#include "mspacket.h"
#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CUpgradeDialog dialog

class CUpgradeDialog : public CBaseDialog
{
// Construction
public:
	CUpgradeDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUpgradeDialog)
	enum { IDD = IDD_UPGRADE };
	CSalesTaxComboBox	m_TaxList;
	CCountryList m_CountryList;
	//}}AFX_DATA

	void UpdateDialog(void);
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CUpgradeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnOrderbymail();
	afx_msg void OnOrderbyphone();
	afx_msg void OnTaxCheck();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelchangeProductList();
	afx_msg void OnOrderbymodem();
	afx_msg void OnSelchangeTaxList();
	afx_msg void OnSelchangeCountryList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void OnInstall(void);

//	virtual void Start(void);

	void DrawPrice(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
	void TotalItems(COrderListBox* pList, long& lPriceTotal);
	void AdjustLicensed(void);
	
	void AddCatalogOrderLineItems(CMsregPacketCatalogOrderRequest* pData, CProductItems* pItems);

	COrderListBox m_ProductList;

	int m_nTaxIndex;

public:
	void SetTaxIndex(int nTaxIndex);
	int GetTaxIndex(void)
		{ return m_nTaxIndex; }
	void UpdateForCurrentCountry(void);
};

#endif
