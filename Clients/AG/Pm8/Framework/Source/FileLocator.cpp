// *****************************************************************************
//
// File name:			FileLocator.cpp
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Definition of the RAccusoftImage class.
//
// Portability:		Platform-independent
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
// Client:				Broderbund Software, Inc.
//
// Copyright (c) 1995-1996 Turning Point Software.  All rights reserved.
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "FileLocator.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ChunkyStorage.h"

const uLONG kFileLocatorChunkID = 0x00230045;

// ****************************************************************************
//
//  Function Name:	RFileLocator::RFileLocator()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		TBD
//
// ****************************************************************************
RFileLocator::RFileLocator()
{
	m_rUNCPath.Empty();
	m_uSerialNumber = 0;
	m_eMediaType = kUnknown;
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::RFileLocator()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		TBD
//
// ****************************************************************************
RFileLocator::RFileLocator(const RFileLocator& rhs)
{
	if (!rhs.m_rUNCPath.IsEmpty())
		m_rUNCPath = rhs.m_rUNCPath;
	else
		m_rUNCPath.Empty();
	m_uSerialNumber = rhs.m_uSerialNumber;
	m_eMediaType = rhs.m_eMediaType;
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::~RFileLocator()
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		
//
// ****************************************************************************
RFileLocator::~RFileLocator()
{
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::Set()
//
//  Description:		Initializes the file locator with information from the
//							given path
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RFileLocator::Set(const RMBCString& rPath)
{
	m_rUNCPath.Empty();
	m_uSerialNumber = 0;
	m_eMediaType = kUnknown;

	try
	{
		SetFilePath(rPath);
		SetVolumeInfo(rPath);
	}
	catch(...)
	{
		throw;
	}
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::Reset()
//
//  Description:		Clears the data in the file locator
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RFileLocator::Reset()
{
	m_rUNCPath.Empty();
	m_uSerialNumber = 0;
	m_eMediaType = kUnknown;
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::operator=()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		TBD
//
// ****************************************************************************
RFileLocator& RFileLocator::operator=(const RFileLocator& rhs)
{
	if (this == &rhs) return *this;

	if (!rhs.m_rUNCPath.IsEmpty())
		m_rUNCPath = rhs.m_rUNCPath;
	else
		m_rUNCPath.Empty();
	m_uSerialNumber = rhs.m_uSerialNumber;
	m_eMediaType = rhs.m_eMediaType;
	return *this;
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::GetUNCPath()
//
//  Description:		Returns the File path of the given media resource
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
inline const RMBCString& RFileLocator::GetUNCPath()
{
	return m_rUNCPath;
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::GetSerialNumber()
//
//  Description:		Returns the serial number of the given media resource
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
inline uLONG RFileLocator::GetSerialNumber()
{
	return m_uSerialNumber;
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::GetMediaType()
//
//  Description:		Returns the media type of the given media resource
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
inline RFileLocator::EMediaType RFileLocator::GetMediaType()
{
	return m_eMediaType;
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::SetFilePath()
//
//  Description:		Sets the m_rUNCPath member
//
//  Returns:			Nothing
//
//  Exceptions:		kNetworkError, kUnknownError
//
// ****************************************************************************
void RFileLocator::SetFilePath(const RMBCString& rPath)
{
#ifdef _WINDOWS
	char* pBuffer = new char[1];
	// Get the size of the buffer required to store the full File path		
	DWORD uResult, uBufferSize = 1;
	uResult = ::WNetGetUniversalName(rPath,
		UNIVERSAL_NAME_INFO_LEVEL,
		pBuffer,
		&uBufferSize);
	if (uResult != ERROR_MORE_DATA)
	{
		// The path could not be resolved into a UNC name, so set the
		// given path as the file path		
		m_rUNCPath = rPath;		
		delete [] pBuffer;
		return;
	}
			
	// Get the full UNC name
	try
	{
		// Allocate an appropriately sized buffer
		delete [] pBuffer;
		pBuffer = new char[uBufferSize];
		
		// Get the File path
		uResult = ::WNetGetUniversalName(rPath,
			UNIVERSAL_NAME_INFO_LEVEL,
			pBuffer,
			&uBufferSize);
		if (uResult != NO_ERROR)
		{
			// An error occurred
			DWORD uLastError = ::GetLastError();
			switch (uLastError)
			{
			case ERROR_CONNECTION_UNAVAIL:
			case ERROR_EXTENDED_ERROR:		
			case ERROR_NO_NET_OR_BAD_PATH:
			case ERROR_NO_NETWORK:
			case ERROR_NOT_CONNECTED:
				throw kNetworkError;
			default:
				throw kUnknownError;
			}			
		}
		else
		{			
			// Set the File path member
			UNIVERSAL_NAME_INFO* pUNCPath = (UNIVERSAL_NAME_INFO*)pBuffer;
			m_rUNCPath = (RMBCString)pUNCPath->lpUniversalName;		
		}
	}
	catch(YException)
	{		
		delete [] pBuffer;
		return;
	}
	
	delete [] pBuffer;
#else	//	MAC
	UnimplementedCode( );
#endif	// MAC or _WINDOWS
}

// ****************************************************************************
//
//  Function Name:	RFileLocator::SetVolumeInfo()
//
//  Description:		Sets the m_rVolumeName and m_uSerialNumber members
//
//  Returns:			Nothing
//
//  Exceptions:		TBD
//
// ****************************************************************************
void RFileLocator::SetVolumeInfo(const RMBCString& rPath)
{
#ifdef	_WINDOWS
	// Strip the file name out of the path
	// REVIEW STA VC5 - Our conversion operator stopped working in VC5, and I dont know why. I have to call it explicitly.
	CString cFullPath = rPath.operator CString( );
	for (int sIndex = cFullPath.GetLength();
		((sIndex > 0) && ((cFullPath[sIndex - 1] != (char)0x2F) && (cFullPath[sIndex - 1] != (char)0x5C)));
		sIndex--);
	CString cPath = cFullPath.Left(sIndex);

	// Set the current directory
	if (!::SetCurrentDirectory(cPath)) throw kUnknownError;

	// Retrieve the serial number
	// Call GetVolumeInformation(NULL, etc.)
	DWORD uSerialNumber, uMaximumComponentLength, uFileSystemFlags;
	if (!::GetVolumeInformation(NULL,
		NULL,
		0,
		&uSerialNumber,
		&uMaximumComponentLength,
		&uFileSystemFlags,
		NULL,
		0))
		throw kUnknownError;
	
	// Set the serial number appropriately
	m_uSerialNumber = uSerialNumber;

	// Retrieve the drive type
	switch(::GetDriveType(cPath))
	{
	case DRIVE_REMOVABLE: // The drive can be removed from the drive.
		m_eMediaType = kRemovable;
		break;

	case DRIVE_FIXED: // The disk cannot be removed from the drive.
		m_eMediaType = kFixed;
		break;
		
	case DRIVE_REMOTE: // The drive is a remote (network) drive.
		m_eMediaType = kRemote;
		break;
		
	case DRIVE_CDROM: // The drive is a CD-ROM drive.
		m_eMediaType = kCompactDisc;
		break;

	case DRIVE_RAMDISK: // The drive is a RAM disk.
		m_eMediaType = kRamDisk;
		break;

	default:
		m_eMediaType = kUnknown;
		break;
	}
#else	//	MAC

#endif	//	MAC or _WINDOWS
}

// Operator<< (global)
RChunkStorage& operator<<(RChunkStorage& lhs, RFileLocator& rhs)
{
	// Write out the data
	lhs.AddChunk(kFileLocatorChunkID);
	{
		// Store the full File path
		lhs << rhs.m_rUNCPath;
		
		// Store the media type
		uLONG uMediaType = (uLONG)rhs.m_eMediaType;
		lhs << uMediaType;

		// Store the volume serial number
		lhs << rhs.m_uSerialNumber;
	}
	lhs.SelectParentChunk();
	
	return lhs;
}

// Operator>> (global)
RChunkStorage& operator>>(RChunkStorage& lhs, RFileLocator& rhs)
{
	RChunkStorage::RChunkSearcher rSearcher;

	// Look for an image data chunk		
	rSearcher = lhs.Start(kFileLocatorChunkID, !kRecursive);
	if (!rSearcher.End())
	{		
		try
		{
			// Read the full File path
			lhs >> rhs.m_rUNCPath;
			
			// Read the media type
			uLONG uMediaType;
			lhs >> uMediaType;
			rhs.m_eMediaType = (RFileLocator::EMediaType)uMediaType;

			// Read the volume serial number
			lhs >> rhs.m_uSerialNumber;
			
			// Clean up
			lhs.SelectParentChunk();	
		}
		catch(YException)
		{
			lhs.SelectParentChunk();
			throw;
		}
	}
	else
	{
		// Couldn't find a valid data chunk
		throw kDataFormatInvalid;
	}	

	return lhs;
}

// Debugging support
#ifdef TPSDEBUG
void RFileLocator::Validate() const
{
}
#endif //TPSDEBUG

