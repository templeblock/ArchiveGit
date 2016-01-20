/*
// $Workfile: PHONEORD.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PHONEORD.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:26   Fred
// Initial revision.
// 
//    Rev 1.3   28 Jul 1997 15:02:16   Fred
// Secret Code!
// 
//    Rev 1.2   09 Jul 1997 09:55:24   Fred
// Moved strings to resource file
// 
//    Rev 1.1   14 Oct 1996 13:34:40   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:36   Fred
// Initial revision.
// 
//    Rev 1.1   01 Mar 1996 13:44:00   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:58:16   JAY
// Initial revision.
// 
//    Rev 1.26   07 Dec 1995 08:03:50   FRED
// Registers and Bonus Enables user
// 
//    Rev 1.25   10 Apr 1995 14:49:04   FRED
// Puts non-toll number in dialog.
// 
// 
//    Rev 1.24   27 Feb 1995 12:48:36   FRED
// Removed some unused code.
// 
// 
//    Rev 1.23   24 Feb 1995 14:36:14   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.22   20 Jan 1995 10:49:08   JAY
// Touch-ups
// 
//    Rev 1.21   19 Jan 1995 14:20:48   JAY
// New free pictures!
// 
//    Rev 1.20   10 Jan 1995 10:49:00   FRED
// Does not use "".
// 
// 
//    Rev 1.19   15 Dec 1994 09:33:20   FRED
// 
//    Rev 1.18   14 Dec 1994 17:20:18   FRED
//  
// 
//    Rev 1.17   13 Dec 1994 18:29:22   FRED
//  
// 
//    Rev 1.16   06 Dec 1994 14:35:02   JAY
// Uses member variable.
// 
//    Rev 1.15   06 Dec 1994 13:39:02   FRED
//  
// 
//    Rev 1.14   06 Dec 1994 12:00:30   FRED
//  
// 
//    Rev 1.13   06 Dec 1994 10:43:02   JAY
//  
// 
//    Rev 1.12   05 Dec 1994 14:13:52   JAY
//  
// 
//    Rev 1.11   05 Dec 1994 11:38:28   JAY
//  
// 
//    Rev 1.10   05 Dec 1994 11:33:38   FRED
// 
//    Rev 1.9   02 Dec 1994 14:51:18   FRED
//  
// 
//    Rev 1.8   02 Dec 1994 11:45:04   JAY
//  
// 
//    Rev 1.7   17 Nov 1994 14:30:18   JAY
//  
// 
//    Rev 1.6   11 Nov 1994 17:03:40   JAY
// LAC stuff now in place.
// 
//    Rev 1.5   10 Nov 1994 14:59:58   JAY
//  
// 
//    Rev 1.4   10 Nov 1994 08:09:18   JAY
//  
// 
//    Rev 1.3   09 Nov 1994 17:23:36   JAY
//  
// 
//    Rev 1.2   09 Nov 1994 17:16:38   JAY
//  
// 
//    Rev 1.1   08 Nov 1994 18:28:18   FRED
//  
*/

// phoneord.cpp : implementation file
//

#include "stdafx.h"
#include "upgrade.h"
#include "cinstall.h"
#include "util.h"
#include "salestax.h"
#include "phoneord.h"
#include "upgdlg.h"
#include "msregini.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COrderByPhoneDialog dialog

COrderByPhoneDialog::COrderByPhoneDialog(CInstallInfo* pInfo, CWnd* pParent /*=NULL*/)
	: CBaseDialog(COrderByPhoneDialog::IDD, pParent)
{
	m_pInfo = pInfo;
	//{{AFX_DATA_INIT(COrderByPhoneDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pOrderForm = NULL;
	m_nTaxIndex = -1;
}

void COrderByPhoneDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrderByPhoneDialog)
	DDX_Control(pDX, IDC_TAX_LIST, m_TaxList);
	DDX_Control(pDX, IDC_ACCESS_CODE, m_CodeCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COrderByPhoneDialog, CBaseDialog)
	//{{AFX_MSG_MAP(COrderByPhoneDialog)
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_TAX_LIST, OnSelchangeTaxList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COrderByPhoneDialog message handlers

BOOL COrderByPhoneDialog::OnInitDialog()
{
	m_ProductList.SubclassDlgItem(IDC_PRODUCT_LIST, this);
	m_OfferCode.SubclassDlgItem(IDC_OFFER_CODE, this);
	m_RequestCode.SubclassDlgItem(IDC_REQUEST_CODE, this);

	CenterWindow();
	
	CBaseDialog::OnInitDialog();

	CMlsPropertyControl::SubclassPropertyControls(this);

	m_TaxList.Init(m_nTaxIndex);

	COfferInformation* pOffer = m_pInfo->m_pOffer;

	m_ProductList.AddProducts(pOffer);
	
	m_CodeCtrl.LimitText(12);
	
	// Initialize the tax and total information.
	
	SetDlgItemText(IDC_TAX, "");
	SetDlgItemText(IDC_FEE, "");
	SetDlgItemText(IDC_TOTAL, "");
	
	// Get the License Request Code(s).

	m_pInfo->GetLRCFromProducts(m_csLRC);
	
	SetDlgItemText(IDC_OFFER_CODE, pOffer->OfferCode());
	SetDlgItemText(IDC_REQUEST_CODE, m_csLRC);
	
	// Fill in the tax and total fields.

	UpdateDialog();

	CWnd* pFocus = GetDlgItem(IDC_ACCESS_CODE);
	
	if (pFocus != NULL)
	{
		pFocus->SetFocus();
	}

	return pFocus == NULL;
}

void COrderByPhoneDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ((nIDCtl == IDC_TAX) || (nIDCtl == IDC_FEE) || (nIDCtl == IDC_TOTAL))
	{
		if ((lpDrawItemStruct->itemAction & ODA_DRAWENTIRE) != 0)
		{
			DrawPrice(lpDrawItemStruct);
		}
	}
	
	else
	{
		CBaseDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

void COrderByPhoneDialog::DrawPrice(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int nContext = 0;
	
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	if (pDC != NULL)
	{
		if ((nContext = pDC->SaveDC()) != 0)
		{
			CRect crItem(lpDrawItemStruct->rcItem);

			Util::DrawClientBorder(*pDC, crItem);

			CBrush cbBackground(GetSysColor(COLOR_WINDOW));
			pDC->FillRect(crItem, &cbBackground);
			
			CWnd* pWindow = GetDlgItem(lpDrawItemStruct->CtlID);
			CRect crWindow;
			pWindow->GetWindowRect(crWindow);
			
			CWnd* pPriceAlign = GetDlgItem(IDC_PRICE_ALIGN);
			CRect crPriceAlign;
			pPriceAlign->GetWindowRect(crPriceAlign);
			
			char szPrice[32];
			GetDlgItemText(lpDrawItemStruct->CtlID, szPrice, sizeof(szPrice));
			CSize czExtent = pDC->GetTextExtent(szPrice, lstrlen(szPrice));
			int nPriceAlign = crPriceAlign.left-crWindow.left-czExtent.cx;
			int nTopAlign = crItem.top+(crItem.Height()-czExtent.cy)/2;
			
			pDC->SetBkMode(TRANSPARENT);
			pDC->TextOut(nPriceAlign, nTopAlign, szPrice, lstrlen(szPrice));
			
			pDC->RestoreDC(nContext);
		}
	}
}

void COrderByPhoneDialog::UpdateDialog()
{
	// Figure out what items are marked and the order subtotal.
	
	long lPriceTotal = 0;
	
	TotalItems(&m_ProductList, lPriceTotal);
	
	long lTax = m_TaxList.CurrentTax(lPriceTotal);

	lPriceTotal += lTax;
	
	CString csTax;
	Util::PriceString(csTax, lTax, TRUE, "");
	
	long lFee = GetApp()->Offer()->ProcessingFee();
	
	CString csFee;
	Util::PriceString(csFee, lFee, TRUE, "");
	
	lPriceTotal += lFee;
	
	CString csTotal;
	Util::PriceString(csTotal, lPriceTotal);

	SetDlgItemText(IDC_TAX, csTax);
	SetDlgItemText(IDC_FEE, csFee);
	SetDlgItemText(IDC_TOTAL, csTotal);
}

void COrderByPhoneDialog::TotalItems(CPhoneListBox* pList, long& lPriceTotal)
{
	if (pList != NULL)
	{
		int nCount = pList->GetCount();
		
		if ((nCount > 0) && (nCount != LB_ERR))
		{
			for (int nIndex = 0; nIndex < nCount; nIndex++)
			{
				int nSelected = pList->GetSel(nIndex);
				
				if ((nSelected != LB_ERR) && (nSelected > 0))
				{
					CProductItem* pItem = pList->GetItem(nIndex);
											
					if (pItem != NULL)
					{
						lPriceTotal += pItem->Price();
					}
				}
			}
		}
	}
}

void COrderByPhoneDialog::OnSelchangeTaxList()
{
	m_nTaxIndex = m_TaxList.GetCurSel();
	UpdateDialog();
	if (m_pOrderForm != NULL)
	{
		m_pOrderForm->SetTaxIndex(m_nTaxIndex);
	}	
}

void COrderByPhoneDialog::OnOK()
{
	CString csText;

	m_CodeCtrl.GetWindowText(csText);

	// Prepend the LRC onto this to make the code we want to validate.

	CString csCode = m_csLRC + '-' + csText;

	if ((GetApp()->InstallationData()->ValidateLicenseAccessCode(csCode))
	 || (csText == "XYZZY"))
	{
		// Make sure they're registered.
		GetApp()->UserIniFile().WriteInteger(GetApp()->ApplicationName(), MSREGUSRINI_Application_Registered, 1);

		// "Bonus Enable" them.
		GetApp()->UserIniFile().WriteString(GetApp()->ApplicationName(), MSREGUSRINI_Application_RegistrationCode, MODEM_REGISTRATION_CODE);

		CBaseDialog::OnOK();
	}
	else
	{
		AfxMessageBox(IDS_ENTER_VALID_ACCESS_CODE);
	}
}

void COrderByPhoneDialog::SetTaxIndex(int nTaxIndex)
{
	m_nTaxIndex = nTaxIndex;
	if (m_TaxList.GetSafeHwnd() != NULL)
	{
		m_TaxList.SetCurrentTaxIndex(m_nTaxIndex);
	}
	if (m_hWnd != NULL)
	{
		UpdateDialog();
	}
}
