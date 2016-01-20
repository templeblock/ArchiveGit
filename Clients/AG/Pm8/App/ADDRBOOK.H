/*
// $Workfile: ADDRBOOK.H $
// $Revision: 1 $
// $Date: 3/03/99 6:01p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADDRBOOK.H $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 20    9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 19    7/11/98 3:37p Rlovejoy
// Added SortReceiverList().
// 
// 18    7/10/98 11:07a Rlovejoy
// Store the dialog's parent in m_pParent.
// 
// 17    6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 16    5/31/98 5:11p Fredf
// More Address Book and merge field changes.
// 
// 15    5/31/98 12:52p Fredf
// Start of address book changes
// 
// 14    2/24/98 5:48p Johno
// Not my task anymore so NOTHING IS MY FAULT!
// Use #define LOOK_AND_FEEL in addrbook.h to turn on / off most changes
// 
// 13    2/20/98 12:15p Johno
// Compile update
// 
// 12    2/12/98 12:11p Johno
// 
// 11    2/05/98 3:43p Johno
// Quick! Before the power goes out!
// 
// 10    1/30/98 11:13a Johno
// Compile update
// 
// 9     1/28/98 1:06p Johno
// So you can compile
// 
// 8     1/27/98 11:50a Johno
// For a version that will compile
// 
// 7     1/20/98 3:07p Johno
// So you can build
// 
// 6     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
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
//    Rev 1.0   14 Aug 1997 15:18:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:36   Fred
// Initial revision.
// 
//    Rev 1.43   13 Jun 1997 17:43:10   Jay
// Updates fields if sender is edited.
// 
//    Rev 1.42   22 Apr 1997 12:08:58   johno
// Added color to "Run" command
// 
//    Rev 1.41   07 Apr 1997 11:30:00   johno
// Overloaded Invalidate () for address book
// (special case)
// 
//    Rev 1.40   04 Apr 1997 17:53:14   johno
// dirived from pmwdialogcolor
// 
//    Rev 1.39   28 Mar 1997 17:11:14   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.38   14 Oct 1996 11:04:22   Jay
// New sounds.
// 
//    Rev 1.37   08 Sep 1996 09:40:14   Jay
// CPmwDialogs with no sound.
// 
//    Rev 1.36   08 Sep 1996 09:23:18   Fred
// Confirm delete address book; New classes which center CDialog based dialogs.
// 
//    Rev 1.35   06 Sep 1996 18:13:32   Jay
// Sound changes.
// 
//    Rev 1.34   05 Sep 1996 08:34:06   Fred
// Fixed bug where pressing enter could cause the address book to go away.
// 
//    Rev 1.33   01 Sep 1996 10:53:10   Fred
// Orphaned fields dialog is now a CDialog since we don't have a sound for it.
// 
//    Rev 1.32   28 Aug 1996 15:22:42   Fred
// Enforces maximum number of names
// 
//    Rev 1.31   21 Aug 1996 16:18:46   Fred
// Added CPmwDialog derived dialog classes for some sender dialogs.
// New Address Book closes current database.
// 
//    Rev 1.30   09 Aug 1996 16:50:10   Fred
// More 16-bit fixes
// 
//    Rev 1.29   08 Aug 1996 17:30:22   Fred
// More 16-bit changes
// 
//    Rev 1.28   05 Aug 1996 10:32:28   Fred
//  
// 
//    Rev 1.27   31 Jul 1996 11:57:22   Fred
// 16-bit compatibility
// 
//    Rev 1.26   25 Jul 1996 11:45:14   Fred
// Fixed some refresh problems
// 
//    Rev 1.25   24 Jul 1996 13:12:22   Fred
// Moved CMlsStringArray to SORTARRY
// 
//    Rev 1.24   23 Jul 1996 08:05:04   Fred
// Some bug fixes
// 
//    Rev 1.23   19 Jul 1996 15:26:44   Fred
//  
// 
//    Rev 1.22   17 Jul 1996 16:38:42   Fred
// Uses addrdata; compresses pictures
// 
//    Rev 1.21   17 Jul 1996 09:04:58   Fred
// Some address book improvements
// 
//    Rev 1.20   16 Jul 1996 17:30:30   Fred
// More address/sender book stuff
// 
//    Rev 1.19   16 Jul 1996 08:50:42   Fred
// Hooked up address/sender book
// 
//    Rev 1.18   12 Jul 1996 16:26:38   Fred
// Address book error handling
// 
//    Rev 1.17   11 Jul 1996 08:37:22   Fred
// More address book features
// 
//    Rev 1.16   06 Jul 1996 12:17:38   Fred
// Address book improvements
// 
//    Rev 1.15   06 Jul 1996 09:25:12   Fred
// Picture editor and other improvements
// 
//    Rev 1.14   03 Jul 1996 18:52:18   Fred
// Picture editor
// 
//    Rev 1.13   02 Jul 1996 10:56:30   Fred
// Sender Information
// 
//    Rev 1.12   01 Jul 1996 07:30:14   Fred
// Receiver list
// 
//    Rev 1.11   27 Jun 1996 10:42:46   Fred
// Receiver list. Before removing receiver from non-receiver views
// 
//    Rev 1.10   24 Jun 1996 19:16:16   Fred
// Some fixes and improvments
// 
//    Rev 1.9   24 Jun 1996 00:29:48   Fred
// Address Book Overview
// 
//    Rev 1.8   21 Jun 1996 09:29:46   Fred
// More overview list
// 
//    Rev 1.7   21 Jun 1996 07:00:52   Fred
// Start of overview list box
// 
//    Rev 1.6   18 Jun 1996 15:20:38   Fred
// Field value list management and editing
// 
//    Rev 1.5   18 Jun 1996 08:24:54   Fred
// More adress book changes
// 
//    Rev 1.4   10 Jun 1996 08:23:36   Fred
// Lots more stuff
// 
//    Rev 1.3   05 Jun 1996 08:50:24   Fred
// New stuff
// 
//    Rev 1.2   03 Jun 1996 14:42:42   Fred
// Added menu bar and accelerators.
// Auto enable/disable of controls also works.
// 
//    Rev 1.1   03 Jun 1996 08:44:24   Fred
//  
// 
//    Rev 1.0   24 May 1996 09:39:40   Fred
// Initial revision.
*/

class CAddressBookNameList;
class CAddressBookTabControl;
class CAddressBookListManager;
class CAddressBookData;
class CAddressBook;
class CSenderData;
class CSelectSenderDialog;
class CSenderInformation;
class CAddressBookListBox;
class CNewAddressBookDialog;
class COpenAddressBookDialog;
class CDeleteAddressBookDialog;
class CAddressBookDoesNotExistDialog;
class CAddressNoReceiversHelpDialog;
class COrphanedFieldsDialog;
class CPersonalSenderInformationDialog;
class CSenderInformationMissingDialog;
class CSenderInformationMissingNoDatabaseDialog;

#ifndef __ADDRBOOK_H__
#define __ADDRBOOK_H__

class NameListRecord;

#include "ipathmgr.h"
#include "bitbtn.h"
#include "panel.h"
#include "flatdb.h"
#include "dbtabs.h"
#include "file.h"
#include "inifile.h"
#include "addrlist.h"
#include "pdlgclr.h"
#include <afxcmn.h>

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define	WM_EDITOR_ACTIVATED				(WM_USER+0x1001)
#define	WM_EDITOR_DEACTIVATED			(WM_USER+0x1002)

#define	WM_LIST_ITEM_SELECTED			(WM_USER+0x1003)
#define	WM_LIST_BUTTON_CLICKED			(WM_USER+0x1004)

#define	WM_GET_FIELD_LIST					(WM_USER+0x1005)

#define	WM_SELECT_RECORD					(WM_USER+0x1006)

#define	WM_GET_RECEIVER_LIST				(WM_USER+0x1007)

#define	WM_GET_DATABASE_TABS				(WM_USER+0x1008)

#define	WM_SENDER_DATABASE_NOT_EMPTY	(WM_USER+0x1009)
#define	WM_SENDER_DATABASE_EMPTY		(WM_USER+0x100A)

struct ControlMovement
{
	enum
	{
		MX		=		0x0001,
		MY		=		0x0002,
		EX		=		0x0004,
		EY		=		0x0008
	};

	UINT m_nID;
	WORD m_wMovement;
};

void MoveControls(CWnd* pParent, int nX, int nY, ControlMovement* pMovement);

LPBITMAPINFO MakeThumbnail(LPCSTR pszFileName, int nMaxWidth, int nMaxHeight, CPalette* pPalette = NULL);

class CIndexList
{
public:
	CIndexList();
	virtual ~CIndexList();

public:
	int GetSize(void);
	int Add(LPCSTR pszName, DWORD dwIndex);
	BOOL GetAt(int nIndex, LPCSTR& pszName, DWORD& dwIndex);
	void DeleteAt(int nIndex);
	void DeleteAll(void);
	int FindByIndex(DWORD dwIndexToFind);

protected:
	CPtrArray m_Data;
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookNameList

class CAddressBookNameList : public CDWordArray
{
	INHERIT(CAddressBookNameList, CDWordArray)

public:
	CAddressBookNameList();
	CAddressBookNameList(CAddressBookNameList& List);
	~CAddressBookNameList();

public:
	// Search for a value.
	int Find(DWORD dwValue);

	// Add a name.
	void AddName(DWORD dwID);
	
	// Delete a name by index.
	void DeleteName(int nName);

	// Delete a name by ID.
	void DeleteName(DWORD dwID)
		{ 	DeleteName(Find(dwID)); }

	// Delete all names.
	void DeleteAllNames(void);

	// Get the name count.
	int GetNameCount(void)
		{ return GetSize(); }

	// Get the name ID at the given index.
	DWORD GetName(int nIndex)
		{ return GetAt(nIndex); }

	// Return the index of a given name ID.
	int FindName(DWORD dwID)
		{ return Find(dwID); }

	// Copy to outside ID array.
	void GetNames(DWORD* pNames);
	void GetNames(CDWordArray* pNames);

	// Block copy from outside ID array.
	void PutNames(const DWORD* pNames, int nCount);

	// Set changed state.
	void SetChanged(BOOL fChanged = TRUE)
		{ m_fChanged = fChanged; }

	// Get changed state.
	BOOL GetChanged(void) const
		{ return m_fChanged; }

protected:
	BOOL m_fChanged;
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookTabControl

class CAddressBookTabControl : public CTabCtrl
{
	INHERIT(CAddressBookTabControl, CTabCtrl)

public:
	CAddressBookTabControl(void);
	virtual ~CAddressBookTabControl(void);

public:
	BOOL GetTabName(int nTab, CString& csName);
	BOOL DeleteAllTabs(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CAddressBookTabControl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
// CAddressBookData
// Class used to communicate with address book. Encapsulates input and
// output data.

class CAddressBookData
{
public:
	CAddressBookData();
	CAddressBookData(NameListRecord* pNameListRecord, BOOL fSelectReceivers = FALSE, BOOL fDisableReceivers = FALSE);
	CAddressBookData(LPCSTR pszAddressBookFullName, LPCSTR pszSortIndexName, DWORD dwReceiverCount, DWORD* pdwReceivers, BOOL fSelectReceivers = FALSE, BOOL fDisableReceivers = FALSE);
	virtual ~CAddressBookData();

public:
	void SetAddressBookFullName(LPCSTR pszAddressBookFullName);
	const CString& GetAddressBookFullName(void) const
		{ return m_csAddressBookFullName; }
	void SetSortIndexName(LPCSTR pszSortIndexName);
	const CString& GetSortIndexName(void) const
		{ return m_csSortIndexName; }
	void SetReceiverList(NameListRecord* pNameListRecord);
	void SetReceiverList(DWORD dwReceiverCount, DWORD* pdwReceivers);
	CAddressBookNameList& GetReceiverList(void)
		{ return m_ReceiverList; }
	void SetSelectReceivers(BOOL fSelectReceivers = TRUE)
		{ m_fSelectReceivers = fSelectReceivers; }
	BOOL GetSelectReceivers(void) const
		{ return m_fSelectReceivers; }
	void SetDisableReceivers(BOOL fDisableReceivers = TRUE)
		{ m_fDisableReceivers = fDisableReceivers; }
	BOOL GetDisableReceivers(void) const
		{ return m_fDisableReceivers; }
	void ClearChanged(void);
	BOOL GetChanged(void)
		{ return GetDatabaseChanged() || GetAddressBookFullNameChanged() || GetSortIndexNameChanged() || GetReceiverListChanged(); }
	void SetDatabaseChanged(BOOL fChanged = TRUE)
		{ m_fDatabaseChanged = fChanged; }
	BOOL GetDatabaseChanged(void) const
		{ return m_fDatabaseChanged; }
	void SetAddressBookFullNameChanged(BOOL fChanged = TRUE)
		{ m_fAddressBookFullNameChanged = fChanged; }
	BOOL GetAddressBookFullNameChanged(void) const
		{ return m_fAddressBookFullNameChanged; }
	void SetSortIndexNameChanged(BOOL fChanged = TRUE)
		{ m_fSortIndexNameChanged = fChanged; }
	BOOL GetSortIndexNameChanged(void) const
		{ return m_fSortIndexNameChanged; }
	void SetReceiverListChanged(BOOL fChanged = TRUE)
		{ GetReceiverList().SetChanged(fChanged); }
	BOOL GetReceiverListChanged(void)
		{ return GetReceiverList().GetChanged(); }

protected:
	CString m_csAddressBookFullName;
	CString m_csSortIndexName;
	CAddressBookNameList m_ReceiverList;
	BOOL m_fSelectReceivers;
	BOOL m_fDisableReceivers;
	BOOL m_fDatabaseChanged;
	BOOL m_fAddressBookFullNameChanged;
	BOOL m_fSortIndexNameChanged;
};

//////////////////////////////////////////////////////////////////////////
// CAddressBook

class CAddressBook : public CPmwDialogColor
{
	INHERIT(CAddressBook, CPmwDialogColor)

public:
	// Maximum number of records allowed in an address book.
	enum
	{
		MAXIMUM_NAMES						=	1000000
	};

// Construction
public:
	CAddressBook(CWnd* pParent, CAddressBookData* pData, COLORREF cr = INVALID_DIALOG_COLOR);
	virtual ~CAddressBook();              // standard destructor

// Dialog Data
	//{{AFX_DATA(CAddressBook)
	enum { IDD = IDD_ADDRESS_BOOK };
	//}}AFX_DATA


// Overrides
   void Invalidate (BOOL bErase = TRUE)
   {
      INHERITED::Invalidate (bErase);
   }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressBook)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddressBook)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnAddWithoutEdit();
	afx_msg void OnDelete();
	afx_msg void OnDeleteAll();
	afx_msg void OnSave();
	afx_msg void OnUndo();
	afx_msg void OnAll();
	afx_msg void OnNone();
	afx_msg void OnFirst();
	afx_msg void OnBack();
	afx_msg void OnNext();
	afx_msg void OnLast();
	afx_msg void OnBackPage();
	afx_msg void OnNextPage();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileDelete();
	afx_msg void OnFileClose();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUpdateFirst(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNone(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNameNumber(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnViewOneatatime();
	afx_msg void OnViewOverview();
	afx_msg void OnViewReceiverlist();
	afx_msg void OnFileImportnames();
	afx_msg void OnFileExportnames();
	afx_msg void OnSetfocusAddressBookFieldList();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
//	afx_msg void OnPaint();
	afx_msg void OnFilePrint();
	//}}AFX_MSG
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelectIndex(UINT nSelection);
	afx_msg void OnAddressBookTabChange(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg LRESULT OnEditorActivated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFieldList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetDatabaseTabs(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectRecord(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetReceiverList(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnHelpUser(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	void UpdateControl(CCmdUI* pCmdUI, BOOL bEnable);

	BOOL CanAdd(void);
	BOOL CanDelete(void);
	BOOL CanDeleteAll(void);
	BOOL CanSave(void);
	BOOL CanUndo(void);
	BOOL CanAll(void);
	BOOL CanNone(void);

	virtual BOOL ConfirmDelete(void);
	virtual BOOL ConfirmDeleteAll(void);

   ERRORCODE DoPrintByDocument (LPCSTR DocName, CPrintInfo* pInfo, CPmwDoc* pDocOut);

	virtual void CloseAddressBook(int nModalResult);

protected:
	CAddressBookTabControl m_Tabs;

	CMlsBitmapButton m_FirstButton;
	CMlsBitmapButton m_BackButton;
	CMlsBitmapButton m_NextButton;
	CMlsBitmapButton m_LastButton;

	CMlsPanel m_NameNumber;
	CAddressBookFieldListBox m_FieldList;
	CAddressBookOverviewListBox m_OverviewList;
	CAddressBookReceiverListBox m_ReceiverList;

	CIndexList m_SortIndexList;
	int m_nCurrentSortIndex;

public:
	virtual int DoModal();
	virtual BOOL DestroyWindow();

protected:
	HACCEL m_hAccelerators;
	int m_nModalResult;

protected:
	CWnd* m_pParent;
	CAddressBookData* m_pData;
	StorageFile* m_pDatabaseFile;
	CFlatFileDatabase* m_pDatabase;
	CFlatFileDatabaseCursor* m_pDatabaseCursor;
	BOOL SetDatabase(CString csFullName);
	void CloseDatabase(void);
	void RefreshIndexInformation(void);
	void SetIndex(DWORD dwIndex);
	CMenu* FindMenuByName(UINT nMenuName);
	void RefreshFieldInformation(void);
	void RefreshNameNumber(void);
	void SortReceiverList(void);
	CString m_csEmptySortMenu;
	CString m_csNameNumberFormat;
	UINT m_uView;
	void SetView(UINT uView);
	BOOL m_fFirstPaint;

	BOOL m_fSizesValid;
	int m_nMinimumWidth;
	int m_nMinimumHeight;
	int m_nCurrentWidth;
	int m_nCurrentHeight;

	BOOL m_fReceiverList;

	CDatabaseTabs* m_pDatabaseTabs;

	BOOL SaveInformation(void);
	BOOL SaveSplitterInformation(CIniFile& IniFile);
	BOOL RestoreInformation(void);
	BOOL RestoreSplitterInformation(CIniFile& IniFile);

	void BeforeRecordChange(void);
	void AfterRecordChange(BOOL fFieldDataValid = FALSE, BOOL fTabDataValid = FALSE);
	BOOL FieldsBlank(void);

	BOOL m_fAddingNewRecord;
	BOOL m_fUpdateTabs;

	CFlatFileValueListManager m_ListManager;

	void CurrentViewBeforeRecordChange(void);
	void CurrentViewRecordModified(DWORD dwStartPosition, DWORD dwEndPosition);
	void CurrentViewAfterRecordChange(void);
	BOOL CurrentViewIsModified(void);
	void CurrentViewSave(void);
	DWORD CurrentViewGetFieldCount(void);
	CFlatFileDatabaseField* CurrentViewGetField(DWORD dwField);
	void CurrentViewRefresh(void);
	BOOL CurrentViewIsEditing(void);
	void CurrentViewShow(BOOL fShow = TRUE);
	void CurrentViewStartNewEdit(void);
	void CurrentViewActivate(BOOL fActive = TRUE);
	void CurrentViewBackPage(void);
	void CurrentViewNextPage(void);
	void CurrentViewAbortNewEdit(void);
	BOOL CurrentViewCanDelete(void);
	BOOL CurrentViewCanUndo(void);
	BOOL CurrentViewCanAll(void);
	BOOL CurrentViewCanNone(void);
	void CurrentViewDatabaseEmpty(void);

	BOOL GetImportantFields(void);

	DWORD m_dwIdField;
	DWORD m_dwFirstNameField;
	DWORD m_dwLastNameField;
	DWORD m_dwFullNameField;

public:
	void GetDisplayName(CFlatFileDatabaseCursor* pCursor, CString& csName, BOOL fIncludeIndexValue = TRUE);
	static void RegisterAddressBookClasses(void);
	static void UnregisterAddressBookClasses(void);

public:
	static BOOL Run(CWnd* pParent, NameListRecord* pNameList = NULL, BOOL fSelectReceivers = FALSE, BOOL fDisableReceivers = FALSE, COLORREF cr = INVALID_DIALOG_COLOR);

public:
	static void GetAddressBookFullName(const CString& csUserName, CString& csFullName);
	static void GetAddressBookUserName(const CString& csFullName, CString& csUserName);
	static BOOL ValidateAddressBookUserName(const CString& csUserName, BOOL fReportError = FALSE);
	static BOOL GetAddressBookUserNames(CStringArray& csaUserNames);
	static BOOL AddressBooksExist(void);
	static void CreateStandardAddressBooks(void);
	static ERRORCODE OpenAddressBookDatabase(
		LPCSTR pszFullName,
		StorageFile*& pDatabaseFile,
		CFlatFileDatabase*& pDatabase,
		BOOL fCreate = FALSE,
		LPCSTR pszDescription = NULL,
		CFlatFileFieldDescription* pFieldDescriptions = NULL,
		CFlatFileIndexDescription* pIndexDescriptions = NULL);
	static ERRORCODE OpenAddressBookSenderDatabase(
		LPCSTR pszFullName,
		StorageFile*& pDatabaseFile,
		CFlatFileDatabase*& pDatabase,
		BOOL fCreate = FALSE);
	static void CloseAddressBookDatabase(StorageFile*& pDatabaseFile, CFlatFileDatabase*& pDatabase);
	static void ConvertVersion2IndexName(LPCSTR pszVersion2, CString& csIndexName);
protected:
   // Used in OnFileExportnames() 
   void 
   WriteFieldString(CString &str, FILE *pFile, TCHAR Delim);
};

//////////////////////////////////////////////////////////////////////////
// CSenderData
// Class used to communicate with sender dialogs. Encapsulates input and
// output data.

class CSenderData
{
public:
	CSenderData();
	CSenderData(LPCSTR pszSenderBookFullName, DWORD dwSenderId = (DWORD)-1);
	virtual ~CSenderData();

public:
	void SetSenderBookFullName(LPCSTR pszSenderBookFullName);
	const CString& GetSenderBookFullName(void) const
		{ return m_csSenderBookFullName; }
	void SetSenderId(DWORD dwSenderId);
	DWORD GetSenderId(void) const
		{ return m_dwSenderId; }
	void ClearChanged(void);
	BOOL GetChanged(void)
		{ return GetSenderBookFullNameChanged() || GetSenderIdChanged() || GetSenderEdited(); }
	void SetSenderBookFullNameChanged(BOOL fChanged = TRUE)
		{ m_fSenderBookFullNameChanged = fChanged; }
	BOOL GetSenderBookFullNameChanged(void) const
		{ return m_fSenderBookFullNameChanged; }
	void SetSenderIdChanged(BOOL fChanged = TRUE)
		{ m_fSenderIdChanged = fChanged; }
	BOOL GetSenderIdChanged(void) const
		{ return m_fSenderIdChanged; }
	void SetSenderEdited(BOOL fEdited = TRUE)
		{ m_fSenderEdited = fEdited; }
	BOOL GetSenderEdited(void) const
		{ return m_fSenderEdited; }

protected:
	CString m_csSenderBookFullName;
	DWORD m_dwSenderId;
	BOOL m_fSenderBookFullNameChanged;
	BOOL m_fSenderIdChanged;
	BOOL m_fSenderEdited;
};

//////////////////////////////////////////////////////////////////////////
// CSelectSenderDialog

class CSelectSenderDialog : public CPmwDialogColor
{
	INHERIT(CSelectSenderDialog, CPmwDialogColor)

// Construction
public:
	CSelectSenderDialog(CSenderData* pData, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectSenderDialog();              // standard destructor

// Dialog Data
	//{{AFX_DATA(CSelectSenderDialog)
	enum { IDD = IDD_SELECT_SENDER };
	CListBox	m_SenderList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectSenderDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectSenderDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnUpdateEnableIfSelected(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	afx_msg void OnDblclkSenderList();
	//}}AFX_MSG
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSenderDatabaseNotEmpty(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSenderDatabaseEmpty(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL DestroyWindow();

protected:
	CMlsBitmapButton m_NewButton;
	CMlsBitmapButton m_DeleteButton;
	CMlsBitmapButton m_EditButton;

	CSenderData* m_pData;
	StorageFile* m_pDatabaseFile;
	CFlatFileDatabase* m_pDatabase;
	CFlatFileDatabaseCursor* m_pCursor;
	BOOL SetDatabase(LPCSTR pszName);
	void CloseDatabase(void);

	void UpdateNameList(void);
	void GetDisplayName(CFlatFileDatabaseCursor* pCursor, CString& csName);
	BOOL SelectItemWithID(DWORD dwID = 0);
	DWORD GetIdOfSelectedItem(void);
	BOOL PositionCursorAtIdOfSelectedItem(void);
	void SetupFieldsForNewSender(CFlatFileDatabaseCursor* pCursor);
	void SetupFieldFromIniFile(CFlatFileDatabaseCursor* pCursor, LPCSTR pszField, CIniFile& IniFile, LPCSTR pszSection, LPCSTR pszEntry);
	BOOL ConfirmDelete(void);

	BOOL m_fFirstPaint;

public:
	static BOOL Run(CWnd* pParent);
};

//////////////////////////////////////////////////////////////////////////
// CSenderInformation

class CSenderInformation : public CPmwDialogColor
{
	INHERIT(CSenderInformation, CPmwDialogColor)

// Construction
public:
	CSenderInformation(CSenderData* pData, CFlatFileDatabaseCursor* pCursor, BOOL fInformationMissing, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSenderInformation();              // standard destructor

// Dialog Data
	//{{AFX_DATA(CSenderInformation)
	enum { IDD = IDD_SENDER_INFORMATION };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSenderInformation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSenderInformation)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSetfocusAddressBookFieldList();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFieldList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSenderDatabaseEmpty(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	CAddressBookFieldListBox m_FieldList;

public:
	virtual int DoModal();
	virtual BOOL DestroyWindow();

protected:
	int m_nModalResult;

protected:
	CSenderData* m_pData;
	CFlatFileDatabaseCursor* m_pCursor;
	BOOL SetDatabase(CFlatFileDatabaseCursor* pCursor);
	void RefreshFieldInformation(void);

	BOOL m_fSizesValid;
	int m_nMinimumWidth;
	int m_nMinimumHeight;
	int m_nCurrentWidth;
	int m_nCurrentHeight;

	BOOL SaveInformation(void);
	BOOL SaveSplitterInformation(CIniFile& IniFile);
	BOOL RestoreInformation(void);
	BOOL RestoreSplitterInformation(CIniFile& IniFile);

	BOOL FieldsBlank(void);

	BOOL m_fAddingNewRecord;

	CFlatFileValueListManager m_ListManager;

protected:
	BOOL m_fInformationMissing;
	BOOL m_fFirstPaint;
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookListBox

class CAddressBookListBox : public CListBox
{
	INHERIT(CAddressBookListBox, CListBox)

// Construction
public:
	CAddressBookListBox();
	virtual ~CAddressBookListBox();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddressBookListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Refresh(void);
	BOOL GetFullName(CString& csFullName, int nIndex = -1);
};

/////////////////////////////////////////////////////////////////////////////
// CNewAddressBookDialog dialog

class CNewAddressBookDialog : public CPmwDialogColor
{
	INHERIT(CNewAddressBookDialog, CPmwDialogColor)

// Construction
public:
	CNewAddressBookDialog(LPCSTR pszUserName, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewAddressBookDialog)
	enum { IDD = IDD_NEW_ADDRESS_BOOK };
	CEdit	m_UserName;
	CString	m_csUserName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewAddressBookDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewAddressBookDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	LPCSTR GetFullName(void)
		{ return m_csFullName; }

protected:
	CString m_csFullName;
};

/////////////////////////////////////////////////////////////////////////////
// COpenAddressBookDialog dialog

class COpenAddressBookDialog : public CPmwDialogColor
{
	INHERIT(COpenAddressBookDialog, CPmwDialogColor)

// Construction
public:
	COpenAddressBookDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenAddressBookDialog)
	enum { IDD = IDD_OPEN_ADDRESS_BOOK };
	CAddressBookListBox	m_AddressBookList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenAddressBookDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenAddressBookDialog)
	virtual void OnOK();
	afx_msg void OnSelchangeAddressBookList();
	afx_msg void OnDblclkAddressBookList();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	LPCSTR GetFullName(void)
		{ return m_csFullName; }

protected:
	void UpdateButtons(void);
	CString m_csFullName;
};

/////////////////////////////////////////////////////////////////////////////
// CDeleteAddressBookDialog dialog

class CDeleteAddressBookDialog : public CPmwDialogColor
{
	INHERIT(CDeleteAddressBookDialog, CPmwDialogColor)

// Construction
public:
	CDeleteAddressBookDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteAddressBookDialog)
	enum { IDD = IDD_DELETE_ADDRESS_BOOK };
	CAddressBookListBox	m_AddressBookList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteAddressBookDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteAddressBookDialog)
	virtual void OnOK();
	afx_msg void OnSelchangeAddressBookList();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkAddressBookList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL ConfirmDelete(const CString& csFullName);

protected:
	void UpdateButtons(void);
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookDoesNotExistDialog dialog

class CAddressBookDoesNotExistDialog : public CPmwDialogColor
{
	INHERIT(CAddressBookDoesNotExistDialog, CPmwDialogColor)

// Construction
public:
	CAddressBookDoesNotExistDialog(LPCSTR pszUserName, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CAddressBookDoesNotExistDialog)
	enum { IDD = IDD_ADDRESS_BOOK_DOES_NOT_EXIST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressBookDoesNotExistDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddressBookDoesNotExistDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	LPCSTR GetFullName(void)
		{ return m_csFullName; }

protected:
	CString m_csFullName;
	CString m_csUserName;
};

/////////////////////////////////////////////////////////////////////////////
// CAddressBookNoReceiversHelpDialog dialog

class CAddressBookNoReceiversHelpDialog : public CPmwDialogColor
{
	INHERIT(CAddressBookNoReceiversHelpDialog, CPmwDialogColor)

// Construction
public:
	CAddressBookNoReceiversHelpDialog(CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CAddressBookNoReceiversHelpDialog)
	enum { IDD = IDD_ADDRESS_BOOK_NO_RECEIVERS_HELP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressBookNoReceiversHelpDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddressBookNoReceiversHelpDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// COrphanedFieldsDialog dialog

class COrphanedFieldsDialog : public CPmwDialogColor
{
	INHERIT(COrphanedFieldsDialog, CPmwDialogColor)

// Construction
public:
	COrphanedFieldsDialog(CStringArray& csaOrphanedFieldNames, LPCSTR pszAddressBookFullName, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(COrphanedFieldsDialog)
	enum { IDD = IDD_ORPHANED_FIELDS };
	CListBox	m_FieldList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrphanedFieldsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COrphanedFieldsDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CStringArray& m_csaOrphanedFieldNames;
	CString m_csAddressBookFullName;
};

/////////////////////////////////////////////////////////////////////////////
// CPersonalSenderInformationDialog dialog

class CPersonalSenderInformationDialog : public CPmwDialogColor
{
	INHERIT(CPersonalSenderInformationDialog, CPmwDialogColor)

// Construction
public:
	CPersonalSenderInformationDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPersonalSenderInformationDialog)
	enum { IDD = IDD_PERSONAL_SENDER_INFORMATION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPersonalSenderInformationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPersonalSenderInformationDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static void Run(CWnd* pParent);
};

/////////////////////////////////////////////////////////////////////////////
// CSenderInformationMissingDialog dialog

class CSenderInformationMissingDialog : public CPmwDialogColor
{
	INHERIT(CSenderInformationMissingDialog, CPmwDialogColor)

// Construction
public:
	CSenderInformationMissingDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSenderInformationMissingDialog)
	enum { IDD = IDD_SEND_INFORMATION_MISSING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSenderInformationMissingDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSenderInformationMissingDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSenderInformationMissingNoDatabaseDialog dialog

class CSenderInformationMissingNoDatabaseDialog : public CPmwDialogColor
{
	INHERIT(CSenderInformationMissingNoDatabaseDialog, CPmwDialogColor)

// Construction
public:
	CSenderInformationMissingNoDatabaseDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSenderInformationMissingNoDatabaseDialog)
	enum { IDD = IDD_SEND_INFORMATION_MISSING_NO_DATABASE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSenderInformationMissingNoDatabaseDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSenderInformationMissingNoDatabaseDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
