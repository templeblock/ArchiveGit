/*
// $Header: /PM8/Download/PATHMGR.CPP 1     3/03/99 6:15p Gbeddow $
//
// Path manager routines.
//
// $Log: /PM8/Download/PATHMGR.CPP $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 32    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 31    10/02/98 7:10p Jayn
// More content manager changes
// 
// 30    9/26/98 8:08p Psasse
// Reduced number of parameters going into GetInternetContent()
// 
// 29    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 28    9/22/98 3:32p Psasse
// converting LPCSTR'S to CString&
// 
// 27    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 26    9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 25    9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 24    9/01/98 1:39p Dennis
// Drastically improved speed of content decompression by allocating our
// own buffer.
// 
// 23    8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 22    7/24/98 7:44p Psasse
// Back to GetAppClosingDown() (set in Mainframe) - specifically to allow
// DrawPlus to stand on its own.
// 
// 21    7/17/98 1:39p Jayn
// Removed a "fix" I made.
// 
// 20    7/16/98 4:32p Psasse
// changed m_fAppClosingDown flag to AfxGetMainWnd->IsWindowVisible to
// eliminate dialogs from launching upon app closing and moved
// FlushClipboard to WM_Destroy in MainWnd 
// 
// 19    7/02/98 12:01p Johno
// Removed unused parameter from LocateContentFile()
// 
// 18    6/30/98 7:22p Psasse
// added m_fAppClosingDown flag to eliminate dialogs from launching
// 
// 17    6/29/98 12:25p Jayn
// Now locks the temp maps around the new Prompt dialogs.
// 
// 16    6/29/98 9:36a Mwilson
// added auto cd detection
// 
// 15    6/17/98 11:17a Johno
// Added check in PathManager::prompt_for_volume() for ::MessageBox()
// fail.
// If the hub is the parent, and it gets destroyed during ::MessageBox(),
// it returns 0
// 
// 14    5/15/98 8:34p Psasse
// Asynchronous progress dialog support
// 
// 13    5/11/98 9:07p Psasse
// content manager becomes a member of the app
// 
// 12    5/04/98 3:53p Jayn
// Changes for Deluxe.
// 
// 11    5/01/98 3:17p Jayn
// 
// 10    4/30/98 7:18p Psasse
// 
// 9     4/23/98 9:11p Psasse
// 
// 8     4/22/98 9:00p Psasse
// 
// 7     4/16/98 1:23p Psasse
// 
// 6     4/14/98 10:11a Dennis
// Enhanced LocateContentFile() to look for content in shared areas.
// 
// 5     4/06/98 9:08p Psasse
// 
// 4     4/05/98 6:36p Psasse
// 
// 3     4/03/98 5:21p Psasse
// 
// 2     4/02/98 9:09p Psasse
// internet cache support
// 
//    Rev 1.1   15 Sep 1997 11:55:34   johno
// Added "rest ++" in bind_removable. This lack was causing
// it to fail to correctly extract the volume lable, which
// caused it to search all removeable drives for it, for example.
// 
// 
//    Rev 1.0   14 Aug 1997 15:23:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:38   Fred
// Initial revision.
// 
//    Rev 1.18   04 Aug 1997 15:28:30   Fred
// file_exists uses local buffer for make_full_path
// 
//    Rev 1.17   26 Jun 1997 17:11:26   Jay
// FileIsRemovable
// 
//    Rev 1.16   24 Jun 1997 15:17:26   Jay
// Support for multiple file paths and CD friendly names
// 
//    Rev 1.15   17 Jun 1997 12:42:28   Jay
// Import; OpenStreams()
// 
//    Rev 1.14   11 Jun 1997 15:49:18   johno
// Add on support
// 
// 
//    Rev 1.13   07 May 1997 12:10:48   Jay
// User directory and collections
// 
//    Rev 1.12   05 May 1997 08:53:12   Jay
// Commented out a debug statement
// 
//    Rev 1.11   30 Apr 1997 16:41:46   dennisp
// Fix for retrieving comressed content
// 
//    Rev 1.10   30 Apr 1997 12:30:36   Jay
// Compressed data streams
// 
//    Rev 1.9   23 Apr 1997 12:08:10   Jay
// path manager changes for collection items
// 
//    Rev 1.8   21 Apr 1997 14:49:38   Jay
// Collection manager
// 
//    Rev 1.7   17 Apr 1997 16:06:08   Jay
// Initial collection integration
// 
//    Rev 1.6   12 Nov 1996 14:07:34   johno
// Moved strings to resource DLL
// 
//    Rev 1.5   04 Nov 1996 15:35:44   johno
// Strings now come from CResourceString
// 
//    Rev 1.4   16 Sep 1996 17:20:50   Jay
// Now gets removable media volume name from CDNAMES.INI.
// 
//    Rev 1.3   16 Sep 1996 14:10:58   Jay
// Can disable prompting for CD.
// 
//    Rev 1.2   13 Sep 1996 09:32:24   Jay
// Restored the is_app_volume call.
// 
//    Rev 1.1   09 Aug 1996 16:50:16   Fred
// More 16-bit fixes
// 
//    Rev 1.0   14 Mar 1996 13:36:46   Jay
// Initial revision.
// 
//    Rev 1.2   13 Mar 1996 14:53:28   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.1   06 Mar 1996 10:28:40   Jay
// Changes from 2.1
// 
//    Rev 2.16   27 Feb 1996 16:48:42   JAY
// New filename translation system supporting subfiles.
// 
//    Rev 2.15   18 Jan 1996 17:09:50   JAY
// New project configuration class
// 
//    Rev 2.14   26 Dec 1995 10:28:20   FRED
// Project structure changes for mac port
// 
//    Rev 2.13   20 Dec 1995 09:56:32   JAY
// 32-bit fixes
// 
//    Rev 2.12   20 Nov 1995 13:01:10   JAY
// 
//    Rev 2.11   17 Oct 1995 15:42:30   JAY
// Fixes
// 
//    Rev 2.10   13 Sep 1995 11:48:32   JAY
// New 32-bit changes
// 
//    Rev 2.9   08 Aug 1995 19:03:10   FRED
//  
// 
//    Rev 2.8   17 Jul 1995 08:35:32   JAY
// Made it encode the home CD first.
// This helps for our picture and project album builds. I don't know it if is
// better the original way or not (I may change it back later).
// 
//    Rev 2.7   07 Jul 1995 16:13:16   JAY
// Include rearranging; document saving with album.
// 
//    Rev 2.6   06 Jul 1995 09:37:42   JAY
// Now always sets the path to SOMETHING.
// 
//    Rev 2.5   03 Jul 1995 16:25:50   JAY
// Now uses PMW2.
// 
//    Rev 2.4   05 Jun 1995 11:33:00   JAY
//  
// 
//    Rev 2.3   19 Apr 1995 12:32:36   JAY
//  
// 
//    Rev 2.2   15 Feb 1995 17:16:18   JAY
// Undo
// 
//    Rev 2.1   08 Feb 1995 13:38:02   JAY
// Reverted. New series.
// 
//    Rev 1.13   04 Oct 1994 14:21:26   JAY
// A cd is only a home cd if is has the app id file.
// 
//    Rev 1.12   16 Sep 1994 14:16:22   JAY
// Changed some ifdefs.
// 
//    Rev 1.11   10 Aug 1994 15:05:10   JAY
// ...
// 
//    Rev 1.11   10 Aug 1994 15:05:00   JAY
// ...
// 
//    Rev 1.10   04 Aug 1994 14:25:48   JAY
// Fixed a typo.
// 
//    Rev 1.9   04 Aug 1994 07:34:52   JAY
// Large model and 32-bit changes
// 
//    Rev 1.8   18 Jul 1994 10:09:04   JAY
// Added must_needs_home_cd().
// Now uses access_file() instead of access().
// 
//    Rev 1.7   21 Jun 1994 13:31:16   JAY
// Removed some debugging messages.
// 
//    Rev 1.6   14 Jun 1994 17:06:42   JAY
// Support for ExtendedPath drive types.
// Home CD data path token is now fully supported.
// 
//    Rev 1.5   10 Jun 1994 10:09:22   JAY
// Added flag pointer argument to bind_to_virtual_file().
// 
//    Rev 1.4   01 Jun 1994 16:48:32   JAY
// Added parameter to file_exists().
// Now supports binding within the actual file name component (not just virtual or
// dynamic components).
// Fixed bug in bind_to_virtual_file().
// 
//    Rev 1.3   31 May 1994 12:45:56   JAY
// Added path_binding_type().
// Fixed code which looks for drives while binding.
// 
//    Rev 1.2   26 May 1994 13:51:36   JAY
// Moved some code out of here to DriveManager.
// More code for prompting for missing disks.
// 
//    Rev 1.1   23 May 1994 09:15:04   JAY
// Moved paths here from CPmwApp.
// Now uses composite filenames.
// Other changes!
// 
//    Rev 1.0   12 May 1994 07:38:36   JAY
// Initial revision.
// 
//    Rev 1.0   06 May 1994 17:11:30   JAY
// Initial revision.
*/

/*
// Note: the following block comments are out-of-date and do not reflect the current
// state of the code. However, they have been left in for historical purposes.
// In particular, when attempting to locate media, the user is *not* given a file
// dialog to use for locating the requested item. The program just looks everywhere
// it can.
*/

/*
// Global thoughts on CD setups:
// The user has one of the following configurations, with 1 being most common:
//
// 1. A single CD drive on the local machine (MSCDEX-identifiable).
// 2. Multiple CD drives on the local machine (MSCDEX-identifiable).
// 3. No identifiable CD drives on the local machine.
//
// Since 1 is the most common case, we should gear the program for that first.
// If we can identify the drive, and there is only one, we should just use it.
*/

/*
// Purpose:
//
// The purpose of these routines is to locate the main application CD.
// If the CD itself is not available, we at least want to locate the CD-ROM
// drive so we can set the CD path.
//
// Notes:
//
// - I want the CD to be identifiable by a path (not just a drive letter).
// A CD which is a drive (e.g. the D drive) will be stored as the root
// directory of that drive (e.g. "D:\").
// - Searching for the app CD involves looking for a specific file on the CD.
// - Searching for the app volume involves reading the drive's volume label.
//
// Possible dialogs the user can see:
//
// (D1) "The CD-ROM drive can not be found. Please specify the path."
//		Modified file dialog.
// (D2) "The 'app' CD is not in drive 'cdpath'. If the CD is not found, some
// features of the program may be unavailable."
//		Try again, Continue, Change Drive (D1)
//
// Specific cases and what I want to happen in each case:
// (1) Case: The app CD is in the CD-ROM drive, and the path is correctly
//     specified by either the local or global INI file CDPath entry.
//     Result: The application continues without user intervention. This is
//     hopefully the normal case.
//
// (2) Case: The app CD is in the CD-ROM drive, but the drive letter has
//     changed (or the CD is in another CD-ROM drive).
//     Result: The application will find its CD in the new drive.
//
// (3) Case: The user's CDPath is specified correctly (there is an identifiable
//     CD-ROM drive there), but the app CD is not in the drive.
//     Result: Dialog D1 is shown with the CDPath drive as the CD drive.
//
// (4) Case: The user's CDPath is incorrect, there is one identifiable CD-ROM
//     drive, but the CD is not in that drive.
//     Result: Dialog D1 is shown with the identified drive as the CD drive.
//
// General order of actions:
//
// (1) Identify all the drives in the system. It is assumed that drive
// type does not change dynamically. Specifically, if MSCDEX is installed,
// determine all the CD drives.
//
// (2) Look for the app CD or volume
//		(a) In the local CDPath place.
//		(b) In the the global CDPath place.
//		(c) In any CD-ROM drive.
//		(d) In any non-CD-ROM (remote) drive.
//
// (3) If a single CD-ROM drive is found, or a CD-ROM drive is specified
// by either the local or global INI file settings, then inform the user that
// the app CD is not in the drive. This allows the user to place the CD back
// in the drive (Retry), continue without identifying the CD (Cancel), or
// change the location of the CD-ROM drive (Change Drive).
//
// Order of execution:
//
// (1) Check the local _???PMW.INI file setting for CDPath for the app CD.
// (2) Check the global PMW.INI file setting for CDPath for the app CD.
// (3) Look for the app volume in any CD drive.
// (4) Look for the app volume in any non-CD drive.
// (5) Prompt the user to identify the CD drive (path). The user will be
//     able to save this as the default local CD path for future sessions.
*/

#include "stdafx.h"
#include "pathmgr.h"

#include "file.h"
#include "compfn.h"			// We need to be able to use CompositeFileName's.
#include "util.h"
#include "memdev.h"
#include "cdeflate.h"
#include "pmwcoll.h"

static char BASED_CODE SECTION_Paths[] = "Paths";
static char BASED_CODE ENTRY_ProjectPath[] = "ProjectPath";
static char BASED_CODE ENTRY_ArtPath[] = "ArtPath";
static char BASED_CODE ENTRY_ContentPath[] = "ContentPath";
static char BASED_CODE ENTRY_SharedPath[] = "SharedPath";
static char BASED_CODE ENTRY_AddressBookPath[] = "AddressBookPathV3";

static char BASED_CODE SECTION_Configuration[] = "Configuration";
static char BASED_CODE ENTRY_CDPath[] = "CDPath";

static char missing_volume_string[] = "*Unknown*";

/////////////////////////////////////////////////////////////////////////////
// CCollectionCompletionData

class CCollectionCompletionData
{
public:
	CPMWCollection*   m_pCollection;
	CFileContent*     m_pContent;

	CContentDirEntry  m_Dir;
	CContentDataEntry m_Data;
	MemoryDevice      m_Device;
	LPBYTE            m_pUncompressedData;
   BYTE              m_bOwnUncompressedData;
};

//////////////////////////////////////////////////////////////////////////
// CStdPathManager routines.

/*
// Constructor.
*/

CStdPathManager::CStdPathManager()
{
	m_fPromptForContent = TRUE;
	m_pDriveManager = NULL;
	m_pCollectionManager = NULL;
	m_pContentManager = NULL;
	m_pMediaPromptNotify = NULL;
}

/*
// Destructor.
*/

CStdPathManager::~CStdPathManager()
{
	Close();
}

/*
// Return the drive for a path.
//
// A return of 0 indicates no identifiable drive.
*/

static inline char PathDrive(LPCSTR path)
{
	if (path == NULL || path[0] == '\0' || path[1] != ':')
	{
		return 0;
	}
	return toupper(path[0]);
}

/*
// Map an extended drive to a normal drive. It returns 0 if failure or if
// the drive is not a good drive.
// An extended drive works if the INI path for it is a simple root path
// (like "x:\"). In that case, we just extract the "x", which must be a
// valid logical drive.
*/

DRIVE_LETTER CStdPathManager::MapExtendedHomeDrive(EXTENDED_DRIVE_LETTER drive)
{
/*
// Is this a legal drive?
*/

	ASSERT(m_pDriveManager != NULL);

	DriveType drive_type = m_pDriveManager->GetDriveType(drive);

	if (drive_type == DT_Invalid
			|| drive_type == DT_Unknown
			|| drive_type == DT_ExtendedPath)
	{
		return 0;
	}

/* Get the drive path. */

	LPCSTR path;
	if ((path = HomeDrivePath(drive)) == NULL)
	{
	/* Illegal drive. */
		return 0;
	}

/* Return the drive for this path. */

	return PathDrive(path);
}

/*
// Set the drive type for an extended drive.
*/

DriveType CStdPathManager::SetExtendedDriveType(LPCSTR path, EXTENDED_DRIVE_LETTER edrive)
{
	ASSERT(m_pDriveManager != NULL);
/*
// See if we have a path. If not, it's a bad path.
// See if there is a drive letter on the front. If so, check it.
// Otherwise, it's relative, so check the drive of the working directory.
*/

	if (path[0] == '\0')
	{
		return m_pDriveManager->SetDriveType(edrive, DT_Unknown);
	}

/*
// If this is not a root directory of a drive, we will mark it as extended.
*/

	if (strlen(path) > 3
			|| path[1] != ':'
			|| path[2] != '\\')
	{
	/*
	// The drive type is of the Extended variety.
	// All we can do on these is look for the ID file.
	*/
		return m_pDriveManager->SetDriveType(edrive, DT_ExtendedPath);
	}

/*
// The path references the root directory of a drive.
// Tell the drive manager to discover the type for us of that drive.
// It will set our type to that as an alias.
*/

	return m_pDriveManager->GetDriveType(path[0], edrive);
}

/*
// Return the path for this drive.
*/

LPCSTR CStdPathManager::HomeDrivePath(char cDrive)
{
	ASSERT(m_pDriveManager != NULL);
	DriveType type = m_pDriveManager->GetDriveType(cDrive);

	// Make sure we have a good type.

	if (type == DT_Invalid || type == DT_Unknown)
	{
		return NULL;
	}

	switch (cDrive)
	{
		case IDriveManager::local_cd_drive_letter:
		{
			return m_csLocalCDPath;
		}
		case IDriveManager::global_cd_drive_letter:
		{
			return m_csGlobalCDPath;
		}
		default:
		{
		/*
		// Get the root for the home CD. We then plug in our drive and
		// away we go.
		*/
			static char cbRoot[32];
			ASSERT(m_csHomeCDRoot.GetLength() < sizeof(cbRoot));
			strcpy(cbRoot, m_csHomeCDRoot);
			cbRoot[0] = cDrive;
			return cbRoot;
		}
	}
}

/*
// Is this the app volume?
//
// Incoming drive is 0 for A, 1 for B, 2 for C, etc.
*/

BOOL CStdPathManager::IsAppVolume(char drive)
{
/*
// Get the volume label for the drive.
*/

#if 1
	if ((drive = MapExtendedHomeDrive(drive)) != 0)
	{
		LPCSTR volume;

		if ((volume = m_pDriveManager->GetDriveVolumeLabel(drive)) != NULL)
		{
			return strcmp(volume, m_csVolumeID) == 0;
		}
	}
#endif

	return FALSE;
}

/*
// Does this drive have the app id file?
//
// Incoming drive is 0 for A, 1 for B, 2 for C, etc.
*/

BOOL CStdPathManager::HasAppIdFile(char cDrive)
{
	LPCSTR pszPath;

	// Get the path for this drive. Is this a legal drive?
	if ((pszPath = HomeDrivePath(cDrive)) == NULL)
	{
		// Illegal drive.
		return FALSE;
	}

	//
	// Build the name of the signature file at the specified path.
	//

	CString csPath = pszPath;
	Util::AppendBackslashToPath(csPath);
	csPath += m_csSignatureFile;

	// See if the file is there.
	UINT uPrevSem = ::SetErrorMode(SEM_FAILCRITICALERRORS);
	UINT uRet = AccessFile(csPath);
	::SetErrorMode(uPrevSem);

	return uRet == 0;
}

/*
// Locate the application data.
*/

BOOL CStdPathManager::FindAppData(char cDrive)
{
/*
// Check first for the app volume, then for the ID file.
// This should hopefully cause the CD's cache to be updated so that the
// file check will work.
//
// Note that we assume that a CD with our volume label also has the ID file,
// but we don't explicitly check.
*/
	if (IsAppVolume(cDrive) || HasAppIdFile(cDrive))
	{
		// This is the drive!
		return TRUE;
	}
	return FALSE;
}

/*
// Open the path manager.
*/

BOOL CStdPathManager::Open(void)
{
	// Setup the working directory.
	char buffer[_MAX_PATH+1];

	::GetModuleFileName(NULL, buffer, sizeof(buffer)-1);
	Util::SplitPath(buffer, &m_csWorkingDirectory, NULL);
	return TRUE;
}

/*
// Close the path manager.
*/

void CStdPathManager::Close(void)
{
	if (m_pMediaPromptNotify != NULL)
	{
		m_pMediaPromptNotify->Release();
		m_pMediaPromptNotify = NULL;
	}
	if (m_pContentManager != NULL)
	{
		m_pContentManager->Release();
		m_pContentManager = NULL;
	}
	if (m_pCollectionManager != NULL)
	{
		m_pCollectionManager->Release();
		m_pCollectionManager = NULL;
	}
	if (m_pDriveManager != NULL)
	{
		m_pDriveManager->Release();
		m_pDriveManager = NULL;
	}
}

/*
// Initialize all paths.
*/

void CStdPathManager::InitPaths(IUserSettings* pUserSettings,
										 IDriveManager* pDriveManager,
										 ICollectionManager* pCollectionManager,
										 IContentManager* pContentManager,
										 IMediaPromptNotify* pMediaPromptNotify,
										 LPCSTR pszVolumeID,
										 LPCSTR pszSignatureFile,
										 LPCSTR pszUserDirectory,
										 LPCSTR pszHomeCDRoot)
{
	ASSERT(pUserSettings != NULL);

	// Set the drive manager.
	ASSERT(pDriveManager != NULL);
	m_pDriveManager = pDriveManager;
	m_pDriveManager->AddRef();			// Add a reference for our pointer.

	// Set the collection manager.
	ASSERT(pCollectionManager != NULL);
	m_pCollectionManager = pCollectionManager;
	m_pCollectionManager->AddRef();			// Add a reference for our pointer.

	// Set the content manager.
	ASSERT(pContentManager != NULL);
	m_pContentManager = pContentManager;
	m_pContentManager->AddRef();				// Add a reference for our pointer.

	// Set the media prompt notify interface.
	if (pMediaPromptNotify != NULL)
	{
		m_pMediaPromptNotify = pMediaPromptNotify;
		m_pMediaPromptNotify->AddRef();		// Add a reference for our pointer.
	}

	if (pszVolumeID != NULL)
	{
		m_csVolumeID = pszVolumeID;
	}
	if (pszSignatureFile != NULL)
	{
		m_csSignatureFile = pszSignatureFile;
	}
	if (pszUserDirectory != NULL)
	{
		m_csUserDirectory = pszUserDirectory;
	}
	if (pszHomeCDRoot == NULL)
	{
		m_csHomeCDRoot = "@:\\";
	}
	else
	{
		m_csHomeCDRoot = pszHomeCDRoot;
	}

	if (m_csUserDirectory.IsEmpty())
	{
      m_csUserDirectory = m_csWorkingDirectory;
	}
   ASSERT(!m_csUserDirectory.IsEmpty());

/*
// Read the paths from the INI file.
// We assume the user directory has already been set.
// We call ExpandPath() here in case the strings have [[U]].
*/

	m_csDocumentDirectory = ExpandPath(pUserSettings->GetString(SECTION_Paths, ENTRY_ProjectPath, "[[U]]\\PROJECTS"));
	m_csDocumentDirectory.MakeUpper();
	m_csPictureDirectory = ExpandPath(pUserSettings->GetString(SECTION_Paths, ENTRY_ArtPath, "[[U]]\\ART"));
	m_csPictureDirectory.MakeUpper();
	m_csAddressBookDirectory = ExpandPath(pUserSettings->GetString(SECTION_Paths, ENTRY_AddressBookPath, "BOOKS"));
	m_csAddressBookDirectory.MakeUpper();
	m_csContentDirectory = ExpandPath(pUserSettings->GetString(SECTION_Paths, ENTRY_ContentPath, "CONTENT"));
	m_csContentDirectory.MakeUpper();

	// Construct the default shared directory.
	CString csProgramFilesDir;
	Util::GetProgramFilesDir(csProgramFilesDir);
	CString csDefault;
	Util::ConstructPath(csDefault, csProgramFilesDir, "Mindscape\\Shared");

	m_csSharedDirectory = ExpandPath(pUserSettings->GetString(SECTION_Paths, ENTRY_SharedPath, csDefault));
	m_csSharedDirectory.MakeUpper();

	//
	// Read the CD paths from the INI files.
	//

	// First, the global CD path.
	m_csGlobalCDPath = pUserSettings->GetString(SECTION_Configuration,
															  ENTRY_CDPath,
															  "",
															  IUserSettings::GlobalSetting);
	SetExtendedDriveType(m_csGlobalCDPath, IDriveManager::global_cd_drive_letter);

	// Then, the local CD path.
	m_csLocalCDPath = pUserSettings->GetString(SECTION_Configuration,
															 ENTRY_CDPath,
															 "",
															 IUserSettings::UserSetting);
	SetExtendedDriveType(m_csLocalCDPath, IDriveManager::local_cd_drive_letter);
}

/*
// Does a file exist?
*/

BOOL CStdPathManager::FileExists(LPCSTR pszFileName, BOOL fLocate)
{
	UINT uPrevSem = ::SetErrorMode(SEM_FAILCRITICALERRORS);

	// Fill out the path appropriately.
	CString csPath;
	if (fLocate)
	{
		StorageFile File;
		csPath = LocatePath(pszFileName, &File);
	}
	else
	{
		csPath = ExpandPath(pszFileName);
	}

	// Check the existence.
	BOOL fExists = FALSE;
	if (!csPath.IsEmpty())
	{
		fExists = (AccessFile(csPath) == 0);
	}

	::SetErrorMode(uPrevSem);

	return fExists;
}

/*
// Is a file on removable media? This assumes some sort of path binding
// type (i.e. "D:\FOO.BAR" will never be removable, but "[[C:LBL]]\FOO.BAR"
// will be.
*/

BOOL CStdPathManager::FileIsRemovable(LPCSTR filename)
{
	CompositeFileName cfn(filename);
	LPCSTR pPath = cfn.get_dynamic_name(TRUE);
	switch (GetPathBindingType(pPath))
	{
		case PBT_RemovableDrive:
		case PBT_InternetPath:
		{
			return TRUE;
		}
		case PBT_CDDrive:
		{
			return TRUE;
		}
		case PBT_CollectionItem:
		{
			// The item is in a collection.
			// Is the collection on removable media?
			CString csOutPath;
			CString csCollection;

			if (BindPath(pPath, csOutPath, &csCollection) == PBT_CollectionItem)
			{
				// We have the collection name.
				CPMWCollection* pCollection = m_pCollectionManager->FindCollection(csCollection);
				if (pCollection != NULL)
				{
					// We have the collection.
					// Get the extension for the data file.
					CString csDataPath;
					if (!pCollection->ExtensionToPath(".CDT", csDataPath))
					{
						// Get the data path for the collection.
						csDataPath = pCollection->GetCDDir();
					}
					// See if that file is removable.
					return FileIsRemovable(csDataPath);
				}
			}
			// Assume TRUE since we can not tell.
			return TRUE;
		}
		default:
		{
			break;
		}
	}
	return FALSE;
}

/*
// Encode a subdirectory.
*/

BOOL CStdPathManager::EncodeSubdirectory(LPCSTR pszPath, CString& csOutPath, LPCSTR pszToken, LPCSTR pszSubdirectory)
{
	// Create a full path for the subdirectory.
	CString csFullSubdir = ExpandPath(pszSubdirectory);

	PCSTR s = csFullSubdir;
	LPCSTR d = pszPath;

	while (*s != '\0')
	{
		if (toupper(*s) != toupper(*d))
		{
		/* Ran out of path or no match. */
			return FALSE;
		}
		s++, d++;
	}

/*
// We have reached the end of the path having matched all the way.
// We can substitute the token for the path.
*/

	if (*d == '\\')
	{
		*d++;
	}

	csOutPath.Format("%s\\%s", pszToken, d);

//	od("Converted %s to %s\r\n", path, out_buffer);

	return TRUE;
}

/*
// Encode the data CD drive.
*/

BOOL CStdPathManager::EncodeHomeCD(LPCSTR pszPath, CString& csOutPath)
{
	LPCSTR pszHomePath;

	if (m_last_home_drive != 0
#ifndef BUILDING_ALBUM_FROM_NOVELL
			&& FindAppData(m_last_home_drive)
#endif
			&& (pszHomePath = HomeDrivePath(m_last_home_drive)) != NULL)
	{
		return EncodeSubdirectory(pszPath, csOutPath, "[[H]]", pszHomePath);
	}
	return FALSE;
}

/*
// Encode a removable media drive.
*/

BOOL CStdPathManager::EncodeRemovableDrive(LPCSTR pszPath, CString& csOutPath)
{
	ASSERT(m_pDriveManager != NULL);
/*
// See if this path references a removable drive.
*/

	char cDrive = PathDrive(pszPath);

	DriveType drive_type = m_pDriveManager->GetDriveType(cDrive);

	if (drive_type != DT_Removable && drive_type != DT_CDROM)
	{
		return FALSE;
	}

/*
// We need to remember the drive letter even though we're making it bindable.
// The drive letter will be the default when being bound.
// Since we got this far, we can be assured that the drive letter exists.
*/

	LPCSTR pszVolume = m_pDriveManager->GetDriveVolumeLabel(cDrive);

	if (pszVolume == NULL)
	{
		pszVolume = missing_volume_string;
	}

	csOutPath.Format("[[%c:%c,%s]]%s",
						  drive_type == DT_CDROM ? 'C' : 'R',
						  cDrive,
						  pszVolume,
						  pszPath+2);

//	od("Converted %s to %s\r\n", path, out_buffer);

	return TRUE;
}

/*
// Encode a path for later dynamic binding.
*/

LPCSTR CStdPathManager::EncodePath(LPCSTR pszPath, CString& csOutPath)
{
/*
// Build the full path for this guy.
// We need this to do full path comparisons.
*/

	CString csFullPath = ExpandPath(pszPath);

/*
// Order of encoding (so, for example, if the picture and background
// subdirectories are the same, it will encode the picture subdirectory):
//
// (1) "[[H]]" Data (home) cd drive
// (2) "[[S]]" Content subdirectory
// (3) "[[P]]" Picture subdirectory
// (4) "[[D]]" Document subdirectory
// (5) "[[A]]" Address book subdirectory
// (6) "[[U]]" User subdirectory
// (7) "[[C:d,label]]" or "[[R:d,label]]" Removable media drive
//
// NOTE: We no longer try to encode the background subdirectory ([[B]]).
*/

	// Encode the home CD first.
	if (!EncodeHomeCD(csFullPath, csOutPath))
	{
		if (!EncodeSubdirectory(csFullPath, csOutPath, "[[S]]", m_csContentDirectory))
		{
			if (!EncodeSubdirectory(csFullPath, csOutPath, "[[P]]", m_csPictureDirectory))
			{
				if (!EncodeSubdirectory(csFullPath, csOutPath, "[[D]]", m_csDocumentDirectory))
				{
					if (!EncodeSubdirectory(csFullPath, csOutPath, "[[A]]", m_csAddressBookDirectory))
					{
						if (!EncodeSubdirectory(csFullPath, csOutPath, "[[U]]", m_csUserDirectory))
						{
							if (!EncodeRemovableDrive(csFullPath, csOutPath))
							{
								return NULL;
							}
						}
					}
				}
			}
		}
	}

	return csOutPath;
}

/*
// Preprocess a binding path to see what type of binding it has.
// A return of PBT_None means that it has no binding. A return of PBT_Unknown
// means that either an error occurred or an unknown (future?) binding was
// found.
*/

PathBindingType CStdPathManager::PreprocessBinding(LPCSTR &pszPath)
{
/*
// Validate the argument.
*/

	if (pszPath == NULL)
	{
		return PBT_Unknown;
	}

/*
// As of now, the late-binding component must begin with a special character
// sequence (one of the binding tokens).
*/

	if (pszPath[0] != '[' || pszPath[1] != '[')
	{
		return PBT_None;
	}

/*
// Now see what kind of binding it is.
*/

	char cType = pszPath[2];
	pszPath += 3;

	switch (cType)
	{
		case 'A':
		{
			// Address Book subdirectory.
			return PBT_AddressBookPath;
		}
		case 'B':
		{
			// Background subdirectory.
			return PBT_PicturePath;				// Now maps to picture path.
		}
		case 'C':
		{
			// CD volume.
			return PBT_CDDrive;
		}
		case 'D':
		{
			// Document subdirectory.
			return PBT_DocumentPath;
		}
		case 'G':
		{
			// Item in content collection.
			return PBT_CollectionItem;
		}
		case 'H':
		{
			// Home (usually CD) data volume.
			return PBT_HomeDataPath;
		}
		case 'P':
		{
			// Picture subdirectory.
			return PBT_PicturePath;
		}
		case 'R':
		{
			// Removable volume (not a CD; e.g. floppy)
			return PBT_RemovableDrive;
		}
		case 'S':
		{
			// Content subdirectory
			return PBT_ContentPath;
		}
		case 'U':
		{
			// Content subdirectory
			return PBT_UserPath;
		}
		case 'I':
		{
			// Internet Cache directory
			return PBT_InternetPath;
		}
		case 's':
		{
			// Internet Cache directory
			return PBT_SharedPath;
		}
		default:
		{
			// We don't know what this is, so we can't interpret it.
			return PBT_Unknown;
		}
	}
}

/*
// Bind to a subdirectory.
*/

BOOL CStdPathManager::BindSubdirectory(LPCSTR pszRest, CString& csOutPath, LPCSTR pszDirectory)
{
/*
// Skip to the next ']'. At this time, we write these guys out such that
// rest[0] and rest[1] will be the terminating ']'s. However, since these
// sequences may be extended in the future, we do an intelligent skip over
// any other data which may comprise the rest of any future sequence to the
// terminating characters. We really don't know what the other data may be,
// so we just ignore it.
*/
	if ((pszRest = strchr(pszRest, ']')) == NULL || pszRest[1] != ']')
	{
	/* Couldn't find the trailing ']]'. Illegally formed. */
		return FALSE;
	}

/* We've passed the sequence. */

	pszRest += 2;						/* Skip the terminating characters. */

/* Now we need to build the path from the passed path and what's left. */

	if (*pszRest == '\\')
	{
	/* Skip the slash. */
		pszRest++;
	}

/*
// Copy the path in.
// We make the directory into a full path so what we end up with finally will
// be complete and usable.
*/

	csOutPath = ExpandPath(pszDirectory);
	Util::AppendBackslashToPath(csOutPath);
	csOutPath += pszRest;

//	od("Built subdirectory path: '%s'\r\n", out_buffer); 

	return TRUE;
}

/*
// Bind to a removable drive.
*/

BOOL CStdPathManager::BindRemovable(LPCSTR pszRest, CString& csOutPath, CString* pcsVolume)
{
	// The next character must be a colon.
	if (*pszRest++ != ':')
	{
		return FALSE;
	}

	// The next character must be the drive letter.
	char cDrive = *pszRest++;

	if (*pszRest == ',')
	{
		// We have a drive letter. Validate it.
		if (cDrive < 'A' || cDrive > 'Z')
		{
			return FALSE;
		}
		pszRest++;
	}
	else
	{
		// No drive letter. Force dummy one.
		cDrive = 'D';
		// We guessed wrong. Back up.
		pszRest--;
	}

	// Extract the volume.
	if (pcsVolume != NULL)
	{
		// Look for the end of the volume label.

		LPCSTR p;
		if ((p = strchr(pszRest, ',')) == NULL)
		{
			if ((p = strchr(pszRest, ']')) == NULL)
			{
				// Illegally formed.
				return FALSE;
			}
		}
		int nLen = p - pszRest;
		*pcsVolume = CString(pszRest).Left(nLen);
		pszRest = p;
	}

	// Skip to the end of the sequence.
	if ((pszRest = strchr(pszRest, ']')) == NULL || pszRest[1] != ']')
	{
		return FALSE;
	}

	// Plug the drive letter in.
	csOutPath = cDrive;
	csOutPath += ':';
	csOutPath += pszRest+2;			// Skip over ']]'.

//	od("Built removable path: '%s' (Volume %s)\r\n",
//				out_buffer,
//				(LPCSTR)(volume == NULL ? "*None*" : volume)); 

	return TRUE;
}

/*
// Bind to a collection item.
// This mostly parses the binding to get the collection name.
*/

BOOL CStdPathManager::BindCollectionItem(LPCSTR pszRest, CString& csOutPath, CString* pcsCollection)
{
	// The next character must be a colon.
	if (*pszRest++ != ':')
	{
		return FALSE;
	}

	// Extract the collection name.
	if (pcsCollection != NULL)
	{
		// Look for the end of the volume label.
		LPCSTR p;
		if ((p = strchr(pszRest, ']')) == NULL)
		{
			// Illegally formed.
			return FALSE;
		}
		int nLen = p - pszRest;
		*pcsCollection = CString(pszRest).Left(nLen);
		pszRest = p;
	}

	// Skip to the end of the sequence.
	if ((pszRest = strchr(pszRest, ']')) == NULL || pszRest[1] != ']')
	{
		return FALSE;
	}

	// Skip over ']]'.
	pszRest += 2;

	// Skip a leading slash.
	if (*pszRest == '\\')
	{
		pszRest++;
	}

	// Extract the remainder of the file name.
	csOutPath = pszRest;

//	od("Built collection item: '%s' (Collection %s)\r\n",
//				out_buffer,
//				(LPCSTR)(volume == NULL ? "*Error*" : volume)); 

	return TRUE;
}

BOOL CStdPathManager::BindInternetItem(LPCSTR pszRest, CString& csOutPath, CString* pcsCollection)
{
	// The next character must be a colon.
	if (*pszRest++ != ':')
	{
		return FALSE;
	}

	// Extract the collection name.
	if (pcsCollection != NULL)
	{
		// Look for the end of the collection name.
		LPCSTR p;
		if ((p = strchr(pszRest, ']')) == NULL)
		{
			// Illegally formed.
			return FALSE;
		}
		int nLen = p - pszRest;
		*pcsCollection = CString(pszRest).Left(nLen);
		pszRest = p;
	}

	// Skip to the end of the sequence.
	if ((pszRest = strchr(pszRest, ']')) == NULL || pszRest[1] != ']')
	{
		return FALSE;
	}

	// Skip over ']]'.
	pszRest += 2;

	// Skip a leading slash.
	if (*pszRest == '\\')
	{
		pszRest++;
	}

	// Extract the remainder of the file name.
	csOutPath = pszRest;

//	od("Built collection item: '%s' (Collection %s)\r\n",
//				out_buffer,
//				(LPCSTR)(volume == NULL ? "*Error*" : volume)); 

	return TRUE;
}

/*
// Construct a drive binding.
*/

BOOL CStdPathManager::ConstructHomeDriveBinding(EXTENDED_DRIVE_LETTER cDrive,
															  LPCSTR pszRest,
															  CString& csOutPath)
{
	// Skip to the end of the sequence.
	if ((pszRest = strchr(pszRest, ']')) == NULL || pszRest[1] != ']')
	{
		return FALSE;
	}

	// Skip over the end of the sequence.
	pszRest += 2;

	// We now have the drive. Get the path for that drive.
	LPCSTR pszPath = HomeDrivePath(cDrive);

	if (pszPath == NULL)
	{
		// No path for the drive!
		if (cDrive > 'Z')
		{
			// Unable to recover for this kind of drive.
			return FALSE;
		}

		// Try valiantly to succeed.
		csOutPath = cDrive;
		csOutPath += ':';
	}
	else
	{
		// Use the path for the drive.
		csOutPath = pszPath;
	}

	if (*pszRest == '\\')
	{
		pszRest++;
	}

	Util::AppendBackslashToPath(csOutPath);
	csOutPath += pszRest;

	return TRUE;
}

/*
// Bind with the home data path.
// Performs a simplistic fail case to attempt a non-prompting bind.
*/

BOOL CStdPathManager::BindHomeDataVolume(LPCSTR pszRest, CString& csOutPath)
{
	char cDrive;

	ASSERT(m_pDriveManager != NULL);

	if ((cDrive = m_last_home_drive) == 0)
	{
		char first_cd_drive = 0;

		// Look for any CD-ROM drive. Most people will only have one.
		for (cDrive = 'A'; cDrive < IDriveManager::last_drive_letter; cDrive++)
		{
			DriveType type;

			// Look on all CD drives and all extended drives that can have
			// a volume label (i.e. that may hold our CD).
			if ((type = m_pDriveManager->GetDriveType(cDrive)) == DT_CDROM
					|| (cDrive > 'Z'
								&& type != DT_Unknown
								&& type != DT_Invalid
								&& type != DT_ExtendedPath))
			{
				// If this is the first CD, remember it just in case.
				if (type == DT_CDROM && first_cd_drive == 0)
				{
					first_cd_drive = cDrive;
				}

				// See if the volume is the one we want.
				if (TestDriveVolume(cDrive, m_csVolumeID, DT_CDROM, TRUE))
				{
					// We found the volume label. This is it for sure.
					m_last_home_drive = cDrive;

					if (cDrive > 'Z')
					{
					/* This is an extended drive. Make sure we know it's a CD. */
						m_pDriveManager->SetDriveType(cDrive, DT_CDROM, TRUE);
					}
					break;
				}
			}
		}

		if (cDrive == IDriveManager::last_drive_letter)
		{
		/*
		// We didn't find the volume id.
		// Make some guesses.
		*/
			DriveType type;

			// Test the "local cd drive" for validity.
			if ((type = m_pDriveManager->GetDriveType(IDriveManager::local_cd_drive_letter)) != DT_Unknown
					&& type != DT_Invalid)
			{
				cDrive = IDriveManager::local_cd_drive_letter;
			}

			// Test the "global cd drive" for validity.
			else if ((type = m_pDriveManager->GetDriveType(IDriveManager::global_cd_drive_letter)) != DT_Unknown
					&& type != DT_Invalid)
			{
				cDrive = IDriveManager::global_cd_drive_letter;
			}

			// Use any cd drive we found.
			else if (first_cd_drive != 0)
			{
				// At least one drive. Assume that that is it.
				cDrive = first_cd_drive;
			}

			// Default to the current working directory.
			else if ((cDrive = PathDrive(m_csWorkingDirectory)) == 0)
			{
				// We need something!
				cDrive = 'C';
			}
		}

		// Do we want to do this?
		m_last_home_drive = cDrive;
	}

	return ConstructHomeDriveBinding(cDrive, pszRest, csOutPath);
}

/*
// Bind a path to the current environment.
// This does not check for the existence of the path; it merely constructs it.
// The function returns the type of binding if successful.
*/

PathBindingType CStdPathManager::BindPath(LPCSTR pszPath, CString& csOutPath, CString* pcsParam /*=NULL*/)
{
	// Initialize the parameter, in case there is none or is not meaningful.
	if (pcsParam != NULL)
	{
		pcsParam->Empty();
	}

	// Is this a valid binding name?
	LPCSTR p = pszPath;
	PathBindingType type;

	switch (type = PreprocessBinding(p))			/* This can change 'p'. */
	{
		case PBT_AddressBookPath:
		{
			// Address book subdirectory.
			if (!BindSubdirectory(p, csOutPath, m_csAddressBookDirectory))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_CDDrive:
		{
			// CD volume.
			if (!BindRemovable(p, csOutPath, pcsParam))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_DocumentPath:
		{
			// Document subdirectory.
			if (!BindSubdirectory(p, csOutPath, m_csDocumentDirectory))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_RemovableDrive:
		{
			// Removable volume.
			if (!BindRemovable(p, csOutPath, pcsParam))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_HomeDataPath:
		{
			// Home (usually CD) data volume.
			if (!BindHomeDataVolume(p, csOutPath))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_PicturePath:
		{
			// Picture subdirectory.
			if (!BindSubdirectory(p, csOutPath, m_csPictureDirectory))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_ContentPath:
		{
			// Content file. */
			if (!BindSubdirectory(p, csOutPath, m_csContentDirectory))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_CollectionItem:
		{
			// Collection item. "Volume" is the collection name.
			if (!BindCollectionItem(p, csOutPath, pcsParam))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_UserPath:
		{
			// User subdirectory.
			if (!BindSubdirectory(p, csOutPath, m_csUserDirectory))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_InternetPath:
		{
			// Cache subdirectory.
			if (!BindInternetItem(p, csOutPath, pcsParam))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_SharedPath:
		{
			// Shared subdirectory.
			if (!BindSubdirectory(p, csOutPath, m_csSharedDirectory))
			{
				type = PBT_Unknown;
			}
			break;
		}
		case PBT_None:
		{
			// Has no binding. Do the simple relative-to-absolute thing.
			csOutPath = BuildPathName(m_csWorkingDirectory, pszPath);
			break;
		}
		default:
		{
			// We don't know what this is, so we can't interpret it.
			ASSERT(FALSE);				// Unknown binding.
			csOutPath = BuildPathName(m_csWorkingDirectory, pszPath);
			break;
		}
	}
	return type;
}

CString CStdPathManager::BuildPathName(LPCSTR path, LPCSTR name)
{
	CString csOutPath;
	if (path != NULL)
	{
		if (name[0] == '\\' && name[1] == '\\')
		{
		/* Network name. This is NOT relative. */
		}
		else if ((name[0] == '\\' || name[0] == '/')
					&& path[1] == ':')
		{
		/* Absolute path minus drive. Copy just the drive letter. */
			csOutPath = path[0];
			csOutPath += ':';
		}
		else if (name[0] == '\0' || name[1] != ':')
		{
		/* Copy the full base path over. */
			csOutPath = path;
			Util::AppendBackslashToPath(csOutPath);
		}
	}
	csOutPath += name;
	return csOutPath;
}

/*
// Test the volume for a drive.
*/

BOOL CStdPathManager::TestDriveVolume(char cDrive, LPCSTR pszVolume, DriveType drive_type, BOOL fReadIt)
{
	// Get the drive type so we can see if it matches.
	ASSERT(m_pDriveManager != NULL);
	if (m_pDriveManager->GetDriveType(cDrive) == drive_type)
	{
		// Try this guy first.

		LPCSTR pszSearchVolume;

		if ((pszSearchVolume = m_pDriveManager->GetDriveVolumeLabel(cDrive, fReadIt)) == NULL)
		{
			pszSearchVolume = missing_volume_string;
		}

		if (stricmp(pszVolume, pszSearchVolume) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*
// Prompt for a volume.
*/

BOOL CStdPathManager::prompt_for_volume(DriveType drive_type, PCSTR volume, LPCSTR file_name)
{
   if (!m_fPromptForContent || m_pMediaPromptNotify == NULL)
   {
      // Just pass on this.
      return FALSE;
   }

// od("Prompt for volume '%s' on drive type %d for file '%s'\r\n",
//             (LPCSTR)volume, drive_type, file_name);

	// Get the media friendly name if there is one.
	CString csMediaName;
   if (strcmp(volume, missing_volume_string) != 0)
   {
      TRY
      {
			// Try in the collection manager.
			m_pCollectionManager->VolumeToFriendlyName(volume, csMediaName);
      }
      END_TRY
	}

	return m_pMediaPromptNotify->PromptForVolume(drive_type, volume, csMediaName, file_name);
}

/*
// Prompt for the home CD.
*/

BOOL CStdPathManager::prompt_for_home_cd(void)
{
   if (!m_fPromptForContent || m_pMediaPromptNotify == NULL)
   {
      // Just pass on this.
      return FALSE;
   }

// od("Prompt for volume '%s' on drive type %d for file '%s'\r\n",
//             (LPCSTR)volume, drive_type, file_name);

	return m_pMediaPromptNotify->PromptForHomeCD();
}

/*
// Find a volume and return its drive letter.
// A return of 0 means failure.
*/

DRIVE_LETTER CStdPathManager::FindVolume(LPCSTR pszVolume, DriveType drive_type, DRIVE_LETTER best_drive, DRIVE_LETTER &drive, int &nState)
{
	switch (nState)
	{
		case 0:
		{
			//
			// State 0. Check "best_drive" for the last label read.
			//

			nState++;						// In case we fail.

			if (TestDriveVolume(best_drive, pszVolume, drive_type, FALSE))
			{
				if (TestDriveVolume(best_drive, pszVolume, drive_type, TRUE))
				{
					// Found it!
					return best_drive;
				}
			}
			// Fall through to...
		}
		case 1:
		{
			//
			// State 1: Begin checking other drives for the last label read.
			//

			drive = 'A';				// Start at the first drive.
			nState++;
			// Fall through to...
		}
		case 2:
		{
			//
			// State 2. Check the next drive for the last label read.
			//

			for (; drive <= 'Z'; drive++)
			{
				// We already checked this guy. Skip him.

				if (drive == best_drive)
				{
					continue;
				}

				// Check it first without reading.
				if (TestDriveVolume(drive, pszVolume, drive_type, FALSE))
				{
					// Now make sure it is still the same (after reading).
					if (TestDriveVolume(drive, pszVolume, drive_type, TRUE))
					{
						// Found it!
						return drive++;
					}
				}
			}
			nState++;
			// Fall through to...
		}
		case 3:
		{
			//
			// State 3: Check the best drive label by reading it.
			//

			nState++;						/* In case we fail. */

			if (TestDriveVolume(best_drive, pszVolume, drive_type, TRUE))
			{
				// Found it!
				return best_drive;
			}

			// Fall through to...
		}
		case 4:
		{
			//
			// State 4: Begin checking drive labels by reading them.
			//

			drive = 'A';				// Start at the first drive.
			nState++;
			// Fall through to...
		}
		case 5:
		{
			//
			// State 5. Check the next drive label by reading it.
			//

			for (; drive <= 'Z'; drive++)
			{
				// We already checked this guy. Skip him.
				if (drive == best_drive)
				{
					continue;
				}

				if (TestDriveVolume(drive, pszVolume, drive_type, TRUE))
				{
					return drive++;
				}
			}
			nState++;
			// Fall through to...
		}
		default:
		{
			// No more to do.
			return 0;
		}
	}
}

/*
// Locate a removable file.
*/

BOOL CStdPathManager::LocateRemovableFile(CString& csOutPath, LPCSTR pszVolume, DriveType drive_type)
{
	DRIVE_LETTER best_drive;

	if ((best_drive = PathDrive(csOutPath)) != 0)
	{
//		od("Try to locate media volume '%s'\r\n", (LPCSTR)volume);

		LPSTR pszOutPath = csOutPath.GetBuffer(csOutPath.GetLength());
		for (;;)
		{
			char drive;
			char state_drive;

			int nState = 0;

			while ((drive = FindVolume(pszVolume, drive_type, best_drive, state_drive, nState)) != 0)
			{
			//
			// We found the volume.
			// Check for the file. We assume that the first letter of the
			// path is the drive letter.
			//
				pszOutPath[0] = drive;

				if (FileExists(pszOutPath))
				{
					// Consider it successfully bound.
					csOutPath.ReleaseBuffer();
					return TRUE;
				}
			}

			// Restore the original drive letter.
			pszOutPath[0] = best_drive;
			csOutPath.ReleaseBuffer();

			//
			// Last ditch effort: look for the file at the last bound path.
			// (Should we be doing this???)
			//

			if (FileExists(csOutPath))
			{
			/*
			// The file exists though the volume is not able to be found.
			// We'll ignore this for now.
			*/
				return TRUE;
			}

			CString csBaseName;
			Util::SplitPath(csOutPath, NULL, &csBaseName);

			if (!prompt_for_volume(drive_type, pszVolume, csBaseName))
			{
				// User said forget it.
				break;
			}
		}
	}
	return FALSE;
}

/*
// Locate a home data file.
*/

BOOL CStdPathManager::LocateHomeDataFile(LPCSTR pszPath, CString& csOutPath)
{
	ASSERT(m_pDriveManager != NULL);

//	od("Locate file %s\r\n", out_buffer);

	// Try the good case first.

	if (FileExists(csOutPath))
	{
		return TRUE;
	}

	// Pre-parse the path to make it easy to construct the name later.

	LPCSTR p = pszPath;

	if (PreprocessBinding(p) != PBT_HomeDataPath)
	{
		ASSERT(FALSE);
		/* We shouldn't even be here! */
		return FALSE;
	}

	//
	// Loop and search.
	//
	// We loop while we are unable to find the volume and the user says "OK"
	// to the insertion prompt.
	//

	for (;;)
	{
		//
		// We need to search in a number of places for the home CD.
		//

		EXTENDED_DRIVE_LETTER cFoundDrive = 0;

		// First look for the home volume on the local search path.

		if (FindAppData(IDriveManager::local_cd_drive_letter))
		{
			cFoundDrive = IDriveManager::local_cd_drive_letter;
		}
		else if (FindAppData(IDriveManager::global_cd_drive_letter))
		{
			cFoundDrive = IDriveManager::global_cd_drive_letter;
		}
		else
		{
			// Search on all CD-ROM drives.
			for (char cDrive = 'A'; cDrive <= 'Z'; cDrive++)
			{
				DriveType type = m_pDriveManager->GetDriveType(cDrive);

				if (type == DT_CDROM && FindAppData(cDrive))
				{
					cFoundDrive = cDrive;
					break;
				}
			}

			if (cFoundDrive == 0)
			{
				// Search on all remote.
				for (char cDrive = 'A'; cDrive <= 'Z'; cDrive++)
				{
					DriveType type = m_pDriveManager->GetDriveType(cDrive);

					if (type == DT_Remote && FindAppData(cDrive))
					{
						cFoundDrive = cDrive;
						break;
					}
				}
			}

			if (cFoundDrive == 0)
			{
				// Search on all non-remote, non-CD >= 'C'.
				for (char cDrive = 'C'; cDrive <= 'Z'; cDrive++)
				{
					DriveType type = m_pDriveManager->GetDriveType(cDrive);

					if (type != DT_CDROM && type != DT_Remote && FindAppData(cDrive))
					{
						cFoundDrive = cDrive;
						break;
					}
				}
			}
		}

		// See if we found the volume.
		if (cFoundDrive != 0)
		{
			//
			// Construct the path.
			//

			if (!ConstructHomeDriveBinding(cFoundDrive, p, csOutPath))
			{
				return FALSE;
			}

			//
			// This is the last drive we found the volume on.
			//

			m_last_home_drive = cFoundDrive;

			//
			// Remember this is a CD-ROM drive if we found the CD here.
			// (This correctly won't change DT_ExtendedPath drives, though.)
			//

			m_pDriveManager->SetDriveType(cFoundDrive, DT_CDROM, TRUE);

			//
			// We assume that if we find the volume, then the user has done all
			// he can do. So no more prompting for the CD.
			//
			// Now we check to see if the file exists.
			//

			return FileExists(csOutPath);
		}

	/*
	// The volume could not be found. Ask the user to put it in a drive.
	*/

		if (!prompt_for_home_cd())
		{
			// User said forget it.
			break;
		}
	}
	return FALSE;
}

/*
// Locate a content file. This will reroute the request to the correct content
// CD if the file is not found in the local path.
*/

BOOL CStdPathManager::LocateContentFile(CString& csOutPath)
{
   // Try the good case first.
   if (FileExists(csOutPath))
   {
      return TRUE;
   }

   // The file does not exist. Re-map the name to the place where it lives
   // on the corresponding content CD.
   // First, split off the base name. This is the name of the content
   // collection.
   CString csName;
   CString csExtension;

   Util::SplitPath(csOutPath,
                   NULL,            // No drive
                   NULL,            // No directory
                   &csName,         // We want a base name
                   &csExtension);   // We want the extension

	// Look for content in shared content areas.
   const CStringArray *pcsaSharedPaths = m_pCollectionManager->GetSharedPaths();
	if (pcsaSharedPaths != NULL)
	{
		for(int i = 0; i < pcsaSharedPaths->GetSize(); i++)
		{
			CString csSharedFile;
			Util::ConstructPath(csSharedFile, pcsaSharedPaths->GetAt(i), csName+csExtension);

			if (FileExists(csSharedFile))
			{
				csOutPath = csSharedFile;
				return TRUE;
			}
		}
	}

   // csName is the name of the collection.
   // Look up the collection in the collection manager.

   CPMWCollection* pCollection = m_pCollectionManager->FindCollection(csName);

   if (pCollection != NULL)
   {
      // Get the data path for the extension.
      CString csDataPath;
      if (!pCollection->ExtensionToPath(csExtension, csDataPath))
      {
         // Get the data path for the collection.
         csDataPath = pCollection->GetCDDir();
      }

      CString csNewPath;
      Util::ConstructPath(csNewPath, csDataPath, csName+csExtension);

      // Re-invoke the path manager with the new path.
		csOutPath = LocatePath(csNewPath);

      return TRUE;
   }

	// Path not found.
   return FALSE;
}

BOOL CStdPathManager::LocateInternetItem(LPCSTR pszPath, CString& csOutPath, LPCSTR pszCollection)
{
	//default cache subdirectory is handled by CContentManager

	CString csFileName = csOutPath;
	if (m_pContentManager != NULL)
	{
		UINT uResult = m_pContentManager->GetInternetContent(pszCollection, csFileName, csOutPath);
		if (uResult == CSSTATUS_Success)
		{
			return TRUE;
		}
	}

	// File not found.
   return FALSE;
}

/*
// Locate a collection item.
// This is a mildly complicated procedure involving collections, etc.
*/

BOOL CStdPathManager::LocateCollectionItem(CString& csOutPath, LPCSTR pCollectionName, StorageFile* pFile)
{
	if (pFile == NULL)
	{
		// We need a file to bind this kind of path.
		// ASSERT(FALSE);
		return FALSE;
	}

   // Grab the name since csOutPath may be changed.
   CString csName = csOutPath;

	// We should now have the collection name.
	// Lookup the collection.
	CPMWCollection* pCollection;

	pCollection = m_pCollectionManager->FindCollection(pCollectionName);

	if (pCollection == NULL)
	{
		// The collection does not exist.
		// Should we show an error?

		// We do not know what the collection is. Return FALSE.
		return FALSE;
	}

	CFileContent* pContent = NULL;
	CCollectionCompletionData* pNotifyData = NULL;

	TRY
	{
		if (pCollection->NewContentStream(&pContent) == ERRORCODE_None)
		{
			pNotifyData = new CCollectionCompletionData;

			pNotifyData->m_Dir.SetKey(csName);
			if (pContent->Find(&pNotifyData->m_Dir, &pNotifyData->m_Data) == ERRORCODE_None)
			{
				// We have the content data.
				// Create the completion notify data.
				pNotifyData->m_pCollection = pCollection;
				pNotifyData->m_pContent = pContent;
				pNotifyData->m_pUncompressedData = NULL;
            pNotifyData->m_bOwnUncompressedData = FALSE;

				DWORD dwSize = pNotifyData->m_Data.GetItemSize();
				pContent->Read(dwSize);

				// Create a memory device to contain this.
				if (pNotifyData->m_Data.m_DataHeader.GetType() == 0
					 // Check for old badly-written files.
					 || (DWORD)pNotifyData->m_Data.m_DataHeader.GetUnCompressedSize() <= dwSize)
				{
					pNotifyData->m_Device.AddPiece(pNotifyData->m_Data.GetData(), dwSize);
				}
				else
				{
					// We need to decompress the data. Use deflate for now.
					CDeflate Deflator;
					DWORD dwUncompressedSize = 0;

               dwUncompressedSize = (DWORD)pNotifyData->m_Data.m_DataHeader.GetUnCompressedSize();
               if(dwUncompressedSize > 0)
               {
                  pNotifyData->m_pUncompressedData = (LPBYTE)new char[dwUncompressedSize];
                  pNotifyData->m_bOwnUncompressedData = TRUE;
               }
					if (Deflator.Decompress((LPBYTE)pNotifyData->m_Data.GetData(),
													dwSize,
													&pNotifyData->m_pUncompressedData,
													&dwUncompressedSize,
                                       pNotifyData->m_pUncompressedData,
                                       dwUncompressedSize))
					{
						// We have the data.
						pNotifyData->m_Device.AddPiece(pNotifyData->m_pUncompressedData,
																 dwUncompressedSize);
					}
					else
					{
						// Failure!
						pCollection->ReleaseContentStream(pContent);
						delete pNotifyData;
						return FALSE;
					}
				}

				// Attach this to the file.
				pFile->set_based_device(&pNotifyData->m_Device, FALSE);
				pFile->set_completion_notify(CollectionCompletion, pNotifyData);

				// Success!
				return TRUE;
			}
		}
	}
	END_TRY

	// Failure!
	if (pContent != NULL)
	{
		pCollection->ReleaseContentStream(pContent);
	}

	delete pNotifyData;

	return FALSE;
}

/*
// This is the completion routine called when a StorageFile based on
// a collection item calls flush().
*/

void CStdPathManager::CollectionCompletion(StorageFilePtr file, LPVOID data)
{
	// Get a pointer to the notify data.
	CCollectionCompletionData* pNotifyData = (CCollectionCompletionData*)data;

	// Detach the memory device.
	file->set_based_device(NULL, FALSE);

	// Delete any uncompressed data.
   if(pNotifyData->m_bOwnUncompressedData == TRUE)
      delete pNotifyData->m_pUncompressedData;
   else
	   CDeflate::FreeData(pNotifyData->m_pUncompressedData);

	// Release the content stream.
	ASSERT(pNotifyData->m_pCollection != NULL);
	ASSERT(pNotifyData->m_pContent != NULL);
	if (pNotifyData->m_pCollection != NULL && pNotifyData->m_pContent != NULL)
	{
		pNotifyData->m_pCollection->ReleaseContentStream(pNotifyData->m_pContent);
	}

	// Get rid of the notify data.
	delete pNotifyData;
}

/*
// Bind to the file via a late-binding name.
// The function returns TRUE if the file exists and nothing else is wrong.
*/

BOOL CStdPathManager::BindToFile(LPCSTR pszPath, CString& csOutPath, StorageFile* pFile /*=NULL*/)
{
	CString csParam;

	// Bind the path to get a type and possible parameter information.

	PathBindingType type = BindPath(pszPath, csOutPath, &csParam);

	switch (type)
	{
		case PBT_Unknown:
		{
			return FALSE;
		}
		case PBT_RemovableDrive:
		case PBT_CDDrive:
		{
		/*
		// Figure out what type of drive we want to look for.
		*/
			return LocateRemovableFile(csOutPath,
												csParam,
												(type == PBT_CDDrive) ? DT_CDROM : DT_Removable);
		}
		case PBT_HomeDataPath:
		{
			return LocateHomeDataFile(pszPath, csOutPath);
		}
		case PBT_ContentPath:
		{
			return LocateContentFile(csOutPath);
		}
		case PBT_CollectionItem:
		{
			return LocateCollectionItem(csOutPath, csParam, pFile);
		}
		case PBT_InternetPath:
		{
			return LocateInternetItem(pszPath, csOutPath, csParam);
		}
		default:
		{
			break;
		}
	}

	// See if the file is there.
	return FileExists(csOutPath);
}

/*
// Return the binding type for this path.
*/

PathBindingType CStdPathManager::GetPathBindingType(LPCSTR pszPath)
{
	return PreprocessBinding(pszPath);
}

/*
// Basic symbolic expansion.
*/

CString CStdPathManager::ExpandPath(LPCSTR pszName, BOOL fWantVirtualName /*=FALSE*/)
{
	CString csOutPath;

	CompositeFileName cfn(pszName);

	LPCSTR pszBaseName = cfn.get_file_name();

/*
// We don't want to try and find the file.
// Just return the appropriate component of the (possibly composite)
// filename fleshed out to an absolute path.
*/

	if (fWantVirtualName && cfn.has_virtual_name())
	{
		// Use the virtual name.

		if (BindPath(cfn.get_virtual_name(), csOutPath) != PBT_Unknown)
		{
			return csOutPath;
		}
	}

	if (cfn.has_dynamic_name())
	{
		if (BindPath(cfn.get_dynamic_name(), csOutPath) != PBT_Unknown)
		{
			return csOutPath;
		}
	}

	// Bind the base name.
	BindPath(pszBaseName, csOutPath);
	return csOutPath;
}

CString CStdPathManager::LocatePath(LPCSTR pszName, StorageFile* pFile /*=NULL*/)
{
	CString csOutPath;

	CompositeFileName cfn(pszName);

	LPCSTR pszBaseName = cfn.get_file_name();

	//
	// If the composite name has a virtual component, use it if the file exists.
	// This is because that is the file the user believes is there, and if
	// the file is updated to actually exist locally (say from a CD-ROM
	// installation), we should use that name first.
	//

	if (cfn.has_virtual_name())
	{
		// There is a virtual name. See if this file exists.
		if (BindToFile(cfn.get_virtual_name(), csOutPath, pFile))
		{
			// Use the virtual name since it now exists.
			return csOutPath;
		}
	}

	//
	// If the composite name has a dynamic component, attempt to bind to it
	// before the bound path. This will prevent using old erroneous bound paths.
	//

	if (cfn.has_dynamic_name())
	{
		if (BindToFile(cfn.get_dynamic_name(), csOutPath, pFile))
		{
			return csOutPath;
		}
	}

	//
	// In some special cases, the incoming base name is actually a binding name.
	// Composite names are not usually stored this way for backward compatability,
	// but some internal code is greatly simplified by allowing binding in this
	// name component.
	//

	if (!BindToFile(pszBaseName, csOutPath, pFile))
	{
		// Not found.
		csOutPath.Empty();
	}

	// Return the found path. It may be empty.
	return csOutPath;
}

/*
// Strip the part of the path which is common to our working subdirectory
// to create a relative path.
*/

LPCSTR CStdPathManager::GetRelativePath(LPCSTR name)
{
	LPCSTR s, d;

/* Strip off any matching path to make a relative path name. */

	for (s = name, d = m_csWorkingDirectory; *s == *d; s++, d++)
		;

	if (s > (name+1))
	{
	 	if (*(s-2) == ':')
		{
		/* Must keep root directory slash. */
			s--;
		}
		else if (*d != '\0')
		{
		/*
		// We didn't suck up all of the current directory.
		// Must make it root relative (but we can leave the drive off).
 		*/
			s = name+2;
		}
	}

	return s;
}

/*
// Basic symbolic reduction.
// The returned string needs to be freed by the caller. 
*/

CString CStdPathManager::MakeCompositePath(LPCSTR pszName)
{
/*
// Make the name relative.
*/

	pszName = GetRelativePath(pszName);

/*
// Encode the virtual name.
// This is no longer used.
*/

	LPCSTR vname = NULL;

/*
// Encode the file name to make the dynamic name if possible.
*/

	LPCSTR dname = NULL;

	CString csDynamicName;
	if (EncodePath(pszName, csDynamicName))
	{
		dname = csDynamicName;
	}

/*
// Make the CompositeFileName and return its composite name.
*/

	CompositeFileName cfn(pszName, dname, vname);
	return cfn.get_composite_name();
}

/*
// Find the home CD or else.
*/

BOOL CStdPathManager::FindHomeCD(void)
{
	BOOL fResult = FALSE;
	CString csName;
	CString csOutPath;

	TRY
	{
		csName = "[[H]]\\";
		csName += m_csSignatureFile;
		fResult = LocateHomeDataFile(csName, csOutPath);
	}
	END_TRY
	return fResult;
}

BOOL CStdPathManager::SetPromptForContent(BOOL fPrompt)
{
	BOOL fOldPrompt = m_fPromptForContent;
	m_fPromptForContent = fPrompt;
	return fOldPrompt;
}

/*
// My version of access().
// This now returns a CFileException enum value (which may be 'none').
*/

int CStdPathManager::AccessFile(LPCSTR pName)
{
	WIN32_FIND_DATA ffd;
	HANDLE hSearch;

	if ((hSearch = FindFirstFile(pName, &ffd)) == INVALID_HANDLE_VALUE)
	{
		return CFileException::OsErrorToException(GetLastError());
	}
	else
	{
		FindClose(hSearch);
		return CFileException::none;			// Which should be 0.
	}
}

/////////////////////////////////////////////////////////////////////////////
// Interface map for CStdPathManager.

BEGIN_INTERFACE_MAP(CStdPathManager, CCmdTarget)
	INTERFACE_PART(CStdPathManager, IID_IPathManager, PathManager)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUnknown interface for CStdPathManager.
// This is standard stuff brought over from the docs.

STDMETHODIMP_(ULONG) CStdPathManager::XPathManager::AddRef()
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CStdPathManager::XPathManager::Release()
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->ExternalRelease();
}

STDMETHODIMP CStdPathManager::XPathManager::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IPathManager interface for CStdPathManager.

//
// Get the working directory.
//

STDMETHODIMP_(LPCSTR) CStdPathManager::XPathManager::GetWorkingDirectory()
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->GetWorkingDirectory();
}

//
// Expand a path. This resolves any dynamic bindings.
// Works with either a composite file name or a regular file name.
//

STDMETHODIMP_(CString) CStdPathManager::XPathManager::ExpandPath(LPCSTR pszName, BOOL fWantVirtualName /*=FALSE*/)
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->ExpandPath(pszName, fWantVirtualName);
}

//
// Locate a path. This resolves any dynamic bindings, making sure the
// resulting file exists or is otherwise accessible (e.g. collection binding).
// Works with either a composite file name or a regular file name.
// The optional StorageFile pointer can be filled in for special cases.
//

STDMETHODIMP_(CString) CStdPathManager::XPathManager::LocatePath(LPCSTR pszName, StorageFile* pFile /*=NULL*/)
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->LocatePath(pszName, pFile);
}

//
// Construct a path (encode it for later binding via ExpandPath or LocatePath).
//

STDMETHODIMP_(CString) CStdPathManager::XPathManager::MakeCompositePath(LPCSTR pszName)
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->MakeCompositePath(pszName);
}

//
// Return the binding type for a path.
//

STDMETHODIMP_(PathBindingType) CStdPathManager::XPathManager::GetPathBindingType(LPCSTR pszPath)
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->GetPathBindingType(pszPath);
}

//
// Does a file exist?
//

STDMETHODIMP_(BOOL) CStdPathManager::XPathManager::FileExists(LPCSTR pszFileName, BOOL fLocate /*=FALSE*/)
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->FileExists(pszFileName, fLocate);
}

//
// Utility function for expanding a single (not composite) path.
//

STDMETHODIMP_(PathBindingType) CStdPathManager::XPathManager::BindPath(LPCSTR pszPath, CString& csOutPath, CString* pcsParam /*=NULL*/)
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->BindPath(pszPath, csOutPath, pcsParam);
}

//
// Set whether we prompt for CDs (etc) or not.
//

STDMETHODIMP_(BOOL) CStdPathManager::XPathManager::SetPromptForContent(BOOL fPrompt)
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->SetPromptForContent(fPrompt);
}

//
// Return whether a file is removable or not.
//

STDMETHODIMP_(BOOL) CStdPathManager::XPathManager::FileIsRemovable(LPCSTR pszPath)
{
	METHOD_PROLOGUE(CStdPathManager, PathManager)
	return pThis->FileIsRemovable(pszPath);
}
