//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/Download/IDRVMGR.H 1     3/03/99 6:15p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/Download/IDRVMGR.H $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 3     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 1     9/15/98 10:49a Jayn
//////////////////////////////////////////////////////////////////////////////

#ifndef __IDRVMGR_H__
#define __IDRVMGR_H__

// {BCF2DEC1-49D1-11d2-985D-00A0246D4780}
DEFINE_GUID(IID_IDriveManager, 0xbcf2dec1, 0x49d1, 0x11d2, 0x98, 0x5d, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80);

//
// Some typedefs to keep straight what a function wants.
//

typedef char DRIVE_LETTER;					/* 'A' to 'Z' */
typedef char EXTENDED_DRIVE_LETTER;		/* 'A' to last_drive_letter-1 */
typedef char DRIVE_INDEX;					/* 0 to (last_drive_letter-1-'A') */

//
// Drive types.
//

enum DriveType
{
	DT_Unknown = -1,
	DT_Invalid = 0,							// 0 from Windows GetDriveType()
	DT_NoRootDir = DRIVE_NO_ROOT_DIR,	// 1 from Windows GetDriveType()
	DT_Removable = DRIVE_REMOVABLE,		// 2 from Windows GetDriveType()
	DT_Fixed = DRIVE_FIXED,					// 3 from Windows GetDriveType()
	DT_Remote = DRIVE_REMOTE,				// 4 from Windows GetDriveType()
	DT_CDROM = DRIVE_CDROM,					// 5 from Windows GetDriveType()
	DT_RAMDisk = DRIVE_RAMDISK,			// 6 from Windows GetDriveType()
	
	DT_ExtendedPath = 100					// Extended drive with path
};

//
// The Drive Manager interface.
//

#undef INTERFACE
#define INTERFACE   IDriveManager

DECLARE_INTERFACE_(IDriveManager, IUnknown)
{
	enum ExtraDriveLetters
	{
		local_cd_drive_letter  = 'Z'+1,
		global_cd_drive_letter = 'Z'+2,
		last_drive_letter
	};

	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IDriveManager methods ***

	//
	// Return the discovered drive type for a drive.
	//

	STDMETHOD_(DriveType, GetDriveType)(THIS_ EXTENDED_DRIVE_LETTER nDrive, EXTENDED_DRIVE_LETTER nAlias = 0) PURE;

	//
	// Set the drive type. This handles weird cases like extended drives.
	// The drive type returned is either the passed type or DT_Invalid if the
	// drive letter is invalid.
	//

	STDMETHOD_(DriveType, SetDriveType)(THIS_ EXTENDED_DRIVE_LETTER nDrive, DriveType nType, BOOL fForce = FALSE) PURE;

	//
	// Return the volume label for a drive.
	// Parameter 'drive' is 'A', 'B', etc.
	// An invalid label may be returned as either NULL or an empty string.
	//

	STDMETHOD_(LPCSTR, GetDriveVolumeLabel)(THIS_ DRIVE_LETTER nDrive, BOOL fReadNow = TRUE) PURE;
};

#endif
