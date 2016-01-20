/*
// $Header: /PM8/Download/DRIVEMGR.CPP 1     3/03/99 6:15p Gbeddow $
//
// Drive manager routines.
//
// $Log: /PM8/Download/DRIVEMGR.CPP $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 6     10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 5     9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
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
//    Rev 1.0   14 Aug 1997 09:38:14   Fred
// Initial revision.
// 
//    Rev 1.1   13 Sep 1996 09:32:48   Jay
// Implemented get_drive_volume_label() for WIN32.
// 
//    Rev 1.0   14 Mar 1996 13:36:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:16   FRED
// Initial revision.
// 
//    Rev 2.2   05 Jun 1995 11:33:54   JAY
//  
// 
//    Rev 2.1   08 Feb 1995 13:36:22   JAY
// Reverted. New series.
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

#include "stdafx.h"
#include "drivemgr.h"

static char BASED_CODE SECTION_Drives[] = "Drives";
static char BASED_CODE ENTRY_IsCD[] = "IsCD";
static char BASED_CODE ENTRY_IsRemote[] = "IsRemote";
static char BASED_CODE ENTRY_IsFixed[] = "IsFixed";
static char BASED_CODE ENTRY_IsRemovable[] = "IsRemovable";

/////////////////////////////////////////////////////////////////////////////
// CStdDriveManager

/////////////////////////////////////////////////////////////////////////////
// Implementations for CStdDriveManager.

CStdDriveManager::CStdDriveManager()
{
	for (int i = 0; i < IDriveManager::last_drive_letter - 'A'; i++)
	{
		m_dtDriveType[i] = DT_Unknown;
	}
}

CStdDriveManager::~CStdDriveManager()
{
//	Close();
}

//
// Open the drive manager.
//

BOOL CStdDriveManager::Open(void)
{
	// Perform any necessary initialization.
	return TRUE;
}

//
// Close the drive manager.
//

void CStdDriveManager::Close(void)
{
}

//
// Init all drive types once the INI file is available.
//

BOOL CStdDriveManager::InitDrives(IUserSettings* pSettings)
{
	// Process the INI file drive type override keys.
	// These allow custom configuration in case software techniques are incorrect.
	// Unfortnuately, the order in which we process these determine what will
	// happen if someone (incorrectly) includes the same drive in more than one
	// list.

	ProcessDriveOverrides(pSettings, ENTRY_IsCD, DT_CDROM);
	ProcessDriveOverrides(pSettings, ENTRY_IsRemote, DT_Remote);
	ProcessDriveOverrides(pSettings, ENTRY_IsFixed, DT_Fixed);
	ProcessDriveOverrides(pSettings, ENTRY_IsRemovable, DT_Removable);

	// Now, we need to initialize all unknown drives.
	for (char cDrive = 'A'; cDrive <= 'Z'; cDrive++)
	{
		GetDriveType(cDrive);
		*(m_VolumeLabels[cDrive-'A']) = '\0';
	}
	return TRUE;
}

//
// Return the discovered drive type for a drive.
// 'nDrive' is [A..Z]
// 'nAlias' is [A..last_drive_letter-1].
//

DriveType CStdDriveManager::GetDriveType(EXTENDED_DRIVE_LETTER nDrive, EXTENDED_DRIVE_LETTER nAlias /*=0*/)
{
	if (nAlias == 0)
	{
		nAlias = nDrive;

		// No alias. We want the type for *this* drive.
		// If the type is extended, we can only return the type.
		// There can be no "discovery".

		if (nDrive > 'Z')
		{
			// Param check.
			if (nDrive >= IDriveManager::last_drive_letter)
			{
				return DT_Invalid;
			}

			// We can only return the type.
			return m_dtDriveType[nDrive - 'A'];
		}
	}

	// Value check everything.

	if (nDrive < 'A' || nDrive > 'Z'
			|| nAlias < 'A' || nAlias >= IDriveManager::last_drive_letter)
	{
		return DT_Invalid;
	}

	nDrive -= 'A';
	nAlias -= 'A';

	// Do the discovery if we need to.

	if (m_dtDriveType[nAlias] == DT_Unknown)
	{
		// Get the type of drive.
		CString csRoot;
		csRoot.Format("%c:\\", nDrive + 'A');
		m_dtDriveType[nAlias] = (DriveType)::GetDriveType(csRoot);
	}
	return m_dtDriveType[nAlias];
}

//
// Set the drive type. This handles weird cases like extended drives.
// The drive type returned is either the passed type or DT_Invalid if the
// drive letter is invalid.
//

DriveType CStdDriveManager::SetDriveType(EXTENDED_DRIVE_LETTER nDrive, DriveType nType, BOOL fForce /*=FALSE*/)
{
	if (nDrive >= 'A' && nDrive < IDriveManager::last_drive_letter)
	{
		nDrive -= 'A';
		if ((fForce && m_dtDriveType[nDrive] != DT_ExtendedPath)
					|| m_dtDriveType[nDrive] == DT_Unknown)
		{
			m_dtDriveType[nDrive] = nType;
		}
		return m_dtDriveType[nDrive];
	}
	return DT_Invalid;
}

//
// Return the volume label for a drive.
// Parameter 'drive' is 'A', 'B', etc.
// An invalid label may be returned as either NULL or an empty string.
//

LPCSTR CStdDriveManager::GetDriveVolumeLabel(DRIVE_LETTER nDrive, BOOL fReadNow /*=TRUE*/)
{
	// Validate the parameter.
	if (nDrive < 'A' || nDrive > 'Z')
	{
		return NULL;
	}

	// We will be reading into our volume label block.
	LPSTR pVolume = m_VolumeLabels[nDrive-'A'];

	// If we need to read, do the read now.
	if (fReadNow)
	{
		// Get the volume label for the drive.
		static char cbPath[] = "X:\\";
		cbPath[0] = nDrive;

		if (!::GetVolumeInformation(cbPath,
											 pVolume,
											 sizeof(VOLUME_LABEL),
											 NULL,
											 NULL,
											 NULL,
											 NULL,
											 0))
		{
			return NULL;
		}
	}
	return pVolume;
}

/*
// Process the possible drive overrides in a configuration string.
*/

void CStdDriveManager::ProcessDriveOverrides(IUserSettings* pSettings, LPCSTR pszEntry, DriveType nType)
{
	TRY
	{
		// Get the profile string.
		CString s = pSettings->GetString(SECTION_Drives, pszEntry, "");

		// All our drive letters are uppercase.
		s.MakeUpper();

		int nLen = s.GetLength();

		for (int i = 0; i < nLen; i++)
		{
			char c = s[i];

			// If it is valid, set its type.
			if (c >= 'A' && c <= 'Z')
			{
				c -= 'A';
				if (m_dtDriveType[c] == DT_Unknown)
				{
					m_dtDriveType[c] = nType;
				}
			}
		}
	}
	END_TRY
}

/////////////////////////////////////////////////////////////////////////////
// Interface map for CStdDriveManager.

BEGIN_INTERFACE_MAP(CStdDriveManager, CCmdTarget)
	INTERFACE_PART(CStdDriveManager, IID_IDriveManager, DriveManager)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUnknown interface for CStdDriveManager.
// This is standard stuff brought over from the docs.

STDMETHODIMP_(ULONG) CStdDriveManager::XDriveManager::AddRef()
{
	METHOD_PROLOGUE(CStdDriveManager, DriveManager)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CStdDriveManager::XDriveManager::Release()
{
	METHOD_PROLOGUE(CStdDriveManager, DriveManager)
	return pThis->ExternalRelease();
}

STDMETHODIMP CStdDriveManager::XDriveManager::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CStdDriveManager, DriveManager)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IDriveManager interface for CStdDriveManager.

//
// Return the discovered drive type for a drive.
//

STDMETHODIMP_(DriveType) CStdDriveManager::XDriveManager::GetDriveType(EXTENDED_DRIVE_LETTER nDrive, EXTENDED_DRIVE_LETTER nAlias /*=0*/)
{
	METHOD_PROLOGUE(CStdDriveManager, DriveManager)
	return pThis->GetDriveType(nDrive, nAlias);
}

//
// Set the drive type. This handles weird cases like extended drives.
// The drive type returned is either the passed type or DT_Invalid if the
// drive letter is invalid.
//

STDMETHODIMP_(DriveType) CStdDriveManager::XDriveManager::SetDriveType(EXTENDED_DRIVE_LETTER nDrive, DriveType nType, BOOL fForce /*=FALSE*/)
{
	METHOD_PROLOGUE(CStdDriveManager, DriveManager)
	return pThis->SetDriveType(nDrive, nType, fForce);
}

//
// Return the volume label for a drive.
// Parameter 'drive' is 'A', 'B', etc.
// An invalid label may be returned as either NULL or an empty string.
//

STDMETHODIMP_(LPCSTR) CStdDriveManager::XDriveManager::GetDriveVolumeLabel(DRIVE_LETTER nDrive, BOOL fReadNow /*=TRUE*/)
{
	METHOD_PROLOGUE(CStdDriveManager, DriveManager)
	return pThis->GetDriveVolumeLabel(nDrive, fReadNow);
}

