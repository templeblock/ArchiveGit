/*
// $Workfile: pmhelp.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/pmhelp.cpp $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 27    1/20/99 11:54a Mwilson
// fixed help window
// 
// 26    9/29/98 1:07p Mwilson
// added support for view instruction pages in HTML help window
// 
// 25    8/26/98 3:24p Mwilson
// added context help for personal delivery
// 
// 24    8/25/98 5:11p Mwilson
// fixed continuous feed hint
// 
// 23    8/24/98 5:38p Mwilson
// enabled more helpful hints
// 
// 22    8/21/98 2:38p Mwilson
// added new help hint id
// 
// 21    8/17/98 5:34p Hforman
// growing/shrinking floating help window with expand/contract
// 
// 20    8/12/98 1:58p Mwilson
// added more helphint id's
// 
// 19    8/07/98 5:10p Mwilson
// added some id's for helpful hints
// 
// 18    7/22/98 12:49p Hforman
// added IDHELP_USING_STATUS_BAR
// 
// 17    7/21/98 12:32p Rlovejoy
// Added more help topic table entries.
// 
// 16    7/17/98 9:09p Hforman
// using HH_DISPLAY_TOPIC instead of HH_HELP_CONTEXT since the latter is
// broken.
// 
// 15    7/08/98 7:55p Hforman
// various fixes, mods, etc.
// 
// 14    7/01/98 6:43p Hforman
// working on floating help - more to come
// 
// 13    6/15/98 6:37p Hforman
// interrum checkin
// 
// 12    6/06/98 2:17p Fredf
// Help window improvments.
// 
// 11    6/04/98 5:56p Fredf
// New starting topic and normal toolbar buttons (yech).
// 
// 10    5/25/98 4:40p Fredf
// Fixed TCard notifcation problem.
// 
// 9     4/30/98 2:11p Fredf
// Restores zero sized window to default size.
// 
// 8     4/29/98 6:50p Fredf
// Handles cases of help failure more gracefully.
// 
// 7     4/28/98 11:17a Fredf
// Reduction of flashing on expand/contract.
// 
// 6     4/27/98 5:30p Fredf
// Improved bar positioning.
// 
// 5     4/24/98 6:35p Fredf
// Added "forward" button.
// 
// 4     4/23/98 7:20p Fredf
// Persistant Show/Hide Help Window.
// 
// 3     4/14/98 7:59p Fredf
// First attempt at Help Window.
// 
// 2     4/14/98 4:05p Fredf
// Ported from test project.
// 
// 1     4/14/98 4:04p Fredf
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwcfg.h"
#include "inifile.h"
#include "util.h"
#include "pmhelp.h"

#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette *pOurPal;

// Type of help window to use for PrintMaster 7.0
#define PM_HELP_TYPE	"Pm70HelpWindow"

// Private message to reframe the help window after expand/contract.
// This basically a kludge to compensate for a bug in Html Help
// which improperly positions the help window in child mode.
#define WM_REFRAME_HELP_WINDOW	(WM_USER+2048)

/////////////////////////////////////////////////////////////////////////////
// CPmHelpWindow

// Constructor
CPmHelpWindow::CPmHelpWindow()
{
	m_fSizesValid = FALSE;
	m_pResizeDC = NULL;
	m_fExpanded = FALSE;
	m_hwndNotify = NULL;
	m_fCaptionEnabled = FALSE;
}

// Destructor
CPmHelpWindow::~CPmHelpWindow()
{
}

// Check if the help window is valid.
BOOL CPmHelpWindow::IsValid(void)
{
	return (!m_csHelpFile.IsEmpty()) && (m_hWnd != NULL) && (m_HtmlHelpControl.IsValid());
}

// Display a topic.
void CPmHelpWindow::DisplayTopic(LPCSTR pszTopic, BOOL bUseCompiledHtmlFile /* = TRUE*/)
{
	if (IsValid())
	{
		CString csHelpFile;
		LPSTR pSubTopic = NULL;
		TRY
		{
			if(bUseCompiledHtmlFile)
			{
				csHelpFile = m_csHelpFile;
				pSubTopic = (char*)pszTopic;
			}
			else
				csHelpFile = pszTopic;

			csHelpFile += ">" PM_HELP_TYPE;

			m_HtmlHelpControl.DisplayTopic(GetSafeHwnd(), csHelpFile, pSubTopic);
		}
		END_TRY
	}
}

struct HELPTOPICS
{
	DWORD	TopicID;
	LPCTSTR HtmlHelpFile;
};

HELPTOPICS HelpTopics[] =
{
	{ ID_HUB_WINDOW,						"Creating_New_Documents.htm" },
	{ IDD_GALLERY_ART,					"Using_the_art_browser.htm" },
	{ IDD_GALLERY_PROJECT,				"Project_browser.htm" },
	{ IDD_NEW_DIALOG,						"Creating_a_Brand_New_Project.htm" },
	{ IDD_GALLERY_SENTIMENT,			"Elements_Sentiments.htm" },
	{ IDD_PRINT,							"Previewing_and_Printing.htm" },
	{ IDD_ART_STORE,						"Art and More Store.htm" },
	{ IDD_YYWB,								"Year.htm" },
	{ IDD_ADDRESS_BOOK,					"Using_your_address_book.htm" },
	{ IDD_PICTURE_ATTRIBUTES,			"Picture_Attributes.htm" },
	{ IDD_TEXT_WRAP,						"Text_Wrap.htm" },
	{ IDD_SAVE_PROJECT_OPTS,			"Saving_your_Project.htm" },
	{ IDD_ADDR_INFO,						"Printmaster_gold_on_the_internet.htm" },
	{ IDD_BILLLING_INFO,					"Printmaster_gold_on_the_internet.htm" },

	{ IDR_POSTERTYPE,						"The_Design_Workspace.htm" },
   { IDR_CARDTYPE,						"The_Design_Workspace.htm" },
   { IDR_BANNERTYPE,						"The_Design_Workspace.htm" },
   { IDR_POSTERTYPE,						"The_Design_Workspace.htm" },
   { IDR_CALENDARTYPE,					"Designing Calendars.htm" },
   { IDR_CERTIFICATETYPE,				"The_Design_Workspace.htm" },
   { IDR_LABELTYPE,						"The_Design_Workspace.htm" },
   { IDR_BIZCARDTYPE,					"The_Design_Workspace.htm" },
   { IDR_STATIONERYTYPE,				"The_Design_Workspace.htm" },
   { IDR_ENVELOPETYPE,					"The_Design_Workspace.htm" },
   { IDR_FAXCOVERTYPE,					"The_Design_Workspace.htm" },
   { IDR_NOTECARDTYPE,					"The_Design_Workspace.htm" },
   { IDR_NEWSLETTERTYPE,				"The_Design_Workspace.htm" },
   { IDR_BROCHURETYPE,					"The_Design_Workspace.htm" },
   { IDR_HALFCARDTYPE,					"The_Design_Workspace.htm" },
   { IDR_WEBPUBTYPE,						"Designing Web Pages.htm" },
   { IDR_POSTCARDTYPE,					"The_Design_Workspace.htm" },
	{ IDR_STICKERTYPE,					"The_Design_Workspace.htm" },
	{ IDR_TSHIRTTYPE,						"The_Design_Workspace.htm" },

	{ IDHELP_USING_SCANNERS,			"Scanning_pictures_and_graphics.htm" },
	{ IDHELP_EVENT_REMINDER,			"Event_Reminders.htm" },
	{ IDHELP_BORDER_PLUS,				"Elements_BorderPlus.htm" },
	{ IDHELP_CARTOON_O_MATIC,			"Elements_CartoonOMatic.htm" },
	{ IDHELP_DRAW_PLUS,					"Elements_DrawPlus.htm" },
	{ IDHELP_PHOTO_ENHANCER,			"Using_PhotoEnhancer.htm" },
	{ IDHELP_PRINT_LARGE,				"Printing_Large_Projects_Output_S.htm" },
	{ IDHELP_WEB_PUBLISHING,			"Web Publishing.htm" },
	{ IDHELP_EDIT_TEXT,					"Elements_Text.htm" },
	{ IDHELP_CONNECTING_TEXT,			"Connecting_text_boxes.htm" },
	{ IDHELP_PICTURES,					"Elements_Pictures.htm" },
	{ IDHELP_LAYERS,						"Elements_Layers.htm" },
	{ IDHELP_ONLINE_ART_GALLERY,		"Online Art Gallery.htm" },
	{ IDHELP_USING_ART_GALLERY,		"Using_the_Art_Browser.htm" },
	{ IDHELP_ORIGINAL_ART_GALLERY,	"Original Art Gallery.htm" },
	{ IDHELP_WORKING_W_PICTURES,		"Working_with_Pictures.htm" },
	{ IDHELP_THE_HUB,						"The_Hub_Screen.htm" },
	{ IDHELP_INTERNET,					"PrintMaster_Gold_on_the_Internet.htm" },
	{ IDHELP_DESIGN_WIZARD,				"Using_the_Design_Wizard.htm" },
	{ IDHELP_WORKSPACE,					"The_Design_Workspace.htm" },
	{ IDHELP_MODIFY_TOOLBAR,			"Using_the_Modify_Toolbar.htm" },
	{ IDHELP_DESIGNING_CALENDARS,		"Designing Calendars.htm" },
	{ IDHELP_CREATING_NEWSLETTERS,	"Creating_Newsletters.htm" },
	{ IDHELP_DESIGNING_WEBPAGES,		"Designing Web Pages.htm" },
	{ IDHELP_WEB_LINKS,					"Web Page Links.htm" },
	{ IDHELP_USING_STATUS_BAR,			"Using_the_status_bar.htm" },
	{ IDHELP_ZOOM_TO_AREA,				"View_Popup_Menu.htm" },
	{ IDHELP_SHORTCUT_KEYS,				"Shortcut_Key_Combinations.htm" },
	{ IDHELP_SETTING_PREFERENCES,				"Setting_Preferences.htm" },
	{ IDHELP_ADDRESS_BOOK,				"Using_Your_Address_Book.htm" },
	{ IDHELP_MAIL_MERGE	,				"Mail Merge Fields.htm" },
	{ IDHELP_TSHIRT,						"Creating a T-Shirt Iron-on.htm" },
	{ IDHELP_PRINT_BANNER,				"Continuous Feed Printing.htm" },
	{ IDHELP_CRAFT_CARDS,				"Card Crafts.htm" },
};

LPCTSTR CPmHelpWindow::GetHelpTopic(DWORD dwContext)
{
	DWORD helpID = dwContext - 0x20000;
	TRACE1("** HelpID = %d\n", helpID);

	for (int i = 0; i < sizeof(HelpTopics) / sizeof(HELPTOPICS); i++)
	{
		if (helpID == HelpTopics[i].TopicID)
			return HelpTopics[i].HtmlHelpFile;
	}
	
	return NULL;
}

// Display contextual help.
void CPmHelpWindow::HelpContext(DWORD dwContext)
{
	// NOTE!!!!
	// The current HTML Help control is BROKEN for HH_HELP_CONTEXT.
	// It will not show a topic in our docking help window, but only shows
	// them in a default "sibling"-type window. To get around this, I'm
	// using HH_DISPLAY_TOPIC instead. This requires mapping the context ID
	// to an HTML file inside pmw.chm. Each topic in the HTML Help compiled
	// file must be in a separate .htm file, so this is pretty straightforward.

	if (IsValid())
	{
		CString csHelpFile;
		TRY
		{
			csHelpFile = m_csHelpFile;
			csHelpFile += ">" PM_HELP_TYPE;

			LPCTSTR pszTopic = GetHelpTopic(dwContext);
			m_HtmlHelpControl.DisplayTopic(GetSafeHwnd(), csHelpFile, pszTopic);

//			m_HtmlHelpControl.HelpContext(GetSafeHwnd(), csHelpFile, dwContext);
		}
		END_TRY
	}
}

BEGIN_MESSAGE_MAP(CPmHelpWindow, CDialogBar)
	//{{AFX_MSG_MAP(CPmHelpWindow)
	ON_WM_SIZE()
	ON_WM_TCARD()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
   ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEACTIVATE()
	ON_WM_PARENTNOTIFY()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
   ON_MESSAGE(WM_REFRAME_HELP_WINDOW, OnReframeHelpWindow)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPmHelpWindow message handlers

// Pass WM_TCARD on to parent.
void CPmHelpWindow::OnTCard(UINT idAction, DWORD dwActionData) 
{
	TRACE("TCARD! %d %08lx\n", idAction, dwActionData);
	if (m_hwndNotify != NULL)
	{
		::SendMessage(m_hwndNotify, WM_TCARD, (WPARAM)idAction, (LPARAM)dwActionData);
	}
}

// Create the help window.
BOOL CPmHelpWindow::Create(LPCSTR pszHelpFile, CWnd* pParentWnd, UINT nIDTemplate, UINT nStyle, UINT uID, int nNavigationTabWidth) 
{
	// Save a pointer to the window we want to notify.
	m_hwndNotify = pParentWnd->GetSafeHwnd();

	// Create the dialog bar.
	BOOL fResult = CDialogBar::Create(pParentWnd, nIDTemplate, nStyle, uID);

	// Save our ID.
	m_uID = uID;

	// Save the width for the navigation tab.
	m_nNavigationTabWidth = nNavigationTabWidth;

	if (fResult)
	{
		// Hide the window by default. It will be shown later if needed.
		ShowWindow(SW_HIDE);

		// Clip children.
		ModifyStyle(0, WS_CLIPCHILDREN);

		// Set titlebar text (floating only)
		CString strTitle = "@@N Help";
		GetConfiguration()->ReplaceText(strTitle);
		SetWindowText(strTitle);

		// Replace the dummy control in the template with the Html Help control.
		CWnd* pOldControl = GetDlgItem(IDC_HELP_CONTROL);
		if (pOldControl != NULL)
		{
			fResult = TRUE;

			// Get the size and position of the dummy control.
			CRect crBounds;
			pOldControl->GetWindowRect(crBounds);
			ScreenToClient(crBounds);

			// Compute the sizes of the border area around the
			// help control within the dialog bar. As the dialog
			// bar is resizes, these borders sizes are retained.
			m_crBorder.left = crBounds.left;
			m_crBorder.top = crBounds.top;
			m_crBorder.right = m_sizeDefault.cx-crBounds.right;
			m_crBorder.bottom = m_sizeDefault.cy-crBounds.bottom;

			CRect crCloseButton;
			crCloseButton.SetRect(0, 0, 12, 12);	// size of bitmap
			m_btnClose.Create(crCloseButton, this, ID_VIEW_HELPWINDOW);
			m_btnClose.SetBitmaps("HELP_CLOSE_U", "HELP_CLOSE_D", pOurPal);
			m_btnClose.SetTransparentColor(RGB(255,0,255));

			// Restore the previous size of the help window.
			CRect crHelpWindow;
			CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);

			// Restore the previous Expand/Contract state.
			m_fExpanded = IniFile.GetInteger("HelpWindow", "Expanded", FALSE);

			if (IniFile.GetWindowPosition("HelpWindow", NULL, NULL, crHelpWindow))
			{
				if (!crHelpWindow.IsRectEmpty())
				{
					m_sizeDefault.cx = crHelpWindow.Width();
					m_sizeDefault.cy = crHelpWindow.Height();
				}
			}

			m_czFloating = m_czDocked = m_sizeDefault;

			TRY
			{
				// Remember the name of the help.
				m_csHelpFile = pszHelpFile;
			}
			END_TRY

			if ((m_csHelpFile.IsEmpty()) || (!Util::FileExists(m_csHelpFile)))
			{
				// Can't use the help file.
				m_csHelpFile.Empty();
			}
			else
			{
				// Load the Html Help control.
				if (m_HtmlHelpControl.Load())
				{
					// Initialize all structure members to zero.
					HH_WINTYPE HHWinType;
					ZeroMemory(&HHWinType, sizeof(HHWinType));

					// Structure size in bytes.
					HHWinType.cbStruct = sizeof(HHWinType);

					// Properties of the HTML window.
					HHWinType.fsWinProperties =
						HHWIN_PROP_TRI_PANE |		// use a tri-pane window
						HHWIN_PROP_TAB_SEARCH |		// show a "Search" tab in navigation pane
						HHWIN_PROP_AUTO_SYNC |		// automatically sync contents and index
						HHWIN_PROP_TRACKING |		// send tracking notification messages
						HHWIN_PROP_NOTB_TEXT |		// no text on toolbar buttons
						HHWIN_PROP_NOTITLEBAR |		
						HHWIN_PROP_NODEF_STYLES |	// no default window styles (only HH_WINTYPE.dwStyles)
						HHWIN_PROP_NODEF_EXSTYLES;	// no default extended window styles (only HH_WINTYPE.dwExStyles)

					// Window styles.
					HHWinType.dwStyles =
						WS_VISIBLE |
						WS_CLIPSIBLINGS |
						WS_CHILD;

					// Extended window styles.
					HHWinType.dwExStyles =
						WS_EX_WINDOWEDGE;

					// Buttons on toolbar pane.
					HHWinType.fsToolBarFlags =
						HHWIN_BUTTON_EXPAND |		// expand/contract button
						HHWIN_BUTTON_BACK |			// back button
						HHWIN_BUTTON_HOME |			// home button
//						HHWIN_BUTTON_ZOOM |			// font button -- too flakey at the moment!
						HHWIN_BUTTON_PRINT;			// print button

					// Full Paths or CHM locations of various files.
					CString csInitialTopic;
					CString csHomePage;
					CString csTableOfContents;
					CString csIndex;

					TRY
					{
						csInitialTopic = "contents.htm";

						csHomePage = m_csHelpFile;
						csHomePage += "::/contents.htm";

						csTableOfContents = m_csHelpFile;
						csTableOfContents += "::/toc.hhc";

						csIndex = m_csHelpFile;
						csIndex += "::/index.hhk";
					}
					END_TRY

					HHWinType.pszFile = csInitialTopic;
					HHWinType.pszHome = csHomePage;
					HHWinType.pszToc = csTableOfContents;
					HHWinType.pszIndex = csIndex;
					
					// Expansion width of navigation pane (left pane).
					HHWinType.iNavWidth = m_nNavigationTabWidth;

					// Initial display state:
					HHWinType.nShowState = SW_RESTORE;

					// TOC should be activated.
					HHWinType.curNavType = HHWIN_NAVTYPE_TOC;

					// Position of navigation tabs.
					HHWinType.tabpos = HHWIN_NAVTAB_TOP;

					// ID to use in WPARAM in WM_NOTIFY.
					HHWinType.idNotify = m_uID;

					// Title of Help Window
					HHWinType.pszCaption= "PrintMaster 7.0 Help Window";

					// Initial state is not expanded.
					HHWinType.fNotExpanded = !m_fExpanded;

					// Indicate which fields in structure are valid.
					HHWinType.fsValidMembers =
						HHWIN_PARAM_STYLES |			// valid dwStyles
						HHWIN_PARAM_PROPERTIES |	// valid fsWinProperties
						HHWIN_PARAM_RECT |			// valid rcWindowPos
						HHWIN_PARAM_TB_FLAGS |		// valid fsToolBarFlags
						HHWIN_PARAM_NAV_WIDTH |		// valid iNavWidth
						HHWIN_PARAM_SHOWSTATE |		// valid nShowState
						HHWIN_PARAM_TABPOS |			// valid tabpos
						HHWIN_PARAM_EXPANSION |		// valid fNotExpanded
						HHWIN_PARAM_CUR_TAB;			// valid curNavType	

					// Specify the name of the new window type.
					HHWinType.pszType = PM_HELP_TYPE;

					// Allow any derived classes to modify the window type information.
					PreSetWinType(&HHWinType);

					// This call creates the new window type from the values in
					// the HH_WINTYPE structure.
					m_HtmlHelpControl.SetWinType(&HHWinType);

					// Display the default topic.
					HWND hWnd = NULL;

					CString csHelpFile;
					TRY
					{
						csHelpFile = m_csHelpFile;
						csHelpFile += ">" PM_HELP_TYPE;

						hWnd = m_HtmlHelpControl.DisplayContents(GetSafeHwnd(), csHelpFile);
					}
					END_TRY

					// If we have successfully created a new help window,
					// then get rid of the old window.
					if (hWnd != NULL)
					{
						// Get rid of the old control.
						if (pOldControl->DestroyWindow())
						{
#if 0
							// remove the "Flat" style in the toolbar
							CWnd HelpWindow;
							HelpWindow.Attach(hWnd);

							CToolBarCtrl* pToolBar = (CToolBarCtrl*)(HelpWindow.GetWindow(GW_CHILD));
							pToolBar->ModifyStyle(TBSTYLE_FLAT, 0);

							HelpWindow.Detach();
#endif
							// Signal that all the cached size information is accurate.
							m_fSizesValid = TRUE;

							fResult = TRUE;
						}
					}
				}
			}

			ASSERT(fResult);
			if (!fResult)
			{
				// We failed, clean up.
			}
		}
	}

	return fResult;
}

// When the dialog bar is sized, size the help control to fit.
void CPmHelpWindow::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	ReframeHelpWindow();
}

// Handle notifications from the help control.
BOOL CPmHelpWindow::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// Unpack the notification.
	NMHDR* pNMHDR = (NMHDR*)lParam;

	// Check if the notification is from the help window.
	if (pNMHDR->idFrom == m_uID)
	{
		// Unpack the help notification.
		TRACE("NOTIFY! %d\n", pNMHDR->code);
		switch (pNMHDR->code)
		{
			case HHN_NAVCOMPLETE:
			{
				HHN_NOTIFY* pHHNNOTIFY = (HHN_NOTIFY*)pNMHDR;
				TRACE("    OnNavComplete: '%s'\n", pHHNNOTIFY->pszUrl);
				break;
			}
			case HHN_TRACK:
			{
				HHNTRACK* pHHNTRACK = (HHNTRACK*)pNMHDR;
				LPCSTR pszAction = NULL;
				switch (pHHNTRACK->idAction)
				{
					case HHACT_TAB_CONTENTS:
					{
						pszAction = "HHACT_TAB_CONTENTS";
						break;
					}
					case HHACT_TAB_INDEX:
					{
						pszAction = "HHACT_TAB_INDEX";
						break;
					}
					case HHACT_TAB_SEARCH:
					{
						pszAction = "HHACT_TAB_SEARCH";
						break;
					}
					case HHACT_TAB_HISTORY:
					{
						pszAction = "HHACT_TAB_HISTORY";
						break;
					}
					case HHACT_TAB_FAVORITES:
					{
						pszAction = "HHACT_TAB_FAVORITES";
						break;
					}
					case HHACT_EXPAND:
					{
						pszAction = "HHACT_EXPAND";
						m_fExpanded = TRUE;
						if (!IsFloating())
						{
							SetRedraw(FALSE);
						}
						m_czDocked.cx += m_nNavigationTabWidth;
						m_czFloating.cx += m_nNavigationTabWidth;
						PostMessage(WM_REFRAME_HELP_WINDOW, 0, 0);
						break;
					}
					case HHACT_CONTRACT:
					{
						pszAction = "HHACT_CONTRACT";
						m_fExpanded = FALSE;
						if (!IsFloating())
						{
							SetRedraw(FALSE);
						}
						m_czDocked.cx -= m_nNavigationTabWidth;
						m_czFloating.cx -= m_nNavigationTabWidth;

						if (m_czDocked.cx < 20)
						{
							m_czDocked.cx = 20;
						}
						PostMessage(WM_REFRAME_HELP_WINDOW, 0, 0);
						break;
					}
					case HHACT_BACK:
					{
						pszAction = "HHACT_BACK";
						break;
					}
					case HHACT_FORWARD:
					{
						pszAction = "HHACT_FORWARD";
						break;
					}
					case HHACT_STOP:
					{
						pszAction = "HHACT_STOP";
						break;
					}
					case HHACT_REFRESH:
					{
						pszAction = "HHACT_REFRESH";
						break;
					}
					case HHACT_HOME:
					{
						pszAction = "HHACT_HOME";
						break;
					}
					case HHACT_SYNC:
					{
						pszAction = "HHACT_SYNC";
						break;
					}
					case HHACT_OPTIONS:
					{
						pszAction = "HHACT_OPTIONS";
						break;
					}
					case HHACT_PRINT:
					{
						pszAction = "HHACT_PRINT";
						break;
					}
					case HHACT_HIGHLIGHT:
					{
						pszAction = "HHACT_HIGHLIGHT";
						break;
					}
					case HHACT_CUSTOMIZE:
					{
						pszAction = "HHACT_CUSTOMIZE";
						break;
					}
					case HHACT_JUMP1:
					{
						pszAction = "HHACT_JUMP1";
						break;
					}
					case HHACT_JUMP2:
					{
						pszAction = "HHACT_JUMP2";
						break;
					}
					case HHACT_ZOOM:
					{
						pszAction = "HHACT_ZOOM";
						break;
					}
					case HHACT_TOC_NEXT:
					{
						pszAction = "HHACT_TOC_NEXT";
						break;
					}
					case HHACT_TOC_PREV:
					{
						pszAction = "HHACT_TOC_PREV";
						break;
					}
					case HHACT_NOTES:
					{
						pszAction = "HHACT_NOTES";
						break;
					}
					default:
					{
						pszAction = "???";
						break;
					}
				}
				TRACE("   TRACK! idAction %d (%s) pszCurUrl '%s'\n", pHHNTRACK->idAction, pszAction, pHHNTRACK->pszCurUrl);
				break;
			}
		}
		return TRUE;
	}

	return CDialogBar::OnNotify(wParam, lParam, pResult);
}

// This handler is for the private message, WM_REFRAME_HELP_WINDOW.
// It simply calls the ReframeHelpWindow() method.
LONG CPmHelpWindow::OnReframeHelpWindow(WPARAM wParam, LPARAM lParam)
{
	ReframeHelpWindow();
	if (!IsFloating())
	{
		SetRedraw(TRUE);
		Invalidate();
		m_pDockSite->RecalcLayout();
	}
	return 0L;
}

// We allow the user to resize the help window while it is docked.
// This handler for the WM_SETCURSOR message allows us to set the
// cursor when the mouse is over the resize area.
BOOL CPmHelpWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// Only works when dialog bar is docked (not floating).
	if (!IsFloating())
	{
		// Get the position of the cursor. If it's over the left
		// border of the dialog bar, then change cursors so the
		// user knows they can do a resize.
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		if (point.x <= m_crBorder.left)
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
			return TRUE;
		}
	}

	// Use default cursor.
	return CDialogBar::OnSetCursor(pWnd, nHitTest, message);
}

// If the user clicks on the left border of the help window,
// then we allow them to resize the window by dragging.

void CPmHelpWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!IsFloating() && (point.x <= m_crBorder.left))
	{
		// Start a resize operation.
		ClientToScreen(&point);
		StartResize(point);
		ResizeLoop();
	}
	else
	{
		// Just pass on the click message.
		CDialogBar::OnLButtonDown(nFlags, point);
	}
}

void CPmHelpWindow::OnDestroy() 
{
	// Remember our position and expanded state.
   CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
	IniFile.WriteWindowPosition("HelpWindow", this);
	IniFile.WriteInteger("HelpWindow", "Expanded", m_fExpanded);

	CDialogBar::OnDestroy();
}

////////////////////////////////////////////////////////////////////
// Overloaded functions

// Derived classes should override this virtual function if they want
// to change the Window Type information before SetWinType() is called.
void CPmHelpWindow::PreSetWinType(HH_WINTYPE* pWinType)
{
	// Default implementation is to do nothing.
}

// Compute the size of help window in the various states (docked, horizontal, ...)
CSize CPmHelpWindow::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	// Return default if it is being docked.
	if ((dwMode & LM_VERTDOCK) || (dwMode & LM_HORZDOCK))
	{
		if (dwMode & LM_STRETCH) // if not docked stretch to fit
		{
			return CSize((dwMode & LM_HORZ) ? 32767 : m_czDocked.cx,
							 (dwMode & LM_HORZ) ? m_czDocked.cy : 32767);
		}
		else
		{
			return m_czDocked;
		}
	}

	// Return default if it is being floated.
	if (dwMode & LM_MRUWIDTH)
	{
		return m_czFloating;
	}

	// In all other cases, accept the dynamic length
	if (dwMode & LM_LENGTHY)
	{
		return CSize(m_czFloating.cx, m_czFloating.cy = nLength);
	}
	else
	{
		return CSize(m_czFloating.cx = nLength, m_czFloating.cy);
	}
}

// Reframe the help control window to the current dimensions
// of the dialog bar.
void CPmHelpWindow::ReframeHelpWindow(void)
{
	// If we don't have accurate information, just ignore the request.
	if ((GetSafeHwnd() != NULL) && m_fSizesValid)
	{
		CRect crClient;
		GetClientRect(crClient);

		// Position the close button.
		if (m_btnClose.GetSafeHwnd() != NULL)
		{
			if (IsFloating())
			{
				// Floating
				m_btnClose.ShowWindow(SW_HIDE);
			}
			else
			{
				// Docked
				CRect crClose;
				m_btnClose.GetClientRect(crClose);
				crClose.OffsetRect(crClient.Width()-crClose.Width()-6, 6);
				m_btnClose.MoveWindow(crClose);
				m_btnClose.ShowWindow(SW_SHOW);
			}
		}

		// Compute the size and placement of the help control window.
		CRect crNew(crClient);

		crNew.left += m_crBorder.left;
		crNew.top += m_crBorder.top;
		crNew.right -= m_crBorder.right;
		crNew.bottom -= m_crBorder.bottom;

		if (!IsFloating())
		{
			crNew.bottom -= 4;
		}
		else
		{
			if (m_btnClose.GetSafeHwnd() != NULL)
			{
				crNew.top = 4;
			}
		}

		if (crNew.IsRectEmpty())
		{
			crNew.SetRectEmpty();
		}

		// Position the help control window.
		HWND hWnd = m_HtmlHelpControl.GetWinHandle(m_csHelpFile, PM_HELP_TYPE);
		if (hWnd != NULL)
		{
			::MoveWindow(hWnd, crNew.left, crNew.top, crNew.Width(), crNew.Height(), TRUE);
		}
	}

	if (IsFloating())
	{
		// Resize floating window
		CWnd* pParent = GetParent();
		if (pParent)
		{
			CWnd* pGrandParent = pParent->GetParent();
			if (pGrandParent && pGrandParent->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
				((CMiniFrameWnd*)pGrandParent)->RecalcLayout();
		}
	}
}

// These two methods should be called by MainFrame::RecalcLayout() to
// give the help window a chance to adjust itself when it is being docked
// and undocked.
void CPmHelpWindow::PreRecalcLayout(void)
{
	ReframeHelpWindow();
}

void CPmHelpWindow::PostRecalcLayout(void)
{
}

////////////////////////////////////////////////////////////////////
// Resize helper functions
//
// These functions are used to implement the resize operation for the
// help window dialog bar. Much of this code is ripped off from the
// MFC file DOCKCONT.CPP

// Start a resize sequence.
void CPmHelpWindow::StartResize(const CPoint& cpDragStart)
{
	// Process any pending WM_PAINT messages before we start resizing.
	MSG Msg;
	while (::PeekMessage(&Msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
	{
		if (!GetMessage(&Msg, NULL, WM_PAINT, WM_PAINT))
		{
			// Got a WM_QUIT.
			return;
		}

		// Do the paint.
		DispatchMessage(&Msg);
	}

	// Get the DC to use. We use the desktop because we need to draw
	// outside of our window.
	CWnd* pWnd = CWnd::GetDesktopWindow();
	if (pWnd->LockWindowUpdate())
	{
		m_pResizeDC = pWnd->GetDCEx(NULL, DCX_WINDOW|DCX_CACHE|DCX_LOCKWINDOWUPDATE);
	}
	else
	{
		m_pResizeDC = pWnd->GetDCEx(NULL, DCX_WINDOW|DCX_CACHE);
	}

	// Remember the starting point of the drag.
	m_cpDragStart = cpDragStart;

	// Remember our starting size.
	GetWindowRect(m_crResizeStart);

	// Initialize the current size.
	m_crResize = m_crResizeStart;
	m_czResize = CSize(4,4);

	// Initialize the last size.
	m_crResizeLast.SetRect(0,0,0,0);
	m_czResizeLast = CSize(0,0);
}

// The resizing loop.
BOOL CPmHelpWindow::ResizeLoop(void)
{
	// Grab the capture.
	SetCapture();

	// Get messages until capture lost or cancelled/accepted.
	while (CWnd::GetCapture() == this)
	{
		MSG Msg;
		if (!::GetMessage(&Msg, NULL, 0, 0))
		{
			// Quit message.
			AfxPostQuitMessage(Msg.wParam);
			break;
		}

		// Branch on the type of message.
		switch (Msg.message)
		{
			case WM_LBUTTONUP:
			{
				// The user released the button. The resize is finished
				// and the size of the help window will be changed.
				EndResize();
				return TRUE;
			}
			case WM_MOUSEMOVE:
			{
				// The user moved the mouse, resize accordingly.
				ASSERT(m_pResizeDC != NULL);
				m_crResize.left = m_crResizeStart.left+(Msg.pt.x-m_cpDragStart.x);
				m_pResizeDC->DrawDragRect(m_crResize, m_czResize, m_crResizeLast, m_czResizeLast);
				m_crResizeLast = m_crResize;
				m_czResizeLast = m_czResize;
				break;
			}
			case WM_KEYDOWN:
			{
				// If the user hits escape, cancel the resize without
				// changing the size of the help window.
				if (Msg.wParam == VK_ESCAPE)
				{
					CancelResize();
					return FALSE;
				}
				break;
			}
			case WM_RBUTTONDOWN:
			{
				// If the user clicks the right mouse button, cancel the resize without
				// changing the size of the help window.
				CancelResize();
				return FALSE;
			}
			default:
			{
				// This is not a message we care about, just dispatch it.
				DispatchMessage(&Msg);
				break;
			}
		}
	}

	return TRUE;
}

// Stop resizing.
void CPmHelpWindow::CancelResize(void)
{
	// Remove the drag rectangle.
	if (m_pResizeDC != NULL)
	{
		m_pResizeDC->DrawDragRect(CRect(0,0,0,0), CSize(0,0), m_crResizeLast, m_czResizeLast);
	}

	// Release the capture.
	ReleaseCapture();

	// Release the DC used for drawing the drag rectangle.
	CWnd* pWnd = CWnd::GetDesktopWindow();
	pWnd->UnlockWindowUpdate();
	if (m_pResizeDC != NULL)
	{
		pWnd->ReleaseDC(m_pResizeDC);
		m_pResizeDC = NULL;
	}
}

// Finish a resize operation.
void CPmHelpWindow::EndResize(void)
{
	// Clean up.
	CancelResize();

	// Resize the window.
	m_czDocked.cx = m_crResize.Width();
	m_czDocked.cy = m_crResize.Height();

	m_pDockSite->RecalcLayout();
}

// Override erase background to draw the caption bar.
BOOL CPmHelpWindow::OnEraseBkgnd(CDC* pDC) 
{
	CDialogBar::OnEraseBkgnd(pDC);

	CRect crCaption;
	if (GetCaptionRect(crCaption))
	{
		pDC->FillSolidRect(crCaption, GetSysColor(IsEnabled() ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));
	}

	return TRUE;
}

// Check if the window is enabled by walking the parent
// chain until we find a disabled window or the active window.
BOOL CPmHelpWindow::IsEnabled(void)
{
	BOOL fEnabled = TRUE;
	CWnd* pActive = GetActiveWindow();
	CWnd* pWnd = this;
	while (pWnd != NULL)
	{
		if (pWnd == pActive)
		{
			break;
		}
		if (!pWnd->IsWindowEnabled())
		{
			fEnabled = FALSE;
			break;
		}
		pWnd = pWnd->GetParent();
	}

	return fEnabled;
}

// Return the rectangle for the caption bar. If there is no
// caption bar (the window is floating), then FALSE is returned
// and the rectangle is set to empty.
BOOL CPmHelpWindow::GetCaptionRect(CRect& crCaption)
{
	crCaption.SetRectEmpty();

	if (!IsFloating())
	{
		CRect crClient;
		GetClientRect(crClient);

		crCaption.SetRect(4, 4, crClient.Width()-4, 4+16);
	}

	return !crCaption.IsRectEmpty();
}

// We go to great lengths to make sure the caption bar is the
// correct color!
LRESULT CPmHelpWindow::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
	if (!IsFloating())
	{
		BOOL fEnabled = IsEnabled();
		if (m_fCaptionEnabled != fEnabled)
		{
			m_fCaptionEnabled = fEnabled;
			CRect crCaption;
			if (GetCaptionRect(crCaption))
			{
				InvalidateRect(crCaption);
			}
		}
	}
	return CDialogBar::OnIdleUpdateCmdUI(wParam, lParam);
}

int CPmHelpWindow::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	if (IsFloating())
	{
		CWnd* pGrandParent = GetParent()->GetParent();
		if (pGrandParent && pGrandParent->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
		{
			pGrandParent->BringWindowToTop();
			pGrandParent->SetActiveWindow();
			pGrandParent->EnableWindow(TRUE);
		}
	}
	
	return CDialogBar::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

// This is here for bringing the undocked help window to the top when
// the titlebar is clicked on. This is not the default Windows behavior.
void CPmHelpWindow::OnCaptureChanged(CWnd *pWnd) 
{
	if (IsFloating())
	{
		CWnd* pGrandParent = GetParent()->GetParent();
		if (pGrandParent && pGrandParent->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
		{
			pGrandParent->BringWindowToTop();
			pGrandParent->SetActiveWindow();
			pGrandParent->EnableWindow(TRUE);
		}
	}
	
	CDialogBar::OnCaptureChanged(pWnd);
}

void CPmHelpWindow::OnParentNotify(UINT message, LPARAM lParam) 
{
	CDialogBar::OnParentNotify(message, lParam);

	if (message == WM_LBUTTONDOWN)
	{
		// I was attempting to fix the redraw bug which happens when you
		// bring the window forward by click in the html control, but I give up.
		// This will just have to be fixed by Microsoft!
#if 0
		CWnd* pGrandParent = GetParent()->GetParent();
		if (pGrandParent)// && pGrandParent->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
		{
			SendMessageToDescendants(WM_MOUSEACTIVATE, (WPARAM)pGrandParent->GetSafeHwnd(), MAKELONG(1, WM_LBUTTONDOWN));
			GetParent()->SendMessage(WM_MOUSEACTIVATE, (WPARAM)pGrandParent->GetSafeHwnd(), MAKELONG(1, WM_LBUTTONDOWN));
			pGrandParent->SendMessage(WM_MOUSEACTIVATE, (WPARAM)pGrandParent->GetSafeHwnd(), MAKELONG(1, WM_LBUTTONDOWN));
		}
#endif
	}
}

