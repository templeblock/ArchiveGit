/*
// $Workfile: BROWSER.CPP $
// $Revision: 3 $
// $Date: 3/08/99 3:55p $
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
// $Log: /PM8/App/BROWSER.CPP $
// 
// 3     3/08/99 3:55p Rgrenfel
// Fixed its OnDestroy method to call the base class method.  Fixes leak
// with base CHybridBaseDlg.
// 
// 2     3/04/99 4:48p Gbeddow
// in CArtBrowserDialog::SaveCategoryTreeState(), add sanity check on
// m_CategoryTreeCtrl  in case we got there from shutting down the dialog
// before it was fully initialized
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 161   3/01/99 12:04p Mwilson
// Added AgProjectBrowser
// 
// 160   2/26/99 8:44p Rgrenfel
// When calling ChooseExternalPicture cast the object to a RectPageObject
// pointer.
// 
// 159   2/26/99 1:10p Gbeddow
// fixed a bug in the Project Gallery that tended to display duplicate
// thumbnails when the dialog first comes up - this change may need to be
// revisited since it sometimes causes the search to execute more than
// once when the dialog comes up
// 
// 158   2/25/99 6:32p Gbeddow
// New Online Art button artwork, and new "Master Art" button and
// functionality in the Art Gallery dialog
// 
// 157   2/23/99 4:27p Dennis
// Replaced sentiment drawing with RTF control for single line sentiment
// and scrolling support
// 
// 156   2/23/99 1:29p Gbeddow
// fix bug 181 - art gallery search results weren't being properly
// restored when the saved state of the category tree control on
// re-entering the dialog was something other than "All"
// 
// 155   2/15/99 3:18p Gbeddow
// fix bug that caused crash in release version bringing up the Art
// Gallery dialog; performance improvements related to inserting
// categories in the Art Gallery tree control
// 
// 154   2/12/99 5:42p Gbeddow
// replace category drop-down control with tree control in the art gallery
// and web art gallery dialogs
// 
// 153   2/01/99 11:39a Rgrenfel
// Added GIF animation preview to the Web Art Gallery
// 
// 152   1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 151   1/25/99 1:20p Gbeddow
// support for the latest spec'd list of graphic types and initial support
// for a new web art browser dialog
// 
// 150   1/05/99 5:31p Gbeddow
// art gallery (graphic browser) support for Print Shop-style graphic type
// filtering
// 
// 149   12/01/98 9:25a Dennis
// Fixed Manual Print for multiple collections
// 
// 148   11/25/98 5:58p Psasse
// Close browser if online account is successfully created or changed
// 
// 147   11/24/98 1:48p Dennis
// Fixed problem in sentiment gallery when switching categories would
// cause no sentiments to be displayed.  Problem new to PM release 7.01
// 
// 146   11/23/98 8:27p Psasse
// EndDialog() when creating or changing online accounts
// 
// 145   11/23/98 1:27p Mwilson
// Added creataparty configuration
// 
// 144   11/21/98 10:45p Psasse
// 
// 143   11/19/98 2:54p Mwilson
// changed error string
// 
// 142   11/18/98 11:03p Psasse
// 
// 141   11/18/98 10:32p Psasse
// New IsItemOKToView function (concerning licensing)
// 
// 140   11/18/98 10:12a Johno
// Dynamic art store hub button
// 
// 139   11/17/98 6:06p Psasse
// Fixed viewing in large mode when not licensed
// 
// 138   11/17/98 5:21p Mwilson
// removed online art gallery button and menu for crafts deluxe
// 
// 137   11/16/98 1:16p Johno
// Crash no More
// (m_pItemPreviews  = NULL)
// 
// 136   11/15/98 6:49p Psasse
// Disallow large thumbnail view when collection is not licensed
// 
// 135   11/10/98 2:07p Gbeddow
// oleimpl.h does not exist in VC6
// 
// 134   11/06/98 12:20p Mwilson
// fixed display problems with large fonts
// 
// 133   10/28/98 11:14a Mwilson
// fixed paste bug
// 
// 132   10/27/98 7:08p Jayn
// 
// 131   10/23/98 5:41p Johno
// Changes for new online art button
// 
// 130   10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 129   10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 128   10/12/98 1:28p Mwilson
// added craft deluxe configuration
// 
// 127   10/08/98 4:51p Psasse
// added online menu options to the project browser
// 
// 126   10/07/98 5:43p Jayn
// Changes to the content manager.
// 
// 125   10/02/98 9:07p Psasse
// 
// 124   10/02/98 7:10p Jayn
// More content manager changes
// 
// 123   10/02/98 2:37p Hforman
// Fixed problems with arrow keys in large mode.
// 
// 122   9/28/98 10:54a Johno
// Use pmwinet.h in place of afxinet.h
// 
// 121   9/26/98 8:03p Psasse
// Changed behavior for checking for locally cached (internet) files
// 
// 120   9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 119   9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 118   9/14/98 12:05p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 117   9/03/98 5:27p Mwilson
// Changed GetSentiment to return a bool
// 
// 116   9/01/98 6:24p Dennis
// Remove Online menu for American Greetings Art Gallery
// 
// 115   8/31/98 2:02p Hforman
// handle changing Craft pages in Large View
// 
// 114   8/29/98 8:21a Dennis
// License Update Fix
// 
// 113   8/28/98 3:04p Hforman
// - fixed problem in MakeThumbnail() with transparent images
// - fixed down arrow key in large mode
// 
// 112   8/27/98 4:12p Hforman
// added code to MakeThumbnail() so 8-bit thumbnails are not dithered into
// PrintMaster palette if user is running in truecolor mode
// 
// 111   8/25/98 1:04p Hforman
// add a check in ProjectTypeChanged() to avoid crash
// 
// 110   8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 109   8/19/98 6:33p Johno
// Fix in DoPrint(void) for manual printing, all on page
// 
// 108   8/19/98 12:13p Johno
// Changes to CProjectBrowserDialog::DoPrint() for manual printing
// 
// 107   8/18/98 2:32p Johno
// Changes to CProjectBrowserDialog::DoPrint() for manual printing
// 
// 106   8/12/98 2:02p Mwilson
// added specific helpful hint for AG craft popups cutouts and big cards
// 
// 105   8/11/98 1:49p Dennis
// Added AG Specific browser tile
// 
// 104   8/11/98 9:34a Johno
// Compile update
// 
// 103   8/10/98 6:44p Hforman
// fix setting of CropCopyright boolean in GetItemInformation()
// 
// 102   8/10/98 5:33p Johno
// Merge?
// 
// 101   8/10/98 4:45p Hforman
// changes for cropping copyright off AG images
// 
// 100   8/07/98 5:10p Mwilson
// added call to show helpful hint for cutout cards
// 
// 99    7/30/98 2:18p Jayn
// 
// 98    7/24/98 10:23p Hforman
// 1) fixed crash bugs w/no preview items
// 2) fewer cancels required for "Insert CD..."
// 3) show checks for small/medium/large on menus
// 4) change where statusbar popups come up
// 
// 97    7/23/98 6:28p Psasse
// Refresh of large thumbnails when properties dialog moved over them is
// fixed
// 
// 96    7/23/98 5:19p Psasse
// delete Copy from the Edit menu of Project Gallery
// 
// 95    7/23/98 10:43a Hforman
// 1) modified "sample" item display
// 2) fixed categories for supercollections & identical categories in
// multiple collections
// 
// 94    7/21/98 8:09p Hforman
// fixed possible crash bug in OnVScroll()
// 
// 93    7/21/98 3:24p Dennis
// Use AG Specific Sentiment Browser Tiles
// 
// 92    7/20/98 4:40p Psasse
// Disable Copy for the Project browser
// 
// 91    7/17/98 8:49p Hforman
// fixed "view set" in OnClickedUnlicensed()
// 
// 90    7/16/98 4:45p Johno
// Added view paging for calandars (like newsletters)
// 
// 89    7/16/98 3:17p Johno
// Check if on last page in OnViewNewsLetterNext() to prevent the AddPage
// dialog 
// 
// 88    7/16/98 12:26p Johno
// Added support for viewing both postcard sides in large (OnUpdateView(),
// OnChangeView())
// 
// 87    7/15/98 2:47p Hforman
// fix bug in CalcNumGroupItems() when only 1 group
// 
// 86    7/15/98 11:28a Hforman
// - fixed crash bug in OnVScroll()
// - Only invalidating item on DoDragDrop() in OnLButtonDown()
// 
// 85    7/14/98 6:34p Jayn
// 
// 84    7/14/98 11:25a Cboggio
// Changed MakeUpper to CharUpper in DisplayGroupHeading, for localization
// 
// 83    7/13/98 6:28p Psasse
// Proper ownership and early cancellation of online progress bar
// 
// 82    7/13/98 5:52p Hforman
// fixed problems with scrolling and selection
// 
// 81    7/13/98 5:00p Mwilson
// changed to default size as a percentage of the screen
// 
// 80    7/10/98 8:54p Hforman
// - fix problem with "All" not at top of category list in Art Gallery
// - finished arrow/page functionality in Project Gallery
// 
// 79    7/08/98 8:21p Hforman
// showing "Copyright Title" on thumbnail titles
// 
// 78    6/30/98 6:06p Hforman
// 1) fix crashes/weirdness on resize of project gallery
// 2) fix problems with initial size/resize
// 3) add vertical line on right side of preview area
// 
// 77    6/29/98 11:35a Hforman
// fix crash bug in browser -- going past end w/projects
// 
// 76    6/24/98 7:18p Psasse
// Fix crash bug when copy/paste from a closed browser
// 
// 75    6/17/98 10:42a Hforman
// put in Any/All functionality for Find
// 
// 74    6/15/98 6:57p Hforman
// add "Match All" for keyword search
// 
// 73    6/15/98 2:53p Jayn
// Fixed some resource leaks and other questionable code.
// 
// 72    6/12/98 11:41a Cboggio
// added option to remove friendly names from browser for locaization
// 
// 71    6/11/98 9:41a Dennis
// AG Specific Gallery Resource loading
// 
// 70    6/05/98 4:49p Hforman
// show Special Collection name under each item in collection
// 
// 69    6/04/98 5:34p Rlovejoy
// Added some more helpful hints.
// 
// 68    6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 67    5/27/98 7:02p Hforman
// remove Welcome stuff, unused colors, etc.
// 
// 66    5/22/98 5:38p Hforman
// passing array of collections to Search() if super collection selected
// 
// 65    5/22/98 12:16p Fredf
// 
// 64    5/22/98 11:38a Hforman
// more work on Sentiment Gallery
// 
// 63    5/22/98 11:18a Fredf
// Ported 4.0 code over to print image index.
// 
// 62    5/21/98 6:56p Hforman
// 
// 61    5/21/98 3:26p Hforman
// 
// 60    5/20/98 6:51p Hforman
// Added super collection mechanism
// 
// 59    5/19/98 3:03p Hforman
// fixed some user category problems
// 
// 58    5/12/98 4:08p Hforman
// remove ProductCode from GetItemInformation(); tweaked "Online" text
// 
// 57    5/11/98 9:07p Psasse
// content manager becomes a member of the app
// 
// 56    5/11/98 8:48a Dennis
// Added SetStartupCollection
// 
// 55    5/09/98 9:08p Psasse
// 
// 54    5/06/98 6:05p Hforman
// removed NewConfirmDeleteDialog() functions; rewrote OnDelete()
// 
// 53    5/04/98 10:56a Hforman
// fixed dumpcat problem
// 
// 52    5/01/98 6:24p Psasse
// Fixed local ContentMgr problem by changing to the App's ContentMgr
// 
// 51    4/30/98 10:23p Fredf
// Support for "Special Collection" button used for American Greetings.
// 
// 50    4/30/98 9:21p Jayn
// Correct usage of m_pDoc.
// 
// 49    4/30/98 7:11p Hforman
// added call to show Category Organizer dialog; fixed Online text
// displaying
// 
// 48    4/29/98 7:25p Jayn
// Fixed transparency problems.
// 
// 47    4/28/98 10:34a Dennis
// Order form filename now comes from collection.
// 
// 46    4/27/98 5:29p Fredf
// Improved bar positioning.
// 
// 45    4/25/98 7:55p Psasse
// 
// 44    4/23/98 9:11p Psasse
// 
// 43    4/23/98 4:13p Psasse
// 
// 42    4/22/98 8:59p Psasse
// 
// 41    4/22/98 6:27p Jayn
// Frees memory if hDC == NULL.
// 
// 40    4/22/98 8:26a Dennis
// Fixed integration bug with 24-bit thumbnail drawing in 8-bit video
// mode.
// 
// 39    4/21/98 2:44p Jayn
// Removed errant line of code.
// 
// 38    4/20/98 2:18p Fredf
// Moved string to resource file.
// 
// 37    4/20/98 1:00p Fredf
// No Matching Items.   Remember to move string to resource file!
// 
// 36    4/20/98 8:43a Dennis
// Added 24-bit thumbnail drawing support
// 
// 35    4/17/98 6:11p Hforman
// displaying "Available Online" string below thumbnail caption; changed
// font size for caption to 14pt
// 
// 34    4/17/98 2:30p Psasse
// ADDED ON-LINE FUNCTIONALITY
// 
// 33    4/16/98 7:14p Hforman
// new dialogs, categories, etc.
// 
// 32    4/14/98 10:01a Dennis
// Removed Media and Style. Added Free Sample logic to
// GetUnlicensedCaption().
// 
// 31    4/13/98 12:02p Hforman
// modified params to ImportItem()
// 
// 30    4/13/98 10:32a Jayn
// New drag/drop stuff.
// 
// 29    4/08/98 6:44p Hforman
// interim checkin - more later
// 
// 28    4/08/98 4:30p Hforman
// interim checkin -- work-in-progress
// 
// 27    4/06/98 11:18a Hforman
// 
// 26    3/31/98 4:45p Hforman
// more work on project groups
// 
// 25    3/27/98 2:14p Hforman
// work-in-progress -- project types shown in preview area, etc.
// 
// 24    3/26/98 10:09a Jayn
// Support for DPP (Serif) importing, previewing, etc.
// 
// 23    3/24/98 1:23p Fredf
// Uses bold font for sidebar control labels.
// 
// 22    3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
// 21    3/19/98 5:51p Hforman
// Oops, fixed a couple of problems with transparency in 256-color mode
// 
// 20    3/19/98 1:08p Hforman
// Transparent thumbnail drawing, etc.
// 
// 19    3/17/98 4:41p Fredf
// New gallery bitmaps.
// 
// 18    3/17/98 3:37p Fredf
// 
// 17    3/12/98 4:39p Hforman
// add "Project Type" combo to project gallery; add in-place editing of
// user friendly names
// 
// 16    3/10/98 5:23p Jayn
// Master page changes
// 
// 15    3/06/98 7:10p Hforman
// interim checkin -- hope I didn't break things too badly :-)
// 
// 14    3/06/98 1:18p Hforman
// 
// 13    3/03/98 9:37p Hforman
// 
// 12    2/27/98 7:08p Hforman
// interim checkin - work in progress
// 
// 11    2/27/98 11:16a Hforman
// initial changes for new design
// 
// 10    2/06/98 11:34a Jayn
// More complete automation support.
// 
// 9     1/22/98 7:18p Hforman
// add Paste functionality; add "Copy Local" option on Import Art
// 
// 8     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 7     1/15/98 3:59p Hforman
// changed for DragAndDrop; remove old WIN16 code
// 
// 6     12/23/97 10:27a Jayn
// 
// 5     12/19/97 5:35p Hforman
// add "Matching Set" Helpful Hint
// 
// 4     12/16/97 2:18p Jayn
// Rearranged meaning of m_pDoc and m_nType.
// 
// 3     12/12/97 5:20p Hforman
// add helpful hints
// 
// 2     12/02/97 10:20a Dennis
// Fixed Project Delete when delete item is checked.
// 
//    Rev 1.4   21 Oct 1997 15:44:56   hal
// 
// modified to allow drawing of large thumbnails to be interrupted
// 
//    Rev 1.3   18 Sep 1997 15:45:52   johno
// Browser reset button
// 
// 
//    Rev 1.2   25 Aug 1997 10:28:46   johno
// "Path of recreation" stuff now called from PmwDoc
// 
//    Rev 1.1   22 Aug 1997 12:17:08   johno
// Changes for new 5.0 hub buttons
// 
// 
//    Rev 1.0   14 Aug 1997 15:18:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:16   Fred
// Initial revision.
// 
//    Rev 1.113   05 Aug 1997 13:06:10   Jay
// Added check for negative index
// 
//    Rev 1.112   05 Aug 1997 11:40:50   Jay
// Fixed re-entrancy problem with OnEraseBckgnd()
// 
//    Rev 1.111   04 Aug 1997 16:01:48   Fred
// Safety checks for no thumbnails conditions
// 
//    Rev 1.110   01 Aug 1997 13:59:14   Fred
// Commented out message box which can come up if CD change is canceled
// 
//    Rev 1.109   31 Jul 1997 15:05:22   johno
// Drag 'n drop is disabled for add ons.
// It appeard to be working, but nothing would happen.
// 
//    Rev 1.108   31 Jul 1997 12:02:54   johno
// Substitute new menu if art browser is an addon
// 
//    Rev 1.107   24 Jul 1997 14:37:38   Jay
// Print project and art improvements (fixes)
// 
//    Rev 1.106   21 Jul 1997 14:48:16   dennisp
// Changed SaveState() for keywords so that if selection is 0, save -1.
// This fixes <None> getting saved under 16-bit windows.
// 
//    Rev 1.105   21 Jul 1997 14:18:46   dennisp
// Added call to KeywordChanged() on OnFind() so keywords get saved when you
// press the Find button after typing a new keyword.
// 
//    Rev 1.104   21 Jul 1997 11:21:52   dennisp
// Keyword string now gets trimmed
// 
//    Rev 1.103   18 Jul 1997 15:33:16   dennisp
// Home now works when only 1 page of items, Improved item row logic for
// GotoItem(), When window is sized, preview maintains current item.
// 
//    Rev 1.102   18 Jul 1997 12:59:10   dennisp
// Named "Your Own" category to fix delete problem
// 
//    Rev 1.101   18 Jul 1997 11:10:24   Jay
// Got rid of an assertion case when Select is pressed with no items.
// 
//    Rev 1.100   16 Jul 1997 15:09:06   dennisp
// Clicking on empty view panel for toolbar no longer brings up menu for large
// project options (Front/Inside/Back,  ect.)
// 
//    Rev 1.99   16 Jul 1997 11:57:30   dennisp
// Fixed keyword state getting restored from YourOwn
// 
//    Rev 1.98   15 Jul 1997 13:46:58   dennisp
// Added fix so user can choose Panel for NoteCards.
// 
//    Rev 1.97   15 Jul 1997 10:37:56   dennisp
// Preview drawing for large thumbnails now clips to page size of document.
// 
//    Rev 1.96   11 Jul 1997 09:38:04   Jay
// Free collection stuff
// 
//    Rev 1.95   11 Jul 1997 09:22:44   dennisp
// Better method for storing collection ID, Edit/Copy uses white for fill color
// and uses screen resolution for bitmap size.
// 
//    Rev 1.94   09 Jul 1997 09:23:20   Jay
// Now deletes files used for drag/drop
// 
//    Rev 1.93   09 Jul 1997 09:00:16   dennisp
// Added support for LONG number of results. Added inproved handling when CD 
// is requested but user hits cancel.  Cleared text controls for empty result
// set in sentiment browser.
// 
//    Rev 1.92   02 Jul 1997 10:03:16   Jay
// Passes graphic parameters through to Open From Disk
// 
//    Rev 1.91   02 Jul 1997 09:30:54   Jay
// Address book choose picture now works
// 
//    Rev 1.90   02 Jul 1997 09:03:26   Jay
// Modal picture selection
// 
//    Rev 1.89   01 Jul 1997 16:56:12   Jay
// Added some missing OnCloseDocument()'s (fixes end-of-program hang).
// 
//    Rev 1.88   01 Jul 1997 10:52:30   Jay
// New MRU stuff
// 
//    Rev 1.87   27 Jun 1997 17:11:02   dennisp
// Added Edit/Copyu support
// 
//    Rev 1.86   27 Jun 1997 16:04:52   Jay
// Photo CD
// 
//    Rev 1.85   26 Jun 1997 17:10:54   Jay
// Changes to Open From Disk to bring it in line. Support for embed_source.
// 
//    Rev 1.84   26 Jun 1997 13:27:36   johno
// "Path of inspiration" open from disk
// 
//    Rev 1.82   25 Jun 1997 15:17:20   Jay
// New 8-bit compression stuff
// 
//    Rev 1.81   25 Jun 1997 14:50:48   dennisp
// Added support to filter out empty categories based on product flags
// 
//    Rev 1.80   20 Jun 1997 14:06:52   dennisp
// Added state logic for Last Item and keyword, removed empty user category from 
// category list, added all support to Sentiments, misc fixes.
// 
//    Rev 1.79   17 Jun 1997 12:42:20   Jay
// Import; OpenStreams()
// 
//    Rev 1.78   16 Jun 1997 13:51:38   johno
// Added SetAddonColors
// 
//    Rev 1.77   13 Jun 1997 15:11:20   Jay
// New licensing return codes
// 
//    Rev 1.76   13 Jun 1997 13:48:24   dennisp
// Added logic to fill in Category combo when "All" is selected for a collection.
// Added state logic for keywords.
// 
//    Rev 1.75   11 Jun 1997 15:17:14   Jay
// Minor licensing fixes.
// 
//    Rev 1.74   11 Jun 1997 12:37:36   Fred
// Changes for sorting
// 
//    Rev 1.73   11 Jun 1997 12:06:44   Jay
// "View Set" now sets collection as well as category (for art).
// 
//    Rev 1.72   11 Jun 1997 11:42:50   Jay
// Licensing, etc.
// 
//    Rev 1.71   09 Jun 1997 18:14:50   doberthur
// Change CProjectBrowserDialog to support Your Project help dialog.
// Changed  CProjectBrowserDialog constructor to default to Ready Made
// help dialog.
// 
//    Rev 1.70   09 Jun 1997 14:31:58   dennisp
// Added capability to goto a category at startup for "Your Own"
// 
//    Rev 1.69   09 Jun 1997 10:43:38   johno
// No change.
// 
//    Rev 1.68   06 Jun 1997 15:34:00   doberthur
// Activate sentiment help dialog.
// 
//    Rev 1.67   06 Jun 1997 14:41:24   Jay
// HasOrderForm
// 
//    Rev 1.66   06 Jun 1997 13:57:22   dennisp
// Added support for going to categories from the Catalog Purchase Dialog
// 
//    Rev 1.65   06 Jun 1997 11:51:12   Jay
// Licensing changes
// 
//    Rev 1.64   06 Jun 1997 09:13:28   dennisp
// Added "All" to project types
// 
//    Rev 1.63   05 Jun 1997 16:20:46   doberthur
// FIx bugs in print project and print art.
// 
//    Rev 1.62   05 Jun 1997 15:36:50   dennisp
// Better licensing support, removed obsolete code
// 
//    Rev 1.61   04 Jun 1997 15:52:12   dennisp
// Added state logic for combos and window pos, Enhanced GetSentiment, generalized GetSelections
// 
//    Rev 1.60   03 Jun 1997 11:18:22   dennisp
// Added GotoItem() method and now use it when switching views
// 
//    Rev 1.59   02 Jun 1997 15:21:08   dennisp
// Added check for content existance before allowing dialog creation to continue
// 
//    Rev 1.58   02 Jun 1997 13:13:14   Jay
// Now supports 8-bit thumbnail.
// 
//    Rev 1.57   01 Jun 1997 16:33:44   dennisp
// Better sizing logic for 1 ups and fixed alt+s accelerator conflict
// 
//    Rev 1.56   01 Jun 1997 13:50:40   Jay
// Fixed a crash in OnDown.
// 
//    Rev 1.55   01 Jun 1997 13:49:06   dennisp
// Improved item selection
// 
//    Rev 1.54   01 Jun 1997 12:41:12   doberthur
// Add code for browser print project.
// 
//    Rev 1.53   30 May 1997 21:20:22   dennisp
// Sentiment browser sizing support, better focus rect
// 
//    Rev 1.52   30 May 1997 19:16:22   Jay
// Properties, etc.
// 
//    Rev 1.51   30 May 1997 18:56:46   dennisp
// Added Item description in large views
// 
//    Rev 1.50   30 May 1997 10:18:06   Jay
// Browser import/export
// 
//    Rev 1.49   30 May 1997 10:08:14   dennisp
// Added line separator to status bar for 32 bit
// 
//    Rev 1.48   29 May 1997 18:18:26   dennisp
// Added "All" to sub-category lists
// 
//    Rev 1.47   29 May 1997 14:40:22   Jay
// Delete
// 
//    Rev 1.46   29 May 1997 14:23:34   dennisp
// Added sentiment selection
// 
//    Rev 1.45   29 May 1997 09:30:18   Jay
// Export, etc.
// 
//    Rev 1.44   28 May 1997 19:39:04   dennisp
// Added sentiment browser
// 
//    Rev 1.43   28 May 1997 09:17:12   Jay
// Catalog support. Better transparent previews.
// 
//    Rev 1.42   27 May 1997 19:24:48   dennisp
// Added support for viewing pages/panels in 1 ups for browser
// 
//    Rev 1.41   23 May 1997 17:42:02   doberthur
// Fix assert in art print.
// 
//    Rev 1.40   23 May 1997 15:42:04   doberthur
// Add print art function in art browser.
// 
// 
//    Rev 1.39   23 May 1997 14:57:30   dennisp
// Status bar and drawing enhancements
// 
//    Rev 1.38   20 May 1997 12:30:38   Jay
// Removed a weirdness.
// 
//    Rev 1.37   20 May 1997 11:06:16   johno
// 
//    Rev 1.36   19 May 1997 20:33:16   johno
// Content licensing added to project
// 
// 
//    Rev 1.35   19 May 1997 12:47:24   dennisp
// Added StatusBar
// 
//    Rev 1.34   16 May 1997 11:42:16   dennisp
// 16 bit fixes for dialog message processing
// 
//    Rev 1.33   15 May 1997 14:58:28   dennisp
// Added 16 bit accelerator support
// 
//    Rev 1.32   15 May 1997 12:12:44   dennisp
// 
//    Rev 1.31   15 May 1997 10:56:52   dennisp
// 
//    Rev 1.30   15 May 1997 10:22:28   dennisp
// Added keyboard support to browser
// 
//    Rev 1.29   13 May 1997 11:46:54   johno
// 
//    Rev 1.28   12 May 1997 18:08:26   Jay
// Browser drag & drop
// 
//    Rev 1.27   09 May 1997 11:32:50   johno
// Welcome color set in Art Gallery ctor
// 
//    Rev 1.26   09 May 1997 10:16:52   Jay
// Project Save
// 
//    Rev 1.25   08 May 1997 09:56:14   johno
// Added code to CArtBrowserDialog::DoSelect
// to support the path of enlightenment,
// Grasshopper
// 
//    Rev 1.24   07 May 1997 17:17:50   dennisp
// Added capability to change colors in browsers from .INI file.
// 
//    Rev 1.23   07 May 1997 12:10:46   Jay
// User directory and collections
// 
//    Rev 1.22   06 May 1997 17:21:58   Jay
// Multi-collection stream management
// 
//    Rev 1.21   06 May 1997 15:32:00   Jay
// Initial multi-collection searches
// 
//    Rev 1.20   06 May 1997 09:18:20   Jay
// Removed extra argument to choose_a_picture.
// 
//    Rev 1.19   01 May 1997 16:00:04   Fred
// Does not allow CTL3D to subclass browser dialog.
// 
//    Rev 1.18   30 Apr 1997 16:44:44   dennisp
// Use GetItemSize() instead of GetDataSize()
// 
//    Rev 1.17   25 Apr 1997 13:02:26   Fred
// Wait cursor is turned on while building thumbnails
// 
//    Rev 1.16   25 Apr 1997 11:50:58   Jay
//  
// 
//    Rev 1.15   25 Apr 1997 11:06:42   Fred
// Alpha Version
// 
//    Rev 1.14   24 Apr 1997 17:34:50   Fred
//  
// 
//    Rev 1.13   24 Apr 1997 15:12:14   Fred
//  
// 
//    Rev 1.12   24 Apr 1997 15:11:00   Fred
//  
// 
//    Rev 1.11   24 Apr 1997 11:04:26   Fred
//  
// 
//    Rev 1.10   23 Apr 1997 18:33:52   Fred
//  
// 
//    Rev 1.9   23 Apr 1997 17:15:34   Jay
// DoSelect for project and picture browsers.
// 
//    Rev 1.8   23 Apr 1997 16:28:34   Jay
// Compressed thumbnails.
// 
//    Rev 1.7   23 Apr 1997 16:16:16   Fred
//  
// 
//    Rev 1.6   23 Apr 1997 14:20:28   Fred
//  
// 
//    Rev 1.5   23 Apr 1997 12:45:46   Jay
// Thumbnail drawing code.
// 
//    Rev 1.4   23 Apr 1997 12:06:54   Fred
//  
// 
//    Rev 1.3   23 Apr 1997 11:38:18   Fred
//  
// 
//    Rev 1.2   23 Apr 1997 10:38:30   Fred
//  
// 
//    Rev 1.1   23 Apr 1997 10:20:32   Fred
// More browser stuff
// 
//    Rev 1.0   22 Apr 1997 12:19:30   Fred
// Initial revision.
*/

#include "stdafx.h"

#include <afxpriv.h>				// For access to AFX_SIZEPARENTPARAMS 
#include <..\src\afximpl.h>	// for AfxCancelModes
#if _MSC_VER < 1200 // GCB 11/10/98 - oleimpl.h does not exist in VC6
	#include <..\src\oleimpl.h>
#endif
#include <..\src\oleimpl2.h>

#include "pmw.h"
#include "pmwdoc.h"
#include "pmwcfg.h"
#include "pmwview.h"
#include "util.h"
#include "utils.h"
#include "sortarry.h"
#include "statice.h"
#include "bitbtn.h"
#include "colordef.h"
#include "pdlgclr.h"
#include "mainfrm.h"
#include "browser.h"
#include "browsupp.h"
#include "browDrop.h"
#include "dither.h"
#include "jpegimpl.h"
#include "gif.h"
#include "memdev.h"
#include "file.h"
#include "clip.h"
#include "printart.h"
#include "carddoc.h"
#include "brodoc.h"
#include "pagedlgs.h"
#include "CnfrmRm.h"
#include "OpenDlgs.h"
#include "compfn.h"
#include "printart.h"
#include "printprj.h"
#include "inifile.h"
#include "glicense.h"
#include "licdll.h"
#include "cdeflate.h"
#include "hint.h"
#include "bmp.h"        // for BitmapToDIB()
#include "BmpTiler.h"
#include "CatOrgnz.h"
#include "pmwini.h"
#include "rmagcom.h"    // For CResourceManager
#include "pmwcoll.h"
#include "strmcach.h"
#include "GifAnimator.h"
#include "AnimationPreviewDlg.h"
#include "ftbld\ftbld.h"			// For RTF formatting

#ifdef LOCALIZE
#include "clocale.h"
#endif

#define MAX_KEYWORD_COMBO_ITEMS 8
const double dPercentView = .9;
static const char BASED_CODE ENTRY_Valid[] = "Valid";

extern CPalette* pOurPal;
extern const RGBQUAD* GetIdentityPalette(void);
extern CPalette *CreateIdentityPalette(void);
extern LPCSTR BrowserAllString(void);	// Actually, it is in here!

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef _SHLOBJ_H_
//
// format of CF_HDROP and CF_PRINTERS, in the HDROP case the data that follows
// is a double null terinated list of file names, for printers they are printer
// friendly names
//
typedef struct _DROPFILES {
   DWORD pFiles;                       // offset of file list
   POINT pt;                           // drop point (client coords)
   BOOL fNC;                           // is it on NonClient area
                                       // and pt is in screen coords
   BOOL fWide;                         // WIDE character switch
} DROPFILES, FAR * LPDROPFILES;
#endif

// Table used to fill in the Graphic Type list box.
ToneStyleMediaTableEntry GraphicTypeTable[] =
{
   { IDS_ALL,									CCategoryDataItem::cfNone				},
   { IDS_GRAPHIC_TYPE_COLOR_GRAPHIC,	CCategoryDataItem::cfColorGraphic	},
   { IDS_GRAPHIC_TYPE_BW_GRAPHIC,		CCategoryDataItem::cfBWGraphic		},
   { IDS_GRAPHIC_TYPE_FINE_ART,			CCategoryDataItem::cfFineArt			},
   { IDS_GRAPHIC_TYPE_PHOTO,				CCategoryDataItem::cfPhoto				},
   { IDS_GRAPHIC_TYPE_CLIP_PHOTO,		CCategoryDataItem::cfClipPhoto		},
   { 0,											0												}
};

// Table used to fill in the tone list box.
ToneStyleMediaTableEntry ToneTable[] =
{
   { IDS_ALL,                    CCategoryDataItem::toneNone            },
   { IDS_TONE_HUMOROUS,          CCategoryDataItem::toneHumorous        },
   { IDS_TONE_SENTIMENTAL,       CCategoryDataItem::toneSentimental     },
   { IDS_TONE_TRADITIONAL,       CCategoryDataItem::toneTraditional     },
   { IDS_TONE_CONTEMPORARY,      CCategoryDataItem::toneContemporary    },
   { IDS_TONE_SPIRITUAL,         CCategoryDataItem::toneSpiritual       },
   { 0,                          0                                      }
};

// Build a string array containing the items selected in a
// tree combo box. If there are no selections, a single
// string of "All" is returned.
void GetSelections(CMlsStringArray& Selections, CTreeComboBox* pComboBox)
{
	int      nSelection = pComboBox->GetCurSel(NULL);
	CString  csCurItemText;

	// Get rid of any current selections.
	Selections.RemoveAll();
	if(nSelection >= 0)
	{
		pComboBox->GetText(nSelection, csCurItemText);
		Selections.SetNames(csCurItemText, pComboBox->GetDelimiter());
	}
	// If there are no selections, return a single string: "All".
	if (Selections.GetSize() == 0)
	{
		Selections.Add(BrowserAllString());
	}
}


/////////////////////////////////////////////////////////
// CPreviewArray
//
// This class is used to hold an array of objects that
// describe the current preview thumbnails.

// Constructor.
CPreviewArray::CPreviewArray()
{
}

// Destructor.
CPreviewArray::~CPreviewArray()
{
   DeleteAll();
}

// Delete all the preview objects.  
void CPreviewArray::DeleteAll(void)
{
   DeleteAt(0, GetSize());
}

// Delete a particular preview object.
void CPreviewArray::DeleteAt(int nIndex, int nCount /*=1*/)
{
   for (int i = 0; i < nCount; i++)
   {
      if (((nIndex+i) >= 0) && ((nIndex+i) < GetSize()))
      {
         delete GetAt(nIndex+i);
         SetAt(nIndex+i, NULL);
      }
   }

   INHERITED::RemoveAt(nIndex, nCount);
}

// Add a new preview given its attributes.
int CPreviewArray::Add(USHORT uCollectionID, DWORD dwItemNumber, int nResultIndex,
							  void* pThumbnailData, const CString& csCaption, const CString& csFile,
							  const CRect& crThumbnail, const CRect& crFocusArea,
							  BOOL bIsLicensed, BOOL bTransparent)
{
	int nIndex = -1;
	Item* pItem = NULL;

	TRY
	{
		pItem = new Item(uCollectionID, dwItemNumber, nResultIndex,
							  pThumbnailData, csCaption, csFile,
							  crThumbnail, crFocusArea, bIsLicensed, bTransparent);
		nIndex = INHERITED::Add(pItem);
	}
	END_TRY

	if (nIndex == -1)
		delete pItem;

	return nIndex;
}

// Find a preview item by its collection/item number.
int CPreviewArray::Find(USHORT uCollectionID, DWORD dwItemNumber)
{
   int nItem = -1;

   for (int nIndex = 0; nIndex < GetSize(); nIndex++)
   {
      Item* pItem = GetAt(nIndex);
      if (pItem != NULL)
      {
         if ((pItem->m_uCollectionID == uCollectionID)
          && (pItem->m_dwItemNumber == dwItemNumber))
         {
            nItem = nIndex;
            break;
         }
      }
   }

   return nItem;
}

// Find a preview item by a point (in dialog client coordinates.)
int CPreviewArray::Find(CPoint Point)
{
   int nItem = -1;

   for (int nIndex = 0; nIndex < GetSize(); nIndex++)
   {
      Item* pItem = GetAt(nIndex);
      if (pItem != NULL)
      {
         if (pItem->m_crFocusArea.PtInRect(Point))
         {
            nItem = nIndex;
            break;
         }
      }
   }

   return nItem;
}

/////////////////////////////////////////////////////////
// CPreviewArray::Item
//
// This class encapsulates the information describing
// a single preview thumbnail.

// Constructor.
CPreviewArray::Item::Item(USHORT uCollectionID, DWORD dwItemNumber, int nResultIndex,
								  void* pThumbnailData, const CString& csCaption, const CString& csAccessName,
								  const CRect& crThumbnail, const CRect& crFocusArea,
								  BOOL bIsLicensed, BOOL bTransparent)
{
   m_uCollectionID = uCollectionID;
   m_dwItemNumber = dwItemNumber;
	m_nResultIndex = nResultIndex;
   m_csCaption = csCaption;
   m_csAccessName = csAccessName;
   m_pThumbnailData = pThumbnailData;
   m_crThumbnail = crThumbnail;
   m_crFocusArea = crFocusArea;
   m_bIsLicensed = bIsLicensed;
   m_bTransparent = bTransparent;
}

// Destructor.
CPreviewArray::Item::~Item()
{
   if(m_pThumbnailData)
      Util::HugeFree(m_pThumbnailData);
   m_pThumbnailData = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Utility routine for OLE drag and drop.

HGLOBAL AFXAPI GetObjectDescriptorData(SIZEL sizel, POINTL pointl)
{
   HGLOBAL     hMem = NULL;

   // Allocate space for OBJECTDESCRIPTOR
   DWORD dwObjectDescSize = sizeof(OBJECTDESCRIPTOR);
   hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT, dwObjectDescSize);
   if (hMem != NULL)
   {
      LPOBJECTDESCRIPTOR lpOD = (LPOBJECTDESCRIPTOR)GlobalLock(hMem);

      lpOD->dwFullUserTypeName = 0; // zero offset indicates that string is not present
      lpOD->dwSrcOfCopy = 0;        // zero offset indicates that string is not present

      // Initialize the rest of the OBJECTDESCRIPTOR
      lpOD->cbSize       = dwObjectDescSize;
      lpOD->clsid        = CLSID_NULL;
      lpOD->dwDrawAspect = 0;
      lpOD->sizel        = sizel;
      lpOD->pointl       = pointl;
      lpOD->dwStatus     = 0;

      GlobalUnlock(hMem);
   }
   return hMem;
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserDialog

CBrowserDialog::CBrowserDialog(UINT uID, CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(uID, pParent),
	m_bAgProduct(FALSE)
{
   //{{AFX_DATA_INIT(CBrowserDialog)
	m_fMatchAny = 0;
   //}}AFX_DATA_INIT

   m_fUpdatePreview = TRUE;
   m_fRefreshSemaphore = FALSE;

   // Tree combo controls.
   m_pCollectionList = NULL;
   m_pGraphicTypeList = NULL;
	m_pProjectTypeList = NULL;
   m_pCategoryList = NULL;
   m_pToneList = NULL;
	m_pPreviewFont = NULL;

	// Edit Title control (user collections only)
	m_pEditTitle = NULL;
   m_uEditedCollectionID = (USHORT)-1;
   m_dwEditedItemNumber = (DWORD)-1;
	m_fEditingTitle = FALSE;

   // Default colors.
   m_clPreviewText = GetSysColor(COLOR_WINDOWTEXT);

   m_crMinimum.SetRectEmpty();

   // Currently selected item.
   m_uSelectedCollectionID = (USHORT)-1;
   m_dwSelectedItemNumber = (DWORD)-1;

   // First item row shown in preview area.
   m_nFirstRow = 0;

	// Column selected in preview area
	m_nCurColumn = 0;

   // Total number of items in result set.
   m_nResultItems = 0;

	// Do we have groups of items (e.g. project types)
	m_fHasGroups = FALSE;
	m_fShowHeadings = FALSE;	// show headings even if only one group

   // Default thumbnail and layout parameters.
	m_pThumbnailPalette = NULL;	// modified PrintMaster palette for transparency
   m_MediumThumbnailSize.cx = 160;
   m_MediumThumbnailSize.cy = 160;
   m_ThumbnailSize = m_MediumThumbnailSize;

   m_ThumbnailMargin.cx = 24;
   m_ThumbnailMargin.cy = 59;

   m_PreviewMargin.cx = 20;
   m_PreviewMargin.cy = 20;

   m_ThumbnailLayout.cx = 0;
   m_ThumbnailLayout.cy = 0;

   m_nCaptionHeight = (m_ThumbnailMargin.cy*2)/3;

   m_nFullThumbnailRows = 0;

   m_pItemPreviews = NULL;

	m_nHeadingBarHeight = 17;
	if(GET_PMWAPP()->LargeFontMode())
		m_nHeadingBarHeight = 20;


   // Flag used to ignore selection changes during initialization.
   m_fIgnoreSelectionChange = FALSE;

   m_hAccelTable = NULL;
   m_nCurPreviewItem = 0;
   m_thumbsize = thumbsizeMedium;

   m_crPreview.SetRectEmpty();

   m_nPopupMenuIndex = -1;

   m_csINISection = "Browser";

   m_pLicenseDll = NULL;
   m_pCurrentLicenseCollection = NULL;

   // Overriding document.
   m_pDoc = NULL;

	// Special collection button.
	m_pSpecialCollectionButton = m_pOnlineArtButton = NULL;

	m_pPathManager = NULL;
	m_pCollectionManager = NULL;
	m_pContentManager = NULL;
}

CBrowserDialog::~CBrowserDialog()
{
   // delete font
	if (m_pPreviewFont)
	{
		m_pPreviewFont->DeleteObject();
	   delete m_pPreviewFont;
	}

   // Get rid of tree combo controls.
   delete m_pCollectionList;
	delete m_pGraphicTypeList;
	delete m_pProjectTypeList;
   delete m_pCategoryList;
   delete m_pToneList;

	delete m_pEditTitle;
	delete m_pThumbnailPalette;

   // Delete any cached item previews.
   if(m_pItemPreviews)
      delete m_pItemPreviews;
   m_pItemPreviews = NULL;

	// Delete any Group info
	for (int i = 0; i < m_ItemGroupInfo.GetSize(); i++)
		delete m_ItemGroupInfo[i];
	m_ItemGroupInfo.RemoveAll();

	delete m_pSpecialCollectionButton;
	delete m_pOnlineArtButton;

   m_CollectionDraw.CloseDocument();

   // Delete the licensing DLL.
   delete m_pLicenseDll;   

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

void CBrowserDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CBrowserDialog)
	DDX_Radio(pDX, IDC_FIND_ANY, m_fMatchAny);
   //}}AFX_DATA_MAP
}

void CBrowserDialog::UpdateAllBrowsers(void)
{
	UpdateBrowser();
}

void CBrowserDialog::UpdateBrowser(void)
{
	UpdateResultSet();
}

// Extract category information from the content database and add it to
// the specified tree combo box.
void CBrowserDialog::AddCategories(CPMWCollection *pCollection,
											  CCategoryDir* pCategoryDir, int nCategory,
											  CTreeComboBox* pComboBox, void* pItemList, int nDepth)
{
   if(pCollection == NULL)
	{
		ASSERT(pCollection);
		return;
	}

   // Check if we have reached the maximum desired depth. This feature is used
	// for initializing the project type entries in the project collection list box.
	// Other code passes -1 for the initial depth which causes the full
   // category tree to be added to the list box.
	if (nDepth == 0)
		return;

	USHORT   usProductFlags = pCollection->GetProductFlags();

	// Loop through all categories at this level.
	while (nCategory >= 0)
	{
		// Get the current category information.
		CCategoryEntry Category;
		if (pCategoryDir->GetItem(nCategory, &Category) == ERRORCODE_None)
		{
			// Don't add categories that have no items for this product
			if(Category.GetFlags() == 0 || ((USHORT)Category.GetFlags() & usProductFlags) != 0)
			{
				// Get category name
				CString csName;
				Category.GetName(&csName);
				if (!csName.IsEmpty())
				{
					// Get index of existing category, or add new category
					int nIndex = pComboBox->FindStringExact(-1, csName, pItemList);
					if (nIndex == CB_ERR)
						nIndex = pComboBox->AddString(csName, nCategory, NULL, pItemList);

					if (nIndex != CB_ERR && nDepth != 1)
					{
						// Add subcategories and sub items to list box item.
						int nSubCategory = pCategoryDir->GetFirstChild(nCategory);
						if (nSubCategory > 0)
						{
							void* pSubItemList = pComboBox->GetItemList(nIndex);
							if (pSubItemList != NULL)
							{
								// Recursively add subcategories.
								AddCategories(pCollection, pCategoryDir, nSubCategory, pComboBox, pSubItemList, nDepth-1);
							}
						}
					}
				}
			}
		}

		// Move on to the next category at the current level.
		nCategory = pCategoryDir->GetNextSibling(nCategory);
		ASSERT(nCategory != NULL);
	}

	// Add "All" to the top of the list, if necessary.
	// We have to do this last since the list may be sorted.
	if ( (pComboBox == m_pCollectionList && nDepth == 1) ||
		  (pComboBox == m_pCategoryList && nDepth == -2) )
	{
		if (pComboBox->FindStringExact(-1, BrowserAllString(), pItemList) == CB_ERR)
			pComboBox->InsertString(0, BrowserAllString(), (DWORD)nCategory, NULL, pItemList);
	}
}

void CBrowserDialog::AddAllCategories(int nStartColl /*=-1*/, int nEndColl /*=-1*/)
{
	int nCollection, nEndCollection;

	// Three possibilities for which collections to process:
	// 1) nStartColl == -1, nEndColl == -1	: Process all collections
	// 2) nStartColl >= 0, nEndColl == -1	: Process only nStartColl collection
	// 3) nStartColl >= 0, nEndColl >= 0	: Process nStartColl through nEndColl
	nCollection = (nStartColl < 0) ? 0 : nStartColl;
	nEndCollection = nEndColl;
	if (nEndColl < 0)
	{
		if (nStartColl < 0)
			nEndCollection = m_pCollectionManager->NumberOfCollections(CollectionType(), CollectionSubType()) - 1;
		else
			nEndCollection = nCollection;
	}

	// Process each collection by calling ProcessCollection().
	for (; nCollection <= nEndCollection; nCollection++)
	{
		CPMWCollection* pCollection = m_pCollectionManager->GetCollection(nCollection, CollectionType(), CollectionSubType());
		if (pCollection != NULL)
		{
			// Open up a stream for reading category information.
			CCategoryDatabase* pDatabase;
			if (pCollection->NewCategoryStream(&pDatabase) != ERRORCODE_None)
				break;

			CCategoryDir* pCategoryDir = pDatabase->GetDir();
			if (pCategoryDir != NULL)
				AddCategories(pCollection, pCategoryDir, 0, m_pCategoryList, NULL, -1);

			pCollection->ReleaseCategoryStream(pDatabase);
		}
	}
}

void CProjectBrowserDialog::AddAllCategories(int nStartColl /*=-1*/,
															int nEndColl /*=-1*/,
															int nCurProject /*=0*/)
{
	int nCollection, nEndCollection;

	// Three possibilities for which collections to process:
	// 1) nStartColl == -1, nEndColl == -1	: Process all collections
	// 2) nStartColl >= 0, nEndColl == -1	: Process only nStartColl collection
	// 3) nStartColl >= 0, nEndColl >= 0	: Process nStartColl through nEndColl
	nCollection = (nStartColl < 0) ? 0 : nStartColl;
	nEndCollection = nEndColl;
	if (nEndColl < 0)
	{
		if (nStartColl < 0)
			nEndCollection = m_pCollectionManager->NumberOfCollections(CollectionType(), CollectionSubType()) - 1;
		else
			nEndCollection = nCollection;
	}

	// Process each collection by calling ProcessCollection().
	for (; nCollection <= nEndCollection; nCollection++)
	{
		CPMWCollection* pCollection = m_pCollectionManager->GetCollection(nCollection, CollectionType(), CollectionSubType());
		if (pCollection == NULL)
			continue;

		// Open up a stream for reading category information.
		CCategoryDatabase* pDatabase;
		if (pCollection->NewCategoryStream(&pDatabase) != ERRORCODE_None)
			return;

		CCategoryDir* pCategoryDir = pDatabase->GetDir();
		ASSERT(pCategoryDir);

		// Add all project super-categories to Project Type list
		if (nCurProject == 0)
			AddCategories(pCollection, pCategoryDir, 0, m_pProjectTypeList, NULL, 1);
   
		// Add all sub-categories for all projects to Category list
		// (or for specific project type, if specified)
		int nProject, nCategory;
		if (pCategoryDir != NULL)
		{
			if (nCurProject > 0)
			{
				// add categories/sub-categories for selected project type

				// find the right project location in the collection
				CString strCurProj;
				m_pProjectTypeList->GetLBText(nCurProject, strCurProj);
				int nProjPos = pCategoryDir->FindFirst(strCurProj);				
				if (nProjPos >= 0)
				{
					nCategory = pCategoryDir->GetFirstChild(nProjPos);
					if (nCategory >= 0)
						AddCategories(pCollection, pCategoryDir, nCategory, m_pCategoryList, NULL, -1);
				}
			}
			else
			{
				nProject = 0;
				nCategory = 0;

				// Get the first category of each project type.
				while(nProject >= 0 &&
						(nCategory = pCategoryDir->GetFirstChild(nProject)) > 0)
				{
					// Add all categories and sub-categories for this project
					AddCategories(pCollection, pCategoryDir, nCategory, m_pCategoryList, NULL, -1);
					// Get next project (Super Category)
					nProject = pCategoryDir->GetNextSibling(nProject);
				}
			}
		}
		pCollection->ReleaseCategoryStream(pDatabase);
	}
}

int CBrowserDialog::RestoreState()
{
   int         nCurKeyword = 0;
	BOOL			fMatchAll = FALSE;
   CComboBox*  pKeywordList = NULL;
   CIniFile    IniFile(GET_PMWAPP()->m_pszProfileName);
   CString     csCategory;

  // Don't load thumbsize for sentiments 
   if(CollectionType() != CPMWCollection::typeSentiments)
      m_thumbsize = (ThumbnailSizes) IniFile.GetInteger(m_csINISection, "ThumbnailSize", (int)thumbsizeMedium);

   if(m_csStartupCategory.IsEmpty() && m_csStartupCollection.IsEmpty())
   {
		// GCB 1/15/99 - normally should use RestoreTreeComboState on new graphic type combo
		// but we may be restoring settings based on double-clicked graphic instead of the
		// previous settings in the dialog
#if 0
      RestoreTreeComboState(m_pGraphicTypeList,        "Graphic Type");
#else
		if (m_pGraphicTypeList)
	      m_pGraphicTypeList->SetCurSel(BrowserAllString());
#endif

      RestoreTreeComboState(m_pToneList,        "Tone");
      pKeywordList = (CComboBox*)GetDlgItem(IDC_KEYWORD_LIST);
      if(pKeywordList)
      {
         int      nKeyword = 1;
         CString  csEntry, csKeyword;
         do
         {
            csEntry.Format("Keyword%02i", nKeyword);
            csKeyword = IniFile.GetString(m_csINISection, csEntry);
            if(!csKeyword.IsEmpty())
               pKeywordList->AddString(csKeyword);
            nKeyword++;
         }
         while(!csKeyword.IsEmpty() && nKeyword <= MAX_KEYWORD_COMBO_ITEMS);
      }
      nCurKeyword = IniFile.GetInteger(m_csINISection, "CurrentKeyword", (int) -1);
		fMatchAll = IniFile.GetInteger(m_csINISection, "MatchAllKeywords", 0);
		if (fMatchAll == 1)
		{
			m_fMatchAny = 1;	// set second radio button, which is "All"
			UpdateData(FALSE);
		}
   }
   else
   {
      CString csAll(BrowserAllString());

		if (m_pGraphicTypeList)
	      m_pGraphicTypeList->SetCurSel(csAll);

      m_pToneList->SetCurSel(csAll);
   }

   if(nCurKeyword >= 0 && pKeywordList)
      pKeywordList->SetCurSel(nCurKeyword);

   // Let selection change cause update of category list but not display
#if 1 // GCB 2/26/99
   m_fIgnoreSelectionChange = FALSE;
#endif
   m_fUpdatePreview = FALSE;
   if(m_csStartupCategory.IsEmpty() && m_csStartupCollection.IsEmpty())
   {
      RestoreTreeComboState(m_pCollectionList,  "Collection");
      RestoreTreeComboState(m_pProjectTypeList,	"Type");
#if 1 // GCB 2/26/99
#else
		ProjectTypeChanged();
#endif
      RestoreTreeComboState(m_pCategoryList,    "Category");
      CString csCollection = IniFile.GetString(m_csINISection, "CollectionID");
      if(!csCollection.IsEmpty())
      {
         CPMWCollection *pCollection = m_pCollectionManager->FindCollection(csCollection);
         if(pCollection)
            m_uSelectedCollectionID = pCollection->GetID();
         m_dwSelectedItemNumber = IniFile.GetDword(m_csINISection, "ItemNumber", m_dwSelectedItemNumber);
      }
   }
   else
   {
      CString  csStartupCollection;
      if(m_csStartupCollection.IsEmpty())
         csStartupCollection.LoadString(IDS_YOUR_OWN);
      else
         csStartupCollection = m_csStartupCollection;
      m_pCollectionList->SetCurSel(csStartupCollection);
   }

   SetThumbnailView(m_thumbsize);

	CRect crPosition;
   if (IniFile.GetWindowPosition(m_csINISection, NULL, &m_fIsMaximized, crPosition))
	{
		// If the window is not maximized, then move it. Otherwise,
		// we'll maximize it later
		if (!m_fIsMaximized)
		{
			MoveWindow(crPosition);
		}
	}
	else
	{
		// we do this to force a call to OnGetMinMaxInfo()
		CRect rect;
		GetWindowRect(rect);
		MoveWindow(rect);
	}

   return ERRORCODE_None;
}

BOOL CBrowserDialog::RestoreTreeComboState(CTreeComboBox* pTreeCombo, LPCSTR szEntry)
{
   int      nListIndex;
   CString  csValue;
   CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);

   if(pTreeCombo == NULL)
      return FALSE;

   csValue = IniFile.GetString(m_csINISection, szEntry, BrowserAllString());
   nListIndex = pTreeCombo->SetCurSel(csValue);
   return TRUE;
}

int CBrowserDialog::SaveState()
{
	if (m_hWnd == NULL)	// if initialization failed
		return ERRORCODE_NotInitialized;

	int nCurKeyword = -1;
	CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);

	IniFile.WriteWindowPosition(m_csINISection, this);
	IniFile.WriteInteger(m_csINISection, "ThumbnailSize", (int)m_thumbsize);
	if(!m_csStartupCategory.IsEmpty() || !m_csStartupCollection.IsEmpty())
		return ERRORCODE_None;

	SaveTreeComboState(m_pCollectionList,	"Collection");
	SaveTreeComboState(m_pProjectTypeList,	"Type");
	SaveTreeComboState(m_pCategoryList,		"Category");
	SaveTreeComboState(m_pToneList,			"Tone");
	CComboBox* pKeywordList = (CComboBox*)GetDlgItem(IDC_KEYWORD_LIST);
	if(pKeywordList)
		{
			CString  csEntry, csKeyword;
			// Skip "None" item which should be in 1st position
			int   nKeywordListCount = pKeywordList->GetCount();
			for(int i=1;i < MAX_KEYWORD_COMBO_ITEMS;i++)
				{
					csKeyword.Empty();
					csEntry.Format("Keyword%02i", i);
					if(i < nKeywordListCount)
						pKeywordList->GetLBText(i, csKeyword);
					IniFile.WriteString(m_csINISection, csEntry, csKeyword);
				}
			nCurKeyword =  pKeywordList->GetCurSel();
			// Never save position 0 which represents none
			if(nCurKeyword == 0)
				nCurKeyword = -1;
		}
	CPMWCollection *pCollection = m_pCollectionManager->FindCollection(m_uSelectedCollectionID);
	if(pCollection)
		{
			IniFile.WriteString(m_csINISection, "CollectionID", pCollection->GetBaseName());
			IniFile.WriteDword(m_csINISection, "ItemNumber", (DWORD)m_dwSelectedItemNumber);
		}
	IniFile.WriteInteger(m_csINISection, "CurrentKeyword", (int) nCurKeyword);

	UpdateData(TRUE);
	IniFile.WriteInteger(m_csINISection, "MatchAllKeywords", m_fMatchAny);

	return ERRORCODE_None;
}

BOOL CBrowserDialog::SaveTreeComboState(CTreeComboBox* pTreeCombo, LPCSTR szEntry)
   {
      int      nCurIndex;
      CString  csCurItem;
      CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);

      if(pTreeCombo == NULL)
         return FALSE;

      nCurIndex = pTreeCombo->GetCurSel();
      if(nCurIndex < 0)
         return FALSE;

      pTreeCombo->GetText(nCurIndex, csCurItem);
      IniFile.WriteString(m_csINISection, szEntry, csCurItem);
      return TRUE;
   }

BEGIN_MESSAGE_MAP(CBrowserDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CBrowserDialog)
   ON_WM_ERASEBKGND()
   ON_WM_ENTERIDLE()
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_CBN_SELCHANGE(IDC_COLLECTION, OnSelchangeCollectionCombo)
   ON_CBN_SELCHANGE(IDC_GRAPHIC_TYPE, OnSelchangeGraphicTypeCombo)
   ON_CBN_SELCHANGE(IDC_PROJECT_TYPE, OnSelchangeProjectTypeCombo)
   ON_CBN_SELCHANGE(IDC_CATEGORY, OnSelchangeCategoryCombo)
   ON_CBN_SELCHANGE(IDC_TONE, OnSelchangeToneCombo)
   ON_CBN_SELCHANGE(IDC_KEYWORD_LIST, OnSelchangeKeywordList)
   ON_BN_CLICKED(IDC_FIND, OnFind)
	ON_BN_CLICKED(IDC_FIND_ANY, OnFindAnyAll)
	ON_BN_CLICKED(IDC_FIND_ALL, OnFindAnyAll)
   ON_BN_CLICKED(IDC_BROWSER_SELECT, OnSelect)
   ON_BN_CLICKED(IDC_BROWSER_RESET, OnReset)
   ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
   ON_UPDATE_COMMAND_UI(ID_EDIT_ORGANIZE, OnUpdateEditOrganizeCategories)
	ON_COMMAND(ID_EDIT_ORGANIZE, OnEditOrganizeCategories)
   ON_BN_CLICKED(IDC_PRINT, OnPrint)
   ON_WM_LBUTTONDOWN()
   ON_WM_RBUTTONDOWN()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_VSCROLL()
   ON_COMMAND(IDC_BROWSER_PGDN, OnPageDown)
	ON_UPDATE_COMMAND_UI(IDC_BROWSER_PGDN, OnUpdatePageDown)
   ON_COMMAND(IDC_BROWSER_DELETE, OnDelete)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_DELETE, OnUpdateDelete)
   ON_COMMAND(IDC_BROWSER_DOWN, OnDown)
	ON_UPDATE_COMMAND_UI(IDC_BROWSER_PGUP, OnUpdatePageUp)
   ON_COMMAND(IDC_BROWSER_PGUP, OnPageUp)
   ON_COMMAND(IDC_BROWSER_UP, OnUp)
   ON_COMMAND(IDC_BROWSER_LEFT, OnLeft)
   ON_COMMAND(IDC_BROWSER_RIGHT, OnRight)
   ON_COMMAND(IDC_BROWSER_HOME, OnHome)
   ON_COMMAND(IDC_BROWSER_END, OnEnd)
   ON_WM_ACTIVATE()
   ON_WM_SETFOCUS()
   ON_UPDATE_COMMAND_UI(ID_BROWSER_INDICATOR_SIZE, OnUpdateSize)
   ON_COMMAND(ID_BROWSER_INDICATOR_SIZE, OnChangeSize)
   ON_UPDATE_COMMAND_UI(ID_BROWSER_INDICATOR_PAGE, OnUpdatePage)
   ON_COMMAND(ID_BROWSER_INDICATOR_PAGE, OnChangePage)
   ON_UPDATE_COMMAND_UI(ID_BROWSER_INDICATOR_ITEM, OnUpdateItem)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_VIEW_SMALL, OnUpdateViewSmall)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_VIEW_MEDIUM, OnUpdateViewMedium)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_VIEW_LARGE, OnUpdateViewLarge)
   ON_COMMAND(IDC_BROWSER_VIEW_SMALL, OnViewSmall)
   ON_COMMAND(IDC_BROWSER_VIEW_MEDIUM, OnViewMedium)
   ON_COMMAND(IDC_BROWSER_VIEW_LARGE, OnViewLarge)
   ON_WM_LBUTTONUP()
   ON_WM_INITMENUPOPUP()
   ON_COMMAND(ID_FILE_EXPORT, OnExport)
   ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, OnUpdateExport)
   ON_COMMAND(ID_FILE_IMPORT, OnImport)
   ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateImport)
   ON_COMMAND(ID_FILE_PROPERTIES, OnProperties)
   ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateProperties)
   ON_WM_CREATE()
   ON_WM_CLOSE()
   ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(IDC_EDIT_TITLE, OnKillfocusEditTitle)
   ON_COMMAND(ID_SPECIAL_COLLECTION, OnSpecialCollection)
	ON_COMMAND(ID_ONLINE_ART, OnOnlineArt)
   ON_WM_SHOWWINDOW()
   ON_COMMAND(IDC_BROWSER_SELECT, OnSelect)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
//   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
   ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_1, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_2, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_3, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_4, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_5, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_6, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_7, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_8, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_9, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_10, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_11, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_12, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_13, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_14, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_15, OnOrderForm)
   ON_COMMAND_EX(IDC_BROWSER_ORDER_FORM_16, OnOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_1, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_2, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_3, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_4, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_5, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_6, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_7, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_8, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_9, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_10, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_11, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_12, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_13, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_14, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_15, OnUpdateOrderForm)
   ON_UPDATE_COMMAND_UI(IDC_BROWSER_ORDER_FORM_16, OnUpdateOrderForm)
END_MESSAGE_MAP()

BOOL CBrowserDialog::DestroyWindow()
{
   SaveState();      // Save current browser settings to users .INI file
   return INHERITED::DestroyWindow();
}

void CBrowserDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
   CPmwDialogColor::OnActivate(nState, pWndOther, bMinimized);
}

void CBrowserDialog::OnSetFocus(CWnd* pOldWnd) 
{
   CPmwDialogColor::OnSetFocus(pOldWnd);
}

// Create a tree combo control and initialize it from a table if one is given.
BOOL CBrowserDialog::CreateTreeComboBox(UINT uID, CTreeComboBox*& pComboBox, ToneStyleMediaTableEntry* pInitializeTable)
{
   // Make sure the dialog control exists.
   if (GetDlgItem(uID) != NULL)
   {
      // Call NewTreeComboBox() to actually allocate the control.
      if ((pComboBox = NewTreeComboBox(uID)) != NULL)
      {
         // Create the tree combo box and replace the existing control.
         if (pComboBox->Create(WS_VISIBLE|WS_TABSTOP|WS_GROUP, this, uID))
         {
            // If the caller wants us to initialize the list, do so now.
            if (pInitializeTable != NULL)
            {
               // Add all the strings. The data value will be set to the bitmask.
               while (pInitializeTable->uStringID != 0)
               {
                  pComboBox->AddString(GET_PMWAPP()->GetResourceStringPointer(pInitializeTable->uStringID), pInitializeTable->dwBitmask);
                  pInitializeTable++;
               }
            }

            return TRUE;
         }
      }
   }

   return FALSE;
}

// Default function for creating and initializing a tree combo box object.
CTreeComboBox* CBrowserDialog::NewTreeComboBox(UINT uID)
{
   CTreeComboBox* pComboBox = NULL;
   TRY
   {
      // Allocate a normal CTreeComboBox object.
      pComboBox = new CTreeComboBox;

      // Tell the control what palette to use.
      pComboBox->m_pPalette = pOurPal;

      // Set the bitmap button. This bitmap is centered in the button.
      // The button border is drawn by the code. If this is not sufficient,
      // we could try using full bitmaps and using StretchBlt(). The issue
      // here is that the height of the control changes with the font
      // selected and the resolution of the user's screen.
      pComboBox->SetButtonBitmap(IDB_TREE_COMBO_BUTTON);

      // Set the normal arrow bitmap. This is the bitmap which is
      // shown to the right of the item name if its has sub items
      // and is *not* selected.
      pComboBox->SetNormalArrowBitmap(IDB_TREE_COMBO_NORMAL_ARROW);

      // Set the normal arrow bitmap. This is the bitmap which is
      // shown to the right of the item name if its has sub items
      // and is selected.

// *** The dark arrow look better with the colors Max selected ***
//    pComboBox->SetSelectedArrowBitmap(IDB_TREE_COMBO_SELECTED_ARROW);
      pComboBox->SetSelectedArrowBitmap(IDB_TREE_COMBO_NORMAL_ARROW);
   }
   END_TRY

   // Return a pointer to the object.
   return pComboBox;
}

// Get a pointer to the current CPmwDoc object.
// NULL is returned if there is no current document.
CPmwDoc* CBrowserDialog::GetDocument(void)
{
#if 0
   // Use 
   if (m_pDoc != NULL)
   {
      return m_pDoc;
   }

   CPmwDoc* pDoc = NULL;
   CPmwView* pView = GetView();
   if (pView != NULL)
   {
      pDoc = pView->GetDocument();
   }
   return pDoc;
#else
	return m_pDoc;
#endif
}

// Get a pointer to the current CPmwView object.
// NULL is returned if there is no current view.
CPmwView* CBrowserDialog::GetView(void)
{
   // Use overriding document if given.
   if (m_pDoc != NULL)
   {
      return m_pDoc->GetCurrentView();
   }
   return (CPmwView*)((CFrameWnd*)AfxGetMainWnd())->GetActiveFrame()->GetActiveView();
}

void CBrowserDialog::CalcRowsPerGroup()
{
	for (int group = 0; group < m_ItemGroupInfo.GetSize(); group++)
	{
		m_ItemGroupInfo[group]->nRows = m_ItemGroupInfo[group]->nCount / m_ThumbnailLayout.cx;
		if (m_ItemGroupInfo[group]->nCount % m_ThumbnailLayout.cx)
			m_ItemGroupInfo[group]->nRows++;
	}
}

int CBrowserDialog::CalcItemRows()
{
	int nItemRows = 0;
	if (m_fHasGroups)
	{
		for (int group = 0; group < m_ItemGroupInfo.GetSize(); group++)
			nItemRows += m_ItemGroupInfo[group]->nRows;
	}
	else
	{
		nItemRows = ((m_nResultItems-1) / m_ThumbnailLayout.cx) + 1;
	}

	return nItemRows;
}

// Compute the layout information for the current browser configuration.
void CBrowserDialog::ComputeLayout()
{
	SetPreviewRect();
   // Done computing preview size here
   if(m_thumbsize == thumbsizeLarge)
   {
      m_ThumbnailSize.cx = m_crPreview.Width() - m_PreviewMargin.cx*2 - m_ThumbnailMargin.cx;
      m_ThumbnailSize.cy = m_crPreview.Height() - m_PreviewMargin.cy*2 - m_ThumbnailMargin.cy;
   }

   // Figure out how many preview items will fit in the preview area.

   // Remember the previous thumbnail layout. If it changes,
   // we'll need to adjust the current row.

   CSize OldThumbnailLayout = m_ThumbnailLayout;

	if (m_thumbsize == thumbsizeLarge)
	{
		m_ThumbnailLayout.cx = m_ThumbnailLayout.cy = 1;
	}
	else
	{
		m_ThumbnailLayout.cx = (m_crPreview.Width() + m_ThumbnailMargin.cx - (2 * m_PreviewMargin.cx)) /
										(m_ThumbnailSize.cx + m_ThumbnailMargin.cx);
		m_ThumbnailLayout.cx = __max(m_ThumbnailLayout.cx, 1);

		m_ThumbnailLayout.cy = (m_crPreview.Height() - (2 * m_PreviewMargin.cy)) /
										(m_ThumbnailSize.cy + m_ThumbnailMargin.cy) + 1;
		m_ThumbnailLayout.cy = __max(m_ThumbnailLayout.cy, 1);
	}

	// The number of rows in the preview which have their full height displayed.
	m_nFullThumbnailRows = __max(m_ThumbnailLayout.cy - 1, 1);

	if (m_fHasGroups)
		CalcRowsPerGroup();

   // Recompute the current row number if the layout has changed.
   if (m_ThumbnailLayout.cx != OldThumbnailLayout.cx)
	{
		if (m_uSelectedCollectionID != (USHORT)-1)
			GotoItem(m_uSelectedCollectionID, m_dwSelectedItemNumber);
	}
}

void CBrowserDialog::SetPreviewRect()
{
   GetClientRect(m_crClient);

   // Compute the sidebar rect. This is the area on the left of the dialog with
	// controls in it. The dialog template contains a control (IDC_SIDEBAR_BOUNDS)
	// that determines the dividing line between the sidebar and the preview areas.
   m_crSidebar.SetRectEmpty();
   CWnd* pSidebar = GetDlgItem(IDC_SIDEBAR_BOUNDS);
   if (pSidebar != NULL)
   {
      pSidebar->GetWindowRect(m_crSidebar);
      ScreenToClient(m_crSidebar);
   }
	m_crSidebar.left = 0;
	m_crSidebar.top = 1;
	m_crSidebar.bottom = m_crClient.bottom;

   // Compute the preview area -- it's what's left over.
   m_crPreview = m_crClient;
	m_crPreview.top = 1;
   m_crPreview.left = m_crSidebar.right + 1;

	// shorten sidebar/preview areas by StatusBar height
   int nStatusBarHeight = GetStatusBarHeight();
   m_crPreview.bottom -= nStatusBarHeight;
   m_crSidebar.bottom -= nStatusBarHeight;

   // Compute the minimum rectangle. This too is set by the position of
   // the IDC_SIDEBAR_BOUNDS control. Basically, the minimum rectangle is
   // set so that the sidebar area always remains.
   m_crMinimum.SetRectEmpty();
   if (pSidebar != NULL)
   {
      pSidebar->GetWindowRect(m_crMinimum);
      ScreenToClient(m_crMinimum);
   }
   m_crMinimum.left = 0;
   m_crMinimum.top = 0;
   m_crMinimum.right = m_crSidebar.right;

}
	
void CBrowserDialog::PositionControls()
{
	// position the "Select" and "Cancel" buttons at bottom of Gallery,
	// just above the status bar
	CRect crDlg;
	GetClientRect(&crDlg);

	int nStatusBarHeight = 0;
	if(m_StatusBar.GetSafeHwnd())
	{
		CRect rectStatusBar;
		m_StatusBar.GetWindowRect(&rectStatusBar);
		nStatusBarHeight = rectStatusBar.Height();

      ScreenToClient(&rectStatusBar);
      rectStatusBar.left = 0;
      rectStatusBar.right = crDlg.right;
      rectStatusBar.bottom = crDlg.bottom;
      rectStatusBar.top = crDlg.bottom - nStatusBarHeight;
      m_StatusBar.MoveWindow(&rectStatusBar);
	}

	int nBtnYOffset = crDlg.bottom - nStatusBarHeight - 10;

	CRect crBtn;
	CWnd* pBtn = GetDlgItem(IDC_BROWSER_SELECT);
	if (pBtn)
	{
		pBtn->GetClientRect(&crBtn);
		pBtn->MapWindowPoints(this, &crBtn);
		crBtn.OffsetRect(0, nBtnYOffset - crBtn.bottom);
		pBtn->MoveWindow(crBtn);
	}

	pBtn = GetDlgItem(IDCANCEL);
	if (pBtn)
	{
		pBtn->GetClientRect(&crBtn);
		pBtn->MapWindowPoints(this, &crBtn);
		crBtn.OffsetRect(0, nBtnYOffset - crBtn.bottom);
		pBtn->MoveWindow(crBtn);
	}
}

int CBrowserDialog::GetStatusBarHeight()
{
	int height = 0;
	if(m_StatusBar.GetSafeHwnd())
	{
		CRect rect;
		m_StatusBar.GetWindowRect(&rect);
		height = rect.Height();
	}
	return height;
}

void CBrowserDialog::GetFrameRect(int nID, CRect &cr3DBorder)
{
   // Compute the rectangle where to draw the 3D "dip" around
   // the frame controls. This is set by the position of
   // the control specified by nID.
   cr3DBorder.SetRectEmpty();
   CWnd* pBorderControl = GetDlgItem(nID);
   if (pBorderControl != NULL)
   {
      pBorderControl->GetWindowRect(cr3DBorder);
      ScreenToClient(cr3DBorder);
   }
}

// Run through each of the known collections for the browser type and
// add call ProcessCollection() for each one.
void CBrowserDialog::ProcessCollections(void)
{
#ifdef FOR_LATER_MAYBE
	// reread the collection ini files in case something's changed
	m_pCollectionManager->UpdateCollections();
#endif

   if (m_pCollectionList != NULL)
   {
      // Add the "All" string to the collection list.
      m_pCollectionList->AddString(BrowserAllString(), NULL);
   }

   if (m_pProjectTypeList != NULL)
   {
      // Add the "All" string to the Project Type list
      m_pProjectTypeList->AddString(BrowserAllString(), NULL);
   }

   // Get the number of collections.
   int nCollections = m_pCollectionManager->NumberOfCollections(CollectionType(), CollectionSubType());

   // Process each collection by calling ProcessCollection().
   for (int nCollection = 0; nCollection < nCollections; nCollection++)
   {
      CPMWCollection* pCollection = m_pCollectionManager->GetCollection(nCollection, CollectionType(), CollectionSubType());
      if (pCollection != NULL)
      {
         ProcessCollection(pCollection);
      }
   }
}

// The default implementation of ProcessCollection().
// Add the collection's friendly  name to the collection combo box.
void CBrowserDialog::ProcessCollection(CPMWCollection* pCollection)
{
	if (m_pCollectionList == NULL)
		return;

	// see if this collection is part of a "Super Collection"
	CString strSuperColl = pCollection->GetSuperCollection();
	if (!strSuperColl.IsEmpty())
	{
		int nIndex = m_pCollectionList->FindStringExact(-1, strSuperColl);
		if (nIndex < 0)
		{
			// create new super collection entry, and add "All" as first choice
			nIndex = m_pCollectionList->AddString(strSuperColl, NULL);
			void* pSubItemList = m_pCollectionList->GetItemList(nIndex);
			m_pCollectionList->AddString(BrowserAllString(), NULL, NULL, pSubItemList);
		}

		// add collection to super collection
		void* pSubItemList = m_pCollectionList->GetItemList(nIndex);
		nIndex = m_pCollectionList->AddString(pCollection->GetFriendlyName(), (DWORD)pCollection, NULL, pSubItemList);
	}
	else
	{
		m_pCollectionList->AddString(pCollection->GetFriendlyName(), (DWORD)pCollection);
	}
}

// Default implementation of CollectionChanged().
// Update the result set for new selections.
void CBrowserDialog::CollectionChanged(void)
{
   UpdateResultSet();
}

// Default implementation of GraphicTypeChanged().
// Update the result set for new selections.
void CBrowserDialog::GraphicTypeChanged(void)
{
   UpdateResultSet();
}

// Default implementation of CategoryChanged().
// Update the result set for new selections.
void CBrowserDialog::CategoryChanged(void)
{
   UpdateResultSet();
}

// Default implementation of ToneChanged().
// Update the result set for new selections.
void CBrowserDialog::ToneChanged(void)
{
   UpdateResultSet();
}

// Default implementation of KeywordChanged().
// Update the result set for new selections.
void CBrowserDialog::KeywordChanged(int nIndex)
{
   // Get the current text.
   CComboBox* pKeywordList = (CComboBox*)GetDlgItem(IDC_KEYWORD_LIST);
   if (pKeywordList != NULL)
   {
      // Get the current keywords.
      CString csKeywords;
      if (nIndex == -1)
      {
         pKeywordList->GetWindowText(csKeywords);
      }
      else
      {
         pKeywordList->GetLBText(nIndex, csKeywords);
      }

      Util::Trim(csKeywords);

      if(csKeywords.Find(m_csKeywordNone) == 0)
      {
         csKeywords.Empty();
         pKeywordList->SetCurSel(-1);
      }
      if (!csKeywords.IsEmpty())
      {
         // Make sure the new keywords are first in the list.
         int nIndex = pKeywordList->FindStringExact(-1, csKeywords);
         if (nIndex != 1)
         {
            if (nIndex != CB_ERR)
            {
               pKeywordList->DeleteString(nIndex);
            }
            pKeywordList->InsertString(1, csKeywords);
            pKeywordList->SetCurSel(1);
         }
      }

      // Limit list to ten entries.
      while (pKeywordList->GetCount() > MAX_KEYWORD_COMBO_ITEMS)
      {
         int nIndex = pKeywordList->GetCount()-1;
         if (pKeywordList->DeleteString(nIndex) == CB_ERR)
         {
            // Don't get stuck in a loop!
            break;
         }
      }
   }
   
   UpdateResultSet();
   CWnd *pSelectButton = GetDlgItem(IDC_BROWSER_SELECT);
   if(pSelectButton)
      pSelectButton->SetFocus();
}

CPMWCollection* CBrowserDialog::GetCollectionSelection()
{
	if (m_pCollectionList == NULL)
		return NULL;

	int nIndex = m_pCollectionList->GetCurSel();
	if (nIndex == CB_ERR)
		return NULL;

	// See if this is a collection in a Super Collection
	void* pSubList = m_pCollectionList->GetItemList(nIndex);
	int nSubIndex = m_pCollectionList->GetCurSel(pSubList);
	if (nSubIndex >= 0)
		return (CPMWCollection*)(m_pCollectionList->GetItemDataPtr(nSubIndex, pSubList));
	else
		return (CPMWCollection*)(m_pCollectionList->GetItemDataPtr(nIndex));
}

// Default implementation of GetGraphicTypeSelection().
// Return the current graphic type selection for searching.
int CBrowserDialog::GetGraphicTypeSelection(void)
{
   int nGraphicType = CCategoryDataItem::cfNone;
   if (m_pGraphicTypeList != NULL)
   {
      int nIndex = m_pGraphicTypeList->GetCurSel();
      if (nIndex != CB_ERR)
         nGraphicType = m_pGraphicTypeList->GetItemData(nIndex);
   }
   return nGraphicType;
}

// Default implementation of GetCategorySelection().
// Return the search string for the current category selections.
void CBrowserDialog::GetCategorySelection(CString& csCategories)
{
   csCategories.Empty();
   if (m_pCategoryList != NULL)
   {
      CMlsStringArray Selections;
      GetSelections(Selections, m_pCategoryList);
      Selections.GetNames(csCategories);
   }
}

// Default implementation of GetKeywordSelection().
// Return the search string for the keyword selections.
void CBrowserDialog::GetKeywordSelection(CString& csKeywords)
{
   csKeywords.Empty();
   CComboBox* pKeywordList = (CComboBox *) GetDlgItem(IDC_KEYWORD_LIST);
   if (pKeywordList != NULL)
   {
      int nIndex = pKeywordList->GetCurSel();
      if(nIndex >= 0)
      {
         pKeywordList->GetLBText(nIndex, csKeywords);
         if(csKeywords.Find(m_csKeywordNone) == 0)
            csKeywords.Empty();
      }
      else
         pKeywordList->GetWindowText(csKeywords);
   }
}

// Default implementation of GetToneSelection().
// Return the current tone selection for searching.
int CBrowserDialog::GetToneSelection(void)
{
   int nTone = CCategoryDataItem::toneNone;
   if (m_pToneList != NULL)
   {
      int nIndex = m_pToneList->GetCurSel();
      if (nIndex != CB_ERR)
      {
         nTone = m_pToneList->GetItemData(nIndex);
      }
   }
   return nTone;
}

// Check if the browser is in "no matching items" state.
BOOL CBrowserDialog::NoMatchingItems(void)
{
	BOOL fResult = FALSE;

	// Make sure no items were found.
	if (m_nResultItems == 0)
	{
		// Did the user actually look for something?
		CString csKeywords;
		GetKeywordSelection(csKeywords);

		fResult = (csKeywords.GetLength () > 0) ||
			(m_pGraphicTypeList && m_pGraphicTypeList->GetCurSel() > 0) ||
			(m_pToneList->GetCurSel() > 0);
	}

	return fResult;
}

// Update the search result set for the new search selections.
// Basically, we get the current search criteria (collection,
// category, tone, and keywords) and hand them
// to the Search() function which does all the real work.
void CBrowserDialog::UpdateResultSet(void)
{
   // PMGTODO: It may make sense to remember the last set of search
   // parameters and, if they have not changed, avoid the call to
   // Search(). We're going to need some sort of saved state information
   // anyway so we can remember the browser's settings across invocations
   // of the browser. Perhaps this should be static data belonging to
   // the appropriate browser class.

	BeginWaitCursor();

   // Clear total number of items in result set.
   m_nResultItems = 0;

   // Get the current category selection.
   CString csCategories;
   GetCategorySelection(csCategories);
	CString csMatchingSet;
	csMatchingSet.LoadString(IDS_MATCHING_SET);
	if (csCategories == csMatchingSet)
		GET_PMWAPP()->ShowHelpfulHint("matching sets", this);

   // Get the current keyword selection.
   CString csKeywords;
   GetKeywordSelection(csKeywords);

   //
	// Set up the search context...
	//

   // Get the current graphic type selection.
   m_SearchContext.m_nGraphicType = GetGraphicTypeSelection();

   // Get the current tone selection.
   int nTone = GetToneSelection();

   // Get the collection type and subtype.
   m_SearchContext.m_nType = CollectionType();
   m_SearchContext.m_nSubType = CollectionSubType();

	// Get the current collection selection.
	m_SearchContext.m_ptrAryCollections.RemoveAll();
	CPMWCollection* pCollection = GetCollectionSelection();
	if (pCollection == NULL)
	{
		// There's two possibilities here:
		// 1) User has "All" collections selected - send empty collection set
		// 2) User has a SuperCollection selected - send collections in group
		CString strColl;
		int nIndex = m_pCollectionList->GetCurSel();
		m_pCollectionList->GetText(nIndex, strColl);
		if (strColl != BrowserAllString())
		{
			// We've got a super collection - get all the collections in it.
			// We skip the first, which is "All"
			void* pSubList = m_pCollectionList->GetItemList(nIndex);
			for (int i = 1; i < m_pCollectionList->GetCount(pSubList); i++)
			{
				m_SearchContext.m_ptrAryCollections.Add(m_pCollectionList->GetItemDataPtr(i, pSubList));
			}
		}
	}
	else // single collection
	{
		m_SearchContext.m_ptrAryCollections.Add(pCollection);
	}

	// Fill array of Project Types if appropriate
	m_SearchContext.m_strAryCategories.RemoveAll();
	CMlsStringArray catArray(csCategories);
	if (CollectionType() == CPMWCollection::typeProjects &&
		 catArray[0] == BrowserAllString())
	{
		 // add all project types to category array (skipping "All")
		int cnt = m_pProjectTypeList->GetCount();
		m_SearchContext.m_strAryCategories.SetSize(cnt-1);
		for (int i = 1; i < cnt; i++)
		{
			CString str;
			m_pProjectTypeList->GetLBText(i, str);

			for (int j = 1; j < catArray.GetSize(); j++)
				str += "|" + catArray[j];

			m_SearchContext.m_strAryCategories.SetAt(i-1, str);
		}
	}
	else
	{
		m_SearchContext.m_strAryCategories.SetSize(1);
		m_SearchContext.m_strAryCategories.SetAt(0, csCategories);
	}

   m_SearchContext.m_strKeywords = csKeywords;
	m_SearchContext.m_fKeywordsMatchAll = FALSE;
	CButton* pWnd = (CButton*)GetDlgItem(IDC_FIND_ALL);
	if (pWnd)
	{
		if (pWnd->GetCheck())
			m_SearchContext.m_fKeywordsMatchAll = TRUE;
	}
   m_SearchContext.m_nTone = nTone;

   // Do the search.
   int nResult = m_pCollectionManager->Search(m_SearchContext);
   TRACE("Update the result set  %s  %s (%d) (found %d Items)\n", (LPCSTR)csCategories, (LPCSTR)csKeywords, nResult, m_SearchContext.GetResultCount());
   if (nResult == ERRORCODE_None)
   {
      m_nResultItems = m_SearchContext.GetResultCount();
		if (m_nResultItems > 0 && m_fHasGroups)
			SetItemsPerGroup();
   }

   // Since we have a new result set, reset the preview information.
   m_uSelectedCollectionID = (USHORT)-1;
   m_dwSelectedItemNumber = (DWORD)-1;
   m_nFirstRow = 0;
	m_nCurColumn = 0;

   if(m_fUpdatePreview)
   {
      UpdateItemPreviews(FALSE);
      UpdateScrollBar();

      if(m_pItemPreviews->GetSize() > 0)
      {
         CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(0);
         SelectItem(pItem->m_uCollectionID, pItem->m_dwItemNumber);

			if(m_thumbsize == thumbsizeLarge)
				SetThumbnailView(thumbsizeMedium);
      }
      // Redraw the preview area.
      InvalidateRect(m_crPreview);
   }

	EndWaitCursor();
}

void CBrowserDialog::SetItemsPerGroup()
{
	// clear out any old group info
	for (int i = 0; i < m_ItemGroupInfo.GetSize(); i++)
		delete m_ItemGroupInfo[i];
	m_ItemGroupInfo.RemoveAll();

	CString strLastGroup("");
	int nWhichGroup = -1;
	for (int item = 0; item < m_nResultItems; item++)
	{
		CPMWCollectionResult Result = m_SearchContext.GetSearchResult(item);

		// Unpack the collection ID and the item number for the current item
		USHORT uCollectionID = Result.GetCollectionID();
		DWORD dwItemNumber = Result.GetItemNumber();

		CPMWCollection* pCollection;
		pCollection = m_pCollectionManager->FindCollection((BYTE)uCollectionID, CollectionType(), CollectionSubType());
		if (pCollection == NULL)
		{
			ASSERT(pCollection);
			continue;
		}

		CString str = pCollection->GetCategory(dwItemNumber);
		int index = str.Find(DELIMITER_CHAR);
		if (index != -1)
			str = str.Left(index);

		if (strLastGroup.CompareNoCase(str) != 0)
		{
			// start of new group
			strLastGroup = str;
			nWhichGroup++;
			GroupInfo* pGroup = new GroupInfo;
			pGroup->strGroup = str;
			pGroup->nCount = 1;
			m_ItemGroupInfo.Add(pGroup);
		}
		else
		{
			if (nWhichGroup != -1)
				m_ItemGroupInfo[nWhichGroup]->nCount++;
		}
	}

	CalcRowsPerGroup();
}

// Get collection name, item file name, and item friendly name for the
// given collection/item pair.
BOOL CBrowserDialog::GetItemInformation(USHORT uCollectionID, DWORD dwItemNumber,
													 CString& csCollection, CString& csFile, CString& csFriendlyName,
													 CString& csAccessName, CString* pcsKeywords /*=NULL*/,
													 BOOL* pfCropCopyright/*=NULL*/)
{
   BOOL fResult = FALSE;

   // Clear strings.
   csCollection.Empty();
   csFile.Empty();
   csFriendlyName.Empty();

   CPMWCollection* pCollection = NULL;
   CFileItems* pFileItems = NULL;
   CFileContent* pContent = NULL;
   BOOL fExternal = FALSE;

   TRY
   {
      // Look up the collection by its ID.
      pCollection = m_pCollectionManager->FindCollection((BYTE)uCollectionID, CollectionType(), CollectionSubType());
      if (pCollection != NULL)
      {
         // Save the collection name.
         csCollection = pCollection->GetBaseName();

         // Open up an item information stream so we can get the file name and
         // friendly name of the given item.
         if (pCollection->NewItemInfoStream(&pFileItems) == ERRORCODE_None)
         {
            // Search for the item.
            CItemsDataEntry Information;
            if (pFileItems->Find(dwItemNumber, &Information) == ERRORCODE_None)
            {
               // Read the item information.
               if (pFileItems->Read() == ERRORCODE_None)
               {
                  // Save the file name and friendly name.
                  csFile = Information.GetFileName();
                  csFriendlyName = Information.GetFriendlyName();

						// Check if we need to strip off copyright from bottom of image
						if (pfCropCopyright != NULL)
						{
							CCategoryDataItem cdItem;
							if ((pCollection->GetCategoryDataItem(dwItemNumber, &cdItem) == ERRORCODE_None) &&
								 (cdItem.GetCustomFlags() & CCategoryDataItem::cfCropCopyright))
								*pfCropCopyright = TRUE;
							else
								*pfCropCopyright = FALSE;
						}

                  if (pcsKeywords != NULL)
                  {
                     CFileKeywordDir* pKeywords = NULL;
                     if (Information.GetKeywordCount() > 0
                           && pCollection->NewKeywordStream(&pKeywords) == ERRORCODE_None)
                     {
                        // Read the keywords.
                        for (int nKeyword = 0;
                              nKeyword < Information.GetKeywordCount();
                              nKeyword++)
                        {
                           CString csKeyword;
                           if (pKeywords->Find(Information.GetKeywordID(nKeyword), csKeyword) == ERRORCODE_None)
                           {
                              if (!pcsKeywords->IsEmpty())
                              {
                                 *pcsKeywords += ' ';
                              }
                              *pcsKeywords += csKeyword;
                           }
                        }
                        pCollection->ReleaseKeywordStream(pKeywords);
                     }
                  }
                  // Everything worked!
                  fResult = TRUE;
               }
            }
         }
         // Determine whether this item is internal or external.
         if (fResult)
         {
#if 0
            fResult = FALSE;
            
            if (pCollection->NewContentStream(&pContent) == ERRORCODE_None)
            {
               // Search for the item.
               CContentDirEntry Dir;
               Dir.SetKey(csFile);
               CContentDataEntry Data;
               if (pContent->Find(&Dir, &Data) == ERRORCODE_None)
               {
                  fExternal = (Dir.GetStorageMethod() == CSortedDBDirEntry::storageExternal);
                  fResult = TRUE;
               }
            }
#else
            fExternal = pCollection->IsUserCollection()
				|| m_pPathManager->GetPathBindingType(csFile) != PBT_None;
#endif
         }
      }
   }
   END_TRY

   if (fResult)
   {
      // Build the access file name.
      if (fExternal)
      {
         // Use the full path.
         csAccessName = csFile;
      }
      else
      {
         csAccessName.Format("[[G:%s]]\\%s", (LPCSTR)csCollection, (LPCSTR)csFile);
      }
   }

   // Free any resources we may still have open.
   if (pCollection != NULL)
   {
      if (pContent != NULL)
         pCollection->ReleaseContentStream(pContent);
      if (pFileItems != NULL)
         pCollection->ReleaseItemInfoStream(pFileItems);
   }

   return fResult;
}

// Compute the proper location and size for the given thumbnail item.
void CBrowserDialog::GetPreviewBounds(
	CRect& crThumbnail,
	CRect& crFocus,
	int nThumbnailWidth, int nThumbnailHeight,
	int nItemPos,							// 0-based item within group
	int nGroup/*=0*/,						// 0-based group item is in
	int nLastGroupBottomPos/*=0*/)	// y coord of bottom of last group
{
	// NOTE: nItemPos is the element in current visable layout, i.e. the top
	// left visible element will be 0, no matter which page is currently showing.

	// Fit the thumbnail aspect into the given width and height.
	Util::FitAspect(&nThumbnailWidth, &nThumbnailHeight,
						 m_ThumbnailSize.cx, m_ThumbnailSize.cy);

	// Build thumbnail rect with thumbnail bounds.
	crThumbnail.SetRect(0, 0,
							  nThumbnailWidth, nThumbnailHeight);
	// Center thumbnail in available width and height.
	crThumbnail.OffsetRect((m_ThumbnailSize.cx - nThumbnailWidth) / 2,
      (m_ThumbnailSize.cy-nThumbnailHeight)/2);

	// Build preview area rect with full thumbnail size
	CRect crPreview;
	crPreview.SetRect(0, 0,
							m_ThumbnailSize.cx + m_ThumbnailMargin.cx,
							m_ThumbnailSize.cy + m_ThumbnailMargin.cy);

	// Offset by position in preview grid.

	int nXOffset = m_PreviewMargin.cx +
			((nItemPos % m_ThumbnailLayout.cx) * (m_ThumbnailSize.cx + m_ThumbnailMargin.cx));

	int nYOffset = m_PreviewMargin.cy +
			((nItemPos / m_ThumbnailLayout.cx) * (m_ThumbnailSize.cy + m_ThumbnailMargin.cy));

	if (nGroup != 0)
	{
		nYOffset += nLastGroupBottomPos + m_nHeadingBarHeight;
	}

	crThumbnail.OffsetRect(nXOffset, nYOffset);
	crPreview.OffsetRect(nXOffset, nYOffset);

	// Offset thumbnail by horizontal margin and vertical margin compensating for caption
	crThumbnail.OffsetRect(m_ThumbnailMargin.cx / 2,
      (m_ThumbnailMargin.cy - m_nCaptionHeight) / 2);

   // Offset into preview area.
	crThumbnail.OffsetRect(m_crPreview.left, m_crPreview.top);
	crPreview.OffsetRect(m_crPreview.left, m_crPreview.top);

	crFocus = crPreview;

	if (m_thumbsize != thumbsizeLarge)
	{
		crFocus.top = crThumbnail.top -
							((m_ThumbnailMargin.cy - m_nCaptionHeight) / 2);
		crFocus.bottom = crThumbnail.bottom + 
							((m_ThumbnailMargin.cy - m_nCaptionHeight) / 2 + m_nCaptionHeight);
	}
}

int CBrowserDialog::CalcNumGroupItems(int nFirstRow, int nNumRows)
{
	int nLastGroup = m_ItemGroupInfo.GetUpperBound();
	if (nLastGroup == -1)
		return 0;

	// maximum number of items that will fit in current layout
	int nMaxItems = m_ThumbnailLayout.cx * m_ThumbnailLayout.cy;

	int nItems = 0;	// number of items found (value to return)
	int nCurRow = 0;	// which row we're on now

	if (nLastGroup == 0)
	{
		// trivial case: only one group
		int nItemsLeft = m_nResultItems - (m_nFirstRow * m_ThumbnailLayout.cx);
		nItems = min(m_ThumbnailLayout.cx * m_ThumbnailLayout.cy, nItemsLeft);
		return nItems;
	}

	// get to the first row of the group we're starting with
	int nGroup = 0;
	for (nGroup = 0; nGroup <= nLastGroup; nGroup++)
	{
		if (nCurRow + m_ItemGroupInfo[nGroup]->nRows > nFirstRow)
			break;

		nCurRow += m_ItemGroupInfo[nGroup]->nRows;
	}

	if (nGroup > nLastGroup)
	{
		// Something's messed up
		ASSERT(0);
		nGroup = nLastGroup;
		return 0;
	}

	// get number of items in nFirstRow's group
	int nItemsToSkip = (nFirstRow - nCurRow) * m_ThumbnailLayout.cx;
	nItems = m_ItemGroupInfo[nGroup]->nCount - nItemsToSkip;
	if (nItems >= nMaxItems)
		return nMaxItems;

	nCurRow += m_ItemGroupInfo[nGroup]->nRows;
	nGroup++;

	// have we run out of groups?
	if (nGroup > nLastGroup)
		return nItems;

	// now add items until we have enough rows, run out of groups,
	// or have as many items as will fit on the page
	while (nGroup <= nLastGroup &&
			 nCurRow + m_ItemGroupInfo[nGroup]->nRows < nFirstRow + nNumRows)
	{
		nItems += m_ItemGroupInfo[nGroup]->nCount;
		if (nItems >= nMaxItems)
			return nMaxItems;

		nCurRow += m_ItemGroupInfo[nGroup]->nRows;
		nGroup++;
	};

	// add items from last group
	if (nGroup <= nLastGroup)
	{
		int nItemsToAdd = (nFirstRow + nNumRows - nCurRow) * m_ThumbnailLayout.cx;
		nItems += min(nItemsToAdd, m_ItemGroupInfo[nGroup]->nCount);
	}

	return nItems;
}

int CBrowserDialog::CalcGroupStartingItem(int nFirstRow)
{
	int nStartingItem = 0;	// what we're looking for
	int nCurRow = 0;			// which row we're on now

	for (int nGroup = 0; nGroup < m_ItemGroupInfo.GetSize(); nGroup++)
	{
		if (nCurRow + m_ItemGroupInfo[nGroup]->nRows > nFirstRow)
			break;

		nCurRow += m_ItemGroupInfo[nGroup]->nRows;
		nStartingItem += m_ItemGroupInfo[nGroup]->nCount;
	}

	// now add full row counts to the row we're looking for
	nStartingItem += (nFirstRow - nCurRow) * m_ThumbnailLayout.cx;

	return nStartingItem;
}

// Calculate the 0-based column number that pItem is in (based on search results)
int CBrowserDialog::CalcColumnFromItem(CPreviewArray::Item* pItem)
{
	int nColumn = 0;

	if (m_fHasGroups)
	{
		int nItemCenter = (pItem->m_crThumbnail.left + pItem->m_crThumbnail.right) / 2;
		nItemCenter -= m_crPreview.left;

		for (nColumn = 0; nColumn < m_ThumbnailLayout.cx; nColumn++)
		{
			int nColLeft = m_PreviewMargin.cx +
				((nColumn % m_ThumbnailLayout.cx) * (m_ThumbnailSize.cx + m_ThumbnailMargin.cx));
			int nColRight = nColLeft + m_ThumbnailSize.cx;
			if (nItemCenter > nColLeft && nItemCenter < nColRight)
				break;
		}
	}
	else
	{
		nColumn = pItem->m_nResultIndex % m_ThumbnailLayout.cx;
	}

	return nColumn;
}

// Calculate the 0-based row number that nItem is in (based on search results)
int CBrowserDialog::CalcRowFromItemNumber(int nItem)
{
	int nRow = 0;		// row we're looking for
	int nItemCnt = 0;

	for (int nGroup = 0; nGroup < m_ItemGroupInfo.GetSize(); nGroup++)
	{
		if (nItemCnt + m_ItemGroupInfo[nGroup]->nCount > nItem)
			break;

		nItemCnt += m_ItemGroupInfo[nGroup]->nCount;
		nRow += m_ItemGroupInfo[nGroup]->nRows;
	}

	// figure out which row of the group the item's in
	nRow += (nItem - nItemCnt) / m_ThumbnailLayout.cx;
	if (nRow < 0)
		nRow = 0;

	return nRow;
}

// Update the item preview array with new previews.
void CBrowserDialog::UpdateItemPreviews(BOOL fReset)
{
	BeginWaitCursor();

	// A cache for the streams we use.
	CStreamCache StreamCache(m_pCollectionManager);

	CPreviewArray* pNewItemPreviews = NULL;
	TRY
	{
		// If the user wants to ignore the old previews, then get rid of them now.
		if (fReset)
		{
			delete m_pItemPreviews;
			m_pItemPreviews = NULL;
		}

		// Allocate an array for the new item previews.
		pNewItemPreviews = new CPreviewArray;

		// Check if the collection has any current search results that need
		// to be previewed.
		ASSERT(m_nResultItems == m_SearchContext.GetResultCount());
		if (m_nResultItems > 0)
		{
			// Loop through each of the results that need to be previewed.

			CString strLastProjectType = "";	// used to check for new project type
			int nGroupItemPos = 0;				// item position within group
			int nProjectGroup = 0;				// which project group the item is in
			int nLastGroupBottomPos = 0;		// bottom of last preview item

			int nItems = 0;
			
			// Calculate number of items that will fit in preview area
			if (m_fHasGroups && m_ItemGroupInfo.GetSize() > 1)
			{
				nItems = CalcNumGroupItems(m_nFirstRow, m_ThumbnailLayout.cy);
			}
			else
			{
				int nItemsLeft = m_nResultItems - (m_nFirstRow * m_ThumbnailLayout.cx);
				nItems = min(m_ThumbnailLayout.cx * m_ThumbnailLayout.cy, nItemsLeft);
			}

			// Calculate where we're starting in the results list
			int nStartingItem;
			if (m_fHasGroups)
				nStartingItem = CalcGroupStartingItem(m_nFirstRow);
			else
				nStartingItem = m_nFirstRow * m_ThumbnailLayout.cx;

			for (int nItem = 0; nItem < nItems; nItem++)
			{
				// Get the current search result.
				int nResult = nItem + nStartingItem;
				if (nResult >= m_SearchContext.GetResultCount())
				{
					ASSERT(0);
					break;
				}

				CPMWCollectionResult Result = m_SearchContext.GetSearchResult(nResult);

				// Unpack the collection ID and the item number for the
				// current result.
				USHORT uCollectionID = Result.GetCollectionID();
				DWORD dwItemNumber = Result.GetItemNumber();

				CPMWCollection* pCollection;
				pCollection = m_pCollectionManager->FindCollection((BYTE)uCollectionID, CollectionType(), CollectionSubType());
				ASSERT(pCollection);
				if(m_thumbsize != thumbsizeLarge)
				{
					// Get the thumbnail information for the current item.
					// Start with the thumbnail stream...
					CFileThumbnails* pFileThumbnails = StreamCache.GetThumbnailStream(uCollectionID);
					if (pFileThumbnails == NULL)
						break;

					CThumbnailDataEntry Thumbnail;
					if (pFileThumbnails->Find(dwItemNumber, &Thumbnail) != ERRORCODE_None)
						continue;

					// we're cool

					int nItemPos = nItem;

					// Check if these are Project collections, and we've changed project type
					// If so, we need to display the project type in the preview area, etc.
					if (m_fHasGroups)
					{
						CString str = pCollection->GetCategory(dwItemNumber);
						int index = str.Find(DELIMITER_CHAR);
						if (index != -1)
							str = str.Left(index);

						if (strLastProjectType.CompareNoCase(str) != 0)
						{
							// start of new Project Type group
							strLastProjectType = str;
							if (nItemPos != 0)
							{
								nProjectGroup++;
								nItemPos = nGroupItemPos = 0;	// reset group item position

								// get position of last item in previous group
								CPMWCollectionResult PrevResult = m_SearchContext.GetSearchResult(nResult-1);
								USHORT uPrevCollectionID = PrevResult.GetCollectionID();
								DWORD dwPrevItemNumber = PrevResult.GetItemNumber();
								int nPrevItem = pNewItemPreviews->Find(uPrevCollectionID, dwPrevItemNumber);
								if (nPrevItem != -1)
								{
									CPreviewArray::Item* pPrevItem = pNewItemPreviews->GetAt(nPrevItem);
									if (pPrevItem)
									{
										CRect rcThumb = pPrevItem->m_crThumbnail;
										nLastGroupBottomPos = (rcThumb.top + rcThumb.bottom + 
																	  m_ThumbnailSize.cy + m_ThumbnailMargin.cy) / 2;
									}
								}
							}
						}
						else if (nProjectGroup != 0)
						{
							nItemPos = ++nGroupItemPos;	// increment group item position
						}
					}

					// Get the preview bounds for the current thumbnail.
					CRect crThumbnail, crFocus;
					GetPreviewBounds(crThumbnail, crFocus,
										  Thumbnail.m_DataHeader.GetWidth(),
										  Thumbnail.m_DataHeader.GetHeight(),
										  nItemPos, nProjectGroup, nLastGroupBottomPos);

					// Check if the thumbnail is already in the old preview list.
					BOOL fUsedOldPreview = FALSE;
					if (m_pItemPreviews != NULL)
					{
						int nOldItem = m_pItemPreviews->Find(uCollectionID, dwItemNumber);
						if (nOldItem != -1)
						{
							CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(nOldItem);
							if ((pItem->m_crThumbnail.Width() == crThumbnail.Width()) &&
								 (pItem->m_crThumbnail.Height() == crThumbnail.Height()))
							{
								// Use the old preview with the new bounds.
								// Move it from the old list to the new list. 
								m_pItemPreviews->SetAt(nOldItem, NULL);
								pItem->m_crThumbnail = crThumbnail;
								pItem->m_crFocusArea = crFocus;
								pItem->m_nResultIndex = nResult;
								pNewItemPreviews->Add(pItem);

								fUsedOldPreview = TRUE;
							}
						}
					}

					// If we used the old preview, we're done. Otherwise,
					// we're going to have to create one from scratch.
					if (fUsedOldPreview)
						continue;

					// Get the item stream for this collection.
					CFileItems* pFileItems = StreamCache.GetItemStream(uCollectionID);
					if (pFileItems == NULL)
						break;

					// Get the information for the item.
					CItemsDataEntry Information;
					if (pFileItems->Find(dwItemNumber, &Information) != ERRORCODE_None)
						continue;

					// Read the information.
					if (pFileItems->Read() != ERRORCODE_None)
						continue;

					// Get the thumbnail data.
					BOOL fGotThumbnailData = FALSE;
					DWORD dwDataSize = Thumbnail.GetItemSize();
					void* pData = NULL;
					TRY
					{
						// Attempt to allocate a block of memory for the thumbnail data.
						pData = Util::HugeAlloc(dwDataSize);

						// Attach the buffer to the thumbnail object.
						Thumbnail.Attach(pData, dwDataSize);

						// Attempt to read in the thumbnail data.
						if (pFileThumbnails->Read(dwDataSize) == ERRORCODE_None)
							fGotThumbnailData = TRUE;
					}
					END_TRY

					if (fGotThumbnailData)
					{
						// Create a preview thumbnail. This will be a packed DIB structure
						// that other code can blt onto the preview area.
						BOOL bTransparent;
						void* pThumbnailData = MakeThumbnail(uCollectionID, dwItemNumber,
																		 crThumbnail, Thumbnail,
																		 pData, &bTransparent);
						if (pThumbnailData != NULL)
						{
							// Get friendly name for caption.
							CString csCaption;
							CString csFile;
							BOOL bIsLicensed = TRUE;
							TRY
							{
								csCaption = Information.GetFriendlyName();
								csFile = Information.GetFileName();
								bIsLicensed = GetUnlicensedCaption(pCollection, dwItemNumber, csCaption);
#ifdef LOCALIZE
								csCaption = CurrentLocale.HandleBrowserCaption(m_csINISection, csCaption, csFile);
#endif
							}
							END_TRY

							// Add the preview to the preview list.
							pNewItemPreviews->Add(uCollectionID, dwItemNumber, nResult,
														 pThumbnailData, csCaption, csFile,
														 crThumbnail, crFocus,
														 bIsLicensed, bTransparent);
						}
					}

					// Detach any buffer we allocated from the thumbnail object.
					Thumbnail.Detach();

					// Delete the thumbnail buffer we allocated.
					Util::HugeFree(pData);
					pData = NULL;
				}
				else // large thumbnails
				{
					// Get the item stream for this collection.
					CFileItems* pFileItems = StreamCache.GetItemStream(uCollectionID);
					if (pFileItems == NULL)
						break;

					// Get the information for the item.
					CItemsDataEntry Information;
					if (pFileItems->Find(dwItemNumber, &Information) != ERRORCODE_None)
						continue;

					// Read the information.
					if (pFileItems->Read() != ERRORCODE_None)
						continue;

					// Get friendly name for caption.
					BOOL bIsLicensed = TRUE;
					CString csCaption;
					CString csFile;
					TRY
					{
						csCaption = Information.GetFriendlyName();
						csFile = Information.GetFileName();
						bIsLicensed = GetUnlicensedCaption(pCollection, dwItemNumber, csCaption);
					}
					END_TRY

					CRect crThumbnail, crFocus;
					GetPreviewBounds(crThumbnail, crFocus,
										  m_ThumbnailSize.cx, m_ThumbnailSize.cy, nItem);

					// Add the preview to the preview list.
					pNewItemPreviews->Add(uCollectionID, dwItemNumber, nResult,
												 NULL, csCaption, csFile,
												 crThumbnail, crFocus,
												 bIsLicensed, FALSE); // not transparent
				} // end large thumbnails
			} // end for()
		}
	}
	CATCH_ALL(exception)
	{
		TRACE("Exception in UpdateItemPreviews\n");
	}
	END_CATCH_ALL

	EndWaitCursor();

	// Delete any old previews that are still left.
	delete m_pItemPreviews;

	// Use the new item previews.
	m_pItemPreviews = pNewItemPreviews;
}

// GetUnlicensedCaption will update the csCaption parameter with the appropriate unlicensed
// caption if the specified item is unlicensed
BOOL CBrowserDialog::GetUnlicensedCaption(CPMWCollection* pCollection, DWORD dwItemNumber, CString &csCaption)
{
	if (pCollection == NULL)
	{
		ASSERT(0);
		return TRUE;
	}

	if (pCollection->IsUserCollection())
		return TRUE;

	CString strProductCode = pCollection->GetProductCode(dwItemNumber);

	BOOL bIsLicensed = LicenseData.ProductIsLicensed(strProductCode);
	BOOL bFreeSample = FALSE;
	if(!bIsLicensed)
	{
		// Check if free sample
		CCategoryDataItem    cdItem;
		ERRORCODE errorcode = pCollection->GetCategoryDataItem(dwItemNumber,&cdItem);
		// Only check if Collection has new flags, 
		// Skip check for older collections
		if(errorcode == ERRORCODE_None &&
			pCollection->GetVersion() > CPMWCollection::verOriginal)
		{
			if(cdItem.GetCustomFlags() & CCategoryDataItem::cfFreeSample)
			{
				bIsLicensed = TRUE;  // It's free
				bFreeSample = TRUE;
			}
		}
	}

	if (!bIsLicensed || bFreeSample)
	{
		CString strFormat;
		CString csCollectionName = pCollection->GetFriendlyName();
		if (bFreeSample)
			strFormat.LoadString(IDS_BROWSER_FREE_SAMPLE);
		else
			strFormat.LoadString(IDS_BROWSER_UNLICENSED_CAPTION);
		ASSERT(!strFormat.IsEmpty());

		// Update callers caption string
		csCaption.Format(strFormat, csCollectionName.GetBuffer(1));
	}

	return bIsLicensed;
}

// Update the scroll bar to fit the current selections.
void CBrowserDialog::UpdateScrollBar(void)
{
   int nScrollMin = 0;
   int nScrollMax = 0;
   int nScrollPos = 0;
   int nDesiredScrollMax;

   // Get the number of search results.
   if (m_nResultItems > 0)
   {
      // Number of rows that can be scrolled off the top of the preview area
      // with the last full row still filled.
		m_nItemRows = CalcItemRows();

      nDesiredScrollMax = m_nItemRows - m_nFullThumbnailRows;
      if (nDesiredScrollMax < 0)
      {
         nDesiredScrollMax = 0;
      }
      if(nDesiredScrollMax > 32768)
      {
         m_nScrollInc = nDesiredScrollMax / 32768 + 1;
         nScrollMax = nDesiredScrollMax / m_nScrollInc;
      }
      else
      {
         m_nScrollInc = 1;
         nScrollMax = nDesiredScrollMax;
      }

      // Make sure 1st preview row is not negative.
      if (m_nFirstRow < nScrollMin)
         m_nFirstRow = nScrollMin;

      nScrollPos = m_nFirstRow / m_nScrollInc;
   }

   // Set new scroll values.
   if (nScrollMin < nScrollMax)
   {
      // Scrolling is enabled.
      EnableScrollBarCtrl(SB_VERT, TRUE);
      SetScrollRange(SB_VERT, nScrollMin, nScrollMax, FALSE);
      SetScrollPos(SB_VERT, nScrollPos);
   }
   else
   {
      // Turn off scrolling.
      SetScrollRange(SB_VERT, 0, 0, FALSE);
      SetScrollPos(SB_VERT, 0);
      EnableScrollBarCtrl(SB_VERT, FALSE);
   }
}

#define NO_DITHER_IN_TRUECOLOR

// Create a packed DIB from compressed thumbnail data.
void* CBrowserDialog::MakeThumbnail(USHORT uCollectionID, DWORD dwItemNumber,
												const CRect& crBounds, CThumbnailDataEntry& Thumbnail,
												void* pData, BOOL* pbTransparent)
{
   LPBITMAPINFO pbi = NULL;
	*pbTransparent = FALSE;		// set later, if transparency found

   CCompressInfo* pCompressInfo = Thumbnail.m_DataHeader.GetCompressInfo();
   if (pCompressInfo != NULL)
   {
      // Get dimensions of thumbnail data.
      int nThumbnailDataWidth = Thumbnail.m_DataHeader.GetWidth();
      int nThumbnailDataHeight = Thumbnail.m_DataHeader.GetHeight();
      int nImageBitsPerPixel = Thumbnail.m_DataHeader.GetBitsPerPixel();

      // Decompress the image data if we need to.
      void* pDecompressedData = NULL;
      BOOL fReduced24To8Bit = FALSE;
      switch (pCompressInfo->GetType())
      {
         case CThumbnailDataHeader::COMPRESSION_JPEG:
         {
            // We need to decompress.
            MemoryDevice Device;
            Device.AddPiece(pData, pCompressInfo->GetCompressedSize());

            // Allocate memory for the destination.
            pDecompressedData = Util::HugeAlloc(pCompressInfo->GetUnCompressedSize());

            // Use decompressed data.
            pData = pDecompressedData;
            if (pData != NULL)
            {
               CJPEGDecompressor Decompressor;
               if (Decompressor.Init(&Device) == ERRORCODE_None)
               {
                  Decompressor.ReadLines(0, (LPBYTE)pData, nThumbnailDataHeight);
                  Decompressor.Finish();
               }
            }
            break;
         }
         case CThumbnailDataHeader::COMPRESSION_ReducedDeflate:
         {
            fReduced24To8Bit = TRUE;
            // Fall through to...
         }
         case CThumbnailDataHeader::COMPRESSION_Deflate:
         {
            // Decompress the data.
            CDeflate Deflator;
            LPBYTE pNewData = NULL;
            DWORD dwNewData = 0;
            Deflator.Decompress((LPBYTE)pData, pCompressInfo->GetCompressedSize(), &pNewData, &dwNewData);
            ASSERT(dwNewData == (DWORD)pCompressInfo->GetUnCompressedSize());

            // Allocate memory for the destination.
            pDecompressedData = Util::HugeAlloc(dwNewData);

            // Use decompressed data.
            pData = pDecompressedData;

            Util::HMemCpy(pData, pNewData, dwNewData);
            Deflator.FreeData(pNewData);
            break;
         }
      }

      // If we have data, then it's now decompressed and ready to turn into a packed DIB.
      if (pData != NULL)
      {
         // Allocate the memory for the packed DIB.
         int nWidth;
         int nHeight;

         int   nDisplayColorDepth;
         int   nBytesPerPixel;      // Dest image bytes per pixel
         int   nHeaderBytes;        // Dest image header size
         BOOL  bMakeTrueColor;      // Dest image will be true color

         // Get DC of Display
         HDC   hDC = ::GetDC(NULL);
         if(hDC == NULL)
			{
				// If we allocated a decompression buffer, free it now.
				Util::HugeFree(pDecompressedData);
            return NULL;
			}

         nDisplayColorDepth = ::GetDeviceCaps(hDC, PLANES) *
            ::GetDeviceCaps(hDC, BITSPIXEL);
         ::ReleaseDC(NULL, hDC);

         // If display device is running in True Color mode and 
         // Source Image is 24 bit bit that has not been reduced to 8 color, 
         // make a True Color Image
         if(nDisplayColorDepth > 8 && !fReduced24To8Bit && nImageBitsPerPixel == 24)
            {
               bMakeTrueColor = TRUE;
               nBytesPerPixel = 3;
            }
         else
            {
               bMakeTrueColor = FALSE;
               nBytesPerPixel = 1;
            }

         // If Image is going to get dithered, final image size will
         // match requested bounds.

         // Image gets dithered when display mode is 8 bit and source image
         // is 24 bit or source image was originally a 24 bit image
         // reduced to 8 bit, and we're running in 8 bit mode.
         if ( (nImageBitsPerPixel == 24 && !bMakeTrueColor) ||
             (fReduced24To8Bit && nDisplayColorDepth <= 8) )
         {
            nWidth = crBounds.Width();
            nHeight = crBounds.Height();
         }
         else
         {
            nWidth = nThumbnailDataWidth;
            nHeight = nThumbnailDataHeight;
         }
         
         // If image has palette, there should be 1 byte/pixel
         // True color has 3 bytes/pixel
         int nByteWidth = ((nWidth * nBytesPerPixel) + 3) & ~3;

         DWORD dwDataBytes = (DWORD)nByteWidth * (DWORD)nHeight;

         if(bMakeTrueColor)
            nHeaderBytes = sizeof(BITMAPINFOHEADER);
         else
            nHeaderBytes = sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
         pbi = (LPBITMAPINFO) Util::HugeAlloc(nHeaderBytes + dwDataBytes);
         if (pbi != NULL)
         {
            // Initialize the DIB header.
            pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            pbi->bmiHeader.biWidth = (LONG)nWidth;
            pbi->bmiHeader.biHeight = (LONG)nHeight;
            pbi->bmiHeader.biPlanes = 1;
            pbi->bmiHeader.biBitCount = 8 * nBytesPerPixel;
            pbi->bmiHeader.biCompression = BI_RGB;
            pbi->bmiHeader.biSizeImage = dwDataBytes;
            pbi->bmiHeader.biXPelsPerMeter =
               pbi->bmiHeader.biYPelsPerMeter = 0;
            pbi->bmiHeader.biClrUsed =
               pbi->bmiHeader.biClrImportant = 0;

            // If making palettized DIB and we're in 8-bit display mode,
				// copy PrintMaster palette into the new DIBs color table.
            if (!bMakeTrueColor)
               memcpy(pbi->bmiColors, GetIdentityPalette(), 256*sizeof(RGBQUAD));

            LPBYTE pDIBData = (LPBYTE)pbi + nHeaderBytes;

            if (fReduced24To8Bit)
            {
					int nPaletteSize = *(WORD*)pData;
					COLORREF* pPalette = (COLORREF*)((LPBYTE)pData+sizeof(WORD));
					*pbTransparent = (nPaletteSize == 256 && pPalette[255] == (DWORD)-1);
#ifdef NO_DITHER_IN_TRUECOLOR
					// If display color is greater than 8-bit (and no transparency),
					// we want to keep the palette that's in the thumbnail
					// and copy the image data straight over.
					if (nDisplayColorDepth > 8 && *pbTransparent == FALSE)
					{
						// Copy the image's original palette into the DIB's palette.
						memcpy(pbi->bmiColors, pPalette, nPaletteSize*sizeof(RGBQUAD));

						// Get a pointer into beginning of image data
						LPBYTE pBmpData = (LPBYTE)pData+sizeof(WORD)+nPaletteSize*sizeof(DWORD);
						memcpy(pDIBData, pBmpData, dwDataBytes);
					}
					else
#endif // NO_DITHER_IN_TRUECOLOR
					{
						// The display is 8-bit, or we have an image with transparency.

						// Dither the image data into an 8-bit DIB, using PrintMaster palette.
						Dither8To8(
							(LPBYTE)pData+sizeof(WORD)+nPaletteSize*sizeof(DWORD),
							nThumbnailDataWidth,
							nThumbnailDataHeight,
							pDIBData,
							nWidth,
							nHeight,
							(RGBQUAD*)pPalette,
							*pbTransparent);
					}
            }
            // If source image is 24 bit, but we cannot make it true color
            else if ( nImageBitsPerPixel == 24 && !bMakeTrueColor)
            {
               int nPaletteSize = *(WORD*)pData;
               Dither24To8(
                  (LPBYTE)pData,
                  nThumbnailDataWidth,
                  nThumbnailDataHeight,
                  pDIBData,
                  nWidth,
                  nHeight,
                  TRUE);
            }
            else
            {
               // Use Source Image Data as is
               Util::HMemCpy(pDIBData, pData, dwDataBytes);
            }
         }
      }

      // If we allocated a decompression buffer, free it now.
      Util::HugeFree(pDecompressedData);
      pDecompressedData = NULL;
   }

   // Return a pointer to the packed DIB.
   return pbi;
}

// Select an item. Only one item may be selected at a time.
void CBrowserDialog::SelectItem(USHORT uCollectionID, DWORD dwItemNumber)
{
	if (m_pItemPreviews == NULL)
		return;

   // Redraw the old selection to remove selection border.
   if (m_pItemPreviews->GetSize() > 1)
      InvalidateItem(m_uSelectedCollectionID, m_dwSelectedItemNumber);

   // Set new selection number.
   m_uSelectedCollectionID = uCollectionID;
   m_dwSelectedItemNumber = dwItemNumber;
   m_nCurPreviewItem = m_pItemPreviews->Find(uCollectionID, dwItemNumber);

	// Redraw the new selection with selection border.
   if (m_pItemPreviews->GetSize() > 1)
      InvalidateItem(m_uSelectedCollectionID, m_dwSelectedItemNumber);
}

int CBrowserDialog::GotoItem(USHORT nID, DWORD dwItemNumber, BOOL fRefresh/*=TRUE*/)
{
   int nResultIndex = m_SearchContext.Find(nID, dwItemNumber);
   if(nResultIndex < 0)
      return ERRORCODE_Fail;

// This code was here, but it is not used!
// CPMWCollectionResult Result = m_SearchContext.GetSearchResult(nResultIndex);

	int nOldFirstRow = m_nFirstRow;

   if (m_fHasGroups)
	{
		m_nFirstRow = CalcRowFromItemNumber(nResultIndex);
	}
	else
	{
		m_nFirstRow = nResultIndex / m_ThumbnailLayout.cx;
	   int nRemainder = ((nResultIndex) % (m_ThumbnailLayout.cx * m_nFullThumbnailRows));
	   if(nResultIndex > 0)
	      m_nFirstRow -=  nRemainder / m_ThumbnailLayout.cx;
	}

	m_nFirstRow = __min(m_nFirstRow, m_nItemRows - m_nFullThumbnailRows);

	if (fRefresh || m_nFirstRow != nOldFirstRow)
	{
		UpdateScrollBar();
		UpdateItemPreviews(FALSE);
		InvalidateRect(m_crPreview);
	}

   SelectItem(nID, dwItemNumber);
	if (m_nCurPreviewItem >= 0)
	{
		CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_nCurPreviewItem);
		m_nCurColumn = CalcColumnFromItem(pItem);
	}

   return ERRORCODE_None;
}

// Invalidate an item preview to force it to redraw.
void CBrowserDialog::InvalidateItem(USHORT uCollectionID, DWORD dwItemNumber)
{
   if (m_pItemPreviews != NULL)
   {
      // Get the item preview descriptor.
      int nItem = m_pItemPreviews->Find(m_uSelectedCollectionID, m_dwSelectedItemNumber);
      if (nItem != -1)
      {
         CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(nItem);
         if (IsItemOKToView(pItem))
         {
            // Invalidate the item preview.
            InvalidateRect(pItem->m_crFocusArea);

				CRect crUpdate;
            GetUpdateRect(crUpdate);
            CRect crOverlap;
            if(!crOverlap.IntersectRect(crUpdate, pItem->m_crFocusArea) &&
					m_thumbsize != thumbsizeLarge)
               ValidateRect(pItem->m_crThumbnail);
         }
      }
   }
}

//
// The user has clicked on an unlicensed item.
// Put up a dialog with some choices for the user.
//

void CBrowserDialog::OnClickedUnlicensed(void)
{
	CMlsStringArray   csaCategory;

	// Extract the product code for the current item.
	// (This was known at click time... can we save it then?)
	CPMWCollection *pCollection = m_pCollectionManager->FindCollection(m_uSelectedCollectionID, CollectionType(), CollectionSubType());
	ASSERT(pCollection);
	if (pCollection == NULL || pCollection->IsUserCollection())
	{
		// How can this happen?
		ASSERT(FALSE);
		return;
	}
	CString csProductCode = pCollection->GetProductCode(m_dwSelectedItemNumber);

	// Put up the dialog allowing them to go to the order form or view the set or cancel.

	CUnlicensedDialog Dialog;
	switch (Dialog.DoModal())
	{
		case IDC_ORDER_FORM:
		{
			// Do the licensing.
			DoLicense(pCollection, &csProductCode);
			break;
		}
		case IDC_VIEW_SET:
		{
			if (CollectionType() == CPMWCollection::typeArt ||
				 CollectionType() == CPMWCollection::typeProjects)
			{
				CString csCollection = pCollection->GetFriendlyName();
				CString csSuperColl = pCollection->GetSuperCollection();
				if (csSuperColl)
				{
					csSuperColl += m_pCollectionList->GetDelimiter() + csCollection;
					csCollection = csSuperColl;
				}
				m_pCollectionList->SetCurSel(csCollection);

				if (CollectionType() == CPMWCollection::typeArt)
				{
					// for art, set the specific category
					CString csCategory = pCollection->GetCategory(m_dwSelectedItemNumber);
					if (m_pCategoryList)
						m_pCategoryList->SetCurSel(csCategory);
				}
				else
				{
					// for projects, set project type and category to "All"
					m_pProjectTypeList->SetCurSel(BrowserAllString());
					m_pCategoryList->SetCurSel(BrowserAllString());

#ifdef WANT_TO_SET_CATEGORY
					csaCategory.SetNames(csCategory, m_pCategoryList->GetDelimiter());
					ASSERT(csaCategory.GetSize() == 3);
					CString csProject = csaCategory[0];
					csCategory = csaCategory[1] + m_pCategoryList->GetDelimiter();
					csCategory += csaCategory[2];
					m_pProjectTypeList->SetCurSel(csProject);
					m_pCategoryList->SetCurSel(csCategory);
#endif
				}

				// clear keywords
				CComboBox* pKeywordList = (CComboBox*)GetDlgItem(IDC_KEYWORD_LIST);
				if (pKeywordList != NULL)
				{
					pKeywordList->SetCurSel(0);
					KeywordChanged(0);
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void CBrowserDialog::DoLicense(CPMWCollection* pCollection, const CString* pcsProductCode /*=NULL*/)
{
   if (pCollection != m_pCurrentLicenseCollection)
   {
      // We have an existing order.
      if (!FinishOrder())
      {
         return;
      }
      delete m_pLicenseDll;
      m_pLicenseDll = NULL;
   }

	BOOL fRefresh = FALSE;

	//
	// See if this is an internet order or not.
	//
	CIniFile OrdFile(pCollection->GetOrderFileName());
	if (OrdFile.GetInteger("Artstore", "IsInternet", 0))
	{
		if (m_pContentManager != NULL)
		{
			m_pContentManager->LicenseProducts(pCollection->GetBaseName());
			fRefresh = TRUE;
		}
	}
	else
	{
		// Non-online order.
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
	#if 1
				csLicenseDllPath = m_pPathManager->LocatePath("[[H]]\\MSREG");
	#else
				csLicenseDllPath = m_pPathManager->GetWorkingDirectory();
	#endif


				// Construct the fully-qualified filename of the LICENSE.DAT file.
				CString csLicenseDat = LicenseData.FileName();

				// Construct the path to the order file.
				CString csOrderFile = pCollection->GetOrderFileName();

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
			m_pCurrentLicenseCollection = pCollection;
			m_pLicenseDll->DoLicense(pcsProductCode);
			fRefresh = TRUE;
		}
	}

	if (fRefresh)
	{
   	LicenseData.Update();
      // Update Preview Info with new licence info.
      UpdateItemPreviews(TRUE);
      // Redraw the preview area.
      InvalidateRect(m_crPreview);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserDialog message handlers

BOOL CBrowserDialog::OnOrderForm(UINT nID)
{
   // Get the collection for this menu ID.

   int nIndex = nID-IDC_BROWSER_ORDER_FORM_1;

   if (nIndex >= 0 && nIndex < m_cpaOrderFormCollections.GetSize())
   {
      CPMWCollection* pCollection = (CPMWCollection*)m_cpaOrderFormCollections.GetAt(nIndex);
      DoLicense(pCollection);
   }

   return TRUE;
}

void CBrowserDialog::OnUpdateOrderForm(CCmdUI *pCmdUI)
{
   BOOL fCheck = FALSE;
   int nIndex = pCmdUI->m_nID-IDC_BROWSER_ORDER_FORM_1;
   if (nIndex >= 0 && nIndex < m_cpaOrderFormCollections.GetSize())
   {
      CPMWCollection* pCollection = (CPMWCollection*)m_cpaOrderFormCollections.GetAt(nIndex);
      fCheck = (pCollection == m_pCurrentLicenseCollection);
   }
   pCmdUI->SetCheck(fCheck);
}

void CBrowserDialog::OnOK()
{
   // Get the control with the focus.
   CWnd* pFocus = CWnd::GetFocus();

   // If the focused control is the collection, graphic type, category, tone,
   // combo box, then don't shut down the dialog.
   if ((pFocus == m_pCollectionList)
	 || (pFocus == m_pGraphicTypeList)
    || (pFocus == m_pCategoryList)
    || (pFocus == m_pToneList))
   {
      // Do not close the dialog.
      return;
   }

   // If the keyword list edit has the focus then select the edit control
   // text and do a search.
   CWnd* pKeywordList = GetDlgItem(IDC_KEYWORD_LIST);
   if (pKeywordList != NULL)
   {
      CEdit* pKeywordEdit = (CEdit*)(pKeywordList->GetDlgItem(1001));
      if (pFocus == pKeywordEdit)
      {
         // Select the text so the user can easily enter another keyword.
         pKeywordEdit->SetSel(0, -1);

         // Do whatever we do when the keyword changes.
         KeywordChanged(-1);

         // Do not close the dialog.
         return;
      }
   }

   // Call inherited OnOK() to shut down dialog.
   INHERITED::OnOK();
}

static UINT BASED_CODE uProjectIndicatorIDs[] =
{
   ID_SEPARATOR,        // status line indicator
   ID_BROWSER_INDICATOR_VIEW, 
   ID_BROWSER_INDICATOR_SIZE, 
   ID_BROWSER_INDICATOR_ITEM, 
//   ID_BROWSER_INDICATOR_PAGE
};

static UINT BASED_CODE uArtIndicatorIDs[] =
{
   ID_SEPARATOR,        // status line indicator
   ID_BROWSER_INDICATOR_SIZE, 
   ID_BROWSER_INDICATOR_ITEM, 
//   ID_BROWSER_INDICATOR_PAGE
};

static UINT BASED_CODE uSentimentIndicatorIDs[] =
{
   ID_SEPARATOR,        // status line indicator
   ID_BROWSER_INDICATOR_ITEM
};

int CBrowserDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// Setup our interfaces. These could come from the constructor someday.
	m_pPathManager = GetGlobalPathManager();
	m_pPathManager->AddRef();

	m_pCollectionManager = GetGlobalCollectionManager();
	m_pCollectionManager->AddRef();

	m_pContentManager = GetGlobalContentManager();
	m_pContentManager->AddRef();

	BOOL fNew = m_pContentManager->CheckForNewInternetContent();

	// See if there is any content to show.
   int nCollections = m_pCollectionManager->NumberOfCollections(CollectionType(), CollectionSubType());
   if(nCollections <= 0)
      {
         CString  strError;
         strError.LoadString(IDS_BROWSER_CONTENT_NOTFOUND);
         AfxMessageBox(strError);
         return -1;
      }

   if (CPmwDialogColor::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   return 0;
}

BOOL CBrowserDialog::CanClose(void)
{
   return FinishOrder();
}

BOOL CBrowserDialog::FinishOrder(void)
{
   if (m_pLicenseDll != NULL)
   {
      if (!m_pLicenseDll->CanClose())
      {
         m_pLicenseDll->DoLicense();
         return FALSE;
      }
   }
   return TRUE;
}

void CBrowserDialog::OnClose() 
{
   if (CanClose())
   {
      INHERITED::OnClose();
   }
}

void CBrowserDialog::OnDestroy()
{
   INHERITED::OnClose();

	// Close all streams in any collections of this type.
	// This is kind of a hack.
	int nType = CollectionType();
	int nCollections = m_pCollectionManager->NumberOfCollections(nType, CPMWCollection::subTypeNone);
	for (int nCollection = 0; nCollection < nCollections; nCollection++)
	{
		CPMWCollection* pCollection = m_pCollectionManager->GetCollection(nCollection, nType, CPMWCollection::subTypeNone);
		pCollection->CloseAllStreams(TRUE);
	}

	int nSubType = CollectionSubType();
	if (nSubType != CPMWCollection::subTypeNone)
	{
		int nCollections = m_pCollectionManager->NumberOfCollections(nType, nSubType);
		for (int nCollection = 0; nCollection < nCollections; nCollection++)
		{
			CPMWCollection* pCollection = m_pCollectionManager->GetCollection(nCollection, nType, nSubType);
			pCollection->CloseAllStreams(TRUE);
		}
	}

	INHERITED::OnDestroy();
}

void CBrowserDialog::SizeBrowserWindow() 
{
	//Set the default window size to a percentage of the screen size
	//This just makes the default larger for high resoulution taking advantage
	//of the extra real estate.
   CIniFile    IniFile(GET_PMWAPP()->m_pszProfileName);
	if(!IniFile.GetInteger(m_csINISection, ENTRY_Valid, FALSE))
	{
		CRect rcWindow;
		GetWindowRect(&rcWindow);
		int nScreenXSize = GetSystemMetrics(SM_CXSCREEN);
		int nScreenYSize = GetSystemMetrics(SM_CYSCREEN);
		rcWindow.InflateRect((int)((nScreenXSize * dPercentView - rcWindow.Width()) / 2), 
									(int)((nScreenYSize * dPercentView - rcWindow.Height()) / 2));
		MoveWindow(rcWindow);
	}

}

BOOL CBrowserDialog::OnInitDialog() 
{
   BeginWaitCursor();

	// Initialize the collection draw.
	m_CollectionDraw.Init(GetPathManager());

	SizeBrowserWindow();

   // Ignore any combo box selection changes while we're
   // setting up the initial state of the interface.
   m_fIgnoreSelectionChange = TRUE;

   INHERITED::OnInitDialog();

   if (GetConfiguration()->IsAddOn () == TRUE)
   {
      CWnd *pSelectButton = GetDlgItem(IDC_BROWSER_SELECT);
      if(pSelectButton != NULL)
         pSelectButton->DestroyWindow();
   }

   LoadAccelTable(MAKEINTRESOURCE(IDR_PROJECT_BROWSER_MENU));

   m_csKeywordNone.LoadString(IDS_BROWSER_KEYWORD_NONE);

	CreateStatusBar();

   // Create the font that will be used to draw the caption text.
   // We select a font height that will give us room for two lines
   // of text if we need it.
   LOGFONT lfFont;
   lfFont.lfHeight = 14; //m_nCaptionHeight/2;
   lfFont.lfWidth = 0;
   lfFont.lfEscapement = 0;
   lfFont.lfOrientation = 0;
   lfFont.lfWeight = 400;
   lfFont.lfItalic = 0;
   lfFont.lfUnderline = 0;
   lfFont.lfStrikeOut = 0;
   lfFont.lfCharSet = ANSI_CHARSET;
   lfFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
   lfFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
   lfFont.lfQuality = DEFAULT_QUALITY;
   lfFont.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
   strcpy(lfFont.lfFaceName, "Arial");
   m_pPreviewFont = new CFont;
   if (!m_pPreviewFont->CreateFontIndirect(&lfFont))
   {
      // bad news!
      delete m_pPreviewFont;
      m_pPreviewFont = NULL;
   }

	// create the edit control used for in-place editing of user titles
	m_pEditTitle = new CEditUserTitle;
	if (m_pEditTitle)
	{
		m_pEditTitle->Create(WS_BORDER|ES_AUTOVSCROLL|ES_CENTER|ES_MULTILINE|ES_WANTRETURN,
									CRect(1,1,1,1), this, IDC_EDIT_TITLE);
		if (m_pPreviewFont)
			m_pEditTitle->SetFont(m_pPreviewFont);
		m_pEditTitle->	LimitText(MAX_FRIENDLY_NAME_LEN);
	}

   // Replace normal combo boxes with tree combo boxes.
   CreateTreeComboBox(IDC_COLLECTION, m_pCollectionList, NULL);
	if (m_pCollectionList)
	{
		// we don't want to see the super collection name
		m_pCollectionList->SetShowCurLevelOnly(TRUE);
	}

   CreateTreeComboBox(IDC_GRAPHIC_TYPE, m_pGraphicTypeList, GraphicTypeTable);	// ps art browser only
   CreateTreeComboBox(IDC_PROJECT_TYPE, m_pProjectTypeList, NULL);	// projects only
   CreateTreeComboBox(IDC_CATEGORY, m_pCategoryList, NULL);
   CreateTreeComboBox(IDC_TONE, m_pToneList, ToneTable);

   // Stuff an empty string into the combo box.
   CComboBox* pKeywordList = (CComboBox*)GetDlgItem(IDC_KEYWORD_LIST);
   if (pKeywordList != NULL)
      pKeywordList->InsertString(0, m_csKeywordNone);

   // Create the Order Forms menu item if needed.
   HandleOrderForms();

   // Iterate through the known collections and add them to the
   // collection combo box.
   ProcessCollections();

	// Handle the "special collection" buttons. (AKA American Greetings, Online Art, Master Art)
	HandleSpecialCollectionButtons();

   // Figure out where things should go.
   ComputeLayout();
   RestoreState();

	PositionControls();

   // Ok, now we'll pay attention to combo box changes.
   m_fIgnoreSelectionChange = FALSE;
#if 0 // GCB 2/26/99
	UpdateResultSet();
#endif

   // Ok, now we'll allow previews updates.
   m_fUpdatePreview = TRUE;

   UpdatePreview(FALSE);   // Don't select preview item

   // The dialog is initially hidden so we don't see all the control
   // stuff happening. We're now ready to reveal our glorious self to
   // the world.
   if (m_fIsMaximized)
      ShowWindow(SW_SHOWMAXIMIZED);
   else
      ShowWindow(SW_SHOW);

   EndWaitCursor();

   return TRUE;  // return TRUE unless you set the focus to a control
}
	 
void CBrowserDialog::CreateStatusBar()
{
	BOOL bCreated = m_StatusBar.Create(this);
   if(bCreated)
   {
		// we've got to do some weird stuff to get the status bar to
		// show up on a dialog...
      CRect clientRect;
      GetClientRect(&clientRect);
      AFX_SIZEPARENTPARAMS layout;
      layout.rect = clientRect;
      layout.hDWP = ::BeginDeferWindowPos(1);
      ::SendMessage(m_StatusBar.m_hWnd, WM_SIZEPARENT, 0,  (LPARAM)&layout);
      ::EndDeferWindowPos(layout.hDWP);
   }

}

BOOL			 
CBrowserDialog::GetArtButtonRect(CRect &r, int resID)
{
	if (*(&r) != NULL)
	{
		r.SetRectEmpty();
		// If the button is not to be shown, return empty rect
		if (ShowArtButton() == TRUE)
		{
			CBitmap Bitmap;
			if (Bitmap.LoadBitmap(resID))
			{
				BITMAP BitmapInfo;
				if (Bitmap.GetObject(sizeof(BitmapInfo), &BitmapInfo) != 0)
				{
					r.SetRect(0, 0, BitmapInfo.bmWidth, BitmapInfo.bmHeight);
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}			 
// Handle the "special collection" buttons that will whisk the user to
// a particular collection. As of this writing, these buttons are for
// American Greetings (disabled for PrintMaster 8 in msreg.ini) and Online Art.
void CBrowserDialog::HandleSpecialCollectionButtons(void)
{
	// Check if we have to make a button at all. This is determined
	// by the "SpecialCollection" key in MSREG.INI. If no such key
	// exists, then we don't have to make a button. If the key does
	// exist and we have a collection with the specified name, then
	// we make the button.

	// Of course we need a place to put the button. This is marked
	// in the dialog template by a control with the id ID_SPECIAL_COLLECTION.

	CWnd* pDummy = GetDlgItem(ID_SPECIAL_COLLECTION);
	if (pDummy != NULL)
	{
		// Get the position of the placeholder in browser client coordinates.
		CRect crDummy;
		pDummy->GetWindowRect(crDummy);
		ScreenToClient(crDummy);
		// Compute the point on which to center the special collection button.
		int nXCenter = (crDummy.left+crDummy.right)/2;
		int nYCenter = (crDummy.top+crDummy.bottom)/2;

		CRect	ArtMoreButtonRect;
		GetArtButtonRect(ArtMoreButtonRect, IDB_ONLINE_ART_UP);

		// Destroy the dummy window.
		pDummy->DestroyWindow();
		pDummy = NULL;
		CRect crButton(0, 0, 0, 0);
		// Check if we have a "special" collection.
		CString csMsregIni = m_pPathManager->ExpandPath("MSREG.INI");
		CIniFile MsregIniFile(csMsregIni);
		m_csSpecialCollection = MsregIniFile.GetString(SECTION_Configuration, "SpecialCollection");
		if (!m_csSpecialCollection.IsEmpty())
		{
			// Check if the special collection is in our collection list.
			if (m_pCollectionList != NULL)
			{
				if (m_pCollectionList->FindStringExact(-1, m_csSpecialCollection) >= 0)
				{
					// Wow, the collection exists. So, we have to create the button (bummer.)
					// By convention, the button will have two states (up and down) with the
					// resource ids IDB_SPECIAL_COLLECTION_UP and IDB_SPECIAL_COLLECTION_DOWN.
					// A CNonRectButton window is used, so the buttons can be be any shape and
					// have transparent areas as long as the transparent areas are exactly the
					// same in the up and down states (a limitation of CNonRectButton.)
					TRY
					{
						m_pSpecialCollectionButton = new CNonRectButton;

						if (m_pSpecialCollectionButton != NULL)
						{
							// Create the window the size of the "up" bitmap. By convention,
							// the "down" bitmap should be the same size.
							CBitmap Bitmap;
							if (Bitmap.LoadBitmap(IDB_SPECIAL_COLLECTION_UP))
							{
								BITMAP BitmapInfo;
								if (Bitmap.GetObject(sizeof(BitmapInfo), &BitmapInfo) != 0)
								{
									crButton.SetRect(0, 0, BitmapInfo.bmWidth, BitmapInfo.bmHeight);
									int	x; 
									// Do we want an online art button?
									if (!ArtMoreButtonRect.IsRectEmpty())
									{
										x = nXCenter / 2;
										x -= crButton.Width() / 2;				// Yeah... make room for it
									}
									else
										x = nXCenter - crButton.Width() / 2;
									crButton.OffsetRect(x, nYCenter-crButton.Height()/2);
									m_pSpecialCollectionButton->Create(crButton, this, ID_SPECIAL_COLLECTION);
									m_pSpecialCollectionButton->SetBitmaps(MAKEINTRESOURCE(IDB_SPECIAL_COLLECTION_UP), MAKEINTRESOURCE(IDB_SPECIAL_COLLECTION_DOWN), pOurPal);
									// By convention, the transparent color is cyan.
									m_pSpecialCollectionButton->SetTransparentColor(RGB(0,255,255));
								}
							}
						}
					}
					END_TRY
				}
			}
		}
		// Do we want this button?
		if (!ArtMoreButtonRect.IsRectEmpty())
		{
			int x;
			// If there is a special collection button, move over
			if (m_pSpecialCollectionButton != NULL)
			{
				x = nXCenter + (nXCenter / 2);
				x -= ArtMoreButtonRect.Width() / 2;
			}
			else
				x = nXCenter - ArtMoreButtonRect.Width() / 2;
			ArtMoreButtonRect.OffsetRect(x, nYCenter - ArtMoreButtonRect.Height() / 2);

			TRY
			{
				if(GetConfiguration()->SupportsOnlineGallery())
				{
					m_pOnlineArtButton = new CNonRectButton;
					m_pOnlineArtButton->Create(ArtMoreButtonRect, this, ID_ONLINE_ART);
					if (GetGlobalContentManager()->CheckForNewInternetContent() == TRUE)
						m_pOnlineArtButton->SetBitmaps(MAKEINTRESOURCE(IDB_HUB_NEWANDMORE_UP), MAKEINTRESOURCE(IDB_HUB_NEWANDMORE_DOWN), pOurPal);
					else
						m_pOnlineArtButton->SetBitmaps(MAKEINTRESOURCE(IDB_ONLINE_ART_UP), MAKEINTRESOURCE(IDB_ONLINE_ART_DOWN), pOurPal);
					// By convention, the transparent color is cyan.
					m_pOnlineArtButton->SetTransparentColor(RGB(0,255,255));
				}
			}
			END_TRY
		}
   }
}

//
// Create the Order Forms menu item if needed.
//

void CBrowserDialog::HandleOrderForms(void)
{
   m_cpaOrderFormCollections.RemoveAll();
   // First, determine whether we have any order forms or not.

   // Get the number of collections.
   int nCollections = m_pCollectionManager->NumberOfCollections(CollectionType(), CollectionSubType());

   BOOL fHasOrderForm = FALSE;
   // Process each collection by calling ProcessCollection().
   int nCollection;
   for (nCollection = 0; nCollection < nCollections; nCollection++)
   {
      CPMWCollection* pCollection = m_pCollectionManager->GetCollection(nCollection, CollectionType(), CollectionSubType());
      if (pCollection != NULL && pCollection->HasOrderForm())
      {
         break;
      }
   }

   // If we did not finish the above loop, then we have an order form,
   // and nCollection was left pointing at the first one.
   if (nCollection < nCollections)
   {
      CMenu* pMenu = GetMenu();
      if (pMenu != NULL)
      {
         // Create a new menu to insert.
         CMenu NewMenu;
         if (NewMenu.CreateMenu())
         {
            int nIndex = 0;
            // Fill out the menu with the menu items.
            while (nCollection < nCollections)
            {
               CPMWCollection* pCollection = m_pCollectionManager->GetCollection(nCollection, CollectionType(), CollectionSubType());
               if (pCollection != NULL && pCollection->HasOrderForm())
               {
                  // Add it to the menu.
                  NewMenu.InsertMenu((UINT)-1,
                                     MF_BYPOSITION|MF_STRING,
                                     (UINT)nIndex+IDC_BROWSER_ORDER_FORM_1,
                                     pCollection->GetFriendlyName());
                  m_cpaOrderFormCollections.Add(pCollection);
                  nIndex++;
               }
               nCollection++;
            }

            // We want to add a "Order Forms" menu (or whatever it localizes to).
            pMenu->InsertMenu((UINT)-1,
                              MF_BYPOSITION|MF_POPUP,
                              (UINT)NewMenu.Detach(),
                              GET_PMWAPP()->GetResourceStringPointer(IDS_OrderFormMenu));
         }
      }
   }
}

BOOL CBrowserDialog::SetIndicatorSize(int nIndex)
{
	int         nWidth, nNewWidth;
	UINT        uID, uStyle, uNewStyle = SBPS_NORMAL;

	ASSERT(nIndex >= 0);
	if(nIndex < 0)
		return FALSE;

	uID = m_StatusBar.GetItemID(nIndex);
	ASSERT(nIndex+1 <= m_StatusBar.GetCount());
	if(nIndex+1 > m_StatusBar.GetCount())
		return FALSE;

	m_StatusBar.GetPaneInfo(nIndex, uID, uStyle, nWidth);
	switch(uID)
	{
		case ID_BROWSER_INDICATOR_SIZE:
			nNewWidth = 55;
			break;
		case ID_BROWSER_INDICATOR_VIEW:
			nNewWidth = 80;
			break;
		case ID_BROWSER_INDICATOR_ITEM:
		case ID_BROWSER_INDICATOR_PAGE:
			nNewWidth = 120;
			break;
		case  ID_SEPARATOR:              // status line indicator
			nNewWidth = 1;
			uNewStyle = uStyle;
			break;
		default:
			// ID not defined
			ASSERT(0);
			break;
	}
	m_StatusBar.SetPaneInfo(nIndex, uID, uNewStyle, nNewWidth);
	return TRUE;
}

void CBrowserDialog::InvalidatePreview()
{
   InvalidateRect(m_crPreview);
}

void CBrowserDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	PaintBkgnd(&dc);
   PaintPreview(&dc);
}

///////////////////////////////////////////////////////////////////
// PaintPreview() -- paint the Preview area of the Browser
///////////////////////////////////////////////////////////////////
void CBrowserDialog::PaintPreview(CDC* pDC)
{
   if (m_thumbsize == thumbsizeLarge)
   {
      //
      // Drawing of large image is deferred until Idle time.
      //
      CBoolSemaphore Semaphore(m_fRefreshSemaphore);
      if (!Semaphore.Lock())
      {
         // Semaphore is locked. We must leave now.
         // We are not allowed to add a refresh while refreshing!
         return;
      }

      // Create a Refresh Extent, and add it to Refresh Array
      RECT clip;
      PBOX extent;
      REFRESH_EXTENT re;

      /* Save the clipping box in the extent. */
      pDC->GetClipBox(&clip);
      extent.x0 = clip.left;
      extent.y0 = clip.top;
      extent.x1 = clip.right;
      extent.y1 = clip.bottom;

	   re.extent = extent;
	   re.refresh_type = REFRESH_ALL;
	   re.my_object = NULL;
	   re.update_object = NULL;
	   re.update_state = NULL;

	   m_RefreshArray.Add(&re);
   }
   else
   {
      // small and medium thumbnails are drawn immediately
      DrawThumbnails(pDC);
   }
}

void CBrowserDialog::DrawThumbnails(CDC* pDC)
{
	// Get the current clip box so we only draw what we need to.
	CRect crClip(0,0,0,0);
	if (pDC->GetClipBox(crClip) == NULLREGION)
		return;  // nothing to do

	CRect crPreviewClip;
	if (!crPreviewClip.IntersectRect(crClip, m_crPreview))
		return;  // nothing to do

	CRect crOverlap;
	if(crOverlap.IntersectRect(crPreviewClip, m_crPreview))
	{
		// See if we are already drawing. If so, try again later.
		static BOOL fDrawing = FALSE;
		CBoolSemaphore Sem(fDrawing);
		if (!Sem.Lock())
		{
			TRACE("CBrowserDialog: Busy drawing!\n");
			InvalidateRect(crOverlap);
		}
		else if (m_pItemPreviews != NULL && m_pItemPreviews->GetSize() > 0)
		{
			// Draw the item previews...

			// Use our global PrintMaster palette.
			CPalette* pOldPalette = pDC->SelectPalette(pOurPal, FALSE);
			pDC->RealizePalette();

			// Set DC
			int nOldStretchBltMode = pDC->SetStretchBltMode(STRETCH_DELETESCANS);

			CString strLastProjectType = "";	// used for Project Headings

			for (int nItem = 0; nItem < m_pItemPreviews->GetSize(); nItem++)
			{
				CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(nItem);
				if (pItem == NULL)
					continue;

				// check if we need to display group headings in the preview area.
				if (m_fShowHeadings)
				{
					CPMWCollection* pCollection;
					pCollection = m_pCollectionManager->FindCollection(pItem->m_uCollectionID, CollectionType(), CollectionSubType());
					if (pCollection)
					{
						CString strProjType = pCollection->GetCategory(pItem->m_dwItemNumber);
						int index = strProjType.Find("/");
						if (index != -1)
							strProjType = strProjType.Left(index);

						if (strLastProjectType.CompareNoCase(strProjType) != 0)
						{
							// start of new Project Type group...

							// If this is the first item, see if there are more items
							// in this group above the preview area we're showing, and
							// if so, display "continued" after the project type
							BOOL fContinue = FALSE;
							if (nItem == 0)
							{
								long nSearch = m_SearchContext.Find(pItem->m_uCollectionID,
																				pItem->m_dwItemNumber);
								if (nSearch > 0)
								{
									if (!m_fHasGroups)
									{
										if (nSearch + 1 > m_ThumbnailLayout.cx)
											fContinue = TRUE;
									}
									else
									{
										int cnt = 0;
										for (int group = 0; group < m_ItemGroupInfo.GetSize(); group++)
										{
											if (m_ItemGroupInfo[group]->strGroup == strProjType)
												break;
											cnt += m_ItemGroupInfo[group]->nCount;
										}
										if (cnt + 1 < nSearch)
											fContinue = TRUE;
									}
								}
							}

							strLastProjectType = strProjType;
							int position = (pItem->m_crFocusArea.top+pItem->m_crFocusArea.bottom) / 2;
							position -= (m_ThumbnailSize.cy + m_ThumbnailMargin.cy) / 2;

							DisplayGroupHeading(pDC, strProjType, position, fContinue);
						}
					}
				}

				// Only draw thumbnail if in clip area
				CRect crThumb, crThumbClip;
				if (crThumbClip.IntersectRect(crPreviewClip, pItem->m_crFocusArea))
				{
					DrawThumbnailPreview(*pDC, pItem);
				}
			} // end for

			// Restore the previous DC settings.
			pDC->SetStretchBltMode(nOldStretchBltMode);

			if (pOldPalette)
				pDC->SelectPalette(pOldPalette, FALSE);
		}
	}
}

// display a heading in the preview area, with the BOTTOM of the heading at nYPos
void CBrowserDialog::DisplayGroupHeading(CDC* pDC, CString heading, int nYPos, BOOL fContinued/*=FALSE*/)
{
	CRect rcFullBar(m_crPreview.left + m_PreviewMargin.cx,
						 nYPos - m_nHeadingBarHeight,
						 m_crPreview.right - m_PreviewMargin.cx,
						 nYPos);

	COLORREF clBox = PALETTERGB(134, 134, 134);
	COLORREF clTextBox = PALETTERGB(215, 215, 215);

#ifdef LOCALIZE
	LPSTR headstr;
	headstr = heading.GetBuffer(heading.GetLength()*2); // allow extra space because in some languages (e.g. German)
	   							   // upper case strings are not necessarily the same length as lower case strings
    CharUpper(headstr);
	heading.ReleaseBuffer(-1);
#else	
	heading.MakeUpper();		// Group Heading is Uppercase
#endif

	CString strHeading = heading;
	if (fContinued)
	{
		CString strContinued;
		strContinued.LoadString(IDS_PROJTYPE_CONTINUED);
		strHeading.Format(strContinued, heading);
	}

	// Create font to use
	CFont* pOldFont = NULL;
	CFont Font;
	if (m_pPreviewFont != NULL)
	{
		LOGFONT lfFont;
		if (m_pPreviewFont->GetObject(sizeof(lfFont), &lfFont) > 0)
		{
			strcpy(lfFont.lfFaceName, "System");
			lfFont.lfHeight = 16;

			if (Font.CreateFontIndirect(&lfFont))
				pOldFont = pDC->SelectObject(&Font);
		}
	}

	// calculate text box size
	CRect rcText = rcFullBar;
	pDC->DrawText(strHeading, rcText, DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT);
	rcText.right += 20;

#if 0
	Util::Draw3dBorder(
							 pDC,
							 rcFullBar,
							 GetSysColor(COLOR_BTNHILIGHT),
							 GetSysColor(COLOR_BTNSHADOW),
							 GetSysColor(COLOR_BTNFACE),
							 GetSysColor(COLOR_BTNTEXT),
							 GetSysColor(COLOR_WINDOW),
							 1,
							 TRUE,
							 TRUE,
							 FALSE);
#endif

	// draw solid box next to text
//	CRect rcRightBar = rcFullBar;
//	rcRightBar.left = rcText.right;

	rcFullBar.right = __max(rcFullBar.right, rcText.right + 10);

	CPen* pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	if (pOldPen != NULL)
	{
		CBrush brush(clBox);
		CBrush* pOldBrush = pDC->SelectObject(&brush);
		if (pOldBrush != NULL)
		{
			pDC->Rectangle(rcFullBar);

#if 0
			// draw box that the text will go in

			CPen pen(PS_SOLID, 0, clBox);
			CPen* pOldPen = pDC->SelectObject(&pen);

			CBrush textbrush(clTextBox);
			CBrush* pOldBrush = pDC->SelectObject(&textbrush);
			pDC->Rectangle(rcText);
#endif

			// draw the text
			COLORREF clOldText = pDC->SetTextColor(RGB_COLOR_WHITE);
			int nOldMode = pDC->SetBkMode(TRANSPARENT);
			pDC->DrawText(strHeading, rcText, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX);

			// clean up dc
			pDC->SetBkMode(nOldMode);
			pDC->SetTextColor(clOldText);

			pDC->SelectObject(pOldBrush);
		}
		pDC->SelectObject(pOldPen);
	}
	if (pOldFont)
		pDC->SelectObject(pOldFont);
}

BOOL CBrowserDialog::DrawLargeImage(REFRESH_EXTENT_PTR pRE, CDC* pDC)
{
   BOOL     interrupted = FALSE;
   BOOL     bLargeDrawFailed = FALSE;
   int      nRetVal;

	REFRESH_EXTENT* pExtent = m_RefreshArray.GetExtent(0);
	PBOX pbox = pExtent->extent;

   // Get the current clip box so we only draw what we need to.
   CRect crClip(pbox.x0, pbox.y0, pbox.x1, pbox.y1);

   // See if we are already drawing. If so, try again later.
	static BOOL fDrawing = FALSE;
	CBoolSemaphore Sem(fDrawing);
	if (!Sem.Lock())
	{
		TRACE("CBrowserDialog: Busy drawing!\n");
      return FALSE;
   }

   // anything to draw?
   if (m_pItemPreviews == NULL || m_pItemPreviews->GetSize() == 0)
      return FALSE;

	// Large preview always shows the one and only item in the preview array
   CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(0);
	if (!pItem)
		return FALSE;

   // Ok, draw the large preview.
   CRect crThumbBounds = pItem->m_crThumbnail;

   CString csCollection, csFile, csFriendlyName, csAccessName;
   CPMWCollection* pCollection =
      m_pCollectionManager->FindCollection(m_uSelectedCollectionID, CollectionType(), CollectionSubType());

   if (pCollection && 
       GetItemInformation(m_uSelectedCollectionID,
                          m_dwSelectedItemNumber,
                          csCollection, csFile, csFriendlyName, csAccessName))
   {
      int   nSavedDC = pDC->SaveDC();
      ASSERT(nSavedDC);

      CSize    csNewSize;
      PBOX     pboxBound;
      CRect    crDrawRect, crDocBounds;
      CRgn     rgnPreview;
      REFRESH_EXTENT* pExt = NULL;

		nRetVal = m_CollectionDraw.CreateDocument(csAccessName, CollectionType());
      if (nRetVal == ERRORCODE_None)
      {
			m_CollectionDraw.GetDocument()->GetPreviewInfo(crThumbBounds,
																		  FALSE,
																		  csNewSize,
																		  pboxBound);

         int nMargin = crThumbBounds.Width() - csNewSize.cx;
         crDocBounds.left = crThumbBounds.left + nMargin/2;
         crDocBounds.right = crThumbBounds.right - nMargin/2;

         nMargin = crThumbBounds.Height() - csNewSize.cy;
         crDocBounds.top = crThumbBounds.top + nMargin/2;
         crDocBounds.bottom = crThumbBounds.bottom - nMargin/2;

         if(crDrawRect.IntersectRect(&crThumbBounds, crDocBounds) == 0)
            crDrawRect = crThumbBounds;

         CRect crPreviewClip;
         if (crPreviewClip.IntersectRect(crClip, crDrawRect))
         {
            if(nSavedDC)
            {
               if (rgnPreview.CreateRectRgn(crPreviewClip.left,
                                            crPreviewClip.top,
                                            crPreviewClip.right,
                                            crPreviewClip.bottom))
               {
                  pDC->SelectClipRgn(&rgnPreview);
               }
            }

				nRetVal = m_CollectionDraw.Draw(CollectionType(), csAccessName,
														  pDC, crDrawRect, (HRGN)rgnPreview,
														  UNSPECIFIED_COLOR, pExt = pRE);
            rgnPreview.DeleteObject();
         }
      }

      if(nRetVal == ERRORCODE_None)
      {
         // draw caption and selection box
         CRect crFocusClip;

         // we need to expand the clipping region to include focus area
         if (crFocusClip.IntersectRect(crClip, pItem->m_crFocusArea))
         {
            if (rgnPreview.CreateRectRgn(crFocusClip.left,
                                         crFocusClip.top,
                                         crFocusClip.right,
                                         crFocusClip.bottom))
            {
               pDC->SelectClipRgn(&rgnPreview);
            }

            DrawSelect(*pDC, pItem, TRUE);
            rgnPreview.DeleteObject();
         }
      }
      else // either drawing failed or was interrupted
      {
         if (pExt == NULL || pExt->update_object == NULL)
         {
            // if an error was returned and update_object is NULL, we're hosed
            bLargeDrawFailed = TRUE;
         }
         else
         {
            // we were interrupted -- pass back extent
            interrupted = TRUE;
         }
      }

      if(nSavedDC)
         pDC->RestoreDC(nSavedDC);
   }

   if(bLargeDrawFailed)
      PostMessage(WM_COMMAND, IDC_BROWSER_VIEW_MEDIUM, 0);

   return interrupted;
}

// Interruptable drawing of Large images
void CBrowserDialog::Refresh()
{
   // if no extents, or not showing large thumbnail, there's nothing to do
   if ((m_RefreshArray.NumberOfExtents() == 0) || (m_thumbsize != thumbsizeLarge))
      return;

	CBoolSemaphore Semaphore(m_fRefreshSemaphore);
	if (!Semaphore.Lock())
   {
      /* Pretend we got interupted. Everything stays the same. */
      TRACE("View semaphore is locked...\n");
      return;
   }

	// Get a DC for drawing
   CDC* pDC = GetDC();

   // Process the refresh extents.
	while (m_RefreshArray.NumberOfExtents() != 0)
	{
		REFRESH_EXTENT* pExtent;

		// Get the next extent.
		pExtent = m_RefreshArray.GetExtent(0);
		PBOX pbox = pExtent->extent;

      // Get the current clip box so we only draw what we need to.
      CRect crClip(pbox.x0, pbox.y0, pbox.x1, pbox.y1);
      if (crClip.IsRectEmpty())
         return;  // nothing to do

      CRect crPreviewClip;
      if (!crPreviewClip.IntersectRect(crClip, m_crPreview))
         return;  // nothing to do

		/* Update the objects. */
		if (DrawLargeImage(pExtent, pDC))
		{
         // we've been interrupted
         break;
		}

		// Get rid of the update state.
		pExtent->FreeUpdateState();
		// Remove extent

		m_RefreshArray.DeleteExtent(0);
	}

	// Release the DC we got.
	ReleaseDC(pDC);
}

BOOL CBrowserDialog::OnEraseBkgnd(CDC* pDC) 
{
	// we're doing the background ourselves
	return TRUE;
}

// Paint all the special backgound colors, textures, borders,  etc.
void CBrowserDialog::PaintBkgnd(CDC* pDC) 
{
	CRect rc;
	GetClientRect(rc);

   // Get the current clip box so we only draw what we need to.
   CRect crClip(0,0,0,0);
   if (pDC->GetClipBox(crClip) == NULLREGION)
      return;  // nothing to do

   // Figure out where things should go.
   ComputeLayout();

   // get clip boxes of Sidebar and Preview areas
   CRect crSidebarClip, crPreviewClip;
   crSidebarClip.IntersectRect(crClip, m_crSidebar);
   crPreviewClip.IntersectRect(crClip, m_crPreview);

   // Use our global PrintMaster palette.
   CPalette* pOldPalette = pDC->SelectPalette(pOurPal, FALSE);
   pDC->RealizePalette();

	// Draw the preview background
	CBmpTiler bmpTiler;
	if ((HBITMAP)m_bmpPreviewBkgrnd)
		bmpTiler.TileBmpInRect(*pDC, m_crPreview, m_bmpPreviewBkgrnd);

	// If no items were found, report this to the user.
	// Note that when the window is sized, the "no matches" string moves
	// around to center itself. This requires the OnSize() handler to
	// invalidate the entire preview area if the browser is in the "no
	// matches found" state.
	//
	// PMGTODO: This could should probably go into PaintPreview(), but it
	// was convenient to put here for the moment, because all the galleries
	// share the function and PaintPreview() is overridden for the sentiment
	// gallery.
	if (NoMatchingItems())
	{
		// Get the font to use. This will be the font used for the thumbnail
		// labels at twice the normal height.
		if (m_pPreviewFont != NULL)
		{
			LOGFONT lfFont;
			if (m_pPreviewFont->GetObject(sizeof(lfFont), &lfFont) > 0)
			{
				lfFont.lfHeight *= 2;

				CFont Font;
				if (Font.CreateFontIndirect(&lfFont))
				{
					CFont* pOldFont = pDC->SelectObject(&Font);
					if (pOldFont != NULL)
					{
						// Get the string to use.
						CString csNoMatch;
						csNoMatch.LoadString(IDS_NO_MATCHES);

						// Center the string.
						CSize czNoMatch = pDC->GetTextExtent(csNoMatch, csNoMatch.GetLength());
						int nX = m_crPreview.left+(m_crPreview.Width()-czNoMatch.cx)/2;
						if (nX < m_crPreview.left)
						{
							nX = m_crPreview.left;
						}
						int nY = m_crPreview.top+(m_crPreview.Height()-czNoMatch.cy)/2;
						if (nY < m_crPreview.top)
						{
							nY = m_crPreview.top;
						}

						// Draw the text over the background.
						COLORREF clOldTextColor = pDC->SetTextColor(m_clPreviewText);
						int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
						UINT uOldTextAlign = pDC->SetTextAlign(TA_TOP);

						pDC->TextOut(nX, nY, csNoMatch);

						pDC->SetTextAlign(uOldTextAlign);
						pDC->SetBkMode(nOldBkMode);
						pDC->SetTextColor(clOldTextColor);

						pDC->SelectObject(pOldFont);
					}
				}
			}
		}
	}

	if (!crSidebarClip.IsRectNull())
	{
		// Draw the Sidebar area. This includes a tiled background, a
		// tiled border, a transparent title, and all the control text
		if ((HBITMAP)m_bmpSidebarBkgrnd && (HBITMAP)m_bmpSidebarBorder)
		{
			BorderPts pts = {3,97,3,97};
			bmpTiler.TileBmpAndBorderInRect(*pDC, m_crSidebar, m_bmpSidebarBkgrnd,
													  m_bmpSidebarBorder, pts);
		}

		// title...
		if ((HBITMAP)m_bmpTitle)
		{
			CRect crSideBar;
			GetClientRect(crSideBar);
			int topPos = 26;
			int centerPos = crSideBar.Width()/2;

			// get position from static control in dialog
			CWnd* pWnd = GetDlgItem(IDC_TITLE);
			if (pWnd)
			{
				CRect rect;
				pWnd->GetWindowRect(&rect);
				ScreenToClient(&rect);
				topPos = rect.top;
				centerPos = (rect.left+rect.right)/2;
			}
			BITMAP BitmapInfo;
			m_bmpTitle.GetBitmap(&BitmapInfo);
			CRect crTitle;
			crTitle.left = centerPos-(BitmapInfo.bmWidth/2);
			crTitle.top = topPos;
			crTitle.right = crTitle.left+BitmapInfo.bmWidth;
			crTitle.bottom = crTitle.top+BitmapInfo.bmHeight;
			Util::TransparentBltBitmap(*pDC, crTitle, m_bmpTitle, NULL, NULL, 0, 0);
		}

		// We're going to draw the static text ourselves, since we want them to be
		// transparent.

		CFont* pDialogFont = GetFont();
		if (pDialogFont != NULL)
		{
			LOGFONT LogFont;
			if (pDialogFont->GetLogFont(&LogFont))
			{
				// Make the font bold.
				LogFont.lfWeight = 700;
				CFont Font;
				if (Font.CreateFontIndirect(&LogFont))
				{
					CFont* pOldFont = pDC->SelectObject(&Font);
					if (pOldFont !=NULL)
					{
						CRect rectCtrl;
						CString text;

						int oldMode = pDC->SetBkMode(TRANSPARENT);

						CWnd* pStatic = GetDlgItem(IDC_COLLECTION_LABEL);
						if (pStatic)
						{
							pStatic->GetWindowRect(&rectCtrl);
							ScreenToClient(&rectCtrl);
							pStatic->GetWindowText(text);
							pDC->DrawText(text, rectCtrl, DT_SINGLELINE);
						}
						pStatic = GetDlgItem(IDC_GRAPHIC_TYPE_LABEL);
						if (pStatic)
						{
							pStatic->GetWindowRect(&rectCtrl);
							ScreenToClient(&rectCtrl);
							pStatic->GetWindowText(text);
							pDC->DrawText(text, rectCtrl, DT_SINGLELINE);
						}
						pStatic = GetDlgItem(IDC_PROJECT_TYPE_LABEL);
						if (pStatic)
						{
							pStatic->GetWindowRect(&rectCtrl);
							ScreenToClient(&rectCtrl);
							pStatic->GetWindowText(text);
							pDC->DrawText(text, rectCtrl, DT_SINGLELINE);
						}
						pStatic = GetDlgItem(IDC_CATEGORY_LABEL);
						if (pStatic)
						{
							pStatic->GetWindowRect(&rectCtrl);
							ScreenToClient(&rectCtrl);
							pStatic->GetWindowText(text);
							pDC->DrawText(text, rectCtrl, DT_SINGLELINE);
						}
						pStatic = GetDlgItem(IDC_TONE_LABEL);
						if (pStatic)
						{
							pStatic->GetWindowRect(&rectCtrl);
							ScreenToClient(&rectCtrl);
							pStatic->GetWindowText(text);
							pDC->DrawText(text, rectCtrl, DT_SINGLELINE);
						}
						pStatic = GetDlgItem(IDC_KEYWORD_LABEL);
						if (pStatic)
						{
							pStatic->GetWindowRect(&rectCtrl);
							ScreenToClient(&rectCtrl);
							pStatic->GetWindowText(text);
							pDC->DrawText(text, rectCtrl, DT_SINGLELINE);
						}
						pDC->SetBkMode(oldMode);
						pDC->SelectObject(pOldFont);
					}
				}
			}
		}
	}

   // Draw some border lines under the menu bar, between the sidebar
   // and preview areas, and between preview area and scrollbar
   CRect crBorder = m_crClient;
   crBorder.bottom = crBorder.top+1;
   Util::FillRectangleWithColor(*pDC, crBorder, RGB_COLOR_BLACK);

	crBorder = m_crSidebar;
	crBorder.right += 1;
	crBorder.left = crBorder.right-1;
	Util::FillRectangleWithColor(*pDC, crBorder, RGB_COLOR_BLACK);

	crBorder = m_crPreview;
	crBorder.left = m_crPreview.right - 1;
	crBorder.right = crBorder.left + 1;
	Util::FillRectangleWithColor(*pDC, crBorder, PALETTERGB(128,128,128));

   // Restore the previous DC settings.
   if (pOldPalette)
      pDC->SelectPalette(pOldPalette, FALSE);
}

CPalette* CBrowserDialog::GetThumbnailPalette()
{
	// Create a special palette to use for transparent thumbnail blts. We take
	// the PrintMaster palette and modify TRANSPARENT_INDEX to be a unique color,
	// so we're sure that no other index gets transparency.

	if (m_pThumbnailPalette == NULL)
	{
		m_pThumbnailPalette = new CPalette;

		struct
		{
			WORD palVersion;
			WORD palNumEntries;
			PALETTEENTRY palEntry[256];
		} logpal;

		logpal.palVersion = 0x300;
		logpal.palNumEntries = 256;
		for (int i = 0; i < 256; i++)
		{
			PALETTEENTRY entry;
			pOurPal->GetPaletteEntries(i, 1, &entry);
			logpal.palEntry[i] = entry;
		}

		logpal.palEntry[TRANSPARENT_INDEX].peRed = 16;
		logpal.palEntry[TRANSPARENT_INDEX].peBlue = 16;
		logpal.palEntry[TRANSPARENT_INDEX].peGreen = 16;

		m_pThumbnailPalette->CreatePalette((LPLOGPALETTE)&logpal);
	}

	return m_pThumbnailPalette;
}

// Draw a thumbnail preview. The item preview object contains
// all the information needed to do this including the preview
// DIB and the caption string. By default, we use the font
// selected into the DC and other DC attributes for drawing.
void CBrowserDialog::DrawThumbnailPreview(CDC& dc, CPreviewArray::Item* pItemPreview)
{
	if (pItemPreview == NULL)
	{
	   ASSERT(pItemPreview != NULL);
		return;
	}

	// Draw the preview thumbnail.
	LPBITMAPINFO pDib = (LPBITMAPINFO)(pItemPreview->m_pThumbnailData);
	if (pDib != NULL)
	{
		LPBYTE pDibData = (LPBYTE)pDib + sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD);

      pDibData = (LPBYTE)pDib + sizeof(BITMAPINFOHEADER);
      // If DIB has a palette, DIB data follows palette entries
      if(pDib->bmiHeader.biBitCount <= 8)
         pDibData += 256*sizeof(RGBQUAD);

		if (pItemPreview->m_bTransparent)
		{
         // Transparency is currently supported with 8 bit DIBs only
         ASSERT(pDib->bmiHeader.biBitCount <= 8);

			// For Transparent drawing, we need to use our special Thumbnail palette,
			// since we have to assure that RGB values in TRANSPARENT_INDEX are unique
			CPalette* pPal = GetThumbnailPalette();
			ASSERT(pPal);
			CPalette* pOldPal = dc.SelectPalette(pPal, TRUE);	// select as background
			dc.RealizePalette();

			// Modify TRANSPARENT_INDEX in the bitmap's colortable
			PALETTEENTRY palEntry;
			pPal->GetPaletteEntries(TRANSPARENT_INDEX, 1, &palEntry);

			pDib->bmiColors[TRANSPARENT_INDEX].rgbRed = palEntry.peRed;
			pDib->bmiColors[TRANSPARENT_INDEX].rgbBlue = palEntry.peBlue;
			pDib->bmiColors[TRANSPARENT_INDEX].rgbGreen = palEntry.peGreen;

			HBITMAP hbm = CreateDIBitmap(dc.GetSafeHdc(), &pDib->bmiHeader,
												  CBM_INIT, pDibData, pDib,
												  DIB_RGB_COLORS);

			if (hbm)
			{
				CBitmap* pBitmap = CBitmap::FromHandle(hbm);
				if (pBitmap)
				{
					CDC dcBitmap;
					dcBitmap.CreateCompatibleDC(&dc);
					CBitmap* pOldBitmap = dcBitmap.SelectObject(pBitmap);
					if (pOldBitmap != NULL)
					{
						CPalette* pOldPal = dcBitmap.SelectPalette(pPal, TRUE);

						CRect crBitmap(0, 0, pDib->bmiHeader.biWidth-1, pDib->bmiHeader.biHeight-1);
						COLORREF transcolor = PALETTERGB(palEntry.peRed, palEntry.peGreen, palEntry.peBlue);
						Util::TransparentBlt(dc, pItemPreview->m_crThumbnail, dcBitmap, crBitmap, transcolor);

						if (pOldPal)
							dcBitmap.SelectPalette(pOldPal, FALSE);

						dcBitmap.SelectObject(pOldBitmap);
					}
				}
				DeleteObject(hbm);
			}

			if (pOldPal)
				dc.SelectPalette(pOldPal, FALSE);
		}
		else
		{
			StretchDIBits( dc.GetSafeHdc(),
								pItemPreview->m_crThumbnail.left,
								pItemPreview->m_crThumbnail.top,
								pItemPreview->m_crThumbnail.Width(),
								pItemPreview->m_crThumbnail.Height(),
								0,
								0,
								(int)(pDib->bmiHeader.biWidth),
								(int)(pDib->bmiHeader.biHeight),
								pDibData,
								pDib,
								DIB_RGB_COLORS,
								SRCCOPY );
		}
   }

	// Check if we're the "selected" item. If so, we to draw a special border.
	BOOL fSelected = (pItemPreview->m_uCollectionID == m_uSelectedCollectionID) &&
						  (pItemPreview->m_dwItemNumber == m_dwSelectedItemNumber);
	DrawSelect(dc, pItemPreview, fSelected);
}

void CBrowserDialog::DrawSelect(CDC& dc, CPreviewArray::Item* pItemPreview, BOOL fSelected)
{
	ASSERT(pItemPreview);
	if(!pItemPreview)
		return;

	// Figure out where to put the caption.
	// We need to do this even if caption is blank, since we need the rect
	// if the user tries to edit the caption later.
	CRect crThumbnail = pItemPreview->m_crThumbnail;
	CRect crCaption(0, 0, m_ThumbnailSize.cx, m_ThumbnailMargin.cy * 2 / 3);
	crCaption.left -= (m_ThumbnailMargin.cx/2 - 4);
	crCaption.right += (m_ThumbnailMargin.cx/2 - 4);
	crCaption.top += 4;
	crCaption.bottom += 6;
	crCaption.OffsetRect(crThumbnail.left - (m_ThumbnailSize.cx - crThumbnail.Width())/2,
								crThumbnail.bottom);

	// Check to see if the graphic needs to be downloaded.
	// If so, we write a text string below the caption telling user it's online.
	BOOL fShowOnlineMsg = FALSE;
	if (m_pPathManager->GetPathBindingType(pItemPreview->m_csAccessName) == PBT_InternetPath)
	{
		CString strColl;
		CString strFile;

		m_pPathManager->BindPath(pItemPreview->m_csAccessName, strFile, &strColl);
		
		if(!GetContentManager()->IsFileLocallyCached(strColl, strFile))
			fShowOnlineMsg = TRUE;
	}

	// See if we're displaying a "Special Collection" item
	UINT uID = pItemPreview->m_uCollectionID;
	CPMWCollection* pCollection = m_pCollectionManager->FindCollection(uID, CollectionType(), CollectionSubType());
#if 0
	CString strName = pCollection->GetFriendlyName();
	BOOL fShowSpecial = (strName == m_csSpecialCollection);
#endif

	CString strCopyrightTitle = pCollection->GetCopyrightTitle();
	BOOL fShowSpecial = !strCopyrightTitle.IsEmpty();

	if (fShowOnlineMsg || fShowSpecial)
	{
		crCaption.bottom -= 10;	// only allow 2 lines of text
	}

	// now save caption rect
	pItemPreview->m_crCaption = crCaption;

	// Draw the caption and "online" strings...
	CString strCaption = pItemPreview->m_csCaption;
	if ((!strCaption.IsEmpty() || fShowOnlineMsg || fShowSpecial) && !m_fEditingTitle)
	{
		// set up the dc bkmode and font
		int prevBkMode = dc.SetBkMode(TRANSPARENT);
		CFont* pOldFont = NULL;
		if (m_pPreviewFont)
			pOldFont = dc.SelectObject(m_pPreviewFont);

		// Draw caption if there is one
		if (!strCaption.IsEmpty())
		{
			// Figure out text color
			COLORREF clTextColor;
			clTextColor = fSelected ? BROWSER_SELECTED_CAPTION_COLOR : m_clPreviewText;
			if(!pItemPreview->m_bIsLicensed)
				clTextColor = BROWSER_UNLICENSED_CAPTION_COLOR;

			COLORREF clOldTextColor = dc.SetTextColor(clTextColor);
			Util::DrawText(&dc, pItemPreview->m_csCaption, crCaption,
								DT_CENTER|DT_TOP|DT_NOPREFIX|DT_WORDBREAK);
			dc.SetTextColor(clOldTextColor);
		}

		// Show additional message if needed
		if (fShowOnlineMsg || fShowSpecial)
		{
			CRect rect = crCaption;
			Util::DrawText(&dc, pItemPreview->m_csCaption, rect,
								DT_CENTER|DT_TOP|DT_NOPREFIX|DT_WORDBREAK|DT_CALCRECT);

			rect.left -= 1;
			rect.right = crCaption.right;
			rect.top = min(rect.bottom, crCaption.bottom);
			rect.bottom += 14;

			CString strMsg;
			if (fShowOnlineMsg)
				strMsg.LoadString(IDS_AVAILABLE_ONLINE);
			else
				strMsg = strCopyrightTitle;

			COLORREF clOldTextColor = dc.SetTextColor(PALETTERGB(0,0,0));
			Util::DrawText(&dc, strMsg, rect, DT_CENTER|DT_TOP);
			dc.SetTextColor(clOldTextColor);
		}

		// restore dc
		dc.SetBkMode(prevBkMode);
		if (pOldFont)
			dc.SelectObject(pOldFont);
	}

	// draw dropshadow if project (but not on Large view)
	if ((CollectionType() == CPMWCollection::typeProjects && m_thumbsize != thumbsizeLarge) ||
		CollectionType() == CPMWCollection::typeSentiments)
	{
		CPen* pOldPen = (CPen*)dc.SelectStockObject(NULL_PEN);
		if (pOldPen != NULL)
		{
			CBrush brush(PALETTERGB(128,128,128));
			CBrush* pOldBrush = dc.SelectObject(&brush);
			if (pOldBrush != NULL)
			{
				CRect rcWhole = pItemPreview->m_crThumbnail;
				rcWhole.OffsetRect(2, 2);

				CRect rect(rcWhole.right - 2, rcWhole.top, rcWhole.right + 2, rcWhole.bottom);
				dc.Rectangle(rect);

				rect.SetRect(rcWhole.left, rcWhole.bottom - 2, rcWhole.right + 2, rcWhole.bottom + 2);
				dc.Rectangle(rect);

				dc.SelectObject(pOldBrush);
			}
			dc.SelectObject(pOldPen);
		}
	}

	if(fSelected)
	{
		// draw box around selection
		CPen Pen(PS_SOLID, 1, PALETTERGB(128,128,128));
		CPen* pOldPen = dc.SelectObject(&Pen);
		if (pOldPen != NULL)
		{
			CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
			if (pOldBrush != NULL)
			{
				CRect crSelect = pItemPreview->m_crFocusArea;
				dc.Rectangle(crSelect);
				crSelect.InflateRect(-1, -1);
				dc.Rectangle(crSelect);

				dc.SelectObject(pOldBrush);
			}
			dc.SelectObject(pOldPen);
		}
	}
}

void CBrowserDialog::OnEnterIdle(UINT nWhy, CWnd* pWho)
{
   // we need this so we get idle messages when Welcome box is up
   CWnd* pParent = GetParent();
   if (pParent)
      pParent->SendMessage(WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)m_hWnd);

	//Send a kick idle to redraw background when top-level dialog is moved around
	
	MSG Msg;
	if(!::PeekMessage(&Msg, NULL, WM_KICKIDLE, WM_KICKIDLE, PM_NOREMOVE))
		SendMessage(WM_KICKIDLE, 0, 0);
}

// The user has changed the size of the browser window.
void CBrowserDialog::OnSize(UINT nType, int cx, int cy) 
{
	if ((cx == 0 && cy == 0) || m_pItemPreviews == NULL)
      return;

	if (m_fEditingTitle)
		OnKillfocusEditTitle();

   // Don't call inherited OnSize() since it will call
   // the CPmwDialog OnSize which does all the control
   // resizing which we don't want (or do we?)
   // PMGTODO: Rip out the control resizing code from CPmwDialog.

	// position the controls at bottom of sidebar
	PositionControls();
   
	BOOL growing = (cy > m_crSidebar.bottom + GetStatusBarHeight());
	BOOL shrinking = (cy < m_crSidebar.bottom + GetStatusBarHeight());
	if (growing)
	{
		// Invalidate old sidebar area bottom if we're growing.
		// We need to do this to erase the old border
		CRect rect(0, m_crSidebar.bottom - 8, m_crSidebar.right, m_crSidebar.bottom);
		InvalidateRect(rect);
	}

   CSize oldThumbnailLayout = m_ThumbnailLayout;
	UpdatePreview();		// note: this calls ComputeLayout()

	if ((m_ThumbnailLayout != oldThumbnailLayout)
	 || (m_thumbsize == thumbsizeLarge)
	 || (NoMatchingItems()))
	{
		UpdateItemPreviews(FALSE);
		InvalidateRect(m_crPreview);
	}
	else if (!growing && !shrinking)
	{
		// We must be changing width - redraw the right edge so
		// headings and vertical lines next to scrollbar gets redrawn
		CRect rect;
		GetClientRect(rect);
		rect.left = m_crMinimum.right;
		if(m_thumbsize != thumbsizeLarge)
			rect.left += m_ThumbnailSize.cx;

		InvalidateRect(rect);
	}

	if (shrinking)
	{
		// Invalidate new sidebar area bottom if we're shrinking
		CRect rect(0, m_crSidebar.bottom - 8, m_crSidebar.right, m_crSidebar.bottom);
		InvalidateRect(rect);
	}
}

// Windows wants to know how big and small we can get. The minimum
// size in m_crMinimum is determined by ComputeLayout().
void CBrowserDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
   // Get normal values.
   INHERITED::OnGetMinMaxInfo(lpMMI);

	if (m_crMinimum.IsRectEmpty())
		return;

   // Enforce minimum size.
   CRect crMinimum(m_crMinimum);
   if(m_thumbsize != thumbsizeLarge)
      crMinimum.right += GetSystemMetrics(SM_CXVSCROLL) + m_ThumbnailSize.cx +
         m_PreviewMargin.cx + m_ThumbnailMargin.cx;
   else
      crMinimum.right = min(m_crSidebar.Width() + 200, 640);

   AdjustWindowRect(crMinimum, GetStyle(), GetMenu() != NULL);
   lpMMI->ptMinTrackSize.x = crMinimum.Width();
   lpMMI->ptMinTrackSize.y = crMinimum.Height();
}

// The user changed the selection in the collection combo box.
void CBrowserDialog::OnSelchangeCollectionCombo() 
{
   if (!m_fIgnoreSelectionChange)
   {
      CollectionChanged();
   }
}

// The user changed the selection in the graphic type combo box.
void CBrowserDialog::OnSelchangeGraphicTypeCombo()
{
   if (!m_fIgnoreSelectionChange)
   {
		GraphicTypeChanged();
   }
}

// The user changed the selection in the Type combo box.
void CBrowserDialog::OnSelchangeProjectTypeCombo() 
{
	if (!m_fIgnoreSelectionChange)
	{
		ProjectTypeChanged();
	}
}

void CBrowserDialog::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	BOOL  fEnable = FALSE;
	long  lResultCount = m_SearchContext.GetResultCount();

	if(CollectionType() == CPMWCollection::typeArt)
	{
		if(lResultCount > 0 && m_uSelectedCollectionID >= 0 && m_dwSelectedItemNumber &&
			m_pItemPreviews != NULL && m_pItemPreviews->GetSize() > 0 && m_nCurPreviewItem >= 0)
		{
			CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_nCurPreviewItem);
			if(pItem && pItem->m_bIsLicensed)
				fEnable = TRUE;
		}
	}
	else
	{
		HMENU hMenu = pCmdUI->m_pMenu->GetSafeHmenu();
		if(hMenu)
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
	}

	pCmdUI->Enable(fEnable);
}

void CBrowserDialog::OnEditCopy() 
{
   if(CollectionType() == CPMWCollection::typeSentiments)
      return;
   
#if 1
	COleDataSource* pDataSource = CreateDragDropSource();
	if (pDataSource != NULL)
	{
		// Set the data on the clipboard.
		pDataSource->SetClipboard();
		// And do *not* delete it. It will delete itself when the clipboard is flushed.
	}

#else
   BOOL        bCreated;
   int         nRetVal;
   CDC         dcMem, dcScreen;
   CBitmap     bitmap, *pOldBitmap;
   PBOX        pboxBound;
   CSize       csNewSize;
   CString     csCollection;
   CString     csFile;
   CString     csFriendlyName;
   CString     csAccessName;

   CPMWCollection *  pCollection = m_pCollectionManager->FindCollection(m_uSelectedCollectionID,CollectionType());
   ASSERT(pCollection);
   if(pCollection == NULL)
      return;

   if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber,
      csCollection, csFile, csFriendlyName, csAccessName) == 0)
      return;


	nRetVal = m_CollectionDraw.CreateDocument(csAccessName);
   if(nRetVal != ERRORCODE_None)
      return;

   // Get System Metric Here
   HDC hScreenDC = ::GetDC(NULL);
   if (hScreenDC == NULL)
     return;
   dcScreen.Attach(hScreenDC);

   int nScreenWidth = dcScreen.GetDeviceCaps(HORZRES);
   int nScreenHeight = dcScreen.GetDeviceCaps(VERTRES);

   CRect crImage(0, 0, nScreenWidth, nScreenHeight);

	m_CollectionDraw.GetDocument()->GetPreviewInfo(crImage,
																  TRUE,
																  csNewSize,
																  pboxBound);
   crImage.right = csNewSize.cx;
   crImage.bottom = csNewSize.cy;

   bCreated = dcMem.CreateCompatibleDC(&dcScreen);
   ASSERT(bCreated);
   if(bCreated)
      {
         BeginWaitCursor();

         bCreated = bitmap.CreateCompatibleBitmap( &dcScreen, crImage.Width(), crImage.Height());
         
         // Done with screen DC
         ::ReleaseDC(NULL, (HDC)dcScreen.Detach());

         ASSERT(bCreated);
         if(bCreated)
            {
               pOldBitmap = dcMem.SelectObject(&bitmap);
					if (pOldBitmap != NULL)
					{
						CBrush brushStock;
						brushStock.CreateStockObject(WHITE_BRUSH);
						dcMem.FillRect(crImage,&brushStock);

						// Draw with fill color of white
						m_CollectionDraw.Draw(CollectionType(),
													 csAccessName,
													 &dcMem,
													 crImage,
													 NULL,
													 RGB(255, 255, 255));

						// Start filling in the clipboard
						OpenClipboard();
						EmptyClipboard();

						HBITMAP hBitmap = (HBITMAP)bitmap.Detach();
						HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();

						// Generate the DIB from the bitmap.
						HGLOBAL hDIB;
						if (::BitmapToDIB(hBitmap, hOurPal, &hDIB) == ERRORCODE_None)
						{
							SetClipboardData(CF_DIB, hDIB);
						}

						SetClipboardData(CF_BITMAP, hBitmap);
						
						CloseClipboard();

						dcMem.SelectObject(pOldBitmap);

						// Put PrintMaster palette on clipboard
						CPalette* pPalette = CreateIdentityPalette();
						if (pPalette != NULL)
						{
							HPALETTE hPalette = (HPALETTE)pPalette->Detach();
							::SetClipboardData(CF_PALETTE, hPalette);
							delete pPalette;
						}
					}
            }
         EndWaitCursor();
      }
   else
        ::ReleaseDC(NULL, (HDC)dcScreen.Detach());
#endif
}

void CBrowserDialog::OnEditOrganizeCategories()
{
	CPMWCollection* pCollection = m_pCollectionManager->GetUserCollection(CollectionType());
	if (pCollection == NULL)
	{
		ASSERT(0);
		return;
	}

	int nStream = CPMWCollection::streamCategory;
	if (!pCollection->IsOpen(nStream))
	{
		if (pCollection->OpenStreams(nStream) != ERRORCODE_None)
			return;
	}

	CCategoryOrganizerDlg dlg(pCollection, this);
	if (dlg.DoModal() != IDCANCEL)
	{
		// User modified something
		UpdateAllBrowsers();
	}
}

void CBrowserDialog::OnUpdateEditOrganizeCategories(CCmdUI *pCmdUI)
{
	CPMWCollection* pCollection = m_pCollectionManager->GetUserCollection(CollectionType());
	if (pCollection == NULL)
		return;

	BOOL fEnable = FALSE;
	CPMWCollectionSearchContext Context;
	if (pCollection->Search(Context) == ERRORCODE_None)
		fEnable = (Context.GetResultCount() > 0);

	pCmdUI->Enable(fEnable);
}

// The user changed the selection in the category combo box.
void CBrowserDialog::OnSelchangeCategoryCombo() 
{
   if (!m_fIgnoreSelectionChange)
   {
      CategoryChanged();
   }
}

// The user changed the selection in the tone combo box.
void CBrowserDialog::OnSelchangeToneCombo() 
{
   if (!m_fIgnoreSelectionChange)
   {
      ToneChanged();
   }
}

// The user changed the selection in the keyword combo box.
void CBrowserDialog::OnSelchangeKeywordList() 
{
   if (!m_fIgnoreSelectionChange)
   {
      CComboBox* pKeywordList = (CComboBox*)GetDlgItem(IDC_KEYWORD_LIST);
      if (pKeywordList != NULL)
      {
         int nIndex = pKeywordList->GetCurSel();
         if (nIndex != CB_ERR)
         {
            KeywordChanged(nIndex);
         }
      }
   }
}

// The user clicked on the "Find" button. Basically, this button
// is just a place for the user to click when they finish typing
// in the keyword edit control.
void CBrowserDialog::OnFind() 
{
   KeywordChanged(-1);
   UpdateResultSet();
}

void CBrowserDialog::OnFindAnyAll() 
{
	UpdateResultSet();
}

// The user clicked the select button.
void CBrowserDialog::OnSelect()
{
   if (GetConfiguration()->IsAddOn () == TRUE)
      return;

   DoSelect();
}

// The user clicked the reset button, so do some printing.
void CBrowserDialog::OnReset()
{
   BeginWaitCursor(); // display the hourglass cursor
   
   m_fIgnoreSelectionChange = TRUE;
   m_fUpdatePreview = FALSE;

   CString csAll(BrowserAllString());

   if (m_pCollectionList != NULL)
      m_pCollectionList->SetCurSel(csAll);
   if (m_pGraphicTypeList != NULL)
      m_pGraphicTypeList->SetCurSel(csAll);
   if (m_pProjectTypeList != NULL)
      m_pProjectTypeList->SetCurSel(csAll);
   if (m_pCategoryList != NULL)
      m_pCategoryList->SetCurSel(csAll);
   if (m_pToneList != NULL)
      m_pToneList->SetCurSel(csAll);

   CComboBox*  pKeywordList = (CComboBox*)GetDlgItem(IDC_KEYWORD_LIST);
   if (pKeywordList != NULL)
      pKeywordList->SetCurSel (-1);
   
   m_fIgnoreSelectionChange = FALSE;
   m_fUpdatePreview = TRUE;

   // Get all re - done, even if collection didn't change
   CollectionChanged();

   EndWaitCursor(); // remove the hourglass cursor
}
// The user clicked the print button, so do some printing.
void CBrowserDialog::OnPrint() 
{
   DoPrint();
}

void CBrowserDialog::CancelEditTitle()
{
	if (m_pEditTitle == NULL)
	{
		ASSERT(0);
		return;
	}

	InvalidateItem(m_uEditedCollectionID, m_dwEditedItemNumber);
	m_uEditedCollectionID = (USHORT)-1;
	m_dwEditedItemNumber = (DWORD)-1;
	m_pEditTitle->ShowWindow(SW_HIDE);
	m_fEditingTitle = FALSE;
}

// remove each word in strWords found in strToModify (case insensitive)
void CBrowserDialog::RemoveWordsFromString(CString* strToModify, const CString& strWords)
{
	// check trivial case
	if (strToModify->CompareNoCase(strWords) == 0)
	{
		strToModify->Empty();
		return;
	}

	CString strToModifyLower = *strToModify;
	strToModifyLower.MakeLower();

	CString strWordsLower = strWords;
	strWordsLower.MakeLower();

	strToModifyLower = ' ' + strToModifyLower + ' ';
	while (!strWordsLower.IsEmpty())
	{
		int blankpos = strWordsLower.Find(' ');
		if (blankpos == -1)
			blankpos = strWordsLower.GetLength();
		CString word = ' ' + strWordsLower.Left(blankpos) + ' ';

		int wordpos = strToModifyLower.Find(word);
		if (wordpos != -1)
		{
			*strToModify = strToModify->Left(wordpos) +
								strToModify->Mid(wordpos+word.GetLength()-1);
			strToModifyLower = ' ' + *strToModify + ' ';
			strToModifyLower.MakeLower();
		}

		strWordsLower = strWordsLower.Mid(blankpos+1);
	}

	strToModify->TrimRight();
}

void CBrowserDialog::ChangeFriendlyName(CPMWCollection* pCollection, DWORD dwItemNum,
													 const CString& strNewFriendlyName)
{
	ASSERT(pCollection != NULL);

	BYTE uCollID = pCollection->GetID();

	// Get the scoop on the item being edited
	CString strCollection, strFile, strFriendlyName, strPath, strKeywords;

	if (GetItemInformation(uCollID, dwItemNum,
								  strCollection, strFile, strFriendlyName, strPath,
								  &strKeywords))
	{
		if (strNewFriendlyName != strFriendlyName)
		{
			CPMWCollectionData Data;
			Data.m_strFriendlyName = strNewFriendlyName;	// set the new title

			// remove old title from keywords
			RemoveWordsFromString(&strKeywords, strFriendlyName);
			Data.m_strKeywords = strKeywords;

			Data.m_strContentFileName = strFile;
			Data.m_dwUnused = 0;
			Data.m_ContentData.Empty();
			CCategoryDataItem cdItem;
			cdItem.Empty();
			Data.m_CategoryData.Set(CString(""), CString(""), &cdItem);

			// Update the item with new title
			pCollection->UpdateItemInfo(dwItemNum, &Data);
			
			// also update the item's preview info (if it's in the list)
			int nItem = m_pItemPreviews->Find(uCollID, dwItemNum);
			if (nItem >= 0)
			{
				CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(nItem);
				if (IsItemOKToView(pItem))
				{
					pItem->m_csCaption = strNewFriendlyName;
					InvalidateRect(pItem->m_crFocusArea);
				}
			}
		}
	}
}

void CBrowserDialog::OnKillfocusEditTitle() 
{
	// Edit Title control is loosing focus -- hide the control,
	// check if string has changed and update the item if so

	if (m_pEditTitle == NULL)
	{
		ASSERT(0);
		return;
	}

	m_pEditTitle->ShowWindow(SW_HIDE);

	if (m_uEditedCollectionID != (USHORT)-1 && m_dwEditedItemNumber != (DWORD)-1)
	{
		CString strNewTitle;
		m_pEditTitle->GetWindowText(strNewTitle);
		CPMWCollection* pCollection = m_pCollectionManager->FindCollection(m_uEditedCollectionID, CollectionType(), CollectionSubType());
		if (pCollection != NULL)
			ChangeFriendlyName(pCollection, m_dwEditedItemNumber, strNewTitle);
	}

	m_uEditedCollectionID = (USHORT)-1;
	m_dwEditedItemNumber = (DWORD)-1;
	m_fEditingTitle = FALSE;
}

void CBrowserDialog::EditItemTitle(CPreviewArray::Item* pItem)
{
	CRect rcCaption = pItem->m_crCaption;

	CDC* pDC = GetDC();
	CBmpTiler bmpTiler;
	if ((HBITMAP)m_bmpPreviewBkgrnd)
		bmpTiler.TileBmpInRect(*pDC, rcCaption, m_bmpPreviewBkgrnd);

	//	show our edit control
	if (m_pEditTitle)
	{
		m_pEditTitle->SetWindowPos(NULL, rcCaption.left-2, rcCaption.top-2,
											rcCaption.Width()+4, rcCaption.Height()+4,
											SWP_NOZORDER | SWP_SHOWWINDOW);

		m_pEditTitle->SetWindowText(pItem->m_csCaption);
		m_pEditTitle->SetSel(0, pItem->m_csCaption.GetLength());
		m_pEditTitle->SetFocus();

		m_uEditedCollectionID = m_uSelectedCollectionID;
		m_dwEditedItemNumber = m_dwSelectedItemNumber;
		m_fEditingTitle = TRUE;
	}
	ReleaseDC(pDC);
}

// Clicking on an item preview moves the selection to that item.
void CBrowserDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GET_PMWAPP()->RemoveHelpfulHint();
	if (m_fEditingTitle)
		OnKillfocusEditTitle();

   if (m_pItemPreviews != NULL)
   {
      int nItem = m_pItemPreviews->Find(point);
      if (nItem != -1)
      {
         CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(nItem);
         if (!pItem)
			{
            ASSERT(pItem);
            return;
			}

			// Check if user clicked in Title area of a currently selected user item.
			// If so, allow them to edit item's title.
			USHORT collID = pItem->m_uCollectionID;
			if (collID == m_uSelectedCollectionID && pItem->m_dwItemNumber == m_dwSelectedItemNumber)
         {
				CPMWCollection* pCollection = m_pCollectionManager->FindCollection(collID, CollectionType(), CollectionSubType());
				if (pCollection != NULL)
				{
					if (pCollection->IsUserCollection() && pItem->m_crCaption.PtInRect(point))
						EditItemTitle(pItem);
				}
			}
			else
			{
				SelectItem(collID, pItem->m_dwItemNumber);
				if (m_nCurPreviewItem >= 0)
				{
					m_nCurColumn = CalcColumnFromItem(pItem);
				}

				CWnd *pSelectButton = GetDlgItem(IDC_BROWSER_SELECT);
				if ( pSelectButton != NULL)
					pSelectButton->SetFocus();
			}

         // If not licensed, display license dialog
         if(!pItem->m_bIsLicensed)
         {
            OnClickedUnlicensed();
            return;
         }
         // No drag 'n drop for addons
         // (it won't work off a CD anyway)
         if (GetConfiguration()->IsAddOn () == FALSE)
         {
            // Proceed to allow OLE drag and drop operation
            COleDataSource* pSource = CreateDragDropSource();
            if (pSource != NULL)
            {
					CWnd* pParentWnd = GetParent();
					BOOL fEnabled = pParentWnd->IsWindowEnabled();
					if (!fEnabled)
					{
						// Re-enable the main window so it can be a drop target.
						pParentWnd->EnableWindow(TRUE);
					}
               // Do the drag and drop.
               DROPEFFECT dropEffect = pSource->DoDragDrop(DROPEFFECT_COPY);
					if (!fEnabled)
					{
						// Re-disable the main window.
						pParentWnd->EnableWindow(FALSE);
						// But not us!
						EnableWindow(TRUE);
					}
			   
               delete pSource;

					if (dropEffect != DROPEFFECT_NONE)
					{
						// repaint the selected item
				      InvalidateItem(m_uSelectedCollectionID, m_dwSelectedItemNumber);
					}
            }
         }
      }
   }
}

// Right-clicking on an item preview moves the selection to that item and
// offers the user an "inspect" menu.
void CBrowserDialog::OnRButtonDown(UINT nFlags, CPoint point) 
{
	GET_PMWAPP()->RemoveHelpfulHint();
	if (m_fEditingTitle)
		OnKillfocusEditTitle();

	if (m_pItemPreviews != NULL)
	{
		int nItem = m_pItemPreviews->Find(point);
		if (nItem != -1)
		{
			CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(nItem);
			SelectItem(pItem->m_uCollectionID, pItem->m_dwItemNumber);
			m_nCurColumn = CalcColumnFromItem(pItem);

			CWnd *pSelectButton = GetDlgItem(IDC_BROWSER_SELECT);
			if ( pSelectButton != NULL)
			{
				pSelectButton->SetFocus();
			}

			// Now, do the popup menu.
			if (m_nPopupMenuIndex != -1)
			{
				POINT menu_p = point;
				ClientToScreen(&menu_p);

				CMenu Menu;

				if (Menu.LoadMenu(IDR_BROWSER_POPUP))
				{
					CMenu* pSubMenu = Menu.GetSubMenu(m_nPopupMenuIndex);
					if (pSubMenu != NULL)
					{
						pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
														 menu_p.x,
														 menu_p.y,
														 this);
					}
				}
			}
		}
	}
}

void CBrowserDialog::OnLButtonUp(UINT nFlags, CPoint point) 
{
   CPmwDialogColor::OnLButtonUp(nFlags, point);
}

void CBrowserDialog::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
   AfxCancelModes(m_hWnd);

   if (bSysMenu)
      return;     /* don't support system menu */

   ASSERT(pMenu != NULL);
   // check the enabled state of various menu items

   CCmdUI state;
   state.m_pMenu = pMenu;
   ASSERT(state.m_pOther == NULL);
   ASSERT(state.m_pParentMenu == NULL);

   // determine if menu is popup in top-level menu and set m_pOther to
   //  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
// if (_afxTrackingMenu == pMenu->m_hMenu)
//    state.m_pParentMenu = pMenu;    // parent == child for tracking popup
// else
   {
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
   }

   state.m_nIndexMax = pMenu->GetMenuItemCount();
   for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
     state.m_nIndex++)
   {
      state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
      if (state.m_nID == 0)
         continue; // menu separator or invalid cmd - ignore it

      ASSERT(state.m_pOther == NULL);
      ASSERT(state.m_pMenu != NULL);
      if (state.m_nID == (UINT)-1)
      {
         // possibly a popup menu, route to first item of that popup
         state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
         if (state.m_pSubMenu == NULL ||
            (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
            state.m_nID == (UINT)-1)
         {
            continue;       /* first item of popup can't be routed to */
         }
         state.DoUpdate(this, FALSE);    // popups are never auto disabled
      }
      else
      {
         /*
         // normal menu item
         // Auto enable/disable if frame window has 'm_bAutoMenuEnable'
         //    set and command is _not_ a system command.
         */
         state.m_pSubMenu = NULL;
         state.DoUpdate(this, /*m_bAutoMenuEnable &&*/ state.m_nID < 0xF000);
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
// CWnd::OnInitMenuPopup(pMenu, nIndex, bSysMenu);
}

// Double-click does a opens the item.
void CBrowserDialog::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   if (GetConfiguration()->IsAddOn () == TRUE)
      return;

   if (m_pItemPreviews != NULL)
   {
      int nItem = m_pItemPreviews->Find(point);
      if (nItem != -1)
      {
         if (GetKeyState(VK_MENU) < 0)
         {
            OnProperties();
         }
         else
         {
            DoSelect();
         }
      }
   }
}

// Handle scroll requests.
void CBrowserDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (m_fEditingTitle)
		OnKillfocusEditTitle();

   ASSERT(m_nScrollInc > 0);
   if(m_nScrollInc <= 0)
      return;

   // Compute the new first row number.
   int nRow = m_nFirstRow;

   // Clip row number.
   int nMinRow;
   int nMaxRow;
	if(pScrollBar)
		pScrollBar->GetScrollRange(&nMinRow, &nMaxRow);
	else
		GetScrollRange(SB_VERT, &nMinRow, &nMaxRow);

   int nDesiredScrollMax = m_nItemRows - m_nFullThumbnailRows;
	BOOL fSelectItem = FALSE;
   switch (nSBCode)
   {
      case SB_TOP:
         nRow = nMinRow;
         break;
      case SB_BOTTOM:
         nRow = nDesiredScrollMax;
         break;
      case SB_LINEUP:
         nRow--;
         break;
      case SB_LINEDOWN:
         nRow++;
         break;
      case SB_PAGEUP:
         nRow -= m_nFullThumbnailRows;
         break;
      case SB_PAGEDOWN:
         nRow += m_nFullThumbnailRows;
         break;

		// PMGTODO: Thumbnail drawing is not currently fast enough to handle SB_THUMBTRACK.
      case SB_THUMBPOSITION:
         nRow = (long) nPos * m_nScrollInc;
         break;

		case SB_ENDSCROLL:
			fSelectItem = TRUE;
			break;
   }

   if (nRow < nMinRow)
      nRow = nMinRow;
   if (nRow > nDesiredScrollMax)
      nRow = nDesiredScrollMax;

   if (nRow != m_nFirstRow)
   {
      m_nFirstRow = nRow;
      UpdateScrollBar();
      UpdateItemPreviews(FALSE);
      InvalidateRect(m_crPreview);
      ASSERT(m_pItemPreviews->GetSize() > 0);

   }

	if (fSelectItem && m_pItemPreviews->GetSize() != 0)
	{
		CPreviewArray::Item* pItem = NULL;
		if (m_fHasGroups)
		{
			// figure out which item to select
			int nRow = 0;
			int nCol = 0;
			while (nRow == 0 && nCol <= m_nCurColumn)
			{
				pItem = m_pItemPreviews->GetAt(nCol);
				if (pItem == NULL)
				{
					ASSERT(pItem);
					break;
				}

				nRow = CalcRowFromItemNumber(pItem->m_nResultIndex) - m_nFirstRow;
				if (nRow > 0 && nCol > 0)
					pItem = m_pItemPreviews->GetAt(--nCol);

				++nCol;
			}
		}
		else // no groups
		{
			pItem = m_pItemPreviews->GetAt(m_nCurColumn);
		}

		if (IsItemOKToView(pItem))
			SelectItem(pItem->m_uCollectionID, pItem->m_dwItemNumber);
   }

   if( (nRow % m_nScrollInc) == 0)
      INHERITED::OnVScroll(nSBCode, nPos, pScrollBar);
}

COLORREF CBrowserDialog::GetColorFromINI(const CString &strSection,
													  const CString &strEntryToGet, COLORREF crDefault)
{
	COLORREF                crColor;
	CEnhancedWordArray      cewaValues;
	CWinApp                 *pApp = AfxGetApp();
	CString                 strEntry;
	DWORD                   wRed, wGreen, wBlue;

	strEntry = pApp->GetProfileString(strSection, strEntryToGet);
	if(strEntry.IsEmpty())
	{
		return crDefault;
	}

	// Break up string into array of word values
	cewaValues.Parse(strEntry);

	ASSERT(cewaValues.GetSize() == 3);
	if(cewaValues.GetSize() < 3)
	{
		return crDefault;
	}
	wRed = cewaValues[0];
	wGreen = cewaValues[1];
	wBlue = cewaValues[2];

	crColor = PALETTERGB(cewaValues[0], cewaValues[1], cewaValues[2]);
	return crColor;
}

void CBrowserDialog::SaveColorToINI(const CString &strSection,
												const CString &strEntryToSave, COLORREF crColor)
{
	CWinApp         *pApp = AfxGetApp();
	char            szBuff[80];

	wsprintf(szBuff, "%i,%i,%i", GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));
	pApp->WriteProfileString(strSection, strEntryToSave, szBuff);
}

void CBrowserDialog::LoadColorsFromINI(const CString &strColorSection)
{
	m_clPreviewText = GetColorFromINI(strColorSection, "PreviewText", m_clPreviewText);
}

/*
// Add the standard browser data exchange format.
*/

BOOL CBrowserDialog::AddDataExchangeFormat(CBrowserDataSource* pSource)
{
   ASSERT(pSource != NULL);

   CString csCollection;
   CString csFile;
   CString csFriendlyName;
   CString csFinalName;
	BOOL    fCropCopyright;

   if (!GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber,
									csCollection, csFile, csFriendlyName, csFinalName,
									NULL, &fCropCopyright))
   {
      return FALSE;
   }

   pSource->SetItemInformation(csCollection, csFinalName, csFriendlyName, fCropCopyright, this);
   pSource->DelayRenderFileData(CF_HDROP);

   int nFilePathSize = csFinalName.GetLength()+1;
   int nFriendlyNameSize = csFriendlyName.GetLength()+1;

   HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT,
                                sizeof(CBrowserDataExchange)
                                    + nFilePathSize + nFriendlyNameSize + 1/*for crop info*/);
   if (hMem != NULL)
   {
      CBrowserDataExchange* pData = (CBrowserDataExchange*)::GlobalLock(hMem);
      if (pData != NULL)
      {
         pData->m_dwFilePathOffset = sizeof(CBrowserDataExchange);
         pData->m_dwFriendlyNameOffset = pData->m_dwFilePathOffset + nFilePathSize;
			pData->m_dwCropCopyrightOffset = pData->m_dwFriendlyNameOffset + nFriendlyNameSize;
         strcpy(((LPSTR)pData)+pData->m_dwFilePathOffset, csFinalName);
         strcpy(((LPSTR)pData)+pData->m_dwFriendlyNameOffset, csFriendlyName);
			memcpy(((LPSTR)pData)+pData->m_dwCropCopyrightOffset, &fCropCopyright, 1);
         ::GlobalUnlock(hMem);

         pSource->CacheGlobalData(GetDataExchangeFormat(), hMem);
         return TRUE;
      }
      ::GlobalFree(hMem);
   }

   // Continue anyway.
   return TRUE;
}

BOOL CBrowserDialog::LoadAccelTable(LPCSTR lpszResourceName)
{
    ASSERT(m_hAccelTable == NULL);  // only do once
    ASSERT(lpszResourceName != NULL);

    HINSTANCE hInst = AfxGetResourceHandle();
    m_hAccelTable = ::LoadAccelerators(hInst, lpszResourceName);
    return (m_hAccelTable != NULL);
}

BOOL CBrowserDialog::PreTranslateMessage(MSG *pMsg)
{
   BOOL bTranslated=FALSE, bOurCombo = FALSE;

	CWnd* pKeywordCtrl = GetDlgItem(IDC_KEYWORD_LIST);
	CWnd  *pFocusWnd = GetFocus();

   if(pFocusWnd)
   {
		// special handling of m_pEditTitle control
		if (pFocusWnd == m_pEditTitle && Util::IsKeyMessage(pMsg, VK_ESCAPE, 0))
		{
			CancelEditTitle();
			return TRUE;
		}

      // Get class name.
      char szClassName[64];
      if (::GetClassName(pFocusWnd->GetSafeHwnd(), szClassName, sizeof(szClassName)-1) > 0)
      {
			if (strcmp(szClassName, "MindscapeTreeComboBox") == 0 &&
				 ((CTreeComboBox*)pFocusWnd)->IsListShown())
         {
               bOurCombo = TRUE;
         }
			else if (pFocusWnd->GetParent() == pKeywordCtrl)
			{
				pFocusWnd = pFocusWnd->GetParent();
	      }
		}
	}

	if (m_hAccelTable != NULL && !bOurCombo &&
		 pFocusWnd != pKeywordCtrl && pFocusWnd != m_pEditTitle)
	{
      bTranslated = ::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg);
	}

   if(!bTranslated)
      bTranslated = INHERITED::PreTranslateMessage(pMsg);

   return bTranslated;
}

void CBrowserDialog::OnUpdatePageDown(CCmdUI *pCmdUI)
{
	int nResultCount = m_SearchContext.GetResultCount();
	int nLastRow = 0;
	if (m_fHasGroups)
		nLastRow = CalcRowFromItemNumber(nResultCount - 1);
	else
		nLastRow = nResultCount / m_ThumbnailLayout.cx;

	pCmdUI->Enable(m_nFirstRow < (nLastRow - m_nFullThumbnailRows));
}

void CBrowserDialog::OnPageDown() 
{
	if (m_pItemPreviews == NULL || m_nResultItems <= 1)
		return;

	OnVScroll(SB_PAGEDOWN, NULL, NULL);
	OnVScroll(SB_ENDSCROLL, NULL, NULL);
}

void CBrowserDialog::OnUpdatePageUp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nFirstRow > 0);
}

void CBrowserDialog::OnPageUp() 
{
	if (m_pItemPreviews == NULL || m_nResultItems <= 1)
		return;

	OnVScroll(SB_PAGEUP, NULL, NULL);
	OnVScroll(SB_ENDSCROLL, NULL, NULL);
}

void CBrowserDialog::OnDown() 
{
	if (m_pItemPreviews == NULL || m_nResultItems <= 1 || m_pItemPreviews->GetSize () < 1)
		return;

	int nNewSelection = 0;

	if (m_fHasGroups && m_thumbsize != thumbsizeLarge)
	{
		// Go forward 1 item at a time until we're where we want to be
		CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_nCurPreviewItem);
		ASSERT(pItem);
		if (pItem == NULL)
			return;

		int nOrigRow = CalcRowFromItemNumber(pItem->m_nResultIndex);
		int nCurRow = nOrigRow;

		// check if we're already on the last row in the results set
		int nLastRow = CalcRowFromItemNumber(m_SearchContext.GetResultCount() - 1);
		if (nCurRow == nLastRow)
			return;

		int nOrigCol = m_nCurColumn;
		int nCurCol = nOrigCol;

		nNewSelection = m_nCurPreviewItem;

		BOOL fDone = FALSE;
		while ((nCurRow != (nOrigRow + 1) || nCurCol < nOrigCol) && !fDone)
		{
			// go forward 1 item
			nNewSelection++;
			if (nNewSelection >= m_pItemPreviews->GetSize())
			{
				nNewSelection = m_pItemPreviews->GetSize()-1;
				fDone = TRUE;
			}

			pItem = m_pItemPreviews->GetAt(nNewSelection);
			nCurRow = CalcRowFromItemNumber(pItem->m_nResultIndex);
			nCurCol = CalcColumnFromItem(pItem);

			if (nCurRow == (nOrigRow + 2))
			{
				// we've gone one too far - back up
				nNewSelection--;
				pItem = m_pItemPreviews->GetAt(nNewSelection);
				nCurRow = CalcRowFromItemNumber(pItem->m_nResultIndex);
				nCurCol = CalcColumnFromItem(pItem);
				fDone = TRUE;
			}

			if (nCurRow >= m_nFirstRow + m_nFullThumbnailRows)
			{
				OnVScroll(SB_LINEDOWN, NULL, NULL);

				// Now figure out which item to select

				// Assume just 1 item per row initially
				nNewSelection = m_nFullThumbnailRows - 1;
				pItem = m_pItemPreviews->GetAt(nNewSelection);

				// First get to the right row
				nCurRow = CalcRowFromItemNumber(pItem->m_nResultIndex);
				int nTargetRow = m_nFirstRow + m_nFullThumbnailRows - 1;
				while (nCurRow != nTargetRow)
				{
					if (nCurRow > nTargetRow)
						nNewSelection--;
					else
						nNewSelection++;

					pItem = m_pItemPreviews->GetAt(nNewSelection);
					nCurRow = CalcRowFromItemNumber(pItem->m_nResultIndex);
				}

				// Now get to the right column
				nCurCol = 0;
				while (nCurCol < m_nCurColumn)
				{
					nNewSelection++;
					if (nNewSelection >= m_pItemPreviews->GetSize())
					{
						nNewSelection = m_pItemPreviews->GetSize() - 1;
						break;
					}

					pItem = m_pItemPreviews->GetAt(nNewSelection);
					nCurCol = CalcColumnFromItem(pItem);
					nCurRow = CalcRowFromItemNumber(pItem->m_nResultIndex);
					if (nCurRow > nTargetRow)
					{
						nNewSelection--;
						break;
					}
				}

				fDone = TRUE;
			}
		}
	}
	else
	{
		nNewSelection = m_nCurPreviewItem + m_ThumbnailLayout.cx;
		if (nNewSelection >= m_nFullThumbnailRows * m_ThumbnailLayout.cx ||
			 nNewSelection >= m_pItemPreviews->GetSize())
		{
			nNewSelection = m_nCurPreviewItem;
			OnVScroll(SB_LINEDOWN, NULL, NULL);
			if (nNewSelection >= m_pItemPreviews->GetSize())
				nNewSelection = m_pItemPreviews->GetSize()-1;
		}
	}

	if (nNewSelection >= m_pItemPreviews->GetSize())
	{
		ASSERT(0);
		nNewSelection = m_pItemPreviews->GetSize()-1;
	}

	CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(nNewSelection);
	if (IsItemOKToView(pItem))
		SelectItem(pItem->m_uCollectionID, pItem->m_dwItemNumber);
}

void CBrowserDialog::OnUp() 
{
	if (m_pItemPreviews == NULL || m_pItemPreviews->GetSize () < 1 || m_nResultItems <= 1)
		return;

	int nNewSelection = -1;

	if (m_fHasGroups && m_thumbsize != thumbsizeLarge)
	{
		CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_nCurPreviewItem);
		ASSERT(pItem);
		if (pItem == NULL)
			return;

		int nOrigRow = CalcRowFromItemNumber(pItem->m_nResultIndex);
		int nCurRow = nOrigRow;

		int nOrigCol = m_nCurColumn;
		int nCurCol = nOrigCol;

		nNewSelection = m_nCurPreviewItem;

		while (nCurRow != (nOrigRow - 1) || nCurCol > nOrigCol)
		{
			// back up 1 item at a time until we're where we want to be
			nNewSelection--;

			if (nNewSelection < 0)
			{
				// We're at the top of the preview area. If there's more
				// items above, scroll up a line, otherwise we're done
				if (m_nFirstRow != 0)
				{
					OnVScroll(SB_LINEUP, NULL, NULL);
					OnVScroll(SB_ENDSCROLL, NULL, NULL);
				}

				// we're out 'o here
				return;
			}

			pItem = m_pItemPreviews->GetAt(nNewSelection);
			nCurRow = CalcRowFromItemNumber(pItem->m_nResultIndex);
			nCurCol = CalcColumnFromItem(pItem);
		}
	}
	else
	{
		nNewSelection = m_nCurPreviewItem - m_ThumbnailLayout.cx;
		if (nNewSelection < 0 && m_nFirstRow > 0)
		{
			nNewSelection = m_nCurPreviewItem;
			OnVScroll(SB_LINEUP, NULL, NULL);
		}
	}

	if (nNewSelection >= 0)
	{
		CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(nNewSelection);
		if(IsItemOKToView(pItem))
			SelectItem(pItem->m_uCollectionID, pItem->m_dwItemNumber);
	}
}

void CBrowserDialog::OnLeft() 
{
	if(m_pItemPreviews == NULL || m_nResultItems <= 1 || m_pItemPreviews->GetSize () < 1)
		return;

	if (m_thumbsize == thumbsizeLarge)
	{
		OnUp();
		return;
	}

	int nCollID;
	DWORD dwItemNumber;
	BOOL fSelectOK = FALSE;

	if (m_nCurPreviewItem - 1 < 0)
	{
		// user is trying to back up beyond current visible items
		if (m_nFirstRow > 0)
		{
			CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_nCurPreviewItem);
			if (IsItemOKToView(pItem))
			{
				int nPrevResult = pItem->m_nResultIndex - 1;
				CPMWCollectionResult Result = m_SearchContext.GetSearchResult(nPrevResult);
				// Get the collection ID and the item number for the item
				nCollID = Result.GetCollectionID();
				dwItemNumber = Result.GetItemNumber();

				// we need to scroll back 1 row
				m_nFirstRow--;
				UpdateScrollBar();
				UpdateItemPreviews(FALSE);
				InvalidateRect(m_crPreview);
				
				fSelectOK = TRUE;
			}
		}
	}
	else
	{
		// move back 1 in visible items
		CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_nCurPreviewItem - 1);
		if (IsItemOKToView(pItem))
		{
			nCollID = pItem->m_uCollectionID;
			dwItemNumber = pItem->m_dwItemNumber;
			fSelectOK = TRUE;
		}
	}
	
	if (fSelectOK)
	{
		SelectItem(nCollID, dwItemNumber);
		CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_nCurPreviewItem);
		m_nCurColumn = CalcColumnFromItem(pItem);
	}
}

void CBrowserDialog::OnRight() 
{
	if(m_pItemPreviews == NULL || m_nResultItems <= 1 || m_pItemPreviews->GetSize () < 1)
		return;

	if (m_thumbsize == thumbsizeLarge)
	{
		OnDown();
		return;
	}

	int nNewSelection = m_nCurPreviewItem + 1;
	if (nNewSelection < m_pItemPreviews->GetSize())
	{
		CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(nNewSelection);
		if (IsItemOKToView(pItem))
		{
			int nResultIndex = pItem->m_nResultIndex;

			int nNewRow;
			if (m_fHasGroups)
				nNewRow = CalcRowFromItemNumber(nResultIndex);
			else
				nNewRow = nResultIndex / m_ThumbnailLayout.cx;

			if (nNewRow >= m_nFirstRow + m_nFullThumbnailRows)
				OnVScroll(SB_LINEDOWN, NULL, NULL);

			SelectItem(pItem->m_uCollectionID, pItem->m_dwItemNumber);
			m_nCurColumn = CalcColumnFromItem(pItem);
		}
	}
}

void CBrowserDialog::OnHome() 
{
	if (m_pItemPreviews == NULL || m_nResultItems <= 1)
		return;

	OnVScroll(SB_TOP, NULL, NULL);
	if (m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
	{
		CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(0);
		if (IsItemOKToView(pItem))
		{
			SelectItem(pItem->m_uCollectionID, pItem->m_dwItemNumber);
			m_nCurColumn = 0;
		}
	}
}

void CBrowserDialog::OnEnd() 
{
	if (m_pItemPreviews == NULL || m_nResultItems <= 1)
		return;

	OnVScroll(SB_BOTTOM, NULL, NULL);
	if (m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
	{
		CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_pItemPreviews->GetSize() - 1);
		if(IsItemOKToView(pItem))
		{
			SelectItem(pItem->m_uCollectionID, pItem->m_dwItemNumber);
			m_nCurColumn = CalcColumnFromItem(pItem);
		}
	}
}

void CBrowserDialog::OnDelete() 
{
	// Look up the collection by its ID.
	CPMWCollection* pCollection = m_pCollectionManager->FindCollection(m_uSelectedCollectionID, CollectionType(), CollectionSubType());
	if (pCollection == NULL)
	{
		ASSERT(0);
		return;
	}

	CString csMsg;

	if (!pCollection->IsUserCollection())
	{
		csMsg.LoadString(IDS_NO_DELETE);
		AfxMessageBox(csMsg);
		return;
	}

	int nRetVal;

	CString csCollection;
	CString csFile;
	CString csFriendlyName;
	CString csFinalName;

	if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber,
								  csCollection, csFile, csFriendlyName, csFinalName))
	{
		CString csFileName = m_pPathManager->ExpandPath(csFile);
		BOOL fProjects = (CollectionType() == CPMWCollection::typeProjects);
		CConfirmFileRemove dlgConfirm(fProjects, csFriendlyName, csFileName, FALSE);
		if (dlgConfirm.DoModal() == IDYES)
		{
			TRY
			{
				nRetVal = pCollection->Delete(m_dwSelectedItemNumber);
			}
			CATCH_ALL(e)
			{
				ASSERT(FALSE);
				nRetVal = ERRORCODE_Fail;
			}
			END_CATCH_ALL

			if (nRetVal == ERRORCODE_None)
			{
				// We deleted the item from the browser.
				// See if user wants to delete the file as well.
				if (dlgConfirm.DeleteChecked())
				{
					TRY
					{
						CFile::Remove(csFileName);
					}
					CATCH_ALL(e)
					{
						CString csFormat;
						if (fProjects)
							csFormat.LoadString(IDS_PROJ_NO_DELETE);
						else
							csFormat.LoadString(IDS_PICT_NO_DELETE);

						csMsg.Format(csFormat, csFileName);
						AfxMessageBox(csMsg, MB_OK);
					}
					END_CATCH_ALL
				}
			}
			else
			{
				csMsg.LoadString(IDS_ERROR_DELETE);
				AfxMessageBox(csMsg);
			}

			// Do it always just in case.
			UpdateAllBrowsers();
		}
	}
}

void CBrowserDialog::OnUpdateDelete(CCmdUI *pCmdUI)
{
   BOOL fEnable = FALSE;

   // Look up the collection by its ID.
   CPMWCollection* pCollection = m_pCollectionManager->FindCollection(m_uSelectedCollectionID, CollectionType(), CollectionSubType());
   if (pCollection != NULL)
   {
      fEnable = pCollection->IsUserCollection();
   }

   pCmdUI->Enable(fEnable);
}

void CBrowserDialog::OnUpdateSize(CCmdUI *pCmdUI)
{
   CString  strSize;

   pCmdUI->Enable();
   switch(m_thumbsize)
   {
      case thumbsizeSmall:
         strSize.LoadString(IDS_BROWSER_MENUITEM_SMALL);
         break;
      case thumbsizeMedium:
         strSize.LoadString(IDS_BROWSER_MENUITEM_MEDIUM);
         break;
      case thumbsizeLarge:
         strSize.LoadString(IDS_BROWSER_MENUITEM_LARGE);
         break;
      default:
         ASSERT(0);
         break;
   }
   pCmdUI->SetText(strSize);
}

void CBrowserDialog::OnUpdatePage(CCmdUI *pCmdUI)
{
   int            nCurPos;
   long           lCurPage=0,  lNumPages=0;
   CString        strText, strCurPage;
   long           lResultCount = m_SearchContext.GetResultCount();

   pCmdUI->Enable();
   if(lResultCount > 0)
   {
      nCurPos = GetScrollPos();
      if(m_nFullThumbnailRows)
         lCurPage = m_nFirstRow / m_nFullThumbnailRows + 1;
      if(m_nFullThumbnailRows)
         lNumPages = (m_nItemRows + m_nFullThumbnailRows - 1)
            / m_nFullThumbnailRows;
   }
   strCurPage.LoadString(IDS_BROWSER_CUR_PAGE);
   strText.Format(strCurPage, lCurPage, lNumPages);
   pCmdUI->SetText(strText);
}

void CBrowserDialog::OnUpdateItem(CCmdUI *pCmdUI)
{
	if(m_pItemPreviews == NULL)
		return;

	int nStartResult;
	if (m_fHasGroups)
		nStartResult = CalcGroupStartingItem(m_nFirstRow);
	else
		nStartResult = m_nFirstRow * m_ThumbnailLayout.cx;

	int nItem = m_pItemPreviews->Find(m_uSelectedCollectionID, m_dwSelectedItemNumber);
	int nCurItem, nResultCount;
	CString  strText, strCurItem;

	pCmdUI->Enable();
	nResultCount = m_SearchContext.GetResultCount();
	if(nItem == -1 || nResultCount == 0)
		nCurItem = -1;
	else
		nCurItem = nStartResult + nItem;

	strCurItem.LoadString(IDS_BROWSER_CUR_ITEM);
	strText.Format(strCurItem, nCurItem+1, nResultCount);
	pCmdUI->SetText(strText);
}

void CBrowserDialog::OnChangeSize()
{
	if (m_fEditingTitle)
		OnKillfocusEditTitle();

   CRect   controlRect;
   CMenu   menuPopup;
   int     nIndex;

   // Get location of this control
   nIndex = m_StatusBar.CommandToIndex(ID_BROWSER_INDICATOR_SIZE);
   ASSERT(nIndex >= 0);
   if(nIndex < 0 || m_SearchContext.GetResultCount() <= 0)
      return;

   menuPopup.CreatePopupMenu();

   menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_SMALL,
            "View Small");
   menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_MEDIUM,
            "View Medium");
   menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_LARGE,
            "View Large");
         
	CPoint pt;
	GetCursorPos(&pt);
   menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
   menuPopup.DestroyMenu();  
}

void CBrowserDialog::OnChangePage()
{
}

// The user clicked the "special collection" button.
void CBrowserDialog::OnSpecialCollection()
{
	TRACE("Show Special Collection |%s|\n", m_csSpecialCollection);
   BeginWaitCursor(); // display the hourglass cursor
   
   m_fIgnoreSelectionChange = TRUE;
   m_fUpdatePreview = FALSE;

   CString csAll(BrowserAllString());

   if (m_pCollectionList != NULL)
      m_pCollectionList->SetCurSel(m_csSpecialCollection);
   if (m_pGraphicTypeList != NULL)
      m_pGraphicTypeList->SetCurSel(csAll);
   if (m_pProjectTypeList != NULL)
      m_pProjectTypeList->SetCurSel(csAll);
   if (m_pCategoryList != NULL)
      m_pCategoryList->SetCurSel(csAll);
   if (m_pToneList != NULL)
      m_pToneList->SetCurSel(csAll);

   CComboBox*  pKeywordList = (CComboBox*)GetDlgItem(IDC_KEYWORD_LIST);
   if (pKeywordList != NULL)
      pKeywordList->SetCurSel(-1);
   
   m_fIgnoreSelectionChange = FALSE;
   m_fUpdatePreview = TRUE;

   // Get all re - done, even if collection didn't change
   CollectionChanged();

   EndWaitCursor(); // remove the hourglass cursor
}

void CBrowserDialog::OnOnlineArt()
{
	EndDialog(ID_ONLINE_ART);
}

// The following CmdUI functions are to enable and diables various user interface
// elements as needed.
LRESULT CBrowserDialog::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	if (lParam <= 0)
	{
		m_StatusBar.SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE);

		// start our Preview draw
		Refresh();
	}

   return AfxGetApp()->OnIdle(lParam);
}

void CBrowserDialog::OnUpdateViewSmall(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_thumbsize == thumbsizeSmall);
}

void CBrowserDialog::OnUpdateViewMedium(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_thumbsize == thumbsizeMedium);
}

void CBrowserDialog::OnUpdateViewLarge(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_thumbsize == thumbsizeLarge);
}

BOOL CBrowserDialog::SupportsThumbnailSize(ThumbnailSizes thumbsizeNew)
{
   return TRUE;   // Default to supporting all sizes
}

ERRORCODE CBrowserDialog::SetThumbnailView(ThumbnailSizes thumbsizeNew)
   {
      ERRORCODE   errorcode = ERRORCODE_Fail;
      if(!SupportsThumbnailSize(thumbsizeNew))
         return errorcode;

      switch(thumbsizeNew)
      {
         case thumbsizeSmall:
            OnViewSmall();
            errorcode = ERRORCODE_None;
            break;
         case thumbsizeMedium:
            OnViewMedium();
            errorcode = ERRORCODE_None;
            break;
         case thumbsizeLarge:
            OnViewLarge();
            errorcode = ERRORCODE_None;
            break;
         default:
            ASSERT(0);  // Unknown type
            break;
      }
      return errorcode;
   }

void CBrowserDialog::OnViewMedium() 
{
   m_ThumbnailSize = m_MediumThumbnailSize;
   m_thumbsize = thumbsizeMedium;

   // Recompute the positions of things given the new size.
   ComputeLayout();

   GotoItem(m_uSelectedCollectionID, m_dwSelectedItemNumber);
}

void CBrowserDialog::OnViewSmall() 
{
//   m_ThumbnailSize.cx = m_MediumThumbnailSize.cx/2;
//   m_ThumbnailSize.cy = m_MediumThumbnailSize.cy/2;
   m_ThumbnailSize.cx = 80;
   m_ThumbnailSize.cy = 80;
   m_thumbsize = thumbsizeSmall;

   // Recompute the positions of things given the new size.
   ComputeLayout();

   GotoItem(m_uSelectedCollectionID, m_dwSelectedItemNumber);
}

void CBrowserDialog::OnViewLarge() 
{

   if (m_nResultItems > 0 && m_nCurPreviewItem >=0 &&
       m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
   {
		if (!SelectedItemIsLicensed())
		{
			OnClickedUnlicensed();
			return;
		}
	}

   m_thumbsize = thumbsizeLarge;
   // Recompute the positions of things given the new size.
   ComputeLayout();

   GotoItem(m_uSelectedCollectionID, m_dwSelectedItemNumber);
}

void CBrowserDialog::UpdatePreview(BOOL bGotoCurrentPreviewItem)
{
   // Recompute the positions of things given the new size.
   ComputeLayout();

   // Fit the item previews into the new layout.
   UpdateItemPreviews(FALSE);

   // Update the scroll bar.
   UpdateScrollBar();

   if(m_pItemPreviews != NULL && m_pItemPreviews->GetSize() > 0)
   {
      if(m_uSelectedCollectionID != (USHORT)-1 && m_dwSelectedItemNumber != (DWORD)-1)
      {
         if(bGotoCurrentPreviewItem)
            GotoItem(m_uSelectedCollectionID , m_dwSelectedItemNumber, FALSE);
      }
      else
      {
         // Fit the item previews into the new layout.
         UpdateItemPreviews(FALSE);

         // Update the scroll bar.
         UpdateScrollBar();

         CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(0);
         SelectItem(pItem->m_uCollectionID, pItem->m_dwItemNumber);
      }
   }
}

void CBrowserDialog::OnExport()
{
   CString csCollection;
   CString csFile;
   CString csFriendlyName;
   CString csAccessName;

   if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber, csCollection, csFile, csFriendlyName, csAccessName))
   {
      // Construct a name to use.
      // If this is the first time, use the "user" directory.
      // Otherwise, use the last directory the user saved to.

      // Get the default directory to use.
      CString csSaveDir;
      GetExportDirectory(csSaveDir);
      csSaveDir = m_pPathManager->ExpandPath(csSaveDir);

      // Strip the base name off the file.
      CString csBaseName;
      Util::SplitPath(csFile, NULL, &csBaseName);

      CFileDialog Dialog(FALSE,
                         NULL,     // No default extension.
                         csBaseName,
                         OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                         GET_PMWAPP()->GetResourceStringPointer(IDS_ExportFilters));

      // Set the initial directory.
      Util::RemoveBackslashFromPath(csSaveDir);
      Dialog.m_ofn.lpstrInitialDir = csSaveDir;
      Dialog.m_ofn.lpstrTitle = GET_PMWAPP()->GetResourceStringPointer(IDS_Export);

      if (Dialog.DoModal() == IDOK)
      {
         CBrowserProgressDialog ProgressDialog(IDD_BROWSER_EXPORT_PROGRESS, 1, this);
         CompositeFileName cfn(csAccessName);
         CString csBaseName;
         Util::SplitPath(cfn.get_file_name(), NULL, &csBaseName);
         ProgressDialog.SetPosition(0, csBaseName);
         
         // They said to do it!

         // Remember this name for next time.
         SetLastExportName(Dialog.GetPathName());

         // Copy the data to the destination file.
         ReadOnlyFile SourceFile(csAccessName);
         StorageFile DestFile(Dialog.GetPathName());
         ERRORCODE error = copy_file(&SourceFile, &DestFile);

         ProgressDialog.SetPosition(1, "");

         if (error != ERRORCODE_None)
         {
            // Get rid of the destination file;
            DestFile.zap();

            // Tell the user what happened.
            LPCSTR pFormat = GET_PMWAPP()->GetResourceStringPointer(IDS_ErrCreateFile);
            CString csMessage;
            csMessage.Format(pFormat, (LPCSTR)DestFile.get_name());
            AfxMessageBox(csMessage);
         }
      }
   }
}

void CBrowserDialog::OnUpdateExport(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(SelectedItemIsLicensed());
}

BOOL CBrowserDialog::SelectedItemIsLicensed(void)
{
   BOOL bLicensed = FALSE;
   if (m_nResultItems > 0 && m_nCurPreviewItem >=0 &&
       m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
   {
		ASSERT(m_nCurPreviewItem < m_pItemPreviews->GetSize());
      CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_nCurPreviewItem);
      ASSERT(pItem != NULL);
      if (pItem != NULL && pItem->m_bIsLicensed)
      {
         bLicensed = TRUE;
      }
   }
   return bLicensed;
}

BOOL CBrowserDialog::IsItemOKToView(CPreviewArray::Item* pItem)
{

	BOOL fAnswer = FALSE;

   if(m_thumbsize == thumbsizeLarge)
	{
		if(pItem)
		{
			if(IsItemLicensed(pItem))
			{
				  fAnswer = TRUE;
			}
		}
	}
	else
	{
		if(pItem)
			fAnswer = TRUE;
	}
	
	if(fAnswer == FALSE)
		SetThumbnailView(thumbsizeMedium);

	return fAnswer;

}


BOOL CBrowserDialog::IsItemLicensed(CPreviewArray::Item* pItem)
{
   BOOL bLicensed = FALSE;
   if (m_nResultItems > 0 && m_nCurPreviewItem >=0 &&
       m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
   {
		ASSERT(m_nCurPreviewItem < m_pItemPreviews->GetSize());
      //CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(m_nCurPreviewItem);
      ASSERT(pItem != NULL);
      if (pItem != NULL && pItem->m_bIsLicensed)
      {
         bLicensed = TRUE;
      }
   }
   return bLicensed;
}

BOOL CArtBrowserDialog::CopyFileToUserArtDir(LPCSTR szOrigFileName, CString& csNewFilename,
														BOOL fPromptOverwrite, int* overwriteRetval)
{
	// copy file to user's Art directory
	CString drive, dir, name, ext;
	Util::SplitPath(szOrigFileName, &drive, &dir, &name, &ext);
	csNewFilename = m_pPathManager->ExpandPath("[[U]]\\ART\\");
	csNewFilename += name + ext;

	// make sure file doesn't already exist in Art directory
	BOOL fDoCopy = TRUE;
	*overwriteRetval = IDOK;
	if (fPromptOverwrite && _access(csNewFilename, 0) == 0)
	{
		CString str;
		str.Format(IDS_WARNING_OVERWRITE_ART, name + ext);
		CMsgBoxYesAll dlg(str);
		*overwriteRetval = dlg.DoModal();
		if (*overwriteRetval == IDNO || *overwriteRetval == IDCANCEL)
			fDoCopy = FALSE;
	}

	if (fDoCopy)
	{
		ReadOnlyFile orig_file(szOrigFileName);
		StorageFile new_file(csNewFilename);
		return (copy_file(&orig_file, &new_file) == ERRORCODE_None);
	}

	return TRUE;
}

void CBrowserDialog::OnImport()
{
   CPMWCollection* pCollection = m_pCollectionManager->GetUserCollection(CollectionType());

   if (pCollection == NULL)
   {
		ASSERT(0);
		return;
	}

   // We want to do the import. Create the dialog to use.
   CImportDlg* pDialog = NULL;
   CString csFilters;
   switch (CollectionType())
   {
      case CPMWCollection::typeProjects:
      {
         // Get the filter string from the configuration.
         GetConfiguration()->DocumentFilter(csFilters);
         pDialog = new CImportProjectDlg(NULL, NULL, csFilters, this);
         break;
      }
      case CPMWCollection::typeArt:
      {
         // Get our filter string.
         CString csFilters2;
         TRY
         {
            LoadConfigurationString(IDS_PICTURE_FILTERS_PART_1, csFilters);
            LoadConfigurationString(IDS_PICTURE_FILTERS_PART_2, csFilters2);
            csFilters += csFilters2;
         }
         CATCH (CMemoryException, e)
         {
            return;
         }
         END_CATCH

         pDialog = new CImportPictureDlg(NULL, NULL, csFilters, this);
         break;
      }
      default:
      {
         // Unhandled case!
         ASSERT(FALSE);
         break;
      }
   }

	if (pDialog && pDialog->DoModal() == IDOK)
	{
		// get all the filenames they selected
		POSITION pos = pDialog->GetStartPosition();
		CStringArray files;
		while (pos != NULL)
		{
			CString file = pDialog->GetNextPathName(pos);
			files.Add(file);
		}

		int nFiles = files.GetSize();
		if (nFiles > 0)
		{
			CBrowserProgressDialog ProgressDialog(IDD_BROWSER_IMPORT_PROGRESS,
															  nFiles, this,
															  IDS_CONFIRM_CANCEL_BROWSER_IMPORT);

			// Do the import!
			BOOL fPromptForOverwrite = TRUE; // used for overwriting of local files
			for (int nItem = 0; nItem < nFiles; nItem++)
			{
				// Import the next one.
				CString csFileName = files[nItem];

				// Update the progress dialog.
				CompositeFileName cfn(csFileName);
				CString csBaseName;
				Util::SplitPath(cfn.get_file_name(), NULL, &csBaseName);
				ProgressDialog.SetPosition(nItem, csBaseName);
				if (ProgressDialog.CheckForAbort())
				{
					break;
				}

				int fDoImport = TRUE;

				// Do we need to do a Local Copy? (importing art only)
				if (CollectionType() == CPMWCollection::typeArt &&
					((CImportPictureDlg*)pDialog)->m_fCopyLocal)
				{
					CString newFileName;
					int overwriteRetVal;
					if ((fDoImport =
						((CArtBrowserDialog*)this)->CopyFileToUserArtDir(csFileName, newFileName,
																	  fPromptForOverwrite,
																	  &overwriteRetVal)))
					{
						csFileName = newFileName;
					}

					// see what needs to be done next
					if (overwriteRetVal == IDYESALL)
						fPromptForOverwrite = FALSE;
					else if (overwriteRetVal == IDNO)
						fDoImport = FALSE;	// go on to next one
					else if (overwriteRetVal == IDCANCEL)
						break;		// we're out 'o here
				}
				
				if (fDoImport)
					pCollection->ImportItem(csFileName, NULL, NULL, pDialog->m_strCategory);
			}

			// Make it look like we went to 100% (or wherever we stopped).
			ProgressDialog.SetPosition(nItem, "");

			UpdateAllBrowsers();
		}
	}

   // Get rid of the dialog.
   delete pDialog;

}

void CBrowserDialog::OnUpdateImport(CCmdUI *pCmdUI)
{
   // We can only import if we have a user collection of the correct type.
   pCmdUI->Enable(m_pCollectionManager->GetUserCollection(CollectionType()) != NULL);
}

void CBrowserDialog::OnProperties()
{
   CPMWCollection* pCollection = m_pCollectionManager->FindCollection((BYTE)m_uSelectedCollectionID, CollectionType(), CollectionSubType());
   if (pCollection != NULL)
   {
      CString csCollection;
      CString csFile;
      CString csFriendlyName;
      CString csPath;
      CString csKeywords;

      // Get the scoop on the selected item.
      if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber, csCollection, csFile, csFriendlyName, csPath, &csKeywords))
      {
         UINT uDialog = pCollection->IsUserCollection()
                              ? IDD_BROWSER_USER_PROPERTIES
                              : IDD_BROWSER_READONLY_PROPERTIES;
         CBrowserPropertiesDialog Dialog(uDialog, this);

         CompositeFileName cfn(csFile);
			CString csFileName = cfn.get_file_name();
			if (pCollection->IsUserCollection())
				csFileName = m_pPathManager->ExpandPath(csFileName);
         Dialog.m_csFileName = csFileName;
         Dialog.m_csFriendlyName = csFriendlyName;
         Dialog.m_csKeywords = csKeywords;

#ifdef LOCALIZE
	   Dialog.m_csFriendlyName = CurrentLocale.HandleBrowserCaption(m_csINISection, csFriendlyName, Dialog.m_csFileName);
#endif

         if (Dialog.DoModal() == IDOK)
         {
            if (Dialog.m_csFriendlyName != csFriendlyName
                  || Dialog.m_csKeywords != csKeywords)
            {
               // Something has changed. Update the item.
               ASSERT(pCollection->IsUserCollection());

               CPMWCollectionData Data;

               Data.m_strContentFileName = csFile;
// FF: DWORD m_dwContentTimeStamp was phased out starting in PM 5.0
//					Data.m_dwContentTimeStamp = Util::ComputeTimeStamp(CString(m_pPathManager->ExpandPath(csFile)));
               Data.m_dwUnused = 0;
               Data.m_strFriendlyName = Dialog.m_csFriendlyName;
               Data.m_strKeywords = Dialog.m_csKeywords;
               Data.m_ContentData.Empty();      // CContentDataEntry
               CCategoryDataItem cdItem;
               cdItem.Empty();
               Data.m_CategoryData.Set(CString(""), CString(""), &cdItem);    // CCategoryData

               // Update existing item.
               pCollection->UpdateItemInfo(m_dwSelectedItemNumber, &Data);

					UpdateItemPreviews(TRUE);
					InvalidateRect(m_crPreview);
            }
         }
      }
   }
}

void CBrowserDialog::OnUpdateProperties(CCmdUI *pCmdUI)
{
}


/////////////////////////////////////////////////////////////////////////////
// CAGProjectBrowserDialog


CAGProjectBrowserDialog::CAGProjectBrowserDialog(CWnd* pParent)
   : CProjectBrowserDialog(CAGProjectBrowserDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CProjectBrowserDialog)
   //}}AFX_DATA_INIT
	m_bAgProduct = TRUE;
	m_strBkgdImageName = "ProjBrowserBkgnd.png";
	m_fTileBkgdImage = FALSE;


}

void CAGProjectBrowserDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAGProjectBrowserDialog)
	DDX_Control(pDX, IDC_PREVIEW_SCROLL, m_ctrlScrollBar); 
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAGProjectBrowserDialog, CProjectBrowserDialog)
   //{{AFX_MSG_MAP(CAGProjectBrowserDialog)
	ON_WM_PAINT()
   ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAGProjectBrowserDialog message handlers

BOOL CAGProjectBrowserDialog::OnInitDialog() 
{
   INHERITED::OnInitDialog();

	CWnd* pWnd = GetDlgItem(IDC_FIND_ANY);
	pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDC_FIND_ALL);
	pWnd->ShowWindow(SW_HIDE);
	m_pCategoryList->ShowWindow(SW_HIDE);
	return TRUE;
}

// The following CmdUI functions are to enable and diables various user interface
// elements as needed.
LRESULT CAGProjectBrowserDialog::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	if (lParam <= 0)
	{
		// start our Preview draw
		Refresh();
	}

   return AfxGetApp()->OnIdle(lParam);
}

void CAGProjectBrowserDialog::SetPreviewRect()
{
   GetClientRect(m_crClient);

   // Compute the sidebar rect. This is the area on the left of the dialog with
	// controls in it. The dialog template contains a control (IDC_SIDEBAR_BOUNDS)
	// that determines the dividing line between the sidebar and the preview areas.
   m_crSidebar.SetRectEmpty();
   CWnd* pSidebar = GetDlgItem(IDC_SIDEBAR_BOUNDS);
   if (pSidebar != NULL)
   {
      pSidebar->GetWindowRect(m_crSidebar);
      ScreenToClient(m_crSidebar);
   }
	m_crSidebar.left = 0;
	m_crSidebar.top = 1;
	m_crSidebar.bottom = m_crClient.bottom;

	CRect rcBottomBar;
   CWnd* pBottomBar = GetDlgItem(IDC_BOTTOM_BOUNDS);
   if (pBottomBar != NULL)
   {
      pBottomBar->GetWindowRect(rcBottomBar);
      ScreenToClient(rcBottomBar);
   }

	CRect rcScroll;
	m_ctrlScrollBar.GetWindowRect(rcScroll);
	ScreenToClient(rcScroll);

   // Compute the preview area -- it's what's left over.
	m_crPreview.top = 7;
   m_crPreview.left = m_crSidebar.right + 1;
	m_crPreview.bottom = rcBottomBar.top;
	m_crPreview.right = rcScroll.left;

   m_crMinimum.left = 0;
   m_crMinimum.top = 0;
   m_crMinimum.bottom = rcBottomBar.bottom;
   m_crMinimum.right = m_crSidebar.right;

}
	

// Paint all the special backgound colors, textures, borders,  etc.
void CAGProjectBrowserDialog::PaintBkgnd(CDC* pDC) 
{

}

void CAGProjectBrowserDialog::UpdateScrollBar()
{
   int nScrollMin = 0;
   int nScrollMax = 0;
   int nScrollPos = 0;
   int nDesiredScrollMax;

   // Get the number of search results.
   if (m_nResultItems > 0)
   {
      // Number of rows that can be scrolled off the top of the preview area
      // with the last full row still filled.
		m_nItemRows = CalcItemRows();

      nDesiredScrollMax = m_nItemRows - m_nFullThumbnailRows;
      if (nDesiredScrollMax < 0)
      {
         nDesiredScrollMax = 0;
      }
      if(nDesiredScrollMax > 32768)
      {
         m_nScrollInc = nDesiredScrollMax / 32768 + 1;
         nScrollMax = nDesiredScrollMax / m_nScrollInc;
      }
      else
      {
         m_nScrollInc = 1;
         nScrollMax = nDesiredScrollMax;
      }

      // Make sure 1st preview row is not negative.
      if (m_nFirstRow < nScrollMin)
         m_nFirstRow = nScrollMin;

      nScrollPos = m_nFirstRow / m_nScrollInc;
   }

   // Set new scroll values.
   if (nScrollMin < nScrollMax)
   {
      // Scrolling is enabled.
//      m_ctrlScrollBar.EnableScrollBarCtrl(SB_VERT, TRUE);
      m_ctrlScrollBar.SetScrollRange(nScrollMin, nScrollMax, FALSE);
      m_ctrlScrollBar.SetScrollPos(nScrollPos);
   }
   else
   {
      // Turn off scrolling.
      m_ctrlScrollBar.SetScrollRange(0, 0, FALSE);
      m_ctrlScrollBar.SetScrollPos(0);
//      m_ctrlScrollBar.EnableScrollBarCtrl(SB_VERT, FALSE);
   }

}

void CAGProjectBrowserDialog::PositionControls()
{

}

void CAGProjectBrowserDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawBkgnd(&dc);
   PaintPreview(&dc);
}


/////////////////////////////////////////////////////////////////////////////
// CProjectBrowserDialog

// Clipboard formats the project browser exports.
UINT CProjectBrowserDialog::m_cfDataExchange = ::RegisterClipboardFormat("PMG 4.0 Project Browser");
CString CProjectBrowserDialog::m_csLastExport;

CProjectBrowserDialog::CProjectBrowserDialog( UINT nID/* = 0*/, CWnd* pParent)
: CBrowserDialog((nID) ? nID : CProjectBrowserDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CProjectBrowserDialog)
   //}}AFX_DATA_INIT

   m_csINISection = "ProjectBrowser";

	// Override default colors.
   LoadColorsFromINI("ProjectBrowserColors");

   m_nPopupMenuIndex = 1;

	m_fHasGroups = TRUE;
	m_fShowHeadings = TRUE;
}

void CProjectBrowserDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CProjectBrowserDialog)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProjectBrowserDialog, CBrowserDialog)
   //{{AFX_MSG_MAP(CProjectBrowserDialog)
   ON_COMMAND(ID_FILE_OPENFROMDISK, OnFileOpenfromdisk)
   ON_COMMAND(ID_FILE_CLOSEBROWSER, OnFileClosebrowser)
   ON_UPDATE_COMMAND_UI(ID_BROWSER_INDICATOR_VIEW, OnUpdateView)
   ON_COMMAND(ID_BROWSER_INDICATOR_VIEW, OnChangeView)
   ON_COMMAND(IDC_BROWSER_VIEW_CARD_FRONT,  OnViewCardFront)
   ON_COMMAND(IDC_BROWSER_VIEW_CARD_INSIDE, OnViewCardInside)
   ON_COMMAND(IDC_BROWSER_VIEW_CARD_BACK,   OnViewCardBack)
   ON_COMMAND(IDC_BROWSER_VIEW_BROCHURE_OUTSIDE, OnViewBrochureOutside)
   ON_COMMAND(IDC_BROWSER_VIEW_BROCHURE_INSIDE, OnViewBrochureInside)
   ON_COMMAND(IDC_BROWSER_VIEW_NEWSLETTER_GOTO, OnViewNewsLetterGoto)
   ON_COMMAND(IDC_BROWSER_VIEW_NEWSLETTER_NEXT, OnViewNewsLetterNext)
   ON_COMMAND(IDC_BROWSER_VIEW_NEWSLETTER_PREV, OnViewNewsLetterPrev)
   ON_COMMAND(IDC_BROWSER_VIEW_NEWSLETTER_FIRST, OnViewNewsLetterFirst)
   ON_COMMAND(IDC_BROWSER_VIEW_NEWSLETTER_LAST, OnViewNewsLetterLast)
	ON_COMMAND(ID_ONLINE_UPDATEACCOUNT, OnOnlineUpdateAccount)
	ON_COMMAND(ID_ONLINE_CREATEACCOUNT, OnOnlineCreateAccount)
	ON_COMMAND(ID_ONLINE_CHANGE_ACCOUNTS, OnOnlineChangeAccounts)
	ON_COMMAND(ID_ONLINE_DELETE_ALL_DOWNLOADED_FILES, OnOnlineDeleteAllDownloadedFiles)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CProjectBrowserDialog::OnUpdateView(CCmdUI *pCmdUI)
{
   BOOL           bEnable;
   CPmwDoc        *pCurrentDoc = m_CollectionDraw.GetDocument();
   CString        strResource;
   CString        strIndicatorText=" ";   // Must be initialized to space
                                          // due to bug in CStatusBar

   if(pCurrentDoc && m_thumbsize == thumbsizeLarge)
   {
      int            nCurrentPanel;
      long           lNumberOfPages;
      PROJECT_TYPE   projectType;
      CMlsStringArray csaMenuItems;

      bEnable = TRUE;
      lNumberOfPages = pCurrentDoc->NumberOfPages();
      projectType = pCurrentDoc->get_project_type();
      switch(projectType)
      {
         case PROJECT_TYPE_Card:
         case PROJECT_TYPE_HalfCard:
         case PROJECT_TYPE_NoteCard:
         csaMenuItems.SetNames(IDS_CARD_PANEL_NAMES);
         nCurrentPanel = pCurrentDoc->get_current_panel();
         ASSERT(nCurrentPanel >= 0);
         if(nCurrentPanel >= 0)
         {
            csaMenuItems.SetNames(IDS_CARD_PANEL_NAMES);
            ASSERT(csaMenuItems.GetSize() == 3);
            if(csaMenuItems.GetSize() == 3)
            {
               strIndicatorText = csaMenuItems[nCurrentPanel];
            }
         }
         break;

			case PROJECT_TYPE_PostCard:
         lNumberOfPages = pCurrentDoc->NumberOfPages();
         ASSERT(lNumberOfPages == 2);
         if(lNumberOfPages > 0)
         {
            csaMenuItems.SetNames(IDS_POSTCARD_PANEL_NAMES);
            ASSERT(csaMenuItems.GetSize() == 2);
            if(csaMenuItems.GetSize() == 2)
            {
					DWORD dwIndex = pCurrentDoc->CurrentPageIndex();
               ASSERT(dwIndex >= 0L && dwIndex < 2L);
               ASSERT(dwIndex <= 0x7fff);
               if(dwIndex <= 0x7fff)
                  strIndicatorText = csaMenuItems[(int)dwIndex];
            }
         }
         break;

         case PROJECT_TYPE_Brochure:
         lNumberOfPages = pCurrentDoc->NumberOfPages();
         ASSERT(lNumberOfPages == 2);
         if(lNumberOfPages > 0)
         {
            csaMenuItems.SetNames(IDS_BROCHURE_PANEL_NAMES);
            ASSERT(csaMenuItems.GetSize() == 2);
            if(csaMenuItems.GetSize() == 2)
            {
					DWORD dwIndex = pCurrentDoc->CurrentPageIndex();
               ASSERT(dwIndex >= 0L && dwIndex < 2L);
               ASSERT(dwIndex <= 0x7fff);
               if(dwIndex <= 0x7fff)
                  strIndicatorText = csaMenuItems[(int)dwIndex];
            }
         }
         break;

			case PROJECT_TYPE_Craft:
				if (pCurrentDoc->NumberOfPages() == 1)
					break;
				// else fall through...
         case PROJECT_TYPE_Calendar:
			case PROJECT_TYPE_Newsletter:
         strResource.LoadString(IDS_BROWSER_INDICATOR_NEWSLETTER_PAGE);
         ASSERT(!strResource.IsEmpty());
         // Resource contains Sheet %ld of %ld
         strIndicatorText.Format(strResource,
            pCurrentDoc->CurrentPageIndex() + 1,
            pCurrentDoc->NumberOfPages());
         break;
      }
   }                                    
   else
      bEnable = FALSE;

   pCmdUI->SetText(strIndicatorText);
   pCmdUI->Enable(bEnable);
}

void CProjectBrowserDialog::OnChangeView()
{
   int         nIndex;
   long        lNumberOfPages;
   UINT        nMenuFlags;
   CRect       controlRect;
   CMenu       menuPopup;

   nIndex = m_StatusBar.CommandToIndex(ID_BROWSER_INDICATOR_VIEW);
   CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();
   ASSERT(nIndex >= 0);
   if(pCurrentDoc == NULL || nIndex < 0)
      return;

	CPoint pt;
	GetCursorPos(&pt);

   PROJECT_TYPE projectType = pCurrentDoc->get_project_type();
   lNumberOfPages = pCurrentDoc->NumberOfPages();
   CMlsStringArray csaMenuItems;
   CString csText;
   switch(projectType)
   {
      case PROJECT_TYPE_Card:
      case PROJECT_TYPE_HalfCard:
      case PROJECT_TYPE_NoteCard:
      TRY
      {
         csaMenuItems.SetNames(IDS_CARD_PANEL_NAMES);
         ASSERT(csaMenuItems.GetSize() == 3);

         menuPopup.CreatePopupMenu();
         menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_CARD_FRONT,
                  csaMenuItems[CARD_PANEL_Front]);
         menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_CARD_INSIDE,
                  csaMenuItems[CARD_PANEL_Inside]);
         menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_CARD_BACK,
                  csaMenuItems[CARD_PANEL_Back]);
         menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
         menuPopup.DestroyMenu();  
      }
      END_TRY
      break;
      
		case PROJECT_TYPE_PostCard:
      csaMenuItems.SetNames(IDS_POSTCARD_PANEL_NAMES);
      ASSERT(csaMenuItems.GetSize() == 2);
      menuPopup.CreatePopupMenu();
      menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_BROCHURE_OUTSIDE,
               csaMenuItems[0]);
      menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_BROCHURE_INSIDE,
               csaMenuItems[1]);
      menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
      menuPopup.DestroyMenu();  
      break;
      
		case PROJECT_TYPE_Brochure:
      csaMenuItems.SetNames(IDS_BROCHURE_PANEL_NAMES);
      ASSERT(csaMenuItems.GetSize() == 2);
      menuPopup.CreatePopupMenu();
      menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_BROCHURE_OUTSIDE,
               csaMenuItems[0]);
      menuPopup.AppendMenu(MF_STRING, IDC_BROWSER_VIEW_BROCHURE_INSIDE,
               csaMenuItems[1]);
      menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
      menuPopup.DestroyMenu();  
      break;
      
		case PROJECT_TYPE_Craft:
			if (pCurrentDoc->NumberOfPages() == 1)
				break;
			// else fall through...
		case PROJECT_TYPE_Calendar:
		case PROJECT_TYPE_Newsletter:
      csaMenuItems.SetNames(IDS_BROWSER_NEWSLETTER_MENUITEMS);
      ASSERT(csaMenuItems.GetSize() == 5);
      menuPopup.CreatePopupMenu();
      nMenuFlags = MF_STRING;
      if(lNumberOfPages <= 1)
         nMenuFlags |= MF_GRAYED; 

      menuPopup.AppendMenu(nMenuFlags, IDC_BROWSER_VIEW_NEWSLETTER_GOTO,
               csaMenuItems[0]);
      menuPopup.AppendMenu(nMenuFlags, IDC_BROWSER_VIEW_NEWSLETTER_NEXT,
               csaMenuItems[1]);
      menuPopup.AppendMenu(nMenuFlags, IDC_BROWSER_VIEW_NEWSLETTER_PREV,
               csaMenuItems[2]);
      menuPopup.AppendMenu(nMenuFlags, IDC_BROWSER_VIEW_NEWSLETTER_FIRST,
               csaMenuItems[3]);
      menuPopup.AppendMenu(nMenuFlags, IDC_BROWSER_VIEW_NEWSLETTER_LAST,
               csaMenuItems[4]);

      menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
      menuPopup.DestroyMenu();  
      break;
   }
}

void CProjectBrowserDialog::OnViewCardFront()
   {
      CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();
      if(pCurrentDoc == NULL)
         return;

      ((CCardDoc *)pCurrentDoc)->set_panel(CARD_PANEL_Front);
      InvalidatePreview();
   }

void CProjectBrowserDialog::OnViewCardInside()
   {
      CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();
      if(pCurrentDoc == NULL)
         return;

      ((CCardDoc *)pCurrentDoc)->set_panel(CARD_PANEL_Inside);
      InvalidatePreview();
   }

void CProjectBrowserDialog::OnViewCardBack()
   {
      CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();
      if(pCurrentDoc == NULL)
         return;

      ((CCardDoc *)pCurrentDoc)->set_panel(CARD_PANEL_Back);
      InvalidatePreview();
   }
// Also serves as postcard front
void CProjectBrowserDialog::OnViewBrochureOutside()
{
   CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();
   if(pCurrentDoc == NULL)
      return;

   ((CBrochureDoc *)pCurrentDoc)->GotoPage(0);
   InvalidatePreview();
}
// Also serves as postcard back
void CProjectBrowserDialog::OnViewBrochureInside()
{
   CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();
   if(pCurrentDoc == NULL)
      return;

   ((CBrochureDoc *)pCurrentDoc)->GotoPage(1);
   InvalidatePreview();
}

void CProjectBrowserDialog::OnViewNewsLetterGoto()
   {
      int      nRetVal;
      CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();

      ASSERT(pCurrentDoc);
      if(pCurrentDoc == NULL)
         return;

      CGotoPageDialog   dlgGoto(pCurrentDoc, this);

      TRY
         {
            nRetVal = dlgGoto.DoModal();
            if(nRetVal == IDOK)
               {
                  long lPage = atol(dlgGoto.m_csPage) - 1;
                  ((CBrochureDoc *)pCurrentDoc)->GotoPage(lPage);
                  InvalidatePreview();
               }
         }
      END_TRY
   }

void CProjectBrowserDialog::OnViewNewsLetterNext()
{
   CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();

   ASSERT(pCurrentDoc);
   if(pCurrentDoc == NULL)
      return;
	// This prevents the add page dialog
	if (pCurrentDoc->CurrentPageIndex() + 1 < pCurrentDoc->NumberOfPages())
	{
		pCurrentDoc->DoPageNext();
		InvalidatePreview();
	}
}

void CProjectBrowserDialog::OnViewNewsLetterPrev()
   {
      CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();

      ASSERT(pCurrentDoc);
      if(pCurrentDoc == NULL)
         return;

      ((CBrochureDoc *)pCurrentDoc)->DoPageBack();
      InvalidatePreview();
   }

void CProjectBrowserDialog::OnViewNewsLetterFirst()
   {
      CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();

      ASSERT(pCurrentDoc);
      if(pCurrentDoc == NULL)
         return;

      ((CBrochureDoc *)pCurrentDoc)->GotoPage(0);
      InvalidatePreview();
   }

void CProjectBrowserDialog::OnViewNewsLetterLast()
   {
      CPmwDoc *pCurrentDoc = m_CollectionDraw.GetDocument();

      ASSERT(pCurrentDoc);
      if(pCurrentDoc == NULL)
         return;

      ((CBrochureDoc *)pCurrentDoc)->GotoPage(pCurrentDoc->NumberOfPages()-1);
      InvalidatePreview();
   }

// This function is called when the browser is being initialized. It's called
// for each known project collection. The name of the collection is added to
// the collection list box, and all the project types supported by the collection
// (super categories) are added to the project type list.
void CProjectBrowserDialog::ProcessCollection(CPMWCollection* pCollection)
{
	if (m_pProjectTypeList == NULL)
		return;	// bad news

	INHERITED::ProcessCollection(pCollection);

   // Open up a stream for reading category information.
   CCategoryDatabase* pDatabase;
   if (pCollection->NewCategoryStream(&pDatabase) == ERRORCODE_None)
   {
		CCategoryDir* pCategoryDir = pDatabase->GetDir();
		if (pCategoryDir != NULL)
		{
			// Add in the first level categories (project types) for the current
			// collection to the Project Type list. Note that AddCategories() is called
			// with a maximum depth of 1 which means only the first level
			// categories (project types) are added; the category and subcategory
			// information is not added since it goes into the category list box.
			AddCategories(pCollection, pCategoryDir, 0, m_pProjectTypeList, NULL, 1);
		}
		pCollection->ReleaseCategoryStream(pDatabase);
	}
}

// The user changed the selected collection. This method updates the
// project type and category combo boxs with the applicable categories.
void CProjectBrowserDialog::CollectionChanged(void)
{
   if (m_pCollectionList == NULL || m_pProjectTypeList == NULL || m_pCategoryList == NULL)
	{
	   INHERITED::CollectionChanged();
		return;
	}

   // Start with empty project type and category lists.
	m_pProjectTypeList->ResetContent();
	m_pCategoryList->ResetContent();

	CPMWCollection* pCollection = GetCollectionSelection();
	if (pCollection == NULL)
	{
		// either "All" collections selected, or root of Supercollection selected
		int nFirst = -1;
		int nLast = -1;

		// If a Supercollection is selected, we need to add
		// categories for all collections in Supercollection.
		int nIndex = m_pCollectionList->GetCurSel();
		CString strColl;
		m_pCollectionList->GetText(nIndex, strColl);
		if (strColl != BrowserAllString())
		{		
			void* pSubList = m_pCollectionList->GetItemList(nIndex);
			pCollection = (CPMWCollection*)m_pCollectionList->GetItemDataPtr(1, pSubList);
			nFirst = m_pCollectionManager->FindCollection(pCollection);
			int nInSuper = m_pCollectionList->GetCount(pSubList);
			pCollection = (CPMWCollection*)m_pCollectionList->GetItemDataPtr(nInSuper-1, pSubList);
			nLast = m_pCollectionManager->FindCollection(pCollection);
		}

		AddAllCategories(nFirst, nLast);
	}
	else
	{
		// Get the selected project type.
		if (m_pProjectTypeList->GetCurSel() == 0 || m_pProjectTypeList->GetCurSel() == CB_ERR)
		{
			// 1st project type (index 0) contains All
			// if CB_ERR, it probably hasn't been initialized yet -- just assume All
			int nCollectionIndex = m_pCollectionManager->FindCollection(pCollection);
			ASSERT(nCollectionIndex >= 0);
			if (nCollectionIndex >= 0)
				AddAllCategories(nCollectionIndex);
		}
		else
		{
			// Open up a stream for reading category information.
			CCategoryDatabase* pDatabase;
			if (pCollection->NewCategoryStream(&pDatabase) == ERRORCODE_None)
			{
				CCategoryDir* pCategoryDirectory = pDatabase->GetDir();
				if (pCategoryDirectory != NULL)
				{
					// create the project type list
					AddCategories(pCollection, pCategoryDirectory, 0, m_pProjectTypeList, NULL, 1);

					int nCategory = 0;
					// Get the first category of the selected project type.
					nCategory = pCategoryDirectory->GetFirstChild(nCategory);
					if (nCategory >= 0)
					{
						// Add category information into the category list. Note that
						// AddCategories() is called with a depth of -1. This causes
						// the entire tree (starting at the specified category) to be added.
						AddCategories(pCollection, pCategoryDirectory, nCategory, m_pCategoryList, NULL, -1);
					}
				}

				pCollection->ReleaseCategoryStream(pDatabase);
         }
      }
   }

   // Put in the "All" entries.
	m_pProjectTypeList->InsertString(0, BrowserAllString());
	if (m_pProjectTypeList->GetCount() > 0)
	{
		m_fIgnoreSelectionChange = TRUE;
		m_pProjectTypeList->SetCurSel(0);
		m_fIgnoreSelectionChange = FALSE;
		m_fHasGroups = TRUE;
	}

	m_pCategoryList->InsertString(0, BrowserAllString());
	if (m_pCategoryList->GetCount() > 0)
	{
		m_fIgnoreSelectionChange = TRUE;
		m_pCategoryList->SetCurSel(0);
		m_fIgnoreSelectionChange = FALSE;
	}

   INHERITED::CollectionChanged();
}

// The user changed the selected project type (super-category). This method
// updates the category combo box with the applicable categories.
void CProjectBrowserDialog::ProjectTypeChanged()
{
	if (m_pCollectionList == NULL || m_pProjectTypeList == NULL || m_pCategoryList == NULL)
		return;

	// Start with an empty category list.
	m_pCategoryList->ResetContent();

	// Get the selected collection object.
	CPMWCollection* pCollection = GetCollectionSelection();

	int nProjectIndex = m_pProjectTypeList->GetCurSel();

	// if project type is "All" (index 0) we have groups of projects
	m_fHasGroups = (nProjectIndex == 0);

	if (pCollection == NULL)
	{
		// either "All" collections selected, or root of Supercollection selected
		int nFirst = -1;
		int nLast = -1;

		// If a Supercollection is selected, we need to add
		// categories for all collections in Supercollection.
		int nIndex = m_pCollectionList->GetCurSel();
		if (nIndex >= 0)
		{
			CString strColl;
			m_pCollectionList->GetText(nIndex, strColl);
			if (strColl != BrowserAllString())
			{		
				void* pSubList = m_pCollectionList->GetItemList(nIndex);
				pCollection = (CPMWCollection*)m_pCollectionList->GetItemDataPtr(1, pSubList);
				nFirst = m_pCollectionManager->FindCollection(pCollection);
				int nInSuper = m_pCollectionList->GetCount(pSubList);
				pCollection = (CPMWCollection*)m_pCollectionList->GetItemDataPtr(nInSuper-1, pSubList);
				nLast = m_pCollectionManager->FindCollection(pCollection);
			}
		}

		AddAllCategories(nFirst, nLast, nProjectIndex);
	}
	else
	{
		if (nProjectIndex == 0 || nProjectIndex == CB_ERR)
		{
			// 1st item (index 0) contains All
			int nCollectionIndex = m_pCollectionManager->FindCollection(pCollection);
			ASSERT(nCollectionIndex >= 0);
			if(nCollectionIndex >= 0)
				AddAllCategories(nCollectionIndex);
		}
		else
		{
			int nCategory = (int)(m_pProjectTypeList->GetItemData(nProjectIndex));
			if (nCategory >= 0)
			{
				// Open up a stream for reading category information.
				CCategoryDatabase* pDatabase;
				if (pCollection->NewCategoryStream(&pDatabase) == ERRORCODE_None)
				{
					CCategoryDir* pCategoryDir = pDatabase->GetDir();
					if (pCategoryDir != NULL)
					{
						// Get the first category of the selected project type.
						nCategory = pCategoryDir->GetFirstChild(nCategory);
						if (nCategory >= 0)
						{
							// Add category information into the category list. Note that
							// AddCategories() is called with a depth of -1. This causes
							// the entire tree (starting at the specified category) to be added.
							AddCategories(pCollection, pCategoryDir, nCategory, m_pCategoryList, NULL, -1);
							m_fHasGroups = FALSE;	// just one project type now
						}
					}

					pCollection->ReleaseCategoryStream(pDatabase);
				}
			}
		}
	}

	// Put in the "All" entry.
	m_pCategoryList->InsertString(0, BrowserAllString());
	if (m_pCategoryList->GetCount() > 0)
	{
		BOOL fSaveIgnoreSelectionChange = m_fIgnoreSelectionChange;
		m_fIgnoreSelectionChange = TRUE;
		m_pCategoryList->SetCurSel(0);
		m_fIgnoreSelectionChange = fSaveIgnoreSelectionChange;
	}

	if (!m_fIgnoreSelectionChange)
		UpdateResultSet();
}

void CProjectBrowserDialog::UpdateBrowser(void)
{
	CString strCurCat;
	GetCategorySelection(strCurCat);

	// Reset categories and update result set
	CollectionChanged();
	UpdateResultSet();

	// If previous selections were not "All", try resetting project type
	// and category back to previous (this could fail if they no longer exist)
	if (!strCurCat.IsEmpty() && strCurCat != BrowserAllString())
	{
		CMlsStringArray strArray(strCurCat);

		// reset Project Type to previous selection
		m_pProjectTypeList->SetCurSel(strArray[0]);
		if (strArray.GetSize() == 2)
		{
			// reset Category to previous selection
			m_pCategoryList->SetCurSel(strArray[1]);
		}
	}
}

void CProjectBrowserDialog::GetCategorySelection(CString& csCategories)
{
	// The project browser breaks the category selections into two pieces.
	// The super category (project type) is in the "project type" list,
	// the category and sub category are selected in the category list.
	if (m_pProjectTypeList != NULL)
	{
		CMlsStringArray Selections;

		// Get the project type.
		GetSelections(Selections, m_pProjectTypeList);
		Selections.GetNames(csCategories);

		// Get the remaining selections from the category list box.
		if (m_pCategoryList != NULL)
		{
			GetSelections(Selections, m_pCategoryList);

			// Append category selections up to an "All" or the end of the list.
			for (int nSelection = 0; nSelection < Selections.GetSize(); nSelection++)
			{
				if (stricmp(Selections.ElementAt(nSelection), BrowserAllString()) == 0)
					break;

				if (!Selections.ElementAt(nSelection).IsEmpty())
				{
					if (!csCategories.IsEmpty())
						csCategories += '|';

					csCategories += Selections.ElementAt(nSelection);
				}
			}
		}
	}
}

// Open the currently selected project in PrintMaster.
void CProjectBrowserDialog::DoSelect(void)
{
   if (m_nResultItems > 0 && m_nCurPreviewItem >=0 &&
       m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
   {
      if (!SelectedItemIsLicensed())
      {
         OnClickedUnlicensed();
         return;
      }

      CString csCollection;
      CString csFile;
      CString csFriendlyName;
      CString csPath;

      // Get the scoop on the selected project.
      if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber,
									  csCollection, csFile, csFriendlyName, csPath))
      {
         if (GET_PMWAPP()->OpenDocumentFile(csPath) != NULL)
         {
				CPMWCollection* pCollection;
				pCollection = m_pCollectionManager->FindCollection((BYTE)m_uSelectedCollectionID, CollectionType(), CollectionSubType());
				if (pCollection == NULL)
				{
					ASSERT(pCollection);
				}
				else if(GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::CraftStandard || 
							GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::CraftDeluxe || 
							GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::CreataParty)
				{
					CString csCategory = pCollection->GetCategory(m_dwSelectedItemNumber);
					csCategory.MakeUpper();
					CString csPopup;
					csPopup.LoadString(IDS_POPUP_CARDS);
					csPopup.MakeUpper();
					
					CString csCutout;
					csCutout.LoadString(IDS_CUTOUT_CARDS);
					csCutout.MakeUpper();
					
					CString csBig;
					csBig.LoadString(IDS_BIG_CARDS);
					csBig.MakeUpper();

					if(csCategory.Find(csPopup) != -1 || 
						csCategory.Find(csCutout) != -1 || 
						csCategory.Find(csBig) != -1 )
					{
						GET_PMWAPP()->ShowHelpfulHint("Craft cards");
					}
				}
            EndDialog(IDOK);
         }
      }
   }
}

CString CProjectBrowserDialog::GetPrintCategory(USHORT uCollectionID, DWORD dwItemNumber)
{
   CPMWCollection* pCollection = m_pCollectionManager->FindCollection((BYTE)uCollectionID, CollectionType(), CollectionSubType());
   CString csCategory = pCollection->GetCategory(dwItemNumber);
   return csCategory;
}
// Process the print project command.
void CProjectBrowserDialog::DoPrint(void)
{
   CString csCollection;
   CString csFile;
   CString csFriendlyName;
   CString csPath;      // art path name
      
   CPmwApp  *pApp = GET_PMWAPP ();
   CPmwDoc* pDoc = (CPmwDoc*)(pApp->NewHiddenDocument());
   if(pDoc == NULL)
      return;

   CPrintInfo pInfo;
   CPrintDialog *dlg_save = pInfo.m_pPD;
   BOOL fManualPrint = (GetKeyState(VK_SHIFT) < 0);
   CArtPrintDialog pdlg(pDoc, fManualPrint);

   pInfo.m_pPD = &pdlg;

   if (AfxGetApp()->DoPrintDialog(&pdlg) != IDOK)
//   if (pdlg.DoModal() != IDOK)
   {
      pDoc->OnCloseDocument();
      pInfo.m_pPD = dlg_save;
      return;
   }

   // check print range
   int printRange =  pdlg.GetPrintRange();  // 0=selected, 1=print page, 2=all
   PrintProjectEntryPtr entry;
   List picture_list(0, TRUE);

	CString	csCategory;
	LPCSTR	pCategory = NULL;
   DWORD		dwItemNumber = (DWORD)-1;
 
   //  different kinds of print ranges
   switch(printRange) 
   { 
      case 0: // just print selected picture
      {
         // Get file name of selected picture
         if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber, 
                                 csCollection, csFile, csFriendlyName, csPath))
         {
				if (fManualPrint)
				{
					csCategory = GetPrintCategory(m_uSelectedCollectionID, m_dwSelectedItemNumber);
					pCategory = csCategory;
				}
            /* Create the picture entry. */
            if ((entry = new PrintProjectEntry(LPCTSTR(csPath), LPCTSTR(csFriendlyName), pCategory, m_uSelectedCollectionID, m_dwSelectedItemNumber)) != NULL)
            {
               /* Add it to the list. */
               picture_list.append(entry);
            }
         }
         break;
      } 
      case 1:   // print all pictures on page
      {
         int nNumberOfItems = m_pItemPreviews->GetSize();

         USHORT uCollectionID = (USHORT)-1;
         CPreviewArray::Item* pItem = NULL;
         CString csPath;

         // Build the picture list.
         for (int i = 0; i < nNumberOfItems; i++) 
         {
            // iterate thru selected collection and add each picture to print list         
            pItem = m_pItemPreviews->GetAt(i);
            //  if (pItem != NULL)
            {
               if (GetItemInformation(pItem->m_uCollectionID, pItem->m_dwItemNumber, 
                                 csCollection, csFile, csFriendlyName, csPath))
               {
                  /* Create the picture entry. */
                  if (fManualPrint)
                  {
                     csCategory = GetPrintCategory(pItem->m_uCollectionID, pItem->m_dwItemNumber);
							pCategory = csCategory;
                  }
                  if ((entry = new PrintProjectEntry(LPCTSTR(csPath), LPCTSTR(csFriendlyName), pCategory, pItem->m_uCollectionID, pItem->m_dwItemNumber)) != NULL)
                  {
                  /* Add it to the list. */
                     picture_list.append(entry);
                  }
                  csPath.Empty();   
               }
            }
         }
         break;
      }
      case 2: // print all pictures in selection group
      {
        long lNumberOfItems = m_SearchContext.GetResultCount();
        USHORT uCollectionID = (USHORT)-1;
        CString csPath;

         // Build the picture list.
			for (long i = 0; i < lNumberOfItems; i++) 
         {
            // Get the current search result.
            CPMWCollectionResult Result = m_SearchContext.GetSearchResult(i);
            // Unpack the collection ID and the item number for the
            // current result.
            uCollectionID = Result.GetCollectionID();
            dwItemNumber = Result.GetItemNumber();

            if (fManualPrint)
            {
               csCategory = GetPrintCategory(uCollectionID, dwItemNumber);
					pCategory = csCategory;
            }
            
				if (GetItemInformation(uCollectionID, dwItemNumber, 
                              csCollection, csFile, csFriendlyName, csPath))
            {
               /* Create the picture entry. */
               if ((entry = new PrintProjectEntry(LPCTSTR(csPath), LPCTSTR(csFriendlyName), pCategory, uCollectionID, dwItemNumber)) != NULL)
               {
               /* Add it to the list. */
                  picture_list.append(entry);
               }
               csPath.Empty();   
            }
         }// end of the for loop
         break;
      }  // case 2
      default:
         ;
   } // end of switch on type of print
        
   LPSTR pDir = NULL;  

   pDoc->SetTitle(GET_PMWAPP()->GetResourceStringPointer (IDS_PROJECTS));
   CProjectPrintView view(picture_list, pDir);

   pDoc->size_to_paper();

   /* Hook the view into the document. */
   view.set_document(pDoc);

   /* Print the project. */
   view.PrintProject(&pInfo);

   /* Remove the doc before its destructor is called. */
   view.clear_document();

   pInfo.m_pPD = dlg_save;
   pDoc->OnCloseDocument();
   picture_list.empty();
}

// Project-specific implementation of drag/drop source.
COleDataSource* CProjectBrowserDialog::CreateDragDropSource(void)
{
   CProjectDataSource* pSource = new CProjectDataSource;
   AddDataExchangeFormat(pSource);

   return pSource;
}

void CProjectBrowserDialog::SetLastExportName(LPCSTR pszLastName)
{
   m_csLastExport = pszLastName;
}

void CProjectBrowserDialog::GetExportDirectory(CString& csDir)
{
   if (m_csLastExport.IsEmpty())
   {
      // Document/project subdirectory.
      csDir = "[[D]]";
   }
   else
   {
      Util::SplitPath(m_csLastExport, &csDir, NULL);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CProjectBrowserDialog message handlers

BOOL CProjectBrowserDialog::OnInitDialog() 
{
  	// load bitmaps - must be done before calling baseclass init
   UINT  uiPreviewBackgroundID = IDB_GALLERY_PREVIEW;
   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
      {
         CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());

         rlProductSpecific.LoadBitmap(m_bmpTitle, CSharedIDResourceManager::riProjectGalleryTitle, pOurPal);
         rlProductSpecific.LoadBitmap(m_bmpSidebarBkgrnd, CSharedIDResourceManager::riProjectGallerySidebar, pOurPal);
         rlProductSpecific.LoadBitmap(m_bmpSidebarBorder, CSharedIDResourceManager::riProjectGallerySidebarBorder, pOurPal);
         uiPreviewBackgroundID = IDB_GALLERY_AGPREVIEW;
      }
   else
      {
	      Util::LoadResourceBitmap(m_bmpTitle,			MAKEINTRESOURCE(IDB_GALLERY_PROJ_TITLE), pOurPal);
	      Util::LoadResourceBitmap(m_bmpSidebarBkgrnd, MAKEINTRESOURCE(IDB_GALLERY_PROJ_SIDEBAR), pOurPal);
	      Util::LoadResourceBitmap(m_bmpSidebarBorder, MAKEINTRESOURCE(IDB_GALLERY_PROJ_SIDEBAR_BORDER), pOurPal);
      }
   Util::LoadResourceBitmap(m_bmpPreviewBkgrnd, MAKEINTRESOURCE(uiPreviewBackgroundID), pOurPal);

	// load menu
	LoadMenu();

	// Now we're ready to call baseclass
   INHERITED::OnInitDialog();

	SetStatusBarIndicators();

	GET_PMWAPP()->ShowHelpfulHint(PLACE_HINT, PLACE_Projects, this);

   return TRUE;  // return TRUE unless you set the focus to a control
}

void CProjectBrowserDialog::LoadMenu() 
{
	CMenu menu;
	menu.LoadMenu(IDR_PROJECT_BROWSER_MENU);
	if(!GetConfiguration()->SupportsOnlineGallery())
		menu.DeleteMenu(3, MF_BYPOSITION);

	SetMenu(&menu);
	menu.Detach();

}

void CProjectBrowserDialog::SetStatusBarIndicators() 
{
   int   nNumIndicators = sizeof(uProjectIndicatorIDs)/sizeof(UINT);
   BOOL  bLoaded = m_StatusBar.SetIndicators(uProjectIndicatorIDs, nNumIndicators);
   ASSERT(bLoaded);

   for(int i=0;i < m_StatusBar.GetCount();i++)
      SetIndicatorSize(i);
}

// Select a project using the Windows File Open dialog.
void CProjectBrowserDialog::OnFileOpenfromdisk() 
{
   if (GET_PMWAPP()->open_a_document())
   {
      EndDialog(IDOK);
   }
}

// The user wants to leave without selecting anything.
void CProjectBrowserDialog::OnFileClosebrowser() 
{
   if (CanClose())
   {
      EndDialog(IDCANCEL);
   }
}

void CProjectBrowserDialog::OnOnlineUpdateAccount() 
{
	GetContentManager()->UpdateAccount();
}

void CProjectBrowserDialog::OnOnlineCreateAccount() 
{
	if(GetContentManager()->CreateNewAccount())
		EndDialog(IDOK);
}

void CProjectBrowserDialog::OnOnlineChangeAccounts() 
{
	if(GetContentManager()->RequestExistingAccount())
		EndDialog(IDOK);
}

void CProjectBrowserDialog::OnOnlineDeleteAllDownloadedFiles()
{
	GetContentManager()->DeleteCachedDirectories(this, NULL);
}

BOOL CProjectBrowserDialog::ShowArtButton()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWebArtBrowserDialog

BEGIN_MESSAGE_MAP(CWebArtBrowserDialog, CArtBrowserDialog)
   //{{AFX_MSG_MAP(CWebArtBrowserDialog)
	ON_COMMAND(IDC_ANIMATION_PREVIEW, OnAnimationPreview)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CWebArtBrowserDialog::CWebArtBrowserDialog(CWnd* pParent /*=NULL*/)
   : CArtBrowserDialog(CWebArtBrowserDialog::IDD, pParent)
{
   m_csINISection = "WebArtBrowser";
}

CWebArtBrowserDialog::~CWebArtBrowserDialog()
{
}

void CWebArtBrowserDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CWebArtBrowserDialog)
	DDX_Control( pDX, IDC_ANIMATION_PREVIEW, m_ctlPreview );
   //}}AFX_DATA_MAP
}


/* Performs item selection and then checks to see that the selected item allows
	preview.
*/
void CWebArtBrowserDialog::SelectItem(USHORT uCollectionID, DWORD dwItemNumber)
{
	// Perform the selection.
	CArtBrowserDialog::SelectItem( uCollectionID, dwItemNumber );

	// Update the availability of the preview button.
	UpdateAnimationPreview();
}	


/* Method to enable and disable the animation preview button dependent on the
	state of the currently selected storage item.
*/
void CWebArtBrowserDialog::UpdateAnimationPreview()
{
	bool bEnable = FALSE;

	if (m_nResultItems > 0 && m_nCurPreviewItem >=0 &&
		m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
	{
      // Set these three.
		CString  csCollection;
		CString  csFile;
		CString  csFriendlyName;
		CString  csAccessName;
		BOOL		fCropCopyright = FALSE;
		CString	strKeywords;
		if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber,
									csCollection, csFile, csFriendlyName, csAccessName,
									&strKeywords, &fCropCopyright ))
		{
			// Check to see if the selected item is an animated GIF.
			CString strWebAnimationCategory;
			strWebAnimationCategory.LoadString( IDS_WEB_ANIMATION_CATEGORY );
		}
	}

//	m_ctlPreview.EnableWindow(bEnable);

	// *** TO BE CHANGED *** For the time being always enabled, error handling will be placed at run time.
	m_ctlPreview.EnableWindow(TRUE);
}

/* Launches the web animation preview dialog when the preview button is pressed.
*/
void CWebArtBrowserDialog::OnAnimationPreview()
{
	if (m_nResultItems > 0 && m_nCurPreviewItem >=0 &&
		m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
	{
      // Set these three.
		CString  csCollection;
		CString  csFile;
		CString  csFriendlyName;
		CString  csAccessName;
		BOOL		fCropCopyright = FALSE;
		if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber,
									csCollection, csFile, csFriendlyName, csAccessName,
									NULL, &fCropCopyright))
		{
			// Determine the type of graphic we are dealing with.
			GRAPHIC_TYPE graphicType;
			if (csAccessName.IsEmpty())
				graphicType = GRAPHIC_TYPE_BMP;
			else
				graphicType = type_of_art_extension(csAccessName);

			// Get the raw data to animate.
			UCHAR *pRawData = NULL;
			DWORD dwSizeOfBuffer;
			if (graphicType == GRAPHIC_TYPE_GIF)
			{
				pRawData = CreateBufferFromStorageName( csAccessName, &dwSizeOfBuffer );
			}

			// Perform the animation if we have some data!
			if (pRawData != NULL)
			{
				// Create a GIF animator.
				CPGIFAnimator *pGIFAnimator = new CPGIFAnimator( pRawData, dwSizeOfBuffer );

				if (pGIFAnimator->IsAnimation())
				{
					// Create the dialog to run the gif animator within.
					CPAnimationPreviewDlg dialogPreview( pGIFAnimator );

					// Run the animation!
					dialogPreview.DoModal();
				}
				else
				{
					AfxMessageBox( IDS_NON_ANIMATION_SELECTED );
				}

				// Clean up...
				delete pGIFAnimator;
				delete [] pRawData;
			}
		}
	}
}


/* This method retrieves the raw data buffer for the specified storage access
	name.  This can be either a physical file reference or a collection name.

	Proper CD requesting is performed.
*/
UCHAR *CWebArtBrowserDialog::CreateBufferFromStorageName(
	const CString &strAccessName,	// The name of the storage to retrieve the buffer data from.
	DWORD *pdwSizeOfBuffer )		// Filled with the size of the buffer.
{
	UCHAR *pRawData = NULL;

	// Set the path from the GCS.
	CString strCollectionName;
	CString strFileName;
	PathBindingType type = GetGlobalPathManager()->BindPath( strAccessName, strFileName, &strCollectionName );
	if (type == PBT_CollectionItem)
	{
		// We are dealing with a collection item, so attempt to load it.
		// The file is being dynamically allocated so that we can delete it
		// before putting the bits into the animator.  This keeps the
		// amount of raw data in memory to a minimum.
		StorageFile *pfileImage = new StorageFile( strAccessName );

		// Find out how big it is so we will know how much data to read.
		ST_DEV_POSITION lSize;
		pfileImage->length( &lSize );	// This should open and read in the file.

		// Allocate a data buffer to hold the image data to transfer over to the component.
		pRawData = new UCHAR[lSize];
		pfileImage->huge_read( (char *)pRawData, lSize, pdwSizeOfBuffer );
		delete pfileImage;

		// Verify for completeness that we got the entire file.
		if (*pdwSizeOfBuffer != (DWORD)lSize)
		{
			TRACE( "Failure to read in the raw image data buffer for use in animating preview.\n" );
			ASSERT( 0 );
			delete [] pRawData;
			pRawData = NULL;
		}
	}
	else
	{
		// We are importing the image, so we can just load off of the filename as it is.
		CString strFixedPath = GetGlobalPathManager()->LocatePath( strAccessName );

		TRY
		{
			// Create a file to read the raw data buffer from.
			CFile fileLocal( strFixedPath, CFile::modeRead );

			// Allocate a data buffer to hold the image data to transfer over to the component.
			DWORD dwSize = fileLocal.GetLength();
			pRawData = new UCHAR[dwSize];
			*pdwSizeOfBuffer = fileLocal.ReadHuge( pRawData, dwSize );

			if (*pdwSizeOfBuffer != dwSize)
			{
				TRACE( "Failure to read in the raw image data buffer for use in animating preview.\n" );
				ASSERT( 0 );
				delete [] pRawData;
				pRawData = NULL;
			}

			fileLocal.Close();
		}
		CATCH( CFileException, exception )
		{
			TRACE( "Handled Exception of file read when attempting to load animation preview data.\n" );
			ASSERT( 0 );
			if (pRawData != NULL)
			{
				delete [] pRawData;
				pRawData = NULL;
			}
		}
		END_CATCH
	}

	return pRawData;
}


/////////////////////////////////////////////////////////////////////////////
// CArtBrowserDialog

// Clipboard formats the art browser exports.
UINT CArtBrowserDialog::m_cfDataExchange = ::RegisterClipboardFormat("PMG 4.0 Art Browser");
CString CArtBrowserDialog::m_csLastExport;
CPtrArray CArtBrowserDialog::m_cpaOpenBrowsers;

CArtBrowserDialog::CArtBrowserDialog(CWnd* pParent /*=NULL*/)
   : CBrowserDialog(CArtBrowserDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CArtBrowserDialog)
	//}}AFX_DATA_INIT

   CommonConstruct(TYPE_ArtGallery);
}

CArtBrowserDialog::CArtBrowserDialog(CArtBrowserDialog::ArtBrowserType nType, CWnd* pParent /*=NULL*/)
   : CBrowserDialog(CArtBrowserDialog::IDD, pParent)
{
   CommonConstruct(nType);
}

CArtBrowserDialog::CArtBrowserDialog(UINT uID, CWnd* pParent /* = NULL */)
   : CBrowserDialog(uID, pParent)
{
   CommonConstruct(TYPE_ArtGallery);
}

void CArtBrowserDialog::CommonConstruct(CArtBrowserDialog::ArtBrowserType nType)
{
	// Override default colors.
   LoadColorsFromINI("ArtBrowserColors");

   m_nType = nType;
   m_nPopupMenuIndex = 0;
   m_csINISection = "ArtBrowser";

   m_pszLastPicture = NULL;
   m_pLastPicture = NULL;

	m_pDropTarget = NULL;

	m_cpaOpenBrowsers.Add(this);

   m_MediumThumbnailSize.cx = 100;
   m_MediumThumbnailSize.cy = 100;
   m_ThumbnailSize = m_MediumThumbnailSize;

	m_pMasterArtButton = NULL;
}

CArtBrowserDialog::~CArtBrowserDialog()
{
	// Remove us from the list of open browsers.
	for (int i = 0; i < m_cpaOpenBrowsers.GetSize(); i++)
	{
		if (m_cpaOpenBrowsers.GetAt(i) == this)
		{
			// Remove us.
			m_cpaOpenBrowsers.RemoveAt(i);

			// We assume we are only in there once.
			break;
		}
	}

	delete m_pDropTarget;
	delete m_pMasterArtButton;
}

void CArtBrowserDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CArtBrowserDialog)
	DDX_Control(pDX, IDC_CATEGORY_TREE_CONTROL, m_CategoryTreeCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CArtBrowserDialog, CBrowserDialog)
   //{{AFX_MSG_MAP(CArtBrowserDialog)
   ON_COMMAND(ID_FILE_OPENFROMDISK, OnFileOpenfromdisk)
   ON_COMMAND(ID_FILE_CLOSEBROWSER, OnFileClosebrowser)
   ON_COMMAND(ID_FILE_OPENPHOTOCD, OnFileOpenPhotoCD)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_ONLINE_UPDATEACCOUNT, OnOnlineUpdateAccount)
	ON_COMMAND(ID_ONLINE_CREATEACCOUNT, OnOnlineCreateAccount)
	ON_COMMAND(ID_ONLINE_CHANGE_ACCOUNTS, OnOnlineChangeAccounts)
	ON_COMMAND(ID_ONLINE_DELETE_ALL_DOWNLOADED_FILES, OnOnlineDeleteAllDownloadedFiles)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CATEGORY_TREE_CONTROL, OnSelchangedCategoryTree)
   ON_COMMAND(ID_MASTER_ART, OnMasterArt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CArtBrowserDialog::HandleSpecialCollectionButtons(void)
{
	INHERITED::HandleSpecialCollectionButtons();

	CWnd* pDummy = GetDlgItem(ID_MASTER_ART);
	if (pDummy != NULL)
	{
		// Get the position of the placeholder in browser client coordinates.
		CRect crDummy;
		pDummy->GetWindowRect(crDummy);
		pDummy->DestroyWindow();
		ScreenToClient(crDummy);
		// Compute the point on which to center the Master Art button.
		int nXCenter = (crDummy.left+crDummy.right)/2;
		int nYCenter = (crDummy.top+crDummy.bottom)/2;

		CRect	MasterArtButtonRect;
		GetArtButtonRect(MasterArtButtonRect, IDB_MASTER_ART_UP);

		// Do we want this button?
		if (!MasterArtButtonRect.IsRectEmpty())
		{
			int x = nXCenter - MasterArtButtonRect.Width() / 2;
			MasterArtButtonRect.OffsetRect(x, nYCenter - MasterArtButtonRect.Height() / 2);

			TRY
			{
				m_pMasterArtButton = new CNonRectButton;
				m_pMasterArtButton->Create(MasterArtButtonRect, this, ID_MASTER_ART);
				m_pMasterArtButton->SetBitmaps(MAKEINTRESOURCE(IDB_MASTER_ART_UP), MAKEINTRESOURCE(IDB_MASTER_ART_DOWN), pOurPal);
				// By convention, the transparent color is cyan.
				m_pMasterArtButton->SetTransparentColor(RGB(0,255,255));
			}
			END_TRY
		}
   }
}

// Extract category information from the content database and add it to
// the specified category tree.
void CArtBrowserDialog::AddCategories(CPMWCollection *pCollection,
											  CCategoryDir* pCategoryDir, int nCategory,
											  int nDepth, HTREEITEM hTreeCtrlParent)
{
   if(pCollection == NULL)
	{
		ASSERT(pCollection);
		return;
	}

   // Check if we have reached the maximum desired depth.
	if (nDepth == 0)
		return;

	TV_ITEM tvI;					// Content for each inserted item
	tvI.mask           = TVIF_TEXT | TVIF_PARAM;
	TV_INSERTSTRUCT tvIns;		// Properties for each inserted item
#if 0
	tvIns.hInsertAfter = TVI_SORT;
#else
	tvIns.hInsertAfter = TVI_LAST;
#endif
	if (hTreeCtrlParent == NULL)
	{
		// Get handle to existing "all" category at the root; if not found, add it
		hTreeCtrlParent = Util::FindTreeCtrlStringExact(m_CategoryTreeCtrl, TVI_ROOT, BrowserAllString());
		if (!hTreeCtrlParent)
		{
			tvI.pszText			= (LPSTR)BrowserAllString();
			tvI.lParam			= 0;
			tvIns.item			= tvI;
			tvIns.hParent		= TVI_ROOT;
			hTreeCtrlParent	= m_CategoryTreeCtrl.InsertItem( &tvIns );
		}
		ASSERT( hTreeCtrlParent );
	}

	USHORT   usProductFlags = pCollection->GetProductFlags();

	// Loop through all categories at this level.
	while (nCategory >= 0)
	{
		// Get the current category information.
		CCategoryEntry Category;
		if (pCategoryDir->GetItem(nCategory, &Category) == ERRORCODE_None)
		{
			// Don't add categories that have no items for this product
			if(Category.GetFlags() == 0 || ((USHORT)Category.GetFlags() & usProductFlags) != 0)
			{
				// Get category name
				if (*(Category.GetName()) != 0)
				{
					// Get handle to existing category at this level; if not found, add new category to tree
					HTREEITEM hCategory = Util::FindTreeCtrlStringExact(m_CategoryTreeCtrl, hTreeCtrlParent, Category.GetName());
					if (!hCategory)
					{
						ASSERT( hTreeCtrlParent );
						tvI.pszText    = (LPSTR)Category.GetName();
						tvI.lParam     = nCategory;
						tvIns.item     = tvI;
						tvIns.hParent  = hTreeCtrlParent;
						hCategory = m_CategoryTreeCtrl.InsertItem( &tvIns );
						ASSERT( hCategory );
					}

					if (hCategory && nDepth != 1)
					{
						// Add subcategories and sub items to list box item.
						int nSubCategory = pCategoryDir->GetFirstChild(nCategory);
						if (nSubCategory > 0)
						{
							// Recursively add subcategories.
							AddCategories(pCollection, pCategoryDir, nSubCategory, nDepth-1, hCategory);
						}
					}
				}
			}
		}

		// Move on to the next category at the current level.
		nCategory = pCategoryDir->GetNextSibling(nCategory);
		ASSERT(nCategory != NULL);
	}
}

void CArtBrowserDialog::AddAllCategories(int nStartColl /*=-1*/, int nEndColl /*=-1*/)
{
	int nCollection, nEndCollection;

	m_CategoryTreeCtrl.SetRedraw( FALSE );
	BOOL fSaveIgnoreSelectionChange = m_fIgnoreSelectionChange;
	m_fIgnoreSelectionChange = TRUE;
	m_CategoryTreeCtrl.DeleteAllItems();
	m_fIgnoreSelectionChange = fSaveIgnoreSelectionChange;

	// Three possibilities for which collections to process:
	// 1) nStartColl == -1, nEndColl == -1	: Process all collections
	// 2) nStartColl >= 0, nEndColl == -1	: Process only nStartColl collection
	// 3) nStartColl >= 0, nEndColl >= 0	: Process nStartColl through nEndColl
	nCollection = (nStartColl < 0) ? 0 : nStartColl;
	nEndCollection = nEndColl;
	if (nEndColl < 0)
	{
		if (nStartColl < 0)
			nEndCollection = m_pCollectionManager->NumberOfCollections(CollectionType(), CollectionSubType()) - 1;
		else
			nEndCollection = nCollection;
	}

	// Process each collection by calling ProcessCollection().
	for (; nCollection <= nEndCollection; nCollection++)
	{
		CPMWCollection* pCollection = m_pCollectionManager->GetCollection(nCollection, CollectionType(), CollectionSubType());
		if (pCollection != NULL)
		{
			// Open up a stream for reading category information.
			CCategoryDatabase* pDatabase;
			if (pCollection->NewCategoryStream(&pDatabase) != ERRORCODE_None)
				break;

			CCategoryDir* pCategoryDir = pDatabase->GetDir();
			if (pCategoryDir != NULL)
				AddCategories(pCollection, pCategoryDir, NULL, -1, NULL);

			pCollection->ReleaseCategoryStream(pDatabase);
		}
	}

	Util::SortTreeCtrl( m_CategoryTreeCtrl );
	SetCategorySelection( CString( BrowserAllString() ) );
	m_CategoryTreeCtrl.Expand( m_CategoryTreeCtrl.GetRootItem(), TVE_EXPAND ); // expand "All" entry at top
	m_CategoryTreeCtrl.SetRedraw( TRUE );
	m_CategoryTreeCtrl.RedrawWindow(NULL, NULL, RDW_NOERASE | RDW_UPDATENOW );
}

// Return the '|' delimited category string for the current category selection in the tree control
//
void CArtBrowserDialog::GetCategorySelection( CString& csCategories )
{
	HTREEITEM hTreeItem = m_CategoryTreeCtrl.GetSelectedItem();
	if (hTreeItem)
		Util::AddCategorySelectionStrings( m_CategoryTreeCtrl, hTreeItem, csCategories );
	if (csCategories.IsEmpty())
		csCategories = BrowserAllString();
}

// Set the current category selection in the tree control from the '|' delimited category string
//
void CArtBrowserDialog::SetCategorySelection( CString& csCategories )
{
	HTREEITEM hParent = m_CategoryTreeCtrl.GetRootItem();
	if (hParent)
	{
		if (!m_fIgnoreSelectionChange)
		{
			m_fIgnoreSelectionChange = TRUE;

			if (csCategories != BrowserAllString())
				Util::SetTreeCtrlSelection( m_CategoryTreeCtrl, hParent, csCategories );
			else
			{
				// handle "All" at root
				m_CategoryTreeCtrl.EnsureVisible( hParent );
				m_CategoryTreeCtrl.SelectItem( hParent );
			}

			m_fIgnoreSelectionChange = FALSE;
		}
	}
}

BOOL CArtBrowserDialog::RestoreCategoryTreeState( LPCSTR szEntry )
{
   CIniFile IniFile( GET_PMWAPP()->m_pszProfileName );
   CString csValue = IniFile.GetString( m_csINISection, szEntry, BrowserAllString() );

	HTREEITEM hParent = m_CategoryTreeCtrl.GetRootItem();
	if (hParent)
	{
		BOOL savefIgnoreSelectionChange = m_fIgnoreSelectionChange;
		m_fIgnoreSelectionChange = TRUE;

		if (csValue != BrowserAllString())
			Util::SetTreeCtrlSelection( m_CategoryTreeCtrl, hParent, csValue );
		else
		{
			// handle "All" at root
			m_CategoryTreeCtrl.EnsureVisible( hParent );
			m_CategoryTreeCtrl.SelectItem( hParent );
		}

		m_fIgnoreSelectionChange = savefIgnoreSelectionChange;
	}

   return TRUE;
}

int CArtBrowserDialog::RestoreState()
{
	int retVal = INHERITED::RestoreState();

	RestoreCategoryTreeState( "Category" );
	CategoryChanged();

   return retVal;
}

BOOL CArtBrowserDialog::SaveCategoryTreeState( LPCSTR szEntry )
{
	// GCB 3/4/99 sanity check in case we got here from shutting down the dialog
	// before it was fully initialized
	if (!::IsWindow(m_CategoryTreeCtrl.m_hWnd))
		return FALSE;
	CString csValue;
	GetCategorySelection( csValue );
   CIniFile IniFile( GET_PMWAPP()->m_pszProfileName );
	IniFile.WriteString( m_csINISection, szEntry, csValue );
   return TRUE;
}

int CArtBrowserDialog::SaveState()
{
	int retVal = INHERITED::SaveState();
	SaveCategoryTreeState( "Category" );
   return retVal;
}

// The user changed the selected collection. This method updates
// the Type and Category controls with the applicable categories.
void CArtBrowserDialog::CollectionChanged(void)
{
   if (m_pCollectionList == NULL)
	{
		INHERITED::CollectionChanged();
		return;
	}

	BeginWaitCursor();

	CPMWCollection* pCollection = GetCollectionSelection();
	if (pCollection == NULL)
	{
		// either "All" collections selected, or root of Supercollection selected
		int nFirst = -1;
		int nLast = -1;

		// If a Supercollection is selected, we need to add
		// categories for all collections in Supercollection.
		int nIndex = m_pCollectionList->GetCurSel();
		CString strColl;
		m_pCollectionList->GetText(nIndex, strColl);
		if (strColl != BrowserAllString())
		{		
			void* pSubList = m_pCollectionList->GetItemList(nIndex);
			pCollection = (CPMWCollection*)m_pCollectionList->GetItemDataPtr(1, pSubList);
			nFirst = m_pCollectionManager->FindCollection(pCollection);
			int nInSuper = m_pCollectionList->GetCount(pSubList);
			pCollection = (CPMWCollection*)m_pCollectionList->GetItemDataPtr(nInSuper-1, pSubList);
			nLast = m_pCollectionManager->FindCollection(pCollection);
		}

		AddAllCategories(nFirst, nLast);
	}
	else // single collection selected
	{
		m_CategoryTreeCtrl.SetRedraw( FALSE );
		BOOL fSaveIgnoreSelectionChange = m_fIgnoreSelectionChange;
		m_fIgnoreSelectionChange = TRUE;
		m_CategoryTreeCtrl.DeleteAllItems();
		m_fIgnoreSelectionChange = fSaveIgnoreSelectionChange;

		CCategoryDatabase* pDatabase;
		if (pCollection->NewCategoryStream(&pDatabase) == ERRORCODE_None)
		{
			// Open up a stream for reading category information.
			CCategoryDir* pCategoryDirectory = pDatabase->GetDir();
			if (pCategoryDirectory != NULL)
				AddCategories(pCollection, pCategoryDirectory, NULL, -1, NULL);

			pCollection->ReleaseCategoryStream(pDatabase);
		}

		Util::SortTreeCtrl( m_CategoryTreeCtrl );
		SetCategorySelection( CString( BrowserAllString() ) );
		m_CategoryTreeCtrl.Expand( m_CategoryTreeCtrl.GetRootItem(), TVE_EXPAND ); // expand "All" entry at top
		m_CategoryTreeCtrl.SetRedraw( TRUE );
		m_CategoryTreeCtrl.RedrawWindow(NULL, NULL, RDW_NOERASE | RDW_UPDATENOW );
	}

	INHERITED::CollectionChanged();

	EndWaitCursor();
}

void CArtBrowserDialog::BringContentIntoWorkspace(CString csPath, CString csFriendlyName,
																  BOOL fCropCopyright /*=FALSE*/)
{
   CPmwView* pView = GetView();
   CPmwDoc* pDoc = GetDocument();

	m_csChosenFileName = csPath;
	m_csChosenFriendlyName = csFriendlyName;

	if (pDoc != NULL)
	{
		GRAPHIC_CREATE_STRUCT* pGcs = pDoc->get_gcs();

		pDoc->SetImportPictureName(csPath, csFriendlyName);
		pGcs->proof = TRUE;
		pGcs->embed_source = m_pPathManager->FileIsRemovable(pGcs->m_csFileName);
		pGcs->m_fSpecialCrop = fCropCopyright;

      if (m_nType == TYPE_ArtGallery)
      {
         if (pView->CreateChosenPicture())
         {
            EndDialog(IDOK);
         }
      }
      else
      {
         // Just end the dialog and return.
         // The caller must decide what to do with it.
         EndDialog(IDOK);
      }
	}
	else
	{
		if (m_nType == TYPE_ModalPicture)
		{
			// Just end the dialog and return.
			// The caller must decide what to do with it.
			EndDialog(IDOK);
		}
		else  // This is the path to enlightenment
		{
			CWnd *p; 
			if ((p = AfxGetMainWnd ()) != NULL) 
			{
				CPmwDoc::SetNewPictureNames(csPath, csFriendlyName,
													 m_pPathManager->FileIsRemovable(csPath), fCropCopyright);
				p->PostMessage (WM_COMMAND, ID_FILE_NEW, 0);
				EndDialog(IDOK);
			}
		}
	}
}

// The user wants to bring the current piece of art into PrintMaster.
void CArtBrowserDialog::DoSelect(void)
{
   if (m_nResultItems > 0 && m_nCurPreviewItem >=0 &&
       m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
   {
      // Only allow user to view content if it was originally included in the product or the user purchases it.
      if (!SelectedItemIsLicensed())
      {
         OnClickedUnlicensed();
         return;
      }

      // Set these three.
      CString  csCollection;
      CString  csFile;
      CString  csFriendlyName;
      CString  csAccessName;
		BOOL		fCropCopyright = FALSE;
      if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber,
                             csCollection, csFile, csFriendlyName, csAccessName, NULL, &fCropCopyright))
      {
         BringContentIntoWorkspace(csAccessName, csFriendlyName, fCropCopyright);
      }
   }
}

// Select a piece of art using the Windows File Open dialog.
void CArtBrowserDialog::OnFileOpenfromdisk() 
{
   OpenFromDisk();
}

void CArtBrowserDialog::OpenFromDisk(BOOL fPhotoCD /*=FALSE*/)
{
   CPmwView* pView = GetView();
   CPmwDoc* pDoc = GetDocument();
   CPmwApp  *pApp = GET_PMWAPP();
   CFrameWnd* pMainWnd = (CFrameWnd*)AfxGetMainWnd();

   // Get a document to use for the picture dialog.
   CPmwDoc* pDialogDoc = pDoc;
   if (pDialogDoc == NULL)
   {
      // Make a temporary document
      pDialogDoc = pApp->NewHiddenDocument();
      if (pDialogDoc == NULL)
      {
         return;
      }
   }

   // Begin our modal state.
   pMainWnd->BeginModalState();

   // Invoke the document to choose a picture.
	CString csPicturePath = m_pPathManager->ExpandPath("[[P]]");
   int nRet = pDialogDoc->ChooseExternalPicture(m_pszLastPicture,
                                                (RectPageObjectPtr)m_pLastPicture,
                                                csPicturePath,
                                                fPhotoCD);

   // End our modal state.
   pMainWnd->EndModalState();

   if (nRet == IDOK)
   {
		GRAPHIC_CREATE_STRUCT* pGCS = pDialogDoc->get_gcs();

		m_csChosenFileName = pGCS->m_csFileName;
		m_csChosenFriendlyName = pGCS->m_csFriendlyName;

		if (pDoc == NULL)
		{
			if (m_nType != TYPE_ModalPicture)
			{
				// This is the path to enlightenment, or to inspiration...

				// No friendly name
				CPmwDoc::SetNewPictureNames (pGCS->m_csFileName, "", pGCS->embed_source);

				pMainWnd->PostMessage (WM_COMMAND, ID_FILE_NEW, 0);
			}
			EndDialog(IDOK);
		}
		else
		{
			// "Modeless" invocation. Create the picture.
         if (m_nType == TYPE_ArtGallery)
         {
				pView->set_arrow_tool();
				if (pDoc->create_chosen_picture(&(pView->get_rc()->source_pbox)) == ERRORCODE_None)
				{
					pDoc->AddCreateCommand(IDCmd_AddPicture);
				}
			}

			EndDialog(IDOK);
		}
   }

   // If we created a document, get rid of it now.
   if (pDialogDoc != pDoc)
   {
      // Close the document.
      pDialogDoc->OnCloseDocument();
   }
}

void CArtBrowserDialog::OnEditPaste() 
{
	COleDataObject dataObj;
	dataObj.AttachClipboard();
	if (m_pDropTarget->IsValidFormat(&dataObj))
	{
		DROPEFFECT de = m_pDropTarget->IsFixedDrive(&dataObj) ?
								DROPEFFECT_LINK : DROPEFFECT_COPY;

		m_pDropTarget->OnDrop(this, &dataObj, de, CPoint(0,0));
	}
}

void CArtBrowserDialog::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	COleDataObject dataObj;
	dataObj.AttachClipboard();
	pCmdUI->Enable(m_pDropTarget->IsValidFormat(&dataObj));
}

void CArtBrowserDialog::UpdateAllBrowsers(void)
{
	for (int i = 0; i < m_cpaOpenBrowsers.GetSize(); i++)
	{
		((CArtBrowserDialog*)m_cpaOpenBrowsers.GetAt(i))->UpdateBrowser();
	}
}

void CArtBrowserDialog::UpdateBrowser(void)
{
	CString strCurCat;
	GetCategorySelection(strCurCat);

	// Reset categories and update result set
	CollectionChanged();
	UpdateResultSet();

	// If previous category was not "All", try resetting the category
	// back to previous selection (this could fail if it no longer exist)
	if (!strCurCat.IsEmpty() && strCurCat != BrowserAllString())
	{
		SetCategorySelection(strCurCat);
	}
}

void CArtBrowserDialog::OnFileOpenPhotoCD()
{
   OpenFromDisk(TRUE);
}

void CArtBrowserDialog::OnMasterArt()
{
   CComboBox* pKeywordList = (CComboBox *)GetDlgItem(IDC_KEYWORD_LIST);
   if (pKeywordList == NULL)
		ASSERT(FALSE);
	else
   {
		// set collection, graphic type, category and tone to "All", then search on "original"

		m_fIgnoreSelectionChange = TRUE;

	   CString csAll(BrowserAllString());

		if (m_pCollectionList)
			m_pCollectionList->SetCurSel(csAll);

		if (m_pGraphicTypeList)
			m_pGraphicTypeList->SetCurSel(csAll);

		HTREEITEM hParent = m_CategoryTreeCtrl.GetRootItem();
		if (hParent)
		{
			m_CategoryTreeCtrl.EnsureVisible( hParent );
			m_CategoryTreeCtrl.SelectItem( hParent );
		}

		if (m_pToneList)
			m_pToneList->SetCurSel(csAll);

      pKeywordList->SetCurSel(1);
      pKeywordList->SetWindowText("original");
		KeywordChanged(-1);

		m_fIgnoreSelectionChange = FALSE;
   }
}

CString CArtBrowserDialog::GetPrintCategory(USHORT uCollectionID, DWORD dwItemNumber)
{
   CPMWCollection* pCollection = m_pCollectionManager->FindCollection((BYTE)uCollectionID, CollectionType(), CollectionSubType());
   CString csCategory = pCollection->GetCategory(dwItemNumber);
   return csCategory;
}

// Process the print art command.
void CArtBrowserDialog::DoPrint(void)
{
   CString csCollection;
   CString csFile;
   CString csFriendlyName;
   CString csPath;      // art path name
      
   CPmwApp  *pApp = GET_PMWAPP ();
   CPmwDoc* pDoc = (CPmwDoc*)(pApp->NewHiddenDocument());
   if(pDoc == NULL)
      return;

   CPrintInfo pInfo;
   CPrintDialog *dlg_save = pInfo.m_pPD;
   BOOL fManualPrint = (GetKeyState(VK_SHIFT) < 0);
   CArtPrintDialog pdlg(pDoc, fManualPrint);
   pInfo.m_pPD = &pdlg;

   if (AfxGetApp()->DoPrintDialog(&pdlg) != IDOK)
   {
      pDoc->OnCloseDocument();
      pInfo.m_pPD = dlg_save;
      return;
   }

   // check print range
   int printRange =  pdlg.GetPrintRange();  // 0=selected, 1=print page, 2=all
   PrintArtEntryPtr entry;
   List picture_list(0, TRUE);
   CString csCategory;

   //  different kinds of print ranges
   switch(printRange) 
   { 
      case 0: // just print selected picture
      {
         // Get file name of selected picture
         if (GetItemInformation(m_uSelectedCollectionID, m_dwSelectedItemNumber, 
                                 csCollection, csFile, csFriendlyName, csPath))
         {
            /* Create the picture entry. */
            if (fManualPrint)
            {
               csCategory = GetPrintCategory(m_uSelectedCollectionID, m_dwSelectedItemNumber);
            }
            if ((entry = new PrintArtEntry(LPCTSTR(csPath), LPCTSTR(csFriendlyName), csCategory)) != NULL)
            {
               /* Add it to the list. */
               picture_list.append(entry);
            }
         }
         break;
      } 
      case 1:   // print all pictures on page
      {
         int nNumberOfItems = m_pItemPreviews->GetSize();

         USHORT uCollectionID = (USHORT)-1;
         DWORD dwItemNumber = (DWORD)-1;
         CPreviewArray::Item* pItem = NULL;

         // Build the picture list.
         for (int i = 0; i < nNumberOfItems; i++) 
         {
            // iterate thru selected collection and add each picture to print list         
            pItem = m_pItemPreviews->GetAt(i);
            //  if (pItem != NULL)
            {
               if (GetItemInformation(pItem->m_uCollectionID, pItem->m_dwItemNumber, 
                                 csCollection, csFile, csFriendlyName, csPath))
               {
                  if (fManualPrint)
                  {
                     csCategory = GetPrintCategory(pItem->m_uCollectionID, pItem->m_dwItemNumber);
                  }
                  /* Create the picture entry. */
                  if ((entry = new PrintArtEntry(LPCTSTR(csPath), LPCTSTR(csFriendlyName), csCategory)) != NULL)
                  {
                  /* Add it to the list. */
                     picture_list.append(entry);
                  }
                  csPath.Empty();   
               }
            }
         }
         break;
      }
      case 2: // print all pictures in selection group
      {
         long lNumberOfItems = m_SearchContext.GetResultCount();
         USHORT uCollectionID = (USHORT)-1;
         DWORD dwItemNumber = (DWORD)-1;

         // Build the picture list.
       for (long i = 0; i < lNumberOfItems; i++) 
         {
            // Get the current search result.
            CPMWCollectionResult Result = m_SearchContext.GetSearchResult(i);
            // Unpack the collection ID and the item number for the
            // current result.
            uCollectionID = Result.GetCollectionID();
            dwItemNumber = Result.GetItemNumber();

            if (fManualPrint)
            {
               csCategory = GetPrintCategory(uCollectionID, dwItemNumber);
            }

            if (GetItemInformation(uCollectionID, dwItemNumber, 
                              csCollection, csFile, csFriendlyName, csPath))
            {
               /* Create the picture entry. */
               if ((entry = new PrintArtEntry(LPCTSTR(csPath), LPCTSTR(csFriendlyName), csCategory)) != NULL)
               {
               /* Add it to the list. */
                  picture_list.append(entry);
               }
               csPath.Empty();   
            }
         }// end of the for loop
         break;
      }  // case 2
      default:
         ;
   } // end of switch on type of print
        
   LPSTR pDir = NULL;  //(LPSTR)LPCTSTR(csFinalName);

   pDoc->SetTitle(GET_PMWAPP()->GetResourceStringPointer (IDS_PICTURES));
   CArtPrintView view(picture_list, pDir);

   pDoc->size_to_paper();

   /* Hook the view into the document. */
   ((CArtPrintDoc *)pDoc)->add_hidden_view(&view);

   /* Print the project. */
   view.PrintProject(&pInfo);

   /* Remove the view before its destructor happens. */
   pDoc->RemoveView(&view);
   pInfo.m_pPD = dlg_save;

   pDoc->OnCloseDocument();
   picture_list.empty();
}

// Art-specific implementation of drag/drop source.
COleDataSource* CArtBrowserDialog::CreateDragDropSource(void)
{
   CArtDataSource* pSource = new CArtDataSource;
   AddDataExchangeFormat(pSource);
   pSource->DelayRenderData(CF_METAFILEPICT);
//   pSource->DelayRenderFileData(CF_DIB);
//   pSource->DelayRenderFileData(CF_BITMAP);

#if 0
   // Create an OBJECTDESCRIPTOR for this operation.
   SIZEL sizel;
   sizel.cx = 2540*2;
   sizel.cy = 2540*2;
   POINTL pointl;
   pointl.x = 2540;
   pointl.y = 2540;

   HGLOBAL hOD = GetObjectDescriptorData(sizel, pointl);
   if (hOD != NULL)
   {
      pSource->CacheGlobalData(Clipboard::m_cfObjectDescriptor, hOD);
   }
#endif

   return pSource;
}

void CArtBrowserDialog::SetLastExportName(LPCSTR pszLastName)
{
   m_csLastExport = pszLastName;
}

void CArtBrowserDialog::GetExportDirectory(CString& csDir)
{
   if (m_csLastExport.IsEmpty())
   {
      // Picture/art subdirectory.
      csDir = "[[P]]";
   }
   else
   {
      Util::SplitPath(m_csLastExport, &csDir, NULL);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CArtBrowserDialog message handlers

BOOL CArtBrowserDialog::OnInitDialog() 
{
   UINT  uiPreviewBackgroundID = IDB_GALLERY_PREVIEW;
   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
   {
      CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());

      rlProductSpecific.LoadBitmap(m_bmpTitle, CSharedIDResourceManager::riArtGalleryTitle, pOurPal);
      rlProductSpecific.LoadBitmap(m_bmpSidebarBkgrnd, CSharedIDResourceManager::riArtGallerySidebar, pOurPal);
      rlProductSpecific.LoadBitmap(m_bmpSidebarBorder, CSharedIDResourceManager::riArtGallerySidebarBorder, pOurPal);
      uiPreviewBackgroundID = IDB_GALLERY_AGPREVIEW;
   }
   else
   {
  	   // load bitmaps - must be done before calling baseclass init
	   Util::LoadResourceBitmap(m_bmpTitle, MAKEINTRESOURCE(IDB_GALLERY_ART_TITLE), pOurPal);
	   Util::LoadResourceBitmap(m_bmpSidebarBkgrnd, MAKEINTRESOURCE(IDB_GALLERY_ART_SIDEBAR), pOurPal);
	   Util::LoadResourceBitmap(m_bmpSidebarBorder, MAKEINTRESOURCE(IDB_GALLERY_ART_SIDEBAR_BORDER), pOurPal);
   }
   Util::LoadResourceBitmap(m_bmpPreviewBkgrnd, MAKEINTRESOURCE(uiPreviewBackgroundID), pOurPal);

	// load menu
	CMenu menu;
	if (CPrintMasterConfiguration::IsAddOn () == TRUE)
	{
		menu.LoadMenu(IDR_ART_BROWSER_MENU_ADDON);
	}   
	else // normal menu
	{
		menu.LoadMenu(IDR_ART_BROWSER_MENU);
      // Remove Online option for American Greetings
      if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
      {
         BOOL bDeleted = menu.DeleteMenu(3, MF_BYPOSITION);
         ASSERT(bDeleted);
      }
	}

	SetMenu(&menu);
	menu.Detach();

	// Now we're ready to call baseclass
   INHERITED::OnInitDialog();

   int   nNumIndicators = sizeof(uArtIndicatorIDs)/sizeof(UINT);
   BOOL bLoaded = m_StatusBar.SetIndicators(uArtIndicatorIDs, nNumIndicators);
   ASSERT(bLoaded);

   for(int i=0;i<m_StatusBar.GetCount();i++)
      SetIndicatorSize(i);

	GET_PMWAPP()->ShowHelpfulHint(PLACE_HINT, PLACE_ArtGallery, this);

	// set up Drag & Drop
	m_pDropTarget = new CArtOleDropTarget;
	m_pDropTarget->Register(this);

   return TRUE;  // return TRUE unless you set the focus to a control
}

// The user wants to leave without selecting anything.
void CArtBrowserDialog::OnFileClosebrowser() 
{
   if (CanClose())
   {
      EndDialog(IDCANCEL);
   }
}

BOOL CArtBrowserDialog::ShowArtButton() 
{
	return (m_nType != TYPE_ModalPicture);
}

/////////////////////////////////////////////////////////////////////////////
// CSentimentBrowserDialog

// Clipboard formats the art browser exports.
UINT CSentimentBrowserDialog::m_cfDataExchange = ::RegisterClipboardFormat("PMG 4.0 Sentiment Browser");

static LPBYTE  s_pFileBuffer = NULL;
static DWORD   s_dwCurOffset = 0;
static DWORD   s_dwBytesRemaining = 0;
static DWORD CALLBACK EditStreamCallback(DWORD dwCookie, LPBYTE pBuffer, LONG cb, LONG *pcb);
CSentimentBrowserDialog::CSentimentBrowserDialog(CWnd* pParent /*=NULL*/)
   : CBrowserDialog(CSentimentBrowserDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CSentimentBrowserDialog)
	//}}AFX_DATA_INIT

	// Override default colors.
   LoadColorsFromINI("SentimentBrowserColors");

   m_csINISection = "SentimentBrowser";

   m_thumbsize = thumbsizeLarge;
   m_ThumbnailMargin.cy = 40;
	m_nCaptionHeight = m_ThumbnailMargin.cy / 2;
}

BOOL CSentimentBrowserDialog::SupportsThumbnailSize(ThumbnailSizes thumbsizeNew)
{
   if(thumbsizeNew != thumbsizeLarge)
      return FALSE;

   return TRUE;
}

void CSentimentBrowserDialog::ComputeLayout()
{
	INHERITED::ComputeLayout();

	CSize OldThumbnailLayout = m_ThumbnailLayout;
	
	m_ThumbnailSize.cy = (m_crPreview.Height()-m_ThumbnailMargin.cy*2 - m_nCaptionHeight*2) / 2;
	m_ThumbnailLayout.cy = m_nFullThumbnailRows = 2;	// Always preview 2 thumbnails

	// Recompute the current row number if the layout has changed.
	if (m_ThumbnailLayout.cx != OldThumbnailLayout.cx)
		m_nFirstRow = (m_nFirstRow * OldThumbnailLayout.cx) / m_ThumbnailLayout.cx;
}

void CSentimentBrowserDialog::PositionControls()
{
	INHERITED::PositionControls();
}

void CSentimentBrowserDialog::UpdateItemPreviews(BOOL fReset)
{
	BOOL	bShow;

	INHERITED::UpdateItemPreviews(fReset);

	if(m_pItemPreviews != NULL && m_pItemPreviews->GetSize() > 0)
	{
		// Position Rich Edit Controls based upon thumbnail locations computed by 
		// base class UpdateItemPreviews
		CPreviewArray::Item* pItemPreview = m_pItemPreviews->GetAt(0);
		m_richEditCtrlSentiment1.SetWindowPos(
			NULL,
			pItemPreview->m_crThumbnail.left,
			pItemPreview->m_crThumbnail.top,
			pItemPreview->m_crThumbnail.Width(),
			pItemPreview->m_crThumbnail.Height(),
			SWP_NOZORDER);

		// Only show second sentiment if item data exists for it
		bShow = FALSE;
		if(m_pItemPreviews->GetSize() > 1)
		{
			bShow = TRUE;
			pItemPreview = m_pItemPreviews->GetAt(1);
			if(pItemPreview)
			{
			m_richEditCtrlSentiment2.SetWindowPos(
				NULL,
				pItemPreview->m_crThumbnail.left,
				pItemPreview->m_crThumbnail.top,
				pItemPreview->m_crThumbnail.Width(),
				pItemPreview->m_crThumbnail.Height(),
				SWP_NOZORDER);
			}
		}
		m_richEditCtrlSentiment2.ShowWindow(bShow);

		// Run through items and send formatted sentiment text to appropriate Rich Edit Control
		for (int i = 0; i < m_pItemPreviews->GetSize(); i++)
		{
			CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(i);
			ASSERT(pItem);
			if(pItem)
			{
				CMlsStringArray csaSentiment;
				GetSentiment(pItem->m_uCollectionID, pItem->m_dwItemNumber,
								 csaSentiment);
				s_dwCurOffset = 0;
				s_pFileBuffer = NULL;
				s_dwBytesRemaining = 0;

				CRichEditCtrl *pRichEditCtrl = NULL;
				switch(i)
				{
				case 0:
					pRichEditCtrl = &m_richEditCtrlSentiment1;
					break;
				case 1:
					pRichEditCtrl = &m_richEditCtrlSentiment2;
					break;
				default:
					ASSERT(0);
					break;
				}
				if(pRichEditCtrl && pRichEditCtrl->GetSafeHwnd() && csaSentiment.GetSize() > 0)
				{
					// Prepare to send formatted text to RTF controls
					CString		csFrontSentiment,csInsideSentiment,csResource;
					CFormatText	rtfText(FT_ANSI);
					HGLOBAL		hBuffer;

					csFrontSentiment = csaSentiment.GetAt(0);
					// Get Inside Sentiment if one exists
					if(m_pItemPreviews->GetSize() > 1)
						csInsideSentiment = csaSentiment.GetAt(1);
					// Remove Leading and Trailing spaces
					Util::Trim(csInsideSentiment);

					// Begin Top level formatting for Headers
					rtfText.BeginGroup ();

					rtfText.SetBold();
					rtfText.SetFont("Arial");
					rtfText.SetFontSize(12);
					rtfText.SetColor(0,0,255);
					rtfText.SetItalic();

					// If Inside sentiment exists, put Front Label in edit control
					// Otherwise it's a single line sentiment
					if(!csInsideSentiment.IsEmpty())
					{
						csResource.LoadString(IDS_SENTIMENT_BROWSER_FRONT);
						rtfText.AddText (csResource.GetBuffer(1));
						rtfText.AddNewParagraph();
					}

					// Begin formatting for sentiment text
					rtfText.BeginGroup ();
					rtfText.AddDeftParagraph();
					rtfText.SetPlain();
					rtfText.SetFont("Arial");
					rtfText.SetFontSize(10);

					// Add Sentiment Body
					// Replace newlines with appropriate RTF commands
					csFrontSentiment.Replace("\n","\r\n\\par ");
					rtfText.AddText (csFrontSentiment.GetBuffer(1));
					// End formatting of sentiment text
					rtfText.EndGroup (1);

					if(!csInsideSentiment.IsEmpty())
					{
						// Replace newlines with appropriate RTF commands
						csInsideSentiment.Replace("\n","\r\n\\par ");

						rtfText.AddNewParagraph();
						rtfText.AddNewParagraph();

						csResource.LoadString(IDS_SENTIMENT_BROWSER_INSIDE);
						rtfText.AddText (csResource.GetBuffer(1));
						rtfText.AddNewParagraph();

						// Begin formatting for sentiment text
						rtfText.BeginGroup ();
						rtfText.AddDeftParagraph();
						rtfText.SetPlain();
						rtfText.SetFont("Arial");
						rtfText.SetFontSize(10);
						rtfText.AddText (csInsideSentiment.GetBuffer(1));
						// End formatting of sentiment text
						rtfText.EndGroup (1);
					}

					rtfText.EndGroup ();	// End top level formatting for Headers

					// Tell RTF formatter that we are done formatting text
					// and want access to RTF data
					hBuffer = rtfText.EndFile();
					char * pBuffer = (char *)GlobalLock(hBuffer);
					
					// Prepare to stream data into RTF edit control
					s_pFileBuffer = (unsigned char *) pBuffer;
					s_dwBytesRemaining = GlobalSize(hBuffer);
#ifdef _DEBUG
#ifdef DUMP_RTF_TO_DISK
					CFile	cfileTemp("c:\\temp\\dump.rtf",CFile::modeCreate|CFile::modeReadWrite);
					cfileTemp.WriteHuge(pBuffer,s_dwBytesRemaining);
					cfileTemp.Close();
#endif
#endif
					s_dwCurOffset = 0;
					EDITSTREAM  editStream;
					editStream.dwCookie=0;
					editStream.dwError=0;
					editStream.pfnCallback=EditStreamCallback;
					// Ask Rich Edit Control to stream data from RTF buffer to its control
					pRichEditCtrl->StreamIn(SF_RTF, editStream);

					// Release RTF buffer
					GlobalUnlock(pBuffer);
					GlobalFree(hBuffer);
				}
			}
		}
   }
}

// Callback function used to stream data from user specified buffer to RichEdit Control
static DWORD CALLBACK EditStreamCallback(DWORD dwCookie, LPBYTE pBuffer, LONG cb, LONG *pcb)
{
   ASSERT(s_pFileBuffer);
   if(s_pFileBuffer == NULL)
      return 1;   // Failure
   ASSERT(pBuffer);
   if(pBuffer)
   {
      LONG  lBytesToRead = cb;

      // Comments on variables:
      // lBytesToRead         Number of bytes requested by RTF control to get
      // s_dwBytesRemaining   Number of bytes remaining from our file buffer
      //                      that have not been read yet.
      // lBytesToRead         Actual number of bytes to be transferred from our
      //                      buffer to callers buffer
      // pcb                  Actual number of bytes transferred to callers
      //                      buffer.

      if(cb > (long)s_dwBytesRemaining)
         lBytesToRead = s_dwBytesRemaining;
      memcpy(pBuffer, &s_pFileBuffer[s_dwCurOffset], lBytesToRead);
      s_dwCurOffset += cb;
      s_dwBytesRemaining -= lBytesToRead;
      *pcb = lBytesToRead;
   }
   return 0;
}

// Compute the proper location and size for the given preview item.
void CSentimentBrowserDialog::GetPreviewBounds(CRect& crThumbnail, CRect& crFocus,
															  int nThumbnailWidth, int nThumbnailHeight,
															  int nItemPos,			// 0-based item
															  int nGroup/*=0*/, // not used
															  int nLastGroupBottomPos/*=0*/)
{
	// NOTE: nItemPos is the element in current visable layout, i.e. the top
	// visible element will be 0, no matter which page is currently showing.

	// Build thumbnail rect with thumbnail bounds.
	crThumbnail.SetRect(0, 0, nThumbnailWidth, nThumbnailHeight);

	// Center thumbnail in available width and height.
	crThumbnail.OffsetRect((m_ThumbnailSize.cx - nThumbnailWidth) / 2,
								  (m_ThumbnailSize.cy-nThumbnailHeight)/2);

	// Offset by position in preview grid.
	int nXOffset = m_PreviewMargin.cx +
			((nItemPos % m_ThumbnailLayout.cx) * (m_ThumbnailSize.cx + m_ThumbnailMargin.cx));

	int nYOffset = m_PreviewMargin.cy +
			((nItemPos / m_ThumbnailLayout.cx) * (m_ThumbnailSize.cy + m_ThumbnailMargin.cy));

	crThumbnail.OffsetRect(nXOffset, nYOffset);

	// Offset thumbnail by horizontal margin
	crThumbnail.OffsetRect(m_ThumbnailMargin.cx / 2, 0);

	// Offset into preview area.
	crThumbnail.OffsetRect(m_crPreview.left, m_crPreview.top);

	// Set Focus area (selection rect)
	crFocus = crThumbnail;
	crFocus.left -= 10;
	crFocus.right += 10;
	crFocus.top -= (m_ThumbnailMargin.cy - m_nCaptionHeight) / 2;
	crFocus.bottom += (m_ThumbnailMargin.cy - m_nCaptionHeight) / 2 + m_nCaptionHeight;
}

void CSentimentBrowserDialog::PaintPreview(CDC* pDC)
{
   // Get the current clip box so we only draw what we need to.
   CRect crClip(0,0,0,0);
   if (pDC->GetClipBox(crClip) == NULLREGION)
      return;  // nothing to do

   CRect crPreviewClip;
   if (!crPreviewClip.IntersectRect(crClip, m_crPreview))
      return;  // nothing to do

	CRect crOverlap;
   if(crOverlap.IntersectRect(crPreviewClip, m_crPreview))
   {
		if (m_pItemPreviews != NULL && m_pItemPreviews->GetSize() > 0)
      {
			for (int i = 0; i < m_pItemPreviews->GetSize(); i++)
			{
				CPreviewArray::Item* pItem = m_pItemPreviews->GetAt(i);
				ASSERT(pItem);
				if(pItem)
				{
					BOOL fSelected = (pItem->m_uCollectionID == m_uSelectedCollectionID) &&
										  (pItem->m_dwItemNumber == m_dwSelectedItemNumber);
					DrawSelect(*pDC, pItem, fSelected);
				}
			}
      }
   }
}

// Trap Events associated with Rich Edit Control
void CSentimentBrowserDialog::OnMessageFilter(NMHDR* pNMHDR, LRESULT* pResult)
{
   MSGFILTER	*pMessageFilter = (MSGFILTER*)pNMHDR;
	long			lParam;
	CPoint		cpMousePos;
	CWnd			*pEditCtrl ;

	switch(pMessageFilter->msg)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
			// Get window handle of control sending message
			pEditCtrl = CWnd::FromHandle(pMessageFilter->nmhdr.hwndFrom);
			if(pEditCtrl && pEditCtrl->GetSafeHwnd())
			{
				// Convert mouse position to parents coordinates 
				cpMousePos.x = LOWORD(pMessageFilter->lParam);
				cpMousePos.y = HIWORD(pMessageFilter->lParam);
				pEditCtrl->ClientToScreen(&cpMousePos);
				pEditCtrl->GetParent()->ScreenToClient(&cpMousePos);
				lParam = MAKELPARAM(cpMousePos.x, cpMousePos.y);
				// Send parent (dialog) mouse message
				pEditCtrl->GetParent()->PostMessage(pMessageFilter->msg, MK_LBUTTON, lParam);
			}
			// eat edit controls mouse messages
			pMessageFilter->msg = 0;
			break;
	}
   
   *pResult = 0;
}

void CSentimentBrowserDialog::DoDataExchange(CDataExchange* pDX)
{
   INHERITED::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CSentimentBrowserDialog)
	DDX_Control(pDX, IDC_SENTIMENT2, m_richEditCtrlSentiment2);
	DDX_Control(pDX, IDC_SENTIMENT1, m_richEditCtrlSentiment1);
	//}}AFX_DATA_MAP
}

BOOL CSentimentBrowserDialog::GetSentiment(USHORT nID, DWORD dwItemNumber, CMlsStringArray   &csaSentiment)
{
   CString           csFile;
   CPMWCollection*   pCollection = NULL;
   CFileItems*       pFileItems = NULL;
   CFileContent*     pContent = NULL;
   BOOL              fExternal = FALSE, fResult = FALSE;
   CContentDirEntry  Dir;
   CContentDataEntry Data;

   ASSERT(CollectionType() == CPMWCollection::typeSentiments);
   if(CollectionType() != CPMWCollection::typeSentiments)
      return fResult;

   TRY
   {
      // Look up the collection by its ID.
      pCollection = m_pCollectionManager->FindCollection((BYTE)nID, CollectionType(), CollectionSubType());
      if (pCollection != NULL)
      {
         // Open up an item information stream so we can get the file name
         // of the given item.
         if (pCollection->NewItemInfoStream(&pFileItems) == ERRORCODE_None)
         {
            // Search for the item.
            CItemsDataEntry Information;
            if (pFileItems->Find(dwItemNumber, &Information) == ERRORCODE_None)
            {
               // Read the item information.
               if (pFileItems->Read() == ERRORCODE_None)
               {
                  // Save the file name and friendly name.
                  csFile = Information.GetFileName();

                  // Everything worked!
                  fResult = TRUE;
               }
            }
         }
         // Determine whether this item is internal or external.
         if (fResult)
         {
            fResult = FALSE;
         
            if (pCollection->NewContentStream(&pContent) == ERRORCODE_None)
            {
               // Search for the item.
               Dir.SetKey(csFile);
               if (pContent->Find(&Dir, &Data) == ERRORCODE_None)
               {
                  fExternal = (Dir.GetStorageMethod() == CSortedDBDirEntry::storageExternal);
                  fResult = TRUE;
               }
            }
         }
      }
   }
   END_TRY

   if (fResult)
   {
      // Build the access file name.
      if (!fExternal)
      {
         // Use the full path.
         if (pContent->Read(Data.GetItemSize()) == ERRORCODE_None)
            csaSentiment.SetNames((const char *)Data.GetData());
      }
      else
      {
         // No support for external Sentiments yet!
         ASSERT(0);
      }
   }

   // Free any resources we may still have open.
   if (pCollection != NULL)
   {
      if (pContent != NULL)
         pCollection->ReleaseContentStream(pContent);
      if (pFileItems != NULL)
         pCollection->ReleaseItemInfoStream(pFileItems);
   }
	return fResult;
}

void CSentimentBrowserDialog::DrawSentimentPreview(CDC& dc,
																	CPreviewArray::Item* pItemPreview,
																	CMlsStringArray &csaSentiment)
{
	if (csaSentiment.GetSize() != 2)
	{
		ASSERT(FALSE);
		return;
	}

	// Draw white box behind sentiment area
	Util::FillRectangleWithColor(dc, pItemPreview->m_crThumbnail, RGB_COLOR_WHITE);

	// Create fonts to use
	CFont fontBold, fontNormal;

	if (m_pPreviewFont != NULL)
	{
		LOGFONT lfFont;
		if (m_pPreviewFont->GetObject(sizeof(lfFont), &lfFont) > 0)
		{
			lfFont.lfHeight = -12;
			lfFont.lfWeight = 700;
			fontBold.CreateFontIndirect(&lfFont);

			lfFont.lfWeight = 400;
			fontNormal.CreateFontIndirect(&lfFont);
		}
	}

	// We need these fonts since we are selecting them below.
	if (fontNormal.GetSafeHandle() == NULL
		 || fontBold.GetSafeHandle() == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	COLORREF clOldTextColor = dc.SetTextColor(m_clPreviewText);
	int nOldBkMode = dc.SetBkMode(TRANSPARENT);

	// Calculate rects

	CRect rcText = pItemPreview->m_crThumbnail;
	rcText.left += 8;
	rcText.right -= 8;
	rcText.top += 8;
	rcText.bottom -= 8;

	CString strFront;
	strFront.LoadString(IDS_FRONT);
	strFront += ":";
	CString strInside;
	strInside.LoadString(IDS_INSIDE);
	strInside += ":";
	// For internationalization, check for longest string
	CString strLonger;
	if (strFront.GetLength() > strInside.GetLength())
		strLonger = strFront;
	else
		strLonger = strInside;

	// Get the size of the rect for the headings (Front/Inside)
	CRect rect = rcText;
	CFont* pOldFont = dc.SelectObject(&fontBold);
	dc.DrawText(strLonger, rect, DT_SINGLELINE | DT_CALCRECT);
	CRect rcHeading = rect;

	// Figure out the rect for the "Front" text
	rcText.left = rcHeading.right + 8;
	rect = rcText;
	CString str = csaSentiment.GetAt(0);
	if (str.IsEmpty())
		str = " ";
	dc.SelectObject(&fontNormal);
	dc.DrawText(str, rect, DT_TOP|DT_NOPREFIX|DT_WORDBREAK|DT_CALCRECT);
	// allow enough room for at least 1 line of "Inside" text
	rcText.bottom = min(rect.bottom, pItemPreview->m_crThumbnail.bottom - 30);

	// Draw "Front" text
	dc.SelectObject(&fontBold);
	dc.DrawText(strFront, rcHeading, DT_TOP);
	dc.SelectObject(&fontNormal);
	dc.DrawText(csaSentiment.GetAt(0), rcText, DT_TOP|DT_NOPREFIX|DT_WORDBREAK);

	// Set the rects for the "Inside" text
	rcHeading.top = rcText.bottom + 8;
	rcHeading.bottom = pItemPreview->m_crThumbnail.bottom - 8;
	rcText.top = rcHeading.top;
	rcText.bottom = rcHeading.bottom;

	// Draw "Inside" text
	dc.SelectObject(&fontBold);
	dc.DrawText(strInside, rcHeading, DT_TOP);
	dc.SelectObject(&fontNormal);
	dc.DrawText(csaSentiment.GetAt(1), rcText, DT_TOP|DT_NOPREFIX|DT_WORDBREAK);

	// cleanup dc
	if (pOldFont)
		dc.SelectObject(pOldFont);

	dc.SetTextColor(clOldTextColor);
	dc.SetBkMode(nOldBkMode);
}

BEGIN_MESSAGE_MAP(CSentimentBrowserDialog, CBrowserDialog)
   //{{AFX_MSG_MAP(CSentimentBrowserDialog)
	ON_NOTIFY(EN_MSGFILTER, IDC_SENTIMENT1, OnMessageFilter)
	ON_NOTIFY(EN_MSGFILTER, IDC_SENTIMENT2, OnMessageFilter)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// The user changed the selected collection. This method
// updates the category combo box with the applicable
// categories.
void CSentimentBrowserDialog::CollectionChanged(void)
{
	if (m_pCollectionList == NULL || m_pCategoryList == NULL)
	{
		INHERITED::CollectionChanged();
		return;
	}

   // Start with an empty category list.
   m_pCategoryList->ResetContent();

   // Put in the "All" entry.
   m_pCategoryList->AddString(BrowserAllString(), (DWORD)-1);

   // Get the selected collection object.
   CPMWCollection* pCollection = GetCollectionSelection();
	if (pCollection == NULL)
	{
		// either "All" collections selected, or root of Supercollection selected
		int nFirst = -1;
		int nLast = -1;

		// If a Supercollection is selected, we need to add
		// categories for all collections in Supercollection.
		int nIndex = m_pCollectionList->GetCurSel();
		CString strColl;
		m_pCollectionList->GetText(nIndex, strColl);
		if (strColl != BrowserAllString())
		{		
			void* pSubList = m_pCollectionList->GetItemList(nIndex);
			pCollection = (CPMWCollection*)m_pCollectionList->GetItemDataPtr(1, pSubList);
			nFirst = m_pCollectionManager->FindCollection(pCollection);
			int nInSuper = m_pCollectionList->GetCount(pSubList);
			pCollection = (CPMWCollection*)m_pCollectionList->GetItemDataPtr(nInSuper-1, pSubList);
			nLast = m_pCollectionManager->FindCollection(pCollection);
		}

		AddAllCategories(nFirst, nLast);
	}
	else // single collection selected
   {
      CCategoryDatabase* pDatabase;
      if (pCollection->NewCategoryStream(&pDatabase) == ERRORCODE_None)
      {
         // Open up a stream for reading category information.
         CCategoryDir* pCategoryDirectory = pDatabase->GetDir();
         if (pCategoryDirectory != NULL)
            AddCategories(pCollection, pCategoryDirectory, 0, m_pCategoryList, NULL, -1);

         pCollection->ReleaseCategoryStream(pDatabase);
      }
   }

   if (m_pCategoryList->GetCount() > 0)
      m_pCategoryList->SetCurSel(0);

   INHERITED::CollectionChanged();
}

void CSentimentBrowserDialog::DoSelect(void)
{
	if (m_nResultItems > 0 && m_nCurPreviewItem >=0 &&
		 m_pItemPreviews && m_pItemPreviews->GetSize() > 0)
	{
		CMlsStringArray curSentiment;
		if(GetSentiment(m_uSelectedCollectionID, m_dwSelectedItemNumber, curSentiment))
		{

			CPmwDoc *pDoc = GetDocument();
			if (pDoc == NULL)	// no open document
			{
				CWnd *p; 
				if ((p = AfxGetMainWnd()) != NULL) 
				{
					CPmwDoc::SetNewSentiment (curSentiment);
      
					p->PostMessage (WM_COMMAND, ID_FILE_NEW, 0);
					EndDialog(IDOK);
				}

				ASSERT (TRUE);
			}
			else
			{
				pDoc->PasteSentiment(curSentiment);
				EndDialog(IDOK);
			}
		}
	}
}


// No printing from Sentiment Gallery
void CSentimentBrowserDialog::DoPrint(void)
{
}

// No drag/drop of sentiments
COleDataSource* CSentimentBrowserDialog::CreateDragDropSource(void)
{
   return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CSentimentBrowserDialog message handlers

BOOL CSentimentBrowserDialog::OnInitDialog() 
{
   UINT  uiPreviewBackgroundID = IDB_GALLERY_PREVIEW;

	// load bitmaps - must be done before calling baseclass init
	if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
	{
		Util::LoadResourceBitmap(m_bmpTitle,			MAKEINTRESOURCE(IDB_GALLERY_AGSENT_TITLE), pOurPal);
		Util::LoadResourceBitmap(m_bmpSidebarBkgrnd, MAKEINTRESOURCE(IDB_GALLERY_AGSENT_SIDEBAR), pOurPal);
		Util::LoadResourceBitmap(m_bmpSidebarBorder, MAKEINTRESOURCE(IDB_GALLERY_AGSENT_SIDEBAR_BORDER), pOurPal);
      uiPreviewBackgroundID = IDB_GALLERY_AGPREVIEW;
	}
	else
	{
		Util::LoadResourceBitmap(m_bmpTitle,			MAKEINTRESOURCE(IDB_GALLERY_SENT_TITLE), pOurPal);
		Util::LoadResourceBitmap(m_bmpSidebarBkgrnd, MAKEINTRESOURCE(IDB_GALLERY_SENT_SIDEBAR), pOurPal);
		Util::LoadResourceBitmap(m_bmpSidebarBorder, MAKEINTRESOURCE(IDB_GALLERY_SENT_SIDEBAR_BORDER), pOurPal);
	}

	Util::LoadResourceBitmap(m_bmpPreviewBkgrnd, MAKEINTRESOURCE(uiPreviewBackgroundID), pOurPal);

   INHERITED::OnInitDialog();

   int   nNumIndicators = sizeof(uSentimentIndicatorIDs)/sizeof(UINT);
   BOOL bLoaded = m_StatusBar.SetIndicators(uSentimentIndicatorIDs, nNumIndicators);
   ASSERT(bLoaded);

   for(int i=0;i<m_StatusBar.GetCount();i++)
      SetIndicatorSize(i);

	GET_PMWAPP()->ShowHelpfulHint("sentiment gallery", this);

	// Trap mouse events for rich edit controls used for displaying sentiments
	long lMask = m_richEditCtrlSentiment1.GetEventMask();
	m_richEditCtrlSentiment1.SetEventMask(lMask | ENM_MOUSEEVENTS);

	lMask = m_richEditCtrlSentiment2.GetEventMask();
	m_richEditCtrlSentiment2.SetEventMask(lMask | ENM_MOUSEEVENTS);

   return TRUE;  // return TRUE unless you set the focus to a control
}

// Export section.
void CSentimentBrowserDialog::SetLastExportName(LPCSTR pszLastName)
{
   ASSERT(FALSE);
}

BOOL CSentimentBrowserDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// Prevent Rich Edit control from changing cursor 
	if(pWnd->GetSafeHwnd() == m_richEditCtrlSentiment1.GetSafeHwnd() ||
		pWnd->GetSafeHwnd() == m_richEditCtrlSentiment2.GetSafeHwnd())
	{
		return TRUE;
	}
	
	return CBrowserDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CSentimentBrowserDialog::GetExportDirectory(CString& csDir)
{
   ASSERT(FALSE);
   csDir.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserDataSource

CBrowserDataSource::CBrowserDataSource()
{
   m_pDialog = NULL;
}

CBrowserDataSource::~CBrowserDataSource()
{
}

void CBrowserDataSource::SetItemInformation(const CString& csCollection,
                                            const CString& csFile,
                                            const CString& csFriendlyName,
														  BOOL fCropCopyright,
                                            CBrowserDialog* pDialog)
{
   m_csCollection = csCollection;
   m_csFile = csFile;
   m_csFriendlyName = csFriendlyName;
	m_fCropCopyright = fCropCopyright;
   m_pDialog = pDialog;
}

BOOL CBrowserDataSource::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
	IPathManager* pPathManager = m_pDialog->GetPathManager();

   /*
   // We're going to be rendering something.
   // Get the current item information.
   */

   TRACE("OnRenderFileData: %u\n", lpFormatEtc->cfFormat);

   switch (lpFormatEtc->cfFormat)
   {
      case CF_HDROP:
      {
         CString csFinalName;

         if (pPathManager->GetPathBindingType(m_csFile) == PBT_CollectionItem)
         {
            ReadOnlyFile Source(m_csFile);
            CString csName;
            CString csExtension;
            CString csTmpFile;

            Util::SplitPath(m_csFile, NULL, NULL, &csName, &csExtension);
            csTmpFile = GET_PMWAPP()->GetTransferDirectory();
            Util::AppendBackslashToPath(csTmpFile);
            csTmpFile += csName;
            csTmpFile += csExtension;

            // We have to copy the data out of the content database to its
            // own file in order to open it.
            StorageFile Dest(csTmpFile);

            if (copy_file(&Source, &Dest) != ERRORCODE_None)
            {
               csTmpFile.Empty();
               Dest.flush();
               Dest.zap();
               return FALSE;
            }
            csTmpFile = pPathManager->ExpandPath(Dest.get_name());
            GET_PMWAPP()->AddTmpFileName(csTmpFile);
            csFinalName = csTmpFile;
         }
         else
         {
            csFinalName = pPathManager->ExpandPath(m_csFile);
         }

         DROPFILES df;

         memset(&df, 0, sizeof(df));
         df.pFiles = sizeof(df);

         pFile->Write(&df, sizeof(df));
         pFile->Write((LPCSTR)csFinalName, csFinalName.GetLength()+1);
         BYTE b0 = 0;
         pFile->Write(&b0, 1);

         return TRUE;
      }
      default:
      {
         break;
      }
   }

   return FALSE;
}

BOOL CBrowserDataSource::OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal)
{
   TRACE("OnRenderGlobalData: %u\n", lpFormatEtc->cfFormat);
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CArtDataSource

CArtDataSource::CArtDataSource()
{
}

CArtDataSource::~CArtDataSource()
{
}

BOOL CArtDataSource::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
	return CBrowserDataSource::OnRenderFileData(lpFormatEtc, pFile);
}

BOOL CArtDataSource::OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal)
{
   switch (lpFormatEtc->cfFormat)
   {
      case CF_METAFILEPICT:
		{
			HGLOBAL hMetaFilePict = RenderAsMetafile();
			if (hMetaFilePict != NULL)
			{
				*phGlobal = hMetaFilePict;
				return TRUE;
			}
			break;
		}
      case CF_DIB:
		{
			break;
		}
		default:
		{
			return CBrowserDataSource::OnRenderGlobalData(lpFormatEtc, phGlobal);
		}
	}
	return FALSE;
}

HGLOBAL CArtDataSource::RenderAsMetafile(void)
{
	HGLOBAL hMetaFilePict = NULL;

	// Create a temporary document to hold our graphic.
	CPmwDoc* pDoc = GET_PMWAPP()->NewHiddenDocument();

	if (pDoc != NULL)
	{
		pDoc->size_to_paper();
		// Create a graphic to draw with.
		GRAPHIC_CREATE_STRUCT* pGcs = pDoc->get_gcs();
		pDoc->SetImportPictureName(m_csFile, m_csFriendlyName);
		pGcs->proof = TRUE;

		if (pDoc->create_chosen_picture() == ERRORCODE_None)
		{
			ASSERT(pDoc->object_list()->first_object() != NULL);

			// We now have our object. Setup the metafile.
			hMetaFilePict = CreateMetafilePictFromObjects(pDoc->object_list());
		}

		// Destroy the document. This frees the graphic as well.
		pDoc->OnCloseDocument();
	}

	return hMetaFilePict;
}
/////////////////////////////////////////////////////////////////////////////
// CProjectDataSource

CProjectDataSource::CProjectDataSource()
{
}

CProjectDataSource::~CProjectDataSource()
{
}

BOOL CProjectDataSource::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
   return CBrowserDataSource::OnRenderFileData(lpFormatEtc, pFile);
}

BOOL CProjectDataSource::OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal)
{
   return CBrowserDataSource::OnRenderGlobalData(lpFormatEtc, phGlobal);
}

/////////////////////////////////////////////////////////////////////////////
// CSentimentDataSource

CSentimentDataSource::CSentimentDataSource()
{
}

CSentimentDataSource::~CSentimentDataSource()
{
}

BOOL CSentimentDataSource::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
   return CBrowserDataSource::OnRenderFileData(lpFormatEtc, pFile);
}

BOOL CSentimentDataSource::OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal)
{
   return CBrowserDataSource::OnRenderGlobalData(lpFormatEtc, phGlobal);
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserStatusBar

CBrowserStatusBar::CBrowserStatusBar()
{
}

CBrowserStatusBar::~CBrowserStatusBar()
{
}

BEGIN_MESSAGE_MAP(CBrowserStatusBar, CStatusBar)
   //{{AFX_MSG_MAP(CBrowserStatusBar)
   ON_WM_LBUTTONDOWN()
   ON_WM_CREATE()
   ON_WM_ERASEBKGND()
   //}}AFX_MSG_MAP
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

LRESULT CBrowserStatusBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
{
   if (GetStyle() & WS_VISIBLE)        // ignore if child is invisible
   {
      CFrameWnd* pTarget = (CFrameWnd*)GetOwner();
      if (pTarget != NULL)
         OnUpdateCmdUI(pTarget, (BOOL)wParam);
   }
   return 0L;
}

int CBrowserStatusBar::GetCount()
{
	return m_nCount;
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserStatusBar message handlers

void CBrowserStatusBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
   UINT  nID, nStyle;
   int   cxWidth;
   for (int nPane = 0; nPane < m_nCount; nPane++)
   {
      CRect crPane;
      // Get the item rect.
      GetItemRect(nPane, crPane);
     if (crPane.PtInRect(point))
      {
         // We hit this pane!
         GetPaneInfo(nPane, nID, nStyle, cxWidth);
         if( (nStyle & SBPS_DISABLED) == 0)
            Util::SendWmCommand(GetParent(), nID, this, 0);
         break;
      }
   }
   
// CStatusBar::OnLButtonDown(nFlags, point);
}

BOOL CBrowserStatusBar::PreCreateWindow(CREATESTRUCT &cs)
{
	cs.style = (cs.style & ~SBARS_SIZEGRIP);
	return CStatusBar::PreCreateWindow(cs);
}

int CBrowserStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CStatusBar::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   return 0;
}


BOOL CBrowserStatusBar::OnEraseBkgnd(CDC* pDC) 
{
   CRect    crClient;

   BOOL bErased = CStatusBar::OnEraseBkgnd(pDC);

   GetClientRect(&crClient);
   CPen *pOldObject = (CPen *)pDC->SelectStockObject(BLACK_PEN);
   pDC->MoveTo(crClient.left,crClient.top);
   pDC->LineTo(crClient.right,crClient.top);
   pDC->SelectObject(pOldObject);

   return bErased;
}


void CArtBrowserDialog::OnOnlineUpdateAccount() 
{
	GetContentManager()->UpdateAccount();
}

void CArtBrowserDialog::OnOnlineCreateAccount() 
{
	if(GetContentManager()->CreateNewAccount())
		EndDialog(IDOK);
}

void CArtBrowserDialog::OnOnlineChangeAccounts() 
{
	if(GetContentManager()->RequestExistingAccount())
		EndDialog(IDOK);
}

void CArtBrowserDialog::OnOnlineDeleteAllDownloadedFiles()
{
	GetContentManager()->DeleteCachedDirectories(this, NULL);
}

void CArtBrowserDialog::OnSelchangedCategoryTree(NMHDR* pNMHDR, LRESULT* pResult)
{
   if (!m_fIgnoreSelectionChange)
   {
      CategoryChanged();
   }

	*pResult = 0;
}


HBRUSH CBrowserDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPmwDialogColor::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_BTN)
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

//
// A little routine to get th browser "All" string.
// It would be nice if this went away.
// 

LPCSTR BrowserAllString(void)
{
	static LPCSTR pszAll = NULL;
	if (pszAll == NULL)
	{
		pszAll = GET_PMWAPP()->GetResourceStringPointer(IDS_ALL);
	}
	return pszAll;
}


