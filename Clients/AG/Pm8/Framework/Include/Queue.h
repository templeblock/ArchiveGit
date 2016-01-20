// ****************************************************************************
//
//  File Name:			Queue.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of RQueue
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
//  $Logfile:: /PM8/Framework/Include/Queue.h                             $
//   $Author:: Gbeddow                            $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_QUEUE_H_
#define	_QUEUE_H_

#include "ThreadSupport.h"

// ****************************************************************************
//
//  Class Name:	RQueue
//
//  Description:	Class to encapsulate a multithreaded queue
//
// ****************************************************************************
//
template <class T> class RQueue
	{
	// Types and constants
	private :
		typedef RList<T*> YJobCollection;
		typedef YJobCollection::YIterator YJobIterator;

	// Construction
	public :
													RQueue( );
													~RQueue( );

	// Operations
	public :
		void										SubmitJob( T* pJob );
		BOOLEAN									CanShutdown( ) const;
		void										DeleteAllJobs( );

	// Protected implementation
	protected :
		void										StartThreads( int nThreads = 1 );
		T*											GetNextJob( );
		void										ReleaseJob( T* pJob );

	// Private implementation
	private :
		static YThreadResult					ThreadFunction( void* pQueue );
		void										DeleteJobsInQueue( YJobCollection& queue );

	// Overridden implementation
	protected :
		virtual void							DeleteJob( T* pJob ) = 0;
		virtual void							ProcessJob( T* pJob ) = 0;
		virtual BOOLEAN						ShouldContinue( ) = 0;
		virtual void							Cancel( ) = 0;

	// Members
	private :
		RCriticalSection						m_CriticalSection;
		RSemaphore								m_JobAvailableSemaphore;
		YJobCollection							m_JobsToBeProcessed;
		YJobCollection							m_JobsProcessing;
		RWaitableObjectList					m_ThreadList;
	};

// Constants
const DWORD kWaitForThreadsTimeout = 10000;
const YThreadResult kNormalTerminate = 1000;
const YThreadResult kNormalCancelTerminate = 1001;
const YThreadResult kForcedCancelTerminate = 1002;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RQueue::RQueue
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RQueue<T>::RQueue( )
	: m_JobAvailableSemaphore( 0, LONG_MAX )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RQueue::StartThreads
//
//  Description:		Starts the queue threads. This function must be called in
//							the derived classes construtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RQueue<T>::StartThreads( int nThreads )
	{
	for( int i = 0; i < nThreads; ++i )
		m_ThreadList.InsertAtEnd( new RThread( ThreadFunction, this, THREAD_PRIORITY_NORMAL ) );
	}

// ****************************************************************************
//
//  Function Name:	RQueue::~RQueue
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> RQueue<T>::~RQueue( )
	{
	TpsAssert( m_JobsToBeProcessed.Count( ) == 0, "Deleting queue with jobs remaining to be processed." );
	TpsAssert( m_JobsProcessing.Count( ) == 0, "Deleting queue with jobs processed." );

	// Delete the thread objects
	DeleteAllItems( m_ThreadList );
	}

// ****************************************************************************
//
//  Function Name:	RQueue::SubmitJob
//
//  Description:		Adds a job to the queue. The queue adopts ownership of
//							the job.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RQueue<T>::SubmitJob( T* pJob )
	{
	// Lock this section of code with a critical section
	RLock lock( m_CriticalSection );

	// Add the job to the end of the queue
	m_JobsToBeProcessed.InsertAtEnd( pJob );

	// Release a thread
	m_JobAvailableSemaphore.Release( 1 );
	}

// ****************************************************************************
//
//  Function Name:	RQueue::GetNextJob
//
//  Description:		Gets the next job from the queue. This call blocks until
//							a job is available.
//
//  Returns:			The next job to be processed. The caller adopts the job.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> T* RQueue<T>::GetNextJob( )
	{
	// Wait until there is a job
	m_JobAvailableSemaphore.Wait( );

	// Lock this section of code with a critical section
	RLock lock( m_CriticalSection );

	// If we have been canceled, return a NULL job
	if( ShouldContinue( ) == FALSE )
		return NULL;

	// Get the job at the head of the queue
	TpsAssert( m_JobsToBeProcessed.Count( ) > 0, "No jobs to be processed" );
	T* pJob = *m_JobsToBeProcessed.Start( );

	// Remove the job from the queue
	m_JobsToBeProcessed.Remove( pJob );

	// Add the job to the processing queue
	m_JobsProcessing.InsertAtEnd( pJob );

	return pJob;
	}

// ****************************************************************************
//
//  Function Name:	RQueue::ReleaseJob
//
//  Description:		Marks a job for deletion
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RQueue<T>::ReleaseJob( T* pJob )
	{
	// Lock this section of code with a critical section
	RLock lock( m_CriticalSection );

	// Remove the job from the processing queue
	m_JobsProcessing.Remove( pJob );

	// delete the job
	DeleteJob( pJob );
	}

// ****************************************************************************
//
//  Function Name:	RQueue::DeleteAllJobs
//
//  Description:		Deletes all jobs in all queues
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RQueue<T>::DeleteAllJobs( )
	{
	RWaitCursor waitCursor;

	// Signal that we are canceling
	Cancel( );

	// Release any threads waiting for jobs. They will get NULL jobs
	for( int i = 0; i < m_ThreadList.Count( ); ++i )
		m_JobAvailableSemaphore.Release( 1 );

	// Delete any jobs that have not yet started
	m_CriticalSection.Lock( );
	DeleteJobsInQueue( m_JobsToBeProcessed );
	m_CriticalSection.Unlock( );

	// Wait for the threads to terminate
	m_ThreadList.Wait( kWaitForThreadsTimeout );

	// If any threads didnt terminate, force terminate them
	RWaitableObjectList::YIterator iterator = m_ThreadList.Start( );
	for( ; iterator != m_ThreadList.End( ); ++iterator )
		static_cast<RThread*>( *iterator )->Terminate( kForcedCancelTerminate );

	// Delete the jobs that were processing
	DeleteJobsInQueue( m_JobsProcessing );
	}

// ****************************************************************************
//
//  Function Name:	RQueue::DeleteJobsInQueue
//
//  Description:		Deletes all items in the specified queue
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> void RQueue<T>::DeleteJobsInQueue( YJobCollection& queue )
	{
	// Iterate the jobs deleting them
	YJobIterator iterator = queue.Start( );
	for( ; iterator != queue.End( ); ++iterator )
		DeleteJob( *iterator );

	// Empty the queue
	queue.Empty( );
	}

// ****************************************************************************
//
//  Function Name:	RQueue::CanShutdown
//
//  Description:		Determines if the queue wants to shutdown
//
//  Returns:			TRUE if there are no jobs processing or remaining to be 
//							processed.
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> BOOLEAN RQueue<T>::CanShutdown( ) const
	{
	return ( m_JobsToBeProcessed.Count( ) == 0 && m_JobsProcessing.Count( ) == 0 );
	}

// ****************************************************************************
//
//  Function Name:	RQueue::PrintThreadFunction
//
//  Description:		Print thread
//
//  Returns:			Result code
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class T> YThreadResult RQueue<T>::ThreadFunction( void* pData )
	{
	// Get a pointer to the print queue
	RQueue<T>* pQueue = static_cast<RQueue<T>*>( pData );

	// Enter the processing loop
	while( pQueue->ShouldContinue( ) )
		{
		// Get the next job
		T* pJob = pQueue->GetNextJob( );

		// If the job is NULL, we have been canceled. Return.
		if( !pJob )
			return kNormalCancelTerminate;

		// Process it
		pQueue->ProcessJob( pJob );

		// Release the job
		pQueue->ReleaseJob( pJob );
		}

	return kNormalTerminate;
	}

#endif	// _QUEUE_H_
