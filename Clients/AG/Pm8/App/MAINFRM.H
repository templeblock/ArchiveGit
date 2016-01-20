/*
// $Workfile: MAINFRM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/MAINFRM.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 70    2/25/99 3:35p Johno
// Support for PhotoOrganizer toolbar button
// 
// 69    2/23/99 3:40p Johno
// 3DPlanet UI enable / disable
// 
// 68    2/01/99 3:35p Blee
// Added code to OnInitMenuPopup that will gray out a popup item if all of
// its children are grayed out.
// 
// 67    1/28/99 4:48p Mwilson
// added handlers to build new custom collections
// 
// 66    1/25/99 1:20p Gbeddow
// support for the latest spec'd list of graphic types and initial support
// for a new web art browser dialog
// 
// 65    1/20/99 10:28a Mwilson
// moved toolbars into seperate DLL
// 
// 64    12/23/98 3:48p Johno
// Preliminary 3D Planet button
// 
// 63    12/18/98 1:31p Gbeddow
// support for new hidden File menu command Preprocess Picture Gallery...
// 
// 62    11/30/98 3:43p Mwilson
// added matching sets dlg for CParty
// 
// 61    11/19/98 11:41a Johno
// Desktop online art button
// 
// 60    10/27/98 4:52p Johno
// Consolidated DoArtStore code into DoBrowserOrArtStore
// 
// 59    10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 58    10/26/98 6:47p Jayn
// Download manager.
// 
// 57    10/26/98 1:24p Johno
// Moved  OnFileOpen() from CPmwApp to CMainFrame
// 
// 56    10/23/98 5:47p Johno
// Added DoBrowserOrArtStore() to handle art store browser previewing;
// Removed unused functions / message handlers
// 
// 55    10/15/98 3:45p Dennis
// New OCX Connection Manager
// 
// 54    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 53    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 52    9/14/98 9:22a Mwilson
// added tool to print out all projects in folder
// 
// 51    9/04/98 3:54p Johno
// Changes for hidden tool ID_EXTRACT_ART_USED, OnExtractArtUsed
// 
// 50    8/05/98 11:59a Mwilson
// added new tool utility to check for art in spreadsheet
// 
// 49    7/22/98 9:13a Mwilson
// changed GetMainWindowProperty to return a CString
// 
// 48    7/14/98 12:02p Mwilson
// added override to destroyWindow to destroy HelpWindow first
// 
// 47    7/07/98 6:02p Hforman
// add On Activate() for Help window; stopped sound effects in help window
// 
// 46    7/02/98 10:14a Mwilson
// changed event reminder menu to Forget me not for AG
// 
// 45    6/30/98 7:22p Psasse
// added m_fAppClosingDown flag to eliminate dialogs from launching
// 
// 44    6/30/98 3:57p Jayn
// Program exit problems now fixed.
// 
// 43    6/29/98 12:25p Jayn
// Create OnDeviceChange message map function. Got rid of WindowProc.
// 
// 42    6/29/98 11:37a Hforman
// remove m_fInitialHub
// 
// 41    6/29/98 9:37a Mwilson
// added code for auto CD detection
// 
// 40    6/24/98 2:59p Cboggio
// Added OnUpdateFileFreeFonts to remove this menu item if required for
// localization
// 
// 39    6/18/98 4:40p Johno
// Hub is now a local - removed much unloved hub code (old sizing stuff,
// ect.)
// 
// 38    6/18/98 12:39p Johno
// Improved hub control
// 
// 37    6/18/98 11:19a Johno
// Changes for modal only hub
// 
// 36    6/17/98 12:22p Johno
// CreateHubWindow () is now an inline that sends a message handled by
// CreateHubWindowModal(). This function may handle a new, more modal, hub
// implementation.
// 
// 35    6/12/98 4:35p Mwilson
// removed perosnal delivery menu item for printmaster
// 
// 34    6/06/98 2:17p Fredf
// Help window improvements.
// 
// 33    6/05/98 8:42a Fredf
// Make IsHub and GetHub public to make debugging easier.
// 
// 32    6/04/98 7:20p Johno
// Added OnShowLegals()
// 
// 31    5/30/98 5:02p Psasse
// check-out error - nothing done
// 
// 30    5/27/98 6:52p Hforman
// yep, more welcome removal
// 
// 29    5/26/98 6:38p Psasse
// Helpful hints sound support
// 
// 28    5/21/98 9:17p Psasse
// added wave file support for hub initialization
// 
// 27    5/21/98 8:33p Fredf
// Start of online collection building.
// 
// 26    5/19/98 11:31a Dennis
// Added Run Silent mode question to GetBuildInfo
// 
// 25    5/15/98 4:42p Fredf
// 
// 24    5/14/98 10:34a Mwilson
// added utility to check for the existence of art files form pmw files
// 
// 23    5/06/98 1:22p Dennis
// Added SetHubAutoDestroy() and AutoDestroyHub() methods.
// 
// 22    5/05/98 3:51p Mwilson
// added handler for sentiment extraction
// 
// 21    4/24/98 6:29p Fredf
// Uses MFC functions SaveBarState() and LoadBarState().
// 
// 20    4/22/98 1:49p Dennis
// Added Art and More Store 
// 
// 19    4/14/98 7:59p Fredf
// First attempt at Help Window.
// 
// 18    4/14/98 10:10a Dennis
// a) Added GetBuildInfo() method for selecting build info.
// b) Enhanced OnBuild… methods to use GetBuildInfo().
// 
// 17    4/10/98 4:56p Rlovejoy
// Changes for new hub implementation.
// 
// 16    4/07/98 7:18p Fredf
// Border and drawing object toolbar buttons now mean "Add" instead of
// "Edit".
// 
// 15    3/21/98 1:18p Fredf
// New menus and toolbar names.
// 
// 14    3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 13    2/27/98 2:45p Jayn
// Added Extract Project
// 
// 12    12/23/97 10:27a Jayn
// 
// 11    12/18/97 12:45p Hforman
// any exit now goes through OnClose()
// 
// 10    12/16/97 5:44p Hforman
// helpful hint menuitem message handlers
// 
// 9     12/12/97 5:18p Hforman
// add new menu item handlers for Help menu
// 
// 8     12/04/97 12:46p Hforman
// added helpful hint stuff
// 
// 7     11/07/97 3:53p Johno
// Use values from ..\Tutorial\TutRes.h
// 
// 6     11/03/97 2:47p Johno
 * 
 * 5     10/31/97 4:59p Johno
 * Spit 'n polish
 * 
 * 4     10/31/97 2:55p Johno
 * Overloaded WinHelp
 * 
 * 3     10/31/97 10:15a Johno
 * Added OnTcard
// 
//    Rev 1.1   20 Aug 1997 17:28:38   johno
// Changes for new 5.0 buttons (preliminary)
// 
//    Rev 1.0   14 Aug 1997 15:22:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:34   Fred
// Initial revision.
// 
//    Rev 1.50   15 Jul 1997 15:56:26   Fred
// Tools are controlled by ShowTools=1; removed obsolete methods.
// 
//    Rev 1.49   14 Jul 1997 13:11:42   Jay
// BeginModalState totally disables window in 16-bit
// 
//    Rev 1.48   14 Jul 1997 11:18:26   Fred
// Added framework for 'Find projects with graphics' tool
// 
//    Rev 1.47   06 Jun 1997 11:51:26   Jay
// Licensing changes
// 
//    Rev 1.46   05 Jun 1997 18:09:48   johno
// More multiple hub stuff
// 
// 
//    Rev 1.45   05 Jun 1997 14:36:10   johno
// Changes for multiple hub support
// 
// 
//    Rev 1.44   04 Jun 1997 17:29:14   johno
// Support for multiple hubs
// 
// 
//    Rev 1.43   29 May 1997 12:54:50   johno
// Free fonts registration
// 
//    Rev 1.42   27 May 1997 17:03:38   johno
// Made public DoWelcome from OnWelcome
// 
//    Rev 1.41   23 May 1997 16:01:52   johno
// Button sound effects
// 
//    Rev 1.40   20 May 1997 16:42:22   johno
// Check for pending content licensing on close
// 
// 
//    Rev 1.39   19 May 1997 13:56:18   Fred
// Event Reminder, etc.
// 
//    Rev 1.38   08 May 1997 16:44:06   johno
// Took out OnActivate, which called DoActivate
// in the hub window. This caused the "persistent
// button" problem.
// 
//    Rev 1.37   22 Apr 1997 08:36:24   Jay
// Build menu items
// 
//    Rev 1.36   21 Apr 1997 10:51:48   johno
// Changes for welcome dialog
// 
//    Rev 1.35   18 Apr 1997 12:44:06   johno
// Took out OnBrandNew - it wasn't doing anything
// 
// 
//    Rev 1.34   31 Mar 1997 17:12:36   Jay
// Start of NFX code
// 
//    Rev 1.33   31 Mar 1997 14:56:20   johno
// Message from MDI child controls hub, toolbars
// 
// 
//    Rev 1.32   31 Mar 1997 14:34:42   johno
// Removed USE_HUB
// 
// 
//    Rev 1.31   28 Mar 1997 17:11:18   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.30   26 Mar 1997 15:57:06   Fred
// Removed some sound manager stuff
// 
//    Rev 1.29   24 Mar 1997 16:17:02   Jay
// more cleanup
// 
//    Rev 1.28   24 Mar 1997 16:15:14   Jay
// Moved hub implementation from MAINFRM.H to MAINFRM.CPP
// 
//    Rev 1.27   18 Mar 1997 13:23:20   Jay
// Preview stuff
// 
//    Rev 1.26   17 Mar 1997 17:24:02   Jay
// Preview stuff.
// 
//    Rev 1.25   17 Mar 1997 15:17:44   johno
// GetSmallestWindowSize will get the maximum clipped size
// if the smallest size is bigger than the screen.
// 
//    Rev 1.24   14 Mar 1997 17:40:10   johno
// Changes for initial beutiful window size
// 
// 
//    Rev 1.23   14 Mar 1997 11:02:52   Fred
// Export Project Text
// 
//    Rev 1.22   12 Mar 1997 17:35:10   johno
// Changes for HUB resource DLL
// 
// 
//    Rev 1.21   11 Mar 1997 16:24:14   johno
// Hub button message handlers
// 
// 
//    Rev 1.20   10 Mar 1997 17:10:38   johno
// Moved HUB control here from App class.
// 
// 
//    Rev 1.19   07 Mar 1997 17:38:34   johno
// Initial HUB in PM
// 
// 
//    Rev 1.18   28 Jan 1997 14:10:36   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.17   23 Jan 1997 11:05:46   Jay
// New popup palettes
// 
//    Rev 1.16   16 Jan 1997 13:02:50   Jay
// New status stuff. More cleanup.
// 
//    Rev 1.15   15 Jan 1997 10:49:38   Jay
// New toolbars
// 
//    Rev 1.14   01 Nov 1996 09:27:20   Jay
// Moved over from 3.01.17
// 
//    Rev 1.14   30 Oct 1996 10:29:22   Fred
// Handles second instance better
// 
//    Rev 1.13   16 Sep 1996 12:38:48   Fred
// Profile stuff can be disabled
// 
//    Rev 1.12   13 Sep 1996 09:53:08   Fred
// Profile Stuff -- will be removed
// 
//    Rev 1.11   02 Sep 1996 14:31:28   Fred
// New picture and project information dialogs
// 
//    Rev 1.10   01 Sep 1996 17:27:06   Fred
// Added double-sided print setup to main menus
// 
//    Rev 1.9   24 Aug 1996 14:14:14   Fred
// Properly focuses album control
// 
//    Rev 1.8   25 Jul 1996 13:50:06   Fred
// Sound stop, play, inspect menu items
// 
//    Rev 1.7   25 Jul 1996 11:54:00   Fred
// Added sound to menu
// 
//    Rev 1.6   24 Jul 1996 13:17:16   Fred
// Gadget hookup
// 
//    Rev 1.5   23 Jul 1996 08:08:30   Fred
// Some changes to control bars -- not finished
// 
//    Rev 1.4   16 Jul 1996 17:31:08   Fred
// More address/sender book stuff
*/

// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "error.h"
#include "statbar.h"
#include "scrollga.h"
#include "pmw.h"
#include "pmwtool.h"
#include "pmhelp.h"

#define  WM_VIEWSCHANGED   WM_USER + 0x2000 

// Maybe IDs outside this range can be used for something else
#define  IS_TUTORIAL_ID(x) ((x >= TUTORIAL_ID_FIRST) && (x <= TUTORIAL_ID_LAST))

class CTutorialDlg;
class CTutorialList;
class CFontPreviewWnd;

class CMainFrame : public CMDIFrameWnd
{
   DECLARE_DYNAMIC(CMainFrame)
   INHERIT(CMainFrame, CMDIFrameWnd)

private:

   void
   KillProjectWindow (void);

   CTutorialList*    m_pTutorialList;
   CTutorialDlg*     m_pTutorialDlg;
	CFontPreviewWnd*	m_pFontPreview;

	HINSTANCE m_hToolbarDll;
   BOOL m_fAppClosingDown;

#ifndef  WIN32
   BOOL MakeHTM (CString *cs);
#endif   

	void	ProcessPictures(BOOL bValidateOnly);

protected:
   HWND m_hwndNextViewer;

   virtual void RecalcLayout(BOOL fNotify = FALSE);
public:
   CMainFrame();

// Attributes
public:

// Operations
public:
   BOOL DoUpgrade(void);
   BOOL DoProductCatalog(void);
   BOOL WaitForDDETransfer(void);
   BOOL m_fWaitingForDDE;
   BOOL FinishCreate(void);
// Hub 
public:
   BOOL GetAppClosingDown(void) const
      { return m_fAppClosingDown; }

   void RequestHubWindow (void);

// Implementation
public:

   void ShowTutorial(int nID);
   CTutorialList* GetTutorialList() const { return m_pTutorialList; }

	// Implementation is in MAINTCRD.CPP
   virtual void WinHelp(DWORD dwData, UINT nCmd);

public:
   virtual ~CMainFrame();
#ifdef _DEBUG
   virtual  void AssertValid() const;
   virtual  void Dump(CDumpContext& dc) const;
#endif
   virtual CWnd* GetMessageBar();

	void BeginModalState(BOOL fOtherApp = FALSE);
	void EndModalState(BOOL fOtherApp = FALSE);

   CMlsSeparatorGadget& SeparatorGadget(void)
      { return m_wndSeparatorGadget; }

   CMlsPageGadget& PageGadget(void)
      { return m_wndPageGadget; }

   CMlsZoomGadget& ZoomGadget(void)
      { return m_wndZoomGadget; }

   CPmwStatusBar& StatusBar(void)
      { return m_wndStatusBar; }

	CToolBar* GetToolBar(UINT nID);

	CPmHelpWindow& HelpWindow(void)
		{ return m_wndHelpWindow; }

   BOOL InPreviewMode(void) const
      { return m_fInPreviewMode; }

   // Fixup our names according to the configuration.

   void FixupConfigurationNames(void);

   virtual BOOL PreTranslateMessage(MSG* msg);

protected:
   BOOL OnCommand(UINT wParam, LONG lParam);

   ERRORCODE GetBuildInfo(LPCSTR szSourceEntry, LPCSTR szDestEntry,
      CString *pcsINIFile,CString *pcsFileTitle,CString *pcsSourceFile,
      CString *pcsDestDir, BOOL *pbRunSilent=NULL);

	// control bar embedded members
   BOOL CreateNewToolBars(void);
   BOOL CreateSeparatorGadget(void);
   BOOL CreatePageGadget(void);
   BOOL CreateZoomGadget(void);
   BOOL CreateStatusBar(void);
	BOOL CreateHelpWindow(void);


   // Old faithful status bar.
   CPmwStatusBar        m_wndStatusBar;

	// Help window.
	CPmHelpWindow			m_wndHelpWindow;

   // Gadgets.
   CMlsSeparatorGadget  m_wndSeparatorGadget;
   CMlsPageGadget       m_wndPageGadget;
   CMlsZoomGadget       m_wndZoomGadget;

   BOOL m_fInPreviewMode;	// Are we previewing?

	BOOL DoWithHelp(void);

	virtual BOOL DestroyWindow();
// Generated message map functions
protected:
   //{{AFX_MSG(CMainFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnDestroy();
   afx_msg BOOL OnQueryNewPalette();
   afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
   afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
   afx_msg void OnFilePictureCatalog();
   afx_msg void OnDevModeChange(LPSTR lpDeviceName);
   afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
   afx_msg void OnDestroyClipboard();
   afx_msg void OnDrawClipboard();
   afx_msg void OnFontChange();
   afx_msg void OnSysColorChange();
   afx_msg void OnFileNetworkLicense();
   afx_msg void OnRenderAllFormats();
   afx_msg void OnRenderFormat(UINT nFormat);
   afx_msg void OnFileFreeFonts();
   afx_msg void OnUpdateFileFreeFonts(CCmdUI* pCmdUI);
   afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnUpdateFileOpenfromdisk(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();   
	afx_msg void OnFileOpenfromdisk();
   afx_msg void OnFilePreferences();
   afx_msg void OnUpdateFilePreferences(CCmdUI* pCmdUI);
   afx_msg void OnPrinterAlignment();
   afx_msg void OnFileUpgrade();
   afx_msg void OnUpdateFileUpgrade(CCmdUI* pCmdUI);
   afx_msg void OnViewAddressBook();
   afx_msg void OnViewSenderInformation();
   afx_msg void OnEventReminder();
   afx_msg void OnUpdateEventReminder(CCmdUI* pCmdUI);
   afx_msg void OnUpdateSoundAvailable(CCmdUI* pCmdUI);
   afx_msg void OnSoundPreferences();
   afx_msg void OnFileImportNames();
   afx_msg void OnDoubleSidedPrintSetup();
   afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnTCard(UINT idAction, DWORD dwActionData);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnHelpRecentHints();
	afx_msg void OnHelpPreferences();
	afx_msg void OnHelpEnableHints();
	afx_msg void OnUpdateHelpEnableHints(CCmdUI* pCmdUI);
	afx_msg void OnHelpEnableTips();
	afx_msg void OnUpdateHelpEnableTips(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnShowLegals();
	afx_msg void OnUpdatePersonalDelivery(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHubNew(CCmdUI* pCmdUI);
	afx_msg void OnHubNew();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnOnlinePreferences();
	afx_msg void OnUpdateOnlinePreferences(CCmdUI* pCmdUI);
	afx_msg void OnActivateApp( BOOL bActive, HTASK hTask );
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnWakeApp(WPARAM wParam, LPARAM lParam);
   afx_msg void OnTutorialChoice( UINT nID );
   afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
   afx_msg void OnWinIniChange(LPCSTR lpszSection);
   afx_msg LRESULT OnNfxTermination(WPARAM wParam, LPARAM lParam);
   afx_msg BOOL OnOurBarCheck(UINT nID);
   afx_msg void OnUpdateControlBarMenu(CCmdUI* pCmdUI);
   afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnDDEInitiate(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnDDEExecute(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnDDETerminate(WPARAM wParam, LPARAM lParam);
   afx_msg void OnTimer(UINT nIDEvent);
   afx_msg void OnUpdateProfileShow(CCmdUI* pCmdUI);
   afx_msg void OnProfileShow();
   afx_msg void OnUpdateDeleteToolMenuItem(CCmdUI* pCmdUI);
   afx_msg void OnUpdateWebArtBrowserMenuItem(CCmdUI* pCmdUI);
   afx_msg void OnExtractProjectText();
   afx_msg void OnExtractProjectSentiments();
   afx_msg void OnFindProjectsWithGraphics();
   afx_msg void OnExtractContent();
   afx_msg void OnDumpDoc();
   afx_msg void OnPrintProjects();
   afx_msg void OnWebArtBrowser();
   afx_msg void OnUpdateDumpDoc(CCmdUI* pCmdUI);
   afx_msg void OnUpdateIndicator(CCmdUI* pCmdUI);

   afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
   afx_msg void OnWithHelp();
   afx_msg void OnYourProjects();
//   afx_msg void OnArtGallery();
   afx_msg void OnExitAlbum();
   afx_msg void OnInternet();
   afx_msg void On3DPlanet();
	afx_msg void OnUpdate3DPlanet(CCmdUI* pCmdUI);
	afx_msg void OnPhotoOrganizer();
	afx_msg void OnUpdatePhotoOrganizer(CCmdUI* pCmdUI);
   afx_msg void OnAddressBook ();
   //afx_msg void OnArtAndMoreStore();
   afx_msg LRESULT OnViewsChanged(WPARAM wParam, LPARAM lParam);
   afx_msg void OnUpdateBuildBrowser(CCmdUI* pCmdUI);
   afx_msg void OnBuildProjects();
   afx_msg void OnBuildPictures();
   afx_msg void OnPreprocessPictures();
   afx_msg void OnBuildSentiments();
   afx_msg void OnBuildOnlineCollection();
   afx_msg void OnBuildArtComponentDatabase();
   afx_msg void OnBuildPageComponentDatabase();
   
   afx_msg void OnAddonInstall();
   afx_msg void OnAddonViewArt();
   afx_msg void OnAddonViewProj();
   afx_msg void OnAddonViewHTML();

   afx_msg LRESULT OnBuildFontList(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnShowFontPreview(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnMoveFontPreview(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnUpdateFontPreview(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT IsPreviewVisible(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnHubQueryAction(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCheckArtExistsDir();
	afx_msg void OnCheckArtExistsSS();
	afx_msg void OnExtractArtUsed();
   afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
   afx_msg LRESULT OnContentChanged(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnSharedPathsChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDesktopArtStore();
	afx_msg void OnUpdateDesktopArtStore(CCmdUI* pCmdUI);
public:

   VOID OIM(CMenu *pMenu) { OnInitMenu(pMenu); }
   VOID OIMP(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) { OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu); }

   void DoFileFreeFonts(void);

	void RebuildFonts(void);

   static CString GetMainWindowProperty(void);
	
	enum UseBrowser
	{
		Store,
		Art,
		Project
	};

	BOOL	DoBrowserOrArtStore(enum CMainFrame::UseBrowser e = Store, CPmwDoc *pArtDoc = NULL);
	BOOL	DoMatchingSets();

	// The message broadcast when content changes.
	static UINT m_uContentChangedMessage;
	static UINT m_uSharedPathsChangedMessage;

private:
	// helper functions for OnInitMenuPopup

	// @cmember enables/disables sub menus items based on subordinate items state
	void SetSubMenusState(CMenu* pPopupMenu, UINT nIndex);

	// @cmember returns number of enabled menu items
	int CountEnabledMenuItems(CMenu* pMenu);
};
