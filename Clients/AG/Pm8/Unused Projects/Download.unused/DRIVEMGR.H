/*
// $Header: /PM8/Download/DRIVEMGR.H 1     3/03/99 6:15p Gbeddow $
//
// The drive manager.
//
// $Log: /PM8/Download/DRIVEMGR.H $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 5     10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 4     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 3     9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 2     9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:20:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:27:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:58   FRED
// Initial revision.
// 
//    Rev 2.2   05 Jun 1995 11:33:42   JAY
//  
// 
//    Rev 2.1   08 Feb 1995 13:39:26   JAY
// Reverted. New series.
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

#include "idrvmgr.h"			// IDriveManager
#include "iuserset.h"		// IUserSettings

/////////////////////////////////////////////////////////////////////////////
// CStdDriveManager command target

class CStdDriveManager : public CCmdTarget
{
public:
	CStdDriveManager();
	virtual ~CStdDriveManager();

// Operations
public:

	// Open the drive manager.
	BOOL Open(void);

	// Close the drive manager.
	void Close(void);

	// Init all drive types once the INI file is available.
	BOOL InitDrives(IUserSettings* pSettings);

// Interface Maps
public:
	// This defines the interface that is passed to others.
	// It will have name m_XDriveManager.
	// To see comments on the functions, see the IUserSettings definition
	// in IDRVMGR.H.

	BEGIN_INTERFACE_PART(DriveManager, IDriveManager)
		INIT_INTERFACE_PART(CStdDriveManager, DriveManager)
		STDMETHOD_(DriveType, GetDriveType)(THIS_ EXTENDED_DRIVE_LETTER nDrive, EXTENDED_DRIVE_LETTER nAlias /*=0*/);
		STDMETHOD_(DriveType, SetDriveType)(THIS_ EXTENDED_DRIVE_LETTER nDrive, DriveType nType, BOOL fForce /*=FALSE*/);
		STDMETHOD_(LPCSTR, GetDriveVolumeLabel)(THIS_ DRIVE_LETTER nDrive, BOOL fReadNow /*=TRUE*/);
	END_INTERFACE_PART(DriveManager)

	DECLARE_INTERFACE_MAP()
protected:
	// Implementation
	DriveType GetDriveType(EXTENDED_DRIVE_LETTER nDrive, EXTENDED_DRIVE_LETTER nAlias = 0);
	DriveType SetDriveType(EXTENDED_DRIVE_LETTER nDrive, DriveType nType, BOOL fForce = FALSE);
	LPCSTR GetDriveVolumeLabel(DRIVE_LETTER nDrive, BOOL fReadNow = TRUE);

	void ProcessDriveOverrides(IUserSettings* pSettings, LPCSTR pszEntry, DriveType nType);

	typedef char VOLUME_LABEL[13];

	// The list of drive types.
	DriveType m_dtDriveType[IDriveManager::last_drive_letter - 'A'];

	// The volume label block.
	// When a volume label is read, it is stored in the appropriate entry of
	// this block. This gives us a memory area to return to the caller, and it
	// also enables us to remember what the last volume label read was.

	VOLUME_LABEL m_VolumeLabels['Z' - 'A' + 1];
};

/////////////////////////////////////////////////////////////////////////////

#endif					// #ifndef __DRIVEMGR_H__
