/*
// $Header: /PM8/Download/PATHMGR.H 1     3/03/99 6:15p Gbeddow $
//
// The path manager.
//
// $Log: /PM8/Download/PATHMGR.H $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 14    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 13    10/02/98 7:10p Jayn
// More content manager changes
// 
// 12    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 11    9/22/98 3:32p Psasse
// converting LPCSTR'S to CString&
// 
// 10    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 9     9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 8     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 7     9/01/98 1:39p Dennis
// Drastically improved speed of content decompression by allocating our
// own buffer.
// 
// 6     8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 5     7/02/98 12:01p Johno
// Removed unused parameter from LocateContentFile()
// 
// 4     4/16/98 1:23p Psasse
// 
// 3     4/06/98 9:08p Psasse
// 
// 2     4/02/98 9:09p Psasse
// internet cache support
// 
//    Rev 1.0   14 Aug 1997 15:23:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:00   Fred
// Initial revision.
// 
//    Rev 1.7   26 Jun 1997 17:11:28   Jay
// FileIsRemovable
// 
//    Rev 1.6   17 Jun 1997 12:42:44   Jay
// Import; OpenStreams()
// 
//    Rev 1.5   09 May 1997 10:17:00   Jay
// Project Save
// 
//    Rev 1.4   07 May 1997 12:10:52   Jay
// User directory and collections
// 
//    Rev 1.3   23 Apr 1997 12:08:14   Jay
// path manager changes for collection items
// 
//    Rev 1.2   17 Apr 1997 16:06:08   Jay
// Initial collection integration
// 
//    Rev 1.1   16 Sep 1996 14:10:58   Jay
// Can disable prompting for CD.
// 
//    Rev 1.0   14 Mar 1996 13:29:32   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 10:28:42   Jay
// Changes from 2.1
// 
//    Rev 2.4   27 Feb 1996 16:47:54   JAY
// New filename translation system supporting subfiles.
// 
//    Rev 2.3   05 Jun 1995 11:35:04   JAY
//  
// 
//    Rev 2.2   19 Apr 1995 12:32:22   JAY
//  
// 
//    Rev 2.1   08 Feb 1995 13:39:56   JAY
// Reverted. New series.
// 
//    Rev 1.9   04 Aug 1994 07:34:56   JAY
// Large model and 32-bit changes
// 
//    Rev 1.8   15 Jul 1994 12:08:36   JAY
// must_needs_home_cd().
// 
//    Rev 1.7   15 Jun 1994 14:06:32   JAY
// Created PathManager interface and removed other PMW dependencies (SoundManager).
// 
//    Rev 1.6   14 Jun 1994 17:07:10   JAY
// Support for ExtendedPath drive types.
// Home CD data path token is now fully supported.
// 
//    Rev 1.5   10 Jun 1994 10:09:08   JAY
// Added flag pointer argument to bind_to_virtual_file().
// 
//    Rev 1.4   01 Jun 1994 16:48:08   JAY
// Added parameter to file_exists().
// 
//    Rev 1.3   31 May 1994 12:45:30   JAY
// Added path_binding_type().
// Fixed code which looks for drives while binding.
// 
//    Rev 1.2   26 May 1994 13:51:52   JAY
// Moved some code out of here to DriveManager.
// More code for prompting for missing disks.
// 
//    Rev 1.1   23 May 1994 09:14:38   JAY
// Moved paths here from CPmwApp.
// Now uses composite filenames.
// Other changes!
// 
//    Rev 1.0   12 May 1994 07:38:38   JAY
// Initial revision.
// 
//    Rev 1.0   06 May 1994 17:11:30   JAY
// Initial revision.
*/

#ifndef __PATHMGR_H__
#define __PATHMGR_H__

#include "iuserset.h"	// IUserSettings definitions
#include "idrvmgr.h"		// IDriveManager definitions
#include "ipathmgr.h"	// IPathManager definitions
#include "icollmgr.h"	// ICollectionManager definitions
#include "icontsrv.h"	// IContentServer definitions
#include "icontmgr.h"	// IContentManager definitions
#include "imedntfy.h"	// IMediaNotify definitions

/*
// The path manager
*/

class CStdPathManager : public CCmdTarget
{
public:
	CStdPathManager();
	virtual ~CStdPathManager();

	BOOL Open(void);
	void Close(void);

	void InitPaths(IUserSettings* pUserSettings,
						IDriveManager* pDriveManager,
						ICollectionManager* pCollectionManager,
						IContentManager* pContentManager,
						IMediaPromptNotify* pMediaPromptNotify,
						LPCSTR pszVolumeID,
						LPCSTR pszSignatureFile,
						LPCSTR pszUserDirectory,
						LPCSTR pszHomeCDRoot);

	// Interface hooks.
	CString ExpandPath(LPCSTR name, BOOL fWantVirtualName = FALSE);
	CString LocatePath(LPCSTR name, StorageFile* pFile = NULL);
	CString MakeCompositePath(LPCSTR pszName);

/*
// Get the picture search path array.
*/

////// Interface functions //////

	PathBindingType GetPathBindingType(LPCSTR pszPath);

/*
// Basic symbolic reduction.
*/

	// Do we prompt for a content (like a CD)?
	// Use wisely. This can turn off home CD prompting as well.
	BOOL SetPromptForContent(BOOL fPrompt);

/*
// Return the working directory.
*/

	LPCSTR GetWorkingDirectory(void) const
		{ return m_csWorkingDirectory; }

/*
// Does a file exist?
*/

	BOOL FileExists(LPCSTR filename, BOOL fLocate = FALSE);

/*
// Is a file on removable media? This assumes some sort of path binding
// type (i.e. "D:\FOO.BAR" will never be removable, but "[[C:LBL]]\FOO.BAR"
// will be.
*/

	BOOL FileIsRemovable(LPCSTR filename);

////// Non-interface functions //////

/*
// Find the home CD or else.
*/

	BOOL FindHomeCD(void);

protected:
/*
// Make a relative path.
// This returns a pointer to somewhere within the original string.
*/

	LPCSTR GetRelativePath(LPCSTR name);

/*
// Set the drive type for an extended drive (and return it).
// This will either return the drive type of the path (if the path is
// a root directory), or it will return DT_ExtendedPath.
*/

	DriveType SetExtendedDriveType(LPCSTR path, EXTENDED_DRIVE_LETTER edrive);

/*
// Map an extended drive to a normal drive. It returns 0 if failure.
*/

	DRIVE_LETTER MapExtendedHomeDrive(EXTENDED_DRIVE_LETTER drive);

	// Prepend path onto name if name is relative.
	CString BuildPathName(LPCSTR path, LPCSTR name);

protected:

/*
// Return a useable home path for a drive.
// A return of NULL means the drive is invalid.
*/

	LPCSTR HomeDrivePath(char cDrive);

	// Home CD identifying routines.
	BOOL IsAppVolume(char cDrive);
	BOOL HasAppIdFile(char cDrive);
	BOOL FindAppData(char cDrive);

/*
// Test a drive for a particular volume.
*/

	BOOL TestDriveVolume(char cDrive, LPCSTR pszVolume, DriveType drive_type, BOOL fReadIt);

/*
// Prompt for a volume.
*/

	BOOL prompt_for_volume(DriveType drive_type, PCSTR volume, LPCSTR file_name);

/*
// Prompt for the home CD.
*/

	BOOL prompt_for_home_cd(void);

/*
// Locate a removable file.
// csOutPath[0] should be the initial drive letter. It will be changed to
// the correct drive letter if the drive is found.
*/

	BOOL LocateRemovableFile(CString& csOutPath, LPCSTR pszVolume, DriveType drive_type);

/*
// Locate the home data file.
*/

	BOOL LocateHomeDataFile(LPCSTR pszPath, CString& csOutPath);

/*
// Locate a content file. This will switch to the correct content CD
// if the local path is not found.
*/

	BOOL LocateContentFile(CString& csOutBuffer);

/*
// Locate a collection item.
// This is a mildly complicated procedure involving collections, etc.
*/

	BOOL LocateCollectionItem(CString& csOutBuffer, LPCSTR pCollectionName, StorageFile* pFile);

/*
// Locate an internet file. This will download the file if necessary and 
// return the correct local path.
*/
	BOOL LocateInternetItem(LPCSTR pszPath, CString& csOutPath, LPCSTR pszCollection);

/*
// Get a file path.
// This is the hook for the StorageFile class to translate file names.
*/

   // The completion support for collection item bindings.
	static void CollectionCompletion(StorageFile* file, LPVOID data);

//
// Encoding routines.
//

	LPCSTR EncodePath(LPCSTR path, CString& csOutPath);
	BOOL EncodeSubdirectory(LPCSTR pszPath, CString& csOutPath, LPCSTR pszToken, LPCSTR pszSubdirectory);
	BOOL EncodeHomeCD(LPCSTR path, CString& csOutPath);
	BOOL EncodeRemovableDrive(LPCSTR path, CString& csOutPath);

//
// Binding (expansion) routines.
//

	PathBindingType PreprocessBinding(LPCSTR &pszPath);
	BOOL ConstructHomeDriveBinding(EXTENDED_DRIVE_LETTER cDrive, LPCSTR pszRest, CString& csOutPath);

	PathBindingType BindPath(LPCSTR pszPath, CString& csOutPath, CString* pcsParam = NULL);

	BOOL BindToFile(LPCSTR pszPath, CString& csOutPath, StorageFile* pFile = NULL);

	BOOL BindSubdirectory(LPCSTR pszRest, CString& csOutPath, LPCSTR pszDirectory);
	BOOL BindRemovable(LPCSTR pszRest, CString& csOutPath, CString* pcsVolume);
	BOOL BindHomeDataVolume(LPCSTR pszRest, CString& csOutPath);
	BOOL BindCollectionItem(LPCSTR pszRest, CString& csOutPath, CString* pcsCollection);
	BOOL BindInternetItem(LPCSTR pszRest, CString& csOutPath, CString* pcsCollection);

	// Find a matching volume on a drive.
	DRIVE_LETTER FindVolume(LPCSTR volume, DriveType drive_type, DRIVE_LETTER best_drive, DRIVE_LETTER &drive, int &nState);

	// Utility functions.
	int AccessFile(LPCSTR pName);

protected:

	IDriveManager* m_pDriveManager;
	ICollectionManager* m_pCollectionManager;
	IContentManager* m_pContentManager;
	IMediaPromptNotify* m_pMediaPromptNotify;

	BOOL m_fPromptForContent;

	CString m_csWorkingDirectory;

	CString m_csUserDirectory;
	CString m_csDocumentDirectory;
	CString m_csPictureDirectory;
	CString m_csAddressBookDirectory;
	CString m_csContentDirectory;
	CString m_csSharedDirectory;			// Typically "\Program Files\Mindscape\Shared"

/*
// Additional CD paths from the INI files.
*/

	CString m_csLocalCDPath;				// CDPath specified in USER.INI
	CString m_csGlobalCDPath;				// CDPath specified in PMW.INI

	CString m_csVolumeID;
	CString m_csSignatureFile;

	CString m_csHomeCDRoot;

/*
// What was the last home drive?
*/

	EXTENDED_DRIVE_LETTER m_last_home_drive;

//
// Interfaces.
//

protected:
	DECLARE_INTERFACE_MAP()

	// This defines the interface that is passed to others.
	// It will have name m_XPathManager.
	// To see comments on the functions, see the IPathManager definition
	// in IPATHMGR.H.
	BEGIN_INTERFACE_PART(PathManager, IPathManager)
		INIT_INTERFACE_PART(CStdPathManager, PathManager)
		STDMETHOD_(LPCSTR, GetWorkingDirectory)(THIS);
		STDMETHOD_(CString, ExpandPath)(THIS_ LPCSTR name, BOOL fWantVirtualName = FALSE);
		STDMETHOD_(CString, LocatePath)(THIS_ LPCSTR name, StorageFile* pFile = NULL);
		STDMETHOD_(CString, MakeCompositePath)(THIS_ LPCSTR pszName);
		STDMETHOD_(PathBindingType, GetPathBindingType)(THIS_ LPCSTR pszPath);
		STDMETHOD_(BOOL, FileExists)(THIS_ LPCSTR pszFilename, BOOL fLocate = FALSE);
		STDMETHOD_(PathBindingType, BindPath)(THIS_ LPCSTR pszPath, CString& csOutPath, CString* pcsParam = NULL);
		STDMETHOD_(BOOL, SetPromptForContent)(THIS_ BOOL fPrompt);
		STDMETHOD_(BOOL, FileIsRemovable)(THIS_ LPCSTR pszPath);
	END_INTERFACE_PART(PathManager)
};

#endif					// #ifndef __PATHMGR_H__
