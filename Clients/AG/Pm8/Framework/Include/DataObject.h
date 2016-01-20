// ****************************************************************************
//
//  File Name:			DataObject.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RDataObject class
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
//  $Logfile:: /PM8/Framework/Include/DataObject.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DATAOBJECT_H_
#define		_DATAOBJECT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef	uLONG			YAccessCount;

// ****************************************************************************
//
//  Class Name:	RDataObject
//
//  Description:	The base Renaissance class for all objects that contain
//						data that may be purgable or backstorable.
//
//						RDataObject will register itself with the Standalone Application
//						to be called when memory is needed.  Evertime the Verify function
//						is called the last accessed field will be modified.
//
// ****************************************************************************
//
class FrameworkLinkage RDataObject : public RObject
	{
	// Construction & destruction
	public :
										RDataObject( );
		virtual						~RDataObject( );
		virtual void				Initialize( );

	//	Operations
	public :
		YAccessCount				LastAccessed( ) const;
		BOOLEAN						IsLocked( ) const;
		BOOLEAN						IsInMemory( ) const;
		void							Lock( );
		void							Unlock( );

		virtual void				Allocate( ) = 0;
		virtual void				Deallocate( ) = 0;

	//	Private members
	private :
		static YAccessCount		m_gAccessCounter;

		YAccessCount				m_LastAccessed;
		BOOLEAN						m_fInMemory;
		YCounter						m_LockCount;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const = 0;
#endif	// TPSDEBUG
	} ;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RDataObject::IsLocked( )
//
//  Description:		Return whether the object is locked.
//
//  Returns:			m_LockCount>0;
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RDataObject::IsLocked( ) const
	{
	return static_cast<BOOLEAN>( m_LockCount > 0 );
	}

// ****************************************************************************
//
//  Function Name:	RDataObject::IsInMemory( )
//
//  Description:		Return whether the object is in memory or not in memory.
//
//  Returns:			IsInMemory
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RDataObject::IsInMemory( ) const
	{
	return m_fInMemory;
	}
// ****************************************************************************
//
//  Function Name:	RDataObject::Initialize( )
//
//  Description:		Initialize the data object by calling the Verify method
//
//  Returns:			Nothing
//
//  Exceptions:		memory or file
//
// ****************************************************************************
//
inline void RDataObject::Initialize( )
	{
	Lock( );
	Unlock( );
	}

// ****************************************************************************
//
//  Function Name:	RDataObject::LastAccessed( )
//
//  Description:		Return the counter for the last accessed time.
//
//  Returns:			m_LastAccessed;
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline YAccessCount RDataObject::LastAccessed( ) const
	{
	return m_LastAccessed;
	}

// ****************************************************************************
//
//				Support Class
//
// ****************************************************************************
//
class RDataObjectLocker
	{
	//	Constructor & Destructor
	public :
													RDataObjectLocker( RDataObject& object );
													RDataObjectLocker( RDataObject* pObject );
													~RDataObjectLocker( );
	//	Operations
	public :
		void										Release( );

	//	Instance Members
	private :
		BOOLEAN									m_fLocked;
		RDataObject*							m_pObject;
	};

// ****************************************************************************
//
//  Function Name:	RDataObjectLocker::RDataObjectLocker( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Any Exception
//
// ****************************************************************************
//
inline RDataObjectLocker::RDataObjectLocker( RDataObject& object )
	: m_pObject( &object ),
	  m_fLocked( FALSE )
	{
	m_pObject->Lock();
	m_fLocked = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDataObjectLocker::RDataObjectLocker( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Any Exception
//
// ****************************************************************************
//
inline RDataObjectLocker::RDataObjectLocker( RDataObject* pObject )
	: m_pObject( pObject ),
	  m_fLocked( FALSE )
	{
	m_pObject->Lock();
	m_fLocked = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDataObjectLocker::~RDataObjectLocker( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RDataObjectLocker::~RDataObjectLocker( )
	{
	if (m_fLocked)
		m_pObject->Unlock();
	}

// ****************************************************************************
//
//  Function Name:	RDataObjectLocker::Release( )
//
//  Description:		Release control of the locked object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void RDataObjectLocker::Release( )
	{
	//	setting locked to false will cause the unlock in the destructor to do nothing
	m_fLocked = FALSE;
	}


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_DATAOBJECT_H_

