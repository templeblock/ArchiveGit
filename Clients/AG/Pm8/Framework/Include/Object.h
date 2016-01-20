// ****************************************************************************
//
//  File Name:			Object.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Declaration of the RObject class
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
//  $Logfile:: /PM8/Framework/Include/Object.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_OBJECT_H_
#define		_OBJECT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

const uLONG kDebugTag = 0x4D485341;

// ****************************************************************************
//
//  Class Name:	RObject
//
//  Description:	The base Renaissance class. All other classes should be
//						derived from this class.
//
//						Currently RObject does nothing in particular. It exists so
//						that if global functionality needs to be added, it can be
//						done in a central location.
//
// ****************************************************************************
//
class FrameworkLinkage RObject
	{
	// Construction & destruction
	public :
												RObject( );
		virtual								~RObject( );

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const = 0;

	private :
		const uLONG							m_DebugTag;
#endif	// TPSDEBUG
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RObject::RObject( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RObject::RObject( )
	Debug( : m_DebugTag( kDebugTag ) )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RObject::~RObject( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RObject::~RObject( )
	{
	;
	}

//	TPS Debug macros for renaissance
#ifdef	TPSDEBUG
	inline	void TpsAssertValid( const RObject* pObj ) { pObj->Validate( ); }
	#define	TpsAssertIsObject( rclass, obj )	TpsAssert( dynamic_cast<rclass*> ((rclass*)( obj ) ), "The given object is not of the requested type" )
#else
	#define	TpsAssertValid( pObj )				NULL
	#define	TpsAssertIsObject( rclass, obj )	NULL
#endif	// TPSDEBUG

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _OBJECT_H_
