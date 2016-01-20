// ****************************************************************************
//
//  File Name:			Archive.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RArchive class
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
//  $Logfile:: /PM8/Framework/Include/Archive.h                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ARCHIVE_H_
#define		_ARCHIVE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
// Class Name:			RArchive
//
// Description:		Base class for objects that support the interface Read &
//							Write.
//
// ****************************************************************************
//
class FrameworkLinkage RArchive : public RObject
	{
	// Operations
	public :
		virtual void						Read( YStreamLength yBytesToRead, uBYTE* pBuffer ) = 0;
		virtual void						Write( YStreamLength yulBytesToRead, const uBYTE* pBuffer ) = 0;
	};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _ARCHIVE_H_
