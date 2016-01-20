/*
// $Header: /PM8/Download/IPATHMGR.H 1     3/03/99 6:15p Gbeddow $
//
// The path manager interface.
//
// $Log: /PM8/Download/IPATHMGR.H $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 3     10/02/98 7:10p Jayn
// More content manager changes
// 
// 2     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:21:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:18   Fred
// Initial revision.
// 
//    Rev 1.1   23 Apr 1997 12:08:14   Jay
// path manager changes for collection items
// 
//    Rev 1.0   14 Mar 1996 13:36:44   Jay
// Initial revision.
// 
//    Rev 1.0   06 Mar 1996 10:28:28   Jay
// Changes from 2.1
// 
//    Rev 2.2   27 Feb 1996 16:47:48   JAY
// New filename translation system supporting subfiles.
// 
//    Rev 2.2   27 Feb 1996 16:47:34   JAY
// 
//    Rev 2.1   08 Feb 1995 13:39:40   JAY
// Reverted. New series.
// 
//    Rev 1.0   15 Jun 1994 14:06:34   JAY
// Initial revision.
*/

#ifndef __IPATHMGR_H__
#define __IPATHMGR_H__

//// {BCF2DEC2-49D1-11d2-985D-00A0246D4780}
DEFINE_GUID(IID_IPathManager, 0xbcf2dec2, 0x49d1, 0x11d2, 0x98, 0x5d, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80);

// The storage file base class. Used for special translations.
class StorageFile;

/*
// Path binding types.
*/

enum PathBindingType
{
	PBT_Unknown = -1,							// Error or unknown binding
	PBT_None = 0,								// No binding token embedded
	PBT_PicturePath,							// Bound to picture path
	PBT_DocumentPath,							// Bound to document path
	PBT_HomeDataPath,							// Bound to PMW data CD path
	PBT_RemovableDrive,						// Bound to removable (floppy) drive
	PBT_CDDrive,								// Bound to CD drive
	PBT_AddressBookPath,						// Bound to address book path
	PBT_ContentPath,							// Bound to content path
	PBT_CollectionItem,						// Bount to a content collection
	PBT_UserPath,								// Bound to address book path
	PBT_InternetPath,							// Bound to internet cache path
	PBT_SharedPath								// Bound to global shared directory
};

/*
// The path manager interface.
*/

#undef INTERFACE
#define INTERFACE   IPathManager

DECLARE_INTERFACE_(IPathManager, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS) PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IPathManager methods ***

	//
	// Get the working directory.
	//

	STDMETHOD_(LPCSTR, GetWorkingDirectory)(THIS) PURE;

	//
	// Expand a path. This resolves any dynamic bindings.
	// Works with either a composite file name or a regular file name.
	//

	STDMETHOD_(CString, ExpandPath)(THIS_ LPCSTR pszName, BOOL fWantVirtualName = FALSE) PURE;

	//
	// Locate a path. This resolves any dynamic bindings, making sure the
	// resulting file exists or is otherwise accessible (e.g. collection binding).
	// Works with either a composite file name or a regular file name.
	// The optional StorageFile pointer can be filled in for special cases.
	//

	STDMETHOD_(CString, LocatePath)(THIS_ LPCSTR pszName, StorageFile* pFile = NULL) PURE;

	//
	// Construct a path (encode it for later binding via ExpandPath or LocatePath).
	//

	STDMETHOD_(CString, MakeCompositePath)(THIS_ LPCSTR pszName) PURE;

	//
	// Return the binding type for a path.
	//

	STDMETHOD_(PathBindingType, GetPathBindingType)(THIS_ LPCSTR pszPath) PURE;

	//
	// Does a file exist?
	//

	STDMETHOD_(BOOL, FileExists)(THIS_ LPCSTR pszFilename, BOOL fLocate = FALSE) PURE;

	//
	// Utility function for expanding a single (not composite) path.
	//

	STDMETHOD_(PathBindingType, BindPath)(THIS_ LPCSTR pszPath, CString& csOutPath, CString* pcsParam = NULL) PURE;

	//
	// Set whether we prompt for CDs (etc) or not.
	//

	STDMETHOD_(BOOL, SetPromptForContent)(THIS_ BOOL fPrompt) PURE;

	//
	// Return whether a file is removable or not.
	//

	STDMETHOD_(BOOL, FileIsRemovable)(THIS_ LPCSTR pszPath) PURE;
};

#endif					// #ifndef __IPATHMGR_H__
