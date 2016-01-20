//
// $Workfile: PMWVIEW.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
//
//
// Revision History:
//
// $Log: /PM8/App/PMWVIEW.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 137   2/19/99 11:32a Johno
// Moved OnWebSiteProperties here from webview.cpp; Enable / disable menu
// item according to project's HTML support
// 
// 136   2/18/99 5:01p Johno
// Use WpPostA in CPmwView::PublishToWeb(); WpPost is no longer present.
// 
// 135   2/11/99 1:07p Cschende
// added drop cap message handlers to the message map
// 
// 134   2/09/99 4:44p Rgrenfel
// Added support for the Made To Order toolbar popup menu for seals and
// timepieces.
// 
// 133   2/08/99 3:17p Rgrenfel
// Added the new commands for logos, initial capitals, smart numbers and
// signatures.
// 
// 132   2/01/99 5:37p Rgrenfel
// Added Seal dialog command to the view.
// 
// 131   2/01/99 2:47p Blee
// beefed up OnUpdateAddPageNumber()
// 
// 130   2/01/99 11:26a Rgrenfel
// Added command for custom Timepieces.
// 
// 129   1/25/99 4:35p Johno
// Moved OnPageProperties() here
// 
// 128   1/25/99 3:38p Rgrenfell
// Added support for object descriptors in Drag/Drop
// 
// 127   1/22/99 4:06p Johno
// Moved / added CmdUI handlers for HTNL menu items
// 
// 126   1/20/99 5:00p Johno
// 
// 125   1/20/99 4:59p Blee
// Added OnUpdateTextBanner to gray out the "Edit Banner Text" command
// when the project type is not a banner.
// 
// 124   1/20/99 10:28a Mwilson
// moved toolbars into seperate DLL
// 
// 123   1/19/99 3:16p Jfleischhauer
// corrected bad comment symbols in header - again
// 
// 122   1/19/99 1:45p Jfleischhauer
// corrected bad comment symbols in header
// 
// 121   1/19/99 1:14p Jfleischhauer
// added command processing for the Photo Workshop popup menu items
// 
// 120   1/14/99 3:43p Jfleischhauer
// additional command handlers for Photo Workshop button and popup window
// 
// 119   1/13/99 1:23p Jfleischhauer
// replaced PhotoEnhancer with Photo Workshop
// 
// 118   1/07/99 6:09p Johno
// Changes to eliminate some h file dependecies
// 
// 117   1/07/99 5:19p Rgrenfel
// Added support to ensure that drag rectangles still draw now that we are
// drag source active.
// 
// 116   1/07/99 3:58p Cschende
// added command handlers for InsertDate
// 
// 115   1/05/99 5:46p Cschende
// added Insert Symbol command handler
// 
// 114   1/04/99 5:12p Johno
// Moved CWebView / CWebDoc code into CPmwDoc / CPmwView.
// 
// 113   12/23/98 12:03p Cschende
// uncommented bullet menu command functions
// 
// 112   12/23/98 11:43a Johno
// Compile update
// 
// 111   12/22/98 10:29a Johno
// Moved most HTML conversion stuff here from webview.*
// 
// 110   12/17/98 3:23p Cschende
// removed temporary bullet selection menu commands
// 
// 109   12/15/98 3:37p Lwilson
// Initial support for PS component editing.
// 
// 108   12/15/98 10:14a Lwilson
// Added headline message map entry.
// 
// 107   12/11/98 1:53p Lwilson
// Added headline dialog support.
// 
// 106   12/10/98 4:11p Cschende
// Added bullet, numbered list and multilevel list support in text
// 
// 105   12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 104   11/30/98 5:35p Johno
// Foo
// 
// 103   11/30/98 2:34p Mwilson
// fix for instruction page
// 
// 102   11/20/98 2:40p Mwilson
// added check to preferences for instruction page enabled
// 
// 101   11/17/98 5:22p Mwilson
// fixed hang when displaying instructions
// 
// 100   11/13/98 12:32p Mwilson
// 
// 99    11/12/98 12:31p Mwilson
// opens instruction page when opening a new project
// 
// 98    11/10/98 2:07p Gbeddow
// oleimpl.h does not exist in VC6
// 
// 97    11/05/98 6:23p Mwilson
// fixed display of instructions in help window.
// 
// 96    10/28/98 2:46p Mwilson
// fixed menu string
// 
// 95    10/28/98 11:14a Mwilson
// fixed paste bug
// 
// 94    10/15/98 3:46p Dennis
// New OCX Connection Manager
// 
// 93    10/13/98 1:21p Mwilson
// changes for instruction page
// 
// 92    10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 91    10/12/98 1:27p Mwilson
// added craft deluxe configuration
// 
// 90    10/01/98 10:25a Mwilson
// added friendly name to instruction page
// 
// 89    9/30/98 4:43p Mwilson
// more changes for instrucition page support
// 
// 88    9/29/98 3:24p Mwilson
// 
// 87    9/29/98 1:07p Mwilson
// added support for view instruction pages in HTML help window
// 
// 86    9/25/98 11:07a Mwilson
// added support for instruction page
// 
// 85    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 84    9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 83    7/21/98 8:39a Jayn
// ChooseFont now supports tables
// 
// 82    7/14/98 5:38p Johno
// Use TestDocumentFlag(CPmwDoc::FLAG_LoadedFromCollection) to test for
// GoToFirstPage() in OnInitialUpdate().
// 
// 81    7/12/98 5:33p Jayn
// Changed how Serif DrawPlus tool bar button worked (add vs. edit)
// 
// 80    7/10/98 6:08p Rlovejoy
// Added command hooks for 'MergeNames'.
// 
// 79    6/21/98 11:56a Cboggio
// Allowed for Serif Draw and YYWB to be removed for localization
// 
// 78    6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 77    6/11/98 9:35a Dennis
// Removed Year you were born from menu for AG.
// 
// 76    6/05/98 7:47p Hforman
// add command handlers for guide creation from menu
// 
// 75    6/05/98 2:12p Jayn
// Overlapping object flashing rectangle(s) in web projects.
// 
// 74    6/05/98 8:53a Fredf
// Changes to automatically bring up the receiver list for the with help
// labels.
// 
// 73    6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 72    6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 71    6/01/98 1:26p Mwilson
// added handler for dircet printing
// 
// 70    5/31/98 7:28p Psasse
// New code for With Help Labels (AddressBook support)
// 
// 69    5/31/98 5:20p Psasse
// fixed initial daily tip coming in under the hub
// 
// 68    5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 67    5/30/98 5:03p Psasse
// With Help Labels support - new message to post for address book
// 
// 66    5/29/98 6:39p Jayn
// Insert column, etc.
// 
// 65    5/26/98 2:39p Jayn
// New table code.
// 
// 64    5/26/98 2:03p Dennis
// Year you were born now appears on menu based upon Product Line from
// configuration object
// 
// 63    5/11/98 9:04p Psasse
// added online menu to postertype
// 
// 62    4/30/98 7:56p Jayn
// Changes for Border Plus and Add Picture From Disk.
// 
// 61    4/24/98 6:33p Fredf
// Fixed bug with simultaneous print previews. Moved preview refresh
// extent pointer into CPmwView... it was a global variable.
// 
// 60    4/18/98 4:24p Fredf
// T-Shirt printing.
// 
// 59    3/21/98 1:18p Fredf
// New menus and toolbar names.
// 
// 58    3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 57    3/19/98 2:32p Jayn
// Finish up master pages.
// 
// 56    3/18/98 12:09p Rlovejoy
// Put up box on YYWB failure.
// 
// 55    3/18/98 11:15a Jayn
// Page numbers
// 
// 54    3/17/98 2:43p Fredf
// New workspace toolbars.
// 
// 53    3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 52    3/11/98 5:13p Johno
// Disadle hyperlink menu item (for non webviews)
// 
// 51    3/11/98 11:20a Rlovejoy
// Call YYWB dialog w/ pointer to view.
// 
// 50    3/10/98 5:24p Jayn
// Master page changes
// 
// 49    3/10/98 10:57a Rlovejoy
// Launches YYWB dialog.
// 
// 48    2/27/98 4:51p Johno
// Moved all web view stuff to webview.cpp / h
// 
// 47    2/27/98 2:31p Dennis
// Replaced blit_scale_pbox with Util::ScalePBOXWithRounding.  Added more
// comments to publish code.
// 
// 46    2/25/98 2:27p Hforman
// add EditBorder functions
// 
// 45    2/25/98 10:40a Jstoner
// table functionality updates...merge/split cells
// row/col selection done
// 
// 44    2/24/98 10:49a Dennis
// Renamed Page Properties control
// 
// 43    2/20/98 7:12p Hforman
// remove param from delete_object()
// 
// 42    2/17/98 11:46a Dennis
// Added Page Background Properties dialog
// 
// 41    2/11/98 12:21p Dennis
// Added Web Background Properties menu handler
// 
// 40    2/11/98 11:36a Jstoner
// changes for table object
// 
// 39    2/11/98 9:29a Dennis
// Added Web Site Properties
// 
// 38    2/10/98 1:26p Dennis
// Misc. cleanup for Web Publishing
// 
// 37    2/04/98 1:04p Dennis
// Added use of global CConnectionManager object
// 
// 36    2/04/98 10:41a Dennis
// Extensive Publish to web site changes
// 
// 35    1/29/98 9:55a Dennis
// Renamed progress control for publish to web site
// 
// 34    1/28/98 5:38p Dennis
// Work in Progress
// 
// 33    1/27/98 5:57p Hforman
// removed extern Clipboard near clipboard
// 
// 32    1/27/98 4:50p Dennis
// More Publish to Web Site work in progress
// 
// 31    1/21/98 9:13a Jstoner
// added command handlers for table cell merge menu items
// 
// 30    1/20/98 11:19a Dennis
// AOL publishing work in progress
// 
// 29    1/13/98 11:15a Jayn
// Transparent GIF support.
// 
// 28    1/05/98 2:17p Dennis
// preview web site code enhancements.
// 
// 27    12/24/97 10:22a Hforman
// 
// 26    12/24/97 9:59a Jayn
// 
// 25    12/24/97 9:17a Dennis
// WEB Preview support
// 
// 24    12/23/97 7:27p Hforman
// add border commands
// 
// 23    12/18/97 3:23p Dennis
// Added preview web site with temp file cleanup.
// 
// 22    12/16/97 12:13p Jstoner
// added auto format menu item and handler
// 
// 21    12/16/97 9:18a Jstoner
// added table support
// 
// 20    12/11/97 4:54p Dennis
// Added Publish to Web... methods
// 
// 19    12/10/97 2:50p Jayn
// Fix make directory selection work under NT.
// 
// 18    12/09/97 4:17p Dennis
// Added state logic for Publishing directory
// 
// 17    12/08/97 2:47p Dennis
// Added capability to choose destination publishing directory.
// 
// 16    12/05/97 5:39p Hforman
// showing project hint in OnInitialUpdate()
// 
// 15    12/04/97 5:49p Hforman
// 
// 14    11/24/97 2:34p Hforman
// 
// 13    11/21/97 5:22p Hforman
// fix DrawGuideline(), etc.
// 
// 12    11/20/97 2:21p Hforman
// remove  m_fGuideToggleReg
// 
// 11    11/19/97 6:43p Hforman
// calling register_toggle() in CreateGuide()
// 
// 10    11/19/97 5:04p Dennis
// Added Zoom to selected object(s)
// 
// 9     11/19/97 2:04p Hforman
// 
// 8     11/14/97 11:49a Hforman
// 
// 7     11/14/97 11:45a Hforman
// add Guide stuff
// 
// 6     11/05/97 5:28p Johno
// Minor changes for "save as graphic"
// 
// 5     11/05/97 2:09p Johno
// Changes for "Save as Graphic" (in save2pict.cpp)
// 
// 4     11/05/97 12:19p Johno
// Added OnFileSaveAsGraphic() 
// 
// 3     11/05/97 10:53a Jstoner
// added some virtual functions to allow calendarview to do it's own
// printing
//  
// 2     10/28/97 5:37p Dennis
// added OnPublishToFolder
// 
//    Rev 1.2   17 Oct 1997 17:34:00   JOE
// multi page calendar modifications
// 
//    Rev 1.1   26 Aug 1997 14:41:08   johno
// Added SetInitialTextStyle function
// Called in OnInitialUpdate, and when pasting a sentiment
// 
// 
//    Rev 1.0   14 Aug 1997 15:24:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:56   Fred
// Initial revision.
// 
//    Rev 1.92   20 Jul 1997 14:19:06   Jay
// ZoomCommand resets tool if in zoom mode.
// 
//    Rev 1.91   11 Jul 1997 17:15:48   Jay
// Refresh semaphore
// 
//    Rev 1.90   01 Jul 1997 17:13:14   johno
// Added IsGoodPrinter to check for BitBlt printer
// 
//    Rev 1.89   13 Jun 1997 14:07:40   Jay
// Makes sure clip_region is NULL before toggle.
// 
//    Rev 1.88   28 May 1997 19:38:04   dennisp
// Added Sentiment browser
// 
//    Rev 1.87   14 May 1997 13:14:42   Jay
// Corrected colors.
// 
//    Rev 1.86   12 May 1997 18:08:32   Jay
// Browser drag & drop
// 
//    Rev 1.85   08 May 1997 11:47:28   johno
// Added WORKSPACE_COLOR to address book
// 
//    Rev 1.84   18 Apr 1997 13:41:46   Jay
// Default stretching
// 
//    Rev 1.83   14 Apr 1997 15:42:40   Jay
// Text box editing changes
// 
//    Rev 1.82   09 Apr 1997 17:14:10   Jay
// PhotoEnhancer
// 
//    Rev 1.81   03 Apr 1997 14:31:26   Jay
// Resource cleanup
// 
//    Rev 1.80   01 Apr 1997 13:16:32   Jay
// Banner preview
// 
//    Rev 1.79   31 Mar 1997 17:12:32   Jay
// Start of NFX code
// 
//    Rev 1.78   28 Mar 1997 17:10:56   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.77   26 Mar 1997 14:52:02   Jay
// Interruptable preview
// 
//    Rev 1.76   25 Mar 1997 13:47:06   Fred
//  
// 
//    Rev 1.75   25 Mar 1997 12:40:20   Jay
// Partial interruptable preview.
// 
//    Rev 1.74   24 Mar 1997 10:35:28   Jay
// Print preview for labels and envelopes
// 
//    Rev 1.73   21 Mar 1997 15:48:58   Jay
// More preview changes.
// 
//    Rev 1.72   21 Mar 1997 11:10:40   Jay
// Print preview changes
// 
//    Rev 1.71   17 Mar 1997 09:40:10   Jay
// Popup warp field palette
// 
//    Rev 1.70   13 Mar 1997 13:17:10   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.69   12 Mar 1997 15:52:12   Jay
// Rotation, etc.
// 
//    Rev 1.68   11 Mar 1997 16:13:52   Jay
// Extensions to the workspace
// 
//    Rev 1.67   06 Mar 1997 17:28:06   Jay
// Print preview stuff
// 
//    Rev 1.66   04 Mar 1997 17:03:48   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.65   21 Feb 1997 15:14:30   Jay
// New mode handling; crop mode support
// 
//    Rev 1.64   19 Feb 1997 16:53:20   Jay
// Snapping
// 
//    Rev 1.63   11 Feb 1997 14:54:40   Jay
// Free draw tool
// 
//    Rev 1.62   07 Feb 1997 14:53:02   Jay
// Rulers on and off. change_view() fixes.
// 
//    Rev 1.61   06 Feb 1997 14:06:46   johno
// ViewText - Zooming is now optional
// 
//    Rev 1.60   06 Feb 1997 12:57:58   Jay
// New Ruler IDs
// 
//    Rev 1.59   06 Feb 1997 09:13:12   Jay
// Rulers!
// 
//    Rev 1.58   03 Feb 1997 17:26:06   johno
// Changes for find / replace
// 
// 
//    Rev 1.57   29 Jan 1997 17:24:36   Jay
// Next and previous object support
// 
//    Rev 1.56   28 Jan 1997 14:10:26   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.55   24 Jan 1997 12:39:12   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.54   23 Jan 1997 11:05:38   Jay
// New popup palettes
// 
//    Rev 1.53   17 Jan 1997 12:13:42   Jay
// New creation tools.
// 
//    Rev 1.52   17 Jan 1997 09:17:30   Jay
// New status panes.
// 
//    Rev 1.51   16 Jan 1997 13:02:44   Jay
// New status stuff. More cleanup.
// 
//    Rev 1.50   15 Jan 1997 10:49:30   Jay
// New toolbars
// 
//    Rev 1.49   06 Jan 1997 17:07:08   Jay
// Character spacing.
// 
//    Rev 1.48   24 Dec 1996 12:07:22   Jay
// Text import.
// 
//    Rev 1.47   05 Dec 1996 12:30:24   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.46   07 Nov 1996 11:12:16   johno
// Moved strings to resource DLL
// 
//    Rev 1.45   01 Nov 1996 17:12:26   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.44   01 Nov 1996 09:33:20   Jay
// Moved over from 3.01.17
// 
//    Rev 1.45   29 Oct 1996 15:30:26   Fred
// Checks for no Twain sources
// 
//    Rev 1.44   23 Oct 1996 00:27:32   Jay
// Passes TRUE for "fPrinting" when setting the info for printer_rc.
// 
//    Rev 1.43   22 Oct 1996 14:22:14   Jay
// Only override HDROP with DIB format (not all of them).
// 
//    Rev 1.42   18 Oct 1996 10:49:52   Jay
// Now sets update time during drop.
// Will paste a CF_DIB from the drop if available.
// 
//    Rev 1.41   10 Oct 1996 14:31:28   Jay
// More dynamic graphic/item updating.
// 
//    Rev 1.40   08 Oct 1996 08:07:08   Fred
//  
// 
//    Rev 1.39   07 Oct 1996 11:44:20   dennisp
// Added improved error message for CTwain.  Added call to DisplayError()
// 
//    Rev 1.38   07 Oct 1996 10:49:38   Jay
// Generalized AddAcquiredFileItem.
// 
//    Rev 1.37   03 Oct 1996 17:16:22   johno
// Aquire menu option
// 
//    Rev 1.36   04 Sep 1996 18:11:26   Jay
// Spell-check fixes.
// 
//    Rev 1.35   06 Aug 1996 14:38:48   Jay
//  
// 
//    Rev 1.34   02 Aug 1996 16:22:34   Jay
// Now saves panel
// 
//    Rev 1.33   02 Aug 1996 15:25:30   Jay
// Bug fixes
// 
//    Rev 1.32   02 Aug 1996 10:47:06   Jay
// More spelling fixes.
// 
//    Rev 1.31   01 Aug 1996 18:20:22   Jay
// Spell check highlight.
// 
//    Rev 1.30   01 Aug 1996 17:46:02   Jay
// Better handling panels, pages, and undo.
// 
//    Rev 1.29   31 Jul 1996 11:57:56   Fred
// 16-bit compatibility
// 
//    Rev 1.28   29 Jul 1996 18:13:00   Fred
//  
// 
//    Rev 1.27   29 Jul 1996 11:22:36   Jay
// Full page
// 
//    Rev 1.26   28 Jul 1996 10:18:54   Jay
// Text color, new color dialog
// 
//    Rev 1.25   26 Jul 1996 18:23:02   Jay
// 
//    Rev 1.24   25 Jul 1996 19:38:46   Jay
//  
// 
//    Rev 1.23   25 Jul 1996 18:21:30   Fred
//  
// 
//    Rev 1.22   25 Jul 1996 13:14:28   Jay
// Multi-page frame glitch correction.
// 
//    Rev 1.21   25 Jul 1996 11:17:02   Jay
// More page stuff
// 
//    Rev 1.20   25 Jul 1996 08:15:06   Fred
// Right mouse button takes down gadget menus
// 
//    Rev 1.19   24 Jul 1996 13:17:16   Fred
// Gadget hookup
// 
//    Rev 1.18   23 Jul 1996 16:54:50   Fred
// Start of button/view hookup
// 
//    Rev 1.17   23 Jul 1996 10:40:48   Jay
// Added newsletters and brochures; removed backgrounds
// 
//    Rev 1.16   18 Jul 1996 11:48:30   Jay
// Now updates frame buttons right away after scroll.
// 
//    Rev 1.15   18 Jul 1996 11:12:06   Jay
// Text Wrap
// 
//    Rev 1.14   17 Jul 1996 15:43:08   Jay
//  
// 
//    Rev 1.13   16 Jul 1996 08:50:52   Fred
// Hooked up address/sender book
// 
//    Rev 1.12   12 Jul 1996 18:01:46   Jay
// Previous and next frame links.
// 
//    Rev 1.11   11 Jul 1996 17:56:44   Jay
//  
// 
//    Rev 1.10   11 Jul 1996 17:46:10   Jay
//  
// 
//    Rev 1.9   11 Jul 1996 11:35:50   Jay
//  
// 
//    Rev 1.8   08 Jul 1996 17:53:34   Jay
// Linked frames
// 
//    Rev 1.7   26 Jun 1996 14:23:18   Jay
//  
// 
//    Rev 1.6   21 Jun 1996 17:51:02   Jay
//  
// 
//    Rev 1.5   17 Jun 1996 12:02:50   Jay
//  
// 
//    Rev 1.4   14 Jun 1996 17:06:38   Jay
//  
// 
//    Rev 1.3   05 Jun 1996 08:22:18   Jay
//  
// 
//    Rev 1.2   24 May 1996 16:12:46   Fred
// TRACEx
// 
//    Rev 1.1   07 May 1996 12:44:52   Jay
// From PMW2
// 
//    Rev 2.64   30 Apr 1996 09:32:22   JAY
// Fixed the banding/non-banding "scaling" problem.
// 
//    Rev 2.63   23 Feb 1996 08:56:28   JAY
// Sets the arrow tool before inserting an OLE object.
// 
//    Rev 2.62   25 Jan 1996 10:54:42   JAY
// Re-enabled drag/drop for 16-bit version.
// 
//    Rev 2.61   18 Jan 1996 17:10:12   JAY
// New project configuration class
// 
//    Rev 2.60   15 Jan 1996 10:26:18   JAY
//  
// 
//    Rev 2.59   26 Dec 1995 10:29:04   FRED
// Project structure changes for mac port
// 
//    Rev 2.58   20 Nov 1995 17:58:42   FRED
// Disabled Drag and Drop for OLE16
// 
//    Rev 2.57   15 Nov 1995 15:47:48   JAY
// More OLE fixes.
// 
//    Rev 2.56   15 Nov 1995 15:23:10   JAY
// OLE fixes, etc.
// 
//    Rev 2.55   10 Oct 1995 08:23:34   JAY
// Now sets item rects for the in-place-active object when view changes position.
// 
//    Rev 2.54   06 Oct 1995 12:00:02   JAY
// New print code, etc.
// 
//    Rev 2.53   29 Sep 1995 16:00:14   JAY
// More drag/drop/paste/OLE stuff
// 
//    Rev 2.52   28 Sep 1995 16:49:44   JAY
// Paste Special
// 
//    Rev 2.51   28 Sep 1995 14:53:24   JAY
// Multi-file drops now work.
// 
//    Rev 2.50   28 Sep 1995 12:13:06   JAY
// More drag/drop stuff
// 
//    Rev 2.49   27 Sep 1995 15:26:30   JAY
// New OLE code
// 
//    Rev 2.48   22 Sep 1995 15:43:22   JAY
// Fix in WIN32 for misdirected tool bar updates.
// 
//    Rev 2.47   01 Sep 1995 14:25:06   JAY
// Added undo for edit title text.
// 
//    Rev 2.46   31 Aug 1995 18:28:16   JAY
// Added handler for ID_SHAPE_DONE.
// 
//    Rev 2.45   30 Aug 1995 14:15:08   JAY
// Uses new PrintPaperInfo() call.
// 
//    Rev 2.44   30 Aug 1995 11:39:40   FRED
// Test print
// 
//    Rev 2.43   24 Aug 1995 16:28:06   JAY
// Added some undo commands (shape and title text creation, rotation)
// 
//    Rev 2.42   24 Aug 1995 14:09:10   JAY
// Removed unused code.
// 
//    Rev 2.41   23 Aug 1995 11:20:42   JAY
// Removed some unused things.
// 
//    Rev 2.40   22 Aug 1995 10:50:00   JAY
// Moved FormatBar setting code to mainfrm.
// 
//    Rev 2.39   21 Aug 1995 18:41:38   FRED
// Modeless picture album
// 
//    Rev 2.38   20 Aug 1995 15:39:48   FRED
// Rotate object control
// 
//    Rev 2.37   15 Aug 1995 14:13:50   JAY
// Some printing corrections.
// 
//    Rev 2.36   15 Aug 1995 14:11:10   FRED
// More Zoom Stuff
// 
//    Rev 2.35   15 Aug 1995 09:18:14   FRED
// Zoom tools
// 
//    Rev 2.34   15 Aug 1995 08:06:20   FRED
// Zoom stuff
// 
//    Rev 2.33   11 Aug 1995 09:38:00   JAY
// Fixed label printing, etc.
// 
//    Rev 2.32   09 Aug 1995 13:10:44   JAY
// Misc changes.
// 
//    Rev 2.31   02 Aug 1995 12:57:48   JAY
// New insert fill-in fields
// 
//    Rev 2.30   02 Aug 1995 08:07:20   FRED
// Fill-in fields stuff.
// 
//    Rev 2.29   28 Jul 1995 12:32:00   JAY
// Printer info. Envelope printing.
// 
//    Rev 2.28   26 Jul 1995 10:52:20   FRED
// New Address Book interface
// 
//    Rev 2.27   21 Jul 1995 17:20:18   JAY
// Label printing
// 
//    Rev 2.26   20 Jul 1995 16:27:10   JAY
//  
// 
//    Rev 2.25   14 Jul 1995 17:23:56   FRED
// Disk open/save from album
// 
//    Rev 2.24   14 Jul 1995 08:41:32   FRED
// When a new warped text box is added,
// the editing dialog is brought up.
// 
// 
//    Rev 2.23   11 Jul 1995 14:19:22   JAY
// Fill-in fields dialog.
// 
//    Rev 2.22   30 Jun 1995 16:35:14   JAY
// New PaperInfo and EditDesktop
// 
//    Rev 2.21   23 Jun 1995 09:07:02   FRED
// Fixed "no shape selection" crash.
// 
// 
//    Rev 2.20   05 Jun 1995 11:33:48   JAY
//  
// 
//    Rev 2.19   19 May 1995 09:25:20   FRED
// Added bounds to path record
// 
//    Rev 2.18   16 May 1995 12:00:24   JAY
// 
//    Rev 2.17   13 May 1995 11:19:48   JAY
//  
// 
//    Rev 2.16   10 May 1995 13:19:50   JAY
// Support for lines, ellipses, and rectangles.
// 
//    Rev 2.15   28 Apr 1995 16:10:54   JAY
// Now handles Txp cursor better (knows if active view or not).
// 
//    Rev 2.14   26 Apr 1995 14:41:36   JAY
//  
// 
//    Rev 2.13   25 Apr 1995 15:25:06   JAY
//  
// 
//    Rev 2.12   25 Apr 1995 09:58:54   JAY
// Get and put warp text.
// 
//    Rev 2.11   20 Apr 1995 16:17:32   FRED
// Warp Text Stuff
// 
//    Rev 2.10   19 Apr 1995 14:44:18   FRED
// Added preliminary support for warped text object.
// 
// 
//    Rev 2.9   19 Apr 1995 12:34:54   JAY
//  
// 
//    Rev 2.8   29 Mar 1995 09:14:36   JAY
// Lots of changes
// 
//    Rev 2.7   20 Mar 1995 16:35:30   JAY
// Beginning of OLE support.
// 
//    Rev 2.6   15 Mar 1995 10:48:12   JAY
// Support for new text editing
// 
//    Rev 2.5   09 Mar 1995 13:50:58   JAY
// Blinking text cursor.
// 
//    Rev 2.4   09 Mar 1995 08:35:14   JAY
// More undo.
// 
//    Rev 2.3   17 Feb 1995 16:47:16   JAY
// Text editing undo and redo.
// 
//    Rev 2.2   15 Feb 1995 17:15:42   JAY
// Undo
// 
//    Rev 2.1   08 Feb 1995 13:33:54   JAY
// Reverted. New series.
// 
//    Rev 1.43   30 Jan 1995 09:33:06   JAY
// Cut and paste
// 
//    Rev 1.42   09 Jan 1995 16:58:26   JAY
// Cut & paste, etc.
// 
//    Rev 1.41   30 Dec 1994 14:01:00   JAY
// New dithering scheme with expanded palette (based on WinG palette).
// 
//    Rev 1.40   08 Dec 1994 08:08:50   JAY
// UserAbort is now global so OnPrint can check it directly.
// 
//    Rev 1.39   16 Sep 1994 14:19:08   JAY
// 
//    Rev 1.38   04 Aug 1994 07:35:00   JAY
// Large model and 32-bit changes
// 
//    Rev 1.37   26 May 1994 13:45:00   JAY
// Now saves rc.destination_hdc properly before using it to prevent re-entrancy
// problems.
// 
//    Rev 1.36   23 May 1994 11:12:52   JAY
// Moved appropriate routines to VIEWCMD.CPP to reduce the size of this file.
// Added PVCS header.

#include "stdafx.h"
ASSERTNAME

#include "pmw.h"
#include "mainfrm.h"

#include <string.h>
#include <ctype.h>
#include <math.h>

#include "pmgobj.h"
#include "hyperrec.h"
#include "imagemap.h"
#include "htmldoc.h"    // For CHTMLTag derived objects
#include "sortarry.h"   // For CSortedDWordArray
#include "filemgr.h"    // For CFileManager
#include "htmlconv.h"   // For HTMLDocConverter class
#include "pmwdoc.h"
#include "webdlg.h"     // For CWebSitePropertiesDialog

#include "pmwview.h"
#include "pmwprint.h"
#include "cntritem.h"

#include "utils.h"

#include "frameobj.h"
#include "dateobj.h"
#include "oleobj.h"

#include "framerec.h"
#include "addrbook.h"

#include "paper.h"
#include "desktop.h"
#include "printer.h"

#include "macrostd.h"

#include "pmwcfg.h"

#include "ruler.h"
#include "util.h"
#include "browser.h"
#include "aboutdlg.h"
#include "hint.h"
#include "toolbar.h"

#include "wpguid.h"

#if _MSC_VER < 1200 // GCB 11/10/98 - oleimpl.h does not exist in VC6
	#include "..\src\oleimpl.h"
#endif
#include "clip.h"

#include "yywbdlg.h"
#include "htmldoc.h"
#include "textrec.h"
// For web publishing
#include "webdef.h"	
#include "WebPubQ.h"
#include "hyplkdlg.h"
#include "commands.h"   // For CCmdChangeSelects in DoHyperlinkObject
#include "grpobj.h"     // For GroupObject
#include "DsnCheck.h"   // For CDesignCheckerDlg
#include "pmdirdlg.h"	// For CChooseDirectoryDialog

// Framework Support.
#include "DataTransfer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CPalette *pOurPal = NULL;

EVENT_INFO event_info;

extern BOOL standard_check_interrupt(VOIDPTR data);

/*
// The redisplay context for a print job.
// This is initialized in OnBeginPrinting when the DC becomes available.
*/

RedisplayContext far printer_rc;          /* Printer context */

/*
// Gamma curves.
*/

BYTE far screen_gamma_curve[256];
BYTE far printer_outline_gamma_curve[256];
BYTE far printer_bitmap_gamma_curve[256];

CTempFileManager CPmwView::m_cfmHelpTempFiles;


/////////////////////////////////////////////////////////////////////////////
// CDragDropState

CDragDropState::CDragDropState()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPmwView
// Web publishing statics, etc.
int					CPmwView::s_nPublishWarn = -1;
int					CPmwView::s_nWebCheck = -1;
CTempFileManager	CPmwView::m_cfmPublishTempFiles;
BOOL					CPmwView::m_fGotDestinationStrings = FALSE;
CString				CPmwView::m_csDestURL;
CString				CPmwView::m_csSiteName;

BOOL
DesignCheckStateGet(void)
{
   if (CPmwView::s_nWebCheck < 0)
   {
      CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
      CString Check = IniFile.GetString(SZ_PUBLISH_SECTION, SZ_PUBLISH_WEBCHECK, "yes");

      CPmwView::s_nWebCheck = Check == "yes" ? 1 : 0;
   }
   
   return CPmwView::s_nWebCheck == 0 ? FALSE : TRUE;
}

void
DesignCheckStatePut(BOOL b)
{
   CPmwView::s_nWebCheck = (b == TRUE) ? 1 : 0;
   CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
   IniFile.WriteString(SZ_PUBLISH_SECTION, SZ_PUBLISH_WEBCHECK, CPmwView::s_nWebCheck == 0 ? "no" : "yes");
}

BOOL
WebPublishWarnGet(void)
{
   if (CPmwView::s_nPublishWarn < 0)
   {
      CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
      CString Check = IniFile.GetString(SZ_PUBLISH_SECTION, SZ_PUBLISH_WARN, "yes");

      CPmwView::s_nPublishWarn = Check == "yes" ? 1 : 0;
   }
   
   return CPmwView::s_nPublishWarn == 0 ? FALSE : TRUE;
}

void
WebPublishWarnPut(BOOL b)
{
   CPmwView::s_nPublishWarn = (b == TRUE) ? 1 : 0;
   CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
   IniFile.WriteString(SZ_PUBLISH_SECTION, SZ_PUBLISH_WARN, CPmwView::s_nWebCheck == 0 ? "no" : "yes");
}

BOOL
CPmwView::QueryPublish()
{
	BOOL ret = TRUE;
	if (WebPublishWarnGet() == TRUE)
	{
		CWebPublishQuery	dlg;
		ret = dlg.DoModal() == IDOK ? TRUE : FALSE;

		if (ret == TRUE)
			WebPublishWarnPut(!dlg.m_fPublishWarning);	
	}
	
	return ret;
}

BOOL
CPmwView::GetDestinationStrings(void)
{
   if (m_fGotDestinationStrings == FALSE)
   {
      CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
      m_csDestURL = IniFile.GetString(SZ_PUBLISH_SECTION, SZ_PUBLISH_DESTURL_ENTRY);
      if(!m_csDestURL.IsEmpty())
         m_csSiteName = IniFile.GetString(SZ_PUBLISH_SECTION, SZ_PUBLISH_SITENAME_ENTRY);
      // This means we looked
      m_fGotDestinationStrings = TRUE;
   }
   
   if ((!m_csDestURL.IsEmpty()) && (!m_csSiteName.IsEmpty()))
      return TRUE;   // This means we have strings

   m_csDestURL.Empty();
   m_csSiteName.Empty();
   
   return FALSE;     // This means we DON'T have strings
}

void
CPmwView::SaveDestinationStrings(LPCSTR Dest, LPCSTR Name)
{
   CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
   IniFile.WriteString(SZ_PUBLISH_SECTION, SZ_PUBLISH_DESTURL_ENTRY, Dest);
   IniFile.WriteString(SZ_PUBLISH_SECTION, SZ_PUBLISH_SITENAME_ENTRY, Name);
   m_fGotDestinationStrings = FALSE;
}

void
CPmwView::DoURLErrorDialog(URLProcessorRetCode rc)
{
   CString  csResource, csMessage;
   switch(rc)
   {
      case enURLErrorNoDefaultBrowser:
      csResource.LoadString(IDS_PUBLISH_NOBROWSER);
      ASSERT(!csResource.IsEmpty());
      AfxMessageBox(csResource);
      break;
   
      default:
      csResource.LoadString(IDS_PUBLISH_OPENURL_ERROR);
      ASSERT(!csResource.IsEmpty());
      csMessage.Format(csResource, rc);
      AfxMessageBox(csMessage);
      break;
   }
}

BOOL
CPmwView::SelectFullHyperlink()
{
   BOOL              ret = FALSE;
   // These may be changed to select the text of one full hyper link
   CHARACTER_INDEX   SelectStart = m_Txp.SelectStart();
   CHARACTER_INDEX   SelectEnd   = m_Txp.SelectEnd();
   
   TRY
   {
      CTextRecord* pText;
      if ((pText = m_Txp.LockText()) != NULL)
      {
         CHARACTER_COUNT TextSize = pText->NumberOfCharacters();

         CTextIterator  Iterator(pText);
         
         DB_RECORD_NUMBER  dbrn = 0;
         CHARACTER_INDEX   i;
         // Find first character within selection with a hyperlink
         for (i = SelectStart; i < SelectEnd; i++)
         {
            Iterator.SetPosition(i);
            if ((dbrn = Iterator.Style().HyperlinkStyle()) != 0)
            {
					SelectStart = i;
               break;   
            }
         }
         // Did we find a hyperlink?
         if (dbrn != 0)
         {
            HyperlinkData Data;
            Iterator.Style().GetHyperlinkData(Data);
				if (Data.IsEnabled() == TRUE)
				{
					// Find the first character of this hyperlink
					for (i = SelectStart; i >= 0; i--)
					{
						Iterator.SetPosition(i);
						if (Iterator.Style().HyperlinkStyle() != dbrn)
							break;
						else
							SelectStart = i;
					}

					// Find the last character of this hyperlink
					for (i = SelectStart; i < TextSize; i++)
					{
						Iterator.SetPosition(i);
						if (Iterator.Style().HyperlinkStyle() != dbrn)
							break;
					}
            
					if (i > SelectStart)
					{
						SelectEnd = i;
						ret = TRUE;
					}
				}
         }
         
         pText->release();
      }
   }
   END_TRY

   if (ret == TRUE)
   {
      m_Txp.SelectText(SelectStart, SelectEnd);
      m_Txp.CalcAll();
   }

   return ret;
}

void
CPmwView::DoHyperlinkText(CPmwDoc* pDoc)
{
   // We must be editing text!
   if (!m_Txp.Valid())
   {
      ASSERT(FALSE);
      return;
   }

   SelectFullHyperlink();  
   
   PMGDatabasePtr pd = m_Txp.Database();
   ASSERT (pd != NULL);
   // Get the style info for the selected text
   CTextStyle Style = m_Txp.SelectStyle();
   HyperlinkData     Data; // ctor initializes this to an "empty" link
   Style.GetHyperlinkData(Data);

   CHyperlinkDialog  Dlg;
   Dlg.SetData(&Data, pDoc);  // The dialog is primed with the existing link if there is one
   DB_RECORD_NUMBER  TextRecordNumber = Style.HyperlinkStyle();
   switch (Dlg.DoModal())
   {
      case IDOK:  // The user created / changed the link
      Dlg.GetData(&Data);
 
      if (TextRecordNumber != 0)    // If there was a link, just change it
      {
         HyperlinkRecord   *pRec;   
         if ((pRec = (HyperlinkRecord*)pd->get_record(TextRecordNumber, NULL, RECORD_TYPE_Hyperlink)) != NULL)
         {
            pRec->SetData(&Data);
            pRec->release();
            ChangeFont(&Style, IDCmd_Hyperlink);
         }
         else
         ASSERT(FALSE);
      }
      else
      {
         TRY
         {
            TextRecordNumber = pd->NewHyperlinkRecord(&Data);

            Style.HyperlinkStyle(TextRecordNumber);

            ChangeFont(&Style, IDCmd_Hyperlink);
			   // Free the creation reference.
			   pd->FreeHyperlinkRecord(TextRecordNumber);
         }
         END_TRY
      }
      pDoc->SetModifiedFlag();
      break;
      
      case IDC_HYPER_REMOVE:
      if (TextRecordNumber != 0)
      {
         Style.HyperlinkStyle(0);
         ChangeFont(&Style, IDCmd_Hyperlink);
      }
      break;
   }
}
     
void
CPmwView::DoHyperlinkObject(CPmwDoc* pDoc, PMGPageObject* pObject)
{
   HyperlinkData  Data;
   pObject->GetHyperlinkData(&Data);

   CHyperlinkDialog  Dlg;
   Dlg.SetData(&Data, pDoc);

   CCmdChangeSelects* pCommand;
   TRY
   {
      pCommand = new CCmdChangeSelects(IDCmd_Hyperlink);
   }
   END_TRY
      
   switch (Dlg.DoModal())
   {
      case IDOK:
      if (pCommand != NULL)
      {
         if (!pCommand->Before(this))
         {
            delete pCommand;
            pCommand = NULL;
         }
      }
      Dlg.GetData(&Data);
      pObject->SetHyperlinkRecord(&Data);   
      pDoc->SetModifiedFlag();
      break;
      
      case IDC_HYPER_REMOVE:
      if (pCommand != NULL)
      {
         if (!pCommand->Before(this))
         {
            delete pCommand;
            pCommand = NULL;
         }
      }
      pObject->KillHyperlinkRecord();
      pDoc->SetModifiedFlag();
      break;

      case IDCANCEL:
      if (pCommand != NULL)
      {
         delete pCommand;
         pCommand = NULL;
      }
      break;
   }
   
   if (pCommand != NULL)
   {
      if (pCommand->After())
      {
         // Add the command for undoing later.
         pDoc->AddCommand(pCommand, FALSE);  // No execute!
      }
      else
      {
         delete pCommand;
      }
   }
}
////
COLORREF CPmwView::m_clrGuides = GUIDE_COLOR;
COLORREF CPmwView::m_clrMasterGuides = MASTER_GUIDE_COLOR;

IMPLEMENT_DYNCREATE(CPmwView, CView)

BEGIN_MESSAGE_MAP(CPmwView, CView)
   //{{AFX_MSG_MAP(CPmwView)
   ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
   ON_COMMAND(ID_CANCEL_EDIT, OnCancelEdit)
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_COMMAND(ID_VIEW_FULL_PAGE, OnViewFullPage)
   ON_COMMAND(ID_VIEW_FULL_WIDTH, OnViewFullWidth)
   ON_WM_SIZE()
   ON_WM_HSCROLL()
   ON_WM_VSCROLL()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_RBUTTONDBLCLK()
   ON_WM_RBUTTONDOWN()
   ON_WM_RBUTTONUP()
   ON_WM_KEYDOWN()
   ON_WM_KEYUP()
   ON_WM_ERASEBKGND()
   ON_COMMAND(ID_PICTURES_IMPORT, OnPictureImport)
   ON_COMMAND(ID_ADD_SENTIMENT, OnAddSentiment)
   ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
   ON_COMMAND(ID_EDIT_GROUP, OnEditGroup)
   ON_COMMAND(ID_EDIT_UNGROUP, OnEditUngroup)
   ON_UPDATE_COMMAND_UI(ID_EDIT_UNGROUP, OnUpdateEditUngroup)
   ON_UPDATE_COMMAND_UI(ID_EDIT_GROUP, OnUpdateEditGroup)
   ON_COMMAND(ID_EDIT_BORDER, OnEditBorder)
   ON_UPDATE_COMMAND_UI(ID_EDIT_BORDER, OnUpdateEditBorder)
   ON_COMMAND(ID_XFLIP, OnXflip)
   ON_COMMAND(ID_YFLIP, OnYflip)
   ON_COMMAND(ID_TEXT_CHOOSEFONT, OnTextChoosefont)
   ON_UPDATE_COMMAND_UI(ID_TEXT_CHOOSEFONT, OnUpdateTextChoosefont)
   ON_WM_CHAR()
   ON_UPDATE_COMMAND_UI(ID_INSERT_FIELD, OnEditingText)
   ON_COMMAND(ID_EDIT_DUPLICATE, OnEditDuplicate)
   ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
   ON_COMMAND(ID_CALENDAR_TITLE, OnCalendarTitle)
   ON_COMMAND(ID_CALENDAR_WHEN, OnCalendarWhen)
   ON_COMMAND(ID_CALENDAR_PICTURE, OnCalendarPicture)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_PICTURE, OnUpdateCalendarPicture)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_TEXT, OnUpdateCalendarText)
   ON_COMMAND(ID_CALENDAR_TEXT, OnCalendarText)
   ON_COMMAND(ID_CALENDAR_CLEAR, OnCalendarClear)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_CLEAR, OnUpdateCalendarClear)
   ON_COMMAND(ID_VIEW_FULL_HEIGHT, OnViewFullHeight)
   ON_COMMAND(ID_CALENDAR_COLOR, OnCalendarColor)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_COLOR, OnUpdateCalendarColor)
   ON_COMMAND(ID_CALENDAR_STYLE, OnCalendarStyle)
   ON_COMMAND(ID_CALENDAR_NAMES, OnCalendarNames)
   ON_COMMAND(ID_CALENDAR_NUMBER, OnCalendarNumber)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_WHEN, OnHaveCalendar)
   ON_COMMAND(ID_TEXT_BANNER, OnTextBanner)
   ON_UPDATE_COMMAND_UI(ID_TEXT_BANNER, OnUpdateTextBanner)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FULL_HEIGHT, OnUpdateViewFullHeight)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FULL_PAGE, OnUpdateViewFullPage)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FULL_WIDTH, OnUpdateViewFullWidth)
   ON_UPDATE_COMMAND_UI(ID_XFLIP, OnSomethingSelected)
   ON_COMMAND(ID_PICTURES_REPLACE, OnPicturesReplace)
   ON_UPDATE_COMMAND_UI(ID_TEXT_EDIT_FRAME, OnUpdateEditFrame)
   ON_COMMAND(ID_TEXT_EDIT_FRAME, OnEditFrame)
   ON_COMMAND(ID_PICTURES_COLOR, OnPicturesColor)
   ON_UPDATE_COMMAND_UI(ID_PICTURES_REPLACE, OnOnePictureSelection)
   ON_COMMAND(ID_EDIT_REPLACE_OBJECT, OnReplaceObject)
   ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE_OBJECT, OnUpdateReplaceObject)
   ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
   ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
   ON_COMMAND(ID_EDIT_CUT, OnEditCut)
   ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
   ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
   ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
   ON_COMMAND(ID_VIEW_50_VIEW, OnView50View)
   ON_UPDATE_COMMAND_UI(ID_VIEW_50_VIEW, OnUpdateView50View)
   ON_COMMAND(ID_VIEW_100_VIEW, OnView100View)
   ON_UPDATE_COMMAND_UI(ID_VIEW_100_VIEW, OnUpdateView100View)
   ON_COMMAND(ID_VIEW_200_VIEW, OnView200View)
   ON_UPDATE_COMMAND_UI(ID_VIEW_200_VIEW, OnUpdateView200View)
   ON_COMMAND(ID_VIEW_AREA_VIEW, OnViewAreaView)
   ON_COMMAND(ID_VIEW_25_VIEW, OnView25View)
   ON_UPDATE_COMMAND_UI(ID_VIEW_25_VIEW, OnUpdateView25View)
   ON_COMMAND(ID_VIEW_400_VIEW, OnView400View)
   ON_UPDATE_COMMAND_UI(ID_VIEW_400_VIEW, OnUpdateView400View)
   ON_COMMAND(ID_PICTURE_ATTRIBUTES, OnPictureAttributes)
   ON_COMMAND(ID_LAYER_BACK, OnSendToBack)
   ON_COMMAND(ID_LAYER_BACKONE, OnBackOne)
   ON_COMMAND(ID_LAYER_FORWARDONE, OnForwardOne)
   ON_COMMAND(ID_LAYER_FRONT, OnSendToFront)
   ON_UPDATE_COMMAND_UI(ID_LAYER_BACK, OnUpdateLayerToBack)
   ON_UPDATE_COMMAND_UI(ID_LAYER_BACKONE, OnUpdateLayerBackOne)
   ON_UPDATE_COMMAND_UI(ID_LAYER_FORWARDONE, OnUpdateLayerForwardOne)
   ON_UPDATE_COMMAND_UI(ID_LAYER_FRONT, OnUpdateLayerToFront)
   ON_COMMAND(ID_POSITION_STANDARD, OnStandardPosition)
   ON_COMMAND(ID_SIZE_STANDARD, OnStandardSize)
   ON_COMMAND(ID_ROTATE_0, OnRotate0)
   ON_UPDATE_COMMAND_UI(ID_ROTATE_0, OnUpdateRotate0)
   ON_COMMAND(ID_ROTATE_45, OnRotate45)
   ON_UPDATE_COMMAND_UI(ID_ROTATE_45, OnUpdateRotate45)
   ON_COMMAND(ID_ROTATE_90, OnRotate90)
   ON_UPDATE_COMMAND_UI(ID_ROTATE_90, OnUpdateRotate90)
   ON_COMMAND(ID_ROTATE_135, OnRotate135)
   ON_UPDATE_COMMAND_UI(ID_ROTATE_135, OnUpdateRotate135)
   ON_COMMAND(ID_ROTATE_180, OnRotate180)
   ON_UPDATE_COMMAND_UI(ID_ROTATE_180, OnUpdateRotate180)
   ON_COMMAND(ID_ROTATE_225, OnRotate225)
   ON_UPDATE_COMMAND_UI(ID_ROTATE_225, OnUpdateRotate225)
   ON_COMMAND(ID_ROTATE_270, OnRotate270)
   ON_UPDATE_COMMAND_UI(ID_ROTATE_270, OnUpdateRotate270)
   ON_COMMAND(ID_ROTATE_315, OnRotate315)
   ON_UPDATE_COMMAND_UI(ID_ROTATE_315, OnUpdateRotate315)
   ON_COMMAND(ID_ROTATE_MORE, OnRotateMore)
   ON_UPDATE_COMMAND_UI(ID_ROTATE_MORE, OnUpdateRotateMore)
   ON_COMMAND(ID_POSITION_VCENTER, OnVcenter)
   ON_UPDATE_COMMAND_UI(ID_POSITION_FULLPAGE, OnOneSelection)
   ON_COMMAND(ID_POSITION_HCENTER, OnHcenter)
   ON_COMMAND(ID_POSITION_CENTERONPAGE, OnCenter)
   ON_COMMAND(ID_POSITION_FULLPAGE, OnFullPage)
   ON_COMMAND(ID_CALENDAR_ATTRIBUTES, OnCalendarAttributes)
   ON_COMMAND(ID_ALIGN_BOTTOM, OnAlignBottom)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_BOTTOM, OnUpdateAlignBottom)
   ON_COMMAND(ID_ALIGN_CENTER, OnAlignCenter)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_CENTER, OnUpdateAlignCenter)
   ON_COMMAND(ID_ALIGN_LEFT, OnAlignLeft)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_LEFT, OnUpdateAlignLeft)
   ON_COMMAND(ID_ALIGN_MIDDLE, OnAlignMiddle)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_MIDDLE, OnUpdateAlignMiddle)
   ON_COMMAND(ID_ALIGN_RIGHT, OnAlignRight)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_RIGHT, OnUpdateAlignRight)
   ON_COMMAND(ID_ALIGN_JUSTIFY, OnAlignJustify)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_JUSTIFY, OnUpdateAlignJustify)
   ON_COMMAND(ID_ALIGN_TOP, OnAlignTop)
   ON_UPDATE_COMMAND_UI(ID_ALIGN_TOP, OnUpdateAlignTop)
   ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
   ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
   ON_WM_INITMENU()
   ON_WM_INITMENUPOPUP()
   ON_UPDATE_COMMAND_UI(ID_POSITION_CENTERONPAGE, OnUpdatePositionStandard)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
   ON_WM_KILLFOCUS()
   ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
   ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
   ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
   ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
   ON_WM_TIMER()
   ON_COMMAND(ID_TEXT_BOLD, OnTextBold)
   ON_UPDATE_COMMAND_UI(ID_TEXT_BOLD, OnUpdateTextBold)
   ON_COMMAND(ID_TEXT_ITALIC, OnTextItalic)
   ON_UPDATE_COMMAND_UI(ID_TEXT_ITALIC, OnUpdateTextItalic)
   ON_COMMAND(ID_TEXT_UNDERLINE, OnTextUnderline)
   ON_UPDATE_COMMAND_UI(ID_TEXT_UNDERLINE, OnUpdateTextUnderline)
   ON_WM_SETFOCUS()
   ON_COMMAND(ID_VIEW_ADDRESS_BOOK, OnViewAddressBook)
   ON_COMMAND(ID_VIEW_SMART_FIELDS, OnViewSmartFields)
   ON_UPDATE_COMMAND_UI(ID_VIEW_SMART_FIELDS, OnUpdateViewSmartFields)
   ON_COMMAND(ID_PICTURES_DRAWING_OBJECT, OnDrawingObject)
   ON_UPDATE_COMMAND_UI(ID_PICTURES_DRAWING_OBJECT, OnUpdateDrawingObject)
   ON_UPDATE_COMMAND_UI(ID_PICTURE_ATTRIBUTES, OnUpdatePictureAttributes)
   ON_COMMAND(ID_INSERT_FIELD, OnInsertField)
   ON_COMMAND(ID_INSERT_MAIL_MERGE_FIELD, OnInsertMailMergeField)
   ON_COMMAND(IDC_PRINT_NAME_LIST, OnSelectMergeNames)
   ON_UPDATE_COMMAND_UI(IDC_PRINT_NAME_LIST, OnUpdateSelectMergeNames)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateViewZoomIn)
   ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateViewZoomOut)
   ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomIn)
   ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomOut)
   ON_COMMAND(ID_FORMAT_PARAGRAPH, OnFormatParagraph)
   ON_COMMAND(ID_FORMAT_TABS, OnFormatTabs)
   ON_COMMAND(ID_FORMAT_FRAME, OnFormatFrame)
   ON_UPDATE_COMMAND_UI(ID_FORMAT_FRAME, OnUpdateFormatFrame)
   ON_COMMAND(ID_TEXT_WRAP, OnTextWrap)
   ON_UPDATE_COMMAND_UI(ID_TEXT_WRAP, OnUpdateTextWrap)
   ON_COMMAND(ID_DELETE_PAGE, OnEditDeletePage)
   ON_UPDATE_COMMAND_UI(ID_DELETE_PAGE, OnUpdateEditDeletePage)
   ON_COMMAND(IDC_SPELLING, OnSpelling)
   ON_UPDATE_COMMAND_UI(IDC_SPELLING, OnUpdateSpelling)
   ON_COMMAND(ID_ADD_PAGE, OnAddPage)
   ON_COMMAND(ID_ADD_CALENDAR_PAGE, OnAddCalendarPage)
   ON_UPDATE_COMMAND_UI(ID_ADD_CALENDAR_PAGE, OnUpdateAddCalendarPage)
   ON_COMMAND(ID_GOTO_PAGE, OnGotoPage)
   ON_UPDATE_COMMAND_UI(ID_GOTO_PAGE, OnUpdateGotoPage)
   ON_COMMAND(ID_GOTO_FIRST_PAGE, OnGotoFirstPage)
   ON_COMMAND(ID_GOTO_LAST_PAGE, OnGotoLastPage)
   ON_COMMAND(ID_GOTO_NEXT_PAGE, OnGotoNextPage)
   ON_UPDATE_COMMAND_UI(ID_GOTO_NEXT_PAGE, OnUpdateGotoNextPage)
   ON_COMMAND(ID_GOTO_PREVIOUS_PAGE, OnGotoPreviousPage)
   ON_UPDATE_COMMAND_UI(ID_GOTO_PREVIOUS_PAGE, OnUpdateGotoPreviousPage)
   ON_UPDATE_COMMAND_UI(ID_GOTO_FIRST_PAGE, OnUpdateGotoFirstPage)
   ON_UPDATE_COMMAND_UI(ID_GOTO_LAST_PAGE, OnUpdateGotoLastPage)
   ON_COMMAND(ID_TEXT_CHOOSECOLOR, OnTextChoosecolor)
   ON_COMMAND(ID_ACQUIRE, OnAcquire)
   ON_UPDATE_COMMAND_UI(ID_ACQUIRE, OnUpdateAcquire)
   ON_COMMAND(ID_SELECT_SOURCE, OnSelectSource)
   ON_UPDATE_COMMAND_UI(ID_SELECT_SOURCE, OnUpdateSelectSource)
   ON_COMMAND(ID_IMPORT_TEXT, OnImportText)
   ON_UPDATE_COMMAND_UI(ID_IMPORT_TEXT, OnUpdateImportText)
   ON_COMMAND(ID_FORMAT_SPACING, OnFormatSpacing)
   ON_UPDATE_COMMAND_UI(ID_TEXT_EDIT_REPLACE, OnUpdateTextEditReplace)
   ON_COMMAND(ID_TEXT_EDIT_REPLACE, OnTextEditReplace)
   ON_COMMAND(ID_VIEW_RULERS, OnRulers)
   ON_UPDATE_COMMAND_UI(ID_VIEW_RULERS, OnUpdateRulers)
   ON_COMMAND(ID_SNAP_TO_RULERS, OnSnapToRulers)
   ON_UPDATE_COMMAND_UI(ID_SNAP_TO_RULERS, OnUpdateSnapToRulers)
   ON_COMMAND(ID_VIEW_GUIDES, OnGuides)
   ON_UPDATE_COMMAND_UI(ID_VIEW_GUIDES, OnUpdateGuides)
   ON_COMMAND(ID_SNAP_TO_GUIDES, OnSnapToGuides)
   ON_UPDATE_COMMAND_UI(ID_SNAP_TO_GUIDES, OnUpdateSnapToGuides)
   ON_COMMAND(ID_LOCK_GUIDES, OnLockGuides)
   ON_UPDATE_COMMAND_UI(ID_LOCK_GUIDES, OnUpdateLockGuides)
   ON_COMMAND(ID_ADD_GUIDE_HORIZONTAL, OnAddHorizontalGuide)
   ON_COMMAND(ID_ADD_GUIDE_VERTICAL, OnAddVerticalGuide)
   ON_COMMAND(ID_VIEW_TEXTBOXES, OnTextBoxOutlines)
   ON_UPDATE_COMMAND_UI(ID_VIEW_TEXTBOXES, OnUpdateTextBoxOutlines)
   ON_COMMAND(ID_ADD_NFX, OnNfxToon)
   ON_UPDATE_COMMAND_UI(ID_ADD_NFX, OnUpdateNfxToon)
//   ON_COMMAND(ID_PHOTO_ENHANCER, OnPhotoEnhancer)
//   ON_UPDATE_COMMAND_UI(ID_PHOTO_ENHANCER, OnUpdatePhotoEnhancer)
	ON_COMMAND(ID_EDIT_PHOTOWORKSHOP_CROPORIENTATION, OnPhotoWorkshopCropOrientation)
	ON_COMMAND(ID_EDIT_PHOTOWORKSHOP_ADJUSTCOLOR, OnPhotoWorkshopAdjustColor)
	ON_COMMAND(ID_EDIT_PHOTOWORKSHOP_BRIGHTNESSFOCUS, OnPhotoWorkshopBrightnessFocus)
	ON_COMMAND(ID_EDIT_PHOTOWORKSHOP_FIXFLAW, OnPhotoWorkshopFixFlaw)
	ON_COMMAND(ID_EDIT_PHOTOWORKSHOP_ARTISTICEFFECTS, OnPhotoWorkshopArtisticEffects)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHOTOWORKSHOP_CROPORIENTATION, OnUpdatePhotoWorkshop)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHOTOWORKSHOP_ADJUSTCOLOR, OnUpdatePhotoWorkshop)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHOTOWORKSHOP_BRIGHTNESSFOCUS, OnUpdatePhotoWorkshop)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHOTOWORKSHOP_FIXFLAW, OnUpdatePhotoWorkshop)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PHOTOWORKSHOP_ARTISTICEFFECTS, OnUpdatePhotoWorkshop)
	ON_UPDATE_COMMAND_UI(ID_PHOTO_WORKSHOP_MENUITEM, OnUpdatePhotoWorkshop)
	ON_COMMAND(ID_PHOTO_WORKSHOP_BUTTON, OnPhotoWorkshopButton)
	ON_UPDATE_COMMAND_UI(ID_PHOTO_WORKSHOP_BUTTON, OnUpdatePhotoWorkshop)
   ON_COMMAND(ID_PHOTO_WORKSHOP_MENU_PALETTE, OnPhotoWorkshopMenuPalette)
	ON_COMMAND(ID_MADE_TO_ORDER_BUTTON, OnMadeToOrderButton)
   ON_COMMAND(ID_MADE_TO_ORDER_MENU_PALETTE, OnMadeToOrderMenuPalette)
	ON_COMMAND(ID_FILE_SAVE_AS_GRAPHIC, OnFileSaveAsGraphic)
	ON_COMMAND(ID_ZOOM_TO_SELECTED, OnZoomToSelected)
	ON_COMMAND(ID_ADD_BORDER, OnAddBorder)
	ON_UPDATE_COMMAND_UI(ID_ADD_BORDER, OnUpdateAddBorder)
	ON_COMMAND(ID_BORDERPLUS, OnBorderPlus)
	ON_UPDATE_COMMAND_UI(ID_BORDERPLUS, OnUpdateBorderPlus)
	ON_COMMAND(ID_YYWB, OnYywb)
	ON_UPDATE_COMMAND_UI(ID_YYWB, OnUpdateYywb)
   ON_UPDATE_COMMAND_UI(IDM_WEB_HYPERLINK, OnUpdateWebHyperlink)
	ON_COMMAND(IDM_WEB_SITE_PROPERTIES, OnWebSiteProperties)
   ON_UPDATE_COMMAND_UI(IDM_WEB_SITE_PROPERTIES, OnUpdateWebSiteProperties)
	ON_COMMAND(IDM_PAGE_PROPERTIES, OnPageProperties)
   ON_UPDATE_COMMAND_UI(IDM_PAGE_PROPERTIES, OnUpdateIfHTML)
	ON_COMMAND(ID_ADD_PAGE_NUMBER, OnAddPageNumber)
   ON_UPDATE_COMMAND_UI(ID_ADD_PAGE_NUMBER, OnUpdateAddPageNumber)
	ON_COMMAND(ID_DRAWPLUS, OnDrawPlus)
	ON_COMMAND(ID_ADD_DRAWING_OBJECT, OnAddDrawingObject)
	ON_UPDATE_COMMAND_UI(ID_ADD_DRAWING_OBJECT, OnUpdateAddDrawingObject)
	ON_COMMAND(ID_EDIT_DRAWING_OBJECT, OnEditDrawingObject)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DRAWING_OBJECT, OnUpdateEditDrawingObject)
	ON_COMMAND(ID_ADD_HEADLINE, OnAddHeadline)
	ON_COMMAND(ID_ADD_PICTURE_FROM_DISK, OnAddPictureFromDisk)
	ON_COMMAND(ID_ONLINE_CREATEACCOUNT, OnOnlineCreateAccount)
	ON_COMMAND(ID_ONLINE_CHANGE_ACCOUNTS, OnOnlineChangeAccounts)
	ON_COMMAND(ID_ONLINE_UPDATEACCOUNT, OnOnlineUpdateAccount)
	ON_COMMAND(ID_ONLINE_DELETE_ALL_DOWNLOADED_FILES, OnOnlineDeleteAllDownloadedFiles)
	ON_COMMAND(ID_TABLE_INSERT, OnTableInsert)
	ON_UPDATE_COMMAND_UI(ID_TABLE_INSERT, OnUpdateTableInsert)
	ON_COMMAND(ID_TABLE_DELETE, OnTableDelete)
	ON_UPDATE_COMMAND_UI(ID_TABLE_DELETE, OnUpdateTableDelete)
	ON_COMMAND(ID_TABLE_FILL_DOWN, OnTableFillDown)
	ON_UPDATE_COMMAND_UI(ID_TABLE_FILL_DOWN, OnUpdateTableFillDown)
	ON_COMMAND(ID_TABLE_FILL_RIGHT, OnTableFillRight)
	ON_UPDATE_COMMAND_UI(ID_TABLE_FILL_RIGHT, OnUpdateTableFillRight)
	ON_COMMAND(ID_TABLE_MERGE_CELLS, OnTableMergeCells)
	ON_UPDATE_COMMAND_UI(ID_TABLE_MERGE_CELLS, OnUpdateTableMergeCells)
	ON_COMMAND(ID_TABLE_SPLIT_CELLS, OnTableSplitCells)
	ON_UPDATE_COMMAND_UI(ID_TABLE_SPLIT_CELLS, OnUpdateTableSplitCells)
	ON_COMMAND(ID_TABLE_FIT_TO_TEXT, OnTableFitToText)
	ON_UPDATE_COMMAND_UI(ID_TABLE_FIT_TO_TEXT, OnUpdateTableFitToText)
   ON_COMMAND(ID_GOTO_INSTRUCTION_PAGE, OnGotoInstructionPage)
   ON_UPDATE_COMMAND_UI(ID_GOTO_INSTRUCTION_PAGE, OnUpdateGotoInstructionPage)
	ON_COMMAND(ID_FORMAT_BULLETS, OnFormatBullets)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_BULLETS, OnUpdateFormatBullets)
	ON_COMMAND(IDM_PUBLISH_TO_FOLDER, OnPublishToFolder)
	ON_UPDATE_COMMAND_UI(IDM_PUBLISH_TO_FOLDER, OnUpdateIfHTML)
	ON_COMMAND(IDM_PUBLISH_TO_WEB, OnPublishToWeb)
	ON_UPDATE_COMMAND_UI(IDM_PUBLISH_TO_WEB, OnUpdateIfHTML)
	ON_COMMAND(IDM_WEB_HYPERLINK, OnWebHyperlink)
	ON_COMMAND(IDM_HOT_SPOT, OnHotSpot)
	ON_UPDATE_COMMAND_UI(IDM_HOT_SPOT, OnUpdateIfHTML)
	ON_COMMAND(IDM_PREVIEW_WEB_SITE, OnPreviewWebSite)
	ON_UPDATE_COMMAND_UI(IDM_PREVIEW_WEB_SITE, OnUpdateIfHTML)
	ON_COMMAND(IDM_WEB_CHECKER, OnWebChecker)
	ON_UPDATE_COMMAND_UI(IDM_WEB_CHECKER, OnUpdateIfHTML)
	ON_COMMAND(IDM_WEB_AUTO, OnWebAuto)
	ON_UPDATE_COMMAND_UI(IDM_WEB_AUTO, OnUpdateWebAuto)
	ON_COMMAND(IDM_WEB_KEEP_TEXT, OnWebKeepText)
	ON_UPDATE_COMMAND_UI(IDM_WEB_KEEP_TEXT, OnUpdateIfHTMLText)
	ON_COMMAND(IDM_WEB_AS_GRAPHIC, OnWebAsGraphic)
	ON_UPDATE_COMMAND_UI(IDM_WEB_AS_GRAPHIC, OnUpdateIfHTMLText)
	ON_COMMAND(ID_FORMAT_INSERT_SYMBOL, OnFormatInsertSymbol)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_INSERT_SYMBOL, OnUpdateFormatInsertSymbol)
   ON_COMMAND(ID_FILE_PRINT_DIRECT, PrintNoDialog)
   ON_UPDATE_COMMAND_UI(ID_INSERT_MAIL_MERGE_FIELD, OnEditingText)
   ON_WM_DROPFILES()
   ON_UPDATE_COMMAND_UI(ID_SIZE_STANDARD, OnSomethingSelected)
   ON_UPDATE_COMMAND_UI(ID_POSITION_STANDARD, OnUpdatePositionStandard)
   ON_UPDATE_COMMAND_UI(ID_YFLIP, OnSomethingSelected)
   ON_UPDATE_COMMAND_UI(ID_EDIT_DUPLICATE, OnSomethingSelected)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_STYLE, OnHaveCalendar)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_NUMBER, OnHaveCalendar)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_NAMES, OnHaveCalendar)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_TITLE, OnHaveCalendar)
   ON_UPDATE_COMMAND_UI(ID_PICTURES_COLOR, OnOnePictureSelection)
   ON_UPDATE_COMMAND_UI(ID_POSITION_HCENTER, OnUpdatePositionStandard)
   ON_UPDATE_COMMAND_UI(ID_POSITION_VCENTER, OnUpdatePositionStandard)
   ON_UPDATE_COMMAND_UI(ID_CALENDAR_ATTRIBUTES, OnUpdateCalendarColor)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_SPECIAL, OnUpdateEditPasteSpecial)
   ON_UPDATE_COMMAND_UI(ID_FORMAT_PARAGRAPH, OnEditingText)
   ON_UPDATE_COMMAND_UI(ID_FORMAT_TABS, OnEditingText)
   ON_UPDATE_COMMAND_UI(ID_TEXT_CHOOSECOLOR, OnEditingText)
   ON_UPDATE_COMMAND_UI(ID_FORMAT_SPACING, OnEditingText)
	ON_COMMAND(ID_MERGE_NAMES, OnSelectMergeNames)
	ON_UPDATE_COMMAND_UI(ID_MERGE_NAMES, OnUpdateSelectMergeNames)
	ON_COMMAND(ID_FORMAT_INSERTDATE, OnFormatInsertdate)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_INSERTDATE, OnUpdateFormatInsertdate)
	//}}AFX_MSG_MAP
   ON_COMMAND(ID_EDIT_PASTE_SPECIAL, OnEditPasteSpecial)
   // Standard printing commands
   ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
   // My stuff!
   ON_COMMAND(ID_TEXTEDIT_DONE, OnTextEditDone)
   ON_COMMAND(IDC_TEXT_STRETCHING, OnTextStretching)
   ON_UPDATE_COMMAND_UI(IDC_TEXT_STRETCHING, OnUpdateTextStretching)
   // Text size support.
   ON_CBN_KILLFOCUS(IDC_TEXT_SIZE, OnTextSizeKillFocus)
   ON_UPDATE_COMMAND_UI(IDC_TEXT_SIZE, OnUpdateTextSize)
   // Text typeface support.
   ON_CBN_KILLFOCUS(IDC_TEXT_TYPEFACE, OnTextTypeface)
   ON_UPDATE_COMMAND_UI(IDC_TEXT_TYPEFACE, OnUpdateTextTypeface)
   // Rotate support.
   ON_COMMAND(ID_STARTUP_SMART_FIELDS, OnStartupSmartFields)
   // Link text support.
   ON_COMMAND(ID_TEXT_LINK, OnTextLink)
   ON_COMMAND(ID_TEXT_PREVIOUS_FRAME, OnTextPreviousFrame)
   ON_COMMAND(ID_TEXT_NEXT_FRAME, OnTextNextFrame)
   ON_COMMAND(ID_X_RULER, OnClickedRuler)
   ON_COMMAND(ID_Y_RULER, OnClickedRuler)
   ON_COMMAND(ID_RULER_CORNER, OnClickedRuler)
   ON_COMMAND(ID_INDICATOR_POSITION, OnPositionIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_POSITION, OnUpdatePositionIndicator)
   ON_COMMAND(ID_INDICATOR_SIZE, OnSizeIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_SIZE, OnUpdateSizeIndicator)
   ON_COMMAND(ID_INDICATOR_ROTATE, OnRotateIndicator)
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_ROTATE, OnUpdateRotateIndicator)
   // Drawing tool support.
   ON_COMMAND(ID_TEXT_CREATE, OnTextTool)
   ON_UPDATE_COMMAND_UI(ID_TEXT_CREATE, OnUpdateTextTool)
   ON_COMMAND(ID_DRAW_FREEDRAW, OnFreeDrawTool)
   ON_UPDATE_COMMAND_UI(ID_DRAW_FREEDRAW, OnUpdateFreeDrawTool)
   ON_COMMAND(ID_DRAW_LINE, OnLineTool)
   ON_UPDATE_COMMAND_UI(ID_DRAW_LINE, OnUpdateLineTool)
   ON_COMMAND(ID_DRAW_RECTANGLE, OnRectangleTool)
   ON_UPDATE_COMMAND_UI(ID_DRAW_RECTANGLE, OnUpdateRectangleTool)
   ON_COMMAND(ID_DRAW_ELLIPSE, OnEllipseTool)
   ON_UPDATE_COMMAND_UI(ID_DRAW_ELLIPSE, OnUpdateEllipseTool)
   ON_COMMAND(ID_FILL_COLOR, OnFillColor)
   ON_COMMAND(ID_FILL_COLOR_PALETTE, OnFillColorPalette)
   ON_UPDATE_COMMAND_UI(ID_FILL_COLOR, OnUpdateFillColor)
   ON_COMMAND(ID_OUTLINE_WIDTH, OnLineWidth)
   ON_COMMAND(ID_OUTLINE_WIDTH_PALETTE, OnLineWidthPalette)
   ON_UPDATE_COMMAND_UI(ID_OUTLINE_WIDTH, OnUpdateLineWidth)
   ON_COMMAND(ID_OUTLINE_COLOR, OnLineColor)
   ON_COMMAND(ID_OUTLINE_COLOR_PALETTE, OnLineColorPalette)
   ON_UPDATE_COMMAND_UI(ID_OUTLINE_COLOR, OnUpdateLineColor)
   ON_COMMAND(ID_SHADOW, OnShadow)
   ON_COMMAND(ID_SHADOW_PALETTE, OnShadowPalette)
   ON_UPDATE_COMMAND_UI(ID_SHADOW, OnUpdateShadow)
   ON_COMMAND(ID_SHADOW_COLOR, OnShadowColor)
   ON_COMMAND(ID_SHADOW_COLOR_PALETTE, OnShadowColorPalette)
   ON_UPDATE_COMMAND_UI(ID_SHADOW_COLOR, OnUpdateShadowColor)
   ON_COMMAND(ID_TEXT_SHAPE, OnTextShape)
   ON_COMMAND(ID_TEXT_SHAPE_PALETTE, OnTextShapePalette)
   ON_UPDATE_COMMAND_UI(ID_TEXT_SHAPE, OnUpdateTextShape)
   ON_COMMAND(ID_PREVIOUS_OBJECT, OnPreviousObject)
   ON_UPDATE_COMMAND_UI(ID_PREVIOUS_OBJECT, OnUpdatePreviousObject)
   ON_COMMAND(ID_NEXT_OBJECT, OnNextObject)
   ON_UPDATE_COMMAND_UI(ID_NEXT_OBJECT, OnUpdateNextObject)
   ON_COMMAND(ID_CROP, OnCrop)
   ON_UPDATE_COMMAND_UI(ID_CROP, OnUpdateCrop)
   ON_COMMAND(ID_RESET_CROPPING, OnResetCropping)
   ON_UPDATE_COMMAND_UI(ID_RESET_CROPPING, OnUpdateResetCropping)
	ON_COMMAND(ID_ADD_TABLE, OnAddTable)
   ON_UPDATE_COMMAND_UI(ID_ADD_TABLE, OnUpdateAddTable)
	ON_COMMAND(ID_AUTOFORMAT_TABLE, OnAutoFormat)
   ON_UPDATE_COMMAND_UI(ID_AUTOFORMAT_TABLE, OnUpdateAutoFormat)
	ON_COMMAND(ID_CELL_FORMATTING, OnCellFormatting)
   ON_UPDATE_COMMAND_UI(ID_CELL_FORMATTING, OnUpdateCellFormatting)
	ON_COMMAND(ID_ADD_CUSTOMGRAPHICS_TIMEPIECES, OnAddTimePiece)
	ON_COMMAND(ID_ADD_CUSTOMGRAPHICS_SEAL, OnAddSeal)
	ON_COMMAND(ID_ADD_CUSTOMGRAPHICS_LOGO, OnAddLogo)
	ON_COMMAND(ID_ADD_CUSTOMGRAPHICS_INITIALCAP, OnAddInitCap)
	ON_COMMAND(ID_ADD_CUSTOMGRAPHICS_NUMBER, OnAddNumber)
	ON_COMMAND(ID_ADD_CUSTOMGRAPHICS_SIGNATURE, OnAddSignature)

   //With Help support
   ON_COMMAND(PMWM_WH_VIEW_ADDRESS_BOOK, OnWithHelpViewAddressBook)

	ON_COMMAND(ID_FORMAT_DROPCAPS_FOURLINES, OnFormatDropcapsFourlines)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_DROPCAPS_FOURLINES, OnUpdateFormatDropcapsFourlines)
	ON_COMMAND(ID_FORMAT_DROPCAPS_NONE, OnFormatDropcapsNone)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_DROPCAPS_NONE, OnUpdateFormatDropcapsNone)
	ON_COMMAND(ID_FORMAT_DROPCAPS_THREELINES, OnFormatDropcapsThreelines)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_DROPCAPS_THREELINES, OnUpdateFormatDropcapsThreelines)
	ON_COMMAND(ID_FORMAT_DROPCAPS_TWOLINES, OnFormatDropcapsTwolines)
	ON_UPDATE_COMMAND_UI(ID_FORMAT_DROPCAPS_TWOLINES, OnUpdateFormatDropcapsTwolines)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPmwView construction/destruction

CPmwView::CPmwView()
      : current_view(VIEW_NONE),
		  current_object(NULL),
        horizontal_scroll_shown(FALSE),
        vertical_scroll_shown(FALSE),
        edit_event_handler(NULL),
        marquee(NULL),
        m_Txp(NULL),
        m_fDefaultStretching(FALSE),
        m_edit_background(NULL),
        m_pCommand(NULL),
        m_fZooming(FALSE),
        m_fNoPrintDialog(FALSE),
		  m_fRefreshSemaphore(FALSE),
		  m_OverlappingExtents(sizeof(PBOX)),
		  m_fDragActive(FALSE),
		  m_fLocalDragDrop(FALSE)
{
   rc.outline_gamma_curve =
      rc.bitmap_gamma_curve = screen_gamma_curve;
   rc.source_pbox.x0 =
      rc.source_pbox.y0 =
      rc.source_pbox.x1 =
      rc.source_pbox.y1 = 0;

   m_pDesktop = NULL;
#ifdef WINLOGO
   m_pDropTarget = NULL;
#endif
   m_pLinkFrameButton = NULL;
   m_pNextFrameButton = NULL;
   m_pPreviousFrameButton = NULL;

   m_nTypeToCreate = 0;
   m_pCreatedObject = NULL;

   m_fStuckTool = FALSE;
   m_pShapeEntry = NULL;

   m_pXRuler = NULL;
   m_pYRuler = NULL;
   m_pXYRuler = NULL;

	m_pPreviewExtent = NULL;

	m_fBringUpReceiverList = FALSE;
}

CPmwView::~CPmwView()
{
/*
// Make sure we delete all the update states in the refresh array.
// They may have GDI objects in them.
*/

   m_RefreshArray.FreeAllExtents();

/*
// If the edit background has been created and not freed, free it now.
*/

   free_edit_background();

/*
// Get rid of any desktop we have.
*/

   delete m_pDesktop;

#ifdef WINLOGO
/*
// Get rid of any drop target we have.
*/

   delete m_pDropTarget;
#endif

   // Delete the buttons we use for text frames.
   delete m_pLinkFrameButton;
   delete m_pNextFrameButton;
   delete m_pPreviousFrameButton;

   // Get rid of the rulers.
   delete m_pXRuler;
   delete m_pYRuler;
   delete m_pXYRuler;
}

BOOL CPmwView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style |= WS_CLIPCHILDREN;
   return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPmwView drawing

VOID draw_page(CPmwDoc *pDoc, RedisplayContext *pContext)
{
   HDC hDC = pContext->destination_hdc;
   HBRUSH hBrush;

   PPNT dimensions = pDoc->get_dimensions();
   PBOX pbox;
   RECT r;

   pbox.x0 = 0;
   pbox.y0 = 0;
   pbox.x1 = dimensions.x;
   pbox.y1 = dimensions.y;

   pContext->pbox_to_screen(&pbox, TRUE);
   pContext->convert_pbox(&pbox, &r, NULL);

/* draw the page itself */

   hBrush = (HBRUSH)SelectObject( hDC, GetStockObject( WHITE_BRUSH ));

   Rectangle( hDC, r.left, r.top, r.right, r.bottom );

   SelectObject( hDC, hBrush );
}

BOOL ComputeZoomFraction(PCOORD* pN, PCOORD* pD, VIEW_TYPE new_view)
{
   #define MAX_ZOOM_FACTOR    (10.0)
   #define ZOOM_STEPS         (6.0)
   #define ZOOM_STEP_FACTOR   (pow(MAX_ZOOM_FACTOR, 1.0/ZOOM_STEPS))
   
   BOOL fResult = FALSE;
   
   if (*pN == 0)
   {
      *pN = 1;
   }
   
   if (*pD == 0)
   {
      *pD = 1;
   }
   
   double n = (double)(*pN);
   double d = (double)(*pD);
   double r = (log(n)-log(d))/log(ZOOM_STEP_FACTOR);
   
   if (new_view == VIEW_ZOOM_IN)
   {
      r += 1.0;
      if (r > ZOOM_STEPS)
      {
         r = ZOOM_STEPS;
      }
      fResult = TRUE;
   }
   
   if (new_view == VIEW_ZOOM_OUT)
   {
      r -= 1.0;
      if (r < -ZOOM_STEPS)
      {
         r = -ZOOM_STEPS;
      }
      fResult = TRUE;
   }
   
   if (fResult)
   {
      if (r >= 0.0)
      {
         *pN = (PCOORD)(pow(ZOOM_STEP_FACTOR, r)*100.0+0.5);
         *pD = (PCOORD)100;
      }
      else
      {
         *pN = (PCOORD)100;
         *pD = (PCOORD)(pow(ZOOM_STEP_FACTOR, -r)*100.0+0.5);
      }
   }
   
   return fResult;
}

void CPmwView::OnDraw(PrintContext* pc)
{
   OnDraw(&pc->m_dcPrint);
}

void CPmwView::OnDraw(CDC* pDC)
{
   CPmwDoc *pDoc = GetDocument();

   if (pDC->IsPrinting())
   {
      if (printer_rc.is_print_preview)
      {
         // Print preview. Use the global preview extent.
         ASSERT(m_pPreviewExtent != NULL);
         if (m_pPreviewExtent != NULL)
         {
            CStdMacroServer* pMacroServer = pDoc->GetMacroServer();

            BOOL fOldShow = pMacroServer->ShowMacroValues();
            pMacroServer->ShowMacroValues(TRUE);
            pDoc->ReflowMacros();

            // Draw all objects into this extent.
            if (!pDoc->draw_objects(m_pPreviewExtent, &printer_rc))
            {
               // Make sure our update object is NULL (this is our check).
               ASSERT(m_pPreviewExtent->update_object == NULL);
               m_pPreviewExtent->update_object = NULL;
            }

            if (!fOldShow)
            {
               pMacroServer->ShowMacroValues(fOldShow);
               pDoc->ReflowMacros();
            }
         }
      }
      else
      {
         // Normal print.
         // Update all the objects into the DC.
         REFRESH_EXTENT re;

         // Build the refresh extent.

         re.extent = printer_rc.source_pbox;
         re.refresh_type = REFRESH_ALL;
         re.my_object = NULL;
         re.update_object = NULL;
         re.update_state = NULL;

         // Draw all objects into this extent.
         pDoc->draw_objects(&re, &printer_rc);
      }
   }
   else
   {
   /*
   // Drawing is deferred until Idle time.
   */
      RECT clip;
      PBOX extent;

   /* Get the clipping box. */

      pDC->GetClipBox(&clip);

//    od("clip: %d, %d, %d, %d\r\n", clip);

   /* Compute the page box for this clipped area. */

      clip.left -= 4;
      clip.top -= 4;
      clip.bottom += 4;
      clip.right += 4;
      rc.screen_to_pbox(&clip, &extent);
      extent.x0 -= 4*rc.redisplay_x_pixel;
      extent.y0 -= 4*rc.redisplay_y_pixel;
      extent.x1 += 4*rc.redisplay_x_pixel;
      extent.y1 += 4*rc.redisplay_y_pixel;

      add_refresh(&extent);
   }
}

VOID CPmwView::generate_print_vars(PrintContext *pc, RECT r)
{
/* Get the real (unscaled) resolution. */
   SHORT real_x_resolution = pc->m_dcPrint.GetDeviceCaps(LOGPIXELSX);
   SHORT real_y_resolution = pc->m_dcPrint.GetDeviceCaps(LOGPIXELSY);

/* Handle the driver scaling factor if necessary. */

   POINT p = pc->m_scaling;

   if (p.x != 0 || p.y != 0)
   {
      real_x_resolution >>= p.x;
      real_y_resolution >>= p.y;

      /*
      // When banding, the rectangle has not yet been scaled,
      // so we need to do it now.
      // When not banding, the rectangle has already been scaled (DPtoLP).
      */

      if (pc->m_fBanding)
      {
         if (p.x != 0)
         {
            r.left >>= p.x;
            r.right >>= p.x;
         }

         if (p.y != 0)
         {
            r.top >>= p.y;
            r.bottom >>= p.y;
         }
      }

      pc->m_dcPrint.SetMapMode(MM_ANISOTROPIC);
      pc->m_dcPrint.SetWindowExt(1, 1);
      pc->m_dcPrint.SetViewportExt(1 << p.x, 1 << p.y);

   /* Tried to set... */
#if 0
      od("Tts Mapping mode: %d; Viewport extent: %d, %d\r\n",
                  pc->m_dcPrint.GetMapMode(),
               pc->m_dcPrint.GetViewportExt());
#endif
   }

   CPmwApp *pApp = GET_PMWAPP();

/*
// Compute how much we need to adjust our coordinate system by.
*/

   CPaperInfo* pPaperInfo = GetDocument()->PrintPaperInfo();
   CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(GetDocument()->get_orientation());
   
   CFixedRect PhysicalMargins = pPaperInfo->PhysicalMargins();
   CFixedRect Margins = pPaperInfo->Margins();
   CFixedPoint OriginOffset = pPaperInfo->OriginOffset();

   if (GetDocument()->get_orientation() == LANDSCAPE)
   {
      pPrinterInfo->OrientRect(&PhysicalMargins);
      pPrinterInfo->OrientRect(&Margins);

      CFixed t = OriginOffset.x;
      OriginOffset.x = OriginOffset.y;
      OriginOffset.y = t;
   }

   CFixedPoint Adjust;

   Adjust.x = OriginOffset.x;
   Adjust.y = OriginOffset.y;
   
   if ((GetDocument()->get_project_type() != PROJECT_TYPE_Envelope) &&
         (GetDocument()->get_project_type() != PROJECT_TYPE_Label) &&
         (GetDocument()->get_project_type() != PROJECT_TYPE_BusinessCard))
   {
      Adjust.x += Margins.Left - PhysicalMargins.Left;
      Adjust.y += Margins.Top - PhysicalMargins.Top;
   }

   POINT PrinterAdjust;

   PrinterAdjust.x = (int)MulFixed(Adjust.x, real_x_resolution);
   PrinterAdjust.y = (int)MulFixed(Adjust.y, real_y_resolution);

   PPNT PageAdjust;

// if (Adjust.x < 0)
// {
//    PageAdjust.x = 0;
// }
// else
   {
      PageAdjust.x = MulFixed(Adjust.x, PAGE_RESOLUTION);
      PageAdjust.x = scale_pcoord(PageAdjust.x,
                                  real_x_resolution,
                                  printer_rc.x_resolution);
   }

// if (Adjust.y < 0)
// {
//    PageAdjust.y = 0;
// }
// else
   {
      PageAdjust.y = MulFixed(Adjust.y, PAGE_RESOLUTION);
      PageAdjust.y = scale_pcoord(PageAdjust.y,
                                  real_y_resolution,
                                  printer_rc.y_resolution);
   }

/* Generate the source box. */

   PBOX print_source_rect;

// od("PrinterAdjust: %d, %d\r\n", PrinterAdjust.x, PrinterAdjust.y);
// od("r: %d, %d, %d, %d; ", r);

   print_source_rect.x0 = (PCOORD)r.left;
   print_source_rect.y0 = (PCOORD)r.top;
   print_source_rect.x1 = (PCOORD)r.right;
   print_source_rect.y1 = (PCOORD)r.bottom;

   printer_rc.scaled_source_x0 = (print_source_rect.x0 - PrinterAdjust.x)*PAGE_RESOLUTION
                        + pc->m_source.x0*printer_rc.x_resolution;
   printer_rc.scaled_source_y0 = (print_source_rect.y0 - PrinterAdjust.y)*PAGE_RESOLUTION
                        + pc->m_source.y0*printer_rc.y_resolution;

// od("p0: %ld, %ld\r\n", printer_rc.scaled_source_x0, printer_rc.scaled_source_y0);

   ScalePBOXWithRounding(&print_source_rect,
								 PAGE_RESOLUTION, printer_rc.x_resolution,
								 PAGE_RESOLUTION, printer_rc.y_resolution);

/* Compute the source pbox. */

   printer_rc.source_pbox.x0 = print_source_rect.x0 + pc->m_source.x0 - PageAdjust.x;
   printer_rc.source_pbox.y0 = print_source_rect.y0 + pc->m_source.y0 - PageAdjust.y;
   printer_rc.source_pbox.x1 = print_source_rect.x1 + pc->m_source.x0 - PageAdjust.x;
   printer_rc.source_pbox.y1 = print_source_rect.y1 + pc->m_source.y0 - PageAdjust.y;

/* Clip it to the document bounds. */

#if 0
   PBOX bound;
   PPNT dims = GetDocument()->get_dimensions();
   bound.x0 =
      bound.y0 = 0;
   bound.x1 = dims.x;
   bound.y1 = dims.y;

   IntersectBox(&printer_rc.source_pbox, &printer_rc.source_pbox, &bound);
#endif

   ScalePBOXWithRounding(&print_source_rect,
                        printer_rc.x_resolution,
                        PAGE_RESOLUTION,
                        printer_rc.y_resolution,
                        PAGE_RESOLUTION);

   printer_rc.destination_rect.left = (SHORT)print_source_rect.x0;
   printer_rc.destination_rect.top = (SHORT)print_source_rect.y0;
   printer_rc.destination_rect.right = (SHORT)print_source_rect.x1;
   printer_rc.destination_rect.bottom = (SHORT)print_source_rect.y1;

// printer_rc.destination_x0 = 0;
// printer_rc.destination_y0 = 0;
   printer_rc.destination_x0 = (SHORT)printer_rc.destination_rect.left;
   printer_rc.destination_y0 = (SHORT)printer_rc.destination_rect.top;

   printer_rc.clip_rect = printer_rc.destination_rect;

   od("pdr: %d, %d; source: %ld, %ld, %ld, %ld; clip %d, %d, %d, %d\r\n",
               printer_rc.destination_rect.top,
               printer_rc.destination_rect.bottom,
               printer_rc.source_pbox,
               printer_rc.clip_rect);
}

int CPmwView::OnPrint(PrintContext *pc, CPrintInfo* pInfo)
{
#ifndef _WIN32
   extern BOOL NEAR UserAbort;

   if (pc->m_fBanding)
   {
      RECT r;
      HDC hdc = printer_rc.destination_hdc;
      BOOL supportsBandInfo = SupportsEscape(&pc->m_dcPrint, BANDINFO);

      // Do this first to set up the banding variables.

      pc->SaveStartOfBand();

      for (SetRectEmpty(&r);;)
      {
         int err;

         if ((err = ::Escape(printer_rc.destination_hdc,
                              NEXTBAND, NULL, NULL, &r)) < 0)
         {
            return err;
         }

//       od("After NEXTBAND: %d, (%d, %d)\r\n",
//          pc->m_dcPrint.GetMapMode(),
//          pc->m_dcPrint.GetViewportExt());

         if (IsRectEmpty(&r))
         {
            break;
         }

         if (supportsBandInfo)
         {
            BANDINFOSTRUCT inbs, outbs;

            inbs.fGraphics = TRUE;
            inbs.fText = FALSE;
            inbs.rcGraphics = r;

            outbs.fGraphics = TRUE;

            if ((err = ::Escape(printer_rc.destination_hdc,
                     BANDINFO, sizeof(inbs), (LPSTR)&inbs, &outbs)) < 0)
            {
               return err;
            }

//          od("Got band %d, %d, %d, %d (%d, %d)\r\n", r,
//                   outbs.fGraphics, outbs.fText);

            if (outbs.fGraphics == 0)
            {
            /* No graphics in this band! Skip it. */
               continue;
            }
         }

#if 0
         POINT p = pc->m_dcPrint.GetViewportOrg();
         od("VO: %d, %d; ", p);
         SIZE size = pc->m_dcPrint.GetViewportExt();
         od("VE: %d, %d; ", size);
         p = pc->m_dcPrint.GetWindowOrg();
         od("Window org: %d, %d; ", p);
         size = pc->m_dcPrint.GetWindowExt();
         od("Window Ext: %d, %d\r\n", size);
#endif

      /* Go to the start of the band. */

         pc->ToStartOfBand();

      /* Generate the band-dependent variables. */

         generate_print_vars(pc, r);

      /* Draw the band. */

         OnDraw(pc);

         if (UserAbort)
         {
         /* The user has said to abort. */
            return SP_USERABORT;
         }
      }
   }
   else
#endif
   {
   /* Draw the full source pbox. */
      generate_print_vars(pc, pInfo->m_rectDraw);
      OnDraw(pc);
   }
   return 0;
}

void CPmwView::generate_preview_vars(CDC *pDC, CPrintInfo* pInfo)
{
   RECT r = pInfo->m_rectDraw;

   int nPrinterResolutionX = pDC->GetDeviceCaps(LOGPIXELSX);
   int nPrinterResolutionY = pDC->GetDeviceCaps(LOGPIXELSY);

   ASSERT(pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)));

   /* Get a DC that doesn't have the printer DC attached. */
   CDC* pScreenDC = CDC::FromHandle(pDC->m_hDC);

   POINT VOrg, WOrg;
   SIZE VExt, WExt;

   VOrg = pScreenDC->GetViewportOrg();
   VExt = pScreenDC->GetViewportExt();
   WOrg = pScreenDC->GetWindowOrg();
   WExt = pScreenDC->GetWindowExt();

   ASSERT(WExt.cx == WExt.cy);
   ASSERT(VExt.cx == VExt.cy);

   /* Get the real (unscaled) resolution. */
   int real_x_resolution = pScreenDC->GetDeviceCaps(LOGPIXELSX);
   int real_y_resolution = pScreenDC->GetDeviceCaps(LOGPIXELSY);

   printer_rc.SetScaling((USHORT)scale_number(VExt.cx, nPrinterResolutionX, real_x_resolution), (USHORT)WExt.cx);
// printer_rc.SetScaling();

   TRACE("r: %d, %d, %d, %d -> ", r);
   pScreenDC->LPtoDP((LPPOINT)&r, 2);
   pScreenDC->SetViewportExt(1, 1);
//   pScreenDC->SetViewportOrg(0, 0);
   pScreenDC->SetWindowExt(1, 1);
   pScreenDC->DPtoLP((LPPOINT)&r, 2);
   TRACE("r: %d, %d, %d, %d\n", r);

// ((CPreviewDC*)pDC)->ClipToPage();

#if 0
   TRACE("VO: %d, %d; ", VOrg);
   TRACE("VE: %d, %d; ", VExt);
   TRACE("WO: %d, %d; ", WOrg);
   TRACE("WE: %d, %d\n", WExt);
#endif

   CPmwApp *pApp = GET_PMWAPP();

/*
// Compute how much we need to adjust our coordinate system by.
*/

   CPaperInfo* pPaperInfo = GetDocument()->PrintPaperInfo();
   CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(GetDocument()->get_orientation());
   
   CFixedRect PhysicalMargins = pPaperInfo->PhysicalMargins();
   CFixedRect Margins = pPaperInfo->Margins();
   CFixedPoint OriginOffset = pPaperInfo->OriginOffset();

   if (GetDocument()->get_orientation() == LANDSCAPE)
   {
      pPrinterInfo->OrientRect(&PhysicalMargins);
      pPrinterInfo->OrientRect(&Margins);

      CFixed t = OriginOffset.x;
      OriginOffset.x = OriginOffset.y;
      OriginOffset.y = t;
   }

   CFixedPoint Adjust;

   Adjust.x = OriginOffset.x;
   Adjust.y = OriginOffset.y;
   
   if ((GetDocument()->get_project_type() != PROJECT_TYPE_Envelope) &&
         (GetDocument()->get_project_type() != PROJECT_TYPE_Label) &&
         (GetDocument()->get_project_type() != PROJECT_TYPE_BusinessCard))
   {
      Adjust.x += Margins.Left - PhysicalMargins.Left;
      Adjust.y += Margins.Top - PhysicalMargins.Top;
   }

   POINT PrinterAdjust;

   PrinterAdjust.x = (int)MulFixed(Adjust.x, real_x_resolution);
   PrinterAdjust.y = (int)MulFixed(Adjust.y, real_y_resolution);

   PPNT PageAdjust;

// if (Adjust.x < 0)
// {
//    PageAdjust.x = 0;
// }
// else
   {
      PageAdjust.x = MulFixed(Adjust.x, PAGE_RESOLUTION);
      PageAdjust.x = scale_pcoord(PageAdjust.x,
                                  real_x_resolution,
                                  printer_rc.x_resolution);
   }

// if (Adjust.y < 0)
// {
//    PageAdjust.y = 0;
// }
// else
   {
      PageAdjust.y = MulFixed(Adjust.y, PAGE_RESOLUTION);
      PageAdjust.y = scale_pcoord(PageAdjust.y,
                                  real_y_resolution,
                                  printer_rc.y_resolution);
   }

/* Generate the source box. */

   PBOX print_source_rect;

// od("PrinterAdjust: %d, %d\r\n", PrinterAdjust.x, PrinterAdjust.y);
// od("r: %d, %d, %d, %d; ", r);

   print_source_rect.x0 = (PCOORD)r.left;
   print_source_rect.y0 = (PCOORD)r.top;
   print_source_rect.x1 = (PCOORD)r.right;
   print_source_rect.y1 = (PCOORD)r.bottom;

   printer_rc.scaled_source_x0 = (print_source_rect.x0 - PrinterAdjust.x)*PAGE_RESOLUTION
                        + m_pc.m_source.x0*printer_rc.x_resolution;
   printer_rc.scaled_source_y0 = (print_source_rect.y0 - PrinterAdjust.y)*PAGE_RESOLUTION
                        + m_pc.m_source.y0*printer_rc.y_resolution;

// od("p0: %ld, %ld\r\n", printer_rc.scaled_source_x0, printer_rc.scaled_source_y0);

   ScalePBOXWithRounding(&print_source_rect,
               PAGE_RESOLUTION, printer_rc.x_resolution,
               PAGE_RESOLUTION, printer_rc.y_resolution);

   ScalePBOXWithRounding(&print_source_rect,
               printer_rc.GetScaleDenominator(), printer_rc.GetScaleNumerator(),
               printer_rc.GetScaleDenominator(), printer_rc.GetScaleNumerator());

/* Compute the source pbox. */

   printer_rc.source_pbox.x0 = print_source_rect.x0 + m_pc.m_source.x0 - PageAdjust.x;
   printer_rc.source_pbox.y0 = print_source_rect.y0 + m_pc.m_source.y0 - PageAdjust.y;
   printer_rc.source_pbox.x1 = print_source_rect.x1 + m_pc.m_source.x0 - PageAdjust.x;
   printer_rc.source_pbox.y1 = print_source_rect.y1 + m_pc.m_source.y0 - PageAdjust.y;

/* Clip it to the document bounds. */

   ScalePBOXWithRounding(&print_source_rect,
                        printer_rc.x_resolution,
                        PAGE_RESOLUTION,
                        printer_rc.y_resolution,
                        PAGE_RESOLUTION);

   ScalePBOXWithRounding(&print_source_rect,
                   printer_rc.GetScaleNumerator(), printer_rc.GetScaleDenominator(),
                   printer_rc.GetScaleNumerator(), printer_rc.GetScaleDenominator());

   printer_rc.destination_rect.left = (SHORT)print_source_rect.x0 + VOrg.x;
   printer_rc.destination_rect.top = (SHORT)print_source_rect.y0 + VOrg.y;
   printer_rc.destination_rect.right = (SHORT)print_source_rect.x1 + VOrg.x;
   printer_rc.destination_rect.bottom = (SHORT)print_source_rect.y1 + VOrg.y;

// printer_rc.destination_x0 = 0;
// printer_rc.destination_y0 = 0;
   printer_rc.destination_x0 = (SHORT)printer_rc.destination_rect.left;
   printer_rc.destination_y0 = (SHORT)printer_rc.destination_rect.top;

   printer_rc.clip_rect = printer_rc.destination_rect;

   TRACE("pdr: %d, %d; source: %ld, %ld, %ld, %ld; clip %d, %d, %d, %d\n",
               printer_rc.destination_rect.top,
               printer_rc.destination_rect.bottom,
               printer_rc.source_pbox,
               printer_rc.clip_rect);
}

/*
// I think this routine is only called for Print Preview.
*/

void CPmwView::OnPrint(CDC *pDC, CPrintInfo* pInfo)
{
   ASSERT(pInfo->m_bPreview);

   // Draw the full source pbox.
   generate_preview_vars(pDC, pInfo);

   REFRESH_EXTENT re;

   re.extent = printer_rc.source_pbox;
   re.refresh_type = REFRESH_ALL;
   re.my_object = NULL;
   re.update_object = NULL;
   re.update_state = NULL;

   re.m_nPreviewPage = pInfo->m_nCurPage;
   re.m_nPreviewSlot = 0;

   // Add this refresh extent to the preview view.
   GetDocument()->UpdateAllViews(NULL, HINT_RefreshPreview, (CObject*)&re);
}

/////////////////////////////////////////////////////////////////////////////
// CPmwView printing
BOOL CPmwView::IsGoodPrinter ()
{
	CPrinterInfo* pPrinterInfo = GET_PMWAPP()->GetPrinterInfo(GetDocument()->get_orientation());
   int   rc = pPrinterInfo->GetPrinterCaps ();
   
   if ((rc & (RC_BITBLT)) != RC_BITBLT)
   {
      CBadPrinterDialog Dialog;
      if (Dialog.DoModal () != IDOK)
      return FALSE;
   }

   return TRUE;
}

BOOL CPmwView::DoPreparePrinting (CPrintInfo* pInfo )
{
   if (CView::DoPreparePrinting (pInfo) == FALSE)
      return FALSE;
   
   return IsGoodPrinter ();
}

BOOL CPmwView::OnPreparePrinting(CPrintInfo* pInfo)
{
#if 1
   ASSERT(pInfo != NULL);
   ASSERT(pInfo->m_pPD != NULL);

   CPmwDoc *pDoc = GetDocument();

   // default preparation

   CPmwApp *app = (CPmwApp *)AfxGetApp();

   HGLOBAL hDevMode = app->get_devmode();

   if (hDevMode != NULL)
   {
      LPDEVMODE dm;

   /* Lock the DevMode structure! */

      if ((dm = (LPDEVMODE)GlobalLock(hDevMode)) != NULL)
      {
      /* Well, we have it now... */

         SHORT desired_orientation = pDoc->GetPrintOrientation() == PORTRAIT
                     ? DMORIENT_PORTRAIT
                     : DMORIENT_LANDSCAPE;

         if (!(dm->dmFields & DM_ORIENTATION)
                  || dm->dmOrientation != desired_orientation)
         {
         /*
         // Orientation is not set or not what we want.
         // Force it to be what we want.
         */
            dm->dmFields |= DM_ORIENTATION;
            dm->dmOrientation = desired_orientation;
         }

#if 0
         od("Orient: %d, size: %d, len:%d, wid:%d, qual:%d\r\n",
            dm->dmOrientation,
            dm->dmPaperSize,
            dm->dmPaperLength,
            dm->dmPaperWidth,
            dm->dmPrintQuality);
#endif

         GlobalUnlock(hDevMode);
      }
   }

   if (pInfo->m_bPreview)
   {
      // Set the preview parameters.
      pInfo->SetMinPage(1);
      pInfo->SetMaxPage(PrintPreviewPageCount());
   }
   else
   {
      // Replace the print dialog.

      CPmwPrint *new_dialog;

      if ((new_dialog = CreatePrintDialog()) == NULL)
      {
         return FALSE;
      }

      delete pInfo->m_pPD;
      pInfo->m_pPD = new_dialog;
   }
#endif
   if (m_fNoPrintDialog)
   {
      pInfo->m_pPD->m_pd.hDevNames = GET_PMWAPP()->get_devnames(TRUE);
      pInfo->m_pPD->m_pd.hDevMode = GET_PMWAPP()->get_devmode(FALSE);
      return pInfo->m_pPD->CreatePrinterDC() != NULL;
   }
   // Do the standard "prepare printing".
   if (!DoPreparePrinting(pInfo))
   {
      return FALSE;
   }

   // Handle the number of preview pages now.
   if (pInfo->m_bPreview)
   {
      // Always!!
//    if (PrintPreviewPageCount() == 1)
      {
         // Force to 1 page.
         pInfo->m_nNumPreviewPages = 1;
      }
   }
   return TRUE;
}

CPmwPrint* CPmwView::CreatePrintDialog(void)
{
   return new CPmwPrint(this);
}

void CPmwView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
   // Construct the printer redisplay context.
   if (pInfo != NULL && pInfo->m_bPreview)
   {
      SHORT x_resolution = ::GetDeviceCaps(pDC->m_hDC, LOGPIXELSX);
      SHORT y_resolution = ::GetDeviceCaps(pDC->m_hDC, LOGPIXELSY);

   // od("X res: %d, Y res: %d\r\n", x_resolution, y_resolution);
      printer_rc.x_resolution = x_resolution;
      printer_rc.y_resolution = y_resolution;

//    printer_rc.SetScaling();

   // od("Print resolution:%d, %d\r\n", printer_rc.x_resolution, printer_rc.y_resolution);

      printer_rc.destination_hdc = pDC->m_hDC;
//    printer_rc.AttributeDC(pDC->m_hAttribDC);

      // Set the info.

      printer_rc.set_info(printer_rc.AttributeDC(), TRUE);

      // Setup to view the whole page.
      PPNT Dims = GetDocument()->get_dimensions();

      m_pc.m_source.x0 = 0;
      m_pc.m_source.y0 = 0;
      m_pc.m_source.x1 = Dims.x;
      m_pc.m_source.y1 = Dims.y;
   }
}

void CPmwView::OnBeginPrinting(CDC *pDC, CPrintInfo* pInfo)
{
   // Construct the printer redisplay context.
   if (pInfo->m_bPreview)
   {
      printer_rc.hwnd = GetSafeHwnd();
      printer_rc.is_printing = FALSE;
      printer_rc.is_print_preview = TRUE;

   // od("Print resolution:%d, %d\r\n", printer_rc.x_resolution, printer_rc.y_resolution);

      printer_rc.outline_gamma_curve = screen_gamma_curve;
      printer_rc.bitmap_gamma_curve = screen_gamma_curve;
      // Don't call build_gamma_curve. It's already built.

   /* Set the interrupt check handler. */

      printer_rc.set_check_interrupt(standard_check_interrupt, (VOIDPTR)&printer_rc);
      printer_rc.terminate_interrupts = FALSE;

   /* Set the name update routine. */

      printer_rc.register_update_name(NULL, NULL);
   }
}

void CPmwView::OnBeginPrinting(PrintContext *pc, CPrintInfo* pInfo)
{
/*
// Construct the printer redisplay context.
*/
   if (pInfo->m_bPreview)
   {
      printer_rc.hwnd = GetSafeHwnd();
//      printer_rc.is_printing = FALSE;
      printer_rc.is_print_preview = TRUE;
   }
   else
   {
      printer_rc.hwnd = 0;
      printer_rc.is_printing = TRUE;         /* Well, we are... */
      printer_rc.is_print_preview = FALSE;

   /* Establish printing from the printer dialog. */

      printer_rc.smoothing = pc->m_pPD->m_smoothing
            ? TRISTATE_ALL
            : TRISTATE_NONE;

      printer_rc.coloring_book = (TRISTATE)pc->m_pPD->m_coloring_book;

      CPmwPrint *pPD = (CPmwPrint *)pInfo->m_pPD;

   /* Make sure our orientation is set correctly. */

      HGLOBAL hDevMode = pPD->m_pd.hDevMode;

      if (hDevMode != NULL)
      {
         LPDEVMODE dm;

      /* Lock the DevMode structure! */

         if ((dm = (LPDEVMODE)GlobalLock(hDevMode)) != NULL)
         {
         /* Well, we have it now... */

            CPmwDoc *pDoc = GetDocument();

            SHORT desired_orientation = pDoc->GetPrintOrientation() == PORTRAIT
                        ? DMORIENT_PORTRAIT
                        : DMORIENT_LANDSCAPE;

            if (!(dm->dmFields & DM_ORIENTATION)
                     || dm->dmOrientation != desired_orientation)
            {
            /*
            // Orientation is not set or not what we want.
            // Force it to be what we want.
            */
               dm->dmFields |= DM_ORIENTATION;
               dm->dmOrientation = desired_orientation;
               pc->m_dcPrint.ResetDC(dm);
            }

#if 0
            od("Orient: %d, size: %d, len:%d, wid:%d, qual:%d\r\n",
               dm->dmOrientation,
               dm->dmPaperSize,
               dm->dmPaperLength,
               dm->dmPaperWidth,
               dm->dmPrintQuality);
#endif

            GlobalUnlock(hDevMode);
         }
      }
   }

   SHORT x_resolution = pc->m_dcPrint.GetDeviceCaps(LOGPIXELSX);
   SHORT y_resolution = pc->m_dcPrint.GetDeviceCaps(LOGPIXELSY);

// od("X res: %d, Y res: %d\r\n", x_resolution, y_resolution);
   printer_rc.x_resolution = (SHORT)((double)x_resolution*pc->m_print_scale);
   printer_rc.y_resolution = (SHORT)((double)y_resolution*pc->m_print_scale);

   printer_rc.SetScaling();

/* Handle the scaling factor. */

   POINT p = pc->m_scaling;

   if (p.x != 0 || p.y != 0)
   {
      printer_rc.x_resolution >>= p.x;
      printer_rc.y_resolution >>= p.y;

      pc->m_dcPrint.SetMapMode(MM_ANISOTROPIC);
      pc->m_dcPrint.SetWindowExt(1, 1);
      pc->m_dcPrint.SetViewportExt(1 << p.x, 1 << p.y);

#if 0
      od("Mapping mode: %d; Viewport extent: %d, %d\r\n",
                  pc->m_dcPrint.GetMapMode(),
                  pc->m_dcPrint.GetViewportExt());
#endif
   }

// od("Print resolution:%d, %d\r\n", printer_rc.x_resolution, printer_rc.y_resolution);

   printer_rc.destination_hdc = pc->m_dcPrint.GetSafeHdc();
   printer_rc.outline_gamma_curve = printer_outline_gamma_curve;
   printer_rc.bitmap_gamma_curve = printer_bitmap_gamma_curve;

   printer_rc.build_gamma_curve();

#if 0
   for (SHORT i = 0; i < 256; i++)
   {
      od("%4d", printer_rc.gamma_curve[i]);
      if ((i & 15) == 15)
      {
         od("\r\n");
      }
   }
#endif

/* Set the info. */

   printer_rc.set_info(printer_rc.destination_hdc, TRUE);

/* Set the interrupt check handler. */

   printer_rc.set_check_interrupt(printer_check_interrupt, (VOIDPTR)&printer_rc);

/* There will be no continuing. */

   printer_rc.terminate_interrupts = TRUE;

/* Set the name update routine. */

   printer_rc.register_update_name(printer_update_name, (VOIDPTR)pc);
}

void CPmwView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   printer_rc.register_update_name(NULL);
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CPmwView::IsSelected(const CObject* pDocItem) const
{
   // TODO: implement this function that tests for a selected OLE client item

   PMGPageObjectPtr pObject = GetDocument()->selected_objects();

   while (pObject != NULL)
   {
      switch (pObject->type())
      {
         case OBJECT_TYPE_OleObject:
         {
            COleClientItem* pItem = ((OleObject*)pObject)->Item();
            if (pDocItem == pItem)
            {
               TRACE1("Object %08lx is selected.\r\n", pItem);
               return TRUE;
            }
            break;
         }
         default:
         {
            break;
         }
      }
      // Go to the next object.
      pObject = pObject->next_selected_object();
   }

// No match.
   return FALSE;
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.
void CPmwView::OnCancelEdit()
{
   // Close any in-place active item on this view.
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL)
   {
      pActiveItem->Close();
   }
   ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CPmwView diagnostics

#ifdef _DEBUG
void CPmwView::AssertValid() const
{
   CView::AssertValid();
}

void CPmwView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}

CPmwDoc* CPmwView::GetDocument() const // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPmwDoc)));
   return (CPmwDoc*) m_pDocument;
}

#endif //_DEBUG

void CPmwView::SetInitialTextStyle (void)
{
   CTextStyle& Style = m_Txp.Style();
   Style.Database(GetDocument()->get_database());
   Style.SetDefault();
}

/////////////////////////////////////////////////////////////////////////////
// CPmwView message handlers

void CPmwView::OnInitialUpdate()
{  
   // Initialize the global Txp.
   // (Comment this out during 16-bit builds until TrueType engine works.)
#if 1
   SetInitialTextStyle ();
#endif

   // Create the rulers.
   CreateRulers();

   // Create the desktop.
   CreateEditDesktop();

   // Create the drag/drop necessities.
#ifdef WINLOGO
   CreateDropTarget();
   DragAcceptFiles();
#endif

   m_pLinkFrameButton = new CButtonWnd("Link Frame", this, ID_TEXT_LINK, IDB_LINKFRAME);
   m_pPreviousFrameButton = new CButtonWnd("Previous Frame", this, ID_TEXT_PREVIOUS_FRAME, IDB_PREVIOUSFRAME);
   m_pNextFrameButton = new CButtonWnd("Next Frame", this, ID_TEXT_NEXT_FRAME, IDB_NEXTFRAME);

/*
// Compute the raster type for the rc.
*/

   rc.hwnd = GetSafeHwnd();
   HDC hDC = ::GetDC(rc.hwnd);
   rc.set_info(hDC);
   rc.register_toggle(CPmwView::refresh_toggle, (VOIDPTR)this);
// m_pDocumentBound = &view_portion;
   rc.m_pDesktop = m_pDesktop;

   ::ReleaseDC(rc.hwnd, hDC);

   set_arrow_tool();

   rc.source_pbox.x0 =
      rc.source_pbox.y0 =
      rc.source_pbox.x1 =
      rc.source_pbox.y1 = 0;

   rc.smoothing = TRISTATE_NONE;       // No smoothing to screen?
   rc.using_images = TRUE;             // Use object images

   OnViewFullPage();

	// show project type hint
	PROJECT_TYPE projtype = GetDocument()->get_project_type();

	if ((GetDocument()->m_wDocumentFlags & CPmwDoc::FLAG_BringUpReceiverList) == 0)
	{
		GET_PMWAPP()->ShowHelpfulHint(PROJECT_HINT, projtype);
	}
	else
	{
		m_fBringUpReceiverList = TRUE;
	}
	// file_type not set for open ready made; 
	// CPmwDoc::FLAG_LoadedFromCollection not set for open with help
   if ((GetDocument()->get_file_type() == TEMPLATE_TYPE)
		|| ((GetDocument()->TestDocumentFlag(CPmwDoc::FLAG_LoadedFromCollection)) == TRUE))
	{
		GetDocument()->GoToFirstPage();
	}
}

void CPmwView::OnActivateView(BOOL bActivate, CView *pActivateView, CView *pDeactivateView)
{
	static CView* pLastActiveView = NULL;
   // Call the default
   CView::OnActivateView(bActivate, pActivateView, pDeactivateView);
	if(bActivate )
	{
		if(pLastActiveView != pActivateView && GET_PMWAPP()->AutoInstructionsEnabled())
			ViewInstructionsInHelpWnd(FALSE);
		
		pLastActiveView = pActivateView;
	}
	//There is not an active view. Make sure help is showing in the window
	if(pActivateView == NULL)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->HelpWindow().DisplayTopic(NULL);		
	}
}

#define NUM_COLORS 125

#if 0
void initialize_palette(void)
{
   int i, r, g, b;
   static struct
   {
      WORD palVersion;
      WORD palNumEntries;
      PALETTEENTRY palPalEntry[NUM_COLORS];
   } near logpal;
   static int near map5[] = { 0, 64, 128, 192, 255 };

   if (pOurPal != NULL)
   {
      return;
   }

   if ((pOurPal = new CPalette) == NULL)
   {
      return;
   }

   logpal.palVersion = 0x300;
   logpal.palNumEntries = NUM_COLORS;

// BYTE flags = PC_EXPLICIT;

   for (i = 0, r = 0; r < 5; r++)
   {
      for (g = 0; g < 5; g++)
      {
         for (b = 0; b < 5; b++)
         {
            logpal.palPalEntry[i].peRed = map5[r];
            logpal.palPalEntry[i].peGreen = map5[g];
            logpal.palPalEntry[i].peBlue = map5[b];
            logpal.palPalEntry[i++].peFlags = 0;
//          logpal.palPalEntry[i++].peFlags = flags;
//          flags = PC_RESERVED;
         }
      }
   }

   pOurPal->CreatePalette((LOGPALETTE far *)&logpal);
}
#else
const RGBQUAD* GetIdentityPalette(void)
{
   // The identity pallete

   static const RGBQUAD BASED_CODE IdentityPalette[256] =
   {
      //  B    G    R
      {   0,   0,   0 },
      {   0,   0, 128 },
      {   0, 128,   0 },
      {   0, 128, 128 },
      { 128,   0,   0 },
      { 128,   0, 128 },
      { 128, 128,   0 },
      { 192, 192, 192 },
      { 192, 220, 192 },
      { 240, 202, 166 },
#if 0
      {   4,   4,   4 },
#else
      {   0,   0,   0 },			// Our transparent color. Masked by entry 0.
#endif
      {   8,   8,   8 },
      {  12,  12,  12 },
      {  17,  17,  17 },
      {  22,  22,  22 },
      {  28,  28,  28 },
      {  34,  34,  34 },
      {  41,  41,  41 },
      {  85,  85,  85 },
      {  77,  77,  77 },
      {  66,  66,  66 },
      {  57,  57,  57 },
      { 129, 129, 129 },
      {   0,   0, 129 },
      {   0, 129,   0 },
      {   0, 129, 129 },
      { 129,   0,   0 },
      { 129,   0, 129 },
      { 129, 129,   0 },
      {   0,   0,  51 },
      {   0,   0, 102 },
      {   0,   0, 153 },
      {   0,   0, 204 },
      {   0,  51,   0 },
      {   0,  51,  51 },
      {   0,  51, 102 },
      {   0,  51, 153 },
      {   0,  51, 204 },
      {   0,  51, 255 },
      {   0, 102,   0 },
      {   0, 102,  51 },
      {   0, 102, 102 },
      {   0, 102, 153 },
      {   0, 102, 204 },
      {   0, 102, 255 },
      {   0, 153,   0 },
      {   0, 153,  51 },
      {   0, 153, 102 },
      {   0, 153, 153 },
      {   0, 153, 204 },
      {   0, 153, 255 },
      {   0, 204,   0 },
      {   0, 204,  51 },
      {   0, 204, 102 },
      {   0, 204, 153 },
      {   0, 204, 204 },
      {   0, 204, 255 },
      {   0, 255, 102 },
      {   0, 255, 153 },
      {   0, 255, 204 },
      {  51,   0,   0 },
      {  51,   0,  51 },
      {  51,   0, 102 },
      {  51,   0, 153 },
      {  51,   0, 204 },
      {  51,   0, 255 },
      {  51,  51,   0 },
      {  51,  51,  51 },
      {  51,  51, 102 },
      {  51,  51, 153 },
      {  51,  51, 204 },
      {  51,  51, 255 },
      {  51, 102,   0 },
      {  51, 102,  51 },
      {  51, 102, 102 },
      {  51, 102, 153 },
      {  51, 102, 204 },
      {  51, 102, 255 },
      {  51, 153,   0 },
      {  51, 153,  51 },
      {  51, 153, 102 },
      {  51, 153, 153 },
      {  51, 153, 204 },
      {  51, 153, 255 },
      {  51, 204,   0 },
      {  51, 204,  51 },
      {  51, 204, 102 },
      {  51, 204, 153 },
      {  51, 204, 204 },
      {  51, 204, 255 },
      {  51, 255,  51 },
      {  51, 255, 102 },
      {  51, 255, 153 },
      {  51, 255, 204 },
      {  51, 255, 255 },
      { 102,   0,   0 },
      { 102,   0,  51 },
      { 102,   0, 102 },
      { 102,   0, 153 },
      { 102,   0, 204 },
      { 102,   0, 255 },
      { 102,  51,   0 },
      { 102,  51,  51 },
      { 102,  51, 102 },
      { 102,  51, 153 },
      { 102,  51, 204 },
      { 102,  51, 255 },
      { 102, 102,   0 },
      { 102, 102,  51 },
      { 102, 102, 102 },
      { 102, 102, 153 },
      { 102, 102, 204 },
      { 102, 153,   0 },
      { 102, 153,  51 },
      { 102, 153, 102 },
      { 102, 153, 153 },
      { 102, 153, 204 },
      { 102, 153, 255 },
      { 102, 204,   0 },
      { 102, 204,  51 },
      { 102, 204, 153 },
      { 102, 204, 204 },
      { 102, 204, 255 },
      { 102, 255,   0 },
      { 102, 255,  51 },
      { 102, 255, 153 },
      { 102, 255, 204 },
      { 204,   0, 255 },
      { 255,   0, 204 },
      { 153, 153,   0 },
      { 153,  51, 153 },
      { 153,   0, 153 },
      { 153,   0, 204 },
      { 153,   0,   0 },
      { 153,  51,  51 },
      { 153,   0, 102 },
      { 153,  51, 204 },
      { 153,   0, 255 },
      { 153, 102,   0 },
      { 153, 102,  51 },
      { 153,  51, 102 },
      { 153, 102, 153 },
      { 153, 102, 204 },
      { 153,  51, 255 },
      { 153, 153,  51 },
      { 153, 153, 102 },
      { 153, 153, 153 },
      { 153, 153, 204 },
      { 153, 153, 255 },
      { 153, 204,   0 },
      { 153, 204,  51 },
      { 102, 204, 102 },
      { 153, 204, 153 },
      { 153, 204, 204 },
      { 153, 204, 255 },
      { 153, 255,   0 },
      { 153, 255,  51 },
      { 153, 204, 102 },
      { 153, 255, 153 },
      { 153, 255, 204 },
      { 153, 255, 255 },
      { 204,   0,   0 },
      { 153,   0,  51 },
      { 204,   0, 102 },
      { 204,   0, 153 },
      { 204,   0, 204 },
      { 153,  51,   0 },
      { 204,  51,  51 },
      { 204,  51, 102 },
      { 204,  51, 153 },
      { 204,  51, 204 },
      { 204,  51, 255 },
      { 204, 102,   0 },
      { 204, 102,  51 },
      { 153, 102, 102 },
      { 204, 102, 153 },
      { 204, 102, 204 },
      { 153, 102, 255 },
      { 204, 153,   0 },
      { 204, 153,  51 },
      { 204, 153, 102 },
      { 204, 153, 153 },
      { 204, 153, 204 },
      { 204, 153, 255 },
      { 204, 204,   0 },
      { 204, 204,  51 },
      { 204, 204, 102 },
      { 204, 204, 153 },
      { 204, 204, 204 },
      { 204, 204, 255 },
      { 204, 255,   0 },
      { 204, 255,  51 },
      { 153, 255, 102 },
      { 204, 255, 153 },
      { 204, 255, 204 },
      { 204, 255, 255 },
      { 204,   0,  51 },
      { 255,   0, 102 },
      { 255,   0, 153 },
      { 204,  51,   0 },
      { 255,  51,  51 },
      { 255,  51, 102 },
      { 255,  51, 153 },
      { 255,  51, 204 },
      { 255,  51, 255 },
      { 255, 102,   0 },
      { 255, 102,  51 },
      { 204, 102, 102 },
      { 255, 102, 153 },
      { 255, 102, 204 },
      { 204, 102, 255 },
      { 255, 153,   0 },
      { 255, 153,  51 },
      { 255, 153, 102 },
      { 255, 153, 153 },
      { 255, 153, 204 },
      { 255, 153, 255 },
      { 255, 204,   0 },
      { 255, 204,  51 },
      { 255, 204, 102 },
      { 255, 204, 153 },
      { 255, 204, 204 },
      { 255, 204, 255 },
      { 255, 255,  51 },
      { 204, 255, 102 },
      { 255, 255, 153 },
      { 255, 255, 204 },
      { 102, 102, 255 },
      { 102, 255, 102 },
      { 102, 255, 255 },
      { 255, 102, 102 },
      { 255, 102, 255 },
      { 255, 255, 102 },
      { 193, 193, 193 },
      {  95,  95,  95 },
      { 119, 119, 119 },
      { 134, 134, 134 },
      { 150, 150, 150 },
      { 203, 203, 203 },
      { 178, 178, 178 },
      { 215, 215, 215 },
      { 221, 221, 221 },
      { 227, 227, 227 },
      { 234, 234, 234 },
      { 241, 241, 241 },
      { 248, 248, 248 },
      { 240, 251, 255 },
      { 164, 160, 160 },
      { 128, 128, 128 },
      {   0,   0, 255 },
      {   0, 255,   0 },
      {   0, 255, 255 },
      { 255,   0,   0 },
      { 255,   0, 255 },
      { 255, 255,   0 },
      { 255, 255, 255 }
   };
   return IdentityPalette;
}

CPalette* CreateIdentityPalette(void)
{
   BOOL fSuccess = FALSE;
   
   HPALETTE hPalette = NULL;
   HPALETTE hOldPalette = NULL;
   CPalette* pPalette = NULL;
   HDC hDC = NULL;
   int i;
   const RGBQUAD* IdentityPalette = GetIdentityPalette();

   // A structure to hold the palette being built.
      
   struct
   {
      WORD wVersion;
      WORD wNumberOfEntries;
      PALETTEENTRY Entries[256];
   } Palette =
   {
      0x300,
      256
   };
   
   TRY
   {
      // Get the screen DC, it will be used to retrieve information
      // about the static colors.
         
      hDC = GetDC(NULL);
         
      if (hDC != NULL)
      {
         // First, clear the system palette to black.
            
         for (i = 0; i < 256; i++)
         {
            Palette.Entries[i].peRed = 0;
            Palette.Entries[i].peGreen = 0;
            Palette.Entries[i].peBlue = 0;
            Palette.Entries[i].peFlags = PC_NOCOLLAPSE;
         }
         
         // Create, select, realize, deselect, and delete the palette.
         
         if (hPalette = CreatePalette((LOGPALETTE *)&Palette))
         {
            hOldPalette = SelectPalette(hDC, hPalette, FALSE);
                  
            if (hOldPalette != NULL)
            {
               RealizePalette(hDC);
               SelectPalette(hDC, hOldPalette, FALSE);
            }
                  
            DeleteObject(hPalette);
            hPalette = NULL;
         }
         
         // Get the current system palette and fill in the non static
         // colors with the colors from our identity palette.
         // If the system palette does not exist, do not fill in any
         // system colors.

         int nStaticColors = 0;

         if (GetSystemPaletteEntries(hDC, 0, 256, Palette.Entries) != 0)
         {
            nStaticColors = GetDeviceCaps(hDC, NUMCOLORS)/2;

            if (nStaticColors > 20/2)
            {
            /*
            // We clip the static colors at twenty. This means we get our
            // 236 entries in the palette. Since we have knocked off some
            // static colors, the mapping is no longer an identity mapping.
            // NOTE: This *may* be PrintMaster-specific.
            */
               nStaticColors = 20/2;
            }
         }

         for (i = 0; i < nStaticColors; i++)
         {
            Palette.Entries[i].peFlags = 0;
         }

         for (; i < 256-nStaticColors; i++)
         {
            Palette.Entries[i].peRed = IdentityPalette[i].rgbRed;
            Palette.Entries[i].peGreen = IdentityPalette[i].rgbGreen;
            Palette.Entries[i].peBlue = IdentityPalette[i].rgbBlue;
            Palette.Entries[i].peFlags = PC_NOCOLLAPSE;
         }
            
         for (; i < 256; i++)
         {
            Palette.Entries[i].peFlags = 0;
         }
            
         // Create the palette.
            
         pPalette = new CPalette;
                  
         if (pPalette != NULL)
         {
            fSuccess = pPalette->CreatePalette((LOGPALETTE *)&Palette);
               
            if (!fSuccess)
            {
               delete pPalette;
               pPalette = NULL;
            }
         }
         
         ReleaseDC(NULL, hDC);
         hDC = NULL;
      }
   }
   CATCH_ALL(e)
   {
      fSuccess = FALSE;
      
      if (hOldPalette != NULL)
      {
         SelectPalette(hDC, hOldPalette, FALSE);
         DeleteObject(hOldPalette);
         hOldPalette = NULL;
      }
      
      if (hPalette != NULL)
      {
         DeleteObject(hPalette);
         hPalette = NULL;
      }
      
      if (hDC != NULL)
      {
         ReleaseDC(NULL, hDC);
      }
      
      delete pPalette;
      pPalette = NULL;
   }
   END_CATCH_ALL
   
   ASSERT(fSuccess);
   
   return pPalette;
}

void initialize_palette(void)
{
   pOurPal = CreateIdentityPalette();
}

#endif

void free_palette(void)
{
   if (pOurPal != NULL)
   {
      delete pOurPal;
   }
}

/*
// Generate the scaled view parameters.
*/

VOID CPmwView::view_scale(PCOORD numerator, PCOORD denominator, PPNT *dimensions, PBOX *pbox)
{                               
// od("In values: %ld, %ld -> ", numerator, denominator);

   while (numerator > 1024 || denominator > 1024)
   {
      numerator >>= 1;
      denominator >>= 1;
   }

   if (numerator == 0)
   {
      numerator++;
   }
   if (denominator == 0)
   {
      denominator++;
   }

   PCOORD big_d = 10*denominator;

   if (numerator > big_d)
   {
      numerator = big_d;
   }

// od("Scale factors: %ld / %ld\r\n", numerator, denominator);

/* Set the scale factors. */

   rc.SetScaling((USHORT)numerator, (USHORT)denominator);

/* Compute the bounds of the document at the new scale. */

   pbox->x0 = 0;
   pbox->y0 = 0;
   pbox->x1 = dimensions->x;
   pbox->y1 = dimensions->y;

   rc.pbox_to_screen(pbox, TRUE);

// od("Scaled doc dims: %ld, %ld, %ld, %ld\r\n", *pbox);
   pbox->x1 -= pbox->x0;
   pbox->y1 -= pbox->y0;

   pbox->x0 = pbox->y0 = 0;
}

VOID CPmwView::update_scrolls(RECT *client, PBOX *pbox, BOOL horz, BOOL vert)
{
   int cwidth = client->right - client->left;
   int cheight = client->bottom - client->top;
   BOOL need_horz, need_vert;

   need_horz = horz && pbox->x1 > (PCOORD)cwidth;
   need_vert = vert && pbox->y1 > (PCOORD)cheight;

/* Handle the scroll bars. */

   horizontal_scroll_shown = need_horz;
   SetScrollRange(SB_HORZ, 0, need_horz ? 1000 : 0, TRUE); 

   vertical_scroll_shown = need_vert;
   SetScrollRange(SB_VERT, 0, need_vert ? 1000 : 0, TRUE); 
}

/*
// Pick the nearest resolution for the passed resolution.
*/

static SHORT nearest_resolution(SHORT res)
{
   static const SHORT BASED_CODE good_res[] =
   {
       40,  45,  50,  60,  72,  75,  90, 100,
      120, 150, 180, 200, 225, 300, 360, 450,
      -1
   };

   const SHORT *sp;
   USHORT distance = 0xffff;
   SHORT new_res = res;

   for (sp = good_res; *sp != -1; sp++)
   {
      USHORT this_distance;
      this_distance = (USHORT)((res < *sp) ? (*sp - res) : (res - *sp));
      if (this_distance < distance)
      {
         distance = this_distance;
         new_res = *sp;
      }
   }
   return new_res;
}

VOID CPmwView::change_view(VIEW_TYPE new_view, BOOL force, PBOX_PTR area)
{
/* See if there's even anything to do. */

   if (!force && new_view == current_view)
   {
   /* Same view; skip. */
      return;
   }

   if (new_view == VIEW_LAST)
   {
      new_view = current_view;
   }

   if (new_view == VIEW_NONE)
   {
      return;
   }

   if (new_view == VIEW_CUSTOM)
   {
      // Force it to recalculate.
      new_view = VIEW_AREA;
   }

   m_Txp.Off();

   // Update the rulers.
   UpdateRulers();

   // Get our document so we can know how big we are.

   CPmwDoc *pDoc = GetDocument();
   PPNT dimensions;
   PBOX pbox;
   RECT client;

   view_portion = pDoc->get_portion();
   if (view_portion.x0 == -1)
   {
   /* First time... */
      pDoc->set_portion();
      view_portion = pDoc->get_portion();
   }

   dimensions.x = view_portion.x1 - view_portion.x0;
   dimensions.y = view_portion.y1 - view_portion.y0;

// od("View portion: %ld, %ld, %ld, %ld\r\n", view_portion);

/* Get the DC for this window so we can know our attributes. */

   CDC *pdc = GetDC();

/* Build the redisplay context. */

   rc.x_resolution = nearest_resolution(pdc->GetDeviceCaps(LOGPIXELSX));
   rc.y_resolution = nearest_resolution(pdc->GetDeviceCaps(LOGPIXELSY));

// od("Using resolution %d, %d\r\n", rc.x_resolution, rc.y_resolution);

/* Release the DC we got. */

   ReleaseDC(pdc);

/* See what our client is. */

   GetUsableClientRect(&client);
// od("***>>> Client rect: %d, %d, %d, %d <<<***\r\n", client);

/* Space a 1/4" around (theoretically) */

   rc.destination_x0 = rc.x_resolution/4;
   rc.destination_y0 = rc.y_resolution/4;

   InflateRect(&client, -rc.destination_x0, -rc.destination_y0);

   if (client.right <= client.left)
   {
      client.right = client.left + 1;
   }
   if (client.bottom <= client.top)
   {
      client.bottom = client.top + 1;
   }

   PBOX old_source = (area == NULL) ? rc.source_pbox : *area;

/* Handle the appropriate view. */

   FLAGS pos_flags = 0;          // Additional position flags.

   switch (new_view)
   {
      case VIEW_AREA:
      {
         PCOORD n, d;

         if (area == NULL)
         {
            n = (PCOORD)rc.GetScaleNumerator();
            d = (PCOORD)rc.GetScaleDenominator();
         }
         else
         {
         /*
         // Compute the client area dimensions.
         */
            POINT client_dims;

            client_dims.x = client.right - client.left;
            client_dims.y = client.bottom - client.top;

         /*
         // Compute the page box dimensions.
         */

            PPNT area_dims;

            area_dims.x = scale_pcoord(area->x1 - area->x0,
                              rc.x_resolution,
                              PAGE_RESOLUTION);
            
            area_dims.y = scale_pcoord(area->y1 - area->y0,
                              rc.y_resolution,
                              PAGE_RESOLUTION);

         /*
         // Compute the scaling parameters which will fit the page box in the
         // client box.
         //
         //          client x       client y
         // If       --------   <   -------- 
         //           area x         area y
         //
         // then use x ratio, otherwise, use y ratio.
         */

            if (client_dims.x * area_dims.y < client_dims.y * area_dims.x)
            {
            /* Use the x scaling factors. */
               n = (PCOORD)client_dims.x;
               d = area_dims.x;
            }
            else
            {
            /* Use the y scaling factors. */
               n = (PCOORD)client_dims.y;
               d = area_dims.y;
            }
         /* Make us be here! */
            old_source = *area;
            
            if (old_source.x0 == old_source.x1)
            {
               old_source.x1++;
            }
            
            if (old_source.y0 == old_source.y1)
            {
               old_source.y1++;
            }
            
            pos_flags = PVIEW_FLAG_center;
         }

         current_view = VIEW_CUSTOM;
         view_scale(n, d, &dimensions, &pbox);
         update_scrolls(&client, &pbox, TRUE, TRUE);
         break;
      }
      case VIEW_25:
      case VIEW_50:
      case VIEW_100:
      case VIEW_200:
      case VIEW_400:
      {
         static const POINT BASED_CODE view_fraction[] =
         {
            { 1, 4 },            // 25%
            { 1, 2 },            // 50%
            { 1, 1 },            // 100%
            { 2, 1 },            // 200%
            { 4, 1 },            // 400%
         };

         POINT vf = view_fraction[new_view - VIEW_25];      // Watch this!

         current_view = new_view;
         view_scale((PCOORD)vf.x, (PCOORD)vf.y, &dimensions, &pbox);
         update_scrolls(&client, &pbox, TRUE, TRUE);
         break;
      }
      case VIEW_ZOOM_IN:
      case VIEW_ZOOM_OUT:
      {
         current_view = VIEW_CUSTOM;
         PCOORD n, d;
         n = (PCOORD)rc.GetScaleNumerator();
         d = (PCOORD)rc.GetScaleDenominator();
         if (ComputeZoomFraction(&n, &d, new_view))
         {
            pos_flags = PVIEW_FLAG_center;
            if (area != NULL)
            {
               old_source = *area;
               if (old_source.x0 == old_source.x1)
               {
                  old_source.x1++;
               }
               if (old_source.y0 == old_source.y1)
               {
                  old_source.y1++;
               }
            }
            view_scale(n, d, &dimensions, &pbox);
            update_scrolls(&client, &pbox, TRUE, TRUE);
         }
         break;
      }
      case VIEW_FULL_PAGE:
      {
         current_view = new_view;

      /* Always compute us without scroll bars. */

         if (vertical_scroll_shown)
         {
            client.right += GetSystemMetrics(SM_CXVSCROLL);
         }

         if (horizontal_scroll_shown)
         {
            client.bottom += GetSystemMetrics(SM_CYHSCROLL);
         }

      /* Try full height first. */

         view_scale((PCOORD)(client.bottom - client.top)*PAGE_RESOLUTION, dimensions.y*rc.y_resolution, &dimensions, &pbox);

         if (pbox.x1 > (PCOORD)(client.right - client.left))
         {
         /* Do full width instead. */

            view_scale((PCOORD)(client.right - client.left)*PAGE_RESOLUTION, dimensions.x*rc.x_resolution, &dimensions, &pbox);
         }

      /* Handle the scroll bars (always off). */

         update_scrolls(&client, &pbox, FALSE, FALSE);

         break;
      }
      case VIEW_FULL_WIDTH:
      {
         SHORT scroll_width = GetSystemMetrics(SM_CXVSCROLL);
         int height;
         BOOL compute_scroll = TRUE;

         current_view = new_view;

         if (horizontal_scroll_shown)
         {
         /* Scroll bar is shown, but it won't be soon. Pretend it isn't. */
            client.bottom += GetSystemMetrics(SM_CYHSCROLL);
         }

      /*
      // To prevent an oscillation problem, we compute it both with and
      // without a scroll bar and decide which one to pick.
      */

      /* Remember how high we are. */

         height = client.bottom - client.top;

      /* Try computation without the scroll bar first. */

         if (vertical_scroll_shown)
         {
         /* Scroll bar is shown. Pretend it isn't. */
            client.right += scroll_width;
         }

      /* Do the scale. */

         view_scale((PCOORD)(client.right - client.left)*PAGE_RESOLUTION, dimensions.x*rc.x_resolution, &dimensions, &pbox);

         if (pbox.y1 > height)
         {
         /*
         // With wide (without a scroll), we do indeed need a scroll.
         // This may be the first step in an oscillation.
         // Recompute WITH the scroll.
         */
            client.right -= scroll_width;
            view_scale((PCOORD)(client.right - client.left)*PAGE_RESOLUTION, dimensions.x*rc.x_resolution, &dimensions, &pbox);
            if (pbox.y1 <= height)
            {
            /*
            // Now, with a scroll, it fits (we don't need a scroll). This is the oscillation.
            // Leave the page computed as if the scroll bar were there,
            // but leave the scroll bar turned off.
            */
               compute_scroll = FALSE;
            }
         }

         update_scrolls(&client, &pbox, FALSE, compute_scroll);
         break;
      }
      case VIEW_FULL_HEIGHT:
      {
         SHORT scroll_height = GetSystemMetrics(SM_CYHSCROLL);
         int width;
         BOOL compute_scroll = TRUE;

         current_view = new_view;

         if (vertical_scroll_shown)
         {
         /* Scroll bar is shown, but it won't be soon. Pretend it isn't. */
            client.right += GetSystemMetrics(SM_CXVSCROLL);
         }

      /*
      // To prevent an oscillation problem, we compute it both with and
      // without a scroll bar and decide which one to pick.
      */

      /* Remember how wide we are. */

         width = client.right - client.left;

      /* Try computation without the scroll bar first. */

         if (horizontal_scroll_shown)
         {
         /* Scroll bar is shown. Pretend it isn't. */
            client.bottom += scroll_height;
         }

      /* Do the scale. */

         view_scale((PCOORD)(client.bottom - client.top)*PAGE_RESOLUTION, dimensions.y*rc.y_resolution, &dimensions, &pbox);

         if (pbox.x1 > width)
         {
         /*
         // With tall (without a scroll), we do indeed need a scroll.
         // This may be the first step in an oscillation.
         // Recompute WITH the scroll.
         */
            client.bottom -= scroll_height;
            view_scale((PCOORD)(client.bottom - client.top)*PAGE_RESOLUTION, dimensions.y*rc.y_resolution, &dimensions, &pbox);
            if (pbox.x1 <= width)
            {
            /*
            // Now, with a scroll, it fits (we don't need a scroll). This is
            // the oscillation.
            // Leave the page computed as if the scroll bar were there,
            // but leave the scroll bar turned off.
            */
               compute_scroll = FALSE;
            }
         }

         update_scrolls(&client, &pbox, compute_scroll, FALSE);
         break;
      }
      default:
      {
      /* Ignore this. */
         return;
      }
   }

/* Compute the source pbox from our destination client area rect. */

   rc.scaled_source_x0 =
   rc.scaled_source_y0 = 0;

   GetUsableClientRect(&client);       /* Get the now current client. */

/*
// Center the view in the client area.
*/

   pbox.x0 = 0;
   pbox.y0 = 0;
   pbox.x1 = dimensions.x;
   pbox.y1 = dimensions.y;
   rc.pbox_to_screen(&pbox, TRUE);

#if 1
// X centering is not desireable (in my opinion - JN).
// (Or, at least, I used to think so. Times change - JN).
   if (pbox.x1 - pbox.x0 < client.right - client.left)
   {
      rc.destination_x0 = (client.left
                           + client.right
                           - (int)(pbox.x1 - pbox.x0))/2;
   }
#endif

   if (pbox.y1 - pbox.y0 < client.bottom - client.top)
   {
      rc.destination_y0 = (client.top
                           + client.bottom
                           - (int)(pbox.y1 - pbox.y0))/2;
   }

   rc.destination_rect = client;
// od("old source pbox is %ld, %ld, %ld, %ld -> ", rc.source_pbox);
   rc.screen_to_pbox(&client, &pbox);

#if 0
   od("dest rect: %d, %d, %d, %d -> page: ", client);
   od("%ld, %ld, %ld, %ld\r\n", pbox);

   {
      PBOX foo = pbox;
      rc.pbox_to_screen(&foo, TRUE);
      od("Scaled back: %ld, %ld, %ld, %ld\r\n", foo);
   }
#endif
   spacing.x = -pbox.x0;
   spacing.y = -pbox.y0;

   pbox.x0 += view_portion.x0;
   pbox.y0 += view_portion.y0;
   pbox.x1 += view_portion.x0;
   pbox.y1 += view_portion.y0;

// PBOX old_source = rc.source_pbox;

   rc.source_pbox = pbox;

/* Compute view extent. */

   view_extent.x0 = view_portion.x0-spacing.x;
   view_extent.y0 = view_portion.y0-spacing.y;
   view_extent.x1 = view_portion.x1+spacing.x;
   view_extent.y1 = view_portion.y1+spacing.y;

// od("source: %ld, %ld, %ld, %ld; extent: %ld, %ld, %ld, %ld\r\n",
//             rc.source_pbox, view_extent);

   if (old_source.x0 == old_source.x1)
   {
      SetScrollPos(SB_HORZ, 0);
      SetScrollPos(SB_VERT, 0);
   }
#if 1
   else
   {
   /* Not first time! */
      position_view(old_source.x0, old_source.y0,
                     old_source.x1, old_source.y1,
                     PVIEW_FLAG_set_scrolls | pos_flags);
   }
#endif

/* Make sure we're correct. */

   rc.scaled_source_x0 = (rc.source_pbox.x0+spacing.x) * rc.x_resolution;
   rc.scaled_source_y0 = (rc.source_pbox.y0+spacing.y) * rc.y_resolution;

   rc.set_redisplay_pixel();

#if 0
   od("fnl src: %ld, %ld, %ld, %ld; sclsrc:%ld, %ld; dst:%d, %d\r\n"
         "scale factor: %d/%d\r\n",
               rc.source_pbox,
               rc.scaled_source_x0,
               rc.scaled_source_y0,
               rc.destination_x0,
               rc.destination_y0,
               rc.GetScaleNumerator(),
               rc.GetScaleDenominator());
#endif

   // Invalidate our whole view.

   Invalidate(TRUE);
}

void CPmwView::ZoomCommand(VIEW_TYPE new_view)
{
	change_view(new_view);
	if (edit_event_handler == mode_zoom)
	{
		set_arrow_tool();
	}
}

void CPmwView::OnView25View()
{
   ZoomCommand(VIEW_25);
}

void CPmwView::OnUpdateView25View(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(current_view == VIEW_25);
}

void CPmwView::OnView50View()
{
	ZoomCommand(VIEW_50);
}

void CPmwView::OnUpdateView50View(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(current_view == VIEW_50);
}

void CPmwView::OnView100View()
{
   ZoomCommand(VIEW_100);
}

void CPmwView::OnUpdateView100View(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(current_view == VIEW_100);
}

void CPmwView::OnView200View()
{
   ZoomCommand(VIEW_200);
}

void CPmwView::OnUpdateView200View(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(current_view == VIEW_200);
}

void CPmwView::OnView400View()
{
	ZoomCommand(VIEW_400);
}

void CPmwView::OnUpdateView400View(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(current_view == VIEW_400);
}

void CPmwView::OnViewFullPage()
{
   ZoomCommand(VIEW_FULL_PAGE);
}

void CPmwView::OnUpdateViewFullPage(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(current_view == VIEW_FULL_PAGE);
}

void CPmwView::OnViewFullWidth()
{
   ZoomCommand(VIEW_FULL_WIDTH);
}

void CPmwView::OnUpdateViewFullWidth(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(current_view == VIEW_FULL_WIDTH);
}

void CPmwView::OnViewFullHeight()
{
   ZoomCommand(VIEW_FULL_HEIGHT);
}

void CPmwView::OnUpdateViewFullHeight(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(current_view == VIEW_FULL_HEIGHT);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.

void CPmwView::OnSetFocus(CWnd* pOldWnd)
{
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL &&
      pActiveItem->GetItemState() == COleClientItem::activeUIState)
   {
      // need to set focus to this item if it is in the same view
      CWnd* pWnd = pActiveItem->GetInPlaceWindow();
      if (pWnd != NULL)
      {
         pWnd->SetFocus();   // don't call the base class
         return;
      }
   }

   CView::OnSetFocus(pOldWnd);
}

void CPmwView::OnSize(UINT nType, int cx, int cy)
{
   CView::OnSize(nType, cx, cy);
   change_view(VIEW_LAST);

   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL)
   {
      ((CPmwCntrItem*)pActiveItem)->SetExtentAndRects();
   }
}

PUBLIC PCOORD
scroll_scale_pcoord(PCOORD x, PCOORD n, PCOORD d)
{
   if ((x < 0) || (n <= 0) || (d <= 0))
   {
      return (0);
   }

   return (x*n + (d/2))/d;
}

/*
// Handle a scroll event.
*/

void CPmwView::OnScroll(int nBar, UINT nSBCode, UINT nPos)
{
   BOOL bHorz = (nBar == SB_HORZ);
   int zOrig, z;   // z = x or y depending on 'nBar'
   int zMin, zMax;

   zOrig = z = GetScrollPos(nBar);
   GetScrollRange(nBar, &zMin, &zMax);

   ASSERT(zMin == 0);

   if (zMax <= 0)
   {
      TRACE0("Warning: no scroll range - ignoring scroll message\n");
      ASSERT(z == 0);     // must be at top
      return;
   }

   switch (nSBCode)
   {
      case SB_TOP:
      {
         z = 0;
         break;
      }

      case SB_BOTTOM:
      {
         z = zMax;
         break;
      }
      
      case SB_LINEUP:
      {
         z -= 10;
         break;
      }

      case SB_LINEDOWN:
      {
         z += 10;
         break;
      }

      case SB_PAGEUP:
      {
         z -= 100;
         break;
      }

      case SB_PAGEDOWN:
      {
         z += 100;
         break;
      }

#if 1
      case SB_THUMBTRACK:
      {
         z = nPos;
         break;
      }
#endif

      default:        // ignore other notifications
      {
         return;
      }
   }

   if (z < 0)
   {
      z = 0;
   }
   else if (z > zMax)
   {
      z = zMax;
   }

   if (z != zOrig)
   {
      PPNT dimensions;

      PCOORD new_x = rc.source_pbox.x0;
      PCOORD view_width = rc.source_pbox.x1 - new_x;
      PCOORD new_y = rc.source_pbox.y0;
      PCOORD view_height = rc.source_pbox.y1 - new_y;

//    od("view extent: %ld, %ld, %ld, %ld; spacing: %u, %d\r\n",
//                   view_extent, spacing);

      dimensions.x = view_extent.x1 - view_extent.x0;
      dimensions.y = view_extent.y1 - view_extent.y0;

      if (bHorz)
      {
         PCOORD full_range = dimensions.x - view_width - 1;
         new_x = scroll_scale_pcoord(z, full_range, 1000-1) + view_extent.x0;
      }
      else
      {
         PCOORD full_range = dimensions.y - view_height - 1;
         new_y = scroll_scale_pcoord(z, full_range, 1000-1) + view_extent.y0;
      }

//    od("Scroll %d; (%ld, %ld)\r\n", z, new_x, new_y);

//    SetScrollPos(nBar, z);

      position_view(new_x, new_y, 
                  new_x + view_width, new_y + view_height,
                  PVIEW_FLAG_scroll | PVIEW_FLAG_set_scrolls);
   }
}

/*
// Position the view at a certain locaton.
*/

VOID CPmwView::position_view(PCOORD new_x0, PCOORD new_y0, PCOORD new_x1, PCOORD new_y1, FLAGS flags)
{
   BOOL scrolled = FALSE;
   PBOX new_window;

#if 0
   od("PV: %ld, %ld, %ld, %ld (%ld, %ld, %ld, %ld)\r\n",
               new_x0, new_y0, new_x1, new_y1,
               rc.source_pbox);
#endif

/*
// Compute clipped centered coordinates.
*/

   PCOORD window_x_size, window_y_size;

   window_x_size = rc.source_pbox.x1-rc.source_pbox.x0;
   window_y_size = rc.source_pbox.y1-rc.source_pbox.y0;

   if (flags & PVIEW_FLAG_center)
   {
      PCOORD cursor_half_x_size, cursor_half_y_size;

      cursor_half_x_size = (new_x1-new_x0)/2;
      cursor_half_y_size = (new_y1-new_y0)/2;

      new_window.x0 = new_x0+cursor_half_x_size-(window_x_size/2);
      new_window.y0 = new_y0+cursor_half_y_size-(window_y_size/2);
   }
   else
   {
      new_window.x0 = new_x0;
      new_window.y0 = new_y0;
   }

   if (new_window.x0+window_x_size > view_extent.x1)
   {
      new_window.x0 = view_extent.x1-window_x_size;
   }

   if (new_window.x0 < view_extent.x0)
   {
      new_window.x0 = view_extent.x0;
   }

   if (new_window.y0+window_y_size > view_extent.y1)
   {
      new_window.y0 = view_extent.y1-window_y_size;
   }

   if (new_window.y0 < view_extent.y0)
   {
      new_window.y0 = view_extent.y0;
   }

   SHORT x_scale = PAGE_RESOLUTION/rc.x_resolution;
   SHORT y_scale = PAGE_RESOLUTION/rc.y_resolution;
// BOOL can_scroll = current_view >= VIEW_25 && current_view <= VIEW_400;
   BOOL can_scroll = current_view >= VIEW_100 && current_view <= VIEW_400;

   if (can_scroll)
   {
   /* Keep the damn thing aligned on our "grid". */
      if (current_view < VIEW_100)
      {
         x_scale /= rc.GetScaleDenominator();      // Numerator should be 1.
         y_scale /= rc.GetScaleDenominator();
      }
//    od("xscale: %d, yscale: %d\r\n", x_scale, y_scale);
//    od("Window p0: %ld, %ld -> ", new_window.x0, new_window.y0);
      new_window.x0 /= x_scale;
      new_window.x0 *= x_scale;
      new_window.y0 /= y_scale;
      new_window.y0 *= y_scale;
//    od("aligned p0: %ld, %ld\r\n", new_window.x0, new_window.y0);
   }

   new_window.x1 = new_window.x0 + window_x_size;
   new_window.y1 = new_window.y0 + window_y_size;

   if (memcmp(&new_window, &rc.source_pbox, sizeof(PBOX)) != 0)
   {
#if 1
      if (can_scroll && (flags & PVIEW_FLAG_scroll))
      {
         PCOORD width, height, cur_width, cur_height;

         width = new_window.x1 - new_window.x0;
         height = new_window.y1 - new_window.y0;
         cur_width = rc.source_pbox.x1 - rc.source_pbox.x0;
         cur_height = rc.source_pbox.y1 - rc.source_pbox.y0;

         if (cur_width == width && cur_height == height)
         {
            PCOORD dx = rc.source_pbox.x0/x_scale - new_window.x0/x_scale;
            PCOORD dy = rc.source_pbox.y0/y_scale - new_window.y0/y_scale;

            if (current_view > VIEW_100)
            {
               dx *= rc.GetScaleNumerator();    // Denominator should be 1.
               dy *= rc.GetScaleNumerator();    // Denominator should be 1.
            }

            RECT client;
            GetUsableClientRect(&client);

         /* Window is scrolling. */

            if (dx < (PCOORD)(client.right - client.left)
                  && dy < (PCOORD)(client.bottom - client.top))
            {
//             od("SW dx: %d, dy: %d\r\n", (SHORT)dx, (SHORT)dy);
               ScrollWindow((SHORT)dx, (SHORT)dy, NULL, NULL);
               scrolled = TRUE;
            }
         }
      }
#endif

      rc.source_pbox = new_window;

//    od("Spacing: %ld, %ld\r\n", spacing);
      rc.scaled_source_x0 = (rc.source_pbox.x0+spacing.x) * rc.x_resolution;
      rc.scaled_source_y0 = (rc.source_pbox.y0+spacing.y) * rc.y_resolution;

      if (!scrolled)
      {
         Invalidate();
      }

   /*
   // Update the scroll bars.
   */

      if (flags & PVIEW_FLAG_set_scrolls)
      {
         PCOORD view_width = rc.source_pbox.x1 - rc.source_pbox.x0;
         PCOORD view_height = rc.source_pbox.y1 - rc.source_pbox.y0;
         SHORT z;
         PPNT dims;

         dims.x = view_extent.x1 - view_extent.x0;
         dims.y = view_extent.y1 - view_extent.y0;

         PCOORD full_range;

         full_range = dims.x - view_width - 1;
         z = (SHORT)scroll_scale_pcoord(rc.source_pbox.x0 - view_extent.x0, 1000-1, full_range);
         SetScrollPos(SB_HORZ, z);

         full_range = dims.y - view_height - 1;
         z = (SHORT)scroll_scale_pcoord(rc.source_pbox.y0 - view_extent.y0, 1000-1, full_range);
         SetScrollPos(SB_VERT, z);
      }

      // Update the rulers.
      UpdateRulers();

      COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
      if (pActiveItem != NULL)
      {
         ((CPmwCntrItem*)pActiveItem)->SetExtentAndRects();
      }
   }

/*
// The edit background is no longer valid.
// We COULD rebuild it. Maybe later...
*/

   free_edit_background();

   UpdateFrameButtons();
}

/*
// Reframe the view to contain a certain locaton.
*/

VOID CPmwView::reframe_view(PBOX cursor)
{
   if ((!((cursor.x0 <= rc.source_pbox.x0) && (cursor.x1 >= rc.source_pbox.x1)) &&
         (((cursor.x0 < rc.source_pbox.x0) && (rc.source_pbox.x0 > view_extent.x0)) ||
          ((cursor.x1 > rc.source_pbox.x1) && (rc.source_pbox.x1 < view_extent.x1)))) ||
       (!((cursor.y0 <= rc.source_pbox.y0) && (cursor.y1 >= rc.source_pbox.y1)) &&
         (((cursor.y0 < rc.source_pbox.y0) && (rc.source_pbox.y0 > view_extent.y0)) ||
          ((cursor.y1 > rc.source_pbox.y1) && (rc.source_pbox.y1 < view_extent.y1)))))
   {
      position_view(
               cursor.x0,
               cursor.y0,
               cursor.x1,
               cursor.y1,
               PVIEW_FLAG_center | PVIEW_FLAG_set_scrolls | PVIEW_FLAG_scroll);
   }
}

void CPmwView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   OnScroll(SB_HORZ, nSBCode, nPos);
}

void CPmwView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   OnScroll(SB_VERT, nSBCode, nPos);
}

BOOL CPmwView::check_escape_event(EVENT_INFO *info)
{
// return info->event == EVENT_RBUTTON_RISING
//       || (info->event == EVENT_KEY_RISING && info->v_key == VK_ESCAPE);
   return (info->event == EVENT_KEY_RISING && info->v_key == VK_ESCAPE);
}

/*
// Invoke the current edit event handler.
*/

VOID CPmwView::invoke_event_handler(VOID)
{
   if ((GetFocus() == this)
    && ((GetKeyState(VK_CONTROL) & 0x8000) != 0)
    && ((GetKeyState(VK_SPACE) & 0x8000) != 0)
    && (m_fZooming || (edit_state == STATE_NORMAL)))
   {
      m_fZooming = TRUE;
      mode_zoom(&event_info);
   }
   
   else
   {
      if (m_fZooming)
      {
         zoom_cleanup();
         edit_state = STATE_NORMAL;
         m_fZooming = FALSE;
      }
      
      if (edit_event_handler != NULL)
      {
         (this->*edit_event_handler)( &event_info );
      }
   }
}

/*
// Signal a new event.
*/

VOID CPmwView::signal_mouse_event(FLAGS nFlags, CPoint point, EVENT event)
{
   event_info.old_xy = event_info.current_xy;
   event_info.current_xy = point;
   event_info.new_position = event_info.current_xy != event_info.old_xy;

   event_info.flags = nFlags;
   event_info.event = event;

   invoke_event_handler();

/* Make sure we have the focus. */
  if ((::GetFocus() != GetSafeHwnd())
    && (::GetActiveWindow() == AfxGetApp()->m_pMainWnd->GetSafeHwnd())
    && (!(((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetToolBar(TOOLBAR_FORMAT)->SendMessage(UM_HAS_FOCUS, 0,0))))
   {
      SetFocus();
   }
}

/*
// Signal an internal event.
*/

VOID CPmwView::signal_internal_event(EVENT event)
{
   event_info.new_position = FALSE;

   event_info.event = event;

   invoke_event_handler();
}

void CPmwView::signal_mode_event(EVENT event, BOOL fInternal /*=FALSE*/, BOOL fInit /*=TRUE*/)
{
   event_info.new_position = FALSE;

   event_info.event = event;
   event_info.fInternal = fInternal;
   event_info.fInit = fInit;

   invoke_event_handler();
}

void CPmwView::OnMouseMove(UINT nFlags, CPoint point)
{
   signal_mouse_event(nFlags, point, EVENT_MOUSE_MOVE);
}

void CPmwView::OnLButtonDown(UINT nFlags, CPoint point)
{
   SetCapture();
   signal_mouse_event(nFlags, point, EVENT_BUTTON_RISING);
}

void CPmwView::OnLButtonUp(UINT nFlags, CPoint point)
{
   signal_mouse_event(nFlags, point, EVENT_BUTTON_FALLING);

/* don't release the capture if right mouse still down... */

   if( !( nFlags & MK_RBUTTON ))
   {
      ReleaseCapture();
   }
}

void CPmwView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   signal_mouse_event(nFlags, point, EVENT_BUTTON_DBLCLK);
}

void CPmwView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
   signal_mouse_event(nFlags, point, EVENT_RBUTTON_DBLCLK);
}

void CPmwView::OnRButtonDown(UINT nFlags, CPoint point)
{
   SetCapture();

   signal_mouse_event(nFlags, point, EVENT_RBUTTON_RISING);
}

void CPmwView::OnRButtonUp(UINT nFlags, CPoint point)
{
   signal_mouse_event(nFlags, point, EVENT_RBUTTON_FALLING);

   /* don't release the capture if left mouse still down... */
   if( !( nFlags & MK_LBUTTON ))
   {
      ReleaseCapture();
   }
}

void CPmwView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   event_info.new_position = FALSE;

   event_info.v_key = nChar;
   event_info.repeat_count = nRepCnt;
   *(UINT*)&event_info.key_state = nFlags;

   if (event_info.key_state.previous_state)
   {
      event_info.event = EVENT_KEY_DOWN;
   }
   else
   {
      if (nChar == VK_SHIFT)
      {
         event_info.flags |= MK_SHIFT;
      }

      if (nChar == VK_CONTROL)
      {
         event_info.flags |= MK_CONTROL;
      }

      event_info.event = EVENT_KEY_RISING;
   }

   invoke_event_handler();
}

void CPmwView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   event_info.new_position = FALSE;
   event_info.v_key = nChar;

   if (nChar == VK_SHIFT)
   {
      event_info.flags &= ~MK_SHIFT;
   }
   if (nChar == VK_CONTROL)
   {
      event_info.flags &= ~MK_CONTROL;
   }

   event_info.event = EVENT_KEY_FALLING;

   invoke_event_handler();
}

void CPmwView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// od("Got char %d, %d, %u\r\n", nChar, nRepCnt, nFlags);

   event_info.new_position = FALSE;

   event_info.v_key = nChar;
   event_info.repeat_count = nRepCnt;
   *(UINT*)&event_info.key_state = nFlags;

   event_info.event = EVENT_KEY;

   invoke_event_handler();
}

/*
// Toggle an object's select marks in this view.
// If the incoming hdc is 0, get one from the view.
*/

BOOL CPmwView::toggle_selects(SHORT which, PMGPageObjectPtr object, HDC hdc)
{
   HWND hWnd;
   HDC hdcSave;
   HRGN hrgnSave;
   BOOL result;

/* Handle the dc. */

   hdcSave = rc.destination_hdc;
   hrgnSave = rc.clip_region;
   rc.clip_region = NULL;

   if (hdc == 0)
   {
      hWnd = GetSafeHwnd();
      rc.destination_hdc = ::GetDC(hWnd);
   }
   else
   {
      rc.destination_hdc = hdc;
   }

   result = object->toggle_selects(&rc, which);

   if (hdc == 0)
   {
      ::ReleaseDC(hWnd, rc.destination_hdc);
   }

   rc.destination_hdc = hdcSave;
   rc.clip_region = hrgnSave;

   return result;
}

/*
// Toggle an object's select marks in all views.
*/

BOOL CPmwView::toggle_object(SHORT which, PMGPageObjectPtr object)
{
/* Toggle this object in all views. */

   return ((CPmwDoc *)GetDocument())->toggle_selects(NULL, 0, object, which);
}

/*
// Select an object.
*/

VOID CPmwView::select_object(PMGPageObjectPtr object)
{
   GetDocument()->select_object(object);
}

/*
// Deselect an object.
*/

VOID CPmwView::deselect_object(PMGPageObjectPtr object)
{
   GetDocument()->deselect_object(object);
}

/*
// Deselect all objects.
*/

void CPmwView::deselect_all(BOOL fUpdateMenu /*=TRUE*/)
{
   GetDocument()->deselect_all(fUpdateMenu);
}

/*
// An object is being removed from the document's page list.
// Remove all references to it.
// If NULL, remove all object references.
*/

VOID CPmwView::removing_object(PMGPageObjectPtr object)
{
   for (int i = m_RefreshArray.NumberOfExtents(); i-- > 0; )
   {
      REFRESH_EXTENT* pExtent = m_RefreshArray.GetExtent(i);

      if (object == NULL || pExtent->update_object == object)
      {
         // This extent needs to be "dereferenced".
         pExtent->update_object = NULL;
         pExtent->FreeUpdateState();
      }
   }

   if (object == NULL || m_pLastSelectedObject == object)
   {
      m_pLastSelectedObject = NULL;
   }
}

/*
// Set the cursor for this window.
*/

VOID CPmwView::set_cursor(HCURSOR hCursor)
{
#ifdef _WIN32
   SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (LONG)hCursor);
#else
   SetClassWord(GetSafeHwnd(), GCW_HCURSOR, (WORD)hCursor);
#endif
   SetCursor(hCursor);
}

/*
// On update, do this.
*/

VOID CPmwView::OnUpdate(CView *sender, LPARAM lHint, CObject *pHint)
{
   switch (lHint)
   {
      case HINT_Refresh:
      {
         REFRESH_STRUCT *rs = (REFRESH_STRUCT *)pHint;
         add_refresh(rs->pextent, rs->type, rs->object, rs->lprExtraPixels);
         break;
      }
      case HINT_Reset:
      case HINT_Rulers:
      {
         CPmwDoc *pDoc = GetDocument();
         pDoc->toggle_selects(this);
         change_view(VIEW_LAST, TRUE); /*lHint == HINT_Rulers);*/
         pDoc->toggle_selects(this);
         break;
      }
      case HINT_CancelEdit:
      {
         OnCancelEdit();
         break;
      }
      case HINT_ReframeOnTxp:
      {
         reframe_on_txp();
         break;
      }
      case HINT_RefreshPreview:
      {
         // Do nothing.
         break;
      }
      default:
      {
      /* Don't know what this is. */
         Invalidate();
         break;
      }
   }
}

BOOL CPmwView::OnEraseBkgnd(CDC* pDC)
{
/* Save what we change. */

   HDC hdcSave = rc.destination_hdc;
   HRGN hrgnSave = rc.clip_region;

   rc.destination_hdc = pDC->GetSafeHdc();
   rc.clip_region = NULL;

   rc.toggle(FALSE, NULL);

   HPALETTE hOldPal = ::SelectPalette(rc.destination_hdc, (HPALETTE)pOurPal->GetSafeHandle(), FALSE);
   ::RealizePalette(rc.destination_hdc);

   RECT clip;
   GetUsableClientRect(&clip);
   rc.erase_area(clip);

   if (hOldPal != NULL)
   {
      ::SelectPalette(rc.destination_hdc, hOldPal, FALSE);
   }

   rc.toggle(TRUE, NULL);

/* Restore what we changed. */

   rc.destination_hdc = hdcSave;
   rc.clip_region = hrgnSave;

   return TRUE;
}

void CPmwView::GetUsableClientRect(LPRECT pRect)
{
   INHERITED::GetClientRect(pRect);

   if (ShowRulers())
   {
      CRect crClient;

      // Account for the X ruler.
      ASSERT(m_pXRuler->GetSafeHwnd() != NULL);
      if (m_pXRuler->GetSafeHwnd() != NULL)
      {
         m_pXRuler->GetClientRect(crClient);
         pRect->top += crClient.Height();
      }

      // Account for the Y ruler.
      ASSERT(m_pYRuler->GetSafeHwnd() != NULL);
      if (m_pYRuler->GetSafeHwnd() != NULL)
      {
         m_pYRuler->GetClientRect(crClient);
         pRect->left += crClient.Width();
      }
   }
}

/*
// Create the rulers.
*/

BOOL CPmwView::CreateRulers(void)
{
   BOOL fSuccess = FALSE;
   TRY
   {
      m_pXRuler = new CRuler(this, CRuler::Horizontal);
      m_pYRuler = new CRuler(this, CRuler::Vertical);
      m_pXYRuler = new CRuler(this, CRuler::Corner);

      ASSERT(m_pXRuler != NULL);
      ASSERT(m_pYRuler != NULL);
      ASSERT(m_pXYRuler != NULL);

      fSuccess = m_pXRuler->Create(ID_X_RULER)
                     && m_pYRuler->Create(ID_Y_RULER)
                     && m_pXYRuler->Create(ID_RULER_CORNER);
   }
   END_TRY

   ASSERT(fSuccess);
   return fSuccess;
}

/*
// Update the rulers.
*/

void CPmwView::UpdateRulers(void)
{
   BOOL fShowRulers = ShowRulers();
   int nShow = fShowRulers ? SW_SHOW : SW_HIDE;

   if (fShowRulers)
   {
      if (m_pXRuler->GetSafeHwnd() != NULL)
      {
         m_pXRuler->Update();
      }
      if (m_pYRuler->GetSafeHwnd() != NULL)
      {
         m_pYRuler->Update();
      }
      if (m_pXYRuler->GetSafeHwnd() != NULL)
      {
         m_pXYRuler->Update();
      }
   }

   // Show the rulers as appropriate.
   if (m_pXRuler->GetSafeHwnd() != NULL)
   {
      m_pXRuler->ShowWindow(nShow);
   }
   if (m_pYRuler->GetSafeHwnd() != NULL)
   {
      m_pYRuler->ShowWindow(nShow);
   }
   if (m_pXYRuler->GetSafeHwnd() != NULL)
   {
      m_pXYRuler->ShowWindow(nShow);
   }
}

BOOL CPmwView::ShowRulers(void)
{
   return GET_PMWAPP()->ShowRulers();
}

void CPmwView::SetShowRulers(BOOL fShowRulers)
{
   GET_PMWAPP()->SetShowRulers(fShowRulers);
}

BOOL CPmwView::SnapToRulers(void)
{
   // Do we snap for this tool?
   BOOL fToolRulerSnap = (edit_event_handler == mode_create) ||
                        ((edit_event_handler == mode_resize) &&
                         (edit_state == RESIZE_STATE_HANDLE ||
                          edit_state == RESIZE_STATE_MOVING ||
                          edit_state == RESIZE_STATE_MOVING_GUIDE));
                  

   return fToolRulerSnap && GET_PMWAPP()->SnapToRulers();
}

void CPmwView::SetSnapToRulers(BOOL fSnapToRulers)
{
   GET_PMWAPP()->SetSnapToRulers(fSnapToRulers);
}

// Guide Stuff....
BOOL CPmwView::ShowGuides()
{
   return GET_PMWAPP()->ShowGuides();
}

void CPmwView::SetShowGuides(BOOL fShowGuides)
{
   GET_PMWAPP()->SetShowGuides(fShowGuides);
}

BOOL CPmwView::SnapToGuides()
{
   // Do we snap to guides for this tool?
   BOOL fSnapToGuides = (edit_event_handler == mode_create) ||
                        ((edit_event_handler == mode_resize) &&
                         (edit_state == RESIZE_STATE_HANDLE ||
                          edit_state == RESIZE_STATE_MOVING) );

   return fSnapToGuides && GET_PMWAPP()->SnapToGuides();
}

/////////////////////////////////////////////////////////////////////////////
// CreateGuide()
//
// Called on left button-down in rulers -- initiates guide creation.
// The new guide won't be added to the array until a button-up
// happens, which is handled in resize_state_moving_guide()
//
// Params:
//    type:   either GUIDE_HORIZ or GUIDE_VERT
/////////////////////////////////////////////////////////////////////////////
void CPmwView::CreateGuide(int type)
{
   SetCapture();  // we want all mouse events
	edit_state = RESIZE_STATE_MOVING_GUIDE;

   // Get current cursor position
   PPNT Cursor;
   CPoint pt;
   GetCursorPagePos(&Cursor);
   rc.ppnt_to_screen(Cursor, &pt);

   m_GuideIndex = -1;   // not in array yet
   m_GuideType = type;

   DrawGuideLine(&rc, type, pt);
   m_LastGuidePt = pt;

   rc.register_toggle(guide_toggle, this);
}

//////////////////////////////////////////////////////////////////////////////
//
// DrawGuideLine
//
// Draw a vertical or horizontal guideline at the x or y position passed in
// (depending on type), going the width or height of the workspace area. The
// line is either XORed or drawn in GUIDECOLOR, depending on fDoColor flag.
//
// Paramaters:
//    pRC:        pointer to rc to use (if NULL use view's rc)
//    type:       Guide type (either GUIDE_HORIZ or GUIDE_VERT)
//    ptPos:      where to position the guideline (only x or y is used)
//    fDoColor:   if TRUE, draw line in GUIDECOLOR, else XOR line
//
//////////////////////////////////////////////////////////////////////////////
void CPmwView::DrawGuideLine(RedisplayContext* pRC, int type, POINT ptPos, BOOL fDoColor/*=FALSE*/, COLORREF Color /*=(COLORREF)-1*/)
{
   if (pRC == NULL)
      pRC = &rc;

   CDC* pDC;
	CPalette* pOldPal = NULL;

   HDC oldHDC = pRC->destination_hdc;
   if (oldHDC == NULL)
	{
		// Get a DC to use.
		pDC = GetDC();
		if (pDC != NULL)
		{
			pRC->destination_hdc = pDC->GetSafeHdc();
			// Select the PrintMaster palette into this DC.
			pOldPal = pDC->SelectPalette(pOurPal, FALSE);
			pDC->RealizePalette();
		}
	}
	else
	{
		pDC = CDC::FromHandle(pRC->destination_hdc);
	}

	// See if we got a DC to use.
   if (pDC == NULL)
   {
      return;
   }

   CRect crView;
	GetClientRect(crView);

   if (fDoColor)
   {
      // draw Guideline in proper color
      CBrush brush;
		if (Color == (COLORREF)-1)
		{
			Color = GetDocument()->EditingMasterPage() ? m_clrMasterGuides : m_clrGuides;
		}
      brush.CreateSolidBrush(Color);

      CRect crGuide = crView;
      if (type == GUIDE_HORIZ)
      {
         crGuide.top = ptPos.y;
         crGuide.bottom = ptPos.y+1;
      }
      else // vertical
      {
         crGuide.left = ptPos.x;
         crGuide.right = ptPos.x+1;
      }

      pRC->toggle(FALSE, crGuide);
      pDC->FillRect(crGuide, &brush);
      pRC->toggle(TRUE, crGuide);

      brush.DeleteObject();
   }
   else
   {
      // XOR mode
      int nOldROP2 = pDC->SetROP2(R2_NOT);

      if (type == GUIDE_VERT)
      {
         // draw vertical guideline
         pDC->MoveTo(ptPos.x, 0);
         pDC->LineTo(ptPos.x, crView.bottom);
      }
      else
      {
         // draw horizontal guideline
         pDC->MoveTo(0, ptPos.y);
         pDC->LineTo(crView.right, ptPos.y);
      }
      pDC->SetROP2(nOldROP2);
   }

   if (oldHDC == NULL)
	{
		if (pOldPal != NULL)
		{
			pDC->SelectPalette(pOldPal, FALSE);
		}
      ReleaseDC(pDC);
	}

   pRC->destination_hdc = oldHDC;
}

void CPmwView::DeleteGuide(int type, int index)
{
   CPmwDoc* pDoc = GetDocument();

   CRect rcUsable;
   GetUsableClientRect(&rcUsable);
   CRect rcInvalid = rcUsable;

   Array* pGuides =
      (type == GUIDE_HORIZ) ? pDoc->get_horiz_guides() : pDoc->get_vert_guides();

   int count = pGuides->count();
   if (index >= count)
   {
      // bad index!
      ASSERT(0);
      return;
   }
   DWORD oldpos = *((DWORD*)pGuides->get_element(index));
   if (type == GUIDE_HORIZ)
   {
      oldpos = rc.page_y_to_screen(oldpos);
      rcInvalid.top = oldpos-1;
      rcInvalid.bottom = oldpos+1;
   }
   else // Vertical
   {
      oldpos = rc.page_x_to_screen(oldpos);
      rcInvalid.left = oldpos-1;
      rcInvalid.right = oldpos+1;
   }
   pGuides->delete_element(index);

   PBOX extent;
   rc.screen_to_pbox(rcInvalid, &extent);
   pDoc->add_refresh(&extent);
   pDoc->SetModifiedFlag();
}

//////////////////////////////////////////////////////////////////////////////
//
// PtOnGuide()
//
// Checks whether a point is over a guideline (with some slop)
//
// Returns: TRUE if point is over guideline, else FALSE
//
// Paramaters:
//    screenpt:   screen position to check
//    extrapix:   how many pixels on either side of guide to allow in check
//    type:       either GUIDE_HORIZ or GUIDE_VERT
//    index:      index into guide array if guide found,
//                -1 if not found.
//
//////////////////////////////////////////////////////////////////////////////
BOOL CPmwView::PtOnGuide(POINT screenpt, int extrapix, int type, int* index)
{
   CPmwDoc* pDoc = GetDocument();

	if (type == GUIDE_HORIZ)
	{
		*index = PtOnHGuide(screenpt, pDoc->get_horiz_guides(), extrapix);
	}
	else
	{
		*index = PtOnVGuide(screenpt, pDoc->get_vert_guides(), extrapix);
	}
	return (*index != -1);
}

int CPmwView::PtOnHGuide(POINT screenpt, Array* pGuides, int extrapix)
{
	int nIndex = -1;
   for (int i = 0; i < pGuides->count(); i++)
   {
		DWORD* pPos = (DWORD*)pGuides->get_element(i);
		int guidepos = rc.page_y_to_screen((PCOORD)*pPos);
		if (screenpt.y >= (guidepos - extrapix) &&
			 screenpt.y <= (guidepos + extrapix))
		{
			nIndex = i;
			break;
		}
	}
	return nIndex;
}

int CPmwView::PtOnVGuide(POINT screenpt, Array* pGuides, int extrapix)
{
	int nIndex = -1;
   for (int i = 0; i < pGuides->count(); i++)
   {
		DWORD* pPos = (DWORD*)pGuides->get_element(i);
		int guidepos = rc.page_x_to_screen((PCOORD)*pPos);
		if (screenpt.x >= (guidepos - extrapix) &&
			 screenpt.x <= (guidepos + extrapix))
		{
			nIndex = i;
			break;
		}
	}
	return nIndex;
}

BOOL CPmwView::refresh_toggle(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data)
{
   CPmwView *view = (CPmwView *)data;

   return ((CPmwDoc *)view->GetDocument())->toggle_selects(view, rc->destination_hdc); 
}

void CPmwView::stop_editing_frame(void)
{
   if (m_Txp.Valid())
   {
      CFrameObject* pObject = m_Txp.Object();
      ASSERT(pObject != NULL);
      if (pObject != NULL)
      {
         if (pObject->get_select_flags() & SELECT_FLAG_solo_select)
         {
            deselect_object(pObject);
         }
         m_Txp.Off();
         m_Txp.Deselect();
         m_Txp.Reset(TRUE);         // Save the style.

         pObject->SetEditing(FALSE);
      }
   }

   if (m_edit_background != NULL)
   {
      ::DeleteObject(m_edit_background);
      m_edit_background = NULL;
   }
   rc.remove_toggle(redisp_toggle_function);
}

void CPmwView::set_mode(EDIT_EVENT_HANDLER handler, BOOL fInternal /*=FALSE*/, BOOL fInit /*=TRUE*/)
{
   CPmwDoc *pDoc = GetDocument();

   // Signal the end of the old mode.
   EVENT nEvent;
   if (handler == edit_event_handler)
   {
      // Same mode!
      nEvent = EVENT_REENTERING_MODE;
   }
   else
   {
      // Leaving this mode for good.
      nEvent = EVENT_LEAVING_MODE;
   }
   signal_mode_event(nEvent, fInternal, fInit);

   // Set the new mode.
   edit_event_handler = handler;
   if (fInit)
   {
      edit_state = STATE_INIT;
   }

   // Signal the start of the new mode.
   signal_mode_event(EVENT_ENTERING_MODE, fInternal, fInit);
}

void CPmwView::OnInitMenu(CMenu* pMenu)
{
   ((CMainFrame *)AfxGetApp()->m_pMainWnd)->OIM(pMenu);
   CView::OnInitMenu(pMenu);
}

void CPmwView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
   ((CMainFrame *)AfxGetApp()->m_pMainWnd)->OIMP(pPopupMenu, nIndex, bSysMenu);
   CView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

void CPmwView::OnKillFocus(CWnd* pNewWnd)
{
   CView::OnKillFocus(pNewWnd);
   GetDocument()->flush();
}

void CPmwView::OnEditUndo()
{
   GetDocument()->Undo();
}

void CPmwView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
   CPmwDoc* pDoc = GetDocument();

   BOOL fCanUndo = pDoc->CanUndo();

   // JRO CString csText = "&Undo ";
   static LPCSTR pszUndo = NULL;
   if (pszUndo == NULL)
   {
      pszUndo = GET_PMWAPP()->GetResourceStringPointer (IDS_MENU_UNDO);
   }
   CString csText = pszUndo;

   if (fCanUndo)
   {
      CString csName;
      pDoc->UndoName(csName);

      csText += csName;
   }
   csText += "\tCtrl+Z";         // Bugger! You have to add this, too.

   pCmdUI->SetText(csText);
   pCmdUI->Enable(fCanUndo);
}

void CPmwView::OnEditRedo()
{
   GetDocument()->Redo();
}

void CPmwView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
   CPmwDoc* pDoc = GetDocument();

   BOOL fCanRedo = pDoc->CanRedo();

   // JRO CString csText = "&Redo ";
   static LPCSTR pszRedo = NULL;
   if (pszRedo == NULL)
   {
      pszRedo = GET_PMWAPP()->GetResourceStringPointer (IDS_MENU_REDO);
   }
   CString csText = pszRedo;

   if (fCanRedo)
   {
      CString csName;
      pDoc->RedoName(csName);

      csText += csName;
   }
   csText += "\tCtrl+Y";         // Bugger! You have to add this, too.

   pCmdUI->SetText(csText);
   pCmdUI->Enable(fCanRedo);
}

void CPmwView::OnTimer(UINT nIDEvent)
{
   switch (nIDEvent)
   {
      case TXP_CURSOR_TIMER:
      {
//       od("Toggle the cursor!\r\n");
         ToggleTxpCursor();
         break;
      }
      default:
      {
         CView::OnTimer(nIDEvent);
         break;
      }
   }
}

/*
// Toggle the txp cursor if there is one.
*/

void CPmwView::ToggleTxpCursor(void)
{
   if (m_Txp.Valid())
   {
      if (m_Txp.Visible() & CTxp::TOGGLE_Cursor)
      {
         m_Txp.Off(CTxp::TOGGLE_Cursor);

         if (IsActiveView())
         {
         /*
         // Normal active toggle.
         // Turn back on in other state.
         */
            cursor_visible = !cursor_visible;
            m_Txp.On(CTxp::TOGGLE_Cursor);
         }
         else
         {
         /*
         // Not currently active. Leave cursor off, but make it so it
         // will show up when turned back on.
         */
            cursor_visible = TRUE;
         }
      }
   }
}

void CPmwView::TxpCursorOn(void)
{
   if (m_Txp.Transition())
   {
      m_Txp.Off(CTxp::TOGGLE_Cursor);
      cursor_visible = TRUE;        // Make the cursor show up next time.
   }
}

BOOL CPmwView::IsActiveView(void)
{
   CFrameWnd* pMainFrm = (CFrameWnd*)AfxGetMainWnd();
   CWnd* pActiveWindow = GetActiveWindow();
   CWnd* pActiveView = pMainFrm->GetActiveFrame()->GetActiveView();

   if (pMainFrm == pActiveWindow)
   {
      if (pActiveView == this)
      {
         return TRUE;
      }
   }
   return FALSE;
}

void CPmwView::TxpOn(void)
{
   if (IsActiveView())
   {
      // Turn on everything (cursor and select).
      m_Txp.On();
   }
   else
   {
      // Only turn on the select since we are not active.
      m_Txp.On(CTxp::TOGGLE_Select);
   }
}

void CPmwView::OnViewAddressBook()
{
/*
// Only put up the address book if the configuration supports it.
*/
   if (!GetConfiguration()->SupportsAddressBook())
   {
      return;
   }

/*
// Put up the address book.
*/

   CPmwDoc* pDoc = GetDocument();
   
   if (pDoc != NULL)
   {
      CStdMacroServer* pMacroServer = pDoc->GetMacroServer();
      
      if (pMacroServer != NULL)
      {
         NameListRecord* pNameList = pMacroServer->GetNameList();
         
         if (pNameList != NULL)
         {
            if (CAddressBook::Run(AfxGetMainWnd(), pNameList, FALSE, FALSE, ADDRESS_BOOK_COLOR))
            {
               pMacroServer->AfterAddressBookRun(this, GetDocument());
            }
         }
      }
   }
}

void CPmwView::OnWithHelpViewAddressBook() 
{
   /*
   // Put up the address book.
   */

   CPmwDoc* pDoc = GetDocument();
   if (pDoc != NULL)
   {
     pDoc->EditNameList(TRUE);
	}
}
/*
// Create the standard edit desktop.
*/

void CPmwView::CreateEditDesktop(void)
{
   //m_pDesktop = new CDocEditDesktop(this);
	m_pDesktop = new CWebEditDesktop(this);
}

BOOL CPmwView::CanInsertPictures(void)
{
   return TRUE;
}

/*
// Helper that creates the doc's chosen picture at the right place in the
// right mode.
*/

BOOL CPmwView::CreateChosenPicture(PPNT* pOrigin /*=NULL*/, BOOL bAddUndo/* = TRUE*/)
{
	CPmwDoc* pDoc = GetDocument();
	set_arrow_tool();
   if (pDoc->create_chosen_picture(&(get_rc()->source_pbox), pOrigin) == ERRORCODE_None)
	{
	   //if this function is called from a paste command, the command will be added to the undo by the PostPaste call
	   if(bAddUndo)
		   	pDoc->AddCreateCommand(IDCmd_AddPicture);

		return TRUE;
	}
	return FALSE;
}

/*
// Create the standard drop target.
*/

void CPmwView::CreateDropTarget(void)
{
   m_pDropTarget = new COleDropTarget();
   m_pDropTarget->Register(this);
}

DROPEFFECT CPmwView::ComputeDropEffect(COleDataObject* pDataObject, DWORD dwKeyState)
{
// Change this for OLE16
//#ifdef WIN32
#if 1
   DROPEFFECT deState =
      (dwKeyState & MK_CONTROL)
         ? (dwKeyState & MK_SHIFT)
            ? DROPEFFECT_LINK          /* Ctrl+Shift = Link */
            : DROPEFFECT_COPY          /* Ctrl = Copy */
         : DROPEFFECT_MOVE;

   return deState;
#else
   return DROPEFFECT_NONE;
#endif
}

BOOL CPmwView::ToggleDragFeedback(RedisplayContext* pContext, BOOL fTurnOn, LPRECT pcrArea, LPVOID pData)
{
   CPmwView* pView = (CPmwView*)pData;

   if (pView->m_DragDropState.m_prevDropEffect != DROPEFFECT_NONE)
   {
      CRect crInt;
      crInt.IntersectRect(pView->m_DragDropState.m_crDrawnRect, pcrArea);

      if (!crInt.IsRectEmpty())
      {
         ::DrawFocusRect(pContext->destination_hdc, pView->m_DragDropState.m_crDrawnRect);
         return TRUE;
      }
   }
   return FALSE;
}

void CPmwView::UpdateDragFeedback(DROPEFFECT de /*=DROPEFFECT_NONE*/,
                                  const PPNT* pPoint /*=NULL*/)
{
/* See if we need to update the drag feedback outline. */

   PPNT p;

   if (de != DROPEFFECT_NONE)
   {
      p.x = pPoint->x - m_DragDropState.m_DragOffset.x;
      p.y = pPoint->y - m_DragDropState.m_DragOffset.y;
   }

   if (de != m_DragDropState.m_prevDropEffect
         || p.x != m_DragDropState.m_DragPoint.x
         || p.y != m_DragDropState.m_DragPoint.y)
   {
      // otherwise, cursor has moved -- need to update the drag feedback
      CClientDC dc(this);
      if (m_DragDropState.m_prevDropEffect != DROPEFFECT_NONE)
      {
         // erase previous focus rect
         dc.DrawFocusRect(m_DragDropState.m_crDrawnRect);
      }
      m_DragDropState.m_prevDropEffect = de;
      if (m_DragDropState.m_prevDropEffect != DROPEFFECT_NONE)
      {
         // draw new focus rect
         m_DragDropState.m_DragPoint = p;

         // Compute new focus rect.

         PBOX pb;
         pb.x0 = p.x;
         pb.y0 = p.y;
         pb.x1 = p.x + m_DragDropState.m_DragSize.x;
         pb.y1 = p.y + m_DragDropState.m_DragSize.y;

         rc.pbox_to_screen(&pb, TRUE);
         rc.convert_pbox(&pb, &m_DragDropState.m_crDrawnRect, NULL);

         dc.DrawFocusRect(m_DragDropState.m_crDrawnRect);
      }
   }
}

DROPEFFECT CPmwView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
   pDataObject->BeginEnumFormats();
   FORMATETC fmt;
   while (pDataObject->GetNextFormat(&fmt))
   {
      TRACE1("format: %d\n", fmt.cfFormat);
   }
   TRACE0("OnDragEnter...\n");

   m_DragDropState.m_fShowFeedback = GetObjectInfo(pDataObject,
                                                     &m_DragDropState.m_DragSize,
                                                     &m_DragDropState.m_DragOffset);

   if (m_DragDropState.m_fShowFeedback)
   {
      rc.register_toggle(ToggleDragFeedback, this);
   }

   TRACE2("size: %ld, %ld; offset: %ld, %ld\n", m_DragDropState.m_DragSize, m_DragDropState.m_DragOffset);

   return OnDragOver(pDataObject, dwKeyState, point);
}

DROPEFFECT CPmwView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// Update move rectangles when dragging within ourselves.
	if (m_fDragActive)
	{
		// The movement flags should be a left mouse button plus the current key state.
		UINT nFlags = MK_LBUTTON | dwKeyState;
		OnMouseMove( nFlags, point );
	}

	// Still compute the drag process.
   DROPEFFECT de = ComputeDropEffect(pDataObject, dwKeyState);

   if (m_DragDropState.m_fShowFeedback)
   {
      PPNT p;

      rc.screen_to_ppnt(point, &p);

      UpdateDragFeedback(de, &p);
   }

   return de;
}

void CPmwView::OnDragLeave(void)
{
   if (m_DragDropState.m_fShowFeedback)
   {
      UpdateDragFeedback();

      rc.remove_toggle(ToggleDragFeedback);
   }
}

BOOL CPmwView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
   OnDragLeave();

   TRACE0("OnDrop...\n");

	// Block against a self drag/drop, our move process will handle this case.
	if (m_fDragActive)
	{
		m_fLocalDragDrop = TRUE;
		return TRUE;
	}

   PPNT PagePoint;

   rc.screen_to_ppnt(point, &PagePoint);

#ifdef _DEBUG
//   CString csMessage;
   FORMATETC fmt;

   pDataObject->BeginEnumFormats();
   while (pDataObject->GetNextFormat(&fmt))
   {
      UINT uFormat = fmt.cfFormat;

      CString csLine;
      if (uFormat >= 0xc000)
      {
         char Buffer[80];
         GetClipboardFormatName(uFormat, Buffer, sizeof(Buffer));

         csLine.Format("Format '%s' (%u) is available\n", Buffer, uFormat);
         TRACE(csLine);
      }
      else
      {
         csLine.Format("Format (%u) is available\n", uFormat);
         TRACE(csLine);
      }
//      csMessage += csLine;
   }
//   AfxMessageBox(csMessage);

#endif
   UINT uFormat = 0;

   if (pDataObject->IsDataAvailable(CArtBrowserDialog::m_cfDataExchange))
	{
		// The internal format takes precendence!
		uFormat = CArtBrowserDialog::m_cfDataExchange;
	}
	else if (pDataObject->IsDataAvailable(kComponentFormat))
	{
		uFormat = kComponentFormat;
	}
	else if (pDataObject->IsDataAvailable(CF_HDROP))
   {
      uFormat = CF_HDROP;
      // DIB overrides HDROP.
      if (pDataObject->IsDataAvailable(CF_DIB))
      {
         uFormat = CF_DIB;
      }
   }
	else if (pDataObject->IsDataAvailable(CF_METAFILEPICT))
	{
		uFormat = CF_METAFILEPICT;
	}
	else if (pDataObject->IsDataAvailable(CF_BITMAP))
	{
		uFormat = CF_BITMAP;
	}

   CPmwDoc* pDoc = GetDocument();

   pDoc->PrePaste();

   if (clipboard.PasteFromOleDataObject(this, uFormat, pDataObject, &PagePoint, TRUE) != ERRORCODE_None)
   {
      AfxMessageBox(IDP_FAILED_TO_CREATE);
      pDoc->AbortPaste();
   }
   else
   {
      pDoc->PostPaste();
   }
   return TRUE;         // We always return TRUE; else, the main frame takes over.
}

BOOL CPmwView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll /*=TRUE*/)
{
   TRACE0("OnScrollBy...\n");
   return CView::OnScrollBy(sizeScroll, bDoScroll);
}

/*
// This handles a File Manager style file drop.
*/

void CPmwView::OnDropFiles(HDROP hDropInfo) 
{     
// Change this for OLE16
//#ifdef WIN32
#if 1
   TRACE0("OnDropFiles...\n");

   SetActiveWindow();      // activate us first !

   POINT p;
   PPNT Where;

   ::DragQueryPoint(hDropInfo, &p);
   rc.screen_to_ppnt(p, &Where);

   CPmwDoc* pDoc = GetDocument();
   pDoc->PrePaste();
   pDoc->PasteDropFiles(hDropInfo, &Where);
   pDoc->PostPaste();

   ::DragFinish(hDropInfo);
#endif
}

void CPmwView::OnEditPasteSpecial() 
{
   COlePasteSpecialDialog Dialog(PSF_SELECTPASTE, NULL, this);
   Dialog.AddStandardFormats(TRUE);

   if (Dialog.DoModal() == IDOK)
   {
      CPmwDoc* pDoc = GetDocument();
      UINT uSelectionType = Dialog.GetSelectionType();
      TRACE1("uSelectionType == %u\n", uSelectionType);
      ERRORCODE error = ERRORCODE_None;

      if (uSelectionType == COlePasteSpecialDialog::pasteLink)
      {
         CPmwCntrItem* pItem = new CPmwCntrItem(pDoc);

         if (Dialog.CreateItem(pItem))
         {
            pDoc->PrePaste();
            if ((error = pDoc->FinishOleObjectCreate(pItem)) != ERRORCODE_None)
            {
               pItem->Delete();
            }
         }
         else
         {
            error = ERRORCODE_Memory;
         }
      }
      else
      {
         int nIndex = Dialog.GetPasteIndex();
         UINT uFormat = Dialog.m_ps.arrPasteEntries[nIndex].fmtetc.cfFormat;

         TRACE1("uFormat == %u\n", uFormat);

         if (uFormat >= 0xc000)
         {
            uFormat = 0;
         }

         COleDataObject DataObject;
         DataObject.AttachClipboard();
         pDoc->PrePaste();
         error = clipboard.PasteFromOleDataObject(this, uFormat, &DataObject);
      }

      if (error == ERRORCODE_None)
      {
         pDoc->PostPaste();
      }
      else
      {
         pDoc->AbortPaste();
      }
   }
}

void CPmwView::OnUpdateEditPasteSpecial(CCmdUI* pCmdUI) 
{
   BOOL has_data = FALSE;

   if (!m_Txp.Valid())
   {
      has_data = clipboard.CanPasteObjects();
   }
   pCmdUI->Enable(has_data);
}

BOOL CPmwView::GadgetVisible(UINT uID)
{
   // Make the zoom gadget visible by default.
   return uID == ID_ZOOM_GADGET || uID == ID_STATUS_GADGET;
}

BOOL CPmwView::GadgetEnabled(UINT uID, UINT uPart)
{
   BOOL fResult = FALSE;

   // Disable all parts if the gadget is not visible.
   if (GadgetVisible(uID))
   {
      switch (uID)
      {
         case ID_ZOOM_GADGET:
         {
            // Handle the zoom gadget part-by-part.
            switch (uPart)
            {
               case ID_GADGET_BACK:
               {
                  fResult = CanZoomOut();
                  break;
               }
               case ID_GADGET_NEXT:
               {
                  fResult = CanZoomIn();
                  break;
               }
               default:
               {
                  fResult = TRUE;
                  break;
               }
            }
            break;
         }
         case ID_PAGE_GADGET:
         {
            // Handle the page gadget part-by-part.
            CPmwDoc* pDocument = GetDocument();
            switch (uPart)
            {
               case ID_GADGET_BACK:
               {
                  fResult = pDocument->CanPageBack();
                  break;
               }
               case ID_GADGET_NEXT:
               {
                  // For the moment!
                  fResult = pDocument->CanPageNext();
                  break;
               }
               default:
               {
                  fResult = TRUE;
                  break;
               }
            }
            break;
         }
         default:
         {
            // It's visible, enable all parts by default.
            fResult = TRUE;
            break;
         }
      }
   }

   return fResult;
}

void CPmwView::GadgetClicked(UINT uID, UINT uPart, CWnd* pGadget)
{
   // By default, text clicks put up a menu.
   if (uPart == ID_GADGET_TEXT)
   {
      // Get the menu name if any.
      LPCSTR pszMenuName = GadgetMenuName(uID, uPart);
      if (pszMenuName != NULL)
      {
         // Load the menu will all the popups.
         CMenu Menu;
         if (Menu.LoadMenu(IDR_GADGETS))
         {
            // MLSTODO: Look up menu by name. This should be something in UTIL.
            UINT uMenuCount = Menu.GetMenuItemCount();
            for (UINT uMenu = 0; uMenu < uMenuCount; uMenu++)
            {
               CString csMenu;
               TRY
               {
#ifdef WIN32
                  Menu.GetMenuString(uMenu, csMenu, MF_BYPOSITION);
#else
                  Menu.GetMenuString(uMenu, csMenu.GetBuffer(128), 128-1, MF_BYPOSITION);
                  csMenu.ReleaseBuffer();
#endif
               }
               END_TRY

               if (stricmp(pszMenuName, csMenu) == 0)
               {
                  // Found the menu.
                  CMenu* pPopupMenu = Menu.GetSubMenu(uMenu);
                  if (pPopupMenu != NULL)
                  {
                     CPoint p;
#ifdef WIN32
                     if (!GetCursorPos(&p))
                     {
                        CRect crGadget;
                        pGadget->GetWindowRect(crGadget);
                        p.x = (crGadget.left+crGadget.right)/2;
                        p.y = (crGadget.top+crGadget.bottom)/2;
                     }
#else
                     GetCursorPos(&p);
#endif

                     // Make parent the main window so UI update will work.
                     pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, p.x, p.y, AfxGetMainWnd(), NULL);
                  }
               }
            }
         }
      }
   }
   else
   {
      // Check for non text clicks.
      switch (uID)
      {
         // The default zoom gadget behavior is handled here.
         case ID_ZOOM_GADGET:
         {
            switch (uPart)
            {
               case ID_GADGET_BACK:
               {
                  OnViewZoomOut();
                  break;
               }
               case ID_GADGET_NEXT:
               {
                  OnViewZoomIn();
                  break;
               }
            }
            break;
         }
         case ID_PAGE_GADGET:
         {
            CPmwDoc* pDocument = GetDocument();
            switch (uPart)
            {
               case ID_GADGET_BACK:
               {
                  BeforePageChange();
                  pDocument->DoPageBack();
                  break;
               }
               case ID_GADGET_NEXT:
               {
                  BeforePageChange();
                  pDocument->DoPageNext();
                  break;
               }
            }
            break;
         }
      }
   }
}

void CPmwView::GadgetText(UINT uID, CString& csText)
{
   switch (uID)
   {
      case ID_ZOOM_GADGET:
      {
         csText.Format("%d%%", (int)(ZoomFactor()*100.0));
         break;
      }
      default:
      {
         csText.Empty();
         break;
      }
   }
}

LPCSTR CPmwView::GadgetMenuName(UINT uID, UINT uPart)
{
   LPCSTR pszMenuName = NULL;

   if (uID == ID_ZOOM_GADGET)
   {
      pszMenuName = "Zoom";
   }

   return pszMenuName;
}

void CPmwView::BeforePageChange(BOOL fToMasterPage /*=FALSE*/)
{
	if (fToMasterPage
			|| m_Txp.Valid()
			|| GetDocument()->EditingMasterPage())
   {
      set_arrow_tool();
   }
}

void CPmwView::SetPanel(int nPanel, BOOL fUpdateMenu /*=TRUE*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// View edit state routines.
// These are used by spell-check right now (and anyone else who wants them).

CViewEditState* CPmwView::CreateEditState(void)
{
   return new CViewEditState;
}

void CPmwView::SaveEditState(CViewEditState* pState)
{
   pState->m_lPage = GetDocument()->CurrentPageRecord();
   pState->m_SourceBox = rc.source_pbox;
   pState->m_EditEventHandler = edit_event_handler;
   pState->m_nState = edit_state;
   pState->m_nView = current_view;
   pState->m_nPanel = GetDocument()->get_current_panel();
   if (m_Txp.Valid())
   {
      pState->m_pTextObject = m_Txp.Object();
      pState->m_lTextIndex = m_Txp.Index();
      pState->m_lTextSelectStart = m_Txp.SelectStart();
      pState->m_lTextSelectEnd = m_Txp.SelectEnd();
   }
   else
   {
      pState->m_pTextObject = NULL;
   }
}

void CPmwView::RestoreEditState(CViewEditState* pState)
{
// m_Txp.Off();

   CPmwDoc* pDoc = GetDocument();
   // Set the page correctly.
   if (pDoc->CurrentPageRecord() != pState->m_lPage)
   {
      pDoc->LoadPage(pState->m_lPage);
   }

   if (pDoc->get_current_panel() != pState->m_nPanel)
   {
      SetPanel(pState->m_nPanel);
   }

   // Get rid of any edit background.
   free_edit_background();

   // Restore the event handler.
   set_mode(pState->m_EditEventHandler, TRUE);
   edit_state = pState->m_nState;

   if (pState->m_pTextObject == NULL)
   {
      pDoc->deselect_all();
   }
   else
   {
      // Restore the text editing state.
      // This covers most of the user-notable settings (but is not perfect!)
      if (m_Txp.Object() == pState->m_pTextObject)
      {
         m_Txp.SetPosition(pState->m_lTextIndex, 0, TRUE);
      }
      else
      {
         // Set up the Txp again.
         m_Txp.Init(pState->m_pTextObject, pState->m_lTextIndex, TxpToggleFunction, this);
      }

      rc.register_toggle(redisp_toggle_function, this);
      m_Txp.Object()->SetEditing(TRUE);

      // Restore the text selection.
      if (pState->m_lTextSelectStart != pState->m_lTextSelectEnd)
      {
         // Text may have changed. We do not do this anymore.
//       m_Txp.SelectText(pState->m_lTextSelectStart, pState->m_lTextSelectEnd);
      }
   }

   // Position the view.

   change_view(pState->m_nView,
               TRUE,
               &pState->m_SourceBox);
}

void CPmwView::ViewObject(PMGPageObject* pObject, PBOX* pWhere /*=NULL*/)
{
// m_Txp.Off();

   CPmwDoc* pDoc = GetDocument();

   // Make sure the object is in the current list.
   if (pObject->Page() != pDoc->CurrentPageRecord())
   {
      pDoc->LoadPage(pObject->Page());
   }

   // Set the correct panel.
   SetPanel(pObject->get_panel());

   if (pWhere != NULL)
   {
      // Make it be visible.
      reframe_view(*pWhere);
   }
}

void CPmwView::ViewText(DB_RECORD_NUMBER lFrame, CHARACTER_INDEX lStartCharacter, CHARACTER_INDEX lEndCharacter, BOOL Zoom)
{
   CPmwDoc* pDoc = GetDocument();

   free_edit_background();
   m_Txp.Off();
   
   if (Zoom == TRUE)
   {
      VIEW_TYPE nView = VIEW_100;
      if (pDoc->get_project_type() == PROJECT_TYPE_Banner
            && pDoc->get_main_frame() != NULL
            && pDoc->get_main_frame()->FrameRecord() == lFrame)
      {
         nView = VIEW_FULL_WIDTH;
      }
      change_view(nView);
    }

   if (SetTxpFrame(lFrame, lStartCharacter)
       && pDoc->selected_objects() != NULL)
   {
      // The selected object is the frame.
      if (!m_Txp.Valid())
      {
         BeginFrameEdit((CFrameObject*)pDoc->selected_objects(), lStartCharacter);
      }
      m_Txp.SelectText(lStartCharacter, lEndCharacter);
      reframe_on_txp();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CViewEditState

CViewEditState::CViewEditState()
{
}

CViewEditState::~CViewEditState()
{
}
  
void CPmwView::OnFileSaveAsGraphic() 
{
   DoFileSaveAsGraphic ();
}

void CPmwView::OnZoomToSelected() 
{
   PBOX        pboxSelected;
   CPmwDoc     *pDoc = GetDocument();

   if(pDoc->GetSelectBound(&pboxSelected) == TRUE)
   {
      change_view(VIEW_AREA, TRUE, &pboxSelected);
   }
   else
      AfxMessageBox(IDS_NO_OBJECT_SELECTED);
}

void CPmwView::OnYywb() 
{
#ifdef LOCALIZE
   if (GetConfiguration()->RemoveYearYouWereBorn())
   {
     return;
   }
#endif

   // Bring up the dialog
	CYywbDlg YywbDlg(this);

	// Wait for completion
	if (YywbDlg.DoModal() == ID_YYWB_ERROR)
	{
		// Some kind of problem
		AfxMessageBox(IDS_CANT_LOAD_YYWB);
	}	
	
}

void CPmwView::OnUpdateYywb(CCmdUI* pCmdUI) 
{
#ifdef LOCALIZE
   if (GetConfiguration()->RemoveYearYouWereBorn())
   {
		if (pCmdUI->m_pMenu)
		   pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
   }
#endif

   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
      pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
   else
      pCmdUI->Enable(TRUE);	
}

void CPmwView::OnWebSiteProperties() 
{
   CWebSitePropertiesDialog   dlgWebProp;

   dlgWebProp.DoModal();
}

void CPmwView::OnUpdateWebSiteProperties(CCmdUI* pCmdUI)
{
   CPmwDoc *pDoc = GetDocument();
	if ((pDoc) && (pDoc->SupportsHTML()))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CPmwView::OnAddPageNumber()
{
	InsertCharacter(MACRO_CHARACTER_Page);
}

//
// Enable the Add Page Number command when editing text on a multi-page project
//
void CPmwView::OnUpdateAddPageNumber(CCmdUI* pCmdUI)
{
	if (GetDocument()->IsMultiPageProjectType())
	{
		OnEditingText(pCmdUI);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CPmwView::OnAddPictureFromDisk() 
{
	if (GetDocument()->ChooseExternalPicture(GET_PMWAPP()->get_last_picture_name(),
														  NULL,
														  "[[P]]") == IDOK)
	{
		CreateChosenPicture();
	}
}

// A hook function to handle things after an object has changed position.
void CPmwView::AfterPositionChange(void)
{
	// Nothing to do in the base class.
}

void CPmwView::OnGotoInstructionPage()
{
	//if the ini entry is set to false display the instructions in the 
	//HTML help window
   if (!GET_PMWAPP()->OurGetProfileInt("Debug", "EditInstructions", FALSE))
	{
		ViewInstructionsInHelpWnd(TRUE);
		return;
	}
	
	//otherwise display the page in the workspace.

	CPmwDoc* pDoc = GetDocument();
	if(pDoc->UpdateInstructionPages() == FALSE)
		return;

	// Compute which page to go to.
	if (pDoc->EditingInstructionPage())
	{
		// Go back to the document page (the one we came from).
		if (m_dwPushedPageIndex >= pDoc->NumberOfPages())
		{
			m_dwPushedPageIndex = pDoc->NumberOfPages()-1;
		}
      BeforePageChange();
		pDoc->GotoPage(m_dwPushedPageIndex);
	}
	else
	{
		// On a normal page. Go to the Instruction page.
		DWORD dwIndex;
		DocumentRecord* pDocRec = pDoc->DocumentRecord();

		// Remember what page we were on.
		m_dwPushedPageIndex = pDoc->CurrentPageIndex();

		dwIndex = 0;

		// Load the Instruction page.
		if (dwIndex < pDocRec->NumberOfInstructionPages())
		{
         BeforePageChange(TRUE);
			pDoc->GotoInstructionPage(dwIndex);
		}
	}
}

void CPmwView::ViewInstructionsInHelpWnd(BOOL bShowWindow)
{
	if(GetConfiguration()->SupportsInstructionPage() == FALSE)
		return;

	CPmwDoc* pDoc = (CPmwDoc*)GetDocument();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(pDoc->NumberOfInstructionPages() == 0 && pFrame->HelpWindow().IsWindowVisible())
	{
		pFrame->HelpWindow().DisplayTopic(NULL);		
	}

	//create the doc name.  Use the doc title + "Html"
	CString csDocName = pDoc->GetTitle() + ".html";

	//create a temp file in the user\tmp dir.
	CString csOutputFile;
	Util::ConstructPath(csOutputFile, GET_PMWAPP()->GetTransferDirectory(), csDocName);
	CFileFind ff;
	if(ff.FindFile(csOutputFile))
	{
		pFrame->HelpWindow().DisplayTopic(csOutputFile, FALSE);
		//make sure the help window is visible
		if(bShowWindow && pFrame->HelpWindow().IsWindowVisible()  == FALSE)
		{
			pFrame->SendMessage(WM_COMMAND, ID_VIEW_HELPWINDOW);
		}
		return;	
	}
	
	
	CHTMLTagDocument tagDocument;
	CHTMLTagHeader tagHeader;
	CHTMLTagBody tagBody;
	CHTMLFont text;
	text.SetFace("Times New Roman");
	text.SetColor(0,0,0);
	text.SetSize(15);

	CString csHeader;
	LoadConfigurationString(IDS_INSTRUCTION_TITLE, csHeader);
	CHTMLTagTitle tagTitle(csHeader);
	csHeader += "\n";
	text.AddText(csHeader);
	tagBody.Add(text);

	text.Empty();
	text.SetFace("Times New Roman");
	text.SetColor(0,0,0);
	text.SetSize(13);

	CString csDescription = pDoc->GetFriendlyName();
	if(csDescription.IsEmpty())
		csDescription = pDoc->GetTitle();

	csDescription += "\n";
	text.AddText(csDescription);
	tagBody.Add(text);

	text.Empty();
	text.SetFace("Times New Roman");
	text.SetColor(0,0,0);
	text.SetSize(12);

	CTextRecord* pTextRecord = NULL;
	ERRORCODE errorcode;
	CString csText;

	PMGPageObject *   pObject;
	ObjectList* pList = pDoc->GetInstructionItems();
	if(!pList)
	{
		return;
	}
	ASSERT(pList);
	for (pObject = (PMGPageObject*)pList->first_object();
						pObject != NULL;
						pObject = (PMGPageObject*)pObject->next_object())
	{
		//check for grapic or group type
		if(pObject->type() == OBJECT_TYPE_Frame)
		{
			pTextRecord = ((CFrameObject*)pObject)->LockTextRecord(&errorcode);
			if(pTextRecord == NULL || errorcode != ERRORCODE_None)
			{
				ASSERT(0);
				continue;
			}
			else
			{
				DWORD nTotalChar = pTextRecord->NumberOfCharacters();
				CHARACTER_INDEX nCharIndex = 0;
				while(nTotalChar > 0)
				{
					long nNumChar = 0;
					CHARACTER* pText = pTextRecord->GetCharacters(nCharIndex, &nNumChar);
					nCharIndex += nNumChar;
					if((DWORD)nNumChar > nTotalChar)
						nNumChar = nTotalChar;

					csText.Empty();

				//Using WideCharToMultiByte does not convert chararacters greater then
				//0x7F properly.  So chars like trade mark and the curly quotes
				//end up getting converted to the default char.  I tried using 
				//the regular quotes as default which is the main problem but
				//there are occurances of the trademark symbol in there as well.
				//--MFW
#if 0
					//calculate the size of buffer needed for conversion
					int nResult = ::WideCharToMultiByte(CP_ACP, 0, pText, nNumChar,
						NULL, 0, NULL, NULL);

					//convert the string.
					char szDefault[] = "\"";
					nResult = ::WideCharToMultiByte(CP_ACP, 0, pText, nNumChar,
						csText.GetBuffer(nResult), nResult, szDefault, NULL);
				
					csText.ReleaseBuffer();

					text.AddText(csText);
#else
					for(long i = 0; i < nNumChar; i++)
					{
						csText += (char)(pText[i]);
					}
					text.AddText(csText);
#endif

					nTotalChar -= (DWORD)nNumChar;
				}
				text.AddText("\n");
				pTextRecord->release();
			}
		}
	}

	tagBody.Add(text);
	tagHeader.SetTitle(tagTitle);
	tagDocument.Init(tagHeader);
	tagDocument.Add(tagBody);

	CFile cHTMLTempDoc(csOutputFile, CFile::modeReadWrite | CFile::modeCreate);
	CString csOutputText;
	//write out the html file
	tagDocument.GetBody(csOutputText);
	cHTMLTempDoc.Write(csOutputText, csOutputText.GetLength());
	cHTMLTempDoc.Close();

	//add file to temp file mgr so it will be removed when app exits.
	m_cfmHelpTempFiles.Add(csOutputFile);

	pFrame->HelpWindow().DisplayTopic(csOutputFile, FALSE);
	//make sure the help window is visible
	if(bShowWindow && pFrame->HelpWindow().IsWindowVisible()  == FALSE)
	{
		pFrame->SendMessage(WM_COMMAND, ID_VIEW_HELPWINDOW);
	}
}

void CPmwView::OnUpdateGotoInstructionPage(CCmdUI* pCmdUI)
{
	if(GetConfiguration()->SupportsInstructionPage() == FALSE)
	{
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
		return;
	}

	CPmwDoc* pDoc = GetDocument();
	BOOL fEnable = TRUE;
	BOOL bChecked = 0;//Unchecked

	if(!GET_PMWAPP()->OurGetProfileInt("Debug", "EditInstructions", FALSE) && 
			pDoc->NumberOfInstructionPages() == 0)
	{
		fEnable = FALSE;
	}
	
	
	// If we are on a Instruction page, set it to "return to normal page".
	if (pDoc->EditingInstructionPage())
	{
		bChecked = 1;//checked
	}
	pCmdUI->Enable(fEnable);
	pCmdUI->SetCheck(bChecked);
}

void CPmwView::RefreshInstructionItems(void)
{
	CPmwDoc* pDoc = GetDocument();

	// Get the list of Instruction page objects for this page.
	ObjectList* pInstructionItems = pDoc->GetInstructionItems();

	// If we have objects, refresh all of them.
	if (pInstructionItems != NULL)
	{
		for (PMGPageObject* pObject = (PMGPageObject*)pInstructionItems->first_object();
			  pObject != NULL;
			  pObject = (PMGPageObject*)pObject->next_object())
		{
			pDoc->refresh_object(pObject);
		}
	}
}
// Changes grouped text objects extra flags
void
CPmwView::SetClearSelectedTextObjectsFlags(DWORD Set, DWORD Clear, BOOL &Changed, ObjectList* pList)
{
   for (PMGPageObjectPtr pObject = (PMGPageObjectPtr)pList->first_object();
         pObject != NULL;
         pObject = (PMGPageObjectPtr)pObject->next_object())
   {
      SetClearObjectFlags(Set, Clear, Changed, pObject);
   }
}
// Changes selected text objects extra flags
void
CPmwView::SetClearSelectedTextObjectsFlags(DWORD Set, DWORD Clear)
{
   BOOL  Changed = FALSE;
   CCmdChangeSelects* pCommand = NULL;
   //CPmwDoc  *pDoc = INHERITED::GetDocument();
	CPmwDoc  *pDoc = GetDocument();

   if(pDoc != NULL)
   {
      TRY
      {
         pCommand = new CCmdChangeSelects(IDCmd_FrameAttributes);
      }
      END_TRY
   
      if (pCommand != NULL)
      {
         if (!pCommand->Before(this))
         {
            delete pCommand;
            pCommand = NULL;
         }
      }

      PMGPageObject  *pObject = pDoc->selected_objects();
	   while (pObject != NULL)
	   {
		   SetClearObjectFlags(Set, Clear, Changed, pObject);

         pObject = pObject->next_selected_object();
      }
      
      if (Changed == TRUE)
      {
         pDoc->SetModifiedFlag();
         if (pCommand != NULL)
         {
            if (pCommand->After())
            {
               // Add the command for undoing later.
               pDoc->AddCommand(pCommand, FALSE);  // No execute!
            }
            else
               delete pCommand;
         }
      }
      else
      if (pCommand != NULL)
         delete pCommand;
   }   
}
// Changes the object's extra flags
void
CPmwView::SetClearObjectFlags(DWORD Set, DWORD Clear, BOOL &Changed, PMGPageObject *pObject)
{
   if (pObject->type() == OBJECT_TYPE_Group)
   {
      // Found a group - do it the "list" way
      ObjectList* pList = ((GroupObject*)pObject)->object_list();
      // NOTE: We may be calling the one who called us - don't be afraid
      SetClearSelectedTextObjectsFlags(Set, Clear, Changed, pList);
   }
   else
   if (pObject->type() == OBJECT_TYPE_Frame) 
   {
      DWORD Flags, FlagsOld;
      Flags = FlagsOld = ((CFrameObject*)pObject)->GetExtraFlags(); 
      FLAGCLR(Flags, Clear);
      FLAGSET(Flags, Set);
      if (Flags != FlagsOld)
      {
         Changed = TRUE;

         ((CFrameObject*)pObject)->SetAllExtraFlags(Flags); 
      }
   }
}
void
CPmwView::SetWarningRect(PBOX pb, DB_RECORD_NUMBER PageNumber)
{
   CPmwDoc *pDoc = GetDocument();
   if (pDoc != NULL)
   {
      BOOL  NewPage = FALSE;
      if (PageNumber > 0)
      {
         if (pDoc->CurrentPageRecord() != PageNumber)
         {
            NewPage = TRUE;
            pDoc->LoadPage(PageNumber);
         }
      }
      else
         pb.Empty();
      // Set (or clear) the warning rect
      if (m_pDesktop != NULL)
      {
         if (!NewPage)
         {
            PBOX  pbOld;
            m_pDesktop->GetWarningRect(pbOld);
            // If there was a warning rect, get rid of it
            if (!pbOld.IsEmpty())
               pDoc->add_refresh(&pbOld);
         }
         m_pDesktop->SetWarningRect(pb);
         if (!pb.IsEmpty())
            pDoc->add_refresh(&pb);
      }
   }
}

void 
CPmwView::OnPreviewWebSite() 
{
	PreviewWebSite(DesignCheckStateGet(), FALSE); 
}

static void 
ReportPublishError(ERRORCODE errorcode)
{
   CString  csResource, csMessage;

   switch(errorcode)
   {
      case ERRORCODE_None:
      break;
      
      case ERRORCODE_Fail:
      case ERRORCODE_Write:
      default:
      csResource.LoadString(IDS_PUBLISH_FAIL);
      ASSERT(!csResource.IsEmpty());
      AfxMessageBox(csResource);
#ifdef _DEBUG
      if(errorcode != ERRORCODE_Fail && errorcode != ERRORCODE_Write)
      {
         csResource.LoadString(IDS_PUBLISH_ERROR);
         ASSERT(!csResource.IsEmpty());
         csMessage.Format(csResource, errorcode);
         AfxMessageBox(csMessage);
      }
#endif
      break;
      
      case ERRORCODE_Abort:
         csResource.LoadString(IDS_PUBLISH_ABORT);
         ASSERT(!csResource.IsEmpty());
         AfxMessageBox(csResource);
         break;
   }
}

void 
CPmwView::PublishToFolder() 
{
   CChooseDirectoryDialog  dlgDir;
   CString                 csPublishPath;
   CString                 csDefaultPath;
   ERRORCODE               errorcode;

   CHTMLDocConverter::GetHTMLPathOnly(csPublishPath); 
	Util::RemoveBackslashFromPath(csPublishPath);
	dlgDir.m_csDirectory = csPublishPath;
	dlgDir.m_ofn.lpstrInitialDir = csPublishPath;
   if(dlgDir.GetDirectory(CChooseDirectoryDialog::OPTION_no_exists_check |
								  CChooseDirectoryDialog::OPTION_expand)
					&& !dlgDir.m_csDirectory.IsEmpty())
   {
      //IniFile.WriteString(SZ_PUBLISH_SECTION, SZ_PUBLISH_PATH_ENTRY, dlgDir.m_csDirectory);
      //CDesignCheckerDlg DesignCheckerDlg("Publish", this);	
      CDesignCheckerDlg DesignCheckerDlg("Continue", this);	
      CHTMLDocConverter::SetHTMLPathOnly(dlgDir.m_csDirectory);
		TRY
		{
			CTempFileManager   FileNames;
         Util::MakeDirectory(dlgDir.m_csDirectory);
			//errorcode = GetWebDocument()->ConvertToHTML(dlgDir.m_csDirectory, &FileNames, &DesignCheckerDlg);
			errorcode = GetDocument()->ConvertToHTML(dlgDir.m_csDirectory, &FileNames, &DesignCheckerDlg);
         if(errorcode != ERRORCODE_None)
         {
            ReportPublishError(errorcode);
         }
         else
         if (DesignCheckStateGet())
         {
            if (DesignCheckerDlg.DoModal() != IDOK)
               FileNames.RemoveAll();
            else
               FileNames.Empty();
         }
		}
		END_TRY
   }
}

void CPmwView::PublishToWeb() 
{
   char                    szDestURL[256], szSiteName[256];
   BOOL                    bChooseSite = TRUE;  // For wizard
   DWORD                   dwDestURLLen, dwSiteLen, dwResult;
   ERRORCODE               errorcode;
   CString                 csMessage, csResource/*, csHomePage*/;
   CString                 csFileName, csAppPath, csPublishPath;
   CPmwApp                 *pApp = GET_PMWAPP();
	IConnectionManager*		pConnectionManager = GetGlobalConnectionManager();

   HINSTANCE               hWebPubLibrary;
   FARPROC                 pProcWpPost = NULL;

   typedef DWORD (WINAPI * TDWpPostProc) (HWND hwnd,
               			 		DWORD cLocalPaths, LPSTR *lppszLocalPaths, 
               					LPDWORD lpcbSiteName, LPSTR lpszSiteName,
               					LPDWORD lpcbURL, LPSTR lpszURL, 
               					DWORD dwFlag);

   TDWpPostProc            pfnWpPost;
   // Load Microsoft Web Publishing Wizard
   hWebPubLibrary = ::LoadLibrary("WEBPOST.DLL");
   if(hWebPubLibrary)
   {
      CString  csAPIName = IdentifierToString(WpPostA);
	   pProcWpPost = GetProcAddress(hWebPubLibrary, csAPIName);
	   if (pProcWpPost == NULL)
	   {
         csResource.LoadString(IDS_WEBPUB_APIFIND_FAILED);
         ASSERT(!csResource.IsEmpty());
         GetConfiguration()->ReplaceText(csResource);
         AfxMessageBox(csResource);
         if(hWebPubLibrary)
            ::FreeLibrary(hWebPubLibrary);
         return;
	   }
      else
         pfnWpPost = (TDWpPostProc) pProcWpPost;
   }
   else
   {
      csResource.LoadString(IDS_WEBPUB_LOAD_FAILED);
      ASSERT(!csResource.IsEmpty());
      GetConfiguration()->ReplaceText(csResource);
      AfxMessageBox(csResource);
      return;
   }

   m_cfmPublishTempFiles.RemoveAll();     // Remove previous preview files (if any)
   // HTML Generation prep
   csPublishPath = GetDocument()->GetPathManager()->ExpandPath("[[U]]\\Tmp");
	Util::RemoveBackslashFromPath(csPublishPath);
   Util::MakeDirectory(csPublishPath);
   
   //CDesignCheckerDlg DesignCheckerDlg("Publish", this);
   CDesignCheckerDlg DesignCheckerDlg("Continue", this);
   // Generate HTML
   //errorcode = GetWebDocument()->ConvertToHTML(csPublishPath, &m_cfmPublishTempFiles, &DesignCheckerDlg);
	errorcode = GetDocument()->ConvertToHTML(csPublishPath, &m_cfmPublishTempFiles, &DesignCheckerDlg);
   if(errorcode != ERRORCODE_None)
   {
      if(hWebPubLibrary)
         ::FreeLibrary(hWebPubLibrary);
      ReportPublishError(errorcode);
      return;
   }
   
   if (DesignCheckStateGet())
   {
      if(DesignCheckerDlg.DoModal() != IDOK)
      {
         if(hWebPubLibrary)
            ::FreeLibrary(hWebPubLibrary);
         m_cfmPublishTempFiles.RemoveAll();     // Remove files (if any)
         return;
      }
   }
   // Try to establish connection
	if (pConnectionManager == NULL
		 || pConnectionManager->Connect() != CInternetConnectStatus::statusConnected)
   {
      if(hWebPubLibrary)
         ::FreeLibrary(hWebPubLibrary);
      return;
   }
   // Publish to web site using Microsoft Publishing Wizard
   const CStringList & cslFileNames = m_cfmPublishTempFiles.GetFileNames();
   if(cslFileNames.GetCount() > 0)
   {
      int      i;
      LPTSTR   *szaFileNames = new LPTSTR[cslFileNames.GetCount()];
      POSITION posFileList;
      // Fill in array of string pointers with pointers to CString data
      for(i=0, posFileList = cslFileNames.GetHeadPosition(); posFileList != NULL && i < cslFileNames.GetCount();i++)
      {
         szaFileNames[i] = (LPTSTR) (LPCSTR) cslFileNames.GetAt(posFileList);
         cslFileNames.GetNext(posFileList);
      }

      szDestURL[0] = 0;
      szSiteName[0] = 0;
      // Get Destination URL from PrintMaster INI File
      GetDestinationStrings();
      // If INI has a destination URL, get site name from INI
      if(!m_csDestURL.IsEmpty())
      {
         strcpy(szDestURL, (LPCSTR) m_csDestURL);
         // If INI has site name, prompt user if they want to publish to it
         if(!m_csSiteName.IsEmpty())
         {
            strcpy(szSiteName, m_csSiteName);
            csResource.LoadString(IDS_PUBLISH_TO_LAST_SITE);
            ASSERT(!csResource.IsEmpty());
            csMessage.Format(csResource, szDestURL, szSiteName);
            if(AfxMessageBox(csMessage, MB_YESNO) == IDYES)
               bChooseSite = FALSE;
         }
      }
      else
         szDestURL[0]=0;
      // If user wants to choose site, send empty strings to wizard
      if(bChooseSite)
      {
         szDestURL[0] = 0;
         szSiteName[0] = 0;
      }

      dwDestURLLen = dwSiteLen = 256;
      // Invoke Web Publishing Wizard
      dwResult = (*pfnWpPost)(
         AfxGetMainWnd()->GetSafeHwnd(),  // Windows to receive focus after post
         cslFileNames.GetCount(),         // number of string pointers in array
         szaFileNames,                    // pointer to array of filenames to be posted
         &dwSiteLen, 
         szSiteName, 
         &dwDestURLLen, 
         szDestURL, 
         0);
      // Cleanup
      delete [] szaFileNames;
      // If publishing succeeded, save dest. URL and Site Name to INI File
      if(dwResult == NO_ERROR)
         SaveDestinationStrings(szDestURL, szSiteName);
   }
   // Make sure PrintMaster has focus
   AfxGetMainWnd()->SetForegroundWindow();

   if(hWebPubLibrary)
      ::FreeLibrary(hWebPubLibrary);
}

void 
CPmwView::PreviewWebSite(BOOL DoCheck, BOOL JustCheck) 
{
   CString                 csPublishPath;
   CString                 csHomePage, csResource, csMessage;
   ERRORCODE               errorcode;

   csPublishPath = GetDocument()->GetPathManager()->ExpandPath("[[U]]\\Tmp");
	Util::RemoveBackslashFromPath(csPublishPath);
   Util::MakeDirectory(csPublishPath);
   m_cfmPublishTempFiles.RemoveAll();     // Remove previous preview files (if any)

   CDesignCheckerDlg *pDesignCheckerDlg;	
   if (DoCheck)
      pDesignCheckerDlg = new CDesignCheckerDlg("Continue", this, !JustCheck);
   else
      pDesignCheckerDlg = NULL;
   // Generate HTML
   //errorcode = GetWebDocument()->ConvertToHTML(csPublishPath, &m_cfmPublishTempFiles, pDesignCheckerDlg);
	errorcode = GetDocument()->ConvertToHTML(csPublishPath, &m_cfmPublishTempFiles, pDesignCheckerDlg);

   if(errorcode == ERRORCODE_None)
   {
      if ((pDesignCheckerDlg == NULL) || (pDesignCheckerDlg->DoModal() == IDOK))
      {
         CURLProcessor  urlProcess;
         Util::AppendBackslashToPath(csPublishPath);
         CHTMLDocConverter::GetHTMLHomeName(csHomePage); 
         csPublishPath += csHomePage;

         URLProcessorRetCode rc;
         if ((rc = urlProcess.OpenURL(csPublishPath)) != enURLErrorNone)
            DoURLErrorDialog(rc);
      }
   }
   else
   {
      ReportPublishError(errorcode);
   }

   delete pDesignCheckerDlg;
}
