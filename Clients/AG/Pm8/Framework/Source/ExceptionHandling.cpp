// ****************************************************************************
//
//  File Name:			ExceptionHandling.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Code to handle exceptions
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/ExceptionHandling.cpp                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef IMAGER
#include "GlobalBuffer.h"
#else
#include	"ApplicationGlobals.h"
#endif

#include	"FrameworkResourceIds.h"
#include "IdleProcessing.h"

// ****************************************************************************
//
//  Function Name:	::ReportException( )
//
//  Description:		Function to inform the user of an exception.
//
//  Returns:			the user's response to the alert dialog
//
//  Exceptions:		None
//
// ****************************************************************************
//
int ReportException( const YException exception )
	{
	RAlert		AlertUserDlg;
	YResourceId	resId	= STRING_ERROR_UNKNOWN_ERROR;
	struct	ExceptionToIdMap
		{
			YException	exception;
			YResourceId	resId;
		} ;
	ExceptionToIdMap	map[] =
		{
			{ kNoError,					STRING_ERROR_NO_ERROR },
			{ kGenericFile,			STRING_ERROR_GENERIC_ERROR },
			{ kFileNotFound,			STRING_ERROR_FILE_NOT_FOUND },
			{ kInvalidPath,			STRING_ERROR_INVALID_PATH },
			{ kEndOfFile,				STRING_ERROR_END_OF_FILE },
			{ kTooManyOpenFiles,		STRING_ERROR_TOO_MANY_OPEN_FILES },
			{ kAccessDenied,			STRING_ERROR_ACCESS_DENIED },
			{ kDirectoryFull,			STRING_ERROR_DIRECTORY_FULL },
			{ kDiskFull,				STRING_ERROR_DISK_FULL },
			{ kBadSeek,					STRING_ERROR_BAD_SEEK },
			{ kDiskHardware,			STRING_ERROR_DISK_HARDWARE },
			{ kShareViolation,		STRING_ERROR_SHARE_VIOLATION },
			{ kLockViolation,			STRING_ERROR_LOCK_VIOLATION },
			{ kResource,				STRING_ERROR_RESOURCE },
			{ kUnknownError,			STRING_ERROR_UNKNOWN_ERROR },
			{ kDataFormatInvalid,	STRING_ERROR_DATA_FORMAT_INVALID },
			{ kDataFormatTooOld,		STRING_ERROR_DATA_FORMAT_TOO_OLD },
			{ kClipboardError,		STRING_ERROR_GETTING_CLIPBOARD_DATA },
			{ kCannotPasteData,		STRING_ERROR_CANNOT_PASTE_DATA },
			{ kInvalidATMVersion,	STRING_ERROR_ATMVERSION_INVALID },
			{ kSwapfileFull,			STRING_ERROR_SWAPFILE_FULL },
			{ kWriteProtected,		STRING_ERROR_WRITE_PROTECT },
			{ kDriveNotReady,			STRING_ERROR_DRIVE_NOT_READY }
		} ;

	//	this return value is not used right now but let's keep it around
	YAlertValues yUserReturn = kAlertOk;

	try
		{
		//	Special cases necessary to not barrage the user...
		if ( exception == kMemory )
			{
#if defined( kFILETESTPROJECT ) || defined( IMAGER ) || defined( MAC )
			//	Warn the user now.
			AlertUserDlg.AlertUser( "Low Memory." );
#elif defined( _WINDOWS )
			//	Warn the user at idle time.
			::GetIdleInfo()->m_fMemoryLowWarningNeeded = TRUE;
#endif
			}

		//	Special cases necessary to not barrage the user...
		else if ( exception == kResource )
			{
#if defined( kFILETESTPROJECT ) || defined( IMAGER ) || defined( MAC )
			//	Warn the user now.
			AlertUserDlg.AlertUser( "Low Resources." );
#elif defined( _WINDOWS )
			//	Warn the user at idle time.
			::GetIdleInfo()->m_fResourceLowWarningNeeded = TRUE;
#endif
			}

		else
			{
			int	count	= sizeof(map)/sizeof(map[0]);
			for ( int i = 0; i < count; ++i )
				{
				if ( map[i].exception == exception )
					{
					resId	= map[i].resId;
					break;
					}
				}
			AlertUserDlg.AlertUser( resId );
			}
		}
	catch( ... )
		{
		//	if we got here there probably isn't much we can do about it
		yUserReturn = kAlertCancel;
		}

	return (int)yUserReturn;
	}


// ****************************************************************************
//
//  Function Name:	::ThrowException( )
//
//  Description:		Function to throw an exception. If USE_MFC_EXCEPTIONS is
//							defined, than the appropriate MFC exception is thrown.
//							Otherwise a normal YException is thrown
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void ThrowException( const YException exception )
	{
	throw( exception );
	}


// ****************************************************************************
//
// Function Name:		GetCrossPlatformException( uLONG ulError )
//
// Description:		Converts a platform specific error code to a cross
//							platform exception code.
//
// Returns:				^
//
// Exceptions:			none
//
// ****************************************************************************
//
uLONG GetCrossPlatformException( uLONG ulError )
	{
#if( _WINDOWS )
	switch( ulError )
		{
		case NO_ERROR :
			return( kNoError );
			break;

		case ERROR_GEN_FAILURE :
			return( kGenericFile );
			break;

		case ERROR_FILE_NOT_FOUND :
//		case CFileException::fileNotFound :
			return( kFileNotFound );
			break;

		case ERROR_PATH_NOT_FOUND :
//		case CFileException::badPath :
			return( kInvalidPath );
			break;

		case kEndOfFile :
//		case CFileException::endOfFile :
			return( kEndOfFile );
			break;

		case ERROR_TOO_MANY_OPEN_FILES :
//		case CFileException::tooManyOpenFiles :
			return( kTooManyOpenFiles );
			break;

		case ERROR_ACCESS_DENIED :
//		case CFileException::accessDenied :
			//	REVIEW - ESV 6/20/96 - would like to distinguish between network file 
			//		access restrictions and locked file opened for write access
			return( kAccessDenied );
			break;

		case ERROR_NOT_ENOUGH_MEMORY :
			return( kMemory );
			break;

		case ERROR_DISK_FULL :
//		case CFileException::diskFull :
			return( kDiskFull );
			break;

		case ERROR_SEEK :
//		case CFileException::badSeek :
			return( kBadSeek );
			break;

		case CFileException::hardIO :
			return( kDiskHardware );
			break;

		case ERROR_INVALID_DRIVE :
			return( kDiskHardware );
			break;

		case ERROR_WRITE_PROTECT :
			return( kWriteProtected );
			break;

//	REVIEW Eric V (BDR) 3/30/97 This should probably have its own message (rather than
//	sharing the kDiskHardware message) since it appears to be the error thrown
//	when trying to access an empty drive a: (see bug 1350).
		case ERROR_NOT_READY :
			return( kDiskHardware );
			break;

		case ERROR_SHARING_VIOLATION :
//		case CFileException::sharingViolation :
			return( kShareViolation );
			break;

		case ERROR_LOCK_VIOLATION :
//		case CFileException::lockViolation :
			return( kLockViolation );
			break;

		case kResource :
			return( kResource );
			break;

		case kUnknownError :
			return( kUnknownError );
			break;

		case kDataFormatInvalid :
			return( kDataFormatInvalid );
			break;

		case kDataFormatTooOld :
			return( kDataFormatTooOld );
			break;

		case kMemory :
		//	user ran out of virtual memory - see bug 3107
		case ERROR_COMMITMENT_LIMIT :
			return( kMemory );
			break;

		case kWriteFailure :
			return( kWriteFailure );
			break;

//	break these out so they are easier to debug
#ifdef TPSDEBUG
		case ERROR_ALREADY_EXISTS :
			return( kGenericFile );
			break;

		case ERROR_USER_MAPPED_FILE :
			return( kGenericFile );
			break;

		case ERROR_FILE_INVALID :
			return( kGenericFile );
			break;

		case ERROR_INVALID_ADDRESS :
			return( kGenericFile );
			break;
#endif

		default :
			TpsAssertAlways( "generic exception used" );
			return( kGenericFile );
		}
#else		// _WINDOWS

	return ulError;

#endif		// _WINDOWS
	}


// ****************************************************************************
//
// Function Name:		GetPlatformException( uLONG ulError )
//
// Description:		Converts the error code returned from GetLastError to a
//							platform specific exception code.
//
// Returns:				platform specific exception code
//
// Exceptions:			none
//
// ****************************************************************************
//
uLONG GetPlatformException( uLONG ulError )
	{
#if( _WINDOWS )
	switch( ulError )
		{
		case kNoError :
			return NO_ERROR;

		case  kGenericFile:
			return ERROR_GEN_FAILURE;

		case  kFileNotFound:
			return ERROR_FILE_NOT_FOUND;

		case kInvalidPath :
			return ERROR_PATH_NOT_FOUND;

		case kEndOfFile :
			return kEndOfFile;

		case kTooManyOpenFiles :
			return ERROR_TOO_MANY_OPEN_FILES;

		case kAccessDenied :
			//	REVIEW - ESV 6/20/96 - would like to distinguish between network file 
			//		access restrictions and locked file opened for write access
			return ERROR_ACCESS_DENIED;

		case kBadSeek :
			return ERROR_SEEK;

		case  kDataFormatInvalid:
			return kDataFormatInvalid;

		case  kShareViolation:
			return ERROR_SHARING_VIOLATION;

		case  kLockViolation:
			return ERROR_LOCK_VIOLATION;

		case  kDiskFull:
			return ERROR_DISK_FULL;

		case kDirectoryFull :
			return CFileException::directoryFull;

		case kDiskHardware :
			return CFileException::hardIO;

		case  kResource:
			return kResource;

		case  kUnknownError:
			return kUnknownError;

		case  kDataFormatTooOld:
			return kDataFormatTooOld;

		case kWriteFailure :
			return kWriteFailure;

		case kWriteProtected :
			return kWriteProtected;

		default :
			UnimplementedCode();
			return kGenericFile;
		}
#else		// _WINDOWS

	return ulError;

#endif		// _WINDOWS
	}

// ****************************************************************************
//
// Function Name:		::ThrowExceptionOnOSError()
//
// Description:		Throws an exception based on the last error set by the OS
//
// Returns:				platform specific exception code
//
// Exceptions:			none
//
// ****************************************************************************
void ThrowExceptionOnOSError()
{
#ifdef _WINDOWS
	throw GetCrossPlatformException(::GetLastError());
#endif
#ifdef MAC
	throw kUnknownError;
#endif
}
