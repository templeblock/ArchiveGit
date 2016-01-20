// ****************************************************************************
//
//  File Name:			ExceptionHandling.h
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
//  $Logfile:: /PM8/Framework/Include/ExceptionHandling.h                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_EXCEPTIONHANDLING_H_
#define		_EXCEPTIONHANDLING_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// define a base value ERROR_USER_DEFINED_BASE for our exceptions that works with the error values for the particular
//	platform we are building for; this allows us to mix return codes and exception codes
//	and have a unified api error dispatch/reporting mechanism

//	ERROR_USER_DEFINED_BASE = 0xF)) is from error.h; other entries in error.h
// conflict with existing defines so I extract just the needed constant
//#include <ERROR.H>
const uLONG ERROR_USER_DEFINED_BASE =     MacWinDos( 0, 0xF000, UNUSED );

typedef uLONG YException;

const YException kNoError						= ERROR_USER_DEFINED_BASE+0;
const YException kGenericFile					= ERROR_USER_DEFINED_BASE+1;
const YException kFileNotFound				= ERROR_USER_DEFINED_BASE+2;
const YException kInvalidPath					= ERROR_USER_DEFINED_BASE+3;
const YException kEndOfFile					= ERROR_USER_DEFINED_BASE+4;
const YException kTooManyOpenFiles			= ERROR_USER_DEFINED_BASE+5;
const YException kAccessDenied				= ERROR_USER_DEFINED_BASE+6;
const YException kDirectoryFull				= ERROR_USER_DEFINED_BASE+7;
const YException kDiskFull						= ERROR_USER_DEFINED_BASE+8;
const YException kBadSeek						= ERROR_USER_DEFINED_BASE+9;
const YException kDiskHardware				= ERROR_USER_DEFINED_BASE+10;
const YException kShareViolation				= ERROR_USER_DEFINED_BASE+11;
const YException kLockViolation				= ERROR_USER_DEFINED_BASE+12;
const YException kResource						= ERROR_USER_DEFINED_BASE+13;
const YException kUnknownError				= ERROR_USER_DEFINED_BASE+14;
const YException kDataFormatInvalid			= ERROR_USER_DEFINED_BASE+15;
const YException kDataFormatTooOld			= ERROR_USER_DEFINED_BASE+16;
const YException kMemory						= ERROR_USER_DEFINED_BASE+17;
const YException kRegistryError				= ERROR_USER_DEFINED_BASE+18;
const YException kNetworkError				= ERROR_USER_DEFINED_BASE+19;
const YException kClipboardError				= ERROR_USER_DEFINED_BASE+20;
const YException kReadFailure					= ERROR_USER_DEFINED_BASE+21;
const YException kWriteFailure				= ERROR_USER_DEFINED_BASE+22;
const YException kNoTextError					= ERROR_USER_DEFINED_BASE+23;
const YException kUsesLZWCompression		= ERROR_USER_DEFINED_BASE+24;
const YException kAccusoftEvalDLLExpired	= ERROR_USER_DEFINED_BASE+25;
const YException kCannotPasteData			= ERROR_USER_DEFINED_BASE+26;
const YException kInvalidATMVersion			= ERROR_USER_DEFINED_BASE+27;
const YException kWriteProtected				= ERROR_USER_DEFINED_BASE+28;
const YException kMonetError					= ERROR_USER_DEFINED_BASE+29;
const YException kSwapfileFull				= ERROR_USER_DEFINED_BASE+30;
const YException kPrintingCanceled			= ERROR_USER_DEFINED_BASE+30;
const YException kDriveNotReady				= ERROR_USER_DEFINED_BASE+31;

FrameworkLinkage void ThrowException( const YException exception );
FrameworkLinkage int ReportException( const YException exception );
FrameworkLinkage uLONG GetPlatformException( uLONG ulError );
FrameworkLinkage uLONG GetCrossPlatformException( uLONG ulError );
FrameworkLinkage void ThrowExceptionOnOSError();

class RForcedMemoryException
	{
		//	Construction & Destruction 
		public :
													RForcedMemoryException( char* szFileName, uLONG ulLineNumber, uLONG ulSize );
													RForcedMemoryException( const RForcedMemoryException& rhs );

		//	Data members
		public :
			char*									m_szFileName;
			uLONG									m_ulLineNumber;
			uLONG									m_ulSize;
	};

inline RForcedMemoryException::RForcedMemoryException( char* szFileName, uLONG ulLineNumber, uLONG ulSize )
	: m_szFileName( szFileName ),
	  m_ulLineNumber( ulLineNumber ),
	  m_ulSize( ulSize )
	{
	;
	}

inline RForcedMemoryException::RForcedMemoryException( const RForcedMemoryException& rhs )
	: m_szFileName( rhs.m_szFileName ),
	  m_ulLineNumber( rhs.m_ulLineNumber ),
	  m_ulSize( rhs.m_ulSize )
	{
	;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _EXCEPTIONHANDLING_H_
