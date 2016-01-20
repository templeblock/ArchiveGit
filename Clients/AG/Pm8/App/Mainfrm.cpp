//
// $Workfile: MAINFRM.CPP $
// $Revision: 4 $
// $Date: 3/08/99 3:36p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
//
//
// Revision History:
//
// $Log: /PM8/App/MAINFRM.CPP $
// 
// 4     3/08/99 3:36p Gbeddow
// support for asking at start of an online collection build whether to
// compress the online data files
// 
// 3     3/08/99 1:15p Mtaber
// Changed upgrade for Classic to Publishing Suite to:
// Classic to Silver
// 
// 2     3/04/99 1:54p Gbeddow
// alter the main window property string so that PrintMaster 8.0 can run
// as a separate instance from a previous version of the product
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 155   2/26/99 10:08a Johno
// Moved AppendNamedApplicationRegistryPath() from PMW.CPP to INIFILE.CPP
// 
// 154   2/25/99 5:23p Johno
// Moved AppendNamedApplicationRegistryPath() from UTIL.CPP to PMW.CPP,
// as some shared versions of UTIL.CPP could not find needed H file
// 
// 153   2/25/99 3:35p Johno
// Support for PhotoOrganizer toolbar button
// 
// 152   2/23/99 3:40p Johno
// 3DPlanet UI enable / disable
// 
// 151   2/23/99 2:13p Johno
// Support for 3DPlanet
// 
// 150   2/09/99 1:48p Gbeddow
// Support in collection building for creating online collection GIF and
// JPEG thumbnail files, and related changes, per spec from Alexei
// 
// 149   2/05/99 11:29a Mwilson
// modified special collection builder names
// 
// 148   2/01/99 4:15p Blee
// Removed obsolete code from OnInitPopupMenu()
// 
// 147   2/01/99 3:35p Blee
// Added code to OnInitMenuPopup that will gray out a popup item if all of
// its children are grayed out.
// 
// 146   2/01/99 1:25p Gbeddow
// change the Web Art... command from a debug menu item in the Tools menu
// to an ordinary command in the Add menu; fix bug that created a new
// document whenever placing a graphic from the Web Art Gallery dialog.
// 
// 145   1/28/99 4:48p Mwilson
// added handlers to build new custom collections
// 
// 144   1/25/99 1:20p Gbeddow
// support for the latest spec'd list of graphic types and initial support
// for a new web art browser dialog
// 
// 143   1/20/99 10:27a Mwilson
// moved toolbars into seperate DLL
// 
// 142   12/23/98 3:48p Johno
// Preliminary 3D Planet button
// 
// 141   12/18/98 1:31p Gbeddow
// support for new hidden File menu command Preprocess Picture Gallery...
// 
// 140   12/04/98 5:31p Gbeddow
// directory reorganization in preparation for integrating Print Shop
// 
// 139   11/30/98 3:43p Mwilson
// added matching sets dlg for CParty
// 
// 138   11/24/98 7:07p Johno
// OnDesktopArtStore() can now cause art to be added to the current
// project (if any) through the art browser.
// 
// 137   11/19/98 11:41a Johno
// Desktop online art button
// 
// 136   11/18/98 10:11a Johno
// Oops - no more dynamic art store hub buttons
// 
// 135   11/17/98 6:01p Johno
// Changes for dynamic art store hub buttons
// 
// 134   10/30/98 3:05p Johno
// Cleanup
// 
// 133   10/27/98 4:51p Johno
// Consolidated DoArtStore code into DoBrowserOrArtStore
// 
// 132   10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 131   10/26/98 6:47p Jayn
// Download manager.
// 
// 130   10/26/98 1:24p Johno
// Moved  OnFileOpen() from CPmwApp to CMainFrame
// 
// 129   10/24/98 7:34p Jayn
// Download Manager and Download Dialog
// 
// 128   10/23/98 5:47p Johno
// Added DoArtStore() to handle art store browser previewing;
// Removed unused functions / message handlers
// 
// 127   10/21/98 1:53p Johno
// Calls to Util::RemoveExtraMenuSeparators()
// 
// 126   10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 125   10/15/98 3:45p Dennis
// New OCX Connection Manager
// 
// 124   10/13/98 1:20p Mwilson
// added upgrade for craft
// 
// 123   9/29/98 5:33p Johno
// CMainFrame::OnUpdateFileFreeFonts() no longer #ifdef LOCALIZE
// 
// 122   9/24/98 11:24a Mwilson
// disabled hub for print projects utility
// 
// 121   9/23/98 11:51a Dennis
// New RTF Support
// 
// 120   9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 119   9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 118   9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 117   9/21/98 5:16p Hforman
// changed prompt for fixing up project files when doing builds
// 
// 116   9/14/98 9:22a Mwilson
// added tool to print out all projects in folder
// 
// 115   9/04/98 3:54p Johno
// Changes for hidden tool ID_EXTRACT_ART_USED, OnExtractArtUsed
// 
// 114   8/24/98 5:38p Mwilson
// enabled more helpful hints
// 
// 113   8/05/98 11:59a Mwilson
// added new tool utility to check for art in spreadsheet
// 
// 112   7/26/98 3:12p Hforman
// fixed problem with palette flashes
// 
// 111   7/22/98 12:04p Mwilson
// Added SupportsPersonalDelivery Function
// 
// 110   7/22/98 9:13a Mwilson
// changed GetMainWindowProperty to return a CString
// 
// 109   7/21/98 3:24p Dennis
// Removed Strange message post to help system from OnBuildSentiments
// 
// 108   7/21/98 12:32p Rlovejoy
// Kludge to allow hub help topic to appear.
// 
// 107   7/16/98 4:32p Psasse
// changed m_fAppClosingDown flag to AfxGetMainWnd->IsWindowVisible to
// eliminate dialogs from launching upon app closing and moved
// FlushClipboard to WM_Destroy in MainWnd 
// 
// 106   7/14/98 3:20p Rlovejoy
// Make hub go away when address book is brought up.
// 
// 105   7/14/98 12:02p Mwilson
// added override to destroyWindow to destroy HelpWindow first
// 
// 103   7/10/98 11:22a Rlovejoy
// Pass NULL as address book parent.
// 
// 102   7/10/98 11:08a Rlovejoy
// Launch the address book with the active window as the parent.
// 
// 101   7/08/98 1:52p Mwilson
// changes upgrade menu item for print standard
// 
// 100   7/08/98 1:12p Hforman
// added WinHelp function
// 
// 99    7/07/98 6:02p Hforman
// add On Activate() for Help window; stopped sound effects in help window
// 
// 98    7/04/98 11:25a Jayn
// Fixed my change to the CD notification code.
// 
// 97    7/02/98 11:43a Mwilson
// Changed the way the menu text for the reminder is loaded in
// 
// 96    7/02/98 8:59a Mwilson
// changed event reminder menu to Forget me not for AG
// 
// 95    7/01/98 5:29p Mwilson
// moved palette init to init instance
// 
// 94    6/30/98 7:22p Psasse
// added m_fAppClosingDown flag to eliminate dialogs from launching
// 
// 93    6/30/98 3:57p Jayn
// Program exit problems now fixed.
// 
// 92    6/29/98 12:25p Jayn
// Create OnDeviceChange message map function. Got rid of WindowProc.
// 
// 91    6/29/98 11:37a Hforman
// remove m_fInitialHub
// 
// 90    6/29/98 9:37a Mwilson
// added code for auto CD detection
// 
// 89    6/25/98 4:10p Johno
// Avoid ASSERT when hub returns IDCANCEL
// 
// 88    6/24/98 2:58p Cboggio
// Added OnUpdateFileFreeFonts to remove this menu item if required for
// localization
// 
// 87    6/24/98 10:10a Mwilson
// added flag for remote fulfillment to not show hub
// 
// 86    6/22/98 10:31a Johno
// Improved OnHubNew()
// 
// 85    6/19/98 6:19p Johno
// Moved PreventHub() to OnHubNew()
// 
// 84    6/18/98 4:40p Johno
// Hub is now a local - removed much unloved hub code (old sizing stuff,
// ect.)
// 
// 83    6/18/98 12:39p Johno
// Improved hub control
// 
// 82    6/18/98 11:19a Johno
// Changes for modal only hub
// 
// 81    6/17/98 12:22p Johno
// CreateHubWindow () is now an inline that sends a message handled by
// CreateHubWindowModal(). This function may handle a new, more modal, hub
// implementation.
// 
// 80    6/15/98 5:32p Dennis
// Removed DestroyHub from OnDDEInitiate() which caused conflict with
// upgrade code.
// 
// 79    6/12/98 4:35p Mwilson
// removed perosnal delivery menu item for printmaster
// 
// 78    6/12/98 2:55p Jayn
// More startup changes.
// 
// 77    6/12/98 10:38a Jayn
// Changes to the startup code (no more 5-second delay)
// 
// 76    6/10/98 4:15p Dennis
// OnInternet changes for AG
// 
// 75    6/09/98 10:33a Mwilson
// disabled hub for remote fulfillment
// 
// 74    6/06/98 2:17p Fredf
// Help window improvements.
// 
// 73    6/05/98 2:33p Fredf
// Allows embedding all pictures.
// 
// 72    6/05/98 10:36a Johno
// OnShowLegals()
// 
// 71    6/05/98 8:43a Fredf
// Uses WithHelpWizard.GetDocumentFlags()
// 
// 70    6/04/98 10:12p Rlovejoy
// Added HH for internet button.
// 
// 69    6/04/98 7:20p Johno
// Added OnShowLegals()
// 
// 68    6/04/98 2:11p Rlovejoy
// Removed reference to unused file.
// 
// 67    5/31/98 5:11p Fredf
// More Address Book and merge field changes.
// 
// 66    5/31/98 5:04p Rlovejoy
// Check if HelpWizard was initialized successfully.
// 
// 65    5/31/98 12:52p Fredf
// Start of address book changes
// 
// 64    5/30/98 5:02p Psasse
// check-out error - nothing done
// 
// 63    5/27/98 7:13p Rlovejoy
// Changed to kill first poster.
// 
// 62    5/27/98 6:52p Hforman
// yep, more welcome removal
// 
// 61    5/27/98 6:20p Hforman
// OK, I think I got it now (removing Welcome stuff)
// 
// 60    5/27/98 6:17p Hforman
// 
// 59    5/27/98 6:06p Hforman
// remove SEND_WELCOME
// 
// 58    5/26/98 6:38p Psasse
// Helpful hints sound support
// 
// 57    5/26/98 3:03p Rlovejoy
// Use new 'With help wizard'.
// 
// 56    5/25/98 4:43p Fredf
// Support for building online collections.
// 
// 55    5/21/98 9:17p Psasse
// added wave file support for hub initialization
// 
// 54    5/21/98 8:33p Fredf
// Start of online collection building.
// 
// 53    5/21/98 1:57p Dennis
// Added Art Collection Selection dialog for building projects
// 
// 52    5/19/98 11:31a Dennis
// Added Run Silent mode question to GetBuildInfo
// 
// 51    5/15/98 6:18p Fredf
// 
// 50    5/15/98 6:11p Johno
// Delete web publication menu items in OnInitMenuPopup()
// 
// 49    5/15/98 5:39p Fredf
// 
// 48    5/15/98 4:42p Fredf
// 
// 47    5/14/98 10:34a Mwilson
// added utility to check for the existence of art files form pmw files
// 
// 46    5/12/98 2:57p Dennis
// Remove inline usage from hub methods.
// 
// 45    5/06/98 1:22p Dennis
// Added SetHubAutoDestroy() and AutoDestroyHub() methods.
// 
// 44    5/05/98 3:51p Mwilson
// added handler for sentiment extraction
// 
// 43    4/30/98 8:21p Jayn
// Fixed hub coming up at program shutdown.
// 
// 42    4/30/98 4:33p Fredf
// Fixed bug with gadgets not being displayed.
// 
// 41    4/29/98 6:50p Fredf
// Handles cases of help failure more gracefully.
// 
// 40    4/28/98 11:17a Fredf
// Reduction of flashing on expand/contract.
// 
// 39    4/27/98 5:30p Fredf
// Improved bar positioning.
// 
// 38    4/24/98 6:29p Fredf
// Uses MFC functions SaveBarState() and LoadBarState().
// 
// 37    4/23/98 7:23p Fredf
// Show/Hide Help Button.
// 
// 36    4/22/98 1:49p Dennis
// Added Art and More Store 
// 
// 35    4/21/98 8:49a Dennis
// Modified DoUpgrade() for American Greetings Upgradable products.
// 
// 34    4/17/98 3:09p Hforman
// and stay out!
// 
// 33    4/16/98 10:33a Rlovejoy
// Don't open the hub if main frame is disabled.
// 
// 32    4/16/98 9:11a Dennis
// Added back pictdlg.h which is required.
// 
// 31    4/15/98 5:59p Hforman
// remove unused include
// 
// 30    4/14/98 7:59p Fredf
// First attempt at Help Window.
// 
// 29    4/14/98 3:49p Dennis
// Added American Greetings Specific changes to DoUpgrade()
// 
// 28    4/14/98 1:21p Rlovejoy
// Use IDC_HUB_ART_GALLERY to launch browser.
// 
// 27    4/14/98 10:10a Dennis
// a) Added GetBuildInfo() method for selecting build info.
// b) Enhanced OnBuild… methods to use GetBuildInfo().
// 
// 26    4/13/98 10:28a Jayn
// 
// 25    4/10/98 4:56p Rlovejoy
// Changes for new hub implementation.
// 
// 24    4/07/98 7:18p Fredf
// Border and drawing object toolbar buttons now mean "Add" instead of
// "Edit".
// 
// 23    4/07/98 9:33a Fredf
// Start of new Hub window.
// 
// 22    4/07/98 8:45a Jayn
// Better picture build strings.
// 
// 21    3/21/98 1:18p Fredf
// New menus and toolbar names.
// 
// 20    3/18/98 6:32p Fredf
// First attempt at turning both side toolbars on and off together.
// 
// 19    3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 18    3/03/98 5:26p Hforman
// fixed bug: m_pTutorialList not being initialized to NULL
// 
// 17    2/27/98 2:45p Jayn
// Added Extract Project
// 
// 16    1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 15    12/23/97 10:27a Jayn
// 
// 14    12/18/97 12:45p Hforman
// any exit now goes through OnClose()
// 
// 13    12/17/97 2:15p Jayn
// Commented out some TRACEs.
// 
// 12    12/16/97 5:44p Hforman
// helpful hint menuitem message handlers
// 
// 11    12/12/97 5:18p Hforman
// add new menu item handlers for Help menu
// 
// 10    12/05/97 5:18p Hforman
// showing Helpful Hints instead of Welcome dialogs
// 
// 9     12/04/97 12:46p Hforman
// added helpful hint stuff
// 
// 8     11/21/97 4:32p Hforman
// Took out TRACE statement in OnMouseActivate()
// 
// 7     11/05/97 6:32p Hforman
// 
// 6     10/31/97 4:58p Johno
// Call KillTrainingCard () when CMainFrame is destroyed
// 
// 5     10/31/97 2:55p Johno
// Overloaded WinHelp
// 
// 4     10/31/97 10:15a Johno
// Added OnTcard
// 
// 3     10/30/97 6:51p Hforman
// moved reading of Tutorial list to FinishCreate()
// 
//    Rev 1.6   01 Oct 1997 13:58:22   johno
// moved ON_WM_WININICHANGE() out of AFX_MSG_MAP
// (ClassWizard no longer supports it?)
// 
//    Rev 1.5   17 Sep 1997 16:34:12   johno
// Support CWndHub.SetSkipButton() and
// CPrintMasterConfiguration.SupportsRegistrationBonus
// 
// 
//    Rev 1.4   05 Sep 1997 17:01:40   johno
// New hub welcome dialog, cleaned up DoWelcome
// 
//    Rev 1.3   22 Aug 1997 12:15:26   johno
// Changes for new 5.0 hub buttons
// 
//    Rev 1.2   21 Aug 1997 16:04:22   johno
// Your art and the path of respiration
// is working
// 
//    Rev 1.1   20 Aug 1997 17:28:18   johno
// Changes for new 5.0 buttons (preliminary)
// 
//    Rev 1.0   14 Aug 1997 15:22:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:08   Fred
// Initial revision.
// 
//    Rev 1.122   11 Aug 1997 17:12:14   johno
// No longer use different window property for add-on
// 
//    Rev 1.121   08 Aug 1997 13:47:40   johno
// Moved add on help disable to CPmwApp::WinHelp
// 
//    Rev 1.120   08 Aug 1997 11:53:38   johno
// Disable help (F1) for add ons
// 
//    Rev 1.119   30 Jul 1997 16:36:18   Fred
// Fixes for BUILD.INI
// 
//    Rev 1.118   30 Jul 1997 15:39:56   Fred
// BUILD.INI
// 
//    Rev 1.117   30 Jul 1997 12:50:44   johno
// Added case for add on in GetMainWindowProperty
// 
// 
//    Rev 1.116   24 Jul 1997 16:43:28   Jay
// Enforces max document limit in 16-bit
// 
//    Rev 1.115   20 Jul 1997 10:25:22   Fred
// Removed processing for Shift-F1 and F1 help for menu items
// 
//    Rev 1.114   18 Jul 1997 16:01:26   Fred
// Sentiment browser disable for CL and BP
// 
//    Rev 1.113   15 Jul 1997 15:56:26   Fred
// Tools are controlled by ShowTools=1; removed obsolete methods.
// 
//    Rev 1.112   14 Jul 1997 13:11:40   Jay
// BeginModalState totally disables window in 16-bit
// 
//    Rev 1.111   14 Jul 1997 11:18:24   Fred
// Added framework for 'Find projects with graphics' tool
// 
//    Rev 1.110   11 Jul 1997 17:08:46   johno
// Edited NGS build stuff
// 
//    Rev 1.109   11 Jul 1997 15:48:20   Fred
//  
// 
//    Rev 1.108   10 Jul 1997 17:02:26   Fred
// Makes sure popup palettes are on screen
// 
//    Rev 1.107   10 Jul 1997 12:02:32   johno
// Added NGI build stuff
// 
//    Rev 1.106   10 Jul 1997 09:00:18   Fred
//  
// 
//    Rev 1.105   09 Jul 1997 12:36:06   Fred
// Standard build paths
// 
//    Rev 1.104   01 Jul 1997 10:11:16   johno
// Use exit dialog with replaceable text in OnExit
// instead of message box
// 
//    Rev 1.103   01 Jul 1997 09:32:22   Jay
// MRU / Open document changes
// 
//    Rev 1.102   25 Jun 1997 16:23:56   johno
// Did NGI collection build
// 
//    Rev 1.101   23 Jun 1997 16:58:50   johno
// No longer use static hub window functions
// 
//    Rev 1.100   18 Jun 1997 18:48:42   doberthur
// With Help.
// 
// 
//    Rev 1.99   18 Jun 1997 16:39:14   johno
// Moved addon stuff to it's own file (adoninst.cpp)
// 
//    Rev 1.98   17 Jun 1997 16:12:30   johno
// Progress bar for addon install (preliminary)
// 
//    Rev 1.97   13 Jun 1997 15:11:28   johno
// 
//    Rev 1.96   12 Jun 1997 18:06:54   johno
// Add on stuff
// 
//    Rev 1.95   11 Jun 1997 12:02:50   johno
// OnAddonViewHTML() now supports PMW style paths ([[H]])
// 
//    Rev 1.94   10 Jun 1997 18:26:00   doberthur
// WithHelp bug fix, save as file.
// 
//    Rev 1.93   10 Jun 1997 16:21:26   johno
// Add on control moved to configuration class
// 
// 
//    Rev 1.92   09 Jun 1997 18:13:18   doberthur
// Add Your Project help dialog.
// 
//    Rev 1.91   09 Jun 1997 14:32:34   dennisp
// Added support to goto proper category from "Your Own" projects
// 
//    Rev 1.90   06 Jun 1997 16:41:30   doberthur
// Add flag to prevent sentiment welcome dialog.
// 
//    Rev 1.89   06 Jun 1997 15:38:54   doberthur
// Sentiment help.
// 
//    Rev 1.88   06 Jun 1997 11:51:18   Jay
// Licensing changes
// 
//    Rev 1.87   06 Jun 1997 11:06:22   johno
// Got rid of GetFilePath - use path manager
// 
// 
//    Rev 1.86   05 Jun 1997 18:09:38   johno
// More multiple hub stuff
// 
// 
//    Rev 1.85   05 Jun 1997 14:35:54   johno
// Changes for multiple hub support
// 
// 
//    Rev 1.84   04 Jun 1997 17:29:00   johno
// Support for multiple hubs
// 
// 
//    Rev 1.83   01 Jun 1997 11:55:10   johno
// Use renamed hub function SizeToParent (was SizeToRectangle) 
// 
// 
//    Rev 1.82   01 Jun 1997 09:00:46   Fred
// New browser build arguments
// 
//    Rev 1.81   29 May 1997 12:55:22   johno
// Free fonts registration
// 
// 
//    Rev 1.80   29 May 1997 09:07:50   johno
// Changes for register hub button
// 
//    Rev 1.79   28 May 1997 12:17:36   johno
// Improved detection and resizing of small hub windows
// 
//    Rev 1.78   27 May 1997 17:04:16   johno
// Made public DoWelcome from OnWelcome
// 
// 
//    Rev 1.77   27 May 1997 13:11:14   johno
// m_MciWnd is now a CSoundFX,
// which is derived from CMciWnd
// 
//    Rev 1.76   23 May 1997 16:51:12   johno
// Changes to sound effects
// 
//    Rev 1.75   23 May 1997 16:06:00   johno
// Button sound effects
// 
//    Rev 1.74   21 May 1997 18:18:28   johno
// Changes for welcome dialogs
// 
// 
//    Rev 1.73   20 May 1997 16:41:40   johno
// Check for pending content licensing on close
// 
//    Rev 1.72   19 May 1997 15:14:20   johno
// CreateHubWindow () will now resize the main window
// to the smallest allowed hub size if it is very small,
// as when closing a tiny workspace.
// 
//    Rev 1.71   19 May 1997 13:56:16   Fred
// Event Reminder, etc.
// 
//    Rev 1.70   12 May 1997 12:19:32   johno
// Moved some internet browser launching code
// to GOHTML.CPP
// 
//    Rev 1.69   09 May 1997 11:35:18   johno
// Set browser welcome color in OnArtGallery
// 
//    Rev 1.68   08 May 1997 16:46:04   johno
// Took out OnActivate, which called DoActivate
// in the hub window. This caused the "persistent
// button" problem.
// 
// 
//    Rev 1.67   08 May 1997 15:43:32   johno
// 
//    Rev 1.66   07 May 1997 14:49:34   johno
// Added dialog for internet browser launch failure
// 
//    Rev 1.65   30 Apr 1997 12:31:04   Jay
// Eradicated extraneous termination message.
// 
//    Rev 1.64   25 Apr 1997 12:13:38   Jay
// New paths for the build stuff.
// 
//    Rev 1.63   23 Apr 1997 14:21:44   johno
// Took out OnWelcome call in OnWithHelp
// (the first with help dialog posts a message)
// 
//    Rev 1.62   23 Apr 1997 14:13:02   Jay
// Picture builder.
// 
//    Rev 1.61   23 Apr 1997 10:20:32   Fred
// More browser stuff
// 
//    Rev 1.60   22 Apr 1997 13:54:56   johno
// Added color reference to call to CAddressBook::Run
// 
//    Rev 1.59   22 Apr 1997 13:16:58   Fred
// Start of project and art browsers
// 
//    Rev 1.58   22 Apr 1997 12:13:16   Jay
// 
//    Rev 1.57   22 Apr 1997 10:06:28   doberthur
// Add WithHelp wizard code to OnWithHelp.
// 
// 
//    Rev 1.56   22 Apr 1997 08:36:22   Jay
// Build menu items
// 
//    Rev 1.55   21 Apr 1997 17:48:56   johno
// Open welcome dialog with NULL as parent
// (makes focus work)
// 
//    Rev 1.54   21 Apr 1997 16:53:08   johno
// Changes for welcome dialogs
// 
// 
//    Rev 1.53   21 Apr 1997 10:51:30   johno
// Changes for welcome dialog
// 
//    Rev 1.52   18 Apr 1997 12:43:20   johno
// Took out OnBrandNew - it wasn't doing anything
// 
//    Rev 1.51   31 Mar 1997 17:12:30   Jay
// Start of NFX code
// 
//    Rev 1.50   31 Mar 1997 14:55:24   johno
// Message from MDI child controls hub, toolbars
// 
//    Rev 1.49   31 Mar 1997 14:34:12   johno
// Removed USE_HUB
// 
// 
//    Rev 1.48   31 Mar 1997 10:38:18   Fred
// Smacker/movie support
// 
//    Rev 1.47   28 Mar 1997 17:10:46   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.46   28 Mar 1997 13:20:02   johno
// Hub buttons bring up "open from disk"
// 
//    Rev 1.45   24 Mar 1997 16:15:12   Jay
// Moved hub implementation from MAINFRM.H to MAINFRM.CPP
// 
//    Rev 1.44   18 Mar 1997 13:23:16   Jay
// Preview stuff
// 
//    Rev 1.43   17 Mar 1997 17:24:04   Jay
// Preview stuff.
// 
//    Rev 1.42   17 Mar 1997 16:37:54   Jay
// Fixed some annoying stuff.
// 
//    Rev 1.41   14 Mar 1997 11:02:50   Fred
// Extract Project Text
// 
//    Rev 1.40   14 Mar 1997 10:34:18   johno
// 
//    Rev 1.39   12 Mar 1997 17:34:36   johno
// Changes for HUB resource DLL
// 
//    Rev 1.38   11 Mar 1997 16:23:34   johno
// Hub button message handlers
// 
//    Rev 1.37   10 Mar 1997 17:09:50   johno
// Moved HUB control here from App class.
// 
// 
//    Rev 1.36   07 Mar 1997 17:37:52   johno
// Initial HUB in PM
// 
// 
//    Rev 1.35   03 Feb 1997 15:55:04   Jay
// Custom tool buttons
// 
//    Rev 1.34   31 Jan 1997 15:51:06   Jay
// Some new toolbar bitmaps.
// 
//    Rev 1.33   28 Jan 1997 14:10:18   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.32   17 Jan 1997 09:17:26   Jay
// New status panes.
// 
//    Rev 1.31   16 Jan 1997 13:02:36   Jay
// New status stuff. More cleanup.
// 
//    Rev 1.30   15 Jan 1997 10:49:20   Jay
// New toolbars
// 
//    Rev 1.29   06 Nov 1996 14:22:26   johno
// Moved strings to resource DLL
// 
//    Rev 1.28   01 Nov 1996 09:27:20   Jay
// Moved over from 3.01.17
// 
//    Rev 1.28   30 Oct 1996 10:29:22   Fred
// Handles second instance better
// 
//    Rev 1.27   16 Sep 1996 17:18:32   Jay
// Doesn't make project album if program is exiting.
// 
//    Rev 1.26   16 Sep 1996 12:38:48   Fred
// Profile stuff can be disabled
// 
//    Rev 1.25   13 Sep 1996 09:53:08   Fred
// Profile Stuff -- will be removed
// 
//    Rev 1.24   08 Sep 1996 09:40:44   Jay
// No upgrade menu item if can't instant upgrade.
// 
//    Rev 1.23   02 Sep 1996 14:31:28   Fred
// New picture and project information dialogs
// 
//    Rev 1.22   01 Sep 1996 17:27:06   Fred
// Added double-sided print setup to main menus
// 
//    Rev 1.21   24 Aug 1996 14:14:12   Fred
// Properly focuses album control
// 
//    Rev 1.20   02 Aug 1996 13:11:28   Fred
// More 16-bit changes
// 
//    Rev 1.19   29 Jul 1996 18:13:36   Fred
//  
// 
//    Rev 1.18   26 Jul 1996 13:28:14   Fred
// New album controls
// 
//    Rev 1.17   25 Jul 1996 13:50:06   Fred
// Sound stop, play, inspect menu items
// 
//    Rev 1.16   25 Jul 1996 11:54:00   Fred
// Added sound to menu
// 
//    Rev 1.15   25 Jul 1996 08:38:18   Fred
// Fixed bug with view menu fixup
// 
//    Rev 1.14   25 Jul 1996 08:33:54   Fred
// Menu and tool bar changes
// 
//    Rev 1.13   24 Jul 1996 13:17:16   Fred
// Gadget hookup
// 
//    Rev 1.12   23 Jul 1996 16:52:18   Fred
// Some gadget stuff
// 
//    Rev 1.11   23 Jul 1996 08:08:30   Fred
// Some changes to control bars -- not finished
// 
//    Rev 1.10   16 Jul 1996 17:31:08   Fred
// More address/sender book stuff
// 
//    Rev 1.9   16 Jul 1996 08:51:26   Fred
// Hooked up address/sender book
// 
//    Rev 1.8   12 Jul 1996 16:27:12   Fred
// Address book error handling
// 
//    Rev 1.7   02 Jul 1996 10:56:36   Fred
// Sender Information
// 
//    Rev 1.6   10 Jun 1996 08:21:04   Fred
//  
// 
//    Rev 1.5   05 Jun 1996 08:49:06   Fred
//  
// 
//    Rev 1.4   24 May 1996 16:11:52   Fred
// TRACEx
// 
//    Rev 1.3   15 May 1996 08:46:10   Jay
// From PMW2
// 

#include "stdafx.h"
#include "pmw.h"

#include "mainfrm.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "pmwprint.h"
#include "pmwini.h"
#include "typeface.h"
#include "register.h"
#include "pmgfont.h"
#include "testprnt.h"
#include "printer.h"
#include "dlgmgr.h"
#include "compdb.h"

#include "catalog.h"

#include "license.h"
#include "utils.h"
#include "cdcache.h"

#include "prefpsh.h"

#include "upgdll.h"              // For DoUpgrade() below

#include "addrbook.h"
#include "addrimp.h"
#include "flatdb.h"
#include "printwiz.h"
#include "TutList.h"
#include "TutorDlg.h"
#include "..\Tutorial\PM\TutRes.h"
#include "MatchSet.h"


#include <dde.h>
#include <cderr.h>
#include <afxpriv.h>
#include <dbt.h>

#include "upgradlg.h"
#include "pmwcfg.h"
#include "nfx.h"

#include "WHlpProj.h"
#include "browser.h"
#include "DlgURL.h"
#include "aboutdlg.h"
#include "toolbar.h"
#include "fontprvw.h"

//#define  JTRACE_ON
#include "jtrace.h"

// For builder stuff
#include "pmdirdlg.h"   // for CChooseDirectoryDialog
#include "inifile.h"
#include "artstore.h"
#include "docdump.h"    // For CDocDump
#include "CollSel.h"    // For CollectionSelectionDialog
#include "Disptext.h"   // For CDisplayTextDialog in OnShowLegals()
#include "wndhub.h"
#include "promtdlg.h"
#include "CheckArtExists.h"
#include "exgraf.h"
#include "ConnMgrW.h"
#include "dbbld.h"

extern void free_palette(void);
extern CPalette *pOurPal;
extern BOOL DoHTML (HWND hwnd, LPCSTR url, BOOL GotoPage = FALSE);
extern void DoAddonInstall (CWnd *pWnd);

extern void DumpDocument(void);		// In docdump.*

#ifdef _WIN32
#include <..\src\afximpl.h>
#define IS_DESCENDENT(hw1, hw2) (AfxIsDescendant(hw1, hw2))
#else
#include <auxdata.h>
#define IS_DESCENDENT(hw1, hw2) (_AfxIsDescendant(hw1, hw2))
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

typedef BOOL (WINAPI* LOAD_TOOLBAR)(CWnd* pParent, CPalette* pPalette);
typedef CToolBar* (WINAPI* GET_TOOLBAR)(UINT nID);


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

UINT CMainFrame::m_uContentChangedMessage = ::RegisterWindowMessage("PMContentChanged");
UINT CMainFrame::m_uSharedPathsChangedMessage = ::RegisterWindowMessage("MindscapeSharedPathsChanged");

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
   //{{AFX_MSG_MAP(CMainFrame)
   ON_WM_CREATE()
   ON_WM_DESTROY()
   ON_WM_QUERYNEWPALETTE()
   ON_WM_PALETTECHANGED()
   ON_WM_ENTERIDLE()
   ON_COMMAND(ID_FILE_PICTURE_CATALOG, OnFilePictureCatalog)
   ON_WM_DEVMODECHANGE()
   ON_WM_CHANGECBCHAIN()
   ON_WM_DESTROYCLIPBOARD()
   ON_WM_DRAWCLIPBOARD()
   ON_WM_FONTCHANGE()
   ON_WM_SYSCOLORCHANGE()
   ON_COMMAND(ID_FILE_NETWORK_LICENSE, OnFileNetworkLicense)
   ON_WM_RENDERALLFORMATS()
   ON_WM_RENDERFORMAT()
   ON_COMMAND(ID_FILE_FREE_FONTS, OnFileFreeFonts)
   ON_UPDATE_COMMAND_UI(ID_FILE_FREE_FONTS, OnUpdateFileFreeFonts)
   ON_WM_SETCURSOR()
   ON_WM_SIZE()
   ON_UPDATE_COMMAND_UI(ID_FILE_OPENFROMDISK, OnUpdateFileOpenfromdisk)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)   
	ON_COMMAND(ID_FILE_OPENFROMDISK, OnFileOpenfromdisk)
   ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
   ON_UPDATE_COMMAND_UI(ID_FILE_PREFERENCES, OnUpdateFilePreferences)
   ON_COMMAND(IDC_PRINTER_ALIGNMENT, OnPrinterAlignment)
   ON_COMMAND(ID_FILE_UPGRADE, OnFileUpgrade)
   ON_UPDATE_COMMAND_UI(ID_FILE_UPGRADE, OnUpdateFileUpgrade)
   ON_COMMAND(ID_VIEW_ADDRESS_BOOK, OnViewAddressBook)
   ON_COMMAND(ID_VIEW_SENDER_INFORMATION, OnViewSenderInformation)
   ON_COMMAND(ID_EVENT_REMINDER, OnEventReminder)
   ON_UPDATE_COMMAND_UI(ID_EVENT_REMINDER, OnUpdateEventReminder)
   ON_UPDATE_COMMAND_UI(ID_SOUND_PLAY, OnUpdateSoundAvailable)
   ON_COMMAND(ID_SOUND_PREFERENCES, OnSoundPreferences)
   ON_COMMAND(ID_FILE_IMPORT_NAMES, OnFileImportNames)
   ON_COMMAND(IDC_DOUBLE_SIDED_PRINT_SETUP, OnDoubleSidedPrintSetup)
   ON_WM_PARENTNOTIFY()
	ON_WM_INITMENU()
	ON_WM_INITMENUPOPUP()
	ON_WM_TCARD()
	ON_WM_NCLBUTTONDOWN()
	ON_COMMAND(ID_HELP_RECENT_HINTS, OnHelpRecentHints)
	ON_COMMAND(ID_HELP_PREFERENCES, OnHelpPreferences)
	ON_COMMAND(ID_HELP_ENABLE_HINTS, OnHelpEnableHints)
	ON_UPDATE_COMMAND_UI(ID_HELP_ENABLE_HINTS, OnUpdateHelpEnableHints)
	ON_COMMAND(ID_HELP_ENABLE_TIPS, OnHelpEnableTips)
	ON_UPDATE_COMMAND_UI(ID_HELP_ENABLE_TIPS, OnUpdateHelpEnableTips)
	ON_WM_CLOSE()
	ON_COMMAND(ID_SHOW_LEGALS, OnShowLegals)
	ON_UPDATE_COMMAND_UI(ID_PERSONAL_DELIVERY, OnUpdatePersonalDelivery)
	ON_UPDATE_COMMAND_UI(ID_HUB_NEW, OnUpdateHubNew)
	ON_COMMAND(ID_HUB_NEW, OnHubNew)
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_ONLINE_PREFERENCES, OnOnlinePreferences)
   ON_WM_ACTIVATE()
   ON_UPDATE_COMMAND_UI(ID_SOUND_EXPLAIN, OnUpdateSoundAvailable)
   ON_UPDATE_COMMAND_UI(ID_SOUND_PREFERENCES, OnUpdateSoundAvailable)
	ON_UPDATE_COMMAND_UI(ID_ONLINE_PREFERENCES, OnUpdateOnlinePreferences)
	ON_WM_ACTIVATEAPP()
	//}}AFX_MSG_MAP
   ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_WAKEAPP, OnWakeApp)
   ON_COMMAND_RANGE(ID_TUTORIAL_FIRST, ID_TUTORIAL_LAST, OnTutorialChoice)
   ON_WM_WININICHANGE()
   ON_REGISTERED_MESSAGE(CCartoonOMatic::m_uTerminationMessage, OnNfxTermination)
   ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateControlBarMenu)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FORMATBAR, OnUpdateControlBarMenu)
   ON_UPDATE_COMMAND_UI(ID_VIEW_BARMENU, OnUpdateControlBarMenu)
   ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateControlBarMenu)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FILEBAR, OnUpdateControlBarMenu)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FORMATBAR, OnUpdateControlBarMenu)
   ON_UPDATE_COMMAND_UI(ID_VIEW_EDIT1BAR, OnUpdateControlBarMenu)
   ON_UPDATE_COMMAND_UI(ID_VIEW_HELPWINDOW, OnUpdateControlBarMenu)
   ON_COMMAND_EX(ID_VIEW_STATUS_BAR, OnOurBarCheck)
   ON_COMMAND_EX(ID_VIEW_FILEBAR, OnOurBarCheck)
   ON_COMMAND_EX(ID_VIEW_FORMATBAR, OnOurBarCheck)
   ON_COMMAND_EX(ID_VIEW_EDIT1BAR, OnOurBarCheck)
   ON_COMMAND_EX(ID_VIEW_HELPWINDOW, OnOurBarCheck)
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
   ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   // message handling for standard DDE commands
   ON_MESSAGE(WM_DDE_INITIATE, OnDDEInitiate)
   ON_MESSAGE(WM_DDE_EXECUTE, OnDDEExecute)
   ON_MESSAGE(WM_DDE_TERMINATE, OnDDETerminate)
   ON_WM_TIMER()
   ON_UPDATE_COMMAND_UI(ID_PROFILE_SHOW, OnUpdateProfileShow)
   ON_COMMAND(ID_PROFILE_SHOW, OnProfileShow)
   ON_UPDATE_COMMAND_UI(ID_FIND_PROJECTS_WITH_GRAPHICS, OnUpdateDeleteToolMenuItem)
   ON_UPDATE_COMMAND_UI(ID_EXTRACT_PROJECT_TEXT, OnUpdateDeleteToolMenuItem)
   ON_UPDATE_COMMAND_UI(ID_EXTRACT_PROJECT_SENTIMENTS, OnUpdateDeleteToolMenuItem)
   ON_UPDATE_COMMAND_UI(ID_CHECK_ART_EXISTS_DIR, OnUpdateDeleteToolMenuItem)
   ON_UPDATE_COMMAND_UI(ID_CHECK_ART_EXISTS_SS, OnUpdateDeleteToolMenuItem)
   ON_UPDATE_COMMAND_UI(ID_PRINT_PROJECTS, OnUpdateDeleteToolMenuItem)
   ON_UPDATE_COMMAND_UI(ID_WEB_ART_BROWSER, OnUpdateWebArtBrowserMenuItem)
   ON_COMMAND(ID_EXTRACT_PROJECT_TEXT, OnExtractProjectText)
   ON_COMMAND(ID_EXTRACT_PROJECT_SENTIMENTS, OnExtractProjectSentiments)
   ON_COMMAND(ID_FIND_PROJECTS_WITH_GRAPHICS, OnFindProjectsWithGraphics)
   ON_COMMAND(ID_CHECK_ART_EXISTS_DIR, OnCheckArtExistsDir)
   ON_COMMAND(ID_CHECK_ART_EXISTS_SS, OnCheckArtExistsSS)
	ON_COMMAND(ID_EXTRACT_ART_USED, OnExtractArtUsed)
   ON_UPDATE_COMMAND_UI(ID_EXTRACT_ART_USED, OnUpdateDeleteToolMenuItem)
   ON_COMMAND(ID_PRINT_PROJECTS, OnPrintProjects)
   ON_COMMAND(ID_WEB_ART_BROWSER, OnWebArtBrowser)
   ON_COMMAND(ID_DUMP_DOC, OnDumpDoc)
   ON_UPDATE_COMMAND_UI(ID_DUMP_DOC, OnUpdateDumpDoc)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_POSITION, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_SIZE, OnUpdateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_ROTATE, OnUpdateIndicator)
   ON_WM_MOUSEACTIVATE()
   ON_WM_GETMINMAXINFO()
   ON_UPDATE_COMMAND_UI(ID_EXTRACT_CONTENT, OnUpdateDeleteToolMenuItem)
   ON_COMMAND(ID_EXTRACT_CONTENT, OnExtractContent)
   // Hub buttons
   ON_COMMAND(IDC_WITH_HELP, OnWithHelp)
   ON_COMMAND(IDC_YOUR_PROJECTS, OnYourProjects) 
   ON_COMMAND(IDC_INTERNET, OnInternet)
   ON_COMMAND(ID_3DPLANET, On3DPlanet)
	ON_UPDATE_COMMAND_UI(ID_3DPLANET, OnUpdate3DPlanet)
	ON_COMMAND(ID_PHOTOORGANIZER, OnPhotoOrganizer)
	ON_UPDATE_COMMAND_UI(ID_PHOTOORGANIZER, OnUpdatePhotoOrganizer)
   ON_COMMAND(IDC_ADDRESS, OnAddressBook)
   // JRO 5.0 hub buttons
   ON_COMMAND(IDC_HUB_EXIT, OnClose)
	ON_MESSAGE(WM_HUB_QUERY_ACTION, OnHubQueryAction)
   //
   ON_COMMAND(IDC_EXIT_ALBUM, OnExitAlbum)
   ON_MESSAGE(WM_VIEWSCHANGED, OnViewsChanged)
   ON_UPDATE_COMMAND_UI(ID_BUILD_PROJECTS, OnUpdateBuildBrowser)
   ON_UPDATE_COMMAND_UI(ID_BUILD_PICTURES, OnUpdateBuildBrowser)
   ON_UPDATE_COMMAND_UI(ID_PREPROCESS_PICTURES, OnUpdateBuildBrowser)
   ON_UPDATE_COMMAND_UI(ID_BUILD_SENTIMENTS, OnUpdateBuildBrowser)
   ON_UPDATE_COMMAND_UI(ID_BUILD_ART_COMPONENT_DATABASE, OnUpdateBuildBrowser)
   ON_UPDATE_COMMAND_UI(ID_BUILD_PAGE_COMPONENT_DATABASE, OnUpdateBuildBrowser)
   ON_COMMAND(ID_BUILD_PROJECTS, OnBuildProjects)
   ON_COMMAND(ID_BUILD_PICTURES, OnBuildPictures)
   ON_COMMAND(ID_PREPROCESS_PICTURES, OnPreprocessPictures)
   ON_COMMAND(ID_BUILD_SENTIMENTS, OnBuildSentiments)
	ON_COMMAND(ID_BUILD_ART_COMPONENT_DATABASE, OnBuildArtComponentDatabase)
	ON_COMMAND(ID_BUILD_PAGE_COMPONENT_DATABASE, OnBuildPageComponentDatabase)

   ON_COMMAND(IDC_ADDON_INSTALL,    OnAddonInstall)
   ON_COMMAND(IDC_ADDON_VIEW_ART,   OnAddonViewArt)
   ON_COMMAND(IDC_ADDON_VIEW_PROJ,  OnAddonViewProj)
   ON_COMMAND(IDC_ADDON_VIEW_BIO,   OnAddonViewHTML)

   ON_REGISTERED_MESSAGE(m_uContentChangedMessage, OnContentChanged)
   ON_REGISTERED_MESSAGE(m_uSharedPathsChangedMessage, OnSharedPathsChanged)

	ON_COMMAND(ID_DESKTOP_ARTSTORE, OnDesktopArtStore)
	ON_UPDATE_COMMAND_UI(ID_DESKTOP_ARTSTORE, OnUpdateDesktopArtStore)
   ON_MESSAGE(UM_BUILD_FONT_LIST, OnBuildFontList)
   ON_MESSAGE(UM_SHOW_PREVIEW, OnShowFontPreview)
   ON_MESSAGE(UM_MOVE_PREVIEW, OnMoveFontPreview)
   ON_MESSAGE(UM_UPDATE_PREVIEW, OnUpdateFontPreview)
   ON_MESSAGE(UM_PREVIEW_VISIBLE, IsPreviewVisible)

   END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame():
	m_pFontPreview(NULL),
	m_hToolbarDll(NULL)
{
   m_hwndNextViewer  = NULL;
   m_fWaitingForDDE  = FALSE;
   m_pTutorialDlg    = NULL;
   m_pTutorialList   = NULL;
   m_fInPreviewMode  = FALSE;
   m_fAppClosingDown = FALSE;
}

CMainFrame::~CMainFrame()
{
	delete m_pTutorialDlg;
	if (m_pTutorialList)
	{
		m_pTutorialList->DestroyList();
		delete m_pTutorialList;
	}
	if(m_hToolbarDll)
		FreeLibrary(m_hToolbarDll);

	if(m_pFontPreview)
		delete m_pFontPreview;
}

CWnd* CMainFrame::GetMessageBar()
{
   // We do not have a message bar anymore.
   return m_fInPreviewMode ? INHERITED::GetMessageBar() : NULL;
}

CString CMainFrame::GetMainWindowProperty(void)
{
   //if (CPrintMasterConfiguration::IsAddOn () == TRUE)
      //return "PrintMasterMainWindowAddOn"; 
   
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	CString csIniPath(path);
	csIniPath = csIniPath.Left(csIniPath.ReverseFind('\\') + 1);
	csIniPath += "MSREG.INI";
   CIniFile MsregIniFile(csIniPath);
   return MsregIniFile.GetString(SECTION_Configuration, "MainWndProp", "PrintMaster8MainWindow");
}

BOOL RemoveUnneededMenus(CMenu* pMenu)
{
   BOOL fChanged = FALSE;
   return fChanged;
}

BOOL CMainFrame::CreateSeparatorGadget(void)
{
   CRect crRect(0,0,0,0);

   if (!m_wndSeparatorGadget.Create(
         NULL,
         "Separator Gadget",
         WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,
         crRect,
         this,
         ID_SEPARATOR_GADGET,
         NULL))
   {
      TRACE0("Failed to create separator gadget\n");
      return FALSE;     // fail to create
   }

   return TRUE;
}

BOOL CMainFrame::CreatePageGadget(void)
{
   CRect crRect(0,0,0,0);

   if (!m_wndPageGadget.Create(
         NULL,
         "Page Gadget",
         WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,
         crRect,
         this,
         ID_PAGE_GADGET,
         NULL))
   {
      TRACE0("Failed to create page gadget\n");
      return FALSE;     // fail to create
   }

   return TRUE;
}

BOOL CMainFrame::CreateZoomGadget(void)
{
   CRect crRect(0,0,0,0);

   if (!m_wndZoomGadget.Create(
         NULL,
         "Zoom Gadget",
         WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,
         crRect,
         this,
         ID_ZOOM_GADGET,
         NULL))
   {
      TRACE0("Failed to create zoom gadget\n");
      return FALSE;     // fail to create
   }

   return TRUE;
}

BOOL CMainFrame::CreateStatusBar(void)
{
   if (!m_wndStatusBar.Create(this))
   {
      TRACE0("Failed to create status bar\n");
      return FALSE;     // fail to create
   }

#if 0
   if (!m_fStatusBarVisible)
   {
   /* Turn it off. */
      CMDIFrameWnd::OnBarCheck(ID_VIEW_STATUS_BAR);
   }
#endif

   return TRUE;
}

//
// Create the new tool bars.
//

BOOL CMainFrame::CreateNewToolBars(void)
{
	BOOL bRetVal = FALSE;
	CString csLibPath = GetGlobalPathManager()->ExpandPath("Toolbar.dll");
	m_hToolbarDll = LoadLibrary(csLibPath);

	if(m_hToolbarDll)
	{
		LOAD_TOOLBAR pLoad = (LOAD_TOOLBAR)::GetProcAddress(m_hToolbarDll, "LoadToolbar");
		if (pLoad != NULL)
		{
			if(pLoad(this, pOurPal))
				bRetVal =  TRUE;
		}

		m_pFontPreview = new CFontPreviewWnd(this);
		m_pFontPreview->ShowWindow(SW_HIDE);
		CToolBar* pToolBar = GetToolBar(TOOLBAR_FORMAT);

   }
   return bRetVal;
}

BOOL CMainFrame::CreateHelpWindow(void)
{
	BOOL fResult = FALSE;

	CString csHelpFile;

	TRY
	{
		csHelpFile = GetGlobalPathManager()->ExpandPath("PMW.CHM");

		if (m_wndHelpWindow.Create(csHelpFile, this, IDD_HELP_WINDOW, CBRS_RIGHT, ID_VIEW_HELPWINDOW, 180))
		{
			m_wndHelpWindow.SetBarStyle(
					m_wndHelpWindow.GetBarStyle() |
					CBRS_TOOLTIPS |
					CBRS_FLYBY |
					CBRS_SIZE_DYNAMIC);

			m_wndHelpWindow.EnableDocking(CBRS_ALIGN_RIGHT);
			EnableDocking(CBRS_ALIGN_ANY);
			DockControlBar(&m_wndHelpWindow);

			fResult = TRUE;
		}
	}
	END_TRY

	return fResult;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CMDIFrameWnd::OnCreate(lpCreateStruct) == 0)
   {
      ::SetProp(GetSafeHwnd(), GetMainWindowProperty(), (HANDLE)1);
      return 0;
   }

   return -1;
}

void CMainFrame::FixupConfigurationNames(void)
{
   m_strTitle.LoadString(IDR_MAINFRAME);
   GetConfiguration()->ReplaceText(m_strTitle);

   OnUpdateFrameTitle(TRUE);
}

/*
// Create the tool bars we will be needing.
*/

BOOL CMainFrame::FinishCreate(void)
{

   // create the list of tutorials
   m_pTutorialList   = new CTutorialList;
   m_pTutorialList->CreateList();

   FixupConfigurationNames();

#if 0
   if (InitializeMenu())
   {
      DrawMenuBar();
   }
#endif

   if (!CreateNewToolBars())
   {
      return FALSE;
   }

   if (!CreateSeparatorGadget())
   {
      return FALSE;
   }

   if (!CreatePageGadget())
   {
      return FALSE;
   }

   if (!CreateZoomGadget())
   {
      return FALSE;
   }

   if (!CreateStatusBar())
   {
      return FALSE;
   }

   m_wndSeparatorGadget.SetFont(m_wndStatusBar.GetFont());
   m_wndPageGadget.SetFont(m_wndStatusBar.GetFont());
   m_wndZoomGadget.SetFont(m_wndStatusBar.GetFont());

	if (!CreateHelpWindow())
	{
		return FALSE;
	}

/* Hook us into the clipboard viewing chain. */

   m_hwndNextViewer = SetClipboardViewer();
// od("Set viewer got %x...\r\n", m_hwndNextViewer);

   return TRUE;
}

// Disable menu commands while waiting for OLE server
BOOL CMainFrame::OnCommand(UINT wParam, LONG lParam)
{
   return CMDIFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::BeginModalState(BOOL fOtherApp /*=FALSE*/)
{
	if (fOtherApp)
	{
#ifndef WIN32
		::SetProp(GetSafeHwnd(), "StayDisabled", (HANDLE)1);
#endif
	}
	CMDIFrameWnd::BeginModalState();
}

void CMainFrame::EndModalState(BOOL fOtherApp /*=FALSE*/)
{
	if (fOtherApp)
	{
#ifndef WIN32
		::RemoveProp(GetSafeHwnd(), "StayDisabled");
#endif
	}
	CMDIFrameWnd::EndModalState();
}

void CMainFrame::ShowTutorial(int nID)
{
   if (m_pTutorialDlg == NULL)
      m_pTutorialDlg = new CTutorialDlg;

   m_pTutorialDlg->ShowTutorial(nID);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
   CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
   CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnDestroy()
{
   GET_PMWAPP()->SaveWindowPosition(this, "Frame");

   ::RemoveProp(GetSafeHwnd(), GetMainWindowProperty());

   CMDIFrameWnd::OnDestroy();

/* Remove us from the clipboard viewing chain. */

// od("Destroy...\r\n");
   ChangeClipboardChain(m_hwndNextViewer);

/* Free the palette we used. */

   free_palette();   
}

BOOL CMainFrame::OnQueryNewPalette()
{               
   int i = 0;

   if (GET_PMWAPP()->GetProcessPaletteMessages())
   {
      CDC *pDC = GetDC();

      CPalette *pOldPal = pDC->SelectPalette(pOurPal, FALSE);

      if (pOldPal != NULL)
      {
         i = pDC->RealizePalette();
   
         pDC->SelectPalette(pOldPal, FALSE);
      }

      ReleaseDC(pDC);

      if (i > 0)
      {
         RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
      }
   }

   return i;
}

void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd)
{
   if (GET_PMWAPP()->GetProcessPaletteMessages())
   {
      if (pFocusWnd != this)
      {
         OnQueryNewPalette();
      }
   }
}

void CMainFrame::OnEnterIdle(UINT nWhy, CWnd* pWho)
{
   CMDIFrameWnd::OnEnterIdle(nWhy, pWho);
   
   if ((pWho != NULL)
	 && (::GetWindowThreadProcessId(pWho->GetSafeHwnd(), NULL) != ::GetCurrentThreadId()))
   {
      return;
   }

   if ((nWhy == MSGF_DIALOGBOX) && (pWho != NULL))
   {
      pWho->SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
      pWho->SendMessageToDescendants(WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0, TRUE, TRUE);
   }
   
/*
// This code is lifted out of the MFC's app Run method.
// Hopefully it works as well here...
*/
   CPmwApp *app = GET_PMWAPP();
   LONG lIdleCount = 0;
   MSG msgCur;

   // check to see if we can do idle work

   while (!::PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE) &&
      app->OnIdle(lIdleCount++))
   {
      // more work to do
   }

   // either we have a message, or OnIdle returned false
}

void CMainFrame::OnFilePictureCatalog()
{
   if (!GetConfiguration()->SupportsPictureCatalog())
   {
      return;
   }

// AfxMessageBox("Picture Catalog is broken.", MB_OK | MB_ICONSTOP);
// return;

/* We need a document to build our printout with. */

   CCatalogDoc doc;

#ifdef SPECIAL_CATALOG
   if (!doc.DoNewDocument(LANDSCAPE))
#else
   if (!doc.DoNewDocument(PORTRAIT))
#endif
   {
      return;
   }

/* Choose the pictures. */

   CString filters;
   CString filters2;

   TRY
   {
      LoadConfigurationString(IDS_PICTURE_FILTERS_PART_1, filters);
      LoadConfigurationString(IDS_PICTURE_FILTERS_PART_2, filters2);
      filters += filters2;
   }
   END_TRY

   CPictureCatalogDialog dlg(&doc, AfxGetApp()->m_pMainWnd, filters);

/* Get the initial directory. */

	CString csCatalogDir = GetGlobalPathManager()->ExpandPath("[[P]]");
   dlg.m_ofn.lpstrInitialDir = csCatalogDir;

/* Run the dialog. */

   if (dlg.DoModal() != IDOK)
   {
      if (CommDlgExtendedError() != FNERR_BUFFERTOOSMALL)
      {
         return;
      }
   }

/* Proceed. */

   dlg.do_print();
}

void CMainFrame::OnDevModeChange(LPSTR lpDeviceName)
{
   CMDIFrameWnd::OnDevModeChange(lpDeviceName);
}

void CMainFrame::OnWinIniChange(LPCSTR lpszSection)
{
   CMDIFrameWnd::OnWinIniChange(lpszSection);
}

void CMainFrame::OnUpdateProfileShow(CCmdUI* pCmdUI) 
{
   if (!GET_PMWAPP()->IsProfiling())
   {
      pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
   }
}

void CMainFrame::OnProfileShow()
{
   GET_PMWAPP()->ProfileShow();
}

void CMainFrame::OnUpdateDeleteToolMenuItem(CCmdUI* pCmdUI) 
{
   if (!GET_PMWAPP()->OurGetProfileInt("Debug", "ShowTools", FALSE))
	{
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
	}
}

void CMainFrame::OnUpdateWebArtBrowserMenuItem(CCmdUI* pCmdUI) 
{
#if 0 // GCB 2/1/99 - change from debug command to regular command in the Add menu
   if (!GET_PMWAPP()->OurGetProfileInt("Debug", "ShowTools", FALSE))
	{
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
	}
#else
   pCmdUI->Enable(TRUE);
#endif
}

void CMainFrame::OnExtractProjectText()
{
   GET_PMWAPP()->ExtractProjectText();
}

void CMainFrame::OnExtractProjectSentiments()
{
   CExtractProjectText tool(FALSE);
   tool.Run();

}

void CMainFrame::OnFindProjectsWithGraphics()
{
   GET_PMWAPP()->FindProjectsWithGraphics();
}

void CMainFrame::OnCheckArtExistsDir()
{
	static CCheckArtExistsDir CheckArtDir;

	CheckArtDir.Run();
}

void CMainFrame::OnCheckArtExistsSS()
{
	static CCheckArtExistsSS CheckArtSS;

	CheckArtSS.Run();
}

void	CMainFrame::OnExtractArtUsed()
{
	CChooseDirectoryDialog dlg;
	dlg.m_ofn.lpstrTitle  = "Select Projects folder";
	if(dlg.DoModal() != IDOK)
		return;

	CString Projects = dlg.m_csDirectory;

	CExtractGraphics ExtractGraphics;
	ExtractGraphics.Extract(Projects);
}

void CMainFrame::OnExtractContent()
{
	CExtractContent George;
	George.Run();
}

void CMainFrame::OnDumpDoc()
{
	DumpDocument();
}

void CMainFrame::OnPrintProjects()
{
	CChooseDirectoryDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		GET_PMWAPP()->m_bPrintProjects = TRUE;
		//get the current dir
		CString csCurDir;
		GetCurrentDirectory(MAX_PATH, csCurDir.GetBuffer(MAX_PATH));
		csCurDir.ReleaseBuffer();
		if(csCurDir.IsEmpty())
		{
			AfxMessageBox("Failed to get current directory");
			return;
		}
		CString csPmwExt;
		csPmwExt.LoadString(IDS_DOCUMENT_FILTERS_PART_1);
		CString csDir = dlg.m_csDirectory;
		//set dir to selected dir
		SetCurrentDirectory(csDir);
		CFileFind ff;
		//loop through files
		BOOL bWorking = ff.FindFile("*.*");
		while(bWorking)
		{
			bWorking = ff.FindNextFile();
			if(!ff.IsDirectory())
			{
				//check to see if file has a printmaster file ext
				CString csPath = ff.GetFilePath();
				CString csExt = csPath.Right(4);
				if(csPmwExt.Find(csExt))
				{
					CPmwDoc* pDoc = (CPmwDoc*)GET_PMWAPP()->OpenDocumentFile(csPath);
					if(pDoc)
					{
						POSITION pos = pDoc->GetFirstViewPosition();
						CView* pView = pDoc->GetNextView(pos);
						if(pView)
							pView->SendMessage(WM_COMMAND, ID_FILE_PRINT_DIRECT);
						
						pDoc->OnCloseDocument();
					}
				}
			}
		}
		
		SetCurrentDirectory(csCurDir);
		GET_PMWAPP()->m_bPrintProjects = FALSE;
	}
}

void CMainFrame::OnWebArtBrowser()
{
	CPmwDoc *pDoc = NULL;
	CFrameWnd *pFrame = GetActiveFrame();
	if (pFrame)
		pDoc = (CPmwDoc*)(pFrame->GetActiveDocument());

	CWebArtBrowserDialog Dialog(NULL);
	Dialog.m_pDoc = pDoc;
	int nDialogRetVal = Dialog.DoModal();
}

void CMainFrame::OnUpdateDumpDoc(CCmdUI* pCmdUI)
{
   if (!GET_PMWAPP()->OurGetProfileInt("Debug", "ViewDocuments", FALSE))
	{
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
	}
}
/*
PRIVATE inline void new_project(SHORT type)
{
   
	GET_PMWAPP()->new_document(type);
}

PRIVATE inline void open_project(SHORT type)
{
   GET_PMWAPP()->open_a_document(type);
}
*/
void CMainFrame::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter)
{
// od("Change cb chain (remove: %d, after: %d)\r\n", hWndRemove, hWndAfter);
   if (hWndRemove == m_hwndNextViewer)
   {
   /* Fixup the chain. */
      m_hwndNextViewer = hWndAfter;
   }
   else if (m_hwndNextViewer != NULL)
   {
   /* Not our problem. Pass it on. */
#ifdef WIN32
      ::SendMessage(m_hwndNextViewer, WM_CHANGECBCHAIN, (WPARAM)hWndRemove, (LPARAM)hWndAfter);
#else
      ::SendMessage(m_hwndNextViewer, WM_CHANGECBCHAIN, (WPARAM)hWndRemove, MAKELPARAM(hWndAfter, 0));
#endif
   }

   CMDIFrameWnd::OnChangeCbChain(hWndRemove, hWndAfter);
}

void CMainFrame::RebuildFonts(void)
{
   typeface_server.build_face_list();

	GetToolBar(TOOLBAR_FORMAT)->SendMessage(UM_UPDATE_FONTS);

   PMGFontServer::reset_fonts();

   GET_PMWAPP()->reset_doc_fonts();
}

void CMainFrame::OnFontChange()
{
   RebuildFonts();

   CMDIFrameWnd::OnFontChange();
}

void CMainFrame::OnFileNetworkLicense()
{
   CPmwApp *pApp = GET_PMWAPP();

   CLicenseDlg ldlg(&pApp->m_installation_info);

   ldlg.DoModal();
}

BOOL CMainFrame::OnOurBarCheck(UINT nID)
{
	BOOL fChanged = FALSE;

	if (nID == ID_VIEW_EDIT1BAR)
	{
		fChanged  = CMDIFrameWnd::OnBarCheck(ID_VIEW_EDIT1BAR);
		fChanged |= CMDIFrameWnd::OnBarCheck(ID_VIEW_EDIT2BAR);
	}
	else
	{
		fChanged =  CMDIFrameWnd::OnBarCheck(nID);
	}

	return fChanged;
}

void CMainFrame::OnUpdateControlBarMenu(CCmdUI* pCmdUI)
{
   CMDIFrameWnd::OnUpdateControlBarMenu(pCmdUI);
}

/*
// Handle the free fonts entry.
*/
void CMainFrame::OnFileFreeFonts()
{
   DoFileFreeFonts();
}

void CMainFrame::OnUpdateFileFreeFonts(CCmdUI *pCmdUI)
{
   if (GetConfiguration()->RemoveRegistration())
   {
      if (pCmdUI->m_pMenu)
		{
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
			Util::RemoveExtraMenuSeparators(pCmdUI->m_pMenu);
      }
   }
}

void CMainFrame::DoFileFreeFonts()
{
   if (RegisterDLL.IsBonusEnabled() == REGRESULT_AlreadyRegistered)
   {
      CPmwDialog dlg(IDD_BONUS_ALREADY_ENABLED);

      dlg.DoModal();

      return;
   }

   RegisterDLL.RegisterBonus();

   if (RegisterDLL.IsBonusEnabled() == REGRESULT_AlreadyRegistered)
   {
   /*
   // We want to rename all the pictures which end in "CGN" to "CGM".
   // And, yes, we even want to do it for the CD version (ask me if you
   // can't figure out why).
   */

      GET_PMWAPP()->UserHasRegistered();
   }
}

void CMainFrame::OnSysColorChange()
{
   CMDIFrameWnd::OnSysColorChange();
}

void CMainFrame::OnUpdateSoundAvailable(CCmdUI* pCmdUI)
{
//   pCmdUI->Enable(sound_manager.sounds_available());
}

void CMainFrame::OnSoundPreferences()
{
   CPreferencesSheet::SelectPreferences(0);
}

void CMainFrame::OnFileImportNames()
{
   if (GetConfiguration()->SupportsAddressBook())
   {
      CString csFullName;
      TRY
      {
         CAddressBookImportWizard::Run(this, csFullName);
      }
      END_TRY
   }
}

void CMainFrame::OnDoubleSidedPrintSetup() 
{
   CDoubleSidedPrintWizard::Run(this);
}

BOOL CMainFrame::PreTranslateMessage(MSG* msg)
{
   HACCEL hAccel = m_hAccelTable;

	CToolBar* pToolbar = GetToolBar(TOOLBAR_FORMAT);
   if (pToolbar && pToolbar->GetSafeHwnd() && pToolbar->SendMessage(UM_HAS_FOCUS,0,0))
   {
      m_hAccelTable = NULL;
   }

   BOOL fResult = CMDIFrameWnd::PreTranslateMessage(msg);

   /* Restore the accelerator table just in case it got NULL'd. */
   m_hAccelTable = hAccel;

   return fResult;
}

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   return CMDIFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
   CMDIFrameWnd::OnSize(nType, cx, cy);
}

void CMainFrame::RecalcLayout(BOOL fNotify /*=FALSE*/)
{
	m_wndHelpWindow.PreRecalcLayout();

	CRect r;
	NegotiateBorderSpace(borderGet, &r);
	m_wndHelpWindow.m_czDocked.cy = r.Height()+4;

   CMDIFrameWnd::RecalcLayout(fNotify);

	m_wndHelpWindow.PostRecalcLayout();
}

void CMainFrame::OnUpdateFileOpenfromdisk(CCmdUI* pCmdUI)
{
}

void CMainFrame::OnFileOpenfromdisk()
{
   GET_PMWAPP()->open_a_document();
}

void CMainFrame::OnFileOpen()
{
   // Run the project browser dialog.
	DoBrowserOrArtStore(Project);
}

void CMainFrame::OnFilePreferences()
{
   CPreferencesSheet::SelectPreferences();
}

void CMainFrame::OnUpdateFilePreferences(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(TRUE);
}

LRESULT CMainFrame::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
#ifdef WIN32
   CMDIFrameWnd::OnIdleUpdateCmdUI();
#else
   CMDIFrameWnd::OnIdleUpdateCmdUI(wParam, lParam);
#endif
   return 0L;
}

void CMainFrame::OnPrinterAlignment()
{
   CPrinterAlignmentTestDialog Dialog(this);
   
   if (Dialog.DoModal() == IDOK)
   {
      CPmwApp* pApp = GET_PMWAPP();
      if (pApp != NULL)
      {
         CPrinterInfo* pPrinterInfo;
         if ((pPrinterInfo = pApp->GetPrinterInfo(PORTRAIT)) != NULL)
         {
            pPrinterInfo->WriteSettings();
         }
         if ((pPrinterInfo = pApp->GetPrinterInfo(LANDSCAPE)) != NULL)
         {
            pPrinterInfo->WriteSettings();
         }
         pApp->update_paper_info();
      }
   }
}

LRESULT CMainFrame::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	// For PMG 4.0, the help file does not have help for menu items
	// (what a feeble help file!) So, we have to just return TRUE in
	// this case without being helpful.
	if (IsTracking())
	{
		return TRUE;
	}
   
   if (lParam == 0 && IsTracking())
      lParam = HID_BASE_COMMAND+m_nIDTracking;

   CMDIChildWnd* pActiveChild = MDIGetActive();
   if (pActiveChild != NULL
#ifdef WIN32
       && AfxCallWndProc(pActiveChild, pActiveChild->m_hWnd, WM_COMMANDHELP, wParam, lParam) != 0)
#else
       && _AfxCallWndProc(pActiveChild, pActiveChild->m_hWnd, WM_COMMANDHELP, wParam, lParam) != 0)
#endif
   {
      // handled by child
      return TRUE;
   }

   if (lParam == 0)
   {
      if (IsTracking())
      {
         lParam = HID_BASE_COMMAND+m_nIDTracking;
      }
      else
      {
         lParam = HID_BASE_RESOURCE+m_nIDHelp;
      }
   }
   if (lParam != 0)
   {
      AfxGetApp()->WinHelp(lParam);
      return TRUE;
   }
   return FALSE;
}

//#ifdef WIN32
#define DDE_WAIT_TIMER     4321

BOOL CMainFrame::WaitForDDETransfer(void)
{
// We only wait for 5 seconds for this.

   SetTimer(DDE_WAIT_TIMER, 5*1000, NULL);
   m_fWaitingForDDE = TRUE;

   return TRUE;
}

LRESULT CMainFrame::OnDDEInitiate(WPARAM wParam, LPARAM lParam)
{
   if (m_fWaitingForDDE)
   {
   /* We are in a DDE session now. We don't need to wait anymore. */
      m_fWaitingForDDE = FALSE;
      KillTimer(DDE_WAIT_TIMER);
   }

   return CMDIFrameWnd::OnDDEInitiate(wParam, lParam);
}

LRESULT CMainFrame::OnDDEExecute(WPARAM wParam, LPARAM lParam)
{
   return CMDIFrameWnd::OnDDEExecute(wParam, lParam);
}

LRESULT CMainFrame::OnDDETerminate(WPARAM wParam, LPARAM lParam)
{
   LRESULT lResult = CMDIFrameWnd::OnDDETerminate(wParam, lParam);

   if (!GET_PMWAPP()->m_fInitialized)
   {
		// Continue on. Do not show the hub.
      if (!GET_PMWAPP()->FinishInitialization(FALSE))
      {
      // Could not initialize. App will not continue.
         PostQuitMessage(0);
      }
   }

   return lResult;
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
   switch (nIDEvent)
   {
      case DDE_WAIT_TIMER:
      {
      // Make really sure the timer is killed.
         KillTimer(DDE_WAIT_TIMER);

      // If we were waiting for DDE, we no longer are.
         if (m_fWaitingForDDE)
         {
            m_fWaitingForDDE = FALSE;
         /*
         // We timed out waiting for DDE transaction.
         */
            if (!GET_PMWAPP()->m_fInitialized)
            {
            /* Continue as if none of this happened. */
               if (!GET_PMWAPP()->FinishInitialization())
               {
                  // Could not initialize. App will not continue.
                  PostQuitMessage(0);
               }
            }
         }
         break;
      }
      default:
      {
         CMDIFrameWnd::OnTimer(nIDEvent);
         break;
      }
   }
}

BOOL CMainFrame::DoUpgrade(void)
{
	BOOL fUpgraded = FALSE;
	CUpgradeDLL Dll;

   if (Dll.Startup())
   {
      if (Dll.Upgrade() == UPGRESULT_NowUpgraded)
      {
      // Set the new configuration in the installation data.

         int nNewConfiguration;

         // If American Greetings Configuration, set new configuration
         // based on product level
         // If PrintMaster configuration, set new configuration to
         // Publishing Suite
         switch(GetConfiguration()->ProductLevel())
         {
            case CPrintMasterConfiguration::PrintStandard:
               nNewConfiguration = CPrintMasterConfiguration::PrintPremium;
               break;
            case CPrintMasterConfiguration::CraftStandard:
               nNewConfiguration = CPrintMasterConfiguration::CraftDeluxe;
               break;
// Remove this #if block when other products are upgradable
#if 0
            case CPrintMasterConfiguration::SpiritStandard:
               nNewConfiguration = CPrintMasterConfiguration::SpiritPremium;
               break;
#endif
            default:
               nNewConfiguration = CPrintMasterConfiguration::Silver;	//mikeyT
               break;
         }
      // Set the new configuration in the installation data.

         GET_PMWAPP()->m_installation_info.nCurrentConfiguration = nNewConfiguration;
         if (!GET_PMWAPP()->m_installation_info.Put())
         {
            // Unable to record upgrade information in PMINST.DAT
            AfxMessageBox(GET_PMWAPP()->GetResourceStringPointer(IDS_UPGRADE_NO_REC));
         }

         fUpgraded = TRUE;

 
      // Create the new configuration.


         CPrintMasterConfiguration::Create(nNewConfiguration);

 
      // Do anything necessary to make us a new program.
     

         GET_PMWAPP()->ChangedConfiguration();
      }
   }
   else
   {
   
   // No instant upgrade DLL. Do it the hard way.
   // Attempt to launch the Product Catalog so they can pay full price.
   
      DoProductCatalog();
   }
   return fUpgraded;

// mikeyT - used for testing of differing configurations
// will remove completely once utility has been created.
// to use: comment out the above code in this function...
// un-comment the code below...
/*
	int nNewConfiguration;
	BOOL fUpgraded = FALSE;
	nNewConfiguration = CPrintMasterConfiguration::Gold;

         GET_PMWAPP()->m_installation_info.nCurrentConfiguration = nNewConfiguration;
         if (!GET_PMWAPP()->m_installation_info.Put())
         {
            // Unable to record upgrade information in PMINST.DAT
            AfxMessageBox(GET_PMWAPP()->GetResourceStringPointer(IDS_UPGRADE_NO_REC));
         }

         fUpgraded = TRUE;

 
      // Create the new configuration.


         CPrintMasterConfiguration::Create(nNewConfiguration);

 
      // Do anything necessary to make us a new program.
     

         GET_PMWAPP()->ChangedConfiguration();

		 return fUpgraded;
*/

}

BOOL CMainFrame::DoProductCatalog(void)
{
   ASSERT(FALSE);
#if 0
   LPCSTR pName;

   if (GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::Demo
         && !((CDemoConfiguration*)GetConfiguration())->CanInstantUpgrade())
   {
      // Launch with the intention of buying PrintMaster.
      pName = "msrun LaunchProdCatPMW";
   }
   else
   {
      // Launch in a generic fashion.
      pName = "msrun LaunchProdCat";
   }

   return WinExec(GetGlobalPathManager()->ExpandPath(pName), SW_SHOW) > 31;
#endif
   return FALSE;
}

void CMainFrame::OnEventReminder(void)
{
   if (GetConfiguration()->SupportsEventReminder())
   {
		GET_PMWAPP()->ShowHelpfulHint("Event Reminder", this, TRUE);
		CString csFullPath;
      CString csDirectory;
      TRY
      {
         csFullPath = GetGlobalPathManager()->ExpandPath("PMRemind");
         Util::SplitPath(csFullPath, &csDirectory, NULL);
         Util::RemoveBackslashFromPath(csDirectory);
         ShellExecute(
            NULL,
            NULL,
            csFullPath,
            NULL,
            csDirectory,
            SW_SHOWNORMAL);
      }
      END_TRY
   }
}

void CMainFrame::OnUpdateEventReminder(CCmdUI* pCmdUI) 
{
	CString csMenuText;
   LoadConfigurationString(IDS_REMINDER_MENU, csMenuText);
	pCmdUI->SetText(csMenuText);	
}

void CMainFrame::OnFileUpgrade() 
{
   if (GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::Demo
         && !((CDemoConfiguration*)GetConfiguration())->CanInstantUpgrade())
   {
      // The demo version allows the user to upgrade by going
      // to the product catalog (aka Discount CD Catalog).
      DoUpgrade();
   }
   else
   {
      // Let the user know how great the Publishing Suite is
      // before going to the order form.
      CNeedToUpgradeDialog Dialog(NULL, NULL);

      if (Dialog.DoModal() == IDOK)
      {
         DoUpgrade();
      }
   }
}

void CMainFrame::OnUpdateFileUpgrade(CCmdUI* pCmdUI) 
{
   if (GetConfiguration() != NULL )
	{
		if(!(GetConfiguration()->SupportsUpgrading()
            && GetConfiguration()->CanInstantUpgrade()))
		{
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
			Util::RemoveExtraMenuSeparators(pCmdUI->m_pMenu);
		}
		else if(GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::PrintStandard )
		{
			CString csAgUpgradeText;
			LoadConfigurationString(IDS_AGPRINT_UPGRADE_TEXT, csAgUpgradeText);
			//we failed to load the text for the menu item so let's just delete it.
			if(csAgUpgradeText.IsEmpty())
			{
				pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
				Util::RemoveExtraMenuSeparators(pCmdUI->m_pMenu);
			}
			else
				pCmdUI->SetText(csAgUpgradeText);
		}
		else if(GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::CraftStandard )
		{
			CString csAgUpgradeText;
			LoadConfigurationString(IDS_AGCRAFT_UPGRADE_TEXT, csAgUpgradeText);
			//we failed to load the text for the menu item so let's just delete it.
			if(csAgUpgradeText.IsEmpty())
			{
				pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
				Util::RemoveExtraMenuSeparators(pCmdUI->m_pMenu);
			}
			else
				pCmdUI->SetText(csAgUpgradeText);
		}
	}
}

void CMainFrame::OnViewAddressBook()
{
/*
// Only put up the address book if the configuration supports it.
*/
   if (GetConfiguration()->SupportsAddressBook())
   {
      CPmwDoc* pDoc = (CPmwDoc*)(GetActiveFrame()->GetActiveDocument());
      if (pDoc != NULL)
      {
         pDoc->EditNameList(FALSE);
      }
      else
      {
         CAddressBook::Run(NULL, NULL, FALSE, TRUE, ADDRESS_BOOK_COLOR);
      }
   }
}

void CMainFrame::OnViewSenderInformation()
{
/*
// Only put up the address book if the configuration supports it.
*/
   if (GetConfiguration()->SupportsAddressBook())
   {
      CPmwDoc* pDoc = (CPmwDoc*)(GetActiveFrame()->GetActiveDocument());
      if (pDoc != NULL)
      {
         pDoc->EditSender();
      }
      else
      {
         CSelectSenderDialog::Run(this);
      }
   }
}

void CMainFrame::OnUpdateIndicator(CCmdUI* pCmdUI)
{
   // We do not have any text to show at this time.
   pCmdUI->SetText(" ");
   pCmdUI->Enable(FALSE);
}

void
CMainFrame::OnInternet()
{
	// Helpful Hint
	if (!GET_PMWAPP()->ShowHelpfulHint("Internet button", NULL, TRUE))
	{
		return;
	}

   CString csUrl;
   csUrl = GetConfiguration()->GetProductURL();

   if (DoHTML (m_hWnd, csUrl, TRUE) == TRUE)
      return;

   CDialogURL  dlgSiteDesc(this);
   CString csDesc;
   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
      csDesc.LoadString(IDS_AG_WEB_SITE_DESC);
   else
      csDesc.LoadString(IDS_PRINTMASTER_WEB_SITE_DESC);
   dlgSiteDesc.SetSiteDescription(csDesc);
   dlgSiteDesc.DoModal ();
}

void
CMainFrame::OnUpdate3DPlanet(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetConfiguration()->SupportsExtraContent());
}

void
CMainFrame::On3DPlanet()
{
	LPCSTR pExtraContentName = GetConfiguration()->GetExtraContentName();
	// The interface should not let us get here if it is not supported
	ASSERT(pExtraContentName != NULL);
	// NULL if unsupported
	if (pExtraContentName != NULL)
	{
		IPathManager*	pPathManager = GetGlobalPathManager();
		// Assume the files are on the user's hard drive
		CString	Path3D = pPathManager->GetWorkingDirectory();
		Path3D += "3DPlanet\\";
		Path3D += pExtraContentName;
		
		BOOL fOldPrompt = pPathManager->SetPromptForContent(FALSE);
		if (pPathManager->FileExists(Path3D, TRUE))
		{
			DoHTML (m_hWnd, Path3D, TRUE);
		}
		else
		{
			Path3D = "[[H]]\\";
			Path3D += "3DPlanet\\";
			Path3D += pExtraContentName;
			// Look for the files on the "home" CD
			pPathManager->SetPromptForContent(TRUE);
			BOOL Exists = pPathManager->FileExists(Path3D, TRUE);
			pPathManager->SetPromptForContent(FALSE);
			// Get "real" path name
			CString csPath = pPathManager->LocatePath(Path3D);
			if (Exists)
				DoHTML (m_hWnd, csPath, TRUE);			
			else
			{
				Path3D.Format(IDS_ErrMissingDLL, pExtraContentName);
				AfxMessageBox(Path3D);
			}
		}
		
		pPathManager->SetPromptForContent(fOldPrompt);
	}
}

void 
CMainFrame::OnPhotoOrganizer()
{
	CWaitCursor	WaitCursor;

	LPCSTR			cp = "pspo.pi";	// Actually an 'INI' file
	IPathManager*	pPathManager = GetGlobalPathManager();
	CString			Dir = pPathManager->GetWorkingDirectory();
	CString			Photo = Dir;
	
	Photo += cp;
	// Build registry thing
	CString strRoot = "Software\\";
	strRoot += GetConfiguration()->GetPrintShopFamilyRegistryPath();
	strRoot += "\\";

	CIniFile	IniFile(Photo);
	if (IniFile.AppendNamedApplicationRegistryPath(strRoot))
	{
		CString	EXEName;
		if (Util::GetRegistryString(HKEY_LOCAL_MACHINE, strRoot, "Exe", EXEName))
 		{														  
         HINSTANCE hi = ShellExecute(NULL, NULL, EXEName, NULL, Dir, SW_SHOWNORMAL);
			if (hi <= (HINSTANCE)32)
			{
				Photo.Format(IDS_ErrMissingDLL, EXEName);
				AfxMessageBox(Photo);
			}
		}
		else
			AfxMessageBox("Can't find registry key");
	}
	else
	{
		Photo.Format(IDS_ErrMissingDLL, cp);
		AfxMessageBox(Photo);
	}
}

void 
CMainFrame::OnUpdatePhotoOrganizer(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetConfiguration()->SupportsPhotoOrganizer());
}

void
CMainFrame::OnAddressBook ()
{
   OnViewAddressBook ();
}

void CMainFrame::OnUpdateHubNew(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GET_PMWAPP()->m_pMainWnd != NULL);
}
// The CWndHub started in OnHubNew() (below) will send this message 
// to find out what to do when a button is pressed. 
// Returning 0 will cause it "broadcast" a command message representing the button event
// Returning non 0 will cause it to ternimate and return that value
// to the caller of RunHubWindowModal()
LRESULT 
CMainFrame::OnHubQueryAction(WPARAM wParam, LPARAM lParam)
{
	ASSERT((CHubQueryInfo*)lParam != NULL);
	if ((CHubQueryInfo*)lParam != NULL)
	{
		ASSERT(((CHubQueryInfo*)lParam)->IsAuthentic() == TRUE);
		if (((CHubQueryInfo*)lParam)->IsAuthentic() == TRUE)
		{
			if (
					(((CHubQueryInfo*)lParam)->Input() == IDC_INTERNET)
					||
					(((CHubQueryInfo*)lParam)->Input() == ID_3DPLANET)
				)
			{
				((CHubQueryInfo*)lParam)->RetValSet(0);
			}
			else
			{
				((CHubQueryInfo*)lParam)->RetValSet(((CHubQueryInfo*)lParam)->Input());
			}
		}
	}

	return 0;
}

void CMainFrame::OnHubNew()
{
	CPmwApp *pApp = GET_PMWAPP();
	//this flags for AG fulfilling personal delivery.  If we are doing this
	//we don't ever want the hub coming up.
	if(pApp->m_bAgRemotePrint || pApp->m_bPrintProjects)
		return;

	if ((pApp->PreventHub() == FALSE) && (IsWindowVisible()))
	{
		int	cmd;
		BOOL	Done;
		for (Done = FALSE; Done != TRUE;)
		{
			CWndHub	WndHub(GetConfiguration()->GetHubDLLName());
			if ((GetConfiguration()->SupportsRegistrationBonus() == FALSE) || (pApp->user_is_registered() == TRUE))
				WndHub.SetSkipButton (ID_FILE_FREE_FONTS);
			if (WndHub.Create (this) != TRUE)
			{
				// Hub failed to create
				ASSERT(FALSE);
				Done = TRUE;
				cmd = -1;
			}
			else
			{
				/*
				// Set / reset button for new content
				if (GetGlobalContentManager()->CheckForNewInternetContent() == TRUE)
				{
					WndHub.SubstituteBitmap(IDB_HUB_ARTANDMORE_UP, IDB_HUB_NEWANDMORE_UP);			
					WndHub.SubstituteBitmap(IDB_HUB_ARTANDMORE_DOWN, IDB_HUB_NEWANDMORE_DOWN);			
				}
				else
				{
					WndHub.SubstituteBitmap(IDB_HUB_NEWANDMORE_UP, IDB_HUB_ARTANDMORE_UP);			
					WndHub.SubstituteBitmap(IDB_HUB_NEWANDMORE_DOWN, IDB_HUB_ARTANDMORE_DOWN);			
				}
				*/
				cmd = WndHub.StartModalLoop();
				WndHub.DestroyWindow ();
			}

			switch(cmd)
			{
				case IDC_WITH_HELP:
				if (DoWithHelp() == TRUE)
					Done = TRUE;
				break;

				case ID_FILE_NEW:
				if (pApp->CanOpenAnotherDocument())
				{
					if (pApp->new_document() == TRUE)
						Done = TRUE;
				}
				break;
				
				case IDC_ADDRESS:
				OnAddressBook();
				break;

				case IDC_INTERNET:
				OnInternet();
				break;

				case IDC_READY_MADE:
				if (DoBrowserOrArtStore(Project) == TRUE)	// Something selected from a browser?
					Done = TRUE;
				break;

				case IDC_HUB_ART_GALLERY:
				if (DoBrowserOrArtStore(Art) == TRUE)		// Something selected from a browser?
					Done = TRUE;	
				break;

				case IDC_ART_AND_MORE_STORE:
				if (DoBrowserOrArtStore() == TRUE)			// Something selected from a browser?
					Done = TRUE;
				break;

				case IDC_PARTY_SETS:
				{
					if(DoMatchingSets() == TRUE)
						Done = TRUE;
				break;
				}

				case IDCANCEL:			// The ESC and ALT / F4 do this
				case IDC_HUB_EXIT:
				Done = TRUE;
				break;

				default:
				// The hub terminated abnormally
				ASSERT(0);
				Done = TRUE;
				break;
			}
		}
	}
}

//bring up the matching sets dlg
BOOL CMainFrame::DoMatchingSets()
{
	CMatchingSetsDlg dlg;
	if(dlg.DoModal() == TRUE)
	{
		CStringArray& csaPathArray = dlg.GetProjArray();
		int nCount = csaPathArray.GetSize();
		if(nCount == 0)
			return FALSE;

		CString csPath;
		for(int i = 0; i < nCount; i++)
		{
			csPath = csaPathArray[i];
			GET_PMWAPP()->OpenDocumentFile(csPath);		
		}
		return TRUE;
	}
	return FALSE;
}

// Bring up the Art Store, perhaps a browser, perhaps both repeatedly 
BOOL	
CMainFrame::DoBrowserOrArtStore(enum CMainFrame::UseBrowser e/*Store*/, CPmwDoc *pArtDoc /*NULL*/)
{
	BOOL					retval = FALSE;
	int               nDialogRetVal;
	CArtStoreDialog   *pstoreDialog;
	// Which browser?
	if (e == Art)
	{
		CArtBrowserDialog Dialog (NULL);
		Dialog.m_pDoc = pArtDoc;
		nDialogRetVal = Dialog.DoModal();
	}
	else
	if (e == Project)
	{
		CProjectBrowserDialog Dialog(NULL);
		nDialogRetVal = Dialog.DoModal();
	}
	else
	if (e == Store)
	{
		nDialogRetVal = ID_ONLINE_ART;	// No browser; simulate user selecting art store
	}
	else
	{
		ASSERT(FALSE);
		return FALSE;
	}
	// Did the user select art store from the browser?
	while (nDialogRetVal == ID_ONLINE_ART)
	{
		nDialogRetVal = -1;

		TRY
		{
			pstoreDialog = new CArtStoreDialog;
			// Did the user select a browser from the art store?
			if (pstoreDialog->DoModal() == IDC_ART_STORE_BROWSER_PREVIEW)
			{
				CPMWCollection    *pCollection = pstoreDialog->GetCollectionToPreview();
 
				if (pCollection != NULL)
				{
					TRACE("Collection %s\n", (LPCSTR)pCollection->GetFriendlyName());

					CString csStartupName;
					csStartupName = pCollection->GetSuperCollection();
					if (!csStartupName.IsEmpty())
					{
						csStartupName += "/";
					}
					csStartupName += pCollection->GetFriendlyName();
					
					if (pCollection->GetType() == CPMWCollection::typeProjects)
					{
						CProjectBrowserDialog Dialog;
						Dialog.SetStartupCollection(csStartupName);

						nDialogRetVal = Dialog.DoModal();
					}
					else
					{
						CArtBrowserDialog Dialog;
						Dialog.SetStartupCollection(csStartupName);
						Dialog.m_pDoc = pArtDoc;

						nDialogRetVal = Dialog.DoModal();
					}
				}
				else
					ASSERT(FALSE);
			}
			
			delete pstoreDialog;
		}
		END_TRY
	}
	//if called from OnHubNew(), TRUE terminates it
	return (nDialogRetVal == IDOK) ? TRUE : FALSE;
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
   CMDIFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void
CMainFrame::OnWithHelp ()
{
	DoWithHelp();
}

BOOL
CMainFrame::DoWithHelp()
{
	BOOL	ret = FALSE;
	if (GET_PMWAPP()->CanOpenAnotherDocument())
	{
		CWHelpWizPropertySheet HelpWizard("");

		if (HelpWizard.InitializedSuccessfully())
		{
			// open the workspace
			if (HelpWizard.DoModal() == ID_WIZFINISH)
			{
				CString projName = HelpWizard.GetTmpProject();
				if(!projName.IsEmpty())
				{
					// Close initial poster	
					GET_PMWAPP()->KillFirstPoster();

					// Open as a "With Help" document.
					WORD wOldFlags = CPmwDoc::m_wGlobalDocumentFlags;
					CPmwDoc::m_wGlobalDocumentFlags = HelpWizard.GetDocumentFlags();
					CPmwDoc* pDoc = (CPmwDoc*)GET_PMWAPP()->OpenDocumentFile(projName);
					CPmwDoc::m_wGlobalDocumentFlags = wOldFlags;

					// remove temp file
					remove(projName);
					
					ret = TRUE;
				}
			}
		}
	}

	return ret;
}

void CMainFrame::OnYourProjects ()
{
   CProjectBrowserDialog Dialog(NULL);
   CString csYourOwn;
   csYourOwn.LoadString(IDS_YOUR_OWN);
   Dialog.SetStartupCategory(csYourOwn);
   Dialog.DoModal();
}

void CMainFrame::OnExitAlbum ()
{
	RequestHubWindow();
}

BOOL CMainFrame::DestroyWindow()
{
	//destroy the help window.  This makes it so the focus is returned 
	//to to the proper app on exit.  
	m_fAppClosingDown = TRUE;

	COleDataSource::FlushClipboard();

	m_wndHelpWindow.DestroyWindow();

	return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::OnClose()
{
   CExitDialog Dialog;
   if (Dialog.DoModal () == IDOK)
   {
		// This code needs some "implementation-dependent" methods of the
		// connection manager, so we grab the underlying implementation class.
		// Ideally, this need would go away (move it to PMW.CPP somehow?)
      CConnectionManager* pConnMgr = GET_PMWAPP()->GetHiddenConnectionManager();

		if(pConnMgr != NULL && pConnMgr->GetSafeHwnd())
		{
			pConnMgr->HangUp();
			pConnMgr->DestroyWindow();
		}

		SaveBarState("BarState");

		CFrameWnd::OnClose();
   }
}

LRESULT 
CMainFrame::OnViewsChanged(WPARAM wParam, LPARAM lParam)
{
   if (wParam == TRUE)
   {
		// Redo the layout based on the new current view. This usually
		// just changes the zoom and page gadgets that are showing.
		RecalcLayout();
   }
   else
   {
		RequestHubWindow ();   
   }

   return 0;
}

int CMainFrame::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
#if 0
   TRACE("OnMouseActivate(%08lx, %d, %x), %08lx\n", pDesktopWnd, nHitTest, message, this);
#endif

   GET_PMWAPP()->RemoveHelpfulHint();

   extern BOOL PopupIsActive(void);
   if (nHitTest == HTCLIENT && message == WM_LBUTTONDOWN)
   {
      if (PopupIsActive())
      {
         return MA_ACTIVATEANDEAT;
      }
   }

   return INHERITED::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
   GET_PMWAPP()->RemoveHelpfulHint();
	
	CMDIFrameWnd::OnNcLButtonDown(nHitTest, point);
}

LRESULT CMainFrame::OnNfxTermination(WPARAM wParam, LPARAM lParam)
{
   CCartoonOMatic::GotTerminationMessage();
   return 0;
}

void CMainFrame::OnUpdateBuildBrowser(CCmdUI* pCmdUI)
{
   if (!GET_PMWAPP()->OurGetProfileInt("Debug", "AllowBuild", FALSE))
   {
      pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
   }
}

void CMainFrame::RequestHubWindow (void)
{
	if (IsWindowVisible())
	{
		PostMessage(WM_COMMAND, ID_HUB_NEW);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Browser database build section.

#ifdef WIN32
#include "collbld.h"
#endif

ERRORCODE CMainFrame::GetBuildInfo(
   LPCSTR szSourceEntry,
   LPCSTR szDestEntry,
   CString *pcsINIFile,
   CString *pcsFileTitle,
   CString *pcsSourceFile,
   CString *pcsDestDir,
   BOOL    *pbRunSilent)
   {
      CFileDialog    cfdSourceFile(TRUE, ".txt", "*.txt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Text Files (*.txt)|All Files (*.*)|*.*||");
      CIniFile       IniFile(GET_PMWAPP()->m_pszProfileName);
      CString        csSourceFile, csPath, csFileTitle, csINIFile;
      CChooseDirectoryDialog  dlgDir;
#ifdef SELECT_LAST_BUILD_FILE
      char           szFileBuffer[_MAX_PATH];
#endif

      ASSERT(pcsINIFile);
      ASSERT(pcsFileTitle);
      ASSERT(pcsSourceFile);
      ASSERT(pcsDestDir);
      if(pcsINIFile == NULL || pcsFileTitle == NULL || pcsSourceFile == NULL ||
         pcsDestDir == NULL)
            return ERRORCODE_BadParameter;

      csSourceFile = IniFile.GetString("DEBUG", szSourceEntry);
      if(!csSourceFile.IsEmpty())
      {
#ifdef SELECT_LAST_BUILD_FILE
         strncpy(szFileBuffer, csSourceFile.GetBuffer(1), _MAX_PATH);
         szFileBuffer[sizeof(szFileBuffer)-1] = NULL;
         cfdSourceFile.m_ofn.lpstrFile = szFileBuffer;
         cfdSourceFile.m_ofn.nMaxFile = sizeof(szFileBuffer);
#endif
         Util::SplitPath(csSourceFile, &csPath, NULL);
         cfdSourceFile.m_ofn.lpstrInitialDir = csPath;
      }
      if(cfdSourceFile.DoModal() != IDOK)
         return ERRORCODE_Abort;

      csSourceFile = cfdSourceFile.GetPathName();
      csFileTitle = cfdSourceFile.GetFileTitle();
      Util::SplitPath(csSourceFile, &csPath, NULL);
      IniFile.WriteString("DEBUG", szSourceEntry, csSourceFile);

      // Build INI File Name
      csINIFile = csPath;
      csINIFile += csFileTitle;
      csINIFile += ".INI";

      // Ask for Destination Directory
      dlgDir.m_ofn.lpstrInitialDir = dlgDir.m_csDirectory = IniFile.GetString("DEBUG", szDestEntry, csPath);

      // Bail if directory doesn't exist or is name is empty
      if( dlgDir.GetDirectory(CChooseDirectoryDialog::OPTION_no_exists_check |
   		   CChooseDirectoryDialog::OPTION_expand) == 0 ||
	      dlgDir.m_csDirectory.IsEmpty())
            return ERRORCODE_Abort;

      Util::MakeDirectory(dlgDir.m_csDirectory);
      IniFile.WriteString("DEBUG", szDestEntry, dlgDir.m_csDirectory);

#ifdef PROMPT_FOR_SILENT_MODE
      if(pbRunSilent)
         {
            int nMsgBoxRetVal = AfxMessageBox(
               "Run Builder in silent mode?", MB_YESNO);
            if(nMsgBoxRetVal == IDYES)
               *pbRunSilent = TRUE;
            else
               *pbRunSilent = FALSE;
         }
#else
     *pbRunSilent = TRUE;  // Always run silent
#endif

      // Update callers strings
      *pcsINIFile = csINIFile;
      *pcsFileTitle = csFileTitle;
      *pcsSourceFile = csSourceFile;
      *pcsDestDir = dlgDir.m_csDirectory;

      return ERRORCODE_None;
   }

void CMainFrame::OnBuildProjects()
{
	IPathManager*	pPathManager = GetGlobalPathManager();
   CString        csINIFile, csFileTitle, csSourceFile, csDestDir;
   BOOL           bRunSilent;
   ERRORCODE      errorcode;
   int            nRetCode;
   CCollectionSelectionDialog dlgCollectionSelection;


   errorcode = GetBuildInfo("ProjBuildSource", "ProjBuildDest",
      &csINIFile, &csFileTitle, &csSourceFile, &csDestDir, &bRunSilent);
   if(errorcode != ERRORCODE_None)
      return;

   if(dlgCollectionSelection.DoModal() != IDOK)
      return;

	BOOL bOnline = FALSE;
   nRetCode = AfxMessageBox("Build project collection for Online Gallery?",
      MB_YESNOCANCEL|MB_DEFBUTTON2);
   if(nRetCode == IDCANCEL)
      return;
   if(nRetCode == IDYES)
	{
		bOnline = TRUE;
		if (AfxMessageBox(
				"The collection files will be "
				"placed in the ONLINE subdirectory. The compressed data files "
				"will be placed in the ONLINE\\FTP subdirectory. The SQL script files "
				"will be placed in the ONLINE\\SQL subdirectory.",
				MB_OKCANCEL) == IDCANCEL)
		{
			return;
		}
	}

   nRetCode = AfxMessageBox("Fixup project files?", MB_YESNOCANCEL);
   if(nRetCode == IDCANCEL)
      return;
   if(nRetCode == IDYES)
      {
         BOOL bUnEmbedGraphics = FALSE;
			BOOL bEmbedGraphics = FALSE;

         nRetCode = AfxMessageBox("Unembed graphics?", MB_YESNOCANCEL);
         if(nRetCode == IDCANCEL)
			{
            return;
			}
         if(nRetCode == IDYES)
			{
            bUnEmbedGraphics = TRUE;
			}
         else
			{
            bUnEmbedGraphics = FALSE;
				nRetCode = AfxMessageBox("Embed ALL graphics?", MB_YESNOCANCEL);
				if(nRetCode == IDCANCEL)
				{
					return;
				}
				if(nRetCode == IDYES)
				{
					bEmbedGraphics = TRUE;
				}
				else
				{
					bEmbedGraphics = FALSE;
				}
			}

         CDocDump    docDump((CPmwDoc *)GetActiveFrame()->GetActiveDocument(), pPathManager);
         docDump.BatchFix(csSourceFile, TRUE, bUnEmbedGraphics, bEmbedGraphics);
      }

   CCollectionBuilder Builder(pPathManager,
										CString(pPathManager->ExpandPath(csINIFile)),
										CString(csFileTitle),
										CString(pPathManager->ExpandPath(csSourceFile)),
										&CString(pPathManager->ExpandPath(csDestDir)),
										bOnline, TRUE);

   Builder.DoBuild(AfxGetMainWnd(), FALSE, bRunSilent);
}

void CMainFrame::OnBuildPictures()
{
	ProcessPictures(FALSE);
}

void CMainFrame::OnPreprocessPictures()
{
	ProcessPictures(TRUE);
}

void CMainFrame::ProcessPictures(BOOL bValidateOnly)
{
	IPathManager*	pPathManager = GetGlobalPathManager();
   CString        csINIFile, csFileTitle, csSourceFile, csDestDir;
   BOOL           bRunSilent;
   int            nRetCode;
   ERRORCODE      errorcode;

   errorcode = GetBuildInfo("PictBuildSource", "PictBuildDest",
      &csINIFile, &csFileTitle, &csSourceFile, &csDestDir, &bRunSilent);
   if(errorcode != ERRORCODE_None)
      return;

	BOOL bOnline = FALSE;
	BOOL bCompressOnlineDataFiles = TRUE;
   nRetCode = AfxMessageBox("Build picture collection and thumbnail files for Online Gallery?",
      MB_YESNOCANCEL|MB_DEFBUTTON2);
   if(nRetCode == IDCANCEL)
      return;
   if(nRetCode == IDYES)
	{
		bOnline = TRUE;

	   nRetCode = AfxMessageBox("Compress online data files?", MB_YESNOCANCEL|MB_DEFBUTTON1);
		if(nRetCode == IDCANCEL)
			return;
	   if(nRetCode == IDNO)
			bCompressOnlineDataFiles = FALSE;

		if (AfxMessageBox(
				"The collection files will be "
				"placed in the ONLINE subdirectory. The data files "
				"will be placed in the ONLINE\\FTP subdirectory. The SQL script files "
				"will be placed in the ONLINE\\SQL subdirectory. The Online Thumbnail "
				"files will be placed in the ONLINE\\FTP\\Thumbnails subdirectory.",
				MB_OKCANCEL) == IDCANCEL)
		{
			return;
		}
	}

   CCollectionBuilder Builder(pPathManager,
										CString(pPathManager->ExpandPath(csINIFile)),
										CString(csFileTitle),
										CString(pPathManager->ExpandPath(csSourceFile)),
										&CString(pPathManager->ExpandPath(csDestDir)),
										bOnline, bCompressOnlineDataFiles);

   Builder.DoBuild(AfxGetMainWnd(), bValidateOnly, bRunSilent);
}

void CMainFrame::OnBuildSentiments()
{
	IPathManager*	pPathManager = GetGlobalPathManager();
   CString        csINIFile, csFileTitle, csSourceFile, csDestDir;
   BOOL           bRunSilent;
   ERRORCODE      errorcode;

   errorcode = GetBuildInfo("SentBuildSource", "SentBuildDest",
      &csINIFile, &csFileTitle, &csSourceFile, &csDestDir, &bRunSilent);
   if(errorcode != ERRORCODE_None)
      return;

   CCollectionBuilder Builder(pPathManager,
										CString(pPathManager->ExpandPath(csINIFile)),
										CString(csFileTitle),
										CString(pPathManager->ExpandPath(csSourceFile)),
										&CString(pPathManager->ExpandPath(csDestDir)),
										FALSE, /* online sentiments are the same as normal sentiment collections */
										TRUE);

   Builder.DoBuild(AfxGetMainWnd(), FALSE, bRunSilent);
}

void CMainFrame::OnAddonInstall()
{
   DoAddonInstall (this);
}

void CMainFrame::OnAddonViewArt()
{
   AfxMessageBox ("View Art Pressed");
}

void CMainFrame::OnAddonViewProj()
{
   AfxMessageBox ("View Projects Pressed");
}

void CMainFrame::OnAddonViewHTML()
{
   LPCSTR   HTMLName = GetConfiguration()->GetHTMLName ();
   if (HTMLName [0] == 0)
   {
#ifdef _DEBUG
      AfxMessageBox ("No HTML file found in ADDON.INI");
#endif      
      return;
   }

   CString csUrl = GetGlobalPathManager()->ExpandPath (HTMLName);
   
   if (DoHTML (m_hWnd, csUrl) == TRUE)
      return;

   CDialogURL  Dlg (this);
   Dlg.DoModal ();
}

void CMainFrame::OnDesktopArtStore()
{
	CPmwDoc *pArtDoc = NULL;
	
	CFrameWnd *pFrame = GetActiveFrame();
	if (pFrame)
		pArtDoc = (CPmwDoc*)(pFrame->GetActiveDocument());

	DoBrowserOrArtStore(Store, pArtDoc);
}

void CMainFrame::OnUpdateDesktopArtStore(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	if (GetGlobalContentManager()->CheckForNewInternetContent() == TRUE)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CMainFrame::OnParentNotify(UINT message, LPARAM lParam) 
{
	CMDIFrameWnd::OnParentNotify(message, lParam);

	if (GET_PMWAPP()->m_fSoundEffectsState == FALSE)
		return;

	if (WM_LBUTTONDOWN == LOWORD (message))
	{
		CPoint   Point (lParam);

		CWnd* pChildWnd = ChildWindowFromPoint(Point, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED);

		// No sound effects on docked Help window
		if (pChildWnd == m_wndHelpWindow.m_pDockBar)
			 return;

		HWND hWnd = pChildWnd->GetSafeHwnd();
		if (hWnd != NULL && hWnd != m_hWnd)
		{
			::ClientToScreen (m_hWnd, &Point);
			::ScreenToClient (hWnd, &Point);
			DWORD d = MAKELONG(Point.x, Point.y);

			UINT  IDButton;
			IDButton = (UINT)::SendMessage (hWnd, WM_HELPHITTEST, 0, d);
			IDButton = LOWORD(IDButton);
      
			if ((IDButton != 0) && (IDButton != (UINT)(SHORT)-1))
			{
				UINT IDWindow = _AfxGetDlgCtrlID (hWnd);
				if (IDButton != IDWindow)
				{
					GET_PMWAPP()->m_MciWnd.PlaySoundFX (IDButton, IDWindow);
				}
			}
		}
	}
}

void CMainFrame::OnTutorialChoice(UINT nID)
{
   int index = nID - ID_TUTORIAL_FIRST;
   int nTutorialID = m_pTutorialList->GetIDFromIndex(index);
   ShowTutorial(nTutorialID);
}

void CMainFrame::OnInitMenu(CMenu* pMenu)
{
	CMDIFrameWnd::OnInitMenu(pMenu);
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// gray out sub menus that have all grayed out items
	SetSubMenusState(pPopupMenu, nIndex);

   UINT uItems = pPopupMenu->GetMenuItemCount();
	if (uItems > 0)
	{
		for (UINT uItem = 0; uItem < uItems; uItem++)
		{
		   UINT uID = pPopupMenu->GetMenuItemID(uItem);
			if (uID == ID_TUTORIAL_LIST)
			{
				pPopupMenu->DeleteMenu(uItem, MF_BYPOSITION);

				// create popups of available tutorials
				LPCSTR pszLastType = NULL;
				LPCSTR pszType = NULL;
				LPCSTR pszTitle = NULL;
				CMenu TutorialMenu;
				for (int i = 0; i < m_pTutorialList->GetSize(); i++)
				{
					if (m_pTutorialList->GetInfoFromIndex(i, NULL, &pszType, &pszTitle, NULL))
					{
						if ((pszLastType == NULL) || (strcmp(pszLastType, pszType) != 0))
						{
							TutorialMenu.Detach();
							TutorialMenu.CreatePopupMenu();
							if (TutorialMenu.GetSafeHmenu() != NULL)
							{
								pPopupMenu->InsertMenu(uItem, MF_BYPOSITION|MF_POPUP|MF_ENABLED|MF_STRING, (UINT)(TutorialMenu.GetSafeHmenu()), pszType);
								uItem++;
							}
						}

						pszLastType = pszType;

						if (TutorialMenu.GetSafeHmenu() != NULL)
						{
							TutorialMenu.AppendMenu(MF_STRING|MF_ENABLED, ID_TUTORIAL_FIRST+i, pszTitle);
						}
					}
				}

				TutorialMenu.Detach();

				break;
			}
      }
   }
}

void CMainFrame::OnHelpPreferences() 
{
   CPreferencesSheet::SelectPreferences(1);
}

void CMainFrame::OnHelpRecentHints() 
{
	GET_PMWAPP()->ShowRecentHintsDialog();
}

void CMainFrame::OnHelpEnableHints() 
{
	GET_PMWAPP()->EnableHelpfulHints(!GET_PMWAPP()->HelpfulHintsEnabled());
}

void CMainFrame::OnUpdateHelpEnableHints(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GET_PMWAPP()->HelpfulHintsEnabled());
}

void CMainFrame::OnHelpEnableTips() 
{
	GET_PMWAPP()->EnableDesignTips(!GET_PMWAPP()->DesignTipsEnabled());
}

void CMainFrame::OnUpdateHelpEnableTips(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GET_PMWAPP()->DesignTipsEnabled());
}

LRESULT CMainFrame::OnWakeApp(WPARAM wParam, LPARAM lParam)
{
	GET_PMWAPP()->WakeWindow();
	return 1L;
}


void CMainFrame::OnShowLegals() 
{
   CDisplayTextDialog   dispTextDialog(this);
   CString  csFileName = GetGlobalPathManager()->ExpandPath("legals.rtf");

   // Try to use RTF file if it exists, otherwise use text version
   if(!Util::FileExists(csFileName))
      csFileName = GetGlobalPathManager()->ExpandPath("legals.txt");	
   dispTextDialog.DoModal("Legals", csFileName);
}

void CMainFrame::OnUpdatePersonalDelivery(CCmdUI* pCmdUI) 
{
	if(!GetConfiguration()->SupportsPersonalDelivery())
	{
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);	
	}
}

BOOL CMainFrame::OnDeviceChange(UINT nEventType, DWORD dwData)
{
/////////             detect eject CDROM and Insertion CDROM  //////////
	switch (nEventType)
	{
		case DBT_DEVICEARRIVAL:          //CDROM is in now
		{
			PDEV_BROADCAST_HDR pDeviceChange = (PDEV_BROADCAST_HDR)dwData;
			if (pDeviceChange->dbch_devicetype == DBT_DEVTYP_VOLUME) 
			{
				PDEV_BROADCAST_VOLUME pDeviceBroadcastVolume = (PDEV_BROADCAST_VOLUME)pDeviceChange;
				if (pDeviceBroadcastVolume->dbcv_flags == DBTF_MEDIA) 
				{
//					SendMessageToDescendants(UM_CDROM_DETECTED);
					CWnd* pPopup = GetLastActivePopup();
					if (pPopup != this)
					{
						pPopup->SendMessage(UM_CDROM_DETECTED);
					}
				}
			}
			return TRUE;
		}
		//we may want to handle a cd removal message in the future.
		case DBT_DEVICEREMOVECOMPLETE:   //removing is complete
		{
			break;
		}
		default:
		{
			break;
		}
	}  
	return CMDIFrameWnd::OnDeviceChange(nEventType, dwData);
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	// If we're in a modal state, disable docking of help window
	if (m_wndHelpWindow.GetSafeHwnd() != NULL)
	{
		if (InModalState())
			m_wndHelpWindow.EnableDocking(0);
		else
			m_wndHelpWindow.EnableDocking(CBRS_ALIGN_RIGHT);
	}
}

void CMainFrame::OnTCard(UINT idAction, DWORD dwActionData) 
{
	switch (idAction)
	{
	case HELP_TCARD_DATA:
		if (IS_TUTORIAL_ID(dwActionData))
			ShowTutorial ((int)dwActionData);
		break;

	case IDCLOSE:
	case HELP_TCARD_OTHER_CALLER:
	default:
		break;
	}
}

// This code is largely from MFC.
// WinHelp is overloaded so we can invoke training card help
void CMainFrame::WinHelp(DWORD dwData, UINT nCmd)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT_VALID(pApp);
	ASSERT(pApp->m_pszHelpFilePath != NULL);
	DWORD dwHelpData = dwData;

	CWaitCursor wait;
	if (IsFrameWnd())
	{
		// CFrameWnd windows should be allowed to exit help mode first
		CFrameWnd* pFrameWnd = (CFrameWnd*)this;
		pFrameWnd->ExitHelpMode();
	}

	// cancel any tracking modes
	SendMessage(WM_CANCELMODE);
	SendMessageToDescendants(WM_CANCELMODE, 0, 0, TRUE, TRUE);

	// need to use top level parent (for the case where m_hWnd is in DLL)
	CWnd* pWnd = GetTopLevelParent();
	pWnd->SendMessage(WM_CANCELMODE);
	pWnd->SendMessageToDescendants(WM_CANCELMODE, 0, 0, TRUE, TRUE);

	// attempt to cancel capture
	HWND hWndCapture = ::GetCapture();
	if (hWndCapture != NULL)
		::SendMessage(hWndCapture, WM_CANCELMODE, 0, 0);

	TRACE3("WinHelp: pszHelpFile = '%s', dwData: $%lx, fuCommand: %d.\n",
		pApp->m_pszHelpFilePath, dwData, nCmd);

	switch (nCmd)
	{
		case HELP_CONTEXT:
		{
			CWnd* pLastPopupWnd = GetLastActivePopup();
			if (pLastPopupWnd != this)
			{
				if (!m_wndHelpWindow.IsFloating())
				{
					int nScrWidth = GetSystemMetrics(SM_CXFULLSCREEN);
					int nScrHeight = GetSystemMetrics(SM_CYFULLSCREEN);

					CRect rcPopup;
					pLastPopupWnd->GetWindowRect(rcPopup);
					int nXPos = rcPopup.right;
					if (nXPos + m_wndHelpWindow.m_czFloating.cx > nScrWidth)
						nXPos = nScrWidth - m_wndHelpWindow.m_czFloating.cx;
					int nYPos = (nScrHeight - m_wndHelpWindow.m_czFloating.cy) / 2;
					FloatControlBar(&m_wndHelpWindow, CPoint(nXPos, nYPos));
				}

				// make sure help window is on top and active
				CWnd* pParent = m_wndHelpWindow.GetParent();
				if (pParent)
				{
					CWnd* pGrandParent = pParent->GetParent();
					if (pGrandParent && pGrandParent->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
					{
						pGrandParent->BringWindowToTop();
						pGrandParent->SetActiveWindow();
						pGrandParent->EnableWindow(TRUE);
					}
				}

				// If we're in a modal state, disable docking of help window
				if (InModalState())
					m_wndHelpWindow.EnableDocking(0);

				// KLUDGE: If hub window is up, send the hub ID
				if (pLastPopupWnd->IsKindOf(RUNTIME_CLASS(CWndHub)))
				{
					dwHelpData = ID_HUB_WINDOW + 0x20000;
				}
			}

			ShowControlBar(&m_wndHelpWindow, TRUE, FALSE);
			m_wndHelpWindow.HelpContext(dwHelpData);
		
			break;
		}

		case HELP_QUIT:
		{
			break;
		}
		default:
		{
			ASSERT(FALSE);
		}
	}
}


void CMainFrame::OnOnlinePreferences() 
{
	IConnectionManager* pConnectionManager = GetGlobalConnectionManager();

   if(pConnectionManager != NULL)
      pConnectionManager->EditPrefs();
}

void CMainFrame::OnUpdateOnlinePreferences(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetGlobalConnectionManager() != NULL);
}

//
// This is called when content changes.
//

LRESULT CMainFrame::OnContentChanged(WPARAM wParam, LPARAM lParam)
{
	GET_PMWAPP()->ReopenCollectionManager();
	return 0;
}

//
// This is called when shared paths change.
//

LRESULT CMainFrame::OnSharedPathsChanged(WPARAM wParam, LPARAM lParam)
{
	GET_PMWAPP()->UpdateSharedPaths();
	return 0;
}

CToolBar* CMainFrame::GetToolBar(UINT nID)
{ 
	CToolBar* pToolBar = NULL;
	if(m_hToolbarDll)
	{
		GET_TOOLBAR pGet = (GET_TOOLBAR)::GetProcAddress(m_hToolbarDll, "GetToolbar");
		if (pGet != NULL)
		{
			pToolBar = pGet(nID);
		}
	}
	return pToolBar; 
}

LRESULT CMainFrame::OnBuildFontList(WPARAM wParam, LPARAM lParam)
{
	SFontComboInfo* pInfo = (SFontComboInfo*)lParam;

	BOOL is_registered = GET_PMWAPP()->user_is_registered();

	// Add all the chooseable faces to the list.
	FaceEntry* entry;
	int i;

	// First the best
	if(pInfo->pMRUList != NULL)
	{
		CRecentFileList& MRUList = *pInfo->pMRUList;
		int nFaceIndex;
		for (i = 0; i  < MRUList.GetSize(); i++)
		{
			if ((nFaceIndex = typeface_server.face_list.find_face(MRUList[i])) != -1)
			{
				entry = typeface_server.face_list.get_face(nFaceIndex);
				if (entry->is_chooseable(is_registered))
				{
					int nIndex = pInfo->pCombo->InsertString(-1, entry->get_name());

					if (nIndex != -1)
					{
						pInfo->pCombo->SetItemData(nIndex, (DWORD)nFaceIndex);
					}
				}
			}
		}
	}
	// Now the rest
	for (entry = typeface_server.face_list.first_face(), i = 0;
		entry != NULL;
		entry = entry->next_face(), i++)
	{
		if (entry->is_chooseable(is_registered))
		{
			int nIndex = pInfo->pCombo->InsertString(-1, entry->get_name());

			if (nIndex != -1)
			{
				pInfo->pCombo->SetItemData(nIndex, (DWORD)i);
			}
		}
	}
	return 0L;
}

LRESULT CMainFrame::OnShowFontPreview(WPARAM wParam, LPARAM lParam)
{
	if(m_pFontPreview)
		m_pFontPreview->ShowWindow(lParam);
	return 0L;
}

LRESULT CMainFrame::OnUpdateFontPreview(WPARAM wParam, LPARAM lParam)
{
	if(m_pFontPreview)
	{
		m_pFontPreview->SetFontID(lParam);
		m_pFontPreview->Invalidate();
	}
	return 0L;

}

LRESULT CMainFrame::OnMoveFontPreview(WPARAM wParam, LPARAM lParam)
{
	if(m_pFontPreview)
		m_pFontPreview->SetWindowPos(&wndTop, wParam, lParam, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	return 0L;
}

LRESULT CMainFrame::IsPreviewVisible(WPARAM wParam, LPARAM lParam)
{
	if(m_pFontPreview)
		return m_pFontPreview->IsWindowVisible();
	
	return FALSE;
}

void CMainFrame::OnActivateApp( BOOL bActive, HTASK hTask )
{
	if(bActive)
	{
		GET_PMWAPP()->ChangeDisplayMode();
	}
	else
	{
		GET_PMWAPP()->RestoreDisplayMode();		
	}

	CFrameWnd::OnActivateApp(bActive, hTask);
}

void CMainFrame::OnBuildArtComponentDatabase() 
{
   CString        csINIFile, csFileTitle, csSourceFile, csDestDir;
   BOOL           bRunSilent;
   ERRORCODE      errorcode;

	errorcode = GetBuildInfo("CompDbBuildSource", "CompDbBuildDest",
		&csINIFile, &csFileTitle, &csSourceFile, &csDestDir, &bRunSilent);
	if(errorcode != ERRORCODE_None)
		return;

	IPathManager* pPathManager = GetGlobalPathManager();
	CPCompDBCollBuilder builder(csSourceFile, csDestDir, csFileTitle);
	builder.DoBuild();
}

void CMainFrame::OnBuildPageComponentDatabase() 
{
   CString        csINIFile, csFileTitle, csSourceFile, csDestDir;
   BOOL           bRunSilent;
   ERRORCODE      errorcode;

	errorcode = GetBuildInfo("CompPageBuildSource", "CompPageBuildDest",
		&csINIFile, &csFileTitle, &csSourceFile, &csDestDir, &bRunSilent);
	if(errorcode != ERRORCODE_None)
		return;

	CPCompPageCollBuilder builder(csSourceFile, csDestDir, csFileTitle);
	builder.DoBuild();

}


/////////
// CMainFrame function checks each menu item for popup menu items 
// belonging to it. Each popup menu is examined to determine if any
// menu items are enabled. If all items of the sub menu are grayed,
//	the popup menu item is grayed.
//
void CMainFrame::SetSubMenusState(
	CMenu* pPopupMenu,   // the menu to display
	UINT nIndex)			// index of the menu in the main menu bar
{		
	// number of menu items
	int nMenuItems = pPopupMenu->GetMenuItemCount();

	CView *	pView = GetActiveView();

	CMenu*	pSubMenu;

	for (int i = 0; i < nMenuItems; i++)
	{
		// sniff out sub menus, if any
		pSubMenu = pPopupMenu->GetSubMenu(i);

		if (pSubMenu)
		{
			// handle nested submenus through recursion
			SetSubMenusState(pSubMenu, i);

			// call base class function for SUB MENU
			// to set menu item states in Update Command UI handlers
			// that are invoked by OnInitMenuPopup
			// this call assumes that pSubMenu is not a system menu
			CFrameWnd::OnInitMenuPopup(pSubMenu, nIndex, FALSE);

			// number of enabled sub menu items
			int nEnabledItems = CountEnabledMenuItems(pSubMenu);

			if (nEnabledItems)
			{
				// enable the pop-up sub menu
				pPopupMenu->EnableMenuItem( i, MF_ENABLED | MF_BYPOSITION);
			}
			else
			{
				// disable the pop-up sub menu
				pPopupMenu->EnableMenuItem( i, MF_GRAYED | MF_BYPOSITION);
			}
		}
	}
}


/////////
// CMainFrame function to return number of enabled menu items
// this function considers item to be enabled if not grayed out
// primarily used to check for entirely grayed out (disabled) menu
//
// Returns: the number of enabled menu items
//
int CMainFrame::CountEnabledMenuItems(
	CMenu* pMenu)			// the menu being processed
{
	int nEnabledItems = 0;

	int nMenuItems = pMenu->GetMenuItemCount();

	int i = 0;

	while (i < nMenuItems)
	{
		UINT nState = pMenu->GetMenuState(i, MF_BYPOSITION);

		if (nState != (UINT)-1)     // -1 signals an error
		{
			if ( ((nState & MF_GRAYED) == 0)        // if not grayed
			  && ((nState & MF_DISABLED) == 0))     // or otherwise disabled
			{
				nEnabledItems++;
			}
		}

		i++;
	}
	return nEnabledItems;
}


