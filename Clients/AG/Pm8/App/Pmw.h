/*
// $Workfile: PMW.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMW.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 73    3/02/99 3:40p Lwu
// added: CPUIPageComponentManager m_pageManager;
// CPUIImageComponentManagerIndexedDB m_imageManager;
// and accessor methods
// 
// 
// 72    2/26/99 10:08a Johno
// Moved AppendNamedApplicationRegistryPath() from PMW.CPP to INIFILE.CPP
// 
// 71    2/25/99 5:23p Johno
// Moved AppendNamedApplicationRegistryPath() from UTIL.CPP to PMW.CPP,
// as some shared versions of UTIL.CPP could not find needed H file
// 
// 70    2/04/99 6:20p Blee
// Added support for PSLaunchDLL module (RunPrintShopWizard())
// 
// 69    2/04/99 3:05p Johno
// Changes for new "Blank Page" (poster) project type
// 
// 68    1/28/99 4:48p Mwilson
// added resolution change on the fly
// 
// 67    1/14/99 9:36a Cschende
// added an enum for the user prefered date format and class member to
// keep track of the user preference and a method for getting the date
// formatted in a string.
// 
// 66    12/23/98 5:33p Psasse
// support for new project type - easyprints
// 
// 65    12/19/98 4:38p Psasse
// Revert to Saved support
// 
// 64    12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 63    11/20/98 1:32p Mwilson
// added capability to turn off auto instructions in preferences
// 
// 62    11/06/98 12:20p Mwilson
// added a member for large font mode
// 
// 61    11/02/98 5:14p Jayn
// 
// 60    10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 59    10/26/98 1:24p Johno
// Moved  OnFileOpen() from CPmwApp to CMainFrame
// 
// 58    10/19/98 10:50a Jayn
// More COM restructuring changes.
// 
// 57    10/12/98 4:13p Johno
// Eliminated CPMContentServer in art store build
// 
// 56    10/07/98 5:46p Jayn
// Changes to the content manager.
// 
// 55    10/05/98 3:03p Jayn
// More content manager cleanup.
// 
// 54    10/02/98 7:10p Jayn
// More content manager changes
// 
// 53    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 52    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 51    9/15/98 4:23p Psasse
// SetOnlineDataServerAddress()
// 
// 50    9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 49    9/14/98 12:10p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 48    9/10/98 4:51p Mwilson
// added flag for tool running
// 
// 47    8/20/98 12:45p Hforman
// Craft changes; add another OpenDocumentFile()
// 
// 46    8/06/98 4:33p Mwilson
// added new craft doc template
// 
// 45    8/05/98 12:00p Mwilson
// moved check art util to mainfrm
// 
// 44    7/06/98 5:43p Rlovejoy
// Function to update added supported projects mask.
// 
// 43    6/30/98 6:23p Psasse
// Online Graphics server address becomes a member of the app and is
// pmgs.ini dependent
// 
// 42    6/29/98 11:42a Hforman
// clean up unused helpful hint methods
// 
// 41    6/24/98 7:20p Psasse
// correctly extracting *.pmo and *.dtt files
// 
// 40    6/19/98 6:20p Johno
// Changed PreventHub, added SetPreventHub
// 
// 39    6/18/98 5:19p Cboggio
// Changes to SetNewBusinessCardSize function for localization
// 
// 38    6/18/98 12:38p Johno
// Improved hub control
// 
// 37    6/18/98 11:02a Johno
// KillFirstPoster() now boolean
// 
// 36    6/17/98 12:19p Johno
// Moved OnHubNew() to MainFrm
// 
// 35    6/12/98 2:55p Jayn
// More startup changes.
// 
// 34    6/12/98 10:38a Jayn
// Changes to the startup code (no more 5-second delay)
// 
// 33    6/05/98 12:10p Fredf
// Changes for new legal and copyright information.
// 
// 32    6/04/98 7:59p Rlovejoy
// Added parameter for ShowHelpfulHin().
// 
// 31    6/01/98 9:12p Psasse
// an attempt to get helpful hints on top of border editor
// 
// 30    6/01/98 1:21p Mwilson
// added flag for determining if this the product AG is using to fulfill
// personal delivery.
// 
// 29    5/31/98 5:20p Psasse
// fixed initial daily tip coming in under the hub
// 
// 28    5/28/98 2:54p Hforman
// add Helpful Hint voice-over functions
// 
// 27    5/27/98 7:17p Rlovejoy
// Made KillFirstPoster() public.
// 
// 26    5/26/98 1:54p Dennis
// Added GetAppIcon()
// 
// 25    5/20/98 9:19p Psasse
// Support for larger bitmaps in Helpful hints window and asynchronous
// progress control becomes more like IE
// 
// 24    5/14/98 10:34a Mwilson
// added utility to check for the existence of art files form pmw files
// 
// 23    5/11/98 9:05p Psasse
// changed the content manager to a member of the app
// 
// 22    4/23/98 9:12p Psasse
// 
// 21    4/23/98 4:01p Rlovejoy
// Added KillFirstPoster() prototype.
// 
// 20    4/20/98 2:18p Fredf
// Migrated 4.0 About Box and Startup dialog changes.
// 
// 19    4/17/98 4:47p Fredf
// T-Shirts
// 
// 18    4/14/98 3:13p Dennis
// Added American Greetings ReplaceMent text String resources and Upgrade
// Dialog.
// 
// 17    4/10/98 4:56p Rlovejoy
// Changes for new hub implementation.
// 
// 16    3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 15    3/10/98 5:24p Jayn
// Master page changes
// 
// 14    2/04/98 1:03p Dennis
// Added CConnectionManager object
// 
// 13    1/19/98 1:21p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 12    1/13/98 11:05a Hforman
// add BorderEditorInitialized()
// 
// 11    1/08/98 2:14p Hforman
// add Border Editor stuff
// 
// 10    12/23/97 10:27a Jayn
// 
// 9     12/16/97 5:43p Hforman
// more helpful hint stuff
// 
// 8     12/12/97 5:17p Hforman
// Helpful Hints stuff
// 
// 7     12/05/97 5:15p Hforman
// more helpful hint stuff
// 
// 6     12/04/97 12:45p Hforman
// add helpful hint stuff
// 
// 5     12/01/97 1:27p Jstoner
// created sticker project type
// 
// 4     11/21/97 4:37p Hforman
// add Text Box Outline showing/hiding
// 
// 3     11/12/97 6:39p Hforman
// add guide stuff
 * 
 * 2     10/31/97 3:55p Jstoner
 * multi page calendar fixes
// 
//    Rev 1.3   19 Sep 1997 17:32:56   johno
// Changes for new Post Card project type
// (preliminary)
// 
// 
//    Rev 1.2   25 Aug 1997 10:31:16   johno
// Use "path of inspiration" stuff now from PmwDoc
// 
//    Rev 1.1   22 Aug 1997 12:16:44   johno
// Changes for new 5.0 hub buttons
// 
// 
//    Rev 1.0   14 Aug 1997 15:23:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:12   Fred
// Initial revision.
// 
//    Rev 1.42   30 Jul 1997 12:51:58   johno
// Removed DoAbout (didn't seem to be being used)
// 
//    Rev 1.41   24 Jul 1997 16:43:32   Jay
// Enforces max document limit in 16-bit
// 
//    Rev 1.40   15 Jul 1997 15:57:26   Fred
// Tools are controlled by ShowTools=1
// 
//    Rev 1.39   14 Jul 1997 11:18:28   Fred
// Added framework for 'Find projects with graphics' tool
// 
//    Rev 1.38   08 Jul 1997 17:27:16   Jay
// Support for Transfer directory
// 
//    Rev 1.37   01 Jul 1997 10:52:38   Jay
// New MRU stuff
// 
//    Rev 1.36   26 Jun 1997 17:11:50   Jay
// Support for embed_source
// 
//    Rev 1.35   17 Jun 1997 11:33:24   Jay
// Template stuff; moved CollectionManager.Open()
// 
//    Rev 1.34   10 Jun 1997 16:19:02   johno
// Add on control moved to configuration class
// 
//    Rev 1.33   06 Jun 1997 11:51:28   Jay
// Licensing changes
// 
//    Rev 1.32   05 Jun 1997 18:09:12   johno
// More multiple hub stuff
// 
// 
//    Rev 1.31   05 Jun 1997 14:36:40   johno
// Changes for multiple hub support
// 
// 
//    Rev 1.30   30 May 1997 09:45:42   Jay
// Added OpenHiddenDocument() & NewHiddenDocument()
// 
//    Rev 1.29   27 May 1997 13:14:04   johno
// m_MciWnd is now a CSoundFX,
// which is derived from CMciWnd
// 
// 
//    Rev 1.28   22 May 1997 12:48:30   johno
// Added boolean (m_fSoundEffectsState),
// initialize from USER.INI
// 
//    Rev 1.27   19 May 1997 20:32:50   johno
// Content licensing added to project
// 
// 
//    Rev 1.26   07 May 1997 16:44:32   johno
// Added csNewPictureFileName and csNewPictureFriendlyName CStrings.
// If these are not empty, the Brand New path will use them to insert
// a picture into a new project.
// The art gallery browser can fill them if invoked from the hub.
// 
//    Rev 1.25   07 May 1997 12:10:52   Jay
// User directory and collections
// 
//    Rev 1.24   29 Apr 1997 17:02:16   johno
// Added CMciWnd object
// 
// 
//    Rev 1.23   18 Apr 1997 13:40:24   Jay
// GetOpenDocumentCount()
// 
//    Rev 1.22   04 Apr 1997 15:35:38   Jay
// Half-fold card and web projects
// 
//    Rev 1.21   31 Mar 1997 10:38:20   Fred
// Smacker/movie support
// 
//    Rev 1.20   28 Mar 1997 17:11:18   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.19   14 Mar 1997 11:02:52   Fred
// Export Project Text
// 
//    Rev 1.18   14 Mar 1997 10:30:00   Fred
// Export project text
// 
//    Rev 1.17   10 Mar 1997 17:09:02   johno
// Moved HUB control to Mainfrm.
// 
// 
//    Rev 1.16   07 Mar 1997 17:39:16   johno
// Initial HUB in PM
// 
// 
//    Rev 1.15   19 Feb 1997 16:53:26   Jay
// Snapping
// 
//    Rev 1.14   06 Feb 1997 09:13:26   Jay
// Rulers!
// 
//    Rev 1.13   09 Jan 1997 16:21:08   dennisp
// Changes for making workspace not shared.
// 
//    Rev 1.12   05 Nov 1996 17:31:22   Jay
// New network stuff
// 
//    Rev 1.11   05 Nov 1996 14:32:32   johno
// Got rid of GetResourceString function
// 
//    Rev 1.10   04 Nov 1996 11:27:20   johno
// Added resource string class
// 
//    Rev 1.9   28 Oct 1996 15:12:48   Jay
//  
// 
//    Rev 1.8   28 Oct 1996 14:04:10   Jay
// Moved changes over from 3.01.
// 
//    Rev 1.8   24 Oct 1996 02:56:16   Fred
// Added Product Number to About Box
// 
//    Rev 1.7   16 Sep 1996 12:38:54   Fred
// Profile stuff can be disabled
// 
//    Rev 1.6   13 Sep 1996 09:53:46   Fred
// Profile Stuff -- will be removed
// 
//    Rev 1.5   23 Jul 1996 10:40:44   Jay
// Added newsletters and brochures; removed backgrounds
// 
//    Rev 1.4   16 Jul 1996 08:53:10   Fred
// Hooked up address/sender book
// 
//    Rev 1.3   10 Jun 1996 08:23:06   Fred
// Added InitPrintMasterIniFile
// 
//    Rev 1.2   07 May 1996 12:44:28   Jay
// From PMW2
// 
//    Rev 2.33   26 Apr 1996 14:23:42   JAY
// New 'scratch' template for internal use
// 
//    Rev 2.32   21 Mar 1996 09:38:46   FRED
// Removed reliance on build.id, uses MSREG.INI instead
// 
//    Rev 2.31   05 Feb 1996 08:07:18   JAY
// Bonus Pack/Classic support
// 
//    Rev 2.30   29 Jan 1996 16:53:30   JAY
// Variable configuration in INSTALL and PMW.
// 
//    Rev 2.29   18 Jan 1996 17:09:50   JAY
// New project configuration class
// 
//    Rev 2.28   08 Jan 1996 09:54:50   JAY
// Added IsIdleMessage() function for WIN32.
// 
//    Rev 2.27   09 Nov 1995 12:27:48   JAY
// Enabled startup DDE handling for 16-bit version
// 
//    Rev 2.26   18 Oct 1995 09:58:40   JAY
// Support for DDE at PMW startup time.
// 
//    Rev 2.25   12 Oct 1995 11:28:42   JAY
// Now registers ICONS with Windows for project types.
// 
//    Rev 2.24   06 Oct 1995 10:09:18   JAY
// New printer code.
// 
//    Rev 2.23   27 Sep 1995 15:27:26   JAY
// New OLE code
// 
//    Rev 2.22   30 Aug 1995 11:39:42   FRED
// Test print
// 
//    Rev 2.21   28 Aug 1995 20:32:38   FRED
// Print alignment
// 
//    Rev 2.20   27 Aug 1995 18:28:58   FRED
// Changes to make install program not reference OLE
// 
//    Rev 2.19   24 Aug 1995 14:08:14   JAY
// Removed unused code.
// 
//    Rev 2.18   09 Aug 1995 13:12:06   JAY
// Misc changes.
// 
//    Rev 2.17   04 Aug 1995 10:47:04   FRED
// Sender fields.
// 
//    Rev 2.16   01 Aug 1995 13:01:24   JAY
// Added CALENDAR_INFO to NEWINFO.
// 
//    Rev 2.15   31 Jul 1995 10:26:46   JAY
// New projects.
// 
//    Rev 2.14   28 Jul 1995 12:31:06   JAY
// Printer info. Envelope printing.
// 
//    Rev 2.13   20 Jul 1995 12:51:18   JAY
// Label project. New paper info stuff. etc
// 
//    Rev 2.12   16 Jul 1995 18:55:28   FRED
// Album Enhancements
// 
//    Rev 2.11   14 Jul 1995 17:23:58   FRED
// Disk open/save from album
// 
//    Rev 2.10   12 Jul 1995 17:44:56   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 2.9   06 Jul 1995 09:36:22   JAY
// Moved window position code to App
// 
//    Rev 2.8   03 Jul 1995 17:33:38   FRED
// Make OK buttons work on Finish pages
// 
//    Rev 2.7   30 Jun 1995 16:35:08   JAY
// New PaperInfo and EditDesktop
// 
//    Rev 2.6   21 Jun 1995 15:48:46   JAY
// Added ChooseAProject(). Fixed stuff using localtime() to check for NULL return.
// 
//    Rev 2.5   16 Jun 1995 13:00:26   JAY
//  
// 
//    Rev 2.4   07 Jun 1995 15:39:52   JAY
//  
// 
//    Rev 2.3   19 Apr 1995 12:32:42   JAY
//  
// 
//    Rev 2.2   20 Mar 1995 16:32:48   JAY
// Added a template server (beginning of OLE changes).
// 
//    Rev 2.1   08 Feb 1995 13:33:40   JAY
// Reverted. New series.
// 
//    Rev 1.36   02 Feb 1995 09:10:04   JAY
// 
//    Rev 1.35   02 Feb 1995 09:07:20   JAY
// Got rid of MC_VERSION stuff.
// 
//    Rev 1.34   30 Jan 1995 09:34:18   JAY
// Cut and paste
// 
//    Rev 1.30   30 Nov 1994 17:02:00   JAY
//  
// 
//    Rev 1.29   17 Nov 1994 11:15:30   JAY
// Back to diskette based version (with sleeper).
// 
//    Rev 1.28   06 Oct 1994 15:19:52   JAY
// Added m_margin_style.
// 
//    Rev 1.27   06 Oct 1994 13:50:40   JAY
// Added VCS stuff.
// Added page margins member to CPmwApp.
*/

#ifndef __PMW_H__
#define __PMW_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "error.h"			// error codes
#include "instdat.h"			// installation data

#include "profile.h"
#include "soundfx.h"
#include "strclass.h"

// Tools (must have ShowTools=1 in [Debug])
#include "projtext.h"
#include "projgrph.h"

#include "iuserset.h"	// IUserSettings definitions
#include "idrvmgr.h"		// IDriveManager definitions
#include "ipathmgr.h"	// IPathManager definitions
#include "icollmgr.h"	// ICollectionManager definitions
#include "icontsrv.h"	// IContentServer definitions
#include "imedntfy.h"	// IMediaNotify definitions
#include "icontmgr.h"	// IContentManager definitions
#include "iconnmgr.h"	// IConnectionManager definitions

#include "incompdb.h"

// forward declarations
class CPaperInfo;
class CPrinterInfo;
class CIniFile;
class CPmwDoc;
class CPMWRecentFileList;
class CHelpfulHint;
class CBorderEditor;
class CBEInterface;
class CConnectionManager;
class CContentManager;
class CResourceManager;
struct CALENDAR_INFO;
class CDisplayMode;

class CPMUserSettings;
class CStdDriveManager;
class CStdPathManager;
class CPMCollectionManager;
#ifndef	ARTSTORE
class CStdContentServer;
#endif
class CSearchArtCollections;

#define VALID_REGCODE		1351

#define CD_VERSION				// CD version
#define WINLOGO					// Logo'd version

typedef struct
{
	int type;								/* Entry and exit */
	int subtype;							/* Entry and exit */
	int orientation;						/* Entry and exit */
	BOOL fDoubleSided;					/* Entry and exit */
	CDocTemplate* pTemplate;			/* Set on exit only. */
	CPaperInfo* m_pPaperInfo;			/* Paper info to use for creation. */
	CALENDAR_INFO* m_pCalendarInfo;	/* Calendar information. */
	short m_NumberOfCalendars;   // how many calendars to create
	short m_PictureStyle;		// is picture style of calendar CAL_PICTURE_STYLE
} NEWINFO, far *NEWINFO_PTR, far &NEWINFO_REF;

/*
// Paper types.
*/
typedef enum
{
	PAPER_TYPE_CUT_SHEET,
	PAPER_TYPE_CONTINUOUS
} PAPER_TYPE;


////////
//	Date/time format user preference. The order here corresponds to  
//  that of the drop-down list in the preferences dialog.    
enum EPDateFormat
{
    DT_UNKNOWN = -1,        // @emem format unspecified
	DT_FIRSTENTRY = 0,		// @emem first valid enum used to count enum
    DT_SYSTEM_DEFAULT = 0,  // @emem use the format of the Windows system
    DT_MM_DD_YY,            // @emem 03/18/96 (slash separator)
    DT_DD_MM_YY,            // @emem 18/03/96 (slash separator)
    DT_MONTH_DAY_YEAR,      // @emem March 18, 1996 (note comma)
    DT_DAY_MONTH_YEAR,      // @emem 18 March 1996 (note no comma)
    DT_YEAR,                // @emem 1996
    DT_MONTH,               // @emem March
    DT_WEEKDAY,             // @emem Monday
	DT_LASTENTRY			// @emem always the last enum
};

// User message to wake the app.
#define WM_WAKEAPP	(WM_USER+0x71B5)		// 'W'(0x17), 'A'(0x01), 'K'(0x0b), 'E' (0x05)

class CDisplayMode
{
public:
	CDisplayMode();
	~CDisplayMode();

	BOOL NeedToChangeMode();
	BOOL ChangeMode();
	BOOL RestoreMode();
protected:

	DEVMODE m_newDevMode;
	DEVMODE m_oldDevMode;
	int m_nCurXRes;
	int m_nCurYRes;
	BOOL bInited;

};



/////////////////////////////////////////////////////////////////////////////
// CPmwApp:
// See pmw.cpp for the implementation of this class
//
class CPmwApp : public CWinApp
{
public:
	CDocTemplate *m_pScratchTemplate;		// Like poster, but not.

	CDocTemplate *m_pCardTemplate;
	CDocTemplate *m_pBannerTemplate;
	CDocTemplate *m_pPosterTemplate;
	CDocTemplate *m_pCalendarTemplate;
	CDocTemplate *m_pLabelTemplate;
	CDocTemplate *m_pEnvelopeTemplate;
	CDocTemplate *m_pBizCardTemplate;
	CDocTemplate *m_pCertificateTemplate;
	CDocTemplate *m_pNoteCardTemplate;
	CDocTemplate *m_pFaxCoverTemplate;
	CDocTemplate *m_pStationeryTemplate;
	CDocTemplate *m_pNewsletterTemplate;
	CDocTemplate *m_pBrochureTemplate;
	CDocTemplate *m_pHalfCardTemplate;
	CDocTemplate *m_pWebPubTemplate;
	CDocTemplate *m_pPostCardTemplate;
	CDocTemplate *m_pStickerTemplate;
	CDocTemplate *m_pTShirtTemplate;
	CDocTemplate *m_pCraftTemplate;
	CDocTemplate *m_pPhotoProjectsTemplate;
CDocTemplate *m_pBlankTemplate;

	enum { MaxDocuments = 3 };		// For 16-bit.
private:

// InitInstance support routines (to keep it clean).

	BOOL CheckPreviousInstance(void);
	BOOL OpenFileSystem(void);
	void Init3DControls(void);
	void ParseCommandLine(void);
	BOOL ReadInstDat(void);
	void NidFilenames(void);
	void ReadDebugSection(void);
	BOOL InitFileSystem(LPCSTR pszUserDirectory);
	//void OpenSoundManager(void);
	void ReadConfigurationSection(void);
	void ReadImageSection(void);
	void ReadBarsSection(void);
	void ReadPrinterSection(void);
	void ReadDesktopSection(void);
	void RegisterMyControls(void);
	void InitializeWorkspace(void);
	void InitializePictureSearchPaths(void);
	BOOL InitializeFonts(void);
	BOOL CreateMainWindow(void);
	void RemindUser(void);
//	void DoAbout(void);
	BOOL OpenBDE(void);
	void RegisterDocTemplates(void);

	// New MRU helpers.
	void LoadStdProfileSettings(void);		// Our version.
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
#ifndef WIN32
	// 32-bit MFC has this (as a CRecentFileList*). We add it for 16-bit.
	CPMWRecentFileList* m_pRecentFileList;
#endif

// Doc template support routines.

	void RegisterDocTemplate(CDocTemplate*& pTemplate,
									 UINT nIDResource,
									 CRuntimeClass* pDocClass,
									 CRuntimeClass* pFrameClass,
									 CRuntimeClass* pViewClass,
									 const CLSID& clsid,
									 int nIconIndex);
public:
	// Get the user settings. No refs added.
	IUserSettings* GetUserSettings(void);

	// Get the drive manager. No refs added.
	IDriveManager* GetDriveManager(void);

	// Get the path manager. No refs added.
	IPathManager* GetPathManager(void);

	// Get the collection manager. No refs added.
	ICollectionManager* GetCollectionManager(void);

	// Get the content server. No refs added.
	IContentServer* GetContentServer(void);

	// Get the media prompt notify object. No refs added.
	IMediaPromptNotify* GetMediaPromptNotify(void);

	// Get the content manager. No refs added.
	IContentManager* GetContentManager(void);

	// Get the connection manager. No refs added.
	IConnectionManager* GetConnectionManager(void);

	// Get the art search collections.
	CSearchArtCollections* GetSearchCollections(void)
		{ return m_pSearchCollections; }

	// Reopen the collection manager.
	void ReopenCollectionManager(void);

	// Update the shared paths.
	void UpdateSharedPaths(void);



protected:
	// Our interfaces.
	CPMUserSettings* m_pUserSettings;
	CStdDriveManager* m_pDriveManager;
	CStdPathManager* m_pPathManager;
	CPMCollectionManager* m_pCollectionManager;
#ifndef	ARTSTORE	
	CStdContentServer *m_pContentServer;
#endif	
	CContentManager *m_pContentManager;
	CConnectionManager* m_pConnectionManager;

	// The collections to look in for missing art.
	CSearchArtCollections* m_pSearchCollections;

	// this is for custom interface art management
	CPUIPageComponentManager m_pageManager;
	CPUIImageComponentManagerIndexedDB m_imageManager;

	LPCSTR m_signature_file;
	LPCSTR m_volume_id;

	CDisplayMode m_displayChanger;

	CString m_csVersion;
	int m_nBuildId;

   CResourceManager  *m_pcrmAppSpecificResources;

	CString m_csParentPartNumber;
	BOOL m_fHintFullLengthBmp;
	CString m_csAddressBook;
	CString m_csSenderBook;
	void WriteAddressBook(void);		// Update m_csAddressBook to INI file.

	// The list of tmp files (generated by drag/drop/copy to clipboard).
	CStringArray m_csaTmpFileNames;

/*
// Workspace names.
*/

	CString m_csWorkspaceFile;
	CString m_csWorkspaceINI;
	void init_workspace_names(void);

public:
	// Get the underlying implementation object for the IConnectionManager interface.
   CConnectionManager* GetHiddenConnectionManager();
   LPCSTR get_signature_file(void)
		{ return m_signature_file; }
	LPCSTR get_volume_id(void)
		{ return m_volume_id; }
	LPCSTR GetVersion(void) const
	   { return m_csVersion; }
	int GetBuildId(void) const
		{ return m_nBuildId; }
	LPCSTR GetParentPartNumber(void) const
		{ return m_csParentPartNumber; }
	const CString& AddressBook(void) const
		{ return m_csAddressBook; }
	const CString& SenderBook(void) const
		{ return m_csSenderBook; }
	void AddressBook(const CString& csAddressBook)
		{ m_csAddressBook = csAddressBook; WriteAddressBook(); }
	void SenderBook(const CString& csSenderBook)
		{ m_csSenderBook = csSenderBook; WriteAddressBook(); }
	DWORD GetSenderId(void);
	void SetSenderId(DWORD dwSenderId);

	void ChangedConfiguration(void);

	void UserHasRegistered(void);

   CResourceManager *   GetResourceManager();
   ERRORCODE            CreateResourceManager();
   void                 DestroyResourceManager();
   HICON                GetAppIcon();

   const BOOL GetHintFullLengthBmp(void) const
   		{ return m_fHintFullLengthBmp; }

	void SetHintFullLengthBmp(BOOL fSetBmp);
	BOOL KillFirstPoster(void);
	void UpdateLicensedProducts(void);

	// Add a tmp file name here (one to be deleted).
	void AddTmpFileName(const CString& csName);

	void ChangeDisplayMode();
	void RestoreDisplayMode();

	void
	SetPreventHub(void)
	{
		m_nPreventHub ++;
	}

	BOOL
	PreventHub(void)
	{
		UINT i = m_nPreventHub;
		if (i > 0)
			m_nPreventHub --;	
		
		return true; //j
		return i == 0 ? FALSE : TRUE;
	}
protected:
	UINT	m_nPreventHub;
	BOOL m_bChangedDevMode;
public:
	InstallationInfo m_installation_info;

	BOOL m_bAgRemotePrint;
	BOOL m_bPrintProjects;//debug/ShowTools=1 only.
	BOOL m_fInitialized;

/*
// Paper size information.
// We try to keep this current.
*/

	CPrinterInfo* GetPrinterInfo(int nOrientation);
	CPaperInfo* GetPaperInfo(int nOrientation);

	int m_nMarginStyle;			/* margins style */

	// Network support.

	CFile m_cfLockFile;

	int m_nUserID;
	int m_nUserIDCount;
	CString m_csUserName;

	int m_nMaxUsers;
	int m_nUserCount;

	BOOL m_first_time_installation;

	BOOL ShowRulers(void) const   { return m_fShowRulers; }
	void SetShowRulers(BOOL fShowRulers);

	BOOL SnapToRulers(void) const { return m_fSnapToRulers; }
	void SetSnapToRulers(BOOL fSnapToRulers);

	BOOL ShowGuides(void) const   { return m_fShowGuides; }
	void SetShowGuides(BOOL fShowGuides);

	BOOL SnapToGuides(void) const { return m_fSnapToGuides; }
	void SetSnapToGuides(BOOL fSnapToGuides);

   BOOL GuidesLocked(void) const { return m_fLockGuides; }
	void SetLockGuides(BOOL fLockGuides);

   BOOL ShowTextBoxOutlines(void) const { return m_fTextBoxOutlines; }
   void SetTextBoxOutlines(BOOL fShow);

	// Helpful Hint functions

	BOOL ShowHelpfulHint(int nType, int nWhich, CWnd* pParent = NULL, BOOL fModal = FALSE);
	BOOL ShowHelpfulHint(const CString& strWhich, CWnd* pParent = NULL, BOOL fModal = FALSE);
	void RemoveHelpfulHint();
	void ShowRecentHintsDialog();
	void ResetHelpfulHints();
	BOOL HelpfulHintsEnabled();
	void EnableHelpfulHints(BOOL enable = TRUE);
	BOOL DesignTipsEnabled();
	void EnableDesignTips(BOOL enable = TRUE);
	BOOL HintsVoiceEnabled();
	void EnableHintsVoice(BOOL enable = TRUE);
	BOOL AutoInstructionsEnabled(){return m_bAutoInstructions;}
	void EnableAutoInstructions(BOOL bEnable = TRUE){m_bAutoInstructions = bEnable;}

	CBEInterface* GetBorderEditorInterface();
	BOOL BorderEditorInitialized();

	BOOL  m_fSoundEffectsState;

	// returns todays date in the string pased formatted by the eFormat passed
	BOOL GetDateFormattedString( CString *pString, EPDateFormat eFormat );

	// returns todays date formatted by the user preference in the CString passed
	BOOL GetUserDateFormattedString( CString *pString )
	{	return( GetDateFormattedString( pString, m_eDateFormat ) ); }

	// enum type of user prefered date format for date strings inserted into text
	EPDateFormat m_eDateFormat;
	
	CPUIPageComponentManager* GetPageManager() { return &m_pageManager;}
	CPUIImageComponentManagerIndexedDB* GetImageManager() { return &m_imageManager;}

protected:
	BOOL m_fShowRulers;
	BOOL m_fSnapToRulers;

   BOOL m_fShowGuides;
	BOOL m_fSnapToGuides;
   BOOL m_fLockGuides;

   BOOL m_fTextBoxOutlines;
	BOOL m_bAutoInstructions;

//   BOOL m_fFirstTimeHub;

   CHelpfulHint* m_pHelpfulHint;
	CBorderEditor* m_pBorderEditor;

	CPaperInfo* m_pPortraitPaperInfo;
	CPaperInfo* m_pLandscapePaperInfo;

	CPrinterInfo* m_pPortraitPrinterInfo;
	CPrinterInfo* m_pLandscapePrinterInfo;

	BOOL GetNetworkUserId(LPSTR lplzNetworkUserName, LPSTR szMessage);
	BOOL NetworkStartup(char *szMessage);
	BOOL initialize_network(VOID);

	BOOL InitUserDirectory(CString& csUserDirectory);
	BOOL setup_ini_file(LPCSTR pszUserDirectory);
	LPSTR GetParameter(LPSTR lpszParamName, LPSTR lpszValue);
	BOOL FindParameter(LPSTR lpszParamName);
	LPSTR LocateParameter(LPSTR lpszParamName, LPSTR lpszValue);

	CPmwDoc *m_pFirstPosterDoc;

public:
	CPmwApp();
	~CPmwApp();

// Variables
// Overrides
	virtual BOOL InitInstance(VOID);
	virtual int ExitInstance(VOID);

	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
#ifdef WIN32
	virtual BOOL IsIdleMessage(MSG* pMsg);
#endif
	virtual CDocument* OpenDocumentFile(LPCSTR lpszFileName);

	// override and pass in flag
	CDocument* OpenDocumentFile(LPCSTR lpszFileName, BOOL fAddToMRU);

	// User transfer directory.
	CString GetTransferDirectory(void);

	// Hidden document support.
	CDocTemplate* GetMatchingTemplate(LPCSTR pszFile, BOOL fExactOnly = FALSE);
	CPmwDoc* OpenHiddenDocument(LPCSTR lpszFileName, LPCSTR lpszOriginalFileName = NULL);
	CPmwDoc* NewHiddenDocument(void);

	BOOL CanOpenAnotherDocument(void);
	int GetOpenDocumentCount(void);

	// Advanced: process async DDE request
	virtual BOOL OnDDECommand(LPTSTR lpszCommand);
	CStringArray m_csaDocsToOpen;

// Implementation

	BOOL FinishInitialization(BOOL fNormalStartup = TRUE);
	BOOL ConfigurationError(void);

	COleTemplateServer m_server;
		// Server object for document creation

	//{{AFX_MSG(CPmwApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFilePrintSetup();
	afx_msg void OnUpdateFileNewOpen(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnUpdateRecentFileMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnOpenRecentFile(UINT nID);
	afx_msg BOOL OnShowHintOnBorderDlg();
	DECLARE_MESSAGE_MAP()

public:						// After DECLARE_MESSAGE_MAP...
	NEWINFO last_new_info;

	CString m_csLastDocument;
	CString m_csMainIni;

	BOOL want_help_messages;

	BOOL new_document(int type = 0);
	BOOL open_a_document(int type = 0);
	BOOL ChooseADocument(CString& csFileName, int nProjectType = 0);

	void WakeWindow(CWnd* pWnd = NULL);

	void get_workspace_names(CString *pcsWorkspaceFile, CString *pcsIniName);
	DWORD get_workspace_size(void);

	HGLOBAL get_devnames(BOOL fUpdate = TRUE);
	HGLOBAL get_devmode(BOOL fUpdate = TRUE);

	void set_dev_handles(HGLOBAL hDevMode, HGLOBAL hDevNames);

	void update_paper_info(HDC hDC = NULL);

	void nid_filename(LPSTR name);

	void reset_doc_fonts(VOID);

	BOOL user_is_registered(VOID);

	BOOL HasNGImages(void);
	BOOL HasAGContent(void);
	void GetCopyrightString(CString& csCopyright);

   CSoundFX m_MciWnd;

	BOOL TimeForReminder(void);

	void RevertToSavedFile(LPCSTR strFilePath);
	// Picture/background name management.

private:
	CString				m_csLastPictureChosen;
	HINSTANCE         m_InstDLL;
   CResourceString   m_ResourceString;
   BOOL				m_bLargeFontMode;

	// The paths where we look for missing graphics.
	CStringArray m_csaPictureSearchPaths;

public:
	BOOL LargeFontMode(){return m_bLargeFontMode;}
	LPCSTR get_last_picture_name() const
		{ return m_csLastPictureChosen; }
	VOID set_last_picture_name(LPCSTR name);
	VOID free_last_picture_name(VOID);

	void register_free_files(void);

	CStringArray *GetPictureSearchPaths(void)
		{ return &m_csaPictureSearchPaths; }

	// Update the picture search paths to (possibly) include new information
	// from a selected picture.
	void UpdatePicturePaths(LPCSTR pszNewName);

/*
// Get a profile string, looking in both the nid'd INI file and PMW.INI
*/
	CString OurGetProfileString(LPCSTR lpszSection, LPCSTR lpszEntry,
				LPCSTR lpszDefault = NULL);

/*
// Get a profile int, looking in both the nid'd INI file and PMW.INI
*/

	UINT OurGetProfileInt(LPCSTR lpszSection, LPCSTR lpszEntry, UINT nDefault);

/*
// Get a profile color, looking in both the nid'd INI file and PMW.INI
*/

	COLORREF OurGetProfileColor(LPCSTR lpszSection, LPCSTR lpszEntry, COLORREF clrDefault);

/*
// Load or save a window's position.
*/

	BOOL LoadWindowPosition(CWnd* pWnd, LPCSTR pszSection, int nCmdShow = SW_SHOWNORMAL, BOOL fNoResize = FALSE, BOOL fSkipIfInvalid = TRUE);
	void SaveWindowPosition(CWnd* pWnd, LPCSTR pszSection);

/*
// Initialize a CIniFile object for an INI file in the PrintMaster directory
// with the given name. This stuff is put in PMW to keep CIniFile free of PrintMaster
// dependancies.
*/

	BOOL InitPrintMasterIniFile(CIniFile* pIniFile, LPCSTR pszName, BOOL fNidIt = FALSE);

/*
// Profile stuff.
*/

	CProfile m_Profile;
	void ProfileAdd(LPCSTR pszLabel)
		{ m_Profile.Add(pszLabel); }
	void ProfileAdd(LPCSTR pszLabel, DWORD dwValue)
		{ m_Profile.Add(pszLabel, dwValue); }
	void ProfileShow(void)
		{ m_Profile.Show(); }
	void ProfileClear(void)
		{ m_Profile.Clear(); }
	void Profiling(BOOL fProfiling = TRUE)
		{ m_Profile.Profiling(fProfiling); }
	BOOL IsProfiling(void)
		{ return m_Profile.IsProfiling(); }
   
/*
// Extract project text tool.
// (must have ShowTools=1 in [Debug])
*/

	CExtractProjectText m_ExtractProjectText;
	void ExtractProjectText(void)
		{ m_ExtractProjectText.Run(); }

/*
// Find projects with graphics tool.
// (must have ShowTools=1 in [Debug])
*/

	CFindProjectsWithGraphics m_FindProjectsWithGraphics;
	void FindProjectsWithGraphics(void)
		{ m_FindProjectsWithGraphics.Run(); }

	// Silly little flag to avoid fighting over the palette
	// while playing a movie.
	//
	// PMGTODO: Investigate whether the movie player needs to respond to
	// palette messages.
	//
	// PMGTODO: If we ever add more video to PMG, then we'll need a better
	// mechanism.

	BOOL m_fProcessPaletteMessages;
	void SetProcessPaletteMessages(BOOL fProcessPaletteMessages = TRUE)
		{ m_fProcessPaletteMessages = fProcessPaletteMessages; }
	BOOL GetProcessPaletteMessages(void)
		{ return m_fProcessPaletteMessages; }

	// Return a pointer to a resource string.
	// The return type is LPCSTR so that the data can not be modified
	// through the returned pointer.
   LPCSTR
   GetResourceStringPointer (UINT id)
   {
      return m_ResourceString.GetPointer (id);
   }

	// support for Print Shop's PSLaunchDLL module
public:
	// Run the wizard's DoFileNew function
	int RunPrintShopWizard();
private:
	// instance handle of the DLL, initially NULL
	static HINSTANCE m_hPSLaunchDLL;

//
// Interfaces.
//

protected:
	DECLARE_INTERFACE_MAP()

	// The media prompt notify interface in the app.
	// This defines the interface that is passed to others.
	// To see comments on the functions, see the IMediaPromptNotify definition
	// in IMEDNTFY.H.
	BEGIN_INTERFACE_PART(MediaPromptNotify, IMediaPromptNotify)
		INIT_INTERFACE_PART(CPmwApp, MediaPromptNotify)
		STDMETHOD_(BOOL, PromptForHomeCD)(THIS);
		STDMETHOD_(BOOL, PromptForVolume)(THIS_ int nDriveType, LPCSTR pszVolume, LPCSTR pszMediaName, LPCSTR pszFileName);
	END_INTERFACE_PART(MediaPromptNotify)

protected:

//
// Renaissance support
//

	BOOL		InitializeRenaissanceFramework( );
	void		ShutdownRenaissanceFramework( );
	void		InitializeBitmapToolbox( );
	CString	FindModulePath();

};

/////////////////////////////////////////////////////////////////////////////

#ifdef LOCALIZE
void SetNewBusinessCardSize(void);
#endif

extern void od(LPSTR lpFormat, ...);
#define printf od

extern VOID center_window_on_parent(CWnd *window, BOOL center_on_screen = FALSE);

#define GET_PMWAPP()		((CPmwApp *)AfxGetApp())

extern void FormatDouble(double dNumber, int nPrecision, CString& csOutput);

#define GetGlobalUserSettings() GET_PMWAPP()->GetUserSettings()
#define GetGlobalDriveManager() GET_PMWAPP()->GetDriveManager()
#define GetGlobalPathManager() GET_PMWAPP()->GetPathManager()
#define GetGlobalCollectionManager() GET_PMWAPP()->GetCollectionManager()
#define GetGlobalContentManager() GET_PMWAPP()->GetContentManager()
#define GetGlobalContentServer() GET_PMWAPP()->GetContentServer()
#define GetGlobalConnectionManager() GET_PMWAPP()->GetConnectionManager()

#endif			// __PMW_H__
