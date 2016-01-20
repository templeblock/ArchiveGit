/*
// $Workfile: ADDRBOOK.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADDRBOOK.CPP $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 47    10/28/98 11:15a Mwilson
// fixed bug for first time run
// 
// 46    9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 45    7/21/98 11:16a Johno
// Skip posting WM_SENDER_DATABASE_EMPTY in CSenderInformation::OnPaint()
// (eliminates redundant dialog)
// 
// 44    7/20/98 1:15p Johno
// Call BeforeRecordChange() and AfterRecordChange() in
// OnFileExportnames() to make sure all records get exported.
// 
// 43    7/15/98 3:13p Johno
// Skip fwrite() in WriteFieldString() if buffer is empty - avoids
// ThrowErrorcodeException()
// 
// 42    7/11/98 3:37p Rlovejoy
// Added SortReceiverList().
// 
// 41    7/11/98 2:48p Jayn
// Got rid of some resource leaks.
// 
// 40    7/10/98 11:22a Rlovejoy
// Determine the address book's safe owner at modal time.
// 
// 39    7/10/98 11:07a Rlovejoy
// Store the dialog's parent.
// 
// 38    7/09/98 2:38p Rlovejoy
// Set focus to Add button on close.
// 
// 37    7/07/98 4:11p Jayn
// Does not use registration info as defaults for sender info.
// 
// 36    6/26/98 10:23a Johno
// Setting document title for print progress dialog now works
// 
// 35    6/25/98 6:03p Johno
// Attempt (failed) to set document name so it is used in print progress
// dialog
// 
// 34    6/12/98 11:15a Cboggio
// Fixed potential localization crash problem in SetupFieldFromIniFile 
// 
// 33    6/05/98 4:50p Hforman
// don't show Helpful Hint if  m_fReceiverList == TRUE
// 
// 32    6/05/98 8:39a Fredf
// Better handling of parent window.
// 
// 31    6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 30    5/31/98 7:27p Psasse
// Workspace Helpful Hint for AddressBook
// 
// 29    5/31/98 5:11p Fredf
// More Address Book and merge field changes.
// 
// 28    5/31/98 12:52p Fredf
// Start of address book changes
// 
// 27    5/27/98 5:56p Hforman
// remove SEND_WELCOME
// 
// 26    5/20/98 11:42a Cboggio
// Added calls to allow a slightly more flexible address book format for
// Localized versions. A better, more flexible solution is still required,
// but this will work for now.
// 
// 25    5/15/98 2:43p Cboggio
// Put some strings into the resource file
// 
// 24    4/27/98 5:29p Fredf
// Improved bar positioning.
// 
// 23    2/24/98 5:48p Johno
// Not my task anymore so NOTHING IS MY FAULT!
// Use #define LOOK_AND_FEEL in addrbook.h to turn on / off most changes
// 
// 22    2/20/98 12:15p Johno
// Compile update
// 
// 21    2/17/98 5:14p Johno
// Moved suppression of double sided print wizard
// to inline PrintProject in addprnvw.h
// 
// 20    2/17/98 2:25p Johno
// Restore DoubleSidedType after printing.
// 
// 19    2/17/98 12:37p Johno
// Suppress double sided print wizard
// 
// 18    2/13/98 10:54a Johno
// Changed CFileDialog in OnFileExportnames() to take an initial
// directory, but not an initial file name.
// 
// 17    2/12/98 3:48p Johno
// AddrView.PrintProject in OnFilePrint now takes a
// CFlatFileDatabaseCursor*
// so that the view can set the index type, and therefor the record sort
// order.
// 
// 16    2/12/98 12:09p Johno
// Print menu item disabled for empty address books
// 
// 15    2/11/98 5:54p Johno
// Clean up
// 
// 14    2/09/98 4:05p Johno
// CAddressBookListBox::Refresh sets selection to first item
// 
// 13    2/06/98 1:24p Johno
// Now use CPrintInfo to get print range
// 
// 12    2/05/98 3:43p Johno
// Quick! Before the power goes out!
// 
// 11    1/30/98 11:13a Johno
// Compile update
// 
// 10    1/28/98 1:06p Johno
// So you can compile
// 
// 9     1/27/98 11:50a Johno
// For a version that will compile
// 
// 8     1/20/98 3:07p Johno
// So you can build
// 
// 7     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 6     1/16/98 4:40p Johno
// Kludge fix for OnFileImportnames
// Hey, its better than crashing
// 
// 5     1/16/98 12:18p Johno
// Clean up
// 
// 4     1/16/98 10:58a Johno
// Added Export (Save As) stuff.
// 
// 3     1/13/98 9:16a Johno
// 
// 2     1/12/98 4:16p Johno
// 
//    Rev 1.1   16 Oct 1997 11:42:54   hal
// params to GetWindowPosition() changed -- also added ShowWindow() calls
// 
//    Rev 1.0   14 Aug 1997 15:18:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:02   Fred
// Initial revision.
// 
//    Rev 1.70   13 Jun 1997 17:43:10   Jay
// Updates fields if sender is edited.
// 
//    Rev 1.69   09 Jun 1997 17:26:06   Jay
// Got rid of VERIFYs causing assertions.
// 
//    Rev 1.68   23 May 1997 16:15:22   Jay
// No more _000PMW.INI
// 
//    Rev 1.67   25 Apr 1997 10:17:40   Jay
// Re-arranged code to get rid of compiler error.
// 
//    Rev 1.66   23 Apr 1997 15:22:12   johno
// 
//    Rev 1.65   22 Apr 1997 12:09:30   johno
// Added color to "Run" command
// 
// 
//    Rev 1.64   21 Apr 1997 10:47:40   johno
// Changes for welcome dialog
// 
//    Rev 1.63   17 Apr 1997 15:14:26   johno
// Changes for the lil' PrintMaster guy
// 
// 
//    Rev 1.62   07 Apr 1997 11:30:36   johno
// Overloaded Invalidate () for address book
// (special case)
// 
//    Rev 1.61   04 Apr 1997 17:52:52   johno
// Derived from pmwdialogcolor
// 
//    Rev 1.60   28 Mar 1997 17:10:38   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.59   12 Nov 1996 09:53:50   johno
// Moved strings to resource DLL
// 
//    Rev 1.58   28 Oct 1996 13:59:24   Jay
// Moved changes from final 3.01.
// 
//    Rev 1.58   22 Oct 1996 09:58:54   Fred
// OnSubclassDlg() validates pointer
// 
//    Rev 1.57   14 Oct 1996 11:04:18   Jay
// New sounds.
// 
//    Rev 1.56   10 Sep 1996 12:50:08   Fred
// Fixed some bugs with switching between address books.
// 
//    Rev 1.55   08 Sep 1996 16:52:24   Fred
// FIxed case where default address book selection was failing
// 
//    Rev 1.54   08 Sep 1996 09:40:12   Jay
// CPmwDialogs with no sound.
// 
//    Rev 1.53   08 Sep 1996 09:23:18   Fred
// Confirm delete address book; New classes which center CDialog based dialogs.
// 
//    Rev 1.52   06 Sep 1996 18:13:34   Jay
// Sound changes.
// 
//    Rev 1.51   05 Sep 1996 08:34:08   Fred
// Fixed bug where pressing enter could cause the address book to go away.
// 
//    Rev 1.50   01 Sep 1996 10:53:10   Fred
// Orphaned fields dialog is now a CDialog since we don't have a sound for it.
// 
//    Rev 1.49   28 Aug 1996 21:25:44   Fred
// More enforcement of maximum number of names
// 
//    Rev 1.48   28 Aug 1996 15:22:40   Fred
// Enforces maximum number of names
// 
//    Rev 1.47   28 Aug 1996 13:25:44   Fred
// No longer crashes in WIN16 with long file name in new dialog
// 
//    Rev 1.46   27 Aug 1996 14:39:14   Fred
//  
// 
//    Rev 1.45   27 Aug 1996 14:04:56   Fred
// Fixed wandering splitter
// 
//    Rev 1.44   21 Aug 1996 18:42:34   Fred
// New address book closes current database (works now)
// 
//    Rev 1.43   21 Aug 1996 16:19:38   Fred
// Added CPmwDialog derived dialog classes for some sender dialogs.
// New Address Book closes current database.
// 
//    Rev 1.42   09 Aug 1996 16:50:10   Fred
// More 16-bit fixes
// 
//    Rev 1.41   08 Aug 1996 17:30:20   Fred
// More 16-bit changes
// 
//    Rev 1.40   05 Aug 1996 18:55:26   Fred
//  
// 
//    Rev 1.39   05 Aug 1996 10:32:30   Fred
//  
// 
//    Rev 1.38   03 Aug 1996 16:42:46   Fred
//  
// 
//    Rev 1.37   02 Aug 1996 14:00:56   Fred
//  
// 
//    Rev 1.36   02 Aug 1996 13:10:46   Fred
// More 16-bit changes
// 
//    Rev 1.35   01 Aug 1996 13:58:36   Fred
// Enables index menu items
// 
//    Rev 1.34   01 Aug 1996 12:48:26   Fred
// Fixed bug with inserting menu items
// 
//    Rev 1.33   01 Aug 1996 10:58:54   Fred
//  
// 
//    Rev 1.32   01 Aug 1996 09:22:38   Fred
// More 16-bit changes
// 
//    Rev 1.31   31 Jul 1996 11:57:50   Fred
// 16-bit compatibility
// 
//    Rev 1.30   25 Jul 1996 11:45:12   Fred
// Fixed some refresh problems
// 
//    Rev 1.29   24 Jul 1996 13:12:20   Fred
// Moved CMlsStringArray to SORTARRY
// 
//    Rev 1.28   23 Jul 1996 08:05:04   Fred
// Some bug fixes
// 
//    Rev 1.27   19 Jul 1996 15:26:46   Fred
//  
// 
//    Rev 1.26   18 Jul 1996 14:38:56   Jay
// Makes full path before call to make directory.
// 
//    Rev 1.25   18 Jul 1996 13:49:24   FRED
// Creates address book paths
// 
//    Rev 1.24   17 Jul 1996 16:38:40   Fred
// Uses addrdata; compresses pictures
// 
//    Rev 1.23   17 Jul 1996 12:56:54   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.22   17 Jul 1996 09:04:56   Fred
// Some address book improvements
// 
//    Rev 1.21   16 Jul 1996 17:30:32   Fred
// More address/sender book stuff
// 
//    Rev 1.20   16 Jul 1996 08:50:44   Fred
// Hooked up address/sender book
// 
//    Rev 1.19   12 Jul 1996 16:26:40   Fred
// Address book error handling
// 
//    Rev 1.18   11 Jul 1996 08:37:24   Fred
// More address book features
// 
//    Rev 1.17   06 Jul 1996 12:17:38   Fred
// Address book improvements
// 
//    Rev 1.16   06 Jul 1996 09:25:14   Fred
// Picture editor and other improvements
// 
//    Rev 1.15   03 Jul 1996 18:52:18   Fred
// Picture editor
// 
//    Rev 1.14   02 Jul 1996 10:56:30   Fred
// Sender Information
// 
//    Rev 1.13   01 Jul 1996 07:30:16   Fred
// Receiver list
// 
//    Rev 1.12   27 Jun 1996 10:42:46   Fred
// Receiver list. Before removing receiver from non-receiver views
// 
//    Rev 1.11   24 Jun 1996 19:16:18   Fred
// Some fixes and improvments
// 
//    Rev 1.10   24 Jun 1996 00:29:48   Fred
// Address Book Overview
// 
//    Rev 1.9   21 Jun 1996 09:29:46   Fred
// More overview list
// 
//    Rev 1.8   21 Jun 1996 07:00:52   Fred
// Start of overview list box
// 
//    Rev 1.7   18 Jun 1996 15:20:38   Fred
// Field value list management and editing
// 
//    Rev 1.6   18 Jun 1996 08:24:54   Fred
// More adress book changes
// 
//    Rev 1.5   10 Jun 1996 18:43:44   Fred
// Memo editor
// 
//    Rev 1.4   10 Jun 1996 08:23:36   Fred
// Lots more stuff
// 
//    Rev 1.3   05 Jun 1996 08:50:24   Fred
// New stuff
// 
//    Rev 1.2   03 Jun 1996 14:43:22   Fred
// Added menu bar and accelerators.
// Auto enable/disable of controls also works.
// 
//    Rev 1.1   03 Jun 1996 08:44:24   Fred
//  
// 
//    Rev 1.0   24 May 1996 09:39:40   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"
#include "pmwdoc.h"
#include "pmwview.h"

#include "util.h"
#include "cfixed.h"
#include "sortarry.h"
#include "namerec.h"
#include "inifile.h"
#include "delname.h"
#include "flatdb.h"
#include "addrimp.h"
#include "pmwwiz.h"
#include "addrdata.h"
#include "addrbook.h"
#include <io.h>
#include "colordef.h"
#include "hint.h"

#include <..\src\afximpl.h>         // for AfxCancelModes()
#include <afxpriv.h>

// For printing
#include "prnaddlg.h"
#include "addprnvw.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

#define new DEBUG_NEW

// Class names used by address book.
static char szComboButtonClassName[] = "MLSComboButton";
static char szComboListClassName[] = "MLSComboList";

// Translate view list box to command.

static UINT IndexToView[] =
{
   ID_VIEW_ONEATATIME,        // index 0
   ID_VIEW_OVERVIEW,          // index 1
   ID_VIEW_RECEIVERLIST       // index 2
};

// Helper function to move or expand a bunch of controls. Each control and its
// movement flags are stored as an element in an array. A pointer to the array
// along with the size deltas are passed in.
void MoveControls(CWnd* pParent, int nX, int nY, ControlMovement* pMovement)
{
   // March through each array element.
   for (int nIndex = 0; pMovement[nIndex].m_nID != 0; nIndex++)
   {
      // Get the window associated with the control.
      CWnd* pWnd = pParent->GetDlgItem(pMovement[nIndex].m_nID);
      if (pWnd != NULL)
      {
         // Figure out the new window size and position based on the
         // movement flags.
         CRect crRect;
         pWnd->GetWindowRect(crRect);
         pParent->ScreenToClient(crRect);

         CRect crNew(crRect);

         WORD wMovement = pMovement[nIndex].m_wMovement;

         if ((wMovement & ControlMovement::MX) != 0)  crNew.OffsetRect(nX, 0);
         if ((wMovement & ControlMovement::MY) != 0)  crNew.OffsetRect(0, nY);
         if ((wMovement & ControlMovement::EX) != 0)  crNew.right += nX;
         if ((wMovement & ControlMovement::EY) != 0)  crNew.bottom += nY;

         // Move the window.
         pWnd->MoveWindow(crNew, FALSE);
      }
   }
}

// Convert a splitter value to a fraction of a window size.
CFixed inline SplitterToFraction(int nSplitter, int nWidth)
{
   return MakeFixed(((double)nSplitter)/((double)nWidth));
}

// Convert a fraction of a window size to a splitter value.
int inline FractionToSplitter(CFixed lFraction, int nWidth)
{
   return (int)(MakeDouble(lFraction)*nWidth+0.5);
}

CIndexList::CIndexList()
{
}

CIndexList::~CIndexList()
{
	DeleteAll();
}

int CIndexList::GetSize(void)
{
	ASSERT((m_Data.GetSize() % 2) == 0);
	return m_Data.GetSize()/2;
}

int CIndexList::Add(LPCSTR pszName, DWORD dwIndex)
{
	int nResult = -1;

	ASSERT((m_Data.GetSize() % 2) == 0);
	ASSERT(pszName != NULL);

	if (pszName != NULL)
	{
		LPSTR pszCopy = new char[strlen(pszName)+1];
		strcpy(pszCopy, pszName);
		m_Data.Add((void*)pszCopy);
		m_Data.Add((void*)dwIndex);
		nResult = GetSize()-1;
	}

	return nResult;
}

BOOL CIndexList::GetAt(int nIndex, LPCSTR& pszName, DWORD& dwIndex)
{
	BOOL fResult = FALSE;

	ASSERT((m_Data.GetSize() % 2) == 0);
	ASSERT((nIndex >= 0) && (nIndex < GetSize()));

	if ((nIndex >= 0) && (nIndex < GetSize()))
	{
		pszName = (LPCSTR)(m_Data.GetAt(nIndex*2+0));
		dwIndex = (DWORD)(m_Data.GetAt(nIndex*2+1));
		fResult = TRUE;
	}

	return fResult;
}

void CIndexList::DeleteAt(int nIndex)
{
	ASSERT((m_Data.GetSize() % 2) == 0);
	ASSERT((nIndex >= 0) && (nIndex < GetSize()));

	if ((nIndex >= 0) && (nIndex < GetSize()))
	{
		LPCSTR pszName;
		DWORD dwIndex;

		if (GetAt(nIndex, pszName, dwIndex))
		{
			delete [] (char*)pszName;
			pszName = NULL;
		}
	}

	m_Data.RemoveAt(nIndex*2+1);
	m_Data.RemoveAt(nIndex*2+0);
}

void CIndexList::DeleteAll(void)
{
	for (int i = GetSize()-1; i >= 0; i--)
	{
		DeleteAt(i);
	}
}

int CIndexList::FindByIndex(DWORD dwIndexToFind)
{
	int nSize = GetSize();
	for (int i = 0; i < nSize; i++)
	{
		LPCSTR pszName;
		DWORD dwIndex;
		if (GetAt(i, pszName, dwIndex))
		{
			if (dwIndex == dwIndexToFind)
			{
				return i;
			}
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookNameList

CAddressBookNameList::CAddressBookNameList()
{
   m_fChanged = FALSE;
}

CAddressBookNameList::CAddressBookNameList(CAddressBookNameList& List)
{
   m_fChanged = FALSE;

	SetSize(List.GetNameCount());
	List.GetNames(GetData());
}

CAddressBookNameList::~CAddressBookNameList()
{
}

// Search for a value.
int CAddressBookNameList::Find(DWORD dwValue)
{
	// Do a linear search on the unsorted array
	int nIndex = -1;

	int i = 0;
	int nMax = GetSize();
	while (i < nMax)
	{
		if (GetAt(i) == dwValue)
		{
			nIndex = i;
			break;
		}
		i++;
	}

	return nIndex;
}


// Add a name.
void CAddressBookNameList::AddName(DWORD dwID)
{
   int nIndex = Find(dwID);
   if (nIndex == -1)
   {
      Add(dwID);
      SetChanged();
   }
}
   
// Delete a name by index.
void CAddressBookNameList::DeleteName(int nName)
{
   if (nName != -1)
   {
      RemoveAt(nName);
      SetChanged();
   }
}

// Delete all names.
void CAddressBookNameList::DeleteAllNames(void)
{
   if (GetSize() > 0)
   {
      RemoveAll();
      SetChanged();
   }
}

// Block copy to outside ID array
void CAddressBookNameList::GetNames(DWORD* pNames)
{
   int nCount = GetNameCount();
   if (nCount > 0)
   {
      memcpy(pNames, &ElementAt(0), nCount*sizeof(DWORD));
   }
}

// Individual copy to outside ID array
void CAddressBookNameList::GetNames(CDWordArray* pNames)
{
   int nCount = GetNameCount();
   if (nCount > 0)
   {
		int i;
		for (i = 0; i < nCount; i++)
		{
			pNames->Add(GetAt(i));
		}
   }
}

// Block copy from outside ID array.
void CAddressBookNameList::PutNames(const DWORD* pNames, int nCount)
{
   TRY
   {
      SetSize(nCount);
      if (nCount != 0)
      {
         memcpy(&ElementAt(0), pNames, nCount*sizeof(DWORD));
      }
   }
   END_TRY

   if (nCount > 0)
   {
      // We've changed!
      SetChanged();
   }
}


//////////////////////////////////////////////////////////////////////////
// CAddressBookTabControl

CAddressBookTabControl::CAddressBookTabControl(void)
{
}

CAddressBookTabControl::~CAddressBookTabControl(void)
{
}

BOOL CAddressBookTabControl::GetTabName(int nTab, CString& csName)
{
   BOOL fResult = FALSE;
                  
   TRY
   {
      csName.Empty();
   
      // Setup a structure for GetItem().
      char szTabName[128];
      TC_ITEM tci;
      tci.mask = TCIF_TEXT;
      tci.pszText = &(szTabName[0]);
      tci.cchTextMax = sizeof(szTabName)-1;
      
      // Get information of the current tab.
      if (GetItem(nTab, &tci))
      {
         csName = szTabName;
      }

      fResult = !csName.IsEmpty();
   }
   END_TRY

   return fResult;   
}

BOOL CAddressBookTabControl::DeleteAllTabs(void)
{
   return DeleteAllItems();
}

BEGIN_MESSAGE_MAP(CAddressBookTabControl, TAB_CONTROL_CLASS)
   //{{AFX_MSG_MAP(CAddressBookTabControl)
   ON_WM_KEYDOWN()
   ON_WM_CHAR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAddressBookTabControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   switch (nChar)
   {
      case VK_UP:
      case VK_DOWN:
      {
         Util::SendWmCommand(GetParent(), (nChar == VK_UP) ? IDC_BACK : IDC_NEXT, NULL, 1);
         break;
      }
      default:
      {
         INHERITED::OnKeyDown(nChar, nRepCnt, nFlags);
         break;
      }
   }
}

void CAddressBookTabControl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   int nCount = GetItemCount();
   if (nCount > 0)
   {
      int nStart = GetCurSel();
      int nCurrent = nStart+1;
      for (;;)
      {
         // Wrap around if necessary.
         if (nCurrent >= nCount)
         {
            nCurrent = 0;
         }

         // If we've tried all the tabs, it's time to quit.
         if (nCurrent == nStart)
         {
            break;
         }

         // Get name of the current tab.
         CString csTabName;
         TRY
         {
            if (GetTabName(nCurrent, csTabName))
            {
               // Check if the first character matches.
               if (toupper(csTabName[0]) == toupper(nChar))
               {
                  // Set the new selection. This does not send a notification.
                  SetCurSel(nCurrent);

                  // Notify our parent.
                  NMHDR nmhdr;
                  nmhdr.hwndFrom = GetSafeHwnd();
                  nmhdr.idFrom = GetDlgCtrlID();
                  nmhdr.code = TCN_SELCHANGE;
                  GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)(&nmhdr));
   
                  // Bail out without giving base class a shot at the character.
                  return;
               }
            }
         }
         END_TRY

         // No Match, move to the next tab.
         nCurrent++;
      }
   }

   INHERITED::OnChar(nChar, nRepCnt, nFlags);
}

//////////////////////////////////////////////////////////////////////////
// CAddressBookData
// Class used to communicate with address book. Encapsulates input and
// output data.

CAddressBookData::CAddressBookData()
{
   SetSelectReceivers(FALSE);
   ClearChanged();
}

CAddressBookData::CAddressBookData(NameListRecord* pNameListRecord, BOOL fSelectReceivers /*=FALSE*/, BOOL fDisableReceivers /*=FALSE*/)
{
   SetAddressBookFullName(pNameListRecord->AddressBookName());
   SetSortIndexName(pNameListRecord->SortField());
   SetReceiverList(pNameListRecord);
   SetSelectReceivers(fSelectReceivers);
   SetDisableReceivers(fDisableReceivers);
   ClearChanged();
}

CAddressBookData::CAddressBookData(LPCSTR pszAddressBookFullName, LPCSTR pszSortIndexName, DWORD dwReceiverCount, DWORD* pdwReceivers, BOOL fSelectReceivers /*=FALSE*/, BOOL fDisableReceivers /*=FALSE*/)
{
   SetAddressBookFullName(pszAddressBookFullName);
   SetSortIndexName(pszSortIndexName);
   SetReceiverList(dwReceiverCount, pdwReceivers);
   SetSelectReceivers(fSelectReceivers);
   SetDisableReceivers(fDisableReceivers);
   ClearChanged();
}

CAddressBookData::~CAddressBookData(void)
{
}

void CAddressBookData::SetAddressBookFullName(LPCSTR pszAddressBookFullName)
{
   if (pszAddressBookFullName == NULL)
   {
      if (!m_csAddressBookFullName.IsEmpty())
      {
         TRY
         {
            m_csAddressBookFullName = pszAddressBookFullName;
            SetAddressBookFullNameChanged();
         }
         END_TRY
      }
   }
   else
   {
      if (m_csAddressBookFullName != pszAddressBookFullName)
      {
         TRY
         {
            m_csAddressBookFullName = pszAddressBookFullName;
            SetAddressBookFullNameChanged();
         }
         END_TRY
      }
   }
}

void CAddressBookData::SetSortIndexName(LPCSTR pszSortIndexName)
{
   if (pszSortIndexName == NULL)
   {
      if (!m_csSortIndexName.IsEmpty())
      {
         TRY
         {
            m_csSortIndexName = pszSortIndexName;
            SetSortIndexNameChanged();
         }
         END_TRY
      }
   }
   else
   {
      if (m_csSortIndexName != pszSortIndexName)
      {
         TRY
         {
            m_csSortIndexName = pszSortIndexName;
            SetSortIndexNameChanged();
         }
         END_TRY
      }
   }
}

void CAddressBookData::SetReceiverList(NameListRecord* pNameListRecord)
{
   // Delete any existing list.
   m_ReceiverList.RemoveAll();

   // The source may be unsorted, so, for now, we just insert each entry.
   DWORD dwReceiverCount = pNameListRecord->Names();
   for (DWORD dwReceiver = 0; dwReceiver < dwReceiverCount; dwReceiver++)
   {
      m_ReceiverList.AddName((DWORD)pNameListRecord->Name((int)dwReceiver));
   }
}

void CAddressBookData::SetReceiverList(DWORD dwReceiverCount, DWORD* pdwReceivers)
{
   // Delete any existing list.
   m_ReceiverList.RemoveAll();

   // The source may be unsorted, so, for now, we just insert each entry.
   if ((dwReceiverCount > 0) && (pdwReceivers != NULL))
   {
      for (DWORD dwReceiver = 0; dwReceiver < dwReceiverCount; dwReceiver++)
      {
         m_ReceiverList.AddName(*(pdwReceivers++));
      }
   }
}

void CAddressBookData::ClearChanged(void)
{
   SetDatabaseChanged(FALSE);
   SetAddressBookFullNameChanged(FALSE);
   SetSortIndexNameChanged(FALSE);
   SetReceiverListChanged(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBook

CAddressBook::CAddressBook(CWnd* pParent, CAddressBookData* pData, COLORREF cr)
	: CPmwDialogColor(pData->GetSelectReceivers() ? IDD_MERGE_NAMES : CAddressBook::IDD, pParent, cr)
{
   //{{AFX_DATA_INIT(CAddressBook)
   //}}AFX_DATA_INIT

	m_fReceiverList = pData->GetSelectReceivers();

	m_pParent = pParent;
   m_pData = pData;

   m_hAccelerators = NULL;
   m_nModalResult = -1;

   m_pDatabaseFile = NULL;
   m_pDatabase = NULL;
   m_pDatabaseCursor = NULL;

   m_csEmptySortMenu.Empty();
   m_csNameNumberFormat.Empty();

   m_fSizesValid = FALSE;

   m_uView = ID_VIEW_ONEATATIME;

   m_pDatabaseTabs = NULL;

   m_fAddingNewRecord = FALSE;

//   m_fFirstPaint = FALSE;

	m_nCurrentSortIndex = -1;
}

CAddressBook::~CAddressBook()
{
   delete m_pDatabaseCursor;
   delete m_pDatabase;
   delete m_pDatabaseFile;
   delete m_pDatabaseTabs;
}

void CAddressBook::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAddressBook)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddressBook, CPmwDialogColor)
   //{{AFX_MSG_MAP(CAddressBook)
   ON_BN_CLICKED(IDC_ADD, OnAdd)
   ON_BN_CLICKED(IDC_ADD_WITHOUT_EDIT, OnAddWithoutEdit)
   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_BN_CLICKED(IDC_DELETE_ALL, OnDeleteAll)
   ON_BN_CLICKED(IDC_SAVE, OnSave)
   ON_BN_CLICKED(IDC_UNDO, OnUndo)
   ON_BN_CLICKED(IDC_ALL, OnAll)
   ON_BN_CLICKED(IDC_NONE, OnNone)
   ON_BN_CLICKED(IDC_FIRST, OnFirst)
   ON_BN_CLICKED(IDC_BACK, OnBack)
   ON_BN_CLICKED(IDC_NEXT, OnNext)
   ON_BN_CLICKED(IDC_LAST, OnLast)
   ON_BN_CLICKED(IDC_BACK_PAGE, OnBackPage)
   ON_BN_CLICKED(IDC_NEXT_PAGE, OnNextPage)
   ON_COMMAND(ID_FILE_NEW_ADDRESS_BOOK, OnFileNew)
   ON_COMMAND(ID_FILE_OPEN_ADDRESS_BOOK, OnFileOpen)
   ON_COMMAND(ID_FILE_DELETE_ADDRESS_BOOK, OnFileDelete)
   ON_COMMAND(ID_FILE_CLOSE_ADDRESS_BOOK, OnFileClose)
   ON_WM_INITMENUPOPUP()
   ON_UPDATE_COMMAND_UI(IDC_FIRST, OnUpdateFirst)
   ON_UPDATE_COMMAND_UI(IDC_BACK, OnUpdateBack)
   ON_UPDATE_COMMAND_UI(IDC_NEXT, OnUpdateNext)
   ON_UPDATE_COMMAND_UI(IDC_LAST, OnUpdateLast)
   ON_UPDATE_COMMAND_UI(IDC_ADD, OnUpdateAdd)
   ON_UPDATE_COMMAND_UI(IDC_DELETE, OnUpdateDelete)
   ON_UPDATE_COMMAND_UI(IDC_DELETE_ALL, OnUpdateDeleteAll)
   ON_UPDATE_COMMAND_UI(IDC_SAVE, OnUpdateSave)
   ON_UPDATE_COMMAND_UI(IDC_UNDO, OnUpdateUndo)
   ON_UPDATE_COMMAND_UI(IDC_ALL, OnUpdateAll)
   ON_UPDATE_COMMAND_UI(IDC_NONE, OnUpdateNone)
   ON_UPDATE_COMMAND_UI(IDC_NAME_NUMBER, OnUpdateNameNumber)
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_COMMAND(ID_VIEW_ONEATATIME, OnViewOneatatime)
   ON_COMMAND(ID_VIEW_OVERVIEW, OnViewOverview)
   ON_COMMAND(ID_VIEW_RECEIVERLIST, OnViewReceiverlist)
   ON_COMMAND(ID_FILE_IMPORT_NAMES, OnFileImportnames)
   ON_COMMAND(ID_FILE_EXPORT_NAMES, OnFileExportnames)
   ON_LBN_SETFOCUS(IDC_ADDRESS_BOOK_FIELD_LIST, OnSetfocusAddressBookFieldList)
   ON_WM_ERASEBKGND()
//   ON_WM_PAINT()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
   ON_NOTIFY(TCN_SELCHANGE, IDC_ADDRESS_BOOK_TABS, OnAddressBookTabChange)
   ON_MESSAGE(WM_EDITOR_ACTIVATED, OnEditorActivated)
   ON_MESSAGE(WM_GET_FIELD_LIST, OnGetFieldList)
   ON_MESSAGE(WM_GET_DATABASE_TABS, OnGetDatabaseTabs)
   ON_MESSAGE(WM_SELECT_RECORD, OnSelectRecord)
   ON_MESSAGE(WM_GET_RECEIVER_LIST, OnGetReceiverList)
//   ON_MESSAGE(WM_HELP_USER, OnHelpUser)
END_MESSAGE_MAP()


BOOL CAddressBook::OnCommand(WPARAM wParam, LPARAM lParam)
{
   WORD wCommand = LOWORD(wParam);
   if ((wCommand >= IDC_SELECT_INDEX_MIN) && (wCommand <= IDC_SELECT_INDEX_MAX))
   {
      OnSelectIndex(wCommand);
      return TRUE;
   }
   
   return INHERITED::OnCommand(wParam, lParam);
}

// Run the address book as a modal dialog.
int CAddressBook::DoModal()
{
   if (Create(m_lpszTemplateName, CWnd::GetSafeOwner(m_pParent, NULL)))
   {
      m_hAccelerators = LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(m_fReceiverList ? IDR_MERGE_NAMES : IDR_ADDRESS_BOOK));

		CWnd* pParent = GetParent();
		if (pParent != NULL)
		{
			pParent->EnableWindow(FALSE);
		}

      EnableWindow(TRUE);

		if (!m_fReceiverList)
			GET_PMWAPP()->ShowHelpfulHint(PLACE_HINT, PLACE_AddressBook, this);

		BOOL fContinue = TRUE;
      while (fContinue)
      {
         MSG Msg;
         while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
         {
            if (!::GetMessage(&Msg, NULL, NULL, NULL))
            {
               fContinue = FALSE;
               ::PostQuitMessage(0);
               break;
            }

            // Send a cancel mode out to the currently focused control. This gives our
            // list box editors a chance to pull up their list boxes.
            if ((Msg.message == WM_LBUTTONDOWN)
             || (Msg.message == WM_NCLBUTTONDOWN))
            {
               CWnd* pFocus = GetFocus();
               if (pFocus != NULL)
               {
                  // only send the message if the window receiving the click is
                  // not the combo button or list box.
                  char szClassName[64];
                  szClassName[0] = '\0';
                  if (Msg.hwnd != NULL)
                  {
                     ::GetClassName(Msg.hwnd, szClassName, sizeof(szClassName)-1);
                  }
                  if ((stricmp(szClassName, "MLSComboButton") != 0)
                   && (stricmp(szClassName, "MLSComboList") != 0))
                  {
                     pFocus->SendMessage(WM_CANCELMODE, 0, 0);
                  }
               }
            }

            // Note that fContinue may be set to FALSE, but we continue to process message
            // until we run out and then we return from DoModal.
            fContinue = GetSafeHwnd() != NULL;
            
            if (!(fContinue && (m_hAccelerators != NULL) && (!CurrentViewIsEditing()) && (TranslateAccelerator(GetSafeHwnd(), m_hAccelerators, &Msg))))
            {
               if (!(fContinue && IsDialogMessage(&Msg)))
               {
                  ::TranslateMessage(&Msg);
                  ::DispatchMessage(&Msg);
               }
            }
         }

         fContinue = fContinue && (GetSafeHwnd() != NULL);

         if (fContinue)
         {
            // send WM_ENTERIDLE since queue is empty
            if ((GetParent() != NULL) && ((GetStyle() & DS_NOIDLEMSG) == 0))
            {
               GetParent()->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)m_hWnd);
            }
         }
      }
   }

   return m_nModalResult;
}

// Save position and other information before window is destroyed.
BOOL CAddressBook::DestroyWindow()
{
   // Save address book window information.
   SaveInformation();

   // re-enable parent if it was disabled
	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		pParent->EnableWindow(TRUE);
	}

   // hide this window and move activation to the parent
   SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

	if (pParent != NULL)
	{
		pParent->SetActiveWindow();
	}

   // finally, destroy this window
   return INHERITED::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBook message handlers

BOOL CAddressBook::OnInitDialog() 
{
   INHERITED::OnInitDialog();

//   m_fFirstPaint = TRUE;

	m_FirstButton.Initialize(IDC_FIRST, this);
	m_BackButton.Initialize(IDC_BACK, this);
	m_NextButton.Initialize(IDC_NEXT, this);
	m_LastButton.Initialize(IDC_LAST, this);

	m_NameNumber.Initialize(IDC_NAME_NUMBER, this);
   
   // Initialize the one-at-a-time list box.
   m_FieldList.SubclassDlgItem(IDC_ADDRESS_BOOK_FIELD_LIST, this);
   m_FieldList.ShowWindow(SW_HIDE);

   // Add the proper border to the one-at-a-time list box.
   m_FieldList.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

   // Get the list box dimensions. All three list boxes have the same size.
   CRect crList;
   m_FieldList.GetWindowRect(crList);
   ScreenToClient(crList);

   // Create the overview list box.
   m_OverviewList.CreateEx(
      WS_EX_CLIENTEDGE,
      AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW),
      "Overview List",
      WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VSCROLL | WS_HSCROLL,
      crList.left,
      crList.top,
      crList.Width(),
      crList.Height(),
      GetSafeHwnd(),
      (HMENU)IDC_ADDRESS_BOOK_OVERVIEW_LIST,
      NULL);

   // Create the receiver list box with the same dimensions.
   m_ReceiverList.CreateEx(
      WS_EX_CLIENTEDGE,
      AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW),
      "Receiver List",
      WS_CHILD | WS_TABSTOP | WS_GROUP | WS_VSCROLL,
      crList.left,
      crList.top,
      crList.Width(),
      crList.Height(),
      GetSafeHwnd(),
      (HMENU)IDC_ADDRESS_BOOK_RECEIVER_LIST,
      NULL);

	CWnd* pInitialFocus = GetDlgItem(m_fReceiverList ? IDCANCEL : IDC_ADD);
	if (pInitialFocus != NULL)
	{
		GotoDlgCtrl(pInitialFocus);
	}

   // Get the string to use for an empty sort menu.
   CMenu* pMenu = FindMenuByName(IDS_ADDRESS_BOOK_SORT_MENU);
   if (pMenu != NULL)
   {
      TRY
      {
         Util::GetMenuString(pMenu, IDC_EMPTY_SORT_MENU, m_csEmptySortMenu, MF_BYCOMMAND);
      }
      END_TRY
   }

   // Get the string to use as the name number format text.
   TRY
   {
      m_NameNumber.GetWindowText(m_csNameNumberFormat);
      m_NameNumber.SetWindowText("");
   }
   END_TRY

   CWnd* pNotebookArea = GetDlgItem(IDC_NOTEBOOK_AREA);
   if (pNotebookArea != NULL)
   {
      CRect crTabs;
      pNotebookArea->GetWindowRect(crTabs);
      ScreenToClient(crTabs);

      m_Tabs.Create(
         TCS_FOCUSONBUTTONDOWN | TCS_FIXEDWIDTH | WS_VISIBLE | WS_CHILD | WS_TABSTOP,
         crTabs,
         this,
         IDC_ADDRESS_BOOK_TABS);

      m_Tabs.SetWindowPos(pNotebookArea, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

      pNotebookArea->DestroyWindow();

      CFont* pFont = GetFont();
      if (pFont != NULL)
      {
         m_Tabs.SetFont(pFont);
      }
   }


#if 0
   // Turn on "clip siblings" for all children.
   for (CWnd* pChild = GetWindow(GW_CHILD); pChild != NULL; pChild = pChild->GetWindow(GW_HWNDNEXT))
   {
      DWORD dwStyle = pChild->GetStyle();
      ::SetWindowLong(pChild->GetSafeHwnd(), GWL_STYLE, dwStyle | WS_CLIPSIBLINGS);
   }
#endif

   // Open the requested address book.
   if (!SetDatabase(m_pData->GetAddressBookFullName()))
   {
      // We cannot open the initial database.
      CString csFormat;
      CString csMessage;
      CString csUserName;
      TRY
      {
         GetAddressBookUserName(m_pData->GetAddressBookFullName(), csUserName);
         csFormat.LoadString(IDS_ADDRESS_BOOK_NOT_AVAILABLE);
         csMessage.Format(csFormat, csUserName);
         AfxMessageBox(csMessage);
      }
      END_TRY

      // Cancel ourselves.
      Util::PostWmCommand(this, IDCANCEL, NULL, 0);
      return FALSE;
   }

   // Compute some layout information that will be used for sizing.
   CRect crDialogWindow;
   GetWindowRect(crDialogWindow);

   m_nMinimumWidth = crDialogWindow.Width();
   m_nMinimumHeight = crDialogWindow.Height();

   CRect crDialogClient;
   GetClientRect(crDialogClient);

   m_nCurrentWidth = crDialogClient.Width();
   m_nCurrentHeight = crDialogClient.Height();

   m_fSizesValid = TRUE;

   // Restore any saved information including the saved position.
   RestoreInformation();
   
   return FALSE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// Save all the information we might want to restore for the next addres book session.
BOOL CAddressBook::SaveInformation(void)
{
   BOOL fResult = FALSE;

   CIniFile IniFile;

   TRY
   {
      // Get the INI file to use.
		IniFile.Name(GET_PMWAPP()->m_pszProfileName);
//    if (GET_PMWAPP()->InitPrintMasterIniFile(&IniFile, "_000PMW.INI", TRUE))
      {
			LPCSTR pszSection = m_fReceiverList ? "MergeNamesDialog" : "AddressBookDialog";

         // Write out the position information.
         IniFile.WriteWindowPosition(pszSection, this);

         // Write out the current view.
         if ((m_uView != ID_VIEW_RECEIVERLIST) || (!m_pData->GetSelectReceivers()))
         {
            IniFile.WriteUnsignedInteger(pszSection, "View", m_uView);
         }

         // Write out the splitter information
         if (SaveSplitterInformation(IniFile))
         {
            fResult = TRUE;
         }
      }
   }
   END_TRY

   return fResult;
}

// Load saved address book information.
BOOL CAddressBook::RestoreInformation(void)
{
   BOOL fResult = FALSE;

   CIniFile IniFile;

   TRY
   {
      // Get the INI file to use.
		IniFile.Name(GET_PMWAPP()->m_pszProfileName);
//    if (GET_PMWAPP()->InitPrintMasterIniFile(&IniFile, "_000PMW.INI", TRUE))
      {
			LPCSTR pszSection = m_fReceiverList ? "MergeNamesDialog" : "AddressBookDialog";
			UINT uView = ID_VIEW_ONEATATIME;

         // Restore the position information.
			if (m_fReceiverList)
			{
	         IniFile.GetWindowPosition(pszSection, this);
            uView = ID_VIEW_RECEIVERLIST;
			}
			else
			{
		      IniFile.GetWindowPosition(pszSection, this);

				// Restore the current view.
				if (m_pData->GetSelectReceivers() && !m_pData->GetDisableReceivers())
				{
					// Caller is forcing selected receivers.
					uView = ID_VIEW_RECEIVERLIST;
				}
				else
				{
					uView = IniFile.GetUnsignedInteger(pszSection, "View", ID_VIEW_ONEATATIME);
					if ((uView != ID_VIEW_ONEATATIME)
					 && (uView != ID_VIEW_OVERVIEW)
					 && (uView != ID_VIEW_RECEIVERLIST))
					{
						// Unknown view.
						uView = ID_VIEW_ONEATATIME;
					}

					if (m_pData->GetDisableReceivers() && (uView == ID_VIEW_RECEIVERLIST))
					{
						// Receiver list is disabled.
						uView = ID_VIEW_ONEATATIME;
					}
				}
			}

			SetView(uView);

			// Restore the splitter information.
			if (RestoreSplitterInformation(IniFile))
			{
				fResult = TRUE;
			}
      }
   }
   END_TRY

   return fResult;
}

// Save splitter position information.
BOOL CAddressBook::SaveSplitterInformation(CIniFile& IniFile)
{
   // Convert splitter to relative size.
   int nSplitter = m_FieldList.GetSplitter();
   CRect crFieldList;
   m_FieldList.GetWindowRect(crFieldList);
   int nWidth = crFieldList.Width();
   CFixed lFraction = SplitterToFraction(nSplitter, nWidth);
	LPCSTR pszSection = m_fReceiverList ? "MergeNamesDialog" : "AddressBookDialog";
   IniFile.WriteLong(pszSection, "FieldListSplitterFraction", (long)lFraction);

   return TRUE;
}

// Load saved splitter information.
BOOL CAddressBook::RestoreSplitterInformation(CIniFile& IniFile)
{
	LPCSTR pszSection = m_fReceiverList ? "MergeNamesDialog" : "AddressBookDialog";

   // Convert relative size to splitter.
   CFixed lFraction = (CFixed)(IniFile.GetLong(pszSection, "FieldListSplitterFraction", UNDEFINED_CFIXED));
   if (lFraction != UNDEFINED_CFIXED)
   {
      CRect crFieldList;
      m_FieldList.GetWindowRect(crFieldList);
      int nWidth = crFieldList.Width();
      int nSplitter = FractionToSplitter(lFraction, nWidth);
      m_FieldList.SetSplitter(nSplitter);
   }

   return TRUE;
}

// Register any special classes which are used by the address book.
void CAddressBook::RegisterAddressBookClasses(void)
{
   WNDCLASS WndClass;

   // Create a special combo button class based on Button.
   if (GetClassInfo(NULL, "button", &WndClass))
   {
      // Same as button except no double clicks, not a global class, uses DefWindowProc(),
      // and uses our class name.
      WndClass.style &= ~(CS_DBLCLKS|CS_GLOBALCLASS);
      WndClass.lpfnWndProc = ::DefWindowProc;
      WndClass.lpszClassName = &(szComboButtonClassName[0]);
      WndClass.hInstance = AfxGetInstanceHandle();

      // Register button class.
      VERIFY(RegisterClass(&WndClass));
   }

   // Create a special combo list box class based on ListBox.
   if (GetClassInfo(NULL, "listbox", &WndClass))
   {
      // Same as list box except no double clicks, not a global class, saves
      // its background bits, and uses our class name.
      WndClass.style &= ~(CS_DBLCLKS|CS_GLOBALCLASS);
      WndClass.style |= CS_SAVEBITS;
      WndClass.lpszClassName = &(szComboListClassName[0]);
      WndClass.hInstance = AfxGetInstanceHandle();

      // Register list box class.
      VERIFY(RegisterClass(&WndClass));
   }
}

// Unregister any special classes which are used by the address book.
void CAddressBook::UnregisterAddressBookClasses(void)
{
   UnregisterClass(&(szComboButtonClassName[0]), AfxGetInstanceHandle());
   UnregisterClass(&(szComboListClassName[0]), AfxGetInstanceHandle());
}

BOOL CAddressBook::OnEraseBkgnd(CDC* pDC)
{
   BOOL fResult = FALSE;
   
   // Exclude all the child windows for cleaner redraw.
   // Setting WS_CLIPCHILDREN should do this, but it
   // doesn't work right under NT 4.0.
   if (pDC->SaveDC() != 0)
   {
      for (CWnd* pChild = GetWindow(GW_CHILD); pChild != NULL; pChild = pChild->GetWindow(GW_HWNDNEXT))
      {
         CRect crChild;
         pChild->GetWindowRect(crChild);
         ScreenToClient(crChild);
         pDC->ExcludeClipRect(crChild);
      }

      fResult = INHERITED::OnEraseBkgnd(pDC);

      pDC->RestoreDC(-1);
   }
   
   return fResult;
}

// The WM_INITMENUPOPUP message is set before a popup menu is shown. This code runs
// through and enables/disables menu items based on what's reported by the CmdUI
// handlers.

void CAddressBook::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
   AfxCancelModes(m_hWnd);

   if (bSysMenu)
   {
      return;     // don't support system menu
   }

   ASSERT(pMenu != NULL);
   // check the enabled state of various menu items
   CCmdUI state;
   state.m_pMenu = pMenu;
   ASSERT(state.m_pOther == NULL);
   ASSERT(state.m_pParentMenu == NULL);
   // determine if menu is popup in top-level menu and set m_pOther to
   //  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
   CMenu* pOurMenu = GetMenu();
   ASSERT(pOurMenu != NULL);
   if (pOurMenu != NULL)
   {
      int nIndexMax = pOurMenu->GetMenuItemCount();
      for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
      {
         if (pOurMenu->GetSubMenu(nIndex) == pMenu)
         {
            // when popup is found, m_pParentMenu is containing menu
            state.m_pParentMenu = pOurMenu;
            break;
         }
      }
   }

   state.m_nIndexMax = pMenu->GetMenuItemCount();
   for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
   {
      state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
      if (state.m_nID == 0)
      {
         continue; // menu separator or invalid cmd - ignore it
      }

      ASSERT(state.m_pOther == NULL);
      ASSERT(state.m_pMenu != NULL);
      if (state.m_nID == (UINT)-1)
      {
         // possibly a popup menu, route to first item of that popup
         state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
         if ((state.m_pSubMenu == NULL)
          || ((state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0)
          || (state.m_nID == (UINT)-1))
         {
            continue;       /* first item of popup can't be routed to */
         }
         state.DoUpdate(this, FALSE);    // popups are never auto disabled
      }
      else
      {
         // normal menu item -- auto enable/disable if command is _not_ a system command
         state.m_pSubMenu = NULL;

         BOOL bDisableIfNoHandler = (state.m_nID < 0xF000);
         if (bDisableIfNoHandler)
         {
            bDisableIfNoHandler = !((state.m_nID >= IDC_SELECT_INDEX_MIN) && (state.m_nID <= IDC_SELECT_INDEX_MAX));
         }
         state.DoUpdate(this, bDisableIfNoHandler);
         // Disable print menu item if the database is empty
         if (state.m_nID == ID_FILE_PRINT)
         {
            DWORD RecCount = 0;
            if (m_pDatabase != NULL)
               m_pDatabase->GetRecordCount(0, &RecCount);
            if (RecCount == 0)
               state.Enable(FALSE);
         }
      }
      // adjust for menu deletions and additions
      UINT nCount = pMenu->GetMenuItemCount();
      if (nCount < state.m_nIndexMax)
      {
         state.m_nIndex -= (state.m_nIndexMax - nCount);
         while (state.m_nIndex < nCount &&
            pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
         {
            state.m_nIndex++;
         }
      }
      state.m_nIndexMax = nCount;
   }
}

// The following CmdUI functions are to enable and diables various user interface
// elements as needed.
LRESULT CAddressBook::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
   // PrintMaster sends this message to us (MAINFRM.CPP) so we know
   // when to update all our controls.
   UpdateDialogControls(this, wParam);
   return 0L;
}
// Helper function for updating a control.
void CAddressBook::UpdateControl(CCmdUI* pCmdUI, BOOL bEnable)
{
   CWnd* pControl = GetDlgItem(pCmdUI->m_nID);
   if (pControl->GetSafeHwnd() != NULL)
   {
      if (pControl->IsWindowVisible())
      {
         if ((!bEnable) && (GetFocus() == pControl))
         {
            NextDlgCtrl();
         }
         pCmdUI->Enable(bEnable);
      }
   }
}
// Update the "First" button.
void CAddressBook::OnUpdateFirst(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, (!m_fAddingNewRecord) && (m_pDatabaseCursor != NULL) && (!m_pDatabaseCursor->AtBOF()));
}
// Update the "Back" button.
void CAddressBook::OnUpdateBack(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, (!m_fAddingNewRecord) && (m_pDatabaseCursor != NULL) && (!m_pDatabaseCursor->AtBOF()));
}
// Update the "Next" button.
void CAddressBook::OnUpdateNext(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, (!m_fAddingNewRecord) && (m_pDatabaseCursor != NULL) && (!m_pDatabaseCursor->AtEOF()) && (m_pDatabaseCursor->GetPosition() < m_pDatabaseCursor->GetRecordCount()-1));
}
// Update the "Last" button.
void CAddressBook::OnUpdateLast(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, (!m_fAddingNewRecord) && (m_pDatabaseCursor != NULL) && (!m_pDatabaseCursor->AtEOF()) && (m_pDatabaseCursor->GetPosition() < m_pDatabaseCursor->GetRecordCount()-1));
}
// Update the "Add" button.
void CAddressBook::OnUpdateAdd(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, CanAdd());
}
// Update the "Delete" button.
void CAddressBook::OnUpdateDelete(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, CanDelete());
}
// Update the "Delete" menu item.
void CAddressBook::OnUpdateDeleteAll(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(CanDeleteAll());
}
// Update the "Save" button.
void CAddressBook::OnUpdateSave(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, CanSave());
}
// Update the "Undo" button.
void CAddressBook::OnUpdateUndo(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, CanUndo());
}
// Update the "All" button.
void CAddressBook::OnUpdateAll(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, CanAll());
}
// Update the "None" button.
void CAddressBook::OnUpdateNone(CCmdUI* pCmdUI)
{
   UpdateControl(pCmdUI, CanNone());
}
// Refresh the current name number.
void CAddressBook::OnUpdateNameNumber(CCmdUI* pCmdUI)
{
   RefreshNameNumber();
}
// Check if we can add a new record.
BOOL CAddressBook::CanAdd(void)
{
   return m_pDatabaseCursor != NULL;
}
// Check if we can delete the current record.
BOOL CAddressBook::CanDelete(void)
{
   return CurrentViewCanDelete();
}
// Check if we can delete the all the names in the address book.
BOOL CAddressBook::CanDeleteAll(void)
{
   return (m_pDatabaseCursor != NULL) && (m_pDatabaseCursor->GetRecordCount() > 0);
}
// Check if we have changes to the current record which can be saved.
BOOL CAddressBook::CanSave(void)
{
   return CurrentViewIsModified();
}
// Check if we have changes to the current record which can be canceled/undone.
BOOL CAddressBook::CanUndo(void)
{
   return CurrentViewCanUndo();
}
// Check if we can select all receivers.
BOOL CAddressBook::CanAll(void)
{
   return CurrentViewCanAll();
}
// Check if we can select no receivers.
BOOL CAddressBook::CanNone(void)
{
   return CurrentViewCanNone();
}
// The user clicked the "Add" button to add a new record to the address book.
void CAddressBook::OnAdd() 
{
   BOOL fCanAdd = CanAdd();
   if (fCanAdd)
   {
      // Check if we are at the maximum number of records.
      if (m_pDatabaseCursor != NULL)
      {
         if (m_pDatabaseCursor->GetRecordCount() >= MAXIMUM_NAMES)
         {
            CString csFormat;
            CString csText;
            TRY
            {
               csFormat.LoadString(IDS_ADDRESS_BOOK_TOO_MANY_NAMES);
               csText.Format(csFormat, CAddressBook::MAXIMUM_NAMES);
               AfxMessageBox(csText);
            }
            END_TRY
            fCanAdd = FALSE;
         }
      }

      if (fCanAdd)
      {
         // If we're in the receiver view, switch back to one-at-a-time.
         if (m_uView == ID_VIEW_RECEIVERLIST)
         {
            SetView(ID_VIEW_ONEATATIME);
         }

         // Do most of the work.
         OnAddWithoutEdit();

         // As a courtesy to the user, start editing in the first field.
         CurrentViewStartNewEdit();
      }
   }
}
// A variation on "Add" which does not automatically start editing.
void CAddressBook::OnAddWithoutEdit() 
{
   BOOL fCanAdd = CanAdd();
   if (fCanAdd)
   {
      // Check if we are at the maximum number of records.
      if (m_pDatabaseCursor != NULL)
      {
         if (m_pDatabaseCursor->GetRecordCount() >= MAXIMUM_NAMES)
         {
            CString csFormat;
            CString csText;
            TRY
            {
               csFormat.LoadString(IDS_ADDRESS_BOOK_TOO_MANY_NAMES);
               csText.Format(csFormat, CAddressBook::MAXIMUM_NAMES);
               AfxMessageBox(csText);
            }
            END_TRY
            fCanAdd = FALSE;
         }
      }

      if (fCanAdd)
      {
         BeforeRecordChange();

         // Initialize the fields to receive the new data.
         m_pDatabaseCursor->New();

         // Set "adding" flag. Note that it must be set twice because AfterRecordChange() clears it.
         m_fAddingNewRecord = TRUE;
         AfterRecordChange(FALSE, FALSE);
         m_fAddingNewRecord = TRUE;
      }
   }
}
// The user clicked the "Delete" button to delete the current record from the address book.
void CAddressBook::OnDelete() 
{
   if (CanDelete())
   {
      // Make sure the user REALLY wants to do this since it cannot be undone.
      if (ConfirmDelete())
      {
         // If this is a new record, then simply cancel the editing and return the
         // user to the first record in the database.
         if (m_fAddingNewRecord)
         {
            CurrentViewAbortNewEdit();
         }
         else
         {
            // Make sure the record is out of the receiver list.
            CFlatFileDatabaseField* pField = m_pDatabaseCursor->GetField("__ID__");
            if (pField != NULL)
            {
               m_pData->GetReceiverList().DeleteName(pField->GetAsUnsigned32());
            }

            // Database is changing.
            m_pData->SetDatabaseChanged();

            // Ok, delete the record!
            m_pDatabaseCursor->Delete(TRUE);

            // Deleting a records invalidates all of the cached lists.
            m_ListManager.FieldModified();

            // Update interface.
				if (m_pDatabaseTabs != NULL)
				{
					m_pDatabaseTabs->UpdateTabs();
				}

            // If the database is now empty, do any special view processing.
            if (m_pDatabaseCursor->GetRecordCount() == 0)
            {
               CurrentViewDatabaseEmpty();
            }
            else
            {
               AfterRecordChange(FALSE, FALSE);
            }
         }
      }
   }
}
// Ask the user if they REALLY want to delete. This question could get annoying.
// Perhaps there should be a "never ask me again" check box?
BOOL CAddressBook::ConfirmDelete(void)
{
   CConfirmDeleteNameDialog Dialog(this);
   TRY
   {
      GetDisplayName(m_pDatabaseCursor, Dialog.m_csName);
   }
   END_TRY

   return Dialog.DoModal() == IDOK;
}
// The user selected the "Delete All Names" menu item to delete the all the names the address book.
void CAddressBook::OnDeleteAll()
{
   if (CanDeleteAll())
   {
      // Make sure the user REALLY wants to do this since it cannot be undone.
      if (ConfirmDeleteAll())
      {
         // Abort any editing.
         if (m_fAddingNewRecord)
         {
            CurrentViewAbortNewEdit();
         }

         // Database is changing.
         m_pData->SetDatabaseChanged();

         // Ok, delete all the records!
         m_pDatabaseCursor->DeleteAll();

         // Deleting all records invalidates all of the cached lists.
         m_ListManager.FieldModified();

         // Delete all the receivers.
         m_pData->GetReceiverList().DeleteAllNames();

         // Do any view specific actions when database is emptied.
         CurrentViewDatabaseEmpty();
      }
   }
}
// Ask the user if they REALLY want to delete all the names.
BOOL CAddressBook::ConfirmDeleteAll(void)
{
   CConfirmDeleteAllNamesDialog Dialog(this);
   return Dialog.DoModal() == IDOK;
}
// The user wants to commit the changes to the current record.
void CAddressBook::OnSave() 
{
   if (CanSave())
   {
      TRY
      {
         // Save any changes.
         BeforeRecordChange();
 
         // Update interface.
         AfterRecordChange(FALSE, FALSE);
      }
      END_TRY
   }
}
// Undo/cancel/abandon any changes made to the current record.
void CAddressBook::OnUndo() 
{
   if (CanUndo())
   {
      // If we are adding a new record, then we "undo" by cancelling the
      // entire operation and returning the user to the first record.
      if (m_fAddingNewRecord)
      {
         CurrentViewAbortNewEdit();
      }
      else
      {
         // Bring back the old values!
         CurrentViewRefresh();
      }
   }
}
// Select all receivers.
void CAddressBook::OnAll() 
{
   if (CanAll())
   {
      if (m_pDatabase != NULL)
      {
         // Select all the records as receivers.
         CFlatFileDatabaseCursor Cursor;
         TRY
         {
            Cursor.Attach(m_pDatabase);
            Cursor.SetIndex((DWORD)0);
            CFlatFileDatabaseField* pField = Cursor.GetField("__ID__");
            Cursor.First();
            while (!Cursor.AtEOF())
            {
               m_pData->GetReceiverList().AddName(pField->GetAsUnsigned32());
               Cursor.Next();
            }
         }
         END_TRY
         m_ReceiverList.Invalidate();
      }
   }
}
// Select no receivers.
void CAddressBook::OnNone() 
{
   if (CanNone())
   {
      // Deselect all receivers.
      m_pData->GetReceiverList().DeleteAllNames();
      m_ReceiverList.Invalidate();
   }
}
// The user want to move to the first record.
void CAddressBook::OnFirst() 
{
   if (m_pDatabaseCursor != NULL)
   {
      TRY
      {
         BeforeRecordChange();
         m_pDatabaseCursor->First();
         AfterRecordChange(FALSE, FALSE);
      }
      END_TRY
   }
}
// The user want to move back a record.
void CAddressBook::OnBack() 
{
   if (m_pDatabaseCursor != NULL)
   {
      TRY
      {
         BeforeRecordChange();
         m_pDatabaseCursor->Previous();
         AfterRecordChange(FALSE, FALSE);
      }
      END_TRY
   }
}
// The user want to move to the next record.
void CAddressBook::OnNext() 
{
   if (m_pDatabaseCursor != NULL)
   {
      TRY
      {
         BeforeRecordChange();
         m_pDatabaseCursor->Next();
         AfterRecordChange(FALSE, FALSE);
      }
      END_TRY
   }
}
// The user want to move to the last record. Note that this is not
// "Last" as defined by the database cursor -- the position after the
// last record.
void CAddressBook::OnLast() 
{
   if (m_pDatabaseCursor != NULL)
   {
      TRY
      {
         BeforeRecordChange();
         m_pDatabaseCursor->SetPosition(m_pDatabaseCursor->GetRecordCount()-1);
         AfterRecordChange(FALSE, FALSE);
      }
      END_TRY
   }
}
// Move back a "page" of records. The definition of "page" depends on the current view.
void CAddressBook::OnBackPage() 
{
   CurrentViewBackPage();
}
// Move back a "page" of records. The definition of "page" depends on the current view.
void CAddressBook::OnNextPage() 
{
   CurrentViewNextPage();
}
// Create a new address book.
void CAddressBook::OnFileNew() 
{
   TRY
   {
      // Save any changes.
      BeforeRecordChange();
      AfterRecordChange(FALSE, FALSE);
   }
   END_TRY

   CloseDatabase();

   CNewAddressBookDialog Dialog(NULL, this);
   if (Dialog.DoModal() == IDOK)
   {
      CString csFullName;
      TRY
      {
         csFullName = Dialog.GetFullName();
         if (!csFullName.IsEmpty())
         {
            // Switch to new address book.
            SetDatabase(csFullName);
         }
      }
      END_TRY
   }
   else
   {
      // Return to previous name.
      SetDatabase(m_pData->GetAddressBookFullName());
   }
}
// Open an address book.
void CAddressBook::OnFileOpen() 
{
   TRY
   {
      // Save any changes.
      BeforeRecordChange();
      AfterRecordChange(FALSE, FALSE);
   }
   END_TRY

   COpenAddressBookDialog Dialog(this);
   if (Dialog.DoModal() == IDOK)
   {
      CString csFullName;
      TRY
      {
         csFullName = Dialog.GetFullName();
         if (!csFullName.IsEmpty())
         {
            // Switch to new address book.
            SetDatabase(csFullName);
         }
      }
      END_TRY
   }
}
// Delete an address book.
void CAddressBook::OnFileDelete() 
{
   TRY
   {
      // Save any changes.
      BeforeRecordChange();
      AfterRecordChange(FALSE, FALSE);
   }
   END_TRY

   CloseDatabase();

   CDeleteAddressBookDialog Dialog(this);
   Dialog.DoModal();

   SetDatabase(m_pData->GetAddressBookFullName());
}
// Close the address book.
void CAddressBook::OnFileClose() 
{
   // Pretend we pressed the close button.
   OnOK();
}

void CAddressBook::OnCancel()
{
	// Cancel is called from the address book dialog as the "Close" button
	// and from the merge names (aka receiver list) dialog as the "Cancel"
	// button.

	CloseAddressBook(IDCANCEL);
}

void CAddressBook::OnOK()
{
	// OK is called only from the merge names dialog.
	CloseAddressBook(IDOK);
}

void CAddressBook::CloseAddressBook(int nModalResult)
{
	// If we're selecting merge names and the user hits cancel,
	// then always return cancel.
	if (m_fReceiverList && (nModalResult == IDCANCEL))
	{
		m_nModalResult = IDCANCEL;
	}
	else
	{
		// We don't want to bail right away. We need to save any data
		// changes and then return IDOK if something changed, or IDCANCEL
		// if nothing is any different.
		TRY
		{
			// Save any changes.
			BeforeRecordChange();
			AfterRecordChange(FALSE, FALSE);
		}
		END_TRY

		// Remember the current sort index selection.
		CString csSortIndexName;
		TRY
		{
			if (m_nCurrentSortIndex != -1)
			{
				LPCSTR pszName;
				DWORD dwIndex;
				if (m_SortIndexList.GetAt(m_nCurrentSortIndex, pszName, dwIndex))
				{
					m_pData->SetSortIndexName(pszName);
				}
			}
		}
		END_TRY

		// Sort the receiver list according to the chosen index
		SortReceiverList();

		// Return IDOK if something was changed, otherwise return IDCANCEL.
		m_nModalResult = m_pData->GetChanged() ? IDOK : IDCANCEL;
	}

	DestroyWindow();
}

void CAddressBook::SortReceiverList()
{
	// Copy the receiver list away for sorting
	CAddressBookNameList& dwaReceivers = m_pData->GetReceiverList();
	CAddressBookNameList dwaCopiedReceiverList(dwaReceivers);

	// Clear the saved receiver list
	dwaReceivers.DeleteAllNames();

   // Search for the sorted receivers.
   if (m_pDatabaseCursor != NULL)
   {
      CFlatFileDatabaseField* pField = m_pDatabaseCursor->GetField("__ID__");
      //m_pDatabaseCursor->SetIndex((DWORD)0);
      m_pDatabaseCursor->First();
      while (!m_pDatabaseCursor->AtEOF())
      {
			DWORD dwID = pField->GetAsUnsigned32();
			if (dwID != 0)
			{
				if (dwaCopiedReceiverList.Find(dwID) != -1)
				{
					dwaReceivers.AddName(dwID);
				}
			}         
         m_pDatabaseCursor->Next();
      }
   }
}

// The user has click on one of the tabs, move to the first
// record associated with the selected tab.
void CAddressBook::OnAddressBookTabChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_pDatabaseTabs != NULL)
   {
	   if (m_Tabs.m_hWnd != NULL)
		{
			int nTabIndex = m_Tabs.GetCurSel();
			if (nTabIndex != -1)
			{
				BeforeRecordChange();
				m_pDatabaseTabs->UpdateCurrentRecord(nTabIndex);
				AfterRecordChange(FALSE, FALSE);
			}
		}
   }
}

BOOL CAddressBook::SetDatabase(CString csFullName)
{
   ERRORCODE Error = ERRORCODE_None;

   // Check if the database exists. If not, then we present the user with some options.
   if (!csFullName.IsEmpty())
   {
      if (!Util::FileExists(csFullName))
      {
         CString csUserName;
         TRY
         {
            GetAddressBookUserName(csFullName, csUserName);
            CAddressBookDoesNotExistDialog Dialog(csUserName, this);
            if (Dialog.DoModal() == IDOK)
            {
               csFullName = Dialog.GetFullName();
            }
            else
            {
               // The user has decided not to open the database.
               return FALSE;
            }
         }
         END_TRY
      }
   }

   // We are going to switch to something (or nothing)!

   // Shut down any existing database.
   CloseDatabase();

   // If we are switching address books, then reset some things that
   // are associated with the current address book.
   if (csFullName.CompareNoCase(m_pData->GetAddressBookFullName()) != 0)
   {
      m_pData->SetAddressBookFullName(csFullName);
      m_pData->SetSortIndexName(NULL);
      m_pData->GetReceiverList().DeleteAllNames();
   }

   // Attempt to open the new database.
   if (!csFullName.IsEmpty())
   {
      TRY
      {
         // Open the database.
         Error = OpenAddressBookDatabase(csFullName, m_pDatabaseFile, m_pDatabase, FALSE);
         if (Error == ERRORCODE_None)
         {
            // Initialize the list manager for the new database (if any).
            m_ListManager.SetDatabase(m_pDatabase);

            // Create a cursor on the database.
            m_pDatabaseCursor = new CFlatFileDatabaseCursor;
            m_pDatabaseCursor->Attach(m_pDatabase);

            // Record the field descriptions of important fields for this database.
            GetImportantFields();

            // Create a tabs object to connect to database to the tab control.
			   if (m_Tabs.m_hWnd != NULL)
				{
	            TRY
		         {
			         m_pDatabaseTabs = new CDatabaseTabs(m_pDatabaseCursor, &m_Tabs);
				   }
					END_TRY
				}

            // Set the index information.
            RefreshIndexInformation();
            RefreshFieldInformation();

            // Select the index. If the current data does not specify an index,
            // then the first index that's listed in the sort list box is used.
            DWORD dwIndex = (DWORD)-1;

            if (!m_pData->GetSortIndexName().IsEmpty())
            {
               // Try to get the index number of the current sort name.
               TRY
               {
                  dwIndex = m_pDatabaseCursor->GetIndexNumberFromName(m_pData->GetSortIndexName());
               }
               CATCH_ALL(e)
               {
                  if ((e->IsKindOf(RUNTIME_CLASS(CErrorcodeException)))
                   && (((CErrorcodeException*)e)->m_error == ERRORCODE_DoesNotExist))
                  {
                     // The address book data must have changed.
                     m_pData->SetSortIndexName(NULL);
                     m_pData->GetReceiverList().DeleteAllNames();
                     dwIndex = (DWORD)-1;
                  }
                  else
                  {
                     THROW_LAST();
                  }
               }
               END_CATCH_ALL
            }

            if (dwIndex == (DWORD)-1)
            {
#if 0
               if (m_cbSortList.GetCount() > 0)
               {
                  SetIndex(m_cbSortList.GetItemData(0));
               }
#endif
					if (m_SortIndexList.GetSize() > 0)
					{
						LPCSTR pszName;
						DWORD dwIndex;
						if (m_SortIndexList.GetAt(0, pszName, dwIndex))
						{
	                  SetIndex(dwIndex);
						}
					}
            }

            if (dwIndex != (DWORD)-1)
            {
               SetIndex(dwIndex);
            }
         }
      }
      CATCH_ALL(e)
      {
         Error = ERRORCODE_IntError;
      }
      END_CATCH_ALL

      // If we got an error, clean up.
      if (Error != ERRORCODE_None)
      {
         CloseDatabase();
      }

      if (Error == ERRORCODE_None)
      {
         OnFirst();
      }
   }

   // Set the window title text.
	if (!m_fReceiverList)
	{
		CString csFormat;
		CString csText;
		CString csUserName;
		TRY
		{
			if ((Error == ERRORCODE_None) && !csFullName.IsEmpty())
			{
				csFormat.LoadString(IDS_ADDRESS_BOOK_TITLE_TEXT_WITH_NAME);
				GetAddressBookUserName(csFullName, csUserName);
				csText.Format(csFormat, (LPCSTR)csUserName);
			}
			else
			{
				csText.LoadString(IDS_ADDRESS_BOOK_TITLE_TEXT_WITHOUT_NAME);
			}
		}
		END_TRY
		SetWindowText(csText);
	}

   return Error == ERRORCODE_None;
}

void CAddressBook::CloseDatabase(void)
{
   // Tell the various data views that the database is no longer valid.
	m_FieldList.SetRedraw(FALSE);
	CWnd* pInitialFocus = GetDlgItem(m_fReceiverList ? IDCANCEL : IDC_ADD);
	if (pInitialFocus != NULL)
	{
		GotoDlgCtrl(pInitialFocus);
	}
   m_FieldList.ResetContent();
   m_OverviewList.SetDatabaseCursor(NULL);
   m_ReceiverList.SetDatabaseCursor(NULL);

   // Delete the tabs object.
   delete m_pDatabaseTabs;
   m_pDatabaseTabs = NULL;

   // There are no tabs, so clear out the tab control.
   if (m_Tabs.m_hWnd != NULL)
   {
      if (m_Tabs.GetItemCount() > 0)
      {
         m_Tabs.DeleteAllTabs();
      }
   }

   // Delete the cursor object.
   delete m_pDatabaseCursor;
   m_pDatabaseCursor = NULL;

   // Refresh the index information to empty.
   RefreshIndexInformation();

   // Close and delete the database object and file objects.
   CloseAddressBookDatabase(m_pDatabaseFile, m_pDatabase);
	m_FieldList.SetRedraw(TRUE);
}

void CAddressBook::RefreshIndexInformation(void)
{
   CFlatFileIndexDescription* pIndexDescriptions = NULL;
   DWORD dwIndexCount = 0;

   if (m_pDatabaseCursor != NULL)
   {
      pIndexDescriptions = m_pDatabaseCursor->GetIndexDescriptions();
      dwIndexCount = m_pDatabaseCursor->GetIndexDescriptionCount();
   }

   // Just in case...
   if (dwIndexCount+IDC_SELECT_INDEX_MIN > IDC_SELECT_INDEX_MAX)
   {
      dwIndexCount = IDC_SELECT_INDEX_MAX-IDC_SELECT_INDEX_MIN+1;
   }

#if 0
   // Refresh the contents of the "sort by" list. box.
   m_cbSortList.ResetContent();
   m_cbSortList.SetCurSel(-1);

   if (m_pDatabaseCursor != NULL)
   {
      for (DWORD dwIndex = 0; dwIndex < dwIndexCount; dwIndex++)
      {
         CFlatFileIndexDescription* pDescription = &(pIndexDescriptions[dwIndex]);
         if (INDEX_USER_DATA_DISPLAY(pDescription->GetUserData()) != INDEX_DISPLAY_NONE)
         {
            int nIndex = m_cbSortList.AddString(pDescription->GetName());
            if (nIndex != CB_ERR)
            {
               m_cbSortList.SetItemData(nIndex, dwIndex);
            }
         }
      }

      m_cbSortList.EnableWindow(m_cbSortList.GetCount() != 0);
      m_cbSortList.SetCurSel(-1);
      m_cbSortList.SetRedraw(TRUE);
   }
#endif
   // Refresh the contents of the "sort by" list. box.
	m_nCurrentSortIndex = -1;
   m_SortIndexList.DeleteAll();

   if (m_pDatabaseCursor != NULL)
   {
      for (DWORD dwIndex = 0; dwIndex < dwIndexCount; dwIndex++)
      {
         CFlatFileIndexDescription* pDescription = &(pIndexDescriptions[dwIndex]);
         if (INDEX_USER_DATA_DISPLAY(pDescription->GetUserData()) != INDEX_DISPLAY_NONE)
         {
				m_SortIndexList.Add(pDescription->GetName(), dwIndex);
         }
      }
   }

   // Refresh the contents of the "sort by" menu.
   CMenu* pSortMenu = FindMenuByName(IDS_ADDRESS_BOOK_SORT_MENU);
   if (pSortMenu != NULL)
   {
      // Delete the current menu items.
      int nCount = pSortMenu->GetMenuItemCount();
      while (nCount-- > 0)
      {
         pSortMenu->DeleteMenu(nCount, MF_BYPOSITION);
      }

      // Add in the new menu items.
      if (m_pDatabaseCursor != NULL)
      {
         for (DWORD dwIndex = 0; dwIndex < dwIndexCount; dwIndex++)
         {
            CFlatFileIndexDescription* pDescription = &(pIndexDescriptions[dwIndex]);
            if (INDEX_USER_DATA_DISPLAY(pDescription->GetUserData()) != INDEX_DISPLAY_NONE)
            {
               pSortMenu->InsertMenu((UINT)-1, MF_BYPOSITION|MF_ENABLED|MF_STRING, (UINT)(IDC_SELECT_INDEX_MIN+dwIndex), pDescription->GetName());
            }
         }
      }

      // If there are no items, add the "empty" indicator.
      if (pSortMenu->GetMenuItemCount() == 0)
      {
         pSortMenu->InsertMenu((UINT)-1, MF_BYPOSITION|MF_DISABLED|MF_STRING, IDC_EMPTY_SORT_MENU, m_csEmptySortMenu);
      }
   
      DrawMenuBar();
   }
}

void CAddressBook::SetIndex(DWORD dwIndex)
{
   if (m_pDatabaseCursor != NULL)
   {
      BeforeRecordChange();

      if (dwIndex < m_pDatabaseCursor->GetIndexDescriptionCount())
      {
#if 0
         // Update the sort list box.
         int nCount = m_cbSortList.GetCount();
         if (nCount != CB_ERR)
         {
            for (int nIndex = 0; nIndex < nCount; nIndex++)
            {
               if (m_cbSortList.GetItemData(nIndex) == dwIndex)
               {
                  m_cbSortList.SetCurSel(nIndex);
               }
            }
         }
#endif
			m_nCurrentSortIndex = m_SortIndexList.FindByIndex(dwIndex);

         // Update the Sort menu.
         CMenu* pSortMenu = FindMenuByName(IDS_ADDRESS_BOOK_SORT_MENU);
         if (pSortMenu != NULL)
         {
            // Uncheck all of the current menu items.
            int nCount = pSortMenu->GetMenuItemCount();
            while (nCount-- > 0)
            {
               pSortMenu->CheckMenuItem(nCount, MF_BYPOSITION|MF_UNCHECKED);
            }

            // Check the menu item for the new index.
            pSortMenu->CheckMenuItem((UINT)(IDC_SELECT_INDEX_MIN+dwIndex), MF_BYCOMMAND|MF_CHECKED);
         }

         // Update the database cursor.
         m_pDatabaseCursor->SetIndex(dwIndex);
         if (m_pDatabaseCursor->GetPosition() == (DWORD)-1)
         {
            m_pDatabaseCursor->SetPosition(0);
         }

         // Refresh the overview and receiver lists.
         m_OverviewList.SetDatabaseCursor(m_pDatabaseCursor);
         m_ReceiverList.SetDatabaseCursor(m_pDatabaseCursor);

         // Update the tabs.
         if (m_pDatabaseTabs != NULL)
         {
            BOOL fSetFormat = FALSE;
            CFlatFileIndexDescription* pDescription = m_pDatabaseCursor->GetIndexDescription();
            if (pDescription != NULL)
            {
               switch (INDEX_USER_DATA_DISPLAY(pDescription->GetUserData()))
               {
                  case INDEX_DISPLAY_ALPHA:
                  {
                     m_pDatabaseTabs->SetAlphaFormat(INDEX_USER_DATA_WORD(pDescription->GetUserData()));
                     fSetFormat = TRUE;
                     break;
                  }
                  case INDEX_DISPLAY_ALPHA_LIST:
                  {
                     m_pDatabaseTabs->SetIndexListFormat(&m_ListManager, pDescription);
                     fSetFormat = TRUE;
                     break;
                  }
                  case INDEX_DISPLAY_ZIP:
                  {
                     m_pDatabaseTabs->SetZipFormat(INDEX_USER_DATA_WORD(pDescription->GetUserData()));
                     fSetFormat = TRUE;
                     break;
                  }
               }
            }

            if (!fSetFormat)
            {
               m_pDatabaseTabs->SetAlphaFormat(2);
            }
         }
      }

      m_fUpdateTabs = TRUE;
      AfterRecordChange(FALSE, FALSE);
   }
}

void CAddressBook::RefreshFieldInformation(void)
{
   // Refresh the field list.
   m_FieldList.SetRedraw(FALSE);
   m_FieldList.ResetContent();

   if (m_pDatabaseCursor != NULL)
   {
      DWORD dwFieldCount = m_pDatabaseCursor->GetFieldCount();

      for (DWORD dwField = 0; dwField < dwFieldCount; dwField++)
      {
         if (FIELD_USER_DATA_EDITOR(m_pDatabaseCursor->GetField(dwField)->GetDescription()->GetUserData()) != FIELD_EDIT_NONE)
         {
            CAddressBookFieldListBoxItem* pItem = new CAddressBookFieldListBoxItem(&m_FieldList, m_pDatabaseCursor->GetField(dwField));
            m_FieldList.AddString((LPCSTR)pItem);
         }
      }
   }

   m_FieldList.SetRedraw(TRUE);
}
// Update the "Name NN of MM" information in the interface. Perhaps this object should
// figure out this information on its own.
void CAddressBook::RefreshNameNumber(void)
{
   CString csText;

   csText.Empty();

   TRY
   {
      if (m_pDatabaseCursor != NULL)
      {
         if (m_fAddingNewRecord)
         {
            Util::LoadString(IDS_ADDING_NEW_NAME, csText);
         }
         else
         {
            DWORD dwCount = m_pDatabaseCursor->GetRecordCount();
            if (dwCount == 0)
            {
               Util::LoadString(IDS_ADDRESS_BOOK_EMPTY, csText);
            }
            else
            {
               csText.Format(m_csNameNumberFormat, m_pDatabaseCursor->GetPosition()+1, dwCount);
            }
         }
      }
   }
   END_TRY

   m_NameNumber.SetWindowText(csText);
}
// The user selected a new sort key from the sort menu.
// Select the associated index.
void CAddressBook::OnSelectIndex(UINT nSelection)
{
   nSelection -= IDC_SELECT_INDEX_MIN;
   SetIndex((DWORD)nSelection);
}
// Helper function to locate a popup menu off the top-level menu by its name.
CMenu* CAddressBook::FindMenuByName(UINT nMenuName)
{
   CMenu* pFindMenu = NULL;

   CString csFindMenu;
   CString csMenu;

   TRY
   {
      // Get the top-level menu.
      CMenu* pMenu = GetMenu();
      if (pMenu != NULL)
      {
         // Get the menu label to look for.
         csFindMenu.LoadString(nMenuName);

         // Search for the menu.
         int nCount = pMenu->GetMenuItemCount();
         for (int nMenu = 0; nMenu < nCount; nMenu++)
         {
            Util::GetMenuString(pMenu, nMenu, csMenu, MF_BYPOSITION);
            if (csMenu == csFindMenu)
            {
               // Found the menu, save the pointer.
               pFindMenu = pMenu->GetSubMenu(nMenu);
               break;
            }
         }
      }
   }
   END_TRY

   return pFindMenu;
}
// When the address book is sized, we have to move all the controls around.
// At the moment, this process is table driven. This allows us to move or expand
// controls by fixed amounts. Centering is not possible with the current code
// because round-off errors would occur and no "original" position information
// is maintained. It could be added if necessary.
void CAddressBook::OnSize(UINT nType, int cx, int cy) 
{
   // In OnSize(), cx and cy are client area dimensions.

   if (m_fSizesValid)
   {
      // Control movement table.
      static ControlMovement Movement[] =
      {
         {  IDC_ADDRESS_BOOK_FIELD_LIST,     ControlMovement::EX|ControlMovement::EY      },
         {  IDC_ADDRESS_BOOK_OVERVIEW_LIST,  ControlMovement::EX|ControlMovement::EY      },
         {  IDC_ADDRESS_BOOK_RECEIVER_LIST,  ControlMovement::EX|ControlMovement::EY      },
         {  IDC_ADDRESS_BOOK_TABS,           ControlMovement::EX|ControlMovement::EY      },
         {  IDC_FIRST,                                           ControlMovement::MY      },
         {  IDC_BACK,                                            ControlMovement::MY      },
         {  IDC_NEXT,                        ControlMovement::MX|ControlMovement::MY      },
         {  IDC_LAST,                        ControlMovement::MX|ControlMovement::MY      },
         {  IDC_NAME_NUMBER,                 ControlMovement::EX|ControlMovement::MY      },
         {  IDC_ADD,                         ControlMovement::MX                          },
         {  IDC_DELETE,                      ControlMovement::MX                          },
         {  IDC_UNDO,								ControlMovement::MX                          },
         {  IDC_SAVE,								ControlMovement::MX                          },
         {  IDC_ALL,									ControlMovement::MX                          },
         {  IDC_NONE,								ControlMovement::MX                          },
         {  IDCANCEL,                        ControlMovement::MX                          },
         {  IDOK,										ControlMovement::MX                          },
         {  0,                               0                                            }
      };

      // Compute the size deltas. In theory, the inherited OnSize could have changed
      // the client size, so we can't use cx and cy.
      CRect crNew;
      GetClientRect(crNew);

      MoveControls(this, crNew.Width()-m_nCurrentWidth, crNew.Height()-m_nCurrentHeight, &(Movement[0]));
   }
   // Redraw it all.
   Invalidate();

   // Save the new sizes.
   m_nCurrentWidth = cx;
   m_nCurrentHeight = cy;
}
// Don't let the window get any smaller than the original design dimensions.
// So, the dialog layed out in App Studio is the minimum dimension address book.
void CAddressBook::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
   INHERITED::OnGetMinMaxInfo(lpMMI);

   if (m_fSizesValid)
   {
      lpMMI->ptMinTrackSize.x = m_nMinimumWidth;
      lpMMI->ptMinTrackSize.y = m_nMinimumHeight;
   }
}
// The user selected "One at a Time" from the View menu.
void CAddressBook::OnViewOneatatime() 
{
   SetView(ID_VIEW_ONEATATIME);
}
// The user selected "Overview" from the View menu.
void CAddressBook::OnViewOverview() 
{
   SetView(ID_VIEW_OVERVIEW);
}
// The user selected "Receiver List" from the View menu.
void CAddressBook::OnViewReceiverlist() 
{
   SetView(ID_VIEW_RECEIVERLIST);
}
// Set the current view. The view combo box and menu are
// updated with the selection.
void CAddressBook::SetView(UINT uView)
{
   if (m_pData->GetDisableReceivers() && (uView == ID_VIEW_RECEIVERLIST))
   {
      // Don't change to receiver list.
      AfxMessageBox(IDS_ADDRESS_BOOK_RECEIVER_LIST_DISABLED);
      uView = m_uView;
   }

   // Save any pending information.
   TRY
   {
      // Save any changes.
      BeforeRecordChange();

      // Update interface.
      AfterRecordChange(FALSE, FALSE);
   }
   END_TRY

   // Update the View menu.
   CMenu* pViewMenu = FindMenuByName(IDS_ADDRESS_BOOK_VIEW_MENU);
   if (pViewMenu != NULL)
   {
      // Uncheck all of the current menu items.
      int nCount = pViewMenu->GetMenuItemCount();
      while (nCount-- > 0)
      {
         pViewMenu->CheckMenuItem(nCount, MF_BYPOSITION|MF_UNCHECKED);
      }

      // Check the menu item for the new index.
      pViewMenu->CheckMenuItem(uView, MF_BYCOMMAND|MF_CHECKED);
   }

   // Hide the previous view.
   CurrentViewShow(FALSE);
   CurrentViewActivate(FALSE);

   // Set the new view.
   m_uView = uView;

   // Show the new view.
   CurrentViewActivate();
   CurrentViewShow();
}
// The user selected "Import Names" from the File menu.
void CAddressBook::OnFileImportnames() 
{
   CString csAddressBookName;

   CloseDatabase();

   // We're off to see the wizard!
   if (CAddressBookImportWizard::Run(this, csAddressBookName))
   {
      // If the imported database has the same name as the one we were
      // using, then the sort index and receiver list may be out of date.
      if (csAddressBookName.CompareNoCase(m_pData->GetAddressBookFullName()) == 0)
      {
         m_pData->SetSortIndexName(NULL);
         m_pData->GetReceiverList().DeleteAllNames();
      }

      // Switch to imported database.
      SetDatabase(csAddressBookName);
   }
   else
   {
      SetDatabase(m_pData->GetAddressBookFullName());
   }
   // JRO KLUDGE!! The window is sized here to get the scroll bars to reinitialize
   // Importing a file with a different number of fields than what was before screws this up
   // There IS a better way to do this...

   CRect Rect;
   GetWindowRect (&Rect);
   Rect.bottom += 1;
   MoveWindow (Rect);
   TRACE ("\n!!KLUDGE ALERT!! CAddressBook::OnFileImportnames() did a kludge!\n\n");
}

#define  ABEX_FLAGS        OFN_HIDEREADONLY|OFN_NOREADONLYRETURN|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST

void CAddressBook::WriteFieldString(CString &str, FILE *pFile, TCHAR Delim)
{
   CString  temp;
   BOOL     GlobalQuote = FALSE;
   // If there is one of these in the string,
   //  put quotes around the whole damned thing
   if (str.FindOneOf ("\r\n,\"") >= 0)
      GlobalQuote = TRUE;   
   
   if (GlobalQuote == TRUE)
      temp += '\"';
   for (int i = 0; i < str.GetLength(); i ++)
   {
      char c = str.GetAt (i); // Double quotes are indicated by double double quotes
      if (c == '\"')
         temp += '\"';
      temp += c;
   }
   if (GlobalQuote == TRUE)
      temp += '\"';
	// If there is nothing to write, avoid exception
   size_t	st = temp.GetLength();
	if (st > 0)
		if (fwrite((const char *)temp, st, 1, pFile) != 1)
			ThrowErrorcodeException(ERRORCODE_IoError);
   // If a delimiter is indicated, write it out
   if (Delim != 0)
   {
      if (fwrite(&Delim,1,1,pFile) != 1)
         ThrowErrorcodeException(ERRORCODE_IoError);
   }
}
// The user selected "Export" from the File menu.
void CAddressBook::OnFileExportnames() 
{
   if (m_pDatabase == NULL)
      return;
   
   CString  Filter;
   Filter.LoadString (IDS_ADDRESS_BOOK_EXPORT_FILTER);

   CString InitialPath = GetGlobalPathManager()->ExpandPath("[[A]]");

   CFileDialog   Dlg (FALSE, "*.abk", NULL, ABEX_FLAGS, Filter, this);
   Dlg.m_ofn.lpstrInitialDir = InitialPath;

   if (Dlg.DoModal () == IDOK)
   {
      CWaitCursor Wait;
      TCHAR    Delimiter = ',';     
      LPCSTR   CRLF = "\r\n";
      CString  FileNameOut = Dlg.GetPathName ();
      ERRORCODE Result = ERRORCODE_Fail;
		// Make sure any new stuff is saved
		TRY
		{
			// Save any changes.
			BeforeRecordChange();
			// Update interface.
			AfterRecordChange(FALSE, FALSE);
		}
		END_TRY
      // ABK file chosen - the database file will just be copied
      if (Dlg.m_ofn.nFilterIndex == 1) 
      {
         if (FileNameOut.CompareNoCase(m_pData->GetAddressBookFullName()) == 0)
         {
            AfxMessageBox (IDS_COPY_SELF);
            return;
         }
         // Let go of current database file
         CloseDatabase();
         
         if (int IfOnlyStorageFileHadACloseMethod = 1) 
         {
            StorageFile FileOut (FileNameOut);
            StorageFile FileIn  (m_pData->GetAddressBookFullName());
            Result = copy_file(&FileIn, &FileOut);
         }
         // Get current database back
         SetDatabase(m_pData->GetAddressBookFullName());
         // It worked - we're done
         if (Result == ERRORCODE_None)
            return;
         // Get rid of possibly currupt file
         remove (FileNameOut);
         
         Filter = GET_PMWAPP()->GetResourceStringPointer (IDS_ERROR_SAVING);
         Filter += FileNameOut;
         AfxMessageBox (Filter);
      }
      else
      if (Dlg.m_ofn.nFilterIndex == 2) // Tab delimited text file chosen
         Delimiter = 9;

      FILE  *pFile;
      if ((pFile = fopen (FileNameOut, "w+b")) != NULL)
      {
         CFlatFileDatabaseCursor Cursor;
         
         TRY
         {
            Cursor.Attach(m_pDatabase);
            Cursor.SetIndex((DWORD)0);
            // Write out all string type field names
            DWORD FieldCount = Cursor.GetFieldCount ();
            for (DWORD d = 0; d < FieldCount; d ++)
            {
               CFlatFileDatabaseField* pField = Cursor.GetField(d);
               if (pField != NULL)
               {
                  if (pField->GetType() == FFT_String)
                  {
                     CFlatFileFieldDescription* pDescription = pField->GetDescription();
                     Filter = pDescription->GetName ();
                     // Write out field name, and delimiter if this is not the last one
                     // Note - there is a bug here if the last field is one that is skipped
                     WriteFieldString (Filter, pFile, d + 1 == FieldCount ? 0 : Delimiter);
                  }
               }
            }
            // Start new line
            if (fwrite(CRLF, 2 , 1, pFile) != 1)
               ThrowErrorcodeException(ERRORCODE_IoError);
            // Walk through each string field as above, but write out each field's value this time
            Cursor.First();
            while (!Cursor.AtEOF())
            {
               for (DWORD d = 0; d < FieldCount; d ++)
               {
                  CFlatFileDatabaseField* pField = Cursor.GetField(d);
                  if (pField != NULL)
                  {
                     if (pField->GetType() == FFT_String)
                     {
                        CFlatFileFieldDescription* pDescription = pField->GetDescription();
                        Filter = pDescription->GetName ();
                        CString Field = Cursor.GetField(Filter)->GetAsString();
                        WriteFieldString (Field, pFile, d + 1 == FieldCount ? 0 : Delimiter);
                     }
                  }
               }
               
               if (fwrite(CRLF, 2 , 1, pFile) != 1)
                  ThrowErrorcodeException(ERRORCODE_IoError);

               Cursor.Next();
            }
            // Done - indicate success
            Result = ERRORCODE_None;
         }
         CATCH (CErrorcodeException, e)
         {
            Result = e->m_error;
         }
         AND_CATCH_ALL(e)
         {
         }
         END_CATCH_ALL

         fclose (pFile);   
         
         if (Result != ERRORCODE_None)
         {
            remove (FileNameOut);
            
            Filter = GET_PMWAPP()->GetResourceStringPointer (IDS_ERROR_SAVING);
            Filter += FileNameOut;
            AfxMessageBox (Filter);
         }
      }
      else
         AfxMessageBox (IDS_ERROR_SAVING);
   }
}
// The address book field list box was focused, start editing fields.
void CAddressBook::OnSetfocusAddressBookFieldList() 
{
   m_FieldList.Edit();
}
// When a field editor is clicked upon and gets the focus, it sends a message to the
// field editor list box which passes the message up to the address book dialog. This
// message is used to automatically put the address book in "Add New Name" mode when
// the user clicks on a field in an empty address book.
LRESULT CAddressBook::OnEditorActivated(WPARAM wParam, LPARAM lParam)
{
   if ((m_pDatabaseCursor->GetRecordCount() == 0) && !m_fAddingNewRecord)
   {
      m_fAddingNewRecord = TRUE;
      m_fUpdateTabs = FALSE;
      AfterRecordChange(TRUE, TRUE);
      m_fAddingNewRecord = TRUE;
   }

   return 0;
}

LRESULT CAddressBook::OnGetFieldList(WPARAM wParam, LPARAM lParam)
{
   return (LRESULT)(m_ListManager.GetListForField((CFlatFileFieldDescription*)lParam));
}

LRESULT CAddressBook::OnGetDatabaseTabs(WPARAM wParam, LPARAM lParam)
{
   return (LRESULT)(m_pDatabaseTabs);
}
// WM_SELECT_RECORD message handler. This is sent by child windows like the overview
// list to cause a partucular record to be selected. The desired record number is
// passed in LPARAM.
LRESULT CAddressBook::OnSelectRecord(WPARAM wParam, LPARAM lParam)
{
   if (m_pDatabaseCursor != NULL)
   {
      TRY
      {
         BeforeRecordChange();
         m_pDatabaseCursor->SetPosition((DWORD)lParam);
         AfterRecordChange(FALSE, FALSE);
      }
      END_TRY
   }

   return 0;
}
// WM_GET_RECEIVER message handler. This is sent by child windows to
// get the address of the receiver name list.
LRESULT CAddressBook::OnGetReceiverList(WPARAM wParam, LPARAM lParam)
{
   return (LRESULT)(&m_pData->GetReceiverList());
}
// WM_HELP_USER message handler. This is sent to ourselves after the first paint.
// We take this opportunity to see if we can give the user some help at the
// start of the process.
#if 0
LRESULT CAddressBook::OnHelpUser(WPARAM wParam, LPARAM lParam)
{
   if ((m_uView == ID_VIEW_RECEIVERLIST)
    && (m_pDatabaseCursor != NULL)
    && (m_pDatabaseCursor->GetRecordCount() == 0))
   {
      CAddressBookNoReceiversHelpDialog Dialog(this);
      Dialog.DoModal();
   }

   return 0;
}
#endif
// Helper function to do all the stuff that needs to be done
// before changing records:
//
//    1) Invalidate any lists based on modified fields.
//    2) Save any field changes back to the database.
//    3) Clear "adding a new record" flag.
//    4) Set m_fUpdateTabs so AfterRecordChange() will know if UpdateTabs() should be called.

void CAddressBook::BeforeRecordChange(void)
{
   // Initialize m_fUpdateTabs to flags. This flag is used by AfterRecordChange()
   // to decide if UpdateTabs() should be called (a potentially expensive operation.)
   // m_fUpdateTabs is set to TRUE if the main field of the current index (the one
   // represented on the tabs) has changed.
   m_fUpdateTabs = FALSE;

   if (m_pDatabaseCursor != NULL)
   {
      TRY
      {
         // Tell the current view we're about to change the record.
         CurrentViewBeforeRecordChange();

         // If the fields have been changed, save the changes.
         if (CurrentViewIsModified())
         {
            // Update the fields with the new values.
            CurrentViewSave();

            // Let the list manager know about the modified fields.
            DWORD dwFieldCount = CurrentViewGetFieldCount();
            for (DWORD dwField = 0; dwField < dwFieldCount; dwField++)
            {
               CFlatFileDatabaseField* pField = CurrentViewGetField(dwField);
               if (pField != NULL)
               {
                  if (pField->IsModified())
                  {
                     // Field was modified.
                     m_ListManager.FieldModified(pField->GetDescription());
                  }
               }
            }

            // Check if the main field of the current index has been modified.
            // If so, then set m_fUpdateTabs so AfterRecordChange() will call UpdateTabs().
            TRY
            {
               // Get current index number.
               DWORD dwIndex = m_pDatabaseCursor->GetIndex();
               if (dwIndex != (DWORD)-1)
               {
                  // Get main field number of current index.
                  DWORD dwField = m_pDatabaseCursor->GetMainFieldOfIndex(dwIndex);
                  if (dwField != (DWORD)-1)
                  {
                     // Get pointer to field structure.
                     CFlatFileDatabaseField* pField = m_pDatabaseCursor->GetField(dwField);
                     if (pField != NULL)
                     {
                        // Set m_fUpdateTabs if main field has been modified.
                        m_fUpdateTabs = m_fAddingNewRecord || pField->IsModified();
                     }
                  }
               }
            }
            END_TRY

            // Database is changing.
            m_pData->SetDatabaseChanged();

            // Record the positions of the record before and after the update.
            // We will report these to the view so it can update itself appropriately.
            DWORD dwStartPosition = (DWORD)-1;
            DWORD dwEndPosition = (DWORD)-1;

            // Save the record back to the database.
            if (m_fAddingNewRecord)
            {
               // Check each of the fields in the new record. If they are all empty,
               // then do not save the record.
               if (!FieldsBlank())
               {
                  m_pDatabaseCursor->SetCounters();
                  m_pDatabaseCursor->Insert();
               }
            }
            else
            {
               dwStartPosition = m_pDatabaseCursor->GetPosition();
               m_pDatabaseCursor->Update();
            }

            dwEndPosition = m_pDatabaseCursor->GetPosition();

            CurrentViewRecordModified(dwStartPosition, dwEndPosition);
         }
      }
      END_TRY
   }

   // No longer adding a new record.
   m_fAddingNewRecord = FALSE;
}
// Helper function to do all the stuff that needs to be done
// after changing records:
//
//    1) Adjust current record back one if it's at the EOF.
//    2) Update the field list editors with values from the new record.
//    3) Update the "Name NN of MM" information in the interface.
//    4) Rebuild the tab name information.
//    5) Select the tab associated with the new record.
void CAddressBook::AfterRecordChange(BOOL fFieldDataValid /*=FALSE*/, BOOL fCurrentTabValid /*=FALSE*/)
{
   if (m_pDatabaseCursor != NULL)
   {
      // Check if we're past the last name. If so, move back one.
      // This can happen after a delete.
      TRY
      {
         if (m_pDatabaseCursor->AtEOF())
         {
            m_pDatabaseCursor->Previous();
         }
      }
      END_TRY

      if (!fFieldDataValid)
      {
         // Refresh the field editors with the new field values.
         CurrentViewRefresh();
      }

      // If we have some tabs, update the tabs to reflect the new record state.
      if (m_pDatabaseTabs != NULL)
      {
         if (m_fUpdateTabs)
         {
            // Rebuild tabs.
            m_pDatabaseTabs->UpdateTabs();
         }

         if (!fCurrentTabValid)
         {
            if (m_fAddingNewRecord)
            {
               // Move all of the tabs to the back.
				   if (m_Tabs.m_hWnd != NULL)
					{
						m_Tabs.SetCurSel(-1);
					}
            }
            else
            {
               // Select a tab based on the current record data.
               m_pDatabaseTabs->UpdateCurrentTab();
            }
         }
      }

      // Tell the current view we changed the record.
      CurrentViewAfterRecordChange();
   }

   // No longer adding a new record.
   m_fAddingNewRecord = FALSE;
}
// Loop through all the fields and check if all of them are empty.
BOOL CAddressBook::FieldsBlank(void)
{
   BOOL fBlank = TRUE;

   DWORD dwFieldCount = CurrentViewGetFieldCount();
   for (DWORD dwField = 0; fBlank && (dwField < dwFieldCount); dwField++)
   {
      CFlatFileDatabaseField* pField = CurrentViewGetField(dwField);
      if (pField != NULL)
      {
         if (pField->GetType() == FFT_Blob)
         {
            fBlank = fBlank & (pField->GetAsUnsigned32() == 0);
         }
         else
         {
            LPCSTR pszString = pField->GetAsString();
            if (pszString != NULL)
            {
               fBlank = fBlank & (strlen(pszString) == 0);
            }
         }
      }
   }

   return fBlank;
}
// The following methods are used to perform an action on the current view.
// Since the views are such different objects at the moment, switch statements
// are used to differentiate between the view types. Eventually, introducing
// a "view" object could help clean things up.
void CAddressBook::CurrentViewBeforeRecordChange(void)
{
   switch (m_uView)
   {
      case ID_VIEW_OVERVIEW:     m_OverviewList.BeforeRecordChange();   break;
      case ID_VIEW_RECEIVERLIST: m_ReceiverList.BeforeRecordChange();   break;
   }
}

void CAddressBook::CurrentViewRecordModified(DWORD dwStartPosition, DWORD dwEndPosition)
{
   switch (m_uView)
   {
      case ID_VIEW_OVERVIEW:     m_OverviewList.RecordModified(dwStartPosition, dwEndPosition); break;
   }
}

void CAddressBook::CurrentViewAfterRecordChange(void)
{
   switch (m_uView)
   {
      case ID_VIEW_OVERVIEW:     m_OverviewList.AfterRecordChange(); break;
      case ID_VIEW_RECEIVERLIST: m_ReceiverList.AfterRecordChange(); break;
   }
}

BOOL CAddressBook::CurrentViewIsModified(void)
{
   BOOL fIsModified = FALSE;

   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   fIsModified = m_FieldList.IsModified();      break;
      case ID_VIEW_OVERVIEW:     fIsModified = m_OverviewList.IsModified();   break;
   }

   return fIsModified;
}

void CAddressBook::CurrentViewSave(void)
{
   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   m_FieldList.Save();     break;
      case ID_VIEW_OVERVIEW:     m_OverviewList.Save();  break;
   }
}

DWORD CAddressBook::CurrentViewGetFieldCount(void)
{
   DWORD dwCount = 0;

   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   dwCount = m_FieldList.GetFieldCount();    break;
      case ID_VIEW_OVERVIEW:     dwCount = m_OverviewList.GetFieldCount(); break;
   }

   return dwCount;
}

CFlatFileDatabaseField* CAddressBook::CurrentViewGetField(DWORD dwField)
{
   CFlatFileDatabaseField* pField = NULL;

   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   pField = m_FieldList.GetField(dwField);      break;
      case ID_VIEW_OVERVIEW:     pField = m_OverviewList.GetField(dwField);   break;
   }

   return pField;
}

void CAddressBook::CurrentViewRefresh(void)
{
   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   m_FieldList.Refresh();     break;
      case ID_VIEW_OVERVIEW:     m_OverviewList.Refresh();  break;
   }
}

BOOL CAddressBook::CurrentViewIsEditing(void)
{
   BOOL fIsEditing = FALSE;

   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   fIsEditing = m_FieldList.IsEditing();     break;
      case ID_VIEW_OVERVIEW:     fIsEditing = m_OverviewList.IsEditing();  break;
   }

   return fIsEditing;
}

void CAddressBook::CurrentViewShow(BOOL fShow /*=TRUE*/)
{
   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:
      {
         m_FieldList.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE| (fShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
         break;
      }
      case ID_VIEW_OVERVIEW:
      {
         m_OverviewList.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE| (fShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
         break;
      }
      case ID_VIEW_RECEIVERLIST:
      {
         m_ReceiverList.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE| (fShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
         break;
      }
   }
}

void CAddressBook::CurrentViewStartNewEdit(void)
{
   // Move off any buttons so that the default button indicator
   // will be properly removed.
   if (!CurrentViewIsEditing())
   {
	   if (m_Tabs.m_hWnd != NULL)
		{
			GotoDlgCtrl(&m_Tabs);
		}
   }

   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   m_FieldList.Edit(0);    break;
      case ID_VIEW_OVERVIEW:     m_OverviewList.Edit(0); break;
   }
}

void CAddressBook::CurrentViewActivate(BOOL fActive /*=TRUE*/)
{
   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   m_FieldList.Refresh();                 break;
      case ID_VIEW_OVERVIEW:     m_OverviewList.ActivateView(fActive);  break;
      case ID_VIEW_RECEIVERLIST: m_ReceiverList.ActivateView(fActive);  break;
   }
}

void CAddressBook::CurrentViewBackPage(void)
{
   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   OnBack();                     break;
      case ID_VIEW_OVERVIEW:     m_OverviewList.BackPage();    break;
      case ID_VIEW_RECEIVERLIST: m_ReceiverList.BackPage();    break;
   }
}

void CAddressBook::CurrentViewNextPage(void)
{
   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:   OnNext();                     break;
      case ID_VIEW_OVERVIEW:     m_OverviewList.NextPage();    break;
      case ID_VIEW_RECEIVERLIST: m_ReceiverList.NextPage();    break;
   }
}

void CAddressBook::CurrentViewAbortNewEdit(void)
{
   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:
      {
         m_pDatabaseCursor->First();
         m_fAddingNewRecord = FALSE;
         m_fUpdateTabs = FALSE;
         AfterRecordChange(FALSE, FALSE);
         break;
      }
      case ID_VIEW_OVERVIEW:
      {
         m_OverviewList.Refresh();
         m_fAddingNewRecord = TRUE;
         m_fUpdateTabs = FALSE;
         AfterRecordChange(FALSE, FALSE);
         m_fAddingNewRecord = TRUE;
         break;
      }
   }
}

BOOL CAddressBook::CurrentViewCanDelete(void)
{
   BOOL fCanDelete = (m_pDatabaseCursor != NULL) && (m_pDatabaseCursor->GetPosition() < m_pDatabaseCursor->GetRecordCount());

   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:
      {
         // Special case: when adding a new record, we leave delete active so that
         // the user can click on it to delete their unsaved entry.
         if (m_fAddingNewRecord)
         {
            fCanDelete = TRUE;
         }
         break;
      }
      case ID_VIEW_OVERVIEW:
      {
         // If we're editing the "new" record in the overview view, then turn on
         // delete if the user has made some changes.
         if (m_fAddingNewRecord)
         {
            fCanDelete = m_OverviewList.IsModified();
         }
         break;
      }
      case ID_VIEW_RECEIVERLIST:
      {
         break;
      }
   }

   return fCanDelete;
}

BOOL CAddressBook::CurrentViewCanUndo(void)
{
   BOOL fCanUndo = CurrentViewIsModified();

   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:
      {
         // Special case: when adding a new record, we leave undo active so that
         // the user can click on it to delete their unsaved entry.
         if (m_fAddingNewRecord)
         {
            fCanUndo = TRUE;
         }
         break;
      }
      case ID_VIEW_OVERVIEW:
      {
         break;
      }
   }

   return fCanUndo;
}

BOOL CAddressBook::CurrentViewCanAll(void)
{
   BOOL fCanAll = FALSE;

   switch (m_uView)
   {
      case ID_VIEW_RECEIVERLIST:
      {
         fCanAll = (m_pDatabaseCursor != NULL) && (m_pDatabaseCursor->GetRecordCount() > 0);
         break;
      }
   }

   return fCanAll;
}

BOOL CAddressBook::CurrentViewCanNone(void)
{
   BOOL fCanNone = FALSE;

   switch (m_uView)
   {
      case ID_VIEW_RECEIVERLIST:
      {
         fCanNone = (m_pDatabaseCursor != NULL) && (m_pDatabaseCursor->GetRecordCount() > 0);
         break;
      }
   }

   return fCanNone;
}

void CAddressBook::CurrentViewDatabaseEmpty(void)
{
   switch (m_uView)
   {
      case ID_VIEW_ONEATATIME:
      {
         // Get to "Address Book Empty" state.
         m_fUpdateTabs = TRUE;
         AfterRecordChange(FALSE, FALSE);
         break;
      }
      case ID_VIEW_OVERVIEW:
      {
         // Initialize the fields to receive the new data.
         m_pDatabaseCursor->New();

         // Set "adding" flag. Note that it must be set twice because AfterRecordChange() clears it.
         m_fAddingNewRecord = TRUE;
         m_fUpdateTabs = TRUE;
         AfterRecordChange(FALSE, FALSE);
         m_fAddingNewRecord = TRUE;
         break;
      }
      case ID_VIEW_RECEIVERLIST:
      {
         // Get to "Address Book Empty" state.
         m_fUpdateTabs = TRUE;
         AfterRecordChange(FALSE, FALSE);
         break;
      }
   }
}

BOOL CAddressBook::GetImportantFields(void)
{
   BOOL fResult = FALSE;

   m_dwIdField = (DWORD)-1;
   m_dwFirstNameField = (DWORD)-1;
   m_dwLastNameField = (DWORD)-1;
   m_dwFullNameField = (DWORD)-1;

   if (m_pDatabaseCursor != NULL)
   {
      TRY
      {
         // Get the ID field... it MUST exist.
         m_dwIdField = m_pDatabaseCursor->GetFieldNumberFromName("__ID__");
         if (m_dwIdField != (DWORD)-1)
         {
            // Get the database's User Data. The names of the important fields are stored there.
            LPVOID pData = NULL;
            DWORD dwSize;
            m_pDatabaseCursor->GetUserData(&pData, &dwSize);
            if (pData != NULL)
            {
               // Parse the user data into separate strings.
               CMlsStringArray csaData((LPCSTR)pData);

               // Free the data that was allocated.
               CFlatFileDatabase::FreeMemory(pData);

               // Field #0 is type of database.

               // Field #1 is the name of the first name field.
               if (csaData.GetSize() > 1)
               {
                  m_dwFirstNameField = m_pDatabaseCursor->GetFieldNumberFromName(csaData.ElementAt(1));
               }

               // Field #2 is the name of the last name field.
               if (csaData.GetSize() > 2)
               {
                  m_dwLastNameField = m_pDatabaseCursor->GetFieldNumberFromName(csaData.ElementAt(2));
               }

               // Field #3 is the name of the full name field.
               if (csaData.GetSize() > 3)
               {
                  m_dwFullNameField = m_pDatabaseCursor->GetFieldNumberFromName(csaData.ElementAt(3));
               }

               // We need both first name and a last name fields, or a full name field.
               if (((m_dwFirstNameField != (DWORD)-1) && (m_dwLastNameField != (DWORD)-1))
                || (m_dwFullNameField != (DWORD)-1))
               {
                  fResult = TRUE;
               }
            }
         }
      }
      END_TRY
   }

   return fResult;
}

void CAddressBook::GetDisplayName(CFlatFileDatabaseCursor* pCursor, CString& csName, BOOL fIncludeIndexValue /*=TRUE*/)
{
   TRY
   {
      csName.Empty();

      if (pCursor != NULL)
      {
         // Try to use first and last name.
         if ((m_dwFirstNameField != (DWORD)-1) && (m_dwLastNameField != (DWORD)-1))
         {
            CFlatFileDatabaseField* pFirstNameField = pCursor->GetField(m_dwFirstNameField);
            CFlatFileDatabaseField* pLastNameField = pCursor->GetField(m_dwLastNameField);
            if ((pFirstNameField != NULL) && (pLastNameField != NULL))
            {
               csName += pFirstNameField->GetAsString();
               if (!csName.IsEmpty())
               {
                  csName += ' ';
               }
               csName += pLastNameField->GetAsString();
            }
         }
         else
         {
            // No first and last name, try to use full name.
            if (m_dwFullNameField != (DWORD)-1)
            {
               CFlatFileDatabaseField* pFullNameField = pCursor->GetField(m_dwFullNameField);
               if (pFullNameField != NULL)
               {
                  csName = pFullNameField->GetAsString();
               }
            }
         }

         // If the name is empty, use something!
         if (csName.IsEmpty())
         {
            csName.LoadString(IDS_EMPTY_NAME);
         }

         // If the caller wants, append the index value if it's not already included as part of the name.
         DWORD dwIndex = pCursor->GetIndex();
         if (dwIndex != (DWORD)-1)
         {
            DWORD dwField = pCursor->GetMainFieldOfIndex(dwIndex);
            if (dwField != (DWORD)-1)
            {
               if ((dwField != m_dwFirstNameField)
                && (dwField != m_dwLastNameField)
                && (dwField != m_dwFullNameField))
               {
                  CFlatFileDatabaseField* pIndexField = pCursor->GetField(dwField);
                  if (pIndexField != NULL)
                  {
                     CString csIndex;
                     TRY
                     {
                        csIndex = pIndexField->GetAsString();
                     }
                     END_TRY
                     if (!csIndex.IsEmpty())
                     {
                        csName += "  (";
                        csName += csIndex;
                        csName += ")";
                     }
                  }
               }
            }
         }
      }
   }
   END_TRY
}
// We trap WM_PAINT so we can give some instructions after the address book has been drawn.
#if 0
void CAddressBook::OnPaint() 
{
   Default();

   if (m_fFirstPaint)
   {
      m_fFirstPaint = FALSE;
      PostMessage(WM_HELP_USER, 0, 0);
   }
}
#endif
// Run the address book.
BOOL CAddressBook::Run(CWnd* pParent, NameListRecord* pNameList /*=NULL*/, BOOL fSelectReceivers /*=FALSE*/, BOOL fDisableReceivers /*=FALSE*/, COLORREF cr /*INVALID_DIALOG_COLOR*/)
{
   BOOL fResult = FALSE;
   CAddressBookData* pData = NULL;
   CString csFullName;

   // If the caller has passed in a specific address book, then make
   // sure it can be accessed.
   BOOL fForceChooseNewAddressBook = FALSE;
   if (pNameList != NULL)
   {
      StorageFile* pDatabaseFile = NULL;
      CFlatFileDatabase* pDatabase = NULL;
      TRY
      {
         csFullName = pNameList->AddressBookName();
         ERRORCODE Error = OpenAddressBookDatabase(csFullName, pDatabaseFile, pDatabase, FALSE);
         CloseAddressBookDatabase(pDatabaseFile, pDatabase);
         if (Error != ERRORCODE_None)
         {
            CString csFormat;
            CString csMessage;
            CString csUserName;
            TRY
            {
               GetAddressBookUserName(csFullName, csUserName);
               csFormat.LoadString(IDS_ADDRESS_BOOK_NOT_AVAILABLE);
               csMessage.Format(csFormat, csUserName);
               if (AfxMessageBox(csMessage, MB_YESNO) != IDYES)
               {
                  // User does not want to continue.
                  return FALSE;
               }
            }
            END_TRY

            csFullName.Empty();
            fForceChooseNewAddressBook = TRUE;
         }
      }
      END_TRY
   }
   // If the caller has not passed a name list, then we are just going to browse/edit
   // the available address books. There is no document involved and thus no receiver
   // list. So, ask the user which address book they want to use. There are three cases:
   //
   //    1) No address books: ask the user if they want to create one.
   //    2) One address book: go ahead and select it.
   //    3) More than one address book: let the user choose.
   if (fForceChooseNewAddressBook || (pNameList == NULL))
   {
      CStringArray csaUserNames;
      TRY
      {
         GetAddressBookUserNames(csaUserNames);
         if (csaUserNames.GetSize() == 0)
         {
            // No address books.
            CAddressBookDoesNotExistDialog Dialog("", pParent);
            if (Dialog.DoModal() == IDOK)
            {
               csFullName = Dialog.GetFullName();
            }
         }
         else if (csaUserNames.GetSize() == 1)
         {
            // One name.
            GetAddressBookFullName(csaUserNames.ElementAt(0), csFullName);
         }
         else if (csaUserNames.GetSize() > 1)
         {
            // More than one name.
            COpenAddressBookDialog Dialog(pParent);
            if (Dialog.DoModal() == IDOK)
            {
               csFullName = Dialog.GetFullName();
            }
         }

         if (!csFullName.IsEmpty())
         {
            pData = new CAddressBookData(csFullName, NULL, 0, NULL, fSelectReceivers, fDisableReceivers);
         }
      }
      END_TRY
   }
   else
   {
      TRY
      {
         pData = new CAddressBookData(pNameList, fSelectReceivers, fDisableReceivers);
      }
      END_TRY
   }

   if (pData != NULL)
   {
		if (pData->GetSelectReceivers())
		{
			StorageFile* pDatabaseFile = NULL;
			CFlatFileDatabase* pDatabase = NULL;
			TRY
			{
				csFullName = pData->GetAddressBookFullName();
				DWORD dwCount = -1;
				if (OpenAddressBookDatabase(csFullName, pDatabaseFile, pDatabase, FALSE) == ERRORCODE_None)
				{
					if (pDatabase->GetRecordCount(0, &dwCount) != ERRORCODE_None)
					{
						dwCount = -1;
					}
				}
				CloseAddressBookDatabase(pDatabaseFile, pDatabase);

				if (dwCount == 0)
				{
					CString csFormat;
					CString csMessage;
					CString csUserName;
					TRY
					{
						GetAddressBookUserName(csFullName, csUserName);
						csFormat.LoadString(IDS_NO_MERGE_NAMES);
						csMessage.Format(csFormat, csUserName);
						if (AfxMessageBox(csMessage, MB_YESNO) != IDYES)
						{
							// User does not want to continue.
							return FALSE;
						}

						// Go into address book editing mode.
						pData->SetSelectReceivers(FALSE);
					}
					END_TRY
				}
			}
			END_TRY
		}

		CAddressBook AddressBook(pParent, pData, cr);
		fResult = AddressBook.DoModal() == IDOK;
   }

   // Copy the new data back to the name list.
   if (fResult && (pNameList != NULL) && (pData != NULL))
   {
      TRY
      {
         pNameList->ClearChanged();

         // Check if data has changed.
         if (fForceChooseNewAddressBook || pData->GetDatabaseChanged())
         {
//          TRACE("Database Changed\n");
            pNameList->SetAddressBookDataChanged();
         }

         // Check if the address book name changed.
         if (fForceChooseNewAddressBook || pData->GetAddressBookFullNameChanged())
         {
//          TRACE("Address Book Name Changed\n");
            pNameList->SetAddressBookNameChanged();
            pNameList->AddressBookName(pData->GetAddressBookFullName());
         }

         // Check if sort field name changed.
         if (fForceChooseNewAddressBook || pData->GetSortIndexNameChanged())
         {
//          TRACE("Sort Index Name Changed\n");
            pNameList->SetSortFieldChanged();
            pNameList->SortField(pData->GetSortIndexName());
         }

         // Check if receiver list has changed.
         if (fForceChooseNewAddressBook || pData->GetReceiverListChanged())
         {
//          TRACE("Receiver List Changed\n");
            pNameList->SetNamesChanged();
            CAddressBookNameList& ReceiverList = pData->GetReceiverList();
            DWORD dwReceiverCount = ReceiverList.GetSize();
            if (dwReceiverCount == 0)
            {
               pNameList->DeleteAllNames();
            }
            else
            {
               DWORD* pdwReceivers = &(ReceiverList.ElementAt(0));
               pNameList->PutAllNames(pdwReceivers, (int)dwReceiverCount);
            }
         }
      }
      END_TRY
   }

   // Free any data object that was allocated.
   delete pData;
   pData = NULL;
      
   return fResult;
}
// Get the full name of an address book given its user name.
// CMemoryException
void CAddressBook::GetAddressBookFullName(const CString& csUserName, CString& csFullName)
{
   CString csTrimmedUserName;
   TRY
   {
      csTrimmedUserName = csUserName;
      Util::Trim(csTrimmedUserName);
      csFullName = GetGlobalPathManager()->ExpandPath("[[A]]");
      csFullName += csTrimmedUserName;
      csFullName += ADDRESS_BOOK_EXTENSION;
   }
   CATCH_ALL(e)
   {
      // Some sort of error. Return an empty string.
      TRY
      {
         csFullName.Empty();
      }
      END_TRY

      THROW_LAST();
   }
   END_CATCH_ALL;
}
// Get the user name of an address book given its full name.
// CMemoryException
void CAddressBook::GetAddressBookUserName(const CString& csFullName, CString& csUserName)
{
   TRY
   {
      Util::SplitPath(csFullName, NULL, NULL, &csUserName, NULL);
      Util::Trim(csUserName);
   }
   CATCH_ALL(e)
   {
      // Some sort of error. Return an empty string.
      TRY
      {
         csUserName.Empty();
      }
      END_TRY

      THROW_LAST();
   }
   END_CATCH_ALL;
}
// Make sure a given user name is valid. It must be a valid base file name for the
// volume holding the PrintMaster address books.
// No exceptions.
BOOL CAddressBook::ValidateAddressBookUserName(const CString& csUserName, BOOL fReportError /*=FALSE*/)
{
   BOOL fResult = FALSE;

   CString csFullName;
   CString csDrive;
   CString csDirectory;
   CString csFile;
   CString csExtension;
   CString csPath;
   TRY
   {
      // First, make sure the name is not empty.
      if (csUserName.IsEmpty())
      {
         if (fReportError)
         {
            AfxMessageBox(IDS_EMPTY_ADDRESS_BOOK_USER_NAME);
         }
      }
      else
      {
         // We test the name by making the full name and then breaking it apart.
         // The elements should be consistent.
         GetAddressBookFullName(csUserName, csFullName);
         Util::SplitPath(csFullName, &csDrive, &csDirectory, &csFile, &csExtension);
         csPath = GetGlobalPathManager()->ExpandPath("[[A]]");
         if ((csDrive+csDirectory == csPath)
          && (csFile == csUserName)
          && (csExtension == ADDRESS_BOOK_EXTENSION)
          && (csFullName.GetLength() < _MAX_PATH))
         {
            fResult = TRUE;
         }
         
         if (!fResult)
         {
            if (fReportError)
            {
               CString csFormat1;
               CString csFormat2;
               CString csText;
               TRY
               {
                  csFormat1.LoadString(IDS_INVALID_ADDRESS_BOOK_USER_NAME_1);
                  csFormat2.LoadString(IDS_INVALID_ADDRESS_BOOK_USER_NAME_2);
                  csFormat1 += csFormat2;
                  csText.Format(csFormat1, (LPCSTR)csUserName);
                  AfxMessageBox(csText);
               }
               END_TRY
            }
         }
      }
   }
   CATCH_ALL(e)
   {
      fResult = FALSE;
   }
   END_CATCH_ALL

   return fResult;
}
// Get an array of the user names of the available address books.
// No exceptions.
BOOL CAddressBook::GetAddressBookUserNames(CStringArray& csaUserNames)
{
   CFileIterator FileIterator;
   CString csSearch;
   CString csFullName;
   CString csUserName;
   TRY
   {
      csaUserNames.RemoveAll();

      // Build the search path.
      csSearch = GetGlobalPathManager()->ExpandPath("[[A]]");
      csSearch += "*";
      csSearch += ADDRESS_BOOK_EXTENSION;

      // Search for the first file.
      BOOL fContinue = FileIterator.FindFirst(csSearch, _A_NORMAL);
      while (fContinue)
      {
         csFullName = FileIterator.Name();
         Util::SplitPath(csFullName, NULL, NULL, &csUserName, NULL);
         if (!csUserName.IsEmpty())
         {
            csaUserNames.Add(csUserName);
         }
         fContinue = FileIterator.FindNext();
      }
   }
   END_TRY

   return csaUserNames.GetSize() > 0;
}
// Check if any address books exist.
// No exceptions.
BOOL CAddressBook::AddressBooksExist(void)
{
   BOOL fResult = FALSE;

   CFileIterator FileIterator;
   CString csSearch;
   TRY
   {
      // Build the search path.
      csSearch = GetGlobalPathManager()->ExpandPath("[[A]]");
      csSearch += "*";
      csSearch += ADDRESS_BOOK_EXTENSION;

      // Search for the first file.
      fResult = FileIterator.FindFirst(csSearch, _A_NORMAL);
   }
   END_TRY

   return fResult;
}
// Create the standard address and sender books.
// No exceptions.
void CAddressBook::CreateStandardAddressBooks(void)
{
   CString csStandardName;
   StorageFile* pDatabaseFile = NULL;
   CFlatFileDatabase* pDatabase = NULL;

   // Create the standard address book.
   TRY
   {
      csStandardName = GET_PMWAPP()->AddressBook();
      OpenAddressBookDatabase(csStandardName, pDatabaseFile, pDatabase, TRUE);
   }
   END_TRY
   CloseAddressBookDatabase(pDatabaseFile, pDatabase);

   // Create the standard sender book.
   TRY
   {
      csStandardName = GET_PMWAPP()->SenderBook();
      OpenAddressBookSenderDatabase(csStandardName, pDatabaseFile, pDatabase, TRUE);
   }
   END_TRY
   CloseAddressBookDatabase(pDatabaseFile, pDatabase);
}
// Open (and create) an address book database.
// No exceptions.
ERRORCODE CAddressBook::OpenAddressBookDatabase(
   LPCSTR pszFullName,
   StorageFile*& pDatabaseFile,
   CFlatFileDatabase*& pDatabase,
   BOOL fCreate /*=FALSE*/,
   LPCSTR pszDescription /*=NULL*/,
   CFlatFileFieldDescription* pFieldDescriptions /*=NULL*/,
   CFlatFileIndexDescription* pIndexDescriptions /*=NULL*/)
{
   ERRORCODE Error = ERRORCODE_None;

   // Clean up any existing data.
   CloseAddressBookDatabase(pDatabaseFile, pDatabase);

   CString csFullName;
   CString csPath;
   TRY
   {
      // Make sure the directory exists.
      csFullName = GetGlobalPathManager()->ExpandPath(pszFullName);
      Util::SplitPath(csFullName, &csPath, NULL);
      Util::MakeDirectory(csPath);
      
      // Create the file object.
      pDatabaseFile = new StorageFile(pszFullName);

      // Create the database object.
      pDatabase = new CFlatFileDatabase(pDatabaseFile);

      // Try to open the database exclusively and perform validation.
      pDatabaseFile->set_open_flags(CFile::modeReadWrite | CFile::shareExclusive);
      Error = pDatabase->read_state(TRUE, TRUE);
      pDatabaseFile->flush();

      // If the file does not exist, attempt to intialize the database.
      if ((Error == ERRORCODE_DoesNotExist) && (fCreate))
      {
         // Create the file.
         if ((Error = pDatabaseFile->initialize()) == ERRORCODE_None)
         {
            // Assign defaults.
            if (pszDescription == NULL)
            {
               // JRO pszDescription = szStandardDatabaseDescription;
               pszDescription = GET_PMWAPP()->GetResourceStringPointer (IDS_STAN_DBASE_DESC);
            }
            if (pFieldDescriptions == NULL)
            {
#ifdef LOCALIZE
				pFieldDescriptions = LocalDatabaseFieldDescriptions(StandardDatabaseFieldDescriptions);
#else
				pFieldDescriptions = StandardDatabaseFieldDescriptions;
#endif

            }
            if (pIndexDescriptions == NULL)
            {
#ifdef LOCALIZE
				pIndexDescriptions = LocalDatabaseIndexDescriptions(StandardDatabaseIndexDescriptions);
#else
				pIndexDescriptions = StandardDatabaseIndexDescriptions;
#endif

            }

            // Make the creation data.
            CFlatFileHeaderRecord::CCreationData CreationData(
               (LPVOID)(LPSTR)pszDescription,
               strlen(pszDescription)+1,
               pFieldDescriptions,
               pIndexDescriptions);

            // Initialize the database and then close it.
            if (((Error = pDatabase->initialize(0, &CreationData)) != ERRORCODE_None)
             || ((Error = pDatabase->flush()) != ERRORCODE_None)
             || ((Error = pDatabase->write_state()) != ERRORCODE_None)
             || ((Error = pDatabaseFile->flush()) != ERRORCODE_None))
            {
               // Got the file, but database error. Delete the file and return the error.
               pDatabaseFile->zap();
            }
         }
      }

      if (Error == ERRORCODE_None)
      {
         // Try to open the database in sharing mode.
         pDatabaseFile->set_open_flags(CFile::modeReadWrite | CFile::shareDenyNone);
         if ((Error = pDatabase->read_state()) == ERRORCODE_None)
         {
            Error = pDatabase->Attach();
         }
      }
   }
   CATCH_ALL(e)
   {
      if (Error == ERRORCODE_None)
      {
         Error = ERRORCODE_IntError;
      }
   }
   END_CATCH_ALL

   if (Error != ERRORCODE_None)
   {
      CloseAddressBookDatabase(pDatabaseFile, pDatabase);
   }

   return Error;
}

// Open (and create) an address book sender database.
// No exceptions.
ERRORCODE CAddressBook::OpenAddressBookSenderDatabase(
      LPCSTR pszFullName,
      StorageFile*& pDatabaseFile,
      CFlatFileDatabase*& pDatabase,
      BOOL fCreate /*=FALSE*/)
{
   // Use OpenAddressBookDatabase() with the proper default field and index descriptions.
   return OpenAddressBookDatabase(
      pszFullName,
      pDatabaseFile,
      pDatabase,
      fCreate,
      // JRO szStandardSenderDatabaseDescription,
      GET_PMWAPP()->GetResourceStringPointer (IDS_STAN_DBASE_DESC_SENDER),
#ifdef LOCALIZE
      LocalDatabaseFieldDescriptions(StandardSenderDatabaseFieldDescriptions),
      LocalDatabaseIndexDescriptions(StandardSenderDatabaseIndexDescriptions));
#else
      StandardSenderDatabaseFieldDescriptions,
      StandardSenderDatabaseIndexDescriptions);
#endif
}
// Close an address book database.
// No exceptions.
void CAddressBook::CloseAddressBookDatabase(StorageFile*& pDatabaseFile, CFlatFileDatabase*& pDatabase)
{
   // Close and delete the database object.
   TRY
   {
      if (pDatabase != NULL)
      {
         pDatabase->flush();
         pDatabase->write_state(TRUE);
         delete pDatabase;
      }
   }
   END_TRY

   // Close and delete the file object.
   TRY
   {
      delete pDatabaseFile;
   }
   END_TRY

   // Always NULL the pointers.
   pDatabase = NULL;
   pDatabaseFile = NULL;
}
// Convert a version 2.0 standard index name.
void CAddressBook::ConvertVersion2IndexName(LPCSTR pszVersion2, CString& csIndexName)
{
   char cbIndexName[128];
   LPSTR pszIndexName = &(cbIndexName[0]);

   BOOL fFirst = TRUE;
   
   while (*pszVersion2 != '\0')
   {
      // Don't copy over "Index".
      if (stricmp(pszVersion2, "INDEX") == 0)
      {
         break;
      }
      if (IsCharUpper(*pszVersion2))
      {
         if (!fFirst)
         {
            *pszIndexName++ = ' ';
         }
         else
         {
            fFirst = FALSE;
         }
      }
      *pszIndexName++ = *pszVersion2++;
   }
   *pszIndexName = '\0';

   TRY
   {
      csIndexName = cbIndexName;
   }
   END_TRY
}
//////////////////////////////////////////////////////////////////////////
// CSenderData
// Class used to communicate with sender dialogs. Encapsulates input and
// output data.
CSenderData::CSenderData()
{
   m_dwSenderId = (DWORD)-1;
   ClearChanged();
}

CSenderData::CSenderData(LPCSTR pszSenderBookFullName, DWORD dwSenderId /*=(DWORD)-1*/)
{
   SetSenderBookFullName(pszSenderBookFullName);
   SetSenderId(dwSenderId);
   ClearChanged();
}

CSenderData::~CSenderData()
{
}

void CSenderData::SetSenderBookFullName(LPCSTR pszSenderBookFullName)
{
   if (pszSenderBookFullName == NULL)
   {
      if (!m_csSenderBookFullName.IsEmpty())
      {
         TRY
         {
            m_csSenderBookFullName = pszSenderBookFullName;
            SetSenderBookFullNameChanged();
         }
         END_TRY
      }
   }
   else
   {
      if (m_csSenderBookFullName != pszSenderBookFullName)
      {
         TRY
         {
            m_csSenderBookFullName = pszSenderBookFullName;
            SetSenderBookFullNameChanged();
         }
         END_TRY
      }
   }
}

void CSenderData::SetSenderId(DWORD dwSenderId)
{
   if (dwSenderId != m_dwSenderId)
   {
      m_dwSenderId = dwSenderId;
      SetSenderIdChanged();
   }
}

void CSenderData::ClearChanged(void)
{
   SetSenderBookFullNameChanged(FALSE);
   SetSenderIdChanged(FALSE);
	SetSenderEdited(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CSelectSenderDialog
CSelectSenderDialog::CSelectSenderDialog(CSenderData* pData, CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(CSelectSenderDialog::IDD, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(CSelectSenderDialog)
   //}}AFX_DATA_INIT

   m_pData = pData;
   m_pDatabaseFile = NULL;
   m_pDatabase = NULL;
   m_pCursor = NULL;
   m_fFirstPaint = TRUE;
}

CSelectSenderDialog::~CSelectSenderDialog()
{
   CloseDatabase();
   delete m_pCursor;
   delete m_pDatabase;
   delete m_pDatabaseFile;
}

void CSelectSenderDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSelectSenderDialog)
   DDX_Control(pDX, IDC_SENDER_LIST, m_SenderList);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectSenderDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CSelectSenderDialog)
   ON_UPDATE_COMMAND_UI(IDOK, OnUpdateEnableIfSelected)
   ON_WM_PAINT()
   ON_BN_CLICKED(IDC_NEW, OnNew)
   ON_BN_CLICKED(IDC_DELETE, OnDelete)
   ON_BN_CLICKED(IDC_EDIT, OnEdit)
   ON_UPDATE_COMMAND_UI(IDC_DELETE, OnUpdateEnableIfSelected)
   ON_UPDATE_COMMAND_UI(IDC_EDIT, OnUpdateEnableIfSelected)
   ON_LBN_DBLCLK(IDC_SENDER_LIST, OnDblclkSenderList)
   //}}AFX_MSG_MAP
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
   ON_MESSAGE(WM_SENDER_DATABASE_NOT_EMPTY, OnSenderDatabaseNotEmpty)
   ON_MESSAGE(WM_SENDER_DATABASE_EMPTY, OnSenderDatabaseEmpty)
END_MESSAGE_MAP()
// Save position and other information before window is destroyed.
BOOL CSelectSenderDialog::DestroyWindow()
{
   // finally, destroy this window
   return INHERITED::DestroyWindow();
}

void CSelectSenderDialog::UpdateNameList(void)
{
   m_SenderList.SetRedraw(FALSE);
   m_SenderList.ResetContent();

   TRY
   {
      if (m_pCursor != NULL)
      {
         CFlatFileDatabaseField* pField = m_pCursor->GetField("__ID__");
         if (pField != NULL)
         {
            m_pCursor->First();
            while (!m_pCursor->AtEOF())
            {
               CString csName;
               TRY
               {
                  GetDisplayName(m_pCursor, csName);
                  int nIndex = m_SenderList.AddString(csName);
                  if (nIndex != LB_ERR)
                  {
                     m_SenderList.SetItemData(nIndex, pField->GetAsUnsigned32());
                  }
               }
               END_TRY
               m_pCursor->Next();
            }
         }
      }
   }
   END_TRY

   m_SenderList.SetRedraw(TRUE);
   m_SenderList.Invalidate();
}

void CSelectSenderDialog::GetDisplayName(CFlatFileDatabaseCursor* pCursor, CString& csName)
{
   TRY
   {
      csName.Empty();

      if (pCursor != NULL)
      {
         // Use first and last name.
		 // CB May 15 1998 removed hard-coded strings for localization.
//		  CFlatFileDatabaseField* pFirstNameField = pCursor->GetField("First Name");
//         CFlatFileDatabaseField* pLastNameField = pCursor->GetField("Last Name");
		 csName.LoadString(IDS_LOC_FIRSTNAME);
		 CFlatFileDatabaseField* pFirstNameField = pCursor->GetField(csName);
		 csName.LoadString(IDS_LOC_LASTNAME);
         CFlatFileDatabaseField* pLastNameField = pCursor->GetField(csName);
		 csName.Empty();
         if ((pFirstNameField != NULL) && (pLastNameField != NULL))
         {
            csName += pFirstNameField->GetAsString();
            if (!csName.IsEmpty())
            {
               csName += ' ';
            }
            csName += pLastNameField->GetAsString();
         }

         // If the name is empty, use something!
         if (csName.IsEmpty())
         {
            csName.LoadString(IDS_EMPTY_NAME);
         }
      }
   }
   END_TRY
}

BOOL CSelectSenderDialog::SelectItemWithID(DWORD dwID /*=0*/)
{
   BOOL fResult = FALSE;

   if (m_pData != NULL)
   {
      // Use ID from data as default.
      if (dwID == 0)
      {
         dwID = m_pData->GetSenderId();
      }
      if (dwID != 0)
      {
         int nCount = m_SenderList.GetCount();
         if (nCount != LB_ERR)
         {
            for (int nIndex = 0; nIndex < nCount; nIndex++)
            {
               DWORD dwID = m_SenderList.GetItemData(nIndex);
               if (dwID != (DWORD)LB_ERR)
               {
                  if (dwID == m_pData->GetSenderId())
                  {
                     m_SenderList.SetCurSel(nIndex);
                     fResult = TRUE;
                  }
               }
            }
         }
      }
   }

   return fResult;
}

DWORD CSelectSenderDialog::GetIdOfSelectedItem(void)
{
   DWORD dwID = 0;

   int nIndex = m_SenderList.GetCurSel();
   if (nIndex != LB_ERR)
   {
      dwID = m_SenderList.GetItemData(nIndex);
      if (dwID == (DWORD)LB_ERR)
      {
         dwID = 0;
      }
   }

   return dwID;
}

BOOL CSelectSenderDialog::PositionCursorAtIdOfSelectedItem(void)
{
   BOOL fResult = FALSE;

   DWORD dwID = GetIdOfSelectedItem();
   if (dwID != 0)
   {
      TRY
      {
         if (m_pCursor != NULL)
         {
            CFlatFileDatabaseField* pField = m_pCursor->GetField("__ID__");
            if (pField != NULL)
            {
               m_pCursor->New();
               pField->SetAsUnsigned32(dwID);
               BOOL fExact = FALSE;
               m_pCursor->Find(&fExact);
               if (fExact)
               {
                  fResult = TRUE;
               }
            }
         }
      }
      END_TRY
   }

   return fResult;
}

void CSelectSenderDialog::SetupFieldsForNewSender(CFlatFileDatabaseCursor* pCursor)
{
   ASSERT(pCursor != NULL);
   if (pCursor != NULL)
   {
      // Initialize all fields to blank.
      pCursor->New();

#if 0
// This was confusing users (at least QA).
// So it has been removed.

      // Initialize some fields from MSREGUSR.INI.
      CIniFile IniFile;
      TRY
      {
         IniFile.Name("MSREGUSR.INI");
// CB May 15 1998 changed to get field strings from resource file
	     CString csTemp;
		 csTemp.LoadString(IDS_LOC_FIRSTNAME);
         SetupFieldFromIniFile(pCursor, csTemp, IniFile, "CustomerInformation", "FirstName");
		 csTemp.LoadString(IDS_LOC_LASTNAME);
         SetupFieldFromIniFile(pCursor, csTemp,  IniFile, "CustomerInformation", "LastName");
		 csTemp.LoadString(IDS_LOC_TITLE);
         SetupFieldFromIniFile(pCursor, csTemp,      IniFile, "CustomerInformation", "Title");
		 csTemp.LoadString(IDS_LOC_COMPANY);
         SetupFieldFromIniFile(pCursor, csTemp,    IniFile, "CustomerInformation", "Company");
		 csTemp.LoadString(IDS_LOC_ADDRESS1);
         SetupFieldFromIniFile(pCursor, csTemp,   IniFile, "CustomerInformation", "Address1");
		 csTemp.LoadString(IDS_LOC_ADDRESS2);
         SetupFieldFromIniFile(pCursor, csTemp,   IniFile, "CustomerInformation", "Address2");
		 csTemp.LoadString(IDS_LOC_CITY);
         SetupFieldFromIniFile(pCursor, csTemp,       IniFile, "CustomerInformation", "City");
		 csTemp.LoadString(IDS_LOC_STATE);
         SetupFieldFromIniFile(pCursor, csTemp,      IniFile, "CustomerInformation", "State");
		 csTemp.LoadString(IDS_LOC_ZIP);
         SetupFieldFromIniFile(pCursor, csTemp,        IniFile, "CustomerInformation", "Zip");
		 csTemp.LoadString(IDS_LOC_PHONE);
         SetupFieldFromIniFile(pCursor, csTemp,      IniFile, "CustomerInformation", "Phone");
		 csTemp.LoadString(IDS_LOC_EMAIL);
         SetupFieldFromIniFile(pCursor, csTemp,      IniFile, "CustomerInformation", "Email");
      }
      END_TRY
#endif
   }
}

void CSelectSenderDialog::SetupFieldFromIniFile(CFlatFileDatabaseCursor* pCursor, LPCSTR pszField, CIniFile& IniFile, LPCSTR pszSection, LPCSTR pszEntry)
{
   CString csValue;
   TRY
   {
      CFlatFileDatabaseField* pField = pCursor->GetField(pszField);
      if (pField != NULL)
      {
         csValue = IniFile.GetString(pszSection, pszEntry, NULL);
// cb moved this line inside the if statement. 
         pField->SetAsString(csValue);
      }
   }
   END_TRY
}
/////////////////////////////////////////////////////////////////////////////
// CSelectSenderDialog message handlers
BOOL CSelectSenderDialog::OnInitDialog() 
{
   TRY
   {
      // Attempt to access the sender database.
      if (!SetDatabase(m_pData->GetSenderBookFullName()))
      {
         // The database could not be accessed.
         // Let the user know and bail out.
         AfxThrowUserException();
      }
      else
      {
         // Use the "ID" index.
         m_pCursor->SetIndex("__ID__");

         // Check if the database is empty. If not, then fill the list box.
         if (m_pCursor->GetRecordCount() > 0)
         {
            // We are going to run the dialog, create and initialize it.
            INHERITED::OnInitDialog();

            m_NewButton.Initialize(IDC_NEW, this);
            m_DeleteButton.Initialize(IDC_DELETE, this);
            m_EditButton.Initialize(IDC_EDIT, this);

            UpdateNameList();
         }
         else
         {
            // The database is empty. Post ourselves a message so we
            // can bring up the editing dialog directly without showing
            // the select sender dialog.
            PostMessage(WM_SENDER_DATABASE_EMPTY, 0, 0);
         }
      }
   }
   CATCH_ALL(e)
   {
      AfxMessageBox(IDS_SENDER_DATABASE_NOT_AVAILABLE);
      Util::PostWmCommand(this, IDCANCEL, NULL, 0);
   }
   END_CATCH_ALL

   return FALSE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}
// The following CmdUI functions are to enable and diables various user interface
// elements as needed.
LRESULT CSelectSenderDialog::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
   // PrintMaster sends this message to us (MAINFRM.CPP) so we know
   // when to update all our controls.
   UpdateDialogControls(this, wParam);
   return 0L;
}
// Enable a button if there is a current selection.
void CSelectSenderDialog::OnUpdateEnableIfSelected(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetIdOfSelectedItem() != 0);
}

void CSelectSenderDialog::OnOK() 
{
   // The user wants to use the currently selected list box item
   // for the sender information.
   if (m_pData != NULL)
   {
      DWORD dwID = GetIdOfSelectedItem();
      if (dwID != 0)
      {
         m_pData->SetSenderId(dwID);
      }
   }

   INHERITED::OnOK();
}

void CSelectSenderDialog::OnCancel() 
{
   // The user wants to return without changing the selected sender.
   INHERITED::OnCancel();
}

BOOL CSelectSenderDialog::SetDatabase(LPCSTR pszFullName)
{
   ERRORCODE Error = ERRORCODE_None;

   // Shut down any existing database.
   CloseDatabase();

   // Remember the current name.
   m_pData->SetSenderBookFullName(pszFullName);

   // Attempt to open the new database.
   TRY
   {
      // Open the database.
      Error = CAddressBook::OpenAddressBookSenderDatabase(pszFullName, m_pDatabaseFile, m_pDatabase, TRUE);

      if (Error == ERRORCODE_None)
      {
         // Create a cursor on the database.
         m_pCursor = new CFlatFileDatabaseCursor;
         m_pCursor->Attach(m_pDatabase);
      }
   }
   CATCH_ALL(e)
   {
      Error = ERRORCODE_IntError;
   }
   END_CATCH_ALL

   // If we got an error, clean up.
   if (Error != ERRORCODE_None)
   {
      CloseDatabase();
   }

   return Error == ERRORCODE_None;
}

void CSelectSenderDialog::CloseDatabase(void)
{
   // Delete the cursor object.
   delete m_pCursor;
   m_pCursor = NULL;

   // Close and delete the database object and file objects.
   CAddressBook::CloseAddressBookDatabase(m_pDatabaseFile, m_pDatabase);
}

LRESULT CSelectSenderDialog::OnSenderDatabaseNotEmpty(WPARAM wParam, LPARAM)
{
   // Check if the ID that was passed is actually in the sender database.
   // If not, then warn the user.
   if (!SelectItemWithID())
   {
      m_SenderList.SetCurSel(-1);
      CSenderInformationMissingDialog Dialog(this);
      Dialog.DoModal();
   }

   return 0L;
}

LRESULT CSelectSenderDialog::OnSenderDatabaseEmpty(WPARAM wParam, LPARAM)
{
   // The sender address book is empty. The user is given the opportunity
   // to provide new sender information.
   int nResult = IDCANCEL;
   TRY
   {
      SetupFieldsForNewSender(m_pCursor);

      CSenderInformation Dialog(m_pData, m_pCursor, TRUE, this);
      if (Dialog.DoModal() == IDOK)
      {
         // Apparently we have new sender information.
         CFlatFileDatabaseField* pField = m_pCursor->GetField("__ID__");
         if (pField != NULL)
         {
            // Save ID of new sender.
            m_pData->SetSenderId(pField->GetAsUnsigned32());
            nResult = IDOK;
         }
      }
   }
   END_TRY

   // Finish the dialog with the appropriate result.
   EndDialog(nResult);
   return 0L;
}

void CSelectSenderDialog::OnPaint() 
{
   Default();

   if (m_fFirstPaint)
   {
      PostMessage(WM_SENDER_DATABASE_NOT_EMPTY, 0, 0);
      m_fFirstPaint = FALSE;
   }
}


void CSelectSenderDialog::OnNew() 
{
   TRY
   {
      SetupFieldsForNewSender(m_pCursor);
      CSenderInformation Dialog(m_pData, m_pCursor, FALSE, this);
      if (Dialog.DoModal() == IDOK)
      {
         // Apparently we have new sender information.
         CFlatFileDatabaseField* pField = m_pCursor->GetField("__ID__");
         if (pField != NULL)
         {
            m_pData->SetSenderId(pField->GetAsUnsigned32());
            UpdateNameList();
            SelectItemWithID();
         }
      }
   }
   END_TRY
}

void CSelectSenderDialog::OnDelete() 
{
   // Confirm a destructive act such as this.
   if (ConfirmDelete())
   {
      // The user really wants to delete the sender information. So, DO IT!
      if (PositionCursorAtIdOfSelectedItem())
      {
         m_pCursor->Delete(TRUE);

         // Update the list box.
         int nIndex = m_SenderList.GetCurSel();
         UpdateNameList();
         if (nIndex != LB_ERR)
         {
            int nCount = m_SenderList.GetCount();
            if (nCount == LB_ERR)
            {
               nIndex = -1;
            }
            else
            {
               if (nIndex >= nCount)
               {
                  nIndex = nCount-1;
               }
            }
            m_SenderList.SetCurSel(nIndex);
         }
      }
   }
}
// Ask the user if they REALLY want to delete. This question could get annoying.
// Perhaps there should be a "never ask me again" check box?
BOOL CSelectSenderDialog::ConfirmDelete(void)
{
   BOOL fResult = FALSE;

   int nIndex = m_SenderList.GetCurSel();
   if (nIndex != LB_ERR)
   {
      CConfirmDeleteSenderDialog Dialog(this);
      TRY
      {
         m_SenderList.GetText(nIndex, Dialog.m_csName);
      }
      END_TRY

      fResult = Dialog.DoModal() == IDOK;
   }

   return fResult;
}

void CSelectSenderDialog::OnEdit() 
{
   // Move the cursor to the current record.
   if (PositionCursorAtIdOfSelectedItem())
   {
      CSenderInformation Dialog(m_pData, m_pCursor, FALSE, this);
      Dialog.DoModal();
		// Assume it has always been changed unless we can somehow tell.
		m_pData->SetSenderEdited();
   }
}

void CSelectSenderDialog::OnDblclkSenderList() 
{
   OnOK();
}
// Run the select sender dialog.
// Returns TRUE if the sender was changed.
BOOL CSelectSenderDialog::Run(CWnd* pParent)
{
   BOOL fResult = FALSE;
   CSenderData* pData = NULL;

   // Allocate and initialize the sender data.
   CIniFile IniFile;
   CString csSenderBookFullName;
   DWORD dwSenderId;
   TRY
   {
      // Get the sender book full name.
      csSenderBookFullName = GET_PMWAPP()->SenderBook();

      // Get the sender ID.
      dwSenderId = GET_PMWAPP()->GetSenderId();

      // Create the data.
      pData = new CSenderData(csSenderBookFullName, dwSenderId);
   }
   END_TRY

   if (pData != NULL)
   {
      CSelectSenderDialog SelectSender(pData, pParent);
      SelectSender.DoModal();
      fResult = pData->GetChanged();
   }

   // If the sender ID changed, write it out.
   if (pData->GetSenderIdChanged())
   {
      GET_PMWAPP()->SetSenderId(pData->GetSenderId());
   }

   // Free any data object that was allocated.
   delete pData;
   pData = NULL;
      
   return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CSenderInformation

CSenderInformation::CSenderInformation(CSenderData* pData, CFlatFileDatabaseCursor* pCursor, BOOL fInformationMissing, CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(CSenderInformation::IDD, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(CSenderInformation)
   //}}AFX_DATA_INIT

   TRY
   {
      m_nModalResult = -1;
      m_pData = pData;
      m_pCursor = pCursor;
      m_fSizesValid = FALSE;
      m_fAddingNewRecord = pCursor->GetPosition() == (DWORD)-1;
      m_fInformationMissing = fInformationMissing;
      m_fFirstPaint = TRUE;
   }
   END_TRY
}

CSenderInformation::~CSenderInformation()
{
}

void CSenderInformation::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSenderInformation)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSenderInformation, CPmwDialogColor)
   //{{AFX_MSG_MAP(CSenderInformation)
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_LBN_SETFOCUS(IDC_ADDRESS_BOOK_FIELD_LIST, OnSetfocusAddressBookFieldList)
   ON_WM_ERASEBKGND()
   ON_WM_PAINT()
   //}}AFX_MSG_MAP
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
   ON_MESSAGE(WM_GET_FIELD_LIST, OnGetFieldList)
   ON_MESSAGE(WM_SENDER_DATABASE_EMPTY, OnSenderDatabaseEmpty)
END_MESSAGE_MAP()

// Run the address book as a modal dialog.
int CSenderInformation::DoModal()
{
   if (Create(m_lpszTemplateName, AfxGetMainWnd()))
   {
      m_pParentWnd->EnableWindow(FALSE);

      EnableWindow(TRUE);

      BOOL fContinue = TRUE;
      while (fContinue)
      {
         MSG Msg;
         while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
         {
            if (!::GetMessage(&Msg, NULL, NULL, NULL))
            {
               fContinue = FALSE;
               ::PostQuitMessage(0);
               break;
            }

            // Send a cancel mode out to the currently focused control. This gives our
            // list box editors a chance to pull up their list boxes.
            if ((Msg.message == WM_LBUTTONDOWN)
             || (Msg.message == WM_NCLBUTTONDOWN))
            {
               CWnd* pFocus = GetFocus();
               if (pFocus != NULL)
               {
                  // only send the message if the window receiving the click is
                  // not the combo button or list box.
                  char szClassName[64];
                  szClassName[0] = '\0';
                  if (Msg.hwnd != NULL)
                  {
                     ::GetClassName(Msg.hwnd, szClassName, sizeof(szClassName)-1);
                  }
                  if ((stricmp(szClassName, "MLSComboButton") != 0)
                   && (stricmp(szClassName, "MLSComboList") != 0))
                  {
                     pFocus->SendMessage(WM_CANCELMODE, 0, 0);
                  }
               }
            }

            // Note that fContinue may be set to FALSE, but we continue to process message
            // until we run out and then we return from DoModal.
            fContinue = GetSafeHwnd() != NULL;

            if (!(fContinue && IsDialogMessage(&Msg)))
            {
               ::TranslateMessage(&Msg);
               ::DispatchMessage(&Msg);
            }
         }

         fContinue = fContinue && (GetSafeHwnd() != NULL);

         if (fContinue)
         {

            // send WM_ENTERIDLE since queue is empty
            if ((GetParent() != NULL) && ((GetStyle() & DS_NOIDLEMSG) == 0))
            {
               GetParent()->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)m_hWnd);
            }
         }
      }

      m_pParentWnd->EnableWindow(TRUE);
   }

   return m_nModalResult;
}

// Save position and other information before window is destroyed.
BOOL CSenderInformation::DestroyWindow()
{
   // Save address book window information.
   SaveInformation();

   // re-enable parent if it was disabled
   m_pParentWnd->EnableWindow(TRUE);

   // hide this window and move activation to the parent
   SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

   m_pParentWnd->SetActiveWindow();

   // finally, destroy this window
   return INHERITED::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CSenderInformation message handlers

BOOL CSenderInformation::OnInitDialog() 
{
   INHERITED::OnInitDialog();

   m_FieldList.SubclassDlgItem(IDC_ADDRESS_BOOK_FIELD_LIST, this);

   // Add the proper border.
   m_FieldList.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

   // Turn on "clip siblings" for all children.
#if 0
   for (CWnd* pChild = GetWindow(GW_CHILD); pChild != NULL; pChild = pChild->GetWindow(GW_HWNDNEXT))
   {
      DWORD dwStyle = pChild->GetStyle();
      ::SetWindowLong(pChild->GetSafeHwnd(), GWL_STYLE, dwStyle | WS_CLIPSIBLINGS);
   }
#endif

   // Compute some layout information that will be used for sizing.
   CRect crDialogWindow;
   GetWindowRect(crDialogWindow);

   m_nMinimumWidth = crDialogWindow.Width();
   m_nMinimumHeight = crDialogWindow.Height();

   CRect crDialogClient;
   GetClientRect(crDialogClient);

   m_nCurrentWidth = crDialogClient.Width();
   m_nCurrentHeight = crDialogClient.Height();

   m_fSizesValid = TRUE;

   // Restore any saved information including the saved position.
   RestoreInformation();

   RefreshFieldInformation();

   m_FieldList.Refresh();

   if (m_fAddingNewRecord)
   {
      m_FieldList.Edit(0);
      return FALSE;
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}
// Save all the information we might want to restore for the next addres book session.
BOOL CSenderInformation::SaveInformation(void)
{
   BOOL fResult = FALSE;

   CIniFile IniFile;

   TRY
   {
      // Get the INI file to use.
		IniFile.Name(GET_PMWAPP()->m_pszProfileName);
//    if (GET_PMWAPP()->InitPrintMasterIniFile(&IniFile, "_000PMW.INI", TRUE))
      {
         // Write out the position information.
         IniFile.WriteWindowPosition("SenderInformationDialog", this);

         // Write out the splitter information
         if (SaveSplitterInformation(IniFile))
         {
            fResult = TRUE;
         }
      }
   }
   END_TRY

   return fResult;
}
// Save splitter position information.
BOOL CSenderInformation::SaveSplitterInformation(CIniFile& IniFile)
{
   // Convert splitter to relative size.
   int nSplitter = m_FieldList.GetSplitter();
   CRect crFieldList;
   m_FieldList.GetWindowRect(crFieldList);
   int nWidth = crFieldList.Width();
   CFixed lFraction = SplitterToFraction(nSplitter, nWidth);
   IniFile.WriteLong("SenderInformationDialog", "FieldListSplitterFraction", (long)lFraction);

   return TRUE;
}
// Load saved address book information.
BOOL CSenderInformation::RestoreInformation(void)
{
   BOOL fResult = FALSE;

   CIniFile IniFile;

   TRY
   {
      // Get the INI file to use.
		IniFile.Name(GET_PMWAPP()->m_pszProfileName);
//    if (GET_PMWAPP()->InitPrintMasterIniFile(&IniFile, "_000PMW.INI", TRUE))
      {
         // Restore the position information.
         BOOL fIsMaximized = FALSE;
         IniFile.GetWindowPosition("SenderInformationDialog", this);

         // Restore the splitter information.
         if (RestoreSplitterInformation(IniFile))
         {
            fResult = TRUE;
         }
      }
   }
   END_TRY

   return fResult;
}
// Load saved splitter information.
BOOL CSenderInformation::RestoreSplitterInformation(CIniFile& IniFile)
{
   // Convert relative size to splitter.
   CFixed lFraction = (CFixed)(IniFile.GetLong("SenderInformationDialog", "FieldListSplitterFraction", UNDEFINED_CFIXED));
   if (lFraction != UNDEFINED_CFIXED)
   {
      CRect crFieldList;
      m_FieldList.GetWindowRect(crFieldList);
      int nWidth = crFieldList.Width();
      int nSplitter = FractionToSplitter(lFraction, nWidth);
      m_FieldList.SetSplitter(nSplitter);
   }

   return TRUE;
}

BOOL CSenderInformation::OnEraseBkgnd(CDC* pDC)
{
   BOOL fResult = FALSE;
   
   // Exclude all the child windows for cleaner redraw.
   // Setting WS_CLIPCHILDREN should do this, but it
   // doesn't work right under NT 4.0.
   if (pDC->SaveDC() != 0)
   {
      for (CWnd* pChild = GetWindow(GW_CHILD); pChild != NULL; pChild = pChild->GetWindow(GW_HWNDNEXT))
      {
         CRect crChild;
         pChild->GetWindowRect(crChild);
         ScreenToClient(crChild);
         pDC->ExcludeClipRect(crChild);
      }
   
      fResult = INHERITED::OnEraseBkgnd(pDC);
      
      pDC->RestoreDC(-1);
   }
   
   return fResult;
}
// The following CmdUI functions are to enable and diables various user interface
// elements as needed.
LRESULT CSenderInformation::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
   // PrintMaster sends this message to us (MAINFRM.CPP) so we know
   // when to update all our controls.
   UpdateDialogControls(this, wParam);
   return 0L;
}

void CSenderInformation::OnOK() 
{
   // The user wants to save the new sender information and return.
   m_nModalResult = IDCANCEL;

   TRY
   {
      // If there is data to save, save it.
      if (m_FieldList.IsModified())
      {
         // Update the fields with the new values.
         m_FieldList.Save();

         // Save the record back to the database.
         if (m_fAddingNewRecord)
         {
            // Check each of the fields in the new record. If they are all empty,
            // then do not save the record.
            if (!FieldsBlank())
            {
               m_pCursor->SetCounters();
               m_pCursor->Insert();
               m_nModalResult = IDOK;
            }
            else
            {
               AfxMessageBox(IDS_SENDER_FIELDS_BLANK);
               m_FieldList.Edit(0);
               return;
            }
         }
         else
         {
            m_pCursor->Update();
            m_nModalResult = IDOK;
         }
      }
   }
   END_TRY

   DestroyWindow();
}

void CSenderInformation::OnCancel() 
{
   // The user want to quit without saving their sender information.
   // Should we confirm?
   m_nModalResult = IDCANCEL;
   DestroyWindow();
}

void CSenderInformation::RefreshFieldInformation(void)
{
   // Refresh the field list.
   m_FieldList.SetRedraw(FALSE);
   m_FieldList.ResetContent();
   if (m_pCursor != NULL)
   {
      DWORD dwFieldCount = m_pCursor->GetFieldCount();

      for (DWORD dwField = 0; dwField < dwFieldCount; dwField++)
      {
         if (FIELD_USER_DATA_EDITOR(m_pCursor->GetField(dwField)->GetDescription()->GetUserData()) != FIELD_EDIT_NONE)
         {
            CAddressBookFieldListBoxItem* pItem = new CAddressBookFieldListBoxItem(&m_FieldList, m_pCursor->GetField(dwField));
            m_FieldList.AddString((LPCSTR)pItem);
         }
      }
   }
   m_FieldList.SetRedraw(TRUE);
}
// When the address book is sized, we have to move all the controls around.
// At the moment, this process is table driven. This allows us to move or expand
// controls by fixed amounts. Centering is not possible with the current code
// because round-off errors would occur and no "original" position information
// is maintained. It could be added if necessary.
void CSenderInformation::OnSize(UINT nType, int cx, int cy) 
{
   // In OnSize(), cx and cy are client area dimensions.

   if (m_fSizesValid)
   {
      // Control movement table.
      static ControlMovement Movement[] =
      {
         {  IDC_ADDRESS_BOOK_FIELD_LIST,     ControlMovement::EX|ControlMovement::EY      },
         {  IDOK,                            ControlMovement::MX|ControlMovement::MY      },
         {  IDCANCEL,                        ControlMovement::MX|ControlMovement::MY      },
         {  IDC_FIELD_LIST_LABEL,            ControlMovement::EX                          },
         {  0,                               0                                            }
      };

      // Compute the size deltas. In theory, the inherited OnSize could have changed
      // the client size, so we can't use cx and cy.
      CRect crNew;
      GetClientRect(crNew);

      MoveControls(this, crNew.Width()-m_nCurrentWidth, crNew.Height()-m_nCurrentHeight, &(Movement[0]));
   }
   // Redraw it all.
   Invalidate();

   // Save the new sizes.
   m_nCurrentWidth = cx;
   m_nCurrentHeight = cy;
}
// Don't let the window get any smaller than the original design dimensions.
// So, the dialog layed out in App Studio is the minimum dimension address book.
void CSenderInformation::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
   INHERITED::OnGetMinMaxInfo(lpMMI);

   if (m_fSizesValid)
   {
      lpMMI->ptMinTrackSize.x = m_nMinimumWidth;
      lpMMI->ptMinTrackSize.y = m_nMinimumHeight;
   }
}
// The address book field list box was focused, start editing fields.
void CSenderInformation::OnSetfocusAddressBookFieldList() 
{
   m_FieldList.Edit();
}

LRESULT CSenderInformation::OnGetFieldList(WPARAM wParam, LPARAM lParam)
{
   return (LRESULT)(m_ListManager.GetListForField((CFlatFileFieldDescription*)lParam));
}
// Loop through all the fields and check if all of them are empty.
BOOL CSenderInformation::FieldsBlank(void)
{
   BOOL fBlank = TRUE;

   DWORD dwFieldCount = m_FieldList.GetFieldCount();
   for (DWORD dwField = 0; fBlank && (dwField < dwFieldCount); dwField++)
   {
      CFlatFileDatabaseField* pField = m_FieldList.GetField(dwField);
      if (pField != NULL)
      {
         if (pField->GetType() == FFT_Blob)
         {
            fBlank = fBlank & (pField->GetAsUnsigned32() == 0);
         }
         else
         {
            LPCSTR pszString = pField->GetAsString();
            if (pszString != NULL)
            {
               fBlank = fBlank & (strlen(pszString) == 0);
            }
         }
      }
   }

   return fBlank;
}

void CSenderInformation::OnPaint() 
{
   Default();

   if (m_fFirstPaint)
   {
      // Check if the information we were call on to edit was missing.
      // If so, post a message to remind ourselves to warn the user.
      if (m_fInformationMissing)
      {
//         PostMessage(WM_SENDER_DATABASE_EMPTY, 0, 0);
      }
      m_fFirstPaint = FALSE;
   }
}

LRESULT CSenderInformation::OnSenderDatabaseEmpty(WPARAM wParam, LPARAM)
{
   // The sender information editing dialog (us) was put up when the
   // user's sender information was determined to be missing. Put
   // up a dialog telling the user that their information was missing
   // and to click OK and enter their new information.
   CSenderInformationMissingNoDatabaseDialog Dialog(this);
   Dialog.DoModal();
   return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookListBox

CAddressBookListBox::CAddressBookListBox()
{
}

CAddressBookListBox::~CAddressBookListBox()
{
}

BEGIN_MESSAGE_MAP(CAddressBookListBox, CListBox)
   //{{AFX_MSG_MAP(CAddressBookListBox)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAddressBookListBox::Refresh(void)
{
   SetRedraw(FALSE);
   ResetContent();

   CStringArray csaUserNames;
   TRY
   {
      CAddressBook::GetAddressBookUserNames(csaUserNames);
      for (int nName = 0; nName  < csaUserNames.GetSize(); nName++)
      {
         AddString(csaUserNames.GetAt(nName));
      }
      if (nName > 0)
         SetCurSel(0);
   }
   END_TRY

   SetRedraw(TRUE);
   Invalidate();
}

BOOL CAddressBookListBox::GetFullName(CString& csFullName, int nIndex /*=-1*/)
{
   BOOL fResult = FALSE;

   csFullName.Empty();

   CString csUserName;
   TRY
   {
      if (nIndex == -1)
      {
         nIndex = GetCurSel();
      }

      if (nIndex != LB_ERR)
      {
         // Get the user name from the list box.
         GetText(nIndex, csUserName);
         Util::Trim(csUserName);

         // Get the full name from the user name.
         CAddressBook::GetAddressBookFullName(csUserName, csFullName);

         fResult = TRUE;
      }
   }
   CATCH_ALL(e)
   {
      csFullName.Empty();
      fResult = FALSE;
   }
   END_CATCH_ALL

   return fResult;
}
/////////////////////////////////////////////////////////////////////////////
// CNewAddressBookDialog dialog
CNewAddressBookDialog::CNewAddressBookDialog(LPCSTR pszUserName, CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(CNewAddressBookDialog::IDD, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(CNewAddressBookDialog)
   m_csUserName = _T("");
   //}}AFX_DATA_INIT

   TRY
   {
      m_csUserName = pszUserName;
      Util::Trim(m_csUserName);
   }
   END_TRY
}

void CNewAddressBookDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CNewAddressBookDialog)
   DDX_Control(pDX, IDC_ADDRESS_BOOK_NAME, m_UserName);
   DDX_Text(pDX, IDC_ADDRESS_BOOK_NAME, m_csUserName);
   //}}AFX_DATA_MAP

   if (pDX->m_bSaveAndValidate)
   {
      Util::Trim(m_csUserName);
   }
}

BEGIN_MESSAGE_MAP(CNewAddressBookDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CNewAddressBookDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewAddressBookDialog message handlers
BOOL CNewAddressBookDialog::OnInitDialog() 
{
   INHERITED::OnInitDialog();
   
   m_UserName.LimitText(_MAX_PATH);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewAddressBookDialog::OnOK() 
{
   // Get the dialog data.
   UpdateData(TRUE);

   StorageFile* pDatabaseFile = NULL;
   CFlatFileDatabase* pDatabase = NULL;

   TRY
   {
      // Check for a valid address book user name.
      if (!CAddressBook::ValidateAddressBookUserName(m_csUserName, TRUE))
      {
         AfxThrowUserException();
      }

      // Get the full name from the user name.
      CAddressBook::GetAddressBookFullName(m_csUserName, m_csFullName);

      // Check if the address book already exists.
      if (Util::FileExists(m_csFullName))
      {
         if (AfxMessageBox(IDS_NEW_ADDRESS_BOOK_EXISTS, MB_YESNO) != IDYES)
         {
            AfxThrowUserException();
         }

         // Delete the file.
         CFile::Remove(m_csFullName);
      }

      // Try to create the new address book.
      ERRORCODE Error = CAddressBook::OpenAddressBookDatabase(m_csFullName, pDatabaseFile, pDatabase, TRUE);
      if (Error != ERRORCODE_None)
      {
         ThrowErrorcodeException(Error);
      }
   }
   CATCH_ALL(e)
   {
      CAddressBook::CloseAddressBookDatabase(pDatabaseFile, pDatabase);

      // If the exception is a user exception, then assume the error has already been reported.
      if (!e->IsKindOf(RUNTIME_CLASS(CUserException)))
      {
         // Report a generic error.
         AfxMessageBox(IDS_CANT_CREATE_NEW_ADDRESS);
      }

      // Put the spotlight back on the user name edit.
      m_UserName.SetFocus();
      m_UserName.SetSel(0, -1);

      // Don't leave.
      AfxThrowUserException();
   }
   END_CATCH_ALL

   CAddressBook::CloseAddressBookDatabase(pDatabaseFile, pDatabase);

   // Do the default.
   INHERITED::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// COpenAddressBookDialog dialog
COpenAddressBookDialog::COpenAddressBookDialog(CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(COpenAddressBookDialog::IDD, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(COpenAddressBookDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void COpenAddressBookDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(COpenAddressBookDialog)
   DDX_Control(pDX, IDC_ADDRESS_BOOK_LIST, m_AddressBookList);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COpenAddressBookDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(COpenAddressBookDialog)
   ON_LBN_SELCHANGE(IDC_ADDRESS_BOOK_LIST, OnSelchangeAddressBookList)
   ON_LBN_DBLCLK(IDC_ADDRESS_BOOK_LIST, OnDblclkAddressBookList)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COpenAddressBookDialog::UpdateButtons(void)
{
   GetDlgItem(IDOK)->EnableWindow(m_AddressBookList.GetCurSel() != LB_ERR);
}

/////////////////////////////////////////////////////////////////////////////
// COpenAddressBookDialog message handlers

void COpenAddressBookDialog::OnOK() 
{
   m_AddressBookList.GetFullName(m_csFullName);
   INHERITED::OnOK();
}

void COpenAddressBookDialog::OnSelchangeAddressBookList() 
{
   UpdateButtons();  
}

void COpenAddressBookDialog::OnDblclkAddressBookList() 
{
   UpdateButtons();
   OnOK();
}

BOOL COpenAddressBookDialog::OnInitDialog() 
{
   INHERITED::OnInitDialog();
   
   m_AddressBookList.Refresh();

   UpdateButtons();
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CDeleteAddressBookDialog dialog

CDeleteAddressBookDialog::CDeleteAddressBookDialog(CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(CDeleteAddressBookDialog::IDD, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(CDeleteAddressBookDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void CDeleteAddressBookDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CDeleteAddressBookDialog)
   DDX_Control(pDX, IDC_ADDRESS_BOOK_LIST, m_AddressBookList);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDeleteAddressBookDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CDeleteAddressBookDialog)
   ON_LBN_SELCHANGE(IDC_ADDRESS_BOOK_LIST, OnSelchangeAddressBookList)
   ON_LBN_DBLCLK(IDC_ADDRESS_BOOK_LIST, OnDblclkAddressBookList)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDeleteAddressBookDialog::UpdateButtons(void)
{
   GetDlgItem(IDOK)->EnableWindow(m_AddressBookList.GetCurSel() != LB_ERR);
}

/////////////////////////////////////////////////////////////////////////////
// CDeleteAddressBookDialog message handlers

void CDeleteAddressBookDialog::OnOK() 
{
   CString csFullName;

   if (m_AddressBookList.GetFullName(csFullName))
   {
      // Make sure the user REALLY wants to do this since it cannot be undone.
      if (ConfirmDelete(csFullName))
      {
         TRY
         {
            CFile::Remove(csFullName);
            m_AddressBookList.Refresh();
         }
         CATCH_ALL(e)
         {
            AfxMessageBox(IDS_CANT_DELETE_ADDRESS_BOOK);
         }
         END_CATCH_ALL
      }
   }

   UpdateButtons();
}
// Ask the user if they REALLY want to delete all the names.
BOOL CDeleteAddressBookDialog::ConfirmDelete(const CString& csFullName)
{
   CConfirmDeleteAddressBookDialog Dialog(this);
   TRY
   {
      CAddressBook::GetAddressBookUserName(csFullName, Dialog.m_csName);
   }
   END_TRY
   return Dialog.DoModal() == IDOK;
}

void CDeleteAddressBookDialog::OnSelchangeAddressBookList() 
{
   UpdateButtons();
}

BOOL CDeleteAddressBookDialog::OnInitDialog() 
{
   INHERITED::OnInitDialog();
   
   m_AddressBookList.Refresh();
   
   UpdateButtons();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteAddressBookDialog::OnDblclkAddressBookList() 
{
   OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookDoesNotExistDialog dialog

CAddressBookDoesNotExistDialog::CAddressBookDoesNotExistDialog(LPCSTR pszUserName, CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(CAddressBook::AddressBooksExist() ? IDD_ADDRESS_BOOK_DOES_NOT_EXIST : IDD_ADDRESS_BOOK_DOES_NOT_EXIST_NO_OPEN, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(CAddressBookDoesNotExistDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   TRY
   {
      m_csUserName = pszUserName;
      Util::Trim(m_csUserName);
   }
   END_TRY
}

void CAddressBookDoesNotExistDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAddressBookDoesNotExistDialog)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddressBookDoesNotExistDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CAddressBookDoesNotExistDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBookDoesNotExistDialog message handlers

BOOL CAddressBookDoesNotExistDialog::OnInitDialog() 
{
   INHERITED::OnInitDialog();

   // Build the instructions text.
   CString csInstructions;
   CString csFormat;
   TRY
   {
      if (!m_csUserName.IsEmpty())
      {
         csFormat.LoadString(IDS_ADDRESS_BOOK_EMPTY_INSTRUCTIONS);
         csInstructions.Format(csFormat, m_csUserName);
      }
      else
      {
         csInstructions.LoadString(IDS_ADDRESS_BOOK_EMPTY_INSTRUCTIONS_NO_NAME);
      }
   }
   END_TRY
   SetDlgItemText(IDC_INSTRUCTIONS, csInstructions);

   // Select the most common selection.
   if (GetDlgItem(IDC_OPEN_ADDRESS_BOOK) != NULL)
   {
      CheckDlgButton(IDC_OPEN_ADDRESS_BOOK, TRUE);
   }
   else
   {
      CheckDlgButton(IDC_NEW_ADDRESS_BOOK, TRUE);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddressBookDoesNotExistDialog::OnOK() 
{
   // The user has selected an option which should give up a full
   // address book name to work with.
   m_csFullName.Empty();

   // Do the selected option.
   if (IsDlgButtonChecked(IDC_OPEN_ADDRESS_BOOK))
   {
      // The user wants to open an existing address book.
      COpenAddressBookDialog Dialog(this);
      if (Dialog.DoModal() == IDOK)
      {
         TRY
         {
            m_csFullName = Dialog.GetFullName();
         }
         END_TRY
      }
   }
   else if (IsDlgButtonChecked(IDC_NEW_ADDRESS_BOOK))
   {
      // The user wants to create a new address book with standard fields.
      CString csUserName;
      TRY
      {
         CNewAddressBookDialog Dialog(m_csUserName, this);
         if (Dialog.DoModal() == IDOK)
         {
            TRY
            {
               m_csFullName = Dialog.GetFullName();
            }
            END_TRY
         }
      }
      END_TRY
   }
   else if (IsDlgButtonChecked(IDC_IMPORT_ADDRESS_BOOK))
   {
      // We're off to see the wizard!
      CAddressBookImportWizard::Run(this, m_csFullName);
   }

   if (!m_csFullName.IsEmpty())
   {
      // Out-a-here!
      INHERITED::OnOK();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CAddressBookNoReceiversHelpDialog dialog

CAddressBookNoReceiversHelpDialog::CAddressBookNoReceiversHelpDialog(CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(CAddressBookNoReceiversHelpDialog::IDD, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(CAddressBookNoReceiversHelpDialog)
   //}}AFX_DATA_INIT
}

void CAddressBookNoReceiversHelpDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAddressBookNoReceiversHelpDialog)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddressBookNoReceiversHelpDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CAddressBookNoReceiversHelpDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressBookNoReceiversHelpDialog message handlers

BOOL CAddressBookNoReceiversHelpDialog::OnInitDialog() 
{
   INHERITED::OnInitDialog();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// COrphanedFieldsDialog dialog

COrphanedFieldsDialog::COrphanedFieldsDialog(CStringArray& csaOrphanedFieldNames, LPCSTR pszAddressBookFullName, CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(COrphanedFieldsDialog::IDD, pParent, ADDRESS_BOOK_COLOR),
   m_csaOrphanedFieldNames(csaOrphanedFieldNames)
{
   //{{AFX_DATA_INIT(COrphanedFieldsDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   TRY
   {
      m_csAddressBookFullName = pszAddressBookFullName;
   }
   END_TRY
}

void COrphanedFieldsDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(COrphanedFieldsDialog)
   DDX_Control(pDX, IDC_FIELD_LIST, m_FieldList);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COrphanedFieldsDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(COrphanedFieldsDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COrphanedFieldsDialog message handlers

BOOL COrphanedFieldsDialog::OnInitDialog() 
{
   INHERITED::OnInitDialog();

   CString csText;
   CString csFormat;
   CString csUserName;
   TRY
   {
      GetDlgItem(IDC_INSTRUCTIONS)->GetWindowText(csFormat);
      CAddressBook::GetAddressBookUserName(m_csAddressBookFullName, csUserName);
      csText.Format(csFormat, csUserName);
      GetDlgItem(IDC_INSTRUCTIONS)->SetWindowText(csText);
   }
   END_TRY

   for (int nName = 0; nName < m_csaOrphanedFieldNames.GetSize(); nName++)
   {
      m_FieldList.AddString(m_csaOrphanedFieldNames.ElementAt(nName));
   }

   CenterWindow();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CPersonalSenderInformationDialog dialog

CPersonalSenderInformationDialog::CPersonalSenderInformationDialog(CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(CPersonalSenderInformationDialog::IDD, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(CPersonalSenderInformationDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void CPersonalSenderInformationDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPersonalSenderInformationDialog)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPersonalSenderInformationDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CPersonalSenderInformationDialog)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPersonalSenderInformationDialog message handlers

void CPersonalSenderInformationDialog::Run(CWnd* pParent)
{
   DWORD dwSenderId = (DWORD)-1;

   // Check if the sender has already been set. If so, then don't ask again.
   if ((dwSenderId = GET_PMWAPP()->GetSenderId()) == (DWORD)-1)
   {
      // The sender ID is unknown, check if we should ask the user to provide some information.
      BOOL fAskForInformation = FALSE;
      CIniFile IniFile;

      TRY
      {
         // Get the INI file to use.
			IniFile.Name(GET_PMWAPP()->m_pszProfileName);
//       if (GET_PMWAPP()->InitPrintMasterIniFile(&IniFile, "_000PMW.INI", TRUE))
         {
            // Check if the user has already been asked to provide sender information.
            fAskForInformation = IniFile.GetInteger("SenderInformationDialog", "AskForSenderInformation", 1);

            // Write back FALSE so they won't be asked again. Each new NID is asked once.
            IniFile.WriteInteger("SenderInformationDialog", "AskForSenderInformation", 0);
         }
      }
      END_TRY

      if (fAskForInformation)
      {
         // Make sure standard address books exist.
         CAddressBook::CreateStandardAddressBooks();

         // Put up the dialog and let the user make a choice.
         CPersonalSenderInformationDialog Dialog(pParent);
         if (Dialog.DoModal() == IDOK)
         {
            // Ok, the user wants to select/enter some sender information.
            // CSelectSenderDialog::Run() records the new sender ID in the nidded INI file.
            CSelectSenderDialog::Run(pParent);
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CSenderInformationMissingDialog dialog

CSenderInformationMissingDialog::CSenderInformationMissingDialog(CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(CSenderInformationMissingDialog::IDD, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(CSenderInformationMissingDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void CSenderInformationMissingDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSenderInformationMissingDialog)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSenderInformationMissingDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CSenderInformationMissingDialog)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSenderInformationMissingDialog message handlers

/////////////////////////////////////////////////////////////////////////////
// CSenderInformationMissingNoDatabaseDialog dialog

CSenderInformationMissingNoDatabaseDialog::CSenderInformationMissingNoDatabaseDialog(CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(CSenderInformationMissingNoDatabaseDialog::IDD, pParent, ADDRESS_BOOK_COLOR)
{
   //{{AFX_DATA_INIT(CSenderInformationMissingNoDatabaseDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}

void CSenderInformationMissingNoDatabaseDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSenderInformationMissingNoDatabaseDialog)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSenderInformationMissingNoDatabaseDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CSenderInformationMissingNoDatabaseDialog)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CAddressBook::OnFilePrint() 
{
   CPmwApp     *pApp = GET_PMWAPP();
   CPrintInfo  Info;
   CPmwDoc*    pDocDlg; 

   if ((pDocDlg = pApp->NewHiddenDocument()) == NULL) // Create document for dialog
      Util::MessageBox(MB_OK, -1, MAKEINTRESOURCE(IDS_Error));
   else
   {
      // Do this now, or it will freak after adding objects
      pDocDlg->size_to_paper();   
      // Get the number of records in the address book
      DWORD             RecCount = 99;
      m_pDatabase->GetRecordCount(0, &RecCount);
      CPrintDialog      *pDlgSave = Info.m_pPD;
      CPrintAddressBookDlg Dlg (pDocDlg, RecCount);
      Info.m_pPD =      &Dlg;

      int ret = pApp->DoPrintDialog (&Dlg);
      // Get rid of document
      pDocDlg->OnCloseDocument();

      if (ret == IDOK)
      {
         BOOL        bSuccess = FALSE;
         CWaitCursor wc;
         CPmwDoc     *pDocTmp;
			// Get the document used as a print template
         if ((pDocTmp = pApp->OpenHiddenDocument(Dlg.FullSelectedName())) != NULL)
         {
            int OrientationSave = pApp->last_new_info.orientation;
            int Orientation = pDocTmp->get_orientation();
            pApp->last_new_info.orientation = Orientation;
            // Create output document
            CPmwDoc*    pDocOut; 
            if ((pDocOut = pApp->NewHiddenDocument()) != NULL)
            {
               // Do this now, or it will freak after adding objects
               pDocOut->size_to_paper();   
               
					TRY
					{
						CString	Title1, Title2;
						Title1.LoadString(IDS_ADDRESS_BOOK_TITLE_TEXT_WITH_NAME);
						Title2.Format(Title1, Dlg.m_FormatValue);
						// Do this before add_hidden_view(), or MFC asserts (AddrView fails IsWindow())
						pDocOut->SetTitle(Title2);
					}
					END_TRY
               
					CPrintAddrView AddrView;
               ((CAddrPrintDoc*)pDocOut)->add_hidden_view(&AddrView);
               
					AddrView.PrintProject (pDocTmp, &Info, m_pDatabaseCursor);
               
               pDocOut->RemoveView(&AddrView);

               bSuccess = TRUE;
            }
            // Allowed the view to go out of scope first
            if (pDocOut != NULL) 
               pDocOut->OnCloseDocument();
            pApp->last_new_info.orientation = OrientationSave;
            pDocTmp->OnCloseDocument();   
         }
         
         if (bSuccess =! TRUE)
            Util::MessageBox(MB_OK, -1, MAKEINTRESOURCE(IDS_Error));
      }
      else
      if (Dlg.DidYouNotFindAny () == TRUE)
         AfxMessageBox (IDS_ADDRESS_BOOK_NO_PRNT_TEMPS);
      Info.m_pPD = pDlgSave;
   }
}
