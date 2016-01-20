/*
// $Workfile: Eventwiz.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/Eventwiz.cpp $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 2     9/03/98 4:03p Mwilson
// initialized font pointer to NULL
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:26   Fred
// Initial revision.
// 
//    Rev 1.2   26 Feb 1997 13:52:48   Fred
// Fixed some problems with sender books
// 
//    Rev 1.1   25 Feb 1997 09:54:22   Fred
// Support for sender events
// 
//    Rev 1.0   20 Feb 1997 13:55:48   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "remind.h"
#include "addrbook.h"
#include "util.h"
#include "eventwiz.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CEventWizardError, CException);

extern CPalette *pOurPal;		// THE Palette


/////////////////////////////////////////////////////////////////////////////
// CAddressBookNameArray
// Class for holding an array of name ID's

CAddressBookIdArray::CAddressBookIdArray()
{
}

CAddressBookIdArray::~CAddressBookIdArray()
{
}


/////////////////////////////////////////////////////////////////////////////
// CAddressNameListBox
//

WNDPROC CAddressNameListBox::m_pfnSuperWndProc = NULL;

CAddressNameListBox::CAddressNameListBox()
{
	m_pCallback = NULL;
	m_dwCallbackData = 0;
}

CAddressNameListBox::~CAddressNameListBox()
{
}

void CAddressNameListBox::SetGetNameCallback(GetNameCallback pCallback, DWORD dwData)
{
	m_pCallback = pCallback;
	m_dwCallbackData = dwData;
}

BEGIN_MESSAGE_MAP(CAddressNameListBox, CListBox)
	//{{AFX_MSG_MAP(CAddressNameListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressNameListBox message handlers

void CAddressNameListBox::DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct)
{
	// Get the item number to draw.
	if (pDrawItemStruct->itemID != -1)
	{
		if ((pDrawItemStruct->itemAction & (ODA_DRAWENTIRE|ODA_SELECT|ODA_FOCUS)) != 0)
		{
			// Get the DC and rectangle to use.
			CDC* pDC = CDC::FromHandle(pDrawItemStruct->hDC);
								
			if (pDC != NULL)
			{
				// Get the colors to use.
				COLORREF clForeground;
				COLORREF clBackground;
				
				if ((pDrawItemStruct->itemState & ODS_SELECTED) != 0)
				{
					// Selected colors.
					clForeground = GetSysColor(COLOR_HIGHLIGHTTEXT);
					clBackground = GetSysColor(COLOR_HIGHLIGHT);
				}
				else
				{
					// Non selected colors.
					clForeground = GetSysColor(COLOR_WINDOWTEXT);
					clBackground = GetSysColor(COLOR_WINDOW);
				}
		
				// Compute the bounding rectangle for the item.
				CRect crItem(pDrawItemStruct->rcItem);
												
				// Draw the background.
				CBrush cbBackground(clBackground);
				pDC->FillRect(crItem, &cbBackground);
							
				// Draw the item string.
				if (m_pCallback != NULL)
				{
					CString csName;
					
					if ((*m_pCallback)(m_dwCallbackData, pDrawItemStruct->itemData, csName))
					{
						COLORREF clOldForeground = pDC->SetTextColor(clForeground);
						COLORREF clOldBackground = pDC->SetBkColor(clBackground);
				
						pDC->TextOut(crItem.left, crItem.top+1, csName,  csName.GetLength());
					
						pDC->SetTextColor(clOldForeground);
						pDC->SetBkColor(clOldBackground);
					}
				}
				
				// If the item has the focus, draw the focus indicator.
				if ((pDrawItemStruct->itemState & ODS_FOCUS) != 0)
				{
					pDC->DrawFocusRect(crItem);
				}
			}
		}
	}
}

void CAddressNameListBox::MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	pMeasureItemStruct->itemWidth = 0;
	pMeasureItemStruct->itemHeight = 0;
	
	// Set the item dimensions based on the current font.
		
	TEXTMETRIC tmFont;
		
	if (FontMetrics(tmFont))
	{
		CRect crClient;
		GetClientRect(crClient);
							
		pMeasureItemStruct->itemWidth = crClient.Width();
		pMeasureItemStruct->itemHeight = tmFont.tmHeight;
	}
}

BOOL CAddressNameListBox::FontMetrics(TEXTMETRIC& Metrics)
{
	BOOL fResult = FALSE;
	
	CClientDC cdcWnd(this);
					
	CFont* pcfWnd = GetFont();
				
	if (pcfWnd != NULL)
	{
		CFont* pcfOld = cdcWnd.SelectObject(pcfWnd);
					
		if (pcfOld != NULL)
		{
			if (cdcWnd.GetTextMetrics(&Metrics))
			{
				fResult = TRUE;
			}
						
			cdcWnd.SelectObject(pcfOld);
		}
	}
	
	return fResult;
}


/////////////////////////////////////////////////////////////////////////////
// CEventWizardError
// Exception class for reporting errors.

void CEventWizardError::Throw(DWORD dwError)
{
	THROW(new CEventWizardError(dwError));
}


/////////////////////////////////////////////////////////////////////////////
// CEventWizardPage

CEventWizardPage::CEventWizardPage(UINT nIDTemplate, UINT nIDCaption /*=0*/) :
	CPmwWizardPage(nIDTemplate, nIDCaption)
{
}

CEventWizardPage::CEventWizardPage(LPCTSTR lpszTemplateName, UINT nIDCaption /*=0*/) :
	CPmwWizardPage(lpszTemplateName, nIDCaption)
{
}

CEventWizardPage::~CEventWizardPage()
{
}

BEGIN_MESSAGE_MAP(CEventWizardPage, CPmwWizardPage)
	//{{AFX_MSG_MAP(CEventWizardPage)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CEventWizardPage::OnSetActive() 
{
	BOOL fResult = INHERITED::OnSetActive();
	if (fResult)
	{
		UpdateButtons();
		if (GetSafeHwnd() != NULL)
		{
			UpdateInterface();
		}
	}
	return fResult;
}

void CEventWizardPage::UpdateButtons(void) 
{
}

void CEventWizardPage::UpdateInterface(void) 
{
}

BOOL CEventWizardPage::OnEraseBkgnd(CDC* pDC) 
{
	CWnd* pBitmap = GetDlgItem(IDC_BITMAP);
	if (pBitmap != NULL)
	{
		CRect crBitmap;
		pBitmap->GetWindowRect(crBitmap);
		ScreenToClient(crBitmap);
		pDC->ExcludeClipRect(crBitmap);
	}
	return INHERITED::OnEraseBkgnd(pDC);
}


/////////////////////////////////////////////////////////////////////////////
// CEventWizardAddressBook dialog

CEventWizardAddressBook::CEventWizardAddressBook()
	: CEventWizardPage(CEventWizardAddressBook::IDD)
{
	//{{AFX_DATA_INIT(CEventWizardAddressBook)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_fUpdateInterface = TRUE;
}

CEventWizardAddressBook::~CEventWizardAddressBook()
{
}

void CEventWizardAddressBook::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventWizardAddressBook)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEventWizardAddressBook, CEventWizardPage)
	//{{AFX_MSG_MAP(CEventWizardAddressBook)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventWizardAddressBook message handlers

BOOL CEventWizardAddressBook::OnInitDialog()
{
	TRACE("AddressBook InitDialog\n");
	INHERITED::OnInitDialog();
	
	m_AddressBookList.SubclassDlgItem(IDC_LIST, this);
	
	UpdateInterface();
	
	return TRUE;
}

LRESULT CEventWizardAddressBook::OnWizardNext() 
{
	LRESULT lResult = -1;

	CString csAddressBookUserName;
	BOOL fIsSenderDatabase;
	TRY
	{
		// Get the address book name.
		if (m_AddressBookList.GetUserName(csAddressBookUserName, fIsSenderDatabase))
		{
			// Tell the wizard the address book name.
			// The wizard object will open the address book and throws
			// an exception if there's an error.
			SetAddressBookUserName(csAddressBookUserName, fIsSenderDatabase);

			// Move to the next page.
			lResult = 0;
		}
	}
	CATCH_ALL(e)
	{
		ReportError(e);
	}
	END_CATCH_ALL

	return lResult;
}

void CEventWizardAddressBook::OnSelchangeList()
{
	UpdateButtons();
}

void CEventWizardAddressBook::UpdateButtons()
{
	DWORD dwButtons = 0;
	if (m_AddressBookList.GetCurSel() != LB_ERR)
	{
		dwButtons |= PSWIZB_NEXT;
	}
	SetWizardButtons(dwButtons);
}

void CEventWizardAddressBook::UpdateInterface(void)
{
	INHERITED::UpdateInterface();
	
	if (m_fUpdateInterface)
	{
		TRACE("Address Book Update\n");
		m_AddressBookList.Refresh(GetAddressBookUserName(), GetIsSenderDatabase());
		m_fUpdateInterface = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEventWizardType dialog

CEventWizardType::CEventWizardType()
	: CEventWizardPage(CEventWizardType::IDD)
{
	//{{AFX_DATA_INIT(CEventWizardType)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEventWizardType::~CEventWizardType()
{
}

void CEventWizardType::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventWizardType)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEventWizardType, CEventWizardPage)
	//{{AFX_MSG_MAP(CEventWizardType)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventWizardType message handlers

BOOL CEventWizardType::OnInitDialog()
{
	INHERITED::OnInitDialog();
	GetConfiguration()->ReplaceDialogText(this);
	
	m_TypeList.SubclassDlgItem(IDC_LIST, this);
	
	UpdateInterface();
	
	return TRUE;
}

LRESULT CEventWizardType::OnWizardNext() 
{
	LRESULT lResult = -1;

	CString csAddressBookName;
	TRY
	{
		// Get the selectd field.
		int nFieldIndex = m_TypeList.GetCurSel();
		if (nFieldIndex >= 0)
		{
			// Tell the wizard which field was chose (All = -1).
			DWORD dwFieldNumber = m_TypeList.GetItemData(nFieldIndex);
			SetEventTypeId(dwFieldNumber);
			
			// Move to the next page.
			lResult = 0;
		}
	}
	CATCH_ALL(e)
	{
		ReportError(e);
	}
	END_CATCH_ALL

	return lResult;
}

void CEventWizardType::OnSelchangeList()
{
	UpdateButtons();
}

void CEventWizardType::UpdateButtons()
{
	DWORD dwButtons = PSWIZB_BACK;
	if (m_TypeList.GetCurSel() != LB_ERR)
	{
		dwButtons |= PSWIZB_NEXT;
	}
	SetWizardButtons(dwButtons);
}

void CEventWizardType::UpdateInterface(void)
{
	INHERITED::UpdateInterface();
	
	if ((m_csAddressBookUserName.IsEmpty) || (GetAddressBookUserName() != m_csAddressBookUserName))
	{
		TRACE("Type Update\n");
		m_TypeList.SetRedraw(FALSE);
		m_TypeList.ResetContent();
		
//		CString csAllName;
		CMlsStringArray csStandardNames;
		CStringArray csaFieldNames;

		// Build the event type name list box. This is basically
		// a list of field names. If the database is a PMG standard
		// database, then Birthday, Anniversary, and User Defined Date
		// are included. If the database is an imported database, then
		// all fields are used. Of course, only fields which have
		// a string type are included.
		
		TRY
		{
		
// For now, the "All" event type is not supported.
// It can lead to LOTS of events and LOTS of time required
// to check the events. The user can still add a reminder
// for each event type to achieve the same effect. It's just
// more work which is the whole point. Other code will need
// to be changed to support the "All" event type.
#if 0
			// Get name to use for "All" event type.
			csAllName;
			csAllName.LoadString(IDS_ALL_EVENT_TYPE_NAME);
			
			int nAllIndex = m_TypeList.AddString(csAllName);
			if (nAllIndex != LB_ERR)
			{
				m_TypeList.SetItemData(nAllIndex, (DWORD)-1);
			}
#endif

			// Get array of date field names in standard PMG database.
			csStandardNames.SetNames(IDS_STANDARD_DATE_FIELD_NAMES);
			
			// Get database cursor.
			CFlatFileDatabaseCursor* pCursor = GetIdCursor();
			if (pCursor != NULL)
			{
				TRY
				{
					for (DWORD dwField = 0; dwField < pCursor->GetFieldCount(); dwField++)
					{
						CFlatFileDatabaseField* pField = pCursor->GetField(dwField);
						if (pField != NULL)
						{
							if ((pField->GetName() != NULL)
							 && (pField->GetType() == FFT_String))
							{
								if ((!GetIsStandardFormat())
								 || (csStandardNames.FindName(pField->GetName()) >= 0))
								{
									int nIndex = m_TypeList.AddString(pField->GetName());
									if (nIndex != LB_ERR)
									{
										m_TypeList.SetItemData(nIndex, dwField);
									}
								}
							}
						}
					}
				}
				END_TRY
			}
		}
		END_TRY
	
		// Run through the items and select the one with the current event id.
		int nCount = m_TypeList.GetCount();
		if (nCount != LB_ERR)
		{
			for (int nIndex = 0; nIndex < nCount; nIndex++)
			{
				if (m_TypeList.GetItemData(nIndex) == GetEventTypeId())
				{
					m_TypeList.SetCurSel(nIndex);
					break;
				}
			}
		}
		
		m_TypeList.SetRedraw(TRUE);
		m_TypeList.Invalidate();
		
		m_csAddressBookUserName = GetAddressBookUserName();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CEventWizardName dialog

CEventWizardName::CEventWizardName()
	: CEventWizardPage(CEventWizardName::IDD)
{
	//{{AFX_DATA_INIT(CEventWizardName)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEventWizardName::~CEventWizardName()
{
}

void CEventWizardName::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventWizardName)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEventWizardName, CEventWizardPage)
	//{{AFX_MSG_MAP(CEventWizardName)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventWizardName message handlers

BOOL CEventWizardName::OnInitDialog()
{
	INHERITED::OnInitDialog();
	
	m_NameList.SubclassDlgItem(IDC_LIST, this);
	
	UpdateInterface();
	
	return TRUE;
}

BOOL CEventWizardName::DoGetNameFromId(DWORD dwData, DWORD dwId, CString& csName)
{
	ASSERT(dwData != 0);
	return ((CEventWizardName*)((void *)dwData))->GetNameFromId(dwId, csName);
}

LRESULT CEventWizardName::OnWizardNext() 
{
	LRESULT lResult = -1;

	TRY
	{
		int nIndex = m_NameList.GetCurSel();
		if (nIndex != LB_ERR)
		{
			// Get the ID associated with the name.
			DWORD dwId = m_NameList.GetItemData(nIndex);
			
			// Set the Name ID in the Wizard.
			SetNameId(dwId);
			
			// Move to the next page.
			lResult = 0;
		}
	}
	CATCH_ALL(e)
	{
		ReportError(e);
	}
	END_CATCH_ALL

	return lResult;
}

void CEventWizardName::OnSelchangeList()
{
	UpdateButtons();
}

void CEventWizardName::UpdateButtons()
{
	DWORD dwButtons = PSWIZB_BACK;
	if (m_NameList.GetCurSel() != LB_ERR)
	{
		dwButtons |= PSWIZB_NEXT;
	}
	SetWizardButtons(dwButtons);
}

void CEventWizardName::UpdateInterface(void) 
{
	INHERITED::UpdateInterface();
	
	if ((m_csAddressBookUserName.IsEmpty) || (GetAddressBookUserName() != m_csAddressBookUserName))
	{
		TRACE("Name Update\n");
		m_NameList.SetRedraw(FALSE);
		m_NameList.ResetContent();
		
		// Add in special "All" entry.
		int nAllIndex = m_NameList.AddString("");
		if (nAllIndex != LB_ERR)
		{
			m_NameList.SetItemData(nAllIndex, (DWORD)-1);
		}
	
		TRY
		{
			CFlatFileDatabaseCursor* pCursor = GetNameCursor();
			
			// If there is no name cursor, then just retrieve the names in "ID" order.
			if (pCursor == NULL)
			{
				pCursor = GetIdCursor();
			}
			
			if (pCursor != NULL)
			{
				// Collect the ID's of all the entries in the address book
				// in the order defined by the index. Id's which belong to
				// address book entries with an empty name are discarded.
				
				CFlatFileDatabaseField* pIdField = pCursor->GetField("__ID__");
				if (pIdField != NULL)
				{
					pCursor->First();
								
					while (!pCursor->AtEOF())
					{
						// Get the name.
						CString csName;
						if (GetNameFromCursor(pCursor, csName))
						{
							int nIndex = m_NameList.AddString("");
							if (nIndex != LB_ERR)
							{
								m_NameList.SetItemData(nIndex, pIdField->GetAsUnsigned32());
							}
						}
									
						pCursor->Next();
					}
				}
			}
		}
		END_TRY
		
		m_NameList.SetGetNameCallback(DoGetNameFromId, (DWORD)this);
		
		// Run through the items and select the one with the current name id.
		int nCount = m_NameList.GetCount();
		if (nCount != LB_ERR)
		{
			for (int nIndex = 0; nIndex < nCount; nIndex++)
			{
				if (m_NameList.GetItemData(nIndex) == GetNameId())
				{
					m_NameList.SetCurSel(nIndex);
					break;
				}
			}
		}
		
		m_NameList.SetRedraw(TRUE);
		m_NameList.Invalidate();
	
		m_csAddressBookUserName = GetAddressBookUserName();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CEventWizardDays dialog

CEventWizardDays::CEventWizardDays()
	: CEventWizardPage(CEventWizardDays::IDD)
{
	//{{AFX_DATA_INIT(CEventWizardDays)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEventWizardDays::~CEventWizardDays()
{
}

void CEventWizardDays::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventWizardDays)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEventWizardDays, CEventWizardPage)
	//{{AFX_MSG_MAP(CEventWizardDays)
	ON_EN_CHANGE(IDC_DAYS, OnChangeDays)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventWizardDays message handlers

BOOL CEventWizardDays::OnInitDialog()
{
	INHERITED::OnInitDialog();

	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_DAYS);
	if (pEdit != NULL)
	{
		pEdit->LimitText(6);
		
		CString csInitialData;
		TRY
		{
			int nDays = GetDays();
			if (nDays == -1)
			{
				csInitialData.LoadString(IDS_INITIAL_DAYS_COUNT);
			}
			else
			{
				csInitialData.Format("%d", nDays);
			}
			pEdit->SetWindowText(csInitialData);
		}
		END_TRY
	}
	
	UpdateInterface();
	
	return TRUE;
}

LRESULT CEventWizardDays::OnWizardNext() 
{
	LRESULT lResult = -1;

	CString csDays;
	
	TRY
	{
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_DAYS);
		if (pEdit != NULL)
		{
			pEdit->GetWindowText(csDays);
			if (Util::ValidUnsignedNumber(csDays, 0, 99))
			{
				// Tell the wizard the number of days.
				SetDays(atoi(csDays));
				
				// Move to the next page.
				lResult = 0;
			}
		}
	}
	CATCH_ALL(e)
	{
		ReportError(e);
	}
	END_CATCH_ALL

	return lResult;
}

void CEventWizardDays::UpdateButtons()
{
	DWORD dwButtons = PSWIZB_BACK;
	
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_DAYS);
	if (pEdit != NULL)
	{
		CString csDays;
		
		TRY
		{
			pEdit->GetWindowText(csDays);
			if (Util::ValidUnsignedNumber(csDays, 0, 99))
			{
				dwButtons |= PSWIZB_NEXT;
			}
		}
		END_TRY
	}
	
	SetWizardButtons(dwButtons);
}

void CEventWizardDays::UpdateInterface()
{
	INHERITED::UpdateInterface();
	TRACE("Days Update\n");
}

void CEventWizardDays::OnChangeDays()
{
	UpdateButtons();
}


/////////////////////////////////////////////////////////////////////////////
// CEventWizardCongratulations dialog

CEventWizardCongratulations::CEventWizardCongratulations()
	: CEventWizardPage(CEventWizardCongratulations::IDD),
	m_pFont(NULL)
{
	//{{AFX_DATA_INIT(CEventWizardCongratulations)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEventWizardCongratulations::~CEventWizardCongratulations()
{
	delete m_pFont;
}

void CEventWizardCongratulations::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventWizardCongratulations)
	DDX_Control(pDX, IDC_STATIC_TM, m_cStaticText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEventWizardCongratulations, CEventWizardPage)
	//{{AFX_MSG_MAP(CEventWizardCongratulations)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventWizardCongratulations message handlers

HBRUSH CEventWizardCongratulations::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CEventWizardPage::OnCtlColor(pDC, pWnd, nCtlColor);
	//we have to set the font to arial because the default font does not 
	//have the trademark symbol in its char set.  Sice this is a prop
	//page it uses the prop sheet font which I was unable to change.
	CFont* pFont = new CFont;
	pFont->CreatePointFont(85, "Arial");
	CFont* pOldFont = pDC->SelectObject(pFont);
	pFont->Detach();
	delete pFont;
	pOldFont->DeleteObject();;
	return hbr;
}

BOOL CEventWizardCongratulations::OnInitDialog()
{
	INHERITED::OnInitDialog();
	m_pFont = new CFont;
	m_pFont->CreatePointFont(80, "Arial");
	GetConfiguration()->ReplaceDialogText(this);
	
	return TRUE;
}

LRESULT CEventWizardCongratulations::OnWizardNext() 
{
	LRESULT lResult = -1;

	TRY
	{
		// Move to the next page.
		lResult = 0;
	}
	CATCH_ALL(e)
	{
		ReportError(e);
	}
	END_CATCH_ALL

	return lResult;
}

void CEventWizardCongratulations::UpdateButtons()
{
	DWORD dwButtons = PSWIZB_FINISH|PSWIZB_BACK;
	SetWizardButtons(dwButtons|PSWIZB_BACK);
}

void CEventWizardCongratulations::UpdateInterface()
{
	INHERITED::UpdateInterface();
	
	TRACE("Congratulations Update\n");
		
	CEdit* pEdit;
		
	// Initialize address book display.
	if ((pEdit = (CEdit*)GetDlgItem(IDC_ADDRESS_BOOK_DISPLAY)) != NULL)
	{
		pEdit->SetWindowText(GetAddressBookUserName());
	}
		
	// Initialize event type display.
	if ((pEdit = (CEdit*)GetDlgItem(IDC_TYPE_DISPLAY)) != NULL)
	{
		CString csType;
		TRY
		{
			GetEventType(csType);
			pEdit->SetWindowText(csType);
		}
		END_TRY
	}
		
	// Initialize name display.
	if ((pEdit = (CEdit*)GetDlgItem(IDC_NAME_DISPLAY)) != NULL)
	{
		CString csName;
		TRY
		{
			GetNameFromId(GetNameId(), csName);
			pEdit->SetWindowText(csName);
		}
		END_TRY
	}
		
	// Initialize days display.
	if ((pEdit = (CEdit*)GetDlgItem(IDC_DAYS_DISPLAY)) != NULL)
	{
		CString csDays;
		TRY
		{
			csDays.Format("%d", GetDays());
			pEdit->SetWindowText(csDays);
		}
		END_TRY
	}
}


/////////////////////////////////////////////////////////////////////////////
// CEventWizard

CEventWizard::CEventWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPmwWizard(nIDCaption, pParentWnd, iSelectPage)
{
	CommonConstruct();
}

CEventWizard::CEventWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPmwWizard(pszCaption, pParentWnd, iSelectPage)
{
	CommonConstruct();
} 

void CEventWizard::CommonConstruct(void)
{
	m_dwEventTypeId = (DWORD)-1;
	m_dwNameId = (DWORD)-1;
	m_nDays = -1;

	Util::LoadResourceBitmap(m_Bitmap, "EVENTWIZ", pOurPal);
}

CEventWizard::~CEventWizard()
{
}

void CEventWizard::InitDialog() 
{
	INHERITED::InitDialog();
	
	CString csTitle;
	TRY
	{
		csTitle.LoadString(IDS_EVENT_WIZARD);
		SetWindowText(csTitle);
	}
	GetConfiguration()->ReplaceDialogText(this);
	END_TRY
}

BEGIN_MESSAGE_MAP(CEventWizard, CPmwWizard)
	//{{AFX_MSG_MAP(CEventWizard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CEvent* CEventWizard::Run(CWnd* pParent)
{
	return Run(pParent, NULL);
}

CEvent* CEventWizard::Run(CWnd* pParent, CEvent* pOldEvent)
{
	CEvent* pEvent = NULL;

	CEventWizard Wizard("", pParent, 0);
	
	TRY
	{
		if (pOldEvent != NULL)
		{
			Wizard.m_csAddressBookUserName = pOldEvent->GetAddressBookUserName();
			Wizard.m_fIsSenderDatabase = pOldEvent->GetIsSenderDatabase();
			Wizard.m_dwEventTypeId = pOldEvent->GetEventTypeId();
			Wizard.m_dwNameId = pOldEvent->GetNameId();
			Wizard.m_nDays = pOldEvent->GetDays();
		}
	}
	END_TRY
	
#ifdef WIN32
	Wizard.m_psh.dwFlags &= ~PSH_HASHELP;
	Wizard.m_AddressBookPage.m_psp.dwFlags &= ~PSP_HASHELP;
	Wizard.m_TypePage.m_psp.dwFlags &= ~PSP_HASHELP;
	Wizard.m_NamePage.m_psp.dwFlags &= ~PSP_HASHELP;
	Wizard.m_DaysPage.m_psp.dwFlags &= ~PSP_HASHELP;
	Wizard.m_CongratulationsPage.m_psp.dwFlags &= ~PSP_HASHELP;
#endif

	Wizard.AddPage(&Wizard.m_AddressBookPage);
	Wizard.AddPage(&Wizard.m_TypePage);
	Wizard.AddPage(&Wizard.m_NamePage);
	Wizard.AddPage(&Wizard.m_DaysPage);
	Wizard.AddPage(&Wizard.m_CongratulationsPage);
	Wizard.SetWizardMode();

	if (Wizard.DoModal() == ID_WIZFINISH)
	{
		// Everything finished, build the object representing the event.
		TRY
		{
			pEvent = new CEvent(
								Wizard.GetAddressBookUserName(),
								Wizard.GetIsSenderDatabase(),
								Wizard.GetEventTypeId(),
								Wizard.GetNameId(),
								Wizard.GetDays());
		}
		END_TRY
	}

	return pEvent;
}

void CEventWizard::ReportError(CException* pException) const
{
	if (pException->IsKindOf(RUNTIME_CLASS(CEventWizardError)))
	{
		switch (((CEventWizardError*)pException)->GetError())
		{
			case CEventWizard::ERROR_CantAccessAddressBook:
			{
				AfxMessageBox(IDS_EVENT_WIZARD_CANT_ACCESS_ADDRESS_BOOK);
				break;
			}
			case CEventWizard::ERROR_NoFieldNames:
			{
				AfxMessageBox(IDS_EVENT_WIZARD_NO_FIELD_NAMES);
				break;
			}
			default:
			{
				AfxMessageBox(IDS_EVENT_WIZARD_CANT_CREATE_EVENT);
				break;
			}
		}
	}
	else
	{
		AfxMessageBox(IDS_EVENT_WIZARD_CANT_CREATE_EVENT);
	}
}

void CEventWizard::SetAddressBookUserName(LPCSTR pszAddressBookUserName, BOOL fIsSenderDatabase)
{
	// Save the address book name.
	TRY
	{
		m_csAddressBookUserName = pszAddressBookUserName;
		m_fIsSenderDatabase = fIsSenderDatabase;
	}
	END_TRY
	
	// Open up the address book database.
	ERRORCODE Error = GetAddressBook()->OpenAddressBook(pszAddressBookUserName, fIsSenderDatabase);
	if (Error != ERRORCODE_None)
	{
		CEventWizardError::Throw(IDS_EVENT_WIZARD_CANT_ACCESS_ADDRESS_BOOK);
	}
			
	// Make sure the database has some fields.
	if (GetAddressBook()->GetIdCursor()->GetFieldCount() == 0)
	{
		CEventWizardError::Throw(IDS_EVENT_WIZARD_NO_FIELD_NAMES);
	}
}

void CEventWizard::SetEventTypeId(DWORD dwEventTypeId)
{
	m_dwEventTypeId = dwEventTypeId;
}

void CEventWizard::GetEventType(CString& csEventType)
{
	csEventType.Empty();
	
	TRY
	{
		DWORD dwEventTypeId = GetEventTypeId();
		if (dwEventTypeId == (DWORD)-1)
		{
			csEventType.LoadString(IDS_ALL_EVENT_TYPE_NAME);
		}
		else
		{
			CFlatFileDatabaseCursor* pIdCursor = GetAddressBook()->GetIdCursor();
			if (pIdCursor != NULL)
			{
				CFlatFileDatabaseField* pField = pIdCursor->GetField(dwEventTypeId);
				if (pField != NULL)
				{
					csEventType = pField->GetName();
				}
			}
		}
	}
	END_TRY
}

void CEventWizard::SetNameId(DWORD dwId)
{
	m_dwNameId = dwId;
}


