// ****************************************************************************
//
//  File Name:			Cache.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RCache template
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
//  $Logfile:: /PM8/Framework/Include/Cache.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CACHE_H_
#define		_CACHE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RCache
//
//  Description:	This is the template class for any object that wants
//						to be cached for performance reasons. The TCacheData is the
//						data that is needed to create the object to be cached.
//						TCacheObject is the object created to be cached and returned
//						to the caller.
//
//		NOTE:			The following assumptions are made.
//							1) TCacheObject can be created when passed a TCacheData&
//							3) The equality operators are implemented to return TRUE
//								when two TCacheDatas or TCacheObjects are the same.
//							4) TCacheObject destructor will clean up after itself.
//
// ****************************************************************************
//
template <class TCacheObject, class TCacheData> class RCache
	{
	//	created classes
	private :
		class RCacheData
			{
				// Constructor, Destructor
				public :
												RCacheData( const TCacheData& data )
													: m_Object( data ),
													  m_Data( data ),
													  m_InUseCount( FALSE )
													{
													;
													}
												~RCacheData( )
													{
													;
													}

				//	Operations
				public :
					BOOLEAN					operator == ( const TCacheObject& object )
													{
													return (m_Object == object);
													}
					BOOLEAN					operator == ( const TCacheData& data )
													{
													return static_cast<BOOLEAN>( (m_Data == data) );
													}

				//	Members:
				public :
					TCacheObject			m_Object;
					TCacheData				m_Data;
					YCounter					m_InUseCount;
			};
		typedef	RList<RCacheData*>	RCacheDataList;

	//	Constructor, Destructor, Initialization
	public :
		RCache( YCounter maxInMemory );
		~RCache( );

	//	Operations
	public :
		TCacheObject&						GetObject( const TCacheData& data );
		void									ReleaseObject( const TCacheObject& object );
		void									UseObject( const TCacheObject& object );

	//	Members
	private :
		YCounter								m_MaxInMemory;
		RCacheDataList						m_CacheList;
		RCriticalSection					m_CacheCriticalSection;
	} ;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RCache::RCache( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TCacheObject, class TCacheData> inline
RCache<TCacheObject,TCacheData>::RCache( YCounter maxInMemory ) : m_MaxInMemory( maxInMemory )
	{
	TpsAssert( m_MaxInMemory > 0, "Donot create a cache with less than 1 object in memory" );
	TpsAssert( m_MaxInMemory < 200, "Large cache, is a cache the best choice for this usage" );
	}

// ****************************************************************************
//
//  Function Name:	RCache::~RCache( )
//
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TCacheObject, class TCacheData> inline
RCache<TCacheObject,TCacheData>::~RCache( )
	{
	RCacheDataList::YIterator	iterator;
	while ( (iterator = m_CacheList.Start()) != m_CacheList.End() )
		{
		RCacheData*	pCacheData = *iterator;
		m_CacheList.RemoveAt(iterator);
		delete pCacheData;
		}
	}

// ****************************************************************************
//
//  Function Name:	RCache::GetObject( )
//
//  Description:		Return a reference to the object given the data
//
//  Returns:			TCacheObject&
//
//  Exceptions:		Any exception that could be thrown by a TCacheObject constructor
//							Memory exception
//
// ****************************************************************************
//
template <class TCacheObject, class TCacheData> inline
TCacheObject& RCache<TCacheObject,TCacheData>::GetObject( const TCacheData& data )
	{
	RLock lock( m_CacheCriticalSection );

	RCacheData*						pCacheData		= NULL;
	RCacheDataList::YIterator	startIterator	= m_CacheList.Start();
	RCacheDataList::YIterator	endIterator		= m_CacheList.End();
	RCacheDataList::YIterator	iterator;

	//	First search if the object is already created and remove it from its current
	//		position (it will be reinserted at the head later)
	for ( iterator = startIterator; iterator != endIterator; ++iterator )
		{
		pCacheData = *iterator;
		if ( pCacheData->m_Data == data )
			{
			//	If the iterator is not the start, I have to move it so CopyItems
			if (iterator != startIterator)
				{
				RCacheDataList::YIterator	dstIterator	= iterator + 1;
				CopyItemsBackwards( iterator, dstIterator, startIterator );
				*startIterator	= pCacheData;
				}
			break;
			}
		}

	//	If iterator is at end, nothing was found so we must create a new one
	if (iterator == endIterator)
		{
		//	First check if we must free up a spot in the list
		if (m_CacheList.Count() == m_MaxInMemory)
			{
			//	we must delete an unused entry, seach backwards for an entry with a
			//		InUse count of 0 and remove it.
			iterator = m_CacheList.End();
			while (iterator != m_CacheList.Start())
				{
				--iterator;
				if ((*iterator)->m_InUseCount == 0)
					break;
				}
			TpsAssert( ( iterator != m_CacheList.Start()) || (*iterator)->m_InUseCount==0,
							"All entries in Cache are in use, If this is a possiblity, increase the cache size" );
			TpsAssert( (*iterator)->m_InUseCount == 0, "The InUseCount of the cache object to be deleted is NOT zero" );
			pCacheData = (*iterator);
			m_CacheList.RemoveAt(iterator);
			delete pCacheData;
			pCacheData = NULL;
			}
			
		//	Create the new CacheData object and insert it at the front of the list
		pCacheData = new RCacheData( data );
		m_CacheList.InsertAtStart( pCacheData );
		}

	//	return a reference to the object (after incrementing the inuse counter)
	++pCacheData->m_InUseCount;
	//TpsAssert( pCacheData->m_InUseCount < 10, "The in use count of this object greater than 10" );
	return pCacheData->m_Object;
	}

// ****************************************************************************
//
//  Function Name:	RCache::ReleaseObject( )
//
//  Description:		Release a reference to the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TCacheObject, class TCacheData> inline
void RCache<TCacheObject,TCacheData>::ReleaseObject( const TCacheObject& object )
	{
	RLock lock( m_CacheCriticalSection );

	RCacheData*						pCacheData;
	RCacheDataList::YIterator	iterator;

	//	Search for the item in the list...
	for ( iterator = m_CacheList.Start(); iterator != m_CacheList.End(); ++iterator )
		{
		pCacheData = *iterator;
		if ( pCacheData->m_Object == object )
			break;
		}

	if (iterator != m_CacheList.End())
		{
		--(pCacheData->m_InUseCount);
		TpsAssert( pCacheData->m_InUseCount >= 0, "In use count of the requested object is less than 0" );
		}
	}

// ****************************************************************************
//
//  Function Name:	RCache::UseObject( )
//
//  Description:		Increment the use count of an object if it exists in the cache
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template <class TCacheObject, class TCacheData> inline
void RCache<TCacheObject,TCacheData>::UseObject( const TCacheObject& object )
	{
	RLock lock( m_CacheCriticalSection );

	RCacheData*						pCacheData;
	RCacheDataList::YIterator	iterator;

	//	Search for the item in the list...
	for ( iterator = m_CacheList.Start(); iterator != m_CacheList.End(); ++iterator )
		{
		pCacheData = *iterator;
		if ( pCacheData->m_Object == object )
			break;
		}

	if (iterator != m_CacheList.End())
		{
		++(pCacheData->m_InUseCount);
		//TpsAssert( pCacheData->m_InUseCount < 10, "The in use count of this object greater than 10" );
		}
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_CACHE_H_

