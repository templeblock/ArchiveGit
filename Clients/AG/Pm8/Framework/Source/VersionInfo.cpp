// ****************************************************************************
//
//  File Name:			VersionInfo.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RVersionInfo class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/VersionInfo.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "VersionInfo.h"

// ****************************************************************************
//
//  Function Name:	RVersionInfo::RVersionInfo( )
//
//  Description:		ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RVersionInfo::RVersionInfo( )
	: m_MajorVersion( 0 ), m_MinorVersion( 0 ), m_RevisionVersion( 0 ), m_InternalVersion( 0 )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RVersionInfo::GetVersionInfo( )
//
//  Description:		Fills this object with version info about the application
//
//  Returns:			TRUE if version info is available
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RVersionInfo::GetVersionInfo( )
	{
	BOOLEAN fReturn = FALSE;

#ifdef _WINDOWS

	char* pVersionInfo = NULL;

	try
		{
		// Get the application name and append .exe
		char appName[ 256 ];
		strcpy( appName, ::AfxGetApp( )->m_pszAppName );
		strcat( appName, ".exe" );

		// Get the version info size
		DWORD	dwToZero;
		DWORD dwVersionSize	= ::GetFileVersionInfoSize( appName, &dwToZero );

		if( dwVersionSize )
			{
			// Allocate memory for the version info
			pVersionInfo = new char[ dwVersionSize ];

			// Get the version info
			if( ::GetFileVersionInfo( appName, dwToZero, dwVersionSize, pVersionInfo ) )
				{
				// Query for the version value
				UINT uLen;
				char *buffer;
				if( VerQueryValue( pVersionInfo,
										 TEXT("\\StringFileInfo\\040904b0\\FileVersion"),
										 (void**)&buffer,
										 &uLen ) )
					{
					// Parse the version string
					sscanf( buffer, "%d,%d,%d,%d", &m_MajorVersion, &m_MinorVersion, &m_RevisionVersion, &m_InternalVersion );

					fReturn = TRUE;
					}
				}
			}
		}

	catch( ... )
		{
		;
		}

	delete [] pVersionInfo;

#endif	// _WINDOWS

	return fReturn;
	}

// ****************************************************************************
//
//  Function Name:	RVersionInfo::Write( )
//
//  Description:		Writes to an archive
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RVersionInfo::Write( RArchive& archive ) const
	{
	// Write the version numbers
	archive << m_MajorVersion;
	archive << m_MinorVersion;
	archive << m_RevisionVersion;
	archive << m_InternalVersion;

	// Write out some extra padding
	uLONG ulExtra = 0;
	archive << ulExtra;
	archive << ulExtra;
	archive << ulExtra;
	archive << ulExtra;
	archive << ulExtra;
	archive << ulExtra;
	archive << ulExtra;
	archive << ulExtra;
	archive << ulExtra;
	}
	
// ****************************************************************************
//
//  Function Name:	RVersionInfo::Read( )
//
//  Description:		Reads from an archive
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RVersionInfo::Read( RArchive& archive )
	{
	// Read the version numbers
	archive >> m_MajorVersion;
	archive >> m_MinorVersion;
	archive >> m_RevisionVersion;
	archive >> m_InternalVersion;

	// Read in the extra padding
	uLONG ulExtra;
	archive >> ulExtra;
	archive >> ulExtra;
	archive >> ulExtra;
	archive >> ulExtra;
	archive >> ulExtra;
	archive >> ulExtra;
	archive >> ulExtra;
	archive >> ulExtra;
	archive >> ulExtra;
	}
