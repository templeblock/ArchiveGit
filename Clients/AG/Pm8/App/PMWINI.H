/*
// $Header: /PM8/App/PMWINI.H 1     3/03/99 6:09p Gbeddow $
//
// INI file definitions.
//
// $Log: /PM8/App/PMWINI.H $:
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 14    10/05/98 3:03p Jayn
// More content manager cleanup.
// 
// 13    9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 12    8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 11    8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 10    7/16/98 1:45p Psasse
// Added Advanced Printing Functionality "Print As Bitmap"
// 
// 9     4/29/98 9:48p Psasse
// 
// 8     4/27/98 5:30p Fredf
// Improved bar positioning.
// 
// 7     4/18/98 4:24p Fredf
// T-Shirt printing.
// 
// 6     4/16/98 7:33p Fredf
// "Preview Actual Size" control in font dialog (NEWFONT.CPP)
// 
// 5     4/14/98 8:00p Fredf
// First attempt at Help Window.
// 
// 4     4/02/98 9:09p Psasse
// internet cache support
// 
// 3     3/24/98 2:10p Rlovejoy
// Added new strings for MRU font list.
// 
// 2     3/21/98 1:18p Fredf
// New menus and toolbar names.
// 
//    Rev 1.0   14 Aug 1997 15:24:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:16   Fred
// Initial revision.
// 
//    Rev 1.11   14 Jul 1997 11:19:00   Fred
// Removed some obsolete ifdefs
// 
//    Rev 1.10   07 May 1997 12:10:54   Jay
// User directory and collections
// 
//    Rev 1.9   17 Apr 1997 16:06:10   Jay
// Initial collection integration
// 
//    Rev 1.8   08 Apr 1997 16:11:18   Jay
// Print Instructions control
// 
//    Rev 1.7   19 Feb 1997 16:53:28   Jay
// Snapping
// 
//    Rev 1.6   06 Feb 1997 09:13:28   Jay
// Rulers!
// 
//    Rev 1.5   16 Jan 1997 13:02:52   Jay
// New status stuff. More cleanup.
// 
//    Rev 1.4   15 Jan 1997 10:49:38   Jay
// New toolbars
// 
//    Rev 1.3   09 Jan 1997 16:19:48   dennisp
// Changes to make workspace not shared.
// 
//    Rev 1.2   06 Aug 1996 11:17:40   Jay
//  
// 
//    Rev 1.1   31 Jul 1996 18:22:02   Jay
// Spelling, etc.
// 
//    Rev 1.0   14 Mar 1996 13:29:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:40   FRED
// Initial revision.
// 
//    Rev 2.16   20 Nov 1995 11:18:12   JAY
// Nre reverse landscape check
// 
//    Rev 2.15   17 Nov 1995 11:12:04   JAY
// Added key to allow skipping of share test.
// 
//    Rev 2.14   09 Oct 1995 15:16:10   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.13   06 Oct 1995 11:59:42   JAY
// New print code, etc.
// 
//    Rev 2.12   03 Oct 1995 10:03:06   JAY
// Label/envelope printing fixes
// 
//    Rev 2.11   30 Aug 1995 14:15:24   JAY
// New free pictures album.
// 
//    Rev 2.10   28 Aug 1995 20:32:36   FRED
// Print alignment
// 
//    Rev 2.9   24 Aug 1995 16:27:32   JAY
// Added undo limit and photo cd entries.
// 
//    Rev 2.8   08 Aug 1995 11:01:46   JAY
// Envelope stuff.
// 
//    Rev 2.7   28 Jul 1995 12:29:48   JAY
// More strings for printer section.
// 
//    Rev 2.6   14 Jul 1995 17:24:36   JAY
// Picture mapping.
// 
//    Rev 2.5   16 Jun 1995 13:00:28   JAY
//  
// 
//    Rev 2.4   05 Jun 1995 11:33:24   JAY
//  
// 
//    Rev 2.3   19 Apr 1995 12:34:36   JAY
//  
// 
//    Rev 2.2   15 Mar 1995 10:48:36   JAY
// Support for new text editing
// 
//    Rev 2.1   08 Feb 1995 13:33:42   JAY
// Reverted. New series.
// 
//    Rev 1.20   09 Jan 1995 16:58:52   JAY
// Cut & paste, etc.
// 
//    Rev 1.19   01 Dec 1994 09:51:58   JAY
//  
// 
//    Rev 1.18   30 Nov 1994 17:02:28   JAY
//  
// 
//    Rev 1.17   06 Oct 1994 15:15:54   JAY
// Added NewMargins and MarginStyle.
// 
//    Rev 1.16   19 Sep 1994 09:55:52   JAY
// Added SeenSleeper
// 
//    Rev 1.15   19 Jul 1994 09:06:46   JAY
// Added SoundBits entry.
// 
//    Rev 1.14   20 Jun 1994 13:30:38   JAY
// Removed CDCache and CDCacheSize.
// Added Workspace entry and Sizes section.
// 
//    Rev 1.13   14 Jun 1994 16:55:48   JAY
// Added CDCacheSize.
// 
//    Rev 1.12   10 Jun 1994 10:20:28   JAY
// Added CDCache[].
// 
//    Rev 1.11   01 Jun 1994 16:45:48   JAY
// Added mention of FontPreviewFile.
// 
//    Rev 1.10   31 May 1994 12:45:04   JAY
// Added drive type overrides.
// 
//    Rev 1.9   16 May 1994 08:48:06   JAY
// Added OutlineGammaValue.
// 
//    Rev 1.8   06 May 1994 17:11:40   JAY
// More support for CDs and sound sequences.
// 
//    Rev 1.7   28 Apr 1994 10:43:44   JAY
// New sound code
// 
//    Rev 1.6   13 Apr 1994 17:17:40   JAY
// Added PostscriptType under the [Printer] section.
// 
//    Rev 1.5   07 Apr 1994 10:51:34   JAY
// Added UseDDBs entry under configuration.
// 
//    Rev 1.4   01 Apr 1994 12:01:00   JAY
// Added sound entries.
*/

/*
// These strings can be found in PMW.CPP.
// Unless otherwise noted, these entries are in the user's nid'd config file
// (i.e. _xxxPMW.INI).
*/

extern char far SECTION_Paths[];
extern char far ENTRY_PictureSearchPath[];
extern char far ENTRY_CachePath[];
extern char far ENTRY_LicensePath[];

extern char far SECTION_Debug[];
extern char far ENTRY_SeeMessages[];
extern char far ENTRY_SeeHelp[];
extern char far ENTRY_EditTemplates[];
extern char far ENTRY_MakeTemplates[];

extern char far SECTION_Fonts[];
extern char far ENTRY_CacheKSize[];
extern char far ENTRY_DefaultFace[];
extern char far ENTRY_DefaultSize[];

extern char far SECTION_Image[];
extern char far ENTRY_MinDiskSpaceK[];
extern char far ENTRY_MaxScaleUp[];
extern char far ENTRY_MaxScaleDown[];

extern char far SECTION_BDE[];
extern char far ENTRY_DllPath[];

extern char far SECTION_Printer[];
extern char far ENTRY_InfFile[];
extern char far ENTRY_GammaValue[];
extern char far ENTRY_OutlineGammaValue[];
extern char far ENTRY_FastBitmapGammaValue[];
extern char far ENTRY_PostscriptType[];
extern char far ENTRY_EnvelopePosition[];
extern char far ENTRY_DefaultEnvelopePosition[];
extern char far ENTRY_BadDC[];
extern char far ENTRY_Copies[];
extern char far ENTRY_Orientation[];
extern char far ENTRY_Tractor[];
extern char far ENTRY_LeftMargin[];
extern char far ENTRY_TopMargin[];
extern char far ENTRY_RightMargin[];
extern char far ENTRY_BottomMargin[];
extern char far ENTRY_LeftOffset[];
extern char far ENTRY_TopOffset[];
extern char far ENTRY_LeftMarginLandscape[];
extern char far ENTRY_TopMarginLandscape[];
extern char far ENTRY_RightMarginLandscape[];
extern char far ENTRY_BottomMarginLandscape[];
extern char far ENTRY_LeftOffsetLandscape[];
extern char far ENTRY_TopOffsetLandscape[];
extern char far ENTRY_ReverseLandscape[];			// Default value
extern char far ENTRY_UseReverseLandscape[];		// Current value
extern char far ENTRY_FastBitmaps[];
extern char far ENTRY_DoubleSidedType[];
extern char far ENTRY_DoubleSided[];
extern char far ENTRY_PrintInstructions[];
extern char far ENTRY_PrintReversed[];
extern char far ENTRY_FlagPrintAsBitmap[];

extern char far SECTION_Album[];
extern char far ENTRY_ProjectAlbum[];
extern char far ENTRY_PictureAlbum[];
extern char far ENTRY_FreePictureAlbum[];

extern char far SECTION_Configuration[];
extern char far ENTRY_TextCompatability[];
extern char far ENTRY_RegistrationCode[];	// In main PMW.INI
extern char far ENTRY_UseDDBs[];
extern char far ENTRY_MarginStyle[];
extern char far ENTRY_NewMargins[];
extern char far ENTRY_ReminderCount[];		// In main PMW.INI
extern char far ENTRY_OEMID[];					// In main PMW.INI
extern char far ENTRY_BonusCode[];				// In main PMW.INI
extern char far ENTRY_BonusPhone[];			// In main PMW.INI
extern char far ENTRY_AddressBook[];
extern char far ENTRY_ProtoDatabase[];
extern char far ENTRY_UseProjectAlbum[];
extern char far ENTRY_UsePictureAlbum[];
extern char far ENTRY_PictureFileMap[];
extern char far ENTRY_UndoLimit[];
extern char far ENTRY_SkipShareTest[];
extern char far ENTRY_ShowRulers[];
extern char far ENTRY_SnapToRulers[];
extern char far ENTRY_LastRunLevel[];
extern char far ENTRY_ShowActualFontSize[];

//extern char far ENTRY_CDPath[];				// In nid'd and PMW.INI files.
															// Defined & used in PMWCD.CPP
//extern char far ENTRY_FontPreviewFile[];	// In main PMW.INI
															// Defined & used in TYPEFACE.CPP

extern char far SECTION_Workspace[];         // Workspace settings - DGP
extern char far ENTRY_WorkspaceFileName[];   // Added by DGP
extern char far ENTRY_WorkspaceSize[];       // Added by DGP

extern char far SECTION_PhotoCD[];
extern char far ENTRY_Draft[];
extern char far ENTRY_Proof[];

extern char far SECTION_FontsMRU[];
extern char far ENTRY_FontMRU[];

// Defined and used in DRIVEMGR.CPP
//extern char far SECTION_Drives[];
//extern char far ENTRY_IsCDROM[];
//extern char far ENTRY_IsRemote[];
//extern char far ENTRY_IsFixed[];
//extern char far ENTRY_IsRemovable[];

// Defined and used in CLIP.CPP
//extern char far SECTION_Clipboard[];
//extern char far ENTRY_Format1[];
//extern char far ENTRY_Format2[];
//extern char far ENTRY_Format3[];

// Defined and used in SPELLCHK.CPP
//extern char far SECTION_Spelling[];
//extern char far ENTRY_IgnoreNumbers[];
//extern char far ENTRY_IgnoreUpperCase[];


