// $Header: /PM8/App/VIEWCMD.CPP 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//
// View commands.
//
// $Log: /PM8/App/VIEWCMD.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 122   3/01/99 3:13p Lwilson
// Fixed bug in OnFillColorPalette() that was wiping out non-solid colors.
// 
// 121   2/26/99 3:47p Johno
// New CopyObjectList() in OnAddPage() now copies everything; objects,
// page properties, guide information 
// 
// 120   2/26/99 2:41p Johno
// OnAddPage() now uses new functions to copy page properties with other
// objects
// 
// 119   2/26/99 12:43p Lwilson
// Updated OnFillColorPalette() to set the fill style, instead of merge
// it.
// 
// 118   2/24/99 3:43p Blee
// Enabled OnPictureAttributes for component objects.  Note that it still
// doesn't do anything, because the Picture Attributes dialog doesn't know
// how to deal with component objects yet.  
// 
// 117   2/15/99 12:49p Psasse
// Moved ChangeRotation into the Document Class
// 
// 116   2/09/99 4:44p Rgrenfel
// Added support for the Made To Order toolbar popup menu for seals and
// timepieces.
// 
// 115   2/01/99 2:48p Blee
// Removed obsolete OnUpdateAddPage() (now in doc)
// 
// 114   1/29/99 2:32p Lwilson
// Fixed gradient determination in OnFillColor().
// 
// 113   1/29/99 11:40a Lwilson
// Updated OnModifyPalette to better handle RColor objects
// 
// 112   1/27/99 1:58p Lwilson
// Added RColor support to OnModifyPalette.
// 
// 111   1/25/99 4:35p Johno
// Moved OnPageProperties() here
// 
// 110   1/22/99 4:06p Johno
// Moved / added CmdUI handlers for HTNL menu items
// 
// 109   1/20/99 5:11p Blee
// Removed redundant code in OnTextBanner.
// Added comments
// 
// 108   1/20/99 5:01p Johno
// 
// 107   1/20/99 4:59p Blee
// Added OnUpdateTextBanner to gray out the "Edit Banner Text" command
// when the project type is not a banner.
// 
// 106   1/20/99 10:28a Mwilson
// moved toolbars into seperate DLL
// 
// 105   1/19/99 3:25p Lwilson
// Fixed comments
// 
// 104   1/19/99 2:58p Lwilson
// Print Shop color dialog integration.
// 
// 103   1/19/99 1:14p Jfleischhauer
// added command processing for the Photo Workshop popup menu items
// 
// 102   1/14/99 3:45p Jfleischhauer
// added CPPhotoWorkshopMenuPalette class and new command handlers for
// Photo Workshop button and popup
// 
// 101   1/13/99 1:22p Jfleischhauer
// removed (by commenting out) Photo Enhancer routines
// 
// 100   12/23/98 1:51p Psasse
// Code clean up
// 
// 99    12/15/98 3:37p Lwilson
// Initial support for PS component editing.
// 
// 98    11/25/98 6:03p Psasse
// RequestExistingAccount no longer takes a parameter
// 
// 97    11/21/98 10:45p Psasse
// 
// 96    11/18/98 10:38p Psasse
// DeleteAllCachedDirectories becomes DeleteCachedDirectories
// 
// 95    11/09/98 2:10p Dennis
// Added support for editing PNG graphics
// 
// 94    10/27/98 4:52p Johno
// Consolidated DoArtStore code into DoBrowserOrArtStore
// 
// 93    10/23/98 5:48p Johno
// Changed OnPictureImport() to use CMainFrame::DoArtStore()
// 
// 92    10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 91    10/07/98 5:46p Jayn
// Changes to the content manager.
// 
// 90    9/28/98 10:55a Johno
// Use pmwinet.h in place of afxinet.h
// 
// 89    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 88    9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 87    9/03/98 5:06p Jayn
// Added calendar pages now propagate the label and number font styles.
// 
// 86    8/28/98 3:41p Jayn
// Calendar fixes
// 
// 85    8/25/98 12:05p Mwilson
// changed some helpful hints
// 
// 84    8/24/98 5:38p Mwilson
// enabled more helpful hints
// 
// 83    8/03/98 12:19p Jayn
// Makes sure border plus is supported before invoking.
// 
// 82    7/25/98 1:51p Jayn
// Cleaned up OnCalendarWhen
// 
// 81    7/22/98 7:37p Johno
// Call pDoc->SetModifiedFlag() if needed in OnTextWrap()
// 
// 80    7/16/98 3:14p Mwilson
// removed include for savepict.h
// 
// 79    7/12/98 5:33p Jayn
// Changed how Serif DrawPlus tool bar button worked (add vs. edit)
// 
// 78    7/02/98 3:12p Rlovejoy
// Eliminated savepict dialog.
// 
// 77    7/01/98 5:22p Psasse
// filtering for Classic version
// 
// 76    6/24/98 4:48p Rlovejoy
// Set default fill type to be a solid fill (fixes bug).
// 
// 75    6/21/98 11:56a Cboggio
// Allowed for Serif Draw and YYWB to be removed for localization
// 
// 74    6/19/98 6:49p Cboggio
// Allow Cartoon-o-matic and Photoenhancer menu items to be removed for
// localization
// 
// 73    6/11/98 9:42a Dennis
// AG Photo Editor replacement
// 
// 72    6/05/98 7:56p Hforman
// add guides from menu
// 
// 71    6/04/98 10:10p Rlovejoy
// Added helpful hint for PhotoEnhancer.
// 
// 70    6/04/98 8:14p Rlovejoy
// Added helpful hints.
// 
// 69    6/02/98 12:09p Cboggio
// Code changes for Metric rulers (centimetres instead of inches) for
// localization
// 
// 68    5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 67    5/27/98 6:41p Hforman
// remove Welcome stuff
// 
// 66    5/27/98 9:33a Cboggio
// 
// 65    5/27/98 9:21a Cboggio
// Changed else to #else !
// 
// 64    5/26/98 6:16p Cboggio
// Use FormatMessage for coordinate strings for localization
// 
// 63    5/26/98 2:39p Jayn
// New table code.
// 
// 62    5/26/98 2:04p Dennis
// Cartoon-o-matic and Serif Draw appear on menu based upon Product Line
// 
// 61    5/22/98 10:48a Hforman
// added call to GetDocument() in OnAddSentiment()
// 
// 60    5/11/98 9:05p Psasse
// added online menu to postertype
// 
// 59    5/05/98 4:46p Rlovejoy
// Gradient button switch.
// 
// 58    4/30/98 9:21p Jayn
// Correct usage of m_pDoc.
// 
// 57    4/30/98 7:56p Jayn
// Changes for Border Plus and Add Picture From Disk.
// 
// 56    4/30/98 4:48p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 55    4/29/98 6:32p Rlovejoy
// Updated for new fill format.
// 
// 54    4/24/98 6:30p Fredf
// Uses MFC functions SaveBarState() and LoadBarState().
// 
// 53    4/15/98 8:59a Fredf
// Properly updates border creation toolbar button.
// 
// 52    4/14/98 4:41p Jayn
// Changes for the new table object.
// 
// 51    4/13/98 1:21p Hforman
// 
// 50    4/13/98 12:07p Hforman
// modified params to ImportItem()
// 
// 49    4/07/98 7:18p Fredf
// Border and drawing object toolbar buttons now mean "Add" instead of
// "Edit".
// 
// 48    4/07/98 8:45a Jayn
// Changes for DPP files.
// 
// 47    3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
// 46    3/21/98 1:18p Fredf
// New menus and toolbar names.
// 
// 45    3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 44    3/18/98 6:31p Fredf
// Added color button top to of popup menus.
// 
// 43    3/17/98 7:09p Fredf
// Added color button to shadow popup.
// 
// 42    3/17/98 2:43p Fredf
// New workspace toolbars.
// 
// 41    3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 40    3/10/98 5:24p Jayn
// Master page changes
// 
// 39    3/02/98 1:52p Jayn
// Fixed handle glitch problem.
// 
// 38    2/25/98 2:27p Hforman
// add EditBorder functions
// 
// 37    2/25/98 10:40a Jstoner
// table functionality updates...merge/split cells
// row/col selection done
// 
// 36    2/11/98 11:36a Jstoner
// changes for table object
// 
// 35    2/05/98 12:00p Hforman
// fixed OnAddBorder()
// 
// 34    1/27/98 5:51p Hforman
// removed extern Clipboard near clipboard
// 
// 33    1/21/98 10:14a Fredf
// Year 2000 support.
// 
// 32    1/21/98 9:18a Jstoner
// cell handling stuff
// 
// 31    1/20/98 3:44p Fredf
// Got rid of GetNowTime(), use GetLocalTime(SYSTEMTIME) instead.
// 
// 30    1/13/98 11:06a Hforman
// implement OnUpdateAddBorder()
// 
// 29    1/05/98 4:56p Hforman
// Border Object code
// 
// 28    12/24/97 1:10p Hforman
// border functionality
// 
// 27    12/24/97 10:22a Hforman
// add border commands
// 
// 26    12/19/97 5:17p Hforman
// add Helpful Hints
// 
// 25    12/18/97 10:04a Jstoner
// fixed crash in auto format menu item
// 
// 24    12/17/97 10:47a Jstoner
// table project incremental update
// 
// 23    12/16/97 12:14p Jstoner
// added auto format handler
// 
// 22    12/16/97 9:19a Jstoner
// added table support
// 
// 21    12/12/97 2:55p Johno
// Moved resize stuff from CPmwView::OnPhotoEnhancer
// to CPmwDoc::DoReplacePicture.
// 
// 20    12/11/97 4:33p Johno
// Check / correct  image dimension changes in OnPhotoEnhancer
// it SEEMS to work OK...
// 
// 19    12/11/97 9:43a Jstoner
// 
// 18    12/11/97 9:20a Jstoner
// vector to bitmap conversion for photo enhancer
// 
// 17    11/26/97 10:45a Jstoner
// minor fixes to desktop
// 
// 16    11/25/97 8:54a Jstoner
// added new calendar type - cal with pic on same page
// 
// 15    11/24/97 8:04a Jstoner
// changed object center on page function to handle top/bottom and
// left/right page calendars
// 
// 14    11/21/97 4:50p Hforman
// 1) add code to update position indicators when moving guides
// 2) add TextBox Outlines show/hide functions
// 
// 13    11/20/97 2:03p Jstoner
// added progress dialogs to time consuming functions
// added temporary dialogs to show where "helpful hints" will go
// 
// 12    11/20/97 9:01a Jstoner
// added progress dialogs to lengthy calendar operations
// 
// 11    11/19/97 2:04p Jstoner
// multi-calendar mods to calendar day cell functions
// double page printing mods for calendar projects
// add calendar page tweaks
// 
// 10    11/18/97 11:10a Jstoner
// enhanced calendar menu items to deal with yearly and/or multipage
// calendar projects
// 
// 9     11/14/97 6:53p Hforman
// tweaked SnapPosition()
// 
// 8     11/14/97 1:12p Jstoner
// 
// 7     11/14/97 11:43a Hforman
// added new Guide functions; modified SnapPosition() to snap to guides
// 
// 6     11/11/97 2:24p Jstoner
// 
// 5     11/10/97 3:50p Jstoner
// fixed some problems regarding adding yearly calendars
// 
// 4     11/07/97 2:22p Jstoner
// year calendar updates...good stopping/saving point
// 
// 3     11/05/97 2:31p Jstoner
// can now print multi-page, single-sided calendar projects
// todo: fix print preview and double-sided printing
// 
// 2     10/31/97 3:55p Jstoner
// multi page calendar fixes
// 
//    Rev 1.2   21 Oct 1997 15:15:12   JOE
// multi-page calendar mods
// 
//    Rev 1.1   17 Oct 1997 17:27:02   JOE
// added command handler for OnAddCalendarPage
// 
//    Rev 1.0   14 Aug 1997 15:26:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:56   Fred
// Initial revision.
// 
//    Rev 1.85   05 Aug 1997 11:46:58   Jay
// Frees the edit background after a "more" position change.
// 
//    Rev 1.84   30 Jul 1997 09:35:06   Jay
// Frees the edit bitmap after a rotate.
// 
//    Rev 1.83   28 Jul 1997 15:16:26   Jay
// Reflows frames after add page with copy.
// 
//    Rev 1.82   20 Jul 1997 14:35:36   Fred
// Improvement on layer menu fix
// 
//    Rev 1.81   20 Jul 1997 12:42:58   Jay
// Now marks frames and sets document modified
// 
//    Rev 1.80   20 Jul 1997 11:21:08   Fred
// Layer menu enabled when editing text.
// 
//    Rev 1.79   18 Jul 1997 16:01:30   Fred
// Sentiment browser disable for CL and BP
// 
//    Rev 1.78   14 Jul 1997 13:11:42   Jay
// BeginModalState totally disables window in 16-bit
// 
//    Rev 1.77   10 Jul 1997 17:02:30   Fred
// Makes sure popup palettes are on screen
// 
//    Rev 1.76   08 Jul 1997 10:12:34   johno
// Found another string for the resource file
// 
//    Rev 1.75   02 Jul 1997 12:58:40   Fred
// Fixed problem with object/text popup duality
// 
//    Rev 1.74   27 Jun 1997 10:16:12   johno
// Moved photo enhancer strings to respource file
// 
//    Rev 1.73   17 Jun 1997 12:42:36   Jay
// Import; OpenStreams()
// 
//    Rev 1.72   16 Jun 1997 15:04:22   Jay
// Acquire now works
// 
//    Rev 1.71   09 Jun 1997 17:30:54   Jay
// Text improvements
// 
//    Rev 1.70   06 Jun 1997 15:39:26   doberthur
// Sentiment help.
// 
//    Rev 1.69   05 Jun 1997 08:52:46   Jay
// Text "mode" changes.
// Don't show cursor position, etc. if not active.
// 
//    Rev 1.68   30 May 1997 17:46:40   Fred
// Calls Util::WaitForWindow()
// 
//    Rev 1.67   29 May 1997 15:09:36   Fred
//  
// 
//    Rev 1.66   29 May 1997 09:39:14   johno
// Changed color passed to art browser for welcome dialog
// 
//    Rev 1.65   28 May 1997 19:38:42   dennisp
// Added call to sentiment browser
// 
//    Rev 1.64   27 May 1997 17:03:04   johno
// Added calls to CMainfrm::DoWelcome
// 
//    Rev 1.63   27 May 1997 11:09:22   Fred
// Clips position of popup window.
// 
//    Rev 1.62   23 May 1997 17:08:20   Fred
// New effects
// 
//    Rev 1.61   15 May 1997 16:52:52   Fred
// New text effects
// 
//    Rev 1.60   23 Apr 1997 17:15:36   Jay
// DoSelect for project and picture browsers.
// 
//    Rev 1.59   22 Apr 1997 13:17:50   Fred
// Start of project and art browsers
// 
//    Rev 1.58   16 Apr 1997 12:12:30   Jay
// Fixes for text undo
// 
//    Rev 1.57   14 Apr 1997 15:42:44   Jay
// Text box editing changes
// 
//    Rev 1.56   11 Apr 1997 14:56:02   Jay
// The PhotoEnhancer title bar could change. Implemented code to check for
// title prefix.
// 
//    Rev 1.55   11 Apr 1997 11:23:42   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.54   09 Apr 1997 17:14:12   Jay
// PhotoEnhancer
// 
//    Rev 1.53   03 Apr 1997 14:31:28   Jay
// Resource cleanup
// 
//    Rev 1.52   31 Mar 1997 17:12:36   Jay
// Start of NFX code
// 
//    Rev 1.51   28 Mar 1997 17:11:10   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.50   25 Mar 1997 12:40:22   Jay
// Partial interruptable preview.
// 
//    Rev 1.49   17 Mar 1997 16:09:12   Jay
// Got rid of a trace.
// 
//    Rev 1.48   17 Mar 1997 15:12:42   Jay
// Text shape undo command.
// 
//    Rev 1.47   17 Mar 1997 14:48:18   Jay
// Warp shape palette and support.
// 
//    Rev 1.46   17 Mar 1997 09:40:14   Jay
// Popup warp field palette
// 
//    Rev 1.45   13 Mar 1997 13:17:16   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.44   12 Mar 1997 15:52:20   Jay
// Rotation, etc.
// 
//    Rev 1.43   11 Mar 1997 17:43:20   Jay
// Rotate stuff
// 
//    Rev 1.42   11 Mar 1997 16:13:58   Jay
// Extensions to the workspace
// 
//    Rev 1.41   04 Mar 1997 17:04:00   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.40   21 Feb 1997 15:14:36   Jay
// New mode handling; crop mode support
// 
//    Rev 1.39   19 Feb 1997 16:53:24   Jay
// Snapping
// 
//    Rev 1.38   11 Feb 1997 14:54:44   Jay
// Free draw tool
// 
//    Rev 1.37   07 Feb 1997 14:53:06   Jay
// Rulers on and off. change_view() fixes.
// 
//    Rev 1.36   06 Feb 1997 09:12:58   Jay
// Status bar stuff.
// 
//    Rev 1.35   03 Feb 1997 17:26:52   johno
// Changes for find / replace
// 
//    Rev 1.34   29 Jan 1997 17:24:40   Jay
// Next and previous object support
// 
//    Rev 1.33   29 Jan 1997 12:48:30   Jay
// Now does deselect_all() when a tool is chosen.
// 
//    Rev 1.32   28 Jan 1997 14:10:34   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.31   27 Jan 1997 13:55:22   Jay
// Default object attributes and size.
// 
//    Rev 1.30   24 Jan 1997 17:05:46   Jay
// Shadow popup.
// 
//    Rev 1.29   24 Jan 1997 12:39:20   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.28   23 Jan 1997 11:05:44   Jay
// New popup palettes
// 
//    Rev 1.27   17 Jan 1997 12:14:58   Jay
// New creation tools.
// 
//    Rev 1.26   15 Jan 1997 10:49:34   Jay
// New toolbars
// 
//    Rev 1.25   08 Jan 1997 15:22:20   Fred
// More text box changes
// 
//    Rev 1.24   09 Sep 1996 16:49:28   Jay
// Spell-checking not available in BP and CL.
// 
//    Rev 1.23   27 Aug 1996 13:23:16   Jay
// Calls SetFocus() at the right time to allow screen update during text edit.
// 
//    Rev 1.22   02 Aug 1996 10:47:10   Jay
// More spelling fixes.
// 
//    Rev 1.21   01 Aug 1996 17:46:04   Jay
// Better handling panels, pages, and undo.
// 
//    Rev 1.20   31 Jul 1996 18:22:04   Jay
// Spelling, etc.
// 
//    Rev 1.19   29 Jul 1996 12:19:32   Jay
// Does not load layout files.
// 
//    Rev 1.18   29 Jul 1996 11:22:38   Jay
// Full page
// 
//    Rev 1.17   29 Jul 1996 08:37:14   Jay
//  
// 
//    Rev 1.16   28 Jul 1996 15:09:02   Jay
//  
// 
//    Rev 1.15   26 Jul 1996 18:16:18   Jay
// Passes view to add/delete page commands
// 
//    Rev 1.14   26 Jul 1996 11:39:26   Jay
// Page stuff
// 
//    Rev 1.13   25 Jul 1996 19:38:48   Jay
//  
// 
//    Rev 1.12   25 Jul 1996 18:21:32   Fred
//  
// 
//    Rev 1.11   23 Jul 1996 10:41:36   Jay
// Added newsletters and brochures; removed backgrounds
// 
//    Rev 1.10   18 Jul 1996 13:48:46   Jay
// Now normalizes position data relative to the "world".
// 
//    Rev 1.9   18 Jul 1996 11:12:08   Jay
// Text Wrap
// 
//    Rev 1.8   17 Jul 1996 18:08:12   Jay
// Can't edit a frame with no lines.
// 
//    Rev 1.7   15 Jul 1996 17:46:42   Jay
//  
// 
//    Rev 1.6   15 Jul 1996 11:35:16   Jay
//  
// 
//    Rev 1.5   28 Jun 1996 08:21:30   Jay
// ReplaceWord for calendar titles
// 
//    Rev 1.4   26 Jun 1996 14:23:18   Jay
//  
// 
//    Rev 1.3   26 Jun 1996 09:07:34   Jay
//  
// 
//    Rev 1.2   13 Jun 1996 15:53:56   Jay
//  
// 
//    Rev 1.1   05 Jun 1996 08:22:20   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:35:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:30   FRED
// Initial revision.
// 
//    Rev 2.22   22 Sep 1995 15:43:26   JAY
// Fix in WIN32 for misdirected tool bar updates.
// 
//    Rev 2.21   15 Sep 1995 14:05:08   JAY
// More 32-bit changes.
// 
//    Rev 2.20   01 Sep 1995 14:25:06   JAY
// Added undo for edit title text.
// 
//    Rev 2.19   22 Aug 1995 10:49:48   JAY
// Moved FormatBar setting code to mainfrm.
// 
//    Rev 2.18   18 Aug 1995 11:46:20   FRED
// Line attributes
// 
//    Rev 2.17   17 Aug 1995 07:21:34   FRED
//  
// 
//    Rev 2.16   16 Aug 1995 10:55:00   JAY
// New position dialog data format.
// 
//    Rev 2.15   14 Aug 1995 11:45:10   JAY
// Slight changes to object position.
// 
//    Rev 2.14   14 Aug 1995 09:14:28   FRED
// Envelope size under preview
// 
//    Rev 2.13   10 Aug 1995 07:05:40   JAY
// No longer brings up the position sidebar.
// 
//    Rev 2.12   09 Aug 1995 13:12:24   JAY
// Misc changes.
// 
//    Rev 2.11   08 Aug 1995 13:24:56   JAY
// Right mouse button menu and side bar fixes.
// 
//    Rev 2.10   01 Aug 1995 13:01:56   JAY
// New "select xxx" dialog stuff.
// 
//    Rev 2.9   17 May 1995 15:42:24   JAY
7//  
// 
//    Rev 2.8   10 May 1995 13:18:46   JAY
// Support for lines, ellipses, and rectangles.
// 
//    Rev 2.7   19 Apr 1995 12:32:20   JAY
//  
// 
//    Rev 2.6   29 Mar 1995 09:16:58   JAY
// Lots of changes
// 
//    Rev 2.5   15 Mar 1995 10:48:18   JAY
// Support for new text editing
// 
//    Rev 2.4   09 Mar 1995 13:50:04   JAY
// Blinking text cursor.
// 
//    Rev 2.3   09 Mar 1995 08:37:58   JAY
// More undo.
// 
//    Rev 2.2   15 Feb 1995 17:16:24   JAY
// Undo
// 
//    Rev 2.1   08 Feb 1995 13:34:04   JAY
// Reverted. New series.
// 
//    Rev 1.2   04 Aug 1994 14:31:06   JAY
// Code and support for fixing picture panel missing name bug.
// 
//    Rev 1.1   20 Jul 1994 16:48:00   JAY
// Disables 'Layer' submenu if a text frame is being edited.
// 
//    Rev 1.0   23 May 1994 11:13:26   JAY
// Initial revision.
*/

#include "stdafx.h"
ASSERTNAME

#include "pmw.h"
#include "mainfrm.h"

#include "pmwdoc.h"
#include "pmwview.h"
#include "pmwcfg.h"
#include "cntritem.h"

#include "posdlg.h"
#include "calstyle.h"
#include "caldated.h"
#include "textflow.h"
#include "utils.h"
#include "util.h"
#include "compfn.h"

#include "textrec.h"
#include "grafobj.h"
#include "compobj.h"
#include "frameobj.h"
#include "framerec.h"
#include "pagerec.h"
#include "lineobj.h"
#include "calobj.h"
#include "dateobj.h"
#include "backobj.h"
#include "brdrobj.h"
#include "oleobj.h"
#include "tblobj.h"
#include "dlgmgr.h"
#include "ctwain.h"
#include "toolbar.h"

#include "commands.h"
#include "tcommand.h"               // text commands
#include "ccommand.h"               // calendar commands

#include "drawdlg.h"       // for drawing object attributes dialog
#include "brdrdlg.h"
#include "paradlg.h"       // for text wrap dialog (for now)
#include "pagedlgs.h"
#include "spellchk.h"
#include "findtext.h"

#include "popup.h"
#include "ruler.h"
#include "rotdlg.h"
#include "clip.h"
#include "nfx.h"
#include "bmp.h"

#include "browser.h"
#include "addcalpg.h"
#include "caldoc.h"
#include "calprogd.h"
#include <math.h>
#include "TabbedEditImage.h"
#include "webdlg.h"     // For CWebSitePropertiesDialog

// Framework Support
#include "ComponentTypes.h"
#include "WinColorDlg.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif


#define MADE_TO_ORDER_SEAL 0
#define MADE_TO_ORDER_TIMEPIECE 1

extern short GetWeeksInMonth(short year, short month); // located in caldoc.cpp
extern void IncrementCalendarDate(CALENDAR_INFO * ciCalInfo, BOOL bWhere);

void CPmwView::GetCursorPagePos(PPNT* p)
{
   POINT Pos;
   GetCursorPos(&Pos);
   ScreenToClient(&Pos);
   rc.screen_to_ppnt(Pos, p);
   SnapPosition(p);
}

void CPmwView::OnInsertObject()
{
   set_arrow_tool();

   // Invoke the standard Insert Object dialog box to obtain information
   //  for new CPmwCntrItem object.
   COleInsertDialog dlg;
   if (dlg.DoModal() != IDOK)
      return;

   BeginWaitCursor();

   CPmwCntrItem* pItem = NULL;

   TRY
   {
      // Create new item connected to this document.
      CPmwDoc* pDoc = GetDocument();
      ASSERT_VALID(pDoc);
      pItem = new CPmwCntrItem(pDoc);
      ASSERT_VALID(pItem);

      // Initialize the item from the dialog data.
      if (!dlg.CreateItem(pItem))
         AfxThrowMemoryException();  // any exception will do
      ASSERT_VALID(pItem);

      deselect_all();

      if (pDoc->FinishOleObjectCreate(pItem) != ERRORCODE_None)
      {
         AfxThrowMemoryException();  // any exception will do
      }

      pDoc->AddCreateCommand(IDCmd_AddOleObject);

      // If item created from class list (not from file) then launch
      //  the server to edit the item.

      if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
         pItem->DoVerb(OLEIVERB_SHOW, this);
   }
   CATCH(CException, e)
   {
      if (pItem != NULL)
      {
         ASSERT_VALID(pItem);
         pItem->Delete();
      }

      AfxMessageBox(IDP_FAILED_TO_CREATE);
   }
   END_CATCH

   EndWaitCursor();
}

void CPmwView::OnEditDelete()
{
   signal_internal_event(EVENT_EDIT_DELETE);
}

void CPmwView::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
   CPmwDoc* pDoc = GetDocument();
   pCmdUI->Enable(pDoc->selected_objects() != NULL
                  && !pDoc->IsGroupedFrame(pDoc->selected_objects()));
}

void CPmwView::OnEditGroup()
{
   GetDocument()->group_selects();  
}

void CPmwView::OnEditUngroup()
{
   GetDocument()->ungroup_selects();   
}

/*
// Update the "ungroup" menu command.
*/

void CPmwView::OnUpdateEditUngroup(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->can_ungroup());
}

/*
// Update the "group" menu command.
*/

void CPmwView::OnUpdateEditGroup(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->can_group());
}

void CPmwView::OnXflip()
{
   GetDocument()->xflip_selects();
}

void CPmwView::OnYflip()
{
   GetDocument()->yflip_selects();
}

void CPmwView::OnEditDuplicate()
{
   GetDocument()->duplicate_selects();
}

void CPmwView::OnPictureImport()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->DoBrowserOrArtStore(CMainFrame::Art, GetDocument());
}

void CPmwView::OnAddSentiment()
{
	// Do we support this?
	if (!GetConfiguration()->SupportsSentimentGallery())
	{
		return;
	}

	CSentimentBrowserDialog SentimentBrowser(NULL);
	SentimentBrowser.m_pDoc = GetDocument();
	SentimentBrowser.DoModal();
}
/*
// Switch to editing a frame.
*/

void CPmwView::edit_frame(CFrameObject* pObject, PPNT_PTR p)
{
   CPmwDoc *pDoc = GetDocument();

   // Make sure the frame can be edited.
   // We can edit it if text has flowed to this position.
   // Get the first character of this frame.
   CHARACTER_INDEX lFirstCharacter = -1;
   CFrameRecord* pFrame = pObject->LockFrameRecord();
   if (pFrame != NULL)
   {
      if (pFrame->NumberOfLines() == 0
            || (lFirstCharacter = pFrame->FirstCharacter()) == -1)
      {
         // Inform the user that there is no text.
         pDoc->select_object(pObject);
//         AfxMessageBox(IDS_ErrEmptyFrame);
         pFrame->release();
         return;
      }
      pFrame->release();
   }

   // If we have a first character, then edit the frame.
   if (lFirstCharacter != -1)
   {
	/*
	// Compute the panel based on the frame being edited.
	*/

		// Begin actually editing the frame.
		BeginFrameEdit(pObject, lFirstCharacter);

		SetFocus();

		// Make sure the cursor is set correctly.
		if (p != NULL)
		{
			m_Txp.MoveToPoint(*p, CTxp::END_NoSelection, TRUE);
		}

		// Show where we are.
		m_Txp.On();
		reframe_on_txp();
   }
}

void CPmwView::BeginFrameEdit(CFrameObject* pObject, CHARACTER_INDEX lCharacter)
{
   CPmwDoc* pDoc = GetDocument();

	if (pObject->get_panel() == OBJECT_IN_CELL_PANEL)
	{
		PMGPageObject* pContainer;
		if ((pContainer = pObject->get_container()) != NULL
				&& pContainer->type() == OBJECT_TYPE_Cell
				&& (pContainer = pContainer->get_container()) != NULL
				&& pContainer->type() == OBJECT_TYPE_Table)
		{
			deselect_all();
			pDoc->select_object(pContainer);
		}
		else
		{
			// Some cell frame containment is not set correctly.
			ASSERT(FALSE);
		}
	}
	else
	{
		deselect_all();
		pDoc->select_object(pObject);
	}

   pObject->SetEditing(TRUE);
   set_text_edit_tool();

   m_Txp.Init(pObject, lCharacter, TxpToggleFunction, this);
   rc.register_toggle(redisp_toggle_function, this);

	// Make sure we have the focus.
   SetFocus();

	// Initial state.
   text_cursor_pbox.x0 = -1;
   cursor_visible = TRUE;
   cursor_on = FALSE;
}

/*
// Choose the color for a picture.
*/

SHORT CPmwView::choose_picture_color(GraphicObjectPtr object, WORD wSoundID)
{
   COLOR object_color = object->get_bcolor(), new_color = object_color;
   CPmwDoc *pDoc = GetDocument();
   SHORT ret;

   if ((ret = pDoc->choose_color(new_color)) == IDOK)
   {
      if (new_color != object_color)
      {
      /*
      // Set up the undo/redo command.
      */
         CCmdChangeObject* pCommand = new CCmdChangeObject(IDCmd_PictureColor);

         if (!pCommand->Before(this, object))
         {
            delete pCommand;
            pCommand = NULL;
         }

      /* Set the new color. */

         object->set_bcolor(new_color);
         object->changed_image();

      /* Make us redraw. */

         pDoc->refresh_object(object);
         pDoc->SetModifiedFlag();

      /*
      // Finish up the undo/redo command.
      */

         if (pCommand != NULL)
         {
            if (pCommand->After())
            {
               pDoc->AddCommand(pCommand, FALSE);
            }
            else
            {
               delete pCommand;
            }
         }
      }
   }
   return ret;
}

/*
// Do we have a calendar to work on?
// This is used by a number of menu handlers.
*/

void CPmwView::OnHaveCalendar(CCmdUI* pCmdUI)
{
/* We need to know if we have a calendar. */
	if (pCmdUI->m_nID == ID_CALENDAR_TITLE)
	{
		CalendarObject * pObject = ((CCalendarDoc*)GetDocument())->get_current_calendar();
		if (pObject != NULL) // make sure there's a calendar on the page!
			pCmdUI->Enable(pObject->get_calendar().get_type() != YEARLY);
		else
			pCmdUI->Enable(FALSE);

	}
	else
   pCmdUI->Enable(GetDocument()->get_current_calendar() != NULL);
}

void CPmwView::OnCalendarTitle(VOID)
{
   CalendarObjectPtr calobj;

/* See if we have any calendar objects to deal with. */

   if ((calobj = GetDocument()->get_current_calendar(TRUE)) != NULL)
   {
      CFrameObject* frame;

      if ((frame = calobj->get_title_frame()) != NULL)
      {
         edit_frame(frame);
      }
   }
}

/*
// Find a word in the text record starting at a given point.
*/

CHARACTER_INDEX FindWord(CTextRecord* pText, LPCSTR pWord, CHARACTER_INDEX lCharacter)
{
   CHARACTER_COUNT lCharacters = pText->NumberOfCharacters();

   CHARACTER* cp = NULL;
   CHARACTER_COUNT lThisCharacters = 0;

   CHARACTER_INDEX lStart = -1;

   // Convert to upper case.
   CString csWord = pWord;
   csWord.MakeUpper();
   pWord = csWord;

   LPCSTR p = pWord;

   // The algorithm here is constructed such that you can march through the
   // text without having to backtrack. It is not highly efficient, but it
   // works.

   while (lCharacter < lCharacters)
   {
      // Prime the character pointer if necessary.
      if (lThisCharacters == 0)
      {
         cp = pText->GetCharacters(lCharacter, &lThisCharacters);
      }

      CHARACTER c = *cp++;
      lThisCharacters--;
      char ch = (char)toupper(c);

      if ((c & 0xff00) == 0 && ch == *p)
      {
         // We found the character!
         if (lStart == -1)
         {
            lStart = lCharacter;
         }

         if (*++p == '\0')
         {
            // We have found it!
            return lStart;
         }
      }
      else
      {
         // We did not find the character!
         lStart = -1;
         if (p != pWord)
         {
         /* The character does not match. */
         /* Calculate the new search index. */

            LPCSTR pFound;

            pFound = pWord-1;    /* '-1' since we add 1 right away. */

            for (;;)
            {
               if ((pFound = strchr(pFound+1, ch)) == NULL)
               {
               /* No more matches. Search from the start of our string. */
                  p = pWord;
                  break;
               }
            /*
            // The character exists here in the string. If the text from here
            // back to the start of the string is the same as from there back,
            // start here instead.
            // Otherwise, start from the front.
            */

               if (pFound >= p)
               {
               /* Can't do it from here... */
                  p = pWord;
                  break;      /* out of for(;;) */
               }

               LPCSTR p1 = pFound;           /* Set up for the loop. */
               LPCSTR p2 = p;

               while (p1 != pWord)
               {
                  p2--;
                  p1--;
                  if (*p2 != *p1)
                  {
                  /* The text does not match. */
                     break;      /* out of while */
                  }
               }

               if (p1 == pWord)
               {
               /* We searched successfully. Continue from here. */
                  p = pFound+1;
                  break;      /* out of for(;;) */
               }
            }
         }
      }
      lCharacter++;
   }
   return -1;
}

/*
// Replace text on a whole word basis.
*/

PUBLIC void ReplaceWord(CFrameObject* pObject, char *pOldWord, char *pNewWord, BOOL fOverwrite)
{
   // Lock the frame record.
   CFrameRecord* pFrame = pObject->LockFrameRecord();
   CTextRecord* pText = NULL;

   if (pFrame != NULL)
   {
      pText = pFrame->LockTextRecord();
      pFrame->release();
   }

   if (pText == NULL)
   {
      // Can not do anything with nothing.
      return;
   }

/* Compute the lengths of the old and new words. */

   int nOldSize = strlen(pOldWord);
   int nNewSize = strlen(pNewWord);

   if (nOldSize != nNewSize)
   {
   /* Can't just overwrite no matter what they say! */
      fOverwrite = FALSE;
   }

   if (nOldSize != 0)
   {
      // Get a Txp for insertion and deletion if necessary.
      CTxp Txp(pObject->get_database());

      // Search for the word in the text.
      // Note that we don't do anything fancy here. This is just used for
      // calendar title text. If you want a general, mucho-text text replacer,
      // you'll need a better algorithm.

      CHARACTER_INDEX lFound = 0;
      BOOL fChanged = FALSE;

      for (;;)
      {
         lFound = FindWord(pText, pOldWord, lFound);
         if (lFound == -1)
         {
            break;
         }

         // We found the text.
         if (fOverwrite)
         {
            // We will simply stuff the new text characters over the old
            // ones. Style changes within the word will be preserved.

            ASSERT(nOldSize == nNewSize);

            for (LPCSTR p = pNewWord; *p != '\0'; )
            {
               *(pText->GetCharacter(lFound++)) = (CHARACTER)(BYTE)*p++;
            }
         }
         else
         {
            // Insert the new word, then delete the old one.
            // The new word will have the style of the first character of
            // the old word.

            Txp.Init(pObject, lFound);
            Txp.CalcStyle();

            for (LPCSTR p = pNewWord; *p != '\0';)
            {
               CHARACTER c = (CHARACTER)(BYTE)*p++;
               Txp.InsertText(&c, 1);
            }

            ASSERT(nOldSize != 0);
            Txp.DeleteText(nOldSize);
         }
         lFound += nNewSize;
         fChanged = TRUE;
      }
      if (fChanged)
      {
         CTextFlow TextFlow(pObject, 0, -1);
         TextFlow.FlowText();
      }
   }
   pText->release();
}

void CPmwView::OnCalendarWhen()
{
	CalendarObjectPtr pObject;
	CPmwDoc *pDoc = GetDocument();

	// In case we are editing text...
	set_arrow_tool();

	// load first page of project, since we start the date there
	DWORD dwCurrentPageIndex = pDoc->CurrentPageIndex();

	pDoc->LoadPage(pDoc->GetPage(0));
	pObject = pDoc->get_current_calendar(TRUE);
	pDoc->LoadPage(pDoc->GetPage(dwCurrentPageIndex));

	if (pObject != NULL)  
	{
		CALENDAR_INFO old_info = pObject->get_calendar().get_info();

		if (!pDoc->object_selected(pObject))
		{
			pDoc->deselect_all();
		}

		// Let's edit the calendar when. 
      CALENDAR_INFO info = old_info;         // Initial state.

      CCalendarDateDlg cdlg(info, FALSE, WIZBUT_OKCAN);

      if (cdlg.DoModal() == IDOK)
      {
			// Extract the info out. 
         cdlg.get_exit_info(info);
         set_arrow_tool();

			// set up undo/redo command for this operation 
			CCmdCalendarWhen* pCommand = new CCmdCalendarWhen;
			if (pCommand->Before(pDoc, pObject))
			{
				// Set the new calendar date.
				((CCalendarDoc*)pDoc)->ChangeCalendarDate(info);

				// complete undo handler
				pDoc->SetModifiedFlag();

				GET_PMWAPP()->ShowHelpfulHint("CalDateChange");
				if (pCommand->After())
				{
					// Add the command.
					pDoc->AddCommand(pCommand, FALSE);
					// NULL it out so it won't get deleted below.
					pCommand = NULL;
				}
			}

			// Delete any command left over at this point.
			delete pCommand;
		}
	}

}
void CPmwView::OnCalendarPicture()
{
   GetDocument()->choose_calendar_picture();
}

void CPmwView::OnUpdateCalendarPicture(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->get_date_selected() != -1);
}

void CPmwView::OnCalendarText()
{
	CPmwDoc *pDoc = GetDocument();
    CalendarObjectPtr calobj = NULL;
	PMGPageObjectPtr obj = NULL;
	if ((obj = pDoc->selected_objects()) != NULL
		&& obj->type() == OBJECT_TYPE_CalendarDate)
	{
		calobj = (CalendarObjectPtr)obj->get_container();
	    CFrameObject* pFrame = calobj->get_date_text(-1, TRUE);
	    edit_frame(pFrame);
   }
}

void CPmwView::OnUpdateCalendarText(CCmdUI* pCmdUI)
{	 
   pCmdUI->Enable(GetDocument()->get_date_selected() != -1); 
}

void CPmwView::OnCalendarClear()
{
/* Make sure we aren't editing text. */

   if (m_Txp.Object() != NULL)
   {
      set_arrow_tool();
   }

   GetDocument()->delete_selects(TRUE, IDCmd_ClearDate);
}

void CPmwView::OnUpdateCalendarClear(CCmdUI* pCmdUI)
{
   BOOL can_clear = FALSE;
   CPmwDoc *pDoc = GetDocument();
   SHORT index;

   if ((index = pDoc->get_date_selected()) != -1)
   {
      //CalendarObjectPtr calobj = pDoc->get_current_calendar();

		CalendarObjectPtr calobj = NULL;
		PMGPageObjectPtr obj = NULL;
		if ((obj = pDoc->selected_objects()) != NULL
			&& obj->type() == OBJECT_TYPE_CalendarDate)
		{
			calobj = (CalendarObjectPtr)obj->get_container();
		}
	
		if (calobj == NULL)
			return;
      can_clear = calobj->get_date_graphic() != NULL
                  || calobj->get_date_text() != NULL;
   }

   pCmdUI->Enable(can_clear);
}

/*
// Choose the color for a date's graphic.
*/

void CPmwView::OnCalendarColor()
{
   CPmwDoc *pDoc = GetDocument();
   //CalendarObjectPtr calobj = pDoc->get_current_calendar();
   CalendarObjectPtr calobj = NULL;
	PMGPageObjectPtr obj = NULL;
	if ((obj = pDoc->selected_objects()) != NULL
		&& obj->type() == OBJECT_TYPE_CalendarDate)
	{
		calobj = (CalendarObjectPtr)obj->get_container();
	}
	
	if (calobj == NULL)
		return;

   GraphicObjectPtr object;

   if ((object = calobj->get_date_graphic()) != NULL)
   {
      choose_picture_color(object);
   }
}

void CPmwView::OnCalendarAttributes()
{
   CPmwDoc *pDoc = GetDocument();

    CalendarObjectPtr calobj = NULL;
	PMGPageObjectPtr obj = NULL;
	if ((obj = pDoc->selected_objects()) != NULL
		&& obj->type() == OBJECT_TYPE_CalendarDate)
	{
		calobj = (CalendarObjectPtr)obj->get_container();
	}
	
	if (calobj == NULL)
		return;
	
 //  CalendarObjectPtr calobj = pDoc->get_current_calendar();

   GraphicObjectPtr object;

   if ((object = calobj->get_date_graphic()) != NULL)
   {
      pDoc->choose_picture_attributes(object);
   }
}

void CPmwView::OnUpdateCalendarColor(CCmdUI* pCmdUI)
{
   BOOL can_happen = FALSE;
   CPmwDoc *pDoc = GetDocument();
   SHORT index;

   if ((index = pDoc->get_date_selected()) != -1)
   {
      //CalendarObjectPtr calobj = pDoc->get_current_calendar();
	   CalendarObjectPtr calobj = NULL;
		PMGPageObjectPtr obj = NULL;
		if ((obj = pDoc->selected_objects()) != NULL
			&& obj->type() == OBJECT_TYPE_CalendarDate)
		{	
			calobj = (CalendarObjectPtr)obj->get_container();
		}
	
		if (calobj == NULL)
			return;

      can_happen = calobj->get_date_graphic() != NULL;
   }

   pCmdUI->Enable(can_happen);
}

void CPmwView::OnCalendarStyle()
{
   CalendarObjectPtr calobj;
   CPmwDoc *pDoc = GetDocument();

	// load first page of project, since we start the date there
	DWORD dwCurrentPageIndex = pDoc->CurrentPageIndex();

	pDoc->LoadPage(pDoc->GetPage(0));
	calobj = pDoc->get_current_calendar(TRUE);
	pDoc->LoadPage(pDoc->GetPage(dwCurrentPageIndex));

   if (calobj != NULL)
   {
      CALENDAR_INFO info = calobj->get_calendar().get_info();

      if (!pDoc->object_selected(calobj))
      {
         pDoc->deselect_all();
      }

   /* Let's edit the calendar style. */

      CCalendarStyleDlg cdlg(info, pDoc);

      if (cdlg.DoModal() == IDOK)
      {
      /* Extract the info out. */
         if (cdlg.get_exit_info(info))
         {
            CCmdCalendarStyle* pCommand = new CCmdCalendarStyle(IDCmd_CalendarStyle);
            if (!pCommand->Before(pDoc, calobj))
            {
               delete pCommand;
               pCommand = NULL;
            }

            set_arrow_tool();
            if (!pDoc->object_selected(calobj))
            {
               pDoc->deselect_all();
            }
						
				((CCalendarDoc*)pDoc)->ChangeCalendarDate(info);
                        
            pDoc->SetModifiedFlag();

            if (pCommand != NULL)
            {
               if (pCommand->After())
               {
                  pDoc->AddCommand(pCommand, FALSE);
               }
               else
               {
               /* Forget the command. */
                  delete pCommand;
               }
            }
         }
      }
   }
}

/*
// Choose the font for calendar weekday names.
*/

void CPmwView::OnCalendarNames()
{
   CalendarObjectPtr calobj;
   CPmwDoc *pDoc = GetDocument();

   if ((calobj = pDoc->get_current_calendar(TRUE)) != NULL)
   {
      CalendarRef calendar = calobj->get_calendar();
      CTextStyle style = calendar.LabelFontStyle();
      CTextStyle NewStyle = style;

      if (pDoc->ChooseFont(NewStyle) == IDOK)
      {
         CCmdWeekdayStyle* pCommand = new CCmdWeekdayStyle;

         if (!pCommand->Before(pDoc, calobj, style))
         {
            delete pCommand;
            pCommand = NULL;
         }

				 ((CCalendarDoc*)pDoc)->ChangeLabelFontStyle(NewStyle);
         //calendar.SetLabelFontStyle(NewStyle);

         pDoc->SetModifiedFlag();

         if (pCommand != NULL)
         {
            if (pCommand->After(NewStyle))
            {
               pDoc->AddCommand(pCommand, FALSE);
            }
            else
            {
               delete pCommand;
            }
         }
      }
   }
}

void CPmwView::OnCalendarNumber()
{
   CalendarObjectPtr calobj;
   CPmwDoc *pDoc = GetDocument();

   if ((calobj = pDoc->get_current_calendar(TRUE)) != NULL)
   {
      CalendarRef calendar = calobj->get_calendar();
      CTextStyle style = calendar.NumberFontStyle();
      CTextStyle NewStyle = style;

      if (pDoc->ChooseFont(NewStyle) == IDOK)
      {
         CCmdNumberStyle* pCommand = new CCmdNumberStyle;

         if (!pCommand->Before(pDoc, calobj, style))
         {
            delete pCommand;
            pCommand = NULL;
         }
				
				 ((CCalendarDoc*)pDoc)->ChangeNumberFontStyle(NewStyle);
         //calendar.SetNumberFontStyle(NewStyle);

         pDoc->SetModifiedFlag();

         if (pCommand != NULL)
         {
            if (pCommand->After(NewStyle))
            {
               pDoc->AddCommand(pCommand, FALSE);
            }
            else
            {
               delete pCommand;
            }
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// The OnTextBanner function is called when the user selects the 
// "Edit Banner Text" command.
//
void CPmwView::OnTextBanner()
{
	// get_main_frame() returns the special banner text object
   CFrameObject* main_frame = GetDocument()->get_main_frame();

   if (main_frame != NULL)
   {
      edit_frame(main_frame);
   }
}

// this function disables the "Edit Banner Text" command if the
// current project type is not a banner type
//
void CPmwView::OnUpdateTextBanner(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->get_project_type() == PROJECT_TYPE_Banner);
}

/////////////////////////////////////////////////////////////////////////////

void CPmwView::OnSomethingSelected(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->something_selected());
}

void CPmwView::OnOneSelection(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->one_selection());
}

void CPmwView::OnPicturesReplace()
{
   GetDocument()->replace_picture();
}

/*
// Choose an object's position.
*/

SHORT CPmwView::choose_object_position(int nStartPage /*=0*/)
{
   SHORT ret = IDCANCEL;
   CPmwDoc *pDoc = GetDocument();
   PMGPageObjectPtr pFirstObject = pDoc->selected_objects();
   PMGDatabasePtr pDatabase = pDoc->get_database();

/*
// Load a layout list.
*/

   LayoutEntryList LayoutList;

#if 0
   if (LayoutList.read_layout_list(pDoc) != ERRORCODE_None)
   {
      return ret;
   }
#endif
   int nElements = LayoutList.count();

/*
// Count the selected objects.
*/

   PMGPageObjectPtr pObject = pFirstObject;
   int nLayoutType = pObject->type();     // *** CHANGE ME!!! ***
   DB_RECORD_NUMBER OldLayout = -1;

   int nCount = 0;
   while (pObject != NULL)
   {
      DB_RECORD_NUMBER ObjLayout = pObject->get_layout();

      if (OldLayout == -1)
      {
         OldLayout = ObjLayout;
      }
      else if (OldLayout != ObjLayout)
      {
         OldLayout = 0;
      }

      nCount++;
      pObject = pObject->next_selected_object();
   }

/*
// Allocate memory for our structures.
*/

   CPositionData Data;
   Data.pElements = NULL;
   Data.pObjects = NULL;

   /* Set lLayoutChosen if we can. */

   Data.lLayoutChosen = 0;
   if (OldLayout > 0)
   {
      ObjectLayoutPtr pLayout;

   /* Select the string which matches the current name. */

      if ((pLayout = (ObjectLayoutPtr)pDatabase->get_record(OldLayout, NULL, RECORD_TYPE_ObjectLayout)) != NULL)
      {
      /* Only match if type is correct. */

         if (nLayoutType == pLayout->get_type())
         {
         /* Find the name. */
            LPCSTR pName = pLayout->get_name();

            for (int nElement = 0; nElement < nElements; nElement++)
            {
               LayoutEntry* pEntry = (LayoutEntry*)LayoutList.element_in_list(nElement+1);
               if (stricmp(pEntry->get_name(), pName) == 0)
               {
               /* Found the name. Layouts are 1-based. */
                  Data.lLayoutChosen = nElement+1;
                  TRACE("Setting chosen layout to %ld.\r\n", Data.lLayoutChosen);
                  break;
               }
            }
         }
         pLayout->release();
      }
   }

   TRY
   {
      /* Setup the data. */

      PBOX PageBound;
      pDoc->get_panel_world(&PageBound);

      Data.PageBound.x0 = 0;
      Data.PageBound.y0 = 0;
      Data.PageBound.x1 = PageBound.x1 - PageBound.x0;
      Data.PageBound.y1 = PageBound.y1 - PageBound.y0;
      Data.lObjectCount = (long)nCount;
      Data.pObjects = new CObjectElement[nCount];

      Data.lElementCount = (long)nElements;
      Data.pElements = new CStandardElement[nElements];

      /* Set the data for the objects. */

      pObject = pFirstObject;

      CObjectElement* pOE = Data.pObjects;

      while (pObject != NULL)
      {
         pOE->ObjectBound = ((RectPageObject*)pObject)->get_unrotated_bound();
         pOE->ObjectBound.x0 -= PageBound.x0;
         pOE->ObjectBound.y0 -= PageBound.y0;
         pOE->ObjectBound.x1 -= PageBound.x0;
         pOE->ObjectBound.y1 -= PageBound.y0;

         if (!pObject->original_dims(&pOE->ObjectSize))
         {
            pOE->ObjectSize.x = pOE->ObjectSize.y = 0;
         }
         pOE->lFlags = 0;
         if (pObject->type() == OBJECT_TYPE_Line)
         {
            pOE->lFlags |= CObjectElement::FLAG_Line;
         }
         if (pObject->get_flags() & OBJECT_FLAG_xflipped)
         {
            pOE->lFlags |= CObjectElement::FLAG_XFlipped;
         }
         if (pObject->get_flags() & OBJECT_FLAG_yflipped)
         {
            pOE->lFlags |= CObjectElement::FLAG_YFlipped;
         }
         pOE->lAngle = (DWORD)MakeFixed(RAD2DEG(pObject->get_rotation()));
         pObject = pObject->next_selected_object();
         pOE++;
      }

      /* Set the data for the elements. */

      CStandardElement* pSE = Data.pElements;
      LayoutEntry* pEntry = (LayoutEntry*)LayoutList.first_element();

      for (int nElement = 0; nElement < nElements; nElement++)
      {
         pSE->pName = pEntry->get_name();

         // Assume only one element per entry.

         LayoutElement* pElement = pEntry->get_element(0);

         pSE->dwFlags = (DWORD)pElement->record.flags;
         pSE->Bound.x0 = (PCOORD)pElement->record.where.left;
         pSE->Bound.y0 = (PCOORD)pElement->record.where.top;
         pSE->Bound.x1 = (PCOORD)pElement->record.where.right;
         pSE->Bound.y1 = (PCOORD)pElement->record.where.bottom;

         // Next...

         pSE++;
         pEntry = (LayoutEntry*)pEntry->next_element();
      }

   /* Do the dialog. */

      ret = DialogManager.DoPositionDialog(&Data, nStartPage);

      if (ret == IDOK)
      {
      /* Create the undo/redo command for this action. */

         CCmdChangeSelects* pCommand = new CCmdChangeSelects(IDCmd_PositionSelects);
         if (!pCommand->Before(this))
         {
            delete pCommand;
            pCommand = NULL;
         }

      /*
      // The user has chosen new positions.
      // If this new position involves a layout, create the layout now.
      */

         DB_RECORD_NUMBER NewLayout = 0;

         if (Data.lLayoutChosen != 0)
         {
            LayoutEntry* pEntry = (LayoutEntry*)LayoutList.element_in_list((int)Data.lLayoutChosen);
            NewLayout = pDatabase->new_layout_record((LPSTR)pEntry->get_name(),
                                          nLayoutType,
                                          pEntry->get_elements());
         }

         pObject = pFirstObject;

         FLAGS wChangedFlags = 0;
         CObjectElement* pElement = Data.pObjects;

         while (pObject != NULL)
         {
            FLAGS flags = pObject->get_flags();

				pDoc->MarkFramesBelow(pObject);

         /* Erase in old place. */

            pDoc->refresh_object(pObject);

         /* Turn off selects to set new layout. */

            pDoc->toggle_object(pObject);

         /* Set the new layout. */

            if (NewLayout == 0)
            {
            /* Manual layout. */
               pObject->set_new_layout(MANUAL_LAYOUT);
               pObject->set_rotation(DEG2RAD(MakeDouble((CFixed)pElement->lAngle)));
               PBOX Bound = pElement->ObjectBound;
               Bound.x0 += PageBound.x0;
               Bound.y0 += PageBound.y0;
               Bound.x1 += PageBound.x0;
               Bound.y1 += PageBound.y0;
               ((RectPageObject*)pObject)->set_unrotated_bound(Bound);
            }
            else
            {
            /* Standard position forces unrotated bound (for now). */
               pObject->set_rotation(0);
            /* Attach this layout to the object. */
               pDatabase->inc_layout_record(NewLayout);  // Another ref.
               pObject->set_new_layout(NewLayout);
            }

         /* Recalc the object. */

            pDoc->calc_object(pObject);

         /* Restore selects. */

            pDoc->toggle_object(pObject);

         /* Show in new position. */

            pDoc->refresh_object(pObject);

            wChangedFlags |= (flags ^ pObject->get_flags());

            pObject = pObject->next_selected_object();
            pElement++;
         }

      /* Free our creation reference. */

         if (NewLayout != 0)
         {
            pDatabase->free_layout_record(NewLayout);
         }

			pDoc->UpdateMarkedFrames(TRUE);
			pDoc->SetModifiedFlag();

      /*
      // Finish up the undo/redo command.
      */

         if (pCommand != NULL)
         {
            if (pCommand->After())
            {
               pDoc->AddCommand(pCommand, FALSE);
            }
            else
            {
               delete pCommand;
            }
         }

      /*
      // See if we need to resize the project.
      */

         if (wChangedFlags & OBJECT_FLAG_flow)
         {
         /* A 'flow' object changed. */
            pDoc->resize_project(NULL);
         }
      }
   }
   END_TRY

   delete [] Data.pObjects;
   delete [] Data.pElements;

	free_edit_background();

   return ret;
}

void CPmwView::OnStandardPosition()
{
   choose_object_position();
}

void CPmwView::OnUpdatePositionStandard(CCmdUI* pCmdUI)
{
/*
// We set the submenu item to NULL so that this will work for the submenu
// as well as the menu item.
*/
   CMenu *save = pCmdUI->m_pSubMenu;

   pCmdUI->m_pSubMenu = NULL;
// OnOneSelection(pCmdUI);
   OnSomethingSelected(pCmdUI);
   pCmdUI->m_pSubMenu = save;
}

void CPmwView::OnStandardSize()
{
   choose_object_position(1);
}

void CPmwView::OnRotate0()
{
   DoSetRotation(0);
}

void CPmwView::OnUpdateRotate0(CCmdUI* pCmdUI)
{
   DoUpdateRotate(pCmdUI, 0);
}

void CPmwView::OnRotate45()
{
   DoSetRotation(45);
}

void CPmwView::OnUpdateRotate45(CCmdUI* pCmdUI)
{
   DoUpdateRotate(pCmdUI, 45);
}

void CPmwView::OnRotate90()
{
   DoSetRotation(90);
}

void CPmwView::OnUpdateRotate90(CCmdUI* pCmdUI)
{
   DoUpdateRotate(pCmdUI, 90);
}

void CPmwView::OnRotate135()
{
   DoSetRotation(135);
}

void CPmwView::OnUpdateRotate135(CCmdUI* pCmdUI)
{
   DoUpdateRotate(pCmdUI, 135);
}

void CPmwView::OnRotate180()
{
   DoSetRotation(180);
}

void CPmwView::OnUpdateRotate180(CCmdUI* pCmdUI)
{
   DoUpdateRotate(pCmdUI, 180);
}

void CPmwView::OnRotate225()
{
   DoSetRotation(225);
}

void CPmwView::OnUpdateRotate225(CCmdUI* pCmdUI)
{
   DoUpdateRotate(pCmdUI, 225);
}

void CPmwView::OnRotate270()
{
   DoSetRotation(270);
}

void CPmwView::OnUpdateRotate270(CCmdUI* pCmdUI)
{
   DoUpdateRotate(pCmdUI, 270);
}

void CPmwView::OnRotate315()
{
   DoSetRotation(315);
}

void CPmwView::OnUpdateRotate315(CCmdUI* pCmdUI)
{
   DoUpdateRotate(pCmdUI, 315);
}

void CPmwView::OnRotateMore()
{
   CCustomRotateDialog Dialog;

   double dAngle = GetDocument()->selected_objects()->get_rotation();

   Dialog.SetRotation(dAngle*RAD2DEGC);

   if (Dialog.DoModal() == IDOK)
   {
      GetDocument()->ChangeRotation(normalize_angle(Dialog.GetRotation()*DEG2RADC));
   }
}

void CPmwView::OnUpdateRotateMore(CCmdUI* pCmdUI)
{
   DoUpdateRotate(pCmdUI);
}

void CPmwView::DoUpdateRotate(CCmdUI* pCmdUI, int nDegrees /*=-1*/)
{
   CPmwDoc* pDoc = GetDocument();
   BOOL fEnable = CanRotate();
   BOOL fCheck = FALSE;
   if (fEnable && nDegrees != -1)
   {
      double dAngle = pDoc->selected_objects()->get_rotation();
      fCheck = (dAngle == DEG2RAD(nDegrees));
   }
   pCmdUI->Enable(fEnable);
   pCmdUI->SetCheck(fCheck);
}

void CPmwView::DoSetRotation(int nDegrees)
{
   GetDocument()->ChangeRotation(DEG2RAD(nDegrees));
}

BOOL CPmwView::CanRotate(void)
{
   CPmwDoc* pDoc = GetDocument();
   PMGPageObject* pObject = pDoc->selected_objects();
   return pDoc->one_selection()
            && ((pObject->get_select_flags() & SELECT_FLAG_rotate_handle)
                  || pObject->type() == OBJECT_TYPE_Line);
}

void CPmwView::OnSendToFront()
{
   GetDocument()->SendToFront();
}

void CPmwView::OnForwardOne()
{
   GetDocument()->ForwardOne();
}

void CPmwView::OnBackOne()
{
   GetDocument()->BackOne();
}

void CPmwView::OnSendToBack()
{
   GetDocument()->SendToBack();
}

void CPmwView::OnCenter()
{
   GetDocument()->CenterSelects();
   free_edit_background();
}

void CPmwView::OnHcenter()
{
   GetDocument()->HCenterSelects();
   free_edit_background();
}

void CPmwView::OnVcenter()
{
   GetDocument()->VCenterSelects();
   free_edit_background();
}

void CPmwView::OnFullPage()
{
   GetDocument()->FullPageSelects();
   free_edit_background();
}

void CPmwView::OnUpdateEditFrame(CCmdUI* pCmdUI)
{
   PMGPageObjectPtr object = GetDocument()->selected_objects();

   pCmdUI->Enable(object != NULL                            // Have an object
               && object->next_selected_object() == NULL    // Single object
               && (object->type() == OBJECT_TYPE_Frame         // Is a frame
									 || (object->type() == OBJECT_TYPE_Cell))
               && edit_event_handler != mode_text_edit);    // Not already
}

void CPmwView::OnEditFrame()
{
   PMGPageObjectPtr object = GetDocument()->selected_objects();

   if (object != NULL)
   {
      switch (object->type())
      {
         case OBJECT_TYPE_Frame:
				 case OBJECT_TYPE_Cell:
         {
            edit_frame((CFrameObject*)object);
            break;
         }
         default:
         {
            break;
         }
      }
   }
}

void CPmwView::OnUpdateLayerToBack(CCmdUI* pCmdUI)
{
   // Multiple objects are assumed to need send-to-back, always.
   // This could be made more exact with a bit more algorithm investment
   // if it is deemed necessary.
   CPmwDoc *pDoc = GetDocument();
   BOOL ok = FALSE;
   if (pDoc->something_selected())
   {
      ok = TRUE;

      if (pDoc->one_selection())
      {
         PMGPageObjectPtr object = pDoc->selected_objects();
         ObjectListPtr list = pDoc->object_list();

         if (object == list->first_that(CPmwDoc::NotBorderCriterion))
         {
            ok = FALSE;
         }
      }
   }

   pCmdUI->Enable(ok);
}

void CPmwView::OnUpdateLayerBackOne(CCmdUI* pCmdUI)
{
   CPmwDoc *pDoc = GetDocument();
   BOOL ok = FALSE;

   if (pDoc->one_selection())
   {
      PMGPageObjectPtr object = pDoc->selected_objects();
      ObjectListPtr list = pDoc->object_list();

      if (object != list->first_that(CPmwDoc::NotBorderCriterion))
      {
         ok = TRUE;
      }
   }

   pCmdUI->Enable(ok);
}

void CPmwView::OnUpdateLayerForwardOne(CCmdUI* pCmdUI)
{
   CPmwDoc *pDoc = GetDocument();
   BOOL ok = FALSE;

   if (pDoc->one_selection())
   {
      PMGPageObjectPtr object = pDoc->selected_objects();

      if (object->next_object() != NULL)
      {
         ok = TRUE;
      }
   }

   pCmdUI->Enable(ok);
}

void CPmwView::OnUpdateLayerToFront(CCmdUI* pCmdUI)
{
   if (pCmdUI->m_pSubMenu != NULL)
   {
   /* Update the whole Layer menu. */
      CMenu *save = pCmdUI->m_pSubMenu;
      pCmdUI->m_pSubMenu = NULL;

		BOOL fEnable = FALSE;

		// Enable the Layer menu if there is an object selected and there is more than
		// one "layerable" object in the document.
      CPmwDoc *pDoc = GetDocument();
		if (pDoc->something_selected())
		{
			ObjectListPtr list = pDoc->object_list();
			if (list != NULL)
			{
				PMGPageObjectPtr object = (PMGPageObjectPtr)(list->first_that(CPmwDoc::NotBorderCriterion));
				if (object != NULL)
				{
					fEnable = object->next_object() != NULL;
				}
			}
		}
		pCmdUI->Enable(fEnable);

      // Multiple objects are considered layerable.
//		pCmdUI->Enable(!m_Txp.Valid()
//							&& pDoc->something_selected()
//							&& (!pDoc->one_selection()
//								|| ((object = pDoc->objects()) != NULL
//									&& object->next_object() != NULL)));

      pCmdUI->m_pSubMenu = save;
   }
   else
   {
      CPmwDoc *pDoc = GetDocument();
      // Multiple objects are assumed to need send-to-front, always.
      // This could be made more exact with a bit more algorithm investment
      // if it is deemed necessary.
      BOOL ok = FALSE;

      if (pDoc->something_selected())
      {
         ok = TRUE;
         if (pDoc->one_selection())
         {
            PMGPageObjectPtr object = pDoc->selected_objects();

            if (object->next_object() == NULL)
            {
               ok = FALSE;
            }
         }
      }

      pCmdUI->Enable(ok);
   }
}

void CPmwView::OnPicturesColor()
{
   PMGPageObjectPtr object;

   if ((object = GetDocument()->selected_objects()) != NULL
            && object->type() == OBJECT_TYPE_Graphic)
   {
      choose_picture_color((GraphicObjectPtr)object);
   }
}

void CPmwView::OnOnePictureSelection(CCmdUI* pCmdUI)
{
   CPmwDoc *pDoc = GetDocument();

   pCmdUI->Enable(pDoc->one_selection()
            && pDoc->selected_objects()->type() == OBJECT_TYPE_Graphic);
}

void CPmwView::OnViewAreaView()
{
   set_zoom_tool();
}

void CPmwView::OnPictureAttributes()
{
   CPmwDoc *pDoc = GetDocument();
   PMGPageObject* pObject = pDoc->selected_objects();
   if (pObject != NULL)
   {
      switch (pObject->type())
      {
         case OBJECT_TYPE_Graphic:
         {
            pDoc->choose_picture_attributes((GraphicObjectPtr)pObject);
            break;
         }

         case OBJECT_TYPE_Component:
			{
				// todo: train CPictureAttrDlg to work with component objects
            // pDoc->choose_picture_attributes((GraphicObjectPtr)pObject);
            break;
			}

#if 0
         case OBJECT_TYPE_Line:
         {
            CLineAttributesDialog Dialog((DrawingObject*)pObject, this);
            PBOX OriginalBound;

            pObject->get_refresh_bound(&OriginalBound, &rc);

            if (Dialog.DoModal() == IDOK)
            {
               pDoc->SetModifiedFlag();
               pDoc->add_refresh(&OriginalBound);
               pDoc->refresh_object(pObject);
            }
            break;
         }
         case OBJECT_TYPE_Rectangle:
         case OBJECT_TYPE_Ellipse:
         case OBJECT_TYPE_DrawingObject:
         {
            CDrawingAttributesDialog Dialog((DrawingObject*)pObject, this);
            PBOX OriginalBound;

            pObject->get_refresh_bound(&OriginalBound, &rc);

            if (Dialog.DoModal() == IDOK)
            {
               pDoc->SetModifiedFlag();
               pDoc->add_refresh(&OriginalBound);
               pDoc->refresh_object(pObject);
            }
            break;
         }
#endif
         default:
         {
            ASSERT(FALSE);
            break;
         }
      }
   }
}

void CPmwView::OnUpdatePictureAttributes(CCmdUI* pCmdUI)
{
   CPmwDoc *pDoc = GetDocument();
   BOOL fEnable = FALSE;

   if (pDoc->one_selection())
   {
      switch (pDoc->selected_objects()->type())
      {
         case OBJECT_TYPE_Graphic:
#if 0
         case OBJECT_TYPE_Rectangle:
         case OBJECT_TYPE_Ellipse:
         case OBJECT_TYPE_Line:
         case OBJECT_TYPE_DrawingObject:
#endif
         {
            fEnable = TRUE;
            break;
         }

         case OBJECT_TYPE_Component:
			{
				// todo: disable for non-image components  -b0b-
            fEnable = TRUE;
            break;
			}

         default:
         {
            break;
         }
      }
   }
   pCmdUI->Enable(fEnable);
}

void CPmwView::OnEditSelectAll()
{
   if (m_Txp.Valid())
   {
      m_Txp.Off();
      m_Txp.SelectAll();
      m_Txp.On();
      SetFocus();
   }
   else
   {
      CPmwDoc *pDoc = GetDocument();
      pDoc->deselect_all();
      pDoc->select_all();
   }
}

void CPmwView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
   BOOL ok = FALSE;

   if (m_Txp.Valid())
   {
      ok = TRUE;
   }
   else
   {
   /* Look for an object to select. */
      PMGPageObjectPtr object;
      CPmwDoc *pDoc = GetDocument();
      SHORT panel = pDoc->get_current_panel();

      for (object = pDoc->objects();
                     object != NULL;
                     object = (PMGPageObjectPtr)object->next_object())
      {
         if (object->get_panel() == panel
               && !(object->get_select_flags() & SELECT_FLAG_solo_select)
               && !(object->get_flags() & OBJECT_FLAG_locked))
         {
            ok = TRUE;
            break;
         }
      }
   }
   pCmdUI->Enable(ok);
}

/////////////////////////////////////////////////////////////////////////////
// Text wrap

void CPmwView::OnTextWrap() 
{
   int nFlow = -2;

   // Accumulate the text wrap from the objects.

   CPmwDoc* pDoc = GetDocument();

   PMGPageObject* pObject;
   for (pObject = pDoc->selected_objects();
        pObject != NULL;
        pObject = pObject->next_selected_object())
   {
      int nThisFlow = (int)pObject->GetFlowAround();
      if (nFlow == -2)
      {
         nFlow = nThisFlow;
      }
      else if (nFlow != nThisFlow)
      {
         // Undefined.
         nFlow = -1;
      }
   }

   CTextWrapDialog Dialog(nFlow);

   if (Dialog.DoModal() == IDOK)
   {
      int nOutFlow = Dialog.m_nFlow;
      if (nOutFlow != -1)
      {
         // Create the undo command.
         CCmdChangeSelects* pCommand = NULL;
         TRY
            pCommand = new CCmdChangeSelects(IDCmd_TextWrap);
         END_TRY

         if (pCommand != NULL)
         {
            if (!pCommand->Before(this))
            {
               delete pCommand;
               pCommand = NULL;
            }
         }

         // Apply the flow to all the objects.
         BOOL fMarked = FALSE;
         for (pObject = pDoc->selected_objects();
              pObject != NULL;
              pObject = pObject->next_selected_object())
         {
            if (nOutFlow != (int)pObject->GetFlowAround())
            {
               pDoc->MarkFramesBelow(pObject);
               pObject->SetFlowAround(nOutFlow);
               pDoc->MarkFramesBelow(pObject);
               fMarked = TRUE;
            }
         }

         // If we didn't change anything, we don't need to undo anything.
         if (!fMarked)
         {
            delete pCommand;
            pCommand = NULL;
         }
         else
         {
				// Update the marked frames.
            pDoc->UpdateMarkedFrames(TRUE);
            pDoc->SetModifiedFlag();
         }

         // If we have a command, add it now.
         if (pCommand != NULL)
         {
            if (pCommand->After())
            {
               pDoc->AddCommand(pCommand, FALSE);
            }
            else
            {
               delete pCommand;
            }
         }
      }
   }
}

void CPmwView::OnUpdateTextWrap(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->something_selected());
}

void CPmwView::OnSpelling() 
{
   // Do we support this?
   if (!GetConfiguration()->SupportsSpellCheck())
   {
      return;
   }

   // Do the spell-check.
   CSpellChecker SpellChecker(this);

   // Open the session. This inits the DLL and opens all necessary lexicons.
   if (SpellChecker.OpenSession())
   {
      // Perform the spelling check.
      SpellChecker.CheckSpelling();

      // We are all done. Close the session.
      SpellChecker.CloseSession();
   }
}

void CPmwView::OnUpdateSpelling(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->DocumentRecord()->NumberOfStories() > 0);
}

void CPmwView::OnTextEditReplace() 
{
   CFindReplaceText FRT (AfxGetMainWnd ());

   FRT.DoModal (this, FALSE, FR_DOWN);
}

void CPmwView::OnUpdateTextEditReplace(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->DocumentRecord()->NumberOfStories() > 0);
}

void CPmwView::OnEditDeletePage() 
{
   CPmwDoc* pDocument = GetDocument();
   set_arrow_tool();

   CDeletePageDialog Dialog(pDocument, this);
   if (Dialog.DoModal() == IDOK)
   {
      // Create a command to hold all the little commands we will generate.
      CCmdDeletePages* pMainCommand = new CCmdDeletePages;

      long lFrom = atol(Dialog.m_csFrom)-1;
      long lTo = atol(Dialog.m_csTo)-1;

      pMainCommand->Snapshot(this, lFrom, lTo);

      while (lFrom <= lTo)
      {
         DB_RECORD_NUMBER lPage = pDocument->GetPage(lFrom);

         CCmdDeletePage* pCommand = new CCmdDeletePage;

         if (pCommand->Snapshot(this, lPage, lFrom))
         {
            pMainCommand->AddCommand(pCommand);
         }
         else
         {
            delete pCommand;
            delete pMainCommand;
            pMainCommand = NULL;
         }

         lFrom++;
      }
      if (pMainCommand != NULL)
      {
         // Add the command to the undo list (doing it in the process).
         pDocument->AddCommand(pMainCommand);
      }
   }
}

void CPmwView::OnUpdateEditDeletePage(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->NumberOfPages() > 1);
}

void CPmwView::OnAddPage() 
{
   CPmwDoc* pDocument = GetDocument();
   set_arrow_tool();

   CAddPageDialog Dialog(pDocument, this);
   if (Dialog.DoModal() == IDOK)
   {
      DWORD dwPages = atol(Dialog.m_csPages);
      DWORD dwAtPage = pDocument->CurrentPageIndex();
      PMGDatabase* pDatabase = pDocument->get_database();
      PageRecord* pSourcePage = NULL;

      if (Dialog.m_nWhere == CAddPageDialog::AfterCurrentPage)
      {
         dwAtPage++;
      }

      if (Dialog.m_nContents == CAddPageDialog::AddPageCopies)
      {
         // We want to lock the page record corresponding to the page number
         // they specified (before inserting).
         DB_RECORD_NUMBER lPage = atol(Dialog.m_csCopyPage)-1;
         pSourcePage = (PageRecord*)pDatabase->get_record(pDocument->GetPage(lPage), NULL, RECORD_TYPE_Page);
         ASSERT(pSourcePage != NULL);
      }

      DWORD dwStartPage = dwAtPage;

      while (dwPages-- != 0)
      {
         if (!pDocument->AddPage(dwAtPage))
         {
            break;
         }
         if (pSourcePage != NULL)
         {
            // Lock the new page.
            PageRecord* pNewPage = (PageRecord*)pDatabase->get_record(pDocument->GetPage(dwAtPage), NULL, RECORD_TYPE_Page);
            if (pNewPage != NULL)
            {
               // Copy the objects over.
               ObjectListPtr	pNewObjects = pNewPage->objects();
					ObjectListPtr	pSourceObjects = pSourcePage->objects();

					pDatabase->CopyObjectList(pNewObjects, pSourceObjects, pNewPage->Id());

					// Reflow text boxes as needed.
					PMGPageObject* pFirstObject = (PMGPageObject*)pNewPage->objects()->first_object();
					if (pFirstObject != NULL)
					{
						pDocument->MarkFramesBelow(NULL, pFirstObject);
						pDocument->UpdateMarkedFrames(TRUE, pFirstObject);
					}
               pNewPage->release(TRUE);
            }
            else
            {
               // Could not get the newly created page!
               ASSERT(FALSE);
            }
         }
         // Copy the items if so desired.
         dwAtPage++;
      }

      // If we had a source page, release it now.
      if (pSourcePage != NULL)
      {
         pSourcePage->release();
      }

      if (dwAtPage > dwStartPage)
      {
         // We added something. Create an undo command for this.
         CCmdInsertPages* pMainCommand = new CCmdInsertPages;

         pMainCommand->Snapshot(this, dwStartPage, dwAtPage-1);

         for (DWORD dwPage = dwStartPage; dwPage < dwAtPage; dwPage++)
         {
            DB_RECORD_NUMBER lPage = pDocument->GetPage(dwPage);
            CCmdInsertPage* pCommand = new CCmdInsertPage;
            if (pCommand->Snapshot(this, lPage, dwPage))
            {
               pMainCommand->AddCommand(pCommand);
            }
            else
            {
               delete pCommand;
               delete pMainCommand;
               pMainCommand = NULL;
               break;
            }
         }

         if (pMainCommand != NULL)
         {
            // Add it but don't execute it. It's already done!
            pDocument->AddCommand(pMainCommand, FALSE);
         }

         // The document is now modified.
         pDocument->SetModifiedFlag();

         // Change to the last page added.
         BeforePageChange();
         pDocument->GotoPage(dwAtPage-1);
      }
   }
}

void CPmwView::OnGotoPage() 
{
   CGotoPageDialog Dialog(GetDocument(), this);
   if (Dialog.DoModal() == IDOK)
   {
      DWORD dwAtPage = atol(Dialog.m_csPage);
      ASSERT(dwAtPage != 0);

      BeforePageChange();
      GetDocument()->GotoPage(dwAtPage-1);
   }
}

void CPmwView::OnUpdateGotoPage(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->NumberOfPages() > 1);
}

void CPmwView::OnGotoFirstPage() 
{
   BeforePageChange();
   GetDocument()->GotoPage(0);
}

void CPmwView::OnUpdateGotoFirstPage(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->CurrentPageIndex() != 0);
}

void CPmwView::OnGotoLastPage() 
{
   BeforePageChange();
   GetDocument()->GotoPage(GetDocument()->NumberOfPages()-1);
}

void CPmwView::OnUpdateGotoLastPage(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->CurrentPageIndex() != GetDocument()->NumberOfPages()-1);
}

void CPmwView::OnGotoNextPage() 
{
   BeforePageChange();
   GetDocument()->DoPageNext();
}

void CPmwView::OnUpdateGotoNextPage(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->CanPageNext());
}

void CPmwView::OnGotoPreviousPage() 
{
   BeforePageChange();
   GetDocument()->DoPageBack();
}

void CPmwView::OnUpdateGotoPreviousPage(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetDocument()->CanPageBack());
}

/////////////////////////////////////////////////////////////////////////////
// Indicator support

void CPmwView::DoIndicatorMenu(int nWhichMenu)
{
   POINT p;
   ::GetCursorPos(&p);

   CMenu menu;
   if (menu.LoadMenu(IDR_INDICATORS))
   {
      CMenu* pSubmenu;
      if ((pSubmenu = menu.GetSubMenu(nWhichMenu)) != NULL)
      {
         pSubmenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                  p.x,
                                  p.y,
                                  this);
      }
   }
}

void CPmwView::OnClickedRuler()
{
   // Invoke the ruler menu.
   DoIndicatorMenu(3);
}

void CPmwView::OnPositionIndicator()
{
   if (GetDocument()->something_selected())
   {
      DoIndicatorMenu(0);
   }
}

void CPmwView::OnUpdatePositionIndicator(CCmdUI* pCmdUI)
{
   PPNT p;
   CString csText = " ";         // Empty by default.
   BOOL fSetText = FALSE;  // And leave it that way.

   CPmwDoc* pDoc = GetDocument();

   // check if we're moving a guide
   if (edit_state == RESIZE_STATE_MOVING_GUIDE)
   {
      GetCursorPagePos(&p);
      fSetText = TRUE;
   }
   else if (pDoc->something_selected())
   {
      PBOX Bound;
      PMGPageObjectPtr pObject = pDoc->selected_objects();

      if (pObject != NULL)
      {
         Bound = pObject->get_bound();

         while ((pObject = pObject->next_selected_object()) != NULL)
         {
            PBOX pbox = pObject->get_bound();

            UnionBox(&Bound, &Bound, &pbox);
         }
      }

      p.x = Bound.x0;
      p.y = Bound.y0;
      fSetText = TRUE;
   }
   else
   {
      GetCursorPagePos(&p);
      fSetText = TRUE;

      if (!AfxGetMainWnd()->IsWindowEnabled())
      {
         fSetText = FALSE;
      }
      else if (edit_state == RESIZE_STATE_SELECT)
      {
         // Dragging out a select box.
         // Use whichever coordinates are upper left.
         if (p.x > select_start.x)
         {
            p.x = select_start.x;
         }
         if (p.y > select_start.y)
         {
            p.y = select_start.y;
         }
      }
      else if (!IsPtInBox(p.x, p.y, &rc.source_pbox))
      {
         // Do not report coordinates outside the source area.
         fSetText = FALSE;
      }
   }

   if (fSetText)
   {
      double dX = ((double)p.x)/PAGE_RESOLUTION;
      double dY = ((double)p.y)/PAGE_RESOLUTION;

      CString csCoord_x;
      CString csCoord_y;

#ifdef LOCALIZE
      Util::FormatDouble(CurrentLocale.ConvertFromInches(dX), -CurrentLocale.ConvertPrecisionFromInches(2), csCoord_x);
      Util::FormatDouble(CurrentLocale.ConvertFromInches(dY), -CurrentLocale.ConvertPrecisionFromInches(2), csCoord_y);
#else
      Util::FormatDouble(dX, -2, csCoord_x);
      Util::FormatDouble(dY, -2, csCoord_y);
#endif

	  csText.FormatMessage(IDS_COORDINATE_FORMAT, csCoord_x, csCoord_y);
  
//	  csText += ", ";
//    csText += csCoord;
//    csText += '"';

   }

#ifndef WIN32
   pCmdUI->Enable(FALSE);
#endif
   pCmdUI->SetText(csText);
}

void CPmwView::OnSizeIndicator()
{
   if (GetDocument()->something_selected())
   {
      DoIndicatorMenu(1);
   }
}

void CPmwView::OnUpdateSizeIndicator(CCmdUI* pCmdUI)
{
   PPNT p;
   CString csText = " ";         // Empty by default.
   BOOL fSetText = FALSE;  // And leave it that way.

   CPmwDoc* pDoc = GetDocument();
   if (pDoc->something_selected() && edit_state != RESIZE_STATE_MOVING_GUIDE)
   {
      PBOX Bound;
      PMGPageObjectPtr pObject = pDoc->selected_objects();

      if (pObject != NULL)
      {
         Bound = pObject->get_bound();

         while ((pObject = pObject->next_selected_object()) != NULL)
         {
            PBOX pbox = pObject->get_bound();

            UnionBox(&Bound, &Bound, &pbox);
         }
      }

      p.x = Bound.x1 - Bound.x0;
      p.y = Bound.y1 - Bound.y0;
      fSetText = TRUE;
   }
   else
   {
      // Nothing to do.
      if (!AfxGetMainWnd()->IsWindowEnabled())
      {
         fSetText = FALSE;
      }
      else if (edit_state == RESIZE_STATE_SELECT)
      {
         GetCursorPagePos(&p);

         // Dragging out a select box.
         p.x -= select_start.x;
         p.y -= select_start.y;

         // Normalize.
         if (p.x < 0) p.x = -p.x;
         if (p.y < 0) p.y = -p.y;

         if (p.x != 0 || p.y != 0)
         {
            fSetText = TRUE;
         }
      }
   }

   if (fSetText)
   {
      double dX = ((double)p.x)/PAGE_RESOLUTION;
      double dY = ((double)p.y)/PAGE_RESOLUTION;

      CString csCoord_x, csCoord_y;

#ifdef LOCALIZE
      Util::FormatDouble(CurrentLocale.ConvertFromInches(dX), -CurrentLocale.ConvertPrecisionFromInches(2), csCoord_x);
      Util::FormatDouble(CurrentLocale.ConvertFromInches(dY), -CurrentLocale.ConvertPrecisionFromInches(2), csCoord_y);
#else
      Util::FormatDouble(dX, -2, csCoord_x);
      Util::FormatDouble(dY, -2, csCoord_y);
#endif

	  csText.FormatMessage(IDS_SPC_BY_SPS, csCoord_x, csCoord_y);

//csText += " by ";
//      CString  Temp;
//      Temp.LoadString (IDS_SPC_BY_SPS);
//      csText += Temp;

//      csText += csCoord;
//      csText += '"';
   }

#ifndef WIN32
   pCmdUI->Enable(FALSE);
#endif
   pCmdUI->SetText(csText);
}

void CPmwView::OnRotateIndicator()
{
   if (CanRotate())
   {
      DoIndicatorMenu(2);
   }
}

void CPmwView::OnUpdateRotateIndicator(CCmdUI* pCmdUI)
{
   CString csAngle = " ";

   CPmwDoc* pDoc = GetDocument();
   ASSERT(pDoc != NULL);

   if (CanRotate() && edit_state != RESIZE_STATE_MOVING_GUIDE)
   {
      PMGPageObjectPtr pObject = pDoc->selected_objects();
      if (pObject != NULL)
      {
         double dAngle;

         switch (pObject->type())
         {
            case OBJECT_TYPE_Line:
            {
               PBOX Bounds = pObject->get_bound();
               double x = (double)(Bounds.x1-Bounds.x0);
               double y = (double)(Bounds.y1-Bounds.y0);
               double dx = fabs(x);
               double dy = fabs(y);
               dAngle = atan2(dy, dx);
               FLAGS Flags = pObject->get_flags();
               if (x < 0.0)
               {
                  Flags ^= OBJECT_FLAG_xflipped;
               }
               if (y < 0.0)
               {
                  Flags ^= OBJECT_FLAG_yflipped;
               }
               if ((((Flags & OBJECT_FLAG_xflipped) != 0) ^ ((Flags & OBJECT_FLAG_yflipped) != 0)) == 0)
               {
                  if (dAngle != 0)
                  {
                     dAngle = PI-dAngle;
                  }
               }
               break;
            }
            default:
            {
               dAngle = normalize_angle(pObject->get_rotation());
               break;
            }
         }

         dAngle = dAngle*RAD2DEGC;
         Util::FormatDouble(dAngle, -1, csAngle);

         csAngle += (char)(BYTE)176;      // The degree character!
      }
   }
#ifndef WIN32
   pCmdUI->Enable(FALSE);
#endif

   pCmdUI->SetText(csAngle);
}

/////////////////////////////////////////////////////////////////////////////
// Tool support

void CPmwView::OnTextTool()
{
   OnTool(OBJECT_TYPE_Frame);
}

void CPmwView::OnUpdateTextTool(CCmdUI* pCmdUI)
{
   OnUpdateTool(pCmdUI, OBJECT_TYPE_Frame);
}

void CPmwView::OnFreeDrawTool()
{
   if (edit_event_handler == mode_draw)
   {
      set_arrow_tool();
   }
   else
   {
      // Turn off all selected objects now.
      deselect_all();

      // Control key down? Tool is stuck!
      m_fStuckTool = !!(GetKeyState(VK_CONTROL) & 0x8000);

      // We are now in the "draw" mode.
      set_mode(mode_draw);
   }
}

void CPmwView::OnUpdateFreeDrawTool(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(TRUE);
   pCmdUI->SetCheck(edit_event_handler == mode_draw);
}

void CPmwView::OnLineTool()
{
   OnTool(OBJECT_TYPE_Line);
}

void CPmwView::OnUpdateLineTool(CCmdUI* pCmdUI)
{
   OnUpdateTool(pCmdUI, OBJECT_TYPE_Line);
}

void CPmwView::OnRectangleTool()
{
   OnTool(OBJECT_TYPE_Rectangle);
}

void CPmwView::OnUpdateRectangleTool(CCmdUI* pCmdUI)
{
   OnUpdateTool(pCmdUI, OBJECT_TYPE_Rectangle);
}

void CPmwView::OnEllipseTool()
{
   OnTool(OBJECT_TYPE_Ellipse);
}

void CPmwView::OnUpdateEllipseTool(CCmdUI* pCmdUI)
{
   OnUpdateTool(pCmdUI, OBJECT_TYPE_Ellipse);
}

void CPmwView::OnTool(DB_OBJECT_TYPE nType)
{
   if (edit_event_handler == mode_create && m_nTypeToCreate == nType)
   {
      set_arrow_tool();
   }
   else
   {
      // Turn off all selected objects now.
      deselect_all();

      // Control key down? Tool is stuck!
      m_fStuckTool = !!(GetKeyState(VK_CONTROL) & 0x8000);

      // We are now in the "create" mode.
      set_mode(mode_create);
      m_nTypeToCreate = nType;
   }
}

void CPmwView::OnUpdateTool(CCmdUI* pCmdUI, DB_OBJECT_TYPE nType)
{
   pCmdUI->Enable(TRUE);
   pCmdUI->SetCheck(edit_event_handler == mode_create && m_nTypeToCreate == nType);
}

static CColorPalette FillColorWindow;
static CLineWidthPalette OutlineWidthWindow;
static CTextLineWidthPalette TextOutlineWidthWindow;
static CColorPalette OutlineColorWindow;
static CShadowPalette ShadowWindow;
static CTextShadowPalette TextShadowWindow;
static CColorPalette ShadowColorWindow;
static CShapePalette ShapeWindow;
static CTextShapePalette TextShapeWindow;
static CPPhotoWorkshopMenuPalette PhotoWorkshopMenuWindow;
static CPMadeToOrderMenuPalette MadeToOrderMenuWindow;

BOOL PopupIsActive(void)
{
   return FillColorWindow.GetSafeHwnd() != NULL
      || OutlineWidthWindow.GetSafeHwnd() != NULL
      || TextOutlineWidthWindow.GetSafeHwnd() != NULL
      || OutlineColorWindow.GetSafeHwnd() != NULL
      || ShadowWindow.GetSafeHwnd() != NULL
      || TextShadowWindow.GetSafeHwnd() != NULL
      || ShadowColorWindow.GetSafeHwnd() != NULL
      || ShapeWindow.GetSafeHwnd() != NULL
      || TextShapeWindow.GetSafeHwnd() != NULL
      || PhotoWorkshopMenuWindow.GetSafeHwnd() != NULL;
}

void CPmwView::OnModifyPalette(UINT uID, UINT uPaletteID, CPopupPalette& Window, BOOL fGradient)
{
	if (dynamic_cast<CColorPalette *>( &Window ))
	{
		CColorPalette& cColorWindow = (CColorPalette&) Window;

		uWORD  flags  = (fGradient ? kColorDefault /*| kExcludeTextures*/ : kShowTransparent);
		RColor rColor = cColorWindow.GetRColor();
		RWinColorDlg dlg( this, flags );
		dlg.SetColor( rColor );

		if (IDOK == dlg.DoModal())
		{
			rColor = dlg.SelectedColor();
			cColorWindow.SetColor( rColor );

			Util::SendWmCommand(this, uPaletteID, &Window, 0);
		}
	}
   else if (Window.GetSafeHwnd() == NULL)
   {
      // Create the window.
      Window.SetOwner(this);
		Window.ShowGradient(fGradient);
      Window.Create(this, uPaletteID);

      // Get the window rect so we can know how big this guy is.
      CRect crWindow;
      Window.GetWindowRect(crWindow);

      int nWidth = crWindow.Width();
      int nHeight = crWindow.Height();

      // Determine the position for the window.

      CPoint p;
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CToolBar* pEdit2Bar = pMainFrame->GetToolBar(TOOLBAR_EDIT2);
      if (pEdit2Bar->IsWindowVisible())
      {
         int nIndex = pEdit2Bar->CommandToIndex(uID);
         ASSERT(nIndex != -1);
         if (nIndex != -1)
         {
            CRect cr;
            pEdit2Bar->GetItemRect(nIndex, cr);
            pEdit2Bar->ClientToScreen(cr);
            ScreenToClient(cr);
            p.x = cr.right;
            p.y = (cr.top + cr.bottom - nHeight)/2;
         }
      }
      else
      {
         CRect cr;
         GetClientRect(cr);
         p.x = (cr.left + cr.right - nWidth)/2;
         p.y = (cr.top + cr.bottom - nHeight)/2;
      }

      ClientToScreen(&p);

      // Make sure palette window is on the screen.
      CRect crScreen;
#ifdef WIN32
      if (SystemParametersInfo(SPI_GETWORKAREA, 0, (LPRECT)crScreen, FALSE) == 0)
      {
         crScreen.SetRectEmpty();
      }
#else
      crScreen.SetRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif

      if (!crScreen.IsRectEmpty())
      {
         if (p.x+nWidth > crScreen.right) 
         {
            p.x = crScreen.right-nWidth;
         }
         if (p.y+nHeight > crScreen.bottom) 
         {
            p.y = crScreen.bottom-nHeight;
         }
         if (p.x < crScreen.left)
         {
            p.x = crScreen.left;
         }
         if (p.y < crScreen.top)
         {
            p.y = crScreen.top;
         }
      }

      // Position the window.
      Window.SetWindowPos(NULL, p.x, p.y, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);

      // Show the window.
      Window.ShowWindow(SW_SHOW);
   }
   else
   {
      // Destroy the window.
      Window.DestroyWindow();
   }
}

void CPmwView::OnFillColor()
{
   BOOL fHasGradient = FALSE;


	if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();
		FillColorWindow.SetColor(NewStyle.FillForegroundColor());
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());
      pDoc->GetSelectedProperties(Properties);

      COLOR Color = UNDEFINED_COLOR;
      Properties.ApplyFillForegroundColor(Color);
      FillColorWindow.SetColor(Color);

		// See if gradient button is available
		PMGPageObjectPtr pObject = pDoc->selected_objects();
		while (pObject != NULL &&
			pObject->type() != OBJECT_TYPE_Rectangle &&
			pObject->type() != OBJECT_TYPE_Ellipse   &&
			pObject->type() != OBJECT_TYPE_DrawingObject)
		{
			if (pObject->type() == OBJECT_TYPE_Component)
			{
				// TODO: change to get interface
				if (kPathComponent == ((ComponentObject *) pObject)->GetComponentType())
					break;
			}

			pObject = pObject->next_selected_object();
		}
		fHasGradient = (pObject != NULL);
   }

   OnModifyPalette(ID_FILL_COLOR, ID_FILL_COLOR_PALETTE, FillColorWindow, fHasGradient);
}

void CPmwView::OnFillColorPalette()
{
   if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();
      COLOR Color = TRANSPARENT_COLOR;
      if (!FillColorWindow.IsNone())
      {
         Color = FillColorWindow.GetColor();
      }
      NewStyle.FillForegroundColor(Color);
      ChangeFont(&NewStyle, IDCmd_FillColor);
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());

		RColor Color = FillColorWindow.GetRColor();
      COLOR BlendColor = TRANSPARENT_COLOR;
		SHORT nFillType = FillFormatV2::FillSolid;

      if (!FillColorWindow.IsNone())
      {
         BlendColor = FillColorWindow.GetBlendColor();
			nFillType = FillColorWindow.GetFillType();
      }

      Properties.SetFillGradientBlendColor(BlendColor, FALSE);
      Properties.SetFillType(nFillType, FALSE);
      Properties.SetFillForegroundColor(Color, FALSE);

      pDoc->SetSelectedProperties(Properties, this, IDCmd_FillColor);
   }
}

void CPmwView::OnUpdateFillColor(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(FillColorWindow.GetSafeHwnd() != NULL);
   pCmdUI->Enable(TRUE);
}

void CPmwView::OnLineWidth()
{
   if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();
      TextOutlineWidthWindow.SetWidth(NewStyle.OutlineStyle(), NewStyle.OutlineWidthType(), NewStyle.OutlineWidth());
      OnModifyPalette(ID_OUTLINE_WIDTH, ID_OUTLINE_WIDTH_PALETTE, TextOutlineWidthWindow);
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());
      pDoc->GetSelectedProperties(Properties);

      SHORT nStyle = -1;
      Properties.ApplyOutlineStyle(nStyle);

      SHORT nWidthType = -1;
      Properties.ApplyOutlineWidthType(nWidthType);

      CFixed lWidth = UNDEFINED_CFIXED;
      Properties.ApplyOutlineWidth(lWidth);

      OutlineWidthWindow.SetWidth(nStyle, nWidthType, lWidth);
      OnModifyPalette(ID_OUTLINE_WIDTH, ID_OUTLINE_WIDTH_PALETTE, OutlineWidthWindow);
   }
}

void CPmwView::OnLineWidthPalette()
{
   if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();

      SHORT nStyle;
      SHORT nWidthType;
      CFixed lWidth;
      TextOutlineWidthWindow.GetWidth(nStyle, nWidthType, lWidth);
      NewStyle.OutlineStyle(nStyle);
      NewStyle.OutlineWidthType(nWidthType);
      NewStyle.OutlineWidth(lWidth);
      if (nStyle != OutlineFormat::None)
      {
         COLOR Color = NewStyle.OutlineForegroundColor();
         if (Color == TRANSPARENT_COLOR)
         {
            Color = NewStyle.FillForegroundColor();
            Color = (Color != COLOR_BLACK) ? COLOR_BLACK : COLOR_LIGHT_GRAY;
            NewStyle.OutlineForegroundColor(Color);
         }
      }

      ChangeFont(&NewStyle, IDCmd_LineWidth);
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());
      SHORT nStyle;
      SHORT nWidthType;
      CFixed lWidth;
      OutlineWidthWindow.GetWidth(nStyle, nWidthType, lWidth);
      Properties.SetOutlineStyle(nStyle, FALSE);
      Properties.SetOutlineWidthType(nWidthType, FALSE);
      Properties.SetOutlineWidth(lWidth, FALSE);
      pDoc->SetSelectedProperties(Properties, this, IDCmd_LineWidth);
   }
}

void CPmwView::OnUpdateLineWidth(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck((OutlineWidthWindow.GetSafeHwnd() != NULL)
					  || (TextOutlineWidthWindow.GetSafeHwnd() != NULL)
					  || (OutlineColorWindow.GetSafeHwnd() != NULL));
   pCmdUI->Enable(TRUE);
}

void CPmwView::OnLineColor()
{
   if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();
      OutlineColorWindow.SetColor(NewStyle.OutlineForegroundColor());
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());
      pDoc->GetSelectedProperties(Properties);

      COLOR Color = UNDEFINED_COLOR;
      Properties.ApplyOutlineForegroundColor(Color);
      OutlineColorWindow.SetColor(Color);
   }

   OnModifyPalette(ID_OUTLINE_WIDTH, ID_OUTLINE_COLOR_PALETTE, OutlineColorWindow);
}

void CPmwView::OnLineColorPalette()
{
   if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();
      COLOR Color = TRANSPARENT_COLOR;
      if (!OutlineColorWindow.IsNone())
      {
         Color = OutlineColorWindow.GetColor();
      }
      NewStyle.OutlineForegroundColor(Color);

      if (Color != TRANSPARENT_COLOR)
      {
         if (NewStyle.OutlineStyle() == OutlineFormat::None)
         {
            NewStyle.OutlineSetType(OutlineFormat::OutlineMedium);
         }
      }

      ChangeFont(&NewStyle, IDCmd_LineColor);
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());
      COLOR Color = TRANSPARENT_COLOR;
      if (!OutlineColorWindow.IsNone())
      {
         Color = OutlineColorWindow.GetColor();
      }
      Properties.SetOutlineForegroundColor(Color, FALSE);
      pDoc->SetSelectedProperties(Properties, this, IDCmd_LineColor);
   }
}

void CPmwView::OnUpdateLineColor(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(OutlineColorWindow.GetSafeHwnd() != NULL);
   pCmdUI->Enable(TRUE);
}

void CPmwView::OnShadow()
{
   if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();
      TextShadowWindow.SetShadow(NewStyle.ShadowStyle(), NewStyle.ShadowOffsetType(), NewStyle.ShadowXOffset(), NewStyle.ShadowYOffset());
      OnModifyPalette(ID_SHADOW, ID_SHADOW_PALETTE, TextShadowWindow);
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());
      pDoc->GetSelectedProperties(Properties);

      SHORT nStyle = -1;
      Properties.ApplyShadowStyle(nStyle);

      SHORT nOffsetType = -1;
      Properties.ApplyShadowOffsetType(nOffsetType);

      CFixed lXOffset = UNDEFINED_CFIXED;
      Properties.ApplyShadowXOffset(lXOffset);

      CFixed lYOffset = UNDEFINED_CFIXED;
      Properties.ApplyShadowYOffset(lYOffset);

      ShadowWindow.SetShadow(nStyle, nOffsetType, lXOffset, lYOffset);
      OnModifyPalette(ID_SHADOW, ID_SHADOW_PALETTE, ShadowWindow);
   }
}

void CPmwView::OnShadowPalette()
{
   if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();

      SHORT nStyle;
      SHORT nWidthOffset;
      CFixed lXOffset;
      CFixed lYOffset;
      TextShadowWindow.GetShadow(nStyle, nWidthOffset, lXOffset, lYOffset);
      NewStyle.ShadowStyle(nStyle);
      NewStyle.ShadowOffsetType(nWidthOffset);
      NewStyle.ShadowXOffset(lXOffset);
      NewStyle.ShadowYOffset(lYOffset);
      COLOR Color = NewStyle.ShadowForegroundColor();
      if (Color == TRANSPARENT_COLOR)
      {
         Color = NewStyle.FillForegroundColor();
         Color = (Color != COLOR_BLACK) ? COLOR_BLACK : COLOR_LIGHT_GRAY;
         NewStyle.ShadowForegroundColor(Color);
      }

      ChangeFont(&NewStyle, IDCmd_Shadow);
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());
      SHORT nStyle;
      SHORT nOffsetType;
      CFixed lXOffset;
      CFixed lYOffset;
      ShadowWindow.GetShadow(nStyle, nOffsetType, lXOffset, lYOffset);
      Properties.SetShadowStyle(nStyle, FALSE);
      Properties.SetShadowOffsetType(nOffsetType, FALSE);
      Properties.SetShadowXOffset(lXOffset, FALSE);
      Properties.SetShadowYOffset(lYOffset, FALSE);
      pDoc->SetSelectedProperties(Properties, this, IDCmd_Shadow);
   }
}

void CPmwView::OnUpdateShadow(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck((ShadowWindow.GetSafeHwnd() != NULL)
					  || (ShadowWindow.GetSafeHwnd() != NULL)
					  || (ShadowColorWindow.GetSafeHwnd() != NULL));
   pCmdUI->Enable(TRUE);
}

void CPmwView::OnShadowColor()
{
   if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();
      ShadowColorWindow.SetColor(NewStyle.ShadowForegroundColor());
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());
      pDoc->GetSelectedProperties(Properties);

      COLOR Color = UNDEFINED_COLOR;
      Properties.ApplyShadowForegroundColor(Color);
      ShadowColorWindow.SetColor(Color);
   }

   OnModifyPalette(ID_SHADOW, ID_SHADOW_COLOR_PALETTE, ShadowColorWindow);
}

void CPmwView::OnShadowColorPalette()
{
   if (m_Txp.Valid())
   {
      CTextStyle NewStyle = m_Txp.SomethingSelected() ? m_Txp.SelectStyle() : m_Txp.Style();
      COLOR Color = TRANSPARENT_COLOR;
      if (!ShadowColorWindow.IsNone())
      {
         Color = ShadowColorWindow.GetColor();
      }
      NewStyle.ShadowForegroundColor(Color);

      if (Color != TRANSPARENT_COLOR)
      {
         if (NewStyle.ShadowStyle() == ShadowFormat::None)
         {
            NewStyle.ShadowSetType(ShadowFormat::ShadowDropDownRight);
         }
      }

      ChangeFont(&NewStyle, IDCmd_ShadowColor);
   }
   else
   {
      CPmwDoc* pDoc = GetDocument();
      CObjectProperties Properties(pDoc->get_database());
      COLOR Color = TRANSPARENT_COLOR;
      if (!ShadowColorWindow.IsNone())
      {
         Color = ShadowColorWindow.GetColor();
      }
      Properties.SetShadowForegroundColor(Color, FALSE);
      pDoc->SetSelectedProperties(Properties, this, IDCmd_ShadowColor);
   }
}

void CPmwView::OnUpdateShadowColor(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(ShadowColorWindow.GetSafeHwnd() != NULL);
   pCmdUI->Enable(TRUE);
}

void CPmwView::OnDrawingObject()
{
   if (ShapeWindow.GetSafeHwnd() == NULL)
   {
      if (edit_event_handler == mode_create && m_nTypeToCreate == OBJECT_TYPE_DrawingObject)
      {
         // Tool is already on. Turn it off.
         OnTool(OBJECT_TYPE_DrawingObject);
      }
      else
      {
         // Turn the tool on.
         OnTool(OBJECT_TYPE_DrawingObject);

         // Create the window.

         ShapeWindow.SetOwner(this);
         ShapeWindow.Create(this, ID_SHAPE_PALETTE);

         // Get the window rect so we can know how big this guy is.
         CRect crWindow;
         ShapeWindow.GetWindowRect(crWindow);

         int nWidth = crWindow.Width();
         int nHeight = crWindow.Height();

         // Determine the position for the window.

         CPoint p;
         CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
			CToolBar* pEdit2Bar = pMainFrame->GetToolBar(TOOLBAR_EDIT2);
         if (pEdit2Bar->IsWindowVisible())
         {
            int nIndex = pEdit2Bar->CommandToIndex(ID_PICTURES_DRAWING_OBJECT);
            ASSERT(nIndex != -1);
            if (nIndex != -1)
            {
               CRect cr;
               pEdit2Bar->GetItemRect(nIndex, cr);
               pEdit2Bar->ClientToScreen(cr);
               ScreenToClient(cr);
               p.x = cr.right;
               p.y = (cr.top + cr.bottom - nHeight)/2;
            }
         }
         else
         {
            CRect cr;
            GetClientRect(cr);
            p.x = (cr.left + cr.right - nWidth)/2;
            p.y = (cr.top + cr.bottom - nHeight)/2;
         }

         ClientToScreen(&p);

         // Make sure window is on the screen.
         CRect crScreen;
#ifdef WIN32
         if (SystemParametersInfo(SPI_GETWORKAREA, 0, (LPRECT)crScreen, FALSE) == 0)
         {
            crScreen.SetRectEmpty();
         }
#else
         crScreen.SetRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif

         if (!crScreen.IsRectEmpty())
         {
            if (p.x+nWidth > crScreen.right) 
            {
               p.x = crScreen.right-nWidth;
            }
            if (p.y+nHeight > crScreen.bottom) 
            {
               p.y = crScreen.bottom-nHeight;
            }
            if (p.x < crScreen.left)
            {
               p.x = crScreen.left;
            }
            if (p.y < crScreen.top)
            {
               p.y = crScreen.top;
            }
         }

         // Position the window.
         ShapeWindow.SetWindowPos(NULL, p.x, p.y, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);

         // Show the window.
         ShapeWindow.ShowWindow(SW_SHOW);
      }
   }
   else
   {
      // Destroy the window.
      ShapeWindow.DestroyWindow();
   }
}

void CPmwView::OnUpdateDrawingObject(CCmdUI* pCmdUI)
{
   OnUpdateTool(pCmdUI, OBJECT_TYPE_DrawingObject);
}

void CPmwView::OnTextShape()
{
   if (TextShapeWindow.GetSafeHwnd() == NULL)
   {
      // Create the window.
      TextShapeWindow.SetOwner(this);
      TextShapeWindow.Create(this, ID_TEXT_SHAPE_PALETTE);

      // Get the window rect so we can know how big this guy is.
      CRect crWindow;
      TextShapeWindow.GetWindowRect(crWindow);

      int nWidth = crWindow.Width();
      int nHeight = crWindow.Height();

      // Determine the position for the window.

      CPoint p;
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CToolBar* pFormatBar = pMainFrame->GetToolBar(TOOLBAR_FORMAT);

      if (pFormatBar->IsWindowVisible())
      {
         int nIndex = pFormatBar->CommandToIndex(ID_TEXT_SHAPE);
         ASSERT(nIndex != -1);
         if (nIndex != -1)
         {
            CRect cr;
            pFormatBar->GetItemRect(nIndex, cr);
            pFormatBar->ClientToScreen(cr);
            ScreenToClient(cr);
            p.x = (cr.left + cr.right - nWidth)/2;
            p.y = cr.bottom;
         }
      }
      else
      {
         CRect cr;
         GetClientRect(cr);
         p.x = (cr.left + cr.right - nWidth)/2;
         p.y = (cr.top + cr.bottom - nHeight)/2;
      }

      ClientToScreen(&p);

      // Make sure window is on the screen.
      CRect crScreen;
#ifdef WIN32
      if (SystemParametersInfo(SPI_GETWORKAREA, 0, (LPRECT)crScreen, FALSE) == 0)
      {
         crScreen.SetRectEmpty();
      }
#else
      crScreen.SetRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif

      if (!crScreen.IsRectEmpty())
      {
         if (p.x+nWidth > crScreen.right) 
         {
            p.x = crScreen.right-nWidth;
         }
         if (p.y+nHeight > crScreen.bottom) 
         {
            p.y = crScreen.bottom-nHeight;
         }
         if (p.x < crScreen.left)
         {
            p.x = crScreen.left;
         }
         if (p.y < crScreen.top)
         {
            p.y = crScreen.top;
         }
      }

      // Position the window.
      TextShapeWindow.SetWindowPos(NULL, p.x, p.y, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);

      // Set the default value.
      CPmwDoc* pDoc = GetDocument();
      PMGDatabase* pDatabase = pDoc->get_database();

      CObjectProperties Properties(pDatabase);
      pDoc->GetSelectedProperties(Properties);

      DB_RECORD_NUMBER lWarpField;

      if (Properties.ApplyWarpField(lWarpField))
      {
         if (lWarpField == 0)
         {
            TextShapeWindow.SetNone();
         }
         else
         {
            WarpFieldRecord* pRecord = (WarpFieldRecord*)pDatabase->get_record(lWarpField, NULL, RECORD_TYPE_WarpField);
            if (pRecord != NULL)
            {
               TextShapeWindow.SetShape(pRecord->Name());
               pRecord->release();
            }
         }
      }
      else
      {
         TextShapeWindow.SetShape(NULL);
      }

      // Show the window.
      TextShapeWindow.ShowWindow(SW_SHOW);
   }
   else
   {
      // Destroy the window.
      TextShapeWindow.DestroyWindow();
   }
}

void CPmwView::OnTextShapePalette()
{
   CPmwDoc* pDoc = GetDocument();
   PMGDatabase* pDatabase = pDoc->get_database();
   CObjectProperties Properties(pDatabase);

   DB_RECORD_NUMBER lWarpField = 0;
   if (!TextShapeWindow.IsNone())
   {
      CWarpField* pWarpField = TextShapeWindow.GetShape();
      if (pWarpField != NULL)
      {
         WarpField_CreateStruct cs;

         cs.m_pszName = pWarpField->Name();
         pWarpField->OriginalBounds()->ToData(cs.m_BoundsData);

         lWarpField = pDatabase->new_warp_field_record(&cs);
      }
   }
   Properties.SetWarpField(lWarpField, FALSE);

   if (lWarpField != 0)
   {
      pDatabase->free_warp_field_record(lWarpField);
   }

   pDoc->SetSelectedProperties(Properties, this, IDCmd_TextShape);
}

void CPmwView::OnUpdateTextShape(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(TextShapeWindow.GetSafeHwnd() != NULL);
   pCmdUI->Enable(TRUE);
}

void CPmwView::OnNextObject()
{
   if (edit_event_handler == mode_resize)
   {
      GetDocument()->SelectNextObject();
   }
}

void CPmwView::OnUpdateNextObject(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(edit_event_handler == mode_resize && GetDocument()->CanSelectNextObject());
}

void CPmwView::OnPreviousObject()
{
   if (edit_event_handler == mode_resize)
   {
      GetDocument()->SelectPreviousObject();
   }
}

void CPmwView::OnUpdatePreviousObject(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(edit_event_handler == mode_resize && GetDocument()->CanSelectPreviousObject());
}

void CPmwView::OnRulers()
{
   GET_PMWAPP()->SetShowRulers(!GET_PMWAPP()->ShowRulers());
}

void CPmwView::OnUpdateRulers(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GET_PMWAPP()->ShowRulers());
   pCmdUI->Enable(TRUE);
}

void CPmwView::OnSnapToRulers()
{
   GET_PMWAPP()->SetSnapToRulers(!GET_PMWAPP()->SnapToRulers());
}

void CPmwView::OnUpdateSnapToRulers(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GET_PMWAPP()->SnapToRulers());
   pCmdUI->Enable(GET_PMWAPP()->ShowRulers());
}

void CPmwView::OnGuides()
{
   GET_PMWAPP()->SetShowGuides(!GET_PMWAPP()->ShowGuides());
}

void CPmwView::OnUpdateGuides(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GET_PMWAPP()->ShowGuides());
   pCmdUI->Enable(TRUE);
}

void CPmwView::OnSnapToGuides()
{
   GET_PMWAPP()->SetSnapToGuides(!GET_PMWAPP()->SnapToGuides());
}

void CPmwView::OnUpdateSnapToGuides(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GET_PMWAPP()->SnapToGuides());
//   pCmdUI->Enable(GET_PMWAPP()->ShowGuides());
}

void CPmwView::OnLockGuides()
{
   GET_PMWAPP()->SetLockGuides(!GET_PMWAPP()->GuidesLocked());
}

void CPmwView::OnUpdateLockGuides(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GET_PMWAPP()->GuidesLocked());
//   pCmdUI->Enable(GET_PMWAPP()->ShowGuides());
}

void CPmwView::AddGuide(int nType)
{
	// Create a horizontal or vertical guide, centered on the
	// current visible part of the workspace.

	CPmwDoc* pDoc = GetDocument();

	Array* pGuides = NULL;
	PBOX pbox = rc.source_pbox;
	PPNT pagePos;
	if (nType == GUIDE_HORIZ)
	{
		pGuides = pDoc->get_horiz_guides();
		pagePos.x = 0;
		pagePos.y = (pbox.y0 + pbox.y1) / 2;
	}
	else
	{
		pGuides = pDoc->get_vert_guides();
		pagePos.x = (pbox.x0 + pbox.x1) / 2;
		pagePos.y = 0;
	}

	if (pGuides == NULL)
	{
		ASSERT(0);
		return;
	}

	CPoint pt;
	rc.ppnt_to_screen(pagePos, &pt);

	// add guide to array
	DWORD dwPos = (nType == GUIDE_HORIZ) ? pagePos.y : pagePos.x;
	int nIndex = pGuides->count();
	pGuides->add_element((void*)&dwPos);
	DrawGuideLine(&rc, nType, pt, ShowGuides() ? TRUE : FALSE);
	pDoc->SetModifiedFlag();

	// Create the undo/redo command for this action
	int undoID = IDCmd_GuideCreate;
	CCmdGuideChange* pGuideUndo = new CCmdGuideChange(undoID);
	ASSERT(pGuideUndo);
	pGuideUndo->Setup(this, nType,
							pDoc->CurrentPageIndex(), pDoc->get_current_panel(),
							0, -1,
							dwPos, nIndex);

	pDoc->AddCommand(pGuideUndo, FALSE);
}

void CPmwView::OnAddHorizontalGuide()
{
	AddGuide(GUIDE_HORIZ);
}

void CPmwView::OnAddVerticalGuide()
{
	AddGuide(GUIDE_VERT);
}

//////////////////////////////////////////////////////////////////////////////
// SnapPosition()
//
// Does both SnapToRulers and SnapToGuides. SnapToGuides takes precedence.
// If we hit a guide, we won't snap to ruler in that direction.
//
// Returns: bits telling whether we snapped to GUIDE_HORIZ and/or GUIDE_VERT
//
//////////////////////////////////////////////////////////////////////////////
int CPmwView::SnapPosition(PPNT* p)
{
   int fSnapGuideX = 0;
   int fSnapGuideY = 0;

   if (SnapToGuides())
   {
		CPmwDoc* pDoc = GetDocument();

		// Get the master page items (for the guides contained therein).
		ObjectList* pMasterItems = pDoc->GetMasterItems();

      // Snap to guides
      POINT pt;
      rc.ppnt_to_screen(*p, &pt);

      int nIndex;
		Array* pGuides;

		// Try to snap to a horizontal guide (page or master page).
		if ((nIndex = PtOnHGuide(pt,
										 pGuides = pDoc->get_horiz_guides(),
										 GUIDE_AREA_SNAP)) != -1
			|| (pMasterItems != NULL
					&& (nIndex = PtOnHGuide(pt,
										 pGuides = pMasterItems->GetHorizGuides(),
										 GUIDE_AREA_SNAP)) != -1))
		{
			p->y = *((DWORD*)pGuides->get_element(nIndex));
         fSnapGuideY = 0xFF;
		}

		// Try to snap to a vertical guide (page or master page).
		if ((nIndex = PtOnVGuide(pt,
										 pGuides = pDoc->get_vert_guides(),
										 GUIDE_AREA_SNAP)) != -1
			|| (pMasterItems != NULL
					&& (nIndex = PtOnVGuide(pt,
										 pGuides = pMasterItems->GetVertGuides(),
										 GUIDE_AREA_SNAP)) != -1))
      {
         p->x = *((DWORD*)pGuides->get_element(nIndex));
         fSnapGuideX = 0xFF;
      }
   }

   if (ShowRulers() && SnapToRulers())
   {
      // Snap to the rulers.
#ifdef LOCALIZE
	  if (CurrentLocale.UseMetric())
	  { 
// handle snapping to metric rulers
// A bit messy, but seems to work
			PCOORD lLastUnit;
			double dLastUnit;
			double dx,dy;
			long NearestMark;

			if (m_pXRuler != NULL && (lLastUnit = m_pXRuler->GetLastUnit()) && ((dLastUnit = m_pXRuler->GetLastdUnit())))
			{
				if (!fSnapGuideX)
				{
					// find nearest mark
					dx =  ((double)(p->x - m_pXRuler->GetLastBaseX0()))/dLastUnit;
					NearestMark = Round(dx);
					// calculate coordinate of nearest mark
					p->x = m_pXRuler->GetLastBaseX0() + (PCOORD) Round((double)NearestMark*dLastUnit);
				}
			}
			if (m_pYRuler != NULL && (lLastUnit = m_pYRuler->GetLastUnit()) && ((dLastUnit = m_pYRuler->GetLastdUnit())))
			{
				if (!fSnapGuideY)
				{
					// find nearest mark
					dy =  ((double)(p->y - m_pYRuler->GetLastBaseY0()))/dLastUnit;
					NearestMark = Round(dy);
					// calculate coordinate of nearest mark
					p->y = m_pYRuler->GetLastBaseY0() + (PCOORD) Round((double)NearestMark*dLastUnit);
				}
			}
	  }
	  else // if metric is not used
#endif
	  {
      	PCOORD lLastUnit;
      	if (m_pXRuler != NULL && (lLastUnit = m_pXRuler->GetLastUnit()) != 0)
      	{
         		if (!fSnapGuideX) // only snap if didn't snap to guide
         		{
            		PCOORD x = p->x + lLastUnit/2;
            		p->x = x - (x % lLastUnit);
         		}
      	}
      	if (m_pYRuler != NULL && (lLastUnit = m_pYRuler->GetLastUnit()) != 0)
      	{
         		if (!fSnapGuideY)
         		{
            		PCOORD y = p->y + lLastUnit/2;
            		p->y = y - (y % lLastUnit);
         		}
      	}
   	  }
   }
   return (fSnapGuideX & GUIDE_VERT) | (fSnapGuideY & GUIDE_HORIZ);
}

void CPmwView::OnTextBoxOutlines()
{
   GET_PMWAPP()->SetTextBoxOutlines(!GET_PMWAPP()->ShowTextBoxOutlines());
}

void CPmwView::OnUpdateTextBoxOutlines(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(GET_PMWAPP()->ShowTextBoxOutlines());
}

void CPmwView::OnCrop()
{
   if (edit_event_handler == mode_crop)
   {
      set_arrow_tool();
   }
   else
   {
      set_mode(mode_crop);
   }
}

void CPmwView::OnUpdateCrop(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(edit_event_handler == mode_crop);
   pCmdUI->Enable(CanCrop());
}

void CPmwView::OnResetCropping()
{
   BOOL fEnable = CanCrop();
   if (fEnable)
   {
      CCmdChangeSelects* pCommand = new CCmdChangeSelects(IDCmd_ResetCropping);

      if (!pCommand->Before(this))
      {
         delete pCommand;
         pCommand = NULL;
      }

      CPmwDoc* pDoc = GetDocument();
      PMGPageObject* pObject = pDoc->selected_objects();
      CDoubleRect& VisibleRect = pObject->GetVisibleRect();
      // Toggle the select off.
      pDoc->toggle_object(pObject);

      // Reset the visible rect.
      VisibleRect.left = 0.0;
      VisibleRect.top = 0.0;
      VisibleRect.right = 1.0;
      VisibleRect.bottom = 1.0;

      // Toggle the select back on.
      pDoc->toggle_object(pObject);
      // Update the appearance of the object.
      pObject->changed_image();
      pDoc->refresh_object(pObject);

      if (pCommand != NULL)
      {
         if (pCommand->After())
         {
            // Add the command for undoing later.
            // No execute!
            pDoc->AddCommand(pCommand, FALSE);
         }
         else
         {
            delete pCommand;
         }
      }
   }
}

void CPmwView::OnUpdateResetCropping(CCmdUI* pCmdUI)
{
   BOOL fEnable = CanCrop();
   if (fEnable)
   {
      PMGPageObject* pObject = GetDocument()->selected_objects();
      CDoubleRect& VisibleRect = pObject->GetVisibleRect();
      if (VisibleRect.left == 0.0
          && VisibleRect.top == 0.0
          && VisibleRect.right == 1.0
          && VisibleRect.bottom == 1.0)
      {
         fEnable = FALSE;
      }
   }
   pCmdUI->Enable(fEnable);
}

/////////////////////////////////////////////////////////////////////////////
// TWAIN Support

void CPmwView::OnAcquire() 
{
   CModalTwain  Twain;
   HWND    hWndParent;
   CPmwDoc *pDoc = GetDocument();

#ifdef WIN32
   hWndParent = GetSafeOwner(NULL)->GetSafeHwnd();
#else
   hWndParent = _AfxGetSafeOwner(NULL);
#endif

   if (!Twain.IsSourceAvailable(hWndParent))
   {
      AfxMessageBox(IDS_NOTWAINSOURCE);
      return;
   }

   HANDLE hDib = Twain.Acquire(hWndParent);
   if (hDib == NULL)
   {
      // If user cancelled operation, don't display error message
      if (Twain.m_ErrorInfo.m_nCode == CTRC_XFER_CANCELED)
         return;

      Twain.DisplayError();
   }
   else
   {		
		BITMAPINFO* pBmi;
		if ((pBmi = (BITMAPINFO*)GlobalLock(hDib)) != NULL)
		{
			GraphicObject* pObject;
			//create a unique name
			char cbName[30];
			clipboard.SetUpdateTime();
			clipboard.BuildPasteName(".BMP", cbName);
			
			//create the graphic object
			CreateBMPFromDIB(cbName, pDoc->get_database(), pBmi, pObject);
			pDoc->handle_new_object(pObject);

			GlobalUnlock(hDib);
		}    
		GlobalFree(hDib);
		
		pDoc->free_import_picture_name();
   }
}

void CPmwView::OnUpdateAcquire(CCmdUI* pCmdUI) 
{
   CModalTwain Twain;
   pCmdUI->Enable(Twain.IsManagerInstalled());
}

void CPmwView::OnSelectSource() 
{
   CModalTwain Twain;

   if (!Twain.IsSourceAvailable(GetSafeHwnd()))
   {
      AfxMessageBox(IDS_NOTWAINSOURCE);
   }
   else
   {
      if (Twain.OpenManager(GetSafeHwnd()))
      {
         Twain.SelectSource();
         Twain.CloseManager();
      }
   }
}

void CPmwView::OnUpdateSelectSource(CCmdUI* pCmdUI) 
{
   CModalTwain Twain;
   pCmdUI->Enable(Twain.IsManagerInstalled());
}

/////////////////////////////////////////////////////////////////////////////
// NFX handling

void CPmwView::OnNfxToon()
{
   // Do we support this?
   if (!GetConfiguration()->SupportsCartoonMaker())
   {
      return;
   }

	// Helpful Hint
	if (!GET_PMWAPP()->ShowHelpfulHint("Cartoon-o-Matic", NULL, TRUE))
	{
		return;
	}
   
   CCartoonOMatic com;
   if (com.NewToon())
   {
      // The user has chosen a model to use. We want to create a graphic to
      // house it.

      // We need to set up a device (ReadOnlyFile) to source the data.
      ReadOnlyFile Source(com.GetOutputFile());
      
      // Build the graphic creation structure.
      // We want to embed source (it will use our device).

      char cbName[30];
      clipboard.SetUpdateTime();
      clipboard.BuildPasteName(".CGM", cbName);

      GRAPHIC_CREATE_STRUCT gcs;

      gcs.m_csFileName = cbName;
      gcs.embed_source = TRUE;
      gcs.pSourceDevice = &Source;

      CPmwDoc* pDoc = GetDocument();
      PMGDatabase* pDatabase = pDoc->get_database();

      GraphicObject* pObject;

      if ((pObject = pDatabase->create_graphic_object(&gcs)) != NULL)
      {
         pDoc->handle_new_object(pObject);
      }
   }
}

void CPmwView::OnUpdateNfxToon(CCmdUI* pCmdUI)
{
   if(GetConfiguration()->Product() != CPrintMasterConfiguration::plPrintMaster)
         pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
#ifdef LOCALIZE
   else if (GetConfiguration()->RemoveCartoonMaker())
   {
		if (pCmdUI->m_pMenu)
		   pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
   }
#endif
}


#if 0		// replaced by Photo Workshop
/*
// Handle the Photo Enhancer function.
*/

static char cbPhotoEnhancerTemplate[5+1] = "~xxxP";

/////////////////////////////////////////////////////////////////////////////
// PhotoEnhancer support.

void CPmwView::OnPhotoEnhancer()
{
   // Do we support this?
   if (!GetConfiguration()->SupportsPhotoEditor())
   {
      return;
   }

	// Helpful Hint
	CString csPhoto("PhotoEnhancer");
	if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
		csPhoto = "Photo Editor";
	if (!GET_PMWAPP()->ShowHelpfulHint(csPhoto, NULL, TRUE))
	{
		return;
	}
   
   CPmwDoc* pDoc = GetDocument();
   PMGPageObject* pObject = pDoc->selected_objects();
   ASSERT(pObject != NULL);
   ASSERT(pObject->type() == OBJECT_TYPE_Graphic);

   // Get the graphic record of the selected picture.
   GraphicRecord* pGraphic = ((GraphicObject*)pObject)->LockGraphicRecord();
   if (pGraphic != NULL)
   {
      // Nid the filename (in case it has not been yet).
      GET_PMWAPP()->nid_filename(cbPhotoEnhancerTemplate);

      // Extract the file to a temp file.
      TmpFile EditFile(cbPhotoEnhancerTemplate);
      if (EditFile.initialize() == ERRORCODE_None)
      {
				BOOL bContinue = TRUE;
				
				if (pGraphic->record.type == GRAPHIC_TYPE_WMF || pGraphic->record.type == GRAPHIC_TYPE_CGM)
				{
					
					if (MessageBox ("Image will be converted to BMP format.", "Warning", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
						bContinue = ((GraphicObject*)pObject)->WriteVectorAsBMP(&EditFile, pDoc) == ERRORCODE_None;
					else 
						bContinue = FALSE;
				}
				else
					bContinue = pGraphic->WriteImageAsBMP(&EditFile) == ERRORCODE_None;
         
			if (bContinue)
         {
            // Close the file so we can access it totally.
            EditFile.flush();

            // Get the initial time stamp.
            CTimeStamp FileStartTime;
				FileStartTime.SetFileModify(EditFile.get_name());

            // Build the command line.
            CString csCommandLine;
            if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
               csCommandLine = pDoc->GetPathManager()->ExpandPath("PHOTOED\\PhEdit32.EXE /mindscape");
            else
               csCommandLine = pDoc->GetPathManager()->ExpandPath("PHOTOENP\\PHOTOEN3.EXE");
            csCommandLine += ' ';
            csCommandLine += EditFile.get_name();

            CWnd* pMainFrm = AfxGetMainWnd();

            ((CMainFrame*)pMainFrm)->BeginModalState(TRUE);

            // Execute Photo Enhancer.
            if (Util::DoWinExec(csCommandLine))
            {
               // Wait for the window to come up.
               CString PhotoEnhancerTitle;
               if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
                  PhotoEnhancerTitle.LoadString (IDS_PHOTO_EDITOR_TITLE);
               else
                  PhotoEnhancerTitle.LoadString (IDS_PHOTO_ENHANCER_TITLE);
               Util::WaitForWindow (PhotoEnhancerTitle, 60000L);

               // Re-enable the main window.
               ((CMainFrame*)pMainFrm)->EndModalState(TRUE);

               // Make sure we are on top and active.
#ifdef WIN32
               pMainFrm->SetForegroundWindow();
#endif
               pMainFrm->BringWindowToTop();
               pMainFrm->UpdateWindow();

               // Get the ending time stamp.
               // We default to dwStartTime-1 so that if the time stamp fails,
               // we will always assume modification.
               CTimeStamp FileEndTime;
					FileEndTime.SetFileModify(EditFile.get_name());

               // See if things changed.
               if ((!FileEndTime.IsValid()) || (FileEndTime != FileStartTime))
               {
                  // We need to update the graphic with the new file.
                  // Originally, I wanted to re-use the same graphic record
                  // under certain circumstances (already edited, single
                  // reference count, etc.). It turns out that it is much
                  // easier just to create a new graphic, and let the normal
                  // graphic mechanism handle it.
                  // (Note: This also allows undo.)
                  GRAPHIC_CREATE_STRUCT gcs;
                  // Build the name to use.
                  char cbName[30];
                  clipboard.SetUpdateTime();
                  clipboard.BuildPasteName(".BMP", cbName);
                  // Fill out the graphic create structure.
                  CString csFriendlyName = pGraphic->m_csName;
                  gcs.m_csFileName = cbName;
                  gcs.m_csFriendlyName = csFriendlyName;
                  gcs.embed_source = FALSE;
                  gcs.m_fCreateImage = TRUE;
                  gcs.pSourceDevice = &EditFile;
                  // Release the graphic before the replace.
                  pGraphic->release();
                  pGraphic = NULL;

                  AfxGetApp()->BeginWaitCursor();
                  // Do the standard replace.
                  if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
                     pDoc->DoReplacePicture((GraphicObject*)pObject, &gcs, TRUE, IDCmd_PhotoEditor);
                  else
                     pDoc->DoReplacePicture((GraphicObject*)pObject, &gcs, TRUE, IDCmd_PhotoEnhancer);
                  AfxGetApp()->EndWaitCursor();
               }
            }
            else
            {
               // Re-enable the main window.
               ((CMainFrame*)pMainFrm)->EndModalState(TRUE);

               // Failed to launch photo editing program

               if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
                  AfxMessageBox(IDS_PHOTO_EDITOR_NO_START);
               else
                  AfxMessageBox(IDS_PHOTO_ENHANCER_NO_START);
            }
         }
      }

      // If we have not already released the graphic, do it now.
      if (pGraphic != NULL)
      {
         pGraphic->release();
      }
   }
}

/*
// Update the Photo Enhancer button.
*/

void CPmwView::OnUpdatePhotoEnhancer(CCmdUI* pCmdUI)
{
#ifdef LOCALIZE
   if (GetConfiguration()->RemovePhotoEditor())
   {
	  if (pCmdUI->m_pMenu)
			pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
      return;
   }
#endif

   BOOL fEnable = FALSE;

   PMGPageObject* pObject = GetDocument()->selected_objects();
   if (pObject != NULL
       && pObject->next_selected_object() == NULL
       && pObject->type() == OBJECT_TYPE_Graphic)
   {
      GraphicRecord* pGraphic = ((GraphicObject*)pObject)->LockGraphicRecord();
      if (pGraphic != NULL)
      {
         // We have the graphic record. See what type it is.
         switch (pGraphic->record.type)
         {
            case GRAPHIC_TYPE_TIFF:
            case GRAPHIC_TYPE_JPEG:
            case GRAPHIC_TYPE_BMP:
            case GRAPHIC_TYPE_PCX:
            case GRAPHIC_TYPE_GIF:
            case GRAPHIC_TYPE_HALO:
            case GRAPHIC_TYPE_PCD:
            case GRAPHIC_TYPE_PSD:
				case GRAPHIC_TYPE_WMF:
				case GRAPHIC_TYPE_CGM:
				case GRAPHIC_TYPE_PNG:
            {
               fEnable = TRUE;
               break;
            }
            default:
            {
               break;
            }
         }
         // Release the record we got.
         pGraphic->release();
      }
   }

   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
   {
      CString  csPhotoEditor;
      csPhotoEditor.LoadString(IDS_PHOTO_EDITOR_TITLE);
      pCmdUI->SetText(csPhotoEditor);
   }
   pCmdUI->Enable(fEnable);
}
#endif	//	#if 0 - replaced by Photo Workshop


///////////////////////////////////////////////////////////////////////////////////////
// add a page(s) to the existing calendar project
void CPmwView::OnAddCalendarPage() 
{
   CPmwDoc* pDocument = GetDocument();
   set_arrow_tool();
	
	short style = ((CCalendarDoc*)pDocument)->GetPictureStyle();

	BOOL bPic = style == CAL_PICTURE_TOP || style == CAL_PICTURE_LEFT;
   CAddCalendarPageDialog Dialog (((CCalendarDoc*)pDocument)->NumberOfPrinterPages(), bPic);

   if (Dialog.DoModal() == IDOK)
   {
		DWORD dwPages = Dialog.m_iAddNumber;  // number of new pages to add
		DWORD dwTotalPages = pDocument->NumberOfPages(); // total pages in project
		DWORD dwAtPage = pDocument->CurrentPageIndex(); // curren working page
		PMGDatabase* pDatabase = pDocument->get_database(); // current database
		PageRecord* pSourcePage = NULL;  // source page for info/possible template 
		PageRecord* pNewPage = NULL; // new page 
		BOOL bEnd = Dialog.m_iWhere == 1;
		BOOL bCopy = Dialog.m_iContents == 1;
		short ps = ((CCalendarDoc*)pDocument)->GetPictureStyle();
		BOOL bPictures =  ps == CAL_PICTURE_TOP || ps == CAL_PICTURE_LEFT;
		DB_RECORD_NUMBER dbPage;
		
		// decide where to insert new pages, and pull the proper page
		if (bEnd)  // insert at end
		{
			// get last page
			if (bPictures)  // use 2nd to last page
			{
				dwTotalPages--;
				dwAtPage  = pDocument->NumberOfPages() -1; // total pages in project;
			}
			else
				dwAtPage  = pDocument->NumberOfPages(); // total pages in project;

			dbPage = dwTotalPages - 1;
		}
		else  // insert at beginning
		{
			dbPage = 0;
			if (bPictures)
			{
				dbPage += 1;
				dwAtPage = 1; // insert at beginning
			}
			else
				dwAtPage = 0; // insert at beginning
		}
			
		// get calendar from template source page and get calendar info
		CALENDAR_INFO ciCalInfo;
		PMGPageObject* pObject = NULL;
		// get source page
		if (!bCopy)
		{
			pSourcePage = (PageRecord*)pDatabase->get_record(pDocument->GetPage(dbPage), NULL, RECORD_TYPE_Page);
		}
		else 
		{
			pSourcePage = (PageRecord*)pDatabase->get_record(pDocument->GetPage(Dialog.m_iCopyPage), NULL, RECORD_TYPE_Page);
		}

	  //ASSERT(pSourcePage != NULL);

		CTextStyle LabelFontStyle;
		CTextStyle NumberFontStyle;
		BOOL fSetStyles = FALSE;

		if (pSourcePage != NULL) // get calendar object from page
		{
			int index = 0;
			pObject = (PMGPageObject*)pSourcePage->objects()->first_object();
			// loop thru and find a calendar to get properties from
			do
			{
				if (pObject->type() == OBJECT_TYPE_Calendar)
				{
					// copy calendar info
					Calendar cal = ((CalendarObject*)pObject)->get_calendar();
					ciCalInfo = cal.get_info();
					LabelFontStyle = cal.LabelFontStyle();
					NumberFontStyle = cal.NumberFontStyle();
					fSetStyles = TRUE;
					// get start date from date source page
					CalendarObject * pSrc = ((CCalendarDoc*)GetDocument())->find_calendar_object(dbPage);
					if (pSrc != NULL)
					{
						Calendar t = pSrc->get_calendar();
						ciCalInfo.year = t.get_info().year;
						ciCalInfo.month = t.get_info().month;
						ciCalInfo.week = t.get_info().week;
					}
					break;
				}
				else 
					pObject = (PMGPageObject*)pObject->next_object();
			} while (pObject != NULL);
		}
		
		DWORD dwStartPage = dwAtPage;
		int numsteps;
		
		if (ciCalInfo.style.type == YEARLY) 
			numsteps = ((int)dwPages * 12) + 1;
		else 
			numsteps = (int)dwPages +1;

		CString strDlgTitle = "Adding page(s)...";
		CCalendarProgressDlg dlg (numsteps, strDlgTitle);
		dlg.Create(IDD_CALENDAR_PROGRESS);
		dlg.ShowWindow(SW_SHOW);
		dlg.Step();
		BeginWaitCursor();
		// create new pages
		while (dwPages-- != 0)
		{
			// insert new page
			if (!pDocument->AddPage(dwAtPage))
			{
				break;
			}

			// Goto the page so created objects
			pDocument->GotoPage(dwAtPage);

			// We do not really need to do hwta follows now that the page is loaded,
			// but I left it that way for now since it worked beyond the page
			// loading problem. TODO: Change this to use the loaded page without
			// locking the record directly.

			// Lock the new page.
			pNewPage = (PageRecord*)pDatabase->get_record(pDocument->GetPage(dwAtPage), NULL, RECORD_TYPE_Page);

			if (pNewPage != NULL)
			{
				// increment date
				if (ciCalInfo.style.type != YEARLY)
				{
					IncrementCalendarDate(&ciCalInfo, (Dialog.m_iWhere == 1)); 
				}
				else 
				{
					if (Dialog.m_iWhere == 1)
						ciCalInfo.year++;
					else
						ciCalInfo.year--;
				}

				if (pSourcePage != NULL) // creating a default calendar object for the page
				{
					// put a calendar on it
					CalendarObject * pNewCal = NULL;
					ObjectList ol (NULL);					
				
					if (!bCopy) // not copying, creating new calendars
					{
						// build calendar create struct
						CALENDAR_CREATE_STRUCT ccs;
						ccs.info = &ciCalInfo;
						ccs.picture_style = ((CCalendarDoc*)pDocument)->GetPictureStyle();
						if (fSetStyles)
						{
							ccs.m_pLabelFontStyle = &LabelFontStyle;
							ccs.m_pNumberFontStyle = &NumberFontStyle;
						}
						//PPNT dim = pDocument->get_dimensions();
						((CCalendarDoc*)pDocument)->CalcCalendarSize(&ccs.bound);
						ccs.full_build = TRUE;

						if (ciCalInfo.style.type == YEARLY) 
						{
							// create and add yearly calendar to new page
							((CCalendarDoc*)pDocument)->CreateYearlyCalendar (&ccs, &ol, &dlg, ccs.picture_style == CAL_PICTURE_SAME);
							pDatabase->CopyObjectList(pNewPage->objects(), &ol, pNewPage->Id());	
						}
						else 
						{
							// create and add new weekly/monthly calendar
							pNewCal = pDatabase->create_calendar_object(&ccs);
							pNewCal->set_panel(MAIN_CALENDAR_PANEL);
							pNewCal->add_flags(OBJECT_FLAG_locked);
							pNewPage->objects()->append(pNewCal);
							dlg.Step();
						}
					}
					else // copy from source page
					{
						// copy all objects from source page
						PMGPageObject* pObj = (PMGPageObject*)pSourcePage->objects()->first_object();
						do
						{
							// copy object and append it to new list
							PMGPageObject * pDestObj = (PMGPageObject*)pObj->duplicate();
							ol.append(pDestObj);
							pObj = (PMGPageObject*)pObj->next_object();
						} while (pObj != NULL);

						// and add them to the new page
						pDatabase->CopyObjectList(pNewPage->objects(), &ol, pNewPage->Id());	

						// replace calendar info and rebuild calendars
					
						{
							// update calendar information to reflect new date
							pObj = (PMGPageObject*)pNewPage->objects()->first_object();
							// find calendar
							do
							{
								if(pObj->type() == OBJECT_TYPE_Calendar)
								{
									if (ciCalInfo.style.type == YEARLY)
									{
										// save old calendar month info, but update year info
										Calendar c = ((CalendarObject*)pObj)->get_calendar();
										CALENDAR_INFO ci = c.get_info();
										ciCalInfo.month = ci.month;
										ciCalInfo.week = ci.week;
									}

									ReplaceCalendarInfo(ciCalInfo, (CalendarObject*)pObj, pDocument);
									dlg.Step();								
									if (ciCalInfo.style.type != YEARLY)
										break; // should only be 1 calendar here.
								}

								pObj = (PMGPageObject*)pObj->next_object();
							} while (pObj != NULL);	
						}
					}
				} // end if 	
			
				pNewPage->release();
				// increment current page, only if adding at end
				if (bEnd)
					dwAtPage++;
			}
		} //while
		dlg.DestroyWindow();
		EndWaitCursor();
		
		// if copy of calendar was made, warn about picture placement
		if (bCopy)
			GET_PMWAPP()->ShowHelpfulHint("CalDateChange");

		// If we had a source page, release it now.
		if (pSourcePage != NULL)
		{
			pSourcePage->release();
		}
     
		if (!bEnd)
			dwAtPage = dwStartPage + Dialog.m_iAddNumber;

		if (dwAtPage > dwStartPage)
		{
			// We added something. Create an undo command for this.
			CCmdInsertPages* pMainCommand = new CCmdInsertPages;

			pMainCommand->Snapshot(this, dwStartPage, dwAtPage-1);

			for (DWORD dwPage = dwStartPage; dwPage < dwAtPage; dwPage++)
			{
				DB_RECORD_NUMBER lPage = pDocument->GetPage(dwPage);
				CCmdInsertPage* pCommand = new CCmdInsertPage;
				if (pCommand->Snapshot(this, lPage, dwPage))
				{
					pMainCommand->AddCommand(pCommand);
				}
				else
				{
					delete pCommand;
					delete pMainCommand;
					pMainCommand = NULL;
					break;
				}
			}

			if (pMainCommand != NULL)
	      {
				// Add it but don't execute it. It's already done!
				pDocument->AddCommand(pMainCommand, FALSE);
			}

			// The document is now modified.
			pDocument->SetModifiedFlag();

			// Change to the last page added.
			BeforePageChange();

			if (bEnd)
				pDocument->GotoPage(dwAtPage-1); // show last page
			else // show first page
				pDocument->GotoPage(0);
		}
	}
}

void CPmwView::OnUpdateAddCalendarPage(CCmdUI* pCmdUI) 
{
}

void CPmwView::ReplaceCalendarInfo(CALENDAR_INFO info, CalendarObjectPtr pObject, CPmwDoc * pDoc)
{
	if (!pDoc->object_selected(pObject))
		pDoc->deselect_all();

	// save old info
	CALENDAR_INFO oldinfo = pObject->get_calendar().get_info();
	pObject->get_calendar().get_info() = info;
	
	// rebuild the calendar
	pDoc->refresh_object(pObject);

	BOOL bFullBuild = TRUE;
	CFrameObject* frameobj;

	if ((frameobj = pObject->get_title_frame()) != NULL)
	{
		// replace the month name
		if (oldinfo.month != info.month)
		{
			ReplaceWord(frameobj,
				Calendar::get_month_name((MONTH)oldinfo.month),
				Calendar::get_month_name((MONTH)info.month),
				FALSE);
		}

		if (oldinfo.year != info.year)
		{
			char old_year[8];
			char new_year[8];

			itoa(oldinfo.year, old_year, 10);
			itoa(info.year, new_year, 10);

			ReplaceWord(frameobj,
				old_year,
				new_year,
				TRUE);
		}
		bFullBuild = FALSE;
	}

	pObject->rebuild(bFullBuild);
	
	pDoc->refresh_object(pObject);

}

void CPmwView::OnAddTable()
{
	GetDocument()->deselect_all(TRUE);

   OnTool(OBJECT_TYPE_Table);
}

void CPmwView::OnUpdateAddTable(CCmdUI* pCmdUI)
{
   OnUpdateTool(pCmdUI, OBJECT_TYPE_Table);

	//Util::PostWmCommand(this, ID_FORMAT_AUTOFORMAT_TABLE, NULL, 0);
}

void CPmwView::OnAddBorder() 
{
#if 0
	// This is the old way where a dialog asked the user
	// if they wanted to create a new border or select
	// an existing one.
	CAddBorderDlg dlg;
	int ret = dlg.DoModal();
	if (ret == ID_BORDER_CREATE)
	{
		OnTool(OBJECT_TYPE_Border);
	}
	else if (ret == ID_BORDER_SELECT)
	{
	   CArtBrowserDialog ArtBrowser;
		ArtBrowser.DoModal();
	}
#else
	if(!GetConfiguration()->SupportsBorderPlus())
		return;

	// This is the new way where we always just create a new one.
	OnTool(OBJECT_TYPE_Border);
#endif
}

void CPmwView::OnUpdateAddBorder(CCmdUI* pCmdUI) 
{
	if (GET_PMWAPP()->BorderEditorInitialized())
	{
		OnUpdateTool(pCmdUI, OBJECT_TYPE_Border);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


// Bring up the Border Editor
void CPmwView::OnEditBorder()
{
	if(!GetConfiguration()->SupportsBorderPlus())
		return;

	PMGPageObjectPtr pObj = GetDocument()->selected_objects();
	if (pObj && pObj->type() == OBJECT_TYPE_Border)
		((BorderObject*)pObj)->ShowBorderEditor();
}

// Update the "Edit Border" menu command.
void CPmwView::OnUpdateEditBorder(CCmdUI* pCmdUI)
{
	// see if we have a single Border object selected
	BOOL enable = FALSE;
	if (GetDocument()->one_selection())
	{
		PMGPageObjectPtr pObj = GetDocument()->selected_objects();
		enable = (pObj && pObj->type() == OBJECT_TYPE_Border);
	}

	pCmdUI->Enable(enable);
}

void CPmwView::OnBorderPlus()
{
	if(!GetConfiguration()->SupportsBorderPlus())
		return;

	PMGPageObject* pObj;
	if (GetDocument()->one_selection()
		 && (pObj = GetDocument()->selected_objects())->type() == OBJECT_TYPE_Border)
	{
		((BorderObject*)pObj)->ShowBorderEditor();
	}
	else
	{
		OnAddBorder();
	}
}

// Update the "Edit Border" menu command.
void CPmwView::OnUpdateBorderPlus(CCmdUI* pCmdUI)
{
	if (GetDocument()->one_selection()
		 && GetDocument()->selected_objects()->type() == OBJECT_TYPE_Border
		 && edit_event_handler != mode_create)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(FALSE);
	}
	else
	{
		OnUpdateAddBorder(pCmdUI);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Serif Drawing Object support.

// Serif drawing object class id.
// {614c7740-57c8-11ce-a50e-000021336621}
static const CLSID BASED_CODE DrawPlusCLSID = 
{ 0x614c7740, 0x57c8, 0x11ce, { 0xa5, 0x0e, 0x0, 0x0, 0x21, 0x33, 0x66, 0x21 } };

// Tool button handler.
void CPmwView::OnDrawPlus()
{
	// Route to either "add" or "edit" depending on whether a
	// DrawPlus object is selected or not.

	CPmwDoc* pDoc = GetDocument();
	if (pDoc->one_selection() && pDoc->selected_objects()->type() == OBJECT_TYPE_Serif)
	{
		OleObject* pObject = (OleObject*)pDoc->selected_objects();
		pObject->Item()->DoVerb(OLEIVERB_OPEN, this);
	}
	else
	{
		OnAddDrawingObject();
	}
}

void CPmwView::OnAddDrawingObject() 
{
#if 1

#ifdef LOCALIZE
   if (GetConfiguration()->RemoveSerifDraw())
   {
     return;
   }
#endif

	if(!GetConfiguration()->SupportsSerifDrawPlus())
		return;

	CPmwCntrItem* pItem = NULL;
	CPmwDoc* pDoc = GetDocument();
	PMGDatabase* pDatabase = pDoc->get_database();
	ERRORCODE error;

	TRY
	{
		CPmwDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		// Helpful Hint
		if (!GET_PMWAPP()->ShowHelpfulHint("DrawPlus!", NULL, TRUE))
		{
			return;
		}

		// Create new item connected to this document.
		pItem = new CPmwCntrItem(pDoc);
		ASSERT_VALID(pItem);

		// Initialize the item using the Draw Plus CLSID.
		if (!pItem->CreateNewItem(DrawPlusCLSID))
			AfxThrowMemoryException();  // any exception will do
		ASSERT_VALID(pItem);

		deselect_all();

		if ((error = pDoc->FinishOleObjectCreate(pItem, NULL, OBJECT_TYPE_Serif)) != ERRORCODE_None)
		{
			AfxThrowMemoryException();  // any exception will do
		}

		pItem->DoVerb(OLEIVERB_OPEN, this);
	}
	CATCH_ALL(e)
	{
		error = ERRORCODE_Fail;
	}
	END_CATCH_ALL

	if (error != ERRORCODE_None)
	{
		delete pItem;
	}
#else
   OnTool(OBJECT_TYPE_Serif);
#endif
}

void CPmwView::OnUpdateAddDrawingObject(CCmdUI* pCmdUI) 
{
   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plPrintMaster)
   {
#ifdef LOCALIZE
       if (GetConfiguration()->RemoveSerifDraw())
	   {
		 if (pCmdUI->m_pMenu)
		    pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);  
	   }
	   else
#endif
	   OnUpdateTool(pCmdUI, OBJECT_TYPE_Serif);
   }
   else
	{
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
	}
}

void CPmwView::OnEditDrawingObject() 
{
	CPmwDoc* pDoc = GetDocument();
	if (pDoc->one_selection() && pDoc->selected_objects()->type() == OBJECT_TYPE_Serif)
	{
		OleObject* pObject = (OleObject*)pDoc->selected_objects();
		pObject->Item()->DoVerb(OLEIVERB_OPEN, this);
	}
}

void CPmwView::OnUpdateEditDrawingObject(CCmdUI* pCmdUI) 
{
   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plPrintMaster)
   {
#ifdef LOCALIZE
		if (GetConfiguration()->RemoveCartoonMaker())
		{
		   if (pCmdUI->m_pMenu)
		     pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
		}
		else
#endif
		{
			CPmwDoc* pDoc = GetDocument();
			pCmdUI->Enable(pDoc->one_selection() && pDoc->selected_objects()->type() == OBJECT_TYPE_Serif);
		}
   }
   else
	{
		pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
	}
}

void CPmwView::OnOnlineCreateAccount() 
{
	GET_PMWAPP()->GetContentManager()->CreateNewAccount();
}

void CPmwView::OnOnlineChangeAccounts() 
{
	GET_PMWAPP()->GetContentManager()->RequestExistingAccount();
}

void CPmwView::OnOnlineUpdateAccount() 
{
	GET_PMWAPP()->GetContentManager()->UpdateAccount();
}

void CPmwView::OnOnlineDeleteAllDownloadedFiles() 
{
	GET_PMWAPP()->GetContentManager()->DeleteCachedDirectories(this, NULL);
}

// ****************************************************************************
//
//		Note:	The following methods are in this module so that the Photo Workshop
//				"palette" of choices is integrated with the other pop-up palettes,
//				and also to keep the command processing routines in one place.
//				However, the main body of the code is in ViewComp.cpp, since it
//				deals with Renaissance component objects.
//																						- jdf
//
// ****************************************************************************
//
//  Function Name:	CPmwView::OnUpdatePhotoWorkshop()
//
//  Description:		Handles enabling / disabling of Photo Workshop menu item
//							and tool button
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPmwView::OnUpdatePhotoWorkshop( CCmdUI* pCmdUI )
{
	// set the submenu item to NULL so that this will work for main menu
	//	item rather than just the submenu items
   CMenu *save = pCmdUI->m_pSubMenu;
   pCmdUI->m_pSubMenu = NULL;

	// call the ViewComp method
   DoUpdatePhotoWorkshop( pCmdUI );
   
	pCmdUI->m_pSubMenu = save;
}

// ****************************************************************************
//
//  Function Name:	CPmwView::OnPhotoWorkshop...()
//
//  Description:		Process the Photo Workshop cascading menu items
//
//	 Note:				These calls come from the menu items themselves; we do
//							not invoke the popup palette that the Photo Workshop
//							button triggers
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPmwView::OnPhotoWorkshopCropOrientation()
{
	DoPhotoWorkshop( kCropOrientTab );
}

void CPmwView::OnPhotoWorkshopAdjustColor()
{
	DoPhotoWorkshop( kAdjustColorTab );
}

void CPmwView::OnPhotoWorkshopBrightnessFocus()
{
	DoPhotoWorkshop( kLightFocusTab );
}

void CPmwView::OnPhotoWorkshopFixFlaw()
{
	DoPhotoWorkshop( kFixFlawTab );
}

void CPmwView::OnPhotoWorkshopArtisticEffects()
{
	DoPhotoWorkshop( kArtEffectsTab );
}

// ****************************************************************************
//
//  Function Name:	CPmwView::OnPhotoWorkshopButton()
//
//  Description:		Process the Photo Workshop button
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPmwView::OnPhotoWorkshopButton()
{
	CPmwDoc* pDoc = GetDocument();
	TpsAssert( pDoc->one_selection(), "Menu item should have been disabled.");
	TpsAssert( pDoc->selected_objects()->type() == OBJECT_TYPE_Component, "Menu item should have been disabled.");
	TpsAssert( ! m_Txp.Valid(), "Menu item should have been disabled.");

	// we shouldn't even be here if a text box is being edited
	if( ! m_Txp.Valid() )
	{
		// bring up the popup window of menu choices
		OnModifyPalette(ID_PHOTO_WORKSHOP_BUTTON, ID_PHOTO_WORKSHOP_MENU_PALETTE, PhotoWorkshopMenuWindow);
   }
}

// ****************************************************************************
//
//  Function Name:	CPmwView::OnPhotoWorkshopMenuPalette()
//
//  Description:		Handles user choice from the popup menu window
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPmwView::OnPhotoWorkshopMenuPalette()
{
	TpsAssert( ! m_Txp.Valid(), "Menu item should have been disabled.");

	// we shouldn't even be here if a text box is being edited
   if( ! m_Txp.Valid() )
   {
		// get index of the user's choice
      int nChoice = PhotoWorkshopMenuWindow.GetMenuItem();

		// get rid of the popup
		PhotoWorkshopMenuWindow.DestroyWindow();

		// call the ViewComp method to display the Photo Workshop dialog
		DoPhotoWorkshop( nChoice );
   }
}


// ****************************************************************************
//
//  Function Name:	CPmwView::OnMadeToOrderButton()
//
//  Description:		Handles user choice from the popup menu window
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPmwView::OnMadeToOrderButton()
{
	// bring up the popup window of menu choices
	OnModifyPalette(ID_MADE_TO_ORDER_BUTTON, ID_MADE_TO_ORDER_MENU_PALETTE, MadeToOrderMenuWindow);
}


// ****************************************************************************
//
//  Function Name:	CPmwView::OnMadeToOrderMenuPalette()
//
//  Description:		Handles the response from the made to order popup menu.
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPmwView::OnMadeToOrderMenuPalette()
{
	// get index of the user's choice
   int nChoice = MadeToOrderMenuWindow.GetMenuItem();

	// get rid of the popup
	MadeToOrderMenuWindow.DestroyWindow();

	if (nChoice == MADE_TO_ORDER_SEAL)
	{
		OnAddSeal();
	}
	else if (nChoice == MADE_TO_ORDER_TIMEPIECE)
	{
		OnAddTimePiece();
	}
	else
	{
		TRACE( "Should not get here.  An invalid menu item ID was returned.\n" );
		ASSERT( FALSE );
	}
}


//**** HTML COMMAND UI
// Looks for selected text objects, tests for like extra flags
BOOL
CPmwView::IsAnyTextObjectSelected(DWORD &Flags, BOOL &AllAlike)
{
   ASSERT(&Flags != NULL);
   ASSERT(&AllAlike != NULL);
   
   Flags = 0;
   // For now, no text objects selected = all alike
   AllAlike = TRUE;  

   CPmwDoc *pDoc = GetDocument();
   if (pDoc != NULL)
   {
      PMGPageObject  *pObject = pDoc->selected_objects();
      BOOL           FoundText = FALSE;
      while (pObject != NULL)
	   {
         TestObject(pObject, Flags, AllAlike, FoundText);
         if ((FoundText == TRUE) && (AllAlike == FALSE))
         {
            // This has no meaning now
            Flags = 0;           
            break;
         }

         pObject = pObject->next_selected_object();
      }   
      
      return FoundText;
   }
   // No text objects selected
   return FALSE;
}
// Tests object for text, like extra flags
void
CPmwView::TestObject(PMGPageObject *pObject, DWORD &Flags, BOOL &AllAlike, BOOL &FoundText)
{
   if (pObject->type() == OBJECT_TYPE_Group)
   {
      // Found a group - do it the "list" way
      ObjectList* pList = ((GroupObject*)pObject)->object_list();
      // NOTE: We may be calling the one who called us - don't be afraid
      IsAnyTextObjectSelected(Flags, AllAlike, pList, FoundText);
   }
   else
   if (pObject->type() == OBJECT_TYPE_Frame)
   {
      // The first text found - just save the flags
      if (FoundText == FALSE) 
      {
         Flags = ((CFrameObject*)pObject)->GetExtraFlags();          
         FoundText = TRUE;
      }
      else
      {
         if (Flags != ((CFrameObject*)pObject)->GetExtraFlags())
         {
            // Not all selected objects have the same flags
            AllAlike = FALSE; 
         }
      }
   } 
}

BOOL
CPmwView::IsJustOneObjectSelected(CPmwDoc **pDoc)
{
   CPmwDoc *pDocLocal;
   if (pDoc == NULL)
      pDoc = &pDocLocal;

   if ((*pDoc = GetDocument()) != NULL)
   {
      return (*pDoc)->one_selection();
   }
   return FALSE;
}
/*
void CPmwView::OnUpdateWebHyperlink(CCmdUI* pCmdUI) 
{
   // Only allowed in WebViews

	// Delete any preceding separator.
	int nIndex = pCmdUI->m_nIndex-1;
	if (nIndex >= 0)
	{
		if (pCmdUI->m_pMenu->GetMenuItemID(nIndex) == 0)
		{
			pCmdUI->m_pMenu->DeleteMenu(nIndex, MF_BYPOSITION);
		}
	}

	// Delete the hyperlink menu item.
   pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND);
}
*/
// Do we show the Hyperlink menu option, or not?
// Looks for grouped text objects, tests for like extra flags
void
CPmwView::IsAnyTextObjectSelected(DWORD &Flags, BOOL &AllAlike, ObjectList* pList, BOOL &FoundText)
{
   PMGPageObjectPtr pObject = (PMGPageObjectPtr)pList->first_object();
   while (pObject != NULL)
   {
      TestObject(pObject, Flags, AllAlike, FoundText);
      // We know all we need to, so quit
      if ((FoundText == TRUE) && (AllAlike == FALSE))
         break;

      pObject = (PMGPageObjectPtr)pObject->next_object();
   }
}

void 
CPmwView::OnUpdateWebHyperlink(CCmdUI* pCmdUI) 
{
	CPmwDoc *pDoc;
   if (IsJustOneObjectSelected(&pDoc) == TRUE)
   {
      if (pDoc->SupportsHTML() != FALSE)
		{
			PMGPageObject* pObject = pDoc->selected_objects();
			if (pObject != NULL)
			{
				if ((pObject->type() != OBJECT_TYPE_Frame) 
					||
					(GetTxp()->SomethingSelected()))
				{
					pCmdUI->Enable(TRUE);
					return;
				}
			}
		}
   }

   pCmdUI->Enable(FALSE);
}
/*
#ifdef   MODIFY_LAYER_MENU
BOOL
CWebView::AnyNonHotspotsSelected(CPmwDoc *pDoc)
{
   if (pDoc != NULL)
   {
      PMGPageObject* pObject;
      for (pObject = pDoc->selected_objects();
         pObject != NULL;
         pObject = pObject->next_selected_object())
      {
         if (pObject->type() != OBJECT_TYPE_Hyperlink)   
            return TRUE;
      }
   }

   return FALSE;
}
#endif
*/
// This is strictly for the Format HTML menu
void
CPmwView::DoMenuEnableAndCheck(CCmdUI* pCmdUI, DWORD Flags)
{
   int   Check = 0;
   BOOL  Enable = FALSE, AllAlike;
   DWORD CurFlags;
   if (IsAnyTextObjectSelected(CurFlags, AllAlike) == TRUE)   
   {  
      // Some text object(s) selected, enable the menu item
      Enable = TRUE;                               
      // Do they all have the same HTML flags set?
      if (AllAlike == TRUE)                        
      {                                            
         // If it's the flag we're looking for, check the menu item
         if (FLAGTEST(CurFlags, Flags) == TRUE)    
            Check = 1;   
      }
   }

   pCmdUI->SetCheck(Check);
   pCmdUI->Enable(Enable);
}

void 
CPmwView::OnUpdateWebAuto(CCmdUI* pCmdUI) 
{
   int   Check = 0;
   BOOL  Enable = FALSE, AllAlike;
   DWORD CurFlags;
	CPmwDoc *pDoc;
   
	if ((pDoc = GetDocument()) != NULL && (pDoc->SupportsHTML() != FALSE))
	{
		if (IsAnyTextObjectSelected(CurFlags, AllAlike) == TRUE)
		{  
			// Some text object(s) selected, enable the menu item
			Enable = TRUE;                               
			// Do they all have the same HTML flags set?
			if (AllAlike == TRUE)                        
			{                                            
				// If no flags set, check auto menu item
				if (FLAGTEST(CurFlags, (HTMLFrame_ForceText | HTMLFrame_ForceGraphic)) == FALSE)
					Check = 1;   
			}
		}
	}

   pCmdUI->SetCheck(Check);
   pCmdUI->Enable(Enable);
}

void 
CPmwView::OnUpdateIfHTMLText(CCmdUI* pCmdUI) 
{
	CPmwDoc *pDoc;
   if ((pDoc = GetDocument()) != NULL && (pDoc->SupportsHTML() != FALSE))
		DoMenuEnableAndCheck(pCmdUI, HTMLFrame_ForceText);
	else
		pCmdUI->Enable(FALSE);
}

void 
CPmwView::OnUpdateIfHTML(CCmdUI* pCmdUI) 
{
	CPmwDoc *pDoc;
   if ((pDoc = GetDocument()) != NULL && (pDoc->SupportsHTML() != FALSE))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}
//**** WEB MESSAGE HANDLERS
void 
CPmwView::OnPublishToFolder() 
{
   if (QueryPublish() == TRUE)
		PublishToFolder(); 
}

void 
CPmwView::OnPublishToWeb() 
{
   if (QueryPublish() == TRUE)
		PublishToWeb();
}
// Set the Hyperlink for an existing object
void CPmwView::OnWebHyperlink() 
{
   CPmwDoc* pDoc = GetDocument();
   if (pDoc != NULL)
   {
      PMGPageObject* pObject = pDoc->selected_objects();
      ASSERT(pObject != NULL);
      
      if (pObject->type() == OBJECT_TYPE_Frame) // Hyperlinks for text boxes...
         DoHyperlinkText(pDoc);
      else
         DoHyperlinkObject(pDoc, pObject);      // are different than for other objects
   }
}

void 
CPmwView::OnHotSpot() 
{
   OnTool(OBJECT_TYPE_Hyperlink);	
}

void 
CPmwView::OnWebChecker() 
{
   PreviewWebSite(TRUE, TRUE); 
}

void 
CPmwView::OnWebAuto() 
{
   SetClearSelectedTextObjectsFlags(0, (HTMLFrame_ForceText | HTMLFrame_ForceGraphic));
}

void 
CPmwView::OnWebKeepText() 
{
   SetClearSelectedTextObjectsFlags(HTMLFrame_ForceText, HTMLFrame_ForceGraphic);
}

void 
CPmwView::OnWebAsGraphic() 
{
   SetClearSelectedTextObjectsFlags(HTMLFrame_ForceGraphic, HTMLFrame_ForceText);
}

void 
CPmwView::OnPageProperties() 
{
   CWebPageProperties	dlgPageProp;
/*
   CCmdChangeSelects* pCommand;
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
*/
   dlgPageProp.DoModal(GetDocument());
/*
   if (pCommand != NULL)
   {
      CPmwDoc *pDoc;
      if ((1 == 1) && (pCommand->After()) && ((pDoc = INHERITED::GetDocument()) != NULL))
      {
         // Add the command for undoing later.
         pDoc->AddCommand(pCommand, FALSE);  // No execute!
      }
      else
      {
         delete pCommand;
      }
   }
*/
}

