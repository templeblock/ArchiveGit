/*
// $Workfile: TCOMBO.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
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
// $Log: /PM8/App/TCOMBO.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 10    11/05/98 4:37p Mwilson
// Fixed overlapping items
// 
// 9     10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 8     9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 7     9/01/98 5:45p Psasse
// correct window placement on "flow up"
// 
// 6     9/01/98 4:34p Dennis
// Removed obsolete code from bug fix
// 
// 5     9/01/98 4:21p Dennis
// Fixed Window Size for items when flowing up.
// 
// 4     7/23/98 10:07a Psasse
// New functionality for 640x480 screen to try and fit all items on screen
// 
// 3     5/22/98 5:26p Hforman
// fixed problem with showing "All" at top level
// 
// 2     5/20/98 6:16p Hforman
// added flag for only showing current level text instead of hierarchy
// 
//    Rev 1.0   14 Aug 1997 15:26:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:40   Fred
// Initial revision.
// 
//    Rev 1.14   23 Jun 1997 14:00:34   Fred
// Draws to offscreen bitmap to eliminate flicker
// 
//    Rev 1.13   11 Jun 1997 12:29:24   Jay
// Added const
// 
//    Rev 1.12   10 Jun 1997 13:36:18   Fred
// Added sorting capability
// 
//    Rev 1.11   04 Jun 1997 15:48:30   dennisp
// Added SetCurSel(CString), GetText(), SetDelimiter(), and GetDelimiter()
// 
//    Rev 1.10   21 May 1997 14:36:38   dennisp
// Added support to control sub-list color
// 
//    Rev 1.9   01 May 1997 15:41:28   Fred
// Calls ReleaseDC()
// 
//    Rev 1.8   01 May 1997 09:01:38   Fred
// Really fixed selection bug.
// 
//    Rev 1.7   28 Apr 1997 14:15:44   Fred
// Fixed selection bug
// 
//    Rev 1.6   28 Apr 1997 10:10:24   Fred
// Fixed some small bugs
// 
//    Rev 1.5   23 Apr 1997 16:16:52   Fred
//  
// 
//    Rev 1.4   23 Apr 1997 12:06:56   Fred
//  
// 
//    Rev 1.3   23 Apr 1997 11:38:20   Fred
//  
// 
//    Rev 1.2   22 Apr 1997 17:48:36   Jay
// Got rid of some extra stdafx.h includes.
// 
//    Rev 1.1   22 Apr 1997 13:17:48   Fred
// Start of project and art browsers
*/

#include "stdafx.h"

#include "util.h"
#include "sortarry.h"
#include "TCombo.h"

extern LPCSTR BrowserAllString(void);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Utility function for drawing a 3D border.
void Draw3DBorder(
 CDC& dc,
 const CRect crBorder,
 COLORREF clOutsideTopLeft,
 COLORREF clOutsideBottomRight,
 COLORREF clInsideTopLeft,
 COLORREF clInsideBottomRight)
{
   // Draw the 3D border:
   //
   //    A-----------AB
   //    |C---------CD|
   //    ||          ||
   //    ||          ||
   //    |C          ||
   //    AD----------D|
   //    B------------B

   CRect crSide;

   // A horizontal
   crSide.SetRect(crBorder.left, crBorder.top, crBorder.right-1, crBorder.top+1);
   Util::FillRectangleWithColor(dc, crSide, clOutsideTopLeft);

   // A vertical
   crSide.SetRect(crBorder.left, crBorder.top+1, crBorder.left+1, crBorder.bottom-1);
   Util::FillRectangleWithColor(dc, crSide, clOutsideTopLeft);

   // B horizontal
   crSide.SetRect(crBorder.left, crBorder.bottom-1, crBorder.right, crBorder.bottom);
   Util::FillRectangleWithColor(dc, crSide, clOutsideBottomRight);

   // B vertical
   crSide.SetRect(crBorder.right-1, crBorder.top, crBorder.right, crBorder.bottom-1);
   Util::FillRectangleWithColor(dc, crSide, clOutsideBottomRight);

   // C horizontal
   crSide.SetRect(crBorder.left+1, crBorder.top+1, crBorder.right-2, crBorder.top+2);
   Util::FillRectangleWithColor(dc, crSide, clInsideTopLeft);

   // C vertical
   crSide.SetRect(crBorder.left+1, crBorder.top+2, crBorder.left+2, crBorder.bottom-2);
   Util::FillRectangleWithColor(dc, crSide, clInsideTopLeft);

   // D horizontal
   crSide.SetRect(crBorder.left+1, crBorder.bottom-2, crBorder.right-1, crBorder.bottom-1);
   Util::FillRectangleWithColor(dc, crSide, clInsideBottomRight);

   // D vertical
   crSide.SetRect(crBorder.right-2, crBorder.top+1, crBorder.right-1, crBorder.bottom-2);
   Util::FillRectangleWithColor(dc, crSide, clInsideBottomRight);
}


/////////////////////////////////////////////////////////////////////////////
// CTreeComboItemArraySort class

CTreeComboItemArraySort::CTreeComboItemArraySort(void)
{
}

CTreeComboItemArraySort::~CTreeComboItemArraySort(void)
{
}

// return number of elements (must be static during sort)
DWORD CTreeComboItemArraySort::Size(void)
{
	CTreeComboItemArray* pArray = (CTreeComboItemArray*)this;
	return (DWORD)(pArray->GetSize());
}

// Compare two elements.
int CTreeComboItemArraySort::Compare(DWORD dwLeft, DWORD dwRight)
{
	CTreeComboItemArray* pArray = (CTreeComboItemArray*)this;
	ASSERT(pArray != NULL);

	CTreeComboItemArray::Item* pLeft = pArray->GetAt((int)dwLeft);
	CTreeComboItemArray::Item* pRight = pArray->GetAt((int)dwRight);
	ASSERT((pLeft != NULL) && (pRight != NULL));

	return pLeft->GetText().CompareNoCase(pRight->GetText());
}

// Compare data item to an element.
int CTreeComboItemArraySort::Compare(void* pData, DWORD dwIndex)
{
	ASSERT(pData != NULL);

	CTreeComboItemArray* pArray = (CTreeComboItemArray*)this;
	ASSERT(pArray != NULL);

	CTreeComboItemArray::Item* pItem = pArray->GetAt((int)dwIndex);
	ASSERT(pItem != NULL);

	return ((CTreeComboItemArray::Item*)pData)->GetText().CompareNoCase(pItem->GetText());
}

// Exchange two elements.
void CTreeComboItemArraySort::Exchange(DWORD dwLeft, DWORD dwRight)
{
	CTreeComboItemArray* pArray = (CTreeComboItemArray*)this;
	ASSERT(pArray != NULL);

	CTreeComboItemArray::Item* pLeft = pArray->GetAt((int)dwLeft);
	CTreeComboItemArray::Item* pRight = pArray->GetAt((int)dwRight);
	ASSERT((pLeft != NULL) && (pRight != NULL));

	pArray->SetAt((int)dwLeft, pRight);
	pArray->SetAt((int)dwLeft, pLeft);
}


/////////////////////////////////////////////////////////////////////////////
// CTreeComboItemArray class

// Constructor
CTreeComboItemArray::CTreeComboItemArray(CTreeComboItemArray* pPreviousItems /*=NULL*/)
{
   m_pPreviousItems = pPreviousItems;
   m_pList = NULL;
   m_nSelection = -1;
   m_nTopIndex = 0;
   m_crBounds.SetRectEmpty();
   m_nNumberOfColumns = 0;
   m_nItemsPerColumn = 0;
	m_fNeedsSorting = FALSE;
}

// Destructor
CTreeComboItemArray::~CTreeComboItemArray()
{
   // Delete the list box if it exists.
   if (m_pList != NULL)
   {
      m_pList->m_pItems = NULL;
      m_pList = NULL;
   }

   // Delete all the items.
   DeleteAll();
}

// Delete all the items in the array.
void CTreeComboItemArray::DeleteAll()
{
   int nLastIndex = GetSize();

   // Run through all the items and delete them.
   for (int nItem = 0; nItem < nLastIndex; nItem++)
   {
      // Get the current item.
      Item* pItem = GetAt(nItem);
      if (pItem != NULL)
      {
         // Delete the item.
         delete pItem;
         pItem = NULL;
         SetAt(nItem, NULL);
      }
   }

   // Remove all the array entries.
   RemoveAll();
}

// Delete the item at the given index.
void CTreeComboItemArray::DeleteAt(int nIndex, int nCount /*=1*/)
{
   // Get the index to delete up to.
   int nLastIndex = nIndex+nCount;
   if (nLastIndex > GetSize())
   {
      nLastIndex = GetSize();
   }

   // Delete the specified items.
   for (int nItem = nIndex; nItem < nLastIndex; nItem++)
   {
      // Get the current item.
      Item* pItem = GetAt(nItem);
      if (pItem != NULL)
      {
         // Delete the item.
         delete pItem;
         pItem = NULL;
         SetAt(nItem, NULL);
      }
   }

   // Remove the array entries for the specified items.
   RemoveAt(nIndex, nCount);
}

// Add an item to the item array. If the item array is sorted, then the item is
// added and the array is fully sorted. If the item array is not sorted, then the
// the item is added to the end of the item array. (Normal ComboBox behavior)
int CTreeComboItemArray::Add(Item* pItem, BOOL fSorted /*=FALSE*/)
{
	int nIndex = -1;

	if (!fSorted)
	{
		// Not sorted, just call Add() in base class.
		nIndex = CPtrArray::Add(pItem);
		m_fNeedsSorting = TRUE;
	}
	else
	{
		// If the items are out of sort order, then sort them.
		if (m_fNeedsSorting)
		{
			// Sort just the items in this list box, not the sub lists.
			QuickSort();
			m_fNeedsSorting = FALSE;
		}

		// Insert the item at the proper position for the current sort order.
		DWORD dwIndex;
		QuickFind(pItem, dwIndex);
		CPtrArray::InsertAt(dwIndex, pItem);

		// Return index of item (or duplicate.)
		nIndex = (int)dwIndex;
	}

	return nIndex;
}

// Insert an item into the item array at the given position. If the item array is
// sorted, then the item array may be unsorted after the insertion.
void CTreeComboItemArray::InsertAt(int nIndex, Item* pItem)
{
	// Insert the item.
	CPtrArray::InsertAt(nIndex, pItem);

	// Inserting a new item can cause the items to go out of sort order.
	m_fNeedsSorting = TRUE;
}

// Search for a string. The search is always case insensitive. If the fExact flag is
// set, then the entire string must match, otherwise the string will match as a prefix.
int CTreeComboItemArray::FindString(int nStartAfter, LPCSTR pszString, BOOL fExact /*=FALSE*/) const
{
   if (GetSize() > 0)
   {
      // Compute the starting index.
      int nStartIndex = nStartAfter+1;
      if ((nStartIndex < 0)
       || (nStartIndex >= GetSize()))
      {
         nStartIndex = 0;
      }

      // Compute the ending index.
      int nEndIndex = nStartIndex-1;
      if (nEndIndex < 0)
      {
         nEndIndex = GetSize()-1;
      }

      // Do the search.
      int nIndex = nStartIndex;
      for (;;)
      {
         Item* pItem = GetAt(nIndex);
         if (pItem != NULL)
         {
            // Compare the item.
            if (fExact)
            {
               if (stricmp(pszString, pItem->GetText()) == 0)
               {
                  return nIndex;
               }
            }
            else
            {
               int nLen1 = strlen(pszString);
               int nLen2 = pItem->GetText().GetLength();
               if ((nLen1 <= nLen2) && (strnicmp(pszString, pItem->GetText(), nLen1) == 0))
               {
                  return nIndex;
               }
            }
         }

         // Check if we searched the entire array.
         if (nIndex == nEndIndex)
         {
            break;
         }

         // Move to the next item.
         nIndex++;
         if (nIndex >= GetSize())
         {
            nIndex = 0;
         }
      };
   }

   return -1;
}

// Sort the items in the item array and any sub item lists.
void CTreeComboItemArray::Sort(void)
{
	// Sort ourselves.
	if (m_fNeedsSorting)
	{
		QuickSort();
		m_fNeedsSorting = FALSE;
	}

	// Sort any sublists.
   CTreeComboItemArray* pItems = NULL;
   if (m_nSelection >= 0)
   {
      CTreeComboItemArray::Item* pItem = GetAt(m_nSelection);
      if (pItem != NULL)
      {
         if (pItem->HasItems())
         {
            pItem->GetItems()->Sort();
         }
      }
   }
}

// Get the pointer to the item array for the currently selected item. If the array
// doesn't exist or no item is selected, NULL is returned.
CTreeComboItemArray* CTreeComboItemArray::GetSubItems(void) const
{
   CTreeComboItemArray* pItems = NULL;
   if (m_nSelection >= 0)
   {
      CTreeComboItemArray::Item* pItem = GetAt(m_nSelection);
      if (pItem != NULL)
      {
         if (pItem->HasItems())
         {
            pItems = pItem->GetItems();
         }
      }
   }
   return pItems;
}

// Return the index of the item in the previous item array which points to this array.
int CTreeComboItemArray::GetPreviousIndex(void) const
{
   if (m_pPreviousItems != NULL)
   {
      for (int nPreviousIndex = 0; nPreviousIndex < m_pPreviousItems->GetSize(); nPreviousIndex++)
      {
         const CTreeComboItemArray::Item* pPreviousItem = m_pPreviousItems->GetAt(nPreviousIndex);
         if (pPreviousItem != NULL)
         {
            if (pPreviousItem->GetItems() == this)
            {
               return nPreviousIndex;
            }
         }
      }
   }
   return -1;
}

// Return whether or not the item array is currently in the selection path.
BOOL CTreeComboItemArray::IsSelected(void) const
{
   if (m_pPreviousItems != NULL)
   {
      return (GetPreviousIndex() == m_pPreviousItems->m_nSelection)
          && (m_pPreviousItems->IsSelected());
   }
   return TRUE;
}

// Compute rectangle of a given column.
void CTreeComboItemArray::GetColumnRect(int nColumn, CRect& crColumn)
{
   ASSERT((nColumn >= 0) && (nColumn < m_nNumberOfColumns));

   crColumn.SetRectEmpty();

   // Set top and bottom.
   crColumn.top = m_crBounds.top+CTreeComboBox::m_Border.cy;
   crColumn.bottom = m_crBounds.bottom-CTreeComboBox::m_Border.cy;

   // Set left.
   crColumn.left = m_crBounds.left+CTreeComboBox::m_Border.cx;
   for (int nIndex = 0; nIndex < nColumn; nIndex++)
   {
      crColumn.left += (int)m_dwaColumnWidth[nIndex]+CTreeComboBox::m_Border.cx;
   }

   // Set right.
   crColumn.right = crColumn.left+(int)m_dwaColumnWidth[nColumn];
}

// Get the number of the column containing a given item.
int CTreeComboItemArray::GetColumnFromItem(int nItem)
{
   int nResult = nItem/m_nItemsPerColumn;
   if (nResult < 0)
   {
      nResult = 0;
   }
   if (nResult >= m_nNumberOfColumns)
   {
      nResult = m_nNumberOfColumns-1;
   }
   return nResult;
}

// Get the item offset of a given item within its column.
int CTreeComboItemArray::GetItemInColumn(int nItem)
{
   int nResult = -1;
   if (nItem >= 0)
   {
      nResult = nItem % m_nItemsPerColumn;
      if (nResult >= GetSize())
      {
         nResult = GetSize()-1;
      }
   }
   return nResult;
}

// Get the number of the first item in the given column.
int CTreeComboItemArray::GetFirstItemInColumn(int nColumn)
{
   ASSERT((nColumn >= 0) && (nColumn < m_nNumberOfColumns));
   return nColumn*m_nItemsPerColumn;
}

// Get the number of the last item in the given column.
int CTreeComboItemArray::GetLastItemInColumn(int nColumn)
{
   ASSERT((nColumn >= 0) && (nColumn < m_nNumberOfColumns));

   int nResult = (nColumn+1)*m_nItemsPerColumn-1;
   if (nResult >= GetSize())
   {
      nResult = GetSize()-1;
   }
   return nResult;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeComboItemArray::Item class

// Constructor
CTreeComboItemArray::Item::Item(LPCSTR pszText, DWORD dwData /*=(DWORD)-1*/, CBitmap* pBitmap /*=NULL*/)
{
   TRY
   {
      m_pItems = NULL;        // Array is created on demand
      m_csText = pszText;
      m_dwData = dwData;
      m_pBitmap = pBitmap;
   }
   END_TRY
}

// Destructor
CTreeComboItemArray::Item::~Item()
{
   // Delete any sub items.
   delete m_pItems;
   m_pItems = NULL;
}

// Create the sub item array if it does not exist.
CTreeComboItemArray* CTreeComboItemArray::Item::CreateItems(CTreeComboItemArray* pPreviousItems)
{
   ASSERT(pPreviousItems != NULL);
   if (m_pItems == NULL)
   {
      TRY
      {
         m_pItems = new CTreeComboItemArray(pPreviousItems);
      }
      END_TRY
   }
   return m_pItems;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeComboBox class

// Class names
LPCSTR CTreeComboBox::m_pszComboBoxClassName = "MindscapeTreeComboBox";
LPCSTR CTreeComboBox::m_pszComboListClassName = "MindscapeTreeComboList";

// Border sizes
CSize CTreeComboBox::m_Border(1, 1);
CSize CTreeComboBox::m_Margin(3, 2);

// Constructor
CTreeComboBox::CTreeComboBox()
{
   m_pPalette = NULL;

   // Default colors.
   m_clWindow = m_clChildWindow = ::GetSysColor(COLOR_WINDOW);
   m_clWindowText = ::GetSysColor(COLOR_WINDOWTEXT);
   m_clHighlight = ::GetSysColor(COLOR_HIGHLIGHT);
   m_clHighlightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
#ifdef WIN32
   m_clBorderShadow = ::GetSysColor(COLOR_3DSHADOW);
   m_clBorderHighlight = ::GetSysColor(COLOR_3DHILIGHT);
   m_clBorderLight = ::GetSysColor(COLOR_3DLIGHT);
   m_clBorderDark = ::GetSysColor(COLOR_3DDKSHADOW);
   m_clButtonFace = ::GetSysColor(COLOR_3DFACE);
#else
   m_clBorderShadow = ::GetSysColor(COLOR_BTNSHADOW);
   m_clBorderHighlight = ::GetSysColor(COLOR_BTNHIGHLIGHT);
   m_clBorderLight = RGB(255,255,255);
   m_clBorderDark = RGB(0,0,0);
   m_clButtonFace = ::GetSysColor(COLOR_BTNFACE);
#endif
   m_clListBorder = ::GetSysColor(COLOR_WINDOWFRAME);

   m_crClient.SetRectEmpty();
   m_crBackground.SetRectEmpty();
   m_crButton.SetRectEmpty();
   m_crText.SetRectEmpty();

   m_fLastPointValid = FALSE;
   m_LastPoint.x = 0;
   m_LastPoint.y = 0;

   m_fFontValid = FALSE;

   m_fListShown = FALSE;

   m_fButtonBitmapValid = FALSE;
   m_nButtonBitmapWidth = 0;
   m_nButtonBitmapHeight = 0;
   m_fButtonPushed = FALSE;
   m_fButtonCapture = FALSE;

   m_fNormalArrowBitmapValid = FALSE;
   m_nNormalArrowBitmapWidth = 0;
   m_nNormalArrowBitmapHeight = 0;

   m_fSelectedArrowBitmapValid = FALSE;
   m_nSelectedArrowBitmapWidth = 0;
   m_nSelectedArrowBitmapHeight = 0;

   m_pActiveList = NULL;
   m_pShowItems = NULL;

   m_csCurrentSelectionText.Empty();
	m_fShowCurLevelOnly = FALSE;
   m_pCurrentSelectionItems = &m_Items;
   m_nCurrentSelectionIndex = -1;

   m_uShowTimeout = TICKS_SHOW;
   m_uHideTimeout = TICKS_HIDE;

   m_fShowTimerActive = FALSE;
   m_fHideTimerActive = FALSE;

   SetDelimiter('/');

	m_fSorted = FALSE;
}

// Destructor
CTreeComboBox::~CTreeComboBox()
{
}

BEGIN_MESSAGE_MAP(CTreeComboBox, CWnd)
   //{{AFX_MSG_MAP(CTreeComboBox)
   ON_WM_ERASEBKGND()
   ON_WM_PAINT()
   ON_WM_LBUTTONDOWN()
   ON_WM_SETFOCUS()
   ON_WM_KILLFOCUS()
   ON_WM_LBUTTONUP()
   ON_WM_MOUSEMOVE()
   ON_WM_CANCELMODE()
   ON_WM_TIMER()
   ON_WM_KEYDOWN()
   ON_WM_GETDLGCODE()
   ON_WM_SYSKEYDOWN()
   ON_WM_CHAR()
   //}}AFX_MSG_MAP
   ON_MESSAGE(WM_GETFONT, OnGetFont)
   ON_MESSAGE(WM_SETFONT, OnSetFont)
   ON_MESSAGE(WM_GETTEXT, OnGetText)
   ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

// Create the control window.
BOOL CTreeComboBox::Create(DWORD dwStyle, const RECT& Bounds, CWnd* pParent, UINT uID)
{
   BOOL fResult = FALSE;
   
   if (CWnd::Create(m_pszComboBoxClassName, "", dwStyle, Bounds, pParent, uID, NULL))
   {
      GetClientRect(m_crClient);
      m_crBackground = m_crClient;
      m_crBackground.InflateRect(-2, -2);
      m_crButton = m_crBackground;
      m_crButton.left = m_crBackground.right = m_crButton.right-m_crButton.Height()+1;
      m_crText = m_crBackground;
      m_crText.InflateRect(-2, -2);

		m_fSorted = (dwStyle & CBS_SORT) != 0;

      fResult = TRUE;
   }

   return fResult;
}

// Create the control window based on the size and position of another window.
// The other window is destroyed and the new control takes its place.
BOOL CTreeComboBox::Create(DWORD dwStyle, CWnd* pParent, UINT uID)
{
   BOOL fResult = FALSE;

   ASSERT(pParent != NULL);

   // Get the existing control.
   CWnd* pOldControl = pParent->GetDlgItem(uID);
   if (pOldControl != NULL)
   {
      // Get the size and position of the existing control.
      CRect crBounds;
      pOldControl->GetWindowRect(crBounds);
      pParent->ScreenToClient(crBounds);

		// Get the old style and pass some of the style bits on.
		// New style can override the old style bits by turning
		// them on in the new style.
		DWORD dwOldStyle = pOldControl->GetStyle();
		dwOldStyle &= CBS_SORT;
		dwStyle |= dwOldStyle;
		
      // Create the new control.
      if (Create(dwStyle, crBounds, pParent, uID))
      {
         // Move the new control into the proper place in the Z-order.
         if (SetWindowPos(pOldControl, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE))
         {
            // Copy the font of the existing control.
            CFont* pOldFont = pOldControl->GetFont();
            if (pOldFont != NULL)
            {
               SetFont(pOldFont);
            }

            // Get rid of the old control.
            if (pOldControl->DestroyWindow())
            {
               fResult = TRUE;
            }
         }
      }
   }

   ASSERT(fResult);
   if (!fResult)
   {
      // We failed, clean up.
      if (m_hWnd != NULL)
      {
         DestroyWindow();
      }
   }

   return fResult;
}

// Register the classes used for the tree combo control.
BOOL CTreeComboBox::RegisterClass(void)
{
   WNDCLASS wc;

   // Register the combo box class.
   wc.style = CS_PARENTDC|CS_HREDRAW|CS_VREDRAW;
   wc.lpfnWndProc = AfxWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = AfxGetInstanceHandle();
   wc.hIcon = NULL;
   wc.hCursor = ::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ARROW));
   wc.hbrBackground = NULL;
   wc.lpszMenuName = NULL;
   wc.lpszClassName = m_pszComboBoxClassName;

   if (::RegisterClass(&wc) == 0)
   {
      return FALSE;
   }

   // Register the combo list class.
   wc.style = CS_SAVEBITS;
   wc.lpfnWndProc = AfxWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = AfxGetInstanceHandle();
   wc.hIcon = NULL;
   wc.hCursor = ::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_ARROW));
   wc.hbrBackground = NULL;
   wc.lpszMenuName = NULL;
   wc.lpszClassName = m_pszComboListClassName;

   if (::RegisterClass(&wc) == 0)
   {
      return FALSE;
   }

   return TRUE;
}

// Unregister the classes used for the tree combo control.
BOOL CTreeComboBox::UnregisterClass(void)
{
   ::UnregisterClass(m_pszComboBoxClassName, AfxGetInstanceHandle());
   ::UnregisterClass(m_pszComboListClassName, AfxGetInstanceHandle());

   return TRUE;
}

// Delete the current font.
void CTreeComboBox::DeleteFont(void)
{
   m_fFontValid = FALSE;
   if (m_Font.m_hObject != NULL)
   {
      m_Font.DeleteObject();
   }
}

// Set the bitmap for the combo box button.
void CTreeComboBox::SetButtonBitmap(UINT uID)
{
   DeleteButtonBitmap();
   if (m_ButtonBitmap.LoadBitmap(uID))
   {
      BITMAP Bitmap;
      if (Util::GetBitmap(&m_ButtonBitmap, &Bitmap))
      {
         // Remember bitmap dimensions
         m_nButtonBitmapWidth = Bitmap.bmWidth;
         m_nButtonBitmapHeight = Bitmap.bmHeight;

         m_fButtonBitmapValid = (m_nButtonBitmapWidth > 0) && (m_nButtonBitmapHeight > 0);
      }
   }
}

// Free the bitmap for the combo box button.
void CTreeComboBox::DeleteButtonBitmap(void)
{
   m_fButtonBitmapValid = FALSE;

   if (m_ButtonBitmap.m_hObject != NULL)
   {
      m_ButtonBitmap.DeleteObject();
   }
}

void CTreeComboBox::DrawButton(CDC& dc)
{
   // Upack the state of the button.
   BOOL fSelected = IsButtonPushed();
   BOOL fFocused = FALSE;
   BOOL fDisabled = !IsWindowEnabled();
   BOOL fDefault = FALSE;

   // Create a bitmap which will be used to draw the button image.
   // When the bitmap is complete, it will be drawn onto the button.
   CRect crButton(0, 0, m_crButton.Width(), m_crButton.Height());
   CBitmap bmButton;
   bmButton.CreateCompatibleBitmap(&dc, crButton.Width(), crButton.Height());

   // Create a memory DC to hold the button image.
   CDC dcButton;
   dcButton.CreateCompatibleDC(&dc);

   // Select our palette into the DC.
   CPalette* pOldPalette;
   SelectPalette(dcButton, pOldPalette);

   // Select the image bitmap into the DC.
   dcButton.SelectObject(&bmButton);

   // Initialize the button bitmap to the button face color.
   Util::FillRectangleWithColor(dcButton, crButton, m_clButtonFace);

   // Compute the area available for content.
   CRect crContent(crButton);

   if (!crContent.IsRectEmpty() && m_fButtonBitmapValid)
   {
      // Compute the bitmap dimensions and its nominal position.
      CRect crBitmap;
      crBitmap.SetRectEmpty();

      crBitmap.SetRect(0, 0, m_nButtonBitmapWidth, m_nButtonBitmapHeight);
      crBitmap.OffsetRect((crContent.Width()-crBitmap.Width())/2, (crContent.Height()-crBitmap.Height())/2);

      // If the button is selected, offset the bitmap.
      if (fSelected)
      {
         crBitmap.OffsetRect(1, 1);
      }

      // Draw the bitmap onto the button image at the computed position.
      if (!crBitmap.IsRectEmpty())
      {
         // Draw the bitmap onto the button.
         Util::TransparentBltBitmap(dcButton, crBitmap, m_ButtonBitmap, NULL, m_pPalette, 0, 0);
      }
   }

   // If the button is disabled, indicate this by embossing the bitmap.
   if (fDisabled)
   {
      Util::Emboss(dcButton, crButton, m_clButtonFace, m_clBorderHighlight, m_clBorderShadow, m_clBorderHighlight);
   }

   // Draw the button in its current state.
   if (fSelected)
   {
      // Button is selected, draw a dark border.
      CRect crDraw(crButton);
      Util::DrawBorderWithColor(dcButton, crDraw, m_clBorderShadow, 1);
   }
   else
   {
      CRect crDraw(crButton);

      // If the button is focused or the default button, draw a
      // black border around it. The rest of the image moves in
      // to make room.
      if (fFocused || fDefault)
      {
         Util::DrawBorderWithColor(dcButton, crDraw, m_clBorderDark, 1);
         crDraw.InflateRect(-1, -1);
      }

      // Draw the raised 3D border:
      //
      //    L-----------LD
      //    |H---------HS|
      //    ||          ||
      //    ||          ||
      //    |H          ||
      //    LS----------S|
      //    D------------D

      Draw3DBorder(dcButton, crButton, m_clBorderLight, m_clBorderDark, m_clBorderHighlight, m_clBorderShadow);
   }

   // If the button is focused, draw the focus rectangle.
   if (fFocused)
   {
      CRect crDraw(crButton);
      crDraw.InflateRect(-4, -4);
      dcButton.DrawFocusRect(crDraw);
   }

   // Copy the bitmap onto the button.
   dc.BitBlt(m_crButton.left, m_crButton.top, crButton.Width(), crButton.Height(), &dcButton, 0, 0, SRCCOPY);

   // Restore the previous palette.
   DeselectPalette(dcButton, pOldPalette);

   // Clean up.
   dcButton.DeleteDC();
   bmButton.DeleteObject();
}

// Set the bitmap for the normal arrow.
void CTreeComboBox::SetNormalArrowBitmap(UINT uID)
{
   DeleteNormalArrowBitmap();
   if (m_NormalArrowBitmap.LoadBitmap(uID))
   {
      BITMAP Bitmap;
      if (Util::GetBitmap(&m_NormalArrowBitmap, &Bitmap))
      {
         // Remember bitmap dimensions
         m_nNormalArrowBitmapWidth = Bitmap.bmWidth;
         m_nNormalArrowBitmapHeight = Bitmap.bmHeight;

         m_fNormalArrowBitmapValid = (m_nNormalArrowBitmapWidth > 0) && (m_nNormalArrowBitmapHeight > 0);
      }
   }
}

// Free the bitmap for the normal arrow.
void CTreeComboBox::DeleteNormalArrowBitmap(void)
{
   m_fNormalArrowBitmapValid = FALSE;

   if (m_NormalArrowBitmap.m_hObject != NULL)
   {
      m_NormalArrowBitmap.DeleteObject();
   }
}

// Set the bitmap for the selected arrow.
void CTreeComboBox::SetSelectedArrowBitmap(UINT uID)
{
   DeleteSelectedArrowBitmap();
   if (m_SelectedArrowBitmap.LoadBitmap(uID))
   {
      BITMAP Bitmap;
      if (Util::GetBitmap(&m_SelectedArrowBitmap, &Bitmap))
      {
         // Remember bitmap dimensions
         m_nSelectedArrowBitmapWidth = Bitmap.bmWidth;
         m_nSelectedArrowBitmapHeight = Bitmap.bmHeight;

         m_fSelectedArrowBitmapValid = (m_nSelectedArrowBitmapWidth > 0) && (m_nSelectedArrowBitmapHeight > 0);
      }
   }
}

// Free the bitmap for the selected arrow.
void CTreeComboBox::DeleteSelectedArrowBitmap(void)
{
   m_fSelectedArrowBitmapValid = FALSE;

   if (m_SelectedArrowBitmap.m_hObject != NULL)
   {
      m_SelectedArrowBitmap.DeleteObject();
   }
}

// Show or hide the combo box list.
void CTreeComboBox::ShowList(BOOL fShow /*=TRUE*/)
{
   // Check if we are already in the proper state.
   if (fShow != m_fListShown)
   {
      // No, check if we are showing.
      if (fShow)
      {
         // Figure out where to show the items.
         Flow();

         // Set the capture so we will get all mouse and keyboard input.
         SetCapture();
      }

      // Show or hide the items
      ShowList(fShow, NULL, FALSE);

      // Remember the new state.
      m_fListShown = fShow;
   }
}

void CTreeComboBox::ShowList(BOOL fShow, CTreeComboItemArray* pItems, BOOL fDeselect)
{
   // Get the item list to use.
   pItems = GetItems(pItems);
   if (pItems != NULL)
   {
      if (fShow)
      {
         // Check if the items already have a list. If not, then we'll need to create one.
         if (pItems->m_pList == NULL)
         {
            // Create the list window.
            TRY
            {
               pItems->m_pList = new CTreeComboList(this, pItems);
            }
            END_TRY
            
            if (pItems->m_pList != NULL)
            {
               pItems->m_pList->Create(WS_CHILD|WS_CLIPSIBLINGS, pItems->m_crBounds, GetDesktopWindow(), (UINT)-1);
               pItems->m_pList->BringWindowToTop();
               pItems->m_pList->SetFont(GetFont());
               pItems->m_pList->ShowWindow(SW_SHOW);
            }
         }

         // Show any sub lists.
         CTreeComboItemArray* pSubItems = pItems->GetSubItems();
         if (pSubItems != NULL)
         {
            ShowList(fShow, pSubItems, fDeselect);
         }
      }
      else
      {
         // Hide any sub lists.
         CTreeComboItemArray* pSubItems = pItems->GetSubItems();
         if (pSubItems != NULL)
         {
            ShowList(fShow, pSubItems, fDeselect);
         }

         // Check if items have a list which is shown.
         if (pItems->m_pList != NULL)
         {
            // If we are hiding the active list, then clear the list pointer.
            if (m_pActiveList == pItems->m_pList)
            {
               m_pActiveList = NULL;
            }

            // Destroy the list window. This will clear the pointer in the item array.
            pItems->m_pList->DestroyWindow();
            delete pItems->m_pList;

            ASSERT(pItems->m_pList == NULL);
         }

         // If we are deselecting, clear the selection.
         if (fDeselect)
         {
            pItems->m_nSelection = -1;
         }
      }
   }
}

// Pop the combo box button in or out.
void CTreeComboBox::PushButton(BOOL fPush /*=TRUE*/)
{
   if (fPush != m_fButtonPushed)
   {
      m_fButtonPushed = fPush;
      InvalidateRect(m_crButton);
      UpdateWindow();
   }
}

// Select out palette into the DC.
void CTreeComboBox::SelectPalette(CDC& dc, CPalette*& pOldPalette)
{
   pOldPalette = NULL;
   if (m_pPalette != NULL)
   {
      pOldPalette = dc.SelectPalette(m_pPalette, FALSE);
      dc.RealizePalette();
   }
}

// Select the previous palette back into the DC.
void CTreeComboBox::DeselectPalette(CDC& dc, CPalette*& pOldPalette)
{
   if (pOldPalette != NULL)
   {
      dc.SelectPalette(pOldPalette, FALSE);
      pOldPalette = NULL;
   }
}

// Shut down the the combo box.
void CTreeComboBox::CancelMode(void)
{
   // Stop all timers.
   StopShowTimer();
   StopHideTimer();

   // Destroy any unused listboxes immediately.
   HideUnselectedItems();

   // If we have the capture, release it.
   if (GetCapture() == this)
   {
      ReleaseCapture();
      m_fButtonCapture = FALSE;
   }

   // Pop up the button.
   PushButton(FALSE);

   // Pull up the list.
   ShowList(FALSE);

   // Set the current recorded selections.
   RestoreCurrentSelections();

   // Redraw our new state.
   Invalidate();
}

// Search for any list boxes which are not part of
// the current select path. This is done by searching
// the children of the desktop (all list boxes are
// children of the desktop.) List boxes which are not
// part of the current select path are destroyed.
void CTreeComboBox::HideUnselectedItems(void)
{
   // Run through all the children of the desktop.
   CWnd* pDesktop = GetDesktopWindow();
   if (pDesktop != NULL)
   {
      CWnd* pWnd = pDesktop->GetWindow(GW_CHILD);
      while (pWnd != NULL)
      {
         // Get the next child in case we destroy the current child.
         CWnd* pNextWnd = pWnd->GetWindow(GW_HWNDNEXT);

         // Make sure the class is correct.
         if (pWnd->IsKindOf(RUNTIME_CLASS(CTreeComboList)))
         {
            CTreeComboList* pList = (CTreeComboList*)pWnd;

            // Make sure it belongs to our combo box.
            if (pList->m_pComboBox == this)
            {
               // Check if the items in this list box are part
               // of the selected path.
               CTreeComboItemArray* pItems = pList->m_pItems;
               ASSERT(pItems->m_pList == pList);
               if (pItems != NULL)
               {
                  if (!pItems->IsSelected())
                  {
                     // If we're getting rid of the active list, then clear the
                     // pointer so we won't reference it.
                     if (pList == m_pActiveList)
                     {
                        m_pActiveList = NULL;
                     }

                     // The list box is not part of the select path, destroy it.
                     pList->DestroyWindow();
                     delete pList;

                     pItems->m_nSelection = -1;

                     ASSERT(pItems->m_pList == NULL);
                  }
               }
            }
         }

         // Move to next child.
         pWnd = pNextWnd;
      }
   }
}

BOOL CTreeComboBox::SetSelection(int nSelect, CTreeComboItemArray* pItems, BOOL fImmediate /*=FALSE*/, BOOL fTopLevelSelection /*=TRUE*/)
{
   if (pItems != NULL)
   {
      // Set the proper selections for previous items which are
      // needed to allow us to be selected.
      SetSelection(pItems->GetPreviousIndex(), pItems->m_pPreviousItems, fImmediate, FALSE);

		// It is possible that the subitems of the item we are selecting are already
		// on the screen and have a selection. This can occur when the the list box
		// is initially dropped down or after nagivation with the keyboard. It's
		// then possible to cause a selection to occur with the mouse. Since the
		// mouse selection should be the last active selection, we need to turn off
		// any selection in the subitems.
      if (fTopLevelSelection && m_fListShown)
      {
			CTreeComboItemArray* pSubItems = pItems->GetSubItems();
			if (pSubItems != NULL)
			{
				if (pSubItems->m_nSelection != -1)
				{
					int nPrevious = pSubItems->m_nSelection;
					pSubItems->m_nSelection = -1;
					InvalidateItemRect(pSubItems, nPrevious);
				}
			}
		}

      // Check if we have selected a new item.
      int nPreviousSelect = pItems->m_nSelection;
      if (nSelect != nPreviousSelect)
      {
         // Set the new selection.
         pItems->m_nSelection = nSelect;

         // If the list is shown, we need to update the display
         // to show the new selections.
         if (m_fListShown)
         {
            // Redraw the selection indicators
            InvalidateItemRect(pItems, nPreviousSelect);
            InvalidateItemRect(pItems, nSelect);

            // If this is the top level call to SetSelection, then use
            // the show and hide timers to update the display.
            if (fTopLevelSelection)
            {
               // Hide the old items.
               if (fImmediate)
               {
                  OnHideTimer();
               }
               else
               {
                  if (!m_fHideTimerActive)
                  {
                     StartHideTimer();
                  }
               }

               // Show the new items.
               if ((m_pShowItems = pItems->GetSubItems()) == NULL)
               {
                  StopShowTimer();
               }
               else
               {
                  if (fImmediate)
                  {
                     OnShowTimer();
                  }
                  else
                  {
                     StartShowTimer();
                  }
               }
            }
         }

         return TRUE;
      }
   }
   return FALSE;
}

// Get a pointer to the item array that contains the final item in
// the select path. These are the same items that will be navigated
// with keyboard commands.
CTreeComboItemArray* CTreeComboBox::GetActiveItems(void) const
{
   CTreeComboItemArray* pItems = GetItems(NULL);
   if (pItems != NULL)
   {
      CTreeComboItemArray* pSubItems = pItems->GetSubItems();
      while ((pSubItems != NULL) && (pSubItems->m_nSelection != -1))
      {
         pItems = pSubItems;
         pSubItems = pItems->GetSubItems();
      }
   }
   return pItems;
}

// Get a const pointer to the selected items.
inline const CTreeComboItemArray* CTreeComboBox::GetConstItems(void* pList /*=NULL*/) const
{
   return (pList == NULL) ? &m_Items : (const CTreeComboItemArray*)pList;
}

// Get a non-const pointer to the select items.
inline CTreeComboItemArray* CTreeComboBox::GetItems(void* pList) const
{
   return (CTreeComboItemArray*)GetConstItems(pList);
}

// Get a const pointer to the selected item.
inline const CTreeComboItemArray::Item* CTreeComboBox::GetConstItem(int nIndex, void* pList /*=NULL*/) const
{
   const CTreeComboItemArray* pItems = GetConstItems(pList);
   if (pItems != NULL)
   {
      if ((nIndex >= 0) && (nIndex < pItems->GetSize()))
      {
         return pItems->GetAt(nIndex);
      }
   }
   return NULL;
}

// Get a non-const pointer to the selected item.
inline CTreeComboItemArray::Item* CTreeComboBox::GetItem(int nIndex, void* pList /*=NULL*/) const
{
   return (CTreeComboItemArray::Item*)GetConstItem(nIndex, pList);
}

// Return a pointer to the sub item list of a given item. Use this function
// to access or create deeper levels in the selection tree.
void* CTreeComboBox::GetItemList(int nIndex, void* pList /*=NULL*/) const
{
   CTreeComboItemArray* pItems = GetItems(pList);
   if (pItems != NULL)
   {
      if ((nIndex >= 0) && (nIndex < pItems->GetSize()))
      {
         CTreeComboItemArray::Item* pItem = pItems->GetAt(nIndex);
         if (pItem != NULL)
         {
            return pItem->CreateItems(pItems);
         }
      }
   }

   return NULL;
}

// Return the number of items.
int CTreeComboBox::GetCount(void* pList /*=NULL*/) const
{
   const CTreeComboItemArray* pItems = GetConstItems(pList);
   if (pItems != NULL)
   {
      return pItems->GetSize();
   }
   return CB_ERR;
}

// Get the index of the currently selected item.
int CTreeComboBox::GetCurSel(void* pList /*=NULL*/) const
{
   const CTreeComboItemArray* pItems = GetConstItems(pList);
   if (pItems != NULL)
   {
      return pItems->m_nSelection;
   }
   return CB_ERR;
}

// Set the currently selected item. Any necessary selection in previous
// lists are automatically made.
int CTreeComboBox::SetCurSel(int nSelect, void* pList /*=NULL*/, BOOL fSelect /*=TRUE*/)
{
   CTreeComboItemArray* pItems = GetItems(pList);
   if (pItems != NULL)
   {
		if ((nSelect >= -1) && (nSelect < pItems->GetSize()))
		{
			if (SetSelection(nSelect, pItems, TRUE))
			{
				if (fSelect)
				{
					CommitCurrentSelections(TRUE);
					Invalidate();
				}
			}

			// Return the new selection.
			return nSelect;
		}
   }

   return CB_ERR;
}

int CTreeComboBox::SetCurSel(const CString &csStringToFind)
{
   int               nCurIndex, nCurLevel, nListCount;
   CString           csSelection;
   CMlsStringArray   csaSearchStrings;

   csaSearchStrings.SetNames(csStringToFind, GetDelimiter());

   // Descend through levels of tree combo box.
   void* pItemList = NULL;
   nListCount = GetCount(pItemList);
   for (nCurIndex = nCurLevel = 0;
      nCurLevel < csaSearchStrings.GetSize() && nCurIndex < nListCount;)
   {
      if (GetLBText(nCurIndex, csSelection, pItemList) < 0)
         break;

      // if match, move to the next level
      if(csSelection.CompareNoCase(csaSearchStrings[nCurLevel]) == 0)
         {
            SetCurSel(nCurIndex, pItemList);
            if(nCurLevel+1 == csaSearchStrings.GetSize())
               return 0;   // Success

            pItemList = GetItemList(nCurIndex, pItemList);
            if(pItemList == NULL)
               break;
            nCurLevel++;
            nCurIndex = 0;
            nListCount = GetCount(pItemList);
         }
      else
         nCurIndex++;
   }

   return -1;
}

// Associate a data DWORD with a particular item.
int CTreeComboBox::SetItemData(int nIndex, DWORD dwData, void* pList /*=NULL*/)
{
   CTreeComboItemArray::Item* pItem = GetItem(nIndex, pList);
   if (pItem != NULL)
   {
      pItem->SetData(dwData);
      return nIndex;
   }
   return CB_ERR;
}

// Retrieve the DWORD data associated with a particular item.
DWORD CTreeComboBox::GetItemData(int nIndex, void* pList /*=NULL*/) const
{
   const CTreeComboItemArray::Item* pItem = GetConstItem(nIndex, pList);
   if (pItem != NULL)
   {
      return pItem->GetData();
   }
   return (DWORD)CB_ERR;
}

// Associate a bitmap with a particular item.
int CTreeComboBox::SetItemBitmap(int nIndex, CBitmap* pBitmap, void* pList /*=NULL*/)
{
   CTreeComboItemArray::Item* pItem = GetItem(nIndex, pList);
   if (pItem != NULL)
   {
      pItem->SetBitmap(pBitmap);
      return nIndex;
   }
   return CB_ERR;
}

// Retrieve the bitmap associated with a particular item.
CBitmap* CTreeComboBox::GetItemBitmap(int nIndex, void* pList /*=NULL*/) const
{
   const CTreeComboItemArray::Item* pItem = GetConstItem(nIndex, pList);
   if (pItem != NULL)
   {
      return pItem->GetBitmap();
   }
   return NULL;
}

// Return the index of the top item displayed.
int CTreeComboBox::GetTopIndex(void* pList /*=NULL*/) const
{
   const CTreeComboItemArray* pItems = GetConstItems(pList);
   if (pItems != NULL)
   {
      return pItems->m_nTopIndex;
   }
   return CB_ERR;
}

// Set the top item displayed.
int CTreeComboBox::SetTopIndex(int nIndex, void* pList /*=NULL*/)
{
   CTreeComboItemArray* pItems = GetItems(pList);
   if (pItems != NULL)
   {
      if ((nIndex >= 0) && (nIndex < pItems->GetSize()))
      {
         pItems->m_nTopIndex = nIndex;
         if (pItems->m_pList != NULL)
         {
            pItems->m_pList->Invalidate();
         }
         return nIndex;
      }
   }
   return CB_ERR;
}

// Get the text of a given item.
int CTreeComboBox::GetLBText(int nIndex, LPSTR pszText, void* pList /*=NULL*/) const
{
   const CTreeComboItemArray::Item* pItem = GetConstItem(nIndex, pList);
   if (pItem != NULL)
   {
      strcpy(pszText, pItem->GetText());
      return strlen(pszText);
   }
   return CB_ERR;
}

// Get the text of a given item into a CString.
int CTreeComboBox::GetLBText(int nIndex, CString& csText, void* pList /*=NULL*/) const
{
   const CTreeComboItemArray::Item* pItem = GetConstItem(nIndex, pList);
   if (pItem != NULL)
   {
      TRY
      {
         csText = pItem->GetText();
      }
      END_TRY
      return csText.GetLength();
   }
   return CB_ERR;
}

// Get the length of the text of a given item.
int CTreeComboBox::GetLBTextLen(int nIndex, void* pList /*=NULL*/) const
{
   const CTreeComboItemArray::Item* pItem = GetConstItem(nIndex, pList);
   if (pItem != NULL)
   {
      return pItem->GetText().GetLength();
   }
   return CB_ERR;
}

// Get Full text of an item including sub-items
int CTreeComboBox::GetText(int nIndex, CString &csText) const
{
   csText.Empty();

   // Descend through levels of tree combo box.
   void* pItemList = NULL;
   for (;;)
   {
      // Get selection at current level.
      int nSelection = GetCurSel(pItemList);
      if (nSelection < 0)
      {
         // No selection.
         break;
      }

      // Get the text of the selection and add it to the string array.
      CString csSelection;
      if (GetLBText(nSelection, csSelection, pItemList))
      {
         if(pItemList)
            csText += m_csDelimiter;
         csText += csSelection;
      }

      // Move to the next level.
      pItemList = GetItemList(nSelection, pItemList);
      if (pItemList == NULL)
      {
         break;
      }
   }
   if(!csText.IsEmpty())
      return csText.GetLength();
   else
      return CB_ERR;
}

char CTreeComboBox::SetDelimiter(char chNewDelimiter)
{
   m_chDelimiter = chNewDelimiter;
   // Initialize CString version of delimiter which is really 
   // what we separate items by
   m_csDelimiter = " ";
   m_csDelimiter += chNewDelimiter;
   m_csDelimiter += " ";
   return chNewDelimiter;
}

char CTreeComboBox::GetDelimiter() const
{
   return m_chDelimiter;
}

void CTreeComboBox::SetShowCurLevelOnly(BOOL fSet /*=TRUE*/)
{
	m_fShowCurLevelOnly = fSet;
}

// Show or hide the drop down list.
void CTreeComboBox::ShowDropDown(BOOL fShow /*=TRUE*/)
{
   ShowList(fShow);
}

// Add a new item at the end.
int CTreeComboBox::AddString(LPCSTR pszString, DWORD dwData /*=(DWORD)-1*/, CBitmap* pBitmap /*=NULL*/, void* pList /*=NULL*/)
{
   ShowDropDown(FALSE);

   CTreeComboItemArray* pItems = GetItems(pList);
   if (pItems != NULL)
   {
      CTreeComboItemArray::Item* pItem = NULL;
      TRY
      {
         pItem = new CTreeComboItemArray::Item(pszString, dwData, pBitmap);
         int nIndex = pItems->Add(pItem, m_fSorted);
			if (nIndex >= 0)
			{
	         pItem = NULL;
		      return nIndex;
			}
      }
      END_TRY

      delete pItem;
      pItem = NULL;
   }
   return CB_ERR;
}

// Delete an item.
int CTreeComboBox::DeleteString(int nIndex, void* pList /*=NULL*/)
{
   ShowDropDown(FALSE);

   CTreeComboItemArray* pItems = GetItems(pList);
   if (pItems != NULL)
   {
      if ((nIndex >= 0) && (nIndex < pItems->GetSize()))
      {
         pItems->DeleteAt(nIndex);
         return nIndex;
      }
   }
   return CB_ERR;
}

// Add an item at a particular index.
int CTreeComboBox::InsertString(int nIndex, LPCSTR pszString, DWORD dwData /*=(DWORD)-1*/, CBitmap* pBitmap /*=NULL*/, void* pList /*=NULL*/)
{
   ShowDropDown(FALSE);

   CTreeComboItemArray* pItems = GetItems(pList);
   if (pItems != NULL)
   {
      if ((nIndex >= 0) && (nIndex <= pItems->GetSize()))
      {
         CTreeComboItemArray::Item* pItem = NULL;
         TRY
         {
            pItem = new CTreeComboItemArray::Item(pszString, dwData, pBitmap);
            pItems->InsertAt(nIndex, pItem);
            pItem = NULL;
            return pItems->GetSize()-1;
         }
         END_TRY

         delete pItem;
         pItem = NULL;
      }
   }
   return CB_ERR;
}

// Delete all items.
void CTreeComboBox::ResetContent(void* pList /*=NULL*/)
{
   ShowDropDown(FALSE);

   CTreeComboItemArray* pItems = GetItems(pList);
   if (pItems != NULL)
   {
      pItems->DeleteAll();
   }
}

// Look for an item that matches a given prefix.
int CTreeComboBox::FindString(int nStartAfter, LPCSTR pszString, void* pList /*=NULL*/) const
{
   const CTreeComboItemArray* pItems = GetConstItems(pList);
   if (pItems != NULL)
   {
      int nIndex = pItems->FindString(nStartAfter, pszString, FALSE);
      if (nIndex != -1)
      {
         return nIndex;
      }
   }
   return CB_ERR;
}

// Look for an item that matches a given string.
int CTreeComboBox::FindStringExact(int nStartAfter, LPCSTR pszString, void* pList /*=NULL*/) const
{
   const CTreeComboItemArray* pItems = GetConstItems(pList);
   if (pItems != NULL)
   {
      int nIndex = pItems->FindString(nStartAfter, pszString, TRUE);
      if (nIndex != -1)
      {
         return nIndex;
      }
   }
   return CB_ERR;
}

// Select an item that matches a given prefix.
int CTreeComboBox::SelectString(int nStartAfter, LPCSTR pszString, void* pList /*=NULL*/, BOOL fSelect /*=TRUE*/)
{
   int nIndex = FindString(nStartAfter, pszString, pList);
   if (nIndex != CB_ERR)
   {
      return SetCurSel(nIndex, pList, fSelect);
   }
   return CB_ERR;
}

// Select an item that matches a given string.
int CTreeComboBox::SelectStringExact(int nStartAfter, LPCSTR pszString, void* pList /*=NULL*/, BOOL fSelect /*=TRUE*/)
{
   int nIndex = FindStringExact(nStartAfter, pszString, pList);
   if (nIndex != CB_ERR)
   {
      return SetCurSel(nIndex, pList, fSelect);
   }
   return CB_ERR;
}

// Sort everything in the combo box.
void CTreeComboBox::Sort(void)
{
   CTreeComboItemArray* pItems = GetItems();
   if (pItems != NULL)
   {
      pItems->Sort();
   }
}

// Erase the background of a list box. This method is called by the
// list box control when it receives a WM_ERASEBKGND message.
BOOL CTreeComboBox::ListEraseBackground(CTreeComboList* pList, CDC& dc)
{
	// Do all the drawing on the paint message.
   return TRUE;
}

// Paint a list box control. This method is called by the list
// box control when it receives a WM_PAINT message.
void CTreeComboBox::ListPaint(CTreeComboList* pList, CPaintDC& dc)
{
	// Get the items associated with the list box.
   CTreeComboItemArray* pItems = pList->m_pItems;
   if (pItems != NULL)
   {
		// All drawing is done to an offscreen bitmap. Create a DC to hold the bitmap.
		CDC dcBitmap;
		if (dcBitmap.CreateCompatibleDC(&dc))
		{
			// Only draw that which is needed. The paint rectangle tells us what to update.
			CRect crBitmap(&(dc.m_ps.rcPaint));
			if (!crBitmap.IsRectEmpty())
			{
				// Create the bitmap.
				CBitmap Bitmap;
				if (Bitmap.CreateCompatibleBitmap(&dc, crBitmap.Width(), crBitmap.Height()))
				{
					// Select the offscreen bitmap into the DC.
					CBitmap* pOldBitmap = dcBitmap.SelectObject(&Bitmap);
					if (pOldBitmap != NULL)
					{
						// Offset the window origin so we can use our normal coordinates.
						dcBitmap.SetWindowOrg(crBitmap.left, crBitmap.top);

						// Select our palette into the bitmap DC.
						CPalette* pOldBitmapPalette;
						SelectPalette(dcBitmap, pOldBitmapPalette);

						// Erase the background with the window color.
						Util::FillRectangleWithColor(dcBitmap, crBitmap, (pItems->m_pPreviousItems == NULL) ? m_clWindow : m_clChildWindow);

						// Get the rectangle for the full window. We'll need this for drawing the various borders.
						CRect crList;
						pList->GetClientRect(crList);

						// Draw the outside border.
						Util::DrawBorderWithColor(dcBitmap, crList, m_clListBorder);

						// Draw the column divider lines.
						CRect crDivider;
						int nX = crList.left;
						for (int nColumn = 1; nColumn < pItems->m_nNumberOfColumns; nColumn++)
						{
							nX += m_Border.cx+(int)pItems->m_dwaColumnWidth[nColumn-1];
							crDivider.SetRect(nX, crList.top+1, nX+1, crList.bottom-1);
							Util::FillRectangleWithColor(dcBitmap, crDivider, m_clListBorder);
						}

						// Draw and text which is needed.
						if (m_fFontValid)
						{
							// Set up the DC for drawing text.
							int nOldBkMode = dcBitmap.SetBkMode(TRANSPARENT);
							CFont* pOldFont = dcBitmap.SelectObject(&m_Font);

							// Draw each of the items.
							for (int nItem = pItems->m_nTopIndex; nItem < pItems->GetSize(); nItem++)
							{
								// Get the position for the item.
								CRect crItem;
								if (GetItemRect(pItems, nItem, crItem))
								{
									// Convert to cordinates withing list box.
									pList->ScreenToClient(crItem);

									// Make sure item is visible.
									if ((crItem.left < crBitmap.right)
									 && (crItem.right > crBitmap.left)
									 && (crItem.top < crBitmap.bottom)
									 && (crItem.bottom > crBitmap.top))
									{
										// Get the item itself.
										CTreeComboItemArray::Item* pItem = pItems->GetAt(nItem);
										if (pItem != NULL)
										{
											// Draw the item.
											ListPaintItem(pList, dcBitmap, pItem, crItem, nItem == pItems->m_nSelection);
										}
									}
								}
							}

							// Restore the bitmap DC.
							dcBitmap.SetBkMode(nOldBkMode);
							dcBitmap.SelectObject(pOldFont);
						}

						// Select the palette into the destination DC.
						CPalette* pOldPalette;
						SelectPalette(dc, pOldPalette);

						// Copy the offscreen bitmap onto the destination DC.
						dc.BitBlt(crBitmap.left, crBitmap.top, crBitmap.Width(), crBitmap.Height(), &dcBitmap, crBitmap.left, crBitmap.top, SRCCOPY);

						// Select the old palette back into the destination DC.
						DeselectPalette(dc, pOldPalette);

						// Select the old palette back into the bitmap DC.
						DeselectPalette(dcBitmap, pOldBitmapPalette);
					}

					// Select the old bitmap back into the bitmap DC.
					dcBitmap.SelectObject(pOldBitmap);
					pOldBitmap = NULL;
				}
			}
		}
	}
}

// Paint an item in a list box control. This is a helper method for ListPaint().
void CTreeComboBox::ListPaintItem(CTreeComboList* pList, CDC& dc, CTreeComboItemArray::Item* pItem, const CRect& crItem, BOOL fSelected)
{
   if (!crItem.IsRectEmpty())
   {
      if (fSelected)
      {
         Util::FillRectangleWithColor(dc, crItem, m_clHighlight);
      }

      CRect crText(crItem);

      BOOL fArrowBitmapValid = fSelected ? m_fNormalArrowBitmapValid : m_fSelectedArrowBitmapValid;

      if (fArrowBitmapValid && pItem->HasItems())
      {
         CBitmap& ArrowBitmap = fSelected ? m_SelectedArrowBitmap : m_NormalArrowBitmap;
         int nArrowBitmapWidth = fSelected ? m_nSelectedArrowBitmapWidth : m_nNormalArrowBitmapWidth;
         int nArrowBitmapHeight = fSelected ? m_nSelectedArrowBitmapHeight : m_nNormalArrowBitmapHeight;

         CRect crArrow;
         crArrow.left = crItem.right-nArrowBitmapWidth;
         crArrow.top = crItem.top+(crItem.Height()-nArrowBitmapHeight)/2;
         crArrow.right = crArrow.left+nArrowBitmapWidth;
         crArrow.bottom = crArrow.top+nArrowBitmapHeight;
         Util::TransparentBltBitmap(dc, crArrow, ArrowBitmap, NULL, m_pPalette);
         crText.right = crArrow.left;
      }

      crText.InflateRect(-2, -1);
      if (!crText.IsRectEmpty())
      {
         COLORREF clOldTextColor = dc.SetTextColor(fSelected ? m_clHighlightText : m_clWindowText);
#ifdef WIN32
         dc.DrawText(pItem->GetText(), crText, DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_TOP);
#else
         dc.DrawText(pItem->GetText(), pItem->GetText().GetLength(), crText, DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_TOP);
#endif
         dc.SetTextColor(clOldTextColor);
      }
   }
}

// Compute the display rectangle of the given item.
BOOL CTreeComboBox::GetItemRect(CTreeComboItemArray* pItems, int nItem, CRect& crItem)
{
   BOOL fResult = FALSE;

   crItem.SetRectEmpty();
   
   // Validate input parameters.
   if ((pItems != NULL)
    && (!pItems->m_crBounds.IsRectEmpty())
    && (pItems->m_nNumberOfColumns > 0)
    && (pItems->m_nItemsPerColumn > 0)
    && (nItem >= 0)
    && (nItem < pItems->GetSize()))
   {
      // Compute the column we are in.
      int nColumn = pItems->GetColumnFromItem(nItem);
      ASSERT((nColumn >= 0) && (nColumn <= pItems->m_nNumberOfColumns-1));

      // Get the rectangle for the column.
      CRect crColumn;
      pItems->GetColumnRect(nColumn, crColumn);

      // Build the rectangle for the column.
      crItem.left = crColumn.left;
      crItem.right = crColumn.right;
      crItem.top = crColumn.top+m_nItemHeight*pItems->GetItemInColumn(nItem);
      crItem.bottom = crItem.top+m_nItemHeight;

      fResult = TRUE;
   }

   return fResult;
}

// Invalidate the image of the specified item.
void CTreeComboBox::InvalidateItemRect(CTreeComboItemArray* pItems, int nItem)
{
   if ((pItems != NULL) && (pItems->m_pList != NULL))
   {
      CRect crItem;
      if (GetItemRect(pItems, nItem, crItem))
      {
         pItems->m_pList->ScreenToClient(crItem);
         pItems->m_pList->InvalidateRect(crItem);
      }
   }
}

// Get the number of the item at a give point in a list box,
// or -1 if there is no such item. Note: the point is
// in combo box client coordinates.
int CTreeComboBox::GetItemFromPoint(CTreeComboItemArray* pItems, CPoint& cpPoint)
{
   int nResult = -1;

   // Convert the point to screen coordinates.
   CPoint cpScreenPoint(cpPoint);
   ClientToScreen(&cpScreenPoint);

   // Make sure the specified items make sense.
   if ((pItems != NULL)
    && (pItems->GetSize() > 0)
    && (pItems->m_crBounds.PtInRect(cpScreenPoint))
    && (pItems->m_nNumberOfColumns > 0)
    && (pItems->m_nItemsPerColumn > 0))
   {
      // Search for the proper column.
      int nOffset = pItems->m_crBounds.left+m_Border.cx;
      for (int nColumn = 0; nColumn < pItems->m_nNumberOfColumns; nColumn++)
      {
         nOffset += (int)pItems->m_dwaColumnWidth[nColumn]+m_Border.cx;
         if (cpScreenPoint.x < nOffset)
         {
            // We have the column, compute the row.
            int nPixelsFromTop = cpScreenPoint.y-(pItems->m_crBounds.top+m_Border.cy);
            if (nPixelsFromTop < 0)
            {
               nPixelsFromTop = 0;
            }
            nResult = (nColumn*pItems->m_nItemsPerColumn)+(nPixelsFromTop/m_nItemHeight);
            if (nResult >= pItems->GetSize())
            {
               nResult = pItems->GetSize()-1;
            }
            break;
         }
      }
   }

   return nResult;
}

// Get a pointer to the list box window at the given point,
// or NULL if there is no such window. Note: the point is
// in combo box client coordinates.
CTreeComboList* CTreeComboBox::GetListFromPoint(CPoint& cpPoint)
{
   // Convert the point to screen coordinates.
   CPoint cpScreenPoint(cpPoint);
   ClientToScreen(&cpScreenPoint);

   // Get the window at the specified point.
   CWnd* pWnd = CWnd::WindowFromPoint(cpScreenPoint);
   if (pWnd != NULL)
   {
      // Make sure it's a list box.
      if (pWnd->IsKindOf(RUNTIME_CLASS(CTreeComboList)))
      {
         // Make sure it belongs to us.
         if (((CTreeComboList*)pWnd)->m_pComboBox == this)
         {
            // Got a list box.
            return (CTreeComboList*)pWnd;
         }
      }
   }

   // Could not find a list box at the specified point.
   return NULL;
}

void CTreeComboBox::Flow(CTreeComboItemArray* pItems /*=NULL*/)
{
   if (m_fFontValid)
   {
      CDC* pDC = GetDC();
      if (pDC != NULL)
      {
         CFont* pOldFont = pDC->SelectObject(&m_Font);
         if (pOldFont != NULL)
         {
            CRect crScreen(0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));
                     
            pItems = GetItems(pItems);

            while (pItems != NULL)
            {
               CTreeComboItemArray* pPreviousItems = pItems->m_pPreviousItems;

               CRect crAvoid;
               int nPosition;

               if (pPreviousItems == NULL)
               {
                  GetWindowRect(crAvoid);
                  nPosition = POSITION_Vertical;
               }
               else
               {
                  crAvoid.SetRectEmpty();
                  if (pPreviousItems->m_nSelection >= 0)
                  {
                     GetItemRect(pPreviousItems, pPreviousItems->m_nSelection, crAvoid);
                  }
                  nPosition = POSITION_Horizontal;
               }

               if (!crAvoid.IsRectEmpty())
               {
                  FlowItems(*pDC, crAvoid, crScreen, pItems, nPosition);
               }

               pItems = pItems->GetSubItems();
            }

            pDC->SelectObject(pOldFont);
         }

         ReleaseDC(pDC);
      }
   }
}

void CTreeComboBox::FlowItems(CDC& dcFont, const CRect& crAvoid, const CRect& crScreen, CTreeComboItemArray* pItems, int nPosition)
{
	BOOL fFlowRight = TRUE;
	BOOL fRefit = FALSE;
	int ItemsToAddToEachColumn = 0; 

	int nTotalListWidth = 0;
	int nTotalListHeight = 0;

   // Make sure out inputs are reasonable.
   ASSERT(!crAvoid.IsRectEmpty());
   ASSERT(!crScreen.IsRectEmpty());
   ASSERT(pItems != NULL);
   ASSERT((nPosition == POSITION_Vertical) || (nPosition == POSITION_Horizontal));

   // Get the number of items to flow. Force a count of at least one.
   int nItems = pItems->GetSize();
   if (nItems == 0)
   {
      nItems = 1;
   }

   // Clear any layout information for the items.
   pItems->m_nNumberOfColumns = 0;
   pItems->m_nItemsPerColumn = 0;

   pItems->m_dwaColumnWidth.RemoveAll();

   int nArrowWidth = m_nNormalArrowBitmapWidth;
   if (nArrowWidth < m_nSelectedArrowBitmapWidth)
   {
      nArrowWidth = m_nSelectedArrowBitmapWidth;
   }

	BOOL fNormal = FALSE;

	// Compute the maximum items per column we can have if we flow down.
	int nDownItems = ((crScreen.bottom-((nPosition == POSITION_Vertical) ? crAvoid.bottom : crAvoid.top)-2*m_Border.cy) / m_nItemHeight) + 1;
	int nMaxHeightDownItems = (((crScreen.bottom - crScreen.top) - (2 * m_Border.cy)) / m_nItemHeight) - 1;

	if (nDownItems < 0)
	{
		nDownItems = 0;
	}
	if (nDownItems > nItems)
	{
		nDownItems = nItems;
	}
	int nDownColumns = (nDownItems == 0) ? 0 : ((nItems-1)/nDownItems)+1;

	// Compute the maximum items per column we can have if we flow up.
	int nUpItems = ((((nPosition == POSITION_Vertical) ? crAvoid.top : crAvoid.bottom)-crScreen.top-2*m_Border.cy)/m_nItemHeight) + 0;
	int nMaxHeightUpItems = (((crScreen.bottom - crScreen.top) - (2 * m_Border.cy)) / m_nItemHeight) - 1;
	
	if (nUpItems < 0)
	{
		nUpItems = 0;
	}
	if (nUpItems > nItems)
	{
		nUpItems = nItems;
	}
	int nUpColumns = (nUpItems == 0) ? 0 : ((nItems-1)/nUpItems)+1;

	// Figure out whether to flow up or down. In general, we'll go down
	// unless flowing up results in two or more fewer columns.
	BOOL fFlowDown = TRUE;
	int nColumns = nDownColumns;
	int nColumnItems = nDownItems;
	int nMaxHeightColumnItems = nMaxHeightDownItems;

	if (((nDownColumns == 0) && (nUpColumns > 0))
	 || (nDownColumns-nUpColumns > 1))
	{
		fFlowDown = FALSE;
		nColumns = nUpColumns;
		nColumnItems = nUpItems;
		nMaxHeightColumnItems = nMaxHeightUpItems;
	}

	while(fNormal == FALSE)
	{

		// Save the number of columns.
		pItems->m_nNumberOfColumns = nColumns;

		// Save the number of items in a column.
		pItems->m_nItemsPerColumn = nColumnItems;

		// Compute the width of each column and the total width.
		int nTotalWidth = 0;
		for (int nColumn = 0; nColumn < nColumns; nColumn++)
		{
			// Figure out the items to size for this column.
			int nColumnWidth = 0;
			int nStartItem = nColumn*nColumnItems;
			int nEndItem = nStartItem+nColumnItems;
			if (nEndItem > pItems->GetSize())
			{
				nEndItem = pItems->GetSize();
			}

			// Loop through the items for this column and size them.
			for (int nItem = nStartItem; nItem < nEndItem; nItem++)
			{
				CTreeComboItemArray::Item* pItem = pItems->GetAt(nItem);
				if (pItem != NULL)
				{
	#ifdef WIN32
					int nXExtent = dcFont.GetTextExtent(pItem->GetText()).cx;
	#else
					int nXExtent = dcFont.GetTextExtent(pItem->GetText(), pItem->GetText().GetLength()).cx;
	#endif
            
					int nWidth = m_Margin.cx+nXExtent+m_Margin.cx;
					if (pItem->HasItems())
					{
						nWidth += nArrowWidth;
					}

					if (nWidth > nColumnWidth)
					{
						nColumnWidth = nWidth;
					}
				}
			}

			if (nColumnWidth < m_crClient.Width()/3)
			{
				nColumnWidth = m_crClient.Width()/3;
			}

			nTotalWidth += nColumnWidth;

			// Get the current item.
			if(pItems->m_dwaColumnWidth.GetSize() < nColumn + 1)
			{
				// Record the column width.
				pItems->m_dwaColumnWidth.Add(nColumnWidth);
			}
			else
			{
				pItems->m_dwaColumnWidth.SetAt(nColumn, nColumnWidth);
			}
		}

		// If this is the initial list box (POSITION_Vertical), then
		// make sure it's at least the full width of the control.
		int nControlWidth = m_crClient.Width()-2*m_Border.cx;
		if ((nPosition == POSITION_Vertical) && (nTotalWidth < nControlWidth))
		{
			// Distribute the required pixels evenly.
			int nExtra = (nControlWidth-nTotalWidth)/nColumns;
			for (int nColumn = 0; nColumn < nColumns; nColumn++)
			{
				pItems->m_dwaColumnWidth[nColumn] += nExtra;
			}
			pItems->m_dwaColumnWidth[0] += (nControlWidth-nTotalWidth) % nColumns;
			nTotalWidth = nControlWidth;
		}

		// Get total width and height with borders.
		nTotalListWidth = nTotalWidth+m_Border.cx*(nColumns+1);

      // DGP - nTotalListHeight no longer subtracts 1 from column height if flowing up
      nTotalListHeight = nColumnItems*m_nItemHeight+2*m_Border.cy;
		
		// Figure out which side the total width will fit on.
		int nRightSpace = crScreen.right-crAvoid.right-nTotalListWidth;
		int nLeftSpace = crAvoid.left-crScreen.left-nTotalListWidth;
		fFlowRight = (nRightSpace > 0) || (nRightSpace > nLeftSpace);

		if(fFlowRight)
		{
			if(((int)crAvoid.right + (int)nTotalListWidth) > (int)crScreen.right)
				fNormal = FALSE;
			else
				fNormal = TRUE;
		}
		else
		{
			if(crAvoid.left - nTotalListWidth < crScreen.left)
				fNormal = FALSE;
			else
				fNormal = TRUE;
		}
		
		if(nColumnItems == nMaxHeightColumnItems)
			fNormal = TRUE;

		if(!fNormal)
		{
			fRefit = TRUE;

			int nTotalWidth = 2 * m_Border.cx;

			int ColumnsThatFit = 0;
			
			if(fFlowRight)
			{
				for (int nColumn = 0; nColumn < pItems->m_nNumberOfColumns; nColumn++)
				{
					nTotalWidth += (int)pItems->m_dwaColumnWidth[nColumn] + m_Border.cx;
					if((crAvoid.right + nTotalWidth) > crScreen.right)
					{
						if(nColumn > 0)
							ColumnsThatFit = nColumn;
						break;
					}
					nTotalWidth += m_Border.cx;
				}
			}
			else
			{
				for (int nColumn = 0; nColumn < pItems->m_nNumberOfColumns; nColumn++)
				{
					nTotalWidth += (int)pItems->m_dwaColumnWidth[nColumn];
					if((crAvoid.left - nTotalWidth) < crScreen.left)
					{
						if(nColumn > 0)
							ColumnsThatFit = nColumn - 1;
						break;
					}
					else
					{
						ColumnsThatFit = nColumn;
					}
					nTotalWidth += m_Border.cx;
				}
			}
			
         if(ColumnsThatFit > 0)
			{
				int ItemsToFit = pItems->GetSize();

				ItemsToAddToEachColumn = ((int)((float)((float)ItemsToFit / (float)ColumnsThatFit) + (float)1) - nColumnItems); 
				
				if(nColumnItems + ItemsToAddToEachColumn > nMaxHeightColumnItems)
				{
					ItemsToAddToEachColumn = nMaxHeightColumnItems - nColumnItems;
					ColumnsThatFit = (ItemsToFit / nMaxHeightColumnItems) + 1;
				}

				// Save the number of columns.
				nColumns = ColumnsThatFit;

				// Save the number of items in a column.
				nColumnItems += ItemsToAddToEachColumn;
			}
		}


	}

	// Figure out the final bounds.
   if (nPosition == POSITION_Vertical)
   {
	   CRect crBounds(0, 0, 0, 0);

		if(ItemsToAddToEachColumn > 0)
		{
			GetWindowRect(crBounds);
			ScreenToClient(crBounds);
		}

      if (fFlowRight)
      {
         pItems->m_crBounds.left = crAvoid.left + crBounds.Width();
         pItems->m_crBounds.right = pItems->m_crBounds.left+nTotalListWidth;
      }
      else
      {
         pItems->m_crBounds.right = crAvoid.right - crBounds.Width();
         pItems->m_crBounds.left = pItems->m_crBounds.right-nTotalListWidth;
      }
      if (fFlowDown)
      {
         pItems->m_crBounds.top = crAvoid.bottom - (ItemsToAddToEachColumn * m_nItemHeight);
         pItems->m_crBounds.bottom = pItems->m_crBounds.top+nTotalListHeight;
      }
      else
      {
         pItems->m_crBounds.bottom = crAvoid.top + (ItemsToAddToEachColumn * m_nItemHeight);
         pItems->m_crBounds.top = pItems->m_crBounds.bottom-nTotalListHeight;
      }
   }
   else
   {
      if (fFlowRight)
      {
         pItems->m_crBounds.left = crAvoid.right-2*m_Border.cx + 0;
         pItems->m_crBounds.right = pItems->m_crBounds.left+nTotalListWidth;
      }
      else
      {
         pItems->m_crBounds.right = crAvoid.left+2*m_Border.cx;
         pItems->m_crBounds.left = pItems->m_crBounds.right-nTotalListWidth;
      }
      if (fFlowDown)
      {
         pItems->m_crBounds.top = crAvoid.top-m_Border.cy;
         pItems->m_crBounds.bottom = pItems->m_crBounds.top+nTotalListHeight;
      }
      else
      {
         pItems->m_crBounds.bottom = crAvoid.bottom+m_Border.cy;
         pItems->m_crBounds.top = pItems->m_crBounds.bottom-nTotalListHeight;
      }
   }
}

void CTreeComboBox::RestoreCurrentSelections(void)
{
   ASSERT(m_pCurrentSelectionItems != NULL);
   SetSelection(m_nCurrentSelectionIndex, m_pCurrentSelectionItems);
}

void CTreeComboBox::CommitCurrentSelections(BOOL fNotify)
{
	m_csCurrentSelectionText.Empty();
	m_pCurrentSelectionItems = &m_Items;
	m_nCurrentSelectionIndex = -1;

	CTreeComboItemArray* pItems = GetItems(NULL);
	while ((pItems != NULL) && (pItems->m_nSelection >= 0))
	{
		m_pCurrentSelectionItems = pItems;
		m_nCurrentSelectionIndex = pItems->m_nSelection;

		CTreeComboItemArray::Item* pSelectedItem = pItems->GetAt(pItems->m_nSelection);;
		ASSERT(pSelectedItem != NULL);

		if (!m_csCurrentSelectionText.IsEmpty() && !m_fShowCurLevelOnly)
			m_csCurrentSelectionText += m_csDelimiter;

		if (m_fShowCurLevelOnly)
		{
			CString str = pSelectedItem->GetText();
			if (str != BrowserAllString() ||
				 (str == BrowserAllString() && m_csCurrentSelectionText.IsEmpty()))
			{
				// We don't want to show "All" if we're down more than 1 level
				m_csCurrentSelectionText = str;
			}
		}
		else
			m_csCurrentSelectionText += pSelectedItem->GetText();

		pItems = pSelectedItem->GetItems();
	}

	if (fNotify)
	{
		Util::SendWmCommand(GetParent(), GetDlgCtrlID(), this, CBN_SELCHANGE);
	}
}

// Set the timeout values for showing and hiding listboxes.
void CTreeComboBox::SetTimeouts(UINT uShowTimeout, UINT uHideTimeout)
{
   m_uShowTimeout = uShowTimeout;
   m_uHideTimeout = uHideTimeout;
}

void CTreeComboBox::StartShowTimer(void)
{
   StopShowTimer();
   SetTimer(TIMER_ID_SHOW, m_uShowTimeout, NULL);
   m_fShowTimerActive = TRUE;
}

void CTreeComboBox::StopShowTimer(void)
{
   KillTimer(TIMER_ID_SHOW);
   m_fShowTimerActive = FALSE;
}

void CTreeComboBox::StartHideTimer(void)
{
   StopShowTimer();
   SetTimer(TIMER_ID_HIDE, m_uHideTimeout, NULL);
   m_fHideTimerActive = TRUE;
}

void CTreeComboBox::StopHideTimer(void)
{
   KillTimer(TIMER_ID_HIDE);
   m_fHideTimerActive = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CTreeComboBox message handlers

// Return the handle of our font.
LRESULT CTreeComboBox::OnGetFont(WPARAM wParam, LPARAM lParam)
{
   if (!m_fFontValid)
   {
      return (LRESULT)0;
   }
   return (LRESULT)(UINT)(m_Font.m_hObject);
}

// Set out font. A copy of the font is created locally and new metric
// information is computed.
LRESULT CTreeComboBox::OnSetFont(WPARAM wParam, LPARAM lParam)
{
   DeleteFont();

   HFONT hFont = NULL;
   if (wParam != 0)
   {
      // Get the font information.
      LOGFONT LogFont;
      if (::GetObject((HFONT)wParam, sizeof(LogFont), &LogFont) != 0)
      {
         // Create the font.
         if (m_Font.CreateFontIndirect(&LogFont))
         {
            // Extract the font metrics we'll need for this control.
            CDC* pDC = GetDC();
            if (pDC != NULL)
            {
               if (pDC->GetTextMetrics(&m_TextMetric))
               {
                  m_nItemHeight = m_Margin.cy+m_TextMetric.tmHeight-m_TextMetric.tmInternalLeading+m_Margin.cy;
                  m_fFontValid = TRUE;
               }

					ReleaseDC(pDC);
            }
         }
      }
   }

   // Redraw if we are supposed to.
   if (LOWORD(lParam) != 0)
   {
      Invalidate();
   }
   
   return 0;
}

// Return our text to the caller.
LRESULT CTreeComboBox::OnGetText(WPARAM wParam, LPARAM lParam)
{
   int nMaxCharacters = (int)wParam;
   LPSTR pszText = (LPSTR)lParam;

   if ((nMaxCharacters > 0) && (pszText != NULL))
   {
      strncpy(pszText, m_csCurrentSelectionText, nMaxCharacters-1);
      pszText[nMaxCharacters] = '\0';
      return (LRESULT)(strlen(pszText)+1);
   }

   return (LRESULT)0;
}

// Setting text is not supported until we have an edit control (same as COMBOBOX).
LRESULT CTreeComboBox::OnSetText(WPARAM wParam, LPARAM lParam)
{
   return (LRESULT)CB_ERR;
}

// Erase the background of the combo box. We draw a sunken 3D border in palette colors.
BOOL CTreeComboBox::OnEraseBkgnd(CDC* pDC) 
{
   // Draw the sunken 3D border:
   //
   //    S-----------SH
   //    |D---------DL|
   //    ||          ||
   //    ||          ||
   //    |D          ||
   //    SL----------L|
   //    H------------H

   // Select in the palette.
   CPalette* pOldPalette;
   SelectPalette(*pDC, pOldPalette);

   // Draw the 3D border.
   Draw3DBorder(*pDC, m_crClient, m_clBorderShadow, m_clBorderHighlight, m_clBorderDark, m_clBorderLight);

   // Deselect the palette.
   DeselectPalette(*pDC, pOldPalette);

   // Return TRUE since we drew something.
   return TRUE;
}

// Paint the combo box. We draw the text and possibly highlight the control if we
// have the focus and the list is not dropped down.
void CTreeComboBox::OnPaint() 
{
   CPaintDC dc(this); // device context for painting

   // Select the palette.
   CPalette* pOldPalette;
   SelectPalette(dc, pOldPalette);

   // Draw the button.
   DrawButton(dc);

   // Fill in the background.
#ifdef WIN32
   dc.FillSolidRect(m_crBackground, m_clWindow);
#else
   CBrush BackgroundBrush;
   if (BackgroundBrush.CreateSolidBrush(m_clWindow))
   {
      dc.FillRect(m_crBackground, &BackgroundBrush);
   }
#endif

   // Draw highlighted if we have the focus.
   BOOL fHighlight = GetFocus() == this;
   if (fHighlight)
   {
      CRect crHighlight(m_crText);
      crHighlight.InflateRect(1, 1);
#ifdef WIN32
      dc.FillSolidRect(crHighlight, m_clHighlight);
#else
      CBrush HighlightBrush;
      if (HighlightBrush.CreateSolidBrush(m_clHighlight))
      {
         dc.FillRect(m_crBackground, &HighlightBrush);
      }
#endif
   }

   // Draw the text.
   if (m_fFontValid)
   {
      int nOldBkMode = dc.SetBkMode(TRANSPARENT);
      COLORREF clOldTextColor = dc.SetTextColor(fHighlight ? m_clHighlightText : m_clWindowText);
      CFont* pOldFont = dc.SelectObject(&m_Font);

#ifdef WIN32
      dc.DrawText(m_csCurrentSelectionText, m_crText, DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
#else
      dc.DrawText(m_csCurrentSelectionText, m_csCurrentSelectionText.GetLength(), m_crText, DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
#endif

      dc.SelectObject(pOldFont);
      dc.SetTextColor(clOldTextColor);
      dc.SetBkMode(nOldBkMode);
   }

   // Deselect the palette.
   DeselectPalette(dc, pOldPalette);
}

void CTreeComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
   CWnd::OnLButtonDown(nFlags, point);

   if (m_crClient.PtInRect(point) && !IsListShown())
   {
      // The user clicked on us while the list was not being
      // shown. In this case, open up the list. If the user
      // clicked on the button, then set a flag so that we
      // can move the button up and down as the mouse moves.

      SetFocus();

      if (m_crButton.PtInRect(point))
      {
         m_fButtonCapture = TRUE;
         PushButton();
      }

      ShowList();
   }
   else
   {
      // The user clicked somewhere other than our window.
      // If the user did not click in a list box, then close
      // close up the combo box without making a selection.
      if (GetListFromPoint(point) == NULL)
      {
         CancelMode();
      }
   }
}

void CTreeComboBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
   // The user released the mouse button. If the user initially
   // clicked on the button, then pop the button back up.
   if (m_fButtonCapture)
   {
      m_fButtonCapture = FALSE;
      PushButton(FALSE);
   }


   if (!m_crClient.PtInRect(point) && IsListShown())
   {
      // The user released the mouse button somewhere outside
      // our window and the list is shown. If the button was
      // released over a list box, make the selection. In any
      // case close up the list box.
      if (GetListFromPoint(point) != NULL)
      {
         CommitCurrentSelections(TRUE);
      }

      CancelMode();
   }

   CWnd::OnLButtonUp(nFlags, point);
}

void CTreeComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
   // The mouse has moved. If we are moving the button up
   // and down, then check if the mouse is over the button
   // and draw the button accordingly.
   if (m_fButtonCapture)
   {
      PushButton(m_crButton.PtInRect(point));
   }

   // Get a pointer to any list box that we're over.
   CTreeComboList* pList = GetListFromPoint(point);

   // Windows sends mouse move messages to a window
   // when it is uncovered. We care about this because
   // we don't want to process a mouse move which is
   // the result of hiding a list box which is no longer
   // part of the selection path. This is really easy to
   // do it you leave the mouse in an area where list
   // boxes are coming and going and try to navigate
   // using the keyboard,
   if ((!m_fLastPointValid)
    || (m_LastPoint != point)
    || (pList == NULL))
   {
      // Remember the old active list in case we change.
      CTreeComboList* pPreviousActiveList = m_pActiveList;
      m_pActiveList = NULL;

      // Check if we are over a list box. If so, this will be the
      // new active list.
      if ((pList != NULL) && (pList->m_pItems != NULL))
      {
         // Once we move over a list box, the button
         // capture behavior is canceled.
         PushButton(FALSE);
         m_fButtonCapture = FALSE;

         // Remember the new active list.
         m_pActiveList = pList;

         // Set the new selection.
         SetSelection(GetItemFromPoint(pList->m_pItems, point), pList->m_pItems);
      }

      // If we had an active list and we are no longer in that list, then
      // we may want to remove the selection from the previous list. For example,
      // if we move the mouse off a list box, we may want to remove the selection
      // from the list box. The one exception is when we move out of one list
      // box into the next list box in the select path. In this case, we do not
      // want to remove the selection from the previous list box since it is part
      // of the select path.

      if ((pPreviousActiveList != NULL) && (pPreviousActiveList != m_pActiveList))
      {
         if (m_pActiveList == NULL)
         {
            // We're moving off a list box onto something that's not
            // a list box. In this case we always remove the selection.
            SetSelection(-1, pPreviousActiveList->m_pItems);
         }
         else
         {
            // Since were moving from one list box to another, reset the
            // hide timer so the list box won't go away too quickly.
            // Basically, nothing will go away until the user keeps the
            // mouse in one list box.
            StopHideTimer();

            // We're moving off a list box onto another list box. Run through the
            // selection path and see which list box we come to first. If we come
            // to the new list box first, then we remove the selection. If we come
            // to the old list box first, then we don't since the new list box is
            // later in the select path.

            CTreeComboItemArray* pItems = &m_Items;
            while ((pItems != NULL) && (pItems != pPreviousActiveList->m_pItems))
            {
               if (pItems == m_pActiveList->m_pItems)
               {
                  // Found the new list box first, remove the selection.
                  SetSelection(-1, pPreviousActiveList->m_pItems);
                  break;
               }
               pItems = pItems->GetSubItems();
            }
         }
      }
   }

   // Remember the last mouse point so we can filter out
   // bogus messages (seem comment above.)
   m_LastPoint = point;
   m_fLastPointValid = TRUE;

   CWnd::OnMouseMove(nFlags, point);
}

void CTreeComboBox::OnSetFocus(CWnd* pOldWnd) 
{
   // We just got the focus! Redraw ourselves to show our new state.
   Invalidate();
   CWnd::OnSetFocus(pOldWnd);
}


void CTreeComboBox::OnKillFocus(CWnd* pNewWnd) 
{
   // The combo box has lost the focus. Close up shop without
   // making any new selections.
   CWnd::OnKillFocus(pNewWnd);
   CancelMode();
}

void CTreeComboBox::OnCancelMode() 
{
   // Someone wants up to shut down.
   CWnd::OnCancelMode();
   CancelMode();
}

void CTreeComboBox::OnTimer(UINT nIDEvent) 
{
   if (nIDEvent == TIMER_ID_SHOW)
   {
      OnShowTimer();
   }

   if (nIDEvent == TIMER_ID_HIDE)
   {
      OnHideTimer();
   }

   CWnd::OnTimer(nIDEvent);
}

void CTreeComboBox::OnShowTimer(void)
{
   StopShowTimer();

   CTreeComboItemArray* pShowItems = m_pShowItems;
   m_pShowItems = NULL;

   if (pShowItems != NULL)
   {
      Flow(pShowItems);
      ShowList(TRUE, pShowItems, FALSE);
   }
}

void CTreeComboBox::OnHideTimer(void)
{
   StopHideTimer();
   HideUnselectedItems();
}

void CTreeComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if (nChar == VK_ESCAPE)
   {
      // ESC always closes up the list box without selecting.
      CancelMode();
   }

   if (nChar == VK_RETURN)
   {
      // Return always commits the current selections and closes the list box.
      CommitCurrentSelections(TRUE);
      CancelMode();
   }

   if (nChar == VK_F4)
   {
      // F4 selects/opens or closes the list box.
      if (m_fListShown)
      {
         CommitCurrentSelections(TRUE);
         CancelMode();
      }
      else
      {
         ShowList(TRUE);
      }
   }

   // Get the items we want to operate on.
   CTreeComboItemArray* pItems = GetActiveItems();
   if (pItems != NULL)
   {
      if (!m_fListShown)
      {
         if ((nChar == VK_UP)
          || (nChar == VK_DOWN)
          || (nChar == VK_LEFT)
          || (nChar == VK_RIGHT)
          || (nChar == VK_HOME)
          || (nChar == VK_END)
          || (nChar == VK_NEXT)
          || (nChar == VK_PRIOR))
         {
            ShowList(TRUE);
         }
      }
      else
      {
         // Get current selection.
         int nSelection = pItems->m_nSelection;

         // Process arrow keys.
         if (nChar == VK_UP)
         {
            // Move up an item.
            if (nSelection == -1)
            {
               nSelection = pItems->GetSize();
            }
            nSelection--;
         }
         
         if (nChar == VK_DOWN)
         {
            // Move down an item.
            nSelection++;
         }

         if (nChar == VK_HOME)
         {
            // Move to first item.
            nSelection = 0;
         }
         
         if (nChar == VK_END)
         {
            // Move to last item.
            nSelection = pItems->GetSize()-1;
         }

         if (nChar == VK_PRIOR)
         {
            // Move to start of column (or previous column.)
            int nColumn = pItems->GetColumnFromItem(nSelection);
            if (pItems->GetItemInColumn(nSelection) == 0)
            {
               if (nColumn > 0)
               {
                  nColumn--;
               }
            }
            nSelection = pItems->GetFirstItemInColumn(nColumn);
         }

         if (nChar == VK_NEXT)
         {
            // Move to end of column (or next column.)
            int nColumn = pItems->GetColumnFromItem(nSelection);
            if (pItems->GetItemInColumn(nSelection) == pItems->m_nItemsPerColumn-1)
            {
               if (nColumn < pItems->m_nNumberOfColumns-1)
               {
                  nColumn++;
               }
            }
            nSelection = pItems->GetLastItemInColumn(nColumn);
         }
         
         if (nChar == VK_LEFT)
         {
            // Move left a column or to the previous list if we're
            // in the first column.
            if (nSelection >= pItems->m_nItemsPerColumn)
            {
               nSelection -= pItems->m_nItemsPerColumn;
            }
            else
            {
               if (pItems->m_pPreviousItems != NULL)
               {
                  SetCurSel(-1, pItems, FALSE);
                  pItems = pItems->m_pPreviousItems;
                  nSelection = pItems->m_nSelection;
               }
            }
         }

         if (nChar == VK_RIGHT)
         {
            // Move to the right. If we're on an item with sub items, then
            // move to the first item in the sub items. If the item does not
            // have sub items, them we move to the next column, If we're at
            // the last column, then we look back to previous lists for the
            // first place were we can move to the next column.
            CTreeComboItemArray* pSubItems = pItems->GetSubItems();
            if (pSubItems != NULL)
            {
               // Move to first item in sub list.
               pItems = pSubItems;
               nSelection = 0;
            }
            else
            {
               while (pItems != NULL)
               {
                  nSelection = pItems->m_nSelection;
                  if (pItems->GetColumnFromItem(nSelection) < pItems->m_nNumberOfColumns-1)
                  {
                     nSelection += pItems->m_nItemsPerColumn;
                     break;
                  }
                  else
                  {
                     pItems = pItems->m_pPreviousItems;
                  }
               }
            }
         }

         // Move the selection.
         if (pItems != NULL)
         {
            if (nSelection < 0)
            {
               nSelection = 0;
            }

            if (nSelection >= pItems->GetSize())
            {
               nSelection = pItems->GetSize()-1;
            }

            if (nSelection != pItems->m_nSelection)
            {
               SetCurSel(nSelection, pItems, FALSE);
            }
         }
      }
   }
      
   CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTreeComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if (nChar >= 32)
   {
      // Search for item in list box which starts with the specified character.
      CTreeComboItemArray* pItems = GetActiveItems();
      if (pItems != NULL)
      {
         // BUild a string containing the character.
         char szString[2];
         szString[0] = nChar;
         szString[1] = '\0';

         // Find the matching item.
         int nIndex = FindString(pItems->m_nSelection, szString, pItems);
         if (nIndex != CB_ERR)
         {
            // Be sure the list is dropped down.
            ShowList(TRUE);

            // Select the item.
            SetCurSel(nIndex, pItems, FALSE);
         }
      }
   }
   
   CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CTreeComboBox::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   if ((nChar == VK_UP) || (nChar == VK_DOWN))
   {
      // ALT Up/Down Arrow selects/opens or closes the list box (same as F4).
      if (m_fListShown)
      {
         CommitCurrentSelections(TRUE);
         CancelMode();
      }
      else
      {
         ShowList(TRUE);
      }
   }

   CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

UINT CTreeComboBox::OnGetDlgCode() 
{
   // We always want the arrow keys.
   UINT uCode = CWnd::OnGetDlgCode() | DLGC_WANTARROWS;

   // We want all characters plus Escape and Return if the list is shown.
   if (m_fListShown && Util::IsKeyMessage((LPMSG)(GetCurrentMessage()->lParam), VK_ESCAPE, VK_RETURN, 0))
   {
      uCode |= DLGC_WANTALLKEYS;
   }
   else
   {
      uCode |= DLGC_WANTCHARS;
   }

   return uCode;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeComboList class

IMPLEMENT_DYNAMIC(CTreeComboList, CWnd)

// Constructor
CTreeComboList::CTreeComboList(CTreeComboBox* pComboBox, CTreeComboItemArray* pItems)
{
   ASSERT(pComboBox != NULL);
   m_pComboBox = pComboBox;
   m_pItems = pItems;

   if (m_pItems != NULL)
   {
      // It's our job to hook the item array up to us.
      m_pItems->m_pList = this;
   }
}

// Destructor
CTreeComboList::~CTreeComboList()
{
   if (m_pItems != NULL)
   {
      // It's our job to unhook the item array from us.
      ASSERT(m_pItems->m_pList == this);
      m_pItems->m_pList = NULL;
      m_pItems = NULL;
   }
}

BEGIN_MESSAGE_MAP(CTreeComboList, CWnd)
   //{{AFX_MSG_MAP(CTreeComboList)
   ON_WM_ERASEBKGND()
   ON_WM_PAINT()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Create the control.
BOOL CTreeComboList::Create(DWORD dwStyle, const RECT& Bounds, CWnd* pParent, UINT uID)
{
   BOOL fResult = FALSE;
   
   // CreateEx is called so we can specify the extended style WS_EX_TOOLWINDOW.
   // This style is required in Win32 so the list box windows do not appear
   // in the task list and task bar (the normal ComboLBox class has this style.)
   if (CWnd::CreateEx(
#ifdef WIN32
         WS_EX_TOOLWINDOW,
#else 
         0,
#endif
         CTreeComboBox::m_pszComboListClassName,
         "PrintMaster",
         dwStyle,
         Bounds.left,
         Bounds.top,
         Bounds.right-Bounds.left,
         Bounds.bottom-Bounds.top,
         pParent->GetSafeHwnd(),
         (HMENU)uID,
         NULL))
   {
      fResult = TRUE;
   }

   return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CTreeComboList message handlers

// Erase the list box background.
BOOL CTreeComboList::OnEraseBkgnd(CDC* pDC) 
{
   // Let the combo box do the drawing.
   return m_pComboBox->ListEraseBackground(this, *pDC);
}

// Paint the list box background.
void CTreeComboList::OnPaint() 
{
   // Let the combo box do the drawing.
   CPaintDC dc(this);
   m_pComboBox->ListPaint(this, dc);
}
