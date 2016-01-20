//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/DownDlg.cpp 1     3/03/99 6:05p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/DownDlg.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 10    12/16/98 6:33p Psasse
// No more prompting OK/Cancel with every directory deletion (on multiple
// selection)
// 
// 9     11/25/98 6:00p Psasse
// Attempt to uncheck deleted collection
// 
// 8     11/23/98 8:28p Psasse
// Refresh when deleting collections
// 
// 7     11/22/98 12:15a Psasse
// Enable/Disable Delete button
// 
// 6     11/21/98 10:45p Psasse
// 
// 5     11/18/98 10:34p Psasse
// Deleting specific downloaded collections support
// 
// 4     10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 3     10/26/98 6:47p Jayn
// Download manager.
// 
// 2     10/24/98 7:34p Jayn
// Download Manager and Download Dialog
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "DownDlg.h"
#include "Downmgr.h"
#include "pmdirdlg.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


UINT CDownloadDialog::m_uContentChangedMessage = ::RegisterWindowMessage("PMContentChanged");

/////////////////////////////////////////////////////////////////////////////
// CCollectionListBox
CCollectionListBox::CCollectionListBox()
{
	m_nDownloadedAlign = -1;
}

CCollectionListBox::~CCollectionListBox()
{
}

void CCollectionListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// You must override DrawItem and MeasureItem for LBS_OWNERDRAWVARIABLE
	ASSERT((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS)) ==
		(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS));

	ASSERT(m_nDownloadedAlign != -1);

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
		CString strDownloaded;
		GetText(lpDrawItemStruct->itemID, strName);

		// Split the text into the two fields.
		int nIndex = strName.Find('\t');
		if (nIndex != -1)
		{
			strDownloaded = strName.Mid(nIndex+1);
			strName = strName.Left(nIndex);
		}

		if (!strName.IsEmpty())
		{
			pDC->ExtTextOut(lpDrawItemStruct->rcItem.left+4,
				lpDrawItemStruct->rcItem.top + max(0, (cyItem - m_cyText) / 2),
				ETO_OPAQUE, &(lpDrawItemStruct->rcItem), strName, strName.GetLength(), NULL);
		}

		if (!strDownloaded.IsEmpty())
		{
			// Line it up where we want it.
			int nX = m_nDownloadedAlign-pDC->GetTextExtent(strDownloaded, strDownloaded.GetLength()).cx/2;
			pDC->ExtTextOut(nX,
				lpDrawItemStruct->rcItem.top + max(0, (cyItem - m_cyText) / 2),
				0, &(lpDrawItemStruct->rcItem), strDownloaded, strDownloaded.GetLength(), NULL);
		}

		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);
	}

	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));
}

/////////////////////////////////////////////////////////////////////////////
// CDownloadDialog dialog

CDownloadDialog::CDownloadDialog(CDownloadCollectionArray* pCollections, CString* pcsDownloadDirectory, LPCSTR pszChosenCollection /*=NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(CDownloadDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDownloadDialog)
	m_csDownloadDirectory = _T("");
	//}}AFX_DATA_INIT

	m_pCollections = pCollections;
	m_pszChosenCollection = pszChosenCollection;
	m_pcsDownloadDirectory = pcsDownloadDirectory;
	m_csDownloadDirectory = *pcsDownloadDirectory;
}

void CDownloadDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownloadDialog)
	DDX_Text(pDX, IDC_DOWNLOAD_DIRECTORY, m_csDownloadDirectory);
	DDV_MaxChars(pDX, m_csDownloadDirectory, 256);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		// We want to copy the selections back to the list.
		int nCount = m_CollectionList.GetCount();
		for (int nIndex = 0; nIndex < nCount; nIndex++)
		{
			int n = m_CollectionList.GetItemData(nIndex);
			m_pCollections->ElementAt(n).m_fDownload = m_CollectionList.GetCheck(nIndex);
		}
		// Pass back the directory.
		*m_pcsDownloadDirectory = m_csDownloadDirectory;
	}
}

BEGIN_MESSAGE_MAP(CDownloadDialog, CDialog)
	//{{AFX_MSG_MAP(CDownloadDialog)
	ON_CLBN_CHKCHANGE(IDC_COLLECTION_LIST, OnChkchangeCollectionList)
	ON_LBN_DBLCLK(IDC_COLLECTION_LIST, OnDblclkCollectionList)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_DELETE_FILES, OnDeleteFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownloadDialog message handlers

BOOL CDownloadDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_CollectionList.SubclassDlgItem(IDC_COLLECTION_LIST, this);

	CWnd* pWnd = GetDlgItem(IDC_DOWNLOADED_TITLE);
	ASSERT(pWnd != NULL);
	if (pWnd != NULL)
	{
		CRect crWindow;
		pWnd->GetWindowRect(crWindow);
		m_CollectionList.ScreenToClient(crWindow);
		m_CollectionList.SetDownloadedAlign((crWindow.left + crWindow.right)/2);
	}

	// Build the collection list.
	BuildCollectionList();

	// Update the download button.
	UpdateDownloadButton();

	return TRUE;
}

//
// Build the collection list.
//

void CDownloadDialog::BuildCollectionList(void)
{
	CString csChosenName;

	m_CollectionList.SetRedraw(FALSE);
	m_CollectionList.ResetContent();

	// Add a string for each of the collections.
	for (int n = 0; n < m_pCollections->GetSize(); n++)
	{
		CDownloadCollection& Collection = m_pCollections->ElementAt(n);
		CString csText;
		csText = Collection.m_Info.m_csDescription;
		csText += '\t';
		csText += Collection.GetStateString();
		int nIndex = m_CollectionList.AddString(csText);
		if (nIndex >= 0)
		{
			// Set the item data to be this index in case it sorts.
			m_CollectionList.SetItemData(nIndex, (DWORD)n);
			// Initialize the check.
			BOOL fChecked = FALSE;
			if (m_pszChosenCollection != NULL
				 && Collection.m_Info.m_csName == m_pszChosenCollection
				 && Collection.m_nState != CDownloadCollection::STATE_Downloaded)
			{
				fChecked = TRUE;
				csChosenName = csText;
			}
			m_CollectionList.SetCheck(nIndex, fChecked);
		}
	}

	if (!csChosenName.IsEmpty())
	{
		m_CollectionList.SelectString(-1, csChosenName);
	}
	m_CollectionList.SetRedraw(TRUE);
	m_CollectionList.Invalidate();
}

//
// Update the download button depending on the state of the checks in
// the listbox.
//

BOOL CDownloadDialog::SomethingChecked(void)
{
	// Run through the items in the checkbox and see if any are checked.
	BOOL fChecked = FALSE;
	// We want to copy the selections back to the list.
	int nCount = m_CollectionList.GetCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		if (m_CollectionList.GetCheck(nIndex))
		{
			fChecked = TRUE;
			break;
		}
	}
	return fChecked;
}


//
// Update the download button to reflect whether something is checked or not.
//

void CDownloadDialog::UpdateDownloadButton(void)
{
	// Enable the button as we have decided.
	CWnd* pWnd = GetDlgItem(IDC_DELETE_FILES);
	CWnd* pWnd2 = GetDlgItem(IDOK);
	if (pWnd != NULL)
	{
		pWnd->EnableWindow(SomethingChecked());
	}
	if (pWnd2 != NULL)
	{
		pWnd2->EnableWindow(SomethingChecked());
	}
}

//
// The check state of an item has changed.
//

void CDownloadDialog::OnChkchangeCollectionList() 
{
	UpdateDownloadButton();
}

//
// Double-click on the list does the same as the Download button (IDOK).
//

void CDownloadDialog::OnDblclkCollectionList() 
{
	// Checking the OK window enabled state does not work!
	if (SomethingChecked())
	{
		OnOK();
	}
}

void CDownloadDialog::OnBrowse() 
{
	CWnd* pWnd = GetDlgItem(IDC_DOWNLOAD_DIRECTORY);
	if (pWnd != NULL)
	{
		CString csDirectory;
		pWnd->GetWindowText(csDirectory);
		Util::RemoveBackslashFromPath(csDirectory);

		CChooseDirectoryDialog Dialog;
		Dialog.m_csDirectory = csDirectory;
		if (Dialog.GetDirectory(CChooseDirectoryDialog::OPTION_no_exists_check |
										CChooseDirectoryDialog::OPTION_expand)
					&& !Dialog.m_csDirectory.IsEmpty())
		{
			pWnd->SetWindowText(Dialog.m_csDirectory);
		}
	}
}

void CDownloadDialog::OnDeleteFiles() 
{

	int nCount = m_CollectionList.GetCount();

	if(nCount > 0)
	{
		if(AfxMessageBox(IDS_PGS_DELETE_DOWNLOADED_FILES, MB_OKCANCEL) == IDOK)
		{
			for (int nIndex = 0; nIndex < nCount; nIndex++)
			{
				if (m_CollectionList.GetCheck(nIndex))
				{
					int n = m_CollectionList.GetItemData(nIndex);
					CDownloadCollection& Collection = m_pCollections->ElementAt(n);

					if(GetGlobalContentManager()->DeleteCachedDirectories(NULL, Collection.m_Info.m_csName))
					{
						if(DoDelete(Collection, m_pszChosenCollection))
							(m_CollectionList.SetCheck(nIndex, 0));

					}
				}
			}
			EndDialog(IDC_DELETE_FILES);
		}
	}
}

// Do the delete.
BOOL CDownloadDialog::DoDelete(CDownloadCollection& Collection, LPCSTR pszDirectory)
{
	if (Collection.m_nState != CDownloadCollection::STATE_Downloaded)
	{
		return TRUE;
	}

	//
	// Now, do the Delete.
	//

	int nStatus = CSSTATUS_Success;
	int nFilesDeleted = 0;

	// Build the source name of the DIR file.
	CString csDirSourceName;
	csDirSourceName = Collection.m_Info.m_csFileName;
	// Sometimes these files end in '.'. Get rid of it if so.
	int nLength = csDirSourceName.GetLength();
	if (nLength > 0 && csDirSourceName[nLength-1] == '.')
	{
		csDirSourceName = csDirSourceName.Left(nLength-1);
	}

	// Stick the DIR extension on.
	csDirSourceName += ".dir";

	// Build the destination name of the DIR file.
	CString csDirFileName;
	
	Util::ConstructPath(csDirFileName, m_csDownloadDirectory, Collection.m_Info.m_csName + ".dir");

	// We have the DIR file.
	if (Collection.m_Dir.Read(csDirFileName))
	{
		// We have the directory.
		const CDownloadFileArray& Files = Collection.m_Dir.GetFiles();
		int nFiles = Files.GetSize();
		for (int nFile = 0; nFile < nFiles; nFile++)
		{

			// Get the file to Delete.
			const CDownloadFile& File = Files.GetAt(nFile);

			if (File.CheckDownloadState(m_csDownloadDirectory))
			{
				// We need to Delete it.
				// Construct the name of the file.
				CString csDestName;
				Util::ConstructPath(csDestName, m_csDownloadDirectory, File.m_csFileName);
				CFile::Remove(csDestName);
				nStatus = CSSTATUS_Success;
				nFilesDeleted ++;
			}

		}
		if (nStatus == CSSTATUS_Success)
		{
				CFile::Remove(csDirFileName);
				nFilesDeleted ++;
		}
	}


	// If we successfully Deleted any files...
	if (nFilesDeleted > 0)
	{
		// ...inform any interested party that the content has changed.
		::PostMessage(HWND_BROADCAST, m_uContentChangedMessage, 0, 0);
		
	}

	return (nStatus == CSSTATUS_Success);
}


/////////////////////////////////////////////////////////////////////////////
// CCollectionProgressDialog dialog

CCollectionProgressDialog::CCollectionProgressDialog()
{
	//{{AFX_DATA_INIT(CCollectionProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CCollectionProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDownloadStatusDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCollectionProgressDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCollectionProgressDialog, CDownloadStatusDialog)
	//{{AFX_MSG_MAP(CCollectionProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCollectionProgressDialog message handlers

BOOL CCollectionProgressDialog::OnInitDialog() 
{
	CDownloadStatusDialog::OnInitDialog();

	// Get the "collections left" format.
	GetDlgItemText(IDC_COLLECTIONS_LEFT, m_csCollectionsLeftFormat);
	SetDlgItemText(IDC_COLLECTIONS_LEFT, "");

	// Get the "current collection" format.
	GetDlgItemText(IDC_CURRENT_COLLECTION, m_csCurrentCollectionFormat);
	SetDlgItemText(IDC_CURRENT_COLLECTION, "");

	// Get the "files left" format.
	GetDlgItemText(IDC_FILES_LEFT, m_csFilesLeftFormat);
	SetDlgItemText(IDC_FILES_LEFT, "");

	return TRUE;
}

//
// Set how many collections are left.
//

void CCollectionProgressDialog::SetCollectionsLeft(int nCollectionsLeft)
{
	CString csText;
	csText.Format(m_csCollectionsLeftFormat, nCollectionsLeft);
	SetDlgItemText(IDC_COLLECTIONS_LEFT, csText);
}

//
// Set the name of the current collection.
//

void CCollectionProgressDialog::SetCurrentCollection(LPCSTR pszCollection)
{
	CString csText;
	csText.Format(m_csCurrentCollectionFormat, pszCollection);
	SetDlgItemText(IDC_CURRENT_COLLECTION, csText);
}

//
// Set how many files are left.
//

void CCollectionProgressDialog::SetFilesLeft(int nFilesLeft)
{
	CString csText;
	if (nFilesLeft == -1)
	{
		// Reading the directory file.
		csText.LoadString(IDS_ReadingCollectionDirectory);
	}
	else
	{
		csText.Format(m_csFilesLeftFormat, nFilesLeft);
	}
	SetDlgItemText(IDC_FILES_LEFT, csText);
}
