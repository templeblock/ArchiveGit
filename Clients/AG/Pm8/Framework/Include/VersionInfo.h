// ****************************************************************************
//
//  File Name:			VersionInfo.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RVersionInfo class
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
//  $Logfile:: /PM8/Framework/Include/VersionInfo.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_VERSIONINFO_H_
#define		_VERSIONINFO_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RVersionInfo
//
//  Description:	Class for retreiving version info
//
// ****************************************************************************
//
class FrameworkLinkage RVersionInfo
	{
	// Operations
	public :
												RVersionInfo( );
		BOOLEAN								GetVersionInfo( );
		void									Write( RArchive& archive ) const;
		void									Read( RArchive& archive );

	// Members
	public :
		uLONG									m_MajorVersion;
		uLONG									m_MinorVersion;
		uLONG									m_RevisionVersion;
		uLONG									m_InternalVersion;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif
