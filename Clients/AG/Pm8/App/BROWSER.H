/*
// $Workfile: BROWSER.H $
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
// $Log: /PM8/App/BROWSER.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 69    3/01/99 12:04p Mwilson
// Added AgProjectBrowser
// 
// 68    2/25/99 6:32p Gbeddow
// New Online Art button artwork, and new "Master Art" button and
// functionality in the Art Gallery dialog
// 
// 67    2/23/99 4:28p Dennis
// Replaced sentiment drawing with RTF control for single line sentiment
// and scrolling support
// 
// 66    2/23/99 1:29p Gbeddow
// fix bug 181 - art gallery search results weren't being properly
// restored when the saved state of the category tree control on
// re-entering the dialog was something other than "All"
// 
// 65    2/15/99 3:18p Gbeddow
// fix bug that caused crash in release version bringing up the Art
// Gallery dialog; performance improvements related to inserting
// categories in the Art Gallery tree control
// 
// 64    2/12/99 5:42p Gbeddow
// replace category drop-down control with tree control in the art gallery
// and web art gallery dialogs
// 
// 63    2/01/99 11:39a Rgrenfel
// Added GIF animation preview to the Web Art Gallery
// 
// 62    1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 61    1/25/99 1:20p Gbeddow
// support for the latest spec'd list of graphic types and initial support
// for a new web art browser dialog
// 
// 60    1/05/99 5:31p Gbeddow
// art gallery (graphic browser) support for Print Shop-style graphic type
// filtering
// 
// 59    11/24/98 1:48p Dennis
// Fixed problem in sentiment gallery when switching categories would
// cause no sentiments to be displayed.  Problem new to PM release 7.01
// 
// 58    11/18/98 10:33p Psasse
// New IsItemOKToView function (concerning unlicensed large thumbs)
// 
// 57    11/17/98 5:22p Mwilson
// removed online art gallery button and menu for crafts deluxe
// 
// 56    10/23/98 5:41p Johno
// Changes for new online art button
// 
// 55    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 54    10/08/98 4:51p Psasse
// added online menu options to the project browser
// 
// 53    10/02/98 9:08p Psasse
// 
// 52    10/02/98 7:10p Jayn
// More content manager changes
// 
// 51    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 50    9/03/98 5:27p Mwilson
// Changed GetSentiment to return a bool
// 
// 49    8/19/98 12:11p Johno
// Made MakeThumbnail() static; it is not all that browser spacific - it
// can be used other places.
// 
// 48    8/11/98 9:34a Johno
// Compile update
// 
// 47    8/10/98 4:46p Hforman
// changes for cropping copyright off AG images
// 
// 46    7/24/98 10:17p Hforman
// add menu update handlers
// 
// 45    7/23/98 10:43a Hforman
// 1) modified "sample" item display
// 2) fixed categories for supercollections & identical categories in
// multiple collections
// 
// 44    7/10/98 8:54p Hforman
// - fix problem with "All" not at top of category list in Art Gallery
// - finished arrow/page functionality in Project Gallery
// 
// 43    6/30/98 6:04p Hforman
// add CalcItemRows()
// 
// 42    6/24/98 7:18p Psasse
// Fix crash bug when copy/paste from a closed browser
// 
// 41    6/17/98 10:42a Hforman
// put in Any/All functionality for Find
// 
// 40    5/27/98 7:02p Hforman
// remove Welcome stuff, unused colors, etc.
// 
// 39    5/22/98 11:38a Hforman
// more work on Sentiment Gallery
// 
// 38    5/22/98 11:18a Fredf
// Ported 4.0 code over to print image index.
// 
// 37    5/21/98 3:27p Hforman
// 
// 36    5/20/98 6:51p Hforman
// Added super collection mechanism
// 
// 35    5/19/98 3:01p Hforman
// 
// 34    5/12/98 4:09p Hforman
// remove ProductCode from GetItemInformation(); tweaked "Online" text
// 
// 33    5/11/98 9:07p Psasse
// content manager becomes a member of the app
// 
// 32    5/11/98 8:48a Dennis
// Added SetStartupCollection
// 
// 31    5/09/98 9:08p Psasse
// 
// 30    5/06/98 6:05p Hforman
// removed NewConfirmDeleteDialog() funtions
// 
// 29    4/30/98 10:23p Fredf
// Support for "Special Collection" button used for American Greetings.
// 
// 28    4/30/98 7:10p Hforman
// made CopyFileToUserArtDir() part of CArtBrowserDialog not
// CBrowserDialog
// 
// 27    4/23/98 9:11p Psasse
// 
// 26    4/23/98 4:14p Psasse
// 
// 25    4/22/98 9:00p Psasse
// 
// 24    4/20/98 1:00p Fredf
// No Matching Items.   Remember to move string to resource file!
// 
// 23    4/17/98 2:30p Psasse
// ADDED ON-LINE FUNCTIONALITY
// 
// 22    4/14/98 10:01a Dennis
// Removed Media and Style. Added Free Sample logic to
// GetUnlicensedCaption().
// 
// 21    4/08/98 6:44p Hforman
// interim checkin - more later
// 
// 20    4/07/98 8:46a Jayn
// To implement dragged formats
// 
// 19    4/06/98 11:18a Hforman
// 
// 18    3/31/98 4:45p Hforman
// more work on project groups
// 
// 17    3/27/98 2:14p Hforman
// work-in-progress -- project types shown in preview area, etc.
// 
// 16    3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
// 15    3/19/98 1:08p Hforman
// 
// 14    3/12/98 4:39p Hforman
// add "Project Type" combo to project gallery; add in-place editing of
// user friendly names
// 
// 13    3/06/98 7:10p Hforman
// interim checkin -- hope I didn't break things too badly :-)
// 
// 12    3/06/98 1:18p Hforman
// 
// 11    3/03/98 9:37p Hforman
// 
// 10    2/27/98 7:08p Hforman
// interim checkin - work in progress
// 
// 9     2/27/98 11:16a Hforman
// initial changes for new design
// 
// 8     2/25/98 5:44p Hforman
// border stuff; browser stuff
// 
// 7     2/06/98 11:34a Jayn
// More complete automation support.
// 
// 6     1/22/98 7:18p Hforman
// add Paste functionality; add "Copy Local" option on Import Art
// 
// 5     1/15/98 3:59p Hforman
// changed for DragAndDrop; remove old WIN16 code
// 
// 4     1/06/98 2:42p Hforman
// added forward declaration
// 
// 3     12/23/97 10:27a Jayn
// 
// 2     12/12/97 11:24a Jayn
// Fixed Class Wizard problem.
// 
//    Rev 1.3   21 Oct 1997 15:45:50   hal
// 
// modified to allow drawing of large thumbnails to be interrupted
// 
//    Rev 1.2   18 Sep 1997 15:45:40   johno
// Browser reset button
// 
// 
//    Rev 1.1   17 Sep 1997 09:10:44   Jay
// Change to make 5.0 compile this.
// 
//    Rev 1.0   14 Aug 1997 15:18:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:46   Fred
// Initial revision.
// 
//    Rev 1.57   09 Jul 1997 09:23:20   Jay
// Now deletes files used for drag/drop
// 
//    Rev 1.56   09 Jul 1997 09:08:14   dennisp
// Added support for LONG number of results. Added inproved handling when CD 
// is requested but user hits cancel.  Cleared text controls for empty result
// set in sentiment browser.
// 
//    Rev 1.55   02 Jul 1997 10:03:14   Jay
// Passes graphic parameters through to Open From Disk
// 
//    Rev 1.54   02 Jul 1997 09:30:52   Jay
// Address book choose picture now works
// 
//    Rev 1.53   02 Jul 1997 09:03:24   Jay
// Modal picture selection
// 
//    Rev 1.52   27 Jun 1997 17:11:12   dennisp
// Added Edit/Copyu support
// 
//    Rev 1.51   27 Jun 1997 16:04:52   Jay
// Photo CD
// 
//    Rev 1.50   25 Jun 1997 14:51:32   dennisp
// Added support to filter out empty categories based on product flags
// 
//    Rev 1.49   20 Jun 1997 14:12:12   dennisp
// Added state logic for Last Item and keyword, removed empty user category from 
// category list, added all support to Sentiments, misc fixes.
// 
//    Rev 1.48   17 Jun 1997 12:42:40   Jay
// Import; OpenStreams()
// 
//    Rev 1.47   16 Jun 1997 13:51:26   johno
// 
//    Rev 1.46   13 Jun 1997 13:49:18   dennisp
// Added logic to fill in Category combo when "All" is selected for a collection.
// Added state logic for keywords.
// 
//    Rev 1.45   11 Jun 1997 11:42:54   Jay
// Licensing, etc.
// 
//    Rev 1.44   09 Jun 1997 18:18:24   doberthur
// Change CProjectBrowserDialog to support Your Project help dialog.
// Changed  CProjectBrowserDialog constructor to default to Ready Made
// help dialog.
// 
//    Rev 1.43   09 Jun 1997 14:32:28   dennisp
// Added capability to goto a category at startup for "Your Own"
// 
//    Rev 1.42   06 Jun 1997 14:41:24   Jay
// HasOrderForm
// 
//    Rev 1.41   06 Jun 1997 11:51:26   Jay
// Licensing changes
// 
//    Rev 1.40   06 Jun 1997 09:14:28   dennisp
// Added "All" to project types
// 
//    Rev 1.39   05 Jun 1997 16:21:20   doberthur
// No change.
// 
//    Rev 1.38   05 Jun 1997 15:37:14   dennisp
// Better licensing support, removed obsolete code
// 
//    Rev 1.37   04 Jun 1997 15:51:18   dennisp
// Added state logic for combos and window pos, Enhanced GetSentiment, generalized GetSelections
// 
//    Rev 1.36   03 Jun 1997 11:18:40   dennisp
// Added GotoItem() method and now use it when switching views
// 
//    Rev 1.35   02 Jun 1997 15:21:42   dennisp
// Added check for content existance before allowing dialog creation to continue
// 
//    Rev 1.34   01 Jun 1997 13:49:20   dennisp
// Improved item selection
// 
//    Rev 1.33   30 May 1997 21:20:46   dennisp
// Sentiment browser sizing support, better focus rect
// 
//    Rev 1.32   30 May 1997 19:16:20   Jay
// Properties, etc.
// 
//    Rev 1.31   30 May 1997 18:57:16   dennisp
// Added Item description in large views
// 
//    Rev 1.30   30 May 1997 10:18:10   Jay
// Browser import/export
// 
//    Rev 1.29   30 May 1997 10:10:14   dennisp
// Added line separator to status bar for 32 bit
// 
//    Rev 1.28   29 May 1997 14:40:22   Jay
// Delete
// 
//    Rev 1.27   29 May 1997 14:23:54   dennisp
// Added sentiment selection
// 
//    Rev 1.26   29 May 1997 09:30:20   Jay
// Export, etc.
// 
//    Rev 1.25   28 May 1997 19:39:12   dennisp
// Added sentiment browser
// 
//    Rev 1.24   28 May 1997 09:17:12   Jay
// Catalog support. Better transparent previews.
// 
//    Rev 1.23   27 May 1997 19:25:12   dennisp
// Added support for viewing pages/panels in 1 ups for browser
// 
//    Rev 1.22   23 May 1997 14:56:46   dennisp
// Status bar and drawing enhancements
// 
//    Rev 1.21   19 May 1997 20:32:32   johno
// Content licensing added to project
// 
// 
//    Rev 1.20   19 May 1997 12:47:34   dennisp
// Added StatusBar
// 
//    Rev 1.19   16 May 1997 11:42:54   dennisp
// 16 bit fixes for dialog message processing
// 
//    Rev 1.18   15 May 1997 14:58:42   dennisp
// Added 16 bit accelerator support
// 
//    Rev 1.17   15 May 1997 10:56:56   dennisp
// 
//    Rev 1.16   15 May 1997 10:22:40   dennisp
// Added keyboard support to browser
// 
//    Rev 1.15   13 May 1997 11:47:02   johno
// 
//    Rev 1.14   12 May 1997 18:08:36   Jay
// Browser drag & drop
// 
//    Rev 1.13   09 May 1997 11:33:32   johno
// Welcome color set in art gallery ctor
// 
//    Rev 1.12   07 May 1997 17:17:30   dennisp
// Added capability to change colors in browsers from .INI file.
// 
//    Rev 1.11   06 May 1997 15:32:00   Jay
// Initial multi-collection searches
// 
//    Rev 1.10   01 May 1997 16:03:40   Fred
// includes ctl3d.h
// 
//    Rev 1.9   01 May 1997 16:00:04   Fred
// Does not allow CTL3D to subclass browser dialog.
// 
//    Rev 1.8   24 Apr 1997 17:34:50   Fred
//  
// 
//    Rev 1.7   24 Apr 1997 15:10:58   Fred
//  
// 
//    Rev 1.6   24 Apr 1997 11:04:24   Fred
//  
// 
//    Rev 1.5   23 Apr 1997 18:33:54   Fred
//  
// 
//    Rev 1.4   23 Apr 1997 17:15:34   Jay
// DoSelect for project and picture browsers.
// 
//    Rev 1.3   23 Apr 1997 16:16:14   Fred
//  
// 
//    Rev 1.2   23 Apr 1997 11:38:18   Fred
//  
// 
//    Rev 1.1   23 Apr 1997 10:20:30   Fred
// More browser stuff
// 
//    Rev 1.0   22 Apr 1997 12:19:28   Fred
// Initial revision.
*/

#ifndef __BROWSER_H__
#define __BROWSER_H__

#include "pdlgclr.h"
#include "tcombo.h"
#include "btnnrect.h"
#include "redisp.h"			// for RedisplayContext
#include "refresh.h"			// for CRefreshArray
#include "pmwcoll.h"
#include "colldrw.h"

// forward declarations
class CPmwView;
class CBrowserDataSource;
class CPrintMasterLicenseDll;
class GraphicObject;
class CArtOleDropTarget;
class CEditUserTitle;
class CThumbnailDataEntry;
class CCategoryDir;

struct REFRESH_EXTENT;

typedef struct
{
	UINT uStringID;
	DWORD dwBitmask;
} ToneStyleMediaTableEntry;

//////////////////////////////////////////////////////////////////////////
// CPreviewArray

class CPreviewArray : public CPtrArray
{
	INHERIT(CPreviewArray, CPtrArray)

public:
	// Array item
	class Item
	{
	public:
		Item(USHORT uCollectionID, DWORD dwItemNumber, int nResultIndex,
			  void* pThumbnailData, const CString& csCaption, const CString& csAccessName,
			  const CRect& crThumbnail, const CRect& crFocusArea,
			  BOOL bIsLicensed, BOOL bTransparent);
		virtual ~Item();

	public:
      BOOL     m_bIsLicensed;
		USHORT   m_uCollectionID;
		DWORD    m_dwItemNumber;
		int		m_nResultIndex;	// index of this item in result array
		void*    m_pThumbnailData;
		CString  m_csCaption;
		CString  m_csAccessName;
		CRect    m_crThumbnail;		// thumbnail bounds of item
		CRect		m_crFocusArea;		// focus area bounds of item (entire preview area)
		CRect		m_crCaption;		// caption area below thumbnail (bottom of preview)
		BOOL		m_bTransparent;	// does thumbnail contain transparency?
	};

public:

	// construction and destruction
	CPreviewArray();
	virtual ~CPreviewArray();
	
public:

	void DeleteAll(void);
	void DeleteAt(int nIndex, int nCount = 1);

	Item* GetAt(int nIndex) const
		{ return (Item*)(INHERITED::GetAt(nIndex)); }
	Item*& ElementAt(int nIndex)
		{ return (Item*&)(INHERITED::ElementAt(nIndex)); }
	Item* operator[](int nIndex) const
		{ return (Item*)(INHERITED::GetAt(nIndex)); }
	Item*& operator[](int nIndex)
		{ return (Item*&)(INHERITED::ElementAt(nIndex)); }

	int Add(USHORT uCollectionID, DWORD dwItemNumber, int nResultIndex,
			  void* pThumbnailData, const CString& csCaption, const CString& csAccessName,
			  const CRect& crThumbnail, const CRect& crFocusArea,
			  BOOL bIsLicensed, BOOL bTransparent);

	int Add(Item* pItem)
		{ return INHERITED::Add(pItem); }
	int Find(USHORT uCollectionID, DWORD dwItemNumber);
	int Find(CPoint Point);
};

/////////////////////////////////////////////////////////////////////////////
// CBrowserDataExchange

struct CBrowserDataExchange
{
	DWORD m_dwFilePathOffset;		// (Relative to start of structure)
	DWORD m_dwFriendlyNameOffset;	// (Relative to start of structure)
	DWORD m_dwCropCopyrightOffset;
};

/////////////////////////////////////////////////////////////////////////////
// CBrowserStatusBar window

class CBrowserStatusBar : public CStatusBar
{
// Construction
public:
	CBrowserStatusBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserStatusBar)
	//}}AFX_VIRTUAL
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);

// Implementation
public:
	virtual        ~CBrowserStatusBar();
   virtual BOOL   PreCreateWindow(CREATESTRUCT &cs);
   int            GetCount();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrowserStatusBar)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBrowserDialog

class CBrowserDialog : public CPmwDialogColor
{
	INHERIT(CBrowserDialog, CPmwDialogColor)
	
// Construction
public:
	CBrowserDialog(UINT uID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBrowserDialog();
   virtual BOOL DestroyWindow();

// Operations
   void         SetStartupCollection(LPCSTR szCollectionFriendlyName)
      {  m_csStartupCollection = szCollectionFriendlyName; }
   void         SetStartupCategory(LPCSTR szStartupCategory)
      {  m_csStartupCategory = szStartupCategory; }

	// You can set this to force a document.
	CPmwDoc*		m_pDoc;

	void ChangeFriendlyName(CPMWCollection* pCollection, DWORD dwItemNum,
									const CString& strNewFriendlyName);
	static  void* MakeThumbnail(USHORT uCollectionID, DWORD dwItemNumber, const CRect& crBounds, CThumbnailDataEntry& Thumbnail, void* pData, BOOL* bTransparent);

	IPathManager* GetPathManager(void)
		{ return m_pPathManager; }
	ICollectionManager* GetCollectionManager(void)
		{ return m_pCollectionManager; }
	IContentManager* GetContentManager(void)
		{ return m_pContentManager; }

// Dialog Data
	//{{AFX_DATA(CBrowserDialog)
	int		m_fMatchAny;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrowserDialog)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSelchangeCollectionCombo();
	afx_msg void OnSelchangeGraphicTypeCombo();
	afx_msg void OnSelchangeProjectTypeCombo();
	afx_msg void OnSelchangeCategoryCombo();
	afx_msg void OnSelchangeToneCombo();
	afx_msg void OnSelchangeKeywordList();
	afx_msg void OnFind();
	afx_msg void OnFindAnyAll();
	afx_msg void OnSelect();
   afx_msg void OnReset();
   afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
   afx_msg void OnUpdateEditOrganizeCategories(CCmdUI *pCmdUI);
	afx_msg void OnEditOrganizeCategories();
	afx_msg void OnPrint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPageDown();
	afx_msg void OnUpdatePageDown(CCmdUI *pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI *pCmdUI);
	afx_msg void OnDown();
	afx_msg void OnPageUp();
	afx_msg void OnUpdatePageUp(CCmdUI *pCmdUI);
	afx_msg void OnUp();
	afx_msg void OnLeft();
	afx_msg void OnRight();
	afx_msg void OnHome();
	afx_msg void OnEnd();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnUpdateSize(CCmdUI *pCmdUI);
	afx_msg void OnChangeSize();
	afx_msg void OnUpdatePage(CCmdUI *pCmdUI);
	afx_msg void OnChangePage();
	afx_msg void OnUpdateItem(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewSmall(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewMedium(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewLarge(CCmdUI *pCmdUI);
	afx_msg void OnViewMedium();
	afx_msg void OnViewSmall();
	afx_msg void OnViewLarge();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnExport();
	afx_msg void OnUpdateExport(CCmdUI *pCmdUI);
	afx_msg void OnImport();
	afx_msg void OnUpdateImport(CCmdUI *pCmdUI);
	afx_msg void OnProperties();
	afx_msg void OnUpdateProperties(CCmdUI *pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnClose();
   afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnKillfocusEditTitle();
	afx_msg void OnSpecialCollection();
	afx_msg void OnOnlineArt();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg BOOL OnOrderForm(UINT nID);
	afx_msg void OnUpdateOrderForm(CCmdUI *pCmdUI);
//	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
   enum ThumbnailSizes
   {
      thumbsizeSmall,
      thumbsizeMedium, 
      thumbsizeLarge 
   };

	virtual void UpdateAllBrowsers(void);
	virtual void UpdateBrowser(void);
   virtual void AddCategories(CPMWCollection *pCollection, CCategoryDir* pCategoryDirectory, int nCategory, CTreeComboBox* pComboBox, void* pItemList, int nDepth);
   virtual void AddAllCategories(int nStartColl=-1, int nEndColl=-1);

	virtual void OnOK();

	BOOL CreateTreeComboBox(UINT uID, CTreeComboBox*& pComboBox, ToneStyleMediaTableEntry* pInitializeTable);
	virtual CTreeComboBox* NewTreeComboBox(UINT uID);

	virtual CPmwDoc* GetDocument(void);
	virtual CPmwView* GetView(void);

	virtual void ComputeLayout(void);
	virtual void PositionControls();
	virtual void PaintPreview(CDC* pDC);
	virtual void PaintBkgnd(CDC* pDC);

	virtual int CollectionType(void) = 0;
	virtual int CollectionSubType(void) = 0;

	virtual void ProcessCollections(void);
	virtual void ProcessCollection(CPMWCollection* pCollection);

	virtual void CollectionChanged(void);
	virtual void GraphicTypeChanged(void);
	virtual void ProjectTypeChanged(void) { AddAllCategories(-1, -1); }
	virtual void CategoryChanged(void);
	virtual void ToneChanged(void);
	virtual void KeywordChanged(int nIndex);

	virtual CPMWCollection* GetCollectionSelection();
	virtual int GetGraphicTypeSelection(void);
	virtual void GetCategorySelection(CString& csCategories);
	virtual void GetKeywordSelection(CString& csKeywords);
	virtual int GetToneSelection(void);

	virtual BOOL NoMatchingItems(void);

	virtual void UpdateResultSet(void);
   virtual ERRORCODE SetThumbnailView(ThumbnailSizes thumbsizeNew);
   virtual BOOL SupportsThumbnailSize(ThumbnailSizes thumbsizeNew);

	void			SetItemsPerGroup();
	void			CalcRowsPerGroup();
	int			CalcNumGroupItems(int nFirstRow, int nNumRows);
	int			CalcGroupStartingItem(int nFirstRow);
	int			CalcRowFromItemNumber(int nItem);
	int			CalcColumnFromItem(CPreviewArray::Item* pItem);
	int			CalcItemRows();

	virtual void GetPreviewBounds(CRect& crThumbnail, CRect& crFocus, int nThumbnailWidth, int nThumbnailHeight,
											int nItemPos, int nGroup = 0, int nLastGroupBottomPos = 0);
	virtual void UpdateItemPreviews(BOOL fReset);
	virtual void UpdateScrollBar(void);
	virtual int GetScrollPos(void){return INHERITED::GetScrollPos(SB_VERT);}
	virtual void SetPreviewRect();

	virtual void DrawThumbnailPreview(CDC& dc, CPreviewArray::Item* pItemPreview);
	virtual void SelectItem(USHORT uCollectionID, DWORD dwItemNumber);
	virtual void InvalidateItem(USHORT uCollectionID, DWORD dwItemNumber);

	virtual void DoSelect(void) = 0;
	virtual void DoPrint(void) = 0;
	virtual COleDataSource* CreateDragDropSource(void) = 0;
	virtual UINT GetDataExchangeFormat(void) const = 0;

	// Export section.
	virtual void SetLastExportName(LPCSTR pszLastName) = 0;
	virtual void GetExportDirectory(CString& csDir) = 0;

	virtual BOOL GetItemInformation(USHORT uCollectionID, DWORD dwItemNumber, CString& csCollection,
											  CString& csFile, CString& csFriendlyName, CString& csAccessName,
											  CString* pcsKeywords = NULL, BOOL* pfCropCopyright = NULL);

   BOOL         LoadAccelTable(LPCSTR lpszResourceName);
   void         LoadColorsFromINI(const CString &strColorSection);
   COLORREF     GetColorFromINI(const CString &strSection,
										  const CString &strEntryToGet, COLORREF crDefault);
   void         SaveColorToINI(const CString &strSection,
										 const CString &strEntryToSave, COLORREF crColor);
   virtual void UpdatePreview(BOOL bSelectFirstPreviewItem = TRUE);
   virtual void InvalidatePreview();
   void         DrawSelect(CDC& dc, CPreviewArray::Item* pItemPreview, BOOL fSelected);
	void         DisplayGroupHeading(CDC* pDC, CString heading, int nYPos, BOOL fContinued=FALSE);
	void         EditItemTitle(CPreviewArray::Item* pItem);
  	virtual BOOL         SetIndicatorSize(int nIndex);
	virtual void HandleSpecialCollectionButtons(void);
	BOOL			 GetArtButtonRect(CRect &r, int resID);
	virtual BOOL ShowArtButton() = 0;
	void			 HandleOrderForms(void);
   void         GetFrameRect(int nID, CRect &cr3DBorder);

   int          GotoItem(USHORT nID, DWORD dwItemNumber, BOOL fRefresh = TRUE);
   virtual int  RestoreState(void);
   virtual int  SaveState(void);
   virtual BOOL SaveTreeComboState(CTreeComboBox* pTreeCombo, LPCSTR szEntry);
   virtual BOOL RestoreTreeComboState(CTreeComboBox* pTreeCombo, LPCSTR szEntry);
   BOOL         GetUnlicensedCaption(CPMWCollection* pCollection,
                 DWORD dwItemNumber, CString &csCaption);

	BOOL AddDataExchangeFormat(CBrowserDataSource* pSource);

	BOOL SelectedItemIsLicensed(void);
	BOOL IsItemLicensed(CPreviewArray::Item* pItem);
	BOOL IsItemOKToView(CPreviewArray::Item* pItem);

	void OnClickedUnlicensed(void);
	void DoLicense(CPMWCollection* pCollection, const CString* pcsProductCode = NULL);
	virtual BOOL CanClose(void);

	BOOL FinishOrder(void);
   void Refresh();
   BOOL DrawLargeImage(REFRESH_EXTENT* pre, CDC* pDC);
	CPalette* GetThumbnailPalette();
   void DrawThumbnails(CDC* pDC);
	virtual int  GetStatusBarHeight();
	virtual void CreateStatusBar();
	virtual void SizeBrowserWindow();
	void CancelEditTitle();
	void RemoveWordsFromString(CString* strToModify, const CString& strWords);

protected:
   CString  m_csINISection;   // Section name for this browser
   CString  m_csKeywordNone;  // Text used to represent "None" in keyword combo
   CString  m_csStartupCategory; // Category name browser starts with
   CString  m_csStartupCollection; // Collection name browser starts with

	CRect m_crMinimum;
	CRect m_crClient;
	CRect m_crSidebar;
	CRect m_crPreview;
	CRect m_crKeywordControls;
   BOOL  m_fIsMaximized;      // is dialog maximized?
   BOOL  m_bAgProduct;      // is this an ag product

   CFont* m_pPreviewFont;     // font used for painting caption text

	CTreeComboBox* m_pCollectionList;
	CTreeComboBox* m_pGraphicTypeList;
	CTreeComboBox* m_pProjectTypeList;
	CTreeComboBox* m_pCategoryList;
	CTreeComboBox* m_pToneList;
   HRGN           m_hrgnPreviewClip;

	CEditUserTitle* m_pEditTitle;
	BOOL				m_fEditingTitle;

	// bitmaps drawn onto dialog
	CBitmap			m_bmpSidebarBkgrnd;
	CBitmap			m_bmpSidebarBorder;
	CBitmap			m_bmpTitle;
	CBitmap			m_bmpPreviewBkgrnd;

   // refresh variables
	RedisplayContext	rc;					/* Screen context */
   BOOL           m_fRefreshSemaphore;
	CRefreshArray  m_RefreshArray;

	CPalette* m_pThumbnailPalette;	// palette used for transparent thumbnails

	COLORREF m_clPreviewText;			// text color for preview captions

	USHORT	m_uSelectedCollectionID;
	DWORD		m_dwSelectedItemNumber;

	USHORT	m_uEditedCollectionID;
	DWORD		m_dwEditedItemNumber;

	CPreviewArray* m_pItemPreviews;

	int		m_nFirstRow;				// First item row shown in preview area
	long		m_nResultItems;			// Total number of items in result set
	int		m_nScrollInc;

	CSize		m_ThumbnailSize;
	CSize		m_ThumbnailMargin;      
	CSize		m_PreviewMargin;         
	CSize		m_ThumbnailLayout;
	CSize		m_MediumThumbnailSize;

   int		m_nCaptionHeight;
	int		m_nHeadingBarHeight;		// height of heading bar for Group Headings
	int		m_nFullThumbnailRows;
   int		m_nItemRows;				// Number of rows taken up by the items.
	int		m_nCurColumn;				// current preview column selected

	// Preview Item Groups
	BOOL		m_fHasGroups;			// multiple groups?
	BOOL		m_fShowHeadings;		// show group headings?
	struct GroupInfo
	{
		CString	strGroup;	// name of group
		int		nCount;		// number of items in this group
		int		nRows;		// number of rows this group currently takes up
	};
	CArray<GroupInfo*,GroupInfo*> m_ItemGroupInfo;

	BOOL		m_fIgnoreSelectionChange;
	BOOL		m_fUpdatePreview;

   HACCEL   m_hAccelTable;
   int      m_nCurPreviewItem;

	int		m_nPopupMenuIndex;

   CBrowserStatusBar m_StatusBar;
   enum ThumbnailSizes m_thumbsize;

   // Current Document (in the Draw object).
	CPMCollectionDraw m_CollectionDraw;

	// The search context.
	CPMWCollectionSearchContext m_SearchContext;

	// The licensing DLL.
	CPrintMasterLicenseDll* m_pLicenseDll; 
	CPMWCollection* m_pCurrentLicenseCollection;
	CPtrArray m_cpaOrderFormCollections;

	// The "special collection" name.
	CString m_csSpecialCollection;
	CNonRectButton *m_pSpecialCollectionButton, *m_pOnlineArtButton;

	// Interfaces.
	IPathManager* m_pPathManager;
	ICollectionManager* m_pCollectionManager;
	IContentManager* m_pContentManager;
};

/////////////////////////////////////////////////////////////////////////////
// CProjectBrowserDialog

class CProjectBrowserDialog : public CBrowserDialog
{
	INHERIT(CProjectBrowserDialog, CBrowserDialog)

// Construction
public:
	CProjectBrowserDialog(UINT nID = 0, CWnd* pParent = NULL); 

// Static formats this dialog drags and drops.
	static UINT m_cfDataExchange;

// Dialog Data
	//{{AFX_DATA(CProjectBrowserDialog)
	enum { IDD = IDD_GALLERY_PROJECT };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectBrowserDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectBrowserDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnFileOpenfromdisk();
	afx_msg void OnFileClosebrowser();
	afx_msg void OnUpdateView(CCmdUI *pCmdUI);
	afx_msg void OnChangeView();
	afx_msg void OnViewCardFront();
	afx_msg void OnViewCardInside();
	afx_msg void OnViewCardBack();
	afx_msg void OnViewBrochureOutside();
	afx_msg void OnViewBrochureInside();
	afx_msg void OnViewNewsLetterGoto();
	afx_msg void OnViewNewsLetterNext();
	afx_msg void OnViewNewsLetterPrev();
	afx_msg void OnViewNewsLetterFirst();
	afx_msg void OnViewNewsLetterLast();
	afx_msg void OnOnlineUpdateAccount();
	afx_msg void OnOnlineCreateAccount();
	afx_msg void OnOnlineChangeAccounts();
	afx_msg void OnOnlineDeleteAllDownloadedFiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

   virtual void AddAllCategories(int nStartColl=-1, int nEndColl=-1, int nCurProject=0);

	// display all project collections
	virtual int CollectionType(void) { return CPMWCollection::typeProjects; }
	virtual int CollectionSubType(void) { return CPMWCollection::subTypeNone; }

	virtual void UpdateBrowser(void);
	virtual void ProcessCollection(CPMWCollection* pCollection);
	virtual void CollectionChanged(void);
	virtual void ProjectTypeChanged(void);
	virtual void GetCategorySelection(CString& csCategories);
	virtual void SetStatusBarIndicators();
	virtual void LoadMenu();

	virtual void DoSelect(void);
	virtual void DoPrint(void);
	virtual COleDataSource* CreateDragDropSource(void);
	CString GetPrintCategory(USHORT uCollectionID, DWORD dwItemNumber);

	// Export section.
	static CString m_csLastExport;
	virtual void SetLastExportName(LPCSTR pszLastName);
	virtual void GetExportDirectory(CString& csDir);

	virtual UINT GetDataExchangeFormat(void) const
		{ return m_cfDataExchange; }
	
	virtual BOOL ShowArtButton();
};

/////////////////////////////////////////////////////////////////////////////
// CProjectBrowserDialog

class CAGProjectBrowserDialog : public CProjectBrowserDialog
{
	INHERIT(CAGProjectBrowserDialog, CProjectBrowserDialog)

// Construction
public:
	CAGProjectBrowserDialog(CWnd* pParent = NULL); 

// Dialog Data
	//{{AFX_DATA(CProjectBrowserDialog)
	enum { IDD = IDD_GALLERY_AGPROJECT };
	CScrollBar m_ctrlScrollBar;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectBrowserDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectBrowserDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	virtual void PaintBkgnd(CDC* pDC);
	virtual void PositionControls();
	virtual int  GetStatusBarHeight(){return 0;}//no status bar in ag browser
	virtual void CreateStatusBar(){}
	virtual void SetStatusBarIndicators(){}
   virtual BOOL SetIndicatorSize(int nIndex){return TRUE;}
	virtual void SizeBrowserWindow(){}
	virtual void LoadMenu(){}
	virtual void UpdateScrollBar(void);
	virtual int GetScrollPos(void){return m_ctrlScrollBar.GetScrollPos();}
	virtual void SetPreviewRect();


};

/////////////////////////////////////////////////////////////////////////////
// CArtBrowserDialog

class CArtBrowserDialog : public CBrowserDialog
{
	INHERIT(CArtBrowserDialog, CBrowserDialog)
	
// Construction
public:
	enum ArtBrowserType
	{
		TYPE_ArtGallery,
		TYPE_ModalPicture				// Special "choose a picture" cases.
	};

	CArtBrowserDialog(CWnd* pParent = NULL);
   CArtBrowserDialog(ArtBrowserType nType, CWnd* pParent = NULL);
   CArtBrowserDialog(UINT uID, CWnd* pParent);
   virtual ~CArtBrowserDialog();

	virtual void UpdateAllBrowsers(void);

	// copy imported file to local user art directory
	BOOL CopyFileToUserArtDir(LPCSTR origFilename, CString& newFilename,
									  BOOL fPromptOverwrite, int* overwriteRetval);

	// Static format this dialog drags and drops.
	static UINT m_cfDataExchange;

// Dialog Data
	//{{AFX_DATA(CArtBrowserDialog)
	enum { IDD = IDD_GALLERY_ART };
	CTreeCtrl	m_CategoryTreeCtrl;
	//}}AFX_DATA

	// Used to pass parameters to Open From Disk.
	LPCSTR m_pszLastPicture;
	GraphicObject* m_pLastPicture;

	// Set to the file and friendly name when a piece of art is chosen.
	// This is useful for those cases where just the name is desired.
	CString m_csChosenFileName;
	CString m_csChosenFriendlyName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArtBrowserDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CArtBrowserDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnFileOpenfromdisk();
	afx_msg void OnFileClosebrowser();
	afx_msg void OnFileOpenPhotoCD();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnOnlineUpdateAccount();
	afx_msg void OnOnlineCreateAccount();
	afx_msg void OnOnlineChangeAccounts();
	afx_msg void OnOnlineDeleteAllDownloadedFiles();
	afx_msg void OnSelchangedCategoryTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMasterArt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CArtOleDropTarget* m_pDropTarget;

	ArtBrowserType m_nType;

	CNonRectButton *m_pMasterArtButton;

	void CommonConstruct(ArtBrowserType nType);
	virtual void HandleSpecialCollectionButtons(void);

   virtual void AddCategories( CPMWCollection *pCollection, CCategoryDir* pCategoryDirectory, int nCategory, int nDepth, HTREEITEM hTreeCtrlParent );
   virtual void AddAllCategories( int nStartColl=-1, int nEndColl=-1 );

	virtual void GetCategorySelection( CString& csCategories );
	virtual void SetCategorySelection( CString& csCategories );

	BOOL RestoreCategoryTreeState( LPCSTR szEntry );
	virtual int RestoreState();

	BOOL SaveCategoryTreeState( LPCSTR szEntry );
	virtual int SaveState();

	// display subTypeArt collections only - no other subtypes such as subTypeWebArt
	virtual int CollectionType(void) { return CPMWCollection::typeArt; }
	virtual int CollectionSubType(void) { return CPMWCollection::subTypeArt; }

	virtual void CollectionChanged(void);
	virtual void UpdateBrowser(void);

	virtual void DoSelect(void);
   virtual void BringContentIntoWorkspace(CString csPath, CString csFriendlyName, BOOL fHasCopyright = FALSE);

	CString GetPrintCategory(USHORT uCollectionID, DWORD dwItemNumber);
	virtual void DoPrint(void);
	virtual COleDataSource* CreateDragDropSource(void);

	// Export section.
	static CString m_csLastExport;
	virtual void SetLastExportName(LPCSTR pszLastName);
	virtual void GetExportDirectory(CString& csDir);
	virtual UINT GetDataExchangeFormat(void) const
		{ return m_cfDataExchange; }

	void OpenFromDisk(BOOL fPhotoCD = FALSE);

	static CPtrArray m_cpaOpenBrowsers;
	virtual BOOL ShowArtButton();
};

/////////////////////////////////////////////////////////////////////////////
// CWebArtBrowserDialog

class CWebArtBrowserDialog : public CArtBrowserDialog
{
	INHERIT(CWebArtBrowserDialog, CArtBrowserDialog)
	
// Construction
public:
	CWebArtBrowserDialog(CWnd* pParent = NULL);
   virtual ~CWebArtBrowserDialog();

protected:
	// display subTypeWebArt collections only - no other subtypes such as subTypeArt
	virtual int CollectionType(void) { return CPMWCollection::typeArt; }
	virtual int CollectionSubType(void) { return CPMWCollection::subTypeWebArt; }
	virtual void SelectItem(USHORT uCollectionID, DWORD dwItemNumber);

	// Provides a data buffer for the specified file.
	UCHAR *CreateBufferFromStorageName( const CString &strAccessName, DWORD *pdwSizeOfBuffer );
	void UpdateAnimationPreview();

// Dialog Data
	//{{AFX_DATA(CWebArtBrowserDialog)
	enum { IDD = IDD_GALLERY_WEB_ART };
	afx_msg void OnAnimationPreview();
	CButton m_ctlPreview;
	//}}AFX_DATA
	DECLARE_MESSAGE_MAP()

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebArtBrowserDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
};

/////////////////////////////////////////////////////////////////////////////
// CSentimentBrowserDialog

class CSentimentBrowserDialog : public CBrowserDialog
{
	INHERIT(CSentimentBrowserDialog, CBrowserDialog)
	
// Construction
public:
	CSentimentBrowserDialog(CWnd* pParent = NULL);

// Static formats this dialog drags and drops.
	static UINT m_cfDataExchange;

// Dialog Data
	//{{AFX_DATA(CSentimentBrowserDialog)
	enum { IDD = IDD_GALLERY_SENTIMENT };
	CRichEditCtrl	m_richEditCtrlSentiment2;
	CRichEditCtrl	m_richEditCtrlSentiment1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSentimentBrowserDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   BOOL  GetSentiment(USHORT nID, DWORD dwItemNumber, CMlsStringArray   &csaSentiment);
	void	DrawSentimentPreview(CDC& dc, CPreviewArray::Item* pItemPreview,
										CMlsStringArray &csaSentiment);

	// Generated message map functions
	//{{AFX_MSG(CSentimentBrowserDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnMessageFilter(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int							m_nSentimentHeight;		// height (pixels) of sentiment box

	virtual void				PositionControls();
	virtual void				UpdateItemPreviews(BOOL fReset);

	// display all sentiment collections
	virtual int CollectionType(void) { return CPMWCollection::typeSentiments; }
	virtual int CollectionSubType(void) { return CPMWCollection::subTypeNone; }

   virtual BOOL            SupportsThumbnailSize(ThumbnailSizes thumbsizeNew);
	virtual void				ComputeLayout(void);
	virtual void				GetPreviewBounds(CRect& crThumbnail, CRect& crFocus, int nThumbnailWidth, int nThumbnailHeight,
														  int nItemPos, int nGroup = 0, int nLastGroupBottomPos = 0);
	virtual void				PaintPreview(CDC* pDC);
	virtual void				CollectionChanged(void);
	virtual void				DoSelect(void);
	virtual void				DoPrint(void);
	virtual COleDataSource*	CreateDragDropSource(void);

	// Export section.
	virtual void            SetLastExportName(LPCSTR pszLastName);
	virtual void            GetExportDirectory(CString& csDir);

	virtual UINT GetDataExchangeFormat(void) const { return m_cfDataExchange; }
	virtual BOOL ShowArtButton() {return FALSE;};
};

/////////////////////////////////////////////////////////////////////////////
// CBrowserDataSource

class CBrowserDataSource : public COleDataSource
{
public:
	CBrowserDataSource();
	virtual ~CBrowserDataSource();

	void SetItemInformation(const CString& csCollection,
									const CString& csFile,
									const CString& csFriendlyName,
									BOOL fCropCopyright,
									CBrowserDialog* pDialog);

protected:
	virtual BOOL OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile);
	virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal);

	CString	m_csCollection;
	CString	m_csFile;
	CString	m_csFriendlyName;
	BOOL		m_fCropCopyright;

	// So we can save our tmp file name.
	CBrowserDialog* m_pDialog;
};

/////////////////////////////////////////////////////////////////////////////
// CArtDataSource

class CArtDataSource : public CBrowserDataSource
{
public:
	CArtDataSource();
	virtual ~CArtDataSource();
	HGLOBAL RenderAsMetafile(void);
protected:
   virtual BOOL OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile);
   virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal);

};

/////////////////////////////////////////////////////////////////////////////
// CProjectDataSource

class CProjectDataSource : public CBrowserDataSource
{
public:
	CProjectDataSource();
	virtual ~CProjectDataSource();
protected:
   virtual BOOL OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile);
   virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal);
};

/////////////////////////////////////////////////////////////////////////////
// CSentimentDataSource

class CSentimentDataSource : public CBrowserDataSource
{
public:
	CSentimentDataSource();
	virtual ~CSentimentDataSource();
protected:
   virtual BOOL OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile);
   virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal);
};

#endif // __BROWSER_H__


/////////////////////////////////////////////////////////////////////////////
