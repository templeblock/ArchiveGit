/*
// $Header: /PM8/msreg/DRIVEMGR.CPP 1     3/03/99 6:22p Gbeddow $
//
// Drive manager routines.
//
// $Log: /PM8/msreg/DRIVEMGR.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   22 Nov 1995 13:06:42   JAY
//  
// 
//    Rev 1.0   22 Nov 1995 09:39:54   JAY
// Initial revision.
// 
//    Rev 1.0   03 Nov 1994 16:51:20   JAY
// Initial revision.
// 
//    Rev 1.5   04 Aug 1994 14:28:32   JAY
// WIN32 GetDriveType returns a value fore CDROM. No longer use int 2f (for WIN32).
// 
//    Rev 1.4   04 Aug 1994 08:30:30   JAY
// Large model and 32-bit changes
// 
//    Rev 1.3   21 Jun 1994 13:31:02   JAY
// Removed some debugging messages.
// 
//    Rev 1.2   14 Jun 1994 17:06:18   JAY
// Added DT_ExtendedPath.
// Added set_drive_type().
// 
//    Rev 1.1   31 May 1994 12:45:04   JAY
// Added drive type overrides.
// 
//    Rev 1.0   26 May 1994 13:52:32   JAY
// Initial revision.
*/ 

#include <windows.h>

#include "drivemgr.h"

//////////////////////////////////////////////////////////////////////////
// CDriveManager routines.

/*
// Constructor.
*/

CDriveManager::CDriveManager()
{
	for (int i = 0; i < LastDriveLetter - 'A'; i++)
	{
		m_dtDriveType[i] = DT_Unknown;
	}
}

/*
// Destructor.
*/

CDriveManager::~CDriveManager()
{
	Close();
}

/*
// Is this a CD-ROM drive?
// Incoming param should be 0 (=A:), 1 (=B:), etc.
*/

#ifndef _WIN32
BOOL CDriveManager::IsCDRomDrive(WORD wDrive)
{
	BOOL bResult = FALSE;			// Assume not a CD-ROM
	_asm
	{
		mov	ax, 150bh				// MSCDEX CD-ROM drive check
		xor	bx, bx
		mov	cx, wDrive
		int	2fh
		cmp	bx, 0ADADh				// Check MSXDEX signature
		jne	not_cd_drive
		or		ax, ax					// Check drive type
		je		not_cd_drive
		mov	bResult, TRUE
	not_cd_drive:
	}
	return bResult;
}
#endif

int GetTypeOfDrive(char cDrive)
{
#ifdef WIN32
	char cbRoot[] = "X:\\";
	cbRoot[0] = cDrive;
	return ::GetDriveType(cbRoot);
#else
	return ::GetDriveType(cDrive - 'A');
#endif
}

/*
// Get the type of a drive.
// Drive letters should be in the range 0 to last_drive_letter-1.
*/

DriveType CDriveManager::DiscoverDriveType(DRIVE_INDEX cDrive)
{
	DriveType dtType;

	switch (dtType = (DriveType)::GetTypeOfDrive(cDrive+'A'))
	{
		case DT_Remote:
		{
#ifndef _WIN32
		/*
		// A CD-ROM drive looks like a remote drive.
		// Flag this as a CD-ROM drive if MSCDEX says it is one.
		*/
			if (IsCDRomDrive(cDrive))
			{
				dtType = DT_CDROM;
			}
#endif
			break;
		}
		default:
		{
			break;
		}
	}
//	od("Type of drive %c is %d\r\n", cDrive + 'A', dtType);

	return dtType;
}

/*
// Return the type of a drive.
// 'drive' is [A..Z]
// 'alias' is [A..last_drive_letter-1].
*/

DriveType CDriveManager::GetDriveType(EXTENDED_DRIVE_LETTER cDrive, EXTENDED_DRIVE_LETTER cAlias)
{
	if (cAlias == 0)
	{
		cAlias = cDrive;

	/*
	// No alias. We want the type for *this* drive.
	// If the type is extended, we can only return the type.
	// There can be no "discovery".
	*/

		if (cDrive > 'Z')
		{
		/* Param check. */
			if (cDrive >= LastDriveLetter)
			{
				return DT_Invalid;
			}
		/* We can only return the type. */
			return m_dtDriveType[cDrive - 'A'];
		}
	}

/* Value check everything. */

	if (cDrive < 'A' || cDrive > 'Z'
			|| cAlias < 'A' || cAlias >= LastDriveLetter)
	{
		return DT_Invalid;
	}

	cDrive -= 'A';
	cAlias -= 'A';

/* Do the discovery if we need to. */

	if (m_dtDriveType[cAlias] == DT_Unknown)
	{
		m_dtDriveType[cAlias] = DiscoverDriveType(cDrive);
	}
	return m_dtDriveType[cAlias];
}

/*
// Set a drive type.
*/

DriveType CDriveManager::SetDriveType(EXTENDED_DRIVE_LETTER cDrive, DriveType Type, BOOL fForce)
{
	if (cDrive >= 'A' && cDrive < LastDriveLetter)
	{
		cDrive -= 'A';
		if ((fForce && m_dtDriveType[cDrive] != DT_ExtendedPath)
					|| m_dtDriveType[cDrive] == DT_Unknown)
		{
			m_dtDriveType[cDrive] = Type;
		}
		return m_dtDriveType[cDrive];
	}
	return DT_Invalid;
}

/*
// Open the drive manager.
*/

BOOL CDriveManager::Open()
{
#if 0
/* Allocate our volume label block. */

	if ((m_volume_labels = (LP_VOLUME_LABEL)system_heap->allocate(('Z' - 'A' + 1)*sizeof(VOLUME_LABEL))) == NULL)
	{
		return FALSE;
	}
#endif

	return TRUE;
}

/*
// Close the drive manager.
*/

void CDriveManager::Close()
{
#if 0
	if (m_volume_labels != NULL)
	{
		system_heap->free(m_volume_labels);
		m_volume_labels = NULL;
	}
#endif
}

/*
// Init all drive types.
*/

void CDriveManager::InitDrives(void)
{
/*
// Process the INI file drive type override keys.
// These allow custom configuration in case software techniques are incorrect.
// Unfortnuately, the order in which we process these determine what will
// happen if someone (incorrectly) includes the same drive in more than one
// list.
*/

#if 0
	process_drive_overrides(ENTRY_IsCD, DT_CDROM);
	process_drive_overrides(ENTRY_IsRemote, DT_Remote);
	process_drive_overrides(ENTRY_IsFixed, DT_Fixed);
	process_drive_overrides(ENTRY_IsRemovable, DT_Removable);
#endif

/* Now, we need to initialize all unknown drives. */

	for (char cDrive = 'A'; cDrive <= 'Z'; cDrive++)
	{
		GetDriveType(cDrive);
//		*(m_volume_labels[cDrive-'A']) = '\0';
	}
}

