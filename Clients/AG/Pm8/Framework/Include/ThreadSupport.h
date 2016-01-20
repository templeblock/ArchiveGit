// ****************************************************************************
//
//  File Name:			ThreadSupport.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of various useful multithreading support.
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
//  $Logfile:: /PM8/Framework/Include/ThreadSupport.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_THREADSUPPORT_H_
#define		_THREADSUPPORT_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef unsigned int YThreadResult;
typedef YThreadResult (*YThreadFunction)( void* pThreadParameter );

// ****************************************************************************
//
//  Class Name:	RLockableObject
//
//  Description:	Abstract base class for lockable objects
//
// ****************************************************************************
//
class FrameworkLinkage RLockableObject
	{
	// Operations
	public :
		virtual void						Lock( ) = 0;
		virtual void						Unlock( ) = 0;
	};

// ****************************************************************************
//
//  Class Name:	RWaitableObject
//
//  Description:	Abstract base class for objects that can be waited on
//
// ****************************************************************************
//
class FrameworkLinkage RWaitableObject
	{
	// Construction & Destruction
	public :
												RWaitableObject( HANDLE hObject, BOOLEAN fAutoDelete = TRUE );
			virtual							~RWaitableObject( );

	// Operations
	public :
		DWORD									Wait( DWORD dwMilliseconds = INFINITE );

	// Members
	protected :
		HANDLE								m_hObject;
		BOOLEAN								m_fAutoDelete;

	friend class RWaitableObjectList;
	};

// ****************************************************************************
//
//  Class Name:	RWaitableObjectList
//
//  Description:	List of objects to wait on
//
// ****************************************************************************
//
class FrameworkLinkage RWaitableObjectList : public RList<RWaitableObject*>
	{
	// Operations
	public :
		DWORD									Wait( DWORD dwMilliseconds = INFINITE );
	};

// ****************************************************************************
//
//  Class Name:	RLock
//
//  Description:	Class to lock and unlock a lockable object
//
// ****************************************************************************
//
class FrameworkLinkage RLock
	{
	// Construction & Destruction
	public :
												RLock( RLockableObject& lockableObject );
												~RLock( );

	// Members
	private :
		RLockableObject&					m_LockableObject;
	};

// ****************************************************************************
//
//  Class Name:	RCriticalSection
//
//  Description:	Critical section implemented as a lockable object
//
// ****************************************************************************
//
class FrameworkLinkage RCriticalSection : public RLockableObject
	{
	// Construction & Destruction
	public :
												RCriticalSection( );
												~RCriticalSection( );

	// Operations
	public :
		virtual void						Lock( );
		virtual void						Unlock( );

	// Members
	private :
		CRITICAL_SECTION					m_CriticalSection;
	};

// ****************************************************************************
//
//  Class Name:	REvent
//
//  Description:	Waitable event object
//
// ****************************************************************************
//
class FrameworkLinkage REvent : public RWaitableObject
	{
	// Construction
	public :
												REvent( BOOLEAN fManualReset, BOOLEAN fInitialState );

	// Operations
	public :
		void									Set( );
		void									Reset( );
	};

// ****************************************************************************
//
//  Class Name:	RMutex
//
//  Description:	Mutex
//
// ****************************************************************************
//
class FrameworkLinkage RMutex : public RWaitableObject, public RLockableObject
	{
	// Construction
	public :
												RMutex( BOOLEAN fInitalOwner );

	// Operations
	public :
		virtual void						Lock( );
		virtual void						Unlock( );
		void									Release( );
	};

// ****************************************************************************
//
//  Class Name:	RSemaphore
//
//  Description:	Semaphore
//
// ****************************************************************************
//
class FrameworkLinkage RSemaphore : public RWaitableObject
	{
	// Construction
	public :
												RSemaphore( LONG initialCount, LONG maximumCount );

	// Operations
	public :
		LONG									Release( LONG releaseCount );
	};

// ****************************************************************************
//
//  Class Name:	RThread
//
//  Description:	Thread
//
// ****************************************************************************
//
class FrameworkLinkage RThread : public RWaitableObject
	{
	// Construction
	public :
												RThread( YThreadFunction threadFunction, void* pData, int nPriority = THREAD_PRIORITY_NORMAL );
			virtual							~RThread( );

	// Operations
	public :
		void									Terminate( YThreadResult result );

	// Members
	private :
		CWinThread*							m_pThread;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	::CreateThread( )
//
//  Description:		Creates a thread					
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline void CreateThread( YThreadFunction threadFunction, void* pData, int nPriority = THREAD_PRIORITY_NORMAL )
	{
	::AfxBeginThread( threadFunction, pData, nPriority );
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _THREADSUPPORT_H_
