/*
// $Workfile: PMWDOC.CPP $
// $Revision: 2 $
// $Date: 3/04/99 2:18p $
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
// $Log: /PM8/App/PMWDOC.CPP $
// 
// 2     3/04/99 2:18p Johno
// Fix in UpdatePageProperties() for text background color bug.
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 88    3/01/99 11:55a Cschende
// added functions to make permanent font substitutions in the document
// after it is read in for font which are no longer going to be
// distributed with PrintMaster
// 
// 87    2/25/99 3:27p Lwilson
// Changed component creation to use a pointer to a CCreationData class or
// variant for in place of an RComponentDocument pointer.
// 
// 86    2/20/99 9:23p Psasse
// Support for Vertical Banners
// 
// 85    2/15/99 1:01p Psasse
// ChangeRotation() moved from the view to the doc class
// 
// 84    2/08/99 10:23a Lwilson
// Temporarily disabled object conversion.
// 
// 83    2/01/99 5:10p Johno
// Compile update
// 
// 82    2/01/99 4:34p Lwilson
// Added initial support for converting PM objects into component objects.
// 
// 81    2/01/99 2:46p Blee
// Added virtual bool IsMultiPageProjectType()
// Added OnUpdateAddPage()
// Added OnUpdateGotoPage()
// 
// 80    2/01/99 9:38a Johno
// Changes for paneled HTML
// 
// 79    1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// Preliminary separate panel backgrounds
// 
// 77    1/27/99 10:50a Johno
// Compile update
// 
// 76    1/21/99 6:13p Johno
// Changes for links, hotspots, and text links!
// 
// 75    1/20/99 4:58p Johno
// Changes for paneled HTML
// 
// 74    1/20/99 4:33p Jfleischhauer
// added fResize flag to ComponentObject::SetComponentData()
// 
// 73    1/20/99 10:28a Mwilson
// moved toolbars into seperate DLL
// 
// 72    1/14/99 2:25p Johno
// Fixed display problem (after creating HTML)
// rearranged code with HTMLCONV.CPP
// 
// 71    1/14/99 11:44a Johno
// Oops! (compile update)
// 
// 70    1/13/99 5:11p Lwilson
// Added SetModifiedFlag() call to end of replace_component() method.
// 
// 69    1/13/99 10:47a Johno
// Changes for HTML projects
// 
// 68    1/07/99 6:11p Johno
// Preliminary "paneled HTML" for some projects
// Changes to eliminate some h file dependecies
// 
// 67    1/07/99 5:14p Rgrenfel
// Made sure rendering isn't skipped when a drop render is in progress.
// 
// 66    12/23/98 3:51p Johno
// Changes for projects as HTML
// 
// 65    12/22/98 10:28a Johno
// Moved most HTML conversion stuff here from webdoc.*
// 
// 64    12/19/98 4:38p Psasse
// Revert to Saved support
// 
// 63    12/15/98 3:37p Lwilson
// Initial support for PS component editing.
// 
// 62    12/14/98 1:06p Lwilson
// Added default sizing support for PS components.
// 
// 61    12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 60    12/02/98 1:56p Johno
// Removed error checks after GET_PMWAPP()->AddressBook();
// An empty address book name is NORMAL for Classic.
// 
// 59    11/20/98 1:33p Mwilson
// added capability to turn off auto instructions in preferences
// 
// 58    11/12/98 12:31p Mwilson
// opens instruction page when opening a new project
// 
// 57    10/28/98 11:14a Mwilson
// fixed paste bug
// 
// 56    10/23/98 4:09p Gbeddow
// Changed LocatePath to ExpandPath in address book startup.
// 
// 55    10/21/98 4:58p Jayn
// Fixed path manager problems.
// 
// 54    10/19/98 10:50a Jayn
// More COM restructuring changes.
// 
// 53    10/05/98 9:53a Mwilson
// changes to print projects utility
// 
// 52    9/30/98 4:44p Mwilson
// added extraction of friendly name in SetBrowserPath
// 
// 51    9/29/98 1:07p Mwilson
// added support for view instruction pages in HTML help window
// 
// 50    9/25/98 11:07a Mwilson
// added support for instruction page
// 
// 49    9/24/98 11:24a Mwilson
// fixed print project utility
// 
// 48    9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 47    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 46    9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 45    9/14/98 9:21a Mwilson
// added function to add filename and page header.  Used by print projects
// tool
// 
// 44    8/10/98 3:47p Hforman
// Crop Copyright modifications
// 
// 43    8/03/98 12:20p Jayn
// Address book stuff is created even in Classic.
// 
// 42    7/24/98 9:09p Hforman
// in DoSave(), searching for dynamic name in DB first
// 
// 41    7/23/98 6:45p Jayn
// My earlier fix for a hack was incorrectly implemented. Now fixed.
// 
// 40    7/22/98 10:01a Jayn
// Fixed a hack that was causing refresh problems.
// 
// 39    7/16/98 1:45p Psasse
// Added Advanced Printing Functionality "Print As Bitmap"
// 
// 38    7/04/98 11:28a Jayn
// Workaround for bug in Deskjet 890C printer.
// 
// 37    6/29/98 3:22p Johno
// Added IsThisPage() - tests if a page with the given record number
// exists
// 
// 36    6/25/98 11:28a Psasse
// fix changing friendly name for multiple saves of same file
// 
// 35    6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 34    6/05/98 8:47a Fredf
// FLAG_BringUpReceiverList (HACK!)
// 
// 33    6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 32    6/01/98 1:24p Mwilson
// changed to use new function name for embed_all_picts
// 
// 31    5/30/98 6:43p Jayn
// Table cell contents; cell formatting undo
// 
// 30    5/20/98 11:25a Cboggio
// Get Default sort index string from resource file
// 
// 29    4/24/98 6:34p Fredf
// Moved print preview extent pointer into CPmwView (it was a global.)
// 
// 28    4/21/98 3:39p Fredf
// Reworked "reverse" printing to apply to all projects.
// 
// 27    4/18/98 4:24p Fredf
// T-Shirt printing.
// 
// 26    4/17/98 11:40a Fredf
// Fixes to code to make "Show Actual Size" persistent.
// 
// 25    4/16/98 7:33p Fredf
// "Preview Actual Size" control in font dialog (NEWFONT.CPP)
// 
// 24    4/16/98 6:55p Hforman
// 
// 23    4/13/98 2:38p Hforman
// add user categories to save
// 
// 22    3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 21    3/19/98 2:32p Jayn
// Finish up master pages.
// 
// 20    3/18/98 11:15a Jayn
// Page numbers
// 
// 19    3/17/98 2:43p Fredf
// New workspace toolbars.
// 
// 18    3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 17    3/10/98 5:24p Jayn
// Master page changes
// 
// 16    3/02/98 3:30p Johno
// Moved web stuff to webdoc.cpp / h
// 
// 15    2/27/98 2:47p Jayn
// "Draw as bitmaps"
// 
// 14    2/17/98 11:45a Dennis
// Added Background Object
// 
// 13    2/12/98 5:56p Jayn
// 
// 12    2/10/98 1:26p Dennis
// Added support for publishing multi-page web documents.
// 
// 11    12/18/97 3:20p Dennis
// Added SetDefaultFace() & DefaultFace() methods to allow overiding
// default typeface for a doc. type.
// 
// All object types except Text are converted to HTML.
// 
// 10    12/10/97 3:00p Dennis
// Added support for Misc. object types for publishing
// 
// 9     12/08/97 2:49p Dennis
// Added dest. directory to ConvertToHTML()
// 
// 8     12/02/97 11:15a Dennis
// Remove focus from objects before publishing.
// 
// 7     12/01/97 1:35p Hforman
// fix bug in size_list() for HalfCard projects
// 
// 6     11/19/97 3:28p Hforman
// modified size_list() to scale guides appropriately
// 
// 5     11/14/97 11:28a Dennis
// Changes for HTML conversion
//
//4     11/03/97 2:14p Jayn
//New CPtrList DrawObjects() method.
// 
// 3     10/30/97 4:31p Dennis
// Added #ifdef _DEBUG block to exclude debug diagnostics for release
// build.
// 
// 2     10/28/97 5:39p Dennis
// Added ConvertToHTML() and BuildHTMLObjectList() methods.
// 
//    Rev 1.4   26 Oct 1997 18:58:26   JOE
// modified calendar desktop to handle picture/calendar
// 
//    Rev 1.3   04 Sep 1997 10:29:10   johno
// Pasting a sentiment is now not part of PmwDoc ctor
// 
// 
//    Rev 1.2   26 Aug 1997 14:44:06   johno
// Pasting a sentiment is now part of PmwDoc
// 
// 
//    Rev 1.1   25 Aug 1997 10:32:56   johno
// "Path of sanitation" stuff moved here
// 
//    Rev 1.0   14 Aug 1997 15:24:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:52   Fred
// Initial revision.
// 
//    Rev 1.68   24 Jul 1997 16:05:30   Jay
// Now create storage ourselves in OnNewDocument
// 
//    Rev 1.67   20 Jul 1997 11:13:50   Jay
// Won't recompute refresh extent if previewing.
// 
//    Rev 1.66   15 Jul 1997 16:37:24   Jay
// Now rebuilds face list if a free font becomes registered.
// 
//    Rev 1.65   01 Jul 1997 10:52:40   Jay
// New MRU stuff
// 
//    Rev 1.64   01 Jul 1997 09:32:24   Jay
// MRU / Open document changes
// 
//    Rev 1.63   17 Jun 1997 12:42:34   Jay
// Import; OpenStreams()
// 
//    Rev 1.62   05 Jun 1997 16:45:38   Jay
// Changed upto_object in DrawObjects
// 
//    Rev 1.61   04 Jun 1997 10:26:18   Jay
// Support for updating the thumbnail
// 
//    Rev 1.60   02 Jun 1997 09:40:08   Jay
// Commented out semaphore.
// 
//    Rev 1.59   01 Jun 1997 14:47:34   Jay
// If'd last change back in.
// 
//    Rev 1.58   01 Jun 1997 14:42:06   Jay
// Commented out code to set the friendly name to the file name.
// 
//    Rev 1.57   15 May 1997 16:47:06   Fred
// Calls new font dialog
// 
//    Rev 1.56   09 May 1997 10:16:56   Jay
// Project Save
// 
//    Rev 1.55   07 May 1997 17:21:48   Jay
// More save project tweaks.
// 
//    Rev 1.54   06 May 1997 10:30:08   Jay
// New browser stuff.
// 
//    Rev 1.53   24 Apr 1997 15:08:18   Jay
// Got rid of some TRACEs
// 
//    Rev 1.52   16 Apr 1997 12:12:26   Jay
// Fixes for text undo
// 
//    Rev 1.51   15 Apr 1997 15:44:44   Jay
// Card Server phase #1
// 
//    Rev 1.50   03 Apr 1997 14:31:22   Jay
// Resource cleanup
// 
//    Rev 1.49   03 Apr 1997 09:31:26   Fred
// More text cleanup
// 
//    Rev 1.48   28 Mar 1997 17:10:54   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.47   26 Mar 1997 14:51:58   Jay
// Interruptable preview
// 
//    Rev 1.46   21 Mar 1997 15:48:54   Jay
// More preview changes.
// 
//    Rev 1.45   21 Mar 1997 11:10:38   Jay
// Print preview changes
// 
//    Rev 1.44   17 Mar 1997 16:08:58   Jay
// Frees the object properties before the database goes away.
// 
//    Rev 1.43   17 Mar 1997 14:48:14   Jay
// Warp shape palette and support.
// 
//    Rev 1.42   06 Mar 1997 13:22:12   Fred
// More text stuff
// 
//    Rev 1.41   25 Feb 1997 09:09:24   Jay
// ASSERT changes.
// 
//    Rev 1.40   21 Feb 1997 16:00:02   Fred
// UpdateState::object
// 
//    Rev 1.39   07 Feb 1997 08:51:58   Fred
// Start of new text drawing pipeline
// 
//    Rev 1.38   24 Jan 1997 14:58:14   Fred
// Added some comments
// 
//    Rev 1.37   21 Jan 1997 14:24:16   dennisp
// Moved object caching into object code (GraphicObject::update()).
// 
//    Rev 1.36   15 Jan 1997 10:49:28   Jay
// New toolbars
// 
//    Rev 1.35   08 Jan 1997 15:22:16   Fred
// More text box changes
// 
//    Rev 1.34   07 Nov 1996 16:40:34   johno
// Moved strings to resource DLL
// 
//    Rev 1.33   15 Oct 1996 09:02:06   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.32   10 Oct 1996 14:31:24   Jay
// More dynamic graphic/item updating.
// 
//    Rev 1.31   07 Oct 1996 17:00:50   Jay
// Busy cursor during more of save.
// Does not initialize graphics on startup anymore.
// 
//    Rev 1.30   04 Sep 1996 08:23:14   Jay
// Reversed order to fix intersection problem.
// 
//    Rev 1.29   01 Sep 1996 10:54:52   Fred
// Fixed assert
// 
//    Rev 1.28   28 Aug 1996 11:14:00   Jay
// Generalized solution to special non-edit documents with sidebar crashes.
// 
//    Rev 1.27   25 Aug 1996 13:24:42   Jay
// Friendly names for pictures
// 
//    Rev 1.26   25 Aug 1996 12:56:26   Jay
// Fixed a crashing bug: big previews in picture album when picture sidebar
// was up.
// 
//    Rev 1.25   21 Aug 1996 15:20:04   Jay
// Resize now updates frame intersections
// 
//    Rev 1.24   21 Aug 1996 14:40:16   Jay
// No longer builds project previews.
// Fix for OLE memory leak: close stream before storage goes away.
// 
//    Rev 1.23   14 Aug 1996 12:38:30   Jay
// An aborted experiment
// 
//    Rev 1.22   09 Aug 1996 12:55:20   Jay
// Add to Album check in Save dialog.
// 
//    Rev 1.21   08 Aug 1996 12:58:02   Jay
// Eliminated frame outline glitches.
// 
//    Rev 1.20   07 Aug 1996 10:24:52   Jay
// Sizes all pages in a project.
// 
//    Rev 1.19   06 Aug 1996 11:17:04   Jay
//  
// 
//    Rev 1.18   02 Aug 1996 15:25:28   Jay
// Bug fixes
// 
//    Rev 1.17   02 Aug 1996 13:11:50   Fred
// More 16-bit changes
// 
//    Rev 1.16   31 Jul 1996 11:56:38   Fred
// 16-bit compatibility
// 
//    Rev 1.15   29 Jul 1996 14:59:00   Jay
// Frame outline control
// 
//    Rev 1.14   26 Jul 1996 18:14:56   Jay
// Bigger previews
// 
//    Rev 1.13   26 Jul 1996 11:39:24   Jay
// Page stuff
// 
//    Rev 1.12   25 Jul 1996 19:38:44   Jay
//  
// 
//    Rev 1.11   25 Jul 1996 18:21:28   Fred
//  
// 
//    Rev 1.10   25 Jul 1996 13:14:24   Jay
// Multi-page frame glitch correction.
// 
//    Rev 1.9   25 Jul 1996 11:17:00   Jay
// More page stuff
// 
//    Rev 1.8   17 Jul 1996 15:43:06   Jay
//  
// 
//    Rev 1.7   16 Jul 1996 17:30:36   Fred
// More address/sender book stuff
// 
//    Rev 1.6   16 Jul 1996 08:53:12   Fred
// Hooked up address/sender book
// 
//    Rev 1.5   03 Jul 1996 18:52:20   Fred
// Picture editor
// 
//    Rev 1.4   26 Jun 1996 14:23:12   Jay
//  
// 
//    Rev 1.3   13 Jun 1996 15:53:54   Jay
//  
// 
//    Rev 1.2   05 Jun 1996 08:21:50   Jay
//  
// 
//    Rev 1.1   23 Apr 1996 08:11:40   Jay
// Fixes for test print.
// Test for text record (commented out).
// 
//    Rev 1.0   14 Mar 1996 13:37:02   Jay
// Initial revision.
// 
//    Rev 1.2   13 Mar 1996 14:54:58   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.1   06 Mar 1996 10:31:20   Jay
// Changes from 2.1
// 
//    Rev 2.66   04 Mar 1996 10:56:10   JAY
// New stuff.
// 
//    Rev 2.65   09 Feb 1996 13:42:08   JAY
// FIXTEMPLATE stuff.
// 
//    Rev 2.64   24 Jan 1996 10:03:52   JAY
// New string management in Configuration
// 
//    Rev 2.63   18 Jan 1996 17:10:24   JAY
// New project configuration class
// 
//    Rev 2.62   15 Jan 1996 09:16:48   JAY
// Changes to fix changes made for 32-bit version.
// 
//    Rev 2.61   26 Dec 1995 10:29:08   FRED
// Project structure changes for mac port
// 
//    Rev 2.60   20 Dec 1995 09:56:34   JAY
// 32-bit fixes
// 
//    Rev 2.59   15 Nov 1995 15:23:16   JAY
// OLE fixes, etc.
// 
//    Rev 2.58   13 Nov 1995 17:05:42   JAY
// Now calls DoFileFreeFonts() if user chooses "Register Now...".
// 
//    Rev 2.57   11 Oct 1995 17:47:18   JAY
// Now handles auto-load documents (from command line).
// 
//    Rev 2.56   10 Oct 1995 15:43:06   FRED
// Properly sizes rotated objects when page size is changed
// 
//    Rev 2.55   06 Oct 1995 10:20:04   JAY
// New print code.
// 
//    Rev 2.54   29 Sep 1995 16:55:22   JAY
// Fixed Send Mail.
// 
//    Rev 2.53   29 Sep 1995 16:34:10   JAY
// Added send mail
// 
//    Rev 2.52   29 Sep 1995 16:00:32   JAY
// More drag/drop/paste/OLE stuff
// 
//    Rev 2.51   27 Sep 1995 15:27:30   JAY
// New OLE code
// 
//    Rev 2.50   21 Sep 1995 13:47:24   JAY
// Changes to CDibDC
// 
//    Rev 2.49   15 Sep 1995 14:05:04   JAY
// More 32-bit changes.
// 
//    Rev 2.48   13 Sep 1995 11:48:40   JAY
// New 32-bit changes
// 
//    Rev 2.47   08 Sep 1995 15:44:30   JAY
// Now treats note cards like cards.
// 
//    Rev 2.46   07 Sep 1995 18:55:30   JAY
// 
//    Rev 2.45   06 Sep 1995 08:22:50   JAY
// More stuff for fixing documents.
// 
//    Rev 2.44   05 Sep 1995 14:44:08   JAY
// Got rid of an ASSERT which was wrong.
// 
//    Rev 2.43   05 Sep 1995 11:35:16   JAY
// Now removes the preview if the user aborts a color preview.
// 
//    Rev 2.42   31 Aug 1995 13:16:40   FRED
// More OLE shutdown fixes.
// 
//    Rev 2.41   30 Aug 1995 16:23:20   FRED
// Fixed problem with GP on close.
// 
//    Rev 2.40   30 Aug 1995 14:13:36   JAY
// Added PrintPaperInfo() method.
// 
//    Rev 2.39   27 Aug 1995 12:34:12   FRED
// New fill-in-fields stuff
// 
//    Rev 2.38   22 Aug 1995 07:58:18   JAY
// CBoolSemaphore now in Util.
// 
//    Rev 2.37   20 Aug 1995 18:14:32   FRED
// Fixed OLE2 leak.
// 
//    Rev 2.36   18 Aug 1995 07:22:02   FRED
// Miscellaneous fixes
// 
//    Rev 2.35   17 Aug 1995 07:20:44   FRED
//  
// 
//    Rev 2.34   16 Aug 1995 16:23:30   JAY
//  
// 
//    Rev 2.33   15 Aug 1995 14:11:42   JAY
// Fixed an error message.
// 
//    Rev 2.32   11 Aug 1995 10:32:04   JAY
// Now regards all documents from the home CD as templates.
// 
//    Rev 2.31   10 Aug 1995 18:04:40   JAY
// Better handling of address book name.
// 
//    Rev 2.30   09 Aug 1995 13:10:34   JAY
// Misc changes.
// 
//    Rev 2.29   07 Aug 1995 07:48:38   FRED
// Standard fill-in fields.
// 
//    Rev 2.28   06 Aug 1995 09:45:32   FRED
// More fill-in fields stuff.
// 
//    Rev 2.27   04 Aug 1995 15:20:18   JAY
// New Save stuff.
// 
//    Rev 2.26   04 Aug 1995 10:47:08   FRED
// Sender fields.
// 
//    Rev 2.25   04 Aug 1995 07:24:58   JAY
// New save dialog
// 
//    Rev 2.24   21 Jul 1995 17:20:48   JAY
// Label printing
// 
//    Rev 2.23   20 Jul 1995 12:50:44   JAY
// Label project. New paper info stuff. etc
// 
//    Rev 2.22   16 Jul 1995 18:55:24   FRED
// Album Enhancements
// 
//    Rev 2.21   14 Jul 1995 17:22:34   FRED
// Disk open/save from album
// 
//    Rev 2.20   12 Jul 1995 17:45:06   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 2.19   11 Jul 1995 14:19:20   JAY
// Fill-in fields dialog.
// 
//    Rev 2.18   11 Jul 1995 09:41:44   JAY
// Initial fill-ins dialog for doc.
// 
//    Rev 2.17   10 Jul 1995 16:26:30   JAY
// More useful stuff for save project.
// 
//    Rev 2.16   08 Jul 1995 15:37:46   JAY
// Misc.
// 
//    Rev 2.15   07 Jul 1995 16:13:10   JAY
// Include rearranging; document saving with album.
// 
//    Rev 2.14   30 Jun 1995 16:35:16   JAY
// New PaperInfo and EditDesktop
// 
//    Rev 2.13   26 Jun 1995 16:58:40   JAY
//  
// 
//    Rev 2.12   16 Jun 1995 13:00:36   JAY
//  
// 
//    Rev 2.11   07 Jun 1995 15:39:56   JAY
//  
// 
//    Rev 2.10   06 Jun 1995 10:10:38   JAY
// Now uses CDibDC.
// 
//    Rev 2.9   05 Jun 1995 11:35:22   JAY
//  
// 
//    Rev 2.8   13 May 1995 11:19:52   JAY
//  
// 
//    Rev 2.7   03 May 1995 14:48:08   JAY
//  
// 
//    Rev 2.6   01 May 1995 15:28:00   JAY
// Restructured the macro stuff
// 
//    Rev 2.5   25 Apr 1995 09:01:20   JAY
// Created macro server to be go-between for document and font engine.
// 
//    Rev 2.4   19 Apr 1995 12:35:54   JAY
//  
// 
//    Rev 2.3   29 Mar 1995 09:14:34   JAY
// Lots of changes
// 
//    Rev 2.2   20 Mar 1995 16:35:54   JAY
// Beginning of OLE support.
// 
//    Rev 2.1   08 Feb 1995 13:33:26   JAY
// Reverted. New series.
// 
//    Rev 1.50   30 Jan 1995 09:34:36   JAY
// Cut and paste
// 
//    Rev 1.49   04 Nov 1994 12:21:52   JAY
// Fixed some overflow problems by changing scale_number()s to scale_pcoord()s.
// 
//    Rev 1.48   06 Oct 1994 15:19:24   JAY
// Now passes margins to info.get_panel_world().
// New sizing code for cards.
// 
//    Rev 1.47   04 Aug 1994 08:30:32   JAY
// Large model and 32-bit changes
// 
//    Rev 1.46   20 Jul 1994 16:46:26   JAY
// Will not do any font stuff if the document is NULL (not initialized yet).
// This handles the case of a WM_FONTCHANGE message at 'When' dialog during
// calendar creation.
// 
//    Rev 1.45   19 Jul 1994 17:05:40   JAY
// Semaphore to prevent reentry of draw_objects().
// 
//    Rev 1.44   15 Jul 1994 12:08:12   JAY
// access_file().
// 
//    Rev 1.43   13 Jul 1994 12:15:18   JAY
// Now calls initialize_graphics with TRUE to force them to initialize.
// 
//    Rev 1.42   07 Jul 1994 11:34:18   JAY
// Update state is correctly maintained in all cases.
// 
//    Rev 1.41   14 Jun 1994 17:00:48   JAY
// embed_pictures() define and call.
// 
//    Rev 1.40   10 Jun 1994 10:19:58   JAY
// Special registration code for CD version.
// 
//    Rev 1.39   09 Jun 1994 11:18:10   JAY
// Hooks CDCache in if it's the CD version.
// 
//    Rev 1.38   26 May 1994 13:50:24   JAY
// Fixed "*.$$$" file renaming code.
// 
//    Rev 1.37   23 May 1994 10:48:54   JAY
// Broke appropriate routines out into DOCPICT.CPP and DOCEDIT.CPP to reduce
// the size of this file and introduce some modularity.
// Added PVCS header.
*/

#include "stdafx.h"

#ifdef WIN32
//#include <..\src\oleimpl2.h>			// for _AfxRelease() and _AfxThrowOleFileExecption()
#include <..\src\afximpl.h>			// for _AfxGetSafeOwner()
#include <afxpriv.h>		// for T2COLE() macro
#else
//#include <..\src\oleimpl.h>			// for _AfxRelease() and _AfxThrowOleFileExecption()
#include <auxdata.h>		// for _AfxGetSafeOwner()
#endif

#include <stdlib.h>		// for __min() and __max() macros
#include <io.h>			// for _access()
#include <errno.h>
#include <math.h>

#include "pmw.h"
#include "mainfrm.h"
#include "system.h"
#include "paper.h"

#include "docdib.h"		// For CDIBInfo
#include "htmlconv.h"   // For HTMLDocConverter class
#include "pmwdoc.h"

#include "pmwview.h"
#include "pmwprint.h"	// for CPmwPreviewView
#include "file.h"
#include "pmcfile.h"
#include "macrorec.h"
#include "namerec.h"
#include "addrbook.h"
#include "toolbar.h"

#include "utils.h"
#include "util.h"
#include "pmgfont.h"

#include "newfont.h"
#include "prvwdlg.h"
#include "convert.h"

#include "prevrec.h"
#include "dither.h"
#include "utils.h"
#include "oleobj.h"
#include "grpobj.h"
#include "tblobj.h"
#include "frameobj.h"
#include "grafobj.h"
#include "compobj.h"

#include "cntritem.h"
#include "macrostd.h"
#include "cdibdc.h"
#include "desktop.h"
#include "register.h"
#include "printer.h"

#include "pmwcfg.h"
#include "saveproj.h"
#include "tcommand.h"
#include "compfn.h"
#include "docdib.h"     // For CDIBInfo
#include "bkgobj.h"     // For background object
#include "bmp.h"			// for InitBitmapInfoHeader
#include "DsnCheck.h"	// For CWebPageStats

#include "pmwini.h"
#include "textrec.h"

#include "cdcache.h"
#include "gif.h"			// For TRANSPARENT_INDEX

#include "commands.h"
#include "lineobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BYTE far screen_gamma_curve[];
extern CPalette *pOurPal;
extern const RGBQUAD* GetIdentityPalette(void);

CHAR CPmwDoc::tempfile_template[5+1] = "~xxxD";

PUBLIC CHAR preamble[] = "PrintMaster Gold Document\r\n\x1a";
//WORD CPmwDoc::wMaxPreviewMemory = 10240L;
WORD CPmwDoc::wMaxPreviewMemory = 0xf000;

// The name of the stream we use in the storage.

const char near CPmwDoc::DocumentStreamName[] = "PMW Document";
const char near TempStreamName[] = "PMW Temp Document";


/////////////////////////////////////////////////////////////////////////////
// OLE CFileException helpers

void AFXAPI FillOleFileException(CFileException* pError, SCODE sc)
{
	ASSERT(pError != NULL);
	ASSERT(FAILED(sc));

	int cause;  // portable CFileException.m_cause

	// error codes 255 or less are DOS/Win32 error codes
	if (SCODE_SEVERITY(sc) == SEVERITY_ERROR &&
		SCODE_FACILITY(sc) == FACILITY_STORAGE &&
		SCODE_CODE(sc) < 0x100)
	{
		ASSERT(SCODE_CODE(sc) != 0);

		// throw an exception matching to the DOS error
		//  (NOTE: only the DOS error part of the SCODE becomes m_lOsError)
		cause = CFileException::OsErrorToException(SCODE_CODE(sc));
		sc = (SCODE)SCODE_CODE(sc);
	}
	else
	{
		// attempt some conversion of storage specific error codes to generic
		//  CFileException causes...
		switch (sc)
		{
		case STG_E_INUSE:
		case STG_E_SHAREREQUIRED:
			cause = CFileException::sharingViolation;
			break;

		case STG_E_NOTCURRENT:
		case STG_E_REVERTED:
		case STG_E_CANTSAVE:
		case STG_E_OLDFORMAT:
		case STG_E_OLDDLL:
			cause = CFileException::generic;
			break;

		default:
			cause = CFileException::generic;
			break;
		}
	}

	// fill in pError
	pError->m_cause = cause;
	pError->m_lOsError = (LONG)sc;
}

void AFXAPI ThrowOleFileException(SCODE sc)
{
	// ignore non-failure codes
	if (!FAILED(sc))
		return;

	// otherwise, construct and exception and throw it
	CFileException e;
	FillOleFileException(&e, sc);
	AfxThrowFileException(e.m_cause, e.m_lOsError);
}

/////////////////////////////////////////////////////////////////////////////
// A safe release for an OLE object.

DWORD AFXAPI OleRelease(LPUNKNOWN* lplpUnknown)
{
	ASSERT(lplpUnknown != NULL);
	if (*lplpUnknown != NULL)
	{
		DWORD dwRef = (*lplpUnknown)->Release();
		*lplpUnknown = NULL;
		return dwRef;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDocCommandNotify

CPmwDocCommandNotify::CPmwDocCommandNotify()
{
	m_pDoc = NULL;
}

BOOL CPmwDocCommandNotify::Notify(CCommand* pCommand, int nNotifyCode)
{
	TRACE("Notify: %u (%d)\n", pCommand->ID(), nNotifyCode);

	if (pCommand->IsTextEditCommand()
		 && ((CBaseTextCommand*)pCommand)->GetEditIndex() != -1)
	{
		// A text command. Start us editing text (in the right frame).
		CBaseTextCommand* pTextCommand = (CBaseTextCommand*)pCommand;

		CPmwView* pView = pTextCommand->GetEditView();
		ASSERT(pView == m_pDoc->GetCurrentView());
		CTxp* pTxp = pView->GetTxp();
		CFrameObject* pFrame = pTextCommand->GetEditFrame();

		if (!pTxp->Valid() || pTxp->Object() != pFrame)
		{
			// We need to switch to editing this frame.

			// Do we need to do this? Will edit_frame() work right without it?
			if (pTxp->Valid())
			{
				pView->set_arrow_tool(TRUE);
			}

			// Begin editing the new frame.
			pView->edit_frame(pFrame);
			pTxp->Index(pTextCommand->GetEditIndex());
		}
	}
	else
	{
		// Not a text command. Make sure we are not editing text.
		CPmwView* pView = m_pDoc->GetCurrentView();
		if (pView != NULL && pView->GetTxp()->Valid())
		{
			pView->set_arrow_tool(TRUE);
		}
	}

	// Allow the command to go through.
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc

BOOL CPmwDoc::allow_edit_templates = FALSE;
BOOL CPmwDoc::make_templates = FALSE;
BOOL CPmwDoc::m_fFixTemplates = FALSE;
WORD CPmwDoc::m_wGlobalDocumentFlags = 0;
// Path of inspiration
CString       CPmwDoc::m_csNewPictureFileName;
CString       CPmwDoc::m_csNewPictureFriendlyName;
BOOL          CPmwDoc::m_fEmbedNewPicture;
BOOL          CPmwDoc::m_fCropCopyright;
CStringArray  CPmwDoc::m_csaNewSentiment;

bool CPmwDoc::m_fDropRenderActive = FALSE;

IMPLEMENT_DYNCREATE(CPmwDoc, COleDocument)
BEGIN_MESSAGE_MAP(CPmwDoc, COleDocument)
	//{{AFX_MSG_MAP(CPmwDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_REVERTTOSAVED, OnRevertToSavedFile)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVERTTOSAVED, OnUpdateRevertToSavedFile)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_ADD_PAGE, OnUpdateAddPage)
	ON_UPDATE_COMMAND_UI(ID_GOTO_PAGE, OnUpdateGotoPage)
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
//	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, INHERITED::OnUpdatePasteMenu)
//	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, INHERITED::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, INHERITED::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, INHERITED::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, INHERITED::OnUpdateObjectVerbMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, INHERITED::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, INHERITED::OnEditConvert)
	// We need to skip the COleDocument::OnFileSendMail; it does weird things.
#ifndef _MAC
	ON_COMMAND(ID_FILE_SEND_MAIL, CDocument::OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

PRIVATE VOID
db_toggle_object(LPVOID data, PMGPageObjectPtr object, SHORT which)
{
	CPmwDoc *pDoc = (CPmwDoc *)data;

	pDoc->toggle_object(object, which);
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc construction/destruction

CPmwDoc::CPmwDoc()
		: selected_list(NULL),
		  page_objects(NULL),
		  document(NULL)
{
	// Set our path manager.
	m_pPathManager = GetGlobalPathManager();
	m_pPathManager->AddRef();

	// Set our collection manager.
	m_pCollectionManager = GetGlobalCollectionManager();
	m_pCollectionManager->AddRef();

	// For most containers, using compound files is a good idea.
	EnableCompoundFile();

	// Assign our album document status.
	m_wDocumentFlags = m_wGlobalDocumentFlags;

//	Create the storage device and the database for this document.

//	mapping.device = new TmpFile((char *)tempfile_template);
//	mapping.device = new CFileFile(&m_Stream);
	mapping.device = new StorageFileOnCFile(&m_Stream);
	mapping.font_server = new PMGFontServer;
	mapping.database = new PMGDatabase(mapping.device, mapping.font_server);
	mapping.m_pMacroServer = new CStdMacroServer;

	m_DefaultProperties.SetDatabase(mapping.database);

// Backward hooks
	((PMGFontServer*)mapping.font_server)->set_database(mapping.database);

	mapping.database->SetMacroServer(mapping.m_pMacroServer);
	mapping.m_pMacroServer->SetDocument(this);

// Register our addtional type. */

// Notifies and handlers.
	mapping.database->set_refresh_notify(refresh_notify, this);
	mapping.database->set_object_toggle_handler(db_toggle_object, this);
	mapping.database->set_missing_graphic_handler(handle_missing_graphic, this);
	mapping.database->set_document_modified_handler(document_modified_handler, this);

// Instance (obsolete?).
	mapping.database->set_instance(AfxGetInstanceHandle());

// Document
	mapping.database->SetDocument(this);

	mapping.database->ShowFrameOutlines((m_wDocumentFlags & 0x1f) == 0);

	mapping.database->set_graphic_cache(&cd_cache);

	current_panel = 0;
	portion.x0 = -1;
	panel_flags = 0;
	m_fAddRefreshes = TRUE;
	refreshes_suspended = FALSE;

	document = NULL;

//	if (GetConfiguration()->SupportsAddressBook(FALSE))
	{
		mapping.m_pMacroServer->ReadReceiverFieldNames(GetPathManager()->LocatePath(GET_PMWAPP()->AddressBook()));
		mapping.m_pMacroServer->ReadSenderFieldNames(GetPathManager()->LocatePath(GET_PMWAPP()->SenderBook()));
		mapping.m_pMacroServer->ReadGlobalFieldNames(GetPathManager()->ExpandPath("PMW.FIF"));
	}

	m_pPaperInfo = NULL;

	// Page index variables.
	m_lLastKnownPageRecord = 0;
	m_lLastKnownPageIndex = (DWORD)-1;

   // Setup the command objects.
   m_CommandNotify.SetDocument(this);
   m_CommandList.SetNotify(&m_CommandNotify);

	m_pBackgroundObject = new CBackgroundObject(OBJECT_TYPE_PageBackground, get_database());

	m_fUserSave = FALSE;
}

CPmwDoc::~CPmwDoc()
{
   // Force the properties to reset now while we have a database.
   m_DefaultProperties.Reset();

   if (document != NULL)
	{
		document->release();
	}

	free_import_picture_name();
	
	mapping.FreeAll();

	if(m_pBackgroundObject)
		delete m_pBackgroundObject;

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
}

BOOL
CPmwDoc::SupportsHTML(void)
{
	switch(get_project_type())
	{
		case PROJECT_TYPE_Poster:
		case PROJECT_TYPE_Card:
		case PROJECT_TYPE_Calendar:
		case PROJECT_TYPE_Certificate:
		case PROJECT_TYPE_NoteCard:
		case PROJECT_TYPE_FaxCover:
		case PROJECT_TYPE_Stationery:
		case PROJECT_TYPE_Newsletter:
		case PROJECT_TYPE_Brochure:
		case PROJECT_TYPE_HalfCard:
		case PROJECT_TYPE_WebPub:
		case PROJECT_TYPE_PostCard:
		return TRUE;
		
		case PROJECT_TYPE_Generic:
		case PROJECT_TYPE_Banner:
		case PROJECT_TYPE_Label:
		case PROJECT_TYPE_Envelope:
		case PROJECT_TYPE_BusinessCard:
		case PROJECT_TYPE_Sticker:
		case PROJECT_TYPE_TShirt:
		case PROJECT_TYPE_Craft:
		case PROJECT_TYPE_PhotoProjects:
		default:
		return FALSE;
	}
}


BOOL
CPmwDoc::NormalHTML(void)
{
	ASSERT(SupportsHTML() == TRUE);
	
	switch(get_project_type())
	{
		case PROJECT_TYPE_Card:
		case PROJECT_TYPE_NoteCard:
		case PROJECT_TYPE_HalfCard:
		case PROJECT_TYPE_PostCard:
		return FALSE;
		
		default:
		return TRUE;
	}
}

#ifdef DUMP_PAGE
PRIVATE VOID
dump_page(ObjectListPtr list)
{
	if (list == NULL)
	{
		od("load_page returned NULL!\r\n");
	}
	else
	{
		PMGPageObjectPtr object;

		od("Got page with %d objects\r\n", list->count());	

		for (object = (PMGPageObjectPtr)list->first_object(); object != NULL; object = (PMGPageObjectPtr)object->next_object())
		{
			od("\ttype: %d, bound: %ld, %ld, %ld, %ld\r\n",
							object->type(), object->get_bound());
		}
	}
}
#endif

ERRORCODE CPmwDoc::NewDocument(ORIENTATION orientation)
{
	DOC_CREATE_STRUCT dcs;
	ERRORCODE error;

/* Create the header on the file. */

	if ((error = mapping.initialize(preamble,
 												SIGNATURE,
												make_templates
							 							? TEMPLATE_TYPE
														: FILE_TYPE,
							 					0)) != ERRORCODE_None)
	{
		return error;
	}

/* Create the initial objects. */

	ProjectPreviewPtr preview = mapping.database->create_project_preview();
//	preview->init_preview(20, 20, 300, 300);
//	preview->set_description("Sample document");

	preview->release();

/* Create a document. */

	dcs.project_type = info.project_type;
	dcs.m_nProjectSubtype = info.project_sub_type;
	dcs.orientation = orientation;

	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

	dcs.paper_width = 0;
	dcs.paper_height = 0;
	dcs.m_pPaperInfo = pApp->last_new_info.m_pPaperInfo;
	dcs.dimensions = dcs.m_pPaperInfo->GetProjectDims();

	if (dcs.orientation == LANDSCAPE)
	{
		PCOORD t = dcs.dimensions.x;
		dcs.dimensions.x = dcs.dimensions.y;
		dcs.dimensions.y = t;
	}

	if ((document = mapping.database->create_document(&dcs)) == NULL)
	{
		return mapping.database->last_creation_error();
	}

	m_pPaperInfo = document->GetPaperInfo();

	// Create the pages for our document.

	if ((error = CreatePages()) != ERRORCODE_None)
	{
		return error;
	}

/* Load it for editing. */

	ASSERT(document->NumberOfPages() > 0);
	LoadPage(document->GetPage(0));

/* Create the macro record. */

	if (!CreateMacroList())
	{
		return ERRORCODE_Memory;
	}

/* Create the name list record. */

	if (!CreateNameList())
	{
		return ERRORCODE_Memory;
	}

	ViewSampleRecord();
	
/* Create a test object. */

//	dump_page(page_objects);

	return ERRORCODE_None;
}

BOOL CPmwDoc::OnNewDocument()
{
	return DoNewDocument();
}

void
CPmwDoc::AddNewStuff (void)
{
   // JRO 5.0 hub buttons
	if (m_csNewPictureFileName.IsEmpty () == FALSE)
   {
      GRAPHIC_CREATE_STRUCT   *pGCS = get_gcs();

      pGCS->m_csFileName = m_csNewPictureFileName;

		pGCS->m_csFriendlyName = m_csNewPictureFriendlyName;
      pGCS->embed_source = m_fEmbedNewPicture;
		pGCS->m_fSpecialCrop = m_fCropCopyright;
      create_chosen_picture();

      KillNewPictureNames ();
   }

   if (m_csaNewSentiment.GetSize () > 0)
   {
      CPmwView *pView;
      if ((pView = GetCurrentView()) != NULL)
      {
         //pView->SetInitialTextStyle ();
         CMlsStringArray   LocalSentiment;
         GetSentiment      (LocalSentiment);
         PasteSentiment (LocalSentiment);
      }
      
      KillNewSentiment ();
   }
}

BOOL CPmwDoc::DoNewDocument(int nOrientation /*=-1*/, BOOL fSizeToPaper /*=TRUE*/)
{
#ifdef WIN32
	if (m_wDocumentFlags & (FLAG_ClipDocument | FLAG_AlbumDocument | FLAG_TestPrintDocument | FLAG_AddressBookDocument))
	{
		m_bEmbedded = FALSE;
	}
#endif
	// Call the default. This used to create the storage.
	// However, it was creating it in "transacted" mode, which used two files
	// handles instead of one. So we bypass that, then create it "direct" ourselves.

	m_bCompoundFile = FALSE;		// We want to create the storage ourselves.
	BOOL fSuccess = INHERITED::OnNewDocument();
	m_bCompoundFile = TRUE;			// Back to compound!
	if (!fSuccess)
		return FALSE;

// Now, create the storage for the new document.
	HRESULT hr = CreateTemporaryStorage(&m_lpRootStg);
	if (FAILED(hr))
	{
		return FALSE;
	}
	
/* Make sure we constructed properly. */

	if (mapping.database == NULL)
	{
		return FALSE;
	}

/*
// nOrientation of -1 maps to last known orientation.
*/

	if (nOrientation == -1)
	{
		nOrientation = ((CPmwApp *)AfxGetApp())->last_new_info.orientation;
	}

/* Create the stream on our storage. */

	if (!m_Stream.CreateStream(m_lpRootStg, DocumentStreamName))
	{
		return FALSE;
	}

/*
// Construct the new document.
*/

	if (NewDocument((ORIENTATION)nOrientation) != ERRORCODE_None)
	{
		return FALSE;
	}

/* Set the default text size. */

	SetDefaultTextSize();

/* Set the default typeface. */

	SetDefaultFace();

/* New documents are modified by default. */

	SetModifiedFlag();

	if (fSizeToPaper
		 && !(m_wDocumentFlags & FLAG_ClipDocument)
		 && !(m_wDocumentFlags & FLAG_AlbumDocument))
	{
		size_to_paper();
	}

#if 0
/*
// A test!!!!!
*/

	PMGDatabase* pDatabase = get_database();
	
	CTextRecord* pRecord = (CTextRecord*)pDatabase->new_record(RECORD_TYPE_Text, NULL);
	if (pRecord == NULL)
	{
		TRACE("new Text record failed! (%d)\n", pDatabase->last_creation_error());
	}
	pRecord->SelfTest();
	pRecord->zap();

/*
// End of a test!!!!!
*/
#endif
   return TRUE;
}

void CPmwDoc::OnCloseDocument(void)
{
	ClearAllCommands();

/*
// For some reason, COleDocument::OnCloseDocument() releases the storage.
// We need to keep the storage around until destruct time because we have a stream open on it.
// To that end, we do an AddRef() on the storage before calling OnCloseDocument(). Since OnCloseDocument()
// calls OleRelease() which NULLs the pointer, we also need to save and restore m_lpRootStg.
//
// I am assuming that the above is happening. This means that on return, m_lpRootStg should be NULL.
// That is the normal case and the case that I am expecting.
// If for some reason m_lpRootStg is not NULL on return, then I don't know what is going on, so I leave
// m_lpRootStg with the new value and just release the storage that was saved.
// Hopefully the ASSERT will catch this case...
*/

	// Close our stream *before* releasing the storage.
	m_Stream.Close();

	LPSTORAGE lpStorageSave = m_lpRootStg;
	if (lpStorageSave != NULL)
	{
		lpStorageSave->AddRef();
	}

	INHERITED::OnCloseDocument();

	OleRelease((LPUNKNOWN*)&lpStorageSave);
}

void CPmwDoc::SetPathName(const char* pszPathName, BOOL bAddToMRU /*=TRUE*/)
{
	INHERITED::SetPathName(pszPathName,
								  bAddToMRU
										&& !(m_wDocumentFlags & (FLAG_AlbumDocument|FLAG_ClipDocument|FLAG_WithHelpDocument|FLAG_NoMRU)));
}

BOOL CPmwDoc::CreateNameList()
{
/*
// Create the name list record.
*/

	NameListRecord* pNameRecord;
	if ((pNameRecord = (NameListRecord*)get_database()->new_record(RECORD_TYPE_NameList, NULL)) != NULL)
	{
		ASSERT(document != NULL);
		if (document != NULL)
		{
			document->set_name_list_record(pNameRecord->Id());
		}

	/*
	// Hook us into the macro server.
	*/

		mapping.m_pMacroServer->SetNameList(pNameRecord);

	/*
	// Set our address book name.
	*/

		CString csName = GetPathManager()->ExpandPath(GET_PMWAPP()->AddressBook());
//		TRACE("Set address book name to %s on create.\r\n", pName);

		CString csTemp;
		csTemp.LoadString(IDS_LOC_RELATIONSHIP);
		pNameRecord->AddressBookName(csName);
		pNameRecord->SortField(csTemp);

		return TRUE;
	}
	return FALSE;
}

BOOL CPmwDoc::GetNameList(void)
{
	DB_RECORD_NUMBER record;
	if ((record = document->get_name_list_record()) != 0)
	{
		NameListRecord* pNameRecord;

		if ((pNameRecord = (NameListRecord*)get_database()->get_record(record, NULL, RECORD_TYPE_NameList)) != NULL)
		{
			// If the name list record is from a previous version, fix it up.
			if (pNameRecord->GetVersion() < 3)
			{
				// All the ids are out of date, so dump 'em.
				pNameRecord->DeleteAllNames();

				// Convert the address book name.
				if (!pNameRecord->AddressBookName().IsEmpty())
				{
					CString csFile;
					CString csExtension;
					CString csFullName;
					TRY
					{
						Util::SplitPath(pNameRecord->AddressBookName(), NULL, NULL, &csFile, &csExtension);
						CAddressBook::GetAddressBookFullName(csFile, csFullName);
						pNameRecord->AddressBookName(csFullName);
						pNameRecord->modified();
					}
					END_TRY
				}

				// Convert the sort index.
				CString csSortIndex;
				TRY
				{
					CAddressBook::ConvertVersion2IndexName(pNameRecord->SortField(), csSortIndex);
					pNameRecord->SortField(csSortIndex);
					pNameRecord->modified();
				}
				END_TRY
			}

			mapping.m_pMacroServer->SetNameList(pNameRecord);

			// If the address book paths are not the same, then use the default address book.
			if (pNameRecord->AddressBookName() != "")
			{
				CString csPath;
				Util::SplitPath(pNameRecord->AddressBookName(), &csPath, NULL);
				csPath = GetPathManager()->ExpandPath(csPath);
				Util::RemoveBackslashFromPath(csPath);
				csPath.MakeUpper();

				CString csAddressBookPath = GetPathManager()->ExpandPath("[[A]]");
				Util::RemoveBackslashFromPath(csAddressBookPath);
				csAddressBookPath.MakeUpper();

				if (csPath != csAddressBookPath)
				{
					pNameRecord->AddressBookName("");
					pNameRecord->modified();
				}
			}

			// If the document does not specify an address book, use the default.
			if (pNameRecord->AddressBookName() == "")
			{
				// We're switching address books, so any id's do not relate.
				pNameRecord->DeleteAllNames();

				CString csName = GetPathManager()->LocatePath(GET_PMWAPP()->AddressBook());
//				TRACE("Set address book name to %s on load.\r\n", pName);

				pNameRecord->AddressBookName(csName);
				pNameRecord->modified();
			}
			return TRUE;
		}
	}

	return CreateNameList();
}

/*
// Register any fonts which are in use.
*/

BOOL CPmwDoc::register_used_fonts(BOOL has_reg_items)
{
	if (document == NULL)
	{
		return FALSE;
	}

	PMGFontServer* font_server = (PMGFontServer*)mapping.font_server;
	DWORD dwCount = document->NumberOfFonts();
	DWORD i;

/* Run through all faces in the array. */

	for (i = 0; i < dwCount; i++)
	{
		DB_RECORD_NUMBER record = document->GetFont(i);
		int face_index = font_server->font_record_to_face(record);
		FaceEntry* entry;

		if ((face_index != -1)
		 && ((entry = typeface_server.get_face(face_index)) != NULL))
		{
			if (entry->fake_register())
			{
			/* This font is a registration font. */
				has_reg_items = TRUE;
			}
		}
	}

	if (has_reg_items)
	{
		((CMainFrame*)AfxGetMainWnd())->GetToolBar(TOOLBAR_FORMAT)->SendMessage(UM_UPDATE_FONTS);
	}

	return has_reg_items;
}

void CPmwDoc::handle_registration_items(void)
{
/* See if the user is registered. If so, we don't need to check anything. */

	if (GET_PMWAPP()->user_is_registered())
	{
	/* Nothing to do! The guy's legal. */
		return;
	}

/*
// The user is not registered. (Bad user! Bad! Bad!)
// Run through the font and graphics lists to see if any referenced fonts
// are in the registration set.
*/
	BOOL has_reg_items = FALSE;

/*
// Handle fonts first. This is the easier case and we need to run
// through them all.
*/

	has_reg_items = register_used_fonts(has_reg_items);
/*
// We do not put up this message for auto-load documents.
// It confuses the operating system.
*/

	if (has_reg_items)
	{
		CUnregisteredItemsDialog Dialog;
		if (Dialog.DoModal() == IDOK)
		{
			((CMainFrame*)AfxGetMainWnd())->DoFileFreeFonts();
		}
	}
}

BOOL CPmwDoc::OnOpenDocument(const char *pszPathName)
{                                       
/* Make sure we constructed properly. */

	if (mapping.database == NULL)
	{
		return FALSE;
	}

	SetModifiedFlag(TRUE);  // dirty during de-serialize

/*
// See what kind of file this is (i.e storage vs. non-storage)
// The name is assumed to be fully bound at this point.
*/

	TRY
	{
#ifdef WIN32
		USES_CONVERSION;
		HRESULT hr = StgIsStorageFile(T2COLE(pszPathName));
#else
		HRESULT hr = StgIsStorageFile(pszPathName);
#endif
	
		switch (GetScode(hr))
		{
			case S_OK:
			{
			/* It is a storage file. */
//				if (m_wDocumentFlags & FLAG_AlbumDocument)
//				{
//					// Open. We will be read only!
//					hr = OpenStorageFile(pszPathName);
//				}
//				else
				{
					// Copy to temp storage so we can edit it.
					hr = CopyStorageToTemp(pszPathName);
				}

				if (FAILED(hr))
				{
					ThrowOleFileException(hr);
				}
	
				TRY
				{
				/*
				// De-serialize the normal contents stream.
				// This gives us our OLE items.
				*/
					LoadFromStorage();
				}
				CATCH_ALL(e)
				{
					OleRelease((LPUNKNOWN*)&m_lpRootStg);
					THROW_LAST();
				}
				END_CATCH_ALL
				break;
			}
			case S_FALSE:
			{
			/*
			// It is not a storage file.
			// We need to convert this to a temporary storage file.
			*/
				hr = ConvertFileToStorage(pszPathName);
				
				if (FAILED(hr))
				{
					ThrowOleFileException(hr);
				}
				break;
			}
			default:
			{
			/* Some other error. */
				ThrowOleFileException(hr);
			}
		}
	
	/* Open the stream on our storage. */
	
		CFileException* pException = new CFileException;
	
//		if (m_wDocumentFlags & FLAG_AlbumDocument)
//		{
//			// Open read-only.
//			if (!m_Stream.OpenStream(m_lpRootStg, DocumentStreamName, CFile::modeRead|CFile::shareExclusive, pException))
//			{
//				THROW(pException);
//			}
//		}
//		else
		{
			if (!m_Stream.OpenStream(m_lpRootStg, DocumentStreamName, CFile::modeReadWrite|CFile::shareExclusive, pException))
			{
				THROW(pException);
			}
		}
		
#ifdef WIN32
		pException->Delete();
#else
		delete pException;
#endif
		
		pException = NULL;
	
		if (!ReadDocument())
		{
			AfxThrowFileException(CFileException::generic);
		}
	
	/* Make sure we're the correct size. */
	
		if (!(m_wDocumentFlags & FLAG_ClipDocument))
		{
			if (!(m_wDocumentFlags & FLAG_AlbumDocument)
					|| m_fFixTemplates
					|| m_pPaperInfo->SlotDims().x == MakeFixed(0)
					|| m_pPaperInfo->SlotDims().y == MakeFixed(0))
			{
				size_to_paper();
			}
		}

	/*
	// Standard files loaded from the home CD are always considered templates
	// regardless of internal type.
	*/

      if (m_wDocumentFlags & FLAG_WithHelpDocument)
      {
         file_type = TEMPLATE_TYPE;
         SetModifiedFlag();
      }
      
	/* Handle the types we can run into. */
	
		switch (file_type)
		{
			case TEMPLATE_TYPE:
			{
				if (allow_edit_templates)
				{
				/* Act the same as a normal document. */
					break;
				}
			/* Fall through to... */
			}
			case OLD_FILE_TYPE:
			case OLD_TEMPLATE_TYPE:
			{
	//			m_strPathName.Empty();      // no path name yet
			// It gets emptied later during save.
	
				if (file_type != TEMPLATE_TYPE)
				{
					SetModifiedFlag();
				}
				break;
			}
			default:
			{
				break;
			}
		}
	
	/* Set the default text size. */
	
		SetDefaultTextSize();
	
	/* Set the default typeface. */
	
		SetDefaultFace();
	
		if (!(m_wDocumentFlags & (FLAG_AlbumDocument|FLAG_ClipDocument|FLAG_WithHelpDocument)))
		{
			handle_registration_items();
		}
		if(GET_PMWAPP()->AutoInstructionsEnabled() && GetConfiguration()->SupportsInstructionPage())
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_GOTO_INSTRUCTION_PAGE);
	}
	CATCH_ALL(e)
	{
		TRY
		{
			int nMsg = AFX_IDP_FAILED_TO_OPEN_DOC;

			if (e->IsKindOf(RUNTIME_CLASS(CFileException)))
			{
				switch (((CFileException*)e)->m_cause)
				{
					case CFileException::fileNotFound:
					case CFileException::badPath:
					{
						nMsg = AFX_IDP_FAILED_INVALID_PATH;
						break;
					}
					case CFileException::diskFull:
					{
						nMsg = AFX_IDP_FAILED_DISK_FULL;
						break;
					}
					case CFileException::accessDenied:
					{
						nMsg = AFX_IDP_FAILED_ACCESS_READ;
						break;
					}
					case CFileException::badSeek:
					case CFileException::generic:
					case CFileException::tooManyOpenFiles:
					case CFileException::invalidFile:
					case CFileException::hardIO:
					case CFileException::directoryFull:
					{
						nMsg = AFX_IDP_FAILED_IO_ERROR_READ;
						break;
					}
					default:
					{
						break;
					}
				}
			}
			
			CString csMsg;
			
			TRY
			{
				AfxFormatString1(csMsg, nMsg, pszPathName);
				AfxMessageBox(csMsg);
			}
			END_TRY
		}
		END_TRY
		
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;
}

/*
// Return the default text size.
// The size passed in is what is suggested (by the user's INI file).
*/

int CPmwDoc::DefaultTextSize(int nDefault)
{
	return nDefault;
}

/*
// Set the default text size.
// This will call the virtual DefaultTextSize() function.
*/

void CPmwDoc::SetDefaultTextSize(void)
{
	PMGFontServer* pServer = (PMGFontServer*)get_font_server();

	pServer->m_nDefaultPointSize = DefaultTextSize(PMGFontServer::default_point_size);
}

/*
// Return the default face.
*/

LPCSTR CPmwDoc::DefaultFace()
{
   return PMGFontServer::default_face;
}

/*
// Set the default typeface.
// This will call the virtual DefaultFace() function.
*/

void CPmwDoc::SetDefaultFace(void)
{
	PMGFontServer* pServer = (PMGFontServer*)get_font_server();

	pServer->SetDefaultFace(DefaultFace());
}

/*
// Copy a storage file to a temporary storage file so we can edit it.
*/

HRESULT CPmwDoc::CopyStorageToTemp(LPCSTR pszPathName)
{
	LPSTORAGE pOldStorage;

/*
// Create our temporary storage.
*/

	HRESULT hr = CreateTemporaryStorage(&m_lpRootStg);
	if (FAILED(hr))
	{
		return hr;
	}

/*
// Open our source storage.
*/

#ifdef WIN32
	USES_CONVERSION;
	hr = ::StgOpenStorage(T2COLE(pszPathName),
#else
	hr = ::StgOpenStorage(pszPathName,
#endif
								 NULL,
								 STGM_READ | STGM_SHARE_DENY_WRITE | STGM_DIRECT,
								 0,
								 0,
								 &pOldStorage);
	if (FAILED(hr))
	{
		OleRelease((LPUNKNOWN*)&m_lpRootStg);
		return hr;
	}

/*
// Copy the source storage to our temp storage.
*/

	hr = pOldStorage->CopyTo(0, 0, 0, m_lpRootStg);

	pOldStorage->Release();

	if (FAILED(hr))
	{
		OleRelease((LPUNKNOWN*)&m_lpRootStg);
		return hr;
	}

	return hr;
}

/*
// Open a storage file so we can view it.
*/

HRESULT CPmwDoc::OpenStorageFile(LPCSTR pszPathName)
{
/*
// Open our source storage.
*/

#ifdef WIN32
	USES_CONVERSION;
	return ::StgOpenStorage(T2COLE(pszPathName),
#else
	return ::StgOpenStorage(pszPathName,
#endif
									NULL,
									STGM_READ | STGM_SHARE_DENY_WRITE | STGM_DIRECT,
									0,
									0,
									&m_lpRootStg);

}

/*
// Create a temporary storage
*/

HRESULT CPmwDoc::CreateTemporaryStorage(LPSTORAGE* ppStorage)
{
	CString csName;

/*
// Get a temporary name for this document.
// We use the fact that a TmpFile generates a tmp file name at construct
// time. So we just take that name and use it elsewhere.
*/
	// Nest a level to force 'tf' to go out of scope when we're done with it.
	{
		TmpFile tf(tempfile_template);
		csName = tf.get_name();
		// And tf is now destructed.
	}

/*
// Create a temporary storage.
// This will be our edit file.
*/

#ifdef WIN32
	USES_CONVERSION;
	return StgCreateDocfile(T2COLE(csName),
#else
	return StgCreateDocfile(csName,
#endif
									  STGM_DELETEONRELEASE
									| STGM_READWRITE
									| STGM_DIRECT
									| STGM_SHARE_EXCLUSIVE
									| STGM_CREATE,
									0,
									ppStorage);
}

/*
// Convert an old document to a storage file.
*/

HRESULT CPmwDoc::ConvertFileToStorage(LPCSTR pszPathName)
{
/*
// Implementation notes:
//
// My first attempt at this was to copy the source file to a temp file
// in non-compound form. Then, I would use StgCreateDocfile with STGM_CONVERT
// to convert the non-compound form to compound form. Unfortunately,
// STGM_CONVERT seems to be incompatible with STGM_DELETEONRELEASE, though this
// fact is not documented anywhere. I felt it was very important (if not
// essential) for the storage file to clean itself up when released instead of
// creating additional code to manage that (which would increase the chances
// of it not getting cleaned up properly).
//
// So now we have method #2. We create an empty temp storage file, then use
// the same copy method to transfer the non-compound file to its final
// data stream in the storage. This may actually be more efficient, and we are
// able to use STGM_DELETEONRELEASE when we create the storage file.
*/

	OleRelease((LPUNKNOWN*)&m_lpRootStg);

	HRESULT hr = CreateTemporaryStorage(&m_lpRootStg);

	if (FAILED(hr))
	{
		return hr;
	}

/*
// Create a stream on this storage.
*/

	COleStreamFile Stream;

	if (!Stream.CreateStream(m_lpRootStg, DocumentStreamName))
	{
		OleRelease((LPUNKNOWN*)&m_lpRootStg);
		return hr;
	}

/*
// Copy the document over to this temporary file.
*/

	ReadOnlyFile SourceFile(pszPathName);
	StorageFileOnCFile DestinationFile(&Stream);

	if (copy_file(&SourceFile, &DestinationFile) != ERRORCODE_None)
	{
		Stream.Close();
		OleRelease((LPUNKNOWN*)&m_lpRootStg);
		return hr;
	}

	Stream.Close();

/*
// And we have succeeded.
*/

	return hr;
}

BOOL CPmwDoc::OnSaveDocument(const char *pszPathName)
{                                       
/* Free the undo states. This causes all objects in limbo to be destroyed. */

	ClearAllCommands();

	document->modified();				/* Always write the document record. */

	if (!WriteDocument())
	{
		return FALSE;
	}

	/* Reload the page objects. This marks the list as modified again. */

	LoadPage(-1);

	/* Full document save. */
	mapping.device->flush();

//	BOOL fRet = INHERITED::OnSaveDocument(pszPathName);

	LPSTORAGE pNewStorage = NULL;

	TRY
	{
		// use helper to save to root storage
		SaveToStorage();

		// commit each of the items
		CommitItems(TRUE);

		/*
		// Create the destination storage.
		*/

#ifdef WIN32
		USES_CONVERSION;
		HRESULT hr = ::StgCreateDocfile(T2COLE(pszPathName),
#else
		HRESULT hr = ::StgCreateDocfile(pszPathName,
#endif
													 STGM_READWRITE
												  | STGM_TRANSACTED
												  | STGM_SHARE_EXCLUSIVE
												  | STGM_CREATE,
												  0,
												  &pNewStorage);
		if (FAILED(hr))
		{
			ThrowOleFileException(hr);
		}

		// Copy the data to the new storage.

		hr = m_lpRootStg->CopyTo(0, 0, 0, pNewStorage);
		if (FAILED(hr))
		{
			ThrowOleFileException(hr);
		}

		hr = pNewStorage->Commit(STGC_ONLYIFCURRENT);
		if (FAILED(hr))
		{
			ThrowOleFileException(hr);
		}
		
		pNewStorage->Release();
	}
	CATCH_ALL(e)
	{
		if (pNewStorage != NULL)
		{
			pNewStorage->Release();
		}

		TRY
		{
			int nMsg = AFX_IDP_FAILED_TO_SAVE_DOC;
			
			if (e->IsKindOf(RUNTIME_CLASS(CFileException)))
			{
				switch (((CFileException*)e)->m_cause)
				{
					case CFileException::fileNotFound:
					case CFileException::badPath:
					{
						nMsg = IDS_ErrInvalidFileName;
						break;
					}
					case CFileException::diskFull:
					{
						nMsg = AFX_IDP_FAILED_DISK_FULL;
						break;
					}
					case CFileException::accessDenied:
					{
						nMsg = AFX_IDP_FAILED_ACCESS_WRITE;
						break;
					}
					case CFileException::badSeek:
					case CFileException::generic:
					case CFileException::tooManyOpenFiles:
					case CFileException::invalidFile:
					case CFileException::hardIO:
					case CFileException::directoryFull:
					{
						nMsg = AFX_IDP_FAILED_IO_ERROR_WRITE;
						break;
					}
					default:
					{
						break;
					}
				}
			}
			
			CString csMsg;
			
			TRY
			{
				AfxFormatString1(csMsg, nMsg, pszPathName);
				AfxMessageBox(csMsg);
			}
			END_TRY
		}
		END_TRY

		return FALSE;
	}
	END_CATCH_ALL

	SetModifiedFlag(FALSE);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc server implementation

COleServerItem* CPmwDoc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem is called by the framework to get the COleServerItem
	//  that is associated with the document.  It is only called when necessary.

//	COletestSrvrItem* pItem = new COletestSrvrItem(this);
//	ASSERT_VALID(pItem);
//	return pItem;
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc serialization

void CPmwDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		// you must save client items as well
	}
	else
	{
		// TODO: add loading code here
		// you must load client items as well
	}
	INHERITED::Serialize(ar);
}

void CPmwDoc::DeleteContents()
{
	// TODO: add additional cleanup before doc-items are deleted
	INHERITED::DeleteContents();	// delete doc-items
}

BOOL CPmwDoc::SaveModified()
{
#ifdef FIXTEMPLATES
	if (!m_fFixTemplates)
#endif
	{
		if (!GetConfiguration()->SupportsSaveProject(FALSE))
		{
			return TRUE;        // ok to continue
		}
	}

	if (!IsModified())
		return TRUE;        // ok to continue

	CString name = m_strPathName;
	if (name.IsEmpty())
	{
		name = m_strTitle;
	}

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
		case IDCANCEL:
			return FALSE;       // don't continue

		case IDYES:
			// If so, either Save or Update, as appropriate
			if (!DoSave(m_strPathName))
				return FALSE;       // don't continue
			break;

		case IDNO:
			// If not saving changes, revert the document
			break;

		default:
			ASSERT(FALSE);
			break;
	}
	return TRUE;    // keep going
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc diagnostics

#ifdef _DEBUG
void CPmwDoc::AssertValid() const
{
	INHERITED::AssertValid();
}

void CPmwDoc::Dump(CDumpContext& dc) const
{
	INHERITED::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc document read

BOOL CPmwDoc::ReadDocument(void)
{
	ERRORCODE error;

/* Get the image of this file from the permanent file. */


	SHORT signature;
	SHORT file_flags;

	if (parse_document_preamble(mapping.device, &signature,
		 				&file_type, &file_flags) != ERRORCODE_None)
	{
		return FALSE;
	}

	BOOL ret = FALSE;

	switch (signature)
	{
		case SIGNATURE:
		{
		/* Read the database state from this file. */

			mapping.database->read_state();

		/* Load the document object. */

			if ((document = (DocumentPtr)mapping.database->get_record(PMG_DOCUMENT_RECORD, &error, RECORD_TYPE_Document)) == NULL)
			{
				od("Got error %d getting the document on load.\r\n", error);
			}
			else
			{
				m_pPaperInfo = document->GetPaperInfo();

			/* Initialize the document fonts. */

				document->initialize_font_records(TRUE);
//				document->initialize_graphic_records(!(m_wDocumentFlags & (FLAG_AlbumDocument|FLAG_ClipDocument)));

			/* Get the macro list. */

				if (!GetMacroList())
				{
					return FALSE;
				}

			/* Get the name list. */

				if (!GetNameList())
				{
					return FALSE;
				}

			/* Load our page. */

				LoadPage();

			/* Make sure we're viewing the correct data. */

				ViewSampleRecord();

//				dump_page(page_objects);

				ret = TRUE;
			}

			SetModifiedFlag(FALSE);
		
			// now that the document is read make font substitutions if neccessary
			MakeFontSubstitutions();

			break;
		}
		case OLD_SIGNATURE:
		{
		/*
 		// Convert to the new format.
		// The old device currently mapped into the database is not
		// appropriate for the database. Create a new one which the database
		// will then use when it's done converting.
 		*/

			COleStreamFile SourceStream;

			// A little cheating to swap our streams around.
			SourceStream.m_lpStream = m_Stream.m_lpStream;
			m_Stream.m_lpStream = NULL;

			StorageFileOnCFile* pOldDevice = (StorageFileOnCFile*)mapping.device;
			pOldDevice->SetCFile(&SourceStream);

			mapping.device = new StorageFileOnCFile(&m_Stream);

			if (!m_Stream.CreateStream(m_lpRootStg, TempStreamName))
			{
				delete pOldDevice;
				return FALSE;
			}

		/* Force the database to use this new device. */

			if (mapping.database->force_new_device(mapping.device) == ERRORCODE_None)
			{
				ERRORCODE error;

				if ((error = NewDocument()) != ERRORCODE_None)
				{
					od("Got error %d creating a new document\r\n", error);
				}
				else
				{

				/*
				// Converted documents are modified by default.
				*/

					SetModifiedFlag();

				/*
				// Copy all possible information over.
				*/

					CPmwApp* pApp = GET_PMWAPP();
					CWnd *main_wnd = pApp->m_pMainWnd;
					CConvertingDialog cdlg(main_wnd);

				/*
				// Throw up the cancel dialog.
				*/

					cdlg.ShowWindow(SW_SHOW);
	//				cdlg.UpdateWindow();
					main_wnd->EnableWindow(FALSE);    // Disable main window

					PBOX Margins = m_pPaperInfo->GetPageMargins();
					RECT rMargins;
					rMargins.left = (int)Margins.x0;
					rMargins.top = (int)Margins.y0;
					rMargins.right = (int)Margins.x1;
					rMargins.bottom = (int)Margins.y1;

					if (mapping.database->convert_old_document(signature, pOldDevice, &info, &rMargins) == ERRORCODE_None)
					{
						ret = TRUE;
					}

					main_wnd->EnableWindow(TRUE);    // Enable main window
					cdlg.DestroyWindow();
				}
			}

		/* Get rid of the device corresponding to the old format file. */

			delete pOldDevice;

		/* Rename the streams. */

			if (ret)
			{
			/* Success! */
#ifdef WIN32
				USES_CONVERSION;
				HRESULT hr = m_lpRootStg->DestroyElement(T2COLE(DocumentStreamName));
#else
				HRESULT hr = m_lpRootStg->DestroyElement(DocumentStreamName);
#endif
				if (FAILED(hr))
				{
					TRACE("Unable to destroy old stream!\r\n");
					return FALSE;
				}

			/* Close the stream and rename it. */

				m_Stream.Close();
#ifdef WIN32
				hr = m_lpRootStg->RenameElement(T2COLE(TempStreamName), T2COLE(DocumentStreamName));
#else
				hr = m_lpRootStg->RenameElement(TempStreamName, DocumentStreamName);
#endif
				if (FAILED(hr))
				{
					TRACE("Unable to rename new stream!\r\n");
					return FALSE;
				}

			/* Open the stream with the new name. */

				if (!m_Stream.OpenStream(m_lpRootStg, DocumentStreamName))
				{
					TRACE("Unable to reopen renamed stream.\r\n");
					return FALSE;
				}

			/* Create the macro list. */

				if (!CreateMacroList())
				{
					TRACE("Unable to create macro list.\r\n");
					return FALSE;
				}

			/* Create the name list. */

				if (!CreateNameList())
				{
					TRACE("Unable to create name list.\r\n");
					return FALSE;
				}
			}

			// now that the document is read make font substitutions if neccessary
			MakeFontSubstitutions();

			break;
		}
		default:
		{
		/* Bad or unrecognized format. */
			break;
		}
	}
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc document flush

BOOL CPmwDoc::FlushDocument(VOID)
{
	return mapping.flush_database() == ERRORCODE_None;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc document write

BOOL CPmwDoc::WriteDocument(void)
{
	if (FlushDocument())
	{
		return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc commands

/////////////////////////////////////////////////////////////////////////////
// CPmwDoc operations

VOID CPmwDoc::set_portion(PBOX_PTR pbox)
{
	if (pbox == NULL)
	{
		if (get_project_type() == PROJECT_TYPE_Banner || get_project_type() == PROJECT_TYPE_Calendar)
		{
			PPNT dimensions = get_dimensions();

			portion.x0 = 0;
			portion.y0 = 0;
			portion.x1 = dimensions.x;
			portion.y1 = dimensions.y;
		}
		else
		{
			CFixedRect Margins = m_pPaperInfo->Margins();
			CFixedPoint Dims = m_pPaperInfo->SlotDims();
			
			CFixedRect Bounds;
	
			if (get_orientation() == LANDSCAPE)
			{
				GET_PMWAPP()->GetPrinterInfo(LANDSCAPE)->OrientRect(&Margins);
				
				CFixed t = Dims.x;
				Dims.x = Dims.y;
				Dims.y = t;
			}
	
   		Bounds.Left = -Margins.Left;
			Bounds.Top = -Margins.Top;
			Bounds.Right = Bounds.Left + Dims.x;
			Bounds.Bottom	= Bounds.Top + Dims.y;

			portion.x0 = MulFixed(Bounds.Left, PAGE_RESOLUTION);
			portion.y0 = MulFixed(Bounds.Top, PAGE_RESOLUTION);
			portion.x1 = MulFixed(Bounds.Right, PAGE_RESOLUTION);
			portion.y1 = MulFixed(Bounds.Bottom, PAGE_RESOLUTION);
		}
	}
	else
	{
		portion = *pbox;
	}
}

// Draw objects into a redisplay context.
// Returns whether it got interrupted or not.
//
//		List				list of objects
//		pre				pointer to refresh extent
//		rc					pointer to rc
//		upto_object		object at which to stop drawing -- used in MODETEXT.CPP which
//								creates the edit background
//		pDoc				pointer to CPmwDoc object

BOOL DrawObjects(CPtrList *List, REFRESH_EXTENT_PTR pre, RedisplayContext *rc, CPmwDoc* pDoc)
{

	// See if we need to draw the objects in a special way.
	if ((pDoc != NULL)						// We need a document
		 && (rc->RequiresBitmaps() == TRUE))
	{
		int nResult = DrawObjectsAsBitmaps(List, pre, rc, pDoc);
		if (nResult >= 0)
		{
			return nResult;
		}
	}

	PMGPageObjectPtr object;
	UpdateStatePtr update_state;
	PBOX rect, area, test_rect;
	RECT clip;
	BOOL did_erase = FALSE;
	BOOL opaque_erase = FALSE;
	PMGPageObject* pOpaqueObject = NULL;
	BOOL result;
	BOOL using_images = rc->using_images;

	if (rc->is_printing)
	{
		// This is a workaround for a bug in certain DeskJet drivers.
		// On the DeskJet 890C (and maybe others), text on top of an outline graphic
		// was drawing incorrectly (with odd colors in the text background). It turns out
		// that having a raster bitmap on the same page fixed the problem! For
		// some reason, having the bitmap there tweaked the driver the right way.
		// Sending a single pixel bitmap to the printer fixes the problem (works
		// around the bug). So that is what we now always do.
		BYTE bData[4] = { 255, 255, 255, 0 };		// Padded single RGB pixel.
		BITMAPINFOHEADER bmiHeader;
		InitBitmapInfoHeader(&bmiHeader, 1, 1, 24);
		::StretchDIBits(rc->destination_hdc, 
							 0,									// DstX
							 0,									// DstY
							 1,									// DstWidth
							 1,									// DstHeight
							 0,									// SrcX
							 0,									// SrcY
							 1,									// SrcWidth
							 1,									// SrcHeight
							 bData,								// Data
							 (BITMAPINFO far *)&bmiHeader,
							 DIB_RGB_COLORS,
							 SRCCOPY);
	}
	// Get the extent.
	rect = pre->extent;
	object = pre->update_object;

	// Intersect it with our source extent (the extent visible through the RC.)
	if (!IntersectBox(&rect, &rect, &rc->source_pbox))
	{
		// The refresh extent is completely outside the source pbox (that area
		// currently shown in the view.) So, there is no visible area to refresh.
		ASSERT(pre->update_state == NULL || pre->update_state->object == object);
		if (object != NULL)
		{
			// Delete any current update state belonging to the update object.
			// PMGTODO: Document the case where the object in the update state
			// is not the same as the object in the refresh extent. Plus, who
			// frees the update state in the case where its object is not the
			// same as the refresh extent object?
			delete pre->update_state;
			pre->update_state = NULL;
		}
		return FALSE;
	}

	// Some part of the object overlaps the source pbox.

	// rect = full refresh extent intersected with source pbox (client rectangle)

	// Convert the extent to screen coordinates.
	area = rect;
	rc->pbox_to_screen(&area, TRUE);
	rc->convert_pbox(&area, &clip, NULL);

	// clip = RECT version of full refresh extent intersected with
	//        source pbox converted to screen coordinates

	// Bump out bounds by one pixel on each side (test_rect).
	// PMGTODO: Document why this is done? To catch cases where
	// single pixel objects (such as calendar lines) dissapear?
	test_rect.x0 = rect.x0 - rc->redisplay_x_pixel;
	test_rect.y0 = rect.y0 - rc->redisplay_y_pixel;
	test_rect.x1 = rect.x1 + rc->redisplay_x_pixel;
	test_rect.y1 = rect.y1 + rc->redisplay_y_pixel;

	// test_rect = full refresh extent intersected with source
	//             pbox (client rectangle) bumped out one pixel
	//             on each side

	// Now draw the objects.

	// "object" is the update object associated with the refresh exent.
	// If it's NULL, then there is no update state (we aren't restarting
	// after an interrupt.)
	// PMGTODO: We're testing the object in the refresh extent, and using
	// that to make a decision about whether we are restarting after an
	// interrupt. Why aren't we checking if there is an update state? And
	// can the update object in the refresh extent be different than
	// the object attached to the update state?

	// "Pos" is the position in the list (as defined by MFC).
	// It corresponds to the *next* object (after calling GetNext() on the list).
	// Since the MFC list works that way, it seems easier to follow.
	POSITION Pos = NULL;
	if (object == NULL)
	{
		// No update state (not restarting after interrupt.)
		update_state = NULL;

		// Start at first object in object list.
		Pos = List->GetHeadPosition();
		object = (Pos == NULL) ? NULL : (PMGPageObject*)List->GetNext(Pos);

		// See if an opaque object completely contains this extent.
		// If so, then we can start drawing at that object since
		// nothing below is visible.

		POSITION SearchPos = Pos;
		PMGPageObjectPtr search_object = object;

		while (search_object != rc->m_pUptoObject && search_object != NULL)
		{
			PBOX bound;
			search_object->get_refresh_bound(&bound, rc);

			// Check if we found an opaque object that completely contains
			// the refresh extent. If so, then we can start drawing at this
			// object. Plus (special added bonus) we can use the object to
			// erase the background.
			//
			// NOTE: A frame object which is being edited and has a background
			// bitmap is considered opaque. So, if a top level text frame
			// is being edited, the redraw will start with it, the proper
			// thing to do because all objects under the text frame have already
			// been drawn into the background bitmap.

			if (((search_object->get_refresh_flags() & REFRESH_FLAG_opaque)
						|| search_object->is_opaque(rc))
				&& IsBoxInBox(&rect, &bound)
				&& !(search_object->get_flags() & OBJECT_FLAG_hidden))
			{
				object = search_object;
				Pos = SearchPos;
				did_erase = TRUE;				// basically, we are going to erase with the opaque object
				opaque_erase = TRUE;
				pOpaqueObject = search_object;
			}
			search_object = (SearchPos == NULL) ? NULL : (PMGPageObjectPtr)List->GetNext(SearchPos);
		}

		// Erase the background if needed. If we have an opaque object, did_erase
		// will be true and we will be "erasing" with the opaque object.
		//
		// NOTE: We only erase if the refresh type is REFRESH_ALL. If the refresh
		// type is REFRESH_ERASE or REFRESH_REDRAW, then the update function is
		// responsible for doing any needed erasing.
		if (!did_erase && (pre->refresh_type == REFRESH_ALL))
		{
			// Do the erase now. This eventually calls the current desktop object
			// (or derived class thereof) to draw itself. This means the page color,
			// and page guided or shadows, etc.
			rc->erase_area(clip);
			did_erase = TRUE;
		}
	}
	else
	{
		// Pick up the previous update state.
		// We are restarting after an interrupt.
		update_state = pre->update_state;
		did_erase = pre->did_erase;

		// Find the position for the continuing object.
		Pos = List->Find(object);
		ASSERT(Pos != NULL);
		if (Pos == NULL)
		{
			return FALSE;
		}

		// Advance Pos (do a GetNext on it) to maintain the meaning of Pos.
		// While we are at it, check that the object we get is the one we have.
		VERIFY(object == (PMGPageObject*)List->GetNext(Pos));

		if (update_state != NULL)
		{
			// Don't use some other object's update state.
			ASSERT(update_state->object == object);
		}
	}

	// Update each object within the extent.

	BOOL saw_my_object = FALSE;

	// Initialize the dither matrix to use for this redisplay context.
	build_dither_matrix(rc);

	// Loop through each object we are suppose to refresh.
	// If we reach m_pUptoObject, then we finish. The m_pUptoObject
	// feature is used to build the background bitmap for
	// text editing (all objects under the text frame.)
	while ((object != rc->m_pUptoObject) && (object != NULL))
	{
		// Ignore hidden objects. For now, the only hidden objects
		// in PrintMaster are pictures and text in calendar dates which
		// are not in the current calendar (cuz the user changed the month.)
		if (!(object->get_flags() & OBJECT_FLAG_hidden))
		{
			PBOX bound;

			// Check if we are at a "special" object. This is used when doing special
			// refresh types for text frame objects. Once the saw_my_object flag is set,
			// it remains set for the next objects as well. This can be used to tell
			// if the current object is above or below the special object.
			if (object == pre->my_object)
			{
				saw_my_object = TRUE;
			}

			// If this is not the special object for this refresh extent, then we
			// can to get its refresh bounds and check if it intersects with the
			// refresh bounds. If not, we can ignore it. If we see the "special
			// object" for this refresh extent, we need to refresh it regardless
			// of its refresh bounds because there may be areas to refresh outside
			// of the current refresh bounds.
			if (object != pre->my_object)
			{
				object->get_refresh_bound(&bound, rc);
			}

			// rect = full refresh extent intersected with source pbox (client rectangle)
			// clip = RECT version of full refresh extent intersected with
			//        source pbox converted to screen coordinates
			// test_rect = full refresh extent intersected with source
			//             pbox (client rectangle) bumped out one pixel
			//             on each side
			// bound = full refresh bounds for non-special object (includes rotation)

			// Make sure the object is something that impacts the refresh extent.
			// This means that it is either a special "do-it-anyway" refresh where
			// the current object matches pre->my_object or a normal object whose
			// refresh bounds overlaps the refresh extent.

			if ((object == pre->my_object)
			 || (IsBoxOnBox(&test_rect, &bound)))
			{
				// Check if the object needs to be "calc"ed. If so, this means
				// its currently being moved or sized. In this case, we basically
				// ignore the object, but mark it with the OBJECT_FLAG_needs_update
				// flag so that when the user stops dragging the the object around,
				// it will be added as a refresh extent and the screen will be
				// properly updated. (See MODETEXT.CPP)
				//
				// PMGTODO: It is unclear if the my_object check does anything since
				// it is only used for text editing. It is possible that my_object
				// could be useful in the future for other objects?
				FLAGS flags = object->get_flags();
				if ((object != pre->my_object)
					 && (flags & OBJECT_FLAG_needs_calc)
					 && !CPmwDoc::IsDropRenderActive())
				{
					object->set_flags(flags | OBJECT_FLAG_needs_update);

					// If there is a current update state, it is invalid.
					if (update_state != NULL)
					{
						// We must free this update state.
						delete update_state;
						update_state = NULL;
					}

					// Ignore the object.
				}
				else
				{
					// We are refreshing the object, so can clear OBJECT_FLAG_needs_update.
					object->set_flags(flags & ~OBJECT_FLAG_needs_update);

					// IMPORTANT!! We must not have an abort check before this test because
					// if we do, the erase may not get done.

					// If we have an object other than the special object pointed to by
					// my_object, then turn the refresh type into REFRESH_ALL. This is
					// necessary since the special refresh types, REFRESH_ERASE and
					// REFRESH_REDRAW, require that no drawing is done beneath them.
					// If we have seen another object that impacts this refresh extent,
					// then this condition is no longer true and we need to switch to
					// REFRESH_ALL.

					if ((pre->refresh_type != REFRESH_ALL)
					 && (object != pre->my_object))
					{
						// Another object overlaps the special object extent, and the
						// refresh type indicates we are going to do some erasing
						// (REFRESH_ERASE or REFRESH_REDRAW). Switch to a full redraw.
						pre->refresh_type = REFRESH_ALL;

						if (!did_erase)
						{
							// Erase the background. This eventually calls the current desktop object
							// (or derived class thereof) to draw itself. This means the page color,
							// and page guided or shadows, etc.
							rc->erase_area(clip);
							did_erase = TRUE;
						}
					}

					// See if we should abort.
					if (rc->check_interrupt())
					{
						break;
					}

					FLAGS refresh_flags = object->get_refresh_flags();

#ifdef DEBUG_UPDATE
					TRACE("{%d {%d} (%lx, %lx)}\n",
 									object->type(),
 									pre->refresh_type,
 									object,
									update_state);
#endif

					BOOL making_image = FALSE;
					RECT local_clip;
					PBOX local_rect;
					BOOL clipped = FALSE;

					// Now update the object to the screen.

					// Normally, the refresh type is REFRESH_ALL in which case the object
					// update function needs to draw its foreground image. If the refresh
					// type is REFRESH_ERASE or REFRESH_REDRAW, then the update function
					// must also draw its background.

					// rect = full refresh extent intersected with source pbox (client rectangle)
					// clip = RECT version of full refresh extent intersected with
					//        source pbox converted to screen coordinates
					// test_rect = full refresh extent intersected with source
					//             pbox (client rectangle) bumped out one pixel
					//             on each side
					// bound = full refresh bounds for non-special object (includes rotation)

					local_clip = clip;
					
					local_rect = rect;

					PBOX world = rc->source_pbox;
					SHORT panel;

					// If drawing to Printer, try to do clipping.
					// NOTE: See CARDDOC.CPP for info on special panel numbers (-2,-1,0).
					//
					// PMGTODO: In theory, card panel -1 (full card view) is no longer used,
					// so the following code could be simplified.

					if ((pDoc != NULL)
					 && ((pDoc->get_project_type() == PROJECT_TYPE_Card) || (pDoc->get_project_type() == PROJECT_TYPE_NoteCard))
					 && ((pDoc->get_current_panel() == -1) || !rc->IsScreen())
					 && ((panel = object->get_panel()) != -1))
					{
						// Basically, this code takes the computed refresh extent (rect)
						// and clipping extent (clip) and clips then to the panel extent
						// if the project is a card and we're printing. The clipped flag
						// is set if the *entire* refresh extent is clipped away.
						//
						// PMGTODO: I think the clipping logic needs to be fixed, do you agree?
						// The clipped flag is set if the refresh extent is completely clipped
						// away. It should use a similar test to the one used above where it it
						// checks if the object refresh bounds is completely clipped away.
						// Given the current code, it would seem that the refresh extent could
						// be reduced to a non-empty rectangle which does not intersect the
						// refresh bounds of the object and we would go ahead and call update().
						// I'm not sure if this just wastes time or can actually draw something
						// that should be clipped. Anyway, this probably a relatively rare case.
						// The function CFrameObject::update() basically catches this case and
						// returns. Other update() functions may do similar things.

						pDoc->get_panel_world(&world, panel);

						if (IntersectBox(&local_rect, &world, &local_rect))
						{
							rc->pbox_to_screen(&world, TRUE);
							if (!rc->convert_pbox(&world, &local_clip, &clip))
							{
								// Nothing to see!
								clipped = TRUE;
							}
						}
						else
						{
							// Nothing to see!
							clipped = TRUE;
						}
					}

					// clipped == TRUE if entire object was clipped away
					if (!clipped)
					{
						// Handle a special case (yes... *** KLUDGE ALERT ***):
						//
						// If we are doing an opaque erase with the "special object" (the text
						// frame), then there must be an editing bitmap and we want to use it
						// to erase. This will only happen if CFrameObject::update() gets
						// a REFRESH_REDRAW. If we send a REFRESH_ALL, then the update function
						// will assume that DrawObjects is building the background.
						//
						// So,
						//				if we're doing an opaque erase
						//		 AND	the object being used to erase is a special text frame
						//		 AND	the refresh type is REFRESH_ALL
						//
						//		THEN	use REFRESH_REDRAW.
						//
						// PMGTODO: I have to think there is a better way to handle this. Maybe
						//	an object could have a virtual method indicating whether it wants
						// to do the erase or not. Anyway, the whole refresh optimization
						// strategy should be revisited at some point.

						if (opaque_erase
							 && pOpaqueObject->type() == OBJECT_TYPE_Frame
							 && ((CFrameObject*)pOpaqueObject)->GetEditBackground() != NULL
							 && (pre->refresh_type == REFRESH_ALL))
						{
							pre->refresh_type = REFRESH_REDRAW;
						}

						opaque_erase = FALSE;

						// rect = full refresh extent intersected with source pbox (client rectangle)
						// clip = RECT version of full refresh extent intersected with
						//        source pbox converted to screen coordinates
						// test_rect = full refresh extent intersected with source
						//             pbox (client rectangle) bumped out one pixel
						//             on each side
						// local_rect = rect (clipped to card panel if project is card and printing)
						// local_clip = clip (clipped to card panel if project is card and printing)
						// bound = full refresh bounds for non-special object (includes rotation) (not used)

						// Draw the object!
						update_state = object->update(
														rc,
														&local_rect,		
														&local_clip,
														update_state,
														pre->refresh_type);
					}

					// If the refresh type was REFRESH_ERASE or REFRESH_REDRAW, then we
					// have done an erase since that's part of the definition. If the
					// process was interrupted, then the did_erase state will be saved
					// in the update state.
					if (pre->refresh_type != REFRESH_ALL)
					{
						// We need to set this because the code that switches to
						// REFRESH_ALL for other objects checks it.
						did_erase = TRUE;

						// Warning: We're assuming that the object DID ERASE before
						// any interrupts were checked. If this is not the case, then
						// we may not get the area erased and have a bad refresh.
						// At the moment, there is no communication about erase status
						// between the object and the function which maintains the erase
						// state in the refresh extent object. For now, we just have to
						// have the rule that the object must do the erase, or maintain
						// a separate erase status in is update state.
					}

					// If the object's update routine returned a non-NULL pointer,
					// the object was interrupted while updating. We should just
					// remember this and leave the object loop for now.
					if (update_state != NULL)
					{
						if (update_state->type == UPDATE_TYPE_HitUptoObject)
						{
							// We hit the UptoObject in the rc.
							// Break out now.
							object = rc->m_pUptoObject;
							update_state = NULL;
							break;
						}
						if (update_state->type == UPDATE_TYPE_Interrupted)
						{
							// Somebody got interrupted. Leave the loop now.
							update_state->object = object;
							break;
						}
						else if (update_state->type == UPDATE_TYPE_Terminated)
						{
							update_state = NULL;
							break;
						}

						// What to do for errors? Just go to next object for now.
						update_state = NULL;
					}
				}
			}
		}

		// Move to next object.
		object = (Pos == NULL) ? NULL : (PMGPageObject*)List->GetNext(Pos);
	}

	// See if we finished. We only finished if we ran out of objects,
	// otherwise we were interrupted.

	result = FALSE;			// Assume all done.

	if (((pre->update_object = object) != rc->m_pUptoObject) && (object != NULL))
	{
		// The current object was not where we were suppose to end.
		// So, we need to remember where we were in the update object
		// so we can restart.

		// Set the update state in case it changed.
		pre->did_erase = did_erase;
		BOOL first = FALSE;
		PBOX new_extent = pre->extent;

		// Only do the following code if it's not a special refresh type
		// (i.e. one with an object).
		if (((pre->my_object == NULL)
				|| (saw_my_object && (object != pre->my_object)))
				&& !rc->is_print_preview)
		{
			// Run through each of the remaining objects and compute the union of
			// the intersections of their refresh bounds with the original refresh
			// extent. This will leave out any refresh areas required by objects that
			// were done before we were interrupted.
			//
			// This helps avoid extra drawing if another refresh extent gets added
			// which overlaps this one.

			// rect = full refresh extent intersected with source pbox (client rectangle)

			first = TRUE;

			while (object != NULL)
			{
				PBOX bound;

				object->get_refresh_bound(&bound, rc);

				if (IntersectBox(&bound, &bound, &rect))
				{
					if (first)
					{
						new_extent = bound;
						first = FALSE;
					}
					else
					{
						UnionBox(&new_extent, &new_extent, &bound);
					}
				}
				object = (Pos == NULL) ? NULL : (PMGPageObject*)List->GetNext(Pos);
			}
		}

		if (first)
		{
			// We didn't find anything!
			// So, we must be done, but how can this happen?
			if (update_state != NULL)
			{
				delete update_state;
				update_state = NULL;
			}
		}
		else
		{
			// We have an extent left to update.
			pre->extent = new_extent;
			result = TRUE;
		}
	}

	// Always set the update state result.
	pre->update_state = update_state;

	// And show the foreground of the desktop.
	if (rc->m_pDesktop != NULL)
	{
		((CDocEditDesktop*)(rc->m_pDesktop))->DrawForeground(&clip, rc);
	}

	//* Turn off any name we had.
	if (rc->want_update_names())
	{
		rc->update_name(NULL, 0);
	}

	return result;
}

/*
// Special draw code for PostScript (and other broken) printers.
// Return value:
//		 1 = terminated
//		 0 = completed successfully
//		-1 = error
*/

int DrawObjectsAsBitmaps(CPtrList* pList, REFRESH_EXTENT_PTR pre, RedisplayContext *rc, CPmwDoc* pDoc)
{
	ASSERT(pre->update_object == NULL);
	ASSERT(pre->update_state == NULL);

	// We want to break the list of objects up into "islands", non-overlapping
	// areas that can be rendered into seperate bitmaps.
	// We take advantage of the RefreshArray functionality to do this for us.

	CRefreshArray Refreshes;

	// Get the extent we want to draw. We will clip our refreshes to this.
	PBOX Extent = pre->extent;

	// Run through all the objects and add a refresh for each.
	POSITION Pos = pList->GetHeadPosition();
	while (Pos != NULL)
	{
		PMGPageObject* pObject = (PMGPageObject*)pList->GetNext(Pos);
		if (pObject->type() != OBJECT_TYPE_PageBackground)
		{
			REFRESH_EXTENT re;
			pObject->get_refresh_bound(&re.extent, rc);
			if (IntersectBox(&re.extent, &re.extent, &Extent)
				 && re.extent.x0 != re.extent.x1
				 && re.extent.y0 != re.extent.y1)
			{
				re.refresh_type = REFRESH_ALL;
				re.my_object = NULL;

				re.update_object = NULL;
				re.update_state = NULL;
				Refreshes.Add(&re, FALSE);				// Compact later
			}
		}
	}

	// Now, compact the refresh array.
	// This will merge all the overlapping extents into the "islands" we want.
	Refreshes.Compact();

	int nResult = 0;			// Assume success.

	HDC hDestinationDC = rc->destination_hdc;

	static int nScale = 1;

	// Get an rc to use.
	RedisplayContext BitmapRC = *rc;
	BitmapRC.is_printing_as_bitmap = FALSE;	// No recursion.
	if (BitmapRC.info.type != RASTER_TYPE_BILEVEL)
	{
		BitmapRC.info.type = RASTER_TYPE_24;
		BitmapRC.info.bpp = 24;
		BitmapRC.info.planes = 1;
	}

	if (Refreshes.NumberOfExtents() > 0)
	{
		// Create a compatible DC to use for our bitmaps.
		// We use the same DC for all bitmaps.
		HDC hBitmapDC = ::CreateCompatibleDC(hDestinationDC);
		if (hBitmapDC != NULL)
		{
			// Run through all the refreshes and draw them.
			for (int nRefresh = 0; nRefresh < Refreshes.NumberOfExtents(); nRefresh++)
			{
				// Get the next extent.
				REFRESH_EXTENT* re = Refreshes.GetExtent(nRefresh);
				// Transform it to destination space. This will determine our bitmap size.
				PBOX Bound = re->extent;
				rc->pbox_to_screen(&Bound, TRUE);
				CRect rDest;
				if (rc->convert_pbox(&Bound, &rDest, &rc->clip_rect))
				{
					TRACE("Bitmap island: %d, %d, %d, %d\n", rDest);

					// We now have the destination rect.
					// Compute the size of the bitmap, taking scaling into account.
					int nSourceWidth = rDest.Width()/nScale;
					int nSourceHeight = rDest.Height()/nScale;

					int nRowBytes;

					switch (BitmapRC.info.type)
					{
						case RASTER_TYPE_BILEVEL:
						{
							nRowBytes = (nSourceWidth+7)/8;
							break;
						}
						case RASTER_TYPE_24:
						case RASTER_TYPE_DIRECT:		// Assume the worst.
						{
							nRowBytes = nSourceWidth*3;
							break;
						}
						case RASTER_TYPE_8:
						case RASTER_TYPE_PALETTE:
						case RASTER_TYPE_256:
						default:
						{
							nRowBytes = nSourceWidth;
							break;
						}
					}
					nRowBytes = (nRowBytes+3) & ~3;
//					int nMaxMemory = 4*1024*1024;
					int nMaxMemory = 256*1024;
					int nStripHeight = nMaxMemory/nRowBytes;
					TRACE("nStripHeight: %d; nSourceHeight: %d\n",
							nStripHeight, nSourceHeight);

					// Fill out a DIB with the right stuff.
					struct
					{
						BITMAPINFO bmi;
						RGBQUAD colors[2];			// for monochrome case.
					} Header;

					if (nStripHeight > nSourceHeight)
					{
						nStripHeight = nSourceHeight;
					}
					InitBitmapInfoHeader((LPBITMAPINFOHEADER)&Header,
												nSourceWidth,
												nStripHeight,
												BitmapRC.info.bpp);
					if (BitmapRC.info.bpp == 1)
					{
						Header.colors[0].rgbBlue =
						Header.colors[0].rgbRed =
						Header.colors[0].rgbGreen = 255;
						Header.colors[0].rgbReserved = 0;

						Header.colors[1].rgbBlue =
						Header.colors[1].rgbRed =
						Header.colors[1].rgbGreen = 0;
						Header.colors[1].rgbReserved = 0;
					}

					CDibDC DibDC;
					if (DibDC.Create(&Header.bmi))
					{	
						// Loop on strips.
						int y0 = 0;
						while (y0 < nSourceHeight)
						{
							int y1 = y0 + nStripHeight;
							if (y1 > nSourceHeight)
							{
								y1 = nSourceHeight;
							}
							int nThisStrip = y1 - y0;
							TRACE("Strip output: %d to %d (%d)\n", y0, y1, nThisStrip);

							// Set the initial state to white.
							DibDC.PatBlt(0, 0, nSourceWidth, nThisStrip, WHITENESS);

							BitmapRC.destination_rect.left = rDest.left;
							BitmapRC.destination_rect.top = rDest.top + y0;
							BitmapRC.destination_rect.right = rDest.right;
							BitmapRC.destination_rect.bottom = rDest.top + y1;
							BitmapRC.destination_x0 = rc->destination_x0-rDest.left;
							BitmapRC.destination_y0 = rc->destination_y0-(rDest.top+y0);
							BitmapRC.destination_hdc = DibDC.GetSafeHdc();

							// Draw into the bitmap.
							DrawObjects(pList, re, &BitmapRC, pDoc);

							// Adjust the dib.
							// This is necessary to account for a bug in PSCRIPT.DRV
							// that prints the wrong part of the DIB if the entire
							// DIB is not being drawn.
							Header.bmi.bmiHeader.biHeight = nThisStrip;
							LPBYTE pData = DibDC.GetData()
										+ (nStripHeight-nThisStrip)*nRowBytes;
							// Draw the bitmap to the screen.
							StretchDIBits(hDestinationDC, 
											  rDest.left,
											  rDest.top+y0,
											  rDest.Width(),
											  nThisStrip,
											  0,
											  0,
											  nSourceWidth,
											  nThisStrip,
											  pData,
											  &Header.bmi,
											  DIB_RGB_COLORS,
											  SRCCOPY);
							y0 = y1;
						}
					}
				}
			}
			::DeleteDC(hBitmapDC);
		}
	}
	return nResult;
}

CFrameObject* CreatePageHeader(CPmwDoc* pDoc)
{
	if(pDoc == NULL)
	{
		ASSERT(0);
		return NULL;
	}

	ASSERT(pDoc);
	CString csHeader;
	CString csFileName = pDoc->GetTitle();

	PMGDatabase* pDatabase = pDoc->get_database();
	int nNumPages = pDoc->NumberOfPages();
	int nCurPage = pDoc->CurrentPageIndex() + 1;//zero based
	if(GET_PMWAPP()->OurGetProfileInt("Debug", "AddPageNumToHeader", FALSE))
		csHeader.Format("%s\nPage %d of %d", csFileName, nCurPage, nNumPages);
	else
		csHeader = csFileName;

	CTextStyle textStyle(pDatabase);
	textStyle.SetDefault();
	//set the spacing
	int nFontSize = GET_PMWAPP()->OurGetProfileInt("Debug", "HeaderFontSize", 4);

	textStyle.Size(MakeFixed(nFontSize));
	textStyle.BaseSize(MakeFixed(nFontSize));
	textStyle.UpdateFontMetrics();
	textStyle.Alignment(ALIGN_right);
	CTxp Txp(pDatabase);

	FRAMEOBJ_CREATE_STRUCT fcs;
	if(!GET_PMWAPP()->OurGetProfileInt("Debug", "HeaderTransparent", FALSE))
		memset(&fcs, 0, sizeof(fcs));

	fcs.vert_alignment = ALIGN_top;
	fcs.alignment = ALIGN_right;
	
	CFrameObject* pFrame;
	PPNT dims = pDoc->get_dimensions();
	double dRotation = 0;

	PBOX pbBounds = {	dims.x - 1800, 0, dims.x, 800 };
	if( pDoc->get_orientation() == LANDSCAPE)
	{
		pbBounds.x0 = dims.x - 1300;
		pbBounds.x1 = dims.x + 500;
		pbBounds.y0 = dims.y - 1300;
		pbBounds.y1 = dims.y - 500;
		dRotation = 3 * 3.14159 / 2;
	}

	if(pDoc->get_project_type() == PROJECT_TYPE_Sticker)
	{
		//for some reason the project dims are off and the header does not show up 
		//on the page so we need to shift it over
		pbBounds.x0 -= 1500;
		pbBounds.x1 -= 1500;
		pbBounds.y0 += 500;
		pbBounds.y1 += 500;
	} 
	fcs.bound = pbBounds;
	if ((pFrame = pDatabase->create_frame_object(&fcs)) != NULL)
	{
		pFrame->set_panel(-1);
		Txp.Init(pFrame, 0);
		Txp.Style(textStyle);

		CTextRecord* pRecord = Txp.LockText();
		pRecord->ChangeParagraphStyle(0, 0, &textStyle);
		pRecord->release();
		Txp.InsertString(csHeader);
		pFrame->set_rotation(dRotation);
		pFrame->calc();	
	}
	return pFrame;
}

BOOL DrawObjects(ObjectListPtr page_objects, REFRESH_EXTENT_PTR pre, RedisplayContext *rc, CPmwDoc* pDoc)
{
	// Build a CPtrList to use for drawing.
	CPtrList List;

   if (pDoc != NULL)
   {
		DocumentRecord* pDocRec = pDoc->DocumentRecord();
		ASSERT(pDocRec != NULL);

		pDoc->AddBackgroundForDrawing(List);

		// Add any master page items.
		ObjectList* pMasterItems = pDoc->GetMasterItems();
		if (pMasterItems != NULL)
		{
			for (PMGPageObject* pObject = (PMGPageObject*)pMasterItems->first_object();
				  pObject != NULL;
				  pObject = (PMGPageObject*)pObject->next_object())
			{
				List.AddTail(pObject);
			}
		}
	}

	// Add all the normal objects.
	for (PMGPageObject* pObject = (PMGPageObject*)page_objects->first_object();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		List.AddTail(pObject);
	}

	if(rc->is_printing && GET_PMWAPP()->m_bPrintProjects)
	{
		if(pDoc != NULL)
		{
			CFrameObject* pFrame = CreatePageHeader(pDoc);
			if(pFrame != NULL)
				List.AddTail(pFrame);
		}
	}

	// And invoke the main DrawObjects function.
	return DrawObjects(&List, pre, rc, pDoc);
}

void
CPmwDoc::AddBackgroundForDrawing(CPtrList &List)
{
	if (m_pBackgroundObject->HasBackground())
	{
		List.AddTail(m_pBackgroundObject);
	}
}

// Draw objects in a document into an RC.
//
//		pre				pointer to refresh extent
//		rc					pointer to rc
//		upto_object		object at which to stop drawing -- this is used in MODETEXT.CPP which
//								creates the edit background
BOOL CPmwDoc::draw_objects(REFRESH_EXTENT_PTR pre, RedisplayContext *rc)
{
	return DrawObjects(page_objects, pre, rc, this);
}
/*
// Add a refresh extent for the document.
*/
VOID CPmwDoc::add_refresh(PBOX_PTR extent, REFRESH_TYPE type, PMGPageObjectPtr object, LPRECT lprExtraPixels)
{
	if (m_fAddRefreshes)
	{
		REFRESH_STRUCT rs;

		rs.pextent = extent;
		rs.type = type;
		rs.object = object;
		rs.lprExtraPixels = lprExtraPixels;

		UpdateAllViews(NULL, HINT_Refresh, (CObject *)&rs);
	}
}
/*
// Update all views.
// Each view should reset its view.
*/
VOID CPmwDoc::reset_views(VOID)
{
	UpdateAllViews(NULL, HINT_Reset, NULL);
}

void CPmwDoc::ReframeOnTxp(void)
{
	UpdateAllViews(NULL, HINT_ReframeOnTxp, NULL);
}

/*
// Handle refreshing of this document during the idle time.
*/

BOOL CPmwDoc::idle_refresh(VOID)
{
	if (refreshes_suspended)
	{
	/* Shouldn't do anything now. */
		return FALSE;
	}

	// Draw the preview view(s) first.
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CView *pView = GetNextView(pos);

		if (pView->IsKindOf(RUNTIME_CLASS(CPmwPreviewView)))
      {
         if (((CPmwPreviewView*)pView)->Refresh())
		   {
			   return TRUE;
		   }
		}
	}

	// Now draw all normal view(s).
	pos = GetFirstViewPosition();
	while (pos)
	{
		CView *pView = GetNextView(pos);

      if (pView->IsKindOf(RUNTIME_CLASS(CPmwView)))
      {
         if (((CPmwView*)pView)->refresh())
		   {
			   return TRUE;
		   }
      }
	}
	return FALSE;
}

/*
// Get the world of a panel with a document of set dimensions.
*/

VOID CPmwDoc::get_panel_world_dims(PBOX_PTR world, PCOORD xsize, PCOORD ysize, SHORT panel)
{
	if (panel == -1)
	{
		panel = get_current_panel();
	}

	CPmwApp* pApp = GET_PMWAPP();

	PBOX Margins = m_pPaperInfo->GetPageMargins();
	RECT rMargins;
	rMargins.left = (int)Margins.x0;
	rMargins.top = (int)Margins.y0;
	rMargins.right = (int)Margins.x1;
	rMargins.bottom = (int)Margins.y1;

	info.get_panel_world(world, xsize, ysize, panel, &rMargins);
}

/*
// Get the dimensions of a panel for the current document.
*/

VOID CPmwDoc::get_panel_world(PBOX_PTR world, SHORT this_panel)
{
	PPNT dims = get_dimensions();

	get_panel_world_dims(world, dims.x, dims.y, this_panel);
}

/*
// The refresh notify that is registered with the database.
*/

VOID CPmwDoc::refresh_notify(
	VOIDPTR data,
	PBOX_PTR area,
	REFRESH_TYPE type,
	PMGPageObjectPtr object,
	LPRECT lprExtraPixels)
{
	// REFRESH_ERASE doesn't work because the frame now draws an outline
	// around itself.
	if (type == REFRESH_ERASE)
	{
//		type = REFRESH_REDRAW;
	}
	((CPmwDoc *)data)->add_refresh(area, type, object, lprExtraPixels);
}

/*
// The document modified handler that is registered with the database.
*/

void CPmwDoc::document_modified_handler(VOIDPTR data)
{
	((CPmwDoc *)data)->SetModifiedFlag();
}

/*
// Get the main frame for this project.
// Default has no main frame.
*/

CFrameObject* CPmwDoc::get_main_frame(VOID)
{
	return NULL;
}

/*
// Resize the project after a frame resizes itself.
// Default does nothing.
*/

BOOL CPmwDoc::resize_project(CFrameObject* pFrame)
{
	CPmwView* pView = GetCurrentView();
	PMGPageObject* pObject;
	if (pView != NULL
			&& pView->EditingText()
			&& (pObject = pView->GetTxp()->Object()) != NULL
			&& pObject->get_panel() == OBJECT_IN_CELL_PANEL)
	{
		// We want to update the table.
		PMGPageObject* pContainer;
		if ((pContainer = pObject->get_container()) != NULL
			 && pContainer->type() == OBJECT_TYPE_Cell
			 && (pContainer = pContainer->get_container()) != NULL
			 && pContainer->type() == OBJECT_TYPE_Table)
		{
			((CTableObject*)pContainer)->FitToText();
		}
	}

	return FALSE;
}

/*
// Choose a font (actually a style).
*/

int CPmwDoc::ChooseFont(CTextStyle& Style)
{
	int ret;

	BOOL bShowActualSize = AfxGetApp()->GetProfileInt(SECTION_Configuration, ENTRY_ShowActualFontSize, FALSE);

	CFontPropertySheet Dialog(this, &Style, bShowActualSize, IDS_FONT_FORMAT, NULL, 0);

	if ((ret = Dialog.DoModal()) == IDOK)
	{
		AfxGetApp()->WriteProfileInt(SECTION_Configuration, ENTRY_ShowActualFontSize, Dialog.GetShowActualSize());

		if (!Dialog.GetStyleChanges(this, &Style))
		{
			// Nothing changed.
			ret = IDCANCEL;
		}
	}

	return ret;
}

void CPmwDoc::OnFileSave()
{
	if (access_file(m_strPathName, 6) != 0)
	{
		// we do not have read-write access or the file does not (now) exist
		DoSave(NULL);
	}
	else
	{
		DoSave(m_strPathName);
	}
}

void CPmwDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsModified());
}

void CPmwDoc::OnRevertToSavedFile()
{
	CString strFileName = m_strPathName;

	if(IsModified() && GetSaved())
	{

		if(AfxMessageBox(IDS_FILE_REVERTTOSAVED, MB_OKCANCEL) == IDOK)
		{
			GET_PMWAPP()->SetPreventHub();
			OnCloseDocument();
			GET_PMWAPP()->RevertToSavedFile(strFileName);
		}

	}
}

void CPmwDoc::OnUpdateRevertToSavedFile(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsModified() && GetSaved());
}

void CPmwDoc::OnFileSaveAs()
{
	DoSave(NULL);
}

/*
// This is the main entry point for the Save/Save As code.
// This contains the logic for invoking the dialog (when necessary) as well
// as doing the actual save (via SaveToFile).
*/

BOOL CPmwDoc::DoSave(const char* pszPathName, BOOL bReplace /*=TRUE*/)
	// Save the document data to a file
	// pszPathName = path name where to save document file
	// if pszPathName is NULL then the user will be prompted (SaveAs)
	// note: pszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
	// Much of this code came from the MFC (DOCCORE.CPP).
{
	// See if the current configuration supports saving.
	// If not, throw up the "unsupported" dialog.
#ifdef FIXTEMPLATES
	if (!m_fFixTemplates)
#endif
	{
		if (!GetConfiguration()->SupportsSaveProject())
		{
			// What to return here? Is this failure or not? I think it is.
			return FALSE;
		}
	}

	// Lookup the project.
	// If we have an original browser path, use it.
	// Otherwise, try the one we were loaded with.

	CString csLoadPath;
	if (m_csBrowserPath.IsEmpty())
	{
		csLoadPath = m_strPathName;
	}
	else
	{
		csLoadPath = m_csBrowserPath;
	}

	// See if we are doing, or need to do, a SaveAs.
	//
	// This is the case if:
	// (1) This is not a normal file (may be a template or old file).
	// (2) We have no file name yet.

	if ((file_type != FILE_TYPE && !m_fFixTemplates)
		 || pszPathName == NULL
		 || pszPathName[0] == '\0')
	{
		// Do a SaveAs...

		CPmwApp *pApp = GET_PMWAPP();

		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);
		BOOL fSetDescription = FALSE;

		CString csName;
		CString csDir;
		if (file_type != FILE_TYPE || m_strPathName[0] == '\0')
		{
			csDir = GetPathManager()->ExpandPath("[[D]]");
		}
		else
		{
			CString csPath = GetPathManager()->ExpandPath(m_strPathName);
			Util::SplitPath(csPath, &csDir, &csName);
		}

		int nExtension = csName.Find('.');
		if (nExtension > -1)
		{
			csName = csName.Left(nExtension);
		}

		CString csExt;
		GetDocTemplate()->GetDocString(csExt, CDocTemplate::filterExt);

		// build the filter string
		CString csFilter;
		GetDocTemplate()->GetDocString(csFilter, CDocTemplate::filterName);
		csFilter += "|*" + csExt + "||";

		// create the "save project" dialog
		CSaveProjectDialog saveDlg(csExt, csName, csFilter, csDir, this);

		CPMWCollection* pCollection;
		LPCSTR pPath = csLoadPath;
		CString csSearchName;

		CString csPathName;
		CString csPathCollection;
		if (GetPathManager()->BindPath(pPath, csPathName, &csPathCollection) == PBT_CollectionItem)
		{
			// Extract the collection name.
			pCollection = GetCollectionManager()->FindCollection(csPathCollection);
			csSearchName = csPathName;
		}
		else
		{
			// Use the user collection.
			pCollection = GetCollectionManager()->GetUserCollection(CPMWCollection::typeProjects);
			csSearchName = GetPathManager()->MakeCompositePath(csLoadPath);
		}

		if (saveDlg.DoModal() != IDOK)
		{
			return FALSE;
		}
		else
		{
			// Get the existing item's description and keywords (if any)
			CString csDescription, csKeywords;
			if (pCollection != NULL)
			{
				DWORD dwItemNumber = 0;
				pCollection->GetItemInfo(csSearchName, dwItemNumber,
												 csDescription, csKeywords);
			}
			
			csDescription.Empty();
			// do the save
			return SaveToFile(saveDlg.GetPathName(), bReplace,
									saveDlg.m_fEmbedPictures, saveDlg.m_fAddToGallery,
									csDescription, csKeywords, saveDlg.m_csCategory);
		}
	}
	else
	{
		// normal Save
		BOOL fSaveInGallery = FALSE;
		CPMWCollection* pCollection = GetCollectionManager()->GetUserCollection(CPMWCollection::typeProjects);
		CString csName = GetPathManager()->MakeCompositePath(csLoadPath);
		if (!csName.IsEmpty())
		{
			// See if the item exists. If so, we want to update it.
			if (pCollection != NULL)
			{
				// First try the dynamic name, which is how items are now stored in the DB.
				// If that fails, try the composite name, which is how they used to be stored.
				CompositeFileName cfn(csName);
				CString strDynamicName = cfn.get_dynamic_name(TRUE);
				DWORD dwItemNumber = 0;
				if (pCollection->FindItem(strDynamicName, &dwItemNumber) == ERRORCODE_None)
					fSaveInGallery = TRUE;
				else if (pCollection->FindItem(csName, &dwItemNumber) == ERRORCODE_None)
					fSaveInGallery = TRUE;
			}
		}

		return SaveToFile(pszPathName, bReplace, FALSE, fSaveInGallery);
	}
}

BOOL CPmwDoc::SaveToFile(LPCSTR pNewName,
								 BOOL fReplace,
								 BOOL fEmbedPictures /*=FALSE*/,
								 BOOL fSaveInGallery /*=FALSE*/,
								 LPCSTR pszDescription /*=NULL*/,
								 LPCSTR pszKeywords /*=NULL*/,
								 LPCSTR pszCategory /*=NULL*/)
{
   // No longer a With Help document. And add to the MRU from now on.
   m_wDocumentFlags &= ~(FLAG_WithHelpDocument|FLAG_NoMRU|FLAG_BringUpReceiverList);
/*
// If we want to set the description, do it now.
*/

#ifdef FIXTEMPLATES
	if (!m_fFixTemplates)
#endif
	{
		// Set the description in the preview for backward compatability.
		ProjectPreviewPtr pPreview;

		if (pszDescription != NULL)
		{
			if ((pPreview = (ProjectPreviewPtr)get_database()->get_record(PMG_PREVIEW_RECORD, NULL, RECORD_TYPE_ProjectPreview)) != NULL)
			{
				pPreview->set_description((LPSTR)pszDescription);
				pPreview->release();
			}
		}

		// If we want to embed all the pictures, do it now.
		if (fEmbedPictures)
		{
			embed_pictures();
		}
	}

	// Build a temporary name for safe saving purposes.
	CString csTempFile;

#ifdef FIXTEMPLATES
	if (m_fFixTemplates)
	{
		csTempFile = pNewName;
	}
	else
#endif
	{
		CString csPath, csFilename;

		Util::SplitPath(pNewName, &csPath, &csFilename);

		csFilename = csFilename.SpanExcluding(".");
		csFilename += ".$$$";

		Util::ConstructPath(csTempFile, csPath, csFilename);
	}

/*
// Save Sequence:
// (1) Save document to file with ".$$$" extension.
// (2) If success, remove the old name and rename saved file to correct name.
//		 Else, remove the partially saved file
*/

	if (!OnSaveDocument(csTempFile))
	{
		// be sure to delete the file

		remove(csTempFile);
		return FALSE;
	}

/* Delete our temp (old) file since we saved correctly. */

#ifdef FIXTEMPLATES
	if (!m_fFixTemplates)
#endif
	{
		if (remove(pNewName) == EACCES
			 || rename(csTempFile, pNewName))
		{
			remove(csTempFile);

			char buffer[120];

			sprintf(buffer,
						// JRO "Unable to access %s for saving.",
                  GET_PMWAPP()->GetResourceStringPointer (IDS_NO_ACCESS_SAVE),
						pNewName);

			AfxMessageBox(buffer, MB_OK | MB_ICONEXCLAMATION);
			SetModifiedFlag();
			return FALSE;
		}
	}

/* Now, it's a file. */

	file_type = FILE_TYPE;

	if (fReplace)
	{
		// Reset the title and change the document name
		SetPathName(pNewName);
		ASSERT(stricmp(m_strPathName, pNewName) == 0);       // must be set
	}

#ifdef FIXTEMPLATES
	if (!m_fFixTemplates)
#endif
	{
      if (fSaveInGallery)
		{
			TRACE("Begin save to album...\n");

			BOOL fSuccess = FALSE;

			BeginWaitCursor();

			CPMWCollection* pCollection = GetCollectionManager()->GetUserCollection(CPMWCollection::typeProjects);
			if (pCollection != NULL)
			{
				int nResult = pCollection->ImportItem(pNewName, pszDescription,
																  pszKeywords, pszCategory,
																  (pszDescription == NULL));

				if (nResult != ERRORCODE_None)
				{
					TRACE("Got error %d adding/updating item.\n", nResult);
				}
				else
				{
					fSuccess = TRUE;
				}
			}
			TRACE("End save to album...\n");

			EndWaitCursor();

			if (!fSuccess)
			{
				Util::MessageBox(MB_OK | MB_ICONEXCLAMATION, (unsigned int)-1,
									  // JRO "Your project has been saved to disk.\n"
									  // "However, it could not be added to the Project Album."
                             GET_PMWAPP()->GetResourceStringPointer (IDS_SAVE_NO_ALBUM));
			}
		}
	}
	/* We're now in the user browser. */
	m_csBrowserPath.Empty();

	SetSaved (TRUE);

	return TRUE;        // success
}

BOOL CPmwDoc::DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc)
{
	return draw_objects(re, rc);
}

void CPmwDoc::GetPreviewBound(PBOX* pBound)
{
	PPNT dims = get_dimensions();
	pBound->x0 =
		pBound->y0 = 0;
	pBound->x1 = dims.x;
	pBound->y1 = dims.y;
}

void CPmwDoc::GetPreviewInfo(const CRect &crDest, BOOL bDrawPreview, CSize &sizeNew, PBOX &pboxBound)
{
	if (bDrawPreview)
		GetPreviewBound(&pboxBound);   // Called for thumbnails, draw front
	else
		GetPanelBoundForPreview(&pboxBound);   // draw current panel

	// Figure out how big the preview is going to be.
	PPNT Dims;
	Dims.x = pboxBound.x1 - pboxBound.x0;
	Dims.y = pboxBound.y1 - pboxBound.y0;

	CPoint cpMaxDims(crDest.Width(), crDest.Height());

	if ((sizeNew.cy = scale_number(Dims.y, (sizeNew.cx = cpMaxDims.x), Dims.x)) > cpMaxDims.y)
	{
		sizeNew.cx = scale_number(Dims.x, (sizeNew.cy = cpMaxDims.y), Dims.y);
	}

	if (sizeNew.cx == 0) sizeNew.cx++;
	if (sizeNew.cy == 0) sizeNew.cy++;
}

void CPmwDoc::size_guides(ObjectListPtr list, PBOX oldbound, PBOX newbound)
{
   // scale guides

   PCOORD x0 = newbound.x0;
   PCOORD y0 = newbound.y0;
   PCOORD x1 = newbound.x1;
   PCOORD y1 = newbound.y1;

   PCOORD dx = x1 - x0;
   PCOORD dy = y1 - y0;

   PCOORD old_dx = oldbound.x1 - oldbound.x0;
   PCOORD old_dy = oldbound.y1 - oldbound.y0;
   
   if (old_dy != dy)
   {
      Array* pGuides = list->GetHorizGuides();
      for (int i = 0; i < pGuides->count(); i++)
      {
         DWORD* pOldy = (DWORD*)pGuides->get_element(i);
         DWORD newy = scale_pcoord(*pOldy - oldbound.y0, dy, old_dy) + y0;
         *pOldy = newy;
      }
   }

   if (old_dx != dx)
   {
      Array* pGuides = list->GetVertGuides();
      for (int i = 0; i < pGuides->count(); i++)
      {
         DWORD* pOldx = (DWORD*)pGuides->get_element(i);
         DWORD newx = scale_pcoord(*pOldx - oldbound.x0, dx, old_dx) + x0;
         *pOldx = newx;
      }
   }
}

void CPmwDoc::size_list(ObjectListPtr list, PCOORD old_xsize, PCOORD old_ysize, PCOORD new_xsize, PCOORD new_ysize)
{
	PPNT old_center = { 0, 0}, new_center = { 0, 0};
	
	PCOORD old_x = old_xsize, old_y = old_ysize, new_x = new_xsize, new_y = new_ysize;
	PMGPageObjectPtr object;

   BOOL fIsCard = (document->get_project_type() == PROJECT_TYPE_Card ||
                   document->get_project_type() == PROJECT_TYPE_NoteCard ||
                   document->get_project_type() == PROJECT_TYPE_HalfCard);
   
	PBOX bound, oldbound;

   for (object = (PMGPageObjectPtr)list->last_object();
						object != NULL;
						object = (PMGPageObjectPtr)object->previous_object())
	{
		ANGLE rot = object->get_rotation();

		if (fIsCard)
		{
    		// The object fits its panel.

         oldbound = object->get_bound();
         get_panel_world(&bound, object->get_panel());

         // scale guides
         ObjectListPtr panel_list = ((GroupObject*)object)->object_list();
         size_guides(panel_list, oldbound, bound);
      }
		else
		{
			oldbound = bound = ((RectPageObjectPtr)object)->get_unrotated_bound();

			if (rot != 0.0)
			{
				PPNT Origin;

				PBOX Foo;
				PPNT Verts[4];

				/* Recalc the original origin. */
				Origin.x = (bound.x0 + bound.x1) >> 1;
				Origin.y = (bound.y0 + bound.y1) >> 1;

				compute_rotated_bound(bound, rot, &Foo, Origin, Verts);

				extern BOOL adjust_vertices(PPNT* Verts, PBOX* oldrect, PBOX* newrect);		// database\pmgobj.cpp
				extern void find_unrot_rect(PPNT* Verts, ANGLE& rot, PBOX* new_box);			// database\pmgobj.cpp

				PBOX OldRect;
				OldRect.x0 = 0;
				OldRect.y0 = 0;
				OldRect.x1 = old_x;
				OldRect.y1 = old_y;

				PBOX NewRect;
				NewRect.x0 = 0;
				NewRect.y0 = 0;
				NewRect.x1 = new_x;
				NewRect.y1 = new_y;

				if (adjust_vertices(Verts, &OldRect, &NewRect))
				{
					/* Now we find the rectangle inside the distorted child object, &	*/
					/* unrotate it. This will become the new child object 'org_bound'.*/
					find_unrot_rect(Verts, rot, &bound);
            }
         }
			else
			{
				if (old_xsize != new_xsize)
				{
					bound.x0 = scale_pcoord(bound.x0-old_center.x, new_x, old_x) + new_center.x;
					bound.x1 = scale_pcoord(bound.x1-old_center.x, new_x, old_x) + new_center.x;
				}

				if (old_ysize != new_ysize)
				{
					bound.y0 = scale_pcoord(bound.y0-old_center.y, new_y, old_y) + new_center.y;
					bound.y1 = scale_pcoord(bound.y1-old_center.y, new_y, old_y) + new_center.y;
				}
         }
      }

		((RectPageObjectPtr)object)->set_unrotated_bound(bound);
		((RectPageObjectPtr)object)->set_rotation(rot);

		object->calc();
	}

   if (!fIsCard)
      size_guides(list, oldbound, bound);
}

/*
// Make sure the project fits on the paper size selected.
*/

VOID CPmwDoc::size_to_paper(VOID)
{
	// Get the app's global (printer defined) paper info.
	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

	// Set our internal paper to this.
	*m_pPaperInfo = *(pApp->GetPaperInfo(get_orientation()));

	// And size to it.
	SizeToInternalPaper();
}

/*
// Resize the document to fit the paper set within us.
*/

void CPmwDoc::SizeToInternalPaper(void)
{
	PPNT new_dims = m_pPaperInfo->GetProjectDims();

/* Get the paper size. */

	if (get_orientation() == LANDSCAPE)
	{
		PCOORD t = new_dims.x;
		new_dims.x = new_dims.y;
		new_dims.y = t;
	}

/* Get the document size. */

	PPNT dims = get_dimensions();

/* Banners are assumed to be OK in the horizontal direction. */

	if (get_project_type() == PROJECT_TYPE_Banner)
	{
	/* Ignore width. */
		new_dims.x = dims.x;
	/* Ignore height. */
		new_dims.y = dims.y;
	}

/* See if we need to adjust. */

	if (dims.x != new_dims.x || dims.y != new_dims.y)
	{
		document->set_dimensions(new_dims);
		document->set_paper_width(0);
		document->set_paper_height(0);

		SizeAllPages(dims, new_dims);

	/* We need to reshow the current view. */

		set_portion();
		reset_views();
	}
}

/*
// Size all the pages in this document.
*/

void CPmwDoc::SizeAllPages(PPNT OldSize, PPNT NewSize)
{
	// Save the current page record. We will restore it later.
	DB_RECORD_NUMBER lCurrentPage = CurrentPageRecord();

	// Run through all the pages and resize them.
	DWORD dwPages = NumberOfPages();
	for (DWORD dwPage = 0; dwPage < dwPages; dwPage++)
	{
		// Get the record number of the next page.
		DB_RECORD_NUMBER lPage = GetPage(dwPage);
		if (lPage != 0)
		{
			// Load the page if not already current.
			if (lPage != CurrentPageRecord())
			{
				LoadPage(lPage, TRUE, FALSE);
			}

			// Size the page.
			SizeCurrentPage(OldSize, NewSize);
		}
	}

	// Reload the original current page if not now current.
	if (lCurrentPage != CurrentPageRecord())
	{
		LoadPage(lCurrentPage, TRUE, FALSE);
	}
}

/*
// Size the current page.
*/

void CPmwDoc::SizeCurrentPage(PPNT OldSize, PPNT NewSize)
{
	// load_page(-1) marks the list as modified as well.
	size_list(document->load_page(-1),
				 OldSize.x, OldSize.y,
				 NewSize.x, NewSize.y);

	// Update any frames that have intersections.
	MarkFramesBelow(NULL);
	UpdateMarkedFrames(TRUE);
}

/*
// Reset the fonts in this document after a font list change.
*/

VOID CPmwDoc::reset_fonts()
{
	if (document != NULL)
	{
		document->initialize_font_records(FALSE);
	}
}

VOID CPmwDoc::flush(VOID)
{
/* For now, just flush (i.e. close) the device. */
	mapping.device->flush();
}

BOOL CPmwDoc::AddPage(DWORD dwAtPage)
{
	DB_RECORD_NUMBER lPage;
	return document->new_page(&lPage, dwAtPage) == ERRORCODE_None;
}

void CPmwDoc::GotoPage(DWORD dwPage, BOOL fUpdateViews /*=TRUE*/)
{
	LoadPage(GetPage(dwPage), TRUE, fUpdateViews);
}

void CPmwDoc::LoadPage(DB_RECORD_NUMBER recPage /*=0*/, BOOL fUpdateMenu /*=TRUE*/, BOOL fUpdateViews /*=TRUE*/)
{
	if (recPage != -1)
	{
		deselect_all(fUpdateMenu);
	}

	page_objects = document->load_page(recPage);

	if (recPage != -1)
	{
		if (page_objects == NULL)
		{
			// We could not load	the page.
			// 
			OnCloseDocument();
		}

	/* Not just a simple mod/access. Go through and link up the OLE objects. */
		PostProcessObjects(page_objects);
		if (fUpdateViews)
		{
			UpdateAllViews(NULL);
		}

      // Update Background object with Page Properties
       UpdatePageProperties();

		if (EditingMasterPage())
		{
			// Reflow the page macros on this page.
			ReflowPageMacros(object_list());
		}
		else
		{
			// Update the master page items (i.e. reflow them).
			UpdateMasterPageItems();
		}
		if (EditingInstructionPage())
		{
			// Reflow the page macros on this page.
			ReflowPageMacros(object_list());
		}
		else
		{
			// Update the Instruction page items (i.e. reflow them).
			UpdateInstructionPageItems();
		}
	}
}

CPageProperties * 
CPmwDoc::GetPageProperties() 
{ 
	return page_objects->GetPageProperties(); 
}

void CPmwDoc::UpdatePageProperties()
{
   CPageProperties *pPageProp = GetPageProperties();
//   ASSERT(pPageProp);
	if (pPageProp)
		UpdatePageProperties(pPageProp);
}

void CPmwDoc::UpdatePageProperties(CPageProperties *pPageProp)
{
   ASSERT(pPageProp);
   if(pPageProp)
   {
      CBackgroundObject *pBackgroundObject = GetBackgroundObject();
      // Update background object with Page Properties
      if(pBackgroundObject)
      {
			PBOX  pboxObjectDims;
			// If we have panels, use the current panel size
			if (number_of_panels() > 1)
			{
				get_panel_world (&pboxObjectDims);
			}
			else
			{
				PPNT  ppntDocDims = get_dimensions();

				pboxObjectDims.x0 = 0;
				pboxObjectDims.y0 = 0;
				pboxObjectDims.x1 = ppntDocDims.x;
				pboxObjectDims.y1 = ppntDocDims.y;
			}
			pBackgroundObject->set_bound(pboxObjectDims);
			
			BOOL NewBackground = pBackgroundObject->GetTextureRecNum() != pPageProp->GetTextureRecNum();
			pBackgroundObject->SetTextureRecNum(pPageProp->GetTextureRecNum());

			NewBackground = (NewBackground == TRUE) || pBackgroundObject->GetBackgroundColor() != pPageProp->GetBackgroundColor();
			pBackgroundObject->SetBackgroundColor(pPageProp->GetBackgroundColor());
			// If the page background has changed, we need to change the view's color 
			// for a text box in edit mode, if it exists
			if (NewBackground)
			{
				CPmwView	*pv;
				if ((pv = GetCurrentView()) != NULL) 
					pv->free_edit_background();
			}
      }
   }
}

void CPmwDoc::UpdateMasterPageItems(void)
{
	ReflowPageMacros(GetMasterItems());
}

void CPmwDoc::UpdateInstructionPageItems(void)
{
	ReflowPageMacros(GetInstructionItems());
}

void CPmwDoc::ReflowPageMacros(ObjectList* pList)
{
	if (pList != NULL)
	{
		// Create an array with just a page macro character in it.
		// This is what triggers the reflow.
		CWordArray cwaMacros;
		cwaMacros.Add(MACRO_CHARACTER_Page);

		// Run through all the objects.
		for (PMGPageObject* pObject = (PMGPageObject*)pList->first_object();
			  pObject != NULL;
			  pObject = (PMGPageObject*)pObject->next_object())
		{
			switch (pObject->type())
			{
				case OBJECT_TYPE_Frame:
				{
					((CFrameObject*)pObject)->ReflowMacros(&cwaMacros);
					break;
				}
				case OBJECT_TYPE_Group:
				{
					ReflowPageMacros(((GroupObject*)pObject)->object_list());
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}
}

CBackgroundObject* CPmwDoc::GetBackgroundObject(void)
{
	return m_pBackgroundObject;
}

/*
// Run through an object list and attach all OLE objects to their
// COleClientItems.
*/

void CPmwDoc::PostProcessObjects(ObjectList* pList)
{
	PMGPageObject* pObject;

	for (pObject = (PMGPageObject*)pList->first_object();
						pObject != NULL;
						pObject = (PMGPageObject*)pObject->next_object())
	{
		switch (pObject->type())
		{
			case OBJECT_TYPE_Graphic:
			case OBJECT_TYPE_Ellipse:
			case OBJECT_TYPE_Rectangle:
			case OBJECT_TYPE_DrawingObject:
			{
				if (m_wGlobalDocumentFlags != CPmwDoc::FLAG_AlbumDocument && FALSE)
				{
					PMGPageObjectPtr pNewObject = ComponentObject::CreateFromObject( pObject );

					if (pNewObject)
					{
						pList->insert_after( pNewObject, pObject );

						if (pList != page_objects)
						{
							/* Remove object from the list. */
							pList->detach( pObject );
						}

						delete_object( pObject );
						pObject = pNewObject;
					}
				}

				break;
			}
			case OBJECT_TYPE_Group:
			case OBJECT_TYPE_Calendar:
			{
				PostProcessObjects(((GroupObject*)pObject)->object_list());
				break;
			}
			case OBJECT_TYPE_OleObject:
			case OBJECT_TYPE_Serif:
			{
			/*
			// This is the real crux of the matter.
			// Fixup this OLE object.
			*/
				OleObject* pOleObject =(OleObject*)pObject;
				DWORD dwItemNumber = pOleObject->ItemNumber();
			/* Find the object with this number. */
				POSITION pos = GetStartPosition();

				while (pos != NULL)
				{
					CPmwCntrItem* pItem = (CPmwCntrItem*)GetNextItem(pos);
					if (pItem != NULL)
					{
						if (pItem->m_dwItemNumber == dwItemNumber)
						{
							pOleObject->Item(pItem);
							pItem->Object(pOleObject);
						/* Done with this one! */
							break;
						}
					}
				}
				ASSERT(pOleObject->Item() != NULL);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

PMGPageObjectPtr CPmwDoc::objects(void)
{
	return (PMGPageObjectPtr)page_objects->first_object();
}

void CPmwDoc::AssignPaperInfo(CPaperInfo& pPaperInfo)
{
	*m_pPaperInfo = pPaperInfo;
}

CPaperInfo* CPmwDoc::PrintPaperInfo(void)
{
	return m_pPaperInfo;
}

/////////////////////////////////////////////////////////////////////////////
// Page management.

/*
// Create the one page for our document.
*/

ERRORCODE CPmwDoc::CreatePages(void)
{
	DB_RECORD_NUMBER lPage;
	return document->new_page(&lPage);
}

/*
// Return the index of the current page (0 - NumberOfPages()-1).
*/

DWORD CPmwDoc::CurrentPageIndex(void)
{
	DB_RECORD_NUMBER lCurrentPageRecord = CurrentPageRecord();
	if (lCurrentPageRecord != m_lLastKnownPageRecord)
	{
		m_lLastKnownPageRecord = lCurrentPageRecord;
		m_lLastKnownPageIndex = document->IndexOfPage(lCurrentPageRecord);
	}
	return m_lLastKnownPageIndex;
}

CString CPmwDoc::GetPageString(DB_RECORD_NUMBER lPage /*=0*/)
{
	// Default is generic "page number" (no value).
	return "#";
}

/*
// Return the number of master pages in the document.
*/

DWORD CPmwDoc::NumberOfMasterPages(void)
{
	return document->NumberOfMasterPages();
}

/*
// Return the number of Instruction pages in the document.
*/

DWORD CPmwDoc::NumberOfInstructionPages(void)
{
	return document->NumberOfInstructionPages();
}

/*
// Return the n'th master page (0 - NumberOfMasterPages()-1).
*/

DB_RECORD_NUMBER CPmwDoc::GetMasterPage(DWORD dwPage)
{
	return document->GetMasterPage(dwPage);
}

/*
// Return the n'th Instruction page (0 - NumberOfInstructionPages()-1).
*/

DB_RECORD_NUMBER CPmwDoc::GetInstructionPage(DWORD dwPage)
{
	return document->GetInstructionPage(dwPage);
}

// Do we have a page with this record number?
BOOL
CPmwDoc::IsThisPage(DB_RECORD_NUMBER Page)
{
	DocumentPtr pDocRec = DocumentRecord();
	if (pDocRec != NULL)
	{
		return pDocRec->IndexOfPage(Page) == (DWORD)-1 ? FALSE : TRUE;
	}
	return FALSE;
}
// Return the master items for this page.
ObjectList* CPmwDoc::GetMasterItems(void)
{
	ObjectList* pMasterItems = NULL;

	int nSided = document->GetSided();
	if (nSided != DocumentRecord::NotSided
			&& !GetPageProperties()->GetHideMasterItems())
	{
		DWORD dwMasterIndex = (nSided == DocumentRecord::SingleSided)
											? 1			// Right page for 
											: ((CurrentPageIndex()+1) & 1);
		pMasterItems = document->LoadMasterPage(dwMasterIndex);
	}
	return pMasterItems;
}

// Return the Instruction items for this page.
ObjectList* CPmwDoc::GetInstructionItems(void)
{
	ObjectList* pInstructionItems = NULL;

	pInstructionItems = document->LoadInstructionPage(0);
	return pInstructionItems;
}

int CPmwDoc::GetSided(void) const
{
	return document->GetSided();
}

DWORD CPmwDoc::MasterPageIndex(void)
{
	return document->IndexOfMasterPage(CurrentPageRecord());
}

DWORD CPmwDoc::InstructionPageIndex(void)
{
	return document->IndexOfInstructionPage(CurrentPageRecord());
}

BOOL CPmwDoc::EditingMasterPage(void)
{
	return MasterPageIndex() != (DWORD)-1;
}

BOOL CPmwDoc::EditingInstructionPage(void)
{
	return InstructionPageIndex() != (DWORD)-1;
}

void CPmwDoc::GotoMasterPage(DWORD dwPage, BOOL fUpdateViews /*=TRUE*/)
{
	LoadPage(GetMasterPage(dwPage), TRUE, fUpdateViews);
}

void CPmwDoc::GotoInstructionPage(DWORD dwPage, BOOL fUpdateViews /*=TRUE*/)
{
	LoadPage(GetInstructionPage(dwPage), TRUE, fUpdateViews);
}

BOOL CPmwDoc::UpdateInstructionPages(void)
{
	if (document->NumberOfInstructionPages() == 0)
	{
		// We need to create an Instruction page.
		if (document->NewInstructionPage() != ERRORCODE_None)
		{
			return FALSE;
		}
	}
	ASSERT(document->NumberOfInstructionPages() > 0);
	return TRUE;
}


/*
// Return the record number of the current page.
*/

DB_RECORD_NUMBER CPmwDoc::CurrentPageRecord(void)
{
	return document->get_last_loaded_page();
}

/*
// Return the number of pages in the document.
*/

DWORD CPmwDoc::NumberOfPages(void)
{
	return document->NumberOfPages();
}

/*
// Return the n'th page (0 - NumberOfPages()-1).
*/

DB_RECORD_NUMBER CPmwDoc::GetPage(DWORD dwPage)
{
	return document->GetPage(dwPage);
}

BOOL CPmwDoc::CanPageBack(void)
{
	if(EditingInstructionPage())
		return FALSE;

	return CurrentPageIndex() > 0;
}

BOOL CPmwDoc::CanPageNext(void)
{
	if(EditingInstructionPage())
	{
		return FALSE;
	}

	return CurrentPageIndex() < NumberOfPages()-1;
}

void CPmwDoc::DoPageBack(void)
{
	DWORD dwCurrentPage = CurrentPageIndex();
	if (dwCurrentPage > 0)
	{
		LoadPage(GetPage(dwCurrentPage-1));
	}
}

void CPmwDoc::DoPageNext(void)
{
	DWORD dwCurrentPage = CurrentPageIndex();
	if (dwCurrentPage < NumberOfPages()-1)
	{
		LoadPage(GetPage(dwCurrentPage+1));
	}
}

/*
// Graphic update. This checks the time stamp, etc.
*/

void CPmwDoc::UpdateGraphics(ObjectList* pList /*=NULL*/)
{
	// Setup the list.
	if (pList == NULL)
	{
		pList = page_objects;
	}

	// Run through all the objects.
	PageObject* pObject;
	for (pObject = pList->first_object(); pObject != NULL; pObject = pObject->next_object())
	{
		switch (pObject->type())
		{
			case OBJECT_TYPE_Graphic:
			{
				GraphicObject* pGraphic = (GraphicObject*)pObject;
				if (pGraphic->UpdateTimeStamp())
				{
					pGraphic->changed_image();
					refresh_object(pGraphic);
				}
				break;
			}
			case OBJECT_TYPE_Group:
			case OBJECT_TYPE_Calendar:
			{
				UpdateGraphics(((GroupObject*)pObject)->object_list());
				break;
			}
		}
	}
}

void CPmwDoc::FlipProject(FLAGS Flags)
{
	DB_RECORD_NUMBER CurrentPage = CurrentPageRecord();

	for (DWORD dwMasterPage = 0; dwMasterPage < NumberOfMasterPages(); dwMasterPage++)
	{
		DB_RECORD_NUMBER MasterPage = GetMasterPage(dwMasterPage);
		FlipPage(MasterPage, Flags);
	}

	for (DWORD dwPage = 0; dwPage < NumberOfPages(); dwPage++)
	{
		DB_RECORD_NUMBER Page = GetPage(dwPage);
		FlipPage(Page, Flags);
	}

	LoadPage(CurrentPage, TRUE, FALSE);
}

void CPmwDoc::FlipPage(DB_RECORD_NUMBER Page, FLAGS Flags)
{
	// Load the page we want to flip.
	LoadPage(Page, TRUE, FALSE);

	// Get the dimensions of our universe.
   PBOX World;
   get_panel_world(&World, -1);

	// Setup the list.
	ObjectList* pObjects = page_objects;

	// Run through all the objects.
	PMGPageObjectPtr pObject;
	for (pObject = (PMGPageObjectPtr)pObjects->last_object();
		  pObject != NULL;
		  pObject = (PMGPageObjectPtr)pObject->previous_object())
	{
		// Flip the current object.
      if (Flags & OBJECT_FLAG_xflipped)
      {
         pObject->xflip();
      }
      if (Flags & OBJECT_FLAG_yflipped)
      {
         pObject->yflip();
      }

      if (pObject->get_layout() <= 0)
      {
         PBOX Bound = ((RectPageObjectPtr)pObject)->get_unrotated_bound();
         PCOORD tmp;

         if (Flags & OBJECT_FLAG_xflipped)
         {
            tmp = World.x1 - (Bound.x0 - World.x0);
            Bound.x0 = World.x1 - (Bound.x1 - World.x0);
            Bound.x1 = tmp;
         }

         if (Flags & OBJECT_FLAG_yflipped)
         {
            tmp = World.y1 - (Bound.y0 - World.y0);
            Bound.y0 = World.y1 - (Bound.y1 - World.y0);
            Bound.y1 = tmp;
         }

         ((RectPageObjectPtr)pObject)->set_unrotated_bound(Bound);
      }
      calc_object(pObject);
   }
}

void CPmwDoc::GoToFirstPage()
{

}

void CPmwDoc::SetBrowserPath(const CString& csBrowserPath)
{
	m_csBrowserPath = csBrowserPath;
	
	if(m_csFriendlyName.IsEmpty() && m_pPathManager->GetPathBindingType(csBrowserPath) == PBT_CollectionItem)
	{
		CString csColl;
		CString csFile;
		CString csKeywords;

		//get the friendly name
		m_pPathManager->BindPath(csBrowserPath, csFile, &csColl);
		CPMWCollection* pCollection = m_pCollectionManager->FindCollection(csColl);
		if(pCollection)
		{
			DWORD dwItemNumber = 0;
			pCollection->GetItemInfo(csFile, dwItemNumber, m_csFriendlyName, csKeywords);
		}
	}
}

ERRORCODE CPmwDoc::create_new_component(CCreationData* pCreationData, PBOX_PTR view_pbox, PPNT* pOrigin /*=NULL*/, ComponentObject** pObject /*= NULL*/)
{
	PMGDatabasePtr database = get_database();
	ERRORCODE error = ERRORCODE_None;

	// Create the graphic.
	ComponentObjectPtr object;
	if ((object = database->create_component_object( pCreationData )) == NULL)
	{
		error = database->last_creation_error();
	}
	else
	{
		PBOX world;
		get_panel_world(&world);
		object->SetDefaultSize( &world );

		handle_new_object(object, view_pbox, TRUE, pOrigin);

		if (pObject)
			*pObject = object;
	}

	return error;
}

void CPmwDoc::replace_component(ComponentObjectPtr pObject, CCreationData* pData, BOOL fResize )
{
	/* Toggle the object off. */
	toggle_object(pObject);
	refresh_object(pObject);

	/* Set the new component data. */
	pObject->SetComponentData( pData, fResize );
	pObject->changed_image();

	calc_object(pObject);
	toggle_object(pObject);
	refresh_object(pObject);

	SetModifiedFlag();
}

ERRORCODE 
CPmwDoc::ConvertToPaneledHTML(const CString &csDirectory,
                  CFileManager *pcfmFileList,
                  CDesignCheckerDlg *pDesignCheckerDlg)
{
   ERRORCODE	errorcode = ERRORCODE_None;

   CPmwView *pCurrentView = GetCurrentView();
   ASSERT(pCurrentView);
   if(pCurrentView == NULL)
      return ERRORCODE_IntError;
   // Remove focus from objects
   pCurrentView->set_arrow_tool();
	// Build some file stuff
	CString  Directory = csDirectory;	
	Util::AppendBackslashToPath(Directory);
	CString  FileName = Directory;
	CString Temp;
	CHTMLDocConverter::GetHTMLFileName(0, Temp);
	FileName += Temp;

	CFileManager   FileList, *pFileList;

   if(pcfmFileList)
      pFileList = pcfmFileList; // Fill in callers file list
   else
      pFileList = &FileList;        // File in local list incase of errors
	// Get some screen information
	CDC *pDC = pCurrentView->GetDC();
	if (!pDC)
	{
		return ERRORCODE_NotInitialized;
	}
	int LogPixX = pDC->GetDeviceCaps (LOGPIXELSX);
	int LogPixY = pDC->GetDeviceCaps (LOGPIXELSY);
	pCurrentView->ReleaseDC (pDC);

	int					MaxWidth = 0, PagePanel = 0;
   DWORD					dwGraphicsSize = 0, dwHTMLSize = 0;
	DWORD					dwPage, dwPages = NumberOfPages();
	// Half folds are wierd, not really 2 pages?
	if (get_project_type() == PROJECT_TYPE_HalfCard)
		dwPages = 1;
	DB_RECORD_NUMBER	lPage, lCurrentPage = CurrentPageRecord();
	short					OldPanel = get_current_panel();
	// Copy panel GIFs, save names here
	CHTMLPanelArrayArray	GifNames;
	// Go through all pages and panels
	for (dwPage = 0; dwPage < dwPages && errorcode == ERRORCODE_None; dwPage++)
	{
		if ((lPage = GetPage(dwPage)) != 0)
		{
			if (lPage != CurrentPageRecord())
				LoadPage(lPage, FALSE, TRUE);
			
			for (short panel = 0; panel < number_of_panels(); panel ++)
			{
				pCurrentView->SetPanel(panel);

				PBOX World;
				get_panel_world (&World);
				// Get the offset of the panel on the page - 
				//  we'll need to move image maps to "image coordinates"
				CRect	RectOffset;
				CHTMLObject::LogicalToDeviceUnits(World, &RectOffset);
				CPoint Dims;
				Dims.x = RectOffset.Width();
				Dims.y = RectOffset.Height();
				COLORREF	cr = PALETTEINDEX(TRANSPARENT_INDEX);
				CPageProperties *pPageProp = GetPageProperties();
				ASSERT(pPageProp);
				if (pPageProp)
					cr = colorref_from_color(pPageProp->GetBackgroundColor());
				CDIBInfo Info;
				BuildPanelDIB(Info, Dims, FALSE, cr);
				// Build GIF filename
				CString	GraphicFileName, BaseFileName;
				GraphicFileName.Format("%simg%05i.gif", Directory, PagePanel);
				// Output DIB to file
				DWORD	d = 0;
  				errorcode = CHTMLObject::OutputGIF(&GraphicFileName, Info, PALETTEINDEX(TRANSPARENT_INDEX), &d);
				if (errorcode == ERRORCODE_None)
				{
					dwGraphicsSize += d;
					// Save the widest image size
					if (Info.GetWidth() > MaxWidth)
						MaxWidth = Info.GetWidth();
					// Save the name of this file
					Util::SplitPath(GraphicFileName, NULL, &BaseFileName);
               // Create and initialize a CHTMLPanelData
					CHTMLPanelData	PanelData;
					PanelData.SetName(BaseFileName);
					// Save this file name 
					pcfmFileList->Add(GraphicFileName);
					// Object list for current page
					ObjectList *pList = object_list();  
					// Collect any URLs in this panel's objects
               PMGPageObject *pObject;
					for (pObject = (PMGPageObject*)pList->first_object();
											pObject != NULL;
											pObject = (PMGPageObject*)pObject->next_object())
					{
						if (pObject->type() == OBJECT_TYPE_Frame)
						{
							CHTMLObject::TextLinksAsMaps(pObject, &PanelData);	
						}
						else
						{
							HyperlinkData  HData;
							pObject->GetHyperlinkData(&HData);
							if (HData.IsValid() == TRUE)
							{
								ASSERT(HData.StringType != TYPE_PageURL);
								// No site page links allowd in paneled HTML
								if (HData.StringType != TYPE_PageURL)
								{
									CString URL;
									if(document)
										document->AppendFullURL(HData, URL);
										
									PMObjectShapeData sd;
									pObject->GetShapeData(sd);
									//PanelData.Add(HData, sd);
									CHTMLObject::AddImageMapInfo(HData, sd, PanelData, URL);
								}
							}
						}
					}
					CHTMLObject::FinalizeImageMaps(DocumentRecord(), PanelData, RectOffset);					
					GifNames.Add(PanelData);
					PagePanel ++;
				}
			}
		}
	}
	// Put things as they were
	if (lCurrentPage != CurrentPageRecord())
		LoadPage(lCurrentPage, TRUE, TRUE);
	if (OldPanel != get_current_panel())
		pCurrentView->SetPanel(OldPanel);

   CHTMLDocConverter    docConverter(this);
   if (docConverter.BuildHTMLForGifs(FileName, GifNames, MaxWidth, pcfmFileList, &dwHTMLSize) == ERRORCODE_None)
		pcfmFileList->Add(FileName);

   CWebPageStats  WebPageStats;

   WebPageStats.SetSizes (dwGraphicsSize, 0, dwHTMLSize);

	if (errorcode == ERRORCODE_None)
	{
		if (pDesignCheckerDlg != NULL)
		{
			pDesignCheckerDlg->AddPageStats(WebPageStats);
		}
	}

   return errorcode;
}

ERRORCODE 
CPmwDoc::ConvertToPagedHTML(const CString &csDirectory,
   CFileManager *pcfmFileList,/*=NULL*/
   CDesignCheckerDlg *pDesignCheckerDlg /*NULL*/)
{
	ObjectList *         pList;
   CHTMLDocConverter    docConverter(this);//, csDirectory, *pcsHomePage, ext);
   ERRORCODE            errorcode = ERRORCODE_None;

   CPmwView *pCurrentView = GetCurrentView();
   ASSERT(pCurrentView);
   if(pCurrentView == NULL)
      return ERRORCODE_IntError;
   // Remove focus from objects
   pCurrentView->set_arrow_tool();
	// Save the current page record. We will restore it later.
	DB_RECORD_NUMBER lCurrentPage = CurrentPageRecord();
	
	// Run through all the pages and produce HTML for them.
	DWORD dwPages = NumberOfPages();

	for (DWORD dwPage = 0; dwPage < dwPages && errorcode == ERRORCODE_None; dwPage++)
	{
		CString  FileName = csDirectory;
		Util::AppendBackslashToPath(FileName);
		CString Temp;
		CHTMLDocConverter::GetHTMLFileName(dwPage, Temp);
		FileName += Temp;
		// Get the record number of the next page.
		DB_RECORD_NUMBER lPage = GetPage(dwPage);
		if (lPage != 0)
		{
			// Load the page if not already current.
			if (lPage != CurrentPageRecord())
			{
				LoadPage(lPage, TRUE, FALSE);
			}
   		pList = object_list();  // Object list for current page
			errorcode = BuildHTMLObjectList(pList, *docConverter.GetHTMLObjectList());

			if(errorcode != ERRORCODE_None)
				continue;

		#ifdef _DEBUG
		#ifdef DUMP_DOC
			afxDump << "Dumping HTMLDocumentConverter Object\n";
			docConverter.Dump(afxDump);
		#endif
		#endif
			CWebPageStats  WebPageStats;
			// Convert PrintMaster document to HTML
			if ((errorcode = docConverter.ConvertToPagedHTML(FileName, pcfmFileList, dwPage + 1, &WebPageStats)) == ERRORCODE_None)
			{
				if (pDesignCheckerDlg != NULL)
				{
					pDesignCheckerDlg->AddPageStats(WebPageStats);
				}
			}

		#ifdef _DEBUG
		#ifdef DUMP_DOC
			afxDump << "Dumping SORTED HTMLDocumentConverter Object\n";
			docConverter.Dump(afxDump);
		#endif
		#endif
			// Remove objects from current page (Empty object list)
			docConverter.RemoveAll();
		}
	}
	// Reload the original current page if not now current.
	if (lCurrentPage != CurrentPageRecord())
	{
		LoadPage(lCurrentPage, TRUE, FALSE);
	}

   return errorcode;
}

ERRORCODE 
CPmwDoc::BuildHTMLObjectList(ObjectList *pList, CPtrList &HTMLObjects)
{
	PMGPageObject *   pObject;
   ERRORCODE         errorcode;

	for (pObject = (PMGPageObject*)pList->first_object();
						pObject != NULL;
						pObject = (PMGPageObject*)pObject->next_object())
	{
		switch (pObject->type())
		{
			case OBJECT_TYPE_Graphic:
			case OBJECT_TYPE_Frame:
         case OBJECT_TYPE_Rectangle:
         case OBJECT_TYPE_Ellipse:
         case OBJECT_TYPE_Line:
         case OBJECT_TYPE_DrawingObject:
         case OBJECT_TYPE_OleObject:
         case OBJECT_TYPE_Serif:
         // Process all unhandled object types for now
			default:
         {
//            GraphicObject *pGraphicObject = (GraphicObject *) pObject;
//            CFrameObject *pFrameObject = (CFrameObject *) pObject;
         // TODO - add error handling here
         // if object was skipped since it is off-page, ignore error
            errorcode = AddPageObject(pObject, HTMLObjects);
            break;
         }
			case OBJECT_TYPE_Group:
         {
            GroupObject *pGroupObject = (GroupObject *) pObject;
            // Break up group into objects and add to document converter
            BuildHTMLObjectList(((GroupObject*)pObject)->object_list(), HTMLObjects);
            break;
         }
		}
	}
   return ERRORCODE_None;
}

ERRORCODE 
CPmwDoc::AddPageObject(PMGPageObject *pObject, CPtrList &HTMLObjects, BOOL OneBigGraphic)
{
   CRect			         crectDeviceObjectPosition, crectGraphic, RectBound;
   CRect                crectDevicePageDims;
   PBOX                 pboxPageDims;
   BOOL                 bMatch = FALSE;
   CHTMLObject          *pHTMLObject;
   POSITION             pos, posMatched;
   ERRORCODE            errorcode;
   PPNT                 PagePntDims;

   ASSERT(pObject);

   CHTMLObject::LogicalToDeviceUnits(pObject, crectDeviceObjectPosition, &RectBound);
   // Get Document Dimensions and convert to device units
   PagePntDims = get_dimensions();
   pboxPageDims.x0 = pboxPageDims.y0 = 0;
   pboxPageDims.x1 = PagePntDims.x;
   pboxPageDims.y1 = PagePntDims.y;
   CHTMLObject::LogicalToDeviceUnits(pboxPageDims, &crectDevicePageDims);
   // Don't add off-page objects
   // First check if object is completely off page
   // If empty, Add object to Warning/Error list indicating error
   if(crectDeviceObjectPosition.left >= crectDevicePageDims.right ||
      crectDeviceObjectPosition.top >= crectDevicePageDims.bottom ||
      crectDeviceObjectPosition.bottom <= 0 ||
      crectDeviceObjectPosition.right <= 0)
         return ERRORCODE_None;
   // Loop through existing objects to check if object we are trying to add
   // intersects with one of existing ones.  If it intersects with existing
   // one, add it to existing objects group.  If it doesn't, add it as 
   // a new object.
   posMatched = NULL;
   for(pos = HTMLObjects.GetHeadPosition(), bMatch = FALSE;
      pos != NULL && !bMatch;
      HTMLObjects.GetNext(pos))
   {
      pHTMLObject = (CHTMLObject *) HTMLObjects.GetAt(pos);
      ASSERT(pHTMLObject);
      if ((OneBigGraphic == TRUE) || (pHTMLObject->DoesRectIntersect(crectDeviceObjectPosition)))
      {
         pHTMLObject->Add(pObject, crectDeviceObjectPosition, RectBound);
         posMatched = pos;
         bMatch = TRUE;
      }
   }
   // If object we are adding doesn't intersect with existing object, 
   // create a new  object with this PrintMaster object 
   if(!bMatch)
   {
      CHTMLObject * pNewHTMLObject = new CHTMLObject;
//if (pNewHTMLObject->IsLink() == TRUE)
//   JTRACE("\nCHTMLDocConverter::Add added new HTML object with hyperlink \n");
      // Add PrintMaster object to new GraphicObject
      if((errorcode=pNewHTMLObject->Add(pObject, crectDeviceObjectPosition, RectBound)) != ERRORCODE_None)
         return errorcode;
      // Add new Object to  Object pointer list
      HTMLObjects.AddTail(pNewHTMLObject);
   }
   else
   {
      // Go through object list an merge intersecting objects resulting
      // from image area growing as a result of previous  object
      // addition
      CHTMLObject * pUpdatedHTMLObject = pHTMLObject;
      pos = HTMLObjects.GetHeadPosition();
      while(pos != NULL)
      {
         pHTMLObject = (CHTMLObject *) HTMLObjects.GetAt(pos);
         ASSERT(pHTMLObject);
         if(pHTMLObject != pUpdatedHTMLObject)
         {
            pHTMLObject->GetRect(&crectGraphic);
            if ((OneBigGraphic == TRUE) || (pUpdatedHTMLObject->DoesRectIntersect(crectGraphic)))
            {
               // Merge updated object with Object that was just found
               // then remove updated object (from initial Add())
               // Updated/new object is merged with object that it intersects with
               // This maintains correct Z-order where last item in object list
               // is topmost.
               pHTMLObject->Add(pUpdatedHTMLObject);
               // Release memory associated with this object
               delete pUpdatedHTMLObject;
               // Remove this object from list
               HTMLObjects.RemoveAt(posMatched);
               bMatch = TRUE;
               // Updated object is now the one that was just found
               pUpdatedHTMLObject = pHTMLObject;
               posMatched = pos;
               // Start checking for intersecting objects 
               // from list top again since region grew
					pUpdatedHTMLObject = pHTMLObject;
					posMatched = pos;
               pos = HTMLObjects.GetHeadPosition();
               continue;
            }
         }
         HTMLObjects.GetNext(pos);
      }
   }
   return ERRORCODE_None;
}
/////////////////////////////////////////////////////////////////////////////
// DIB generation
#pragma optimize("",off)
ERRORCODE CPmwDoc::BuildPanelDIB(CDIBInfo& Info, POINT MaxDims, BOOL bTrueColor/*=FALSE*/, COLORREF clBackground /*=RGB(255, 255, 255)*/)
{
   // Build the DIB for the whole panel.
   PBOX World;
   get_panel_world(&World);

   return BuildDIB(Info, World, MaxDims, NULL, bTrueColor, clBackground);
}

ERRORCODE CPmwDoc::BuildDIB(CDIBInfo& Info, PBOX Bound, POINT MaxDims, CPtrList *plObjectList/*=NULL*/, BOOL bTrueColor/*=FALSE*/, COLORREF clBackground /*=RGB(255, 255, 255)*/)
{
   int   nBytesPerPixel = (bTrueColor == TRUE) ? 3 : 1;
   PPNT  dims;

   dims.x = Bound.x1 - Bound.x0;
   dims.y = Bound.y1 - Bound.y0;

   double dX = (double)dims.x;
   double dY = (double)dims.y;

   double dXScale = (double)MaxDims.x/dX;
   double dYScale = (double)MaxDims.y/dY;
   double dScale = dXScale < dYScale ? dXScale : dYScale;

   int nWidth = (int)(dScale*dX);
   int nHeight = (int)(dScale*dY);

   if (nWidth == 0)
   {
      nWidth++;
   }
   if (nHeight == 0)
   {
      nHeight++;
   }

   TRACE("w: %d, h: %d; A: %d\r\n", nWidth, nHeight, nWidth*nHeight);

   WORD wScaleNum = (WORD)(dScale*16384);
   WORD wScaleDenom = 16384;


	RedisplayContext rc;
	rc.hwnd = NULL;

	rc.x_resolution =
		rc.y_resolution = PAGE_RESOLUTION;

	rc.SetScaling(wScaleNum, wScaleDenom);
	od("num:%u, denom:%u\r\n", wScaleNum, wScaleDenom);

	rc.destination_rect.left =
		rc.destination_rect.top =
		rc.destination_x0 =
		rc.destination_y0 = 0;

	rc.destination_rect.right = nWidth;
	rc.destination_rect.bottom = nHeight;

	rc.clip_rect = rc.destination_rect;

	rc.outline_gamma_curve =
		rc.bitmap_gamma_curve = screen_gamma_curve;

	rc.source_pbox = Bound;

	rc.scaled_source_x0 = rc.source_pbox.x0 * rc.x_resolution;
	rc.scaled_source_y0 = rc.source_pbox.y0 * rc.y_resolution;

// od("dims:%ld, %ld\r\n", dims);

	// Recompute the destination with the rc parameters we have set.
	// Sometimes it rounds down a pixel.
	PBOX RealBound = Bound;
	rc.pbox_to_screen(&RealBound, TRUE);
	CRect crReal;
	rc.convert_pbox(&RealBound, &crReal, NULL);

	nWidth = crReal.Width();
	nHeight = crReal.Height();

	// Update the rc with the revised numbers.
	rc.destination_rect.right = nWidth;
	rc.destination_rect.bottom = nHeight;

	rc.clip_rect = rc.destination_rect;

/*
// Allocate the packed DIB.
*/

   int nByteWidth = ((nWidth * nBytesPerPixel) + 3) & ~3;
   DWORD dwDataBytes = (DWORD)nByteWidth * (DWORD)nHeight;
   int nHeaderBytes;

   if(bTrueColor)
      nHeaderBytes = sizeof(BITMAPINFOHEADER);
   else
      nHeaderBytes = sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);

   TRY
   {
      // Allocate memory buffers in the info.
      Info.Allocate(nHeaderBytes, dwDataBytes);
   }
   CATCH_ALL(e)
   {
      ERRORCODE_Memory;
   }
   END_CATCH_ALL

   // Fill out the bitmap info.
   BITMAPINFO* pbi = Info.BitmapInfo();

   pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   pbi->bmiHeader.biWidth = (LONG)nWidth;
   pbi->bmiHeader.biHeight = (LONG)nHeight;
   pbi->bmiHeader.biPlanes = 1;
   pbi->bmiHeader.biBitCount = 8 * nBytesPerPixel;
   pbi->bmiHeader.biCompression = BI_RGB;
   pbi->bmiHeader.biSizeImage = dwDataBytes;
   pbi->bmiHeader.biXPelsPerMeter =
      pbi->bmiHeader.biYPelsPerMeter = 0;
   pbi->bmiHeader.biClrImportant = 0;
   if(bTrueColor)
      {
         pbi->bmiHeader.biClrUsed = 0;
      }
   else
      {
         pbi->bmiHeader.biClrUsed = 256;
         memcpy(pbi->bmiColors, GetIdentityPalette(), 256*sizeof(RGBQUAD));
      }


// od("Preview dims: %d, %d\r\n", width, height);

   ERRORCODE nSuccess = ERRORCODE_Fail;
   CDibDC dc;

   if (dc.Create(pbi))
   {
      // Select our palette into the DC.
      dc.SelectPalette(pOurPal, FALSE);
      dc.RealizePalette();

      SHORT dc_res = dc.GetDeviceCaps(LOGPIXELSX);

      // Clear the map.
		if (nBytesPerPixel == 1 && (clBackground & 0x01000000))
		{
			// This handles our transparent trick correctly.
			memset(dc.GetData(), clBackground & 0xff, dwDataBytes);
		}
		else
		{
			CBrush cbBackground;
			if (cbBackground.CreateSolidBrush(clBackground))
			{
				CRect r(0, 0, nWidth, nHeight);
				dc.FillRect(&r, &cbBackground);
			}
		}

      rc.destination_hdc = dc.GetSafeHdc();

      // Palette device.

      rc.info.planes = 1;
      if(bTrueColor)
      {
         rc.info.bpp = 24;
         rc.info.type = RASTER_TYPE_24;
      }
      else
      {
         rc.info.bpp = 8;
         rc.info.type = RASTER_TYPE_256;
      }

      rc.is_thumbnail = TRUE;       // We are drawing the thumbnail.

      /* Update all the objects into the DC. */

      REFRESH_EXTENT re;

      /* Build the refresh extent. */

      re.extent = rc.source_pbox;
      re.refresh_type = REFRESH_ALL;
      re.my_object = NULL;
      re.update_object = NULL;
      re.update_state = NULL;

      /*
      // Draw all objects into this extent.
      // Note whether the user terminated or not.
      */

      BOOL fTerminated;
      if(plObjectList == NULL)
         fTerminated = draw_objects(&re, &rc);
      else
         fTerminated = DrawObjects(plObjectList, &re, &rc);

      // Get the bitmap bits.
      if (!fTerminated)
      {
#if defined(WIN32) && !defined(_MAC)
         // The 32-bit CDibDC uses a DIB section, which buffers its own
         // image. We want it in our structure, so we need to copy the
         // data over at this point.
         memcpy(Info.BitmapData(), dc.GetData(), dwDataBytes);
#endif
         nSuccess = ERRORCODE_None;
      }

      CPalette StockPal;
      StockPal.CreateStockObject(DEFAULT_PALETTE);
      dc.SelectPalette(&StockPal, FALSE);

      dc.DeleteDC();
   }

   return nSuccess;
}

void CPmwDoc::ChangeRotation(double dNewAngle)
{
   PMGPageObjectPtr pObject = selected_objects();
   if (pObject != NULL)
   {
   /*
   // Set up the undo/redo command.
   */
//       CCmdChangeObject* pCommand = new CCmdChangeObject(IDCmd_ChangeRotation);

//       if (!pCommand->Before(pDoc, pObject))
      CCmdChangeSelects* pCommand = new CCmdChangeSelects(IDCmd_ChangeRotation);

      if (!pCommand->Before(GetCurrentView()))
      {
         delete pCommand;
         pCommand = NULL;
      }

		// Mark frames below this object.
		MarkFramesBelow(pObject);

      toggle_object(pObject);
      refresh_object(pObject);
      
      switch (pObject->type())
      {
         case OBJECT_TYPE_Line:
         {
            PBOX Bounds = pObject->get_bound();
            double dx = fabs((double)(Bounds.x1-Bounds.x0));
            double dy = fabs((double)(Bounds.y1-Bounds.y0));
            double r = sqrt(dx*dx+dy*dy)/2.0;
            double sx = r*cos(dNewAngle);
            double sy = r*sin(dNewAngle);
            double x = fabs(sx);
            double y = fabs(sy);
            PCOORD rx = (PCOORD)(x+0.5);
            PCOORD ry = (PCOORD)(y+0.5);
            PCOORD x2 = (PCOORD)(x*2.0+0.5);
            PCOORD y2 = (PCOORD)(y*2.0+0.5);
            PCOORD cx = (Bounds.x0+Bounds.x1+1)/2;
            PCOORD cy = (Bounds.y0+Bounds.y1+1)/2;
            Bounds.x0 = cx-rx;
            Bounds.y0 = cy-ry;
            Bounds.x1 = Bounds.x0+x2;
            Bounds.y1 = Bounds.y0+y2;
            ((LineObject*)pObject)->set_bound(Bounds);
            
            pObject->remove_flags(OBJECT_FLAG_xflipped|OBJECT_FLAG_yflipped);
            
            if (((sx >= 0.0) ^ (sy >= 0.0)) == 0)
            {
               pObject->add_flags(OBJECT_FLAG_xflipped);
            }
            
            break;
         }
			case OBJECT_TYPE_Frame:
			{
				GetCurrentView()->free_edit_background();
            // Fall through to...
			}
         default:
         {
            pObject->set_rotation(dNewAngle);
            break;
         }
      }
   
      pObject->calc();
      pObject->changed_image();
      toggle_object(pObject);
      refresh_object(pObject);

		UpdateMarkedFrames(TRUE);
		SetModifiedFlag();

   /*
   // Finish up the undo/redo command.
   */

      if (pCommand != NULL)
      {
         if (pCommand->After())
         {
            AddCommand(pCommand, FALSE);
         }
         else
         {
            delete pCommand;
         }
      }
   }
}



//
// Enable Add Page command for multi-page project types
//
void CPmwDoc::OnUpdateAddPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsMultiPageProjectType());
}

//
// Enable Goto Page command for multi-page project types
//
void CPmwDoc::OnUpdateGotoPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsMultiPageProjectType());
}

/////////////////////////////////////////////////
// MakeFontSubstitutions
//
// Replaces usage of illegally distributed fonts within
// the document with legally distributed fonts
//
void CPmwDoc::MakeFontSubstitutions()
{
	// first make substitutions in the base style(s)
	PMGFontServer* pServer = (PMGFontServer*)get_font_server();
	bool fReflowAll = pServer->SubstituteFonts();

	// next check substitutions in the style changes
	DocumentPtr pDocRec = DocumentRecord();
	ASSERT(pDocRec != NULL);
	DWORD dwStories = pDocRec->NumberOfStories();
	//loop through stories
	for (DWORD dwStoryIndex = 0; dwStoryIndex < dwStories; dwStoryIndex++)
	{
		DB_RECORD_NUMBER lStory = pDocRec->GetStory(dwStoryIndex);

		// Lock the text record.
		CTextRecord *pTextRecord = (CTextRecord*)(get_database()->get_record(lStory, NULL, RECORD_TYPE_Text));
		if (pTextRecord )
		{
			// if a substitution is made reflow text
			if( pTextRecord->SubstituteFonts( pServer ) || fReflowAll )
			{
				// reflow all text in this record
				pTextRecord->Reflow();
			}
			pTextRecord->release();
		}
	}
}

#pragma optimize("",on)

