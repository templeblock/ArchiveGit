/*
// $Header: /PM8/msreg/DRIVEMGR.H 1     3/03/99 6:22p Gbeddow $
//
// The drive manager.
//
// $Log: /PM8/msreg/DRIVEMGR.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   22 Nov 1995 09:39:42   JAY
// Initial revision.
// 
//    Rev 1.0   03 Nov 1994 16:51:20   JAY
// Initial revision.
// 
//    Rev 1.5   16 Sep 1994 14:16:52   JAY
// Changed some ifdefs.
// 
//    Rev 1.4   04 Aug 1994 14:26:10   JAY
// CDROM has a value for WIN32.
// 
//    Rev 1.3   04 Aug 1994 07:34:54   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   14 Jun 1994 17:06:02   JAY
// Added DT_ExtendedPath.
// Added set_drive_type().
// 
//    Rev 1.1   31 May 1994 12:44:54   JAY
// Added drive type overrides.
// 
//    Rev 1.0   26 May 1994 13:52:34   JAY
// Initial revision.
*/ 

#ifndef __DRIVEMGR_H__
#define __DRIVEMGR_H__

/*
// Drive types.
*/

enum DriveType
{
	DT_Unknown = -1,
	DT_Invalid = 0,							// 0 from Windows GetDriveType()
	DT_Removable = DRIVE_REMOVABLE,		// 2 from Windows GetDriveType()
	DT_Fixed = DRIVE_FIXED,					// 3 from Windows GetDriveType()
	DT_Remote = DRIVE_REMOTE,				// 4 from Windows GetDriveType()
#ifdef _WIN32
	DT_CDROM = DRIVE_CDROM,					// CD-ROM drive
#else
	DT_CDROM = 100,							// CD-ROM drive
#endif
	DT_ExtendedPath							// Extended drive with path
};

/*
// Some typedefs to keep straight what a function wants.
*/

typedef char DRIVE_LETTER;					/* 'A' to 'Z' */
typedef char EXTENDED_DRIVE_LETTER;		/* 'A' to last_drive_letter-1 */
typedef char DRIVE_INDEX;					/* 0 to (last_drive_letter-1-'A') */

/*
// The drive manager
*/

class CDriveManager
{
public:
	enum ExtraDriveLetters
	{
		LastDriveLetter = 'Z'+1
	};

protected:

	typedef char VOLUME_LABEL[13];
	typedef VOLUME_LABEL far *LP_VOLUME_LABEL;

/* The list of drive types. */

	DriveType m_dtDriveType[LastDriveLetter - 'A'];

/*
// Get the type of a drive. The drive should be a letter between 'A' and 'Z'.
*/

	static BOOL IsCDRomDrive(WORD wDrive);
	static DriveType DiscoverDriveType(DRIVE_INDEX drive);

public:

/*
// Constructor.
*/

	CDriveManager();

/*
// Destructor.
*/

	~CDriveManager();

/*
// Open the drive manager.
*/

	BOOL Open(void);

/*
// Close the drive manager.
*/

	void Close(void);

/*
// Init all drive types once the INI file is available.
*/

	void InitDrives(void);

/*
// Return the discovered drive type for a drive.
*/

	DriveType GetDriveType(EXTENDED_DRIVE_LETTER cDrive, EXTENDED_DRIVE_LETTER alias = 0);

/*
// Set the drive type. This handles weird cases like extended drives.
// The drive type returned is either the passed type or DT_Invalid if the
// drive letter is invalid.
*/

	DriveType SetDriveType(EXTENDED_DRIVE_LETTER cDrive, DriveType Type, BOOL fForce = FALSE);
};

#endif					// #ifndef __DRIVEMGR_H__
