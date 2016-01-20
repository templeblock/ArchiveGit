//////////////////////////////////////////////////////////////////////////////
// $Heading: PGSOrWiz.cpp$
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/PGSOrWiz.cpp $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 35    11/25/98 6:02p Psasse
// Add text relating CD number to Collection for Order Wizard
// 
// 34    11/23/98 5:31p Johno
// 
// 33    11/23/98 3:59p Johno
// Changes for initial checked order item
// 
// 32    11/22/98 12:16a Psasse
// Default to current month in expiration combo
// 
// 31    11/21/98 6:25p Johno
// Changes for separate collection / cd listboxes
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "util.h"
#include "intldll.h"
#include "glicense.h"

#include "PGSOrWiz.h"

static char BASED_CODE cbDefaultCountryCode[] = "US";

/////////////////////////////////////////////////////////////////////////////
// COrderFormListBox
COrderFormListBox::COrderFormListBox()
{
	m_nPriceAlign = -1;
	m_pItems = NULL;
}

COrderFormListBox::~COrderFormListBox()
{
}

void COrderFormListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(m_pItems != NULL);

	// You must override DrawItem and MeasureItem for LBS_OWNERDRAWVARIABLE
	ASSERT((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS)) ==
		(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS));

	ASSERT(m_nPriceAlign != -1);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		// Get the window rect.
		CRect crListBox;
		GetWindowRect(crListBox);

		int cyItem = GetItemHeight(lpDrawItemStruct->itemID);
		BOOL fDisabled = !IsWindowEnabled() || !IsEnabled(lpDrawItemStruct->itemID);

		COLORREF newTextColor = fDisabled ?
			RGB(0x80, 0x80, 0x80) : GetSysColor(COLOR_WINDOWTEXT);  // light gray
		COLORREF oldTextColor = pDC->SetTextColor(newTextColor);

		COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

		if (newTextColor == newBkColor)
			newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

		if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
		{
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		}

		if (m_cyText == 0)
			VERIFY(cyItem >= CalcMinimumItemHeight());

		CString strName;
		CString strPrice;
		GetText(lpDrawItemStruct->itemID, strName);

		// Split the text into the two fields.
		int nIndex = strName.Find('\t');
		if (nIndex != -1)
		{
			strPrice = strName.Mid(nIndex+1);
			strName = strName.Left(nIndex);
		}

		// Get the item.
		int nItem = GetItemData(lpDrawItemStruct->itemID);
		COnlineOrderFormItem& Item = m_pItems->GetAt(nItem);
/*			
		CFont cfText;
		CFont* pOldFont = NULL;

		// Make CDs bold.
		if (Item.m_fIsCD)
		{
			CFont* pFont = GetFont();
			if (pFont != NULL)
			{
				LOGFONT lf;
				if (pFont->GetLogFont(&lf))
				{
					// Create a new bolder font.
					lf.lfWeight = FW_BOLD;
					if (cfText.CreateFontIndirect(&lf))
					{
						pOldFont = pDC->SelectObject(&cfText);
					}
				}
			}
		}
*/
		if (!strName.IsEmpty())
		{
			pDC->ExtTextOut(lpDrawItemStruct->rcItem.left+4,
				lpDrawItemStruct->rcItem.top + max(0, (cyItem - m_cyText) / 2),
				ETO_OPAQUE, &(lpDrawItemStruct->rcItem), strName, strName.GetLength(), NULL);
		}

		if (!strPrice.IsEmpty())
		{
			// Format the string correctly.
			double dPrice = atof(strPrice);
			Util::PriceString(strPrice, long(dPrice*100.0), TRUE, "Free");
			if (Item.m_fIsCD)
			{
				strPrice += " + S/H";
			}

			// Line it up where we want it.
			int nX = m_nPriceAlign-pDC->GetTextExtent(strPrice, strPrice.GetLength()).cx;
			pDC->ExtTextOut(nX,
				lpDrawItemStruct->rcItem.top + max(0, (cyItem - m_cyText) / 2),
				0, &(lpDrawItemStruct->rcItem), strPrice, strPrice.GetLength(), NULL);
		}

		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);
/*
		if (pOldFont != NULL)
		{
			pDC->SelectObject(pOldFont);
		}
*/
	}

	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));
}

/////////////////////////////////////////////////////////////////////////////
// CPGSWizardPage property page
IMPLEMENT_DYNAMIC(CPGSWizardPage, CPropertyPage)

CPGSWizardPage::CPGSWizardPage(UINT uID) : CPropertyPage(uID)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

void CPGSWizardPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPGSWizardPage)
	//}}AFX_DATA_MAP
}

CPGSOrderWizard* CPGSWizardPage::GetSheet(void)
{
	return (CPGSOrderWizard*)GetParent();
}

const CServerOrderFormInfo* CPGSWizardPage::GetOrderFormInfo(void)
{
	return GetSheet()->GetOrderFormInfo();
}

COnlineOrderFormItemArray& CPGSWizardPage::GetOrderFormItems(void)
{
	return GetSheet()->GetOrderFormItems();
}

BEGIN_MESSAGE_MAP(CPGSWizardPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPGSWizardPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPGSWizardPage message handlers

BOOL CPGSWizardPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CPGSWizardPage::OnWizardNext() 
{
	return CPropertyPage::OnWizardNext();
}

LRESULT CPGSWizardPage::OnWizardBack() 
{	
	return CPropertyPage::OnWizardBack();
}

BOOL CPGSWizardPage::OnSetActive() 
{
	CPropertyPage::OnSetActive();

	// Disable the "Back" button
	CPropertySheet* pSheet = GetSheet();
	int nIndex = pSheet->GetActiveIndex();
	if (nIndex == 0)
	{
		pSheet->SetWizardButtons(PSWIZB_NEXT);
	}
	else if (nIndex == pSheet->GetPageCount()-1)
	{
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	}
	else
	{
		pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
	return TRUE;
}

void CPGSWizardPage::DrawPrice(LPDRAWITEMSTRUCT lpDrawItemStruct)
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

//
// Fill out an item info structure from the currently chosen items.
//

void CPGSWizardPage::FillItemInfo(CServerPurchaseItemInfo& ItemInfo)
{
	// re-zero the arrays
	ItemInfo.m_csaCDs.RemoveAll();
	ItemInfo.m_csaCollections.RemoveAll();

	COnlineOrderFormItemArray& Items = GetOrderFormItems();
	const CServerOrderFormInfo* pOrderFormInfo = GetOrderFormInfo();

	for (int n = 0; n < Items.GetSize(); n++)
	{
		if(Items[n].m_fChecked && !Items[n].m_fPurchased)
		{
			CServerOrderFormCollectionInfo& CollectionInfo = pOrderFormInfo->m_Collections.GetAt(Items[n].m_nInfoIndex);

			if(Items[n].m_fIsCD)
			{
				ItemInfo.m_csaCDs.Add(CollectionInfo.m_csCDID);
			}
			else
			{
				ItemInfo.m_csaCollections.Add(CollectionInfo.m_csCollectionID);
			}
		}
	}
}

// Fill out a location info structure.
void CPGSWizardPage::FillLocationInfo(CServerLocationInfo& LocationInfo)
{
	CServerAccountInfo& Info = GetAccountInfo();

	LocationInfo.m_csState = Info.m_csState;
	LocationInfo.m_csZip = Info.m_csZip;
	LocationInfo.m_csCountry = Info.m_csCountry;
}

void CPGSWizardPage::FillPriceOrderInfo(CServerPriceOrderInfo& OrderInfo)
{
	FillItemInfo(OrderInfo.m_ItemInfo);
	FillLocationInfo(OrderInfo.m_LocationInfo);
}

CServerAccountInfo& CPGSWizardPage::GetAccountInfo(void)
{
	return GetSheet()->GetAccountInfo();
}

/////////////////////////////////////////////////////////////////////////////
// CPGSOrderFormPage dialog
IMPLEMENT_DYNAMIC(CPGSOrderFormPage, CPGSWizardPage)

CPGSOrderFormPage::CPGSOrderFormPage() :
	CPGSWizardPage(CPGSOrderFormPage::IDD)
{

}

void CPGSOrderFormPage::DoDataExchange(CDataExchange* pDX)
{
	CPGSWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPGSOrderFormPage)
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		UpdateItemsFromList();
	}
}

BEGIN_MESSAGE_MAP(CPGSOrderFormPage, CPGSWizardPage)
	//{{AFX_MSG_MAP(CPGSOrderFormPage)
	ON_BN_CLICKED(IDC_COMPUTE_TOTALS, OnComputeTotals)
	ON_CLBN_CHKCHANGE(IDC_PRODUCT_LIST, OnChkchangeProductList)
	ON_CLBN_CHKCHANGE(IDC_CD_LIST, OnChkchangeProductList)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPGSOrderFormPage message handlers


BOOL CPGSOrderFormPage::OnInitDialog()
{
	CPGSWizardPage::OnInitDialog();

	m_ListBox.SubclassDlgItem(IDC_PRODUCT_LIST ,this);
	m_ListBoxCD.SubclassDlgItem(IDC_CD_LIST ,this);

	const CServerOrderFormInfo* pOrderForm = GetOrderFormInfo();
	if (pOrderForm != NULL)
	{
		CWnd* pWnd;
		// Get the price align window.
		pWnd = GetDlgItem(IDC_PRICE_ALIGN);
		ASSERT(pWnd != NULL);
		CRect crWindow;
		pWnd->GetWindowRect(crWindow);
		m_nPriceAlign = crWindow.left;

		m_ListBox.ScreenToClient(crWindow);
		m_ListBox.SetPriceAlign(crWindow.left);
		m_ListBoxCD.SetPriceAlign(crWindow.left);
		m_ListBox.SetOrderFormItems(&GetSheet()->GetOrderFormItems());
		m_ListBoxCD.SetOrderFormItems(&GetSheet()->GetOrderFormItems());

		// Set the instruction text.
		CString csText;
		LPCSTR pszHeader = pOrderForm->m_csOrderHeader;
		// Convert '\\' + 'n' to '\n'.
		while (*pszHeader != '\0')
		{
			if (pszHeader[0] == '\\' && pszHeader[1] == 'n')
			{
				csText += '\r';
				csText += '\n';
				pszHeader += 2;
			}
			else
			{
				csText += *pszHeader++;
			}
		}
		SetDlgItemText(IDC_ORDER_INSTRUCTIONS, csText);

		BuildOrderList();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CPGSOrderFormPage::OnSetActive() 
{
	CPGSWizardPage::OnSetActive();
	UpdateButtons();
	return TRUE;
}

void CPGSOrderFormPage::BuildOrderList()
{
	const CServerOrderFormInfo* pOrderForm = GetOrderFormInfo();
	if (pOrderForm != NULL)
	{
		// Add items to the listbox.
		m_ListBox.SetRedraw(FALSE);
		m_ListBoxCD.SetRedraw(FALSE);

		// Empty the current list.
		m_ListBox.ResetContent();
		m_ListBoxCD.ResetContent();
		GetOrderFormItems().RemoveAll();

		// Track when we cross CDs.
		CString csLastCD;
		int n;					// Scope outside the "for" loop.
		for (n = 0; n < pOrderForm->m_Collections.GetSize(); n++)
		{
			CServerOrderFormCollectionInfo& CollectionInfo = pOrderForm->m_Collections.GetAt(n);
			// See if the CD info has changed.
			if (CollectionInfo.m_csCDID != csLastCD)
			{
				// If we had a last CD, add the item for it now.
				if (!csLastCD.IsEmpty())
				{
					AddCDItem(n-1);
				}
				// Set the new "last" CD ID.
				csLastCD = CollectionInfo.m_csCDID;
			}

			// Add the item for this collection.
			AddCollectionItem(n, m_csInitialCheck);
		}

		// Add a final CD if necessary.
		if (!csLastCD.IsEmpty())
		{
			AddCDItem(n-1);
		}
		
		m_ListBox.SetRedraw(TRUE);
		m_ListBoxCD.SetRedraw(TRUE);
		m_ListBox.Invalidate();
		m_ListBoxCD.Invalidate();
	}
	// Make sure we are in sync.
	ASSERT(GetOrderFormItems().GetSize() == m_ListBox.GetCount() + m_ListBoxCD.GetCount());
}

//
// Add a collection item to the order form list.
//

void CPGSOrderFormPage::AddCollectionItem(int nIndex, CString &str)
{
	CServerOrderFormCollectionInfo& CollectionInfo = GetOrderFormInfo()->m_Collections.GetAt(nIndex);

	// Add a line for this collection.
	COnlineOrderFormItem Item;
	Item.m_fPurchased = !CollectionInfo.m_csCollectionPurchased.IsEmpty();
	Item.m_fChecked = Item.m_fPurchased;
	Item.m_fIsCD = FALSE;
	Item.m_nInfoIndex = nIndex;

	CString csText = CollectionInfo.m_csCollectionDescription;
	if (m_csInitialCheck == csText)
	{
		Item.m_fChecked = TRUE;
		m_csInitialCheck.Empty();	
	}
	//csText += " (";
	csText += CollectionInfo.m_csCDLocation;
	//csText += ")";
	csText += '\t';
	csText += CollectionInfo.m_csCollectionPrice;

	Item.m_nListIndex = m_ListBox.AddString(csText);
	if (Item.m_nListIndex >= 0)
	{
		GetOrderFormItems().Add(Item);
		m_ListBox.SetItemData(Item.m_nListIndex, GetOrderFormItems().GetSize() - 1);
		// If we have been purchased, check and disable us.
		if (Item.m_fPurchased)
		{
			m_ListBox.SetCheck(Item.m_nListIndex, 2);
			m_ListBox.Enable(Item.m_nListIndex, FALSE);
		}
		else
		if (Item.m_fChecked)
			m_ListBox.SetCheck(Item.m_nListIndex, 1);
	}
}

//
// Add a CD item to the order form list.
//

void CPGSOrderFormPage::AddCDItem(int nIndex)
{
	CServerOrderFormCollectionInfo& CollectionInfo = GetOrderFormInfo()->m_Collections.GetAt(nIndex);

	// Add a line for this CD.
	COnlineOrderFormItem Item;
	Item.m_fPurchased = !CollectionInfo.m_csCDPurchased.IsEmpty();
	Item.m_fChecked = Item.m_fPurchased;
	Item.m_fIsCD = TRUE;
	Item.m_nInfoIndex = nIndex;

	CString csText = CollectionInfo.m_csCDDescription;
	csText += '\t';
	csText += CollectionInfo.m_csCDPrice;

	Item.m_nListIndex = m_ListBoxCD.AddString(csText);
	if (Item.m_nListIndex >= 0)
	{
		GetOrderFormItems().Add(Item);
		m_ListBoxCD.SetItemData(Item.m_nListIndex, GetOrderFormItems().GetSize() - 1);
		// If we have been purchased, check and disable us.
		if (Item.m_fPurchased)
		{
			m_ListBoxCD.SetCheck(Item.m_nListIndex, 2);
			m_ListBoxCD.Enable(Item.m_nListIndex, FALSE);
		}
	}
}

//
// Update the items from the list.
//

void CPGSOrderFormPage::UpdateItemsFromList(void)
{
	// Run through all the items in the list box and update the corresponding
	// item in the item array.
	COnlineOrderFormItemArray& Items = GetOrderFormItems();

	for (int n = 0; n < Items.GetSize(); n++)
	{
		if (Items[n].m_fIsCD == FALSE)
			Items[n].m_fChecked = m_ListBox.GetCheck(Items[n].m_nListIndex);
		else
			Items[n].m_fChecked = m_ListBoxCD.GetCheck(Items[n].m_nListIndex);
	}
}

//
// Compute totals for the currently selected items.
//

void CPGSOrderFormPage::OnComputeTotals() 
{
	CServerPriceOrderInfo OrderInfo;
	CServerPriceInfo PriceInfo;

	UpdateData(TRUE);

	// All we want is the item info.
	FillItemInfo(OrderInfo.m_ItemInfo);
//	OrderInfo.m_ItemInfo.m_csaCDs.RemoveAll();	// CDs are free!

	// Get the price for the current order state.
	GetSheet()->GetContentManager()->PriceInternetOrder(OrderInfo, PriceInfo);

	SetDlgItemText(IDC_TOTAL, PriceInfo.m_csItemTotal);
}

void CPGSOrderFormPage::OnChkchangeProductList() 
{
	UpdateButtons();
	SetDlgItemText(IDC_TOTAL, "");
}

//
// Update the next button as appropriate.
//

void CPGSOrderFormPage::UpdateButtons(void)
{
	CPropertySheet* pSheet = GetSheet();
	BOOL fSomethingChecked = SomethingChecked();
	// Enable the "next" button.
	pSheet->SetWizardButtons(fSomethingChecked ? PSWIZB_NEXT : 0);

	// Enable the "Compute Totals" button.
	CWnd* pButton = GetDlgItem(IDC_COMPUTE_TOTALS);
	if (pButton != NULL)
	{
		pButton->EnableWindow(fSomethingChecked);
	}
}

BOOL CPGSOrderFormPage::SomethingChecked(void)
{
	// Run through the items in the checkbox and see if any are checked.
	BOOL fChecked = FALSE;
	// We want to copy the selections back to the list.
	int nCount = m_ListBox.GetCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		if (m_ListBox.GetCheck(nIndex) && m_ListBox.IsEnabled(nIndex))
		{
			fChecked = TRUE;
			break;
		}
	}
	
	if (fChecked == FALSE)
	{
		nCount = m_ListBoxCD.GetCount();
		for (int nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (m_ListBoxCD.GetCheck(nIndex) && m_ListBoxCD.IsEnabled(nIndex))
			{
				fChecked = TRUE;
				break;
			}
		}
	}

	return fChecked;
}

void CPGSOrderFormPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CPGSWizardPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
	if (nIDCtl == IDC_TOTAL)
	{
		if ((lpDrawItemStruct->itemAction & ODA_DRAWENTIRE) != 0)
		{
			DrawPrice(lpDrawItemStruct);
		}
	}
	else
	{
		CPGSWizardPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPGSOrderUserInformationPage dialog
IMPLEMENT_DYNAMIC(CPGSOrderUserInformationPage, CPGSWizardPage)

CPGSOrderUserInformationPage::CPGSOrderUserInformationPage() :
	CPGSWizardPage(CPGSOrderUserInformationPage::IDD)
{
	//{{AFX_DATA_INIT(CPGSOrderUserInformationPage)
	m_strFirstName = _T("");
	m_strLastName = _T("");
	m_strStreet1 = _T("");
	m_strStreet2 = _T("");
	m_strCity = _T("");
	m_strState = _T("");
	m_strZipCode = _T("");
	m_strPhone = _T("");
	m_strEMail = _T("");
	//}}AFX_DATA_INIT

}

void CPGSOrderUserInformationPage::DoDataExchange(CDataExchange* pDX)
{
	CPGSWizardPage::DoDataExchange(pDX);
	if (!pDX->m_bSaveAndValidate)
	{
		FromAccountInfo(GetAccountInfo());
	}

	//{{AFX_DATA_MAP(CPGSOrderUserInformationPage)
	DDX_Control(pDX, IDC_COUNTRY_LIST, m_cbCountry);
	DDX_Text(pDX, IDC_FIRSTNAME, m_strFirstName);
	DDV_MaxChars(pDX, m_strFirstName, 255);
	DDX_Text(pDX, IDC_LASTNAME, m_strLastName);
	DDV_MaxChars(pDX, m_strLastName, 255);
	DDX_Text(pDX, IDC_STREET1, m_strStreet1);
	DDV_MaxChars(pDX, m_strStreet1, 255);
	DDX_Text(pDX, IDC_STREET2, m_strStreet2);
	DDV_MaxChars(pDX, m_strStreet2, 255);
	DDX_Text(pDX, IDC_TEMPCITY, m_strCity);
	DDV_MaxChars(pDX, m_strCity, 255);
	DDX_Text(pDX, IDC_TEMPSTATE, m_strState);
	DDV_MaxChars(pDX, m_strState, 2);
	DDX_Text(pDX, IDC_TEMPZIP, m_strZipCode);
	DDV_MaxChars(pDX, m_strZipCode, 15);
	DDX_Text(pDX, IDC_TEMPPHONE, m_strPhone);
	DDV_MaxChars(pDX, m_strPhone, 255);
	DDX_Text(pDX, IDC_EMAIL, m_strEMail);
	DDV_MaxChars(pDX, m_strEMail, 255);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		ToAccountInfo(GetAccountInfo());
	}
}

BEGIN_MESSAGE_MAP(CPGSOrderUserInformationPage, CPGSWizardPage)
	//{{AFX_MSG_MAP(CPGSOrderUserInformationPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPGSOrderUserInformationPage message handlers

BOOL CPGSOrderUserInformationPage::OnInitDialog()
{
	CPGSWizardPage::OnInitDialog();

	// Fill out the country combo box.
	CComboBox* pCountryCombo = (CComboBox*)GetDlgItem(IDC_COUNTRY_LIST);
	CString csDefaultCountry;
	if (pCountryCombo != NULL)
	{
		// Fill out the country combo box.
		const CServerCountryInfoArray& Countries = GetSheet()->GetCountries();
		for (int nCountry = 0; nCountry < Countries.GetSize(); nCountry++)
		{
			const CServerCountryInfo& Country = Countries.GetAt(nCountry);
			int nNewIndex = pCountryCombo->AddString(Country.m_csName);
			if (nNewIndex >= 0)
			{
				pCountryCombo->SetItemData(nNewIndex, nCountry);

				// Set the default.
				if (Country.m_csCode == cbDefaultCountryCode)
				{
					// Remember this as the default index.
					csDefaultCountry = Country.m_csName;
				}
			}
		}
	}

	// Set the selected country in the combo box.
	SetSelectedCountry(GetAccountInfo().m_csCountry, csDefaultCountry);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CPGSOrderUserInformationPage::OnSetActive() 
{
	return CPGSWizardPage::OnSetActive();
}

LRESULT CPGSOrderUserInformationPage::OnWizardNext() 
{
	UpdateData(TRUE);

	if(m_strFirstName.IsEmpty()) 
	{
		AfxMessageBox(IDS_PGSWIZ_FILL_FIRSTNAME_FIELD);
		GetDlgItem(IDC_FIRSTNAME)->SetFocus();
		return -1;
	}	
	else if (m_strLastName.IsEmpty())
	{
		AfxMessageBox(IDS_PGSWIZ_FILL_LASTNAME_FIELD);
		GetDlgItem(IDC_LASTNAME)->SetFocus();
		return -1;
	}	
	else if (m_strStreet1.IsEmpty())
	{
		AfxMessageBox(IDS_PGSWIZ_FILL_STREET_FIELD);
		GetDlgItem(IDC_STREET1)->SetFocus();
		return -1;
	}	
	else if (m_strCity.IsEmpty())
	{
		AfxMessageBox(IDS_PGSWIZ_FILL_CITY_FIELD);
		GetDlgItem(IDC_TEMPCITY)->SetFocus();
		return -1;
	}
	else if (m_strZipCode.IsEmpty())
	{
		AfxMessageBox(IDS_PGSWIZ_FILL_ZIPCODE_FIELD);
		GetDlgItem(IDC_TEMPZIP)->SetFocus();
		return -1;
	}
	else if (m_strState.IsEmpty())
	{
		AfxMessageBox(IDS_PGSWIZ_FILL_STATE_FIELD);
		GetDlgItem(IDC_TEMPSTATE)->SetFocus();
		return -1;
	}
	else if (m_strPhone.IsEmpty())
	{
		AfxMessageBox(IDS_PGSWIZ_FILL_PHONE_FIELD);
		GetDlgItem(IDC_TEMPPHONE)->SetFocus();
		return -1;
	}
	else
	{
		return CPGSWizardPage::OnWizardNext();
	}
}

void CPGSOrderUserInformationPage::FromAccountInfo(const CServerAccountInfo& AccountInfo)
{
	m_strFirstName = AccountInfo.m_csCustFirstName;
	m_strLastName =  AccountInfo.m_csCustLastName;
	m_strStreet1 = AccountInfo.m_csStreet1;
	m_strStreet2 = AccountInfo.m_csStreet2;
	m_strCity = AccountInfo.m_csCity;
	m_strZipCode = AccountInfo.m_csZip;
	m_strState = AccountInfo.m_csState.Left(2);
	m_strEMail = AccountInfo.m_csEMailAddr;
	m_strPhone = AccountInfo.m_csPhone;
}

void CPGSOrderUserInformationPage::ToAccountInfo(CServerAccountInfo& AccountInfo)
{
	AccountInfo.m_csCustFirstName = m_strFirstName;
	AccountInfo.m_csCustLastName = m_strLastName ;
	AccountInfo.m_csStreet1 = m_strStreet1;
	AccountInfo.m_csStreet2 = m_strStreet2;
	AccountInfo.m_csCity = m_strCity;
	AccountInfo.m_csZip = m_strZipCode;
	AccountInfo.m_csCountry = GetSelectedCountry();
	AccountInfo.m_csState = m_strState;
	AccountInfo.m_csEMailAddr = m_strEMail;
	AccountInfo.m_csPhone = m_strPhone;
}

//
// Get the selected country (two letter abbreviation).
//

CString CPGSOrderUserInformationPage::GetSelectedCountry(void)
{
	CString csCountry;

	const CServerCountryInfoArray& Countries = GetSheet()->GetCountries();
	int nCurSel = m_cbCountry.GetCurSel();
	if (nCurSel >= 0 && (nCurSel = m_cbCountry.GetItemData(nCurSel)) >= 0)
	{
		csCountry = Countries.GetAt(nCurSel).m_csCode;
	}

	return csCountry;
}

//
// Set the selected country.
//

void CPGSOrderUserInformationPage::SetSelectedCountry(LPCSTR pszCountry, LPCSTR pszDefault)
{
	// Try to match the incoming country.
	if (m_cbCountry.SelectString(-1, pszCountry) == CB_ERR)
	{
		// If no match, use the default.
		if (pszDefault[0] == '\0' || m_cbCountry.SelectString(-1, pszDefault) == CB_ERR)
		{
			// Set something!
			m_cbCountry.SetCurSel(0);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPGSOrderConfirmationPage dialog
IMPLEMENT_DYNAMIC(CPGSOrderConfirmationPage, CPGSWizardPage)

CPGSOrderConfirmationPage::CPGSOrderConfirmationPage() :
	CPGSWizardPage(CPGSOrderConfirmationPage::IDD)
{
	//{{AFX_DATA_INIT(CPGSOrderConfirmationPage)
	m_strCreditCardNumber = _T("");
	m_strTotalPrice = _T("");
	//}}AFX_DATA_INIT
}

void CPGSOrderConfirmationPage::DoDataExchange(CDataExchange* pDX)
{
	CPGSWizardPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CPGSOrderConfirmationPage)
	DDX_Control(pDX, IDC_CREDIT_CARD_TYPE, m_cbCreditCardType);
	DDX_Control(pDX, IDC_SUBTOTAL, m_btnSubTotal);
	DDX_Control(pDX, IDC_TAX, m_btnTax);
	DDX_Control(pDX, IDC_FEE, m_btnFee);
	DDX_Control(pDX, IDC_TOTAL, m_btnTotal);
	DDX_Control(pDX, IDC_SHIPPING_COMBO, m_cbShipping);
	DDX_Control(pDX, IDC_EXPIRATION_MONTH, m_cbExpirationMonth);
	DDX_Control(pDX, IDC_EXPIRATION_YEAR, m_cbExpirationYear);
	DDX_Text(pDX, IDC_CREDIT_CARD_NUMBER, m_strCreditCardNumber);
	DDV_MaxChars(pDX, m_strCreditCardNumber, 255);
	DDX_Text(pDX, IDC_EXPIRATION_MONTH, m_strCreditCardExpirationMonth);
	DDX_Text(pDX, IDC_EXPIRATION_YEAR, m_strCreditCardExpirationYear);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPGSOrderConfirmationPage, CPGSWizardPage)
	//{{AFX_MSG_MAP(CPGSOrderConfirmationPage)
	ON_WM_DRAWITEM()
   ON_CBN_SELCHANGE(IDC_SHIPPING_COMBO, OnSelchangeShipping)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPGSOrderConfirmationPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (nIDCtl == IDC_TOTAL ||
		 nIDCtl == IDC_SUBTOTAL ||
		 nIDCtl == IDC_FEE ||
		 nIDCtl == IDC_TAX)
	{
		if ((lpDrawItemStruct->itemAction & ODA_DRAWENTIRE) != 0)
		{
			DrawPrice(lpDrawItemStruct);
		}
	}
	else
	{
		CPGSWizardPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPGSOrderConfirmationPage message handlers

BOOL CPGSOrderConfirmationPage::OnInitDialog()
{
	CPGSWizardPage::OnInitDialog();

	m_ListBox.SubclassDlgItem(IDC_ORDER_LIST ,this);

	CWnd* pWnd;
	// Get the price align window.
	pWnd = GetDlgItem(IDC_PRICE_ALIGN);
	ASSERT(pWnd != NULL);
	CRect crWindow;
	pWnd->GetWindowRect(crWindow);
	m_nPriceAlign = crWindow.left;

	m_ListBox.ScreenToClient(crWindow);
	m_ListBox.SetPriceAlign(crWindow.left);
	m_ListBox.SetOrderFormItems(&GetSheet()->GetOrderFormItems());

	FillConfirmedOrderList();

	FillCreditCardList();

	m_cbExpirationMonth.SetCurSel(0);

	FillCreditCardExpirationYearList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CPGSOrderConfirmationPage::OnSetActive() 
{
	// Fill out the current order state.
	if(FillConfirmedOrderList())
	{
		FillShippingList();
	}

	// Set the initial prices.
	UpdatePrices();

	return CPGSWizardPage::OnSetActive();
}

//
// Update the price buttons.
//

void CPGSOrderConfirmationPage::UpdatePrices(void)
{
	// Get the price for the current order state.
	CServerPriceOrderInfo OrderInfo;
	FillPriceOrderInfo(OrderInfo);
	OrderInfo.m_csShipID = GetSelectedShippingMethod();

	CServerPriceInfo PriceInfo;
	GetSheet()->GetContentManager()->PriceInternetOrder(OrderInfo, PriceInfo);

	SetButtons(PriceInfo);
}

BOOL CPGSOrderConfirmationPage::OnWizardFinish() 
{
	UpdateData(TRUE);

	if(m_strCreditCardNumber.IsEmpty() ||
		m_strCreditCardExpirationMonth.IsEmpty() ||
		m_strCreditCardExpirationYear.IsEmpty())
	{
		AfxMessageBox (IDS_PGSWIZ_FILL_CREDIT_INFO);
		return FALSE;
	}

	
	// Put up the "order in progress" dialog.
	CDialog ProgressDialog;
	ProgressDialog.Create(IDD_ORDER_IN_PROGRESS, CWnd::GetSafeOwner(NULL));
	ProgressDialog.GetParent()->EnableWindow(FALSE);
	ProgressDialog.ShowWindow(SW_SHOW);
	ProgressDialog.CenterWindow();
	ProgressDialog.UpdateWindow();

	// Send the purchasing info to the server.
	CServerPurchaseInfo PurchaseInfo;
	PurchaseInfo.m_AccountInfo = GetAccountInfo();
	FillItemInfo(PurchaseInfo.m_ItemInfo);
	PurchaseInfo.m_csShipID = GetSelectedShippingMethod();
	ToPaymentInfo(PurchaseInfo.m_PaymentInfo);

	CServerPurchaseResult PurchaseResult;
	int nStatus = GetSheet()->GetContentManager()->PurchaseInternetOrder(PurchaseInfo, PurchaseResult);

	// Take down the "Order in progress" dialog.
	ProgressDialog.GetParent()->EnableWindow(TRUE);
	ProgressDialog.ShowWindow(SW_HIDE);

	if (nStatus == CSSTATUS_Success)
	{
		// License the products.
		LicenseNewProducts();

		CString strOrderNumber = PurchaseResult.m_csOrderNumber;
		CString strResultText = PurchaseResult.m_csResultText;

		GetSheet()->GetContentManager()->SendAccountInfo(PurchaseInfo.m_AccountInfo);

		AfxMessageBox(strResultText);

		return CPGSWizardPage::OnWizardFinish();
	}
	else
	{
		return FALSE;
	}
}


//
// License the products just purchased.
//

void CPGSOrderConfirmationPage::LicenseNewProducts(void)
{
	// Construct a map with the new product codes.
	CMapStringToPtr mapLicensed;

	COnlineOrderFormItemArray& Items = GetOrderFormItems();
	const CServerOrderFormInfo* pOrderFormInfo = GetOrderFormInfo();

	for (int n = 0; n < Items.GetSize(); n++)
	{
		if(Items[n].m_fChecked && !Items[n].m_fPurchased && !Items[n].m_fIsCD)
		{
			CServerOrderFormCollectionInfo& CollectionInfo = pOrderFormInfo->m_Collections.GetAt(Items[n].m_nInfoIndex);

			mapLicensed.SetAt(CollectionInfo.m_csCollectionSKU, NULL);
		}
	}

	// License the products.
	LicenseData.UpdateOnlineProducts(mapLicensed, TRUE);
}

//
// Fill out the credit card list.
//

void CPGSOrderConfirmationPage::FillCreditCardList(void)
{
	m_cbCreditCardType.SetRedraw(FALSE);
	m_cbCreditCardType.ResetContent();

	const CServerOrderFormInfo* pOrderForm = GetOrderFormInfo();

	if (pOrderForm != NULL)
	{
		for (int n = 0; n < pOrderForm->m_csaCardTypes.GetSize(); n++)
		{
			CString String = pOrderForm->m_csaCardTypes.GetAt(n);
			int nIndex = String.Find('|');
			if (nIndex > 0)
			{
				String = String.Left(nIndex);
			}
			m_cbCreditCardType.AddString(String);
		}
	}

	m_cbCreditCardType.SetCurSel(0);
	m_cbCreditCardType.SetRedraw(TRUE);
	m_cbCreditCardType.Invalidate();
}


//
// Fill out the credit card expiration year combo list.
//

void CPGSOrderConfirmationPage::FillCreditCardExpirationYearList(void)
{
	m_cbExpirationYear.SetRedraw(FALSE);
	m_cbExpirationYear.ResetContent();

	int nYear = 0;
	int nMonth = 0;

#ifdef WIN32
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	nYear = (int)(SystemTime.wYear);
	nMonth = (int)(SystemTime.wMonth);
#else
	time_t BinaryTime;
	time(&BinaryTime);
	CTime Time(BinaryTime);

	nYear = Time.GetYear();
	nMonth = Time.GetMonth();
#endif

	for (int n =  nYear; n < nYear + 10; n++)
	{
		CString String;
		String.Format("%d", n); 
		if (String.GetLength() > 0)
		{
			String = String.Right(2);
		}
		m_cbExpirationYear.AddString(String);
	}

	m_cbExpirationMonth.SetCurSel(nMonth - 1); //zero-based
	m_cbExpirationMonth.SetRedraw(TRUE);
	m_cbExpirationMonth.Invalidate();

	m_cbExpirationYear.SetCurSel(0);
	m_cbExpirationYear.SetRedraw(TRUE);
	m_cbExpirationYear.Invalidate();
}

//
// Get the selected credit card type (two letter abbreviation).
//

CString CPGSOrderConfirmationPage::GetSelectedCreditCardType(void)
{
	CString csCreditCardType;

	const CServerOrderFormInfo* pOrderForm = GetOrderFormInfo();
	if (pOrderForm != NULL)
	{
		int nCurSel = m_cbCreditCardType.GetCurSel();
		if (nCurSel >= 0)
		{
			csCreditCardType = pOrderForm->m_csaCardTypes.GetAt(nCurSel);
			int nIndex = csCreditCardType.Find('|');
			if (nIndex > 0)
			{
				csCreditCardType = csCreditCardType.Mid(nIndex+1);
			}
		}
	}

	return csCreditCardType;
}

//
// Fill out the confirmed order list.
//

BOOL CPGSOrderConfirmationPage::FillConfirmedOrderList(void)
{
	//disable shipping combo unless a CD ordered
	m_cbShipping.EnableWindow(FALSE);
	m_cbShipping.SetCurSel(-1);
	BOOL fCDChecked = FALSE;

	// Add items to the listbox.
	m_ListBox.SetRedraw(FALSE);

	// Empty the current list.
	m_ListBox.ResetContent();

	COnlineOrderFormItemArray& Items = GetOrderFormItems();
	const CServerOrderFormInfo* pOrderFormInfo = GetOrderFormInfo();

	for (int n = 0; n < Items.GetSize(); n++)
	{
		if(Items[n].m_fChecked && !Items[n].m_fPurchased)
		{
			CServerOrderFormCollectionInfo& CollectionInfo = pOrderFormInfo->m_Collections.GetAt(Items[n].m_nInfoIndex);

			CString csName;
			CString csPrice;
			if(Items[n].m_fIsCD)
			{
				csName = CollectionInfo.m_csCDDescription;
				csPrice = CollectionInfo.m_csCDPrice;
				
				//a CD was ordered - enable shipping combo
				m_cbShipping.EnableWindow(TRUE);
				fCDChecked = TRUE;
			}
			else
			{
				csName = CollectionInfo.m_csCollectionDescription;
				csPrice = CollectionInfo.m_csCollectionPrice;
			}

			// Build the line to add.
			CString csText;
			csText = csName;
			csText += '\t';
			csText += csPrice;
			int nIndex = m_ListBox.AddString(csText);
			if (nIndex >= 0)
			{
				m_ListBox.SetItemData(nIndex, n);
			}
		}
	}

	m_ListBox.SetRedraw(TRUE);
	m_ListBox.Invalidate();

	return fCDChecked;
}

//
// Fill out the shipping option combo.
//

void CPGSOrderConfirmationPage::FillShippingList(void)
{
	// Get the shipping methods from the server.
	m_ShippingMethods.RemoveAll();
	CServerLocationInfo LocationInfo;
	FillLocationInfo(LocationInfo);
	if (GetSheet()->GetContentManager()->GetShippingInfo(LocationInfo, m_ShippingMethods) == CSSTATUS_Success)
	{
		// OK. We succeeded.
	}

	m_cbShipping.SetRedraw(FALSE);
	m_cbShipping.ResetContent();

	for (int n = 0; n < m_ShippingMethods.GetSize(); n++)
	{
		const CServerShippingInfo& ShippingMethod = m_ShippingMethods.GetAt(n);
		m_cbShipping.AddString(ShippingMethod.m_csName);
	}

	m_cbShipping.SetCurSel(0);
	m_cbShipping.SetRedraw(TRUE);
	m_cbShipping.Invalidate();
}

//
// Get the selected shipping method (abbreviation).
//

CString CPGSOrderConfirmationPage::GetSelectedShippingMethod(void)
{
	CString csShipID;

	int nCurSel = m_cbShipping.GetCurSel();
	if (nCurSel >= 0)
	{
		csShipID = m_ShippingMethods.GetAt(nCurSel).m_csShipID;
	}

	return csShipID;
}

void CPGSOrderConfirmationPage::ToPaymentInfo(CServerPaymentInfo& PaymentInfo)
{
	PaymentInfo.m_csCardType = GetSelectedCreditCardType();
	PaymentInfo.m_csCardNumber = m_strCreditCardNumber;
	PaymentInfo.m_csCardExpiration = m_strCreditCardExpirationMonth;
	PaymentInfo.m_csCardExpiration += m_strCreditCardExpirationYear;
}

void CPGSOrderConfirmationPage::OnSelchangeShipping()
{
	UpdatePrices();
}

void CPGSOrderConfirmationPage::SetButtons(CServerPriceInfo& PriceInfo)
{
	SetDlgItemText(IDC_SUBTOTAL, PriceInfo.m_csItemTotal);
	SetDlgItemText(IDC_FEE, PriceInfo.m_csShipping);
	SetDlgItemText(IDC_TAX, PriceInfo.m_csSalesTax);
	SetDlgItemText(IDC_TOTAL, PriceInfo.m_csOrderTotal);
}

/////////////////////////////////////////////////////////////////////////////
// CConfirmedOrderFormListBox
CConfirmedOrderFormListBox::CConfirmedOrderFormListBox()
{
	m_nPriceAlign = -1;
	m_pItems = NULL;
}

CConfirmedOrderFormListBox::~CConfirmedOrderFormListBox()
{
}

void CConfirmedOrderFormListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(m_pItems != NULL);

	// You must override DrawItem and MeasureItem for LBS_OWNERDRAWVARIABLE
	ASSERT((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS)) ==
		(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS));

	ASSERT(m_nPriceAlign != -1);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		// Get the window rect.
		CRect crListBox;
		GetWindowRect(crListBox);

		int cyItem = GetItemHeight(lpDrawItemStruct->itemID);
		BOOL fDisabled = FALSE; //!IsWindowEnabled() || !IsEnabled(lpDrawItemStruct->itemID);

		COLORREF newTextColor = fDisabled ?
			RGB(0x80, 0x80, 0x80) : GetSysColor(COLOR_WINDOWTEXT);  // light gray
		COLORREF oldTextColor = pDC->SetTextColor(newTextColor);

		COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

		if (newTextColor == newBkColor)
			newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

		if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
		{
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		}

		CString strName;
		CString strPrice;
		GetText(lpDrawItemStruct->itemID, strName);

		// Split the text into the two fields.
		int nIndex = strName.Find('\t');
		if (nIndex != -1)
		{
			strPrice = strName.Mid(nIndex+1);
			strName = strName.Left(nIndex);
		}

		// Get the item.
		int nItem = GetItemData(lpDrawItemStruct->itemID);
		COnlineOrderFormItem& Item = m_pItems->GetAt(nItem);

		CFont cfText;
		CFont* pOldFont = NULL;

		// Make CDs bold.
		if (Item.m_fIsCD)
		{
			CFont* pFont = GetFont();
			if (pFont != NULL)
			{
				LOGFONT lf;
				if (pFont->GetLogFont(&lf))
				{
					// Create a new bolder font.
					lf.lfWeight = FW_BOLD;
					if (cfText.CreateFontIndirect(&lf))
					{
						pOldFont = pDC->SelectObject(&cfText);
					}
				}
			}
		}

		if (!strName.IsEmpty())
		{
			pDC->ExtTextOut(lpDrawItemStruct->rcItem.left+4,
				lpDrawItemStruct->rcItem.top,
				ETO_OPAQUE, &(lpDrawItemStruct->rcItem), strName, strName.GetLength(), NULL);
		}

		if (!strPrice.IsEmpty())
		{
			// Format the string correctly.
			double dPrice = atof(strPrice);
			Util::PriceString(strPrice, long(dPrice*100.0), TRUE, "Free");
			if (Item.m_fIsCD)
			{
				strPrice += " + S/H";
			}

			// Line it up where we want it.
			int nX = m_nPriceAlign-pDC->GetTextExtent(strPrice, strPrice.GetLength()).cx;
			pDC->ExtTextOut(nX,
				lpDrawItemStruct->rcItem.top,
				0, &(lpDrawItemStruct->rcItem), strPrice, strPrice.GetLength(), NULL);
		}

		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);

		if (pOldFont != NULL)
		{
			pDC->SelectObject(pOldFont);
		}
	}

	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));
}


/////////////////////////////////////////////////////////////////////////////
// CPGSOrderWizard

CPGSOrderWizard::CPGSOrderWizard(const CServerOrderFormInfo* pOrderFormInfo, CServerAccountInfo& AccountInfo, const CServerCountryInfoArray& Countries, IContentManager* pContentManager, IPathManager* pPathManager, LPCSTR InitialCheck /*NULL*/)
	: CPmwWizard(IDS_OrderWizardTitle)
{
	// Remember the order form.
	m_pOrderFormInfo = pOrderFormInfo;

	// Remember the account info.
	m_AccountInfo = AccountInfo;

	// Remember the countries.
	m_Countries.Copy(Countries);

	// Remember the content manager.
	m_pContentManager = pContentManager;

	m_psh.dwFlags &= ~PSH_HASHELP;
	
	m_OrderFormPage.m_csInitialCheck = InitialCheck;

	SetWizardMode();

	// Add the pages.
	AddPage(&m_OrderFormPage);
	AddPage(&m_UserInformationPage);
	AddPage(&m_ConfirmationPage);
}

CPGSOrderWizard::~CPGSOrderWizard(void)
{
}

void CPGSOrderWizard::InitDialog() 
{
	INHERITED::InitDialog();
}

BEGIN_MESSAGE_MAP(CPGSOrderWizard, CPmwWizard)
	//{{AFX_MSG_MAP(CPGSOrderWizard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


