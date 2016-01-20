//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CatOrgnz.cpp 1     3/03/99 6:03p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/CatOrgnz.cpp $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 9     2/20/99 8:56p Psasse
// Add PhotoProjects to Icon list
// 
// 8     11/12/98 5:25p Mwilson
// added craft project type
// 
// 7     7/26/98 7:21p Hforman
// changed baseclass to CPmwDialog to handle palette changes
// 
// 6     5/19/98 3:09p Hforman
// 
// 5     5/12/98 4:27p Hforman
// 
// 4     5/12/98 4:19p Hforman
// 
// 3     5/06/98 6:06p Hforman
// More functionality added -- more to come
// 
// 2     4/30/98 7:03p Hforman
// initial implementation - more to come
// 
// 1     4/16/98 10:12a Hforman
// 
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// NOTE: this implementation is currently tied to the parent being a
// CBrowserDialog. Functionality will have to be pulled out of	the
// browser in order for this to be a stand-alone dialog. (hforman)
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "project.h"
#include "pmwtempl.h"
#include "pmwcoll.h"
#include "compfn.h"
#include "UsrCtgry.h"
#include "Browser.h"
#include "BrowSupp.h"
#include "CnfrmRm.h"	// CConfirmFileRemove
#include "CatOrgnz.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCategoryOrganizerDlg dialog

// NOTE: These must be in the same order as PROJECT_TYPE enum!
UINT CCategoryOrganizerDlg::m_ProjIconIDs[] =
{
	IDR_POSTERTYPE,
	IDR_CARDTYPE,
	IDR_BANNERTYPE,
	IDR_CALENDARTYPE,
	IDR_LABELTYPE,
	IDR_ENVELOPETYPE,
	IDR_BIZCARDTYPE,
	IDR_CERTIFICATETYPE,
	IDR_NOTECARDTYPE,
	IDR_FAXCOVERTYPE,
	IDR_STATIONERYTYPE,
	IDR_NEWSLETTERTYPE,
	IDR_BROCHURETYPE,
	IDR_HALFCARDTYPE,
	IDR_WEBPUBTYPE,
	IDR_POSTCARDTYPE,
	IDR_STICKERTYPE,
	IDR_TSHIRTTYPE,
	IDR_PHOTOPROJECTSTYPE,
	IDR_CRAFTTYPE
};

CCategoryOrganizerDlg::CCategoryOrganizerDlg(CPMWCollection* pCollection, CBrowserDialog* pParent)
	: CPmwDialog(IDD, pParent)
{
	ASSERT(pCollection);
	m_pCollection = pCollection;
	m_pBrowser = pParent;

	m_pCategoryDB = NULL;
	m_pCategoryDir = NULL;
	m_pFileItems = NULL;

	m_pDragImage = NULL;
	m_fDragging = FALSE;
	m_hItemDrag = NULL;
	m_hItemDrop = NULL;

	m_fProjects = (m_pCollection->GetType() == CPMWCollection::typeProjects);
	m_fSomethingChanged = FALSE;

	//{{AFX_DATA_INIT(CCategoryOrganizerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCategoryOrganizerDlg::~CCategoryOrganizerDlg()
{
	if (m_pCollection)
	{
		if (m_pFileItems)
			m_pCollection->ReleaseItemInfoStream(m_pFileItems);
	}
}

void CCategoryOrganizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCategoryOrganizerDlg)
	DDX_Control(pDX, IDC_MOVE, m_btnMove);
	DDX_Control(pDX, IDC_CATEGORY_TREE, m_CategoryTree);
	//}}AFX_DATA_MAP
}

// Get filename and friendly name of an item
BOOL CCategoryOrganizerDlg::GetItemNames(DWORD dwItem, BOOL fFullPath, CString& strFilename, CString&strFriendlyName)
{
	ASSERT(m_pFileItems);

	BOOL fOK = FALSE;
	CItemsDataEntry itemInfo;
	if (m_pFileItems->Find(dwItem, &itemInfo) == ERRORCODE_None &&
		 m_pFileItems->Read() == ERRORCODE_None)
	{
		// Get the file name
		CString filename = itemInfo.GetFileName();
		CompositeFileName cfn(filename);

		if (fFullPath)
		{
			strFilename = cfn.get_file_name();
		}
		else // just return filename - no path
		{
			CString csBaseName;
			Util::SplitPath(cfn.get_file_name(), NULL, &strFilename);
		}

		strFriendlyName = itemInfo.GetFriendlyName();
		fOK = TRUE;
	}

	return fOK;
}

void CCategoryOrganizerDlg::GetCategories(CStringArray& catArray)
{
	HTREEITEM hItem = m_CategoryTree.GetRootItem();
	while (hItem != NULL)
	{
		CString cat = m_CategoryTree.GetItemText(hItem);
		catArray.Add(cat);
		hItem = m_CategoryTree.GetNextSiblingItem(hItem);
	}
}

BEGIN_MESSAGE_MAP(CCategoryOrganizerDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CCategoryOrganizerDlg)
	ON_BN_CLICKED(IDC_MOVE, OnMove)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_CATEGORY_TREE, OnEndLabelEdit)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_CATEGORY_TREE, OnBeginLabelEdit)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_CATEGORY_TREE, OnBeginDrag)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TVN_SELCHANGED, IDC_CATEGORY_TREE, OnSelchangedCategoryTree)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCategoryOrganizerDlg message handlers

BOOL CCategoryOrganizerDlg::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	if (m_pCollection == NULL)
	{
		ASSERT(0);
		return TRUE;
	}

	// Get the collection's category database
	m_pCategoryDB = m_pCollection->GetCategoryDB();
	if (m_pCategoryDB == NULL)
	{
		ASSERT(0);
		return TRUE;
	}

	// Get the category directory from the DB
	m_pCategoryDir = m_pCategoryDB->GetDir();
	if (m_pCategoryDir == NULL)
	{
		ASSERT(0);
		return TRUE;
	}

   // Open an Item info stream for getting file names & friendly names
	if (m_pCollection->NewItemInfoStream(&m_pFileItems) != ERRORCODE_None)
	{
		ASSERT(0);
		return TRUE;
	}

	// Create our image list (icons shown on treeview). The first icon is
	// for categories, then comes icons for items -- either projects or art.

	m_ImageList.Create(16, 16, TRUE, 0, 1);

	CBitmap bm;
	bm.LoadBitmap(IDB_CATEGORY_ICON);
	m_ImageList.Add(&bm, RGB(192, 192, 192));
	bm.DeleteObject();

	if (m_fProjects)
	{

		HICON hIcon;
		for (int i = 0; i < sizeof(m_ProjIconIDs)/sizeof(UINT); i++)
		{
			hIcon = (HICON)::LoadImage(AfxGetResourceHandle(),
												MAKEINTRESOURCE(m_ProjIconIDs[i]),
												IMAGE_ICON,
												16, 16,
												LR_DEFAULTCOLOR);

			m_ImageList.Add(hIcon);
		}
	}
	else
	{
		bm.LoadBitmap(IDB_ART_ICON);
		m_ImageList.Add(&bm, RGB(192, 192, 192));
		bm.DeleteObject();
	}

	m_CategoryTree.SetImageList(&m_ImageList, TVSIL_NORMAL);

	// fill out the category tree
	FillCategoryTree();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CCategoryOrganizerDlg::FillCategoryTree()
{
	TVINSERTSTRUCT tvis;
	TVITEM tvi;
	tvi.mask	= TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvis.hInsertAfter	= TVI_SORT;
	tvis.item = tvi;

	CString strProject;

	for (int i = 0; i < m_pCategoryDir->GetCategoryCount(); i++)
	{
		CString strCategory;

		CCategoryEntry* pEntry = m_pCategoryDir->GetCategoryEntry(i);
		ASSERT(pEntry);

		if (m_fProjects)
		{
			if (pEntry->GetLevel() == 0)
			{
				// get the project type, which gets prepended to children
				strProject = pEntry->GetName();
				continue;
			}
			ASSERT(!strProject.IsEmpty());
			strCategory = strProject + "|" + pEntry->GetName();
		}
		else // art collection is just one level deep
		{
			strCategory = pEntry->GetName();
		}

		if (strCategory.IsEmpty())
		{
			ASSERT(0);
			break;
		}

		// get the item data
		CCategoryData catData;
		m_pCategoryDB->FindFirst(strCategory, &catData);

		if (m_fProjects)
		{
			// we only want the category name now
			strCategory = pEntry->GetName();
		}

		// See if this category is already in the tree
		BOOL fFoundCategory = FALSE;
		HTREEITEM hCategoryItem = m_CategoryTree.GetRootItem();
		for (UINT node = 0; node < m_CategoryTree.GetCount(); node++)
		{
			if (m_CategoryTree.GetItemText(hCategoryItem) == strCategory)
			{
				fFoundCategory = TRUE;
				break;
			}
			hCategoryItem = m_CategoryTree.GetNextItem(hCategoryItem, TVGN_NEXT);
		}

		if (!fFoundCategory)
		{

			tvis.item.pszText = strCategory.GetBuffer(strCategory.GetLength());
			strCategory.ReleaseBuffer();

			tvis.item.iImage				= 0;
			tvis.item.iSelectedImage	= 0;
			tvis.hParent					= NULL;
			hCategoryItem = m_CategoryTree.InsertItem(&tvis);
		}

		for (DWORD dwItem = 0; dwItem < catData.GetItemCount(); dwItem++)
		{
			// get info on item
			CString strFileName, strFriendlyName;
			CCategoryDataItem* pItem = (CCategoryDataItem *)catData.m_carrayItems.GetAt(dwItem);
			if (pItem == NULL)
			{
				ASSERT(pItem);
				continue;
			}

			DWORD dwItemNum = pItem->GetItemNumber();
			if (!GetItemNames(dwItemNum, FALSE, strFileName, strFriendlyName))
				continue;

			if (m_fProjects)
			{
				// get the project type so we can display the proper icon
				CDocTemplate* pTemplate = GET_PMWAPP()->GetMatchingTemplate(strFileName, TRUE);
				int nType = ((CPmwDocTemplate*)pTemplate)->ProjectType();
				ASSERT(nType > 0 && nType < PROJECT_TYPE_Last);

				tvis.item.iImage				= nType;
				tvis.item.iSelectedImage	= nType;
			}
			else
			{
				tvis.item.iImage				= 1;
				tvis.item.iSelectedImage	= 1;
			}

			tvis.item.pszText = strFriendlyName.GetBuffer(strFriendlyName.GetLength());
			strFriendlyName.ReleaseBuffer();

			tvis.hParent			= hCategoryItem;
			// save the item's collection number
			tvis.item.mask			|= TVIF_PARAM;
			tvis.item.lParam		= dwItemNum;

			m_CategoryTree.InsertItem(&tvis);
		}

		// expand the category parent
		m_CategoryTree.Expand(hCategoryItem, TVE_EXPAND);
	}
}

BOOL CCategoryOrganizerDlg::PreTranslateMessage(MSG* pMsg) 
{
	// we need to send RETURN and ESCAPE characters to the
	// tree control's CEdit window
	if (pMsg->message == WM_KEYDOWN &&
		 pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
	{
		CEdit* edit = m_CategoryTree.GetEditControl();        
		if (edit)           
		{
			edit->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
			return TRUE;         
		}
	}
	
	return CPmwDialog::PreTranslateMessage(pMsg);
}

void CCategoryOrganizerDlg::OnMove() 
{
	// Allow user to move an item to a new category.

	HTREEITEM hSelItem = m_CategoryTree.GetSelectedItem();

	if (m_CategoryTree.GetParentItem(hSelItem) == NULL)
		return;	// can't move a category!
	
	// Get selected item name
	CString strItemName = m_CategoryTree.GetItemText(hSelItem);
	CString strItemCat = m_CategoryTree.GetItemText(m_CategoryTree.GetParentItem(hSelItem));

	// Get the list of categories for the user to select from
	CStringArray catlist;
	GetCategories(catlist);
	for (int i = 0; i < catlist.GetSize(); i++)
	{
		if (catlist[i] == strItemCat)
		{
			catlist.RemoveAt(i);
			break;
		}
	}

	// Show the move dialog
	CMoveToCatDlg dlg(catlist, strItemName);
	if (dlg.DoModal() == IDOK)
	{
		CString strNewCat = dlg.m_strCategory;
		HTREEITEM hNewCat = m_CategoryTree.GetRootItem();
		while (hNewCat != NULL)
		{
			CString cat = m_CategoryTree.GetItemText(hNewCat);
			if (cat == strNewCat)
				break;

			hNewCat = m_CategoryTree.GetNextSiblingItem(hNewCat);
		}

		MoveItemToNewParent(hSelItem, hNewCat);
	}

	m_CategoryTree.SetFocus();
}

void CCategoryOrganizerDlg::OnDelete() 
{
	HTREEITEM hSelItem = m_CategoryTree.GetSelectedItem();

	CString strFormat, strMessage;

	CString strError;
	strError.LoadString(IDS_ERROR_DELETE);
		
	if (m_CategoryTree.GetParentItem(hSelItem) == NULL)
	{
		// User wants to delete a category

		// See if there are any items in the category. If not just delete it.
		if (!m_CategoryTree.ItemHasChildren(hSelItem))
		{
			CString strCat = m_CategoryTree.GetItemText(hSelItem);
			strFormat.LoadString(IDS_DELETE_CATEGORY);
			strMessage.Format(strFormat, strCat);
			if (AfxMessageBox(strMessage, MB_YESNO) == IDYES)
				m_CategoryTree.DeleteItem(hSelItem);
		}
		else
		{
			CConfirmCategoryRemove dlg;
			if (dlg.DoModal() == IDYES)
			{
				BOOL fDeleteFiles = dlg.DeleteChecked();

				m_fSomethingChanged = TRUE;

				// Delete all the items in the category
				HTREEITEM hChildItem = m_CategoryTree.GetChildItem(hSelItem);
				while (hChildItem != NULL)
				{
					DWORD dwItem = m_CategoryTree.GetItemData(hChildItem);
					CString strFilename, strFriendlyName;
					if (!GetItemNames(dwItem, TRUE, strFilename, strFriendlyName))
					{
						AfxMessageBox(strError);
						return;
					}
					// delete the item
					if (m_pCollection->Delete(dwItem) != ERRORCODE_None)
					{
						// Warn user and see if they want to continue
						strFormat.LoadString(IDS_ERROR_DELETE_ITEM);
						strMessage.Format(strFormat, strFriendlyName);
						if (AfxMessageBox(strMessage, MB_YESNO) == IDNO)
							break; // stop deleting items
					}
					else
					{
						// We deleted the item from the collection. Delete file if requested.
						if (fDeleteFiles)
						{
							TRY
							{
								CFile::Remove(strFilename);
							}
							CATCH_ALL(e)
							{
								strFormat.LoadString(IDS_NO_DELETE_FILE);
								strMessage.Format(strFormat, strFilename);
								if (AfxMessageBox(strMessage, MB_YESNO) == IDNO)
									break;
							}
							END_CATCH_ALL
						}
					}
					
					hChildItem = m_CategoryTree.GetNextSiblingItem(hChildItem);
				} // end while

				// Delete the Category node
				m_CategoryTree.DeleteItem(hSelItem);
			}
		}
	}
	else
	{
		// deleting an item
		DWORD dwItem = m_CategoryTree.GetItemData(hSelItem);
		CString strFilename, strFriendlyName;
		if (!GetItemNames(dwItem, TRUE, strFilename, strFriendlyName))
		{
			AfxMessageBox(strError);
			return;
		}

		// show Confirm dialog
		CConfirmFileRemove dlgConfirm(m_fProjects, strFriendlyName, strFilename, FALSE);
		if (dlgConfirm.DoModal() == IDYES)
		{
			if (m_pCollection->Delete(dwItem) == ERRORCODE_None)
			{
				m_fSomethingChanged = TRUE;

				// We deleted the item. See if user wants to delete the file as well.
				if (dlgConfirm.DeleteChecked())
				{
					TRY
					{
						CFile::Remove(strFilename);
					}
					CATCH_ALL(e)
					{
						if (m_fProjects)
							strFormat.LoadString(IDS_PROJ_NO_DELETE);
						else
							strFormat.LoadString(IDS_PICT_NO_DELETE);

						strMessage.Format(strFormat, strFilename);
						AfxMessageBox(strMessage, MB_OK);
					}
					END_CATCH_ALL
				}

				// delete the item in the tree ctrl
				m_CategoryTree.DeleteItem(hSelItem);
			}
			else
			{
				AfxMessageBox(strError);
			}
		}
	}

	m_CategoryTree.SetFocus();
}

void CCategoryOrganizerDlg::OnSelchangedCategoryTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// Enable/Disable Move button --
	// Selection must be a leaf, and there must be more than one category.
	HTREEITEM hItem = m_CategoryTree.GetRootItem();
	BOOL fOnlyOneCat = (m_CategoryTree.GetNextSiblingItem(hItem) == NULL);

	hItem = pNMTreeView->itemNew.hItem;
	BOOL fCatSelected = (m_CategoryTree.GetParentItem(hItem) == NULL);

	m_btnMove.EnableWindow(!fCatSelected && !fOnlyOneCat);
	
	*pResult = 0;
}

void CCategoryOrganizerDlg::OnNew() 
{
	// Allow user to create a new category. Don't set the m_fSomethingChanged
	// flag yet, since this new category is just "virtual" until something is
	// moved into it.
	CStringArray catArray;
	GetCategories(catArray);
	CAddUserCategory dlg(catArray, this);
	if (dlg.DoModal() == IDOK)
	{
		m_CategoryTree.InsertItem(dlg.m_strCategory, TVI_ROOT, TVI_SORT);
	}

	m_CategoryTree.SetFocus();
}

void CCategoryOrganizerDlg::OnRename() 
{
	HTREEITEM hItem = m_CategoryTree.GetSelectedItem();
	m_CategoryTree.EditLabel(hItem);
}

void CCategoryOrganizerDlg::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	// restrict number of characters that can be entered
	CEdit* pEdit = m_CategoryTree.GetEditControl();
	if (pEdit)
	{
		if (m_CategoryTree.GetParentItem(pTVDispInfo->item.hItem) == NULL)
		{
			// editing a category
			pEdit->SetLimitText(MAX_CATEGORY_NAME_LEN);
		}
		else
		{
			// editing a project/picture name
			pEdit->SetLimitText(MAX_FRIENDLY_NAME_LEN);
		}
	}

	*pResult = 0;
}

void CCategoryOrganizerDlg::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	TVITEM* pItem = &pTVDispInfo->item;

	CString strNew = pItem->pszText;
	if (!strNew.IsEmpty())
	{
		CString strOld = m_CategoryTree.GetItemText(pItem->hItem);

		if (m_CategoryTree.GetParentItem(pItem->hItem) != NULL)
		{
			// a file's friendly name has been changed
			m_CategoryTree.SetItemText(pItem->hItem, strNew);
			DWORD dwItemNum = m_CategoryTree.GetItemData(pItem->hItem);
			m_pBrowser->ChangeFriendlyName(m_pCollection, dwItemNum, strNew);
		}
		else
		{
			// A category name has been changed.
			// Make sure name doesn't already exist.
			CStringArray catArray;
			GetCategories(catArray);
			if (!Util::IsStrInArray(catArray, strNew))
			{
				if (m_pCollection->RenameCategory(strOld, strNew) == ERRORCODE_None)
				{
					m_fSomethingChanged = TRUE;
					m_CategoryTree.SetItemText(pItem->hItem, strNew);
				}
			}
			else
			{
				CString str;
				str.LoadString(IDS_CATEGORY_EXISTS);
				AfxMessageBox(str);
			}
		}
	}
	
	*pResult = 0;
}

void CCategoryOrganizerDlg::OnOK()
{
	// don't want the dialog to go away when Return is pressed
}

void CCategoryOrganizerDlg::OnClose() 
{
	// we're out 'o here
	if (m_fSomethingChanged)
		EndDialog(IDOK);
	else
		EndDialog(IDCANCEL);
}

//
// Drag and Drop functions...
//
void CCategoryOrganizerDlg::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	m_hItemDrag = pNMTreeView->itemNew.hItem;
	if (m_hItemDrag && m_CategoryTree.GetParentItem(m_hItemDrag) != NULL)
	{
		ASSERT(!m_fDragging);
		m_fDragging = TRUE;
		m_hItemDrop = NULL;
		m_CategoryTree.SelectItem(m_hItemDrag);

		// get the image list for dragging
		m_pDragImage = m_CategoryTree.CreateDragImage(m_hItemDrag);
		ASSERT(m_pDragImage);
		m_pDragImage->DragShowNolock(FALSE);

		// figure out where to put the cursor within the DragImage
		CRect rect;
		m_CategoryTree.GetItemRect(m_hItemDrag, rect, TRUE);
		int xpos = pNMTreeView->ptDrag.x - rect.left + 16;
		int ypos = pNMTreeView->ptDrag.y - rect.top;
		
		m_pDragImage->BeginDrag(0, CPoint(xpos,ypos));
		m_pDragImage->DragMove(pNMTreeView->ptDrag);
		m_pDragImage->DragEnter(&m_CategoryTree, pNMTreeView->ptDrag);
		SetCapture();
	}
	
	*pResult = 0;
}

void CCategoryOrganizerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_fDragging)
	{
		if (m_pDragImage == NULL)
		{
			ASSERT(0);
			m_fDragging = FALSE;
			return;
		}

		MapWindowPoints(&m_CategoryTree, &point, 1);
		m_pDragImage->DragMove(point);

		// see if we need to scroll
		CRect rect;
		m_CategoryTree.GetClientRect(rect);
		if (point.y > rect.bottom - 10)
			TRACE("*** Scroll Down! ***\n");
		else if (point.y < rect.top + 10)
			TRACE("*** Scroll Up! ***\n");

		HTREEITEM	hitem;
		UINT			flags;
		if ((hitem = m_CategoryTree.HitTest(point, &flags)) != NULL)
		{
			m_pDragImage->DragLeave(&m_CategoryTree);
			m_CategoryTree.SelectDropTarget(hitem);
			m_hItemDrop = hitem;
			m_pDragImage->DragEnter(&m_CategoryTree, point);
		}
	}
	
	CPmwDialog::OnMouseMove(nFlags, point);
}

void CCategoryOrganizerDlg::OnTimer(UINT nIDEvent) 
{
	CPmwDialog::OnTimer(nIDEvent);

#ifdef FOR_LATER
	// This code came from www.codeguru.com

	if( nIDEvent != m_nTimerID )
	{
		CPmwDialog::OnTimer(nIDEvent);
		return;
	}

	// Doesn't matter that we didn't initialize m_timerticks
	m_timerticks++;

	POINT pt;
	GetCursorPos( &pt );
	RECT rect;
	m_CategoryTree.GetClientRect( &rect );
	m_CategoryTree.ClientToScreen( &rect );

	// NOTE: Screen coordinate is being used because the call
	// to DragEnter had used the Desktop window.
	m_pDragImage->DragMove(pt);

	HTREEITEM hitem = m_CategoryTree.GetFirstVisibleItem();

	if( pt.y < rect.top + 10 )
	{
		// We need to scroll up
		// Scroll slowly if cursor near the treeview control
		int slowscroll = 6 - (rect.top + 10 - pt.y) / 20;
		if( 0 == ( m_timerticks % (slowscroll > 0? slowscroll : 1) ) )
		{
			m_pDragImage->DragShowNolock(FALSE);
			m_CategoryTree.SendMessage(WM_VSCROLL, SB_LINEUP);
			m_CategoryTree.SelectDropTarget(hitem);
			m_hItemDrop = hitem;
			m_pDragImage->DragShowNolock(TRUE);
		}
	}
	else if( pt.y > rect.bottom - 10 )
	{
		// We need to scroll down
		// Scroll slowly if cursor near the treeview control
		int slowscroll = 6 - (pt.y - rect.bottom + 10 ) / 20;
		if ( 0 == ( m_timerticks % (slowscroll > 0? slowscroll : 1) ) )
		{
			m_pDragImage->DragShowNolock(FALSE);
			m_CategoryTree.SendMessage( WM_VSCROLL, SB_LINEDOWN);
			int nCount = m_CategoryTree.GetVisibleCount();
			for ( int i=0; i<nCount-1; ++i )
				hitem = m_CategoryTree.GetNextVisibleItem(hitem);
			if( hitem )
				m_CategoryTree.SelectDropTarget(hitem);
			m_hItemDrop = hitem;
			m_pDragImage->DragShowNolock(TRUE);
		}
	}
#endif // FOR_LATER
}

void CCategoryOrganizerDlg::MoveItemToNewParent(HTREEITEM hItemToMove, HTREEITEM hNewParent)
{
	// transfer an item to a new category

	TV_INSERTSTRUCT	tvstruct;
	TCHAR					sztBuffer[MAX_FRIENDLY_NAME_LEN+1];
	HTREEITEM			hNewItem;

	tvstruct.item.hItem = hItemToMove;
	tvstruct.item.cchTextMax = MAX_FRIENDLY_NAME_LEN;
	tvstruct.item.pszText = sztBuffer;
	tvstruct.item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	m_CategoryTree.GetItem(&tvstruct.item);  // get info of the moving element
	tvstruct.hParent = hNewParent;
	tvstruct.hInsertAfter = TVI_SORT;

	CString strErr;
	strErr.LoadString(IDS_ERROR_CATEGORY_MOVE_ITEM);

	// get item number and names
	DWORD dwItemNum = tvstruct.item.lParam;
	CString strFilename, strFriendlyName;
	if (!GetItemNames(dwItemNum, TRUE, strFilename, strFriendlyName))
	{
		AfxMessageBox(strErr);
		return;
	}

	// Let's call this the point of no return
	m_fSomethingChanged = TRUE;

	// get current category
	CString strOldCategory = m_pCategoryDB->GetCategory(dwItemNum, TRUE);

	// delete from old place
	if (m_pCategoryDB->Delete(dwItemNum) != ERRORCODE_None)
	{
		AfxMessageBox(strErr);
		return;
	}
	m_CategoryTree.DeleteItem(hItemToMove);

	// insert in new place
	CString strNewCategory = m_CategoryTree.GetItemText(hNewParent);

	// check if we have a project name to prepend to new category
	// Hey, what's with the different delimiters?!
	CMlsStringArray catstrs(strOldCategory, '/');
	if (catstrs.GetSize() == 2)
	{
		strNewCategory = catstrs[0] + CString('|') + strNewCategory;
	}

	// create an item to add to category DB
	CCategoryDataItem catDataItem;
	catDataItem.Empty();
	catDataItem.SetItemNumber(dwItemNum);

	CCategoryData catData;
	catData.Set(strNewCategory, CString(""), &catDataItem);
	if (m_pCategoryDB->Add(&catData, TRUE) != ERRORCODE_None)
	{
		AfxMessageBox(strErr);
		return;
	}
	hNewItem = m_CategoryTree.InsertItem(&tvstruct);
	m_CategoryTree.SelectItem(hNewItem);
}

void CCategoryOrganizerDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_fDragging)
	{
		// end dragging
		m_pDragImage->DragLeave(&m_CategoryTree);
		m_pDragImage->EndDrag();

		delete m_pDragImage;
		m_pDragImage = NULL;

		if (m_hItemDrop && m_CategoryTree.GetParentItem(m_hItemDrop) == NULL &&
			 m_CategoryTree.GetParentItem(m_hItemDrag) != m_hItemDrop)
		{
			MoveItemToNewParent(m_hItemDrag, m_hItemDrop);
		}
		else
		{
			MessageBeep(0);
		}

		// stop intercepting all mouse messages
		::ReleaseCapture();
		m_CategoryTree.SelectDropTarget(NULL);
		m_fDragging = FALSE;
	}
	
	CPmwDialog::OnLButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CMoveToCatDlg dialog

CMoveToCatDlg::CMoveToCatDlg(const CStringArray& categories, const CString& strItemName,
									  CWnd* pParent /*=NULL*/)
	: CPmwDialog(CMoveToCatDlg::IDD, pParent)
{
	m_aryCategories.Copy(categories);
	m_strItemName = strItemName;

	//{{AFX_DATA_INIT(CMoveToCatDlg)
	m_strMessage = _T("");
	m_strCategory = _T("");
	//}}AFX_DATA_INIT
}

void CMoveToCatDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoveToCatDlg)
	DDX_Control(pDX, IDC_CATEGORY_LIST, m_CategoryList);
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	DDX_LBString(pDX, IDC_CATEGORY_LIST, m_strCategory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoveToCatDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CMoveToCatDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoveToCatDlg message handlers

BOOL CMoveToCatDlg::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();

	// create user info string
	CString strMsgFormat;
	strMsgFormat.LoadString(IDS_MOVE_ITEM_TO_NEW_CATEGORY);
	m_strMessage.Format(strMsgFormat, m_strItemName);

	// fill list with categories
	for (int i = 0; i < m_aryCategories.GetSize(); i++)
		m_CategoryList.AddString(m_aryCategories[i]);
	m_CategoryList.SetCurSel(0);
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMoveToCatDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CPmwDialog::OnOK();
}
