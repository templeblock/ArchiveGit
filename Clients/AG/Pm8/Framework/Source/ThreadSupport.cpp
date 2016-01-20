// ****************************************************************************
//
//  File Name:			ThreadSupport.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of various useful multithreading support.
//
//  Portability:		Win32 specific
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
//  $Logfile:: /PM8/Framework/Source/ThreadSupport.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "ThreadSupport.h"

#undef FrameworkLinkage
#define FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RLock::RLock
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RLock::RLock( RLockableObject& lockableObject )
	: m_LockableObject( lockableObject )
	{
	m_LockableObject.Lock( );
	}

// ****************************************************************************
//
//  Function Name:	RLock::~RLock
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RLock::~RLock( )
	{
	m_LockableObject.Unlock( );
	}

// ****************************************************************************
//
//  Function Name:	RCriticalSection::RCriticalSection
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCriticalSection::RCriticalSection( )
	{
	::InitializeCriticalSection( &m_CriticalSection );
	}

// ****************************************************************************
//
//  Function Name:	RCriticalSection::~RCriticalSection
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCriticalSection::~RCriticalSection( )
	{
	::DeleteCriticalSection( &m_CriticalSection );
	}

// ****************************************************************************
//
//  Function Name:	RCriticalSection::Lock
//
//  Description:		Locks
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCriticalSection::Lock( )
	{
	::EnterCriticalSection( &m_CriticalSection );
	}

// ****************************************************************************
//
//  Function Name:	RCriticalSection::Unlock
//
//  Description:		Unlock
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RCriticalSection::Unlock( )
	{
	::LeaveCriticalSection( &m_CriticalSection );
	}

// ****************************************************************************
//
//  Function Name:	RWaitableObject::RWaitableObject
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RWaitableObject::RWaitableObject( HANDLE hObject, BOOLEAN fAutoDelete )
	: m_hObject( hObject ),
	  m_fAutoDelete( fAutoDelete )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RWaitableObject::~RWaitableObject
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RWaitableObject::~RWaitableObject( )
	{
	if( m_fAutoDelete )
		::CloseHandle( m_hObject );
	}

// ****************************************************************************
//
//  Function Name:	RWaitableObject::Wait
//
//  Description:		Waits on this object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
DWORD RWaitableObject::Wait( DWORD dwMilliseconds )
	{
	return ::WaitForSingleObject( m_hObject, dwMilliseconds );
	}

// ****************************************************************************
//
//  Function Name:	RWaitableObjectList::Wait
//
//  Description:		Waits on all objects in this list
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
DWORD RWaitableObjectList::Wait( DWORD dwMilliseconds )
	{
	TpsAssert( Count( ) <= MAXIMUM_WAIT_OBJECTS, "Too many objects to wait for." );

	// Make an array of all the objects in this list
	HANDLE handles[ MAXIMUM_WAIT_OBJECTS ];

	int i = 0;
	for( YIterator iterator = Start( ); iterator != End( ); ++iterator, ++i )
		handles[ i ] = ( *iterator )->m_hObject;

	// Wait for them
	return ::WaitForMultipleObjects( Count( ), handles, TRUE, dwMilliseconds );
	}

// ****************************************************************************
//
//  Function Name:	REvent::REvent
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REvent::REvent( BOOLEAN fManualReset, BOOLEAN fInitialState )
: RWaitableObject( ::CreateEvent( NULL, fManualReset, fInitialState, NULL ) )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	REvent::Set
//
//  Description:		Sets the event
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REvent::Set( )
	{
	::SetEvent( m_hObject );
	}

// ****************************************************************************
//
//  Function Name:	REvent::Reset
//
//  Description:		Resets the event
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REvent::Reset( )
	{
	::ResetEvent( m_hObject );
	}

// ****************************************************************************
//
//  Function Name:	RMutex::RMutex
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMutex::RMutex( BOOLEAN fInitalOwner )
: RWaitableObject( ::CreateMutex( NULL, fInitalOwner, NULL ) )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMutex::Lock
//
//  Description:		Locks
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMutex::Lock( )
	{
	Wait( );
	}

// ****************************************************************************
//
//  Function Name:	RMutex::Unlock
//
//  Description:		Unlocks
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMutex::Unlock( )
	{
	Release( );
	}

// ****************************************************************************
//
//  Function Name:	RMutex::Release
//
//  Description:		Releases the mutex
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMutex::Release( )
	{
	::ReleaseMutex( m_hObject );
	}

// ****************************************************************************
//
//  Function Name:	RSemaphore::RSemaphore
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSemaphore::RSemaphore( LONG initialCount, LONG maximumCount )
	: RWaitableObject( ::CreateSemaphore( NULL, initialCount, maximumCount, NULL ) )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSemaphore::Release
//
//  Description:		Releases the semaphore
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
LONG RSemaphore::Release( LONG releaseCount )
	{
	LONG previous;
	::ReleaseSemaphore( m_hObject, releaseCount, &previous );
	return previous;
	}

// ****************************************************************************
//
//  Function Name:	RThread::RThread
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RThread::RThread( YThreadFunction threadFunction, void* pData, int nPriority )
	: RWaitableObject( NULL, FALSE )
	{
	m_pThread = ::AfxBeginThread( threadFunction, pData, nPriority );
	m_hObject = m_pThread->m_hThread;
	m_pThread->m_bAutoDelete = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RThread::~RThread
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RThread::~RThread( )
	{
	delete m_pThread;
	}

// ****************************************************************************
//
//  Function Name:	RThread::Terminate
//
//  Description:		Terminates the thread
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RThread::Terminate( YThreadResult result )
	{
	// If the thread has not already terminated, force terminate it
	if( ::WaitForSingleObject( m_hObject, 0 ) == WAIT_TIMEOUT )
		::TerminateThread( m_hObject, result );
	}
