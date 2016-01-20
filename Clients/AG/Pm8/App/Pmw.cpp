/*
// $Workfile: PMW.CPP $
// $Revision: 2 $
// $Date: 3/05/99 6:02p $
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
// $Log: /PM8/App/PMW.CPP $
// 
// 2     3/05/99 6:02p Johno
// ShutdownRenaissanceFramework() was not checking for NULL pointers, as
// is the case with a multiple instance early shut down; a second instance
// would crash.
// (a.k.a "Clever syntax syndrome")
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 173   3/01/99 11:56a Cschende
// change default font from "Tiemp" to "Imperial"
// 
// 172   2/26/99 3:01p Dennis
// Put back in initialization of rich edit control.
// 
// 171   2/26/99 10:08a Johno
// Moved AppendNamedApplicationRegistryPath() from PMW.CPP to INIFILE.CPP
// 
// 170   2/25/99 5:23p Johno
// Moved AppendNamedApplicationRegistryPath() from UTIL.CPP to PMW.CPP,
// as some shared versions of UTIL.CPP could not find needed H file
// 
// 169   2/25/99 3:45p Blee
// Added flag so that only one cancel is necessary during startup if you
// don't have the Home CD.
// 
// 168   2/24/99 9:15p Psasse
// fixed  a memory leak with PhotoProjects
// 
// 167   2/24/99 1:39p Johno
// Fix for icon / file association errors
// 
// 166   2/23/99 4:26p Dennis
// Added init of RTF control
// 
// 165   2/20/99 9:22p Psasse
// Support for Vertical Banners
// 
// 164   2/04/99 6:20p Blee
// Added support for PSLaunchDLL module (RunPrintShopWizard())
// 
// 163   2/04/99 3:05p Johno
// Changes for new "Blank Page" (poster) project type
// 
// 162   1/29/99 3:58p Blee
// in InitFileSystem, allow running without CD in drive
// 
// 161   1/28/99 4:48p Mwilson
// added resolution change on the fly
// 
// 160   1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 159   1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 158   1/20/99 10:28a Mwilson
// moved toolbars into seperate DLL
// 
// 157   1/14/99 9:44a Cschende
// added an enum for the user prefered date format and class member to
// keep track of the user preference and a method for getting the date
// formatted in a string
// 
// 156   1/13/99 5:29p Gbeddow
// enable the ImageGear GIF extension for the image component
// 
// 155   12/24/98 5:08p Psasse
// added PhotoProjects wizard (pre-liminary)
// 
// 154   12/23/98 5:33p Psasse
// support for new project type - easyprints
// 
// 153   12/19/98 4:38p Psasse
// Revert to Saved support
// 
// 152   12/16/98 11:36a Lwilson
// Added PS color dialog along with collection support for textures.
// 
// 151   12/10/98 10:59a Lwilson
// Removed byte alignment pragmas from framework dependencies, as they are
// now implemented in the individual framework includes.  Also, removed
// FrameworkIncludes.h as it is now in the precompiled header.
// 
// 150   12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 149   11/23/98 1:27p Mwilson
// Added creataparty configuration
// 
// 148   11/20/98 1:32p Mwilson
// added capability to turn off auto instructions in preferences
// 
// 147   11/10/98 2:07p Gbeddow
// oleimpl.h does not exist in VC6
// 
// 146   11/10/98 10:48a Gbeddow
// changes for VC6 which no longer defines EW_REBOOTSYSTEM in winuser.h
// 
// 145   11/09/98 9:44a Dennis
// Removed update of URL
// 
// 144   11/06/98 4:53p Jayn
// 
// 143   11/06/98 12:20p Mwilson
// added a member for large font mode
// 
// 142   11/02/98 5:14p Jayn
// 
// 141   10/28/98 1:48p Jayn
// More polish for ordering and downloading.
// 
// 140   10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 139   10/26/98 1:24p Johno
// Moved  OnFileOpen() from CPmwApp to CMainFrame
// 
// 138   10/20/98 5:32p Jayn
// Improving the online order form.
// 
// 137   10/19/98 11:07a Jayn
// Fixes
// 
// 136   10/19/98 10:50a Jayn
// More COM restructuring changes.
// 
// 135   10/19/98 10:28a Dennis
// Connection Manager error now from resource
// 
// 134   10/15/98 3:48p Dennis
// New OCX Connection Manager
// 
// 133   10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 132   10/12/98 4:13p Johno
// Eliminated CPMContentServer in art store build
// 
// 131   10/12/98 1:27p Mwilson
// added craft deluxe configuration
// 
// 130   10/07/98 5:46p Jayn
// Changes to the content manager.
// 
// 129   10/05/98 3:03p Jayn
// More content manager cleanup.
// 
// 128   10/03/98 9:19p Psasse
// changed behavior to "Open" call for ContentManager
// 
// 127   10/02/98 7:10p Jayn
// More content manager changes
// 
// 126   9/29/98 5:34p Johno
// Changes (?) for art and more store
// 
// 125   9/29/98 1:06p Jayn
// Some changes to keep the code from using a deleted path manager.
// 
// 124   9/29/98 12:30p Johno
// Delay creation of border editor until after configuration object
// created (not actually used)
// 
// 123   9/28/98 10:54a Johno
// Use pmwinet.h in place of afxinet.h
// 
// 122   9/23/98 11:02a Dennis
// Added AfxEnableControlContainer
// 
// 121   9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 120   9/22/98 3:32p Psasse
// converting LPCSTR'S to CString&
// 
// 119   9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 118   9/15/98 4:24p Psasse
// Added random data server functionality
// 
// 117   9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 116   9/14/98 12:10p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 115   9/10/98 4:51p Mwilson
// added flag for tool running
// 
// 114   8/28/98 3:20p Dennis
// Updated AG Copyright
// 
// 113   8/20/98 6:17p Hforman
// 
// 112   8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 111   8/20/98 12:45p Hforman
// Craft changes; add another OpenDocumentFile()
// 
// 110   8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 109   8/06/98 4:32p Mwilson
// added new craft project type
// 
// 108   7/26/98 2:59p Hforman
// palette flash fixes
// 
// 107   7/20/98 3:31p Psasse
// Launching Project files (*.sig) from the desktop works again
// 
// 106   7/20/98 9:30a Dennis
// Fix for PreTranslateMessage crash
// 
// 105   7/18/98 7:32p Psasse
// Changed behavior of PreTranslateMessage to Send On keydown to Helpful
// Hint
// 
// 104   7/16/98 4:03p Rlovejoy
// Call KillFirstPoster() regardless if file is part of a collection.
// 
// 103   7/16/98 1:45p Psasse
// Added Advanced Printing Functionality "Print As Bitmap"
// 
// 102   7/14/98 6:34p Jayn
// 
// 101   7/14/98 5:36p Johno
// Set CPmwDoc::FLAG_LoadedFromCollection in CPmwApp::OpenDocumentFile()
// 
// 100   7/12/98 5:35p Jayn
// Moved online stuff earlier on in the startup so it would be available
// in
// automation case.
// 
// 99    7/08/98 3:21p Mwilson
// added printstandard unsupported dialog function
// 
// 98    7/07/98 4:56p Dennis
// Added "PrintMasterEngine" global atom
// 
// 97    7/06/98 5:43p Rlovejoy
// Function to update added supported projects mask.
// 
// 96    7/06/98 10:12a Rlovejoy
// Kill first poster on document open.
// 
// 95    7/01/98 5:29p Mwilson
// moved palette init to init instance
// 
// 94    7/01/98 5:22p Psasse
// more robust filtering routines
// 
// 93    6/30/98 6:23p Psasse
// Online Graphics server address becomes a member of the app and is
// pmgs.ini dependent
// 
// 92    6/29/98 11:42a Hforman
// clean up unused helpful hint methods
// 
// 91    6/24/98 7:20p Psasse
// correctly extracting *.pmo and *.dtt files
// 
// 90    6/24/98 4:38p Dennis
// PrintMaster now uses AG Designs copyright
// 
// 89    6/19/98 6:20p Johno
// Changed PreventHub, added SetPreventHub
// 
// 88    6/18/98 5:19p Cboggio
// Changes to SetNewBusinessCardSize function for localization
// 
// 87    6/18/98 4:32p Johno
// Deleted hub calls to MainFrm
// 
// 86    6/18/98 12:38p Johno
// Improved hub control
// 
// 85    6/18/98 11:02a Johno
// KillFirstPoster() now boolean
// 
// 84    6/17/98 4:34p Psasse
// changed \n\n to \r\n\r\n
// 
// 83    6/17/98 12:19p Johno
// Moved OnHubNew() to MainFrm
// 
// 82    6/17/98 11:06a Jayn
// 
// 81    6/16/98 9:13a Dennis
// AG Specific Copyright Changes
// 
// 80    6/12/98 4:35p Mwilson
// fixed startup bug
// 
// 79    6/12/98 2:55p Jayn
// More startup changes.
// 
// 78    6/12/98 10:38a Jayn
// Changes to the startup code (no more 5-second delay)
// 
// 77    6/11/98 9:36a Dennis
// Better Resource Management loading
// 
// 76    6/05/98 2:32p Fredf
// More loading box changes.
// 
// 75    6/05/98 12:10p Fredf
// Changes for new legal and copyright information.
// 
// 74    6/04/98 9:40p Psasse
// better looking modal Helpful Hint windows
// 
// 73    6/04/98 7:59p Rlovejoy
// Added parameter for ShowHelpfulHin().
// 
// 72    6/04/98 7:20p Psasse
// modal helpful hint support
// 
// 71    6/02/98 3:00p Jayn
// 
// 70    6/01/98 9:12p Psasse
// an attempt to get helpful hints on top of border editor
// 
// 69    6/01/98 1:22p Mwilson
// added stuff for command line handling and for DDE commands.  Used for
// remote fulfillment
// 
// 68    5/31/98 5:20p Psasse
// fixed initial daily tip coming in under the hub
// 
// 67    5/28/98 2:54p Hforman
// add Helpful Hint voice-over functions
// 
// 66    5/26/98 2:39p Jayn
// New table code.
// 
// 65    5/26/98 1:55p Dennis
// Added GetAppIcon(). Moved call to HangUp() forward so configuration
// object can be used.
// 
// 64    5/20/98 9:19p Psasse
// Support for larger bitmaps in Helpful hints window and asynchronous
// progress control becomes more like IE
// 
// 63    5/18/98 3:11p Cboggio
// Added #ifdef LOCALIZE to make sure that localization changes won't
// affect the US English version. This will be used for all localization
// changes (except for the addtion of a few strings in the resource file),
// although the localization changes shouldn't (in theory) affect the US
// version functionality.
// The CLocale class is now used to handle locale information.
// 
// 62    5/12/98 6:05p Johno
// Changed m_csCurrentLocale to csCurrentLocale to compile.
// 
// 61    5/12/98 5:39p Cboggio
// Added call to setlocale for localization purposes
// 
// 60    5/12/98 5:38p Cboggio
// 
// 59    5/11/98 9:05p Psasse
// changed the content manager to a member of the app
// 
// 58    5/09/98 1:57p Psasse
// added #include "progbar.h" for ConnMgr support
// 
// 57    4/30/98 3:48p Fredf
// Calendar brand new previews.
// 
// 56    4/30/98 2:13p Fredf
// Help window is initially hidden if help system could not load.
// 
// 55    4/29/98 9:47p Psasse
// 
// 54    4/29/98 6:50p Fredf
// Handles cases of help failure more gracefully.
// 
// 53    4/27/98 5:30p Fredf
// Improved bar positioning.
// 
// 52    4/25/98 7:55p Psasse
// 
// 51    4/24/98 6:34p Fredf
// Font dialog uses new color combo boxes.
// 
// 50    4/23/98 9:11p Psasse
// 
// 49    4/23/98 7:20p Fredf
// Persistant Show/Hide Help Window.
// 
// 48    4/23/98 4:01p Rlovejoy
// Moved kill first poster code into a function.
// 
// 47    4/21/98 9:15a Fredf
// Removed duplicate method.
// 
// 46    4/21/98 8:50a Dennis
// Removed #ifdef AG_BUILD.  Configuration now determines which resource
// manager gets created (if any).
// 
// 45    4/20/98 2:18p Fredf
// Migrated 4.0 About Box and Startup dialog changes.
// 
// 44    4/18/98 4:24p Fredf
// T-Shirt printing.
// 
// 43    4/17/98 4:47p Fredf
// T-Shirts
// 
// 42    4/16/98 7:32p Fredf
// "Preview Actual Size" control in font dialog (NEWFONT.CPP)
// 
// 41    4/16/98 6:47p Hforman
// new Open dialog
// 
// 40    4/15/98 11:08a Dennis
// Fixed compile warning
// 
// 39    4/14/98 7:59p Fredf
// First attempt at Help Window.
// 
// 38    4/14/98 3:14p Dennis
// Added ResourceManager for American Greetings product.
// 
// 37    4/14/98 12:07p Rlovejoy
// Post message to put up the hub.
// 
// 36    4/10/98 4:56p Rlovejoy
// Changes for new hub implementation.
// 
// 35    4/02/98 9:09p Psasse
// internet cache support
// 
// 34    3/24/98 2:10p Rlovejoy
// Added new strings for MRU font list.
// 
// 33    3/21/98 1:18p Fredf
// New menus and toolbar names.
// 
// 32    3/17/98 2:43p Fredf
// New workspace toolbars.
// 
// 31    3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 30    3/10/98 5:24p Jayn
// Master page changes
// 
// 29    2/10/98 1:25p Dennis
// Added New Web Project Dialog for brand new path
// 
// 28    2/04/98 1:03p Dennis
// Added CConnectionManager object
// 
// 27    1/20/98 3:44p Fredf
// Got rid of GetNowTime(), use GetLocalTime(SYSTEMTIME) instead.
// 
// 26    1/20/98 8:54a Fredf
// Moved CTimeStamp from UTIL to TIMESTMP.
// 
// 25    1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 24    1/13/98 11:07a Hforman
// add BorderEditorInitialized()
// 
// 23    1/13/98 10:23a Hforman
// fixed memory leak related to borders
// 
// 22    1/08/98 2:14p Hforman
// add Border Editor stuff
// 
// 21    12/23/97 10:27a Jayn
// 
// 20    12/16/97 5:43p Hforman
// more helpful hint stuff
// 
// 19    12/15/97 12:45p Jayn
// Automation
// 
// 18    12/12/97 5:17p Hforman
// Helpful Hints stuff
// 
// 17    12/11/97 4:23p Dennis
// Added Web doc/view
// 
// 16    12/05/97 5:15p Hforman
// more helpful hint stuff
// 
// 15    12/04/97 12:45p Hforman
// add helpful hint stuff
// 
// 14    12/01/97 1:27p Jstoner
// created sticker project type
// 
// 13    11/25/97 8:54a Jstoner
// added new calendar type - cal with pic on same page
// 
// 12    11/21/97 4:40p Hforman
// added TextBox Outline show/hide stuff; added updating of all document
// views when changing view of Rulers/Guides/TextBox Outlines
// 
// 11    11/13/97 2:59p Jstoner
// minor change to caldatedlg constructor
// 
// 10    11/12/97 6:39p Hforman
// add guide stuff
// 
// 9     11/10/97 3:50p Jstoner
// fixed some problems regarding adding yearly calendars
// 
// 8     11/10/97 12:35p Jstoner
// fixed so that calendar wizard dialgs step back in correct order.
// 
// 7     11/05/97 2:31p Jstoner
// can now print multi-page, single-sided calendar projects
// todo: fix print preview and double-sided printing
 * 
 * 6     11/03/97 2:43p Jstoner
 * added wmf's to calendar wizard dialogs
 * 
 * 5     10/31/97 3:55p Jstoner
 * multi page calendar fixes
// 
//    Rev 1.11   26 Oct 1997 18:59:06   JOE
// modified calendar desktop to handle picture/calendar
// 
//    Rev 1.10   30 Sep 1997 11:05:22   johno
// Changes to new_document for post cards
// 
//    Rev 1.9   26 Sep 1997 16:59:38   johno
// Added post cards to new_document
// 
//    Rev 1.8   19 Sep 1997 17:33:34   johno
// Changes for new Post Card project type
// (preliminary)
// 
// 
//    Rev 1.7   04 Sep 1997 10:51:30   johno
// 
//    Rev 1.6   04 Sep 1997 10:27:58   johno
// Pasting a sentiment is now not part of PmwDoc ctor
// 
//    Rev 1.5   26 Aug 1997 14:41:46   johno
// Pasting a sentiment is now part of PmwDoc
// 
// 
//    Rev 1.4   25 Aug 1997 10:32:18   johno
// Use "path of inspiration" stuff now from PmwDoc
// 
// 
//    Rev 1.3   22 Aug 1997 12:16:12   johno
// Changes for new 5.0 hub buttons
// 
// 
//    Rev 1.2   19 Aug 1997 15:38:52   johno
// In WinHelp, changed CPrintMasterConfiguration::IsAddOn ()
// to GetConfiguration()->IsHelpFile ()
// 
// 
//    Rev 1.1   15 Aug 1997 15:40:16   Fred
// Can set movie with movie= in [Configuration]
// 
//    Rev 1.0   14 Aug 1997 15:23:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:48   Fred
// Initial revision.
// 
//    Rev 1.118   08 Aug 1997 13:49:46   johno
// Disable help for addons
// 
//    Rev 1.117   30 Jul 1997 14:01:48   Jay
// GetNowTime function in Util
// 
//    Rev 1.116   30 Jul 1997 12:44:32   johno
// Added case for addon about in OnAppAbout
// Took out DoAbout, as it does not seem to be used
// 
//    Rev 1.115   24 Jul 1997 16:43:30   Jay
// Enforces max document limit in 16-bit
// 
//    Rev 1.114   21 Jul 1997 17:24:28   Jay
// MciWnd is now init'd before the sender info is checked.
// 
//    Rev 1.113   21 Jul 1997 11:02:00   dennisp
// Added call to show wait cursor in OpenDocumentFile
// 
//    Rev 1.112   18 Jul 1997 15:44:44   Jay
// Passes clean string to AddToRecentFileList.
// 
//    Rev 1.111   14 Jul 1997 11:18:26   Fred
// Added framework for 'Find projects with graphics' tool
// 
//    Rev 1.110   10 Jul 1997 17:02:28   Fred
// Makes sure popup palettes are on screen
// 
//    Rev 1.109   08 Jul 1997 17:27:18   Jay
// Support for Transfer directory
// 
//    Rev 1.108   01 Jul 1997 10:52:34   Jay
// New MRU stuff
// 
//    Rev 1.107   27 Jun 1997 17:44:50   johno
// Use IDR_MAINFRAME_ADDON for main window if we are an addon
// 
//    Rev 1.106   26 Jun 1997 17:11:50   Jay
// Support for embed_source
// 
//    Rev 1.105   26 Jun 1997 11:39:08   johno
// new_document now works with empty friendly name
// (for "path of inspiration" open from disk).
// 
//    Rev 1.104   18 Jun 1997 16:42:14   johno
// 
//    Rev 1.103   17 Jun 1997 12:24:28   johno
// Added registration and unregistration
// of CProgressBar (for addon install)
// 
//    Rev 1.102   17 Jun 1997 11:33:24   Jay
// Template stuff; moved CollectionManager.Open()
// 
//    Rev 1.101   13 Jun 1997 08:54:32   Jay
// 
//    Rev 1.100   11 Jun 1997 15:48:46   johno
// Add on support
// 
//    Rev 1.99   10 Jun 1997 16:20:02   johno
// Add on control moved to configuration class
// 
// 
//    Rev 1.98   06 Jun 1997 16:47:10   Jay
// Startup dialog.
// 
//    Rev 1.97   06 Jun 1997 11:51:22   Jay
// Licensing changes
// 
//    Rev 1.96   05 Jun 1997 18:09:00   johno
// More multiple hub stuff
// 
// 
//    Rev 1.95   05 Jun 1997 14:36:26   johno
// Changes for multiple hub support
// 
// 
//    Rev 1.94   03 Jun 1997 10:12:32   Fred
// Reenabled file sharing check.
// 
//    Rev 1.93   03 Jun 1997 09:52:38   Fred
// Does not require user to install share.exe
// 
//    Rev 1.92   02 Jun 1997 14:02:28   Fred
// ShowMovie INI flag
// 
//    Rev 1.91   02 Jun 1997 11:04:28   Jay
// Set product flags for configuration.
// 
//    Rev 1.90   01 Jun 1997 16:07:00   Fred
// Uses path manager to find license DLL. Disables movie in 16-bit
// 
//    Rev 1.89   30 May 1997 15:25:12   Jay
// Removed some unused stuff to make this compile.
// 
//    Rev 1.88   30 May 1997 09:45:44   Jay
// Added OpenHiddenDocument() & NewHiddenDocument()
// 
//    Rev 1.87   23 May 1997 16:15:20   Jay
// No more _000PMW.INI
// 
//    Rev 1.86   22 May 1997 15:26:12   johno
// Changes for brand new path orientation
// 
//    Rev 1.85   22 May 1997 12:50:10   johno
// Added boolean (m_fSoundEffectsState),
// initialize from USER.INI
// 
// 
//    Rev 1.84   20 May 1997 17:08:58   johno
// Now deletes m_pLicenseDLL in destructor
// 
//    Rev 1.83   20 May 1997 16:43:36   johno
// 
//    Rev 1.82   19 May 1997 20:32:00   johno
// Content licensing added to project
// 
// 
//    Rev 1.81   14 May 1997 14:07:02   Jay
// Turned off DragAcceptFiles. PMGTODO: Re-enable it correctly.
// 
//    Rev 1.80   14 May 1997 10:16:44   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.79   08 May 1997 15:41:44   johno
// 
//    Rev 1.78   07 May 1997 16:47:14   johno
// Added csNewPictureFileName and csNewPictureFriendlyName CStrings.
// If these are not empty, the Brand New path will use them to insert
// a picture into a new project.
// The art gallery browser can fill them if invoked from the hub.
// 
// 
//    Rev 1.77   07 May 1997 14:31:16   Jay
// Fixed bugs in workspace section.
// 
//    Rev 1.76   07 May 1997 12:10:48   Jay
// User directory and collections
// 
//    Rev 1.75   01 May 1997 17:09:36   Jay
// 
//    Rev 1.74   29 Apr 1997 17:01:26   johno
// Added CMciWnd object
// 
//    Rev 1.73   28 Apr 1997 16:02:58   Fred
//  
// 
//    Rev 1.72   28 Apr 1997 08:39:06   Fred
//  
// 
//    Rev 1.71   25 Apr 1997 12:52:02   Fred
// Open from disk runs project browser.
// 
//    Rev 1.70   25 Apr 1997 09:57:14   Fred
// Reads smacker movie from \RUNTIME on CD
// 
//    Rev 1.69   22 Apr 1997 13:17:46   Fred
// Start of project and art browsers
*/

#include "stdafx.h"
#include "pmw.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <direct.h>
#include <io.h>
#include <dos.h>

#include <malloc.h>

#include <stdlib.h>						//for rand()

#include "time.h"                   // for time functions
#include "sys\types.h"              // for stat.h
#include "sys\stat.h"               // for _stat() call

#include "mainfrm.h"
#include "pmwtempl.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "pmwmdi.h"
#include "pmwini.h"
#include "aboutdlg.h"
#include "OpenDlgs.h"
#include "labeldlg.h"
#include "Craftdlg.h"
#include "envtype.h"
#include "postcard.h"
#include "addrbook.h"
#include "calendar.h"
#include "PhPrjDlg.h"

#include "bmp.h"
#include "gif.h"
#include "jpeg.h"
#include "pcd.h"
#include "pcx.h"
#include "pngimpl.h"
#include "grafobj.h"

#include "signdoc.h"
#include "signview.h"
#include "carddoc.h"
#include "cardview.h"
#include "banndoc.h"
#include "bannview.h"
#include "caldoc.h"
#include "calview.h"
#include "lbldoc.h"
#include "lblview.h"
#include "envdoc.h"
#include "envview.h"
#include "newsdoc.h"             // Newsletter document
#include "newsview.h"            // Newsletter view
#include "brodoc.h"              // Brochure document
#include "broview.h"             // Brochure view
#include "webdoc.h"              // Web document
#include "webview.h"             // Web view
#include "webdlg.h"              // For Page Setup
#include "easyprintsdoc.h"
#include "easyprintsview.h"

#include "typeface.h"
#include "pmgfont.h"

// Implementations of interfaces.
#include "userset.h"
#include "drivemgr.h"
#include "pathmgr.h"
#include "collmgr.h"
#include "contsrv.h"
#include "contmgr.h"

#include "newdlg.h"
#include "oriendlg.h"
#include "util.h"                /* MFC utilities */
#include "utils.h"               /* "Jay" utilities */
#include "color.h"
#include "objimg.h"
#include "clip.h"
#include "compfn.h"
#include "register.h"
#include "grpobj.h"
#include "frameobj.h"
#include "cdemodlg.h"
#include "demodlg.h"
#include "upgradlg.h"
#include "promtdlg.h"
#include "helphint.h"
#include "ipframe.h"
#include "borders.h"
#include "spinbutt.h"            /* Spin button control. */
#include "cdcache.h"

#include "cwmfctl.h"
#include "lblprvw.h"             /* Label preview control. */
#include "phoprjprvw.h"          /* PhotoProject preview control. */
#include "envprvw.h"             /* Envelope preview control. */
#include "celctl.h"					// Cell outline control
#include "tblctl.h"					// Table autoformat control

#include "dlgmgr.h"

#include "printer.h"             /* CPrinterInfo */
#include "paper.h"               /* CPaperInfo */

#include "sharenot.h"
#include "glicense.h"

#ifndef WIN32
#if _MSC_VER < 1200 // GCB 11/10/98 - oleimpl.h does not exist in VC6
	#include <oleimpl.h>
#endif
#endif

#include <afxpriv.h>

#include "pmwcfg.h"
#include "globatom.h"
#include "inifile.h"

#include "tcombo.h"

#include "browser.h"

//j#include "smacker.h"
#include "cprogbar.h"
#include "pmwrfl.h"

#include "caltyped.h"
#include "calpicd.h"
#include "caldated.h"
#include "stkrdoc.h"
#include "stkrview.h"
#include "progbar.h"
#include "connmgrw.h"   // Connection Manager (for internet)
#include "srchart.h"

// Resource Manager related includes
#include "rm.h"		   // For CResourceManager 
#include "rmagcom.h"
#include "rmprint.h"
#include "rmcraft.h"
#include "rmspirit.h"

#include "pmwinet.h"
#include "ContMgr.h"

// Framework Support
ASSERTNAME
#include "Application.h"
#include "ApplicationGlobals.h"
#include "ComponentManager.h"
#include "ResourceManager.h"
#include "PrintManager.h"
#include "stream.h"
#include "FrameworkResourceIDs.h"
#include "ColorDialog.h"
#include "ImageLibrary.h"

// Path To Project interfaces
#include <initguid.h>
#include "LaunchInterfaces.h"		// Interfaces to PSLaunchDLL

// localization stuff
// Default language is always US English, so 
// whether or not this option is used should not affect how the US version
// works. However, to avoid any risk of the localization code changes
// affecting the US version, the localization stuff will only be included
// if LOCALIZE is defined
#ifdef LOCALIZE
#include "clocale.h"
#endif


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BYTE far screen_gamma_curve[];
extern BOOL near fWantDebugMessages;
extern void initialize_palette(void);

#ifdef _DEBUG
//#define NODEBUGMEM
#endif
#ifdef NODEBUGMEM
extern "C" int NEAR afxMemDF;
#endif

/*
// INI file strings.
*/

char far SECTION_Paths[] = "Paths";
char far ENTRY_PictureSearchPath[] = "PictureSearchPath";
char far ENTRY_CachePath[] = "Cache";
char far ENTRY_LicensePath[] = "LicensePath";

char far SECTION_Debug[] = "Debug";
char far ENTRY_SeeMessages[] = "SeeMessages";
char far ENTRY_SeeHelp[] = "SeeHelp";
char far ENTRY_SeeProfile[] = "SeeProfile";
char far ENTRY_EditTemplates[] = "EditTemplates";
char far ENTRY_MakeTemplates[] = "MakeTemplates";
char far ENTRY_FixTemplates[] = "FixTemplates";
char far ENTRY_No386[] = "No386";

char far SECTION_Fonts[] = "Fonts";
char far ENTRY_CacheKSize[] = "CacheKSize";
char far ENTRY_DefaultFace[] = "DefaultFace";
char far ENTRY_DefaultSize[] = "DefaultSize";

char far SECTION_Image[] = "Image";
char far ENTRY_MinDiskSpaceK[] = "MinDiskSpaceK";
char far ENTRY_MaxScaleUp[] = "MaxScaleUp";
char far ENTRY_MaxScaleDown[] = "MaxScaleDown";

char far ENTRY_DllPath[] = "DllPath";

char far SECTION_Printer[] = "Printer";
char far ENTRY_IniFile[] = "IniFile";
char far ENTRY_GammaValue[] = "GammaValue";
char far ENTRY_PostscriptType[] = "PostscriptType";
char far ENTRY_OutlineGammaValue[] = "OutlineGammaValue";
char far ENTRY_FastBitmapGammaValue[] = "FastBitmapGammaValue";
char far ENTRY_EnvelopePosition[] = "EnvelopePosition";
char far ENTRY_DefaultEnvelopePosition[] = "DefaultEnvelopePosition";
char far ENTRY_BadDC[] = "BadDC";
char far ENTRY_Copies[] = "Copies";
char far ENTRY_Orientation[] = "Orientation";
char far ENTRY_Tractor[] = "Tractor";
char far ENTRY_LeftMargin[] = "LeftMargin";
char far ENTRY_TopMargin[] = "TopMargin";
char far ENTRY_RightMargin[] = "RightMargin";
char far ENTRY_BottomMargin[] = "BottomMargin";
char far ENTRY_LeftOffset[] = "LeftOffset";
char far ENTRY_TopOffset[] = "TopOffset";
char far ENTRY_LeftMarginLandscape[] = "LeftMarginLandscape";
char far ENTRY_TopMarginLandscape[] = "TopMarginLandscape";
char far ENTRY_RightMarginLandscape[] = "RightMarginLandscape";
char far ENTRY_BottomMarginLandscape[] = "BottomMarginLandscape";
char far ENTRY_LeftOffsetLandscape[] = "LeftOffsetLandscape";
char far ENTRY_TopOffsetLandscape[] = "TopOffsetLandscape";
char far ENTRY_ReverseLandscape[] = "ReverseLandscape";
char far ENTRY_UseReverseLandscape[] = "UseReverseLandscape";
char far ENTRY_FastBitmaps[] = "FastBitmaps";
char far ENTRY_DoubleSidedType[] = "DoubleSidedType";
char far ENTRY_DoubleSided[] = "DoubleSided";
char far ENTRY_PrintInstructions[] = "PrintInstructions";
char far ENTRY_PrintReversed[] = "PrintReversed";
char far ENTRY_FlagPrintAsBitmap[] = "PrintAsBitmap";

char far SECTION_Configuration[] = "Configuration";
char far ENTRY_TextCompatability[] = "TextCompatability";
char far ENTRY_RegistrationCode[] = "RegistrationCode";  // In main PMW.INI
char far ENTRY_UseDDBs[] = "UseDDBs";
char far ENTRY_ShowMovie[] = "ShowMovie";
char far ENTRY_Movie[] = "Movie";
char far ENTRY_MarginStyle[] = "MarginStyle";
char far ENTRY_NewMargins[] = "NewMargins";
char far ENTRY_ReminderCount[] = "ReminderCount";
char far ENTRY_OEMID[] = "OEMID";
char far ENTRY_AddressBook[] = "AddressBookV3";
char far ENTRY_SenderBook[] = "SenderBook";
char far ENTRY_UndoLimit[] = "UndoLimit";
char far ENTRY_SkipShareTest[] = "SkipShareTest";
char far ENTRY_ShowRulers[] = "ShowRulers";
char far ENTRY_SnapToRulers[] = "SnapToRulers";
char far ENTRY_ShowGuides[] = "ShowGuides";
char far ENTRY_SnapToGuides[] = "SnapToGuides";
char far ENTRY_LockGuides[] = "LockGuides";
char far ENTRY_TextBoxOutlines[] = "TextBoxOutlines";
char far ENTRY_LastRunLevel[] = "LastRunLevel";
char far ENTRY_ShowActualFontSize[] = "ShowActualFontSize";
char far ENTRY_HintFullLengthBmp[] = "UseFullLengthBitmapsForHelpfulHints";

char far ENTRY_SkipAutoRunAtom[] = "SkipAutoRunAtom";    // In MSREG.INI
char far ENTRY_Version[] = "Version";                    // In MSREG.INI
char far ENTRY_ParentPartNumber[] = "ParentPartNumber";  // In MSREG.INI

char far SECTION_PhotoCD[] = "PhotoCD";
char far ENTRY_Draft[] = "Draft";
char far ENTRY_Proof[] = "Proof";

// New workspace section - DGP
char far SECTION_Workspace[] = "Workspace";
char far ENTRY_WorkspaceFileName[] = "File";
char far ENTRY_WorkspaceSize[] = "Size";

char far SECTION_Users[] = "Users";
char far ENTRY_DefaultDir[] = "DefaultDir";

char far SECTION_Desktop[] = "Desktop";
char far ENTRY_GuideColor[] = "GuideColor";
char far ENTRY_MasterGuideColor[] = "MasterGuideColor";

char far SECTION_FontsMRU[] = "Recent Font List";
char far ENTRY_FontMRU[] = "Font%d";

char far SECTION_Online[] = "Online";
char far ENTRY_ConnectURL[] = "ConnectURL";		// The URL to test for when connecting.

#ifdef LOCALIZE
// CB - new section for locale settings
// Default is US English if no values are found in the ini file
char far SECTION_Locale[] = "Locale";
char far ENTRY_Language[] = "Language";
char far ENTRY_ResourceDLLName[] = "ResourceDLL";
#endif

// For MRU.
static char BASED_CODE szFileSection[] = "Recent File List";
static char BASED_CODE szFileEntry[] = "File%d";

#define  FAR_STRING(n, s)  static const char BASED_CODE n[] = s;

// MLSTODO: Get thee to the resource file!
// JRO FAR_STRING(szTrialVersionExpired1, "Your Trial Version of the PrintMaster Gold Publishing Suite has expired.");
// FAR_STRING(szTrialVersionExpired2, "Trial Version Has Expired");
// FAR_STRING(szCannotDetermineConfiguration, "Cannot determine PrintMaster configuration.");
// FAR_STRING(szInsufficientMemory, "Insufficient memory to run.");
// FAR_STRING(szUnableToFindFonts, "Unable to find any TrueType fonts.");
// FAR_STRING(szEditBackOfCardNotAvailable, "Editing the back of the card is not available in @@D.");
// FAR_STRING(szFeatureNotAvailable, "This feature is not available in @@D.");

UINT uTrialVersionExpired1         = IDS_TRIAL_EXPIRED1;
UINT uTrialVersionExpired2         = IDS_TRIAL_EXPIRED2;
UINT uCannotDetermineConfiguration = IDS_CONFIG_NO_SEE;
UINT uInsufficientMemory           = IDS_NO_MEM;
UINT uUnableToFindFonts            = IDS_FONTS_NO_SEE;
UINT uEditBackOfCardNotAvailable   = IDS_CARD_NO_EDIT_BACK;
UINT uFeatureNotAvailable          = IDS_NO_FEATURE;

#ifdef WIN32
static char BASED_CODE cbResourceDLLName[] = "pmwres32.dll";
#else
static char BASED_CODE cbResourceDLLName[] = "pmwres.dll";
#endif


const int MIN_XRES = 800;
const int MIN_YRES = 600;

// These values are used if the CPmwApp object should exit windows
// when the CPmwApp destructor is called.
static BOOL fCallExitWindows = FALSE;
static DWORD dwExitWindowsCode;

// This flag prevents repeated prompting for the CD if it's not available during startup
static bool bHomeCDNotAvailable = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CPmwApp

BEGIN_MESSAGE_MAP(CPmwApp, CWinApp)
   //{{AFX_MSG_MAP(CPmwApp)
   ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
   ON_COMMAND(ID_FILE_NEW, OnFileNew)
   ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
   ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNewOpen)
   ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileNewOpen)
   //}}AFX_MSG_MAP
   // Standard file based document commands
   // Standard print setup command
// ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
   // Global help commands
   ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
   ON_COMMAND(ID_HELP_USING, CWinApp::OnHelpUsing)
   ON_COMMAND(ID_HELP, CWinApp::OnHelp)
   ON_COMMAND(ID_CONTEXT_HELP, CWinApp::OnContextHelp)
   ON_COMMAND(ID_DEFAULT_HELP, CWinApp::OnHelpIndex)
   // MRU code.
   ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateRecentFileMenu)
   ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE2, OnUpdateRecentFileMenu)
   ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE3, OnUpdateRecentFileMenu)
   ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE4, OnUpdateRecentFileMenu)
   ON_COMMAND_EX(ID_FILE_MRU_FILE1, OnOpenRecentFile)
   ON_COMMAND_EX(ID_FILE_MRU_FILE2, OnOpenRecentFile)
   ON_COMMAND_EX(ID_FILE_MRU_FILE3, OnOpenRecentFile)
   ON_COMMAND_EX(ID_FILE_MRU_FILE4, OnOpenRecentFile)

   ON_COMMAND(PMWM_PUT_PLACE_HINT_ON_BORDER, OnShowHintOnBorderDlg)

END_MESSAGE_MAP()

static void CheckProcessorType();
BOOL g_fProcessorIs386Compatible = FALSE;

#if 0
//#include <toolhelp.h>

void CheckLocalHeap(char cWhere)
{
   LOCALENTRY entry;

   memset(&entry, 0, sizeof(entry));
   entry.dwSize = sizeof(entry);

   static char BASED_CODE s0[] = "*** Check heap at %c *** \r\n";
   TRACE1(s0, cWhere);

   for (BOOL fResult = LocalFirst(&entry, AfxGetInstanceHandle());
        fResult;
        fResult = LocalNext(&entry))
   {
      static char BASED_CODE s1[] = "Type: %u, Size: %u\r\n";
      TRACE2(s1, entry.wType, entry.wSize);
   }
}
#endif

/*
// Ugrade the icon after a product change.
*/

static void UpgradeIcon(BOOL fCheck)
{
   BOOL fDoUpdate = TRUE;
   int nLevel = GetConfiguration()->ProductLevel();
   if (fCheck)
   {
   /*
   // Read the INI file to see what the level was last time we ran.
   // We only change the icon if we ran before at a different level.
   // The assumption is that if there is no key, the program has never
   // been run and the install program has just created the group.
   // This will not be true if a workstation install was done and then
   // PrintMaster was never run, but that seems a little rare.
   // (I could be wrong!)
   */
      int nLastLevel = AfxGetApp()->GetProfileInt(SECTION_Configuration,
                                        ENTRY_LastRunLevel,
                                        0);

      if (nLastLevel == 0 || nLastLevel == nLevel)
      {
      /* We don't need to update now. */
         fDoUpdate = FALSE;
      }
   }

/*
// Always write the level.
*/

   AfxGetApp()->WriteProfileInt(SECTION_Configuration, ENTRY_LastRunLevel, nLevel);

/*
// Update the icon if necessary.
*/

   if (fDoUpdate)
   {
      CString csPath;
      TRY
      {
         csPath = GetGlobalPathManager()->ExpandPath("MSRUN.EXE");
         csPath += " UpgradeIcon";
         WinExec(csPath, SW_SHOWNORMAL);
      }
      END_TRY
   }
}

/////////////////////////////////////////////////////////////////////////////
// CPmwApp construction

CPmwApp::CPmwApp()
      : m_bAgRemotePrint(FALSE),
		m_bPrintProjects(FALSE),
		m_bLargeFontMode(FALSE),
		m_bAutoInstructions(TRUE),
		m_bChangedDevMode(FALSE)
{
#ifdef NODEBUGMEM
   afxMemDF = 0;        // No memory debug.
#endif

   // Place all significant initialization in InitInstance

   m_pPortraitPrinterInfo = new CPrinterInfo;
   m_pLandscapePrinterInfo = new CPrinterInfo;
   m_pPortraitPaperInfo = new CPaperInfo;
   m_pLandscapePaperInfo = new CPaperInfo;

	m_pHelpfulHint = new CHelpfulHint;

	m_pBorderEditor = NULL;

   m_signature_file = NULL;
   m_volume_id = NULL;

   last_new_info.type = PROJECT_TYPE_BlankPage;
   last_new_info.subtype = 0;
   last_new_info.orientation = PORTRAIT;
   last_new_info.fDoubleSided = TRUE;
   last_new_info.pTemplate = NULL;
   last_new_info.m_pPaperInfo = new CPaperInfo;
   last_new_info.m_pCalendarInfo = NULL;

   fWantDebugMessages = FALSE;
   m_fInitialized = FALSE;

   m_pScratchTemplate = NULL;
   m_pCardTemplate = NULL;
   m_pBannerTemplate = NULL;
   m_pPosterTemplate = NULL;
   m_pCalendarTemplate = NULL;
   m_pLabelTemplate = NULL;
   m_pEnvelopeTemplate = NULL;
   m_pBizCardTemplate = NULL;
   m_pCertificateTemplate = NULL;
   m_pNoteCardTemplate = NULL;
   m_pFaxCoverTemplate = NULL;
   m_pStationeryTemplate = NULL;
   m_pNewsletterTemplate = NULL;
   m_pBrochureTemplate = NULL;
   m_pHalfCardTemplate = NULL;
   m_pWebPubTemplate = NULL;
   m_pPostCardTemplate = NULL;
   m_pTShirtTemplate = NULL;
   m_pCraftTemplate = NULL;
   m_pPhotoProjectsTemplate = NULL;
	m_pBlankTemplate = NULL;

	m_pFirstPosterDoc = NULL;

   m_InstDLL = NULL;

   m_fProcessPaletteMessages = TRUE;

   m_fSoundEffectsState = TRUE;

   m_eDateFormat = DT_SYSTEM_DEFAULT;

   m_pcrmAppSpecificResources = NULL;
	m_nPreventHub = 0;

	m_pUserSettings = NULL;
	m_pDriveManager = NULL;
	m_pPathManager = NULL;
	m_pCollectionManager = NULL;
#ifndef	ARTSTORE
	m_pContentServer = NULL;
#endif
	m_pContentManager = NULL;
	m_pConnectionManager = NULL;

	m_pSearchCollections = NULL;
}

CPmwApp::~CPmwApp()
{
   delete m_pScratchTemplate;

   // Normally I wouldn't check m_hFile, but Close() has a stupid
   // ASSERT for the file being open. Since the file may be open or
   // not at this point, I have to check even the code performs correctly
   // without it (except for the ASSERT).
   if (m_cfLockFile.m_hFile != CFile::hFileNull)
   {
      m_cfLockFile.Close();
   }

   // Delete any tmp files we created.
   for (int n = 0; n < m_csaTmpFileNames.GetSize(); n++)
   {
      TRY
      {
         CFile::Remove(m_csaTmpFileNames[n]);
      }
      END_TRY
   }

   delete m_pPortraitPrinterInfo;
   delete m_pLandscapePrinterInfo;
   delete m_pPortraitPaperInfo;
   delete m_pLandscapePaperInfo;
   delete last_new_info.m_pPaperInfo;

   delete m_pHelpfulHint;

	if (m_pBorderEditor)
	{
		if (m_pBorderEditor->IsInitialized())
			m_pBorderEditor->ShutdownBorderEditor();
		delete m_pBorderEditor;
	}

#ifndef WIN32
   delete m_pRecentFileList;
   m_pRecentFileList = NULL;
#endif

	delete m_pSearchCollections;

	// Get rid of the undo command database mapping since it references
	// a file (i.e. the path manager).
	CCommand::m_Mapping.FreeAll();

	// Release our interfaces.
#ifndef	ARTSTORE
	if (m_pContentServer != NULL)
	{
		m_pContentServer->InternalRelease();
		m_pContentServer = NULL;
	}
#endif
	// Release our interfaces.
	if (m_pConnectionManager != NULL)
	{
		// CWnd's don't delete themselves!
		VERIFY(m_pConnectionManager->InternalRelease() == 0);
		delete m_pConnectionManager;
		m_pConnectionManager = NULL;
	}

	if (m_pContentManager != NULL)
	{
		m_pContentManager->InternalRelease();
		m_pContentManager = NULL;
	}

	if (m_pCollectionManager != NULL)
	{
		m_pCollectionManager->InternalRelease();
		m_pCollectionManager = NULL;
	}

	if (m_pPathManager != NULL)
	{
		m_pPathManager->InternalRelease();
		m_pPathManager = NULL;
	}

	if (m_pDriveManager != NULL)
	{
		m_pDriveManager->InternalRelease();
		m_pDriveManager = NULL;
	}

	if (m_pUserSettings != NULL)
	{
		m_pUserSettings->InternalRelease();
		m_pUserSettings = NULL;
	}

   if (fCallExitWindows)
   {
      ExitWindows(dwExitWindowsCode, 0);
   }
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPmwApp object

CPmwApp NEAR theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {76F54460-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE PosterCLSID = 
{ 0x76f54460, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54461-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE CardCLSID = 
{ 0x76f54461, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54462-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE BannerCLSID =
{ 0x76f54462, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54463-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE CalendarCLSID =
{ 0x76f54463, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54464-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE LabelCLSID =
{ 0x76f54464, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

 // {76F54465-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE EnvelopeCLSID =
{ 0x76f54465, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54466-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE BizCardCLSID =
{ 0x76f54466, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54467-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE CertificateCLSID =
{ 0x76f54467, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54468-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE NoteCardCLSID =
{ 0x76f54468, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

 // {76F54469-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE FaxCoverCLSID =
{ 0x76f54469, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F5446A-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE StationeryCLSID =
{ 0x76f5446a, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F5446B-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE NewsletterCLSID =
{ 0x76f5446b, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F5446C-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE BrochureCLSID =
{ 0x76f5446c, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F5446D-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE HalfCardCLSID =
{ 0x76f5446d, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F5446E-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE WebPubCLSID =
{ 0x76f5446e, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F5446F-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE PostCardCLSID =
{ 0x76f5446f, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54470-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE StickerCLSID =
{ 0x76f54470, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54471-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE TShirtCLSID =
{ 0x76f54471, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {76F54472-046F-11cf-B79A-0000C0E9C528}
static const CLSID BASED_CODE PhotoProjectsCLSID =
{ 0x76f54472, 0x46f, 0x11cf, { 0xb7, 0x9a, 0x0, 0x0, 0xc0, 0xe9, 0xc5, 0x28 } };

// {52B5F441-2C89-11d2-860B-00A0C9C56CDE}
static const CLSID BASED_CODE CraftCLSID = 
{ 0x52b5f441, 0x2c89, 0x11d2, { 0x86, 0xb, 0x0, 0xa0, 0xc9, 0xc5, 0x6c, 0xde } };

/////////////////////////////////////////////////////////////////////////////
// CPmwApp initialization

// Find the PrintMaster main window. You know, the one with the
// property named CMainFrame::GetMainWindowProperty().

static HWND FindMainWindow(void)
{
   HWND hWndDesktop = ::GetDesktopWindow();
   HWND hWndChild;
   for (hWndChild = ::GetWindow(hWndDesktop, GW_CHILD);
                  hWndChild != NULL;
                  hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT))
   {
      HANDLE hData = ::GetProp(hWndChild, CMainFrame::GetMainWindowProperty());
      if (hData == (HANDLE)1)
      {
         break;
      }
   }
   return hWndChild;
}

static BOOL system_is_monochrome(void)
{
   HDC hDC = ::GetDC(NULL);
   if (hDC != NULL)
   {
      BOOL fIsMonochrome =  (::GetDeviceCaps(hDC, BITSPIXEL)*::GetDeviceCaps(hDC, PLANES)) <= 1;
      ::ReleaseDC(NULL, hDC);
      return fIsMonochrome;
   }
/* Assume not. */
   return FALSE;
}

typedef void (CALLBACK *VOIDFUNC)(void);
typedef BOOL (CALLBACK *FILTERFUNC)(MSG*);
typedef void (CALLBACK *IDLEFUNC)(void);

static m_fAppHidden = FALSE;
BOOL CPmwApp::InitInstance()
{
	m_fAppHidden = (RunEmbedded() || RunAutomated());

   ProfileAdd("Start InitInstance");
   ::SetHandleCount(100);

   AfxEnableControlContainer();

	AfxInitRichEdit();

   // JRO
   m_InstDLL = LoadLibrary ((LPCSTR) cbResourceDLLName);


#ifdef  WIN32
   if (m_InstDLL == NULL)
#else
   if (m_InstDLL < HINSTANCE_ERROR)
#endif
   {
//#ifdef  _DEBUG
      AfxMessageBox ("Can't find resource DLL");
//#endif
      m_InstDLL = NULL;
      return FALSE;
   }

   AfxSetResourceHandle (m_InstDLL);
   m_ResourceString.SetInstanceHandle (m_InstDLL);

	HDC hDC = GetDC(NULL);
	if(GetDeviceCaps(hDC, BITSPIXEL) < 8)
	{
		AfxMessageBox(IDS_INVALID_COLOR_DEPTH);
		return FALSE;
	}
	ReleaseDC(NULL, hDC);


	// GCB - enable the ImageGear GIF extension for the image component
	try
	{
		const char kGifExtension[] = "iglzw32s.dll" ;
		// cannot be const! It won't work with it - KDM
		char kGifLicense[]   = "BRODERBUND" ;
		char kGifAuth[]      = "T104XN8ANL8A05" ;
		RImageLibrary rLibrary;

		if (rLibrary.LoadExtension(kImageExtGIF, (LPCSTR)kGifExtension, (LPCSTR)kGifLicense, (LPCSTR)kGifAuth))
		{
//			m_bGifEnabled = TRUE;				
		}
		else
		{
			TRACE("IG_ext_load failed, GIFs are not enabled\n");
//			m_bGifEnabled = FALSE;
		}
//		if (!rLibrary.LoadExtension( kImageExtFPX, kFPXExtension, kFPXLicense, kFPXAuth ))
//		{
//			TRACE("IG_ext_load failed, Flash Pix format is not enabled\n");
//		}
	}
	catch (...)
	{
 		TRACE("IG_ext_load failed, GIFs are not enabled\n");
//		m_bGifEnabled = FALSE;
	}

/*
// First, check if the program is already running. If so, try to switch to
// the window of the existing instance. We don't run twice.
*/

   if (!m_fAppHidden && !CheckPreviousInstance())
   {
   /* Already running. */
      return FALSE;
   }

/*
// Initialize the OLE 2.0 libraries
*/

   ProfileAdd("Start AfxOleInit");
   if (!AfxOleInit())
   {
      AfxMessageBox(IDP_OLE_INIT_FAILED);
      return FALSE;
   }
   ProfileAdd("End AfterAfxOleInit");

/*
// Make sure we completely constructed.
*/

   CheckProcessorType();

	if(m_displayChanger.NeedToChangeMode())
	{
		if(m_displayChanger.ChangeMode())
			m_bChangedDevMode = TRUE;
		else
		{
			AfxMessageBox(IDS_INVALID_RESOLUTION);
			return FALSE;
		}
	}

/*
// Create the main MDI Frame window.
*/
	
   if (!CreateMainWindow())
   {
      return FALSE;
   }

   InitializeRenaissanceFramework( );

/*
// Win 32 only:
//
// At this point, we see if we have been launched to handle a file open
// (i.e. a DDE transaction). If so, we enter a special mode where we wait
// for the DDE transaction to happen. Otherwise, we fall through and finish
// normal initialization.
// PrintMaster takes so long to start up, that in the multi-tasking environment
// of Win 32, the one initiating DDE times out and fails. So we need to jump
// into a message loop right away and initialize later.
*/

   CString csCmdLine = m_lpCmdLine;

	BOOL bRetVal = FALSE;
	//check the command line to see if we are executing a dde command.
	
	//This is NOT working
	//if (csCmdLine.CompareNoCase("dde") != -1)

	if (!csCmdLine.IsEmpty())
	{
	   // Assume this is a document name coming in.
	   // We need to initiate DDE transfer.

		if (((CMainFrame*)m_pMainWnd)->WaitForDDETransfer())
		{
		  /*
		  // We are setup for the DDE transfer. Go to main loop now.
		  // Initialization will be completed later.
		  */
			 bRetVal = TRUE;
		}
	}
	else
	{
		//is there anything on the command line
		BOOL fHasCommandLine = (csCmdLine.GetLength() != 0);
		// Finish the initialization.	
		bRetVal = FinishInitialization(!fHasCommandLine);

		// If we have a command line, process it.
		if (fHasCommandLine)
		{
			if(bRetVal)
			{
				CCommandLineInfo cmdInfo;
				CWinApp::ParseCommandLine(cmdInfo);

				// Dispatch commands specified on the command line
				if (!ProcessShellCommand(cmdInfo))
					return FALSE;
			}
		}
	}

	/* Seed the random-number generator with current time so that
	* the numbers will be different every time we run.    */
   srand( (unsigned)time( NULL ) );
   

   return bRetVal;
}

// Maybe make an app member someday?

static CGlobalAtom AutoRunAtom;
static CGlobalAtom MainAppAtom;

BOOL CPmwApp::FinishInitialization(BOOL fNormalStartup /*=TRUE*/)
{
   ProfileAdd("Start FinishInitialization\n");

//
// Create the user settings.
//

	m_pUserSettings = new CPMUserSettings;

//
// Create the drive manager.
//

	m_pDriveManager = new CStdDriveManager;

//
// Create the path manager.
//

	m_pPathManager = new CStdPathManager;

//
// Create the collection manager.
//

	m_pCollectionManager = new CPMCollectionManager;

//
// Create the content server.
//
#ifndef	ARTSTORE
	m_pContentServer = new CStdContentServer;
#endif
//
// Create the content manager.
//

	m_pContentManager = new CContentManager;

//
// Create the connection manager.
//

	m_pConnectionManager = new CConnectionManager;

//
// Create the connection manager control.
//

   if (!m_pConnectionManager->Create(m_pMainWnd, IDC_CONNECTION_MGR))
	{
      CString  csMessage;
      csMessage.LoadString(IDS_ERR_CONNMGR_NOTFOUND);
      ASSERT(!csMessage.IsEmpty());
      AfxMessageBox(csMessage);

		TRACE("Warning: Failed to find Connection Manager component!\n");
		// Delete the object. Since we haven't passed it to anybody yet,
		// it doesn't have any refs, so we can safely delete it ourselves
		// without worrying about dangling references.
		delete m_pConnectionManager;
		m_pConnectionManager = NULL;
	}

/*
// Open the file subsystem.
// This allows us to build paths to our home directory (only!).
*/

   ProfileAdd("Start OpenFileSystem");
   if (!OpenFileSystem())
   {
      return FALSE;
   }
   ProfileAdd("End OpenFileSystem");

/*
// Remember our main INI filename.
*/

   m_csMainIni = GetPathManager()->ExpandPath("PMW.INI");

/*
// Initialize 3D controls.
*/

   Init3DControls();

/*
// Read the installation data.
*/
   if (CPrintMasterConfiguration::IsAddOn () == TRUE)
   {
      m_installation_info.nInstallConfiguration = CPrintMasterConfiguration::AddOn;
      m_installation_info.nCurrentConfiguration = CPrintMasterConfiguration::AddOn;   
   }
   else
   {
      ProfileAdd("Start ReadInstDat");
      if (!ReadInstDat())
      {
         return FALSE;
      }
      ProfileAdd("End ReadInstDat");
   }

/*
// Validate that we have some kind of configuration.
*/

   if (m_installation_info.nInstallConfiguration == 0
         || m_installation_info.nCurrentConfiguration == 0)
   {
      return ConfigurationError();
   }

/*
// Compute the configuration file name.
*/

   m_signature_file = CPrintMasterConfiguration::GetSignatureFile(m_installation_info.nInstallConfiguration);

   if (m_signature_file == NULL)
   {
      return ConfigurationError();
   }
   
/*
// Create our product configuration.
*/

   if (!CPrintMasterConfiguration::Create(m_installation_info.nCurrentConfiguration, GetPathManager()))
   {
      return ConfigurationError();
   }

   if(CreateResourceManager() != ERRORCODE_None)
      return ConfigurationError();

	//if(GetConfiguration()->SupportsBorderPlus(FALSE))
	m_pBorderEditor = new CBorderEditor;

   // If configuration doesn't use PrintMaster Icon, change Main Window Icon
   if(GetResourceManager() != NULL)
      {
         HICON hNewAppIcon = GetAppIcon();
         if(hNewAppIcon)
            ::SetClassLong(AfxGetMainWnd()->GetSafeHwnd(), GCL_HICON, (LONG)hNewAppIcon);
      }

	if(pOurPal == NULL)
	{
		initialize_palette();
	}
   CStartupDialog* pStartupDialog = NULL;
	if (!m_fAppHidden)
	{
		pStartupDialog = new CStartupDialog(m_pMainWnd);
	}

   // Set the ProductFlags in the collection.
   CPMWCollection::SetProductFlags(GetConfiguration()->ProductFlags());

   CString csMsregIni = GetPathManager()->ExpandPath("MSREG.INI");
   CIniFile MsregIniFile(csMsregIni);

#ifdef LOCALIZE
   // CB - check for locale info in msreg.ini
   CurrentLocale.SetLocale(MsregIniFile.GetInteger(SECTION_Locale, ENTRY_Language, LOCALE_ID_USENGLISH));
#endif


/*
// Get the Version string and build ID from MSREG.INI. Save these
// in the App object.
*/
  
   m_csVersion = MsregIniFile.GetString(SECTION_Configuration, ENTRY_Version);
   
   LPCSTR pszVersion = m_csVersion;
   m_nBuildId = 0;
   pszVersion = strchr(pszVersion, '.');
   if (pszVersion != NULL)
   {
      pszVersion = strchr(pszVersion+1, '.');
      if (pszVersion != NULL)
      {
         m_nBuildId = atoi(pszVersion+1);
      }
   }

/*
// Get the parent part number from MSREG.INI
*/

   m_csParentPartNumber = MsregIniFile.GetString(SECTION_Configuration, ENTRY_ParentPartNumber);
   
/*
// Get the volume label to use for our CD.
*/

   m_volume_id = CPrintMasterConfiguration::GetVolumeLabel(m_installation_info.nInstallConfiguration, GetBuildId());

/*
// Parse the command line.
*/

   ParseCommandLine();

/*
// Initialize the network code.
// This will give us the user name and number.
*/

   ProfileAdd("Start initialize_network");
   if (!initialize_network())
   {
      return FALSE;
   }
   ProfileAdd("End initialize_network");

/*
// Create the nid'd templates now that we have the user ID.
*/

   NidFilenames();

/*
// Set the user subdirectory.
*/
	CString csUserDirectory;
   if (!InitUserDirectory(csUserDirectory))
   {
      return FALSE;
   }

/*
// Setup the INI file name in the APP for this user.
// From this point on, we can read settings from the INI files.
*/

   setup_ini_file(csUserDirectory);

/*
// Read any debug settings. We try to do this early on so we can use od().
*/

   ReadDebugSection();

//
// Read the online settings.
//

	if (m_pConnectionManager != NULL)
	{
		CString csURL = OurGetProfileString("Online", "ConnectURL");
		if (!csURL.IsEmpty())
		{
			m_pConnectionManager->SetURL(csURL);
		}
	}

/*
// Create our atom to prevent AutoRun from taking place if the user has to
// insert the CD. Note that we can't read the MSREG.INI off the CD since
// that would require putting the CD in the drive. We want this to be
// in effect before putting the CD in.
*/

// CString csMsregIni = GetPathManager()->ExpandPath("MSREG.INI");
// CIniFile MsregIniFile(csMsregIni);

   CString csAtom = MsregIniFile.GetString(SECTION_Configuration, ENTRY_SkipAutoRunAtom);
   if (!csAtom.IsEmpty())
   {
      AutoRunAtom.Name(csAtom);
      AutoRunAtom.Create();         // If it fails.. oh, well.
   }

   // Create an atom that has same name regardless of Product Line
   MainAppAtom.Name("PrintMasterEngine");
   MainAppAtom.Create();

/*
// Now that the INI files are available, we can initialize INI related info.
*/

   ProfileAdd("Start InitFileSystem");
   if (!InitFileSystem(csUserDirectory))
   {
      return FALSE;
   }
   ProfileAdd("End InitFileSystem");

/*
// If this is the demo (trial) version, see if we have any counts left.
*/

   if (GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::Demo)
   {
      BOOL fRun = TRUE;

      if (m_installation_info.nTrialCount == 0)
      {
      /* They have run out of times to run. */
         fRun = FALSE;
      }
      else
      {
      // Some counts left. Decrement it now.
         m_installation_info.nTrialCount--;
         if (!m_installation_info.Put())
         {
         /* Can't write count. Pretend they have run out of times to run. */
            fRun = FALSE;
         }
      }

      if (!fRun)
      {
         // JRO if (!GetConfiguration()->UnsupportedDialog(szTrialVersionExpired1, szTrialVersionExpired2))
         if (!GetConfiguration()->UnsupportedDialog(GetResourceStringPointer (uTrialVersionExpired1), GetResourceStringPointer (uTrialVersionExpired2)))
         {
            return FALSE;
         }
      }
   }

/*
// Read various INI file sections.
*/

   ReadConfigurationSection();
   ReadImageSection();
//   ReadBarsSection();
   ReadPrinterSection();
   ReadDesktopSection();

	// Set the log file name for the content server.
	CIniFile PMGSIniFile(m_pPathManager->ExpandPath("PMGS.INI"));
	CString csLogFile = PMGSIniFile.GetString("Debug", "LogFile");
	if (!csLogFile.IsEmpty())
	{
		csLogFile = m_pPathManager->ExpandPath(csLogFile);
	}
	
	m_pContentServer->SetLogFileName(csLogFile);

	// read in Helpful Hints (including user's configuration file)
   m_pHelpfulHint->LoadHelpfulHints();
   m_fHintFullLengthBmp = OurGetProfileInt(SECTION_Configuration, ENTRY_HintFullLengthBmp, 0);

/*
// Upgrade our icon if necessary.
*/

   UpgradeIcon(TRUE);            // But only if necessary.

/*
// Read the photo cd resolutions.
*/

   PCDHelper::m_wDraftResolution = OurGetProfileInt(SECTION_PhotoCD, ENTRY_Draft, PCDHelper::m_wDraftResolution);
   PCDHelper::m_wProofResolution = OurGetProfileInt(SECTION_PhotoCD, ENTRY_Proof, PCDHelper::m_wProofResolution);

	// Set the default resolutions for various formats.
	BMPHelper::m_pntResolution = GetConfiguration()->GetDefaultBMPResolution();
	GIFHelper::m_pntResolution = GetConfiguration()->GetDefaultGIFResolution();
	JPEGHelper::m_pntResolution = GetConfiguration()->GetDefaultJPGResolution();
	PCXHelper::m_pntResolution = GetConfiguration()->GetDefaultPCXResolution();
	PNGHelper::m_pntResolution = GetConfiguration()->GetDefaultPNGResolution();

/*
// Register our custom controls.
*/

   RegisterMyControls();

/*
// Check if file sharing is available. If not, then inform the user.
*/

   BOOL fSkipShareTest = OurGetProfileInt(SECTION_Configuration, ENTRY_SkipShareTest, FALSE);

   if (!fSkipShareTest && !Util::FileSharingAvailable())
   {
      CShareNotLoadedDialog ShareNotLoadedDialog;
      if (ShareNotLoadedDialog.DoModal() == IDOK)
      {
         // Report what happened.
         if (ShareNotLoadedDialog.m_fAutoexecBatModified)
         {
            CAutoexecBatModifiedDialog AutoexecBatModifiedDialog(ShareNotLoadedDialog.m_csBackupName);
            
            if (AutoexecBatModifiedDialog.DoModal() == IDOK)
            {
               // Make sure system will be rebooted.
               fCallExitWindows = TRUE;
#ifndef EW_REBOOTSYSTEM // GCB 11/9/98 - VC6 winuser.h no longer defines this
	#define EW_REBOOTSYSTEM 0x0043L
#endif
               dwExitWindowsCode = EW_REBOOTSYSTEM;
            }
         }
         else
         {
            CCannotModifyAutoexecBatDialog CannotModifyAutoexecBatDialog(ShareNotLoadedDialog.m_csReason);
            CannotModifyAutoexecBatDialog.DoModal();
         }
      }

      return FALSE;
   }

/*
// Read the color list.
*/

   ProfileAdd("Start system_color_list.read");
   system_color_list.read("PMW.CLR");
   ProfileAdd("End system_color_list.read");

/*
// Startup the registration DLL.
// If this fails, we will continue anyway.
*/

   ProfileAdd("Start RegisterDLL.Startup");
   RegisterDLL.Startup();
   ProfileAdd("End RegisterDLL.Startup");

/*
// Make sure the free files are registered if the user has already registered.
*/

   if (m_first_time_installation && user_is_registered())
   {
   /* Make sure the free files are now available. */
      register_free_files();
   }
/*
#ifdef WIN32
   if (!PmwThunk.Initialize())
   {
      AfxMessageBox(IDS_NO_THUNK, MB_OK | MB_ICONEXCLAMATION);
      return FALSE;
   }
#endif
*/
/*
// Initialize the document engine.
// We're guaranteed that the callback function won't be called while in
// init_document_engine().
*/

   ProfileAdd("Start init_document_engine");
   if (!init_document_engine(typeface_server.get_face_path, GetPathManager()->GetWorkingDirectory()))
   {
      // JRO AfxMessageBox(szInsufficientMemory, MB_OK | MB_ICONEXCLAMATION);
      // JRO Maybe this will free enough for message box
      m_ResourceString.Kill ();

      AfxMessageBox(GetResourceStringPointer (uInsufficientMemory), MB_OK | MB_ICONEXCLAMATION);
      return FALSE;
   }
   ProfileAdd("End init_document_engine");

/*
// Initialize the workspace file.
*/

   ProfileAdd("Start InitializeWorkspace");
   InitializeWorkspace();
   ProfileAdd("End InitializeWorkspace");

/*
// Initialize the font and typeface subsystem.
*/

   ProfileAdd("Start InitializeFonts");
   if (!InitializeFonts())
   {
      return FALSE;
   }
   ProfileAdd("End InitializeFonts");

/*
// Miscellaneous initialization.
*/

   LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// initialize the Border Editor
	if (m_pBorderEditor)
		m_pBorderEditor->StartupBorderEditor();

	//determine if we are in large fonts before we finish the creation of the main window
	CWnd*  pDesktop = CWnd::GetDesktopWindow();	
	if(pDesktop)
	{
		CDC* pDC = pDesktop->GetWindowDC();
		if(pDC)
		{
			int nOldMode = pDC->SetMapMode(MM_TEXT);
			CFont* pFont = pDC->GetCurrentFont();
			if(pFont)
			{
				LOGFONT lf;
				pFont->GetLogFont(&lf);
				if(lf.lfHeight > 16)
					m_bLargeFontMode = TRUE;
			}

			pDC->SetMapMode(nOldMode);
			pDesktop->ReleaseDC(pDC);
		}
	}


/*
// Finish creating the main window.
// This creates the tool bars, menu, etc.
*/

   ProfileAdd("Start FinishCreate");
   ((CMainFrame*)m_pMainWnd)->FinishCreate();
   ProfileAdd("End FinishCreate");

/*
// Create the scratch template.
// Newsletter is the most flexible generic project.
*/

   ProfileAdd("Start Create the scratch template");
   ASSERT(m_pScratchTemplate == NULL);
   m_pScratchTemplate = new CPmwDocTemplate(IDR_NEWSLETTERTYPE,
                                            RUNTIME_CLASS(CNewsletterDoc),
                                            RUNTIME_CLASS(CPmwMDIChild),
                                            RUNTIME_CLASS(CNewsletterView));
   ProfileAdd("End Create the scratch template");

/*
// Register the application's document templates.  Document templates
//  serve as the connection between documents, frame windows and views.
*/

   ProfileAdd("Start RegisterDocTemplates");
   RegisterDocTemplates();
   ProfileAdd("End RegisterDocTemplates");

/*
// Register these document templates with the registry.
*/

   ProfileAdd("Start RegisterShellFileTypes");
   RegisterShellFileTypes();
   ProfileAdd("End RegisterShellFileTypes");

/*
// Open the dialog manager.
// This needs to be done after the main window is created since the dialog
// manager needs the handle of the main window.
*/

   ProfileAdd("Start DialogManager.Open");
   DialogManager.Open(GetPathManager()->GetWorkingDirectory(), NULL);
   ProfileAdd("End DialogManager.Open");

/*
// Make sure the common screen gamma curve is built.
*/

   RedisplayContext::build_gamma_curve(screen_gamma_curve, 1000);

/*
// Get the paper sizes from the printer driver.
*/

   ProfileAdd("Start UpdatePrinterSelection");
   UpdatePrinterSelection(m_hDevNames == NULL); //force default if no current
   ProfileAdd("End UpdatePrinterSelection");

   ProfileAdd("Start update_paper_info");
   update_paper_info();
   ProfileAdd("End update_paper_info");

	//
	// Create the art search collections.
	//

	m_pSearchCollections = new CSearchArtCollections(GetPathManager()->ExpandPath("[[S]]\\SRCHART.INI"));
	m_pSearchCollections->Update();

   // Open the collection manager.
   m_pCollectionManager->Open(GetPathManager(), user_is_registered());

/*
// We are now completely initialized.
*/

   m_fInitialized = TRUE;

   // Get rid of the startup dialog now.
   // The destructor would do this, but we want it to happen at this point.
   // So we force it.

	if (pStartupDialog != NULL)
	{
		pStartupDialog->DestroyWindow();
		delete pStartupDialog;
	}

// Startup the MCI window.
   m_MciWnd.init();

	// Always register the automation.
	COleTemplateServer::RegisterAll();

	// Open the content manager.
	m_pContentManager->Open(GetPathManager(),
									GetUserSettings(),
									GetConnectionManager(),
									GetContentServer(),
									GetParentPartNumber(),
									GetVersion());

	// allow prompting for HomeCD now if InitFileSystem had turned it off
	bHomeCDNotAvailable = FALSE;

///////////////////////////////////////////////////////////////////////////
// End of necessary initialization before running embedded or automated. //
///////////////////////////////////////////////////////////////////////////

/*
// Parse the command line to see if launched as OLE server
// If so, we just return at this point and let things happen as necessary
// under such an OLE case.
*/

//   if (RunEmbedded() || RunAutomated())
   if (m_fAppHidden)
   {
      /*
      // Application was run with /Embedding or /Automation.  Don't show the
      //  main window in this case.
      */
//		COleObjectFactory::RegisterAll();
      return TRUE;
   }

///////////////////////////////////////////////////////////
// We are being launched stand-alone.                    //
// Prepare the interface and show any necessary dialogs. //
///////////////////////////////////////////////////////////

   // When a server application is launched stand-alone, it is a good idea
   //  to update the system registry in case it has been damaged.
// m_server.UpdateRegistry(OAT_INPLACE_SERVER);
	COleObjectFactory::UpdateRegistryAll();

/*
// If the show flag is NORMAL, load as we were saved last time.
// If the show flag is not NORMAL, show us as it says.
*/

   if (LoadWindowPosition(m_pMainWnd, "Frame", m_nCmdShow, FALSE, FALSE) == FALSE)
   {
      //((CMainFrame*)m_pMainWnd)->SizeToSmallestBest ();   
   }

   if (OurGetProfileInt(SECTION_Configuration, ENTRY_ShowMovie, TRUE))
   {
      WriteProfileInt(SECTION_Configuration, ENTRY_ShowMovie, FALSE);

		// Get the name of the movie to play.
		CString csMovie = OurGetProfileString(SECTION_Configuration, ENTRY_Movie, "[[H]]\\runtime\\welcome.smk");

      SetProcessPaletteMessages(FALSE);
//j      CSmacker Smacker(m_pMainWnd, GetPathManager()->LocatePath(csMovie));
//j      Smacker.PlayMovie(FALSE);
      SetProcessPaletteMessages(TRUE);

      WriteProfileInt(SECTION_Configuration, ENTRY_ShowMovie, TRUE);
   }

    // Restore the state of the various control bars.
	// All the bars are created hidden by default. We show them
	// now to get them to their true default state (shown) and then
	// call LoadBarState() to move them to their final positions.
//	((CMainFrame*)m_pMainWnd)->FileBar().ShowWindow(SW_SHOW);
//	((CMainFrame*)m_pMainWnd)->FormatBar().ShowWindow(SW_SHOW);
//	((CMainFrame*)m_pMainWnd)->Edit1Bar().ShowWindow(SW_SHOW);
//	((CMainFrame*)m_pMainWnd)->Edit2Bar().ShowWindow(SW_SHOW);
	((CMainFrame*)m_pMainWnd)->ZoomGadget().ShowWindow(SW_SHOW);
	((CMainFrame*)m_pMainWnd)->HelpWindow().ShowWindow(SW_SHOW);

	((CMainFrame*)m_pMainWnd)->LoadBarState("BarState");

	if (!((CMainFrame*)m_pMainWnd)->HelpWindow().IsValid())
	{
		((CMainFrame*)m_pMainWnd)->HelpWindow().ShowWindow(SW_HIDE);
	}

/*
// If there are any documents to be opened, do them now.
*/

   int nDocsToOpen = m_csaDocsToOpen.GetSize();
   if (nDocsToOpen != 0)
   {
      for (int nDoc = 0; nDoc < nDocsToOpen; nDoc++)
      {
		char* pCommand = new char[m_csaDocsToOpen[nDoc].GetLength() + 1];
		strcpy(pCommand, (LPCTSTR)m_csaDocsToOpen[nDoc]);
		CWinApp::OnDDECommand(pCommand);
		delete pCommand;
		}
      m_csaDocsToOpen.RemoveAll();

      return TRUE;
   }

/*
// If this is a demo (aka Trial Version) then let the user know.
*/

   if (GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::Demo)
   {
      CDemoDescriptionDialog Dialog(m_pMainWnd);
      Dialog.DoModal();
   }

/*
// If this is a first time installation and the user is not currently
// registered, then remind the user to register and of the treasures that
// will be showered upon him for doing so.
*/

   RemindUser();

/*
// Ask the user to provide some sender information if they haven't already.
*/

   if (GetConfiguration()->SupportsAddressBook(FALSE))
   {
      CPersonalSenderInformationDialog::Run(m_pMainWnd);
   }

/*
// Put up the main menu and proceed!
*/
	if (fNormalStartup)
	{
		// Create the initial poster
		last_new_info.pTemplate = m_pPosterTemplate;
		m_pFirstPosterDoc = (CPmwDoc*)last_new_info.pTemplate->OpenDocumentFile(NULL);
		m_pFirstPosterDoc->SetModifiedFlag(FALSE);
	}

   ProfileAdd("End FinishInitialization");

	m_bAgRemotePrint = OurGetProfileInt(SECTION_Configuration, "RemotePrint", 0);

	if(m_bAgRemotePrint)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		HANDLE hInitDone = CreateEvent(&sa, FALSE, FALSE, "AG_DDE_Event");
		SetEvent(hInitDone);
		CloseHandle(hInitDone);
	}

	if (fNormalStartup)
	{
		((CMainFrame*)m_pMainWnd)->RequestHubWindow();
	}

   return TRUE;
}

CConnectionManager* CPmwApp::GetHiddenConnectionManager()
{
   return m_pConnectionManager;
}

BOOL CPmwApp::ConfigurationError(void)
{
   // JRO AfxMessageBox(szCannotDetermineConfiguration, MB_OK | MB_ICONEXCLAMATION);
   AfxMessageBox(GetResourceStringPointer (uCannotDetermineConfiguration), MB_OK | MB_ICONEXCLAMATION);
   return FALSE;
}

void CPmwApp::UpdateLicensedProducts(void)
{
	// Add product codes in all project collection to the running mask
	// maintained in the configuration object.
	ICollectionManager* pCollectionManager = GetCollectionManager();
	int i;
	int nNumCollections = pCollectionManager->NumberOfCollections(CPMWCollection::typeProjects, CPMWCollection::subTypeNone);
	for (i = 0; i < nNumCollections; i++)
	{
      CPMWCollection* pCollection = 
			pCollectionManager->GetCollection(i, CPMWCollection::typeProjects, CPMWCollection::subTypeNone);

		ASSERT(pCollection != NULL);
		if (pCollection != NULL)
		{
			CString csProductCode = pCollection->GetProductCode();
			DWORD dwProjectTypes = pCollection->GetProjectTypes();

			// If the product is licensed, add it
			if (LicenseData.ProductIsLicensed(csProductCode))
			{
				GetConfiguration()->AddProjectType(dwProjectTypes);
			}
		}
	}

	// Register any new document templates
	RegisterDocTemplates();
}

int CPmwApp::ExitInstance()
{
	RestoreDisplayMode();
	
	ShutdownRenaissanceFramework();

   WriteAddressBook();

	// write Helpful Hints user configuration file
	m_pHelpfulHint->SaveHintsUserConfig();

   exit_document_engine();

   delete pFontCache;

   // Get rid of the configuration.
   CPrintMasterConfiguration::Destroy();

   DestroyResourceManager();

   // Shutdown the registration DLL.
   RegisterDLL.Shutdown();

   cd_cache.close();

	// Close the content manager
	if (m_pContentManager != NULL)
	{
		m_pContentManager->Close();
	}

   // Close the collection manager
	if (m_pCollectionManager != NULL)
	{
		m_pCollectionManager->Close();
	}

   // Close the path manager.
	if (m_pPathManager != NULL)
	{
		m_pPathManager->Close();
	}

   // Close the drive manager.
	if (m_pDriveManager != NULL)
	{
		m_pDriveManager->Close();
	}

/* Unregister any classes created for the address book. */
   CAddressBook::UnregisterAddressBookClasses();

/* Unregister any classes created for the tree combo box. */
   CTreeComboBox::UnregisterClass();

   CProgressBar::UnregisterMyClass();

   m_MciWnd.bye ();

   // free Print Shop's wizard, if necessary
   if (m_hPSLaunchDLL != NULL)
   {
      FreeLibrary(m_hPSLaunchDLL);
   }

   // Free PMW Resources - do not do anything with resources after this point!
   if (m_InstDLL != NULL)
   {
      FreeLibrary(m_InstDLL);
   }

   // MRU is handled automatically in 32-bit.
   // Do it by hand for 16-bit.
#ifndef WIN32
   // Empty the old-style MRU names to prevent their being written.
   // Emptying the first is sufficient.
   m_strRecentFiles[0].Empty();

   // And write out our recent file list.
   if (m_pRecentFileList != NULL)
      m_pRecentFileList->WriteList();
#endif


   return CWinApp::ExitInstance();     // Call the default.
}

// GetAppIcon currently retrieves only product specific icon
// If product type is PrintMaster, no icon is returned
HICON CPmwApp::GetAppIcon()
   {
   HICON             hAppIcon = NULL;
   CResourceLoader   rlProductSpecific;

   if(GetResourceManager() == NULL)
      return hAppIcon;

   // Start using product specific resources
   rlProductSpecific.Attach(GetResourceManager());

   // Get Program Icon from App specific resource DLL
   // Note: DLL must remain in memory for lifespan of application

   // DoneWithResources() restores where resources are loaded from 
   // by making a call to AfxSetResourceHandle()
   // Note: DLL Still remains resident after this call
   // DoneWithResources() is called when CResourceLoader object is destroyed
   hAppIcon = rlProductSpecific.LoadIcon(CSharedIDResourceManager::riAppIcon);
#if 0
            if(pcrmAppSpecificResources->IsKindOf(RUNTIME_CLASS(CPrintResourceManager)))
            {
               TRACE("IsKindOf CPrintResourceManager");
            }
            else if(pcrmAppSpecificResources->IsKindOf(RUNTIME_CLASS(CCraftResourceManager)))
            {
               TRACE("IsKindOf CCraftResourceManager");
            }
            else if(pcrmAppSpecificResources->IsKindOf(RUNTIME_CLASS(CSpiritResourceManager)))
            {
               TRACE("IsKindOf CSpiritResourceManager");
            }
#endif
      return hAppIcon;
   }

CResourceManager * CPmwApp::GetResourceManager()
   {
      return m_pcrmAppSpecificResources;
   }

ERRORCODE CPmwApp::CreateResourceManager()
   {
//      switch(m_installation_info.nCurrentConfiguration)
      switch(GetConfiguration()->ProductLevel())
         {
            case CPrintMasterConfiguration::PrintStandard:
            case CPrintMasterConfiguration::PrintPremium:
               m_pcrmAppSpecificResources = (CResourceManager *) new CPrintResourceManager;
               break;
            case CPrintMasterConfiguration::CraftStandard:
            case CPrintMasterConfiguration::CraftDeluxe:
            case CPrintMasterConfiguration::CreataParty:
              m_pcrmAppSpecificResources = (CResourceManager *) new CCraftResourceManager;
               break;
            case CPrintMasterConfiguration::SpiritStandard:
               m_pcrmAppSpecificResources = (CResourceManager *) new CSpiritResourceManager;
               break;
#if 0
            default:
               ASSERT(0);
               return ERRORCODE_Fail;
               break;
#endif
         }
      return ERRORCODE_None;
   }

void CPmwApp::DestroyResourceManager()
   {
      if(m_pcrmAppSpecificResources)
         delete m_pcrmAppSpecificResources;
   }

/*
// Handle a multiple run of this program.
*/

BOOL CPmwApp::CheckPreviousInstance(void)
{
   HWND hPrevWnd;

   if ((hPrevWnd = FindMainWindow()) != NULL)
   {
		// Tell the other app to wake up.
		if (!::SendMessage(hPrevWnd, WM_WAKEAPP, (WPARAM)0, (LPARAM)0))
		{
			// Other app did not wake. Wake it ourselves.
			WakeWindow(CWnd::FromHandle(hPrevWnd));
		}
		return FALSE;
	}
	return TRUE;
}

/*
// Open the file system components.
*/

BOOL CPmwApp::OpenFileSystem(void)
{
/*
// Initialize the drive manager.
// This determines all drive types and sets up a buffer for volume label reads.
*/

	if (m_pDriveManager == NULL || !m_pDriveManager->Open())
   {
      return FALSE;
   }

/*
// Initialize the path manager.
// This does things like read the current working directory and establish
// the relative paths for all the application files.
*/

	if (m_pPathManager == NULL || !m_pPathManager->Open())
	{
		return FALSE;
	}

/*
// Hook in to the StorageFile class.
*/

	StorageFile::SetPathManager(GetPathManager());

   return TRUE;
}

/*
// Init the file system from the INI files.
*/
BOOL CPmwApp::InitFileSystem(LPCSTR pszUserDirectory)
{
	// Get interfaces to pass to the path manager.
	IUserSettings* pIUserSettings = GetUserSettings();
	IDriveManager* pIDriveManager = GetDriveManager();
	ICollectionManager* pICollectionManager = GetCollectionManager();
	IContentManager* pIContentManager = GetContentManager();
	IMediaPromptNotify* pIMediaPromptNotify = GetMediaPromptNotify();
	ASSERT(pIUserSettings != NULL);
	ASSERT(pIDriveManager != NULL);
	ASSERT(pIContentManager != NULL);

	BOOL fResult = FALSE;

	if (pIUserSettings != NULL && pIDriveManager != NULL)
	{
		// Initialize the drives.
		m_pDriveManager->InitDrives(pIUserSettings);

		// Initialize the path manager.
		m_pPathManager->InitPaths(pIUserSettings,
										  pIDriveManager,
										  pICollectionManager,
										  pIContentManager,
										  pIMediaPromptNotify,
										  get_volume_id(),
										  get_signature_file(),
										  pszUserDirectory,
										  GetConfiguration()->GetHomeCDRoot());

		//
		// Initialize the picture search paths.
		//

		InitializePictureSearchPaths();

		/*
		// Make sure we can find the home CD.
		*/

		if (!m_pPathManager->FindHomeCD())
		{
			// set flag to prevent repeated Home CD prompting during startup
			bHomeCDNotAvailable = TRUE;
		}

		fResult = TRUE;
	}

	return fResult;
}

void CPmwApp::InitializePictureSearchPaths(void)
{
/*
// Read and build the picture search path array.
*/

	CString picture_search_path = GetUserSettings()->GetString(SECTION_Paths, ENTRY_PictureSearchPath, "");

	int length;

	while ((length = picture_search_path.GetLength()) != 0)
	{
		int index;

	/* Try to find the separator. */

		if ((index = picture_search_path.Find(';')) == -1)
		{
		/* No more substrings. Suck up to the end. */
			index = length;
		}

	/* Create the new string. */

		char buffer[_MAX_PATH];

		strncpy(buffer, (PCSTR)picture_search_path, index);
		buffer[index] = '\0';
		strupr(buffer);

		char *p;

		while ((p = strchr(buffer, '/')) != NULL)
		{
		/* Make sure we like the slashes. */
			*p = '\\';
		}
		
		if (buffer[0] != '\0')
		{
			int len = strlen(buffer);
			if (buffer[len-1] != '\\' && buffer[len-1] != ':')
			{
				buffer[len++] = '\\';
				buffer[len] = '\0';
			}

			TRY
			{
				m_csaPictureSearchPaths.Add(buffer);
			}
			CATCH_ALL (e)
			{
				break;
			}
			END_CATCH_ALL
		}

	/* See if we need to stop. */

		if (index == length)
		{
			break;
		}

	/* Remove the part we just used. */

		picture_search_path = picture_search_path.Right(length - (index+1));
	}
}

/*
// Initialize the 3D controls.
*/

void CPmwApp::Init3DControls(void)
{
   if (system_is_monochrome())
   {
      SetDialogBkColor(RGB(255, 255, 255));
   }
   else
   {
      SetDialogBkColor();        // set dialog background color to gray
   }
}

void CPmwApp::RegisterDocTemplate(CDocTemplate*& pTemplate,
                                  UINT nIDResource,
                                  CRuntimeClass* pDocClass,
                                  CRuntimeClass* pFrameClass,
                                  CRuntimeClass* pViewClass,
                                  const CLSID& clsid,
                                  int nIconIndex)
{
/*
// If the the template is already created, there is nothing to do.
*/

   if (pTemplate != NULL)
   {
      return;
   }

/*
// If we don't support it, don't create the template.
*/

   if (!GetConfiguration()->SupportsProjectType(nIDResource, FALSE))
   {
      return;
   }

/*
// Create the document template for this type.
*/

   pTemplate = new CPmwDocTemplate(nIDResource,
                                   pDocClass,
                                   pFrameClass,
                                   pViewClass);
   AddDocTemplate(pTemplate);

#ifdef WINLOGO
   pTemplate->SetContainerInfo(IDR_PMWTYPE_CNTR_IP);

   // convert the CLSID to a string
   LPCSTR pszCLSIDObject;
   LPTSTR lptCLSIDObject;
#ifdef WIN32
   USES_CONVERSION;
   ::StringFromCLSID(clsid, (LPOLESTR*)&lptCLSIDObject);
   pszCLSIDObject = OLE2CT((LPOLESTR)lptCLSIDObject);
#else
   ::StringFromCLSID(clsid, (LPSTR*)&lptCLSIDObject);
   pszCLSIDObject = (LPSTR)lptCLSIDObject;
#endif

   if (pszCLSIDObject != NULL)
   {
   /*
   // Now register the icon.
   */

      HKEY hKey;
      LONG lRes = 0L;

      static char BASED_CODE lpszCLSID[]     = "CLSID";      // OLE Class ID Section of eg DB
      static char BASED_CODE lpszDefIcon[]   = "\\DefaultIcon"; // Default Icon Subkey
      char lpBuffer[64];                  // Working buffer

      // get path name to server
      CString strPathName;

#if 1
      // Icons are now in the resource DLL.
      strPathName = GetPathManager()->ExpandPath(cbResourceDLLName);
#else
#ifdef WIN32
      AfxGetModuleShortFileName(AfxGetInstanceHandle(), strPathName);
#else
      LPSTR pPath = strPathName.GetBuffer(_MAX_PATH);
      ::GetModuleFileName(AfxGetInstanceHandle(), pPath, _MAX_PATH);
      strPathName.ReleaseBuffer();
#endif
#endif

      /* Icon path is "server,icon#" */
      CString csIconPath = strPathName;
      csIconPath += ',';
      csIconPath += itoa(nIconIndex, lpBuffer, 10);

      // Get the file type id.
      CString strFileTypeId;
      if (pTemplate->GetDocString(strFileTypeId, CDocTemplate::regFileTypeId)
            && !strFileTypeId.IsEmpty())
      {
         // Get the human-readable type name.
         CString strFileTypeName;
         if (!pTemplate->GetDocString(strFileTypeName,
                                      CDocTemplate::regFileTypeName))
         {
            strFileTypeName = strFileTypeId;    // use id name
         }

         // Open the reg db for the "CLSID" key, this top-level key
         // is where OLE2 and other OLE aware apps will look for
         // this information.

         lRes = RegOpenKey(HKEY_CLASSES_ROOT, lpszCLSID, &hKey);

         if (lRes == ERROR_SUCCESS)
         {
         /*
         // Register the Object
         //
         // First step is to add the type to the registry.
         // The type is the "type id" defined by the doc template.
         // The name of the type is the human-readable "type name" defined by
         // the doc template.
         */

            //>>> Add "root\\<type>=<long name>.

            lRes = RegSetValue(HKEY_CLASSES_ROOT,
                               strFileTypeId,
                               REG_SZ,
                               strFileTypeName,
                               strFileTypeName.GetLength());

            if (lRes == ERROR_SUCCESS)
            {
               /*
               // The second step is to add the CLSID to the newly added
               // type key.
               // Build "classid" subkey string  "{ <type> }\\CLSID"
               */

               sprintf(lpBuffer,
                       "%s\\%s",
                       (LPCSTR)strFileTypeId,
                       (LPCSTR)lpszCLSID);

               //>>> Add "root\<type>\CLSID=<clsid>.

               lRes = RegSetValue(HKEY_CLASSES_ROOT,
                                  lpBuffer,
                                  REG_SZ,
                                  pszCLSIDObject,
                                  lstrlen(pszCLSIDObject));

            /*
            // Set the value of the CLSID Entry to the Human Readable 
            // name of the Object.
            */

               //>>> Add "root\CLSID\<clsid>=<long name>.
               lRes = RegSetValue(hKey,
                                  pszCLSIDObject,
                                  REG_SZ,
                                  strFileTypeName,
                                  strFileTypeName.GetLength());

               if (lRes == ERROR_SUCCESS)
               {   
                  /*
                  // Build "defaulticon" subkey string  "{ <class id> }\\DefaultIcon"
                  */
                  lstrcpy (lpBuffer, pszCLSIDObject);
                  lstrcat (lpBuffer, lpszDefIcon);                          

                  /*
                  // Set Object's default icon entry to point to the 
                  // default icon for the object                            
                  */

                  //>>> Add "root\CLSID\<clsid>\DefaultIcon=<server,icon#>.
                  lRes = RegSetValue(hKey,
                                  lpBuffer,
                                  REG_SZ,
                                  csIconPath,
                                  csIconPath.GetLength());
               }
            }

         // Close the reg db.

            RegCloseKey(hKey);   
         }

#ifdef WIN32
         AfxFreeTaskMem(lptCLSIDObject);
#else
         _AfxFreeTaskMem(lptCLSIDObject);
#endif
      }
   }

#endif
}

static void SetTemplateProjectType(CDocTemplate* pTemplate, int nType)
{
   if (pTemplate != NULL)
   {
      ((CPmwDocTemplate*)pTemplate)->ProjectType(nType);
   }
}

/*
// Register the document templates.
*/
void CPmwApp::RegisterDocTemplates(void)
{
// JRO The icon index (last parameter of RegisterDocTemplate()) wants to be the index 
// of the project's strings in the string IDS_PROJECT_TYPES 
// AND the PROJECT_TYPE enum (in PROJECT.H) 
   RegisterDocTemplate(m_pBlankTemplate,
                       IDR_BLANKTYPE,
                       RUNTIME_CLASS(CPosterDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CPosterView),
                       PosterCLSID,
                       21);

   RegisterDocTemplate(m_pPosterTemplate,
                       IDR_POSTERTYPE,
                       RUNTIME_CLASS(CPosterDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CPosterView),
                       PosterCLSID,
                       1);

   RegisterDocTemplate(m_pCardTemplate,
                       IDR_CARDTYPE,
                       RUNTIME_CLASS(CCardDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CCardView),
                       CardCLSID,
                       2);

   RegisterDocTemplate(m_pHalfCardTemplate,
                       IDR_HALFCARDTYPE,
                       RUNTIME_CLASS(CHalfCardDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CHalfCardView),
                       HalfCardCLSID,
                       14);

   RegisterDocTemplate(m_pBannerTemplate,
                       IDR_BANNERTYPE,
                       RUNTIME_CLASS(CBannerDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CBannerView),
                       BannerCLSID,
                       3);

   RegisterDocTemplate(m_pCalendarTemplate,
                       IDR_CALENDARTYPE,
                       RUNTIME_CLASS(CCalendarDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CCalendarView),
                       CalendarCLSID,
                       4);

   RegisterDocTemplate(m_pLabelTemplate,
                       IDR_LABELTYPE,
                       RUNTIME_CLASS(CLabelDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CLabelView),
                       LabelCLSID,
                       5);

   RegisterDocTemplate(m_pEnvelopeTemplate,
                       IDR_ENVELOPETYPE,
                       RUNTIME_CLASS(CEnvelopeDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CEnvelopeView),
                       EnvelopeCLSID,
                       6);

   RegisterDocTemplate(m_pBizCardTemplate,
                       IDR_BIZCARDTYPE,
                       RUNTIME_CLASS(CBizCardDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CBizCardView),
                       BizCardCLSID,
                       7);

   RegisterDocTemplate(m_pCertificateTemplate,
                       IDR_CERTIFICATETYPE,
                       RUNTIME_CLASS(CCertificateDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CCertificateView),
                       CertificateCLSID,
                       8);

   RegisterDocTemplate(m_pNoteCardTemplate,
                       IDR_NOTECARDTYPE,
                       RUNTIME_CLASS(CNoteCardDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CNoteCardView),
                       NoteCardCLSID,
                       9);

   RegisterDocTemplate(m_pFaxCoverTemplate,
                       IDR_FAXCOVERTYPE,
                       RUNTIME_CLASS(CFaxCoverDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CFaxCoverView),
                       FaxCoverCLSID,
                       10);

   RegisterDocTemplate(m_pStationeryTemplate,
                       IDR_STATIONERYTYPE,
                       RUNTIME_CLASS(CStationeryDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CStationeryView),
                       StationeryCLSID,
                       11);

   RegisterDocTemplate(m_pNewsletterTemplate,
                       IDR_NEWSLETTERTYPE,
                       RUNTIME_CLASS(CNewsletterDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CNewsletterView),
                       NewsletterCLSID,
                       12);

   RegisterDocTemplate(m_pBrochureTemplate,
                       IDR_BROCHURETYPE,
                       RUNTIME_CLASS(CBrochureDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CBrochureView),
                       BrochureCLSID,
                       13);

   RegisterDocTemplate(m_pWebPubTemplate,
                       IDR_WEBPUBTYPE,
                       RUNTIME_CLASS(CWebDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CWebView),
                       WebPubCLSID,
                       15);

   RegisterDocTemplate(m_pPostCardTemplate,
                       IDR_POSTCARDTYPE,
                       RUNTIME_CLASS(CPostCardDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CPostCardView),
                       PostCardCLSID,
                       16);

	RegisterDocTemplate(m_pStickerTemplate,
                       IDR_STICKERTYPE,
                       RUNTIME_CLASS(CStickerDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CStickerView),
                       StickerCLSID,
                       17);

   RegisterDocTemplate(m_pTShirtTemplate,
                       IDR_TSHIRTTYPE,
                       RUNTIME_CLASS(CTShirtDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CTShirtView),
                       CertificateCLSID,
                       18);

   RegisterDocTemplate(m_pCraftTemplate,
                       IDR_CRAFTTYPE,
                       RUNTIME_CLASS(CCraftDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CCraftView),
                       CraftCLSID,
                       19);

   RegisterDocTemplate(m_pPhotoProjectsTemplate,
                       IDR_PHOTOPROJECTSTYPE,
                       RUNTIME_CLASS(CEasyPrintsDoc),
                       RUNTIME_CLASS(CPmwMDIChild),        // our MDI child frame
                       RUNTIME_CLASS(CEasyPrintsView),
                       PhotoProjectsCLSID,
                       20);

   SetTemplateProjectType(m_pPosterTemplate, PROJECT_TYPE_Poster);
   SetTemplateProjectType(m_pCardTemplate, PROJECT_TYPE_Card);
   SetTemplateProjectType(m_pBannerTemplate, PROJECT_TYPE_Banner);
   SetTemplateProjectType(m_pCalendarTemplate, PROJECT_TYPE_Calendar);
   SetTemplateProjectType(m_pLabelTemplate, PROJECT_TYPE_Label);
   SetTemplateProjectType(m_pEnvelopeTemplate, PROJECT_TYPE_Envelope);
   SetTemplateProjectType(m_pBizCardTemplate, PROJECT_TYPE_BusinessCard);
   SetTemplateProjectType(m_pCertificateTemplate, PROJECT_TYPE_Certificate);
   SetTemplateProjectType(m_pNoteCardTemplate, PROJECT_TYPE_NoteCard);
   SetTemplateProjectType(m_pFaxCoverTemplate, PROJECT_TYPE_FaxCover);
   SetTemplateProjectType(m_pStationeryTemplate, PROJECT_TYPE_Stationery);
   SetTemplateProjectType(m_pNewsletterTemplate, PROJECT_TYPE_Newsletter);
   SetTemplateProjectType(m_pBrochureTemplate, PROJECT_TYPE_Brochure);
   SetTemplateProjectType(m_pHalfCardTemplate, PROJECT_TYPE_HalfCard);
   SetTemplateProjectType(m_pWebPubTemplate, PROJECT_TYPE_WebPub);
   SetTemplateProjectType(m_pPostCardTemplate, PROJECT_TYPE_PostCard);
	SetTemplateProjectType(m_pStickerTemplate, PROJECT_TYPE_Sticker);
   SetTemplateProjectType(m_pTShirtTemplate, PROJECT_TYPE_TShirt);
   SetTemplateProjectType(m_pCraftTemplate, PROJECT_TYPE_Craft);
   SetTemplateProjectType(m_pPhotoProjectsTemplate, PROJECT_TYPE_PhotoProjects);
	SetTemplateProjectType(m_pBlankTemplate, PROJECT_TYPE_BlankPage);

#if 0
   // Register all OLE server factories as running.  This enables the
   //  OLE 2.0 libraries to create objects from other applications.
   // Note: MDI applications register all server objects without regard
   //  to the /Embedding or /Automation on the command line.

// This is all server stuff.
   m_pPosterTemplate->SetServerInfo(
      IDR_PMWTYPE_SRVR_EMB, IDR_PMWTYPE_SRVR_IP,
      RUNTIME_CLASS(CInPlaceFrame));

   // Connect the COleTemplateServer to the document template.
   //  The COleTemplateServer creates new documents on behalf
   //  of requesting OLE containers by using information
   //  specified in the document template.

   m_server.ConnectTemplate(clsid, m_pPosterTemplate, FALSE);
   COleTemplateServer::RegisterAll();
#endif
}

/*
// Parse the command line.
*/

void CPmwApp::ParseCommandLine(void)
{
   m_first_time_installation = FALSE;

   if (FindParameter("/I"))
   {
      m_first_time_installation = TRUE;
   }
}

/*
// Read the PMINST.DAT file.
*/

BOOL CPmwApp::ReadInstDat(void)
{
   CString csInstdatName;

   TRY
      LoadConfigurationString(IDS_INSTDAT_NAME, csInstdatName);
   END_TRY

   CString csName = GetPathManager()->ExpandPath(csInstdatName);

   if (!m_installation_info.Get(csName, FALSE))
   {
      CString message;

      TRY
      {
         char buffer[_MAX_PATH];
         LoadConfigurationString(IDS_INSTDAT_MISSING, message);
         sprintf(buffer, message, (LPCSTR)csName);
         ::MessageBox(0,
                     buffer,
                     AfxGetAppName(),
                     MB_OK | MB_ICONSTOP | MB_TASKMODAL);
      }
      CATCH_ALL(e)
      {
      }
      END_CATCH_ALL
   /* Regardless. */
      return FALSE;
   }

   return TRUE;
}

/*
// Nid all filenames for this user.
*/

void CPmwApp::NidFilenames(void)
{
   nid_filename(CPmwDoc::tempfile_template);
   nid_filename(CObjectImage::image_template);
   nid_filename(Clipboard::m_cbClipboardTemplate);
   nid_filename(CCommand::m_cbUndoTemplate);
}

/*
// Read all Debug section settings.
*/

void CPmwApp::ReadDebugSection(void)
{
   fWantDebugMessages = OurGetProfileInt(SECTION_Debug, ENTRY_SeeMessages, FALSE);
   want_help_messages = OurGetProfileInt(SECTION_Debug, ENTRY_SeeHelp, FALSE);
   Profiling(OurGetProfileInt(SECTION_Debug, ENTRY_SeeProfile, FALSE));
   CPmwDoc::allow_edit_templates = OurGetProfileInt(SECTION_Debug, ENTRY_EditTemplates, CPmwDoc::allow_edit_templates);
   CPmwDoc::make_templates = OurGetProfileInt(SECTION_Debug, ENTRY_MakeTemplates, CPmwDoc::make_templates);
   CPmwDoc::m_fFixTemplates = OurGetProfileInt(SECTION_Debug, ENTRY_FixTemplates, CPmwDoc::m_fFixTemplates);

   if (OurGetProfileInt(SECTION_Debug, ENTRY_No386, FALSE))
   {
      g_fProcessorIs386Compatible = FALSE;
   }
}

/*
// Read the Configuration section.
*/

void CPmwApp::ReadConfigurationSection(void)
{
/*
// Read any configuration settings.
*/

   RedisplayContext::m_use_ddbs = OurGetProfileInt(SECTION_Configuration,
                                             ENTRY_UseDDBs, TRUE);

/* MarginStyle overrides NewMargins. */

   m_nMarginStyle = OurGetProfileInt(SECTION_Configuration,
                                       ENTRY_MarginStyle, (UINT)-1);

   if (m_nMarginStyle == -1)
   {
      if (OurGetProfileInt(SECTION_Configuration, ENTRY_NewMargins, TRUE))
      {
         m_nMarginStyle = CPaperInfo::MARGINS_Equal;
      }
      else
      {
         m_nMarginStyle = CPaperInfo::MARGINS_Minimal;
      }
   }
   else if (m_nMarginStyle == CPaperInfo::MARGINS_Symmetric2)
   {
   // Two symmetrics map to symmetric.
      m_nMarginStyle = CPaperInfo::MARGINS_Symmetric;
   }

/*
// Read the address book choice.
*/

   m_csAddressBook = OurGetProfileString(SECTION_Configuration, ENTRY_AddressBook, "[[A]]\\ADDRESS.ABK");
   m_csSenderBook = OurGetProfileString(SECTION_Configuration, ENTRY_SenderBook, "[[A]]\\SENDER\\SENDER.ABK");

/*
// Read the txp compatability.
*/

   CCommandList::SetUndoLimit(OurGetProfileInt(SECTION_Configuration, ENTRY_UndoLimit, 25));

   m_fShowRulers = OurGetProfileInt(SECTION_Configuration, ENTRY_ShowRulers, TRUE);
   m_fSnapToRulers = OurGetProfileInt(SECTION_Configuration, ENTRY_SnapToRulers, FALSE);

   m_fShowGuides = OurGetProfileInt(SECTION_Configuration, ENTRY_ShowGuides, TRUE);
   m_fSnapToGuides = OurGetProfileInt(SECTION_Configuration, ENTRY_SnapToGuides, TRUE);
   m_fLockGuides = OurGetProfileInt(SECTION_Configuration, ENTRY_LockGuides, FALSE);

   m_fTextBoxOutlines = OurGetProfileInt(SECTION_Configuration, ENTRY_TextBoxOutlines, TRUE);
   m_fSoundEffectsState = OurGetProfileInt (SECTION_Configuration, "Sound Effects", TRUE);

   m_eDateFormat = (EPDateFormat)OurGetProfileInt(SECTION_Configuration, "Date Format", TRUE);
}

/*
// Read the Image section.
*/

void CPmwApp::ReadImageSection(void)
{
/*
// Read the object image settings.
*/

   CObjectImage::minimum_image_space = (long)OurGetProfileInt(SECTION_Image, ENTRY_MinDiskSpaceK, 300) * 1024L;
   CObjectImage::max_scale_up = OurGetProfileInt(SECTION_Image, ENTRY_MaxScaleUp, 1);
   CObjectImage::max_scale_down = OurGetProfileInt(SECTION_Image, ENTRY_MaxScaleDown, 1);

}

/*
// Read the Bars section.
*/

void CPmwApp::ReadBarsSection(void)
{
/*
// Read the settings for the bars.
*/

//	CMainFrame::m_fStatusBarVisible = OurGetProfileInt(SECTION_Bars, ENTRY_ShowStatus, TRUE);
//	CMainFrame::m_fFileBarVisible = OurGetProfileInt(SECTION_Bars, ENTRY_ShowFile, TRUE);
//	CMainFrame::m_fFormatBarVisible = OurGetProfileInt(SECTION_Bars, ENTRY_ShowFormat, TRUE);
//	CMainFrame::m_fEditBarVisible = OurGetProfileInt(SECTION_Bars, ENTRY_ShowEdit, TRUE);
//	CMainFrame::m_fHelpWindowVisible = OurGetProfileInt(SECTION_Help, ENTRY_ShowHelp, TRUE);
}

/*
// Read the Bars section.
*/

void CPmwApp::ReadPrinterSection(void)
{
/*
// Read the printer configuration settings.
*/
   m_pPortraitPrinterInfo->Defaults();
   m_pLandscapePrinterInfo->Defaults();

   CString csName = GET_PMWAPP()->OurGetProfileString(SECTION_Printer,
                                                      ENTRY_IniFile,
                                                      "PMWPRINT.INI");
   csName = GetPathManager()->ExpandPath(csName);
   m_pPortraitPrinterInfo->SetIniName(csName);
   m_pLandscapePrinterInfo->SetIniName(csName);
}

/*
// Read the Desktop section.
*/

void CPmwApp::ReadDesktopSection(void)
{
	CPmwView::m_clrGuides = OurGetProfileColor(SECTION_Desktop, ENTRY_GuideColor, CPmwView::m_clrGuides);
	CPmwView::m_clrMasterGuides = OurGetProfileColor(SECTION_Desktop, ENTRY_GuideColor, CPmwView::m_clrMasterGuides);
}

/*
// Register any control classes we need.
*/

void CPmwApp::RegisterMyControls(void)
{
/*
// Register the spin button control.
*/

   CSpinButton::RegisterMyClass();

/*
// Register the frame edit control
*/

//   CFrameEdit::RegisterMyClass();

/*
// Register the windows metafile control.
*/

   CWindowsMetaFileControl::RegisterMyClass();

/*
// Register the label preview control.
*/

   CLabelPreview::RegisterMyClass();

/*
// Register the photoproject preview control.
*/

   CPhotoProjectPreview::RegisterMyClass();

/*
// Register the envelope preview control.
*/

   CEnvelopePreview::RegisterMyClass();
   
/*
// Register the controls used by the address book.
*/

   CAddressBook::RegisterAddressBookClasses();

// Register the classes used by the tree combo box.

   CTreeComboBox::RegisterClass();

// Register the class used by the addon install dialog.

   CProgressBar::RegisterMyClass();

	// Register the class used by the cell formatting dialog.
   CCellOutlineControl::RegisterMyClass();

	// Register the class used by the table autoformat dialog.
   CTableFormatControl::RegisterMyClass();
}

/*
// Initialize the workspace.
*/

void CPmwApp::InitializeWorkspace(void)
{
   TRY
   {
      init_workspace_names();

      DWORD cache_size = get_workspace_size();

      cd_cache.open(m_csWorkspaceFile, cache_size, GetPathManager());
   }
   CATCH_ALL(e)
   {
   }
   END_CATCH_ALL
}

/*
// Initialize the font system.
*/

BOOL CPmwApp::InitializeFonts(void)
{
/*
// Read the font cache size.
*/

   int font_cache_k = OurGetProfileInt(SECTION_Fonts, ENTRY_CacheKSize, 512);

   DWORD font_cache_size = 1024L*font_cache_k;

   PMGFontServer::default_face = OurGetProfileString(SECTION_Fonts, ENTRY_DefaultFace, "Imperial");
   PMGFontServer::default_point_size = OurGetProfileInt(SECTION_Fonts, ENTRY_DefaultSize, 36);

/*
// Create the font cache.
*/

   if ((pFontCache = new CFontCache(font_cache_size, 64000L)) == NULL)
   {
      // JRO AfxMessageBox(szInsufficientMemory, MB_OK | MB_ICONEXCLAMATION);
      // JRO Maybe this will free enough for message box
      m_ResourceString.Kill ();

      AfxMessageBox(GetResourceStringPointer (uInsufficientMemory), MB_OK | MB_ICONEXCLAMATION);
      exit_document_engine();
      return FALSE;
   }

/*
// Startup the typeface server.
*/

   if (!typeface_server.startup(PMGFontServer::default_face))
   {
      // JRO AfxMessageBox(szUnableToFindFonts, MB_OK | MB_ICONEXCLAMATION);
      AfxMessageBox(GetResourceStringPointer (uUnableToFindFonts), MB_OK | MB_ICONEXCLAMATION);
      exit_document_engine();
      return FALSE;
   }

   return TRUE;
}

/*
// Create the main window.
*/

BOOL CPmwApp::CreateMainWindow(void)
{
   CMainFrame* pMainFrame = new CMainFrame;
   if (CPrintMasterConfiguration::IsAddOn () == TRUE)
   {
      if (!pMainFrame->LoadFrame(IDR_MAINFRAME_ADDON))
      {
         return FALSE;
      }
   }
   else
   {
      if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
      {
         return FALSE;
      }
   }
   m_pMainWnd = pMainFrame;

   // enable file manager drag/drop
//   m_pMainWnd->DragAcceptFiles();

   // enable DDE Execute open
   EnableShellOpen();

   return TRUE;
}

/*
// Remind the user if we need to.
*/

void CPmwApp::RemindUser(void)
{
   if (TimeForReminder() && !user_is_registered())
   {
		// Remind the user to register.
		RegisterDLL.Register();
		if (RegisterDLL.IsBonusEnabled() == REGRESULT_AlreadyRegistered)
		{
			UserHasRegistered();
		}
   }
}

/*
// Parse a parameter in a command line. A Parameter always begins
// at the start of the command line or after a space. No spaces
// are allowed within parameters, but quoted strings are handled
// properly and may contain spaces. All comparisons are case-insensitive.
*/

LPSTR CPmwApp::LocateParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
   LPSTR lpszResult = NULL;
   
   LPSTR lpszCommandLine = m_lpCmdLine;
   int nParamNameLength = lstrlen(lpszParamName);
   int nCommandLineLength = lstrlen(lpszCommandLine);
   BOOL fInQuote = FALSE;
   BOOL fStartOfParameter = TRUE;
   BOOL fFoundParameter = FALSE;
   
   while ((!fFoundParameter) && (nCommandLineLength >= nParamNameLength))
   {
      if (fStartOfParameter)
      {
         // A parameter can start here, check for a match.
         
         int i = 0;
         
         for (;;)
         {
            char ch1 = lpszCommandLine[i];
            if ((ch1 > 'a') && (ch1 < 'z')) ch1 = (char)(ch1+'A'-'a');
            
            char ch2 = lpszParamName[i];
            if ((ch2 > 'a') && (ch2 < 'z')) ch2 = (char)(ch2+'A'-'a');
            
            if (ch2 == 0)
            {
               fFoundParameter = TRUE;
               break;
            }
            
            if (ch1 != ch2)
            {
               fStartOfParameter = FALSE;
               break;
            }
            
            i++;
         }
      }
      
      else
      {
         if (*lpszCommandLine == ' ')
         {
            fStartOfParameter = !fInQuote;
         }
         
         else if (*lpszCommandLine == '"')
         {
            fInQuote = !fInQuote;
         }
         
         lpszCommandLine++;
         nCommandLineLength--;
      }
   }
   
   if (fFoundParameter)
   {
      lpszResult = lpszCommandLine;
      
      if (lpszValue != NULL)
      {
         *lpszValue = 0;
         
         lpszCommandLine += nParamNameLength;

         if (*lpszCommandLine == '=')
         {
            lpszCommandLine++;
         }
         
         fInQuote = FALSE;
         
         for (;;)
         {
            if (*lpszCommandLine == 0)
            {
               break;
            }
            
            if ((!fInQuote) && (*lpszCommandLine == ' '))
            {
               break;
            }
            
            if (*lpszCommandLine == '"')
            {
               fInQuote = !fInQuote;
            }
            
            else
            {
               *(lpszValue++) = *lpszCommandLine;
            }
            
            lpszCommandLine++;
         }
         
         *lpszValue = 0;
      }
   }
   
   return lpszResult;
}

BOOL CPmwApp::FindParameter(LPSTR lpszParamName)
{
   return LocateParameter(lpszParamName, NULL) != NULL;
}

LPSTR CPmwApp::GetParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
   if (LocateParameter(lpszParamName, lpszValue) == NULL)
   {
      return NULL;
   }
   
   else
   {
      return lpszValue;
   }
}

/*
// Initialize the user directory.
*/

BOOL CPmwApp::InitUserDirectory(CString& csUserDirectory)
{
   CIniFile IniFile(m_csMainIni);
   CString csEntry;
   CString csPath;

   if (GetConfiguration()->IsAddOn () == FALSE)
   {
      TRY
      {
         // The default user directory is "USERxxx" (xxx is user ID).
         // However, we allow this to be overidden in the [Users] section of the
         // PMW.INI either specifically ("xxxDIR=") or generally ("DefaultDir=").

         char cbSection[16];
         sprintf(cbSection, "%03dDIR", m_nUserID);

         // Get the default value.
         csEntry = IniFile.GetString(SECTION_Users, ENTRY_DefaultDir, "USER%03d");

         // Get any user override.
         csEntry = IniFile.GetString(SECTION_Users, cbSection, csEntry);

         // Format the path using the user ID.
         csPath.Format(csEntry, m_nUserID);
      }
      END_TRY
   }
   else
   {
      csPath = GetPathManager()->GetWorkingDirectory();
   }

	csUserDirectory = GetPathManager()->ExpandPath(csPath);

   // Make sure the directory exists.
   if (!Util::MakeDirectory(csUserDirectory))
   {
      // LPCSTR pFormat = "Can't create the directory\n%s";
      LPCSTR pFormat = GET_PMWAPP()->GetResourceStringPointer(IDS_ErrCreateDirectory);
      CString csMessage;
      csMessage.Format(pFormat, (LPCSTR)csUserDirectory);
      AfxMessageBox(csMessage);
      return FALSE;
   }

   return TRUE;
}

BOOL CPmwApp::setup_ini_file(LPCSTR pszUserDirectory)
{
   CString csName;
   
   if (GetConfiguration()->IsAddOn () == TRUE)
		Util::ConstructPath(csName, pszUserDirectory, m_pszProfileName);
   else
		Util::ConstructPath(csName, pszUserDirectory, "USER.INI");

   // The m_pszProfileName string is never freed.
   // We need to disable memory tracking for this, just like MFC does.
#ifdef _DEBUG
   BOOL fEnabled = AfxEnableMemoryTracking(FALSE);
#endif
   free((void *)m_pszProfileName);
   m_pszProfileName = _strdup(csName);
#ifdef _DEBUG
   AfxEnableMemoryTracking(fEnabled);
#endif

   if (GetConfiguration()->IsAddOn () == TRUE)
      return TRUE;

/*
// Make sure the INI file exists.
// If not, create a blank one.
*/

   if (access_file(m_pszProfileName) != 0)
   {
   /* We need to create one. */

      StorageFile new_ini_file(m_pszProfileName);
      ReadOnlyFile default_ini_file("PMWINI.DEF");

      ERRORCODE error;

      if ((error = copy_file(&default_ini_file, &new_ini_file)) != ERRORCODE_None)
      {
         char buffer[256];

         // JRO sprintf(buffer, "Got error %d creating file %s\r\n", error, m_pszProfileName);
         sprintf(buffer, GetResourceStringPointer (IDS_FILE_CREATE_ERROR), error, m_pszProfileName);

         ::MessageBox(0,
                  buffer,
                  AfxGetAppName(),
                  MB_OK | MB_TASKMODAL | MB_ICONEXCLAMATION);
         return FALSE;
      }
   }
   return TRUE;
}

// App command to run the dialog
void CPmwApp::OnAppAbout()
{
   if (CPrintMasterConfiguration::IsAddOn () == TRUE)
   {
      RunAboutAddOn ();  
   }
   else
   {
      CAboutDlg aboutDlg;
      aboutDlg.DoModal();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CPmwApp commands

void CPmwApp::OnFileNew()
{
   if (CanOpenAnotherDocument())
   {
		new_document();
	}
}

void CPmwApp::RevertToSavedFile(LPCSTR strFileName)
{

	OpenDocumentFile(strFileName);

	// Open this document.
//		int nIndex = 0;
//		if (OpenDocumentFile((*m_pRecentFileList)[nIndex]) == NULL)
//		{
//			m_pRecentFileList->Remove(nIndex);
//		}

	//}
}


void SetNewBusinessCardSize(void)
{
   // Paper info is business card info.

   CPaperInfo* pPaperInfo = GET_PMWAPP()->last_new_info.m_pPaperInfo;

#ifdef LOCALIZE
   double PaperWidth, PaperHeight,
		  CardWidth, CardHeight, StepWidth, StepHeight,
	      VerticalMargin, HorizontalMargin;
   int nCardsAcross, nCardsDown;

   CurrentLocale.GetBusinessCardDimsInInches(&PaperWidth, &PaperHeight,
							   &CardWidth, &CardHeight, 
                               &StepWidth, &StepHeight,
	      				       &VerticalMargin, &HorizontalMargin,
						       &nCardsAcross, &nCardsDown);
   pPaperInfo->InitAsLabels(
                            (CFixed) (PaperWidth*0x10000), (CFixed) (PaperHeight*0x10000),  
                            (CFixed) (CardWidth*0x10000), (CFixed) (CardHeight*0x10000),  
                            nCardsAcross, nCardsDown,                     
                            (CFixed) (HorizontalMargin*0x10000), (CFixed) (VerticalMargin*0x10000),  
                            (CFixed) (StepWidth*0x10000), (CFixed) (StepHeight*0x10000)); 


#else

   pPaperInfo->InitAsLabels(
                            0x00088000L, 0x000b0000,        /* 8.5", 11.0" */
                            0x00038000L, 0x00020000,        /* 3.5", 2.0" */
                            2, 5,                           /* 2 x 5 */
                            0x0000c000L, 0x00008000,        /* 0.75", 0.5" */
                            0x00038000L, 0x00020000);    /* 3.5", 2.0" */
#endif
      
   CFixedRect MinimumMargins;
   MinimumMargins.Left = 
      MinimumMargins.Right =
      MinimumMargins.Top =
      MinimumMargins.Bottom = MakeFixed(1)/8;

   pPaperInfo->MinimumMargins(MinimumMargins);
   pPaperInfo->ComputeMargins();
   // JRO pPaperInfo->SetName("Business Cards");
   pPaperInfo->SetName(GET_PMWAPP()->GetResourceStringPointer (IDS_BUSINESS_CARDS));
}

BOOL CPmwApp::new_document(int nProjectType)
{
   int      retval;
   BOOL     fResult = FALSE;

   ASSERT(nProjectType == 0);       // Since we now ignore this.

#ifdef WIN32
   CNewDialog dlg(NULL, last_new_info);
#else
   CNewDialog dlg(&m_templateList, last_new_info);
#endif

   do
   {
      if ((retval = dlg.DoModal()) != IDOK)
      {
         CPmwDoc::KillNewStuff ();
         return FALSE;     // none - cancel operation
      }

   /* Get the new info. */

      dlg.get_exit_info(&last_new_info);

   // Now set any paper info we need set.
      switch (last_new_info.type)
      {
			case PROJECT_TYPE_Label:
			case PROJECT_TYPE_Sticker:
         {
         // Choose the type of label.
            CLabelTypeDialog Dialog(last_new_info.m_pPaperInfo, m_pMainWnd, WIZBUT_BACK | WIZBUT_CANCEL | WIZBUT_FINISH, BRAND_NEW_COLOR);

            if ((retval = Dialog.DoModal()) == IDCANCEL)
            {
               // I guess they changed their mind.
               CPmwDoc::KillNewStuff ();
               return FALSE;
            }
            
            if (retval == IDOK)
               *(last_new_info.m_pPaperInfo) = *(CPaperInfo*)Dialog.GetChosenLabel();
            break;
         }
         case PROJECT_TYPE_Envelope:
         {
         // Choose the type of envelope.
            CEnvelopeTypeDialog Dialog(last_new_info.m_pPaperInfo, m_pMainWnd, WIZBUT_BACK | WIZBUT_CANCEL | WIZBUT_FINISH, BRAND_NEW_COLOR);

            if ((retval = Dialog.DoModal()) == IDCANCEL)
            {
               // I guess they changed their mind.
               CPmwDoc::KillNewStuff ();
               return FALSE;
            }
         
            if (retval == IDOK)
               *(last_new_info.m_pPaperInfo) = *(Dialog.GetChosenEnvelope());
            break;
         }
         case PROJECT_TYPE_Craft:
         {
         // Choose the type of Craft.
            CCraftTypeDlg Dialog(m_pMainWnd, WIZBUT_BACK | WIZBUT_CANCEL | WIZBUT_FINISH, BRAND_NEW_COLOR);

            if ((retval = Dialog.DoModal()) == IDCANCEL)
            {
               // I guess they changed their mind.
               CPmwDoc::KillNewStuff ();
               return FALSE;
            }
         
            if (retval == IDOK)
				{
					CString csProjectPath = Dialog.GetChosenCraft();
					last_new_info.subtype = Dialog.GetChosenType();

					if(!csProjectPath.IsEmpty())
					{
						CPmwDoc* pDoc = (CPmwDoc*)OpenDocumentFile(csProjectPath, FALSE);
						if (pDoc != NULL)
						{
							pDoc->DocumentRecord()->set_project_type(PROJECT_TYPE_Craft);
							pDoc->DocumentRecord()->SetProjectSubtype(last_new_info.subtype);
							return TRUE;
						}
					}
					return FALSE;
				
				}
				break;
         }
         case PROJECT_TYPE_PhotoProjects:
         {
				// Choose the type of PhotoProject.
				CPhotoProjectsTypeDlg Dialog(last_new_info.m_pPaperInfo, m_pMainWnd, WIZBUT_BACK | WIZBUT_CANCEL | WIZBUT_FINISH, BRAND_NEW_COLOR);

            if ((retval = Dialog.DoModal()) == IDCANCEL)
            {
               // I guess they changed their mind.
               CPmwDoc::KillNewStuff ();
               return FALSE;
            }
         
            if (retval == IDOK)
				{
					*(last_new_info.m_pPaperInfo) = *(CPaperInfo*)Dialog.GetChosenPhotoProject(); 
					last_new_info.subtype = Dialog.GetChosenType();
				
				}
				break;
         }
         case PROJECT_TYPE_PostCard:
         {
         // Choose the type of Post Card.
            CPostCardTypeDialog Dialog(last_new_info.m_pPaperInfo, m_pMainWnd, WIZBUT_BACK | WIZBUT_CANCEL | WIZBUT_FINISH, BRAND_NEW_COLOR);

            if ((retval = Dialog.DoModal()) == IDCANCEL)
            {
               // I guess they changed their mind.
               CPmwDoc::KillNewStuff ();
               return FALSE;
            }
         
            if (retval == IDOK)
            {
               *(last_new_info.m_pPaperInfo) = *(Dialog.GetChosenPostCard());
               last_new_info.orientation = last_new_info.m_pPaperInfo->Orientation ();
            }
            break;
         }
         case PROJECT_TYPE_BusinessCard:
         {
            COrientationDialog dlgo(last_new_info);
            
            if ((retval = dlgo.DoModal()) == IDCANCEL)
            {
               CPmwDoc::KillNewStuff ();
               return FALSE;     // none - cancel operation
            }
            
            if (retval == IDOK)
            {
               dlgo.get_exit_info(&last_new_info);
               SetNewBusinessCardSize();
            }
            break;
         }
         case PROJECT_TYPE_WebPub:
         {
            CWebDialog  dlgWebSetup(last_new_info.m_pPaperInfo);

            if ((retval = dlgWebSetup.DoModal()) == IDCANCEL)
            {
               CPmwDoc::KillNewStuff ();
               return FALSE;     // none - cancel operation
            }
            break;
         }
         case PROJECT_TYPE_Calendar:
         {
				short backto = 0;

				static CALENDAR_INFO StaticInfo;
				// set defaults
				StaticInfo.style.type = MONTHLY;
						
            while (TRUE)
            {
					if (last_new_info.m_pCalendarInfo == NULL)
					{
						last_new_info.m_pCalendarInfo = &StaticInfo;
					}

					switch (backto)
					{
						case -1:
							break;
						case 0:
						{
							CCalendarTypeDlg typedlg (BRAND_NEW_COLOR, &StaticInfo);
							retval = typedlg.DoModal();

							if (retval == IDOK)
							{
								typedlg.get_exit_info (&StaticInfo);
								backto++;
							}
							break;
						}			

						case 1:
						{
							COrientationDialog dlgo(last_new_info);
							retval = dlgo.DoModal();
							if (retval == IDOK)
							{
								backto++;
								dlgo.get_exit_info(&last_new_info);
							}
							break;
						}
						case 2:
						{
							CCalendarPictureDlg picdlg (&last_new_info, BRAND_NEW_COLOR);
									//StaticInfo.style.type, 
									//last_new_info.orientation);

							retval = picdlg.DoModal();
							if (retval == IDOK)
							{
								backto++;
								last_new_info.m_PictureStyle = picdlg.m_picture_style;
							}
							break;
						}

						case 3:
						{
							SYSTEMTIME Now;
							GetLocalTime(&Now);
			
							StaticInfo.month = (MONTH)Now.wMonth-1;
							StaticInfo.year = Now.wYear;

							// Compute the week. 
							StaticInfo.week = 0;

							int weekday = Now.wDayOfWeek;
							int day = Now.wDay-1;         // 0 based

							while (weekday < day)
							{
								StaticInfo.week++;
								day -= 7;
							}

							StaticInfo.first_day_of_week = SUNDAY;
							StaticInfo.last_day_of_week = SATURDAY;
							last_new_info.m_NumberOfCalendars = 1;

							CCalendarDateDlg datedlg (StaticInfo, TRUE, WIZBUT_BACK | WIZBUT_CANCEL | WIZBUT_FINISH, BRAND_NEW_COLOR);
							retval = datedlg.DoModal();
							
							if (retval == IDOK)
							{
								backto++;
								datedlg.get_exit_info(StaticInfo);
								last_new_info.m_pCalendarInfo = &StaticInfo;
								last_new_info.m_NumberOfCalendars = datedlg.m_iNumObjects;
								break;
							}
							break;
						}
					}

					if (retval == IDCANCEL)
					{
						CPmwDoc::KillNewStuff ();
						return FALSE;     // none - cancel operation
					}
			
					if (retval == IDC_BACK)
					{
						backto--;
						if (backto < 0)
							break; // exit do loop and go back to project type dialog
					}

					if (backto > 3)
						break;
				};
         }
         default:
         {
				if (last_new_info.type == PROJECT_TYPE_Calendar)
				{
					break;
				}

				if (last_new_info.type == PROJECT_TYPE_Banner && ((GetConfiguration()->SupportsVerticalBanners() == FALSE)))
				{
					if (retval == IDOK)
						*(last_new_info.m_pPaperInfo) = *GetPaperInfo(last_new_info.orientation);
					
					break;
				}

				// Get Orientation information if necessary
            if (last_new_info.type != PROJECT_TYPE_TShirt)
            {

					COrientationDialog dlgo(last_new_info);
            
					if ((retval = dlgo.DoModal()) == IDCANCEL)
					{
						CPmwDoc::KillNewStuff ();
						return FALSE;     // none - cancel operation
					}
            
					dlgo.get_exit_info(&last_new_info);

            }

            if (retval == IDOK)
               *(last_new_info.m_pPaperInfo) = *GetPaperInfo(last_new_info.orientation);
            break;
         }
      }
   } while (retval == IDC_BACK);

/* Open according to the template. */
	// Remove the initial poster
	KillFirstPoster();

   // These projects have their actual paper orientation flipped from what
   // is selected in the orientation dialog.
   if ((last_new_info.type == PROJECT_TYPE_Brochure)
    || (last_new_info.type == PROJECT_TYPE_BusinessCard)
    || (last_new_info.type == PROJECT_TYPE_HalfCard))
   {
      last_new_info.orientation = (last_new_info.orientation == PORTRAIT) ? LANDSCAPE : PORTRAIT;
   }

   CPmwDoc* pDoc;
   pDoc = (CPmwDoc*)last_new_info.pTemplate->OpenDocumentFile(NULL);

   if (pDoc != NULL)
      pDoc->AddNewStuff ();
   else
      CPmwDoc::KillNewStuff ();   

   if ((last_new_info.type == PROJECT_TYPE_Brochure) 
      || (last_new_info.type == PROJECT_TYPE_BusinessCard))
      last_new_info.orientation = (last_new_info.orientation == PORTRAIT) ? LANDSCAPE : PORTRAIT;

   fResult = (pDoc != NULL);

   return fResult;
}

BOOL CPmwApp::KillFirstPoster()
{
	// (Have to search for it)
	BOOL fFoundFirstPoster = FALSE;
	POSITION pos = m_pPosterTemplate->GetFirstDocPosition();
	while (pos != NULL && !fFoundFirstPoster)
	{
		fFoundFirstPoster = (m_pFirstPosterDoc == m_pPosterTemplate->GetNextDoc(pos));
	}
	if (fFoundFirstPoster &&
		 m_pFirstPosterDoc != NULL && 
		 !m_pFirstPosterDoc->IsModified())
	{
		CPmwView* pView = m_pFirstPosterDoc->GetCurrentView();
		CMDIChildWnd* pChild = (CMDIChildWnd*)pView->GetParent();
		ASSERT(pChild->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
		pChild->MDIDestroy();
		m_pFirstPosterDoc = NULL;
	}

	if (fFoundFirstPoster)
		SetPreventHub();
	
	return fFoundFirstPoster;
}

BOOL CPmwApp::ChooseADocument(CString& csFileName, int nProjectType /*=0*/)
{
   CHAR drive[_MAX_DRIVE];
   CHAR dir[_MAX_DIR];
   CHAR fname[_MAX_FNAME];
   CHAR ext[_MAX_EXT];
   CHAR full_dir[_MAX_PATH], full_name[_MAX_PATH];
   BOOL got_name = FALSE;

   if (++nProjectType != 1)         // List is one-based.
   {
   /* Skip "*.*" entry in filter list. */
      nProjectType++;
   }

/* Handle the global last name. */

   if (m_csLastDocument.IsEmpty())
   {
		strcpy(full_dir, GetPathManager()->ExpandPath("[[D]]"));
      *full_name = '\0';
   }
   else
   {
		strcpy(full_name, GetPathManager()->ExpandPath(m_csLastDocument, TRUE));
      got_name = TRUE;
   }

   if (got_name)
   {
      _splitpath(full_name, drive, dir, fname, ext);
      sprintf(full_dir, "%s%s", drive, dir);
      sprintf(full_name, "%s%s", fname, ext);
   }

   /*
   // Get the filter string from the configuration.
   */

   CString filters;
   GetConfiguration()->DocumentFilter(filters);

   /*
   // Construct the dialog object.
   */

   COpenProjectDlg opendlg(full_name, full_dir, filters);
   opendlg.m_ofn.nFilterIndex = nProjectType;

   // Do the dialog.
   if (opendlg.DoModal() != IDOK)
   {
      return FALSE;
   }

	// Get the chosen name.
   CString filename = opendlg.GetPathName();

	// Save a copy of the name for next time.

   m_csLastDocument = GetPathManager()->MakeCompositePath(filename);

   csFileName = filename;
   return TRUE;
}

BOOL CPmwApp::open_a_document(int nProjectType)
{
   BOOL fResult = FALSE;
   
   CString csName;

   if (ChooseADocument(csName, nProjectType))
   {
   /* Do the open. */
      fResult = OpenDocumentFile(csName) != NULL;
   }
   
   return fResult;
}

BOOL CPmwApp::OnIdle(LONG lCount)
{                   
   static BOOL fSemaphore = FALSE;

   CBoolSemaphore Semaphore(fSemaphore);

   if (!Semaphore.Lock())
   {
      TRACE0("App idle semaphore is locked...\n");
      return FALSE;
   }

   BOOL more = FALSE;

   static DWORD idle_start;
   DWORD now = GetTickCount();

   if (lCount == 0)
   {
      idle_start = now;
		_heapmin();
      // Update the license data from the catalog's LICENSE.DAT.
      LicenseData.Update();
#ifndef _WIN32
      switch (_fheapchk())
      {
         case _HEAPBADBEGIN:
         {
            od("Heap has bad beginning.\r\n");
            break;
         }
         case _HEAPBADNODE:
         {
            od("Heap has bad node.\r\n");
            break;
         }
         case _HEAPEMPTY:
         {
            od("Heap is empty!\r\n");
            break;
         }
         case _HEAPOK:
         {
         /* Do nothing. */
            break;
         }
      }
#endif
   }

// od("idle %ld -> ", lCount);

/* See if the app needs more time. */

   if (CWinApp::OnIdle(lCount))
   {                          
//    od("app handled it\r\n");
      more = TRUE;
   }                           

// od("update now!\r\n");

/*
// Now do our stuff.
// Run through all document templates and see if any documents need refreshing.
*/

#ifdef WIN32
   POSITION tpos = GetFirstDocTemplatePosition();
#else
   POSITION tpos = m_templateList.GetHeadPosition();
#endif
   while (tpos)
   {
#ifdef WIN32
      CDocTemplate* pTemplate = GetNextDocTemplate(tpos);
#else
      CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(tpos);
#endif

      POSITION dpos = pTemplate->GetFirstDocPosition();

      while (dpos)
      {
         CPmwDoc* pDoc = (CPmwDoc *)pTemplate->GetNextDoc(dpos);

         if (pDoc->idle_refresh())
         {
//          od("idle: %ld (%d)\r\n", lCount, TRUE);
            more = TRUE;
         }
      }
   }

// od("idle: %ld (%d)\r\n", lCount, more);

	// Enable/disable font combo boxes, depending on existence
	// of a document.
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)m_pMainWnd;
	if (pFrame != NULL)
	{
//		((CMainFrame*)pFrame)->FormatBar().EnableFontBoxes(pFrame->GetActiveFrame() != pFrame);
	}

   return more;
}

BOOL CPmwApp::CanOpenAnotherDocument(void)
{
#ifndef WIN32
   if (GetOpenDocumentCount() >= MaxDocuments)
   {
      AfxMessageBox(IDS_MAX_DOCUMENTS_OPEN);
      return FALSE;
   }
#endif
   return TRUE;
}

int CPmwApp::GetOpenDocumentCount(void)
{
   int nCount = 0;
#ifdef WIN32
   POSITION tpos = GetFirstDocTemplatePosition();
#else
   POSITION tpos = m_templateList.GetHeadPosition();
#endif
   while (tpos)
   {
#ifdef WIN32
      CDocTemplate* pTemplate = GetNextDocTemplate(tpos);
#else
      CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(tpos);
#endif

      POSITION dpos = pTemplate->GetFirstDocPosition();

      while (dpos)
      {
         pTemplate->GetNextDoc(dpos);
         nCount++;
      }
   }
   return nCount;
}

BOOL CPmwApp::PreTranslateMessage(MSG* pMsg)
{
	int retval;

	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
      // Make sure the window exists before sending message
      if(m_pHelpfulHint != NULL && m_pHelpfulHint->GetHelpfulHintWnd() != NULL)
      {
		   m_pHelpfulHint->GetHelpfulHintWnd()->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		   retval = TRUE;
      }
      else
   		retval = CWinApp::PreTranslateMessage(pMsg);
	}
	else
	{
		retval = CWinApp::PreTranslateMessage(pMsg);
	}

	return retval;
}

void CPmwApp::WinHelp(DWORD dwData, UINT nCmd)
{
   // No help files for addons
   //if (CPrintMasterConfiguration::IsAddOn () == TRUE)
   //   return;
   
   // Add ons can have help, determined by CPrintMasterConfiguration 
   // from ADDON.INI.
   
   if (GetConfiguration()->IsHelpFile () == FALSE)
      return;

   if (want_help_messages)
   {
      char buffer[50];
      // JRO wsprintf(buffer, "Command: %x, Help id: %lx", nCmd, dwData);
      wsprintf(buffer, GetResourceStringPointer (IDS_COMMAND_HELP), nCmd, dwData);

      if (::MessageBox(::GetLastActivePopup(m_pMainWnd->GetSafeHwnd()),
                     buffer,
                     // JRO "You Asked for Help?",
                     GetResourceStringPointer (IDS_Q_WANT_HELP),
                     MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
      {
         return;
      }
   }
   CWinApp::WinHelp(dwData, nCmd);
}

#ifdef WIN32
BOOL CPmwApp::IsIdleMessage(MSG* pMsg)
{
// Default filters out WM_PAINT. We put it back in.
   return (pMsg->message == WM_PAINT) || CWinApp::IsIdleMessage(pMsg);
}
#endif

CDocument* CPmwApp::OpenDocumentFile(LPCSTR lpszFileName)
{
	return OpenDocumentFile(lpszFileName, TRUE);
}

CDocument* CPmwApp::OpenDocumentFile(LPCSTR lpszFileName, BOOL fAddToMRU)
{
   if (!CanOpenAnotherDocument())
   {
      return FALSE;
   }

	// Kill the initial poster
	KillFirstPoster();

   CDocument* pDoc = NULL;
   CString csFileName = lpszFileName;

   if (GetPathManager()->GetPathBindingType(lpszFileName) == PBT_CollectionItem)
   {
      CString csDestPath;
      CString csName;
      CString csExtension;
      CString csFinalName;
      
		BeginWaitCursor();

      ReadOnlyFile Source(lpszFileName);

      Util::SplitPath(lpszFileName, NULL, NULL, &csName, &csExtension);
      csDestPath = GetTransferDirectory();
      Util::AppendBackslashToPath(csDestPath);
      csDestPath += csName;
      csDestPath += csExtension;

      // We have to copy the data out of the content database to its
      // own file in order to open it.
      StorageFile Dest(csDestPath);

      if (copy_file(&Source, &Dest) == ERRORCODE_None)
      {
         Dest.flush();
         csFinalName = GetPathManager()->ExpandPath(Dest.get_name());

         // Open the document, but do not add the path to the MRU.
         WORD wGDFSave = CPmwDoc::m_wGlobalDocumentFlags;
         CPmwDoc::m_wGlobalDocumentFlags = CPmwDoc::FLAG_NoMRU | CPmwDoc::FLAG_LoadedFromCollection;
         pDoc = CWinApp::OpenDocumentFile(csFinalName);
         CPmwDoc::m_wGlobalDocumentFlags = wGDFSave;

         if (pDoc != NULL)
         {
            ((CPmwDoc*)pDoc)->SetBrowserPath(lpszFileName);
            ((CPmwDoc*)pDoc)->DoPostOpen();
            ((CPmwDoc*)pDoc)->set_file_type(TEMPLATE_TYPE);
				if (fAddToMRU)
            AddToRecentFileList(csFileName);    // Do not use lpszFileName. It may be an MRU string!
         }
      }

      Dest.flush();
      Dest.zap();

      EndWaitCursor();
   }
   else
   {
      // Open the document with the original name.
      pDoc = CWinApp::OpenDocumentFile(GetPathManager()->LocatePath(lpszFileName));
   }

   // See if we need to complain about this type of project.
   if (pDoc == NULL && GetPathManager()->FileExists(lpszFileName))
   {
      GetConfiguration()->SupportsProjectType(-1);
   }
   return pDoc;
}

/*
// Get the user transfer directory.
*/

CString CPmwApp::GetTransferDirectory(void)
{
   CString csDir = GetPathManager()->ExpandPath("[[U]]\\TMP");
   if (!Util::MakeDirectory(csDir))
   {
      // We assume the user directory exists.
      csDir = GetPathManager()->ExpandPath("[[U]]");
   }
   return csDir;
}

CDocTemplate* CPmwApp::GetMatchingTemplate(LPCSTR pszFile, BOOL fExactOnly /*=FALSE*/)
{
   // find the highest confidence
#ifdef WIN32
   POSITION pos = GetFirstDocTemplatePosition();
#else
   POSITION pos = m_templateList.GetHeadPosition();
#endif
   CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
   CDocTemplate* pBestTemplate = NULL;
   CDocument* pOpenDocument = NULL;

   while (pos != NULL)
   {
#ifdef WIN32
      CDocTemplate* pTemplate = GetNextDocTemplate(pos);
#else
      CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
#endif
      ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

      CDocTemplate::Confidence match;
      ASSERT(pOpenDocument == NULL);
#ifdef _MAC
      //TODO:MAC: what is our document type?
      match = pTemplate->MatchDocType(pszFile, 'PMST', pOpenDocument);
#else
      match = pTemplate->MatchDocType(pszFile, pOpenDocument);
#endif
      if (match > bestMatch)
      {
         bestMatch = match;
         pBestTemplate = pTemplate;
      }
      if (match == CDocTemplate::yesAlreadyOpen)
         break;      // stop here
   }

   // If we only want an exact match, only accept an exact match.
   if (fExactOnly && bestMatch < CDocTemplate::yesAttemptNative)
   {
      // Not an exact match.
      pBestTemplate = NULL;
   }

   return pBestTemplate;
}

CPmwDoc* CPmwApp::OpenHiddenDocument(LPCSTR pszFileName, LPCSTR pszOriginalFileName /*=NULL*/)
{
   CString csDocTypeName;

   if(pszOriginalFileName)
      csDocTypeName = pszOriginalFileName;
   else
      csDocTypeName = pszFileName;

   CDocTemplate* pBestTemplate = GetMatchingTemplate(csDocTypeName);
   if (pBestTemplate == NULL)
   {
      TRACE0("Error: can't open document\n");
      return NULL;
   }

   // Create an "album" document.
   WORD wGDFSave = CPmwDoc::m_wGlobalDocumentFlags;
   CPmwDoc::m_wGlobalDocumentFlags = CPmwDoc::FLAG_AlbumDocument;
   CPmwDoc* pDoc = (CPmwDoc*)pBestTemplate->OpenDocumentFile(pszFileName, FALSE);
   CPmwDoc::m_wGlobalDocumentFlags = wGDFSave;

   return pDoc;
}

CPmwDoc* CPmwApp::NewHiddenDocument(void)
{
   // Create an "album" document using the scratch template.
   CDocTemplate* pTemplate = m_pScratchTemplate;

   WORD fGDFSave = CPmwDoc::m_wGlobalDocumentFlags;
   CPmwDoc::m_wGlobalDocumentFlags = CPmwDoc::FLAG_AlbumDocument;
   CPmwDoc* pDoc = (CPmwDoc*)pTemplate->OpenDocumentFile(NULL, FALSE);
   CPmwDoc::m_wGlobalDocumentFlags = fGDFSave;

#ifndef WIN32
   if (pDoc != NULL)
   {
   /* OpenDocumentFile doesn't do this if not bMakeVisible. */
      if (!pDoc->DoNewDocument())
      {
         pDoc->OnCloseDocument();
         pDoc = NULL;
      }
   }
#endif
   return pDoc;
}

/*
// Init the workspace INI file names.
*/

void CPmwApp::init_workspace_names(void)
{
   CString csFullWorkspacePath;

   TRY
   {
   /* Get the workspace (cache) file name. */

      // Use unique workspace filename for each user - DGP
      char name[] = "[[U]]\\PMW.WRK";
      m_csWorkspaceFile = OurGetProfileString(SECTION_Workspace, ENTRY_WorkspaceFileName, name);

   /* Fully expand the path name. */

      csFullWorkspacePath = GetPathManager()->ExpandPath(m_csWorkspaceFile);

      // Use the main user INI file for the workspace INI file.
      m_csWorkspaceINI = m_pszProfileName;
   }
   CATCH_ALL(e)
   {
      csFullWorkspacePath.Empty();

      m_csWorkspaceFile.Empty();
      m_csWorkspaceINI.Empty();
      THROW_LAST();
   }
   END_CATCH_ALL
}

void CPmwApp::WakeWindow(CWnd* pWnd /*=NULL*/)
{
	if (pWnd == NULL)
	{
		pWnd = AfxGetMainWnd();
		ASSERT(pWnd != NULL);
	}

	if (pWnd == AfxGetMainWnd() && !pWnd->IsWindowVisible())
	{
		// We need to wake the main window.
		LoadWindowPosition(pWnd, "Frame", SW_SHOWNORMAL, FALSE, FALSE);
	}

	if (pWnd->IsWindowVisible())
	{
		CWnd* pWndLastActive = pWnd->GetLastActivePopup();

		if (pWndLastActive != NULL)
		{
			if (pWnd->IsIconic())
			{
				pWnd->ShowWindow(SW_RESTORE);
			}
			else
			{
#ifdef WIN32
				pWnd->SetForegroundWindow();
#endif
				pWnd->BringWindowToTop();

				if (pWndLastActive != pWnd)
				{
#ifdef WIN32
					pWndLastActive->SetForegroundWindow();
#endif
					pWndLastActive->BringWindowToTop();
            }
         }
      }
   }
}

/*
// Get the workspace INI file names.
*/

void CPmwApp::get_workspace_names(CString* pcsWorkspaceFile, CString* pcsWorkspaceINI)
{
   if (pcsWorkspaceFile != NULL)
   {
      *pcsWorkspaceFile = m_csWorkspaceFile;
   }
   if (pcsWorkspaceINI != NULL)
   {
      *pcsWorkspaceINI = m_csWorkspaceINI;
   }
}

DWORD CPmwApp::get_workspace_size(void)
{
   CString csWorkspaceSize;
   DWORD dwCacheSize = 0;

   TRY
   {
      // Get workspace size from users NID .INI file - DGP
      csWorkspaceSize = OurGetProfileString(SECTION_Workspace, ENTRY_WorkspaceSize, "5000000");

      dwCacheSize = atol(csWorkspaceSize);
   }
   CATCH_ALL(e)
   {
      csWorkspaceSize.Empty();
      THROW_LAST();                               
   }
   END_CATCH_ALL

   return dwCacheSize;
}

/*
// Get the global dev mode memory.
*/

HGLOBAL CPmwApp::get_devmode(BOOL fUpdate /*=TRUE*/)
{
   if (fUpdate)
   {
      UpdatePrinterSelection(FALSE);
   }
   return m_hDevMode;
}

/*
// Get the global dev mode memory.
*/

HGLOBAL CPmwApp::get_devnames(BOOL fUpdate /*=TRUE*/)
{
   if (fUpdate)
   {
      UpdatePrinterSelection(FALSE);
   }
   return m_hDevNames;
}

VOID CPmwApp::set_dev_handles(HGLOBAL hDevMode, HGLOBAL hDevNames)
{
   m_hDevMode = hDevMode;
   m_hDevNames = hDevNames;
}

/*
// Update the paper info.
// It is assumed that hDev* variables are correct (unless told otherwise).
*/

VOID CPmwApp::update_paper_info(HDC in_hDC)
{
/*
// Update the printer info first.
// This will set margins, envelope orientation, etc.
*/

   m_pPortraitPrinterInfo->UpdateFromDevNames(m_hDevNames, m_hDevMode, in_hDC, PORTRAIT);
   m_pLandscapePrinterInfo->UpdateFromDevNames(m_hDevNames, m_hDevMode, in_hDC, LANDSCAPE);

   m_pPortraitPaperInfo->InitAsSheet(
      m_pPortraitPrinterInfo->PaperSize(),
      m_pPortraitPrinterInfo->OriginOffset(),
      m_pPortraitPrinterInfo->Margins(),
      m_pPortraitPrinterInfo->PaperType(),
      m_nMarginStyle);

   m_pLandscapePaperInfo->InitAsSheet(
      m_pLandscapePrinterInfo->PaperSize(),
      m_pLandscapePrinterInfo->OriginOffset(),
      m_pLandscapePrinterInfo->Margins(),
      m_pLandscapePrinterInfo->PaperType(),
      m_nMarginStyle);

/* Set the graphic object global "fast bitmap" setting. */

   GraphicObject::SetDirectBitmapPrinting(m_pPortraitPrinterInfo->FastBitmaps());

/* Update any existing documents if the paper changed size. */

// ** We always do this now. Some projects care about more than just size. **
// if (OldProjectDims.x != 0 &&
//          (OldProjectDims.x != NewProjectDims.x
//             || OldProjectDims.y != NewProjectDims.y))
   {
   /*
   // Now do our stuff.
   // Run through all document templates and see if any documents need refreshing.
   */

#ifdef WIN32
      POSITION tpos = GetFirstDocTemplatePosition();
#else
      POSITION tpos = m_templateList.GetHeadPosition();
#endif
      while (tpos)
      {
#ifdef WIN32
         CDocTemplate* pTemplate = GetNextDocTemplate(tpos);
#else
         CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(tpos);
#endif

         POSITION dpos = pTemplate->GetFirstDocPosition();
         while (dpos)
         {
            CPmwDoc* pDoc = (CPmwDoc *)pTemplate->GetNextDoc(dpos);

            pDoc->size_to_paper();
         }
      }
   }
}

/*
// A common routine to center a window on its parent (or the screen).
*/

PUBLIC VOID
center_window_on_parent(CWnd *window, BOOL center_on_screen)
{
   CWnd *parent = NULL;

/* Center on the desktop if no parent or that was requested. */

   if (center_on_screen)
   {
      parent = window->GetDesktopWindow();
   }
   else
   {
//    parent = window->GetParent();
   }

   window->CenterWindow(parent);
}

void CPmwApp::OnFilePrintSetup()
{
// CPrintDialog pd(TRUE);           // Print setup dialog.
   CPrintSetupDialog pd;            // Print setup dialog.

#ifndef WIN32
   pd.m_pd.Flags |= PD_ENABLESETUPTEMPLATE;
   pd.m_pd.lpSetupTemplateName = MAKEINTRESOURCE(IDD_PRINT_SETUP);
   pd.m_pd.hInstance = AfxGetResourceHandle();     // We need this
#endif

   if (DoPrintDialog(&pd) == IDOK)
   {
      update_paper_info();
   }
}

/*
// Generate a NID'ed file name.
// The passed name must have the form "_xxxYYYY.YYY", i.e. must have room
// (the 'xxx') for the nid number.
*/

VOID CPmwApp::nid_filename(LPSTR name)
{
   CHAR buffer[10];

   sprintf(buffer, "%03d", m_nUserID);
   memcpy(name+1, buffer, 3);
}

VOID CPmwApp::reset_doc_fonts(VOID)
{
   BOOL registered = user_is_registered();

#ifdef WIN32
   POSITION tpos = GetFirstDocTemplatePosition();
#else
   POSITION tpos = m_templateList.GetHeadPosition();
#endif
   while (tpos)
   {
#ifdef WIN32
      CDocTemplate* pTemplate = GetNextDocTemplate(tpos);
#else
      CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(tpos);
#endif

      POSITION dpos = pTemplate->GetFirstDocPosition();
      while (dpos)
      {
         CPmwDoc* pDoc = (CPmwDoc *)pTemplate->GetNextDoc(dpos);

         pDoc->reset_fonts();
         if (!registered)
         {
         /* We need to "turn on" the registration faces in the list. */
            pDoc->register_used_fonts();
         }
      }
   }
}

/*
// See if the user is registered.
*/

BOOL CPmwApp::user_is_registered(VOID)
{
   if(GetConfiguration()->SupportsRegistrationBonus())
		return RegisterDLL.IsBonusEnabled() == REGRESULT_AlreadyRegistered;
	return TRUE;
}

BOOL CPmwApp::HasNGImages(void)
{
   CString csMsregIni = GetPathManager()->ExpandPath("MSREG.INI");
   CIniFile MsregIniFile(csMsregIni);
	return MsregIniFile.GetInteger(SECTION_Configuration, "HasNGImages", FALSE);
}

BOOL CPmwApp::HasAGContent(void)
{
   CString csMsregIni = GetPathManager()->ExpandPath("MSREG.INI");
   CIniFile MsregIniFile(csMsregIni);
	return MsregIniFile.GetInteger(SECTION_Configuration, "HasAGContent", FALSE);
}

void CPmwApp::GetCopyrightString(CString& csCopyright)
{
   LPCSTR psz2LF = "\r\n\r\n";
	CString csPiece;
	TRY
	{
      if(GetConfiguration()->Product() != CPrintMasterConfiguration::plAmericanGreetings)
         {
		      LoadConfigurationString(IDS_MSCOPYRIGHT, csCopyright);
		      if (HasAGContent())
		      {
			      csCopyright += psz2LF;
			      LoadConfigurationString(IDS_AGDESIGNS_COPYRIGHT, csPiece);
			      csCopyright += csPiece;
		      }
            if (HasNGImages())
		      {
			      csCopyright += psz2LF;
			      LoadConfigurationString(IDS_NGCOPYRIGHT_1, csPiece);
			      csCopyright += csPiece;
			      LoadConfigurationString(IDS_NGCOPYRIGHT_2, csPiece);
			      csCopyright += csPiece;
			      LoadConfigurationString(IDS_NGCOPYRIGHT_3, csPiece);
			      csCopyright += csPiece;
		      }
      #if 0
		      if (!GetConfiguration()->IsAddOn())
		      {
			      csCopyright += psz2LF;
			      LoadConfigurationString(IDS_PHOTO_COPYRIGHT, csPiece);
			      csCopyright += csPiece;
			      csCopyright += psz2LF;
			      LoadConfigurationString(IDS_SPELLER_COPYRIGHT, csPiece);
			      csCopyright += csPiece;
		      }
      #endif
         }
      else
         {
            // American Greetings Copyright
			   LoadConfigurationString(IDS_MSONLY_COPYRIGHT, csCopyright);
			   csCopyright += psz2LF;
		      LoadConfigurationString(IDS_AGCOPYRIGHT, csPiece);
			   csCopyright += csPiece;
         }
	}
	END_TRY
}

#define REMINDER_DAYS_TO_WAIT 14          /* Two weeks (14 days) between. */

BOOL CPmwApp::TimeForReminder(void)
{
	BOOL fResult = FALSE;

	// Get the number of times we have issued our reminder.
   int nCount;

   if (m_first_time_installation)
   {
		// Always force it if just installing.
      nCount = 0;
   }
   else
   {
      nCount = OurGetProfileInt(SECTION_Configuration, ENTRY_ReminderCount, 0);
   }

	// Get the inst.dat name.
   CString csInstDatName;
	CString csInstDatFullName;
	CString csCount;

   TRY
	{
      LoadConfigurationString(IDS_INSTDAT_NAME, csInstDatName);
		csInstDatFullName = GetPathManager()->ExpandPath(csInstDatName);

		// Create a time stamp based on the creation time of INST.DAT.
		CTimeStamp RemindTime;
		if (RemindTime.SetFileCreate(csInstDatFullName))
		{
			// Get a time stamp of "now".
			CTimeStamp NowTime;
			if (NowTime.Set())
			{
				// Compute the time stamp of when we would want to next remind the user.
				RemindTime += CTimeStampSpan(nCount*REMINDER_DAYS_TO_WAIT, 0, 0, 0);

				// Check if it's time to remind the user.
				if (NowTime >= RemindTime)
				{
		         // Catch up to "now" so we will be sync'ed.
					while (NowTime >= RemindTime)
					{
						nCount++;
						RemindTime += CTimeStampSpan(REMINDER_DAYS_TO_WAIT, 0, 0, 0);
					}

					// Write out new count.
					csCount.Format("%d", nCount);
					::WritePrivateProfileString(SECTION_Configuration, ENTRY_ReminderCount, csCount, m_csMainIni);

					// We're want to remind the user.
					fResult = TRUE;
				}
			}
		}
	}
	END_TRY

	return fResult;
}

static void CheckProcessorType()
{
#ifdef _WIN32
   g_fProcessorIs386Compatible = TRUE;
#else
   DWORD dwWinFlags;

   dwWinFlags = GetWinFlags();

   g_fProcessorIs386Compatible =
      ((dwWinFlags & (WF_CPU386 | WF_CPU486 | WF_ENHANCED)) != 0);
#endif
}

/*
// Format a double precision value into a string with a maximum
// number of places after the decimal point. If there is no fractional
// part, no decimal point is included. Trailing zeros are eliminated.
// The number is rounded to the appropriate precision.
*/

void FormatDouble(double dNumber, int nPrecision, CString& csOutput)
{
   csOutput = "";
   
   // save sign
   BOOL bNegative = FALSE;
   
   if (dNumber < 0.0)
   {
      bNegative = TRUE;
      dNumber = -dNumber;
   }
   
   // compute minimum value that can be represented.  
   double dMin = 1.0;
   int i;

   for (i = 0; i < nPrecision; i++)
   {
      dMin *= 10.0;
   }
   
   // do one divide to minimize roundoff errors.
   dMin = 1.0/dMin;
   
   // round value for given precision
   dNumber += dMin/2.0;
      
   // if value is too small, return a zero
   if (dNumber < dMin)
   {
      csOutput = "0";
   }
   
   else
   {
      // check for negative number and add sign if necessary
      if (bNegative)
      {
         csOutput += '-';
         dNumber = -dNumber;
      }
   
      // put leading zero before decimal point.
      if (dNumber < 1.0)
      {
         csOutput += '0';
      }
      
      // normalize number (<1.0) and count digits to the left
      int nCount = 0;
      
      while (dNumber >= 1.0)
      {
         dNumber /= 10.0;
         nCount++;
      }
      
      // place digits into string
      
      int nTrim = csOutput.GetLength();
      
      nPrecision = -nPrecision;
      
      while (nCount > nPrecision)
      {
         if (nCount == 0)
         {
            csOutput += '.';
         }
         
         double dDigit;
         char cDigit;
         
         dNumber = modf(dNumber*10.0, &dDigit);
         csOutput += (cDigit = (char)((int)dDigit)+'0');
         
         if ((nCount > 0) || (cDigit != '0'))
         {
            nTrim = csOutput.GetLength();
         }
         
         nCount--;
      }
      
      csOutput = csOutput.Left(nTrim);
   }
}

/*
// Set the last picture name.
*/

VOID CPmwApp::set_last_picture_name(LPCSTR name)
{
   m_csLastPictureChosen = name;
}

/*
// Make a path from a path and a file name.
*/

PRIVATE VOID near
make_path(char *buffer, PCSTR path, PCSTR name)
{
/* Build the path. */

   strcpy(buffer, path);
   append_slash(buffer);
   strcat(buffer, name);
}

/*
// "Register" some files.
*/

PRIVATE BOOL near
register_files(PCSTR path, PCSTR old_spec, PCSTR new_ext)
{
   char name[_MAX_PATH];
   char new_name[_MAX_PATH];
   char *name_end, *new_name_end;
   int len;

/* Build the file spec. */

   make_path(name, path, "");
   len = strlen(name);

/* Set pointers for the end of the paths. */

   name_end = name + len;
   new_name_end = new_name + len;

/* Source and dest paths are the same. */

   strcpy(new_name, name);          /* Copy the path over. */

/* Search for all matching files. */

   strcpy(name_end, old_spec);

   CFileIterator Iter;
   BOOL fRet;

   for (fRet = Iter.FindFirst(name, _A_NORMAL);
        fRet;
        fRet = Iter.FindNext())
   {
      char *p;

   /* Build the names. */

      strcpy(name_end, Iter.Name());
      strcpy(new_name_end, Iter.Name());

      if ((p = strchr(new_name_end, '.')) != NULL)
      {
         strcpy(p+1, new_ext);
//       printf("%s -> %s\n", name, new_name);
         if (access_file(new_name) == 0)
         {
         /* File already exists. */
            if (unlink(name) != 0)
            {
               return FALSE;
            }
         }
         else
         {
            if (rename(name, new_name) != 0)
            {
               return FALSE;
            }
         }
      }
   }
   return TRUE;
}

void CPmwApp::register_free_files()
{
   BeginWaitCursor();
	CString csPictureDirectory = GetPathManager()->ExpandPath("[[P]]");
   register_files(csPictureDirectory, "*.CGN", "CGM");
   EndWaitCursor();
}

/*
// Get a profile string, looking in both the nid'd INI file and PMW.INI
*/

CString CPmwApp::OurGetProfileString(LPCSTR lpszSection, LPCSTR lpszEntry, LPCSTR lpszDefault)
{
   CString string;

   if (lpszDefault == NULL)
   {
   /* Don't pass in NULL! */
      lpszDefault = "";
   }

/* First read in the main INI file. */

   char *sb = string.GetBuffer(_MAX_PATH);
   int cb = ::GetPrivateProfileString(lpszSection,
                             lpszEntry,
                             lpszDefault,
                             sb,
                             _MAX_PATH,
                             m_csMainIni);
   string.ReleaseBuffer(cb);

/* Now look in the nid'd INI file. */

   return GetProfileString(lpszSection, lpszEntry, string);
}

/*
// Get a profile int, looking in both the nid'd INI file and PMW.INI
*/

UINT CPmwApp::OurGetProfileInt(LPCSTR lpszSection, LPCSTR lpszEntry, UINT nDefault)
{
/* First read in the main INI file. */

   nDefault = ::GetPrivateProfileInt(lpszSection,
                                     lpszEntry,
                                     nDefault,
                                     m_csMainIni);

/* Now look in the nid'd INI file. */

   return GetProfileInt(lpszSection, lpszEntry, nDefault);
}

/*
// Get a profile color, looking in both the nid'd INI file and PMW.INI
*/

COLORREF CPmwApp::OurGetProfileColor(LPCSTR pszSection, LPCSTR pszEntry, COLORREF clrDefault)
{
	CString csColor = OurGetProfileString(pszSection, pszEntry, "");
	if (!csColor.IsEmpty())
	{
		CEnhancedWordArray cewaValues;
		cewaValues.Parse(csColor);
		if (cewaValues.GetSize() == 3)
		{
			clrDefault = PALETTERGB(cewaValues.GetAt(0),
											cewaValues.GetAt(1),
											cewaValues.GetAt(2));
		}
	}
	return clrDefault;
}

#ifdef _OLD_DEBUG_
static DWORD dwNewAlloced = 0L;
static size_t the_magic_new_size = 0;

void far *operator new(size_t size)
{
   void far *mem = ::malloc(size);

   if (mem != NULL)
   {
      size = ::_msize(mem);
      if (size == the_magic_new_size)
      {
         od("This is it!\r\n");
      }
      dwNewAlloced += size;
      od("new %x (%lx)\r\n", size, dwNewAlloced);
   }
   return mem;
}

void operator delete(void far *p)
{
   if (p != NULL)
   {
      size_t size = ::_msize(p);
      dwNewAlloced -= size;
      od("delete %x (%lx)\r\n", size, dwNewAlloced);
   }
   ::free(p);
}
#endif

/*
// Write value of m_csAddressBook to _???PMW.INI file.
*/

void CPmwApp::WriteAddressBook(void)
{
   if (!m_csAddressBook.IsEmpty())
   {
      WriteProfileString(SECTION_Configuration, ENTRY_AddressBook, m_csAddressBook);
   }
   if (!m_csSenderBook.IsEmpty())
   {
      WriteProfileString(SECTION_Configuration, ENTRY_SenderBook, m_csSenderBook);
   }
}

/////////////////////////////////////////////////////////////////////////////
// General utilities.

static const char BASED_CODE ENTRY_Valid[] = "Valid";
static const char BASED_CODE ENTRY_Maximized[] = "Maximized";
static const char BASED_CODE ENTRY_X0[] = "X0";
static const char BASED_CODE ENTRY_Y0[] = "Y0";
static const char BASED_CODE ENTRY_Width[] = "Width";
static const char BASED_CODE ENTRY_Height[] = "Height";
static const char BASED_CODE ENTRY_ScreenWidth[] = "ScreenWidth";
static const char BASED_CODE ENTRY_ScreenHeight[] = "ScreenHeight";

/*
// Load a window's position from the user's INI file.
// The window is considered shown after this call.
*/

BOOL CPmwApp::LoadWindowPosition(CWnd* pWnd,
                                 LPCSTR pszSection,
                                 int nCmdShow /*=SW_SHOWNORMAL*/,
                                 BOOL fNoResize /*=FALSE*/,
                                 BOOL fSkipIfInvalid /*=TRUE*/)
{
   if (pszSection == NULL
         || (!GetProfileInt(pszSection, ENTRY_Valid, FALSE)
               && fSkipIfInvalid))
   {
      pWnd->ShowWindow(SW_SHOW);
      return FALSE;           // Not positioned.
   }

   if (nCmdShow != SW_SHOWNORMAL)
   {
      pWnd->ShowWindow(nCmdShow);
   }
   else
   {
      int nScreenXSize = GetSystemMetrics(SM_CXSCREEN);
      int nScreenYSize = GetSystemMetrics(SM_CYSCREEN);

      int fMaximize = GetProfileInt(pszSection, ENTRY_Maximized, TRUE);

      if (fMaximize)
      {
         if (::GetWindowLong(pWnd->GetSafeHwnd(), GWL_STYLE) & WS_MAXIMIZEBOX)
         {
            pWnd->ShowWindow(SW_SHOWMAXIMIZED);
         }
         else
         {
         /* Leave the window as is (dialog perhaps?). */
            pWnd->ShowWindow(SW_SHOW);
         }
      }
      else
      {
      /* Get all the old information. Use appropriate defaults. */

         int nOldScreenXSize = GetProfileInt(pszSection, ENTRY_ScreenWidth, nScreenXSize);
         int nOldScreenYSize = GetProfileInt(pszSection, ENTRY_ScreenHeight, nScreenYSize);

         int nXSize = GetProfileInt(pszSection, ENTRY_Width, 640);
         int nYSize = GetProfileInt(pszSection, ENTRY_Height, 480);
         int nX0 = GetProfileInt(pszSection, ENTRY_X0, (nOldScreenXSize-nXSize)/2);
         int nY0 = GetProfileInt(pszSection, ENTRY_Y0, (nOldScreenYSize-nYSize)/2);

      /* Scale to the new screen size. */

         if (nScreenXSize != nOldScreenXSize)
         {
            nX0 = MulDiv(nX0, nScreenXSize, nOldScreenXSize);
            nXSize = MulDiv(nXSize, nScreenXSize, nOldScreenXSize);
         }

         if (nScreenYSize != nOldScreenYSize)
         {
            nY0 = MulDiv(nY0, nScreenYSize, nOldScreenYSize);
            nYSize = MulDiv(nYSize, nScreenYSize, nOldScreenYSize);
         }

      /* If we are told not to resize, then just position it. */

         if (fNoResize)
         {
            CRect crBounds;
            pWnd->GetWindowRect(&crBounds);

            nXSize = crBounds.Width();
            nYSize = crBounds.Height();
         }

      /* Make sure we fit on the screen. */

         if (nXSize > nScreenXSize)
         {
            nXSize = nScreenXSize;
         }
         if (nYSize > nScreenYSize)
         {
            nYSize = nScreenYSize;
         }

      /* Set the position. Force it to be entirely on the screen. */

         if (nX0 < 0)
         {
            nX0 = 0;
         }
         if (nY0 < 0)
         {
            nY0 = 0;
         }
         if (nX0 + nXSize > nScreenXSize)
         {
            nX0 = nScreenXSize - nXSize;
         }
         if (nY0 + nYSize > nScreenYSize)
         {
            nY0 = nScreenYSize - nYSize;
         }

      /* Set the window here. */

         pWnd->MoveWindow(nX0, nY0, nXSize, nYSize);
         pWnd->ShowWindow(SW_SHOW);
      }
   }
   return TRUE;
}

/*
// Save a window's position to the user's INI file.
*/

void CPmwApp::SaveWindowPosition(CWnd* pWnd, LPCSTR pszSection)
{
   if (pszSection == NULL || pWnd->IsIconic())
   {
   /* Do nothing. */
      return;
   }

   WriteProfileInt(pszSection, ENTRY_Maximized, pWnd->IsZoomed());

   CRect crBounds;
   pWnd->GetWindowRect(&crBounds);

   WriteProfileInt(pszSection, ENTRY_X0, crBounds.left);
   WriteProfileInt(pszSection, ENTRY_Y0, crBounds.top);
   WriteProfileInt(pszSection, ENTRY_Width, crBounds.Width());
   WriteProfileInt(pszSection, ENTRY_Height, crBounds.Height());

   WriteProfileInt(pszSection,
                   ENTRY_ScreenWidth,
                   GetSystemMetrics(SM_CXSCREEN));
   WriteProfileInt(pszSection,
                   ENTRY_ScreenHeight,
                   GetSystemMetrics(SM_CYSCREEN));

   WriteProfileInt(pszSection, ENTRY_Valid, TRUE);
}

/*
// Initialize a CIniFile object for an INI file in the PrintMaster directory with the
// given name. This stuff is put in PMW to keep CIniFile free of PrintMaster dependancies.
//
// For non-nidded file names, pszName should be:  YYYYYYYY.YYY  (normal 8.3)
// For nidded file names, pszName should be:      _000YYYY.YYY  (000 will be replaced with NID)
*/

BOOL CPmwApp::InitPrintMasterIniFile(CIniFile* pIniFile, LPCSTR pszName, BOOL fNidIt /*=FALSE*/)
{
   BOOL fResult = FALSE;

   if (pIniFile != NULL)
   {
      char szIniFileName[_MAX_PATH];
      strcpy(szIniFileName, pszName);

      // "Nid" the filename is the user wants.
      if (fNidIt)
      {
         nid_filename(szIniFileName);
      }

      // Construct the full path name.
      TRY
      {
         pIniFile->Name(GetPathManager()->ExpandPath(szIniFileName));  // Can throw an exception.
         fResult = TRUE;
      }
      END_TRY
   }

   return fResult;
}

void CPmwApp::OnUpdateFileNewOpen(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_pMainWnd != NULL);
}

CPrinterInfo* CPmwApp::GetPrinterInfo(int nOrientation)
{
   return (nOrientation == PORTRAIT) ? m_pPortraitPrinterInfo : m_pLandscapePrinterInfo;
}

CPaperInfo* CPmwApp::GetPaperInfo(int nOrientation)
{
   return (nOrientation == PORTRAIT) ? m_pPortraitPaperInfo : m_pLandscapePaperInfo;
}

BOOL CPmwApp::OnDDECommand(LPTSTR lpszCommand)
{
	if (m_fInitialized)
	{
		   // Do the normal thing.
		CWinApp::OnDDECommand(lpszCommand);
		return TRUE;
	}
	else
	{
		m_csaDocsToOpen.Add(lpszCommand);
		//    OpenDocumentFile(lpszCommand);
		return TRUE;
	}
}

/*
// The configuration has changed.
// Do anything necessary to make the program reflect the new configuration.
*/

void CPmwApp::ChangedConfiguration(void)
{
   // Set the ProductFlags in the collection.
   CPMWCollection::SetProductFlags(GetConfiguration()->ProductFlags());

   /*
   // Register any document templates which weren't before.
   */

   RegisterDocTemplates();

   /*
   // Fixup the main window if there is one.
   */

   CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
   if (pMainWnd != NULL)
   {
      pMainWnd->FixupConfigurationNames();
      pMainWnd->SendMessage(WM_FONTCHANGE);
   }

   /*
   // Upgrade the icon (always).
   */

   UpgradeIcon(FALSE);

	m_pCollectionManager->UpdateFreeCollection(user_is_registered());
}

void CPmwApp::UserHasRegistered(void)
{
	register_free_files();
	((CMainFrame*)AfxGetMainWnd())->RebuildFonts();
	m_pCollectionManager->UpdateFreeCollection(user_is_registered());
}

// Functions for getting and setting the current sender Id.
DWORD CPmwApp::GetSenderId(void)
{
   DWORD dwSenderId = (DWORD)-1;

   CIniFile IniFile;
   TRY
   {
      IniFile.Name(GET_PMWAPP()->m_pszProfileName);
//    if (InitPrintMasterIniFile(&IniFile, "_000PMW.INI", TRUE))
      {
         dwSenderId = IniFile.GetDword("SenderInformationDialog", "SenderId", (DWORD)-1);
      }
   }
   END_TRY
   
   return dwSenderId;
}

void CPmwApp::SetSenderId(DWORD dwSenderId)
{
   CIniFile IniFile;
   TRY
   {
      IniFile.Name(GET_PMWAPP()->m_pszProfileName);
//    if (InitPrintMasterIniFile(&IniFile, "_000PMW.INI", TRUE))
      {
         dwSenderId = IniFile.WriteDword("SenderInformationDialog", "SenderId", dwSenderId, (DWORD)-1);
      }
   }
   END_TRY
}

void CPmwApp::SetShowRulers(BOOL fShowRulers)
{
   m_fShowRulers = fShowRulers;
   AfxGetApp()->WriteProfileInt(SECTION_Configuration, ENTRY_ShowRulers, fShowRulers);

   POSITION tpos = GetFirstDocTemplatePosition();
   while (tpos)
   {
      CDocTemplate* pTemplate = GetNextDocTemplate(tpos);
      POSITION dpos = pTemplate->GetFirstDocPosition();
      while (dpos)
      {
         CDocument* pDoc = pTemplate->GetNextDoc(dpos);
         pDoc->UpdateAllViews(NULL, HINT_Rulers, NULL);
      }
   }
}

void CPmwApp::SetSnapToRulers(BOOL fSnapToRulers)
{
   m_fSnapToRulers = fSnapToRulers;
   AfxGetApp()->WriteProfileInt(SECTION_Configuration, ENTRY_SnapToRulers, fSnapToRulers);
}

void CPmwApp::SetHintFullLengthBmp(BOOL fSetBmp)
{
   m_fHintFullLengthBmp = fSetBmp;
   AfxGetApp()->WriteProfileInt(SECTION_Configuration, ENTRY_HintFullLengthBmp, fSetBmp);
}

void CPmwApp::SetShowGuides(BOOL fShowGuides)
{
   m_fShowGuides = fShowGuides;
   AfxGetApp()->WriteProfileInt(SECTION_Configuration, ENTRY_ShowGuides, fShowGuides);

   POSITION tpos = GetFirstDocTemplatePosition();
   while (tpos)
   {
      CDocTemplate* pTemplate = GetNextDocTemplate(tpos);
      POSITION dpos = pTemplate->GetFirstDocPosition();
      while (dpos)
      {
         CDocument* pDoc = pTemplate->GetNextDoc(dpos);
         pDoc->UpdateAllViews(NULL, NULL, NULL);
      }
   }
}

void CPmwApp::SetSnapToGuides(BOOL fSnapToGuides)
{
   m_fSnapToGuides = fSnapToGuides;
   AfxGetApp()->WriteProfileInt(SECTION_Configuration, ENTRY_SnapToGuides, fSnapToGuides);
}

void CPmwApp::SetLockGuides(BOOL fLockGuides)
{
   m_fLockGuides = fLockGuides;
   AfxGetApp()->WriteProfileInt(SECTION_Configuration, ENTRY_LockGuides, fLockGuides);
}

void CPmwApp::SetTextBoxOutlines(BOOL fShow)
{
   m_fTextBoxOutlines = fShow;
   AfxGetApp()->WriteProfileInt(SECTION_Configuration, ENTRY_TextBoxOutlines, fShow);

   POSITION tpos = GetFirstDocTemplatePosition();
   while (tpos)
   {
      CDocTemplate* pTemplate = GetNextDocTemplate(tpos);
      POSITION dpos = pTemplate->GetFirstDocPosition();
      while (dpos)
      {
         CDocument* pDoc = pTemplate->GetNextDoc(dpos);
         pDoc->UpdateAllViews(NULL, NULL, NULL);
      }
   }
}

// Helpful Hint functions
BOOL CPmwApp::ShowHelpfulHint(int nType, int nWhich, CWnd* pParent/*=NULL*/, BOOL fModal/*=FALSE*/)
{
	return m_pHelpfulHint->ShowHelpfulHint((HINT_TYPE)nType, nWhich, pParent, fModal);
}

BOOL CPmwApp::ShowHelpfulHint(const CString& strWhich, CWnd* pParent/*=NULL*/, BOOL fModal/*=FALSE*/)
{
	return m_pHelpfulHint->ShowHelpfulHint(strWhich, pParent, fModal);
}

BOOL CPmwApp::OnShowHintOnBorderDlg()
{
	return m_pHelpfulHint->ShowHelpfulHint("BorderPlus!");
}

void CPmwApp::RemoveHelpfulHint()
{
	m_pHelpfulHint->RemoveHelpfulHint();
}

void CPmwApp::ShowRecentHintsDialog()
{
	m_pHelpfulHint->ShowRecentHintsDialog();
}

void CPmwApp::ResetHelpfulHints()
{
	m_pHelpfulHint->ResetHints();
}

void CPmwApp::EnableHelpfulHints(BOOL enable /*=TRUE*/)
{
	m_pHelpfulHint->EnableHelpfulHints(enable);
}
BOOL CPmwApp::HelpfulHintsEnabled()
{
	return m_pHelpfulHint->HelpfulHintsEnabled();
}

void CPmwApp::EnableDesignTips(BOOL enable /*=TRUE*/)
{
	m_pHelpfulHint->EnableDesignTips(enable);
}
BOOL CPmwApp::DesignTipsEnabled()
{
	return m_pHelpfulHint->DesignTipsEnabled();
}

void CPmwApp::EnableHintsVoice(BOOL enable /*=TRUE*/)
{
	m_pHelpfulHint->EnableHintsVoice(enable);
}
BOOL CPmwApp::HintsVoiceEnabled()
{
	return m_pHelpfulHint->HintsVoiceEnabled();
}

CBEInterface* CPmwApp::GetBorderEditorInterface()
{
	if (m_pBorderEditor && m_pBorderEditor->IsInitialized())
		return m_pBorderEditor->GetBEInterface();
	else
		return NULL;
}

BOOL CPmwApp::BorderEditorInitialized()
{
	return (m_pBorderEditor && m_pBorderEditor->IsInitialized());
}

void CPmwApp::AddTmpFileName(const CString& csName)
{
   // Look in the list.
   for (int n = 0; n < m_csaTmpFileNames.GetSize(); n++)
   {
      if (m_csaTmpFileNames[n].CompareNoCase(csName) == 0)
      {
         return;
      }
   }
   m_csaTmpFileNames.Add(csName);
}


/////////////////////////////////////////////////////////////////////////////
// MRU code.

//
// Our version of LoadStdProfileSettings to set up our MRU.
//

void CPmwApp::LoadStdProfileSettings(void)
{
#ifdef WIN32
   CWinApp::LoadStdProfileSettings(0);    // No MRU.
#else
   CWinApp::LoadStdProfileSettings();
#endif   
   m_pRecentFileList = new CPMWRecentFileList(0, szFileSection, szFileEntry, 4);
   m_pRecentFileList->ReadList();
}

void CPmwApp::OnUpdateRecentFileMenu(CCmdUI* pCmdUI)
{
   ASSERT_VALID(this);
   if (m_pRecentFileList == NULL) // no MRU files
      pCmdUI->Enable(FALSE);
   else
      m_pRecentFileList->UpdateMenu(pCmdUI);
}

BOOL CPmwApp::OnOpenRecentFile(UINT nID)
{
   ASSERT_VALID(this);
   ASSERT(m_pRecentFileList != NULL);

   ASSERT(nID >= ID_FILE_MRU_FILE1);
   ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
   int nIndex = nID - ID_FILE_MRU_FILE1;
   ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

   TRACE2("MRU: open file (%d) '%s'.\n", (nIndex) + 1,
         (LPCTSTR)(*m_pRecentFileList)[nIndex]);

   // Open this document.
   if (OpenDocumentFile((*m_pRecentFileList)[nIndex]) == NULL)
   {
      m_pRecentFileList->Remove(nIndex);
   }

   return TRUE;
}

void CPmwApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
   ASSERT_VALID(this);
   ASSERT(lpszPathName != NULL);
   ASSERT(AfxIsValidString(lpszPathName));

   if (m_pRecentFileList != NULL)
   {
//    // fully qualify the path name
//    CString csFullPath = GetPathManager()->ExpandPath(lpszPathName);

      // then add to recent file list
      m_pRecentFileList->Add(lpszPathName);
   }
}

//
// Get the user settings. No refs added.
//

IUserSettings* CPmwApp::GetUserSettings(void)
{
	IUserSettings* pIUserSettings = NULL;
	if (m_pUserSettings != NULL)
	{
		// Get the interface. This does an AddRef.
		pIUserSettings = (IUserSettings*)m_pUserSettings->GetInterface(&IID_IUserSettings);
	}
	return pIUserSettings;
}

//
// Get the drive manager. No refs added.
//

IDriveManager* CPmwApp::GetDriveManager(void)
{
	IDriveManager* pIDriveManager = NULL;
	if (m_pDriveManager != NULL)
	{
		// Get the interface.
		pIDriveManager = (IDriveManager*)m_pDriveManager->GetInterface(&IID_IDriveManager);
	}
	return pIDriveManager;
}

//
// Get the path manager. No refs added.
//

IPathManager* CPmwApp::GetPathManager(void)
{
	IPathManager* pIPathManager = NULL;
	if (m_pPathManager != NULL)
	{
		// Get the interface.
		pIPathManager = (IPathManager*)m_pPathManager->GetInterface(&IID_IPathManager);
	}
	return pIPathManager;
}

//
// Get the collection manager. No refs added.
//

ICollectionManager* CPmwApp::GetCollectionManager(void)
{
	ICollectionManager* pICollectionManager = NULL;
	if (m_pCollectionManager != NULL)
	{
		// Get the interface.
		pICollectionManager = (ICollectionManager*)m_pCollectionManager->GetInterface(&IID_ICollectionManager);
	}
	return pICollectionManager;
}

//
// Get the content server. No refs added.
//

IContentServer* CPmwApp::GetContentServer(void)
{
	IContentServer* pIContentServer = NULL;
#ifndef	ARTSTORE
	if (m_pContentServer != NULL)
	{
		// Get the interface.
		pIContentServer = (IContentServer*)m_pContentServer->GetInterface(&IID_IContentServer);
	}
#endif
	return pIContentServer;
}

// Get the media prompt notify object. No refs added.
IMediaPromptNotify* CPmwApp::GetMediaPromptNotify(void)
{
	// Get the interface. It's one of the app's interfaces.
	return (IMediaPromptNotify*)GetInterface(&IID_IMediaPromptNotify);
}

//
// Get the content manager. No refs added.
//

IContentManager* CPmwApp::GetContentManager(void)
{
	IContentManager* pIContentManager = NULL;
	if (m_pContentManager != NULL)
	{
		// Get the interface.
		pIContentManager = (IContentManager*)m_pContentManager->GetInterface(&IID_IContentManager);
	}
	return pIContentManager;
}

//
// Get the connection manager. No refs added.
//

IConnectionManager* CPmwApp::GetConnectionManager(void)
{
	IConnectionManager* pIConnectionManager = NULL;
	if (m_pConnectionManager != NULL)
	{
		// Get the interface.
		pIConnectionManager = (IConnectionManager*)m_pConnectionManager->GetInterface(&IID_IConnectionManager);
	}
	return pIConnectionManager;
}

/*
// Update the picture paths to include the path for this picture if the
// path is not already included in the paths list.
*/

void CPmwApp::UpdatePicturePaths(LPCSTR pszNewName)
{
	int16 count = m_csaPictureSearchPaths.GetSize();

	for (int16 i = 0; i < count; i++)
	{
		if (m_csaPictureSearchPaths[i] == pszNewName)
		{
			return;
		}
	}

	TRY
	{
		m_csaPictureSearchPaths.Add(pszNewName);
	}
	CATCH_ALL (e)
	{
		return;
	}
	END_CATCH_ALL
}

//
// Reopen the collection manager when things have changed.
//

void CPmwApp::ReopenCollectionManager(void)
{
	CWaitCursor wait;
	m_pCollectionManager->Reopen();
}

// Update the shared paths.
void CPmwApp::UpdateSharedPaths(void)
{
	m_pCollectionManager->UpdateSharedPaths();
}

/////////////////////////////////////////////////////////////////////////////
// Interface map for CPmwApp.

BEGIN_INTERFACE_MAP(CPmwApp, CCmdTarget)
	INTERFACE_PART(CPmwApp, IID_IMediaPromptNotify, MediaPromptNotify)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUnknown part of the IMediaNofiy interface.
// This is a specialized implementation of IUnknown. It's a little strange,
// but I think it'll work the way we want.

STDMETHODIMP_(ULONG) CPmwApp::XMediaPromptNotify::AddRef()
{
	// The standard method for this calls ExternalAddRef on "pThis".
	// However, we don't really want to app to have reference counts.
	// So, we don't do anything in this function.
	return 1;
}

STDMETHODIMP_(ULONG) CPmwApp::XMediaPromptNotify::Release()
{
	// The standard method for this calls ExternalAddRef on "pThis".
	// However, we don't really want to app to have reference counts.
	// So, we don't do anything in this function.
	return 1;
}

STDMETHODIMP CPmwApp::XMediaPromptNotify::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
	// The standard method for this calls ExternalQueryInterface on "pThis".
	// However, we don't really want to app to have reference counts.
	// So, we Release the count back if we successfully get the interface.
	METHOD_PROLOGUE(CPmwApp, MediaPromptNotify)
	DWORD dwResult = pThis->ExternalQueryInterface(&iid, ppvObj); 

	// If we got the interface, set the reference count back.
	if (dwResult == S_OK)
	{
		pThis->InternalRelease();
	}

	return (HRESULT)dwResult;
}

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IMediaPromptNotify interface for CPmwApp
//
// returns TRUE if the home CD was found, FALSE if it wasn't
//
STDMETHODIMP_(BOOL) CPmwApp::XMediaPromptNotify::PromptForHomeCD()
{
	METHOD_PROLOGUE(CPmwApp, MediaPromptNotify)

	if (bHomeCDNotAvailable)	
	{
		return FALSE;		// don't even try
	}

	// Build the message string.
// JRO
/*
   wsprintf(buffer, "Please insert the PrintMaster Gold CD\n"
					 "into any drive and press OK.\n"
					 "\n"
					 "Press Cancel if the disk is not available");
*/
   CString csMessage;
   LoadConfigurationString(IDS_PLEASE_INSERT_CD_GOLD, csMessage);
   CString csTitle;
   LoadConfigurationString(IDS_INSERT_CD_GOLD, csTitle);

	// Put up the message box.
   CPromptDlg dlg(csTitle, csMessage);
	AfxLockTempMaps();
	UINT uResult = dlg.DoModal();
	AfxUnlockTempMaps(FALSE);

	// return TRUE if user pressed OK
	return uResult == IDOK;
}

STDMETHODIMP_(BOOL) CPmwApp::XMediaPromptNotify::PromptForVolume(int nDriveType, LPCSTR pszVolume, LPCSTR pszMediaName, LPCSTR pszFileName)
{
	METHOD_PROLOGUE(CPmwApp, MediaPromptNotify)

	// Build the volume label component of the message.
	// Compute whether we have a friendly name or not.

   BOOL fFriendlyVolume = FALSE;
	CString csVolume;
	if (pszMediaName[0] == '\0')
	{
		// JRO
		//sprintf(volume_piece, "(labeled '%s') ", csName);
		csVolume.Format(pThis->GetResourceStringPointer(IDS_LABELED_S), pszVolume);
	}
	else
	{
		csVolume.Format("'%s'", pszMediaName);
		fFriendlyVolume = TRUE;
	}

	// Build the message string.

	CString csMessage;
// JRO
/*
   wsprintf(buffer, "Please insert the %s %scontaining\n"
                     "%s\n"
                     "into any drive and press OK.\n"
                     "\n"
                     "Press Cancel if the disk is not available",
               (LPCSTR)((drive_type == DT_CDROM) ? "CD" : "disk"),
               (LPCSTR)volume_piece,
               (LPCSTR)file_name);
*/
	csMessage.Format(pThis->GetResourceStringPointer (fFriendlyVolume ? IDS_PLEASE_INSERT_CD_FRIENDLY : IDS_PLEASE_INSERT_CD),
						  (LPCSTR)((nDriveType == DT_CDROM)
									  ? pThis->GetResourceStringPointer(IDS_CD)
									  : pThis->GetResourceStringPointer(IDS_DISK)),
						  (LPCSTR)csVolume,
						  pszFileName);

   // If we are going to be prompting for a CD, turn off any sounds
   // that may be playing from the home CD.
   if (nDriveType == DT_CDROM)
   {
		pThis->m_MciWnd.stop_sound();
   }
   
	if(!((CMainFrame*)AfxGetMainWnd())->GetAppClosingDown())
	{
	   CString csTitle = pThis->GetResourceStringPointer (IDS_INSERT_DISK);
	   CPromptDlg dlg(csTitle, csMessage);
		AfxLockTempMaps();
		UINT uResult = dlg.DoModal();
		AfxUnlockTempMaps(FALSE);

		if (uResult == IDOK)
	   {
		  return TRUE;
	   }
	}

	// Otherwise, just cancel for now.
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// PMW-specific implementations of the PMWCFG stuff (which INSTALL cannot use).

// Can we instantly upgrade?

BOOL CPrintMasterConfiguration::CanInstantUpgrade(void)
{
   CIniFile MsregIniFile;
   GET_PMWAPP()->InitPrintMasterIniFile(&MsregIniFile, "MSREG.INI");

   return MsregIniFile.GetInteger(SECTION_Configuration, "CanInstantUpgrade", TRUE);
}

BOOL CPrintMasterConfiguration::IsNetworkVersion(void)
{
	// PMGTODO: Either fix this HACK so it's not tied to a particular
	// set of parent part numbers, or add the 7.0 PPN to the list.
	// We could use another key in MSREG.INI to indicate "networkness".
   CString csPPN = GET_PMWAPP()->GetParentPartNumber();
   return csPPN.CompareNoCase("112590") == 0;
}

void CDemoConfiguration::GetDemoIniFileName(CString& csFileName)
{
   TRY
      csFileName = GetGlobalPathManager()->ExpandPath(DemoIniFileName);
   END_TRY
}

#define  FLIPPED  (OBJECT_FLAG_xflipped | OBJECT_FLAG_yflipped)

static void StuffText(LPCSTR pText, CTxp* pTxp)
{
   pTxp->InsertString(pText);
}

BOOL CDemoConfiguration::SupportsCardBack(BOOL fComplain /*=TRUE*/, CPmwDoc* pDoc /*=NULL*/)
{
   if (pDoc != NULL)
   {
      GroupObjectPtr pPanel = ((CCardDoc*)pDoc)->GetPanelGroup(CARD_PANEL_Back);
      if (pPanel != NULL)
      {
         ProjectInfo& Info = pDoc->GetProjectInfo();

         BOOL fFlipped = (Info.pip[CARD_PANEL_Back].flags & FLIPPED) != 0;

      /* Make sure the panel is blank. */

         pPanel->destroy_objects();

      /* Add our objects to this panel. */

         FRAMEOBJ_CREATE_STRUCT fcs;
         CFrameObject* pFrame;
         PBOX Bound = pPanel->get_bound();
         PCOORD PanelDX = Bound.x1 - Bound.x0;
         PCOORD PanelDY = Bound.y1 - Bound.y0;
         
         PCOORD xgap = PanelDX/8;

         fcs.bound.x0 = Bound.x0 + xgap;
         fcs.bound.x1 = Bound.x1 - xgap;

         if (fFlipped)
         {
            fcs.bound.y0 = Bound.y0;
            fcs.bound.y1 = Bound.y1 - PanelDY/2;
         }
         else
         {
            fcs.bound.y0 = Bound.y0 + PanelDY/2;
            fcs.bound.y1 = Bound.y1;
         }

         fcs.alignment = ALIGN_center;
         fcs.vert_alignment = ALIGN_bottom;
//       fcs.text = NULL;
         fcs.flags = 0;
//       fcs.pStyle = &Style;

         if ((pFrame = pDoc->create_frame_object(&fcs)) != NULL)
         {
            // JRO
/*            
            static char BASED_CODE Text1[] =
               "Published exclusively by\n"
               "PrintMaster Gold CD Publishing Suite.\n"
               "Copyright 1996 MicroLogic Software,\n"
               "a division of Mindscape.\n"
               "All rights reserved.\n\n";
            static char BASED_CODE Text2[] =
               "For more information, call 1-800-888-9078.";
*/
            CTextStyle Style(pDoc->get_database());
            Style.SetDefault();
            int nFace = typeface_server.find_face("GenevaNrw");
            Style.Font(((PMGFontServer*)pDoc->get_font_server())->font_face_to_record(nFace));
            Style.Size(MakeFixed(10));
            Style.BaseSize(MakeFixed(10));
            Style.UpdateFontMetrics();

            CTxp Txp(NULL);

            Txp.Init(pFrame, 0);
            Txp.SetHorizontalAlignment(fcs.alignment);
            Txp.Style(Style);

            // JRO StuffText(Text1, &Txp);
            StuffText(GET_PMWAPP()->GetResourceStringPointer (IDS_PUBLISHED_BY), &Txp);

            Txp.Style().Bold(TRUE);
            Txp.Style().UpdateFontMetrics();
            // JRO StuffText(Text2, &Txp);
            StuffText(GET_PMWAPP()->GetResourceStringPointer (IDS_FOR_INFO_CALL), &Txp);

            if (fFlipped)
            {
               pFrame->xflip();
               pFrame->yflip();
            }
            pPanel->object_list()->append(pFrame);
         }
      /*
      // When we're done adding all the objects...
      // Generate all the correct coordinates for the objects and the group (the object needs
      // its "group" variables set correctly).
      */
         pPanel->establish(FALSE);
      }
   }
   else if (fComplain)
   {
      // JRO UnsupportedDialog(szEditBackOfCardNotAvailable);
      UnsupportedDialog(GET_PMWAPP()->GetResourceStringPointer (uEditBackOfCardNotAvailable));
   }
   return FALSE;
}

BOOL CDemoConfiguration::UnsupportedDialog(LPCSTR pMessage /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
/*
// Prevent re-entrancy.
*/
   static BOOL fInHere = FALSE;
   CBoolSemaphore Sem(fInHere);
   if (Sem.Lock())
   {
      if (pMessage == NULL)
      {
         // JRO pMessage = szFeatureNotAvailable;
         pMessage = GET_PMWAPP()->GetResourceStringPointer (uFeatureNotAvailable);
      }

      if (CanInstantUpgrade())
      {
         CNeedToUpgradeDialog Dialog(pMessage, pTitle);
         
         if (Dialog.DoModal() == IDOK)
         {
            return ((CMainFrame*)AfxGetMainWnd())->DoUpgrade();
         }
      }
      else
      {
         CDemoVersionDialog Dialog(pMessage, pTitle);
         
         if (Dialog.DoModal() == IDOK)
         {
            return ((CMainFrame*)AfxGetMainWnd())->DoUpgrade();
         }
      }
   }
   return FALSE;
}

BOOL CPrintStandardConfiguration::UnsupportedDialog(LPCSTR pMessage /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
/*
// Prevent re-entrancy.
*/
   static BOOL fInHere = FALSE;
   CBoolSemaphore Sem(fInHere);
   if (Sem.Lock())
   {
      if (pMessage == NULL)
      {
         // JRO pMessage = szFeatureNotAvailable;
         pMessage = GET_PMWAPP()->GetResourceStringPointer (uFeatureNotAvailable);
      }

      if (CanInstantUpgrade())
      {
         CNeedToUpgradeDialog Dialog(pMessage, pTitle);
         
         if (Dialog.DoModal() == IDOK)
         {
            return ((CMainFrame*)AfxGetMainWnd())->DoUpgrade();
         }
      }
      else
      {
         CDialog Dialog(IDD_NON_INSTANT_UPGRADE);

         Dialog.DoModal();
      }
   }
   return FALSE;
}

BOOL CBonusPackConfiguration::UnsupportedDialog(LPCSTR pMessage /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
/*
// Prevent re-entrancy.
*/
   static BOOL fInHere = FALSE;
   CBoolSemaphore Sem(fInHere);
   if (Sem.Lock())
   {
      if (pMessage == NULL)
      {
         // JRO pMessage = szFeatureNotAvailable;
         pMessage = GET_PMWAPP()->GetResourceStringPointer (uFeatureNotAvailable);
      }

      if (CanInstantUpgrade())
      {
         CNeedToUpgradeDialog Dialog(pMessage, pTitle);
         
         if (Dialog.DoModal() == IDOK)
         {
            return ((CMainFrame*)AfxGetMainWnd())->DoUpgrade();
         }
      }
      else
      {
         CDialog Dialog(IDD_NON_INSTANT_UPGRADE);

         Dialog.DoModal();
      }
   }
   return FALSE;
}

void ValidateHeap(void)
{
#ifndef _WIN32
   switch (_fheapchk())
   {
      case _HEAPBADBEGIN:
      {
         od("Heap has bad beginning.\r\n");
         break;
      }
      case _HEAPBADNODE:
      {
         od("Heap has bad node.\r\n");
         break;
      }
      case _HEAPEMPTY:
      {
         od("Heap is empty!\r\n");
         break;
      }
      case _HEAPOK:
      {
      /* Do nothing. */
         break;
      }
   }
#endif
}

#if 0
int CountFileHandles(void)
{
   int nFiles = 0;
   CPtrArray cpaFiles;

   for (;;)
   {
      CFile* pFile = new CStdioFile;
      CString csName;
      csName.Format("c:\\tmp\\file%04d", nFiles);
      if (!pFile->Open(csName, CFile::modeCreate | CFile::modeReadWrite))
      {
         delete pFile;
         break;
      }
      cpaFiles.Add(pFile);
      nFiles++;
   }

   // Close all the files.
   while (cpaFiles.GetSize() > 0)
   {
      delete (CFile*)cpaFiles.GetAt(0);
      cpaFiles.RemoveAt(0);
   }

   int nWinFiles = 0;

   CWordArray cwaFiles;
   for (;;)
   {
      CString csName;
      csName.Format("c:\\tmp\\file%04d", nWinFiles);
      OFSTRUCT  open_buf;

      HFILE hfile = OpenFile(csName, &open_buf, OF_CREATE | OF_READWRITE | OF_SHARE_DENY_WRITE);
      if (hfile == HFILE_ERROR)
      {
         break;
      }
      cwaFiles.Add((WORD)hfile);
      nWinFiles++;
   }

   // Close all the files.
   while (cwaFiles.GetSize() > 0)
   {
      _lclose(cwaFiles.GetAt(0));
      cwaFiles.RemoveAt(0);
   }

   od(">>> CAN OPEN %d (%d) FILES <<<\r\n", nFiles, nWinFiles);
   return nFiles;
}
#endif

BOOL CPmwApp::InitializeRenaissanceFramework( )
{
	// Initialize some of the TPS libs. From RApplication::RApplication( )
	::tintStartup( );

	// Create the application globals. Its OK to pass a NULL app here, just don't
	// call any functions on the globals that need it.
	// From RStandaloneApplication::RStandaloneApplication( )
	RApplication::m_gsApplicationGlobals = new RApplicationGlobals( NULL );
	RApplication::m_gsApplicationGlobals->CreateColorDialog = IColorDialogImp::CreateInstance;

#ifdef	TPSDEBUG
	// Startup TPS debug library. From RApplication::Initialize( )
	DEBUGPARMS DebugParms;
	DebugParms.hwndApp = m_pMainWnd->GetSafeHwnd( );
	DebugParms.hinstApp = m_hInstance;
	::tdb_StartupDebugLib( &DebugParms, 0 );
#endif	// TPSDEBUG

	// Add the resource.dll instance to the resource manager
//	if( m_hRenaissanceResources != NULL )
//	{
//		::GetResourceManager( ).AddResourceFile( m_hRenaissanceResources );
		::GetResourceManager( ).AddResourceFile( AfxGetResourceHandle() );
//	}

	// Start up the print manager. This will be done in background thread.
	RPrintManager::Startup( );

	// Initialize the bitmap toolbox
//	UseRenaissanceResources();
	InitializeBitmapToolbox( );
//	UseParkRowResources();

	// Load the component servers
	::GetComponentManager( ).LoadComponentServers( FindModulePath( ) + "Components", 0 );

	// Tell the framework about the renaissance definition of the palette dialog.
	ASSERT( RApplication::m_gsApplicationGlobals );
//	RApplication::m_gsApplicationGlobals->CreateColorDialog = IColorDialogImp::CreateInstance;

	return TRUE;
}

void CPmwApp::ShutdownRenaissanceFramework( )
{
	// free bitmap subsystem
	if (RApplication::m_gsApplicationGlobals != NULL)
	{
		if (RApplication::m_gsApplicationGlobals->GetGlobalBitmapPool())
			tbitShutdown();							
		// Remove the Renaissance resources
		RResourceManager	&rm = ::GetResourceManager();
		if (&rm != NULL)
			rm.RemoveResourceFile(AfxGetResourceHandle());
//	::FreeLibrary( m_hRenaissanceResources );
//	::AfxSetResourceHandle( m_hInstance );

		// Unload the component servers
		::GetComponentManager( ).UnloadComponentServers();
		// Shutdown the print manager
		if (RPrintManager::IsPrintManagerInitialized())
			RPrintManager::GetPrintManager().Shutdown();
		// call fixes the infamous 24 and 512 byte memory leaks, fix found by Rich G.
		// JRO WARNING: This assumes that RApplication::m_gsApplicationGlobals == NULL (above)
		// indicates that RStream was never initialized, as when a second instance of the app is run. 
		// Calling SystemShutdown() in this case
		// causes bad TpsAssert things to happen in debug builds
		RStream::SystemShutdown();
	}

#ifdef	TPSDEBUG
	// Shutdown TPS debug library
	::tdb_ShutdownDebugLib( );
#endif	// TPSDEBUG
	// Delete the application globals
	delete RApplication::m_gsApplicationGlobals;
	// Shutdown some of the TPS libs
	::tintShutdown( );
}

/////////
// @mfunc <c CBPubapp> function finds the Help directory.
//
// @rdesc CString containing the path (empty on unrecoverable failure)
//
CString CPmwApp::FindModulePath()
{
    // Get the current application directory
    char szAppPath [_MAX_PATH];
    char szDrive   [_MAX_DRIVE];
    char szDir     [_MAX_DIR];

    ::GetModuleFileName( NULL, szAppPath, _MAX_PATH);
    ::_splitpath( szAppPath, szDrive, szDir, NULL, NULL );

    CString strPath;
    strPath = CString(szDrive);
    strPath += CString(szDir);

    return strPath;
}

// ****************************************************************************
//
//  Function Name:	InitializeBitmapToolbox( )
//
//  Description:	Initializes the bitmap toolbox
//
//  Returns:		Nothing
//
//  Exceptions:		FALSE if the app should terminate
//
// ****************************************************************************
//
void CPmwApp::InitializeBitmapToolbox( )
	{
	HNATIVE	hnColors = NULL;

	//	Initialize the bitmap toolbox - needs palette, so must be after AddResourceFile
	try
	{
			YResourceType		trt;
			SetResourceType(trt, 'P','L','T','T');
			if ( NULL == (hnColors = ::GetResourceManager().GetResource(trt, 10000)) )
				throw kResource;

			if (!tbitStartup (3, 0, hnColors))		//	initialize with 3 ports for any depth
				throw kMemory;

			::GetResourceManager().FreeResource(hnColors);
			hnColors = NULL;								//	in case of exception
	}
	catch( YException /* exception */ )
	{
			if(hnColors != NULL)
				::GetResourceManager().FreeResource(hnColors);

			RAlert( ).AlertUser( STRING_ERROR_GENERAL_STARTUP );

//			throw FALSE;
	}
}

// ****************************************************************************
//
//  Function Name:	GetDateFormattedString( )
//
//  Description:	passed a string and a date format, gets the current date 
//					and converts it to a string using the date format passed
//
//  Returns:		TRUE on success
//
// ****************************************************************************
BOOL CPmwApp::GetDateFormattedString( CString *pString, EPDateFormat eFormat )
{

	char *pstrFormat;

	switch( eFormat )
	{
		case DT_MM_DD_YY:           
			pstrFormat = " MM'/'dd'/'yy ";
			break;
		case DT_DD_MM_YY:
			pstrFormat = " dd'/'MM'/'yy ";
			break;
		case DT_MONTH_DAY_YEAR:
			pstrFormat = " MMMM d',' yyyy ";
			break;
		case DT_DAY_MONTH_YEAR:
			pstrFormat = " d MMMM yyyy ";
			break;
		case DT_YEAR:
			pstrFormat = " yyyy ";
			break;
		case DT_MONTH:
			pstrFormat = " MMMM ";
			break;
		case DT_WEEKDAY:
			pstrFormat = " dddd ";
			break;
		default:
			pstrFormat = NULL;	// use system format
			break;
	}

	TCHAR strDate[128];			// get todays date in the formated by the users preference
	// get the current date in the format desired
	int ret = GetDateFormat(0, 0, NULL, pstrFormat, strDate, 128 );
	
	*pString = strDate;

	return( ret );
}

void CPmwApp::ChangeDisplayMode()
{
	if(m_displayChanger.NeedToChangeMode())
	{
		if(m_displayChanger.ChangeMode())
			m_bChangedDevMode = TRUE;
	}
}

void CPmwApp::RestoreDisplayMode()
{
	if(m_bChangedDevMode)
	{
		m_displayChanger.RestoreMode();
	}
}

CDisplayMode::CDisplayMode():
	bInited(FALSE)
{

}

CDisplayMode::~CDisplayMode()
{

}

BOOL CDisplayMode::NeedToChangeMode()
{
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	int m_nCurXRes = dc.GetDeviceCaps(HORZRES);
	int m_nCurYRes = dc.GetDeviceCaps(VERTRES);
	if(m_nCurXRes < MIN_XRES || m_nCurYRes < MIN_YRES)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CDisplayMode::RestoreMode()
{
	LONG lResult = ChangeDisplaySettings(&m_oldDevMode, 0);
	return (lResult == DISP_CHANGE_SUCCESSFUL);	
}

BOOL CDisplayMode::ChangeMode()
{
	BOOL bRetVal = FALSE;
	if(!bInited)
	{
		m_oldDevMode.dmSize = sizeof(DEVMODE);
		m_oldDevMode.dmDriverExtra = 0;
		BOOL bSuccess = EnumDisplaySettings(NULL, 0, &m_oldDevMode);
		if(bSuccess)
		{
			m_oldDevMode.dmPelsWidth = m_nCurXRes;
			m_oldDevMode.dmPelsHeight = m_nCurYRes;
			m_oldDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
			memcpy(&m_newDevMode, &m_oldDevMode, sizeof(DEVMODE));
			m_newDevMode.dmPelsWidth = MIN_XRES;
			m_newDevMode.dmPelsHeight = MIN_YRES;
			LONG lResult = ChangeDisplaySettings(&m_newDevMode, CDS_TEST);
			if(lResult == DISP_CHANGE_SUCCESSFUL)
				bInited = TRUE;
			else
				return bRetVal;
		}
		else
			return bRetVal;
	
	}
	LONG lResult = ChangeDisplaySettings(&m_newDevMode, 0);
	bRetVal = (lResult == DISP_CHANGE_SUCCESSFUL);	

	return bRetVal;
}

//////////
// Support for Print Shop's Path To Project wizard (PSLaunchDLL.dll)
HINSTANCE CPmwApp::m_hPSLaunchDLL = NULL;

// This function launches Print Shop's Path to Project wizard
int CPmwApp::RunPrintShopWizard()
{
	// function pointer type for return of GetProcAddress from PSLaunchDLL
	typedef HRESULT ( *pfnDllGetClassObject )( REFCLSID rclsid, REFIID riid, LPVOID* ppv );

	// assume Cancel until told otherwise
	EResult result = kResultCancel;

	if (m_hPSLaunchDLL == NULL)	// DLL not loaded yet?
	{
		CString strDllPath = GetGlobalPathManager()->ExpandPath("PSLaunchDLL.dll");
		m_hPSLaunchDLL = ::LoadLibrary(strDllPath);
	}

	if (m_hPSLaunchDLL)
	{
		IPSLaunchCtrl* pInterface;
		pfnDllGetClassObject CreateInstance = (pfnDllGetClassObject) ::GetProcAddress( m_hPSLaunchDLL, "DllCreateInstance" );

		if (CreateInstance && S_OK == CreateInstance( CLSID_PSLaunchCtrl, IID_IPSLaunchCtrl, (void **) &pInterface ))
		{
			pInterface->DoFileNew( NULL, kAppLauncher, &result );
			pInterface->Release();
		}
	}

	return int(result);
}
