/*
// $Workfile: artstore.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/artstore.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 49    12/01/98 8:39p Psasse
// Better Download button disabling/enabling
// 
// 48    11/23/98 10:19p Psasse
// Clear "live art" flag in the OnInit
// 
// 47    11/23/98 3:58p Johno
// Changes for initial checked order item
// 
// 46    11/21/98 10:45p Psasse
// 
// 45    11/19/98 2:54p Mwilson
// changed error string
// 
// 44    11/18/98 10:30p Psasse
// New download button support
// 
// 43    11/17/98 7:05p Psasse
// added default bmp name for no downloaded packs
// 
// 42    11/17/98 5:49p Psasse
// Fixed UpdateWhenlastSeenInternetContent
// 
// 41    11/17/98 4:13p Dennis
// Added check for valid filename
// 
// 40    11/15/98 6:49p Psasse
// Enable brochure (pack art) when none has been downloaded, update flag
// for looking at new internet content
// 
// 39    11/11/98 5:39p Johno
// 
// 38    10/30/98 5:25p Jayn
// 
// 37    10/26/98 6:47p Jayn
// Download manager.
// 
// 36    10/24/98 7:34p Jayn
// Download Manager and Download Dialog
// 
// 35    10/23/98 6:25p Jayn
// New License routines. Changes to content server API.
// 
// 34    10/23/98 5:45p Johno
// Moved code in OnArtStorePreview() to mainfrm.cpp
// 
// 33    10/22/98 4:00p Jayn
// 
// 32    10/21/98 9:06p Psasse
// added PriceOrder support
// 
// 31    10/21/98 7:01p Jayn
// 
// 31    10/21/98 7:01p Jayn
// 
// 30    10/21/98 4:14p Jayn
// More improvements to the order process.
// 
// 29    10/19/98 9:18p Psasse
// added user information functionality to online purchasing
// 
// 28    10/19/98 5:41p Jayn
// Beginnings of the online order form.
// 
// 27    10/19/98 11:07a Jayn
// Fixes
// 
// 26    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 25    9/25/98 6:08p Jayn
// 
// 24    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 23    9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 22    8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 21    7/26/98 7:05p Hforman
// changed baseclass from CDialog to CPmwDialog for palette handling
// 
// 20    7/26/98 2:04p Jayn
// Minor string/flow changes
// 
// 19    7/14/98 6:34p Jayn
// 
// 18    7/09/98 10:45a Jayn
// New GetOrderForms & FindORDFiles
// 
// 17    7/08/98 1:45p Dennis
// Misc. Fixes
// 
// 16    7/08/98 1:04p Jayn
// Simplified filtering. Added Array variant to FillxxxFromList.
// 
// 15    7/01/98 5:21p Psasse
// better error checking and more robust filtering
// 
// 14    6/29/98 10:14a Psasse
// resolve conflicts
// 
// 13    6/29/98 9:45a Psasse
// New dynamic mechanism for the addition/deletion and filtering of
// artstore components
// 
// 12    6/23/98 1:50p Dennis
// Changed source for Bitmap Files
// 
// 11    6/18/98 4:30p Johno
// Deleted pFrameWnd->DestroyHub() - the function no longer exists.
// 
// 10    6/11/98 4:15p Dennis
// Removed Window Sizing
// 
// 9     6/05/98 6:19p Fredf
// Fixed problem with browser parent.
// 
// 8     6/05/98 4:59p Fredf
// 
// 7     6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 6     6/02/98 4:24p Rlovejoy
// Check for mouse down events so we can remove Hints.
// 
// 5     6/02/98 1:19p Rlovejoy
// Show helpful hint.
// 
// 4     5/27/98 6:36p Hforman
// remove color param from Browser dialog creation
// 
// 3     5/19/98 3:02p Dennis
// Error Message now loaded from resource file
// 
// ArtStore.cpp : implementation file
//
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"

#include "ArtStore.h"
#include "pmwcfg.h"
#include "util.h"
#include "licdll.h"
#include "bmptiler.h"
#include "browser.h"
#include "rmagcom.h"  //for resolution
#include "ddb.h"
#include "glicense.h"		// for LicenseData
#include "downmgr.h"			// CDownloadManager

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette* pOurPal;  // PrintMaster Palette

#define TAB_HEIGHT_PADDING 10
#define DIALOG_WIDTH       550
#define DIALOG_HEIGHT      (400 + TAB_HEIGHT_PADDING)


COrderFormInfo::~COrderFormInfo()
   {
      Empty();
   }

void COrderFormInfo::Empty()
   {
      int   nArraySize = m_cpaPackInfo.GetSize();
      const CUnlockPackInfo *pofiPackInfo;

      // Run through CPackInfo list and free objects
      for(int i=0;i < nArraySize;i++)
         {
            pofiPackInfo = m_cpaPackInfo.GetAt(i);
            delete pofiPackInfo;
         }
      m_cpaPackInfo.RemoveAll();
   }

ERRORCODE COrderFormInfo::Read(LPCSTR szOrderFileName)
   {
      ERRORCODE   errorcode = ERRORCODE_None;
      CString     csProductsEntry, csSection;

      // Remove any existing pack info.

      // First check for file existance
		SetFileName(szOrderFileName);
      ASSERT(!GetFileName().IsEmpty() && Util::FileExists(GetFileName()));
      if(GetFileName().IsEmpty() || !Util::FileExists(GetFileName()))
         return ERRORCODE_Open;

		// Setup the INI file so we can get the section names.
		CIniFile IniFile(GetFileName());

      // First determine which sections should be read based on "Products" entry
      csProductsEntry = IniFile.GetString("Offer", "Products");
      ASSERT(!csProductsEntry.IsEmpty());
      if(csProductsEntry.IsEmpty())
         return ERRORCODE_Fail;

      // First Word represents section prefix for entire unlockable set
      csSection = GetFirstWord(csProductsEntry);
      ASSERT(!csSection.IsEmpty());
      if(csSection.IsEmpty())
         return ERRORCODE_Fail;
      csSection += "_ID";

#if 0
      SetProductImageFileName(IniFile.GetString(csSection, "Image"));
      ASSERT(!GetProductImageFileName().IsEmpty());
#endif

      // Remaining words in Products entry should represent unlockable packs
      csSection = GetNextWord();
      while(!csSection.IsEmpty())
         {
            CUnlockPackInfo   *pPackInfo;

            csSection += "_ID";

            // Create new pack object
            pPackInfo = new CUnlockPackInfo;
            ASSERT(pPackInfo);
            if(pPackInfo == NULL)
               return ERRORCODE_Memory;

            pPackInfo->SetName(IniFile.GetString(csSection, "Name"));
            pPackInfo->SetCollectionBaseName(IniFile.GetString(csSection, "CollectionBaseName"));
            pPackInfo->SetImageFileName(IniFile.GetString(csSection, "Image"));
            pPackInfo->SetProductCode(IniFile.GetString(csSection, "Code"));
            pPackInfo->SetFilters(IniFile.GetString(csSection, "Filters"));
            TRACE("Pack Info: Name=%s Coll Name=%s Image=%s\n",
               pPackInfo->GetName(), pPackInfo->GetCollectionBaseName(),
               pPackInfo->GetImageFileName());
            if(pPackInfo->GetName().IsEmpty() ||
               pPackInfo->GetCollectionBaseName().IsEmpty() ||
					pPackInfo->GetImageFileName().IsEmpty())
               {
                  // Pack is missing information
                  ASSERT(0);
                  delete pPackInfo;
               }
				else
	            m_cpaPackInfo.Add(pPackInfo);

            csSection = GetNextWord();
         };

      if(m_cpaPackInfo.GetSize() <= 0)
         return ERRORCODE_Fail;

      return ERRORCODE_None;
   }

CString COrderFormInfo::GetFirstWord(LPCSTR szSourceString)
   {
      m_nCurCharIndex = 0;
      m_csSourceString = szSourceString;

      return GetNextWord();
   }

CString COrderFormInfo::GetNextWord()
   {
      int      nWordLen = 0, nStartWordIndex, nStringLen = m_csSourceString.GetLength();

      // Find Word start
      while(m_csSourceString[m_nCurCharIndex] == ' ' ||
            m_csSourceString[m_nCurCharIndex] == '(' ||
            m_csSourceString[m_nCurCharIndex] == ')' ||
            m_csSourceString[m_nCurCharIndex] == '\t' )
            {
               m_nCurCharIndex++;
               if(m_nCurCharIndex >= nStringLen)
                  return "";
               continue;
            }

      // Find end of word
      nStartWordIndex = m_nCurCharIndex;
      while(m_csSourceString[m_nCurCharIndex] != ' ' &&
            m_csSourceString[m_nCurCharIndex] != '(' &&
            m_csSourceString[m_nCurCharIndex] != ')' &&
            m_csSourceString[m_nCurCharIndex] != '\t' &&
            m_csSourceString[m_nCurCharIndex] != NULL)
            {
               nWordLen++;
               m_nCurCharIndex++;
            }
      if(nWordLen <= 0)
         return "";

      // Advance past end of this word in preparation for next call
      m_nCurCharIndex++;
      return m_csSourceString.Mid(nStartWordIndex, nWordLen);
   }

CUnlockPackInfo::CUnlockPackInfo()
   {
   }

CUnlockPackInfo::~CUnlockPackInfo()
   {
   }

/////////////////////////////////////////////////////////////////////////////
// CArtStoreDialog dialog

CArtStoreDialog::CArtStoreDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CArtStoreDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArtStoreDialog)
	//}}AFX_DATA_INIT

   m_bHasTabControl = FALSE;
   m_nCurTab = 0;
   m_cszTabImage.cx = m_cszTabImage.cy = 0;

	m_pPathManager = NULL;
	m_pCollectionManager = NULL;
	m_pContentManager = NULL;
	m_pCollectionToPreview = NULL;
}


CArtStoreDialog::~CArtStoreDialog()
{
	int nArraySize = m_cpaOrderInfo.GetSize();
	COrderFormInfo *pofiOrderInfo;

	// Run through OrderInfo list and free objects
	for(int i=0;i < nArraySize;i++)
	{
		pofiOrderInfo = m_cpaOrderInfo.GetAt(i);
		ASSERT(pofiOrderInfo);
		delete pofiOrderInfo;
	}
	m_cpaOrderInfo.RemoveAll();

	// Free our interfaces.
	if (m_pPathManager != NULL)
	{
		m_pPathManager->Release();
		m_pPathManager = NULL;
	}
	if (m_pCollectionManager != NULL)
	{
		m_pCollectionManager->Release();
		m_pCollectionManager = NULL;
	}
	if (m_pContentManager != NULL)
	{
		m_pContentManager->Release();
		m_pContentManager = NULL;
	}
}

void CArtStoreDialog::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArtStoreDialog)
	DDX_Control(pDX, IDC_ART_STORE_BROWSER_PREVIEW, m_btnPreview);
	DDX_Control(pDX, IDC_ART_STORE_ORDER, m_btnOrder);
	DDX_Control(pDX, IDC_DOWNLOAD, m_btnDownload);
	DDX_Control(pDX, IDC_ART_STORE_PACK_LIST, m_clbPack);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArtStoreDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CArtStoreDialog)
	ON_WM_DRAWITEM()
	ON_LBN_SELCHANGE(IDC_ART_STORE_PACK_LIST, OnSelchangeArtStorePackList)
	ON_LBN_DBLCLK(IDC_ART_STORE_PACK_LIST, OnDblclkArtStorePackList)
	ON_BN_CLICKED(IDC_ART_STORE_ORDER, OnArtStoreOrder)
	ON_BN_CLICKED(IDC_ART_STORE_BROWSER_PREVIEW, OnArtStorePreview)
	ON_BN_CLICKED(IDC_DOWNLOAD, OnArtStoreDownload)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_BN_CLICKED(IDC_UPDATE_ONLINE, OnUpdateOnline)
	//}}AFX_MSG_MAP
   ON_NOTIFY(TCN_SELCHANGE, IDC_ART_STORE_TABS, OnArtStoreTabChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArtStoreDialog message handlers

BOOL CArtStoreDialog::OnInitDialog() 
{
	// Get our interfaces. Maybe someday these will be passed in.
	m_pPathManager = GetGlobalPathManager();
	m_pPathManager->AddRef();

	m_pCollectionManager = GetGlobalCollectionManager();
	m_pCollectionManager->AddRef();

	m_pContentManager = GetGlobalContentManager();
	m_pContentManager->AddRef();

   BOOL        bSuccess;
   CRect       crClientArea, crToolbar(0, 0, 0, 0), crControl;
   int         nXMargin = 25, nYMargin = 25, nButtonHeight = 0;
   int         nNewWidth, nNewHeight;
   CWnd        *pcwndControl;

	INHERITED::OnInitDialog();

   if(!GetOrderForms())
   {
		// Inform the user that no content is installed
      CString  csMessage;
      csMessage.LoadString(IDS_ERR_LOADING_ORDER_FORMS);
      AfxMessageBox(csMessage);
      EndDialog(IDABORT);
      return TRUE;
   }

   m_bHasTabControl = FALSE;
	// Sort the tabs correctly.
	GetSortedArray();

   if(!InitImageList())
   {
      CString  csMessage, csResource, csResource2;
      csResource.LoadString(IDS_ERR_LOADING_ORDER_FORMS);
      csResource2.LoadString(IDS_RECOMMEND_REINSTALL);
      csMessage.Format("%s\n%s", (LPCSTR)csResource, (LPCSTR)csResource2);
      AfxMessageBox(csMessage);
      EndDialog(IDABORT);
      return TRUE;
   }
   // Get a Button Height (assume all buttons are same height)
   pcwndControl = (CWnd *)GetDlgItem(IDCANCEL);
   ASSERT(pcwndControl);
   if(pcwndControl)
   {
      pcwndControl->GetClientRect(&crControl);
      nButtonHeight = crControl.Height();
   }

   // Create Tab Control before repositioning other controls since we
   // need to account for size of it.
   GetClientRect(&crClientArea);

   // Create tab control
   m_bHasTabControl = TRUE;
   if(m_tabctrlProduct.Create(TCS_OWNERDRAWFIXED | TCS_TABS | WS_VISIBLE | WS_TABSTOP, crClientArea, this, IDC_ART_STORE_TABS))
   {
      TC_ITEM  tcItem;

      m_tabctrlProduct.SetImageList(&m_ilLogos);

      int   nArraySize = m_cpaOrderInfo.GetSize();
      COrderFormInfo *pofiOrderInfo;

      // Run through OrderInfo list and get product names for tabs
      for(int i=0;i < nArraySize;i++)
      {
         pofiOrderInfo = m_cpaOrderInfo.GetAt(i);
         if(pofiOrderInfo)
         {
            tcItem.mask = TCIF_IMAGE;
            //the image bitmap is built according to sorted array;
				tcItem.iImage = i; 
            bSuccess = m_tabctrlProduct.InsertItem(i, &tcItem);
         }
      }
      CSize csizeItem(m_cszTabImage.cx, m_cszTabImage.cy + TAB_HEIGHT_PADDING);
      m_tabctrlProduct.SetItemSize(csizeItem);
      m_tabctrlProduct.GetItemRect(0, &crToolbar);
   }
   m_tabctrlProduct.GetClientRect(&crClientArea);
   // Adjust client area to include toolbar (if any)
   crClientArea.top     += crToolbar.Height() + (nYMargin/2);
   crClientArea.bottom  -= (nYMargin/2);
   crClientArea.left    += (nXMargin/2);
   crClientArea.right   -= (nXMargin/2);

   CRect    crListBoxClientArea;
   m_clbPack.GetClientRect(&crListBoxClientArea);
   bSuccess = m_clbPack.SetWindowPos(
            NULL,          // CWnd *pWndInsertAfter
            crClientArea.left,               // New X position
            crClientArea.top,                // New Y position
            crListBoxClientArea.Width(),
            crClientArea.Height(),
            SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
   ASSERT(bSuccess);
   // Get available space for pack preview area and use it to 
   // center pack preview control
   CSize csizeFreeSpace(0,0);
   csizeFreeSpace.cx = crClientArea.right;
   m_clbPack.GetWindowRect(&crControl);
   ScreenToClient(&crControl);
   csizeFreeSpace.cx -= crControl.right;

   csizeFreeSpace.cy = crClientArea.Height() - nButtonHeight - nYMargin;
   // Reposition and size Pack Preview Window
   CWnd *pcwndPreview = (CWnd *)GetDlgItem(IDC_ART_STORE_PACK_PREVIEW);
   if(pcwndPreview)
   {
      nNewWidth   = 300+2;					// picture preview has a one pixel
      nNewHeight  = 265+2;					//    border all around
      csizeFreeSpace.cx -= nNewWidth;
      csizeFreeSpace.cy -= nNewHeight;

      ASSERT(csizeFreeSpace.cx > 0);
      if(csizeFreeSpace.cx < 0)
         csizeFreeSpace.cx = 0;
      //ASSERT(csizeFreeSpace.cy > 0);
      if(csizeFreeSpace.cy < 0)
         csizeFreeSpace.cy = 0;

      bSuccess = pcwndPreview->SetWindowPos(
         NULL,          // CWnd *pWndInsertAfter
         crControl.right + csizeFreeSpace.cx/2, // New X position
         crClientArea.top + csizeFreeSpace.cy/2,             // New Y position
         nNewWidth,
         nNewHeight,
         SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
      ASSERT(bSuccess);
   }
   // Reposition buttons at bottom
	PositionButton(IDC_UPDATE_ONLINE, crClientArea.bottom);
	PositionButton(IDC_ART_STORE_BROWSER_PREVIEW, crClientArea.bottom);
	PositionButton(IDC_ART_STORE_ORDER, crClientArea.bottom);
	PositionButton(IDCANCEL, crClientArea.bottom);

	// Initialize the picture preview
	m_cppPackPreview.Initialize(this, IDC_ART_STORE_PACK_PREVIEW, NULL);

   DisplayPacks();
	UpdatePackButtons();

	m_pContentManager->UpdateWhenLastSeenInternetContent();
	
	GET_PMWAPP()->ShowHelpfulHint("Art & More", this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CArtStoreDialog::PositionButton(UINT uID, int nNewButtonBottom)
{
	int nNewButtonTop = nNewButtonBottom;
   CWnd* pcwndControl = (CWnd *)GetDlgItem(uID);
   if(pcwndControl)
   {
		CRect crControl;
      pcwndControl->GetWindowRect(&crControl);
      ScreenToClient(&crControl);
      nNewButtonTop -= crControl.Height();
      pcwndControl->SetWindowPos(
         NULL,                // CWnd *pWndInsertAfter
         crControl.left,      // New X position
         nNewButtonTop,       // New Y position
         0,
         0,
         SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER);
   }
}

BOOL CArtStoreDialog::GetOrderForms()
{
   CString           csINIFileName;
   COrderFormInfo    *pofiOrderForm;
   ERRORCODE         errorcode = ERRORCODE_None;

	CStringArray csaORDFiles;
	FindORDFiles(csaORDFiles);

	// Order form filenames are located in the respective *.ord files
	for (int nOrdFile = 0; nOrdFile < csaORDFiles.GetSize(); nOrdFile++)
	{
		// Setup the INI file so we can get the section names.
		csINIFileName = m_pPathManager->LocatePath(csaORDFiles.GetAt(nOrdFile));

		if(!Util::FileExists(csINIFileName))
		{
			// Internal error.
			ASSERT(FALSE);
			continue;
		}

		// Setup the INI file so we can get the section names.
		CIniFile IniFile(csINIFileName);

		CString csSection = "Artstore";

		pofiOrderForm = new COrderFormInfo;
		ASSERT(pofiOrderForm);
		if(pofiOrderForm == NULL)
		{
			errorcode = ERRORCODE_Memory;
			continue;
		}

		pofiOrderForm->SetProductName(IniFile.GetString(csSection, "Product Name"));
		errorcode = pofiOrderForm->Read(csINIFileName);

		// If FileName is missing or doesn't exist or Product Name is missing, skip it
		if(errorcode != ERRORCODE_None ||
			pofiOrderForm->GetProductName().IsEmpty())
		{
			delete pofiOrderForm;
			continue;
		}

		pofiOrderForm->SetTabImageFileName(m_pPathManager->LocatePath(IniFile.GetString(csSection, "TabImageFileName")));

		pofiOrderForm->SetTabSortOrder(IniFile.GetInteger(csSection, "SortOrder", 0));
		// Are we an internet pack?
		pofiOrderForm->SetIsInternetContent(IniFile.GetInteger(csSection, "IsInternet", FALSE));

		CString csFilters = IniFile.GetString(csSection, "Filters");
		if(!GetConfiguration()->ExcludeFilters(csFilters))
			m_cpaOrderInfo.Add(pofiOrderForm);
		else
			delete pofiOrderForm;
	}

	// We want to always have an "Online" order form.
	// See if it exists.
	BOOL fHaveOnline = FALSE;
	for (int n = 0; n < m_cpaOrderInfo.GetSize(); n++)
	{
		pofiOrderForm = (COrderFormInfo*)m_cpaOrderInfo.GetAt(n);
		if (pofiOrderForm->GetIsInternetContent())
		{
			fHaveOnline = TRUE;
			break;
		}
	}

	// If it does not exist, create it.
	if (!fHaveOnline)
	{
		// Create an order info for the Online content.
		// It'll be empty, but that's OK. It still provides a place for
		// the user to go to "bootstrap" them into the online process.
		pofiOrderForm = new COrderFormInfo;

		CString csOnline;
		csOnline.LoadString(IDS_OnlineTabName);
		pofiOrderForm->SetProductName(csOnline);
		pofiOrderForm->SetTabImageFileName("Online.bmp");
		pofiOrderForm->SetTabSortOrder(100);
		// We are an internet pack.
		pofiOrderForm->SetIsInternetContent(TRUE);

		m_cpaOrderInfo.Add(pofiOrderForm);
	}

	// We succeeded if we found any order forms.
	return m_cpaOrderInfo.GetSize() > 0;
}

void CArtStoreDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (nIDCtl == IDC_ART_STORE_PACK_PREVIEW)
   {
		m_cppPackPreview.DrawPreview(lpDrawItemStruct);
   }
   else
   	INHERITED::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CArtStoreDialog::OnSelchangeArtStorePackList() 
{
   int               nArraySize = m_cpaOrderInfo.GetSize();
   COrderFormInfo    *pofiOrderInfo;

   ASSERT(m_nCurTab < nArraySize);
   if(m_nCurTab >= nArraySize)
      return;

   pofiOrderInfo = m_cpaOrderInfo.GetAt(m_nCurTab);

   const CUnlockPackInfo * pPackInfo;
   pPackInfo = pofiOrderInfo->GetPack(m_clbPack.GetItemData(m_clbPack.GetCurSel()));
   ASSERT(pPackInfo);
   if(pPackInfo)
   {
      m_cppPackPreview.NewGraphicPreview(m_pPathManager->LocatePath(pPackInfo->GetImageFileName()));
   }

   // Enable Buttons tied to packs
	UpdatePackButtons();
}

void CArtStoreDialog::OnArtStoreTabChange(NMHDR* pnmhdr, LRESULT* pResult) 
{
   m_nCurTab = m_tabctrlProduct.GetCurSel();
   // Update list of packs
   DisplayPacks();
	UpdatePackButtons();
}

void CArtStoreDialog::UpdatePackButtons(void)
{
	BOOL fEnablePreview = FALSE;
	BOOL fEnableDownload = TRUE;
	int nPreviewText = IDS_ArtStorePreview;
	BOOL fEnableOrder = FALSE;

	// Get the current parameters to see how to set up the buttons.
	const CUnlockPackInfo* pCurrentPack = GetCurrentPack();

	if (pCurrentPack != NULL)
	{
		// If we have a pack, then "Order" is enabled.
		fEnableOrder = TRUE;

		// Get the collection for this pack.
		CPMWCollection* pCollection = m_pCollectionManager->FindCollection(pCurrentPack->GetCollectionBaseName());
		if (pCollection == NULL)
		{
			// The collection is not currently installed.
			// Is this an online order form?
			COrderFormInfo* pCurrentOrderForm = GetCurrentOrderForm();
			if (pCurrentOrderForm != NULL && pCurrentOrderForm->GetIsInternetContent())
			{
				// An online pack. This turns into a download.
				//nPreviewText = IDS_ArtStoreDownload;
				m_btnDownload.ShowWindow(SW_SHOW);
				fEnableDownload = TRUE;
			}
			else
			{
				// Not an online pack and nothing to preview.
				// Disable the button.
				m_btnDownload.ShowWindow(SW_HIDE);
			}
		}
		else
		{
			// The collection is installed. We can preview it!
			COrderFormInfo* pCurrentOrderForm = GetCurrentOrderForm();
			if (pCurrentOrderForm != NULL && pCurrentOrderForm->GetIsInternetContent())
			{
				m_btnDownload.ShowWindow(SW_SHOW);
			}
			else
			{
				m_btnDownload.ShowWindow(SW_HIDE);
			}

			fEnablePreview = TRUE;
		}
	}
	else
	{
		fEnableDownload = FALSE;
	}

	// Get the string to put in the preview button.
	CString csPreviewText;
	csPreviewText.LoadString(nPreviewText);

	// Finally, update the buttons.
   m_btnOrder.EnableWindow(fEnableOrder);
   m_btnPreview.EnableWindow(fEnablePreview);
   m_btnDownload.EnableWindow(fEnableDownload);
	m_btnPreview.SetWindowText(csPreviewText);
}

ERRORCODE CArtStoreDialog::DisplayPacks()
   {
      int               nArraySize = m_cpaOrderInfo.GetSize();
      COrderFormInfo    *pofiOrderInfo;
      CString           csPackImage;

      ASSERT(m_nCurTab < nArraySize);
      if(m_nCurTab >= nArraySize)
         return ERRORCODE_Fail;

      pofiOrderInfo = m_cpaOrderInfo.GetAt(m_nCurTab);
      ASSERT(pofiOrderInfo);
      if(pofiOrderInfo == NULL)
         return ERRORCODE_Fail;

		UpdateOnlineControls(pofiOrderInfo->GetIsInternetContent());

      // Populate Pack list
      int nPackCount = pofiOrderInfo->GetPackCount();
      const CUnlockPackInfo * pPackInfo;

      m_clbPack.ResetContent();
      for(int i=0;i<nPackCount;i++)
      {
         pPackInfo = pofiOrderInfo->GetPack(i);
         ASSERT(pPackInfo);
         if(pPackInfo)
         {
				if (!GetConfiguration()->ExcludeFilters(pPackInfo->GetFilters()))
				{
					int nNewItemIndex = m_clbPack.AddString(pPackInfo->GetName());
					ASSERT(nNewItemIndex >= 0);
					if(nNewItemIndex >= 0)
						m_clbPack.SetItemData(nNewItemIndex, i);
				}
         }
      }
		if (nPackCount > 0)
		{
			m_clbPack.SetCurSel(0);
			OnSelchangeArtStorePackList();
		}
		else
		{
			// Get rid of the preview.
			m_cppPackPreview.NewGraphicPreview(NULL);

			// Display Pack Image in preview
			csPackImage = "oag.bmp";  //our default image for no packs
			Util::Trim(csPackImage);	// Remove Leading and Trailing Spaces
			ASSERT(!csPackImage.IsEmpty());
			// TODO-If image is missing, display "Image Not Available" 
			if(!csPackImage.IsEmpty())
				m_cppPackPreview.NewGraphicPreview(csPackImage);

		}


      return ERRORCODE_None;
   }

void CArtStoreDialog::UpdateOnlineControls(BOOL fIsOnline)
{
	// Size the pack list according to whether the pack is online or not.
	CWnd* pOnlineButton = GetDlgItem(IDC_UPDATE_ONLINE);
	CWnd* pPackList = GetDlgItem(IDC_ART_STORE_PACK_LIST);

	CWnd* pWndAlign;
	if (fIsOnline)
	{
		// This is an online pack.
		// Align the bottom of the pack list with the preview.
		pWndAlign = GetDlgItem(IDC_ART_STORE_PACK_PREVIEW);
	}
	else
	{
		// This is not an online pack.
		// Align the bottom of the pack list with the button bottom.
		pWndAlign = GetDlgItem(IDCANCEL);
	}

	// Handle the pack list resizing.
	if (pWndAlign != NULL && pPackList != NULL)
	{
		// Get the list rectangle.
		CRect crList;
		pPackList->GetWindowRect(crList);

		// Get the alignment control rectangle.
		CRect crAlign;
		pWndAlign->GetWindowRect(crAlign);

		// Resize the window.
		pPackList->SetWindowPos(NULL,
										0, 0,
										crList.Width(),
										crAlign.bottom - crList.top,
										SWP_NOMOVE | SWP_NOZORDER);
	}

	// Handle the online button.
	if (pOnlineButton)
	{
		pOnlineButton->EnableWindow(fIsOnline);
		pOnlineButton->ShowWindow(fIsOnline ? SW_SHOW : SW_HIDE);
	}
}

void CArtStoreDialog::OnDblclkArtStorePackList() 
{
   OnArtStorePreview();
}

void CArtStoreDialog::OnArtStoreOrder() 
   {
      COrderFormInfo    *pofiOrderInfo;
      int               nArraySize = m_cpaOrderInfo.GetSize();
      CString           csProductCode, csMessage;

      pofiOrderInfo = m_cpaOrderInfo.GetAt(m_nCurTab);
      ASSERT(pofiOrderInfo);
      if(pofiOrderInfo == NULL)
         return;

      const CUnlockPackInfo * pPackInfo = GetCurrentPack();
      if(pPackInfo == NULL)
         return;

      csProductCode = pPackInfo->GetProductCode();
      ASSERT(!csProductCode.IsEmpty());
      if(csProductCode.IsEmpty())
         {
            // Product Code missing
            csMessage.LoadString(IDS_RECOMMEND_REINSTALL);
            AfxMessageBox(csMessage);
            return;
         }

      // Construct the path to the order file.
      CString csOrderFile = pofiOrderInfo->GetFileName();
      ASSERT(Util::FileExists(csOrderFile));
      if(!Util::FileExists(csOrderFile))
         {
            CString  csSubMessage, csResource, csResource2;

            csResource.LoadString(IDS_ERR_ORDER_FILE_MISSING);
            csSubMessage.Format((LPCSTR)csResource, (LPCSTR)csOrderFile);
            csResource2.LoadString(IDS_RECOMMEND_REINSTALL);
            csMessage.Format("%s\n%s",
               (LPCSTR)csSubMessage,
               (LPCSTR)csResource2);
            AfxMessageBox(csMessage);
            return;
         }

		// See what kind of collection this is.

		if (pofiOrderInfo->GetIsInternetContent())
		{
			if (m_pContentManager != NULL)
			{
				CString str;
				int x = m_clbPack.GetCurSel();
				if (x >= 0)
					m_clbPack.GetText(x, str);
				m_pContentManager->LicenseProducts(pPackInfo->GetCollectionBaseName(), str);
			}
		}
		else
		{
			// Non-internet pack.
			CPrintMasterLicenseDll* m_pLicenseDll = NULL;
			if (m_pLicenseDll == NULL)
			{
				// Initialize the dll that allows content unlocking
				CString csLicenseDllPath;
				AfxGetApp()->BeginWaitCursor();
				TRY
				{
					// Create the DLL.
					m_pLicenseDll = new CPrintMasterLicenseDll;

					// Construct the path to the licensing DLLs.
					csLicenseDllPath = m_pPathManager->LocatePath("[[H]]\\MSREG");

					// Construct the fully-qualified filename of the LICENSE.DAT file.
					CString csLicenseDat = LicenseData.FileName();

					// Construct the path to the MSREG.INI file.
					CString csMsregIni = m_pPathManager->ExpandPath("MSREG.INI");

					// Get the filters to exclude.
					CString csFilters = GetConfiguration()->GetFilters();

					switch (m_pLicenseDll->Startup(csLicenseDllPath,
															csLicenseDat,
															csOrderFile,
															csMsregIni,
															csFilters))
					{
						case LICENSERESULT_Success:
						{
							break;
						}
						case LICENSERESULT_DLLNotFound:
						case LICENSERESULT_IntlDLLNotFound:
						{
							CString csMsg;
							LoadConfigurationString(IDS_LicenseDLLMissing, csMsg);
							AfxMessageBox(csMsg);
							break;
						}
						case LICENSERESULT_InvalidDLL:
						{
							AfxMessageBox(IDS_LicenseDLLInvalid);
							break;
						}
						case LICENSERESULT_AlreadyInUse:
						{
							// This means the DLL is already in use.
							AfxMessageBox(IDS_LicenseDLLInUse);
							break;
						}
						case LICENSERESULT_InvalidMsregIni:
						{
							// Configuration error. MSREG.INI is invalid. Fix it!
							ASSERT(FALSE);
							break;
						}
						default:
						{
							// Generic error.
							break;
						}
					}
				}
				END_TRY
				AfxGetApp()->EndWaitCursor();

				/* If we have the DLL but it's not valid, get rid of it. */
				if (m_pLicenseDll != NULL && !m_pLicenseDll->IsValid())
				{
					delete m_pLicenseDll;
					m_pLicenseDll = NULL;
				}
			}

			if (m_pLicenseDll != NULL)
			{
//				m_pCurrentLicenseCollection = pCollection;
				m_pLicenseDll->DoLicense(&csProductCode);
			}
			// Delete the licensing DLL.
			delete m_pLicenseDll;
		}
   }

const CUnlockPackInfo * CArtStoreDialog::GetCurrentPack()
{
   const CUnlockPackInfo * pPackInfo = NULL;
   COrderFormInfo *pofiOrderInfo = m_cpaOrderInfo.GetAt(m_nCurTab);

   if(pofiOrderInfo != NULL)
	{
		int nCurSel = m_clbPack.GetCurSel();
		if (nCurSel >= 0)
		{
			pPackInfo = pofiOrderInfo->GetPack(m_clbPack.GetItemData(nCurSel));
			ASSERT(pPackInfo);
		}
	}

   return pPackInfo;
}

COrderFormInfo* CArtStoreDialog::GetCurrentOrderForm()
{
	return m_cpaOrderInfo.GetAt(m_nCurTab);
}

void CArtStoreDialog::OnArtStorePreview() 
{
	// Get the current pack.
	const CUnlockPackInfo* pCurrentPack = GetCurrentPack();
	if (pCurrentPack == NULL)
	{
		return;
	}

	// See if the collection exists.
	CString csBaseName = pCurrentPack->GetCollectionBaseName();
	CPMWCollection* pCollection = m_pCollectionManager->FindCollection(csBaseName);
	if (pCollection != NULL)
	{
		// We have a collection to preview.
		m_pCollectionToPreview = pCollection;
		EndDialog(IDC_ART_STORE_BROWSER_PREVIEW);
	}
}

void CArtStoreDialog::OnArtStoreDownload() 
{
	// Get the current pack.
	const CUnlockPackInfo* pCurrentPack = GetCurrentPack();
	if (pCurrentPack == NULL)
	{
		return;
	}

	// See if the collection exists.
	CString csBaseName = pCurrentPack->GetCollectionBaseName();
	CPMWCollection* pCollection = m_pCollectionManager->FindCollection(csBaseName);
	//if (pCollection == NULL)
	//{
		BOOL fComplain = TRUE;
		// Get the current order form.
		COrderFormInfo* pOrderForm = GetCurrentOrderForm();
		if (pOrderForm != NULL)
		{
			// See if this is an internet order form.
			if (pOrderForm->GetIsInternetContent())
			{
				// This is an online order form.
				// Allow the user to download.
				CDownloadManager DownloadManager;
				DownloadManager.Run(m_pContentManager, m_pPathManager, csBaseName);
				fComplain = FALSE;

				// In case the status has changed.
				UpdatePackButtons();
			}
		}

		if (fComplain)
		{
#if 0
			CString csResource;
			csResource.LoadString(IDS_ERR_ART_STORE_MISSING_CONTENT);

			CString csResource2;
			csResource2.LoadString(IDS_RECOMMEND_REINSTALL);

			CString csMessage;
			csMessage.Format("%s\n%s",
								  (LPCSTR)csResource,
								  (LPCSTR)csResource2);
			AfxMessageBox(csMessage);
#endif
		}
	//}
}

void CArtStoreDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	GET_PMWAPP()->RemoveHelpfulHint();

	INHERITED::OnLButtonDown(nFlags, point);
}

void CArtStoreDialog::OnRButtonDown(UINT nFlags, CPoint point)
{
	GET_PMWAPP()->RemoveHelpfulHint();

	INHERITED::OnRButtonDown(nFlags, point);
}

BOOL CArtStoreDialog::InitImageList()
{
	// Create the bitmap to select into the image list.

	BOOL fSuccess = FALSE;

	CBitmap cbmImages;
	CSize czBitmap;

	if (BuildImageListBitmap(cbmImages, czBitmap))
	{
		// Set the size of a tab image.
		m_cszTabImage = czBitmap;

		// Create Image List with Logo Bitmaps
		if (m_ilLogos.Create(czBitmap.cx, czBitmap.cy, ILC_COLORDDB, 0, 10))
		{
			m_ilLogos.Add(&cbmImages, RGB(255,0,255));
			fSuccess = TRUE;
		}
	}
	return fSuccess;
}

BOOL CArtStoreDialog::BuildImageListBitmap(CBitmap& cbmImages, CSize& czBitmap)
{
	// Have we created the bitmap yet?
	BOOL fCreatedBitmap = FALSE;
	CBitmap* pOldBitmap = NULL;

	// We need a screen DC to create a compatible bitmap.
	CDC* pScreenDC = GetDC();

	// A DC to hold the destination bitmap.
	CDC DestinationDC;
	CDC SourceDC;

	// Create the DC first. This gives us the context for creating DDBs
	// with the correct palette.
	if (SourceDC.CreateCompatibleDC(NULL))
	{
		CPalette* pOldSourcePal = SourceDC.SelectPalette(pOurPal, TRUE);
		SourceDC.RealizePalette();

		if (DestinationDC.CreateCompatibleDC(NULL))
		{
			CPalette* pOldPal = DestinationDC.SelectPalette(pOurPal, TRUE);
			DestinationDC.RealizePalette();

			int nArraySize = m_cpaOrderInfo.GetSize();

			for (int i = 0; i < nArraySize; i++)
			{
				COrderFormInfo* pofiOrderInfo = m_cpaOrderInfo.GetAt(i);
				if(pofiOrderInfo == NULL)
				{
					ASSERT(FALSE);
					return FALSE;
				}

				// Get the name of the image file.
				CString csTabImageFile = pofiOrderInfo->GetTabImageFileName();		

				if (!m_pPathManager->FileExists(csTabImageFile))
				{
					continue;
				}

				// Create a DDB.
				CDDB DDB(csTabImageFile, pScreenDC->GetSafeHdc(), TRUE);

				// Attach the bitmap to a CBitmap for ease of use.
				CBitmap bmImage;
				bmImage.Attach(DDB.GetBitmap());
				if (bmImage.GetSafeHandle() != NULL)
				{
					BITMAP bm;
					bmImage.GetBitmap(&bm);

					if (!fCreatedBitmap)
					{
						int nWidth = bm.bmWidth * nArraySize;

						if (cbmImages.CreateCompatibleBitmap(pScreenDC, nWidth, bm.bmHeight))
						{
							// Select the new bitmap into the destination DC.
							pOldBitmap = DestinationDC.SelectObject(&cbmImages);

							// assume all the thumbnail bitmaps are the same size
							czBitmap = CSize(bm.bmWidth, bm.bmHeight);
							fCreatedBitmap = TRUE;

							// Clear the bitmap to white.
							DestinationDC.PatBlt(0, 0, nWidth, bm.bmHeight, WHITENESS);
						}
						else
						{
							break;
						}
					}

					// Select the source bitmap.

					CBitmap* pOldSourceBitmap = SourceDC.SelectObject(&bmImage);
					if (pOldSourceBitmap != NULL)
					{
						DestinationDC.StretchBlt(i * czBitmap.cx, 0, czBitmap.cx, czBitmap.cy, &SourceDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
						SourceDC.SelectObject(pOldSourceBitmap);
					}

					// Detach the bitmap.
					bmImage.Detach();
				}
			}

			// Select out the destination bitmap if we selected it in.
			if (pOldBitmap != NULL)
			{
				DestinationDC.SelectObject(pOldBitmap);
			}

			// Select back the destination palette if we selected it out.
			if (pOldPal != NULL)
			{
				DestinationDC.SelectPalette(pOldPal, FALSE);
			}
		}

		// Select back the source palette if we selected it out.
		if (pOldSourcePal != NULL)
		{
			SourceDC.SelectPalette(pOldSourcePal, FALSE);
		}
	}

	// Release the DC.
	ReleaseDC(pScreenDC);

	return fCreatedBitmap;
}
   
void CArtStoreDialog::GetSortedArray()
{

	int i;

	int nArraySize = m_cpaOrderInfo.GetSize();

	m_cpaSortedOrderInfo.RemoveAll();
	//m_cpaSortedOrderInfo.SetSize(nArraySize);	

	for(i = 0; i < nArraySize; i++)
	{

	  COrderFormInfo* pofiOrderInfo = m_cpaOrderInfo.GetAt(i);

	  ASSERT(pofiOrderInfo);

	  if(pofiOrderInfo == NULL)
		 return ;

	  m_cpaSortedOrderInfo.Add(pofiOrderInfo);	
	}	
	
	// Empty the source array.
	m_cpaOrderInfo.RemoveAll();
	//m_cpaOrderInfo.SetSize(nArraySize);

	int nCollections;

	// Now re-fill the source array in sorted order.
	while ((nCollections = m_cpaSortedOrderInfo.GetSize()) > 0)
	{
		COrderFormInfo* pNext = NULL;
		int nNext;

		// Compute the next collection to move over.
		for (int nCollection = 0; nCollection < nCollections; nCollection++)
		{
			COrderFormInfo* pCollection = m_cpaSortedOrderInfo.GetAt(nCollection);
			BOOL fSetNext = FALSE;
			if (pNext == NULL)
			{
				// Always set next if first one.
				fSetNext = TRUE;
			}
			else
			{
				// We need to do a comparison.
				int n = pCollection->GetTabSortOrder() - pNext->GetTabSortOrder();
				if (n == 0)
				{
					// Same index. Compare the names.
					// We set it if the new name is less than the current one.
					//fSetNext = pCollection->GetFriendlyName().CompareNoCase(pNext->GetFriendlyName()) < 0;
					fSetNext = TRUE;
				}
				else if (n > 0)
				{
					// Greater index. Use this one.
					fSetNext = TRUE;
				}
			}

			// If we need to set the next variables, do it now.
			if (fSetNext)
			{
				pNext = pCollection;
				nNext = nCollection;
			}
		}

		// Move the next one over.
		ASSERT(pNext != NULL);

		m_cpaOrderInfo.InsertAt(0, pNext);

		m_cpaSortedOrderInfo.RemoveAt(nNext);
	}
}

void CArtStoreDialog::FindORDFiles(LPCSTR pszDirectory, CStringArray& csaORDFiles)
{
	// Build a search path for ORD files in this directory.
	CString csSearchPath;
	Util::ConstructPath(csSearchPath, pszDirectory, "*.ord");

	// Iterate the ORD files.
	CFileIterator fit;
	for (BOOL fRet = fit.FindFirst(m_pPathManager->ExpandPath(csSearchPath), _A_NORMAL|_A_RDONLY);
		  fRet;
		  fRet = fit.FindNext())
	{
		// We have an ORD file.
		CString csORDFile;
		// Build the full path for it.
		Util::ConstructPath(csORDFile, pszDirectory, fit.Name());
		// Add it to the list.
		csaORDFiles.Add(csORDFile);
	}
}

void CArtStoreDialog::FindORDFiles(CStringArray& csaOrdFiles)
{
	// Find ORD files in the local content directory.
	FindORDFiles("[[S]]", csaOrdFiles);

	// Find ORD files in the shared areas.
   const CStringArray *pcsaSharedPaths = m_pCollectionManager->GetSharedPaths();
   if(pcsaSharedPaths)
	{
		for(int i=0; i < pcsaSharedPaths->GetSize(); i++)
		{
			FindORDFiles(pcsaSharedPaths->GetAt(i), csaOrdFiles);
		}
	}
}

//
// Get the latest content from online.
//

void CArtStoreDialog::OnUpdateOnline() 
{
	// We assume the current content is online.
	COrderFormInfo* pOrderForm = GetCurrentOrderForm();
	if (pOrderForm == NULL || !pOrderForm->GetIsInternetContent())
	{
		return;
	}

	// Update the ONLINE.ORD file.
	if (m_pContentManager->UpdateOnlineOrderForm())
	{
		// Order form has changed. Update it now.
		pOrderForm->Empty();
		CString csFileName = m_pPathManager->ExpandPath("[[S]]\\Online.ord");
		pOrderForm->Read(csFileName);
		DisplayPacks();

	}

}

/////////////////////////////////////////////////////////////////////////////
// CODTabCtrl

CODTabCtrl::CODTabCtrl()
{
}

CODTabCtrl::~CODTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CODTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CODTabCtrl)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CODTabCtrl message handlers

BOOL CODTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
   CRect       crFill;

   GetClientRect(&crFill);

	CBitmap  bmpBackground;
   BOOL     bLoaded=FALSE;
   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
      {
         CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());

         bLoaded = rlProductSpecific.LoadBitmap(bmpBackground, CSharedIDResourceManager::riProjectGallerySidebar, pOurPal);
      }
   else
      bLoaded = Util::LoadResourceBitmap(bmpBackground, MAKEINTRESOURCE(IDB_GALLERY_PROJ_SIDEBAR), pOurPal);
	if (!bLoaded)
   	return CTabCtrl::OnEraseBkgnd(pDC);

   // Tile the background.
	CBmpTiler Tiler;
	Tiler.TileBmpInRect(*pDC, crFill, bmpBackground);
   return TRUE;
}

void CODTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
   {
      CDC         dc;
      CRect       crFill, crBorder;
      BOOL        bSuccess;

      dc.Attach(lpDrawItemStruct->hDC);

      // Set rectangles equal to drawing area of control
      crFill = lpDrawItemStruct->rcItem;

	   CBitmap  bmpBackground;
      BOOL     bLoaded;
      if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
         {
            CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());

            bLoaded = rlProductSpecific.LoadBitmap(bmpBackground, CSharedIDResourceManager::riProjectGallerySidebar, pOurPal);
         }
      else
         bLoaded = Util::LoadResourceBitmap(bmpBackground, MAKEINTRESOURCE(IDB_GALLERY_PROJ_SIDEBAR), pOurPal);
	   if (bLoaded)
	   {
         // Tile the background.
		   CBmpTiler Tiler;
		   Tiler.TileBmpInRect(dc, crFill, bmpBackground);
	   }

      if(lpDrawItemStruct->itemAction == ODA_DRAWENTIRE)
      {
         CODImageList   *pImageList;
         CBitmap        *pcbmImage= NULL;
         int            nNumTabs = GetItemCount();
         int            nImageCount;   // Number of images in image list

         pImageList = GetImageList();
         ASSERT(pImageList);
         if(pImageList && (pcbmImage = pImageList->GetBitmap(0)) != NULL &&
            nNumTabs > 0 && (nImageCount = pImageList->GetImageCount()) > 0)
            {
      	      BITMAP         bmInfo;

               // Get Bitmap Dimensions so it can be centered on tab
	            if(pcbmImage->GetBitmap(&bmInfo))
	            {
      	         int      bmWidth, bmHeight;
                  CPoint   cptCenteredImage;

                  bmWidth = bmInfo.bmWidth / nImageCount;
		            bmHeight = bmInfo.bmHeight;

                  // If tab is not selected, adjust center of image by dropping down
                  if((lpDrawItemStruct->itemState & ODS_SELECTED) == 0)
                     crFill.top += 2;

                  cptCenteredImage.x = crFill.left + (crFill.Width() - bmWidth) / 2;
                  cptCenteredImage.y = crFill.top + (crFill.Height() - bmHeight) / 2;

                  TC_ITEM  tcItem;
                  tcItem.mask = TCIF_IMAGE;  // Just get image info
                  bSuccess = GetItem(lpDrawItemStruct->itemID, &tcItem);
                  ASSERT(bSuccess);
                  if(bSuccess)
                  {
                     // Have ImageList control draw it
                     bSuccess = pImageList->Draw(&dc, tcItem.iImage, cptCenteredImage, ILD_NORMAL);
                     ASSERT(bSuccess);
                  }
	            }
            }
         ASSERT(pcbmImage);   // If this assert fails, failed to get imagelist bitmap
      }

      // Done with dc
      dc.Detach();
   }

//////////////////////////////////////////////////////////////
// ODImageList Methods

CODImageList * CODTabCtrl::GetImageList()
   {
      return (CODImageList *) CTabCtrl::GetImageList();
   }

int CODImageList::Add(CBitmap *pbmImage, CBitmap *pbmMask)
   {
      HBITMAP  hNewBmpCopy;
      int      nNewImageIndex = CImageList::Add(pbmImage, pbmMask);

      if(nNewImageIndex < 0)
         return nNewImageIndex;

      hNewBmpCopy = DuplicateDDB(
         (HBITMAP)pbmImage->GetSafeHandle(),    // Image to copy
         (HPALETTE)pOurPal->GetSafeHandle());   // Palette to use
      ASSERT(hNewBmpCopy);
      if(hNewBmpCopy == NULL)
         return -1;

      m_cbmImage.Attach(hNewBmpCopy);
      return nNewImageIndex;
   }

int CODImageList::Add(CBitmap *pbmImage, COLORREF crMask)
   {
      HBITMAP  hNewBmpCopy;
      int      nNewImageIndex = CImageList::Add(pbmImage, crMask);

      if(nNewImageIndex < 0)
         return nNewImageIndex;

      hNewBmpCopy = DuplicateDDB(
         (HBITMAP)pbmImage->GetSafeHandle(),    // Image to copy
         (HPALETTE)pOurPal->GetSafeHandle());   // Palette to use
      ASSERT(hNewBmpCopy);
      if(hNewBmpCopy == NULL)
         return -1;

      m_cbmImage.Attach(hNewBmpCopy);
      return nNewImageIndex;
   }

BOOL  CODImageList::Draw(CDC *pDC, int nImageIndex, POINT pt, UINT nStyle)
   {
      BOOL           bImageDrawn = FALSE;
      int            nImageCount = GetImageCount();
      int            nImageWidth, nImageHeight;
      BITMAP         bmInfo;
      CBitmap        *pcbmImage= NULL;

      ASSERT(pDC);
      if(pDC == NULL)
         return FALSE;

      if(nImageCount <= 0)
         return FALSE;

      pcbmImage = GetBitmap(0);
      ASSERT(pcbmImage);
      if(pcbmImage == NULL)
         return FALSE;

	   if (pcbmImage->GetBitmap(&bmInfo) == 0)
         return FALSE;

      nImageWidth = bmInfo.bmWidth / nImageCount;
		nImageHeight = bmInfo.bmHeight;

      CDC      dcBitmap;
      if (dcBitmap.CreateCompatibleDC(pDC))
         {
            CBitmap* pOldBmp = dcBitmap.SelectObject(pcbmImage);

      		bImageDrawn = pDC->BitBlt(
               pt.x, 
               pt.y, 
               nImageWidth, 
               nImageHeight, 
               &dcBitmap,
		         nImageWidth * nImageIndex, 
               0, 
               SRCCOPY);

            // deselect the bitmap
            if (pOldBmp)
            	dcBitmap.SelectObject(pOldBmp);
         }
      return bImageDrawn;
   }

CBitmap * CODImageList::GetBitmap(int nImage)
   {
      return &m_cbmImage;
   }

HBITMAP CODImageList::DuplicateDDB ( HBITMAP hbmOriginal, HPALETTE hPalette )
{
    BITMAP  bm;
    HBITMAP   hbmCopy, hbmOld;
    HDC       hdcOriginal, hdcCopy;
    HDC       hDC = GetDC( NULL );
    HPALETTE  hPalOriginalOld, hPalCopyOld;

    /* create new bitmap with same dimensions as old one */
    GetObject( hbmOriginal, sizeof(BITMAP), &bm );
    hbmCopy = CreateCompatibleBitmap( hDC, bm.bmWidth, bm.bmHeight );
    if (!hbmCopy)
    {
        ReleaseDC( NULL, hDC );
        return( NULL );
    }

    /* create two DC's and select a bitmap in each */
    hdcOriginal = CreateCompatibleDC( hDC );
    hdcCopy     = CreateCompatibleDC( hDC );

    if(hPalette)
      {
        hPalOriginalOld = ::SelectPalette(hdcOriginal, hPalette, TRUE);
        hPalCopyOld     = ::SelectPalette(hdcCopy, hPalette, TRUE);
      }
    hbmOld = (HBITMAP) ::SelectObject( hdcOriginal, hbmOriginal );
    hbmOld = (HBITMAP) ::SelectObject( hdcCopy, hbmCopy );

    /* transfer original image to copy */
    BitBlt( hdcCopy, 0, 0,
            bm.bmWidth, bm.bmHeight,
            hdcOriginal, 0, 0,
            SRCCOPY );

    /* destroy all the DC's */
    ::SelectObject( hdcOriginal, hbmOld );
    ::SelectObject( hdcCopy, hbmOld );
    if(hPalette)
      {
        ::SelectPalette(hdcOriginal,  hPalOriginalOld, TRUE);
        ::SelectPalette(hdcCopy,      hPalCopyOld, TRUE);
      }
    DeleteDC( hdcOriginal );
    DeleteDC( hdcCopy );
    ReleaseDC( NULL, hDC );
    return( hbmCopy );
}
