/*
// $Workfile: UPGDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/UPGDLG.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:34   Fred
// Initial revision.
// 
//    Rev 1.3   09 Jul 1997 09:55:30   Fred
// Moved strings to resource file
// 
//    Rev 1.2   15 Oct 1996 09:16:34   Fred
// Country selected from upgrade dialog
// 
//    Rev 1.1   14 Oct 1996 13:34:44   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:40   Fred
// Initial revision.
// 
//    Rev 1.3   01 Mar 1996 13:44:00   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.2   06 Feb 1996 10:38:32   FRED
// MicroLogic --> Mindscape.
// 
// 
//    Rev 1.1   05 Feb 1996 17:41:08   FRED
// Changes for upgrade
// 
//    Rev 1.0   02 Feb 1996 08:57:50   JAY
// Initial revision.
// 
//    Rev 1.59   22 Dec 1995 10:25:56   FRED
// Fixed bug where tax was not properly sent
// 
//    Rev 1.58   18 Dec 1995 10:31:24   FRED
// Updates order form tax stuff along with reginfo
// 
//    Rev 1.57   15 Dec 1995 17:58:50   FRED
// Forces tax on CA modem orders.
// 
//    Rev 1.56   07 Dec 1995 14:52:02   FRED
// Version
// 
//    Rev 1.55   07 Dec 1995 08:03:50   FRED
// Registers and Bonus Enables user
// 
//    Rev 1.54   05 Dec 1995 18:23:28   FRED
// Entire library index is 0 if CGL is not found
// 
//    Rev 1.53   05 Dec 1995 12:12:48   FRED
// Order by Modem
// 
//    Rev 1.52   30 Nov 1995 10:57:06   JAY
// Fixed a message in Modem section.
// 
//    Rev 1.51   29 Nov 1995 11:25:12   JAY
//  
// 
//    Rev 1.50   29 Nov 1995 10:25:28   JAY
// Electronic Registration
// 
//    Rev 1.49   01 Mar 1995 16:54:52   FRED
// 
//    Rev 1.48   28 Feb 1995 17:32:50   JAY
// Changed CDL to CGL.
// 
//    Rev 1.47   28 Feb 1995 14:30:26   JAY
// Now updates the product items at dialog update time.
// 
//    Rev 1.46   28 Feb 1995 09:13:44   FRED
// InitDialog() returns FALSE to not change focus.
// 
// 
//    Rev 1.45   27 Feb 1995 16:14:18   FRED
// Some fine tuning
// 
//    Rev 1.44   27 Feb 1995 12:26:18   FRED
// Hooked up Entire Library check box.
// 
//    Rev 1.43   24 Feb 1995 14:36:06   FRED
// Restructured product tree. Got rid of CProductForSale
// 
//    Rev 1.42   19 Jan 1995 14:20:24   JAY
// New free pictures!
// 
//    Rev 1.41   13 Jan 1995 13:47:20   FRED
//  
// 
//    Rev 1.40   12 Jan 1995 16:54:04   JAY
// Now with sound!
// 
//    Rev 1.39   10 Jan 1995 14:24:32   JAY
// Order state reflected back into order dialog.
// 
//    Rev 1.38   10 Jan 1995 10:48:44   FRED
// Does not use "discount".
// 
// 
//    Rev 1.37   04 Jan 1995 11:30:26   JAY
// Selective installation.
// 
//    Rev 1.36   21 Dec 1994 17:13:16   JAY
//  
// 
//    Rev 1.35   16 Dec 1994 17:51:44   FRED
//  
// 
//    Rev 1.34   15 Dec 1994 17:29:42   FRED
//  
// 
//    Rev 1.33   15 Dec 1994 14:46:44   JAY
// Selective install
// 
//    Rev 1.32   15 Dec 1994 13:09:02   FRED
//  
// 
//    Rev 1.31   15 Dec 1994 09:33:22   FRED
// 
//    Rev 1.30   14 Dec 1994 18:00:54   FRED
// 
//    Rev 1.29   14 Dec 1994 17:19:40   FRED
//  
// 
//    Rev 1.28   14 Dec 1994 16:25:20   FRED
// Updates product item flags.
// 
// 
//    Rev 1.27   14 Dec 1994 15:26:44   FRED
//  
// 
//    Rev 1.26   13 Dec 1994 18:28:58   FRED
//  
// 
//    Rev 1.25   11 Dec 1994 10:56:40   FRED
//  
// 
//    Rev 1.24   06 Dec 1994 14:35:16   JAY
// Install congrats dialog.
// 
//    Rev 1.23   06 Dec 1994 14:00:24   JAY
// Updates the tax selection back into the order form after phone or mail order.
// 
//    Rev 1.22   06 Dec 1994 12:00:16   FRED
//  
// 
//    Rev 1.21   06 Dec 1994 10:43:08   JAY
//  
// 
//    Rev 1.20   06 Dec 1994 09:02:16   FRED
//  
// 
//    Rev 1.19   05 Dec 1994 15:10:44   FRED
//  
// 
//    Rev 1.18   05 Dec 1994 11:38:30   JAY
//  
// 
//    Rev 1.17   02 Dec 1994 14:51:20   FRED
//  
// 
//    Rev 1.16   02 Dec 1994 11:45:10   JAY
//  
// 
//    Rev 1.15   02 Dec 1994 08:37:42   FRED
//  
// 
//    Rev 1.14   01 Dec 1994 14:01:46   FRED
//  
// 
//    Rev 1.13   22 Nov 1994 14:07:10   FRED
//  
// 
//    Rev 1.12   17 Nov 1994 14:30:20   JAY
//  
// 
//    Rev 1.11   11 Nov 1994 16:52:22   FRED
//  
// 
//    Rev 1.10   11 Nov 1994 14:42:54   JAY
// 
//    Rev 1.9   10 Nov 1994 17:45:18   JAY
//  
// 
//    Rev 1.8   10 Nov 1994 15:00:04   JAY
//  
// 
//    Rev 1.7   10 Nov 1994 08:09:28   JAY
//  
// 
//    Rev 1.6   09 Nov 1994 17:23:36   JAY
//  
// 
//    Rev 1.5   09 Nov 1994 17:16:40   JAY
//  
// 
//    Rev 1.4   08 Nov 1994 18:27:58   FRED
//  
*/

// cordrdlg.cpp : implementation file
//

#include "stdafx.h"
#include "upgrade.h"
#include "upgdlg.h"

#include "phoneord.h"
#include "mailord.h"
#include "cinstall.h"
#include "ownerlb.h"
#include "orderlb.h"
#include "prod.h"
#include "util.h"

#include "search.h"
#include "nomodem.h"
#include "reginfo.h"
#include "send.h"
#include "callfail.h"
#include "ordfail.h"
#include "congrats.h"
#include "mspacket.h"
#include "msregini.h"

#ifndef WIN32
#include "auxdata.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpgradeDialog dialog

CUpgradeDialog::CUpgradeDialog(CWnd* pParent /*=NULL*/)
		: CBaseDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CUpgradeDialog)
	//}}AFX_DATA_INIT
	m_nTaxIndex = -1;
}

void CUpgradeDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpgradeDialog)
	DDX_Control(pDX, IDC_TAX_LIST, m_TaxList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUpgradeDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CUpgradeDialog)
	ON_BN_CLICKED(IDC_ORDERBYMAIL, OnOrderbymail)
	ON_BN_CLICKED(IDC_ORDERBYPHONE, OnOrderbyphone)
	ON_WM_DRAWITEM()
	ON_LBN_SELCHANGE(IDC_PRODUCT_LIST, OnSelchangeProductList)
	ON_BN_CLICKED(IDC_ORDERBYMODEM, OnOrderbymodem)
	ON_CBN_SELCHANGE(IDC_TAX_LIST, OnSelchangeTaxList)
	ON_CBN_SELCHANGE(IDC_COUNTRY_LIST, OnSelchangeCountryList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CUpgradeDialog::UpdateDialog()
{
	m_ProductList.UpdateProductItems();

	// Figure out what items are marked and the order subtotal.
	
	long lPriceTotal = 0;
	long lSubTotal = 0;
	
	TotalItems(&m_ProductList, lPriceTotal);
	
	lSubTotal = lPriceTotal;
	
	CString csSubTotal;
	Util::PriceString(csSubTotal, lSubTotal, TRUE, "");
	
	long lTax = m_TaxList.CurrentTax(lPriceTotal);

	lPriceTotal += lTax;
	
	BOOL fEnable = lPriceTotal > 0;
	
	CString csTax;
	Util::PriceString(csTax, lTax, TRUE, "");
	
	long lFee = lSubTotal > 0 ? GetApp()->Offer()->ProcessingFee() : 0;
	
	CString csFee;
	Util::PriceString(csFee, lFee, TRUE, "");
	
	lPriceTotal += lFee;
	
	CString csTotal;
	Util::PriceString(csTotal, lPriceTotal);
	
	SetDlgItemText(IDC_SUBTOTAL, csSubTotal);
	SetDlgItemText(IDC_TAX, csTax);
	SetDlgItemText(IDC_FEE, csFee);
	SetDlgItemText(IDC_TOTAL, csTotal);
}

void CUpgradeDialog::TotalItems(COrderListBox* pList, long& lPriceTotal)
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

/////////////////////////////////////////////////////////////////////////////
// CUpgradeDialog message handlers

BOOL CUpgradeDialog::OnInitDialog()
{
	m_ProductList.SubclassDlgItem(IDC_PRODUCT_LIST, this);
	
	CBaseDialog::OnInitDialog();
	
	CenterWindow();

	m_CountryList.SubclassDlgItem(IDC_COUNTRY_LIST, this);
	m_CountryList.Fill(GetApp()->UserIniFile().GetInteger(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_CallingFrom, 0));
	
	UpdateForCurrentCountry();

	// Initialize the sales tax list. If there is a state recorded in MSREGUSR.INI,
	// then base the initial selection on that value.
	CString csState;
	int nSalesTaxIndex = -1;
	TRY
	{
		csState = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_State);
		nSalesTaxIndex = CSalesTaxComboBox::StateTaxIndex(csState);	
	}
	END_TRY
	m_TaxList.Init(nSalesTaxIndex);
	
	m_ProductList.AddProducts(GetApp()->Offer());

	SetDlgItemText(IDC_SUBTOTAL, "");
	SetDlgItemText(IDC_TAX, "");
	SetDlgItemText(IDC_FEE, "");
	SetDlgItemText(IDC_TOTAL, "");

	UpdateDialog();
	
	return TRUE;
}

void CUpgradeDialog::OnOrderbymail()
{
	// Update the product items with the selection flags from the list box.
	
	m_ProductList.UpdateProductItems();

	CInstallInfo Info;

	COrderByMailDialog Dialog(&Info, this);
	// We need to compute the various order totals so we can reflect them
	// in the order information dialog.
				
	long lTotal = 0;
	long lSubTotal = 0;
	
	TotalItems(&m_ProductList, lTotal);
	
	lSubTotal = lTotal;
	
	long lTax = m_TaxList.CurrentTax(lTotal);

	lTotal += lTax;

	long lShipping = lSubTotal > 0 ? GetApp()->Offer()->ProcessingFee() : 0;

	lTotal += lShipping;

	// Fill in values used by dialog.

	Dialog.m_lSubTotal = lSubTotal;
	Dialog.m_lTax = lTax;
	Dialog.m_lShipping = lShipping;
	Dialog.m_pOrderForm = this;
				
	if (Dialog.DoModal() == IDOK)
	{
		m_ProductList.DisableLicensedItems();
		UpdateDialog();
		OnInstall();
	}

	UpdateDialog();
}

void CUpgradeDialog::OnOrderbyphone()
{
	// Update the product items with the selection flags from the list box.
	
	m_ProductList.UpdateProductItems();
	
	if (GetApp()->Offer()->CountProductFlags(PRODUCT_ITEM_Selected) == 0)
	{
		AfxMessageBox(IDS_NO_ITEMS_SELECTED_PHONE);
		return;
	}

	CInstallInfo Info;

	COrderByPhoneDialog Dialog(&Info, this);
	Dialog.m_pOrderForm = this;
	Dialog.SetTaxIndex(m_TaxList.GetCurSel());
	if (Dialog.DoModal() == IDOK)
	{
		m_ProductList.DisableLicensedItems();
		UpdateDialog();
		OnInstall();
	}
	
	UpdateDialog();
}

void CUpgradeDialog::OnInstall()
{
// Congratulate them.

	CCongratulationsDialog CongratsDialog(this);
	CongratsDialog.DoModal();
								
// Successful transaction. They have upgraded.

	GetApp()->SetUpgradeResult(UPGRESULT_NowUpgraded);

	EndDialog(IDOK);
}

void CUpgradeDialog::OnTaxCheck()
{
	UpdateDialog();
}

void CUpgradeDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ((nIDCtl == IDC_SUBTOTAL)
	 || (nIDCtl == IDC_TAX)
	 || (nIDCtl == IDC_FEE)
	 || (nIDCtl == IDC_TOTAL))
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

void CUpgradeDialog::DrawPrice(LPDRAWITEMSTRUCT lpDrawItemStruct)
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
			
			CWnd* pPriceAlign = GetDlgItem(IDC_EXTENSION_ALIGN);
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

void CUpgradeDialog::OnSelchangeProductList()
{
	UpdateDialog();
}

void CUpgradeDialog::OnOrderbymodem()
{
	CString csReason;

	// Update the product items with the selection flags from the list box.
	m_ProductList.UpdateProductItems();
	
	if (GetApp()->Offer()->CountProductFlags(PRODUCT_ITEM_Selected) == 0)
	{
		AfxMessageBox(IDS_NO_ITEMS_SELECTED_MODEM);
		return;
	}

	// Build an CInstallInfo structure for us to use.
	CInstallInfo Info;

	// Perform the modem search.
	CSearchingDialog SearchDialog(this);

	switch (SearchDialog.DoModal())
	{
		case IDOK:
		{
			// User wants to send their order over the modem.
			int nPort = SearchDialog.m_nPort;
			long lSpeed = SearchDialog.m_lSpeed;
			BOOL fRetryRegister;
			
			int nPaymentMethod = 0;
			CString csCardNumber;
			CString csExpirationDate;

			do
			{
				fRetryRegister = FALSE;
            
				// Ask the user for some information.
				CRegistrationInformationDialog Dialog((UINT)-1, this);
				
				TRY
				{
					Dialog.m_fModemOrder = TRUE;
					Dialog.m_nPaymentMethod = nPaymentMethod;
					Dialog.m_csCardNumber = csCardNumber;
					Dialog.m_csExpirationDate = csExpirationDate;
				}
				END_TRY

				// We need to compute the various order totals so we can reflect them
				// in the order information dialog.
				
				long lTotal = 0;
				long lSubTotal = 0;
	
				TotalItems(&m_ProductList, lTotal);
	
				lSubTotal = lTotal;
	
				long lTax = m_TaxList.CurrentTax(lTotal);

				lTotal += lTax;
	
				long lShipping = lSubTotal > 0 ? Info.m_pOffer->ProcessingFee() : 0;
	
				lTotal += lShipping;

				// Fill in values used by RegInfo dialog.
				
				Dialog.m_lSubTotal = lSubTotal;
				Dialog.m_lTax = lTax;
				Dialog.m_lShipping = lShipping;
				Dialog.m_pOrderForm = this;
				
				if (Dialog.DoModal() == IDOK)
				{
					CMsregPacketHeader* pPacketHeader = NULL;
					CMsregPacketCatalogOrderRequest* pCatalogOrderData = NULL;
					CString csFileName;
					CFile cfFile;
					CString csWindowsDir;
					char* pTempName = NULL;
					BOOL fSuccess = FALSE;
					
					TRY
					{
						// Tax status may have changed, recompute some stuff.
						
						lTotal = lSubTotal;
	
						lTax = m_TaxList.CurrentTax(lTotal);

						lTotal += lTax;

						lShipping = lSubTotal > 0 ? Info.m_pOffer->ProcessingFee() : 0;

						lTotal += lShipping;
					
						nPaymentMethod = Dialog.m_nPaymentMethod;
						csCardNumber = Dialog.m_csCardNumber;
						csExpirationDate = Dialog.m_csExpirationDate;
						
						// Package the user's order into a packet.
						pCatalogOrderData = new CMsregPacketCatalogOrderRequest;

						// Product name.
						pCatalogOrderData->ProductName() = GetApp()->ApplicationName();
	
						// Product version.
						pCatalogOrderData->AddField("Version", GetApp()->MsRegIniFile().GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_Version));
	
						// Part Number.
						pCatalogOrderData->AddField("ParentPartNumber", GetApp()->MsRegIniFile().GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_ParentPartNumber));
					
						// User information.
						pCatalogOrderData->AddField("FirstName", Dialog.m_csFirstName);
						pCatalogOrderData->AddField("LastName", Dialog.m_csLastName);
						pCatalogOrderData->AddField("Company", Dialog.m_csCompany);
						pCatalogOrderData->AddField("Title", "");
						pCatalogOrderData->AddField("Address1", Dialog.m_csAddress1);
						pCatalogOrderData->AddField("Address2", Dialog.m_csAddress2);
						pCatalogOrderData->AddField("City", Dialog.m_csCity);
						pCatalogOrderData->AddField("State", Dialog.m_csState);
						pCatalogOrderData->AddField("Country", Dialog.m_csCountry);
						pCatalogOrderData->AddField("Zip", Dialog.m_csZip);
						pCatalogOrderData->AddField("Phone", Dialog.m_csPhone);
						pCatalogOrderData->AddField("Email", Dialog.m_csEmail);
						
						static char* PaymentMethodStrings[] =
						{
							"VSA",			// Visa
							"MC",				// MasterCard
							"AMX",			// American Express
							"DIS"				// Discover
						};
						
						ASSERT((Dialog.m_nPaymentMethod >= 0) && (Dialog.m_nPaymentMethod < (sizeof(PaymentMethodStrings)/sizeof(PaymentMethodStrings[0]))));
						
						if ((Dialog.m_nPaymentMethod >= 0) && (Dialog.m_nPaymentMethod < (sizeof(PaymentMethodStrings)/sizeof(PaymentMethodStrings[0]))))
						{
							pCatalogOrderData->AddField("PaymentMethod", PaymentMethodStrings[Dialog.m_nPaymentMethod]);
						}
						
						pCatalogOrderData->AddField("CardNumber", Dialog.m_csCardNumber);
						pCatalogOrderData->AddField("ExpirationDate", Dialog.m_csExpirationDate);
	
						// Tax, shipping, and total.
						pCatalogOrderData->Tax() = (DWORD)lTax;
						pCatalogOrderData->Shipping() = (DWORD)lShipping;
						pCatalogOrderData->Total() = (DWORD)lTotal;
						
						// Offer codes.
						pCatalogOrderData->OfferCode() = Info.m_pOffer->OfferCode();
						
						// Result codes.
						Info.GetLRCFromProducts(pCatalogOrderData->RequestCode());
						
						// Line Items.
						AddCatalogOrderLineItems(pCatalogOrderData, Info.m_pOffer);

						// Build packet header.					
						pPacketHeader = new CMsregPacketHeader;

						// Customer ID.
						pPacketHeader->CustomerId() = GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_CustomerNumber);
						
						// Packet.
						pPacketHeader->AddPacket(pCatalogOrderData);
						pCatalogOrderData = NULL;
						
						// Get the Windows directory in case we need to create a
						// temporary file there.
						if (GetWindowsDirectory(csWindowsDir.GetBuffer(_MAX_PATH), _MAX_PATH) == 0)
						{
							csWindowsDir.Empty();
						}
						else
						{
							csWindowsDir.ReleaseBuffer();
						}

						// Create a temporary file name.
						pTempName = _tempnam((LPSTR)(LPCSTR)csWindowsDir, "REG");
						if (pTempName != NULL)
						{
							// Delete the file 
							csFileName = pTempName;
							free(pTempName);
							pTempName = NULL;

							if (cfFile.Open(csFileName, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
							{
								// Write out the packet to the temp file.
								pPacketHeader->WriteToFile(cfFile);
								fSuccess = TRUE;
							}
						}
					}
					END_TRY

					// Clean up (fSuccess indicates whether we actually had success.)
					delete pPacketHeader;
					pPacketHeader = NULL;
							
					delete pCatalogOrderData;
					pCatalogOrderData = NULL;
							
					cfFile.Close();
							
					if (pTempName != NULL)
					{
						free(pTempName);
						pTempName = NULL;
					}
					
					// If we were not successful, then it was because of some internal problem
					// such as running out of memory or not being able to create the file.
					// Put up a message and let the user try again if they want.
					
					if (!fSuccess)
					{
						AfxMessageBox(IDS_COULD_NOT_SAVE_INFORMATION);
						fRetryRegister = TRUE;
					}
					else
					{
						// Attempt to send the file to the receiver.
						CSendingDialog SendingDialog(nPort, lSpeed, csFileName, this);

						switch (SendingDialog.DoModal())
						{
							case IDOK:
							{
								// The request was successfully transmitted, we should have
								// a response. Check if the order was successfully processed.

								DWORD dwResult = CMsregPacketCatalogOrderResponse::CATALOGORDERRESULT_Error;
									
								TRY
								{
									// Attempt to open the response file.
									if (!cfFile.Open(csFileName, CFile::modeRead|CFile::shareExclusive))
									{
										AfxThrowUserException();
									}

									// Read the response packet.
									pPacketHeader = new CMsregPacketHeader;
									pPacketHeader->ReadFromFile(cfFile);

									// Validate the response.
									CMsregPacketCatalogOrderResponse* pResponse;
									if ((pPacketHeader->Type() != CMsregPacket::BLOCK_PacketHeader)
									 || (pPacketHeader->Version() != CMsregPacket::VERSION_PacketHeader)
									 || (pPacketHeader->PacketCount() != 1)
									 || ((pResponse = (CMsregPacketCatalogOrderResponse*)(pPacketHeader->GetPacket(0))) == NULL)
									 || (pResponse->Type() != CMsregPacket::BLOCK_CatalogOrderResponse)
									 || (pResponse->Version() != CMsregPacket::VERSION_CatalogOrderResponse))
									{
										AfxThrowUserException();
									}
									
									// Get the result.
									dwResult = pResponse->Result();

									// Got a response, see if the order was successful.
									if (dwResult != CMsregPacketCatalogOrderResponse::CATALOGORDERRESULT_Ok)
									{
										AfxThrowUserException();
									}

									// Remember their customer ID.
									GetApp()->UserIniFile().WriteString(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_CustomerNumber, pPacketHeader->CustomerId());

									// Remember that they're registered.
									GetApp()->UserIniFile().WriteInteger(GetApp()->ApplicationName(), MSREGUSRINI_Application_Registered, 1);

									// "Bonus Enable" them.
									GetApp()->UserIniFile().WriteString(GetApp()->ApplicationName(), MSREGUSRINI_Application_RegistrationCode, MODEM_REGISTRATION_CODE);

									// License the products (without confirming LAC).
									Info.LicenseSelectedProducts();

									// Update the dialog to reflect the new state of the licensed products.
									CheckDlgButton(IDC_TAX_CHECK, GetApp()->UserNeedsTax());
									m_ProductList.DisableLicensedItems();
									UpdateDialog();
									
									// Let the user play with their new toys.
									OnInstall();
								}
								CATCH_ALL(e)
								{
									csReason.Empty();
								
									// Some sort of error occurred, report it.
									switch (dwResult)
									{
										case CMsregPacketCatalogOrderResponse::CATALOGORDERRESULT_InvalidCreditCardNumber:
										{
											Util::SafeLoadString(IDS_INVALID_CREDIT_CARD_NUMBER, csReason);
											break;
										}
										case CMsregPacketCatalogOrderResponse::CATALOGORDERRESULT_CreditCardExpired:
										{
											Util::SafeLoadString(IDS_EXPIRED_CREDIT_CARD, csReason);
											break;
										}
										case CMsregPacketCatalogOrderResponse::CATALOGORDERRESULT_IncompleteCustomerInformation:
										{
											Util::SafeLoadString(IDS_INCOMPLETE_CUSTOMER_INFORMATION, csReason);
											break;
										}
										default:
										{
											Util::SafeLoadString(IDS_COULD_NOT_PROCESS, csReason);
											break;
										}
									}
									
									if (!csReason.IsEmpty())
									{
										COrderFailedDialog Dialog(csReason, this);
										Dialog.DoModal();
									}
									
									// Let the user try again.
									fRetryRegister = TRUE;
								}
								END_CATCH_ALL
                        
                        // Clean up.
                        delete pPacketHeader;
                        pPacketHeader = NULL;
                        
								cfFile.Close();

								break;
							}
							case IDCANCEL:
							{
								// They want to go back. Just go back to the register dialog.
								fRetryRegister = TRUE;
								break;
							}
							case IDABORT:
							{
								// Some sort of modem error occurred, report it.
								CCallFailedDialog Dialog(this);
								Dialog.DoModal();
								fRetryRegister = TRUE;
								break;
							}
							default:
							{
								ASSERT(FALSE);
								break;
							}
						}
					}
               
               // Make sure we delete temporary file.
					TRY
					{
						CFile::Remove(csFileName);
					}
					END_TRY
				}
			} while (fRetryRegister);

			break;
		}
		case IDABORT:
		{
			// No modem was found, report this fact.
			CNoModemDialog Dialog(this);
			Dialog.DoModal();
			break;
		}
	}
   
   // Make sure the dialog is up to date.
	UpdateDialog();
}

void CUpgradeDialog::AddCatalogOrderLineItems(CMsregPacketCatalogOrderRequest* pData, CProductItems* pItems)
{
	pData->DeleteAllLineItems();

	int nItems = pItems->ItemCount();
	
	for (int nItem = 0; nItem < nItems; nItem++)
	{
		CProductItem* pItem = pItems->Item(nItem);
					
		if (pItem != NULL)
		{
			// If the item is already licensed, then we cannot order it again.
			if (!pItem->IsLicensed())
			{
				// The item is not licensed. If it or one or more of its subitems is marked and
				// not icensed, then there is a line ittem to build.
				if ((pItem->TestFlag(PRODUCT_ITEM_Marked))
				 || (pItem->Items()->CountProductFlags(PRODUCT_ITEM_Marked, CProductItems::NotLicensed) > 0))
				{
					// Check if the item itself is marked.
					if (pItem->TestFlag(PRODUCT_ITEM_Marked))
					{
						// Yes, build a line item for it.
						CMsregPacketCatalogOrderLineItem* pLineItem = new CMsregPacketCatalogOrderLineItem;
						if (pLineItem != NULL)
						{
							pLineItem->Category() = pItem->Code();
							pLineItem->Quantity() = 1;
							pLineItem->Cost() = (DWORD)(pItem->Price());
							pLineItem->DeleteAllCollections();
							pData->AddLineItem(pLineItem);
							pLineItem = NULL;
						}
					}
					else
					{
						// The item itself is not marked. Check if the item is
						// a group of collections. If so, then build a line item
						// for the selected collections.
						
						if (strlen(pItem->CollectionName()) > 0)
						{
							CMsregPacketCatalogOrderLineItem* pLineItem = new CMsregPacketCatalogOrderLineItem;
							if (pLineItem != NULL)
							{
								pLineItem->Category() = pItem->Code();
								pLineItem->Quantity() = 1;
								pLineItem->Cost() = 0;
								pLineItem->DeleteAllCollections();
								
								CProductItems* pCollections = pItem->Items();
								if (pCollections != NULL)
								{
									int nCollections = pCollections->ItemCount();
									for (int nCollection = 0; nCollection < nCollections; nCollection++)
									{
										CProductItem* pCollection = pCollections->Item(nCollection);
										
										// Check if the collection is marked.
										if ((pCollection != NULL)
										 && (pCollection->TestFlag(PRODUCT_ITEM_Marked))
										 && (!pCollection->IsLicensed()))
										{
											pLineItem->Cost() += (DWORD)(pCollection->Price());
											pLineItem->AddCollection(pCollection->Name());
										}
									}
								}
								
								pData->AddLineItem(pLineItem);
								pLineItem = NULL;
							}
						}
						else
						{
							// This item is not marked and is not a group of collections.
							// Simply call ourselves to build line items for any marked
							// child items.
							
							AddCatalogOrderLineItems(pData, pItem->Items());
						}
					}
				}
			}
		}
	}
}

void CUpgradeDialog::OnSelchangeTaxList()
{
	m_nTaxIndex = m_TaxList.GetCurSel();
	UpdateDialog();	
}

void CUpgradeDialog::OnSelchangeCountryList()
{
	UpdateForCurrentCountry();
	UpdateDialog();	
}

void CUpgradeDialog::SetTaxIndex(int nTaxIndex)
{
	m_nTaxIndex = nTaxIndex;
	m_TaxList.SetCurrentTaxIndex(m_nTaxIndex);
	if (m_hWnd != NULL)
	{
		UpdateDialog();
	}
}


void CUpgradeDialog::UpdateForCurrentCountry(void)
{
	GetApp()->FreeInternationalData();
	
	TRY
	{
		// Get the current country.
		WORD wCountryId = m_CountryList.GetCountryId();
		if (wCountryId != 0)
		{
			// Save the selection.
			GetApp()->UserIniFile().WriteInteger(MSREGUSRINI_Section_CustomerInfo, MSREGUSRINI_CustomerInfo_CallingFrom, wCountryId);
				
			// Get the registration parameters for the country.
			CInternationalData* pData = new CInternationalData;
			if (pData != NULL)
			{
				if (!GetApp()->GetInternationalData(wCountryId, pData))
				{
					delete pData;
					pData = NULL;
				}
			}
			
			// Save the registration parameters.
			GetApp()->SetInternationalData(pData);
			
			// We can now get the sales tax information. Requires international data
			// and ini files to be set up.
			SalesTaxList.Init();
		}
	}
	END_TRY
	
	// Only enable the order buttons that are supported.
	CInternationalData* pInternationalData = GetApp()->GetInternationalData();
   
   BOOL fModemExists = FALSE;
   BOOL fPhoneExists = FALSE;
   BOOL fFaxExists = FALSE;
   BOOL fMailExists = FALSE;
   
	if (pInternationalData != NULL)
	{
		fModemExists = pInternationalData->m_InstantSales.m_Modem.Exists();
		fPhoneExists = pInternationalData->m_InstantSales.m_Phone.Exists();
		fFaxExists = pInternationalData->m_InstantSales.m_Fax.Exists();
		fMailExists = pInternationalData->m_InstantSales.m_Mail.Exists();
	}
			
	CWnd* pOrderByModem = GetDlgItem(IDC_ORDERBYMODEM);
	if (pOrderByModem != NULL)
	{
		pOrderByModem->EnableWindow(fModemExists);
	}
	
	CWnd* pOrderByPhone = GetDlgItem(IDC_ORDERBYPHONE);
	if (pOrderByPhone != NULL)
	{
		pOrderByPhone->EnableWindow(fPhoneExists);
	}
	
	CWnd* pOrderByMail = GetDlgItem(IDC_ORDERBYMAIL);
	if (pOrderByMail != NULL)
	{
		pOrderByMail->EnableWindow(fFaxExists && fMailExists);
	}
}
