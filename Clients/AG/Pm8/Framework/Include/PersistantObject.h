// ****************************************************************************
//
//  File Name:			PersistantObject.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RPersistantObject class
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
//  $Logfile:: /PM8/Framework/Include/PersistantObject.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PERSISTANTOBJECT_H_
#define		_PERSISTANTOBJECT_H_

#ifndef	_DATAOBJECT_H_
#include	"DataObject.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RStorage;
class RChunkStorage;

// ****************************************************************************
//
//  Class Name:	RPersistantObject
//
//  Description:	The base Renaissance class for all objects that contain
//						data that is backstorable.
//
// ****************************************************************************
//
class FrameworkLinkage RPersistantObject : public RDataObject
	{
	// Construction & destruction
	public :
										RPersistantObject( );
		virtual						~RPersistantObject( );

	//	Operations
	public :
		void							Associate( RChunkStorage* pStorage );

		virtual void				Allocate( );
		virtual void				Deallocate( );

		virtual void				PurgeData( ) = 0;
		virtual void				Read( RStorage& storage ) = 0;
		virtual void				Write( RStorage& storage ) const = 0;

	//	Private members
	private :
		RChunkStorage*				m_pStorage;
		RChunkStorage*				m_ChunkId;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void				Validate( ) const = 0;
#endif	// TPSDEBUG
	} ;

// ****************************************************************************
// 					Inlines
// ****************************************************************************


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_PERSISTANTOBJECT_H_

